#pragma once

#include <libcommon/def.hpp>
#include <libcommon/crt.hpp>
#include <libcommon/maybe.hpp>
#include <libcommon/mutex.hpp>
#include <libcommon/guards.hpp>
#include <libcommon/sharedptr.hpp>

#include <stdexcept>
#include <string>
#include <stdlib.h>
#include <memory.h>
#include <cmath>
#include <sstream>

#include <libgraphics/allocator.hpp>

namespace libgraphics {

/// impl: basic format
template < class _t_pixel_type, libcommon::SizeType _v_count >
struct BasicPixel {
    static const libcommon::SizeType Count = _v_count;

    typedef _t_pixel_type                       PixelType;
    typedef BasicPixel<_t_pixel_type, Count>     SelfType;

    static const libcommon::SizeType Size = sizeof( PixelType ) * Count;

    BasicPixel() {
        for( libcommon::SizeType i = 0; Count > i; ++i ) {
            Values[i] = 0;
        }
    }
    BasicPixel( const SelfType& rhs ) {
        memcpy( ( void* )&Values, ( void* )&rhs.Values, Size );
    }
    BasicPixel( BasicPixel&& rhs ) {
        swap( rhs );
    }

    SelfType& operator = ( const SelfType& rhs ) {
        ( void ) memcpy(
            &this->Values,
            &rhs.Values,
            Size
        );

        return *this;
    }

    void swap( BasicPixel& rhs ) {
        for( libcommon::SizeType i = 0; Count > i; ++i ) {
            std::swap(
                rhs.Values[i],
                Values[i]
            );
        }
    }

    bool operator == ( const SelfType& rhs ) const {
        for( libcommon::SizeType i = 0; Count > i; ++i ) {
            if( Values[i] != rhs.Values[i] ) {
                return false;
            }
        }

        return true;
    }
    bool operator != ( const SelfType& rhs ) const {
        for( libcommon::SizeType i = 0; Count > i; ++i ) {
            if( Values[i] != rhs.Values[i] ) {
                return true;
            }
        }

        return false;
    }

    SelfType operator + ( const SelfType& rhs ) const {
        SelfType p( *this );

        for( libcommon::SizeType i = 0; Count > i; ++i ) {
            p.Values[i] += rhs.Values[i];
        }

        return p;
    }
    SelfType operator * ( const SelfType& rhs ) const {
        SelfType p( *this );

        for( libcommon::SizeType i = 0; Count > i; ++i ) {
            p.Values[i] *= rhs.Values[i];
        }

        return p;
    }
    SelfType operator - ( const SelfType& rhs ) const {
        SelfType p( *this );

        for( libcommon::SizeType i = 0; Count > i; ++i ) {
            p.Values[i] -= rhs.Values[i];
        }

        return p;
    }
    SelfType operator / ( const SelfType& rhs ) const {
        SelfType p( *this );

        for( libcommon::SizeType i = 0; Count > i; ++i ) {
            p.Values[i] /= rhs.Values[i];
        }

        return p;
    }


    SelfType operator + ( const PixelType& val ) const {
        SelfType p( *this );

        for( libcommon::SizeType i = 0; Count > i; ++i ) {
            p.Values[i] += val;
        }

        return p;
    }
    SelfType operator - ( const PixelType& val ) const {
        SelfType p( *this );

        for( libcommon::SizeType i = 0; Count > i; ++i ) {
            p.Values[i] -= val;
        }

        return p;
    }
    SelfType operator * ( const PixelType& val ) const {
        SelfType p( *this );

        for( libcommon::SizeType i = 0; Count > i; ++i ) {
            p.Values[i] *= val;
        }

        return p;
    }
    SelfType operator / ( const PixelType& val ) const {
        SelfType p( *this );

        for( libcommon::SizeType i = 0; Count > i; ++i ) {
            p.Values[i] /= val;
        }

        return p;
    }

    SelfType& operator += ( const SelfType& rhs ) {
        for( libcommon::SizeType i = 0; Count > i; ++i ) {
            Values[i] += rhs.Values[i];
        }

        return *this;
    }
    SelfType& operator *= ( const SelfType& rhs ) {
        for( libcommon::SizeType i = 0; Count > i; ++i ) {
            Values[i] *= rhs.Values[i];
        }

        return *this;
    }
    SelfType& operator /= ( const SelfType& rhs ) {
        for( libcommon::SizeType i = 0; Count > i; ++i ) {
            Values[i] /= rhs.Values[i];
        }

        return *this;
    }
    SelfType& operator -= ( const SelfType& rhs ) {
        for( libcommon::SizeType i = 0; Count > i; ++i ) {
            Values[i] -= rhs.Values[i];
        }

        return *this;
    }

    SelfType& operator += ( const PixelType& val ) {
        for( libcommon::SizeType i = 0; Count > i; ++i ) {
            Values[i] += val;
        }

        return *this;
    }
    SelfType& operator -= ( const PixelType& val ) {
        for( libcommon::SizeType i = 0; Count > i; ++i ) {
            Values[i] -= val;
        }

        return *this;
    }
    SelfType& operator *= ( const PixelType& val ) {
        for( libcommon::SizeType i = 0; Count > i; ++i ) {
            Values[i] *= val;
        }

        return *this;
    }
    SelfType& operator /= ( const PixelType& val ) {
        for( libcommon::SizeType i = 0; Count > i; ++i ) {
            Values[i] /= val;
        }

        return *this;
    }


    PixelType   Values[Count];
};


/// collection of different types of formats
namespace formats {

/// the format family
namespace family {
enum t {
    RGB,
    RGBA,
    BGR,
    BGRA,
    Mono,
    ARGB,
    Invalid
};
}

struct EChannelDepth {
    enum t {
        b8 = sizeof( unsigned char ),
        b16 = sizeof( unsigned short ),
        b32 = sizeof( unsigned int ),
        b32f = sizeof( float )
    };
};

template < unsigned long channelCount >
struct EPixelDepth {
    enum t {
        b8 = EChannelDepth::b8 * channelCount,
        b16 = EChannelDepth::b16 * channelCount,
        b32 = EChannelDepth::b32 * channelCount,
        b32f = EChannelDepth::b32f * channelCount
    };
};
template < unsigned long channelDepth >
inline unsigned long getPixelDepth( unsigned long channelCount ) {
    return channelCount * channelDepth;
}
}

struct Format {
    libcommon::SizeType     byteSize;
    formats::family::t      family;
    libcommon::SizeType     channels;

    Format() : byteSize( 0 ), family( formats::family::Invalid ), channels( 0 ) {}
    Format( const libcommon::SizeType& _size, const formats::family::t& _family, const libcommon::SizeType& _channels ) : byteSize( _size ), family( _family ), channels( _channels ) {}
    Format( const Format& rhs ) : byteSize( rhs.byteSize ), family( rhs.family ), channels( rhs.channels ) {}

    bool sameFamily( const Format& rhs ) const { return ( family == rhs.family ); }
    bool same( const Format& rhs ) const { return ( family == rhs.family ) && ( byteSize == rhs.byteSize ) && ( channels == rhs.channels ); }

    bool operator == ( const Format& rhs ) const {
        return same( rhs );
    }
    bool operator != ( const Format& rhs ) const {
        return !same( rhs );
    }
};

namespace formats {

struct Mono8 {
    typedef libgraphics::BasicPixel <
    unsigned char,
             1
             > t;
    static const libcommon::SizeType ByteSize;// t::Size;
    static const formats::family::t  Family;// formats::family::Mono;
    static const libcommon::SizeType Channels; // 1;

    static Format toFormat() {
        return Format(
                   ByteSize,
                   Family,
                   Channels
               );
    }
};
struct Mono16 {
    typedef libgraphics::BasicPixel <
    unsigned short,
             1
             > t;
    static const libcommon::SizeType ByteSize;// t::Size;
    static const formats::family::t  Family;// formats::family::Mono;
    static const libcommon::SizeType Channels; // 1;

