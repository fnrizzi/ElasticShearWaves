
#ifndef ROM_PROBLEM_RANK_TWO_FORCING_HPP_
#define ROM_PROBLEM_RANK_TWO_FORCING_HPP_

#include "../../shared/all.hpp"
#include "../shwavepp.hpp"
#include "../common_types.hpp"
#include "compute_rom_jacobians.hpp"
#include "load_basis.hpp"
#include "run_rom_rank_two_forcing.hpp"
#include "rom_problem_base.hpp"

namespace kokkosapp{

class RomProblemRankTwoForcing final
  : public RomProblemBase, kokkosapp::commonTypes
{
  using kokkosapp::commonTypes::scalar_t;
  using kokkosapp::commonTypes::sc_t;
  using kokkosapp::commonTypes::int_t;
  using kokkosapp::commonTypes::parser_t;
  using kokkosapp::commonTypes::klr;
  using kokkosapp::commonTypes::kll;
  using kokkosapp::commonTypes::exe_space;

  static constexpr bool usingFullMesh = true;
  using mesh_info_t	= MeshInfo<sc_t, int_t, usingFullMesh>;

  // here the state is a rank-2 view so layout matters
  using state_d_t	= Kokkos::View<sc_t**, kll, exe_space>;
  using state_h_t	= typename state_d_t::host_mirror_type;
  using jacobian_d_type = KokkosSparse::CrsMatrix<sc_t, int_t, exe_space>;
  using fom_t           = ShWavePP<sc_t, int_t, mesh_info_t, jacobian_d_type, exe_space>;

  // here the basis is a rank-2 view and layout matters for binary IO
  // but not for performance, the basis are only need to compute reduced jacobians
  // leave it to ll
  using basis_d_t      = Kokkos::View<sc_t**, kll, exe_space>;
  using basis_h_t      = typename basis_d_t::host_mirror_type;

  // here the rom_jac is a rank-2 view and layout matters for performance
  // should pick layout to optimize gemm for every time step
  using rom_jac_d_t    = Kokkos::View<sc_t**, kll, exe_space>;
  using rom_jac_h_t    = typename rom_jac_d_t::host_mirror_type;

  using obs_t		= Observer<int_t, sc_t, state_d_t>;
  using forcing_t       = RankTwoForcing<sc_t, state_d_t, int_t>;

private:
  const parser_t & parser_;
  const int_t fSize_;
  const mesh_info_t meshInfo_;
  const int_t nVpFom_;
  const int_t nSpFom_;
  const int_t nVp_;
  const int_t nSp_;

  state_d_t xVp_d_;
  state_d_t xSp_d_;
  basis_d_t phiVp_d_;
  basis_d_t phiSp_d_;
  rom_jac_d_t Jvp_d_;
  rom_jac_d_t Jsp_d_;
  obs_t observerObj_;

public:
  RomProblemRankTwoForcing(const parser_t & parser)
    : parser_(parser),
      fSize_(parser.getForcingSize()),
      meshInfo_(parser.getMeshDir()),
      nVpFom_(meshInfo_.getNumVpPts()),
      nSpFom_(meshInfo_.getNumSpPts()),
      nVp_(parser.getRomSize(dofId::vp)),
      nSp_(parser.getRomSize(dofId::sp)),
      xVp_d_("xVp_d", nVp_, fSize_),
      xSp_d_("xSp_d", nSp_, fSize_),
      phiVp_d_("phiVp_d", nVpFom_, nVp_),
      phiSp_d_("phiSp_d", nSpFom_, nSp_),
      Jvp_d_("romJvp_d", nVp_, nSp_),
      Jsp_d_("romJsp_d", nSp_, nVp_),
      observerObj_(nVp_, nSp_, parser, fSize_)
  {
    loadBasis<int_t, sc_t, basis_h_t>(parser_, phiVp_d_, phiSp_d_);
  }

public:
  void execute() final
  {
    multiRunSamplingForcingPeriod();
  }

private:
  void multiRunSamplingForcingPeriod()
  {
    // compute romJacs
    auto matObj = createMaterialModel<sc_t>(parser_);
    fom_t fomObj(meshInfo_);
    fomObj.computeJacobiansWithMatProp(*matObj);
    computeRomOperatorsUsingFomJacsWithMatProp(parser_, fomObj, phiVp_d_, phiSp_d_, Jvp_d_, Jsp_d_);

    // *** create vector of signals ***
    const auto periods = parser_.getValues(0);
    std::vector<Signal<sc_t>> signals;
    for (auto i=0; i<periods.size(); ++i)
    {
      // use the signal that was set from input file
      // so everything remains the same except for the
      signals.emplace_back(parser_.getSignal());
      signals.back().resetPeriod(periods[i]);
    }

    // forcing object
    forcing_t forcing(parser_, meshInfo_, fomObj,
		      parser_.getSourceProperty("depth"),
		      parser_.getSourceProperty("angle"), true);
    // index of where source acts
    const auto fIndex   = forcing.getVpGid();

    // view target row of phiVp
    auto phiVpRow_d = Kokkos::subview(phiVp_d_, fIndex, Kokkos::ALL());

    // create state-like object that pre-multiplies the forcing
    state_d_t phiVpRhoInv_d("phiVpRhoInv_d", nVp_, fSize_);
    for (auto j=0; j<fSize_; ++j){
      auto col = Kokkos::subview(phiVpRhoInv_d, Kokkos::ALL(), j);
      Kokkos::deep_copy(col, phiVpRow_d);
    }

    // extract the target value from rhoInv
    const auto rhoInvVpValue = fomObj.viewInvDensityHost(dofId::vp)(fIndex);
    // scale using the rho
    KokkosBlas::scal(phiVpRhoInv_d, rhoInvVpValue, phiVpRhoInv_d);

    const std::size_t numSets = signals.size()/fSize_;
    for (std::size_t i=0; i<numSets; ++i)
    {
      // each loop iteration handles fSize_ signals
      // so replace signals starting from i*fSize_
      forcing.replaceSignals(signals, i*fSize_);

      // need to recheck that the new signal still meets conditions
      doChecks(forcing, fomObj);

      observerObj_.prepForNewRun(i);

      kokkosapp::runRomRankTwoForcing(parser_.getNumSteps(), parser_.getTimeStepSize(),
				      phiVpRhoInv_d, forcing, observerObj_,
				      Jvp_d_, Jsp_d_, xVp_d_, xSp_d_);
      processCollectedData();
    }
  }

  template <typename forcing_t, typename fom_t>
  void doChecks(const forcing_t & forcing, fom_t & fomObj){
    if (parser_.checkDispersion())
      checkDispersionCriterion(meshInfo_, forcing.getMaxFreq(),
    			       fomObj.getMinShearWaveVelocity());

    if (parser_.checkCfl())
      checkCfl(meshInfo_, parser_.getTimeStepSize(), fomObj.getMaxShearWaveVelocity());
  }

  void processCollectedData()
  {
    // only process final/stored data if dummy basis = false
    if (parser_.enableRandomDummyBasis() == false)
    {

      const auto startTime  = std::chrono::high_resolution_clock::now();
      if(parser_.enableSnapshotMatrix()){
	observerObj_.writeSnapshotMatrixToFile(dofId::vp);
	observerObj_.writeSnapshotMatrixToFile(dofId::sp);
      }
      const auto finishTime = std::chrono::high_resolution_clock::now();
      const std::chrono::duration<double> elapsed = finishTime - startTime;
      std::cout << "\nfinalProcessTime = "
		<< std::fixed << std::setprecision(10) << elapsed.count();
      std::cout << "\n";
    }
  }
};

}//end namespace
#endif
