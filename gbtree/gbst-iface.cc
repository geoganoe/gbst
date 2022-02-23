//
// This program tests the ability of the file organizer string handling
//   classes to provide the design goals for handling the folder and
//   file name strings.  These strings can potentially be made from a
//   number of string encoding types, or even no formal encoding at all
//   given the rules for naming files in some file systems.
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
// This file contains the code to implement the functions needed to
//   interface the name string save requests from the external sources
//   to the string management classes.  The string is first identified
//   for its encoding type, and then converting that string to a UTF-8
//   compatible string if it is not already UTF-8 encoded.  If that can
//   not be done in a bi-directionally reversable manner, a
//   bi-directionally reversible UTF-8 compatible string is produced
//   such that it can be used to recreate the original string when
//   needed. The two resulting strings are then stored consecutively in
//   the name string table with the second string hidden but able to be
//   able to be retrieved due to starting right after the main string.
//
//
// Use the following command to build this object:
//   g++ -std=c++17 -c fo-string-table.cc
//   ar -Prs ~/data/lib/libfoutil.a fo-string-table.o
//   ar -Ptv ~/data/lib/libfoutil.a

#include "gbst-iface.h"
#include "../uni-utils/hex-symbol.h"
#include "../uni-utils/uni-utils.h"
#include "hash-rcrd-type.h"

void atexit_handl_2()
{
    cout << "at exit handler number 2." << endl;
}

extern significant_collation_chars scc;

//
// The default styp_flags set which is initialized by the class
//   gbst_interface_type constructor
styp_flags default_flg_set;

gbst_interface_type::gbst_interface_type()
{
    const int result_2 = atexit( atexit_handl_2 );
    //
    // Start by initializing the derived data structures that are needed
    // The scc_set array is:
    // May now be fixed in the declaration - ggguniq_str_bal_list[ 0 ] = us;

    if ( result_2 != 0 ) cout << "Regist. for atexit hdlr 2 failed" << endl;
    for ( int idx = 0; idx < scc_size_ascii; idx++ )
      scc_set[ idx ] = cmask & scc.ascii_set[ idx ];
    for ( int idx = scc_size_ascii; idx < scc_set_size; idx++ )
      scc_set[ idx ] = scc.utf_16_set[ idx - scc_size_ascii ];
    if ( test_scc_set_validity() )
    {
        // It should be OK now
        cout << "With the greek symbol [α] replacing the [us] control "
          "character, the test_scc_set_validity() function passes." << endl;
    }
    // The default string record flag set is
    // Fill in default values for the flags in the set
    default_flg_set.ASCII_7 = 1;
    default_flg_set.UTF_8_orig = 1;
    default_flg_set.UTF_8_compat = 1;
    default_flg_set.ISO_8859_1 = 0;
    default_flg_set.UCS_2 = 0;
    default_flg_set.UTF_16 = 0;
    //
    // The scc_idx_to_str_bal array contains strings of indexes into the
    //   scc_set array that represent the values from the ggguniq_str_bal_list
    //   array.  Since base_search_str strings must be regularly computed, it
    //   is easier if those computations can be done based on the set of
    //   indexes rather than trying to work with variously sized UTF-8
    //   characters
    int digit_start = find_scc_ascii_index( '0' );
    int letter_start = find_scc_ascii_index( 'a' );
    for ( int idx = 0; idx < ggg_bal_lst_siz; idx++ )
    {
        //
        // The scc_set array can change, so this section needs to be dynamic
        //   in order to keep up.  This section creates a scc_set index version
        //   of the string ggguniq_str_bal_list[ ggg_bal_lst_siz ] which
        //   contains the set of string balancing bases for the first 5 levels
        //   of the binary tree
        int ch_idx;
        for ( char blst_ch : ggguniq_str_bal_list[ idx ] )
        {
            if ( blst_ch >= 'a' && blst_ch <= 'z' )
              scc_idx_to_str_bal[ idx ].push_back( static_cast<uint8_t>(
              letter_start + ( blst_ch - 'a' ) ) );
            else if ( blst_ch >= '0' && blst_ch <= '9' )
              scc_idx_to_str_bal[ idx ].push_back( static_cast<uint8_t>(
              digit_start + ( blst_ch - '0' ) ) );
            else if ( ( ch_idx = find_scc_ascii_index( blst_ch ) ) >= 0 )
              scc_idx_to_str_bal[ idx ].push_back( static_cast<uint8_t>( ch_idx ) );
            else errs << "Character " << blst_ch <<
              " was not found in the scc_idx array." << endl;
        }
    }

    //
    // Initialize the UTF-8 Name String gbtree
    utf8_rcrd_type base_utf8;
    base_utf8.init_name_str_vector();

#define SETUP_hash_test
#ifdef SETUP_hash_test  // Define this macro to enable the hash testing
    //
    // This section sets up the sha1 and md5 hash btree testing which just
    //   tests creating btrees for the sha1 and md5 hashes of the name
    //   strings that are used for the name string pointer record test
    //
    sha1_rcrd_type base_sha1;
    md5_rcrd_type base_md5;
    base_sha1.init_dgst_vector( 's' );
    base_md5.init_dgst_vector( 'm' );
#endif  //  #ifdef SETUP_hash_test

    //     utf8_rcrd_type base_name_string;
    //     base_name_string.init_name_str_vector();
    nxt_tbl_index = 1;
}

