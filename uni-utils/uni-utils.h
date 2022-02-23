//
// This provides the declaration for some useful Unicode handling
//   utilities.  So far, two utilities have been included, but more may
//   be coming in the future.
//

#ifndef UNI_UTILS_H
#define UNI_UTILS_H

#include <string>
#include <vector>
#include <iostream>
#include <cstdint>

using namespace std;

#ifdef INdevel
    // Declarations/definitions/code for development only

#endif // #ifdef INdevel

//
// This structure declaration provides a compact way to specify Unicode
//   code points along with their side effects and characteristics.  It
//   uses the minimum feasible space to store each code point, thus can
//   be used as a base for programs that need to use a small subset of
//   the Unicode set while maintaining the needed information about the
//   code points being used.  This information currently consists of
//   whether or not the code point is a combining character which will
//   cause it to not need a character space in output, whether it is a
//   double width (also known as full width) character which will cause
//   it to require two normal character spaces, or is a right_to_left
//   encoding character which is used by arabic and other languages
//   where the writing is from the right side of the page to the left
//   rather than the English norm of writing from the left of the page
//   to the right.  The plane_num field is an indicator of the 16 bit
//   plane that the code point occupies where plane 0 is the basic
//   multilingual plane, and the numbers 1 to 16 are the supplementary
//   planes that comprise the rest of the possible values for valid
//   Unicode code points.  Finally the cod_pt_lo16 contains the rest of
//   the bits needed to define the code point.
struct code_point_cmpct {
    static vector<code_point_cmpct> cpc_db;
    static bool cpc_init;
    static uint16_t cod_pt_lo16;
    static uint16_t cod_pt_hi16;
    uint8_t combining : 1;
    uint8_t double_width : 1;
    uint8_t right_to_left : 1;
    uint8_t plane_num : 5;
    uint8_t cod_pt_lsb;
    uint8_t cod_pt_2sb;

    code_point_cmpct();
    uint32_t Unicode_val();
    bool is_combining();
    bool is_double_width();
    bool is_right_to_left();
    bool u32lookup( uint32_t codpt );
};

//
// This function tests a string containing the set of UTF-8 characters
//   intended to represent a single Unicode code point.  It is a quick
//   test to verify that a sequence is valid or not, so no analysis will
//   be available for failed sequences, but merely a true or false
//   return.
bool is_ch_set_valid_utf8( string ch_set2validate );

//
// This function tests a Unicode code point (limited to the 16 bit set)
//   and returns true if it is in the set of known combining characters
bool is_cod_pt_combining( uint16_t tstch );

//
// Convert Unicode code point to a UTF-8 string.  This function will
//   return a valid UTF-8 string that represents the passed Unicode code
//   point number.  It checks to insure that the passed point is not a
//   member of the surrogates area, and if so will replace those code
//   point requests with the Unicode replacement character instead.  All
//   other code point requests are assumed to be valid and the
//   representative UTF-8 string is returned for them.
string U_code_pt_to_UTF_8( uint32_t code_id );


#endif  // UNI_UTILS_H
