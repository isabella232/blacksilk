#pragma once

#include <libgraphics/base.hpp>
#include <libgraphics/bitmap.hpp>
#include <libgraphics/io/pipelineobject.hpp>

#include <libcommon/scopedptr.hpp>
#include <libcommon/atomics.hpp>
#include <libcommon/noncopyable.hpp>

#include <vector>

namespace libgraphics {
namespace io {

template < class _t_any >
struct PipelineObjectGroup {
        typedef _t_any ValueType;
        typedef _t_any& Reference;
        typedef const _t_any& ConstReference;
        typedef _t_any* Pointer;
        typedef const _t_any* ConstPointer;

        typedef std::shared_ptr<ValueType> ElementType;
        typedef std::vector<ElementType> ContainerType;
        typedef typename ContainerType::iterator Iterator;
        typedef typename ContainerType::const_iterator ConstIterator;

        PipelineObjectGroup() {}
        PipelineObjectGroup( const PipelineObjectGroup& rhs ) : m_Container( rhs.m_Container ) {}
        PipelineObjectGroup( Iterator begin, Iterator end ) : m_Container( begin, end ) {}

        bool add( Pointer ptr ) {
            if( !contains( ptr ) ) {
                m_Container.push_back( ElementType( ptr ) );
                return true;
            }

            return false;
        }
        bool add( const ElementType& elem ) {
            if( !contains( elem.get() ) ) {
                m_Container.push_back( elem );
                return true;
            }

            return false;
        }

        bool remove( Pointer ptr ) {
            for( auto it = m_Container.begin(); it != m_Container.end(); ++it ) {
                if( ( *it ).get() == ptr ) {
                    m_Container.erase( it );
                    return true;
                }
            }

            return false;
        }
        bool remove( const ElementType& elem ) {
            return remove( elem.get() );
        }
        bool removeByIndex( size_t index ) {
            const auto it = m_Container.begin() + index;

            if( it != m_Container.end() ) {
                m_Container.erase( it );
                return true;
            }

            return false;
        }
        bool removeByName( const char* name ) {
            for( auto it = m_Container.begin(); it != m_Container.end(); ++it ) {
                if( std::string( ( *it )->name() ) == name ) {
                    m_Container.erase( it );
                    return true;
                }
            }

            return false;
        }
        size_t removeByExtension( const char* extension ) {
            size_t count( 0 );

            for( auto it = m_Container.begin(); it != m_Container.end(); ++it ) {
                if( ( *it )->supportsExtension( extension ) ) {
                    it = m_Container.erase( it );

                    if( ( it != m_Container.end() && ( it != m_Container.begin() ) ) ) {
                        --it;

                    }

                    ++count;
                }
            }

            return count;
        }
        template < class _t_pred >
        size_t removeByPredicate( const _t_pred& pred ) {
            size_t count( 0 );

            for( auto it = m_Container.begin(); it != m_Container.end(); ++it ) {
                if( pred( ( *it ).get() ) ) {
                    it = m_Container.erase( it );

                    if( ( it != m_Container.end() && ( it != m_Container.begin() ) ) ) {
                        --it;

                    }

                    ++count;
                }
            }

            return count;
        }


        PipelineObjectGroup<_t_any> selectByExtension( const char* extension ) {
            PipelineObjectGroup<_t_any> grp;

            for( auto it = m_Container.begin(); it != m_Container.end(); ++it ) {
                if( ( *it )->supportsExtension( extension ) ) {
                    grp.add( ( *it ) );
                }
            }

            return grp;
        }
        template < class _t_pred >
        PipelineObjectGroup<_t_any> selectByPredicate( const _t_pred& pred ) {
            PipelineObjectGroup<_t_any> grp;

            for( auto it = m_Container.begin(); it != m_Container.end(); ++it ) {
                if( pred( ( *it ).get() ) ) {
                    grp.add( ( *it ) );
                }
            }

            return grp;
        }

        size_t count() {
            return size();
        }
        size_t countByExtension( const char* extension ) {
            size_t count( 0 );

            for( auto it = m_Container.begin(); it != m_Container.end(); ++it ) {
                if( ( *it )->supportsExtension( extension ) ) {
                    ++count;
                }
            }

            return count;
        }
        template < class _t_pred >
        size_t countByPredicate( const _t_pred& pred ) {
            size_t count( 0 );

            for( auto it = m_Container.begin(); it != m_Container.end(); ++it ) {
                if( pred( ( *it ).get() ) ) {
                    ++count;
                }
            }

            return count;
        }
        bool contains( const ElementType& elem ) const {
            return contains( elem.get() );
        }
        bool contains( ConstPointer ptr ) const {
            for( auto it = m_Container.begin(); it != m_Container.end(); ++it ) {
                if( it->get() == ptr ) {
                    return true;
                }
            }

            return false;
        }

        void clear() {
            m_Container.clear();
        }
        bool empty() const {
            return m_Container.empty();
        }
        size_t size() const {
            return m_Container.size();
        }

        Iterator begin() {
            return m_Container.begin();
        }
        ConstIterator begin() const {
            return m_Container.begin();
        }
        ConstIterator constBegin() const {
            return m_Container.begin();
        }
        Iterator end() {
            return m_Container.end();
        }
        ConstIterator end() const {
            return m_Container.end();
        }
        ConstIterator constEnd() const {
            return m_Container.end();
        }

        Pointer at( const size_t& index ) const {
            assert( size() > index );
            return ( begin() + index ).get();
        }
        Pointer front() {
            return m_Container.front().get();
        }
        ConstPointer front() const {
            return m_Container.front().get();
        }
        Pointer back() {
            return m_Container.back().get();
        }
        ConstPointer back() const {
            return m_Container.back().get();
        }

        Iterator erase( Iterator pos ) {
            return m_Container.erase( pos );
        }
        Iterator erase( Iterator _begin, Iterator _end ) {
            return m_Container.erase( _begin, _end );
        }

    private:
        ContainerType   m_Container;
};
typedef PipelineObjectGroup<PipelineObject> StdPipelineObjectGroup;

}
}
