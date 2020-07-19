#include <libgraphics/fx/operations/helpers/gl_helpers.hpp>
#include <QElapsedTimer>
#include <QDebug>

namespace libgraphics {
namespace fx {
namespace operations {

std::pair<std::string, std::string> makeOpDirective( const std::string& operation ) {
    return std::make_pair( std::string( "$OPERATION" ), operation );
}


static const char genericOperationFromValue_Kernel[] =
    "#version 120 \n"
    "uniform float     Value;"
    "vec4 channelOp() {"
    "   $OPERATION"
    "}"
    "void main() {"
    "   gl_FragColor = channelOp();"
    "}";

static const char genericOperationChannelValue_Kernel[] =
    "#version 120 \n"

    "uniform sampler2D MainTexture;"
    "uniform float     Value;"

    "uniform int Width;"
    "uniform int Height;"

    "vec4 texelFetch( sampler2D tex, float x, float y ) {"
    "        return texture2D( tex, vec2( float(x) / float(Width), float(y) / float(Height) ));"
    "}"
    "vec4 minMax(vec4 val) {"
    " return vec4( max(0.0, min(val.r,1.0) ), max(0.0, min(val.g,1.0) ), max(0.0, min(val.b,1.0) ), max(0.0, min(val.a,1.0) ) );  "
    "}"
    "vec4 channelOp( vec4 color ) {"
    "   $OPERATION"
    "}"
    "void main() {"
    "   vec4 color = texelFetch( MainTexture, gl_FragCoord.x, gl_FragCoord.y );"
    "   gl_FragColor = minMax(channelOp(color));"
    "}";

static const char genericOperationColorValue_Kernel[] =
    "#version 120 \n"

    "uniform sampler2D MainTexture;"
    "uniform vec4      InputColor;"

    "uniform int Width;"
    "uniform int Height;"

    "vec4 texelFetch( sampler2D tex, float x, float y ) {"
    "        return texture2D( tex, vec2( float(x) / float(Width), float(y) / float(Height) ));"
    "}"
    "vec4 minMax(vec4 val) {"
    " return vec4( max(0.0, min(val.r,1.0) ), max(0.0, min(val.g,1.0) ), max(0.0, min(val.b,1.0) ), max(0.0, min(val.a,1.0) ) );  "
    "}"
    "vec4 channelOp( vec4 color ) {"
    "   $OPERATION"
    "}"
    "void main() {"
    "   vec4 color = texelFetch( MainTexture, gl_FragCoord.x, gl_FragCoord.y );"
    "   gl_FragColor = minMax(channelOp(color));"
    "}";

static const char genericOperationImageValue_Kernel[] =
    "#version 120 \n"

    "uniform sampler2D MainTexture;"
    "uniform sampler2D OtherTexture;"

    "uniform int Width;"
    "uniform int Height;"

