
#include <assert.h>
#include <QDebug>

#include <libgraphics/fx/operations/complex.hpp>
#include <libgraphics/fx/operations/complex/gl.hpp>
#include <libgraphics/fx/operations/helpers/gl_helpers.hpp>

namespace libgraphics {
namespace fx {
namespace operations {

static const char VignetteKernel[] =
    "#version 120 \n"
    "uniform int        Width;"
    "uniform int        Height;"
    "uniform int        MaxWidth;"
    "uniform int        MaxHeight;"
    "uniform int        X;"
    "uniform int        Y;"
    "uniform vec3       VignetteCenter;"
    "uniform float      Strength;"
    "uniform float      Radius;"
    "uniform sampler2D  MainTexture;"
    "vec4 sample( sampler2D tex, vec2 position ) {"
    "    return texture2D( tex, vec2( position.x / Width, position.y / Height));"
    "}"
    "void main() {"
    "    vec2 relativeCenter     = vec2( float(MaxWidth) * VignetteCenter.x * 0.01, float(MaxHeight) * VignetteCenter.y * 0.01 );"
    "    float currentDistance   = abs( distance(relativeCenter, vec2( float(X) + float(gl_FragCoord.x), float(Y) + float(gl_FragCoord.y) ) ) );"
    "    float maxDistance       = Radius * 0.01 * ( float(MaxWidth + MaxHeight) * 0.5 );"
    "    float vignetteValue     = Strength * 0.01 * (currentDistance / maxDistance);"
    "    vec4  pixel             = sample(MainTexture,gl_FragCoord.xy);"
    "    vec4  currentValue      = vec4("
    "        ( 1.0 - vignetteValue ) * pixel.r + ( vignetteValue * ( pixel.r * pixel.r ) ),"
    "        ( 1.0 - vignetteValue ) * pixel.g + ( vignetteValue * ( pixel.g * pixel.g ) ),"
    "        ( 1.0 - vignetteValue ) * pixel.b + ( vignetteValue * ( pixel.b * pixel.b ) ),"
    "        ( 1.0 - vignetteValue ) * pixel.a + ( vignetteValue * ( pixel.a * pixel.a ) )"
    "    );"
    "    gl_FragColor = vec4( max( 0.0, min(currentValue.r,1.0)),max( 0.0, min(currentValue.g,1.0)),max( 0.0, min(currentValue.b,1.0)),max( 0.0, min(currentValue.a,1.0)) );"
    "}";


struct Vignette : public GLImgOp {
    virtual ~Vignette() {}

    struct Params {
        backend::gl::EffectParameterTexture     mainTexture;
        backend::gl::EffectParameterInt         width;
        backend::gl::EffectParameterInt         height;
        backend::gl::EffectParameterInt         maxWidth;
        backend::gl::EffectParameterInt         maxHeight;
        backend::gl::EffectParameterInt         x;
        backend::gl::EffectParameterInt         y;
        backend::gl::EffectParameterFloat       radius;
        backend::gl::EffectParameterFloat       strength;
        backend::gl::EffectParameterVec3        center;
    };
    Params                                  params;
    float                                   radius;
    float                                   strength;
    int                                     maxWidth;
    int                                     maxHeight;
    float                                   centerX;
    float                                   centerY;

    Vignette() : radius( 1.0 ), strength( 1.0 ),
        maxWidth( 1024 ), maxHeight( 1024 ), centerX( 0 ), centerY( 0 ) {}


    virtual std::string                 getKernelSource() {
        return std::string( VignetteKernel );
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
        params.x.reset( "X", 1024 );
        {
            params.x.assignId( effect->getParameterId( "X" ) );
        }
        params.y.reset( "Y", 1024 );
        {
            params.y.assignId( effect->getParameterId( "Y" ) );
        }
        params.maxWidth.reset( "MaxWidth", 1024 );
        {
            params.maxWidth.assignId( effect->getParameterId( "MaxWidth" ) );
        }
        params.maxHeight.reset( "MaxHeight", 1024 );
        {
            params.maxHeight.assignId( effect->getParameterId( "MaxHeight" ) );
        }
        params.radius.reset( "Radius", 1.0f );
        {
            params.radius.assignId( effect->getParameterId( "Radius" ) );
        }
        params.strength.reset( "Strength", 1.0f );
        {
            params.strength.assignId( effect->getParameterId( "Strength" ) );
        }
        params.center.reset( "VignetteCenter", backend::gl::createVec3Data( 1.0, 1.0, 1.0 ) );
        {
            params.center.assignId( effect->getParameterId( "VignetteCenter" ) );
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
            params.x.reset( area.x );
            params.y.reset( area.y );
            params.width.reset( area.width );
            params.height.reset( area.height );
            params.mainTexture.reset( texture );
            params.radius.reset( radius );
            params.strength.reset( strength );
            params.maxWidth.reset( maxWidth );
            params.maxHeight.reset( maxHeight );
            params.center.reset( backend::gl::createVec3Data( this->centerX, this->centerY, 0.0f ) );

            backend::gl::applyEffectParameterToEffect( getEffect(), params.width );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.height );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.mainTexture );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.radius );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.strength );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.maxWidth );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.maxHeight );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.center );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.x );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.y );
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


/// operation: applyVignette
void applyVignette_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src,
    Rect32I area,
    const Point32F& center,
    float radius,
    float strength
) {
    LIBGRAPHICS_FILTER_INSTANCE( Vignette, device, filter );

    filter->radius          = radius;
    filter->strength        = strength;
    filter->centerX         = center.x;
    filter->centerY         = center.y;
    filter->maxWidth        = ( int )dst->width();
    filter->maxHeight       = ( int )dst->height();

    const auto rendered = renderTiledImageOperation(
                              device,
                              dst,
                              src,
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
