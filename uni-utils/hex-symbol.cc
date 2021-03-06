//
// Function to create the UTF-8 string from an index into a set of
//   Unicode code points stored in an array.  The function is simple
//   since the array of code points are all in the range that a three
//   character UTF-8 string is required, and they are all valid code
//   points.  Thus verification, validation and UTF-8 structure
//   determination is not necessary here.
//
//    Copyright (C) 2022  George Ganoe
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// Use the following commands to build the executable and add it to the library:
//   g++ -std=c++17 -c hex-symbol.cc
//   ar -Prs ~/data/lib/libfoutil.a hex-symbol.o
//

#include "hex-symbol.h"

std::string hex_symbol( uint16_t idx, bool use_ctl_pics )
{
    // Initialize the string to illegal characters in case the parameter
    //   is out of range
    std::string u8st( 3, static_cast<char>( 0x0080 ) );
    if ( idx < 256 )
    {
        uint16_t ch3;
        if ( use_ctl_pics && idx < 32 )
          ch3 = control_pics[ idx ];
        else
          ch3 = hex_byte_dots[ idx ];
        uint16_t ch2 = 0x0080 + (( ch3 >> 6 ) & 0x003f);
        uint16_t ch1 = 0x00e0 + (( ch3 >> 12 ) & 0x000f);
        ch3 = 0x0080 + ( ch3 & 0x003f );
        u8st[ 0 ] = static_cast<char>(ch1);
        u8st[ 1 ] = static_cast<char>(ch2);
        u8st[ 2 ] = static_cast<char>(ch3);
    }
    return u8st;
}
