#include <libgraphics/image.hpp>
#include <libgraphics/backend/common/formats.hpp>
#include <QDebug>

#include <log/log.hpp>

namespace libgraphics {

struct ImageLayerMask::Private {
    struct BackendImageObj {
        const int backendId;
        libgraphics::fxapi::ApiBackendDevice* device;
        fxapi::ApiImageObject*  imageObject;

        BackendImageObj( int _id, fxapi::ApiBackendDevice* _dev ) :
            backendId( _id ), device( _dev ), imageObject( nullptr ) {
            assert( device );

            imageObject = device->createTexture2D();
            assert( imageObject );
        }
        explicit BackendImageObj( fxapi::ApiBackendDevice* _dev ) :
            backendId( _dev->backendId() ), device( _dev ), imageObject( nullptr ) {
            assert( device );

            imageObject = device->createTexture2D();
            assert( imageObject );
        }
        BackendImageObj(
            fxapi::ApiBackendDevice* _device,
            void* source,
            size_t width,
            size_t height,
            libgraphics::fxapi::EPixelFormat::t format
        ) : backendId( _device->backendId() ), device( _device ), imageObject( nullptr ) {

            assert( device );
            assert( source );
            assert( width > 0 );
            assert( height > 0 );
            assert( format != libgraphics::fxapi::EPixelFormat::Empty );

            imageObject = device->createTexture2D();
            assert( imageObject );

            const auto successfullyInitializedFromData = imageObject->createFromData(
                        format,
                        width,
                        height,
                        source
                    );
            assert( successfullyInitializedFromData );

            if( !successfullyInitializedFromData ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
                qDebug() << "BackendImageObj::BackendImageObj(): Failed to initialize imageObject from data.";
#endif

                this->reset();
            }
        }

        ~BackendImageObj() {
            const auto ret = device->destroyTexture2D(
                                 imageObject
                             );
            assert( ret );

            ( void )ret;
        }
        bool reset() {
            assert( imageObject );

            const auto ret = device->destroyTexture2D(
                                 imageObject
                             );
            assert( ret );

            if( ret ) {
                imageObject = device->createTexture2D();
            }

            return ret;
        }
    };
    std::vector<BackendImageObj>    objects;
    size_t                          width;
    size_t                          height;
    fxapi::EPixelFormat::t          format;

    Private() : width( 0 ), height( 0 ),
        format( fxapi::EPixelFormat::Empty ) {}

    BackendImageObj* objectForBackend( int backendId ) {
        for( auto it = objects.begin(); it != objects.end(); ++it ) {
            if( ( *it ).backendId == backendId ) {
                return &( *it );
            }
        }

        return nullptr;
    }
    const BackendImageObj* objectForBackend( int backendId ) const {
        for( auto it = objects.begin(); it != objects.end(); ++it ) {
            if( ( *it ).backendId == backendId ) {
                return &( *it );
            }
        }

        return nullptr;
    }
    BackendImageObj* objectForDevice( fxapi::ApiBackendDevice* device ) {
        for( auto it = objects.begin(); it != objects.end(); ++it ) {
            if( ( *it ).device == device ) {
                return &( *it );
            }
        }

        return nullptr;
    }
    const BackendImageObj* objectForDevice( fxapi::ApiBackendDevice* device ) const {
        for( auto it = objects.begin(); it != objects.end(); ++it ) {
            if( ( *it ).device == device ) {
                return &( *it );
            }
        }

        return nullptr;
    }

    void assign( const ImageLayerMask& rhs ) {
        assert( rhs.empty() );

        objects.clear();

        format       = rhs.format();
        width        = rhs.width();
        height       = rhs.height();

        for( auto it = rhs.d->objects.begin(); it != rhs.d->objects.end(); ++it ) {
            objects.emplace_back(
                ( *it ).device
            );

            const auto sucessfullyCreated = objects.back().imageObject->create(
                                                format,
                                                width,
                                                height
                                            );
            assert( sucessfullyCreated );
            ( void )sucessfullyCreated;

            const auto sucessfullyCopied = objects.back().imageObject->copy(
                                               ( *it ).imageObject,
                                               libgraphics::Rect32I( ( int )width, ( int )height ),
                                               0,
                                               0
                                           );
            assert( sucessfullyCopied );
            ( void )sucessfullyCopied;
        }
    }

