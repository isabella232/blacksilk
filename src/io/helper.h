//
//  helper.h
//  yuv
//
//  Created by Samuel Albrecht on 1/18/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//
#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

/// \begin
/// \brief  blacksilk main namespace
namespace blacksilk {

/// \brief  prints a backtrace
/// \param  function name
/// \return nothing
void print_trace( const char* func );

/// \brief  writes a raw data block
/// \param  data block
/// \param  length
/// \param  file name
/// \return nothing
void writeRAW( unsigned char* data, int size, char* filename );

std::string trim( std::string s );
float toFloat( std::string s );

std::string fromPoints( const std::vector<float>& xs, const std::vector<float>& ys );
void toPoints( std::string s, std::vector<float>& xs,  std::vector<float>& ys );

std::vector<std::string> split( const std::string&, const char seperator = ' ', bool removeEmpty = true );
}

