/**
    Copyright 2013 by FD Imaging

    http://fd-imaging.com

    All rights reserved.
*/
#pragma once


#include <vector>
#include <map>
#include <stack>
#include <list>

#include <libserialization++/PropertyDecoder.hpp>
#include <libserialization++/PropertyEncoder.hpp>
#include <libserialization++/Property.hpp>
#include <libserialization++/Serializer.hpp>
#include <libserialization++/Typeable.hpp>

namespace spp {

class       PropertyContainer;
class       AnyContainer;

template < class _t_value >
class       Vector;

template < class _t_value >
class       List;

template < class _t_index, class _t_value >
class       Map;

template < class _t_value >
class       Stack;

/**
    \class      PropertyContainer
    \since      0.4.0-0
    \brief
        The PropertyContainer is the base class for all types
        of single-typed container classes, which support
        serialization.
*/
class PropertyContainer : public AutoSerializable {
    public:
        /**
            \fn         PropertyContainer
            \since      0.4.0-0
            \brief
                Constructs a new PropertyContainer from the specified
                type info.
        */
        PropertyContainer( const spp::PropertyTypeInfo& info );

        /**
            \fn         ~PropertyContainer
            \since      0.4.0-0
            \brief
                Frees the current state.
        */
        virtual ~PropertyContainer() {}

        /**
            \fn         GetLastProperty
            \since      0.4.0-0
            \brief
                Returns the last element as
                property.
        */
        virtual spp::Property      GetLastProperty() = 0;

        /**
            \fn         GetElementTypeInfo
            \since      0.4.0-0
            \brief
                Returns the element type info
                structure.
        */
        const spp::PropertyTypeInfo&  GetElementTypeInfo() const;

        /**
            \fn         AddProperty
            \since      0.4.0-0
            \brief
                Adds a new element to the current container
                from the specified property element.
        */
        virtual bool        AddProperty( spp::Property& property ) = 0;
        virtual bool        AddProperty( const std::string& name, const spp::PropertyTypeInfo& typeInfo, void* data ) = 0;

        /**
            \fn         RemoveProperty
            \since      0.4.0-0
            \brief
                Removes a value, which is equal to the specified
                property from the internal container.
        */
        virtual bool        RemoveProperty( spp::Property& property ) = 0;

        /**
            \fn         ContainsProperty
            \since      0.4.0-0
            \brief
                Returns true, if the current container contains
                the specified property instance.
        */
        virtual bool        ContainsProperty( spp::Property& property ) = 0;
        virtual bool        ContainsProperty( const std::string& name ) = 0;

        /**
            \fn         IsPropertySerializable
            \since      0.4.0-0
            \brief
                Returns true, if the current assigned type is
                a spp::Serializable/spp::AutoSerializable implementation.
        */
        virtual bool        IsLastPropertySerializable() = 0;

        /**
            \fn         CreateInstance
            \since      0.4.0-0
            \brief
                Creates a new instance of the internal
                property type.
        */
        virtual void*       CreateInstance() = 0;
    protected:
        spp::PropertyTypeInfo           m_ElementTypeInfo;
};

/**
    \class      Vector
    \since      0.4.0-0
    \brief
        A serializable vector class
        wrapper.
*/
template < class _t_value >
class       Vector : public PropertyContainer {
    public:
        typedef     _t_value                ValueType;
        typedef     _t_value&               Reference;
        typedef     const _t_value&         ConstReference;
        typedef     _t_value*               Pointer;
        typedef     const _t_value*         ConstPointer;

        typedef     std::vector< ValueType >                VectorType;
        typedef     Vector< ValueType >                     SelfType;
        typedef     typename VectorType::iterator           Iterator;
        typedef     typename VectorType::const_iterator     ConstIterator;

        static const size_t DefaultVectorSize = 8;

        /**
            \fn         Vector
            \since      0.4.0-0
            \brief
                Constructs a Vector instance of the
                specified size.
        */
        Vector( size_t len = DefaultVectorSize ) : PropertyContainer( spp::GetTypeInfo< ValueType >() ) {

            Reserve( len );

        }

        /**
            \fn         Vector
            \since      0.4.0-0
            \brief
                Constructs a Vector instance of the
                specified size and fills all elements
                with the specified value.
        */
        Vector( size_t len, ConstReference ref ) : PropertyContainer( spp::GetTypeInfo< ValueType >() ) {

            Assign( len, ref );

        }

