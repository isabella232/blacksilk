#pragma once

#include <libgraphics/base.hpp>
#include <libgraphics/bitmap.hpp>
#include <libgraphics/io/pipelineobject.hpp>
#include <libgraphics/io/pipelineobjectgroup.hpp>

#include <libcommon/scopedptr.hpp>
#include <libcommon/atomics.hpp>
#include <libcommon/noncopyable.hpp>

#include <functional>
#include <vector>

namespace libgraphics {
namespace io {

/// interface: PipelineImporter
class PipelineImporter : public libgraphics::io::PipelineObject  {
    public:
        virtual ~PipelineImporter() {}

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

};
typedef libgraphics::io::PipelineObjectGroup<PipelineImporter> PipelineImporterGroup;

/// callbacks
typedef bool ( CallbackImportFromData )(
    void*, size_t, libgraphics::Bitmap*
);
typedef bool ( CallbackImportFromPath )(
    const char*, libgraphics::Bitmap*
);

/// helpers
PipelineImporter* makeImporterFromCallbacks(
    const char* name,
    const char* extension,
    CallbackImportFromData* fromDataCallback,
    CallbackImportFromPath* fromPathCallback
);
PipelineImporter* makeImporterFromFunctions(
    const char* name,
    const char* extension,
    std::function< CallbackImportFromData >   fromDataFn,
    std::function< CallbackImportFromPath >   fromPathFn
);

}
}
