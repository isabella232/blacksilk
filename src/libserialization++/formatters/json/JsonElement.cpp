
#include <libserialization++/PropertyEncoder.hpp>
#include <libserialization++/PropertyDecoder.hpp>
#include <libserialization++/formatters/json/JsonElement.hpp>
#include <libserialization++/formatters/json/JsonParser.hpp>

using namespace spp;
using namespace spp::formatters::json;

PrinterFormattingOptions::PrinterFormattingOptions() {

    AfterNewLineScopeBegin          = false;
    AfterNewLineScopeEnd            = false;
    BeforeNewLineScopeBegin         = false;
    BeforeNewLineScopeEnd           = false;
    AfterNewLineCommata             = false;
    TokenCommataSpaceCount          = 0;
    TokenScopeModifierSpaceCount    = 0;
    TokenSpaceCount                 = 0;

}

PrinterFormattingOptions PrinterFormattingOptions::Lean() {

    PrinterFormattingOptions    options;

    options.AfterNewLineScopeBegin          = false;
    options.AfterNewLineScopeEnd            = false;
    options.BeforeNewLineScopeBegin         = false;
    options.BeforeNewLineScopeEnd           = false;
    options.AfterNewLineCommata             = false;
    options.TokenCommataSpaceCount          = 0;
    options.TokenScopeModifierSpaceCount    = 0;
    options.TokenSpaceCount                 = 0;

    return options;

}

PrinterFormattingOptions PrinterFormattingOptions::Pretty() {

    PrinterFormattingOptions    options;

    options.AfterNewLineScopeBegin          = true;
    options.AfterNewLineScopeEnd            = true;
    options.BeforeNewLineScopeBegin         = false;
    options.BeforeNewLineScopeEnd           = true;
    options.AfterNewLineCommata             = true;
    options.TokenCommataSpaceCount          = 0;
    options.TokenScopeModifierSpaceCount    = 4;
    options.TokenSpaceCount                 = 2;

    return options;

}

JsonElement::JsonElement() {

    this->m_Type        = JsonValueType::Unknown;
    this->m_ValueType   = JsonValueType::Unknown;

}

JsonElement::JsonElement( Stream* stream ) {

    this->m_Type        = JsonValueType::Unknown;
    this->m_ValueType   = JsonValueType::Unknown;

    this->Read( stream );

}

JsonElement::~JsonElement() {

    this->m_Children.clear();
    this->m_Siblings.clear();
    this->m_Value.clear();
    this->m_Name.clear();

}

std::vector< libcommon::SharedPtr< JsonElement > >::iterator JsonElement::ChildBegin() {

    return this->m_Children.begin();

}

std::vector< libcommon::SharedPtr< JsonElement > >::iterator JsonElement::ChildEnd() {

    return this->m_Children.end();

}

std::vector< libcommon::SharedPtr< JsonElement > >::iterator JsonElement::SiblingBegin() {

    return this->m_Siblings.begin();

}

std::vector< libcommon::SharedPtr< JsonElement > >::iterator JsonElement::SiblingEnd() {

    return this->m_Siblings.end();

}

void            JsonElement::SetValue( const std::string& value ) {

    SetValue( std::wstring( value.begin(), value.end() ) );

}

void            JsonElement::SetValue( const std::wstring& value ) {

    m_Value = value;

}

void            JsonElement::SetName( const std::string& name ) {

    SetName( std::wstring( name.begin(), name.end() ) );

}

void            JsonElement::SetName( const std::wstring& name ) {

    this->m_Name        = name;

}

std::wstring    JsonElement::GetName() const {

    return m_Name;

}

std::string     JsonElement::GetAnsiName() const {

    return std::string( m_Name.begin(), m_Name.end() );

}

std::wstring    JsonElement::GetValue() const {

    return m_Value;

}

std::string     JsonElement::GetAnsiValue() const {

    return std::string( m_Value.begin(), m_Value.end() );

}

bool            JsonElement::Read( spp::Stream*  stream ) {

    std::string         streamData;

    size_t initial      = stream->GetPosition();
    size_t length       = stream->Length() - stream->GetPosition();

    if( length > 0 ) {

        streamData.resize( length + 1, '\0' );

        if( length != stream->Read( ( void* )streamData.c_str(), length * sizeof( char ) ) ) {
            return false;
        }

        stream->Move( length );

        JsonParserState state;

        bool result = json::Parse( state, streamData );

        if( result ) {
            libcommon::SharedPtr< JsonElement >                  root     = state.GetRoot();

            if( root.get() == 0 ) {
                *this = *root.get();

                return true;
            }

        }

        stream->SetPosition( initial );
        state.Clear();

        return false;

    }

    return false;


}

