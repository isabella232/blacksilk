#pragma once

#include <libgraphics/base.hpp>
#include <libgraphics/bitmap.hpp>
#include <libgraphics/fxapi.hpp>
#include <libgraphics/image.hpp>

namespace libgraphics {
namespace fx {
namespace operations {

void sampleWeightedSum2x2_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src,
    float       factor, /// between 0.001 and 0.999
    float*      weights
);
void sampleWeightedSum3x3_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src,
    float       factor, /// between 0.001 and 0.999
    float*      weights
);
void sampleWeightedSum4x4_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src,
    float       factor, /// between 0.001 and 0.999
    float*      weights
);
void sampleWeightedSum5x5_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src,
    float       factor, /// between 0.001 and 0.999
    float*      weights
);
void sampleWeightedSum6x6_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src,
    float       factor, /// between 0.001 and 0.999
    float*      weights
);


void bicubicSample_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src,
    float       factor, /// between 0.001 and 0.999
    float       lobe = -0.5f
);
void lanczosSample_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src,
    float       factor, /// between 0.001 and 0.999
    int         order = 3
);


/// based on mitchell netravali

/// B=C=1/3
void mitchellNetravaliSample_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src,
    float       factor /// between 0.001 and 0.999
);

/// B=1,C=0
void bicubicBSplineSample_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src,
    float       factor /// between 0.001 and 0.999
);

/// Catmull-Rom Spline
void catmullRomSample_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src,
    float       factor /// between 0.001 and 0.999
);


}
}
}
