
#include <libgraphics/fx/operations/basic/general/cpu.hpp>
#include <libgraphics/fx/operations/basic/general/gl.hpp>

#include <QDebug>

namespace libgraphics {
namespace fx {
namespace operations {

void fill(
    ImageLayer* dst,
    libgraphics::Rect32I area,
    int value
) {
    assert( dst );
    assert( dst->width() >= area.width + area.x );
    assert( dst->height() >= area.height + area.y );

    if( !dst ) {
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
        fill_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area,
            value
        );

        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        fill_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area,
            value
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
void fill(
    ImageLayer* dst,
    libgraphics::Rect32I area,
    const libgraphics::Format& format,
    void* color
) {
    assert( dst );
    assert( dst->width() >= area.width + area.x );
    assert( dst->height() >= area.height + area.y );
    assert( color );

    if( !dst || !color ) {
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
        fill_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area,
            format,
            color
        );

        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        fill_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area,
            format,
            color
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

void fillChannel(
    ImageLayer* dst,
    libgraphics::Rect32I area,
    size_t channelIndex,
    unsigned char value
) {
    assert( dst );
    assert( dst->width() >= area.width + area.x );
    assert( dst->height() >= area.height + area.y );

    if( !dst ) {
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
        fillChannel_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area,
            channelIndex,
            value
        );

        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        fillChannel_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area,
            channelIndex,
            value
        );

        rendered = true;
    }

    assert( rendered );
#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !rendered ) {
        qDebug() << "fillChannel(): Failed to apply operation to ImageLayer.";
    }

#endif
}

void fillChannel(
    ImageLayer* dst,
    libgraphics::Rect32I area,
    size_t channelIndex,
    unsigned short value
) {
    assert( dst );
    assert( dst->width() >= area.width + area.x );
    assert( dst->height() >= area.height + area.y );

    if( !dst ) {
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
        fillChannel_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area,
            channelIndex,
            value
        );

        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        fillChannel_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area,
            channelIndex,
            value
        );

        rendered = true;
    }

    assert( rendered );
#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !rendered ) {
        qDebug() << "fillChannel(): Failed to apply operation to ImageLayer.";
    }

#endif
}

void fillChannel(
    ImageLayer* dst,
    libgraphics::Rect32I area,
    size_t channelIndex,
    unsigned int value
) {
    assert( dst );
    assert( dst->width() >= area.width + area.x );
    assert( dst->height() >= area.height + area.y );

    if( !dst ) {
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
        fillChannel_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area,
            channelIndex,
            value
        );

        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        fillChannel_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area,
            channelIndex,
            value
        );

        rendered = true;
    }

    assert( rendered );
#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !rendered ) {
        qDebug() << "fillChannel(): Failed to apply operation to ImageLayer.";
    }

#endif
}

void fillChannel(
    ImageLayer* dst,
    libgraphics::Rect32I area,
    size_t channelIndex,
    float value
) {
    assert( dst );
    assert( dst->width() >= area.width + area.x );
    assert( dst->height() >= area.height + area.y );

    if( !dst ) {
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
        fillChannel_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area,
            channelIndex,
            value
        );

        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        fillChannel_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area,
            channelIndex,
            value
        );

        rendered = true;
    }

    assert( rendered );
#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !rendered ) {
        qDebug() << "fillChannel(): Failed to apply operation to ImageLayer.";
    }

#endif
}

}
}
}