int gbst_interface_type::search_place_name( string fil_sys_name )
{
    // GGG - Need to work on this
    // This is where the btree stuff needs to be implemented, at first
    //   for the file ~/temp/sort-nx-rnd-test.txt, and then for file
    //   ~/data/src/file-organizer/temp/tmp-ggtest-w-spec.txt and
    //   then ~/data/temp/dell-5520-usb-256-drives/ggfsnames.txt which
    //   is the individual name separated list of all folder and file
    //   names in the ggguniqfilearciv-files-sums.txt hashes listing.
    int new_str_place;
    int new_sha_place;
    int new_md5_place;
    //
    // Need to convert the fil_sys_name to a UTF-8 compatible string that is
    //   bi-directionally convertable to the original string.  If that UTF-8
    //   compatible string is not equivalent to the real string, a new string
    //   that is both equivalent and a UTF-8 string must be created and
    //   stored immediately after the original string.
    styp_flags flg_set = default_flg_set;
    styp_flags flg_idd = identify_encoding( fil_sys_name, flg_set );
    string utf8name;
    if ( flg_idd.UTF_8_orig == 1 )
    {
        utf8name = fil_sys_name;

#ifdef INdevel    // Declarations for development only
        if ( dbgf.a2 ) dbgs << "String identified as UTF-8.";
#endif // #ifdef INdevel

    }
    else if ( flg_idd.ISO_8859_1 == 1 )
    {
        utf8name = utf8from8859_1( fil_sys_name );
        flg_idd.UTF_8_compat = 1;

#ifdef INdevel    // Declarations for development only
        if ( dbgf.a2 ) dbgs << "String identified as ISO_8859_1 and" <<
          " converted to UTF-8 compatible.";
#endif // #ifdef INdevel

    }
    else
    {
        errs << "Unexpected string type found." << endl;
    }

#ifdef INdevel
  if ( dbgf.a2 )
  {
    dbgs << " UTF-8 as chars: \"";
    for ( auto ch : utf8name )
    {
        if ( isgraph( ch ) ) dbgs << ch;
        else dbgs << hex_symbol( cmask & ch );
    }
    dbgs << "\"" << endl;
  }
#endif // #ifdef INdevel

#ifdef SETUP_hash_test  // Define this macro to enable the hash testing
    sha1dgstArrayType str_sha_dgst;
    unsigned char *ssd = SHA1( reinterpret_cast<const unsigned char*>(
      utf8name.c_str() ), utf8name.size(), str_sha_dgst.data() );
    if ( ssd != str_sha_dgst.data() )
    {
        errs << "The SHA1 call returned an unexpected pointer." << endl;
    }
    md5dgstArrayType str_md5_dgst;
    unsigned char *smd = MD5( reinterpret_cast<const unsigned char*>(
      utf8name.c_str() ), utf8name.size(), str_md5_dgst.data() );
    if ( smd != str_md5_dgst.data() )
    {
        errs << "The SHA1 call returned an unexpected pointer." << endl;
    }
#endif  //  #ifdef SETUP_hash_test

#ifdef INFOdisplay
    str_utf8 rcinftxt( "New rcrd: txt[" + utf8name + "]" );
    int colnum = rcinftxt.numsym;
    uint16_t cmask = 0x0ff;

#ifdef SETUP_hash_test  // Define this macro to enable the hash testing
    ostringstream shahex;
    shahex << "sha1[" << hex;
    for ( auto hbyt : str_sha_dgst )
    {
        uint16_t tbyt = cmask & hbyt;
        shahex << setfill( '0' ) << setw( 2 ) << tbyt;
    }
    shahex << "]," << dec;
    ostringstream md5hex;
    md5hex << "md5[" << hex;
    for ( auto hbyt : str_md5_dgst )
    {
        uint16_t tbyt = cmask & hbyt;
        md5hex << setfill( '0' ) << setw( 2 ) << tbyt;
    }
    md5hex << setfill( ' ' ) << dec << "].";
    int hashsiz = shahex.str().size() + 1 + md5hex.str().size();
    iout << rcinftxt.target;
    colnum += 2 + hashsiz;
    if ( inf_wid > colnum )
    {
        iout << ", ";
    }
    else
    {
        iout << "," << endl << "    ";
    }
    iout << shahex.str() << " " << md5hex.str() << endl;

#else  // Not doing hash test, just display utf8 name

    iout << rcinftxt.target << "." << endl;
#endif  //  #ifdef SETUP_hash_test

#endif // #ifdef INFOdisplay

    //
    // Create the new record as a temp to get the defaults set correctly
    //   then copy to new_str_ptr so it can be referenced when needed.
    utf8_rcrd_type tmp_spr( utf8name, flg_idd );
    new_str_ptr = tmp_spr;

#ifdef SETUP_hash_test  // Define this macro to enable the hash testing
    sha1_rcrd_type new_sha_rcrd( str_sha_dgst );
    md5_rcrd_type new_md5_rcrd( str_md5_dgst );
#endif  //  #ifdef SETUP_hash_test

    // Once the string(s) are settled, the string record flags,
    //   srchstr_node_add array, btree_parent, btree_child_left, and
    //   btree_child_right need to be initialized.  In most cases, the
    //   default values will be appropriate, with the most common alternative
    //   being an ISO_8859_1 string which is bi-directionally convertable to
    //   and from UTF-8.  That case is supported by setting ASCII_7 = 0,
    //   UTF_8_orig = 0, UTF_8_compat = 1, and ISO_8859_1 = 1.  The rest can
    //   be left at the default, and that flag set can be used to do the
    //   conversion back to the original string when needed.  Other cases
    //   can be easily determined when needed by refering to the File
    //   Organizer Planning article that is supplied with this code.
    //
    // Any new search has to start with the node at the head of the tree.
    //   That node pointer is stored at the btree_child_right index of the
    //   0 index record, and need not be the original head which was index 1,
    //   but could be the index of some other node that has replaced the
    //   original.  Any replacements would be done by the place_new_node
    //   method which is a part of the base class gbtree (see gbtree.{h,cc}.
    //

#ifdef INdevel    // Declarations for development only
  if ( dbgf.a3 )
  {
    tmp_spr.shossp( "Disp static search parms and tmp_spr "
      "before call to place_new_node:" );
    new_str_ptr.shossp( "Disp static search parms and new_str_ptr "
      "before call to place_new_node:" );
  }
#endif // #ifdef INdevel

    if ( ( new_str_place = new_str_ptr.place_new_node() ) > 0

#ifdef SETUP_hash_test  // Define this macro to enable the hash testing
      && ( new_sha_place = new_sha_rcrd.place_new_node() ) > 0
      && ( new_md5_place = new_md5_rcrd.place_new_node() ) > 0
#endif  //  #ifdef SETUP_hash_test

       )  // Close parentheses on if statement
    {
        if ( new_str_place < nxt_tbl_index )
        {
            // The name string is not unique, and the returned ID is for
            //   the original one.  This is not an error, but there may
            //   be a need in the future to add something here, possibly
            //   a tracker for duplicate names.
        }
        else nxt_tbl_index = new_str_place + 1;
        //
        // Get the sha1 and md5 digests for the string here and place
        //   them in their respective hash record type btree after writing
        //   the note to the info screen.
        //
    }
    else
    {

#ifdef INdevel
    // Declarations/definitions/code for development only
        string msg = " One of the holding vectors or the string_table[] array";
        msg += " is full so no new names can be added to the data base.";
        getout( msg, 2 );

#else // not INdevel

        my_exit_msg = "Not ready error from place_new_node() method.";
        myexit();
#endif // #ifdef INdevel

    }
    return new_str_place;
}

void gbst_interface_type::test_btree_bsv()
{

#ifdef INdevel
    if ( test_bsv_debug )
      dbgs << "Setting up the new_str_ptr record." << endl;
#endif  //  #ifdef INdevel

    utf8_rcrd_type tmp_spr;
    new_str_ptr = tmp_spr;
    new_str_ptr.test_bss_compute();
}
