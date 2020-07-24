#pragma once

#include <libgraphics/base.hpp>
#include <libgraphics/bitmap.hpp>
#include <libgraphics/io/pipelineobject.hpp>
#include <libgraphics/io/pipelineobjectgroup.hpp>

#include <libcommon/atomics.hpp>
#include <libcommon/noncopyable.hpp>

#include <functional>
#include <vector>

namespace libgraphics {
namespace io {

/// interface: PipelineExporter
class PipelineExporter : public libgraphics::io::PipelineObject  {
    public:
        virtual ~PipelineExporter() {}

        /// io methods
        virtual bool exportToStream(
            void* data,
            size_t length,
            libgraphics::Bitmap* toSave
        ) = 0;
        virtual bool exportToPath(
            const char* path,
            libgraphics::Bitmap* toSave
        ) = 0;

};
typedef libgraphics::io::PipelineObjectGroup<PipelineExporter> PipelineExporterGroup;

/// callbacks
typedef bool ( CallbackExportToStream )(
    void*, size_t, libgraphics::Bitmap*
);
typedef bool ( CallbackExportToPath )(
    const char*, libgraphics::Bitmap*
);

/// helpers
PipelineExporter* makeExporterFromCallbacks(
    const char* name,
    const char* extension,
    CallbackExportToStream* toStreamCallback,
    CallbackExportToPath* toPathCallback
);
PipelineExporter* makeExporterFromFunctions(
    const char* name,
    const char* extension,
    std::function< CallbackExportToStream >   toDataFn,
    std::function< CallbackExportToPath >   toPathFn
);

}
}
