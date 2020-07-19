#pragma once

#include <libcommon/def.hpp>
#include <libcommon/atomics.hpp>
#include <libcommon/weakref.hpp>

#include <functional>

namespace libcommon {

/// similiar to std::shared_ptr
template < class _t_any >
struct LIBCOMMON_API SharedPtr {
        typedef _t_any              ValueType;
        typedef ValueType*          Pointer;
        typedef const ValueType*    ConstPointer;
        typedef ValueType&          Reference;
        typedef const ValueType&    ConstReference;

        typedef std::function<void( _t_any* )>    DeleterFn;

        struct ValContainer {
            private:
                Pointer                         m_Value;
                DeleterFn                       m_Deleter;
                libcommon::atomics::type64      m_RefCounter;
                libcommon::atomics::type64      m_WeakRefCounter;
            public:
                ValContainer( const DeleterFn& deleter, Pointer value = NULL ) : m_Value( value ), m_Deleter( deleter ) {
                    m_WeakRefCounter    = 0;
                    m_RefCounter        = 0;
                }
                ~ValContainer() {
                    if( m_Value != nullptr ) {
                        m_Deleter( m_Value );
                        m_Value = nullptr;
                    }

                    assert( m_RefCounter == 0 );

                    m_RefCounter        = 0;
                    m_Value             = nullptr;
                    m_WeakRefCounter    = 0;
                }
                void    addRef() {
                    libcommon::atomics::inc64(
                        &m_RefCounter
                    );
                }
                bool    release() {
                    assert( m_RefCounter > 0 );
                    return ( libcommon::atomics::dec64(
                                 &m_RefCounter
                             ) == 0 );
                }
                bool    empty() const {
                    return ( m_Value == NULL );
                }
                Pointer get() {
                    return m_Value;
                }
                ConstPointer get() const {
                    return m_Value;
                }
                libcommon::atomics::type64*    weakRefCounterPtr() {
                    return &m_WeakRefCounter;
                }
                libcommon::atomics::type64     weakRefCount() const {
                    return libcommon::atomics::fetch64(
                               &m_WeakRefCounter
                           );
                }
                libcommon::atomics::type64     refCount() const {
                    return libcommon::atomics::fetch64(
                               ( libcommon::Int64* )&m_RefCounter
                           );
                }
        };

        explicit SharedPtr( Pointer val = NULL ) : m_Container( nullptr ) {
            reset( val );
        }
        SharedPtr( const DeleterFn& deleter, Pointer val = NULL ) : m_Container( nullptr ) {
            reset( deleter, val );
        }
        SharedPtr( const SharedPtr& rhs ) : m_Container( nullptr ) {
            tidy();
            copy( rhs.m_Container );
        }
        SharedPtr( SharedPtr&& rhs ) : m_Container( nullptr ) {
            this->m_Container = rhs.m_Container;
            rhs.m_Container   = nullptr;
        }
        SharedPtr& operator = ( const SharedPtr& rhs ) {
            tidy();
            copy( rhs.m_Container );

            return *this;
        }
        SharedPtr& operator = ( Pointer ptr ) {
            reset( ptr );

            return *this;
        }
        SharedPtr& operator = ( SharedPtr&& rhs ) {
            ValContainer* c( this->m_Container );
            this->m_Container = rhs.m_Container;
            rhs.m_Container   = c;

            return *this;
        }
        ~SharedPtr() {
            reset();
        }

        void swap( SharedPtr& rhs ) {
            ValContainer* c( this->m_Container );
            this->m_Container = rhs.m_Container;
            rhs.m_Container   = c;
        }

        void assign( const SharedPtr& rhs ) {
            tidy();
            copy( rhs.m_Container );
        }
        template < class _t_value >
        void assign( _t_value* ptr ) {
            reset( ( Pointer )ptr );
        }
        void reset( Pointer val = NULL ) {
            tidy();
            copy(
            new ValContainer( []( _t_any * ptr ) { delete ptr;}, val )
            );
        }
        void reset( const DeleterFn& deleter, Pointer val = NULL ) {
            tidy();
            copy(
                new ValContainer( deleter, val )
            );
        }
        bool empty() const {
            return ( m_Container == NULL ) || ( m_Container->empty() );
        }
        bool valid() const {
            return ( m_Container != NULL );
        }
        libcommon::AddressType address() const {
            assert( m_Container );
            return ( libcommon::AddressType )( m_Container->get() );
        }

        Pointer get() const {
            assert( m_Container );
            return m_Container->get();
        }
        Pointer get() {
            assert( m_Container );
            return m_Container->get();
        }
        ConstPointer operator * () const {
            assert( m_Container );
            return m_Container->get();
        }
        Pointer operator * () {
            assert( m_Container );
            return m_Container->get();
        }
        ConstPointer operator -> () const {
            assert( m_Container );
            return m_Container->get();
        }
        Pointer operator -> () {
            assert( m_Container );
            return m_Container->get();
        }

        bool operator == ( ConstPointer p ) const {
            return ( ( ( libcommon::AddressType )get() ) == ( ( libcommon::AddressType )p ) );
        }
        bool operator != ( ConstPointer p ) const {
            return ( ( ( libcommon::AddressType )get() ) != ( ( libcommon::AddressType )p ) );
        }

        template < class _t_value >
        bool operator == ( const SharedPtr<_t_value>& rhs ) const {
            /// 64bit only
            return ( ( libcommon::AddressType )get() == ( libcommon::AddressType )rhs.get() );
        }
        template < class _t_value >
        bool operator != ( const SharedPtr<_t_value>& rhs ) const {
            /// 64bit only
            return ( ( libcommon::AddressType )get() != ( libcommon::AddressType )rhs.get() );
        }
        template < class _t_value >
        bool operator > ( const SharedPtr<_t_value>& rhs ) const {
            /// 64bit only
            return ( ( libcommon::AddressType )get() > ( libcommon::AddressType )rhs.get() );
        }
        template < class _t_value >
        bool operator < ( const SharedPtr<_t_value>& rhs ) const {
            /// 64bit only
            return ( ( libcommon::AddressType )get() < ( libcommon::AddressType )rhs.get() );
        }
        template < class _t_value >
        bool operator <= ( const SharedPtr<_t_value>& rhs ) const {
            /// 64bit only
            return ( ( libcommon::AddressType )get() <= ( libcommon::AddressType )rhs.get() );
        }
        template < class _t_value >
        bool operator >= ( const SharedPtr<_t_value>& rhs ) const {
            /// 64bit only
            return ( ( libcommon::AddressType )get() >= ( libcommon::AddressType )rhs.get() );
        }

        libcommon::atomics::type64 refCount() const {
            assert( m_Container );
            return ( m_Container->refCount() );
        }
        libcommon::atomics::type64 weakRefCount() const {
            assert( m_Container );
            return ( m_Container->weakRefCount() );
        }

        libcommon::WeakRef<ValueType> weakRef() const {
            assert( m_Container );

            return libcommon::WeakRef<ValueType>(
                       m_Container->get(),
                       m_Container->weakRefCounterPtr()
                   );
        }

    private:
        void copy( ValContainer* other ) {
            tidy();

            m_Container = other;

            if( m_Container != nullptr ) {
                m_Container->addRef();
            }
        }
        void tidy() {
            if( m_Container != nullptr ) {
                if( m_Container->release() ) {
                    delete m_Container;
                }
            }

            m_Container = nullptr;
        }

        ValContainer*   m_Container;
};


}
