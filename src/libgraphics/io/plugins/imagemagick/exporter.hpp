#pragma once

#include <libgraphics/io/pipelineplugin.hpp>
#include <libgraphics/io/pipelineimporter.hpp>
#include <libgraphics/io/pipeline.hpp>

class MagickExporter : public libgraphics::io::PipelineExporter {
    public:
        struct Private;

        MagickExporter( const char* extension );
        virtual ~MagickExporter() {}

        /// info methods
        virtual const char* name();
        virtual const char* mainExtension();
        virtual bool supportsExtension(
            const char* extension
        );
        virtual bool supportsActionFromData();
        virtual bool supportsActionFromPath( const char* path );

        /// io methods
        virtual bool exportToStream(
            void* data,
            size_t length,
            libgraphics::Bitmap* toSave
        );
        virtual bool exportToPath(
            const char* path,
            libgraphics::Bitmap* toSave
        );

    protected:
        libcommon::PimplPtr<Private> d;
};
