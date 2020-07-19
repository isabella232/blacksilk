
#include <libserialization++/SerializationFormatter.hpp>
#include <libserialization++/SerializationProvider.hpp>
#include <libserialization++/SerializationStream.hpp>
#include <libserialization++/SerializationInfo.hpp>

using namespace spp;

SerializationInfo::SerializationInfo( Stream* stream, Formatter* formatter ) {

    this->m_Formatter           = formatter;
    this->m_SerializationStream = stream;

    this->m_NumberOfSerializedBytes         = 0;
    this->m_NumberOfSerializedObjects       = 0;
    this->m_NumberOfSerializedProperties    = 0;

}

SerializationInfo::~SerializationInfo() {

    this->m_Formatter                       = 0;
    this->m_SerializationStream             = 0;
    this->m_NumberOfSerializedBytes         = 0;
    this->m_NumberOfSerializedObjects       = 0;
    this->m_NumberOfSerializedProperties    = 0;

}

void        SerializationInfo::Reset() {

    this->m_Formatter                       = 0;
    this->m_SerializationStream             = 0;
    this->m_NumberOfSerializedBytes         = 0;
    this->m_NumberOfSerializedObjects       = 0;
    this->m_NumberOfSerializedProperties    = 0;

}

Stream*      SerializationInfo::GetStream() const {

    return this->m_SerializationStream;

}

Formatter*  SerializationInfo::GetFormatter() const {

    return this->m_Formatter;

}

size_t      SerializationInfo::GetNumberOfSerializedProperties() const {

    return this->m_NumberOfSerializedProperties;

}

size_t      SerializationInfo::GetNumberOfSerializedBytes() const {

    return this->m_NumberOfSerializedBytes;

}

size_t      SerializationInfo::GetNumberOfSerializedObjects() const {

    return this->m_NumberOfSerializedObjects;

}
