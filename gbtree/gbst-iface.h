//
// This provides the declaration for the file organizer string handling
//   classes that are needed for handling the folder and file name
//   strings.  These strings can potentially be made from a number of
//   string encoding types, or even no formal encoding at all given the
//   rules for naming files in some file systems.
//
// The objective of the string handling classes is to accept any string
//   that has been found in the target file system types and determine
//   the encoding type and save it in the null terminated name string
//   table in a way that the original string can be recovered when
//   needed.  The string pointer table record associated with the
//   string will provide the information needed to assist in assuring
//   the capability exists and can be implemented.
//
// This file contains the declaration for the string pointer table class
//   that contains the arrays needed for the contents of the name store
//   and string pointer records, and also provides the management
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
    // GGG - Some additional methods may be implemented to simplify
    //   fo_string_ptr record handling.
    //
    // Searches the b tree for the name, and returns fo_string_ptr
    //   index for it.  That index can be the index of an existing
    //   fo_string_ptr record if it already exists.
    int search_place_name( string fil_sys_name );
    //
    // Test the btree base search variable process
    void test_btree_bsv();
    // Show the fo_string_ptr[] array
};

#endif //GBST_IFACE_H
