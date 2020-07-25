#pragma once

#include <libgraphics/bitmap.hpp>
#include <libgraphics/allocator.hpp>

#include <memory>

#define FXAPI_BACKEND_OPENGL        0x10
#define FXAPI_BACKEND_CPU           0x20

namespace libgraphics {

/// represents the abstract api for different
/// types of imaging renderers
namespace fxapi {

/// texel access
namespace ETexelAccessBehavior {
enum t {
    Mirror, /// Mirrors the texture at borders
    Clone
};
}

/// plain pixel-formats
namespace EPixelFormat {
enum t {
    /// unsigned integer
    /// formats
    RGB8,
    RGB16,
    RGBA8,
    RGBA16,
    Mono8,
    Mono16,

    /// signed integer
    /// formats
    RGB16S,
    RGBA16S,
    Mono16S,

    /// signed floating point
    /// formats
    RGB32F,
    RGBA32F,
    Mono32F,

    /// invalid formats
    Empty
};

/// returns the minimal possible pixel
/// value
template < EPixelFormat::t val >
struct GetPixelMin {
    static const int value = 0;
};

template <>
struct GetPixelMin< EPixelFormat::RGB16S > {
    static const int value = -32767;
};
template <>
struct GetPixelMin< EPixelFormat::Mono16S > {
    static const int value = -32767;
};
template <>
struct GetPixelMin< EPixelFormat::RGBA16S > {
    static const int value = -32767;
};

/// returns the maximal possible
/// pixel value
template < EPixelFormat::t val >
struct GetPixelMax;
template <>
struct GetPixelMax<EPixelFormat::RGB8> {
    static const int value = 255;
};
template <>
struct GetPixelMax<EPixelFormat::RGB16> {
    static const int value = 65535;
};
template <>
struct GetPixelMax<EPixelFormat::RGB16S> {
    static const int value = 32767;
};
template <>
struct GetPixelMax<EPixelFormat::RGB32F> {
    static const int value = 1;
};
template <>
struct GetPixelMax<EPixelFormat::RGBA8> {
    static const int value = 255;
};
template <>
struct GetPixelMax<EPixelFormat::RGBA16> {
    static const int value = 65535;
};
template <>
struct GetPixelMax<EPixelFormat::RGBA16S> {
    static const int value = 32767;
};
template <>
struct GetPixelMax<EPixelFormat::RGBA32F> {
    static const int value = 1;
};
template <>
struct GetPixelMax<EPixelFormat::Mono8> {
    static const int value = 255;
};
template <>
struct GetPixelMax<EPixelFormat::Mono16> {
    static const int value = 65535;
};
template <>
struct GetPixelMax<EPixelFormat::Mono16S> {
    static const int value = 32767;
};
template <>
struct GetPixelMax<EPixelFormat::Mono32F> {
    static const int value = 1;
};

/// returns the pixel size
template < EPixelFormat::t val >
struct GetPixelSize;
template <>
struct GetPixelSize<t::Mono8> {
    static const size_t value = sizeof( unsigned char ) * 1;
};
template <>
struct GetPixelSize<t::Mono16> {
    static const size_t value = sizeof( unsigned short ) * 1;
};
template <>
struct GetPixelSize<t::RGB8> {
    static const size_t value = sizeof( unsigned char ) * 3;
};
template <>
struct GetPixelSize<t::RGBA8> {
    static const size_t value = sizeof( unsigned char ) * 4;
};
template <>
struct GetPixelSize<t::RGB16> {
    static const size_t value = sizeof( unsigned short ) * 3;
};
template <>
struct GetPixelSize<t::RGBA16> {
    static const size_t value = sizeof( unsigned short ) * 4;
};
template <>
struct GetPixelSize<t::RGB16S> {
    static const size_t value = sizeof( short ) * 3;
};
template <>
struct GetPixelSize<t::RGBA16S> {
    static const size_t value = sizeof( short ) * 4;
};
template <>
struct GetPixelSize<t::Mono16S> {
    static const size_t value = sizeof( short );
};
template <>
struct GetPixelSize<t::RGB32F> {
    static const size_t value = sizeof( float ) * 3;
};
template <>
struct GetPixelSize<t::RGBA32F> {
    static const size_t value = sizeof( float ) * 4;
};
template <>
struct GetPixelSize<t::Mono32F> {
    static const size_t value = sizeof( float );
};

static inline const char* toString( t val ) {
    switch( val ) {

        /// unsigned integer
        /// formats
        case t::Mono8: return "Mono8";

        case t::Mono16: return "Mono16";

        case t::RGB8: return "RGB8";

        case t::RGBA8: return "RGBA8";

        case t::RGB16: return "RGB16";

        case t::RGBA16: return "RGBA16";

        /// signed integer
        /// formats
        case t::Mono16S: return "Mono16S";

        case t::RGB16S: return "RGB16S";

        case t::RGBA16S: return "RGBA16S";

        /// signed floating point
        /// formats
        case t::Mono32F: return "Mono32F";

        case t::RGB32F: return "RGB32F";

        case t::RGBA32F: return "RGBA32F";

        /// No chosen format
        case t::Empty: return "Empty";

        /// Unhandled
        default:
            assert( false );
            return "Empty";
    }
}

static inline const bool hasAlphaPlane( t val ) {
    switch( val ) {

        /// unsigned integer
        /// formats
        case t::Mono8:
        case t::Mono16:
        case t::RGB8:
        case t::RGB16:
        case t::Mono16S:
        case t::RGB16S:
        case t::Mono32F:
        case t::RGB32F:
            return false;

        case t::RGBA8:
        case t::RGBA16:
        case t::RGBA16S:
        case t::RGBA32F:
            return true;


        /// signed floating point
        /// formats
        default:
            assert( false );
            return false;
    }
}

static inline const int getPixelMin( t val ) {
    switch( val ) {

        /// unsigned integer
        /// formats
        case t::Mono8: return GetPixelMin<t::Mono8>::value;

        case t::Mono16: return GetPixelMin<t::Mono16>::value;

        case t::RGB8: return GetPixelMin<t::RGB8>::value;

        case t::RGBA8: return GetPixelMin<t::RGBA8>::value;

        case t::RGB16: return GetPixelMin<t::RGB16>::value;

        case t::RGBA16: return GetPixelMin<t::RGBA16>::value;

        /// signed integer
        /// formats
        case t::Mono16S: return GetPixelMin<t::Mono16S>::value;

        case t::RGB16S: return GetPixelMin<t::RGB16S>::value;

        case t::RGBA16S: return GetPixelMin<t::RGBA16S>::value;

        /// signed floating point
        /// formats
        case t::Mono32F: return GetPixelMin<t::Mono32F>::value;

        case t::RGB32F: return GetPixelMin<t::RGB32F>::value;

        case t::RGBA32F: return GetPixelMin<t::RGBA32F>::value;


        default:
            assert( false );
            return 0;
    }
}
static inline const int getPixelMax( t val ) {
    switch( val ) {

        /// unsigned integer
        /// formats
        case t::Mono8: return GetPixelMax<t::Mono8>::value;

        case t::Mono16: return GetPixelMax<t::Mono16>::value;

        case t::RGB8: return GetPixelMax<t::RGB8>::value;

        case t::RGBA8: return GetPixelMax<t::RGBA8>::value;

        case t::RGB16: return GetPixelMax<t::RGB16>::value;

        case t::RGBA16: return GetPixelMax<t::RGBA16>::value;

        /// signed integer
        /// formats
        case t::Mono16S: return GetPixelMax<t::Mono16S>::value;

        case t::RGB16S: return GetPixelMax<t::RGB16S>::value;

        case t::RGBA16S: return GetPixelMax<t::RGBA16S>::value;

        /// signed floating point
        /// formats
        case t::Mono32F: return GetPixelMax<t::Mono32F>::value;

        case t::RGB32F: return GetPixelMax<t::RGB32F>::value;

        case t::RGBA32F: return GetPixelMax<t::RGBA32F>::value;


        default:
            assert( false );
            return 0;
    }
}

static inline const size_t getPixelSize( t val ) {
    switch( val ) {

        /// unsigned integer
        /// formats
        case t::Mono8: return GetPixelSize<t::Mono8>::value;

        case t::Mono16: return GetPixelSize<t::Mono16>::value;

        case t::RGB8: return GetPixelSize<t::RGB8>::value;

        case t::RGBA8: return GetPixelSize<t::RGBA8>::value;

        case t::RGB16: return GetPixelSize<t::RGB16>::value;

        case t::RGBA16: return GetPixelSize<t::RGBA16>::value;

        /// signed integer
        /// formats
        case t::Mono16S: return GetPixelSize<t::Mono16S>::value;

        case t::RGB16S: return GetPixelSize<t::RGB16S>::value;

        case t::RGBA16S: return GetPixelSize<t::RGBA16S>::value;

        /// signed floating point
        /// formats
        case t::Mono32F: return GetPixelSize<t::Mono32F>::value;

        case t::RGB32F: return GetPixelSize<t::RGB32F>::value;

        case t::RGBA32F: return GetPixelSize<t::RGBA32F>::value;

        default:
            assert( false );
            return 0;
    }
}
static inline const size_t getChannelCount( t val ) {
    switch( val ) {

        /// unsigned integer
        /// formats
        case t::Mono8: return 1;

        case t::Mono16: return 1;

        case t::RGB8: return 3;

        case t::RGBA8: return 4;

        case t::RGB16: return 3;

        case t::RGBA16: return 4;

        /// signed integer
        /// formats
        case t::Mono16S: return 1;

        case t::RGB16S: return 3;

        case t::RGBA16S: return 4;

        /// signed floating point
        /// formats
        case t::Mono32F: return 1;

        case t::RGB32F: return 3;

        case t::RGBA32F: return 4;

        default:
            assert( false );
            return 0;
    }
}
static inline const size_t getChannelSize( t val ) {
    return getPixelSize( val ) / getChannelCount( val );
}

static inline const size_t getPlaneByteSize( t val, size_t width, size_t height ) {
    return ( width * height * getChannelSize( val ) );
}
static inline const size_t getPlaneByteSize( t val, const libgraphics::Rect32I& dim ) {
    return getPlaneByteSize(
               val,
               dim.width,
               dim.height
           );
}
static inline double getPlaneMegaPixels( t /*val*/, size_t width, size_t height ) {
    return ( float )( width * height ) / ( 1000.0f * 1000.0f );
}
static inline double getPlaneMegaPixels( t val, const libgraphics::Rect32I& dim ) {
    return getPlaneMegaPixels(
               val,
               dim.width,
               dim.height
           );
}

static inline bool isFloatingPointFormat( const EPixelFormat::t& format ) {
    switch( format ) {
        case t::Mono32F:
        case t::RGB32F:
        case t::RGBA32F:
            return true;

        default:
            return false;
    }
}
static inline bool isUnsignedFormat( const EPixelFormat::t& format ) {
    switch( format ) {
        case t::Mono16S:
        case t::Mono32F:
        case t::RGB16S:
        case t::RGB32F:
        case t::RGBA16S:
        case t::RGBA32F:
            return true;

        default:
            return false;
    }
}
static inline bool isSignedFormat( const EPixelFormat::t& format ) {
    switch( format ) {
        case t::Mono8:
        case t::Mono16:
        case t::RGB8:
        case t::RGB16:
        case t::RGBA8:
        case t::RGBA16:
            return true;

        default:
            return false;
    }
}
static inline const EPixelFormat::t getAssociatedAlphaFormat( const EPixelFormat::t& format ) {
    switch( format ) {
        case t::RGB8:
            return t::RGBA8;

        case t::RGB16:
            return t::RGBA16;

        case t::RGB16S:
            return t::RGBA16S;

        case t::RGB32F:
            return t::RGBA32F;

        default:
            break;
    }

    return hasAlphaPlane( format ) ? format : EPixelFormat::Empty;
}
static inline const EPixelFormat::t getCompatibleSignedFormat( const EPixelFormat::t& unsigned_format ) {
    switch( unsigned_format ) {
        case t::Mono8:
            return t::Mono16S;

        case t::Mono16:
            return t::Mono32F;

        case t::RGB8:
            return t::RGB16S;

        case t::RGB16:
            return t::RGB32F;

        case t::RGBA8:
            return t::RGBA16S;

        case t::RGBA16:
            return t::RGBA32F;

        case t::Mono16S:
        case t::Mono32F:
        case t::RGB16S:
        case t::RGB32F:
        case t::RGBA16S:
        case t::RGBA32F:
            return unsigned_format; /** warning: format already signed **/

        default:
            assert( false );
            break;
    }

    return EPixelFormat::Empty;
}
}

class ApiBackendDevice;
class ApiImageObject;
class ApiImageOperation;
class ApiResource;

/// basic interface for rendering
/// images.

/// interface: ApiBackendDevice
class ApiBackendDevice {
    public:
        virtual ~ApiBackendDevice() {}

