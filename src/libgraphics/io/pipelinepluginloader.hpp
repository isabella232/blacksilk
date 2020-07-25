#pragma once

#include <libgraphics/base.hpp>
#include <libcommon/atomics.hpp>
#include <libcommon/noncopyable.hpp>

#include <memory>

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
        std::shared_ptr<Private> d;
};

}
}
