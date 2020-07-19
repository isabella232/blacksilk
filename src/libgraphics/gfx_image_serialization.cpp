#include <libgraphics/image.hpp>
#include <libgraphics/image_p.hpp>
#include <libserialization++.hpp>
#include <log/log.hpp>
#include <QDebug>

#include <iostream>

namespace libgraphics {
struct MetaFormat : spp::AutoSerializable {
    static const int magic = 1178880333; // == 'FDIM' FD Imaging - Meta Format

    struct Header : spp::AutoSerializable {
        Header() : format( libgraphics::fxapi::EPixelFormat::Empty ),
            width( 0 ), height( 0 ), numberOfLayers( 0 ), totalSize( 0 ) {}

        inline bool operator == ( const Header& rhs ) const {
            return ( format == rhs.format ) && ( width == rhs.width ) &&
                   ( height == rhs.height ) && ( numberOfLayers == rhs.numberOfLayers ) && ( totalSize == rhs.totalSize );
        }

        virtual ~Header() {}
        virtual spp::PropertyCollection GetProperties() {
            spp::PropertyCollection collection( "header", this );

            bool ret( true );
            ret = collection.Add(
                      spp::MakeValueProperty(
                          this,
                          "format",
                          &format
                      )
                  );
            LOGB_ASSERT( ret, "format not added to collection" );

            ret = collection.Add(
                      spp::MakeValueProperty(
                          this,
                          "width",
                          &width
                      )
                  );
            LOGB_ASSERT( ret, "width not added to collection" );

            ret = collection.Add(
                      spp::MakeValueProperty(
                          this,
                          "height",
                          &height
                      )
                  );
            LOGB_ASSERT( ret, "height not added to collection" );

            ret = collection.Add(
                      spp::MakeValueProperty(
                          this,
                          "numberOfLayers",
                          &numberOfLayers
                      )
                  );
            LOGB_ASSERT( ret, "numberOfLayers not added to collection" );

            ret = collection.Add(
                      spp::MakeValueProperty(
                          this,
                          "totalSize",
                          &totalSize
                      )
                  );
            LOGB_ASSERT( ret, "totalSize not added to collection" );

            return collection;
        }

        libgraphics::fxapi::EPixelFormat::t format;
        size_t  width;
        size_t  height;
        size_t  numberOfLayers;
        size_t  totalSize;
    };
    struct MetaData : spp::AutoSerializable {
        struct Tag : spp::AutoSerializable {
            Tag() {}
            explicit Tag( const ImageMetaInfoTag& tag ) : name( tag.name() ), data( tag.data() ) {}
            virtual ~Tag() {}

            virtual spp::PropertyCollection GetProperties() {
                spp::PropertyCollection collection( "header", this );

                bool ret( true );
                ret = collection.Add(
                          spp::MakeValueProperty(
                              this,
                              "name",
                              &name
                          )
                      );
                LOGB_ASSERT( ret, "name not added to collection" );

                ret = collection.Add(
                          spp::MakeValueProperty(
                              this,
                              "data",
                              &data
                          )
                      );
                LOGB_ASSERT( ret, "data not added to collection" );

                return collection;
            }

            inline bool operator == ( const Tag& rhs ) const {
                return ( name == rhs.name ) && ( data == rhs.data );
            }

            std::string name;
            std::string data;
        };
        struct Directory : spp::AutoSerializable {
            explicit Directory( std::string _name = "" ) : name( _name ) {}
            virtual ~Directory() {}

            virtual spp::PropertyCollection GetProperties() {
                spp::PropertyCollection collection( name, this );

                bool ret( false );
                ret = collection.Add(
                          spp::MakeValueProperty(
                              this,
                              "name",
                              &name
                          )
                      );
                LOGB_ASSERT( ret, "name not added to collection" );

                ret = collection.Add(
                          spp::MakeCollectionProperty(
                              this,
                              "tags",
                              &tags
                          )
                      );
                LOGB_ASSERT( ret, "tags not added to collection" );

                return collection;
            }

            inline bool operator == ( const Directory& rhs ) const {
                return ( name == rhs.name ) && ( tags == rhs.tags );
            }

            void emplaceTag( const ImageMetaInfoTag& tag ) {
                tags.Append(
                    Tag( tag )
                );
            }

            std::string name;
            spp::Vector<Tag>  tags;
        };

        MetaData() {}
        virtual ~MetaData() {}

