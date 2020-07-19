#pragma once

#include <assert.h>
#include <libcommon/def.hpp>

/// The following code section implements a standard set of atomics
#if LIBCOMMON_IS_GCC_COMPATIBLE
#   include <mmintrin.h>
#endif

namespace libcommon {
namespace atomics {

typedef     Int32   type32;
typedef     Int64   type64;

#if LIBCOMMON_IS_GCC_COMPATIBLE
template < class _t_any >
LIBCOMMON_PRIVATE_API LIBCOMMON_FORCE_INLINE _t_any  base_add( _t_any volatile* atomic, const _t_any& val ) {
    assert( atomic );

    return __sync_add_and_fetch( atomic, val );
}
template < class _t_any >
LIBCOMMON_PRIVATE_API LIBCOMMON_FORCE_INLINE _t_any  base_sub( _t_any volatile* atomic, const _t_any& val ) {
    assert( atomic );

    return __sync_sub_and_fetch( atomic, val );
}
template < class _t_any >
LIBCOMMON_PRIVATE_API LIBCOMMON_FORCE_INLINE _t_any  base_inc( _t_any volatile* atomic ) {
    assert( atomic );

    return __sync_add_and_fetch( atomic, 1 );
}
template < class _t_any >
LIBCOMMON_PRIVATE_API LIBCOMMON_FORCE_INLINE _t_any  base_dec( _t_any volatile* atomic ) {
    assert( atomic );

    return __sync_sub_and_fetch( atomic, 1 );
}
template < class _t_any >
LIBCOMMON_PRIVATE_API LIBCOMMON_FORCE_INLINE _t_any  base_fetch( _t_any volatile* atomic ) {
    assert( atomic );

    return __sync_add_and_fetch( atomic, 0 );
}
template < class _t_any >
LIBCOMMON_PRIVATE_API LIBCOMMON_FORCE_INLINE bool  base_equal( _t_any volatile* atomic, const _t_any& ref ) {
    assert( atomic );

    return ( base_fetch<_t_any>( atomic ) == ref );
}
template < class _t_any >
LIBCOMMON_PRIVATE_API LIBCOMMON_FORCE_INLINE _t_any    base_assign( _t_any volatile* atomic, const _t_any& val ) {
    assert( atomic );

    return __sync_lock_test_and_set( atomic, val );
}
template < class _t_any >
LIBCOMMON_PRIVATE_API LIBCOMMON_FORCE_INLINE  _t_any    base_exchange( _t_any volatile* atomic, const _t_any& cmp, const _t_any& val ) {
    assert( atomic );

    return __sync_val_compare_and_swap( atomic, cmp, val );
}
#endif


LIBCOMMON_PRIVATE_API     libcommon::Int64        add64( libcommon::Int64 volatile* value, const libcommon::Int64& val );
LIBCOMMON_PRIVATE_API     libcommon::Int64        sub64( libcommon::Int64 volatile* value, const libcommon::Int64& val );
LIBCOMMON_PRIVATE_API     libcommon::Int64        inc64( libcommon::Int64 volatile* value );
LIBCOMMON_PRIVATE_API     libcommon::Int64        dec64( libcommon::Int64 volatile* value );
LIBCOMMON_PRIVATE_API     libcommon::Int64        fetch64( libcommon::Int64 volatile* value );
LIBCOMMON_PRIVATE_API     libcommon::Int64        assign64( libcommon::Int64 volatile* value, const libcommon::Int64& val );
LIBCOMMON_PRIVATE_API     libcommon::Int64        exchange64( libcommon::Int64 volatile* atomic, const libcommon::Int64& cmp, const libcommon::Int64& val );
LIBCOMMON_PRIVATE_API     bool                    equal64( libcommon::Int64 volatile* atomic, const libcommon::Int64& val );

LIBCOMMON_PRIVATE_API     libcommon::Int32        add32( libcommon::Int32 volatile* value, const libcommon::Int32& val );
LIBCOMMON_PRIVATE_API     libcommon::Int32        sub32( libcommon::Int32 volatile* value, const libcommon::Int32& val );
LIBCOMMON_PRIVATE_API     libcommon::Int32        inc32( libcommon::Int32 volatile* value );
LIBCOMMON_PRIVATE_API     libcommon::Int32        dec32( libcommon::Int32 volatile* value );
LIBCOMMON_PRIVATE_API     libcommon::Int32        fetch32( libcommon::Int32 volatile* value );
LIBCOMMON_PRIVATE_API     libcommon::Int32        assign32( libcommon::Int32 volatile* value, const libcommon::Int32& val );
LIBCOMMON_PRIVATE_API     libcommon::Int32        exchange32( libcommon::Int32 volatile* atomic, const libcommon::Int32& cmp, const libcommon::Int32& val );
LIBCOMMON_PRIVATE_API     bool                    equal32( libcommon::Int32 volatile* atomic, const libcommon::Int32& val );

}
}
