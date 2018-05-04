cmake_minimum_required(VERSION 3.3)
set(modelutilities_VERSION_MAJOR 0)
set(modelutilities_VERSION_MINOR 0)
set(modelutilities_VERSION_PATCH 1)
set(modelutilities_VERSION "${modelutilities_VERSION_MAJOR}.${modelutilities_VERSION_MINOR}.${modelutilities_VERSION_PATCH}")
if(POLICY CMP0025)
  cmake_policy(SET CMP0025 NEW)
endif()
if(POLICY CMP0048)
    cmake_policy(SET CMP0048 NEW)
endif()
if(POLICY CMP0057)
    cmake_policy(SET CMP0057 NEW)
endif()
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
project(ModelUtilitiesLib VERSION ${modelutilities_VERSION})
include(CTest)
set(REQUIRED_QT_VERSION 5.0.0)
option(BUILD_STATIC_LIBS "Build the static library" OFF)
option(BUILD_EXAMPLES "Build the examples" OFF)
option(NO_GUI "Disable all features requiring QtGui and QtWidgets" OFF)
option(NO_WIDGETS "Disable all features requiring QtWidgets" OFF)
option(BUILD_DOCS "Enables or disables the build of the documentation" ON)
option(BUILD_ROLEMASKPROXY "Enables or disables the build of Role Mask Proxy Model" ON)
option(BUILD_MODELSERIALISATION "Enables or disables the build of Model Serialisation" ON)
option(BUILD_INSERTPROXY "Enables or disables the build of Insert Proxy Model" ON)
option(OPTIMISE_FOR_MANY_ROLES "Set this property to ON if you plan to store more than 20 different roles in the models to optimise performance" OFF)
find_package(Qt5Core ${REQUIRED_QT_VERSION} REQUIRED)
find_package(Qt5Gui ${REQUIRED_QT_VERSION})
find_package(Qt5Widgets ${REQUIRED_QT_VERSION})
find_package(Doxygen OPTIONAL_COMPONENTS mscgen dia dot)
message(STATUS "Found Qt Core ${Qt5Core_VERSION}")
set(modelutilities_INCLUDE ${modelutilities_INCLUDE} ${Qt5Core_INCLUDE_DIRS})
set(modelutilities_INCLUDE ${modelutilities_INCLUDE} "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(modelutilities_INCLUDE ${modelutilities_INCLUDE} ${CMAKE_CURRENT_SOURCE_DIR})
set(modelutilities_LIBS ${modelutilities_LIBS} ${Qt5Core_LIBRARIES})
set(modelutilities_COMPILE_DEFINE ${modelutilities_COMPILE_DEFINE} ${Qt5Core_COMPILE_DEFINITIONS})
if(OPTIMISE_FOR_MANY_ROLES)
    set(modelutilities_COMPILE_DEFINE ${modelutilities_COMPILE_DEFINE} OPTIMISE_FOR_MANY_ROLES)
endif()
if(NOT Qt5Gui_FOUND)
    set(NO_GUI ON)
endif()
if(NOT Qt5Widgets_FOUND) 
    set(NO_WIDGETS ON)
endif()
if(NOT NO_GUI)
    message(STATUS "Found Qt Gui ${Qt5Gui_VERSION}")
    set(modelutilities_LIBS ${modelutilities_LIBS} ${Qt5Gui_LIBRARIES})
    set(modelutilities_INCLUDE ${modelutilities_INCLUDE} ${Qt5Gui_INCLUDE_DIRS})
    set(modelutilities_COMPILE_DEFINE ${modelutilities_COMPILE_DEFINE} ${Qt5Gui_COMPILE_DEFINITIONS})
endif()
if(NOT NO_WIDGETS)
    message(STATUS "Found Qt Widgets ${Qt5Widgets_VERSION}")
    set(modelutilities_LIBS ${modelutilities_LIBS} ${Qt5Widgets_LIBRARIES})
    set(modelutilities_INCLUDE ${modelutilities_INCLUDE} ${Qt5Widgets_INCLUDE_DIRS})
    set(modelutilities_COMPILE_DEFINE ${modelutilities_COMPILE_DEFINE} ${Qt5Widgets_COMPILE_DEFINITIONS})
endif()

if(DOXYGEN_FOUND)
    message(STATUS "Found Doxygen ${DOXYGEN_VERSION}")
else()  
    set(BUILD_DOCS OFF)
endif()
if(CMAKE_BUILD_TYPE MATCHES DEBUG)
    set(BUILD_DOCS OFF)
endif()
if(BUILD_STATIC_LIBS)
    set(CMAKE_STATIC_LIBRARY_SUFFIX "_static${CMAKE_STATIC_LIBRARY_SUFFIX}")
endif()
set(modelutilities_COMPILE_DEFINE ${modelutilities_COMPILE_DEFINE} MODELUTILITIES_LIB)
if(BUILD_ROLEMASKPROXY)
    set(modelutilities_SRCS src/rolemaskproxymodel.cpp ${modelutilities_SRCS})
    set(modelutilities_INSTALL_INCLUDE src/rolemaskproxymodel.h src/includes/RoleMaskProxyModel ${modelutilities_INSTALL_INCLUDE})
endif()
if(BUILD_INSERTPROXY)
    set(modelutilities_SRCS src/insertproxymodel.cpp ${modelutilities_SRCS})
    set(modelutilities_INSTALL_INCLUDE src/insertproxymodel.h src/includes/InsertProxyModel ${modelutilities_INSTALL_INCLUDE})
endif()
if(BUILD_MODELSERIALISATION)
    set(modelutilities_SRCS
        src/abstractmodelserialiser.cpp
        src/abstractmultiroleserialiser.cpp
        src/abstractsingleroleserialiser.cpp
        src/binarymodelserialiser.cpp
        src/csvmodelserialiser.cpp
        src/htmlmodelserialiser.cpp
        src/jsonmodelserialiser.cpp
        src/xmlmodelserialiser.cpp
        ${modelutilities_SRCS}
    )
    set(modelutilities_INSTALL_INCLUDE 
        src/abstractmodelserialiser.h
        src/abstractmultiroleserialiser.h
        src/abstractsingleroleserialiser.h
        src/binarymodelserialiser.h
        src/csvmodelserialiser.h
        src/htmlmodelserialiser.h
        src/jsonmodelserialiser.h
        src/xmlmodelserialiser.h
        src/includes/BinaryModelSerialiser
        src/includes/CsvModelSerialiser
        src/includes/HtmlModelSerialiser
        src/includes/JsonModelSerialiser
        src/includes/ModelSerialisers
        src/includes/XmlModelSerialiser
        ${modelutilities_INSTALL_INCLUDE}
    )
endif()
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
    set(modelutilities_COMPILE_DEFINE ${modelutilities_COMPILE_DEFINE} MODELUTILITIES_STATIC)
else()
    add_library(modelutilities SHARED ${modelutilities_SRCS})
endif()
install(FILES ${modelutilities_INSTALL_INCLUDE}
    DESTINATION include 
    COMPONENT headers
)
install(TARGETS modelutilities
    EXPORT modelutilitiesBinary
    RUNTIME DESTINATION bin COMPONENT library
    LIBRARY DESTINATION lib COMPONENT library
    ARCHIVE DESTINATION lib COMPONENT library
)
install(FILES modelutilities-config.cmake DESTINATION cmake)
install(EXPORT modelutilitiesBinary DESTINATION cmake)
target_include_directories(modelutilities PUBLIC 
    $<BUILD_INTERFACE:${modelutilities_INCLUDE}>
    $<INSTALL_INTERFACE:include>
    )
target_link_libraries(modelutilities PUBLIC ${modelutilities_LIBS})
target_compile_definitions(modelutilities PUBLIC ${modelutilities_COMPILE_DEFINE})
set_target_properties(modelutilities PROPERTIES 
    VERSION ${modelutilities_VERSION}
    EXPORT_NAME "ModelUtilities"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${modelutilities_PlatformDir}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${modelutilities_PlatformDir}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${modelutilities_PlatformDir}/bin"
)
if(BUILD_DOCS)
    set(htmlDocRedirect "<!DOCTYPE html><html><head><meta http-equiv=\"refresh\" content=\"0; url=html/index.html\" /></head><body><p><a href=\"html/index.html\">Redirect</a></p></body></html>")
    file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/docs/htmldocs.html" ${htmlDocRedirect})
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/DoxygenConfig.doxyfile)
    add_custom_target( doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_IN}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs/html DESTINATION doc COMPONENT documentation)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/docs/htmldocs.html DESTINATION doc COMPONENT documentation)
endif()
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
if(BUILD_TESTING)
    enable_testing()
    option(TEST_OUTPUT_XML "Redirects the test output to xml files in the TestResults folder" OFF)
    if(TEST_OUTPUT_XML)
        file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/TestResults")
    endif()
    add_subdirectory(tests)
endif()
SET(CPACK_PACKAGE_VERSION_MAJOR modelutilities_VERSION_MAJOR)
SET(CPACK_PACKAGE_VERSION_MINOR modelutilities_VERSION_MINOR)
SET(CPACK_PACKAGE_VERSION_PATCH modelutilities_VERSION_PATCH)
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.markdown")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
SET(CPACK_PACKAGE_FILE_NAME "modelutilities-${modelutilities_VERSION}-${CMAKE_SYSTEM_NAME}-${modelutilities_PlatformDir}")
include(CPack)

