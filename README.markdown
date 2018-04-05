# Qt Model Utilities

This library is a collection of utilities for Qt's Model/View framework.

### Status
| **master** | **dev** |
|------------|---------|
| [![Appveyor Build status](https://ci.appveyor.com/api/projects/status/3x8h2laxlbh9wc7c/branch/master?svg=true)](https://ci.appveyor.com/project/VSRonin/qtmodelutilities/branch/master) | [![Appveyor Build status](https://ci.appveyor.com/api/projects/status/3x8h2laxlbh9wc7c/branch/dev?svg=true)](https://ci.appveyor.com/project/VSRonin/qtmodelutilities/branch/dev) |
| [![Travis Build Status](https://travis-ci.org/VSRonin/QtModelUtilities.svg?branch=master)](https://travis-ci.org/VSRonin/QtModelUtilities) | [![Travis Build Status](https://travis-ci.org/VSRonin/QtModelUtilities.svg?branch=dev)](https://travis-ci.org/VSRonin/QtModelUtilities) |

### Contents

+ [Role Mask Proxy Model](https://vsronin.github.io/QtModelUtilities/md__r_e_a_d_m_e__role__mask__proxy__model.html): A proxy that will act as a mask on top of the source model to intercept data.
+ [Insert Proxy Model](https://vsronin.github.io/QtModelUtilities/md__r_e_a_d_m_e__insert__proxy__model.html): A proxy to add an extra row, column or both to allow users to insert new sections with a familiar interface.
+ [Model Serialisation](https://vsronin.github.io/QtModelUtilities/md__r_e_a_d_m_e__model__serialisation.html): Implements a general method to serialise QAbstractItemModel based models to various common formats.

### Installation

#### Official Binary Release
If your system is compatible with one of the 5 main platforms you can download the pre-compiled dinamically linked library from [the releases page](https://github.com/VSRonin/QtModelUtilities/releases).

#### Build from Source
This library uses [CMake](https://cmake.org/) as build system and allows usage both using static and dynamic linking. 
See INSTALL for detailed build instructions.

#### Use the source directly
If you use `qmake` (Qt Creator), you can include the entire source of the library directly in your code by adding `include(path/to/source/modelutilities.pri)` in your `.pro` file.

### Platforms

The library should be compatible with all platform supported by Qt, the CI will build, test and deploy the following configurations:

+ Windows MSVC 2015 32bit Qt 5.9
+ Windows MSVC 2015 64bit Qt 5.9
+ Windows MinGW 5.3.0 32bit Qt 5.9
+ Ubuntu Trusty gcc6 64bit Qt 5.9
+ Mac OS X LLVM 8.1 64bit Qt 5.10

A reduced suite of tests, to ensure compatibility, is also run on 

+ Ubuntu Trusty gcc6 64bit Qt 5.1
+ Ubuntu Trusty gcc6 64bit Qt 5.6
+ Windows MinGW 4.7.0 32bit Qt 5.0 (manual)
+ Windows MSVC 2013 32bit Qt 5.8 (manual)

### Examples

See the Examples folder

### Documentation

[Available online](https://vsronin.github.io/QtModelUtilities/index.html)

Docs can, alternatively, be built using Doxygen and the DoxygenConfig.doxyfile file in the Docs foldes.

### Requirements:

+ Qt 5
+ C++11
+ Doxygen (optional to build documentation)

### Contributing

Contributions are welcome. 
You can either [open a ticket](https://github.com/VSRonin/QtModelUtilities/issues) or submit a pull request on the [project's GitHub page](https://github.com/VSRonin/QtModelUtilities).

Contributed code should adhere to the [Qt Coding Style Guide](https://wiki.qt.io/Qt_Coding_Style).