        /// methods
        virtual bool    initialize() = 0;
        virtual bool    shutdown() = 0;
        virtual void    synchronize() = 0;

        /// constructors
        virtual ApiResource* createTexture1D() = 0;
        virtual ApiResource* createTexture1D( const EPixelFormat::t& format, size_t length ) = 0;
        virtual ApiResource* createTexture1D( const EPixelFormat::t& format, size_t length, void* data ) = 0;
        virtual ApiImageObject* createTexture2D() = 0;
        virtual ApiImageObject* createTexture2D( const EPixelFormat::t& format, size_t width, size_t height ) = 0;
        virtual ApiImageObject* createTexture2D( const EPixelFormat::t& format, size_t width, size_t height, void* data ) = 0;

        /// destructors
        virtual bool destroyTexture1D( ApiResource* resource ) = 0;
        virtual bool destroyTexture2D( ApiImageObject* resource ) = 0;

        /// statistics
        virtual size_t  queryManagedMemoryConsumption() = 0;
        virtual size_t  queryBackendMemoryConsumption() = 0;

        /// properties
        virtual const char* name() = 0;
        virtual int backendId() = 0;

        /// Releases unused objects and returns the number of
        /// released objects.
        virtual size_t  cleanUp() = 0;

        virtual std::shared_ptr<libgraphics::StdDynamicPoolAllocator>  allocator() = 0;
        virtual void setAllocator( const std::shared_ptr<libgraphics::StdDynamicPoolAllocator>& newAllocator ) = 0;
};

/// interface: ApiImageOperation
class ApiImageOperation {
    public:
        virtual ~ApiImageOperation() {}
};

/// interface: ApiResource
class ApiResource {
    public:
        /// destr.
        virtual ~ApiResource() {}

