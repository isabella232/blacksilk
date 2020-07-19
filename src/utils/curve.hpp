#pragma once

#include <QVector>
#include <QPoint>

namespace blacksilk {

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

void lerp( QPointF& dest, const QPointF& a, const QPointF& b, const float t );

void bezier( QPointF& dest, const QPointF& a, const QPointF& b, const QPointF& c, const QPointF& d, const float t );

QVector< Quadruple<QPointF> > calcControlPoints( const QVector<float>& xsIn, const QVector<float>& ysIn );
QVector< Quadruple<QPointF> > calcControlPoints( const QVector<QPointF>& points );

QVector<QPointF> calcBezier( const QVector<float>& xsIn, const QVector<float>& ysIn, size_t count );
QVector<QPointF> calcBezier( const QVector<QPointF>& points );

}
