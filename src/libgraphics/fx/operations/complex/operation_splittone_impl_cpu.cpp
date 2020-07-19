
#include <assert.h>
#include <QDebug>
#include <math.h>

#include <libgraphics/fx/operations/complex.hpp>
#include <libgraphics/fx/operations/complex/cpu.hpp>
#include <libgraphics/fx/operations/helpers/cpu_helpers.hpp>

namespace libgraphics {
namespace fx {
namespace operations {

struct kernel_splittone {
    kernel_splittone(
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
void cpuSplittone(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::backend::cpu::ImageObject*   destination,
    libgraphics::backend::cpu::ImageObject*   source,
    libgraphics::Rect32I area,
    kernel_splittone params
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

        _t_pixel_type* ptrDstPixel = ( _t_pixel_type* )( ( ( char* )destinationBuffer ) + ( ( ( area.y + y ) * destination->width() ) + x + area.x ) * pixelLength );
        _t_pixel_type* ptrSrcPixel = ( _t_pixel_type* )( ( ( char* )sourceBuffer ) + ( ( ( area.y + y ) * source->width() ) + x + area.x ) * pixelLength );

        math::Color3f   currentColor = GetColor3f( maxValue, ptrSrcPixel );

        const float     intensity    = currentColor.r;
        const float     intensityHighlights     = intensity * params.weight;
        const float     intensityRest           = 1.0f - intensityHighlights;
        const float     intensityShadows        = intensityRest * intensityRest;
        const float     intensityOriginal       = 1.0f - intensityHighlights - intensityShadows;

        math::Color3f   finalColor;

        finalColor      += math::overlay( currentColor, math::Color3f( highlights[0], highlights[1], highlights[2] ) ) * intensityHighlights;
        finalColor      += math::overlay( currentColor, math::Color3f( shadows[0], shadows[1], shadows[2] ) ) * intensityShadows;
        finalColor      += math::Color3f( intensity ) * intensityOriginal;

        finalColor       = math::minMaxUniform( finalColor );

        SetColor3f( maxValue, finalColor, ptrDstPixel );
    }
}

void splittone_CPU(
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
                    &cpuSplittone<short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_splittone( brightR, brightG, brightB, darkR, darkG, darkB, weight )
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
                    &cpuSplittone<float>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_splittone( brightR, brightG, brightB, darkR, darkG, darkB, weight )
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
                    &cpuSplittone<unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_splittone( brightR, brightG, brightB, darkR, darkG, darkB, weight )
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
                    &cpuSplittone<unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_splittone( brightR, brightG, brightB, darkR, darkG, darkB, weight )
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
