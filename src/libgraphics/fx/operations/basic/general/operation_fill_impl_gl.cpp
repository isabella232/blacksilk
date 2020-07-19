
#include <libgraphics/fx/operations/basic/general/gl.hpp>
#include <libgraphics/fx/operations/helpers/gl_helpers.hpp>

#include <limits>
#include <QDebug>

namespace libgraphics {
namespace fx {
namespace operations {


static const char fill_operation_kernel[] =
    "#version 120 \n"
    "uniform vec4 color;"
    "void main() {"
    "   gl_FragColor = color;"
    "}";
struct FillOperation : public libgraphics::backend::gl::ImageOperation {
    struct Params {
        backend::gl::EffectParameterVec4        color;
    };

    libcommon::ScopedPtr<backend::gl::Effect>   effect;
    Params                                      params;
    bool                                        initialized;
    float                                       r;
    float                                       g;
    float                                       b;
    float                                       a;

    FillOperation() : initialized( false ), r( 0.0f ), g( 0.0f ), b( 0.0f ),
        a( 0.0f ) {}
    virtual ~FillOperation() {}

    void initializeKernel() {
        if( !effect.empty() ) {
            return;
        }

        effect.reset(
            new backend::gl::Effect( "fill_operation_kernel", fill_operation_kernel )
        );

        if( effect->empty() ) {
            qDebug() << "Failed to compile filter: fill_operation_kernel";
            assert( false );
        }

        params.color.reset( "color", backend::gl::Vec4Data() );
    }

    virtual void submitImageMergeParameters( libgraphics::fxapi::ApiBackendDevice* device, libgraphics::fxapi::ApiImageObject* destination, libgraphics::fxapi::ApiImageObject* source, libgraphics::Rect32I area,
            backend::gl::Texture* texture, backend::gl::Texture* otherTexture, size_t tx, size_t ty ) {
        assert( false );

        ( void )device;
        ( void )destination;
        ( void )source;
        ( void )area;
        ( void )texture;
        ( void )otherTexture;
    }

    virtual void submitParameters( libgraphics::fxapi::ApiBackendDevice* device,
                                   libgraphics::fxapi::ApiImageObject* destination,
                                   libgraphics::fxapi::ApiImageObject* source,
                                   libgraphics::Rect32I   area,
                                   backend::gl::Texture* texture, size_t tx, size_t ty ) {
        ( void )device;
        ( void )destination;
        ( void )source;
        ( void )area;
        ( void )texture;

        /// apply parameters
        {
            params.color.reset(
                backend::gl::createVec4Data(
                    r, g, b, a
                )
            );

            backend::gl::applyEffectParameterToEffect( getEffect(), params.color );
        }
    }

    virtual backend::gl::Effect* getEffect() {
        return effect.get();
    }
    virtual bool initialize( libgraphics::fxapi::ApiBackendDevice* dev ) {
        ( void )dev;

        if( !this->initialized ) {
            this->initialized = true;
            this->initializeKernel();
        }

        return true;
    }

};


static const char opFill[] = "return vec4(Value,Value,Value,Value);";
void fill_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::Rect32I area,
    int value
) {
    LIBGRAPHICS_FILTER_INSTANCE_EX( GenericImgOpFromValue, device, filter, makeOpDirective( opFill ) );

    filter->value = ( float )value / 255.0f;

    const auto rendered = renderTiledImageOperation(
                              device,
                              dst,
                              dst,
                              area,
                              filter
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "add_GL: Failed to render operation.";
    }

}


static const char fill_channel_operation_kernel[] =
    "#version 120 \n"
    "uniform sampler2D mainTexture;"
    "uniform int width;"
    "uniform int height;"
    "uniform vec4 color;"
    "uniform int channelIndex;"
    "void main() {"
    " gl_FragColor = texture2D( mainTexture, vec2( gl_FragCoord.x / width, gl_FragCoord.y / height ) );"
    " if ( channelIndex == 0 )"
    "     gl_FragColor.r = color.r;"
    " if ( channelIndex == 1 )"
    "     gl_FragColor.g = color.g;"
    " if ( channelIndex == 2 )"
    "     gl_FragColor.b = color.b;"
    " if ( channelIndex == 3 )"
    "     gl_FragColor.a = color.a;"
    "}";
struct FillChannelOperation : public libgraphics::backend::gl::ImageOperation {
    struct Params {
        backend::gl::EffectParameterVec4        color;
        backend::gl::EffectParameterTexture     mainTexture;
        backend::gl::EffectParameterInt         width;
        backend::gl::EffectParameterInt         height;
        backend::gl::EffectParameterInt         channelIndex;
    };

