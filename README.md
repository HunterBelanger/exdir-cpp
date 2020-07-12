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

## Usage
Here is a brief example of exdir-cpp in action
```cpp
#include<exdir/exdir.hpp>

int main() {

  exdir::File file = exdir::create_file("test.exdir");

  exdir::Group group1 = file.create_group("group1");

  std::vector<int> test_data { 1,  2,  3,  4,
                               5,  6,  7,  8,
                               9, 10, 11, 12,
                              13, 14, 15, 16};

  exdir::NDArray test_array(test_data, {4,4});

  test_array.reshape({4,2,2});

  exdir::Dataset<int> dset_1 group1.create_dataset<int>("data_set_1", test_array);

  dset_1.attrs["density"] = 12.4;

  dset_1.data(0,0,0) = 17.;

  dset_1.data.reshape({4,4});

  return 0;
}
```
A more in-depth explination of the classes and usage is given in the wiki 
[here](https://github.com/HunterBelanger/exdir-cpp/wiki/Usage).

## Dependencies
This package is dependent on the [yaml-cpp](https://github.com/jbeder/yaml-cpp)
library, to handle all interactions with the ```.yaml``` files. It is present in
most distros repositories, and can usually be installed with your system's
package manager.

* **Ubuntu / Linux Mint :**
```sudo apt install libyaml-cpp-dev```

* **Fedora :**
```sudo dnf install yaml-cpp-devel```

* **Arch / Manjaro :**
```sudo pacman -S yaml-cpp```

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
If you would like to install the library and header file to your system's
default location, you may then run ```sudo make install``` after. You can
also set the install location yourself by running cmake with the
```-DCMAKE_INSTALL_PREFIX=/dir/to/install``` flag.
