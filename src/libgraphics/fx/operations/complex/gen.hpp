#pragma once

#include <libgraphics/base.hpp>
#include <libgraphics/bitmap.hpp>
#include <libgraphics/fxapi.hpp>
#include <libgraphics/image.hpp>

namespace libgraphics {
namespace fx {
namespace operations {

/// operation: adaptiveBwMixer
void adaptiveBWMixer_GEN(
    fxapi::ApiBackendDevice* device,
    ImageLayer* destination,
    ImageLayer* source,
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
void splittone_GEN(
    fxapi::ApiBackendDevice* device,
    ImageLayer* destination,
    ImageLayer* source,
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
void filmgrain_GEN(
    fxapi::ApiBackendDevice* device,
    ImageLayer*                 dst,
    ImageLayer*                 src,
    Rect32I                     area,
    ImageLayer*                 grainLayer,
    const std::vector<float>&   curveData,
    bool                        isMonoGrain
);

/// operation: cascadedSharpen
void cascadedSharpen_GEN(
    fxapi::ApiBackendDevice* device,
    ImageLayer*                 dst,
    ImageLayer*                 src,
    Rect32I                     area,
    const std::vector< std::tuple< libgraphics::ImageLayer*, float, float > >& cascades,
    float threshold
);


}
}
}