        /**
            \fn         Vector
            \since      0.4.0-0
            \brief
                Copies data from another
                vector instance.
        */
        Vector( const SelfType& other ) : PropertyContainer( spp::GetTypeInfo< ValueType >() )  {

            Assign( other.Begin(), other.End() );

        }

        /**
            \fn         Vector
            \since      0.4.0-0
            \brief
                Assigns the internal data from the
                specified raw buffer.
        */
        Vector( Pointer elements, size_t length ) : PropertyContainer( spp::GetTypeInfo< ValueType >() )  {

            Assign( elements, length );

        }

        /**
            \fn         Vector
            \since      0.4.0-0
            \brief
                Swaps the data between this and
                another Vector instance.
        */
        Vector( Vector&& rhs ) : PropertyContainer( spp::GetTypeInfo< ValueType >() )  {

            Swap( std::move( rhs ) );

        }

        /**
            \fn         Vector
            \since      0.4.0-0
            \brief
                Assigns the specified content
                to the new vector instance.
        */
        template < class InputIterator >
        Vector( InputIterator begin, InputIterator end ) : PropertyContainer( spp::GetTypeInfo< ValueType >() ) {

            Assign( begin, end );

        }

        /**
            \fn         ~Vector
            \since      0.4.0-0
            \brief
                Frees the current state.
        */
        virtual ~Vector() {

            m_Elements.clear();

        }

        /**
            \fn         Sort
            \since      0.4.0-0
            \brief
                Sorts the given internal container using the
                specified comperator.
        */
        template < class _t_comperator >
        void    Sort( _t_comperator comp ) {

            size_t          minimal( 0 );
            size_t          left( 0 );
            size_t          length( this->m_Elements.capacity() );

            do {

                for( size_t i = left + 1; length > i; ++i ) {

                    if( comp( m_Elements[i], m_Elements[minimal] ) ) {

                        minimal = i;

                        break;

                    }

                }

                std::swap(
                    m_Elements[ minimal ],
                    m_Elements[ left ]
                );


            } while( left < length );

        }

        /**
            \fn         operator ==
            \since      0.4.0-0
            \brief
                Returns true, if this and another vector are
                equal.
        */
        bool            operator == ( const SelfType& other ) {

            if( Size() != other.Size() ) {
                return false;
            }

            size_t count( 0 );

            for( auto it = Begin(); it != End(); ++it ) {

                if( other.At( count ) != ( *it ) ) {

                    return false;

                }

            }

            return true;
        }

        /**
            \fn         operator !=
            \since      0.4.0-0
            \brief
                Returns true, if this and another vector are
                not equal.
        */
        bool            operator != ( const SelfType& other ) {

            if( Size() != other.Size() ) {
                return true;
            }

            size_t count( 0 );

            for( auto it = Begin(); it != End(); ++it ) {

                if( other.At( count ) != ( *it ) ) {

                    return true;

                }

            }

            return false;
        }

        /**
            \fn         operator =
            \since      0.4.0-0
            \brief
                Copies the data from the specified
                vector instance.
        */
        SelfType&         operator = ( const SelfType& other ) {

            Assign( other.Begin(), other.End() );

            return *this;

        }

        /**
            \fn         GetLastProperty
            \since      0.4.0-0
            \brief
                Returns the last element as
                property.
        */
        virtual spp::Property      GetLastProperty() {

            return spp::MakeIndexProperty<SelfType, int, SelfType, ValueType>(
                       "",
                       this,
                       ( int )( Size() - 1 )
                   );

        }

        /**
            \fn         IsLastPropertySerializable
            \since      0.4.0-0
            \brief
                Returns true, if the current assigned type is
                a spp::Serializable/spp::AutoSerializable implementation.
        */
        virtual bool        IsLastPropertySerializable() {

            if( Size() > 0 ) {

                try {

                    ( void )( dynamic_cast< spp::Serializable* >( ( spp::Serializable* )&Back() ) != 0 );

                } catch( ... ) {
                    return false;
                }

                return true;
            }

            return false;

        }

        /**
            \fn         CreateInstance
            \since      0.4.0-0
            \brief
                Creates a new instance of the internal
                property type.
        */
        virtual void*       CreateInstance() {

            PushBack( ValueType() );

            return ( void* )&Back();

        }

