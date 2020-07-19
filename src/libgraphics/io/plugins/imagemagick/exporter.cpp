#include <libgraphics/io/plugins/imagemagick/exporter.hpp>
#include <libgraphics/io/plugins/imagemagick/pluginmain.hpp>

struct MagickExporter::Private : libcommon::PimplPrivate {
    std::string ext;

    bool copyFromBitmap(
        Magick::Quantum* packet,
        libgraphics::Bitmap* toSave,
        bool is16bit
    ) {
        const auto buf = toSave->buffer();

        if( toSave->format().family == libgraphics::formats::family::RGBA ) {
            if( !is16bit ) {
                for( size_t y = 0; toSave->height() > y; ++y ) {
                    for( size_t x = 0; toSave->width() > x; ++x ) {
                        *packet++ = 255 * ( ( ( libgraphics::formats::RGBA8::t* )buf ) + ( y * toSave->width() ) + x )->Values[3]; // alpha
                        *packet++ = 255 * ( ( ( libgraphics::formats::RGBA8::t* )buf ) + ( y * toSave->width() ) + x )->Values[0]; // red
                        *packet++ = 255 * ( ( ( libgraphics::formats::RGBA8::t* )buf ) + ( y * toSave->width() ) + x )->Values[1]; // green
                        *packet++ = 255 * ( ( ( libgraphics::formats::RGBA8::t* )buf ) + ( y * toSave->width() ) + x )->Values[2]; // blue
                    }
                }
            } else {
                for( size_t y = 0; toSave->height() > y; ++y ) {
                    for( size_t x = 0; toSave->width() > x; ++x ) {
                        *packet++ = ( ( ( libgraphics::formats::RGBA16::t* )buf ) + ( y * toSave->width() ) + x )->Values[3];
                        *packet++ = ( ( ( libgraphics::formats::RGBA16::t* )buf ) + ( y * toSave->width() ) + x )->Values[0]; // red
                        *packet++ = ( ( ( libgraphics::formats::RGBA16::t* )buf ) + ( y * toSave->width() ) + x )->Values[1]; // green
                        *packet++ = ( ( ( libgraphics::formats::RGBA16::t* )buf ) + ( y * toSave->width() ) + x )->Values[2]; // blue
                    }
                }
            }
        } else if( toSave->format().family == libgraphics::formats::family::RGB ) {
            if( !is16bit ) {
                for( size_t y = 0; toSave->height() > y; ++y ) {
                    for( size_t x = 0; toSave->width() > x; ++x ) {
                        *packet++ = 255 * ( *( ( ( libgraphics::formats::RGB8::t* )buf ) + ( y * toSave->width() ) + x ) ).Values[0]; // red
                        *packet++ = 255 * ( *( ( ( libgraphics::formats::RGB8::t* )buf ) + ( y * toSave->width() ) + x ) ).Values[1]; // green
                        *packet++ = 255 * ( *( ( ( libgraphics::formats::RGB8::t* )buf ) + ( y * toSave->width() ) + x ) ).Values[2]; // blue
                    }
                }
            } else {
                for( size_t y = 0; toSave->height() > y; ++y ) {
                    for( size_t x = 0; toSave->width() > x; ++x ) {
                        *packet++ = ( ( ( libgraphics::formats::RGB16::t* )buf ) + ( y * toSave->width() ) + x )->Values[0];
                        *packet++ = ( ( ( libgraphics::formats::RGB16::t* )buf ) + ( y * toSave->width() ) + x )->Values[1];
                        *packet++ = ( ( ( libgraphics::formats::RGB16::t* )buf ) + ( y * toSave->width() ) + x )->Values[2];
                    }
                }
            }
        } else if( toSave->format().family == libgraphics::formats::family::BGR ) {
            if( !is16bit ) {
                for( size_t y = 0; toSave->height() > y; ++y ) {
                    for( size_t x = 0; toSave->width() > x; ++x ) {
                        *packet++ = 255 * ( ( ( libgraphics::formats::BGR8::t* )buf ) + ( y * toSave->width() ) + x )->Values[2];
                        *packet++ = 255 * ( ( ( libgraphics::formats::BGR8::t* )buf ) + ( y * toSave->width() ) + x )->Values[1];
                        *packet++ = 255 * ( ( ( libgraphics::formats::BGR8::t* )buf ) + ( y * toSave->width() ) + x )->Values[0];
                    }
                }
            } else {
                for( size_t y = 0; toSave->height() > y; ++y ) {
                    for( size_t x = 0; toSave->width() > x; ++x ) {
                        *packet++ = ( ( ( libgraphics::formats::BGR16::t* )buf ) + ( y * toSave->width() ) + x )->Values[2];
                        *packet++ = ( ( ( libgraphics::formats::BGR16::t* )buf ) + ( y * toSave->width() ) + x )->Values[1];
                        *packet++ = ( ( ( libgraphics::formats::BGR16::t* )buf ) + ( y * toSave->width() ) + x )->Values[0];
                    }
                }
            }
        } else if( toSave->format().family == libgraphics::formats::family::Mono ) {
            if( !is16bit ) {
                for( size_t y = 0; toSave->height() > y; ++y ) {
                    for( size_t x = 0; toSave->width() > x; ++x ) {
                        *packet++ = 255 * ( ( ( libgraphics::formats::Mono8::t* )buf ) + ( y * toSave->width() ) + x )->Values[0];
                    }
                }
            } else {
                for( size_t y = 0; toSave->height() > y; ++y ) {
                    for( size_t x = 0; toSave->width() > x; ++x ) {
                        *packet++ = ( ( ( libgraphics::formats::Mono16::t* )buf ) + ( y * toSave->width() ) + x )->Values[0];
                    }
                }
            }
        } else {
            return false; /** unknown pixel format **/
        }

        return true;
    }

};

