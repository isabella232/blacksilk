
#include <libgraphics/allocator.hpp>
#include <libfoundation/app/application.hpp>
#include <libgraphics/fxapi.hpp>
#include <libgraphics/backend/gl/gl_backenddevice.hpp>
#include <libgraphics/backend/cpu/cpu_backenddevice.hpp>
#include <QDebug>

namespace libfoundation {
namespace app {

struct ApplicationBackend::Private {
    std::shared_ptr<libgraphics::StdDynamicPoolAllocator>      alloc;
    libgraphics::fxapi::ApiBackendDevice*   gpuBackend;
    libgraphics::fxapi::ApiBackendDevice*   cpuBackend;
    bool cpuInitialized;
    bool gpuInitialized;

    Private() : cpuInitialized( false ), gpuInitialized( false ), cpuBackend( nullptr ),
        gpuBackend( nullptr ) {}
};

ApplicationBackend::ApplicationBackend() : d( new Private() ) {
    this->initializeAllocators();
}

ApplicationBackend::~ApplicationBackend() {
    if( initialized() ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "ApplicationBackend::~ApplicationBackend(): Backend objects still in existence. This results in major memory leaks...";
#endif
        assert( false );
    }

    delete d->cpuBackend;
    delete d->gpuBackend;

    d->cpuInitialized = false;
    d->gpuInitialized = false;
}

void ApplicationBackend::initializeAllocators() {
    if( !d->alloc ) {
        d->alloc.reset( new libgraphics::StdDynamicPoolAllocator() );
    }
}

/// setup
bool ApplicationBackend::initializeCpuBackend() {
    if( this->initialized() ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "ApplicationBackend::initializeCpuBackend(): Already initialized.";
#endif
        return true;
    }

    /// initializing cpu backend
    d->cpuBackend = ( libgraphics::fxapi::ApiBackendDevice* )new libgraphics::backend::cpu::BackendDevice();
    assert( d->cpuBackend );

    const bool successfullyInitializedCpuBackend = d->cpuBackend->initialize();
    assert( successfullyInitializedCpuBackend );

    if( !successfullyInitializedCpuBackend ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "ApplicationBackend::initializeCpuBackend(): Failed to initialize cpu-backend aborting...";
#endif
        delete d->cpuBackend;

        d->cpuBackend = nullptr;

        return false;
    }

    d->cpuInitialized = true;
    d->cpuBackend->setAllocator( d->alloc );

    return true;
}

bool ApplicationBackend::initializeGpuBackend() {
    if( this->initialized() ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "ApplicationBackend::initializeGpuBackend(): Already initialized.";
#endif
        return true;
    }

    /// initializing gpu backend
    d->gpuBackend = ( libgraphics::fxapi::ApiBackendDevice* )new libgraphics::backend::gl::BackendDevice();
    assert( d->gpuBackend );


    const bool successfullyInitializedGpuBackend = d->gpuBackend->initialize();
    assert( successfullyInitializedGpuBackend );

    if( !successfullyInitializedGpuBackend ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "ApplicationBackend::initializeGpuBackend(): Failed to initialize gpu-backend aborting...";
#endif

        delete d->gpuBackend;

        d->gpuBackend = nullptr;

        return false;
    }

    d->gpuInitialized = true;
    d->gpuBackend->setAllocator( d->alloc );

    return true;
}

bool ApplicationBackend::shutdownCpuBackend() {

    if( !initialized() ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "ApplicationBackend::shutdown(): Not able to shutdown uninitialized ApplicationBackend instance.";
#endif
        return false;
    }

    const bool successfullyShutdownCpuBackend = d->cpuBackend->shutdown();
    assert( successfullyShutdownCpuBackend );

    if( !successfullyShutdownCpuBackend ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "ApplicationBackend::shutdown(): Failed to shutdown cpu-backend";
#endif
        return false;
    }

    delete d->cpuBackend;
    d->cpuBackend = nullptr;
    d->cpuInitialized = false;

    return true;
}

bool ApplicationBackend::shutdownGpuBackend() {

    const bool successfullyShutdownGpuBackend = d->gpuBackend->shutdown();
    assert( successfullyShutdownGpuBackend );

    if( !successfullyShutdownGpuBackend ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "ApplicationBackend::shutdown(): Failed to shutdown gpu-backend";
#endif
        return false;
    }

    delete d->gpuBackend;
    d->gpuBackend = nullptr;
    d->gpuInitialized = false;

#ifdef LIBFOUNDATION_DEBUG_OUTPUT
    qDebug() << "ApplicationBackend::shutdown(): Successfully shutdown ApplicationBackend instance.";
#endif

    return true;
}

bool ApplicationBackend::shutdown() {
    bool good( true );

    if( this->cpuInitialized() ) {
        if( !this->shutdownCpuBackend() ) {
            good = false;
        }
    }

    if( this->gpuInitialized() ) {
        if( !this->shutdownGpuBackend() ) {
            good = false;
        }
    }

    return good;
}

bool ApplicationBackend::initialized() const {
    return d->gpuInitialized && d->cpuInitialized;
}

bool ApplicationBackend::cpuInitialized() const {
    return d->cpuInitialized;
}

bool ApplicationBackend::gpuInitialized() const {
    return d->gpuInitialized;
}

void ApplicationBackend::cleanUp() {
    const auto numberOfReleasedCPUObjects = cpuBackend()->cleanUp();
    qDebug() << "Info:      Cleaned up" << numberOfReleasedCPUObjects << " objects from cpu-backend.";

    const auto numberOfReleasedGPUObjects = gpuBackend()->cleanUp();
    qDebug() << "Info:      Cleaned up" << numberOfReleasedGPUObjects << " objects from gpu-backend.";
}

libgraphics::fxapi::ApiBackendDevice* ApplicationBackend::gpuBackend() const {
    return d->gpuBackend;
}

libgraphics::fxapi::ApiBackendDevice* ApplicationBackend::cpuBackend() const {
    return d->cpuBackend;
}

std::shared_ptr<libgraphics::StdDynamicPoolAllocator>& ApplicationBackend::allocator() {
    return this->d->alloc;
}

const std::shared_ptr<libgraphics::StdDynamicPoolAllocator>& ApplicationBackend::allocator() const {
    return this->d->alloc;
}

}
}
