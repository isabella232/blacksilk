#pragma once

#include <libcommon/scopedptr.hpp>
#include <libcommon/noncopyable.hpp>
#include <libcommon/atomics.hpp>
#include <libcommon/guards.hpp>

#include <algorithm>
#include <vector>
#include <memory>
#include <mutex>

namespace libgraphics {
namespace detail {
template < class _t_allocatable >
struct AllocatorBase {
    typedef _t_allocatable      ValueType;
    typedef ValueType*          Pointer;
    typedef const Pointer       ConstPointer;
    typedef ValueType&          Reference;
    typedef const ValueType&    ConstReference;

    static const size_t Alignment = 64;
};
}

template < class _t_value_type >
struct Allocator : detail::AllocatorBase<_t_value_type> {
    typedef detail::AllocatorBase<_t_value_type>    BaseType;

    typedef typename BaseType::ValueType ValueType;
    typedef typename BaseType::Pointer Pointer;
    typedef typename BaseType::ConstPointer ConstPointer;
    typedef typename BaseType::Reference Reference;
    typedef typename BaseType::ConstReference ConstReference;

    Pointer alloc() const throw() {
        return new ValueType();
    }
    /** legacy c++ code **/
    template < class _t_arg0 >
    Pointer alloc( _t_arg0 arg0 ) const throw() {
        return new ValueType(
                   std::forward<_t_arg0>( arg0 )
               );
    }
    template < class _t_arg0, class _t_arg1 >
    Pointer alloc( _t_arg0 arg0, _t_arg1 arg1 ) const throw() {
        return new ValueType(
                   std::forward<_t_arg0>( arg0 ),
                   std::forward<_t_arg1>( arg1 )
               );
    }
    template < class _t_arg0, class _t_arg1, class _t_arg2 >
    Pointer alloc( _t_arg0 arg0, _t_arg1 arg1, _t_arg2 arg2 ) const throw() {
        return new ValueType(
                   std::forward<_t_arg0>( arg0 ),
                   std::forward<_t_arg1>( arg1 ),
                   std::forward<_t_arg2>( arg2 )
               );
    }
    template < class _t_arg0, class _t_arg1, class _t_arg2, class _t_arg3 >
    Pointer alloc( _t_arg0 arg0, _t_arg1 arg1, _t_arg2 arg2, _t_arg3 arg3 ) const throw() {
        return new ValueType(
                   std::forward<_t_arg0>( arg0 ),
                   std::forward<_t_arg1>( arg1 ),
                   std::forward<_t_arg2>( arg2 ),
                   std::forward<_t_arg3>( arg3 )
               );
    }
    template < class _t_arg0, class _t_arg1, class _t_arg2, class _t_arg3, class _t_arg4 >
    Pointer alloc( _t_arg0 arg0, _t_arg1 arg1, _t_arg2 arg2, _t_arg3 arg3, _t_arg4 arg4 ) const throw() {
        return new ValueType(
                   std::forward<_t_arg0>( arg0 ),
                   std::forward<_t_arg1>( arg1 ),
                   std::forward<_t_arg2>( arg2 ),
                   std::forward<_t_arg3>( arg3 ),
                   std::forward<_t_arg3>( arg4 )
               );
    }
    template < class _t_arg0, class _t_arg1, class _t_arg2, class _t_arg3, class _t_arg4, class _t_arg5 >
    Pointer alloc( _t_arg0 arg0, _t_arg1 arg1, _t_arg2 arg2, _t_arg3 arg3, _t_arg4 arg4, _t_arg5 arg5 ) const throw() {
        return new ValueType(
                   std::forward<_t_arg0>( arg0 ),
                   std::forward<_t_arg1>( arg1 ),
                   std::forward<_t_arg2>( arg2 ),
                   std::forward<_t_arg3>( arg3 ),
                   std::forward<_t_arg3>( arg4 ),
                   std::forward<_t_arg3>( arg5 )
               );
    }

