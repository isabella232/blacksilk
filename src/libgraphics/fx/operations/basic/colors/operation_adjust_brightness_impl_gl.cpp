#include <libgraphics/fx/operations/basic/colors/gl.hpp>
#include <libgraphics/fx/operations/basic/colors/cpu.hpp>
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
static const char AdjustBrightnessCurve_Kernel[] = "#version 120 \n"
        "uniform int     Width;"
        "uniform int     Height;"
        "uniform sampler2D MainTexture;"
        "uniform sampler1D Curve;"
        "vec4 sample( sampler2D tex, vec2 position ) {"
        "    return texture2D( tex, vec2( position.x / Width, position.y / Height));"
        "}"
        "vec4 noiseSample( sampler2D tex, vec2 position ) {"
        "    return texture2D( tex, vec2( position.x * 0.0019, position.y * 0.0019));"
        "}"
        "vec4 curveSample( float value, float orig ) {"
        "    if ( (value - 0.007 <= orig) && (value + 0.007 >= orig) ) {"
        "        return vec4( orig, orig, orig, orig );"
        "    } else {"
        "        return vec4( value, value, value, value );"
        "    }"
        "}"
        "void main() {"
        "    vec2 npos       = vec2( gl_FragCoord.x, gl_FragCoord.y );"
        "    vec4  realColor = sample( MainTexture, npos );"
        "    float val       = realColor.r;"
        "    float  weight   = texture1D( Curve,  min(0.999,max(0.0,val)) ).r;"
        "    gl_FragColor    = curveSample(weight,val);"
        "}";

struct AdjustBrightnessCurve : public GLImgOp {
    AdjustBrightnessCurve() : curve( nullptr ) {}
    virtual ~AdjustBrightnessCurve() {}

    struct Params {
        backend::gl::EffectParameterTexture     mainTexture;
        backend::gl::EffectParameterInt         width;
        backend::gl::EffectParameterInt         height;
        backend::gl::EffectParameterPixelArray  curve;
    };
    Params                                  params;
    backend::gl::PixelArray*                curve;

    virtual std::string                 getKernelSource() {
        return std::string( AdjustBrightnessCurve_Kernel );
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
        params.curve.reset( "Curve", nullptr );
        {
            params.curve.assignId( effect->getParameterId( "Curve" ) );
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
            params.curve.reset( this->curve );

            backend::gl::applyEffectParameterToEffect( getEffect(), params.width );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.height );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.mainTexture );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.curve );
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

void adjustBrightness_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject*     destination,
    libgraphics::fxapi::ApiImageObject*     source,
    libgraphics::Rect32I   area,
    float* curveData,
    size_t curveLength
) {
    LIBGRAPHICS_FILTER_INSTANCE( AdjustBrightnessCurve, device, filter );

    float* data( curveData );

    static const size_t averageAreaSize = 4;
    static const float  weight          = 1.0f / ( float )averageAreaSize;
    static const size_t maxCurveLength  = 8192; /// maximal curve length
    const float scalingFactor           = curveLength / maxCurveLength;

    if( scalingFactor > 1.0f ) {
        const size_t scaledLength       = curveLength * ( 1.0f / scalingFactor );
        data                            = new float[scaledLength];

        for( int i = 0; scaledLength > i; ++i ) {
            float sum( 0.0f );

            for( int p = 0; averageAreaSize > p; ++p ) {
                int srcPos  = std::floor( ( i + p ) * scalingFactor );
                srcPos      = ( srcPos < 0 ) ? 0 : srcPos;
                srcPos      = ( curveLength <= srcPos ) ? ( curveLength - 1 ) : srcPos;

                sum += curveData[srcPos] * weight;
            }

            data[i] = sum;
        }

        curveLength = scaledLength;
    } else if( curveLength == 0 ) {
        qDebug() << "Failed to execute operation: Curve is of invalid length(== 0).";
        assert( false );
        return;
    }

    backend::gl::PixelArray curveTexture( libgraphics::fxapi::EPixelFormat::Mono32F, curveLength, ( const void* )data );
    filter->curve = &curveTexture;

    const auto rendered = renderTiledImageOperation(
                              device,
                              destination,
                              source,
                              area,
                              filter.get()
                          );
    filter->curve = nullptr;

    if( scalingFactor > 1.0f ) {
        delete [] data;
    }

    assert( rendered );

    if( !rendered ) {
        qDebug() << "convertToMonochrome_GL: Failed to render operation.";
    }
}

static const char simple_kernel[] =
    "#version 120 \n"
    "uniform sampler2D mainTexture;"
    "uniform int width;"
    "uniform int height;"
    "uniform float param_factor;"
    "void main() {"
    "   gl_FragColor = texture2D( mainTexture, vec2( gl_FragCoord.x / width, gl_FragCoord.y / height ) ) * param_factor;"
    "}";

struct LinearAdjustBrightness : public libgraphics::backend::gl::ImageOperation {
    virtual ~LinearAdjustBrightness() {}

    struct Params {
        backend::gl::EffectParameterTexture     mainTexture;
        backend::gl::EffectParameterInt         width;
        backend::gl::EffectParameterInt         height;
        backend::gl::EffectParameterFloat       param_factor;
    };

    std::unique_ptr<backend::gl::Effect>    effect;
    Params                                       params;
    bool                                         initialized;
    float                                        adjustment;

    LinearAdjustBrightness() : initialized( false ), adjustment( 0.0f ) {}

    void initializeSimpleKernelEffect() {
        if( effect ) {
            return;
        }

        effect.reset(
            new backend::gl::Effect( "simple_kernel", simple_kernel )
        );

        if( effect->empty() ) {
            qDebug() << "Failed to compile filter: simple_kernel(curve)";
        }

        params.mainTexture.reset( "mainTexture", nullptr );
        {
            params.mainTexture.assignId(
                effect->getParameterId( "mainTexture" )
            );
        }

        params.width.reset( "width", 1024 );
        {
            params.width.assignId(
                effect->getParameterId( "width" )
            );
        }

        params.height.reset( "height", 1024 );
        {
            params.height.assignId(
                effect->getParameterId( "height" )
            );
        }

        params.param_factor.reset( "param_factor", 1.0f );
        {
            params.param_factor.assignId(
                effect->getParameterId( "param_factor" )
            );
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
            params.param_factor.reset( adjustment );
            params.mainTexture.reset( texture );

            backend::gl::applyEffectParameterToEffect( getEffect(), params.width );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.height );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.param_factor );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.mainTexture );
        }
    }

    virtual backend::gl::Effect* getEffect() {
        return effect.get();
    }
    virtual bool initialize( libgraphics::fxapi::ApiBackendDevice* dev ) {
        ( void )dev;

        if( !this->initialized ) {
            this->initialized = true;
            this->initializeSimpleKernelEffect();
        }

        return true;
    }

};

void adjustBrightness_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject*     destination,
    libgraphics::fxapi::ApiImageObject*     source,
    libgraphics::Rect32I   area,
    float adjustment
) {
    assert( device );
    assert( destination );
    assert( source );

    static std::unique_ptr<LinearAdjustBrightness> filter;

    if( !filter ) {
        filter.reset( new LinearAdjustBrightness() );
    }

    filter->adjustment = adjustment;

    const bool rendered = renderTiledImageOperation(
                              device,
                              destination,
                              source,
                              area,
                              filter.get()
                          );
    assert( rendered );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !rendered ) {
        qDebug() << "adjustBrightness_GL(): Failed to apply operation to image.";
    }

#endif
}

}
}
}
