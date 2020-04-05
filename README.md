# Exdir for C++
![License](https://img.shields.io/github/license/HunterBelanger/exdir-cpp.svg)

This library is a C++ implementation of the Exdir specification for storing data.
Exdir is methodology of organizing and storing data, which is meant to replace 
HDF5. As with HDF5, data is stored in a heirarchy, but instead of using a single 
binary file, Exdir uses the filesystem of the users computer to establish this 
heirarchy. Within an Exdir file (which is just a folder on your computer), there
can be Groups, Datasets, or Raws. Groups are used to categorize information, and
groups may be nested within one antoher, and one group may have multiple other
groups as well. Datasets exist in groups, and contain numeric data, stored in a
Numpy npy binary file. Datasets may not contain anyting else except a Raw. Raws
are used to store raw data in other formats. These may be found inside of a
Group, or a Dataset. 

To learn more about Exdir the paper describing the specification can be found 
[here](https://www.frontiersin.org/articles/10.3389/fninf.2018.00016/full), and 
the reference implentation (written for Python) can be found 
[here](https://github.com/CINPLA/exdir).

## WARNING TO USERS
This library has been made public, but is not quite ready for use yet. Users are 
encouraged to take a look at it, and feel free to use it in your projects, but 
understand that not all features have been implemented, and many things could 
still change. Once this project reaches a more stable state, this warning will
be removed.

## Usage
A brief example program is provided in the ```example``` directory, and is 
compiled with the libraries. You can take a look at this to get a brief idea of
how it works. I will be writting more documentation soon.

## Install
All that is required to build exdir-cpp is a C++ compiler capable of the C++17
standard. This library does indeed use C++17 features, and will not build 
without them. This should not be an issue for most useers however, as any 
somewhat modern version of GCC or Clang should be more than adaquet. The 
library should build just fine on Unix  like systems. There is nothing 
preventing this project from being build on Windows systems either, accept for
the fact that those options have yet to be added or tested to the cmake files.

To build on Unix systems, navigate to the directory where you wish to store the 
files and then run the following:
```
git clone https://github.com/HunterBelanger/exdir-cpp.git
cd exdir-cpp
mkdir build && cd build
cmake ..
make -j
```
This will build both static and shared libraries for both exdir-cpp and 
yaml-cpp. These may be installed, allong with the header files for both
programs by running ```make install```. The install directory may of course
be specified with the usual ```-DCMAKE_INSTALL_PREFIX=/dir/to/install``` flag
with the cmake command.

## Notice
This library uses yaml-cpp, which is already provided in the vendor directory. 
The github repository for yaml-cpp is located 
[here](https://github.com/jbeder/yaml-cpp). It is a great piece of software 
which I recomend users take a look at. No changes have been made to the source,
just changes to the cmake to simplify the build for the purposes of this
project.
