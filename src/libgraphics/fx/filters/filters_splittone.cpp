
#include <QDebug>
#include <libgraphics/fx/operations/basic.hpp>
#include <libgraphics/fx/operations/complex.hpp>
#include <libgraphics/fx/filters/splittone.hpp>

namespace libgraphics {
namespace fx {
namespace filters {

SplitTone::SplitTone( fxapi::ApiBackendDevice* _device ) : Filter( "SplitTone", _device ), m_Balance( 0.0f ) {}

bool SplitTone::process(
    libgraphics::ImageLayer*    destination,
    libgraphics::ImageLayer*    source
) {
    return process(
               this->backend(),
               destination,
               source
           );
}

bool SplitTone::process(
    fxapi::ApiBackendDevice*    device,
    libgraphics::ImageLayer*    destination,
    libgraphics::ImageLayer*    source
) {

    assert( device != nullptr );
    assert( destination != nullptr );
    assert( source != nullptr );

    if( device == nullptr ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to process SplitTone filter using invalid backend device.";
#endif
        return false;
    }

    if( destination == nullptr ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to process SplitTone filter using invalid destination buffer as target.";
#endif
        return false;
    }

    if( source == nullptr ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to process SplitTone filter using invalid source buffer.";
#endif
        return false;
    }

    libgraphics::fx::operations::splittone(
        device,
        destination,
        source,
        source->size(),
        this->m_Highlights.Values[0],
        this->m_Highlights.Values[1],
        this->m_Highlights.Values[2],
        this->m_Shadows.Values[0],
        this->m_Shadows.Values[1],
        this->m_Shadows.Values[2],
        this->m_Balance
    );

    return true;
}

FilterPreset SplitTone::toPreset() const {
    FilterPreset preset;

    preset.floats()["Balance"].value = this->m_Balance;

    preset.floats()["HighlightsFactor.R"].value = this->m_Highlights.Values[0];
    preset.floats()["HighlightsFactor.G"].value = this->m_Highlights.Values[1];
    preset.floats()["HighlightsFactor.B"].value = this->m_Highlights.Values[2];

    preset.floats()["ShadowsFactor.R"].value = this->m_Shadows.Values[0];
    preset.floats()["ShadowsFactor.G"].value = this->m_Shadows.Values[1];
    preset.floats()["ShadowsFactor.B"].value = this->m_Shadows.Values[2];

    preset.filterName() = this->m_Name;
    preset.name()       = "Current";

    return preset;
}

bool SplitTone::fromPreset( const FilterPreset& preset ) {
    bool success( false );

    if( preset.containsValueWithName( "Balance" ) ) {
        const auto valueIt = preset.floats().find( "Balance" );

        if( valueIt != preset.floats().end() ) {
            success = true;
            this->m_Balance = ( *valueIt ).second.value;
        }
    }

    if( preset.containsValueWithName( "HighlightsFactor.R" ) ) {
        const auto valueIt = preset.floats().find( "HighlightsFactor.R" );

        if( valueIt != preset.floats().end() ) {
            success = true;
            this->m_Highlights.Values[0] = ( *valueIt ).second.value;
        }
    }

    if( preset.containsValueWithName( "HighlightsFactor.G" ) ) {
        const auto valueIt = preset.floats().find( "HighlightsFactor.G" );

        if( valueIt != preset.floats().end() ) {
            success = true;
            this->m_Highlights.Values[1] = ( *valueIt ).second.value;
        }
    }

    if( preset.containsValueWithName( "HighlightsFactor.B" ) ) {
        const auto valueIt = preset.floats().find( "HighlightsFactor.B" );

        if( valueIt != preset.floats().end() ) {
            success = true;
            this->m_Highlights.Values[2] = ( *valueIt ).second.value;
        }
    }

    if( preset.containsValueWithName( "ShadowsFactor.R" ) ) {
        const auto valueIt = preset.floats().find( "ShadowsFactor.R" );

        if( valueIt != preset.floats().end() ) {
            success = true;
            this->m_Shadows.Values[0] = ( *valueIt ).second.value;
        }
    }

    if( preset.containsValueWithName( "ShadowsFactor.G" ) ) {
        const auto valueIt = preset.floats().find( "ShadowsFactor.G" );

        if( valueIt != preset.floats().end() ) {
            success = true;
            this->m_Shadows.Values[1] = ( *valueIt ).second.value;
        }
    }

    if( preset.containsValueWithName( "ShadowsFactor.B" ) ) {
        const auto valueIt = preset.floats().find( "ShadowsFactor.B" );

        if( valueIt != preset.floats().end() ) {
            success = true;
            this->m_Shadows.Values[2] = ( *valueIt ).second.value;
        }
    }

    return success;
}

float& SplitTone::balance() {
    return this->m_Balance;
}

const float& SplitTone::balance() const {
    return this->m_Balance;
}

void SplitTone::setBalance( const float& balance ) {
    this->m_Balance = balance;
}

libgraphics::formats::RGBA32F::t& SplitTone::highlights() {
    return this->m_Highlights;
}

const libgraphics::formats::RGBA32F::t& SplitTone::highlights() const {
    return this->m_Highlights;
}

void SplitTone::setHighlights( const libgraphics::formats::RGBA32F::t& value ) {
    this->m_Highlights = value;
}

libgraphics::formats::RGBA32F::t& SplitTone::shadows() {
    return this->m_Shadows;
}

const libgraphics::formats::RGBA32F::t& SplitTone::shadows() const {
    return this->m_Shadows;
}

void SplitTone::setShadows( const libgraphics::formats::RGBA32F::t& value ) {
    this->m_Shadows = value;
}

Filter* SplitTone::clone() {
    SplitTone* clonedFilter = new SplitTone(
        this->m_Device
    );
    clonedFilter->m_Balance     = m_Balance;
    clonedFilter->m_Device      = m_Device;
    clonedFilter->m_Name        = m_Name;
    clonedFilter->m_Highlights  = m_Highlights;
    clonedFilter->m_Shadows     = m_Shadows;

    return ( Filter* )clonedFilter;
}

}
}
}
