#pragma once

#include <libgraphics/base.hpp>
#include <libgraphics/bitmap.hpp>
#include <libgraphics/io/pipelineinfo.hpp>
#include <libcommon/atomics.hpp>
#include <libcommon/noncopyable.hpp>

#include <libgraphics/io/pipelineplugin.hpp>
#include <libgraphics/io/pipelineexporter.hpp>
#include <libgraphics/io/pipelineimporter.hpp>
#include <libgraphics/io/pipelineprocessingstage.hpp>

#define LIBGRAPHICS_IO_FORMAT_RAW  "RAW"
#define LIBGRAPHICS_IO_FORMAT_JPEG  "JPEG"
#define LIBGRAPHICS_IO_FORMAT_TIF "TIF"
#define LIBGRAPHICS_IO_FORMAT_PNG "PNG"
#define LIBGRAPHICS_IO_FORMAT_BMP "BMP"
#define LIBGRAPHICS_IO_FORMAT_JPEG2000 "JPEG2000"

namespace libgraphics {
namespace io {

/// external
class PipelineExporter;
class PipelineImporter;
class PipelinePlugin;
class PipelineProcessingStage;
class PipelineProcessor;

/// forward
class Pipeline;
class StdPipeline;

/// interface: Pipeline
class Pipeline : public libcommon::INonCopyable {
    public:
        virtual ~Pipeline() {}

        /// processing management
        /// the pipeline only holds weak references
        virtual bool addProcessingStage(
            PipelineProcessingStage* stage
        ) = 0;

        virtual bool removeProcessingStage(
            PipelineProcessingStage* stage
        ) = 0;
        virtual bool removeProcessingStageByName(
            const char* name
        ) = 0;
        virtual bool removeProcessingStageByIndex(
            size_t index
        ) = 0;

        virtual PipelineProcessingStage* getProcessingStageByIndex(
            size_t index
        ) = 0;
        virtual PipelineProcessingStage* getFirstProcessingStageByName(
            const char* name
        ) = 0;

        virtual bool containsProcessingStage(
            const char* name
        ) = 0;
        virtual bool containsProcessingStage(
            PipelineProcessingStage* stage
        ) = 0;

        virtual size_t countProcessingStages() = 0;
        virtual size_t countProcessors() = 0;

        virtual bool clearProcessingStages() = 0;

        virtual void disableAllProcessingStages() = 0;
        virtual void enableAllProcessingStages() = 0;

        virtual bool enableProcessingStage(
            size_t index
        ) = 0;
        virtual bool enableProcessingStage(
            const char* name
        ) = 0;

        virtual bool disableProcessingStage(
            size_t index
        ) = 0;
        virtual bool disableProcessingStage(
            const char* name
        ) = 0;

        /// io management
        virtual bool addExportersFromPlugin(
            PipelinePlugin* plugin
        ) = 0;
        virtual bool addExporter(
            PipelineExporter* exporter
        ) = 0;

        virtual bool addImportersFromPlugin(
            PipelinePlugin* plugin
        ) = 0;
        virtual bool addImporter(
            PipelineImporter* importer
        ) = 0;

        virtual bool removeExporter(
            PipelineExporter* exporter
        ) = 0;
        virtual bool removeExporterByName(
            const char* name
        ) = 0;
        virtual bool removeExportersForExtension(
            const char* extension
        ) = 0;

        virtual bool removeImporter(
            PipelineImporter* importer
        ) = 0;
        virtual bool removeImporterByName(
            const char* name
        ) = 0;
        virtual bool removeImportersForExtension(
            const char* extension
        ) = 0;

        virtual bool containsExporterForExtension(
            const char* extension
        ) = 0;
        virtual bool containsImporterForExtension(
            const char* extension
        ) = 0;

        virtual bool containsExporter(
            PipelineExporter* exporter
        ) = 0;
        virtual bool containsImporter(
            PipelineImporter* importer
        ) = 0;

