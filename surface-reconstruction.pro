TEMPLATE = app

QT += qml quick widgets datavisualization
CONFIG += c++11

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += src/main.cpp \
    src/surfacegraph.cpp

RESOURCES += qml/resources.qrc

OTHER_FILES += doc/src/* \
               doc/images/* \
               qml/*
# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    src/surfacegraph.h
