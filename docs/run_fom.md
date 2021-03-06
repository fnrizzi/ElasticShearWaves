
# Creating and running a full case
Here we describe how to create and run a full-order model simulation.

## Preparing the env
Set the following env variables:
```bash
export ESWSRCDIR=<path-to-the-code-repository>
export MYWORKDIR=<the-same-work-directory-used-for-building-process>
export MYRUNDIR=${MYWORKDIR}/myFirstRun
mkdir ${MYRUNDIR}
```
Here we assume you already ran the [step-by-step guide](./docs/build.md)
to build the code and used `MYWORKDIR` as the working directory for that procedure,
so that `${MYWORKDIR}/build` contains all the executables.

You also need Python>3.6 with the following packages: matplotlib, numpy, scipy, yaml.


## Generating the mesh
The code has been developed such that the mesh is generated with Python
and is used by the C++ code. There are two main reasons for this choice:
first, it allows us to decouple the mesh generation from the actual physics code;
second, we developed the code to support the concept of sample mesh,
which is a key feature for nonlinear ROMs. This is **not** needed right now
to solve the current elastic shear wave problem because this is a linear problem,
but it can be useful if, in the future, we extend the code to support nonlinear problems.

To specify the grid, one only needs to specify the grid for the velocity points because
the stress points are defined based on the staggered scheme (see paper).
Assume the test case you want to run uses a grid of 200 x 1000 velocity points
along the radial and polar directions, respectively.
To generate the mesh files proceed as follows:
```python
cd ${ESWSRCDIR}/meshing
python create_single_mesh.py -nr 200 -nth 1000 -working-dir ${MYRUNDIR}
```
This should generate a directory `${MYRUNDIR}/mesh200x1000` containing:
```bash
-rw-r--r--  1 fnrizzi  staff   4.5M Aug 30 12:20 coeff_vp.dat
-rw-r--r--  1 fnrizzi  staff    28M Aug 30 12:20 graph_sp.dat
-rw-r--r--  1 fnrizzi  staff    16M Aug 30 12:20 graph_vp.dat
-rw-r--r--  1 fnrizzi  staff   231B Aug 30 12:20 mesh_info.dat
```

## Create input file
Inputs for the code are based on yaml.
For the purpose of this guide, you can start from doing:
```bash
cp ${ESWSRCDIR}/tutorialRunFiles/fom_run/input.yaml ${MYRUNDIR}
```
The input file is organized into sections:
- *general*: contains general inputs, e.g., where the mesh is, time stepping, etc;
- *io*: contains parameters to collect data, e.g., the snapshot matrix and seismogram;
- *source*: contains parameters to define the kind of source signal, and its depth with respect to earth surface;
- *material*: defines the type of material to use.


## Run the FOM
After creating the input file, we can now link the FOM executable and run:
```bash
cd ${MYRUNDIR}
ln -s ${MYWORKDIR}/build/shwave_fom .
OMP_NUM_THREADS=4 OMP_PLACES=threads OMP_PROC_BIND=spread ./shwave_fom input.yaml
```
This should generate inside `${MYRUNDIR}` the following files:
```
coords_sp.txt : coordinates of the velocity grid points
coords_vp.txt : coordinates of the stresses grid points
seismogram_0  : seismogram at the receiver locations set in input.yaml
snaps_vp_0    : snapshot matrix for the velocity
snaps_sp_0    : snapshot matrix for the stresses
```

## Post-process data
To post-process the data, we already created ad-hoc Python scripts for visualizing
the results of this run. Copy the scripts to the destination:
```bash
cp ${ESWSRCDIR}/tutorialRunFiles/fom_run/*.py ${MYRUNDIR}
```

First, we visualize the seismogram data by doing:
```bash
cd ${MYRUNDIR}
python plotSeismogram.py
```
which should generate a plot like this:
![image](https://github.com/fnrizzi/ElasticShearWaves/blob/master/tutorialRunFiles/fom_run/seismogram.png)

Second, we can visualize the full wavefield at three times, `t=1000, 1500, 2000` (seconds) as follows:
```bash
cd ${MYRUNDIR}
ln -s ${MYWORKDIR}/build/extractStateFromSnaps .
./extractStateFromSnaps --snaps=./snaps_vp_0 binary \
	--fsize=1 --outformat=ascii --timesteps=4000 6000 8000 --samplingfreq=100 --outfileappend=vp
python plotWavefield.py
```
which should generate three plots as follows:<br>
<img src="https://github.com/fnrizzi/ElasticShearWaves/blob/master/tutorialRunFiles/fom_run/wavefield_4000.png" width="33%">
<img src="https://github.com/fnrizzi/ElasticShearWaves/blob/master/tutorialRunFiles/fom_run/wavefield_6000.png" width="33%">
<img src="https://github.com/fnrizzi/ElasticShearWaves/blob/master/tutorialRunFiles/fom_run/wavefield_8000.png" width="33%">