    bool isCompatibleFormat( libgraphics::fxapi::EPixelFormat::t format ) const {
        switch( format ) {
            case libgraphics::fxapi::EPixelFormat::Mono8:
            case libgraphics::fxapi::EPixelFormat::Mono16:
                return true;

            default: break;
        }

        return false;
    }
};


void copyChannelData( void* dst, void* src, libgraphics::Rect32I area, int width, int height, libgraphics::fxapi::EPixelFormat::t format, size_t sourceChannelIndex, size_t destChannelIndex ) {
    LOGB_ASSERT( dst, "invalid dst" );
    LOGB_ASSERT( src, "invalid src" );
    LOGB_ASSERT( width >= area.width + area.x, "bad width" );
    LOGB_ASSERT( height >= area.height + area.y, "bad height" );

    const auto formatChannelCount = libgraphics::fxapi::EPixelFormat::getChannelCount(
                                        format
                                    );
    const auto formatPixelSize = libgraphics::fxapi::EPixelFormat::getPixelSize(
                                     format
                                 );
    const auto formatChannelSize = formatPixelSize / formatChannelCount;

    assert( formatChannelCount > 0 );
    assert( formatPixelSize > 0 );
    assert( formatChannelSize > 0 );

    for( int x = 0; area.width > x; ++x ) {
        for( int y = 0; area.height > y; ++y ) {
            ( void ) memcpy(
                ( void* )( ( char* )dst + ( ( ( area.y * width ) + ( y * width ) + x + area.x ) * formatPixelSize ) + ( destChannelIndex * formatChannelSize ) ),
                ( const void* )( ( char* )src + ( ( ( area.y * width ) + ( y * width ) + x + area.x ) * formatChannelSize ) + ( sourceChannelIndex * formatChannelSize ) ),
                formatChannelSize
            );
        }
    }
}
void copyChannelData( void* dst, void* src, libgraphics::Rect32I area, int width, int height, libgraphics::fxapi::EPixelFormat::t format, size_t channelIndex ) {
    LOGB_ASSERT( dst, "invalid dst" );
    LOGB_ASSERT( src, "invalid src" );
    LOGB_ASSERT( width >= area.width + area.x, "bad width" );
    LOGB_ASSERT( height >= area.height + area.y, "bad height" );

    const auto formatChannelCount = libgraphics::fxapi::EPixelFormat::getChannelCount(
                                        format
                                    );
    const auto formatPixelSize = libgraphics::fxapi::EPixelFormat::getPixelSize(
                                     format
                                 );
    const auto formatChannelSize = formatPixelSize / formatChannelCount;

    assert( formatChannelCount > 0 );
    assert( formatPixelSize > 0 );
    assert( formatChannelSize > 0 );

    for( int x = 0; area.width > x; ++x ) {
        for( int y = 0; area.height > y; ++y ) {
            ( void ) memcpy(
                ( void* )( ( char* )dst + ( ( y * area.width ) + x ) * formatChannelSize ),
                ( const void* )( ( char* )src + ( ( ( area.y * width ) + ( y * width ) + x + area.x ) * formatPixelSize ) + ( channelIndex * formatChannelSize ) ),
                formatChannelSize
            );
        }
    }
}
void copyChannelDataToBitmap( void* dst, void* src, libgraphics::Rect32I area, int width, int height, libgraphics::fxapi::EPixelFormat::t format, size_t channelIndex ) {
    LOGB_ASSERT( dst, "invalid dst" );
    LOGB_ASSERT( src, "invalid src" );
    LOGB_ASSERT( width >= area.width + area.x, "bad width" );
    LOGB_ASSERT( height >= area.height + area.y, "bad height" );

    const auto formatChannelCount = libgraphics::fxapi::EPixelFormat::getChannelCount(
                                        format
                                    );
    const auto formatPixelSize = libgraphics::fxapi::EPixelFormat::getPixelSize(
                                     format
                                 );
    const auto formatChannelSize = formatPixelSize / formatChannelCount;

    assert( formatChannelCount > 0 );
    assert( formatPixelSize > 0 );
    assert( formatChannelSize > 0 );

    for( int x = 0; area.width > x; ++x ) {
        for( int y = 0; area.height > y; ++y ) {
            ( void ) memcpy(
                ( void* )( ( char* )dst + ( ( ( area.y * width ) + ( y * width ) + x + area.x ) * formatPixelSize ) + ( channelIndex * formatChannelSize ) ),
                ( const void* )( ( char* )src + ( ( y * area.width ) + x ) * formatChannelSize ),
                formatChannelSize
            );
        }
    }
}

/// copy
bool copyData( fxapi::ApiBackendDevice* device, fxapi::ApiImageObject* dst, fxapi::ApiImageObject* src, libgraphics::Rect32I area, int destX, int destY ) {
    assert( dst );
    assert( src );
    assert( dst->width() >= area.width + destX );
    assert( dst->height() >= area.height + destY );
    assert( src->width() >= area.width + area.x );
    assert( src->height() >= area.height + area.y );
    assert( src->format() == dst->format() );

    if( !dst || !src ) {
        return false;
    }

    if( src->format() != dst->format() ) {
        return false;
    }

    if( dst->width() < area.width + destX ) {
        return false;
    }

    if( dst->height() < area.height + destY ) {
        return false;
    }

    if( src->width() < area.width + area.x ) {
        return false;
    }

    if( src->height() < area.height + area.y ) {
        return false;
    }

    if( src->backendId() == src->backendId() ) {
        return dst->copy(
                   src,
                   area,
                   destX,
                   destY
               );
    }

    const auto currentFormat = dst->format();
    const auto pixelSize = libgraphics::fxapi::EPixelFormat::getPixelSize( currentFormat );
    const auto temporaryBuffer = device->allocator()->alloc(
                                     area.width * area.height * pixelSize
                                 );
    assert( temporaryBuffer->data );

    const auto successfullyRetrievedSourceData = src->retrieve(
                temporaryBuffer->data,
                area
            );
    assert( successfullyRetrievedSourceData );

    if( !successfullyRetrievedSourceData ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "copyData(): Failed to receive data from source backend object.";
#endif
        return false;
    }

    const auto successfullyUploadedSourceData = dst->upload(
                temporaryBuffer->data,
                libgraphics::Rect32I(
                    ( int )destX,
                    ( int )destY,
                    area.width,
                    area.height
                ), 0, 0
            );
    assert( successfullyUploadedSourceData );

    if( !successfullyUploadedSourceData ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "copyData(): Failed to upload data to destination backend object.";
#endif
        return false;
    }

    return true;
}

ImageLayerMask::ImageLayerMask( libgraphics::fxapi::ApiBackendDevice* backendDevice ) : d( new Private() ) {
    assert( backendDevice );

    if( backendDevice ) {
        d->objects.emplace_back( backendDevice );
    }
}

ImageLayerMask::ImageLayerMask( const ImageLayerMask& rhs ) : d( new Private() ) {
    d->assign( rhs );
}

ImageLayerMask::ImageLayerMask( ImageLayerMask&& rhs ) : d( new Private() ) {
    std::swap(
        d,
        rhs.d
    );
}

ImageLayerMask::ImageLayerMask( libgraphics::fxapi::ApiBackendDevice* backendDevice, fxapi::EPixelFormat::t format, size_t width, size_t height ) : d( new Private() ) {
    assert( backendDevice );

    if( backendDevice ) {
        d->objects.emplace_back( backendDevice );
    }

    const auto successfullyInitialized = reset(
            format,
            width,
            height
                                         );
    assert( successfullyInitialized );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !successfullyInitialized ) {
        qDebug() << "ImageLayerMask::ImageLayerMask(): Failed to create ImageLayerMask instance from specified parameters.";
    }

#endif
}

