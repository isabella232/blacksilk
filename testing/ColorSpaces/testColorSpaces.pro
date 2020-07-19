#-------------------------------------------------
#
# Project created by QtCreator 2011-12-04T16:13:13
#
#-------------------------------------------------

QT       += core gui testlib

TARGET = testColorSpaces
CONFIG   += console
CONFIG   -= app_bundle
DESTDIR  += bin
TEMPLATE = app

PRI_DIR = ../../build/commons/qmake/blacksilk/include
macx: include( $${PRI_DIR}/mac.pri )

INCLUDEPATH += ../../src .

QMAKE_CXXFLAGS_DEBUG += -O0 -g

HEADERS += helpers.h
SOURCES += testColorSpaces.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"


