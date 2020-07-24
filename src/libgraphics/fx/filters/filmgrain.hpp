#pragma once

#include <libgraphics/filter.hpp>

namespace libgraphics {
namespace fx {
namespace filters {

class FilmGrain : public libgraphics::Filter {
    public:
        explicit FilmGrain( fxapi::ApiBackendDevice* _device );
        virtual ~FilmGrain() {}

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

        /// grain image
        bool loadGrainFromData(
            fxapi::ApiBackendDevice* device,
            fxapi::EPixelFormat::t format,
            size_t width,
            size_t height,
            void* data
        );
        void resetGrain();
        void resetGrain(
            fxapi::ApiBackendDevice* device,
            fxapi::EPixelFormat::t format,
            size_t width,
            size_t height
        );
        void resetGrain(
            std::unique_ptr<libgraphics::ImageLayer>& rhs
        );
        std::unique_ptr<libgraphics::ImageLayer>&  grainLayer();
        const std::unique_ptr<libgraphics::ImageLayer>&   grainLayer() const;

        /// curve data
        void assignCurve( const std::vector<libgraphics::Point32F>& points );
        void resetCurve();

        const size_t                    curveLength() const;
        libgraphics::Point32F&          curvePointAtIndex( size_t index );
        const libgraphics::Point32F&    curvePointAtIndex( size_t index ) const;

        const std::vector<libgraphics::Point32F>&   curve() const;
        const std::vector<float>&                   curveData() const;

        void addCurvePoint( const libgraphics::Point32F& curvePoint );
        bool insertCurvePoint( std::vector<libgraphics::Point32F>::iterator where, const libgraphics::Point32F& curvePoint );
        bool removeCurvePoint( std::vector<libgraphics::Point32F>::iterator which );

        void deleteGrainForBackend( int backendId );

        /// mono grain
        bool& monoGrain();
        const bool& monoGrain() const;
        void setMonoGrain( bool value );

        /// grain blur radius
        float& grainBlurRadius();
        const float& grainBlurRadius() const;
        void setGrainBlurRadius( float radius );

        virtual Filter* clone();
    protected:
        void calculateGrainImage();
        void updateCurveData( size_t dataLen );

        bool                                    m_ModifiedCurve;
        std::vector<float>                      m_CurveData;
        std::vector<libgraphics::Point32F>      m_CurvePoints;

        std::unique_ptr<libgraphics::ImageLayer>   m_GrainLayer;
        bool                                            m_MonoGrain;
        float                                           m_GrainBlurRadius;
};

}
}
}
