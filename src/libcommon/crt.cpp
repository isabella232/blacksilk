
#include <libcommon/crt.hpp>

#if ( LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_MAC ) || ( LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_LINUX )
#   if ( LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_MAC )
#       include <mach/task.h>
#   endif
#   include <pthread.h>
#   include <unistd.h>
#   include <stdlib.h>
#endif

#include <iostream>

#if ( LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_WINDOWS )
#   include <Windows.h>
#endif

namespace libcommon {

LIBCOMMON_API void    exit( libcommon::UInt32 code ) {
    ::exit( code );
}

LIBCOMMON_API void    abort( const libcommon::Char* msg ) {
    assert( msg );

    libcommon::stdDump( msg );

    libcommon::exit( 1 );
}

LIBCOMMON_API void    stdDump( const libcommon::Char* msg ) {
    assert( msg );

    std::cout.write( msg, strlen( msg ) );
}

#if ( LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_MAC ) || ( LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_LINUX )

#if ( LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_MAC )
LIBCOMMON_API libcommon::UInt32     getCurrentThreadId() {
    ::mach_port_t threadId( ::pthread_mach_thread_np( pthread_self() ) );

    return static_cast<libcommon::UInt32>( threadId );
}
#else
LIBCOMMON_API libcommon::UInt32     getCurrentThreadId() {
    return static_cast<libcommon::UInt32>( pthread_self() );
}
#endif

LIBCOMMON_API libcommon::UInt32     getCurrentProcessId() {
    return static_cast<libcommon::UInt32>( getpid() );
}

LIBCOMMON_API libcommon::Boolean    isUnixSystem() {
    return true;
}

LIBCOMMON_API libcommon::ESystem::t getSystem() {
    return libcommon::ESystem::Mac;
}

#elif ( LIBCOMMON_SYSTEM == LIBCOMMON_WINDOWS )

LIBCOMMON_API libcommon::UInt32     getCurrentThreadId() {

    ::DWORD     threadId( ::GetCurrentThreadId() );

    return threadId;

}

LIBCOMMON_API libcommon::UInt32     getCurrentProcessId() {

    ::DWORD     processId( ::GetCurrentProcessId() );

    return processId;

}

LIBCOMMON_API libcommon::Boolean    isUnixSystem() {

    return false;

}

LIBCOMMON_API libcommon::ESystem::t getSystem() {

    return libcommon::ESystem::Windows;

}

#else
#   error not implemented
#endif

}
