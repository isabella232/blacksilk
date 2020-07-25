#include <libgraphics/backend/cpu/cpu_backenddevice.hpp>
#include <libgraphics/backend/cpu/cpu_imageobject.hpp>

#include <QThreadPool>
#include <QDebug>

namespace libgraphics {
namespace backend {
namespace cpu {

/// PixelArray
struct PixelArray::Private {
    void*                       data;
    size_t                      length;
    fxapi::EPixelFormat::t      format;

    Private() : data( nullptr ), length( 0 ),
        format( fxapi::EPixelFormat::Empty ) {}
};

PixelArray::PixelArray() : d( new Private() ) {}

PixelArray::PixelArray( libgraphics::fxapi::EPixelFormat::t format, size_t length ) : d( new Private() ) {
    reset(
        format,
        length
    );
}

PixelArray::PixelArray( libgraphics::fxapi::EPixelFormat::t format, size_t length, const void* data ) : d( new Private() ) {
    reset(
        format,
        length,
        data
    );
}

PixelArray::PixelArray( const PixelArray& rhs ) : d( new Private() ) {
    reset(
        rhs
    );
}

PixelArray& PixelArray::operator = ( const PixelArray& rhs ) {
    reset(
        rhs
    );

    return *this;
}

PixelArray::~PixelArray() {
    reset();
}

void PixelArray::reset() {
    if( d->length != 0 ) {
        delete []( char* )d->data;

        d->data = nullptr;
        d->format = fxapi::EPixelFormat::Empty;
        d->length = 0;
    }
}

void PixelArray::reset( libgraphics::fxapi::EPixelFormat::t format, size_t length ) {
    assert( length > 0 );

    reset(
        format,
        length,
        nullptr
    );
}

void PixelArray::reset( libgraphics::fxapi::EPixelFormat::t format, size_t length, const void* data ) {
    reset();

    size_t pixelLen = libgraphics::fxapi::EPixelFormat::getPixelSize(
                          format
                      );

    d->data = ( void* ) new char[ pixelLen * length ];
    d->format = format;
    d->length = length;

    memcpy(
        d->data,
        ( const void* )data,
        pixelLen * length
    );
}

void PixelArray::reset( const backend::cpu::PixelArray& rhs ) {
    reset(
        rhs.d->format,
        rhs.d->length,
        rhs.d->data
    );
}

libgraphics::fxapi::EPixelFormat::t PixelArray::format() const {
    return d->format;
}

size_t PixelArray::length() const {
    return d->length;
}

bool PixelArray::empty() const {
    return ( d->data == nullptr );
}

void PixelArray::upload( size_t offset, const void* source, size_t length ) {
    assert( d->length >= offset + length );

    memcpy(
        ( void* )( ( char* )d->data + offset ),
        source,
        length
    );
}

void PixelArray::retrieve( size_t offset, void* destination, size_t length ) {
    assert( d->length >= offset + length );

    memcpy(
        destination,
        ( void* )( ( char* )d->data + offset ),
        length
    );
}

void* PixelArray::data() const {
    return d->data;
}

unsigned char* PixelArray::get8Bit( size_t pos ) {
    assert( !empty() );

    size_t pixelLen = libgraphics::fxapi::EPixelFormat::getPixelSize(
                          d->format
                      );

    return ( ( unsigned char* )d->data + ( pixelLen * pos ) );
}

unsigned short* PixelArray::get16Bit( size_t pos ) {
    assert( !empty() );

    size_t pixelLen = libgraphics::fxapi::EPixelFormat::getPixelSize(
                          d->format
                      );

    return ( unsigned short* )( ( char* )d->data + ( pixelLen * pos ) );
}

/// DataRegion
struct DataRegion::Private {
    std::vector<std::unique_ptr<DataRegionEntry> > entries;
    libcommon::atomics::type32  used;

    Private( size_t count, size_t length, void* buffer ) : used( 0 ) {
        for( size_t i = 0; count > i; ++i ) {
            entries.push_back( std::unique_ptr<DataRegionEntry>( new DataRegionEntry( buffer, length * i ) ) );
        }
    }

