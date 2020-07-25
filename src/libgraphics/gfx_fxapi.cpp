#include <libgraphics/fxapi.hpp>

#include <QDebug>

#include <libgraphics/backend/cpu/cpu_backenddevice.hpp>
#include <libgraphics/backend/gl/gl_backenddevice.hpp>

namespace libgraphics {
namespace fxapi {

struct Backend {
    std::shared_ptr<libgraphics::fxapi::ApiBackendDevice>  cpuBackend;
    std::shared_ptr<libgraphics::fxapi::ApiBackendDevice>  glBackend;
};
static std::shared_ptr<Backend>    backend;

void initialize() {
    backend.reset(
        new Backend()
    );

    setupCPUBackend();
    setupOpenGLBackend();
}

void setupOpenGLBackend() {
    backend->glBackend.reset(
        new libgraphics::backend::gl::BackendDevice()
    );
}

void setupCPUBackend() {
    backend->cpuBackend.reset(
        new libgraphics::backend::cpu::BackendDevice()
    );
}

ApiBackendDevice*   getOpenGLBackend() {
    return backend->glBackend.get();
}

ApiBackendDevice*   getCPUBackend() {
    return backend->cpuBackend.get();
}

ApiScopedImgRef::ApiScopedImgRef(
    ApiBackendDevice* dev,
    ApiImageObject* obj
) : m_ImageObject( nullptr ), m_ImageBackend( dev ) {

    assert( obj != nullptr );
    assert( m_ImageBackend != nullptr );

    if( m_ImageBackend != nullptr ) {
        m_ImageObject = m_ImageBackend->createTexture2D(
                            obj->format(),
                            obj->width(),
                            obj->height()
                        );
        assert( m_ImageObject != nullptr );
    }
}

ApiScopedImgRef::ApiScopedImgRef(
    ApiBackendDevice* dev
) : m_ImageObject( nullptr ), m_ImageBackend( dev ) {
    assert( dev != nullptr );

    m_ImageObject = m_ImageBackend->createTexture2D();
}

ApiScopedImgRef::ApiScopedImgRef(
    ApiBackendDevice* dev,
    const EPixelFormat::t& format,
    size_t width,
    size_t height
) : m_ImageObject( nullptr ), m_ImageBackend( dev ) {
    assert( dev != nullptr );

    m_ImageObject = m_ImageBackend->createTexture2D(
                        format,
                        width,
                        height
                    );
}

ApiScopedImgRef::ApiScopedImgRef(
    ApiBackendDevice* dev,
    const EPixelFormat::t& format,
    size_t width,
    size_t height,
    void* data
) : m_ImageObject( nullptr ), m_ImageBackend( dev ) {
    assert( dev != nullptr );

    m_ImageObject = m_ImageBackend->createTexture2D(
                        format,
                        width,
                        height,
                        data
                    );
}

ApiScopedImgRef::~ApiScopedImgRef() {
    assert( m_ImageBackend != nullptr );

    if( m_ImageObject != nullptr ) {
        const auto sucessfullyDestroyed = m_ImageBackend->destroyTexture2D( m_ImageObject );
        assert( sucessfullyDestroyed );
        ( void )sucessfullyDestroyed;
    }
}

}
}
