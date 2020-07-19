#pragma once

#include <libcommon/def.hpp>
#include <libcommon/weakref.hpp>

#if !( LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_WINDOWS )
#   error this file is only compatible with windows system
#endif

namespace libcommon {
namespace win {

typedef void**      RawHandleVal;

/// basic type for all different kinds of handle
/// types.
/// Handles should be allocated following the RIIA pattern.
class LIBCOMMON_API IHandle {
    public:
        virtual ~IHandle() {}

        /// closes the current handle. This destructor should
        /// call this method.
        virtual void close() = 0;
};

/// template wrapper for windows
/// handle types.
template < class _t_handle >
struct ScopedHandle {
        typedef _t_handle       HandleType;

        ScopedHandle( const libcommon::WeakRef<HandleType>& handleValue, bool deleteAfterUsage = true ) : m_Handle( handleValue ), m_DeleteAfterUsage( deleteAfterUsage ) {}
        ScopedHandle( ScopedHandle&& rhs ) : m_Handle( LIBCOMMON_NULL ), m_DeleteAfterUsage( false ) {
            std::swap(
                rhs.m_Handle,
                m_Handle
            );
            std::swap(
                rhs.m_DeleteAfterUsage,
                m_DeleteAfterUsage
            );
        }

        ~ScopedHandle() {
            assert( m_Handle.get() );

            if( m_Handle.get() != LIBCOMMON_NULL ) {
                m_Handle->close();
            }

            if( m_DeleteAfterUsage ) {
                delete( m_Handle.get() );
            }
        }
        void close() {
            assert( m_Handle.get() );

            m_Handle->close();

            if( m_DeleteAfterUsage ) {
                delete m_Handle.get();
                m_DeleteAfterUsage = false;
            }
        }

        libcommon::WeakRef<HandleType>&         handle() { return m_Handle; }
        const libcommon::WeakRef<HandleType>&   handle() const { return m_Handle; }
    private:
        libcommon::WeakRef<HandleType>      m_Handle;
        bool                                m_DeleteAfterUsage;
};

/// the native windows HANDLE type
class LIBCOMMON_API NativeHandle : public libcommon::win::IHandle {
    public:
        NativeHandle( const RawHandleVal& val );
        virtual ~NativeHandle();

        virtual void close();
    protected:
        RawHandleVal    m_Val;
};

/// creates a  new native scoped handle
libcommon::win::ScopedHandle<NativeHandle>  makeHandle( const RawHandleVal& val );

}
}