    Pointer inplaceAlloc( void* p ) const {
        return new( ( void* )p ) ValueType();
    }
    template < class _t_arg0, class _t_arg1 >
    Pointer inplaceAlloc( void* p, _t_arg0 arg0, _t_arg1 arg1 ) const throw() {
        return new( ( void* )p ) ValueType(
                   std::forward<_t_arg0>( arg0 ),
                   std::forward<_t_arg1>( arg1 )
               );
    }
    template < class _t_arg0, class _t_arg1, class _t_arg2 >
    Pointer inplaceAlloc( void* p, _t_arg0 arg0, _t_arg1 arg1, _t_arg2 arg2 ) const throw() {
        return new( ( void* )p ) ValueType(
                   std::forward<_t_arg0>( arg0 ),
                   std::forward<_t_arg1>( arg1 ),
                   std::forward<_t_arg2>( arg2 )
               );
    }
    template < class _t_arg0, class _t_arg1, class _t_arg2, class _t_arg3 >
    Pointer inplaceAlloc( void* p, _t_arg0 arg0, _t_arg1 arg1, _t_arg2 arg2, _t_arg3 arg3 ) const throw() {
        return new( ( void* )p ) ValueType(
                   std::forward<_t_arg0>( arg0 ),
                   std::forward<_t_arg1>( arg1 ),
                   std::forward<_t_arg2>( arg2 ),
                   std::forward<_t_arg3>( arg3 )
               );
    }
    template < class _t_arg0, class _t_arg1, class _t_arg2, class _t_arg3, class _t_arg4 >
    Pointer inplaceAlloc( void* p, _t_arg0 arg0, _t_arg1 arg1, _t_arg2 arg2, _t_arg3 arg3, _t_arg4 arg4 ) const throw() {
        return new( ( void* )p ) ValueType(
                   std::forward<_t_arg0>( arg0 ),
                   std::forward<_t_arg1>( arg1 ),
                   std::forward<_t_arg2>( arg2 ),
                   std::forward<_t_arg3>( arg3 ),
                   std::forward<_t_arg3>( arg4 )
               );
    }
    template < class _t_arg0, class _t_arg1, class _t_arg2, class _t_arg3, class _t_arg4, class _t_arg5 >
    Pointer inplaceAlloc( void* p, _t_arg0 arg0, _t_arg1 arg1, _t_arg2 arg2, _t_arg3 arg3, _t_arg4 arg4, _t_arg5 arg5 ) const throw() {
        return new( ( void* )p ) ValueType(
                   std::forward<_t_arg0>( arg0 ),
                   std::forward<_t_arg1>( arg1 ),
                   std::forward<_t_arg2>( arg2 ),
                   std::forward<_t_arg3>( arg3 ),
                   std::forward<_t_arg3>( arg4 ),
                   std::forward<_t_arg3>( arg5 )
               );
    }
    bool canAllocate() const {
        return true;
    }
    void dealloc( Pointer p ) const {
        assert( p );
        delete p;
    }
    void destroy( Pointer p ) const {
        assert( p );
        p->~ValueType();
    }
};

template < class _t_policy >
struct DynamicPoolAllocator : libcommon::INonCopyable {
        typedef void*   Pointer;
        typedef const void* ConstPointer;
        typedef _t_policy Policy;

        struct Entry : libcommon::INonCopyable {
            size_t                          length;
            libcommon::atomics::type32      used;
            void*                           data;

            Entry( size_t _length ) : length( _length ), used( 0 ), data( nullptr ) {
                data = ( void* )new char[length];

                assert( data != nullptr );
                assert( length != 0 );
            }

            Entry( Entry&& rhs ) throw() : length( rhs.length ), used( rhs.used ), data( rhs.data ) {
                rhs.data = nullptr;
                rhs.used = 0;
                rhs.length = 0;
            }
            Entry& operator = ( Entry&& rhs ) throw() {
                std::swap(
                    rhs.data,
                    data
                );
                std::swap(
                    rhs.used,
                    used
                );
                std::swap(
                    rhs.length,
                    length
                );

                return *this;
            }

            ~Entry() {
                assert( libcommon::atomics::equal32( &used, 0 ) == true );
                delete []( char* )data;
            }
            void acquire() {
                volatile bool _acquired = tryAcquire();

                while( !_acquired ) {
                    _acquired = tryAcquire();
                }
            }
            bool tryAcquire() {
                if( libcommon::atomics::exchange32( &used, 0, 1 ) == 0 ) {
                    return true;
                }

                return false;
            }
            void release() {
                libcommon::atomics::exchange32( &used, 1, 0 );
            }
        };

