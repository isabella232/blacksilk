#pragma once

#include <libgraphics/base.hpp>
#include <libgraphics/bitmap.hpp>
#include <libgraphics/fxapi.hpp>

namespace libgraphics {
namespace backend {

libgraphics::Format    fromCompatibleFormat( fxapi::EPixelFormat::t format );
fxapi::EPixelFormat::t toCompatibleFormat( libgraphics::Format format );
bool isCompatibleFormat( libgraphics::Format format );
bool isCompatibleFormat( libgraphics::Bitmap* bitmap );

}
}
