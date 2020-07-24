#pragma once

#include <assert.h>
#include <vector>

#include <libgraphics/fxapi.hpp>

#include <libgraphics/backend/gl/gl_resource.hpp>
#include <libgraphics/backend/gl/gl_resourcepool.hpp>

#include <libgraphics/utils/gl.hpp>

namespace libgraphics {
namespace backend {
namespace gl {

/// forward
class Texture;
class TexturePool;

/// extern
class RenderTarget;
class Effect;

/// class: Texture
class Texture : public backend::gl::Resource {
    public:
        friend class RenderTarget;
        friend class Effect;

        struct Private;

        /// constr. / destr.
        Texture();
        Texture( size_t size, fxapi::EPixelFormat::t format );
        Texture( size_t size, fxapi::EPixelFormat::t format, void* data );
        Texture( size_t width, size_t height, fxapi::EPixelFormat::t format );
        Texture( size_t width, size_t height, fxapi::EPixelFormat::t format, void* data );
        virtual ~Texture();

        /// create/reset
        virtual void reset();
        void reset( size_t size, fxapi::EPixelFormat::t format );
        void reset( size_t size, fxapi::EPixelFormat::t format, void* data );
        void reset( size_t width, size_t height, fxapi::EPixelFormat::t format );
        void reset( size_t width, size_t height, fxapi::EPixelFormat::t format, void* data );

        /// handling data

        /// downloading, retrieving data
        virtual bool retrieve(
            void* buffer
        );
        virtual bool retrieve(
            void* buffer,
            libgraphics::Rect32I rect
        );
        virtual bool retrieve(
            libgraphics::Bitmap* bitmap
        );
        virtual bool retrieve(
            libgraphics::Bitmap* bitmap,
            libgraphics::Rect32I rect
        );

        /// uploading data
        virtual bool upload(
            libgraphics::Bitmap* bitmap
        );
        virtual bool upload(
            libgraphics::Bitmap* bitmap,
            libgraphics::Rect32I rect
        );
        virtual bool upload(
            void*  data
        );
        virtual bool upload(
            void*  data,
            libgraphics::Rect32I rect
        );

        /// properties
        size_t  width() const;
        size_t  height() const;
        fxapi::EPixelFormat::t format() const;
        bool empty() const;

        /// from: backend::gl::Resource
        virtual size_t getGpuMemoryConsumption();
        virtual size_t getCpuMemoryConsumption();

        /// internals
        utils::GLTexture*   internalTexture() const;


        /// For debugging purposes.
        virtual bool tryAcquire();
        virtual void acquire();
        virtual bool release();
    protected:
        libcommon::PimplPtr<Private>   d;
};

/// class: TexturePool
class TexturePool : public backend::gl::GenericResourcePool<Texture> {
    public:
        TexturePool() {}
        virtual ~TexturePool() {}

        /// texture-specific pool methods

        /// acquire
        Texture* acquireTexture( size_t width, size_t height, fxapi::EPixelFormat::t format );
        Texture* tryAcquireTexture( size_t width, size_t height, fxapi::EPixelFormat::t format );

        /// find
        Texture* findTexture( size_t width, size_t height, fxapi::EPixelFormat::t format );

        /// count
        size_t  countTextures( size_t width, size_t height, fxapi::EPixelFormat::t format );
        size_t  countAvailableTextures( size_t width, size_t height, fxapi::EPixelFormat::t format );
        size_t  countAcquiredTextures( size_t width, size_t height, fxapi::EPixelFormat::t format );

        size_t  countTexturesWithFormat( fxapi::EPixelFormat::t format );
        size_t  countTexturesWithSize( size_t width, size_t height );
        size_t  countTexturesWithMinimalSize( size_t width, size_t height );

        /// create
        Texture* createTexture();
        Texture* createTexture( size_t size, fxapi::EPixelFormat::t format );
        Texture* createTexture( size_t size, fxapi::EPixelFormat::t format, void* data );
        Texture* createTexture( size_t width, size_t height, fxapi::EPixelFormat::t format );
        Texture* createTexture( size_t width, size_t height, fxapi::EPixelFormat::t format, void* data );


        Texture* createAndAcquireTexture();
        Texture* createAndAcquireTexture( size_t size, fxapi::EPixelFormat::t format );
        Texture* createAndAcquireTexture( size_t size, fxapi::EPixelFormat::t format, void* data );
        Texture* createAndAcquireTexture( size_t width, size_t height, fxapi::EPixelFormat::t format );
        Texture* createAndAcquireTexture( size_t width, size_t height, fxapi::EPixelFormat::t format, void* data );
};
}
}
}
