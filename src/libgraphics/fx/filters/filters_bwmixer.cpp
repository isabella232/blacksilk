
#include <libgraphics/fx/filters/bwmixer.hpp>
#include <libgraphics/fx/operations/basic.hpp>

namespace libgraphics {
namespace fx {
namespace filters {

BWMixer::BWMixer( fxapi::ApiBackendDevice* _device ) : libgraphics::Filter( "BWMixer", _device ) {
    this->resetToUniform();
}

bool BWMixer::process(
    libgraphics::ImageLayer*    destination,
    libgraphics::ImageLayer*    source
) {
    return process(
               this->backend(),
               destination,
               source
           );
}

bool BWMixer::process(
    fxapi::ApiBackendDevice*    device,
    libgraphics::ImageLayer*    destination,
    libgraphics::ImageLayer*    source
) {
    assert( destination != nullptr );
    assert( source != nullptr );
    assert( device != nullptr );

    if( destination == nullptr || source == nullptr ) {
        return false;
    }

    const float factors[3] = {
        this->m_RedSensitivity,
        this->m_GreenSensitivity,
        this->m_BlueSensitivity
    };

    libgraphics::fx::operations::convertToMonochrome(
        destination,
        source,
        source->size(),
        ( float* )factors
    );

    return true;
}

FilterPreset BWMixer::toPreset() const {
    FilterPreset preset;

    preset.floats()["RedSensitivity"].value = this->m_RedSensitivity;
    preset.floats()["GreenSensitivity"].value = this->m_GreenSensitivity;
    preset.floats()["BluedSensitivity"].value = this->m_BlueSensitivity;

    preset.filterName() = this->m_Name;
    preset.name()       = "Current";

    return preset;
}

bool BWMixer::fromPreset( const FilterPreset& preset ) {
    bool success( false );

    if( preset.containsValueWithName( "RedSensitivity" ) ) {
        const auto valueIt = preset.floats().find( "RedSensitivity" );

        if( valueIt != preset.floats().end() ) {
            success = true;
            this->m_RedSensitivity = ( *valueIt ).second.value;
        }
    }

    if( preset.containsValueWithName( "GreenSensitivity" ) ) {
        const auto valueIt = preset.floats().find( "GreenSensitivity" );

        if( valueIt != preset.floats().end() ) {
            success = true;
            this->m_GreenSensitivity = ( *valueIt ).second.value;
        }
    }

    if( preset.containsValueWithName( "BlueSensitivity" ) ) {
        const auto valueIt = preset.floats().find( "BlueSensitivity" );

        if( valueIt != preset.floats().end() ) {
            success = true;
            this->m_BlueSensitivity = ( *valueIt ).second.value;
        }
    }

    return success;
}


float& BWMixer::redSensitivity() {
    return this->m_RedSensitivity;
}

const float& BWMixer::redSensitivity() const {
    return this->m_RedSensitivity;
}

void BWMixer::setRedSensitivity( const float& val ) {
    this->m_RedSensitivity = val;
}

float& BWMixer::greenSensitivity() {
    return this->m_GreenSensitivity;
}

const float& BWMixer::greenSensitivity() const {
    return this->m_GreenSensitivity;
}

void BWMixer::setGreenSensitivity( const float& val ) {
    this->m_GreenSensitivity = val;
}

float& BWMixer::blueSensitivity() {
    return this->m_BlueSensitivity;
}

const float& BWMixer::blueSensitivity() const {
    return this->m_BlueSensitivity;
}

void BWMixer::setBlueSensitivity( const float& val ) {
    this->m_BlueSensitivity = val;
}

void BWMixer::resetToUniform() {
    this->m_RedSensitivity = 1.0f;
    this->m_GreenSensitivity = 1.0f;
    this->m_BlueSensitivity = 1.0f;
}

void BWMixer::reset( float r, float g, float b ) {
    this->m_RedSensitivity = r;
    this->m_GreenSensitivity = g;
    this->m_BlueSensitivity = b;
}


Filter* BWMixer::clone() {
    BWMixer* clonedFilter = new BWMixer(
        this->m_Device
    );
    clonedFilter->m_Device      = m_Device;
    clonedFilter->m_Name        = m_Name;

    clonedFilter->m_BlueSensitivity     = m_BlueSensitivity;
    clonedFilter->m_RedSensitivity      = m_RedSensitivity;
    clonedFilter->m_GreenSensitivity    = m_GreenSensitivity;

    return ( Filter* )clonedFilter;
}

}
}
}
