#include <QDebug>
#include <QElapsedTimer>
#include <libgraphics/image.hpp>
#include <libgraphics/backend/common/formats.hpp>
#include <libgraphics/fx/operations/basic.hpp>
#include <libgraphics/debug.hpp>

#include <log/log.hpp>

namespace libgraphics {
//// extern
///
extern void copyChannelData( void* dst, void* src, libgraphics::Rect32I area, int width, int height, libgraphics::fxapi::EPixelFormat::t format, size_t channelIndex );
extern void copyChannelData( void* dst, void* src, libgraphics::Rect32I area, int width, int height, libgraphics::fxapi::EPixelFormat::t format, size_t sourceChannelIndex, size_t destChannelIndex );
extern bool copyData( fxapi::ApiBackendDevice* device, fxapi::ApiImageObject* dst, fxapi::ApiImageObject* src, libgraphics::Rect32I area, int destX, int destY );

//// impl
struct ImageLayer::Private : libcommon::PimplPrivate {
    struct BackendImageObj {
        const int backendId;
        libgraphics::fxapi::ApiBackendDevice* device;
        fxapi::ApiImageObject*  imageObject;


        inline bool operator == ( const BackendImageObj& obj ) const {
            return ( obj.imageObject == imageObject );
        }
        inline bool operator != ( const BackendImageObj& obj ) const {
            return ( obj.imageObject != imageObject );
        }

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

        bool removeAlphaChannel( size_t width, size_t height, fxapi::EPixelFormat::t format, fxapi::EPixelFormat::t newFormat ) {
            assert( width > 0 );
            assert( height > 0 );
            assert( format != fxapi::EPixelFormat::Empty );

            if( width == 0 ) {
                return false;
            }

            if( height == 0 ) {
                return false;
            }

            if( format == fxapi::EPixelFormat::Empty ) {
                return false;
            }

            const auto formatChannelCount = libgraphics::fxapi::EPixelFormat::getChannelCount(
                                                newFormat
                                            );
            const auto formatPixelSize = libgraphics::fxapi::EPixelFormat::getPixelSize(
                                             newFormat
                                         );
            const auto formatChannelSize = formatPixelSize / formatChannelCount;

            assert( formatChannelCount > 0 );
            assert( formatPixelSize > 0 );
            assert( formatChannelSize > 0 );

            const auto temporarySourceBuffer = device->allocator()->alloc(
                                                   formatPixelSize * width * height
                                               );
            assert( temporarySourceBuffer->data );

            const auto successfullyReceivedData = this->imageObject->retrieve(
                    temporarySourceBuffer->data
                                                  );
            assert( successfullyReceivedData );

            if( !successfullyReceivedData ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
                qDebug() << "BackendImageObj::addAlphaChannel(): Failed to receive source data.";
#endif
                return false;
            }

            const auto temporaryNonAlphaBuffer = device->allocator()->alloc(
                    ( ( formatChannelCount + 1 ) * formatChannelSize ) * width * height
                                                 );
            assert( temporaryNonAlphaBuffer->data );

            for( size_t x = 0; width > x; ++x ) {
                for( size_t y = 0; height > y; ++y ) {
                    ( void ) memcpy(
                        ( void* )( ( char* )temporaryNonAlphaBuffer->data + ( ( y * width ) + x ) * formatPixelSize ),
                        ( const void* )( ( char* )temporarySourceBuffer->data + ( ( ( y * width ) + x ) * ( ( formatChannelCount + 1 ) * formatChannelSize ) ) ),
                        formatPixelSize
                    );
                }
            }

            const auto successfullyResetted = this->reset();
            assert( successfullyResetted );

            if( !successfullyResetted ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
                qDebug() << "BackendImageObj::removeAlphaChannel(): Failed to add alpha channel.";
#endif
                return false;
            }

            const auto successfullyInitializedFromData = this->imageObject->createFromData(
                        newFormat,
                        width,
                        height,
                        temporaryNonAlphaBuffer->data
                    );
            assert( successfullyInitializedFromData );

            if( !successfullyInitializedFromData ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
                qDebug() << "BackendImageObj::removeAlphaChannel(): Failed to create new backend image object.";
#endif

                this->reset();

                return false;
            }

            return true;
        }

        bool addAlphaChannel( size_t width, size_t height, fxapi::EPixelFormat::t format, fxapi::EPixelFormat::t newFormat ) {
            assert( width > 0 );
            assert( height > 0 );
            assert( format != fxapi::EPixelFormat::Empty );

            if( width == 0 ) {
                return false;
            }

            if( height == 0 ) {
                return false;
            }

            if( format == fxapi::EPixelFormat::Empty ) {
                return false;
            }

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

            const auto temporarySourceBuffer = device->allocator()->alloc(
                                                   formatPixelSize * width * height
                                               );
            assert( temporarySourceBuffer->data );

            const auto successfullyReceivedData = this->imageObject->retrieve(
                    temporarySourceBuffer->data
                                                  );
            assert( successfullyReceivedData );

            if( !successfullyReceivedData ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
                qDebug() << "BackendImageObj::addAlphaChannel(): Failed to receive source data.";
#endif
                return false;
            }

            const auto temporaryAlphaBuffer = device->allocator()->alloc(
                                                  ( ( formatChannelCount + 1 ) * formatChannelSize ) * width * height
                                              );
            assert( temporaryAlphaBuffer->data );

            for( size_t x = 0; width > x; ++x ) {
                for( size_t y = 0; height > y; ++y ) {
                    ( void ) memcpy(
                        ( void* )( ( char* )temporaryAlphaBuffer->data + ( ( y * width ) + x ) * ( ( formatChannelCount + 1 ) * formatChannelSize ) ),
                        ( const void* )( ( char* )temporarySourceBuffer->data + ( ( ( y * width ) + x ) * formatPixelSize ) ),
                        formatPixelSize
                    );
                    ( void ) memset(
                        ( void* )( ( char* )temporaryAlphaBuffer->data + ( ( y * width ) + x + 3 ) * ( ( formatChannelCount + 1 ) * formatChannelSize ) ),
                        0,
                        formatChannelSize
                    );
                }
            }

            const auto successfullyResetted = this->reset();
            assert( successfullyResetted );

            if( !successfullyResetted ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
                qDebug() << "BackendImageObj::addAlphaChannel(): Failed to add alpha channel.";
#endif
                return false;
            }

            const auto successfullyInitializedFromData = this->imageObject->createFromData(
                        newFormat,
                        width,
                        height,
                        temporaryAlphaBuffer->data
                    );
            assert( successfullyInitializedFromData );

            if( !successfullyInitializedFromData ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
                qDebug() << "BackendImageObj::addAlphaChannel(): Failed to create new backend image object.";
#endif

                this->reset();

                return false;
            }

            return true;
        }

    };
    struct MaskInfo {
        ImageLayerMask* layerMask;
        ImageLayerMaskMode* maskMode;

        MaskInfo() : layerMask( nullptr ),
            maskMode( nullptr ) {}

        void clear() {
            layerMask = nullptr;
            maskMode = nullptr;
        }
    };
    MaskInfo maskInfo;

    /// format info
    size_t width;
    size_t height;
    fxapi::EPixelFormat::t format;

    /// meta info
    std::string name;

    /// backend image objects
    std::vector< libcommon::ScopedPtr<BackendImageObj> > objects;



    Private() : width( 0 ), height( 0 ),
        format( fxapi::EPixelFormat::Empty ) {}

    void assign( const ImageLayer& rhs ) {
        assert( !rhs.empty() );

        objects.clear();

        format       = rhs.format();
        width        = rhs.width();
        height       = rhs.height();
        name         = rhs.name();

        for( auto it = rhs.d->objects.begin(); it != rhs.d->objects.end(); ++it ) {
            objects.emplace_back(
                libcommon::ScopedPtr<BackendImageObj>( new BackendImageObj( ( *it )->device ) )
            );

            const auto sucessfullyCreated = objects.back()->imageObject->create(
                                                format,
                                                width,
                                                height
                                            );
            assert( sucessfullyCreated );

            if( !sucessfullyCreated ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
                qDebug() << "Failed to create image layer backend object - " << "width:" << width << "height:" << height;
#endif
                continue;
            }

            const auto sucessfullyCopied = objects.back()->imageObject->copy(
                                               ( *it )->imageObject,
                                               libgraphics::Rect32I( ( int )width, ( int )height ),
                                               0,
                                               0
                                           );
            assert( sucessfullyCopied );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

            if( !sucessfullyCopied ) {
                qDebug() << "Failed to copy data from another image layer backend object - " << "width:" << width << "height:" << height;
            }

#endif
        }
    }

