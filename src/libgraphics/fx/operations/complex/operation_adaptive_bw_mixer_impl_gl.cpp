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

static const char adaptiveBWMixer_Kernel[] =
    "#version 120 \n"
    "uniform sampler2D MainTexture;"

    "uniform vec3 BrightWeights;"
    "uniform vec3 DarkWeights;"
    "uniform float Weight;"

    "uniform int Width;"
    "uniform int Height;"
    "vec4 texelFetch( sampler2D tex, float x, float y ) {"
    "        return texture2D( tex, vec2( float(x) / float(Width), float(y) / float(Height) ));"
    "}"
    "void main() {"
    "    vec4 color = texelFetch( MainTexture, gl_FragCoord.x, gl_FragCoord.y );"
    "    float luma = Weight + ((color.r * 0.299 ) + (color.g * 0.587 ) + (color.b * 0.114 ));"
    "    float bright = ((color.r * BrightWeights.r ) + (color.g * BrightWeights.g ) + (color.b * BrightWeights.b ));"
    "    float dark = ((color.r * DarkWeights.r ) + (color.g * DarkWeights.g ) + (color.b * DarkWeights.b ));"
    "    float combined = ( dark * ( 1.0 - luma ) ) + ( bright * luma );"
    "    gl_FragColor = vec4(combined,combined,combined,1.0);"
    "}";

struct AdaptiveBWMixer : public GLImgOp {
    virtual ~AdaptiveBWMixer() {}

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
        return std::string( adaptiveBWMixer_Kernel );
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



/// operation: adaptiveBWMixer
void adaptiveBWMixer_GL(
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
    LIBGRAPHICS_FILTER_INSTANCE( AdaptiveBWMixer, device, filter );

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
        qDebug() << "adaptiveBWMixer_GL: Failed to render operation.";
    }
}

}
}
}
