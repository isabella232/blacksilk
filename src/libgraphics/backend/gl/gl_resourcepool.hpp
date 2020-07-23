#pragma once

#include <assert.h>
#include <vector>

#include <libcommon/scopedptr.hpp>

#include <libgraphics/fxapi.hpp>
#include <libgraphics/backend/gl/gl_resource.hpp>

namespace libgraphics {
namespace backend {
namespace gl {

/// forward
class Resource;
template < class _t_resource >
struct GenericResourcePool;

/// impl: GenericResourcePool
template < class _t_resource >
struct GenericResourcePool : public libcommon::INonCopyable {
        typedef _t_resource ResourceType;

        GenericResourcePool() {}
        virtual ~GenericResourcePool() {
            m_Resources.clear();
        }

        size_t cleanUp() {
            size_t      len( 0 );

            for( auto it = this->m_Resources.begin(); it != this->m_Resources.end(); ) {
                if( !( *it )->acquired() ) {
                    it = this->m_Resources.erase( it );
                    ++len;
                } else {
                    ++it;
                }
            }

            return len;
        }

        /// container methods
        size_t  size() const {
            return m_Resources.size();
        }

        size_t  totalCpuMemoryConsumption() {
            size_t total( 0 );

            for( auto it = m_Resources.begin(); it != m_Resources.end(); ++it ) {
                total += ( *it )->getCpuMemoryConsumption();
            }

            return total;
        }

        size_t  totalGpuMemoryConsumption() {
            size_t total( 0 );

            for( auto it = m_Resources.begin(); it != m_Resources.end(); ++it ) {
                total += ( *it )->getGpuMemoryConsumption();
            }

            return total;
        }

        bool    empty() const {
            return m_Resources.empty();
        }

        void    add( ResourceType* resource ) {
            m_Resources.push_back(
                libcommon::ScopedPtr<ResourceType>( resource )
            );
        }

        bool    remove( ResourceType* resource ) {
            for( auto it = m_Resources.begin(); it != m_Resources.end(); ++it ) {
                if( ( *it ).get() == resource ) {
                    m_Resources.erase( it );

                    return true;
                }
            }

            return false;
        }

        bool    remove( size_t index ) {
            if( m_Resources.size() > index ) {
                m_Resources.erase( m_Resources.begin() + index );

                return true;
            }

            return false;
        }

        void    clear() {
            m_Resources.clear();
        }

        size_t  countAvailable() {
            size_t count( 0 );

            for( auto it = m_Resources.begin(); it != m_Resources.end(); ++it ) {
                if( !( *it )->acquired() ) {
                    ++count;
                }
            }

            return count;
        }
        template < class _t_pred >
        size_t  countAvailable( _t_pred pred ) {
            size_t count( 0 );

            for( auto it = m_Resources.begin(); it != m_Resources.end(); ++it ) {
                if( !( *it )->acquired() && pred( ( *it ).get() ) ) {
                    ++count;
                }
            }

            return count;
        }

        size_t  countAcquired() {
            size_t count( 0 );

            for( auto it = m_Resources.begin(); it != m_Resources.end(); ++it ) {
                if( ( *it )->acquired() ) {
                    ++count;
                }
            }

            return count;
        }
        template < class _t_pred >
        size_t  countAcquired( _t_pred pred ) {
            size_t count( 0 );

            for( auto it = m_Resources.begin(); it != m_Resources.end(); ++it ) {
                if( ( *it )->acquired() && pred( ( *it ).get() ) ) {
                    ++count;
                }
            }

            return count;
        }
        template < class _t_pred >
        size_t  count( _t_pred pred ) {
            size_t count( 0 );

            for( auto it = m_Resources.begin(); it != m_Resources.end(); ++it ) {
                if( pred( ( *it ).get() ) ) {
                    ++count;
                }
            }

            return count;
        }

        ResourceType*   at( size_t index ) const {
            assert( m_Resources.size() > index );

            if( m_Resources.size() > index ) {
                return m_Resources.at( index );
            }

            return nullptr;
        }

        /// find
        template < class _t_pred >
        ResourceType*   find( _t_pred pred ) {
            for( auto it = m_Resources.begin(); it != m_Resources.end(); ++it ) {
                if( pred( ( *it ).get() ) ) {
                    return ( *it ).get();
                }
            }

            return nullptr;
        }

        template < class _t_pred >
        ResourceType*   find( _t_pred pred, size_t begin, size_t end ) {
            assert( m_Resources.size() > begin );
            assert( m_Resources.size() > end );

            if( m_Resources.size() <= begin ) {
                return nullptr;
            }

            if( m_Resources.size() <= end ) {
                return nullptr;
            }

            for( auto it = m_Resources.begin() + begin; it != ( m_Resources.begin() + end ); ++it ) {
                if( pred( ( *it ).get() ) ) {
                    return ( *it ).get();
                }
            }

            return nullptr;
        }

        /// acquire
        ResourceType*   acquireResource() {
            for( auto it = m_Resources.begin(); it != m_Resources.end(); ++it ) {
                if( ( *it )->tryAcquire() ) {
                    return ( *it ).get();
                }
            }

            return nullptr;
        }

        template < class _t_pred >
        ResourceType*   acquireResource( _t_pred pred ) {
            ResourceType* resource = find<_t_pred>( pred );

            if( resource != nullptr ) {
                resource->acquire();

                return resource;
            }

            return nullptr;
        }

        template < class _t_pred >
        ResourceType*   acquireResource( _t_pred pred, size_t begin, size_t end ) {
            ResourceType* resource = find<_t_pred>( pred, begin, end );

            if( resource != nullptr ) {
                resource->acquire();

                return resource;
            }

            return nullptr;
        }

        /// tryAcquire
        ResourceType*   tryAcquireResource() {
            for( auto it = this->m_Resources.begin(); it != this->m_Resources.end(); ++it ) {
                ResourceType* resource = ( *it );

                if( resource->tryAcquire() ) {
                    return resource;
                }
            }

            return nullptr;
        }

        template < class _t_pred >
        ResourceType*   tryAcquireResource( _t_pred pred ) {
            ResourceType* resource = find<_t_pred>( pred );

            if( resource != nullptr ) {
                if( resource->tryAcquire() ) {
                    return resource;
                }
            }

            return nullptr;
        }

        template < class _t_pred >
        ResourceType*   tryAcquireResource( _t_pred pred, size_t begin, size_t end ) {
            ResourceType* resource = find<_t_pred>( pred, begin, end );

            if( resource != nullptr ) {
                if( resource->tryAcquire() ) {
                    return resource;
                }
            }

            return nullptr;
        }
    protected:
        std::vector< libcommon::ScopedPtr< ResourceType > > m_Resources;
};
typedef GenericResourcePool<backend::gl::Resource>  ResourcePool;

}
}
}
