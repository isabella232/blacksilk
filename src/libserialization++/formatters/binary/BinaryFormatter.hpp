/**
    Copyright 2013 by FD Imaging

    http://fd-imaging.com

    All rights reserved.
*/
#pragma once

#include <string>
#include <typeinfo>

#include <libserialization++/Property.hpp>
#include <libserialization++/PropertyDecoder.hpp>
#include <libserialization++/PropertyEncoder.hpp>
#include <libserialization++/SerializationFormatter.hpp>
#include <libserialization++/PropertyCollection.hpp>
#include <libserialization++/PropertyAccessor.hpp>
#include <libserialization++/TypeInfo.hpp>
#include <libserialization++/Collections.hpp>
#include <libserialization++/SerializationProvider.hpp>

namespace spp {
namespace formatters {
namespace binary {


/**
    \class      Provider
    \since      0.4.0-0
    \brief
        The provider class for binary-api
        serialization.
*/
class   Provider : public spp::Provider {
    public:

        /**
            \fn     ~Provider
            \since  0.4.0-0
            \brief
                Frees the current internal
                state.
        */
        virtual ~Provider() {}

        /**
            \fn         CreateSerialization
            \since      0.4.0-0
            \brief
                Constructs a new serialization info instance from the
                given existing provider.
        */
        virtual std::shared_ptr<SerializationInfo>      CreateSerialization( spp::Stream* stream );
};


/**
    \class      Formatter
    \since      0.4.0-0
    \brief
        The basic binary formatter
        type.
*/
class   Formatter : public spp::Formatter {
    public:
        /**
            \fn     Formatter
            \since  0.4.0-0
            \brief
                Constructs  a new empty serialization
                formatter.
        */
        Formatter();

        /**
            \fn     Deserialize
            \since  0.4.0-0
            \brief
                Deserializes a specific property collection
                from the internal stream.
        */
        virtual bool Deserialize( spp::PropertyCollection collection );
        virtual bool Deserialize( spp::Property property );
        virtual bool Deserialize( spp::PropertyContainer* container );

        /**
            \fn     Serialize
            \since  0.4.0-0
            \brief
                Serializes a specific property collection
                to the internal stream.
        */
        virtual bool Serialize( spp::PropertyCollection collection );
        virtual bool Serialize( spp::Property property );

        /**
            \fn     Reset
            \since  0.4.0-0
            \brief
                Resets the current state and sets a new stream
                object.
        */
        virtual void Reset( Stream* stream );
    private:
        virtual bool DeserializeContainerElement( spp::PropertyContainer* container );

};

}
}
}
