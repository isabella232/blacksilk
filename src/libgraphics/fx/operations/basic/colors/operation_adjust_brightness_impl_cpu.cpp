#include <libgraphics/fx/operations/basic/colors/gl.hpp>
#include <libgraphics/fx/operations/basic/colors/cpu.hpp>
#include <libgraphics/fx/operations/helpers/cpu_helpers.hpp>

#include <libgraphics/backend/cpu/cpu_backenddevice.hpp>
#include <libgraphics/backend/cpu/cpu_imageobject.hpp>
#include <libgraphics/backend/cpu/cpu_imageoperation.hpp>

#include <QThreadPool>
#include <QRunnable>
#include <QDebug>

#include <limits>

namespace libgraphics {
namespace fx {
namespace operations {

/** impl: adjustment **/
struct kernel_linear_adjustment_pack {
    float   adjustment;

    kernel_linear_adjustment_pack( float _adjustment ) :
        adjustment( _adjustment ) {}
};

template < class _t_pixel_type >
void kernel_linear_adjustment(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::backend::cpu::ImageObject*   destination,
    libgraphics::backend::cpu::ImageObject*   source,
    libgraphics::Rect32I area,
    kernel_linear_adjustment_pack parameters
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

        for( size_t n = 0; channelCount > n; ++n ) {
            *dstPixel = std::min( ( _t_pixel_type )( *srcPixel * parameters.adjustment ), std::numeric_limits<_t_pixel_type>::max() );

            ++srcPixel;
            ++dstPixel;
        }

    }

}

struct kernel_curve_adjustment_pack {
    float*  curveData;
    size_t  curveLength;

    kernel_curve_adjustment_pack( float* _d, size_t _l ) :
        curveData( _d ), curveLength( _l ) {}
};

template < class _t_pixel_type >
void kernel_curve_adjustment(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::backend::cpu::ImageObject*   destination,
    libgraphics::backend::cpu::ImageObject*   source,
    libgraphics::Rect32I area,
    kernel_curve_adjustment_pack params
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

        for( size_t n = 0; channelCount > n; ++n ) {
            float adjustment = *( params.curveData + *srcPixel );

            *dstPixel = ( _t_pixel_type )std::max<double>( 0, std::min<double>( ( double )std::numeric_limits<_t_pixel_type>::max() * adjustment, ( double )std::numeric_limits<_t_pixel_type>::max() ) );

            ++srcPixel;
            ++dstPixel;
        }
    }

}

/** impl: curve based **/
void adjustBrightness_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject*     destination,
    libgraphics::fxapi::ApiImageObject*     source,
    libgraphics::Rect32I   area,
    float* curveData,
    size_t curveLength
) {
    assert( destination->format() == source->format() );

    switch( destination->format() ) {
        case fxapi::EPixelFormat::Mono8:
            assert( std::numeric_limits<unsigned char>::max() <= curveLength );

            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )destination,
                ( backend::cpu::ImageObject* )source,
                area,
                std::bind(
                    &kernel_curve_adjustment<unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_curve_adjustment_pack( curveData, curveLength )
                )
            );
            break;

        case fxapi::EPixelFormat::Mono16:
            assert( std::numeric_limits<unsigned short>::max() <= curveLength );

            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )destination,
                ( backend::cpu::ImageObject* )source,
                area,
                std::bind(
                    &kernel_curve_adjustment<unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_curve_adjustment_pack( curveData, curveLength )
                )
            );

        case fxapi::EPixelFormat::RGB8:
            assert( std::numeric_limits<unsigned char>::max() <= curveLength );

            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )destination,
                ( backend::cpu::ImageObject* )source,
                area,
                std::bind(
                    &kernel_curve_adjustment<unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_curve_adjustment_pack( curveData, curveLength )
                )
            );

            break;

        case fxapi::EPixelFormat::RGB16:
            assert( std::numeric_limits<unsigned short>::max() <= curveLength );

            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )destination,
                ( backend::cpu::ImageObject* )source,
                area,
                std::bind(
                    &kernel_curve_adjustment<unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_curve_adjustment_pack( curveData, curveLength )
                )
            );
            break;

        case fxapi::EPixelFormat::RGBA8:
            assert( std::numeric_limits<unsigned char>::max() <= curveLength );

            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )destination,
                ( backend::cpu::ImageObject* )source,
                area,
                std::bind(
                    &kernel_curve_adjustment<unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_curve_adjustment_pack( curveData, curveLength )
                )
            );
            break;

        case fxapi::EPixelFormat::RGBA16:
            assert( std::numeric_limits<unsigned short>::max() <= curveLength );

            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )destination,
                ( backend::cpu::ImageObject* )source,
                area,
                std::bind(
                    &kernel_curve_adjustment<unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_curve_adjustment_pack( curveData, curveLength )
                )
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }
}

void adjustBrightness_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject*     destination,
    libgraphics::fxapi::ApiImageObject*     source,
    libgraphics::Rect32I   area,
    float adjustment
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
                    &kernel_linear_adjustment<unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_linear_adjustment_pack( adjustment )
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
                    &kernel_linear_adjustment<unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_linear_adjustment_pack( adjustment )
                )
            );

        case fxapi::EPixelFormat::RGB8:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )destination,
                ( backend::cpu::ImageObject* )source,
                area,
                std::bind(
                    &kernel_linear_adjustment<unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_linear_adjustment_pack( adjustment )
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
                    &kernel_linear_adjustment<unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_linear_adjustment_pack( adjustment )
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
                    &kernel_linear_adjustment<unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_linear_adjustment_pack( adjustment )
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
                    &kernel_linear_adjustment<unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_linear_adjustment_pack( adjustment )
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