        /**
            \fn         Insert
            \since      0.4.0-0
            \brief
                Inserts a new element at the
                specified location.
        */
        Iterator        Insert( Iterator pos, ConstReference value ) {

            return m_Elements.insert( pos, value );

        }

        /**
            \fn         Insert
            \since      0.4.0-0
            \brief
                Inserts a new range of values at the
                specified location.
        */
        template < class InputIterator >
        Iterator        Insert( Iterator pos, InputIterator begin, InputIterator end ) {

            return m_Elements.insert( pos, begin, end );

        }

        /**
            \fn         Insert
            \since      0.4.0-0
            \brief
                Inserts n new elements at the specified
                location.
        */
        Iterator        Insert( Iterator pos, size_t len, ConstReference val = ValueType() ) {

            return m_Elements.insert( pos, len, val );

        }


        /**
            \fn         GetProperties
            \since      0.4.0-0
            \brief
                Returns a list of serializable
                properties.
        */
        virtual PropertyCollection GetProperties() {

            PropertyCollection collection( "Vector", this );

            int         counter( 0 );
            std::string element( "element" );

            for( auto it = Begin(); it != End(); ++it ) {

                std::stringstream ss;

                ss << element;
                ss << " #";
                ss << counter;
                /// template < class _t_container, class _t_index, class _t_owner_type, class _t_value_type >


                auto prop = spp::MakeIndexProperty<VectorType, int, SelfType, ValueType >(
                                ss.str(),
                                &m_Elements,
                                ( const int& )counter
                            );

                collection.Add( prop );

                ++counter;
            }

            return collection;

        }

        /**
            \fn         AddProperty
            \since      0.4.0-0
            \brief
                Adds a new element to the current container
                from the specified property element.
        */
        virtual bool        AddProperty( spp::Property& property ) {

            if( property.GetTypeInfo() == GetElementTypeInfo() ) {

                ValueType val = property.GetGetter().Get<ValueType>();

                Append( val );

                return true;

            }

            return false;

        }

        virtual bool        AddProperty( const std::string& name, const spp::PropertyTypeInfo& typeInfo, void* data ) {

            ( void )name;

            if( GetElementTypeInfo() == typeInfo ) {

                if( data != 0 ) {
                    PushBack( *( ( ValueType* )data ) );
                } else {
                    PushBack( ValueType() );
                }

                return true;

            }

            return false;

        }

        /**
            \fn         RemoveProperty
            \since      0.4.0-0
            \brief
                Removes a value, which is equal to the specified
                property from the internal container.
        */
        virtual bool        RemoveProperty( spp::Property& property ) {

            if( property.GetTypeInfo() == GetElementTypeInfo() ) {

                ValueType val = property.GetGetter().Get<ValueType>();

                return Remove( val );

            }

            return false;

        }

        /**
            \fn         ContainsProperty
            \since      0.4.0-0
            \brief
                Returns true, if the current container contains
                the specified property instance.
        */
        virtual bool        ContainsProperty( spp::Property& property ) {

            if( property.GetTypeInfo() == GetElementTypeInfo() ) {

                ValueType val = property.GetGetter().Get<ValueType>();

                return ( Find( val ) != End() );

            }

            return false;



        }

        /// The Vector class does not support name-based properties.
        virtual bool        ContainsProperty( const std::string& name ) {

            ( void )name;

            return false;

        }

        /**
            \fn         Size
            \since      0.4.0-0
            \brief
                Returns the size of the current
                vector.
        */
        size_t          Size() const {

            return m_Elements.size();

        }

        /**
            \fn         Capacity
            \since      0.4.0-0
            \brief
                Returns the capacity of the current
                vector.
        */
        size_t          Capacity() const {

            return m_Elements.capacity();

        }

        /**
            \fn         Begin
            \since      0.4.0-0
            \brief
                Returns the first iterator of the current
                vector.
        */
        Iterator        Begin() {

            return m_Elements.begin();

        }

        ConstIterator   Begin() const {

            return m_Elements.cbegin();

        }

        ConstIterator   ConstBegin() const {

            return m_Elements.cbegin();

        }

