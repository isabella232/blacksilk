#pragma once

#include <libgraphics/base.hpp>
#include <libgraphics/bitmap.hpp>
#include <libgraphics/fxapi.hpp>
#include <libgraphics/image.hpp>

#include <libgraphics/fx/operations/basic.hpp>
#include <libgraphics/backend/cpu/cpu_backenddevice.hpp>
#include <libgraphics/backend/cpu/cpu_imageobject.hpp>
#include <libgraphics/backend/cpu/cpu_imageoperation.hpp>

#include <limits>
#include <functional>

#include <QRunnable>
#include <QThreadPool>

namespace libgraphics {
namespace fx {
namespace operations {

void cpuExecuteTileBased(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::backend::cpu::ImageObject*   destination,
    libgraphics::backend::cpu::ImageObject*   source,
    libgraphics::Rect32I area,
    std::function<void( libgraphics::fxapi::ApiBackendDevice*, libgraphics::backend::cpu::ImageObject*, libgraphics::backend::cpu::ImageObject*, libgraphics::Rect32I )>   kernel,
    bool manualSync = false
);

namespace math {

struct Color3f {
        float r;
        float g;
        float b;

        inline Color3f() : r( 0.0f ), g( 0.0f ), b( 0.0f ) {}
        inline Color3f( float _r, float _g, float _b ) :
            r( _r ), g( _g ), b( _b ) {}
        inline Color3f( const Color3f& rhs ) : r( rhs.r ), g( rhs.g ),
            b( rhs.b ) {}
        inline explicit Color3f( float val ) : r( val ), g( val ), b( val ) {}

        inline bool operator == ( const Color3f& rhs ) const {
            return floatEqual( rhs.r, r ) && floatEqual( rhs.g, g ) &&
                   floatEqual( rhs.b, b );
        }
        inline bool operator != ( const Color3f& rhs ) const {
            return !floatEqual( rhs.r, r ) || !floatEqual( rhs.g, g ) ||
                   !floatEqual( rhs.b, b );
        }

        inline Color3f& operator *= ( const Color3f& rhs ) {
            r *= rhs.r;
            g *= rhs.g;
            b *= rhs.b;
            return *this;
        }
        inline Color3f& operator /= ( const Color3f& rhs ) {
            r /= rhs.r;
            g /= rhs.g;
            b /= rhs.b;
            return *this;
        }
        inline Color3f& operator += ( const Color3f& rhs ) {
            r += rhs.r;
            g += rhs.g;
            b += rhs.b;
            return *this;
        }
        inline Color3f& operator -= ( const Color3f& rhs ) {
            r -= rhs.r;
            g -= rhs.g;
            b -= rhs.b;
            return *this;
        }


        inline Color3f& operator *= ( const float& val ) {
            r *= val;
            g *= val;
            b *= val;
            return *this;
        }
        inline Color3f& operator /= ( const float& val ) {
            r /= val;
            g /= val;
            b /= val;
            return *this;
        }
        inline Color3f& operator += ( const float& val ) {
            r += val;
            g += val;
            b += val;
            return *this;
        }
        inline Color3f& operator -= ( const float& val ) {
            r -= val;
            g -= val;
            b -= val;
            return *this;
        }

        inline Color3f operator * ( const Color3f& rhs ) const {
            return Color3f(
                       r * rhs.r,
                       g * rhs.g,
                       b * rhs.b
                   );
        }
        inline Color3f operator / ( const Color3f& rhs ) const {
            return Color3f(
                       r / rhs.r,
                       g / rhs.g,
                       b / rhs.b
                   );
        }
        inline Color3f operator + ( const Color3f& rhs ) const {
            return Color3f(
                       r + rhs.r,
                       g + rhs.g,
                       b + rhs.b
                   );
        }
        inline Color3f operator - ( const Color3f& rhs ) const {
            return Color3f(
                       r - rhs.r,
                       g - rhs.g,
                       b - rhs.b
                   );
        }

        inline Color3f operator * ( const float& val ) const {
            return Color3f(
                       r * val,
                       g * val,
                       b * val
                   );
        }
        inline Color3f operator / ( const float& val ) const {
            return Color3f(
                       r / val,
                       g / val,
                       b / val
                   );
        }
        inline Color3f operator + ( const float& val ) const {
            return Color3f(
                       r + val,
                       g + val,
                       b + val
                   );
        }
        inline Color3f operator - ( const float& val ) const {
            return Color3f(
                       r - val,
                       g - val,
                       b - val
                   );
        }