    static Format toFormat() {
        return Format(
                   ByteSize,
                   Family,
                   Channels
               );
    }
};
struct Mono32 {
    typedef libgraphics::BasicPixel <
    unsigned int,
             1
             > t;
    static const libcommon::SizeType ByteSize;// t::Size;
    static const formats::family::t  Family;// formats::family::Mono;
    static const libcommon::SizeType Channels; // 1;

    static Format toFormat() {
        return Format(
                   ByteSize,
                   Family,
                   Channels
               );
    }
};

struct RGB8 {
    typedef libgraphics::BasicPixel <
    unsigned char,
             3
             > t;
    static const libcommon::SizeType ByteSize;// t::Size;
    static const formats::family::t  Family;// formats::family::RGB;
    static const libcommon::SizeType Channels; // 3;

    static Format toFormat() {
        return Format(
                   ByteSize,
                   Family,
                   Channels
               );
    }
};
struct RGB16 {
    typedef libgraphics::BasicPixel <
    unsigned short,
             3
             > t;
    static const libcommon::SizeType ByteSize;// t::Size;
    static const formats::family::t  Family;// formats::family::RGB;
    static const libcommon::SizeType Channels; // 3;

    static Format toFormat() {
        return Format(
                   ByteSize,
                   Family,
                   Channels
               );
    }
};
struct RGB32 {
    typedef libgraphics::BasicPixel <
    unsigned int,
             3
             > t;
    static const libcommon::SizeType ByteSize;// t::Size;
    static const formats::family::t  Family;// formats::family::RGB;
    static const libcommon::SizeType Channels; // 3;

    static Format toFormat() {
        return Format(
                   ByteSize,
                   Family,
                   Channels
               );
    }
};
struct RGB32F {
    typedef libgraphics::BasicPixel <
    float,
    3
    > t;
    static const libcommon::SizeType ByteSize;// t::Size;
    static const formats::family::t  Family;// formats::family::RGB;
    static const libcommon::SizeType Channels; // 3;

    static Format toFormat() {
        return Format(
                   ByteSize,
                   Family,
                   Channels
               );
    }
};

struct RGBA8 {
    typedef libgraphics::BasicPixel <
    unsigned char,
             4
             > t;
    static const libcommon::SizeType ByteSize;// t::Size;
    static const formats::family::t  Family;// formats::family::RGBA;
    static const libcommon::SizeType Channels; // 4;

    static Format toFormat() {
        return Format(
                   ByteSize,
                   Family,
                   Channels
               );
    }
};
struct RGBA16 {
    typedef libgraphics::BasicPixel <
    unsigned short,
             4
             > t;
    static const libcommon::SizeType ByteSize;// t::Size;
    static const formats::family::t  Family;// formats::family::RGBA;
    static const libcommon::SizeType Channels; // 4;

    static Format toFormat() {
        return Format(
                   ByteSize,
                   Family,
                   Channels
               );
    }
};
struct RGBA32 {
    typedef libgraphics::BasicPixel <
    unsigned int,
             4
             > t;
    static const libcommon::SizeType ByteSize;// t::Size;
    static const formats::family::t  Family;// formats::family::RGBA;
    static const libcommon::SizeType Channels; // 4;

    static Format toFormat() {
        return Format(
                   ByteSize,
                   Family,
                   Channels
               );
    }
};
struct RGBA32F {
    typedef libgraphics::BasicPixel <
    float,
    4
    > t;
    static const libcommon::SizeType ByteSize;// t::Size;
    static const formats::family::t  Family;// formats::family::RGBA;
    static const libcommon::SizeType Channels; // 4;

    static Format toFormat() {
        return Format(
                   ByteSize,
                   Family,
                   Channels
               );
    }
};

struct ARGB8 {
    typedef libgraphics::BasicPixel <
    unsigned char,
             4
             > t;
    static const libcommon::SizeType ByteSize;// t::Size;
    static const formats::family::t  Family;// formats::family::ARGB;
    static const libcommon::SizeType Channels; // 4;

    static Format toFormat() {
        return Format(
                   ByteSize,
                   Family,
                   Channels
               );
    }
};
struct ARGB16 {
    typedef libgraphics::BasicPixel <
    unsigned short,
             4
             > t;
    static const libcommon::SizeType ByteSize;// t::Size;
    static const formats::family::t  Family;// formats::family::ARGB;
    static const libcommon::SizeType Channels; // 4;

    static Format toFormat() {
        return Format(
                   ByteSize,
                   Family,
                   Channels
               );
    }
};
struct ARGB16S {
    typedef libgraphics::BasicPixel <
    signed short,
           4
           > t;
    static const libcommon::SizeType ByteSize;
    static const formats::family::t  Family;
    static const libcommon::SizeType Channels;

    static Format toFormat() {
        return Format(
                   ByteSize,
                   Family,
                   Channels
               );
    }
};
struct ARGB32 {
    typedef libgraphics::BasicPixel <
    unsigned int,
             4
             > t;
    static const libcommon::SizeType ByteSize;// t::Size;
    static const formats::family::t  Family;// formats::family::ARGB;
    static const libcommon::SizeType Channels; // 4;

    static Format toFormat() {
        return Format(
                   ByteSize,
                   Family,
                   Channels
               );
    }
};
struct ARGB32S {
    typedef libgraphics::BasicPixel <
    signed int,
           4
           > t;
    static const libcommon::SizeType ByteSize;
    static const formats::family::t  Family;
    static const libcommon::SizeType Channels;

    static Format toFormat() {
        return Format(
                   ByteSize,
                   Family,
                   Channels
               );
    }
};
struct ARGB32F {
    typedef libgraphics::BasicPixel <
    float,
    4
    > t;
    static const libcommon::SizeType ByteSize;// t::Size;
    static const formats::family::t  Family;// formats::family::ARGB;
    static const libcommon::SizeType Channels; // 4;

    static Format toFormat() {
        return Format(
                   ByteSize,
                   Family,
                   Channels
               );
    }
};


struct BGR8 {
    typedef libgraphics::BasicPixel <
    unsigned char,
             3
             > t;
    static const libcommon::SizeType ByteSize;// t::Size;
    static const formats::family::t  Family;// formats::family::BGR;
    static const libcommon::SizeType Channels; // 3;

    static Format toFormat() {
        return Format(
                   ByteSize,
                   Family,
                   Channels
               );
    }
};
struct BGR16 {
    typedef libgraphics::BasicPixel <
    unsigned short,
             3
             > t;
    static const libcommon::SizeType ByteSize;// t::Size;
    static const formats::family::t  Family;// formats::family::BGR;
    static const libcommon::SizeType Channels; // 3;

    static Format toFormat() {
        return Format(
                   ByteSize,
                   Family,
                   Channels
               );
    }
};
struct BGR32 {
    typedef libgraphics::BasicPixel <
    unsigned int,
             3
             > t;
    static const libcommon::SizeType ByteSize;// t::Size;
    static const formats::family::t  Family;// formats::family::BGR;
    static const libcommon::SizeType Channels; // 3;

    static Format toFormat() {
        return Format(
                   ByteSize,
                   Family,
                   Channels
               );
    }
};
struct BGR32F {
    typedef libgraphics::BasicPixel <
    float,
    3
    > t;
    static const libcommon::SizeType ByteSize;// t::Size;
    static const formats::family::t  Family;// formats::family::BGR;
    static const libcommon::SizeType Channels; // 3;

    static Format toFormat() {
        return Format(
                   ByteSize,
                   Family,
                   Channels
               );
    }
};


/// signed integer formats
struct RGB16S {
    typedef libgraphics::BasicPixel <
    signed short,
           3
           > t;
    static const libcommon::SizeType ByteSize;
    static const formats::family::t  Family;
    static const libcommon::SizeType Channels;

    static Format toFormat() {
        return Format(
                   ByteSize,
                   Family,
                   Channels
               );
    }
};
struct RGB32S {
    typedef libgraphics::BasicPixel <
    signed int,
           3
           > t;
    static const libcommon::SizeType ByteSize;
    static const formats::family::t  Family;
    static const libcommon::SizeType Channels;

