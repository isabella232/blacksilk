#pragma once

#include <libcommon/def.hpp>
#include <libcommon/prelude.hpp>

namespace libcommon {

template < class _t_key, class _t_value >
struct Pair {
    typedef _t_key          KeyType;
    typedef _t_value        ValueType;
    typedef _t_value&       Reference;
    typedef const _t_value& ConstReference;
    typedef _t_value*       Pointer;
    typedef const _t_value* ConstPointer;

    explicit Pair( ConstReference _value ) : value( _value ), key( KeyType( _value ) ) {}
    Pair( const KeyType& _key, ConstReference _value ) : key( _key ), value( _value ) {}
    Pair( const Pair& other ) : key( other.key ), value( other.value ) {}
    Pair( Pair&& rhs ) {
        libcommon::swap(
            rhs.value,
            value
        );

        libcommon::swap(
            rhs.key,
            key
        );
    }

    Pair& operator = ( const Pair& rhs ) {
        value   = rhs.value;
        key     = rhs.key;

        return *this;
    }

    Pair& operator = ( Pair&& rhs ) {
        libcommon::swap(
            rhs.value,
            value
        );

        libcommon::swap(
            rhs.key,
            key
        );

        return *this;
    }

    bool operator == ( const Pair& rhs ) {
        return ( key == rhs.key );
    }
    bool operator != ( const Pair& rhs ) {
        return ( key != rhs.key );
    }
    bool operator > ( const Pair& rhs ) {
        return ( key > rhs.key );
    }
    bool operator >= ( const Pair& rhs ) {
        return ( key >= rhs.key );
    }
    bool operator < ( const Pair& rhs ) {
        return ( key < rhs.key );
    }
    bool operator <= ( const Pair& rhs ) {
        return ( key <= rhs.key );
    }
    template < class _t_any_val >
    bool operator == ( const Pair<_t_key, _t_any_val>& rhs ) {
        return ( key == rhs.key );
    }
    template < class _t_any_val >
    bool operator != ( const Pair<_t_key, _t_any_val>& rhs ) {
        return ( key != rhs.key );
    }
    template < class _t_any_val >
    bool operator < ( const Pair<_t_key, _t_any_val>& rhs ) {
        return ( key < rhs.key );
    }
    template < class _t_any_val >
    bool operator < ( const Pair<_t_key, _t_any_val>& rhs ) {
        return ( key < rhs.key );
    }
    template < class _t_any_val >
    bool operator > ( const Pair<_t_key, _t_any_val>& rhs ) {
        return ( key > rhs.key );
    }
    template < class _t_any_val >
    bool operator >= ( const Pair<_t_key, _t_any_val>& rhs ) {
        return ( key >= rhs.key );
    }

    void swap( Pair&& rhs ) {
        libcommon::swap(
            rhs.value,
            value
        );

        libcommon::swap(
            rhs.key,
            key
        );
    }

    ValueType       value;
    KeyType         key;
};

template < class _t_key, class _t_value >
const _t_key&   getKey( const Pair<_t_key, _t_value>& val ) {
    return val.key;
}
template < class _t_key, class _t_value >
const _t_value&   getValue( const Pair<_t_key, _t_value>& val ) {
    return val.value;
}
template < class _t_key, class _t_value >
Pair<_t_key, _t_value>   makePair( const _t_key& key, const _t_value& val ) {
    return Pair<_t_key, _t_value>(
               key,
               val
           );
}

}