        explicit DynamicPoolAllocator( Policy _policy = Policy() ) :
            m_Size( 0 ), m_Capacity( 0 ), m_AllocatorPolicy( _policy ) {}
        template < class _t_other_policy >
        DynamicPoolAllocator( DynamicPoolAllocator<_t_other_policy>&& rhs ) :
            m_Entries( std::move( rhs.m_Entries ) ),
            m_Size( rhs.m_Size ),
            m_Capacity( rhs.m_Capacity ) {
        }
        template < class _t_other_policy >
        void assign( DynamicPoolAllocator<_t_other_policy>&& rhs ) {
            std::lock_guard<std::recursive_mutex> lock( m_Mutex );
            std::lock_guard<std::recursive_mutex> other( rhs.m_Mutex );

            m_Entries   = std::move( rhs.m_Entries );
            m_Size      = rhs.m_Size;
            m_Capacity  = rhs.m_Capacity;
            m_AllocatorPolicy   = rhs.m_AllocatorPolicy;
        }

        /// container properties
        bool empty() const {
            return m_Entries.empty();
        }
        size_t size() const {
            return m_Size;
        }
        size_t capacity() const {
            return m_Capacity;
        }

        /// policy management
        const Policy& policy() const {
            return m_AllocatorPolicy;
        }
        Policy& policy() {
            return m_AllocatorPolicy;
        }
        void setPolicy( const Policy& policy ) {
            m_AllocatorPolicy = policy;
        }

        /// allocating raw data
        struct Blob {
                void*   data;

                Blob() : data( nullptr ) {}
                explicit Blob( void* _data ) :
                    data( _data ), alloc( nullptr ) {}
                Blob( void* _data, DynamicPoolAllocator<_t_policy>* _alloc ) :
                    data( _data ), alloc( _alloc ) {}
                ~Blob() {
                    assert( data );

                    if( alloc != nullptr ) {
                        alloc->dealloc(
                            data
                        );
                    }
                }
                bool empty() const { return data == nullptr; }
            private:
                DynamicPoolAllocator<_t_policy>*    alloc;
        };
        friend struct Blob;