    static Format toFormat() {
        return Format(
                   ByteSize,
                   Family,
                   Channels
               );
    }
};
struct RGBA16S {
    typedef libgraphics::BasicPixel <
    signed short,
           4
           > t;
    static const libcommon::SizeType ByteSize;
    static const formats::family::t  Family;
    static const libcommon::SizeType Channels;

    static Format toFormat() {
        return Format(
                   ByteSize,
                   Family,
                   Channels
               );
    }
};
struct RGBA32S {
    typedef libgraphics::BasicPixel <
    signed int,
           4
           > t;
    static const libcommon::SizeType ByteSize;
    static const formats::family::t  Family;
    static const libcommon::SizeType Channels;

    static Format toFormat() {
        return Format(
                   ByteSize,
                   Family,
                   Channels
               );
    }
};
struct Mono16S {
    typedef libgraphics::BasicPixel <
    signed short,
           1
           > t;
    static const libcommon::SizeType ByteSize;
    static const formats::family::t  Family;
    static const libcommon::SizeType Channels;

    static Format toFormat() {
        return Format(
                   ByteSize,
                   Family,
                   Channels
               );
    }
};

struct Mono32S {
    typedef libgraphics::BasicPixel <
    signed int,
           1
           > t;
    static const libcommon::SizeType ByteSize;// t::Size;
    static const formats::family::t  Family;// formats::family::Mono;
    static const libcommon::SizeType Channels; // 1;

    static Format toFormat() {
        return Format(
                   ByteSize,
                   Family,
                   Channels
               );
    }
};
}

template < class _t_any >
inline float mapColorValueToFloat( const _t_any& value, const _t_any& max ) {
    return ( float )value / ( float )max;
}
template < class _t_any >
inline float mapColorValueToFloat( const _t_any& value ) {
    return mapColorValueToFloat( value, std::numeric_limits<_t_any>::max() );
}
template < class _t_any >
inline float mapValueRangeToFloat( const _t_any& value, const _t_any& min, const _t_any& max ) {
    assert( max > min );
    return ( value < min ) ? 0.0f : ( ( float )value / ( max - min ) );
}
template < class _t_any >
inline _t_any mapFloatToValueRange( const float& value, const _t_any& min, const _t_any& max ) {
    assert( max > min );
    long long intermediate = ( long long )std::ceil( value * max );
    return ( _t_any )std::max<_t_any>( min, ( _t_any )std::min<long long>( intermediate, max ) );
}

template < class _t_first, class _t_second >
inline _t_second mapRanges( const _t_first& value, const _t_first& min0, const _t_first& max0,
                            const _t_second& min1, const _t_second& max1 ) {
    float floatingValue = mapValueRangeToFloat( value, min0, max0 );
    return mapFloatToValueRange(
               floatingValue,
               min1,
               max1
           );
}

/// impl: point
template < class _t_any >
struct Point {
    typedef _t_any      ValueType;

    ValueType   x;
    ValueType   y;

    Point() : x( 0 ), y( 0 ) {}
    Point( const ValueType& _x, const ValueType& _y ) : x( _x ), y( _y ) {}

    inline bool operator == ( const Point& rhs ) const {
        return ( x == rhs.x ) && ( y == rhs.y );
    }
    inline bool operator != ( const Point& rhs ) const {
        return ( x != rhs.x ) || ( y != rhs.y );
    }

    bool isNull() const {
        return ( x == 0 ) && ( y == 0 );
    }
    double length() const {
        return sqrt( ( double )( x * x ) + ( double )( y * y ) );
    }
    double distanceTo( const Point& other ) const {
        return Point(
                   x - other.x,
                   y - other.y
               ).length();
    }

    Point& operator += ( const Point& rhs ) {
        x += rhs.x;
        y += rhs.y;

        return *this;
    }
    template < class _t_type >
    Point& operator += ( const _t_type& val ) {
        x += val;
        y += val;

        return *this;
    }
    Point& operator -= ( const Point& rhs ) {
        x -= rhs.x;
        y -= rhs.y;

        return *this;
    }
    template < class _t_type >
    Point& operator -= ( const _t_type& val ) {
        x -= val;
        y -= val;

        return *this;
    }
    Point& operator *= ( const Point& rhs ) {
        x *= rhs.x;
        y *= rhs.y;

        return *this;
    }
    template < class _t_type >
    Point& operator *= ( const _t_type& val ) {
        x *= val;
        y *= val;

        return *this;
    }
    Point& operator /= ( const Point& rhs ) {
        x /= rhs.x;
        y /= rhs.y;

        return *this;
    }
    template < class _t_type >
    Point& operator /= ( const _t_type& val ) {
        x /= val;
        y /= val;

        return *this;
    }

    Point operator + ( const Point& rhs ) const {
        return Point(
                   x + rhs.x,
                   y + rhs.y
               );
    }
    template < class _t_type >
    Point operator + ( const _t_type& val ) const {
        return Point(
                   x + val,
                   y + val
               );
    }
    Point operator - ( const Point& rhs ) const {
        return Point(
                   x - rhs.x,
                   y - rhs.y
               );
    }
    template < class _t_type >
    Point operator - ( const _t_type& val ) const {
        return Point(
                   x - val,
                   y - val
               );
    }
    Point operator * ( const Point& rhs ) const {
        return Point(
                   x * rhs.x,
                   y * rhs.y
               );
    }
    template < class _t_type >
    Point operator * ( const _t_type& val ) const {
        return Point(
                   x * val,
                   y * val
               );
    }
    Point operator / ( const Point& rhs ) const {
        return Point(
                   x / rhs.x,
                   y / rhs.y
               );
    }
    template < class _t_type >
    Point operator / ( const _t_type& val ) const {
        return Point(
                   x / val,
                   y / val
               );
    }

    bool normalize() {
        if( !isNull() ) {
            const auto len = length();

            x /= len;
            y /= len;

            return true;
        }

        return false;
    }
    Point normalized() const {
        Point p( x, y );

        assert( !p.isNull() );
        p.normalize();

        return p;
    }
    double dot( const Point& rhs ) const {
        return ( double )( x * rhs.x ) + ( double )( y * rhs.y );
    }
    double perpDot( const Point& rhs ) const {
        return ( double )( y * rhs.x ) - ( double )( x * rhs.y );
    }

    /// \todo implement double/float comparison
};
typedef Point<float>             Point32F;
typedef Point<double>            Point64F;
typedef Point<char>              Point8I;
typedef Point<unsigned char>     Point8UI;
typedef Point<short>             Point16I;
typedef Point<unsigned short>    Point16UI;
typedef Point<int>               Point32I;
typedef Point<unsigned int>      Point32UI;
typedef Point<long long>         Point64I;
typedef Point<unsigned long long> Point64UI;

/// impl: line
template < class _t_any >
struct Line {
    typedef _t_any      ValueType;

    Line() {}
    Line( const Point<ValueType>& _begin, const Point<ValueType>& _end ) :
        begin( _begin ), end( _end ) {}
    Line( ValueType bx, ValueType by, ValueType ex, ValueType ey ) :
        begin( bx, by ), end( ex, ey ) {}

    bool intersect( const Point<ValueType>& other ) const {
        const auto _dir = direction();
        const auto _distance = begin.distanceTo( other );

        return ( other == ( begin + ( _dir * _distance ) ) );
    }
    bool intersect( const Line<ValueType>& other ) const {
        const auto a = end - begin;
        const auto b = other.end - other.begin;

        const auto f = a.perpDot( b );

        if( f <= 0.005 ) {
            return false;
        }

        Point<ValueType> c( other.end - end );
        const auto aa = a.perpDot( c );
        const auto bb = b.perpDot( c );

        if( f <= 0 ) {
            if( aa > 0 || bb > 0 || aa < f || b < f ) {
                return false;
            }
        } else {
            if( aa < 0 || bb < 0 || aa > f || b > f ) {
                return false;
            }
        }

        return true;
    }
    bool getIntersection( const Line<ValueType>& other, Point<ValueType>& intersection ) const {
        const auto a = end - begin;
        const auto b = other.end - other.begin;

        const auto f = a.perpDot( b );

        if( ( f <= 0.005 ) && ( f >= 0.005 ) ) {
            return false;
        }

        Point<ValueType> c( other.end - end );
        const auto aa = a.perpDot( c );
        const auto bb = b.perpDot( c );

        if( f <= 0 ) {
            if( aa > 0 || bb > 0 || aa < f || bb < f ) {
                return false;
            }
        } else {
            if( aa < 0 || bb < 0 || aa > f || bb > f ) {
                return false;
            }
        }

        const double dist = 1.0 - ( aa / f );

        intersection = ( ( other.end - other.begin ) * dist ) + other.begin;
        return true;
    }

