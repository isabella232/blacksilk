/**
    Copyright 2013 by FD Imaging

    http://fd-imaging.com

    All rights reserved.
*/
#pragma once

#include <exception>
#include <stdexcept>
#include <memory>

#include <libserialization++/TypeInfo.hpp>

namespace spp {

/**
    \class      PropertyObjectHandle
    \since      0.4.0-0
    \brief
        The AbstractPropertyObjectHandle base class allows
        for type-indiependant but rtti-based object
        binding.
*/
class  PropertyObjectHandle {
    public:
        /**
            \fn     PropertyObjectHandle
            \since  0.4.0-0
            \brief
                Constructs a new empty AbstractPropertyOwnerHandle
                with the specified type info.
        */
        PropertyObjectHandle( const spp::PropertyTypeInfo& info, const void* bindingPtr = 0 );

        /**
            \fn     ~PropertyObjectHandle
            \since  0.4.0-0
            \brief
                Resets the internal state and deletes
                all members.
        */
        virtual ~PropertyObjectHandle();

        /**
            \fn     GetBindingPtr
            \since  0.4.0-0
            \brief
                Returns the currently bound object
                pointer.
        */
        const void* GetBindingPtr() const;

        /**
            \fn     GetTypeInfo
            \since  0.4.0-0
            \brief
                Returns the current type info for the
                selected object type.
        */
        const spp::PropertyTypeInfo& GetTypeInfo() const;

        /**
            \fn     Assign
            \since  0.4.0-0
            \brief
                Assigns a new BindingPtr to the internal
                variable.

                This may throws the TypeMismatchError exception.
        */
        template < class _t_owner_type >
        void Assign( _t_owner_type* ptr ) {
            this->Assign( spp::GetTypeInfo<_t_owner_type>(), ( const void* )ptr );
        }

        void Assign( const spp::PropertyTypeInfo& info, const void* bindingPtr );
    protected:
        void*                       m_BindingPtr;
        spp::PropertyTypeInfo       m_BindingType;
};

/**
    \class      PropertyHandleBindable
    \since      0.4.0-0
    \brief
        The PropertyHandleBindable is the base-class for
        all PropertyObjectHandle bindable classes.
*/
class PropertyHandleBindable {
    public:
        /**
            \fn     PropertyHandleBindable
            \since  0.4.0-0
            \brief
                Constructs a new empty PropertyHandleBindable
                instance.
        */
        PropertyHandleBindable() {}

        /**
            \fn     ~PropertyHandleBindable
            \since  0.4.0-0
            \brief
                Frees the internal data and deletes
                the current state.
        */
        virtual ~PropertyHandleBindable() {}

        /**
            \fn         Attach
            \since      0.4.0-0
            \brief
                Attaches a PropertyObjectHandle instance to the
                current state.
        */
        virtual bool Attach( PropertyObjectHandle& handle ) = 0;
};

/**
    \class      BasePropertyBindable
    \since      0.4.0-0
    \brief
        A BasePropertyBindable is the base class for all
        different kinds of property getter and setter objects,
        which need an explicit binding to a specified
        owner object.
*/
template < class _t_owner_type >
class BasePropertyBindable : public PropertyHandleBindable {
    public:
        typedef     _t_owner_type               OwnerType;
        typedef     OwnerType*                  OwnerTypePtr;
        typedef     OwnerType&                  OwnerTypeRef;

        /**
            \fn     BasePropertyBindable
            \since  0.4.0-0
            \brief
                Constructs a new empty BasePropertyBindable instance.
        */
        BasePropertyBindable() : m_Binding( 0 ), m_OwnerTypeInfo( spp::GetTypeInfo< OwnerType >() ) {}

        /**
            \fn     ~BasePropertyBindable
            \since  0.4.0-0
            \brief
                Frees the internal data and resets the complete
                state.
        */
        virtual ~BasePropertyBindable() {

            m_Binding   = 0;

        }

        /**
            \fn     Attach
            \since  0.4.0-0
            \brief
                Attaches a new binding to the current
                bindable object.
        */
        virtual bool Attach( OwnerTypePtr p ) {

            if( p != 0 ) {

                m_Binding = p;

                return true;

            }

            return false;

        }

        virtual bool Attach( PropertyObjectHandle& handle ) {

            if( handle.GetTypeInfo().Equal( m_OwnerTypeInfo ) ) {

                return this->Attach( ( OwnerTypePtr )handle.GetBindingPtr() );

            }

            return false;
        }
    protected:

