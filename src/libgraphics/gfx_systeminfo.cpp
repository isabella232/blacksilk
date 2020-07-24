
#include <libcommon/metrics.hpp>
#include <libcommon/crt.hpp>
#include <libcommon/prelude.hpp>
#include <libgraphics/systeminfo.hpp>

#include <optional>

#undef major
#undef minor

/** windows version strings **/
const std::string    libgraphics::SystemInfo::OperatingSystem::Version::Windows10                   = "Windows 10";
const std::string    libgraphics::SystemInfo::OperatingSystem::Version::Windows8_1                  = "Windows 8.1";
const std::string    libgraphics::SystemInfo::OperatingSystem::Version::WindowsServer2012_R2        = "Windows Server 2012 R2";
const std::string    libgraphics::SystemInfo::OperatingSystem::Version::Windows8                    = "Windows 8";
const std::string    libgraphics::SystemInfo::OperatingSystem::Version::WindowsServer2012_R1        = "Windows Server 2012 R1";
const std::string    libgraphics::SystemInfo::OperatingSystem::Version::Windows7                    = "Windows 7";
const std::string    libgraphics::SystemInfo::OperatingSystem::Version::WindowsServer2008_R2        = "Windows Server 2008 R2";
const std::string    libgraphics::SystemInfo::OperatingSystem::Version::WindowsServer2008_R1        = "Windows Server 2008 R1";
const std::string    libgraphics::SystemInfo::OperatingSystem::Version::WindowsVista                = "Windows Vista";
const std::string    libgraphics::SystemInfo::OperatingSystem::Version::WindowsServer2003_R2        = "Windows Server 2003 R2";
const std::string    libgraphics::SystemInfo::OperatingSystem::Version::WindowsServer2003_R1        = "Windows Server 2003 R1";
const std::string    libgraphics::SystemInfo::OperatingSystem::Version::WindowsXP                   = "Windows XP";
const std::string    libgraphics::SystemInfo::OperatingSystem::Version::Windows2000                 = "Windows 2000";

#ifdef __APPLE__
#   include <unistd.h>
#   include <stdio.h>
#   include <sys/sysctl.h>
#   include <gl.h>
#   include <OpenGL.h>
#   include <ApplicationServices/ApplicationServices.h>
#   include <glext.h>
#endif

#if LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_WINDOWS
#   include <Windows.h>
#   include <GL/glew.h>
#   include <intrin.h>
#   include <Shlwapi.h>
#   include <libcommon/win/handle.hpp>
#endif

#if LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_LINUX
#   include <GL/glew.h>
#   include <sys/utsname.h>
#   include <iostream>
#   include <fstream>
#   include <string>
#   include <map>
#   include <numeric>
#   include <limits>
#   include <algorithm>
#endif

/** basic dxgi utilities for windows **/
#if LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_WINDOWS

#   include <dxgi.h>
#   include <d3d9.h>

LIBGRAPHICS_PRIVATE_API bool dxgi_support() {

    ::HMODULE       d3d10Module( NULL );
    ::HMODULE       dxgiModule( NULL );

    d3d10Module = ::LoadLibraryA(
                      "d3d10.dll"
                  );
    dxgiModule  = ::LoadLibraryA(
                      "dxgi.dll"
                  );

    bool    result(
        ( ( d3d10Module != NULL ) && ( dxgiModule != NULL ) )
    );

    ( void )  ::FreeLibrary(
        d3d10Module
    );
    ( void )  ::FreeLibrary(
        dxgiModule
    );

    return result;

}

static struct LIBGRAPHICS_PRIVATE_API __auto_init_dxgi_check {
    bool     value;
    __auto_init_dxgi_check() : value( dxgi_support() ) {}

    bool operator()() const {
        return value;
    }
} dxgi_check;