        std::shared_ptr<Blob> alloc( size_t length ) {
            const auto& entry = acquireEntry(
                                    length
                                );
            return std::shared_ptr<Blob>(
                       new Blob( entry.data, this )
                   );
        }
        template < class _t_type >
        std::shared_ptr<_t_type> emplace() {
            static Allocator<_t_type> _allocator;
            const auto entry = acquireEntry(
                                   sizeof( _t_type )
                               );

            return std::shared_ptr<_t_type>(
            [this]( _t_type * p ) { this->dealloc( p ); },
            _allocator.inplaceAlloc( entry.data )
                   );
        }
        template < class _t_type, class _t_arg0 >
        std::shared_ptr<_t_type> emplace( _t_arg0 arg0 ) {
            static Allocator<_t_type> _allocator;
            const auto entry = acquireEntry(
                                   sizeof( _t_type )
                               );

            return std::shared_ptr<_t_type>(
            [this]( _t_type * p ) { this->dealloc( p ); },
            _allocator.inplaceAlloc(
                entry.data,
                std::forward<_t_arg0>( arg0 )
            )
                   );
        }
        template < class _t_type, class _t_arg0, class _t_arg1 >
        std::shared_ptr<_t_type> emplace( _t_arg0 arg0, _t_arg1 arg1 ) {
            static Allocator<_t_type> _allocator;
            const auto entry = acquireEntry(
                                   sizeof( _t_type )
                               );

            return std::shared_ptr<_t_type>(
            [this]( _t_type * p ) { this->dealloc( p ); },
            _allocator.inplaceAlloc(
                entry.data,
                std::forward<_t_arg0>( arg0 ),
                std::forward<_t_arg1>( arg1 )
            )
                   );
        }
        template < class _t_type, class _t_arg0, class _t_arg1, class _t_arg2 >
        std::shared_ptr<_t_type> emplace( _t_arg0 arg0, _t_arg1 arg1, _t_arg2 arg2 ) {
            static Allocator<_t_type> _allocator;
            const auto entry = acquireEntry(
                                   sizeof( _t_type )
                               );

            return std::shared_ptr<_t_type>(
            [this]( _t_type * p ) { this->dealloc( p ); },
            _allocator.inplaceAlloc(
                entry.data,
                std::forward<_t_arg0>( arg0 ),
                std::forward<_t_arg1>( arg1 ),
                std::forward<_t_arg2>( arg2 )
            )
                   );
        }
        template < class _t_type, class _t_arg0, class _t_arg1, class _t_arg2, class _t_arg3 >
        std::shared_ptr<_t_type> emplace( _t_arg0 arg0, _t_arg1 arg1, _t_arg2 arg2, _t_arg3 arg3 ) {
            static Allocator<_t_type> _allocator;
            const auto entry = acquireEntry(
                                   sizeof( _t_type )
                               );

            return std::shared_ptr<_t_type>(
            [this]( _t_type * p ) { this->dealloc( p ); },
            _allocator.inplaceAlloc(
                entry.data,
                std::forward<_t_arg0>( arg0 ),
                std::forward<_t_arg1>( arg1 ),
                std::forward<_t_arg2>( arg2 ),
                std::forward<_t_arg3>( arg3 )
            )
                   );
        }
        template < class _t_type, class _t_arg0, class _t_arg1, class _t_arg2, class _t_arg3, class _t_arg4 >
        std::shared_ptr<_t_type> emplace( _t_arg0 arg0, _t_arg1 arg1, _t_arg2 arg2, _t_arg3 arg3, _t_arg4 arg4 ) {
            static Allocator<_t_type> _allocator;
            const auto entry = acquireEntry(
                                   sizeof( _t_type )
                               );

            return std::shared_ptr<_t_type>(
            [this]( _t_type * p ) { this->dealloc( p ); },
            _allocator.inplaceAlloc(
                entry.data,
                std::forward<_t_arg0>( arg0 ),
                std::forward<_t_arg1>( arg1 ),
                std::forward<_t_arg2>( arg2 ),
                std::forward<_t_arg3>( arg3 ),
                std::forward<_t_arg4>( arg4 )
            )
                   );
        }
        template < class _t_type, class _t_arg0, class _t_arg1, class _t_arg2, class _t_arg3, class _t_arg4, class _t_arg5 >
        std::shared_ptr<_t_type> emplace( _t_arg0 arg0, _t_arg1 arg1, _t_arg2 arg2, _t_arg3 arg3, _t_arg4 arg4, _t_arg5 arg5 ) {
            static Allocator<_t_type> _allocator;
            const auto entry = acquireEntry(
                                   sizeof( _t_type )
                               );

            return std::shared_ptr<_t_type>(
            [this]( _t_type * p ) { this->dealloc( p ); },
            _allocator.inplaceAlloc(
                entry.data,
                std::forward<_t_arg0>( arg0 ),
                std::forward<_t_arg1>( arg1 ),
                std::forward<_t_arg2>( arg2 ),
                std::forward<_t_arg3>( arg3 ),
                std::forward<_t_arg4>( arg4 ),
                std::forward<_t_arg5>( arg5 )
            )
                   );
        }
        void dealloc( void* p ) {
            assert( p );

            std::lock_guard<std::recursive_mutex> lock( m_Mutex );

            for( auto it = m_Entries.begin(); it != m_Entries.end(); ++it ) {
                if( ( ( size_t )( *it ).data == ( size_t )p ) ) {
                    if( ( libcommon::atomics::exchange32( &( *it ).used, 1, 0 ) == 1 ) ) {
                        --m_Size;
                        break;
                    } else {
                        assert( false );
                    }
                }
            }
        }
        template < class _t_type >
        void dealloc( _t_type* p ) {
            assert( p );

            static Allocator<_t_type> _allocator;
            _allocator.destroy( p );

            dealloc( ( void* )p );
        }

        /// info
        size_t queryMemoryCapacity() {
            std::lock_guard<std::recursive_mutex> lock( m_Mutex );

            size_t capacity( 0 );

            for( auto it = this->m_Entries.begin(); it != this->m_Entries.end(); ++it ) {
                capacity += ( *it ).length;
            }

            return capacity;
        }
        size_t queryMemoryConsumption() {
            std::lock_guard<std::recursive_mutex> lock( m_Mutex );

            size_t consumption( 0 );

            for( auto it = this->m_Entries.begin(); it != this->m_Entries.end(); ++it ) {
                if( it->used == 1 ) {
                    consumption += it->length;
                }
            }

            return consumption;
        }

        /// resizing
        void reserve( size_t entryCount, size_t entrySize ) {
            assert( entrySize > 0 );
            assert( entryCount > 0 );

            if( ( entryCount == 0 ) || ( entrySize == 0 ) ) {
                return;
            }

            std::lock_guard<std::recursive_mutex> lock( m_Mutex );

            for( size_t i = 0; entryCount > i; ++i ) {
                m_Entries.emplace_back(
                    entrySize
                );
            }

            m_Capacity += entryCount;

            sort();
        }
        void ensureCapacity( size_t capacity, size_t entrySize ) {
            std::lock_guard<std::recursive_mutex> lock( m_Mutex );

            size_t totalCapacity = countUnusedOfSize( entrySize ) + countUsedOfSize( entrySize );

            if( totalCapacity < capacity ) {
                reserve( capacity - totalCapacity, entrySize );
            }
        }
        void ensureUnused( size_t capacity, size_t entrySize ) {
            std::lock_guard<std::recursive_mutex> lock( m_Mutex );

            size_t unusedOfSize = countUnusedOfSize( entrySize );

            if( unusedOfSize < capacity ) {
                reserve( capacity - unusedOfSize, entrySize );
            }
        }

