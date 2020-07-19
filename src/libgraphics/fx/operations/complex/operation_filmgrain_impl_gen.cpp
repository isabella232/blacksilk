
#include <assert.h>
#include <QDebug>
#include <math.h>

#include <libgraphics/image.hpp>

#include <libgraphics/fx/operations/basic.hpp>
#include <libgraphics/fx/operations/complex.hpp>
#include <libgraphics/fx/operations/complex/gen.hpp>

namespace libgraphics {
namespace fx {
namespace operations {

void filmgrain_GEN(
    fxapi::ApiBackendDevice* device,
    ImageLayer*                 destination,
    ImageLayer*                 source,
    Rect32I                     area,
    ImageLayer*                 grainLayer,
    const std::vector<float>&   curveData,
    bool                        isMonoGrain
) {
    ( void )isMonoGrain;

    libcommon::ScopedPtr<ImageLayer> grainOverlay( makeImageLayer( device, destination ) );
    libcommon::ScopedPtr<ImageLayer> weightedLayer( makeImageLayer( device, destination ) );
    {
        libgraphics::fx::operations::adjustBrightness(
            weightedLayer,
            source,
            area,
            ( float* )curveData.data(),
            curveData.size()
        );
        libgraphics::fx::operations::overlay(
            grainOverlay,
            source,
            grainLayer,
            area
        );
    }

    libcommon::ScopedPtr<ImageLayer> negatedWeightedLayer( makeImageLayer( device, destination ) );
    {
        libgraphics::fx::operations::negate(
            negatedWeightedLayer,
            weightedLayer,
            area
        );
    }

    libcommon::ScopedPtr<ImageLayer> upperLayer( makeImageLayer( device, destination ) );
    {
        libgraphics::fx::operations::multiply(
            upperLayer,
            source,
            negatedWeightedLayer,
            area
        );
    }

    libcommon::ScopedPtr<ImageLayer> lowerLayer( makeImageLayer( device, destination ) );
    {
        libgraphics::fx::operations::multiply(
            lowerLayer,
            grainOverlay,
            weightedLayer,
            area
        );
    }

    libgraphics::fx::operations::add(
        destination,
        upperLayer,
        lowerLayer,
        area
    );


}

}
}
}
