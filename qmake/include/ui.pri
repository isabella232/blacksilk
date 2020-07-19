HEADERS +=  \
            $${SRC_DIR}/ui/mixerwidget.hpp \
            $${SRC_DIR}/ui/curveswidget.hpp \
            $${SRC_DIR}/ui/sharpenwidget.hpp \
            $${SRC_DIR}/ui/vignettewidget.hpp \
            $${SRC_DIR}/ui/grainwidget.hpp \
            $${SRC_DIR}/ui/splittonewidget.hpp \
            \
            $${SRC_DIR}/ui/filterwidget.h \
            $${SRC_DIR}/ui/about.h \
            $${SRC_DIR}/ui/mainwindow.h \
            $${SRC_DIR}/ui/glwidget.h \
            $${SRC_DIR}/ui/yuvframe.h \
            $${SRC_DIR}/ui/vframe.h \
            $${SRC_DIR}/ui/curveframe.h \
            $${SRC_DIR}/ui/finishgroup.h \
            $${SRC_DIR}/ui/grainpreview.h \
            $${SRC_DIR}/ui/filedealer.h \
            $${SRC_DIR}/ui/pointlessbar.hpp \
            $${SRC_DIR}/ui/filterundocommand.hpp \
            $${SRC_DIR}/ui/actionundocommand.hpp \
            $${SRC_DIR}/ui/presetundocommand.hpp \
            $${SRC_DIR}/ui/slider.hpp \
            $${SRC_DIR}/ui/spinbox.hpp \
            $${SRC_DIR}/ui/doublespinbox.hpp


SOURCES +=  \
            $${SRC_DIR}/ui/mixerwidget.cpp \
            $${SRC_DIR}/ui/curveswidget.cpp \
            $${SRC_DIR}/ui/sharpenwidget.cpp \
            $${SRC_DIR}/ui/vignettewidget.cpp \
            $${SRC_DIR}/ui/grainwidget.cpp \
            $${SRC_DIR}/ui/splittonewidget.cpp \
            \
            $${SRC_DIR}/ui/filterwidget.cpp \
            $${SRC_DIR}/ui/about.cpp \
            $${SRC_DIR}/ui/mainwindow.cpp \
            $${SRC_DIR}/ui/glwidget.cpp \
            $${SRC_DIR}/ui/yuvframe.cpp \
            $${SRC_DIR}/ui/vframe.cpp \
            $${SRC_DIR}/ui/curveframe.cpp \
            $${SRC_DIR}/ui/filedealer.cpp \
            $${SRC_DIR}/ui/finishgroup.cpp \
            $${SRC_DIR}/ui/grainpreview.cpp \
            $${SRC_DIR}/ui/pointlessbar.cpp \
            $${SRC_DIR}/ui/filterundocommand.cpp \
            $${SRC_DIR}/ui/actionundocommand.cpp \
            $${SRC_DIR}/ui/presetundocommand.cpp \
            $${SRC_DIR}/ui/slider.cpp \
            $${SRC_DIR}/ui/spinbox.cpp \
            $${SRC_DIR}/ui/doublespinbox.cpp


FORMS +=    \
            $${SRC_DIR}/ui/mixerwidget.ui \
            $${SRC_DIR}/ui/curveswidget.ui \
            $${SRC_DIR}/ui/sharpenwidget.ui \
            $${SRC_DIR}/ui/vignettewidget.ui \
            $${SRC_DIR}/ui/grainwidget.ui \
            $${SRC_DIR}/ui/splittonewidget.ui \
            \
            $${SRC_DIR}/ui/about.ui \
            $${SRC_DIR}/ui/mainwindow.ui \
            $${SRC_DIR}/ui/finishgroup.ui

win32 {
    SOURCES += $${SRC_DIR}/ui/win/wineventfilter.cpp
    HEADERS += $${SRC_DIR}/ui/win/wineventfilter.hpp
}


