#pragma once

#include <libgraphics/base.hpp>
#include <libgraphics/filterpreset.hpp>
#include <libgraphics/filterpresetcollection.hpp>
#include <libgraphics/image.hpp>
#include <libgraphics/fxapi.hpp>

namespace libgraphics {

/// impl: Filter, base class for all different kinds of filter objects
class Filter : public libcommon::INonCopyable {
    public:
        Filter( const std::string& _name, fxapi::ApiBackendDevice* _device );
        explicit Filter( fxapi::ApiBackendDevice* _device );
        virtual ~Filter() {}

        std::string& name();
        const std::string& name() const;

        void setName( const std::string& name );

        fxapi::ApiBackendDevice* backend() const;
        void setBackend( fxapi::ApiBackendDevice* _backend );

        virtual bool process(
            libgraphics::ImageLayer*    destination,
            libgraphics::ImageLayer*    source
        ) = 0;
        virtual bool process(
            fxapi::ApiBackendDevice*    device,
            libgraphics::ImageLayer*    destination,
            libgraphics::ImageLayer*    source
        ) = 0;

        virtual Filter* clone() = 0;
        virtual FilterPreset toPreset() const = 0;
        virtual bool fromPreset( const FilterPreset& preset ) = 0;
    protected:
        std::string m_Name;
        fxapi::ApiBackendDevice* m_Device;
};

/// applies the given filter to the specified
/// Image or ImageLayer instance.
bool applyFilter(
    fxapi::ApiBackendDevice* backend,
    Filter* filter,
    libgraphics::ImageLayer* destination
);
bool applyFilter(
    fxapi::ApiBackendDevice* backend,
    Filter* filter,
    libgraphics::ImageLayer* destination,
    libgraphics::ImageLayer* source
);
bool applyFilter(
    fxapi::ApiBackendDevice* backend,
    Filter* filter,
    libgraphics::Image* destination
);
bool applyFilter(
    fxapi::ApiBackendDevice* backend,
    Filter* filter,
    libgraphics::Image* destination,
    libgraphics::Image* source
);


}