        /**
            \fn     SetBinding
            \since  0.4.0-0
            \brief
                    Sets the internal binding object
                    pointer.
        */
        void        SetBinding( OwnerTypePtr p ) {
            m_Binding = p;
        }

        /**
            \fn     GetBinding
            \since  0.4.0-0
            \brief
                    Returns the internal binding
                    object pointer.
        */
        OwnerTypePtr    GetBinding() const {
            return m_Binding;
        }

        OwnerTypePtr               m_Binding;
        spp::PropertyTypeInfo      m_OwnerTypeInfo;
};

/**
    \class      AbstractPropertySetter
    \since      0.4.0-0
    \brief
        RTTI-based property Setter base-class.
*/
class AbstractPropertySetter  {
    public:
        /**
            \fn     ~AbstractPropertySetter
            \since  0.4.0-0
            \brief
                Abstract destructor.
        */
        virtual ~AbstractPropertySetter() {}

        /**
            \fn     Bind
            \since  0.4.0-0
            \brief
                Binds an object-handle to this
                setter.
        */
        virtual void    Bind( PropertyObjectHandle& handle ) = 0;

        /**
            \fn     operator ()
            \since  0.4.0-0
            \brief
                Accesses the internal assigned
                property value.
        */
        virtual bool operator()( const spp::PropertyTypeInfo& typeInfo, void* value ) = 0;

        /**
            \fn     operator ()
            \since  0.4.0-0
            \brief
                Accesses the internal assigned
                property value.
        */
        template < class _t_value_type >
        bool operator()( _t_value_type* value ) {
            return this->operator()( spp::GetTypeInfo<_t_value_type>(), static_cast<void*>( value ) );
        }
};

/**
    \class      PropertyIndexSetter
    \since      0.4.0-0
    \brief
        Acccesses the specific property value
        with a specified index value.
*/
template < class _t_container, class _t_index, class _t_owner_type, class _t_value_type >
class   PropertyIndexSetter : public BasePropertyBindable< _t_owner_type >, public AbstractPropertySetter {
    public:
        spp::PropertyTypeInfo      OwnerTypeInfo;
        spp::PropertyTypeInfo      TypeInfo;

        typedef     _t_value_type           ValueType;
        typedef     ValueType*              Pointer;
        typedef     const ValueType*        ConstPointer;
        typedef     ValueType&              Reference;
        typedef     const ValueType&        ConstReference;

        typedef     _t_container            ContainerType;
        typedef     _t_index                IndexType;

        //typedef     ContainerType(_t_owner_type::*MemberPtr);
        typedef       ContainerType*        MemberPtr;
        typedef     _t_owner_type           OwnerType;


        /**
            \fn     PropertyIndexSetter
            \since  0.4.0-0
            \brief
                Constructs a new empty PropertyIndexSetter
                instance.
        */
        PropertyIndexSetter( MemberPtr ptr, IndexType index ) : BasePropertyBindable< _t_owner_type >(),
            OwnerTypeInfo( GetTypeInfo<_t_owner_type>() ), TypeInfo( GetTypeInfo<_t_value_type>() ), m_Member( ptr ), m_Index( index ) {}

        /**
            \fn     ~ValuePropertyGetter
            \since  0.4.0-0
            \brief
                Base destructor.
        */
        virtual ~PropertyIndexSetter() {}

        /**
            \fn     Bind
            \since  0.4.0-0
            \brief
                Binds an object-handle to this
                setter.
        */
        virtual void    Bind( PropertyObjectHandle& handle ) {

            this->Attach( handle );

        }

        /**
            \fn     operator ()
            \since  0.4.0-0
            \brief
                Accesses the internal assigned
                property value.
        */
        virtual bool operator()( const spp::PropertyTypeInfo& typeInfo, void* value ) {
            if( value == 0 ) {
                return false;
            }


            if( TypeInfo == typeInfo ) {

                ( *m_Member )[m_Index] = *( ( typename OwnerType::Pointer )value );

                return true;

            }

            return false;

        }

        virtual bool operator()( _t_owner_type* ownerObject, spp::PropertyTypeInfo& typeInfo, void* value ) {
            if( value == 0 ) {
                return false;
            }

            ( void )ownerObject;

            if( TypeInfo == typeInfo ) {

                ( *m_Member )[m_Index] = *( ( typename OwnerType::Pointer )value );

                return true;

            }

            return false;

        }

    protected:
        MemberPtr       m_Member;
        IndexType       m_Index;
};


/**
    \class      PointerPropertySetter
    \since      0.4.0-0
    \brief
        Acccesses the specific property value
        directly.
*/
template < class _t_owner_type, class _t_value_type >
class   PointerPropertySetter : public BasePropertyBindable< _t_owner_type >, public AbstractPropertySetter {
    public:
        spp::PropertyTypeInfo      OwnerTypeInfo;
        spp::PropertyTypeInfo      TypeInfo;

