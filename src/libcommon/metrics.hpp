#pragma once

namespace libcommon {
namespace metrics {
/// frequency
template < class _t_any >
static double  toHertz( const _t_any& val ) {
    return static_cast<double>( val );
}
template < class _t_any >
static double   toKilohertz( const _t_any& val ) {
    return static_cast<double>( val / 1000.0 );
}
template < class _t_any >
static double   toMegahertz( const _t_any& val ) {
    return static_cast<double>( val / ( 1000.0 * 1000.0 ) );
}
template < class _t_any >
static double   toGigahertz( const _t_any& val ) {
    return static_cast<double>( val / ( 1000.0 * 1000.0 * 1000.0 ) );
}

template < class _t_any >
static _t_any   hertz( const _t_any& val ) {
    return val;
}
template < class _t_any >
static _t_any   kilohertz( const _t_any& val ) {
    return ( val * 1000 );
}
template < class _t_any >
static _t_any   megahertz( const _t_any& val ) {
    return ( val * 1000 * 1000 );
}
template < class _t_any >
static _t_any   gigahertz( const _t_any& val ) {
    return ( val * 1000 * 1000 * 1000 );
}

/// memory units
template < class _t_any >
static double  toBytes( const _t_any& val ) {
    return static_cast<double>( val );
}
template < class _t_any >
static double   toKilobytes( const _t_any& val ) {
    return static_cast<double>( val / 1024.0 );
}
template < class _t_any >
static double   toMegabytes( const _t_any& val ) {
    return static_cast<double>( val / ( 1024.0 * 1024.0 ) );
}
template < class _t_any >
static double   toGigabytes( const _t_any& val ) {
    return static_cast<double>( val / ( 1024.0 * 1024.0 * 1024.0 ) );
}
template < class _t_any >
static double   toTerrabytes( const _t_any& val ) {
    return static_cast<double>( val / ( 1024.0 * 1024.0 * 1024.0 * 1024.0 ) );
}

template < class _t_any >
static _t_any  bytes( const _t_any& val ) {
    return val;
}
template < class _t_any >
static _t_any  kilobytes( const _t_any& val ) {
    return val * 1024;
}
template < class _t_any >
static _t_any  megabytes( const _t_any& val ) {
    return val * 1024 * 1024;
}
template < class _t_any >
static _t_any  gigabytes( const _t_any& val ) {
    return val * 1024 * 1024 * 1024;
}
template < class _t_any >
static _t_any  terrabytes( const _t_any& val ) {
    return val * 1024 * 1024 * 1024 * 1024;
}

}

}
