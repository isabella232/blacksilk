
#include <assert.h>
#include <QDebug>
#include <math.h>

#include <libgraphics/fx/operations/complex.hpp>
#include <libgraphics/fx/operations/complex/cpu.hpp>
#include <libgraphics/fx/operations/helpers/cpu_helpers.hpp>

namespace libgraphics {
namespace fx {
namespace operations {

struct kernel_apply_vignette_pack {
    kernel_apply_vignette_pack(
        const libgraphics::Point32F& _center,
        const float& _radius,
        const float& _strength,
        const int& _totalWidth,
        const int& _totalHeight
    ) : center( _center ), radius( _radius ), strength( _strength ),
        totalWidth( _totalWidth ), totalHeight( _totalHeight ) {}

    const libgraphics::Point32F&        center;
    const float&                        radius;
    const float&                        strength;
    const int&                          totalWidth;
    const int&                          totalHeight;
};

template <  class _t_pixel_type >
void kernel_apply_vignette(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::backend::cpu::ImageObject*   destination,
    libgraphics::backend::cpu::ImageObject*   source,
    libgraphics::Rect32I area,
    kernel_apply_vignette_pack params
) {
    ( void )device;

    void*   destinationBuffer = destination->data();
    void*   sourceBuffer = source->data();

    size_t pixelLength = libgraphics::fxapi::EPixelFormat::getPixelSize(
                             destination->format()
                         );
    size_t channelCount = libgraphics::fxapi::EPixelFormat::getChannelCount(
                              destination->format()
                          );

    const auto maxValue = std::numeric_limits<_t_pixel_type>::max();
    const Point32F c(
        ( float )params.totalWidth * 0.01f * ( float )params.center.x,
        ( float )params.totalHeight * 0.01f * ( float )params.center.y
    );

    for( size_t p = 0; ( area.width * area.height ) > p; ++p ) {
        const size_t y = ( p - ( p % ( int )area.width ) ) / area.width;
        const size_t x = ( p - ( y * ( int )area.width ) );
        _t_pixel_type* dstPixel = ( _t_pixel_type* )( ( ( char* )destinationBuffer ) + ( ( ( area.y + y ) * destination->width() ) + x + area.x ) * pixelLength );
        _t_pixel_type* srcPixel = ( _t_pixel_type* )( ( ( char* )sourceBuffer ) + ( ( ( area.y + y ) * source->width() ) + x + area.x ) * pixelLength );

        const float distance = fabs( c.distanceTo(
                                         libgraphics::Point32F(
                                             ( float )( area.x + x ),
                                             ( float )( area.y + y )
                                         )
                                     ) );
        const float maxDistance     = params.radius * 0.01f * ( ( float )( params.totalHeight + params.totalHeight ) * 0.5f );
        const float vignetteValue   = params.strength * 0.01f * distance / maxDistance;

        for( size_t i = 0; channelCount > i; ++i ) {
            const float inputValue = ( *( srcPixel + i ) / ( float )maxValue );
            float currentValue =
                ( 1.0f - vignetteValue ) * inputValue + ( vignetteValue * ( inputValue * inputValue ) );
            currentValue = std::max<float>( 0.0, std::min<float>( 1.0, currentValue ) );

            const _t_pixel_type conv = std::ceil( currentValue * maxValue );

            *( dstPixel + i ) = std::max<int>( 0, std::min<int>( maxValue, conv ) );
        }

    }
}

/// operation: applyVignette
void applyVignette_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src,
    Rect32I area,
    const Point32F& center,
    float radius,
    float strength
) {

    assert( device != nullptr );
    assert( dst != nullptr );
    assert( src != nullptr );

    switch( dst->format() ) {
        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
        case fxapi::EPixelFormat::Mono16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src,
                area,
                std::bind(
                    &kernel_apply_vignette<short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_apply_vignette_pack( center, radius, strength, dst->width(), dst->height() )
                )
            );
            break;

        case fxapi::EPixelFormat::RGB32F:
        case fxapi::EPixelFormat::RGBA32F:
        case fxapi::EPixelFormat::Mono32F:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src,
                area,
                std::bind(
                    &kernel_apply_vignette<float>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_apply_vignette_pack( center, radius, strength, dst->width(), dst->height() )
                )
            );
            break;


        case fxapi::EPixelFormat::RGB8:
        case fxapi::EPixelFormat::RGBA8:
        case fxapi::EPixelFormat::Mono8:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src,
                area,
                std::bind(
                    &kernel_apply_vignette<unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_apply_vignette_pack( center, radius, strength, dst->width(), dst->height() )
                )
            );
            break;

        case fxapi::EPixelFormat::RGB16:
        case fxapi::EPixelFormat::RGBA16:
        case fxapi::EPixelFormat::Mono16:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src,
                area,
                std::bind(
                    &kernel_apply_vignette<unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_apply_vignette_pack( center, radius, strength, dst->width(), dst->height() )
                )
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }

}

}
}
}
