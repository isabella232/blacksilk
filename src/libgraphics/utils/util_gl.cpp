
#include <libgraphics/systeminfo.hpp>
#include <libgraphics/utils/gl.hpp>

#include <map>
#include <QDebug>
#include <log/log.hpp>


#ifndef __APPLE__
#   include <gl/glew.h>
#   include <GL/gl.h>
#else
#   include <OpenGL.h>
#   include <gl.h>
#   include <glext.h>
#   include <OpenGL/gl3.h>
#endif

namespace libgraphics {
namespace utils {

static const GL::t_enum ERROR_INTERNAL = 0x1337;

/** helpers **/

GL::t_enum GL::getOpenGLInternalTextureFormatType( GL::ETextureDataFormat::t format ) {
    switch( format ) {
        /// ARGB/BGRA
        case GL::ETextureDataFormat::RGBA8:
        case GL::ETextureDataFormat::BGRA8:
        case GL::ETextureDataFormat::RGBA16:
        case GL::ETextureDataFormat::BGRA16:
        case GL::ETextureDataFormat::RGBA16F:
        case GL::ETextureDataFormat::BGRA16F:
        case GL::ETextureDataFormat::RGBA32:
        case GL::ETextureDataFormat::BGRA32:
        case GL::ETextureDataFormat::RGBA32F:
        case GL::ETextureDataFormat::BGRA32F:
            return GL_RGBA;

        /// RGB/BGR
        case GL::ETextureDataFormat::BGR8:
        case GL::ETextureDataFormat::RGB8:
        case GL::ETextureDataFormat::BGR16:
        case GL::ETextureDataFormat::RGB16:
        case GL::ETextureDataFormat::BGR16F:
        case GL::ETextureDataFormat::RGB16F:
        case GL::ETextureDataFormat::BGR32:
        case GL::ETextureDataFormat::RGB32:
        case GL::ETextureDataFormat::BGR32F:
        case GL::ETextureDataFormat::RGB32F:
            return GL_RGB;

        /// RG
        case GL::ETextureDataFormat::RG8:
        case GL::ETextureDataFormat::RG16:
        case GL::ETextureDataFormat::RG16F:
        case GL::ETextureDataFormat::RG32:
        case GL::ETextureDataFormat::RG32F:
            return GL_RG;

        /// Mono
        case GL::ETextureDataFormat::Mono8:
        case GL::ETextureDataFormat::Mono16:
        case GL::ETextureDataFormat::Mono16F:
        case GL::ETextureDataFormat::Mono32:
        case GL::ETextureDataFormat::Mono32F:
            return GL_RED;

        default:
            return 0;
    }
}


std::string GL::getOpenGLInternalTextureFormatName( GL::ETextureDataFormat::t format ) {
    switch( format ) {
        /// ARGB/BGRA
        case GL::ETextureDataFormat::RGBA8:                 return "RGBA8";

        case GL::ETextureDataFormat::BGRA8:                 return "BGRA8";

        case GL::ETextureDataFormat::RGBA16:                return "RGBA16";

        case GL::ETextureDataFormat::BGRA16:                return "BGRA16";

        case GL::ETextureDataFormat::RGBA16F:               return "RGBA16F";

        case GL::ETextureDataFormat::BGRA16F:               return "BGRA16F";

        case GL::ETextureDataFormat::RGBA32:                return "RGBA32";

        case GL::ETextureDataFormat::BGRA32:                return "BGRA32";

        case GL::ETextureDataFormat::RGBA32F:               return "RGBA32F";

        case GL::ETextureDataFormat::BGRA32F:               return "BGRA32F";

        /// RGB/BGR
        case GL::ETextureDataFormat::BGR8:                  return "BGR8";

        case GL::ETextureDataFormat::RGB8:                  return "RGB8";

        case GL::ETextureDataFormat::BGR16:                 return "BGR16";

        case GL::ETextureDataFormat::RGB16:                 return "RGB16";

        case GL::ETextureDataFormat::BGR16F:                return "BGR16F";

        case GL::ETextureDataFormat::RGB16F:                return "RGB16F";

        case GL::ETextureDataFormat::BGR32:                 return "BGR32";

        case GL::ETextureDataFormat::RGB32:                 return "RGB32";

        case GL::ETextureDataFormat::BGR32F:                return "BGR32F";

        case GL::ETextureDataFormat::RGB32F:                return "RGB32F";

        /// RG
        case GL::ETextureDataFormat::RG8:                   return "RG8";

        case GL::ETextureDataFormat::RG16:                  return "RG16";

        case GL::ETextureDataFormat::RG16F:                 return "RG16F";

        case GL::ETextureDataFormat::RG32:                  return "RG32";

        case GL::ETextureDataFormat::RG32F:                 return "RG32F";

        /// Mono
        case GL::ETextureDataFormat::Mono8:                 return "Mono8";

        case GL::ETextureDataFormat::Mono16:                return "Mono16";

        case GL::ETextureDataFormat::Mono16F:               return "Mono16F";

        case GL::ETextureDataFormat::Mono32:                return "Mono32";

        case GL::ETextureDataFormat::Mono32F:               return "Mono32F";

        case GL::ETextureDataFormat::Empty:                 return "Empty";

        default:
            return "Unknown";
    }
}

GL::t_enum GL::getOpenGLInternalTextureFormatElementType( GL::ETextureDataFormat::t format ) {
    switch( format ) {
        /// ARGB/BGRA
        case GL::ETextureDataFormat::RGBA8:
        case GL::ETextureDataFormat::BGRA8:
            return GL_UNSIGNED_BYTE;

        case GL::ETextureDataFormat::RGBA16:
        case GL::ETextureDataFormat::BGRA16:
            return GL_UNSIGNED_SHORT;

        case GL::ETextureDataFormat::RGBA16F:
        case GL::ETextureDataFormat::BGRA16F:
            return GL_HALF_FLOAT;

        case GL::ETextureDataFormat::RGBA32:
        case GL::ETextureDataFormat::BGRA32:
            return GL_UNSIGNED_INT;

        case GL::ETextureDataFormat::RGBA32F:
        case GL::ETextureDataFormat::BGRA32F:
            return GL_FLOAT;

        /// RGB/BGR
        case GL::ETextureDataFormat::BGR8:
        case GL::ETextureDataFormat::RGB8:
            return GL_UNSIGNED_BYTE;

        case GL::ETextureDataFormat::BGR16:
        case GL::ETextureDataFormat::RGB16:
            return GL_UNSIGNED_SHORT;

        case GL::ETextureDataFormat::BGR16F:
        case GL::ETextureDataFormat::RGB16F:
            return GL_HALF_FLOAT;

        case GL::ETextureDataFormat::BGR32:
        case GL::ETextureDataFormat::RGB32:
            return GL_UNSIGNED_INT;

        case GL::ETextureDataFormat::BGR32F:
        case GL::ETextureDataFormat::RGB32F:
            return GL_FLOAT;

        /// RG
        case GL::ETextureDataFormat::RG8:
            return GL_UNSIGNED_BYTE;

        case GL::ETextureDataFormat::RG16:
            return GL_UNSIGNED_SHORT;

        case GL::ETextureDataFormat::RG16F:
            return GL_HALF_FLOAT;

        case GL::ETextureDataFormat::RG32:
            return GL_UNSIGNED_INT;

        case GL::ETextureDataFormat::RG32F:
            return GL_FLOAT;

        /// Mono
        case GL::ETextureDataFormat::Mono8:
            return GL_UNSIGNED_BYTE;

        case GL::ETextureDataFormat::Mono16:
            return GL_UNSIGNED_SHORT;

        case GL::ETextureDataFormat::Mono16F:
            return GL_HALF_FLOAT;

        case GL::ETextureDataFormat::Mono32:
            return GL_UNSIGNED_INT;

        case GL::ETextureDataFormat::Mono32F:
            return GL_FLOAT;

        default:
            return 0;
    }
}

GL::ETextureDataFormat::t GL::fromOpenGLInternalTextureFormat( GL::t_enum format ) {
    switch( format ) {
        case GL_RGB8:
            return GL::ETextureDataFormat::RGB8;

        case GL_RGB16:
            return GL::ETextureDataFormat::RGB16;

        case GL_RGB16F:
            return GL::ETextureDataFormat::RGB16F;

        case GL_RGB32I:
            return GL::ETextureDataFormat::RGB32;

        case GL_RGB32F:
            return GL::ETextureDataFormat::RGB32F;

        case GL_RGBA8:
            return GL::ETextureDataFormat::RGBA8;

        case GL_RGBA16:
            return GL::ETextureDataFormat::RGBA16;

        case GL_RGBA16F:
            return GL::ETextureDataFormat::RGBA16F;

        case GL_RGBA32I:
            return GL::ETextureDataFormat::RGBA32;

        case GL_RGBA32F:
            return GL::ETextureDataFormat::RGBA32F;

        case GL_RG8:
            return GL::ETextureDataFormat::RG8;

        case GL_RG16:
            return GL::ETextureDataFormat::RG16;

        case GL_RG16F:
            return GL::ETextureDataFormat::RG16F;

        case GL_RG32I:
            return GL::ETextureDataFormat::RG32;

        case GL_RG32F:
            return GL::ETextureDataFormat::RG32F;

        case GL_R8:
            return GL::ETextureDataFormat::Mono8;

        case GL_R16:
            return GL::ETextureDataFormat::Mono16;

        case GL_R16F:
            return GL::ETextureDataFormat::Mono16F;

        case GL_R32I:
            return GL::ETextureDataFormat::Mono32;

        case GL_R32F:
            return GL::ETextureDataFormat::Mono32F;

        default:
            return GL::ETextureDataFormat::Empty;
    }
}

GL::t_enum GL::getOpenGLInternalTextureFormat( GL::ETextureDataFormat::t format ) {
    switch( format ) {
        /// ARGB/BGRA
        case GL::ETextureDataFormat::RGBA8:
        case GL::ETextureDataFormat::BGRA8:
            return GL_RGBA8;

        case GL::ETextureDataFormat::RGBA16:
        case GL::ETextureDataFormat::BGRA16:
#ifdef INTEL_UBUNTU_SNORM
            return GL_RGBA16_SNORM;
#else
            return GL_RGBA16;
#endif

        case GL::ETextureDataFormat::RGBA16F:
        case GL::ETextureDataFormat::BGRA16F:
            return GL_RGBA16F;

        case GL::ETextureDataFormat::RGBA32:
        case GL::ETextureDataFormat::BGRA32:
            return GL_RGBA32I;

        case GL::ETextureDataFormat::RGBA32F:
        case GL::ETextureDataFormat::BGRA32F:
            return GL_RGBA32F;

        /// RGB/BGR
        case GL::ETextureDataFormat::BGR8:
        case GL::ETextureDataFormat::RGB8:
            return GL_RGB8;

        case GL::ETextureDataFormat::BGR16:
        case GL::ETextureDataFormat::RGB16:
#ifdef INTEL_UBUNTU_SNORM
            return GL_RGB16_SNORM;
#else
            return GL_RGB16;
#endif

        case GL::ETextureDataFormat::BGR16F:
        case GL::ETextureDataFormat::RGB16F:
            return GL_RGB16F;

        case GL::ETextureDataFormat::BGR32:
        case GL::ETextureDataFormat::RGB32:
            return GL_RGB32I;

        case GL::ETextureDataFormat::BGR32F:
        case GL::ETextureDataFormat::RGB32F:
            return GL_RGB32F;

        /// RG
        case GL::ETextureDataFormat::RG8:
            return GL_RG8;

        case GL::ETextureDataFormat::RG16:
            return GL_RG16;

        case GL::ETextureDataFormat::RG16F:
            return GL_RG16F;

        case GL::ETextureDataFormat::RG32:
            return GL_RG32I;

        case GL::ETextureDataFormat::RG32F:
            return GL_RG32F;

        /// Mono
        case GL::ETextureDataFormat::Mono8:
            return GL_R8;

        case GL::ETextureDataFormat::Mono16:
            return GL_R16;

        case GL::ETextureDataFormat::Mono16F:
            return GL_R16F;

        case GL::ETextureDataFormat::Mono32:
            return GL_R32I;

        case GL::ETextureDataFormat::Mono32F:
            return GL_R32F;

        default:
            return 0;
    }
}

GL::t_sizei GL::getTextureFormatPixelSize( GL::ETextureDataFormat::t format ) {
    size_t len = 0;

    switch( format ) {
        /// ARGB/BGRA
        case GL::ETextureDataFormat::RGBA8:
        case GL::ETextureDataFormat::BGRA8:
            len = 4 * sizeof( char );
            break;

        case GL::ETextureDataFormat::RGBA16:
        case GL::ETextureDataFormat::BGRA16:
            len = 4 * sizeof( short );
            break;

        case GL::ETextureDataFormat::RGBA16F:
        case GL::ETextureDataFormat::BGRA16F:
            len = 4 * sizeof( short );
            break;

        case GL::ETextureDataFormat::RGBA32:
        case GL::ETextureDataFormat::BGRA32:
            len = 4 * sizeof( int );
            break;

        case GL::ETextureDataFormat::RGBA32F:
        case GL::ETextureDataFormat::BGRA32F:
            len = 4 * sizeof( float );
            break;

        /// RGB/BGR
        case GL::ETextureDataFormat::BGR8:
        case GL::ETextureDataFormat::RGB8:
            len = 3 * sizeof( char );
            break;

        case GL::ETextureDataFormat::BGR16:
        case GL::ETextureDataFormat::RGB16:
            len = 3 * sizeof( short );
            break;

        case GL::ETextureDataFormat::BGR16F:
        case GL::ETextureDataFormat::RGB16F:
            len = 3 * sizeof( short );
            break;

        case GL::ETextureDataFormat::BGR32:
        case GL::ETextureDataFormat::RGB32:
            len = 3 * sizeof( int );
            break;

        case GL::ETextureDataFormat::BGR32F:
        case GL::ETextureDataFormat::RGB32F:
            len = 3 * sizeof( float );
            break;

        /// RG
        case GL::ETextureDataFormat::RG8:
            len = 2 * sizeof( char );
            break;

        case GL::ETextureDataFormat::RG16:
            len = 2 * sizeof( short );
            break;

        case GL::ETextureDataFormat::RG16F:
            len = 2 * sizeof( short );
            break;

        case GL::ETextureDataFormat::RG32:
            len = 2 * sizeof( int );
            break;

        case GL::ETextureDataFormat::RG32F:
            len = 2 * sizeof( float );
            break;

        /// Mono
        case GL::ETextureDataFormat::Mono8:
            len = 1 * sizeof( char );
            break;

        case GL::ETextureDataFormat::Mono16:
            len = 1 * sizeof( short );
            break;

        case GL::ETextureDataFormat::Mono16F:
            len = 1 * sizeof( short );
            break;

        case GL::ETextureDataFormat::Mono32:
            len = 1 * sizeof( int );
            break;

        case GL::ETextureDataFormat::Mono32F:
            len = 1 * sizeof( float );
            break;

        default:
            break;
    }

    return ( GL::t_sizei )len;
}

/** GLDbgContext **/
struct GLDbgContext::GLPriv : libcommon::PimplPrivate {
    std::vector< GLDbgContext::ErrInfo >    errors;
    std::vector< GLDbgContext::CallInfo >   calls;

