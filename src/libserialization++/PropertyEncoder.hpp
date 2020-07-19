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
    \class      PropertyEncoder
    \since      0.4.0-0
    \brief
        The PropertyEncoder class converts  a string to a property-specific
        value.
*/
class   PropertyEncoder {
    public:
        /**
            \fn     ~PropertyEncoder
            \since  0.4.0-0
            \brief
                Frees the internal data.
        */
        virtual ~PropertyEncoder() {}

        /**
            \fn     Encode
            \since  0.4.0-0
            \brief
               Converts a string to a property-specific
               value.
        */
        static void Encode( Property& dst, std::string val );
};

}
