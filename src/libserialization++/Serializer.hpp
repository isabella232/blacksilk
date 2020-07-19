/**
    Copyright 2013 by FD Imaging

    http://fd-imaging.com

    All rights reserved.
*/
#pragma once

#include <vector>

#include <libserialization++/Property.hpp>
#include <libserialization++/PropertyAccessor.hpp>

namespace spp {

class   SerializationInfo;
class   PropertyCollection;

/**
    \class      Serializable
    \since      0.4.0-0
    \brief
        The Serializable is the base class for all instance-based
        property provider types.
*/
class       Serializable {
    public:
        /**
            \fn     ~Serializable
            \since  0.4.0-0
            \brief
                Default destructor.
        */
        virtual ~Serializable() {}

        /**
            \fn     GetProperties
            \since  0.4.0-0
            \brief
                Returns the property-collection for the
                current serializable instance.
        */
        virtual PropertyCollection GetProperties() = 0;

        /**
            \fn     Serialize
            \since  0.4.0-0
            \brief
                Serializes the current Serializable
                instance object.
        */
        virtual bool Serialize( SerializationInfo* serializationInfo ) = 0;

        /**
            \fn     Deserialize
            \since  0.4.0-0
            \brief
                Deserializes the current Serializable instance
                from the specified object.
        */
        virtual bool Deserialize( SerializationInfo* serializationInfo ) = 0;
};

/**
    \class      AutoSerializable
    \since      0.4.0-0
    \brief
        The AutoSerializable class implements the Serialize() and
        Deserialize() methods inherited from the Serializable class
        type.
*/
class   AutoSerializable : public Serializable {
    public:
        /**
            \fn     ~AutoSerializable
            \since  0.4.0-0
            \brief
                Default destructor.
        */
        virtual ~AutoSerializable() {}

        /**
            \fn     Serialize
            \since  0.4.0-0
            \brief
                Serializes the current Serializable
                instance object.
        */
        virtual bool Serialize( SerializationInfo* serializationInfo );

        /**
            \fn     Deserialize
            \since  0.4.0-0
            \brief
                Deserializes the current Serializable instance
                from the specified object.
        */
        virtual bool Deserialize( SerializationInfo* serializationInfo );
};


///  template < class _t_owner_type, class _t_value_type >
template < class _t_owner_type >
spp::Property    MakeProperty( const std::string& name, Serializable* serializable ) {

    return Property(
               PropertyDesc( name ),
               spp::GetTypeInfo< Serializable* >(),
               spp::PropertyGetter(
                   spp::GetTypeInfo< _t_owner_type >(),
                   spp::GetTypeInfo< Serializable* >(),
                   ( spp::AbstractPropertyGetter* )( new spp::PointerPropertyGetter<_t_owner_type, Serializable* >(
                               serializable
                           ) )
               ),
               spp::PropertySetter(
                   spp::GetTypeInfo< _t_owner_type >(),
                   spp::GetTypeInfo< Serializable* >(),
                   ( spp::AbstractPropertySetter* )( new spp::PointerPropertySetter<_t_owner_type, Serializable* >(
                               serializable
                           ) )
               )
           );

}


}