    GL::t_sizei     objectCount;
    GL::t_sizei     textureCount;
    GL::t_sizei     shaderCount;
    GL::t_sizei     programCount;
    GL::t_sizei     frameBufferCount;

    GL::t_sizei     allocatedTextureData;
    GL::t_sizei     allocatedRawData;

    GLPriv() : objectCount( 0 ), textureCount( 0 ), shaderCount( 0 ),
        frameBufferCount( 0 ), allocatedTextureData( 0 ), allocatedRawData( 0 ) {}
};

GLDbgContext::GLDbgContext() : d( new GLPriv() ) {}

void GLDbgContext::reset() {
    d->errors.clear();
    d->calls.clear();

    d->objectCount = 0;
    d->textureCount = 0;
    d->shaderCount = 0;
    d->programCount = 0;
    d->frameBufferCount = 0;

    d->allocatedRawData = 0;
    d->allocatedTextureData = 0;
}

const std::vector<GLDbgContext::ErrInfo>& GLDbgContext::glErrors() const {
    return d->errors;
}

const std::vector<GLDbgContext::CallInfo>& GLDbgContext::calls() const {
    return d->calls;
}

GL::t_sizei     GLDbgContext::countShaders() const {
    return d->shaderCount;
}

GL::t_sizei     GLDbgContext::countPrograms() const {
    return d->programCount;
}

GL::t_sizei     GLDbgContext::countTextureObjects() const {
    return d->textureCount;
}

GL::t_sizei     GLDbgContext::countFrameBuffers() const {
    return d->frameBufferCount;
}

GL::t_sizei     GLDbgContext::totalAllocatedData() const {
    return d->allocatedRawData + d->allocatedTextureData;
}

GL::t_sizei     GLDbgContext::totalAllocatedTextureData() const {
    return d->allocatedTextureData;
}

GL::t_sizei     GLDbgContext::totalAllocatedUnknownData() const {
    return d->allocatedRawData;
}

void GLDbgContext::reportNewShader( GL* ctx, GLShader* object ) {
    ( void ) ctx;
    ( void ) object;

    qDebug() << "Created new shader object(id=" << object->id() << ").";

    d->shaderCount += 1;
}

void GLDbgContext::reportNewProgram( GL* ctx, GLProgram* program ) {
    ( void ) ctx;
    ( void ) program;

    qDebug() << "Created new program object(id=" << program->id() << ").";

    d->programCount += 1;
}

void GLDbgContext::reportNewTextureObject( GL* ctx, GLTexture* texture ) {
    ( void ) ctx;

    qDebug() << "Created new texture object(id=" << texture->id() << ").";

    size_t len = texture->width() * texture->height();

    if( len != 0 ) {
        reportNewTextureData( ctx, texture );
    }

    d->textureCount += 1;
}

void GLDbgContext::reportNewTextureData( GL* ctx, GLTexture* texture ) {
    ( void ) ctx;

    size_t len = 0;

    switch( texture->dimension() ) {
        case GL::ETextureDimension::Dim1: len = texture->width() * GL::getTextureFormatPixelSize( texture->format() ); break;

        case GL::ETextureDimension::Dim2: len = texture->width() * texture->height() * GL::getTextureFormatPixelSize( texture->format() ); break;

        case GL::ETextureDimension::Dim3: len = texture->width() * texture->height() * texture->depth() * GL::getTextureFormatPixelSize( texture->format() ); break;

        default: len = 0; assert( false ); break;
    }

    qDebug() << "Allocated texture data of " << len << " bytes(width=" << texture->width() << ", height=" << texture->height() << ", depth=" << texture->depth() << ").";
    qDebug() <<     " * format: " << GL::getOpenGLInternalTextureFormatName( texture->format() ).data();

    d->allocatedTextureData += ( GL::t_sizei )len;
}

void GLDbgContext::reportNewRawData( GL* ctx, GL::t_sizei length ) {
    ( void ) ctx;

    qDebug() << "Allocated buffer data of " << length << "bytes.";
    d->allocatedRawData += length;
}

void GLDbgContext::reportNewFrameBuffer( GL* ctx, GLFrameBuffer* buffer ) {
    ( void ) ctx;
    ( void ) buffer;

    qDebug() << "Created new frambuffer object(id=" << buffer->id() << ").";
    d->frameBufferCount++;
}

void GLDbgContext::reportResourceDeallocation( GL* ctx, GLShader* obj ) {
    ( void ) ctx;
    ( void ) obj;

    qDebug() << "Destroyed shader object(id=" << obj->id() << ").";
    assert( d->shaderCount != 0 );
    d->shaderCount--;
}

void GLDbgContext::reportResourceDeallocation( GL* ctx, GLProgram* obj ) {
    ( void ) ctx;
    ( void ) obj;

    qDebug() << "Destroyed program object(id=" << obj->id() << ").";
    assert( d->programCount != 0 );
    d->programCount--;
}

void GLDbgContext::reportResourceDeallocation( GL* ctx, GLTexture* obj ) {
    ( void ) ctx;
    ( void ) obj;

    size_t len = 0;

    switch( obj->dimension() ) {
        case GL::ETextureDimension::Dim1: len = obj->width() * GL::getTextureFormatPixelSize( obj->format() ); break;

        case GL::ETextureDimension::Dim2: len = obj->width() * obj->height() * GL::getTextureFormatPixelSize( obj->format() ); break;

        case GL::ETextureDimension::Dim3: len = obj->width() * obj->height() * obj->depth() * GL::getTextureFormatPixelSize( obj->format() ); break;

        default: len = 0; assert( false ); break;
    }

    qDebug() << "Deallocated texture data of " << len << " bytes(width=" << obj->width() << ", height=" << obj->height() << ", depth=" << obj->depth() << ").";
    qDebug() <<     " * format: " << GL::getOpenGLInternalTextureFormatName( obj->format() ).data();

    assert( d->allocatedTextureData >= len );
    d->allocatedTextureData -= ( GL::t_sizei )len;

    assert( d->textureCount != 0 );
    d->textureCount--;
}

void GLDbgContext::reportResourceDeallocation( GL* ctx, GLFrameBuffer* obj ) {
    ( void ) ctx;
    ( void ) obj;

    qDebug() << "Destroyed framebuffer object(id=" << obj->id() << ").";
    assert( d->frameBufferCount != 0 );
    d->frameBufferCount--;
}

void GLDbgContext::reportResourceDeallocation( GL* ctx, GL::t_sizei len ) {
    ( void ) ctx;

    qDebug() << "Deallocated buffer data of " << len << " bytes.";
    assert( d->allocatedRawData >= len );
    d->allocatedRawData -= len;
}

void GLDbgContext::reportCall( GL* ctx, const std::string& function, const std::map<std::string, std::string>& parameters ) {
    ( void ) ctx;

    d->calls.push_back(
        CallInfo( function, parameters, true )
    );
}

void GLDbgContext::reportInvalidCall( GL* ctx, const std::string& function, const std::map<std::string, std::string>& parameters ) {
    ( void ) ctx;

    d->calls.push_back(
        CallInfo( function, parameters, false )
    );
}

void GLDbgContext::reportError( GL* ctx, GLErr* err ) {
    ( void ) ctx;

    d->errors.push_back(
        ErrInfo(
            err->errorMessage(),
            err->errorType()
        )
    );
}

/** GLTexture **/
struct GLTexture::GLPriv : libcommon::PimplPrivate {
    GL::t_uint      textureId;
    GL::t_sizei     width;
    GL::t_sizei     height;
    GL::t_sizei     depth;

    GL::ETextureDataFormat::t   format;
    GL::ETextureDimension::t    dim;
    GL::t_enum                  internalFormat;

    GLPriv() : textureId( 0 ), width( 0 ), height( 0 ), depth( 0 ),
        format( GL::ETextureDataFormat::Empty ), dim( GL::ETextureDimension::Dim2 ), internalFormat( 0 ) {}
};

GLTexture::GLTexture() : d( new GLPriv() ) {}

GL::t_sizei GLTexture::depth() const {
    return d->depth;
}

GL::t_uint  GLTexture::id() const {
    return d->textureId;
}

GL::t_sizei GLTexture::width() const {
    return d->width;
}

GL::t_sizei GLTexture::height() const {
    return d->height;
}

GL::ETextureDataFormat::t GLTexture::format() const {
    return d->format;
}

GL::ETextureDimension::t GLTexture::dimension() const {
    return d->dim;
}

bool GLTexture::empty() const {
    return ( d->textureId == 0 );
}

/** GLRawBuffer **/
struct GLRawBuffer::GLPriv : libcommon::PimplPrivate {
    GL::t_uint      bufferId;
    GL::t_sizei     size;
    GL::ERawBufferHint::t hint;
    GLPriv() : bufferId( 0 ), size( 0 ), hint( GL::ERawBufferHint::None ) {}
};

GLRawBuffer::GLRawBuffer() : d( new GLPriv() ) {}

const GL::ERawBufferHint::t GLRawBuffer::hint() const {
    return d->hint;
}

const GL::t_uint    GLRawBuffer::id() const {
    return d->bufferId;
}

const GL::t_sizei   GLRawBuffer::size() const {
    return d->size;
}

const bool          GLRawBuffer::empty() const {
    return ( d->bufferId == 0 );
}

/** GLErr **/
struct GLErr::GLPriv : libcommon::PimplPrivate {
    GL*  context;
    std::string errorMessage;
    std::string errorType;
};

GLErr::GLErr() : d( new GLPriv() ) {}

GL* GLErr::associatedOpenGLContext() const {
    return d->context;
}

const std::string& GLErr::errorMessage() const {
    return d->errorMessage;
}

const std::string& GLErr::errorType() const {
    return d->errorType;
}

/** GL **/
struct GL::GLPriv : libcommon::PimplPrivate {
    bool    initialized;
    libcommon::ScopedPtr<GLDbgContext>  dbgContext;
    libcommon::ScopedPtr< libgraphics::SystemInfo > sysInfo;
    std::vector< libcommon::ScopedPtr< GLErr > > errors;

    bool texture1DEnabled;
    bool texture2DEnabled;
    bool texture3DEnabled;

