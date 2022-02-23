//
// This provides the code to implement some useful Unicode handling
//   utilities.  So far, two utilities have been included, but more may
//   be coming in the future.
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
//
// Use the following command to build this object:
//   g++ -std=c++17 -c uni-utils.cc
//   ar -Prs ~/data/lib/libfoutil.a uni-utils.o
//   ar -Ptv ~/data/lib/libfoutil.a

#include "uni-utils.h"
// ??  #include "../unicode-utils/hex-symbol.h"

vector<code_point_cmpct> code_point_cmpct::cpc_db = {};
bool code_point_cmpct::cpc_init = false;
uint16_t code_point_cmpct::cod_pt_lo16 = 0;
uint16_t code_point_cmpct::cod_pt_hi16 = 0;

code_point_cmpct::code_point_cmpct()
{
    combining = 0;
    double_width = 0;
    right_to_left = 0;
    plane_num = 0;
    cod_pt_lsb = 0;
    cod_pt_2sb = 0;
    cod_pt_lo16 = 0;
    cod_pt_hi16 = 0;
    if ( !cpc_init )
    {
        cpc_db.reserve( 100 );
        cpc_init = true;
        cpc_db.push_back( *this );
    }
}

//
// Define the methods for struct code_point_cmpct
uint32_t code_point_cmpct::Unicode_val()
{
    uint32_t cdpt = plane_num;
    cod_pt_lo16 = cod_pt_2sb;
    cod_pt_lo16 = ( cod_pt_lo16 << 8 ) + cod_pt_lsb;
    cdpt <<= 16;
    cdpt += cod_pt_lo16;
    return cdpt;
}

bool code_point_cmpct::is_combining()
{
    return combining;
}

bool code_point_cmpct::is_double_width()
{
    return double_width;
}

bool code_point_cmpct::is_right_to_left()
{
    return right_to_left;
}

bool code_point_cmpct::u32lookup( uint32_t codpt )
{
    bool cpc_found = false;
    cod_pt_lo16 = codpt & 0x0ffff;
    cod_pt_hi16 = codpt >> 16;
    if ( cod_pt_hi16 > 16 )
      *this = cpc_db[ 0 ];
    else
    {
        for ( auto& cpc_item : cpc_db )
        {
            uint16_t tst_lo16 = cpc_item.cod_pt_2sb;
            tst_lo16 = ( tst_lo16 << 8 ) + cpc_item.cod_pt_lsb;
            if ( cod_pt_hi16 == cpc_item.plane_num &&
              cod_pt_lo16 == tst_lo16 )
            {
                *this = cpc_item;
                cpc_found = true;
                break;
            }
        }
        if ( !cpc_found ) *this = cpc_db[ 0 ];
    }
    return cpc_found;
}