    Point<ValueType>    direction() const {
        return ( end - begin ).normalized();
    }

    ValueType length() const {
        return static_cast<ValueType>( ( end - begin ).length() );
    }

    inline bool operator == ( const Line<ValueType>& rhs ) const {
        return ( begin == rhs.begin ) && ( end == rhs.end );
    }
    inline bool operator != ( const Line<ValueType>& rhs ) const {
        return ( begin != rhs.begin ) || ( end != rhs.end );
    }

    Point<ValueType>    begin;
    Point<ValueType>    end;
};
typedef Line<float>             Line32F;
typedef Line<double>            Line64F;
typedef Line<char>              Line8I;
typedef Line<unsigned char>     Line8UI;
typedef Line<short>             Line16I;
typedef Line<unsigned short>    Line16UI;
typedef Line<int>               Line32I;
typedef Line<unsigned int>      Line32UI;
typedef Line<long long>         Line64I;
typedef Line<unsigned long long> Line64UI;

/// impl: rect
template < class _t_any >
struct  Rect {
    typedef _t_any      ValueType;

    ValueType       x;
    ValueType       y;
    ValueType       width;
    ValueType       height;

    Rect() : x( 0 ), y( 0 ), width( 0 ), height( 0 ) {}
    Rect( const ValueType& _width, const ValueType& _height ) : x( 0 ), y( 0 ), width( _width ), height( _height ) {}
    Rect( const ValueType& _x, const ValueType& _y, const ValueType& _width, const ValueType& _height ) : x( _x ), y( _y ), width( _width ), height( _height ) {}

    inline bool operator == ( const Rect& rhs ) const {
        return ( x == rhs.x ) && ( y == rhs.y ) && ( width == rhs.width ) && ( height == rhs.height );
    }
    inline bool operator != ( const Rect& rhs ) const {
        return ( x != rhs.x ) || ( y != rhs.y ) || ( width != rhs.width ) || ( height != rhs.height );
    }

    std::string toString() const {
        std::stringstream ss;
        ss << "[" << x << "," << y << "," << width << "," << height << "]";
        return ss.str();
    }

    ValueType size() const {
        return width * height;
    }

    bool contains( ValueType px, ValueType py )  const {
        if( px >= x && px <= x + width ) {
            if( py >= y && py <= y + height ) {
                return true;
            }
        }

        return false;
    }
    bool contains( const Point<ValueType>& other ) const {
        return contains(
                   other.x,
                   other.y
               );
    }
    bool contains( const Line<ValueType>& other )  const {
        return contains(
                   other.begin
               )  && contains(
                   other.end
               );
    }
    bool contains( const Rect<ValueType>& other )  const {
        return contains(
                   other.topLeft()
               ) && contains(
                   other.topRight()
               ) && contains(
                   other.bottomLeft()
               ) && contains(
                   other.bottomLeft()
               );
    }

    bool intersect( ValueType px, ValueType py )  const {
        if( px >= x && px <= x + width ) {
            if( py >= y && py <= y + height ) {
                return true;
            }
        }

        return false;
    }
    bool intersect( const Point<ValueType>& other )  const {
        return intersect(
                   other.x,
                   other.y
               );
    }
    bool intersect( const Rect& other )  const {
        return intersect(
                   other.topLeft()
               ) || intersect(
                   other.topRight()
               ) || intersect(
                   other.bottomLeft()
               ) || intersect(
                   other.bottomLeft()
               );
    }
    bool getIntersection( const Rect& other, Rect& intersectionRect )  const {
        /*
         Case 1:
            A       A

                B           B
            A       A

                B           B
         Case 2: ( see first if )
            A               A
                B       B

                B       B
            A               A
         Case 3:
            A               A
                    B               B

                    B               B
            A               A

        Case 4:
            A               A

         B                      B
            A               A

         B                      B

        */
        const auto containsTopLeft = other.contains( topLeft() );
        const auto containsTopRight = other.contains( topRight() );
        const auto containsBottomLeft = other.contains( bottomLeft() );
        const auto containsBottomRight = other.contains( bottomRight() );

        const auto containmentCount =
            ( int )containsTopLeft +
            ( int )containsTopRight +
            ( int )containsBottomLeft +
            ( int )containsBottomRight;

        /// case 1: other contains all points
        if( containmentCount == 4 ) {
            intersectionRect = *this;
            return true;
        }

        /// case 2: other contains one point
        if( containmentCount == 1 ) {
            if( containsTopLeft ) {
                Point<ValueType> intersectionLeft;

                if( !other.bottomLine().getIntersection( leftLine(), intersectionLeft ) ) {
                    return false;
                }

                Point<ValueType> intersectionTop;

                if( !other.rightLine().getIntersection( topLine(), intersectionTop ) ) {
                    return false;
                }

                intersectionRect.x = topLeft().x;
                intersectionRect.y = topLeft().y;

                intersectionRect.width = intersectionTop.x - intersectionRect.x;
                intersectionRect.height = intersectionLeft.y - intersectionRect.y;

                return true;
            } else if( containsTopRight ) {
                Point<ValueType> intersectionTop;

                if( !other.leftLine().getIntersection( topLine(), intersectionTop ) ) {
                    return false;
                }

                Point<ValueType> intersectionRight;

                if( !other.bottomLine().getIntersection( rightLine(), intersectionRight ) ) {
                    return false;
                }

                intersectionRect.x = intersectionTop.x;
                intersectionRect.y = intersectionTop.y;

                intersectionRect.width = intersectionRight.x - intersectionRect.x;
                intersectionRect.height = intersectionRight.y - intersectionRect.y;

                return true;
            } else if( containsBottomLeft ) {
                Point<ValueType> intersectionLeft;

                if( !other.topLine().getIntersection( leftLine(), intersectionLeft ) ) {
                    return false;
                }

                Point<ValueType> intersectionBottom;

                if( !other.rightLine().getIntersection( bottomLine(), intersectionBottom ) ) {
                    return false;
                }

                intersectionRect.x = intersectionLeft.x;
                intersectionRect.y = intersectionLeft.y;

                intersectionRect.width = intersectionBottom.x - intersectionRect.x;
                intersectionRect.height = bottomLeft().y - intersectionRect.y;

                return true;
            } else if( containsBottomRight ) {
                Point<ValueType> intersectionRight;

                if( !other.topLine().getIntersection( rightLine(), intersectionRight ) ) {
                    return false;
                }

                Point<ValueType> intersectionBottom;

                if( !other.leftLine().getIntersection( bottomLine(), intersectionBottom ) ) {
                    return false;
                }

                intersectionRect.x = intersectionBottom.x;
                intersectionRect.y = intersectionRight.y;

                intersectionRect.width = intersectionRight.x - intersectionRect.x;
                intersectionRect.height = bottomRight().y - intersectionRect.y;

                return true;
            }

            return false;
        } else if( containmentCount == 2 ) {
            if( other.contains( topLine() ) ) {

                Point<ValueType> intersectionRight;

                if( !other.bottomLine().getIntersection( rightLine(), intersectionRight ) ) {
                    return false;
                }

                intersectionRect.x = topLeft().x;
                intersectionRect.y = topLeft().y;

                intersectionRect.width = intersectionRight.x - intersectionRect.x;
                intersectionRect.height = intersectionRight.y - intersectionRect.y;

                return true;
            } else if( other.contains( rightLine() ) ) {

                Point<ValueType> intersectionTop;

                if( !other.leftLine().getIntersection( topLine(), intersectionTop ) ) {
                    return false;
                }

                intersectionRect.x = intersectionTop.x;
                intersectionRect.y = intersectionTop.y;

                intersectionRect.width = bottomRight().x - intersectionRect.x;
                intersectionRect.height = bottomRight().y - intersectionRect.y;

                return true;
            } else if( other.contains( leftLine() ) ) {

                Point<ValueType> intersectionBottom;

                if( !other.rightLine().getIntersection( bottomLine(), intersectionBottom ) ) {
                    return false;
                }

                intersectionRect.x = topLeft().x;
                intersectionRect.y = topLeft().y;

                intersectionRect.width = intersectionBottom.x - intersectionRect.x;
                intersectionRect.height = intersectionBottom.y - intersectionRect.y;

                return true;
            } else if( other.contains( bottomLine() ) ) {

                Point<ValueType> intersectionLeft;

                if( !other.topLine().getIntersection( leftLine(), intersectionLeft ) ) {
                    return false;
                }

                intersectionRect.x = intersectionLeft.x;
                intersectionRect.y = intersectionLeft.y;

                intersectionRect.width = bottomRight().x - intersectionRect.x;
                intersectionRect.height = bottomRight().y - intersectionRect.y;

                return true;
            }

            return false;
        }

        return false;

    }

