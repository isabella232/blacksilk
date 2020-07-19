#pragma once

#include <libgraphics/filter.hpp>

namespace libgraphics {
namespace fx {
namespace filters {

class SplitTone : public libgraphics::Filter {
    public:
        explicit SplitTone( fxapi::ApiBackendDevice* _device );
        virtual ~SplitTone() {}

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

        float& balance();
        const float& balance() const;
        void setBalance( const float& balance );

        libgraphics::formats::RGBA32F::t& highlights();
        const libgraphics::formats::RGBA32F::t& highlights() const;
        void setHighlights( const libgraphics::formats::RGBA32F::t& value );

        libgraphics::formats::RGBA32F::t& shadows();
        const libgraphics::formats::RGBA32F::t& shadows() const;
        void setShadows( const libgraphics::formats::RGBA32F::t& value );

        virtual Filter* clone();
    protected:
        float m_Balance;
        libgraphics::formats::RGBA32F::t m_Highlights;
        libgraphics::formats::RGBA32F::t m_Shadows;
};

}
}
}
