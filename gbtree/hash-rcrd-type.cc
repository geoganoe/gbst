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
// This file contains the template code to implement the hash record
//   classes that manage the hash data.  The template class provides the
//   mechanisms necessary
//   to receive any valid file hash array from any of the supported file
//   system types and store them in a hash type specific byte array,
//   while providing a guarantee that only unique hash values are stored
//   in the hash record data table.  It does this by using the gbtree
//   binary tree class to manage the search to find if the hash already
//   exists in the hash record table and if found returns the id of the
//   existing copy instead of creating a new record.
//
#include "hash-rcrd-type.h"
#include <cstring>

#ifdef USEncurses
#include "ncursio.h"
#endif   //  #ifdef USEncurses

// Class hash_rcrd_type private members

#define DOatexit
#ifdef DOatexit
#include <iostream>

void atexit_handl_6()
{
    cout << "at exit handler number 6." << endl;
}
#endif  //    #ifdef DOatexit

template<class N_array >
vector<name_string_hold> hash_rcrd_type<N_array >::h_nmst_hld = {};

//  template<class N_array >
//  vector<spr_bsv_state<N_array > > hash_rcrd_type<N_array >::bsv_state_vec = {};

//
// Trying to incorporate these into the template
//    vector<sha1_rcrd_type> sha1_rcrds = {};
//    vector<md5_rcrd_type> md5_rcrds = {};
// So one becomes: sha1_rcrd_type::dgst_rcrds and the other is
//   md5_rcrd_type::dgst_rcrds
template<class N_array >
vector<hash_rcrd_type<N_array > > hash_rcrd_type<N_array >::dgst_rcrds = {};

template<class N_array >
N_array hash_rcrd_type<N_array >::base_sea_var_min;
template<class N_array >
N_array hash_rcrd_type<N_array >::base_sea_var_max;
template<class N_array >
N_array hash_rcrd_type<N_array >::base_sea_var;

template<class N_array > struct
hash_rcrd_type<N_array >::test_local hash_rcrd_type<N_array >::loc_var;

//
// GGG - Trying to get templates to accept the local struct declaration as
//   a valid type seems nearly impossible.
//   template<class N_array >
//   vector<
//     ( template<class N_array >
//       hash_rcrd_type<N_array >::spr_bsl_state )
//     > hash_rcrd_type<N_array >::bsl_state_vec = {};  // doesn't work
//
// GGG - The only way I could get the compiler to accept the spr_bsv_state
//   struct as a type is to specialize the instantiation of its use to
//   define the needed vector.
template<>
vector<sha1_rcrd_type::spr_bsv_state> sha1_rcrd_type::bsv_state_vec = {};
template<>
vector<md5_rcrd_type::spr_bsv_state> md5_rcrd_type::bsv_state_vec = {};

template<class N_array > struct
hash_rcrd_type<N_array >::spr_bsv_state hash_rcrd_type<N_array >::init_state;

// Class hash_rcrd_type protected members

template<class N_array >
void hash_rcrd_type<N_array >::dgstHexOut(N_array& dgstAry, string& rstr)
{
    stringstream dgstString;
    dgstString << hex;
    for (const auto ucs: dgstAry)
    {
        dgstString.fill('0');
        dgstString.width(2);
        dgstString << static_cast<uint16_t>( 0x00ff & ucs );
    }
    // string strmd5 = dgstString.str();
    dgstString << dec;
    rstr = dgstString.str();
    return;
}

template<class N_array >
bool hash_rcrd_type<N_array >::prep4search()
{
    bool ready = dgst_rcrds.capacity() > dgst_rcrds.size() + siz_buffer;
    //
    // There may be more that needs to be done, but for now, this will
    //   do.
    return ready;
}

template<class N_array >
int hash_rcrd_type<N_array >::save_bsv_state()
{
    bsv_state_vec.emplace_back();
    //    bsv_state_vec.emplace_back( base_sea_var_min,
    //      base_sea_var_max, base_sea_var );
    return bsv_state_vec.size() - 1;
}

