#include <libgraphics/fx/operations/basic/colors/gl.hpp>
#include <libgraphics/fx/operations/helpers/gl_helpers.hpp>
#include <libgraphics/backend/gl/gl_effect.hpp>
#include <libgraphics/backend/gl/gl_imageobject.hpp>
#include <libgraphics/backend/gl/gl_imageoperation.hpp>
#include <libgraphics/backend/gl/gl_rendertarget.hpp>
#include <libgraphics/backend/gl/gl_backenddevice.hpp>


#include <QDebug>

namespace libgraphics {
namespace fx {
namespace operations {

static const char ConvertToMonochrome_Kernel[] =
    "#version 120 \n"
    "uniform sampler2D MainTexture; \n"
    "uniform vec3 Factors; \n"
    "uniform int Width; \n"
    "uniform int Height; \n"

    "vec4 texelFetch( sampler2D tex, float x, float y ) {"
    "        return texture2D( tex, vec2( float(x) / float(Width), float(y) / float(Height) ));"
    "}"
    "void main() {"
    "    vec4 color  = texelFetch( MainTexture, gl_FragCoord.x, gl_FragCoord.y );"
    "    float mono  = (color.r * Factors.r) + (color.g * Factors.g) + (color.b * Factors.b);"
    "    gl_FragColor = vec4(mono,mono,mono,color.a);"
    "}";

struct ConvertToMonochrome : public GLImgOp {
    virtual ~ConvertToMonochrome() {}

    struct Params {
        backend::gl::EffectParameterTexture     mainTexture;
        backend::gl::EffectParameterInt         width;
        backend::gl::EffectParameterInt         height;
        backend::gl::EffectParameterVec3        factors;
    };
    Params                                  params;
    libgraphics::backend::gl::Vec3Data      factors;



    virtual std::string                 getKernelSource() {
        return std::string( ConvertToMonochrome_Kernel );
    }
    virtual bool                        initializeParameters( backend::gl::Effect* effect ) {
        params.mainTexture.reset( "MainTexture", nullptr );
        {
            params.mainTexture.assignId( effect->getParameterId( "MainTexture" ) );
        }
        params.width.reset( "Width", 1024 );
        {
            params.width.assignId( effect->getParameterId( "Width" ) );
        }
        params.height.reset( "Height", 1024 );
        {
            params.height.assignId( effect->getParameterId( "Height" ) );
        }
        params.factors.reset( "Factors", backend::gl::createVec3Data( 1.0f, 1.0f, 1.0f ) );
        {
            params.factors.assignId( effect->getParameterId( "Factors" ) );
        }
        return true;
    }

    virtual void submitParameters( libgraphics::fxapi::ApiBackendDevice* device,
                                   libgraphics::fxapi::ApiImageObject* destination,
                                   libgraphics::fxapi::ApiImageObject* source,
                                   libgraphics::Rect32I   area,
                                   backend::gl::Texture* texture, size_t tx, size_t ty ) {
        ( void )device;
        ( void )destination;
        ( void )source;

        /// apply parameters
        {
            params.width.reset( area.width );
            params.height.reset( area.height );
            params.mainTexture.reset( texture );
            params.factors.reset( factors );

            backend::gl::applyEffectParameterToEffect( getEffect(), params.width );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.height );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.mainTexture );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.factors );
        }
    }
    virtual void submitImageMergeParameters( libgraphics::fxapi::ApiBackendDevice* device, libgraphics::fxapi::ApiImageObject* destination, libgraphics::fxapi::ApiImageObject* source, libgraphics::Rect32I area,
            backend::gl::Texture* texture, backend::gl::Texture* otherTexture, size_t tx, size_t ty ) {
        assert( false );

        ( void )device;
        ( void )destination;
        ( void )source;
        ( void )area;
        ( void )texture;
        ( void )otherTexture;
    }
};



/// operation: convertToMonochrome
void convertToMonochrome_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* destination,
    libgraphics::fxapi::ApiImageObject* source,
    libgraphics::Rect32I   area,
    float* channelFactors
) {
    const auto format           = destination->format();
    const auto channelCount     = libgraphics::fxapi::EPixelFormat::getChannelCount( format );

    float realChannelFactors[3];
    memset( ( void* )realChannelFactors, 0, sizeof( float ) * 3 );

    if( channelCount < 3 ) {
        switch( channelCount ) {
            case 0: assert( false ); break;

            case 1: {
                realChannelFactors[0] = channelFactors[0];
                realChannelFactors[1] = 1.0f;
                realChannelFactors[2] = 1.0f;
                break;
            }

            case 2: {
                realChannelFactors[0] = channelFactors[0];
                realChannelFactors[1] = channelFactors[1];
                realChannelFactors[2] = 1.0f;
            }
        }
    } else {
        memcpy( ( void* )realChannelFactors, ( const void* )channelFactors, sizeof( float ) * 3 );
    }

    LIBGRAPHICS_FILTER_INSTANCE( ConvertToMonochrome, device, filter );

    filter->factors.data[0] = realChannelFactors[0];
    filter->factors.data[1] = realChannelFactors[1];
    filter->factors.data[2] = realChannelFactors[2];

    const auto rendered = renderTiledImageOperation(
                              device,
                              destination,
                              source,
                              area,
                              filter
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "convertToMonochrome_GL: Failed to render operation.";
    }
}

void convertToMonochrome_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    Rect32I area
) {
    assert( false );
}

}
}
}
