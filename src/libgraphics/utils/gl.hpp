#pragma once

#include <libgraphics/base.hpp>
#include <libcommon/atomics.hpp>
#include <libcommon/noncopyable.hpp>

#include <libgraphics/bitmap.hpp>

#include <vector>
#include <map>
#include <string>

namespace libgraphics {
namespace utils {
/// forward definition of structures
struct GL;
struct GLFrameBuffer;
struct GLShader;
struct GLProgram;
struct GLTexture;
struct GLDbgContext;
struct GLErr;
struct GLRawBuffer;

/// type bound api
struct GL : libcommon::INonCopyable {
        typedef unsigned int t_enum;
        typedef unsigned char t_boolean;
        typedef unsigned int t_bitfield;
        typedef signed char t_byte;
        typedef short t_short;
        typedef int t_int;
        typedef int t_sizei;
        typedef unsigned char t_ubyte;
        typedef unsigned short t_ushort;
        typedef unsigned int t_uint;
        typedef float t_float;
        typedef float t_clampf;
        typedef double t_double;
        typedef double t_clampd;
        typedef void t_void;

        struct GLPriv;

        /// constr./destr.
        virtual ~GL();

        void assignDbgContext( GLDbgContext* ctx );
        GLDbgContext* currentDbgContext();
        bool hasDbgContext() const;

        void newError( t_int errorType, const std::string& errorMessage );
        void newError( const std::string& errorType, const std::string& errorMessage );
        const GLErr* lastError() const;

        bool initialize();
        bool initialized() const;

        /// static initialization set
        static GL* construct();

        /// static function set to avoid name collision with
        /// original opengl headers.

        ///
        /// LEGACY MATRIX AND VIEW OPERATIONS
        ///

        static bool viewport( GL* ctx, t_int x, t_int y, t_int width, t_int height );
        static bool clearColors( GL* ctx, float r, float g, float b );
        static bool clearColorsAndDepth( GL* ctx, float r, float g, float b );

        enum EMatrixMode : unsigned int {
            ModelView = 0x1700,
            Projection = 0x1701
        };

        static bool setMatrixMode( GL* ctx, EMatrixMode mode );
        static bool loadIdentity( GL* ctx );
        static bool ortho(
            GL* ctx,
            float left,
            float right,
            float bottom,
            float top,
            float znear,
            float zfar
        );
        static bool translate(
            GL* ctx,
            float x,
            float y,
            float z
        );
        static bool scale(
            GL* ctx,
            float x,
            float y,
            float z
        );


        /// todo: standard projection

        ///
        /// RAW BUFFER CREATION
        ///

        struct ERawBufferHint {
            enum t {

                /** for images **/
                StaticUpload,
                StaticDownload,

                /** for most applications suitable **/
                StreamUpload,
                StreamDownload,

                /** for videos **/
                DynamicUpload,
                DynamicDownload,

                /** empty **/
                None
            };
        };

        static bool createRawBuffer( GL* ctx, GLRawBuffer** outRef );
        static bool createRawBuffer( GL* ctx, GLRawBuffer** outRef, void* data, size_t length, ERawBufferHint::t hint );
        static bool createRawBuffer( GL* ctx, GLRawBuffer** outRef, size_t length, ERawBufferHint::t hint );

        static bool createRawBufferData( GL* ctx, GLRawBuffer* buffer, size_t length, ERawBufferHint::t hint );
        static bool createRawBufferData( GL* ctx, GLRawBuffer* buffer, void* data, size_t length, ERawBufferHint::t hint );

        static bool writeRawBufferData( GL* ctx, GLRawBuffer* buffer, const void* data, size_t length, size_t offset );
        static bool readRawBufferData( GL* ctx, GLRawBuffer* buffer, void* data, size_t length, size_t offset );

        static bool disposeRawBufferData( GL* ctx, GLRawBuffer* buffer );

        ///
        /// TEXTURE MANAGEMENT
        ///