ImageLayerMask::ImageLayerMask( libgraphics::fxapi::ApiBackendDevice* backendDevice, fxapi::EPixelFormat::t format, size_t width, size_t height, void* data ) : d( new Private() ) {
    assert( data );
    assert( backendDevice );

    if( backendDevice ) {
        d->objects.emplace_back( backendDevice );
    }

    const auto successfullyInitialized = reset(
            format,
            width,
            height,
            data
                                         );
    assert( successfullyInitialized );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !successfullyInitialized ) {
        qDebug() << "ImageLayerMask::ImageLayerMask(): Failed to create ImageLayerMask instance from specified parameters.";
    }

#endif
}

ImageLayerMask::ImageLayerMask( libgraphics::fxapi::ApiBackendDevice* backendDevice, const libgraphics::BitmapInfo& info ) : d( new Private() ) {
    assert( backendDevice );

    if( backendDevice ) {
        d->objects.emplace_back( backendDevice );
    }

    const auto successfullyInitialized = reset(
            info
                                         );
    assert( successfullyInitialized );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !successfullyInitialized ) {
        qDebug() << "ImageLayerMask::ImageLayerMask(): Failed to create ImageLayerMask instance from specified parameters.";
    }

#endif
}

ImageLayerMask::ImageLayerMask( libgraphics::fxapi::ApiBackendDevice* backendDevice, libgraphics::Bitmap* bitmap, size_t channelIndex ) : d( new Private() )  {
    assert( backendDevice );

    if( backendDevice ) {
        d->objects.emplace_back( backendDevice );
    }

    const auto successfullyInitialized = reset(
            bitmap,
            channelIndex
                                         );
    assert( successfullyInitialized );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !successfullyInitialized ) {
        qDebug() << "ImageLayerMask::ImageLayerMask(): Failed to create ImageLayerMask instance from specified parameters.";
    }

