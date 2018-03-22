cmake_minimum_required(VERSION 3.0)
set(modelutilities_VERSION "0.0.1")
cmake_policy(SET CMP0048 NEW)
set(CMAKE_AUTOMOC ON)
project(ModelUtilitiesLib VERSION ${modelutilities_VERSION})
set(REQUIRED_QT_VERSION 5.0.0)
option(BUILD_STATIC_LIBS "Build the static library" OFF)
option(BUILD_TESTS "Build the tests" OFF)
option(NO_GUI "Disable all features requiring QtGui and QtWidgets" OFF)
option(NO_WIDGETS "Disable all features requiring QtWidgets" OFF)
find_package(Qt5Core ${REQUIRED_QT_VERSION} REQUIRED)
find_package(Qt5Gui ${REQUIRED_QT_VERSION})
find_package(Qt5Widgets ${REQUIRED_QT_VERSION})
message(STATUS "Found Qt Core ${Qt5Core_VERSION}")
if(NO_GUI)
    if(Qt5Gui_FOUND)
        message(STATUS "Found Qt Gui ${Qt5Gui_VERSION}")
        set(modelutilities_OPTIONAL_LIBS ${modelutilities_OPTIONAL_LIBS} ${Qt5Gui_LIBRARIES})
        set(modelutilities_OPTIONAL_INCLUDE ${modelutilities_OPTIONAL_INCLUDE} ${Qt5Gui_INCLUDE_DIRS})
        set(modelutilities_OPTIONAL_DEFINE ${modelutilities_OPTIONAL_DEFINE} ${Qt5Gui_DEFINITIONS})
    endif(Qt5Gui_FOUND)
    if(NO_WIDGETS)
        if(Qt5Widgets_FOUND)
            message(STATUS "Found Qt Widgets ${Qt5Widgets_VERSION}")
            set(modelutilities_OPTIONAL_LIBS ${modelutilities_OPTIONAL_LIBS} ${Qt5Widgets_LIBRARIES})
            set(modelutilities_OPTIONAL_INCLUDE ${modelutilities_OPTIONAL_INCLUDE} ${Qt5Widgets_INCLUDE_DIRS})
            set(modelutilities_OPTIONAL_DEFINE ${modelutilities_OPTIONAL_DEFINE} ${Qt5Widgets_DEFINITIONS})
        endif(Qt5Widgets_FOUND)
    endif(NO_WIDGETS)
endif(NO_GUI)
if(BUILD_STATIC_LIBS)
    set(modelutilities_OPTIONAL_DEFINE "-DMODELUTILITIES_STATIC" ${modelutilities_DEFINITIONS})
    set(CMAKE_STATIC_LIBRARY_SUFFIX "_static${CMAKE_STATIC_LIBRARY_SUFFIX}")
endif()
set(modelutilities_COMPILE_DEFINE "-DMODELUTILITIES_LIB" ${Qt5Core_DEFINITIONS} ${modelutilities_OPTIONAL_DEFINE})
include_directories(./src ${Qt5Core_INCLUDE_DIRS} ${modelutilities_OPTIONAL_INCLUDE})
set(modelutilities_SRCS
    src/abstractmodelserialiser.cpp
    src/abstractmultiroleserialiser.cpp
    src/abstractsingleroleserialiser.cpp
    src/binarymodelserialiser.cpp
    src/csvmodelserialiser.cpp
    src/htmlmodelserialiser.cpp
    src/insertproxymodel.cpp
    src/jsonmodelserialiser.cpp
    src/rolemaskproxymodel.cpp
    src/xmlmodelserialiser.cpp
)
if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
    set(modelutilities_IS64BITS OFF)
else()
    set(modelutilities_IS64BITS ON)
endif()
if(modelutilities_IS64BITS)
    set(modelutilities_PlatformDir "x64")
else()
    set(modelutilities_PlatformDir "x86")
endif()
if(BUILD_STATIC_LIBS)
    add_library(modelutilities STATIC ${modelutilities_SRCS})
else()
    add_library(modelutilities SHARED ${modelutilities_SRCS})
endif()
add_definitions(${modelutilities_COMPILE_DEFINE})
target_link_libraries(modelutilities PRIVATE ${Qt5Core_LIBRARIES} ${modelutilities_OPTIONAL_LIBRARIES})
set_target_properties(modelutilities PROPERTIES 
    VERSION ${modelutilities_VERSION}
    SOVERSION 0
    EXPORT_NAME "ModelUtilities"
    ARCHIVE_OUTPUT_DIRECTORY "${modelutilities_PlatformDir}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${modelutilities_PlatformDir}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${modelutilities_PlatformDir}/bin"
)


