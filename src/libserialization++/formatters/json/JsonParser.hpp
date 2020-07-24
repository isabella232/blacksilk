/**
    Copyright 2013 by FD Imaging

    http://fd-imaging.com

    All rights reserved.
*/
#pragma once

#include <string>
#include <typeinfo>

#include <libserialization++/PropertyAccessor.hpp>
#include <libserialization++/TypeInfo.hpp>

#include <libcommon/sharedptr.hpp>

#include <libserialization++/formatters/json/JsonElement.hpp>

#define JSON_ERROR_SYNTAX           0
#define JSON_ERROR_SEMANTIC         1
#define JSON_ERROR_IO               2

namespace spp {
namespace formatters {
namespace json {

typedef std::string::iterator   Iterator;

class   JsonParserState;
class   JsonElementParserOperator;
class   JsonStringParserOperator;
class   JsonObjectBeginParserOperator;
class   JsonObjectEndParserOperator;
class   JsonArrayBeginParserOperator;
class   JsonArrayEndParserOperator;
class   JsonValueSeperatorParserOperator;
class   JsonElementSeperatorParserOPerator;
class   JsonIntegerParserOperator;
class   JsonFloatParserOperator;
class   JsonIdParserOperator;

/**
    \fn         Expect
    \since      0.4.0-0
    \brief
        Returns true, if the next char is equal to the
        specified character value.
*/
bool            Expect( JsonParserState& state, char val );

/**
    \fn         SkipChars
    \since      0.4.0-0
    \brief
        Skips all unparsable chars like ' ' or '\t' or '\n'.
*/
bool            SkipChars( JsonParserState& state, Iterator& it );

/**
    \fn         SkipCharsUntil
    \since      0.4.0-0
    \brief
        Skips all chars until the specified char
        appears.
*/
bool            SkipCharsUntil( JsonParserState& state, Iterator& it, char val );

/**
    \fn         SkipCharsWhile
    \since      0.4.0-0
    \brief
        Skips only the specified
        char.
*/
bool            SkipCharsWhile( JsonParserState& state, Iterator& it, char val );

/**
    \class      JsonElementParserOperator
    \since      0.4.0-0
    \brief
        The basic JsonElementParserOperator class is the base
        class for all different kinds of element parsers.

        Each element parsers is able to parse a specific
        type of json data.
*/
class   JsonElementParserOperator {
    public:
        virtual ~JsonElementParserOperator() {}

        /**
            \fn         operator ()
            \since      0.4.0-0
            \brief
                Parses the specific json element. If the current
                raw data is not parsable, this JsonElementOperator
                generates an error or aborts.
        */
        virtual bool operator()( JsonParserState& state ) = 0;
};

class   JsonStringParserOperator : public JsonElementParserOperator  {
    public:
        virtual ~JsonStringParserOperator() {}

        /**
            \fn         operator ()
            \since      0.4.0-0
            \brief
                Parses a JSON-String.
        */
        virtual bool operator()( JsonParserState& state );
};

class   JsonObjectBeginParserOperator : public JsonElementParserOperator  {
    public:
        virtual ~JsonObjectBeginParserOperator() {}

        /**
            \fn         operator ()
            \since      0.4.0-0
            \brief
                Parses a JSON-String.
        */
        virtual bool operator()( JsonParserState& state );
};

class   JsonObjectEndParserOperator : public JsonElementParserOperator  {
    public:
        virtual ~JsonObjectEndParserOperator() {}

        /**
            \fn         operator ()
            \since      0.4.0-0
            \brief
                Parses a JSON-String.
        */
        virtual bool operator()( JsonParserState& state );
};

class   JsonArrayBeginParserOperator : public JsonElementParserOperator  {
    public:
        virtual ~JsonArrayBeginParserOperator() {}

        /**
            \fn         operator ()
            \since      0.4.0-0
            \brief
                Parses a JSON-String.
        */
        virtual bool operator()( JsonParserState& state );
};

class   JsonArrayEndParserOperator : public JsonElementParserOperator  {
    public:
        virtual ~JsonArrayEndParserOperator() {}

