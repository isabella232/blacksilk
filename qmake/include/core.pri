
DEFINES += 'BLACKSILK_FORCE_STREAMLINED_RENDERING=0'
DEFINES += 'BLACKSILK_DISPLAY_STATS=0'

# utilities
HEADERS += \
            $${SRC_DIR}/utils/hostmachine.hpp \
            $${SRC_DIR}/utils/app.hpp \
            $${SRC_DIR}/utils/preset.hpp \
            $${SRC_DIR}/utils/curve.hpp \
            $${SRC_DIR}/utils/graphicsview.hpp \
            $${SRC_DIR}/utils/offscreencontext.hpp \
            $${SRC_DIR}/utils/conv.hpp \
            $${SRC_DIR}/utils/colorspaces.hpp
SOURCES += \
            $${SRC_DIR}/utils/hostmachine.cpp \
            $${SRC_DIR}/utils/bs_app.cpp \
            $${SRC_DIR}/utils/bs_offscreencontext.cpp \
            $${SRC_DIR}/utils/bs_preset.cpp \
            $${SRC_DIR}/utils/bs_curve.cpp \
            $${SRC_DIR}/utils/bs_graphicsview.cpp

