# Qt Model Utilities

This library is a collection of utilities for Qt's Model/View framework.

### Status
[![Build Status](https://github.com/VSRonin/QtModelUtilities/actions/workflows/buildtest.yml/badge.svg?branch=master)](https://github.com/VSRonin/QtModelUtilities/actions)
[![Qt 5.1 Build Status](https://travis-ci.com/VSRonin/QtModelUtilities.svg?branch=master)](https://travis-ci.com/github/VSRonin/QtModelUtilities)

### Contents

+ [Role Mask Proxy Model](READMERoleMaskProxyModel.md): A proxy that will act as a mask on top of the source model to intercept data.
+ [Insert Proxy Model](READMEInsertProxyModel.md): A proxy to add an extra row, column or both to allow users to insert new sections with a familiar interface.
+ [Root Index Proxy Model](READMERootIndexProxyModel.md): A proxy to only show the portion of the model that are branches to a particular index.
+ [Model Serialisation](READMEModelSerialisation.md): Implements a general method to serialise `QAbstractItemModel` based models to various common formats.
+ [Generic Model](READMEGenericModel.md): A convenience model for generic use implementing the full `QAbstractItemModel` interface.
+ ~~Transpose Proxy Model: A proxy model to [transpose](https://en.wikipedia.org/wiki/Transpose#Examples) the original model.~~ Now part of Qt: `QTransposeProxyModel`

### Installation

#### Official Binary Release
If your system is compatible with one of the main platforms listed below you can download the pre-compiled dinamically linked library from [the releases page](https://github.com/VSRonin/QtModelUtilities/releases).

#### Build from Source
This library uses [CMake](https://cmake.org/) as build system and allows usage both using static and dynamic linking as well as customising what parts of the library you want to build. 
See [INSTALL](INSTALL.md) for detailed build instructions.

#### Use sources directly
You can include the library in your CMake (3.11 or newer) project simply by using `FetchContent`:
```
include(FetchContent)
FetchContent_Declare(QtModelUtilitiesFC
    GIT_REPOSITORY https://github.com/VSRonin/QtModelUtilities.git
    GIT_TAG master
)
FetchContent_MakeAvailable(QtModelUtilitiesFC)
```
You are then able to use they library by just adding `target_link_libraries(MyApp PUBLIC QtModelUtilities::QtModelUtilities)`

#### Using the Library
Once you installed/unpacked your library in a directory (e.g. `path/to/QtModelUtilities`)
+ **CMake**: add `path/to/QtModelUtilities` to `CMAKE_PREFIX_PATH`, then you can use `find_package(QtModelUtilities)`/`target_link_libraries(MyApp PRIVATE QtModelUtilities::QtModelUtilities)` in your CMakeLists.
+ **qmake**: add `INCLUDEPATH += path/to/QtModelUtilities/include` and `LIBS += -Lpath/to/QtModelUtilities/lib/ -QtModelUtilities` to your .pro file
+ **Visual Studio**: Right click on your project and select properties
    + under Configuration Properties->Linker->Additional Library Directories add `path/to/QtModelUtilities/lib`
    + under Configuration Properties->Linker->Input->Additional Dependencies add `QtModelUtilities.lib`
    + under C/C++->Additional Include Directories add `path/to/QtModelUtilities/include`

### Platforms

The library should be compatible with all platform supported by Qt, the CI will build, test and deploy the following configurations:

+ Windows MSVC 2019 64bit Qt 5.15
+ Windows MSVC 2019 64bit Qt 6.3
+ Windows MinGW 8.1 64bit Qt 5.15
+ Windows MinGW 8.1 64bit Qt 6.3
+ Ubuntu g++ 64bit Qt 5.15
+ Ubuntu g++ 64bit Qt 6.3
+ Mac OS X Clang 64bit Qt 5.15
+ Mac OS X Clang 64bit Qt 6.3

A reduced suite of tests, to ensure compatibility, is also run on:

+ Ubuntu g++ 64bit Qt 6.2
+ Ubuntu g++ 64bit Qt 5.12
+ Ubuntu g++ 64bit Qt 5.9
+ Ubuntu Trusty g++ 6 64bit Qt 5.1

### Examples

See the examples folder

### Documentation

[Available online](https://vsronin.github.io/QtModelUtilities/index.html)

Docs can, alternatively, be built using Doxygen and the DoxygenConfig.doxyfile file in the Docs foldes.

### Requirements:

+ Qt 5.1 or later
+ C++11
+ Doxygen (optional to build documentation)

### Contributing

Contributions are welcome. 
You can either [open a ticket](https://github.com/VSRonin/QtModelUtilities/issues) or submit a pull request on the [project's GitHub page](https://github.com/VSRonin/QtModelUtilities).

Contributed code should adhere to the [Qt Coding Style Guide](https://wiki.qt.io/Qt_Coding_Style).