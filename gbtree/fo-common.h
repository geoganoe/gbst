//
// This provides the common declaration for the file organizer string
//   handling classes that are needed for handling the folder and
//   file name strings.  These common declarations are needed by
//   virtually every object module in the development.
//

#ifndef FO_COMMON_H
#define FO_COMMON_H

#include <string>
// #include <iostream>
#include <sstream>

using namespace std;

//
// Macro to enable code intended for development only
#define INdevel

//
// GGG - For now, the INFOdisplay define needs to be turned on at all
//   times until I decide how to solve the problem with getting name
//   strings.
#define INFOdisplay

//
// Macro to enable ncurses I/O capabilities
#define USEncurses

//
// Macro to provide the btree graphic output display coding additions
// #define GENbtreeGRF

#ifndef INFOdisplay
#ifdef USEncurses
#define INFOdisplay
#endif  //  #ifdef USEncurses
#endif // #ifndef INFOdisplay

//
// Define constants required throughout the file organizer sources
//
// Eventually, this will need to be set to a high number to accomodate
//   the needs of the many file systems to be aggragated, but for
//   current testing purposes, a smaller number is needed.
const int max_num_rcrd = 2200;
//
// This size buffer is an arbitrary number of elements that the capacity
//   must exceed the current size by in order to continue processing.
const int siz_buffer = 5;

//
// Since bit fields are implemented starting from the least significant
//   bit, I am changing the order here to reflect the bit placement
//   used for the command line operations option
struct flag_set {
    uint16_t play_name_store_stream : 1;   // 0x0001
    uint16_t show_name_store_strings : 1;  // 0x0002
    uint16_t show_data_record_sizes : 1;   // 0x0004
    uint16_t play_back_names_read : 1;     // 0x0008
    uint16_t show_info_output : 1;         // 0x0010
    uint16_t read_strings_from_file : 1;   // 0x0020
    uint16_t show_debugging_output : 1;    // 0x0040
    uint16_t track_base_search_vars : 1;   // 0x0080
    uint16_t show_default_base_vars : 1;   // 0x0100
    uint16_t change_start_str_num : 1;     // 0x0200
    uint16_t show_btree_graph_display : 1; // 0x0400
    uint16_t test_base_search_vars : 1;    // 0x0800
    uint16_t show_gbtree_traverse : 1;     // 0x1000
    uint16_t temp1bit : 1;                 // 0x2000
    uint16_t temp2bit : 1;                 // 0x4000
    uint16_t show_any : 1;                 // 0x8000
};

extern flag_set pflg;
extern stringstream dbstrm;
extern ostringstream infstrm;
extern ostringstream grfstrm;
extern ostringstream erstrm;

#ifdef INFOdisplay    // Declarations for development only
// This is the base search variable info save stream it needs to be active
//   when the info window display or the info output is desired.
extern stringstream bsvistrm;
#endif // #ifdef INFOdisplay

extern ostream& iout;
extern ostream& gout;
extern ostream& errs;
extern ostream& dbgs;

#ifdef INdevel  // Declarations/definitions/code for development only
struct debug_flags {
    // The debug flag comments to be filled in as the purpose is assigned.
    uint32_t a0 : 1;  // gbtree traverse debug flag
    uint32_t a1 : 1;  // test base search variables debug flag
    uint32_t a2 : 1;  // Name string conversion and validation dbg flag
    uint32_t a3 : 1;  // New string rcrd setup debugging flag
    uint32_t a4 : 1;  // Display name push and pop debug flag
    uint32_t a5 : 1;  // Set base search variable debug flag for utf8
    uint32_t a6 : 1;  // Set base search variable debug flag for hash types
    uint32_t a7 : 1;  // Replace node process debug flag
    uint32_t b0 : 1;  // Find my place process debug flag
    uint32_t b1 : 1;  // Place new node process debug flag
    uint32_t b2 : 1;  // Debug flag for Identify Encoding function
    uint32_t b3 : 1;  // Index vector storage monitoring debug flag
    uint32_t b4 : 1;  // str_utf8 class coding debug flag
    uint32_t b5 : 1;  //
    uint32_t b6 : 1;  //
    uint32_t b7 : 1;  //
    uint32_t c0 : 1;  //
    uint32_t c1 : 1;  //
    uint32_t c2 : 1;  //
    uint32_t c3 : 1;  //
    uint32_t c4 : 1;  //
    uint32_t c5 : 1;  //
    uint32_t c6 : 1;  //
    uint32_t c7 : 1;  //
    uint32_t d0 : 1;  //
    uint32_t d1 : 1;  //
    uint32_t d2 : 1;  //
    uint32_t d3 : 1;  //
    uint32_t d4 : 1;  //
    uint32_t d5 : 1;  //
    uint32_t d6 : 1;  //
    uint32_t d7 : 1;  //
};
const string dbgn =
  "a0a1a2a3a4a5a6a7b0b1b2b3b4b5b6b7c0c1c2c3c4c5c6c7d0d1d2d3d4d5d6d7";

extern debug_flags dbgf;
extern bool test_bsv_debug;
extern bool in_main;
#endif  //  #ifdef INdevel

//
// Declare extern for the info height and width variables here because
//   many modules need to use them.  The values will be defined in the
//   fo-string-test.cc module depending on whether or not ncurses is
//   being used.
extern int16_t inf_ht, inf_wid;

void atexit_handl_1();
void atexit_handl_2();
void atexit_handl_3();
void atexit_handl_4();
void atexit_handl_5();
void atexit_handl_6();
void atexit_handl_7();
void atexit_handl_8();
void atexit_handl_9();


#endif  // FO_COMMON_H
