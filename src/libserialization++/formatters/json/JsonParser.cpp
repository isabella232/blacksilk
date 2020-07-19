
#include <libserialization++/formatters/json/JsonParser.hpp>
#include <libserialization++/formatters/json/JsonElement.hpp>

using namespace spp;
using namespace spp::formatters;
using namespace spp::formatters::json;


JsonParserState::JsonParserState() {

    this->m_Depth           = 0;
    this->m_Line            = 0;
    this->m_Position        = 0;
    this->m_Good            = false;

}

JsonParserState::JsonParserState( const std::string& data ) {

    this->m_Depth           = 0;
    this->m_Line            = 0;
    this->m_Position        = 0;
    this->m_Good            = false;

    Setup( data );

}

void    JsonParserState::Setup( const std::string& data ) {

    this->m_Depth           = 0;
    this->m_Line            = 0;
    this->m_Position        = 0;
    this->m_Good            = true;

    this->m_Data            = data;
    this->m_Begin           = m_Data.begin();
    this->m_End             = m_Data.end();
    this->m_Current         = m_Data.begin();

}

size_t  JsonParserState::GetLineCounter() const {

    return this->m_Line;

}

size_t  JsonParserState::GetScopeDepth() const {

    return this->m_Depth;

}

size_t  JsonParserState::GetPosition() const {

    return this->m_Position;

}

Iterator    JsonParserState::Begin() const {

    return this->m_Begin;

}

Iterator    JsonParserState::End() const {

    return this->m_End;

}

bool    JsonParserState::Current( Iterator& it ) {

    if( m_Good ) {

        it = m_Current;

        return true;

    }

    return false;

}

bool    JsonParserState::Next( Iterator& it ) {

    if( m_Good ) {

        if( m_Current + 1 == m_End ) {
            m_Good = false;

            return false;
        }

        m_Current++;
        m_Position++;

        it = m_Current;

        if( *m_Current == '\n' ) {
            m_Line++;
        }

        return true;
    }

    return false;

}

bool    JsonParserState::Previous( Iterator& it ) {

    if( m_Good ) {

        if( m_Current == m_Begin ) {
            m_Good = false;

            return false;
        }

        m_Current--;
        m_Position--;

        it = m_Current;

        if( *m_Current == '\n' ) {
            m_Line--;
        }

        return true;
    }

    return false;

}

void    JsonParserState::Register( const libcommon::SharedPtr< JsonElement >& elem ) {

    for( auto it = m_Elements.begin(); it != m_Elements.end(); ++it ) {
        if( elem == *it ) {
            return;
        }
    }

    m_LastElement = elem;

    this->m_Elements.push_back( elem );

    if( this->m_ScopeStack.size() > 0 ) {
        this->m_ScopeStack.back()->AddChild( elem );
    }

}

void    JsonParserState::RegisterErrorType( const JsonParserErrorType& type ) {

    this->m_ErrorTypes.push_back( type );

}

void    JsonParserState::RegisterErrorType( size_t id, const std::string& name ) {

    JsonParserErrorType type;

    type.ID     = id;
    type.Name   = name;

    RegisterErrorType( type );

}

void    JsonParserState::Error( size_t id, const std::string& message ) {

    JsonParserError error;

    error.Line      = this->m_Line;
    error.Message   = message;
    error.Type      = id;

    this->m_Errors.push_back( error );

    m_Good = false;

}

std::vector< JsonParserError >  JsonParserState::GetErrors() const {

    return this->m_Errors;

}

std::vector< JsonParserErrorType > JsonParserState::GetErrorTypes() const {

    return this->m_ErrorTypes;

}

void        JsonParserState::EnterScope( const libcommon::SharedPtr< JsonElement >& elem ) {

    this->m_ScopeStack.push_back( elem );

    if( m_ScopeStack.size() == 1 ) {
        m_Root = elem;
    }

}

void        JsonParserState::LeaveScope() {

    m_ScopeStack.erase( m_ScopeStack.begin() + ( m_ScopeStack.size() - 1 ) );

}