        inline float sum() const {
            return r + g + b;
        }
        inline float weightedSum( float val ) const {
            return ( r * val ) + ( g * val ) + ( b * val );
        }
        inline float weightedSum( float a1, float a2, float a3 ) {
            return ( r * a1 ) + ( g * a2 ) + ( b * a3 );
        }
        inline float cubicSum() const {
            return ( r * r ) + ( g * g ) + ( b * b );
        }

        inline float length() const {
            const float _sum = cubicSum();

            if( !floatEqual( _sum, 0.0f ) ) {
                return sqrt(
                           _sum
                       );
            }

            return 0.0f;
        }
        inline Color3f normalize() const {
            const float _len = length();

            if( !floatEqual( _len, 0.0f ) ) {
                return Color3f(
                           r / _len,
                           g / _len,
                           b / _len
                       );
            }

            return Color3f();
        }
        inline Color3f negate() const {
            return Color3f(
                       1.0f - r,
                       1.0f - g,
                       1.0f - b
                   );
        }
        inline Color3f abs() const {
            return Color3f(
                       fabs( r ),
                       fabs( g ),
                       fabs( b )
                   );
        }

        inline void tie( float& _r, float& _g, float& _b ) {
            _r = r;
            _g = g;
            _b = b;
        }
    private:
        static inline bool floatEqual( float a, float b ) {
            return ( fabs( a - b ) < 0.00005f );
        }
};
template < int _max_value >
struct Color3i {
        int r;
        int g;
        int b;

        static const int maxValue = _max_value;

        inline Color3i() : r( 0 ), g( 0 ), b( 0 ) {}
        inline Color3i( int _r, int _g, int _b ) :
            r( _r ), g( _g ), b( _b ) {}
        inline Color3i( const Color3i& rhs ) : r( rhs.r ), g( rhs.g ),
            b( rhs.b ) {}
        inline explicit Color3i( int val ) : r( val ), g( val ), b( val ) {}

        inline bool operator == ( const Color3i& rhs ) const {
            return ( r == rhs.r ) && ( g == rhs.g ) && ( b == rhs.b );
        }
        inline bool operator != ( const Color3i& rhs ) const {
            return ( r != rhs.r ) || ( g != rhs.g ) || ( b != rhs.b );
        }

        inline Color3f toColor3f() const {
            return Color3f(
                       ( float )r / maxValue,
                       ( float )g / maxValue,
                       ( float )b / maxValue
                   );
        }
        static inline Color3i fromColor3f( const Color3f& rhs ) {
            return Color3i(
                       ( int )( rhs.r * maxValue ),
                       ( int )( rhs.g * maxValue ),
                       ( int )( rhs.b * maxValue )
                   );
        }


        inline Color3i& operator *= ( const Color3i& rhs ) {
            r = ( r * rhs.r / maxValue );
            g = ( g * rhs.g / maxValue );
            b = ( b * rhs.b / maxValue );
            return *this;
        }
        inline Color3i& operator /= ( const Color3i& rhs ) {
            r = ( r / rhs.r * maxValue );
            g = ( g / rhs.g * maxValue );
            b = ( b / rhs.b * maxValue );
            return *this;
        }
        inline Color3i& operator += ( const Color3i& rhs ) {
            r += rhs.r;
            g += rhs.g;
            b += rhs.b;
            return *this;
        }
        inline Color3i& operator -= ( const Color3i& rhs ) {
            r -= rhs.r;
            g -= rhs.g;
            b -= rhs.b;
            return *this;
        }


        inline Color3i& operator *= ( const int& val ) {
            r = r * val / maxValue;
            g = g * val / maxValue;
            b = b * val / maxValue;
            return *this;
        }
        inline Color3i& operator /= ( const int& val ) {
            r = r / val * maxValue;
            g = r / val * maxValue;
            b = r / val * maxValue;
            return *this;
        }
        inline Color3i& operator += ( const int& val ) {
            r += val;
            g += val;
            b += val;
            return *this;
        }
        inline Color3i& operator -= ( const int& val ) {
            r -= val;
            g -= val;
            b -= val;
        }

