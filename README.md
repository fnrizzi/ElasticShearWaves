
# Scope
C++ code for simulating elastic seismic shear waves in an axisymmetric domain.
The implementation uses Kokkos and Kokkos-kernels to enable performance portability,
and we provide capabilities to run both the full order model (FOM)
as well as the Galerkin reduced order model (ROM).

This code has been developed for: cite-article.
You can find more details in that paper.


# Content

- [bash script driving the build](./do_build.sh)
- [C++ source code and tests](./cpp)
- [meshing](./meshing)
- [Python scripts for processing and workflows](./python_scripts)


# Building

Here is a step-by-step guide on how to build/install all
TPLs needed and the code.


## Step 1: set basic environment
To simplify the process, define the following env variables:
```bash
export ESWSRCDIR=<path-to-the-code-repository>
export CC=<path-to-your-C-compiler>
export CXX=<path-to-your-C++-compiler>
export FC=<path-to-your-Fortran-compiler>
export MYWORKDIR=<path-to-where-you-want-to-work-in>
```
and execute:
```bash
mkdir -p ${MYWORKDIR}
```

## Step 2: BLAS/LAPACK
Here we build and install BLAS and LAPACK.
Proceed as follows:
```bash
cd ${MYWORKDIR}
mkdir tpls && cd tpls
cp ${ESWSRCDIR}/bash/build_openblas.sh .
bash build_openblas.sh
```
and then define:
```bash
export BLAS_ROOT=${MYWORKDIR}/tpls/openblas/install
export BLASLIBNAME=openblas
export LAPACK_ROOT=${MYWORKDIR}/tpls/openblas/install
export LAPACKLIBNAME=openblas
```
this should build and install OpenBLAS such that
inside `$MYWORKDIR}/tpls/openblas/install/lib` you should see something as:
```bash
total 78328
drwxr-xr-x  3 fnrizzi  staff    96B Aug 30 09:40 cmake
lrwxr-xr-x  1 fnrizzi  staff    34B Aug 30 09:40 libopenblas.0.dylib
lrwxr-xr-x  1 fnrizzi  staff    30B Aug 30 09:40 libopenblas.a
lrwxr-xr-x  1 fnrizzi  staff    34B Aug 30 09:40 libopenblas.dylib
drwxr-xr-x  3 fnrizzi  staff    96B Aug 30 09:40 pkgconfig
```
**Note**: if you already have BLAS/LAPACK installed, you can skip
the build step above and directly set the needed env vars to
point to your BLAS/LAPACK installation.


## Step 3: Kokkos and Kokkos-kernels
Now that we have BLAS/LAPACK built, we build Kokkos and Kokkos-kernels.
Proceed as follows:
```bash
cd ${MYWORKDIR}/tpls
cp ${ESWSRCDIR}/bash/build_kokkos_and_kernels.sh .
export KOKKOSPFX=${MYWORKDIR}/tpls/kokkos/kokkos_install
export KOKKOSKERPFX=${MYWORKDIR}/tpls/kokkos/kokkos_kernels_install
bash build_kokkos_and_kernels.sh
```
**Remarks**:
* the above process builds Kokkos/Kokkos-kernels *without* any arch-specific
optimization, since this is meant to work on any system. However, if you want to
have arch-specific optimizations (and you should), you need to change the arch flag
passed to Kokkos (see inside `build_kokkos_and_kernels.sh`) and rebuild;
* if you already have Kokkos/Kokkos-kernels installed, you can skip the build step
above and directly set the needed env vars to point to your installation.


<!-- - Build/install kokkos and kokkos-kernels. -->
<!-- Currently we need to have only OpenMP execution enabled. -->

<!-- - Use [this file](./do_build.sh) as follows: -->
<!-- ```bash -->
<!-- ./do_build.sh \ -->
<!--  -working-dir=<where-you-want-to-build> \ -->
<!--  -kokkos-pfx=<path-to-your-kokkos-installation> \ -->
<!--  -kokkos-ker-pfx=<path-to-your-kokkos-kernels-installation> \ -->
<!--  --omp=yes -->
<!-- ``` -->


<!-- # Creating a RUN -->

<!-- - First, you need to generate the grid. -->
<!-- For example, assume you want a grid of 150 x 600 velocity points -->
<!-- along the radial and polar directions: -->
<!-- ```python -->
<!-- python create_single_mesh.py \ -->
<!--  -nr 150 -nth 600 \ -->
<!--  -working-dir <destination-of-the-grid-files> -->
<!-- ``` -->
<!-- Note that this generates the grid for all the degrees of freedom, namely velocity -->
<!-- and stresses, since the grid is staggered. -->

<!-- - Second, look at -->