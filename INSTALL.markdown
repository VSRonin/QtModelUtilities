# Building from sources
The build system of this library is [CMake](https://cmake.org/), the minimum supported version is 3.3

Once you installed CMake, open a terminal window with the C++ compiler and Qt environment set up. The procedure is platform dependant. 
On Windows MSVC, for example, it consist in calling `vcvarsall.bat` from the compiler folder and `qtenv2.bat` from the bin folder of Qt.

Now you can call CMake passing arguments to customise the build. The first choice is the generator. It is passed via the `-G "Generator"` argument
You can find a list of generators in the [CMake documentation](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html);
the most commonly used ones are `-G "Unix Makefiles"` on linux and macOS, `-G "MinGW Makefiles"` on Windows MinGW and `-G "NMake Makefiles"` on Windows MSVC.
It's also advisable to create a "build" folder inside the downloaded source tree to separate the build artefacts from the original sources.

The next option is the build type, this is decided by passing the `-DCMAKE_BUILD_TYPE=DEBUG` or `-DCMAKE_BUILD_TYPE=RELEASE` argument. 
For the debug build you can also specify the `-DCMAKE_DEBUG_POSTFIX=d` or `-DCMAKE_DEBUG_POSTFIX=_debug` option to append `d` or `_debug` to the name of the built library.

The final almost-mandatory argument is `-DCMAKE_INSTALL_PREFIX="path/to/installed/library"` where you can specify the path where the library will be installed.
Using, again, Windows MSVC as an example, a minimal CMake invocation would look like this:
`cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX="./installed" ../`

You can also specify an number of arguments to customise the build even further:

| Argument | Effect |
|----------|--------|
| `-DBUILD_STATIC_LIBS=ON` | Builds a statically linked library instead of a dynamic one |
| `-DBUILD_TESTING=ON` | Compiles the unit tests included in the sources |
| `-DBUILD_EXAMPLES=ON` | Compiles the examples of use included in the sources |
| `-DNO_GUI=ON` | Disables all functionalities that require the QtGUI module. This option is automatically set if the module is not installed |
| `-DNO_WIDGETS=ON` | Disables all functionalities that require the QtWidgets module. This option is automatically set if the module is not installed |
| `-DBUILD_DOCS=OFF` | Disables the creation of the documentation in HTML format. This option is automatically set if [Doxygen](www.doxygen.org) is not installed |
| `-DBUILD_ROLEMASKPROXY=OFF` | Exclude the Role Mask Proxy Model module of the library |
| `-DBUILD_MODELSERIALISATION=OFF` | Exclude the Model Serialisation module of the library |
| `-DBUILD_INSERTPROXY=OFF` | Exclude the Insert Proxy Model module of the library |
| `-DOPTIMISE_FOR_MANY_ROLES=ON` | Some of the models are optimised so that they work best if every cell in the model holds no more than around 20 different roles. This is in line with how Qt's native models are optimised. If you plan to store more roles per cell you can enable this option to improve performance |
| `-DTEST_OUTPUT_XML=ON` | This is mainly used by the CI. If this option is set, the tests will generate an xml file with results rather than printing them to the console |

After running CMake you can call `cmake --build .` to compile the project.
You can call `cmake --build . --target test` or `ctest` to run the unit tests.
Finally you can call `cmake --build . --target install` to install the library in the destination specified by `-DCMAKE_INSTALL_PREFIX=`.
