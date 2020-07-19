
#include <assert.h>
#include <QDebug>
#include <math.h>

#include <libgraphics/fx/operations/complex.hpp>
#include <libgraphics/fx/operations/complex/cpu.hpp>
#include <libgraphics/fx/operations/helpers/cpu_helpers.hpp>
#include <libgraphics/backend/cpu/cpu_backenddevice.hpp>
#include <libgraphics/backend/cpu/cpu_imageobject.hpp>

namespace libgraphics {
namespace fx {
namespace operations {

struct kernel_filmgrain_pack {
    libgraphics::backend::cpu::ImageObject*     grainLayer;
    std::vector<float>                          curveData;
    bool                                        isMonoGrain;

    kernel_filmgrain_pack() : grainLayer( nullptr ), isMonoGrain( false ) {}
};



template <  class _t_pixel_type >
void cpuFilmgrain(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::backend::cpu::ImageObject*   destination,
    libgraphics::backend::cpu::ImageObject*   source,
    libgraphics::Rect32I area,
    kernel_filmgrain_pack params
) {
    ( void )device;

    void*   destinationBuffer = destination->data();
    void*   sourceBuffer = source->data();
    void*   noiseBuffer  = params.grainLayer->data();

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

    const auto maxValue = std::numeric_limits<_t_pixel_type>::max();

    for( size_t p = 0; ( area.width * area.height ) > p; ++p ) {
        const size_t y = ( p - ( p % ( int )area.width ) ) / area.width;
        const size_t x = ( p - ( y * ( int )area.width ) );

        _t_pixel_type* ptrDstPixel = ( _t_pixel_type* )( ( ( char* )destinationBuffer ) + ( ( ( area.y + y ) * destination->width() ) + x + area.x ) * pixelLength );
        _t_pixel_type* ptrSrcPixel = ( _t_pixel_type* )( ( ( char* )sourceBuffer ) + ( ( ( area.y + y ) * source->width() ) + x + area.x ) * pixelLength );
        _t_pixel_type* ptrNoisePixel = ( _t_pixel_type* )( ( ( char* )noiseBuffer ) + ( ( ( area.y + y ) * source->width() ) + x + area.x ) * pixelLength );

        math::Color3f   realColor   = GetColor3f( maxValue, ptrSrcPixel );
        const size_t    index       = MapFloat( maxValue, realColor.r );
        const float     value       = GetColor3f( maxValue, ptrNoisePixel ).r;
        const float     weight      = params.curveData[index];
        math::Color3f   wh          = math::overlay( realColor, math::Color3f( value ) );

        realColor                   = math::minMaxUniform( ( realColor * ( 1.0f - weight ) ) + ( wh * weight ) );

        SetColor3f( maxValue, realColor, ptrDstPixel );
    }
}

void filmgrain_CPU(
    libgraphics::fxapi::ApiBackendDevice*   device,
    libgraphics::fxapi::ApiImageObject*     dst,
    libgraphics::fxapi::ApiImageObject*     src,
    Rect32I                                 area,
    libgraphics::fxapi::ApiImageObject*     grainLayer,
    const std::vector<float>&               curveData,
    bool                                    isMonoGrain
) {

    assert( device != nullptr );
    assert( dst != nullptr );
    assert( src != nullptr );


    kernel_filmgrain_pack params;
    params.curveData            = curveData;
    params.isMonoGrain          = isMonoGrain;
    params.grainLayer           = ( backend::cpu::ImageObject* )grainLayer;

    switch( dst->format() ) {
        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src,
                area,
                std::bind(
                    &cpuFilmgrain<signed short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    params
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
                    &cpuFilmgrain<unsigned int>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    params
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
                    &cpuFilmgrain<unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    params
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
                    &cpuFilmgrain<unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    params
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
