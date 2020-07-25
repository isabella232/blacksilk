#pragma once

#include <assert.h>

#include <libgraphics/fxapi.hpp>

#include <libgraphics/backend/gl/gl_resource.hpp>
#include <libgraphics/backend/gl/gl_resourcepool.hpp>
#include <libgraphics/backend/gl/gl_texture.hpp>

namespace libgraphics {
namespace backend {
namespace gl {

class ImageObject : public fxapi::ApiImageObject {
    public:
        struct Private;

        ImageObject( fxapi::ApiBackendDevice* device );
        virtual ~ImageObject();

        /// own properties
        virtual size_t  tileCount() const;
        virtual size_t  countVerticalTiles() const;
        virtual size_t  countHorizontalTiles() const;

        virtual gl::Texture* tileTextureAtPosition( size_t x, size_t y );
        virtual gl::Texture* tileTexture( size_t nx, size_t ny );

        virtual gl::RenderTarget* tileRenderTargetAtPosition( size_t x, size_t y );
        virtual gl::RenderTarget* tileRenderTarget( size_t nx, size_t ny );

        virtual libgraphics::Rect32I tileArea( size_t nx, size_t ny );
        virtual libgraphics::Rect32I tileAreaAtPosition( size_t x, size_t y );

        virtual libgraphics::Rect32I logicalTileArea( size_t nx, size_t ny );
        virtual libgraphics::Rect32I logicalTileAreaAtPosition( size_t x, size_t y );

        virtual gl::TexturePool* texturePool();

        /// create
        virtual bool createFromBitmap(
            libgraphics::Bitmap* bitmap
        );
        virtual bool createFromBitmap(
            libgraphics::Bitmap* bitmap,
            libgraphics::Rect32I rect
        );
        virtual bool createFromBitmapInfo(
            const libgraphics::BitmapInfo& info
        );
        virtual bool createFromData(
            fxapi::EPixelFormat::t format,
            size_t width,
            size_t height,
            void* data
        );
        virtual bool create(
            fxapi::EPixelFormat::t format,
            size_t width,
            size_t height
        );

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
            libgraphics::Rect32I rect,
            size_t destX,
            size_t destY
        );
        virtual bool upload(
            void*  data
        );
        virtual bool upload(
            void*  data,
            libgraphics::Rect32I rect,
            size_t destX,
            size_t destY
        );

        /// copy
        virtual bool copy(
            ApiImageObject* source,
            libgraphics::Rect32I sourceRect,
            size_t destX,
            size_t destY
        );

        /// data management
        virtual void discardBuffers();
        virtual void synchronize();

        /// properties
        virtual int width();
        virtual int height();
        virtual fxapi::EPixelFormat::t format();
        virtual long backendId();

        bool    isMonoFormat() const;

        virtual bool empty();
    protected:
        std::shared_ptr<Private>   d;
};

class StreamLinedImageObject : public ImageObject {
    public:
        struct Private;

        StreamLinedImageObject( fxapi::ApiBackendDevice* device );
        virtual ~StreamLinedImageObject();

        /// own properties
        virtual size_t  tileCount() const;
        virtual size_t  countVerticalTiles() const;
        virtual size_t  countHorizontalTiles() const;

        virtual gl::Texture* tileTextureAtPosition( size_t x, size_t y );
        virtual gl::Texture* tileTexture( size_t nx, size_t ny );

        virtual gl::RenderTarget* tileRenderTargetAtPosition( size_t x, size_t y );
        virtual gl::RenderTarget* tileRenderTarget( size_t nx, size_t ny );

        virtual libgraphics::Rect32I tileArea( size_t nx, size_t ny );
        virtual libgraphics::Rect32I tileAreaAtPosition( size_t x, size_t y );

        virtual libgraphics::Rect32I logicalTileArea( size_t nx, size_t ny );
        virtual libgraphics::Rect32I logicalTileAreaAtPosition( size_t x, size_t y );

        virtual gl::TexturePool* texturePool();

        /// create
        virtual bool createFromBitmap(
            libgraphics::Bitmap* bitmap
        );
        virtual bool createFromBitmap(
            libgraphics::Bitmap* bitmap,
            libgraphics::Rect32I rect
        );
        virtual bool createFromBitmapInfo(
            const libgraphics::BitmapInfo& info
        );
        virtual bool createFromData(
            fxapi::EPixelFormat::t format,
            size_t width,
            size_t height,
            void* data
        );
        virtual bool create(
            fxapi::EPixelFormat::t format,
            size_t width,
            size_t height
        );

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
            libgraphics::Rect32I rect,
            size_t destX,
            size_t destY
        );
        virtual bool upload(
            void*  data
        );
        virtual bool upload(
            void*  data,
            libgraphics::Rect32I rect,
            size_t destX,
            size_t destY
        );

        /// copy
        virtual bool copy(
            ApiImageObject* source,
            libgraphics::Rect32I sourceRect,
            size_t destX,
            size_t destY
        );

        /// data management
        virtual void discardBuffers();
        virtual void synchronize();

        /// properties
        virtual int width();
        virtual int height();
        virtual fxapi::EPixelFormat::t format();
        virtual long backendId();

        bool    isMonoFormat() const;

        virtual bool empty();
    protected:
        std::shared_ptr<Private>    d;
};


}
}
}
