/**
    Copyright 2013 by FD Imaging

    http://fd-imaging.com

    All rights reserved.
*/
#pragma once

#include    <string>
#include    <sstream>

#include    <libserialization++/TypeInfo.hpp>
#include    <libserialization++/Property.hpp>
#include    <libserialization++/PropertyCollection.hpp>
#include    <libserialization++/PropertyAccessor.hpp>

namespace spp {

/**
    \class      PropertyDecoder
    \since      0.4.0-0
    \brief
        The PropertyDecoder base class decodes a single property
        to an untyped string value.
*/
class   PropertyDecoder {
    public:
        /**
            \fn     ~PropertyDecoder
            \since  0.4.0-0
            \brief
                Frees the internal data.
        */
        virtual ~PropertyDecoder() {}

        /**
            \fn     Decode
            \since  0.4.0-0
            \brief
                Converts a specific value to
                a string value.
        */
        static std::string   Decode( spp::Property& property );
        static std::string   Decode( spp::PropertyTypeInfo& info, void* p );
        static std::string   Decode( unsigned char* val );
        static std::string   Decode( unsigned short* val );
        static std::string   Decode( unsigned int* val );
        static std::string   Decode( unsigned long* val );
        static std::string   Decode( unsigned long long* val );
        static std::string   Decode( char* val );
        static std::string   Decode( short* val );
        static std::string   Decode( int* val );
        static std::string   Decode( long* val );
        static std::string   Decode( long long* val );
        static std::string   Decode( double* val );
        static std::string   Decode( float* val );
        static std::string   Decode( char** val );
};

}
