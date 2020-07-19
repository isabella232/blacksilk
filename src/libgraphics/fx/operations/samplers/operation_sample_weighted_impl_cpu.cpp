
#include <assert.h>
#include <QDebug>
#include <math.h>

#include <libgraphics/fx/operations/samplers/cpu.hpp>
#include <libgraphics/fx/operations/helpers/cpu_helpers.hpp>
#include <libgraphics/backend/cpu/cpu_backenddevice.hpp>
#include <libgraphics/backend/cpu/cpu_imageobject.hpp>

namespace libgraphics {
namespace fx {
namespace operations {

template < class _t_pixel_type, size_t _v_matrix_size >
void sampleByMatrix(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src,
    float       factor, /// between 0.001 and 0.999
    float*      weights
) {
    assert( device != nullptr );
    assert( weights != nullptr );
    assert( ( factor > 0.001 ) && ( factor < 1.0 ) );
    assert( dst->format() == src->format() );


    libgraphics::backend::cpu::BackendDevice*   cpuDevice = ( libgraphics::backend::cpu::BackendDevice* )device;
    assert( cpuDevice != nullptr );

    libgraphics::backend::cpu::ImageObject*     cpuDst    = ( libgraphics::backend::cpu::ImageObject* )dst;
    libgraphics::backend::cpu::ImageObject*     cpuSrc    = ( libgraphics::backend::cpu::ImageObject* )src;
    assert( cpuDst != nullptr );
    assert( cpuSrc != nullptr );

    static const size_t matrixRowSize       = _v_matrix_size;
    static const size_t matrixColumnSize    = _v_matrix_size;

    const size_t  scaledWidth  = std::floor( ( float )src->width() * factor );
    const size_t  scaledHeight = std::floor( ( float )src->height() * factor );
    assert( ( scaledWidth * scaledHeight ) != 0 );

    if( ( scaledWidth * scaledHeight ) == 0 ) {
        return;
    }

    cpuDst->discardBuffers();
    const auto successfullyCreated = cpuDst->create(
                                         cpuSrc->format(),
                                         scaledWidth,
                                         scaledHeight
                                     );
    assert( successfullyCreated );

    const float         prX             = ( float )( cpuSrc->width() - 1 ) / scaledWidth;
    const float         prY             = ( float )( cpuSrc->height() - 1 ) / scaledHeight;
    const size_t        channelCount    = libgraphics::fxapi::EPixelFormat::getChannelCount( src->format() );

    _t_pixel_type* srcBuffer            = ( _t_pixel_type* )cpuSrc->data();
    _t_pixel_type* dstBuffer            = ( _t_pixel_type* )cpuDst->data();

    assert( srcBuffer != nullptr );
    assert( dstBuffer != nullptr );


    for( size_t c = 0; channelCount > c; ++c ) {
        for( int p = 0; ( int )scaledWidth * ( int )scaledHeight > p; ++p ) {

            const int y = ( p - ( p % ( int )scaledWidth ) ) / scaledWidth;
            const int x = ( p - ( y * ( int )scaledWidth ) );

            int totalChannelValue( 0 );

            for( size_t i = 0; ( matrixRowSize * matrixColumnSize ) > i; ++i ) {
                const int matrixY = ( i - ( i % matrixRowSize ) ) / matrixRowSize;
                const int matrixX = ( i - ( matrixY * matrixRowSize ) ); // - ( matrixRowSize / 2 );

                const float weightedFactor = weights[( matrixY * matrixRowSize ) + matrixX ];

                int srcX    = std::floor( ( x + matrixY ) * prX );
                int srcY    = std::floor( ( y + matrixX ) * prY );
                srcX        = ( srcX < 0 ) ? 0 : srcX;
                srcY        = ( srcY < 0 ) ? 0 : srcY;

                srcX        = ( srcX >= cpuSrc->width() ) ? ( cpuSrc->width() - 1 ) : srcX;
                srcY        = ( srcY >= cpuSrc->height() ) ? ( cpuSrc->height() - 1 ) : srcY;

                totalChannelValue += ( int ) fabs( ( float )( *( ( ( _t_pixel_type* )srcBuffer ) + ( ( srcY * cpuSrc->width() ) + srcX ) * channelCount + c ) ) * weightedFactor );
            }

            *( ( ( _t_pixel_type* )dstBuffer ) + ( ( y * cpuDst->width() ) + x )  * channelCount + c ) = ( _t_pixel_type )totalChannelValue;
        }
    }

}

template < size_t _v_matrix_size >
void sampleByMatrixHelper(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src,
    float       factor, /// between 0.001 and 0.999
    float*      weights
) {
    assert( device != nullptr );
    assert( weights != nullptr );
    assert( ( factor > 0.001 ) && ( factor < 1.0 ) );
    assert( dst->format() == src->format() );

    switch( dst->format() ) {

        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
        case fxapi::EPixelFormat::Mono16S:
            sampleByMatrix<signed short, _v_matrix_size>(
                device,
                dst,
                src,
                factor,
                weights
            );
            break;

        case fxapi::EPixelFormat::RGB32F:
        case fxapi::EPixelFormat::RGBA32F:
        case fxapi::EPixelFormat::Mono32F:
            sampleByMatrix<float, _v_matrix_size>(
                device,
                dst,
                src,
                factor,
                weights
            );
            break;

        case fxapi::EPixelFormat::RGB8:
        case fxapi::EPixelFormat::RGBA8:
        case fxapi::EPixelFormat::Mono8:
            sampleByMatrix<unsigned char, _v_matrix_size>(
                device,
                dst,
                src,
                factor,
                weights
            );
            break;

        case fxapi::EPixelFormat::RGBA16:
        case fxapi::EPixelFormat::RGB16:
        case fxapi::EPixelFormat::Mono16:
            sampleByMatrix<unsigned short, _v_matrix_size>(
                device,
                dst,
                src,
                factor,
                weights
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }
}


void sampleWeightedSum2x2_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src,
    float       factor, /// between 0.001 and 0.999
    float*      weights
) {
    sampleByMatrixHelper<2>(
        device,
        dst,
        src,
        factor,
        weights
    );
}

void sampleWeightedSum3x3_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src,
    float       factor, /// between 0.001 and 0.999
    float*      weights
) {
    sampleByMatrixHelper<3>(
        device,
        dst,
        src,
        factor,
        weights
    );
}

void sampleWeightedSum4x4_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src,
    float       factor, /// between 0.001 and 0.999
    float*      weights
) {
    sampleByMatrixHelper<4>(
        device,
        dst,
        src,
        factor,
        weights
    );
}

void sampleWeightedSum5x5_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src,
    float       factor, /// between 0.001 and 0.999
    float*      weights
) {
    sampleByMatrixHelper<5>(
        device,
        dst,
        src,
        factor,
        weights
    );
}

void sampleWeightedSum6x6_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src,
    float       factor, /// between 0.001 and 0.999
    float*      weights
) {
    sampleByMatrixHelper<6>(
        device,
        dst,
        src,
        factor,
        weights
    );
}


}
}
}
