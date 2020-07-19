#include <libgraphics/filtercollection.hpp>

namespace libgraphics {

void FilterCollection::add( Filter* filter ) {
    this->m_Filters.push_back(
        libcommon::SharedPtr<Filter>( filter )
    );
}

void FilterCollection::add( const libcommon::SharedPtr<Filter>& filter ) {
    this->m_Filters.push_back(
        filter
    );
}

bool FilterCollection::contains( Filter* filter ) const {
    for( auto it = this->m_Filters.begin(); it != this->m_Filters.end(); ++it ) {
        if( ( *it ).get() == filter ) {
            return true;
        }
    }

    return false;
}

bool FilterCollection::containsByName( const std::string& name ) const {
    for( auto it = this->m_Filters.begin(); it != this->m_Filters.end(); ++it ) {
        if( ( *it )->name() == name ) {
            return true;
        }
    }

    return false;
}

bool FilterCollection::remove( Filter* filter ) {
    for( auto it = this->m_Filters.begin(); it != this->m_Filters.end(); ++it ) {
        if( ( *it ).get() == filter ) {
            this->m_Filters.erase( it );

            return true;
        }
    }

    return false;
}

bool FilterCollection::removeByName( const std::string& name ) {
    for( auto it = this->m_Filters.begin(); it != this->m_Filters.end(); ++it ) {
        if( ( *it )->name() == name ) {
            this->m_Filters.erase( it );

            return true;
        }
    }

    return false;
}

size_t FilterCollection::count() const {
    return this->m_Filters.size();
}

FilterCollection::ContainerType::iterator FilterCollection::begin() {
    return this->m_Filters.begin();
}

FilterCollection::ContainerType::const_iterator FilterCollection::begin() const {
    return this->m_Filters.begin();
}

FilterCollection::ContainerType::const_iterator FilterCollection::constBegin() const {
    return this->m_Filters.begin();
}

FilterCollection::ContainerType::iterator FilterCollection::end() {
    return this->m_Filters.end();
}

FilterCollection::ContainerType::const_iterator FilterCollection::end() const {
    return this->m_Filters.end();
}

FilterCollection::ContainerType::const_iterator FilterCollection::constEnd() const {
    return this->m_Filters.end();
}

void FilterCollection::clear() {
    this->m_Filters.clear();
}

libcommon::SharedPtr<Filter> FilterCollection::byName( const std::string& name ) {
    for( auto it = this->m_Filters.begin(); it != this->m_Filters.end(); ++it ) {
        if( ( *it )->name() == name ) {
            return ( *it );
        }
    }

    assert( false );

    return libcommon::SharedPtr<Filter>();
}

}