LIBGRAPHICS_PRIVATE_API std::vector< libcommon::WeakRef<::IDXGIAdapter> > dxgi_enumerate_adapaters() {

    if( !dxgi_check() ) {
        return std::vector< libcommon::WeakRef< ::IDXGIAdapter > >();
    }

    std::vector< libcommon::WeakRef< ::IDXGIAdapter > >     adapters;
    ::IDXGIAdapter*                                         currentAdapter( NULL );
    ::IDXGIFactory*                                         factory( NULL );

    ::HRESULT                                               ret( ERROR_SUCCESS );

    ret = ::CreateDXGIFactory(
              __uuidof( ::IDXGIFactory ),
              ( void** )&factory
          );

    assert( ret == S_OK );
    assert( factory );

    if( ( ret != S_OK ) || ( factory == NULL ) ) {
        return std::vector< libcommon::WeakRef< ::IDXGIAdapter > >();
    }

    libgraphics::UInt32 adapterCount( 0 );

    while( factory->EnumAdapters( adapterCount, &currentAdapter ) != DXGI_ERROR_NOT_FOUND ) {

        adapters.push_back( libcommon::WeakRef< ::IDXGIAdapter >( currentAdapter ) );

        ++adapterCount;
    }

    ( VOID )  factory->Release();


    return adapters;
}

LIBGRAPHICS_PRIVATE_API libgraphics::UInt64 dxgi_query_vram_from_adapter( libcommon::WeakRef< ::IDXGIAdapter > adapter ) {
    libgraphics::UInt64     length( 0 );

    if( adapter.empty() ) {
        return 0;
    }

    ::DXGI_ADAPTER_DESC     adapterDesc;

    ::HRESULT   ret = adapter->GetDesc(
                          &adapterDesc
                      );

    assert( ret == S_OK );

    if( ret != ERROR_SUCCESS ) {
        return 0;
    }

    length = static_cast< libgraphics::UInt64>(
                 adapterDesc.DedicatedVideoMemory
             );

    return length;
}

#endif

/** basic sysctl utilities for bsd kernels **/
#if ( LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_MAC )

LIBGRAPHICS_PRIVATE_API bool sysctl_info( int department, int type, size_t length, void* out ) {
    int arguments[2];

    arguments[0]    = department;
    arguments[1]    = type;

    return ( 0 == ::sysctl(
                 arguments,
                 2,
                 out,
                 &length,
                 NULL,
                 0
             ) );
}

LIBGRAPHICS_PRIVATE_API size_t sysctl_info_get_length( int department, int type ) {
    size_t length( 0 );
    int arguments[2];

    arguments[0]    = department;
    arguments[1]    = type;

    if( 0 == ::sysctl(
                arguments,
                2,
                NULL,
                &length,
                NULL,
                0
            ) ) {
        return length;
    }

    return 0;
}

template < class _t_type >
LIBGRAPHICS_PRIVATE_API _t_type sysctl_info_val( int department, int type, size_t length = sizeof( _t_type ) ) {
    _t_type val;

    sysctl_info( department, type, length, reinterpret_cast<void*>( &val ) );

    return val;
}

LIBGRAPHICS_PRIVATE_API char* sysctl_info_string( int department, int type ) {
    size_t length = sysctl_info_get_length(
                        department,
                        type
                    );

    assert( length );

    char* string = new char[length];

    assert( string );

    if( sysctl_info( department, type, length, reinterpret_cast<void*>( string ) ) ) {
        return string;
    }

    delete [] string;

    return string;
}
#endif

