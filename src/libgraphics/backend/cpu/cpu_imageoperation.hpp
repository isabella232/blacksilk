#pragma once

#include <libgraphics/base.hpp>
#include <libgraphics/bitmap.hpp>
#include <libgraphics/fxapi.hpp>

#include <libcommon/atomics.hpp>

namespace libgraphics {
namespace backend {
namespace cpu {

class ImageOperation : fxapi::ApiImageOperation {
    public:
        struct Private;

        /// constr.
        ImageOperation( const char* name );

        /// destr.
        virtual ~ImageOperation();

        /// properties
        virtual const char* name();
        virtual bool working();
        virtual long backendId();
    private:
        std::shared_ptr<Private>   d;
};

}
}
}
