
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

void cascadedSharpen_GEN(
    fxapi::ApiBackendDevice*    device,
    ImageLayer*                 destination,
    ImageLayer*                 source,
    Rect32I                     area,
    const std::vector< std::tuple< libgraphics::ImageLayer*, float, float > >& cascades,
    float threshold
) {
    std::unique_ptr<libgraphics::ImageLayer> frontBuffer( makeImageLayer( device, "default", source->format(), source->width(), source->height() ) );
    std::unique_ptr<libgraphics::ImageLayer> backBuffer( makeImageLayer( device, "default", source->format(), source->width(), source->height() ) );
    std::unique_ptr<libgraphics::ImageLayer> compositeBuffer( makeImageLayer( device, "default", source->format(), source->width(), source->height() ) );

    std::unique_ptr<libgraphics::ImageLayer> usmMapCurrent( makeImageLayer( device, "default", source->format(), source->width(), source->height() ) );
    std::unique_ptr<libgraphics::ImageLayer> usmMapLast( makeImageLayer( device, "default", source->format(), source->width(), source->height() ) );

    std::unique_ptr<libgraphics::ImageLayer> blurCompositeFront( makeImageLayer( device, "default", source->format(), source->width(), source->height() ) );
    std::unique_ptr<libgraphics::ImageLayer> blurCompositeBack( makeImageLayer( device, "default", source->format(), source->width(), source->height() ) );

    {

        libgraphics::fx::operations::fill(
            blurCompositeFront.get(),
            area,
            128
        );

        libgraphics::fx::operations::fill(
            blurCompositeBack.get(),
            area,
            128
        );

        libgraphics::fx::operations::fill(
            compositeBuffer.get(),
            area,
            128
        );

        libgraphics::fx::operations::fill(
            frontBuffer.get(),
            area,
            128
        );

        libgraphics::fx::operations::fill(
            backBuffer.get(),
            area,
            128
        );

        libgraphics::fx::operations::fill(
            usmMapCurrent.get(),
            area,
            128
        );

        libgraphics::fx::operations::fill(
            usmMapLast.get(),
            area,
            128
        );
    }


    for( size_t i = 0; cascades.size() > i; ++i ) {
        libgraphics::ImageLayer*    cascade( nullptr );
        float                       blurRadius( 0.0f );
        float                       strength( 0.0f );

        std::tie( cascade, blurRadius, strength ) = cascades[i];

        fx::operations::grainExtract(
            usmMapCurrent.get(),
            cascade,
            source,
            area
        );

        fx::operations::max(
            blurCompositeBack.get(),
            usmMapCurrent.get(),
            blurCompositeFront.get(),
            area
        );

        fx::operations::grainExtract(
            destination,
            usmMapCurrent.get(),
            usmMapLast.get(),
            area
        );

        fx::operations::grainMultiply(
            compositeBuffer.get(),
            destination,
            area,
            ( strength / 100.0f )
        );

        fx::operations::grainMerge(
            backBuffer.get(),
            frontBuffer.get(),
            compositeBuffer.get(),
            area
        );


        std::swap(
            frontBuffer,
            backBuffer
        );
        std::swap(
            usmMapLast,
            usmMapCurrent
        );
        std::swap(
            blurCompositeFront,
            blurCompositeBack
        );
    }

    /// resetting resources...
    {
        usmMapLast.reset();
        usmMapCurrent.reset();
    }

    /**

        float factor = 1.0/( Threshold + 0.05 ) * maxw;
        factor = min( factor, 1.0 );
        factor = max( factor, 0.0 );

        gl_FragColor   = factor * (basePixel - temp) + (1.0 - factor) * basePixel; */

    const float factor = 1.0f - std::max( threshold / 100.0f, 0.01f );//1.0f / ( ( this->m_Threshold / 100.0f ) + 0.05f );
    std::unique_ptr<libgraphics::ImageLayer> thresholdMap( makeImageLayer( device, "default", source->format(), source->width(), source->height() ) );
    {
        libgraphics::fx::operations::multiply(
            thresholdMap.get(),
            blurCompositeFront.get(),
            area,
            factor
        );
    }

    std::unique_ptr<libgraphics::ImageLayer> differenceMap( makeImageLayer( device, "default", source->format(), source->width(), source->height() ) );
    {
        libgraphics::fx::operations::grainExtract(
            differenceMap.get(),
            source,
            frontBuffer.get(),
            area
        );
    }

    std::unique_ptr<libgraphics::ImageLayer> basePixelDst( makeImageLayer( device, "default", source->format(), source->width(), source->height() ) );
    {
        libgraphics::fx::operations::multiply(
            basePixelDst.get(),
            differenceMap.get(),
            thresholdMap.get(),
            area
        );
    } /** ==> factor * (basePixel - temp) */

    std::unique_ptr<libgraphics::ImageLayer> negatedThresholdMap( makeImageLayer( device, "default", source->format(), source->width(), source->height() ) );
    {
        libgraphics::fx::operations::negate(
            negatedThresholdMap.get(),
            thresholdMap.get(),
            area
        );
    }

    std::unique_ptr<libgraphics::ImageLayer> negatedBasePixelDst( makeImageLayer( device, "default", source->format(), source->width(), source->height() ) );
    {
        libgraphics::fx::operations::multiply(
            negatedBasePixelDst.get(),
            source,
            negatedThresholdMap.get(),
            area
        );
    } // ==> (1.0 - factor) * basePixel;

    libgraphics::fx::operations::add(
        destination,
        basePixelDst.get(),
        negatedBasePixelDst.get(),
        area
    );
}

}
}
}
