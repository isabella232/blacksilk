
#include <libcommon/atomics.hpp>

#if LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_WINDOWS
#   include <Windows.h>
#   include <xmmintrin.h>
#   include <emmintrin.h>
#   include <immintrin.h>
#endif

namespace libcommon {
namespace atomics {

#if LIBCOMMON_COMPILER == LIBCOMMON_COMPILER_MSVC


LIBCOMMON_PRIVATE_API     libcommon::Int64        add64( libcommon::Int64 volatile* value, const libcommon::Int64& val ) {

    assert( value );

    return ::InterlockedAdd64(
               value,
               val
           );

}

LIBCOMMON_PRIVATE_API     libcommon::Int64        sub64( libcommon::Int64 volatile* value, const libcommon::Int64& val ) {

    assert( value );

    return ::InterlockedAdd64(
               value,
               -val
           );

}

LIBCOMMON_PRIVATE_API     libcommon::Int64        inc64( libcommon::Int64 volatile* value ) {

    assert( value );

    return ::InterlockedIncrement64(
               value
           );

}

LIBCOMMON_PRIVATE_API     libcommon::Int64        dec64( libcommon::Int64 volatile* value ) {

    assert( value );

    return ::InterlockedDecrement64(
               value
           );

}

LIBCOMMON_PRIVATE_API     libcommon::Int64        fetch64( libcommon::Int64 volatile* value ) {

    assert( value );

    return ::InterlockedAdd64(
               value,
               0
           );

}

LIBCOMMON_PRIVATE_API     libcommon::Int64        assign64( libcommon::Int64 volatile* value, const libcommon::Int64& val ) {

    assert( value );

    return ::InterlockedExchange64(
               value,
               val
           );

}

LIBCOMMON_PRIVATE_API     libcommon::Int64        exchange64( libcommon::Int64 volatile* atomic, const libcommon::Int64& cmp, const libcommon::Int64& val ) {

    assert( atomic );

    return ::InterlockedCompareExchange64(
               atomic,
               val,
               cmp
           );

}

LIBCOMMON_PRIVATE_API     bool       equal64( libcommon::Int64 volatile* atomic, const libcommon::Int64& val ) {
    assert( atomic );

    return fetch64( atomic ) == val;
}

LIBCOMMON_PRIVATE_API     libcommon::Int32        add32( libcommon::Int32 volatile* value, const libcommon::Int32& val ) {

    assert( value );

    return ::InterlockedAdd(
               ( LONG volatile* )value,
               val
           );
}

LIBCOMMON_PRIVATE_API     libcommon::Int32        sub32( libcommon::Int32 volatile* value, const libcommon::Int32& val ) {

    assert( value );

    return ::InterlockedAdd(
               ( LONG volatile* )value,
               -val
           );
}

LIBCOMMON_PRIVATE_API     libcommon::Int32        inc32( libcommon::Int32 volatile* value ) {

    assert( value );

    return ::InterlockedIncrement(
               ( LONG volatile* )value
           );

}

LIBCOMMON_PRIVATE_API     libcommon::Int32        dec32( libcommon::Int32 volatile* value ) {
    assert( value );

    return ::InterlockedDecrement(
               ( LONG volatile* )value
           );

}

LIBCOMMON_PRIVATE_API     libcommon::Int32        fetch32( libcommon::Int32 volatile* value ) {

    assert( value );

    return ::InterlockedAdd(
               ( LONG volatile* )value,
               0
           );
}

LIBCOMMON_PRIVATE_API     libcommon::Int32        assign32( libcommon::Int32 volatile* value, const libcommon::Int32& val ) {

    assert( value );

    return ::InterlockedExchange(
               ( LONG volatile* )value,
               val
           );
}

LIBCOMMON_PRIVATE_API     libcommon::Int32        exchange32( libcommon::Int32 volatile* atomic, const libcommon::Int32& cmp, const libcommon::Int32& val ) {

    assert( atomic );

    return ::InterlockedCompareExchange(
               ( LONG volatile* )atomic,
               val,
               cmp
           );
}

LIBCOMMON_PRIVATE_API     bool      equal32( libcommon::Int32 volatile* atomic, const libcommon::Int32& val ) {
    assert( atomic );

    return fetch32( atomic ) == val;
}


#elif LIBCOMMON_IS_POSIX


LIBCOMMON_PRIVATE_API     libcommon::Int64        add64( libcommon::Int64 volatile* value, const libcommon::Int64& val ) {

    assert( value );

    return base_add<libcommon::Int64>(
               value,
               val
           );

}

LIBCOMMON_PRIVATE_API     libcommon::Int64        sub64( libcommon::Int64 volatile* value, const libcommon::Int64& val ) {

    assert( value );

    return base_sub<libcommon::Int64>(
               value,
               val
           );

}

LIBCOMMON_PRIVATE_API     libcommon::Int64        inc64( libcommon::Int64 volatile* value ) {

    assert( value );

    return base_inc<libcommon::Int64>(
               value
           );

}

LIBCOMMON_PRIVATE_API     libcommon::Int64        dec64( libcommon::Int64 volatile* value ) {

    assert( value );

    return base_dec<libcommon::Int64>(
               value
           );

}

LIBCOMMON_PRIVATE_API     libcommon::Int64        fetch64( libcommon::Int64 volatile* value ) {

    assert( value );

    return base_fetch(
               value
           );

}

LIBCOMMON_PRIVATE_API     libcommon::Int64        assign64( libcommon::Int64 volatile* value, const libcommon::Int64& val ) {

    assert( value );

    return base_assign(
               value,
               val
           );

}

LIBCOMMON_PRIVATE_API     libcommon::Int64        exchange64( libcommon::Int64 volatile* atomic, const libcommon::Int64& cmp, const libcommon::Int64& val ) {

    assert( atomic );

    return base_exchange(
               atomic,
               cmp,
               val
           );

}


LIBCOMMON_PRIVATE_API     bool       equal64( libcommon::Int64 volatile* atomic, const libcommon::Int64& val ) {

    assert( atomic );

    return base_equal<Int64>(
               atomic,
               val
           );

}

LIBCOMMON_PRIVATE_API     libcommon::Int32        add32( libcommon::Int32 volatile* value, const libcommon::Int32& val ) {

    assert( value );

    return base_add<libcommon::Int32>(
               value,
               val
           );

}

LIBCOMMON_PRIVATE_API     libcommon::Int32        sub32( libcommon::Int32 volatile* value, const libcommon::Int32& val ) {

    assert( value );

    return base_sub<libcommon::Int32>(
               value,
               val
           );

}

LIBCOMMON_PRIVATE_API     libcommon::Int32        inc32( libcommon::Int32 volatile* value ) {

    assert( value );

    return base_inc<libcommon::Int32>(
               value
           );

}

LIBCOMMON_PRIVATE_API     libcommon::Int32        dec32( libcommon::Int32 volatile* value ) {

    assert( value );

    return base_dec<libcommon::Int32>(
               value
           );

}

LIBCOMMON_PRIVATE_API     libcommon::Int32        fetch32( libcommon::Int32 volatile* value ) {

    assert( value );

    return base_fetch<Int32>(
               value
           );

}

LIBCOMMON_PRIVATE_API     libcommon::Int32        assign32( libcommon::Int32 volatile* value, const libcommon::Int32& val ) {

    assert( value );

    return base_assign<Int32>(
               value,
               val
           );

}

LIBCOMMON_PRIVATE_API     libcommon::Int32        exchange32( libcommon::Int32 volatile* atomic, const libcommon::Int32& cmp, const libcommon::Int32& val ) {

    assert( atomic );

    return base_exchange<Int32>(
               atomic,
               cmp,
               val
           );

}

LIBCOMMON_PRIVATE_API     bool        equal32( libcommon::Int32 volatile* atomic, const libcommon::Int32& val ) {

    assert( atomic );

    return base_equal<Int32>(
               atomic,
               val
           );

}

#else
#   error   not implemented
#endif

}
}
