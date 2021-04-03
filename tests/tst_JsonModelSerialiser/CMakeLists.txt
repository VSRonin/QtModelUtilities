cmake_minimum_required(VERSION 3.2)
include(TestMacro)
BasicTest(JsonModelSerialiser)
target_sources(tst_JsonModelSerialiser PRIVATE
    ../tst_SerialisersCommon/tst_serialiserscommon.h
    ../tst_SerialisersCommon/tst_serialiserscommon.cpp
)
target_include_directories(tst_JsonModelSerialiser PRIVATE ../tst_SerialisersCommon/)
