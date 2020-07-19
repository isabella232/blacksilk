
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
) {
    assert( device != nullptr );
    assert( destination != nullptr );
    assert( source != nullptr );

    const float luma[3] = {
        0.299f,
        0.587f,
        0.114f
    };
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


    libcommon::ScopedPtr<libgraphics::ImageLayer>   weightedLuma( makeImageLayer( device, source ) );
    libcommon::ScopedPtr<libgraphics::ImageLayer>   negatedWeightedLuma( makeImageLayer( device, source ) );
    {
        libcommon::ScopedPtr<libgraphics::ImageLayer>   temp( makeImageLayer( device, source ) );

        libgraphics::fx::operations::convertToMonochrome(
            temp,
            source,
            area,
            ( float* )luma
        );

        libgraphics::fx::operations::add(
            weightedLuma,
            temp,
            area,
            weight
        );

        libgraphics::fx::operations::negate(
            negatedWeightedLuma,
            weightedLuma,
            area
        );
    }

    libcommon::ScopedPtr<libgraphics::ImageLayer>   weightedHighlights( makeImageLayer( device, destination ) );
    {
        libcommon::ScopedPtr<libgraphics::ImageLayer>   highlightsMonochrome( makeImageLayer( device, destination ) );

        libgraphics::fx::operations::convertToMonochrome(
            highlightsMonochrome,
            source,
            area,
            ( float* )highlights
        );
        libgraphics::fx::operations::multiply(
            weightedHighlights,
            highlightsMonochrome,
            weightedLuma,
            area
        );
    }

    libcommon::ScopedPtr<libgraphics::ImageLayer>   weightedShadows( makeImageLayer( device, destination ) );
    {
        libcommon::ScopedPtr<libgraphics::ImageLayer>   shadowMonochrome( makeImageLayer( device, destination ) );

        libgraphics::fx::operations::convertToMonochrome(
            shadowMonochrome,
            source,
            area,
            ( float* )shadows
        );

        libgraphics::fx::operations::multiply(
            weightedShadows,
            shadowMonochrome,
            negatedWeightedLuma,
            area
        );
    }

    libgraphics::fx::operations::add(
        destination,
        weightedHighlights,
        weightedShadows,
        area
    );
}

}
}
}
