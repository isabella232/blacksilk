
#include <assert.h>
#include <QDebug>

#include <libgraphics/fx/operations/complex.hpp>
#include <libgraphics/fx/operations/complex/gl.hpp>
#include <libgraphics/fx/operations/complex/cpu.hpp>


namespace libgraphics {
namespace fx {
namespace operations {

void applyVignette(
    fxapi::ApiBackendDevice* backend,
    ImageLayer* dst,
    ImageLayer* src,
    Rect32I area,
    const Point32F& center,
    float radius,
    float strength
) {
    assert( dst != nullptr );
    assert( src != nullptr );

    if( dst == nullptr || src == nullptr ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "applyVignette() failed: Invalid source or destination.";
#endif
        return;
    }

    bool rendered( false );

    if( ( backend->backendId() == FXAPI_BACKEND_CPU ) && dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        applyVignette_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area,
            center,
            radius,
            strength
        );
        rendered = true;
    }

    if( ( backend->backendId() == FXAPI_BACKEND_OPENGL ) && dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        applyVignette_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area,
            center,
            radius,
            strength
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;

}

}
}
}