        /// methods
        virtual void reset() = 0;
};

/// interface: ApiImageObject
class ApiImageObject {
    public:
        /// destr.
        virtual ~ApiImageObject() {}

        /// create
        virtual bool createFromBitmap(
            libgraphics::Bitmap* bitmap
        ) = 0;
        virtual bool createFromBitmap(
            libgraphics::Bitmap* bitmap,
            libgraphics::Rect32I rect
        ) = 0;
        virtual bool createFromBitmapInfo(
            const libgraphics::BitmapInfo& info
        ) = 0;
        virtual bool createFromData(
            EPixelFormat::t format,
            size_t width,
            size_t height,
            void* data
        ) = 0;
        virtual bool create(
            EPixelFormat::t format,
            size_t width,
            size_t height
        ) = 0;

        /// downloading, retrieving data
        virtual bool retrieve(
            void* buffer
        ) = 0;
        virtual bool retrieve(
            void* buffer,
            libgraphics::Rect32I rect
        ) = 0;
        virtual bool retrieve(
            libgraphics::Bitmap* bitmap
        ) = 0;
        virtual bool retrieve(
            libgraphics::Bitmap* bitmap,
            libgraphics::Rect32I rect
        ) = 0;

        /// uploading data
        virtual bool upload(
            libgraphics::Bitmap* bitmap
        ) = 0;
        virtual bool upload(
            libgraphics::Bitmap* bitmap,
            libgraphics::Rect32I rect,
            size_t destX,
            size_t destY
        ) = 0;
        virtual bool upload(
            void*  data
        ) = 0;
        virtual bool upload(
            void*  data,
            libgraphics::Rect32I rect,
            size_t destX,
            size_t destY
        ) = 0;