        typedef     _t_value_type           ValueType;
        typedef     ValueType*              Pointer;
        typedef     const ValueType*        ConstPointer;
        typedef     ValueType&              Reference;
        typedef     const ValueType&        ConstReference;

        typedef     ValueType MemberPtr;

        /**
            \fn     ValuePropertySetter
            \since  0.4.0-0
            \brief
                Constructs a new empty ValuePropertySetter
                instance.
        */
        PointerPropertySetter( MemberPtr ptr ) : BasePropertyBindable< _t_owner_type >(), OwnerTypeInfo( GetTypeInfo<_t_owner_type>() ), TypeInfo( GetTypeInfo<_t_value_type>() ), m_Member( ptr ) {}

        /**
            \fn     ~ValuePropertyGetter
            \since  0.4.0-0
            \brief
                Base destructor.
        */
        virtual ~PointerPropertySetter() {}

        /**
            \fn     Bind
            \since  0.4.0-0
            \brief
                Binds an object-handle to this
                setter.
        */
        virtual void    Bind( PropertyObjectHandle& handle ) {

            this->Attach( handle );

        }

        /**
            \fn     operator ()
            \since  0.4.0-0
            \brief
                Accesses the internal assigned
                property value.
        */
        virtual bool operator()( const spp::PropertyTypeInfo& typeInfo, void* value ) {
            if( value == 0 ) {
                return false;
            }

            ( void )typeInfo;
            ( void )value;

            return false;

        }

        virtual bool operator()( _t_owner_type* ownerObject, spp::PropertyTypeInfo& typeInfo, void* value ) {
            if( value == 0 ) {
                return false;
            }


            ( void )value;
            ( void )ownerObject;
            ( void )typeInfo;

            return false;

        }

    protected:
        MemberPtr       m_Member;
};


/**
    \class      ValuePropertySetter
    \since      0.4.0-0
    \brief
        Acccesses the specific property value
        directly.
*/
template < class _t_owner_type, class _t_value_type >
class   ValuePropertySetter : public BasePropertyBindable< _t_owner_type >, public AbstractPropertySetter {
    public:
        spp::PropertyTypeInfo      OwnerTypeInfo;
        spp::PropertyTypeInfo      TypeInfo;

        typedef     _t_value_type           ValueType;
        typedef     ValueType*              Pointer;
        typedef     const ValueType*        ConstPointer;
        typedef     ValueType&              Reference;
        typedef     const ValueType&        ConstReference;

        typedef     _t_value_type( _t_owner_type::*MemberPtr );

        /**
            \fn     ValuePropertySetter
            \since  0.4.0-0
            \brief
                Constructs a new empty ValuePropertySetter
                instance.
        */
        ValuePropertySetter( MemberPtr ptr ) : BasePropertyBindable< _t_owner_type >(), m_Member( ptr ), OwnerTypeInfo( GetTypeInfo<_t_owner_type>() ), TypeInfo( GetTypeInfo<_t_value_type>() ) {}

        /**
            \fn     ~ValuePropertyGetter
            \since  0.4.0-0
            \brief
                Base destructor.
        */
        virtual ~ValuePropertySetter() {}

        /**
            \fn     Bind
            \since  0.4.0-0
            \brief
                Binds an object-handle to this
                setter.
        */
        virtual void    Bind( PropertyObjectHandle& handle ) {

            this->Attach( handle );

        }

        /**
            \fn     operator ()
            \since  0.4.0-0
            \brief
                Accesses the internal assigned
                property value.
        */
        virtual bool operator()( const spp::PropertyTypeInfo& typeInfo, void* value ) {
            if( value == 0 ) {
                return false;
            }


            if( TypeInfo == typeInfo ) {

                BasePropertyBindable<_t_owner_type>::GetBinding()->*m_Member = *( ( Pointer )value );

                return true;

            }

            return false;

        }

        virtual bool operator()( _t_owner_type* ownerObject, spp::PropertyTypeInfo& typeInfo, void* value ) {
            if( value == 0 ) {
                return false;
            }


            if( TypeInfo == typeInfo ) {

                ownerObject->*m_Member = *( ( Pointer )value );

                return true;

            }

            return false;

        }

    protected:
        MemberPtr       m_Member;
};


/**
    \class      FunctionalPropertyGetter
    \since      0.4.0-0
    \brief
        Accesses a specific property value using a specified
        getter method.
*/
template < class _t_owner_type, class _t_value_type >
class   FunctionalPropertySetter : public BasePropertyBindable< _t_owner_type >, public AbstractPropertySetter {
    public:

        spp::PropertyTypeInfo      OwnerTypeInfo;
        spp::PropertyTypeInfo      TypeInfo;

        typedef     _t_value_type           ValueType;
        typedef     ValueType*              Pointer;
        typedef     const ValueType*        ConstPointer;
        typedef     ValueType&              Reference;
        typedef     const ValueType&        ConstReference;

        typedef     void( _t_owner_type::*SetterPtr )( _t_value_type );

        /**
            \fn     FunctionalPropertySetter
            \since  0.4.0-0
            \brief
                Constructs a new empty FunctionalPropertyGetter
                instance.
        */
        FunctionalPropertySetter( SetterPtr ptr ) : BasePropertyBindable< _t_owner_type >(), m_Setter( ptr ), OwnerTypeInfo( GetTypeInfo<_t_owner_type>() ), TypeInfo( GetTypeInfo<_t_value_type>() ) {}

        /**
            \fn     ~FunctionalPropertySetter
            \since  0.4.0-0
            \brief
                Base destructor.
        */
        virtual ~FunctionalPropertySetter() {}

        /**
            \fn     Bind
            \since  0.4.0-0
            \brief
                Binds an object-handle to this
                setter.
        */
        virtual void    Bind( PropertyObjectHandle& handle ) {

            this->Attach( handle );

        }

        /**
            \fn     operator ()
            \since  0.4.0-0
            \brief
                Accesses the internal assigned
                property value.
        */
        virtual bool operator()( const spp::PropertyTypeInfo& typeInfo, void* value ) {
            if( value == 0 ) {
                return false;
            }


            if( TypeInfo == typeInfo ) {

                ( BasePropertyBindable<_t_owner_type>::GetBinding()->*m_Setter )( *( Pointer )value );

                return true;

            }

            return false;

        }

        virtual bool operator()( _t_owner_type* ownerObject, spp::PropertyTypeInfo& typeInfo, void* value ) {
            if( value == 0 ) {
                return false;
            }


            if( TypeInfo == typeInfo ) {

                ( ownerObject->*m_Setter )( *( Pointer )value );

                return true;

            }

            return false;

        }
    protected:
        SetterPtr       m_Setter;
};


/**
    \class      AbstractPropertyGetter
    \since      0.4.0-0
    \brief
        RTTI-based property getter base-class.
*/
class AbstractPropertyGetter {
    public:

        AbstractPropertyGetter() {}
        virtual ~AbstractPropertyGetter() {}

        /**
            \fn     Bind
            \since  0.4.0-0
            \brief
                Binds an object-handle to this
                setter.
        */
        virtual void    Bind( PropertyObjectHandle& handle ) = 0;

        /**
            \fn     operator ()
            \since  0.4.0-0
            \brief
                Accesses the internal assigned
                property value.
        */
        virtual bool operator()( const spp::PropertyTypeInfo& typeInfo, void* value ) = 0;

        /**
            \fn     operator ()
            \since  0.4.0-0
            \brief
                Accesses the internal assigned
                property value.
        */
        template < class _t_value_type >
        bool operator()( _t_value_type* value ) {
            return this->operator()( spp::GetTypeInfo<_t_value_type>(), static_cast<void*>( value ) );
        }
};

/**
    \class      PointerPropertyGetter
    \since      0.4.0-0
    \brief
        Acccesses the specific property value
        directly.
*/
template < class _t_owner_type, class _t_value_type >
class   PointerPropertyGetter : public BasePropertyBindable< _t_owner_type >, public AbstractPropertyGetter {
    public:
        spp::PropertyTypeInfo      OwnerTypeInfo;
        spp::PropertyTypeInfo      TypeInfo;

        typedef     _t_value_type           ValueType;
        typedef     ValueType*              Pointer;
        typedef     const ValueType*        ConstPointer;
        typedef     ValueType&              Reference;
        typedef     const ValueType&        ConstReference;

        typedef     ValueType               MemberPtr;

        /**
            \fn     ValuePropertyGetter
            \since  0.4.0-0
            \brief
                Constructs a new empty ValuePropertyGetter
                instance.
        */
        PointerPropertyGetter( MemberPtr ptr ) : BasePropertyBindable< _t_owner_type >(), OwnerTypeInfo( GetTypeInfo<_t_owner_type>() ), TypeInfo( GetTypeInfo<_t_value_type>() ), m_Member( ptr ) {}

