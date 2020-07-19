
QT       += widgets opengl network testlib

TARGET = testMixer
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
DESTDIR  += bin

OBJECTS_DIR = meta
MOC_DIR = meta
UI_DIR = meta
RCC_DIR = meta

MAIN_DIR = ../..
PRI_DIR = ../../build/commons/qmake/blacksilk/include
SRC_DIR = ../../src

include( $${PRI_DIR}/version.pri )
macx: include( $${PRI_DIR}/mac.pri )
unix: !macx: include( $${PRI_DIR}/linux.pri )

include( $${PRI_DIR}/ui.pri )
include( $${PRI_DIR}/graphics.pri )
include( $${PRI_DIR}/core.pri )
include( $${PRI_DIR}/log.pri )

# basic libraries
include( $${PRI_DIR}/libgraphics.pri )
include( $${PRI_DIR}/libcommon.pri )
include( $${PRI_DIR}/libfoundation.pri )
include( $${PRI_DIR}/libserialization++.pri )

INCLUDEPATH += . $${SRC_DIR}

HEADERS +=  $${SRC_DIR}/io/helper.h

SOURCES +=  testMixerTest.cpp \
            $${SRC_DIR}/io/helper.cpp

RESOURCES += $${MAIN_DIR}/resources/resources.qrc

DEFINES += SRCDIR=\\\"$$PWD/\\\"
