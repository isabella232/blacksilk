#pragma once

#include <libgraphics/filter.hpp>

namespace libgraphics {
namespace fx {
namespace filters {

class Vignette : public libgraphics::Filter {
    public:
        explicit Vignette( fxapi::ApiBackendDevice* _device );
        virtual ~Vignette() {}

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

        float& strength();
        const float& strength() const;
        void setStrength( const float& value );

        float& radius();
        const float& radius() const;
        void setRadius( const float& value );

        libgraphics::Point32F& center();
        const libgraphics::Point32F& center() const;
        void setCenter( const libgraphics::Point32F center );

        virtual Filter* clone();
    protected:
        float m_Strength;
        float m_Radius;
        libgraphics::Point32F m_Center;
};

}
}
}
