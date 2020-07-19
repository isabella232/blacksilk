
#include <libgraphics/fx/operations/complex.hpp>
#include <libgraphics/fx/operations/complex/gl.hpp>
#include <libgraphics/fx/operations/complex/cpu.hpp>
#include <libgraphics/fx/operations/complex/gen.hpp>

namespace libgraphics {
namespace fx {
namespace operations {

void splittone(
    fxapi::ApiBackendDevice* backend,
    ImageLayer* dst,
    ImageLayer* src,
    Rect32I area,
    float brightR,
    float brightG,
    float brightB,
    float darkR,
    float darkG,
    float darkB,
    float weight
) {
    assert( dst );
    assert( src );
    assert( !dst->empty() );
    assert( !src->empty() );

    bool rendered( false );

    if( ( backend->backendId() == FXAPI_BACKEND_CPU ) && dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        splittone_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area,
            brightR,
            brightG,
            brightB,
            darkR,
            darkG,
            darkB,
            weight
        );
        rendered = true;
    }

    if( ( backend->backendId() == FXAPI_BACKEND_OPENGL ) && dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        splittone_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area,
            brightR,
            brightG,
            brightB,
            darkR,
            darkG,
            darkB,
            weight
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

}
}
}