    GLPriv() : initialized( false ), texture1DEnabled( false ), texture2DEnabled( false ),
        texture3DEnabled( false ) {}
};

/** private helpers **/

bool GL::checkForPowerOf2( GL* ctx, GL::t_sizei width ) {
    if( ( width % 2 != 0 ) ) {
        if( !ctx->d->sysInfo->supportOpenGLFeature( "GL_ARB_texture_non_power_of_two" ) ) {
            ctx->newError(
                "GL::checkForPowerOf2",
                "Texture dimensions not power of two."
            );

            assert( false );

            return false;
        }
    }

    return true;
}
bool GL::checkForPowerOf2( GL* ctx, GL::t_sizei width, GL::t_sizei height ) {
    if( ( width % 2 != 0 ) || ( height % 2 != 0 ) ) {
        if( !ctx->d->sysInfo->supportOpenGLFeature( "GL_ARB_texture_non_power_of_two" ) ) {
            ctx->newError(
                "GL::checkForPowerOf2",
                "Texture dimensions not power of two."
            );

            assert( false );

            return false;
        }
    }

    return true;
}
bool GL::checkForPowerOf2( GL* ctx, GL::t_sizei width, GL::t_sizei height, GL::t_sizei depth ) {
    if( ( width % 2 != 0 ) || ( height % 2 != 0 ) || ( depth % 2 != 0 ) ) {
        if( !ctx->d->sysInfo->supportOpenGLFeature( "GL_ARB_texture_non_power_of_two" ) ) {
            ctx->newError(
                "GL::checkForPowerOf2",
                "Texture dimensions not power of two."
            );

            assert( false );

            return false;
        }
    }

    return true;
}

GL::GL() : d( new GLPriv() ) {}

GL::~GL() {
    d->dbgContext.reset();
}

void GL::assignDbgContext( GLDbgContext* ctx ) {
    d->dbgContext.reset( ctx );
}

GLDbgContext* GL::currentDbgContext() {
    return d->dbgContext.get();
}

bool GL::hasDbgContext() const {
    return !d->dbgContext.empty();
}

void GL::newError( t_int errorType, const std::string& errorMessage ) {

    std::string errorName;

    switch( errorType ) {
        case GL_INVALID_ENUM:
            errorName = "GL_INVALID_ENUM";
            break;

        case GL_INVALID_VALUE:
            errorName = "GL_INVALID_VALUE";
            break;

        case GL_INVALID_OPERATION:
            errorName = "GL_INVALID_OPERATION";
            break;

        case GL_STACK_OVERFLOW:
            errorName = "GL_STACK_OVERFLOW";
            break;

        case GL_STACK_UNDERFLOW:
            errorName = "GL_STACK_UNDERFLOW";
            break;

        case GL_OUT_OF_MEMORY:
            errorName = "GL_OUT_OF_MEMORY";
            break;

        case GL_INVALID_FRAMEBUFFER_OPERATION:
            errorName = "GL_INVALID_FRAMEBUFFER_OPERATION";
            break;

        case GL_TABLE_TOO_LARGE:
            errorName = "GL_TABLE_TOO_LARGE";
            break;

        case ERROR_INTERNAL:
            errorName = "LIBGRAPHICS_INTERNAL";
            break;

        default:
            errorName = "UNKNOWN_ERROR";
            break;
    }

    newError( errorName, errorMessage );
}

void GL::newError( const std::string& errorType, const std::string& errorMessage ) {
    this->d->errors.push_back(
        libcommon::ScopedPtr<GLErr>( new GLErr() )
    );
    this->d->errors.back()->d->errorMessage = errorMessage;
    this->d->errors.back()->d->errorType = errorType;
    this->d->errors.back()->d->context = this;

#ifdef LIBGRAPHICS_SHOW_GL_ERRORS
    qDebug() << "OpenGL Error: " << errorType.c_str() << ": " << errorMessage.c_str();
#endif

    if( !d->dbgContext.empty() ) {
        d->dbgContext->reportError(
            this,
            d->errors.back().get()
        );
    }
}

const GLErr* GL::lastError() const {
    return this->d->errors.back().get();
}

bool GL::initialize() {
    if( !d->initialized ) {
        d->sysInfo.reset( new libgraphics::SystemInfo() );
        d->initialized = true;

        return true;
    }

    return true;
}

bool GL::initialized() const {
    return d->initialized;
}

/// static initialization set
GL* GL::construct() {
    GL* glCtx = new GL();

    if( !glCtx->initialize() ) {
        delete glCtx;

        return nullptr;
    }

    return glCtx;
}

///
/// LEGACY MATRIX AND VIEW OPERATIONS
///

bool GL::setMatrixMode( GL* ctx, EMatrixMode mode ) {
    assert( ctx );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    glMatrixMode( ( GLenum )mode );

    return true;
}

bool GL::loadIdentity( GL* ctx ) {
    assert( ctx );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    glLoadIdentity();

    return true;
}

bool GL::ortho(
    GL* ctx,
    float left,
    float right,
    float bottom,
    float top,
    float znear,
    float zfar
) {
    assert( ctx );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    glOrtho( left, right, bottom, top, znear, zfar );

    return true;
}

bool GL::translate(
    GL* ctx,
    float x,
    float y,
    float z
) {
    assert( ctx );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    glTranslatef( x, y, z );

    return true;
}

bool GL::scale(
    GL* ctx,
    float x,
    float y,
    float z
) {
    assert( ctx );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    glScalef( x, y, z );

    return true;
}

bool GL::viewport( GL* ctx, t_int x, t_int y, t_int width, t_int height ) {
    assert( ctx );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    glViewport( x, y, width, height );

    return true;
}

bool GL::clearColors( GL* ctx, float r, float g, float b ) {
    assert( ctx );

    if( !ctx->initialized() ) {
        assert( false );
    }

    glClearColor( r, g, b, 1.0 );
    glClear( GL_COLOR_BUFFER_BIT );

    return true;
}

bool GL::clearColorsAndDepth( GL* ctx, float r, float g, float b ) {
    assert( ctx );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    glClearColor( r, g, b, 1.0 );
    glClearDepth( 0.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    return true;

}


/** texture **/

void GL::enableTexture( GL* ctx, ETextureDimension::t dim ) {
    assert( ctx );

    if( !ctx->initialized() ) {
        assert( false );
        return;
    }

    switch( dim ) {
        case ETextureDimension::Dim1:
            if( ctx->d->texture1DEnabled ) { break; }

            ::glEnable( GL_TEXTURE_1D );

            ctx->d->texture1DEnabled = true;
            break;

        case ETextureDimension::Dim2:
            if( ctx->d->texture2DEnabled ) { break; }

            ::glEnable( GL_TEXTURE_2D );

            ctx->d->texture2DEnabled = true;
            break;

        case ETextureDimension::Dim3:
            if( ctx->d->texture3DEnabled ) { break; }

            ::glEnable( GL_TEXTURE_3D );

            ctx->d->texture3DEnabled = true;
            break;
    }
}

void GL::disableTexture( GL* ctx, ETextureDimension::t dim ) {
    assert( ctx );

    if( !ctx->initialized() ) {
        assert( false );
        return;
    }

    switch( dim ) {
        case ETextureDimension::Dim1:
            if( !ctx->d->texture1DEnabled ) { break; }

            ::glDisable( GL_TEXTURE_1D );
            ::glDisable( GL_TEXTURE_1D_ARRAY );

            ctx->d->texture1DEnabled = false;
            break;

        case ETextureDimension::Dim2:
            if( !ctx->d->texture2DEnabled ) { break; }

            ::glDisable( GL_TEXTURE_2D );
            ::glDisable( GL_TEXTURE_2D_ARRAY );

            ctx->d->texture2DEnabled = false;
            break;

        case ETextureDimension::Dim3:
            if( !ctx->d->texture3DEnabled ) { break; }

            ::glDisable( GL_TEXTURE_3D );

            ctx->d->texture3DEnabled = false;
            break;

        default:
            break;
    }
}

bool GL::isTextureEnabled( GL* ctx, ETextureDimension::t dim ) {
    assert( ctx );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    switch( dim ) {
        case ETextureDimension::Dim1:
            return ctx->d->texture1DEnabled;

        case ETextureDimension::Dim2:
            return ctx->d->texture2DEnabled;

        case ETextureDimension::Dim3:
            return ctx->d->texture3DEnabled;

        default:
            return false;
    }
}

bool GL::createTexture( GL* ctx, GLTexture** outRef ) {
    assert( ctx );
    assert( outRef );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    ( void )::glGetError();

    glEnable( GL_TEXTURE_1D );
    glEnable( GL_TEXTURE_2D );

    *outRef = new GLTexture();


    GLuint texId( 0 );
    ( *outRef )->d->textureId = 0;

    ::glGenTextures( 1, &texId );
    ( *outRef )->d->textureId = texId;

    const auto err = ::glGetError();

    if( err != GL_NO_ERROR ) {
        assert( false );
        return false;
    }

    if( ctx->hasDbgContext() ) {
        ctx->d->dbgContext->reportNewTextureObject(
            ctx,
            *outRef
        );
    }

    return true;
}

/** synchronous texture creation and upload **/
bool GL::createTexture( GL* ctx, GLTexture** outRef, ETextureDimension::t dim, ETextureDataFormat::t format, t_uint width, t_uint height, t_uint depth, t_void* data ) {
    switch( dim ) {
        case ETextureDimension::Dim1:
            return GL::createTexture1D(
                       ctx,
                       outRef,
                       format,
                       width,
                       data
                   );

        case ETextureDimension::Dim2:
            return GL::createTexture2D(
                       ctx,
                       outRef,
                       format,
                       width,
                       height,
                       data
                   );

        case ETextureDimension::Dim3:
            return GL::createTexture3D(
                       ctx,
                       outRef,
                       format,
                       width,
                       height,
                       depth,
                       data
                   );

        default:
            return false;
    }
}

bool GL::createTexture1D( GL* ctx, GLTexture** outRef, ETextureDataFormat::t format, t_uint width, t_void* data ) {
    assert( ctx );
    assert( outRef );
    assert( format != ETextureDataFormat::Empty );
    assert( width > 0 );
    assert( data );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    if( GL::createTexture( ctx, outRef ) ) {

        return GL::createTextureData1D(
                   ctx,
                   *outRef,
                   format,
                   width,
                   data
               );

    }

    return false;
}

bool GL::createTexture2D( GL* ctx, GLTexture** outRef, ETextureDataFormat::t format, t_uint width, t_uint height, t_void* data ) {
    assert( ctx );
    assert( outRef );
    assert( format != ETextureDataFormat::Empty );
    assert( width > 0 );
    assert( height > 0 );
    assert( data );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    if( GL::createTexture( ctx, outRef ) ) {

        return GL::createTextureData2D(
                   ctx,
                   *outRef,
                   format,
                   width,
                   height,
                   data
               );

    }

    return false;
}

bool GL::createTexture3D( GL* ctx, GLTexture** outRef, ETextureDataFormat::t format, t_uint width, t_uint height, t_uint depth, t_void* data ) {
    assert( ctx );
    assert( outRef );
    assert( format != ETextureDataFormat::Empty );
    assert( width > 0 );
    assert( height > 0 );
    assert( depth > 0 );
    assert( data );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    if( GL::createTexture( ctx, outRef ) ) {

        return GL::createTextureData3D(
                   ctx,
                   *outRef,
                   format,
                   width,
                   height,
                   depth,
                   data
               );

    }

    return false;
}

bool GL::createTextureData( GL* ctx, GLTexture* texture, ETextureDimension::t dim, ETextureDataFormat::t format, t_uint width, t_uint height, t_uint depth, t_void* data ) {
    switch( dim ) {
        case ETextureDimension::Dim1:
            return GL::createTextureData1D(
                       ctx,
                       texture,
                       format,
                       width,
                       data
                   );

        case ETextureDimension::Dim2:
            return GL::createTextureData2D(
                       ctx,
                       texture,
                       format,
                       width,
                       height,
                       data
                   );

        case ETextureDimension::Dim3:
            return GL::createTextureData3D(
                       ctx,
                       texture,
                       format,
                       width,
                       height,
                       depth,
                       data
                   );

        default:
            return false;
    }
}

bool GL::createTextureData1D( GL* ctx, GLTexture* texture, ETextureDataFormat::t format, t_uint width, t_void* data ) {
    assert( ctx );
    assert( texture );
    assert( format != ETextureDataFormat::Empty );
    assert( width > 0 );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    ( void ) ::glGetError();

    if( !GL::checkForPowerOf2( ctx, width ) ) {
        ctx->newError(
            ERROR_INTERNAL,
            "Intended texture dimensions are not power of two."
        );
        assert( false );
        return false;
    }

    if( ( data != nullptr ) && ( texture->width() == width ) ) {
        return GL::updateTextureData(
                   ctx,
                   texture,
                   0,
                   0,
                   0,
                   width,
                   0,
                   0,
                   data
               );
    }

    GL::t_enum  formatType( GL::getOpenGLInternalTextureFormatType( format ) );
    GL::t_enum  formatElementType( GL::getOpenGLInternalTextureFormatElementType( format ) );
    GL::t_enum  internalType( GL::getOpenGLInternalTextureFormat( format ) );

    ::glBindTexture(
        GL_TEXTURE_1D,
        texture->d->textureId
    );

    glTexParameterf( GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

    ::glTexImage1D(
        GL_TEXTURE_1D,
        0,
        ( GLint )internalType,
        width,
        0,
        formatType,
        formatElementType,
        ( const void* )data
    );

    GL::t_enum err = ::glGetError();

    if( err != GL_NO_ERROR ) {
        ctx->newError(
            err,
            "Failed to create one-dimensional texture"
        );
        assert( false );

        return false;
    }


    ::glBindTexture(
        GL_TEXTURE_1D,
        0
    );

    texture->d->width             = width;
    texture->d->height            = 0;
    texture->d->depth             = 0;
    texture->d->dim               = GL::ETextureDimension::Dim1;
    texture->d->format            = format;
    texture->d->internalFormat    = internalType;

    ctx->currentDbgContext()->reportNewTextureData(
        ctx,
        texture
    );

    return true;

}

bool GL::createTextureData2D( GL* ctx, GLTexture* texture, ETextureDataFormat::t format, t_uint width, t_uint height, t_void* data ) {
    assert( ctx );
    assert( texture );
    assert( format != ETextureDataFormat::Empty );
    assert( width > 0 );
    assert( height > 0 );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    ( void ) ::glGetError();

    if( !GL::checkForPowerOf2( ctx, width ) ) {
        qDebug() << "Invalid texture dimensions( width=" << width << ", height=" << height << ").";
        ctx->newError(
            ERROR_INTERNAL,
            "Intended texture dimensions are not power of two."
        );
        assert( false );

        return false;
    }


    if( ( data != nullptr ) && ( texture->width() == width ) && ( texture->height() == height ) ) {
        return GL::updateTextureData(
                   ctx,
                   texture,
                   0,
                   0,
                   0,
                   width,
                   height,
                   0,
                   data
               );
    }

    GL::t_enum  formatType( GL::getOpenGLInternalTextureFormatType( format ) );
    GL::t_enum  formatElementType( GL::getOpenGLInternalTextureFormatElementType( format ) );
    GL::t_enum  internalType( GL::getOpenGLInternalTextureFormat( format ) );

    ::glBindTexture(
        GL_TEXTURE_2D,
        texture->d->textureId
    );

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

    ::glTexImage2D(
        GL_TEXTURE_2D,
        0,
        ( GLint )internalType,
        width,
        height,
        0,
        formatType,
        formatElementType,
        ( const void* )data
    );

    GL::t_enum err = ::glGetError();

    if( err != GL_NO_ERROR ) {
        ctx->newError(
            err,
            "Failed to create two-dimensional texture"
        );
        assert( false );

        return false;
    }

    ::glBindTexture(
        GL_TEXTURE_2D,
        0
    );

    texture->d->width             = width;
    texture->d->height            = height;
    texture->d->depth             = 0;
    texture->d->dim               = GL::ETextureDimension::Dim2;
    texture->d->format            = format;
    texture->d->internalFormat    = internalType;

    ctx->currentDbgContext()->reportNewTextureData(
        ctx,
        texture
    );

    return true;

}

bool GL::createTextureData3D( GL* ctx, GLTexture* texture, ETextureDataFormat::t format, t_uint width, t_uint height,  t_uint depth, t_void* data ) {
    assert( ctx );
    assert( texture );
    assert( format != ETextureDataFormat::Empty );
    assert( width > 0 );
    assert( height > 0 );
    assert( depth > 0 );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    ( void ) ::glGetError();

    if( !GL::checkForPowerOf2( ctx, width, height ) ) {
        qDebug() << "Invalid texture dimensions( width=" << width << ", height=" << height << ", depth=" << depth << " ).";
        ctx->newError(
            ERROR_INTERNAL,
            "Failed to create three-dimensional texture"
        );
        assert( false );

        return false;
    }

    if( ( data != nullptr ) && ( texture->width() == width ) && ( texture->height() == height ) && ( texture->depth() == depth ) ) {
        return GL::updateTextureData(
                   ctx,
                   texture,
                   0,
                   0,
                   0,
                   width,
                   height,
                   depth,
                   data
               );
    }

    GL::t_enum  formatType( GL::getOpenGLInternalTextureFormatType( format ) );
    GL::t_enum  formatElementType( GL::getOpenGLInternalTextureFormatElementType( format ) );
    GL::t_enum  internalType( GL::getOpenGLInternalTextureFormat( format ) );

    ::glBindTexture(
        GL_TEXTURE_3D,
        texture->d->textureId
    );

    glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT );
    glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT );
    glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

    ::glTexImage3D(
        GL_TEXTURE_3D,
        0,
        ( GLint )internalType,
        width,
        height,
        depth,
        0,
        formatType,
        formatElementType,
        ( const void* )data
    );

    GL::t_enum err = ::glGetError();

    if( err != GL_NO_ERROR ) {
        ctx->newError(
            err,
            "Failed to create three-dimensional texture"
        );
        assert( false );

        return false;
    }

    ::glBindTexture(
        GL_TEXTURE_3D,
        0
    );

    texture->d->width             = width;
    texture->d->height            = height;
    texture->d->depth             = depth;
    texture->d->dim               = GL::ETextureDimension::Dim3;
    texture->d->format            = format;
    texture->d->internalFormat    = internalType;

    ctx->currentDbgContext()->reportNewTextureData(
        ctx,
        texture
    );

    return true;

}

bool GL::updateTextureData( GL* ctx, GLTexture* texture, t_int x, t_int y, t_int z, t_sizei width, t_sizei height,  t_uint depth, t_void* data ) {
    assert( ctx );
    assert( texture );
    assert( texture->format() != ETextureDataFormat::Empty );
    assert( data );

    ( void ) ::glGetError();

    GL::t_enum  formatType( GL::getOpenGLInternalTextureFormatType( texture->format() ) );
    GL::t_enum  formatElementType( GL::getOpenGLInternalTextureFormatElementType( texture->format() ) );

    if( texture->dimension() == GL::ETextureDimension::Dim1 ) {
        assert( width > 0 );

        if( !GL::checkForPowerOf2( ctx, width ) ) {
            qDebug() << "Invalid texture dimensions( width=" << width << " ).";
            assert( false );
            return false;
        }

        ::glBindTexture(
            GL_TEXTURE_1D,
            texture->d->textureId
        );

        ::glTexSubImage1D(
            GL_TEXTURE_1D,
            0,
            x,
            width,
            formatType,
            formatElementType,
            data
        );

        ::glBindTexture(
            GL_TEXTURE_1D,
            0
        );

        GL::t_enum err = ::glGetError();

        if( err != GL_NO_ERROR ) {
            ctx->newError(
                err,
                "Failed to update one-dimensional texture"
            );
            assert( false );

            return false;
        }

        return true;

    } else if( texture->dimension() == GL::ETextureDimension::Dim2 ) {
        assert( width > 0 );
        assert( height > 0 );

        if( !GL::checkForPowerOf2( ctx, width, height ) ) {
            qDebug() << "Invalid texture dimensions( width=" << width << ", height=" << height << " ).";
            assert( false );
            return false;
        }

        glActiveTexture( GL_TEXTURE0 );
        ::glBindTexture(
            GL_TEXTURE_2D,
            texture->d->textureId
        );

        ::glTexSubImage2D(
            GL_TEXTURE_2D,
            0,
            x,
            y,
            width,
            height,
            formatType,
            formatElementType,
            data
        );

        ::glBindTexture(
            GL_TEXTURE_2D,
            0
        );

        GL::t_enum err = ::glGetError();

        if( err != GL_NO_ERROR ) {
            ctx->newError(
                err,
                "Failed to update two-dimensional texture"
            );
            assert( false );

            return false;
        }

        return true;
    } else if( texture->dimension() == GL::ETextureDimension::Dim3 ) {
        assert( width > 0 );
        assert( height > 0 );
        assert( depth > 0 );

        if( !GL::checkForPowerOf2( ctx, width, height ) ) {
            qDebug() << "Invalid texture dimensions( width=" << width << ", height=" << height << ", depth=" << depth << " ).";
            assert( false );
            return false;
        }

        ::glBindTexture(
            GL_TEXTURE_3D,
            texture->d->textureId
        );

        glTexSubImage3D(
            GL_TEXTURE_3D,
            0,
            x,
            y,
            z,
            width,
            height,
            depth,
            formatType,
            formatElementType,
            data
        );

        ::glBindTexture(
            GL_TEXTURE_3D,
            0
        );

        GL::t_enum err = ::glGetError();

        if( err != GL_NO_ERROR ) {
            ctx->newError(
                err,
                "Failed to update three-dimensional texture"
            );
            assert( false );

            return false;
        }

        return true;
    }

    return false;

}

bool GL::updateTextureData( GL* ctx, GLTexture* texture, t_void* data ) {
    return GL::updateTextureData(
               ctx,
               texture,
               0,
               0,
               0,
               texture->width(),
               texture->height(),
               texture->depth(),
               data
           );
}

/** raw buffers **/

bool GL::createRawBuffer( GL* ctx, GLRawBuffer** outRef ) {
    assert( ctx );
    assert( outRef );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    ( void ) ::glGetError();

    *outRef = new GLRawBuffer();

    glGenBuffers(
        1,
        &( *outRef )->d->bufferId
    );

    return true;
}

bool GL::createRawBuffer( GL* ctx, GLRawBuffer** outRef, void* data, size_t length, ERawBufferHint::t hint ) {
    assert( ctx );
    assert( outRef );
    assert( data );
    assert( length > 0 );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    if( GL::createRawBuffer( ctx, outRef ) ) {

        return GL::createRawBufferData(
                   ctx,
                   *outRef,
                   data,
                   length,
                   hint
               );

    }

    return false;

}

bool GL::createRawBuffer( GL* ctx, GLRawBuffer** outRef, size_t length, ERawBufferHint::t hint ) {
    assert( ctx );
    assert( outRef );
    assert( length > 0 );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    if( GL::createRawBuffer( ctx, outRef ) ) {

        return GL::createRawBufferData(
                   ctx,
                   *outRef,
                   length,
                   hint
               );

    }

    return false;

}

static GL::t_enum bufferHintToOpenGLUsage( GL::ERawBufferHint::t hint ) {
    switch( hint ) {
        case GL::ERawBufferHint::DynamicDownload:
            return GL_DYNAMIC_READ;

        case GL::ERawBufferHint::DynamicUpload:
            return GL_DYNAMIC_DRAW;

        case GL::ERawBufferHint::StaticDownload:
            return GL_STATIC_READ;

        case GL::ERawBufferHint::StaticUpload:
            return GL_STATIC_DRAW;

        case GL::ERawBufferHint::StreamDownload:
            return GL_STREAM_READ;

        case GL::ERawBufferHint::StreamUpload:
            return GL_STREAM_DRAW;

        case GL::ERawBufferHint::None:
        default:
            return GL_DYNAMIC_READ;
    }
}

bool GL::createRawBufferData( GL* ctx, GLRawBuffer* buffer, size_t length, ERawBufferHint::t hint ) {
    return GL::createRawBufferData(
               ctx,
               buffer,
               NULL,
               length,
               hint
           );
}

bool GL::createRawBufferData( GL* ctx, GLRawBuffer* buffer, void* data, size_t length, ERawBufferHint::t hint ) {
    assert( ctx );
    assert( length > 0 );
    assert( buffer );

    ( void ) ::glGetError();

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    glBindBuffer(
        GL_ARRAY_BUFFER,
        buffer->id()
    );

    glBufferData(
        GL_ARRAY_BUFFER,
        ( GLsizeiptr )length,
        data,
        bufferHintToOpenGLUsage( hint )
    );

    glBindBuffer(
        GL_ARRAY_BUFFER,
        0
    );

    GL::t_enum err = ::glGetError();

    if( err != GL_NO_ERROR ) {
        ctx->newError(
            err,
            "Failed to create raw buffer"
        );
        assert( false );

        return false;
    }

    buffer->d->size     = ( GL::t_sizei )length;
    buffer->d->hint     = hint;

    ctx->currentDbgContext()->reportNewRawData(
        ctx,
        ( GL::t_sizei )length
    );

    return true;
}

bool GL::writeRawBufferData( GL* ctx, GLRawBuffer* buffer, const void* data, size_t length, size_t offset ) {
    assert( ctx );
    assert( length > 0 );
    assert( buffer );

    ( void ) ::glGetError();

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    if( buffer->empty() ) {
        assert( false );
        return false;
    }

    glBindBuffer(
        GL_ARRAY_BUFFER,
        buffer->id()
    );

    void* gpuBuf = glMapBuffer(
                       GL_ARRAY_BUFFER,
                       GL_WRITE_ONLY
                   );

    if( !gpuBuf ) {
        glUnmapBuffer(
            GL_ARRAY_BUFFER
        );

        return false;
    }

    memcpy( ( void* )( ( char* )gpuBuf + offset ), ( const void* )data, length );

    glUnmapBuffer(
        GL_ARRAY_BUFFER
    );

    GL::t_enum err = ::glGetError();

    if( err != GL_NO_ERROR ) {
        ctx->newError(
            err,
            "Failed to write to raw buffer"
        );
        assert( false );

        return false;
    }

    return true;

}

bool GL::readRawBufferData( GL* ctx, GLRawBuffer* buffer, void* data, size_t length, size_t offset ) {

    assert( ctx );
    assert( length > 0 );
    assert( buffer );

    ( void ) ::glGetError();

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    if( buffer->empty() ) {
        assert( false );
        return false;
    }

    glBindBuffer(
        GL_ARRAY_BUFFER,
        buffer->id()
    );

    void* gpuBuf = glMapBuffer(
                       GL_ARRAY_BUFFER,
                       GL_READ_ONLY
                   );

    if( !gpuBuf ) {
        glUnmapBuffer(
            GL_ARRAY_BUFFER
        );

        return false;
    }

    memcpy( ( void* )data, ( const void* )( ( char* )gpuBuf + offset ), length );

    glUnmapBuffer(
        GL_ARRAY_BUFFER
    );

    GL::t_enum err = ::glGetError();

    if( err != GL_NO_ERROR ) {
        ctx->newError(
            err,
            "Failed to write to raw buffer"
        );
        assert( false );

        return false;
    }

    return true;

}

bool GL::disposeRawBufferData( GL* ctx, GLRawBuffer* buffer ) {
    assert( ctx );
    assert( buffer );

    ( void ) ::glGetError();

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    if( buffer->empty() ) {
        assert( false );
        return false;
    }

    glBindBuffer(
        GL_ARRAY_BUFFER,
        buffer->id()
    );

    glBufferData(
        GL_ARRAY_BUFFER,
        0,
        NULL,
        bufferHintToOpenGLUsage( buffer->hint() )
    );

    glBindBuffer(
        GL_ARRAY_BUFFER,
        0
    );

    GL::t_enum err = ::glGetError();

    if( err != GL_NO_ERROR ) {
        ctx->newError(
            err,
            "Failed to dispose raw buffer"
        );
        assert( false );

        return false;
    }

    ctx->currentDbgContext()->reportResourceDeallocation(
        ctx,
        ( GL::t_sizei )buffer->size()
    );
    buffer->d->size = 0;

    return true;
}

/** asynchronous texture upload **/

bool GL::createTextureAsync( GL* ctx, GLTexture** outRef, ETextureDimension::t dim, ETextureDataFormat::t format, t_uint width, t_uint height, t_uint depth, GLRawBuffer* data ) {
    switch( dim ) {
        case ETextureDimension::Dim1:
            return GL::createTexture1DAsync(
                       ctx,
                       outRef,
                       format,
                       width,
                       data
                   );

        case ETextureDimension::Dim2:
            return GL::createTexture2DAsync(
                       ctx,
                       outRef,
                       format,
                       width,
                       height,
                       data
                   );

        case ETextureDimension::Dim3:
            return GL::createTexture3DAsync(
                       ctx,
                       outRef,
                       format,
                       width,
                       height,
                       depth,
                       data
                   );

        default:
            return false;
    }
}

bool GL::createTexture1DAsync( GL* ctx, GLTexture** outRef, ETextureDataFormat::t format, t_uint width, GLRawBuffer* data ) {
    assert( ctx );
    assert( outRef );
    assert( format != ETextureDataFormat::Empty );
    assert( width > 0 );
    assert( data );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    if( GL::createTexture( ctx, outRef ) ) {

        return GL::createTextureData1DAsync(
                   ctx,
                   *outRef,
                   format,
                   width,
                   data
               );

    }

    return false;
}

bool GL::createTexture2DAsync( GL* ctx, GLTexture** outRef, ETextureDataFormat::t format, t_uint width, t_uint height, GLRawBuffer* data ) {
    assert( ctx );
    assert( outRef );
    assert( format != ETextureDataFormat::Empty );
    assert( width > 0 );
    assert( height > 0 );
    assert( data );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    if( GL::createTexture( ctx, outRef ) ) {

        return GL::createTextureData2DAsync(
                   ctx,
                   *outRef,
                   format,
                   width,
                   height,
                   data
               );

    }

    return false;
}

bool GL::createTexture3DAsync( GL* ctx, GLTexture** outRef, ETextureDataFormat::t format, t_uint width, t_uint height, t_uint depth, GLRawBuffer* data ) {
    assert( ctx );
    assert( outRef );
    assert( format != ETextureDataFormat::Empty );
    assert( width > 0 );
    assert( height > 0 );
    assert( depth > 0 );
    assert( data );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    if( GL::createTexture( ctx, outRef ) ) {

        return GL::createTextureData3DAsync(
                   ctx,
                   *outRef,
                   format,
                   width,
                   height,
                   depth,
                   data
               );

    }

    return false;
}


bool GL::createTextureDataAsync( GL* ctx, GLTexture* texture, ETextureDimension::t dim, ETextureDataFormat::t format, t_uint width, t_uint height, t_uint depth, GLRawBuffer* data ) {
    switch( dim ) {
        case ETextureDimension::Dim1:
            return GL::createTextureData1DAsync(
                       ctx,
                       texture,
                       format,
                       width,
                       data
                   );

        case ETextureDimension::Dim2:
            return GL::createTextureData2DAsync(
                       ctx,
                       texture,
                       format,
                       width,
                       height,
                       data
                   );

        case ETextureDimension::Dim3:
            return GL::createTextureData3DAsync(
                       ctx,
                       texture,
                       format,
                       width,
                       height,
                       depth,
                       data
                   );

        default:
            return false;
    }
}

bool GL::createTextureData1DAsync( GL* ctx, GLTexture* texture, ETextureDataFormat::t format, t_uint width, GLRawBuffer* data ) {
    assert( ctx );
    assert( texture );
    assert( format != ETextureDataFormat::Empty );
    assert( width > 0 );
    assert( data );
    assert( !data->empty() );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    ( void ) ::glGetError();

    if( !GL::checkForPowerOf2( ctx, width, 2 ) ) {
        qDebug() << "Invalid texture dimensions( width=" << width << " ).";
        assert( false );
        return false;
    }

    if( texture->width() == width ) {
        return GL::updateTextureData(
                   ctx,
                   texture,
                   0,
                   0,
                   0,
                   width,
                   0,
                   0,
                   data
               );
    }

    GL::t_enum  formatType( GL::getOpenGLInternalTextureFormatType( format ) );
    GL::t_enum  formatElementType( GL::getOpenGLInternalTextureFormatElementType( format ) );
    GL::t_enum  internalType( GL::getOpenGLInternalTextureFormat( format ) );

    ::glBindTexture(
        GL_TEXTURE_1D,
        texture->d->textureId
    );

    glTexParameterf( GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

    glBindBuffer(
        GL_PIXEL_UNPACK_BUFFER,
        data->id()
    );

    ::glTexImage1D(
        GL_TEXTURE_1D,
        0,
        ( GLint )internalType,
        width,
        0,
        formatType,
        formatElementType,
        NULL
    );

    glBindBuffer(
        GL_PIXEL_UNPACK_BUFFER,
        0
    );

    GL::t_enum err = ::glGetError();

    if( err != GL_NO_ERROR ) {
        ctx->newError(
            err,
            "Failed to create one-dimensional texture"
        );
        assert( false );

        return false;
    }


    ::glBindTexture(
        GL_TEXTURE_1D,
        0
    );

    texture->d->width             = width;
    texture->d->height            = 0;
    texture->d->depth             = 0;
    texture->d->dim               = GL::ETextureDimension::Dim1;
    texture->d->format            = format;
    texture->d->internalFormat    = internalType;

    ctx->currentDbgContext()->reportNewTextureData(
        ctx,
        texture
    );

    return true;

}

bool GL::createTextureData2DAsync( GL* ctx, GLTexture* texture, ETextureDataFormat::t format, t_uint width, t_uint height, GLRawBuffer* data ) {
    assert( ctx );
    assert( texture );
    assert( format != ETextureDataFormat::Empty );
    assert( width > 0 );
    assert( height > 0 );
    assert( data );
    assert( !data->empty() );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    ( void ) ::glGetError();

    if( !GL::checkForPowerOf2( ctx, width, height ) ) {
        qDebug() << "Invalid texture dimensions( width=" << width << ", height=" << height << " ).";
        assert( false );
        return false;
    }


    if( ( texture->width() == width ) && ( texture->height() == height ) ) {
        return GL::updateTextureData(
                   ctx,
                   texture,
                   0,
                   0,
                   0,
                   width,
                   height,
                   0,
                   data
               );
    }

    GL::t_enum  formatType( GL::getOpenGLInternalTextureFormatType( format ) );
    GL::t_enum  formatElementType( GL::getOpenGLInternalTextureFormatElementType( format ) );
    GL::t_enum  internalType( GL::getOpenGLInternalTextureFormat( format ) );

    ::glBindTexture(
        GL_TEXTURE_2D,
        texture->d->textureId
    );

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT );
    glTexEnvf( GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_TEXTURE );

    glBindBuffer(
        GL_PIXEL_UNPACK_BUFFER,
        data->id()
    );

    ::glTexImage2D(
        GL_TEXTURE_2D,
        0,
        ( GLint )internalType,
        width,
        height,
        0,
        formatType,
        formatElementType,
        NULL
    );

    glBindBuffer(
        GL_PIXEL_UNPACK_BUFFER,
        0
    );

    GL::t_enum err = ::glGetError();

    if( err != GL_NO_ERROR ) {
        ctx->newError(
            err,
            "Failed to create two-dimensional texture"
        );
        assert( false );

        return false;
    }

    ::glBindTexture(
        GL_TEXTURE_2D,
        0
    );

    texture->d->width             = width;
    texture->d->height            = height;
    texture->d->depth             = 0;
    texture->d->dim               = GL::ETextureDimension::Dim2;
    texture->d->format            = format;
    texture->d->internalFormat    = internalType;

    ctx->currentDbgContext()->reportNewTextureData(
        ctx,
        texture
    );
    return true;
}

bool GL::createTextureData3DAsync( GL* ctx, GLTexture* texture, ETextureDataFormat::t format, t_uint width, t_uint height,  t_uint depth, GLRawBuffer* data ) {
    assert( ctx );
    assert( texture );
    assert( format != ETextureDataFormat::Empty );
    assert( width > 0 );
    assert( height > 0 );
    assert( depth > 0 );
    assert( data );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    ( void ) ::glGetError();

    if( !GL::checkForPowerOf2( ctx, width, height, depth ) ) {
        qDebug() << "Invalid texture dimensions( width=" << width << ", height=" << height << ", depth=" << depth << " ).";
        assert( false );
        return false;
    }

    if( ( texture->width() == width ) && ( texture->height() == height ) && ( texture->depth() == depth ) ) {
        return GL::updateTextureData(
                   ctx,
                   texture,
                   0,
                   0,
                   0,
                   width,
                   height,
                   depth,
                   data
               );
    }

    GL::t_enum  formatType( GL::getOpenGLInternalTextureFormatType( format ) );
    GL::t_enum  formatElementType( GL::getOpenGLInternalTextureFormatElementType( format ) );
    GL::t_enum  internalType( GL::getOpenGLInternalTextureFormat( format ) );

    ::glBindTexture(
        GL_TEXTURE_3D,
        texture->d->textureId
    );

    glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT );
    glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT );
    glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

