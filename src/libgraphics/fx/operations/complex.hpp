#pragma once

#include <libgraphics/base.hpp>
#include <libgraphics/bitmap.hpp>
#include <libgraphics/fxapi.hpp>
#include <libgraphics/image.hpp>

namespace libgraphics {
namespace fx {
namespace operations {

void applyVignette(
    fxapi::ApiBackendDevice* backend,
    ImageLayer* dst,
    ImageLayer* src,
    Rect32I area,
    const Point32F& center,
    float radius,
    float strength
);


/**
 *  renders a gaussian blur.
 */
void gaussianBlur(
    fxapi::ApiBackendDevice* backend,
    ImageLayer* dst,
    ImageLayer* src,
    Rect32I area,
    float radius
);

/**
 *  renders a vertical gaussian
 *  blur.
 */
void verticalGaussianBlur(
    fxapi::ApiBackendDevice* backend,
    ImageLayer* dst,
    ImageLayer* src,
    Rect32I area,
    float radius
);

/**
 *  renders a horizontal gaussian
 *  blur.
 */
void horizontalGaussianBlur(
    fxapi::ApiBackendDevice* backend,
    ImageLayer* dst,
    ImageLayer* src,
    Rect32I area,
    float radius
);


/// operation: adaptiveBwMixer
void adaptiveBWMixer(
    fxapi::ApiBackendDevice* backend,
    ImageLayer* dst,
    ImageLayer* src,
    Rect32I area,
    float brightR,
    float brightG,
    float brightB,
    float darkR,
    float darkG,
    float darkB,
    float weight
);

/// operation: splittone
void splittone(
    fxapi::ApiBackendDevice* backend,
    ImageLayer* dst,
    ImageLayer* src,
    Rect32I area,
    float brightR,
    float brightG,
    float brightB,
    float darkR,
    float darkG,
    float darkB,
    float weight
);

/// operation: filmgrain
void filmgrain(
    fxapi::ApiBackendDevice* backend,
    ImageLayer*                 dst,
    ImageLayer*                 src,
    Rect32I                     area,
    ImageLayer*                 grainLayer,
    const std::vector<float>&   curveData,
    bool                        isMonoGrain
);

/// operation: cascadedSharpen
void cascadedSharpen(
    fxapi::ApiBackendDevice* backend,
    ImageLayer*                 dst,
    ImageLayer*                 src,
    Rect32I                     area,
    const std::vector< std::tuple< libgraphics::ImageLayer*, float, float > >& cascades,
    float threshold
);

}
}
}
