

#include <libgraphics/fx/operations/complex.hpp>
#include <libgraphics/fx/operations/complex/gl.hpp>
#include <libgraphics/fx/operations/complex/cpu.hpp>
#include <libgraphics/fx/operations/helpers/cpu_helpers.hpp>

#include <cmath>
#include <QDebug>

namespace libgraphics {
namespace fx {
namespace operations {

struct kernel_gaussian_blur_pack {
    enum dir {
        vertical,
        horizontal
    };

    kernel_gaussian_blur_pack(
        dir _dir,
        float _radius
    ) : direction( _dir ), radius( _radius ) {}

    const float radius;
    const dir direction;
};

template < class _t_pixel_type >
void kernel_fixed_gaussian_blur(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::backend::cpu::ImageObject*   destination,
    libgraphics::backend::cpu::ImageObject*   source,
    libgraphics::Rect32I area,
    kernel_gaussian_blur_pack parameters
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

    float* values = new float[channelCount];
    memset( ( void* )values, 0, sizeof( float ) * channelCount );

    const auto maxValue = std::numeric_limits<_t_pixel_type>::max();
    const float sigmaSquared = ( parameters.radius * 1.141f );

    /// construct and normalize gaussian kernel.
    const int kernelLength = ceil( parameters.radius ) * 4 + 1;
    float* linearKernel = new float[kernelLength];
    float sum( 0.0f );

    {
        for( size_t i = 0; kernelLength > i; ++i ) {
            const int centeredIndex = ( int )i - ( kernelLength / 2 );
            linearKernel[i] = exp( -( centeredIndex * centeredIndex ) / sigmaSquared );
            sum += linearKernel[i];
        }

        for( size_t i = 0; kernelLength > i; ++i ) {
            linearKernel[i] /= sum;
        }
    }

    /// iterate over all pixels
    for( size_t p = 0; ( area.width * area.height ) > p; ++p ) {
        const size_t y = ( p - ( p % ( int )area.width ) ) / area.width;
        const size_t x = ( p - ( y * ( int )area.width ) );
        memset( ( void* )values, 0, sizeof( float ) * channelCount );

        _t_pixel_type* dstPixel = ( _t_pixel_type* )( ( char* )destinationBuffer ) + ( ( ( area.y + y ) * destination->width() ) + x + area.x ) * pixelLength;
        _t_pixel_type* srcPixel = ( _t_pixel_type* )( ( char* )sourceBuffer ) + ( ( ( area.y + y ) * source->width() ) + x + area.x ) * pixelLength;
        ( void )srcPixel;

        for( int i = 0; kernelLength > i; ++i ) {
            const int centeredIndex = i - ( kernelLength / 2 );

            int px( area.x + x );
            int py( area.y + y );

            if( kernel_gaussian_blur_pack::horizontal == parameters.direction ) {
                px += centeredIndex;

                if( px >= ( int )source->width() ) {
                    px = px - ( int )source->width();
                } else if( px < 0 ) {
                    px = ( int )source->width() - abs( px );
                }
            } else {
                py += centeredIndex;

                if( py >= ( int )source->height() ) {
                    py = py - ( int )source->height();
                } else if( py < 0 ) {
                    py = ( int )source->height() - abs( py );
                }
            }

            _t_pixel_type* pixel = ( _t_pixel_type* )( ( ( char* )sourceBuffer ) + ( ( ( ( py ) * source->width() ) + px ) * pixelLength ) );

            for( size_t n = 0; channelCount > n; ++n ) {
                values[n] += *( pixel + n ) * linearKernel[i];
            }
        }

        for( size_t n = 0; channelCount > n; ++n ) {
            *( dstPixel + n ) = std::max<_t_pixel_type>( 0, std::min<int>( maxValue, ( int )std::ceil( values[n] ) ) );
        }

    }

    delete [] linearKernel;
    delete [] values;
}

template < class _t_pixel_type >
void kernel_horizontal_gaussian_blur_rgb(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::backend::cpu::ImageObject*   destination,
    libgraphics::backend::cpu::ImageObject*   source,
    libgraphics::Rect32I area,
    kernel_gaussian_blur_pack parameters
) {
    typedef libgraphics::BasicPixel<_t_pixel_type, 3> Pixel;
    ( void )device;

    void*   destinationBuffer = destination->data();
    void*   sourceBuffer = source->data();

    const int size      = 4 * ceil( parameters.radius ) + 1;
    const int halfSize  = size / 2;
    const float div     = parameters.radius * 1.141f;

    float values[3] = {
        0.0f,
        0.0f,
        0.0f
    };

    const auto maxValue = std::numeric_limits<_t_pixel_type>::max();

    for( size_t p = 0; ( area.width * area.height ) > p; ++p ) {
        const size_t y = ( p - ( p % ( int )area.width ) ) / area.width;
        const size_t x = ( p - ( y * ( int )area.width ) );
        Pixel* dstPixel = ( ( Pixel* )destinationBuffer + ( ( area.y + y ) * destination->width() ) + x + area.x );

        float sum = 0.0f;

        for( int i = 0; size > i; ++i ) {
            int mx = i - halfSize;
            sum += exp( -( mx * mx ) / div );
        }

        values[0] = 0.0f; values[1] = 0.0f; values[2] = 0.0f;

        for( int i = 0; size > i; ++i ) {
            int mx = i - halfSize;
            int px = area.x + x + mx;
            int py = area.y + y;

            if( px >= ( int )source->width() ) {
                px = px - ( int )source->width();
            } else if( px < 0 ) {
                px = ( int )source->width() - abs( px );
            }

            Pixel* pixel = ( ( Pixel* )sourceBuffer + ( ( py  * source->width() ) + px ) );


            const auto fexpval = exp( -( mx * mx ) / div ) / sum;
            {
                values[0] += ( ( float )( *pixel ).Values[0] / maxValue ) * fexpval;
                values[1] += ( ( float )( *pixel ).Values[1] / maxValue ) * fexpval;
                values[2] += ( ( float )( *pixel ).Values[2] / maxValue ) * fexpval;
            }
        }

        ( *dstPixel ).Values[0] = std::max<_t_pixel_type>( 0, std::min<_t_pixel_type>( maxValue, std::ceil( ( float )maxValue * values[0] ) ) );
        ( *dstPixel ).Values[1] = std::max<_t_pixel_type>( 0, std::min<_t_pixel_type>( maxValue, std::ceil( ( float )maxValue * values[1] ) ) );
        ( *dstPixel ).Values[2] = std::max<_t_pixel_type>( 0, std::min<_t_pixel_type>( maxValue, std::ceil( ( float )maxValue * values[2] ) ) );
    }

}

template < class _t_pixel_type >
void kernel_vertical_gaussian_blur_rgb(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::backend::cpu::ImageObject*   destination,
    libgraphics::backend::cpu::ImageObject*   source,
    libgraphics::Rect32I area,
    kernel_gaussian_blur_pack parameters
) {
    typedef libgraphics::BasicPixel<_t_pixel_type, 3> Pixel;
    ( void )device;

    void*   destinationBuffer = destination->data();
    void*   sourceBuffer = source->data();

    const int size      = 4 * ceil( parameters.radius ) + 1;
    const int halfSize  = size / 2;
    const float div     = parameters.radius * 1.141f;

    float values[3] = {
        0.0f,
        0.0f,
        0.0f
    };

    const auto maxValue = std::numeric_limits<_t_pixel_type>::max();

    for( size_t p = 0; ( area.width * area.height ) > p; ++p ) {
        const size_t y = ( p - ( p % ( int )area.width ) ) / area.width;
        const size_t x = ( p - ( y * ( int )area.width ) );
        Pixel* dstPixel = ( ( Pixel* )destinationBuffer + ( ( area.y + y ) * destination->width() ) + x + area.x );

        float sum = 0.0f;

        for( int i = 0; size > i; ++i ) {
            int mx = i - halfSize;
            sum += exp( -( mx * mx ) / div );
        }

        values[0] = 0.0f; values[1] = 0.0f; values[2] = 0.0f;

        for( int i = 0; size > i; ++i ) {
            int mx = i - halfSize;
            int px = area.x + x;
            int py = area.y + y + mx;

            if( py >= ( int )source->height() ) {
                py = py - ( int )source->height();
            } else if( py < 0 ) {
                py = ( int )source->height() - abs( py );
            }

            Pixel* pixel = ( ( Pixel* )sourceBuffer + ( ( py  * source->width() ) + px ) );


            const auto fexpval = exp( -( mx * mx ) / div ) / sum;
            {
                values[0] += ( ( float )( *pixel ).Values[0] / maxValue ) * fexpval;
                values[1] += ( ( float )( *pixel ).Values[1] / maxValue ) * fexpval;
                values[2] += ( ( float )( *pixel ).Values[2] / maxValue ) * fexpval;
            }
        }

        ( *dstPixel ).Values[0] = std::max<_t_pixel_type>( 0, std::min<_t_pixel_type>( maxValue, std::ceil( ( float )maxValue * values[0] ) ) );
        ( *dstPixel ).Values[1] = std::max<_t_pixel_type>( 0, std::min<_t_pixel_type>( maxValue, std::ceil( ( float )maxValue * values[1] ) ) );
        ( *dstPixel ).Values[2] = std::max<_t_pixel_type>( 0, std::min<_t_pixel_type>( maxValue, std::ceil( ( float )maxValue * values[2] ) ) );
    }
}

template < class _t_pixel_type >
void kernel_directional_gaussian_blur(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::backend::cpu::ImageObject*   destination,
    libgraphics::backend::cpu::ImageObject*   source,
    libgraphics::Rect32I area,
    kernel_gaussian_blur_pack parameters
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

    const int size      = 4 * ceil( parameters.radius ) + 1;
    const int halfSize  = size / 2;
    const float div     = parameters.radius * 1.141f;

    float* values = new float[channelCount];
    memset( ( void* )values, 0, sizeof( float ) * channelCount );

    const auto maxValue = std::numeric_limits<_t_pixel_type>::max();

    for( size_t p = 0; ( area.width * area.height ) > p; ++p ) {
        const size_t y = ( p - ( p % ( int )area.width ) ) / area.width;
        const size_t x = ( p - ( y * ( int )area.width ) );

        _t_pixel_type* dstPixel = ( _t_pixel_type* )( ( char* )destinationBuffer ) + ( ( ( area.y + y ) * destination->width() ) + x + area.x ) * pixelLength;
        _t_pixel_type* srcPixel = ( _t_pixel_type* )( ( char* )sourceBuffer ) + ( ( ( area.y + y ) * source->width() ) + x + area.x ) * pixelLength;
        ( void )srcPixel;

        float sum = 0.0f;

        for( int i = 0; size > i; ++i ) {
            int mx = i - halfSize;
            sum += exp( -( mx * mx ) / div );
        }

        memset( ( void* )values, 0, sizeof( float ) * channelCount );

        if( parameters.direction ==  kernel_gaussian_blur_pack::horizontal ) {
            for( int i = 0; size > i; ++i ) {
                int mx = i - halfSize;
                int px = area.x + x + mx;
                int py = area.y + y;

                if( px >= ( int )source->width() ) {
                    px = px - ( int )source->width();
                } else if( px < 0 ) {
                    px = ( int )source->width() - abs( px );
                }

                _t_pixel_type* pixel = ( _t_pixel_type* )( ( ( char* )sourceBuffer ) + ( ( ( ( py ) * source->width() ) + px ) * pixelLength ) );

                const auto fexpval = exp( -( mx * mx ) / div );
                float lval = 0.0f;
                float pval = 0.0f;

                for( size_t n = 0; channelCount > n; ++n ) {
                    pval = ( ( ( float ) * ( pixel + n ) ) / maxValue );
                    lval =  fexpval * pval / sum;
                    values[n] += lval;
                }
            }

            for( size_t n = 0; channelCount > n; ++n ) {
                const auto value = std::max<_t_pixel_type>( 0, std::min<_t_pixel_type>( maxValue, std::ceil( ( float )maxValue * values[n] ) ) );
                *( dstPixel + n ) = value;
            }
        } else {
            for( int i = 0; size > i; ++i ) {
                int mx = i - halfSize;
                int px = area.x + x;
                int py = area.y + y + mx;

                if( py >= ( int )source->height() ) {
                    py = py - ( int )source->height();
                } else if( py < 0 ) {
                    py = ( int )source->height() - abs( py );
                }

                _t_pixel_type* pixel = ( _t_pixel_type* )( ( ( char* )sourceBuffer ) + ( ( ( ( py ) * source->width() ) + px ) * pixelLength ) );

                const auto fexpval = exp( -( mx * mx ) / div );
                float lval = 0.0f;
                float pval = 0.0f;

                for( size_t n = 0; channelCount > n; ++n ) {
                    pval = ( ( ( float ) * ( pixel + n ) ) / maxValue );
                    lval =  fexpval * pval / sum;
                    values[n] += lval;
                }
            }

            for( size_t n = 0; channelCount > n; ++n ) {
                const auto value = std::max<_t_pixel_type>( 0, std::min<_t_pixel_type>( maxValue, std::ceil( ( float )maxValue * values[n] ) ) );
                *( dstPixel + n ) = value;
            }
        }

    }

    delete [] values;

}



void gaussianBlur_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* destination,
    libgraphics::fxapi::ApiImageObject* source,
    Rect32I area,
    float radius
) {
    assert( device );
    assert( source );

    libgraphics::fxapi::ApiScopedImgRef    temporaryLayer(
        device,
        destination->format(),
        destination->width(),
        destination->height()
    );

    horizontalGaussianBlur_CPU(
        device,
        temporaryLayer,
        source,
        area,
        radius
    );
    verticalGaussianBlur_CPU(
        device,
        destination,
        temporaryLayer,
        area,
        radius
    );
}

void verticalGaussianBlur_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* destination,
    libgraphics::fxapi::ApiImageObject* source,
    Rect32I area,
    float radius
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
                    &kernel_directional_gaussian_blur<unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_gaussian_blur_pack( kernel_gaussian_blur_pack::vertical, radius )
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
                    &kernel_directional_gaussian_blur<unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_gaussian_blur_pack( kernel_gaussian_blur_pack::vertical, radius )
                )
            );

        case fxapi::EPixelFormat::RGB8:

            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )destination,
                ( backend::cpu::ImageObject* )source,
                area,
                std::bind(
                    &kernel_fixed_gaussian_blur<unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_gaussian_blur_pack( kernel_gaussian_blur_pack::vertical, radius )
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
                    &kernel_fixed_gaussian_blur<unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_gaussian_blur_pack( kernel_gaussian_blur_pack::vertical, radius )
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
                    &kernel_directional_gaussian_blur<unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_gaussian_blur_pack( kernel_gaussian_blur_pack::vertical, radius )
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
                    &kernel_directional_gaussian_blur<unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_gaussian_blur_pack( kernel_gaussian_blur_pack::vertical, radius )
                )
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }
}

void horizontalGaussianBlur_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* destination,
    libgraphics::fxapi::ApiImageObject* source,
    Rect32I area,
    float radius
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
                    &kernel_directional_gaussian_blur<unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_gaussian_blur_pack( kernel_gaussian_blur_pack::horizontal, radius )
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
                    &kernel_directional_gaussian_blur<unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_gaussian_blur_pack( kernel_gaussian_blur_pack::horizontal, radius )
                )
            );

        case fxapi::EPixelFormat::RGB8:

            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )destination,
                ( backend::cpu::ImageObject* )source,
                area,
                std::bind(
                    &kernel_fixed_gaussian_blur<unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_gaussian_blur_pack( kernel_gaussian_blur_pack::horizontal, radius )
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
                    &kernel_fixed_gaussian_blur<unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_gaussian_blur_pack( kernel_gaussian_blur_pack::horizontal, radius )
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
                    &kernel_directional_gaussian_blur<unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_gaussian_blur_pack( kernel_gaussian_blur_pack::horizontal, radius )
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
                    &kernel_directional_gaussian_blur<unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    kernel_gaussian_blur_pack( kernel_gaussian_blur_pack::horizontal, radius )
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
