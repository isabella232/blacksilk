
#include <libserialization++/SerializationFormatter.hpp>

using namespace spp;

Formatter::Formatter() {
    this->m_Stream      = 0;
}

Stream*     Formatter::GetStream() const {

    return this->m_Stream;

}
