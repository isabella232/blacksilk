
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

struct kernel_cascaded_sharpen_params {
    kernel_cascaded_sharpen_params() :
        blurred0( nullptr ), blurred1( nullptr ),
        blurred2( nullptr ), blurred3( nullptr ),
        intensity0( 0.0f ), intensity1( 0.0f ),
        intensity2( 0.0f ), intensity3( 0.0f ),
        threshold( 0.0f ) {}

    backend::cpu::ImageObject*      blurred0;
    backend::cpu::ImageObject*      blurred1;
    backend::cpu::ImageObject*      blurred2;
    backend::cpu::ImageObject*      blurred3;
    float                           threshold;
    float                           intensity0;
    float                           intensity1;
    float                           intensity2;
    float                           intensity3;
};


template <  class _t_pixel_type, int _t_pixel_max >
void cpuCascadedSharpenWith4(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::backend::cpu::ImageObject*   destination,
    libgraphics::backend::cpu::ImageObject*   source,
    libgraphics::Rect32I area,
    kernel_cascaded_sharpen_params params
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

    if( params.blurred0->format() != destination->format() ) {
        assert( false );
        return;
    }

    if( params.blurred1->format() != destination->format() ) {
        assert( false );
        return;
    }

    if( params.blurred2->format() != destination->format() ) {
        assert( false );
        return;
    }

    if( params.blurred3->format() != destination->format() ) {
        assert( false );
        return;
    }

    static const auto maxValue = std::numeric_limits<_t_pixel_type>::max();


    void*   blurBuffer0 = params.blurred0->data();
    void*   blurBuffer1 = params.blurred1->data();
    void*   blurBuffer2 = params.blurred2->data();
    void*   blurBuffer3 = params.blurred3->data();

    typedef math::Color3i <
    _t_pixel_max
    > Color3;

    const float quotient = 1.0f - std::max<float>( params.threshold / 100.0f, 0.01f );

    for( size_t p = 0; ( area.width * area.height ) > p; ++p ) {
        const size_t y = ( p - ( p % ( int )area.width ) ) / area.width;
        const size_t x = ( p - ( y * ( int )area.width ) );

        _t_pixel_type* ptrDstPixel = ( _t_pixel_type* )( ( ( char* )destinationBuffer ) + ( ( ( area.y + y ) * destination->width() ) + x + area.x ) * pixelLength );
        _t_pixel_type* ptrSrcPixel = ( _t_pixel_type* )( ( ( char* )sourceBuffer ) + ( ( ( area.y + y ) * source->width() ) + x + area.x ) * pixelLength );

        _t_pixel_type* ptrBlurPixel0 = ( _t_pixel_type* )( ( ( char* )blurBuffer0 ) + ( ( ( area.y + y ) * source->width() ) + x + area.x ) * pixelLength );
        _t_pixel_type* ptrBlurPixel1 = ( _t_pixel_type* )( ( ( char* )blurBuffer1 ) + ( ( ( area.y + y ) * source->width() ) + x + area.x ) * pixelLength );
        _t_pixel_type* ptrBlurPixel2 = ( _t_pixel_type* )( ( ( char* )blurBuffer2 ) + ( ( ( area.y + y ) * source->width() ) + x + area.x ) * pixelLength );
        _t_pixel_type* ptrBlurPixel3 = ( _t_pixel_type* )( ( ( char* )blurBuffer3 ) + ( ( ( area.y + y ) * source->width() ) + x + area.x ) * pixelLength );

        Color3  basePixel = GetColor3i( Color3, ptrSrcPixel );
        Color3  blurred0  = GetColor3i( Color3, ptrBlurPixel0 );
        Color3  blurred1  = GetColor3i( Color3, ptrBlurPixel1 );
        Color3  blurred2  = GetColor3i( Color3, ptrBlurPixel2 );
        Color3  blurred3  = GetColor3i( Color3, ptrBlurPixel3 );

        Color3  raw0      = blurred0 - basePixel;
        Color3  raw1      = blurred1 - basePixel;
        Color3  raw2      = blurred2 - basePixel;
        Color3  raw3      = blurred3 - basePixel;

        Color3 weighted0  = raw0            * ( params.intensity0 / 100.0f );
        Color3 weighted1  = ( raw1 - raw0 )   * ( params.intensity1 / 100.0f );
        Color3 weighted2  = ( raw2 - raw1 )   * ( params.intensity2 / 100.0f );
        Color3 weighted3  = ( raw3 - raw2 )   * ( params.intensity3 / 100.0f );

        Color3 w0         = math::grainExtract( blurred0, basePixel );
        Color3 w1         = math::grainExtract( blurred1, basePixel );
        Color3 w2         = math::grainExtract( blurred2, basePixel );
        Color3 w3         = math::grainExtract( blurred3, basePixel );

        Color3 overall(
            ( int )std::max<float>( 0.0, std::min<float>( maxValue, std::max<float>( w0.r, std::max<float>( w1.r, std::max<float>( w2.r, w3.r ) ) ) * quotient ) ),
            ( int )std::max<float>( 0.0, std::min<float>( maxValue, std::max<float>( w0.g, std::max<float>( w1.g, std::max<float>( w2.g, w3.g ) ) ) * quotient ) ),
            ( int )std::max<float>( 0.0, std::min<float>( maxValue, std::max<float>( w0.b, std::max<float>( w1.b, std::max<float>( w2.b, w3.b ) ) ) * quotient ) )
        );

        Color3 sum  = weighted0 + weighted1 + weighted2 + weighted3;
        Color3 temp = ( ( overall * ( basePixel - sum ) ) + ( overall.negate() * basePixel ) );

        SetColor3i( Color3, temp, ptrDstPixel );
    }
}


