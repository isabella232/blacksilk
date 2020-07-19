/**
    Copyright 2013 by FD Imaging

    http://fd-imaging.com

    All rights reserved.
*/
#pragma once

namespace spp {

class       Provider;
class       Formatter;
class       Stream;

/**
    \class      SerializationInfo
    \since      0.4.0-0
    \brief
        Contains all objects and structures needed to
        serialize a specific range of objects.
*/
class   SerializationInfo {
    public:
        friend class Provider;

        /**
            \fn     SerializationInfo
            \since  0.4.0-0
            \brief
                Constructs a new SerializationInfo instance from
                the given parameters.
        */
        SerializationInfo( Stream* stream, Formatter* formatter );

        /**
            \fn     ~SerializationInfo
            \since  0.4.0-0
            \brief
                Frees the curren internal
                state.
        */
        virtual ~SerializationInfo();

        /**
            \fn     Reset
            \since  0.4.0-0
            \brief
                Resets the current serialization info
                state.
        */
        void        Reset();

        /**
            \fn     GetStream
            \since  0.4.0-0
            \brief
                Returns the currently active serialization stream
                component.
        */
        Stream*      GetStream() const;

        /**
            \fn     GetFormatter
            \since  0.4.0-0
            \brief
                Returns the currently used
                formatter instance.
        */
        Formatter*  GetFormatter() const;

        /**
            \fn     GetNumberOfSerializedProperties
            \since  0.4.0-0
            \brief
                Returns the total number of serialized
                properties.
        */
        size_t      GetNumberOfSerializedProperties() const;

        /**
            \fn     GetNumberOfSerializedBytes
            \since  0.4.0-0
            \brief
                Returns the total number of serialized
                bytes.
        */
        size_t      GetNumberOfSerializedBytes() const;

        /**
            \fn     GetNumberOfSerializedObjects
            \since  0.4.0-0
            \brief
                Returns the total number of serialized
                objects.
        */
        size_t      GetNumberOfSerializedObjects() const;
    protected:

        size_t                      m_NumberOfSerializedBytes;
        size_t                      m_NumberOfSerializedProperties;
        size_t                      m_NumberOfSerializedObjects;
        Stream*                     m_SerializationStream;
        Formatter*                  m_Formatter;
};

}
