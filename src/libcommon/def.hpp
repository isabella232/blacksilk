#pragma once

/// common global incldues
#include    <utility>
#include    <typeinfo>
#include    <assert.h>

/// common defines
#define LIBCOMMON_NULL          0
#define LIBCOMMON_INVALID       999999
#define LIBCOMMON_UNKNOWN       666666

/// check for the current compiler

#define LIBCOMMON_COMPILER_MSVC         0
#define LIBCOMMON_COMPILER_GCC          1
#define LIBCOMMON_COMPILER_CLANG        2
#define LIBCOMMON_COMPILER_BORLANDCC    3
#define LIBCOMMON_COMPILER_COMO         4
#define LIBCOMMON_COMPILER_UNKNOWN      LIBCOMMON_UNKNOWN
#define LIBCOMMON_COMPILER_INVALID      LIBCOMMON_INVALID
namespace libcommon {
namespace ECompiler {
enum t {
    MSVC        = LIBCOMMON_COMPILER_MSVC,
    GCC         = LIBCOMMON_COMPILER_GCC,
    Clang       = LIBCOMMON_COMPILER_CLANG,
    BorlandCC   = LIBCOMMON_COMPILER_BORLANDCC,
    Como        = LIBCOMMON_COMPILER_COMO,
    Unknown     = LIBCOMMON_UNKNOWN,
    Invalid     = LIBCOMMON_INVALID
};
}
}

#if defined(__BORLANDC__) && !defined(LIBCOMMON_COMPILER)
#   define LIBCOMMON_COMPILER       LIBCOMMON_COMPILER_BORLANDCC
#endif

#if defined(__clang__) && !defined(LIBCOMMON_COMPILER)
#   define LIBCOMMON_COMPILER       LIBCOMMON_COMPILER_CLANG
#endif

#if defined(__COMO__) && !defined(LIBCOMMON_COMPILER)
#   define LIBCOMMON_COMPILER       LIBCOMMON_COMPILER_COMO
#endif

#if defined(__GNUC__) && !defined(LIBCOMMON_COMPILER)
#   define LIBCOMMON_COMPILER       LIBCOMMON_COMPILER_GCC
#endif

#if defined(_MSC_VER) && !defined(LIBCOMMON_COMPILER)
#   define LIBCOMMON_COMPILER       LIBCOMMON_COMPILER_MSVC
#endif

#if !defined(LIBCOMMON_COMPILER)
#   warning     libcommon: failed to recognize compiler
#   define      LIBCOMMON_COMPILER      LIBCOMMON_UNKNOWN
#endif

#if LIBCOMMON_COMPILER == LIBCOMMON_UNKNOWN
#   undef       LIBCOMMON_COMPILER
#   define      LIBCOMMON_COMPILER      LIBCOMMON_COMPILER_INVALID
#   error       libcommon: invalid compiler
#endif

#ifndef NULL
#   define NULL 0
#endif


#define LIBCOMMON_IS_COMPILER(x)         ( LIBCOMMON_COMPILER == x )
#define LIBCOMMON_IS_NOT_COMPILER(x)    !( LIBCOMMON_COMPILER == x )
#define LIBCOMMON_IS_GCC_COMPATIBLE      ( LIBCOMMON_COMPILER == LIBCOMMON_COMPILER_GCC ) || ( LIBCOMMON_COMPILER == LIBCOMMON_COMPILER_CLANG )

/// check for the current system
///
#define LIBCOMMON_SYSTEM_WINDOWS            0
#define LIBCOMMON_SYSTEM_LINUX              1
#define LIBCOMMON_SYSTEM_MAC                2
#define LIBCOMMON_SYSTEM_ANDROID            3
#define LIBCOMMON_SYSTEM_IOS                4
#define LIBCOMMON_SYSTEM_UNKNOWN            LIBCOMMON_UNKNOWN
#define LIBCOMMON_SYSTEM_INVALID            LIBCOMMON_INVALID

namespace libcommon {
namespace ESystem {
enum t {
    Windows         = LIBCOMMON_SYSTEM_WINDOWS,
    Linux           = LIBCOMMON_SYSTEM_LINUX,
    Mac             = LIBCOMMON_SYSTEM_MAC,
    Android         = LIBCOMMON_SYSTEM_ANDROID,
    iOS             = LIBCOMMON_SYSTEM_IOS,
    Unknown         = LIBCOMMON_UNKNOWN,
    Invalid         = LIBCOMMON_INVALID
};
}
}

/** check for mac **/
#if defined(__APPLE__) && defined(__MACH__) && !defined(LIBCOMMON_SYSTEM)
#   define  LIBCOMMON_SYSTEM        LIBCOMMON_SYSTEM_MAC
#endif

