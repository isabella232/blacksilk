
#include <utils/curve.hpp>

#include <libgraphics/bezier.hpp>
#include <libgraphics/curve.hpp>

namespace blacksilk {

// simple linear interpolation between two points
void lerp( QPointF& dest, const QPointF& a, const QPointF& b, const float t ) {
    dest.setX( a.x() + ( b.x() - a.x() )*t );
    dest.setY( a.y() + ( b.y() - a.y() )*t );
}

// evaluate a point on a bezier-curve. t goes from 0 to 1.0
void bezier( QPointF& dest, const QPointF& a, const QPointF& b, const QPointF& c, const QPointF& d, const float t ) {
    typedef libgraphics::Point<float>   TPoint;

    TPoint p0( a.x(), a.y() );
    TPoint p1( b.x(), b.y() );
    TPoint p2( c.x(), c.y() );
    TPoint p3( d.x(), d.y() );

    const auto pret = libgraphics::cubicBezier<float>(
                          p0,
                          p1,
                          p2,
                          p3,
                          t
                      );

    dest.setX( pret.x );
    dest.setY( pret.y );
}

QVector< Quadruple<QPointF> > calcControlPoints( const QVector<QPointF>& points ) {
    assert( !points.empty() );

    QVector<float>  xsIn;
    QVector<float>  ysIn;

    xsIn.resize( points.size() );
    ysIn.resize( points.size() );

    size_t index( 0 );

    for( auto it = points.begin(); it != points.end(); ++it ) {
        xsIn[index] = ( *it ).x();
        ysIn[index] = ( *it ).y();

        ++index;
    }

    return blacksilk::calcControlPoints( xsIn, ysIn );
}

QVector< Quadruple<QPointF> > calcControlPoints( const QVector<float>& xsIn, const QVector<float>& ysIn ) {
    assert( !xsIn.empty() && !ysIn.empty() );
    assert( xsIn.size() == ysIn.size() );

    int size = xsIn.size();
    QVector< Quadruple<QPointF> > retVal( size );

    float multi = 0.2;

    QVector<float> xs = xsIn;
    xs.push_front( xsIn[0] + xsIn[1] - xsIn[0] );
    xs.push_back( xsIn[size - 1] + xsIn[size - 2] - xsIn[size - 1] );

    QVector<float> ys = ysIn;
    ys.push_front( ysIn[0] + ysIn[1] - ysIn[0] );
    ys.push_back( ysIn[size - 1] + ysIn[size - 2] - ysIn[size - 1] );

    if( xs.size() > 4 ) {

        for( int i = 1; i < size; ++i ) {
            QPointF A( xs[i - 1], ys[i - 1] );
            QPointF B( xs[i],     ys[i] );
            QPointF C( xs[i + 1], ys[i + 1] );
            QPointF D( xs[i + 2], ys[i + 2] );

            QPointF B2 = B + multi * ( C - A );
            QPointF C2 = C + multi * ( B - D );

            B2.setY( clamp<float>( B2.y(), 0.0, 1.0 ) );
            C2.setY( clamp<float>( C2.y(), 0.0, 1.0 ) );

            if( B2.x() > C.x() ) {
                float x1 = B2.x() - B.x();
                float x2 = C.x()  - B.x();

                float y1 = B2.y() - C.y();
                float y2 = x2 / x1 * y1;

                B2.setX( C.x() );
                B2.setY( C.y() + y2 );
            }

            if( C2.x() < B.x() ) {
                float x1 = C.x() - C2.x();
                float x2 = C.x() - B.x();

                float y1 = C2.y() - B.y();
                float y2 = x2 / x1 * y1;

                C2.setX( B.x() );
                C2.setY( B.y() + y2 );
            }

            if( C2.x() < B2.x() ) {
                float center = ( B2.x() + C2.x() ) / 2.0;

                float xC1 = C.x() - C2.x();
                float xC2 = center - C.x();

                float xB1 = B.x() - B2.x();
                float xB2 = center - B.x();

                float yC1 = C.y() - C2.y();
                float yB1 = B.y() - B2.y();

                float yC2 = xC2 / xC1 * yC1;
                float yB2 = xB2 / xB1 * yB1;

                C2.setY( C.y() + yC2 );
                B2.setY( B.y() + yB2 );

                C2.setX( center );
                B2.setX( center );
            }

            retVal[i - 1] = Quadruple<QPointF>( B, B2, C2, C );
        }
    } else {
        retVal.clear();
    }

    return retVal;
}


QVector<QPointF> calcBezier( const QVector<QPointF>& points ) {
    assert( !points.empty() );

    QVector<float>  xsIn;
    QVector<float>  ysIn;

    xsIn.resize( points.size() );
    ysIn.resize( points.size() );

    size_t index( 0 );

    for( auto it = points.begin(); it != points.end(); ++it ) {
        xsIn[index] = ( *it ).x();
        ysIn[index] = ( *it ).y();

        ++index;
    }

    return blacksilk::calcBezier( xsIn, ysIn, points.size() );
}

QVector<QPointF> calcBezier( const QVector<float>& xsIn, const QVector<float>& ysIn, size_t count ) {
    assert( count != 0 );
    assert( !xsIn.empty() );
    assert( !ysIn.empty() );

    QVector<QPointF> retVal;
    retVal.resize( count );

    float step = 1.0 / count;

    QVector< Quadruple<QPointF> > controls = calcControlPoints( xsIn, ysIn );

    for( int i = 0; i < controls.size(); ++i ) {

        // Oversample 4 times
        int steps = 4 * ( ( controls[i].D.x() - controls[i].A.x() ) / step );

        for( int j = 0; j < steps; ++j ) {

            QPointF dest;

            bezier( dest, controls[i].A, controls[i].B, controls[i].C, controls[i].D, ( float )j / steps );

            int pos = ( int )( dest.x() * count );
            pos = clamp<int>( pos, 0, count - 1 );

            retVal[pos] = dest;
        }
    }

    return retVal;
}

}
