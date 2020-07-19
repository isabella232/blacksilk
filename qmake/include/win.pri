
DEFINES -= UNICODE
DEFINES += NOMINMAX

CONFIG(debug, debug|release) {
# performance
    QMAKE_CXXFLAGS += /Qpar              # parallel code generation
    QMAKE_CXXFLAGS += /Ox                # optimisation
    QMAKE_CXXFLAGS += /Ob2               # inline all possible
    QMAKE_CXXFLAGS += /Oi                # activate system internal functions
    QMAKE_CXXFLAGS += /Ot                # prefer fast code
    QMAKE_CXXFLAGS += /Oy                # surpress frame pointer
    QMAKE_CXXFLAGS += /GT                # fibre safe optimisation
    QMAKE_CXXFLAGS += /GL                # complete optimisation, needs /LTCG
    QMAKE_LFLAGS   += /LTCG              # link time code generation
    QMAKE_LFLAGS   += /OPT:REF           # delete unused functions
    QMAKE_LFLAGS   += /OPT:ICF           # COMDAT folding
}
