cmake_minimum_required(VERSION 3.2)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets REQUIRED)

add_executable(exam_InsertProxyModel_Subclass WIN32 exam_inserproxysubclass.cpp)
add_executable(exam_InsertProxyModel_Commit WIN32 exam_inserproxycommit.cpp)

target_include_directories(exam_InsertProxyModel_Subclass PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(exam_InsertProxyModel_Commit PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(exam_InsertProxyModel_Subclass PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets QtModelUtilities::QtModelUtilities)
target_link_libraries(exam_InsertProxyModel_Commit PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets QtModelUtilities::QtModelUtilities)

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