        /**
            \fn         End
            \since      0.4.0-0
            \brief
                Returns the last iterator of the current
                vector.
        */
        Iterator        End() {

            return m_Elements.end();

        }

        ConstIterator   End() const {

            return m_Elements.cend();

        }

        ConstIterator   ConstEnd() const {

            return m_Elements.cend();

        }

        /**
            \fn         Erase
            \since      0.4.0-0
            \brief
                Removes a specific element
                from the current internal
                list.
        */
        Iterator        Erase( Iterator pos ) {

            return m_Elements.erase( pos );

        }

        Iterator        Erase( Iterator begin, Iterator end ) {

            return m_Elements.erase( begin, end );

        }

        /**
            \fn         Clear
            \since      0.4.0-0
            \brief
                Removes all elements from the current
                internal list.
        */
        void            Clear() {

            m_Elements.clear();

        }

        /**
            \fn         Reserve
            \since      0.4.0-0
            \brief
                Reserves a specific number of
                elements.
        */
        void            Reserve( size_t num ) {

            m_Elements.reserve( num );

        }

        /**
            \fn         Resize
            \since      0.4.0-0
            \brief
                Resizes the current
                vector.
        */
        void            Resize( size_t length, const ValueType& val = ValueType() ) {

            m_Elements.resize( length, val );

        }

        /**
            \fn         Data
            \since      0.4.0-0
            \brief
                Returns the raw
                data.
        */
        Pointer         Data() {

            return m_Elements.data();

        }

        ConstPointer    Data() const {

            return ( ConstPointer )m_Elements.data();

        }

        ConstPointer    ConstData() const {

            return ( ConstPointer )m_Elements.data();

        }

        /**
            \fn         Front
            \since      0.4.0-0
            \brief
                Returns a reference to the
                first element of this
                vector.
        */
        Reference       Front() {

            return m_Elements.front();

        }

        /**
            \fn         Back
            \since      0.4.0-0
            \brief
                Returns a reference to the
                last element of this
                vector.
        */
        Reference       Back() {

            return m_Elements.back();

        }

        /**
            \fn         Empty
            \since      0.4.0-0
            \brief
                Returns true, if the current
                vector type is empty.
        */
        bool            Empty() const {

            return m_Elements.empty();

        }

        /**
            \fn         Shrink
            \since      0.4.0-0
            \brief
                Removes all unused
                elements.
        */
        void            Shrink() {

            m_Elements.shrink_to_fit();

        }


        /**
            \fn         At
            \since      0.4.0-0
            \brief
                Accesses an element at the
                specified position.
        */
        Reference       At( size_t pos ) {

            return m_Elements.at( pos );

        }

        ConstReference  At( size_t pos ) const {

            return m_Elements.at( pos );

        }

        Reference       operator []( size_t pos ) {

            return m_Elements.at( pos );

        }

        ConstReference  operator []( size_t pos ) const {

            return m_Elements.at( pos );

        }

        /**
            \fn         Swap
            \since      0.4.0-0
            \brief
                Swaps the contents of two
                containers.
        */
        void            Swap( SelfType&& rhs ) {

            m_Elements.swap( rhs.m_Elements );

        }

        /**
            \fn         PushFront
            \since      0.4.0-0
            \brief
                Appends a new element at
                the front.
        */
        void            PushFront( ConstReference ref ) {

            m_Elements.insert( m_Elements.begin(), ref );

        }

        /**
            \fn         PushBack
            \since      0.4.0-0
            \brief
                Appends a new element at
                the end.
        */
        void            PushBack( ConstReference ref ) {

            m_Elements.push_back( ref );

        }

        /**
            \fn         Append
            \since      0.4.0-0
            \brief
                Appends a new element at
                the end.

                Equal to PushBack
        */
        void            Append( ConstReference ref ) {

            m_Elements.push_back( ref );

        }

        /**
            \fn         PopBack
            \since      0.4.0-0
            \brief
                Removes the last element of the
                current container and returns
                the removed value.
        */
        ConstReference  PopBack() {

            ValueType elem = Back();

            m_Elements.erase( Begin() + ( Size() - 1 ) );

            return elem;

        }

        /**
            \fn         PopFront
            \since      0.4.0-0
            \brief
                Removes the front element of the
                current container and returns
                the removed value.
        */
        ConstReference  PopFront() {

            ValueType elem = Front();

            m_Elements.erase( Begin() );

            return elem;

        }

