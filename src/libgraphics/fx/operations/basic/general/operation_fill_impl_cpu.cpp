
#include <libgraphics/fx/operations/basic/general/cpu.hpp>
#include <libgraphics/fx/operations/helpers/cpu_helpers.hpp>

#include <QDebug>

namespace libgraphics {
namespace fx {
namespace operations {

void kernel_fill_value(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::backend::cpu::ImageObject*   destination,
    libgraphics::backend::cpu::ImageObject*   source,
    libgraphics::Rect32I area,
    int value
) {
    assert( device );
    assert( destination );
    ( void )source;

    void*   destinationBuffer   = destination->data();
    const auto pixelLength      = libgraphics::fxapi::EPixelFormat::getPixelSize(
                                      destination->format()
                                  );

    for( size_t y = 0; area.height > y; ++y ) {
        void* dstPixel = ( void* )( ( ( char* )destinationBuffer ) + ( ( ( area.y + y ) * destination->width() ) + area.x ) * pixelLength );

        ( void ) ::memset(
            dstPixel,
            value,
            pixelLength * area.width
        );
    }
}

struct kernel_fill_params {
    const libgraphics::Format& format;
    void* color;

    kernel_fill_params( const libgraphics::Format& _format, void* _color ) :
        format( _format ), color( _color ) {}
};
void kernel_fill(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::backend::cpu::ImageObject*   destination,
    libgraphics::backend::cpu::ImageObject*   source,
    libgraphics::Rect32I area,
    kernel_fill_params params
) {
    assert( device );
    assert( destination );
    ( void )source;

    void*   destinationBuffer = destination->data();

    const auto pixelLength = libgraphics::fxapi::EPixelFormat::getPixelSize(
                                 destination->format()
                             );
    assert( params.format.byteSize == pixelLength );

    if( params.format.byteSize != pixelLength ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "kernel_fill(): Incompatible formats.";
#endif
        return;
    }

    for( size_t x = 0; area.width > x; ++x ) {
        for( size_t y = 0; area.height > y; ++y ) {
            void* dstPixel = ( void* )( ( ( char* )destinationBuffer ) + ( ( ( area.y + y ) * destination->width() ) + x + area.x ) * pixelLength );

            ( void ) memcpy(
                dstPixel,
                params.color,
                pixelLength
            );
        }
    }
}

template < class _t_value >
struct kernel_fill_channel_params {
    size_t channelIndex;
    _t_value value;