void        JsonParserState::PushState() {

    JsonParserStateFrame    frame;

    frame.Depth     = this->m_Depth;
    frame.Line      = this->m_Line;
    frame.Position  = this->m_Position;

    this->m_StateStack.push_back( frame );

}

void        JsonParserState::PopState() {

    if( m_StateStack.size() > 0 ) {

        JsonParserStateFrame frame = m_StateStack.back();

        this->m_Current         = this->m_Begin + frame.Position;
        this->m_Position        = frame.Position;
        this->m_Line            = frame.Line;
        this->m_Depth           = frame.Depth;

        m_StateStack.pop_back();

    }

}

void        JsonParserState::Clear() {

    this->m_Elements.clear();
    this->m_ErrorTypes.clear();
    this->m_Errors.clear();
    this->m_ScopeStack.clear();
    this->m_StateStack.clear();
    this->m_Data.clear();

    m_LastElement.reset();
    m_Root.reset();

    this->m_Current     = this->m_End;
    this->m_Begin       = this->m_End;

    this->m_Depth       = 0;
    this->m_Line        = 0;
    this->m_Position    = 0;

    this->m_Good        = false;

}

std::string JsonParserState::GetData() const {

    return this->m_Data;

}

bool    JsonParserState::Good() const {

    return this->m_Good;

}

libcommon::SharedPtr< JsonElement >      JsonParserState::GetLast() const {

    return this->m_LastElement;

}

libcommon::SharedPtr< JsonElement >      JsonParserState::GetRoot() const {

    return this->m_Root;

}

libcommon::SharedPtr< JsonElement >      JsonParserState::GetScope() const {

    if( this->m_ScopeStack.size() > 0 ) {

        return this->m_ScopeStack.back();

    }

    return libcommon::SharedPtr< JsonElement >();
}

std::vector< libcommon::SharedPtr< JsonElement > >      JsonParserState::GetElements() const {

    return this->m_Elements;

}

bool            spp::formatters::json::Expect( JsonParserState& state, char val ) {

    bool result( false );

    state.PushState();

    Iterator it;

    if( state.Next( it ) ) {
        result = ( *it == val );
    }

    state.PopState();

    return result;

}

bool            spp::formatters::json::SkipChars( JsonParserState& state, Iterator& it ) {

    bool result( false );

    Iterator current = it;

    if( state.Current( current ) ) {
        while( ( *current == ' ' || *current == '\n' || *current == '\t' ) ) {
            if( !state.Next( current ) ) {
                break;
            }
        }

        result = true;
    }

    if( result ) {
        it = current;
    }

    return result;

}

bool            spp::formatters::json::SkipCharsUntil( JsonParserState& state, Iterator& it, char val ) {

    bool result( false );

    Iterator current = it;

    if( state.Current( current ) ) {
        while( ( *current != val ) ) {
            if( !state.Next( current ) ) {
                break;
            }
        }

        result = true;
    }

    if( result ) {
        it = current;
    }

    return result;

}

bool            spp::formatters::json::SkipCharsWhile( JsonParserState& state, Iterator& it, char val ) {

    bool result( false );

    Iterator current = it;

    if( state.Current( current ) ) {
        while( ( *current == val ) ) {
            if( !state.Next( current ) ) {
                break;
            }
        }

        result = true;
    }

    if( result ) {
        it = current;
    }

    return result;

}


