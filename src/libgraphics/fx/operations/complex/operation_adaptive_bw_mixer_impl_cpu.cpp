
#include <assert.h>
#include <QDebug>
#include <math.h>

#include <libgraphics/fx/operations/complex.hpp>
#include <libgraphics/fx/operations/complex/cpu.hpp>
#include <libgraphics/fx/operations/helpers/cpu_helpers.hpp>

namespace libgraphics {
namespace fx {
namespace operations {

struct kernel_adaptive_bwmixer {
    kernel_adaptive_bwmixer(
        const float& _brightR,
        const float& _brightG,
        const float& _brightB,
        const float& _darkR,
        const float& _darkG,
        const float& _darkB,
        const float& _weight
    ) : brightR( _brightR ), brightG( _brightG ), brightB( _brightB ),
        darkR( _darkR ), darkG( _darkG ), darkB( _darkB ), weight( _weight ) {}

    float       brightR;
    float       brightG;
    float       brightB;
    float       darkR;
    float       darkG;
    float       darkB;
    float       weight;
};


template <  class _t_pixel_type >
void cpuAdaptiveBWMixer(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::backend::cpu::ImageObject*   destination,
    libgraphics::backend::cpu::ImageObject*   source,
    libgraphics::Rect32I area,
    kernel_adaptive_bwmixer params
) {
    ( void )device;

    void*   destinationBuffer = destination->data();
    void*   sourceBuffer = source->data();

    const size_t pixelLength = libgraphics::fxapi::EPixelFormat::getPixelSize(
                                   destination->format()
                               );
    const size_t channelCount = libgraphics::fxapi::EPixelFormat::getChannelCount(
                                    destination->format()
                                );

    assert( channelCount >= 3 );

    if( channelCount < 3 ) {
        return;
    }

    const float highlights[4] = {
        params.brightR,
        params.brightG,
        params.brightB,
        1.0f
    };
    const float shadows[4] = {
        params.darkR,
        params.darkG,
        params.darkB,
        1.0f
    };

    const auto maxValue = std::numeric_limits<_t_pixel_type>::max();

    for( size_t p = 0; ( area.width * area.height ) > p; ++p ) {
        const size_t y = ( p - ( p % ( int )area.width ) ) / area.width;
        const size_t x = ( p - ( y * ( int )area.width ) );

        _t_pixel_type* dstPixel = ( _t_pixel_type* )( ( ( char* )destinationBuffer ) + ( ( ( area.y + y ) * destination->width() ) + x + area.x ) * pixelLength );
        _t_pixel_type* srcPixel = ( _t_pixel_type* )( ( ( char* )sourceBuffer ) + ( ( ( area.y + y ) * source->width() ) + x + area.x ) * pixelLength );

        const float luma    = params.weight + ( MapToFloat( maxValue, GetR( srcPixel ) ) * 0.299f )  + ( MapToFloat( maxValue, GetG( srcPixel ) ) * 0.587f )  +
                              ( MapToFloat( maxValue, GetB( srcPixel ) ) * 0.114f );
        const float bright  = ( MapToFloat( maxValue, GetR( srcPixel ) ) * highlights[0] ) + ( MapToFloat( maxValue, GetG( srcPixel ) ) * highlights[1] ) +
                              ( MapToFloat( maxValue, GetB( srcPixel ) ) * highlights[2] );
        const float dark  = ( MapToFloat( maxValue, GetR( srcPixel ) ) * shadows[0] ) + ( MapToFloat( maxValue, GetG( srcPixel ) ) * shadows[1] ) +
                            ( MapToFloat( maxValue, GetB( srcPixel ) ) * shadows[2] );
        const float combined = ( dark * ( 1.0f - luma ) ) + ( bright * luma );

        GetR( dstPixel ) = MinMax( maxValue, MapFloat( maxValue, combined ) );
        GetG( dstPixel ) = MinMax( maxValue, MapFloat( maxValue, combined ) );
        GetB( dstPixel ) = MinMax( maxValue, MapFloat( maxValue, combined ) );
    }
}

void adaptiveBWMixer_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src,
    libgraphics::Rect32I   area,
    float brightR,
    float brightG,
    float brightB,
    float darkR,
    float darkG,
    float darkB,
    float weight
) {

    assert( device != nullptr );
    assert( dst != nullptr );
    assert( src != nullptr );

    switch( dst->format() ) {
        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src,
                area,
                std::bind(
                    &cpuAdaptiveBWMixer<short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_adaptive_bwmixer( brightR, brightG, brightB, darkR, darkG, darkB, weight )
                )
            );
            break;

        case fxapi::EPixelFormat::RGB32F:
        case fxapi::EPixelFormat::RGBA32F:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src,
                area,
                std::bind(
                    &cpuAdaptiveBWMixer<float>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_adaptive_bwmixer( brightR, brightG, brightB, darkR, darkG, darkB, weight )
                )
            );
            break;


        case fxapi::EPixelFormat::RGB8:
        case fxapi::EPixelFormat::RGBA8:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src,
                area,
                std::bind(
                    &cpuAdaptiveBWMixer<unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_adaptive_bwmixer( brightR, brightG, brightB, darkR, darkG, darkB, weight )
                )
            );
            break;

        case fxapi::EPixelFormat::RGB16:
        case fxapi::EPixelFormat::RGBA16:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src,
                area,
                std::bind(
                    &cpuAdaptiveBWMixer<unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_adaptive_bwmixer( brightR, brightG, brightB, darkR, darkG, darkB, weight )
                )
            );
            break;

        default:
            assert( false );
            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }
}

}
}
}
