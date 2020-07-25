
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

    std::unique_ptr<ImageLayer> grainOverlay( makeImageLayer( device, destination ) );
    std::unique_ptr<ImageLayer> weightedLayer( makeImageLayer( device, destination ) );
    {
        libgraphics::fx::operations::adjustBrightness(
            weightedLayer.get(),
            source,
            area,
            ( float* )curveData.data(),
            curveData.size()
        );
        libgraphics::fx::operations::overlay(
            grainOverlay.get(),
            source,
            grainLayer,
            area
        );
    }

    std::unique_ptr<ImageLayer> negatedWeightedLayer( makeImageLayer( device, destination ) );
    {
        libgraphics::fx::operations::negate(
            negatedWeightedLayer.get(),
            weightedLayer.get(),
            area
        );
    }

    std::unique_ptr<ImageLayer> upperLayer( makeImageLayer( device, destination ) );
    {
        libgraphics::fx::operations::multiply(
            upperLayer.get(),
            source,
            negatedWeightedLayer.get(),
            area
        );
    }

    std::unique_ptr<ImageLayer> lowerLayer( makeImageLayer( device, destination ) );
    {
        libgraphics::fx::operations::multiply(
            lowerLayer.get(),
            grainOverlay.get(),
            weightedLayer.get(),
            area
        );
    }

    libgraphics::fx::operations::add(
        destination,
        upperLayer.get(),
        lowerLayer.get(),
        area
    );


}

}
}
}
