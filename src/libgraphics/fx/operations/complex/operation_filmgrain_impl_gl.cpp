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

static const char   FilmgrainKernel_MonoGrain[] =
    "#version 120\n"
    "uniform int Width;"
    "uniform int Height;"
    "uniform sampler2D MainTexture;"
    "uniform sampler2D NoiseMap;"
    "uniform sampler1D Weights;"
    "vec4 sample( sampler2D tex, vec2 position ) {"
    "    return texture2D( tex, vec2( position.x / Width, position.y / Height));"
    "}"
    "vec4 noiseSample( sampler2D tex, vec2 position ) {"
    "    return texture2D( tex, vec2( position.x / Width, position.y / Height));"
    "}"
    "vec4 overlay( vec4 I, vec4 M ) {"
    "    I.rgb *= 255.0;"
    "    M.rgb *= 255.0;"
    "    vec4 E = (I * 0.0039) * (I + (2.0 * M * 0.0039) * (255.0 - I));"
    "    E.rgb *= 0.0039;"
    "    return E;"
    "}"
    "vec4 grainMerge( vec4 I, float m ) {"
    "    I.rgb *= 255.0;"
    "    vec4 E = (1-m)*I + m*I/ (255.0 / 2);"
    "    E.rgb /= 255.0;"
    "    return E;"
    "}"
    "void main() {"
    "    vec2 npos       = gl_FragCoord.xy;"
    "    vec4  realColor = vec4(0.0,0.0,0.0,0.0);"
    "    realColor       = sample( MainTexture, npos );"
    "    float  value    = sample( NoiseMap, npos ).r;"
    "    float  weight   = texture1D( Weights,  realColor.x ).r;"
    "    vec4  wh        = overlay( realColor, vec4(value,value,value,value) );"
    "    gl_FragColor    = ( (realColor * (1.0 - weight)) + (wh * weight) );"
    "}";
static const char   FilmgrainKernel_ColoredGrain[] =
    "#version 120\n"
    "uniform int Width;"
    "uniform int Height;"
    "uniform sampler2D MainTexture;"
    "uniform sampler2D NoiseMap;"
    "uniform sampler1D Weights;"
    "vec4 sample( sampler2D tex, vec2 position ) {"
    "    return texture2D( tex, vec2( position.x / Width, position.y / Height));"
    "}"
    "vec4 noiseSample( sampler2D tex, vec2 position ) {"
    "    return texture2D( tex, vec2( position.x / Width, position.y / Height));"
    "}"
    "vec4 overlay( vec4 I, vec4 M ) {"
    "    I.rgb *= 255.0;"
    "    M.rgb *= 255.0;"
    "    vec4 E = (I * 0.0039) * (I + (2.0 * M * 0.0039) * (255.0 - I));"
    "    E.rgb *= 0.0039;"
    "    return E;"
    "}"
    "vec4 grainMerge( vec4 I, float m ) {"
    "    I.rgb *= 255.0;"
    "    vec4 E = (1-m)*I + m*I/ (255.0 / 2);"
    "    E.rgb /= 255.0;"
    "    return E;"
    "}"
    "void main() {"
    "    vec2 npos       = gl_FragCoord.xy;"
    "    vec4  realColor = vec4(0.0,0.0,0.0,0.0);"
    "    realColor       = sample( MainTexture, npos );"
    "    vec4  value     = sample( NoiseMap, npos );"
    "    float  weight   = texture1D( Weights,  realColor.x ).r;"
    "    vec4  wh        = overlay( realColor, vec4(value.r,value.g,value.b,value.a) );"
    "    gl_FragColor    = (realColor * (1.0 - weight)) + (wh * weight);"
    "}";

struct Filmgrain : public GLImgOp {
    Filmgrain( bool isMonoGrain ) : weights( nullptr ), noiseMap( nullptr ), source( isMonoGrain ? FilmgrainKernel_MonoGrain : FilmgrainKernel_ColoredGrain ) {}
    virtual ~Filmgrain() {}

    struct Params {
        backend::gl::EffectParameterTexture     mainTexture;
        backend::gl::EffectParameterTexture     noiseMap;
        backend::gl::EffectParameterInt         width;
        backend::gl::EffectParameterInt         height;
        backend::gl::EffectParameterPixelArray  weights;
    };
    Params                                  params;
    backend::gl::PixelArray*                weights;
    backend::gl::Texture*                   noiseMap;
    std::string                             source;

