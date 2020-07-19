#ifndef HELPERS_H
#define HELPERS_H

#include <utils/colorspaces.hpp>

using namespace blacksilk::graphics;

bool fuzzyCompare( float a, float b ) {
    return fabs( a - b ) < 0.001;
}

//! \brief Compares two floats to 4 digits\n
template<class T>
bool areEqual( const ColorRGB<T>& a, const ColorRGB<T>& b )
{
    if( ! fuzzyCompare((float)a.r, (float)b.r) ) return false;
    if( ! fuzzyCompare((float)a.g, (float)b.g) ) return false;
    if( ! fuzzyCompare((float)a.b, (float)b.b) ) return false;

    return true;
}

template<class T>
bool areEqual( const BGR<T>& a, const BGR<T>& b )
{
    if( ! fuzzyCompare((float)a.r, (float)b.r) ) return false;
    if( ! fuzzyCompare((float)a.g, (float)b.g) ) return false;
    if( ! fuzzyCompare((float)a.b, (float)b.b) ) return false;

    return true;
}

#endif // HELPERS_H