        size_t countUsedOfSize( size_t entrySize ) {
            std::lock_guard<std::recursive_mutex> lock( m_Mutex );

            size_t count( 0 );

            for( auto it = m_Entries.begin(); it != m_Entries.end(); ++it ) {
                if( ( ( *it ).length == entrySize ) && ( libcommon::atomics::equal32( &( *it ).used, 1 ) ) ) {
                    ++count;
                }
            }

            return count;
        }
        size_t countUsedOfCompatibleSize( size_t entrySize ) {
            std::lock_guard<std::recursive_mutex> lock( m_Mutex );

            size_t count( 0 );

            for( auto it = m_Entries.begin(); it != m_Entries.end(); ++it ) {
                if( ( ( *it ).length >= entrySize ) && ( libcommon::atomics::equal32( &( *it ).used, 1 ) ) ) {
                    ++count;
                }
            }

            return count;
        }

        size_t countUnusedOfSize( size_t entrySize ) {
            std::lock_guard<std::recursive_mutex> lock( m_Mutex );

            size_t count( 0 );

            for( auto it = m_Entries.begin(); it != m_Entries.end(); ++it ) {
                if( ( ( *it ).length == entrySize ) && ( libcommon::atomics::equal32( &( *it ).used, 0 ) ) ) {
                    ++count;
                }
            }

            return count;
        }
        size_t countUnusedOfCompatibleSize( size_t entrySize ) {
            std::lock_guard<std::recursive_mutex> lock( m_Mutex );

            size_t count( 0 );

            for( auto it = m_Entries.begin(); it != m_Entries.end(); ++it ) {
                if( ( ( *it ).length >= entrySize ) && ( libcommon::atomics::equal32( &( *it ).used, 0 ) ) ) {
                    ++count;
                }
            }

            return count;
        }

        size_t releaseUnusedOfSize( size_t entries, size_t entryLength ) {
            std::lock_guard<std::recursive_mutex> lock( m_Mutex );

            size_t freed( 0 );

            for( auto it = m_Entries.begin(); it != m_Entries.end(); ) {
                if( libcommon::atomics::equal32( &( *it ).used, 0 ) && ( ( *it ).length == entryLength ) ) {
                    it = m_Entries.erase( it );
                    ++freed;
                } else {
                    ++it;
                }

                if( freed >= entries ) {
                    break;
                }
            }

            m_Capacity      -= freed;

            return freed;
        }
        size_t releaseUnusedOfCompatibleSize( size_t entries, size_t size ) {
            std::lock_guard<std::recursive_mutex> lock( m_Mutex );

            size_t freed( 0 );

            for( auto it = m_Entries.begin(); it != m_Entries.end(); ) {
                if( libcommon::atomics::equal32( &( *it ).used, 0 ) && ( ( *it ).length >= size ) ) {
                    it = m_Entries.erase( it );
                    ++freed;
                } else {
                    ++it;
                }

                if( freed >= entries ) {
                    break;
                }
            }

            m_Capacity      -= freed;

            return freed;
        }

        size_t releaseUnused( size_t entries = 0 ) {
            std::lock_guard<std::recursive_mutex> lock( m_Mutex );

            size_t freed( 0 );

            for( auto it = m_Entries.begin(); it != m_Entries.end(); ) {
                if( libcommon::atomics::equal32( &( *it ).used, 0 ) ) {
                    it = m_Entries.erase( it );
                    ++freed;

                    if( it == m_Entries.end() ) {
                        break;
                    }
                } else {
                    ++it;
                }

                if( ( freed >= entries ) && ( entries != 0 ) ) {
                    break;
                }
            }

            m_Capacity      -= freed;

            return freed;
        }

        bool containsEntriesOfSize( size_t length ) {
            return ( countUnusedOfSize( length ) > 0 );
        }
        bool containsEntriesOfCompatibleSize( size_t length ) {
            return ( countUnusedOfCompatibleSize( length ) > 0 );
        }

        /// container management
        typedef typename std::vector<Entry>::iterator Iterator;
        typedef typename std::vector<Entry>::const_iterator ConstIterator;