    libcommon::ScopedPtr<backend::gl::Effect>   effect;
    Params                                      params;
    bool                                        initialized;
    float                                       r;
    float                                       g;
    float                                       b;
    float                                       a;
    size_t                                      channelIndex;

    FillChannelOperation() : initialized( false ), r( 0.0f ), g( 0.0f ), b( 0.0f ),
        a( 0.0f ), channelIndex( 0 ) {}
    virtual ~FillChannelOperation() {}

    void initializeKernel() {
        if( !effect.empty() ) {
            return;
        }

        effect.reset(
            new backend::gl::Effect( "fill_channel_operation_kernel", fill_channel_operation_kernel )
        );

        if( effect->empty() ) {
            qDebug() << "Failed to compile filter: fill_channel_operation_kernel";
            assert( false );
        }

        params.color.reset( "color", backend::gl::Vec4Data() );
        params.channelIndex.reset( "channelIndex", 0 );
        params.width.reset( "width", 0 );
        params.height.reset( "height", 0 );
        params.mainTexture.reset( "mainTexture", nullptr );
    }

    virtual void submitParameters( libgraphics::fxapi::ApiBackendDevice* device,
                                   libgraphics::fxapi::ApiImageObject* destination,
                                   libgraphics::fxapi::ApiImageObject* source,
                                   libgraphics::Rect32I   area,
                                   backend::gl::Texture* texture, size_t tx, size_t ty ) {
        ( void )device;
        ( void )destination;
        ( void )source;

        /// apply parameters
        {
            params.color.reset(
                backend::gl::createVec4Data(
                    r, g, b, a
                )
            );
            params.channelIndex.reset(
                ( int )channelIndex
            );
            params.height.reset(
                area.height
            );
            params.width.reset(
                area.width
            );
            params.mainTexture.reset(
                texture
            );

            backend::gl::applyEffectParameterToEffect( getEffect(), params.color );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.channelIndex );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.height );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.width );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.mainTexture );
        }
    }
    virtual void submitImageMergeParameters( libgraphics::fxapi::ApiBackendDevice* device, libgraphics::fxapi::ApiImageObject* destination, libgraphics::fxapi::ApiImageObject* source, libgraphics::Rect32I area,
            backend::gl::Texture* texture, backend::gl::Texture* otherTexture, size_t tx, size_t ty ) {
        assert( false );

        ( void )device;
        ( void )destination;
        ( void )source;
        ( void )area;
        ( void )texture;
        ( void )otherTexture;
    }

    virtual backend::gl::Effect* getEffect() {
        return effect.get();
    }
    virtual bool initialize( libgraphics::fxapi::ApiBackendDevice* dev ) {
        ( void )dev;

        if( !this->initialized ) {
            this->initialized = true;
            this->initializeKernel();
        }

        return true;
    }

};

