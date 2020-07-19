#pragma once

#include <libgraphics/filter.hpp>

namespace libgraphics {
namespace fx {
namespace filters {

class BWMixer : public libgraphics::Filter {
    public:
        explicit BWMixer( fxapi::ApiBackendDevice* _device );
        virtual ~BWMixer() {}

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

        float& redSensitivity();
        const float& redSensitivity() const;
        void setRedSensitivity( const float& val );

        float& greenSensitivity();
        const float& greenSensitivity() const;
        void setGreenSensitivity( const float& val );

        float& blueSensitivity();
        const float& blueSensitivity() const;
        void setBlueSensitivity( const float& val );

        void resetToUniform();
        void reset( float r, float g, float b );

        virtual Filter* clone();
    protected:
        float m_RedSensitivity;
        float m_GreenSensitivity;
        float m_BlueSensitivity;
};

}
}
}
