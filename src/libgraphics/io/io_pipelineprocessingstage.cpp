#include <libgraphics/io/pipelineprocessor.hpp>
#include <libgraphics/io/pipelineprocessingstage.hpp>
#include <vector>

namespace libgraphics {
namespace io {

struct PipelineProcessingStage::Private {
    std::string name;
    bool enabled;
    std::vector<std::shared_ptr<PipelineProcessor> > processors;

    Private() : enabled( false ) {}
};

PipelineProcessingStage::PipelineProcessingStage() : d( new Private() ) {}

bool PipelineProcessingStage::enabled() const {
    return d->enabled;
}

void PipelineProcessingStage::enable() {
    d->enabled = true;
}

void PipelineProcessingStage::disable() {
    d->enabled = false;
}

void PipelineProcessingStage::addProcessor( PipelineProcessor* processor ) {
    d->processors.push_back( std::shared_ptr<PipelineProcessor>( processor ) );
}

bool PipelineProcessingStage::removeProcessor( PipelineProcessor* processor ) {
    for( auto it = d->processors.begin(); it != d->processors.end(); ++it ) {
        if( ( *it ).get() == processor ) {
            d->processors.erase( it );
            return true;
        }
    }

    return false;
}

void PipelineProcessingStage::clearProcessors() {
    d->processors.clear();
}

size_t PipelineProcessingStage::countProcessors() const {
    return d->processors.size();
}

bool PipelineProcessingStage::containsProcessor( PipelineProcessor* processor ) {
    for( auto it = d->processors.begin(); it != d->processors.end(); ++it ) {
        if( ( *it ).get() == processor ) {
            return true;
        }
    }

    return false;
}

const std::string& PipelineProcessingStage::name() const {
    return d->name;
}

void PipelineProcessingStage::setName( const std::string& name ) {
    d->name = name;
}

bool PipelineProcessingStage::process( libgraphics::Bitmap* bitmap ) {
    for( auto it = d->processors.begin(); d->processors.end() != it; ++it ) {
        if( !( *it )->process( bitmap ) ) {
            return false;
        }
    }

    return true;
}

}
}
