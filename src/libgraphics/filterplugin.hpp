#pragma once

#include <libgraphics/base.hpp>
#include <libgraphics/filter.hpp>
#include <libgraphics/filterpreset.hpp>
#include <libgraphics/filterpresetcollection.hpp>
#include <libgraphics/image.hpp>
#include <libgraphics/fxapi.hpp>

namespace libgraphics {

class Filter;
class FilterCollection;

/// base class for filter plugins
class FilterFactory : libcommon::INonCopyable {
    public:
        virtual ~FilterFactory() {}

        /// constructs a new filter object
        virtual Filter* create() = 0;
};

/// filter plugin class
class FilterPlugin : libcommon::INonCopyable {
    public:
        virtual ~FilterPlugin() {}

        /// the filter plugin class looses the ownership of all enumerated
        /// filter classes.
        virtual size_t enumerateFilters(
            FilterCollection* collection
        ) = 0;

        /// returns the number of enumeratable filter
        /// objects
        virtual size_t countEnumeratableFilters() = 0;

        /// returns the name of the plugin
        /// object
        virtual std::string pluginName() const = 0;
};

/// generic filter plugin class
class GenericFilterPlugin : public FilterPlugin {
    public:
        GenericFilterPlugin() {}
        explicit GenericFilterPlugin( const std::string& name ) : m_Name( name ) {}

        virtual ~GenericFilterPlugin() {}

        void registerFactory( FilterFactory* factory );
        void registerFactory( const std::shared_ptr<FilterFactory>& factory );

        void clearFactories();

        /// the filter plugin class looses the ownership of all enumerated
        /// filter classes.
        virtual size_t enumerateFilters(
            FilterCollection* collection
        );

        /// returns the number of enumeratable filter
        /// objects
        virtual size_t countEnumeratableFilters();

        /// returns the name of the plugin
        /// object
        virtual std::string pluginName() const;
    protected:
        std::vector<std::shared_ptr<FilterFactory> > m_FilterFactories;
        std::string m_Name;
};

/// filter plugin api
namespace filterapi {

typedef libgraphics::Filter* ( *CallbackCreateFilter )( void );
typedef libgraphics::FilterFactory* ( *CallbackCreateFilterFactory )( void );
typedef libgraphics::FilterPlugin* ( *CallbackCreateFilterPlugin )( void );


}

/// impl: FilterPluginGroup - manages a group of filter plugin
///  objects
class FilterPluginGroup : public libcommon::INonCopyable {
    public:
        typedef std::vector<std::shared_ptr<FilterPlugin> > ContainerType;

        virtual ~FilterPluginGroup() {}

        void add( FilterPlugin* plugin );
        void add( const std::shared_ptr<FilterPlugin>& plugin );

        bool remove( FilterPlugin* plugin );
        bool remove( const std::shared_ptr<FilterPlugin>& plugin );
        bool removeByName( const std::string& pluginName );
        bool removeByIndex( size_t index );

        size_t count() const;

        void clear();

        ContainerType::iterator begin();
        ContainerType::const_iterator begin() const;
        ContainerType::const_iterator constBegin() const;

        ContainerType::iterator end();
        ContainerType::const_iterator end() const;
        ContainerType::const_iterator constEnd() const;

        bool contains( FilterPlugin* plugin ) const;
        bool containsByName( const std::string& name ) const;

        size_t enumerateAll(
            FilterCollection* filters
        );
    protected:
        ContainerType    m_Plugins;
};

}

#define LIBGRAPHICS_FILTERAPI_CALLBACKCREATEFILTER "CallbackCreateFilter"
#define LIBGRAPHICS_FILTERAPI_CALLBACKCREATEFILTERFACTORY "CallbackCreateFilterFactory"
#define LIBGRAPHICS_FILTERAPI_CALLBACKCREATEFILTERPLUGIN "CallbackCreateFilterPlugin"
