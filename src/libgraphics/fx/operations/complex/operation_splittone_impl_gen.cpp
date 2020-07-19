
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


    libcommon::ScopedPtr<libgraphics::ImageLayer> highlightIntensityMap( makeImageLayer( device, destination ) );
    {
        libgraphics::fx::operations::multiply(
            highlightIntensityMap,
            source,
            area,
            weight
        );
    }

    libcommon::ScopedPtr<libgraphics::ImageLayer> topLayer( makeImageLayer( device, destination ) );
    libcommon::ScopedPtr<libgraphics::ImageLayer> intermediateLayer( makeImageLayer( device, destination ) );
    {
        libgraphics::fx::operations::overlay(
            intermediateLayer,
            source,
            area,
            libgraphics::makeRGBA32F(
                highlights[0], highlights[1], highlights[2], 1.0f
            )
        );

        libgraphics::fx::operations::multiply(
            topLayer,
            intermediateLayer,
            highlightIntensityMap,
            area
        );
    }

    libcommon::ScopedPtr<libgraphics::ImageLayer> shadowsIntensityMap( makeImageLayer( device, destination ) );
    libcommon::ScopedPtr<libgraphics::ImageLayer> negatedIntensityLayer( makeImageLayer( device, destination ) );
    {
        libgraphics::fx::operations::negate(
            negatedIntensityLayer,
            highlightIntensityMap,
            area
        );

        libgraphics::fx::operations::multiply(
            shadowsIntensityMap,
            negatedIntensityLayer,
            negatedIntensityLayer,
            area
        );
    }

    libcommon::ScopedPtr<libgraphics::ImageLayer> mediumLayer( makeImageLayer( device, destination ) );
    {
        libcommon::ScopedPtr<libgraphics::ImageLayer> intermediateLayer( makeImageLayer( device, destination ) );
        libgraphics::fx::operations::overlay(
            intermediateLayer,
            source,
            area,
            libgraphics::makeRGBA32F(
                shadows[0], shadows[1], shadows[2], 1.0f
            )
        );

        libgraphics::fx::operations::multiply(
            mediumLayer,
            intermediateLayer,
            shadowsIntensityMap,
            area
        );
    }

    libcommon::ScopedPtr<libgraphics::ImageLayer> bottomLayer( makeImageLayer( device, destination ) );
    {
        libcommon::ScopedPtr<libgraphics::ImageLayer> intensityRest( makeImageLayer( device, destination ) );
        libgraphics::fx::operations::subtract(
            intensityRest,
            negatedIntensityLayer,
            shadowsIntensityMap,
            area
        );

        libgraphics::fx::operations::multiply(
            bottomLayer,
            source,
            intensityRest,
            area
        );
    }

    libcommon::ScopedPtr<libgraphics::ImageLayer> composedUpperLayer( makeImageLayer( device, destination ) );
    {
        libgraphics::fx::operations::add(
            composedUpperLayer,
            topLayer,
            mediumLayer,
            area
        );
    }

    libgraphics::fx::operations::add(
        destination,
        composedUpperLayer,
        bottomLayer,
        area
    );
}

}
}
}
