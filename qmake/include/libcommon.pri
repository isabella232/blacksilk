HEADERS +=  $$files($${SRC_DIR}/libcommon/*.hpp,true)
SOURCES +=  $$files($${SRC_DIR}/libcommon/*.cpp,true)

!win32 {
    HEADERS -=  $$files($${SRC_DIR}/libcommon/win/*.hpp,true)
    SOURCES -=  $$files($${SRC_DIR}/libcommon/win/*.cpp,true)
}
