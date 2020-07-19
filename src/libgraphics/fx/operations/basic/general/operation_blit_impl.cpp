
#include <libgraphics/fx/operations/basic/general/cpu.hpp>
#include <libgraphics/fx/operations/basic/general/gl.hpp>

#include <QDebug>

namespace libgraphics {
namespace fx {
namespace operations {

void blit(
    ImageLayer* dst,
    ImageLayer* src,
    libgraphics::Rect32I sourceArea,
    libgraphics::Rect32I destArea
) {
    assert( dst );
    assert( src );
    assert( dst->width() >= destArea.width + destArea.x );
    assert( dst->height() >= destArea.height + destArea.y );
    assert( src->width() >= sourceArea.width + sourceArea.x );
    assert( src->height() >= sourceArea.height + sourceArea.y );
    assert( sourceArea.width == destArea.width );
    assert( sourceArea.height == destArea.height );

    if( !dst || !src ) {
        return;
    }

    if( sourceArea.width != destArea.width ) {
        return;
    }

    if( sourceArea.height != destArea.height ) {
        return;
    }

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        blit_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src->internalImageForBackend( FXAPI_BACKEND_CPU ),
            sourceArea,
            destArea
        );

        rendered = true;
    } else if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        blit_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            sourceArea,
            destArea
        );

        rendered = true;
    }

    assert( rendered );
    ( void )rendered;
}


}
}
}