bool is_ch_set_valid_utf8( string ch_set2validate )
{
  // The string should contain one valid UTF-8 character.  This function
  //   will return true if the character is valid, otherwise false
  // UTF-8 conversion from Unicode code point
  //   ASCII codes U+0000 to U+007f -> 0x00 to 0x7f respectively
  //
  // The two character set consists of the code points U+0080 to U+07ff
  //   and has codes:   1100 0020  1000 0000 to 1101 1111  1011 1111
  //   The bits of the code point fill the 'b' positions of the sample below
  //   from right to left.  Maximum of 3 significant hex digits can be
  //   encoded into two UTF-8 positions
  //     Examples:                 110b bbbb  10bb bbbb
  //     bit columns hex char pos ->  1 1122    22 3333
  //   U+00a2 -> 000 1010 0010 ->  1100 0010  1010 0010 -> 0xc2 0xa2 ¢
  //   U+00bd -> 000 1011 1101 ->  1100 0010  1011 1101 -> 0xc2 0xbd ½
  //   U+00f1 -> 000 1111 0001 ->  1100 0011  1011 0001 -> 0xc3 0xb1 ñ
  //   U+0394 -> 011 1001 0100 ->  1100 1110  1001 0100 -> 0xce 0x94 Δ
  //   U+03a3 -> 011 1010 0011 ->  1100 1110  1010 0011 -> 0xce 0xa3 Σ
  //   U+03b1 -> 011 1011 0001 ->  1100 1110  1011 0001 -> 0xce 0xb1 α
  //   U+03c0 -> 011 1100 0000 ->  1100 1111  1000 0000 -> 0xcf 0x80 π
  //   U+0416 -> 100 0001 0110 ->  1101 0000  1001 0110 -> 0xd0 0x96 Ж
  //
  // The three character set consists of code points U+0800 to U+fffd
  //   and has codes:
  //              1110 bbbb  10bb bbbb  10bb bbbb
  //     Examples:     1111    22 2233    33 4444 <- bit columns hex char pos
  //   U+0800 ->  1110 0000  1010 0000  1000 0000 -> 0xe0 0xa0 0x80
  //   U+0e01 ->  1110 0000  1011 1000  1000 0001 -> 0xe0 0xb8 0x81 ก
  //   U+0e3f ->  1110 0000  1011 1000  1011 1111 -> 0xe0 0xb8 0xbf ฿
  //   U+0e53 ->  1110 0000  1011 1001  1001 0011 -> 0xe0 0xb9 0x93 ๓
  //   U+1780 ->  1110 0001  1001 1110  1000 0000 -> 0xe1 0x9e 0x80 ក
  //   U+201c ->  1110 0010  1000 0000  1001 1100 -> 0xe2 0x80 0x9c “
  //   U+201d ->  1110 0010  1000 0000  1001 1101 -> 0xe2 0x80 0x9d ”
  //   U+20ac ->  1110 0010  1000 0010  1010 1100 -> 0xe2 0x82 0xac €
  //   U+23da ->  1110 0010  1000 1111  1001 1010 -> 0xe2 0x8f 0x9a ⏚
  //   U+23fb ->  1110 0010  1000 1111  1011 1011 -> 0xe2 0x8f 0xbb ⏻
  //   U+2423 ->  1110 0010  1001 0000  1010 0011 -> 0xe2 0x90 0xa3 ␣
  //   U+25cf ->  1110 0010  1001 0111  1000 1111 -> 0xe2 0x97 0x8f ●
  //   U+2654 ->  1110 0010  1001 1001  1001 0100 -> 0xe2 0x99 0x94 ♔
  //   U+4e3d ->  1110 0100  1011 1000  1011 1101 -> 0xe4 0xb8 0xbd 丽
  // Next 4 are surrogates; high surrogate lower and upper bounds
  //   U+d800 ->  1110 1101  1010 0000  1000 0000 -> 0xed 0xa0 0x80
  //   U+dbff ->  1110 1101  1010 1111  1011 1111 -> 0xed 0xaf 0xbf
  //                        low surrogate lower and upper bounds
  //   U+dc00 ->  1110 1101  1011 0000  1000 0000 -> 0xed 0xa0 0x80
  //   U+dfff ->  1110 1101  1011 1111  1011 1111 -> 0xed 0xaf 0xbf
  //
  //   U+fb01 ->  1110 1111  1010 1100  1000 0001 -> 0xef 0xac 0x81 ﬁ
  //   U+fffd ->  1110 1111  1011 1111  1011 1101 -> 0xef 0xbf 0xbd �
  //     Not a character
  //   U+ffff ->  1110 1111  1011 1111  1011 1111 -> 0xef 0xbf 0xbf ￿
  //
  // The four character set consists of the code points U+10000 to U+10ffff
  //   and has codes:
  //             1111 0bbb  10bb bbbb  10bb bbbb  10bb bbbb
  //     Examples:     122    22 3333    44 4455    55 6666 <- hex bit cols
  // U+010000 -> 1111 0000  1001 0000  1000 0000  1000 0000 xf0 x90 x80 x80 𐀀
  // U+01d456 -> 1111 0000  1001 1101  1001 0001  1001 0110 xf0 x9d x91 x96 𝑖
  // U+01f377 -> 1111 0000  1001 1111  1000 1101  1011 0111 xf0 x9f x8d xb7 🍷
  // U+01f799 -> 1111 0000  1001 1111  1001 1110  1001 1001 xf0 x9f x9e x99 🞙
  // U+01fa02 -> 1111 0000  1001 1111  1010 1000  1000 0010 xf0 x9f xa8 x82 🨂
  // U+01fa7a -> 1111 0000  1001 1111  1010 1001  1011 1010 xf0 x9f xa9 xba 🩺
  // U+01fa81 -> 1111 0000  1001 1111  1010 1010  1000 0001 xf0 x9f xaa x81 🪁
  // U+01ffff -> 1111 0000  1001 1111  1011 1111  1011 1111 xf0 x9f xbf xbf
  // U+02000b -> 1111 0000  1010 0000  1000 0000  1000 1011 xf0 xa0 x80 x8b 𠀋
  // U+059aff -> 1111 0001  1001 1001  1010 1011  1011 1111 xf1 x99 xab xbf
  // U+09daff -> 1111 0010  1001 1101  1010 1011  1011 1111 xf2 x9d xab xbf
  // U+100000 -> 1111 0100  1000 0000  1000 0000  1000 0000 xf4 x80 x80 x80
  // U+10ffff -> 1111 0100  1000 1111  1011 1111  1011 1111 xf4 x8f xbf xbf
  //
    if ( ch_set2validate.size() == 2 )
    {
        uint16_t c1 = 0x00ff & ch_set2validate[0];
        uint16_t c2 = 0x00ff & ch_set2validate[1];

        // Logical complete test
        // if ( c1 >= 0x00c2 && c1 <= 0x00df &&
        //   c2 >= 0x0080 && c2 <= 0x00bf ) return true;
        //
        // But this one should do the same thing more efficiently
        if ( c1 >= 0x00c2 && c1 <= 0x00df &&
          ( c2 & 0xff40 ) == 0x0000 ) return true;
    }
    else if ( ch_set2validate.size() == 3 )
    {
        uint16_t c1 = 0x00ff & ch_set2validate[0];
        uint16_t c2 = 0x00ff & ch_set2validate[1];
        uint16_t c3 = 0x00ff & ch_set2validate[2];
        if ( c1 == 0x00e0 )
        {
            if ( c2 >= 0x00a0 && c2 <= 0x00bf && ( c3 & 0x00c0 ) == 0x0080 )
              return true;
        }
        else if ( c1 > 0x00e0 && c1 <= 0x00ef )
        {
            // exclude the surrogates area
            if ( c1 == 0x00ed && ( c2 >= 0x00a0 ) ) return false;
            // exclude invalid code points U+fffe and U+ffff
            if ( c1 == 0x00ef && ( c2 >= 0x00bf ) &&
              ( c3 == 0x00be || c3 == 0x00bf ) ) return false;
            if ( ( c2 & 0x00c0 ) == 0x0080 && ( c3 & 0x00c0 ) == 0x0080 )
              return true;
        }
    }
    else if ( ch_set2validate.size() == 4 )
    {
        uint16_t c1 = 0x00ff & ch_set2validate[0];
        uint16_t c2 = 0x00ff & ch_set2validate[1];
        uint16_t c3 = 0x00ff & ch_set2validate[2];
        uint16_t c4 = 0x00ff & ch_set2validate[3];
        if ( c1 == 0x00f0 && c2 < 0x0090 ) return false;
        if ( ( c2 & 0x00c0 ) == 0x0080 && ( c3 & 0x00c0 ) == 0x0080 &&
          ( c4 & 0x00c0 ) == 0x0080 )
        {
            if ( c1 >= 0x00f0 && c1 <= 0x00f3 ) return true;
            if ( c1 == 0x00f4 && c2 <= 0x008f ) return true;
        }
    }
    else if ( ch_set2validate.size() == 1 &&
      ( ch_set2validate[0] & 0x80 ) == 0 ) return true;
    return false;
}