MagickExporter::MagickExporter( const char* ext ) : d( new Private() ) {
    d->ext.assign( ext, strlen( ext ) );
}

/// info methods
const char* MagickExporter::name() {
    return "MagickExporter";
}

const char* MagickExporter::mainExtension() {
    return LIBGRAPHICS_IO_FORMAT_JPEG;
}

bool MagickExporter::supportsExtension(
    const char* extension
) {
    return ( d->ext == extension );
}

bool MagickExporter::supportsActionFromData() {
    return true;
}

bool MagickExporter::supportsActionFromPath( const char* path ) {
    ( void )path;
    return true;
}

bool MagickExporter::exportToStream(
    void* data,
    size_t length,
    libgraphics::Bitmap* toSave
) {
    assert( data );
    assert( length );
    assert( toSave );

    bool _ret = iomagick::initializePlugin();
    assert( _ret );

    if( !_ret ) {
        return false;
    }

#if IMAGEMAGICK_IMPORTER_GLOBAL_META_IMAGE
    std::shared_ptr<Magick::Image>      image( nullptr );

    if( MagickPluginState::global().begin() != MagickPluginState::global().end() ) {
        auto imageObject = *MagickPluginState::global().begin();

        image = imageObject.second;
    } else {
        image.reset(
            new Magick::Image( Magick::Geometry( toSave->width(), toSave->height() ), Magick::Color( 0, 0, 0 ) )
        );
    }

#else
    std::shared_ptr<Magick::Image> image( new Magick::Image( Magick::Geometry( toSave->width(), toSave->height() ), Magick::Color( 0, 0, 0 ) ) );
#endif

    image->modifyImage();
    image->type( Magick::TrueColorType );

    std::string formatName;

    switch( toSave->format().family ) {
        case libgraphics::formats::family::ARGB:
            formatName = "RGBA";
            break;

        case libgraphics::formats::family::RGB:
            formatName = "RGB";
            break;

        case libgraphics::formats::family::Mono:
            formatName = "Gray";
            break;

        case libgraphics::formats::family::BGR:
            formatName = "RGB";
            break;

        case libgraphics::formats::family::BGRA:
            formatName = "RGBA";
            break;

        default:
            return false; /** invalid format **/
    }

    image->magick( formatName );
    image->magick( d->ext );

    Magick::Quantum* packet = image->getPixels( 0, 0, toSave->width(), toSave->height() );
    assert( packet );

    if( !packet ) {
        return false;
    }

    bool is16bit( false );

    switch( toSave->format().byteSize / toSave->format().channels ) {
        case 1:
            is16bit = false;
            break;

        case 2:
            is16bit = true;
            break;

        default:
            return false; // no support for 32bit images
    }

    if( !d->copyFromBitmap(
                packet,
                toSave,
                is16bit
            ) ) {
        assert( false );
        return false;
    }

    image->syncPixels();

    Magick::Blob formattedData;

    try {
        image->write( &formattedData, formatName );
    } catch( ... ) {
        return false;
    }

    assert( formattedData.length() >= length );

    if( formattedData.length() > length ) {
        return false; /** target data is too big **/
    }

    ( void ) memcpy( ( void* )data, formattedData.data(), formattedData.length() );

    return true;
}

