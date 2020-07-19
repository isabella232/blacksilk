#pragma once

#include <vector>

#include <libgraphics/base.hpp>
#include <libgraphics/bitmap.hpp>
#include <libgraphics/curve.hpp>

namespace libgraphics {

/**
    \fn linearInterpolate
    \since 1.0
    \brief Interpolates between Point a and Point b
        linearly.
*/
template < class _t_any >
libgraphics::Point<_t_any>  linearInterpolate(
    const libgraphics::Point<_t_any>&   a,
    const libgraphics::Point<_t_any>&   b,
    const float& t
) {
    assert( t <= 1.0f );

    const libgraphics::Point<_t_any>&   bigger(
        ( a.y > b.y ) ? a : b
    );
    const libgraphics::Point<_t_any>&   smaller(
        ( a.y < b.y ) ? a : b
    );

    const _t_any  dX = bigger.x - smaller.x;
    const _t_any  dY = bigger.y - smaller.y;

    return libgraphics::Point<_t_any>(
               a.x + ( dX * t ),
               b.y + ( dY * t )
           );
}
template < class _t_any >
_t_any  linearInterpolate(
    const _t_any&   a,
    const _t_any&   b,
    const float&    t
) {
    assert( t <= 1.0f );

    return ( a + ( ( b - a ) * t ) );
}
template < class _t_index, class _t_any >
libgraphics::CurvePoint<_t_index, _t_any>    linearInterpolate(
    const libgraphics::CurvePoint<_t_index, _t_any>&  a,
    const libgraphics::CurvePoint<_t_index, _t_any>&  b,
    float t
) {
    assert( t <= 1.0f );

    typedef CurvePoint<_t_index, _t_any>             CurvePointType;
    typedef typename CurvePointType::IndexType      IndexType;
    typedef typename CurvePointType::ValueType      ValueType;

    const IndexType indexDiff   = std::max( a.index, b.index ) - std::min( a.index, b.index );
    const ValueType valueDiff   = std::max( a.value, b.value ) - std::max( a.value, b.value );

    return CurvePointType(
               a.index + ( indexDiff * t ),
               a.value + ( valueDiff * t )
           );
}

/**
    \fn linearBezier
    \since 1.0
    \brief Calculates the linear bezier curve between
        a and b at index f.
*/
template < class _t_any >
libgraphics::Point<_t_any>  linearBezier(
    const libgraphics::Point<_t_any>&   a,
    const libgraphics::Point<_t_any>&   b,
    const float& t
) {
    assert( t <= 1.0f );

    return linearInterpolate<_t_any>(
               a,
               b,
               t
           );
}
template < class _t_index, class _t_any >
libgraphics::CurvePoint<_t_index, _t_any>    linearBezier(
    const libgraphics::CurvePoint<_t_index, _t_any>&  a,
    const libgraphics::CurvePoint<_t_index, _t_any>&  b,
    const float& t
) {
    assert( t <= 1.0f );

    return linearInterpolate<_t_index, _t_any>(
               a,
               b,
               t
           );
}

/**
    \fn quadraticBezier
    \since 1.0
    \brief Calculates a quadratic bezier curve
        interpolation.
*/
template < class _t_any >
libgraphics::Point<_t_any> quadraticBezier(
    const libgraphics::Point<_t_any>& a,
    const libgraphics::Point<_t_any>& b,
    const float& t
) {
    assert( t <= 1.0f );

    const float negated = 1.0f - t;
    return negated * ( ( a * negated ) + ( t * b ) ) + t * ( ( negated * a ) + ( t * b ) );
}

template < class _t_index, class _t_any >
libgraphics::CurvePoint<_t_index, _t_any>    quadraticBezier(
    const libgraphics::CurvePoint<_t_index, _t_any>&  a,
    const libgraphics::CurvePoint<_t_index, _t_any>&  b,
    const float& t
) {
    assert( t <= 1.0f );

    libgraphics::CurvePoint<_t_index, _t_any>    ret;

    const float negated = 1.0f - t;
    ret.index = negated * ( ( a.index * negated ) + ( t * b.index ) ) + t * ( ( negated * a.index ) + ( t * b.index ) );
    ret.value = negated * ( ( a.value * negated ) + ( t * b.value ) ) + t * ( ( negated * a.value ) + ( t * b.value ) );

    return ret;
}

/**
    \fn cubicBezier
    \since 1.0
    \brief Calculates the cubic bezier curve
        interpolation.
*/
template < class _t_any >
libgraphics::Point<_t_any>  cubicBezier(
    const libgraphics::Point<_t_any>& p0,
    const libgraphics::Point<_t_any>& p1,
    const libgraphics::Point<_t_any>& p2,
    const libgraphics::Point<_t_any>& p3,
    const float& t
) {
    const float negated = 1.0f - t;
    return p0 * pow( negated, 3 ) + ( p1 * 3 * pow( negated, 2 ) * t ) + ( p2 * 3 * negated * pow( t, 2 ) ) + ( p3 * std::pow( t, 3 ) );
}

template < class _t_index, class _t_any >
libgraphics::CurvePoint<_t_index, _t_any>  cubicBezier(
    const libgraphics::CurvePoint<_t_index, _t_any>& p0,
    const libgraphics::CurvePoint<_t_index, _t_any>& p1,
    const libgraphics::CurvePoint<_t_index, _t_any>& p2,
    const libgraphics::CurvePoint<_t_index, _t_any>& p3,
    const float& t
) {
    const float negated = 1.0f - t;
    libgraphics::CurvePoint<_t_index, _t_any> ret;

    ret.value = p0.value * pow( negated, 3 ) + ( 3 * pow( negated, 2 ) * t * p1.value ) + ( 3 * negated * pow( t, 2 ) * p2.value ) + ( std::pow( t, 3 ) * p3.value );
    ret.index = p0.index * pow( negated, 3 ) + ( 3 * pow( negated, 2 ) * t * p1.index ) + ( 3 * negated * pow( t, 2 ) * p2.index ) + ( std::pow( t, 3 ) * p3.index );

    return ret;
}


/**
    \class QuadraticBezierCurvePointSampler
    \since 1.0
    \brief A curve point sampler based on quadratic
        bezier curves.
*/
template < class _t_range, class _t_index >
struct QuadraticBezierCurvePointSampler {
    typedef _t_range                            ValueRange;
    typedef _t_index                            IndexType;
    typedef typename ValueRange::ValueType      ValueType;
    typedef CurvePoint<IndexType, ValueType>     CurvePointType;