#endif
}

ImageLayerMask::ImageLayerMask( libgraphics::fxapi::ApiBackendDevice* backendDevice, libgraphics::Bitmap* bitmap, size_t channelIndex, libgraphics::Rect32I area ) : d( new Private() )  {
    assert( backendDevice );

    if( backendDevice ) {
        d->objects.emplace_back( backendDevice );
    }

    const auto successfullyInitialized = reset(
            bitmap,
            channelIndex,
            area
                                         );
    assert( successfullyInitialized );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !successfullyInitialized ) {
        qDebug() << "ImageLayerMask::ImageLayerMask(): Failed to create ImageLayerMask instance from specified parameters.";
    }

#endif
}

ImageLayerMask& ImageLayerMask::operator = ( const ImageLayerMask& rhs ) {
    this->d->assign( rhs );
    assert( !this->empty() );

    return *this;
}

bool ImageLayerMask::operator == ( const ImageLayerMask& rhs ) const {
    return ( rhs.d.get() == d.get() );
}

bool ImageLayerMask::operator != ( const ImageLayerMask& rhs ) const {
    return ( rhs.d.get() != d.get() );
}

/// properties
int ImageLayerMask::width() const {
    return d->width;
}

int ImageLayerMask::height() const {
    return d->height;
}

bool ImageLayerMask::empty() const {
    return ( d->width == 0 ) && ( d->height == 0 );
}

fxapi::EPixelFormat::t ImageLayerMask::format() const {
    return d->format;
}

size_t ImageLayerMask::byteSize() const {
    return d->width * d->height * pixelSize();
}

size_t ImageLayerMask::pixelSize() const {
    return libgraphics::fxapi::EPixelFormat::getPixelSize(
               this->d->format
           );
}

size_t ImageLayerMask::channelSize() const {
    return pixelSize();
}

/// resetting the internal state
void ImageLayerMask::reset() {
    d->objects.clear();

    d->width = 0;
    d->height = 0;
    d->format = libgraphics::fxapi::EPixelFormat::Empty;
}

bool ImageLayerMask::reset( fxapi::EPixelFormat::t format, int width, int height ) {

    /// check for compatible format
    const auto isCompatibleFormat = d->isCompatibleFormat( format );
    assert( isCompatibleFormat );

    if( !isCompatibleFormat ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "ImageLayerMask::reset(): Failed to create new ImageLayerMask. Invalid format.";
#endif

        return false;
    }

    /// check dimensions
    assert( width > 0 );
    assert( height > 0 );

    if( width == 0 || height == 0 ) {
        return false;
    }

    if( ( this->width() == width ) && ( this->height() == height ) && ( this->format() == format ) ) {
        return true; /** image layer mask already has the correct dimensions **/
    }

    /// iterate over all internal image objects and
    /// create new backend objects.
    for( auto it = d->objects.begin(); it != d->objects.end(); ++it ) {
        const auto successfullyInitialized = ( *it ).imageObject->create(
                format,
                width,
                height
                                             );
        assert( successfullyInitialized );

        if( !successfullyInitialized ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "ImageLayerMask::reset(): Failed to create backend-object. ImageLayerMask corrupted. Resetting...";
#endif
            reset();

            return false;
        }
    }

    d->width = width;
    d->height = height;
    d->format = format;

    return true;

}

bool ImageLayerMask::reset( fxapi::EPixelFormat::t format, int width, int height, void* data ) {
    assert( data );

    /// check for compatible format
    const auto isCompatibleFormat = d->isCompatibleFormat( format );
    assert( isCompatibleFormat );

    if( !isCompatibleFormat ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "ImageLayerMask::reset(): Failed to create new ImageLayerMask. Invalid format.";
#endif

        return false;
    }

    /// check dimensions
    assert( width > 0 );
    assert( height > 0 );

    if( width == 0 || height == 0 ) {
        return false;
    }

    if( ( this->width() == width ) && ( this->height() == height ) && ( this->format() == format ) ) {
        /** image layer mask already has the correct dimensions, so just update the data. **/
        for( auto it = d->objects.begin(); it != d->objects.end(); ++it ) {
            const auto successfullyUploaded = ( *it ).imageObject->upload(
                                                  data
                                              );
            assert( successfullyUploaded );

            if( !successfullyUploaded ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
                qDebug() << "ImageLayerMask::reset(): Failed to upload new data. ImageLayerMask corrupted. Resetting...";
#endif
                reset();

                return false;
            }
        }

        return true;
    }

    /// iterate over all internal image objects and
    /// create new backend objects.
    for( auto it = d->objects.begin(); it != d->objects.end(); ++it ) {
        const auto successfullyInitialized = ( *it ).imageObject->createFromData(
                format,
                width,
                height,
                data
                                             );
        assert( successfullyInitialized );

        if( !successfullyInitialized ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "ImageLayerMask::reset(): Failed to create backend-object. ImageLayerMask corrupted. Resetting...";
#endif
            reset();

            return false;
        }
    }

    d->width = width;
    d->height = height;
    d->format = format;

    return true;
}