        inline Color3i& operator *= ( const float& val ) {
            r *= val;
            g *= val;
            b *= val;
            return *this;
        }
        inline Color3i& operator /= ( const float& val ) {
            r /= val;
            g /= val;
            b /= val;
            return *this;
        }
        inline Color3i& operator += ( const float& val ) {
            r += ( val * maxValue );
            g += ( val * maxValue );
            b += ( val * maxValue );
            return *this;
        }
        inline Color3i& operator -= ( const float& val ) {
            r -= ( val * maxValue );
            g -= ( val * maxValue );
            b -= ( val * maxValue );
            return *this;
        }

        inline Color3i operator * ( const Color3i& rhs ) const {
            return Color3i(
                       ( int )( ( r * rhs.r ) / maxValue ),
                       ( int )( ( g * rhs.g ) / maxValue ),
                       ( int )( ( b * rhs.b ) / maxValue )
                   );
        }
        inline Color3i operator / ( const Color3i& rhs ) const {
            return Color3i(
                       ( int )( ( r /  rhs.r ) * maxValue ),
                       ( int )( ( g /  rhs.g ) * maxValue ),
                       ( int )( ( b /  rhs.b ) * maxValue )
                   );
        }
        inline Color3i operator + ( const Color3i& rhs ) const {
            return Color3i(
                       r + rhs.r,
                       g + rhs.g,
                       b + rhs.b
                   );
        }
        inline Color3i operator - ( const Color3i& rhs ) const {
            return Color3i(
                       r - rhs.r,
                       g - rhs.g,
                       b - rhs.b
                   );
        }

        inline Color3i operator * ( const int& val ) const {
            return Color3i(
                       ( r * val / maxValue ),
                       ( g * val / maxValue ),
                       ( b * val / maxValue )
                   );
        }
        inline Color3i operator / ( const int& val ) const {
            return Color3i(
                       ( r / val * maxValue ),
                       ( g / val * maxValue ),
                       ( b / val * maxValue )
                   );
        }
        inline Color3i operator + ( const int& val ) const {
            return Color3i(
                       r + val,
                       g + val,
                       b + val
                   );
        }
        inline Color3i operator - ( const int& val ) const {
            return Color3i(
                       r - val,
                       g - val,
                       b - val
                   );
        }

        inline Color3i operator * ( const float& val ) const {
            return Color3i(
                       ( int )r * val,
                       ( int )g * val,
                       ( int )b * val
                   );
        }
        inline Color3i operator / ( const float& val ) const {
            return Color3i(
                       ( int )r / val,
                       ( int )g / val,
                       ( int )b / val
                   );
        }
        inline Color3i operator + ( const float& val ) const {
            return Color3i(
                       r + ( maxValue * val ),
                       g + ( maxValue * val ),
                       b + ( maxValue * val )
                   );
        }
        inline Color3i operator - ( const float& val ) const {
            return Color3i(
                       r - ( maxValue * val ),
                       g - ( maxValue * val ),
                       b - ( maxValue * val )
                   );
        }

        inline int sum() const {
            return r + g + b;
        }
        inline float weightedSum( float val ) const {
            return ( r * val ) + ( g * val ) + ( b * val );
        }
        inline float weightedSum( float a1, float a2, float a3 ) {
            return ( r * a1 ) + ( g * a2 ) + ( b * a3 );
        }
        inline int cubicSum() const {
            return ( r * r ) + ( g * g ) + ( b * b );
        }

