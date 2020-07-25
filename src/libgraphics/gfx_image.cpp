#include <libgraphics/image.hpp>
#include <libgraphics/image_p.hpp>
#include <libgraphics/backend/common/formats.hpp>
#include <QDebug>
#include <QElapsedTimer>

#include <libgraphics/debug.hpp>

namespace libgraphics {

ImageLayer* makeImageLayer(
    fxapi::ApiBackendDevice* device,
    const std::string& name,
    libgraphics::fxapi::EPixelFormat::t format,
    size_t width,
    size_t height
) {
    assert( device != nullptr );
    assert( format != libgraphics::fxapi::EPixelFormat::Empty );
    assert( width * height > 0 );

    const auto imageLayer = new ImageLayer(
        device,
        name
    );
    assert( imageLayer != nullptr );


    if( imageLayer != nullptr ) {
        const auto successfullyResetted = imageLayer->reset(
                                              format,
                                              width,
                                              height
                                          );

        if( !successfullyResetted ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "Failed to reset image layer. Aborting...";
#endif
            delete imageLayer;

            return nullptr;
        }

        return imageLayer;
    }

    return nullptr;
}

ImageLayer* makeImageLayer(
    fxapi::ApiBackendDevice* _device,
    ImageLayer* _templateInstance,
    bool _copyData
) {
    ImageLayer* layer = makeImageLayer(
                            _device,
                            _templateInstance->name(),
                            _templateInstance->format(),
                            _templateInstance->width(),
                            _templateInstance->height()
                        );
    assert( layer != nullptr );

    if( layer != nullptr && _copyData ) {
        const auto successfullyCopied = layer->copy(
                                            _templateInstance
                                        );
        assert( successfullyCopied == true );

        if( !successfullyCopied ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "Failed to create new image layer from template instance - Failed to copy data.";
#endif
            delete layer;
        }
    }

    return layer;
}

ImageLayer* makeImageLayer(
    fxapi::ApiBackendDevice* device,
    const std::string& name,
    libgraphics::fxapi::EPixelFormat::t format,
    size_t width,
    size_t height,
    void* data
) {
    assert( device != nullptr );
    assert( format != libgraphics::fxapi::EPixelFormat::Empty );
    assert( data != nullptr );
    assert( width * height > 0 );

    const auto imageLayer = new ImageLayer(
        device,
        name
    );
    assert( imageLayer != nullptr );

    if( imageLayer != nullptr ) {
        const auto successfullyResetted = imageLayer->reset(
                                              format,
                                              width,
                                              height,
                                              data
                                          );
        assert( successfullyResetted );

        if( !successfullyResetted ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "Failed to reset image layer. Aborting...";
#endif
            delete imageLayer;

            return nullptr;
        }

        return imageLayer;
    }

    return nullptr;
}

ImageLayer* makeImageLayer(
    fxapi::ApiBackendDevice* device,
    const std::string& name,
    libgraphics::Bitmap* bitmap
) {
    return makeImageLayer(
               device,
               name,
               bitmap,
               libgraphics::Rect32I(
                   ( int )bitmap->width(),
                   ( int )bitmap->height()
               )
           );
}

ImageLayer* makeImageLayer(
    fxapi::ApiBackendDevice* device,
    const std::string& name,
    libgraphics::Bitmap* bitmap,
    libgraphics::Rect32I area
) {
    assert( device != nullptr );
    assert( bitmap != nullptr );

    const auto imageLayer = new ImageLayer(
        device,
        name
    );
    assert( imageLayer != nullptr );

    if( imageLayer != nullptr ) {
        const auto successfullyResetted = imageLayer->reset(
                                              bitmap,
                                              area
                                          );
        assert( successfullyResetted );

        if( !successfullyResetted ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "Failed to reset image layer. Aborting...";
#endif
            delete imageLayer;

            return nullptr;
        }

        return imageLayer;
    }

    return nullptr;
}

ImageLayer* makeImageLayer(
    fxapi::ApiBackendDevice* device,
    const std::string& name,
    const libgraphics::BitmapInfo& info
) {
    assert( device != nullptr );
    assert( info.width() * info.height() > 0 );

    const auto imageLayer = new ImageLayer(
        device,
        name
    );
    assert( imageLayer != nullptr );

    if( imageLayer != nullptr ) {
        const auto successfullyResetted = imageLayer->reset(
                                              info
                                          );
        assert( successfullyResetted );

        if( !successfullyResetted ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "Failed to reset image layer. Aborting...";
#endif
            delete imageLayer;

            return nullptr;
        }

        return imageLayer;
    }

    return nullptr;
}

ImageLayer* makeImageLayer(
    fxapi::ApiBackendDevice* device,
    const std::string& name
) {
    const auto imageLayer = new ImageLayer(
        device,
        name
    );
    assert( imageLayer != nullptr );

    return imageLayer;
}

ImageLayer* emplaceImageLayer(
    Image* destinationImage,
    fxapi::ApiBackendDevice* device,
    const std::string& name
) {
    assert( destinationImage );
    assert( device );

    const auto imageLayer = makeImageLayer(
                                device,
                                name
                            );
    assert( imageLayer != nullptr );

    if( !destinationImage->appendLayer(
                imageLayer
            ) ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to append layer to Image instance. Specified layer is incompatible.";
#endif
        delete imageLayer;

        return nullptr;
    }

    return imageLayer;
}

ImageLayer* emplaceImageLayer(
    Image* destinationImage,
    fxapi::ApiBackendDevice* device,
    const std::string& name,
    libgraphics::fxapi::EPixelFormat::t format,
    size_t width,
    size_t height
) {
    assert( destinationImage );
    assert( device );

    const auto imageLayer = makeImageLayer(
                                device,
                                name,
                                format,
                                width,
                                height
                            );
    assert( imageLayer != nullptr );

    if( !destinationImage->appendLayer(
                imageLayer
            ) ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to append layer to Image instance. Specified layer is incompatible.";
#endif
        delete imageLayer;

        return nullptr;
    }

    return imageLayer;
}

ImageLayer* emplaceImageLayer(
    Image* destinationImage,
    fxapi::ApiBackendDevice* device,
    const std::string& name,
    libgraphics::fxapi::EPixelFormat::t format,
    size_t width,
    size_t height,
    void* data
) {
    assert( destinationImage );
    assert( device );

    const auto imageLayer = makeImageLayer(
                                device,
                                name,
                                format,
                                width,
                                height,
                                data
                            );
    assert( imageLayer != nullptr );

    if( !destinationImage->appendLayer(
                imageLayer
            ) ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to append layer to Image instance. Specified layer is incompatible.";
#endif
        delete imageLayer;

        return nullptr;
    }

    return imageLayer;
}

ImageLayer* emplaceImageLayer(
    Image* destinationImage,
    fxapi::ApiBackendDevice* device,
    const std::string& name,
    const libgraphics::BitmapInfo& info
) {
    assert( destinationImage );
    assert( device );

    const auto imageLayer = makeImageLayer(
                                device,
                                name,
                                info
                            );
    assert( imageLayer != nullptr );

    if( !destinationImage->appendLayer(
                imageLayer
            ) ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to append layer to Image instance. Specified layer is incompatible.";
#endif
        delete imageLayer;

        return nullptr;
    }

    return imageLayer;
}

ImageLayer* emplaceImageLayer(
    Image* destinationImage,
    fxapi::ApiBackendDevice* device,
    const std::string& name,
    libgraphics::Bitmap* bitmap
) {
    assert( destinationImage );
    assert( device );

    const auto imageLayer = makeImageLayer(
                                device,
                                name,
                                bitmap
                            );
    assert( imageLayer != nullptr );

    if( !destinationImage->appendLayer(
                imageLayer
            ) ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to append layer to Image instance. Specified layer is incompatible.";
#endif
        delete imageLayer;

        return nullptr;
    }

    return imageLayer;
}

ImageLayer* emplaceImageLayer(
    Image* destinationImage,
    fxapi::ApiBackendDevice* device,
    const std::string& name,
    libgraphics::Bitmap* bitmap,
    libgraphics::Rect32I area
) {
    assert( destinationImage );
    assert( device );

    const auto imageLayer = makeImageLayer(
                                device,
                                name,
                                bitmap,
                                area
                            );
    assert( imageLayer != nullptr );

    if( !destinationImage->appendLayer(
                imageLayer
            ) ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to append layer to Image instance. Specified layer is incompatible.";
#endif
        delete imageLayer;

        return nullptr;
    }

    return imageLayer;
}

Image* makeImage() {
    const auto imageObject = new Image();
    assert( imageObject != nullptr );

    return imageObject;
}

Image* makeImage(
    fxapi::ApiBackendDevice* device,
    const std::string& name,
    libgraphics::fxapi::EPixelFormat::t format,
    size_t width,
    size_t height
) {
    const auto imageObject = new Image();
    assert( imageObject != nullptr );

    const auto successfullyResetted = imageObject->reset(
                                          device,
                                          format,
                                          width,
                                          height
                                      );
    assert( successfullyResetted );

    if( !successfullyResetted ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to reset image object. Aborting...";
#endif
        delete imageObject;

        return nullptr;
    }

    imageObject->topLayer()->setName(
        name
    );

    return imageObject;
}

Image* makeImage(
    fxapi::ApiBackendDevice* device,
    const std::string& name,
    libgraphics::fxapi::EPixelFormat::t format,
    size_t width,
    size_t height,
    void* data
) {
    assert( data != nullptr );

    const auto imageObject = makeImage(
                                 device,
                                 name,
                                 format,
                                 width,
                                 height
                             );
    assert( imageObject != nullptr );

    if( imageObject != nullptr ) {
        const auto successfullyCopied = imageObject->topLayer()->copy(
                                            data,
                                            imageObject->topLayer()->size(),
                                            0,
                                            0
                                        );
        assert( successfullyCopied );

        if( !successfullyCopied ) {
            delete imageObject;

#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "Failed to create image top layer from specified data blob.";
#endif
            return nullptr;
        }

        return imageObject;
    }

    return nullptr;
}

Image* makeImage(
    fxapi::ApiBackendDevice* device,
    const std::string& name,
    libgraphics::Bitmap* bitmap
) {
    assert( device != nullptr );
    assert( bitmap != nullptr );

    return makeImage(
               device,
               name,
               bitmap,
               libgraphics::Rect32I(
                   ( int )bitmap->width(),
                   ( int )bitmap->height()
               )
           );
}

Image* makeImage(
    fxapi::ApiBackendDevice* device,
    const std::string& name,
    libgraphics::Bitmap* bitmap,
    libgraphics::Rect32I area
) {
    assert( device != nullptr );
    assert( bitmap != nullptr );
    assert( area.width * area.height > 0 );

    const auto compatibleFormat = libgraphics::backend::toCompatibleFormat(
                                      bitmap->format()
                                  );
    assert( compatibleFormat != libgraphics::fxapi::EPixelFormat::Empty );

    const auto imageObject = makeImage(
                                 device,
                                 name,
                                 compatibleFormat,
                                 area.width,
                                 area.height
                             );
    assert( imageObject != nullptr );

    if( imageObject != nullptr ) {
        const auto successfullyCopiedData = imageObject->topLayer()->copy(
                                                bitmap,
                                                area,
                                                0,
                                                0
                                            );
        assert( successfullyCopiedData );

        if( !successfullyCopiedData ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "Failed to copy data from bitmap to ImageLayer instance.";
#endif
            delete imageObject;

            return nullptr;
        }

        return imageObject;
    }

    return nullptr; /** error **/
}

Image* makeImage(
    fxapi::ApiBackendDevice* device,
    const std::string& name,
    const libgraphics::BitmapInfo& info
) {
    assert( device != nullptr );
    assert( info.width() * info.height() > 0 );

    const auto compatibleFormat = libgraphics::backend::toCompatibleFormat(
                                      info.format()
                                  );
    assert( compatibleFormat != libgraphics::fxapi::EPixelFormat::Empty );

    const auto imageObject = makeImage(
                                 device,
                                 name,
                                 compatibleFormat,
                                 info.width(),
                                 info.height()
                             );
    assert( imageObject != nullptr );

    return imageObject;
}

static const std::string defaultLayerName = "default";

Image::Image() : d( new Private() ) {}

Image::Image( fxapi::ApiBackendDevice* device, fxapi::EPixelFormat::t format, size_t width, size_t height ) : d( new Private() ) {
    d->width = width;
    d->height = height;
    d->format = format;

    const auto layer = createAndAppendLayer(
                           device,
                           defaultLayerName,
                           width,
                           height
                       );
    assert( layer );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !layer ) {
        qDebug() << "Image::Image(): Failed to create default image layer.";
    }

#endif
}