namespace libgraphics {

/** common code **/
bool SystemInfo::supportOpenGLFeature( const std::string& featureName ) {

    for( auto it = this->m_OpenGLExtensions.begin(); it != this->m_OpenGLExtensions.end(); ++it ) {
        if( ( *it ) == featureName ) {
            return true;
        }
    }

    return false;

}

void SystemInfo::queryOpenGLInfo() {

    this->m_OpenGLRenderer      = ( const char* ) ::glGetString( GL_RENDERER );
    this->m_OpenGLVendor        = ( const char* ) ::glGetString( GL_VENDOR );

    struct _version_t {
        ::GLint     major;
        ::GLint     minor;
        _version_t() : major( 0 ), minor( 0 ) {
            std::string versionString = ( const char* ) ::glGetString( GL_VERSION );

            if( versionString.size() >= 3 ) {
                {
                    std::stringstream ss;
                    ss << versionString[0];
                    ss >> major;
                }
                {
                    std::stringstream ss;
                    ss << versionString[2];
                    ss >> minor;
                }
            }
        }
    } version;

    this->m_OpenGLVersion.release   = version.major;
    this->m_OpenGLVersion.major     = version.minor;
    this->m_OpenGLVersion.minor     = 0;

    std::string rawList = ( const char* ) ::glGetString( GL_EXTENSIONS );

    this->m_OpenGLExtensions.clear();

    if( !rawList.empty() ) {

        std::string currentExtension;

        for( auto it = rawList.begin(); it != rawList.end(); ++it ) {
            currentExtension.clear();

            while( ( it != rawList.end() ) && ( *it != ' ' ) ) {
                currentExtension += *it;
                ++it;
            }

            if( !currentExtension.empty() ) {
                this->m_OpenGLExtensions.push_back( currentExtension );
            }

            if( rawList.end() == it ) {
                break;
            }
        }

    }
}


/** mac specific code **/
#ifdef __APPLE__

void SystemInfo::queryCpuInfo() {

    /// query the amount of available cpu cores
    size_t numCpuCores( 0 );
    {
        numCpuCores = sysctl_info_val<size_t>( CTL_HW, HW_AVAILCPU );

        if( numCpuCores == 0 ) {
            numCpuCores = sysctl_info_val<size_t>( CTL_HW, HW_NCPU );

            if( numCpuCores == 0 ) {
                numCpuCores = 1;
            }
        }
    }
    this->m_CpuCoreCount    = numCpuCores;

    /// query the processor core frequency
    this->m_CpuCoreClockSpeed = sysctl_info_val<libgraphics::UInt32>( CTL_HW, HW_CPU_FREQ );

    /// query the machine name
    this->m_CpuId   = sysctl_info_string( CTL_HW, HW_MACHINE );

    /// query the machine vendor
    this->m_CpuVendor = "Apple";
}

void SystemInfo::queryGeneralInfo() {
    this->m_Architecture        = 64; /// currently only 64bit builds
    this->m_AvailableMemory     = 0;
    this->m_Unixoid             = true;

    libgraphics::UInt64 size = sysctl_info_val<libgraphics::UInt64>( CTL_HW, HW_MEMSIZE );

    this->m_AvailableMemory = size;
}

void SystemInfo::querySystemInfo() {
    this->m_SystemVersion = sysctl_info_string( CTL_KERN, KERN_OSRELEASE );
    this->m_SystemName    = "Mac OS X";
}

void SystemInfo::queryGPUInfo() {

    this->m_GpuVendor       = this->m_OpenGLVendor;
    this->m_GpuId           = this->m_OpenGLRenderer;

}

#elif LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_WINDOWS

void SystemInfo::queryGPUInfo() {

    this->m_GpuVendor       = this->m_OpenGLVendor;
    this->m_GpuId           = this->m_OpenGLRenderer;

    /// query available vram
    libgraphics::UInt64     vramSize( 0 );

    do {
        if( !::dxgi_check() ) {
            break;
        }

        auto adapters = ::dxgi_enumerate_adapaters();

        if( adapters.empty() ) {

            for( auto it = adapters.begin(); it != adapters.end(); ++it ) {
                ( *it )->Release();
            }

            break;
        }

        auto firstAdapter = adapters.front(); /// \todo handle multi adapter systems
        vramSize          = ::dxgi_query_vram_from_adapter( firstAdapter );

        for( auto it = adapters.begin(); it != adapters.end(); ++it ) {
            ( *it )->Release();
        }
    } while( false );

    this->m_GpuAvailableMemory      = vramSize;
}

void SystemInfo::queryCpuInfo() {

    /// query processor count
    libgraphics::UInt64 coreCount( 0 );
    {
        ::SYSTEM_INFO       systemInfo;
        {
            ::memset( ( void* )&systemInfo, 0, sizeof( systemInfo ) );
        }

        ::GetSystemInfo(
            &systemInfo
        );

        coreCount = static_cast<libgraphics::UInt64>( systemInfo.dwNumberOfProcessors );
    }
    this->m_CpuCoreCount    = ( libgraphics::UInt16 )coreCount;


    /// query vendor identifier etc., and frequency
    static const std::string registryKey = "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0\\";
    static const std::string subkey_processorName = "ProcessorNameString";
    static const std::string subkey_vendorName    = "VendorIdentifier";
    static const std::string subkey_freq          = "~MHz";

    libgraphics::UInt64 freq;
    std::string     vendor;
    std::string     id;
    {
        ::DWORD type( REG_SZ );
        ::DWORD length( MAX_PATH );
        ::PVOID stringData( NULL );

        /// allocate data
        {
            stringData = ( ::PVOID ) malloc( sizeof( CHAR ) * MAX_PATH );
        }

        /// query vendor
        {
            LSTATUS ret = ::SHGetValueA(
                              HKEY_LOCAL_MACHINE,
                              ( LPCSTR )registryKey.c_str(),
                              ( LPCSTR )subkey_vendorName.c_str(),
                              &type,
                              stringData,
                              &length
                          );

            assert( ret == ERROR_SUCCESS );
            assert( stringData );

            if( ret == ERROR_SUCCESS ) {
                vendor.assign( ( const char* )stringData );
            }
        }

        length          = MAX_PATH;

        /// query id
        {
            LSTATUS ret = ::SHGetValueA(
                              HKEY_LOCAL_MACHINE,
                              ( LPCSTR )registryKey.c_str(),
                              ( LPCSTR )subkey_processorName.c_str(),
                              &type,
                              stringData,
                              &length
                          );

            assert( ret == ERROR_SUCCESS );
            assert( stringData );

            if( ret == ERROR_SUCCESS ) {
                id.assign( ( const char* )stringData );
            }
        }

        type    = REG_DWORD;
        length  = sizeof( DWORD );

        /// query freq
        {
            LSTATUS ret = ::SHGetValueA(
                              HKEY_LOCAL_MACHINE,
                              ( LPCSTR )registryKey.c_str(),
                              ( LPCSTR )subkey_freq.c_str(),
                              &type,
                              stringData,
                              &length
                          );

            assert( ret == ERROR_SUCCESS );

            if( ret == ERROR_SUCCESS ) {
                freq = *( ( DWORD* )stringData );
            }
        }

        /// free data
        free( ( void* )stringData );
    }

    assert( !vendor.empty() );
    assert( !id.empty() );
    assert( freq > 0 );

    this->m_CpuCoreClockSpeed   = ( libgraphics::UInt32 )libcommon::metrics::megahertz<libgraphics::UInt64>( freq );
    this->m_CpuId               = id;
    this->m_CpuVendor           = vendor;

}

void SystemInfo::queryGeneralInfo() {
    this->m_Architecture        = 64;
    this->m_AvailableMemory     = 0;
    this->m_Unixoid             = false;

    /// query available cpu-accessible memory
    {
        ::MEMORYSTATUSEX        status;
        {
            ::memset( ( void* )&status, 0, sizeof( status ) );
            status.dwLength = sizeof( status );
        }

        ::BOOL  ret =::GlobalMemoryStatusEx(
                         &status
                     );
        ( void )ret;

        assert( ret );

        /// \todo error log?

        this->m_AvailableMemory = static_cast<libgraphics::UInt64>( status.ullTotalPhys );

        assert( this->m_AvailableMemory > 0 );
    }
}

void SystemInfo::querySystemInfo() {
    this->m_SystemName          = "Windows";

    /// query os version
    {

        ::OSVERSIONINFOEX versionInfo;
        {
            ::memset( ( void* )&versionInfo, 0, sizeof( versionInfo ) );
            versionInfo.dwOSVersionInfoSize = sizeof( versionInfo );
        }

        ::BOOL  ret = GetVersionEx(
                          ( OSVERSIONINFO* )&versionInfo
                      );
        ( void )ret;

        assert( ret );

        /// \todo error log?


        std::string resultingVersion;

        switch( versionInfo.dwMajorVersion ) {
            case 6:
                switch( versionInfo.dwMinorVersion ) {
                    case 0: {
                        if( versionInfo.wProductType == VER_NT_WORKSTATION ) {
                            resultingVersion = OperatingSystem::Version::WindowsVista;
                        } else {
                            resultingVersion = OperatingSystem::Version::WindowsServer2008_R1;
                        }
                    }
                    break;

                    case 1: {
                        if( versionInfo.wProductType == VER_NT_WORKSTATION ) {
                            resultingVersion = OperatingSystem::Version::Windows7;
                        } else {
                            resultingVersion = OperatingSystem::Version::WindowsServer2008_R2;
                        }
                    }
                    break;

                    case 2: {
                        if( versionInfo.wProductType == VER_NT_WORKSTATION ) {
                            resultingVersion = OperatingSystem::Version::Windows8;
                        } else {
                            resultingVersion = OperatingSystem::Version::WindowsServer2012_R1;
                        }
                    }
                    break;

                    case 3: {
                        if( versionInfo.wProductType == VER_NT_WORKSTATION ) {
                            resultingVersion = OperatingSystem::Version::Windows8_1;
                        } else {
                            resultingVersion = OperatingSystem::Version::WindowsServer2012_R2;
                        }
                    }
                    break;

                    default: break; /** unknown **/
                }

                break;

            case 5:
                switch( versionInfo.dwMinorVersion ) {
                    case 0: {
                        resultingVersion = OperatingSystem::Version::Windows2000;
                    }
                    break;

                    case 1: {
                        resultingVersion = OperatingSystem::Version::WindowsXP;
                    }
                    break;

                    case 2: {
                        if( ::GetSystemMetrics( SM_SERVERR2 ) == 0 ) {
                            resultingVersion = OperatingSystem::Version::WindowsServer2003_R1;
                        } else {
                            resultingVersion = OperatingSystem::Version::WindowsServer2003_R2;
                        }
                    }
                    break;

                    default: break;
                }

                break;

            default: break;
        }

        assert( !resultingVersion.empty() );

        this->m_SystemVersion = resultingVersion;
    }
}


#elif ( LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_LINUX )

void SystemInfo::queryGPUInfo() {

    this->m_GpuVendor       = this->m_OpenGLVendor;
    this->m_GpuId           = this->m_OpenGLRenderer;
}


namespace proc_parser {
std::string::iterator   ignore_whitespaces( std::string::iterator current, std::string::iterator end ) {
    while( ( ( *current == ' ' ) || ( *current == '\t' ) || ( *current == '\n' ) ) && current != end ) {
        ++current;
    }

    return current;
}
std::string::reverse_iterator   ignore_whitespaces_r( std::string::reverse_iterator current, std::string::reverse_iterator end ) {
    while( ( ( *current == ' ' ) || ( *current == '\t' ) || ( *current == '\n' ) ) && current != end ) {
        ++current;
    }

    return current;
}
std::string::iterator   until_whitespace_or_delimeter( std::string::iterator current, std::string::iterator end ) {
    while( ( *current != ' ' ) && ( *current != '\t' ) && ( *current != '\n' ) && current != end ) {
        ++current;
    }

    return current - 1;
}

std::optional< std::pair< std::string::iterator, std::pair<std::string, std::string> > >   get_entry( std::string::iterator current, std::string::iterator end ) {
    current = ignore_whitespaces( current, end );

    if( current == end ) {
        return std::nullopt;
    }

    {
        auto dot = std::find( current, end, ':' );

        if( dot == end ) {
            return std::nullopt;
        }

        std::string raw_name( current, dot - 1 );
        std::string name;
        {
            const auto front_it = ignore_whitespaces( raw_name.begin(), raw_name.end() );
            const auto back_it  = ignore_whitespaces_r( raw_name.rbegin(), raw_name.rend() );

            name.assign( front_it, back_it.base() );
        }

        auto endl = std::find( dot + 1, end, '\n' );

        if( endl == end ) {
            return std::nullopt;
        }

        std::string raw_value( dot + 1, endl );
        std::string value;
        {
            const auto front_it = ignore_whitespaces( raw_value.begin(), raw_value.end() );
            const auto back_it  = ignore_whitespaces_r( raw_value.rbegin(), raw_value.rend() );

            value.assign( front_it, back_it.base() );
        }

        return std::make_optional( std::pair<std::string::iterator, std::pair<std::string, std::string> >( endl + 1, std::pair<std::string, std::string>( name, value ) ) );
    }

}

typedef std::vector<std::pair<std::string, std::string> >  proc_map_t;

proc_map_t::iterator    find_entry( proc_map_t& map, const std::string& name ) {
    for( auto it = map.begin(); it != map.end(); ++it ) {
        if( ( *it ).first == name ) {
            return it;
        }
    }

    return map.end();
}
proc_map_t::iterator    find_entry_between( proc_map_t::iterator begin, const proc_map_t::iterator& end, const std::string& name ) {

    while( begin != end ) {
        if( ( *begin ).first == name ) {
            return begin;
        }

        ++begin;
    }

    return end;
}

}


void SystemInfo::queryCpuInfo() {

    /// query processor information
    do {
        static const std::string proc_path = "/proc/cpuinfo";
        std::ifstream input_file( proc_path.c_str(), std::ios_base::in );

        std::string contents( ( std::istreambuf_iterator<char>( input_file ) ), std::istreambuf_iterator<char>() );

        if( !input_file.is_open() ) {
            break;
        }

        if( contents.empty() ) {
            break;
        }

        struct proc_entry {
            size_t      core_count;
            std::string vendor_id;
            size_t      physical_id;
            std::string model;
            double      mhz;

            proc_entry() : core_count( 0 ), vendor_id( "" ),
                physical_id( 0 ), model( "" ), mhz( 0.0 ) {}
        };

        auto    current         = contents.begin();

        const auto    end       = contents.end();

        proc_parser::proc_map_t proc_map;

        while( true ) {
            auto    entry           = proc_parser::get_entry( current, end );

            if( !entry ) {
                break;
            }

            proc_map.push_back( entry->second );

            current                     = entry->first;
        }

        std::map<size_t, proc_entry> entries;

        proc_parser::proc_map_t::iterator map_it = proc_map.begin();

        do {
            auto    current_proc =
                proc_parser::find_entry_between(
                    map_it,
                    proc_map.end(),
                    "processor"
                );

            if( current_proc == proc_map.end() ) {
                break;
            }

            auto    next_proc =
                proc_parser::find_entry_between(
                    current_proc + 1,
                    proc_map.end(),
                    "processor"
                );

            auto    current_entry =
                proc_parser::find_entry_between(
                    current_proc,
                    next_proc,
                    "physical id"
                );

            /// find physical id
            size_t  physical_cpu_id( 0 );
            {
                if( current_entry == next_proc ) {
                    break;
                }

                {
                    std::stringstream ss;
                    ss << ( *current_entry ).second;
                    ss >> physical_cpu_id;
                }
            }

            /// get map proc entry
            auto& map_proc_entry =
                entries[physical_cpu_id];

            /// increment number of cores
            map_proc_entry.core_count++;

            /// find vendor id
            if( map_proc_entry.vendor_id.empty() ) {
                current_entry =     proc_parser::find_entry_between(
                                        current_proc,
                                        next_proc,
                                        "vendor_id"
                                    );

                std::string  vendor;
                {
                    if( current_entry == next_proc ) {
                        break;
                    }

                    vendor = ( *current_entry ).second;
                }

                map_proc_entry.vendor_id = vendor;
            }


            /// find model name
            if( map_proc_entry.model.empty() ) {
                current_entry =     proc_parser::find_entry_between(
                                        current_proc,
                                        next_proc,
                                        "model name"
                                    );

                std::string  model;
                {
                    if( current_entry == next_proc ) {
                        break;
                    }

                    model = ( *current_entry ).second;
                }

                map_proc_entry.model = model;
            }

            /// find mhz
            if( map_proc_entry.mhz < 1.0 ) {

                // try scaling_max_freq first
                {
                    std::ifstream scaling_max_freq( "/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq" );
                    double  kHz( 0.0 );
                    scaling_max_freq >> kHz;
                    map_proc_entry.mhz = kHz / 1000.0;
                }

                // else cpuinfo
                if( map_proc_entry.mhz < 1.0 ) {

                    current_entry =     proc_parser::find_entry_between(
                                            current_proc,
                                            next_proc,
                                            "cpu MHz"
                                        );

                    double  mhz( 0.0 );
                    {
                        if( current_entry == next_proc ) {
                            break;
                        }

                        {
                            std::stringstream ss;
                            ss << ( *current_entry ).second;
                            ss >> mhz;
                        }
                    }

                    map_proc_entry.mhz = mhz;
                }
            }

            map_it = next_proc;
        } while( map_it != proc_map.end() );

        /// compose data
        ///
        if( entries.empty() ) {
            break;
        }

        auto first_proc = ( *entries.begin() ).second;

        this->m_CpuCoreClockSpeed   = libcommon::metrics::megahertz<libcommon::UInt64>( first_proc.mhz );
        this->m_CpuCoreCount        = first_proc.core_count;
        this->m_CpuVendor           = first_proc.vendor_id;
        this->m_CpuId               = first_proc.model;
    } while( false );

}

void SystemInfo::queryGeneralInfo() {
    this->m_Architecture        = 64;
    this->m_AvailableMemory     = 0;
    this->m_Unixoid             = true;
}

void SystemInfo::querySystemInfo() {

    /// query system version and name
    {
        ::utsname   sys_version;

        if( ::uname( &sys_version ) == 0 ) {
            this->m_SystemVersion = sys_version.release;
            this->m_SystemName    = sys_version.sysname;
        } else { /// fallback to default values
            this->m_SystemName    = "DefaultLinux";
        }
    }

    /// query ram
    do {
        std::string     data;
        std::ifstream   mem_info_file( "/proc/meminfo" );

        unsigned long memory_length( 0 );

        while( mem_info_file >> data ) {
            if( data == "MemTotal:" ) {
                if( mem_info_file >> memory_length ) {
                    break;
                }

                break;
            }

            mem_info_file.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );
        }

        this->m_AvailableMemory = libcommon::metrics::kilobytes<libcommon::UInt64>( memory_length );


    } while( false );

}

