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

static const char SplittoneKernel[] =
    "#version 120 \n"
    "uniform  int Width;"
    "uniform  int Height;"
    "uniform  float Weight;"
    "uniform  vec3 BrightWeights;"
    "uniform  vec3 DarkWeights;"
    "uniform sampler2D MainTexture;"
    "vec4 sample( sampler2D tex, vec2 position ) {"
    "    return texture2D( tex, vec2( position.x / Width, position.y / Height));"
    "}"
    "vec4 overlay( vec4 I, vec4 M ) {"
    "    I.rgb *= 255.0;"
    "    M.rgb *= 255.0;"
    "    vec4 E = (I * 0.0039) * (I + (M * 2.0 * 0.0039) * (255.0 - I));"
    "    E.rgb *= 0.0039;"
    "    return E;"
    "}"
    "void main() {"
    "    vec4 finalColor			= vec4(0.0,0.0,0.0,0.0);"
    "    vec4 currentColor		= sample( MainTexture, vec2(gl_FragCoord.x,gl_FragCoord.y));"
    "    float intensity           = currentColor.x;"
    "    float intensityHighlights = intensity * Weight;"
    "    float intensityRest       = 1.0 - intensityHighlights;"
    "    float intensityShadows    = intensityRest*intensityRest;"
    "    float intensityOriginal   = 1.0 - intensityHighlights - intensityShadows;"
    "    finalColor  = overlay( currentColor, vec4(BrightWeights,1.0)) * intensityHighlights;"
    "    finalColor  += overlay( currentColor, vec4(DarkWeights,1.0)) * intensityShadows;"
    "    finalColor  += vec4( intensity, intensity, intensity, 1.0 ) * intensityOriginal;"
    "    gl_FragColor = finalColor;"
    "}";

struct Splittone : public GLImgOp {
    virtual ~Splittone() {}

    struct Params {
        backend::gl::EffectParameterTexture     mainTexture;
        backend::gl::EffectParameterInt         width;
        backend::gl::EffectParameterInt         height;

        backend::gl::EffectParameterFloat       weight;

        backend::gl::EffectParameterVec3        brightWeights;
        backend::gl::EffectParameterVec3        darkWeights;
    };
    Params params;

    float                   weight;
    backend::gl::Vec3Data   brightWeights;
    backend::gl::Vec3Data   darkWeights;


    virtual std::string                 getKernelSource() {
        return std::string( SplittoneKernel );
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
        params.weight.reset( "Weight", 1.0f );
        {
            params.weight.assignId( effect->getParameterId( "Weight" ) );
        }
        params.brightWeights.reset( "BrightWeights", backend::gl::createVec3Data( 1.0f, 1.0f, 1.0f ) );
        {
            params.brightWeights.assignId( effect->getParameterId( "BrightWeights" ) );
        }
        params.darkWeights.reset( "DarkWeights", backend::gl::createVec3Data( 1.0f, 1.0f, 1.0f ) );
        {
            params.darkWeights.assignId( effect->getParameterId( "DarkWeights" ) );
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
            params.brightWeights.reset( brightWeights );
            params.darkWeights.reset( darkWeights );
            params.weight.reset( weight );

            backend::gl::applyEffectParameterToEffect( getEffect(), params.width );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.height );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.mainTexture );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.weight );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.brightWeights );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.darkWeights );
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



/// operation: splittone_GL
void splittone_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* destination,
    libgraphics::fxapi::ApiImageObject* source,
    libgraphics::Rect32I   area,
    float brightR,
    float brightG,
    float brightB,
    float darkR,
    float darkG,
    float darkB,
    float weight
) {
    LIBGRAPHICS_FILTER_INSTANCE( Splittone, device, filter );

    filter->brightWeights.data[0]   = brightR;
    filter->brightWeights.data[1]   = brightG;
    filter->brightWeights.data[2]   = brightB;

    filter->darkWeights.data[0]     = darkR;
    filter->darkWeights.data[1]     = darkG;
    filter->darkWeights.data[2]     = darkB;

    filter->weight                  = weight;

    const auto rendered = renderTiledImageOperation(
                              device,
                              destination,
                              source,
                              area,
                              filter.get()
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "splittone_GL: Failed to render operation.";
    }
}



}
}
}
