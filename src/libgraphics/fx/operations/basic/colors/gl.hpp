#pragma once

#include <libgraphics/base.hpp>
#include <libgraphics/bitmap.hpp>
#include <libgraphics/fxapi.hpp>
#include <libgraphics/image.hpp>

namespace libgraphics {
namespace fx {
namespace operations {

/** adjusts the brightness of the specified
 *  image layer
*/
void adjustBrightness_GL(
    fxapi::ApiBackendDevice* device,
    fxapi::ApiImageObject* dst,
    fxapi::ApiImageObject* src,
    libgraphics::Rect32I area,
    float value
);
void adjustBrightness_GL(
    fxapi::ApiBackendDevice* device,
    fxapi::ApiImageObject* dst,
    fxapi::ApiImageObject* src,
    Rect32I area,
    float* values,
    size_t length
);

/// operation: convertToMonochrome
void convertToMonochrome_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* destination,
    libgraphics::fxapi::ApiImageObject* source,
    libgraphics::Rect32I   area,
    float* channelFactors
);
void convertToMonochrome_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    Rect32I area
);


}
}
}