    virtual std::string                 getKernelSource() {
        return source;
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
        params.weights.reset( "Weights", nullptr );
        {
            params.weights.assignId( effect->getParameterId( "Weights" ) );
        }
        params.noiseMap.reset( "NoiseMap", nullptr );
        {
            params.noiseMap.assignId( effect->getParameterId( "NoiseMap" ) );
        }
        return true;
    }

    virtual void submitParameters( libgraphics::fxapi::ApiBackendDevice* device,
                                   libgraphics::fxapi::ApiImageObject* destination,
                                   libgraphics::fxapi::ApiImageObject* source,
                                   libgraphics::Rect32I   area,
                                   backend::gl::Texture* texture, size_t /*tx*/, size_t /*ty*/ ) {
        ( void )device;
        ( void )destination;
        ( void )source;

        /// apply parameters
        {
            params.mainTexture.reset( texture );
            params.width.reset( area.width );
            params.height.reset( area.height );
            params.weights.reset( this->weights );
            params.noiseMap.reset( this->noiseMap );

            backend::gl::applyEffectParameterToEffect( getEffect(), params.width );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.height );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.mainTexture );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.weights );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.noiseMap );
        }
    }
    virtual void submitImageMergeParameters( libgraphics::fxapi::ApiBackendDevice* device, libgraphics::fxapi::ApiImageObject* destination, libgraphics::fxapi::ApiImageObject* source, libgraphics::Rect32I area,
            backend::gl::Texture* texture, backend::gl::Texture* otherTexture, size_t /*tx*/, size_t /*ty*/ ) {
        assert( false );

        ( void )device;
        ( void )destination;
        ( void )source;
        ( void )area;
        ( void )texture;
        ( void )otherTexture;
    }
};
void filmgrain_GL(
    libgraphics::fxapi::ApiBackendDevice*   device,
    libgraphics::fxapi::ApiImageObject*     destination,
    libgraphics::fxapi::ApiImageObject*     source,
    Rect32I                                 area,
    libgraphics::fxapi::ApiImageObject*     grainLayer,
    const std::vector<float>&               curveData,
    bool                                    isMonoGrain
) {
    LIBGRAPHICS_FILTER_INSTANCE_EX( Filmgrain, device, filterMonoGrain, true );
    LIBGRAPHICS_FILTER_INSTANCE_EX( Filmgrain, device, filterColoredGrain, false );

    Filmgrain* filter = ( isMonoGrain ) ? filterMonoGrain.get() : filterColoredGrain.get();

    size_t length( curveData.size() );
    float* data( ( float* )curveData.data() );

    static const size_t averageAreaSize = 4;
    static const float  weight          = 1.0f / ( float )averageAreaSize;
    static const size_t maxCurveLength  = 8192; /// maximal curve length
    const float scalingFactor           = ( float )length / ( float )maxCurveLength;

    if( scalingFactor > 1.0f ) {
        const size_t scaledLength       = length * ( 1.0f / scalingFactor );
        data                            = new float[scaledLength];

        for( size_t i = 0; scaledLength > i; ++i ) {
            float sum( 0.0f );

            for( size_t p = 0; averageAreaSize > p; ++p ) {
                int srcPos  = std::floor( ( i + p ) * scalingFactor );
                srcPos      = std::max( 0, srcPos );
                srcPos      = ( length <= srcPos ) ? ( length - 1 ) : srcPos;

                sum += curveData[srcPos] * weight;
            }

            data[i] = sum;
        }

        length = scaledLength;
    }

    backend::gl::PixelArray     curve( libgraphics::fxapi::EPixelFormat::Mono32F, length, data );
    filter->noiseMap            = ( ( backend::gl::ImageObject* )grainLayer )->tileTexture( 0, 0 );
    filter->weights             = &curve;

    const auto rendered = renderTiledImageOperation(
                              device,
                              destination,
                              source,
                              area,
                              filter
                          );
    assert( rendered );

    filter->noiseMap = nullptr;
    filter->weights  = nullptr;

    if( scalingFactor > 1.0f ) {
        delete [] data;
    }

    if( !rendered ) {
        qDebug() << "adaptiveBWMixer_GL: Failed to render operation.";
    }
}

}
}
}