    glBindBuffer(
        GL_PIXEL_UNPACK_BUFFER,
        data->id()
    );

    ::glTexImage3D(
        GL_TEXTURE_3D,
        0,
        ( GLint )internalType,
        width,
        height,
        depth,
        0,
        formatType,
        formatElementType,
        NULL
    );

    glBindBuffer(
        GL_PIXEL_UNPACK_BUFFER,
        0
    );

    GL::t_enum err = ::glGetError();

    if( err != GL_NO_ERROR ) {
        ctx->newError(
            err,
            "Failed to create three-dimensional texture"
        );
        assert( false );
        return false;
    }

    ::glBindTexture(
        GL_TEXTURE_3D,
        0
    );

    texture->d->width             = width;
    texture->d->height            = height;
    texture->d->depth             = depth;
    texture->d->dim               = GL::ETextureDimension::Dim3;
    texture->d->format            = format;
    texture->d->internalFormat    = internalType;

    ctx->currentDbgContext()->reportNewTextureData(
        ctx,
        texture
    );

    return true;
}

/** synchronous texture data retrieval **/
bool GL::retrieveTextureData( GL* ctx, GLTexture* texture, t_sizei width, t_sizei height,  t_uint depth, t_void* outData ) {
    assert( ctx );
    assert( texture );
    assert( texture->format() != ETextureDataFormat::Empty );
    assert( outData );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    ( void ) ::glGetError();

    GL::t_enum  formatType( GL::getOpenGLInternalTextureFormatType( texture->format() ) );
    GL::t_enum  formatElementType( GL::getOpenGLInternalTextureFormatElementType( texture->format() ) );
    GLenum target( GL_TEXTURE_1D );

    if( texture->dimension() == GL::ETextureDimension::Dim1 ) {
        if( !GL::checkForPowerOf2( ctx, width ) ) {
            qDebug() << "Invalid texture dimensions( width=" << width << " ).";
            assert( false );
            return false;
        }

        target = GL_TEXTURE_1D;
    } else if( texture->dimension() == GL::ETextureDimension::Dim2 ) {
        if( !GL::checkForPowerOf2( ctx, width, height ) ) {
            qDebug() << "Invalid texture dimensions( width=" << width << ", height=" << height << " ).";
            assert( false );
            return false;
        }

        target = GL_TEXTURE_2D;
    } else if( texture->dimension() == GL::ETextureDimension::Dim3 ) {
        if( !GL::checkForPowerOf2( ctx, width, height, depth ) ) {
            qDebug() << "Invalid texture dimensions( width=" << width << ", height=" << height << ", depth=" << depth << " ).";
            assert( false );
            return false;
        }

        target = GL_TEXTURE_3D;
    } else {
        assert( false );

        return false;
    }

    glBindTexture(
        target,
        texture->d->textureId
    );

    glGetTexImage(
        target,
        0,
        formatType,
        formatElementType,
        ( GLvoid* )outData
    );

    glBindTexture(
        target,
        0
    );

    GL::t_enum err = ::glGetError();

    if( err != GL_NO_ERROR ) {
        ctx->newError(
            err,
            "Failed to retrieve texture data"
        );
        assert( false );
        return false;
    }

    return true;

}

