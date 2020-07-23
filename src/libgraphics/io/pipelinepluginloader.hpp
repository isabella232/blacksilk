#pragma once

#include <libgraphics/base.hpp>
#include <libcommon/scopedptr.hpp>
#include <libcommon/atomics.hpp>

namespace libgraphics {
namespace io {

/// external
class PipelinePlugin;

/// impl: PipelinePluginLoader
class PipelinePluginLoader : public libcommon::INonCopyable {
    public:
        struct Private;

        PipelinePluginLoader();
        virtual ~PipelinePluginLoader() {}

        const char* path() const;
        bool opened() const;
        bool load( const char* path );
        PipelinePlugin* instantiatePlugin();
    private:
        libcommon::PimplPtr<Private> d;
};

}
}
