#include <fstream>
#include <libcommon/fileutils.hpp>

bool libcommon::fileutils::fileExists( const std::string& filename ) {
    std::ifstream file( filename.c_str() );
    return file.is_open();
}

std::string libcommon::fileutils::fromFile( const std::string& filename ) {
    std::ifstream file( filename.c_str(), std::ios_base::in | std::ios_base::binary );
    return std::string( ( std::istreambuf_iterator<char>( file ) ), ( std::istreambuf_iterator<char>() ) );
}


bool libcommon::fileutils::toFile( const std::string& filename, const std::string& data ) {
    std::ofstream file( filename.c_str(), std::ios_base::out | std::ios_base::binary );

    if( file.is_open() ) {
        file << data;
        return true;
    }

    return false;
}
