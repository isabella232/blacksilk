
#include <libserialization++/PropertyCollection.hpp>

using namespace spp;

PropertyCollection::PropertyCollection( const std::string& name, const spp::PropertyTypeInfo& info ) : m_OwnerObjectInfo( info ) {
    this->m_OwnerObject = 0;
    this->m_Name        = name;
}

PropertyCollection::PropertyCollection( const PropertyCollection& collection ) : m_OwnerObjectInfo( collection.m_OwnerObjectInfo ) {

    this->m_Properties  = collection.m_Properties;
    this->m_OwnerObject = collection.m_OwnerObject;
    this->m_Name        = collection.m_Name;

}

PropertyCollection::~PropertyCollection() {

}

PropertyCollection& PropertyCollection::operator << ( const Property& property ) {

    Add( property );

    return *this;

}

PropertyCollection& PropertyCollection::operator << ( const PropertyCollection& collection ) {

    Add( collection );

    return *this;

}

bool PropertyCollection::Add( const Property& property ) {

    bool found( false );

    for( auto it = this->m_Properties.begin(); it != this->m_Properties.end(); ++it ) {
        if( ( *it ).GetDesc().GetUniqueIdentifier() == property.GetDesc().GetUniqueIdentifier() ) {
            found = true;

            break;
        }
    }

    if( !found ) {

        Property prop( property );

        prop.GetGetter().Bind( this->m_OwnerObjectInfo, m_OwnerObject );
        prop.GetSetter().Bind( this->m_OwnerObjectInfo, m_OwnerObject );

        this->m_Properties.push_back( prop );

        return true;

    }

    return false;

}

bool PropertyCollection::Add( const PropertyCollection& collection ) {

    bool result( true );

    for( auto it = collection.Begin(); it != collection.End(); ++it ) {
        if( !this->Add( *it ) ) {
            result = false;
            break;
        }
    }

    return result;

}

bool    PropertyCollection::Bind( const spp::PropertyTypeInfo& typeInfo, void* ptr ) {

    bool result( true );

    this->m_OwnerObject = ptr;

    for( auto it = Begin(); it != End(); ++it ) {
        if( !( *it ).GetSetter().Bind( typeInfo, ptr ) ) {
            result = false;
            break;
        }

        if( !( *it ).GetGetter().Bind( typeInfo, ptr ) ) {
            result = false;
            break;
        }
    }

    return result;

}

bool    PropertyCollection::Bind( PropertyObjectHandle& handle ) {

    bool result( true );

    this->m_OwnerObject     = ( void* )handle.GetBindingPtr();

    for( auto it = Begin(); it != End(); ++it ) {
        if( !( *it ).GetSetter().Bind( handle.GetTypeInfo(), ( void* )handle.GetBindingPtr() ) ) {
            result = false;
            break;
        }

        if( !( *it ).GetGetter().Bind( handle.GetTypeInfo(), ( void* )handle.GetBindingPtr() ) ) {
            result = false;
            break;
        }
    }

    return result;
}

PropertyCollection::Iterator         PropertyCollection::Begin() {

    return this->m_Properties.begin();

}

PropertyCollection::ConstIterator    PropertyCollection::Begin() const {

    return this->m_Properties.begin();

}

PropertyCollection::ConstIterator    PropertyCollection::BeginConst() const {

    return this->m_Properties.cbegin();

}

PropertyCollection::Iterator         PropertyCollection::End() {

    return this->m_Properties.end();

}

PropertyCollection::ConstIterator    PropertyCollection::End() const {

    return this->m_Properties.end();

}

PropertyCollection::ConstIterator    PropertyCollection::EndConst() const {

    return this->m_Properties.cend();

}

size_t      PropertyCollection::Size() const {

    return this->m_Properties.size();

}

void        PropertyCollection::Clear() {

    this->m_Properties.clear();

}

bool    PropertyCollection::RemovePropertyById( const size_t& id ) {

    bool result( false );

    for( auto it = Begin(); it != End(); ++it ) {
        if( ( *it ).GetDesc().GetUniqueIdentifier() == id ) {

            this->m_Properties.erase( it );

            result = true;

            break;

        }
    }

    return result;

}

bool    PropertyCollection::RemovePropertyByName( const std::string& name ) {

    bool result( false );

    for( auto it = Begin(); it != End(); ++it ) {
        if( ( *it ).GetDesc().GetName() == name ) {

            this->m_Properties.erase( it );

            result = true;

            break;

        }
    }

    return result;


}

Property& PropertyCollection::GetPropertyByName( const std::string& name ) {

    for( auto it = Begin(); it != End(); ++it ) {
        if( ( *it ).GetDesc().GetName() == name ) {

            return *it;

        }
    }

    return *m_Properties.begin();

}

Property& PropertyCollection::GetPropertyById( size_t id ) {

    for( auto it = Begin(); it != End(); ++it ) {
        if( ( *it ).GetDesc().GetUniqueIdentifier() == id ) {

            return *it;

        }
    }

    return *m_Properties.begin();

}


std::string PropertyCollection::GetName() const {

    return this->m_Name;

}

void        PropertyCollection::SetName( const std::string& name ) {

    this->m_Name    = name;

}

