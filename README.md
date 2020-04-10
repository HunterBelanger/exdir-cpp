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
Here is a brief example of exdir-cpp in action
```cpp
#include<exdir/exdir.hpp>

int main() {

  exdir::File file = exdir::create_file("test.exdir");

  file.create_group("group1");

  exdir::Group group1 = file.get_group("group1");

  std::vector<int> test_data { 1,  2,  3,  4,
                               5,  6,  7,  8,
                               9, 10, 11, 12,
                              13, 14, 15, 16};

  exdir::Array test_array(test_data, {4,4}, exdir::DType::INT64);

  test_array.reshape({4,2,2});

  group1.create_dataset("data_set_1", test_array);

  exdir::Dataset dset_1 = group1.get_dataset("data_set_1");

  dset_1.attrs["density"] = 12.4;

  dset_1.write();

  return 0;
}
```
A more in-depth explination of the classes and usage is given in the wiki 
[here](https://github.com/HunterBelanger/exdir-cpp/wiki/Usage).

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
This library uses yaml-cpp (version 0.6.3), which is already provided in the 
vendor directory. The github repository for yaml-cpp is located 
[here](https://github.com/jbeder/yaml-cpp). It is a great piece of software 
which I recomend users take a look at. The only changes which have been made
were a small change to the source in 
```vendor\yaml-cpp\include\yaml-cpp\node\detail\node_iterator.h``` at line 56
to comply with the C++17 standard (and to make Visual Studio warnings go away), 
and the cmake, to simplify the build process.