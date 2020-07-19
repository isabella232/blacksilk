// Copyright Filtered Digital Imaging

#include <cstring>
#include <regex>
#include <iostream>

#include <QApplication>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDebug>
#include <QtCore>
#include <QtGlobal>
#include <QLoggingCategory>

#if __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

#include <utils/app.hpp>
#include <utils/preset.hpp>
#include <utils/hostmachine.hpp>
#include <utils/offscreencontext.hpp>
#include <libgraphics/curve.hpp>
#include <libgraphics/bezier.hpp>
#include "mainwindow.h"
#include "log/log.hpp"
#include <libcommon/stringutils.hpp>

using namespace blacksilk::ui;
using namespace blacksilk;

static const std::string    default_output_postfix  = "_filtered";

static std::unique_ptr<blacksilk::OffscreenContext>     offscreenCtx;
static bool                                             useGpuRendering = false;

void shutdownCommandLine() {
    if( theApp()->initialized() ) {
        const auto successfullyShutdown = theApp()->shutdown();

        if( !successfullyShutdown ) {
            assert( false );
            qDebug() << "Error: Failed to shutdown program. Exiting.";
            exit( -1 );
        }

        offscreenCtx.reset();
    }
}

#if __APPLE__
void hideFromDock() {
    ProcessSerialNumber PSN = { 0, kCurrentProcess };
    GetCurrentProcess( &PSN );
    TransformProcessType( &PSN, kProcessTransformToBackgroundApplication );
}
#endif // __APPLE__

void initializeCommandLine() {

#if __APPLE__
    // hide if run as CLI app
    hideFromDock();
#endif // __APPLE__

    if( offscreenCtx.get() == nullptr ) {
        offscreenCtx.reset( new blacksilk::OffscreenContext() );

        if( offscreenCtx->valid() ) {
            useGpuRendering = true;
        } else {
            useGpuRendering = false;
        }
    }

    if( useGpuRendering ) {
        const auto successfullyInitializedGpuBackend = theApp()->initializeGraphicsPreview();

        if( !successfullyInitializedGpuBackend ) {
            qDebug() << "Failed to initialize gpu graphics backend. Falling back to cpu-rendering.";
            useGpuRendering = false;
        }

        assert( successfullyInitializedGpuBackend );
    }
}

namespace blacksilk {
struct CommandIdentifier {
    std::string     full;
    std::string     shortcut;
    std::string     desc;

    CommandIdentifier(
        const std::string&  _full,
        const std::string&  _shortcut,
        const std::string&  _desc
    ) : full( _full ), shortcut( _shortcut ), desc( _desc ) {}
};
typedef std::function<bool( const std::vector<std::string>& )>        CommandFn;
typedef std::pair<CommandIdentifier, CommandFn>                      CommandEntry;
}



