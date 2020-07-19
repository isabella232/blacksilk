
#include <libgraphics/fx/operations/complex.hpp>
#include <libgraphics/fx/operations/complex/gl.hpp>
#include <libgraphics/fx/operations/complex/cpu.hpp>

#include <QDebug>

namespace libgraphics {
namespace fx {
namespace operations {

void gaussianBlur(
    fxapi::ApiBackendDevice* backend,
    ImageLayer* dst,
    ImageLayer* src,
    Rect32I area,
    float radius
) {
    assert( dst );
    assert( src );
    assert( !dst->empty() );
    assert( !src->empty() );

    bool rendered( false );

    if( ( backend->backendId() == FXAPI_BACKEND_CPU ) && dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        gaussianBlur_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area,
            radius
        );
        rendered = true;
    }

    if( ( backend->backendId() == FXAPI_BACKEND_OPENGL ) && dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        gaussianBlur_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area,
            radius
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void verticalGaussianBlur(
    fxapi::ApiBackendDevice* backend,
    ImageLayer* dst,
    ImageLayer* src,
    Rect32I area,
    float radius
) {
    assert( dst );
    assert( src );
    assert( !dst->empty() );
    assert( !src->empty() );

    bool rendered( false );

    if( ( backend->backendId() == FXAPI_BACKEND_CPU ) && dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        verticalGaussianBlur_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area,
            radius
        );
        rendered = true;
    }

    if( ( backend->backendId() == FXAPI_BACKEND_OPENGL ) && dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        verticalGaussianBlur_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area,
            radius
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void horizontalGaussianBlur(
    fxapi::ApiBackendDevice* backend,
    ImageLayer* dst,
    ImageLayer* src,
    Rect32I area,
    float radius
) {
    assert( dst );
    assert( src );
    assert( !dst->empty() );
    assert( !src->empty() );

    bool rendered( false );

    if( ( backend->backendId() == FXAPI_BACKEND_CPU ) && dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        horizontalGaussianBlur_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area,
            radius
        );
        rendered = true;
    }

    if( ( backend->backendId() == FXAPI_BACKEND_OPENGL ) && dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        horizontalGaussianBlur_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area,
            radius
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

}
}
}
