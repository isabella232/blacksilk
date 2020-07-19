#include <libgraphics/backend/gl/gl_imageobject.hpp>
#include <libgraphics/backend/gl/gl_backenddevice.hpp>
#include <libgraphics/backend/common/formats.hpp>
#include <libgraphics/debug.hpp>

#include <QDebug>
#include <QElapsedTimer>

namespace libgraphics {
namespace backend {
namespace gl {

static const size_t tileSize = 1024;

bool checkIsMonoFormat( fxapi::EPixelFormat::t format ) {
#ifdef _DEBUG
    qDebug() << "Format to test: " << fxapi::EPixelFormat::toString( format );
#endif

    switch( format ) {
        case libgraphics::fxapi::EPixelFormat::Mono8:
        case libgraphics::fxapi::EPixelFormat::Mono16:
        case libgraphics::fxapi::EPixelFormat::Mono16S:
        case libgraphics::fxapi::EPixelFormat::Mono32F:
#ifdef _DEBUG
            qDebug() << "isMonoFormat: true";
#endif
            return true;

        default:
#ifdef _DEBUG
            qDebug() << "isMonoFormat: false";
#endif
            return false;
    }
}

struct ImageObject::Private : libcommon::PimplPrivate {
    struct Tile {
        Tile() : x( 0 ), y( 0 ), realWidth( 0 ),
            realHeight( 0 ), texture( nullptr ), renderTarget( nullptr ) {}

        Rect32I tileArea() const {
            assert( texture );

            return Rect32I(
                       ( int )x, ( int )y, ( int )texture->width(), ( int )texture->height()
                   );
        }
        Rect32I realTileArea() const {
            assert( texture );

            return Rect32I(
                       ( int )x, ( int )y, ( int )realWidth, ( int )realHeight
                   );
        }

        size_t          x;
        size_t          y;
        size_t          realWidth;
        size_t          realHeight;
        gl::Texture*    texture;
        gl::RenderTarget*   renderTarget;
    };
    Private() :
        device( nullptr ),
        width( 0 ),
        height( 0 ),
        realWidth( 0 ),
        realHeight( 0 ),
        horizontalTiles( 0 ),
        verticalTiles( 0 ),
        format( fxapi::EPixelFormat::Empty ) {}
    ~Private() {
        reset();
    }

    void reset() {
        const bool _monoFormat = checkIsMonoFormat( format );

        if( !_monoFormat ) {
            for( auto it = tiles.begin(); it != tiles.end(); ++it ) {
                ( *it ).renderTarget->forceRelease();
            }
        } else {
            for( auto it = tiles.begin(); it != tiles.end(); ++it ) {
                ( *it ).texture->forceRelease();
            }
        }

        tiles.clear();
    }

    void reserveTilesWithSize( size_t realWidth, size_t realHeight, size_t horizontal, size_t vertical, size_t tileSize ) {
        const bool _monoFormat = checkIsMonoFormat( format );

        if( _monoFormat ) {
            qDebug() << "Warning: ImageObject uses a mono-format.";
        }

        assert( device );
        assert( realWidth > 0 );
        assert( realHeight > 0 );
        assert( horizontal > 0 );
        assert( vertical > 0 );

        reset();

        horizontalTiles = horizontal;
        verticalTiles = vertical;

        if( _monoFormat ) {
            /** the exception for mono color formats **/
            for( size_t y = 0; vertical > y; ++y ) {
                for( size_t x = 0; horizontal > x; ++x ) {
                    Tile t;
                    t.renderTarget = nullptr;
                    t.texture      = device->texturePool()->tryAcquireTexture(
                                         tileSize,
                                         tileSize,
                                         format
                                     );

                    if( !t.texture ) {
                        t.texture = device->texturePool()->createAndAcquireTexture(
                                        tileSize,
                                        tileSize,
                                        format
                                    );
                        assert( t.texture );
                    }

                    t.x = x * tileSize;
                    t.y = y * tileSize;

                    if( ( ( x + 1 ) * tileSize ) >= realWidth ) {
                        t.realWidth = realWidth - ( x * tileSize );
                    } else {
                        t.realWidth = tileSize;
                    }

                    if( ( ( y + 1 ) * tileSize ) >= realHeight ) {
                        t.realHeight = realHeight - ( y * tileSize );
                    } else {
                        t.realHeight = tileSize;
                    }

                    tiles.push_back( t );
                }
            }
        } else {
            /** the usual routine for normal color formats **/
            for( size_t y = 0; vertical > y; ++y ) {
                for( size_t x = 0; horizontal > x; ++x ) {
                    Tile t;
                    t.renderTarget = device->renderTargetPool()->tryAcquireRenderTarget(
                                         tileSize,
                                         tileSize,
                                         format
                                     );

                    if( !t.renderTarget ) {
                        t.renderTarget = device->renderTargetPool()->createAndAcquireRenderTarget(
                                             device->texturePool(),
                                             tileSize,
                                             tileSize,
                                             format
                                         );
                        assert( t.renderTarget );
                    }

                    assert( t.renderTarget->complete() );

                    t.texture = t.renderTarget->attachedTexture();
                    assert( t.texture != nullptr );

                    t.x = x * tileSize;
                    t.y = y * tileSize;

                    if( ( ( x + 1 ) * tileSize ) >= realWidth ) {
                        t.realWidth = realWidth - ( x * tileSize );
                    } else {
                        t.realWidth = tileSize;
                    }

                    if( ( ( y + 1 ) * tileSize ) >= realHeight ) {
                        t.realHeight = realHeight - ( y * tileSize );
                    } else {
                        t.realHeight = tileSize;
                    }

                    tiles.push_back( t );
                }
            }
        }
    }
    std::vector<Tile*>  getTilesInArea( libgraphics::Rect32I area ) {
        std::vector<Tile*>  areaTiles;

        for( auto it = tiles.begin(); it != tiles.end(); ++it ) {
            if( area.contains( ( *it ).realTileArea() ) ) {
                areaTiles.push_back(
                    &( *it )
                );
            }
        }

        return areaTiles;
    }


    gl::BackendDevice*  device;
    std::vector<Tile>   tiles;

    /// total size of the image
    size_t              width;
    size_t              height;

    /// the real logical size
    size_t              realWidth;
    size_t              realHeight;

    /// tiles
    size_t              verticalTiles;
    size_t              horizontalTiles;