bool GL::retrieveTextureData( GL* ctx, GLTexture* texture, t_void* outData ) {

    return retrieveTextureData(
               ctx,
               texture,
               texture->width(),
               texture->height(),
               texture->depth(),
               outData
           );

}


/** asynchronous texture data retrieval using dma **/
bool GL::retrieveTextureDataAsync( GL* ctx, GLTexture* texture, t_sizei width, t_sizei height,  t_uint depth, GLRawBuffer* outData ) {
    assert( ctx );
    assert( texture );
    assert( texture->format() != ETextureDataFormat::Empty );
    assert( outData );
    assert( !outData->empty() );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    ( void ) ::glGetError();

    GL::t_enum  formatType( GL::getOpenGLInternalTextureFormatType( texture->format() ) );
    GL::t_enum  formatElementType( GL::getOpenGLInternalTextureFormatElementType( texture->format() ) );
    GLenum target( GL_TEXTURE_1D );

    if( texture->dimension() == GL::ETextureDimension::Dim1 ) {
        if( !GL::checkForPowerOf2( ctx, width ) ) {
            qDebug() << "Invalid texture dimensions( width=" << width << " ).";
            assert( false );
            return false;
        }

        target = GL_TEXTURE_1D;
    } else if( texture->dimension() == GL::ETextureDimension::Dim2 ) {
        if( !GL::checkForPowerOf2( ctx, width, height ) ) {
            qDebug() << "Invalid texture dimensions( width=" << width << ", height=" << height << " ).";
            assert( false );
            return false;
        }

        target = GL_TEXTURE_2D;
    } else if( texture->dimension() == GL::ETextureDimension::Dim3 ) {
        if( !GL::checkForPowerOf2( ctx, width, height, depth ) ) {
            qDebug() << "Invalid texture dimensions( width=" << width << ", height=" << height << ", depth=" << depth << " ).";
            assert( false );
            return false;
        }

        target = GL_TEXTURE_3D;
    } else {
        assert( false );
        return false;
    }

    glBindTexture(
        target,
        texture->d->textureId
    );

    glBindBuffer(
        GL_PIXEL_PACK_BUFFER,
        outData->id()
    );

    glGetTexImage(
        target,
        0,
        formatType,
        formatElementType,
        NULL
    );

    glBindBuffer(
        GL_PIXEL_PACK_BUFFER,
        0
    );

    glBindTexture(
        target,
        0
    );

    GL::t_enum err = ::glGetError();

    if( err != GL_NO_ERROR ) {
        ctx->newError(
            err,
            "Failed to retrieve texture data"
        );
        assert( false );
        return false;
    }

    return true;

}