    inline bool tryAcquire() {
        auto threadId = libcommon::getCurrentThreadId();

        if( ( libcommon::atomics::exchange32( &used, 0, threadId ) == 0 ) ||
                libcommon::atomics::equal32( &used, threadId ) ) {
            return true;
        }

        return false;
    }
    inline void acquire() {
        volatile bool acquired( tryAcquire() );

        while( !acquired ) {
            acquired = tryAcquire();

            libcommon::sleep( 1 );
        }
    }
};

DataRegion::~DataRegion() {
    reset();
}

DataRegionEntry* DataRegion::at( size_t index ) {
    return d->entries.at( index ).get();
}

DataRegionEntry* DataRegion::front() {
    return d->entries.front().get();
}

DataRegionEntry* DataRegion::back() {
    return d->entries.back().get();
}

DataRegionEntry* DataRegion::acquireUnused() {
    for( auto it = d->entries.begin(); it != d->entries.end(); ++it ) {
        if( ( *it )->tryAcquire() ) {
            return ( *it ).get();
        }
    }

    return nullptr;
}

void DataRegion::reset() {
    delete []( char* )this->buffer;

    d.reset();
}

void* DataRegion::alloc( size_t _count, size_t _length ) {
    void* buf = ( void* )( new char[_count * _length] );

    this->d.reset(
        new Private( _count, _length, buf )
    );

    return buf;
}

void DataRegion::acquire() {
    return d->acquire();
}

bool DataRegion::tryAcquire() {
    return d->tryAcquire();
}

bool DataRegion::isUsed() {
    return !libcommon::atomics::equal32( &d->used, 0 );
}


/// BackendDevice
struct BackendDevice::Private {
    std::vector< std::unique_ptr<DataRegion> >         dataRegions;
    std::vector< std::unique_ptr<PixelArray> >         pixelArrays;
    std::vector< std::unique_ptr<ImageObject> >        imageObjects;
    std::shared_ptr<libgraphics::StdDynamicPoolAllocator>   allocator;
    QThreadPool threadPool;

