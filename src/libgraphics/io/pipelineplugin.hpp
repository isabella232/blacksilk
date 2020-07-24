#pragma once

#include <libgraphics/base.hpp>
#include <libcommon/atomics.hpp>

#include <libgraphics/io/pipelineexporter.hpp>
#include <libgraphics/io/pipelineimporter.hpp>
#include <libgraphics/io/pipelineplugininfo.hpp>

#define LIBGRAPHICS_IO_CallbackCreatePipelinePluginApiName "CreatePipelinePlugin"

namespace libgraphics {
namespace io {

/// forward
class PipelinePlugin;
class GenericPipelinePlugin;

/// extern
class PipelineImporter;
class PipelineExporter;

/// interface: PipelinePlugin
class PipelinePlugin : public libcommon::INonCopyable {
    public:
        virtual ~PipelinePlugin() {}

        virtual const char* name() = 0;

        virtual size_t countExporters() = 0;
        virtual size_t countImporters() = 0;

        virtual size_t countExportersForExtension( const char* extension ) = 0;
        virtual size_t countImportersForExtension( const char* extension ) = 0;

        virtual PipelineImporterGroup importersForExtension( const char* extension ) = 0;
        virtual PipelineExporterGroup exportersForExtension( const char* extension ) = 0;

        virtual PipelineImporterGroup importers() = 0;
        virtual PipelineExporterGroup exporters() = 0;

        virtual PipelinePluginInfo info() = 0;
};
typedef bool( CallbackCreatePipelinePlugin )( PipelinePlugin** );

/// class: GenericPipelinePlugin
class GenericPipelinePlugin : public PipelinePlugin {
    public:
        struct Private;

        GenericPipelinePlugin( const char* name );
        virtual ~GenericPipelinePlugin() {}

        /// from PipelinePlugin
        virtual const char* name();

        virtual size_t countExporters();
        virtual size_t countImporters();

        virtual size_t countExportersForExtension( const char* extension );
        virtual size_t countImportersForExtension( const char* extension );

        virtual PipelineImporterGroup importersForExtension( const char* extension );
        virtual PipelineExporterGroup exportersForExtension( const char* extension );

        virtual PipelineImporterGroup importers();
        virtual PipelineExporterGroup exporters();

        virtual PipelinePluginInfo info();

        /// methods
        void registerPipelineImporter( PipelineImporter* importer );
        void registerPipelineImporter( const std::shared_ptr<PipelineImporter>& importer );
        void registerPipelineExporter( PipelineExporter* exporter );
        void registerPipelineExporter( const std::shared_ptr<PipelineExporter>& exporter );

        bool unregisterPipelineImporter( PipelineImporter* importer );
        bool unregisterPipelineExporter( PipelineExporter* exporter );

        void setName( const char* name );
    private:
        libcommon::PimplPtr<Private>   d;
};
}
}