bool MagickExporter::exportToPath(
    const char* path,
    libgraphics::Bitmap* toSave
) {
    assert( path );
    assert( toSave );

    bool _ret = iomagick::initializePlugin();
    assert( _ret );

    if( !_ret ) {
        return false;
    }

#if IMAGEMAGICK_IMPORTER_GLOBAL_META_IMAGE
    std::shared_ptr<Magick::Image>      image( nullptr );

    if( MagickPluginState::global().begin() != MagickPluginState::global().end() ) {
        auto imageObject = *MagickPluginState::global().begin();

        image = imageObject.second;
    } else {
        image.reset(
            new Magick::Image( Magick::Geometry( toSave->width(), toSave->height() ), Magick::Color( 0, 0, 0 ) )
        );
    }

#else
    std::shared_ptr<Magick::Image> image( new Magick::Image( Magick::Geometry( toSave->width(), toSave->height() ), Magick::Color( 0, 0, 0 ) ) );
#endif

    assert( image.get() != nullptr );

    const auto imageType = iomagick::getMagickImageTypeFromFormat(
                               toSave->format()
                           );

    image->modifyImage();
    image->type( imageType );

    std::string formatName;

    switch( toSave->format().family ) {
        case libgraphics::formats::family::RGBA:
            formatName = "RGBA";
            break;

        case libgraphics::formats::family::RGB:
            formatName = "RGB";
            break;

        case libgraphics::formats::family::Mono:
            formatName = "Gray";
            break;

        case libgraphics::formats::family::BGR:
            formatName = "RGB";
            break;

        case libgraphics::formats::family::BGRA:
            formatName = "RGBA";
            break;

        default:
            return false; /** invalid format **/
    }

    image->magick( formatName );
    image->magick( d->ext );


    Magick::Quantum* packet = image->getPixels( 0, 0, toSave->width(), toSave->height() );
    assert( packet );

    if( !packet ) {
        return false;
    }

    bool is16bit( false );

    switch( toSave->format().byteSize / toSave->format().channels ) {
        case 1:
            is16bit = false;
            break;

        case 2:
            is16bit = true;
            break;

        default:
            return false; // no support for 32bit images
    }

    if( !d->copyFromBitmap(
                packet,
                toSave,
                is16bit
            ) ) {
        assert( false );
        return false;
    }

    image->syncPixels();

    try {
        image->write( std::string( path ) );
    } catch( ... ) {
        return false;
    }

    return true;

}

void MagickExporter::lock() {
    ( void )0;
}

bool MagickExporter::tryLock() {
    ( void )0;
    return true;
}

void MagickExporter::unlock() {
    ( void )0;
}