    Private() : allocator( new libgraphics::StdDynamicPoolAllocator() ) {}
    ~Private() {
        dataRegions.clear();
        pixelArrays.clear();
        imageObjects.clear();
    }
};

BackendDevice::BackendDevice() : d( new Private() ) {
    this->threadPool()->setMaxThreadCount( 2 );
}

bool    BackendDevice::initialize() {
    if( d.get() == nullptr ) {
        assert( false );
        return false;
    }

    const auto idealThreadCount = std::max( 1, QThread::idealThreadCount() - 1 );
    d->threadPool.setMaxThreadCount( idealThreadCount );

    return true;
}

bool    BackendDevice::shutdown() {
    return true;
}

void    BackendDevice::synchronize() {
    ( void ) this->d->threadPool.waitForDone();
}

fxapi::ApiResource* BackendDevice::createTexture1D() {
    return ( fxapi::ApiResource* ) new libgraphics::backend::cpu::PixelArray();
}

fxapi::ApiResource* BackendDevice::createTexture1D( const fxapi::EPixelFormat::t& format, size_t length ) {
    return ( fxapi::ApiResource* ) new libgraphics::backend::cpu::PixelArray(
               format,
               length
           );
}

fxapi::ApiResource* BackendDevice::createTexture1D( const fxapi::EPixelFormat::t& format, size_t length, void* data ) {
    return ( fxapi::ApiResource* ) new libgraphics::backend::cpu::PixelArray(
               format,
               length,
               data
           );
}


///\todo clean up image object management!11
fxapi::ApiImageObject* BackendDevice::createTexture2D() {
    if( !this->allocator() ) {
        fxapi::ApiImageObject* obj = ( fxapi::ApiImageObject* ) new libgraphics::backend::cpu::ImageObject();
        this->d->imageObjects.push_back( std::unique_ptr<ImageObject>( ( ImageObject* )obj ) );

        return obj;
    } else {
        fxapi::ApiImageObject* obj = ( fxapi::ApiImageObject* ) new libgraphics::backend::cpu::ImageObject( d->allocator.get() );
        this->d->imageObjects.push_back( std::unique_ptr<ImageObject>( ( ImageObject* )obj ) );

        return obj;
    }

}

fxapi::ApiImageObject* BackendDevice::createTexture2D( const fxapi::EPixelFormat::t& format, size_t width, size_t height ) {
    if( !this->allocator() ) {
        fxapi::ApiImageObject* obj = ( fxapi::ApiImageObject* ) new libgraphics::backend::cpu::ImageObject(
                                         format,
                                         width,
                                         height
                                     );
        this->d->imageObjects.push_back( std::unique_ptr<ImageObject>( ( ImageObject* )obj ) );

        return obj;
    } else {
        fxapi::ApiImageObject* obj = ( fxapi::ApiImageObject* ) new libgraphics::backend::cpu::ImageObject(
                                         d->allocator.get(),
                                         format,
                                         width,
                                         height
                                     );
        this->d->imageObjects.push_back( std::unique_ptr<ImageObject>( ( ImageObject* )obj ) );

        return obj;
    }
}

fxapi::ApiImageObject* BackendDevice::createTexture2D( const fxapi::EPixelFormat::t& format, size_t width, size_t height, void* data ) {
    if( !this->allocator() ) {
        fxapi::ApiImageObject* obj = ( fxapi::ApiImageObject* ) new libgraphics::backend::cpu::ImageObject(
                                         format,
                                         width,
                                         height,
                                         data
                                     );
        this->d->imageObjects.push_back( std::unique_ptr<ImageObject>( ( ImageObject* )obj ) );

        return obj;
    } else {
        fxapi::ApiImageObject* obj = ( fxapi::ApiImageObject* ) new libgraphics::backend::cpu::ImageObject(
                                         d->allocator.get(),
                                         format,
                                         width,
                                         height,
                                         data
                                     );
        this->d->imageObjects.push_back( std::unique_ptr<ImageObject>( ( ImageObject* )obj ) );

        return obj;
    }
}

bool BackendDevice::destroyTexture1D( fxapi::ApiResource* resource ) {
    if( resource != nullptr ) {
        for( auto it = d->pixelArrays.begin(); it != d->pixelArrays.end(); ++it ) {
            if( ( *it ).get() == resource ) {
                d->pixelArrays.erase( it );
                return true;
            }
        }
    }

    return false;
}

bool BackendDevice::destroyTexture2D( fxapi::ApiImageObject* resource ) {
    if( resource != nullptr ) {
        for( auto it = d->imageObjects.begin(); it != d->imageObjects.end(); ++it ) {
            if( ( *it ).get() == resource ) {
                d->imageObjects.erase( it );
                return true;
            }
        }
    }

    return false;
}


size_t  BackendDevice::queryManagedMemoryConsumption() {
    size_t length( 0 );

    for( auto it = d->dataRegions.begin(); it != d->dataRegions.end(); ++it ) {
        length += ( *it )->count * ( *it )->length;
    }

    for( auto it = d->imageObjects.begin(); it != d->imageObjects.end(); ++it ) {
        size_t pixelLen = libgraphics::fxapi::EPixelFormat::getPixelSize(
                              ( *it )->format()
                          );

        length += pixelLen * ( *it )->width() * ( *it )->height();
    }

    for( auto it = d->pixelArrays.begin(); it != d->pixelArrays.end(); ++it ) {
        size_t pixelLen = libgraphics::fxapi::EPixelFormat::getPixelSize(
                              ( *it )->format()
                          );

        length += pixelLen * ( *it )->length();
    }

    return length;
}

size_t  BackendDevice::queryBackendMemoryConsumption() {
    return queryManagedMemoryConsumption();
}

size_t BackendDevice::countTextureInstances() const {
    return this->d->imageObjects.size() + this->d->pixelArrays.size();
}

size_t BackendDevice::countTexture1DInstances() const {
    return this->d->pixelArrays.size();
}

size_t BackendDevice::countTexture2DInstances() const {
    return this->d->imageObjects.size();
}

DataRegion*    BackendDevice::allocDataRegion(
    size_t numberOfEntries,
    size_t entrySize
) {
    DataRegion* region = findDataRegion(
                             numberOfEntries,
                             entrySize
                         );

    if( region == nullptr ) {
        return newDataRegion(
                   numberOfEntries,
                   entrySize
               );
    }

    return region;
}

DataRegion*    BackendDevice::newDataRegion(
    size_t numberOfEntries,
    size_t entrySize
) {
    DataRegion* region = new DataRegion(
        numberOfEntries,
        entrySize
    );

    d->dataRegions.push_back(
        std::unique_ptr<DataRegion>(
            region
        )
    );

    return region;
}

DataRegion*    BackendDevice::findDataRegion(
    size_t numberOfEntries,
    size_t entrySize
) {
    DataRegion* result( nullptr );

    for( auto it = d->dataRegions.begin(); it != d->dataRegions.end(); ++it ) {
        if( ( ( *it )->count >= numberOfEntries ) && ( ( *it )->length >= entrySize ) ) {
            if( ( *it )->tryAcquire() ) {
                result = ( *it ).get();

                break;
            }
        }
    }

    return result;
}

size_t BackendDevice::countDataRegions() const {
    return d->dataRegions.size();
}

size_t BackendDevice::countDataSize() const {
    size_t length( 0 );

    for( auto it = d->dataRegions.begin(); it != d->dataRegions.end(); ++it ) {
        length += ( *it )->count * ( *it )->length;
    }

    return length;
}

size_t  BackendDevice::cleanUp() {
    return this->d->allocator->releaseUnused();
}

void BackendDevice::clearDataRegions() {
    this->d->dataRegions.clear();
}

const char* BackendDevice::name() {
    static const std::string deviceName = "CPU";

    return deviceName.c_str();
}

int BackendDevice::backendId() {
    return FXAPI_BACKEND_CPU;
}

std::shared_ptr<libgraphics::StdDynamicPoolAllocator>  BackendDevice::allocator() {
    return d->allocator;
}

void BackendDevice::setAllocator( const std::shared_ptr<StdDynamicPoolAllocator>& newAllocator ) {
    d->allocator = newAllocator;
}

QThreadPool* BackendDevice::threadPool() {
    return &d->threadPool;
}

}
}
}