        struct ETextureDimension {
            enum t {
                Dim1,
                Dim2,
                Dim3
            };
        };
        static void enableTexture( GL* ctx, ETextureDimension::t dim );
        static void disableTexture( GL* ctx, ETextureDimension::t dim );
        static bool isTextureEnabled( GL* ctx, ETextureDimension::t dim );

        /// texture creations
        struct ETextureDataFormat {
            enum t {
                /// 3 and 4 channels
                RGB8,
                RGB16,
                RGB16F,
                RGB32,
                RGB32F,

                RGBA8,
                RGBA16,
                RGBA16F,
                RGBA32,
                RGBA32F,

                BGRA8,
                BGRA16,
                BGRA16F,
                BGRA32,
                BGRA32F,

                BGR8,
                BGR16,
                BGR16F,
                BGR32,
                BGR32F,

                /// 1 and 2 channels
                RG8,
                RG16,
                RG16F,
                RG32F,
                RG32,

                Mono8,
                Mono16,
                Mono16F,
                Mono32F,
                Mono32,

                /// Invalid/Unknown
                Empty
            };
        };
        static t_sizei getTextureFormatPixelSize( GL::ETextureDataFormat::t format );
        static t_enum getOpenGLInternalTextureFormat( GL::ETextureDataFormat::t format );
        static GL::ETextureDataFormat::t fromOpenGLInternalTextureFormat( GL::t_enum format );
        static t_enum getOpenGLInternalTextureFormatType( GL::ETextureDataFormat::t format );
        static t_enum getOpenGLInternalTextureFormatElementType( GL::ETextureDataFormat::t format );
        static std::string getOpenGLInternalTextureFormatName( GL::ETextureDataFormat::t format );

        static bool createTexture( GL* ctx, GLTexture** outRef );

        /** synchronous texture creation and upload **/
        static bool createTexture( GL* ctx, GLTexture** outRef, ETextureDimension::t dim, ETextureDataFormat::t format, t_uint width, t_uint height, t_uint depth, t_void* data );
        static bool createTexture1D( GL* ctx, GLTexture** outRef, ETextureDataFormat::t format, t_uint width, t_void* data );
        static bool createTexture2D( GL* ctx, GLTexture** outRef, ETextureDataFormat::t format, t_uint width, t_uint height, t_void* data );
        static bool createTexture3D( GL* ctx, GLTexture** outRef, ETextureDataFormat::t format, t_uint width, t_uint height, t_uint depth, t_void* data );

        static bool createTextureData( GL* ctx, GLTexture* texture, ETextureDimension::t dim, ETextureDataFormat::t format, t_uint width, t_uint height, t_uint depth, t_void* data );
        static bool createTextureData1D( GL* ctx, GLTexture* texture, ETextureDataFormat::t format, t_uint width, t_void* data );
        static bool createTextureData2D( GL* ctx, GLTexture* texture, ETextureDataFormat::t format, t_uint width, t_uint height, t_void* data );
        static bool createTextureData3D( GL* ctx, GLTexture* texture, ETextureDataFormat::t format, t_uint width, t_uint height,  t_uint depth, t_void* data );

        static bool updateTextureData( GL* ctx, GLTexture* texture, t_int x, t_int y,  t_int z, t_sizei width, t_sizei height,  t_uint depth, t_void* data );
        static bool updateTextureData( GL* ctx, GLTexture* texture, t_void* data );

        /** asynchronous texture creation and upload using dma  **/
        static bool createTextureAsync( GL* ctx, GLTexture** outRef, ETextureDimension::t dim, ETextureDataFormat::t format, t_uint width, t_uint height, t_uint depth, GLRawBuffer* data );
        static bool createTexture1DAsync( GL* ctx, GLTexture** outRef, ETextureDataFormat::t format, t_uint width, GLRawBuffer* data );
        static bool createTexture2DAsync( GL* ctx, GLTexture** outRef, ETextureDataFormat::t format, t_uint width, t_uint height, GLRawBuffer* data );
        static bool createTexture3DAsync( GL* ctx, GLTexture** outRef, ETextureDataFormat::t format, t_uint width, t_uint height,  t_uint depth, GLRawBuffer* data );

