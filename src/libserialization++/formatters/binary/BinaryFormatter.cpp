#include <cstring>

#include <libserialization++/SerializationStream.hpp>
#include <libserialization++/Property.hpp>
#include <libserialization++/Serializer.hpp>
#include <libserialization++/TypeInfo.hpp>
#include <libserialization++/formatters/binary/BinaryFormatter.hpp>

using namespace spp;
using namespace spp::formatters::binary;

struct      BinaryProperty {
    char        PropertyName[255];
    size_t      PropertyNameLength;
    void*       PropertyData;
    size_t      PropertyDataLength;
    char        PropertyType[255];
    size_t      PropertyTypeLength;

    size_t      ChildCount;
};

spp::PropertyTypeInfo   GetTypeInfoForString( const char* typeBuf ) {
    std::string     type( typeBuf );

    if( type == "char" ) {
        return spp::GetTypeInfo<char>();
    }

    if( type == "unsigned char" ) {
        return spp::GetTypeInfo<unsigned char>();
    }

    if( type == "short" ) {
        return spp::GetTypeInfo<short>();
    }

    if( type == "unsigned short" ) {
        return spp::GetTypeInfo<unsigned short>();
    }

    if( type == "int" ) {
        return spp::GetTypeInfo<int>();
    }

    if( type == "unsigned int" ) {
        return spp::GetTypeInfo<unsigned int>();
    }

    if( type == "long" ) {
        return spp::GetTypeInfo<long>();
    }

    if( type == "unsigned long" ) {
        return spp::GetTypeInfo<unsigned long>();
    }

    if( type == "long long" ) {
        return spp::GetTypeInfo<long long>();
    }

    if( type == "unsigned long long" ) {
        return spp::GetTypeInfo<unsigned long long>();
    }

    if( type == "double" ) {
        return spp::GetTypeInfo<double>();
    }

    if( type == "float" ) {
        return spp::GetTypeInfo<float>();
    }

    if( type == "string" ) {
        return spp::GetTypeInfo<std::string>();
    }

    if( type == "wstring" ) {
        return spp::GetTypeInfo<std::wstring>();
    }

    if( type == "bool" ) {
        return spp::GetTypeInfo<bool>();
    }

    if( type == "object" ) {
        return spp::GetTypeInfo<Serializable*>();
    }

    /// Default.
    return spp::GetTypeInfo<char>();
}


