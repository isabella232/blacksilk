#pragma once

#include <libgraphics/base.hpp>

#include <libcommon/atomics.hpp>
#include <libcommon/noncopyable.hpp>

#include <vector>
#include <string>
#include <memory>

namespace libgraphics {
namespace io {

class Pipeline;

class PipelineInfo {
    public:
        struct Private {
            struct Info {
                std::string     extension;
                bool            importable;
                bool            exportable;
            };
            std::vector<Info>   extensions;
        };

        friend class Pipeline;
        friend class StdPipeline;

        PipelineInfo();

        bool supportsExtension( const char* extension );
        bool isExtensionImportable( const char* extension );
        bool isExtensionExportable( const char* extension );

        size_t  countImporters() const;
        size_t  countExporters() const;
    protected:
        void addSupportedExtension( const char* extension );
        void addImporterExtension( const char* extension );
        void addExporterExtension( const char* extension );

        std::shared_ptr<Private>   d;
};

}
}