    fxapi::EPixelFormat::t      format;
};

size_t calculateTileCount( size_t realSize, size_t tileSize = 1024 ) {
    size_t t = realSize % tileSize;
    return ( ( realSize - t ) / tileSize ) + ( t > 0 ? 1 : 0 );
}

ImageObject::ImageObject( fxapi::ApiBackendDevice* device ) : d( new Private() ) {
    assert( device );

    d->device = ( libgraphics::backend::gl::BackendDevice* )device;

    LIBGRAPHICS_MEMORY_LOG_ALLOCATE( this );
}

ImageObject::~ImageObject() {
    LIBGRAPHICS_MEMORY_LOG_DEALLOCATE( this );

    d.reset();
}

/// own properties
size_t  ImageObject::tileCount() const {
    return d->tiles.size();
}

size_t  ImageObject::countVerticalTiles() const {
    return d->verticalTiles;
}

size_t  ImageObject::countHorizontalTiles() const {
    return d->horizontalTiles;
}

gl::Texture* ImageObject::tileTextureAtPosition( size_t x, size_t y ) {
    assert( d->width >= x );
    assert( d->height >= y );

    size_t rx = ( x % tileSize );
    size_t tx = ( ( x - rx ) / tileSize ) + ( rx > 0 ? 1 : 0 );

    size_t ry = ( y % tileSize );
    size_t ty = ( ( y - ry ) / tileSize ) + ( ry > 0 ? 1 : 0 );

    return d->tiles.at(
               d->horizontalTiles * ty + tx
           ).texture;
}

gl::Texture* ImageObject::tileTexture( size_t nx, size_t ny ) {
    assert( d->width / tileSize >= nx );
    assert( d->height / tileSize >= ny );

    return d->tiles.at(
               d->horizontalTiles * ny + nx
           ).texture;
}

gl::RenderTarget* ImageObject::tileRenderTargetAtPosition( size_t x, size_t y ) {
    assert( d->width >= x );
    assert( d->height >= y );

    assert( !isMonoFormat() );

    if( isMonoFormat() ) {
        qDebug() << "Error: ImageObjects of mono-channel-formats are not eligible as rendertargets.";
        return nullptr;
    }

    size_t rx = ( x % tileSize );
    size_t tx = ( ( x - rx ) / tileSize ) + ( rx > 0 ? 1 : 0 );

    size_t ry = ( y % tileSize );
    size_t ty = ( ( y - ry ) / tileSize ) + ( ry > 0 ? 1 : 0 );

    return d->tiles.at(
               d->horizontalTiles * ty + tx
           ).renderTarget;
}

gl::RenderTarget* ImageObject::tileRenderTarget( size_t nx, size_t ny ) {
    assert( d->width / tileSize >= nx );
    assert( d->height / tileSize >= ny );

    assert( !isMonoFormat() );

    if( isMonoFormat() ) {
        qDebug() << "Error: ImageObjects of mono-channel-formats are not eligible as rendertargets.";
        return nullptr;
    }

    return d->tiles.at(
               d->horizontalTiles * ny + nx
           ).renderTarget;
}

libgraphics::Rect32I ImageObject::logicalTileArea( size_t nx, size_t ny ) {
    assert( d->width / tileSize >= nx );
    assert( d->height / tileSize >= ny );

    return d->tiles.at(
               d->horizontalTiles * ny + nx
           ).realTileArea();
}

libgraphics::Rect32I ImageObject::logicalTileAreaAtPosition( size_t x, size_t y ) {
    assert( d->width >= x );
    assert( d->height >= y );

    size_t rx = ( x % tileSize );
    size_t tx = ( ( x - rx ) / tileSize ) + ( rx > 0 ? 1 : 0 );

    size_t ry = ( y % tileSize );
    size_t ty = ( ( y - ry ) / tileSize ) + ( ry > 0 ? 1 : 0 );

    return d->tiles.at(
               d->horizontalTiles * ty + tx
           ).realTileArea();
}

libgraphics::Rect32I ImageObject::tileArea( size_t nx, size_t ny ) {
    assert( d->width / tileSize >= nx );
    assert( d->height / tileSize >= ny );

    return d->tiles.at(
               d->horizontalTiles * ny + nx
           ).tileArea();
}

libgraphics::Rect32I ImageObject::tileAreaAtPosition( size_t x, size_t y ) {
    assert( d->width >= x );
    assert( d->height >= y );

    size_t rx = ( x % tileSize );
    size_t tx = ( ( x - rx ) / tileSize ) + ( rx > 0 ? 1 : 0 );

    size_t ry = ( y % tileSize );
    size_t ty = ( ( y - ry ) / tileSize ) + ( ry > 0 ? 1 : 0 );

    return d->tiles.at(
               ( d->width / tileSize ) * ty + tx
           ).tileArea();
}

gl::TexturePool* ImageObject::texturePool() {
    return d->device->texturePool();
}


/// create
bool ImageObject::createFromBitmap(
    libgraphics::Bitmap* bitmap
) {
    assert( bitmap );
    assert( !bitmap->empty() );

    return createFromBitmap(
               bitmap,
               libgraphics::Rect32I( 0, 0, ( int )bitmap->width(), ( int )bitmap->height() )
           );
}

bool ImageObject::createFromBitmap(
    libgraphics::Bitmap* bitmap,
    libgraphics::Rect32I rect
) {
    LIBGRAPHICS_MEMORY_LOG_SCOPED_RESET( this );

    assert( bitmap );
    assert( !bitmap->empty() );

    if( bitmap->empty() ) {
        return false;
    }

    /// check and assign internal buffer format
    const auto compatibleFormat = backend::toCompatibleFormat(
                                      bitmap->format()
                                  );

    if( compatibleFormat == fxapi::EPixelFormat::Empty ) {
        return false;
    }

    this->d->format = compatibleFormat;

    /// reserve tiles and copy data
    const auto _horizontalTiles = calculateTileCount( rect.width, tileSize );
    const auto _verticalTiles = calculateTileCount( rect.height, tileSize );

    this->d->reserveTilesWithSize(
        rect.width,
        rect.height,
        _horizontalTiles,
        _verticalTiles,
        tileSize
    );
    assert( !d->tiles.empty() );

    void* intermediateBuffer = nullptr;

    for( auto it = d->tiles.begin(); it != d->tiles.end(); ++it ) {

        /// step 1: calculate intermediate buffer size and allocate temporary
        ///  buffer
        const auto currentBufferSize = libgraphics::fxapi::EPixelFormat::getPixelSize(
                                           this->d->format
                                       ) * ( *it ).texture->width() * ( *it ).texture->height();
        assert( currentBufferSize );

        const auto buf = this->d->device->allocator()->alloc(
                             currentBufferSize
                         );
        intermediateBuffer = buf->data;
        assert( intermediateBuffer );


        assert( intermediateBuffer );

        /// step 2: copy data from bitmap to intermediate buffer
        bitmap->write( intermediateBuffer, ( *it ).texture->width(), ( *it ).texture->height(), rect.x + ( *it ).x, rect.y + ( *it ).y, ( *it ).realWidth, ( *it ).realHeight );

        /// step 3: upload intermediate data to gpu-side buffer
        ( *it ).texture->upload( intermediateBuffer );
    }

    return true;
}

bool ImageObject::createFromBitmapInfo(
    const libgraphics::BitmapInfo& info
) {
    LIBGRAPHICS_MEMORY_LOG_SCOPED_RESET( this );

    /// check and assign internal buffer format
    const auto compatibleFormat = backend::toCompatibleFormat(
                                      info.format()
                                  );

    if( compatibleFormat == fxapi::EPixelFormat::Empty ) {
        return false;
    }

    this->d->format = compatibleFormat;

    /// reserve tiles and copy data
    const auto _horizontalTiles = calculateTileCount( info.width(), tileSize );
    const auto _verticalTiles = calculateTileCount( info.height(), tileSize );

    this->d->reserveTilesWithSize(
        info.width(),
        info.height(),
        _horizontalTiles,
        _verticalTiles,
        tileSize
    );
    assert( !d->tiles.empty() );

    return true;
}

bool ImageObject::createFromData(
    fxapi::EPixelFormat::t format,
    size_t width,
    size_t height,
    void* data
) {
    LIBGRAPHICS_MEMORY_LOG_SCOPED_RESET( this );

    assert( width > 0 );
    assert( height > 0 );
    assert( data );

    /// check and assign internal buffer format
    if( format == fxapi::EPixelFormat::Empty ) {
        return false;
    }

    this->d->format = format;

    /// reserve tiles and copy data
    const auto _horizontalTiles = calculateTileCount( width, tileSize );
    const auto _verticalTiles = calculateTileCount( height, tileSize );

    this->d->reserveTilesWithSize(
        width,
        height,
        _horizontalTiles,
        _verticalTiles,
        tileSize
    );
    assert( !d->tiles.empty() );

    this->d->width  = width;
    this->d->height = height;
    this->d->realWidth = _horizontalTiles * tileSize;
    this->d->realHeight = _verticalTiles * tileSize;


    void* intermediateBuffer = nullptr;

    for( auto it = d->tiles.begin(); it != d->tiles.end(); ++it ) {

        /// step 1: calculate intermediate buffer size and allocate temporary
        ///  buffer
        const auto currentBufferSize = libgraphics::fxapi::EPixelFormat::getPixelSize(
                                           this->d->format
                                       ) * ( *it ).texture->width() * ( *it ).texture->height();
        assert( currentBufferSize );
        const auto buf = this->d->device->allocator()->alloc(
                             currentBufferSize
                         );
        intermediateBuffer = buf->data;
        assert( intermediateBuffer );

        /// step 2: copy data from bitmap to intermediate buffer
        const auto pixelSize = libgraphics::fxapi::EPixelFormat::getPixelSize(
                                   this->d->format
                               );

        for( size_t i = 0; ( *it ).realHeight > i; ++i ) {
            memcpy(
                ( void* )( ( char* )intermediateBuffer + ( ( i * ( *it ).texture->width() ) * pixelSize ) ),
                ( void* )( ( char* )data + ( ( ( i * width ) + ( ( *it ).y * width ) + ( *it ).x ) * pixelSize ) ),
                pixelSize * ( *it ).realWidth
            );
        }

        /// step 3: upload intermediate data to gpu-side buffer
        ( *it ).texture->upload( intermediateBuffer );
    }

    return true;
}

bool ImageObject::create(
    fxapi::EPixelFormat::t format,
    size_t width,
    size_t height
) {
    LIBGRAPHICS_MEMORY_LOG_SCOPED_RESET( this );

    assert( format != fxapi::EPixelFormat::Empty );
    assert( height > 0 );
    assert( width > 0 );

    /// check and assign internal buffer format
    if( format == fxapi::EPixelFormat::Empty ) {
        return false;
    }

    this->d->format = format;

    /// reserve tiles and copy data
    const auto _horizontalTiles = calculateTileCount( width, tileSize );
    const auto _verticalTiles = calculateTileCount( height, tileSize );

    this->d->reserveTilesWithSize(
        width,
        height,
        _horizontalTiles,
        _verticalTiles,
        tileSize
    );
    assert( !d->tiles.empty() );

    this->d->width  = width;
    this->d->height = height;
    this->d->realWidth = _horizontalTiles * tileSize;
    this->d->realHeight = _verticalTiles * tileSize;

    return true;
}

/// downloading, retrieving data
bool ImageObject::retrieve(
    void* buffer
) {
    assert( !empty() );
    assert( buffer );

    return retrieve(
               buffer,
               libgraphics::Rect32I( 0, 0, ( int )width(), ( int )height() )
           );
}

bool ImageObject::retrieve(
    void* buffer,
    libgraphics::Rect32I rect
) {
    assert( !empty() );
    assert( buffer );
    assert( this->width() >= rect.x + rect.width );
    assert( this->height() >= rect.y + rect.height );

    std::vector<Private::Tile*> areaTiles = d->getTilesInArea(
            rect
                                            );
    assert( !areaTiles.empty() );

    if( areaTiles.empty() ) {
        return false;
    }

    void* intermediateBuffer = nullptr;

    for( auto it = areaTiles.begin(); it != areaTiles.end(); ++it ) {

        /// step 1: calculate intermediate buffer size and allocate temporary
        ///  buffer
        const auto currentBufferSize = libgraphics::fxapi::EPixelFormat::getPixelSize(
                                           this->d->format
                                       ) * ( *it )->texture->width() * ( *it )->texture->height();
        assert( currentBufferSize );

        const auto buf = this->d->device->allocator()->alloc(
                             currentBufferSize
                         );
        intermediateBuffer = buf->data;
        assert( intermediateBuffer );

        /// step 2: retrieve texture data
        const auto successfullyRetrievedDataFromTexture = ( *it )->texture->retrieve( intermediateBuffer );
        assert( successfullyRetrievedDataFromTexture );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

        if( !successfullyRetrievedDataFromTexture ) {
            qDebug() << "Failed to retrieve data from backend opengl texture object.";
        }

#endif

        /// step 3: copy data from intermediate buffer to
        ///     destination
        Rect32I tileArea = ( *it )->tileArea();
        Rect32I intersectionArea;

        if( !tileArea.getIntersection( rect, intersectionArea ) ) {
            continue;
        }

        const auto pixelLen = libgraphics::fxapi::EPixelFormat::getPixelSize(
                                  this->d->format
                              );

        for( int i = 0; intersectionArea.height > i; ++i ) {
            memcpy(
                ( void* )( ( char* )buffer + ( ( i * rect.width ) + ( intersectionArea.y * rect.width ) + intersectionArea.x ) * pixelLen ),
                ( const void* )( ( char* )intermediateBuffer  + ( i * tileArea.width ) * pixelLen ),
                pixelLen * intersectionArea.width
            );
        }

    }

    return true;
}

bool ImageObject::retrieve(
    libgraphics::Bitmap* bitmap
) {
    assert( bitmap );

    return retrieve(
               bitmap,
               libgraphics::Rect32I(
                   0,
                   0,
                   ( int )bitmap->width(),
                   ( int )bitmap->height()
               )
           );
}

bool ImageObject::retrieve(
    libgraphics::Bitmap* bitmap,
    libgraphics::Rect32I rect
) {
    assert( bitmap );
    assert( width() >= rect.x + rect.width );
    assert( height() >= rect.y + rect.height );

    if( width() < rect.x + rect.width ) {
        return false;
    }

    if( height() < rect.y + rect.height ) {
        return false;
    }

    /// dirty hack: allocate temporary buffer for the specified
    ///     rectangle

    auto intermediateBufferSize = libgraphics::fxapi::EPixelFormat::getPixelSize(
                                      this->d->format
                                  ) * rect.width * rect.height;
    const auto buf           = this->d->device->allocator()->alloc(
                                   intermediateBufferSize
                               );
    void* intermediateBuffer = buf->data;

    if( !retrieve( intermediateBuffer, rect ) ) {
        return false;
    }

    const auto result = bitmap->copy(
                            intermediateBuffer,
                            rect.x,
                            rect.y,
                            rect.width,
                            rect.height
                        );

    return result;
}

/// uploading data
bool ImageObject::upload(
    libgraphics::Bitmap* bitmap
) {
    assert( bitmap );
    assert( width() >= bitmap->width() );
    assert( height() >= bitmap->height() );

    if( width() != bitmap->width() ) {
        return false;
    }

    if( height() != bitmap->height() ) {
        return false;
    }

    assert( !d->tiles.empty() );

    if( d->tiles.empty() ) {
        return false;
    }

    for( auto it = d->tiles.begin(); it != d->tiles.end(); ++it ) {
        ( *it ).texture->upload(
            bitmap,
            ( *it ).realTileArea()
        );
    }


    return true;
}

bool ImageObject::upload(
    libgraphics::Bitmap* bitmap,
    libgraphics::Rect32I rect,
    size_t destX,
    size_t destY
) {
    assert( !empty() );
    assert( bitmap );
    assert( this->width() >= destX + rect.width );
    assert( this->height() >= destY + rect.height );

    if( this->width() < destX + rect.width ) {
        return false;
    }

    if( this->height() < destY + rect.height ) {
        return false;
    }

    std::vector<Private::Tile*> areaTiles = d->getTilesInArea(
            libgraphics::Rect32I( ( int )destX, ( int )destY, rect.width, rect.height )
                                            );
    assert( !areaTiles.empty() );

    if( areaTiles.empty() ) {
        return false;
    }

    for( auto it = areaTiles.begin(); it != areaTiles.end(); ++it ) {
        Rect32I uploadArea;

        if( !( *it )->realTileArea().getIntersection( rect, uploadArea ) ) {
            continue;
        }

        ( *it )->texture->upload(
            bitmap,
            uploadArea
        );
    }

    return true;
}

bool ImageObject::upload(
    void*  data
) {
    assert( data );
    assert( !empty() );

    return upload( data, Rect32I( 0, 0, ( int )width(), ( int )height() ), 0, 0 );
}

bool ImageObject::upload(
    void*  data,
    libgraphics::Rect32I rect,
    size_t destX,
    size_t destY
) {
    assert( data );
    assert( !empty() );
    assert( width() >= rect.x + rect.width );
    assert( height() >= rect.y + rect.height );

    std::vector<Private::Tile*> areaTiles = d->getTilesInArea(
            libgraphics::Rect32I( ( int )destX, ( int )destY, rect.width, rect.height )
                                            );
    assert( !areaTiles.empty() );

    if( areaTiles.empty() ) {
        return false;
    }

    void* intermediateBuffer( nullptr );

    for( auto it = areaTiles.begin(); it != areaTiles.end(); ++it ) {

        Rect32I uploadArea;

        if( !( *it )->realTileArea().getIntersection( rect, uploadArea ) ) {
            continue;
        }

        /// step 1: calculate intermediate buffer size and allocate temporary
        ///  buffer
        const auto pixelLen = libgraphics::fxapi::EPixelFormat::getPixelSize(
                                  this->d->format
                              );
        const auto currentBufferSize = pixelLen * uploadArea.width * uploadArea.height;
        assert( currentBufferSize );

        const auto buf = this->d->device->allocator()->alloc(
                             currentBufferSize
                         );
        intermediateBuffer = buf->data;
        assert( intermediateBuffer );

        /// step 2: copy bytes from data to intermediateBuffer

        for( size_t i = 0; uploadArea.height > i; ++i ) {
            ( void ) memcpy(
                ( void* )( ( char* )intermediateBuffer + ( ( i * uploadArea.width ) /* + (uploadArea.y * (*it)->texture->width()) + uploadArea.x */ ) * pixelLen ),
                ( const void* )( ( char* )data + ( ( i * rect.width ) + ( uploadArea.y * rect.width ) + uploadArea.x + rect.x + ( rect.y * rect.width ) ) * pixelLen ),
                pixelLen * uploadArea.width
            );
        }

        bool ret = ( *it )->texture->upload(
                       intermediateBuffer,
                       libgraphics::Rect32I( uploadArea.width, uploadArea.height )
                   );
        assert( ret );

        if( !ret ) {
            return false;
        }
    }

    return true;
}

/// copy
bool ImageObject::copy(
    ApiImageObject* source,
    libgraphics::Rect32I sourceRect,
    size_t destX,
    size_t destY
) {
    assert( source );
    assert( !empty() );

    const auto bufferSize = libgraphics::fxapi::EPixelFormat::getPixelSize(
                                this->d->format
                            ) * sourceRect.width * sourceRect.height;

    if( !bufferSize ) {
        return false;
    }

    const auto memBlob = this->d->device->allocator()->alloc(
                             bufferSize
                         );
    void* buffer = memBlob->data;

    if( !source->retrieve( buffer, sourceRect ) ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to retrieve data from source.";
#endif

        return false;
    }

    if( !this->upload( buffer, libgraphics::Rect32I( ( int )destX, ( int )destY, sourceRect.width, sourceRect.height ), 0, 0 ) ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to upload data to backend.";
#endif

        return false;
    }

    return true;
}

/// data management
void ImageObject::discardBuffers() {
    d.reset( new Private() );
}

void ImageObject::synchronize() {
    /// dummy
}

/// properties
int  ImageObject::width() {
    return d->width;
}

int  ImageObject::height() {
    return d->height;
}

fxapi::EPixelFormat::t ImageObject::format() {
    return d->format;
}

long ImageObject::backendId() {
    return FXAPI_BACKEND_OPENGL;
}

bool ImageObject::empty() {
    return ( d->width == 0 ) && ( d->height == 0 ) && d->tiles.empty();
}

bool ImageObject::isMonoFormat() const {
    return checkIsMonoFormat( d->format );
}

/** IMPL: StreamLinedImageObject
*/

struct StreamLinedImageObject::Private : libcommon::PimplPrivate {
    gl::BackendDevice*                              device;

