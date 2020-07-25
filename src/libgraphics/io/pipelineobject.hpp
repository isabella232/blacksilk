#pragma once

#include <libgraphics/base.hpp>
#include <libcommon/atomics.hpp>
#include <libcommon/noncopyable.hpp>

namespace libgraphics {
namespace io {

/// interface: PipelineObject
class PipelineObject : public libcommon::INonCopyable {
    public:
        virtual ~PipelineObject() {}

        /// info methods
        virtual const char* name() = 0;
        virtual const char* mainExtension() = 0;
        virtual bool supportsExtension(
            const char* extension
        ) = 0;
        virtual bool supportsActionFromData() = 0;
        virtual bool supportsActionFromPath( const char* path ) = 0;
};

}
}