bool GL::retrieveTextureDataAsync( GL* ctx, GLTexture* texture, GLRawBuffer* outData ) {

    return retrieveTextureDataAsync(
               ctx,
               texture,
               texture->width(),
               texture->height(),
               texture->depth(),
               outData
           );

}

/** general texture functions **/
bool GL::bindTexture( GL* ctx, GLTexture* texture ) {
    assert( ctx );
    assert( texture );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    GLenum target( GL_TEXTURE_1D );

    if( texture->dimension() == GL::ETextureDimension::Dim1 ) {
        target = GL_TEXTURE_1D;
    } else if( texture->dimension() == GL::ETextureDimension::Dim2 ) {
        target = GL_TEXTURE_2D;
    } else if( texture->dimension() == GL::ETextureDimension::Dim3 ) {
        target = GL_TEXTURE_3D;
    } else {
        assert( false );

        return false;
    }

    ( void )::glGetError();

    glBindTexture(
        target,
        texture->d->textureId
    );

#ifdef _PERF
    const auto err = ::glGetError();

    if( err != GL_NO_ERROR ) {
        assert( false );
        return false;
    }

#endif

    return true;
}

bool GL::unbindTexture( GL* ctx, GLTexture* texture ) {
    assert( ctx );
    assert( texture );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    GLenum target( GL_TEXTURE_1D );

    if( texture->dimension() == GL::ETextureDimension::Dim1 ) {
        target = GL_TEXTURE_1D;
    } else if( texture->dimension() == GL::ETextureDimension::Dim2 ) {
        target = GL_TEXTURE_2D;
    } else if( texture->dimension() == GL::ETextureDimension::Dim3 ) {
        target = GL_TEXTURE_3D;
    } else {
        assert( false );

        return false;
    }

    glBindTexture(
        target,
        0
    );

    return true;

}

bool GL::activeTexture( GL* ctx, t_enum texturePort ) {
    assert( ctx );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    glActiveTexture( GL_TEXTURE0 + texturePort );

    return true;
}


///
/// FRAMEBUFFER MANAGEMENT
///

struct GLFrameBuffer::GLPriv : libcommon::PimplPrivate {
    ::GLuint    id;
    ::GLuint    rid;
    GLTexture*  associatedTexture;

    GLPriv() : id( 0 ), rid( 0 ), associatedTexture( nullptr ) {}
};

GLFrameBuffer::GLFrameBuffer() : d( new GLPriv() ) {}

GL::t_uint  GLFrameBuffer::id() const {
    return d->id;
}

bool GLFrameBuffer::empty() const {
    return ( this->d->id == 0 );
}

GLTexture* GLFrameBuffer::associatedTexture() const {
    return d->associatedTexture;
}

bool GL::createFrameBuffer( GL* ctx, GLFrameBuffer** outRef ) {
    assert( ctx );
    assert( outRef );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    *outRef = new GLFrameBuffer();

    ( *outRef )->d->id = 0;
    ( *outRef )->d->rid = 0;
    ( *outRef )->d->associatedTexture = nullptr;

    glGenFramebuffers( 1, &( *outRef )->d->id );
    glGenRenderbuffers( 1, &( *outRef )->d->rid );

    ctx->currentDbgContext()->reportNewFrameBuffer(
        ctx,
        *outRef
    );

    return true;
}

bool GL::attachTextureToFrameBuffer( GL* ctx, GLFrameBuffer* fbo, GLTexture* texture, bool bit16Rbo ) {

    assert( ctx );
    assert( texture );
    assert( fbo );

    if( !ctx->initialized() ) {
        LOG_ERROR( "GL* ctx is not initialized" );
        assert( false );
        return false;
    }

    if( !fbo->empty() && !texture->empty() ) {
        glBindRenderbuffer( GL_RENDERBUFFER, fbo->d->rid );
        glBindFramebuffer( GL_FRAMEBUFFER, fbo->d->id );

        glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->d->textureId, 0 );

        if( bit16Rbo ) {
            glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, texture->width(), texture->height() );
        } else {
            glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, texture->width(), texture->height() );
        }

        glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fbo->d->rid );

        GLenum ret = glCheckFramebufferStatus( GL_FRAMEBUFFER );
        GLenum err = glGetError();

        if( ret != GL_FRAMEBUFFER_COMPLETE ) {
            switch( ret ) {
                case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                    ctx->newError(
                        err,
                        "Failed to create framebuffer. Incomplete attachment."
                    );
                    assert( false );
                    break;

                case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                    ctx->newError(
                        err,
                        "Failed to create framebuffer. Missing attachment."
                    );
                    assert( false );
                    break;

                case GL_FRAMEBUFFER_UNSUPPORTED:
                    ctx->newError(
                        err,
                        "Failed to create framebuffer. Framebuffers not supported."
                    );
                    assert( false );
                    break;

                default:
                    break;
            }

            assert( false );

            glBindFramebuffer( GL_FRAMEBUFFER, 0 );
            glBindRenderbuffer( GL_RENDERBUFFER, 0 );

            return false;
        }

        if( err != GL_NO_ERROR ) {
            assert( false );

            glBindFramebuffer( GL_FRAMEBUFFER, 0 );
            glBindRenderbuffer( GL_RENDERBUFFER, 0 );

            return false;
        }

        fbo->d->associatedTexture = texture;

        glBindFramebuffer( GL_FRAMEBUFFER, 0 );
        glBindRenderbuffer( GL_RENDERBUFFER, 0 );

        return true;
    }

    return false;
}

bool GL::bindFrameBuffer( GL* ctx, GLFrameBuffer* fbo ) {
    assert( ctx );
    assert( fbo );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    if( !fbo->empty() ) {
        glBindFramebuffer(
            GL_FRAMEBUFFER,
            fbo->d->id
        );

        return true;
    }

    return false;
}

bool GL::unbindFrameBuffer( GL* ctx, GLFrameBuffer* fbo ) {
    assert( ctx );
    assert( fbo );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    if( !fbo->empty() ) {
        glBindFramebuffer(
            GL_FRAMEBUFFER,
            0
        );

        return true;
    }

    return false;
}

/** GLShader **/
struct GLShader::GLPriv : libcommon::PimplPrivate {
    std::string     source;
    std::vector<std::string>    errors;
    GLuint  id;
    GL::EShaderType::t  type;

    GLPriv() : id( 0 ), type( GL::EShaderType::Fragment ) {}
};

GLShader::GLShader() : d( new GLPriv() ) {}

const std::string& GLShader::source() const {
    return d->source;
}

GL::t_uint  GLShader::id() const {
    return d->id;
}

const std::vector<std::string>& GLShader::errors() const {
    return d->errors;
}

bool GLShader::compiled() const {
    return ( d->id != 0 ) && ( d->errors.empty() );
}

GL::EShaderType::t GLShader::type() const {
    return d->type;
}

///
/// SHADER/PROGRAM MANAGEMENT
///

bool GL::createVertexShader( GL* ctx, GLShader** outRef, const std::string& source, std::vector<std::string>* errors ) {
    assert( ctx );
    assert( outRef );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    ( void )glGetError();

    *outRef = new GLShader();
    ( *outRef )->d->id = glCreateShader( GL_VERTEX_SHADER );
    ( *outRef )->d->type = GL::EShaderType::Vertex;

    const char* src = source.c_str();
    GLint       len = ( GLint )source.size();
    glShaderSource( ( *outRef )->d->id, 1, &src, &len );

    glCompileShader( ( *outRef )->d->id );

    GLint compileStatus = 0;
    glGetShaderiv( ( *outRef )->d->id, GL_COMPILE_STATUS, &compileStatus );

    if( compileStatus != GL_TRUE ) {
        int  logLength = 0;
        glGetShaderiv( ( *outRef )->d->id, GL_INFO_LOG_LENGTH, &logLength );
        char* log = new char[ logLength ];
        glGetShaderInfoLog( ( *outRef )->d->id, logLength, 0, ( GLchar* )log );

        if( errors != NULL ) {
            errors->push_back( std::string( log, logLength ) );
        }

        ( *outRef )->d->errors.push_back( std::string( log, logLength ) );

        GLenum err = glGetError();

        ctx->newError(
            err,
            "Failed to compile vertex shader: " + errors->back()
        );

        return false;
    }

    ctx->currentDbgContext()->reportNewShader(
        ctx,
        *outRef
    );

    return true;
}

bool GL::createFragmentShader( GL* ctx, GLShader** outRef, const std::string& source, std::vector<std::string>* errors ) {
    assert( ctx );
    assert( outRef );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    ( void )glGetError();

    *outRef = new GLShader();
    ( *outRef )->d->id = glCreateShader( GL_FRAGMENT_SHADER );
    ( *outRef )->d->type = GL::EShaderType::Fragment;

    const char* src = source.c_str();
    GLint       len = ( GLint )source.size();
    glShaderSource( ( *outRef )->d->id, 1, &src, &len );

    glCompileShader( ( *outRef )->d->id );

    GLint compileStatus = 0;
    glGetShaderiv( ( *outRef )->d->id, GL_COMPILE_STATUS, &compileStatus );

    if( compileStatus != GL_TRUE ) {
        int  logLength = 0;
        glGetShaderiv( ( *outRef )->d->id, GL_INFO_LOG_LENGTH, &logLength );
        char* log = new char[ logLength ];
        glGetShaderInfoLog( ( *outRef )->d->id, logLength, 0, ( GLchar* )log );

        if( errors != NULL ) {
            errors->push_back( std::string( log, logLength ) );
        }

        ( *outRef )->d->errors.push_back( std::string( log, logLength ) );

        GLenum err = glGetError();

        ctx->newError(
            err,
            "Failed to compile fragment shader: " + ( *outRef )->d->errors.back()
        );
        assert( false );

        return false;
    }

    ctx->currentDbgContext()->reportNewShader(
        ctx,
        *outRef
    );

    return true;

}

/** GLProgram **/

struct GLProgram::GLPriv : libcommon::PimplPrivate {
    GLuint  id;
    GLShader*   vertexShader;
    GLShader*   fragmentShader;
    std::vector<std::string>    errors;
    bool bound;
    bool linked;

    GLPriv() : id( 0 ), vertexShader( nullptr ), fragmentShader( nullptr ), bound( false ),
        linked( false ) {}
};

GLProgram::GLProgram() : d( new GLPriv() ) {}

const std::vector<std::string>& GLProgram::errors() const {
    return d->errors;
}

bool GLProgram::linked() const {
    return d->linked;
}

GL::t_uint  GLProgram::id() const {
    return d->id;
}

GLShader* GLProgram::attachedVertexShader() const {
    return d->vertexShader;
}