bool ImageLayerMask::reset( const libgraphics::BitmapInfo& info ) {
    const auto format = libgraphics::backend::toCompatibleFormat( info.format() );
    const auto width = info.width();
    const auto height = info.height();

    /// check for compatible format
    const auto isCompatibleFormat = d->isCompatibleFormat( format );
    assert( isCompatibleFormat );

    if( !isCompatibleFormat ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "ImageLayerMask::reset(): Failed to create new ImageLayerMask. Invalid format.";
#endif

        return false;
    }

    /// check dimensions
    assert( width > 0 );
    assert( height > 0 );

    if( width == 0 || height == 0 ) {
        return false;
    }

    if( ( this->width() == width ) && ( this->height() == height ) && ( this->format() == format ) ) {
        return true; /** image layer mask already has the correct dimensions **/
    }

    /// iterate over all internal image objects and
    /// create new backend objects.
    for( auto it = d->objects.begin(); it != d->objects.end(); ++it ) {
        const auto successfullyInitialized = ( *it ).imageObject->createFromBitmapInfo(
                info
                                             );
        assert( successfullyInitialized );

        if( !successfullyInitialized ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "ImageLayerMask::reset(): Failed to create backend-object. ImageLayerMask corrupted. Resetting...";
#endif
            reset();

            return false;
        }
    }

    d->width = width;
    d->height = height;
    d->format = format;

    return true;
}

bool ImageLayerMask::reset( libgraphics::Bitmap* bitmap, size_t channelIndex ) {
    return reset(
               bitmap,
               channelIndex,
               libgraphics::Rect32I( bitmap->width(), bitmap->height() )
           );
}

bool ImageLayerMask::reset( libgraphics::Bitmap* bitmap, size_t channelIndex, libgraphics::Rect32I area ) {
    assert( bitmap );

    const auto bitmapFormat = bitmap->format();
    const auto width = bitmap->width();
    const auto height = bitmap->height();
    const auto compatibleFormat = libgraphics::backend::toCompatibleFormat( bitmapFormat );

    assert( bitmapFormat.channels > 0 );
    assert( bitmapFormat.channels >= channelIndex );

    if( bitmapFormat.channels < channelIndex ) {
        return false;
    }

    assert( compatibleFormat != libgraphics::fxapi::EPixelFormat::Empty );

    if( compatibleFormat == libgraphics::fxapi::EPixelFormat::Empty ) {
        return false;
    }

    libgraphics::fxapi::EPixelFormat::t format( libgraphics::fxapi::EPixelFormat::Empty );

    switch( bitmapFormat.byteSize / bitmapFormat.channels ) {
        case 1: /// 8bit mono format
            format = libgraphics::fxapi::EPixelFormat::Mono8;
            break;

        case 2: /// 16bit mono format
            format = libgraphics::fxapi::EPixelFormat::Mono16;
            break;

        default: break;
    }

    assert( format != libgraphics::fxapi::EPixelFormat::Empty );

    if( format == libgraphics::fxapi::EPixelFormat::Empty ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "ImageLayerMask::reset(): Failed to reset. Bitmap format is not compatible. ImageLayerMask state preserved.";
#endif
        return false;
    }

    const auto temporaryChannelBuffer = this->d->objects.front().device->allocator()->alloc(
                                            libgraphics::fxapi::EPixelFormat::getPixelSize( format ) * width * height
                                        );
    assert( temporaryChannelBuffer->data );

    copyChannelData(
        temporaryChannelBuffer->data,
        bitmap->buffer(),
        area,
        bitmap->width(),
        bitmap->height(),
        compatibleFormat,
        channelIndex
    );

    const auto successfullyCreatedBackendObjects = this->reset(
                format,
                width,
                height,
                temporaryChannelBuffer->data
            );
    assert( successfullyCreatedBackendObjects );

    if( !successfullyCreatedBackendObjects ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "ImageLayerMask::reset(): Failed to create backend objects.";
#endif
        return false;
    }

    d->format = format;
    d->width = width;
    d->height = height;

    return true;
}