    Line<ValueType>  topLine() const {
        return Line<ValueType>(
                   topLeft(),
                   topRight()
               );
    }
    Line<ValueType>  bottomLine() const {
        return Line<ValueType>(
                   bottomLeft(),
                   bottomRight()
               );
    }
    Line<ValueType>  leftLine() const {
        return Line<ValueType>(
                   topLeft(),
                   bottomLeft()
               );
    }
    Line<ValueType>  rightLine() const {
        return Line<ValueType>(
                   topRight(),
                   bottomRight()
               );
    }

    Point<ValueType> origin() const {
        return topLeft();
    }
    Point<ValueType> topLeft() const {
        return Point<ValueType>(
                   x,
                   y
               );
    }
    Point<ValueType> topRight() const {
        return Point<ValueType>(
                   x + width,
                   y
               );
    }
    Point<ValueType> bottomLeft() const {
        return Point<ValueType>(
                   x,
                   y + height
               );
    }
    Point<ValueType> bottomRight() const {
        return Point<ValueType>(
                   x + width,
                   y + height
               );
    }

    ValueType area() const {
        return width * height;
    }
};
typedef Rect<float>             Rect32F;
typedef Rect<double>            Rect64F;
typedef Rect<char>              Rect8I;
typedef Rect<unsigned char>     Rect8UI;
typedef Rect<short>             Rect16I;
typedef Rect<unsigned short>    Rect16UI;
typedef Rect<int>               Rect32I;
typedef Rect<unsigned int>      Rect32UI;
typedef Rect<long long>         Rect64I;
typedef Rect<unsigned long long> Rect64UI;

/// impl: bitmap, bitmapinfo, bitmapview


/**
    \class      BitmapInfo
    \brief
            A structure, which contains all meta information for a bitmap. Can also act as a template
            for a Bitmap's creation(format, dimensions).
*/
class LIBCOMMON_API BitmapInfo {
    public:
        /**
            \fn     BitmapInfo
            \brief
                Constructs a new BitmapInfo using the specified parameters.
        */
        explicit BitmapInfo( const libgraphics::Format& format, const libcommon::SizeType& width = 0, const libcommon::SizeType& height = 0 );
        BitmapInfo( const BitmapInfo& rhs );

        /**
            \fn     width
            \brief
                Returns the width of the hypothetical Bitmap plane.
        */
        int width() const;

        /**
            \fn     height
            \brief
                Returns the height of the hypothetical Bitmap plane.
        */
        int height() const;

        /**
            \fn     format
            \brief
                Returns the format of the hypothetical Bitmap plane.
        */
        const Format& format() const;

        /**
            \fn     formatByteSize
            \brief
                Returns the format's pixel byte size of the hypothetical Bitmap plane.
        */
        const libcommon::SizeType& formatByteSize() const;

        /**
            \fn     pixelCount
            \brief
                Returns the number of pixels of the hypothetical Bitmap plane.
        */
        libcommon::SizeType pixelCount() const;

        /**
            \fn     channels
            \brief
                Returns the number of channels of the hypothetical Bitmap plane.
        */
        const libcommon::SizeType& channels() const;

        /**
            \fn     pixelSize
            \brief
                Returns the size of a pixel.
        */
        const libcommon::SizeType& pixelSize() const;

        /// Setters.
        void setFormat( const libgraphics::Format& format );
        void setWidth( const libcommon::SizeType& width );
        void setHeight( const libcommon::SizeType& height );
        void setSize( const libcommon::SizeType& width, const libcommon::SizeType& height );

        /**
            \fn reset
            \brief
                Resets the complete internal state.
        */
        void reset( const libgraphics::Format& format, const libcommon::SizeType& width = 0, const libcommon::SizeType& height = 0 );
    protected:
        libgraphics::Format         m_Format;
        libcommon::SizeType         m_Width;
        libcommon::SizeType         m_Height;
};

/// Forward decl
class Bitmap;
template < class _t_format >
class BitmapView;

struct take_ownership_t {};


/**
    \class      Bitmap
    \brief
        The Bitmap class is a basic management class for single-layer Pixel buffers with generic
        formats.

        The Bitmap class has no concept of pixels. Its basically an extended Buffer class with allocator capability
        for two dimensional planes of generic types.
*/
class LIBCOMMON_API Bitmap {
    public:
        /**
            \fn     Bitmap()
            \brief
                Various types of constructors which basically just call different versions of reset.

                Initializes the state of the Bitmap instance. Bitmap() creates an invalid bitmap state with
                no accessible buffer.
        */
        Bitmap();
        Bitmap( const libgraphics::Format& format, const libcommon::UInt64& width, const libcommon::UInt64& height );
        Bitmap( const libgraphics::Format& format, const libcommon::UInt64& width, const libcommon::UInt64& height, void* data );

        explicit Bitmap( libgraphics::StdDynamicPoolAllocator* allocator );
        Bitmap( libgraphics::StdDynamicPoolAllocator* allocator, const libgraphics::Format& format, const libcommon::UInt64& width, const libcommon::UInt64& height );
        Bitmap( libgraphics::StdDynamicPoolAllocator* allocator, const libgraphics::Format& format, const libcommon::UInt64& width, const libcommon::UInt64& height, void* data );

        explicit Bitmap( const BitmapInfo& info );
        Bitmap( const Bitmap& other );

        /**
            \fn Bitmap
            \brief
                Instead of allocating a new buffer, this constructor uses the specified data buffer and takes over it's ownership.
        */
        Bitmap( take_ownership_t, const libgraphics::Format& format, const libcommon::UInt64& width, const libcommon::UInt64& height, void* data );


        /// destr. / basically reset
        ~Bitmap();

        /**
            \fn     swap
            \brief
                Swaps all internal properties with rhs.
        */
        void swap( Bitmap&& rhs );

        /**
            \fn     operator =
            \brief
                    Copies data from the specified rhs Bitmap.
        */
        Bitmap& operator = ( const Bitmap& rhs );

        /**
            \fn     clone
            \brief
                Clones the current Bitmap's state and returns the instance.
        */
        libcommon::SharedPtr<Bitmap>    clone() const;

        /**
            \fn     validPixel
            \brief
                Checks if the specified vector (x,y) points to a valid pixel and thus
                is directly accessible.
        */
        libcommon::Boolean validPixel( const libcommon::UInt64 x, const libcommon::UInt64 y );

        /// Simple comparison operation.
        bool operator == ( const BitmapInfo& info ) const;
        bool operator != ( const BitmapInfo& info ) const;
        bool operator == ( const Bitmap& info ) const;
        bool operator != ( const Bitmap& info ) const;

