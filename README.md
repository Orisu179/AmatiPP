Amati++
============

Based off of the original [Amati Project](https://github.com/glocq/Amati) by [Grégoire Locqueville](https://github.com/glocq), this project builds on top of it
and will have more features. 
This project uses the [pamplejuce](https://github.com/sudara/pamplejuce) template as a starting point.

More details will be added as the project gets developed.

Building
--------

- To build, make sure that libfaust is installed from [here](https://faust.grame.fr/downloads/), and [CMake](https://cmake.org/download/) is installed.

- Next, populate the sub repositories with ``git submodule update --init --recursive``

- Open the project in Clion or VS2022 and build the standalone version, otherwise run the following in order:
  -   ``mkdir build``
  -   ``cd build``
  -   ``cmake ..``
  -   ``cmake --build .``  

### Nix build

You can build the project using [Nix](https://nixos.org/) on Linux: just perform the above steps from within a Nix shell.

