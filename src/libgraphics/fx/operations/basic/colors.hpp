#pragma once

#include <libgraphics/base.hpp>
#include <libgraphics/bitmap.hpp>
#include <libgraphics/fxapi.hpp>
#include <libgraphics/image.hpp>
#include <libgraphics/curve.hpp>

namespace libgraphics {
namespace fx {
namespace operations {

/** adjusts the brightness of the specified
 *  image layer
*/
void adjustBrightness(
    ImageLayer* dst,
    ImageLayer* src,
    libgraphics::Rect32I area,
    float value
);
void adjustBrightness(
    ImageLayer* dst,
    ImageLayer* src,
    Rect32I area,
    float* values,
    size_t length
);

/**
 *  converts the colored image to a monochrome
 *  image.
 */
void convertToMonochrome(
    ImageLayer* dst,
    ImageLayer* src,
    Rect32I area,
    float* channelFactors
);
void convertToMonochrome(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    Rect32I area
);
void convertToMonochrome(
    ImageLayer* dst,
    ImageLayer* src,
    Rect32I area,
    float* brightColorFactors,
    float* darkColorFactors
);

}
}
}
