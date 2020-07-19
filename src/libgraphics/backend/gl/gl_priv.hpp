#pragma once

#include <libgraphics/backend/gl/gl_texture.hpp>
#include <libgraphics/backend/gl/gl_rendertarget.hpp>
#include <libgraphics/backend/gl/gl_backenddevice.hpp>
#include <libgraphics/backend/common/formats.hpp>
#include <libgraphics/utils/gl.hpp>

namespace libgraphics {
namespace backend {
namespace gl {

struct Texture::Private : libcommon::PimplPrivate {
    libgraphics::utils::GLTexture*      tex;
    libgraphics::fxapi::EPixelFormat::t format;

    Private() : tex( nullptr ), format( libgraphics::fxapi::EPixelFormat::Empty ) {
        bool ret = utils::GL::createTexture(
                       gl::globalCtx(),
                       &tex
                   );
        ( void ) ret;
        assert( ret );
    }
    ~Private() {
        if( tex != 0 ) {
            utils::GL::destroyResource( gl::globalCtx(), tex );
        }

        tex = nullptr;
    }
};

struct RenderTarget::Private : libcommon::PimplPrivate {
    utils::GLFrameBuffer*   frameBuffer;
    Texture*                texture;

    Private() : frameBuffer( nullptr ),
        texture( nullptr ) {}
};

struct Resource::Private : libcommon::PimplPrivate {
    libcommon::atomics::type32  used;
    Private() : used( 0 ) {}
};

}
}
}
