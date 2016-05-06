TEMPLATE = app

QT += qml quick widgets core gui
CONFIG += c++11

# The .cpp file which was generated for your project. Feel free to hack it.
RESOURCES += qml/resources.qrc \
    shaders/shaders.qrc \
    shaders/textures.qrc


SOURCES += src/main.cpp \
    src/geometryengine.cpp \
    src/surfacegraph.cpp

HEADERS += \
    src/geometryengine.h \
    src/surfacegraph.h

OTHER_FILES += doc/src/* \
               doc/images/* \
               qml/*
# Default rules for deployment.
include(deployment.pri)
