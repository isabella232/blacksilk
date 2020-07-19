#pragma once

#include <libcommon/def.hpp>

namespace libcommon {

/// base class for lockable classes
class LIBCOMMON_API ILockable {
    public:
        virtual ~ILockable() {}

        virtual void    lock() = 0;
        virtual bool    tryLock() = 0;
        virtual void    unlock() = 0;
};

}
