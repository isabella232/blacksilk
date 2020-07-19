

#include <libgraphics/fx/operations/complex.hpp>
#include <libgraphics/fx/operations/complex/gl.hpp>
#include <libgraphics/fx/operations/complex/cpu.hpp>
#include <libgraphics/fx/operations/complex/gen.hpp>

namespace libgraphics {
namespace fx {
namespace operations {

/// operation: filmgrain
void filmgrain(
    fxapi::ApiBackendDevice* backend,
    ImageLayer*                 dst,
    ImageLayer*                 src,
    Rect32I                     area,
    ImageLayer*                 grainLayer,
    const std::vector<float>&   curveData,
    bool                        isMonoGrain
) {
    assert( dst );
    assert( src );
    assert( !dst->empty() );
    assert( !src->empty() );

    bool rendered( false );

    if( ( backend->backendId() == FXAPI_BACKEND_CPU ) && dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        if( !grainLayer->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
            grainLayer->updateDataForBackend( dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ), FXAPI_BACKEND_CPU );
        }

        filmgrain_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area,
            grainLayer->internalImageForBackend( FXAPI_BACKEND_CPU ),
            curveData,
            isMonoGrain
        );
        rendered = true;
    }

    if( ( backend->backendId() == FXAPI_BACKEND_OPENGL ) && dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        if( !grainLayer->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
            grainLayer->updateDataForBackend( dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ), FXAPI_BACKEND_OPENGL );
        }

        filmgrain_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area,
            grainLayer->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            curveData,
            isMonoGrain
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

}
}
}
