
#include <cstring>

#include <libserialization++/PropertyAccessor.hpp>
#include <libserialization++/Property.hpp>

static size_t     PropertyDescCounter = 0;

using namespace spp;

PropertyDesc::PropertyDesc( const StringType& name ) {

    this->m_Name                = name;
    this->m_UniqueIdentifier    = PropertyDescCounter;

    ++PropertyDescCounter;

}

void PropertyDesc::SetName( const StringType& name ) {

    this->m_Name                = name;

}

const PropertyDesc::StringType&   PropertyDesc::GetName() const {

    return this->m_Name;

}

const PropertyDesc::IdType&       PropertyDesc::GetUniqueIdentifier() const {

    return this->m_UniqueIdentifier;

}


PropertyTypeInfo::PropertyTypeInfo( const std::type_info& info, const size_t& size, bool isClass ) : TypeInfo( info ), TypeSize( size ), IsClass( isClass ) {

}

PropertyTypeInfo& PropertyTypeInfo::operator = ( const PropertyTypeInfo& info ) {
    memcpy( ( void* )&this->TypeInfo, ( const void* )&info.TypeInfo, sizeof( info.TypeInfo ) );

    this->TypeSize      = info.TypeSize;

    return *this;
}

const char* PropertyTypeInfo::TypeName() const {

    return TypeInfo.name();

}

size_t      PropertyTypeInfo::TypeHash() const {

    return TypeInfo.hash_code();

}

bool    PropertyTypeInfo::SizeSmaller( const PropertyTypeInfo& info ) const {

    return ( info.TypeSize > TypeSize );

}

bool    PropertyTypeInfo::SizeBigger( const PropertyTypeInfo& info ) const {

    return ( info.TypeSize < TypeSize );

}

bool    PropertyTypeInfo::SizeEqual( const PropertyTypeInfo& info ) const  {

    return ( info.TypeSize == TypeSize );

}

bool    PropertyTypeInfo::Equal( const PropertyTypeInfo& info ) const {

    return ( info.TypeInfo == TypeInfo );

}

bool    PropertyTypeInfo::NonEqual( const PropertyTypeInfo& info ) const {

    return !( info.TypeInfo == TypeInfo );
}

bool    PropertyTypeInfo::operator == ( const PropertyTypeInfo& info ) const {

    return ( info.TypeInfo == TypeInfo );

}

bool    PropertyTypeInfo::operator != ( const PropertyTypeInfo& info ) const {

    return !( info.TypeInfo == TypeInfo );
}


Property::Property( const PropertyDesc& desc, const PropertyTypeInfo& info, const PropertyGetter& getter, const PropertySetter& setter ) : m_TypeInfo( info ), m_Description( desc ),
    m_Getter( getter ), m_Setter( setter ) {
}

PropertyTypeInfo       Property::GetTypeInfo() const {

    return this->m_TypeInfo;

}

PropertyDesc& Property::GetDesc() {

    return this->m_Description;

}

const PropertyDesc& Property::GetDesc() const {

    return this->m_Description;

}

void Property::SetDesc( const PropertyDesc& desc ) {

    this->m_Description = desc;

}

PropertyValueType::t    Property::GetValueType() const {

    return this->m_Type;

}

void    Property::SetValueType( PropertyValueType::t valueType ) {

    this->m_Type = valueType;

}

PropertyGetter&      Property::GetGetter() {

    return this->m_Getter;

}

PropertySetter&      Property::GetSetter() {

    return this->m_Setter;

}

const PropertyGetter&      Property::GetGetter() const {

    return this->m_Getter;

}

const PropertySetter&      Property::GetSetter() const {

    return this->m_Setter;

}

Property&   Property::operator = ( const Property& prop ) {

    this->m_Getter      = prop.m_Getter;
    this->m_Setter      = prop.m_Setter;
    this->m_Type        = prop.m_Type;
    this->m_Description = prop.m_Description;

    memcpy( ( void* )&this->m_TypeInfo, ( const void* )&prop.m_TypeInfo, sizeof( prop.m_TypeInfo ) );

    return *this;

}
