#include <vector>

#include <libgraphics/io/pipeline.hpp>

namespace libgraphics {
namespace io {

StdPipeline::StdPipeline() : d( new Private() ) {}

/// processing management
/// the pipeline only holds weak references
bool StdPipeline::addProcessingStage(
    PipelineProcessingStage* stage
) {
    assert( stage );

    d->stages.push_back(
        std::unique_ptr< PipelineProcessingStage >( stage )
    );

    return true;
}

bool StdPipeline::removeProcessingStage(
    PipelineProcessingStage* stage
) {
    for( auto it = d->stages.begin(); it != d->stages.end(); ++it ) {
        if( ( *it ).get() == stage ) {
            d->stages.erase( it );
            return true;
        }
    }

    return false;
}

bool StdPipeline::removeProcessingStageByName(
    const char* name
) {
    for( auto it = d->stages.begin(); it != d->stages.end(); ++it ) {
        if( ( *it )->name() == name ) {
            d->stages.erase( it );
            return true;
        }
    }

    return false;
}

bool StdPipeline::removeProcessingStageByIndex(
    size_t index
) {
    if( d->stages.size() > index ) {
        d->stages.erase( d->stages.begin() + index );
        return true;
    }

    return false;
}

PipelineProcessingStage* StdPipeline::getProcessingStageByIndex(
    size_t index
) {
    if( d->stages.size() > index ) {
        return ( *( d->stages.begin() + index ) ).get();
    }

    return nullptr;
}

PipelineProcessingStage* StdPipeline::getFirstProcessingStageByName(
    const char* name
) {
    for( auto it = d->stages.begin(); it != d->stages.end(); ++it ) {
        if( ( *it )->name() == name ) {
            return ( *it ).get();
        }
    }

    return nullptr;
}

bool StdPipeline::containsProcessingStage(
    const char* name
) {
    for( auto it = d->stages.begin(); it != d->stages.end(); ++it ) {
        if( ( *it )->name() == name ) {
            return true;
        }
    }

    return false;
}

bool StdPipeline::containsProcessingStage(
    PipelineProcessingStage* stage
) {
    for( auto it = d->stages.begin(); it != d->stages.end(); ++it ) {
        if( ( *it ).get() == stage ) {
            return true;
        }
    }

    return false;
}

size_t StdPipeline::countProcessingStages() {
    return d->stages.size();
}

size_t StdPipeline::countProcessors() {
    size_t  count( 0 );

    for( auto it = d->stages.begin(); it != d->stages.end(); ++it ) {
        count += ( *it )->countProcessors();
    }

    return count;
}

bool StdPipeline::clearProcessingStages() {
    d->stages.clear();
    return true;
}

void StdPipeline::disableAllProcessingStages() {
    for( auto it = d->stages.begin(); it != d->stages.end(); ++it ) {
        ( *it )->disable();
    }
}

void StdPipeline::enableAllProcessingStages() {
    for( auto it = d->stages.begin(); it != d->stages.end(); ++it ) {
        ( *it )->enable();
    }
}

bool StdPipeline::enableProcessingStage(
    size_t index
) {
    if( d->stages.size() > index ) {
        ( *( d->stages.begin() + index ) )->enable();
        return true;
    }

    return false;
}

bool StdPipeline::enableProcessingStage(
    const char* name
) {
    for( auto it = d->stages.begin(); it != d->stages.end(); ++it ) {
        if( ( *it )->name() == name ) {
            ( *it )->enable();
            return true;
        }
    }

    return false;
}

bool StdPipeline::disableProcessingStage(
    size_t index
) {
    if( d->stages.size() > index ) {
        ( *( d->stages.begin() + index ) )->disable();
        return true;
    }

    return false;
}

bool StdPipeline::disableProcessingStage(
    const char* name
) {
    for( auto it = d->stages.begin(); it != d->stages.end(); ++it ) {
        if( ( *it )->name() == name ) {
            ( *it )->disable();
            return true;
        }
    }

    return false;
}

/// io management
bool StdPipeline::addExportersFromPlugin(
    PipelinePlugin* plugin
) {
    if( plugin ) {
        const auto exporterGroup = plugin->exporters();

        for( auto it = exporterGroup.begin(); it != exporterGroup.end(); ++it ) {
            d->exporters.add( ( *it ) );
        }

        return true;
    }

    return false;
}

bool StdPipeline::addExporter(
    PipelineExporter* exporter
) {
    if( exporter ) {
        d->exporters.add( exporter );
        return true;
    }

    return false;
}

bool StdPipeline::addImportersFromPlugin(
    PipelinePlugin* plugin
) {
    if( plugin ) {
        const auto importerGroup = plugin->importers();

        for( auto it = importerGroup.begin(); it != importerGroup.end(); ++it ) {
            d->importers.add( ( *it ) );
        }

        return true;
    }

    return false;
}

bool StdPipeline::addImporter(
    PipelineImporter* importer
) {
    if( importer ) {
        d->importers.add( importer );
        return true;
    }

    return false;
}

bool StdPipeline::removeExporter(
    PipelineExporter* exporter
) {
    return d->exporters.remove( exporter );
}

bool StdPipeline::removeExporterByName(
    const char* name
) {
    return d->exporters.removeByName( name );
}

bool StdPipeline::removeExportersForExtension(
    const char* extension
) {
    return ( d->exporters.removeByExtension( extension ) > 0 );
}

bool StdPipeline::removeImporter(
    PipelineImporter* importer
) {
    return d->importers.remove( importer );
}

bool StdPipeline::removeImporterByName(
    const char* name
) {
    return d->importers.removeByName( name );
}

bool StdPipeline::removeImportersForExtension(
    const char* extension
) {
    return ( d->importers.removeByExtension( extension ) > 0 );
}

bool StdPipeline::containsExporterForExtension(
    const char* extension
) {
    return ( d->exporters.countByExtension( extension ) > 0 );
}

bool StdPipeline::containsImporterForExtension(
    const char* extension
) {
    return ( d->importers.countByExtension( extension ) > 0 );
}

bool StdPipeline::containsExporter(
    PipelineExporter* exporter
) {
    return d->exporters.contains( exporter );
}

bool StdPipeline::containsImporter(
    PipelineImporter* importer
) {
    return d->importers.contains( importer );
}

void StdPipeline::clearImporters() {
    d->importers.clear();
}

void StdPipeline::clearExporters() {
    d->exporters.clear();
}

size_t StdPipeline::countImporters() {
    return d->importers.count();
}

size_t StdPipeline::countExporters() {
    return d->exporters.count();
}

/// general management methods

void StdPipeline::reset() {
    d.reset( new Private() );
}

libgraphics::io::PipelineInfo   StdPipeline::info() {
    PipelineInfo info;

    for( auto it = d->exporters.begin(); it != d->exporters.end(); ++it ) {
        info.addExporterExtension( ( *it )->mainExtension() );
    }

    for( auto it = d->importers.begin(); it != d->importers.end(); ++it ) {
        info.addImporterExtension( ( *it )->mainExtension() );
    }

    return info;
}

/// io methods
bool StdPipeline::importFromData(
    void* data,
    size_t length,
    libgraphics::Bitmap* out
) {
    for( auto it = d->importers.begin(); it != d->importers.end(); ++it ) {
        if( ( *it )->supportsActionFromData() ) {
            if( ( *it )->importFromData( data, length, out ) ) {
                return true;
            }
        }
    }

    return false;
}

bool StdPipeline::importFromPath(
    const char* path,
    libgraphics::Bitmap* out
) {
    for( auto it = d->importers.begin(); it != d->importers.end(); ++it ) {
        if( ( *it )->supportsActionFromPath( path ) ) {
            if( ( *it )->importFromPath( path, out ) ) {
                return true;
            }
        }
    }

    return false;
}

bool StdPipeline::exportToStream(
    const char* extension,
    void* data,
    size_t length,
    libgraphics::Bitmap* toSave
) {
    auto exporters = d->exporters.selectByExtension( extension );

    for( auto it = exporters.begin(); it != exporters.end(); ++it ) {
        if( ( *it )->supportsActionFromData() ) {
            if( ( *it )->exportToStream( data, length, toSave ) ) {
                return true;
            }
        }
    }

    return false;
}

bool StdPipeline::exportToPath(
    const char* extension,
    const char* path,
    libgraphics::Bitmap* toSave
) {
    auto exporters = d->exporters.selectByExtension( extension );

    for( auto it = exporters.begin(); it != exporters.end(); ++it ) {
        if( ( *it )->supportsActionFromPath( path ) ) {
            if( ( *it )->exportToPath( path, toSave ) ) {
                return true;
            }
        }
    }

    return false;
}


}
}