namespace blacksilk {
namespace cmd {

template < class _t_any >
std::string toStringAny( const _t_any& val ) {
    std::stringstream ss;
    ss << val;
    return ss.str();
}
std::string toString( const std::string& value ) { return value; }
std::string toString( const bool& value ) { return toStringAny( value ); }
std::string toString( const int& value ) { return toStringAny( value ); }
std::string toString( const unsigned int& value ) { return toStringAny( value ); }
std::string toString( const float& value ) { return toStringAny( value ); }
std::string toString( const short& value ) { return toStringAny( value ); }
std::string toString( const unsigned short& value ) { return toStringAny( value ); }
std::string toString( const char& value ) { return toStringAny( value ); }
std::string toString( const unsigned char& value ) { return toStringAny( value ); }
template < class _t_any, size_t _v_count >
std::string toString( const libgraphics::BasicPixel<_t_any, _v_count>& val ) {
    std::stringstream ss;
    ss << "Pixel(";

    for( size_t i = 0; _v_count > i; ++i ) {
        ss << val.Values[i];

        if( i + 1 < _v_count ) {
            ss << ",";
        }
    }

    ss << ")";
    return ss.str();
}
template  < class _t_any >
std::string toString( const libgraphics::Point<_t_any>& val ) {
    std::stringstream ss;
    ss << "Point(" << val.x << "," << val.y << ")";
    return ss.str();
}
template < class _t_any >
std::string toString( const libgraphics::Line<_t_any>& val ) {
    std::stringstream ss;
    ss << "Line(" << val.begin.x << "," << val.begin.y << "," << val.end.x << "," << val.end.y << ")";
    return ss.str();
}
template < class _t_any >
std::string toString( const libgraphics::Rect<_t_any>& val ) {
    std::stringstream ss;
    ss << "Rect(" << val.x << "," << val.y << "," << val.width << "," << val.height << ")";
    return ss.str();
}

template < class _t_map >
void print_preset_map( const _t_map& map ) {
    for( auto it = map.begin(); it != map.end(); ++it ) {
        if( !( *it ).first.empty() ) {
            std::cout << "\t" << ( *it ).first << ": " << toString( ( *it ).second.value ) << std::endl;
        }
    }
}
template < class _t_map >
void print_preset_color_map( const _t_map& map ) {
    for( auto it = map.begin(); it != map.end(); ++it ) {
        if( !( *it ).first.empty() ) {
            typedef typename std::remove_cv< decltype( ( *it ).second.value ) >::type t;

            std::cout << "\t" << ( *it ).first << ": " << toString<typename t::PixelType, t::Count>( ( *it ).second.value ) << std::endl;
        }
    }
}
bool print_presets( const std::vector<std::string>& str ) {
    ( void )str;

    std::cout << std::endl;
    std::cout << "Dumping internal preset collection..." << std::endl;
    std::cout << std::endl;

    for( auto it = theApp()->currentSession->presets()->begin(); it != theApp()->currentSession->presets()->end(); ++it ) {
        std::cout << "Filter Name: " << ( *it ).preset.filterName().c_str() << std::endl;
        std::cout << "Preset Name: " << ( *it ).preset.name().c_str() << std::endl;

        print_preset_map( ( *it ).preset.positions() );
        print_preset_map( ( *it ).preset.lines() );
        print_preset_map( ( *it ).preset.rects() );
        print_preset_map( ( *it ).preset.floats() );
        print_preset_map( ( *it ).preset.ints() );
        print_preset_map( ( *it ).preset.chars() );
        print_preset_map( ( *it ).preset.uints() );
        print_preset_map( ( *it ).preset.switches() );
        print_preset_map( ( *it ).preset.strings() );
        print_preset_color_map( ( *it ).preset.rgb8s() );
        print_preset_color_map( ( *it ).preset.rgb16s() );
        print_preset_color_map( ( *it ).preset.argb8s() );
        print_preset_color_map( ( *it ).preset.argb16s() );
        print_preset_color_map( ( *it ).preset.mono8s() );
        print_preset_color_map( ( *it ).preset.mono16s() );
        print_preset_map( ( *it ).preset.points() );


        std::cout << "*****************************" << std::endl;
    }

    return true;
}
bool print_help( const std::vector<std::string>& str );

bool get_argument( const std::vector<std::string>& arguments, const std::string& argumentName, const std::string& argumentShortcut, std::string& value ) {
    for( auto it = arguments.begin(); it != arguments.end(); ++it ) {
        if( libcommon::stringutils::iequals( ( *it ), argumentName ) || libcommon::stringutils::iequals( ( *it ), argumentShortcut ) ) {
            if( it + 1 != arguments.end() ) {
                ++it;

                const std::string& currentValue( *it );

                if( ( currentValue.find( "--" ) != 0 ) && ( currentValue.find( "-" ) != 0 ) ) {
                    value = currentValue;
                }

                return true;
            }
        }
    }

    return false;
}
bool is_option( const std::string& opt ) {
    size_t count( 0 );

    for( auto it = opt.begin(); it != opt.end(); ++it ) {
        if( ( ( *it ) == ' ' ) || ( ( *it ) == '\t' ) ) {
            continue;
        }

        if( ( *it ) == '-' ) {
            if( count == 0 ) {
                return true;
            }
        }

        ++count;
    }

    return false;
}

bool get_arguments( const std::vector<std::string>& arguments, const std::string& argumentName, const std::string& argumentShortcut, std::vector<std::string>& values,
                    size_t maxCount = 0 ) {

    for( auto it = arguments.begin(); it != arguments.end(); ++it ) {
        if( libcommon::stringutils::iequals( ( *it ), argumentName ) || libcommon::stringutils::iequals( ( *it ), argumentShortcut ) ) {
            if( it + 1 != arguments.end() ) {
                it = it + 1;

                for( auto st = it; st != arguments.end(); ++st ) {
                    const std::string& currentValue( ( *st ) );

                    if( ( values.size() >= maxCount ) && ( maxCount != 0 ) ) {
                        break;
                    }

                    if( !is_option( currentValue ) ) {
                        values.push_back( currentValue );
                    } else {
                        break;
                    }
                }

                break;
            } else {
                break;
            }
        }
    }

    return ( values.size() > 0 );
}
bool get_filenames( const std::vector<std::string>& arguments, std::vector<std::string>& values ) {
    for( auto it = arguments.begin(); it != arguments.end(); ++it ) {
        const std::string currentValue( *it );

        if( !is_option( currentValue ) ) {
            values.push_back( currentValue );
        } else {
            break;
        }
    }

    return ( values.size() > 0 );
}

std::string getOutputFileName( const std::string& inputFilename, const std::string& _postfix = "_filtered" ) {
    QFileInfo fi( inputFilename.c_str() );
    const std::string outputFileName = fi.absolutePath().toStdString() + "/" + fi.completeBaseName().toStdString() + _postfix + "." + fi.suffix().toStdString();
    return outputFileName;
}
std::string getOutputFileNameWithDir( const std::string& inputFilename, std::string subfolder, const std::string& _postfix = "_filtered" ) {
    assert( !inputFilename.empty() );
    assert( !subfolder.empty() );

    QFileInfo fi( inputFilename.c_str() );

    if( ( subfolder.back() != '/' ) && ( subfolder.back() != '\\' ) ) {
        subfolder += "/";
    }

    const std::string outputFileName = fi.absolutePath().toStdString() + "/" + subfolder + fi.completeBaseName().toStdString() + _postfix + "." + fi.suffix().toStdString();
    return outputFileName;
}
libfoundation::app::EImageFormat::t getFileFormatBySuffix( const char* _suffix ) {
    assert( _suffix != nullptr );

    if( _suffix == nullptr ) {
        return libfoundation::app::EImageFormat::Unknown;
    }

    if( *_suffix == '.' ) {
        ++_suffix;
    }

    std::string str_suffix( _suffix );

    if( libcommon::stringutils::iequals( str_suffix, LIBGRAPHICS_IO_FORMAT_BMP ) ) {
        return libfoundation::app::EImageFormat::BMP;
    } else if( libcommon::stringutils::iequals( str_suffix, LIBGRAPHICS_IO_FORMAT_JPEG ) ) {
        return libfoundation::app::EImageFormat::JPEG;
    } else if( libcommon::stringutils::iequals( str_suffix, LIBGRAPHICS_IO_FORMAT_PNG ) ) {
        return libfoundation::app::EImageFormat::PNG;
    } else if( libcommon::stringutils::iequals( str_suffix, LIBGRAPHICS_IO_FORMAT_TIF ) ) {
        return libfoundation::app::EImageFormat::TIF;
    }

    return libfoundation::app::EImageFormat::Unknown;
}


bool render_preset( const std::vector<std::string>& args ) {
    std::vector<std::string>    filenames; /** input images **/
    const bool specifiedFilenames = get_filenames( args, filenames );
    assert( specifiedFilenames );

#if LIBGRAPHICS_DEBUG_OUTPUT

    for( auto it = filenames.begin(); it != filenames.end(); ++it ) {
        std::cout << "Specified filename: " << ( *it ).c_str() << std::endl;
    }

#endif

    if( specifiedFilenames ) {
        std::cout << "Error: You need to specify a source image." << std::endl;
        print_help( args );
        return false;
    }

    std::string destinationPath;
    bool        isDestinationDir( false );
    const auto specifiedDestinationPath = get_argument(
            args,
            "--output",
            "-o",
            destinationPath
                                          );

    if( !specifiedDestinationPath || destinationPath.empty() ) {
        destinationPath = ""; /// empty, no dir
    } else {
        if( filenames.size() > 1 ) {
            QFileInfo fi( destinationPath.c_str() );
            bool err( false );

            if( !fi.isDir() && fi.exists() ) {
                std::cout << "Error: You need to specifiy a directory when filtering multiple files. The specified path does not point to a directory." << std::endl;
                err = true;
            } else if( !fi.exists() ) {
                QDir dir( "" );
                dir.mkdir( destinationPath.c_str() );

                fi.refresh();

                if( !fi.isDir() || !fi.exists() ) {
                    std::cout << "Failed to create directory. Specify another path." << std::endl;
                    err = true;
                }
            }

            isDestinationDir = true;

            if( err ) {
                ( void ) print_help( args );
                return false;
            }
        }
    }

    std::string                 presetData;
    const auto specifiedPresetData = get_argument(
                                         args,
                                         "--preset",
                                         "-p",
                                         presetData
                                     );
    assert( specifiedPresetData );
    assert( !presetData.empty() );

    if( !specifiedPresetData || presetData.empty() ) {
        std::cout << "Error: You need to specify a preset(path or string)." << std::endl;
        print_help( args );
        return false;
    }

    bool isFile( false );
    QFileInfo fi( presetData.c_str() );

    if( fi.isFile() && fi.exists() ) {
        isFile = true;
    }

    if( isFile ) {
        if( !theApp()->loadPresetFromPath( presetData ) ) {
            std::cout << "Error: Failed to load preset from specified path." << std::endl;
            return false;
        }
    } else {
        std::cout << "Info: Argument not an existing file, trying to parse as preset." << std::endl;
        libgraphics::FilterPresetCollection   presets;

        if( !blacksilk::createFilterPresetCollection( theApp()->currentSession->presets(), presets, presetData ) ) {
            std::cout << "Error: Invalid preset. Please correct the preset data." << std::endl;
            print_help( args );
            return false;
        }

        theApp()->usePresets(
            presets
        );
    }


    for( size_t i = 0; filenames.size() > i; ++i ) {
        const std::string currentInput( filenames[i] );
        QFileInfo inputInfo( currentInput.c_str() );

        if( !inputInfo.exists() ) {
            qDebug() << "Error: Path does not exist - " << inputInfo.filePath();
            continue;
        }

        if( !destinationPath.empty() && isDestinationDir ) {
            if( ( destinationPath.back() != '/' ) || ( destinationPath.back() != '\\' ) ) {
                destinationPath += "/";
            }
        }


        if( inputInfo.isDir() ) {
            QDir dir( currentInput.c_str(), "*", QDir::Name, QDir::Files );
            QFileInfoList fileInfoList = dir.entryInfoList( QDir::Files, QDir::Name );

            for( int j = 0; fileInfoList.size() > j; ++j ) {
                QFileInfo currentEntry( fileInfoList.at( j ) );

                QString absolutePath( currentEntry.absoluteFilePath() );
                QFileInfo info( absolutePath );

                if( !info.exists() ) {
                    qDebug() << "Error: File does not exist - " << absolutePath;
                    continue;
                }

                libfoundation::app::EImageFormat::t imageFormat( getFileFormatBySuffix( ( char* )inputInfo.suffix().toStdString().c_str() ) );

                if( imageFormat == libfoundation::app::EImageFormat::Unknown ) {
                    imageFormat = libfoundation::app::EImageFormat::JPEG;
                    std::cout << "Warning: Couldnt query image format from original. Format set to default(JPEG)." << std::endl;
                }

                if( theApp()->currentSession->importImageFromPath( absolutePath.toStdString() ) ) {
                    const std::string stdAbsPath = absolutePath.toStdString();
                    const std::string output = destinationPath.empty() ? getOutputFileName( stdAbsPath ) : ( isDestinationDir ? getOutputFileNameWithDir( stdAbsPath, destinationPath ) : destinationPath );
                    theApp()->currentSession->exportImage(
                        output,
                        imageFormat,
                        false,
                        nullptr,
                        !useGpuRendering
                    );
                }

            }
        } else {

            if( theApp()->currentSession->importImageFromPath( currentInput ) ) {
                if( theApp()->filterCascadedSharpen != nullptr ) {
                    theApp()->filterCascadedSharpen->updateCascades();
                }

                libfoundation::app::EImageFormat::t imageFormat( getFileFormatBySuffix( ( char* )inputInfo.suffix().toStdString().c_str() ) );

                if( imageFormat == libfoundation::app::EImageFormat::Unknown ) {
                    imageFormat = libfoundation::app::EImageFormat::JPEG;
                    std::cout << "Warning: Couldnt query image format from original. Format set to default(JPEG)." << std::endl;
                }

                std::cout << "SUFFIX: " << inputInfo.suffix().toStdString() << std::endl;

                const std::string output = destinationPath.empty() ? getOutputFileName( currentInput ) : ( isDestinationDir ? getOutputFileNameWithDir( currentInput, destinationPath ) : destinationPath );

                theApp()->currentSession->exportImage(
                    output,
                    imageFormat,
                    false,
                    nullptr,
                    !useGpuRendering
                );
            }
        }

        theApp()->filterFilmGrain->resetCurve();
        theApp()->filterFilmGrain->resetGrain();
        theApp()->filterCascadedSharpen->deleteBlurBuffersForBackend( FXAPI_BACKEND_OPENGL );
        theApp()->filterCascadedSharpen->deleteBlurBuffersForBackend( FXAPI_BACKEND_CPU );
    }

    return true;
}

}
}

