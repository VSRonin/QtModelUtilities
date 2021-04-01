cmake_minimum_required(VERSION 3.2)
project(ModelUtilitiesLib LANGUAGES CXX)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Gui)
message(STATUS "Found Qt Core ${Qt5Core_VERSION}")


if(NOT "${Qt${QT_VERSION_MAJOR}Gui_FOUND}")
    set(NO_GUI ON)
endif()
if(NOT "${Qt${QT_VERSION_MAJOR}Widgets_FOUND}")
    set(NO_WIDGETS ON)
endif()
if(NOT NO_GUI)
    message(STATUS "Found Qt Gui ${Qt${QT_VERSION_MAJOR}Gui_VERSION}")
else()
    set(NO_WIDGETS ON)
endif()
if(NOT NO_WIDGETS)
    message(STATUS "Found Qt Widgets ${Qt${QT_VERSION_MAJOR}Widgets_VERSION}")
endif()
if(BUILD_DOCS)
    find_package(Doxygen OPTIONAL_COMPONENTS mscgen dia dot)
    if(DOXYGEN_FOUND)
        message(STATUS "Found Doxygen ${DOXYGEN_VERSION}")
    else()
        set(BUILD_DOCS OFF)
    endif()
endif()
set(modelutilities_SRCS modelutilities_global.h)
set(modelutilities_INSTALL_INCLUDE modelutilities_global.h)
if(BUILD_ROLEMASKPROXY)
    set(rolemaskproxy_SRCS rolemaskproxymodel.cpp rolemaskproxymodel.h private/rolemaskproxymodel_p.h)
    set(modelutilities_SRCS ${rolemaskproxy_SRCS} ${modelutilities_SRCS})
    set(modelutilities_INSTALL_INCLUDE rolemaskproxymodel.h includes/RoleMaskProxyModel ${modelutilities_INSTALL_INCLUDE})
    source_group(RoleMaskProxyModel FILES ${rolemaskproxy_SRCS})
endif()
if(BUILD_INSERTPROXY)
    set(insertproxy_SRCS insertproxymodel.cpp insertproxymodel.h private/insertproxymodel_p.h)
    set(modelutilities_SRCS ${insertproxy_SRCS} ${modelutilities_SRCS})
    set(modelutilities_INSTALL_INCLUDE insertproxymodel.h includes/InsertProxyModel ${modelutilities_INSTALL_INCLUDE})
    source_group(InsertProxyModel FILES ${insertproxy_SRCS})
endif()
if(BUILD_MODELSERIALISATION)
    set(modelserialisation_SRCS
        abstractmodelserialiser.cpp
        abstractstringserialiser.cpp
        abstractsingleroleserialiser.cpp
        binarymodelserialiser.cpp
        csvmodelserialiser.cpp
        htmlmodelserialiser.cpp
        jsonmodelserialiser.cpp
        xmlmodelserialiser.cpp
        abstractmodelserialiser.h
        abstractstringserialiser.h
        abstractsingleroleserialiser.h
        binarymodelserialiser.h
        csvmodelserialiser.h
        htmlmodelserialiser.h
        jsonmodelserialiser.h
        xmlmodelserialiser.h
        private/abstractmodelserialiser_p.h
        private/abstractstringserialiser_p.h
        private/abstractsingleroleserialiser_p.h
        private/binarymodelserialiser_p.h
        private/csvmodelserialiser_p.h
        private/htmlmodelserialiser_p.h
        private/jsonmodelserialiser_p.h
        private/xmlmodelserialiser_p.h
    )
    set(modelutilities_SRCS ${modelserialisation_SRCS} ${modelutilities_SRCS})
    set(modelutilities_INSTALL_INCLUDE 
        abstractmodelserialiser.h
        abstractstringserialiser.h
        abstractsingleroleserialiser.h
        binarymodelserialiser.h
        csvmodelserialiser.h
        htmlmodelserialiser.h
        jsonmodelserialiser.h
        xmlmodelserialiser.h
        includes/BinaryModelSerialiser
        includes/CsvModelSerialiser
        includes/HtmlModelSerialiser
        includes/JsonModelSerialiser
        includes/ModelSerialisers
        includes/XmlModelSerialiser
        ${modelutilities_INSTALL_INCLUDE}
    )
    source_group(ModelSerialisation FILES ${modelserialisation_SRCS})
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
target_link_libraries(modelutilities PUBLIC Qt${QT_VERSION_MAJOR}::Core)
if(NOT NO_GUI)
        target_link_libraries(modelutilities PUBLIC Qt${QT_VERSION_MAJOR}::Gui)
endif()
if(NOT NO_WIDGETS)
        target_link_libraries(modelutilities PUBLIC Qt${QT_VERSION_MAJOR}::Widgets)
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
    EXPORT_NAME "modelutilities"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${modelutilities_PlatformDir}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${modelutilities_PlatformDir}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${modelutilities_PlatformDir}/bin"
)
include(CMakePackageConfigHelpers)
write_basic_package_version_file("modelutilitiesConfigVersion.cmake"
    VERSION ${VERSION_SHORT}
    COMPATIBILITY SameMajorVersion
)
install(FILES ${modelutilities_INSTALL_INCLUDE}
    DESTINATION include
)
install(TARGETS modelutilities
    EXPORT modelutilitiesTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
)
if((NOT BUILD_STATIC_LIBS) AND WIN32 AND MSVC AND (NOT MSVC_VERSION LESS 1600))
    install(FILES $<TARGET_PDB_FILE:modelutilities> DESTINATION bin OPTIONAL)
endif()
install(EXPORT modelutilitiesTargets
	FILE QtModelUtilitiesTargets.cmake
	NAMESPACE qmu::
	DESTINATION lib/cmake/modelutilities
)
install(FILES "modelutilitiesConfig.cmake" "${CMAKE_CURRENT_BINARY_DIR}/modelutilitiesConfigVersion.cmake"
    DESTINATION lib/cmake/modelutilities
)
SET(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README.markdown")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
SET(CPACK_PACKAGE_FILE_NAME "QtModelUtilities-${VERSION_SHORT}-${CMAKE_SYSTEM_NAME}-${modelutilities_PlatformDir}")
include(CPack)