    libgraphics::fxapi::EPixelFormat::t             format;
    size_t                                          width;
    size_t                                          height;

    size_t                                          horizontalTiles;
    size_t                                          verticalTiles;
    size_t                                          tileSize;

    struct Tile {
        Tile() : x( 0 ), y( 0 ), realWidth( 0 ),
            realHeight( 0 ), texture( nullptr ), renderTarget( nullptr ) {}

        Rect32I tileArea() const {
            assert( texture );

            return Rect32I(
                       ( int )x, ( int )y, ( int )texture->width(), ( int )texture->height()
                   );
        }
        Rect32I realTileArea() const {
            assert( texture );

            return Rect32I(
                       ( int )x, ( int )y, ( int )realWidth, ( int )realHeight
                   );
        }
        size_t planeSizeInBytes() const {
            return libgraphics::fxapi::EPixelFormat::getPlaneByteSize(
                       texture->format(),
                       texture->width(),
                       texture->height()
                   );
        }
        size_t realPlaneSizeInBytes() const {
            return libgraphics::fxapi::EPixelFormat::getPlaneByteSize(
                       texture->format(),
                       realWidth,
                       realHeight
                   );
        }

        size_t              x;
        size_t              y;
        size_t              realWidth;
        size_t              realHeight;
        gl::Texture*        texture;
        gl::RenderTarget*   renderTarget;
    };
    Tile                                            currentGpuTile;
    libgraphics::Bitmap                             bitmap;

