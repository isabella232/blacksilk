
#include <libgraphics/fx/filters/curves.hpp>
#include <libgraphics/fx/operations/basic.hpp>
#include <sstream>

#include <QDebug>

namespace libgraphics {
namespace fx {
namespace filters {

Curves::Curves( fxapi::ApiBackendDevice* _device ) : libgraphics::Filter( "Curves", _device ), m_ModifiedCurve( true ) {}

bool Curves::process(
    libgraphics::ImageLayer*    destination,
    libgraphics::ImageLayer*    source
) {
    return this->process(
               this->backend(),
               destination,
               source
           );
}

bool Curves::process(
    fxapi::ApiBackendDevice*    /*device*/,
    libgraphics::ImageLayer*    destination,
    libgraphics::ImageLayer*    source
) {
    // assert(device);
    assert( destination );
    assert( source );

    if( destination == nullptr || source == nullptr ) {
        return false;
    }

    const size_t pixelMax = libgraphics::fxapi::EPixelFormat::getPixelMax( source->format() );

    if( this->m_ModifiedCurve || ( this->m_CurveData.size() != ( pixelMax + 1 ) ) ) {
        this->m_ModifiedCurve = true; /** ensure its actually set to true **/
        this->updateCurveData(
            pixelMax + 1
        );
    }

    libgraphics::fx::operations::adjustBrightness(
        destination,
        source,
        source->size(),
        this->m_CurveData.data(),
        this->m_CurveData.size()
    );

    return true;
}


FilterPreset Curves::toPreset() const {
    FilterPreset preset;

    // TODO: Needed?
    preset.ints()["Length"].value = this->m_CurveData.size();

    size_t index( 0 );

    for( auto it = this->m_CurvePoints.begin(); it != this->m_CurvePoints.end(); ++it ) {
        std::stringstream ss;
        ss << "Point";
        ss << index;

        preset.points()[ss.str()].value = ( *it );

        ++index;
    }

    preset.filterName() = this->m_Name;
    preset.name()       = "Current";


    return preset;
}

bool Curves::fromPreset( const FilterPreset& preset ) {

    // TODO "Length" needed in preset?
    if( !preset.points().empty() ) {
        this->m_CurvePoints.clear();
        const auto points = preset.points();

        this->m_CurvePoints.resize( points.size() );
        std::transform(
            points.begin(),
            points.end(),
            m_CurvePoints.begin(),
        []( const std::pair<std::string, FilterPresetValuePoint>& p ) {
            return p.second.value;
        }
        );
        std::sort(
            this->m_CurvePoints.begin(),
            this->m_CurvePoints.end(),
        []( const Point32F & first, const Point32F & second ) {
            return ( first.x < second.x );
        }
        );

        this->m_ModifiedCurve = true;

        return true;
    }

    return false;
}

void Curves::assignCurve( const std::vector<libgraphics::Point32F>& points ) {
    this->m_ModifiedCurve = true;
    this->m_CurvePoints   = points;
}

void Curves::resetCurve() {
    this->m_ModifiedCurve = true;

    this->m_CurveData.clear();
    this->m_CurvePoints.clear();
}

const size_t                    Curves::curveLength() const {
    return this->m_CurvePoints.size();
}

libgraphics::Point32F&          Curves::curvePointAtIndex( size_t index ) {
    assert( this->m_CurvePoints.size() >= index );

    return *( this->m_CurvePoints.begin() + index );
}

const libgraphics::Point32F&    Curves::curvePointAtIndex( size_t index ) const {
    assert( this->m_CurvePoints.size() >= index );

    return *( this->m_CurvePoints.begin() + index );
}

const std::vector<libgraphics::Point32F>&   Curves::curve() const {
    return this->m_CurvePoints;
}

const std::vector<float>&                   Curves::curveData() const {
    return this->m_CurveData;
}

void Curves::addCurvePoint( const libgraphics::Point32F& curvePoint ) {
    this->m_ModifiedCurve = true;

    this->m_CurvePoints.push_back( curvePoint );
    std::sort(
        this->m_CurvePoints.begin(),
        this->m_CurvePoints.end(),
    []( const Point32F & first, const Point32F & second ) {
        return ( first.x < second.x );
    }
    );
}

bool Curves::insertCurvePoint( std::vector<libgraphics::Point32F>::iterator where, const libgraphics::Point32F& curvePoint ) {
    assert( where != m_CurvePoints.end() );

    if( where != m_CurvePoints.end() ) {
        m_CurvePoints.insert( where, curvePoint );
        m_ModifiedCurve = true;

        return true;
    }

    return false;
}

bool Curves::removeCurvePoint( std::vector<libgraphics::Point32F>::iterator which ) {
    assert( which != m_CurvePoints.end() );

    if( which != m_CurvePoints.end() ) {
        m_CurvePoints.erase( which );
        m_ModifiedCurve = true;

        return true;
    }

    return false;
}

void Curves::updateCurveData( size_t dataLen ) {
    assert( !m_CurvePoints.empty() );
    assert( dataLen != 0 );

    if( this->m_ModifiedCurve ) {
        this->m_ModifiedCurve = false;

        this->m_CurveData.clear();

        std::vector<Point32F> plottedCurve;

        if( m_CurvePoints.size() == 2 ) {
            plottedCurve = libgraphics::calcLinear( this->m_CurvePoints, dataLen );
        } else {
            plottedCurve = libgraphics::calcBezier( this->m_CurvePoints, dataLen );
        }

        assert( plottedCurve.size() == dataLen );

        this->m_CurveData.resize( dataLen );

        std::transform(
            plottedCurve.begin(),
            plottedCurve.end(),
            m_CurveData.begin(),
        []( const libgraphics::Point32F & p ) {
            return p.y;
        }
        );
    }
}


Filter* Curves::clone() {
    Curves* clonedFilter = new Curves(
        this->m_Device
    );
    clonedFilter->m_Device      = m_Device;
    clonedFilter->m_Name        = m_Name;

    clonedFilter->m_CurveData       = m_CurveData;
    clonedFilter->m_CurvePoints     = m_CurvePoints;
    clonedFilter->m_ModifiedCurve   = m_ModifiedCurve;

    return ( Filter* )clonedFilter;
}

}
}
}
