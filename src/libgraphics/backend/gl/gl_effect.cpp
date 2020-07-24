#include <libgraphics/backend/gl/gl_backenddevice.hpp>
#include <libgraphics/backend/gl/gl_effect.hpp>
#include <libgraphics/backend/gl/gl_priv.hpp>
#include <libgraphics/utils/gl.hpp>
#include <libgraphics/debug.hpp>

#include <gl/glew.h>

#include <QDebug>
#include <QFile>

namespace libgraphics {
namespace backend {
namespace gl {

void applyEffectParameterToEffect( Effect* effect, EffectParameterInt& param ) {
    assert( effect );

    if( !param.changed() ) {
        return;
    }

    bool ret = effect->applyParameter( param.id(), param.value() );

    if( !ret ) {
        qDebug() << "Failed to apply parameter with name" << "'" << param.name().c_str() << "' to effect '" << effect->name() << "'.";
    }
}

void applyEffectParameterToEffect( Effect* effect, EffectParameterShort& param ) {
    assert( effect );

    if( !param.changed() ) {
        return;
    }

    bool ret = effect->applyParameter( param.id(), param.value() );

    if( !ret ) {
        qDebug() << "Failed to apply parameter with name" << "'" << param.name().c_str() << "' to effect '" << effect->name() << "'.";
    }
}

void applyEffectParameterToEffect( Effect* effect, EffectParameterFloat& param ) {
    assert( effect );

    if( !param.changed() ) {
        return;
    }

    bool ret = effect->applyParameter( param.id(), param.value() );

    if( !ret ) {
        qDebug() << "Failed to apply parameter with name" << "'" << param.name().c_str() << "' to effect '" << effect->name() << "'.";
    }
}

void applyEffectParameterToEffect( Effect* effect, EffectParameterVec2& param ) {
    assert( effect );

    if( !param.changed() ) {
        return;
    }

    bool ret = effect->applyParameterVec2( param.id(), ( float* )param.value().data );

    if( !ret ) {
        qDebug() << "Failed to apply parameter with name" << "'" << param.name().c_str() << "' to effect '" << effect->name() << "'.";
    }
}

void applyEffectParameterToEffect( Effect* effect, EffectParameterVec3& param ) {
    assert( effect );

    if( !param.changed() ) {
        return;
    }

    bool ret = effect->applyParameterVec3( param.id(), ( float* )param.value().data );

    if( !ret ) {
        qDebug() << "Failed to apply parameter with name" << "'" << param.name().c_str() << "' to effect '" << effect->name() << "'.";
    }
}

void applyEffectParameterToEffect( Effect* effect, EffectParameterVec4& param ) {
    assert( effect );

    if( !param.changed() ) {
        return;
    }

    bool ret = effect->applyParameterVec4( param.id(), ( float* )param.value().data );

    if( !ret ) {
        qDebug() << "Failed to apply parameter with name" << "'" << param.name().c_str() << "' to effect '" << effect->name() << "'.";
    }
}

void applyEffectParameterToEffect( Effect* effect, EffectParameterMat9& param ) {
    assert( effect );

    if( !param.changed() ) {
        return;
    }

    bool ret = effect->applyParameterMat9( param.id(), ( float* )param.value().data );

    if( !ret ) {
        qDebug() << "Failed to apply parameter with name" << "'" << param.name().c_str() << "' to effect '" << effect->name() << "'.";
    }
}

void applyEffectParameterToEffect( Effect* effect, EffectParameterMat16& param ) {
    assert( effect );

    if( !param.changed() ) {
        return;
    }

    bool ret = effect->applyParameterMat16( param.id(), ( float* )param.value().data );

    if( !ret ) {
        qDebug() << "Failed to apply parameter with name" << "'" << param.name().c_str() << "' to effect '" << effect->name() << "'.";
    }
}

void applyEffectParameterToEffect( Effect* effect, EffectParameterTexture& param ) {
    assert( effect );

    if( !param.changed() ) {
        return;
    }

    bool ret = effect->applyParameterTexture( param.id(), param.value() );

    if( !ret ) {
        qDebug() << "Failed to apply parameter with name" << "'" << param.name().c_str() << "' to effect '" << effect->name() << "'.";
    }
}

void applyEffectParameterToEffect( Effect* effect, EffectParameterPixelArray& param ) {
    assert( effect );

    if( !param.changed() ) {
        return;
    }

    bool ret = effect->applyParameterPixelArray( param.id(), param.value() );

    if( !ret ) {
        qDebug() << "Failed to apply parameter with name" << "'" << param.name().c_str() << "' to effect '" << effect->name() << "'.";
    }
}

struct Effect::Private : libcommon::PimplPrivate {
    utils::GLProgram*   program;
    utils::GLShader*    pixelShader;
    std::string         name;
    bool                bound;
    size_t              usageCount;

