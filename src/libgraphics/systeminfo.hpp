#pragma once

#include <libgraphics/base.hpp>
#include <string>
#include <sstream>
#include <vector>

#undef major
#undef minor

namespace libgraphics {

/// Queries general information about the current
/// system.
/// \todo is only able to work with systems which run on one processor. no smp support.
class LIBGRAPHICS_API SystemInfo {
    public:
        struct OperatingSystem {
            enum t {
                Windows,
                Linux,
                Mac,
                iOS,
                Android
            };

            struct Version {
                /** mac version strings **/
                /// ... \todo add version strings
                /** linux version strings **/
                /// ... \todo add version strings
                /** windows version strings **/
                static const std::string    Windows10;
                static const std::string    Windows8_1;
                static const std::string    WindowsServer2012_R2;
                static const std::string    Windows8;
                static const std::string    WindowsServer2012_R1;
                static const std::string    Windows7;
                static const std::string    WindowsServer2008_R2;
                static const std::string    WindowsServer2008_R1;
                static const std::string    WindowsVista;
                static const std::string    WindowsServer2003_R2;
                static const std::string    WindowsServer2003_R1;
                static const std::string    WindowsXP;
                static const std::string    Windows2000;
            };
        };
        struct OpenGLVersion {
            libgraphics::UInt16         release; /// 2.
            libgraphics::UInt16         major;   /// 1.
            libgraphics::UInt16         minor;   /// 0

            OpenGLVersion() : release( 0 ), major( 0 ), minor( 0 ) {}

            bool operator == ( const OpenGLVersion& rhs ) const {
                return ( isSame( rhs ) );
            }
            bool operator != ( const OpenGLVersion& rhs ) const {
                return ( !isSame( rhs ) );
            }
            bool operator > ( const OpenGLVersion& rhs ) const {
                return isNewer( rhs );
            }
            bool operator >= ( const OpenGLVersion& rhs ) const {
                return isNewer( rhs ) || isSame( rhs );
            }
            bool operator < ( const OpenGLVersion& rhs ) const {
                return isOlder( rhs );
            }
            bool operator <= ( const OpenGLVersion& rhs ) const {
                return isOlder( rhs ) || isSame( rhs );
            }
            bool isSame( const OpenGLVersion& rhs ) const {
                return ( release == rhs.release ) && ( major == rhs.major ) && ( minor == rhs.minor );
            }
            bool isNewer( const OpenGLVersion& rhs ) const {
                if( rhs.release == release ) {
                    if( rhs.major == major ) {
                        return ( minor > rhs.minor );
                    } else {
                        return ( major > rhs.major );
                    }
                } else {
                    return ( release > rhs.release );
                }
            }
            bool isOlder( const OpenGLVersion& rhs ) const {
                return !isNewer( rhs ) && !isSame( rhs );
            }
            std::string toString() const {
                std::stringstream   ss;

                ss << release;
                ss << ".";
                ss << major;
                ss << ".";
                ss << minor;

                return ss.str();
            }
        };


        SystemInfo();

        const libgraphics::UInt64&      availableMemory() const;
        const libgraphics::UInt8&       architecture() const;

        const std::string&              cpuVendor() const;
        const std::string&              cpuId() const;
        const libgraphics::UInt16&      cpuCoreCount() const;
        const libgraphics::UInt32&      cpuCoreClockSpeed() const;

        const std::string&              systemName() const;
        const std::string&              systemVersion() const;
        const bool&                     unixoid() const;

        const OpenGLVersion&            openGLVersion() const;
        const std::string&              openGLRenderer() const;
        const std::string&              openGLVendor() const;
        const std::vector<std::string>& openGLExtensions() const;

        const std::string&              gpuVendor() const;
        const std::string&              gpuId() const;
        const libgraphics::UInt64&      gpuAvailableMemory() const;

        bool                            supportOpenGLFeature( const std::string& featureName );
    private:
        void    queryCpuInfo();
        void    queryGeneralInfo();
        void    querySystemInfo();
        void    queryOpenGLInfo();
        void    queryGPUInfo();

        /// general info
        libgraphics::UInt64         m_AvailableMemory; /** RAM **/
        libgraphics::UInt8          m_Architecture; /** 32 or 64 **/

        /// cpu info
        std::string                 m_CpuVendor;
        std::string                 m_CpuId;
        libgraphics::UInt16         m_CpuCoreCount;
        libgraphics::UInt32         m_CpuCoreClockSpeed;

        /// system info
        std::string                 m_SystemName;
        std::string                 m_SystemVersion;
        bool                        m_Unixoid;

        /// opengl info
        OpenGLVersion               m_OpenGLVersion;
        std::string                 m_OpenGLRenderer;
        std::string                 m_OpenGLVendor;
        std::vector<std::string>    m_OpenGLExtensions;

        /// gpu info
        std::string                 m_GpuVendor;
        std::string                 m_GpuId;
        libgraphics::UInt64         m_GpuAvailableMemory; /** VRAM **/
};

}