static const blacksilk::CommandEntry    commandEntries[] = {
    blacksilk::CommandEntry( std::make_pair( CommandIdentifier( "--help", "-h", "Prints out the summary of all commands." ), blacksilk::cmd::print_help ) ),
    blacksilk::CommandEntry( std::make_pair( CommandIdentifier( "--print-presets", "", "Prints the internal preset collection." ), blacksilk::cmd::print_presets ) ),
    blacksilk::CommandEntry( std::make_pair( CommandIdentifier( "--preset", "-p", "Filters the specified images using the specified preset. The preset can be defined either by a string"
                             "\n\t or by a file. Use --output,-o to specify an output file name. When filtering multiple images, --output,-o"
                             "\n\t declares a destination directory(BlackSilk automatically appends the postfix 'filtered' to the rendered images). "
                             "\n\t    BlackSilk.exe MyImage.jpeg --preset \"bwmixer=highlights(120,120,100),shadows(120,50,120),weight(30)\" MyImage.jpeg "
                             "\n\t      "
                             "\n\t    BlackSilk.exe MyImage.jpeg --preset \"bwmixer=average,splittone=sepia\" --output Filtered.jpeg "
                             "\n\t    BlackSilk.exe MyImage.jpeg --preset \"bwmixer=average,splittone=sepia\" -o Filtered.jpeg "
                             "\n\t    BlackSilk.exe MyImages/ --preset \"bwmixer=average,splittone=sepia\" "
                             "\n\t    BlackSilk.exe MyImage.jpeg --preset mypreset.bs " ), blacksilk::cmd::render_preset ) ),
    blacksilk::CommandEntry( std::make_pair( CommandIdentifier( "--output", "-o", "Saves the filtered image to the specified path See --preset,-p for more information." ), blacksilk::cmd::render_preset ) ),
};
static const size_t                     commandEntriesLen = sizeof( commandEntries ) / sizeof( blacksilk::CommandEntry );