void cascadedSharpenWith4_CPU(
    libgraphics::fxapi::ApiBackendDevice*   device,
    libgraphics::fxapi::ApiImageObject*     dst,
    libgraphics::fxapi::ApiImageObject*     src,
    Rect32I                                 area,
    const std::tuple< libgraphics::ImageLayer*, float, float >& cascade0,
    const std::tuple< libgraphics::ImageLayer*, float, float >& cascade1,
    const std::tuple< libgraphics::ImageLayer*, float, float >& cascade2,
    const std::tuple< libgraphics::ImageLayer*, float, float >& cascade3,
    float threshold
) {

    assert( device != nullptr );
    assert( dst != nullptr );
    assert( src != nullptr );

    float   p( 0.0f );

    float   intensity0( 0.0f );
    float   intensity1( 0.0f );
    float   intensity2( 0.0f );
    float   intensity3( 0.0f );

    libgraphics::ImageLayer* blurred0( nullptr );
    libgraphics::ImageLayer* blurred1( nullptr );
    libgraphics::ImageLayer* blurred2( nullptr );
    libgraphics::ImageLayer* blurred3( nullptr );

    std::tie( blurred0, p, intensity0 ) = cascade0;
    std::tie( blurred1, p, intensity1 ) = cascade1;
    std::tie( blurred2, p, intensity2 ) = cascade2;
    std::tie( blurred3, p, intensity3 ) = cascade3;

    kernel_cascaded_sharpen_params params;
    params.blurred0 = ( backend::cpu::ImageObject* )blurred0->internalImageForBackend( FXAPI_BACKEND_CPU );
    params.blurred1 = ( backend::cpu::ImageObject* )blurred1->internalImageForBackend( FXAPI_BACKEND_CPU );
    params.blurred2 = ( backend::cpu::ImageObject* )blurred2->internalImageForBackend( FXAPI_BACKEND_CPU );
    params.blurred3 = ( backend::cpu::ImageObject* )blurred3->internalImageForBackend( FXAPI_BACKEND_CPU );

    params.intensity0   = intensity0;
    params.intensity1   = intensity1;
    params.intensity2   = intensity2;
    params.intensity3   = intensity3;

    params.threshold    = threshold;

    switch( dst->format() ) {
        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src,
                area,
                std::bind(
                    &cpuCascadedSharpenWith4<signed short, SHRT_MAX>,
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
                    &cpuCascadedSharpenWith4<float, INT_MAX>,
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
                    &cpuCascadedSharpenWith4<unsigned char, UCHAR_MAX>,
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
                    &cpuCascadedSharpenWith4<unsigned short, USHRT_MAX>,
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