Image::Image( fxapi::ApiBackendDevice* device, fxapi::EPixelFormat::t format, size_t width, size_t height, void* data ) : d( new Private() ) {
    d->width = width;
    d->height = height;
    d->format = format;

    const auto layer = createAndAppendLayer(
                           device,
                           defaultLayerName,
                           width,
                           height,
                           data
                       );
    assert( layer );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !layer ) {
        qDebug() << "Image::Image(): Failed to create default image layer.";
    }

#endif
}

Image::Image( const Image& rhs ) : d( new Private() ) {
    d->layers = rhs.d->layers;
    d->format = rhs.d->format;
    d->width = rhs.d->width;
    d->height = rhs.d->height;
}

Image::Image( Image&& rhs ) : d( new Private() )  {
    std::swap(
        rhs.d,
        d
    );
}

Image& Image::operator = ( const Image& rhs ) {
    d->layers = rhs.d->layers;
    d->format = rhs.d->format;
    d->width = rhs.d->width;
    d->height = rhs.d->height;

    return *this;
}

Image::~Image() {
    d.reset();
}

size_t      Image::width() const {
    return this->d->width;
}

size_t      Image::height() const {
    return this->d->height;
}

fxapi::EPixelFormat::t  Image::format() const {
    return this->d->format;
}

