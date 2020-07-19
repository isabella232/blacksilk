#include <libgraphics/fx/operations/basic/colors/gl.hpp>
#include <libgraphics/fx/operations/basic/colors/cpu.hpp>
#include <libgraphics/fx/operations/helpers/cpu_helpers.hpp>

#include <QDebug>

namespace libgraphics {
namespace fx {
namespace operations {

struct kernel_to_monochrome_pack {
    float*  channelFactors;

    explicit kernel_to_monochrome_pack( float* _d ) :
        channelFactors( _d ) {}
};

template <  class _t_pixel_type >
void kernel_to_monochrome_generic(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::backend::cpu::ImageObject*   destination,
    libgraphics::backend::cpu::ImageObject*   source,
    libgraphics::Rect32I area,
    kernel_to_monochrome_pack params
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

    for( size_t p = 0; ( area.width * area.height ) > p; ++p ) {
        const size_t y = ( p - ( p % ( int )area.width ) ) / area.width;
        const size_t x = ( p - ( y * ( int )area.width ) );
        _t_pixel_type* dstPixel = ( _t_pixel_type* )( ( ( char* )destinationBuffer ) + ( ( ( area.y + y ) * destination->width() ) + x + area.x ) * pixelLength );
        _t_pixel_type* srcPixel = ( _t_pixel_type* )( ( ( char* )sourceBuffer ) + ( ( ( area.y + y ) * source->width() ) + x + area.x ) * pixelLength );

        double sum( 0 );

        for( size_t n = 0; channelCount > n; ++n ) {
            float adjustment = *( params.channelFactors + n );
            sum += ( ( ( float ) * srcPixel ) / std::numeric_limits<_t_pixel_type>::max() ) * adjustment;

            ++srcPixel;
        }

        for( size_t n = 0; channelCount > n; ++n ) {
            *dstPixel = ( _t_pixel_type )std::max<double>( 0.0, std::min<double>( std::numeric_limits<_t_pixel_type>::max(), ( double )( sum * std::numeric_limits<_t_pixel_type>::max() ) ) );

            ++dstPixel;
        }

    }
}

template <  class _t_pixel_type >
void kernel_to_monochrome(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::backend::cpu::ImageObject*   destination,
    libgraphics::backend::cpu::ImageObject*   source,
    libgraphics::Rect32I area,
    kernel_to_monochrome_pack params
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

    for( size_t p = 0; ( area.width * area.height ) > p; ++p ) {
        const size_t y = ( p - ( p % ( int )area.width ) ) / area.width;
        const size_t x = ( p - ( y * ( int )area.width ) );
        _t_pixel_type* dstPixel = ( _t_pixel_type* )( ( ( char* )destinationBuffer ) + ( ( ( area.y + y ) * destination->width() ) + x + area.x ) * pixelLength );
        _t_pixel_type* srcPixel = ( _t_pixel_type* )( ( ( char* )sourceBuffer ) + ( ( ( area.y + y ) * source->width() ) + x + area.x ) * pixelLength );

        double sum( 0 );

        for( size_t n = 0; channelCount > n; ++n ) {
            float adjustment = *( params.channelFactors + n );
            sum += ( ( ( float ) * srcPixel ) / std::numeric_limits<_t_pixel_type>::max() ) * adjustment;

            ++srcPixel;
        }

        for( size_t n = 0; channelCount > n; ++n ) {
            *dstPixel = ( _t_pixel_type )std::max<double>( 0.0, std::min<double>( std::numeric_limits<_t_pixel_type>::max(), ( double )( sum * std::numeric_limits<_t_pixel_type>::max() ) ) );

            ++dstPixel;
        }

    }
}



template < class _t_pixel_type >
void kernel_convert_to_monochrome_img(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::backend::cpu::ImageObject*   dst,
    libgraphics::backend::cpu::ImageObject*   src0,
    libgraphics::backend::cpu::ImageObject*   src1,
    libgraphics::Rect32I area
) {
    assert( device != nullptr );
    assert( dst != nullptr );
    assert( src0 != nullptr );
    assert( src1 != nullptr );

    void*   destinationBuffer = dst->data();
    void*   sourceBuffer0   = src0->data();
    void*   sourceBuffer1   = src1->data();

    size_t pixelLength = libgraphics::fxapi::EPixelFormat::getPixelSize(
                             dst->format()
                         );
    size_t channelCount = libgraphics::fxapi::EPixelFormat::getChannelCount(
                              dst->format()
                          );

    const auto maxValue = std::numeric_limits<_t_pixel_type>::max();

    for( size_t p = 0; ( area.width * area.height ) > p; ++p ) {
        const size_t y = ( p - ( p % ( int )area.width ) ) / area.width;
        const size_t x = ( p - ( y * ( int )area.width ) );
        _t_pixel_type* dstPixel = ( _t_pixel_type* )( ( char* )destinationBuffer ) + ( ( ( area.y + y ) * dst->width() ) + x + area.x ) * pixelLength;
        _t_pixel_type* srcPixel = ( _t_pixel_type* )( ( char* )sourceBuffer0 ) + ( ( ( area.y + y ) * src0->width() ) + x + area.x ) * pixelLength;
        _t_pixel_type* factorPixel = ( _t_pixel_type* )( ( char* )sourceBuffer1 ) + ( ( ( area.y + y ) * src1->width() ) + x + area.x ) * pixelLength;

        double sum( 0 );

        for( size_t n = 0; channelCount > n; ++n ) {
            const float adjustment = ( ( float ) * factorPixel ) / maxValue;
            sum += ( ( ( float ) * srcPixel ) / maxValue ) * adjustment;

            ++srcPixel;
            ++factorPixel;
        }

        for( size_t n = 0; channelCount > n; ++n ) {
            *dstPixel = ( _t_pixel_type )std::max<double>( 0.0, std::min<double>( maxValue, ( double )( sum * std::numeric_limits<_t_pixel_type>::max() ) ) );

            ++dstPixel;
        }

    }
}

void convertToMonochrome_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    Rect32I area
) {

    assert( device != nullptr );
    assert( dst != nullptr );
    assert( src0 != nullptr );
    assert( src1 != nullptr );

    switch( dst->format() ) {
        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
        case fxapi::EPixelFormat::Mono16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &kernel_convert_to_monochrome_img<short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB32F:
        case fxapi::EPixelFormat::RGBA32F:
        case fxapi::EPixelFormat::Mono32F:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &kernel_convert_to_monochrome_img<float>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;


        case fxapi::EPixelFormat::RGB8:
        case fxapi::EPixelFormat::RGBA8:
        case fxapi::EPixelFormat::Mono8:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &kernel_convert_to_monochrome_img<unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB16:
        case fxapi::EPixelFormat::RGBA16:
        case fxapi::EPixelFormat::Mono16:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &kernel_convert_to_monochrome_img<unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }

}


void convertToMonochrome_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* destination,
    libgraphics::fxapi::ApiImageObject* source,
    libgraphics::Rect32I   area,
    float* channelFactors
) {
    assert( destination->format() == source->format() );

    switch( destination->format() ) {
        case fxapi::EPixelFormat::Mono8:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )destination,
                ( backend::cpu::ImageObject* )source,
                area,
                std::bind(
                    &kernel_to_monochrome<unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_to_monochrome_pack( channelFactors )
                )
            );
            break;

        case fxapi::EPixelFormat::Mono16:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )destination,
                ( backend::cpu::ImageObject* )source,
                area,
                std::bind(
                    &kernel_to_monochrome<unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_to_monochrome_pack( channelFactors )
                )
            );

        case fxapi::EPixelFormat::RGB8:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )destination,
                ( backend::cpu::ImageObject* )source,
                area,
                std::bind(
                    &kernel_to_monochrome<unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_to_monochrome_pack( channelFactors )
                )
            );

            break;

        case fxapi::EPixelFormat::RGB16:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )destination,
                ( backend::cpu::ImageObject* )source,
                area,
                std::bind(
                    &kernel_to_monochrome<unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_to_monochrome_pack( channelFactors )
                )
            );
            break;

        case fxapi::EPixelFormat::RGBA8:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )destination,
                ( backend::cpu::ImageObject* )source,
                area,
                std::bind(
                    &kernel_to_monochrome<unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_to_monochrome_pack( channelFactors )
                )
            );
            break;

        case fxapi::EPixelFormat::RGBA16:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )destination,
                ( backend::cpu::ImageObject* )source,
                area,
                std::bind(
                    &kernel_to_monochrome<unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_to_monochrome_pack( channelFactors )
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