        /**
            \fn     ~ValuePropertyGetter
            \since  0.4.0-0
            \brief
                Base destructor.
        */
        ~PointerPropertyGetter() {}

        /**
            \fn     Bind
            \since  0.4.0-0
            \brief
                Binds an object-handle to this
                setter.
        */
        virtual void    Bind( PropertyObjectHandle& handle ) {

            this->Attach( handle );

        }

        /**
            \fn     operator ()
            \since  0.4.0-0
            \brief
                Accesses the internal assigned
                property value.
        */
        virtual bool operator()( const spp::PropertyTypeInfo& typeInfo, void* value ) {
            if( value == 0 ) {
                return false;
            }


            if( TypeInfo == typeInfo ) {

                *( ( Pointer )value ) = m_Member;

                return true;

            }

            return false;

        }

        virtual bool operator()( _t_owner_type* ownerObject, spp::PropertyTypeInfo& typeInfo, void* value ) {
            if( value == 0 ) {
                return false;
            }

            ( void )ownerObject;


            if( TypeInfo == typeInfo ) {

                *( ( Pointer )value ) = m_Member;

                return true;

            }

            return false;

        }

    protected:
        MemberPtr       m_Member;
};



/**
    \class      ValuePropertyGetter
    \since      0.4.0-0
    \brief
        Acccesses the specific property value
        directly.
*/
template < class _t_owner_type, class _t_value_type >
class   ValuePropertyGetter : public BasePropertyBindable< _t_owner_type >, public AbstractPropertyGetter {
    public:
        spp::PropertyTypeInfo      OwnerTypeInfo;
        spp::PropertyTypeInfo      TypeInfo;

        typedef     _t_value_type           ValueType;
        typedef     ValueType*              Pointer;
        typedef     const ValueType*        ConstPointer;
        typedef     ValueType&              Reference;
        typedef     const ValueType&        ConstReference;

        typedef     _t_value_type( _t_owner_type::*MemberPtr );

        /**
            \fn     ValuePropertyGetter
            \since  0.4.0-0
            \brief
                Constructs a new empty ValuePropertyGetter
                instance.
        */
        ValuePropertyGetter( MemberPtr ptr ) : BasePropertyBindable< _t_owner_type >(), m_Member( ptr ), OwnerTypeInfo( GetTypeInfo<_t_owner_type>() ), TypeInfo( GetTypeInfo<_t_value_type>() ) {}

        /**
            \fn     ~ValuePropertyGetter
            \since  0.4.0-0
            \brief
                Base destructor.
        */
        ~ValuePropertyGetter() {}

        /**
            \fn     Bind
            \since  0.4.0-0
            \brief
                Binds an object-handle to this
                setter.
        */
        virtual void    Bind( PropertyObjectHandle& handle ) {

            this->Attach( handle );

        }

        /**
            \fn     operator ()
            \since  0.4.0-0
            \brief
                Accesses the internal assigned
                property value.
        */
        virtual bool operator()( const spp::PropertyTypeInfo& typeInfo, void* value ) {
            if( value == 0 ) {
                return false;
            }


            if( TypeInfo == typeInfo ) {

                *( ( Pointer* )value ) = &( BasePropertyBindable<_t_owner_type>::GetBinding()->*m_Member );

                return true;

            }

            return false;

        }

        virtual bool operator()( _t_owner_type* ownerObject, spp::PropertyTypeInfo& typeInfo, void* value ) {
            if( value == 0 ) {
                return false;
            }


            if( TypeInfo == typeInfo ) {

                *( ( Pointer* )value ) = &( ownerObject->*m_Member );

                return true;

            }

            return false;

        }

    protected:
        MemberPtr       m_Member;
};

/**
    \class      PropertyIndexGetter
    \since      0.4.0-0
    \brief
        Acccesses the specific property value
        with a specified index value.
*/
template < class _t_container, class _t_index, class _t_owner_type, class _t_value_type >
class   PropertyIndexGetter : public BasePropertyBindable< _t_owner_type >, public AbstractPropertyGetter {
    public:
        spp::PropertyTypeInfo      OwnerTypeInfo;
        spp::PropertyTypeInfo      TypeInfo;

        typedef     _t_value_type           ValueType;
        typedef     ValueType*              Pointer;
        typedef     const ValueType*        ConstPointer;
        typedef     ValueType&              Reference;
        typedef     const ValueType&        ConstReference;

        typedef     _t_container            ContainerType;
        typedef     _t_index                IndexType;

        typedef     ContainerType* MemberPtr;//ContainerType(_t_owner_type::*MemberPtr);


