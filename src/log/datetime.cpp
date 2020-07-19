#include "datetime.hpp"

#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>

std::string datetime::now() {

    std::stringstream now;

    auto tp = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>( tp.time_since_epoch() );
    size_t modulo = ms.count() % 1000;

    time_t seconds = std::chrono::duration_cast<std::chrono::seconds>( ms ).count();

    now << std::put_time( std::localtime( &seconds ), "%T." );

    // ms
    now.fill( '0' );
    now.width( 3 );
    now << modulo;

    return now.str();
}

int datetime::compilationYear() {
    int year = 0;
    std::stringstream ss( __DATE__ );
    std::string tmp;
    ss >> tmp;
    ss >> tmp;
    ss >> year;
    return year;
}