bool ImageLayerMask::reset( const ImageLayerMask& other ) {
    this->reset();

    d->assign( other );
    assert( !this->empty() );

    return true;
}

/// retrieving and updating data
bool ImageLayerMask::retrieve(
    void* buffer
) {
    return retrieve(
               buffer,
               libgraphics::Rect32I( ( int )width(), ( int )height() )
           );
}

bool ImageLayerMask::retrieve(
    void* buffer,
    libgraphics::Rect32I rect
) {
    assert( buffer );
    assert( width() >= rect.x + rect.width );
    assert( height() >= rect.y + rect.height );

    if( width() < rect.width + rect.x ) {
        return false;
    }

    if( height() < rect.height + rect.y ) {
        return false;
    }

    if( this->d->objectForBackend( FXAPI_BACKEND_CPU ) != nullptr ) {
        Private::BackendImageObj* obj = d->objectForBackend( FXAPI_BACKEND_CPU );

        const auto successfullyRetrievedData = obj->imageObject->retrieve(
                buffer,
                rect
                                               );
        assert( successfullyRetrievedData );

        if( !successfullyRetrievedData ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "ImageLayerMask::retrieve(): Failed to retrieve data from cpu-backend object.";
#endif
            return false;
        }

        return true;
    }

    Private::BackendImageObj* obj = &d->objects.front();
    assert( obj );

    const auto successfullyRetrievedData = obj->imageObject->retrieve(
            buffer,
            rect
                                           );
    assert( successfullyRetrievedData );

    if( !successfullyRetrievedData ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "ImageLayerMask::retrieve(): Failed to retrieve data from backend object.";
#endif
        return false;
    }

    return true;
}

bool ImageLayerMask::retrieve(
    libgraphics::Bitmap* bitmap,
    size_t channelIndex
) {
    return retrieve(
               bitmap,
               channelIndex,
               libgraphics::Rect32I(
                   bitmap->width(), bitmap->height()
               )
           );
}

bool ImageLayerMask::retrieve(
    libgraphics::Bitmap* bitmap,
    size_t channelIndex,
    libgraphics::Rect32I rect
) {
    assert( !empty() );

    if( this->width() < rect.width + rect.x ) {
        assert( false );
        return false;
    }

    if( this->height() < rect.height + rect.y ) {
        assert( false );
        return false;
    }

    if( bitmap->width() < rect.width ) {
        assert( false );
        return false;
    }

    if( bitmap->height() < rect.height ) {
        assert( false );
        return false;
    }

    /// check for compatible formats.
    const auto bitmapFormat = bitmap->format();
    const auto compatibleFormat = libgraphics::backend::toCompatibleFormat( bitmapFormat );

    assert( bitmapFormat.channels > 0 );
    assert( bitmapFormat.channels >= channelIndex );

    if( bitmapFormat.channels < channelIndex ) {
        return false;
    }

    assert( compatibleFormat != libgraphics::fxapi::EPixelFormat::Empty );

    if( compatibleFormat == libgraphics::fxapi::EPixelFormat::Empty ) {
        return false;
    }

    /// allocate and copy temporary data
    const auto temporaryChannelData = this->d->objects.front().device->allocator()->alloc(
                                          rect.width * rect.height * pixelSize()
                                      );
    assert( temporaryChannelData->data );
    const auto successfullyRetrievedData = this->retrieve(
            temporaryChannelData->data,
            rect
                                           );
    assert( successfullyRetrievedData );

    if( !successfullyRetrievedData ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "ImageLayerMask::retrieve(): Failed to retrieve channel data.";
#endif
        return false;
    }

    copyChannelDataToBitmap(
        bitmap->buffer(),
        temporaryChannelData->data,
        rect,
        bitmap->width(),
        bitmap->height(),
        compatibleFormat,
        channelIndex
    );

    return true;

}

bool ImageLayerMask::copy(
    fxapi::ApiImageObject* source,
    libgraphics::Rect32I sourceRect,
    int destX,
    int destY
) {
    assert( source );
    assert( width() >= sourceRect.width + destX );
    assert( height() >= sourceRect.height + destY );
    assert( source->width() >= sourceRect.width + sourceRect.x );
    assert( source->height() >= sourceRect.height + sourceRect.y );

    if( !source ) {
        return false;
    }

    if( width() < sourceRect.width + destX ) {
        return false;
    }

    if( height() < sourceRect.height + destY ) {
        return false;
    }

    if( source->width() < sourceRect.width + sourceRect.x ) {
        return false;
    }

    if( source->height() < sourceRect.height + sourceRect.y ) {
        return false;
    }

    for( auto it = d->objects.begin(); it != d->objects.end(); ++it ) {
        const auto successfullyCopiedData =
            copyData( ( *it ).device, ( *it ).imageObject, source, sourceRect, destX, destY );
        assert( successfullyCopiedData );

        if( !successfullyCopiedData ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "ImageLayerMask::copy(): Failed to copy data to backend object. ImageLayer corrupted. Resetting internal state.";
#endif

            this->reset();

            return false;
        }
    }

    return true;
}

