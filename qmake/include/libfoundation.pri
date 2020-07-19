HEADERS +=  $$files($${SRC_DIR}/libfoundation/*.hpp,true)
SOURCES +=  $$files($${SRC_DIR}/libfoundation/*.cpp,true)

win32 {
    LIBS += -lShell32
}

# DEFINES += LIBFOUNDATION_DEBUG_OUTPUT