#define STRING_FROM_TYPE_INFO( x ) \
    if ( info == GetTypeInfoForString(#x) ) { \
        return #x; \
    }

const char*             GetStringFromTypeInfo( const spp::PropertyTypeInfo& info ) {

    ( void )info;

    STRING_FROM_TYPE_INFO( char );
    STRING_FROM_TYPE_INFO( unsigned char );
    STRING_FROM_TYPE_INFO( short );
    STRING_FROM_TYPE_INFO( unsigned short );
    STRING_FROM_TYPE_INFO( int );
    STRING_FROM_TYPE_INFO( unsigned int );
    STRING_FROM_TYPE_INFO( long );
    STRING_FROM_TYPE_INFO( unsigned long );
    STRING_FROM_TYPE_INFO( long long );
    STRING_FROM_TYPE_INFO( unsigned long long );
    STRING_FROM_TYPE_INFO( float );
    STRING_FROM_TYPE_INFO( double );
    STRING_FROM_TYPE_INFO( bool );
    STRING_FROM_TYPE_INFO( object );
    STRING_FROM_TYPE_INFO( string );
    STRING_FROM_TYPE_INFO( wstring );

    return "";

}


std::shared_ptr<SerializationInfo>      spp::formatters::binary::Provider::CreateSerialization( spp::Stream* stream ) {

    SerializationInfo* info = new SerializationInfo( stream, new spp::formatters::binary::Formatter() );

    info->GetFormatter()->Reset( stream );

    return std::shared_ptr< SerializationInfo >( info );

}

spp::formatters::binary::Formatter::Formatter() : spp::Formatter() {
    this->m_Stream      = 0;
}

bool spp::formatters::binary::Formatter::Deserialize( spp::Property property ) {

    if( this->m_Stream == 0 ) {
        return false;
    }

    size_t             total( m_Stream->GetPosition() );
    BinaryProperty     blob;
    void*              buffer( 0 );

    size_t bytesRead = m_Stream->Read( ( void* )&blob, sizeof( char ) * sizeof( BinaryProperty ) );
    m_Stream->Move( sizeof( char ) * sizeof( BinaryProperty ) );

    if( bytesRead != sizeof( blob ) ) {
        return false;
    }

    size_t length   = blob.PropertyDataLength;

    if( length > 0 ) {
        buffer      = ( void* )new char[ length ];

        m_Stream->Read( ( void* )buffer, sizeof( char ) * length );
        m_Stream->Move( sizeof( char ) * length );

    }

    std::string         typeName( blob.PropertyType, blob.PropertyTypeLength );
    std::string         name( blob.PropertyName, blob.PropertyNameLength );

    if( name != property.GetDesc().GetName() ) {
        m_Stream->SetPosition( total );

        delete []( char* )buffer;

        return false;
    }

    if( buffer != 0 ) {

        if( typeName == "string" ) {

            std::string strBuffer( ( char* )buffer, blob.PropertyDataLength );
            property.GetSetter().operator()( property.GetTypeInfo(), ( void* )&strBuffer );

        } else if( typeName == "wstring" ) {

            std::wstring strBuffer( ( wchar_t* )buffer, blob.PropertyDataLength );
            property.GetSetter().operator()( property.GetTypeInfo(), ( void* )&strBuffer );

        } else {

            property.GetSetter().operator()( property.GetTypeInfo(), buffer );

        }

        delete []( char* ) buffer;

    }

    if( blob.ChildCount > 0 ) {

        bool                hasChildren             = false;
        spp::Serializable*  parentSerializable      = 0;
        PropertyTypeInfo info                      = property.GetTypeInfo();

        if( info == spp::GetTypeInfo< spp::Serializable* >() ||
                info == spp::GetTypeInfo< spp::AutoSerializable* >() ) {

            spp::Serializable*      serializable = property.GetGetter().Get< spp::Serializable* >();

            if( serializable != 0 ) {
                parentSerializable  = serializable;
                hasChildren         = true;
            }
        }

        if( parentSerializable != 0 ) {

            auto collection = parentSerializable->GetProperties();

            return this->Deserialize( collection );

        } else {

            return false;

        }

    }


    return true;

}

bool spp::formatters::binary::Formatter::DeserializeContainerElement( spp::PropertyContainer* container ) {

    if( this->m_Stream == 0 ) {
        return false;
    }

    size_t             total( m_Stream->GetPosition() );
    BinaryProperty     blob;
    void*              buffer( 0 );

    size_t bytesRead = m_Stream->Read( ( void* )&blob, sizeof( char ) * sizeof( BinaryProperty ) );
    m_Stream->Move( sizeof( char ) * sizeof( BinaryProperty ) );

    if( bytesRead != sizeof( blob ) ) {
        return false;
    }

    size_t length   = blob.PropertyDataLength;

    if( length > 0 ) {
        buffer      = ( void* )new char[ length ];

        m_Stream->Read( ( void* )buffer, sizeof( char ) * length );
        m_Stream->Move( sizeof( char ) * length );

    }

    std::string         typeName( blob.PropertyType, blob.PropertyTypeLength );
    std::string         name( blob.PropertyName, blob.PropertyNameLength );

    if( buffer != 0 && length > 0 ) {

        if( container->GetElementTypeInfo() == GetTypeInfoForString( typeName.c_str() ) ) {

            container->AddProperty( name, container->GetElementTypeInfo(), buffer );

        }

        delete []( char* ) buffer;

    }

    if( blob.ChildCount > 0 ) {

        bool                hasChildren             = false;
        spp::Serializable*  parentSerializable      = 0;

        if( container->IsLastPropertySerializable() ) {

            spp::Serializable*      serializable = ( spp::Serializable* )container->CreateInstance();

            if( serializable != 0 ) {
                parentSerializable  = serializable;
                hasChildren         = true;
            }
        }

        if( parentSerializable != 0 ) {

            auto collection = parentSerializable->GetProperties();

            return this->Deserialize( collection );

        } else {

            return false;

        }

    }


    return true;

}


bool spp::formatters::binary::Formatter::Deserialize( spp::PropertyContainer* container ) {

    if( this->m_Stream == 0 ) {
        return false;
    }

    size_t             initial( m_Stream->GetPosition() );
    size_t             total( m_Stream->GetPosition() );
    BinaryProperty     blob;
    void*              buffer( 0 );

    while( true ) {

        total            = m_Stream->GetPosition();
        size_t bytesRead = m_Stream->Read( ( void* )&blob, sizeof( char ) * sizeof( BinaryProperty ) );

        if( !m_Stream->Move( sizeof( char ) * sizeof( BinaryProperty ) ) ) {
            break;
        }

        if( bytesRead != sizeof( BinaryProperty ) ) {
            return false;
        }

        size_t length   = blob.PropertyDataLength;

        if( length > 0 ) {
            buffer      = ( void* )new char[ length ];

            m_Stream->Read( ( void* )buffer, sizeof( char ) * length );

            if( !m_Stream->Move( sizeof( char ) * length ) ) {
                break;
            }

        }

        bool serialized( true );

        m_Stream->SetPosition( total );
        serialized = DeserializeContainerElement( container );


        if( !serialized ) {
            m_Stream->SetPosition( initial );

            break;
        }

    }

    return false;
}

bool spp::formatters::binary::Formatter::Deserialize( spp::PropertyCollection collection ) {

    if( this->m_Stream == 0 ) {
        return false;
    }

    size_t             initial( m_Stream->GetPosition() );
    size_t             total( m_Stream->GetPosition() );
    BinaryProperty     blob;
    void*              buffer( 0 );

    while( true ) {

        total            = m_Stream->GetPosition();
        size_t bytesRead = m_Stream->Read( ( void* )&blob, sizeof( char ) * sizeof( BinaryProperty ) );

        if( !m_Stream->Move( sizeof( char ) * sizeof( BinaryProperty ) ) ) {
            break;
        }

        if( bytesRead != sizeof( BinaryProperty ) ) {
            return false;
        }

        size_t length   = blob.PropertyDataLength;

        if( length > 0 ) {
            buffer      = ( void* )new char[ length ];

            m_Stream->Read( ( void* )buffer, sizeof( char ) * length );

            if( !m_Stream->Move( sizeof( char ) * length ) ) {
                break;
            }

        }

        std::string         name( blob.PropertyName, blob.PropertyNameLength );

        bool serialized( false );

        for( auto it = collection.Begin(); it != collection.End(); ++it ) {

            if( ( *it ).GetDesc().GetName() == name ) {

                m_Stream->SetPosition( total );

                serialized = Deserialize( *it );

                break;

            }

        }

        if( !serialized ) {
            m_Stream->SetPosition( initial );

            break;
        }

    }

    return false;
}

bool spp::formatters::binary::Formatter::Serialize( spp::Property property ) {
    if( this->m_Stream == 0 ) {
        return false;
    }

    bool                hasChildren             = false;
    spp::Serializable*  parentSerializable      = 0;
    PropertyTypeInfo info                      = property.GetTypeInfo();

    if( info == spp::GetTypeInfo< spp::Serializable* >() ||
            info == spp::GetTypeInfo< spp::AutoSerializable* >() ) {

        spp::Serializable*      serializable = property.GetGetter().Get< spp::Serializable* >();

        if( serializable != 0 ) {
            parentSerializable  = serializable;
            hasChildren         = true;
        }
    }


    BinaryProperty          blob;
    BinaryProperty*         finalBlob = 0;

    blob.ChildCount                 = 0;
    blob.PropertyData               = 0;
    blob.PropertyDataLength         = 0;

    /// Property Name and Type
    memset( ( void* )blob.PropertyName, 0, 255 );
    memcpy( ( void* )blob.PropertyName, ( const void* )property.GetDesc().GetName().c_str(), property.GetDesc().GetName().size() );

    blob.PropertyNameLength         = property.GetDesc().GetName().size();
    const char* typeName            = ::GetStringFromTypeInfo( property.GetTypeInfo() );

    if( typeName == 0 ) {
        return false;
    }

    blob.PropertyTypeLength = strlen( typeName );
    memset( ( void* )blob.PropertyType, 0, 255 );
    memcpy( ( void* )blob.PropertyType, ( const void* )typeName, strlen( typeName ) );


    /// Fill the value; First check, if the value has to be deduced. Strings or WStrings can't be written
    /// to a stream in their pure memory structure.
    if( info == spp::GetTypeInfo< std::string >() ) {

        std::string     val     = property.GetGetter().Get<std::string>();
        size_t          size    = val.size() + 1;

        blob.PropertyDataLength = size + 1;

        finalBlob               = ( BinaryProperty* )( new char[ sizeof( BinaryProperty ) + size ] );
        *finalBlob              = blob;

        memset( ( void* )finalBlob, 0, size + sizeof( BinaryProperty ) );
        memcpy( ( void* )( ( char* )finalBlob + sizeof( BinaryProperty ) ), ( const void* )val.c_str(), val.size() );

    } else if( info == spp::GetTypeInfo< std::wstring >() ) {

        std::wstring     val    = property.GetGetter().Get<std::wstring>();
        size_t          size    = ( val.size() ) * sizeof( wchar_t ) ;

        blob.PropertyDataLength = size + 1;

        finalBlob               = ( BinaryProperty* )( new char[ sizeof( BinaryProperty ) + size ] );
        *finalBlob              = blob;

        memset( ( void* )finalBlob, 0, size + sizeof( BinaryProperty ) );
        memcpy( ( void* )( ( char* )finalBlob + sizeof( BinaryProperty ) ), ( const void* )val.c_str(), val.size() * sizeof( wchar_t ) );

    } else if( info == spp::GetTypeInfo< spp::Serializable* >() ||
               info == spp::GetTypeInfo< spp::AutoSerializable* >() ) {

        blob.PropertyData       = 0;
        blob.PropertyDataLength = 0;

        finalBlob               = ( BinaryProperty* )( new char[ sizeof( BinaryProperty ) ] );
        memset( ( void* )finalBlob, 0, sizeof( BinaryProperty ) );

        *finalBlob              = blob;
    } else {

        size_t          length          = info.TypeSize;
        char*           buffer          = new char[ length ];

        property.GetGetter().operator()( info, ( void* )buffer );

        blob.PropertyDataLength = length;

        finalBlob               = ( BinaryProperty* )( new char[ sizeof( BinaryProperty ) + length ] );
        memset( ( void* )finalBlob, 0, sizeof( BinaryProperty ) + length );

        *finalBlob              = blob;

        memcpy( ( void* )( ( char* )finalBlob + sizeof( BinaryProperty ) ), ( const void* )buffer, length );

        delete [] buffer;
    }

    /// Now count the number of children.
    if( hasChildren ) {

        finalBlob->ChildCount       = parentSerializable->GetProperties().Size();

    }

    /// Now write current property.
    size_t written = m_Stream->Write( ( void* )finalBlob, sizeof( BinaryProperty ) + blob.PropertyDataLength );

    m_Stream->Move( sizeof( BinaryProperty ) + blob.PropertyDataLength );

    /// Now write all children or return.
    if( written == sizeof( BinaryProperty ) + blob.PropertyDataLength ) {

        if( hasChildren ) {

            bool result     = true;
            auto collection = parentSerializable->GetProperties();

            for( auto it = collection.Begin(); it != collection.End(); ++it ) {
                result &= Serialize( *it );
            }

            return result;

        }

        return true;

    }

    return false;
}

bool spp::formatters::binary::Formatter::Serialize( spp::PropertyCollection collection ) {
    if( this->m_Stream == 0 ) {
        return false;
    }

    bool    result( true );

    for( auto it = collection.Begin(); it != collection.End(); ++it ) {
        spp::Property&      property = *it;

        result &= Serialize( property );

        if( !result ) {
            break;
        }
    }

    return result;
}

void spp::formatters::binary::Formatter::Reset( Stream* stream ) {

    m_Stream = stream;

    return;
}