ImageMetaInfo*  Image::metaInfo() {
    return &d->info;
}

const ImageMetaInfo*  Image::metaInfo() const {
    return &d->info;
}

ImageLayer* Image::createAndAppendLayer(
    fxapi::ApiBackendDevice* device,
    const std::string& name
) {
    ImageLayer* imageLayer = new ImageLayer(
        device,
        name
    );
    assert( imageLayer );

    const auto successfullyInitialized = imageLayer->reset(
            d->format,
            d->width,
            d->height
                                         );
    assert( successfullyInitialized );

    if( !successfullyInitialized ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Image::createAndAppendLayer(): Failed to create image layer.";
#endif
        delete imageLayer;

        return nullptr;
    }

    this->d->layers.push_back(
        std::shared_ptr<ImageLayer>( imageLayer )
    );
    /*
    this->d->layers.insert(
        this->d->layers.begin(),
        std::shared_ptr<ImageLayer>(imageLayer)
    );*/

    return imageLayer;
}

ImageLayer* Image::createAndAppendLayer(
    fxapi::ApiBackendDevice* device,
    const std::string& name,
    size_t width,
    size_t height,
    void* data
) {
    assert( width > 0 );
    assert( height > 0 );
    assert( data );

    ImageLayer* imageLayer = new ImageLayer(
        device,
        name
    );
    assert( imageLayer );

    const auto successfullyInitialized = imageLayer->reset(
            d->format,
            width,
            height,
            data
                                         );
    assert( successfullyInitialized );

    if( !successfullyInitialized ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Image::createAndAppendLayer(): Failed to create image layer.";
#endif
        delete imageLayer;

        return nullptr;
    }

    this->d->layers.push_back(
        std::shared_ptr<ImageLayer>( imageLayer )
    );
    /*
    this->d->layers.insert(
        this->d->layers.begin(),
        std::shared_ptr<ImageLayer>(imageLayer)
    );*/

    return imageLayer;
}

