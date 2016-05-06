TEMPLATE = app

QT += qml quick widgets datavisualization
CONFIG += c++11

INCLUDEPATH += $$PWD/Dionysus/include/

LIBS += -lCGAL -lgmp -lmpfr

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += src/main.cpp \
    src/surfacegraph.cpp \
    src/triangulation.cpp

RESOURCES += qml/resources.qrc

OTHER_FILES += doc/src/* \
               doc/images/* \
               qml/*
# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    src/surfacegraph.h \
    src/triangulation.h

QMAKE_CXXFLAGS += -Wno-unused -O0
