message(mac)

LIBS += -framework CoreServices -framework Carbon -framework ApplicationServices -framework OpenGL

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.13
QMAKE_MAC_SDK = macosx10.14

QMAKE_CXXFLAGS_WARN_ON -= -Wall
QMAKE_CXXFLAGS_WARN_ON += -Wall
QMAKE_CXXFLAGS_WARN_ON += -Wno-unknown-pragmas
QMAKE_CXXFLAGS_WARN_ON += -Wno-ignored-qualifiers # do not warn on const return values
QMAKE_CXXFLAGS_WARN_ON += -Wno-sign-compare
QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-variable
QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter
QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
