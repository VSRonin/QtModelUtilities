cmake_minimum_required(VERSION 3.3)
if(POLICY CMP0025)
  cmake_policy(SET CMP0025 NEW)
endif()
if(POLICY CMP0048)
    cmake_policy(SET CMP0048 NEW)
endif()
if(POLICY CMP0057)
    cmake_policy(SET CMP0057 NEW)
endif()
set (CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
project(exem_TransposeProxyModel VERSION "1.0") 
set(REQUIRED_QT_VERSION 5.0.0)
find_package(Qt5Core ${REQUIRED_QT_VERSION} REQUIRED)
find_package(Qt5Gui ${REQUIRED_QT_VERSION} REQUIRED)
find_package(Qt5Widgets ${REQUIRED_QT_VERSION} REQUIRED)

set(exem_TransposeProxyModel_LIBS ${exem_TransposeProxyModel_LIBS} ${Qt5Core_LIBRARIES})
set(exem_TransposeProxyModel_INCLUDE ${exem_TransposeProxyModel_INCLUDE} ${Qt5Core_INCLUDE_DIRS})
set(exem_TransposeProxyModel_COMPILE_DEFINE ${exem_TransposeProxyModel_COMPILE_DEFINE} ${Qt5Core_COMPILE_DEFINITIONS} )

set(exem_TransposeProxyModel_LIBS ${exem_TransposeProxyModel_LIBS} ${Qt5Gui_LIBRARIES})
set(exem_TransposeProxyModel_INCLUDE ${exem_TransposeProxyModel_INCLUDE} ${Qt5Gui_INCLUDE_DIRS})
set(exem_TransposeProxyModel_COMPILE_DEFINE ${exem_TransposeProxyModel_COMPILE_DEFINE} ${Qt5Gui_COMPILE_DEFINITIONS})

set(exem_TransposeProxyModel_LIBS ${exem_TransposeProxyModel_LIBS} ${Qt5Widgets_LIBRARIES})
set(exem_TransposeProxyModel_INCLUDE ${exem_TransposeProxyModel_INCLUDE} ${Qt5Widgets_INCLUDE_DIRS})
set(exem_TransposeProxyModel_COMPILE_DEFINE ${exem_TransposeProxyModel_COMPILE_DEFINE} ${Qt5Widgets_COMPILE_DEFINITIONS})

set(exem_TransposeProxyModel_INCLUDE ${exem_TransposeProxyModel_INCLUDE} ${CMAKE_CURRENT_SOURCE_DIR})
set(exem_TransposeProxyModel_SRCS
    exem_transposeproxymodel.cpp
)
add_executable(exem_TransposeProxyModel WIN32 ${exem_TransposeProxyModel_SRCS})
if(BUILD_STATIC_LIBS)
    set(exem_TransposeProxyModel_DEFINE ${exem_TransposeProxyModel_DEFINE} MODELUTILITIES_STATIC)
endif()
add_dependencies(exem_TransposeProxyModel modelutilities)
target_include_directories(exem_TransposeProxyModel PRIVATE ../../src ../../src/includes ${exem_TransposeProxyModel_INCLUDE})
target_link_libraries(exem_TransposeProxyModel PRIVATE modelutilities ${exem_TransposeProxyModel_LIBS})
target_compile_definitions(exem_TransposeProxyModel PRIVATE ${exem_TransposeProxyModel_DEFINE})
set_target_properties(exem_TransposeProxyModel PROPERTIES 
    VERSION "1.0"
    SOVERSION 1
    EXPORT_NAME "exemTransposeProxyModel"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${modelutilities_PlatformDir}/lib/examples"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${modelutilities_PlatformDir}/lib/examples"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${modelutilities_PlatformDir}/bin/examples"
)
