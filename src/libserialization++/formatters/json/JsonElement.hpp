/**
    Copyright 2013 by FD Imaging

    http://fd-imaging.com

    All rights reserved.
*/
#pragma once

#include <vector>
#include <string>
#include <typeinfo>

#include <libserialization++/PropertyAccessor.hpp>
#include <libserialization++/TypeInfo.hpp>
#include <libserialization++/SerializationStream.hpp>

namespace spp {
namespace formatters {
namespace json {

/**
    \struct         PrinterFormattingOptions
    \since          0.4.0-0
    \brief
        The PrinterFormattingOptions structure influences
        the formatting of any given Json-Serialization
        routine.
*/
struct  PrinterFormattingOptions {
    /**
        The number of spaces between tokens.
    */
    size_t      TokenSpaceCount;

    /**
        The number of spaces between commata and
        next token.
    */
    size_t      TokenCommataSpaceCount;

    /**
        The number of spaces between token and
        scope modifier( [, }, {, ] )
    */
    size_t      TokenScopeModifierSpaceCount;

    /**
        Is true, if the JsonElement should  write a new
        line after before new scope opening.
    */
    bool        BeforeNewLineScopeBegin;

    /**
        Is true, if the JsonElement should  write a new
        line after each new scope opening.
    */
    bool        AfterNewLineScopeBegin;

    /**
        Is true, if the JsonElement should  write a new
        line after before new scope closing.
    */
    bool        BeforeNewLineScopeEnd;

    /**
        Is true, if the JsonElement should  write a new
        line after each new scope closing.
    */
    bool        AfterNewLineScopeEnd;

    /**
        Is true, if the JsonElement should write a new
        line after each commata.
    */
    bool        AfterNewLineCommata;

    /**
        \fn     PrinterFormattingOptions
        \since  0.4.0-0
        \brief
            Constructs a new default PrinterFormattingOptions
            structure. Default mode is Lean.
    */
    PrinterFormattingOptions();

    /**
        \fn     Lean
        \since  0.4.0-0
        \brief
            Constructs a new PrinterFormattingOptions instance, which
            uses minimal memory space.
    */
    static PrinterFormattingOptions Lean();

    /**
        \fn     Pretty
        \since  0.4.0-0
        \brief
            Constructs a new PrinterFormattingOptions instance, which
            is easy to read.
    */
    static PrinterFormattingOptions Pretty();
};

/**
    \enum           JsonValueType
    \since          0.4.0-0
    \brief
        Represents the value of a specific json
        element.
*/
namespace JsonValueType {
enum t {

    /// String-Values or Character Arrays
    String,

    /// Integer/Decimal Types
    Integer,

    /// Floating-Point Types
    Float,

    /// List Value
    Array,

    /// Temporary Array
    TemporaryArray,

    /// Object Value
    Object,

    /// Temporary Object
    TemporaryObject,

    /// Element
    Element,

    /// TemporaryElement
    TemporaryElement,

    /// Unknown
    Unknown

};
}

/**
    \class  JsonElement
    \since  0.4.0-0
    \brief
        Basic element.
*/
class   JsonElement {
    public:

        /**
            \fn         JsonElement
            \since      0.4.0-0
            \brief
                Constructs a new empty JsonElement
                instance.
        */
        JsonElement();

        /**
            \fn         JsonElement
            \since      0.4.0-0
            \brief
                Constructs a new empty JsonElement
                instance. Reads a new state from the
                internal stream.

        */
        JsonElement( Stream* stream );

        /**
            \fn         ~JsonElement
            \since      0.4.0-0
            \brief
                Frees the current state.
        */
        virtual ~JsonElement();

        /**
            \fn         ChildBegin
            \since      0.4.0-0
            \brief
                Returns the iterator to the first
                JsonElement sibling vector.
        */
        std::vector< std::shared_ptr< JsonElement > >::iterator ChildBegin();

        /**
            \fn         ChildEnd
            \since      0.4.0-0
            \brief
                Returns the iterator to the last
                JsonElement sibling vector.
        */
        std::vector< std::shared_ptr< JsonElement > >::iterator ChildEnd();

        /**
            \fn         SiblingBegin
            \since      0.4.0-0
            \brief
                Returns the iterator to the first
                JsonElement sibling vector.
        */
        std::vector< std::shared_ptr< JsonElement > >::iterator SiblingBegin();