    std::vector<std::pair<int, Texture*> >       boundTextures;
    std::vector<std::pair<int, PixelArray*> >    boundPixelArrays;
    std::map<int, std::string>                  knownUniforms;

    Private() : program( nullptr ), pixelShader( nullptr ), bound( false ), usageCount( 0 ) {}
    ~Private() {
        knownUniforms.clear();
        boundTextures.clear();

        if( pixelShader ) {
            bool ret = utils::GL::destroyResource(
                           globalCtx(),
                           pixelShader
                       );
            assert( ret );
        }

        if( program ) {
            bool ret = utils::GL::destroyResource(
                           globalCtx(),
                           program
                       );
            assert( ret );
        }

        program = nullptr;
        pixelShader = nullptr;
    }
};

static const char svprogram_source[] =
    "        #version 120 \n"
    "                     \n"
    "        void main() { \n"
    "           gl_Position    = gl_ModelViewProjectionMatrix * gl_Vertex; \n"
    "           gl_TexCoord[0] = gl_MultiTexCoord0; \n"
    "       }\n";
struct StandardVertexProgram {
    StandardVertexProgram() {
        std::vector<std::string> errors;
        bool ret = utils::GL::createVertexShader(
                       globalCtx(),
                       &shader,
                       std::string( svprogram_source ),
                       &errors
                   );

        if( !ret ) {
            qDebug() << "Failed to compile effect with name: " << "StandardVertexProgram";

            if( !errors.empty() ) {
                for( auto it = errors.begin(); it != errors.end(); ++it ) {
                    qDebug() << "GLSL-Compiler error: " << ( *it ).c_str();
                }
            }
        }

        assert( ret );
    }
    ~StandardVertexProgram() {
        utils::GL::destroyResource( globalCtx(), shader );
        shader = nullptr;
    }

