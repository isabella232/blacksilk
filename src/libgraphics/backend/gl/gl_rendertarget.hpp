#pragma once

#include <assert.h>

#include <libcommon/sharedptr.hpp>
#include <libcommon/scopedptr.hpp>

#include <libgraphics/fxapi.hpp>
#include <libgraphics/backend/gl/gl_resource.hpp>
#include <libgraphics/backend/gl/gl_resourcepool.hpp>

#include <libgraphics/utils/gl.hpp>

namespace libgraphics {
namespace backend {
namespace gl {

/// forward
class RenderTarget;
class RenderTargetPool;

/// extern
class Texture;
class TexturePool;

/// class: RenderTarget
class RenderTarget : public backend::gl::Resource {
    public:
        struct Private;

        /// constr. / destr.
        RenderTarget();
        virtual ~RenderTarget();

        /// methods
        virtual void reset();

        bool attachTexture( Texture* texture );
        void detachTexture();
        bool complete() const;

        Texture* attachedTexture() const;

        /// properties
        size_t  width() const;
        size_t  height() const;
        fxapi::EPixelFormat::t format() const;
        bool empty() const;

        /// from: backend::gl::Resource
        virtual size_t getGpuMemoryConsumption();
        virtual size_t getCpuMemoryConsumption();

        /// internals
        utils::GLFrameBuffer*   internalRenderTarget();


        /// For debugging purposes.
        virtual bool tryAcquire();
        virtual void acquire();
        virtual bool release();
    protected:
        libcommon::PimplPtr<Private>   d;
};

/// class: RenderTargetPool
class RenderTargetPool : public backend::gl::GenericResourcePool<RenderTarget> {
    public:
        /// constr. / destr.
        RenderTargetPool();
        virtual ~RenderTargetPool() {}

        /// methods
        RenderTarget*   createRenderTarget();
        RenderTarget*   createAndAcquireRenderTarget();
        RenderTarget*   createAndAcquireRenderTarget( TexturePool* associatedPool, size_t width, size_t height, fxapi::EPixelFormat::t format );

        RenderTarget*   tryAcquireRenderTarget( size_t width, size_t height, fxapi::EPixelFormat::t format );
        RenderTarget*   acquireRenderTarget( size_t width, size_t height, fxapi::EPixelFormat::t format );

};

}
}
}
