cmake_minimum_required(VERSION 3.2)
include(TestMacro)
BasicTest(CsvModelSerialiser)
target_sources(tst_CsvModelSerialiser PRIVATE
    ../tst_SerialisersCommon/tst_serialiserscommon.h
    ../tst_SerialisersCommon/tst_serialiserscommon.cpp
)
target_include_directories(tst_CsvModelSerialiser PRIVATE ../tst_SerialisersCommon/)