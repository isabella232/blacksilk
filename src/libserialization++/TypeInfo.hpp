/**
    Copyright 2013 by FD Imaging

    http://fd-imaging.com

    All rights reserved.
*/
#pragma once

#include <string>
#include <typeinfo>
#include <xc/xbase>

#ifdef SPP_STL_LEGACY
namespace std {
template < class _t_type >
struct remove_reference {
    typedef _t_type    value_type;
};
template < class _t_type >
struct remove_reference< _t_type& > {
    typedef _t_type    value_type;
};
template < class _t_type >
struct remove_reference < _t_type&& > {
    typedef _t_type    value_type;
};
}
#endif

namespace spp {

/**
    \class      PropertyTypeInfo
    \since      0.4.0-0
    \brief
                The PropertyTypeInfo structure is an enhanced version
                of the standard type_info structure.
*/
struct PropertyTypeInfo {
    const   std::type_info&          TypeInfo;
    size_t                      TypeSize;
    bool                        IsClass;


    /**
        \fn     PropertyTypeInfo
        \since  0.4.0-0
        \brief
            Constructs a new PropertyTypeInfo from the specified
            parameters.
    */
    PropertyTypeInfo( const std::type_info& info, const size_t& size, bool isClass = false );

    /**
        \fn     operator =
        \since  0.4.0-0
        \brief
            Copies the contents of this PropertyTypeInfo to
            another property type info.
    */
    PropertyTypeInfo& operator = ( const PropertyTypeInfo& info );

    /**
        \fn     TypeName
        \since  0.4.0-0
        \brief  Returns the name of the current
                internal type_info.
    */
    const char* TypeName() const;

    /**
        \fn     TypeHash
        \since  0.4.0-0
        \brief  Returns the hash of the current
                internal type_info.
    */
    size_t      TypeHash() const;

    /**
        \fn     SizeSmaller
        \since  0.4.0-0
        \brief
            Returns true, if the size of another PropertyTypeInfo structure is smaller
            than the current size.
    */
    bool    SizeSmaller( const PropertyTypeInfo& info )const ;

    /**
        \fn     SizeBigger
        \since  0.4.0-0
        \brief
            Returns true, if the size of another PropertyTypeInfo structure is bigger
            than the current size.
    */
    bool    SizeBigger( const PropertyTypeInfo& info )const ;

    /**
        \fn     SizeEqual
        \since  0.4.0-0
        \brief
            Returns true, if the size of another PropertyTypeInfo structure is equal
            to the current size.
    */
    bool    SizeEqual( const PropertyTypeInfo& info )const ;

    /**
        \fn     Equal
        \since  0.4.0-0
        \brief
            Returns true, if the specified PropertyTypeInfo
            is equal compared to the current
            type.
    */
    bool    Equal( const PropertyTypeInfo& info )const ;
    bool    operator == ( const PropertyTypeInfo& info ) const;

    /**
        \fn     NonEqual
        \since  0.4.0-0
        \brief
            Returns true, if the specified PropertyTypeInfo
            is not equal compared to the current
            type.
    */
    bool    NonEqual( const PropertyTypeInfo& info )const ;
    bool    operator != ( const PropertyTypeInfo& info ) const;
};


/**
    \struct         TypeSize
    \since          0.4.0-0
    \brief
        Returns the size of the specified
        size.
*/
template < class _t_value_type >
struct TypeSize {
    enum {
        Size = sizeof( typename std::remove_reference<_t_value_type>::type )
    };
};


/**
    \fn     GetTypeInfo
    \since  0.4.0-0
    \brief
        Queries the TypeInfo structure for the specified
        value type.
*/
template < class _t_value_type >
const PropertyTypeInfo&    GetTypeInfo() {

    static const PropertyTypeInfo info(
        typeid( _t_value_type ),
        TypeSize<_t_value_type>::Size,
        _xc::_ximpl::__X_meta_is_class< _t_value_type >::value &&
        !std::is_same< _t_value_type, std::string >::value &&
        !std::is_same< _t_value_type, std::wstring >::value
    );

    return info;

}

}
