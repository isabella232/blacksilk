#pragma once

#include <vector>
#include <memory>
#include <libgraphics/io/pipelineplugin.hpp>

#include <Magick++.h>
#undef _ASSERT_H_DECLS // fixes Magicks namespaced include of <cassert>

extern "C" LIBCOMMON_EXPORT bool CreatePipelinePlugin(
    libgraphics::io::PipelinePlugin** plugin
);
namespace iomagick {
bool initializePlugin();
libgraphics::Format getFormatFromMagickImage( Magick::Image* image );
Magick::ImageType   getMagickImageTypeFromFormat( libgraphics::Format format );
}

class MagickPluginState {
    public:
        struct ImageOrigin {
            enum t {
                FromPath,
                FromMemory
            };
            std::string     path;
            t               origin;

            ImageOrigin() : origin( FromMemory ) {}
            ImageOrigin( const std::string& _path ) :
                path( _path ), origin( FromPath ) {}
        };

        std::pair< ImageOrigin, std::shared_ptr< Magick::Image > >& newEntry();
        std::pair< ImageOrigin, std::shared_ptr< Magick::Image > >& newEntry(
            const std::string& _path
        );

        std::pair< ImageOrigin, std::shared_ptr< Magick::Image > >& byPath(
            const std::string& _path
        );

        std::vector<std::pair< ImageOrigin, std::shared_ptr< Magick::Image > > >::iterator  begin();
        std::vector<std::pair< ImageOrigin, std::shared_ptr< Magick::Image > > >::iterator  end();

        void erase(
            const std::vector<std::pair< ImageOrigin, std::shared_ptr< Magick::Image > > >::iterator& it
        );
        void clear();

        static MagickPluginState&   global();
    protected:
        std::vector<std::pair< ImageOrigin, std::shared_ptr< Magick::Image > > >  m_ImageCache;
};