    BackendImageObj* getImageForBackend( int backend ) {
        for( auto it = objects.begin(); it != objects.end(); ++it ) {
            if( ( *it )->backendId == backend ) {
                return ( *it ).get();
            }
        }

        return nullptr;
    }
};


ImageLayer::ImageLayer( libgraphics::fxapi::ApiBackendDevice* backendDevice, std::string _name ) : d( new Private() ) {
    d->name = _name;

    d->objects.emplace_back(
        libcommon::ScopedPtr<Private::BackendImageObj>( new Private::BackendImageObj( backendDevice ) )
    );

    LIBGRAPHICS_MEMORY_LOG_ALLOCATE( this );
}

ImageLayer::ImageLayer() : d( new Private() ) {
    LIBGRAPHICS_MEMORY_LOG_ALLOCATE( this );
}

ImageLayer::~ImageLayer() {
    LIBGRAPHICS_MEMORY_LOG_DEALLOCATE( this );
    d->objects.clear();
}

ImageLayer::ImageLayer( const ImageLayer& rhs ) : d( new Private() ) {
    d->assign( rhs );

    LIBGRAPHICS_MEMORY_LOG_ALLOCATE( this );
}

ImageLayer::ImageLayer( ImageLayer&& rhs ) : d( std::move( rhs.d ) ) {}

ImageLayer& ImageLayer::operator = ( const ImageLayer& rhs ) {
    d->assign( rhs );

    return *this;
}

bool ImageLayer::operator == ( const ImageLayer& rhs ) const {
    return ( this->width() == rhs.width() ) && ( this->height() == rhs.height() )
           && ( this->format() == rhs.format() );
}

bool ImageLayer::operator != ( const ImageLayer& rhs ) const {
    return !( this->width() != rhs.width() ) || !( this->height() == rhs.height() )
           || ( this->format() == rhs.format() );
}

/** internal properties **/
const std::string&  ImageLayer::name() const {
    return d->name;
}

void ImageLayer::setName( const std::string& _name ) {
    d->name = _name;
}

/** general imaging methods **/

/// reset
bool ImageLayer::reset() {
    LIBGRAPHICS_MEMORY_LOG_SCOPED_RESET( this );

    d->objects.clear();

    d->width = 0;
    d->height = 0;
    d->format = fxapi::EPixelFormat::Empty;

    return true;
}

bool ImageLayer::reset(
    libgraphics::Bitmap* bitmap
) {
    LIBGRAPHICS_MEMORY_LOG_SCOPED_RESET( this );

    assert( bitmap );

    if( !bitmap ) {
        return false;
    }

    const auto compatibleBackendFormat = libgraphics::backend::toCompatibleFormat( bitmap->format() );
    assert( compatibleBackendFormat != libgraphics::fxapi::EPixelFormat::Empty );

    if( ( width() == bitmap->width() ) && ( height() == bitmap->height() ) && ( format() == compatibleBackendFormat ) ) {
        for( auto it = d->objects.begin(); it != d->objects.end(); ++it ) {
            const auto sucessfullyUploaded = ( *it )->imageObject->upload(
                                                 bitmap
                                             );
            assert( sucessfullyUploaded );

            if( !sucessfullyUploaded ) {
                qDebug() << "ImageLayer::reset(): Failed to update backend image object.";
                reset();

                return false;
            }
        }

        return true;
    }

    d->width = bitmap->width();
    d->height = bitmap->height();
    d->format = compatibleBackendFormat;
    d->maskInfo.clear();

    for( auto it = d->objects.begin(); it != d->objects.end(); ++it ) {
        ( *it )->reset();

        const auto successfullyCreated = ( *it )->imageObject->createFromBitmap(
                                             bitmap
                                         );
        assert( successfullyCreated );

        if( !successfullyCreated ) {
            qDebug() << "ImageLayer::reset(): Failed to create backend image object.";
            reset();

            return false;
        }
    }

    return true;
}

bool ImageLayer::reset(
    libgraphics::Bitmap* bitmap,
    libgraphics::Rect32I rect
) {
    LIBGRAPHICS_MEMORY_LOG_SCOPED_RESET( this );

    assert( bitmap );
    assert( rect.size() > 0 );

    if( !bitmap ) {
        return false;
    }

    if( rect.size() <= 0 ) {
        return false;
    }

    const auto compatibleBackendFormat = libgraphics::backend::toCompatibleFormat( bitmap->format() );
    assert( compatibleBackendFormat != libgraphics::fxapi::EPixelFormat::Empty );

    if( ( width() == rect.width ) && ( height() == rect.height ) && ( format() == compatibleBackendFormat ) ) {
        for( auto it = d->objects.begin(); it != d->objects.end(); ++it ) {
            const auto sucessfullyUploaded = ( *it )->imageObject->upload(
                                                 bitmap,
                                                 rect,
                                                 0,
                                                 0
                                             );
            assert( sucessfullyUploaded );

            if( !sucessfullyUploaded ) {
                qDebug() << "ImageLayer::reset(): Failed to update backend image object.";
                reset();

                return false;
            }
        }

        return true;
    }

    d->width = bitmap->width();
    d->height = bitmap->height();
    d->format = compatibleBackendFormat;
    d->maskInfo.clear();

    for( auto it = d->objects.begin(); it != d->objects.end(); ++it ) {
        ( *it )->reset();

        const auto successfullyCreated = ( *it )->imageObject->createFromBitmap(
                                             bitmap,
                                             rect
                                         );
        assert( successfullyCreated );

        if( !successfullyCreated ) {
            qDebug() << "ImageLayer::reset(): Failed to create backend image object.";
            reset();

            return false;
        }
    }

    return true;
}

bool ImageLayer::reset(
    const libgraphics::BitmapInfo& info
) {
    LIBGRAPHICS_MEMORY_LOG_SCOPED_RESET( this );

    assert( info.width() > 0 );
    assert( info.height() > 0 );

    if( info.width() == 0 || info.height() == 0 ) {
        return false;
    }

    const auto compatibleBackendFormat = libgraphics::backend::toCompatibleFormat( info.format() );
    assert( compatibleBackendFormat != libgraphics::fxapi::EPixelFormat::Empty );

    if( ( width() == info.width() ) && ( height() == info.height() ) && ( format() == compatibleBackendFormat ) ) {
        return true;
    }

    d->width = info.width();
    d->height = info.height();
    d->format = compatibleBackendFormat;
    d->maskInfo.clear();

    for( auto it = d->objects.begin(); it != d->objects.end(); ++it ) {
        ( *it )->reset();

        const auto successfullyCreated = ( *it )->imageObject->createFromBitmapInfo(
                                             info
                                         );
        assert( successfullyCreated );

        if( !successfullyCreated ) {
            qDebug() << "ImageLayer::reset(): Failed to create backend image object from BitmapInfo structure.";
            reset();

            return false;
        }
    }

    return true;
}

bool ImageLayer::reset(
    fxapi::EPixelFormat::t format,
    int width,
    int height,
    void* data
) {
    LIBGRAPHICS_MEMORY_LOG_SCOPED_RESET( this );

    assert( format != libgraphics::fxapi::EPixelFormat::Empty );
    assert( width > 0 );
    assert( height > 0 );
    assert( data );

    if( !data || !width || !height ) {
        return false;
    }

    if( format == libgraphics::fxapi::EPixelFormat::Empty ) {
        qDebug() << "ImageLayer::reset(): Failed to reset ImageLayer instance - invalid fxapi-format specified. (format: " << format << ")";
        return false;
    }

    if( ( this->format() == format ) && ( this->width() == width ) && ( this->height() == height ) ) {
        for( auto it = d->objects.begin(); it != d->objects.end(); ++it ) {
            const auto sucessfullyUploaded = ( *it )->imageObject->upload(
                                                 data,
                                                 libgraphics::Rect32I( ( int )width, ( int )height ),
                                                 0,
                                                 0
                                             );
            assert( sucessfullyUploaded );

            if( !sucessfullyUploaded ) {
                qDebug() << "ImageLayer::reset(): Failed to update backend image object.";
                reset();

                return false;
            }
        }

        this->d->width = width;
        this->d->height = height;

        return true;
    }

    for( auto it = d->objects.begin(); it != d->objects.end(); ++it ) {
        ( *it )->reset();

        const auto successfullyCreated = ( *it )->imageObject->createFromData(
                                             format,
                                             width,
                                             height,
                                             data
                                         );
        assert( successfullyCreated );

        if( !successfullyCreated ) {
            qDebug() << "ImageLayer::reset(): Failed to create backend image object from raw data.";
            reset();

            return false;
        }
    }

    this->d->format = format;
    this->d->width = width;
    this->d->height = height;

    return true;
}

bool ImageLayer::reset(
    fxapi::EPixelFormat::t format,
    int width,
    int height
) {
    LIBGRAPHICS_MEMORY_LOG_SCOPED_RESET( this );

    assert( format != libgraphics::fxapi::EPixelFormat::Empty );
    assert( width > 0 );
    assert( height > 0 );

    if( !width || !height ) {
        return false;
    }

    if( format == libgraphics::fxapi::EPixelFormat::Empty ) {
        qDebug() << "ImageLayer::reset(): Failed to reset ImageLayer instance - invalid fxapi-format specified. (format: " << format << ")";
        return false;
    }

    if( ( this->format() == format ) && ( this->width() == width ) && ( this->height() == height ) ) {
        return true;
    }

    for( auto it = d->objects.begin(); it != d->objects.end(); ++it ) {
        ( *it )->reset();

        QElapsedTimer t;
        t.start();
        const auto successfullyCreated = ( *it )->imageObject->create(
                                             format,
                                             width,
                                             height
                                         );
        assert( successfullyCreated );

        if( !successfullyCreated ) {
            qDebug() << "ImageLayer::reset(): Failed to create backend image object from raw data.";
            reset();

            return false;
        }
    }

    this->d->format = format;
    this->d->width = width;
    this->d->height = height;

    return true;
}

template < class _t_any >
bool fillArea(
    ImageLayer* dst,
    libgraphics::Rect32I area,
    typename _t_any::t value
) {
    assert( dst->width() >= area.width + area.x );
    assert( dst->height() >= area.height + area.y );

    if( dst->width() < ( area.width + area.x ) ) {
        return false;
    }

    if( dst->height() < ( area.height + area.y ) ) {
        return false;
    }

    const auto currentBitmapFormat = _t_any::toFormat();
    const auto compatibleFormat = libgraphics::backend::toCompatibleFormat(
                                      currentBitmapFormat
                                  );
    assert( compatibleFormat != libgraphics::fxapi::EPixelFormat::Empty );

    if( compatibleFormat == libgraphics::fxapi::EPixelFormat::Empty ) {
        return false;
    }

    libgraphics::fx::operations::fill(
        dst,
        area,
        currentBitmapFormat,
        ( void* )&value
    );

    return true;
}

/// fill
bool ImageLayer::fill(
    libgraphics::Rect32I area,
    libgraphics::formats::RGB8::t value
) {
    return fillArea<libgraphics::formats::RGB8>(
               this,
               area,
               value
           );
}

bool ImageLayer::fill(
    libgraphics::Rect32I area,
    libgraphics::formats::RGBA8::t value
) {
    return fillArea<libgraphics::formats::RGBA8>(
               this,
               area,
               value
           );
}

bool ImageLayer::fill(
    libgraphics::Rect32I area,
    libgraphics::formats::RGB16::t value
) {
    return fillArea<libgraphics::formats::RGB16>(
               this,
               area,
               value
           );
}

bool ImageLayer::fill(
    libgraphics::Rect32I area,
    libgraphics::formats::RGBA16::t value
) {
    return fillArea<libgraphics::formats::RGBA16>(
               this,
               area,
               value
           );
}

bool ImageLayer::fill(
    libgraphics::Rect32I area,
    libgraphics::formats::RGB32::t value
) {
    return fillArea<libgraphics::formats::RGB32>(
               this,
               area,
               value
           );
}

bool ImageLayer::fill(
    libgraphics::Rect32I area,
    libgraphics::formats::RGBA32::t value
) {
    return fillArea<libgraphics::formats::RGBA32>(
               this,
               area,
               value
           );
}

bool ImageLayer::fill(
    libgraphics::Rect32I area,
    libgraphics::formats::RGB32F::t value
) {
    return fillArea<libgraphics::formats::RGB32F>(
               this,
               area,
               value
           );
}

bool ImageLayer::fill(
    libgraphics::Rect32I area,
    libgraphics::formats::RGBA32F::t value
) {
    return fillArea<libgraphics::formats::RGBA32F>(
               this,
               area,
               value
           );
}

bool ImageLayer::fill(
    libgraphics::Rect32I area,
    libgraphics::formats::Mono8::t value
) {
    return fillArea<libgraphics::formats::Mono8>(
               this,
               area,
               value
           );
}

bool ImageLayer::fill(
    libgraphics::Rect32I area,
    libgraphics::formats::Mono16::t value
) {
    return fillArea<libgraphics::formats::Mono16>(
               this,
               area,
               value
           );
}

bool ImageLayer::fill(
    libgraphics::Rect32I area,
    libgraphics::formats::Mono32::t value
) {
    return fillArea<libgraphics::formats::Mono32>(
               this,
               area,
               value
           );
}


template < class _t_value >
void fillChannelAreaWrapper(
    ImageLayer* dst,
    libgraphics::Rect32I area,
    size_t channelIndex,
    _t_value val
) {
    ( void )dst;
    ( void )area;
    ( void )channelIndex;
    ( void )val;

    assert( false );

    return;
}
template <>
void fillChannelAreaWrapper(
    ImageLayer* dst,
    libgraphics::Rect32I area,
    size_t channelIndex,
    unsigned char val
) {
    libgraphics::fx::operations::fillChannel(
        dst,
        area,
        channelIndex,
        val
    );
}
template <>
void fillChannelAreaWrapper(
    ImageLayer* dst,
    libgraphics::Rect32I area,
    size_t channelIndex,
    unsigned short val
) {
    libgraphics::fx::operations::fillChannel(
        dst,
        area,
        channelIndex,
        val
    );
}
template <>
void fillChannelAreaWrapper(
    ImageLayer* dst,
    libgraphics::Rect32I area,
    size_t channelIndex,
    unsigned int val
) {
    libgraphics::fx::operations::fillChannel(
        dst,
        area,
        channelIndex,
        val
    );
}

template < class _t_value >
bool fillChannelArea(
    ImageLayer* dst,
    libgraphics::Rect32I area,
    size_t channelIndex,
    _t_value val
) {
    assert( dst );
    assert( dst->width() >= area.width + area.x );
    assert( dst->height() >= area.height + area.y );

    const auto formatChannelCount = libgraphics::fxapi::EPixelFormat::getChannelCount(
                                        dst->format()
                                    );
    const auto formatPixelSize = libgraphics::fxapi::EPixelFormat::getPixelSize(
                                     dst->format()
                                 );
    const auto formatChannelSize = formatPixelSize / formatChannelCount;

    assert( formatChannelCount > 0 );
    assert( formatChannelSize > 0 );
    assert( formatPixelSize > 0 );
    assert( sizeof( _t_value ) == formatChannelSize );

    if( sizeof( unsigned char ) != formatChannelSize ) {
        return false;
    }

    if( channelIndex > formatChannelCount ) {
        return false;
    }

    if( dst->width() < area.width + area.x ) {
        return false;
    }

    if( dst->height() < area.height + area.y ) {
        return false;
    }

    fillChannelAreaWrapper<_t_value>(
        dst,
        area,
        channelIndex,
        val
    );

    return true;
}
bool ImageLayer::fillChannel(
    libgraphics::Rect32I area,
    size_t channelIndex,
    unsigned char value
) {
    return fillChannelArea<unsigned char>(
               this,
               area,
               channelIndex,
               value
           );
}

bool ImageLayer::fillChannel(
    libgraphics::Rect32I area,
    size_t channelIndex,
    unsigned short value
) {
    return fillChannelArea<unsigned short>(
               this,
               area,
               channelIndex,
               value
           );
}

bool ImageLayer::fillChannel(
    libgraphics::Rect32I area,
    size_t channelIndex,
    unsigned int value
) {
    return fillChannelArea<unsigned int>(
               this,
               area,
               channelIndex,
               value
           );
}

bool ImageLayer::fillChannel(
    libgraphics::Rect32I area,
    size_t channelIndex,
    float value
) {
    return fillChannelArea<float>(
               this,
               area,
               channelIndex,
               value
           );
}

/// downloading, retrieving data
bool ImageLayer::retrieve(
    void* buffer
) {
    assert( buffer );

    if( !buffer ) {
        return false;
    }

    /** the fastest option to retrieve in the internal data is to copy the data from an
     *  cpu backend object to the specified buffer. */
    /// step 1: look for a cpu-backend object

    bool successfullyRetrieved( false );

    if( this->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        for( auto it = d->objects.begin(); it != d->objects.end(); ++it ) {
            if( ( *it )->backendId == FXAPI_BACKEND_CPU ) {
                successfullyRetrieved = ( *it )->imageObject->retrieve(
                                            buffer
                                        );
                break;
            }
        }

        assert( successfullyRetrieved );
#ifdef LIBGRAPHICS_DEBUG_OUTPUT

        if( !successfullyRetrieved ) {
            qDebug() << "ImageLayer::retrieve(): Failed to retrieve data from cpu-backend object.";
        }

#endif
    }

    if( !successfullyRetrieved ) {
        successfullyRetrieved = d->objects.front()->imageObject->retrieve(
                                    buffer
                                );

        assert( successfullyRetrieved );

        if( !successfullyRetrieved ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "ImageLayer::retrieve(): Failed to receive data from backend object.";
#endif
            return false;
        }
    }

    return true;
}

bool ImageLayer::retrieve(
    void* buffer,
    libgraphics::Rect32I rect
) {
    assert( buffer );
    assert( width() >= rect.width + rect.x );
    assert( height() >= rect.height + rect.y );

    if( !buffer ) {
        return false;
    }

    if( width() < rect.width + rect.x ) {
        return false;
    }

    if( height() < rect.height + rect.y ) {
        return false;
    }

    /** the fastest option to retrieve in the internal data is to copy the data from an
     *  cpu backend object to the specified buffer. */
    /// step 1: look for a cpu-backend object

    bool successfullyRetrieved( false );

    if( this->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        for( auto it = d->objects.begin(); it != d->objects.end(); ++it ) {
            if( ( *it )->backendId == FXAPI_BACKEND_CPU ) {
                successfullyRetrieved = ( *it )->imageObject->retrieve(
                                            buffer,
                                            rect
                                        );
                break;
            }
        }

        assert( successfullyRetrieved );
#ifdef LIBGRAPHICS_DEBUG_OUTPUT

        if( !successfullyRetrieved ) {
            qDebug() << "ImageLayer::retrieve(): Failed to retrieve data from cpu-backend object.";
        }

#endif
    }

    if( !successfullyRetrieved ) {
        successfullyRetrieved = d->objects.front()->imageObject->retrieve(
                                    buffer,
                                    rect
                                );

        assert( successfullyRetrieved );

        if( !successfullyRetrieved ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "ImageLayer::retrieve(): Failed to receive data from backend object.";
#endif
            return false;
        }
    }

    return true;

}

bool ImageLayer::retrieve(
    libgraphics::Bitmap* bitmap
) {
    assert( bitmap );
    assert( bitmap->width() >= width() );
    assert( bitmap->height() >= height() );

    if( !bitmap ) {
        return false;
    }

    if( bitmap->width() < width() ) {
        return false;
    }

    if( bitmap->height() < height() ) {
        return false;
    }

    /** the fastest option to retrieve in the internal data is to copy the data from an
     *  cpu backend object to the specified buffer. */
    /// step 1: look for a cpu-backend object

    bool successfullyRetrieved( false );

    if( this->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        for( auto it = d->objects.begin(); it != d->objects.end(); ++it ) {
            if( ( *it )->backendId == FXAPI_BACKEND_CPU ) {
                successfullyRetrieved = ( *it )->imageObject->retrieve(
                                            bitmap
                                        );
                break;
            }
        }

        assert( successfullyRetrieved );
#ifdef LIBGRAPHICS_DEBUG_OUTPUT

        if( !successfullyRetrieved ) {
            qDebug() << "ImageLayer::retrieve(): Failed to retrieve data from cpu-backend object.";
        }

#endif
    }

    if( !successfullyRetrieved ) {
        successfullyRetrieved = d->objects.front()->imageObject->retrieve(
                                    bitmap
                                );

        assert( successfullyRetrieved );

        if( !successfullyRetrieved ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "ImageLayer::retrieve(): Failed to receive data from backend object.";
#endif
            return false;
        }
    }

    return true;
}

bool ImageLayer::retrieve(
    libgraphics::Bitmap* bitmap,
    libgraphics::Rect32I rect
) {
    assert( bitmap );
    assert( bitmap->width() >= rect.width );
    assert( bitmap->height() >= rect.height );
    assert( width() >= rect.width + rect.x );
    assert( height() >= rect.height + rect.y );
    assert( format() == libgraphics::backend::toCompatibleFormat( bitmap->format() ) );

    if( !bitmap ) {
        return false;
    }

    if( bitmap->width() < rect.width ) {
        return false;
    }

    if( bitmap->height() < rect.height ) {
        return false;
    }


    /** the fastest option to retrieve in the internal data is to copy the data from an
     *  cpu backend object to the specified buffer. */
    /// step 1: look for a cpu-backend object

    bool successfullyRetrieved( false );

    if( this->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        for( auto it = d->objects.begin(); it != d->objects.end(); ++it ) {
            if( ( *it )->backendId == FXAPI_BACKEND_CPU ) {
                successfullyRetrieved = ( *it )->imageObject->retrieve(
                                            bitmap,
                                            rect
                                        );
                break;
            }
        }

        assert( successfullyRetrieved );
#ifdef LIBGRAPHICS_DEBUG_OUTPUT

        if( !successfullyRetrieved ) {
            qDebug() << "ImageLayer::retrieve(): Failed to retrieve data from cpu-backend object.";
        }

#endif
    }

    if( !successfullyRetrieved ) {
        successfullyRetrieved = d->objects.front()->imageObject->retrieve(
                                    bitmap,
                                    rect
                                );

        assert( successfullyRetrieved );

        if( !successfullyRetrieved ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "ImageLayer::retrieve(): Failed to receive data from backend object.";
#endif
            return false;
        }
    }

    return true;
}

bool ImageLayer::retrieveChannel(
    size_t channelIndex,
    void* buffer
) {
    return retrieveChannel(
               channelIndex,
               buffer,
               libgraphics::Rect32I( 0, 0, ( int )width(), ( int )height() )
           );
}
bool ImageLayer::retrieveChannel(
    size_t channelIndex,
    void* buffer,
    libgraphics::Rect32I rect
) {
    assert( buffer );
    assert( width() >= rect.width + rect.x );
    assert( height() >= rect.height + rect.y );

    const auto currentFormat = format();
    const auto formatChannelCount = libgraphics::fxapi::EPixelFormat::getChannelCount(
                                        currentFormat
                                    );
    const auto formatPixelSize = libgraphics::fxapi::EPixelFormat::getPixelSize(
                                     currentFormat
                                 );
    const auto formatChannelSize = formatPixelSize / formatChannelCount;

    assert( formatChannelCount > 0 );
    assert( formatPixelSize > 0 );
    assert( formatChannelSize > 0 );

    if( width() < rect.width + rect.x ) {
        return false;
    }

    if( height() < rect.height + rect.y ) {
        return false;
    }

    if( formatChannelCount == 0 ) {
        return false;
    }

    if( formatPixelSize == 0 ) {
        return false;
    }

    if( formatChannelSize == 0 ) {
        return false;
    }

    /** Try to receive the channel data from the internal
     *  cpu object first. */
    bool successfullyReceived( false );

    if( this->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        for( auto it = d->objects.begin(); it != d->objects.end(); ++it ) {
            if( ( *it )->backendId == FXAPI_BACKEND_CPU ) {
                const auto temporaryBufferObject = ( *it )->device->allocator()->alloc(
                                                       rect.width * rect.height * formatPixelSize
                                                   );
                assert( temporaryBufferObject->data );

                const auto successfullyReceivedFromBackend = ( *it )->imageObject->retrieve(
                            temporaryBufferObject->data,
                            rect
                        );
                assert( successfullyReceivedFromBackend );

                if( !successfullyReceivedFromBackend ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
                    qDebug() << "ImageLayer::retrieveChannel(): Failed to retrieve channel data from cpu-backend object.";
#endif
                    break;
                }

                /** copy data **/
                copyChannelData(
                    buffer,
                    temporaryBufferObject->data,
                    rect,
                    rect.width,
                    rect.height,
                    currentFormat,
                    channelIndex
                );

                successfullyReceived = true;
            }
        }
    }

    if( !successfullyReceived ) {
        const auto temporaryBufferObject = d->objects.front()->device->allocator()->alloc(
                                               rect.width * rect.height * formatPixelSize
                                           );
        assert( temporaryBufferObject->data );

        const auto successfullyReceivedFromBackend = d->objects.front()->imageObject->retrieve(
                    temporaryBufferObject->data,
                    rect
                );
        assert( successfullyReceivedFromBackend );


        if( !successfullyReceivedFromBackend ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "ImageLayer::retrieveChannel(): Failed to retrieve channel data from backend.";
#endif
            return false;
        }

        /** copy data **/
        copyChannelData(
            buffer,
            temporaryBufferObject->data,
            rect,
            rect.width,
            rect.height,
            currentFormat,
            channelIndex
        );

        successfullyReceived = true;
    }

    return successfullyReceived;
}

bool ImageLayer::retrieveChannel(
    size_t channelIndex,
    libgraphics::Bitmap* bitmap
) {
    return retrieveChannel(
               channelIndex,
               bitmap,
               libgraphics::Rect32I( 0, 0, ( int )width(), ( int )height() )
           );
}

bool ImageLayer::retrieveChannel(
    size_t channelIndex,
    libgraphics::Bitmap* bitmap,
    libgraphics::Rect32I rect
) {
    assert( bitmap );
    assert( width() >= rect.width + rect.x );
    assert( height() >= rect.height + rect.y );
    assert( bitmap->width() >= rect.width + rect.x );
    assert( bitmap->height() >= rect.height + rect.y );
    assert( format() == libgraphics::backend::toCompatibleFormat( bitmap->format() ) );

    const auto currentFormat = format();
    const auto formatChannelCount = libgraphics::fxapi::EPixelFormat::getChannelCount(
                                        currentFormat
                                    );
    const auto formatPixelSize = libgraphics::fxapi::EPixelFormat::getPixelSize(
                                     currentFormat
                                 );
    const auto formatChannelSize = formatPixelSize / formatChannelCount;

    assert( formatChannelCount > 0 );
    assert( formatPixelSize > 0 );
    assert( formatChannelSize > 0 );

    if( width() < rect.width + rect.x ) {
        return false;
    }

    if( height() < rect.height + rect.y ) {
        return false;
    }

    if( formatChannelCount == 0 ) {
        return false;
    }

    if( formatPixelSize == 0 ) {
        return false;
    }

    if( formatChannelSize == 0 ) {
        return false;
    }

    /** Try to receive the channel data from the internal
     *  cpu object first. */
    bool successfullyReceived( false );

    if( this->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        for( auto it = d->objects.begin(); it != d->objects.end(); ++it ) {
            if( ( *it )->backendId == FXAPI_BACKEND_CPU ) {
                const auto temporaryBufferObject = ( *it )->device->allocator()->alloc(
                                                       rect.width * rect.height * formatPixelSize
                                                   );
                assert( temporaryBufferObject->data );

                const auto successfullyReceivedFromBackend = ( *it )->imageObject->retrieve(
                            temporaryBufferObject->data,
                            rect
                        );
                assert( successfullyReceivedFromBackend );

                if( !successfullyReceivedFromBackend ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
                    qDebug() << "ImageLayer::retrieveChannel(): Failed to retrieve channel data from cpu-backend object.";
#endif
                    break;
                }

                /** copy data **/
                copyChannelData(
                    bitmap->buffer(),
                    temporaryBufferObject->data,
                    rect,
                    bitmap->width(),
                    bitmap->height(),
                    currentFormat,
                    channelIndex
                );

                successfullyReceived = true;
            }
        }
    }

    if( !successfullyReceived ) {
        const auto temporaryBufferObject = d->objects.front()->device->allocator()->alloc(
                                               rect.width * rect.height * formatPixelSize
                                           );
        assert( temporaryBufferObject->data );

        const auto successfullyReceivedFromBackend = d->objects.front()->imageObject->retrieve(
                    temporaryBufferObject->data,
                    rect
                );
        assert( successfullyReceivedFromBackend );

        if( !successfullyReceivedFromBackend ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "ImageLayer::retrieveChannel(): Failed to retrieve channel data from backend.";
#endif
            return false;
        }

        /** copy data **/
        copyChannelData(
            bitmap->buffer(),
            temporaryBufferObject->data,
            rect,
            bitmap->width(),
            bitmap->height(),
            currentFormat,
            channelIndex
        );

        successfullyReceived = true;
    }

    return successfullyReceived;
}

bool ImageLayer::copy(
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
            copyData( ( *it )->device, ( *it )->imageObject, source, sourceRect, destX, destY );
        assert( successfullyCopiedData );

        if( !successfullyCopiedData ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "ImageLayer::copy(): Failed to copy data to backend object. ImageLayer corrupted. Resetting internal state.";
#endif

            assert( this->reset() );

            return false;
        }
    }

