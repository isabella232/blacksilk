
#include <libserialization++/Collections.hpp>
#include <libserialization++/SerializationStream.hpp>
#include <libserialization++/Property.hpp>
#include <libserialization++/Serializer.hpp>
#include <libserialization++/TypeInfo.hpp>

#include <libserialization++/formatters/json/JsonParser.hpp>
#include <libserialization++/formatters/json/JsonFormatter.hpp>

#include <libserialization++/PropertyDecoder.hpp>
#include <libserialization++/PropertyEncoder.hpp>
#include <libserialization++/formatters/json/JsonElement.hpp>


using namespace spp;
using namespace spp::formatters::json;

bool        BaseReflectElementTreeContainer( libcommon::SharedPtr<JsonElement> root, spp::PropertyContainer* container );

bool        BaseReflectElementTree( libcommon::SharedPtr<JsonElement> root, spp::Property& property ) {

    if( root->GetType() == JsonValueType::Object || root->GetType() == JsonValueType::Array ) {

        bool                isContainer             = false;
        bool                hasChildren             = false;
        spp::Serializable*  parentSerializable      = 0;
        PropertyTypeInfo info                      = property.GetTypeInfo();


        void*                   elem         = 0;
        spp::Serializable*      serializable = 0;

        if( property.GetGetter().GetGetter()->operator()( info, &elem ) ) {
            serializable = ( spp::Serializable* )elem;
        }


        if( serializable != 0 ) {
            parentSerializable  = serializable;
            hasChildren         = true;
        }

        if( info == spp::GetTypeInfo< spp::PropertyContainer* >() ) {
            isContainer = true;
        }


        if( isContainer ) {

            spp::PropertyContainer* container = property.GetGetter().Get< spp::PropertyContainer* >();

            return BaseReflectElementTreeContainer( root, container );

        } else if( hasChildren ) {

            auto collection = parentSerializable->GetProperties();

            for( auto st = root->ChildBegin(); st != root->ChildEnd(); ++st ) {

                for( auto it = collection.Begin(); it != collection.End(); ++it ) {

                    if( ( *it ).GetDesc().GetName() == ( *st )->GetAnsiName() ) {

                        BaseReflectElementTree( ( *st ), *it );

                    }

                }

            }

        }

        return true;


    } else if( root->GetType() == JsonValueType::Element ) {

        PropertyEncoder::Encode( property, root->GetAnsiValue() );

        return true;

    }

    return false;

}


bool        BaseReflectElementTreeContainer( libcommon::SharedPtr<JsonElement> root, spp::PropertyContainer* container ) {

    if( root->GetType() == JsonValueType::Object || root->GetType() == JsonValueType::Array ) {

        const PropertyTypeInfo& info                = container->GetElementTypeInfo();

        if( info == spp::GetTypeInfo< std::string >() ||
                info == spp::GetTypeInfo< std::wstring> () ) {

            for( auto it = root->ChildBegin(); it != root->ChildEnd(); ++it ) {

                std::string ansiValue = ( *it )->GetAnsiValue();
                std::string ansiName  = ( *it )->GetAnsiName();

                container->AddProperty( ansiName, info, ( void* )&ansiValue );

            }

            return true;

        } else {

            for( auto it = root->ChildBegin(); it != root->ChildEnd(); ++it ) {
                std::string ansiName  = ( *it )->GetAnsiName();

                container->AddProperty( ansiName, container->GetElementTypeInfo(), 0 );

                auto lastProperty = container->GetLastProperty();

                BaseReflectElementTree( ( *it ), lastProperty );
            }

            return true;

        }

        return true;

    }

    return false;

}

bool        BaseReflectElementTree( libcommon::SharedPtr<JsonElement> root, spp::PropertyContainer* container ) {

    bool    result( true );

    for( auto it = root->ChildBegin(); it != root->ChildEnd(); ++it ) {
        result &= BaseReflectElementTreeContainer( ( *it ), container );
    }

    return result;

}

bool        BaseReflectElementTree( libcommon::SharedPtr<JsonElement> root, spp::PropertyCollection& collection ) {

    bool    result( true );

    for( auto it = root->ChildBegin(); it != root->ChildEnd(); ++it ) {
        for( auto st = collection.Begin(); st != collection.End(); ++st ) {

            if( ( *it )->GetAnsiName() == ( *st ).GetDesc().GetName() ) {

                result &= BaseReflectElementTree( *it, *st );

                break;
            }

        }
    }

    return result;

}

void        BaseReflectPropertyCollection( libcommon::SharedPtr<JsonElement> element, spp::Property& property ) {
    libcommon::SharedPtr<JsonElement>     child( new JsonElement() );

    child->SetName( property.GetDesc().GetName() );

    if( property.GetTypeInfo() != spp::GetTypeInfo<spp::Serializable*>() &&
            property.GetTypeInfo() != spp::GetTypeInfo<spp::PropertyContainer*>() &&
            !property.GetTypeInfo().IsClass ) {

        child->SetValue( spp::PropertyDecoder::Decode( property ) );

        if( property.GetTypeInfo() == spp::GetTypeInfo<std::string>() ) {

            child->SetType( JsonValueType::String );

        } else if( property.GetTypeInfo() == spp::GetTypeInfo<std::wstring>() ) {

            child->SetType( JsonValueType::String );

        } else {

            child->SetType( JsonValueType::Integer );

        }

    } else {

        child->SetType( JsonValueType::Object );

        bool                hasChildren             = false;
        spp::Serializable*  parentSerializable      = 0;
        PropertyTypeInfo info                      = property.GetTypeInfo();

        if( info == spp::GetTypeInfo< spp::Serializable* >() ||
                info == spp::GetTypeInfo< spp::AutoSerializable* >()  ||
                info == spp::GetTypeInfo<spp::PropertyContainer*>()  ||
                info.IsClass ) {

            void*                   elem         = 0;
            spp::Serializable*      serializable = 0;

            if( property.GetGetter().GetGetter()->operator()( info, &elem ) ) {
                serializable = ( spp::Serializable* )elem;
            }


            if( serializable != 0 ) {
                parentSerializable  = serializable;
                hasChildren         = true;
            }
        }

        if( hasChildren ) {

            auto collection = parentSerializable->GetProperties();

            for( auto it = collection.Begin(); it != collection.End(); ++it ) {

                BaseReflectPropertyCollection( child, *it );

            }

        }

    }

    element->AddChild( child );

    for( auto it = element->SiblingBegin(); it != element->SiblingEnd(); ++it ) {
        ( *it )->AddSiblingRef( child );

        child->AddSiblingRef( *it );
    }
}