        static bool createTextureDataAsync( GL* ctx, GLTexture* texture, ETextureDimension::t dim, ETextureDataFormat::t format, t_uint width, t_uint height, t_uint depth, GLRawBuffer* data );
        static bool createTextureData1DAsync( GL* ctx, GLTexture* texture, ETextureDataFormat::t format, t_uint width, GLRawBuffer* data );
        static bool createTextureData2DAsync( GL* ctx, GLTexture* texture, ETextureDataFormat::t format, t_uint width, t_uint height, GLRawBuffer* data );
        static bool createTextureData3DAsync( GL* ctx, GLTexture* texture, ETextureDataFormat::t format, t_uint width, t_uint height,  t_uint depth, GLRawBuffer* data );

        static bool updateTextureDataAsync( GL* ctx, GLTexture* texture, t_int x, t_int y, t_sizei width, t_sizei height,  t_uint depth, GLRawBuffer* data );
        static bool updateTextureDataAsync( GL* ctx, GLTexture* texture, GLRawBuffer* data );

        /** synchronous texture data retrieval **/
        static bool retrieveTextureData( GL* ctx, GLTexture* texture, t_sizei width, t_sizei height,  t_uint depth, t_void* outData );
        static bool retrieveTextureData( GL* ctx, GLTexture* texture, t_void* outData );

        /** asynchronous texture data retrieval using dma **/
        static bool retrieveTextureDataAsync( GL* ctx, GLTexture* texture, t_sizei width, t_sizei height,  t_uint depth, GLRawBuffer* outData );
        static bool retrieveTextureDataAsync( GL* ctx, GLTexture* texture, GLRawBuffer* outData );

        /** general texture functions **/
        static bool bindTexture( GL* ctx, GLTexture* texture );
        static bool unbindTexture( GL* ctx, GLTexture* texture );
        static bool activeTexture( GL* ctx, t_enum texturePort );


        ///
        /// FRAMEBUFFER MANAGEMENT
        ///

        static bool createFrameBuffer( GL* ctx, GLFrameBuffer** outRef );
        static bool attachTextureToFrameBuffer( GL* ctx, GLFrameBuffer* fbo, GLTexture* texture, bool bit16Rbo = false );
        static bool bindFrameBuffer( GL* ctx, GLFrameBuffer* fbo );
        static bool unbindFrameBuffer( GL* ctx, GLFrameBuffer* fbo );


        ///
        /// SHADER/PROGRAM MANAGEMENT
        ///
        struct EShaderType {
            enum t {
                Vertex,
                Fragment
            };
        };

        static bool createVertexShader( GL* ctx, GLShader** outRef, const std::string& source, std::vector<std::string>* errors = nullptr );
        static bool createFragmentShader( GL* ctx, GLShader** outRef, const std::string& source, std::vector<std::string>* errors = nullptr );

        static bool createProgram( GL* ctx, GLProgram** outRef );
        static bool createProgram( GL* ctx, GLProgram** outRef, GLShader* vertexShader, GLShader* fragmentShader, std::vector<std::string>* errors );

        static bool attachShaderToProgram( GL* ctx, GLProgram* program, GLShader* shader );

        static bool linkProgram( GL* ctx, GLProgram* program, std::vector<std::string>* errors );
        static bool bindProgram( GL* ctx, GLProgram* program );
        static bool unbindProgram( GL* ctx, GLProgram* program );

        static bool setUniform( GL* ctx, GLProgram* program, const std::string& uniformName, t_boolean val );
        static bool setUniform( GL* ctx, GLProgram* program, t_int uniformLocation, t_boolean val );
        static bool setUniform( GL* ctx, GLProgram* program, const std::string& uniformName, t_int val );
        static bool setUniform( GL* ctx, GLProgram* program, t_int uniformLocation, t_int val );
        static bool setUniform( GL* ctx, GLProgram* program, const std::string& uniformName, t_uint val );
        static bool setUniform( GL* ctx, GLProgram* program, t_int uniformLocation, t_uint val );
        static bool setUniform( GL* ctx, GLProgram* program, const std::string& uniformName, t_float val );
        static bool setUniform( GL* ctx, GLProgram* program, t_int uniformLocation, t_float val );
        static bool setUniform( GL* ctx, GLProgram* program, const std::string& uniformName, t_double val );
        static bool setUniform( GL* ctx, GLProgram* program, t_int uniformLocation, t_double val );
        static bool setUniform( GL* ctx, GLProgram* program, const std::string& uniformName, t_short val );
        static bool setUniform( GL* ctx, GLProgram* program, t_int uniformLocation, t_short val );
        static bool setUniform( GL* ctx, GLProgram* program, const std::string& uniformName, t_ushort val );
        static bool setUniform( GL* ctx, GLProgram* program, t_int uniformLocation, t_ushort val );

