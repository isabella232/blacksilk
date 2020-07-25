#pragma once

#include <libcommon/def.hpp>

namespace libcommon {

/// returns the current thread id
LIBCOMMON_API libcommon::UInt32     getCurrentThreadId();

/// returns the current process id
LIBCOMMON_API libcommon::UInt32     getCurrentProcessId();

/// returns true, if the current system is based on
/// linux specifications and somewhat posix-compatible
LIBCOMMON_API libcommon::Boolean    isUnixSystem();

/// returns the current system as enum value
LIBCOMMON_API libcommon::ESystem::t getSystem();

/// exits the current process
LIBCOMMON_API void    exit( libcommon::UInt32 code );

/// writes an error message to stderr/stdout and
/// exits the current progress
LIBCOMMON_API void    abort( const libcommon::Char* msg );

/// writes a message to stdout
LIBCOMMON_API void    stdDump( const libcommon::Char* msg );

}
