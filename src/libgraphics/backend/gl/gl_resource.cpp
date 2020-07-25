
#include <libgraphics/backend/gl/gl_resource.hpp>
#include <libgraphics/backend/gl/gl_priv.hpp>

#include <QElapsedTimer>
#include <QDebug>

namespace libgraphics {
namespace backend {
namespace gl {

Resource::Resource() : d( new Private() ) {}
Resource::~Resource() {
    if( this->acquired() ) {
#if LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Warning: Unreleased resource is now being deleted.";
#endif
#if LIBGRAPHICS_DEBUG_RESOURCE
        assert( false );
#endif
    }
}

bool Resource::acquired() {
    return d->used != 0;
}

bool Resource::tryAcquire() {
    libcommon::UInt32 threadId = libcommon::getCurrentThreadId();
    libcommon::UInt32 zero = 0;

    if( d->used.compare_exchange_weak( zero, threadId ) ) {
        return true;
    }

#if LIBGRAPHICS_DEBUG_RESOURCE && LIBGRAPHICS_DEBUG_RESOURCE_PEDANTIC
    qDebug() << "Warning: Failed to acquire resource.";
#endif
    return false;
}

void Resource::acquire() {
    volatile bool successfullyAcquired = tryAcquire();

    while( !successfullyAcquired ) {
        successfullyAcquired = tryAcquire();
    }
}

bool Resource::release() {
    assert( d->used != 0 );
#if LIBGRAPHICS_DEBUG_RESOURCE

    if( d->used == 0 ) {
        qDebug() << "Warning: Can't release unused resource.";
    }

#endif

    libcommon::UInt32 threadId = libcommon::getCurrentThreadId();
    libcommon::UInt32 zero = 0;
    bool successfullyReleased = d->used.compare_exchange_strong( threadId, zero );

#if LIBGRAPHICS_DEBUG_RESOURCE

    if( !successfullyReleased ) {
        qDebug() << "Warning: Failed to release resource.";
    }

#endif
    return successfullyReleased;
}

void Resource::forceRelease() {
    d->used = 0;
}



}
}
}