        /**
            \fn     PropertyIndexGetter
            \since  0.4.0-0
            \brief
                Constructs a new empty PropertyIndexGetter
                instance.
        */
        PropertyIndexGetter( MemberPtr ptr, IndexType index ) :
            BasePropertyBindable< _t_owner_type >(),
            OwnerTypeInfo( GetTypeInfo<_t_owner_type>() ),
            TypeInfo( GetTypeInfo<ValueType>() ),
            m_Member( ptr ),
            m_Index( index ) {}

        /**
            \fn     ~PropertyIndexGetter
            \since  0.4.0-0
            \brief
                Base destructor.
        */
        virtual ~PropertyIndexGetter() {}

        /**
            \fn     Bind
            \since  0.4.0-0
            \brief
                Binds an object-handle to this
                setter.
        */
        virtual void    Bind( PropertyObjectHandle& handle ) {

            this->Attach( handle );

        }

        /**
            \fn     operator ()
            \since  0.4.0-0
            \brief
                Accesses the internal assigned
                property value.
        */
        virtual bool operator()( const spp::PropertyTypeInfo& typeInfo, void* value ) {
            if( value == 0 ) {
                return false;
            }

            if( TypeInfo == typeInfo ) {

                *( ( Pointer* )value ) = ( static_cast<Pointer>( &( *m_Member )[m_Index] ) ); //( GetBinding()->*m_Member )[m_Index];

                return true;

            }

            return false;

        }

        virtual bool operator()( _t_owner_type* ownerObject, spp::PropertyTypeInfo& typeInfo, void* value ) {
            if( value == 0 ) {
                return false;
            }

            ( void )ownerObject;

            if( TypeInfo == typeInfo ) {

                *( ( Pointer* )value ) = ( static_cast<Pointer>( &( *m_Member )[m_Index] ) ); //( GetBinding()->*m_Member )[m_Index];

                return true;

            }

            return false;

        }

    protected:
        MemberPtr       m_Member;
        IndexType       m_Index;
};

/**
    \class      FunctionalPropertyGetter
    \since      0.4.0-0
    \brief
        Accesses a specific property value using a specified
        getter method.
*/
template < class _t_owner_type, class _t_value_type >
class   FunctionalPropertyGetter : public BasePropertyBindable< _t_owner_type >, public AbstractPropertyGetter {
    public:

        spp::PropertyTypeInfo      OwnerTypeInfo;
        spp::PropertyTypeInfo      TypeInfo;

        typedef     _t_value_type           ValueType;
        typedef     ValueType*              Pointer;
        typedef     const ValueType*        ConstPointer;
        typedef     ValueType&              Reference;
        typedef     const ValueType&        ConstReference;

        typedef     _t_value_type( _t_owner_type::*GetterPtr )( void )const;

        /**
            \fn     FunctionalPropertyGetter
            \since  0.4.0-0
            \brief
                Constructs a new empty FunctionalPropertyGetter
                instance.
        */
        FunctionalPropertyGetter( GetterPtr ptr ) : BasePropertyBindable< _t_owner_type >(), m_Getter( ptr ), OwnerTypeInfo( GetTypeInfo<_t_owner_type>() ), TypeInfo( GetTypeInfo<_t_value_type>() ) {}

        /**
            \fn     ~FunctionalPropertyGetter
            \since  0.4.0-0
            \brief
                Base destructor.
        */
        ~FunctionalPropertyGetter() {}

        /**
            \fn     Bind
            \since  0.4.0-0
            \brief
                Binds an object-handle to this
                setter.
        */
        virtual void    Bind( PropertyObjectHandle& handle ) {

            this->Attach( handle );

        }

        /**
            \fn     operator ()
            \since  0.4.0-0
            \brief
                Accesses the internal assigned
                property value.
        */
        virtual bool operator()( const spp::PropertyTypeInfo& typeInfo, void* value ) {
            if( value == 0 ) {
                return false;
            }


            if( TypeInfo.Equal( typeInfo ) ) {

                *( ( Pointer )value ) = ( BasePropertyBindable<_t_owner_type>::GetBinding()->*m_Getter )();

                return true;

            }

            return false;

        }

