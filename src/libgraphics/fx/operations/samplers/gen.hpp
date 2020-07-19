#pragma once

#include <libgraphics/base.hpp>
#include <libgraphics/bitmap.hpp>
#include <libgraphics/fxapi.hpp>
#include <libgraphics/image.hpp>


/** Info on downsampling:
 *      http://pixinsight.com/doc/docs/InterpolationAlgorithms/InterpolationAlgorithms.html
 *      http://http.developer.nvidia.com/GPUGems/gpugems_ch24.html
 */

namespace libgraphics {
namespace fx {
namespace operations {

void sampleWeightedSum2x2_GEN(
    libgraphics::fxapi::ApiBackendDevice* device,
    ImageLayer* dst,
    ImageLayer* src,
    float       factor, /// between 0.001 and 0.999
    float*      weights
);
void sampleWeightedSum3x3_GEN(
    libgraphics::fxapi::ApiBackendDevice* device,
    ImageLayer* dst,
    ImageLayer* src,
    float       factor, /// between 0.001 and 0.999
    float*      weights
);
void sampleWeightedSum4x4_GEN(
    libgraphics::fxapi::ApiBackendDevice* device,
    ImageLayer* dst,
    ImageLayer* src,
    float       factor, /// between 0.001 and 0.999
    float*      weights
);
void sampleWeightedSum5x5_GEN(
    libgraphics::fxapi::ApiBackendDevice* device,
    ImageLayer* dst,
    ImageLayer* src,
    float       factor, /// between 0.001 and 0.999
    float*      weights
);
void sampleWeightedSum6x6_GEN(
    libgraphics::fxapi::ApiBackendDevice* device,
    ImageLayer* dst,
    ImageLayer* src,
    float       factor, /// between 0.001 and 0.999
    float*      weights
);


void bicubicSample_GEN(
    libgraphics::fxapi::ApiBackendDevice* device,
    ImageLayer* dst,
    ImageLayer* src,
    float       factor, /// between 0.001 and 0.999
    float       lobe = -0.5f
);
void lanczosSample_GEN(
    libgraphics::fxapi::ApiBackendDevice* device,
    ImageLayer* dst,
    ImageLayer* src,
    float       factor, /// between 0.001 and 0.999
    int         order = 3
);


/// based on mitchell netravali

/// B=C=1/3
void mitchellNetravaliSample_GEN(
    libgraphics::fxapi::ApiBackendDevice* device,
    ImageLayer* dst,
    ImageLayer* src,
    float       factor /// between 0.001 and 0.999
);

/// B=1,C=0
void bicubicBSplineSample_GEN(
    libgraphics::fxapi::ApiBackendDevice* device,
    ImageLayer* dst,
    ImageLayer* src,
    float       factor /// between 0.001 and 0.999
);

/// Catmull-Rom Spline
void catmullRomSample_GEN(
    libgraphics::fxapi::ApiBackendDevice* device,
    ImageLayer* dst,
    ImageLayer* src,
    float       factor /// between 0.001 and 0.999
);

}
}
}