//
// Tests if a 16 bit code point is in the known combining set.  Known is
//   derived over time by adding new ones as they occur.
uint16_t known_combining_chars[] {
    0x0e31, 0x0e34, 0x0e35, 0x0e36, 0x0e37, 0x0e38, 0x0e39, 0x0e3a,
    0x0e47, 0x0e48, 0x0e49, 0x0e4a, 0x0e4b, 0x0e4c, 0x0e4d, 0x0e4e
    // 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e
};
uint16_t num_combining = sizeof( known_combining_chars ) / sizeof( uint16_t );

bool is_cod_pt_combining( uint16_t tstch )
{
    bool in_cmb_set = false;
    // do test
    for ( int idx = 0; !in_cmb_set && idx < num_combining; idx++ )
    {
        if ( tstch == known_combining_chars[ idx ] ) in_cmb_set = true;
    }
    return in_cmb_set;
}

//
// Convert Unicode code point to a UTF-8 string
string U_code_pt_to_UTF_8( uint32_t code_id )
{
    string utf_str; // debug done -  = "String [";
    // If out of range, return the replacement character
    if ( code_id > 0x010ffff ) code_id = 0x0fffd;
    if ( code_id < 0x0080 )
    {
        // The string is just the single character expressed by the code point
        utf_str.push_back( static_cast<char>( code_id & 0x00ff ) );
    }
    else if ( code_id < 0x0800 )
    {
        // These characters require a string with two char values
        // debug done - utf_str.push_back( '2' );
        char u1 = 0xc0, u2 = 0x80;
        u2 |= static_cast<char>( code_id & 0x0003f );
        u1 |= static_cast<char>( ( code_id >> 6 ) & 0x0003f );
        utf_str.push_back( u1 );
        utf_str.push_back( u2 );
    }
    else if ( code_id < 0x010000 )
    {
        // These characters require a string with three char values and the
        //   surrogates need to be avoided
        // debug done - utf_str.push_back( '3' );
        char u1 = 0xe0, u2 = 0x80, u3 = 0x80;
        if ( code_id >= 0x0d800 && code_id <= 0x0dfff )
        {
            // debug done - utf_str.push_back( 'a' );
            // This is the surrogate range, return the replacement character
            //   U+FFFD as a substitute
            u1 = 0xef;
            u2 = 0xbf;
            u3 = 0xbd;
        }
        else
        {
            // debug done - utf_str.push_back( 'b' );
            // debug done - utf_str += to_string( code_id );
            // debug done - utf_str.push_back( ':' );
            u3 |= static_cast<char>( code_id & 0x0003f );
            u2 |= static_cast<char>( ( code_id >>= 6 ) & 0x0003f );
            u1 |= static_cast<char>( ( code_id >> 6 ) & 0x000ff );
        }
        utf_str.push_back( u1 );
        utf_str.push_back( u2 );
        utf_str.push_back( u3 );
    }
    else
    {
        // These characters require a string with four char values
        char u1 = 0xf0, u2 = 0x80, u3 = 0x80, u4 = 0x80;
        u4 |= static_cast<char>( code_id & 0x0003f );
        u3 |= static_cast<char>( ( code_id >>= 6 ) & 0x0003f );
        u2 |= static_cast<char>( ( code_id >>= 6 ) & 0x0003f );
        u1 |= static_cast<char>( ( code_id >> 6 ) & 0x00007 );
        utf_str.push_back( u1 );
        utf_str.push_back( u2 );
        utf_str.push_back( u3 );
        utf_str.push_back( u4 );
    }
    // debug done - utf_str.push_back( ']' );
    return utf_str;
}

