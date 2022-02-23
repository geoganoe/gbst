//
// This file is a part of the file organizer hash record handling classes
//   that are needed for handling the hashes generated for files. These
//   hashes will be either sha1 or md5 hash arrays that provide the
//   unique hash information about a target file.
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
// This file contains the declaration for the hash record class that
//   manages the hash data.  The class provides the mechanisms necessary
//   to receive any valid file hash array from any of the supported file
//   system types and store them in a hash type specific byte array,
//   while providing a guarantee that only unique hash values are stored
//   in the hash record data table.  It does this by using the gbtree
//   binary tree class to manage the search to find if the hash already
//   exists in the hash record table and if found returns the id of the
//   existing copy instead of creating a new record.
//
// Use the following command to build this object:
//   g++ -std=c++17 -c hash-rcrd-type.cc
//   ar -Prs ~/data/lib/libfoutil.a hash-rcrd-type.o
//   ar -Ptv ~/data/lib/libfoutil.a

#ifndef HASH_RCRD_H
#define HASH_RCRD_H

// #include <string>    in common included by fo-utils
// #include <iostream>
// #include <sstream>   in common included by fo-utils
// #include <cstdint>   included by fo-utils
#include "fo-utils.h"
#include "gbtree.h"
#include <openssl/sha.h>
#include <openssl/md5.h>

#ifdef INdevel
    // Declarations/definitions/code for development only

#endif // #ifdef INdevel

using namespace std;

//   class md5sha1Digest
//   {
//   public:
//       md5dgstArrayType md5dgstArray; /* enough size for hash MD5 */
//       sha1dgstArrayType sha1dgstArray; /* enough size for hash SHA1 */
//       md5sha1Digest();
//       string md5dgstHexOut();
//       string Sha1dgstHexOut();
//   protected:
//       template<typename dgstArrayTyp>
//           void dgstHexOut(dgstArrayTyp& dgstAry, string& rstr);
//
//   };

// MD5_DIGEST_LENGTH is 16 bytes long which creates 32 hex digits
typedef array<unsigned char,MD5_DIGEST_LENGTH> md5dgstArrayType;
// SHA_DIGEST_LENGTH is 20 bytes long which creates 40 hex digits
typedef array<unsigned char,SHA_DIGEST_LENGTH> sha1dgstArrayType;

//
// GGG - Try again to make this state struct local to the class.  It
//   would not work before, but I may have figured out how to do it.
//    template<class N_array>
//    struct spr_bsv_state {
//        N_array bsv_min;
//        N_array bsv_max;
//        N_array bsv_var;
//
//        spr_bsv_state( N_array min, N_array max, N_array var )
//        {
//            bsv_min = min;
//            bsv_max = max;
//            bsv_var = var;
//        }
//    };

template<class N_array >
class hash_rcrd_type : public gbtree
{
    struct test_local {
        uint16_t val01;
        char letters[ 6 ];

        test_local()
        {
            val01 = 0;
            const string temp = "abcdef";
            for ( char let : temp ) letters[ val01++ ] = let;
        }
    };

    struct spr_bsv_state {
        N_array bsv_min;
        N_array bsv_max;
        N_array bsv_var;

        spr_bsv_state()
        {
            if ( in_main == false )
            {
                const int result_6 = atexit( atexit_handl_6 );
                if ( result_6 != 0 ) errs << "atexit reg hdlr 6 fail." << endl;
            }
            bsv_min = base_sea_var_min;
            bsv_max = base_sea_var_max;
            bsv_var = base_sea_var;
        }
        void restore_state()
        {
            base_sea_var_min = bsv_min;
            base_sea_var_max = bsv_max;
            base_sea_var = bsv_var;
        }
    };

    static vector<name_string_hold > h_nmst_hld;
    //    static vector<spr_bsv_state<N_array > > bsv_state_vec;
    static vector<hash_rcrd_type<N_array > > dgst_rcrds;
    static N_array base_sea_var_min;
    static N_array base_sea_var_max;
    static N_array base_sea_var;
    static test_local loc_var;
    static vector<spr_bsv_state> bsv_state_vec;
    static spr_bsv_state init_state;

protected:
    // uint16_t reserv01;
    N_array hashVal;

    // template<typename dgstArrayTyp>
    void dgstHexOut(N_array& dgstAry, string& rstr);
    bool prep4search();
    int save_bsv_state();
    void restore_bsv_state( int sv_idx );
    void release_bsv_state( int sv_idx );

#ifdef INFOdisplay

    void push_name_struct( int nm_rc_id );
    void pop_name_struct();
    void get_name_io( string& id_strng );
    const str_utf8& get_rcrd_display_name();
    static string typ_strng;
    string get_type_strng();
    static char r_typ;
    char get_rcrd_type();

#endif // #ifdef INFOdisplay

public:
    int what_is_my_id();

#ifdef INdevel   // Declarations/definitions/code for development only
    void gb_get_out( string intro, int nprmt, bool disp_table = false );
    uint16_t get_hash_idx( int idx );
#endif // #ifdef INdevel

    hash_rcrd_type();
    hash_rcrd_type( N_array& a_dgst );
    //
    // Returns a reference to the record ID passed as the parameter
    // This reference is used to access member and method information about
    //   the various class record instances that are related to the process
    //   being done on the current default class instance.
    hash_rcrd_type& get_node( int node_idx );
    string get_hex_coded_hash();
    // Must now be called by self node
    int cmp_node2base( void );
    // These two compare the calling instance's record info to the info at
    //   the node ID that is passed as a parameter.  The calling instance
    //   can be the default instance represented by the current "this"
    //   pointer or another specified class member instance such as by a
    //   reference that has been obtained via the get_node() method
    //   described above.
    int cmp_rcrd2base( int node_idx );
    int cmp_rcrd2node( int node_idx );

    // This method is now driven by the base class management of the binary
    //   tree and as such, the base class knows when the base search variable
    //   needs to be updated and will call this method when necessary
    // GGG - set_base_srch_var() method will be different than this, but
    //   will be better described as this class is developed.
    // The set_base_srch_var() method will return the number of characters
    //   stored in the srchstr_node_add array (0-6) when successful, and a
    //   negative value if it detects an error.  This may be changed once
    //   the reliability of the process has been proven
    int set_base_srch_var();
    void traverse_hash_records();
    //
    // The derived class needs to do the replace part for the base search
    //   variable then return the reference it obtains for the node to be
    //   replaced
    hash_rcrd_type& replace_node_derived( int node_idx );
    //   virtual hash_rcrd_type& replace_node_derived( int node_idx ) = 0;
    //
    // Initializes a node record for this derived class in the data base
    //   array and copies the calling records data to it then returns the
    //   id of the new data base node created.
    virtual int add_new_node();
    void init_dgst_vector( char rec_typ );

};

template class hash_rcrd_type<sha1dgstArrayType > ;
using sha1_rcrd_type = hash_rcrd_type<sha1dgstArrayType >;

template class hash_rcrd_type<md5dgstArrayType > ;
using md5_rcrd_type = hash_rcrd_type<md5dgstArrayType >;

//  template class hash_rcrd_type<array<unsigned char,12 > > ;
//  using tmp_rcrd_type = hash_rcrd_type<array<unsigned char,12 > >;


#endif  //  #ifndef HASH_RCRD_H