        inline bool operator == ( const MetaData& rhs ) const {
            return ( directories == rhs.directories );
        }

        void fromImageMetaInfo( libgraphics::ImageMetaInfo* info ) {
            assert( info );

            directories.Clear();

            for( size_t i = 0; info->countDirectories() > i; ++i ) {
                const auto directory = info->directoryByIndex(
                                           i
                                       );

                Directory serialDir;

                for( auto it = directory.begin(); it != directory.end(); ++it ) {
                    serialDir.emplaceTag(
                        ( *it )
                    );
                }

                serialDir.name = directory.name();
            }
        }

        void toImageMetaInfo( libgraphics::ImageMetaInfo* info ) {
            info->clearDirectories();

            for( auto it = directories.Begin(); it != directories.End(); ++it ) {
                const auto dir = ( *it );

                auto metaDirectory = info->addDirectory(
                                         dir.name
                                     );

                for( auto st = dir.tags.Begin(); st != dir.tags.End(); ++st ) {
                    metaDirectory.add(
                        ( *st ).name,
                        ( *st ).data
                    );
                }
            }
        }

        virtual spp::PropertyCollection GetProperties() {
            spp::PropertyCollection collection( "meta-info", this );

            const auto ret = collection.Add(
                                 spp::MakeCollectionProperty(
                                     this,
                                     "directories",
                                     &directories
                                 )
                             );
            LOGB_ASSERT( ret, "directories not added to collection" );

            return collection;
        }
        spp::Vector<Directory>  directories;
    };
    struct LayerData : spp::AutoSerializable {
        std::string         binaryFormat;
        size_t              width;
        size_t              height;
        size_t              offset;
        size_t              byteSize;
        std::string name;

        LayerData() : binaryFormat( "none" ), width( 0 ), height( 0 ),
            offset( 0 ), byteSize( 0 ) {}
        virtual ~LayerData() {}

        inline bool operator == ( const LayerData& rhs ) const {
            return ( rhs.binaryFormat == binaryFormat ) &&
                   ( rhs.width == width ) &&
                   ( rhs.height == height ) &&
                   ( rhs.offset == offset ) &&
                   ( rhs.byteSize == byteSize ) &&
                   ( rhs.name == name );
        }

        virtual spp::PropertyCollection GetProperties() {
            spp::PropertyCollection collection( "layer-" + name, this );

            bool ret = collection.Add(
                           spp::MakeValueProperty(
                               this,
                               "binaryFormat",
                               &binaryFormat
                           )
                       );
            LOGB_ASSERT( ret, "binaryFormat not added to collection" );

            ret = collection.Add(
                      spp::MakeValueProperty(
                          this,
                          "width",
                          &width
                      )
                  );
            LOGB_ASSERT( ret, "width not added to collection" );

            ret = collection.Add(
                      spp::MakeValueProperty(
                          this,
                          "height",
                          &height
                      )
                  );
            LOGB_ASSERT( ret, "height not added to collection" );

            ret = collection.Add(
                      spp::MakeValueProperty(
                          this,
                          "offset",
                          &offset
                      )
                  );
            LOGB_ASSERT( ret, "offset not added to collection" );

            ret = collection.Add(
                      spp::MakeValueProperty(
                          this,
                          "byteSize",
                          &byteSize
                      )
                  );
            LOGB_ASSERT( ret, "byteSize not added to collection" );

            ret = collection.Add(
                      spp::MakeValueProperty(
                          this,
                          "name",
                          &name
                      )
                  );
            LOGB_ASSERT( ret, "name not added to collection" );

            return collection;
        }
    };

    Header  header;
    MetaData meta;
    spp::Vector<LayerData>  layers;

    MetaFormat() {}
    virtual ~MetaFormat() {}

