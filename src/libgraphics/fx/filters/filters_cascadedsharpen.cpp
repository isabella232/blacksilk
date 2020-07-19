
#include <QDebug>
#include <sstream>
#include <libgraphics/fx/operations/basic.hpp>
#include <libgraphics/fx/operations/complex.hpp>
#include <libgraphics/fx/filters/cascadedsharpen.hpp>

namespace libgraphics {
namespace fx {
namespace filters {

CascadedSharpen::CascadedSharpen( fxapi::ApiBackendDevice* _device ) : Filter( "CascadedSharpen", _device ), m_Threshold( 0.0f ), m_ShouldUpdateCascades( true ) {}

void CascadedSharpen::generateBlurBuffer(
    size_t index, fxapi::ApiBackendDevice* device, libgraphics::ImageLayer* baseImage, const libgraphics::fxapi::EPixelFormat::t format, size_t width, size_t height, const float& blurRadius
) {
    assert( width > 0 );
    assert( height > 0 );
    assert( format != libgraphics::fxapi::EPixelFormat::Empty );
    assert( blurRadius >= 0.005f );

    assert( this->m_Cascades.size() > index );

    if( this->m_Cascades.size() < index ) {
        return;
    }

    if( ( this->m_Cascades[index].buffer.empty() ) || ( !this->m_Cascades[index].buffer->containsDataForBackend( device->backendId() ) ) ) {
        this->m_Cascades[index].buffer.reset(
            new libgraphics::ImageLayer(
                device
            )
        );
    }

    const auto successfullyResetted = this->m_Cascades[index].buffer->reset(
                                          format,
                                          width,
                                          height
                                      );
    assert( successfullyResetted );

    if( !successfullyResetted ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to reset blur buffer.";
#endif
        return;
    }

    libgraphics::fx::operations::gaussianBlur(
        device,
        this->m_Cascades[index].buffer.get(),
        baseImage,
        baseImage->size(),
        blurRadius
    );
}

bool CascadedSharpen::process(
    libgraphics::ImageLayer*    destination,
    libgraphics::ImageLayer*    source
) {
    assert( destination );
    assert( source );

    return process(
               this->backend(),
               destination,
               source
           );
}

void CascadedSharpen::generateCascades( const std::vector<float>& cascades,
                                        libgraphics::fxapi::ApiBackendDevice* backend,
                                        libgraphics::ImageLayer* baseImage ) {
    this->setCascadeCount(
        cascades.size()
    );

    size_t index( 0 );

    for( auto it = cascades.begin(); it != cascades.end(); ++it ) {
        this->m_Cascades[index].blurRadius = ( *it );

        this->generateBlurBuffer(
            index,
            backend,
            baseImage,
            baseImage->format(),
            baseImage->width(),
            baseImage->height(),
            this->m_Cascades[index].blurRadius
        );
        ++index;
    }
}

void CascadedSharpen::updateCascades() {
    m_ShouldUpdateCascades = true;
}

bool CascadedSharpen::process(
    fxapi::ApiBackendDevice*    device,
    libgraphics::ImageLayer*    destination,
    libgraphics::ImageLayer*    source
) {
    assert( device != nullptr );
    assert( destination != nullptr );
    assert( source != nullptr );

    std::vector< std::tuple<ImageLayer*, float, float> >  cascades;
    bool    didUpdateCascades( false );

    for( size_t i = 0; this->m_Cascades.size() > i; ++i ) {

        bool                     incompatibleCascades( false );
        libgraphics::ImageLayer* currentCascadeBuffer( this->m_Cascades[i].buffer.get() );

        if( currentCascadeBuffer != nullptr ) {
            if( source->format() != currentCascadeBuffer->format() ) {
                incompatibleCascades = true;
            } else if( source->size() != currentCascadeBuffer->size() ) {
                incompatibleCascades = true;
            } else if( !currentCascadeBuffer->containsDataForBackend( device->backendId() ) ) {
                incompatibleCascades = true;
            }
        } else {
            incompatibleCascades = true;
        }

        if( m_ShouldUpdateCascades || incompatibleCascades ) {
            this->generateBlurBuffer(
                i,
                device,
                source,
                source->format(),
                source->width(),
                source->height(),
                this->m_Cascades[i].blurRadius
            );
            didUpdateCascades = true;
        }

        cascades.push_back(
            std::make_tuple( this->m_Cascades[i].buffer.get(), this->m_Cascades[i].blurRadius, this->m_Cascades[i].strength )
        );
    }

    if( didUpdateCascades ) {
        this->m_ShouldUpdateCascades = false;
    }

    libgraphics::fx::operations::cascadedSharpen(
        device,
        destination,
        source,
        source->size(),
        cascades,
        this->m_Threshold
    );

    return true;
}

void CascadedSharpen::setThreshold( float threshold ) {
    this->m_Threshold = threshold;
}

const float& CascadedSharpen::threshold() const {
    return this->m_Threshold;
}

float& CascadedSharpen::threshold() {
    return this->m_Threshold;
}

FilterPreset CascadedSharpen::toPreset() const {
    FilterPreset preset;

    preset.filterName() = this->name();
    preset.ints()["NumberOfCascades"].value = this->m_Cascades.size();
    preset.floats()["Threshold"].value = this->m_Threshold;

    for( size_t i = 0; this->m_Cascades.size() > i; ++i ) {
        std::stringstream ss;
        ss << i;

        const std::string valStrength( "Strength" + ss.str() );
        preset.floats()[valStrength].value = this->m_Cascades[i].strength;

        const std::string valRadius( "BlurRadius" + ss.str() );
        preset.floats()[valRadius].value = this->m_Cascades[i].blurRadius;
    }

    preset.filterName() = this->m_Name;
    preset.name()       = "Current";

    return preset;
}

bool CascadedSharpen::fromPreset( const FilterPreset& preset ) {

    bool foundCascadeCount( false );

    if( preset.containsValueWithName( "NumberOfCascades" ) ) {
        const auto it = preset.ints().find( "NumberOfCascades" );

        if( it != preset.ints().end() ) {
            this->setCascadeCount(
                ( *it ).second.value
            );
            foundCascadeCount = true;
        }
    }

    size_t numberOfFoundCascades( 0 );

    if( foundCascadeCount ) {
        for( size_t i = 0; this->m_Cascades.size() > i; ++i ) {
            std::stringstream ss;
            ss << i;

            const std::string valStrength( "Strength" + ss.str() );
            const std::string valRadius( "BlurRadius" + ss.str() );

            const auto itStrength = preset.floats().find( valStrength );

            if( itStrength != preset.floats().end() ) {
                this->m_Cascades[i].strength = ( *itStrength ).second.value;
            } else {
                break;
            }

            const auto itRadius = preset.floats().find( valRadius );

            if( itRadius != preset.floats().end() ) {
                this->m_Cascades[i].blurRadius = ( *itRadius ).second.value;
            } else {
                break;
            }

            ++numberOfFoundCascades;
        }
    }

    const auto itThreshold = preset.floats().find( "Threshold" );

    if( itThreshold != preset.floats().end() ) {
        this->m_Threshold = ( *itThreshold ).second.value;
    }

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( numberOfFoundCascades != this->m_Cascades.size() ) {
        qDebug() << "Preset is invalid. Number of cascade info entries not sufficient.";
    }

#endif

    return ( numberOfFoundCascades == this->m_Cascades.size() );
}

void CascadedSharpen::setCascadeCount( size_t count ) {
    this->m_Cascades.resize( count );
}

size_t CascadedSharpen::cascadeCount() const {
    return this->m_Cascades.size();
}

void CascadedSharpen::setCascadeStrength( size_t index, float strength ) {
    assert( this->m_Cascades.size() > index );
    this->m_Cascades[index].strength = strength;
}

void CascadedSharpen::setCascadeBlurRadius( size_t index, float radius ) {
    assert( this->m_Cascades.size() > index );

    if( this->m_Cascades[index].blurRadius != radius ) {
        this->m_Cascades[index].buffer.reset();
    }

    this->m_Cascades[index].blurRadius = radius;
}

const float& CascadedSharpen::cascadeStrength( size_t index ) const {
    assert( this->m_Cascades.size() > index );
    return this->m_Cascades[index].strength;
}

const float& CascadedSharpen::cascadeBlurRadius( size_t index ) const {
    assert( this->m_Cascades.size() > index );
    return this->m_Cascades[index].blurRadius;
}

void CascadedSharpen::deleteBlurBuffersForBackend( int backendId ) {
    for( auto it = this->m_Cascades.begin(); it != this->m_Cascades.end(); ++it ) {
        if( !( *it ).buffer.empty() ) {
            ( *it ).buffer->deleteDataForBackend( backendId );
        }
    }
}

void CascadedSharpen::setCascadeBlurBuffer( size_t index, libgraphics::fxapi::EPixelFormat::t format, size_t width, size_t height ) {
    assert( this->m_Cascades.size() > index );

    const auto successfullyResetted = this->m_Cascades[index].buffer->reset(
                                          format,
                                          width,
                                          height
                                      );
    assert( successfullyResetted );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

    if( !successfullyResetted ) {
        qDebug() << "Failed to reset blur buffer.";
    }

#endif
}

void CascadedSharpen::setCascadeBlurBuffer( size_t index, const libcommon::SharedPtr<libgraphics::ImageLayer>& buffer ) {
    assert( this->m_Cascades.size() > index );

    this->m_Cascades[index].buffer = buffer;
}

const libcommon::SharedPtr<libgraphics::ImageLayer>& CascadedSharpen::cascadeBlurBuffer( size_t index ) {
    return this->m_Cascades[index].buffer;
}

Filter* CascadedSharpen::clone() {
    CascadedSharpen* clonedFilter = new CascadedSharpen(
        this->m_Device
    );
    clonedFilter->m_Device      = m_Device;
    clonedFilter->m_Name        = m_Name;

    clonedFilter->m_Cascades                = m_Cascades;
    clonedFilter->m_ShouldUpdateCascades    = m_ShouldUpdateCascades;
    clonedFilter->m_Threshold               = m_Threshold;

    return ( Filter* )clonedFilter;
}


}
}
}
