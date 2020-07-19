# Qt config
QT += core gui opengl widgets

TARGET = BlackSilk
TEMPLATE = app
CONFIG += x86_64

DEFINES *= BLACKSILK_STANDALONE
DEFINES *= QT_NO_OPENGL_ES_2
DEFINES += QT_MESSAGELOGCONTEXT

MAIN_DIR = $$_PRO_FILE_PWD_/..
PRI_DIR  = $${MAIN_DIR}/qmake/include
SRC_DIR  = $${MAIN_DIR}/src

include( $${PRI_DIR}/setup.pri )

DESTDIR  = $${MAIN_DIR}/bin/$${COMPILE_MODE}

include( $${PRI_DIR}/version.pri )
macx:  include( $${PRI_DIR}/mac.pri )
linux: include( $${PRI_DIR}/linux.pri )
win32: include( $${PRI_DIR}/win.pri )

include( $${PRI_DIR}/standalone.pri )
include( $${PRI_DIR}/imagemagick.pri )

include( $${PRI_DIR}/ui.pri )
include( $${PRI_DIR}/graphics.pri )
include( $${PRI_DIR}/core.pri )
include( $${PRI_DIR}/shader.pri )
include( $${PRI_DIR}/log.pri )

# basic libraries
include( $${PRI_DIR}/libgraphics.pri )
include( $${PRI_DIR}/libcommon.pri )
include( $${PRI_DIR}/libfoundation.pri )
include( $${PRI_DIR}/libserialization++.pri )

RESOURCES += $${MAIN_DIR}/resources/resources.qrc
!macx: RESOURCES += $${MAIN_DIR}/resources/qtconf.qrc

ICON = $${MAIN_DIR}/resources/Icons/Icon.icns
win32: RC_FILE = $${MAIN_DIR}/resources/win/blacksilk.rc

CONFIG(release) {
    DEFINES += NDEBUG
}