        virtual bool operator()( _t_owner_type* ownerObject, spp::PropertyTypeInfo& typeInfo, void* value ) {
            if( value == 0 ) {
                return false;
            }


            if( TypeInfo.Equal( typeInfo ) ) {

                *( ( Pointer )value ) = ( ownerObject->*m_Getter )();

                return true;

            }

            return false;

        }
    protected:
        GetterPtr       m_Getter;
};

template < class _t_owner_type, class _t_value_type >
spp::ValuePropertySetter< _t_owner_type, _t_value_type >*   MakePropertySetter( _t_value_type( _t_owner_type::*ptr ) ) {
    return new spp::ValuePropertySetter< _t_owner_type, _t_value_type >(
               ptr
           );
}

template < class _t_owner_type, class _t_value_type >
spp::ValuePropertyGetter< _t_owner_type, _t_value_type >*   MakePropertyGetter( _t_value_type( _t_owner_type::*ptr ) ) {
    return new spp::ValuePropertyGetter< _t_owner_type, _t_value_type >(
               ptr
           );
}

template < class _t_owner_type, class _t_value_type >
spp::FunctionalPropertyGetter< _t_owner_type, _t_value_type >*   MakePropertyGetter( _t_value_type( _t_owner_type::*ptr )( void ) ) {
    return new spp::FunctionalPropertyGetter< _t_owner_type, _t_value_type >(
               ptr
           );
}

template < class _t_owner_type, class _t_value_type >
spp::FunctionalPropertySetter< _t_owner_type, _t_value_type >*   MakePropertySetter( void( _t_owner_type::*ptr )( const _t_value_type& ) ) {
    return new spp::FunctionalPropertySetter< _t_owner_type, _t_value_type >(
               ptr
           );
}

template < class _t_owner_type, class _t_container >
spp::PropertyIndexGetter< _t_container, int, _t_owner_type, typename _t_container::value_type > MakePropertyIndexGetter( _t_container( _t_owner_type::*ptr ), int index ) {
    return new spp::PropertyIndexGetter< _t_container, int, _t_owner_type, typename _t_container::value_type  >(
               ptr,
               index
           );
}

template < class _t_owner_type, class _t_container >
spp::PropertyIndexSetter< _t_container, int, _t_owner_type, typename _t_container::value_type > MakePropertyIndexSetter( _t_container( _t_owner_type::*ptr ), int index ) {
    return new spp::PropertyIndexSetter< _t_container, int, _t_owner_type, typename _t_container::value_type  >(
               ptr,
               index
           );
}


/**
    \class      PropertyGetter
    \since      0.4.0-0
    \brief
        The PropertyGetter is the high-level
        property getter accessor.
*/
class PropertyGetter {
    public:
        /**
            \fn     PropertyGetter
            \since  0.4.0-0
            \brief
                Constructs a new empty PropertyGetter
                instance.
        */
        PropertyGetter( const spp::PropertyTypeInfo& ownerTypeInfo, const spp::PropertyTypeInfo& valueTypeInfo );

        /**
            \fn     PropertyGetter
            \since  0.4.0-0
            \brief
                Constructs a new empty PropertyGetter
                instance. Assigns the provided getter
                instance.
        */
        PropertyGetter( const spp::PropertyTypeInfo& ownerTypeInfo, const spp::PropertyTypeInfo& valueTypeInfo, spp::AbstractPropertyGetter* getter );

        /**
            \fn    PropertyGetter
            \since  0.4.0-0
            \brief
                Basic copy constructor
        */
        PropertyGetter( const PropertyGetter& rhs );

        /**
            \fn     ~PropertyGetter
            \since  0.4.0-0
            \brief
                Frees the internal data and resets
                the complete state.
        */
        virtual ~PropertyGetter();

        /**
            \fn     GetOwnerTypeInfo
            \since  0.4.0-0
            \brief
                Returns the owner-type specific
                type information.
        */
        const spp::PropertyTypeInfo&    GetOwnerTypeInfo() const;

        /**
            \fn     GetValueTypeInfo
            \since  0.4.0-0
            \brief
                Returns the value-type specific
                type information.
        */
        const spp::PropertyTypeInfo&    GetValueTypeInfo() const;

        /**
            \fn     GetObjectHandle
            \since  0.4.0-0
            \brief
                Returns the currently assigned
                object handle.
        */
        const spp::PropertyObjectHandle& GetObjectHandle() const;

        /**
            \fn     Bind
            \since  0.4.0-0
            \brief
                Binds this PropertyGetter to a new
                owner type instance.
        */
        bool    Bind( const spp::PropertyTypeInfo& info, void* ptr );

        template < class _t_owner_type >
        bool    Bind( _t_owner_type* ptr ) {

            return this->Bind(
                       spp::GetTypeInfo< _t_owner_type >(),
                       ( void* )ptr
                   );

        }

        /**
            \fn     SetGetter
            \since  0.4.0-0
            \brief
                Sets a new Getter instance. Deletes the
                current one.
        */
        void        SetGetter( spp::AbstractPropertyGetter* getter );

