# The Bouncy Particle Sampler
An implementation of new MCMC algorithm (see https://arxiv.org/abs/1510.02451)

## Resolving dependencies

Building the project requires having installed the development packages for:
  * GSL (GNU Scientific Library)
  * Qt5 (Core, Gui, PrintSupport and Widgets components are needed)

Resolving dependencies for Ubuntu:
`sudo apt-get install libgsl-dev qtbase5-dev`

Resolving dependencies for Mac OS X:
`brew install gsl qt`

## Building the examples

After resolving dependencies, the examples can be built as follows:

```
git clone https://github.com/TomasVaskevicius/bouncy-particle-sampler.git
cd bouncy-particle-sampler
mkdir build && cd build
cmake ..
make
````

Then, for example, to run the benchmarking example execute:

`./examples/benchmarking_example/benchmarking_example`
