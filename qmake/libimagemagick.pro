include( include/libimagemagick.pri )

# libgraphics
HEADERS += \
    $${SRC_DIR}/libgraphics/bitmap.hpp \
    $${SRC_DIR}/libgraphics/base.hpp
SOURCES += \
    $${SRC_DIR}/libgraphics/gfx_bitmap.cpp \
    $${SRC_DIR}/libgraphics/gfx_base.cpp

# libgraphics.io
HEADERS += \
    $${SRC_DIR}/libgraphics/io/pipeline.hpp \
    $${SRC_DIR}/libgraphics/io/pipelineexporter.hpp \
    $${SRC_DIR}/libgraphics/io/pipelineimporter.hpp \
    $${SRC_DIR}/libgraphics/io/pipelineinfo.hpp \
    $${SRC_DIR}/libgraphics/io/pipelineplugin.hpp \
    $${SRC_DIR}/libgraphics/io/pipelineplugininfo.hpp \
#    $${SRC_DIR}/libgraphics/io/pipelinepluginloader.hpp \
    $${SRC_DIR}/libgraphics/io/pipelineprocessingstage.hpp \
    $${SRC_DIR}/libgraphics/io/pipelineobject.hpp \
    $${SRC_DIR}/libgraphics/io/pipelineobjectgroup.hpp \
    $${SRC_DIR}/libgraphics/io/pipelineprocessor.hpp

SOURCES += \
    $${SRC_DIR}/libgraphics/io/io_pipeline.cpp \
    $${SRC_DIR}/libgraphics/io/io_pipelineexporter.cpp \
    $${SRC_DIR}/libgraphics/io/io_pipelineimporter.cpp \
    $${SRC_DIR}/libgraphics/io/io_pipelineinfo.cpp \
    $${SRC_DIR}/libgraphics/io/io_pipelineplugin.cpp \
    $${SRC_DIR}/libgraphics/io/io_pipelineplugininfo.cpp \
#    $${SRC_DIR}/libgraphics/io/io_pipelinepluginloader.cpp \
    $${SRC_DIR}/libgraphics/io/io_pipelineprocessingstage.cpp \
    $${SRC_DIR}/libgraphics/io/io_pipelineprocessor.cpp


# libgraphics.io.plugins.imagemagick
HEADERS +=  \
    $${SRC_DIR}/libgraphics/io/plugins/imagemagick/pluginmain.hpp \
    $${SRC_DIR}/libgraphics/io/plugins/imagemagick/importer.hpp \
    $${SRC_DIR}/libgraphics/io/plugins/imagemagick/exporter.hpp

SOURCES +=  \
    $${SRC_DIR}/libgraphics/io/plugins/imagemagick/pluginmain.cpp \
    $${SRC_DIR}/libgraphics/io/plugins/imagemagick/importer.cpp \
    $${SRC_DIR}/libgraphics/io/plugins/imagemagick/exporter.cpp
