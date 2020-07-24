
#include <libserialization++/formatters/binary/BinaryFormatter.hpp>
#include <libserialization++/formatters/json/JsonFormatter.hpp>
#include <libserialization++/SerializationProvider.hpp>

using namespace spp;

std::shared_ptr<SerializationInfo>      spp::CreateSerialization( const spp::Apis::t& api, Stream* stream ) {

    if( api == spp::Apis::Binary ) {

        spp::formatters::binary::Provider     provider;

        return provider.CreateSerialization( stream );

    } else if( api == spp::Apis::Json ) {

        spp::formatters::json::Provider     provider;

        return provider.CreateSerialization( stream );

    }

    return std::shared_ptr< SerializationInfo >( nullptr );

}
