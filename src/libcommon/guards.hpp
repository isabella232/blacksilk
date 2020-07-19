#pragma once

#include <libcommon/def.hpp>
#include <libcommon/lockable.hpp>
#include <libcommon/noncopyable.hpp>

#ifdef _WIN32
#   include <Windows.h>
#endif

namespace libcommon {

/// base class for guards
template < class _t_action >
struct LIBCOMMON_API ScopedGuard : libcommon::INonCopyable  {
        typedef _t_action       Action;

        explicit ScopedGuard( const Action& action = Action() ) : m_Action( action ) {
            m_Action.begin();
        }
        ScopedGuard( ScopedGuard&& rhs ) {
            std::swap(
                m_Action,
                rhs.m_Action
            );
        }

        void swap( ScopedGuard& rhs ) {
            std::swap(
                m_Action,
                rhs.m_Action
            );
        }

        ~ScopedGuard() {
            m_Action.end();
        }
    private:
        Action  m_Action;
};

struct LIBCOMMON_API LockGuard : libcommon::INonCopyable {
        struct Action {
                Action( libcommon::ILockable* lockable = nullptr ) : m_Lockable( lockable ) {
                    assert( lockable );
                }
                void begin() {
                    assert( m_Lockable );
                    m_Lockable->lock();
                }
                void end() {
                    assert( m_Lockable );
                    m_Lockable->unlock();
                }
            private:
                libcommon::ILockable*     m_Lockable;
        };

        void swap( LockGuard& rhs ) {
            m_Guard.swap( rhs.m_Guard );
        }

        LockGuard( libcommon::ILockable* lockable ) : m_Guard( Action( lockable ) ) {}
        LockGuard( LockGuard&& rhs ) : m_Guard( std::forward < decltype( m_Guard ) && > ( rhs.m_Guard ) ) {}

    private:
        ScopedGuard<LockGuard::Action> m_Guard;
};

}

namespace std {
inline void swap( libcommon::LockGuard& lhs, libcommon::LockGuard& rhs ) {
    lhs.swap( rhs );
}
}