bool ImageLayerMask::copy(
    ImageLayerMask* source,
    libgraphics::Rect32I sourceRect,
    int destX,
    int destY
) {
    assert( source );
    assert( width() >= sourceRect.width + destX );
    assert( height() >= sourceRect.height + destY );
    assert( source->width() >= sourceRect.width + sourceRect.x );
    assert( source->height() >= sourceRect.height + sourceRect.y );

    if( !source ) {
        return false;
    }

    if( width() < sourceRect.width + destX ) {
        return false;
    }

    if( height() < sourceRect.height + destY ) {
        return false;
    }

    if( source->width() < sourceRect.width + sourceRect.x ) {
        return false;
    }

    if( source->height() < sourceRect.height + sourceRect.y ) {
        return false;
    }

    std::map<Private::BackendImageObj*, bool>  objectSet;

    for( auto it = d->objects.begin(); it != d->objects.end();  ++it ) {
        objectSet[ &( *it ) ] = false;
    }

    assert( !objectSet.empty() );

    /// first run: copy matching backend objects
    for( auto it = objectSet.begin(); it != objectSet.end(); ++it ) {
        const auto backendObj = ( *it ).first;
        assert( backendObj );

        if( source->containsDataForBackend( backendObj->backendId ) ) {
            const auto successfullyCopiedData = this->copy(
                                                    source->d->objectForBackend( backendObj->backendId )->imageObject,
                                                    sourceRect,
                                                    destX,
                                                    destY
                                                );

            if( !successfullyCopiedData ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
                qDebug() << "ImageLayer::copy(): Failed to copy data from source to destination backend object.";
#endif
                return false;
            }

            ( *it ).second = true;
        }
    }

    /// second run: copy non-matching backend objects
    for( auto it = objectSet.begin(); it != objectSet.end(); ++it ) {
        if( !( *it ).second ) {
            const auto backendObj = ( *it ).second;
            LOGB_ASSERT( backendObj, "invalid backend" );

            const auto successfullyCopiedData = this->copy(
                                                    source->d->objects.front().imageObject,
                                                    sourceRect,
                                                    destX,
                                                    destY
                                                );

            if( !successfullyCopiedData ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
                qDebug() << "ImageLayer::copy(): Failed to copy data from source to destination backend object.";
#endif
                return false;
            }

            ( *it ).second = true;
        }
    }

    return true;

}

bool ImageLayerMask::copy(
    libgraphics::Bitmap* source,
    size_t channelIndex,
    libgraphics::Rect32I sourceRect,
    int destX,
    int destY
) {
    assert( source );
    assert( width() >= sourceRect.width + destX );
    assert( height() >= sourceRect.height + destY );
    assert( source->width() >= sourceRect.width + sourceRect.x );
    assert( source->height() >= sourceRect.height + sourceRect.y );

    if( !source ) {
        return false;
    }

    if( width() < sourceRect.width + destX ) {
        return false;
    }

    if( height() < sourceRect.height + destY ) {
        return false;
    }

    if( source->width() < sourceRect.width + sourceRect.x ) {
        return false;
    }

    if( source->height() < sourceRect.height + sourceRect.y ) {
        return false;
    }

    const auto compatibleFormat = libgraphics::backend::toCompatibleFormat(
                                      source->format()
                                  );
    assert( compatibleFormat != libgraphics::fxapi::EPixelFormat::Empty );

    if( compatibleFormat == libgraphics::fxapi::EPixelFormat::Empty ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "ImageLayerMask::copy(): Failed to copy data. Incompatible formats.";
#endif
        return false;
    }

    const auto temporaryChannelData = this->d->objects.front().device->allocator()->alloc(
                                          sourceRect.width * sourceRect.height * ( libgraphics::fxapi::EPixelFormat::getPixelSize( compatibleFormat ) / libgraphics::fxapi::EPixelFormat::getChannelCount( compatibleFormat ) )
                                      );
    assert( temporaryChannelData->data );

    copyChannelData(
        temporaryChannelData->data,
        source->buffer(),
        sourceRect,
        source->width(),
        source->height(),
        compatibleFormat,
        channelIndex
    );

    for( auto it = d->objects.begin(); it != d->objects.end(); ++it ) {
        const auto successfullyUploadedData = ( *it ).imageObject->upload(
                temporaryChannelData->data,
                sourceRect,
                destX,
                destY
                                              );
        assert( successfullyUploadedData );

        if( !successfullyUploadedData ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "ImageLayer::copy(): Failed to upload image data to backend object. ImageLayer corrupted. Resetting...";
#endif
            reset();

            return false;
        }
    }

    return true;
}

