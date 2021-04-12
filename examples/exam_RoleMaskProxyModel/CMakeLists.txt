cmake_minimum_required(VERSION 3.5)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Sql)

set(exam_RoleMaskProxyModel_EditableSQL_SRCS
    exam_rolemaskeditablesql.cpp
    dbresource.qrc
)
add_executable(exam_RoleMaskProxyModel_Highlight WIN32 exam_rolemaskhighlight.cpp)

target_include_directories(exam_RoleMaskProxyModel_Highlight PRIVATE ../../src ../../src/includes)
target_link_libraries(exam_RoleMaskProxyModel_Highlight PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets modelutilities)
set_target_properties(exam_RoleMaskProxyModel_Highlight PROPERTIES 
    AUTOMOC ON
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    VERSION "1.0"
    EXPORT_NAME "examRoleMaskProxyModelHighlight"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${modelutilities_PlatformDir}/lib/examples"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${modelutilities_PlatformDir}/lib/examples"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${modelutilities_PlatformDir}/bin/examples"
)
install(TARGETS exam_RoleMaskProxyModel_Highlight
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

if("${Qt${QT_VERSION_MAJOR}Sql_FOUND}")
    message(STATUS "Found Qt Sql ${Qt${QT_VERSION_MAJOR}Sql_VERSION}")
    add_executable(exam_RoleMaskProxyModel_EditableSQL WIN32 exam_rolemaskeditablesql.cpp dbresource.qrc)
    target_include_directories(exam_RoleMaskProxyModel_EditableSQL PRIVATE ../../src ../../src/includes)
    target_link_libraries(exam_RoleMaskProxyModel_EditableSQL PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql modelutilities)
    set_target_properties(exam_RoleMaskProxyModel_EditableSQL PROPERTIES 
        AUTOMOC ON
        AUTORCC ON
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED ON
        VERSION "1.0"
        SOVERSION 1
        EXPORT_NAME "examRoleMaskProxyModelEditableSQL"
        ARCHIVE_OUTPUT_DIRECTORY "../../${modelutilities_PlatformDir}/lib/examples"
        LIBRARY_OUTPUT_DIRECTORY "../../${modelutilities_PlatformDir}/lib/examples"
        RUNTIME_OUTPUT_DIRECTORY "../../${modelutilities_PlatformDir}/bin/examples"
    )
    install(TARGETS exam_RoleMaskProxyModel_EditableSQL
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
    )
endif()
