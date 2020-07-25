#pragma once

#include <libgraphics/base.hpp>
#include <libgraphics/bitmap.hpp>
#include <libgraphics/io/pipelineinfo.hpp>
#include <libcommon/atomics.hpp>
#include <libcommon/noncopyable.hpp>

#include <functional>

namespace libgraphics {
namespace io {

/// interface: PipelineProcessor
class PipelineProcessor : libcommon::INonCopyable {
    public:
        virtual ~PipelineProcessor() {}

        virtual bool process(
            libgraphics::Bitmap* bitmap
        ) = 0;
};

typedef bool( ProcessCallback )( libgraphics::Bitmap* );

/// helpers
PipelineProcessor* makePipelineProcessorFromCallback(
    ProcessCallback* callback
);
PipelineProcessor* makePipelineProcessorFromFunction(
    const std::function<ProcessCallback>& fn
);

}
}