bool ImageLayerMask::copy(
    void* data,
    libgraphics::Rect32I sourceRect,
    int destX,
    int destY
) {
    assert( data );
    assert( width() >= sourceRect.width + destX );
    assert( height() >= sourceRect.height + destY );

    if( !data ) {
        return false;
    }

    if( width() < sourceRect.width + destX ) {
        return false;
    }

    if( height() < sourceRect.height + destY ) {
        return false;
    }

    for( auto it = d->objects.begin(); it != d->objects.end(); ++it ) {
        const auto successfullyUploadedData = ( *it ).imageObject->upload(
                data,
                sourceRect,
                destX,
                destY
                                              );
        assert( successfullyUploadedData );

        if( !successfullyUploadedData ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "ImageLayer::copy(): Failed to upload image data to backend object. ImageLayer corrupted. Resetting...";
#endif
            this->reset();

            return false;
        }
    }

    return true;
}

bool ImageLayerMask::copy(
    libgraphics::Bitmap* bitmap,
    size_t channelIndex
) {
    return copy(
               bitmap,
               channelIndex,
               libgraphics::Rect32I( 0, 0, bitmap->width(), bitmap->height() ),
               0,
               0
           );
}

bool ImageLayerMask::copy(
    ImageLayerMask* layer
) {
    return copy(
               layer,
               libgraphics::Rect32I( 0, 0, ( int )width(), ( int )height() ),
               0,
               0
           );
}

/// automatic backend mirroring
bool ImageLayerMask::containsDataForBackend(
    int backendId
) const {
    return ( this->d->objectForBackend( backendId ) != nullptr );
}

bool ImageLayerMask::containsDataForDevice(
    libgraphics::fxapi::ApiBackendDevice* device
) const {
    return ( this->d->objectForDevice( device ) != nullptr );
}

void ImageLayerMask::updateDataForBackend(
    libgraphics::fxapi::ApiBackendDevice* device,
    int backendId
) {
    assert( !empty() );

    if( this->d->objects.empty() ) {
        d->objects.emplace_back( backendId, device );
    } else {

        if( containsDataForBackend( backendId ) ) {
            return;
        }

        const auto currentFormat = format();
        const auto channelCount = libgraphics::fxapi::EPixelFormat::getChannelCount(
                                      currentFormat
                                  );
        const auto pixelSize = libgraphics::fxapi::EPixelFormat::getPixelSize(
                                   currentFormat
                               );
        const auto channelSize = pixelSize / channelCount;

        LOGB_ASSERT( channelCount > 0, "invalid channel count" );
        LOGB_ASSERT( pixelSize > 0, "invalid pixel size" );
        LOGB_ASSERT( channelSize > 0, "invalid channel size" );

        const auto temporarySourceBuffer = device->allocator()->alloc(
                                               width() * height() * pixelSize
                                           );
        assert( temporarySourceBuffer->data );

        const auto successfullyReceivedSourceData = this->retrieve(
                    temporarySourceBuffer->data
                );
        assert( successfullyReceivedSourceData );

        if( !successfullyReceivedSourceData ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "ImageLayerMask::updateDataForBackend(): Failed to retrieve source data.";
#endif
            return;
        }

        this->d->objects.emplace_back(
            device,
            temporarySourceBuffer->data,
            width(),
            height(),
            format()
        );
    }
}

/// internals
fxapi::ApiImageObject* ImageLayerMask::internalImageForBackend(
    int backendId
) const {
    const auto obj = this->d->objectForBackend( backendId );
    assert( obj );

    if( obj != nullptr ) {
        return obj->imageObject;
    }

    return nullptr;
}

fxapi::ApiImageObject* ImageLayerMask::internalImageForDevice(
    fxapi::ApiBackendDevice* device
) const {
    const auto obj = this->d->objectForDevice( device );
    assert( obj );

    if( obj != nullptr ) {
        return obj->imageObject;
    }

    return nullptr;
}

fxapi::ApiBackendDevice* ImageLayerMask::internalDeviceForBackend(
    int backendId
) const {
    for( auto it = d->objects.begin(); it != d->objects.end(); ++it ) {
        if( ( *it ).backendId == backendId ) {
            return ( *it ).device;
        }
    }

    return nullptr;
}

}