#if !defined(LIBCOMMON_SYSTEM) && ( defined(macintosh) || defined(Macintosh) )
#   define      LIBCOMMON_SYSTEM        LIBCOMMON_INVALID
#   error       Macintosh not supported.
#endif

#if defined(__linux__) && !defined(LIBCOMMON_SYSTEM)
#   define      LIBCOMMON_SYSTEM        LIBCOMMON_SYSTEM_LINUX
#endif

#if defined(_WIN32) && !defined(LIBCOMMON_SYSTEM)
#   define      LIBCOMMON_SYSTEM        LIBCOMMON_SYSTEM_WINDOWS
#endif

#if defined(__ANDROID__) && !defined(LIBCOMMON_SYSTEM)
#   define      LIBCOMMON_SYSTEM        LIBCOMMON_SYSTEM_ANDROID
#endif

#if defined(__APPLE__) && !defined(LIBCOMMON_SYSTEM)
#   include "TargetConditionals.h"
#   if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
#       define  LIBCOMMON_SYSTEM        LIBCOMMON_SYSTEM_IOS
#   endif
#endif

#if !defined(LIBCOMMON_SYSTEM)
#   warning libcommon: failed to recognize build system
#   define LIBCOMMON_SYSTEM     LIBCOMMON_SYSTEM_UNKNOWN
#endif

#if LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_UNKNOWN
#   undef LIBCOMMON_SYSTEM
#   define LIBCOMMON_SYSTEM     LIBCOMMON_SYSTEM_INVALID
#   error   libcommon: invalid build system
#endif

#define     LIBCOMMON_IS_SYSTEM(x)        ( LIBCOMMON_SYSTEM == x )
#define     LIBCOMMON_IS_NOT_SYSTEM(x)   !( LIBCOMMON_SYSTEM == x )
#define     LIBCOMMON_IS_BSD_LIKE         ( LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_MAC )
#define     LIBCOMMON_IS_POSIX            ( ( LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_MAC ) || ( LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_LINUX ) || ( LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_ANDROID ) \
    || ( LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_IOS ) )


/// check for the target arch
#define LIBCOMMON_ARCH_X86              0
#define LIBCOMMON_ARCH_AMD64            1
#define LIBCOMMON_ARCH_ARM32            2
#define LIBCOMMON_ARCH_ARM64            3
#define LIBCOMMON_ARCH_INVALID          LIBCOMMON_INVALID
#define LIBCOMMON_ARCH_UNKNOWN          LIBCOMMON_UNKNOWN

namespace libcommon {
namespace EArch {
enum t {
    X86         = LIBCOMMON_ARCH_X86,
    AMD64       = LIBCOMMON_ARCH_AMD64,
    ARM32       = LIBCOMMON_ARCH_ARM32,
    ARM64       = LIBCOMMON_ARCH_ARM64,
    Invalid     = LIBCOMMON_ARCH_INVALID,
    Unknown     = LIBCOMMON_ARCH_UNKNOWN
};
}
}

#if !defined(LIBCOMMON_ARCH) && ( defined(__amd64__) || defined(__amd64) || defined(__x86_64__) && defined(__x86_64) || defined(_M_X64) || defined(_M_AMD64) )
#   define LIBCOMMON_ARCH       LIBCOMMON_ARCH_AMD64
#endif

#if !defined(LIBCOMMON_ARCH) && ( defined(__arm__) || defined(__thumb__) || defined(__TARGET_ARCH_ARM) || defined(__TARGET_ARCH_THUMB) || defined(_ARM) || defined(_M_ARM) || defined(_M_ARMT) )
#   define LIBCOMMON_ARCH       LIBCOMMON_ARCH_ARM32
#endif

#if !defined(LIBCOMMON_ARCH) && ( defined(__aarch64__) )
#   define LIBCOMMON_ARCH       LIBCOMMON_ARCH_ARM64
#endif

#if !defined(LIBCOMMON_ARCH) && ( defined(i386) || defined(__i386) || defined(__i386__) || defined(__IA32__) || defined(_M_I86) || defined(_M_IX86) || defined(__X86__) || defined(_X86_) || defined(__I86__) || defined(__INTEL__) )
#   define  LIBCOMMON_ARCH      LIBCOMMON_ARCH_X86
#endif

#if !defined(LIBCOMMON_ARCH)
#   define LIBCOMMON_ARCH       LIBCOMMON_ARCH_UNKNOWN
#   warning libcommon: failed to recognize target arch
#endif

#if LIBCOMMON_ARCH == LIBCOMMON_ARCH_UNKNOWN
#   undef   LIBCOMMON_ARCH
#   define  LIBCOMMON_ARCH  LIBCOMMON_ARCH_INVALID
#   error   libcommon: invalid target arch
#endif