void fill_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::Rect32I area,
    const libgraphics::Format& format,
    void* color
) {
    assert( device );
    assert( dst );
    assert( color );

    static libcommon::ScopedPtr<FillOperation> filter;

    if( filter.empty() ) {
        filter.reset( new FillOperation() );
    }

    filter->a = 0.0f;
    filter->r = 0.0f;
    filter->g = 0.0f;
    filter->b = 0.0f;

    const auto channelSize = format.byteSize / format.channels;
    assert( channelSize > 0 );

    if( channelSize == 0 ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "fill_GL(): Invalid format.";
#endif
        return;
    }


    bool success( true );

    switch( format.channels ) {
        case 1:
            switch( channelSize ) {
                case sizeof( unsigned char ):
                    filter->r = ( ( float ) * ( unsigned char* )color / std::numeric_limits<unsigned char>::max() );
                    break;

                case sizeof( unsigned short ):
                    filter->r = ( ( float ) * ( unsigned short* )color / std::numeric_limits<unsigned short>::max() );
                    break;

                case sizeof( unsigned int ):
                    filter->r = ( ( float ) * ( unsigned int* )color / std::numeric_limits<unsigned int>::max() );
                    break;

                default:
                    break;
            }

            break;

        case 2:
            switch( channelSize ) {
                case sizeof( unsigned char ):
                    filter->r = ( ( float ) * ( unsigned char* )color / std::numeric_limits<unsigned char>::max() );
                    filter->g = ( ( float ) * ( ( unsigned char* )color + 1 ) / std::numeric_limits<unsigned char>::max() );
                    break;

                case sizeof( unsigned short ):
                    filter->r = ( ( float ) * ( unsigned short* )color / std::numeric_limits<unsigned short>::max() );
                    filter->g = ( ( float ) * ( ( unsigned short* )color + 1 ) / std::numeric_limits<unsigned short>::max() );
                    break;

                case sizeof( unsigned int ):
                    filter->r = ( ( float ) * ( unsigned int* )color / std::numeric_limits<unsigned int>::max() );
                    filter->g = ( ( float ) * ( ( unsigned int* )color + 1 ) / std::numeric_limits<unsigned int>::max() );
                    break;

                default:
                    break;
            }

            break;

        case 3:
            switch( channelSize ) {
                case sizeof( unsigned char ):
                    filter->r = ( *( unsigned char* )color / std::numeric_limits<unsigned char>::max() );
                    filter->g = ( *( ( unsigned char* )color + 1 ) / std::numeric_limits<unsigned char>::max() );
                    filter->b = ( *( ( unsigned char* )color + 2 ) / std::numeric_limits<unsigned char>::max() );
                    break;

                case sizeof( unsigned short ):
                    filter->r = ( *( unsigned short* )color / std::numeric_limits<unsigned short>::max() );
                    filter->g = ( *( ( unsigned short* )color + 1 ) / std::numeric_limits<unsigned short>::max() );
                    filter->b = ( *( ( unsigned short* )color + 2 ) / std::numeric_limits<unsigned short>::max() );
                    break;

                case sizeof( unsigned int ):
                    filter->r = ( *( unsigned int* )color / std::numeric_limits<unsigned int>::max() );
                    filter->g = ( *( ( unsigned int* )color + 1 ) / std::numeric_limits<unsigned int>::max() );
                    filter->b = ( *( ( unsigned int* )color + 2 ) / std::numeric_limits<unsigned int>::max() );
                    break;

                default:
                    break;
            }

            break;

        case 4:
            switch( channelSize ) {
                case sizeof( unsigned char ):
                    filter->r = ( ( float ) * ( unsigned char* )color / std::numeric_limits<unsigned char>::max() );
                    filter->g = ( ( float ) * ( ( unsigned char* )color + 1 ) / std::numeric_limits<unsigned char>::max() );
                    filter->b = ( ( float ) * ( ( unsigned char* )color + 2 ) / std::numeric_limits<unsigned char>::max() );
                    filter->a = ( ( float ) * ( ( unsigned char* )color + 3 ) / std::numeric_limits<unsigned char>::max() );
                    break;

                case sizeof( unsigned short ):
                    filter->r = ( ( float ) * ( unsigned short* )color / std::numeric_limits<unsigned short>::max() );
                    filter->g = ( ( float ) * ( ( unsigned short* )color + 1 ) / std::numeric_limits<unsigned short>::max() );
                    filter->b = ( ( float ) * ( ( unsigned short* )color + 2 ) / std::numeric_limits<unsigned short>::max() );
                    filter->a = ( ( float ) * ( ( unsigned short* )color + 3 ) / std::numeric_limits<unsigned short>::max() );
                    break;

                case sizeof( unsigned int ):
                    filter->r = ( ( float ) * ( unsigned int* )color / std::numeric_limits<unsigned int>::max() );
                    filter->g = ( ( float ) * ( ( unsigned int* )color + 1 ) / std::numeric_limits<unsigned int>::max() );
                    filter->b = ( ( float ) * ( ( unsigned int* )color + 2 ) / std::numeric_limits<unsigned int>::max() );
                    filter->a = ( ( float ) * ( ( unsigned int* )color + 3 ) / std::numeric_limits<unsigned int>::max() );
                    break;

                default:
                    break;
            }

            break;

        default:
            success = false;
            break;
    }

    assert( success );

    if( !success ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "fill_GL(): Operation failed. Invalid format.";
#endif
        return;
    }

    libgraphics::fxapi::ApiImageObject* temporaryObject = device->createTexture2D();
    assert( temporaryObject );

    const auto successfullyInitialized = temporaryObject->create(
            dst->format(),
            dst->width(),
            dst->height()
                                         );

    assert( successfullyInitialized );

    if( !successfullyInitialized ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "fill_GL(): Failed to create temporary rendertarget.";
#endif
        device->destroyTexture2D( temporaryObject );

        return;
    }

    blit_GL(
        device,
        temporaryObject,
        dst,
        libgraphics::Rect32I( ( int )dst->width(), ( int )dst->height() ),
        libgraphics::Rect32I( ( int )dst->width(), ( int )dst->height() )
    );

    const bool rendered = renderTiledImageOperation(
                              device,
                              dst,
                              temporaryObject,
                              area,
                              filter.get()
                          );
    assert( rendered );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !rendered ) {
        qDebug() << "fill_GL(): Failed to apply operation to image.";
    }

