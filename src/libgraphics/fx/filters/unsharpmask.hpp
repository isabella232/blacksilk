#pragma once

#include <libgraphics/filter.hpp>

namespace libgraphics {
namespace fx {
namespace filters {

class UnsharpMask : public libgraphics::Filter {
    public:
        explicit UnsharpMask( fxapi::ApiBackendDevice* _device );
        virtual ~UnsharpMask() {}

        virtual bool process(
            libgraphics::ImageLayer*    destination,
            libgraphics::ImageLayer*    source
        );
        virtual bool process(
            fxapi::ApiBackendDevice*    device,
            libgraphics::ImageLayer*    destination,
            libgraphics::ImageLayer*    source
        );

        virtual FilterPreset toPreset() const;
        virtual bool fromPreset( const FilterPreset& preset );

        void setStrength( float strength );
        const float& strength() const;
        float& strength();

        void setBlurRadius( float radius );
        const float& blurRadius() const;
        float& blurRadius();

        void setBlurBuffer( const libgraphics::fxapi::EPixelFormat::t format, size_t width, size_t height );
        void setBlurBuffer( const std::shared_ptr<libgraphics::ImageLayer>& blurBuffer );
        const std::shared_ptr<libgraphics::ImageLayer>& blurBuffer() const;

        virtual Filter* clone();
    protected:
        void generateBlurBuffer(
            fxapi::ApiBackendDevice* device, libgraphics::ImageLayer* baseImage, const libgraphics::fxapi::EPixelFormat::t format, size_t width, size_t height, const float& blurRadius
        );

        std::shared_ptr<libgraphics::ImageLayer>   m_BlurBuffer;
        float m_Strength;
        float m_BlurRadius;
};

}
}
}