template<class N_array >
void hash_rcrd_type<N_array >::restore_bsv_state( int sv_idx )
{
    base_sea_var_min = bsv_state_vec[ sv_idx ].bsv_min;
    base_sea_var_max = bsv_state_vec[ sv_idx ].bsv_max;
    base_sea_var = bsv_state_vec[ sv_idx ].bsv_var;
}

template<class N_array >
void hash_rcrd_type<N_array >::release_bsv_state( int sv_idx )
{
    size_t req_idx = sv_idx;
    if ( req_idx == bsv_state_vec.size() - 1 )
      bsv_state_vec.pop_back();
    else
    {
        errs << "Requested release of bsv_state not at end as expected." <<
          endl;
        bsv_state_vec.erase( bsv_state_vec.begin() + sv_idx );
    }
}

#ifdef INFOdisplay  //  {
template<class N_array >
void hash_rcrd_type<N_array >::push_name_struct( int nm_rc_id )
{
    // GGG - This method is believed to be working
    string d_str;
    dgstHexOut( hashVal, d_str );
    h_nmst_hld.emplace_back( d_str, lm_nm_str_sz );
}

template<class N_array >
void hash_rcrd_type<N_array >::pop_name_struct()
{
    // GGG - This method is believed to be working
    h_nmst_hld.pop_back();
}

template<class N_array >
void hash_rcrd_type<N_array >::get_name_io( string& id_strng )
{
    // GGG - This method is believed to be working
    int clevel = get_level();
    ostringstream nam_io_strm;
    nam_io_strm << h_nmst_hld.back().lim_nmstr.target << id_strng <<
      get_rcrd_type() << setw( lvl_str_siz ) << clevel << " ";
    info_add = nam_io_strm.str();
}

template<class N_array >
const str_utf8& hash_rcrd_type<N_array >::get_rcrd_display_name()
{
    return h_nmst_hld.back().nmstr;
}

template<class N_array >
string hash_rcrd_type<N_array >::typ_strng = "";

template<class N_array >
string hash_rcrd_type<N_array >::get_type_strng()
{
    return typ_strng;
}

template<class N_array>
char hash_rcrd_type<N_array>::r_typ = 'd';

template<class N_array >
char hash_rcrd_type<N_array >::get_rcrd_type()
{
    return r_typ;
}
#endif // #ifdef INFOdisplay  }

// Class hash_rcrd_type public members

template<class N_array >
int hash_rcrd_type<N_array >::what_is_my_id()
{
    // GGG - This method TBD
    size_t myid = dgst_rcrds.size();
    //    int arrsize = sizeof( dgst_rcrds[0] );
    //    int mysize = sizeof( *this );
    //    int nrecs = arrsize / mysize;
    auto arradr = &dgst_rcrds[0];
    auto myadr = this;
    if ( myadr < arradr || !( (myid = (myadr - arradr) ) < dgst_rcrds.size() ) )
    {

#ifdef INdevel  //  {
        dbgs << "This record does not belong to the name pointer record set" <<
          " myadr = " << myadr << ", arradr = " << arradr <<
          ", myid = " << myid;
        getout( ".  ", 0 );

#else

        my_exit_msg = "what_is_my_id method called by record that is not"
          " in the record set.";
        myexit();
#endif // #ifdef INdevel    }

    }
    return myid;
}

#ifdef INdevel  //  {
// Declarations/definitions/code for development only
template<class N_array >
void hash_rcrd_type<N_array >::gb_get_out( string intro, int nprmt,
  bool disp_table )
{
    if ( disp_table )
    {
        getout( "", nprmt );
    }
    else getout( intro, nprmt );
}

template<class N_array >
uint16_t hash_rcrd_type<N_array >::get_hash_idx( int idx )
{
    return hashVal[ idx ];
}
#endif // #ifdef INdevel    }

