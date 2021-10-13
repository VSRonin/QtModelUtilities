include(TestMacro)
BasicTest(BinaryModelSerialiser)
target_sources(tst_BinaryModelSerialiser PRIVATE
    ../tst_SerialisersCommon/tst_serialiserscommon.h
    ../tst_SerialisersCommon/tst_serialiserscommon.cpp
)
target_include_directories(tst_BinaryModelSerialiser PRIVATE ../tst_SerialisersCommon/)
