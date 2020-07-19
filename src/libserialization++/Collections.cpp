
#include <libserialization++/Collections.hpp>

using namespace spp;

PropertyContainer::PropertyContainer( const spp::PropertyTypeInfo& info ) : m_ElementTypeInfo( info ) {

}

const spp::PropertyTypeInfo&  PropertyContainer::GetElementTypeInfo() const {

    return this->m_ElementTypeInfo;

}
