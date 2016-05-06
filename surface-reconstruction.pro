TEMPLATE = app

QT += qml quick widgets core gui
CONFIG += c++11

INCLUDEPATH += $$PWD/Dionysus/include/

LIBS += -lCGAL -lgmp -lmpfr

RESOURCES += qml/resources.qrc \
    shaders/shaders.qrc \
    shaders/textures.qrc

SOURCES += src/main.cpp \
    src/triangulation.cpp \
    src/geometryengine.cpp \
    src/surfacegraph.cpp

HEADERS += \
    src/geometryengine.h \
    src/surfacegraph.h \
    src/triangulation.h

OTHER_FILES += doc/src/* \
               doc/images/* \
               qml/*
# Default rules for deployment.
include(deployment.pri)

QMAKE_CXXFLAGS += -Wno-unused -O0
