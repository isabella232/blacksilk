#pragma once

#include <vector>
#include <libgraphics/filter.hpp>
#include <libgraphics/curve.hpp>
#include <libgraphics/bezier.hpp>

namespace libgraphics {
namespace fx {
namespace filters {

class Curves : public libgraphics::Filter {
    public:
        explicit Curves( fxapi::ApiBackendDevice* _device );
        virtual ~Curves() {}

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

        virtual Filter* clone();
    protected:
        void updateCurveData( size_t dataLen );

        bool                                    m_ModifiedCurve;
        std::vector<float>                      m_CurveData;
        std::vector<libgraphics::Point32F>      m_CurvePoints;
};

}
}
}