        /**
            \fn     GetGetter
            \since  0.4.0-0
            \brief
                Returns the current Getter instance.
        */
        spp::AbstractPropertyGetter* GetGetter() const;

        /**
            \fn     operator ()
            \since  0.4.0-0
            \brief
                Accesses the current internal getter and
                returns the assigned value.
        */
        bool    operator()( const spp::PropertyTypeInfo& info, void* out );

        /**
            \fn     Get
            \since  0.4.0-0
            \brief
                Tries to query the currently assigned
                getter value.
        */
        template < class _t_value_type >
        _t_value_type   Get() {

            _t_value_type       value;

            this->operator()( spp::GetTypeInfo<_t_value_type>(), ( void* )&value );

            return value;

        }
    protected:
        std::shared_ptr<spp::AbstractPropertyGetter>            m_Getter;
        spp::PropertyTypeInfo                                   m_OwnerTypeInfo;
        spp::PropertyTypeInfo                                   m_ValueTypeInfo;
        spp::PropertyObjectHandle                               m_ObjectHandle;
};

/**
    \class      PropertySetter
    \since      0.4.0-0
    \brief
        The PropertySetter is the high-level
        property setter accessor.
*/
class PropertySetter {
    public:
        /**
            \fn     PropertySetter
            \since  0.4.0-0
            \brief
                Constructs a new empty PropertySetter
                instance.
        */
        PropertySetter( const spp::PropertyTypeInfo& ownerTypeInfo, const spp::PropertyTypeInfo& valueTypeInfo );

        /**
            \fn     PropertySetter
            \since  0.4.0-0
            \brief
                Constructs a new empty PropertySetter
                instance. Assigns the provided setter
                instance.
        */
        PropertySetter( const spp::PropertyTypeInfo& ownerTypeInfo, const spp::PropertyTypeInfo& valueTypeInfo, spp::AbstractPropertySetter* setter );

        /**
            \fn    PropertySetter
            \since  0.4.0-0
            \brief
                Basic copy constructor
        */
        PropertySetter( const PropertySetter& rhs );

        /**
            \fn     ~PropertySetter
            \since  0.4.0-0
            \brief
                Frees the internal data and resets
                the complete state.
        */
        virtual ~PropertySetter();

        /**
            \fn     GetOwnerTypeInfo
            \since  0.4.0-0
            \brief
                Returns the owner-type specific
                type information.
        */
        const spp::PropertyTypeInfo&    GetOwnerTypeInfo() const;

        /**
            \fn     GetValueTypeInfo
            \since  0.4.0-0
            \brief
                Returns the value-type specific
                type information.
        */
        const spp::PropertyTypeInfo&    GetValueTypeInfo() const;

        /**
            \fn     GetObjectHandle
            \since  0.4.0-0
            \brief
                Returns the currently assigned
                object handle.
        */
        const spp::PropertyObjectHandle& GetObjectHandle() const;

        /**
            \fn     Bind
            \since  0.4.0-0
            \brief
                Binds this PropertyGetter to a new
                owner type instance.
        */
        bool    Bind( const spp::PropertyTypeInfo& info, void* ptr );

        template < class _t_owner_type >
        bool    Bind( _t_owner_type* ptr ) {

            return this->Bind(
                       spp::GetTypeInfo< _t_owner_type >(),
                       ( void* )ptr
                   );

        }

        /**
            \fn     SetSetter
            \since  0.4.0-0
            \brief
                Sets a new Setter instance. Deletes the
                current one.
        */
        void        SetSetter( spp::AbstractPropertySetter* setter );

        /**
            \fn     GetSetter
            \since  0.4.0-0
            \brief
                Returns the current Setter instance.
        */
        spp::AbstractPropertySetter* GetSetter() const;

        /**
            \fn     operator ()
            \since  0.4.0-0
            \brief
                Accesses the current internal getter and
                returns the assigned value.
        */
        bool    operator()( const spp::PropertyTypeInfo& info, void* out );

        /**
            \fn     Get
            \since  0.4.0-0
            \brief
                Tries to query the currently assigned
                getter value.
        */
        template < class _t_value_type >
        void   Set( _t_value_type value ) {

            this->operator()( spp::GetTypeInfo< _t_value_type >(), ( void* )&value );

            return;

        }
    protected:
        std::shared_ptr<spp::AbstractPropertySetter>    m_Setter;
        spp::PropertyTypeInfo                           m_OwnerTypeInfo;
        spp::PropertyTypeInfo                           m_ValueTypeInfo;
        spp::PropertyObjectHandle                       m_ObjectHandle;
};
}
