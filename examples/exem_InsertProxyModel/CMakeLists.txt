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
if(MSVC)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
endif()
set(CMAKE_AUTOMOC ON)
project(exem_InsertProxyModel VERSION "1.0") 
set(REQUIRED_QT_VERSION 5.0.0)
find_package(Qt5Core ${REQUIRED_QT_VERSION} REQUIRED)
find_package(Qt5Gui ${REQUIRED_QT_VERSION} REQUIRED)
find_package(Qt5Widgets ${REQUIRED_QT_VERSION} REQUIRED)

set(exem_InsertProxyModel_LIBS ${exem_InsertProxyModel_LIBS} ${Qt5Core_LIBRARIES})
set(exem_InsertProxyModel_INCLUDE ${exem_InsertProxyModel_INCLUDE} ${Qt5Core_INCLUDE_DIRS})
set(exem_InsertProxyModel_COMPILE_DEFINE ${exem_InsertProxyModel_COMPILE_DEFINE} ${Qt5Core_COMPILE_DEFINITIONS} )

set(exem_InsertProxyModel_LIBS ${exem_InsertProxyModel_LIBS} ${Qt5Gui_LIBRARIES})
set(exem_InsertProxyModel_INCLUDE ${exem_InsertProxyModel_INCLUDE} ${Qt5Gui_INCLUDE_DIRS})
set(exem_InsertProxyModel_COMPILE_DEFINE ${exem_InsertProxyModel_COMPILE_DEFINE} ${Qt5Gui_COMPILE_DEFINITIONS})

set(exem_InsertProxyModel_LIBS ${exem_InsertProxyModel_LIBS} ${Qt5Widgets_LIBRARIES})
set(exem_InsertProxyModel_INCLUDE ${exem_InsertProxyModel_INCLUDE} ${Qt5Widgets_INCLUDE_DIRS})
set(exem_InsertProxyModel_COMPILE_DEFINE ${exem_InsertProxyModel_COMPILE_DEFINE} ${Qt5Widgets_COMPILE_DEFINITIONS})

set(exem_InsertProxyModel_INCLUDE ${exem_InsertProxyModel_INCLUDE} ${CMAKE_CURRENT_SOURCE_DIR})
set(exem_InsertProxyModel_Subclass_SRCS
    exem_inserproxysubclass.cpp
)
set(exem_InsertProxyModel_Commit_SRCS
    exem_inserproxycommit.cpp
)
add_executable(exem_InsertProxyModel_Subclass WIN32 ${exem_InsertProxyModel_Subclass_SRCS})
add_executable(exem_InsertProxyModel_Commit WIN32 ${exem_InsertProxyModel_Commit_SRCS})
target_include_directories(exem_InsertProxyModel_Subclass PRIVATE ../../src ../../src/includes ${exem_InsertProxyModel_INCLUDE})
target_link_libraries(exem_InsertProxyModel_Subclass PRIVATE modelutilities ${exem_InsertProxyModel_LIBS})
target_compile_definitions(exem_InsertProxyModel_Subclass PRIVATE ${exem_InsertProxyModel_DEFINE})
set_target_properties(exem_InsertProxyModel_Subclass PROPERTIES 
    VERSION "1.0"
    SOVERSION 1
    EXPORT_NAME "exemModelSerialisation"
    ARCHIVE_OUTPUT_DIRECTORY "../../${modelutilities_PlatformDir}/lib/examples"
    LIBRARY_OUTPUT_DIRECTORY "../../${modelutilities_PlatformDir}/lib/examples"
    RUNTIME_OUTPUT_DIRECTORY "../../${modelutilities_PlatformDir}/bin/examples"
)
target_include_directories(exem_InsertProxyModel_Commit PRIVATE ../../src ../../src/includes ${exem_InsertProxyModel_INCLUDE})
target_link_libraries(exem_InsertProxyModel_Commit PRIVATE modelutilities ${exem_InsertProxyModel_LIBS})
target_compile_definitions(exem_InsertProxyModel_Commit PRIVATE ${exem_InsertProxyModel_DEFINE})
set_target_properties(exem_InsertProxyModel_Commit PROPERTIES 
    VERSION "1.0"
    SOVERSION 1
    EXPORT_NAME "exemModelSerialisation"
    ARCHIVE_OUTPUT_DIRECTORY "../../${modelutilities_PlatformDir}/lib/examples"
    LIBRARY_OUTPUT_DIRECTORY "../../${modelutilities_PlatformDir}/lib/examples"
    RUNTIME_OUTPUT_DIRECTORY "../../${modelutilities_PlatformDir}/bin/examples"
)
