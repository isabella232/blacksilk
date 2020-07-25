#include <libgraphics/backend/gl/gl_backenddevice.hpp>
#include <libgraphics/backend/gl/gl_priv.hpp>
#include <libgraphics/backend/gl/gl_imageobject.hpp>
#include <libgraphics/utils/gl.hpp>

namespace libgraphics {
namespace backend {
namespace gl {

struct PixelArray::Private {
    utils::GLTexture*                       texture;
    size_t                                  length;
    libgraphics::fxapi::EPixelFormat::t     format;

    Private() : texture( nullptr ), length( 0 ), format( libgraphics::fxapi::EPixelFormat::Empty ) {}
    ~Private() {
        const auto successfullyDestroyedResource = utils::GL::destroyResource( globalCtx(), texture );
        assert( successfullyDestroyedResource );
        ( void )successfullyDestroyedResource;
    }
};

/// constr.
PixelArray::PixelArray() : d( new Private() ) {
    reset();
}

PixelArray::PixelArray( libgraphics::fxapi::EPixelFormat::t format, size_t length ) : d( new Private() )  {
    reset( format, length );
}

PixelArray::PixelArray( libgraphics::fxapi::EPixelFormat::t format, size_t length, const void* data ) : d( new Private() ) {
    reset( format, length, data );
}

/// destr.
PixelArray::~PixelArray() {
    this->d.reset();
}

/// reset
void PixelArray::reset() {
    if( this->d->texture != nullptr ) {
        ( void ) utils::GL::destroyResource( globalCtx(), this->d->texture );
    }

    ( void ) utils::GL::createTexture(
        globalCtx(),
        &this->d->texture
    );
}

namespace {
inline utils::GL::ETextureDataFormat::t fxToGLFormat( libgraphics::fxapi::EPixelFormat::t format ) {
    utils::GL::ETextureDataFormat::t texFormat( utils::GL::ETextureDataFormat::Empty );

    switch( format ) {
        case libgraphics::fxapi::EPixelFormat::Mono8:
            texFormat = utils::GL::ETextureDataFormat::Mono8;
            break;

        case libgraphics::fxapi::EPixelFormat::Mono16S:
        case libgraphics::fxapi::EPixelFormat::Mono16:
            texFormat = utils::GL::ETextureDataFormat::Mono16;
            break;

        case libgraphics::fxapi::EPixelFormat::Mono32F:
            texFormat = utils::GL::ETextureDataFormat::Mono32F;
            break;

        default:
            assert( false );
            return utils::GL::ETextureDataFormat::Empty;
    }

    return texFormat;
}
}

void PixelArray::reset( libgraphics::fxapi::EPixelFormat::t format, size_t length ) {
    utils::GL::ETextureDataFormat::t texFormat = fxToGLFormat( format );

    d->length = length;
    d->format = format;

    ( void ) utils::GL::createTexture1D(
        globalCtx(),
        &this->d->texture,
        texFormat,
        length,
        nullptr
    );
}

void PixelArray::reset( libgraphics::fxapi::EPixelFormat::t format, size_t length, const void* data ) {
    utils::GL::ETextureDataFormat::t texFormat = fxToGLFormat( format );

    d->length = length;
    d->format = format;

    ( void ) utils::GL::createTexture1D(
        globalCtx(),
        &this->d->texture,
        texFormat,
        length,
        ( void* )data
    );
}

/// properties
libgraphics::fxapi::EPixelFormat::t PixelArray::format() const {
    return d->format;
}

size_t PixelArray::length() const {
    return d->length;
}

bool PixelArray::empty() const {
    return ( d->length == 0 );
}

/// methods
void PixelArray::upload( size_t offset, const void* source, size_t length ) {
    assert( this->length() >= offset + length );
    assert( source );

    utils::GL::updateTextureData(
        globalCtx(),
        this->d->texture,
        offset,
        0,
        0,
        length,
        0,
        0,
        ( void* )source
    );
}

void PixelArray::retrieve( size_t offset, void* destination, size_t length ) {
    assert( this->length() >= offset + length );
    assert( destination );

    /// !fix retrieve

    if( ( offset == 0 ) && ( length == this->d->length ) ) {

        utils::GL::retrieveTextureData(
            globalCtx(),
            this->d->texture,
            this->d->length,
            0,
            0,
            destination
        );

        return;
    }

    void* fullBuffer = ( void* ) new char[ sizeof( char ) * this->d->length ];
    assert( fullBuffer != nullptr );

    utils::GL::retrieveTextureData(
        globalCtx(),
        this->d->texture,
        this->d->length,
        0,
        0,
        fullBuffer
    );

    memcpy( ( void* )destination, ( void* )( ( char* )fullBuffer + offset ), length );
    delete []( char* ) fullBuffer;

    return;
}

utils::GLTexture*    PixelArray::texture() {
    return this->d->texture;
}

struct BackendDevice::Private {
    std::unique_ptr<gl::EffectPool>    effectPool;
    std::unique_ptr<gl::TexturePool>   texturePool;
    std::unique_ptr<gl::RenderTargetPool>  renderTargetPool;
    std::shared_ptr<libgraphics::StdDynamicPoolAllocator>  allocator;

