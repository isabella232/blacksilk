#pragma once

#include <libgraphics/base.hpp>
#include <libgraphics/bitmap.hpp>
#include <libgraphics/fxapi.hpp>
#include <libgraphics/image.hpp>

namespace libgraphics {
namespace fx {
namespace operations {

/// operation: applyGrainSubtract
void applyGrainSubtract_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
);

/// operation: applyGrainAdd
void applyGrainAdd_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
);

/// operation: grainMultiply
void grainMultiply_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area,
    float value
);
void grainMultiply_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
);

/// operation: normalize
void normalize_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src,
    const libgraphics::Rect32I& area
);

/// operation: minThreshold
void minThreshold_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area,
    float minimalValue
);
void maxThreshold_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area,
    float maximalValue
);

/// operation: negate
void negate_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area
);

/// operation: min
void min_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
);
void min_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I area,
    const float& minimalChannelValue
);

/// operation: max
void max_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
);
void max_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I area,
    const float& maxChannelValue
);

/// operation: add
void add_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
);
void add_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area,
    float value
);
void add_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area,
    const libgraphics::formats::RGBA32F::t& color
);

/// operation: subtract
void subtract_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
);
void subtract_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area,
    float value
);
void subtract_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area,
    const libgraphics::formats::RGBA32F::t& color
);

/// operation: multiply
void multiply_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
);
void multiply_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area,
    float value
);
void multiply_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area,
    const libgraphics::formats::RGBA32F::t& color
);

/// operation: divide
void divide_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
);
void divide_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area,
    float value
);
void divide_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area,
    const libgraphics::formats::RGBA32F::t& color
);

/// operation: fillArea_GL
void fill_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::Rect32I area,
    const libgraphics::Format& format,
    void* color
);
void fill_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::Rect32I area,
    int value
);
void fillChannel_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::Rect32I area,
    size_t channelIndex,
    unsigned char value
);
void fillChannel_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::Rect32I area,
    size_t channelIndex,
    unsigned short value
);
void fillChannel_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::Rect32I area,
    size_t channelIndex,
    unsigned int value
);
void fillChannel_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::Rect32I area,
    size_t channelIndex,
    float value
);

/// operation: blit
void blit_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject*     destination,
    libgraphics::fxapi::ApiImageObject*     source,
    libgraphics::Rect32I sourceArea,
    libgraphics::Rect32I destArea
);

/// operation: adjustBrightness
void adjustBrightness_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject*     destination,
    libgraphics::fxapi::ApiImageObject*     source,
    libgraphics::Rect32I   area,
    float* curveData,
    size_t curveLength
);
void adjustBrightness_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject*     destination,
    libgraphics::fxapi::ApiImageObject*     source,
    libgraphics::Rect32I   area,
    float adjustment
);

/// operation: gimp layer modes

void grainMerge_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
);
void alphaBlend_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area,
    float opacity
); /// ignores the alpha channel
void alphaBlend_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
);
void screen_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
);
void overlay_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
);
void overlay_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area,
    const libgraphics::formats::RGBA32F::t& color
);
void dodge_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
);
void burn_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
);
void hardLight_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
);
void softLight_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
);
void grainExtract_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
);
void difference_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
);

}
}
}
