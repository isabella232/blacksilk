#pragma once

#include <libgraphics/utils/gl.hpp>
#include <libgraphics/backend/gl/gl_texture.hpp>
#include <libgraphics/backend/gl/gl_resource.hpp>
#include <libgraphics/backend/gl/gl_resourcepool.hpp>
#include <libgraphics/backend/gl/gl_effect.hpp>
#include <libgraphics/backend/gl/gl_rendertarget.hpp>

namespace libgraphics {
namespace backend {
namespace gl {

class PixelArray : public fxapi::ApiResource {
    public:
        struct Private;

        /// constr.
        PixelArray();
        PixelArray( libgraphics::fxapi::EPixelFormat::t format, size_t length );
        PixelArray( libgraphics::fxapi::EPixelFormat::t format, size_t length, const void* data );

        /// destr.
        virtual ~PixelArray();

        /// reset
        virtual void reset();
        void reset( libgraphics::fxapi::EPixelFormat::t format, size_t length );
        void reset( libgraphics::fxapi::EPixelFormat::t format, size_t length, const void* data );

        /// properties
        libgraphics::fxapi::EPixelFormat::t format() const;
        size_t length() const;
        bool empty() const;
        void* data() const;

        /// methods
        void upload( size_t offset, const void* source, size_t length );
        void retrieve( size_t offset, void* destination, size_t length );

        /// returns the current tex
        utils::GLTexture*    texture();
    protected:
        libcommon::PimplPtr<Private>   d;
};

class BackendDevice : public fxapi::ApiBackendDevice {
    public:
        struct Private;

        BackendDevice();
        virtual ~BackendDevice() {}

        /// methods
        virtual bool    initialize();
        virtual bool    shutdown();
        virtual void    synchronize(); /// synchronizes internal operations with the current thread

        /// constructors
        virtual fxapi::ApiResource* createTexture1D();
        virtual fxapi::ApiResource* createTexture1D( const fxapi::EPixelFormat::t& format, size_t length );
        virtual fxapi::ApiResource* createTexture1D( const fxapi::EPixelFormat::t& format, size_t length, void* data );
        virtual fxapi::ApiImageObject* createTexture2D();
        virtual fxapi::ApiImageObject* createTexture2D( const fxapi::EPixelFormat::t& format, size_t width, size_t height );
        virtual fxapi::ApiImageObject* createTexture2D( const fxapi::EPixelFormat::t& format, size_t width, size_t height, void* data );

        virtual fxapi::ApiImageObject* createStreamLinedTexture2D();
        virtual fxapi::ApiImageObject* createStreamLinedTexture2D( const fxapi::EPixelFormat::t& format, size_t width, size_t height );
        virtual fxapi::ApiImageObject* createStreamLinedTexture2D( const fxapi::EPixelFormat::t& format, size_t width, size_t height, void* data );

        /// destructors
        virtual bool destroyTexture1D( fxapi::ApiResource* resource );
        virtual bool destroyTexture2D( fxapi::ApiImageObject* resource );

        /// statistics
        virtual size_t  queryManagedMemoryConsumption();
        virtual size_t  queryBackendMemoryConsumption();

        /// properties
        virtual const char* name();
        virtual int backendId();

        virtual libcommon::SharedPtr<libgraphics::StdDynamicPoolAllocator>  allocator();
        virtual void setAllocator( const libcommon::SharedPtr<libgraphics::StdDynamicPoolAllocator>& newAllocator );

        /// common resource pools
        libgraphics::backend::gl::EffectPool* effectPool();
        libgraphics::backend::gl::RenderTargetPool* renderTargetPool();
        libgraphics::backend::gl::TexturePool* texturePool();

        /// Releases unused objects and returns the number of
        /// released objects.
        virtual size_t  cleanUp();

        ///
        enum class ERenderingMode : unsigned long {
            Realtime,     /// For preview rendering
            StreamLined   /// For rendering of really big images on
            /// gpus with insufficient memory
        };
        const ERenderingMode&   renderingMode();
        bool                    switchRenderingMode(
            const ERenderingMode& newRenderingMode
        );

    protected:
        libcommon::PimplPtr<Private>   d;
};

/// returns the global gl object
utils::GL*  globalCtx();
void        resetGlobalCtx();
void        initializeGlobalCtx();

}
}
}