    CurvePointType  sample( const CurvePointType& first, const CurvePointType& second, const IndexType& index ) const {
        assert( index > first.index );
        assert( index < second.index );
        assert( first.index < second.index );

        const IndexType indexDiff   = second.index - first.index;
        const IndexType indexOffset = index - first.index;

        assert( indexDiff != 0 );
        assert( indexOffset != 0 );

        const float indexFactor = ( float )indexOffset / ( float )indexDiff;

        return libgraphics::quadraticBezier(
                   first,
                   second,
                   indexFactor
               );
    }
};

/**
    \class CubicBezierCurvePointSampler
    \since 1.0
    \brief A curve point sampler based on cubic
        bezier curves.
*/
template < class _t_range, class _t_index >
struct CubicBezierCurvePointSampler {
    typedef _t_range                            ValueRange;
    typedef _t_index                            IndexType;
    typedef typename ValueRange::ValueType      ValueType;
    typedef CurvePoint<IndexType, ValueType>     CurvePointType;

    CubicBezierCurvePointSampler(
        const CurvePointType& _upperBound,
        const CurvePointType& _lowerBound
    ) : upperBound( _upperBound ), lowerBound( _lowerBound ) {}
    CurvePointType  sample( const CurvePointType& first, const CurvePointType& second, const IndexType& index ) const {
        assert( index > first.index );
        assert( index < second.index );
        assert( first.index < second.index );

        const IndexType indexDiff   = second.index - first.index;
        const IndexType indexOffset = index - first.index;

        assert( indexDiff != 0 );
        assert( indexOffset != 0 );

        const float indexFactor = ( float )indexOffset / ( float )indexDiff;

        return libgraphics::cubicBezier(
                   first,
                   second,
                   upperBound,
                   lowerBound,
                   indexFactor
               );
    }

    CurvePointType  upperBound;
    CurvePointType  lowerBound;
};


template<class T>
struct Quadruple {
    T A;
    T B;
    T C;
    T D;
    Quadruple( const T& a, const T& b, const T& c, const T& d ) : A( a ), B( b ), C( c ), D( d ) {}
    Quadruple( ) : A( T() ), B( T() ), C( T() ), D( T() ) {}
};

template<class T>
T clamp( T var, T min, T max ) {
    if( var < min ) {
        return min;
    }

    if( var > max ) {
        return max;
    } else {
        return var;
    }
}

std::vector< Quadruple<Point32F> > calcControlPoints( const std::vector<float>& xsIn, const std::vector<float>& ysIn );
std::vector< Quadruple<Point32F> > calcControlPoints( const std::vector<Point32F>& points );

std::vector<Point32F> calcBezier( const std::vector<float>& xsIn, const std::vector<float>& ysIn, size_t count );
std::vector<Point32F> calcLinear( const std::vector<Point32F>& points, size_t count );
std::vector<Point32F> calcBezier( const std::vector<Point32F>& points, size_t count );


}
