
#include <libserialization++/PropertyAccessor.hpp>

using namespace spp;

PropertyObjectHandle::PropertyObjectHandle( const spp::PropertyTypeInfo& info, const void* bindingPtr ) : m_BindingType( info ) {

    this->m_BindingPtr      = ( void* )bindingPtr;

}

PropertyObjectHandle::~PropertyObjectHandle() {

    this->m_BindingPtr      = 0;

}

const void* PropertyObjectHandle::GetBindingPtr() const {

    return this->m_BindingPtr;

}

const spp::PropertyTypeInfo& PropertyObjectHandle::GetTypeInfo() const {

    return this->m_BindingType;

}

void PropertyObjectHandle::Assign( const spp::PropertyTypeInfo& info, const void* bindingPtr ) {

    if( m_BindingType.Equal( info ) ) {

        this->m_BindingPtr = ( void* )bindingPtr;

    }

}


PropertyGetter::PropertyGetter( const spp::PropertyTypeInfo& ownerTypeInfo, const spp::PropertyTypeInfo& valueTypeInfo ) : m_OwnerTypeInfo( ownerTypeInfo ), m_ValueTypeInfo( valueTypeInfo ),
    m_ObjectHandle( spp::PropertyObjectHandle( m_OwnerTypeInfo, 0 ) ) {

}

PropertyGetter::PropertyGetter( const spp::PropertyTypeInfo& ownerTypeInfo, const spp::PropertyTypeInfo& valueTypeInfo, spp::AbstractPropertyGetter* getter ) : m_OwnerTypeInfo( ownerTypeInfo ), m_ValueTypeInfo( valueTypeInfo ),
    m_ObjectHandle( spp::PropertyObjectHandle( m_OwnerTypeInfo, 0 ) ), m_Getter( getter )  {

}

PropertyGetter::PropertyGetter( const PropertyGetter& rhs ) : m_OwnerTypeInfo( rhs.m_OwnerTypeInfo ), m_ValueTypeInfo( rhs.m_ValueTypeInfo ), m_ObjectHandle( rhs.m_ObjectHandle ), m_Getter( rhs.m_Getter )  {

}

PropertyGetter::~PropertyGetter() {
    this->m_Getter.reset();
}

const spp::PropertyTypeInfo&    PropertyGetter::GetOwnerTypeInfo() const {

    return this->m_OwnerTypeInfo;

}

const spp::PropertyTypeInfo&    PropertyGetter::GetValueTypeInfo() const {

    return this->m_ValueTypeInfo;

}

const spp::PropertyObjectHandle& PropertyGetter::GetObjectHandle() const {

    return this->m_ObjectHandle;

}

bool    PropertyGetter::Bind( const spp::PropertyTypeInfo& info, void* ptr ) {

    this->m_ObjectHandle.Assign( info, ( const void* )ptr );
    this->m_Getter->Bind( m_ObjectHandle );

    return ( info.Equal( this->m_OwnerTypeInfo ) );

}

void    PropertyGetter::SetGetter( spp::AbstractPropertyGetter* getter ) {

    this->m_Getter = libcommon::SharedPtr< spp::AbstractPropertyGetter >( getter );
}

spp::AbstractPropertyGetter* PropertyGetter::GetGetter() const {

    return this->m_Getter.get();

}

bool PropertyGetter::operator()( const spp::PropertyTypeInfo& info, void* out ) {

    if( this->m_Getter != 0 ) {

        return this->m_Getter->operator()( info, out );

    }

    return false;

}


PropertySetter::PropertySetter( const spp::PropertyTypeInfo& ownerTypeInfo, const spp::PropertyTypeInfo& valueTypeInfo ) : m_OwnerTypeInfo( ownerTypeInfo ), m_ValueTypeInfo( valueTypeInfo ),
    m_ObjectHandle( spp::PropertyObjectHandle( m_OwnerTypeInfo, 0 ) ) {

}

PropertySetter::PropertySetter( const spp::PropertyTypeInfo& ownerTypeInfo, const spp::PropertyTypeInfo& valueTypeInfo, spp::AbstractPropertySetter* setter ) : m_OwnerTypeInfo( ownerTypeInfo ), m_ValueTypeInfo( valueTypeInfo ),
    m_ObjectHandle( spp::PropertyObjectHandle( m_OwnerTypeInfo, 0 ) ), m_Setter( libcommon::SharedPtr< spp::AbstractPropertySetter >( setter ) ) {

}

PropertySetter::PropertySetter( const PropertySetter& rhs ) : m_OwnerTypeInfo( rhs.m_OwnerTypeInfo ), m_ValueTypeInfo( rhs.m_ValueTypeInfo ), m_ObjectHandle( rhs.m_ObjectHandle ),
    m_Setter( rhs.m_Setter ) {

    this->m_Setter              = rhs.m_Setter;

}

PropertySetter::~PropertySetter() {

}

const spp::PropertyTypeInfo&    PropertySetter::GetOwnerTypeInfo() const {

    return this->m_OwnerTypeInfo;

}

const spp::PropertyTypeInfo&    PropertySetter::GetValueTypeInfo() const {

    return this->m_ValueTypeInfo;

}

const spp::PropertyObjectHandle& PropertySetter::GetObjectHandle() const {

    return this->m_ObjectHandle;

}

bool    PropertySetter::Bind( const spp::PropertyTypeInfo& info, void* ptr ) {

    this->m_ObjectHandle.Assign( info, ( const void* )ptr );
    this->m_Setter->Bind( m_ObjectHandle );

    return ( info.Equal( this->m_OwnerTypeInfo ) );

}

void    PropertySetter::SetSetter( spp::AbstractPropertySetter* setter ) {

    this->m_Setter = setter;

}

spp::AbstractPropertySetter* PropertySetter::GetSetter() const {

    return this->m_Setter.get();

}

bool PropertySetter::operator()( const spp::PropertyTypeInfo& info, void* out ) {

    if( this->m_Setter != 0 ) {

        return this->m_Setter->operator()( info, out );

    }

    return false;

}
