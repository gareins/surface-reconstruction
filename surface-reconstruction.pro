TEMPLATE = app

QT += qml quick widgets core gui
CONFIG += c++11 no_keywords

INCLUDEPATH += $$PWD/Dionysus/include/
#INCLUDEPATH += ../../Dionysus/include/

LIBS += -lCGAL -lgmp -lmpfr -lboost_thread -lboost_system -lboost_filesystem -lboost_serialization

RESOURCES += qml/resources.qrc \
    shaders/shaders.qrc \
    shaders/textures.qrc

SOURCES += src/main.cpp \
    src/triangulation.cpp \
    src/geometryengine.cpp \
    src/surfacegraph.cpp

HEADERS += \
    src/main.h \
    src/geometryengine.h \
    src/surfacegraph.h \
    src/triangulation.h \
    src/Miniball_dynamic_d.h \
    examples/alphashapes3d.h \
    examples/alphashapes3d.hpp \

OTHER_FILES += doc/src/* \
               doc/images/* \
               qml/*

# Default rules for deployment.
include(deployment.pri)

QMAKE_CXXFLAGS += -Wno-unused -frounding-math
