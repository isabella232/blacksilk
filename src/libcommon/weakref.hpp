#pragma once

#include <libcommon/def.hpp>
#include <libcommon/atomics.hpp>

namespace libcommon {

/// standard reference
template < class _t_any >
struct LIBCOMMON_API WeakRef {
        typedef _t_any              ValueType;
        typedef ValueType*          Pointer;
        typedef const ValueType*    ConstPointer;
        typedef ValueType&          Reference;
        typedef const ValueType&    ConstReference;

        WeakRef() : m_Val( NULL ), m_WeakRefCounter( NULL ) {}
        WeakRef( Pointer val ) : m_Val( val ), m_WeakRefCounter( NULL ) {}
        WeakRef( Pointer val, libcommon::atomics::type64* counter ) : m_Val( val ), m_WeakRefCounter( counter ) {
            if( m_WeakRefCounter != NULL ) {
                libcommon::atomics::inc64( m_WeakRefCounter );
            }
        }
        WeakRef( const WeakRef& rhs ) {
            m_WeakRefCounter    = rhs.m_WeakRefCounter;
            m_Val               = rhs.m_Val;

            if( m_WeakRefCounter != NULL ) {
                libcommon::atomics::inc64( m_WeakRefCounter );
            }
        }
        WeakRef& operator = ( const WeakRef& rhs ) {
            if( m_WeakRefCounter != NULL ) {
                libcommon::atomics::dec64( m_WeakRefCounter );
            }

            m_WeakRefCounter    = rhs.m_WeakRefCounter;
            m_Val               = rhs.m_Val;

            if( m_WeakRefCounter != NULL ) {
                libcommon::atomics::inc64( m_WeakRefCounter );
            }

            return *this;
        }
        ~WeakRef() {
            if( m_WeakRefCounter != NULL ) {
                libcommon::atomics::dec64( m_WeakRefCounter );
            }

            m_WeakRefCounter    = NULL;
            m_Val               = NULL;
        }
        bool    empty() const {
            return ( m_Val == NULL );
        }
        ConstPointer get() const {
            return m_Val;
        }
        Pointer get() {
            return m_Val;
        }
        ConstPointer operator * () const {
            assert( m_Val );
            return m_Val;
        }
        Pointer operator * () {
            assert( m_Val );
            return m_Val;
        }
        ConstPointer operator -> () const {
            assert( m_Val );
            return m_Val;
        }
        Pointer operator -> () {
            assert( m_Val );
            return m_Val;
        }
        libcommon::AddressType address() const {
            return ( libcommon::AddressType )m_Val;
        }

        template < class _t_value >
        bool operator == ( const WeakRef<_t_value>& rhs ) const {
            /// 64bit only
            return ( ( libcommon::AddressType )m_Val == ( libcommon::AddressType )rhs.m_Val );
        }
        template < class _t_value >
        bool operator != ( const WeakRef<_t_value>& rhs ) const {
            /// 64bit only
            return ( ( libcommon::AddressType )m_Val != ( libcommon::AddressType )rhs.m_Val );
        }
        template < class _t_value >
        bool operator > ( const WeakRef<_t_value>& rhs ) const {
            /// 64bit only
            return ( ( libcommon::AddressType )m_Val > ( libcommon::AddressType )rhs.m_Val );
        }
        template < class _t_value >
        bool operator < ( const WeakRef<_t_value>& rhs ) const {
            /// 64bit only
            return ( ( libcommon::AddressType )m_Val < ( libcommon::AddressType )rhs.m_Val );
        }
        template < class _t_value >
        bool operator <= ( const WeakRef<_t_value>& rhs ) const {
            /// 64bit only
            return ( ( libcommon::AddressType )m_Val <= ( libcommon::AddressType )rhs.m_Val );
        }
        template < class _t_value >
        bool operator >= ( const WeakRef<_t_value>& rhs ) const {
            /// 64bit only
            return ( ( libcommon::AddressType )m_Val >= ( libcommon::AddressType )rhs.m_Val );
        }
        libcommon::atomics::type64     refCount() const {
            if( m_WeakRefCounter != NULL ) {
                return libcommon::atomics::fetch64(
                           m_WeakRefCounter
                       );
            }

            return 0;
        }
    private:
        Pointer                         m_Val;
        libcommon::atomics::type64*     m_WeakRefCounter;
};


}