    Private() : device( nullptr ), format( libgraphics::fxapi::EPixelFormat::Empty ),
        width( 0 ), height( 0 ), tileSize( 1024 ), verticalTiles( 0 ), horizontalTiles( 0 ) {}
    ~Private() {
        if( containsGpuTile() ) {
            if( checkIsMonoFormat( format ) ) {
                currentGpuTile.texture->forceRelease();
            } else {
                currentGpuTile.renderTarget->forceRelease();
            }
        }

        const auto successfullyReleased = bitmap.reset();
        assert( successfullyReleased );
    }

    libgraphics::Rect32I    realTileArea( size_t nx, size_t ny ) const {
        libgraphics::Rect32I area;

        area.x = nx * tileSize;
        area.y = ny * tileSize;
        area.width = tileSize;
        area.height = tileSize;

        return area;
    }
    libgraphics::Rect32I    tileArea( size_t nx, size_t ny ) const {
        libgraphics::Rect32I area;

        area.x = nx * tileSize;
        area.y = ny * tileSize;
        area.width = tileSize;
        area.height = tileSize;

        if( ( ( nx + 1 ) * tileSize ) >= width ) {
            area.width = width - ( nx * tileSize );
        } else {
            area.width = tileSize;
        }

        if( ( ( ny + 1 ) * tileSize ) >= height ) {
            area.height = height - ( ny * tileSize );
        } else {
            area.height = tileSize;
        }

        return area;
    }

