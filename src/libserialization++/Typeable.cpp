
#include <libserialization++/Typeable.hpp>

using namespace spp;

static TypeablePool     pool;

TypeablePool&       spp::GetGlobalTypeablePool() {

    return pool;

}

Typeable::Typeable( const spp::PropertyTypeInfo& typeInfo, TypeableDecoder* decoder, TypeableEncoder* encoder, const std::string& typeName, size_t typeId ) : m_PropertyTypeInfo( typeInfo ),
    m_Converter( TypeableConverter( typeInfo, 0 ) ) {

    this->m_TypeId          = typeId;
    this->m_TypeName        = typeName;
    this->m_Decoder         = decoder;
    this->m_Encoder         = encoder;

}

Typeable::Typeable( const Typeable& typeable ) : m_PropertyTypeInfo( typeable.m_PropertyTypeInfo ), m_Converter( typeable.m_Converter ) {

    this->m_Decoder             = typeable.m_Decoder;
    this->m_Encoder             = typeable.m_Encoder;
    this->m_TypeId              = typeable.m_TypeId;
    this->m_TypeName            = typeable.m_TypeName;

}

std::string     Typeable::GetTypeName() const {

    return this->m_TypeName;

}

void            Typeable::SetTypeName( const std::string& name ) {

    this->m_TypeName = name;

}

size_t          Typeable::GetTypeId() const {

    return this->m_TypeId;

}

void            Typeable::SetTypeId( size_t id ) {

    this->m_TypeId      = id;

}

const spp::PropertyTypeInfo&    Typeable::GetTypeInfo() const {

    return this->m_PropertyTypeInfo;

}

TypeableEncoder*  Typeable::GetEncoder() const {

    return this->m_Encoder;

}

void Typeable::SetEncoder( TypeableEncoder* decoder ) {

    this->m_Encoder = decoder;

}

TypeableDecoder*  Typeable::GetDecoder() const {

    return this->m_Decoder;

}

void Typeable::SetDecoder( TypeableDecoder* decoder ) {

    this->m_Decoder = decoder;

}

TypeableConverter  Typeable::GetConverter() {

    return this->m_Converter;

}

TypeableConverter::TypeableConverter( const spp::PropertyTypeInfo& typeInfo, ConverterFunction function ) : m_TypeInfo( typeInfo ) {

    this->m_Function        = function;

}

TypeableConverter::~TypeableConverter() {

    this->m_Function = 0;

}

bool    TypeableConverter::Match( const void* ptr, const spp::PropertyTypeInfo& typeInfo ) {

    return ( ( *m_Function )( ptr, typeInfo ) != 0 );

}

TypeableConverter::ConverterFunction               TypeableConverter::GetConverter() const {

    return this->m_Function;

}

void                            TypeableConverter::SetConverter( ConverterFunction fn ) {

    this->m_Function = fn;

}

const spp::PropertyTypeInfo&   TypeableConverter::GetTypeInfo() const {

    return this->m_TypeInfo;

}