        Iterator begin() {
            return m_Entries.begin();
        }
        ConstIterator begin() const {
            return m_Entries.begin();
        }
        ConstIterator constBegin() const {
            return m_Entries.cbegin();
        }

        Iterator end() {
            return m_Entries.end();
        }
        ConstIterator end() const {
            return m_Entries.end();
        }
        ConstIterator constEnd() const {
            return m_Entries.cend();
        }

        Entry& front() {
            return m_Entries.front();
        }
        const Entry& front() const {
            return m_Entries.front();
        }

        Entry& back() {
            return m_Entries.back();
        }
        const Entry& back() const {
            return m_Entries.back();
        }
    private:
        void sort() {
            std::sort(
                begin(),
                end(),
            []( const Entry & first, const Entry & second ) {
                return ( first.length > second.length );
            }
            );
        }

        Entry& acquireEntry( size_t length ) {
            const auto alignedLength            = m_AllocatorPolicy.getAlignedLength( length );
            const auto fastAlloc                = m_AllocatorPolicy.fastAlloc();
            const auto defaultReallocationCount = m_AllocatorPolicy.getDefaultReallocationCount( alignedLength );

            assert( defaultReallocationCount > 0 );
            assert( alignedLength != 0 );

            std::lock_guard<std::recursive_mutex> lock( m_Mutex );

            if( m_Capacity > m_Size ) {
                if( fastAlloc ) {
                    for( auto it = m_Entries.begin(); it != m_Entries.end(); ++it ) {
                        if( ( ( *it ).length >= alignedLength ) && ( libcommon::atomics::exchange32( &( *it ).used, 0, 1 ) == 0 ) ) {
                            ++m_Size;
                            return ( *it );
                        }
                    }
                } else {
                    for( auto it = m_Entries.begin(); it != m_Entries.end(); ++it ) {
                        if( ( ( *it ).length == alignedLength ) && ( libcommon::atomics::exchange32( &( *it ).used, 0, 1 ) == 0 ) ) {
                            ++m_Size;
                            return ( *it );
                        }
                    }
                }
            }

            reserve( defaultReallocationCount, alignedLength );

            for( auto it = m_Entries.begin(); it != m_Entries.end(); ++it ) {
                if( ( ( *it ).length == alignedLength ) && ( libcommon::atomics::exchange32( &( *it ).used, 0, 1 ) == 0 ) ) {
                    ++m_Size;
                    return ( *it );
                }
            }

            assert( false );
            return m_Entries.front();
        }

        libcommon::Int32  m_Size;
        libcommon::Int32  m_Capacity;
        Policy  m_AllocatorPolicy;
        std::vector<Entry>  m_Entries;
        std::recursive_mutex m_Mutex;
};

struct AllocatorPolicy {
        explicit AllocatorPolicy(
            size_t _defaultReallocationCount = 1,
            size_t _alignment = 32,
            bool _fastAlloc = true
        ) : m_FastAlloc( _fastAlloc ), m_DefaultReallocationCount( _defaultReallocationCount ),
            m_Alignment( _alignment ) {}

