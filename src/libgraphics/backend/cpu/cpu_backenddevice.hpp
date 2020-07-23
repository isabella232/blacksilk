#pragma once

#include <vector>

#include <libgraphics/base.hpp>
#include <libgraphics/bitmap.hpp>
#include <libgraphics/fxapi.hpp>

#include <libcommon/atomics.hpp>
#include <libcommon/scopedptr.hpp>
#include <libcommon/sharedptr.hpp>

class QThreadPool;

namespace libgraphics {
namespace backend {
namespace cpu {

class DataRegion;
class DataRegionRef;
struct DataRegionEntry;

class PixelArray : public fxapi::ApiResource {
    public:
        struct Private;

        /// constr.
        PixelArray();
        PixelArray( libgraphics::fxapi::EPixelFormat::t format, size_t length );
        PixelArray( libgraphics::fxapi::EPixelFormat::t format, size_t length, const void* data );
        PixelArray( const PixelArray& rhs );

        PixelArray& operator = ( const PixelArray& rhs );

        /// destr.
        virtual ~PixelArray();

        /// reset
        virtual void reset();
        void reset( libgraphics::fxapi::EPixelFormat::t format, size_t length );
        void reset( libgraphics::fxapi::EPixelFormat::t format, size_t length, const void* data );
        void reset( const PixelArray& rhs );

        /// properties
        libgraphics::fxapi::EPixelFormat::t format() const;
        size_t length() const;
        bool empty() const;
        void* data() const;

        /// methods
        void upload( size_t offset, const void* source, size_t length );
        void retrieve( size_t offset, void* destination, size_t length );

        /// accessors
        unsigned char* get8Bit( size_t pos );
        unsigned short* get16Bit( size_t pos );
    protected:
        libcommon::PimplPtr<Private>   d;
};

class BackendDevice : public libgraphics::fxapi::ApiBackendDevice {
    public:
        struct Private;

        /// constr.
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

        /// destructors
        virtual bool destroyTexture1D( fxapi::ApiResource* resource );
        virtual bool destroyTexture2D( fxapi::ApiImageObject* resource );

        /// statistics
        virtual size_t  queryManagedMemoryConsumption();
        virtual size_t  queryBackendMemoryConsumption();

        size_t countTextureInstances() const;
        size_t countTexture1DInstances() const;
        size_t countTexture2DInstances() const;

        /// memory
        DataRegion*    allocDataRegion(
            size_t numberOfEntries,
            size_t entrySize
        );
        DataRegion*    newDataRegion(
            size_t numberOfEntries,
            size_t entrySize
        );
        DataRegion*    findDataRegion(
            size_t numberOfEntries,
            size_t entrySize
        );
        size_t countDataRegions() const;
        size_t countDataSize() const;
        void clearDataRegions();

        virtual size_t  cleanUp();

        /// properties
        virtual const char* name();
        QThreadPool* threadPool();

        virtual int backendId();

        virtual std::shared_ptr<libgraphics::StdDynamicPoolAllocator>  allocator();
        virtual void setAllocator( const std::shared_ptr<libgraphics::StdDynamicPoolAllocator>& newAllocator );
    protected:
        libcommon::PimplPtr<Private>   d;
};

struct DataRegionEntry {
        inline DataRegionEntry(
            void* _orig,
            size_t _offset
        ) : offset( _offset ), buffer( getRelativePtr( _orig ) ), used( 0 ) {}
        inline void* getRelativePtr( const void* buf ) {
            return ( void* )( ( const char* )buf + offset );
        }
        inline bool tryAcquire() {
            if( libcommon::atomics::exchange32( &used, 0, libcommon::getCurrentThreadId() ) ) {
                return true;
            }

            return false;
        }
        inline void acquire() {
            volatile bool acquired( false );

            while( !acquired ) {
                acquired = tryAcquire();
            }
        }

        const size_t  offset;
        const void*   buffer;
    private:
        libcommon::atomics::type32  used;
};

class DataRegion : public libcommon::INonCopyable {
    public:
        friend class BackendDevice;

        struct Private;

        const size_t count;
        const size_t length;
        const void*  buffer;

        inline DataRegion( const size_t numberOfEntries, const size_t entryLength ) :
            count( numberOfEntries ), length( entryLength ), buffer( alloc( count, length ) ) {}
        virtual ~DataRegion();

        DataRegionEntry* at( size_t index );
        DataRegionEntry* front();
        DataRegionEntry* back();
        DataRegionEntry* acquireUnused();
    private:
        void acquire();
        bool tryAcquire();
        bool isUsed();

        void reset();
        void* alloc( size_t _count, size_t _length );

        libcommon::PimplPtr<Private>   d;
};


}
}
}
