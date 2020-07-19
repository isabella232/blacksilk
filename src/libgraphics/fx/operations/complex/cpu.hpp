#pragma once

#include <libgraphics/base.hpp>
#include <libgraphics/bitmap.hpp>
#include <libgraphics/fxapi.hpp>
#include <libgraphics/image.hpp>

namespace libgraphics {
namespace fx {
namespace operations {

/// operation: applyVignette
void applyVignette_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src,
    Rect32I area,
    const Point32F& center,
    float radius,
    float strength
);

/// operation: gaussianBlur
/**
 *  renders a gaussian blur.
 */
void gaussianBlur_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* destination,
    libgraphics::fxapi::ApiImageObject* source,
    Rect32I area,
    float radius
);

/**
 *  renders a vertical gaussian
 *  blur.
 */
void verticalGaussianBlur_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* destination,
    libgraphics::fxapi::ApiImageObject* source,
    Rect32I area,
    float radius
);

/**
 *  renders a horizontal gaussian
 *  blur.
 */
void horizontalGaussianBlur_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* destination,
    libgraphics::fxapi::ApiImageObject* source,
    Rect32I area,
    float radius
);

/// operation: adaptiveBwMixer
void adaptiveBWMixer_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* destination,
    libgraphics::fxapi::ApiImageObject* source,
    libgraphics::Rect32I   area,
    float brightR,
    float brightG,
    float brightB,
    float darkR,
    float darkG,
    float darkB,
    float weight
);

/// operation: adaptiveBwMixer
void splittone_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* destination,
    libgraphics::fxapi::ApiImageObject* source,
    libgraphics::Rect32I   area,
    float brightR,
    float brightG,
    float brightB,
    float darkR,
    float darkG,
    float darkB,
    float weight
);

/// operation: filmgrain
void filmgrain_CPU(
    libgraphics::fxapi::ApiBackendDevice*   device,
    libgraphics::fxapi::ApiImageObject*     destination,
    libgraphics::fxapi::ApiImageObject*     source,
    Rect32I                                 area,
    libgraphics::fxapi::ApiImageObject*     grainLayer,
    const std::vector<float>&               curveData,
    bool                                    isMonoGrain
);

/// operation: cascadedSharpenWith4
void cascadedSharpenWith4_CPU(
    libgraphics::fxapi::ApiBackendDevice*   device,
    libgraphics::fxapi::ApiImageObject*     destination,
    libgraphics::fxapi::ApiImageObject*     source,
    Rect32I                                 area,
    const std::tuple< libgraphics::ImageLayer*, float, float >& cascade0,
    const std::tuple< libgraphics::ImageLayer*, float, float >& cascade1,
    const std::tuple< libgraphics::ImageLayer*, float, float >& cascade2,
    const std::tuple< libgraphics::ImageLayer*, float, float >& cascade3,
    float threshold
);
}
}
}