bool JsonStringParserOperator::operator()( JsonParserState& state ) {

    Iterator        current;

    if( state.Current( current ) ) {

        if( !json::SkipChars( state, current ) ) {
            state.Error( JSON_ERROR_SYNTAX, "Failed to parse empty invalid document." );

            return false;
        }

        if( *current == '"' && state.Next( current ) ) {

            Iterator            beginContents = current;
            Iterator            endContents   = current + 1;

            if( !json::SkipCharsUntil( state, endContents, '"' ) ) {
                state.Error( JSON_ERROR_SYNTAX, "Missing '\"' to close json string." );

                return false;
            }

            if( state.GetLast().get() != 0 ) {

                if( state.GetLast()->GetType() == JsonValueType::TemporaryElement ) {

                    state.GetLast()->SetType( JsonValueType::Element );
                    state.GetLast()->SetValueType( JsonValueType::String );
                    state.GetLast()->SetValue( std::string( beginContents, endContents ) );

                    return true;

                }

            }

            libcommon::SharedPtr< JsonElement >  element( new JsonElement() );

            element->SetType( JsonValueType::String );
            element->SetValue( std::string( beginContents, endContents ) );

            state.Register( element );

            return true;

        }

        return false;

    }

    return false;

}

bool JsonElementSeperatorParserOperator::operator()( JsonParserState& state ) {

    Iterator        current;

    if( state.Current( current ) ) {

        if( !json::SkipChars( state, current ) ) {
            state.Error( JSON_ERROR_SYNTAX, "Failed to parse empty invalid document." );

            return false;
        }

        if( *current == ':' ) {

            if( state.GetLast().get() == 0 ) {
                state.Error( JSON_ERROR_SYNTAX, "':' not allowed to be the initial element." );

                return false;
            }

            if( state.GetScope().get() != 0 ) {

                if( ( state.GetScope()->GetType() == JsonValueType::TemporaryArray ) ) {
                    state.Error( JSON_ERROR_SEMANTIC, "An array can't contain named elements." );

                    return false;
                }
            }

            libcommon::SharedPtr< JsonElement > last = state.GetLast();

            last->SetValueType( JsonValueType::Unknown );
            last->SetType( JsonValueType::TemporaryElement );
            last->SetName( last->GetValue() );
            last->SetValue( "" );

            return true;
        }

        return false;

    }

    return false;

}

bool JsonValueSeperatorParserOperator::operator()( JsonParserState& state ) {

    Iterator        current;

    if( state.Current( current ) ) {

        if( !json::SkipChars( state, current ) ) {
            state.Error( JSON_ERROR_SYNTAX, "Failed to parse empty invalid document." );

            return false;
        }

        if( *current == ',' ) {

            if( state.GetLast().get() == 0 ) {
                state.Error( JSON_ERROR_SYNTAX, "',' not allowed to be the initial element." );

                return false;
            }

            libcommon::SharedPtr< JsonElement > last = state.GetLast();

            if( last->GetType() == JsonValueType::TemporaryElement ) {
                state.Error( JSON_ERROR_SEMANTIC, "Invalid ','. Each named element needs a value!" );

                return false;
            }

            return true;
        }

        return false;

    }

    return false;

}

bool JsonArrayBeginParserOperator::operator()( JsonParserState& state ) {

    Iterator        current;

    if( state.Current( current ) ) {

        if( !json::SkipChars( state, current ) ) {
            state.Error( JSON_ERROR_SYNTAX, "Failed to parse empty invalid document." );

            return false;
        }

        if( *current == '[' ) {

            libcommon::SharedPtr< JsonElement > last = state.GetLast();

            if( last.get() != 0 && ( last->GetType() == JsonValueType::TemporaryElement ) ) {

                last->SetType( JsonValueType::TemporaryArray );
                last->SetValue( "" );

                state.EnterScope( last );

            } else {

                libcommon::SharedPtr< JsonElement > elem( new JsonElement() );

                elem->SetType( JsonValueType::TemporaryArray );
                elem->SetValueType( JsonValueType::Unknown );

                state.Register( elem );
                state.EnterScope( elem );

            }

            return true;

        }

        return false;

    }

    return false;

}

