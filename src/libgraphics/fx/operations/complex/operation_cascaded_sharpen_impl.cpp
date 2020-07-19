
#include <libgraphics/fx/operations/complex.hpp>
#include <libgraphics/fx/operations/complex/gl.hpp>
#include <libgraphics/fx/operations/complex/cpu.hpp>
#include <libgraphics/fx/operations/complex/gen.hpp>

namespace libgraphics {
namespace fx {
namespace operations {

void cascadedSharpen(
    fxapi::ApiBackendDevice* backend,
    ImageLayer*                 dst,
    ImageLayer*                 src,
    Rect32I                     area,
    const std::vector< std::tuple< libgraphics::ImageLayer*, float, float > >& cascades,
    float threshold
) {
    assert( dst );
    assert( src );
    assert( !dst->empty() );
    assert( !src->empty() );

    bool rendered( false );

    if( ( backend->backendId() == FXAPI_BACKEND_CPU ) && dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        if( cascades.size() == 4 ) {
            cascadedSharpenWith4_CPU(
                dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
                dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
                src->internalImageForBackend( FXAPI_BACKEND_CPU ),
                area,
                cascades[0],
                cascades[1],
                cascades[2],
                cascades[3],
                threshold
            );

        } else {
            cascadedSharpen_GEN(
                dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
                dst,
                src,
                area,
                cascades,
                threshold
            );
        }

        rendered = true;
    }

    if( ( backend->backendId() == FXAPI_BACKEND_OPENGL ) && dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        if( cascades.size() == 4 ) {
            cascadedSharpenWith4_GL(
                dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
                dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
                src->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
                area,
                cascades[0],
                cascades[1],
                cascades[2],
                cascades[3],
                threshold
            );

        } else {
            cascadedSharpen_GEN(
                dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
                dst,
                src,
                area,
                cascades,
                threshold
            );
        }

        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;

}

}
}
}
