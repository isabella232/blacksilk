#pragma once

#include <libcommon/def.hpp>
#include <libcommon/crt.hpp>
#include <libcommon/noncopyable.hpp>
#include <libcommon/lockable.hpp>
#include <libcommon/atomics.hpp>

namespace libcommon {

struct LIBCOMMON_API Mutex : public libcommon::INonCopyable, public libcommon::ILockable  {
        Mutex() {
            libcommon::atomics::assign64(
                &m_MutexVal,
                0
            );
        }
        virtual ~Mutex() {}
        virtual void lock() {
            volatile bool _locked = tryLock();

            while( !_locked ) {
                _locked = tryLock();
            }
        }
        virtual void unlock() {
            ( void ) libcommon::atomics::exchange64(
                &m_MutexVal,
                1,
                0
            );
        }
        virtual bool tryLock() {
            return ( libcommon::atomics::exchange64(
                         &m_MutexVal,
                         0,
                         1
                     ) == 0 ) ;
        }
    private:
        libcommon::atomics::type64 m_MutexVal;
};


struct LIBCOMMON_API RecursiveMutex : public libcommon::INonCopyable, public libcommon::ILockable  {
        RecursiveMutex() {
            libcommon::atomics::assign64(
                &m_MutexVal,
                0
            );
            libcommon::atomics::assign64(
                &m_OwningThread,
                0
            );
        }
        virtual ~RecursiveMutex() {}
        virtual void lock() {
            volatile bool _locked = tryLock();

            while( !_locked ) {
                _locked = tryLock();
            }
        }
        virtual void unlock() {
            if( libcommon::atomics::equal64( &m_OwningThread, libcommon::getCurrentThreadId() ) ) {
                if( libcommon::atomics::dec64( &m_MutexVal ) == 0 ) {
                    libcommon::atomics::assign64( &m_OwningThread, 0 );
                }
            }
        }
        virtual bool tryLock() {
            if( libcommon::atomics::equal64( &m_OwningThread, libcommon::getCurrentThreadId() ) ) {
                libcommon::atomics::inc64(
                    &m_MutexVal
                );

                return true;
            } else if( libcommon::atomics::equal64( &m_OwningThread, 0 ) ) {
                libcommon::atomics::exchange64( &m_OwningThread, 0, libcommon::getCurrentThreadId() );

                if( libcommon::atomics::equal64( &m_OwningThread, libcommon::getCurrentThreadId() ) ) {
                    libcommon::atomics::inc64( &m_MutexVal );
                    return true;
                }

                return false;
            }

            return false;
        }
    private:
        libcommon::atomics::type64 m_OwningThread;
        libcommon::atomics::type64 m_MutexVal;
};

}