    virtual spp::PropertyCollection GetProperties() {
        spp::PropertyCollection collection( "header", this );

        bool ret = collection.Add(
                       spp::MakeValueProperty(
                           this,
                           "header",
                           &header
                       )
                   );
        LOGB_ASSERT( ret, "header not added to collection" );

        ret = collection.Add(
                  spp::MakeValueProperty(
                      this,
                      "meta",
                      &meta
                  )
              );
        LOGB_ASSERT( ret, "meta not added to collection" );

        ret = collection.Add(
                  spp::MakeCollectionProperty(
                      this,
                      "layers",
                      &layers
                  )
              );
        LOGB_ASSERT( ret, "layers not added to collection" );

        return collection;
    }
};

/**
    .fdm - fdi meta format:
        * (signed 4 bytes)magic field
        * (unsigned 4 bytes)meta format length
        * (n bytes) meta format headers encoded in json
        * (n bytes) layer data
*/

void metaFormatInstanceFromImage( MetaFormat* data, Image* image ) {
    assert( data );
    assert( image );

    data->header.format = image->format();
    data->header.height = image->height();
    data->header.width = image->width();
    data->header.numberOfLayers = image->layerCount();

    for( size_t i = 0; data->header.numberOfLayers > i; ++i ) {
        MetaFormat::LayerData layerData;

        const auto currentLayer = image->layerByIndex( i );
        assert( currentLayer );

        if( !currentLayer ) {
            continue;
        }

        layerData.width         = currentLayer->width();
        layerData.height        = currentLayer->height();
        layerData.offset        = 0;
        layerData.byteSize      = currentLayer->byteSize();

        data->header.totalSize += layerData.byteSize;

        data->layers.Append( layerData );
    }

    data->meta.fromImageMetaInfo(
        image->metaInfo()
    );
}

/// serialization
size_t Image::readFromFile(
    libgraphics::fxapi::ApiBackendDevice* defaultDevice,
    const std::string& path
) {
    assert( defaultDevice );

    spp::FileStream fs( path, spp::FileOpenMode::AlwaysOpen, spp::FileStreamMode::Readable );

    assert( fs.Length() == 0 );

    if( fs.Length() == 0 ) {
        return 0;
    }

    void* temporaryBuffer = ( void* ) new char[fs.Length()];
    assert( temporaryBuffer );

    const size_t readBytes = fs.Read(
                                 &temporaryBuffer,
                                 fs.Length()
                             );
    assert( readBytes > 0 );

    if( readBytes > 0 ) {
        const size_t readBytes = this->readFromData(
                                     defaultDevice,
                                     temporaryBuffer,
                                     fs.Length()
                                 );

        fs.Close();

        delete []( char* )temporaryBuffer;

        return readBytes;
    }

    delete []( char* )temporaryBuffer;
    return 0;
}

size_t Image::readFromData(
    libgraphics::fxapi::ApiBackendDevice* defaultDevice,
    void* data,
    size_t size
) {
    assert( defaultDevice );
    assert( data );
    assert( size > 0 );

    if( !data || !size ) {
        return 0;
    }

    const bool hasValidMagicField = ( *( ( int* )data ) == MetaFormat::magic );

    if( !hasValidMagicField ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Image::readFromData(): Failed to read from data chunk. Invalid magic field.";
#endif
        return 0;
    }

    const size_t metaFormatLength = *( size_t* )( *( char* )data + sizeof( int ) );
    assert( metaFormatLength > 0 );

    if( metaFormatLength == 0 ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Image::readFromData(): metaFormatLength is zero. ";
#endif
        return 0;
    }

    assert( size > sizeof( int ) + metaFormatLength );

    if( size < sizeof( int ) + metaFormatLength ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Image::readFromData(): FDM file corrupted. metaFormatLength is bigger than the complete buffer.";
#endif
        return 0;
    }

    char* metaFormatPtr = ( ( char* )data + sizeof( int ) + sizeof( size_t ) ); // skip magic bytes

    MetaFormat metaFormat;
    {
        spp::MemoryStream memoryStream;
        spp::formatters::json::Formatter jsonFormatter;

        memoryStream.Attach(
            ( void* )metaFormatPtr,
            spp::MemoryStreamMode::Readable,
            metaFormatLength
        );
        assert( memoryStream.Length() > 0 );

        jsonFormatter.Reset( &memoryStream );

        const bool result = jsonFormatter.Deserialize(
                                metaFormat.GetProperties()
                            );
        assert( result );

        if( !result ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "Image::readFromData(): Failed to deserialize MetaFormat json data.";
#endif
            return 0;
        }
    }

    if( ( metaFormat.header.width == 0 ) || ( metaFormat.header.height == 0 ) ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Image::readFromFile(): FDM file invalid. Header describes image of size zero.";
#endif
        return 0;
    }

    if( ( metaFormat.header.format == fxapi::EPixelFormat::Empty ) ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Image::readFromFile(): FDM file invalid. Header describes image of invalid format.";
#endif
        return 0;
    }

    this->reset();

    this->d->width      = metaFormat.header.width;
    this->d->height     = metaFormat.header.height;
    this->d->format     = metaFormat.header.format;

    this->d->info.clearDirectories();

    metaFormat.meta.toImageMetaInfo(
        &this->d->info
    );

    for( auto it = metaFormat.layers.Begin(); it != metaFormat.layers.End(); ++it ) {
        void* ptr = ( ( char* )data + ( *it ).offset );

        assert( ptr );

        if( ( *it ).offset + ( *it ).byteSize > size ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "Image::readFromFile(): FDM file invalid. Layer structure points to invalid memory region.";
#endif
            continue;
        }

        const auto layer = this->createAndAppendLayer(
                               defaultDevice,
                               ( *it ).name,
                               ( *it ).width,
                               ( *it ).height,
                               ptr
                           );
        assert( layer );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

        if( !layer ) {
            qDebug() << "Image::readFromFile(): Failed to create layer from file layer data.";
        }

#endif
    }

    return size;
}

size_t Image::writeToFile(
    const std::string& path
) {
    size_t totalSize( 0 );

    for( auto it = d->layers.begin(); it != d->layers.end(); ++it ) {
        totalSize += ( *it )->byteSize() + ( sizeof( MetaFormat::LayerData ) * 3 );
    }

    for( size_t i = 0; d->info.countDirectories() > i; ++i ) {
        const ImageMetaInfoDirectory& dir = d->info.directoryByIndex( i );

        totalSize += dir.name().length() * 2;

        for( auto it = dir.constBegin(); it != dir.constEnd(); ++it ) {
            totalSize += ( *it ).name().length() * 2;
            totalSize += ( *it ).length();
        }
    }

    totalSize += 1024;
    assert( totalSize > 0 );

    spp::MemoryStream memoryStream( new char[totalSize], spp::MemoryStreamMode::Buffer );

    if( memoryStream.Length() == 0 ) {
        return 0;
    }

    const auto bytesWritten = writeToData(
                                  memoryStream.GetPointer(),
                                  totalSize
                              );
    assert( bytesWritten > 0 );

    if( bytesWritten == 0 ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Image::writeToFile(): Failed to write image to path " << path.c_str();
#endif
        return 0;
    }

    spp::FileStream fs( path, spp::FileOpenMode::BinaryAlwaysCreate, spp::FileStreamMode::ReadWritable );

    if( bytesWritten != fs.Write( memoryStream.GetPointer(), bytesWritten ) ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Image::writeToFile(): Failed to write image to path " << path.c_str();
#endif
        return 0;
    }

    fs.Flush();

    return bytesWritten;
}

size_t Image::writeToData(
    void* data,
    size_t size
) {
    assert( data );
    assert( size > 0 );

    if( empty() ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Image::writeToData(): Failed to write image to data. Image is empty.";
#endif
        return 0;
    }

    MetaFormat format;

    metaFormatInstanceFromImage(
        &format,
        this
    );

    std::string jsonData;
    const auto successfullySerialized = SppSerializeJsonToMemory(
                                            &format,
                                            jsonData
                                        );
    assert( successfullySerialized );

    if( jsonData.empty() || !successfullySerialized ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Image::writeToData(): Failed to serialize meta format block to json.";
#endif
        return 0;
    }

    if( sizeof( int ) + sizeof( size_t ) + jsonData.size() > size ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Image::writeToData(): Buffer is too small.";
#endif
        return 0;
    }

    const int       magic( MetaFormat::magic );
    const size_t    metaFormatSize( jsonData.size() );
    {
        ( void ) memcpy(
            ( void* )( ( char* )data ),
            ( const void* )&magic,
            sizeof( magic )
        );
        ( void ) memcpy(
            ( void* )( ( char* )data + sizeof( int ) ),
            ( const void* )&metaFormatSize,
            sizeof( magic )
        );
    }

    spp::MemoryStream memoryStream;
    memoryStream.Attach(
        data,
        spp::MemoryStreamMode::ReadWritable,
        size
    );

    memoryStream.MoveBegin();
    assert( memoryStream.Move( sizeof( int ) + sizeof( size_t ) ) );

    const auto headerBytesWritten = memoryStream.Write(
                                        ( void* )jsonData.c_str(),
                                        jsonData.length()
                                    );
    assert( headerBytesWritten != jsonData.length() );

    if( headerBytesWritten !=  jsonData.length() ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Imgae::writeToData(): Failed to write json blob to stream.";
#endif
        return 0;
    }

    assert( memoryStream.Move( headerBytesWritten ) );

    size_t index( 0 );

    for( auto it = format.layers.Begin(); it != format.layers.End(); ++it ) {
        const auto currentLayer = this->layerByIndex(
                                      index
                                  );
        assert( currentLayer );
        ++index;

        if( !currentLayer ) {
            continue;
        }

        if( currentLayer->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
            const auto temporaryBuffer = currentLayer->internalDeviceForBackend(
                                             FXAPI_BACKEND_CPU
                                         )->allocator()->alloc( currentLayer->byteSize() );
            assert( temporaryBuffer->data );

            const auto successfullyRetrievedData = currentLayer->retrieve(
                    temporaryBuffer->data
                                                   );
            assert( successfullyRetrievedData );

            if( !successfullyRetrievedData ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
                qDebug() << "Image::writeToData(): Failed to retrieve image data.";
#endif
                continue;
            }

            const auto bytesWritten = memoryStream.Write(
                                          temporaryBuffer->data,
                                          currentLayer->byteSize()
                                      );
            assert( bytesWritten > 0 );

            if( bytesWritten != currentLayer->byteSize() ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
                qDebug() << "Image::writeToData(): Failed to write image data.";
#endif
                continue;
            }

            ( *it ).offset = memoryStream.GetPosition();
            assert( memoryStream.Move( bytesWritten ) );
        } else {
            libgraphics::Bitmap bitmap;
            const auto successfullyRetrievedData = currentLayer->retrieve(
                    &bitmap
                                                   );
            assert( successfullyRetrievedData );

            if( !successfullyRetrievedData ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
                qDebug() << "Image::writeToData(): Failed to retrieve image data.";
#endif
                continue;
            }

            const auto bytesWritten = memoryStream.Write(
                                          bitmap.buffer(),
                                          currentLayer->byteSize()
                                      );
            LOGB_ASSERT( bytesWritten > 0, "no bytes written" );

            ( *it ).offset = memoryStream.GetPosition();
            assert( memoryStream.Move( bytesWritten ) );
        }
    }

    memoryStream.Flush();


    return memoryStream.GetPosition();
}

/// reading image layers from file and memory
ImageLayer* Image::loadImageLayerFromFile(
    fxapi::ApiBackendDevice* device,
    const std::string& path,
    io::Pipeline* ioPipeline
) {
    assert( ioPipeline );
    assert( device );

    libgraphics::Bitmap importedBitmap;
    const auto successfullyImportedFromPath = ioPipeline->importFromPath(
                path.c_str(),
                &importedBitmap
            );

    assert( successfullyImportedFromPath );

    if( !successfullyImportedFromPath ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Image::loadImageLayerFromFile(): Failed to import data from path. Aborting...";
#endif
        importedBitmap.reset();

        return nullptr;
    }

    ImageLayer* imageLayer = new ImageLayer(
        device,
        ""
    );
    assert( imageLayer );

    const auto successfullyUploadedData = imageLayer->reset(
            &importedBitmap
                                          );
    assert( successfullyUploadedData );

    if( !successfullyUploadedData ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Image::loadImageLayerFromFile(): Failed to create ImageLayer instance from bitmap.";
#endif
        delete imageLayer;

        importedBitmap.reset();

        return nullptr;
    }

    appendLayer(
        imageLayer
    );

    return imageLayer;
}

ImageLayer* Image::loadImageLayerFromData(
    fxapi::ApiBackendDevice* device,
    void* data,
    size_t length,
    io::Pipeline* ioPipeline
) {
    assert( ioPipeline );
    assert( device );
    assert( data );

    libgraphics::Bitmap importedBitmap;
    const auto successfullyImportedFromPath = ioPipeline->importFromData(
                data,
                length,
                &importedBitmap
            );

    assert( successfullyImportedFromPath );

    if( !successfullyImportedFromPath ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Image::loadImageLayerFromData(): Failed to import data from path. Aborting...";
#endif
        importedBitmap.reset();

        return nullptr;
    }

    ImageLayer* imageLayer = new ImageLayer(
        device,
        ""
    );
    assert( imageLayer );

    const auto successfullyUploadedData = imageLayer->reset(
            &importedBitmap
                                          );
    assert( successfullyUploadedData );

    if( !successfullyUploadedData ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Image::loadImageLayerFromData(): Failed to create ImageLayer instance from bitmap.";
#endif
        delete imageLayer;

        importedBitmap.reset();

        return nullptr;
    }

    appendLayer(
        imageLayer
    );

    return imageLayer;

}


}
