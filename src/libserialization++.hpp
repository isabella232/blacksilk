/**
    Copyright 2013 by FD Imaging

    http://fd-imaging.com

    All rights reserved.
*/
#pragma once


/// Include Main Library Headers
#include <libserialization++/Collections.hpp>
#include <libserialization++/Property.hpp>
#include <libserialization++/PropertyAccessor.hpp>
#include <libserialization++/PropertyDecoder.hpp>
#include <libserialization++/PropertyEncoder.hpp>
#include <libserialization++/SerializationFormatter.hpp>
#include <libserialization++/SerializationInfo.hpp>
#include <libserialization++/SerializationProvider.hpp>
#include <libserialization++/SerializationStream.hpp>
#include <libserialization++/Serializer.hpp>
#include <libserialization++/Typeable.hpp>
#include <libserialization++/TypeInfo.hpp>

/// Include default binary formatter
#include <libserialization++/formatters/binary/BinaryFormatter.hpp>

/// Include default json formatter
#include <libserialization++/formatters/json/JsonElement.hpp>
#include <libserialization++/formatters/json/JsonFormatter.hpp>
#include <libserialization++/formatters/json/JsonParser.hpp>

/// Shortcuts/Helper Functions

/**

    The following set of functions represent shortcuts/helper functions
    for serializing serializable objects.

*/
/**
    \fn         SppSerializeJsonToFile
    \since      0.4.0-0
    \brief
        Serializes the given object as json representation
        to the specified file.
*/
template < class _t_class_type >
static inline bool SppSerializeJsonToFile( _t_class_type* type, const std::string& path ) {

    if( static_cast< spp::Serializable* >( type ) == 0 ) {
        return false;
    }

    spp::FileStream                         fileStream( path, spp::FileOpenMode::AlwaysCreate, spp::FileStreamMode::Writable );
    spp::formatters::json::Formatter        jsonFormatter;

    jsonFormatter.Reset( &fileStream );

    bool result = jsonFormatter.Serialize(
                      ( ( spp::Serializable* )type )->GetProperties()
                  );

    fileStream.Flush();
    fileStream.Close();

    return result;
}

static inline bool SppSerializeJsonToFile( spp::PropertyCollection& collection, const std::string& path ) {

    spp::FileStream                         fileStream( path, spp::FileOpenMode::AlwaysCreate, spp::FileStreamMode::Writable );
    spp::formatters::json::Formatter        jsonFormatter;

    jsonFormatter.Reset( &fileStream );

    bool result = jsonFormatter.Serialize(
                      collection
                  );

    fileStream.Flush();
    fileStream.Close();

    return result;
}


/**
    \fn         SppSerializeBinaryToFile
    \since      0.4.0-0
    \brief
        Serializes the given object as binary representation
        to the specified file.
*/
template < class _t_class_type >
static inline bool SppSerializeBinaryToFile( _t_class_type* type, const std::string& path ) {

    if( static_cast< spp::Serializable* >( type ) == 0 ) {
        return false;
    }

    spp::FileStream                         fileStream( path, spp::FileOpenMode::AlwaysCreate, spp::FileStreamMode::Writable );
    spp::formatters::binary::Formatter      binaryFormatter;

    binaryFormatter.Reset( &fileStream );

    bool result = binaryFormatter.Serialize(
                      ( ( spp::Serializable* )type )->GetProperties()
                  );

    fileStream.Flush();
    fileStream.Close();

    return result;
}

static inline bool SppSerializeBinaryToFile( spp::PropertyCollection& collection, const std::string& path ) {

    spp::FileStream                         fileStream( path, spp::FileOpenMode::AlwaysCreate, spp::FileStreamMode::Writable );
    spp::formatters::binary::Formatter      binaryFormatter;

    binaryFormatter.Reset( &fileStream );

    bool result = binaryFormatter.Serialize(
                      collection
                  );

    fileStream.Flush();
    fileStream.Close();

    return result;
}

/**
    \fn         SppSerializeJsonToMemory
    \since      0.4.0-0
    \brief
        Serializes the given object as json representation
        to the specified buffer.
*/
template < class _t_class_type >
static inline bool SppSerializeJsonToMemory( _t_class_type* type, std::string& buffer ) {

    if( static_cast< spp::Serializable* >( type ) == 0 ) {
        return false;
    }

    spp::MemoryStream                       memoryStream( ( void* )buffer.c_str(), spp::MemoryStreamMode::ReadWritable );
    spp::formatters::json::Formatter        jsonFormatter;

    jsonFormatter.Reset( &memoryStream );

    bool result = jsonFormatter.Serialize(
                      ( ( spp::Serializable* )type )->GetProperties()
                  );

    memoryStream.Flush();
    memoryStream.Close();

    return result;
}

static inline bool SppSerializeJsonToMemory( spp::PropertyCollection& collection, std::string& buffer ) {


    spp::MemoryStream                       memoryStream( ( void* )buffer.c_str(), spp::MemoryStreamMode::ReadWritable );
    spp::formatters::json::Formatter        jsonFormatter;

    jsonFormatter.Reset( &memoryStream );

    bool result = jsonFormatter.Serialize(
                      collection
                  );

    memoryStream.Flush();
    memoryStream.Close();

    return result;
}