ImageLayer* Image::createAndAppendLayer(
    fxapi::ApiBackendDevice* device,
    const std::string& name,
    size_t width,
    size_t height
) {
    assert( width > 0 );
    assert( height > 0 );

    ImageLayer* imageLayer = new ImageLayer(
        device,
        name
    );
    assert( imageLayer );

    const auto successfullyInitialized = imageLayer->reset(
            d->format,
            width,
            height
                                         );
    assert( successfullyInitialized );

    if( !successfullyInitialized ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Image::createAndAppendLayer(): Failed to create image layer.";
#endif
        delete imageLayer;

        return nullptr;
    }

    this->d->layers.insert(
        this->d->layers.begin(),
        std::shared_ptr<ImageLayer>( imageLayer )
    );

    return imageLayer;
}


bool Image::appendLayer(
    ImageLayer* layer
) {
    assert( layer );

    if( d->format == layer->format() ) {
        this->d->layers.push_back( std::shared_ptr<ImageLayer>( layer ) );

        return true;
    }

    return false;
}

bool Image::appendLayerWithCompatibleDevice(
    ImageLayer* layer
) {
    assert( layer );

    if( d->layers.empty() ) {
        appendLayer( layer );
        return true;
    }

    bool foundDevice( false );

    for( auto it = d->layers.begin(); it != d->layers.end(); ++it ) {
        if( ( ( *it )->internalDeviceForBackend( FXAPI_BACKEND_CPU ) == layer->internalDeviceForBackend( FXAPI_BACKEND_CPU ) ) ||
                ( ( *it )->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ) == layer->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ) ) ) {
            foundDevice = true;
            break;
        }
    }

    if( foundDevice ) {
        appendLayer( layer );

        return true;
    }

    return false;
}

bool Image::removeLayer(
    ImageLayer* layer
) {
    assert( layer );

    for( auto it = d->layers.begin(); it != d->layers.end(); ++it ) {
        if( ( *it ).get() == layer ) {
            d->layers.erase( it );
            return true;
        }
    }

    return false;
}

