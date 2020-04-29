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
    set(modelutilities_SRCS src/rolemaskproxymodel.cpp ${modelutilities_SRCS})
    set(modelutilities_INSTALL_INCLUDE src/rolemaskproxymodel.h src/includes/RoleMaskProxyModel ${modelutilities_INSTALL_INCLUDE})
endif()
if(BUILD_INSERTPROXY)
    set(modelutilities_SRCS src/insertproxymodel.cpp ${modelutilities_SRCS})
    set(modelutilities_INSTALL_INCLUDE src/insertproxymodel.h src/includes/InsertProxyModel ${modelutilities_INSTALL_INCLUDE})
endif()
if(BUILD_TRANSPOSEPROXY)
    set(modelutilities_SRCS src/transposeproxymodel.cpp src/treemapproxymodel.cpp ${modelutilities_SRCS})
    set(modelutilities_INSTALL_INCLUDE src/transposeproxymodel.h src/treemapproxymodel.h src/includes/TransposeProxyModel ${modelutilities_INSTALL_INCLUDE})
endif()
if(BUILD_MODELSERIALISATION)
    set(modelutilities_SRCS
        src/abstractmodelserialiser.cpp
        src/abstractstringserialiser.cpp
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
        src/abstractstringserialiser.h
        src/abstractsingleroleserialiser.h
        src/binarymodelserialiser.h
        src/csvmodelserialiser.h
        src/htmlmodelserialiser.h
        src/jsonmodelserialiser.h
        src/xmlmodelserialiser.h
        src/modelutilities_global.h
        src/includes/BinaryModelSerialiser
        src/includes/CsvModelSerialiser
        src/includes/HtmlModelSerialiser
        src/includes/JsonModelSerialiser
        src/includes/ModelSerialisers
        src/includes/XmlModelSerialiser
        ${modelutilities_INSTALL_INCLUDE}
    )
endif()

if(BUILD_STATIC_LIBS)
	set(CMAKE_STATIC_LIBRARY_SUFFIX "_static${CMAKE_STATIC_LIBRARY_SUFFIX}")
    add_library(modelutilities STATIC ${modelutilities_SRCS})
    target_compile_definitions(modelutilities PRIVATE MODELUTILITIES_STATIC)
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

#install(FILES ${modelutilities_INSTALL_INCLUDE}
#    DESTINATION include 
#    COMPONENT headers
#)
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
include(CMakePackageConfigHelpers)
write_basic_package_version_file("QtModelUtilitiesVersion.cmake" VERSION ${VERSION_SHORT} COMPATIBILITY ${VERSION_MAJOR})
install(FILES "QtModelUtilitiesConfig.cmake" "QtModelUtilitiesVersion.cmake" DESTINATION lib/cmake/QtModelUtilitiesQ)
SET(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../README.markdown")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE")
SET(CPACK_PACKAGE_FILE_NAME "modelutilities-${modelutilities_VERSION}-${CMAKE_SYSTEM_NAME}-${modelutilities_PlatformDir}")
include(CPack)

