
QT       += testlib widgets

TARGET = testYUVFrame
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
DESTDIR  += bin

OBJECTS_DIR = meta
MOC_DIR = meta
UI_DIR = meta
RCC_DIR = meta

PRI_DIR = ../../build/commons/qmake/blacksilk/include
SRC_DIR = ../../src

macx:        include( $${PRI_DIR}/mac.pri )
unix: !macx: include( $${PRI_DIR}/linux.pri )

INCLUDEPATH +=  . \
                $${SRC_DIR}/ \
                $${SRC_DIR}/graphics

HEADERS +=   \
            helpers.h \
            $${SRC_DIR}/utils/colorspaces.hpp \
            $${SRC_DIR}/ui/yuvframe.h

SOURCES +=  testYUVFrameTest.cpp \
            $${SRC_DIR}/ui/yuvframe.cpp


DEFINES += SRCDIR=\\\"$$PWD/\\\"
