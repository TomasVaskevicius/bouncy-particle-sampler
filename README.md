# The Bouncy Particle Sampler
An implementation of new MCMC algorithm (see https://arxiv.org/abs/1510.02451)

## Resolving dependencies

Building the project requires having installed:
  * Eigen3 (A linear algebra library - [download](http://eigen.tuxfamily.org/index.php?title=Main_Page) - tested with 3.3.3 version)
  * GSL (GNU Scientific Library)
  * Qt5 (Core, Gui, PrintSupport and Widgets components are needed)
  * CVODES 2.9.0 (Required by the Stan-Math library)
  * Boost (Required by the Stan-Math library, tested with 1.62.0 version)

Resolving dependencies for Ubuntu:
`sudo apt-get install libgsl-dev qtbase5-dev libboost-all-dev`

Resolving dependencies for Mac OS X:
`brew install gsl qt boost`

The Sundials CVODES library can be found [here](http://computation.llnl.gov/projects/sundials/sundials-software).
You can download and install this library by following these instructions:
```
wget http://computation.llnl.gov/projects/sundials/download/cvodes-2.9.0.tar.gz
tar -xvzf cvodes-2.9.0.tar.gz
cd cvodes-2.9.0
mkdir build && cd build
cmake ..
sudo make install
```

## Building the examples and running tests

After resolving dependencies, the examples can be built as follows:

```
git clone https://github.com/TomasVaskevicius/bouncy-particle-sampler.git
cd bouncy-particle-sampler
mkdir build && cd build
cmake ..
make
```
Then, for example, to run the benchmarking example execute:
`./examples/benchmarking_example/benchmarking_example`

And to run the unit tests, execute:
`ctest`
