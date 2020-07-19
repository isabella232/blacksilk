
INCLUDEPATH += $${MAIN_DIR}/libs/imagemagick/include

MAGICK_DIR=$${MAIN_DIR}/libs/imagemagick/bin/$${COMPILE_MODE}
LIBS += -L$${MAGICK_DIR}
DEFINES += 'MAGICKCORE_HDRI_ENABLE=0'
DEFINES += 'MAGICKCORE_QUANTUM_DEPTH=16'

mac {
    # brew install little-cms2 freetype
    INCLUDEPATH += /usr/local/include
    LIBS += -L/usr/local/lib
    LIBS += $${MAGICK_DIR}/libMagick++-7.Q16.a \
            $${MAGICK_DIR}/libMagickWand-7.Q16.a \
            $${MAGICK_DIR}/libMagickCore-7.Q16.a \
            -llcms2 -ltiff -lfreetype -ljpeg -lbz2 \
            -lxml2 -lm -lpthread -llzma -lz
    LIBS += -lpng16
}

linux {
    LIBS += -L/usr/lib
    LIBS += $${MAGICK_DIR}/libMagick++-7.Q16.so \
            $${MAGICK_DIR}/libMagickWand-7.Q16.so \
            $${MAGICK_DIR}/libMagickCore-7.Q16.so \
            -llcms2 -ltiff -lfreetype -ljpeg -llzma \
            -lbz2 -lxml2 -lz -lm -lpthread -lltdl -lfontconfig
    LIBS += -lpng16 -lX11 -lXext
}

win32 {
    DEFINES += STATIC_MAGICK
    LIBS += /LIBPATH:$${MAGICK_DIR}
    LIBS += User32.lib Advapi32.lib OleAut32.lib Gdi32.lib Opengl32.lib
    LIBS += -lCORE_$${IM_FLAG}_coders_ \
            -lCORE_$${IM_FLAG}_ffi_ \
            -lCORE_$${IM_FLAG}_filters_ \
            -lCORE_$${IM_FLAG}_jpeg_ \
            -lCORE_$${IM_FLAG}_lcms_ \
            -lCORE_$${IM_FLAG}_libxml_ \
            -lCORE_$${IM_FLAG}_MagickCore_ \
            -lCORE_$${IM_FLAG}_Magick++_ \
            -lCORE_$${IM_FLAG}_png_ \
            -lCORE_$${IM_FLAG}_tiff_ \
            -lCORE_$${IM_FLAG}_MagickWand_ \
            -lCORE_$${IM_FLAG}_zlib_
}

