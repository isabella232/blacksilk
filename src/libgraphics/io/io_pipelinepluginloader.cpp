
#include <libgraphics/io/pipelineplugin.hpp>
#include <libgraphics/io/pipelinepluginloader.hpp>
#include <log/log.hpp>

#include <QLibrary>

namespace libgraphics {
namespace io {

struct PipelinePluginLoader::Private : libcommon::PimplPrivate {
    CallbackCreatePipelinePlugin*   callback;
    std::string path;

#if LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_WINDOWS
    HMODULE library;
    Private() : callback( nullptr ), library( nullptr ) {}
#else
    QLibrary library;
    Private() : callback( nullptr ) {}
#endif

};

PipelinePluginLoader::PipelinePluginLoader() : d( new Private() ) {}

const char* PipelinePluginLoader::path() const {
    return d->path.c_str();
}

bool PipelinePluginLoader::opened() const {
#if LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_WINDOWS
    return ( d->library != nullptr );
#else
    return ( d->library.isLoaded() );
#endif
}

bool PipelinePluginLoader::load( const char* path ) {
    d.reset( new Private() );

    d->path.assign( path );
#if LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_WINDOWS
    d->library = ::LoadLibrary( path );

    if( d->library != nullptr ) {
        d->callback = ( CallbackCreatePipelinePlugin* )::GetProcAddress( d->library, LIBGRAPHICS_IO_CallbackCreatePipelinePluginApiName );
#else
    d->library.setFileName( QString( path ) );

    if( d->library.load() ) {
        d->callback = ( CallbackCreatePipelinePlugin* )d->library.resolve( LIBGRAPHICS_IO_CallbackCreatePipelinePluginApiName );
#endif

        if( d->callback ) {
            return true;
        }

#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        LOG( "Failed to load plugin. Callback not found or invalid." );
#endif
    }

#ifdef LIBGRAPHICS_DEBUG_OUTPUT
    LOG( std::string( "Failed to load plugin from file-path '" ) + path + "'." );
#if LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_WINDOWS
    LOG( std::string( "GetLastError : " ) + std::to_string( ::GetLastError() ) );
#endif
#endif

    d.reset( new Private() );

    return false;
}

PipelinePlugin* PipelinePluginLoader::instantiatePlugin() {
    if( d->callback ) {
        PipelinePlugin* plgObject( nullptr );

        if( ( *d->callback )( &plgObject ) ) {
            return plgObject;
        }
    }

    return nullptr;
}

}
}
