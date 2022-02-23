//
// This program tests the ability of the file organizer string handling
//   classes to provide the design goals for handling the folder and
//   file name strings.  These strings can potentially be made from a
//   number of string encoding types, or even no formal encoding at all
//   given the rules for naming files in some file systems.
//
// The objective of the string handling classes is to accept any string
//   that has been found in the target file system types and determine
//   the encoding type and save it in the null terminated name string
//   table in a way that the original string can be recovered when
//   needed.  The string pointer table record associated with the
//   string will provide the information needed to assist in assuring
//   the capability exists and can be implemented.
//
// This file contains the code to implement the name store class that
//   stores the character strings that represent the names of the
//   folders or files that describe the location of particular files
//   in each recorded file system.
//
//
// Use the following command to build this object:
//   g++ -std=c++17 -c utf8-name-store.cc
//   ar -Prs ~/data/lib/libfoutil.a utf8-name-store.o
//   ar -Ptv ~/data/lib/libfoutil.a

#include "utf8-name-store.h"
#include "fo-common.h"
#include <cstring>
#include <iomanip>
#include <ctype.h>

#define DOatexit
#ifdef DOatexit
#include <iostream>

void atexit_handl_5()
{
    cout << "at exit handler number 5." << endl;
}
#endif  //    #ifdef DOatexit

int utf8_name_store::nam_str_intro_last_idx = 0;

utf8_name_store::utf8_name_store()
{
    const int result_5 = atexit( atexit_handl_5 );
    name_store_size = dflt_store_size;
    nxt_index = 0;
    int first_index =
      store_name( "The name string sequence begins here: " );
    if ( first_index != 0 || ( result_5 != 0 ) )
    {
        errs << "The first_index value of " << first_index <<
          " or the atexit return of " << result_5 <<
          " is unexpected." << endl <<
          "Verify the code for utf8_name_store before continuing. " << endl <<
          "The program will exit with error 1 at source line " <<
          __LINE__ << " ..." << endl;
        exit( 1 );
    }
    nam_str_intro_last_idx = nxt_index - 1;
}

int utf8_name_store::store_name( string name_chars )
{
    // initialize the index to the invalid error code
    int str_index = -1;
    if ( valid_name ( name_chars ) && name_space_left() > name_chars.size() )
    {
        char* dest = strcpy( &name_store[ nxt_index ], name_chars.c_str() );
        if ( !( dest == &name_store[ nxt_index ] ) )
        {
            errs << "Unexpected return pointer from strcpy in store_name()"
              " method." << endl;
        }
        str_index = nxt_index;
        nxt_index += name_chars.size() + 1;
    }
    return str_index;
}

string utf8_name_store::retrieve_name( int name_index )
{
    string ret_name = "� Invalid request";
    if ( name_index > nam_str_intro_last_idx && name_index < nxt_index )
    {
        if ( name_store[ name_index - 1 ] != '\0' )
        {
            errs << "Invalid request to retrieve a name string.  "
              "The requested index was not" << endl <<
              "    at the beginning of a string." << endl;
        }
        else
          ret_name = &name_store[ name_index ];
    }
    else
    {
        errs << "Requested name string index out of range:  want = " <<
          name_index << ", valid range from " << nam_str_intro_last_idx + 1 <<
          " to less than " << nxt_index << "." << endl;
    }
    return ret_name;
}

string utf8_name_store::get_name_store_chs( int start_idx, int nchars )
{
    string ret_str = "";
    if ( start_idx < nxt_index )
    {
        int count = (start_idx + nchars < nxt_index ?
          nchars : nxt_index - start_idx );
        ret_str.insert( 0, name_store+start_idx, count );
    }
    return ret_str;
}

size_t utf8_name_store::name_space_left()
{
    return name_store_size > nxt_index ? name_store_size - nxt_index : 0;
}

bool utf8_name_store::valid_name( string str_var )
{
    bool nam_good = true;
    for( auto ch: str_var )
    {
        if ( ch == '/' || ch == '\0' ) nam_good = false;
    }
    return nam_good;
}