    utils::GLShader*    shader;
};
static std::unique_ptr<StandardVertexProgram>  svprogram;
utils::GLShader*    getStandardVertexShader() {
    if( !svprogram ) {
        svprogram.reset( new StandardVertexProgram() );
    }

    return svprogram->shader;
}

Effect::Effect() : d( new Private() ) {
    LIBGRAPHICS_MEMORY_LOG_ALLOCATE( this );
}

Effect::Effect( const char* name, const char* source ) : d( new Private() ) {
    ( void )name;
    ( void )source;

    reset( name, source );
}

Effect::~Effect() {
    LIBGRAPHICS_MEMORY_LOG_DEALLOCATE( this );

    d.reset();
}

void Effect::reset() {
    d.reset( new Private() );
}

void Effect::reset( const char* name, const char* source ) {
    d.reset( new Private() );

    d->name.assign( name );

    std::vector<std::string> errors;
    bool ret = utils::GL::createFragmentShader(
                   globalCtx(),
                   &d->pixelShader,
                   std::string( source ),
                   &errors
               );

    if( !ret ) {
        qDebug() << "Error: Failed to compile effect with name: " << name;

        if( !errors.empty() ) {
            for( auto it = errors.begin(); it != errors.end(); ++it ) {
                qDebug() << "GLSL-Compiler error: " << ( *it ).c_str();
            }
        }
    }

    assert( ret );

    errors.clear();
    ret = utils::GL::createProgram(
              globalCtx(),
              &d->program,
              getStandardVertexShader(),
              d->pixelShader,
              &errors
          );

    if( !ret ) {
        qDebug() << "Error: Failed to link effect with name: " << name;

        if( !errors.empty() ) {
            for( auto it = errors.begin(); it != errors.end(); ++it ) {
                qDebug() << "GLSL-Linker error: " << ( *it ).c_str();
            }
        }
    }

    assert( ret );
    assert( d->program->linked() );
}

void Effect::bind() {
    assert( !empty() );

    if( !d->bound && !empty() ) {

        if( !utils::GL::bindProgram( globalCtx(), d->program ) ) {
            assert( false );
            return;
        }

        d->bound = true;
    }
}

void Effect::updateTextureParameters() {
    if( d->bound ) {
        unsigned int counter( 0 );

        for( auto it = d->boundTextures.begin(); it != d->boundTextures.end(); ++it ) {
            utils::GL::setUniformTexture( globalCtx(), d->program, ( *it ).first, counter );

            utils::GL::activeTexture( globalCtx(), counter );
            utils::GL::bindTexture( globalCtx(), ( *it ).second->d->tex );

            ++counter;
        }

        ++counter;

        for( auto it = d->boundPixelArrays.begin(); it != d->boundPixelArrays.end(); ++it ) {
            utils::GL::setUniformTexture( globalCtx(), d->program, ( *it ).first, counter );

            utils::GL::activeTexture( globalCtx(), counter );
            utils::GL::bindTexture( globalCtx(), ( *it ).second->texture() );

            ++counter;
        }

        /// reset
        ( void ) utils::GL::activeTexture( globalCtx(), 0 );
    }
}

void Effect::unbind() {
    assert( !empty() );

    if( d->bound && !empty() ) {
        utils::GL::unbindProgram( globalCtx(), d->program );

        d->bound = false;
    }
}

bool Effect::empty() const {
    return ( d->pixelShader == nullptr ) || ( d->program == nullptr );
}

const char* Effect::name() const {
    return d->name.c_str();
}

bool Effect::containsParameter( const char* name ) {
    utils::GL::t_int loc( 0 );

    if( !utils::GL::getUniformLocation( globalCtx(), d->program, std::string( name, strlen( name ) ), &loc ) ) {
        return false;
    }

    return ( loc >= 0 );
}

int Effect::getParameterId( const char* name ) {
    utils::GL::t_int loc( 0 );

    if( !utils::GL::getUniformLocation( globalCtx(), d->program, std::string( name, strlen( name ) ), &loc ) ) {
        return -1;
    }

    return loc;
}

bool Effect::applyParameter( const int& id, int value ) {
    if( d->bound ) {
        glUniform1i( id, value );
        return true;
    }

    return false;
}

bool Effect::applyParameter( const int& id, short value ) {
    if( d->bound ) {
        glUniform1i( id, value );
        return true;
    }

    return false;
}

bool Effect::applyParameter( const int& id, float value ) {
    if( d->bound ) {
        glUniform1f( id, value );
        return true;
    }

    return false;
}


bool Effect::applyParameterVec2( const int& id, float* vals ) {
    if( d->bound ) {
        glUniform2fv( id, 1, vals );
        return true;
    }

    return false;
}

bool Effect::applyParameterVec3( const int& id, float* vals ) {
    if( d->bound ) {
        glUniform3fv( id, 1, vals );
        return true;
    }

    return false;
}

bool Effect::applyParameterVec4( const int& id, float* vals ) {
    if( d->bound ) {
        glUniform4fv( id, 1, vals );
        return true;
    }

    return false;
}

bool Effect::applyParameterMat9( const int& id, float* vals ) {
    if( d->bound ) {
        return utils::GL::setUniformMatrix9f( globalCtx(), d->program, id, vals );
    }

    return false;
}

bool Effect::applyParameterMat16( const int& id, float* vals ) {
    if( d->bound ) {
        return utils::GL::setUniformMatrix16f( globalCtx(), d->program, id, vals );
    }

    return false;
}


bool Effect::applyParameterPixelArray( const int& id, PixelArray* tex ) {
    assert( tex );
    assert( id >= 0 );

    for( auto it = this->d->boundPixelArrays.begin(); it != this->d->boundPixelArrays.end(); ++it ) {
        if( ( *it ).first == id ) {
            ( *it ).second = tex;
            return true;
        }
    }

    this->d->boundPixelArrays.push_back(
        std::make_pair( id, tex )
    );

    return true;
}

bool Effect::applyParameterTexture( const int& id, Texture* tex ) {
    assert( tex );
    assert( id >= 0 );

    for( auto it = this->d->boundTextures.begin(); it != this->d->boundTextures.end(); ++it ) {
        if( ( *it ).first == id ) {
            ( *it ).second = tex;
            return true;
        }
    }

    this->d->boundTextures.push_back(
        std::make_pair( id, tex )
    );

    return true;
}

size_t Effect::getGpuMemoryConsumption() {
    return 0;
}

size_t Effect::getCpuMemoryConsumption() {
    return 0;
}

bool Effect::acquired() {
    return ( d->usageCount != 0 );
}

bool Effect::tryAcquire() {
    ++d->usageCount;
    return true;
}

void Effect::acquire() {
    ++d->usageCount;
}

bool Effect::release() {
    --d->usageCount;
    return true;
}

utils::GLProgram*   Effect::internalProgram() const {
    return d->program;
}

Effect* EffectPool::createEffect() {
    Effect* effect = new Effect();

    this->add( effect );
    effect->acquire();

    return effect;
}

Effect* EffectPool::createEffectFromSource( const char* name, const char* source ) {
    Effect* effect = new Effect(
        name,
        source
    );

    this->add( effect );
    effect->acquire();

    return effect;
}

Effect* EffectPool::createEffectFromPath( const char* name, const char* path ) {
    std::string source;

    QFile file( path );

    if( !file.isOpen() ) {
        return nullptr;
    }

    QByteArray contents = file.readAll();
    source.assign( contents.data(), contents.size() );

    Effect* effect = new Effect(
        name,
        source.c_str()
    );

    this->add( effect );
    effect->acquire();

    return effect;
}


}
}
}