        /**
            \fn         copy
            \brief
                        Copies parts of the specified source buffer into the internal Bitmap image plane.

                        Compatible formats are assumed.
        */
        bool copy( const Bitmap* source, Rect32I sourceRect, const int destinationX, const int destinationY );
        bool copy( const void* source, const int x, const int y, const int width, const int height );
        bool copy( const void* source, Rect32I sourceRect, const int destinationX, const int destinationY );
        bool copy( const void* source, Rect32I sourceRect, size_t sourcePlaneWidth, size_t sourcePlaneHeight, const int destinationX, const int destinationY );

        /**
            \fn         copyChannel
            \brief      Copies  a specified source channel into a destination channel. This routine copies a specified segment of data from the supplied buffer to the
                        internal Bitmap image plane.

                        Compatible channel formats are assumed.
        */
        bool copyChannel( size_t sourceChannelIndex, size_t destinationChannelIndex, const Bitmap* source, Rect32I sourceRect, const int destinationX, const int destinationY );
        bool copyChannel( size_t sourceChannelIndex, size_t destinationChannelIndex, const void* source, const int x, const int y, const int width, const int height );
        bool copyChannel( size_t sourceChannelIndex, size_t destinationChannelIndex, const void* source, Rect32I sourceRect, const int destinationX, const int destinationY );
        bool copyChannel( size_t sourceChannelIndex, size_t destinationChannelIndex, const void* source, Rect32I sourceRect, size_t sourcePlaneWidth, size_t, const int destinationX, const int destinationY );

        /**
            \fn         write
            \brief      Copies a specified rectangle of the current Bitmap image plane to the specified
                        buffer.

                        Returns true, if the copy process was successful.
        */
        bool write( void* destination, const Rect64UI& dstRect, const Rect64UI& srcRect );
        bool write( void* destination, const Rect64UI& dstRect, size_t dstPlaneWidth, size_t dstPlaneHeight, const Rect64UI& srcRect );

        bool write( Bitmap* destination, Rect32I sourceRect, const libcommon::SizeType& destinationX, const libcommon::SizeType& destinationY );
        bool write( void* destination, const int x, const int y, const int width, const int height );
        bool write( void* destination, const int dstWidth, const int dstHeight, const int x, const int y, const int width, const int height );

        /**
            \fn         create
            \brief
                Constructs a new Bitmap instance using the given paramters.

                May return nothing, if the supplied parameters are invalid(e.g. invalid data buffer. ).
        */
        static libcommon::Maybe< libcommon::SharedPtr<Bitmap> > create( const libgraphics::Format& format, const libcommon::UInt64& width, const libcommon::UInt64& height );
        static libcommon::Maybe< libcommon::SharedPtr<Bitmap> > create( const libgraphics::Format& format, const libcommon::UInt64& width, const libcommon::UInt64& height, void* data );
        static libcommon::Maybe< libcommon::SharedPtr<Bitmap> > create( const BitmapInfo& info );

        /**
            \fn         reset
            \brief
                    Resets the internal state and creates a new one. Uses the specified allocator to do so.
        */
        bool reset( libgraphics::StdDynamicPoolAllocator* allocator, const libgraphics::Format& format, const int width, const int height, void* data );
        bool reset( libgraphics::StdDynamicPoolAllocator* allocator, const libgraphics::Format& format, const int width, const int height );
        bool reset( libgraphics::StdDynamicPoolAllocator* allocator, const BitmapInfo& info );

        /**
            \fn         reset
            \brief
                    Resets the internal state: Deletes the internal Bitmap buffer and may create a new
                    Bitmap plane.

                    Uses the currently associated internal allocator to create new objects.
        */
        bool reset( const libgraphics::Format& format, const libcommon::UInt64& width, const libcommon::UInt64& height, void* data );
        bool reset( const libgraphics::Format& format, const libcommon::UInt64& width, const libcommon::UInt64& height );
        bool reset( const BitmapInfo& info );
        bool reset( const Bitmap& other );
        bool reset();

        /**
            \fn discardAlphaChannel
            \since 1.0
            \brief Discards the data of the alpha channel of the specified bitmap
                object. The bitmap transforms itself to the corresponding format without alpha plane.
                    ( RGBA8 => RGB8,
                      RGBA16 => RGB16,
                      etc. )
        */
        bool discardAlphaChannel();

        /**
            \fn discardChannel
            \since 1.0
            \brief Discards the specified data plane and sets the specified format.
        */
        bool discardChannel( size_t channelIndex, const libgraphics::Format& format );

        /**
            \fn transformFormat
            \since 1.0
            \brief Tries to transform the current pixel format to the specified
                one. Returns true, if the operation was successfully executed. Thus the bitmap
                acquires the new format.
        */
        bool transformFormat( const libgraphics::Format& destinationFormat );

        /**
            \fn toFormat
            \since 1.0
            \brief Constructs a copy of this bitmap which is formatted in the specified
                format. May return a null-pointer.
        */
        libgraphics::Bitmap* toFormat(
            const libgraphics::Format& destinationFormat
        );
        libgraphics::Bitmap* toFormat(
            const libgraphics::Format&  destinationFormat,
            const libgraphics::Rect32I& area
        );

        /**
            \fn empty
            \brief
                Returns true, if the Bitmap instance has no valid
                state and thus holds no data.
        */
        bool empty() const;

        /**
            \fn buffer
            \brief
                Returns a direct pointer to the internal Bitmap buffer.
        */
        void* buffer();
        const void* buffer() const;

        /**
            \fn size
            \brief
                Returns the dimensions(width,height) of the current
                Bitmap as rectangle. (x,y) are always zero.
        */
        libgraphics::Rect32I    size() const;

        /**
            \fn formatByteSize
            \brief
                Returns the pixel-size of the internal Bitmap format.

                If empty() the result is invalid.
        */
        const libcommon::SizeType&  formatByteSize() const;

        /**
            \fn pixelCount
            \brief
                Returns the number of pixels this Bitmap
                holds.

                Basically size.width * size.height.
        */
        libcommon::SizeType  pixelCount() const;

        /**
            \fn channels
            \brief
                Returns the number of channels of
                the internal format.
        */
        const libcommon::SizeType&  channels() const;

        /**
            \fn width
            \brief
                Returns the width of the internal Bitmap
                plane.
        */
        int width() const;

        /**
            \fn height
            \brief
                Returns the height of the internal Bitmap
                plane.
        */
        int height() const;

        /**
            \fn format
            \brief
                Returns the internal Bitmap's format.
        */
        const libgraphics::Format&  format() const;

        /**
            \fn info
            \brief
                Returns the internal Meta-information(dimensions,format,etc.)
                as BitmapInfo structure.
        */
        BitmapInfo info() const;

        /**
            \fn manualLock
            \brief
                Returns a LockGuard which guards the internal
                Bitmap mutex.

                The Bitmap's mutex can be used for synchronization
                across multiple threads.
        */
        libcommon::LockGuard    manualLock();

        /**
            \fn synchronize
            \brief
                Synchronizes the Bitmap with all other accessing threads.

                Simply creates a LockGuard and waits till access is granted.
        */
        void    synchronize();

        /**
            \fn view
            \brief
                Creates a BitmapView with access to the current
                instance.
        */
        template < class _t_format >
        BitmapView<_t_format>   view() {
            return BitmapView<_t_format>( libcommon::WeakRef<Bitmap>( this ) );
        }

        /**
            \fn containsAllocator
            \brief
                Returns true, if the current Bitmap is associated with an
                allocator.
        */
        bool containsAllocator() const;

        /**
            \fn allocator
            \brief
                Returns the internal allocator or nullptr.
        */
        libgraphics::StdDynamicPoolAllocator*   allocator() const;

        /**
            \fn assignAllocator
            \brief
                Assigns a new allocator to the internal Bitmap state. Copies the internal state from the initial allocator/raw buffer
                to the new one.

                This routine may be memory and processor intensive.
        */
        bool assignAllocator(
            libgraphics::StdDynamicPoolAllocator* _allocator
        );

