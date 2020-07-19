#include <libgraphics/filter.hpp>
#include <QDebug>

namespace libgraphics {

Filter::Filter( const std::string& _name, fxapi::ApiBackendDevice* _device ) : m_Name( _name ), m_Device( _device ) {}

Filter::Filter( fxapi::ApiBackendDevice* _device ) : m_Device( _device ) {}

std::string& Filter::name() {
    return this->m_Name;
}

const std::string& Filter::name() const {
    return this->m_Name;
}

void Filter::setName( const std::string& name ) {
    this->m_Name = name;
}

fxapi::ApiBackendDevice* Filter::backend() const {
    return this->m_Device;
}

void Filter::setBackend( fxapi::ApiBackendDevice* _backend ) {
    this->m_Device = _backend;
}

bool applyFilter(
    fxapi::ApiBackendDevice* backend,
    Filter* filter,
    libgraphics::ImageLayer* destination
) {
    assert( filter );
    assert( destination );

    libcommon::ScopedPtr<libgraphics::ImageLayer> temporarySource(
        destination->duplicate()
    );

    const auto successfullyProcessed = filter->process(
                                           backend,
                                           destination,
                                           temporarySource.get()
                                       );
    assert( successfullyProcessed );

    if( !successfullyProcessed ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to execute filter(" << filter->name().c_str() << ").";
#endif
        return false;
    }

    return true;
}

bool applyFilter(
    fxapi::ApiBackendDevice* backend,
    Filter* filter,
    libgraphics::ImageLayer* destination,
    libgraphics::ImageLayer* source
) {
    assert( filter );
    assert( destination );

    const auto successfullyProcessed = filter->process(
                                           backend,
                                           destination,
                                           source
                                       );
    assert( successfullyProcessed );

    if( !successfullyProcessed ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to execute filter(" << filter->name().c_str() << ").";
#endif
        return false;
    }

    return true;
}

bool applyFilter(
    fxapi::ApiBackendDevice* backend,
    Filter* filter,
    libgraphics::Image* destination
) {
    assert( filter );
    assert( destination );

    for( size_t i = 0; destination->layerCount() > i; ++i ) {
        libcommon::ScopedPtr<libgraphics::ImageLayer> temporarySource(
            destination->layerByIndex( i )->duplicate()
        );

        const auto successfullyProcessed = filter->process(
                                               backend,
                                               destination->layerByIndex( i ),
                                               temporarySource.get()
                                           );
        assert( successfullyProcessed );

        if( !successfullyProcessed ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "Failed to execute filter(" << filter->name().c_str() << ").";
#endif
            return false;
        }
    }

    return true;
}

bool applyFilter(
    fxapi::ApiBackendDevice* backend,
    Filter* filter,
    libgraphics::Image* destination,
    libgraphics::Image* source
) {
    assert( filter );
    assert( destination );

    const auto count = std::min( source->layerCount(), destination->layerCount() );
    assert( count > 0 );

    for( size_t i = 0; count > i; ++i ) {
        const auto successfullyProcessed = filter->process(
                                               backend,
                                               destination->layerByIndex( i ),
                                               source->layerByIndex( i )
                                           );
        assert( successfullyProcessed );

        if( !successfullyProcessed ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "Failed to execute filter(" << filter->name().c_str() << ").";
#endif
            return false;
        }
    }

    return true;
}

}
