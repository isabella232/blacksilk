#include <libfoundation/app/application.hpp>
#include <cmath>

#ifdef __APPLE__
namespace libfoundation {
namespace app {
class MacApplicationSystemLayer : public libfoundation::app::ApplicationSystemLayer {
    public:
        virtual ~MacApplicationSystemLayer() {}

        virtual std::string getTemporaryPath() { return ""; }
        virtual std::string getApplicationPath() { return ""; }
        virtual std::string getSessionPath() { return ""; }
        virtual bool deleteFile( const std::string& path ) {
            ( void )path;
            return false;
        }

        virtual bool createFile( const std::string& path ) {
            ( void )path;
            return false;
        }
};

ApplicationSystemLayer* createSystemLayerForPlatform() {
    return new MacApplicationSystemLayer();
}

}
}
#endif

#if LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_LINUX
namespace libfoundation {
namespace app {
class LinuxApplicationSystemLayer : public libfoundation::app::ApplicationSystemLayer {
    public:
        virtual ~LinuxApplicationSystemLayer() {}

        virtual std::string getTemporaryPath() { return ""; }
        virtual std::string getApplicationPath() { return ""; }
        virtual std::string getSessionPath() { return ""; }
        virtual bool deleteFile( const std::string& path ) {
            ( void )path;
            return false;
        }

        virtual bool createFile( const std::string& path ) {
            ( void )path;
            return false;
        }
};

ApplicationSystemLayer* createSystemLayerForPlatform() {
    return new LinuxApplicationSystemLayer();
}

}
}
#endif

#ifdef _WIN32
#   include <Windows.h>
#   include <ShlObj.h>
#   undef min

namespace libfoundation {
namespace app {

class WindowsApplicationSystemLayer : public libfoundation::app::ApplicationSystemLayer {
    public:
        virtual ~WindowsApplicationSystemLayer() {}

        virtual std::string getTemporaryPath() {
            char    temporaryPath[MAX_PATH];

            const auto result = ::SHGetFolderPathA(
                                    NULL,
                                    CSIDL_APPDATA,
                                    NULL,
                                    0,
                                    temporaryPath
                                );
            assert( SUCCEEDED( result ) );

            if( SUCCEEDED( result ) ) {
                return std::string( ( char* )temporaryPath );
            }

            return std::string();
        }

        virtual std::string getApplicationPath() {
            char    path[MAX_PATH];

            const auto success = ::GetModuleFileNameA(
                                     NULL,
                                     ( char* )path,
                                     MAX_PATH
                                 );
            assert( success > 0 );

            if( success == 0 ) {
                return std::string();
            }

            return std::string( ( char* )path, success );
        }

        virtual std::string getSessionPath() {
            char    temporaryPath[MAX_PATH];

            const auto result = ::SHGetFolderPathA(
                                    NULL,
                                    CSIDL_MYDOCUMENTS,
                                    NULL,
                                    0,
                                    temporaryPath
                                );
            assert( SUCCEEDED( result ) );

            if( SUCCEEDED( result ) ) {
                return std::string( ( char* )temporaryPath );
            }

            return std::string();
        }

        virtual bool deleteFile( const std::string& path ) {
            return ( ::DeleteFileA(
                         path.c_str()
                     ) == TRUE );
        }

        virtual bool createFile( const std::string& path ) {
            return ( ::CreateFileA(
                         path.c_str(),
                         GENERIC_READ | GENERIC_WRITE,
                         0,
                         NULL,
                         CREATE_ALWAYS,
                         FILE_ATTRIBUTE_NORMAL,
                         NULL
                     ) != INVALID_HANDLE_VALUE );
        }
};


ApplicationSystemLayer* createSystemLayerForPlatform() {
    return new WindowsApplicationSystemLayer();
}


}
}

#endif



/// general stuff
namespace libfoundation {
namespace app {

std::string ApplicationSystemLayer::getRandomFileName( size_t length ) const {
    return getRandomFileNameWithExtension( "", length );
}

static const size_t numberOfPossibleChars = 10 + ( 25 * 2 );
static char numberToChar( char number ) {
    if( number >= 35 ) {
        return 'a' + std::min( number - 35, 25 );
    }

    if( number >= 10 ) {
        return 'A' + ( number - 10 );
    }

    return '0' + number;
}

std::string ApplicationSystemLayer::getRandomFileNameWithExtension( const std::string& ext, size_t length ) const {
    std::string fileName;

    assert( length != 0 );

    while( --length != 0 ) {
        const auto number = rand() % numberOfPossibleChars;

        fileName += numberToChar(
                        ( char )number
                    );
    }

    return ext.empty() ? fileName : ( fileName + "." + ext );
}

}
}