    kernel_fill_channel_params( size_t _index, _t_value _val ) :
        channelIndex( _index ), value( _val ) {}
};
template < class _t_value >
void kernel_fill_channel(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::backend::cpu::ImageObject*   destination,
    libgraphics::backend::cpu::ImageObject*   source,
    libgraphics::Rect32I area,
    kernel_fill_channel_params<_t_value> params
) {
    assert( device );
    assert( destination );
    ( void )source;

    void*   destinationBuffer = destination->data();

    const auto pixelLength = libgraphics::fxapi::EPixelFormat::getPixelSize(
                                 destination->format()
                             );
    const auto channelCount = libgraphics::fxapi::EPixelFormat::getChannelCount(
                                  destination->format()
                              );
    const auto channelSize = pixelLength / channelCount;

    for( size_t p = 0; ( area.width * area.height ) > p; ++p ) {
        const size_t y = ( p - ( p % ( int )area.width ) ) / area.width;
        const size_t x = ( p - ( y * ( int )area.width ) );
        void* dstPixel = ( void* )( ( ( char* )destinationBuffer ) + ( ( ( area.y + y ) * destination->width() ) + x + area.x ) * pixelLength );

        ( void ) memcpy(
            ( void* )( ( char* )dstPixel + ( params.channelIndex * channelSize ) ),
            ( void* )&params.value,
            sizeof( _t_value )
        );
    }
}

/// operation: fill_CPU
void fill_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::Rect32I area,
    int value
) {
    assert( device );
    assert( dst );
    assert( dst->width() >= area.width + area.x );
    assert( dst->height() >= area.height + area.y );

    if( dst->width() < area.width + area.x ) {
        return;
    }

    if( dst->height() < area.height + area.y ) {
        return;
    }

    fx::operations::cpuExecuteTileBased(
        device,
        ( backend::cpu::ImageObject* )dst,
        ( backend::cpu::ImageObject* )nullptr,
        area,
        std::bind(
            &kernel_fill_value,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3,
            std::placeholders::_4,
            value
        )
    );

    return;
}

void fill_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::Rect32I area,
    const libgraphics::Format& format,
    void* color
) {
    assert( device );
    assert( dst );
    assert( dst->width() >= area.width + area.x );
    assert( dst->height() >= area.height + area.y );
    assert( color );

    if( dst->width() < area.width + area.x ) {
        return;
    }

    if( dst->height() < area.height + area.y ) {
        return;
    }

    fx::operations::cpuExecuteTileBased(
        device,
        ( backend::cpu::ImageObject* )dst,
        ( backend::cpu::ImageObject* )nullptr,
        area,
        std::bind(
            &kernel_fill,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3,
            std::placeholders::_4,
            kernel_fill_params( format, color )
        )
    );

    return;
}

void fillChannel_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::Rect32I area,
    size_t channelIndex,
    unsigned char value
) {
    assert( device );
    assert( dst );
    assert( dst->width() >= area.width + area.x );
    assert( dst->height() >= area.height + area.y );

    if( dst->width() < area.width + area.x ) {
        return;
    }

    if( dst->height() < area.height + area.y ) {
        return;
    }

    fx::operations::cpuExecuteTileBased(
        device,
        ( backend::cpu::ImageObject* )dst,
        ( backend::cpu::ImageObject* )nullptr,
        area,
        std::bind(
            &kernel_fill_channel<unsigned char>,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3,
            std::placeholders::_4,
            kernel_fill_channel_params<unsigned char>( channelIndex, value )
        )
    );

    return;
}

void fillChannel_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::Rect32I area,
    size_t channelIndex,
    unsigned short value
) {
    assert( device );
    assert( dst );
    assert( dst->width() >= area.width + area.x );
    assert( dst->height() >= area.height + area.y );

    if( dst->width() < area.width + area.x ) {
        return;
    }

    if( dst->height() < area.height + area.y ) {
        return;
    }

    fx::operations::cpuExecuteTileBased(
        device,
        ( backend::cpu::ImageObject* )dst,
        ( backend::cpu::ImageObject* )nullptr,
        area,
        std::bind(
            &kernel_fill_channel<unsigned short>,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3,
            std::placeholders::_4,
            kernel_fill_channel_params<unsigned short>( channelIndex, value )
        )
    );

    return;
}

void fillChannel_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::Rect32I area,
    size_t channelIndex,
    unsigned int value
) {
    assert( device );
    assert( dst );
    assert( dst->width() >= area.width + area.x );
    assert( dst->height() >= area.height + area.y );

    if( dst->width() < area.width + area.x ) {
        return;
    }

    if( dst->height() < area.height + area.y ) {
        return;
    }

    fx::operations::cpuExecuteTileBased(
        device,
        ( backend::cpu::ImageObject* )dst,
        ( backend::cpu::ImageObject* )nullptr,
        area,
        std::bind(
            &kernel_fill_channel<unsigned int>,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3,
            std::placeholders::_4,
            kernel_fill_channel_params<unsigned int>( channelIndex, value )
        )
    );

    return;
}

void fillChannel_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::Rect32I area,
    size_t channelIndex,
    float value
) {
    assert( device );
    assert( dst );
    assert( dst->width() >= area.width + area.x );
    assert( dst->height() >= area.height + area.y );

    if( dst->width() < area.width + area.x ) {
        return;
    }

    if( dst->height() < area.height + area.y ) {
        return;
    }

    fx::operations::cpuExecuteTileBased(
        device,
        ( backend::cpu::ImageObject* )dst,
        ( backend::cpu::ImageObject* )nullptr,
        area,
        std::bind(
            &kernel_fill_channel<float>,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3,
            std::placeholders::_4,
            kernel_fill_channel_params<float>( channelIndex, value )
        )
    );

    return;
}

}
}
}
