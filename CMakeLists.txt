cmake_minimum_required(VERSION 3.3)
set(modelutilities_VERSION "0.0.1")
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0057 NEW)
set (CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)
project(ModelUtilitiesLib VERSION ${modelutilities_VERSION})
set(REQUIRED_QT_VERSION 5.0.0)
option(BUILD_STATIC_LIBS "Build the static library" OFF)
option(BUILD_TESTS "Build the tests" OFF)
option(NO_GUI "Disable all features requiring QtGui and QtWidgets" OFF)
option(NO_WIDGETS "Disable all features requiring QtWidgets" OFF)
option(BUILD_DOCS "Enables or disables the build of the documentation" ON)
option(BUILD_ROLEMASKPROXY "Enables or disables the build of Role Mask Proxy Model" ON)
option(BUILD_MODELSERIALISATION "Enables or disables the build of Model Serialisation" ON)
option(BUILD_INSERTPROXY "Enables or disables the build of Insert Proxy Model" ON)
find_package(Qt5Core ${REQUIRED_QT_VERSION} REQUIRED)
find_package(Qt5Gui ${REQUIRED_QT_VERSION})
find_package(Qt5Widgets ${REQUIRED_QT_VERSION})
find_package(Doxygen REQUIRED dot OPTIONAL_COMPONENTS mscgen dia)
message(STATUS "Found Qt Core ${Qt5Core_VERSION}")
if(NOT Qt5Gui_FOUND)
    set(NO_GUI ON)
endif()
if(NOT Qt5Widgets_FOUND) 
    set(NO_WIDGETS ON)
endif()
if(NO_GUI)
    set(modelutilities_COMPILE_DEFINE ${modelutilities_COMPILE_DEFINE} ${Qt5Core_COMPILE_DEFINITIONS})
else()
    message(STATUS "Found Qt Gui ${Qt5Gui_VERSION}")
    set(modelutilities_OPTIONAL_LIBS ${modelutilities_OPTIONAL_LIBS} ${Qt5Gui_LIBRARIES})
    set(modelutilities_OPTIONAL_INCLUDE ${modelutilities_OPTIONAL_INCLUDE} ${Qt5Gui_INCLUDE_DIRS})
    if(NO_WIDGETS)
        set(modelutilities_COMPILE_DEFINE ${modelutilities_COMPILE_DEFINE} ${Qt5Gui_COMPILE_DEFINITIONS})
    else()
        message(STATUS "Found Qt Widgets ${Qt5Widgets_VERSION}")
        set(modelutilities_OPTIONAL_LIBS ${modelutilities_OPTIONAL_LIBS} ${Qt5Widgets_LIBRARIES})
        set(modelutilities_OPTIONAL_INCLUDE ${modelutilities_OPTIONAL_INCLUDE} ${Qt5Widgets_INCLUDE_DIRS})
        set(modelutilities_COMPILE_DEFINE ${modelutilities_COMPILE_DEFINE} ${Qt5Widgets_COMPILE_DEFINITIONS})
    endif()
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
    set(modelutilities_INCLUDE src/rolemaskproxymodel.h src/includes/RoleMaskProxyModel ${modelutilities_INCLUDE})
endif()
if(BUILD_INSERTPROXY)
    set(modelutilities_SRCS src/insertproxymodel.cpp ${modelutilities_SRCS})
    set(modelutilities_INCLUDE src/insertproxymodel.h src/includes/InsertProxyModel ${modelutilities_INCLUDE})
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
    set(modelutilities_INCLUDE 
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
        ${modelutilities_INCLUDE}
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
    target_compile_definitions(modelutilities PUBLIC MODELUTILITIES_STATIC)
else()
    add_library(modelutilities SHARED ${modelutilities_SRCS})
endif()
install(FILES ${modelutilities_INCLUDE} DESTINATION include COMPONENT headers)
install(TARGETS modelutilities
    RUNTIME DESTINATION bin COMPONENT library
    LIBRARY DESTINATION lib COMPONENT library
    ARCHIVE DESTINATION lib COMPONENT library
)
target_include_directories(modelutilities PRIVATE ${Qt5Core_INCLUDE_DIRS} ${modelutilities_OPTIONAL_INCLUDE})
target_link_libraries(modelutilities PRIVATE ${Qt5Core_LIBRARIES} ${modelutilities_OPTIONAL_LIBS})
target_compile_definitions(modelutilities PRIVATE ${modelutilities_COMPILE_DEFINE})
set_target_properties(modelutilities PROPERTIES 
    VERSION ${modelutilities_VERSION}
    SOVERSION 0
    EXPORT_NAME "ModelUtilities"
    ARCHIVE_OUTPUT_DIRECTORY "${modelutilities_PlatformDir}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${modelutilities_PlatformDir}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${modelutilities_PlatformDir}/bin"
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
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

