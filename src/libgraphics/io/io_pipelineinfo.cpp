
#include <string>
#include <cstring>
#include <vector>
#include <libgraphics/io/pipelineinfo.hpp>

namespace libgraphics {
namespace io {

PipelineInfo::PipelineInfo() : d( new Private() ) {

}

bool PipelineInfo::supportsExtension( const char* extension ) {
    for( auto it = d->extensions.begin(); it != d->extensions.end(); ++it ) {
        if( ( *it ).extension == extension ) {
            return ( *it ).importable && ( *it ).exportable;
        }
    }

    return false;
}

bool PipelineInfo::isExtensionImportable( const char* extension ) {
    for( auto it = d->extensions.begin(); it != d->extensions.end(); ++it ) {
        if( ( *it ).extension == extension ) {
            return ( *it ).importable;
        }
    }

    return false;
}

bool PipelineInfo::isExtensionExportable( const char* extension ) {
    for( auto it = d->extensions.begin(); it != d->extensions.end(); ++it ) {
        if( ( *it ).extension == extension ) {
            return ( *it ).exportable;
        }
    }

    return false;
}

size_t  PipelineInfo::countImporters() const {
    size_t count( 0 );

    for( auto it = d->extensions.begin(); it != d->extensions.end(); ++it ) {
        if( ( *it ).importable ) {
            ++count;
        }
    }

    return count;
}

size_t  PipelineInfo::countExporters() const {
    size_t count( 0 );

    for( auto it = d->extensions.begin(); it != d->extensions.end(); ++it ) {
        if( ( *it ).exportable ) {
            ++count;
        }
    }

    return count;
}

void PipelineInfo::addSupportedExtension( const char* extension ) {
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

void PipelineInfo::addImporterExtension( const char* extension ) {
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

void PipelineInfo::addExporterExtension( const char* extension ) {
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