bool Image::removeLayerByName(
    const std::string& name
) {
    for( auto it = d->layers.begin(); it != d->layers.end(); ++it ) {
        if( ( *it )->name() == name ) {
            d->layers.erase( it );
            return true;
        }
    }

    return false;
}

bool Image::removeLayerByIndex(
    size_t index
) {
    assert( d->layers.size() > index );

    if( d->layers.size() > index ) {
        d->layers.erase( d->layers.begin() + index );
        return true;
    }

    return false;
}

void Image::clearLayers() {
    d->layers.clear();
}

bool Image::updateImageLayerDataForBackend(
    libgraphics::fxapi::ApiBackendDevice* device,
    int backendId
) {
    assert( device );

    bool updated( true );

    for( auto it = d->layers.begin(); it != d->layers.end(); ++it ) {
        ( *it )->updateDataForBackend( device, backendId );

        if( !( *it )->containsDataForBackend( backendId ) ) {
            updated = false;
            break;
        }
    }

    return updated;
}

size_t  Image::layerCount() const {
    return d->layers.size();
}

ImageLayer* Image::layerByName( const std::string& name ) const {
    for( auto it = d->layers.begin(); it != d->layers.end(); ++it ) {
        if( ( *it )->name() == name ) {
            return ( ImageLayer* )( *it ).get();
        }
    }

    return nullptr;
}

ImageLayer* Image::layerByIndex( size_t index ) {
    assert( d->layers.size() > index );

    if( d->layers.size() > index ) {
        return ( *( d->layers.begin() + index ) ).get();
    }

    return nullptr;
}

bool Image::empty() const {
    if( !d ) { return true; }

    return d->layers.empty();
}

size_t Image::indexForLayer( ImageLayer* layer ) {
    assert( !empty() );

    size_t distance( 0 );

    for( auto it = d->layers.begin(); it != d->layers.end(); ++it ) {
        if( ( *it ).get() == layer ) {
            return distance;
        }

        ++distance;
    }

    return Image::npos;
}

bool Image::moveLayerUp( ImageLayer* layer ) {
    size_t index = indexForLayer( layer );

    assert( index == Image::npos );

    if( index != Image::npos ) {
        if( index == 0 ) {
            return false; /** layer already at the top **/
        }

        assert( layerByIndex( index - 1 ) ); // check upper layer

        ( *( d->layers.begin() + index ) ).swap(
            *( d->layers.begin() + ( index - 1 ) )
        );

        return true;
    }

    return false;
}

bool Image::moveLayerDown( ImageLayer* layer ) {
    size_t index = indexForLayer(
                       layer
                   );

    assert( index == Image::npos );

    if( index != Image::npos ) {
        if( index == ( d->layers.size() - 1 ) ) {
            return false; /** layer already at the top **/
        }

        assert( layerByIndex( index + 1 ) ); // check lower layer

        ( *( d->layers.begin() + index ) ).swap(
            *( d->layers.begin() + ( index + 1 ) )
        );

        return true;
    }

    return false;
}

bool Image::moveLayer( ImageLayer* layer, int steps ) {
    assert( layer );

    if( steps == 0 ) {
        return true;
    }

    size_t currentIndex = indexForLayer(
                              layer
                          );

    if( currentIndex != Image::npos ) {
        if( steps > 0 ) {
            if( currentIndex + steps > ( d->layers.size() - 1 ) ) {
                return false;
            }

            while( steps != 0 ) {
                const auto successfullyMoved = moveLayerUp(
                                                   layer
                                               );
                assert( successfullyMoved );

                if( !successfullyMoved ) { break; }

                --steps;
            }

            return ( steps == 0 );
        } else {
            if( ( ( size_t ) abs( steps ) > currentIndex ) ) {
                return false;
            }

            while( steps != 0 ) {
                const auto successfullyMoved = moveLayerDown(
                                                   layer
                                               );
                assert( successfullyMoved );

                if( !successfullyMoved ) { break; }

                ++steps;
            }

            return ( steps == 0 );
        }
    }

    return false;
}

bool Image::switchLayers( ImageLayer* first, ImageLayer* second ) {
    assert( first );
    assert( second );

    size_t firstIndex = indexForLayer(
                            first
                        );
    size_t secondIndex = indexForLayer(
                             second
                         );

    return switchLayers(
               firstIndex,
               secondIndex
           );
}

bool Image::switchLayers( size_t firstIndex, size_t secondIndex ) {
    assert( firstIndex != Image::npos );
    assert( secondIndex != Image::npos );

    if( ( firstIndex != Image::npos ) && ( secondIndex != Image::npos ) ) {
        ( *( d->layers.begin() + firstIndex ) ).swap(
            *( d->layers.begin() + secondIndex )
        );

        return true;
    }

    return false;
}

