#pragma once

#include <libgraphics/base.hpp>
#include <libgraphics/filter.hpp>
#include <libgraphics/filterpreset.hpp>
#include <libgraphics/filterpresetcollection.hpp>
#include <libgraphics/image.hpp>
#include <libgraphics/fxapi.hpp>

namespace libgraphics {

class Filter;
class FilterCollection : public libcommon::INonCopyable {
    public:
        typedef std::vector<std::shared_ptr<Filter> > ContainerType;

        virtual ~FilterCollection() {}

        void add( Filter* filter );
        void add( const std::shared_ptr<Filter>& filter );

        bool contains( Filter* filter ) const;
        bool containsByName( const std::string& name ) const;

        bool remove( Filter* filter );
        bool removeByName( const std::string& name );

        size_t count() const;

        ContainerType::iterator begin();
        ContainerType::const_iterator begin() const;
        ContainerType::const_iterator constBegin() const;

        ContainerType::iterator end();
        ContainerType::const_iterator end() const;
        ContainerType::const_iterator constEnd() const;

        void clear();

        std::shared_ptr<Filter>    byName( const std::string& name );
    protected:
        ContainerType  m_Filters;
};

}