template<class N_array >
hash_rcrd_type<N_array >::hash_rcrd_type()
{
    hashVal.fill( 0 );
}

template<class N_array >
hash_rcrd_type<N_array >::hash_rcrd_type( N_array& a_dgst )
{
    hashVal = a_dgst;
    string parm;
    dgstHexOut( a_dgst, parm );
}

template<class N_array>
hash_rcrd_type<N_array>& hash_rcrd_type<N_array>::get_node( int node_idx )
{
    if ( node_idx < 0 ||
      static_cast<size_t>( node_idx ) >= dgst_rcrds.size() )
    {
        iout << "Invalid node index for vector with size " <<
          dgst_rcrds.size() << " records requested by "
          "get_node( " << node_idx << " ) method, exiting." << endl;
        myexit ();
    }
    return dgst_rcrds.at( node_idx );
}

template<class N_array >
string hash_rcrd_type<N_array >::get_hex_coded_hash()
{
    string rstr;
    dgstHexOut( hashVal, rstr );
    return rstr;
}

template<class N_array >
int hash_rcrd_type<N_array >::cmp_node2base( void )
{
    // N_array node_str2cmp = hashVal;
    int cmp_rslt =
      memcmp( hashVal.data(), base_sea_var.data(), hashVal.size() );
    return cmp_rslt;
}

template<class N_array >
int hash_rcrd_type<N_array >::cmp_rcrd2base( int node_idx )
{
    int cmp_rslt =
      memcmp( hashVal.data(), base_sea_var.data(), hashVal.size() );
    return cmp_rslt;
}

template<class N_array >
int hash_rcrd_type<N_array >::cmp_rcrd2node( int node_idx )
{
    hash_rcrd_type<N_array >& node_ref = get_node( node_idx );
    int cmp_rslt =
      memcmp( hashVal.data(), node_ref.hashVal.data(), hashVal.size() );
    return cmp_rslt;
}

template<class N_array >
int hash_rcrd_type<N_array >::set_base_srch_var()
{
    int clevel = get_level();
    bool right_chld = get_rt_child_flg() == 1;
    if ( clevel == 1 )
    {
        base_sea_var_min.fill( 0x00 );
        base_sea_var.fill( 0x00 );
        base_sea_var[ 0 ] = 0x80;
        base_sea_var_max.fill( 0xff );
    }
    else
    {
        // The base search variable is determined by algorithm based on
        //   the current level and whether the search node is a left or
        //   a right child.  If a left child node, the bit from the
        //   previous level is reset to 0, and the current level bit is
        //   set to 1.  If a right child, the bit from the previous
        //   level is left at 1, and the current level bit is set to 1.
        const unsigned char msk_ary[] = { 0x80, 0x40, 0x20, 0x10,
                                          0x08, 0x04, 0x02, 0x01 };
        int c_byt_no = ( clevel - 1 ) / 8;
        int p_byt_no = ( clevel - 2 ) / 8;
        unsigned char c_bit_msk = msk_ary[ ( clevel - 1 ) % 8 ];
        unsigned char p_bit_msk = ~( msk_ary[ ( clevel - 2 ) % 8 ] );
        if ( right_chld )
        {
            base_sea_var_min = base_sea_var;
            base_sea_var[ c_byt_no ] |= c_bit_msk;
        }
        else
        {
            base_sea_var_max = base_sea_var;
            base_sea_var[ p_byt_no ] &= p_bit_msk;
            base_sea_var[ c_byt_no ] |= c_bit_msk;
        }
    }
    string min_strng;
    dgstHexOut( base_sea_var_min, min_strng );
    string max_strng;
    dgstHexOut( base_sea_var_max, max_strng );
    string base_sea_strng;
    dgstHexOut( base_sea_var, base_sea_strng );

#ifdef INdevel    // Declarations for development only
    if ( dbgf.a6 )
      dbgs << "return base search var |" << base_sea_strng << "|." << endl;
#endif // #ifdef INdevel

#ifdef INFOdisplay    // Declarations for development only
    //
    // First define the strings
    str_utf8 bsmin( min_strng.substr( 0, 7 ) + "…", infsea_str_siz,
      fit_center | fit_balance );
    int cmp_sv2min = strcoll( base_sea_strng.c_str(), min_strng.c_str() );
    str_utf8 bsmax( max_strng.substr( 0, 7 ) + "…", infsea_str_siz,
      fit_center | fit_balance );
    int cmp_max2sv = strcoll( max_strng.c_str(), base_sea_strng.c_str() );
    str_utf8 bas_sea_trans( base_sea_strng.substr( 0, 7 ) + "…", infsea_str_siz,
      fit_center | fit_balance );
    ostringstream bsv_disp_strm;
    bsv_disp_strm.imbue(std::locale("C"));
    bsv_disp_strm << bsmin.target <<
      ( cmp_sv2min  > 0 ? "↑" : cmp_sv2min  < 0 ? "↓" : "0" ) <<
      bas_sea_trans.target <<
      ( cmp_max2sv  > 0 ? "↑" : cmp_max2sv  < 0 ? "↓" : "0" ) <<
      bsmax.target << setw( id_str_siz ) << what_is_my_id() << " " <<
      get_hex_coded_hash();
    info_add += bsv_disp_strm.str();
#endif // #ifdef INFOdisplay

    return 0;
}