libcommon::SharedPtr<JsonElement>        spp::formatters::json::ReflectPropertyCollection( spp::Property& property ) {

    libcommon::SharedPtr<JsonElement> root( new JsonElement() );

    BaseReflectPropertyCollection( root, property );

    return root;

}


void        spp::formatters::json::ReflectPropertyCollection( libcommon::SharedPtr<JsonElement> root, spp::Property& property ) {

    BaseReflectPropertyCollection( root, property );

}

libcommon::SharedPtr<SerializationInfo>      spp::formatters::json::Provider::CreateSerialization( spp::Stream* stream ) {

    SerializationInfo* info = new SerializationInfo( stream, new spp::formatters::json::Formatter() );

    info->GetFormatter()->Reset( stream );

    return libcommon::SharedPtr< SerializationInfo >( info );

}

spp::formatters::json::Formatter::Formatter() : spp::Formatter() {

    this->m_Stream      = 0;

}

bool spp::formatters::json::Formatter::Deserialize( spp::Property property ) {

    std::string         streamData;

    size_t initial      = m_Stream->GetPosition();
    size_t length       = m_Stream->Length() - m_Stream->GetPosition();

    if( length > 0 ) {

        streamData.resize( length + 1, '\0' );

        if( length != m_Stream->Read( ( void* )streamData.c_str(), length * sizeof( char ) ) ) {
            return false;
        }

        m_Stream->Move( length );

        JsonParserState state;

        bool result = json::Parse( state, streamData );

        if( result ) {

            libcommon::SharedPtr< JsonElement >                  root     = state.GetRoot();

            BaseReflectElementTree( root, property );

            return true;
        }

        m_Stream->SetPosition( initial );
        state.Clear();

        return false;

    }

    return false;

}

bool spp::formatters::json::Formatter::Deserialize( spp::PropertyContainer* container ) {

    std::string         streamData;

    size_t initial      = m_Stream->GetPosition();
    size_t length       = m_Stream->Length() - m_Stream->GetPosition();

    if( length > 0 ) {

        streamData.resize( length + 1, '\0' );

        if( length != m_Stream->Read( ( void* )streamData.c_str(), length * sizeof( char ) ) ) {
            return false;
        }

        m_Stream->Move( length );

        JsonParserState state;

        bool result = json::Parse( state, streamData );

        if( result ) {

            libcommon::SharedPtr< JsonElement >                  root     = state.GetRoot();

            BaseReflectElementTree( root, container );

            return true;
        }

        m_Stream->SetPosition( initial );
        state.Clear();

        return false;

    }

    return false;

}

bool spp::formatters::json::Formatter::Deserialize( spp::PropertyCollection collection ) {

    std::string         streamData;

    size_t initial      = m_Stream->GetPosition();
    size_t length       = m_Stream->Length() - m_Stream->GetPosition();

    if( length > 0 ) {

        streamData.resize( length + 1, '\0' );

        if( length != m_Stream->Read( ( void* )streamData.c_str(), length * sizeof( char ) ) ) {
            return false;
        }

        m_Stream->Move( length );

        JsonParserState state;

        bool result = json::Parse( state, streamData );

        if( result ) {

            libcommon::SharedPtr< JsonElement >                  root     = state.GetRoot();

            BaseReflectElementTree( root, collection );

            return true;
        }

        m_Stream->SetPosition( initial );
        state.Clear();

        return false;

    }

    return false;


}

bool spp::formatters::json::Formatter::Serialize( spp::Property property ) {

    libcommon::SharedPtr<JsonElement> element = ReflectPropertyCollection( property );

    ( *( element->ChildBegin() ) )->Write( this->m_Stream, true );

    return true;

}

bool spp::formatters::json::Formatter::Serialize( spp::PropertyCollection collection ) {

    bool                                 result( true );
    libcommon::SharedPtr<JsonElement>         root( new JsonElement() );

    root->SetType( JsonValueType::Object );
    root->SetName( collection.GetName() );

    for( auto it = collection.Begin(); it != collection.End(); ++it ) {

        ReflectPropertyCollection( root, *it );

    }

    for( auto it = root->ChildBegin(); it != root->ChildEnd(); ++it ) {

        for( auto st = root->ChildBegin(); st != root->ChildEnd(); ++st ) {

            if( *it == *st ) {
                continue;
            }

            ( *it )->AddSiblingRef( *st );

        }

    }

    root->Write( this->m_Stream, true );

    return result;

}


void spp::formatters::json::Formatter::Reset( Stream* stream ) {

    this->m_Stream      = stream;

    return;

}