bool JsonArrayEndParserOperator::operator()( JsonParserState& state ) {

    Iterator        current;

    if( state.Current( current ) ) {

        if( !json::SkipChars( state, current ) ) {
            state.Error( JSON_ERROR_SYNTAX, "Failed to parse empty invalid document." );

            return false;
        }

        if( *current == ']' ) {

            libcommon::SharedPtr< JsonElement > scope = state.GetScope();

            if( scope.get() == 0 ) {
                state.Error( JSON_ERROR_SEMANTIC, "']' needs an '['" );

                return false;
            }

            if( scope->GetType() != JsonValueType::TemporaryArray ) {
                state.Error( JSON_ERROR_SEMANTIC, "']' needs an '['" );

                return false;
            }

            scope->SetType( JsonValueType::Array );

            state.LeaveScope();

            return true;

        }

        return false;

    }

    return false;


}

bool JsonObjectBeginParserOperator::operator()( JsonParserState& state ) {

    Iterator        current;

    if( state.Current( current ) ) {

        if( !json::SkipChars( state, current ) ) {
            state.Error( JSON_ERROR_SYNTAX, "Failed to parse empty invalid document." );

            return false;
        }

        if( *current == '{' ) {

            libcommon::SharedPtr< JsonElement > last = state.GetLast();

            if( last.get() != 0 && ( last->GetType() == JsonValueType::TemporaryElement ) ) {

                last->SetType( JsonValueType::TemporaryObject );
                last->SetValue( "" );

                state.EnterScope( last );

            } else {

                libcommon::SharedPtr< JsonElement > elem( new JsonElement() );

                elem->SetType( JsonValueType::TemporaryObject );
                elem->SetValueType( JsonValueType::Unknown );

                state.Register( elem );
                state.EnterScope( elem );

            }

            return true;

        }

        return false;

    }

    return false;


}

bool JsonObjectEndParserOperator::operator()( JsonParserState& state ) {

    Iterator        current;

    if( state.Current( current ) ) {

        if( !json::SkipChars( state, current ) ) {
            state.Error( JSON_ERROR_SYNTAX, "Failed to parse empty invalid document." );

            return false;
        }

        if( *current == '}' ) {

            libcommon::SharedPtr< JsonElement > scope = state.GetScope();

            if( scope.get() == 0 ) {
                state.Error( JSON_ERROR_SEMANTIC, "'}' needs an '{'" );

                return false;
            }

            if( scope->GetType() != JsonValueType::TemporaryObject ) {
                state.Error( JSON_ERROR_SEMANTIC, "'}' needs an '{'" );

                return false;
            }

            scope->SetType( JsonValueType::Object );

            state.LeaveScope();

            return true;

        }

        return false;

    }

    return false;


}

bool   IsNumberChar( char a ) {
    std::string numberChars( "-0123456789" );

    return ( numberChars.find( a ) != std::string::npos ) ;
}

bool            SkipNumber( JsonParserState& state, Iterator& it ) {

    bool result( false );

    Iterator current = it;

    if( state.Current( current ) ) {
        while( ( IsNumberChar( *current ) ) ) {
            if( !state.Next( current ) ) {
                break;
            }
        }

        result = true;
    }

    if( result ) {
        it = current;
    }

    return result;

}

bool JsonIntegerParserOperator::operator()( JsonParserState& state ) {

    Iterator        current;

    if( state.Current( current ) ) {

        if( !json::SkipChars( state, current ) ) {
            state.Error( JSON_ERROR_SYNTAX, "Failed to parse empty invalid document." );

            return false;
        }

        if( IsNumberChar( *current ) ) {

            Iterator            beginContents = current;
            Iterator            endContents   = current + 1;

            if( !SkipNumber( state, endContents ) ) {
                state.Error( JSON_ERROR_SYNTAX, "Invalid json document." );

                return false;
            }

            /// ToDo: Check if the integer is valid.

            if( state.GetLast().get() != 0 ) {

                if( state.GetLast()->GetType() == JsonValueType::TemporaryElement ) {

                    state.GetLast()->SetType( JsonValueType::Element );
                    state.GetLast()->SetValueType( JsonValueType::Integer );
                    state.GetLast()->SetValue( std::string( beginContents, endContents ) );

                    return true;

                }

            }

            libcommon::SharedPtr< JsonElement >  element( new JsonElement() );

            element->SetType( JsonValueType::Integer );
            element->SetValue( std::string( beginContents, endContents ) );

            state.Register( element );

            return true;

        }

        return false;

    }

    return false;


}