    return true;
}

bool ImageLayer::copy(
    ImageLayer* source,
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
        objectSet[( *it ).get() ] = false;
    }

    assert( !objectSet.empty() );

    /// first run: copy matching backend objects
    for( auto it = objectSet.begin(); it != objectSet.end(); ++it ) {
        const auto backendObj = ( *it ).first;
        assert( backendObj );

        if( source->containsDataForBackend( backendObj->backendId ) ) {
            const auto successfullyCopiedData = ( *it ).first->imageObject->copy(
                                                    source->d->getImageForBackend( backendObj->backendId )->imageObject,
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
            const auto backendObj = ( *it ).first;
            LOGB_ASSERT( backendObj, "Invalid key" );

            const auto successfullyCopiedData = ( *it ).first->imageObject->copy(
                                                    source->d->objects.front()->imageObject,
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

bool ImageLayer::copy(
    libgraphics::Bitmap* bitmap,
    libgraphics::Rect32I sourceRect,
    int destX,
    int destY
) {
    assert( bitmap );
    assert( width() >= sourceRect.width + destX );
    assert( height() >= sourceRect.height + destY );
    assert( bitmap->width() >= sourceRect.width + sourceRect.x );
    assert( bitmap->height() >= sourceRect.height + sourceRect.y );
    assert( format() == libgraphics::backend::toCompatibleFormat( bitmap->format() ) );

    if( !bitmap ) {
        return false;
    }

    if( width() < sourceRect.width + destX ) {
        return false;
    }

    if( height() < sourceRect.height + destY ) {
        return false;
    }

    if( bitmap->width() < sourceRect.width + sourceRect.x ) {
        return false;
    }

    if( bitmap->height() < sourceRect.height + sourceRect.y ) {
        return false;
    }

    for( auto it = d->objects.begin(); it != d->objects.end(); ++it ) {
        const auto successfullyUploadedData = ( *it )->imageObject->upload(
                bitmap,
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

bool ImageLayer::copy(
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
        const auto successfullyUploadedData = ( *it )->imageObject->upload(
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

bool ImageLayer::copy(
    libgraphics::Bitmap* bitmap
) {
    return copy(
               bitmap,
               libgraphics::Rect32I( 0, 0, bitmap->width(), bitmap->height() ),
               0,
               0
           );
}

bool ImageLayer::copy(
    ImageLayer* layer
) {
    return copy(
               layer,
               libgraphics::Rect32I( 0, 0, ( int )width(), ( int )height() ),
               0,
               0
           );
}

bool ImageLayer::copyChannel(
    size_t channelIndex,
    fxapi::ApiImageObject* source,
    libgraphics::Rect32I sourceRect,
    size_t destX,
    size_t destY
) {
    return copyChannel(
               channelIndex,
               channelIndex,
               source,
               sourceRect,
               destX,
               destY
           );
}

bool ImageLayer::copyChannel(
    size_t channelIndex,
    ImageLayer* source,
    libgraphics::Rect32I sourceRect,
    size_t destX,
    size_t destY
) {
    return copyChannel(
               channelIndex,
               channelIndex,
               source,
               sourceRect,
               destX,
               destY
           );
}

bool ImageLayer::copyChannel(
    size_t channelIndex,
    libgraphics::Bitmap* bitmap,
    libgraphics::Rect32I sourceRect,
    size_t destX,
    size_t destY
) {
    return copyChannel(
               channelIndex,
               channelIndex,
               bitmap,
               sourceRect,
               destX,
               destY
           );
}

bool ImageLayer::copyChannel(
    size_t channelIndex,
    void* data,
    libgraphics::Rect32I sourceRect,
    size_t destX,
    size_t destY
) {
    return copyChannel(
               channelIndex,
               channelIndex,
               data,
               sourceRect,
               destX,
               destY
           );
}

bool ImageLayer::copyChannel(
    size_t channelIndex,
    libgraphics::Bitmap* bitmap
) {
    return copyChannel(
               channelIndex,
               channelIndex,
               bitmap
           );
}

bool ImageLayer::copyChannel(
    size_t channelIndex,
    ImageLayer* layer
) {
    return copyChannel(
               channelIndex,
               channelIndex,
               layer
           );
}

bool ImageLayer::copyChannel(
    size_t sourceChannelIndex,
    size_t destChannelIndex,
    fxapi::ApiImageObject* source,
    libgraphics::Rect32I sourceRect,
    int destX,
    int destY
) {
    assert( source );
    assert( source->width() >= sourceRect.width + sourceRect.x );
    assert( source->height() >= sourceRect.height + sourceRect.y );
    assert( width() >= sourceRect.width + destX );
    assert( height() >= sourceRect.height + destY );
    assert( format() == source->format() );

    if( !source ) {
        return false;
    }

    if( format() != source->format() ) {
        return false;
    }

    if( source->width() < sourceRect.width + sourceRect.x ) {
        return false;
    }

    if( source->height() < sourceRect.height + sourceRect.y ) {
        return false;
    }

    if( width() < sourceRect.width + destX ) {
        return false;
    }

    if( height() < sourceRect.height + destY ) {
        return false;
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

    const auto destinationBufferObject = this->d->objects.front()->device->allocator()->alloc(
            sourceRect.width * sourceRect.height * pixelSize
                                         );
    assert( destinationBufferObject->data );

    const auto sourceBufferObject = this->d->objects.front()->device->allocator()->alloc(
                                        sourceRect.width * sourceRect.height * pixelSize
                                    );
    assert( sourceBufferObject->data );

    /// step 0: retrieve destination data
    bool successfullyObtainedDestinationData( false );

    if( this->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        const auto cpuBackendObject = this->d->getImageForBackend( FXAPI_BACKEND_CPU );
        assert( cpuBackendObject );

        const auto successfullyReceivedCpuData = cpuBackendObject->imageObject->retrieve(
                    destinationBufferObject->data,
                    libgraphics::Rect32I(
                        ( int )destX,
                        ( int )destY,
                        sourceRect.width,
                        sourceRect.height
                    )
                );
        assert( successfullyReceivedCpuData );

        if( !successfullyReceivedCpuData ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "ImageLayer::copyChannel(): Failed to receive cpu-backend object data. ImageLayer corrupted. Resetting...";
#endif
            reset();

            return false;
        }

        successfullyObtainedDestinationData = true;
    }

    if( !successfullyObtainedDestinationData ) {
        const auto successfullyReceivedData = this->d->objects.front()->imageObject->retrieve(
                destinationBufferObject->data,
                libgraphics::Rect32I(
                    ( int )destX,
                    ( int )destY,
                    sourceRect.width,
                    sourceRect.height
                )
                                              );
        assert( successfullyReceivedData );

        if( !successfullyReceivedData ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "ImageLayer::copyChannel(): Failed to receive backend data. ImageLayer corrupted. Resetting...";
#endif
            reset();

            return false;
        }

        successfullyObtainedDestinationData = true;
    }

    assert( successfullyObtainedDestinationData );

    /// step 1: retrieve source data
    const auto successfullyReceivedSourceData = source->retrieve(
                sourceBufferObject->data,
                sourceRect
            );
    assert( successfullyReceivedSourceData );

    if( !successfullyReceivedSourceData ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "ImageLayer::copyChannel(): Failed to receive backend data from source object. ImageLayer corrupted. Resetting...";
#endif
        reset();

        return false;
    }

    /// step 2: copy channel data from source to destination
    ///  buffer
    copyChannelData(
        destinationBufferObject->data,
        sourceBufferObject->data,
        sourceRect,
        sourceRect.width,
        sourceRect.height,
        currentFormat,
        sourceChannelIndex,
        destChannelIndex
    );

    /// step 3: upload updated data to all internal objects.
    const auto successfullyCopiedUpdatedData = this->copy(
                destinationBufferObject->data,
                libgraphics::Rect32I(
                    0, 0, sourceRect.width, sourceRect.height
                ),
                destX,
                destY
            );
    assert( successfullyCopiedUpdatedData );

    if( !successfullyCopiedUpdatedData ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "ImageLayer::copyChannel(): Failed to upload updated data to backend objects. ImageLayer corrupted. Resetting...";
#endif
        reset();

        return false;
    }

    return true;
}

bool ImageLayer::copyChannel(
    size_t sourceChannelIndex,
    size_t destChannelIndex,
    ImageLayer* source,
    libgraphics::Rect32I sourceRect,
    int destX,
    int destY
) {
    assert( source );
    assert( source->format() == format() );

    if( !source ) {
        return false;
    }

    if( source->format() != format() ) {
        return false;
    }

    return this->copyChannel(
               sourceChannelIndex,
               destChannelIndex,
               source->d->objects.front()->imageObject,
               sourceRect,
               destX,
               destY
           );
}

bool ImageLayer::copyChannel(
    size_t sourceChannelIndex,
    size_t destChannelIndex,
    libgraphics::Bitmap* bitmap,
    libgraphics::Rect32I sourceRect,
    int destX,
    int destY
) {
    assert( bitmap );
    assert( bitmap->width() >= sourceRect.width + sourceRect.x );
    assert( bitmap->height() >= sourceRect.height + sourceRect.y );
    assert( libgraphics::fxapi::EPixelFormat::getChannelSize( format() ) == libgraphics::fxapi::EPixelFormat::getChannelSize( libgraphics::backend::toCompatibleFormat( bitmap->format() ) ) );
    assert( width() >= sourceRect.width + destX );
    assert( height() >= sourceRect.height + destY );

    if( !bitmap ) {
        return false;
    }

    if( width() < sourceRect.width + destX ) {
        return false;
    }

    if( height() < sourceRect.height + destY ) {
        return false;
    }

    if( bitmap->width() < sourceRect.width + sourceRect.x ) {
        return false;
    }

    if( bitmap->height() < sourceRect.height + sourceRect.y ) {
        return false;
    }

    if( libgraphics::fxapi::EPixelFormat::getChannelSize( format() ) != libgraphics::fxapi::EPixelFormat::getChannelSize( libgraphics::backend::toCompatibleFormat( bitmap->format() ) ) ) {
        return false;
    }

    /// step 0: receive destination data
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

    const auto destinationBufferObject = this->d->objects.front()->device->allocator()->alloc(
            sourceRect.width * sourceRect.height * pixelSize
                                         );
    assert( destinationBufferObject->data );

    /// step 0: retrieve destination data
    bool successfullyObtainedDestinationData( false );

    if( this->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        const auto cpuBackendObject = this->d->getImageForBackend( FXAPI_BACKEND_CPU );
        assert( cpuBackendObject );

        const auto successfullyReceivedCpuData = cpuBackendObject->imageObject->retrieve(
                    destinationBufferObject->data,
                    libgraphics::Rect32I(
                        ( int )destX,
                        ( int )destY,
                        sourceRect.width,
                        sourceRect.height
                    )
                );
        assert( successfullyReceivedCpuData );

        if( !successfullyReceivedCpuData ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "ImageLayer::copyChannel(): Failed to receive cpu-backend object data. ImageLayer corrupted. Resetting...";
#endif
            reset();

            return false;
        }

        successfullyObtainedDestinationData = true;
    }

    if( !successfullyObtainedDestinationData ) {
        const auto successfullyReceivedData = this->d->objects.front()->imageObject->retrieve(
                destinationBufferObject->data,
                libgraphics::Rect32I(
                    ( int )destX,
                    ( int )destY,
                    sourceRect.width,
                    sourceRect.height
                )
                                              );
        assert( successfullyReceivedData );

        if( !successfullyReceivedData ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "ImageLayer::copyChannel(): Failed to receive backend data. ImageLayer corrupted. Resetting...";
#endif
            reset();

            return false;
        }

        successfullyObtainedDestinationData = true;
    }

    assert( successfullyObtainedDestinationData );

    /// step 2: now update channel data
    copyChannelData(
        destinationBufferObject->data,
        bitmap->buffer(),
        sourceRect,
        bitmap->width(),
        bitmap->height(),
        currentFormat,
        sourceChannelIndex,
        destChannelIndex
    );

    /// step 3: update internal region
    const auto successfullyCopiedUpdatedData = this->copy(
                destinationBufferObject->data,
                libgraphics::Rect32I(
                    0, 0, sourceRect.width, sourceRect.height
                ),
                destX,
                destY
            );
    assert( successfullyCopiedUpdatedData );

    if( !successfullyCopiedUpdatedData ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "ImageLayer::copyChannel(): Failed to upload updated data to backend objects. ImageLayer corrupted. Resetting...";
#endif
        reset();

        return false;
    }

    return true;
}

bool ImageLayer::copyChannel(
    size_t sourceChannelIndex,
    size_t destChannelIndex,
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

    /// step 0: receive destination data
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

    const auto destinationBufferObject = this->d->objects.front()->device->allocator()->alloc(
            sourceRect.width * sourceRect.height * pixelSize
                                         );
    assert( destinationBufferObject->data );

    /// step 0: retrieve destination data
    bool successfullyObtainedDestinationData( false );

    if( this->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        const auto cpuBackendObject = this->d->getImageForBackend( FXAPI_BACKEND_CPU );
        assert( cpuBackendObject );

        const auto successfullyReceivedCpuData = cpuBackendObject->imageObject->retrieve(
                    destinationBufferObject->data,
                    libgraphics::Rect32I(
                        ( int )destX,
                        ( int )destY,
                        sourceRect.width,
                        sourceRect.height
                    )
                );
        assert( successfullyReceivedCpuData );

        if( !successfullyReceivedCpuData ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "ImageLayer::copyChannel(): Failed to receive cpu-backend object data. ImageLayer corrupted. Resetting...";
#endif
            reset();

            return false;
        }

        successfullyObtainedDestinationData = true;
    }

    if( !successfullyObtainedDestinationData ) {
        const auto successfullyReceivedData = this->d->objects.front()->imageObject->retrieve(
                destinationBufferObject->data,
                libgraphics::Rect32I(
                    ( int )destX,
                    ( int )destY,
                    sourceRect.width,
                    sourceRect.height
                )
                                              );
        assert( successfullyReceivedData );

        if( !successfullyReceivedData ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "ImageLayer::copyChannel(): Failed to receive backend data. ImageLayer corrupted. Resetting...";
#endif
            reset();

            return false;
        }

        successfullyObtainedDestinationData = true;
    }

    assert( successfullyObtainedDestinationData );

    /// step 2: now update channel data
    copyChannelData(
        destinationBufferObject->data,
        data,
        sourceRect,
        sourceRect.width,
        sourceRect.height,
        currentFormat,
        sourceChannelIndex,
        destChannelIndex
    );

    /// step 3: update internal region
    const auto successfullyCopiedUpdatedData = this->copy(
                destinationBufferObject->data,
                libgraphics::Rect32I(
                    0, 0, sourceRect.width, sourceRect.height
                ),
                destX,
                destY
            );
    assert( successfullyCopiedUpdatedData );

    if( !successfullyCopiedUpdatedData ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "ImageLayer::copyChannel(): Failed to upload updated data to backend objects. ImageLayer corrupted. Resetting...";
#endif
        reset();

        return false;
    }

    return true;
}

bool ImageLayer::copyChannel(
    size_t sourceChannelIndex,
    size_t destChannelIndex,
    libgraphics::Bitmap* bitmap
) {
    assert( bitmap );

    return copyChannel(
               sourceChannelIndex,
               destChannelIndex,
               bitmap,
               libgraphics::Rect32I( 0, 0, bitmap->width(), bitmap->height() ),
               0,
               0
           );
}

bool ImageLayer::copyChannel(
    size_t sourceChannelIndex,
    size_t destChannelIndex,
    ImageLayer* layer
) {
    assert( layer );

    return copyChannel(
               sourceChannelIndex,
               destChannelIndex,
               ( libgraphics::ImageLayer* )layer,
               libgraphics::Rect32I( 0, 0, ( int )layer->width(), ( int )layer->height() ),
               0,
               0
           );
}

/// constructs sub textures of the current
/// one.
ImageLayer* ImageLayer::duplicate() {
    assert( !empty() );

    ImageLayer* layer = new ImageLayer(
        *this
    );
    assert( layer );

    return layer;
}

ImageLayer* ImageLayer::duplicateArea(
    libgraphics::Rect32I sourceRect
) {
    assert( !empty() );
    assert( width() >= sourceRect.width + sourceRect.x );
    assert( height() >= sourceRect.height + sourceRect.y );

    if( empty() ) {
        return nullptr;
    }

    if( width() < sourceRect.width + sourceRect.x ) {
        return nullptr;
    }

    if( height() < sourceRect.height + sourceRect.y ) {
        return nullptr;
    }

    /// step 0: copy internal source data
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

    const auto sourceBufferObject = d->objects.front()->device->allocator()->alloc(
                                        sourceRect.width * sourceRect.height * pixelSize
                                    );
    assert( sourceBufferObject->data );

    const auto successfullyReceived = this->retrieve(
                                          sourceBufferObject->data,
                                          sourceRect
                                      );
    assert( successfullyReceived );

    if( !successfullyReceived ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "ImageLayer::duplicateArea(): Failed to receive specified image region.";
#endif
        return nullptr;
    }

    /// step 1: create layer and layer backend objects
    ImageLayer* layer = new ImageLayer();
    assert( layer );

    layer->d->format = this->format();
    layer->d->height = sourceRect.height;
    layer->d->width = sourceRect.width;

    for( auto it = d->objects.begin(); it != d->objects.end(); ++it ) {
        layer->d->objects.emplace_back(
            new Private::BackendImageObj(
                ( *it )->device,
                sourceBufferObject->data,
                sourceRect.width,
                sourceRect.height,
                currentFormat
            )
        );
    }

    return layer;
}

ImageLayer* ImageLayer::duplicateChannel( size_t channelIndex ) {
    return duplicateChannelArea(
               channelIndex,
               libgraphics::Rect32I(
                   0,
                   0,
                   ( int )this->width(),
                   ( int )this->height()
               )
           );
}

ImageLayer* ImageLayer::duplicateChannelArea(
    size_t channelIndex,
    libgraphics::Rect32I sourceRect
) {
    assert( !empty() );
    assert( width() >= sourceRect.width + sourceRect.x );
    assert( height() >= sourceRect.height + sourceRect.y );

    if( empty() ) {
        return nullptr;
    }

    if( width() < sourceRect.width + sourceRect.x ) {
        return nullptr;
    }

    if( height() < sourceRect.height + sourceRect.y ) {
        return nullptr;
    }

    /// step 0: find compatible image format.
    const auto currentFormat = format();
    const auto channelCount = libgraphics::fxapi::EPixelFormat::getChannelCount(
                                  currentFormat
                              );
    const auto pixelSize = libgraphics::fxapi::EPixelFormat::getPixelSize(
                               currentFormat
                           );
    const auto channelSize = pixelSize / channelCount;

    assert( channelCount > 0 );
    assert( pixelSize > 0 );
    assert( channelSize > 0 );

    libgraphics::fxapi::EPixelFormat::t compatibleFormat( libgraphics::fxapi::EPixelFormat::Empty );

    switch( channelSize ) {
        case sizeof( unsigned char ):
            compatibleFormat = fxapi::EPixelFormat::Mono8;
            break;

        case sizeof( unsigned short ):
            compatibleFormat = fxapi::EPixelFormat::Mono16;
            break;

        default: break;
    }

    assert( compatibleFormat == libgraphics::fxapi::EPixelFormat::Empty );

    if( compatibleFormat == libgraphics::fxapi::EPixelFormat::Empty ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "ImageLayer::duplicateChannelArea(): Incompatible format.";
#endif
        return nullptr;
    }

    /// step 1: allocate temporary buffer and extract channel
    ///  data

    const auto sourceBufferObject = d->objects.front()->device->allocator()->alloc(
                                        sourceRect.width * sourceRect.height * pixelSize
                                    );
    assert( sourceBufferObject->data );

    const auto successfullyReceived = this->retrieveChannel(
                                          channelIndex,
                                          sourceBufferObject->data,
                                          sourceRect
                                      );
    assert( successfullyReceived );

    if( !successfullyReceived ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "ImageLayer::duplicateChannelArea(): Failed to receive specified image region.";
#endif
        return nullptr;
    }

    /// step 2: create image layer
    ImageLayer* layer = new ImageLayer();
    assert( layer );

    layer->d->format = compatibleFormat;
    layer->d->height = sourceRect.height;
    layer->d->width = sourceRect.width;

    for( auto it = d->objects.begin(); it != d->objects.end(); ++it ) {
        layer->d->objects.emplace_back(
            new Private::BackendImageObj(
                ( *it )->device,
                sourceBufferObject->data,
                sourceRect.width,
                sourceRect.height,
                currentFormat
            )
        );
    }

    return layer;

}

/// mask-management
bool ImageLayer::hasMask() const {
    return ( this->d->maskInfo.layerMask != nullptr );
}

bool ImageLayer::hasMaskMode() const {
    return ( this->d->maskInfo.maskMode != nullptr );
}

ImageLayerMask* ImageLayer::mask() const {
    return this->d->maskInfo.layerMask;
}

ImageLayerMaskMode* ImageLayer::maskMode() const {
    return this->d->maskInfo.maskMode;
}

void ImageLayer::setMask( ImageLayerMask* mask ) {
    this->d->maskInfo.layerMask = mask;
}

void ImageLayer::setMaskMode( ImageLayerMaskMode* mode ) {
    this->d->maskInfo.maskMode = mode;
}

bool ImageLayer::assignMask( ImageLayerMask* mask, ImageLayerMaskMode* mode ) {
    setMask( mask );
    setMaskMode( mode );

    return true;
}

bool ImageLayer::applyMask() {
    if( hasMask() && hasMaskMode() ) {
        return this->d->maskInfo.maskMode->apply(
                   this,
                   this->d->maskInfo.layerMask
               );
    }

    return false;
}

/// alpha channels
bool ImageLayer::hasAlphaChannel() const {
    switch( this->format() ) {
        case fxapi::EPixelFormat::RGBA8:
            return true;

        case fxapi::EPixelFormat::RGBA16:
            return true;

        default:
            return false;
    }
}

bool ImageLayer::addAlphaChannel() {
    if( !hasAlphaChannel() ) {

        fxapi::EPixelFormat::t alphaFormat( fxapi::EPixelFormat::Empty );

        switch( this->format() ) {
            case fxapi::EPixelFormat::RGB8:
                alphaFormat = fxapi::EPixelFormat::RGBA8;
                break;

            case fxapi::EPixelFormat::RGB16:
                alphaFormat = fxapi::EPixelFormat::RGBA16;
                break;

            default: break;
        }

        assert( alphaFormat != fxapi::EPixelFormat::Empty );

        if( alphaFormat == fxapi::EPixelFormat::Empty ) {
            return false;
        }

        for( auto it = this->d->objects.begin(); it != this->d->objects.end(); ++it ) {
            const bool successfullyAddedAlphaChannel = ( *it )->addAlphaChannel(
                        width(),
                        height(),
                        this->format(),
                        alphaFormat
                    );

            assert( successfullyAddedAlphaChannel );

            if( !successfullyAddedAlphaChannel ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
                qDebug() << "ImageLayer::addAlphaChannel(): Failed to add alpha channel to image layer backend object. ImageLayer corrupted. Resetting...";
#endif
                this->reset();

                return false;
            }
        }

        d->format = alphaFormat;

        return true;
    }

    return false;
}

bool ImageLayer::removeAlphaChannel() {
    if( hasAlphaChannel() ) {

        fxapi::EPixelFormat::t nonAlphaFormat( fxapi::EPixelFormat::Empty );

        switch( this->format() ) {
            case fxapi::EPixelFormat::RGBA8:
                nonAlphaFormat = fxapi::EPixelFormat::RGB8;
                break;

            case fxapi::EPixelFormat::RGBA16:
                nonAlphaFormat = fxapi::EPixelFormat::RGB16;
                break;

            default: break;
        }

        assert( nonAlphaFormat != fxapi::EPixelFormat::Empty );

        if( nonAlphaFormat == fxapi::EPixelFormat::Empty ) {
            return false;
        }

        for( auto it = this->d->objects.begin(); it != this->d->objects.end(); ++it ) {
            const bool successfullyRemovedAlphaChannel = ( *it )->removeAlphaChannel(
                        width(),
                        height(),
                        this->format(),
                        nonAlphaFormat
                    );

            assert( successfullyRemovedAlphaChannel );

            if( !successfullyRemovedAlphaChannel ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
                qDebug() << "ImageLayer::removeAlphaChannel(): Failed to remove alpha channel to image layer backend object. ImageLayer corrupted. Resetting...";
#endif
                this->reset();

                return false;
            }
        }

        d->format = nonAlphaFormat;

        return true;
    }

    return false;
}

ImageLayer* ImageLayer::duplicateAlphaChannel() {
    if( hasAlphaChannel() ) {
        return this->duplicateChannel(
                   3
               );
    }

    return nullptr;
}

/// data management
void ImageLayer::synchronize() {
    for( auto it = d->objects.begin(); it != d->objects.end(); ++it ) {
        ( *it )->imageObject->synchronize();
    }
}

/// properties
libgraphics::Rect32I ImageLayer::size() const {
    return libgraphics::Rect32I( width(), height() );
}

int  ImageLayer::width() const {
    return d->width;
}

int  ImageLayer::height() const {
    return d->height;
}

fxapi::EPixelFormat::t ImageLayer::format() const {
    return d->format;
}

bool ImageLayer::empty() const {
    return ( d->width == 0 ) && ( d->height == 0 );
}

size_t ImageLayer::byteSize() const {
    return width() * height() * pixelSize();
}

size_t ImageLayer::pixelSize() const {
    return libgraphics::fxapi::EPixelFormat::getPixelSize( format() );
}

size_t ImageLayer::channelSize() const {
    return pixelSize() / libgraphics::fxapi::EPixelFormat::getChannelCount( format() );
}

/// internals, automatic backend mirroring
bool ImageLayer::containsDataForBackend(
    int backendId
) const {
    for( auto it = d->objects.begin(); it != d->objects.end(); ++it ) {
        if( ( *it )->backendId == backendId ) {
            return true;
        }
    }

    return false;
}

bool ImageLayer::containsDataForDevice(
    libgraphics::fxapi::ApiBackendDevice* device
) const {
    for( auto it = d->objects.begin(); it != d->objects.end(); ++it ) {
        if( ( *it )->device == device ) {
            return true;
        }
    }

    return false;
}

void ImageLayer::updateDataForBackend(
    libgraphics::fxapi::ApiBackendDevice* device,
    int backendId
) {
    assert( !empty() );

    if( this->d->objects.empty() ) {
        this->d->objects.emplace_back(
            new Private::BackendImageObj(
                device,
                nullptr,
                width(),
                height(),
                format()
            )
        );
    } else {

        if( containsDataForBackend( backendId ) ) {
            return;
        }

        const auto img_width        = width();
        const auto img_height       = height();
        assert( img_width * img_height != 0 );

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
                                               img_width * img_height * pixelSize
                                           );
        assert( temporarySourceBuffer->data );

        const auto successfullyReceivedSourceData = this->retrieve(
                    temporarySourceBuffer->data
                );
        assert( successfullyReceivedSourceData );

        if( !successfullyReceivedSourceData ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "ImageLayer::updateDataForBackend(): Failed to retrieve source data.";
#endif
            return;
        }

        this->d->objects.emplace_back(
            new Private::BackendImageObj(
                device,
                temporarySourceBuffer->data,
                img_width,
                img_height,
                currentFormat
            )
        );
    }
}

fxapi::ApiImageObject* ImageLayer::internalImageForBackend(
    int backendId
) const {
    for( auto it = d->objects.begin(); it != d->objects.end(); ++it ) {
        if( ( *it )->backendId == backendId ) {
            return ( *it )->imageObject;
        }
    }

    return nullptr;
}

fxapi::ApiImageObject* ImageLayer::internalImageForDevice(
    fxapi::ApiBackendDevice* device
) const {
    for( auto it = d->objects.begin(); it != d->objects.end(); ++it ) {
        if( ( *it )->device == device ) {
            return ( *it )->imageObject;
        }
    }

    return nullptr;
}

fxapi::ApiBackendDevice* ImageLayer::internalDeviceForBackend(
    int backendId
) const {
    for( auto it = d->objects.begin(); it != d->objects.end(); ++it ) {
        if( ( *it )->backendId == backendId ) {
            return ( *it )->device;
        }
    }

    return nullptr;
}

bool ImageLayer::updateInternalState( int backendId ) {
    fxapi::EPixelFormat::t      cFormat( this->format() );
    int                         cWidth( this->width() );
    int                         cHeight( this->height() );

    bool found( false );

    for( auto it = d->objects.begin(); it != d->objects.end(); ++it ) {
        if( ( *it )->backendId == backendId ) {
            cWidth  = ( *it )->imageObject->width();
            cHeight = ( *it )->imageObject->height();
            cFormat = ( *it )->imageObject->format();

            found = true;

            break;
        }
    }

    if( !found ) {
        return false;
    }

    this->d->width  = cWidth;
    this->d->height = cHeight;
    this->d->format = cFormat;

    /// !\todo image backend synchronization mode


    for( auto it = d->objects.begin(); it != d->objects.end(); ) {
        if( ( ( *it )->imageObject->width() != cWidth ) || ( ( *it )->imageObject->height() != cHeight ) || ( ( *it )->imageObject->format() != cFormat ) ) {
            it = d->objects.erase( it );
            continue;
        }

        ++it;
    }

    return true;
}

bool ImageLayer::deleteDataForBackend( int backendId ) {
    for( auto it = d->objects.begin(); it != d->objects.end(); ++it ) {
        if( ( *it )->backendId == backendId ) {
            ( *it )->reset();
            it = d->objects.erase( it );
            return true;
        }
    }

    return false;
}

bool ImageLayer::deleteDataForDevice( libgraphics::fxapi::ApiBackendDevice* device ) {
    for( auto it = d->objects.begin(); it != d->objects.end(); ++it ) {
        if( ( *it )->device == device ) {
            ( *it )->reset();
            it = d->objects.erase( it );
            return true;
        }
    }

    return false;
}


}