        static bool setUniformTexture( GL* ctx, GLProgram* program, const std::string& uniformName, t_uint sampler );
        static bool setUniformTexture( GL* ctx, GLProgram* program, t_int uniformLocation, t_uint sampler );

        static bool setUniformVector2f( GL* ctx, GLProgram* program, const std::string& uniformName, float* data );
        static bool setUniformVector2f( GL* ctx, GLProgram* program, t_int uniformLocation, float* data );
        static bool setUniformVector3f( GL* ctx, GLProgram* program, const std::string& uniformName, float* data );
        static bool setUniformVector3f( GL* ctx, GLProgram* program, t_int uniformLocation, float* data );
        static bool setUniformVector4f( GL* ctx, GLProgram* program, const std::string& uniformName, float* data );
        static bool setUniformVector4f( GL* ctx, GLProgram* program, t_int uniformLocation, float* data );
        static bool setUniformMatrix9f( GL* ctx, GLProgram* program, const std::string& uniformName, float* data );
        static bool setUniformMatrix9f( GL* ctx, GLProgram* program, t_int uniformLocation, float* data );
        static bool setUniformMatrix16f( GL* ctx, GLProgram* program, const std::string& uniformName, float* data );
        static bool setUniformMatrix16f( GL* ctx, GLProgram* program, t_int uniformLocation, float* data );

        static bool getUniformLocation( GL* ctx, GLProgram* program, const std::string& uniformName, t_int* outLocation );

        ///
        /// RENDERING HELPER
        ///

        static bool drawEffectToFrameBuffer( GL* ctx, GLProgram* program, GLFrameBuffer* destinationBuffer );
        static bool drawTexture2DToFrameBuffer( GL* ctx, GLTexture* texture, GLFrameBuffer* destinationBuffer );
        static bool drawEffectToFrameBuffer( GL* ctx, GLProgram* program, GLFrameBuffer* destinationBuffer, const libgraphics::Rect32I& area );
        static bool drawTexture2DToFrameBuffer( GL* ctx, GLTexture* texture, GLFrameBuffer* destinationBuffer, const libgraphics::Rect32I& area );

        static bool drawEffect( GL* ctx, GLProgram* program, int width, int height );
        static bool drawTexture2D( GL* ctx, GLTexture* texture, int width, int height );

        ///
        /// RESOURCE DEALLOCATION
        ///

        static bool destroyResource( GL* ctx, GLTexture* res );
        static bool destroyResource( GL* ctx, GLShader* res );
        static bool destroyResource( GL* ctx, GLFrameBuffer* res );
        static bool destroyResource( GL* ctx, GLProgram* res );
        static bool destroyResource( GL* ctx, GLRawBuffer* res );
    private:

        /// private helpers
        static bool checkForPowerOf2( GL* ctx, GL::t_sizei width );
        static bool checkForPowerOf2( GL* ctx, GL::t_sizei width, GL::t_sizei height );
        static bool checkForPowerOf2( GL* ctx, GL::t_sizei width, GL::t_sizei height, GL::t_sizei depth );

        /// internal stuff
        GL();

        std::shared_ptr<GLPriv> d;
};

struct GLTexture {
        struct GLPriv;
        friend struct GL;

        GL::t_uint  id() const;
        GL::t_sizei width() const;
        GL::t_sizei height() const;
        GL::t_sizei depth() const;
        GL::ETextureDataFormat::t format() const;
        GL::ETextureDimension::t dimension() const;
        bool empty() const;
    private:
        GLTexture();
        std::shared_ptr<GLPriv> d;
};

struct GLFrameBuffer {
        struct GLPriv;
        friend struct GL;

