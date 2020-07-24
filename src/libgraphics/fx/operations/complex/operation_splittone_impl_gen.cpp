
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

/// operation: splittone_GEN
void splittone_GEN(
    libgraphics::fxapi::ApiBackendDevice* device,
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
) {
    assert( device != nullptr );
    assert( destination != nullptr );
    assert( source != nullptr );

    const float highlights[3] = {
        brightR,
        brightG,
        brightB
    };
    const float shadows[3] = {
        darkR,
        darkG,
        darkB
    };


    std::unique_ptr<libgraphics::ImageLayer> highlightIntensityMap( makeImageLayer( device, destination ) );
    {
        libgraphics::fx::operations::multiply(
            highlightIntensityMap.get(),
            source,
            area,
            weight
        );
    }

    std::unique_ptr<libgraphics::ImageLayer> topLayer( makeImageLayer( device, destination ) );
    std::unique_ptr<libgraphics::ImageLayer> intermediateLayer( makeImageLayer( device, destination ) );
    {
        libgraphics::fx::operations::overlay(
            intermediateLayer.get(),
            source,
            area,
            libgraphics::makeRGBA32F(
                highlights[0], highlights[1], highlights[2], 1.0f
            )
        );

        libgraphics::fx::operations::multiply(
            topLayer.get(),
            intermediateLayer.get(),
            highlightIntensityMap.get(),
            area
        );
    }

    std::unique_ptr<libgraphics::ImageLayer> shadowsIntensityMap( makeImageLayer( device, destination ) );
    std::unique_ptr<libgraphics::ImageLayer> negatedIntensityLayer( makeImageLayer( device, destination ) );
    {
        libgraphics::fx::operations::negate(
            negatedIntensityLayer.get(),
            highlightIntensityMap.get(),
            area
        );

        libgraphics::fx::operations::multiply(
            shadowsIntensityMap.get(),
            negatedIntensityLayer.get(),
            negatedIntensityLayer.get(),
            area
        );
    }

    std::unique_ptr<libgraphics::ImageLayer> mediumLayer( makeImageLayer( device, destination ) );
    {
        std::unique_ptr<libgraphics::ImageLayer> intermediateLayer( makeImageLayer( device, destination ) );
        libgraphics::fx::operations::overlay(
            intermediateLayer.get(),
            source,
            area,
            libgraphics::makeRGBA32F(
                shadows[0], shadows[1], shadows[2], 1.0f
            )
        );

        libgraphics::fx::operations::multiply(
            mediumLayer.get(),
            intermediateLayer.get(),
            shadowsIntensityMap.get(),
            area
        );
    }

    std::unique_ptr<libgraphics::ImageLayer> bottomLayer( makeImageLayer( device, destination ) );
    {
        std::unique_ptr<libgraphics::ImageLayer> intensityRest( makeImageLayer( device, destination ) );
        libgraphics::fx::operations::subtract(
            intensityRest.get(),
            negatedIntensityLayer.get(),
            shadowsIntensityMap.get(),
            area
        );

        libgraphics::fx::operations::multiply(
            bottomLayer.get(),
            source,
            intensityRest.get(),
            area
        );
    }

    std::unique_ptr<libgraphics::ImageLayer> composedUpperLayer( makeImageLayer( device, destination ) );
    {
        libgraphics::fx::operations::add(
            composedUpperLayer.get(),
            topLayer.get(),
            mediumLayer.get(),
            area
        );
    }

    libgraphics::fx::operations::add(
        destination,
        composedUpperLayer.get(),
        bottomLayer.get(),
        area
    );
}

}
}
}
