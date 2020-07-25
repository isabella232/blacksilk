#include <fstream>
#include <libcommon/fileutils.hpp>

bool libcommon::fileutils::fileExists( const std::string& filename ) {
    std::ifstream file( filename.c_str() );
    return file.is_open();
}

std::string libcommon::fileutils::fromFile( const std::string& filename ) {
    std::ifstream file( filename.c_str(), std::ios_base::in | std::ios_base::binary );

    file.seekg( 0, std::ios::end );
    size_t size = ( size_t ) file.tellg();
    file.seekg( 0, std::ios::beg );

    std::string buffer( size, '\0' );
    file.read( buffer.data(), buffer.size() );

    return buffer;
}


bool libcommon::fileutils::toFile( const std::string& filename, const std::string& data ) {
    std::ofstream file( filename.c_str(), std::ios_base::out | std::ios_base::binary );

    if( file.is_open() ) {
        file.write( data.data(), data.size() );
        return true;
    }

    return false;
}