        /**
            \fn         operator ()
            \since      0.4.0-0
            \brief
                Parses a JSON-String.
        */
        virtual bool operator()( JsonParserState& state );
};

class   JsonValueSeperatorParserOperator  : public JsonElementParserOperator  {
    public:
        virtual ~JsonValueSeperatorParserOperator() {}

        /**
            \fn         operator ()
            \since      0.4.0-0
            \brief
                Parses a JSON-String.
        */
        virtual bool operator()( JsonParserState& state );
};

class   JsonElementSeperatorParserOperator : public JsonElementParserOperator  {
    public:
        virtual ~JsonElementSeperatorParserOperator() {}

        /**
            \fn         operator ()
            \since      0.4.0-0
            \brief
                Parses a JSON-String.
        */
        virtual bool operator()( JsonParserState& state );
};

class   JsonIntegerParserOperator : public JsonElementParserOperator  {
    public:
        virtual ~JsonIntegerParserOperator() {}

        /**
            \fn         operator ()
            \since      0.4.0-0
            \brief
                Parses a JSON-String.
        */
        virtual bool operator()( JsonParserState& state );
};

class   JsonFloatParserOperator : public JsonElementParserOperator  {
    public:
        virtual ~JsonFloatParserOperator() {}

        /**
            \fn         operator ()
            \since      0.4.0-0
            \brief
                Parses a JSON-String.
        */
        virtual bool operator()( JsonParserState& state );
};

class   JsonIdParserOperator : public JsonElementParserOperator  {
    public:
        virtual ~JsonIdParserOperator() {}

        /**
            \fn         operator ()
            \since      0.4.0-0
            \brief
                Parses a JSON-String.
        */
        virtual bool operator()( JsonParserState& state );
};

struct  JsonParserStateFrame {
    size_t          Line;
    size_t          Position;
    size_t          Depth;

    JsonParserStateFrame() : Line( 0 ), Position( 0 ), Depth( 0 ) {}
    ~JsonParserStateFrame() {}
};
struct JsonParserErrorType {
    size_t          ID;
    std::string     Name;
};
struct JsonParserError {
    size_t          Line;
    size_t          Type;
    std::string     Message;
};

/**
    \class      JsonParserState
    \since      0.4.0-0
    \brief
        Represents the state of the current
        JsonParser pipeline.
*/
class   JsonParserState {
    public:
        /**
            \fn     JsonParserState
            \since  0.4.0-0
            \brief
                Constructs a new default empty JsonParserState
                instance.
        */
        JsonParserState();

        /**
            \fn     JsonParserState
            \since  0.4.0-0
            \brief
                Constructs a new JsonParserState instance from the
                specified data.
        */
        JsonParserState( const std::string& data );

        /**
            \fn     JsonParserState
            \since  0.4.0-0
            \brief
                Basic destruvtor.
        */
        ~JsonParserState() {
            // Clear();
        }

        /**
            \fn     Setup
            \since  0.4.0-0
            \brief
                Discards the current state and setups a new state based
                on the provided data.
        */
        void    Setup( const std::string& data );

        /**
            \fn     GetLineCounter
            \since  0.4.0-0
            \brief
                Returns the current line-counter.
        */
        size_t  GetLineCounter() const;

        /**
            \fn     GetScopeDepth
            \since  0.4.0-0
            \brief
                Returns the current scope depth.
        */
        size_t  GetScopeDepth() const;

        /**
            \fn     GetPosition
            \since  0.4.0-0
            \brief
                Returns the current internal
                position.
        */
        size_t  GetPosition() const;

        /**
            \fn     Begin
            \since  0.4.0-0
            \brief
                Returns the first data
                iterator.
        */
        Iterator    Begin() const;

        /**
            \fn     End
            \since  0.4.0-0
            \brief
                Returns the last data
                iterator.
        */
        Iterator    End() const;

        /**
            \fn     Current
            \since  0.4.0-0
            \brief
                Returns true, if the provided iterator
                was sucessfully filled with the current
                iterator data.
        */
        bool    Current( Iterator& it );

        /**
            \fn     Next
            \since  0.4.0-0
            \brief
                Returns true, if the JsonParserState instance was
                able to iterate to the next char value. Fills it
                with the new current iterator.
        */
        bool    Next( Iterator& it );