    /// constructs the internal gpu-tile
    bool constructGpuTile() {
        const bool _isMonoFormat = checkIsMonoFormat( format );

        if( _isMonoFormat ) {
            qDebug() << "Warning: ImageObject is mono-format.";
        }

        if( containsGpuTile() ) {
            return false;
        }

        if( !containsBitmap() ) {
            return false;
        }

        currentGpuTile.x            = 0;
        currentGpuTile.y            = 0;
        currentGpuTile.realHeight   = 0;
        currentGpuTile.realWidth    = 0;

        if( _isMonoFormat ) {
            currentGpuTile.renderTarget = nullptr;
            currentGpuTile.texture      = device->texturePool()->tryAcquireTexture(
                                              tileSize,
                                              tileSize,
                                              format
                                          );

            if( !currentGpuTile.texture ) {
                currentGpuTile.texture = device->texturePool()->createAndAcquireTexture(
                                             tileSize,
                                             tileSize,
                                             format
                                         );
                assert( currentGpuTile.texture );
            }

            return true;
        } else {
            currentGpuTile.renderTarget = device->renderTargetPool()->tryAcquireRenderTarget(
                                              tileSize,
                                              tileSize,
                                              format
                                          );

            if( !currentGpuTile.renderTarget ) {
                currentGpuTile.renderTarget = device->renderTargetPool()->createAndAcquireRenderTarget(
                                                  device->texturePool(),
                                                  tileSize,
                                                  tileSize,
                                                  format
                                              );
                assert( currentGpuTile.renderTarget );
            }

            assert( currentGpuTile.renderTarget->complete() );

            currentGpuTile.texture = currentGpuTile.renderTarget->attachedTexture();
            assert( currentGpuTile.texture != nullptr );

            return ( currentGpuTile.renderTarget->complete() );
        }
    }

    /// discards the current internal state
    bool discardGpuState() {
        if( containsGpuTile() ) {
            currentGpuTile.renderTarget->forceRelease();
            return true;
        }

        return false;
    }

