
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


    std::unique_ptr<libgraphics::ImageLayer>   weightedLuma( makeImageLayer( device, source ) );
    std::unique_ptr<libgraphics::ImageLayer>   negatedWeightedLuma( makeImageLayer( device, source ) );
    {
        std::unique_ptr<libgraphics::ImageLayer>   temp( makeImageLayer( device, source ) );

        libgraphics::fx::operations::convertToMonochrome(
            temp.get(),
            source,
            area,
            ( float* )luma
        );

        libgraphics::fx::operations::add(
            weightedLuma.get(),
            temp.get(),
            area,
            weight
        );

        libgraphics::fx::operations::negate(
            negatedWeightedLuma.get(),
            weightedLuma.get(),
            area
        );
    }

    std::unique_ptr<libgraphics::ImageLayer>   weightedHighlights( makeImageLayer( device, destination ) );
    {
        std::unique_ptr<libgraphics::ImageLayer>   highlightsMonochrome( makeImageLayer( device, destination ) );

        libgraphics::fx::operations::convertToMonochrome(
            highlightsMonochrome.get(),
            source,
            area,
            ( float* )highlights
        );
        libgraphics::fx::operations::multiply(
            weightedHighlights.get(),
            highlightsMonochrome.get(),
            weightedLuma.get(),
            area
        );
    }

    std::unique_ptr<libgraphics::ImageLayer>   weightedShadows( makeImageLayer( device, destination ) );
    {
        std::unique_ptr<libgraphics::ImageLayer>   shadowMonochrome( makeImageLayer( device, destination ) );

        libgraphics::fx::operations::convertToMonochrome(
            shadowMonochrome.get(),
            source,
            area,
            ( float* )shadows
        );

        libgraphics::fx::operations::multiply(
            weightedShadows.get(),
            shadowMonochrome.get(),
            negatedWeightedLuma.get(),
            area
        );
    }

    libgraphics::fx::operations::add(
        destination,
        weightedHighlights.get(),
        weightedShadows.get(),
        area
    );
}

}
}
}