        /**
            \fn clearAllocator
            \brief Sets the internal allocator to nullptr. Moves the current bitmap buffer
                to a newly allocated blob.
        */
        void clearAllocator();

        /**
            \fn saveRawData
            \brief
                Writes the current Bitmaps raw data to the specified
                file-path(may overwrite existing files).

                Returns true, if the operation was successful.
        */
        bool saveRawData( const std::string& _path );

        /**
            \fn loadRawData
            \brief
                Resets the internal state and tries to load a bitmap from the
                raw-data blob at the specified file-path.

                The specified data has to be formatted in the specified format.
        */
        bool loadRawData( const libgraphics::Format& _format, size_t width, size_t height, const std::string& _path );

    protected:
        void*                       m_BitmapBuffer;
        libcommon::SizeType         m_BitmapHeight;
        libcommon::SizeType         m_BitmapWidth;
        libgraphics::Format         m_Format;
        libcommon::RecursiveMutex   m_AccessLock;

        libgraphics::StdDynamicPoolAllocator*                                   m_InternalAllocator;
        libcommon::SharedPtr<libgraphics::StdDynamicPoolAllocator::Blob>        m_InternalMemoryBlob;
};

struct BitmapException : std::runtime_error {
    explicit BitmapException( std::string msg = "unknown bitmap error" ) : std::runtime_error( msg ) {}
    virtual ~BitmapException() {}
};

struct InvalidBufferBitmapException : BitmapException {
    explicit InvalidBufferBitmapException( std::string msg = "Tried to access buffer at invalid bitmap position." ) :
        BitmapException( msg ) {}
    virtual ~InvalidBufferBitmapException() {}
};

template < class _t_format >
class BitmapView {
    public:
        typedef _t_format                   FormatType;
        typedef typename FormatType::t      PixelType;

        explicit BitmapView( const libcommon::WeakRef<Bitmap>& bitmap ) : m_Bitmap( bitmap ), m_LockGuard( m_Bitmap->manualLock() ) {}
        BitmapView( BitmapView&& rhs ) : m_LockGuard( NULL ) {
            std::swap(
                rhs.m_Bitmap,
                m_Bitmap
            );
            std::swap(
                rhs.m_LockGuard,
                m_LockGuard
            );
        }