/**
    \fn         SppSerializeBinaryToMemory
    \since      0.4.0-0
    \brief
        Serializes the given object as binary representation
        to the specified buffer.
*/
template < class _t_class_type >
static inline bool SppSerializeBinaryToMemory( _t_class_type* type, std::string& buffer ) {

    if( static_cast< spp::Serializable* >( type ) == 0 ) {
        return false;
    }

    spp::MemoryStream                       memoryStream( ( void* )buffer.c_str(), spp::MemoryStreamMode::ReadWritable );
    spp::formatters::binary::Formatter      binaryFormatter;

    binaryFormatter.Reset( &memoryStream );

    bool result = binaryFormatter.Serialize(
                      ( ( spp::Serializable* )type )->GetProperties()
                  );

    memoryStream.Flush();
    memoryStream.Close();

    return result;
}

static inline bool SppSerializeBinaryToMemory( spp::PropertyCollection& collection, std::string& buffer ) {

    spp::MemoryStream                       memoryStream( ( void* )buffer.c_str(), spp::MemoryStreamMode::ReadWritable );
    spp::formatters::binary::Formatter      binaryFormatter;

    binaryFormatter.Reset( &memoryStream );

    bool result = binaryFormatter.Serialize(
                      collection
                  );

    memoryStream.Flush();
    memoryStream.Close();

    return result;
}


/**

    The following set of functions represent shortcuts/helper functions
    for deserializing serializable objects.

*/

/**
    \fn         SppDeserializeJsonFromFile
    \since      0.4.0-0
    \brief
        Deserializes the given object from the specified json
        file.
*/
static inline bool SppDeserializeJsonFromFile( spp::PropertyContainer* type, const std::string& path ) {

    if( static_cast< spp::Serializable* >( type ) == 0 ) {
        return false;
    }

    spp::FileStream                         fileStream( path, spp::FileOpenMode::Open, spp::FileStreamMode::Readable );
    spp::formatters::json::Formatter        jsonFormatter;

    jsonFormatter.Reset( &fileStream );

    bool result = jsonFormatter.Deserialize(
                      type
                  );

    fileStream.Close();

    return result;
}

/**
    \fn         SppDeserializeBinaryFromFile
    \since      0.4.0-0
    \brief
        Deserializes the given object from the specified binary
        file.
*/
template < class _t_class_type >
static inline bool SppDeserializeBinaryFromFile( spp::PropertyContainer* type, const std::string& path ) {

    if( static_cast< spp::Serializable* >( type ) == 0 ) {
        return false;
    }

    spp::FileStream                         fileStream( path, spp::FileOpenMode::Open, spp::FileStreamMode::Readable );
    spp::formatters::binary::Formatter      binaryFormatter;

    binaryFormatter.Reset( &fileStream );

    bool result = binaryFormatter.Deserialize(
                      type
                  );

    fileStream.Close();

    return result;
}

/**
    \fn         SppDeserializeJsonFromFile
    \since      0.4.0-0
    \brief
        Deserializes the given object from the specified json
        file.
*/
static inline bool SppDeserializeJsonFromFile( spp::Serializable* type, const std::string& path ) {

    if( static_cast< spp::Serializable* >( type ) == 0 ) {
        return false;
    }

    spp::FileStream                         fileStream( path, spp::FileOpenMode::Open, spp::FileStreamMode::Readable );
    spp::formatters::json::Formatter        jsonFormatter;

    jsonFormatter.Reset( &fileStream );

    bool result = jsonFormatter.Deserialize(
                      ( ( spp::Serializable* )type )->GetProperties()
                  );

    fileStream.Close();

    return result;
}

static inline bool SppDeserializeJsonFromFile( spp::PropertyCollection& collection, const std::string& path ) {

    spp::FileStream                         fileStream( path, spp::FileOpenMode::Open, spp::FileStreamMode::Readable );
    spp::formatters::json::Formatter        jsonFormatter;

    jsonFormatter.Reset( &fileStream );

    bool result = jsonFormatter.Deserialize(
                      collection
                  );

    fileStream.Close();

    return result;
}

/**
    \fn         SppDeserializeBinaryFromFile
    \since      0.4.0-0
    \brief
        Deserializes the given object from the specified binary
        file.
*/
static inline bool SppDeserializeBinaryFromFile( spp::Serializable* type, const std::string& path ) {

    if( static_cast< spp::Serializable* >( type ) == 0 ) {
        return false;
    }

    spp::FileStream                         fileStream( path, spp::FileOpenMode::Open, spp::FileStreamMode::Readable );
    spp::formatters::binary::Formatter      binaryFormatter;

    binaryFormatter.Reset( &fileStream );

    bool result = binaryFormatter.Deserialize(
                      ( ( spp::Serializable* )type )->GetProperties()
                  );

    fileStream.Close();

    return result;
}

static inline bool SppDeserializeBinaryFromFile( spp::PropertyCollection& collection, const std::string& path ) {

    spp::FileStream                         fileStream( path, spp::FileOpenMode::Open, spp::FileStreamMode::Readable );
    spp::formatters::binary::Formatter      binaryFormatter;

    binaryFormatter.Reset( &fileStream );

    bool result = binaryFormatter.Deserialize(
                      collection
                  );

    fileStream.Close();

    return result;
}
