
#include <QDebug>
#include <libgraphics/fx/filters/vignette.hpp>
#include <libgraphics/fx/operations/basic.hpp>
#include <libgraphics/fx/operations/complex.hpp>

namespace libgraphics {
namespace fx {
namespace filters {

Vignette::Vignette( fxapi::ApiBackendDevice* _device )  : libgraphics::Filter( "Vignette", _device ), m_Strength( 1.0f ), m_Radius( 1.0f ) {}


bool Vignette::process(
    libgraphics::ImageLayer*    destination,
    libgraphics::ImageLayer*    source
) {
    assert( this->backend() != nullptr );

    return this->process(
               this->backend(),
               destination,
               source
           );
}

bool Vignette::process(
    fxapi::ApiBackendDevice*    device,
    libgraphics::ImageLayer*    destination,
    libgraphics::ImageLayer*    source
) {
    assert( device != nullptr );
    assert( destination != nullptr );
    assert( source != nullptr );

    if( device == nullptr ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to process Vignette filter using invalid backend device.";
#endif
        return false;
    }

    if( destination == nullptr ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to process Vignette filter using invalid destination buffer as target.";
#endif
        return false;
    }

    if( source == nullptr ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to process Vignette filter using invalid source buffer.";
#endif
        return false;
    }

    libgraphics::fx::operations::applyVignette(
        device,
        destination,
        source,
        source->size(),
        this->center(),
        this->radius(),
        this->strength()
    );

    return true;
}


FilterPreset Vignette::toPreset() const {
    FilterPreset preset;

    preset.floats()["X"].value = this->m_Center.x;
    preset.floats()["Y"].value = this->m_Center.y;
    preset.floats()["Strength"].value = this->m_Strength;
    preset.floats()["Radius"].value = this->m_Radius;

    preset.filterName() = this->m_Name;
    preset.name()       = "Current";

    return preset;
}

bool Vignette::fromPreset( const FilterPreset& preset ) {
    bool success( false );

    if( preset.containsValueWithName( "X" ) ) {
        const auto valueIt = preset.floats().find( "X" );

        if( valueIt != preset.floats().end() ) {
            success = true;
            this->m_Center.x = ( *valueIt ).second.value;
        }
    }

    if( preset.containsValueWithName( "Y" ) ) {
        const auto valueIt = preset.floats().find( "Y" );

        if( valueIt != preset.floats().end() ) {
            success = true;
            this->m_Center.y = ( *valueIt ).second.value;
        }
    }

    if( preset.containsValueWithName( "Strength" ) ) {
        const auto valueIt = preset.floats().find( "Strength" );

        if( valueIt != preset.floats().end() ) {
            success = true;
            this->m_Strength = ( *valueIt ).second.value;
        }
    }

    if( preset.containsValueWithName( "Radius" ) ) {
        const auto valueIt = preset.floats().find( "Radius" );

        if( valueIt != preset.floats().end() ) {
            success = true;
            this->m_Radius = ( *valueIt ).second.value;
        }
    }

    return success;
}


float& Vignette::strength() {
    return this->m_Strength;
}

const float& Vignette::strength() const {
    return this->m_Strength;
}


void Vignette::setStrength( const float& value ) {
    this->m_Strength = value;
}


float& Vignette::radius() {
    return this->m_Radius;
}

const float& Vignette::radius() const {
    return this->m_Radius;
}

void Vignette::setRadius( const float& value ) {
    this->m_Radius = value;
}


libgraphics::Point32F& Vignette::center() {
    return this->m_Center;
}

const libgraphics::Point32F& Vignette::center() const {
    return this->m_Center;
}

void Vignette::setCenter( const libgraphics::Point32F center ) {
    this->m_Center = center;
}

Filter* Vignette::clone() {
    Vignette* clonedFilter = new Vignette(
        this->m_Device
    );
    clonedFilter->m_Device      = m_Device;
    clonedFilter->m_Name        = m_Name;

    clonedFilter->m_Center      = m_Center;
    clonedFilter->m_Radius      = m_Radius;
    clonedFilter->m_Strength    = m_Strength;

    return ( Filter* )clonedFilter;
}
}
}
}
