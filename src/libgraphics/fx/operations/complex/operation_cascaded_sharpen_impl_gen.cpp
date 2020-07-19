
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
    libcommon::ScopedPtr<libgraphics::ImageLayer> frontBuffer( makeImageLayer( device, "default", source->format(), source->width(), source->height() ) );
    libcommon::ScopedPtr<libgraphics::ImageLayer> backBuffer( makeImageLayer( device, "default", source->format(), source->width(), source->height() ) );
    libcommon::ScopedPtr<libgraphics::ImageLayer> compositeBuffer( makeImageLayer( device, "default", source->format(), source->width(), source->height() ) );

    libcommon::ScopedPtr<libgraphics::ImageLayer> usmMapCurrent( makeImageLayer( device, "default", source->format(), source->width(), source->height() ) );
    libcommon::ScopedPtr<libgraphics::ImageLayer> usmMapLast( makeImageLayer( device, "default", source->format(), source->width(), source->height() ) );

    libcommon::ScopedPtr<libgraphics::ImageLayer> blurCompositeFront( makeImageLayer( device, "default", source->format(), source->width(), source->height() ) );
    libcommon::ScopedPtr<libgraphics::ImageLayer> blurCompositeBack( makeImageLayer( device, "default", source->format(), source->width(), source->height() ) );

    {

        libgraphics::fx::operations::fill(
            blurCompositeFront,
            area,
            128
        );

        libgraphics::fx::operations::fill(
            blurCompositeBack,
            area,
            128
        );

        libgraphics::fx::operations::fill(
            compositeBuffer,
            area,
            128
        );

        libgraphics::fx::operations::fill(
            frontBuffer,
            area,
            128
        );

        libgraphics::fx::operations::fill(
            backBuffer,
            area,
            128
        );

        libgraphics::fx::operations::fill(
            usmMapCurrent,
            area,
            128
        );

        libgraphics::fx::operations::fill(
            usmMapLast,
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
            usmMapCurrent,
            cascade,
            source,
            area
        );

        fx::operations::max(
            blurCompositeBack,
            usmMapCurrent,
            blurCompositeFront,
            area
        );

        fx::operations::grainExtract(
            destination,
            usmMapCurrent,
            usmMapLast,
            area
        );

        fx::operations::grainMultiply(
            compositeBuffer,
            destination,
            area,
            ( strength / 100.0f )
        );

        fx::operations::grainMerge(
            backBuffer,
            frontBuffer,
            compositeBuffer,
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
    libcommon::ScopedPtr<libgraphics::ImageLayer> thresholdMap( makeImageLayer( device, "default", source->format(), source->width(), source->height() ) );
    {
        libgraphics::fx::operations::multiply(
            thresholdMap,
            blurCompositeFront,
            area,
            factor
        );
    }

    libcommon::ScopedPtr<libgraphics::ImageLayer> differenceMap( makeImageLayer( device, "default", source->format(), source->width(), source->height() ) );
    {
        libgraphics::fx::operations::grainExtract(
            differenceMap,
            source,
            frontBuffer,
            area
        );
    }

    libcommon::ScopedPtr<libgraphics::ImageLayer> basePixelDst( makeImageLayer( device, "default", source->format(), source->width(), source->height() ) );
    {
        libgraphics::fx::operations::multiply(
            basePixelDst,
            differenceMap,
            thresholdMap,
            area
        );
    } /** ==> factor * (basePixel - temp) */

    libcommon::ScopedPtr<libgraphics::ImageLayer> negatedThresholdMap( makeImageLayer( device, "default", source->format(), source->width(), source->height() ) );
    {
        libgraphics::fx::operations::negate(
            negatedThresholdMap,
            thresholdMap,
            area
        );
    }

    libcommon::ScopedPtr<libgraphics::ImageLayer> negatedBasePixelDst( makeImageLayer( device, "default", source->format(), source->width(), source->height() ) );
    {
        libgraphics::fx::operations::multiply(
            negatedBasePixelDst,
            source,
            negatedThresholdMap,
            area
        );
    } // ==> (1.0 - factor) * basePixel;

    libgraphics::fx::operations::add(
        destination,
        basePixelDst,
        negatedBasePixelDst,
        area
    );
}

}
}
}