ImageLayerGroup Image::layers() const {
    ImageLayerGroup grp;

    for( auto it = d->layers.begin(); it != d->layers.end(); ++it ) {
        grp.append( *it );
    }

    return grp;
}

ImageLayerGroup Image::layersWithName( const std::string& name ) const {
    ImageLayerGroup grp;

    for( auto it = d->layers.begin(); it != d->layers.end(); ++it ) {
        if( ( *it )->name() == name ) {
            grp.append( *it );
        }
    }

    return grp;
}

ImageLayerGroup Image::layersWithSize( int width, int height ) const {
    ImageLayerGroup grp;

    for( auto it = d->layers.begin(); it != d->layers.end(); ++it ) {
        if( ( ( *it )->width() == width ) && ( ( *it )->height() == height ) ) {
            grp.append( *it );
        }
    }

    return grp;
}

ImageLayerGroup Image::layersWithFormat( fxapi::EPixelFormat::t format ) const {
    ImageLayerGroup grp;

    for( auto it = d->layers.begin(); it != d->layers.end(); ++it ) {
        if( ( *it )->format() == format ) {
            grp.append( *it );
        }
    }

    return grp;
}

ImageLayerGroup Image::layersWithConfig( fxapi::EPixelFormat::t format, int width, int height ) const {
    ImageLayerGroup grp;

    for( auto it = d->layers.begin(); it != d->layers.end(); ++it ) {
        if( ( ( *it )->width() == width ) && ( ( *it )->height() == height ) && ( ( *it )->format() == format ) ) {
            grp.append( *it );
        }
    }

    return grp;
}

ImageLayerGroup Image::layersWithBackend( int backend ) const {
    ImageLayerGroup grp;

    for( auto it = d->layers.begin(); it != d->layers.end(); ++it ) {
        if( ( ( *it )->containsDataForBackend( backend ) ) ) {
            grp.append( *it );
        }
    }

    return grp;
}

bool Image::containsImageLayer( ImageLayer* layer ) const {
    for( auto it = d->layers.begin(); it != d->layers.end(); ++it ) {
        if( ( *it ).get() == layer ) {
            return true;
        }
    }

    return false;
}

bool Image::containsImageLayerWithName( const std::string& name ) const {
    for( auto it = d->layers.begin(); it != d->layers.end(); ++it ) {
        if( ( *it )->name() == name ) {
            return ( *it ).get();
        }
    }

    return false;
}

ImageLayer* Image::topLayer() const {
    assert( !empty() );
    return ( ImageLayer* )d->layers.front().get();
}

ImageLayer* Image::bottomLayer() const {
    assert( !empty() );
    return ( ImageLayer* )d->layers.back().get();
}

ImageLayer* Image::cloneTopLayer() {
    ImageLayer* currentTopLayer = topLayer();
    assert( currentTopLayer );

    if( !currentTopLayer ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Image::cloneTopLayer(): Failed to clone top layer. No valid layer found.";
#endif
        return nullptr;
    }

    ImageLayer* clonedLayer = currentTopLayer->duplicate();
    assert( clonedLayer );

    if( !clonedLayer ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Image::cloneTopLayer(): Failed to clone top layer. ";
#endif
        return nullptr;
    }

    this->appendLayer(
        clonedLayer
    );

    return clonedLayer;
}

ImageLayer* Image::cloneBottomLayer() {
    ImageLayer* currentBottomLayer = bottomLayer();
    assert( currentBottomLayer );

    if( !currentBottomLayer ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Image::cloneBottomLayer(): Failed to clone bottom layer. No valid layer found.";
#endif
        return nullptr;
    }

    ImageLayer* clonedLayer = currentBottomLayer->duplicate();
    assert( clonedLayer );

    if( !clonedLayer ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Image::cloneBottomLayer(): Failed to clone bottom layer. ";
#endif
        return nullptr;
    }

    this->appendLayer(
        clonedLayer
    );

    return clonedLayer;
}


void Image::reset() {
    d.reset( new Private() );
}

