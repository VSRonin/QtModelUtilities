include(TestMacro)
BasicTest(XmlModelSerialiser)
target_sources(tst_XmlModelSerialiser PRIVATE
    ../tst_SerialisersCommon/tst_serialiserscommon.h
    ../tst_SerialisersCommon/tst_serialiserscommon.cpp
)
target_include_directories(tst_XmlModelSerialiser PRIVATE ../tst_SerialisersCommon/)
