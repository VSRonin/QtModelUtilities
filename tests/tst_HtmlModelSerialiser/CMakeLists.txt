include(TestMacro)
BasicTest(HtmlModelSerialiser)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Network)
if("${Qt${QT_VERSION_MAJOR}Network_FOUND}")
    message(STATUS "Found Qt Network ${Qt${QT_VERSION_MAJOR}Network_VERSION}")
endif()
target_link_libraries(tst_HtmlModelSerialiser PRIVATE Qt${QT_VERSION_MAJOR}::Network)
target_sources(tst_HtmlModelSerialiser PRIVATE
    ../tst_SerialisersCommon/tst_serialiserscommon.h
    ../tst_SerialisersCommon/tst_serialiserscommon.cpp
)
target_include_directories(tst_HtmlModelSerialiser PRIVATE ../tst_SerialisersCommon/)
