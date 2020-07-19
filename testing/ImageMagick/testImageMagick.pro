
QT       += widgets opengl testlib network

TARGET = testImageMagick
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
DESTDIR  += bin

MAIN_DIR = ../..
PRI_DIR = ../../build/commons/qmake/blacksilk/include
SRC_DIR = ../../src
include( $${PRI_DIR}/setup.pri )

linux: PLATFORM=linux
win32: PLATFORM=win
macx:  PLATFORM=mac
macx:  include( $${PRI_DIR}/mac.pri )
linux: include( $${PRI_DIR}/linux.pri )
win32: include( $${PRI_DIR}/win.pri )

include( $${PRI_DIR}/log.pri )
include( $${PRI_DIR}/graphics.pri )
include( $${PRI_DIR}/imagemagick.pri )

include( $${PRI_DIR}/libserialization++.pri )
include( $${PRI_DIR}/libgraphics.pri )
include( $${PRI_DIR}/libcommon.pri )
include( $${PRI_DIR}/libfoundation.pri )
include( $${PRI_DIR}/core.pri )

RESOURCES += $${MAIN_DIR}/resources/resources.qrc

INCLUDEPATH +=  $${SRC_DIR} .

SOURCES += testImageMagick.cpp
HEADERS += cat.h

DEFINES += SRCDIR=\\\"$$PWD/\\\"
DEFINES += LIBGRAPHICS_DEBUG_OUTPUT
DEFINES += BLACKSILK_STANDALONE
