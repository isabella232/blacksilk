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

namespace spp {

class       Property;
struct      PropertyDesc;
class       PropertySetter;
class       PropertyGetter;

/**
    \enum   PropertyValueType
    \since  0.4.0-0
    \brief
        Represents the type of the Property internal value.
*/
namespace PropertyValueType {
enum t {
    /// Used for basic integer and decimal types.
    TypeInteger,

    /// Used for floating point types.
    TypeFloat,

    /// Used for strings and char
    /// buffers
    TypeString,

    /// Used for raw structures.
    TypeStructure,

    /// Used for serializable types.
    TypePropertyClass,

    /// Used for pointers.
    TypePointer,

    /// Used for buffers.
    TypeBuffer
};
}

/**
    \class PropertyDesc
    \since 0.4.0-0
    \brief
        Describes and identifies a specific
        property value.
*/
struct PropertyDesc {

        typedef std::string             StringType;
        typedef size_t                  IdType;

        /**
            \fn     PropertyDesc
            \since  0.4.0-0
            \brief
                Constructs  a new PropertyDesc with the specified
                parameters.
        */
        PropertyDesc( const StringType& name );

        /**
            \fn     SetName
            \since  0.4.0-0
            \brief
                Sets the name of the internal
                property.
        */
        void SetName( const StringType& name );

        /**
           \fn      GetName
           \since   0.4.0-0
           \brief
                Returns the name of the specified
                property.
         */
        const StringType&   GetName() const;

        /**
           \fn      GetUniqueIdentifier
           \since   0.4.0-0
           \brief
                Returns the id of the specified
                property.
         */
        const IdType&       GetUniqueIdentifier() const;

    private:
        StringType              m_Name;
        IdType                  m_UniqueIdentifier;
};

/**
    \class  Property
    \since  0.4.0-0
    \brief
        A Property connects meta-information and meta-descriptions
        with a specified object-specific value. The value is accessed
        with getters and setters.
*/
class   Property {
    public:

        /**
            \fn Property
            \since  0.4.0-0
            \brief
                Constructs a new Property instance with the specified
                values.
        */
        Property( const PropertyDesc& desc, const PropertyTypeInfo& info, const PropertyGetter& getter, const PropertySetter& setter );

        /**
            \fn     GetTypeInfo
            \since  0.4.0-0
            \brief
                Returns the internal type info structure, which is used
                to identify the accessed object type.
        */
        PropertyTypeInfo       GetTypeInfo() const;

        /**
            \fn     GetDesc
            \since  0.4.0-0
            \brief
                Returns the current description of this
                property instance.
        */
        PropertyDesc& GetDesc();
        const PropertyDesc& GetDesc() const;

        /**
            \fn     SetDesc
            \since  0.4.0-0
            \brief
                Sets the object description of this
                property instance.
        */
        void SetDesc( const PropertyDesc& desc );

        /**
            \fn     GetValueType
            \since  0.4.0-0
            \brief
                Returns the internally assigned value type for the
                internal property value.
        */
        PropertyValueType::t    GetValueType() const;

        /**
            \fn     SetValueType
            \since  0.4.0-0
            \brief
                Sets the internal assigned value type.
        */
        void    SetValueType( PropertyValueType::t valueType );

        /**
            \fn     GetGetter
            \since  0.4.0-0
            \brief
                Returns the current getter object.
        */
        PropertyGetter&            GetGetter();
        const PropertyGetter&      GetGetter() const;

        /**
            \fn     GetSetter
            \since  0.4.0-0
            \brief
                Returns the setter object.
        */
        PropertySetter&            GetSetter();
        const PropertySetter&      GetSetter() const;