        /// copy
        virtual bool copy(
            ApiImageObject* source,
            libgraphics::Rect32I sourceRect,
            size_t destX,
            size_t destY
        ) = 0;

        /// data management
        virtual void discardBuffers() = 0;
        virtual void synchronize() = 0;

        /// properties
        virtual int  width() = 0;
        virtual int  height() = 0;
        virtual EPixelFormat::t format() = 0;
        virtual long backendId() = 0;

        virtual bool empty() = 0;
};

class ApiScopedImgRef : libcommon::INonCopyable {
    public:
        ApiScopedImgRef(
            ApiBackendDevice* dev,
            ApiImageObject* obj
        );
        ApiScopedImgRef(
            ApiBackendDevice* dev
        );
        ApiScopedImgRef(
            ApiBackendDevice* dev,
            const EPixelFormat::t& format,
            size_t width,
            size_t height
        );
        ApiScopedImgRef(
            ApiBackendDevice* dev,
            const EPixelFormat::t& format,
            size_t width,
            size_t height,
            void* data
        );
        virtual ~ApiScopedImgRef();

        inline ApiImageObject* operator -> () { return m_ImageObject; }
        inline const ApiImageObject* operator -> () const { return m_ImageObject; }
        inline ApiImageObject* operator * () { return m_ImageObject; }
        inline const ApiImageObject* operator * () const { return m_ImageObject; }

        inline bool empty() const { return ( m_ImageObject != nullptr ); }
        inline operator bool () const { return !empty(); }
        inline operator ApiImageObject* () const { return m_ImageObject; }

        inline ApiBackendDevice* device() const { return m_ImageBackend; }
        inline ApiImageObject* img() const { return m_ImageObject; }
    private:
        ApiImageObject*     m_ImageObject;
        ApiBackendDevice*   m_ImageBackend;
};


void initialize();
void setupOpenGLBackend();
void setupCPUBackend();

ApiBackendDevice*   getOpenGLBackend();
ApiBackendDevice*   getCPUBackend();

}
}
