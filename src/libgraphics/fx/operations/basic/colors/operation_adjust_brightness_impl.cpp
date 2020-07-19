
#include <libgraphics/fx/operations/basic/colors/gl.hpp>
#include <libgraphics/fx/operations/basic/colors/cpu.hpp>

namespace libgraphics {
namespace fx {
namespace operations {

/// helpers. temporary cpp-file.
size_t getTileSize() {
    return 1024;
}

size_t getTileCountForRectangle(
    libgraphics::Rect32I   area,
    size_t _tileSize = 1024
) {
    const size_t tileSize = _tileSize;

    return ( ( area.width - ( area.width % tileSize ) ) / tileSize ) + 1
           +
           ( ( area.height - ( area.height % tileSize ) ) / tileSize ) + 1;
}

size_t getVerticalTileCountForRectangle(
    libgraphics::Rect32I   area,
    size_t _tileSize = 1024
) {
    const size_t tileSize = _tileSize;

    return ( ( area.height - ( area.height % tileSize ) ) / tileSize ) + 1;
}

size_t getHorizontalTileCountForRectangle(
    libgraphics::Rect32I   area,
    size_t _tileSize = 1024
) {
    const size_t tileSize = _tileSize;

    return ( ( area.width - ( area.width % tileSize ) ) / tileSize ) + 1;
}

/// operation: adjust-brightness
/// desc: Adjusts the brightness of an image object based on
/// a curve.
void adjustBrightness(
    ImageLayer* dst,
    ImageLayer* src,
    Rect32I area,
    float* values,
    size_t length
) {
    assert( dst );
    assert( src );
    assert( !dst->empty() );
    assert( !src->empty() );

    assert( values );
    assert( length > 0 );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        adjustBrightness_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area,
            values,
            length
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        adjustBrightness_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area,
            values,
            length
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void adjustBrightness(
    ImageLayer* dst,
    ImageLayer* src,
    libgraphics::Rect32I area,
    float value
) {
    assert( dst );
    assert( src );
    assert( dst->empty() );
    assert( src->empty() );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        adjustBrightness_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area,
            value
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        adjustBrightness_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area,
            value
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

}
}
}
