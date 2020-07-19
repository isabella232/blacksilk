#pragma once

#include <libgraphics/filter.hpp>

namespace libgraphics {
namespace fx {
namespace filters {

class BWAdaptiveMixer : public libgraphics::Filter {
    public:
        explicit BWAdaptiveMixer( fxapi::ApiBackendDevice* _device );
        virtual ~BWAdaptiveMixer() {}

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

        float&  balance();
        const float& balance() const;
        void setBalance( const float& value );

        libgraphics::formats::RGB32F::t&    highlightWeights();
        const libgraphics::formats::RGB32F::t&  highlightWeights() const;
        void setHighlightWeights( const libgraphics::formats::RGB32F::t& value );

        libgraphics::formats::RGB32F::t&    shadowWeights();
        const libgraphics::formats::RGB32F::t&  shadowWeights() const;
        void setShadowWeights( const libgraphics::formats::RGB32F::t& value );

        virtual Filter* clone();
    protected:
        float                               m_Balance;
        libgraphics::formats::RGB32F::t     m_HighlightWeights;
        libgraphics::formats::RGB32F::t     m_ShadowWeights;
};

}
}
}
