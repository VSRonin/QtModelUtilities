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
set(CMAKE_AUTOMOC ON)
project(tst_RoleMaskProxyModel VERSION "1.0") 
set(REQUIRED_QT_VERSION 5.0.0)
find_package(Qt5Test ${REQUIRED_QT_VERSION} REQUIRED)
find_package(Qt5Gui ${REQUIRED_QT_VERSION} REQUIRED)
message(STATUS "Found Qt Test ${Qt5Test_VERSION}")
set(tst_RoleMaskProxyModel_LIBS ${tst_RoleMaskProxyModel_LIBS} ${Qt5Test_LIBRARIES})
set(tst_RoleMaskProxyModel_INCLUDE ${tst_RoleMaskProxyModel_INCLUDE} ${Qt5Test_INCLUDE_DIRS})
set(tst_RoleMaskProxyModel_COMPILE_DEFINE ${tst_RoleMaskProxyModel_COMPILE_DEFINE} ${Qt5Test_COMPILE_DEFINITIONS} )
if(NOT NO_GUI)
set(tst_RoleMaskProxyModel_LIBS ${tst_RoleMaskProxyModel_LIBS} ${Qt5Gui_LIBRARIES})
set(tst_RoleMaskProxyModel_INCLUDE ${tst_RoleMaskProxyModel_INCLUDE} ${Qt5Gui_INCLUDE_DIRS})
set(tst_RoleMaskProxyModel_COMPILE_DEFINE ${tst_RoleMaskProxyModel_COMPILE_DEFINE} ${Qt5Gui_COMPILE_DEFINITIONS})
endif()
set(tst_RoleMaskProxyModel_INCLUDE ${tst_RoleMaskProxyModel_INCLUDE} ${CMAKE_CURRENT_SOURCE_DIR})
set(tst_RoleMaskProxyModel_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tst_rolemaskproxymodel.cpp
)
add_executable(tst_RoleMaskProxyModel ${tst_RoleMaskProxyModel_SRCS})
target_include_directories(tst_RoleMaskProxyModel PRIVATE ../../src ${tst_RoleMaskProxyModel_INCLUDE})
target_link_libraries(tst_RoleMaskProxyModel PRIVATE modelutilities ${tst_RoleMaskProxyModel_LIBS})
target_compile_definitions(tst_RoleMaskProxyModel PRIVATE ${tst_RoleMaskProxyModel_DEFINE})
set_target_properties(tst_RoleMaskProxyModel PROPERTIES 
    VERSION "1.0"
    SOVERSION 1
    EXPORT_NAME "tstRoleMaskProxyModel"
    ARCHIVE_OUTPUT_DIRECTORY "../../${modelutilities_PlatformDir}/lib"
    LIBRARY_OUTPUT_DIRECTORY "../../${modelutilities_PlatformDir}/lib"
    RUNTIME_OUTPUT_DIRECTORY "../../${modelutilities_PlatformDir}/bin"
)