        /**
            \fn         Assign
            \since      0.4.0-0
            \brief

                The same as Fill().

                Sets every element of the internal container
                to the specified value.
        */
        void            Assign( size_t length, ConstReference value ) {

            m_Elements.assign( length, value );

        }

        void            Fill( size_t length, ConstReference value ) {

            m_Elements.assign( length, value );

        }

        /**
            \fn         Assign
            \since      0.4.0-0
            \brief
                Assigns a new range of values
                to the current state.
        */
        template < class InputIterator >
        void            Assign( InputIterator begin, InputIterator end ) {

            m_Elements.assign( begin, end );

        }

        /**
            \fn         Assign
            \since      0.4.0-0
            \brief
                Assigns new data from the specified
                raw buffer.
        */
        void            Assign( Pointer elements, size_t length ) {

            m_Elements.clear();

            EnsureCapacity( length );

            for( size_t i = 0; length > i; ++i ) {
                m_Elements.push_back( elements[ i ] );
            }

        }

        /**
            \fn         EnsureCapacity
            \since      0.4.0-0
            \brief
                Ensures that the current container
                is able to contain the specified
                amount of elements.
        */
        void            EnsureCapacity( size_t len ) {

            if( Capacity() < len ) {

                Reserve( len - Capacity() );

            }

        }

        /**
            \fn         Remove
            \since      0.4.0-0
            \brief
                Removes the specified element
                from the internal vector.
        */
        bool            Remove( ConstReference ref ) {

            return ( Erase( Find( ref ) ) != End() );

        }

        template < class _t_filter >
        bool            Remove( _t_filter filter ) {

            for( auto it = Begin(); it != End(); ++it ) {

                if( filter( *it ) ) {

                    Erase( it );

                    return true;

                }

            }

            return false;

        }


        /**
            \fn         RemoveAll
            \since      0.4.0-0
            \brief
                Removes all elements, which are equal
                to the parameter.

                Returns the number of removed elements.
        */
        size_t          RemoveAll( ConstReference ref ) {

            size_t count( 0 );

            for( auto it = Begin(); it != End(); ++it ) {

                if( ( *it ) == ref ) {

                    it = Erase( it );

                    ++count;

                }

            }

            return count;

        }

        template < class _t_filter >
        size_t          RemoveAll( _t_filter filter ) {
            size_t count( 0 );

            for( auto it = Begin(); it != End(); ++it ) {

                if( filter( *it ) ) {

                    it = Erase( it );

                    ++count;

                }

            }

            return count;

        }

        /**
            \fn         Find
            \since      0.4.0-0
            \brief
                Searches for the specified
                element. Returns a valid iterator, if the
                element was found.
        */
        Iterator        Find( ConstReference ref ) {

            return std::find( Begin(), End(), ref );

        }

        ConstIterator   Find( ConstReference ref ) const {

            return std::find( Begin(), End(), ref );

        }

        template < class _t_predicate >
        Iterator        Find( _t_predicate predicate, ConstReference ref ) {

            for( auto it = m_Elements.begin(); it != m_Elements.end(); ++it ) {

                if( predicate( *it, ref ) ) {

                    return it;

                }

            }

            return End();

        }

        template < class _t_predicate >
        ConstIterator   Find( _t_predicate predicate, ConstReference ref ) const {

            for( auto it = m_Elements.begin(); it != m_Elements.end(); ++it ) {

                if( predicate( *it, ref ) ) {

                    return it;

                }

            }

            return End();

        }

        template < class _t_predicate >
        Iterator        Find( _t_predicate predicate ) {

            for( auto it = m_Elements.begin(); it != m_Elements.end(); ++it ) {

                if( predicate( *it ) ) {

                    return it;

                }

            }

            return End();

        }

        template < class _t_predicate >
        ConstIterator   Find( _t_predicate predicate ) const {

            for( auto it = m_Elements.begin(); it != m_Elements.end(); ++it ) {

                if( predicate( *it ) ) {

                    return it;

                }

            }

            return End();

        }

        /**
            \fn         Select
            \since      0.4.0-0
            \brief
                Creates a new collection from the current one, filled with
                elements, which match the given predicate.
        */
        template < class _t_predicate >
        SelfType        Select( _t_predicate predicate ) {

            SelfType vec;

            for( auto it = Begin(); it != End(); ++it ) {

                if( predicate( *it ) ) {

                    vec.PushBack( *it );

                }

            }

            return vec;

        }