    "vec4 texelFetch( sampler2D tex, float x, float y ) {"
    "        return texture2D( tex, vec2( float(x) / float(Width), float(y) / float(Height) ));"
    "}"
    "vec4 minMax(vec4 val) {"
    " return vec4( max(0.0, min(val.r,1.0) ), max(0.0, min(val.g,1.0) ), max(0.0, min(val.b,1.0) ), max(0.0, min(val.a,1.0) ) );  "
    "}"
    "vec4 channelOp( vec4 color, vec4 InputColor ) {"
    "   $OPERATION"
    "}"
    "void main() {"
    "   vec4 color = texelFetch( MainTexture, gl_FragCoord.x, gl_FragCoord.y );"
    "   vec4 inputColor = texelFetch( OtherTexture, gl_FragCoord.x, gl_FragCoord.y );"
    "   gl_FragColor = minMax(channelOp(color,inputColor));"
    "}";


GenericImgOpFromValue::GenericImgOpFromValue( const std::pair<std::string, std::string >& dir ) : opDirective( dir ), source( genericOperationFromValue_Kernel ) {
    std::vector< std::pair<std::string, std::string > > directives;
    directives.push_back( opDirective );

    const auto successfullyAppliedDirective = replaceDirectives(
                directives,
                source
            );
    assert( successfullyAppliedDirective );

    if( !successfullyAppliedDirective ) {
        qDebug() << "Error: Failed to apply generic directive to filter source template.";
    }
}

std::string                 GenericImgOpFromValue::getKernelSource() {
    return source;
}
bool                        GenericImgOpFromValue::initializeParameters( backend::gl::Effect* effect ) {
    params.value.reset( "Value", 1.0f );
    {
        params.value.assignId( effect->getParameterId( "Value" ) );
    }

    return true;
}

void GenericImgOpFromValue::submitImageMergeParameters( libgraphics::fxapi::ApiBackendDevice* device, libgraphics::fxapi::ApiImageObject* destination, libgraphics::fxapi::ApiImageObject* source, libgraphics::Rect32I area,
        backend::gl::Texture* texture, backend::gl::Texture* otherTexture, size_t tx, size_t ty ) {
    assert( false );

    ( void )device;
    ( void )destination;
    ( void )source;
    ( void )area;
    ( void )texture;
    ( void )otherTexture;
}
void GenericImgOpFromValue::submitParameters( libgraphics::fxapi::ApiBackendDevice* device,
        libgraphics::fxapi::ApiImageObject* destination,
        libgraphics::fxapi::ApiImageObject* source,
        libgraphics::Rect32I   area,
        backend::gl::Texture* texture, size_t tx, size_t ty ) {
    ( void )device;
    ( void )destination;
    ( void )source;
    ( void )area;
    ( void )texture;

    /// apply parameters
    {
        params.value.reset( value );
        backend::gl::applyEffectParameterToEffect( getEffect(), params.value );
    }
}


GenericImgOpChannelValue::GenericImgOpChannelValue( const std::pair<std::string, std::string >& dir ) : opDirective( dir ), source( genericOperationChannelValue_Kernel ) {
    std::vector< std::pair<std::string, std::string > > directives;
    directives.push_back( opDirective );

    const auto successfullyAppliedDirective = replaceDirectives(
                directives,
                source
            );
    assert( successfullyAppliedDirective );

    if( !successfullyAppliedDirective ) {
        qDebug() << "Error: Failed to apply generic directive to filter source template.";
    }
}

std::string                 GenericImgOpChannelValue::getKernelSource() {
    return source;
}
bool                        GenericImgOpChannelValue::initializeParameters( backend::gl::Effect* effect ) {
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
    params.value.reset( "Value", 1.0f );
    {
        params.value.assignId( effect->getParameterId( "Value" ) );
    }

    return true;
}

void GenericImgOpChannelValue::submitImageMergeParameters( libgraphics::fxapi::ApiBackendDevice* device, libgraphics::fxapi::ApiImageObject* destination, libgraphics::fxapi::ApiImageObject* source, libgraphics::Rect32I area,
        backend::gl::Texture* texture, backend::gl::Texture* otherTexture, size_t tx, size_t ty ) {
    assert( false );

    ( void )device;
    ( void )destination;
    ( void )source;
    ( void )area;
    ( void )texture;
    ( void )otherTexture;
}
void GenericImgOpChannelValue::submitParameters( libgraphics::fxapi::ApiBackendDevice* device,
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
        params.value.reset( value );

        backend::gl::applyEffectParameterToEffect( getEffect(), params.width );
        backend::gl::applyEffectParameterToEffect( getEffect(), params.height );
        backend::gl::applyEffectParameterToEffect( getEffect(), params.mainTexture );
        backend::gl::applyEffectParameterToEffect( getEffect(), params.value );
    }
}



GenericImgOpColorValue::GenericImgOpColorValue( const std::pair<std::string, std::string >& dir ) : opDirective( dir ), source( genericOperationColorValue_Kernel ) {
    std::vector< std::pair<std::string, std::string > > directives;
    directives.push_back( opDirective );

    const auto successfullyAppliedDirective = replaceDirectives(
                directives,
                source
            );
    assert( successfullyAppliedDirective );

    if( !successfullyAppliedDirective ) {
        qDebug() << "Error: Failed to apply generic directive to filter source template.";
    }
}
std::string                 GenericImgOpColorValue::getKernelSource() {
    return source;
}
bool                        GenericImgOpColorValue::initializeParameters( backend::gl::Effect* effect ) {
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
    params.value.reset( "InputColor", libgraphics::backend::gl::createVec4Data( 1.0f, 1.0f, 1.0f, 1.0f ) );
    {
        params.value.assignId( effect->getParameterId( "InputColor" ) );
    }

    return true;
}
void GenericImgOpColorValue::submitImageMergeParameters( libgraphics::fxapi::ApiBackendDevice* device, libgraphics::fxapi::ApiImageObject* destination, libgraphics::fxapi::ApiImageObject* source, libgraphics::Rect32I area,
        backend::gl::Texture* texture, backend::gl::Texture* otherTexture, size_t tx, size_t ty ) {
    assert( false );

    ( void )device;
    ( void )destination;
    ( void )source;
    ( void )area;
    ( void )texture;
    ( void )otherTexture;
}
void GenericImgOpColorValue::submitParameters( libgraphics::fxapi::ApiBackendDevice* device,
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
        params.value.reset( value );

        backend::gl::applyEffectParameterToEffect( getEffect(), params.width );
        backend::gl::applyEffectParameterToEffect( getEffect(), params.height );
        backend::gl::applyEffectParameterToEffect( getEffect(), params.mainTexture );
        backend::gl::applyEffectParameterToEffect( getEffect(), params.value );
    }
}


GenericImgOpImageValue::GenericImgOpImageValue( const std::pair<std::string, std::string >& dir ) : opDirective( dir ), source( genericOperationImageValue_Kernel ) {
    std::vector< std::pair<std::string, std::string > > directives;
    directives.push_back( opDirective );

    const auto successfullyAppliedDirective = replaceDirectives(
                directives,
                source
            );
    assert( successfullyAppliedDirective );

    if( !successfullyAppliedDirective ) {
        qDebug() << "Error: Failed to apply generic directive to filter source template.";
    }
}

std::string                 GenericImgOpImageValue::getKernelSource() {
    return source;
}
bool                        GenericImgOpImageValue::initializeParameters( backend::gl::Effect* effect ) {
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
    params.value.reset( "OtherTexture", nullptr );
    {
        params.value.assignId( effect->getParameterId( "OtherTexture" ) );
    }

    return true;
}

void GenericImgOpImageValue::submitImageMergeParameters( libgraphics::fxapi::ApiBackendDevice* device, libgraphics::fxapi::ApiImageObject* destination, libgraphics::fxapi::ApiImageObject* source, libgraphics::Rect32I area,
        backend::gl::Texture* texture, backend::gl::Texture* otherTexture, size_t tx, size_t ty ) {
    ( void )device;
    ( void )destination;
    ( void )source;

    /// apply parameters
    {
        params.width.reset( area.width );
        params.height.reset( area.height );
        params.mainTexture.reset( texture );
        params.value.reset( otherTexture );

        backend::gl::applyEffectParameterToEffect( this->m_Effect, params.width );
        backend::gl::applyEffectParameterToEffect( this->m_Effect, params.height );
        backend::gl::applyEffectParameterToEffect( this->m_Effect, params.mainTexture );
        backend::gl::applyEffectParameterToEffect( this->m_Effect, params.value );

    }
}

void GenericImgOpImageValue::submitParameters( libgraphics::fxapi::ApiBackendDevice* device,
        libgraphics::fxapi::ApiImageObject* destination,
        libgraphics::fxapi::ApiImageObject* source,
        libgraphics::Rect32I   area,
        backend::gl::Texture* texture, size_t tx, size_t ty ) {
    ( void )device;
    ( void )destination;
    ( void )source;
    ( void )area;
    ( void )texture;

    assert( false );

}

std::string tillNextDelimeter( const std::string& source, size_t index ) {
    assert( index < source.size() );
    assert( source[index] == '$' );

    std::string str;

    const char delimeters[] = {
        '\n',
        ' ',
        ',',
        ';',
        '\t',
        ')',
        '(',
        '+',
        '-',
        '*',
        '/',
        '.',
        '&',
        '|',
        '}',
        '{'
    };
    const auto delimetersLength = sizeof( delimeters ) / sizeof( char );

    for( size_t i = index; source.size() > i; ++i ) {
        bool found( false );

        for( size_t j = 0; delimetersLength > j; ++j ) {
            if( delimeters[j] == source[i] ) {
                found = true;
                break;
            }
        }

        if( found ) {
            break;
        }

        str += source[i];
    }

    assert( !str.empty() );
    return str;
}

bool replaceDirectives(
    std::vector<std::pair<std::string, std::string> >    directives,
    std::string&                                        source
) {
    std::string localSource;

    for( size_t i = 0; source.size() > i; ++i ) {
        if( source[i] != '$' ) {
            localSource += source[i];
        } else {
            const auto directive = tillNextDelimeter(
                                       source,
                                       i
                                   );
            assert( !directive.empty() );
            assert( directive[0] == '$' );

            bool found( false );
            std::string replacement;

            for( auto st = directives.begin(); st != directives.end(); ++st ) {
                if( ( ( *st ).first == directive ) ) {
                    found = true;
                    replacement = ( *st ).second;
                    break;
                }
            }

            assert( found );

            if( !found ) {
                return false;
            }

            i           += ( directive.size() - 1 );
            localSource += replacement;
        }
    }

    /// apply result
    source = localSource;

    return true;
}

bool renderTiledImageMergeOperation(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject*     destination,
    libgraphics::fxapi::ApiImageObject*     source0,
    libgraphics::fxapi::ApiImageObject*     source1,
    libgraphics::Rect32I   area,
    backend::gl::ImageOperation* operation
) {
    assert( device );
    assert( destination );
    assert( source0 );
    assert( source1 );
    assert( operation );

    const auto operationInitialized = operation->initialize( device );
    assert( operationInitialized );

    if( !operationInitialized ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "renderTiledImageOperation(): Failed to initialize ImageOperation.";
#endif
        return false;
    }

    /// render gl
    {
        backend::gl::ImageObject* glDst     = ( backend::gl::ImageObject* )destination;
        backend::gl::ImageObject* glSrc0    = ( backend::gl::ImageObject* )source0;
        backend::gl::ImageObject* glSrc1    = ( backend::gl::ImageObject* )source1;

        /// compatibility checks
        if( glDst->tileCount() != glSrc0->tileCount() ) {
            assert( false );
            return false;
        }

        if( glDst->tileCount() != glSrc1->tileCount() ) {
            assert( false );
            return false;
        }

        if( glDst->width() < ( area.x + area.width ) ) {
            assert( false );
            return false;
        }

        if( glDst->height() < ( area.y + area.height ) ) {
            assert( false );
            return false;
        }

        if( glSrc0->width() < ( area.x + area.width ) ) {
            assert( false );
            return false;
        }

        if( glSrc0->height() < ( area.y + area.height ) ) {
            assert( false );
            return false;
        }

        if( glSrc1->width() < ( area.x + area.width ) ) {
            assert( false );
            return false;
        }

        if( glSrc1->height() < ( area.y + area.height ) ) {
            assert( false );
            return false;
        }

        if( ( glDst->width() != glSrc0->width() ) || ( glDst->height() != glSrc0->height() ) ) {
            assert( false );
            return false;
        }

        /// render loop
        size_t horizontal( glSrc0->countHorizontalTiles() );
        size_t vertical( glSrc0->countVerticalTiles() );

        assert( horizontal == glSrc1->countHorizontalTiles() );
        assert( vertical == glSrc1->countVerticalTiles() );

        for( size_t y = 0; vertical > y; ++y ) {
            for( size_t x = 0; horizontal > x; ++x ) {
                const auto baseTileArea = glDst->tileArea( x, y );
                libgraphics::Rect32I logicalTileArea;

                if( !baseTileArea.getIntersection( area, logicalTileArea ) ) {
                    if( !area.getIntersection( baseTileArea, logicalTileArea ) ) {
                        continue;
                    }
                }

                libgraphics::Rect32I renderArea(
                    logicalTileArea.x - baseTileArea.x,
                    logicalTileArea.y - baseTileArea.y,
                    logicalTileArea.width,
                    logicalTileArea.height
                );


                auto src0Tile = glSrc0->tileTexture( x, y );
                auto src1Tile = glSrc1->tileTexture( x, y );
                auto dstTile  = glDst->tileRenderTarget( x, y )->internalRenderTarget();

                operation->getEffect()->bind();
                {
                    operation->submitImageMergeParameters(
                        device,
                        destination,
                        source0,
                        baseTileArea,
                        src0Tile,
                        src1Tile,
                        x,
                        y
                    );

                    operation->getEffect()->updateTextureParameters();

                    bool _ret = utils::GL::drawEffectToFrameBuffer( backend::gl::globalCtx(), operation->getEffect()->internalProgram(),
                                dstTile, renderArea
                                                                  );
                    ( void )_ret;

                    assert( _ret );

                }
            }
        }

        operation->getEffect()->unbind();
    }

    return true;
}

bool renderTiledImageOperation(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject*     destination,
    libgraphics::fxapi::ApiImageObject*     source,
    libgraphics::Rect32I   area,
    backend::gl::ImageOperation* operation
) {
    assert( device );
    assert( destination );
    assert( source );
    assert( operation );

    const auto operationInitialized = operation->initialize( device );
    assert( operationInitialized );

    if( !operationInitialized ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "renderTiledImageOperation(): Failed to initialize ImageOperation.";
#endif
        return false;
    }

    /// render gl
    {
        backend::gl::ImageObject* glDst = ( backend::gl::ImageObject* )destination;
        backend::gl::ImageObject* glSrc = ( backend::gl::ImageObject* )source;

        /// compatibility checks
        if( glDst->tileCount() != glSrc->tileCount() ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "Error: Failed to render: source and destination not compatible.";
#endif
            assert( false );
            return false;
        }

        if( glDst->width() < ( area.x + area.width ) ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "Error: Failed to render: source and destination not compatible.";
#endif
            assert( false );
            return false;
        }

        if( glDst->height() < ( area.y + area.height ) ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "Error: Failed to render: source and destination not compatible.";
#endif
            assert( false );
            return false;
        }

        if( glSrc->width() < ( area.x + area.width ) ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "Error: Failed to render: source and destination not compatible.";
#endif
            assert( false );
            return false;
        }

        if( glSrc->height() < ( area.y + area.height ) ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "Error: Failed to render: source and destination not compatible.";
#endif
            assert( false );
            return false;
        }

        if( ( glDst->width() != glSrc->width() ) || ( glDst->height() != glSrc->height() ) ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "Error: Failed to render: source and destination not compatible.";
#endif
            assert( false );
            return false;
        }


        /// render loop
        size_t horizontal( glSrc->countHorizontalTiles() );
        size_t vertical( glSrc->countVerticalTiles() );

        for( size_t y = 0; vertical > y; ++y ) {
            for( size_t x = 0; horizontal > x; ++x ) {
                const auto baseTileArea = glDst->tileArea( x, y );
                libgraphics::Rect32I logicalTileArea;

                auto currentSourceTile = glSrc->tileTexture( x, y );
                auto currentDestTile   = glDst->tileRenderTarget( x, y )->internalRenderTarget();

                if( !baseTileArea.getIntersection( area, logicalTileArea ) ) {
                    if( !area.getIntersection( baseTileArea, logicalTileArea ) ) {
                        continue;
                    }
                }

                libgraphics::Rect32I renderArea(
                    logicalTileArea.x - baseTileArea.x,
                    logicalTileArea.y - baseTileArea.y,
                    logicalTileArea.width,
                    logicalTileArea.height
                );

                operation->getEffect()->bind();
                {
                    operation->submitParameters(
                        device,
                        destination,
                        source,
                        baseTileArea,
                        currentSourceTile,
                        x,
                        y
                    );

                    operation->getEffect()->updateTextureParameters();

                    bool _ret = utils::GL::drawEffectToFrameBuffer( backend::gl::globalCtx(), operation->getEffect()->internalProgram(),
                                currentDestTile, renderArea
                                                                  );

                    ( void )_ret;

                    assert( _ret );
                }
            }
        }

        operation->getEffect()->unbind();

    }

    return true;
}

GLImgOp::GLImgOp() : m_Initialized( false ) {}

backend::gl::Effect*        GLImgOp::getEffect() {
    return this->m_Effect;
}

bool                        GLImgOp::initialize( libgraphics::fxapi::ApiBackendDevice* _rawDev ) {
    assert( _rawDev != nullptr );

    if( !this->m_Initialized ) {

        backend::gl::BackendDevice* device = ( backend::gl::BackendDevice* )_rawDev;

        const auto kernelSource = this->getKernelSource();
        assert( !kernelSource.empty() );

        if( kernelSource.empty() ) {
            qDebug() << "Error: Failed to initialize GLImgOp with empty kernel source.";
            return false;
        }

        this->m_Effect = device->effectPool()->createEffect();
        this->m_Effect->reset( "GLImgOp", kernelSource.c_str() );

        assert( !this->m_Effect->empty() );

        if( this->m_Effect->empty() ) {
            qDebug() << "Error: Failed to compile Effect object from kernel source.";
            return false;
        }

        const auto successfullyInitializedParameters = this->initializeParameters(
                    this->getEffect()
                );
        assert( successfullyInitializedParameters );

        if( !successfullyInitializedParameters ) {
            qDebug() << "Error: Failed to initialize kernel parameters.";
            device->effectPool()->remove(
                this->m_Effect
            );

            return false;
        }

        this->m_Initialized = true;
        return true;
    }

    return true;
}


}
}
}
