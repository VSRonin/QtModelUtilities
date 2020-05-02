cmake_minimum_required(VERSION 3.3)
project(ModelUtilitiesLib LANGUAGES CXX)

find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Widgets Gui)

message(STATUS "Found Qt Core ${Qt5Core_VERSION}")


if(NOT Qt5Gui_FOUND)
    set(NO_GUI ON)
endif()
if(NOT Qt5Widgets_FOUND) 
    set(NO_WIDGETS ON)
endif()
if(NOT NO_GUI)
    message(STATUS "Found Qt Gui ${Qt5Gui_VERSION}")
endif()
if(NOT NO_WIDGETS)
    message(STATUS "Found Qt Widgets ${Qt5Widgets_VERSION}")
endif()
if(BUILD_DOCS)
    find_package(Doxygen OPTIONAL_COMPONENTS mscgen dia dot)
    if(DOXYGEN_FOUND)
        message(STATUS "Found Doxygen ${DOXYGEN_VERSION}")
    else()
        set(BUILD_DOCS OFF)
    endif()
endif()

if(BUILD_ROLEMASKPROXY)
    set(modelutilities_SRCS rolemaskproxymodel.cpp ${modelutilities_SRCS})
    set(modelutilities_INSTALL_INCLUDE rolemaskproxymodel.h includes/RoleMaskProxyModel ${modelutilities_INSTALL_INCLUDE})
endif()
if(BUILD_INSERTPROXY)
    set(modelutilities_SRCS insertproxymodel.cpp ${modelutilities_SRCS})
    set(modelutilities_INSTALL_INCLUDE insertproxymodel.h includes/InsertProxyModel ${modelutilities_INSTALL_INCLUDE})
endif()
if(BUILD_TRANSPOSEPROXY)
    set(modelutilities_SRCS transposeproxymodel.cpp treemapproxymodel.cpp ${modelutilities_SRCS})
    set(modelutilities_INSTALL_INCLUDE transposeproxymodel.h treemapproxymodel.h includes/TransposeProxyModel ${modelutilities_INSTALL_INCLUDE})
endif()
if(BUILD_MODELSERIALISATION)
    set(modelutilities_SRCS
        abstractmodelserialiser.cpp
        abstractstringserialiser.cpp
        abstractsingleroleserialiser.cpp
        binarymodelserialiser.cpp
        csvmodelserialiser.cpp
        htmlmodelserialiser.cpp
        jsonmodelserialiser.cpp
        xmlmodelserialiser.cpp
        ${modelutilities_SRCS}
    )
    set(modelutilities_INSTALL_INCLUDE 
        abstractmodelserialiser.h
        abstractstringserialiser.h
        abstractsingleroleserialiser.h
        binarymodelserialiser.h
        csvmodelserialiser.h
        htmlmodelserialiser.h
        jsonmodelserialiser.h
        xmlmodelserialiser.h
        modelutilities_global.h
        includes/BinaryModelSerialiser
        includes/CsvModelSerialiser
        includes/HtmlModelSerialiser
        includes/JsonModelSerialiser
        includes/ModelSerialisers
        includes/XmlModelSerialiser
        ${modelutilities_INSTALL_INCLUDE}
    )
endif()

if(BUILD_STATIC_LIBS)
    set(CMAKE_STATIC_LIBRARY_SUFFIX "_static${CMAKE_STATIC_LIBRARY_SUFFIX}")
    add_library(modelutilities STATIC ${modelutilities_SRCS})
    target_compile_definitions(modelutilities PUBLIC MODELUTILITIES_STATIC)
else()
    add_library(modelutilities SHARED ${modelutilities_SRCS})
endif()
target_compile_definitions(modelutilities PRIVATE MODELUTILITIES_LIB)
if(OPTIMISE_FOR_MANY_ROLES)
    target_compile_definitions(modelutilities PRIVATE OPTIMISE_FOR_MANY_ROLES)
endif()
target_link_libraries(modelutilities PUBLIC Qt5::Core)
if(NOT NO_GUI)
	target_link_libraries(modelutilities PUBLIC Qt5::Gui)
endif()
if(NOT NO_WIDGETS)
	target_link_libraries(modelutilities PUBLIC Qt5::Widgets)
endif()

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
	
set_target_properties(modelutilities PROPERTIES 
    AUTOMOC ON
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    VERSION ${VERSION_SHORT}
    EXPORT_NAME "ModelUtilities"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${modelutilities_PlatformDir}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${modelutilities_PlatformDir}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${modelutilities_PlatformDir}/bin"
)

install(FILES ${modelutilities_INSTALL_INCLUDE}
    DESTINATION include
    COMPONENT headers
)
install(TARGETS modelutilities
    EXPORT modelutilitiesTargets
    RUNTIME DESTINATION bin COMPONENT library
    LIBRARY DESTINATION lib COMPONENT library
    ARCHIVE DESTINATION lib COMPONENT library
    INCLUDES DESTINATION include COMPONENT library
)
install(EXPORT modelutilitiesTargets
	FILE QtModelUtilitiesTargets.cmake
	NAMESPACE QtModelUtilities::
	DESTINATION lib/cmake/QtModelUtilities
)
SET(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../README.markdown")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE")
SET(CPACK_PACKAGE_FILE_NAME "QtModelUtilities-${VERSION_SHORT}-${CMAKE_SYSTEM_NAME}-${modelutilities_PlatformDir}")
include(CPack)

