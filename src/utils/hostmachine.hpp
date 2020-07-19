#pragma once

#include <libgraphics/base.hpp>
#include <libgraphics/systeminfo.hpp>
#include <libcommon/sharedptr.hpp>
#include <libcommon/metrics.hpp>

#include <vector>
#include <string>
#include <tuple>

namespace blacksilk {

/// Represents the minimal host machine requirements
struct HostMachineRequirements {
    libgraphics::SystemInfo::OpenGLVersion      minimalOpenGLVersion;
    libgraphics::UInt64                         minimalCpuFreq;
    libgraphics::UInt16                         minimalCpuCores;
    libgraphics::UInt64                         minimalVRAMSize;
    libgraphics::UInt64                         minimalRAMSize;

    HostMachineRequirements() : minimalCpuFreq( 0 ), minimalCpuCores( 0 ), minimalVRAMSize( 0 ), minimalRAMSize( 0 ) {}
};

/// Represents the current host machine
class HostMachine {
    public:
        /// returns the global HostMachine instance
        static libcommon::WeakRef<HostMachine>          global();

        bool                                            compatibleWith( const HostMachineRequirements& config );
        libgraphics::SystemInfo&                        systemInfo();
        const libgraphics::SystemInfo&                  systemInfo() const;
    protected:
        libgraphics::SystemInfo      m_SystemInfo;
};

/// prints information about the current
/// system.
void printSystemInfo();

/// prints all available opengl extensions to
/// the log.
void printOpenGLExtensionInfo();

/// prints the machine requirements to the log
void printMachineRequirements( const HostMachineRequirements& requirements );

std::tuple<std::string, std::string> applicationPath();

}
