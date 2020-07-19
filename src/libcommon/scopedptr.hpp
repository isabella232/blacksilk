#pragma once

#include <libcommon/def.hpp>
#include <libcommon/weakref.hpp>
#include <libcommon/noncopyable.hpp>
#include <libcommon/atomics.hpp>

#include <libcommon/pimpl.hpp>

namespace libcommon {

/// similiar to std::unique_ptr
template < class _t_any >
struct LIBCOMMON_API ScopedPtr : public libcommon::INonCopyable {
        typedef _t_any              ValueType;
        typedef ValueType*          Pointer;
        typedef const ValueType*    ConstPointer;
        typedef ValueType&          Reference;
        typedef const ValueType&    ConstReference;

        ScopedPtr() : m_Val( NULL ) {}
        explicit ScopedPtr( Pointer val ) : m_Val( val ), m_WeakRefCounter( 0 ) {}
        ~ScopedPtr() {
            if( m_Val != nullptr ) {
                delete m_Val;
            }
        }

        template < class _t_value >
        ScopedPtr( ScopedPtr<_t_value>&& rhs ) : m_Val( NULL ), m_WeakRefCounter( 0 ) {
            std::swap(
                rhs.m_Val,
                m_Val
            );
        }
        template < class _t_value >
        void swap( ScopedPtr<_t_value>&& rhs ) {
            reset();

            std::swap(
                rhs.m_Val,
                m_Val
            );
        }

        template < class _t_value >
        ScopedPtr& operator = ( ScopedPtr<_t_value>&& rhs ) {
            reset();

            std::swap(
                rhs.m_Val,
                m_Val
            );

            return *this;
        }

        void clear() {
            m_Val = NULL;
            m_WeakRefCounter = 0;
        }
        void reset( Pointer newVal = NULL ) {
            delete m_Val;
            m_Val = newVal;
            m_WeakRefCounter = 0;
        }

        void assign( Pointer value ) {
            reset( value );
        }
        template < class _t_value >
        void assign( ScopedPtr<_t_value>&& rhs ) {
            reset( ( Pointer )rhs.m_Val );

            rhs.m_Val = NULL;
            rhs.m_WeakRefCounter = 0;

            m_WeakRefCounter = 0;
        }

        bool empty() const {
            return ( m_Val == NULL );
        }

        inline operator ConstPointer() const {
            return get();
        }
        inline operator Pointer() const {
            return m_Val;
        }

        inline ConstPointer get() const {
            return m_Val;
        }
        inline Pointer get() {
            return m_Val;
        }
        inline ConstPointer operator * () const {
            assert( m_Val != nullptr );
            return m_Val;
        }
        inline Pointer operator * () {
            assert( m_Val != nullptr );
            return m_Val;
        }
        inline ConstPointer operator -> () const {
            assert( m_Val != nullptr );
            return m_Val;
        }
        inline Pointer operator -> () {
            assert( m_Val != nullptr );
            return m_Val;
        }

        libcommon::AddressType address() const {
            return ( libcommon::AddressType )m_Val;
        }

        template < class _t_value >
        inline bool operator == ( _t_value* ptr ) const {
            return ( ( libcommon::AddressType )m_Val == ( libcommon::AddressType )ptr );
        }
        template < class _t_value >
        inline bool operator != ( _t_value* ptr ) const {
            return ( ( libcommon::AddressType )m_Val != ( libcommon::AddressType )ptr );
        }

        template < class _t_value >
        inline bool operator == ( const ScopedPtr<_t_value>& rhs ) const {
            /// 64bit only
            return ( ( libcommon::AddressType )m_Val == ( libcommon::AddressType )rhs.m_Val );
        }
        template < class _t_value >
        inline bool operator != ( const ScopedPtr<_t_value>& rhs ) const {
            /// 64bit only
            return ( ( libcommon::AddressType )m_Val != ( libcommon::AddressType )rhs.m_Val );
        }
        template < class _t_value >
        inline bool operator > ( const ScopedPtr<_t_value>& rhs ) const {
            /// 64bit only
            return ( ( libcommon::AddressType )m_Val > ( libcommon::AddressType )rhs.m_Val );
        }
        template < class _t_value >
        inline bool operator < ( const ScopedPtr<_t_value>& rhs ) const {
            /// 64bit only
            return ( ( libcommon::AddressType )m_Val < ( libcommon::AddressType )rhs.m_Val );
        }
        template < class _t_value >
        inline bool operator <= ( const ScopedPtr<_t_value>& rhs ) const {
            /// 64bit only
            return ( ( libcommon::AddressType )m_Val <= ( libcommon::AddressType )rhs.m_Val );
        }
        template < class _t_value >
        inline bool operator >= ( const ScopedPtr<_t_value>& rhs ) const {
            /// 64bit only
            return ( ( libcommon::AddressType )m_Val >= ( libcommon::AddressType )rhs.m_Val );
        }
        libcommon::UInt32 weakRefCount() const {
            return libcommon::atomics::fetch32(
                       &m_WeakRefCounter
                   );
        }

        libcommon::WeakRef<ValueType> weakRef() const {
            assert( m_Val );

            return libcommon::WeakRef<ValueType>(
                       get(),
                       &m_WeakRefCounter
                   );
        }

    private:
        Pointer m_Val;
        libcommon::Int32 m_WeakRefCounter;
};


}
