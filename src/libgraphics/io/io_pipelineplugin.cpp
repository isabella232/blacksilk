#include <libgraphics/io/pipelineplugin.hpp>

namespace libgraphics {
namespace io {

struct GenericPipelinePlugin::Private : libcommon::PimplPrivate {
    PipelineExporterGroup       exporters;
    PipelineImporterGroup       importers;
    std::string                 name;

};

GenericPipelinePlugin::GenericPipelinePlugin( const char* name ) : d( new Private() ) {
    d->name = name;
}

const char* GenericPipelinePlugin::name() {
    return d->name.c_str();
}

size_t GenericPipelinePlugin::countExporters() {
    return d->exporters.size();
}

size_t GenericPipelinePlugin::countImporters() {
    return d->importers.size();
}

size_t GenericPipelinePlugin::countExportersForExtension( const char* extension ) {
    return d->exporters.countByExtension( extension );
}

size_t GenericPipelinePlugin::countImportersForExtension( const char* extension ) {
    return d->importers.countByExtension( extension );
}

PipelineImporterGroup GenericPipelinePlugin::importersForExtension( const char* extension ) {
    return d->importers.selectByExtension( extension );
}

PipelineExporterGroup GenericPipelinePlugin::exportersForExtension( const char* extension ) {
    return d->exporters.selectByExtension( extension );
}

PipelineImporterGroup GenericPipelinePlugin::importers() {
    return d->importers;
}

PipelineExporterGroup GenericPipelinePlugin::exporters() {
    return d->exporters;
}

PipelinePluginInfo GenericPipelinePlugin::info() {
    PipelinePluginInfo info;

    for( auto it = d->exporters.begin(); it != d->exporters.end(); ++it ) {
        info.addExporterExtension( ( *it )->mainExtension() );
    }

    for( auto it = d->importers.begin(); it != d->importers.end(); ++it ) {
        info.addImporterExtension( ( *it )->mainExtension() );
    }

    return info;
}

/// methods
void GenericPipelinePlugin::registerPipelineImporter( PipelineImporter* importer ) {
    d->importers.add( importer );
}

void GenericPipelinePlugin::registerPipelineImporter( const libcommon::SharedPtr<PipelineImporter>& importer ) {
    d->importers.add( importer );
}

void GenericPipelinePlugin::registerPipelineExporter( PipelineExporter* exporter ) {
    d->exporters.add( exporter );
}

void GenericPipelinePlugin::registerPipelineExporter( const libcommon::SharedPtr<PipelineExporter>& exporter ) {
    d->exporters.add( exporter );
}

bool GenericPipelinePlugin::unregisterPipelineImporter( PipelineImporter* importer ) {
    return d->importers.remove( importer );
}

bool GenericPipelinePlugin::unregisterPipelineExporter( PipelineExporter* exporter ) {
    return d->exporters.remove( exporter );
}

void GenericPipelinePlugin::setName( const char* name ) {
    d->name = name;
}

}
}
