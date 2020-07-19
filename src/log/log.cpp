#include "log.hpp"
#include "datetime.hpp"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <string>
#include <thread>
#include <mutex>
#include <cstdlib>
#include <regex>

#ifdef WIN32
#include <Windows.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#if LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_LINUX
#include <unistd.h>
#include <sys/syscall.h>
#define gettid() syscall(SYS_gettid)
#endif
#endif

static std::mutex mutex;

std::string logging::logFilename() {

#ifdef WIN32
    std::string filename = std::string( getenv( "TEMP" ) ) + "\\BlackSilk.log";
#elif __APPLE__
    std::string filename = std::string( getenv( "HOME" ) ) + "/Library/Caches/com.fd-imaging.blacksilk/BlackSilk.log";
#else
    std::string filename( "/tmp/BlackSilk.log" );
#endif // WIN32

    return filename;
}

static void rotateLog() {
    std::string filename = logging::logFilename();
    std::string backup1  = filename + ".1";
    std::string backup2  = filename + ".2";

    std::remove( backup2.c_str() );
    std::rename( backup1.c_str(),  backup2.c_str() );
    std::rename( filename.c_str(), backup1.c_str() );
}

void initLogFile() {
    static bool initialized = false;

    if( !initialized ) {
        initialized = true;

#if __APPLE__
        std::string cache = std::string( getenv( "HOME" ) ) + "/Library/Caches/com.fd-imaging.blacksilk";
        mkdir( cache.c_str(), 0777 );
#endif
        rotateLog();
    }
}

int threadId() {
#if _WIN32
    int tid = ::GetCurrentThreadId();
#else
    int tid = ::gettid();
#endif
    return tid;
}

std::string indent( const std::string& s, int count ) {
    std::stringstream ret;
    std::stringstream ss( s );
    std::string item;
    std::string whitespaces( count, ' ' );

    if( std::getline( ss, item ) ) {
        ret << item << std::endl;

        while( std::getline( ss, item ) ) {
            ret << whitespaces << item << std::endl;
        }
    }

    return ret.str();
}


bool logging::writeLog( const char* level, const char* cfile, int sourceline, const char* function, std::string content ) {

    std::stringstream logline;

    // time
    logline.width( 6 );
    logline << datetime::now();
    logline << level;
    logline << " T";
    logline.width( 4 );
    logline << threadId() << " ";

    std::string file( cfile );
#ifndef _WIN32
    file = file.substr( file.find_last_of( "/" ) + 1 );
#else
    file = file.substr( file.find_last_of( "\\" ) + 1 );
#endif // _WIN32
    file = file.substr( 0, file.find_last_of( "." ) );

    logline.width( 20 );
    logline << file << " ";

    logline.width( 60 );
    logline << function << "(";
    logline.width( 4 );
    logline.setf( std::ios::right );
    logline << sourceline << "): ";

    // indent paragraph by current size
    logline.seekg( 0, std::ios::end );
    logline << indent( content, logline.tellg() );

#ifndef _DEBUG
    std::lock_guard<std::mutex> lock( mutex );
#endif

#ifdef _DEBUG
    std::cout << logline.str();
#endif // _DEBUG

    initLogFile();

    std::ofstream ofs( logFilename(), std::ios::out | std::ios::app );

    if( ofs.is_open() ) {
        ofs << logline.str() << std::flush;

        return true;
    } else {
        return false;
    }
}

#ifdef QT_CORE_LIB

void logging::customMessageHandler( QtMsgType type, const QMessageLogContext& context, const QString& message ) {

    const char* cfile     = context.file ? context.file : "Qt";
    const char* cfunction = context.function ? context.function : "Qt";

    // remove class and arguments
    std::string function( cfunction );

    /** slows down debug mode by multiple magnitudes **/
    /*
    #ifdef _WIN32
    function = std::regex_replace( function, std::regex( "\\<.*\\>.*" ), "[]{}" ); // shorten lambdas
    function = std::regex_replace( function, std::regex( "\\(.*\\)" ), "" );       // clean arguments
    function = std::regex_replace( function, std::regex( ".*\\s{1}" ), "" );       // clean return values
    function = std::regex_replace( function, std::regex( ".*::" ), "" );           // clean namespaces
    #endif*/

    switch( type ) {
        case QtDebugMsg:
            LOGQT( LEVEL_INFO, message.toStdString(), cfile, context.line, function.c_str() );
            break;

        case QtInfoMsg:
            LOGQT( LEVEL_INFO, message.toStdString(), cfile, context.line, function.c_str() );
            break;

        case QtWarningMsg:
            LOGQT( LEVEL_WARNING, message.toStdString(), cfile, context.line, function.c_str() );
            break;

        case QtCriticalMsg:
            LOGQT( LEVEL_ERROR, message.toStdString(), cfile, context.line, function.c_str() );
            break;

        case QtFatalMsg:
            LOGQT( LEVEL_ERROR, message.toStdString(), cfile, context.line, function.c_str() );
    }

    if( type == QtFatalMsg ) {
        abort();
    }
}

#endif // QT_CORE_LIB