        inline float length() const {
            const float _sum = cubicSum();

            if( !floatEqual( _sum, 0.0f ) ) {
                return sqrt(
                           _sum
                       );
            }

            return 0.0f;
        }
        inline Color3i normalize() const {
            const float _len = length();

            if( !floatEqual( _len, 0.0f ) ) {
                return Color3i(
                           r / _len,
                           g / _len,
                           b / _len
                       );
            }

            return Color3i();
        }
        inline Color3i negate() const {
            return Color3i(
                       maxValue - r,
                       maxValue - g,
                       maxValue - b
                   );
        }
        inline Color3i abs() const {
            return Color3i(
                       fabs( r ),
                       fabs( g ),
                       fabs( b )
                   );
        }
        inline void tie( int& _r, int& _g, int& _b ) {
            _r = r;
            _g = g;
            _b = b;
        }
    private:
        static inline bool floatEqual( float a, float b ) {
            return ( fabs( a - b ) < 0.00005f );
        }
};

inline Color3f minMaxUniform( const Color3f& input ) {
    return Color3f(
               std::min<float>( 1.0f, std::max<float>( 0.0f, input.r ) ),
               std::min<float>( 1.0f, std::max<float>( 0.0f, input.g ) ),
               std::min<float>( 1.0f, std::max<float>( 0.0f, input.b ) )
           );
}
inline Color3f grainExtract( const Color3f& first, const Color3f& second ) {
    return Color3f(
               first.r - second.r + 0.5f,
               first.g - second.g + 0.5f,
               first.b - second.b + 0.5f
           );
}
inline Color3f overlay( Color3f i, Color3f m ) {
    i *= 255.0f;
    m *= 255.0f;
    Color3f e = ( i * 0.0039f ) * ( i + ( m * 2.0f * 0.0039f ) * ( Color3f( 255.0f ) - i ) );
    e *= 0.0039f;
    return e;
}
inline Color3f grainMerge( Color3f i, float m ) {
    return Color3f(
               i.r + m - 0.5f,
               i.g + m - 0.5f,
               i.b + m - 0.5f
           );
}



template < int _max_value >
inline Color3i<_max_value> minMaxUniform( const Color3i<_max_value>& input ) {
    return Color3i<_max_value>(
               std::min<int>( _max_value, std::max<int>( 0, input.r ) ),
               std::min<int>( _max_value, std::max<int>( 0, input.g ) ),
               std::min<int>( _max_value, std::max<int>( 0, input.b ) )
           );
}

template < int _max_value >
inline Color3i<_max_value> grainExtract( const Color3i<_max_value>& first, const Color3i<_max_value>& second ) {
    return Color3i<_max_value>(
               first.r - second.r + ( int )( _max_value / 2 ),
               first.g - second.g + ( int )( _max_value / 2 ),
               first.b - second.b + ( int )( _max_value / 2 )
           );
}

template < int _max_value >
inline Color3i<_max_value> overlay( Color3i<_max_value> i, Color3i<_max_value> m ) {
    Color3i<_max_value> e = ( i * ( 1.0f / _max_value ) ) * ( i + ( m * 2.0f * ( 1.0f / _max_value ) ) * ( Color3i<_max_value>( _max_value ) - i ) );
    return e;
}

template < int _max_value >
inline Color3i<_max_value> grainMerge( Color3i<_max_value> i, float m ) {
    return Color3i<_max_value>(
               i.r + ( m * _max_value ) - ( _max_value / 2 ),
               i.g + ( m * _max_value ) - ( _max_value / 2 ),
               i.b + ( m * _max_value ) - ( _max_value / 2 )
           );
}
template < int _max_value >
inline Color3i<_max_value> max( const Color3i<_max_value>& first, const Color3i<_max_value>& second ) {
    const auto len_first    = first.length();
    const auto len_second   = second.length();
    return ( len_first > len_second ) ? first : second;
}
template < int _max_value >
inline Color3i<_max_value> min( const Color3i<_max_value>& first, const Color3i<_max_value>& second ) {
    const auto len_first    = first.length();
    const auto len_second   = second.length();
    return ( len_first < len_second ) ? first : second;
}

}

}
}
}

#define GetR(p) *(p + 0)
#define GetG(p) *(p + 1)
#define GetB(p) *(p + 2)
#define MapToFloat(max,value) ( (float)value / (float)max )
#define MapFloat(max,value) std::ceil( (float)value * (float)max)
#define MinMax(maxv,value) std::min<int>(maxv,std::max<int>(0,value))
#define MinMaxF(maxv,value) std::min<float>(maxv,std::max<float>(0,value))

#define GetColor3i(type,p) type ( (int) GetR(p), (int) GetG(p), (int) GetB(p) )
#define SetColor3i(type,color,p) \
    GetR(p) = color.r; \
    GetG(p) = color.g; \
    GetB(p) = color.b;


#define GetColor3f(max,p) math::Color3f( MapToFloat( max, GetR(p) ), MapToFloat( max, GetG(p) ),MapToFloat( max, GetB(p) ))
#define SetColor3f(maxv,color,p) \
    GetR(p) = MapFloat(maxv,color.r); \
    GetG(p) = MapFloat(maxv,color.g); \
    GetB(p) = MapFloat(maxv,color.b);
