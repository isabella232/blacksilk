#include <libgraphics/backend/gl/gl_texture.hpp>
#include <libgraphics/backend/gl/gl_backenddevice.hpp>
#include <libgraphics/utils/gl.hpp>
#include <libgraphics/backend/common/formats.hpp>
#include <libgraphics/backend/gl/gl_priv.hpp>
#include <algorithm>

#include <QDebug>
#include <libgraphics/debug.hpp>

namespace libgraphics {
namespace backend {
namespace gl {

/// constr. / destr.
Texture::Texture() : d( new Private() ) {
    LIBGRAPHICS_MEMORY_LOG_ALLOCATE( this );
}

Texture::Texture( size_t size, fxapi::EPixelFormat::t format ) : d( new Private() ) {
    LIBGRAPHICS_MEMORY_LOG_ALLOCATE( this );
    reset( size, format );
}

Texture::Texture( size_t size, fxapi::EPixelFormat::t format, void* data ) : d( new Private() )  {
    LIBGRAPHICS_MEMORY_LOG_ALLOCATE( this );
    reset( size, format, data );
}

Texture::Texture( size_t width, size_t height, fxapi::EPixelFormat::t format ) : d( new Private() )  {
    LIBGRAPHICS_MEMORY_LOG_ALLOCATE( this );
    reset( width, height, format );
}

Texture::Texture( size_t width, size_t height, fxapi::EPixelFormat::t format, void* data ) : d( new Private() )  {
    LIBGRAPHICS_MEMORY_LOG_ALLOCATE( this );
    reset( width, height, format, data );
}

Texture::~Texture() {
    LIBGRAPHICS_MEMORY_LOG_DEALLOCATE( this );
    this->d.reset();
}

/// create/reset
void Texture::reset() {
    LIBGRAPHICS_MEMORY_LOG_DEALLOCATE( this );
    d.reset( new Private() );
    LIBGRAPHICS_MEMORY_LOG_ALLOCATE( this );
}

namespace {
utils::GL::ETextureDataFormat::t fxToGLFormat( fxapi::EPixelFormat::t format ) {
    utils::GL::ETextureDataFormat::t    glFormat;

    switch( format ) {
        case fxapi::EPixelFormat::Mono8:
            glFormat = utils::GL::ETextureDataFormat::Mono8;
            break;

        case fxapi::EPixelFormat::Mono16S:
        case fxapi::EPixelFormat::Mono16:
            glFormat = utils::GL::ETextureDataFormat::Mono16;
            break;

        case fxapi::EPixelFormat::Mono32F:
            glFormat = utils::GL::ETextureDataFormat::Mono32F;
            break;

        case fxapi::EPixelFormat::RGB8:
            glFormat = utils::GL::ETextureDataFormat::RGB8;
            break;

        case fxapi::EPixelFormat::RGB16:
            glFormat = utils::GL::ETextureDataFormat::RGB16;
            break;

        case fxapi::EPixelFormat::RGBA8:
            glFormat = utils::GL::ETextureDataFormat::RGBA8;
            break;

        case fxapi::EPixelFormat::RGBA16:
            glFormat = utils::GL::ETextureDataFormat::RGBA16;
            break;

        default:
            glFormat = utils::GL::ETextureDataFormat::Empty;
    }

    return glFormat;
}
}

void Texture::reset( size_t size, fxapi::EPixelFormat::t format ) {
    LIBGRAPHICS_MEMORY_LOG_DEALLOCATE( this );

    assert( format != fxapi::EPixelFormat::Empty );

    utils::GL::ETextureDataFormat::t    glFormat = fxToGLFormat( format );
    assert( glFormat != utils::GL::ETextureDataFormat::Empty );

    bool ret = utils::GL::createTextureData2D(
                   gl::globalCtx(),
                   d->tex,
                   glFormat,
                   ( utils::GL::t_uint )size,
                   ( utils::GL::t_uint )size,
                   nullptr
               );
    ( void )ret;
    assert( ret );

    if( ret ) {
        this->d->format = format;
    }

    LIBGRAPHICS_MEMORY_LOG_ALLOCATE( this );
}

void Texture::reset( size_t size, fxapi::EPixelFormat::t format, void* data ) {
    LIBGRAPHICS_MEMORY_LOG_DEALLOCATE( this );
    assert( format != fxapi::EPixelFormat::Empty );

    utils::GL::ETextureDataFormat::t    glFormat = fxToGLFormat( format );
    assert( glFormat != utils::GL::ETextureDataFormat::Empty );

    bool ret = utils::GL::createTextureData2D(
                   gl::globalCtx(),
                   d->tex,
                   glFormat,
                   ( utils::GL::t_uint )size,
                   ( utils::GL::t_uint )size,
                   data
               );
    ( void )ret;
    assert( ret );

    if( ret ) {
        this->d->format = format;
    }

    LIBGRAPHICS_MEMORY_LOG_ALLOCATE( this );
}

void Texture::reset( size_t width, size_t height, fxapi::EPixelFormat::t format ) {
    LIBGRAPHICS_MEMORY_LOG_DEALLOCATE( this );
    assert( format != fxapi::EPixelFormat::Empty );

    utils::GL::ETextureDataFormat::t    glFormat = fxToGLFormat( format );
    assert( glFormat != utils::GL::ETextureDataFormat::Empty );

    bool ret = utils::GL::createTextureData2D(
                   gl::globalCtx(),
                   d->tex,
                   glFormat,
                   ( utils::GL::t_uint )width,
                   ( utils::GL::t_uint )height,
                   nullptr
               );
    ( void )ret;
    assert( ret );

    if( ret ) {
        this->d->format = format;
    }

    LIBGRAPHICS_MEMORY_LOG_ALLOCATE( this );
}

void Texture::reset( size_t width, size_t height, fxapi::EPixelFormat::t format, void* data ) {
    LIBGRAPHICS_MEMORY_LOG_DEALLOCATE( this );
    assert( format != fxapi::EPixelFormat::Empty );

    utils::GL::ETextureDataFormat::t    glFormat = fxToGLFormat( format );
    assert( glFormat != utils::GL::ETextureDataFormat::Empty );

    bool ret = utils::GL::createTextureData2D(
                   gl::globalCtx(),
                   d->tex,
                   glFormat,
                   ( utils::GL::t_uint )width,
                   ( utils::GL::t_uint )height,
                   data
               );
    ( void )ret;
    assert( ret );

    if( ret ) {
        this->d->format = format;
    }

    LIBGRAPHICS_MEMORY_LOG_ALLOCATE( this );
}


/// handling data

/// downloading, retrieving data
bool Texture::retrieve(
    void* buffer
) {
    if( !empty() ) {
        return utils::GL::retrieveTextureData(
                   globalCtx(),
                   d->tex,
                   ( utils::GL::t_uint )width(),
                   ( utils::GL::t_uint )height(),
                   0,
                   buffer
               );
    }

    return false;
}

bool Texture::retrieve(
    void* buffer,
    libgraphics::Rect32I rect
) {
    assert( buffer );

    if( width() < ( rect.width + rect.x ) ) {
        return false;
    }

    if( height() < ( rect.height + rect.y ) ) {
        return false;
    }

    size_t len = libgraphics::fxapi::EPixelFormat::getPixelSize(
                     format()
                 );

    void* intermediateBuffer = ( void* ) new char[
                                width() * height() * len
                            ];

    if( retrieve( intermediateBuffer ) ) {
        void* destinationBuffer = buffer;
        void* sourceBuffer = intermediateBuffer;

        for( size_t i = 0; rect.height > i; ++i ) {
            memcpy( ( void* )( ( char* )destinationBuffer + ( i *  rect.width ) ),
                    ( const void* )( ( const char* )sourceBuffer + ( i *  width() ) + ( rect.y * width() ) + rect.x ),
                    len * rect.width );
        }

        delete []( char* )intermediateBuffer;

        return true;
    }

    delete []( char* )intermediateBuffer;

    return false;
}

bool Texture::retrieve(
    libgraphics::Bitmap* bitmap
) {
    assert( bitmap );

    const auto compatibleFormat = backend::toCompatibleFormat( bitmap->format() );

    if( bitmap->width() < width() ) {
        return false;
    }

    if( bitmap->height() < height() ) {
        return false;
    }

    if( compatibleFormat != format() ) {
        return false;
    }

    const size_t len = libgraphics::fxapi::EPixelFormat::getPixelSize(
                           format()
                       );
    const size_t bufSize = width() * height() * len;


    void* intermediateBuffer( nullptr );
    std::shared_ptr<libgraphics::StdDynamicPoolAllocator::Blob>    bufBlob;

    if( bitmap->allocator() != nullptr ) {
        bufBlob = bitmap->allocator()->alloc(
                      bufSize
                  );
        assert( bufBlob->data != nullptr );

        intermediateBuffer = bufBlob->data;
    } else {
        intermediateBuffer = ( void* ) new char[
                          bufSize
                      ];
    }

    const size_t minWidth   = std::min<size_t>( width(), bitmap->width() );
    const size_t minHeight  = std::min<size_t>( height(), bitmap->height() );

    if( retrieve( intermediateBuffer ) ) {
        bool ret = bitmap->copy(
                       intermediateBuffer,
                       libgraphics::Rect32I( minWidth, minHeight ),
                       width(),
                       height(),
                       0,
                       0
                   );
        assert( ret );

        if( !bufBlob ) {
            delete []( char* )intermediateBuffer;
        }

        return ret;
    }

    if( !bufBlob ) {
        delete []( char* )intermediateBuffer;
    }

    return false;
}

bool Texture::retrieve(
    libgraphics::Bitmap* bitmap,
    libgraphics::Rect32I rect
) {
    assert( bitmap );

    if( bitmap->width() < rect.width ) {
        return false;
    }

    if( bitmap->height() < rect.height ) {
        return false;
    }

    if( width() < ( rect.width + rect.x ) ) {
        return false;
    }

    if( height() < ( rect.height + rect.y ) ) {
        return false;
    }

    if( backend::toCompatibleFormat( bitmap->format() ) != format() ) {
        return false;
    }

    const size_t len = libgraphics::fxapi::EPixelFormat::getPixelSize(
                           format()
                       );
    const size_t bufSize = width() * height() * len;

    void* intermediateBuffer( nullptr );
    std::shared_ptr<libgraphics::StdDynamicPoolAllocator::Blob>    bufBlob;

    if( bitmap->allocator() != nullptr ) {
        bufBlob = bitmap->allocator()->alloc(
                      bufSize
                  );
        assert( bufBlob->data != nullptr );

        intermediateBuffer = bufBlob->data;
    } else {
        intermediateBuffer = ( void* ) new char[
                          bufSize
                      ];
    }


    if( retrieve( intermediateBuffer ) ) {
        void* destinationBuffer = bitmap->buffer();
        void* sourceBuffer = intermediateBuffer;

        for( size_t i = 0; rect.height > i; ++i ) {
            memcpy( ( void* )( ( char* )destinationBuffer + ( ( i *  bitmap->width() ) * len ) ),
                    ( const void* )( ( const char* )sourceBuffer + ( ( ( i *  width() ) + ( rect.y * width() ) + rect.x ) * len ) ),
                    len * rect.width );
        }

        bool ret = bitmap->copy( intermediateBuffer, 0, 0, rect.width, rect.height );

        if( !bufBlob ) {
            delete []( char* )intermediateBuffer;
        }

        return ret;
    }

    if( !bufBlob ) {
        delete []( char* )intermediateBuffer;
    }

    return false;
}


/// uploading data
bool Texture::upload(
    libgraphics::Bitmap* bitmap
) {
    assert( bitmap );

    if( backend::toCompatibleFormat( bitmap->format() ) != format() ) {
        return false;
    }

    if( bitmap->width() < width() ) {
        return false;
    }

    if( bitmap->height() < height() ) {
        return false;
    }

    if( ( bitmap->width() == width() ) && ( bitmap->height() >= height() ) ) {
        return upload(
                   bitmap->buffer()
               );
    }

    const size_t len = libgraphics::fxapi::EPixelFormat::getPixelSize(
                           format()
                       );
    const size_t bufSize = width() * height() * len;

    void* intermediateBuffer( nullptr );
    std::shared_ptr<libgraphics::StdDynamicPoolAllocator::Blob>    bufBlob;

    if( bitmap->allocator() != nullptr ) {
        bufBlob = bitmap->allocator()->alloc(
                      bufSize
                  );
        assert( bufBlob->data != nullptr );

        intermediateBuffer = bufBlob->data;
    } else {
        intermediateBuffer = ( void* ) new char[
                          bufSize
                      ];
    }

    if( bitmap->write( intermediateBuffer, 0, 0, width(), height() ) ) {
        bool ret = upload(
                       intermediateBuffer
                   );

        if( !bufBlob ) {
            delete []( char* )intermediateBuffer;
        }

        return ret;
    }

    if( !bufBlob ) {
        delete []( char* )intermediateBuffer;
    }

    return false;
}

bool Texture::upload(
    libgraphics::Bitmap* bitmap,
    libgraphics::Rect32I rect
) {
    assert( bitmap );

    if( backend::toCompatibleFormat( bitmap->format() ) != format() ) {
        return false;
    }

    if( bitmap->width() < rect.width ) {
        return false;
    }

    if( bitmap->height() < rect.height ) {
        return false;
    }

    if( width() < ( rect.width + rect.x ) ) {
        return false;
    }

    if( height() < ( rect.height + rect.y ) ) {
        return false;
    }

    if( ( rect.width == width() ) && ( rect.height >= height() ) ) {
        return upload(
                   bitmap->buffer(),
                   rect
               );
    }

    size_t len = libgraphics::fxapi::EPixelFormat::getPixelSize(
                     format()
                 );
    void* intermediateBuffer = ( void* ) new char[
                                rect.width * rect.height * len
                            ];

    if( bitmap->write( intermediateBuffer, 0, 0, rect.width, rect.height ) ) {
        bool ret = upload(
                       intermediateBuffer,
                       rect
                   );

        delete []( char* )intermediateBuffer;

        return ret;
    }

    delete []( char* )intermediateBuffer;

    return false;
}

bool Texture::upload(
    void*  data
) {
    assert( data );

    return utils::GL::updateTextureData(
               globalCtx(),
               d->tex,
               data
           );
}

bool Texture::upload(
    void*  data,
    libgraphics::Rect32I rect
) {
    assert( data );

    if( width() < ( rect.width + rect.x ) ) {
        return false;
    }

    if( height() < ( rect.height + rect.y ) ) {
        return false;
    }

    return utils::GL::updateTextureData(
               globalCtx(),
               d->tex,
               rect.x,
               rect.y,
               0,
               rect.width,
               rect.height,
               0,
               data
           );
}

/// properties
size_t  Texture::width() const {
    return d->tex->width();
}

size_t  Texture::height() const {
    return d->tex->height();
}

fxapi::EPixelFormat::t Texture::format() const {
    return d->format;
}

bool Texture::empty() const {
    return d->tex->empty();
}


/// from: backend::gl::Resource
size_t Texture::getGpuMemoryConsumption() {
    if( d->format == libgraphics::fxapi::EPixelFormat::Empty ) {
        return 0;
    }

    size_t pixelLen = libgraphics::fxapi::EPixelFormat::getPixelSize(
                          d->format
                      );

    return pixelLen * width() * height();
}

size_t Texture::getCpuMemoryConsumption() {
    return 0;
}

utils::GLTexture* Texture::internalTexture() const {
    return d->tex;
}

bool Texture::tryAcquire() {
    const auto ret = gl::Resource::tryAcquire();

    if( ret ) {
        LIBGRAPHICS_MEMORY_LOG_ACQUIRE( this );
    }

    return ret;
}

void Texture::acquire() {
    gl::Resource::acquire();
    LIBGRAPHICS_MEMORY_LOG_ACQUIRE( this );
}

bool Texture::release() {
    const auto ret = gl::Resource::release();

    if( ret ) {
        LIBGRAPHICS_MEMORY_LOG_RELEASE( this );
    }

    return ret;
}

/// texture-specific pool methods

/// acquire
Texture* TexturePool::acquireTexture( size_t width, size_t height, fxapi::EPixelFormat::t format ) {

    for( auto it = m_Resources.begin(); it != m_Resources.end(); ++it ) {
        if( ( ( *it )->width() == width ) && ( ( *it )->height() == height ) && ( ( *it )->format() == format ) ) {
            ( *it )->acquire();

            return ( *it ).get();
        }
    }

    return nullptr;
}

Texture* TexturePool::tryAcquireTexture( size_t width, size_t height, fxapi::EPixelFormat::t format ) {

    for( auto it = m_Resources.begin(); it != m_Resources.end(); ++it ) {
        if( ( ( *it )->width() == width ) && ( ( *it )->height() == height ) && ( ( *it )->format() == format ) ) {
            if( ( *it )->tryAcquire() ) {

                return ( *it ).get();
            }
        }
    }

    return nullptr;
}


/// find
Texture* TexturePool::findTexture( size_t width, size_t height, fxapi::EPixelFormat::t format ) {
    for( auto it = m_Resources.begin(); it != m_Resources.end(); ++it ) {
        if( ( ( *it )->width() == width ) && ( ( *it )->height() == height ) && ( ( *it )->format() == format ) ) {
            return ( *it ).get();
        }
    }

    return nullptr;
}


/// count
size_t  TexturePool::countTextures( size_t width, size_t height, fxapi::EPixelFormat::t format ) {
    size_t c( 0 );

    for( auto it = m_Resources.begin(); it != m_Resources.end(); ++it ) {
        if( ( ( *it )->width() == width ) && ( ( *it )->height() == height ) && ( ( *it )->format() == format ) ) {
            ++c;
        }
    }

    return c;
}

size_t  TexturePool::countAvailableTextures( size_t width, size_t height, fxapi::EPixelFormat::t format ) {
    size_t c( 0 );

    for( auto it = m_Resources.begin(); it != m_Resources.end(); ++it ) {
        if( !( *it )->acquired() && ( ( *it )->width() == width ) && ( ( *it )->height() == height ) && ( ( *it )->format() == format ) ) {
            ++c;
        }
    }

    return c;
}

size_t  TexturePool::countAcquiredTextures( size_t width, size_t height, fxapi::EPixelFormat::t format ) {
    size_t c( 0 );

    for( auto it = m_Resources.begin(); it != m_Resources.end(); ++it ) {
        if( ( *it )->acquired() && ( ( *it )->width() == width ) && ( ( *it )->height() == height ) && ( ( *it )->format() == format ) ) {
            ++c;
        }
    }

    return c;
}

size_t  TexturePool::countTexturesWithFormat( fxapi::EPixelFormat::t format ) {
    size_t c( 0 );

    for( auto it = m_Resources.begin(); it != m_Resources.end(); ++it ) {
        if( ( ( *it )->format() == format ) ) {
            ++c;
        }
    }

    return c;
}

size_t  TexturePool::countTexturesWithSize( size_t width, size_t height ) {
    size_t c( 0 );

    for( auto it = m_Resources.begin(); it != m_Resources.end(); ++it ) {
        if( ( ( *it )->width() == width ) && ( ( *it )->height() == height ) ) {
            ++c;
        }
    }

    return c;
}

size_t  TexturePool::countTexturesWithMinimalSize( size_t width, size_t height ) {
    size_t c( 0 );

    for( auto it = m_Resources.begin(); it != m_Resources.end(); ++it ) {
        if( ( ( *it )->width() >= width ) && ( ( *it )->height() >= height ) ) {
            ++c;
        }
    }

    return c;
}

/// create
Texture* TexturePool::createTexture() {
    Texture* tex = new Texture();

    this->add( tex );

    return tex;
}

Texture* TexturePool::createTexture( size_t size, fxapi::EPixelFormat::t format ) {
    Texture* tex = new Texture( size, format );

    this->add( tex );

    return tex;
}

Texture* TexturePool::createTexture( size_t size, fxapi::EPixelFormat::t format, void* data ) {
    Texture* tex = new Texture( size, format, data );

    this->add( tex );

    return tex;
}

Texture* TexturePool::createTexture( size_t width, size_t height, fxapi::EPixelFormat::t format ) {
    Texture* tex = new Texture( width, height, format );

    this->add( tex );

    return tex;
}

Texture* TexturePool::createTexture( size_t width, size_t height, fxapi::EPixelFormat::t format, void* data ) {
    Texture* tex = new Texture( width, height, format, data );

    this->add( tex );

    return tex;
}

Texture* TexturePool::createAndAcquireTexture() {
    Texture* tex = new Texture();

    tex->acquire();

    this->add( tex );

    return tex;
}

Texture* TexturePool::createAndAcquireTexture( size_t size, fxapi::EPixelFormat::t format ) {
    Texture* tex = new Texture( size, format );

    tex->acquire();

    this->add( tex );

    return tex;
}

Texture* TexturePool::createAndAcquireTexture( size_t size, fxapi::EPixelFormat::t format, void* data ) {
    Texture* tex = new Texture( size, format, data );

    tex->acquire();

    this->add( tex );

    return tex;
}

Texture* TexturePool::createAndAcquireTexture( size_t width, size_t height, fxapi::EPixelFormat::t format ) {
    Texture* tex = new Texture( width, height, format );

    tex->acquire();

    this->add( tex );

    return tex;
}

Texture* TexturePool::createAndAcquireTexture( size_t width, size_t height, fxapi::EPixelFormat::t format, void* data ) {
    Texture* tex = new Texture( width, height, format, data );

    tex->acquire();

    this->add( tex );

    return tex;
}
}
}
}
