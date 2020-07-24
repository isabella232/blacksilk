
#include <libfoundation/app/backgroundtask.hpp>
#include <libcommon/atomics.hpp>

namespace libfoundation {
namespace app {

BackgroundTask::BackgroundTask( std::string _name ) : m_Name( _name ) {}

std::string& BackgroundTask::name() {
    return this->m_Name;
}

const std::string& BackgroundTask::name() const {
    return this->m_Name;
}

void BackgroundTask::setName( const std::string& value ) {
    this->m_Name = value;
}

bool AsyncBackgroundTask::running() {
    return libcommon::atomics::equal64(
               &this->m_Running,
               1
           );
}

void AsyncBackgroundTask::beginExecution() {
    ( void ) libcommon::atomics::exchange64(
        &this->m_Running,
        0,
        1
    );
}

void AsyncBackgroundTask::endExecution() {
    ( void ) libcommon::atomics::exchange64(
        &this->m_Running,
        1,
        0
    );
}

}
}