#else
#   error not implemented
#endif

SystemInfo::SystemInfo() {
    this->m_Architecture            = 0;
    this->m_AvailableMemory         = 0;
    this->m_CpuCoreClockSpeed       = 0;
    this->m_CpuCoreCount            = 0;
    this->m_CpuId                   = "";
    this->m_CpuVendor               = "";
    this->m_GpuAvailableMemory      = 0;
    this->m_GpuId                   = "";
    this->m_GpuVendor               = "";
    this->m_SystemName              = "";
    this->m_SystemVersion           = "";
    this->m_Unixoid                 = false;

    this->queryGeneralInfo();
    this->queryCpuInfo();
    this->querySystemInfo();
    this->queryOpenGLInfo();
    this->queryGPUInfo();
}

const libgraphics::UInt64&      SystemInfo::availableMemory() const {

    return this->m_AvailableMemory;

}

const libgraphics::UInt8&       SystemInfo::architecture() const {

    return this->m_Architecture;

}

const std::string&              SystemInfo::cpuVendor() const {

    return this->m_CpuVendor;

}

const std::string&              SystemInfo::cpuId() const {

    return this->m_CpuId;

}

const libgraphics::UInt16&      SystemInfo::cpuCoreCount() const {

    return this->m_CpuCoreCount;

}

const libgraphics::UInt32&      SystemInfo::cpuCoreClockSpeed() const {

    return this->m_CpuCoreClockSpeed;

}

const std::string&              SystemInfo::systemName() const {

    return this->m_SystemName;

}

const std::string&              SystemInfo::systemVersion() const {

    return this->m_SystemVersion;

}

const bool&                     SystemInfo::unixoid() const {

    return this->m_Unixoid;

}

const SystemInfo::OpenGLVersion&            SystemInfo::openGLVersion() const {

    return this->m_OpenGLVersion;

}

const std::string&              SystemInfo::openGLRenderer() const {

    return this->m_OpenGLRenderer;

}

const std::string&              SystemInfo::openGLVendor() const {

    return this->m_OpenGLVendor;

}

const std::vector<std::string>&  SystemInfo::openGLExtensions() const {

    return this->m_OpenGLExtensions;

}

const std::string&              SystemInfo::gpuVendor() const {

    return this->m_GpuVendor;

}

const std::string&              SystemInfo::gpuId() const {

    return this->m_GpuId;

}

const libgraphics::UInt64&      SystemInfo::gpuAvailableMemory() const {

    return this->m_GpuAvailableMemory;

}



}
