
#include <libgraphics/io/plugins/imagemagick/exporter.hpp>
#include <libgraphics/io/plugins/imagemagick/importer.hpp>
#include <libgraphics/io/plugins/imagemagick/pluginmain.hpp>

struct MagickSession {
    MagickSession() {
        this->initialized = true;

        try {
            Magick::InitializeMagick( nullptr );
        } catch( ... ) {
            this->initialized = false;
        }
    }
    ~MagickSession() {}

    bool initialized;
};
std::unique_ptr<MagickSession> __magickSession;


std::pair< MagickPluginState::ImageOrigin, std::shared_ptr< Magick::Image > >& MagickPluginState::newEntry() {
    this->m_ImageCache.push_back(
        std::make_pair(
            MagickPluginState::ImageOrigin(),
            std::shared_ptr<Magick::Image>(
                new Magick::Image()
            )
        )
    );

    return this->m_ImageCache.back();
}

std::pair< MagickPluginState::ImageOrigin, std::shared_ptr< Magick::Image > >& MagickPluginState::newEntry(
    const std::string& _path
) {
    this->m_ImageCache.push_back(
        std::make_pair(
            MagickPluginState::ImageOrigin(
                _path
            ),
            std::shared_ptr<Magick::Image>(
                new Magick::Image()
            )
        )
    );

    return this->m_ImageCache.back();
}

std::pair< MagickPluginState::ImageOrigin, std::shared_ptr< Magick::Image > >& MagickPluginState::byPath(
    const std::string& _path
) {
    for( auto it = this->m_ImageCache.begin(); it != this->m_ImageCache.end(); ++it ) {
        if( ( *it ).first.path == _path ) {
            return ( *it );
        }
    }

    assert( false );
    return *( this->m_ImageCache.begin() );
}

std::vector<std::pair< MagickPluginState::ImageOrigin, std::shared_ptr< Magick::Image > > >::iterator  MagickPluginState::begin() {
    return this->m_ImageCache.begin();
}

std::vector<std::pair< MagickPluginState::ImageOrigin, std::shared_ptr< Magick::Image > > >::iterator  MagickPluginState::end() {
    return this->m_ImageCache.end();
}

void MagickPluginState::erase(
    const std::vector<std::pair< ImageOrigin, std::shared_ptr< Magick::Image > > >::iterator& it
) {
    assert( it != this->m_ImageCache.end() );
    this->m_ImageCache.erase(
        it
    );
}

void MagickPluginState::clear() {
    this->m_ImageCache.clear();
}

static MagickPluginState    __globalPluginState;
MagickPluginState&   MagickPluginState::global() {
    return __globalPluginState;
}

namespace iomagick {

Magick::ImageType   getMagickImageTypeFromFormat( libgraphics::Format format ) {
    assert( format.byteSize != 0 );

    switch( format.family ) {
        case libgraphics::formats::family::RGB:
            return Magick::TrueColorType;

        case libgraphics::formats::family::RGBA:
            return Magick::TrueColorAlphaType;

        default:
            return ( Magick::ImageType )0;
    }
}

libgraphics::Format getFormatFromMagickImage( Magick::Image* image ) {
    assert( image != nullptr );

    if( image == nullptr ) {
        return libgraphics::Format();
    }

    const auto depth    = image->depth();
    const auto type     = image->type();

    switch( type ) {
        case Magick::GrayscaleType:
        case Magick::TrueColorType: {
            switch( depth ) {
                case 8:
                    return libgraphics::formats::RGB8::toFormat();

                case 16:
                    return libgraphics::formats::RGB16::toFormat();
            }

            break;
        }

        case Magick::GrayscaleAlphaType:
        case Magick::TrueColorAlphaType: {
            switch( depth ) {
                case 8:
                    return libgraphics::formats::RGBA8::toFormat();

                case 16:
                    return libgraphics::formats::RGBA16::toFormat();
            }

            break;
        }

        default:
            assert( false );
    }

    return libgraphics::Format();
}

bool initializePlugin() {
    if( !__magickSession ) {
        __magickSession.reset( new MagickSession() );
        return __magickSession->initialized;
    }

    return __magickSession->initialized;
}

}

bool CreatePipelinePlugin(
    libgraphics::io::PipelinePlugin** plugin
) {
    assert( plugin );

    if( !plugin ) {
        return false;
    }

    libgraphics::io::GenericPipelinePlugin* gp = new libgraphics::io::GenericPipelinePlugin( "MagickPlugin" );

    /// register exporters
    {
        gp->registerPipelineExporter(
            new MagickExporter( LIBGRAPHICS_IO_FORMAT_JPEG )
        );
        gp->registerPipelineExporter(
            new MagickExporter( LIBGRAPHICS_IO_FORMAT_BMP )
        );
        gp->registerPipelineExporter(
            new MagickExporter( LIBGRAPHICS_IO_FORMAT_PNG )
        );
        gp->registerPipelineExporter(
            new MagickExporter( LIBGRAPHICS_IO_FORMAT_TIF )
        );
    }

    /// register importers
    {
        gp->registerPipelineImporter(
            new MagickImporter( LIBGRAPHICS_IO_FORMAT_JPEG )
        );
        gp->registerPipelineImporter(
            new MagickImporter( LIBGRAPHICS_IO_FORMAT_BMP )
        );
        gp->registerPipelineImporter(
            new MagickImporter( LIBGRAPHICS_IO_FORMAT_PNG )
        );
        gp->registerPipelineImporter(
            new MagickImporter( LIBGRAPHICS_IO_FORMAT_TIF )
        );
    }

    *plugin = ( libgraphics::io::PipelinePlugin* )gp;

    return true;
}
