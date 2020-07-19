#pragma once

#include <libcommon/def.hpp>
#include <libcommon/noncopyable.hpp>

namespace libcommon {

template < class _t_any >
struct  Maybe;

template <>
struct  Maybe<void> {
    bool operator == ( const Maybe<void>& ) const {
        return false;
    }
    bool operator != ( const Maybe<void>& ) const {
        return true;
    }
    template < class _t_any >
    bool operator == ( const _t_any& ) const {
        return false;
    }
    template < class _t_any >
    bool operator != ( const _t_any& ) const {
        return true;
    }
};
typedef Maybe<void>     Nothing;

template < class _t_any >
struct  Maybe<const _t_any&> : libcommon::INonCopyable {
    public:
        typedef _t_any          ValueType;
        typedef ValueType*      Pointer;
        typedef const Pointer   ConstPointer;
        typedef ValueType&      Reference;
        typedef const _t_any&   ConstReference;

        struct MaybeValue {
                MaybeValue( ConstReference _val ) : val( _val ) {}

                ConstReference get_val() const {
                    return val;
                }
            private:
                ConstReference val;
        };

        Maybe() : m_Value( NULL ) {}
        Maybe( ConstReference ref ) : m_Value( new MaybeValue( ref ) ) {}
        Maybe( Maybe&& rhs ) : m_Value( NULL ) {
            swap( rhs );
        }
        Maybe( const Nothing& rhs ) : m_Value( NULL ) {
            ( void )rhs;
        }
        Maybe& operator = ( const Maybe& rhs ) {
            swap( rhs );

            return *this;
        }

        Pointer operator -> () const {
            assert( m_Value );
            return &m_Value->get_val();
        }

        ConstReference operator * () const {
            assert( m_Value );
            return m_Value->get_val();
        }

        void swap( Maybe& rhs ) {
            MaybeValue* val = rhs.m_Value;

            rhs.m_Value     = val;
            m_Value         = rhs;
        }

        bool    empty() const {
            return ( m_Value == NULL );
        }
        bool    nothing() const {
            return empty();
        }
        bool    valid() const {
            return !empty();
        }

        ConstReference  value() const {
            assert( m_Value );
            return m_Value->get_val();
        }

        bool operator == ( const Maybe<void>& invalid ) const {
            ( void )invalid;

            return nothing();
        }
        bool operator != ( const Maybe<void>& invalid ) const {
            ( void )invalid;

            return !nothing();
        }
    protected:
        MaybeValue* m_Value;
};

template < class _t_any >
struct  Maybe<_t_any&> : libcommon::INonCopyable {
    public:
        typedef _t_any          ValueType;
        typedef ValueType*      Pointer;
        typedef const Pointer   ConstPointer;
        typedef ValueType&      Reference;
        typedef const _t_any&   ConstReference;

        struct MaybeValue {
                MaybeValue( Reference _val ) : val( _val ) {}

                ConstReference get_val() const {
                    return val;
                }
                Reference get_val() {
                    return val;
                }
            private:
                Reference val;
        };

        Maybe() : m_Value( NULL ) {}
        Maybe( Reference ref ) : m_Value( new MaybeValue( ref ) ) {}
        Maybe( Maybe&& rhs ) : m_Value( NULL ) {
            swap( rhs );
        }
        Maybe( const Nothing& rhs ) : m_Value( NULL ) {
            ( void )rhs;
        }
        Maybe& operator = ( const Maybe& rhs ) {
            swap( rhs );

            return *this;
        }

        Pointer operator -> () const {
            assert( m_Value );
            return &m_Value->get_val();
        }

        ConstReference operator * () const {
            assert( m_Value );
            return m_Value->get_val();
        }
        Reference operator * () {
            assert( m_Value );
            return m_Value->get_val();
        }

        void swap( Maybe& rhs ) {
            MaybeValue* val = rhs.m_Value;

            rhs.m_Value     = val;
            m_Value         = rhs;
        }

        bool    empty() const {
            return ( m_Value == NULL );
        }
        bool    nothing() const {
            return empty();
        }
        bool    valid() const {
            return !empty();
        }

        ConstReference  value() const {
            assert( m_Value );
            return m_Value->get_val();
        }

        bool operator == ( const Maybe<void>& invalid ) const {
            ( void )invalid;

            return nothing();
        }
        bool operator != ( const Maybe<void>& invalid ) const {
            ( void )invalid;

            return !nothing();
        }
    protected:
        MaybeValue* m_Value;
};

template < class _t_any >
struct  Maybe : libcommon::INonCopyable {
    public:
        typedef _t_any          ValueType;
        typedef ValueType*      Pointer;
        typedef const Pointer   ConstPointer;
        typedef ValueType&      Reference;
        typedef const _t_any&   ConstReference;

        struct MaybeValue {
                MaybeValue( ConstReference _val ) : val( _val ) {}

                ConstReference get_val() const {
                    return val;
                }
                Reference get_val() {
                    return val;
                }
            private:
                ValueType   val;
        };

        Maybe() : m_Value( NULL ) {}
        Maybe( ConstReference ref ) : m_Value( new MaybeValue( ref ) ) {}
        Maybe( Maybe&& rhs ) : m_Value( NULL ) {
            swap( rhs );
        }
        Maybe( const Nothing& rhs ) : m_Value( NULL ) {
            ( void )rhs;
        }
        Maybe& operator = ( Maybe& rhs ) {
            swap( rhs );

            return *this;
        }

        Pointer operator -> () const {
            assert( m_Value );
            return &m_Value->get_val();
        }

        ConstReference operator * () const {
            assert( m_Value );
            return m_Value->get_val();
        }
        Reference operator * () {
            assert( m_Value );
            return m_Value->get_val();
        }

        void swap( Maybe& rhs ) {
            MaybeValue* val = rhs.m_Value;

            rhs.m_Value     = m_Value;
            m_Value         = val;
        }

        bool    empty() const {
            return ( m_Value == NULL );
        }
        bool    nothing() const {
            return empty();
        }
        bool    valid() const {
            return !empty();
        }

        ConstReference  value() const {
            assert( m_Value );
            return m_Value->get_val();
        }

        bool operator == ( const Maybe<void>& invalid ) const {
            ( void )invalid;

            return nothing();
        }
        bool operator != ( const Maybe<void>& invalid ) const {
            ( void )invalid;

            return !nothing();
        }
    protected:
        MaybeValue* m_Value;
};

template < class _t_value >
libcommon::Maybe<_t_value>  just( const _t_value& val ) {
    return libcommon::Maybe<_t_value>( val );
}
static inline libcommon::Nothing nothing() {
    return libcommon::Nothing();
}

/// returns true, if the given Maybe instance is invalid.
template < class _t_any >
libcommon::Boolean  failed( const libcommon::Maybe<_t_any>& val ) {
    return ( val.empty() );
}

/// returns true, if the given Maybe instance is valid.
template < class _t_any >
libcommon::Boolean  success( const libcommon::Maybe<_t_any>& val ) {
    return !( val.empty() );
}

}