        virtual void clearImporters() = 0;
        virtual void clearExporters() = 0;

        virtual size_t countImporters() = 0;
        virtual size_t countExporters() = 0;

        /// general management methods

        virtual void reset() = 0;
        virtual libgraphics::io::PipelineInfo   info() = 0;

        /// io methods
        virtual bool importFromData(
            void* data,
            size_t length,
            libgraphics::Bitmap* out
        ) = 0;
        virtual bool importFromPath(
            const char* path,
            libgraphics::Bitmap* out
        ) = 0;

        virtual bool exportToStream(
            const char* extension,
            void* data,
            size_t length,
            libgraphics::Bitmap* toSave
        ) = 0;
        virtual bool exportToPath(
            const char* extension,
            const char* path,
            libgraphics::Bitmap* toSave
        ) = 0;
};

/// impl: StdPipeline
class StdPipeline : public Pipeline {
    public:
        struct Private : libcommon::PimplPrivate {
            std::vector< std::unique_ptr< PipelineProcessingStage > >  stages;
            PipelineImporterGroup importers;
            PipelineExporterGroup exporters;
        };

        StdPipeline();
        virtual ~StdPipeline() {}

        /// processing management
        /// the pipeline only holds weak references
        virtual bool addProcessingStage(
            PipelineProcessingStage* stage
        );

        virtual bool removeProcessingStage(
            PipelineProcessingStage* stage
        );
        virtual bool removeProcessingStageByName(
            const char* name
        );
        virtual bool removeProcessingStageByIndex(
            size_t index
        );

        virtual PipelineProcessingStage* getProcessingStageByIndex(
            size_t index
        );
        virtual PipelineProcessingStage* getFirstProcessingStageByName(
            const char* name
        );

        virtual bool containsProcessingStage(
            const char* name
        );
        virtual bool containsProcessingStage(
            PipelineProcessingStage* stage
        );

        virtual size_t countProcessingStages();
        virtual size_t countProcessors();

        virtual bool clearProcessingStages();

        virtual void disableAllProcessingStages();
        virtual void enableAllProcessingStages();

        virtual bool enableProcessingStage(
            size_t index
        );
        virtual bool enableProcessingStage(
            const char* name
        );

        virtual bool disableProcessingStage(
            size_t index
        );
        virtual bool disableProcessingStage(
            const char* name
        );

        /// io management
        virtual bool addExportersFromPlugin(
            PipelinePlugin* plugin
        );
        virtual bool addExporter(
            PipelineExporter* exporter
        );

        virtual bool addImportersFromPlugin(
            PipelinePlugin* plugin
        );
        virtual bool addImporter(
            PipelineImporter* importer
        );

        virtual bool removeExporter(
            PipelineExporter* exporter
        );
        virtual bool removeExporterByName(
            const char* name
        );
        virtual bool removeExportersForExtension(
            const char* extension
        );

        virtual bool removeImporter(
            PipelineImporter* importer
        );
        virtual bool removeImporterByName(
            const char* name
        );
        virtual bool removeImportersForExtension(
            const char* extension
        );

        virtual bool containsExporterForExtension(
            const char* extension
        );
        virtual bool containsImporterForExtension(
            const char* extension
        );

        virtual bool containsExporter(
            PipelineExporter* exporter
        );
        virtual bool containsImporter(
            PipelineImporter* importer
        );

        virtual void clearImporters();
        virtual void clearExporters();

        virtual size_t countImporters();
        virtual size_t countExporters();

        /// general management methods

        virtual void reset();
        virtual libgraphics::io::PipelineInfo   info();

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

        virtual bool exportToStream(
            const char* extension,
            void* data,
            size_t length,
            libgraphics::Bitmap* toSave
        );
        virtual bool exportToPath(
            const char* extension,
            const char* path,
            libgraphics::Bitmap* toSave
        );
    private:
        libcommon::PimplPtr<Private> d;
};

}
}