    /// copies the current tile back to the cpu
    /// buffer
    bool reflectGpuState() {

        if( !containsGpuTile() || !containsBitmap() ) {
            assert( false );
            return false;
        }

        libgraphics::Bitmap tileBitmap(
            device->allocator().get(),
            libgraphics::backend::fromCompatibleFormat( format ),
            currentGpuTile.texture->width(),
            currentGpuTile.texture->height()
        );

        assert( !tileBitmap.empty() );
        const auto successfullyRetrieved = currentGpuTile.texture->retrieve(
                                               &tileBitmap
                                           );
        assert( successfullyRetrieved );

        if( !successfullyRetrieved ) {
            return false;
        }

        const auto successfullyCopied = bitmap.copy(
                                            &tileBitmap,
                                            libgraphics::Rect32I( 0, 0, currentGpuTile.realWidth, currentGpuTile.realHeight ),
                                            currentGpuTile.x,
                                            currentGpuTile.y
                                        );
        assert( successfullyCopied );

        return successfullyCopied;
    }

    /// reflect the current gpu-state and switch to a new
    /// tile
    bool switchTile( size_t sx, size_t sy ) {
        if( containsGpuTile() ) {
            const auto successfullyReflected = reflectGpuState();
            assert( successfullyReflected );

            if( !successfullyReflected ) {
                return false;
            }
        } else {
            const auto successfullySetup = constructGpuTile();
            assert( successfullySetup );

            if( !successfullySetup ) {
                return false;
            }
        }

        currentGpuTile.x = ( sx * tileSize );
        currentGpuTile.y = ( sy * tileSize );

        currentGpuTile.realWidth = std::min(
                                       tileSize,
                                       width - currentGpuTile.x
                                   );
        currentGpuTile.realHeight = std::min(
                                        tileSize,
                                        height - currentGpuTile.y
                                    );

        /// step 1: calculate intermediate buffer size and allocate temporary
        ///  buffer
        libgraphics::Bitmap tileData(
            bitmap.allocator(),
            libgraphics::backend::fromCompatibleFormat( format ),
            tileSize,
            tileSize
        );

        /// step 2: copy data from bitmap to intermediate buffer
        const auto successfullyWritten = bitmap.write( &tileData, libgraphics::Rect32I( currentGpuTile.x, currentGpuTile.y, currentGpuTile.realWidth, currentGpuTile.realHeight ), 0, 0 ); //bitmap.write( intermediateBuffer, libgraphics::Rect64UI(currentGpuTile.realWidth,currentGpuTile.realHeight), tileSize, tileSize, libgraphics::Rect64UI(currentGpuTile.x,currentGpuTile.y,currentGpuTile.realWidth,currentGpuTile.realHeight) );
        assert( successfullyWritten );

        /// step 3: upload intermediate data to gpu-side buffer
        const auto successfullyUploaded = currentGpuTile.texture->upload( &tileData );
        assert( successfullyUploaded );

        return true;
    }

