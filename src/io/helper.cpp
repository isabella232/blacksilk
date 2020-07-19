
#include <string>
#include <iostream>
#include <sstream>

#include "helper.h"

namespace blacksilk {

void writeRAW( unsigned char* data, int size, char* filename ) {
    std::ofstream out( filename, std::ios::out );

    if( out.is_open() ) {
        out.write( ( char* )data, size );
        out.close();
    }
}

float toFloat( std::string s ) {
    float f = 0.0;
    std::stringstream ss;
    ss << s;
    ss >> f;
    return f;
}

std::string fromPoints( const std::vector<float>& xs, const std::vector<float>& ys ) {
    std::stringstream stream;
    stream << "points=[ " ;

    for( size_t i = 0; i < xs.size(); i++ ) {
        stream << xs[i] << " " << ys[i];

        if( i + 1 != xs.size() ) {
            stream << ", ";
        } else {
            stream << " ";
        }
    }

    stream << "]";

    return stream.str();
}

void toPoints( std::string s, std::vector<float>& xs,  std::vector<float>& ys ) {
    s = trim( s );
    s.erase( 0, s.find_first_not_of( '[' ) + 1 );
    s.erase( s.find_last_not_of( ']' ) );

    std::vector<std::string> tokens = blacksilk::split( s, ',' );

    for( size_t i = 0; i < tokens.size(); i++ ) {
        std::vector<std::string> token = blacksilk::split( tokens[i], ' ' );

        if( token.size() == 2 ) {
            xs.push_back( blacksilk::toFloat( token[0] ) );
            ys.push_back( blacksilk::toFloat( token[1] ) );
        }
    }
}

std::string trim( std::string s ) {
    s.erase( s.find_last_not_of( " \n\r\t" ) + 1 );
    s.erase( 0, s.find_first_not_of( " \n\r\t" ) );
    return s;
}

// \sa http://oopweb.com/CPP/Documents/CPPHOWTO/Volume/C++Programming-HOWTO-7.html
std::vector<std::string> split( const std::string& input, const char separator, bool removeEmpty ) {

    std::vector<std::string> tokens;

    // Skip separator at beginning.
    std::string::size_type lastPos = input.find_first_not_of( separator, 0 );

    // Find first "non-delimiter".
    std::string::size_type pos     = input.find_first_of( separator, lastPos );

    while( std::string::npos != pos || std::string::npos != lastPos ) {

        // Found a token, add it to the vector.
        std::string token = input.substr( lastPos, pos - lastPos );
        token = trim( token );

        if( ! token.empty() && removeEmpty ) {
            tokens.push_back( token );
        }

        // Skip separator.  Note the "not_of"
        lastPos = input.find_first_not_of( separator, pos );

        // Find next "non-delimiter"
        pos = input.find_first_of( separator, lastPos );
    }

    return tokens;
}
}