    BackendDevice::ERenderingMode                  renderingMode;

    Private() : effectPool( new gl::EffectPool() ),
        texturePool( new gl::TexturePool() ),
        renderTargetPool( new gl::RenderTargetPool() ),
        allocator( new libgraphics::StdDynamicPoolAllocator() ),
        renderingMode( BackendDevice::ERenderingMode::Realtime ) {}
};

BackendDevice::BackendDevice() : d( new Private() ) {}

/// methods
bool    BackendDevice::initialize() {
    initializeGlobalCtx();

    utils::GL* ctx = globalCtx();
    assert( ctx != nullptr );

    if( ctx != nullptr ) {
        return true;
    }


    return false;
}

void    BackendDevice::synchronize() {
    /// empty
}

bool    BackendDevice::shutdown() {
    return true;
}

/// constructors
fxapi::ApiResource* BackendDevice::createTexture1D() {
    return ( fxapi::ApiResource* ) new PixelArray();
}

fxapi::ApiResource* BackendDevice::createTexture1D( const fxapi::EPixelFormat::t& format, size_t length ) {
    return ( fxapi::ApiResource* ) new PixelArray( format, length );
}

fxapi::ApiResource* BackendDevice::createTexture1D( const fxapi::EPixelFormat::t& format, size_t length, void* data ) {
    return ( fxapi::ApiResource* ) new PixelArray( format, length, data );
}

fxapi::ApiImageObject* BackendDevice::createStreamLinedTexture2D() {
    return ( fxapi::ApiImageObject* ) new StreamLinedImageObject( this );
}

fxapi::ApiImageObject* BackendDevice::createStreamLinedTexture2D( const fxapi::EPixelFormat::t& format, size_t width, size_t height ) {
    auto imageObject = new StreamLinedImageObject( this );

    bool ret = imageObject->create( format, width, height );
    assert( ret );

    if( !ret ) {
        delete imageObject;
    }

    return ( fxapi::ApiImageObject* )imageObject;
}

fxapi::ApiImageObject* BackendDevice::createStreamLinedTexture2D( const fxapi::EPixelFormat::t& format, size_t width, size_t height, void* data ) {
    auto imageObject = new StreamLinedImageObject( this );

    bool ret = imageObject->createFromData( format, width, height, data );
    assert( ret );

    if( !ret ) {
        delete imageObject;
    }

    return ( fxapi::ApiImageObject* )imageObject;
}

fxapi::ApiImageObject* BackendDevice::createTexture2D() {
    if( this->d->renderingMode == ERenderingMode::StreamLined ) {
        return createStreamLinedTexture2D();
    }

    return ( fxapi::ApiImageObject* ) new ImageObject( this );
}

fxapi::ApiImageObject* BackendDevice::createTexture2D( const fxapi::EPixelFormat::t& format, size_t width, size_t height ) {
    if( this->d->renderingMode == ERenderingMode::StreamLined ) {
        return createStreamLinedTexture2D(
                   format,
                   width,
                   height
               );
    }

    auto imageObject = new ImageObject( this );

    bool ret = imageObject->create( format, width, height );
    assert( ret );

    if( !ret ) {
        delete imageObject;
    }

    return ( fxapi::ApiImageObject* )imageObject;
}

fxapi::ApiImageObject* BackendDevice::createTexture2D( const fxapi::EPixelFormat::t& format, size_t width, size_t height, void* data ) {
    if( this->d->renderingMode == ERenderingMode::StreamLined ) {
        return createStreamLinedTexture2D(
                   format,
                   width,
                   height,
                   data
               );
    }

    auto imageObject = new ImageObject( this );

    bool ret = imageObject->createFromData( format, width, height, data );
    assert( ret );

    if( !ret ) {
        delete imageObject;
    }

    return ( fxapi::ApiImageObject* )imageObject;
}

/// destructors
bool BackendDevice::destroyTexture1D( fxapi::ApiResource* resource ) {
    assert( resource );

    delete resource;

    return true;
}

bool BackendDevice::destroyTexture2D( fxapi::ApiImageObject* resource ) {
    assert( resource );

    delete resource;

    return true;
}

/// statistics
size_t  BackendDevice::queryManagedMemoryConsumption() {
    size_t  totalSize( 0 );
    totalSize += this->effectPool()->totalCpuMemoryConsumption() + this->renderTargetPool()->totalCpuMemoryConsumption()
                 + texturePool()->totalCpuMemoryConsumption();

    return totalSize;
}

size_t  BackendDevice::queryBackendMemoryConsumption() {
    size_t  totalSize( 0 );
    totalSize += this->effectPool()->totalGpuMemoryConsumption() + this->renderTargetPool()->totalGpuMemoryConsumption()
                 + texturePool()->totalGpuMemoryConsumption();

    return totalSize;
}

/// properties
const char* BackendDevice::name() {
    return "GL";
}

int BackendDevice::backendId() {
    return FXAPI_BACKEND_OPENGL;
}

std::shared_ptr<libgraphics::StdDynamicPoolAllocator>  BackendDevice::allocator() {
    return d->allocator;
}

void BackendDevice::setAllocator( const std::shared_ptr<libgraphics::StdDynamicPoolAllocator>& newAllocator ) {
    d->allocator = newAllocator;
}

/// common resource pools
libgraphics::backend::gl::EffectPool* BackendDevice::effectPool() {
    return d->effectPool.get();
}

libgraphics::backend::gl::RenderTargetPool* BackendDevice::renderTargetPool() {
    return d->renderTargetPool.get();
}

libgraphics::backend::gl::TexturePool* BackendDevice::texturePool() {
    return d->texturePool.get();
}

const BackendDevice::ERenderingMode&   BackendDevice::renderingMode() {
    return d->renderingMode;
}

bool                    BackendDevice::switchRenderingMode(
    const ERenderingMode& newRenderingMode
) {
    this->d->renderingMode = newRenderingMode;
    return true;
}

size_t  BackendDevice::cleanUp() {
    return this->d->renderTargetPool->cleanUp() + this->d->texturePool->cleanUp() + this->d->allocator->releaseUnused();
}

/// returns the global gl object
std::unique_ptr<utils::GL> ctx;


void initializeGlobalCtx() {
    if( !ctx ) {
        ctx.reset( utils::GL::construct() );

        ctx->assignDbgContext( new utils::GLDbgContext() );
    }
}

void resetGlobalCtx() {
    if( ctx ) {
        ctx.reset( nullptr );
    }
}


utils::GL*  globalCtx() {
    assert( ctx.get() );
    return ctx.get();
}

}
}
}