namespace blacksilk {
namespace cmd {
bool print_help( const std::vector<std::string>& str ) {
    ( void )str;

    std::cout << std::endl;
    std::cout << "Summary of all commands: " << std::endl;

    for( size_t i = 0; commandEntriesLen > i; ++i ) {

        std::cout << "\t" << commandEntries[i].first.full;

        if( !commandEntries[i].first.shortcut.empty() ) {
            std::cout << ", " << commandEntries[i].first.shortcut;
        }

        std::cout << ": " << commandEntries[i].first.desc << std::endl;
    }

    return true;
}
}
}

void printVersion() {
    std::stringstream ss;
    ss << BLACKSILK_VERSION_MAJOR;
    ss << ".";
    ss << BLACKSILK_VERSION_MINOR;
    ss << ".";
    ss << BLACKSILK_VERSION_RELEASE;

    const std::string versionStr = ss.str();

#ifdef BLACKSILK_TEST_SUITE
    qDebug() << "Black Silk Test Suite - version" << versionStr.c_str() << "-";
#else
#ifdef BLACKSILK_STANDALONE
    qDebug() << "Black Silk Standalone - version" << versionStr.c_str() << "-";
#else
    qDebug() << "Black Silk Plugin     - version" << versionStr.c_str() << "-";
#endif
#endif

    qDebug() << "Build Date: " << __DATE__ << "-" << __TIME__;
}

