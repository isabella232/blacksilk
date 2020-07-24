
#include <libgraphics/fx/operations/complex.hpp>
#include <libgraphics/fx/operations/complex/cpu.hpp>
#include <libgraphics/fx/operations/complex/gl.hpp>
#include <libgraphics/fx/operations/helpers/gl_helpers.hpp>

namespace libgraphics {
namespace fx {
namespace operations {

void gaussianBlur_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* destination,
    libgraphics::fxapi::ApiImageObject* source,
    Rect32I area,
    float radius
) {
    assert( device );
    assert( source );

    libgraphics::fxapi::ApiScopedImgRef    temporaryLayer(
        device,
        destination->format(),
        destination->width(),
        destination->height()
    );

    horizontalGaussianBlur_GL(
        device,
        temporaryLayer,
        source,
        area,
        radius
    );
    verticalGaussianBlur_GL(
        device,
        destination,
        temporaryLayer,
        area,
        radius
    );
}


static const char HorizontalGaussianBlurKernel[] =
    "#version 120 \n"
    "uniform int Width;"
    "uniform int Height;"
    "uniform float  Radius;"
    "uniform sampler2D MainTexture;"
    "vec4 sample( sampler2D tex, vec2 position ) {"
    "    return texture2D( tex, vec2( abs(position.x / Width), abs(position.y / Height)));"
    "}"
    "void main(void)"
    "{"
    "    if ( Radius > 0.0 ) {"
    "        vec4 color = vec4( 0.0, 0.0, 0.0, 0.0 );"
    "        float i         = 0.0;"
    "        float size      = 4 * ceil(Radius) + 1;"
    "        float halfSize  = float(int(size * 0.5));"
    "        float div       = Radius * 1.141;"
    "        float sum       = 0.0;"
    "        while ( i < size ) {"
    "            float xM = i - halfSize;"
    "            sum    += exp( (-(xM * xM) / div ) );"
    "            i      += 1.0;"
    "        }"
    "        i = 0.0;"
    "        while( i < size ) {"
    "            float xM = i - halfSize;"
    "            color   += exp( (-(xM * xM) / div ) ) * sample(MainTexture, vec2(gl_FragCoord.x, gl_FragCoord.y + xM)) / sum;"
    "            i += 1.0;"
    "        }"
    "        gl_FragColor = color;"
    "    } else {"
    "        gl_FragColor = sample(MainTexture,gl_FragCoord.xy);"
    "    }"
    "}";

static const char VerticalGaussianBlurKernel[] =
    "#version 120 \n"
    "uniform int Width;"
    "uniform int Height;"
    "uniform float  Radius;"
    "uniform sampler2D MainTexture;"
    "vec4 sample( sampler2D tex, vec2 position ) {"
    "    return texture2D( tex, vec2( abs(position.x / Width), abs(position.y / Height)));"
    "}"
    "void main(void)"
    "{"
    "    if ( Radius > 0.0 ) {"
    "        vec4 color = vec4( 0.0, 0.0, 0.0, 0.0 );"
    "        float i         = 0.0;"
    "        float size      = 4 * ceil(Radius) + 1;"
    "        float halfSize  = float(int(size / 2));"
    "        float div       = Radius * 1.141;"
    "        float sum       = 0.0;"
    "        while ( i < size ) {"
    "            float xM = i - halfSize;"
    "            sum    += exp( (-(xM * xM) / div ) );"
    "            i      += 1.0;"
    "        }"
    "        i = 0.0;"
    "        while( i < size ) {"
    "            float xM = i - halfSize;"
    "            color   += exp( (-(xM * xM) / div ) ) * sample(MainTexture, vec2(gl_FragCoord.x + xM, gl_FragCoord.y)) / sum;"
    "            i += 1.0;"
    "        }"
    "        gl_FragColor = color;"
    "    } else {"
    "        gl_FragColor = sample(MainTexture,gl_FragCoord.xy);"
    "    }"
    "}";

struct GaussianBlur : public GLImgOp {
    virtual ~GaussianBlur() {}

    struct Params {
        backend::gl::EffectParameterTexture     mainTexture;
        backend::gl::EffectParameterInt         width;
        backend::gl::EffectParameterInt         height;
        backend::gl::EffectParameterFloat       radius;

    };
    Params                                  params;
    float                                   radius;
    std::string                             source;

    GaussianBlur( const std::string& _source ) : source( _source ) {}

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
        params.radius.reset( "Radius", 1.0f );
        {
            params.radius.assignId( effect->getParameterId( "Radius" ) );
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
            params.radius.reset( radius );

            backend::gl::applyEffectParameterToEffect( getEffect(), params.width );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.height );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.mainTexture );
            backend::gl::applyEffectParameterToEffect( getEffect(), params.radius );
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


void horizontalGaussianBlur_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* destination,
    libgraphics::fxapi::ApiImageObject* source,
    Rect32I area,
    float radius
) {
    LIBGRAPHICS_FILTER_INSTANCE_EX( GaussianBlur, device, filter, HorizontalGaussianBlurKernel );

    filter->radius       = radius;

    const auto rendered = renderTiledImageOperation(
                              device,
                              destination,
                              source,
                              area,
                              filter.get()
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "add_GL: Failed to render operation.";
    }
}



void verticalGaussianBlur_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* destination,
    libgraphics::fxapi::ApiImageObject* source,
    Rect32I area,
    float radius
) {
    LIBGRAPHICS_FILTER_INSTANCE_EX( GaussianBlur, device, filter, VerticalGaussianBlurKernel );

    filter->radius       = radius;

    const auto rendered = renderTiledImageOperation(
                              device,
                              destination,
                              source,
                              area,
                              filter.get()
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "add_GL: Failed to render operation.";
    }
}

}
}
}
