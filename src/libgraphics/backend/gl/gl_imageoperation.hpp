#pragma once

#include <libgraphics/fxapi.hpp>

namespace libgraphics {
namespace backend {
namespace gl {

class Texture;
class Effect;

class ImageOperation : public fxapi::ApiImageOperation {
    public:
        virtual ~ImageOperation() {}

        virtual void submitParameters( libgraphics::fxapi::ApiBackendDevice* device, libgraphics::fxapi::ApiImageObject* destination, libgraphics::fxapi::ApiImageObject* source, libgraphics::Rect32I area, gl::Texture* texture,
                                       size_t tx, size_t ty ) = 0;
        virtual void submitImageMergeParameters( libgraphics::fxapi::ApiBackendDevice* device, libgraphics::fxapi::ApiImageObject* destination, libgraphics::fxapi::ApiImageObject* source, libgraphics::Rect32I area,
                gl::Texture* texture, gl::Texture* otherTexture,
                size_t tx, size_t ty ) = 0;

        virtual Effect* getEffect() = 0;
        virtual bool initialize( libgraphics::fxapi::ApiBackendDevice* device ) = 0;
};

}
}
}
