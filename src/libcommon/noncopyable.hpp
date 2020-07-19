#pragma once

#include <libcommon/def.hpp>

namespace libcommon {

/// base class for non-copyable objects
struct LIBCOMMON_API    INonCopyable {
    virtual ~INonCopyable() {}
    INonCopyable() {}
    INonCopyable( const INonCopyable& ) = delete;
    INonCopyable& operator = ( const INonCopyable& ) = delete;
};


}
