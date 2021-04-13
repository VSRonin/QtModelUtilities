macro(BasicTest TestName)
    set(targetName "tst_${TestName}")
    set(testProjectName "tst${TestName}")
    string(TOLOWER ${TestName} TestSourceFileName)
    find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Test REQUIRED)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Test REQUIRED)
    if(NOT NO_GUI)
        find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Gui)
        if(NOT "${Qt${QT_VERSION_MAJOR}Gui_FOUND}")
            set(NO_GUI ON)
        else()
            message(STATUS "Found Qt Gui ${Qt${QT_VERSION_MAJOR}Gui_VERSION}")
        endif()
    endif()
    message(STATUS "Found Qt Test ${Qt${QT_VERSION_MAJOR}Test_VERSION}")
    add_executable(${targetName}
        main.cpp
        "tst_${TestSourceFileName}.cpp"
        "tst_${TestSourceFileName}.h"
    )
    if("${Qt${QT_VERSION_MAJOR}Test_VERSION}" VERSION_LESS "5.11.0")
        file(DOWNLOAD "https://code.qt.io/cgit/qt-creator/qt-creator.git/plain/src/shared/modeltest/modeltest.cpp" "${CMAKE_CURRENT_BINARY_DIR}/modeltest.cpp" STATUS ModelTestDownloadStatus)
        list(GET ModelTestDownloadStatus 0 ModelTestDownloadStatusCode)
        if(ModelTestDownloadStatusCode EQUAL 0)
            file(DOWNLOAD "https://code.qt.io/cgit/qt-creator/qt-creator.git/plain/src/shared/modeltest/modeltest.h" "${CMAKE_CURRENT_BINARY_DIR}/modeltest.h" STATUS ModelTestDownloadStatus)
            list(GET ModelTestDownloadStatus 0 ModelTestDownloadStatusCode)
            if(ModelTestDownloadStatusCode EQUAL 0)
                target_sources(${targetName} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/modeltest.h" "${CMAKE_CURRENT_BINARY_DIR}/modeltest.cpp")
            else()
                target_compile_definitions(${targetName} PRIVATE MOC_MODEL_TEST)
            endif()
        else()
            target_compile_definitions(${targetName} PRIVATE MOC_MODEL_TEST)
        endif()
    endif()
    target_include_directories(${targetName} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
    target_link_libraries(${targetName} PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Test modelutilities)
    if(NOT NO_GUI)
        target_link_libraries(${targetName} PRIVATE Qt${QT_VERSION_MAJOR}::Gui)
    endif()
    target_compile_definitions(${targetName} PRIVATE QT_NO_CAST_TO_ASCII)
    set_target_properties(${targetName} PROPERTIES
        AUTOMOC ON
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED ON
        VERSION "1.0"
        SOVERSION 1
        EXPORT_NAME ${testProjectName}
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${modelutilities_PlatformDir}/lib/test"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${modelutilities_PlatformDir}/lib/test"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${modelutilities_PlatformDir}/bin/test"
    )
    if(TEST_OUTPUT_XML)
        set(Test_Output_FileName ${testProjectName})
        if(CMAKE_BUILD_TYPE MATCHES DEBUG)
            set(Test_Output_FileName "${Test_Output_FileName}_debug")
        endif()
        add_test(NAME ${testProjectName} WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/../TestResults" COMMAND $<TARGET_FILE:${targetName}> -o ${Test_Output_FileName},xml)
    else()
        add_test(NAME ${testProjectName} WORKING_DIRECTORY "${CMAKE_BINARY_DIR}" COMMAND $<TARGET_FILE:${targetName}>)
    endif()
endmacro()