    inline bool containsGpuTile() const {
        return ( currentGpuTile.texture != nullptr ) &&
               ( currentGpuTile.renderTarget != nullptr );
    }
    inline bool containsBitmap() const {
        return !bitmap.empty();
    }
};

StreamLinedImageObject::StreamLinedImageObject( fxapi::ApiBackendDevice* device ) : ImageObject( device ), d( new Private() ) {
    assert( device );

    d->device = ( libgraphics::backend::gl::BackendDevice* )device;
    LIBGRAPHICS_MEMORY_LOG_ALLOCATE( this );
}

StreamLinedImageObject::~StreamLinedImageObject() {
    LIBGRAPHICS_MEMORY_LOG_DEALLOCATE( this );
    d.reset();
}

/// own properties
size_t  StreamLinedImageObject::tileCount() const {
    return d->verticalTiles * d->horizontalTiles;
}

size_t  StreamLinedImageObject::countVerticalTiles() const {
    return d->verticalTiles;
}

size_t  StreamLinedImageObject::countHorizontalTiles() const {
    return d->horizontalTiles;
}

gl::Texture* StreamLinedImageObject::tileTextureAtPosition( size_t x, size_t y ) {
    assert( d->width >= x );
    assert( d->height >= y );

    const size_t rx = ( x % tileSize );
    const size_t tx = ( ( x - rx ) / tileSize ) + ( rx > 0 ? 1 : 0 );

    const size_t ry = ( y % tileSize );
    const size_t ty = ( ( y - ry ) / tileSize ) + ( ry > 0 ? 1 : 0 );

    if( ( d->currentGpuTile.x != ( tx * tileSize ) ) || ( d->currentGpuTile.y != ( ty * tileSize ) ) ) {
        const auto successfullySwitched = d->switchTile(
                                              tx,
                                              ty
                                          );
        assert( successfullySwitched );
#if LIBGRAPHICS_DEBUG_OUTPUT

        if( !successfullySwitched ) {
            qDebug() << "Error: Failed to switch to specified tile(" << tx << "," << ty << ")";
        }

#endif
    }

    return d->currentGpuTile.texture;
}

gl::Texture* StreamLinedImageObject::tileTexture( size_t nx, size_t ny ) {
    assert( d->width / tileSize >= nx );
    assert( d->height / tileSize >= ny );

    if( ( d->currentGpuTile.x != ( nx * d->tileSize ) ) || ( d->currentGpuTile.y != ( ny * d->tileSize ) ) ) {
        const auto successfullySwitched = d->switchTile(
                                              nx,
                                              ny
                                          );
        assert( successfullySwitched );
#if LIBGRAPHICS_DEBUG_OUTPUT

        if( !successfullySwitched ) {
            qDebug() << "Error: Failed to switch to specified tile(" << nx << "," << ny << ")";
        }

#endif
    }

    return d->currentGpuTile.texture;
}

gl::RenderTarget* StreamLinedImageObject::tileRenderTargetAtPosition( size_t x, size_t y ) {
    assert( d->width >= x );
    assert( d->height >= y );

    assert( !isMonoFormat() );

    if( isMonoFormat() ) {
        qDebug() << "Error: ImageObjects of mono-channel-formats are not eligible as rendertargets.";
        return nullptr;
    }

    const size_t rx = ( x % tileSize );
    const size_t tx = ( ( x - rx ) / tileSize ) + ( rx > 0 ? 1 : 0 );

    const size_t ry = ( y % tileSize );
    const size_t ty = ( ( y - ry ) / tileSize ) + ( ry > 0 ? 1 : 0 );

    if( ( d->currentGpuTile.x != ( tx * tileSize ) ) || ( d->currentGpuTile.y != ( ty * tileSize ) ) ) {
        const auto successfullySwitched = d->switchTile(
                                              tx,
                                              ty
                                          );
        assert( successfullySwitched );
#if LIBGRAPHICS_DEBUG_OUTPUT

        if( !successfullySwitched ) {
            qDebug() << "Error: Failed to switch to specified tile(" << tx << "," << ty << ")";
        }

#endif
    }

    return d->currentGpuTile.renderTarget;
}

gl::RenderTarget* StreamLinedImageObject::tileRenderTarget( size_t nx, size_t ny ) {
    assert( d->width / tileSize >= nx );
    assert( d->height / tileSize >= ny );

    assert( !isMonoFormat() );

    if( isMonoFormat() ) {
        qDebug() << "Error: ImageObjects of mono-channel-formats are not eligible as rendertargets.";
        return nullptr;
    }

    if( ( d->currentGpuTile.x != ( nx * d->tileSize ) ) || ( d->currentGpuTile.y != ( ny * d->tileSize ) ) ) {
        const auto successfullySwitched = d->switchTile(
                                              nx,
                                              ny
                                          );
        assert( successfullySwitched );
#if LIBGRAPHICS_DEBUG_OUTPUT

        if( !successfullySwitched ) {
            qDebug() << "Error: Failed to switch to specified tile(" << nx << "," << ny << ")";
        }

#endif
    }

    return d->currentGpuTile.renderTarget;
}

libgraphics::Rect32I StreamLinedImageObject::logicalTileArea( size_t nx, size_t ny ) {
    assert( d->width / tileSize >= nx );
    assert( d->height / tileSize >= ny );

    return d->tileArea(
               nx,
               ny
           );
}

libgraphics::Rect32I StreamLinedImageObject::logicalTileAreaAtPosition( size_t x, size_t y ) {
    assert( d->width >= x );
    assert( d->height >= y );

    const size_t rx = ( x % tileSize );
    const size_t tx = ( ( x - rx ) / tileSize ) + ( rx > 0 ? 1 : 0 );

    const size_t ry = ( y % tileSize );
    const size_t ty = ( ( y - ry ) / tileSize ) + ( ry > 0 ? 1 : 0 );

    return d->tileArea(
               tx,
               ty
           );
}

libgraphics::Rect32I StreamLinedImageObject::tileArea( size_t nx, size_t ny ) {
    assert( d->width / tileSize >= nx );
    assert( d->height / tileSize >= ny );

    return d->realTileArea(
               nx,
               ny
           );
}

libgraphics::Rect32I StreamLinedImageObject::tileAreaAtPosition( size_t x, size_t y ) {
    assert( d->width >= x );
    assert( d->height >= y );

    const size_t rx = ( x % tileSize );
    const size_t tx = ( ( x - rx ) / tileSize ) + ( rx > 0 ? 1 : 0 );

    const size_t ry = ( y % tileSize );
    const size_t ty = ( ( y - ry ) / tileSize ) + ( ry > 0 ? 1 : 0 );

    return d->realTileArea(
               tx,
               ty
           );
}
gl::TexturePool* StreamLinedImageObject::texturePool() {
    return d->device->texturePool();
}


/// create
bool StreamLinedImageObject::createFromBitmap(
    libgraphics::Bitmap* bitmap
) {
    assert( bitmap );
    assert( !bitmap->empty() );

    return createFromBitmap(
               bitmap,
               libgraphics::Rect32I( 0, 0, ( int )bitmap->width(), ( int )bitmap->height() )
           );
}

bool StreamLinedImageObject::createFromBitmap(
    libgraphics::Bitmap* bitmap,
    libgraphics::Rect32I rect
) {
    LIBGRAPHICS_MEMORY_LOG_SCOPED_RESET( this );

    assert( bitmap );
    assert( !bitmap->empty() );

    libgraphics::fxapi::EPixelFormat::t   compatibleFormat = libgraphics::backend::toCompatibleFormat(
                bitmap->format()
            );
    assert( compatibleFormat != libgraphics::fxapi::EPixelFormat::Empty );

    if( compatibleFormat == libgraphics::fxapi::EPixelFormat::Empty ) {
        return false;
    }

    d->format       = compatibleFormat;
    d->width        = rect.width;
    d->height       = rect.height;
    d->horizontalTiles = calculateTileCount( rect.width, d->tileSize );
    d->verticalTiles   = calculateTileCount( rect.height, d->tileSize );

    const auto successfullyReset = d->bitmap.reset(
                                       d->device->allocator().get(),
                                       bitmap->format(),
                                       rect.width,
                                       rect.height
                                   );
    assert( successfullyReset );

    const auto successfullyCopied = d->bitmap.copy(
                                        bitmap,
                                        libgraphics::Rect32I( rect.x, rect.y, rect.width, rect.height ),
                                        0,
                                        0
                                    );
    assert( successfullyCopied );

    ( void ) d->discardGpuState();

    const auto successfullySwitched = this->d->switchTile( 0, 0 );
    assert( successfullySwitched );

    return true;
}

bool StreamLinedImageObject::createFromBitmapInfo(
    const libgraphics::BitmapInfo& info
) {
    libgraphics::fxapi::EPixelFormat::t    compatibleFormat = libgraphics::backend::toCompatibleFormat(
                info.format()
            );
    assert( compatibleFormat != libgraphics::fxapi::EPixelFormat::Empty );

    if( compatibleFormat == libgraphics::fxapi::EPixelFormat::Empty ) {
        return false;
    }

    return create(
               compatibleFormat,
               info.width(),
               info.height()
           );
}

bool StreamLinedImageObject::createFromData(
    fxapi::EPixelFormat::t format,
    size_t width,
    size_t height,
    void* data
) {
    LIBGRAPHICS_MEMORY_LOG_SCOPED_RESET( this );

    assert( ( width * height ) != 0 );
    assert( data != nullptr );
    assert( format != fxapi::EPixelFormat::Empty );

    libgraphics::Format bitmapFormat = libgraphics::backend::fromCompatibleFormat(
                                           format
                                       );

    d->format       = format;
    d->width        = width;
    d->height       = height;
    d->horizontalTiles = calculateTileCount( width, d->tileSize );
    d->verticalTiles   = calculateTileCount( height, d->tileSize );

    const auto successfullyReset = d->bitmap.reset(
                                       d->device->allocator().get(),
                                       bitmapFormat,
                                       width,
                                       height,
                                       data
                                   );
    assert( successfullyReset );

    ( void ) d->discardGpuState();

    const auto successfullySwitched = this->d->switchTile( 0, 0 );
    assert( successfullySwitched );

    return true;
}

bool StreamLinedImageObject::create(
    fxapi::EPixelFormat::t format,
    size_t width,
    size_t height
) {
    LIBGRAPHICS_MEMORY_LOG_SCOPED_RESET( this );

    assert( ( width * height ) != 0 );
    assert( format != fxapi::EPixelFormat::Empty );

    libgraphics::Format bitmapFormat = libgraphics::backend::fromCompatibleFormat(
                                           format
                                       );

    d->format       = format;
    d->width        = width;
    d->height       = height;
    d->horizontalTiles = calculateTileCount( width, d->tileSize );
    d->verticalTiles   = calculateTileCount( height, d->tileSize );

    const auto successfullyReset = d->bitmap.reset(
                                       d->device->allocator().get(),
                                       bitmapFormat,
                                       width,
                                       height
                                   );
    assert( successfullyReset );

    ( void ) d->discardGpuState();

    const auto successfullySwitched = this->d->switchTile( 0, 0 );
    assert( successfullySwitched );

    return true;
}

/// downloading, retrieving data
bool StreamLinedImageObject::retrieve(
    void* buffer
) {
    assert( !empty() );
    assert( buffer );

    return retrieve(
               buffer,
               libgraphics::Rect32I( 0, 0, ( int )width(), ( int )height() )
           );
}

bool StreamLinedImageObject::retrieve(
    void* buffer,
    libgraphics::Rect32I rect
) {
    assert( !empty() );
    assert( buffer );
    assert( this->width() >= rect.x + rect.width );
    assert( this->height() >= rect.y + rect.height );

    this->synchronize();

    const auto successfullyWritten = d->bitmap.write(
                                         buffer,
                                         rect.x,
                                         rect.y,
                                         rect.width,
                                         rect.height
                                     );
    assert( successfullyWritten );

    return successfullyWritten;
}

bool StreamLinedImageObject::retrieve(
    libgraphics::Bitmap* bitmap
) {
    assert( !empty() );
    assert( bitmap );

    this->synchronize();

    const auto minWidth = std::min(
                              d->bitmap.width(),
                              bitmap->width()
                          );
    const auto minHeight = std::min(
                               d->bitmap.height(),
                               bitmap->height()
                           );
    assert( ( minWidth * minHeight ) != 0 );

    const auto successfullyWritten = d->bitmap.write(
                                         bitmap,
                                         libgraphics::Rect32I( minWidth, minHeight ),
                                         0,
                                         0
                                     );
    assert( successfullyWritten );

    return successfullyWritten;
}

bool StreamLinedImageObject::retrieve(
    libgraphics::Bitmap* bitmap,
    libgraphics::Rect32I rect
) {
    assert( bitmap );
    assert( width() >= rect.x + rect.width );
    assert( height() >= rect.y + rect.height );
    assert( !empty() );

    this->synchronize();

    const auto successfullyWritten = d->bitmap.write(
                                         bitmap,
                                         libgraphics::Rect32I( rect.x, rect.y, rect.width, rect.height ),
                                         0,
                                         0
                                     );
    assert( successfullyWritten );

    return successfullyWritten;
}

/// uploading data
bool StreamLinedImageObject::upload(
    libgraphics::Bitmap* bitmap
) {
    assert( bitmap );
    assert( width() >= bitmap->width() );
    assert( height() >= bitmap->height() );

    return upload(
               bitmap,
               libgraphics::Rect32I( 0, 0, bitmap->width(), bitmap->height() ),
               0,
               0
           );
}

bool StreamLinedImageObject::upload(
    libgraphics::Bitmap* bitmap,
    libgraphics::Rect32I rect,
    size_t destX,
    size_t destY
) {
    assert( !empty() );
    assert( bitmap );
    assert( this->width() >= destX + rect.width );
    assert( this->height() >= destY + rect.height );

    const auto successfullyCopied = d->bitmap.copy(
                                        bitmap,
                                        libgraphics::Rect32I( rect.x, rect.y, rect.width, rect.height ),
                                        destX,
                                        destY
                                    );
    assert( successfullyCopied );

    if( !successfullyCopied ) {
        return false;
    }

    const auto successfullySwitched = d->switchTile( 0, 0 );
    assert( successfullySwitched );

    if( !successfullyCopied ) {
        return false;
    }

    return true;
}

bool StreamLinedImageObject::upload(
    void*  data
) {
    assert( data );
    assert( !empty() );

    return upload( data, Rect32I( 0, 0, ( int )width(), ( int )height() ), 0, 0 );
}

bool StreamLinedImageObject::upload(
    void*  data,
    libgraphics::Rect32I rect,
    size_t destX,
    size_t destY
) {
    assert( !empty() );
    assert( data != nullptr );
    assert( this->width() >= destX + rect.width );
    assert( this->height() >= destY + rect.height );

    const auto successfullyCopied = d->bitmap.copy( data, libgraphics::Rect32I( rect.x, rect.y, rect.width, rect.height ), destX, destY );
    assert( successfullyCopied );

    if( !successfullyCopied ) {
        return false;
    }

    const auto successfullySwitched = d->switchTile( 0, 0 );
    assert( successfullySwitched );

    if( !successfullyCopied ) {
        return false;
    }

    return true;
}

/// copy
bool StreamLinedImageObject::copy(
    ApiImageObject* source,
    libgraphics::Rect32I sourceRect,
    size_t destX,
    size_t destY
) {
    assert( source );
    assert( !empty() );

    const auto bufferSize = libgraphics::fxapi::EPixelFormat::getPixelSize(
                                this->d->format
                            ) * sourceRect.width * sourceRect.height;

    if( !bufferSize ) {
        return false;
    }

    const auto memBlob = this->d->device->allocator()->alloc(
                             bufferSize
                         );
    void* buffer = memBlob->data;

    if( !source->retrieve( buffer, sourceRect ) ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to retrieve data from source.";
#endif

        return false;
    }

    if( !this->upload( buffer, libgraphics::Rect32I( ( int )destX, ( int )destY, sourceRect.width, sourceRect.height ), 0, 0 ) ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to upload data to backend.";
#endif

        return false;
    }

    return true;
}

/// data management
void StreamLinedImageObject::discardBuffers() {
    d.reset( new Private() );
}

void StreamLinedImageObject::synchronize() {
    const auto successfullyReflected = d->reflectGpuState();
    assert( successfullyReflected );
}

/// properties
int  StreamLinedImageObject::width() {
    return d->width;
}

int  StreamLinedImageObject::height() {
    return d->height;
}

fxapi::EPixelFormat::t StreamLinedImageObject::format() {
    return d->format;
}

long StreamLinedImageObject::backendId() {
    return FXAPI_BACKEND_OPENGL;
}

bool StreamLinedImageObject::empty() {
    return ( d->width == 0 ) && ( d->height == 0 );
}

bool StreamLinedImageObject::isMonoFormat() const {
    return checkIsMonoFormat( d->format );
}



}
}
}
