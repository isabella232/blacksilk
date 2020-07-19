#pragma once

#include <libgraphics/base.hpp>
#include <libgraphics/filter.hpp>
#include <libgraphics/filterpreset.hpp>
#include <libgraphics/filterpresetcollection.hpp>
#include <libgraphics/image.hpp>
#include <libgraphics/fxapi.hpp>

namespace libgraphics {

class Filter;
class FilterStack : public libcommon::INonCopyable {
    public:
        typedef std::vector<libcommon::SharedPtr<Filter> > ContainerType;

        FilterStack() {}
        virtual ~FilterStack() {}

        size_t  count() const;

        libcommon::SharedPtr<Filter>    top();
        const libcommon::SharedPtr<Filter>&    top() const;

        libcommon::SharedPtr<Filter>    bottom();
        const libcommon::SharedPtr<Filter>&    bottom() const;

        void clear();

        void pushFront( Filter* filter );
        void pushFront( const libcommon::SharedPtr<Filter>& filter );
        void pushBack( Filter* filter );
        void pushBack( const libcommon::SharedPtr<Filter>& filter );

        void insert( ContainerType::iterator it, Filter* filter );
        void insert( ContainerType::iterator it, const libcommon::SharedPtr<Filter>& filter );

        ContainerType::iterator begin();
        ContainerType::const_iterator begin() const;
        ContainerType::const_iterator constBegin() const;

        ContainerType::iterator end();
        ContainerType::const_iterator end() const;
        ContainerType::const_iterator constEnd() const;

        libcommon::SharedPtr<Filter>    byIndex( size_t index ) const;
        libcommon::SharedPtr<Filter>    byName( const std::string& name ) const;

        bool remove( ContainerType::iterator it );
        bool remove( const libcommon::SharedPtr<Filter>& filter );
        bool remove( Filter* filter );
        bool removeByName( const std::string& name );
        bool removeByIndex( size_t index );

        bool process(
            libgraphics::ImageLayer*    destination,
            libgraphics::ImageLayer*    source
        );
        bool process(
            fxapi::ApiBackendDevice*    device,
            libgraphics::ImageLayer*    destination,
            libgraphics::ImageLayer*    source
        );
    protected:
        ContainerType  m_Filters;
};

}