#endif

    device->destroyTexture2D( temporaryObject );
}

static libcommon::ScopedPtr<FillChannelOperation> channelFilter;

template < class _t_any >
void fillChannelHelper(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::Rect32I area,
    size_t channelIndex,
    _t_any value
) {
    assert( device );
    assert( dst );

    if( channelFilter.empty() ) {
        channelFilter.reset( new FillChannelOperation );
    }

    bool success( true );

    switch( channelIndex ) {
        case 0:
            channelFilter->r = ( ( float )value / std::numeric_limits<_t_any>::max() );
            break;

        case 1:
            channelFilter->g = ( ( float )value / std::numeric_limits<_t_any>::max() );
            break;

        case 2:
            channelFilter->b = ( ( float )value / std::numeric_limits<_t_any>::max() );
            break;

        case 3:
            channelFilter->a = ( ( float )value / std::numeric_limits<_t_any>::max() );
            break;

        default:
            success = false;
            break;
    }

    channelFilter->channelIndex = channelIndex;

    assert( success );

    if( !success ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "fillChannel_GL(): Invalid channelIndex. Operation failed.";
#endif
        return;
    }

    libgraphics::fxapi::ApiImageObject* temporaryObject = device->createTexture2D();
    assert( temporaryObject );

    const auto successfullyInitialized = temporaryObject->create(
            dst->format(),
            dst->width(),
            dst->height()
                                         );

    assert( successfullyInitialized );

    if( !successfullyInitialized ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "fill_GL(): Failed to create temporary rendertarget.";
#endif
        device->destroyTexture2D( temporaryObject );

        return;
    }

    blit_GL(
        device,
        temporaryObject,
        dst,
        libgraphics::Rect32I( ( int )dst->width(), ( int )dst->height() ),
        libgraphics::Rect32I( ( int )dst->width(), ( int )dst->height() )
    );

    const bool rendered = renderTiledImageOperation(
                              device,
                              dst,
                              temporaryObject,
                              area,
                              channelFilter.get()
                          );
    assert( rendered );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !rendered ) {
        qDebug() << "fill_GL(): Failed to apply operation to image.";
    }

#endif

    device->destroyTexture2D( temporaryObject );
}

void fillChannel_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::Rect32I area,
    size_t channelIndex,
    unsigned char value
) {
    return fillChannelHelper<unsigned char>(
               device,
               dst,
               area,
               channelIndex,
               value
           );
}


void fillChannel_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::Rect32I area,
    size_t channelIndex,
    unsigned short value
) {
    return fillChannelHelper<unsigned short>(
               device,
               dst,
               area,
               channelIndex,
               value
           );
}

void fillChannel_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::Rect32I area,
    size_t channelIndex,
    unsigned int value
) {
    return fillChannelHelper<unsigned int>(
               device,
               dst,
               area,
               channelIndex,
               value
           );
}

void fillChannel_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::Rect32I area,
    size_t channelIndex,
    float value
) {
    return fillChannelHelper<float>(
               device,
               dst,
               area,
               channelIndex,
               value
           );
}

}
}
}