        bool fastAlloc() const { return m_FastAlloc; }
        size_t getDefaultReallocationCount( size_t ) const { return m_DefaultReallocationCount; }
        size_t getAlignedLength( size_t length ) const { return length + ( length % m_Alignment ); }
    private:
        bool m_FastAlloc;
        size_t m_DefaultReallocationCount;
        size_t m_Alignment;
};
template < size_t _defaultReallocationCount, size_t _alignment, bool _fastAlloc >
struct GenericAllocatorPolicy : AllocatorPolicy {
    GenericAllocatorPolicy() : AllocatorPolicy( _defaultReallocationCount, _alignment, _fastAlloc ) {}
};
typedef GenericAllocatorPolicy<8, 32, true> DefaultAllocatorPolicy;
typedef DynamicPoolAllocator<AllocatorPolicy> StdDynamicPoolAllocator;

template < class _t_value >
struct FixedPoolAllocator : detail::AllocatorBase<_t_value>,
    libcommon::INonCopyable {
        typedef detail::AllocatorBase<_t_value>    BaseType;

        typedef typename BaseType::ValueType ValueType;
        typedef typename BaseType::Pointer Pointer;
        typedef typename BaseType::ConstPointer ConstPointer;
        typedef typename BaseType::Reference Reference;
        typedef typename BaseType::ConstReference ConstReference;

        static const size_t EntrySize =  sizeof( _t_value ) + ( sizeof( _t_value ) % BaseType::Alignment );
        struct Entry {
            libcommon::atomics::type32      used;
            void*                           data;

            Entry() : used( 0 ), data( ( void* )new char[EntrySize] ) {}
            ~Entry() {
                assert( libcommon::atomics::equal32( &used, 0 ) == true );
                delete []( char* )data;
            }
        };
        typedef std::shared_ptr<_t_value>  InstanceType;

        FixedPoolAllocator() : m_Size( 0 ),
            m_Capacity( 0 ) {}

        /// allocations
        InstanceType    alloc() {
            auto entry = acquireEntry();
            return InstanceType( [this]( Pointer p ) { this->dealloc( p ); }, m_Alloc.inplaceAlloc( entry.data ) );
        }
        template < class _t_arg0 >
        InstanceType emplace( _t_arg0 arg0 ) {
            auto entry = acquireEntry();

            return InstanceType( [this]( Pointer p ) { this->dealloc( p ); }, m_Alloc.inplaceAlloc(
                entry.data,
                std::forward<_t_arg0>( arg0 )
            ) );
        }
        template < class _t_arg0, class _t_arg1 >
        InstanceType emplace( _t_arg0 arg0, _t_arg1 arg1 ) {
            auto entry = acquireEntry();

            return InstanceType( [this]( Pointer p ) { this->dealloc( p ); }, m_Alloc.inplaceAlloc(
                entry.data,
                std::forward<_t_arg0>( arg0 ),
                std::forward<_t_arg1>( arg1 )
            ) );
        }
        template < class _t_arg0, class _t_arg1, class _t_arg2 >
        InstanceType emplace( _t_arg0 arg0, _t_arg1 arg1, _t_arg2 arg2 ) {
            auto entry = acquireEntry();

            return InstanceType( [this]( Pointer p ) { this->dealloc( p ); }, m_Alloc.inplaceAlloc(
                entry.data,
                std::forward<_t_arg0>( arg0 ),
                std::forward<_t_arg1>( arg1 ),
                std::forward<_t_arg2>( arg2 )
            ) );
        }
        template < class _t_arg0, class _t_arg1, class _t_arg2, class _t_arg3 >
        InstanceType emplace( _t_arg0 arg0, _t_arg1 arg1, _t_arg2 arg2, _t_arg3 arg3 ) {
            auto entry = acquireEntry();

            return InstanceType( [this]( Pointer p ) { this->dealloc( p ); },  m_Alloc.inplaceAlloc(
                entry.data,
                std::forward<_t_arg0>( arg0 ),
                std::forward<_t_arg1>( arg1 ),
                std::forward<_t_arg2>( arg2 ),
                std::forward<_t_arg3>( arg3 )
            ) );
        }
        template < class _t_arg0, class _t_arg1, class _t_arg2, class _t_arg3, class _t_arg4 >
        InstanceType emplace( _t_arg0 arg0, _t_arg1 arg1, _t_arg2 arg2, _t_arg3 arg3, _t_arg4 arg4 ) {
            auto entry = acquireEntry();

            return InstanceType( [this]( Pointer p ) { this->dealloc( p ); }, m_Alloc.inplaceAlloc(
                entry.data,
                std::forward<_t_arg0>( arg0 ),
                std::forward<_t_arg1>( arg1 ),
                std::forward<_t_arg2>( arg2 ),
                std::forward<_t_arg3>( arg3 ),
                std::forward<_t_arg4>( arg4 )
            ) );
        }
        template < class _t_arg0, class _t_arg1, class _t_arg2, class _t_arg3, class _t_arg4, class _t_arg5 >
        InstanceType emplace( _t_arg0 arg0, _t_arg1 arg1, _t_arg2 arg2, _t_arg3 arg3, _t_arg4 arg4, _t_arg5 arg5 ) {
            auto entry = acquireEntry();

            return InstanceType( [this]( Pointer p ) { this->dealloc( p ); },  m_Alloc.inplaceAlloc(
                entry.data,
                std::forward<_t_arg0>( arg0 ),
                std::forward<_t_arg1>( arg1 ),
                std::forward<_t_arg2>( arg2 ),
                std::forward<_t_arg3>( arg3 ),
                std::forward<_t_arg4>( arg4 ),
                std::forward<_t_arg5>( arg5 )
            ) );
        }

        /// manual dealloc
        void dealloc( Pointer p ) {
            std::lock_guard<std::recursive_mutex> lock( m_Mutex );

            for( auto it = m_Entries.begin(); it != m_Entries.end(); ++it ) {
                if( ( ( size_t )( *it ).data == ( size_t )p ) && ( libcommon::atomics::equal32( &( *it ).used, 1 ) ) ) {
                    ( *it ).used = 0;
                    ( void ) libcommon::atomics::dec32( &m_Size );

                    m_Alloc.destroy( p );

                    return;
                }
            }
        }

        /// container management
        bool empty() const {
            return m_Entries.empty();
        }
        size_t size() const {
            return static_cast<size_t>( m_Size );
        }
        size_t capacity() const {
            return static_cast<size_t>( m_Capacity );
        }

        void reset() {
            std::lock_guard<std::recursive_mutex> lock( m_Mutex );

            m_Entries.clear();
            m_Size = 0;
            m_Capacity = 0;
        }
        void reserve( size_t entries ) {
            if( entries == 0 ) {
                return;
            }

            std::lock_guard<std::recursive_mutex> lock( m_Mutex );

            m_Entries.reserve( entries );
            libcommon::atomics::add32( &m_Capacity, ( int )entries );
        }
        void ensureCapacity( size_t capacity ) {
            std::lock_guard<std::recursive_mutex> lock( m_Mutex );

            reserve( ( capacity > m_Capacity ) ? ( capacity - m_Capacity ) : 0 );
        }
        void ensureUnsued( size_t capacity ) {
            std::lock_guard<std::recursive_mutex> lock( m_Mutex );

            reserve( ( capacity > ( m_Capacity - m_Size ) ) ? ( capacity - ( m_Capacity - m_Size ) ) : 0 );
        }

        size_t releaseUnused() { return releaseUnused( m_Capacity - m_Size ); }
        size_t releaseUnused( size_t entries ) {
            std::lock_guard<std::recursive_mutex> lock( m_Mutex );

            size_t freed( 0 );

            for( auto it = m_Entries.begin(); it != m_Entries.end(); ) {
                if( libcommon::atomics::equal32( &( *it ).used, 0 ) ) {
                    it = m_Entries.erase( it );
                    ++freed;
                } else {
                    ++it;
                }

                if( freed >= entries ) {
                    break;
                }
            }

            m_Size -= freed;

            return freed;
        }
    private:
        Entry& acquireEntry() {
            std::lock_guard<std::recursive_mutex> lock( m_Mutex );

            while( m_Size >= m_Capacity ) {
                reserve( 1 );
            }

            for( auto it = m_Entries.begin(); it != m_Entries.end(); ++it ) {
                if( libcommon::atomics::exchange32( &( *it ).used, 0, 1 ) == 0 ) {
                    ( void ) libcommon::atomics::inc32( &m_Size );

                    return ( *it );
                }
            }

            assert( false );
            return m_Entries.front();
        }

        std::recursive_mutex    m_Mutex;
        libcommon::atomics::type32  m_Size;
        libcommon::atomics::type32  m_Capacity;
        Allocator<_t_value>     m_Alloc;
        std::vector<Entry>      m_Entries;
};
namespace detail {
template < size_t len >
struct VariadicDataType {
    static const size_t length = len;
    char    data[len];