        /**
            \fn         LogOr
            \since      0.4.0-0
            \brief
                Executes a logical or set operation
                on the current container.
        */
        SelfType            LogOr( SelfType& other ) {

            SelfType r( other.Begin(), other.End() );

            for( auto it = Begin(); it != End(); ++it ) {
                r.PushBack( *it );
            }

            return r;

        }

        /**
            \fn         LogAnd
            \since      0.4.0-0
            \brief
                Executes a logical and set operation
                on the current container.
        */
        SelfType            LogAnd( SelfType& other ) {
            auto pred = [&]( ConstReference ref ) -> bool {

                return ( other.Find( ref ) != other.End() );

            };

            return Select(
                       pred
                   );
        }

        /**
            \fn         LogXor
            \since      0.4.0-0
            \brief
                Executes a logical xor set operation
                on the current container.
        */
        SelfType            LogXor( SelfType& other ) {
            auto c1 = LogNot( other );
            auto c2 = other.LogNot( *this );

            return c1.LogOr( c2 );
        }

        /**
            \fn         LogNot
            \since      0.4.0-0
            \brief
                Executes a logical not set operation
                on the current container.
        */
        SelfType            LogNot( SelfType& other ) {
            auto pred = [&]( ConstReference ref ) -> bool {

                return ( other.Find( ref ) == other.End() );

            };

            return Select(
                       pred
                   );
        }

        /**
            \fn         Each
            \since      0.4.0-0
            \brief
                Iterates over each element.
        */
        template < class _t_fun >
        void            Each( _t_fun fn ) {

            for( auto it = Begin(); it != End(); ++it ) {

                fn( *it );

            }

        }

        template  < class _t_predicate, class _t_fun >
        void            Each( _t_predicate pred, _t_fun fun ) {

            for( auto it = Begin(); it != End(); ++it ) {

                if( pred( *it ) ) {

                    fn( *it );

                }

            }

        }
    protected:
        VectorType          m_Elements;
};

template < class _t_value_type >
inline bool    operator == ( const Vector< _t_value_type >& r1, const Vector< _t_value_type >& r2 ) {
    return ( r1.ConstData() == r2.ConstData() );
}

/**
    Default collection of raw buffer
    classes.
*/
typedef Vector< char >              ArrayBuffer8;
typedef Vector< short >             ArrayBuffer16;
typedef Vector< int >               ArrayBuffer32;
typedef Vector< long long >         ArrayBuffer64;

/**
    \class      List
    \since      0.4.0-0
    \brief
        A serializable list class
        wrapper.
*/
template < class _t_value >
class       List : public PropertyContainer {
    public:

    protected:
};

/**
    \class      Map
    \since      0.4.0-0
    \brief
        A serializable map class
        wrapper.
*/
template < class _t_index, class _t_value >
class       Map : public PropertyContainer {
    public:

    protected:
};

/**
    \class      Stack
    \since      0.4.0-0
    \brief
        A serializable stack class
        wrapper.
*/
template < class _t_value >
class       Stack : public PropertyContainer {
    public:

    protected:
};



///  template < class _t_owner_type, class _t_value_type >
template < class _t_owner_type >
spp::Property    MakeCollectionProperty( _t_owner_type* ptr, const std::string& name, PropertyContainer* serializable ) {
    ( void )ptr;

    return Property(
               PropertyDesc( name ),
               spp::GetTypeInfo< PropertyContainer* >(),
               spp::PropertyGetter(
                   spp::GetTypeInfo< _t_owner_type >(),
                   spp::GetTypeInfo< PropertyContainer* >(),
                   ( spp::AbstractPropertyGetter* )( new spp::PointerPropertyGetter<_t_owner_type, PropertyContainer* >(
                               serializable
                           ) )
               ),
               spp::PropertySetter(
                   spp::GetTypeInfo< _t_owner_type >(),
                   spp::GetTypeInfo< PropertyContainer* >(),
                   ( spp::AbstractPropertySetter* )( new spp::PointerPropertySetter<_t_owner_type, PropertyContainer* >(
                               serializable
                           ) )
               )
           );

}

}
