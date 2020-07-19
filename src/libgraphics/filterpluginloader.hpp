#pragma once

#include <libgraphics/base.hpp>
#include <libgraphics/filter.hpp>
#include <libgraphics/filterplugin.hpp>
#include <libgraphics/image.hpp>
#include <libgraphics/fxapi.hpp>

namespace libgraphics {

/// basic functions to load filter plugin objects
/// from dlls/shared libraries

bool isValidFilterApiLibrary(
    const std::string& path
);
bool isValidFilterPluginLibrary(
    const std::string& path
);

bool enumerateFiltersFromLibrary(
    const std::string& path,
    FilterCollection* collection
);

Filter* loadFilterFromLibrary(
    const std::string& path
);
Filter* loadFilterFromLibrary(
    const std::string& path,
    const std::string& callbackName
);
Filter* loadFilterFromCallback(
    void* callback
);

FilterPlugin* loadFilterPluginFromLibrary(
    const std::string& path
);
FilterPlugin* loadFilterPluginFromLibrary(
    const std::string& path,
    const std::string& callbackName
);
FilterPlugin* loadFilterPluginFromCallback(
    void* callback
);

FilterFactory* loadFilterFactoryFromLibrary(
    const std::string& path
);
FilterFactory* loadFilterFactoryFromLibrary(
    const std::string& path,
    const std::string& callbackName
);
FilterFactory* loadFilterFactoryFromCallback(
    void* callback
);

}
