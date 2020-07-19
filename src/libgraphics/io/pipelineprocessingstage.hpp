#pragma once

#include <libgraphics/base.hpp>
#include <libgraphics/bitmap.hpp>
#include <libgraphics/io/pipelineinfo.hpp>
#include <libcommon/scopedptr.hpp>
#include <libcommon/atomics.hpp>
#include <libcommon/noncopyable.hpp>

#include <string>
#include <functional>

namespace libgraphics {
namespace io {

/// extern
class PipelineProcessor;

/// class
class PipelineProcessingStage : public libcommon::INonCopyable {
    public:
        struct Private;

        PipelineProcessingStage();
        virtual ~PipelineProcessingStage() {}

        void addProcessor( PipelineProcessor* processor );
        bool removeProcessor( PipelineProcessor* processor );
        void clearProcessors();
        size_t countProcessors() const;
        bool containsProcessor( PipelineProcessor* processor );

        const std::string& name() const;
        void setName( const std::string& name );

        bool process( libgraphics::Bitmap* bitmap );

        bool enabled() const;
        void enable();
        void disable();
    protected:
        libcommon::PimplPtr<Private>   d;
};

}
}
