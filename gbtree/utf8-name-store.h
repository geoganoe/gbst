//
// This provides the declaration for the file organizer string handling
//   classes that are needed for handling the folder and file name
//   strings.  These strings can potentially be made from a number of
//   string encoding types, or even no formal encoding at all given the
//   rules for naming files in some file systems.
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
// The objective of the string handling classes is to accept any string
//   that has been found in the target file system types and determine
//   the encoding type and save it in the null terminated name string
//   table in a way that the original string can be recovered when
//   needed.  The string pointer table record associated with the
//   string will provide the information needed to assist in assuring
//   the capability exists and can be implemented.
//
// This file contains the declaration for the name store class that
//   stores the character strings that represent the names of the
//   folders or files that describe the location of particular files
//   in each recorded file system.
//

#ifndef UTF8_NAME_STORE_H
#define UTF8_NAME_STORE_H

#include "fo-common.h"

#ifdef INdevel
    // Declarations/definitions/code for development only

#endif // #ifdef INdevel

using namespace std;

//
// Define constants for this include file
const int dflt_store_size = max_num_rcrd * 256;
// limit the size for now -- const int ptr_tbl_max_rcrd = 1024 * 64;
const int ptr_tbl_max_rcrd = max_num_rcrd;

class utf8_name_store
{
    static int nam_str_intro_last_idx;
    int name_store_size;
    int nxt_index;
    char name_store[ dflt_store_size ];

public:
    utf8_name_store();
    int store_name( string name_chars );  // returns start index
    string retrieve_name( int name_index );
    string get_name_store_chs( int start_idx, int nchars );
    size_t name_space_left();
    int get_intro_last_idx() { return nam_str_intro_last_idx; } ;

private:
    bool valid_name( string str_var );
};



#endif  // UTF8_NAME_STORE_H