    void* get() const {
        return ( void* )&data;
    }
};
typedef VariadicDataType<32>        VarData32;
typedef VariadicDataType<32>        VarData64;
typedef VariadicDataType<32>        VarData128;
typedef VariadicDataType<32>        VarData256;
typedef VariadicDataType<32>        VarData512;
typedef VariadicDataType<32>        VarData1024;
typedef VariadicDataType<32>        VarData2048;
}
typedef FixedPoolAllocator<detail::VarData32>    FixedPoolAllocator32;
typedef FixedPoolAllocator<detail::VarData64>    FixedPoolAllocator64;
typedef FixedPoolAllocator<detail::VarData128>   FixedPoolAllocator128;
typedef FixedPoolAllocator<detail::VarData256>   FixedPoolAllocator256;
typedef FixedPoolAllocator<detail::VarData512>   FixedPoolAllocator512;
typedef FixedPoolAllocator<detail::VarData1024>  FixedPoolAllocator1024;
typedef FixedPoolAllocator<detail::VarData2048>  FixedPoolAllocator2048;


/// interface: BaseAllocator
class BaseAllocator {
    public:
        virtual ~BaseAllocator() {}

        virtual void* alloc(
            size_t length
        ) = 0;
        virtual void dealloc(
            void* ptr
        ) = 0;

        virtual bool canAllocate(
            size_t length
        ) = 0;
};

}
