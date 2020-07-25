#pragma once

#include <libgraphics/io/pipelineplugin.hpp>
#include <libgraphics/io/pipelineimporter.hpp>
#include <libgraphics/io/pipeline.hpp>

class MagickImporter : public libgraphics::io::PipelineImporter {
    public:
        struct Private;

        MagickImporter( const char* format );
        virtual ~MagickImporter() {}

        /// info methods
        virtual const char* name();
        virtual const char* mainExtension();
        virtual bool supportsExtension(
            const char* extension
        );
        virtual bool supportsActionFromData();
        virtual bool supportsActionFromPath( const char* path );

        /// io methods
        virtual bool importFromData(
            void* data,
            size_t length,
            libgraphics::Bitmap* out
        );
        virtual bool importFromPath(
            const char* path,
            libgraphics::Bitmap* out
        );

    protected:
        std::shared_ptr<Private> d;
};
