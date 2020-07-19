/**
    Copyright 2013 by FD Imaging

    http://fd-imaging.com

    All rights reserved.
*/
#pragma once

#include <libserialization++/PropertyCollection.hpp>

namespace spp {

class       PropertyContainer;
class       Stream;

/**
    \class      Formatter
    \since      0.4.0-0
    \brief
        Able to read and write serializable objects from or to Streams.
*/
class       Formatter {
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
            \fn     ~SerializationFormatter
            \since  0.4.0-0
            \brief
                Frees the current data and resets the
                internal state.
        */
        virtual ~Formatter() {}


        /**
            \fn     Deserialize
            \since  0.4.0-0
            \brief
                Deserializes a specific property collection
                from the internal stream.
        */
        virtual bool Deserialize( spp::PropertyCollection collection ) = 0;
        virtual bool Deserialize( spp::Property property ) = 0;
        virtual bool Deserialize( spp::PropertyContainer* container ) = 0;

        /**
            \fn     Serialize
            \since  0.4.0-0
            \brief
                Serializes a specific property collection
                to the internal stream.
        */
        virtual bool Serialize( spp::PropertyCollection collection ) = 0;
        virtual bool Serialize( spp::Property property ) = 0;

        /**
            \fn     Reset
            \since  0.4.0-0
            \brief
                Resets the current state and sets a new stream
                object.
        */
        virtual void Reset( Stream* stream ) = 0;

        /**
            \fn     GetStream
            \since  0.4.0-0
            \brief
                Returns the internal active stream object.
        */
        Stream*     GetStream() const;

    protected:
        Stream*         m_Stream;
};

}
