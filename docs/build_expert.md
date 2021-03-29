
# Building process (assuming CMake familiarity)

This page describes the prerequisites and how to build the code.
It is intended for those who are already familiar with CMake.
If you prefer to fully bypass handling CMake and the code's TPLs 
directly, we also provide a [step-by-step guide](./build.md).


## Prerequisites
Requires: 

- CMake>=3.11.0

- C++ compiler with C++14 support

- BLAS/LAPACK 

- Kokkos/Kokkos-kernels: see [this script](../bash/build_kokkos_and_kernels.sh) 
if you want to see our ready-to-use bash script for these TPLs. 

- The yaml-cpp library: https://github.com/jbeder/yaml-cpp

- The Eigen library: https://eigen.tuxfamily.org/index.php?title=Main_Page


The code has been tested on MaxOS Catalina 10.15.5 
and RedHat 7.0 with GCC-8.3.1, GCC-8.4.0, using Kokkos-3.1.1, 
Eigen-3.3.7 and yaml-cpp-0.6.3.


## Build 

Make a build directory *outside* of the source tree and do the usual. 
Here is a template cmake command:

```
export ESWSRCDIR=<path-to-the-code-repository>
export CXX=<path-to-your-C++-compiler>

mkdir build && cd build
cmake -DCMAKE_CXX_COMPILER=${CXX} \
      -DCMAKE_VERBOSE_MAKEFILE:BOOL=TRUE \
      -DCMAKE_BUILD_TYPE=Release \
      \
      -DEIGEN_INCLUDE_DIR=<path-to-eigen-headers> \
      \
      -DYAMLCPP_INCLUDE_DIR=<path-to-yamlcpp-root-installation>/include \
      -DYAMLCPP_LIB_DIR=<path-to-yamlcpp-root-installation>/lib \
      \
      -DKokkosKernels_DIR=<path-to-kokkoskernels-root-installation>/lib/cmake/KokkosKernels \
      ${ESWSRCDIR}/cpp
make -j4
```

You can then run the tests:
```bash
# from inside the build directory
ctest
```
which should display (if tests pass) something like this:
```bash
      Start  1: fomInnerDomainKokkos1
 1/10 Test  #1: fomInnerDomainKokkos1 .............   Passed    0.66 sec
      Start  2: fomInnerDomainKokkos2
 2/10 Test  #2: fomInnerDomainKokkos2 .............   Passed    0.60 sec
      Start  3: fomNearSurfaceKokkos1
 3/10 Test  #3: fomNearSurfaceKokkos1 .............   Passed    0.67 sec
      Start  4: fomNearSurfaceKokkos2
 4/10 Test  #4: fomNearSurfaceKokkos2 .............   Passed    0.63 sec
      Start  5: fomNearCmbKokkos1
 5/10 Test  #5: fomNearCmbKokkos1 .................   Passed    0.91 sec
      Start  6: fomNearCmbKokkos2
 6/10 Test  #6: fomNearCmbKokkos2 .................   Passed    0.80 sec
 ...
```
