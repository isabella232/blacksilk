#pragma once

#include <qglobal.h>

#define CONVERSION_RED_PART   0.333333333333
#define CONVERSION_GREEN_PART 0.333333333334
#define CONVERSION_BLUE_PART  0.333333333333

namespace blacksilk {
namespace graphics {

template <class T>
class ColorRGB {
    public:
        T r;
        T g;
        T b;

        ColorRGB( T rIn = 0, T gIn = 0, T bIn = 0 ) : r( rIn ), g( gIn ), b( bIn ) {}

        float lumaFromRGB() const;
        void lumaToRGB( float luma );

        float sumFromRGB() const;
        void sumToRGB( float sum );
};

struct YUV {
    float y;
    float u;
    float v;
};

template<class T>
bool areEqual( ColorRGB<T> rgb, YUV& yuv ) {
    YUV yuv2;
    RGB2YUV( rgb, yuv2 );

    if( ! qFuzzyCompare( yuv2.y, yuv.y ) ) {
        return false;
    }

    if( ! qFuzzyCompare( yuv2.u, yuv.u ) ) {
        return false;
    }

    if( ! qFuzzyCompare( yuv2.v, yuv.v ) ) {
        return false;
    }

    return true;
}

template <class T>
class BGR {
    public:
        T b;
        T g;
        T r;

        BGR( T bIn = 0, T gIn = 0, T rIn = 0 ) : b( bIn ), g( gIn ), r( rIn ) {}
};

template <class T>
int CLAMP255( T in ) {
    if( in < 0 ) {
        return 0;
    }

    if( in > 255 ) {
        return 255;
    } else {
        return in;
    }
}

template <class T, int lower, int upper>
T CLAMP( T in ) {
    if( in < lower ) {
        return lower;
    }

    if( in > upper ) {
        return upper;
    }

    return in;
}

template <class T>
int CLAMP65535( T in ) {
    if( in < 0 ) {
        return 0;
    }

    if( in > 65535 ) {
        return 65535;
    } else {
        return in;
    }
}

template <class T>
float CLAMP1f( T in ) {
    if( in > 1.0 ) {
        return 1.0;
    } else if( in < 0.0 ) {
        return 0.0;
    } else {
        return in;
    }
}

template <class T>
void RGB2YUV( const ColorRGB<T>& rgb, YUV& yuv ) {
    yuv.y = CONVERSION_RED_PART   * ( float )rgb.r +
            CONVERSION_GREEN_PART * ( float )rgb.g +
            CONVERSION_BLUE_PART  * ( float )rgb.b;
    yuv.v = ( float )( rgb.r - yuv.y );
    yuv.u = ( float )( rgb.b - yuv.y );
}

template <class T>
void BGR2YUV( const BGR<T>& bgr, YUV& yuv ) {
    yuv.y = CONVERSION_RED_PART   * ( float )bgr.r +
            CONVERSION_GREEN_PART * ( float )bgr.g +
            CONVERSION_BLUE_PART  * ( float )bgr.b;
    yuv.v = ( float )( bgr.r - yuv.y );
    yuv.u = ( float )( bgr.b - yuv.y );
}

template <class T>
void ColorRGB<T>::lumaToRGB( float luma ) {
    YUV yuv;
    RGB2YUV( *this, yuv );
    yuv.y = luma;
    YUV2RGB16U( yuv, *this );
}

template <class T>
void ColorRGB<T>::sumToRGB( float sum ) {
    YUV yuv;
    RGB2YUV( *this, yuv );
    yuv.y = sum / 3.0;
    YUV2RGB16U( yuv, *this );
}

template <class T>
float ColorRGB<T>::sumFromRGB() const {
    float sum = ( float )r +
                ( float )g +
                ( float )b;
    return sum;
}

template <class T>
float ColorRGB<T>::lumaFromRGB() const {
    float luma = CONVERSION_RED_PART   * ( float )this->r +
                 CONVERSION_GREEN_PART * ( float )this->g +
                 CONVERSION_BLUE_PART  * ( float )this->b;
    return luma;
}

template <class T>
void YUV2RGB8( const YUV& yuv, ColorRGB<T>& rgb ) {
    rgb.r = CLAMP255( yuv.v + yuv.y );
    rgb.b = CLAMP255( yuv.u + yuv.y );
    rgb.g = CLAMP255( 0.5 + ( ( yuv.y - CONVERSION_RED_PART * ( float )rgb.r - CONVERSION_BLUE_PART * ( float )rgb.b ) / CONVERSION_GREEN_PART ) );
}

template <class T>
void YUV2BGR8( const YUV& yuv, BGR<T>& bgr ) {
    bgr.r = CLAMP255( yuv.v + yuv.y );
    bgr.b = CLAMP255( yuv.u + yuv.y );
    bgr.g = CLAMP255( 0.5 + ( ( yuv.y - CONVERSION_RED_PART * ( float )bgr.r - CONVERSION_BLUE_PART * ( float )bgr.b ) / CONVERSION_GREEN_PART ) );
}

template <class T>
void YUV2RGB16( const YUV& yuv, ColorRGB<T>& rgb ) {
    rgb.r = CLAMP65535( yuv.v + yuv.y );
    rgb.b = CLAMP65535( yuv.u + yuv.y );
    rgb.g = CLAMP255( 0.5 + ( ( yuv.y - CONVERSION_RED_PART * ( float )rgb.r - CONVERSION_BLUE_PART * ( float )rgb.b ) / CONVERSION_GREEN_PART ) );
}

template <class T>
void YUV2RGB16U( const YUV& yuv, ColorRGB<T>& rgb ) {
    rgb.r = CLAMP < T, -512, 512 > ( yuv.v + yuv.y );
    rgb.b = CLAMP < T, -512, 512 > ( yuv.u + yuv.y );
    rgb.g = CLAMP < T, -512, 512 > ( ( yuv.y - CONVERSION_RED_PART * ( float )rgb.r - CONVERSION_BLUE_PART * ( float )rgb.b ) / CONVERSION_GREEN_PART );
}

template <class T>
void YUV2RGB32( const YUV& yuv, ColorRGB<T>& rgb ) {
    rgb.r = CLAMP1f( yuv.v + yuv.y );
    rgb.b = CLAMP1f( yuv.u + yuv.y );
    rgb.g = CLAMP1f( ( yuv.y - CONVERSION_RED_PART * ( float )rgb.r - CONVERSION_BLUE_PART * ( float )rgb.b ) / CONVERSION_GREEN_PART );
}

}
}
