
QT       += widgets opengl testlib network

TARGET = testFilterUndoCommand
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

macx: include( $${PRI_DIR}/mac.pri )
unix: !macx: include( $${PRI_DIR}/linux.pri )

include( $${PRI_DIR}/log.pri )
include( $${PRI_DIR}/graphics.pri )

include( $${PRI_DIR}/libserialization++.pri )
include( $${PRI_DIR}/libgraphics.pri )
include( $${PRI_DIR}/libcommon.pri )
include( $${PRI_DIR}/libfoundation.pri )
include( $${PRI_DIR}/core.pri )

INCLUDEPATH +=  $${SRC_DIR} \
                . \

HEADERS +=  $${SRC_DIR}/ui/filterundocommand.hpp

SOURCES +=  $${SRC_DIR}/ui/filterundocommand.cpp \
            testFilterUndoCommand.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"
