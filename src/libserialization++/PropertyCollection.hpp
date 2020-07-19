/**
    Copyright 2013 by FD Imaging

    http://fd-imaging.com

    All rights reserved.
*/
#pragma once

#include <vector>
#include <exception>
#include <stdexcept>

#include <libserialization++/PropertyAccessor.hpp>
#include <libserialization++/Property.hpp>
#include <libserialization++/TypeInfo.hpp>

namespace spp {

/**
    \class      PropertyCollection
    \since      0.4.0-0
    \brief
        Represents a simple collection of different
        kinds of properties.

        The PropertyCollection class is able to hold
        and manage properties of one specific owner
        type.
*/
class   PropertyCollection {
    public:

        typedef std::vector< Property >         PropertyVector;
        typedef PropertyVector::iterator        Iterator;
        typedef PropertyVector::const_iterator  ConstIterator;

        /**
            \fn     PropertyCollection
            \since  0.4.0-0
            \brief
                Constructs a new empty PropertyCollection
                instance.
        */
        template  < class _t_obj >
        PropertyCollection( const std::string& name, _t_obj* p = 0 ) : m_OwnerObjectInfo( spp::GetTypeInfo< typename  std::remove_pointer<_t_obj>::type >() ) {
            if( p ) {
                Bind< _t_obj >( p );
            }

            this->m_Name = name;
        }

        PropertyCollection( const std::string& name, const spp::PropertyTypeInfo& info );

        /**
            \fn     PropertyCollection
            \since  0.4.0-0
            \brief
                Copies the complete internal data of another
                property collection.
        */
        PropertyCollection( const PropertyCollection& collection );

        /**
            \fn     ~PropertyCollection
            \since  0.4.0-0
            \brief
                Frees the internal data.
        */
        virtual ~PropertyCollection();

        /**
            \fn     operator <<
            \since  0.4.0-0
            \brief
                Appends a new property to the internal
                property vector list.
        */
        PropertyCollection& operator << ( const Property& property );
        PropertyCollection& operator << ( const PropertyCollection& collection );

        /**
            \fn     Add
            \since  0.4.0-0
            \brief
                Appends a new property to the internal
                property vector list.
        */
        bool Add( const Property& property );
        bool Add( const PropertyCollection& collection );

        /**
            \fn     Bind
            \since  0.4.0-0
            \brief
                Binds the current property collection to the
                specified object.
        */
        bool    Bind( const spp::PropertyTypeInfo& typeInfo, void* ptr );
        bool    Bind( PropertyObjectHandle& handle );

        template < class _t_obj >
        bool    Bind( typename std::remove_pointer<_t_obj>::type* instance ) {
            return this->Bind(
                       spp::GetTypeInfo< typename  std::remove_pointer<_t_obj>::type >(),
                       ( void* )instance
                   );
        }

        /**
            \fn     Begin
            \since  0.4.0-0
            \brief
                Returns the iterator to the first
                element of the internal list.
        */
        Iterator         Begin();
        ConstIterator    Begin() const;
        ConstIterator    BeginConst() const;

        /**
            \fn     End
            \since  0.4.0-0
            \brief
                Returns the iterator to the last
                element of the internal list.
        */
        Iterator         End();
        ConstIterator    End() const;
        ConstIterator    EndConst() const;

        /**
            \fn     Size
            \since  0.4.0-0
            \brief
                Returns the number of currently allocated
                properties.
        */
        size_t      Size() const;

        /**
            \fn     Clear
            \since  0.4.0-0
            \brief
                Removes all properties from the internal
                list.
        */
        void        Clear();

        /**
            \fn     RemovePropertyById
            \since  0.4.0-0
            \brief
                Removes a specific property in the current collection
                by id.
        */
        bool    RemovePropertyById( const size_t& id );

        /**
            \fn     RemovePropertyByName
            \since  0.4.0-0
            \brief
                Removes a specific property in the current collection
                by name.
        */
        bool    RemovePropertyByName( const std::string& name );

        /**
            \fn     GetPropertyByName
            \since  0.4.0-0
            \brief
                Returns the property, which is identified
                by the specified name.
        */
        Property& GetPropertyByName( const std::string& name );

        /**
            \fn     GetPropertyById
            \since  0.4.0-0
            \brief
                Returns the property, which is identified
                by the specified number.
        */
        Property& GetPropertyById( size_t id );

        /**
            \fn     GetName
            \since  0.4.0-0
            \brief
                Returns the name of the current
                property collection.
        */
        std::string GetName() const;

        /**
            \fn     SetName
            \since  0.4.0-0
            \brief
                Sets the name of the current
                property collection.
        */
        void        SetName( const std::string& name );

        /**
            \fn     Inplace
            \since  0.4.0-0
            \brief
                Constructs a new inplace instance.
        */
        template < class _t_obj >
        static PropertyCollection Inplace( const std::string& name, _t_obj* object ) {
            return PropertyCollection( name, object );
        }

    protected:
        void*                               m_OwnerObject;
        const spp::PropertyTypeInfo&        m_OwnerObjectInfo;
        PropertyVector                      m_Properties;
        std::string                         m_Name;
};

}