GLShader* GLProgram::attachedFragmentShader() const {
    return d->fragmentShader;
}

///
/// PROGRAM MANAGEMENT
///

bool GL::createProgram( GL* ctx, GLProgram** outRef ) {
    assert( ctx );
    assert( outRef );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    ( void )glGetError();

    *outRef = new GLProgram();
    ( *outRef )->d->id = glCreateProgram();

    GLenum err = glGetError();

    if( err != GL_NO_ERROR ) {
        ctx->newError(
            err,
            "Failed to create program."
        );
        assert( false );

        return false;
    }

    ctx->currentDbgContext()->reportNewProgram(
        ctx,
        *outRef
    );

    return true;
}

bool GL::createProgram( GL* ctx, GLProgram** outRef, GLShader* vertexShader, GLShader* fragmentShader, std::vector<std::string>* errors ) {
    assert( ctx );
    assert( outRef );
    assert( vertexShader );
    assert( fragmentShader );

    if( GL::createProgram( ctx, outRef ) ) {
        ( void ) glGetError();

        if( !attachShaderToProgram( ctx, *outRef, vertexShader ) ) {
            assert( false );
            return false;
        }

        if( !attachShaderToProgram( ctx, *outRef, fragmentShader ) ) {
            assert( false );
            return false;
        }

        if( !linkProgram( ctx, *outRef, errors ) ) {
            assert( false );
            return false;
        }

        return true;
    }

    return false;
}

bool GL::attachShaderToProgram( GL* ctx, GLProgram* program, GLShader* shader ) {
    assert( ctx );
    assert( program );
    assert( shader );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    if( shader->compiled() && !program->linked() ) {
        if( shader->type() == EShaderType::Fragment ) {
            program->d->fragmentShader = shader;
        } else {
            program->d->vertexShader = shader;
        }

        glAttachShader( program->d->id, shader->d->id );

        return true;
    }

    return false;

}

bool GL::linkProgram( GL* ctx, GLProgram* program, std::vector<std::string>* errors ) {
    assert( ctx );
    assert( program );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    ( void )glGetError();

    if( ( program->d->fragmentShader != NULL ) && ( program->d->vertexShader != NULL ) ) {
        if( !program->d->fragmentShader->compiled() ) {
            ctx->newError(
                0,
                "Failed to link program: Fragment shader not compiled."
            );
            assert( false );

            return false;
        }

        if( !program->d->vertexShader->compiled() ) {
            ctx->newError(
                0,
                "Failed to link program: Vertex shader not compiled."
            );
            assert( false );

            return false;
        }

        ( void ) glGetError();

        glLinkProgram( program->d->id );

        GLint status( 0 );
        glGetProgramiv( program->d->id, GL_LINK_STATUS, &status );

        if( status != GL_TRUE ) {
            int logLength = 0;
            glGetProgramiv( program->d->id, GL_INFO_LOG_LENGTH, &logLength );
            std::string log( logLength, '\0' );
            glGetProgramInfoLog( program->d->id, logLength, 0, ( GLchar* )log.c_str() );

            if( errors != NULL ) {
                errors->push_back( log );
            }

            ctx->newError( 0, "Failed to link program: " + log );
            assert( false );

            return false;
        }

        program->d->linked = true;

        return true;
    }

    return false;

}

bool GL::bindProgram( GL* ctx, GLProgram* program ) {
    assert( ctx );
    assert( program );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    ( void ) glGetError();

    if( !program->linked() ) {
        ctx->newError(
            0,
            "Failed to use program: program not linked"
        );
        assert( false );
        return false;
    }

    if( !program->d->bound ) {
        glUseProgram( program->d->id );
        program->d->bound = true;
    }

    return true;
}

bool GL::unbindProgram( GL* ctx, GLProgram* program ) {
    assert( ctx );
    assert( program );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    ( void ) glGetError();

    if( !program->linked() ) {
        ctx->newError(
            0,
            "Failed to use program: program not linked"
        );
        assert( false );

        return false;
    }

    if( program->d->bound ) {
        glUseProgram( 0 );

        program->d->bound = false;
    }

    return true;
}

///
/// UNIFORMS
///

bool GL::setUniform( GL* ctx, GLProgram* program, const std::string& uniformName, t_boolean val ) {
    assert( ctx );
    assert( program );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !ctx->initialized() ) {
        assert( false );
    }

#endif

    ( void ) glGetError();

    t_int position( 0 );

    if( !GL::getUniformLocation( ctx, program, uniformName, &position ) ) {
        return false;
    }

    return GL::setUniform( ctx, program, position, val );
}

bool GL::setUniform( GL* ctx, GLProgram* program, t_int uniformLocation, t_boolean val ) {
    assert( ctx );
    assert( program );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

#endif

    ( void ) glGetError();

    glUniform1i( uniformLocation, val );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT
    GLenum err = glGetError();

    if( err != GL_NO_ERROR ) {

        ctx->newError(
            err,
            "Failed to set uniform"
        );
        assert( false );

        return false;
    }

#endif

    return true;
}

bool GL::setUniform( GL* ctx, GLProgram* program, const std::string& uniformName, t_int val ) {
    assert( ctx );
    assert( program );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

#endif

    ( void ) glGetError();

    t_int position( 0 );

    if( !GL::getUniformLocation( ctx, program, uniformName, &position ) ) {
        return false;
    }

    return GL::setUniform( ctx, program, position, val );
}

bool GL::setUniform( GL* ctx, GLProgram* program, t_int uniformLocation, t_int val ) {
    assert( ctx );
    assert( program );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

#endif

    ( void ) glGetError();

    glUniform1i( uniformLocation, val );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT
    GLenum err = glGetError();

    if( err != GL_NO_ERROR ) {

        ctx->newError(
            err,
            "Failed to set uniform"
        );

        return false;
    }

#endif

    return true;
}

bool GL::setUniform( GL* ctx, GLProgram* program, const std::string& uniformName, t_uint val ) {
    assert( ctx );
    assert( program );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

#endif

    ( void ) glGetError();

    t_int position( 0 );

    if( !GL::getUniformLocation( ctx, program, uniformName, &position ) ) {
        return false;
    }

    return GL::setUniform( ctx, program, position, val );
}

bool GL::setUniform( GL* ctx, GLProgram* program, t_int uniformLocation, t_uint val ) {
    assert( ctx );
    assert( program );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

#endif

    ( void ) glGetError();

    glUniform1ui( uniformLocation, val );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT
    GLenum err = glGetError();

    if( err != GL_NO_ERROR ) {

        ctx->newError(
            err,
            "Failed to set uniform"
        );
        assert( false );

        return false;
    }

#endif

    return true;
}

bool GL::setUniform( GL* ctx, GLProgram* program, const std::string& uniformName, t_float val ) {
    assert( ctx );
    assert( program );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

#endif

    ( void ) glGetError();

    t_int position( 0 );

    if( !GL::getUniformLocation( ctx, program, uniformName, &position ) ) {
        return false;
    }

    return GL::setUniform( ctx, program, position, val );
}

bool GL::setUniform( GL* ctx, GLProgram* program, t_int uniformLocation, t_float val ) {
    assert( ctx );
    assert( program );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

#endif

    ( void ) glGetError();

    glUniform1f( uniformLocation, val );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT
    GLenum err = glGetError();

    if( err != GL_NO_ERROR ) {

        ctx->newError(
            err,
            "Failed to set uniform"
        );
        assert( false );

        return false;
    }

#endif

    return true;
}

bool GL::setUniform( GL* ctx, GLProgram* program, const std::string& uniformName, t_double val ) {
    assert( ctx );
    assert( program );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

#endif

    ( void ) glGetError();

    t_int position( 0 );

    if( !GL::getUniformLocation( ctx, program, uniformName, &position ) ) {
        return false;
    }

    return GL::setUniform( ctx, program, position, val );
}

bool GL::setUniform( GL* ctx, GLProgram* program, t_int uniformLocation, t_double val ) {
    assert( ctx );
    assert( program );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

#endif

    ( void ) glGetError();

#ifdef __APPLE__
    glUniform1f( uniformLocation, val );
#else
    glUniform1d( uniformLocation, val );
#endif

#ifdef LIBGRAPHICS_DEBUG_OUTPUT
    GLenum err = glGetError();

    if( err != GL_NO_ERROR ) {

        ctx->newError(
            err,
            "Failed to set uniform"
        );
        assert( false );

        return false;
    }

#endif

    return true;
}

bool GL::setUniform( GL* ctx, GLProgram* program, const std::string& uniformName, t_short val ) {
    assert( ctx );
    assert( program );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

#endif

    ( void ) glGetError();

    t_int position( 0 );

    if( !GL::getUniformLocation( ctx, program, uniformName, &position ) ) {
        return false;
    }

    return GL::setUniform( ctx, program, position, val );
}

bool GL::setUniform( GL* ctx, GLProgram* program, t_int uniformLocation, t_short val ) {
    assert( ctx );
    assert( program );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

#endif

    ( void ) glGetError();

    glUniform1i( uniformLocation, val );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT
    GLenum err = glGetError();

    if( err != GL_NO_ERROR ) {

        ctx->newError(
            err,
            "Failed to set uniform"
        );
        assert( false );

        return false;
    }

#endif

    return true;
}

bool GL::setUniform( GL* ctx, GLProgram* program, const std::string& uniformName, t_ushort val ) {
    assert( ctx );
    assert( program );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

#endif

    ( void ) glGetError();

    t_int position( 0 );

    if( !GL::getUniformLocation( ctx, program, uniformName, &position ) ) {
        assert( false );
        return false;
    }

    return GL::setUniform( ctx, program, position, val );
}

bool GL::setUniform( GL* ctx, GLProgram* program, t_int uniformLocation, t_ushort val ) {
    assert( ctx );
    assert( program );


#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

#endif

    glUniform1i( uniformLocation, val );


#ifdef LIBGRAPHICS_DEBUG_OUTPUT
    GLenum err = glGetError();

    if( err != GL_NO_ERROR ) {

        ctx->newError(
            err,
            "Failed to set uniform"
        );
        assert( false );

        return false;
    }

#endif

    return true;
}

bool GL::setUniformTexture( GL* ctx, GLProgram* program, const std::string& uniformName, t_uint sampler ) {
    assert( ctx );
    assert( program );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

#endif

    ( void ) glGetError();

    t_int position( 0 );

    if( !GL::getUniformLocation( ctx, program, uniformName, &position ) ) {
        assert( false );
        return false;
    }

    return GL::setUniformTexture( ctx, program, position, sampler );
}

bool GL::setUniformTexture( GL* ctx, GLProgram* program, t_int uniformLocation, t_uint sampler ) {
    assert( ctx );
    assert( program );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

#endif

    ( void ) glGetError();

    glUniform1i( uniformLocation, sampler );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT
    GLenum err = glGetError();

    if( err != GL_NO_ERROR ) {

        ctx->newError(
            err,
            "Failed to set uniform"
        );
        assert( false );

        return false;
    }

#endif

    return true;
}

bool GL::setUniformVector2f( GL* ctx, GLProgram* program, const std::string& uniformName, float* data ) {
    assert( ctx );
    assert( program );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

#endif

    ( void ) glGetError();

    t_int position( 0 );

    if( !GL::getUniformLocation( ctx, program, uniformName, &position ) ) {
        assert( false );
        return false;
    }

    return GL::setUniformVector2f( ctx, program, position, data );
}

bool GL::setUniformVector2f( GL* ctx, GLProgram* program, t_int uniformLocation, float* data ) {
    assert( ctx );
    assert( program );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

#endif

    ( void ) glGetError();

    glUniform2fv( uniformLocation, 1, data );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT
    GLenum err = glGetError();

    if( err != GL_NO_ERROR ) {

        ctx->newError(
            err,
            "Failed to set uniform"
        );
        assert( false );

        return false;
    }

#endif

    return true;
}

bool GL::setUniformVector3f( GL* ctx, GLProgram* program, const std::string& uniformName, float* data ) {
    assert( ctx );
    assert( program );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

#endif

    ( void ) glGetError();

    t_int position( 0 );

    if( !GL::getUniformLocation( ctx, program, uniformName, &position ) ) {
        assert( false );
        return false;
    }

    return GL::setUniformVector3f( ctx, program, position, data );
}

bool GL::setUniformVector3f( GL* ctx, GLProgram* program, t_int uniformLocation, float* data ) {
    assert( ctx );
    assert( program );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

#endif

    ( void ) glGetError();

    glUniform3fv( uniformLocation, 1, data );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT
    GLenum err = glGetError();

    if( err != GL_NO_ERROR ) {

        ctx->newError(
            err,
            "Failed to set uniform"
        );
        assert( false );

        return false;
    }

#endif

    return true;
}

bool GL::setUniformVector4f( GL* ctx, GLProgram* program, const std::string& uniformName, float* data ) {
    assert( ctx );
    assert( program );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

#endif

    ( void ) glGetError();

    t_int position( 0 );

    if( !GL::getUniformLocation( ctx, program, uniformName, &position ) ) {
        assert( false );
        return false;
    }

    return GL::setUniformVector4f( ctx, program, position, data );
}

