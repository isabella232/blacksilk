CONFIG += c++14

# standard fdi setup
INCLUDEPATH += $${SRC_DIR}
# set debug/release info
CONFIG(debug, debug|release) {
    COMPILE_MODE=debug
    D=D # e.g. for msvcp140d.dll
    DEFINES += _DEBUG
    IM_FLAG=DB
}

CONFIG(release, debug|release) {
    COMPILE_MODE=release
    DEFINES += _RELEASE
    IM_FLAG=RL
}
unix:!macx: CONFIG += linux

OBJECTS_DIR = $${MAIN_DIR}/tmp/$${TARGET}/$${COMPILE_MODE}/objects
MOC_DIR = $${MAIN_DIR}/tmp/$${TARGET}/$${COMPILE_MODE}/moc
UI_DIR = $${MAIN_DIR}/tmp/$${TARGET}/$${COMPILE_MODE}/uic
RCC_DIR = $${MAIN_DIR}/tmp/$${TARGET}/$${COMPILE_MODE}/rcc
#MAKEFILE = $${MAIN_DIR}/tmp/$${TARGET}/$${COMPILE_MODE}/Makefile

DEFINES += __PRETTY_FUNCTION__=__FUNCTION__

# patch BS for Ubuntu 14.04. w/ Intel drivers
exists( $${MAIN_DIR}/../../GL_RGBA16_SNORM ) {
    message(Fix for Intel on Ubuntu 14.04.:)
    message(Use GL_RGB(A)16_SNORM instead GL_RGB(A)16)
    DEFINES += INTEL_UBUNTU_SNORM
}
