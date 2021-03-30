cmake_minimum_required(VERSION 3.2)
project(exam_ModelSerialisation LANGUAGES CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets REQUIRED)

add_executable(exam_ModelSerialisation WIN32 exam_modelserialisation.cpp)
if(BUILD_STATIC_LIBS)
    target_compile_definitions(exam_ModelSerialisation PRIVATE MODELUTILITIES_STATIC)
endif()
target_include_directories(exam_ModelSerialisation PRIVATE ../../src ../../src/includes)
target_link_libraries(exam_ModelSerialisation PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets modelutilities)
set_target_properties(exam_ModelSerialisation PROPERTIES
    AUTOMOC ON
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    VERSION "1.0"
    EXPORT_NAME "examModelSerialisation"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${modelutilities_PlatformDir}/lib/examples"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${modelutilities_PlatformDir}/lib/examples"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${modelutilities_PlatformDir}/bin/examples"
)
install(TARGETS exam_ModelSerialisation
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
