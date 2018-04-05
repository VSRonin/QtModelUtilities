SOURCES += $$files($$PWD/src/*.cpp)
HEADERS += $$files($$PWD/src/*.h)
HEADERS += $$files($$PWD/src/*.hpp)
SOURCES += $$files($$PWD/src/private/*.cpp)
HEADERS += $$files($$PWD/src/private/*.h)
HEADERS += $$files($$PWD/src/private/*.hpp)
DEFINES += MODELUTILITIES_STATIC
INCLUDEPATH += $$PWD/src
INCLUDEPATH += $$PWD/src/includes
INCLUDEPATH += $$PWD/src/private