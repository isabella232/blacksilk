#include <libgraphics/backend/gl/gl_texture.hpp>
#include <libgraphics/backend/gl/gl_rendertarget.hpp>
#include <libgraphics/backend/gl/gl_backenddevice.hpp>
#include <libgraphics/utils/gl.hpp>
#include <libgraphics/backend/gl/gl_priv.hpp>

#include <QDebug>
#include <libgraphics/debug.hpp>

namespace libgraphics {
namespace backend {
namespace gl {

RenderTarget::RenderTarget() : d( new Private() ) {
    utils::GL::createFrameBuffer( globalCtx(), &d->frameBuffer );
    LIBGRAPHICS_MEMORY_LOG_ALLOCATE( this );
}

RenderTarget::~RenderTarget() {


    LIBGRAPHICS_MEMORY_LOG_DEALLOCATE( this );

    if( d->frameBuffer != nullptr ) {
        utils::GL::destroyResource( globalCtx(), d->frameBuffer );

        if( d->texture != nullptr ) {
            this->detachTexture();
        }

        d->frameBuffer = nullptr;
        d->texture = nullptr;
    }
}

void RenderTarget::reset() {
    LIBGRAPHICS_MEMORY_LOG_DEALLOCATE( this );

    if( d->texture != nullptr ) {
        this->detachTexture();
    }

    if( d->frameBuffer != nullptr ) {
        utils::GL::destroyResource( globalCtx(), d->frameBuffer );

        d->frameBuffer = nullptr;
        d->texture = nullptr;
    }

    utils::GL::createFrameBuffer( globalCtx(), &d->frameBuffer );
    LIBGRAPHICS_MEMORY_LOG_ALLOCATE( this );
}

bool RenderTarget::attachTexture( Texture* texture ) {
    if( !empty() ) {
        reset();
    }

    if( this->d->texture != nullptr ) {
        this->detachTexture();
    }

    d->texture = texture;
    utils::GL::attachTextureToFrameBuffer( globalCtx(), d->frameBuffer, d->texture->d->tex );

    return true;
}

void RenderTarget::detachTexture() {
    if( d->texture != nullptr ) {
        const auto successfullyReleased = d->texture->release();
        assert( successfullyReleased );

        if( !successfullyReleased ) {
            d->texture->forceRelease();
        }
    }
}

bool RenderTarget::complete() const {
    if( d->texture && d->frameBuffer ) {
        return true;
    }

    return false;
}

Texture* RenderTarget::attachedTexture() const {
    return d->texture;
}

size_t  RenderTarget::width() const {
    assert( d->texture );

    return d->texture->width();
}

size_t  RenderTarget::height() const {
    assert( d->texture );

    return d->texture->height();
}

fxapi::EPixelFormat::t RenderTarget::format() const {
    assert( d->texture );

    return fxapi::EPixelFormat::Empty;
}

bool RenderTarget::empty() const {
    return ( d->frameBuffer == nullptr );
}

size_t RenderTarget::getGpuMemoryConsumption() {
    return 0;
}

size_t RenderTarget::getCpuMemoryConsumption() {
    return 0;
}

utils::GLFrameBuffer* RenderTarget::internalRenderTarget() {
    return d->frameBuffer;
}

bool RenderTarget::tryAcquire() {
    const auto ret = gl::Resource::tryAcquire();

    if( ret ) {
        LIBGRAPHICS_MEMORY_LOG_ACQUIRE( this );
    }

    return ret;
}

void RenderTarget::acquire() {
    gl::Resource::acquire();
    LIBGRAPHICS_MEMORY_LOG_ACQUIRE( this );
}

bool RenderTarget::release() {
    const auto ret = gl::Resource::release();

    if( ret ) {
        LIBGRAPHICS_MEMORY_LOG_RELEASE( this );
    }

    return ret;
}

/// constr. / destr.
RenderTargetPool::RenderTargetPool() {}

/// methods
RenderTarget*   RenderTargetPool::createRenderTarget() {
    RenderTarget* target = new RenderTarget();

    this->add( target );

    return target;
}

RenderTarget*   RenderTargetPool::createAndAcquireRenderTarget() {
    RenderTarget* target = new RenderTarget();

    target->acquire();
    this->add( target );

    return target;
}

RenderTarget*   RenderTargetPool::createAndAcquireRenderTarget( TexturePool* associatedPool, size_t width, size_t height, fxapi::EPixelFormat::t format ) {
    RenderTarget* target = new RenderTarget();

    target->acquire();

    {
        Texture*    attachableTexture = associatedPool->tryAcquireTexture(
                                            width, height, format
                                        );

        if( !attachableTexture ) {
            attachableTexture = associatedPool->createAndAcquireTexture( width, height, format );
            assert( attachableTexture != nullptr );
        }

        const auto successfullyAttached = target->attachTexture(
                                              attachableTexture
                                          );

        if( !successfullyAttached ) {
            attachableTexture->forceRelease();
            delete target;

            return nullptr;
        }
    }

    this->add( target );

    return target;
}

RenderTarget*   RenderTargetPool::tryAcquireRenderTarget( size_t width, size_t height, fxapi::EPixelFormat::t format ) {
    for( auto it = m_Resources.begin(); it != m_Resources.end(); ++it ) {
        if( ( *it )->attachedTexture() == nullptr ) {
            continue;
        }

        if( ( ( *it )->attachedTexture()->width() == width ) && ( ( *it )->attachedTexture()->height() == height ) && ( ( *it )->attachedTexture()->format() == format ) ) {
            if( ( *it )->tryAcquire() ) {

                return ( *it ).get();
            }
        }
    }

    return nullptr;

}

RenderTarget*   RenderTargetPool::acquireRenderTarget( size_t width, size_t height, fxapi::EPixelFormat::t format ) {

    for( auto it = m_Resources.begin(); it != m_Resources.end(); ++it ) {
        if( ( *it )->attachedTexture() == nullptr ) {
            continue;
        }

        if( ( ( *it )->attachedTexture()->width() == width ) && ( ( *it )->attachedTexture()->height() == height ) && ( ( *it )->attachedTexture()->format() == format ) ) {
            ( *it )->acquire();

            return ( *it ).get();
        }
    }

    return nullptr;

}

}
}
}