template<class N_array >
void hash_rcrd_type<N_array >::traverse_hash_records()
{
#ifdef INFOdisplay  // traverse_records will only work with this set
    int saved_state_idx = save_bsv_state();
    traverse_records();
    restore_bsv_state( saved_state_idx );
    release_bsv_state( saved_state_idx );
#else
    iout <<
      "method traverse_hash_records() called without INFOdisplay set." << endl;
#endif // #ifdef INFOdisplay
}

template<class N_array>
hash_rcrd_type<N_array>&
  hash_rcrd_type<N_array>::replace_node_derived( int node_idx )
{
    //
    // So far there is nothing to be done other than returning the
    //   reference to the node
    if ( node_idx < 0 ||
      static_cast<size_t>( node_idx ) >= dgst_rcrds.size() )
    {
        iout << "Invalid sha1 records node index requested by the "
          "replace_node_derived() method, exiting." << endl;
        myexit ();
    }
    return dgst_rcrds.at( node_idx );
}

// This can't be done here as it requires the abstract template class
//   to create an instantiation - no longer abstract so try again
template<class N_array>
int hash_rcrd_type<N_array>::add_new_node()
{
    dgst_rcrds.push_back( *this );
    return dgst_rcrds.size() - 1;
}

template<class N_array>
void hash_rcrd_type<N_array>::init_dgst_vector( char rec_typ )
{
    if ( dgst_rcrds.capacity() >= max_num_rcrd )
    {
        // Initialization is already done, so just return
        return;
    }
    dgst_rcrds.reserve( max_num_rcrd );
    if ( dgst_rcrds.size() == 0 ) dgst_rcrds.push_back( *this );
    h_nmst_hld.reserve( 8 );
    bsv_state_vec.reserve( 10 );
#ifdef INFOdisplay
    r_typ = rec_typ;
    if ( rec_typ == 's' )
    {
        typ_strng = "sha1";
    }
    else if ( rec_typ == 'm' )
    {
        typ_strng = "md5";
    }
    else
    {
        typ_strng = "unknown";
    }
#endif // #ifdef INFOdisplay
    for ( int idx = 0; idx < loc_var.val01; idx ++ )
      iout << loc_var.letters[ idx ] << ' ';
    spr_bsv_state tst_state;
    string d_str;
    dgstHexOut( tst_state.bsv_var, d_str );
    iout << "tested local types, cur bsv " << d_str << "." << endl;
}
