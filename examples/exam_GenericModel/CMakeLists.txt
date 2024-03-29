cmake_minimum_required(VERSION 3.9)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets REQUIRED)
add_executable(exam_GenericModel exam_genericmodel.cpp)
target_include_directories(exam_GenericModel PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_link_libraries(exam_GenericModel PRIVATE QtModelUtilities::QtModelUtilities Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets)
set_target_properties(exam_GenericModel PROPERTIES
    AUTOMOC ON
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    VERSION "1.0"
    EXPORT_NAME "examGenericModel"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${modelutilities_PlatformDir}/lib/examples"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${modelutilities_PlatformDir}/lib/examples"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${modelutilities_PlatformDir}/bin/examples"
)
