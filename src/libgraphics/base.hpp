#pragma once

#include <utility>

#include <libcommon/def.hpp>
#include <libcommon/atomics.hpp>

#ifdef LIBGRAPHICS_DEBUG_OUTPUT
#   define LIBGRAPHICS_FEATURE_MEMORY_REPORT        1
#else
#   define LIBGRAPHICS_FEATURE_MEMORY_REPORT        0
#endif


namespace libgraphics {
/** basic char types **/
typedef libcommon::Byte8                Byte8;
typedef libcommon::UByte8               UByte8;

typedef libcommon::Char                 Char;
typedef libcommon::UChar                UChar;

typedef libcommon::Byte16               Byte16;
typedef libcommon::UByte16              UByte16;

typedef libcommon::WChar                WChar;
typedef libcommon::WUChar               WUChar;

/** basic integer types **/
typedef libcommon::Int8                 Int8;
typedef libcommon::UInt8                UInt8;

typedef libcommon::Int16                Int16;
typedef libcommon::UInt16               UInt16;

typedef libcommon::Int32                Int32;
typedef libcommon::UInt32               UInt32;

typedef libcommon::Int64                Int64;
typedef libcommon::UInt64               UInt64;

/** basic floating point types **/
typedef libcommon::Single               Single;
typedef libcommon::Double               Double;
typedef libcommon::Float32              Float32;
typedef libcommon::Float64              Float64;

/** general types **/
typedef libcommon::Boolean              Boolean;



/** standard helper container types **/
template < class _t_key, class _t_val >
struct Pair {
    typedef _t_key      KeyType;
    typedef _t_val      ValueType;

    typedef ValueType&          Reference;
    typedef const ValueType&    ConstReference;
    typedef ValueType*          Pointer;
    typedef const ValueType*    ConstPointer;

    explicit Pair( const KeyType& _key ) : key( _key ), value( ValueType() ) {}
    Pair( const KeyType& _key, const ValueType& _value ) :
        key( _key ), value( _value ) {}
    Pair( const Pair<_t_key, _t_val>& rhs ) :
        key( rhs.key ), value( rhs.value ) {}

    inline void assign( const Pair<_t_key, _t_val>& rhs ) {
        key = rhs.key;
        value = rhs.value;
    }
    inline void swap( Pair<_t_key, _t_val>& other ) {
        std::swap(
            key,
            other.key
        );
        std::swap(
            value,
            other.value
        );
    }

    inline bool operator == ( const Pair<_t_key, _t_val>& rhs ) const { return ( key == rhs.key ) && ( value == rhs.value ); }
    inline bool operator != ( const Pair<_t_key, _t_val>& rhs ) const { return ( key != rhs.key ) || ( value != rhs.value ); }

    inline Pair<_t_key, _t_val>& operator = ( const Pair<_t_key, _t_val>& rhs ) {
        assign( rhs );
        return *this;
    }

    KeyType     key;
    ValueType   value;
};

}


/// the following section of defines sets all standard and general
/// preprecessor values

#define LIBGRAPHICS_IMPORT      LIBCOMMON_IMPORT
#define LIBGRAPHICS_EXPORT      LIBCOMMON_EXPORT
#define LIBGRAPHICS_API         LIBCOMMON_API
#define LIBGRAPHICS_PRIVATE_API LIBCOMMON_PRIVATE_API

#define LIBGRAPHICS_EXTERN      LIBCOMMON_EXTERN
#define LIBGRAPHICS_EXTERN_CPP  LIBCOMMON_EXTERN_CPP
#define LIBGRAPHICS_EXTERN_C    LIBCOMMON_EXTERN_C

#define LIBGRAPHICS_INLINE          LIBCOMMON_INLINE
#define LIBGRAPHICS_FORCE_INLINE    LIBCOMMON_FORCE_INLINE
#define LIBGRAPHICS_NULL            LIBCOMMON_NULL

/// include debug headers and assertions
#include <assert.h>
