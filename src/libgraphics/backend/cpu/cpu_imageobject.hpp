#pragma once

#include <libgraphics/base.hpp>
#include <libgraphics/bitmap.hpp>
#include <libgraphics/fxapi.hpp>

#include <libcommon/atomics.hpp>
#include <libcommon/pimpl.hpp>

#include <libgraphics/allocator.hpp>

namespace libgraphics {
namespace backend {
namespace cpu {

class ImageObject : public libgraphics::fxapi::ApiImageObject {
    public:
        struct Private;

        ImageObject();
        explicit ImageObject( libgraphics::Bitmap* bitmap );
        ImageObject( libgraphics::Bitmap* bitmap, libgraphics::Rect32I rect );
        ImageObject( const libgraphics::BitmapInfo& info );
        ImageObject( fxapi::EPixelFormat::t format, size_t width, size_t height, void* data );
        ImageObject( fxapi::EPixelFormat::t format, size_t width, size_t height );

        explicit ImageObject( libgraphics::StdDynamicPoolAllocator* allocator );
        ImageObject( libgraphics::StdDynamicPoolAllocator* allocator, const libgraphics::BitmapInfo& info );
        ImageObject( libgraphics::StdDynamicPoolAllocator* allocator, fxapi::EPixelFormat::t format, size_t width, size_t height, void* data );
        ImageObject( libgraphics::StdDynamicPoolAllocator* allocator, fxapi::EPixelFormat::t format, size_t width, size_t height );

        /// destr.
        virtual ~ImageObject();

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
        virtual int  width();
        virtual int  height();
        virtual fxapi::EPixelFormat::t format();
        virtual long backendId();
        void* data();

        virtual bool empty();

        /// allocator stuff
        bool containsAllocator() const;
        libgraphics::StdDynamicPoolAllocator*   allocator() const;
        bool assignAllocator(
            libgraphics::StdDynamicPoolAllocator* _allocator
        );
        void clearAllocator();
    private:
        libcommon::PimplPtr<Private> d;
};

}
}
}
