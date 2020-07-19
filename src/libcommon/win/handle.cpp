
#include <libcommon/win/handle.hpp>

#include <Windows.h>
#include <assert.h>

namespace libcommon {
namespace win {

NativeHandle::NativeHandle( const RawHandleVal& val ) : m_Val( val ) {}
NativeHandle::~NativeHandle() {
    this->close();

    this->m_Val = LIBCOMMON_NULL;
}
void NativeHandle::close() {
    ::BOOL  ret = ::CloseHandle(
                      this->m_Val
                  );

    ( void )ret;
    assert( ret );
}

}
}