        GL::t_uint  id() const;
        GLTexture* associatedTexture() const;
        bool empty() const;
    private:
        GLFrameBuffer();
        std::shared_ptr<GLPriv> d;
};

struct GLShader {
        struct GLPriv;
        friend struct GL;

        GL::t_uint  id() const;
        const std::string& source() const;
        const std::vector<std::string>& errors() const;
        bool compiled() const;
        GL::EShaderType::t type() const;
    private:
        GLShader();
        std::shared_ptr<GLPriv> d;
};

struct GLProgram {
        struct GLPriv;
        friend struct GL;

        GL::t_uint  id() const;
        const std::vector<std::string>& errors() const;
        bool linked() const;
        GLShader* attachedVertexShader() const;
        GLShader* attachedFragmentShader() const;
    private:
        GLProgram();
        std::shared_ptr<GLPriv> d;
};

struct GLErr {
        struct GLPriv;
        friend struct GL;

        GL* associatedOpenGLContext() const;
        const std::string& errorMessage() const;
        const std::string& errorType() const;
    private:
        GLErr();
        std::shared_ptr<GLPriv> d;
};

struct GLRawBuffer {
        struct GLPriv;
        friend struct GL;

        const GL::t_uint    id() const;
        const GL::t_sizei   size() const;
        const bool          empty() const;
        const GL::ERawBufferHint::t hint() const;
    private:
        GLRawBuffer();
        std::shared_ptr<GLPriv> d;
};

struct GLDbgContext {
        struct GLPriv;

        GLDbgContext();

        ///
        void reset();

        /// error management
        struct ErrInfo {
            const std::string message;
            const std::string type;

            ErrInfo( const std::string& msg, const std::string& tp ) :
                message( msg ), type( tp ) {}
        };
        struct CallInfo {
            const std::string function;
            const std::map<std::string, std::string> parameters;
            const bool valid;

            CallInfo( const std::string& _function, const std::map<std::string, std::string>& _parameters, bool _valid ) :
                function( _function ), parameters( _parameters ), valid( _valid ) {}
        };

        const std::vector<ErrInfo>& glErrors() const;
        const std::vector<CallInfo>& calls() const;

        /// stats
        GL::t_sizei     countShaders() const;
        GL::t_sizei     countPrograms() const;
        GL::t_sizei     countTextureObjects() const;
        GL::t_sizei     countFrameBuffers() const;

        GL::t_sizei     totalAllocatedData() const;
        GL::t_sizei     totalAllocatedTextureData() const;
        GL::t_sizei     totalAllocatedUnknownData() const;

        /// new objects
        void reportNewShader( GL* ctx, GLShader* object );
        void reportNewProgram( GL* ctx, GLProgram* program );
        void reportNewTextureObject( GL* ctx, GLTexture* texture );
        void reportNewTextureData( GL* ctx, GLTexture* texture );
        void reportNewRawData( GL* ctx, GL::t_sizei length );
        void reportNewFrameBuffer( GL* ctx, GLFrameBuffer* buffer );

        /// delete objects
        void reportResourceDeallocation( GL* ctx, GLShader* obj );
        void reportResourceDeallocation( GL* ctx, GLProgram* obj );
        void reportResourceDeallocation( GL* ctx, GLTexture* obj );
        void reportResourceDeallocation( GL* ctx, GLFrameBuffer* obj );
        void reportResourceDeallocation( GL* ctx, GL::t_sizei len );

        /// report calls
        void reportCall( GL* ctx, const std::string& function, const std::map<std::string, std::string>& parameters = ( std::map<std::string, std::string>() ) );
        void reportInvalidCall( GL* ctx, const std::string& function, const std::map<std::string, std::string>& parameters = ( std::map<std::string, std::string>() ) );
        void reportError( GL* ctx, GLErr* err );
    private:
        std::shared_ptr<GLPriv> d;
};

}
}
