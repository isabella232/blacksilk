#pragma once

#include <libgraphics/base.hpp>
#include <libcommon/atomics.hpp>

#include <memory>

namespace libgraphics {
namespace io {

class PipelinePluginInfo {
    public:
        struct Private {
            struct Info {
                std::string     extension;
                bool            importable;
                bool            exportable;
            };
            std::vector<Info>   extensions;
        };

        PipelinePluginInfo();

        bool supportsExtension( const char* extension );
        bool isExtensionImportable( const char* extension );
        bool isExtensionExportable( const char* extension );

        size_t  countImporters() const;
        size_t  countExporters() const;

        void addSupportedExtension( const char* extension );
        void addImporterExtension( const char* extension );
        void addExporterExtension( const char* extension );
    protected:
        std::shared_ptr<Private> d;
};

}
}