bool GL::setUniformVector4f( GL* ctx, GLProgram* program, t_int uniformLocation, float* data ) {
    assert( ctx );
    assert( program );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

#endif

    ( void ) glGetError();

    glUniform4fv( uniformLocation, 1, data );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT
    GLenum err = glGetError();

    if( err != GL_NO_ERROR ) {

        ctx->newError(
            err,
            "Failed to set uniform"
        );
        assert( false );

        return false;
    }

#endif

    return true;
}

bool GL::setUniformMatrix9f( GL* ctx, GLProgram* program, const std::string& uniformName, float* data ) {
    assert( ctx );
    assert( program );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

#endif

    ( void ) glGetError();

    t_int position( 0 );

    if( !GL::getUniformLocation( ctx, program, uniformName, &position ) ) {
        assert( false );
        return false;
    }

    return GL::setUniformMatrix9f( ctx, program, position, data );
}

bool GL::setUniformMatrix9f( GL* ctx, GLProgram* program, t_int uniformLocation, float* data ) {
    assert( ctx );
    assert( program );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

#endif

    ( void ) glGetError();

    glUniformMatrix3fv( uniformLocation, 1, false, data );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT
    GLenum err = glGetError();

    if( err != GL_NO_ERROR ) {

        ctx->newError(
            err,
            "Failed to set uniform"
        );
        assert( false );

        return false;
    }

#endif

    return true;
}

bool GL::setUniformMatrix16f( GL* ctx, GLProgram* program, const std::string& uniformName, float* data ) {
    assert( ctx );
    assert( program );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

#endif

    ( void ) glGetError();

    t_int position( 0 );

    if( !GL::getUniformLocation( ctx, program, uniformName, &position ) ) {
        assert( false );
        return false;
    }

    return GL::setUniformMatrix16f( ctx, program, position, data );
}

bool GL::setUniformMatrix16f( GL* ctx, GLProgram* program, t_int uniformLocation, float* data ) {
    assert( ctx );
    assert( program );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

#endif

    ( void ) glGetError();

    glUniformMatrix4fv( uniformLocation, 1, false, data );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT
    GLenum err = glGetError();

    if( err != GL_NO_ERROR ) {

        ctx->newError(
            err,
            "Failed to set uniform"
        );
        assert( false );

        return false;
    }

#endif

    return true;
}

bool GL::getUniformLocation( GL* ctx, GLProgram* program, const std::string& uniformName, t_int* outLocation ) {
    assert( ctx );
    assert( program );
    assert( outLocation );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

#endif

    ( void ) glGetError();

    *outLocation = glGetUniformLocation( program->d->id, uniformName.c_str() );

    if( *outLocation == -1 ) {
        GLenum err = glGetError();

        ctx->newError(
            err,
            "Failed to retrieve uniform location"
        );
        assert( false );

        return false;
    }

    return true;
}

///
/// RENDERING HELPER
///

bool GL::drawEffectToFrameBuffer( GL* ctx, GLProgram* program, GLFrameBuffer* destinationBuffer ) {
    assert( ctx );
    assert( program );
    assert( destinationBuffer );
    assert( destinationBuffer->associatedTexture() );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    ( void ) glGetError();

    if( destinationBuffer->empty() || !program->linked() ) {
        ctx->newError(
            0,
            "Failed to render effect to specified destination: invalid parameters"
        );
        assert( false );

        return false;
    }

    GLint width = destinationBuffer->associatedTexture()->width();
    GLint height = destinationBuffer->associatedTexture()->height();

    glBindFramebuffer( GL_FRAMEBUFFER, destinationBuffer->d->id );
    glViewport( 0, 0, width, height );

    glClear( GL_COLOR_BUFFER_BIT );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0, width, 0, height, -1, 1 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    if( !program->d->bound ) {
        glUseProgram( program->d->id );
    }

    glBegin( GL_QUADS );
    glTexCoord2f( 0, 1 );
    glVertex2f( 0, 0 );
    glTexCoord2f( 0, 0 );
    glVertex2f( 0, ( float )height );
    glTexCoord2f( 1, 0 );
    glVertex2f( ( float )width, ( float )height );
    glTexCoord2f( 1, 1 );
    glVertex2f( ( float )width, 0 );
    glEnd();

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    return true;
}

bool GL::drawEffectToFrameBuffer( GL* ctx, GLProgram* program, GLFrameBuffer* destinationBuffer, const libgraphics::Rect32I& area ) {
    assert( ctx );
    assert( program );
    assert( destinationBuffer );
    assert( destinationBuffer->associatedTexture() );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    ( void ) glGetError();

    if( destinationBuffer->empty() || !program->linked() ) {
        ctx->newError(
            0,
            "Failed to render effect to specified destination: invalid parameters"
        );
        assert( false );

        return false;
    }

    GLint width = area.width;
    GLint height = area.height;

    glBindFramebuffer( GL_FRAMEBUFFER, destinationBuffer->d->id );
    glViewport( area.x, area.y, width, height );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0, width, 0, height, -1, 1 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    if( !program->d->bound ) {
        glUseProgram( program->d->id );
    }

    glBegin( GL_QUADS );

    glTexCoord2f( 0, 1 );
    glVertex2f( ( float )0, ( float )0 );
    glTexCoord2f( 0, 0 );
    glVertex2f( ( float )0, ( float )height );
    glTexCoord2f( 1, 0 );
    glVertex2f( ( float )width, ( float )height );
    glTexCoord2f( 1, 1 );
    glVertex2f( ( float )width, ( float )0 );
    glEnd();

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    return true;
}

bool GL::drawTexture2DToFrameBuffer( GL* ctx, GLTexture* texture, GLFrameBuffer* destinationBuffer ) {
    assert( ctx );
    assert( destinationBuffer );
    assert( destinationBuffer->associatedTexture() );
    assert( texture );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    ( void ) glGetError();

    if( destinationBuffer->empty() || texture->empty() ) {
        ctx->newError(
            0,
            "Failed to render texture to specified destination: invalid parameters"
        );
        assert( false );

        return false;
    }

    if( texture->dimension() != ETextureDimension::Dim2 ) {
        ctx->newError(
            0,
            "Failed to render texture to specified destination: invalid texture dimension"
        );
        assert( false );

        return false;
    }

    GLint width = destinationBuffer->associatedTexture()->width();
    GLint height = destinationBuffer->associatedTexture()->height();

    glBindFramebuffer( GL_FRAMEBUFFER, destinationBuffer->d->id );

    glViewport( 0, 0, width, height );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0, width, 0, height, -1.0, 1.0 );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glClear( GL_COLOR_BUFFER_BIT );


    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, texture->d->textureId );

    glBegin( GL_QUADS );
    glTexCoord2f( 0, 1 );
    glVertex2f( 0, 0 );
    glTexCoord2f( 0, 0 );
    glVertex2f( 0, ( float )height );
    glTexCoord2f( 1, 0 );
    glVertex2f( ( float )width, ( float )height );
    glTexCoord2f( 1, 1 );
    glVertex2f( ( float )width, 0 );
    glEnd();

    glBindTexture( GL_TEXTURE_2D, 0 );
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    return true;
}

bool GL::drawTexture2DToFrameBuffer( GL* ctx, GLTexture* texture, GLFrameBuffer* destinationBuffer, const libgraphics::Rect32I& area ) {
    assert( ctx );
    assert( destinationBuffer );
    assert( destinationBuffer->associatedTexture() );
    assert( texture );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    ( void ) glGetError();

    if( destinationBuffer->empty() || texture->empty() ) {
        ctx->newError(
            0,
            "Failed to render texture to specified destination: invalid parameters"
        );
        assert( false );

        return false;
    }

    if( texture->dimension() != ETextureDimension::Dim2 ) {
        ctx->newError(
            0,
            "Failed to render texture to specified destination: invalid texture dimension"
        );
        assert( false );

        return false;
    }

    GLint width = area.width;
    GLint height = area.height;

    glViewport( area.x, area.y, width, height );

    glBindFramebuffer( GL_FRAMEBUFFER, destinationBuffer->d->id );
    glClear( GL_COLOR_BUFFER_BIT );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( area.x, width, area.y, height, -1.0, 1.0 );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, texture->d->textureId );

    glBegin( GL_QUADS );
    glTexCoord2f( 0, 0 );
    glVertex2f( ( float )area.x, ( float )area.y );
    glTexCoord2f( 0, 1 );
    glVertex2f( ( float )area.x, ( float )height + area.y );
    glTexCoord2f( 1, 1 );
    glVertex2f( ( float )width + area.x, ( float )height + area.y );
    glTexCoord2f( 1, 0 );
    glVertex2f( ( float )width + area.x, ( float )area.y );
    glEnd();

    glBindTexture( GL_TEXTURE_2D, 0 );
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    return true;
}
bool GL::drawEffect( GL* ctx, GLProgram* program, int width, int height ) {
    assert( ctx );
    assert( program );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    ( void ) glGetError();

    if( !program->linked() ) {
        ctx->newError(
            0,
            "Failed to render to screen: program not linked"
        );
        assert( false );

        return false;
    }

    glViewport( 0, 0, width, height );

    glClear( GL_COLOR_BUFFER_BIT );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0, width, 0, height, -1, 1 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    if( !program->d->bound ) {
        glUseProgram( program->d->id );
    }

    glBegin( GL_QUADS );
    glTexCoord2f( 0, 1 );
    glVertex2f( 0, 0 );
    glTexCoord2f( 0, 0 );
    glVertex2f( 0, ( float )height );
    glTexCoord2f( 1, 0 );
    glVertex2f( ( float )width, ( float )height );
    glTexCoord2f( 1, 1 );
    glVertex2f( ( float )width, 0 );
    glEnd();

    return true;
}

bool GL::drawTexture2D( GL* ctx, GLTexture* texture, int width, int height ) {
    assert( ctx );
    assert( texture );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    ( void ) glGetError();

    if( texture->empty() ) {
        ctx->newError(
            0,
            "Failed to render texture to screen: texture is empty"
        );
        assert( false );
        return false;
    }

    if( texture->dimension() != ETextureDimension::Dim2 ) {
        ctx->newError(
            0,
            "Failed to render texture to specified destination: invalid texture dimension"
        );
        assert( false );
        return false;
    }

    glViewport( 0, 0, width, height );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    glClear( GL_COLOR_BUFFER_BIT );

    glOrtho( 0, width, 0, height, -1.0, 1.0 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, texture->d->textureId );

    glBegin( GL_QUADS );
    glTexCoord2f( 0, 1 );
    glVertex2f( 0, 0 );
    glTexCoord2f( 0, 0 );
    glVertex2f( 0, ( float )height );
    glTexCoord2f( 1, 0 );
    glVertex2f( ( float )width, ( float )height );
    glTexCoord2f( 1, 1 );
    glVertex2f( ( float )width, 0 );
    glEnd();

    glBindTexture( GL_TEXTURE_2D, 0 );

    return true;
}

///
/// RESOURCE DEALLOCATION
///

bool GL::destroyResource( GL* ctx, GLTexture* res ) {
    assert( ctx );
    assert( res );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    if( !res->empty() ) {
        ctx->currentDbgContext()->reportResourceDeallocation(
            ctx,
            res
        );
        static size_t counter = 0;
        ++counter;

        GLuint deletedTid = res->d->textureId;
        ( void )deletedTid;

        glDeleteTextures( 1, &res->d->textureId );

        res->d->textureId       = 0;
        res->d->depth           = 0;
        res->d->height          = 0;
        res->d->width           = 0;
        res->d->format          = ETextureDataFormat::Empty;

        return true;
    }

    return false;
}

bool GL::destroyResource( GL* ctx, GLShader* res ) {
    assert( ctx );
    assert( res );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    if( res->compiled() ) {
        ctx->currentDbgContext()->reportResourceDeallocation(
            ctx,
            res
        );

        glDeleteShader( res->d->id );

        res->d->errors.clear();
        res->d->id = 0;
        res->d->source.clear();

        return true;
    }

    return false;
}

bool GL::destroyResource( GL* ctx, GLFrameBuffer* res ) {
    assert( ctx );
    assert( res );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    if( !res->empty() ) {
        ctx->currentDbgContext()->reportResourceDeallocation(
            ctx,
            res
        );

        glDeleteBuffers( 1, &res->d->rid );
        glDeleteBuffers( 1, &res->d->id );

        res->d->associatedTexture = nullptr;
        res->d->id = 0;
        res->d->rid = 0;

        return true;
    }

    return false;
}

bool GL::destroyResource( GL* ctx, GLProgram* res ) {
    assert( ctx );
    assert( res );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    if( res->linked() ) {
        ctx->currentDbgContext()->reportResourceDeallocation(
            ctx,
            res
        );

        glDeleteProgram( res->d->id );

        res->d->fragmentShader = nullptr;
        res->d->vertexShader = 0;
        res->d->id = 0;
        res->d->bound = false;
        res->d->errors.clear();

        return true;
    }

    return false;
}

bool GL::destroyResource( GL* ctx, GLRawBuffer* res ) {
    assert( ctx );
    assert( res );

    if( !ctx->initialized() ) {
        assert( false );
        return false;
    }

    if( !res->empty() ) {
        ctx->currentDbgContext()->reportResourceDeallocation(
            ctx,
            res->size()
        );

        glDeleteBuffers( 1, &res->d->bufferId );

        return true;
    }

    return false;
}

}
}
