
#include <libgraphics/io/plugins/imagemagick/pluginmain.hpp>
#include <libgraphics/io/plugins/imagemagick/importer.hpp>

#include <iostream>

struct MagickImporter::Private {
    std::string     extension;
};

MagickImporter::MagickImporter( const char* format ) : d( new Private() ) {
    this->d->extension.assign( format );
}

/// info methods
const char* MagickImporter::name() {
    return "MagickImporter";
}

const char* MagickImporter::mainExtension() {
    return d->extension.c_str();
}

bool MagickImporter::supportsExtension(
    const char* extension
) {
    std::string ext( extension );

    return ( ext == d->extension );
}

bool MagickImporter::supportsActionFromData() {
    return true;
}

bool MagickImporter::supportsActionFromPath( const char* path ) {
    ( void )path;
    return true;
}


/// This compile-unit somehow triggers a MSVC internal compiler errors, which prevents the linker from generating
/// valid bytecode. The internal error leads to misleading sizes of basic types thus copying memory from
/// Magick to libgraphics::Bitmap may result in corrupted images. The fix is to encapsule copying images with alpha plane
/// from the original routine. DO NOT EDIT THIS AREA WITHOUT INTENSIVE TESTING!
///
template < class _t_format_type >
void copyFromQuantumNoAlpha( libgraphics::Bitmap* out, Magick::Quantum* packet ) {
    typedef typename _t_format_type::PixelType      PixelType;

    const auto buf      = out->buffer();

    assert( buf != nullptr );
    assert( out->formatByteSize() == sizeof( _t_format_type ) );

    for( size_t y = 0; out->height() > y; ++y ) {
        for( size_t x = 0; out->width() > x; ++x ) {
            ( ( ( _t_format_type* )buf ) + ( y * out->width() ) + x )->Values[0] = ( PixelType )( *packet++ );
            ( ( ( _t_format_type* )buf ) + ( y * out->width() ) + x )->Values[1] = ( PixelType )( *packet++ );
            ( ( ( _t_format_type* )buf ) + ( y * out->width() ) + x )->Values[2] = ( PixelType )( *packet++ );
        }
    }
}
template < class _t_format_type >
void copyFromQuantumWithAlpha( libgraphics::Bitmap* out, Magick::Quantum* packet ) {
    typedef typename _t_format_type::PixelType      PixelType;

    const auto buf      = out->buffer();

    assert( buf != nullptr );
    assert( out->formatByteSize() == sizeof( _t_format_type ) );

    for( size_t y = 0; out->height() > y; ++y ) {
        for( size_t x = 0; out->width() > x; ++x ) {
            ( ( ( _t_format_type* )buf ) + ( y * out->width() ) + x )->Values[3] = ( PixelType )( *packet++ );
            ( ( ( _t_format_type* )buf ) + ( y * out->width() ) + x )->Values[0] = ( PixelType )( *packet++ );
            ( ( ( _t_format_type* )buf ) + ( y * out->width() ) + x )->Values[1] = ( PixelType )( *packet++ );
            ( ( ( _t_format_type* )buf ) + ( y * out->width() ) + x )->Values[2] = ( PixelType )( *packet++ );
        }
    }

}

/// io methods
bool MagickImporter::importFromData(
    void* data,
    size_t length,
    libgraphics::Bitmap* out
) {
#if IMAGEMAGICK_IMPORTER_GLOBAL_META_IMAGE
    MagickPluginState::global().clear();
#endif

    assert( data );
    assert( length > 0 );
    assert( out );

    bool _ret = iomagick::initializePlugin();
    assert( _ret );

    if( !_ret ) {
        return false;
    }

    auto imageObject = MagickPluginState::global().newEntry();
    imageObject.second.reset(
        new Magick::Image(
            Magick::Blob( data, length )
        )
    );
    Magick::Image* image = imageObject.second.get();

    if( ( image->columns() == 0 ) || ( image->rows() == 0 ) ) {
        return false;
    }

    if( !image->isValid() ) {
        return false;
    }

    const auto imageFormat  = iomagick::getFormatFromMagickImage( image );
    const auto ret          = out->reset( imageFormat, image->columns(), image->rows() );
    assert( ret );

    const auto realSize         = out->formatByteSize();
    const auto computedSize     = ( out->width() * out->height() * imageFormat.byteSize );
    assert( realSize == computedSize );

    if( ( imageFormat.family == libgraphics::formats::family::RGB ) || ( imageFormat.family == libgraphics::formats::family::RGBA ) ) {
        Magick::Quantum* packet = image->getPixels( 0, 0, image->columns(), image->rows() );
        assert( packet != nullptr );

        const auto hasAlpha = ( imageFormat.family == libgraphics::formats::family::RGBA );
        const auto is16bit  = ( ( imageFormat.byteSize / imageFormat.channels ) == 2 );


        if( !is16bit ) {
            if( hasAlpha ) {
                copyFromQuantumWithAlpha<libgraphics::formats::RGBA8::t>( out, packet );
            } else {
                copyFromQuantumNoAlpha<libgraphics::formats::RGB8::t>( out, packet );
            }
        } else {
            if( hasAlpha ) {
                copyFromQuantumWithAlpha<libgraphics::formats::RGBA16::t>( out, packet );
            } {

                copyFromQuantumNoAlpha<libgraphics::formats::RGB16::t>( out, packet );
            }
        }

        return true;
    }


    return false;
}

bool MagickImporter::importFromPath(
    const char* path,
    libgraphics::Bitmap* out
) {
#if IMAGEMAGICK_IMPORTER_GLOBAL_META_IMAGE
    MagickPluginState::global().clear();
#endif

    assert( path );
    assert( out );

    bool _ret = iomagick::initializePlugin();
    assert( _ret );

    if( !_ret ) {
        return false;
    }

    auto imageObject = MagickPluginState::global().newEntry(
                           std::string( path )
                       );
    Magick::Image* image = imageObject.second.get();

    try {
        image->read( std::string( path, strlen( path ) ) );
    } catch( ... ) {}

    if( ( image->columns() == 0 ) || ( image->rows() == 0 ) ) {
        return false;
    }

    if( !image->isValid() ) {
        return false;
    }


    const auto imageFormat  = iomagick::getFormatFromMagickImage( image );
    const auto ret          = out->reset( imageFormat, image->columns(), image->rows() );
    assert( ret );

    const auto realSize         = out->pixelCount() * out->formatByteSize();
    const auto computedSize     = ( out->width() * out->height() * imageFormat.byteSize );
    assert( realSize == computedSize );

    if( ( imageFormat.family == libgraphics::formats::family::RGB ) || ( imageFormat.family == libgraphics::formats::family::RGBA ) ) {
        Magick::Quantum* packet = image->getPixels( 0, 0, image->columns(), image->rows() );
        assert( packet != nullptr );

        const auto hasAlpha = ( imageFormat.family == libgraphics::formats::family::RGBA );
        const auto is16bit  = ( ( imageFormat.byteSize / imageFormat.channels ) == 2 );

        if( !is16bit ) {
            if( hasAlpha ) {
                copyFromQuantumWithAlpha<libgraphics::formats::RGBA8::t>( out, packet );
            } else {
                copyFromQuantumNoAlpha<libgraphics::formats::RGB8::t>( out, packet );
            }
        } else {
            if( hasAlpha ) {
                copyFromQuantumWithAlpha<libgraphics::formats::RGBA16::t>( out, packet );
            } {

                copyFromQuantumNoAlpha<libgraphics::formats::RGB16::t>( out, packet );
            }
        }

        return true;
    }

    return false;
}
