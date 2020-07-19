#include <libgraphics/filtercollection.hpp>
#include <libgraphics/filterpluginloader.hpp>
#include <QLibrary>
#include <QLibraryInfo>
#include <QDebug>

namespace libgraphics {

bool isValidFilterApiLibrary(
    const std::string& path
) {
    QLibrary library( path.c_str() );

    if( library.load() ) {
        const auto contains_CallbackCreateFilter =
            library.resolve( LIBGRAPHICS_FILTERAPI_CALLBACKCREATEFILTER ) != nullptr;
        const auto contains_CallbackCreateFilterFactory =
            library.resolve( LIBGRAPHICS_FILTERAPI_CALLBACKCREATEFILTERFACTORY ) != nullptr;
        const auto contains_CallbackCreateFilterPlugin =
            library.resolve( LIBGRAPHICS_FILTERAPI_CALLBACKCREATEFILTERPLUGIN ) != nullptr;

        return contains_CallbackCreateFilter || contains_CallbackCreateFilterFactory || contains_CallbackCreateFilterPlugin;
    }

    return false;
}

bool isValidFilterPluginLibrary(
    const std::string& path
) {
    QLibrary library( path.c_str() );

    if( library.load() ) {
        const auto contains_CallbackCreateFilterPlugin =
            library.resolve( LIBGRAPHICS_FILTERAPI_CALLBACKCREATEFILTERPLUGIN ) != nullptr;

        return contains_CallbackCreateFilterPlugin;
    }

    return false;
}

bool enumerateFiltersFromLibrary(
    const std::string& path,
    FilterCollection* collection
) {
    QLibrary library( path.c_str() );

    if( library.load() ) {
        const auto contains_CallbackCreateFilter =
            library.resolve( LIBGRAPHICS_FILTERAPI_CALLBACKCREATEFILTER ) != nullptr;
        const auto contains_CallbackCreateFilterFactory =
            library.resolve( LIBGRAPHICS_FILTERAPI_CALLBACKCREATEFILTERFACTORY ) != nullptr;
        const auto contains_CallbackCreateFilterPlugin =
            library.resolve( LIBGRAPHICS_FILTERAPI_CALLBACKCREATEFILTERPLUGIN ) != nullptr;

        if( !contains_CallbackCreateFilter && !contains_CallbackCreateFilterFactory && !contains_CallbackCreateFilterPlugin ) {
            assert( false );
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "libgraphics::enumerateFiltersFromLibrary(): Specified library is not valid.";
#endif
            return false;
        }

        if( contains_CallbackCreateFilter ) {
            Filter* filterObject = libgraphics::loadFilterFromCallback(
                                       ( void* )library.resolve( LIBGRAPHICS_FILTERAPI_CALLBACKCREATEFILTER )
                                   );
            assert( filterObject );

            if( !filterObject ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
                qDebug() << "libgraphics::enumerateFiltersFromLibrary(): Failed to create valid Filter object from CallbackCreateFilter function.";
#endif
                return false;
            }

            collection->add(
                filterObject
            );
        }

        if( contains_CallbackCreateFilterFactory ) {
            FilterFactory* factory = libgraphics::loadFilterFactoryFromCallback(
                                         ( void* )library.resolve( LIBGRAPHICS_FILTERAPI_CALLBACKCREATEFILTERFACTORY )
                                     );
            assert( factory );

            if( !factory ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
                qDebug() << "libgraphics::enumerateFiltersFromLibrary(): Failed to create valid FilterFactory object from CallbackCreateFilterFactory function.";
#endif
                return false;
            }

            const auto filterObject = factory->create();

            assert( filterObject );

            if( !filterObject ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
                qDebug() << "libgraphics::enumerateFiltersFromLibrary(): Failed to create valid Filter object from FilterFactory instance.";
#endif
                delete factory;

                return false;
            }

            collection->add(
                filterObject
            );

            delete factory;
        }

        if( contains_CallbackCreateFilterPlugin ) {
            FilterPlugin* plugin = libgraphics::loadFilterPluginFromCallback(
                                       ( void* )library.resolve( LIBGRAPHICS_FILTERAPI_CALLBACKCREATEFILTERPLUGIN )
                                   );
            assert( plugin );

            if( !plugin ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
                qDebug() << "libgraphics::enumerateFiltersFromLibrary(): Failed to create valid FilterPlugin object from CallbackCreateFilterPlugin function.";
#endif
                return false;
            }

            const auto enumerated = plugin->enumerateFilters(
                                        collection
                                    );
            assert( enumerated > 0 );

            if( enumerated == 0 ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
                qDebug() << "libgraphics::enumerateFiltersFromLibrary(): Failed to enumerate Filter objects from FilterPlugin instance.";
#endif
                delete plugin;

                return false;
            }

            delete plugin;
        }


        return true;
    }

    return false;
}

Filter* loadFilterFromLibrary(
    const std::string& path
) {
    return loadFilterFromLibrary(
               path,
               LIBGRAPHICS_FILTERAPI_CALLBACKCREATEFILTER
           );
}

Filter* loadFilterFromLibrary(
    const std::string& path,
    const std::string& callbackName
) {
    QLibrary library( path.c_str() );

    if( library.load() ) {
        const auto containsCallback =
            library.resolve( callbackName.c_str() ) != nullptr;
        assert( containsCallback );

        if( !containsCallback ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "libgraphics::loadFilterFromLibrary(): Failed to load filter from library. Invalid callback.";
#endif

            return nullptr;
        }

        return loadFilterFromCallback(
                   ( void* )library.resolve( callbackName.c_str() )
               );
    }

    return nullptr;
}

Filter* loadFilterFromCallback(
    void* callback
) {
    assert( callback );

    return ( ( libgraphics::filterapi::CallbackCreateFilter )callback )();
}

FilterPlugin* loadFilterPluginFromLibrary(
    const std::string& path
) {
    return loadFilterPluginFromLibrary(
               path,
               LIBGRAPHICS_FILTERAPI_CALLBACKCREATEFILTERPLUGIN
           );
}

FilterPlugin* loadFilterPluginFromLibrary(
    const std::string& path,
    const std::string& callbackName
) {
    QLibrary library( path.c_str() );

    if( library.load() ) {
        const auto containsCallback =
            library.resolve( callbackName.c_str() ) != nullptr;
        assert( containsCallback );

        if( !containsCallback ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "libgraphics::loadFilterPluginFromLibrary(): Failed to load FilterPlugin instance from library. Invalid callback.";
#endif

            return nullptr;
        }

        return loadFilterPluginFromCallback(
                   ( void* )library.resolve( callbackName.c_str() )
               );
    }

    return nullptr;
}

FilterPlugin* loadFilterPluginFromCallback(
    void* callback
) {
    assert( callback );

    return ( ( libgraphics::filterapi::CallbackCreateFilterPlugin )callback )();
}

FilterFactory* loadFilterFactoryFromLibrary(
    const std::string& path
) {
    return loadFilterFactoryFromLibrary(
               path,
               LIBGRAPHICS_FILTERAPI_CALLBACKCREATEFILTERFACTORY
           );
}

FilterFactory* loadFilterFactoryFromLibrary(
    const std::string& path,
    const std::string& callbackName
) {
    QLibrary library( path.c_str() );

    if( library.load() ) {
        const auto containsCallback =
            library.resolve( callbackName.c_str() ) != nullptr;
        assert( containsCallback );

        if( !containsCallback ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "libgraphics::loadFilterFactoryFromLibrary(): Failed to load FilterFactory instance from library. Invalid callback.";
#endif

            return nullptr;
        }

        return loadFilterFactoryFromCallback(
                   ( void* )library.resolve( callbackName.c_str() )
               );
    }

    return nullptr;
}

FilterFactory* loadFilterFactoryFromCallback(
    void* callback
) {
    assert( callback );

    return ( ( libgraphics::filterapi::CallbackCreateFilterFactory )callback )();
}

}