bool            JsonElement::Write( spp::Stream* stream, bool prettyPrint, size_t tabs ) {
    bool result( false );

    spp::formatters::json::PrinterFormattingOptions options =
        prettyPrint ? PrinterFormattingOptions::Pretty() : PrinterFormattingOptions::Lean();

    if( !stream ) {
        return false;
    }

    std::string     value;

    value += std::string( tabs, '\t' );


    switch( this->m_Type ) {
        case json::JsonValueType::String:
        case json::JsonValueType::Integer:
        case json::JsonValueType::Float:

            if( GetName().size() > 0 ) {
                value += "\"";
                value += GetAnsiName();
                value += "\"";

                value += std::string( options.TokenSpaceCount, ' ' );
                value += ":";
                value += std::string( options.TokenSpaceCount, ' ' );
            }

            if( this->m_Type == json::JsonValueType::String ) {

                value += "\"";
                value += GetAnsiValue();
                value += "\"";

            } else {

                value += GetAnsiValue();

            }

            stream->Write( ( void* )value.c_str(), value.size() * sizeof( char ) );
            stream->Move( value.size() * sizeof( char ) );

            result = true;

            break;

        case json::JsonValueType::Object:
        case json::JsonValueType::Array:

            if( GetName().size() > 0 ) {
                value += "\"";
                value += GetAnsiName();
                value += "\"";

                value += std::string( options.TokenSpaceCount, ' ' );
                value += ":";
                value += std::string( options.TokenSpaceCount, ' ' );
            }

            if( options.BeforeNewLineScopeBegin ) {
                value += "\n";
            }

            if( this->m_Type == json::JsonValueType::Object ) {
                value += "{";
            } else {
                value += "[";
            }

            if( options.AfterNewLineScopeBegin ) {
                value += "\n";
            }

            stream->Write( ( void* )value.c_str(), value.size() * sizeof( char ) );
            stream->Move( value.size() * sizeof( char ) );

            for( auto it = this->ChildBegin(); it != this->ChildEnd(); ++it ) {
                ( *it )->Write(
                    stream,
                    prettyPrint,
                    prettyPrint ? tabs + 1 : tabs
                );

                value.clear();

                if( it != ( this->ChildBegin() + ( this->CountChildren() - 1 ) ) ) {
                    value += std::string( options.TokenCommataSpaceCount, ' ' );
                    value += ",";
                }

                if( options.AfterNewLineCommata ) {
                    value += "\n";
                }

                stream->Write( ( void* )value.c_str(), value.size() * sizeof( char ) );
                stream->Move( value.size() * sizeof( char ) );
            }


            value.clear();
            value += std::string( tabs, '\t' );

            if( this->m_Type == json::JsonValueType::Object ) {
                value += "}";
            } else {
                value += "]";
            }

            stream->Write( ( void* )value.c_str(), value.size() * sizeof( char ) );
            stream->Move( value.size() * sizeof( char ) );

            result = true;

            break;

        default:
            result = false;
    }

    return result;

}

void            JsonElement::AddSiblingRef( const libcommon::SharedPtr< JsonElement >& sibling ) {

    this->m_Siblings.push_back( sibling );

}

void            JsonElement::AddSiblingRef( JsonElement* sibling ) {

    this->m_Siblings.push_back( libcommon::SharedPtr< JsonElement >( sibling ) );

}

void            JsonElement::ClearSiblings() {

    this->m_Siblings.clear();

}

bool            JsonElement::RemoveSibling( const libcommon::SharedPtr< JsonElement >& sibling ) {

    return RemoveSibling(
               sibling.get()
           );

}

bool            JsonElement::RemoveSibling( JsonElement* sibling ) {

    bool result( false );

    for( auto it = this->m_Siblings.begin(); it != this->m_Siblings.end(); ++it ) {

        if( ( *it ).get() == sibling ) {

            m_Siblings.erase( it );

            result = true;

            break;

        }

    }

    return result;

}

size_t          JsonElement::CountSiblings() const {

    return this->m_Siblings.size();

}

void            JsonElement::AddChild( const libcommon::SharedPtr< JsonElement >& child ) {

    this->m_Children.push_back( child );

}

void            JsonElement::AddChild( JsonElement* child ) {

    this->m_Children.push_back( libcommon::SharedPtr< JsonElement >( child ) );

}

void            JsonElement::ClearChildren() {

    this->m_Children.clear();

}

bool            JsonElement::RemoveChild( const libcommon::SharedPtr< JsonElement >& sibling ) {

    return RemoveChild(
               sibling.get()
           );

}

bool            JsonElement::RemoveChild( JsonElement* sibling ) {

    bool result( false );

    for( auto it = this->m_Children.begin(); it != this->m_Children.end(); ++it ) {

        if( ( *it ).get() == sibling ) {

            m_Children.erase( it );

            result = true;

            break;

        }

    }

    return result;

}

size_t          JsonElement::CountChildren() const {

    return ( this->m_Children.size() );

}

void            JsonElement::Reset() {

    this->m_Children.clear();
    this->m_Siblings.clear();
    this->m_Value.clear();
    this->m_Name.clear();

}

JsonValueType::t    JsonElement::GetValueType() const {

    return this->m_ValueType;

}

void                JsonElement::SetValueType( JsonValueType::t val ) {

    this->m_ValueType  = val;

}

JsonValueType::t    JsonElement::GetType() const {

    return this->m_Type;

}

void                JsonElement::SetType( JsonValueType::t val ) {

    this->m_Type  = val;

}

