
#include <libgraphics/fx/operations/basic/colors/gl.hpp>
#include <libgraphics/fx/operations/basic/colors/cpu.hpp>

#include <QDebug>

namespace libgraphics {
namespace fx {
namespace operations {

void convertToMonochrome(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    Rect32I area
) {
    assert( dst );
    assert( src0 );
    assert( src1 );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src0->containsDataForBackend( FXAPI_BACKEND_CPU ) && src1->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        convertToMonochrome_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src0->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src1->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src0->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src1->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        assert( false );
        convertToMonochrome_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src0->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src1->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void convertToMonochrome(
    ImageLayer* dst,
    ImageLayer* src,
    Rect32I area,
    float* channelFactors
) {
    assert( dst );
    assert( dst->width() >= area.width + area.x );
    assert( dst->height() >= area.height + area.y );
    assert( src );
    assert( channelFactors );

    if( !dst || !channelFactors ) {
        return;
    }

    if( dst->width() < area.width + area.x ) {
        return;
    }

    if( dst->height() < area.height + area.y ) {
        return;
    }

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        convertToMonochrome_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area,
            channelFactors
        );

        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        convertToMonochrome_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area,
            channelFactors
        );

        rendered = true;
    }

    assert( rendered );
#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !rendered ) {
        qDebug() << "fill(): Failed to apply operation to ImageLayer.";
    }

#endif
}

void convertToMonochrome(
    ImageLayer* /*dst*/,
    ImageLayer* /*src*/,
    Rect32I /*area*/,
    float* /*brightColorFactors*/,
    float* /*darkColorFactors*/
) {
    assert( false );
}

}
}
}
