
#include <libserialization++/SerializationFormatter.hpp>
#include <libserialization++/SerializationInfo.hpp>
#include <libserialization++/Serializer.hpp>

using namespace spp;

bool spp::AutoSerializable::Serialize( SerializationInfo* serializationInfo ) {
    return serializationInfo->GetFormatter()->Serialize( this->GetProperties() );
}

bool spp::AutoSerializable::Deserialize( SerializationInfo* serializationInfo ) {
    return serializationInfo->GetFormatter()->Deserialize( this->GetProperties() );
}
