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
        typedef std::vector<std::shared_ptr<Filter> > ContainerType;

        FilterStack() {}
        virtual ~FilterStack() {}

        size_t  count() const;

        std::shared_ptr<Filter>    top();
        const std::shared_ptr<Filter>&    top() const;

        std::shared_ptr<Filter>    bottom();
        const std::shared_ptr<Filter>&    bottom() const;

        void clear();

        void pushFront( Filter* filter );
        void pushFront( const std::shared_ptr<Filter>& filter );
        void pushBack( Filter* filter );
        void pushBack( const std::shared_ptr<Filter>& filter );

        void insert( ContainerType::iterator it, Filter* filter );
        void insert( ContainerType::iterator it, const std::shared_ptr<Filter>& filter );

        ContainerType::iterator begin();
        ContainerType::const_iterator begin() const;
        ContainerType::const_iterator constBegin() const;

        ContainerType::iterator end();
        ContainerType::const_iterator end() const;
        ContainerType::const_iterator constEnd() const;

        std::shared_ptr<Filter>    byIndex( size_t index ) const;
        std::shared_ptr<Filter>    byName( const std::string& name ) const;

        bool remove( ContainerType::iterator it );
        bool remove( const std::shared_ptr<Filter>& filter );
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
