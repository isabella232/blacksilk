#include <libgraphics/fx/operations/helpers/cpu_helpers.hpp>
#include <libgraphics/fx/operations/complex.hpp>
#include <libgraphics/fx/operations/complex/cpu.hpp>
#include <libgraphics/fx/operations/complex/gl.hpp>
#include <libgraphics/backend/gl/gl_effect.hpp>
#include <libgraphics/backend/gl/gl_imageobject.hpp>
#include <libgraphics/backend/gl/gl_imageoperation.hpp>
#include <libgraphics/backend/gl/gl_rendertarget.hpp>
#include <libgraphics/backend/gl/gl_backenddevice.hpp>
#include <libgraphics/fx/operations/helpers/gl_helpers.hpp>


#include <QDebug>

namespace libgraphics {
namespace fx {
namespace operations {

static const char CascadedSharpenWith4Kernel[] =
    "#version 120 \n"
    "uniform sampler2D MainTexture;"
    "uniform sampler2D Blurred0;"
    "uniform sampler2D Blurred1;"
    "uniform sampler2D Blurred2;"
    "uniform sampler2D Blurred3;"
    "uniform float     Intensity0;"
    "uniform float     Intensity1;"
    "uniform float     Intensity2;"
    "uniform float     Intensity3;"
    "uniform float     Threshold;"
    "uniform int       Width;"
    "uniform int       Height;"
    "vec4 sample( sampler2D tex, vec2 position ) {"
    "        return texture2D( tex, vec2( position.x / Width, position.y / Height));"
    "}"
    "vec4 grainExtract( vec4 first, vec4 second ) {"
    "    return vec4("
    "        first.r - second.r + 0.5,"
    "        first.g - second.g + 0.5,"
    "        first.b - second.b + 0.5,"
    "        first.a - second.a + 0.5"
    "    );"
    "}"
    "vec4 negate(vec4 a) {"
    "    return vec4("
    "        1.0 - a.r,"
    "        1.0 - a.g,"
    "        1.0 - a.b,"
    "        1.0 - a.a"
    "    );"
    "}"
    "void main() {"
    "    vec4  basePixel      = sample( MainTexture, gl_FragCoord.xy );"
    "    vec4 blurred0    = sample( Blurred0, gl_FragCoord.xy);"
    "    vec4 blurred1    = sample( Blurred1, gl_FragCoord.xy);"
    "    vec4 blurred2    = sample( Blurred2, gl_FragCoord.xy);"
    "    vec4 blurred3    = sample( Blurred3, gl_FragCoord.xy);"
    "    vec4  raw0       = (blurred0 - basePixel);"
    "    vec4  raw1       = (blurred1 - basePixel);"
    "    vec4  raw2       = (blurred2 - basePixel);"
    "    vec4  raw3       = (blurred3 - basePixel);"
    "    vec4 weighted0   = (raw0       ) * (Intensity0 / 100.0);"
    "    vec4 weighted1   = (raw1 - raw0) * (Intensity1 / 100.0);"
    "    vec4 weighted2   = (raw2 - raw1) * (Intensity2 / 100.0);"
    "    vec4 weighted3   = (raw3 - raw2) * (Intensity3 / 100.0);"
    "    vec4 w1  = grainExtract(blurred0,basePixel);"
    "    vec4 w2  = grainExtract(blurred1,basePixel);"
    "    vec4 w3  = grainExtract(blurred2,basePixel);"
    "    vec4 w4  = grainExtract(blurred3,basePixel);"
    "    float quotient = 1.0 - max( Threshold / 100.0, 0.01 );"
    "    vec4 max_overall = vec4("
    "        max( 0.0, min( 1.0, max(w1.r, max(w2.r, max(w3.r, w4.r ) ) ) * quotient ) ),"
    "        max( 0.0, min( 1.0, max(w1.g, max(w2.g, max(w3.g, w4.g ) ) ) * quotient ) ),"
    "        max( 0.0, min( 1.0, max(w1.b, max(w2.b, max(w3.b, w4.b ) ) ) * quotient ) ),"
    "        max( 0.0, min( 1.0, max(w1.a, max(w2.a, max(w3.a, w4.a ) ) ) * quotient ) )"
    "    );"
    "    vec4 temp = weighted0 + weighted1 + weighted2 + weighted3;"
    "    vec4 ret  = ( ( max_overall * (basePixel - temp) + negate(max_overall) * basePixel) );"
    "    gl_FragColor = ret;"
    "}";


struct CascadedSharpenWith4 : public GLImgOp {
    virtual ~CascadedSharpenWith4() {}

