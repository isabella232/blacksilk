#include <libgraphics/filterplugin.hpp>
#include <libgraphics/filtercollection.hpp>
#include <QDebug>

namespace libgraphics {

void GenericFilterPlugin::registerFactory( FilterFactory* factory ) {
    this->m_FilterFactories.push_back(
        std::shared_ptr<FilterFactory>( factory )
    );
}

void GenericFilterPlugin::registerFactory( const std::shared_ptr<FilterFactory>& factory ) {
    this->m_FilterFactories.push_back(
        factory
    );
}

void GenericFilterPlugin::clearFactories() {
    this->m_FilterFactories.clear();
}

/// the filter plugin class looses the ownership of all enumerated
/// filter classes.
size_t GenericFilterPlugin::enumerateFilters(
    FilterCollection* collection
) {
    assert( collection );

    if( collection == nullptr ) {
        return 0;
    }

    size_t count( 0 );

    for( auto it = this->m_FilterFactories.begin(); it != this->m_FilterFactories.end(); ++it ) {
        const auto filterObject = ( *it )->create();
        assert( filterObject );

        if( filterObject ) {
            ++count;

            collection->add(
                filterObject
            );
        }
    }

    assert( count > 0 );

    return count;
}

/// returns the number of enumeratable filter
/// objects
size_t GenericFilterPlugin::countEnumeratableFilters() {
    return this->m_FilterFactories.size();
}

/// returns the name of the plugin
/// object
std::string GenericFilterPlugin::pluginName() const {
    return this->m_Name;
}

void FilterPluginGroup::add( FilterPlugin* plugin ) {
    this->m_Plugins.push_back(
        std::shared_ptr<FilterPlugin>( plugin )
    );
}

void FilterPluginGroup::add( const std::shared_ptr<FilterPlugin>& plugin ) {
    this->m_Plugins.push_back( plugin );
}

bool FilterPluginGroup::remove( FilterPlugin* plugin ) {
    for( auto it = this->m_Plugins.begin(); it != this->m_Plugins.end(); ++it ) {
        if( it->get() == plugin ) {
            this->m_Plugins.erase( it );

            return true;
        }
    }

    return false;
}

bool FilterPluginGroup::remove( const std::shared_ptr<FilterPlugin>& plugin ) {
    for( auto it = this->m_Plugins.begin(); it != this->m_Plugins.end(); ++it ) {
        if( ( *it ) == plugin ) {
            this->m_Plugins.erase( it );

            return true;
        }
    }

    return false;
}

bool FilterPluginGroup::removeByName( const std::string& pluginName ) {
    for( auto it = this->m_Plugins.begin(); it != this->m_Plugins.end(); ++it ) {
        if( ( *it )->pluginName() == pluginName ) {
            this->m_Plugins.erase( it );

            return true;
        }
    }

    return false;
}

bool FilterPluginGroup::removeByIndex( size_t index ) {
    assert( this->m_Plugins.size() > index );

    this->m_Plugins.erase( this->m_Plugins.begin() + index );
    return true;
}

size_t FilterPluginGroup::count() const {
    return this->m_Plugins.size();
}

void FilterPluginGroup::clear() {
    this->m_Plugins.clear();
}

FilterPluginGroup::ContainerType::iterator FilterPluginGroup::begin() {
    return this->m_Plugins.begin();
}

FilterPluginGroup::ContainerType::const_iterator FilterPluginGroup::begin() const {
    return this->m_Plugins.begin();
}

FilterPluginGroup::ContainerType::const_iterator FilterPluginGroup::constBegin() const {
    return this->m_Plugins.begin();
}

FilterPluginGroup::ContainerType::iterator FilterPluginGroup::end() {
    return this->m_Plugins.end();
}

FilterPluginGroup::ContainerType::const_iterator FilterPluginGroup::end() const {
    return this->m_Plugins.end();
}

FilterPluginGroup::ContainerType::const_iterator FilterPluginGroup::constEnd() const {
    return this->m_Plugins.end();
}

bool FilterPluginGroup::contains( FilterPlugin* plugin ) const {
    for( auto it = this->m_Plugins.begin(); it != this->m_Plugins.end(); ++it ) {
        if( it->get() == plugin ) {
            return true;
        }
    }

    return false;
}

bool FilterPluginGroup::containsByName( const std::string& name ) const {
    for( auto it = this->m_Plugins.begin(); it != this->m_Plugins.end(); ++it ) {
        if( ( *it )->pluginName() == name ) {
            return true;
        }
    }

    return false;
}

size_t FilterPluginGroup::enumerateAll(
    FilterCollection* filters
) {
    assert( filters );

    if( filters == nullptr ) {
        return 0;
    }

    size_t totalCount( 0 );

    for( auto it = this->m_Plugins.begin(); it != this->m_Plugins.end(); ++it ) {
        const auto count = ( *it )->enumerateFilters(
                               filters
                           );

        assert( count > 0 );

        if( count == 0 ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "FilterPluginGroup::enumerateAll(): Failed to enumerate filters from plugin '" << ( *it )->pluginName().c_str() << "'.";
#endif
            continue;
        }

        totalCount += totalCount;
    }

    return totalCount;
}

}
