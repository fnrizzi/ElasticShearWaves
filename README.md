
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

To simplify the process, define the following env variable:
```bash
export MYWORKDIR=<path-to-where-you-want-to-work-in>
```

You need to have:
```bash
export CC=<path-to-your-C-compiler>
export CXX=<path-to-your-CXX-compiler>
```

## BLAS/LAPACK
If you



## Kokkos and Kokkos-kernels



- Build/install kokkos and kokkos-kernels.
Currently we need to have only OpenMP execution enabled.

- Use [this file](./do_build.sh) as follows:
```bash
./do_build.sh \
 -working-dir=<where-you-want-to-build> \
 -kokkos-pfx=<path-to-your-kokkos-installation> \
 -kokkos-ker-pfx=<path-to-your-kokkos-kernels-installation> \
 --omp=yes
```


# Creating a RUN

- First, you need to generate the grid.
For example, assume you want a grid of 150 x 600 velocity points
along the radial and polar directions:
```python
python create_single_mesh.py \
 -nr 150 -nth 600 \
 -working-dir <destination-of-the-grid-files>
```
Note that this generates the grid for all the degrees of freedom, namely velocity
and stresses, since the grid is staggered.

- Second, look at
