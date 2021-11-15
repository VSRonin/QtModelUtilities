macro(BasicTest TestName)
    cmake_minimum_required(VERSION 3.9)
    set(targetName "tst_${TestName}")
    set(testProjectName "tst${TestName}")
    string(TOLOWER ${TestName} TestSourceFileName)
    add_executable(${targetName}
        main.cpp
        "tst_${TestSourceFileName}.cpp"
        "tst_${TestSourceFileName}.h"
    )
    target_link_libraries(${targetName} PRIVATE tstLib::tstLib)
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
        set(Test_Output_FileName "${Test_Output_FileName}.xml")
        add_test(NAME ${testProjectName} WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/../TestResults" COMMAND $<TARGET_FILE:${targetName}> -o ${Test_Output_FileName},xml)
    else()
        add_test(NAME ${testProjectName} WORKING_DIRECTORY "${CMAKE_BINARY_DIR}" COMMAND $<TARGET_FILE:${targetName}>)
    endif()
endmacro()
