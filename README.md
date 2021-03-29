
# Scope

This repository contains code for simulating elastic seismic shear waves in an axisymmetric domain.
The implementation uses Kokkos and Kokkos-kernels to enable performance portability,
and provides capabilities to run both the full order model (FOM)
as well as the Galerkin reduced order model (ROM).

Specifically, this code has been developed mainly for [this paper](https://arxiv.org/abs/2009.11742) (currently under review).

# Building
To build the code, you can use one of the following two alternatives: 

- If you are an "expert" and want to use CMake directly, 
you can refer to [this page](./docs/build_expert.md).

- If you prefer a more guided procedure where we show you step-by-step 
how to install the TPLs, and provide you a way to build the code 
using helper bash scripts that we prepared, you can  
follow this [step-by-step guide](./docs/build.md).

# Step-by-step for running a full-order model simulation
Follow [this guide](./docs/run_fom.md) for an example to run a FOM run.

# Step-by-step for a rank-1 Galerkin ROM simulation
Follow [this guide](./docs/run_rom.md) for an example to run a rank-1 Galerkin run.

# Step-by-step for a rank-2 Galerkin ROM simulation
Coming soon.

# High-level content
- [C++ source code](./cpp/src)
- [C++ tests](./cpp/tests)
- [meshing](./meshing)
- [Python scripts for processing and workflows](./python_scripts)

# C++ code references in detail
- FOM
  - [main file](./cpp/src/kokkos/main_fom.cc)
  - [rank-1 problem class](./cpp/src/kokkos/fom/fom_problem_rank_one_forcing.hpp)
  - [rank-2 problem class](./cpp/src/kokkos/fom/fom_problem_rank_two_forcing.hpp)
  - [kernel call for velocity](./cpp/src/kokkos/fom/fom_velocity_update.hpp)
  - [kernel call for stresses](./cpp/src/kokkos/fom/fom_stress_update.hpp)
  - [time loop driver](./cpp/src/kokkos/fom/run_fom.hpp)

- ROM 
  - [main file](./cpp/src/kokkos/main_rom.cc)
  - [rank-1 problem class](./cpp/src/kokkos/rom/rom_problem_rank_one_forcing.hpp)
  - [rank-2 problem class](./cpp/src/kokkos/rom/rom_problem_rank_two_forcing.hpp)
  - [rank-1 solution loop](./cpp/src/kokkos/rom/run_rom_rank_one_forcing.hpp)
  - [rank-2 solution loop](./cpp/src/kokkos/rom/run_rom_rank_two_forcing.hpp)


# License

[![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)

File is [here](./LICENSE).