/**
    Copyright 2013 by FD Imaging

    http://fd-imaging.com

    All rights reserved.
*/
#pragma once

#include <stdexcept>
#include <string>
#include <map>
#include <libserialization++/TypeInfo.hpp>
#include <libserialization++/PropertyEncoder.hpp>
#include <libserialization++/PropertyDecoder.hpp>

namespace spp {

class       TypeErrorException;
class       TypeNameErrorException;
class       Typeable;
class       TypeableConverter;
class       TypeableDecoder;
class       TypeableEncoder;

/**
    \class      TypeableDecoder
    \since      0.4.0-0
    \brief
        The TypeableDecoder class is a basic
        property type decoder.
*/
class   TypeableDecoder {
    public:
        virtual ~TypeableDecoder() {}

        /**
            \fn         Decode
            \since      0.4.0-0
            \brief
                Decodes the given property type to a string.
        */
        virtual std::string   Decode( spp::PropertyTypeInfo& info, void* p ) = 0;
};

/**
    \class      TypeableEncoder
    \since      0.4.0-0
    \brief
        The TypeableEncoder class converts  a string to a property-specific
        value.
*/
class   TypeableEncoder {
    public:
        /**
            \fn     ~TypeableEncoder
            \since  0.4.0-0
            \brief
                Frees the internal data.
        */
        virtual ~TypeableEncoder() {}

        /**
            \fn     Encode
            \since  0.4.0-0
            \brief
               Converts a string to a property-specific
               value.
        */
        virtual void Encode( Property& dst, std::string val ) = 0;
};

/**
    \class      TypeErrorException
    \since      0.4.0-0
    \brief
        Represents a simple cast-based type
        error exception.
*/
class   TypeErrorException : std::runtime_error {
    public:
        TypeErrorException() : std::runtime_error( "Failed to convert to destination type" ) {}
        TypeErrorException( const std::string& msg ) : std::runtime_error( msg ) {}
};

/**
    \class      TypeNameErrorException
    \since      0.4.0-0
    \brief
        Represents a simple cast-based type
        error exception.
*/
class   TypeNameErrorException : std::runtime_error {
    public:
        TypeNameErrorException() : std::runtime_error( "Typename mismatch" ) {}
        TypeNameErrorException( const std::string& msg ) : std::runtime_error( msg ) {}
};

/**
    \class      TypeableConverter
    \since      0.4.0-0
    \brief
        The TypeableConverter is able to cast
        a specific void blob to the specified
        type.
*/
class   TypeableConverter {
    public:

        typedef     void* ( *ConverterFunction )( const void*, const spp::PropertyTypeInfo& );

        /**
            \fn         TypeableConverter
            \since      0.4.0-0
            \brief
                Constructs a new empty TypeableConverter instance from
                the specified TypeInfo and function pointer.
        */
        TypeableConverter( const spp::PropertyTypeInfo& typeInfo, ConverterFunction function );

        /**
            \fn         ~TypeableConverter
            \since      0.4.0-0
            \brief
                Frees the current state and deletes the internal
                data.
        */
        ~TypeableConverter();

        /**
            \fn         Match
            \since      0.4.0-0
            \brief
                Returns true, if this TypeableConverter
                is able to cast the specified pointer
                blob.
        */
        virtual bool    Match( const void* ptr, const spp::PropertyTypeInfo& typeInfo );

        /**
            \fn         GetConverter
            \since      0.4.0-0
            \brief
                Returns the internal converter
                function.
        */
        ConverterFunction               GetConverter() const;

        /**
            \fn         SetConverter
            \since      0.4.0-0
            \brief
                Sets a new converter function.
        */
        void                            SetConverter( ConverterFunction fn );

        /**
            \fn         GetTypeInfo
            \since      0.4.0-0
            \brief
                Returns the current property type
                info structure.
        */
        const spp::PropertyTypeInfo&   GetTypeInfo() const;

    protected:
        ConverterFunction               m_Function;
        const spp::PropertyTypeInfo&    m_TypeInfo;
};

/**
    \class      Typeable
    \since      0.4.0-0
    \brief
        Connects a specific TypeInfo structure
        with a name and extended property
        information.
*/
class   Typeable {
    public:
        /**
            \fn     Typeable
            \since  0.4.0-0
            \brief
                Constructs a new empty Typeable instance from
                the specified property type info.
        */
        Typeable( const spp::PropertyTypeInfo& typeInfo, TypeableDecoder* decoder, TypeableEncoder* encoder, const std::string& typeName = "", size_t typeId = 0 );

        /**
            \fn     Typeable
            \since  0.4.0-0
            \brief
                Basic copy constructor.
        */
        Typeable( const Typeable& typeable );

        /**
            \fn         GetTypeName
            \since      0.4.0-0
            \brief
                Returns the current type name.
        */
        std::string     GetTypeName() const;

        /**
            \fn         SetTypeName
            \since      0.4.0-0
            \brief
                Sets the current type name.
        */
        void            SetTypeName( const std::string& name );

        /**
            \fn         GetTypeId
            \since      0.4.0-0
            \brief
                Returns the current type
                id.
        */
        size_t          GetTypeId() const;

        /**
            \fn         SetTypeId
            \since      0.4.0-0
            \brief
                Sets the current type id.
        */
        void            SetTypeId( size_t id );

        /**
            \fn         GetTypeInfo
            \since      0.4.0-0
            \brief
                Returns the current property
                type info structure.
        */
        const spp::PropertyTypeInfo&    GetTypeInfo() const;

        /**
            \fn         GetEncoder
            \since      0.4.0-0
            \brief
                Returns the Encoder.
        */
        TypeableEncoder*  GetEncoder() const;

        /**
            \fn         SetEncoder
            \since      0.4.0-0
            \brief
                Sets a new encoder.
        */
        void SetEncoder( TypeableEncoder* decoder );

        /**
            \fn         GetDecoder
            \since      0.4.0-0
            \brief
                Returns the decoder.
        */
        TypeableDecoder*  GetDecoder() const;

        /**
            \fn         SetDecoder
            \since      0.4.0-0
            \brief
                Sets a new decoder.
        */
        void SetDecoder( TypeableDecoder* decoder );

        /**
            \fn         GetConverter
            \since      0.4.0-0
            \brief
                Returns the current converter.
        */
        TypeableConverter       GetConverter();
    protected:
        std::string                             m_TypeName;
        size_t                                  m_TypeId;
        const spp::PropertyTypeInfo&            m_PropertyTypeInfo;
        TypeableConverter                       m_Converter;
        TypeableDecoder*                        m_Decoder;
        TypeableEncoder*                        m_Encoder;
};


typedef     std::map< std::string, Typeable >               TypeablePool;

/**
    \fn     GetGlobalTypeablePool
    \since  0.4.0-0
    \brief
        Returns the global typeable pool instance.
*/
TypeablePool&       GetGlobalTypeablePool();

}