bool Image::reset( fxapi::ApiBackendDevice* device, fxapi::EPixelFormat::t format, size_t width, size_t height ) {
    assert( width * height > 0 );
    assert( format != libgraphics::fxapi::EPixelFormat::Empty );
    assert( device != nullptr );

    reset();

    this->d->format = format;
    this->d->width = width;
    this->d->height = height;

    libgraphics::ImageLayer* topLayerObject = new libgraphics::ImageLayer(
        device
    );
    const auto successfullyInitializedImageLayer = topLayerObject->reset(
                format,
                width,
                height
            );
    assert( successfullyInitializedImageLayer );

    if( !successfullyInitializedImageLayer ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to initialize new top layer.";
#endif
        return false;
    }

    const auto successfullyAppendedImageLayer = this->appendLayer(
                topLayerObject
            );
    assert( successfullyAppendedImageLayer );

    if( !successfullyAppendedImageLayer ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to append image layer.";
#endif
        return false;
    }

    return true;
}

bool Image::reset( fxapi::ApiBackendDevice* device, fxapi::EPixelFormat::t format, size_t width, size_t height, void* data ) {
    if( reset( device, format, width, height ) ) {
        return this->topLayer()->copy(
                   data,
                   this->topLayer()->size(),
                   0,
                   0
               );
    }

    return false;
}

bool Image::reset( fxapi::ApiBackendDevice* device, libgraphics::Bitmap* source ) {
    assert( device != nullptr );
    assert( source != nullptr );

    return reset(
               device,
               source,
               libgraphics::Rect32I(
                   ( int )source->width(),
                   ( int )source->height()
               )
           );
}

bool Image::reset( fxapi::ApiBackendDevice* device, libgraphics::Bitmap* source, libgraphics::Rect32I sourceArea ) {
    assert( sourceArea.width * sourceArea.height > 0 );
    assert( device != nullptr );
    assert( source != nullptr );

    reset();

    this->d->format = libgraphics::backend::toCompatibleFormat( source->format() );
    this->d->width = ( int )sourceArea.width;
    this->d->height = ( int )sourceArea.height;

    libgraphics::ImageLayer* topLayerObject = new libgraphics::ImageLayer(
        device
    );
    const auto successfullyInitializedImageLayer = topLayerObject->reset(
                source,
                sourceArea
            );
    assert( successfullyInitializedImageLayer );

    if( !successfullyInitializedImageLayer ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to initialize new top layer.";
#endif
        return false;
    }

    const auto successfullyAppendedImageLayer = this->appendLayer(
                topLayerObject
            );
    assert( successfullyAppendedImageLayer );

    if( !successfullyAppendedImageLayer ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to append image layer.";
#endif
        return false;
    }

    return true;
}

bool Image::reset( fxapi::ApiBackendDevice* device, const libgraphics::BitmapInfo& info ) {
    assert( info.width() * info.height() > 0 );
    assert( device != nullptr );

    reset();

    this->d->format = libgraphics::backend::toCompatibleFormat( info.format() );
    this->d->width = ( int )info.width();
    this->d->height = ( int )info.height();

    libgraphics::ImageLayer* topLayerObject = new libgraphics::ImageLayer(
        device
    );
    const auto successfullyInitializedImageLayer = topLayerObject->reset(
                info
            );
    assert( successfullyInitializedImageLayer );

    if( !successfullyInitializedImageLayer ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to initialize new top layer.";
#endif
        return false;
    }

    const auto successfullyAppendedImageLayer = this->appendLayer(
                topLayerObject
            );
    assert( successfullyAppendedImageLayer );

    if( !successfullyAppendedImageLayer ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to append image layer.";
#endif
        return false;
    }

    return true;
}

size_t Image::queryMemoryConsumptionForBackend( int backendId ) const {
    ( void )backendId;
    return 0;
}

size_t Image::queryMemoryConsumption() const {
    return 0;
}


/// ImageLayerGroup
ImageLayerGroup::ImageLayerGroup() : d( new Private() ) {}

ImageLayerGroup::ImageLayerGroup( const ImageLayerGroup& rhs ) : d( new Private() ) {
    for( auto it = rhs.d->layers.begin(); it != rhs.d->layers.end(); ++it ) {
        d->layers.push_back( ( *it ) );
    }
}

ImageLayerGroup& ImageLayerGroup::operator = ( const ImageLayerGroup& rhs ) {
    for( auto it = rhs.d->layers.begin(); it != rhs.d->layers.end(); ++it ) {
        d->layers.push_back( ( *it ) );
    }

    return *this;
}

void ImageLayerGroup::append(
    ImageLayer* layer
) {
    d->layers.push_back(
        std::shared_ptr<ImageLayer>( layer )
    );
}

void ImageLayerGroup::append(
    std::shared_ptr<ImageLayer> layer
) {
    d->layers.push_back(
        layer
    );
}

bool ImageLayerGroup::remove(
    ImageLayer* layer
) {
    for( auto it = d->layers.begin(); it != d->layers.end(); ++it ) {
        if( ( *it ).get() == layer ) {
            d->layers.erase( it );
            return true;
        }
    }

    return false;
}