bool   IsFloatChar( char a ) {
    std::string numberChars( ".-0123456789" );

    return ( numberChars.find( a ) != std::string::npos ) ;
}

bool            SkipFloat( JsonParserState& state, Iterator& it ) {

    bool result( false );

    Iterator current = it;

    if( state.Current( current ) ) {
        while( ( IsFloatChar( *current ) ) ) {
            if( !state.Next( current ) ) {
                break;
            }
        }

        result = true;
    }

    if( result ) {
        it = current;
    }

    return result;

}


bool JsonFloatParserOperator::operator()( JsonParserState& state ) {

    Iterator        current;

    if( state.Current( current ) ) {

        if( !json::SkipChars( state, current ) ) {
            state.Error( JSON_ERROR_SYNTAX, "Failed to parse empty invalid document." );

            return false;
        }

        if( IsFloatChar( *current ) ) {

            Iterator            beginContents = current;
            Iterator            endContents   = current + 1;

            if( !SkipFloat( state, endContents ) ) {
                state.Error( JSON_ERROR_SYNTAX, "Invalid json document." );

                return false;
            }

            /// ToDo: Check if the integer is valid.

            if( state.GetLast().get() != 0 ) {

                if( state.GetLast()->GetType() == JsonValueType::TemporaryElement ) {

                    state.GetLast()->SetType( JsonValueType::Element );
                    state.GetLast()->SetValueType( JsonValueType::Float );
                    state.GetLast()->SetValue( std::string( beginContents, endContents ) );

                    return true;

                }

            }

            libcommon::SharedPtr< JsonElement >  element( new JsonElement() );

            element->SetType( JsonValueType::Float );
            element->SetValue( std::string( beginContents, endContents ) );

            state.Register( element );

            return true;

        }

        return false;

    }

    return false;


}

bool JsonIdParserOperator::operator()( JsonParserState& state ) {

    return true;

}


bool        json::Parse( JsonParserState& state, const std::string& data ) {

    if( data.size() == 0 ) {
        return false;
    }

    state.Setup( data );

    std::vector< libcommon::SharedPtr< JsonElementParserOperator >  > operators;

    {

        operators.push_back( libcommon::SharedPtr< JsonElementParserOperator >( new json::JsonStringParserOperator() ) );
        operators.push_back( libcommon::SharedPtr< JsonElementParserOperator >( new json::JsonArrayBeginParserOperator() ) );
        operators.push_back( libcommon::SharedPtr< JsonElementParserOperator >( new json::JsonArrayEndParserOperator() ) );
        operators.push_back( libcommon::SharedPtr< JsonElementParserOperator >( new json::JsonObjectBeginParserOperator() ) );
        operators.push_back( libcommon::SharedPtr< JsonElementParserOperator >( new json::JsonObjectEndParserOperator() ) );
        operators.push_back( libcommon::SharedPtr< JsonElementParserOperator >( new json::JsonValueSeperatorParserOperator() ) );
        operators.push_back( libcommon::SharedPtr< JsonElementParserOperator >( new json::JsonElementSeperatorParserOperator() ) );
        operators.push_back( libcommon::SharedPtr< JsonElementParserOperator >( new json::JsonIntegerParserOperator() ) );
        operators.push_back( libcommon::SharedPtr< JsonElementParserOperator >( new json::JsonFloatParserOperator() ) );

    }

    Iterator it;

    state.Current( it );

    while( state.Good() ) {

        for( auto st = operators.begin(); st != operators.end(); ++st ) {
            libcommon::SharedPtr< JsonElementParserOperator >    op = *st;

            state.PushState();

            if( !op->operator()( state ) ) {
                state.PopState();
            } else {
                break;
            }
        }

        state.Next( it );

    }

    return true;

}
