#pragma once

#include <vector>
#include <iterator>

#include <libgraphics/base.hpp>
#include <libgraphics/bitmap.hpp>
#include <libgraphics/image.hpp>
#include <libgraphics/io/pipeline.hpp>
#include <libcommon/noncopyable.hpp>
#include <map>

#include <QDebug>

namespace libgraphics {
struct Image::Private {
    std::vector<libcommon::SharedPtr<libgraphics::ImageLayer> >     layers;
    ImageMetaInfo   info;

    size_t  width;
    size_t  height;
    fxapi::EPixelFormat::t  format;

    Private() : width( 0 ), height( 0 ),
        format( fxapi::EPixelFormat::Empty ) {}
    ~Private() {
        layers.clear();
        info.clearDirectories();
    }
};
}
