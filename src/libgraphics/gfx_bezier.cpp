
#include <libgraphics/bezier.hpp>

namespace libgraphics {

//! \TOOD Dup of lerp() in bs_curve.cpp?
void lerp( Point32F& dest, const Point32F& a, const Point32F& b, const Point32F& /*c*/, const Point32F& /*d*/, const float t ) {
    dest.x = ( a.x + ( b.x - a.x ) * t );
    dest.y = ( a.y + ( b.y - a.y ) * t );
}

std::vector< Quadruple<Point32F> > calcControlPoints( const std::vector<float>& xsIn, const std::vector<float>& ysIn ) {
    assert( !xsIn.empty() && !ysIn.empty() );
    assert( xsIn.size() == ysIn.size() );

    int size = xsIn.size();
    std::vector< Quadruple<Point32F> > retVal( size );

    float multi = 0.2;

    std::vector<float> xs( xsIn );
    xs.insert( xs.begin(), xsIn[0] + xsIn[1] - xsIn[0] );
    xs.push_back( xsIn[size - 1] + xsIn[size - 2] - xsIn[size - 1] );

    std::vector<float> ys( ysIn );
    ys.insert( ys.begin(), ysIn[0] + ysIn[1] - ysIn[0] );
    ys.push_back( ysIn[size - 1] + ysIn[size - 2] - ysIn[size - 1] );

    if( xs.size() > 4 ) {

        for( int i = 1; i < size; ++i ) {
            Point32F A( xs[i - 1], ys[i - 1] );
            Point32F B( xs[i],     ys[i] );
            Point32F C( xs[i + 1], ys[i + 1] );
            Point32F D( xs[i + 2], ys[i + 2] );

            Point32F B2 = B + ( ( C - A ) * multi );
            Point32F C2 = C + ( ( B - D ) * multi );

            B2.y = ( clamp<float>( B2.y, 0.0, 1.0 ) );
            C2.y = ( clamp<float>( C2.y, 0.0, 1.0 ) );

            if( B2.x > C.x ) {
                float x1 = B2.x - B.x;
                float x2 = C.x  - B.x;

                float y1 = B2.y - C.y;
                float y2 = x2 / x1 * y1;

                B2.x = ( C.x );
                B2.y = ( C.y + y2 );
            }

            if( C2.x < B.x ) {
                float x1 = C.x - C2.x;
                float x2 = C.x - B.x;

                float y1 = C2.y - B.y;
                float y2 = x2 / x1 * y1;

                C2.x = ( B.x );
                C2.y = ( B.y + y2 );
            }

            if( C2.x < B2.x ) {
                float center = ( B2.x + C2.x ) / 2.0;

                float xC1 = C.x - C2.x;
                float xC2 = center - C.x;

                float xB1 = B.x - B2.x;
                float xB2 = center - B.x;

                float yC1 = C.y - C2.y;
                float yB1 = B.y - B2.y;

                float yC2 = xC2 / xC1 * yC1;
                float yB2 = xB2 / xB1 * yB1;

                C2.y = ( C.y + yC2 );
                B2.y = ( B.y + yB2 );

                C2.x = ( center );
                B2.x = ( center );
            }

            retVal[i - 1] = Quadruple<Point32F>( B, B2, C2, C );
        }
    } else {
        retVal.clear();
    }

    return retVal;
}

std::vector< Quadruple<Point32F> > calcControlPoints( const std::vector<Point32F>& points ) {
    assert( !points.empty() );

    std::vector<float>  xsIn;
    std::vector<float>  ysIn;

    xsIn.resize( points.size() );
    ysIn.resize( points.size() );

    size_t index( 0 );

    for( auto it = points.begin(); it != points.end(); ++it ) {
        xsIn[index] = ( *it ).x;
        ysIn[index] = ( *it ).y;

        ++index;
    }

    return calcControlPoints( xsIn, ysIn );
}

std::vector<Point32F> calcBezier( const std::vector<float>& xsIn, const std::vector<float>& ysIn, size_t count ) {
    assert( count != 0 );
    assert( !xsIn.empty() );
    assert( !ysIn.empty() );

    std::vector<Point32F> retVal;
    retVal.resize( count );

    float step = 1.0 / count;

    std::vector< Quadruple<Point32F> > controls = calcControlPoints( xsIn, ysIn );

    for( size_t i = 0; i < controls.size(); ++i ) {

        // Oversample 4 times
        int steps = 4 * ( ( controls[i].D.x - controls[i].A.x ) / step );

        for( int j = 0; j < steps; ++j ) {

            Point32F dest;

            dest = libgraphics::cubicBezier( controls[i].A, controls[i].B, controls[i].C, controls[i].D, ( float )j / steps );

            int pos = ( int )( dest.x * count );
            pos = clamp<int>( pos, 0, count - 1 );

            retVal[pos] = dest;
        }
    }

    return retVal;
}

std::vector<Point32F> calcLinear( const std::vector<Point32F>& points, size_t count ) {

    std::vector<Point32F> retVal;
    retVal.resize( count );

    /* linear interpolation */
    for( size_t i = 0; i < count; i++ ) {
        float x = ( float )( i ) / count;
        int lower  = 0;
        int higher = 1;

        while( x > points[higher].x ) {
            lower ++;
            higher++;
        }

        float part = ( x - points[lower].x ) / ( points[higher].x - points[lower].x );
        float y = 1.0 * ( points[higher].y * part + points[lower].y * ( 1 - part ) );
        y = libgraphics::clamp<float>( y, 0.f, 1.f );
        retVal[i] = Point32F( x, y );
    }

    return retVal;
}

std::vector<Point32F> calcBezier( const std::vector<Point32F>& points, size_t count ) {
    assert( !points.empty() );

    std::vector<float>  xsIn;
    std::vector<float>  ysIn;

    xsIn.resize( points.size() );
    ysIn.resize( points.size() );

    size_t index( 0 );

    for( auto it = points.begin(); it != points.end(); ++it ) {
        xsIn[index] = ( *it ).x;
        ysIn[index] = ( *it ).y;

        ++index;
    }

    return calcBezier( xsIn, ysIn, count );
}

}