    struct Params {
        backend::gl::EffectParameterTexture     mainTexture;

        backend::gl::EffectParameterTexture     blurred0;
        backend::gl::EffectParameterTexture     blurred1;
        backend::gl::EffectParameterTexture     blurred2;
        backend::gl::EffectParameterTexture     blurred3;

        backend::gl::EffectParameterInt         width;
        backend::gl::EffectParameterInt         height;

        backend::gl::EffectParameterFloat       threshold;

        backend::gl::EffectParameterFloat       intensity0;
        backend::gl::EffectParameterFloat       intensity1;
        backend::gl::EffectParameterFloat       intensity2;
        backend::gl::EffectParameterFloat       intensity3;
    };
    Params params;

    float                   threshold;
    float                   intensity0;
    float                   intensity1;
    float                   intensity2;
    float                   intensity3;

    backend::gl::ImageObject*   blurred0;
    backend::gl::ImageObject*   blurred1;
    backend::gl::ImageObject*   blurred2;
    backend::gl::ImageObject*   blurred3;


    virtual std::string                 getKernelSource() {
        return std::string( CascadedSharpenWith4Kernel );
    }
    virtual bool                        initializeParameters( backend::gl::Effect* effect ) {
        params.mainTexture.reset( "MainTexture", nullptr );
        {
            params.mainTexture.assignId( effect->getParameterId( "MainTexture" ) );
        }

        params.blurred0.reset( "Blurred0", nullptr );
        {
            params.blurred0.assignId( effect->getParameterId( "Blurred0" ) );
        }
        params.blurred1.reset( "Blurred1", nullptr );
        {
            params.blurred1.assignId( effect->getParameterId( "Blurred1" ) );
        }
        params.blurred2.reset( "Blurred2", nullptr );
        {
            params.blurred2.assignId( effect->getParameterId( "Blurred2" ) );
        }
        params.blurred3.reset( "Blurred3", nullptr );
        {
            params.blurred3.assignId( effect->getParameterId( "Blurred3" ) );
        }


        params.width.reset( "Width", 1024 );
        {
            params.width.assignId( effect->getParameterId( "Width" ) );
        }
        params.height.reset( "Height", 1024 );
        {
            params.height.assignId( effect->getParameterId( "Height" ) );
        }

        params.threshold.reset( "Threshold", 1.0f );
        {
            params.threshold.assignId( effect->getParameterId( "Threshold" ) );
        }

        params.intensity0.reset( "Intensity0", 1.0f );
        {
            params.intensity0.assignId( effect->getParameterId( "Intensity0" ) );
        }
        params.intensity1.reset( "Intensity1", 1.0f );
        {
            params.intensity1.assignId( effect->getParameterId( "Intensity1" ) );
        }
        params.intensity2.reset( "Intensity2", 1.0f );
        {
            params.intensity2.assignId( effect->getParameterId( "Intensity2" ) );
        }
        params.intensity3.reset( "Intensity3", 1.0f );
        {
            params.intensity3.assignId( effect->getParameterId( "Intensity3" ) );
        }

        return true;
    }