        libcommon::Maybe<const PixelType&>    at( const libcommon::UInt64 x, const libcommon::UInt64 y ) const {
            const PixelType*  pixels = ( const PixelType* )this->m_Bitmap->buffer();

            if( pixels != NULL ) {

                if( ( m_Bitmap->width() >= x ) && ( m_Bitmap->height() >= y ) ) {
                    return just( pixels[ y * m_Bitmap->width() + x ] );
                }

                return libcommon::nothing();

            }

            return libcommon::nothing();
        }
        libcommon::Maybe<PixelType&>    at( const libcommon::UInt64 x, const libcommon::UInt64 y ) {
            PixelType*  pixels = ( PixelType* )this->m_Bitmap->buffer();

            if( pixels != NULL ) {

                if( ( m_Bitmap->width() >= x ) && ( m_Bitmap->height() >= y ) ) {
                    return just( pixels[ y * m_Bitmap->width() + x ] );
                }

                return libcommon::nothing();

            }

            return libcommon::nothing();
        }
        const PixelType&    unsafeAt( const libcommon::UInt64 x, const libcommon::UInt64 y ) const {
            const PixelType*  pixels = ( const PixelType* )this->m_Bitmap->buffer();

            if( pixels != NULL ) {

                if( ( m_Bitmap->width() >= x ) && ( m_Bitmap->height() >= y ) ) {
                    return pixels[ y * m_Bitmap->width() + x ];
                }

            }

            throw InvalidBufferBitmapException();
        }
        PixelType&    unsafeAt( const libcommon::UInt64 x, const libcommon::UInt64 y ) {
            PixelType*  pixels = ( PixelType* )this->m_Bitmap->buffer();

            if( pixels != NULL ) {

                if( ( m_Bitmap->width() >= x ) && ( m_Bitmap->height() >= y ) ) {
                    return pixels[ y * m_Bitmap->width() + x ];
                }

            }

            throw InvalidBufferBitmapException();
        }
    protected:
        libcommon::WeakRef<Bitmap>  m_Bitmap;
        libcommon::LockGuard        m_LockGuard;
};

template < class _t_pixel_type >
static inline libgraphics::BasicPixel<_t_pixel_type, 1> makePixelValue1(
    const _t_pixel_type& v0
) {
    libgraphics::BasicPixel<_t_pixel_type, 1>    bp;
    bp.Values[0] = v0;
    return bp;
}
template < class _t_pixel_type >
static inline libgraphics::BasicPixel<_t_pixel_type, 2> makePixelValue2(
    const _t_pixel_type& v0,
    const _t_pixel_type& v1
) {
    libgraphics::BasicPixel<_t_pixel_type, 2>    bp;
    bp.Values[0] = v0; bp.Values[1] = v1;
    return bp;
}
template < class _t_pixel_type >
static inline libgraphics::BasicPixel<_t_pixel_type, 3> makePixelValue3(
    const _t_pixel_type& v0,
    const _t_pixel_type& v1,
    const _t_pixel_type& v2
) {
    libgraphics::BasicPixel<_t_pixel_type, 3>    bp;
    bp.Values[0] = v0; bp.Values[1] = v1; bp.Values[2] = v2;
    return bp;
}
template < class _t_pixel_type >
static inline libgraphics::BasicPixel<_t_pixel_type, 4> makePixelValue4(
    const _t_pixel_type& v0,
    const _t_pixel_type& v1,
    const _t_pixel_type& v2,
    const _t_pixel_type& v3
) {
    libgraphics::BasicPixel<_t_pixel_type, 4>    bp;
    bp.Values[0] = v0; bp.Values[1] = v1; bp.Values[2] = v2; bp.Values[3] = v3;
    return bp;
}


static inline libgraphics::formats::ARGB8::t   makeRGBA8(
    const libgraphics::formats::ARGB8::t::PixelType& r,
    const libgraphics::formats::ARGB8::t::PixelType& g,
    const libgraphics::formats::ARGB8::t::PixelType& b,
    const libgraphics::formats::ARGB8::t::PixelType& a
) {
    typedef std::remove_const< std::remove_reference< decltype( r ) >::type >::type pixel_type;
    return makePixelValue4<pixel_type>(
               r, g, b, a
           );
}

static inline libgraphics::formats::ARGB16::t   makeRGBA16(
    const libgraphics::formats::ARGB16::t::PixelType& r,
    const libgraphics::formats::ARGB16::t::PixelType& g,
    const libgraphics::formats::ARGB16::t::PixelType& b,
    const libgraphics::formats::ARGB16::t::PixelType& a
) {
    typedef std::remove_const< std::remove_reference< decltype( r ) >::type >::type pixel_type;
    return makePixelValue4<pixel_type>(
               r, g, b, a
           );
}
static inline libgraphics::formats::ARGB32::t   makeRGBA32(
    const libgraphics::formats::ARGB32::t::PixelType& r,
    const libgraphics::formats::ARGB32::t::PixelType& g,
    const libgraphics::formats::ARGB32::t::PixelType& b,
    const libgraphics::formats::ARGB32::t::PixelType& a
) {
    typedef std::remove_const< std::remove_reference< decltype( r ) >::type >::type pixel_type;
    return makePixelValue4<pixel_type>(
               r, g, b, a
           );
}
static inline libgraphics::formats::ARGB32F::t   makeRGBA32F(
    const libgraphics::formats::ARGB32F::t::PixelType& r,
    const libgraphics::formats::ARGB32F::t::PixelType& g,
    const libgraphics::formats::ARGB32F::t::PixelType& b,
    const libgraphics::formats::ARGB32F::t::PixelType& a
) {
    typedef std::remove_const< std::remove_reference< decltype( r ) >::type >::type pixel_type;
    return makePixelValue4<pixel_type>(
               r, g, b, a
           );
}

static inline libgraphics::formats::ARGB8::t   makeARGB8(
    const libgraphics::formats::ARGB8::t::PixelType& a,
    const libgraphics::formats::ARGB8::t::PixelType& r,
    const libgraphics::formats::ARGB8::t::PixelType& g,
    const libgraphics::formats::ARGB8::t::PixelType& b
) {
    typedef std::remove_const< std::remove_reference< decltype( r ) >::type >::type pixel_type;
    return makePixelValue4<pixel_type>(
               a, r, g, b
           );
}

static inline libgraphics::formats::ARGB16::t   makeARGB16(
    const libgraphics::formats::ARGB16::t::PixelType& a,
    const libgraphics::formats::ARGB16::t::PixelType& r,
    const libgraphics::formats::ARGB16::t::PixelType& g,
    const libgraphics::formats::ARGB16::t::PixelType& b
) {
    typedef std::remove_const< std::remove_reference< decltype( r ) >::type >::type pixel_type;
    return makePixelValue4<pixel_type>(
               a, r, g, b
           );
}
static inline libgraphics::formats::ARGB32::t   makeARGB32(
    const libgraphics::formats::ARGB32::t::PixelType& a,
    const libgraphics::formats::ARGB32::t::PixelType& r,
    const libgraphics::formats::ARGB32::t::PixelType& g,
    const libgraphics::formats::ARGB32::t::PixelType& b
) {
    typedef std::remove_const< std::remove_reference< decltype( r ) >::type >::type pixel_type;
    return makePixelValue4<pixel_type>(
               a, r, g, b
           );
}
static inline libgraphics::formats::ARGB32F::t   makeARGB32F(
    const libgraphics::formats::ARGB32F::t::PixelType& a,
    const libgraphics::formats::ARGB32F::t::PixelType& r,
    const libgraphics::formats::ARGB32F::t::PixelType& g,
    const libgraphics::formats::ARGB32F::t::PixelType& b
) {
    typedef std::remove_const< std::remove_reference< decltype( r ) >::type >::type pixel_type;
    return makePixelValue4<pixel_type>(
               a, r, g, b
           );
}

static inline libgraphics::formats::RGB8::t   makeRGB8(
    const libgraphics::formats::RGB8::t::PixelType& r,
    const libgraphics::formats::RGB8::t::PixelType& g,
    const libgraphics::formats::RGB8::t::PixelType& b
) {
    typedef std::remove_const< std::remove_reference< decltype( r ) >::type >::type pixel_type;
    return makePixelValue3<pixel_type>(
               r, g, b
           );
}
static inline libgraphics::formats::RGB16::t   makeRGB16(
    const libgraphics::formats::RGB16::t::PixelType& r,
    const libgraphics::formats::RGB16::t::PixelType& g,
    const libgraphics::formats::RGB16::t::PixelType& b
) {
    typedef std::remove_const< std::remove_reference< decltype( r ) >::type >::type pixel_type;
    return makePixelValue3<pixel_type>(
               r, g, b
           );
}
static inline libgraphics::formats::RGB32::t   makeRGB32(
    const libgraphics::formats::RGB32::t::PixelType& r,
    const libgraphics::formats::RGB32::t::PixelType& g,
    const libgraphics::formats::RGB32::t::PixelType& b
) {
    typedef std::remove_const< std::remove_reference< decltype( r ) >::type >::type pixel_type;
    return makePixelValue3<pixel_type>(
               r, g, b
           );
}
static inline libgraphics::formats::RGB32F::t   makeRGB32F(
    const libgraphics::formats::RGB32F::t::PixelType& b,
    const libgraphics::formats::RGB32F::t::PixelType& g,
    const libgraphics::formats::RGB32F::t::PixelType& r
) {
    typedef std::remove_const< std::remove_reference< decltype( r ) >::type >::type pixel_type;
    return makePixelValue3<pixel_type>(
               r, g, b
           );
}


static inline libgraphics::formats::RGB8::t   makeBGR8(
    const libgraphics::formats::RGB8::t::PixelType& b,
    const libgraphics::formats::RGB8::t::PixelType& g,
    const libgraphics::formats::RGB8::t::PixelType& r
) {
    typedef std::remove_const< std::remove_reference< decltype( r ) >::type >::type pixel_type;
    return makePixelValue3<pixel_type>(
               b, g, r
           );
}
static inline libgraphics::formats::RGB16::t   makeBGR16(
    const libgraphics::formats::RGB16::t::PixelType& b,
    const libgraphics::formats::RGB16::t::PixelType& g,
    const libgraphics::formats::RGB16::t::PixelType& r
) {
    typedef std::remove_const< std::remove_reference< decltype( r ) >::type >::type pixel_type;
    return makePixelValue3<pixel_type>(
               b, g, r
           );
}
static inline libgraphics::formats::RGB32::t   makeBGR32(
    const libgraphics::formats::RGB32::t::PixelType& b,
    const libgraphics::formats::RGB32::t::PixelType& g,
    const libgraphics::formats::RGB32::t::PixelType& r
) {
    typedef std::remove_const< std::remove_reference< decltype( r ) >::type >::type pixel_type;
    return makePixelValue3<pixel_type>(
               b, g, r
           );
}
static inline libgraphics::formats::RGB32F::t   makeBGR32F(
    const libgraphics::formats::RGB32F::t::PixelType& b,
    const libgraphics::formats::RGB32F::t::PixelType& g,
    const libgraphics::formats::RGB32F::t::PixelType& r
) {
    typedef std::remove_const< std::remove_reference< decltype( r ) >::type >::type pixel_type;
    return makePixelValue3<pixel_type>(
               b, g, r
           );
}
static inline libgraphics::formats::Mono8::t    makeMono8(
    const libgraphics::formats::Mono8::t::PixelType& val
)  {
    typedef std::remove_const< std::remove_reference< decltype( val ) >::type >::type pixel_type;
    return makePixelValue1<pixel_type>(
               val
           );
}
static inline libgraphics::formats::Mono16::t    makeMono16(
    const libgraphics::formats::Mono16::t::PixelType& val
)  {
    typedef std::remove_const< std::remove_reference< decltype( val ) >::type >::type pixel_type;
    return makePixelValue1<pixel_type>(
               val
           );
}
static inline libgraphics::formats::Mono16S::t    makeMono16S(
    const libgraphics::formats::Mono16S::t::PixelType& val
)  {
    typedef std::remove_const< std::remove_reference< decltype( val ) >::type >::type pixel_type;
    return makePixelValue1<pixel_type>(
               val
           );
}
static inline libgraphics::formats::Mono32::t    makeMono16(
    const libgraphics::formats::Mono32::t::PixelType& val
)  {
    typedef std::remove_const< std::remove_reference< decltype( val ) >::type >::type pixel_type;
    return makePixelValue1<pixel_type>(
               val
           );
}
static inline libgraphics::formats::Mono32S::t    makeMono16(
    const libgraphics::formats::Mono32S::t::PixelType& val
)  {
    typedef std::remove_const< std::remove_reference< decltype( val ) >::type >::type pixel_type;
    return makePixelValue1<pixel_type>(
               val
           );
}

/// conv
bool transformFormat(
    libgraphics::Bitmap* dst,
    libgraphics::Bitmap* src,
    const libgraphics::Format&  dstFormat
);
bool transformFormat(
    libgraphics::Bitmap* dst,
    libgraphics::Bitmap* src,
    const libgraphics::Format&  dstFormat,
    const libgraphics::Rect32I& dstArea,
    const libgraphics::Rect32I& srcArea
);
bool transformFormat(
    void* dstBuf,
    const void* srcBuf,
    const libgraphics::Format& dstFormat,
    const libgraphics::Format& srcFormat,
    const libgraphics::Rect32I& dstArea,
    const libgraphics::Rect32I& srcArea,
    size_t dstWidth,
    size_t srcWidth
);

}
