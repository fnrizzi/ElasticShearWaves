
#ifndef MATERIAL_MODEL_PREM_HPP_
#define MATERIAL_MODEL_PREM_HPP_

#include "material_model_base.hpp"

template<typename scalar_t, typename parser_t>
class PremMaterialModel final : public MaterialModelBase<scalar_t>
{
  using profile_params_t = typename parser_t::profile_params_t;

  const parser_t & parser_;

public:
  PremMaterialModel(const parser_t & parser) : parser_(parser){}

  void computeAt(const scalar_t & radiusFromEarthCenterMeters,
		 const scalar_t & angleRadians,
		 scalar_t & rho,
		 scalar_t & vs) const final
  {
    constexpr auto thousand  = constants<scalar_t>::thousand();
    constexpr auto esrKm     = constants<scalar_t>::earthSurfaceRadiusKm();
    constexpr auto esrMeters = constants<scalar_t>::earthSurfaceRadiusMeters();

    // https://www.cfa.harvard.edu/~lzeng/papers/PREM.pdf

    // If you use the Preliminary reference Earth model (PREM)
    // for your own research, please refer to
    // Dziewonski, A.M., and D.L. Anderson. 1981. “Preliminary reference Earth model.” Phys. Earth Plan. Int. 25:297-356.
    // IRIS DMC (2011), Data Services Products: EMC, A repository of Earth models, https://doi.org/10.17611/DP/EMC.1.

    // in your publication.

    const auto rKm = radiusFromEarthCenterMeters/thousand;
    const auto x   = rKm/esrKm;
    const auto xSq = x*x;
    const auto xCu = x*x*x;

    if(rKm >= 6356.0){
      // crust
      rho = 2.6;
      vs = 3.2;
    }
    else if(rKm >= 6346.6 and rKm < 6356.0){
      // crust
      rho = 2.9;
      vs = 3.9;
    }

    else if(rKm >= 6291.0 and rKm < 6346.6){
      rho = 2.691  + 0.6924*x;
      vs  = 2.1519 + 2.3481*x;
    }

    else if(rKm >= 6151.0 and rKm < 6291.0){
      rho = 2.691  + 0.6924*x;
      vs  = 2.1519 + 2.3481*x;
    }

    else if(rKm >= 5971.0 and rKm < 6151.0){
      // transition zone
      rho = 7.1089 - 3.8045*x;
      vs  = 8.9496 - 4.4597*x;
    }

    else if(rKm >= 5771.0 and rKm < 5971.0){
      // transition zone
      rho = 11.2494 - 8.0298*x;
      vs  = 22.3512 - 18.5856*x;
    }

    else if(rKm >= 5701.0 and rKm < 5771.0){
      // transition zone
      rho = 5.3197 - 1.4836*x;
      vs  = 9.9839 - 4.9324*x;
    }

    else if(rKm >= 5600.0 and rKm < 5701.0){
      // lower mantle part 3
      rho = 7.9565 - 6.4761*x  + 5.5283*xSq - 3.0807*xCu;
      vs = 22.3459 - 17.2473*x - 2.0834*xSq + 0.9783*xCu;
    }

    else if(rKm >= 3630.0 and rKm < 5600.0){
      // lower mantle part 2
      rho = 7.9565 - 6.4761*x  + 5.5283*xSq  - 3.0807*xCu;
      vs = 11.1671 - 13.7818*x + 17.4575*xSq - 9.2777*xCu;
    }

    else if(rKm >= 3480.0 and rKm < 3630.0){
      // lower mantle part 1
      rho = 7.9565 - 6.4761*x + 5.5283*xSq - 3.0807*xCu;
      vs  = 6.9254 + 1.4672*x - 2.0834*xSq + 0.9783*xCu;
    }

    else if(rKm >= 1221.5 and rKm < 3480.0){
      // outer core
      rho = 12.5815 - 1.2638*x - 3.6426*xSq - 5.5281*xCu;
      vs = 0.0;
    }
    else if(rKm < 1221.5){
      // inner core
      rho = 13.0885 - 8.8381*xSq;
      vs  = 3.6678  - 4.4475*xSq;
    }

    // convert vs from m/s to km/s
    vs *= thousand;
    // convert rho from g/cm^3 to km/m^3
    rho *= thousand;
  }
};

#endif
