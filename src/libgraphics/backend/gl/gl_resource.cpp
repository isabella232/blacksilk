
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
    return !libcommon::atomics::equal32(
               &d->used,
               0
           );
}

bool Resource::tryAcquire() {
    const auto threadId = ( libcommon::atomics::type32 )libcommon::getCurrentThreadId();

    if( libcommon::atomics::exchange32( &d->used, 0, threadId ) == 0 ) {
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
    const auto threadId = ( libcommon::atomics::type32 )libcommon::getCurrentThreadId();

    assert( d->used != 0 );
#if LIBGRAPHICS_DEBUG_RESOURCE

    if( d->used == 0 ) {
        qDebug() << "Warning: Can't release unused resource.";
    }

#endif

    const auto successfullyReleased = ( libcommon::atomics::exchange32( &d->used, threadId, 0 ) == threadId );
#if LIBGRAPHICS_DEBUG_RESOURCE

    if( !successfullyReleased ) {
        qDebug() << "Warning: Failed to release resource.";
    }

#endif
    return successfullyReleased;
}

void Resource::forceRelease() {
    libcommon::atomics::assign32( &d->used, 0 );
}



}
}
}
