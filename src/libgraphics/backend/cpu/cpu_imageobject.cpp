#include <libgraphics/backend/cpu/cpu_imageobject.hpp>
#include <libgraphics/backend/common/formats.hpp>
#include <libgraphics/debug.hpp>
#include <QDebug>

namespace libgraphics {
namespace backend {
namespace cpu {

struct ImageObject::Private {
    libgraphics::Bitmap bitmap;
    fxapi::EPixelFormat::t format;
    libgraphics::StdDynamicPoolAllocator* allocator;

    Private() : format( fxapi::EPixelFormat::RGB8 ), allocator( nullptr ) {}
    ~Private() {
        ( void ) bitmap.reset();
    }
};

ImageObject::ImageObject() : d( new Private() ) {
    d->format = fxapi::EPixelFormat::Empty;
    LIBGRAPHICS_MEMORY_LOG_ALLOCATE( this );
}

ImageObject::ImageObject( libgraphics::Bitmap* bitmap ) : d( new Private() ) {
    assert( bitmap );
    LIBGRAPHICS_MEMORY_LOG_ALLOCATE( this );

    ( void ) createFromBitmap(
        bitmap
    );

}

ImageObject::ImageObject( libgraphics::Bitmap* bitmap, libgraphics::Rect32I rect ) : d( new Private() ) {
    assert( bitmap );
    LIBGRAPHICS_MEMORY_LOG_ALLOCATE( this );

    ( void ) createFromBitmap(
        bitmap,
        rect
    );
}

ImageObject::ImageObject( const libgraphics::BitmapInfo& info ) : d( new Private() ) {
    LIBGRAPHICS_MEMORY_LOG_ALLOCATE( this );

    ( void ) createFromBitmapInfo(
        info
    );
}

ImageObject::ImageObject( fxapi::EPixelFormat::t format, size_t width, size_t height, void* data ) : d( new Private() ) {
    assert( data );
    LIBGRAPHICS_MEMORY_LOG_ALLOCATE( this );

    ( void ) createFromData(
        format,
        width,
        height,
        data
    );
}

ImageObject::ImageObject( fxapi::EPixelFormat::t format, size_t width, size_t height ) : d( new Private() ) {
    LIBGRAPHICS_MEMORY_LOG_ALLOCATE( this );

    ( void ) create(
        format,
        width,
        height
    );
}

ImageObject::ImageObject( libgraphics::StdDynamicPoolAllocator* allocator ) : d( new Private() ) {
    this->d->allocator = allocator;
    LIBGRAPHICS_MEMORY_LOG_ALLOCATE( this );
}


ImageObject::ImageObject( libgraphics::StdDynamicPoolAllocator* allocator, const libgraphics::BitmapInfo& info ) : d( new Private() ) {
    this->d->allocator = allocator;

    ( void ) createFromBitmapInfo(
        info
    );
}

ImageObject::ImageObject( libgraphics::StdDynamicPoolAllocator* allocator, fxapi::EPixelFormat::t format, size_t width, size_t height, void* data ) : d( new Private() ) {
    this->d->allocator = allocator;
    LIBGRAPHICS_MEMORY_LOG_ALLOCATE( this );

    ( void ) createFromData(
        format,
        width,
        height,
        data
    );
}

ImageObject::ImageObject( libgraphics::StdDynamicPoolAllocator* allocator, fxapi::EPixelFormat::t format, size_t width, size_t height ) : d( new Private() ) {
    this->d->allocator = allocator;
    LIBGRAPHICS_MEMORY_LOG_ALLOCATE( this );

    ( void ) create(
        format,
        width,
        height
    );
}

/// destr.
ImageObject::~ImageObject() {
    LIBGRAPHICS_MEMORY_LOG_DEALLOCATE( this );
}

/// create
bool ImageObject::createFromBitmap(
    libgraphics::Bitmap* bitmap
) {
    assert( bitmap );
    LIBGRAPHICS_MEMORY_LOG_SCOPED_RESET( this );


    /// check for compatible format
    if( isCompatibleFormat( bitmap ) ) {
        if( this->d->allocator != nullptr ) {
            ( void ) d->bitmap.reset(
                this->d->allocator,
                bitmap->format(),
                bitmap->width(),
                bitmap->height()
            );
        } else {
            ( void ) d->bitmap.reset(
                bitmap->format(),
                bitmap->width(),
                bitmap->height()
            );
        }

        ( void ) d->bitmap.copy(
            bitmap,
            Rect32I( bitmap->width(), bitmap->height() ),
            0,
            0
        );

        return true;

    }

    return false;
}

bool ImageObject::createFromBitmap(
    libgraphics::Bitmap* bitmap,
    libgraphics::Rect32I rect
) {
    assert( bitmap );
    LIBGRAPHICS_MEMORY_LOG_SCOPED_RESET( this );

    /// check for compatible format
    if( isCompatibleFormat( bitmap ) ) {
        if( this->d->allocator != nullptr ) {
            ( void ) d->bitmap.reset(
                this->d->allocator,
                bitmap->format(),
                rect.width,
                rect.height
            );
        } else {
            ( void ) d->bitmap.reset(
                bitmap->format(),
                rect.width,
                rect.height
            );
        }

        ( void ) d->bitmap.copy(
            bitmap,
            rect,
            0,
            0
        );

        d->format = toCompatibleFormat( bitmap->format() );

        return true;
    }

    return false;
}

bool ImageObject::createFromBitmapInfo(
    const libgraphics::BitmapInfo& info
) {
    LIBGRAPHICS_MEMORY_LOG_SCOPED_RESET( this );

    if( isCompatibleFormat( info.format() ) ) {
        if( this->d->allocator != nullptr ) {
            d->bitmap.reset( this->d->allocator, info );
        } else {
            d->bitmap.reset( info );
        }

        d->format = toCompatibleFormat( info.format() );

        return true;
    }

    return false;
}

bool ImageObject::createFromData(
    fxapi::EPixelFormat::t format,
    size_t width,
    size_t height,
    void* data
) {
    assert( data );
    LIBGRAPHICS_MEMORY_LOG_SCOPED_RESET( this );

    if( format != fxapi::EPixelFormat::Empty ) {

        libcommon::SizeType _size( 0 );
        formats::family::t _family( formats::family::RGB );
        libcommon::SizeType _channels( 0 );

        switch( format ) {
            case fxapi::EPixelFormat::RGB8:
                _size = 3;
                _family = formats::family::RGB;
                _channels = 3;
                break;

            case fxapi::EPixelFormat::RGBA8:
                _size = 4;
                _family = formats::family::RGBA;
                _channels = 4;
                break;

            case fxapi::EPixelFormat::RGB16:
                _size = 6;
                _family = formats::family::RGB;
                _channels = 3;
                break;

            case fxapi::EPixelFormat::RGBA16:
                _size = 8;
                _family = formats::family::RGBA;
                _channels = 4;
                break;

            default:
                throw std::runtime_error(
                    "ImageObject: Failed to create ImageObject. Invalid format."
                );
        }

        libgraphics::Format bitmapFormat( _size, _family, _channels );

        if( this->d->allocator != nullptr ) {
            ( void ) d->bitmap.reset(
                this->d->allocator,
                bitmapFormat,
                width,
                height,
                data
            );
        } else {
            ( void ) d->bitmap.reset(
                bitmapFormat,
                width,
                height,
                data
            );
        }

        d->format = format;

        return true;

    }

    return false;
}

bool ImageObject::create(
    fxapi::EPixelFormat::t format,
    size_t width,
    size_t height
) {
    LIBGRAPHICS_MEMORY_LOG_SCOPED_RESET( this );

    if( format != fxapi::EPixelFormat::Empty ) {

        libcommon::SizeType _size( 0 );
        formats::family::t _family( formats::family::RGB );
        libcommon::SizeType _channels( 0 );

        switch( format ) {
            case fxapi::EPixelFormat::RGB8:
                _size = 3;
                _family = formats::family::RGB;
                _channels = 3;
                break;

            case fxapi::EPixelFormat::RGBA8:
                _size = 4;
                _family = formats::family::RGBA;
                _channels = 4;
                break;

            case fxapi::EPixelFormat::RGB16:
                _size = 6;
                _family = formats::family::RGB;
                _channels = 3;
                break;

            case fxapi::EPixelFormat::RGBA16:
                _size = 8;
                _family = formats::family::RGBA;
                _channels = 4;
                break;

            default:
                throw std::runtime_error(
                    "ImageObject: Failed to create ImageObject. Invalid format."
                );
        }

        libgraphics::Format bitmapFormat( _size, _family, _channels );

        if( this->d->allocator != nullptr ) {
            ( void ) d->bitmap.reset(
                this->d->allocator,
                bitmapFormat,
                width,
                height
            );
        } else {
            ( void ) d->bitmap.reset(
                bitmapFormat,
                width,
                height
            );
        }

        d->format = format;


        return true;
    }

    return false;
}

/// downloading, retrieving data
bool ImageObject::retrieve(
    void* buffer
) {
    assert( buffer );

    if( empty() ) {
        return false;
    }

    return retrieve(
               buffer,
               libgraphics::Rect32I(
                   ( unsigned int )width(),
                   ( unsigned int )height()
               )
           );
}

bool ImageObject::retrieve(
    void* buffer,
    libgraphics::Rect32I rect
) {
    assert( buffer );

    if( empty() ) {
        return false;
    }

    return d->bitmap.write(
               buffer,
               rect.x,
               rect.y,
               rect.width,
               rect.height
           );
}

bool ImageObject::retrieve(
    libgraphics::Bitmap* bitmap
) {
    assert( bitmap );
    assert( bitmap->height() == this->height() );
    assert( bitmap->width() == this->width() );
    assert( bitmap->format() == this->d->bitmap.format() );

    if( empty() ) {
        return false;
    }

    return d->bitmap.write(
               bitmap,
               bitmap->size(),
               0,
               0
           );
}

bool ImageObject::retrieve(
    libgraphics::Bitmap* bitmap,
    libgraphics::Rect32I rect
) {
    assert( bitmap );

    if( empty() ) {
        return false;
    }

    return d->bitmap.write(
               bitmap,
               rect.x,
               rect.y,
               rect.width,
               rect.height
           );
}

/// uploading data
bool ImageObject::upload(
    libgraphics::Bitmap* bitmap
) {
    assert( bitmap );

    if( empty() ) {
        return false;
    }

    return d->bitmap.copy(
               bitmap,
               0,
               0,
               d->bitmap.width(),
               d->bitmap.height()
           );
}

bool ImageObject::upload(
    libgraphics::Bitmap* bitmap,
    libgraphics::Rect32I rect,
    size_t destX,
    size_t destY
) {
    assert( bitmap );

    if( empty() ) {
        return false;
    }

    return d->bitmap.copy(
               bitmap,
               rect,
               destX,
               destY
           );
}

bool ImageObject::upload(
    void*  data
) {
    assert( data );

    if( empty() ) {
        return false;
    }

    return d->bitmap.copy(
               data,
               0,
               0,
               width(),
               height()
           );
}

bool ImageObject::upload(
    void*  data,
    libgraphics::Rect32I rect,
    size_t destX,
    size_t destY
) {
    assert( data );

    if( empty() ) {
        return false;
    }

    return d->bitmap.copy(
               data,
               rect,
               destX,
               destY
           );
}

/// copy
bool ImageObject::copy(
    ApiImageObject* source,
    libgraphics::Rect32I sourceRect,
    size_t destX,
    size_t destY
) {
    assert( source );

    if( source->width() < ( size_t )( sourceRect.x + sourceRect.width ) ) {
        return false;
    }

    if( source->height() < ( size_t )( sourceRect.y + sourceRect.height ) ) {
        return false;
    }

    if( width() < ( width() + destX ) ) {
        return false;
    }

    if( height() < ( height() + destY ) ) {
        return false;
    }

    if( source->backendId() == backendId() ) {
        ImageObject* imageObject = ( ImageObject* )source;

        return d->bitmap.copy(
                   &imageObject->d->bitmap,
                   sourceRect,
                   destX,
                   destY
               );
    }

    size_t  pixelSize = fxapi::EPixelFormat::getPixelSize( this->format() );
    auto    dataBuf = this->d->allocator->alloc(
                          sourceRect.width * sourceRect.height * pixelSize
                      );
    void*   buf = dataBuf->data;

    assert( buf );

    if( !source->retrieve( buf, sourceRect ) ) {
        return false;
    }

    bool ret = d->bitmap.copy(
                   buf,
                   destX,
                   destY,
                   sourceRect.width,
                   sourceRect.height
               );

    return ret;

}

/// data management
void ImageObject::discardBuffers() {
    d->bitmap.reset();
}

void ImageObject::synchronize() {
    d->bitmap.synchronize();
}

/// properties
int  ImageObject::width() {
    return d->bitmap.width();
}

int ImageObject::height() {
    return d->bitmap.height();
}

fxapi::EPixelFormat::t ImageObject::format() {
    return d->format;
}

long ImageObject::backendId() {
    return FXAPI_BACKEND_CPU;
}

bool ImageObject::empty() {
    return ( ( width() == 0 ) && ( height() == 0 ) );
}

void* ImageObject::data() {
    return d->bitmap.buffer();
}

/// allocator stuff
bool ImageObject::containsAllocator() const {
    return ( this->d->allocator != nullptr );
}

libgraphics::StdDynamicPoolAllocator*   ImageObject::allocator() const {
    return this->d->allocator;
}

bool ImageObject::assignAllocator(
    libgraphics::StdDynamicPoolAllocator* _allocator
) {
    assert( _allocator );

    if( !_allocator ) {
        return false;
    }

    d->allocator = _allocator;
    return d->bitmap.assignAllocator( _allocator );
}

void ImageObject::clearAllocator() {
    d->allocator = nullptr;
    d->bitmap.clearAllocator();
}

}
}
}
