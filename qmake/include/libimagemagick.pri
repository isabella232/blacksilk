win32:      TARGET = libgraphics.io.plugins.imagemagick
macx:       TARGET = graphics.io.plugins.imagemagick
unix:!macx: TARGET = graphics.io.plugins.imagemagick

TEMPLATE = lib
CONFIG += x86_64
CONFIG -= qt
DEFINES += IMAGEMAGICK_IMPORTER_GLOBAL_META_IMAGE

MAIN_DIR = $$_PRO_FILE_PWD_/..
PRI_DIR  = $${MAIN_DIR}/qmake/include

include( $${PRI_DIR}/setup.pri )

DESTDIR  = $${MAIN_DIR}/libs/libgraphics.io.plugins.imagemagick/bin/$${COMPILE_MODE}
SRC_DIR  = $${MAIN_DIR}/src
INCLUDEPATH += $${SRC_DIR}

macx:       include( $${PRI_DIR}/mac.pri )
unix:!macx: include( $${PRI_DIR}/linux.pri )
win32:      include( $${PRI_DIR}/win.pri )

include( $${PRI_DIR}/imagemagick.pri )
include( $${PRI_DIR}/libcommon.pri )
include( $${PRI_DIR}/log.pri )

linux {
    # set library search path to .
    QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN
}
