cmake_minimum_required(VERSION 3.5)
project(exam_InsertProxyModel LANGUAGES CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets REQUIRED)

add_executable(exam_InsertProxyModel_Subclass WIN32 exam_inserproxysubclass.cpp)
add_executable(exam_InsertProxyModel_Commit WIN32 exam_inserproxycommit.cpp)
if(BUILD_STATIC_LIBS)
    target_compile_definitions(exam_InsertProxyModel_Subclass PRIVATE MODELUTILITIES_STATIC)
    target_compile_definitions(exam_InsertProxyModel_Commit PRIVATE MODELUTILITIES_STATIC)
endif()
target_include_directories(exam_InsertProxyModel_Subclass PRIVATE ../../src ../../src/includes)
target_include_directories(exam_InsertProxyModel_Commit PRIVATE ../../src ../../src/includes)
target_link_libraries(exam_InsertProxyModel_Subclass PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets modelutilities)
target_link_libraries(exam_InsertProxyModel_Commit PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets modelutilities)
set_target_properties(exam_InsertProxyModel_Subclass PROPERTIES
    AUTOMOC ON
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    VERSION "1.0"
    EXPORT_NAME "examInsertProxyModelSubclass"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${modelutilities_PlatformDir}/lib/examples"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${modelutilities_PlatformDir}/lib/examples"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${modelutilities_PlatformDir}/bin/examples"
)
set_target_properties(exam_InsertProxyModel_Commit PROPERTIES
    AUTOMOC ON
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    VERSION "1.0"
    EXPORT_NAME "examInsertProxyModelSubclass"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${modelutilities_PlatformDir}/lib/examples"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${modelutilities_PlatformDir}/lib/examples"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${modelutilities_PlatformDir}/bin/examples"
)
install(TARGETS exam_InsertProxyModel_Subclass
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
install(TARGETS exam_InsertProxyModel_Commit
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
