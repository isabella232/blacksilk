
#include <string>
#include <cstring>
#include <vector>
#include <libgraphics/io/pipelineplugininfo.hpp>

namespace libgraphics {
namespace io {

PipelinePluginInfo::PipelinePluginInfo() : d( new Private() ) {

}

bool PipelinePluginInfo::supportsExtension( const char* extension ) {
    for( auto it = d->extensions.begin(); it != d->extensions.end(); ++it ) {
        if( ( *it ).extension == extension ) {
            return ( *it ).importable && ( *it ).exportable;
        }
    }

    return false;
}

bool PipelinePluginInfo::isExtensionImportable( const char* extension ) {
    for( auto it = d->extensions.begin(); it != d->extensions.end(); ++it ) {
        if( ( *it ).extension == extension ) {
            return ( *it ).importable;
        }
    }

    return false;
}

bool PipelinePluginInfo::isExtensionExportable( const char* extension ) {
    for( auto it = d->extensions.begin(); it != d->extensions.end(); ++it ) {
        if( ( *it ).extension == extension ) {
            return ( *it ).exportable;
        }
    }

    return false;
}

size_t  PipelinePluginInfo::countImporters() const {
    size_t count( 0 );

    for( auto it = d->extensions.begin(); it != d->extensions.end(); ++it ) {
        if( ( *it ).importable ) {
            ++count;
        }
    }

    return count;
}

size_t  PipelinePluginInfo::countExporters() const {
    size_t count( 0 );

    for( auto it = d->extensions.begin(); it != d->extensions.end(); ++it ) {
        if( ( *it ).exportable ) {
            ++count;
        }
    }

    return count;
}

void PipelinePluginInfo::addSupportedExtension( const char* extension ) {
    for( auto it = d->extensions.begin(); it != d->extensions.end(); ++it ) {
        if( ( *it ).extension == extension ) {
            return;
        }
    }

    Private::Info info;
    info.exportable = true;
    info.importable = true;
    info.extension.assign( extension, strlen( extension ) );

    d->extensions.push_back( info );
}

void PipelinePluginInfo::addImporterExtension( const char* extension ) {
    for( auto it = d->extensions.begin(); it != d->extensions.end(); ++it ) {
        if( ( *it ).extension == extension ) {
            ( *it ).importable = true;

            return;
        }
    }

    Private::Info info;
    info.exportable = false;
    info.importable = true;
    info.extension.assign( extension, strlen( extension ) );

    d->extensions.push_back( info );
}

void PipelinePluginInfo::addExporterExtension( const char* extension ) {
    for( auto it = d->extensions.begin(); it != d->extensions.end(); ++it ) {
        if( ( *it ).extension == extension ) {
            ( *it ).exportable = true;

            return;
        }
    }

    Private::Info info;
    info.exportable = true;
    info.importable = false;
    info.extension.assign( extension, strlen( extension ) );

    d->extensions.push_back( info );
}

}
}
