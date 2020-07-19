
#include <QDebug>
#include <libgraphics/backend/common/formats.hpp>
#include <libgraphics/fx/filters/unsharpmask.hpp>
#include <libgraphics/fx/operations/basic.hpp>
#include <libgraphics/fx/operations/complex.hpp>

namespace libgraphics {
namespace fx {
namespace filters {

UnsharpMask::UnsharpMask( fxapi::ApiBackendDevice* _device )  : Filter( "UnsharpMask", _device ), m_BlurRadius( 1.0f ), m_Strength( 1.0f ) {}

bool UnsharpMask::process(
    libgraphics::ImageLayer*    destination,
    libgraphics::ImageLayer*    source
) {
    assert( destination != nullptr );
    assert( source != nullptr );

    return this->process(
               this->backend(),
               destination,
               source
           );
}

void UnsharpMask::generateBlurBuffer(
    fxapi::ApiBackendDevice* device, libgraphics::ImageLayer* baseImage, const libgraphics::fxapi::EPixelFormat::t format, size_t width, size_t height, const float& blurRadius
) {
    assert( width > 0 );
    assert( height > 0 );
    assert( format != libgraphics::fxapi::EPixelFormat::Empty );
    assert( blurRadius >= 0.005f );

    if( this->m_BlurBuffer.empty() ) {
        this->m_BlurBuffer.reset(
            new libgraphics::ImageLayer(
                device
            )
        );
    }

    const auto successfullyResetted = this->m_BlurBuffer->reset(
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
        this->m_BlurBuffer.get(),
        baseImage,
        baseImage->size(),
        blurRadius
    );
}

bool UnsharpMask::process(
    fxapi::ApiBackendDevice*    device,
    libgraphics::ImageLayer*    destination,
    libgraphics::ImageLayer*    source
) {
    assert( device );
    assert( destination );
    assert( source );

    /// check for format
    assert( destination->format() == source->format() );

    if( source->format() != destination->format() ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to process UnsharpMask using two incompatible ImageLayer instances.";
#endif
        return false;
    }

    /// check for dimensions
    assert( source->size() == destination->size() );

    if( source->size() != destination->size() ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() <<  "Failed to process UnsharpMask using two ImageLayer instances with incompatible dimensions.";
#endif
        return false;
    }

    /// generate blur buffer
    generateBlurBuffer(
        device,
        source,
        destination->format(),
        destination->width(),
        destination->height(),
        this->m_BlurRadius
    );

    assert( !this->m_BlurBuffer.empty() );

    if( this->m_BlurBuffer.empty() ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to create blurred base image for unsharp mask.";
#endif
        return false;
    }

    /// run the filter
    libcommon::ScopedPtr<libgraphics::ImageLayer>   baseLayer(
        new libgraphics::ImageLayer(
            device
        )
    );
    libcommon::ScopedPtr<libgraphics::ImageLayer>   sharpMask(
        new libgraphics::ImageLayer(
            device
        )
    );

    if( !baseLayer->reset( destination->format(), destination->width(), destination->height() ) ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to create base image layer.";
#endif
        return false;
    }

    if( !sharpMask->reset( destination->format(), destination->width(), destination->height() ) ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to create intermediate destination image layer.";
#endif
        return false;
    }

    libgraphics::fx::operations::subtract(
        sharpMask.get(),
        this->m_BlurBuffer.get(),
        source,
        baseLayer->size()
    );
    libgraphics::fx::operations::multiply(
        baseLayer.get(),
        sharpMask.get(),
        sharpMask->size(),
        this->m_Strength
    );
    libgraphics::fx::operations::subtract(
        destination,
        source,
        baseLayer.get(),
        source->size()
    );

    return true;
}

FilterPreset UnsharpMask::toPreset() const {
    FilterPreset preset;

    preset.floats()["Strength"].value = this->m_Strength;
    preset.floats()["BlurRadius"].value = this->m_BlurRadius;

    preset.filterName() = this->m_Name;
    preset.name()       = "Current";

    return preset;
}

bool UnsharpMask::fromPreset( const FilterPreset& preset ) {
    bool found( false );

    if( preset.containsValueWithName( "Strength" ) ) {
        const auto it = preset.floats().find( "Strength" );

        if( it != preset.floats().end() ) {
            this->m_Strength = ( *it ).second.value;
            found = true;
        }
    }

    if( preset.containsValueWithName( "BlurRadius" ) ) {
        const auto it = preset.floats().find( "BlurRadius" );

        if( it != preset.floats().end() ) {
            this->m_BlurRadius = ( *it ).second.value;
            found = true;
        }
    }

    return found;
}

void UnsharpMask::setStrength( float strength ) {
    this->m_Strength = strength;
}

const float& UnsharpMask::strength() const {
    return this->m_Strength;
}

float& UnsharpMask::strength() {
    return this->m_Strength;
}

void UnsharpMask::setBlurRadius( float radius ) {
    if( this->m_BlurRadius != radius ) {
        this->m_BlurBuffer.reset();
    }

    this->m_BlurRadius = radius;
}

const float& UnsharpMask::blurRadius() const {
    return this->m_BlurRadius;
}

float& UnsharpMask::blurRadius() {
    return this->m_BlurRadius;
}

void UnsharpMask::setBlurBuffer( const libgraphics::fxapi::EPixelFormat::t format, size_t width, size_t height ) {
    const auto success = this->m_BlurBuffer->reset( format, width, height );

    assert( success );
    ( void )success;
}

void UnsharpMask::setBlurBuffer( const libcommon::SharedPtr<libgraphics::ImageLayer>& blurBuffer ) {
    this->m_BlurBuffer = blurBuffer;
}

const libcommon::SharedPtr<libgraphics::ImageLayer>& UnsharpMask::blurBuffer() const {
    return this->m_BlurBuffer;
}

Filter* UnsharpMask::clone() {
    UnsharpMask* clonedFilter = new UnsharpMask(
        this->m_Device
    );
    clonedFilter->m_Device      = m_Device;
    clonedFilter->m_Name        = m_Name;

    clonedFilter->m_BlurRadius  = m_BlurRadius;
    clonedFilter->m_Strength    = m_Strength;

    return ( Filter* )clonedFilter;
}

}
}
}