#define LIBCOMMON_IS_ARCH(x)        ( LIBCOMMON_ARCH == x )
#define LIBCOMMON_IS_NOT_ARCH(x)   !( LIBCOMMON_ARCH == x )
#define LIBCOMMON_IS_ARM    ( LIBCOMMON_ARCH == LIBCOMMON_ARCH_ARM32 ) || ( LIBCOMMON_ARCH == LIBCOMMON_ARCH_ARM64 )
#define LIBCOMMON_IS_64BIT  ( LIBCOMMON_ARCH == LIBCOMMON_ARCH_ARM64 ) || ( LIBCOMMON_ARCH == LIBCOMMON_ARCH_AMD64 )
#define LIBCOMMON_IS_32BIT  !( LIBCOMMON_IS_64BIT )

/// default config
#if !defined(LIBCOMMON_CONFIG_BUILD_DYNLIB)
#   define  LIBCOMMON_CONFIG_BUILD_DYNLIB    0
#endif
#if !defined(LIBCOMMON_CONFIG_LINK_DYNLIB)
#   define  LIBCOMMON_CONFIG_LINK_DYNLIB     0
#endif
#if !defined(LIBCOMMON_CONFIG_ENFORCE_CPP_COMPILER_FEATURES)
#   define  LIBCOMMON_CONFIG_ENFORCE_CPP_COMPILER_FEATURES 1
#endif

/// common defines

#if LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_WINDOWS
#   define      LIBCOMMON_DLL_IMPORT        __declspec(dllimport)
#   define      LIBCOMMON_DLL_EXPORT        __declspec(dllexport)
#   define      LIBCOMMON_EXTERN            extern
#   define      LIBCOMMON_EXTERN_C          extern "C"
#   define      LIBCOMMON_EXTERN_CPP        extern "Cpp"
#   define      LIBCOMMON_INLINE            inline
#   define      LIBCOMMON_FORCE_INLINE      __forceinline
#   define      LIBCOMMON_IMPORT            LIBCOMMON_DLL_IMPORT
#   define      LIBCOMMON_EXPORT            LIBCOMMON_DLL_EXPORT
#   if  LIBCOMMON_CONFIG_BUILD_DYNLIB
#       if  LIBCOMMON_CONFIG_LINK_DYNLIB
#           define LIBCOMMON_API        LIBCOMMON_DLL_IMPORT
#           define LIBCOMMON_PRIVATE_API
#       else
#           define LIBCOMMON_API        LIBCOMMON_DLL_EXPORT
#           define LIBCOMMON_PRIVATE_API
#       endif
#   else
#       if  LIBCOMMON_CONFIG_LINK_DYNLIB
#           define LIBCOMMON_API            LIBCOMMON_DLL_IMPORT
#           define LIBCOMMON_PRIVATE_API
#       else
#           define LIBCOMMON_API
#           define LIBCOMMON_PRIVATE_API
#       endif
#   endif
#   define      LIBCOMMON_VAL               1
#else
#   define      LIBCOMMON_EXTERN            extern
#   define      LIBCOMMON_EXTERN_C          extern "C"
#   define      LIBCOMMON_EXTERN_CPP        extern "Cpp"
#   define      LIBCOMMON_INLINE            inline
#   define      LIBCOMMON_FORCE_INLINE      __attribute__((always_inline))
#   define      LIBCOMMON_IMPORT
#   define      LIBCOMMON_EXPORT
#   define      LIBCOMMON_API
#   define      LIBCOMMON_PRIVATE_API
#   define      LIBCOMMON_VAL               0
#endif

/// default c++ config
#define     LIBCOMMON_CPP_CONFIG_DELETED_FUNCTIONS      0
#define     LIBCOMMON_CPP_CONFIG_RVALUE                 0
#define     LIBCOMMON_CPP_CONFIG_VARIADIC_TEMPLATES     0
#define     LIBCOMMON_CPP_CONFIG_AUTO                   0
#define     LIBCOMMON_CPP_CONFIG_NULLPTR                0

