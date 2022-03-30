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
// The objective of the gbst_interface_type is to accept any string
//   that has been found in the target file system types and determine
//   the encoding type and save it in the null terminated name string
//   table in a way that the original string can be recovered when
//   needed.  The UTF-8 name string record member associated with the
//   string will provide the information needed to assist in assuring
//   the capability exists and can be implemented.
//
// This file contains the declaration for the
//   gbst_interface_type class that manages the
//   interface between the external user of the strings and the string
//   management function provided by the name store, the string pointer
//   record, and the gbtree classes.  This class receives the raw
//   strings from the external source and provides an encoding
//   identified string that is then converted into a UTF-8 compatible
//   string to be added to the name string data base.

#ifndef GBST_IFACE_H
#define GBST_IFACE_H

#include "utf8-name-store.h"
#include "utf8-rcrd-type.h"
#include <iostream>

#ifdef INdevel
    // Declarations/definitions/code for development only

#endif // #ifdef INdevel

using namespace std;

class gbst_interface_type
{

public:
    utf8_rcrd_type new_str_ptr;
    int nxt_tbl_index;

    gbst_interface_type();
    //
    // Searches the b tree for the name, and returns utf8_rcrd_type
    //   index for it.  That index can be the index of an existing
    //   utf8_rcrd_type record if it already exists.
    int search_place_name( string fil_sys_name );
    //
    // Test the btree base search variable process
    void test_btree_bsv();
    // Show the fo_string_ptr[] array
};

#endif //GBST_IFACE_H
