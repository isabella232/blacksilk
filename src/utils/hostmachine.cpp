
#include <utils/hostmachine.hpp>

#include <libgraphics/base.hpp>
#include <libgraphics/systeminfo.hpp>
#include <QDebug>

#if LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_MAC
#include <mach-o/dyld.h>
#elif LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_LINUX
#include <unistd.h>
#elif LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_WINDOWS
#include <Windows.h>
#include <cstdint>
#endif

namespace blacksilk {

libcommon::WeakRef<HostMachine>    HostMachine::global() {
    static libcommon::SharedPtr<HostMachine> local( 0 );

    if( local.empty() ) {
        local.reset( new HostMachine() );
    }

    return local.weakRef();
}

bool HostMachine::compatibleWith( const HostMachineRequirements& config ) {

    bool    good( true );

    if( this->m_SystemInfo.availableMemory() < config.minimalRAMSize ) {
        good = false;
    }

    if( this->m_SystemInfo.cpuCoreCount() < config.minimalCpuCores ) {
        good = false;
    }

    if( this->m_SystemInfo.cpuCoreClockSpeed() < config.minimalCpuFreq ) {
        good = false;
    }

    /** querying the available gpu memory is not working reliable on all platforms, so
     *  disable this check */
    //if ( this->m_SystemInfo.gpuAvailableMemory() < config.minimalVRAMSize ) {
    //    good = false;
    //}

    if( this->m_SystemInfo.openGLVersion().isOlder( config.minimalOpenGLVersion ) ) {
        good = false;
    }

    return good;

}

libgraphics::SystemInfo& HostMachine::systemInfo() {
    return this->m_SystemInfo;
}

const libgraphics::SystemInfo&  HostMachine::systemInfo() const {
    return this->m_SystemInfo;
}



void printOpenGLExtensionInfo() {
    libgraphics::SystemInfo&  info = HostMachine::global()->systemInfo();

    qDebug() << "*** AVAILABLE OPENGL EXTENSIONS ***";

    for( auto it = info.openGLExtensions().begin(); it != info.openGLExtensions().end(); ++it ) {
        qDebug() << "   * " << ( *it ).c_str();
    }

    qDebug() << "*************************";
}

void printMachineRequirements( const HostMachineRequirements& requirements ) {
    auto machine = HostMachine::global();

    qDebug() << "Checking system requirements...";

    if( machine->compatibleWith( requirements ) ) {
        qDebug() << "   ===> Ok. Machine compatible with requirements.";
    } else {
        qDebug() << "   ===> System incompatible. Program may crash.";

        if( machine->systemInfo().availableMemory() < requirements.minimalRAMSize ) {
            qDebug() << "       - insufficient RAM. At least" << libcommon::metrics::toMegabytes( requirements.minimalRAMSize ) << "mb needed. ";
        }

        if( machine->systemInfo().cpuCoreCount() < requirements.minimalCpuCores ) {
            qDebug() << "       - insufficient number of cpu cores. At least " << requirements.minimalCpuCores << " needed. ";
        }

        if( machine->systemInfo().cpuCoreClockSpeed() < requirements.minimalCpuFreq ) {
            qDebug() << "       - insufficient cpu speed. At least " << libcommon::metrics::toGigahertz( requirements.minimalCpuFreq ) << "GHz needed. ";
        }

        /** querying the available gpu memory is not working reliable on all platforms, so
         *  disable this check */
        //if ( machine->systemInfo().gpuAvailableMemory() < requirements.minimalVRAMSize ) {
        //    qDebug() << "       - insufficient VRAM. At least" << libcommon::metrics::toMegabytes( requirements.minimalVRAMSize )  << "mb needed. ";
        //}

        if( machine->systemInfo().openGLVersion().isOlder( requirements.minimalOpenGLVersion ) ) {
            qDebug() << "       - supported OpenGL version too old. At least version " << requirements.minimalOpenGLVersion.toString().c_str() << " needed. ";
        }

    }
}


void printSystemInfo() {
    libgraphics::SystemInfo&  info = HostMachine::global()->systemInfo();

    qDebug() << "*** SYSTEM INFORMATION ***";
    qDebug() << "- System: ";
    qDebug() << "   * Name:                 " << info.systemName().c_str();
    qDebug() << "   * Version:              " << info.systemVersion().c_str();
    qDebug() << "   * RAM size:             " << libcommon::metrics::toMegabytes( info.availableMemory() ) << "mb";
    qDebug() << "   * Architecture:         " << info.architecture();
    qDebug() << "- Cpu: ";
    qDebug() << "   * Number of cores:      " << info.cpuCoreCount();
    qDebug() << "   * Clock speed:          " << libcommon::metrics::toGigahertz( info.cpuCoreClockSpeed() ) << "GHz";
    qDebug() << "   * Vendor:               " << info.cpuVendor().c_str();
    qDebug() << "   * Id:                   " << info.cpuId().c_str();
    qDebug() << "- Gpu: ";
    qDebug() << "   * OpenGL Version:       " << info.openGLVersion().toString().c_str();
    qDebug() << "   * Renderer              " << info.openGLRenderer().c_str();
    qDebug() << "   * Vendor:               " << info.openGLVendor().c_str();
    qDebug() << "   * VRAM size:            " << libcommon::metrics::toMegabytes( info.gpuAvailableMemory() ) << "mb";
    qDebug() << "**************************";
}

std::tuple<std::string, std::string> applicationPath() {

    std::string dir;  // e.g. /Applications/BlackSilk.app/Contents/MacOS
    std::string base; // e.g. BlackSilk

    std::string fullPath( 1024, '\0' );
    uint32_t size = fullPath.size();

#if LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_MAC
    bool ok = _NSGetExecutablePath( ( char* )fullPath.c_str(), &size ) == 0;
#elif LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_LINUX
    size = readlink( "/proc/self/exe", ( char* )fullPath.c_str(), size );
    bool ok = size > 0;
#elif LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_WINDOWS
    size = GetModuleFileNameA( 0, ( char* )fullPath.c_str(), size );
    bool ok = size > 0;
#endif

    if( ok ) {
        fullPath.resize( size );
    } else {
        fullPath.clear();
    }

    if( !fullPath.empty() ) {

#if LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_WINDOWS
        size_t pos = fullPath.find_last_of( '\\' );
#else
        size_t pos = fullPath.find_last_of( '/' );
#endif

        if( pos != std::string::npos ) {
            dir = fullPath.substr( 0, pos );
            base = fullPath.substr( pos );
        }
    }

    return std::make_tuple( dir, base );
}

}