        /**
            \fn         SiblingEnd
            \since      0.4.0-0
            \brief
                Returns the iterator to the last
                JsonElement sibling vector.
        */
        std::vector< std::shared_ptr< JsonElement > >::iterator SiblingEnd();

        /**
            \fn         SetValue
            \since      0.4.0-0
            \brief
                Sets the current value.
        */
        void            SetValue( const std::string& value );
        void            SetValue( const std::wstring& value );

        /**
            \fn         SetName
            \since      0.4.0-0
            \brief
                Sets the current name.
        */
        void            SetName( const std::string& name );
        void            SetName( const std::wstring& name );

        /**
            \fn         GetName
            \since      0.4.0-0
            \brief
                Returns the current JsonElement
                name.
        */
        std::wstring    GetName() const;
        std::string     GetAnsiName() const;

        /**
            \fn         GetValue
            \since      0.4.0-0
            \brief
                Returns the current JsonElement
                value.
        */
        std::wstring    GetValue() const;
        std::string     GetAnsiValue() const;

        /**
            \fn         Read
            \since      0.4.0-0
            \brief
                Reads a new JsonElement from the specified
                stream. Deletes the complete internal state before
                reading.
        */
        bool            Read( spp::Stream*  stream );

        /**
            \fn         Write
            \since      0.4.0-0
            \brief
                Writes the current JsonElement to the specified
                stream.
        */
        bool            Write( spp::Stream* stream, bool prettyPrint = false, size_t tabs = 0 );

        /**
            \fn         AddSiblingRef
            \since      0.4.0-0
            \brief
                Adds a new sibling reference to the
                current JsonElement instance.
        */
        void            AddSiblingRef( const std::shared_ptr< JsonElement >& sibling );
        void            AddSiblingRef( JsonElement* sibling );

        /**
            \fn         ClearSiblings
            \since      0.4.0-0
            \brief
                Removes all sibling references from the current
                state.
        */
        void            ClearSiblings();

        /**
            \fn         RemoveSibling
            \since      0.4.0-0
            \brief
                Removes  a specific sibling from the current
                state.
        */
        bool            RemoveSibling( const std::shared_ptr< JsonElement >& sibling );
        bool            RemoveSibling( JsonElement* sibling );

        /**
            \fn         CountSiblings
            \since      0.4.0-0
            \brief
                Returns the number of registered siblings.
        */
        size_t          CountSiblings() const;

        /**
            \fn         AddChild
            \since      0.4.0-0
            \brief
                Adds a new child element to the current
                JsonElement instance.
        */
        void            AddChild( const std::shared_ptr< JsonElement >& child );
        void            AddChild( JsonElement* child );

        /**
            \fn         ClearChildren
            \since      0.4.0-0
            \brief
                Removes all child references from the
                current state.
        */
        void            ClearChildren();

        /**
            \fn         RemoveChild
            \since      0.4.0-0
            \brief
                Removes a specified Child instance from
                the current state.
        */
        bool            RemoveChild( const std::shared_ptr< JsonElement >& sibling );
        bool            RemoveChild( JsonElement* sibling );

        /**
            \fn         CountChildren
            \since      0.4.0-0
            \brief
                Returns the number of registered siblings.
        */
        size_t          CountChildren() const;

        /**
            \fn         Reset
            \since      0.4.0-0
            \brief
                Resets the current state of the
                JsonElement instance.
        */
        void            Reset();

        /**
            \fn         GetType
            \since      0.4.0-0
            \brief
                Returns the current set JsonValueType.
        */
        JsonValueType::t    GetValueType() const;

        /**
            \fn         SetType
            \since      0.4.0-0
            \brief
                Sets a new JsonValueType.
        */
        void                SetValueType( JsonValueType::t val );

        /**
            \fn         GetType
            \since      0.4.0-0
            \brief
                Returns the current set JsonValueType.
        */
        JsonValueType::t    GetType() const;

        /**
            \fn         SetType
            \since      0.4.0-0
            \brief
                Sets a new JsonValueType.
        */
        void                SetType( JsonValueType::t val );

    protected:
        std::wstring                                        m_Name;
        std::wstring                                        m_Value;
        JsonValueType::t                                    m_Type;
        JsonValueType::t                                    m_ValueType;
        std::vector< std::shared_ptr< JsonElement > >       m_Siblings;
        std::vector< std::shared_ptr< JsonElement > >       m_Children;
};



}
}
}