        /**
            \fn     Previous
            \since  0.4.0-0
            \brief
                Returns true, if the JsonParserState instance was
                able to iterate to the previous char value. Fills it
                with the new current iterator.
        */
        bool    Previous( Iterator& it );

        /**
            \fn     Register
            \since  0.4.0-0
            \brief
                Registers a new element and assigns the new element to the
                current scope.
        */
        void    Register( const std::shared_ptr< JsonElement >& elem );

        /**
            \fn     RegisterErrorType
            \since  0.4.0-0
            \brief
                Registers a new error type.
        */
        void    RegisterErrorType( const JsonParserErrorType& type );
        void    RegisterErrorType( size_t id, const std::string& name );

        /**
            \fn     Error
            \since  0.4.0-0
            \brief
                Logs a new error and sets the current state flag
                to invalid.
        */
        void    Error( size_t id, const std::string& message );

        /**
            \fn     GetErrors
            \since  0.4.0-0
            \brief
                Returns the current error list.
        */
        std::vector< JsonParserError >  GetErrors() const;

        /**
            \fn     GetErrorTypes
            \since  0.4.0-0
            \brief
                Returns the current error type list.
        */
        std::vector< JsonParserErrorType > GetErrorTypes() const;

        /**
            \fn     EnterScope
            \since  0.4.0-0
            \brief
                Enters the scope of the specified
                element.
        */
        void        EnterScope( const std::shared_ptr< JsonElement >& elem );

        /**
            \fn     LeaveScope
            \since  0.4.0-0
            \brief
                Leaves the current scope.
        */
        void        LeaveScope();

        /**
            \fn     PushState
            \since  0.4.0-0
            \brief
                Saves the current
                state.
        */
        void        PushState();

        /**
            \fn     PopState
            \since  0.4.0-0
            \brief
                Restores the last
                state.
        */
        void        PopState();

        /**
            \fn     Clear
            \since  0.4.0-0
            \brief
                Discards the internal
                state and deletes all
                data.
        */
        void        Clear();

        /**
            \fn     GetData
            \since  0.4.0-0
            \brief
                Returns the current
                data chunk.
        */
        std::string GetData() const;

        /**
            \fn     Good
            \since  0.4.0-0
            \brief
                Returns true, if the current JsonParserState
                is still valid.
        */
        bool    Good() const;

        /**
            \fn     GetLast
            \since  0.4.0-0
            \brief
                Returns the last
                element.
        */
        std::shared_ptr< JsonElement >      GetLast() const;

        /**
            \fn     GetRoot
            \since  0.4.0-0
            \brief
                Returns the root
                element.
        */
        std::shared_ptr< JsonElement >      GetRoot() const;

        /**
            \fn     GetScope
            \since  0.4.0-0
            \brief
                Returns the current scope
                element.
        */
        std::shared_ptr< JsonElement >      GetScope() const;

        /**
            \fn     GetElements
            \since  0.4.0-0
            \brief
                Returns all elements.
        */
        std::vector< std::shared_ptr< JsonElement > >      GetElements() const;

    protected:
        std::vector< std::shared_ptr< JsonElement > >           m_ScopeStack;
        std::vector< JsonParserStateFrame >                     m_StateStack;
        std::vector< JsonParserErrorType >                      m_ErrorTypes;
        std::vector< JsonParserError >                          m_Errors;
        std::shared_ptr< JsonElement >                          m_LastElement;
        std::shared_ptr< JsonElement >                          m_Root;
        std::vector< std::shared_ptr< JsonElement > >           m_Elements;

        size_t                                                  m_Depth;
        size_t                                                  m_Line;
        size_t                                                  m_Position;

        std::string                                             m_Data;
        Iterator                                                m_Begin;
        Iterator                                                m_End;
        Iterator                                                m_Current;

        bool                                                    m_Good;
};

/**
    \fn     Parse
    \since  0.4.0-0
    \brief
        Parses a raw string stream into
        a JsonElement tree.
*/
bool        Parse( JsonParserState& state, const std::string& data );

}
}
}
