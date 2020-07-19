
message(standalone $${COMPILE_MODE})
INCLUDEPATH += $${SRC_DIR}

IM_PLUGIN=libgraphics.io.plugins.imagemagick

# mac: build app package for mac
macx {
    QMAKE_POST_LINK += mkdir -p $${DESTDIR}/$${TARGET}.$${TEMPLATE}/Contents/Resources/;
    QMAKE_POST_LINK += mkdir -p $${DESTDIR}/$${TARGET}.$${TEMPLATE}/Contents/Frameworks/;
    QMAKE_POST_LINK += cp $${MAIN_DIR}/libs/$${IM_PLUGIN}/bin/$${COMPILE_MODE}/$${IM_PLUGIN}.dylib $${DESTDIR}/$${TARGET}.$${TEMPLATE}/Contents/Frameworks/;

    # write version number into bundle
    QMAKE_POST_LINK += echo $$VERSION > $${DESTDIR}/$${TARGET}.$${TEMPLATE}/Contents/Resources/version.txt;

    # QMAKE_POST_LINK += macdeployqt $${DESTDIR}/$${TARGET}.$${TEMPLATE};
    QMAKE_POST_LINK += macdeployqt $${DESTDIR}/$${TARGET}.$${TEMPLATE} -executable=$${DESTDIR}/$${TARGET}.$${TEMPLATE}/Contents/Frameworks/$${IM_PLUGIN}.dylib;
    # QMAKE_POST_LINK += $${MAIN_DIR}/scripts/mac/fix_qt_frameworks.sh $${DESTDIR}/$${TARGET}.$${TEMPLATE};
}

linux {
    # set library search path to ./lib
    QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN/lib

    # reduce binary size
    CONFIG(release, debug|release) { QMAKE_POST_LINK += strip --strip-all $${DESTDIR}/$${TARGET}; }

    QMAKE_POST_LINK += mkdir -p $${DESTDIR}/lib/;
    QMAKE_POST_LINK += cp -H $${MAIN_DIR}/libs/$${IM_PLUGIN}/bin/$${COMPILE_MODE}/$${IM_PLUGIN}.so $${DESTDIR}/lib/;
    QMAKE_POST_LINK += cp -H $${MAIN_DIR}/libs/imagemagick/bin/$${COMPILE_MODE}/*.so.7 $${DESTDIR}/lib/;
    QMAKE_POST_LINK += cp -H $${MAIN_DIR}/libs/imagemagick/bin/$${COMPILE_MODE}/*.so.4 $${DESTDIR}/lib/;

    # write version number into bundle
    QMAKE_POST_LINK += echo $$VERSION > $${DESTDIR}/lib/version.txt;
}

# win: add manifest
win32 {
    CRLF = $$escape_expand(\n\t)
    DESTDIR_WIN = $${DESTDIR}
    DESTDIR_WIN ~= s,/,\\,g
    FULL_TARGET = $${DESTDIR_WIN}\\$${TARGET}.exe

    # write version number into bundle
    QMAKE_POST_LINK += $$quote( echo $$VERSION > $${DESTDIR_WIN}/version.txt $$CRLF )

    # check with
    # mt -inputresource:BlackSilk.exe -out:extracted.manifest
    QMAKE_POST_LINK += $$quote( echo \"chmod +w...\" & icacls $${DESTDIR_WIN} /grant \"%USERNAME%:(OI)(CI)F\" $$CRLF )
    QMAKE_POST_LINK += $$quote( echo \"Adding manifest...\" & mt -nologo \
                           -manifest \"$${MAIN_DIR}/resources/win/compat.manifest\" \
                           -outputresource:$${FULL_TARGET} $$CRLF )
    QMAKE_POST_LINK += $$quote( echo \"Deploy...\" & $$(QTDIR)/bin/windeployqt \"$${FULL_TARGET}\" $$CRLF )

    # copy deps to dest (http://stackoverflow.com/a/3984180)
    EXTRA_BINFILES = $${MAIN_DIR}/libs/$${IM_PLUGIN}/bin/$${COMPILE_MODE}/$${IM_PLUGIN}.dll \
                     $$(SYSTEMROOT)\Sysnative\msvcp140$${D}.dll \
                     $$(SYSTEMROOT)\Sysnative\vcruntime140$${D}.dll
    EXTRA_BINFILES ~= s,/,\\,g

    for( FILE, EXTRA_BINFILES ) {
        QMAKE_POST_LINK += $$quote( copy /y $${FILE} $${DESTDIR_WIN} $$CRLF )
    }
    message($$QMAKE_POST_LINK)
}

SOURCES += $${SRC_DIR}/ui/main.cpp
