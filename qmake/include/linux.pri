message(linux)

LIBS += -lGLU -lGL

QMAKE_CXXFLAGS += -Wno-ignored-qualifiers # do not warn on const return values
QMAKE_CXXFLAGS += -Wno-sign-compare
QMAKE_CXXFLAGS += -Wno-unused-variable
QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS += -Wno-reorder

QMAKE_CXXFLAGS_RELEASE += -msse2 -Os -std=c++0x
QMAKE_CXXFLAGS_DEBUG += -pg -std=c++0x

QMAKE_LFLAGS_DEBUG += -pg
QMAKE_LFLAGS_RELEASE += -flto