// workaround for QTBUG-52954
static void initializeQOpenGLContext() {
    LOG( "Initializing QOpenGLContext" );
    QOpenGLContext context;
    context.create();
}

// Q_COREAPP_STARTUP_FUNCTION( initializeQOpenGLContext )

int main( int argc, char* argv[] ) {
    // log qt opengl stuff
    QLoggingCategory::setFilterRules( QStringLiteral( "qt.qpa.gl=true" ) );

    qInstallMessageHandler( logging::customMessageHandler );
    printVersion();

    QApplication::setAttribute( Qt::AA_UseHighDpiPixmaps );
    QApplication::setAttribute( Qt::AA_UseDesktopOpenGL );

    QApplication a( argc, argv );

    QApplication::setOrganizationName( "FD Imaging" );
    QApplication::setOrganizationDomain( "fd-imaging.com" );
    QApplication::setApplicationName( "blacksilk" );


    bool run = true;
    bool display = true;

    initializeApplication();

    std::vector<std::string>    args;
    {
        for( size_t i = 0; ( size_t )argc > i; ++i ) {
            args.push_back( argv[i] );
        }
    }
    assert( !args.empty() );
    args.erase( args.begin() );

    if( args.size() > 0 ) {
        LOG( "Entering command line mode" );
        initializeCommandLine();

        for( size_t i = 0; commandEntriesLen > i; ++i ) {
            for( size_t j = 0; args.size() > j; ++j ) {
                if( libcommon::stringutils::iequals( args[j], commandEntries[i].first.full ) || libcommon::stringutils::iequals( args[j], commandEntries[i].first.shortcut ) ) {
                    const bool suc = commandEntries[i].second( args );
                    ( void )suc;
                    shutdownCommandLine();
                    LOG( "Exiting command line mode" );
                    return 0;
                }
            }
        }

        cmd::print_help( args );

        shutdownCommandLine();
        LOG( "Exiting command line mode" );
        return 0;
    }

    // Update and language settings dialog
    MainWindow mainwindow( theApp()->currentSession, &run, &display );
    mainwindow.showMaximized();

    // draw the sample flower
    mainwindow.openSample();

    return a.exec();
}
