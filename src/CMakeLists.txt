cmake_minimum_required(VERSION 3.9)

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

if(BUILD_ROLEMASKPROXY)
    set(rolemaskproxy_SRCS rolemaskproxymodel.cpp rolemaskproxymodel.h private/rolemaskproxymodel_p.h)
    set(modelutilities_SRCS ${rolemaskproxy_SRCS} ${modelutilities_SRCS})
    set(modelutilities_INSTALL_INCLUDE rolemaskproxymodel.h includes/RoleMaskProxyModel ${modelutilities_INSTALL_INCLUDE})
    source_group(RoleMaskProxyModel FILES ${rolemaskproxy_SRCS})
    set(modules_DEFS QTMODELUTILITIES_ROLEMASKPROXYMODEL ${modules_DEFS})
endif()
if(BUILD_INSERTPROXY)
    set(insertproxy_SRCS insertproxymodel.cpp insertproxymodel.h private/insertproxymodel_p.h)
    set(modelutilities_SRCS ${insertproxy_SRCS} ${modelutilities_SRCS})
    set(modelutilities_INSTALL_INCLUDE insertproxymodel.h includes/InsertProxyModel ${modelutilities_INSTALL_INCLUDE})
    source_group(InsertProxyModel FILES ${insertproxy_SRCS})
    set(modules_DEFS QTMODELUTILITIES_INSERTPROXYMODEL ${modules_DEFS})
endif()
if(BUILD_ROOTINDEXPROXY)
    set(rootindexproxy_SRCS rootindexproxymodel.cpp rootindexproxymodel.h private/rootindexproxymodel_p.h)
    set(modelutilities_SRCS ${rootindexproxy_SRCS} ${modelutilities_SRCS})
    set(modelutilities_INSTALL_INCLUDE rootindexproxymodel.h includes/RootIndexProxyModel ${modelutilities_INSTALL_INCLUDE})
    source_group(RootIndexModel FILES ${rootindexproxy_SRCS})
    set(modules_DEFS QTMODELUTILITIES_ROOTINDEXPROXYMODEL ${modules_DEFS})
endif()
if(BUILD_GENERICMODEL)
    set(genericmodel_SRCS genericmodel.cpp genericmodel.h private/genericmodel_p.h)
    set(modelutilities_SRCS ${genericmodel_SRCS} ${modelutilities_SRCS})
    set(modelutilities_INSTALL_INCLUDE genericmodel.h includes/GenericModel ${modelutilities_INSTALL_INCLUDE})
    source_group(GenericModel FILES ${genericmodel_SRCS})
    set(modules_DEFS QTMODELUTILITIES_GENERICMODEL ${modules_DEFS})
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
    set(modules_DEFS QTMODELUTILITIES_MODELSERIALISER ${modules_DEFS})
endif()
if(BUILD_DOCS)
    set(htmlDocRedirect "<!DOCTYPE html><html><head><title>Redirect</title><meta http-equiv=\"refresh\" content=\"0; url=html/index.html\" /></head><body><p><a href=\"html/index.html\">Click here if you are not redirected automatically</a></p></body></html>")
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/docs/htmldocs.html" ${htmlDocRedirect})
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../docs/DoxygenConfig.doxyfile DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/DoxygenConfig.doxyfile "\nPROJECT_NUMBER = ${VERSION_SHORT}\nINPUT = ../\nOUTPUT_DIRECTORY = ${CMAKE_CURRENT_BINARY_DIR}/docs")
    add_custom_target( doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/DoxygenConfig.doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs/html DESTINATION docs COMPONENT documentation)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/docs/htmldocs.html DESTINATION docs COMPONENT documentation)
endif()
if(modelutilities_SRCS)
    set(modelutilities_SRCS ${modelutilities_SRCS} private/modelutilities_common_p.h)
    include(GenerateExportHeader)
    include(GNUInstallDirs)
    add_library(QtModelUtilities ${modelutilities_SRCS})
    add_library(QtModelUtilities::QtModelUtilities ALIAS QtModelUtilities)
    generate_export_header(QtModelUtilities
        STATIC_DEFINE MODELUTILITIES_STATIC
        EXPORT_MACRO_NAME MODELUTILITIES_EXPORT
        EXPORT_FILE_NAME globaldefs/modelutilities_global.h
    )
    target_include_directories(QtModelUtilities PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/globaldefs>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/includes>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
    if(NOT BUILD_SHARED_LIBS)
        target_compile_definitions(QtModelUtilities PUBLIC MODELUTILITIES_STATIC)
    endif()
    target_compile_definitions(QtModelUtilities PRIVATE MODELUTILITIES_LIB QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII)
    target_compile_definitions(QtModelUtilities PUBLIC ${modules_DEFS})
    if(OPTIMISE_FOR_MANY_ROLES)
        target_compile_definitions(QtModelUtilities PRIVATE OPTIMISE_FOR_MANY_ROLES)
    endif()
    target_link_libraries(QtModelUtilities PRIVATE Qt${QT_VERSION_MAJOR}::Core)
    if(NOT NO_GUI)
        target_link_libraries(QtModelUtilities PRIVATE Qt${QT_VERSION_MAJOR}::Gui)
    endif()
    if(NOT NO_WIDGETS)
        target_link_libraries(QtModelUtilities PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
    endif()


    set_target_properties(QtModelUtilities PROPERTIES
        AUTOMOC ON
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED ON
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN TRUE
        SOVERSION ${VERSION_MAJOR}
        VERSION ${VERSION_SHORT}
        EXPORT_NAME "QtModelUtilities"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${modelutilities_PlatformDir}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${modelutilities_PlatformDir}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${modelutilities_PlatformDir}/bin"
    )
    include(CMakePackageConfigHelpers)
    write_basic_package_version_file("QtModelUtilitiesConfigVersion.cmake"
        VERSION ${VERSION_SHORT}
        COMPATIBILITY SameMajorVersion
    )
    install(FILES ${modelutilities_INSTALL_INCLUDE} "${CMAKE_CURRENT_BINARY_DIR}/globaldefs/modelutilities_global.h"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        COMPONENT QtModelUtilities_Development
    )
    install(TARGETS QtModelUtilities
        EXPORT QtModelUtilitiesTargets
        RUNTIME
            DESTINATION ${CMAKE_INSTALL_BINDIR}
            COMPONENT QtModelUtilities_Runtime
        LIBRARY
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT QtModelUtilities_Runtime
            NAMELINK_SKIP
        INCLUDES
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            COMPONENT QtModelUtilities_Development
        ARCHIVE
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT QtModelUtilities_Development
    )
    if(BUILD_SHARED_LIBS)
        install(TARGETS QtModelUtilities
            EXPORT QtModelUtilitiesTargets
            LIBRARY
                DESTINATION ${CMAKE_INSTALL_LIBDIR}
                COMPONENT QtModelUtilities_Development
                NAMELINK_ONLY
        )
    endif()
    install(EXPORT QtModelUtilitiesTargets
            FILE QtModelUtilitiesTargets.cmake
            NAMESPACE QtModelUtilities::
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QtModelUtilities
            COMPONENT QtModelUtilities_Development
    )
    install(FILES "QtModelUtilitiesConfig.cmake" "${CMAKE_CURRENT_BINARY_DIR}/QtModelUtilitiesConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QtModelUtilities
        COMPONENT QtModelUtilities_Development
    )
endif()