/// enforce default compiler config
#if LIBCOMMON_CONFIG_ENFORCE_CPP_COMPILER_FEATURES
#   undef LIBCOMMON_CPP_CONFIG_AUTO
#   undef LIBCOMMON_CPP_CONFIG_RVALUE
#   undef LIBCOMMON_CPP_CONFIG_VARIADIC_TEMPLATES
#   undef LIBCOMMON_CPP_CONFIG_DELETED_FUNCTIONS
#   undef LIBCOMMON_CPP_CONFIG_NULLPTR
#   if LIBCOMMON_COMPILER == LIBCOMMON_COMPILER_MSVC || LIBCOMMON_COMPILER == LIBCOMMON_COMPILER_COMO || LIBCOMMON_COMPILER == LIBCOMMON_COMPILER_BORLANDCC
#       define     LIBCOMMON_CPP_CONFIG_DELETED_FUNCTIONS      0
#       define     LIBCOMMON_CPP_CONFIG_RVALUE                 1
#       define     LIBCOMMON_CPP_CONFIG_VARIADIC_TEMPLATES     0
#       define     LIBCOMMON_CPP_CONFIG_AUTO                   1
#       define     LIBCOMMON_CPP_CONFIG_NULLPTR                0
#   else
#       define     LIBCOMMON_CPP_CONFIG_DELETED_FUNCTIONS      1
#       define     LIBCOMMON_CPP_CONFIG_RVALUE                 1
#       define     LIBCOMMON_CPP_CONFIG_VARIADIC_TEMPLATES     1
#       define     LIBCOMMON_CPP_CONFIG_AUTO                   1
#       define     LIBCOMMON_CPP_CONFIG_NULLPTR                1
#   endif
#endif

#if LIBCOMMON_CPP_CONFIG_NULLPTR
#   undef   LIBCOMMON_NULL
#   define  LIBCOMMON_NULL      nullptr
#endif

namespace libcommon {

/** basic char types **/
typedef char                Byte8;
typedef unsigned char       UByte8;

typedef char                Char;
typedef unsigned char       UChar;

typedef short               Byte16;
typedef unsigned short      UByte16;

typedef short               WChar;
typedef unsigned short      WUChar;

/** basic integer types **/
typedef char                Int8;
typedef unsigned char       UInt8;

typedef short               Int16;
typedef unsigned short      UInt16;

typedef int                 Int32;
typedef unsigned int        UInt32;

typedef long long           Int64;
typedef unsigned long long  UInt64;

/** basic floating point types **/
typedef float               Single;
typedef double              Double;
typedef Single              Float32;
typedef Double              Float64;

/** general types **/
typedef bool                Boolean;

/** platform dependent types **/
#if LIBCOMMON_IS_64BIT
typedef UInt64              SizeType;
typedef Int64               OffsetType;
typedef UInt64              AddressType;
#else
typedef UInt32              SizeType;
typedef Int32               OffsetType;
typedef UInt32              AddressType;
#endif

/** basic compile-time constants **/

/// represents a compile-time constant
template < class _t_class, _t_class _v_value >
struct  Constant {
    typedef _t_class            ValueType;
    typedef ValueType*          Pointer;
    typedef const ValueType*    ConstPointer;
    typedef ValueType&          Reference;
    typedef const ValueType&    ConstReference;

    static const ValueType      value       = _v_value;

    ConstReference operator * () const {
        return value;
    }
    ConstReference get() const {
        return value;
    }

    /// implicit conversion
    operator ValueType() const {
        return get();
    }
};

typedef Constant< libcommon::Boolean, true >      TrueConstant;
typedef Constant< libcommon::Boolean, false >     FalseConstant;
typedef Constant< libcommon::Int32, 0 >           NullConstant;

/// evaluates to TrueConstant, if the given type is
/// signed integer type.
template < class _t_type >
struct  IsSignedInteger : FalseConstant {};
template <>
struct  IsSignedInteger< libcommon::Int8 > : TrueConstant {};
template <>
struct  IsSignedInteger< libcommon::Int16 > : TrueConstant {};
template <>
struct  IsSignedInteger< libcommon::Int32 > : TrueConstant {};
template <>
struct  IsSignedInteger< libcommon::Int64 > : TrueConstant {};


/// evaluates to TrueConstant, if the given type is
/// an unsigned integer type.
template < class _t_type >
struct  IsUnsignedInteger : FalseConstant {};
template <>
struct  IsUnsignedInteger< libcommon::UInt8 > : TrueConstant {};
template <>
struct  IsUnsignedInteger< libcommon::UInt16 > : TrueConstant {};
template <>
struct  IsUnsignedInteger< libcommon::UInt32 > : TrueConstant {};
template <>
struct  IsUnsignedInteger< libcommon::UInt64 > : TrueConstant {};

/// evaluates to TrueConstant, if the given type is
/// an integer type
template < class _t_type >
struct  IsInteger : Constant < libcommon::Boolean, IsSignedInteger< _t_type >::value ||
    IsUnsignedInteger< _t_type >::value > {};

template < class _t_any >
const _t_any& getMax( const _t_any& a, const _t_any& b ) {
    return ( a > b ) ? a : b;
}
template < class _t_any >
const _t_any& getMin( const _t_any& a, const _t_any& b ) {
    return ( a < b ) ? a : b;
}

}


