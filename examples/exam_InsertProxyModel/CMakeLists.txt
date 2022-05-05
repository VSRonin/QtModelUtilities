cmake_minimum_required(VERSION 3.9)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Sql)

add_executable(exam_InsertProxyModel_Subclass exam_inserproxysubclass.cpp)
add_executable(exam_InsertProxyModel_Commit exam_inserproxycommit.cpp)

target_include_directories(exam_InsertProxyModel_Subclass PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_include_directories(exam_InsertProxyModel_Commit PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_link_libraries(exam_InsertProxyModel_Subclass PRIVATE QtModelUtilities::QtModelUtilities Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(exam_InsertProxyModel_Commit PRIVATE QtModelUtilities::QtModelUtilities Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(exam_InsertProxyModel_Subclass PROPERTIES
    AUTOMOC ON
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
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
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    VERSION "1.0"
    EXPORT_NAME "examInsertProxyModelSubclass"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${modelutilities_PlatformDir}/lib/examples"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${modelutilities_PlatformDir}/lib/examples"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${modelutilities_PlatformDir}/bin/examples"
)

if("${Qt${QT_VERSION_MAJOR}Sql_FOUND}")
    message(STATUS "Found Qt Sql ${Qt${QT_VERSION_MAJOR}Sql_VERSION}")
    add_executable(exam_InsertProxyModel_SQLTable exam_insertsqltable.cpp dbresource.qrc)
    target_include_directories(exam_InsertProxyModel_SQLTable PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
    target_link_libraries(exam_InsertProxyModel_SQLTable PRIVATE QtModelUtilities::QtModelUtilities Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql)
    set_target_properties(exam_InsertProxyModel_SQLTable PROPERTIES 
        AUTOMOC ON
        AUTORCC ON
        WIN32_EXECUTABLE TRUE
        MACOSX_BUNDLE TRUE
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED ON
        VERSION "1.0"
        EXPORT_NAME "examInsertProxyModelSQLTable"
        ARCHIVE_OUTPUT_DIRECTORY "../../${modelutilities_PlatformDir}/lib/examples"
        LIBRARY_OUTPUT_DIRECTORY "../../${modelutilities_PlatformDir}/lib/examples"
        RUNTIME_OUTPUT_DIRECTORY "../../${modelutilities_PlatformDir}/bin/examples"
    )
endif()
