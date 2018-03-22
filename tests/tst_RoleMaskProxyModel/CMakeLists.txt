cmake_minimum_required(VERSION 3.0)
cmake_policy(SET CMP0048 NEW)
set(CMAKE_AUTOMOC ON)
project(tst_RoleMaskProxyModel VERSION "1.0")
set(REQUIRED_QT_VERSION 5.0.0)
find_package(Qt5Test ${REQUIRED_QT_VERSION} REQUIRED)
message(STATUS "Found Qt Test ${Qt5Test_VERSION}")
set(tst_RoleMaskProxyModel_COMPILE_DEFINE ${tst_RoleMaskProxyModel_COMPILE_DEFINE} ${Qt5Test_COMPILE_DEFINITIONS} )
if(NO_GUI)
    set(tst_RoleMaskProxyModel_COMPILE_DEFINE ${tst_RoleMaskProxyModel_COMPILE_DEFINE} ${Qt5Core_COMPILE_DEFINITIONS})
else()
    set(tst_RoleMaskProxyModel_OPTIONAL_LIBS ${tst_RoleMaskProxyModel_OPTIONAL_LIBS} ${Qt5Gui_LIBRARIES})
    set(tst_RoleMaskProxyModel_OPTIONAL_INCLUDE ${tst_RoleMaskProxyModel_OPTIONAL_INCLUDE} ${Qt5Gui_INCLUDE_DIRS})
    if(NO_WIDGETS)
        set(tst_RoleMaskProxyModel_COMPILE_DEFINE ${tst_RoleMaskProxyModel_COMPILE_DEFINE} ${Qt5Gui_COMPILE_DEFINITIONS})
    else()
        set(tst_RoleMaskProxyModel_OPTIONAL_LIBS ${tst_RoleMaskProxyModel_OPTIONAL_LIBS} ${Qt5Widgets_LIBRARIES})
        set(tst_RoleMaskProxyModel_OPTIONAL_INCLUDE ${tst_RoleMaskProxyModel_OPTIONAL_INCLUDE} ${Qt5Widgets_INCLUDE_DIRS})
        set(tst_RoleMaskProxyModel_COMPILE_DEFINE ${tst_RoleMaskProxyModel_COMPILE_DEFINE} ${Qt5Widgets_COMPILE_DEFINITIONS})
    endif()
endif()
include_directories(../../src ${Qt5Test_INCLUDE_DIRS} ${tst_RoleMaskProxyModel_OPTIONAL_INCLUDE})
set(tst_RoleMaskProxyModel_SRCS
    main.cpp
    tst_rolemaskproxymodel.cpp
)
add_executable(tst_RoleMaskProxyModel ${tst_RoleMaskProxyModel_SRCS})
target_link_libraries(tst_RoleMaskProxyModel PRIVATE modelutilities ${Qt5Test_LIBRARIES} ${tst_RoleMaskProxyModel_OPTIONAL_LIBS})
target_compile_definitions(tst_RoleMaskProxyModel PRIVATE ${tst_RoleMaskProxyModel_DEFINE})
#if(BUILD_STATIC_LIBS)
#    target_compile_definitions(tst_RoleMaskProxyModel PRIVATE MODELUTILITIES_STATIC)
#endif()
set_target_properties(tst_RoleMaskProxyModel PROPERTIES 
    VERSION "1.0"
    SOVERSION 1
    EXPORT_NAME "tstRoleMaskProxyModel"
    ARCHIVE_OUTPUT_DIRECTORY "../../${modelutilities_PlatformDir}/lib"
    LIBRARY_OUTPUT_DIRECTORY "../../${modelutilities_PlatformDir}/lib"
    RUNTIME_OUTPUT_DIRECTORY "../../${modelutilities_PlatformDir}/bin"
)