        /**
            \fn     operator =
            \since  0.4.0-0
            \brief
                Assigns a new Property value to this
                instance.
        */
        Property&   operator = ( const Property& prop );
    protected:
        PropertyDesc            m_Description;
        PropertyGetter          m_Getter;
        PropertySetter          m_Setter;
        PropertyTypeInfo        m_TypeInfo;
        PropertyValueType::t    m_Type;
};

template < class _t_container, class _t_index, class _t_owner_type, class _t_value_type >
Property    MakeIndexProperty( const std::string& name, _t_container* container, const _t_index& index ) {

    ///
    /// template < class _t_container, class _t_index, class _t_owner_type, class _t_value_type >
    return Property(
               PropertyDesc( name ),
               spp::GetTypeInfo< _t_value_type >(),

               spp::PropertyGetter(
                   spp::GetTypeInfo< _t_owner_type >(),
                   spp::GetTypeInfo< _t_value_type >(),
                   ( spp::AbstractPropertyGetter* )new spp::PropertyIndexGetter< _t_container, _t_index, _t_owner_type, _t_value_type >( container, index )
               ),
               spp::PropertySetter(
                   spp::GetTypeInfo< _t_owner_type >(),
                   spp::GetTypeInfo< _t_value_type >(),
                   ( spp::AbstractPropertySetter* )(
                       new spp::PropertyIndexSetter< _t_container, _t_index, _t_owner_type, _t_value_type >( container, index )
                   )
               )
           );

}

///  template < class _t_owner_type, class _t_value_type >
template < class _t_owner_type, class _t_value_type >
Property    MakeValueProperty( const std::string& name, _t_value_type( _t_owner_type::*value ) ) {

    return Property(
               PropertyDesc( name ),
               spp::GetTypeInfo< _t_value_type >(),
               spp::PropertyGetter(
                   spp::GetTypeInfo< _t_owner_type >(),
                   spp::GetTypeInfo< _t_value_type >(),
                   ( spp::AbstractPropertyGetter* )( new spp::ValuePropertyGetter<_t_owner_type, _t_value_type >(
                               value
                           ) )
               ),
               spp::PropertySetter(
                   spp::GetTypeInfo< _t_owner_type >(),
                   spp::GetTypeInfo< _t_value_type >(),
                   ( spp::AbstractPropertySetter* )( new spp::ValuePropertySetter<_t_owner_type, _t_value_type >(
                               value
                           ) )
               )
           );

}

template < class _t_owner_type, class _t_value_type >
Property    MakeValueProperty( _t_owner_type* owner, const std::string& name, _t_value_type* value ) {

    ( void )owner;

    return Property(
               PropertyDesc( name ),
               spp::GetTypeInfo< _t_value_type >(),
               spp::PropertyGetter(
                   spp::GetTypeInfo< _t_owner_type >(),
                   spp::GetTypeInfo< _t_value_type >(),
                   ( spp::AbstractPropertyGetter* )( new spp::PointerPropertySetter<_t_owner_type, _t_value_type* >(
                               value
                           ) )
               ),
               spp::PropertySetter(
                   spp::GetTypeInfo< _t_owner_type >(),
                   spp::GetTypeInfo< _t_value_type >(),
                   ( spp::AbstractPropertySetter* )( new spp::PointerPropertyGetter<_t_owner_type, _t_value_type* >(
                               value
                           ) )
               )
           );

}

template < class _t_owner_type, class _t_value_type >
Property    MakeProperty( const std::string& name, _t_value_type( _t_owner_type::*getter )( void )const, void( _t_owner_type::*setter )( _t_value_type ) ) {

    return Property(
               PropertyDesc( name ),
               spp::GetTypeInfo< _t_value_type >(),
               spp::PropertyGetter(
                   spp::GetTypeInfo< _t_owner_type >(),
                   spp::GetTypeInfo< _t_value_type >(),
                   ( spp::AbstractPropertyGetter* ) new spp::FunctionalPropertyGetter<_t_owner_type, _t_value_type >(
                       ( typename spp::FunctionalPropertyGetter<_t_owner_type, _t_value_type >::GetterPtr )getter
                   )
               ),
               spp::PropertySetter(
                   spp::GetTypeInfo< _t_owner_type >(),
                   spp::GetTypeInfo< _t_value_type >(),
                   ( spp::AbstractPropertySetter* )( new spp::FunctionalPropertySetter<_t_owner_type, _t_value_type >(
                               ( typename spp::FunctionalPropertySetter<_t_owner_type, _t_value_type >::SetterPtr )setter
                           ) )
               )
           );

}

template < class _t_owner_type, class _t_value_type >
Property    MakeProperty( const std::string& name, _t_value_type( _t_owner_type::*getter )( void ), void( _t_owner_type::*setter )( _t_value_type ) ) {

    return Property(
               PropertyDesc( name ),
               spp::GetTypeInfo< _t_value_type >(),
               spp::PropertyGetter(
                   spp::GetTypeInfo< _t_owner_type >(),
                   spp::GetTypeInfo< _t_value_type >(),
                   ( spp::AbstractPropertyGetter* )( new spp::FunctionalPropertyGetter<_t_owner_type, _t_value_type >(
                               ( typename spp::FunctionalPropertyGetter<_t_owner_type, _t_value_type >::GetterPtr )getter
                           ) )
               ),
               spp::PropertySetter(
                   spp::GetTypeInfo< _t_owner_type >(),
                   spp::GetTypeInfo< _t_value_type >(),
                   ( spp::AbstractPropertySetter* )( new spp::FunctionalPropertySetter<_t_owner_type, _t_value_type >(
                               ( typename spp::FunctionalPropertySetter<_t_owner_type, _t_value_type >::SetterPtr )setter
                           ) )
               )
           );

}

}