    virtual void submitParameters( libgraphics::fxapi::ApiBackendDevice* device,
                                   libgraphics::fxapi::ApiImageObject* destination,
                                   libgraphics::fxapi::ApiImageObject* source,
                                   libgraphics::Rect32I   area,
                                   backend::gl::Texture* texture,
                                   size_t tx,
                                   size_t ty ) {
        ( void )device;
        ( void )destination;
        ( void )source;

        /// apply parameters
        {
            params.width.reset( area.width );
            params.height.reset( area.height );
            params.mainTexture.reset( texture );

            params.threshold.reset( threshold );

            params.intensity0.reset( intensity0 );
            params.intensity1.reset( intensity1 );
            params.intensity2.reset( intensity2 );
            params.intensity3.reset( intensity3 );

            params.blurred0.reset( blurred0->tileTexture( tx, ty ) );
            params.blurred1.reset( blurred1->tileTexture( tx, ty ) );
            params.blurred2.reset( blurred2->tileTexture( tx, ty ) );
            params.blurred3.reset( blurred3->tileTexture( tx, ty ) );


            backend::gl::applyEffectParameterToEffect( getEffect(), params.width );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.height );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.threshold );

            backend::gl::applyEffectParameterToEffect( getEffect(), params.blurred0 );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.blurred1 );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.blurred2 );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.blurred3 );

            backend::gl::applyEffectParameterToEffect( getEffect(), params.mainTexture );

            backend::gl::applyEffectParameterToEffect( getEffect(), params.intensity0 );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.intensity1 );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.intensity2 );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.intensity3 );
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
        ( void )tx;
        ( void )ty;
    }
};

void cascadedSharpenWith4_GL(
    libgraphics::fxapi::ApiBackendDevice*   device,
    libgraphics::fxapi::ApiImageObject*     destination,
    libgraphics::fxapi::ApiImageObject*     source,
    Rect32I                                 area,
    const std::tuple< libgraphics::ImageLayer*, float, float >& cascade0,
    const std::tuple< libgraphics::ImageLayer*, float, float >& cascade1,
    const std::tuple< libgraphics::ImageLayer*, float, float >& cascade2,
    const std::tuple< libgraphics::ImageLayer*, float, float >& cascade3,
    float threshold
) {
    LIBGRAPHICS_FILTER_INSTANCE( CascadedSharpenWith4, device, filter );
    float   p( 0.0f );

    float   intensity0( 0.0f );
    float   intensity1( 0.0f );
    float   intensity2( 0.0f );
    float   intensity3( 0.0f );

    libgraphics::ImageLayer*    blurred0( nullptr );
    libgraphics::ImageLayer*    blurred1( nullptr );
    libgraphics::ImageLayer*    blurred2( nullptr );
    libgraphics::ImageLayer*    blurred3( nullptr );

    std::tie( blurred0, p, intensity0 ) = cascade0;
    std::tie( blurred1, p, intensity1 ) = cascade1;
    std::tie( blurred2, p, intensity2 ) = cascade2;
    std::tie( blurred3, p, intensity3 ) = cascade3;

    filter->threshold       = threshold;

    filter->blurred0        = ( backend::gl::ImageObject* )blurred0->internalImageForBackend( FXAPI_BACKEND_OPENGL );
    filter->blurred1        = ( backend::gl::ImageObject* )blurred1->internalImageForBackend( FXAPI_BACKEND_OPENGL );
    filter->blurred2        = ( backend::gl::ImageObject* )blurred2->internalImageForBackend( FXAPI_BACKEND_OPENGL );
    filter->blurred3        = ( backend::gl::ImageObject* )blurred3->internalImageForBackend( FXAPI_BACKEND_OPENGL );

    filter->intensity0      = intensity0;
    filter->intensity1      = intensity1;
    filter->intensity2      = intensity2;
    filter->intensity3      = intensity3;

    const auto rendered = renderTiledImageOperation(
                              device,
                              destination,
                              source,
                              area,
                              filter
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "add_GL: Failed to render operation.";
    }

}

}
}
}