bool ImageLayerGroup::removeByName(
    const std::string& name
) {
    for( auto it = d->layers.begin(); it != d->layers.end(); ++it ) {
        if( ( *it )->name() == name ) {
            d->layers.erase( it );
            return true;
        }
    }

    return false;
}

bool ImageLayerGroup::removeByIndex(
    size_t index
) {
    assert( d->layers.size() > index );

    if( d->layers.size() < index ) {
        return false;
    }

    ( void ) d->layers.erase(
        d->layers.begin() + index
    );

    return true;
}

void ImageLayerGroup::reset() {
    d->layers.clear();
}

bool ImageLayerGroup::empty() const {
    return d->layers.empty();
}

size_t ImageLayerGroup::size() const {
    return d->layers.size();
}

ImageLayerGroup ImageLayerGroup::layersWithName( const std::string& name ) const {
    ImageLayerGroup grp;

    for( auto it = d->layers.begin(); it != d->layers.end(); ++it ) {
        if( ( *it )->name() == name ) {
            grp.append( ( *it ) );
        }
    }

    return grp;
}

ImageLayerGroup ImageLayerGroup::layersWithSize( int width, int height ) const {
    ImageLayerGroup grp;

    for( auto it = d->layers.begin(); it != d->layers.end(); ++it ) {
        if( ( ( *it )->width() == width ) && ( ( *it )->height() == height ) ) {
            grp.append( ( *it ) );
        }
    }

    return grp;
}

ImageLayerGroup ImageLayerGroup::layersWithFormat( fxapi::EPixelFormat::t format ) const {
    ImageLayerGroup grp;

    for( auto it = d->layers.begin(); it != d->layers.end(); ++it ) {
        if( ( ( *it )->format() == format ) ) {
            grp.append( ( *it ) );
        }
    }

    return grp;
}

ImageLayerGroup ImageLayerGroup::layersWithConfig( fxapi::EPixelFormat::t format, int width, int height ) const {
    ImageLayerGroup grp;

    for( auto it = d->layers.begin(); it != d->layers.end(); ++it ) {
        if( ( ( *it )->width() == width ) && ( ( *it )->height() == height ) && ( ( *it )->format() == format ) ) {
            grp.append( ( *it ) );
        }
    }

    return grp;
}

ImageLayerGroup ImageLayerGroup::layersWithBackend( int backend ) const {
    ImageLayerGroup grp;

    for( auto it = d->layers.begin(); it != d->layers.end(); ++it ) {
        if( ( ( *it )->containsDataForBackend( backend ) ) ) {
            grp.append( ( *it ) );
        }
    }

    return grp;
}

bool ImageLayerGroup::contains( ImageLayer* layer ) const {
    for( auto it = d->layers.begin(); it != d->layers.end(); ++it ) {
        if( ( *it ).get() == layer ) {
            return true;
        }
    }

    return false;
}

bool ImageLayerGroup::contains( const std::string& name ) const {
    for( auto it = d->layers.begin(); it != d->layers.end(); ++it ) {
        if( ( *it )->name() == name ) {
            return true;
        }
    }

    return false;
}

bool ImageLayerGroup::updateImageLayerDataForBackend(
    libgraphics::fxapi::ApiBackendDevice* device,
    int backendId
) {
    assert( device );

    for( auto it = d->layers.begin(); it != d->layers.end(); ++it ) {
        ( *it )->updateDataForBackend(
            device,
            backendId
        );
    }

    return true;
}

ImageLayerGroup::Iterator   ImageLayerGroup::begin() {
    return d->layers.begin();
}

ImageLayerGroup::ConstIterator ImageLayerGroup::begin() const {
    return d->layers.begin();
}

ImageLayerGroup::ConstIterator ImageLayerGroup::constBegin() const {
    return d->layers.cbegin();
}

ImageLayerGroup::Iterator ImageLayerGroup::end() {
    return d->layers.end();
}

ImageLayerGroup::ConstIterator ImageLayerGroup::end() const {
    return d->layers.end();
}

ImageLayerGroup::ConstIterator ImageLayerGroup::constEnd() const {
    return d->layers.cend();
}

std::shared_ptr<ImageLayer>    ImageLayerGroup::front() const {
    return d->layers.front();
}

std::shared_ptr<ImageLayer>    ImageLayerGroup::back() const {
    return d->layers.back();
}

std::shared_ptr<ImageLayer>    ImageLayerGroup::at( size_t index ) const {
    return *( d->layers.begin() + index );
}

}
