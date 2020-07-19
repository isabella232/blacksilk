
#include <QDebug>
#include <QElapsedTimer>
#include <libgraphics/fx/operations/basic.hpp>
#include <libgraphics/fx/operations/complex.hpp>
#include <libgraphics/fx/filters/bwadaptivemixer.hpp>

#include <gl/glew.h>

namespace libgraphics {
namespace fx {
namespace filters {

BWAdaptiveMixer::BWAdaptiveMixer( fxapi::ApiBackendDevice* _device ) : libgraphics::Filter( "BWAdaptiveMixer", _device ), m_Balance( 1.0f ) {}

bool BWAdaptiveMixer::process(
    libgraphics::ImageLayer*    destination,
    libgraphics::ImageLayer*    source
) {
    return process(
               this->backend(),
               destination,
               source
           );
}

bool BWAdaptiveMixer::process(
    fxapi::ApiBackendDevice*    device,
    libgraphics::ImageLayer*    destination,
    libgraphics::ImageLayer*    source
) {
    assert( destination != nullptr );
    assert( source != nullptr );
    assert( device != nullptr );

    if( device == nullptr ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to process BWAdaptiveMixer filter using invalid backend device.";
#endif
        return false;
    }

    if( destination == nullptr ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to process BWAdaptiveMixer filter using invalid destination buffer as target.";
#endif
        return false;
    }

    if( source == nullptr ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to process BWAdaptiveMixer filter using invalid source buffer.";
#endif
        return false;
    }

    libgraphics::fx::operations::adaptiveBWMixer(
        device,
        destination,
        source,
        source->size(),
        this->m_HighlightWeights.Values[0],
        this->m_HighlightWeights.Values[1],
        this->m_HighlightWeights.Values[2],
        this->m_ShadowWeights.Values[0],
        this->m_ShadowWeights.Values[1],
        this->m_ShadowWeights.Values[2],
        this->m_Balance
    );

    return true;
}

FilterPreset BWAdaptiveMixer::toPreset() const {
    FilterPreset preset;

    preset.floats()["HighlightWeights.R"].value = this->m_HighlightWeights.Values[0];
    preset.floats()["HighlightWeights.G"].value = this->m_HighlightWeights.Values[1];
    preset.floats()["HighlightWeights.B"].value = this->m_HighlightWeights.Values[2];

    preset.floats()["ShadowWeights.R"].value = this->m_ShadowWeights.Values[0];
    preset.floats()["ShadowWeights.G"].value = this->m_ShadowWeights.Values[1];
    preset.floats()["ShadowWeights.B"].value = this->m_ShadowWeights.Values[2];

    preset.floats()["Balance"].value = this->m_Balance;

    preset.filterName() = this->m_Name;
    preset.name()       = "Current";

    return preset;
}

bool BWAdaptiveMixer::fromPreset( const FilterPreset& preset ) {
    bool success( false );


    if( preset.containsValueWithName( "HighlightWeights.R" ) ) {
        const auto valueIt = preset.floats().find( "HighlightWeights.R" );

        if( valueIt != preset.floats().end() ) {
            success = true;
            this->m_HighlightWeights.Values[0] = ( *valueIt ).second.value;
        }
    }

    if( preset.containsValueWithName( "HighlightWeights.G" ) ) {
        const auto valueIt = preset.floats().find( "HighlightWeights.G" );

        if( valueIt != preset.floats().end() ) {
            success = true;
            this->m_HighlightWeights.Values[1] = ( *valueIt ).second.value;
        }
    }

    if( preset.containsValueWithName( "HighlightWeights.B" ) ) {
        const auto valueIt = preset.floats().find( "HighlightWeights.B" );

        if( valueIt != preset.floats().end() ) {
            success = true;
            this->m_HighlightWeights.Values[2] = ( *valueIt ).second.value;
        }
    }


    if( preset.containsValueWithName( "ShadowWeights.R" ) ) {
        const auto valueIt = preset.floats().find( "ShadowWeights.R" );

        if( valueIt != preset.floats().end() ) {
            success = true;
            this->m_ShadowWeights.Values[0] = ( *valueIt ).second.value;
        }
    }

    if( preset.containsValueWithName( "ShadowWeights.G" ) ) {
        const auto valueIt = preset.floats().find( "ShadowWeights.G" );

        if( valueIt != preset.floats().end() ) {
            success = true;
            this->m_ShadowWeights.Values[1] = ( *valueIt ).second.value;
        }
    }

    if( preset.containsValueWithName( "ShadowWeights.B" ) ) {
        const auto valueIt = preset.floats().find( "ShadowWeights.B" );

        if( valueIt != preset.floats().end() ) {
            success = true;
            this->m_ShadowWeights.Values[2] = ( *valueIt ).second.value;
        }
    }

    if( preset.containsValueWithName( "Balance" ) ) {
        const auto valueIt = preset.floats().find( "Balance" );

        if( valueIt != preset.floats().end() ) {
            success = true;
            this->m_Balance = ( *valueIt ).second.value;
        }
    }

    return success;
}


float&  BWAdaptiveMixer::balance() {
    return this->m_Balance;
}

const float& BWAdaptiveMixer::balance() const {
    return this->m_Balance;
}

void BWAdaptiveMixer::setBalance( const float& value ) {
    this->m_Balance = value;
}

libgraphics::formats::RGB32F::t&    BWAdaptiveMixer::highlightWeights() {
    return this->m_HighlightWeights;
}

const libgraphics::formats::RGB32F::t&  BWAdaptiveMixer::highlightWeights() const {
    return this->m_HighlightWeights;
}

void BWAdaptiveMixer::setHighlightWeights( const libgraphics::formats::RGB32F::t& value ) {
    this->m_HighlightWeights = value;
}

libgraphics::formats::RGB32F::t&    BWAdaptiveMixer::shadowWeights() {
    return this->m_ShadowWeights;
}

const libgraphics::formats::RGB32F::t&  BWAdaptiveMixer::shadowWeights() const {
    return this->m_ShadowWeights;
}

void BWAdaptiveMixer::setShadowWeights( const libgraphics::formats::RGB32F::t& value ) {
    this->m_ShadowWeights = value;
}

Filter* BWAdaptiveMixer::clone() {
    BWAdaptiveMixer* clonedFilter = new BWAdaptiveMixer(
        this->m_Device
    );
    clonedFilter->m_Balance     = m_Balance;
    clonedFilter->m_Device      = m_Device;
    clonedFilter->m_HighlightWeights    = m_HighlightWeights;
    clonedFilter->m_Name                = m_Name;
    clonedFilter->m_ShadowWeights       = m_ShadowWeights;

    return ( Filter* )clonedFilter;
}

}
}
}
