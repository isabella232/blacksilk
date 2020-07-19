#pragma once

#include <libgraphics/filter.hpp>
#include <vector>

namespace libgraphics {
namespace fx {
namespace filters {

/**
    const std::vector< std::tuple< libgraphics::ImageLayer, float, float > >& cascades,
    float threshold
*/

class CascadedSharpen : public libgraphics::Filter {
    public:
        explicit CascadedSharpen( fxapi::ApiBackendDevice* _device );
        virtual ~CascadedSharpen() {}

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

        void setCascadeCount( size_t count );
        size_t cascadeCount() const;

        void setCascadeStrength( size_t index, float strength );
        void setCascadeBlurRadius( size_t index, float radius );

        const float& cascadeStrength( size_t index ) const;
        const float& cascadeBlurRadius( size_t index ) const;

        void deleteBlurBuffersForBackend( int backendId );
        void setCascadeBlurBuffer( size_t index, libgraphics::fxapi::EPixelFormat::t format, size_t width, size_t height );
        void setCascadeBlurBuffer( size_t index, const libcommon::SharedPtr<libgraphics::ImageLayer>& buffer );
        const libcommon::SharedPtr<libgraphics::ImageLayer>& cascadeBlurBuffer( size_t index );

        void generateCascades( const std::vector<float>& cascades,
                               libgraphics::fxapi::ApiBackendDevice* backend,
                               libgraphics::ImageLayer* baseImage );

        void setThreshold( float threshold );
        const float& threshold() const;
        float& threshold();

        void updateCascades();

        virtual Filter* clone();
    protected:
        void generateBlurBuffer(
            size_t index, fxapi::ApiBackendDevice* device, libgraphics::ImageLayer* baseImage, const libgraphics::fxapi::EPixelFormat::t format, size_t width, size_t height, const float& blurRadius
        );

        struct CascadeEntry {
            float   blurRadius;
            float   strength;
            libcommon::SharedPtr<libgraphics::ImageLayer> buffer;

            CascadeEntry() : blurRadius( 1.0f ), strength( 1.0f ) {}
        };
        std::vector<CascadeEntry>   m_Cascades;
        bool                        m_ShouldUpdateCascades;
        float                       m_Threshold;
};


}
}
}
