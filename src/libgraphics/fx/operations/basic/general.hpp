#pragma once

#include <libgraphics/base.hpp>
#include <libgraphics/bitmap.hpp>
#include <libgraphics/fxapi.hpp>
#include <libgraphics/image.hpp>

namespace libgraphics {
namespace fx {
namespace operations {

void applyGrainSubtract(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area
);
void applyGrainAdd(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area
);

void grainMerge(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area
);
void grainMultiply(
    ImageLayer* dst,
    ImageLayer* src0,
    const libgraphics::Rect32I& area,
    float value
);
void grainMultiply(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area
);

void alphaBlend(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area,
    float opacity
); /// ignores the alpha channel
void alphaBlend(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area
);
void screen(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area
);
void overlay(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area
);
void overlay(
    ImageLayer* dst,
    ImageLayer* src,
    const libgraphics::Rect32I& area,
    const libgraphics::formats::RGBA32F::t& color
);
void dodge(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area
);
void burn(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area
);
void hardLight(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area
);
void grainExtract(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area
);
void difference(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area
);

void normalize(
    ImageLayer* dst,
    ImageLayer* src,
    const libgraphics::Rect32I& area
);

void minThreshold(
    ImageLayer* dst,
    ImageLayer* src,
    const libgraphics::Rect32I& area,
    float minimalValue
);
void maxThreshold(
    ImageLayer* dst,
    ImageLayer* src,
    const libgraphics::Rect32I& area,
    float maximalValue
);

void negate(
    ImageLayer* dst,
    ImageLayer* src,
    const libgraphics::Rect32I& area
);
void min(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area
);
void min(
    ImageLayer* dst,
    ImageLayer* src,
    const libgraphics::Rect32I area,
    const float& minimalChannelValue
);
void max(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area
);
void max(
    ImageLayer* dst,
    ImageLayer* src,
    const libgraphics::Rect32I area,
    const float& maxChannelValue
);

void add(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area
);
void add(
    ImageLayer* dst,
    ImageLayer* src,
    const libgraphics::Rect32I& area,
    float value
);
void add(
    ImageLayer* dst,
    ImageLayer* src,
    const libgraphics::Rect32I& area,
    const libgraphics::formats::RGBA32F::t& color
);

void subtract(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area
);
void subtract(
    ImageLayer* dst,
    ImageLayer* src,
    const libgraphics::Rect32I& area,
    float value
);
void subtract(
    ImageLayer* dst,
    ImageLayer* src,
    const libgraphics::Rect32I& area,
    const libgraphics::formats::RGBA32F::t& color
);

void multiply(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area
);
void multiply(
    ImageLayer* dst,
    ImageLayer* src,
    const libgraphics::Rect32I& area,
    float value
);
void multiply(
    ImageLayer* dst,
    ImageLayer* src,
    const libgraphics::Rect32I& area,
    const libgraphics::formats::RGBA32F::t& color
);

void divide(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area
);
void divide(
    ImageLayer* dst,
    ImageLayer* src,
    const libgraphics::Rect32I& area,
    float value
);
void divide(
    ImageLayer* dst,
    ImageLayer* src,
    const libgraphics::Rect32I& area,
    const libgraphics::formats::RGBA32F::t& color
);

/** fills the specified area
 */
void fill(
    ImageLayer* dst,
    libgraphics::Rect32I area,
    const libgraphics::Format& format,
    void* color
);
void fill(
    ImageLayer* dst,
    libgraphics::Rect32I area,
    int value
);
void fillChannel(
    ImageLayer* dst,
    libgraphics::Rect32I area,
    size_t channelIndex,
    unsigned char value
);
void fillChannel(
    ImageLayer* dst,
    libgraphics::Rect32I area,
    size_t channelIndex,
    unsigned short value
);
void fillChannel(
    ImageLayer* dst,
    libgraphics::Rect32I area,
    size_t channelIndex,
    unsigned int value
);
void fillChannel(
    ImageLayer* dst,
    libgraphics::Rect32I area,
    size_t channelIndex,
    float value
);

/** copies data from one to another
 * image layer */
void blit(
    ImageLayer* dst,
    ImageLayer* src,
    libgraphics::Rect32I sourceArea,
    libgraphics::Rect32I destArea
);

}
}
}
