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
// This file contains the code to implement the string pointer record
//   which tracks the location in the name store table of the name
//   string , maintains a record of the encoding type of the original
//   string as well as information about its storage.  As a derived
//   class of gbtree, it provides the support functions for properly
//   collating the strings within the btree so that duplicate strings
//   can be detected during the search phase,  It also provides the
//   computation for maintaining the base search variable that is used
//   by the gbtree base class for dynamically balancing the binary
//   tree.
//
//
// Use the following command to build this object:
//   g++ -std=c++17 -c utf8-rcrd-type.cc
//   ar -Prs ~/data/lib/libfoutil.a utf8-rcrd-type.o
//   ar -Ptv ~/data/lib/libfoutil.a

#include "utf8-rcrd-type.h"
#include "gbst-iface.h"
#include "gbtree.h"
//  #include "fo-utils.h"
#include "../uni-utils/uni-utils.h"
#include "../uni-utils/hex-symbol.h"
#include <cstring>
#include <iomanip>
#include <ctype.h>
#include <cmath>

#ifdef USEncurses
#include "ncursio.h"
#endif   //  #ifdef USEncurses

void atexit_handl_4()
{
    cout << "at exit handler number 4." << endl;
}

ostringstream infstrm;
ostringstream grfstrm;
ostringstream dbstrm;
ostringstream erstrm;

#ifdef INFOdisplay    // Declarations for development only
// This is the base search variable info save stream it needs to be active
//   when the macro INFOdisplay is defined
ostringstream bsvistrm;
ostream& errs = bsvistrm;
ostream& iout = bsvistrm;

#elif defined (USEncurses)

// For now, this is a kind of no op because when USEncurses is defined
//   it automatically defines INFOdisplay as well
ostream& errs = erstrm;
ostream& iout = infstrm;

#else  // not #ifdef INFOdisplay and not defined (USEncurses)

ostream& errs = cout;
ostream& iout = cout;
#endif // #ifdef INFOdisplay

#ifdef USEncurses
ostream& gout = grfstrm;
#else
ostream& gout = cout;
#endif  //  #ifdef USEncurses

#if defined (USEncurses) || !defined (INdevel)
ostream& dbgs = dbstrm;
#else
ostream& dbgs = cout;
#endif  //  #if defined (USEncurses) || !defined (INdevel)

extern significant_collation_chars scc;

//
// Five level version
string ggguniq_str_bal_list[ ggg_bal_lst_siz ] {
  // Temporary comment out for test.
  //   "$", "26e28b", "749c", "adjacf", "arrayp", "bad01u",
  //   "buildj", "classbp", "constan", "db25ma", "dlmall", "ecaa23",
  //   "extras", "fsuite", "gsreu", "igamma", "isvoie", "lfcubi",
  //   "mainpb", "msp430", "oclcpp", "physicb", "printg", "reg908",
  //   "sam4cmp", "segmen", "sockett", "strpya", "testcr", "tommw3",
  //   "underm", "wavets", "zzzzz"
      "00",   "23",   "72",  "add",  "arn",   "b8",
    "bufi", "clas", "cono", "dati",  "div",   "eb",
    "exti",   "fs",  "gsr",   "ig",  "isu",  "lfd",
     "mai",   "ms", "obja",  "phy", "prin",  "reg",
    "sam4",  "sel", "socl", "stru", "test",  "too",
     "uni",   "wd",   "zz"
};

// Four level version
//   string ggguniq_str_bal_list[ ggg_bal_lst_siz ] {
//       "$", "749c", "arrayp", "buildj", "constan",
//       "dlmal", "extras", "gsreu", "iswc", "mainpb",
//       "ocla", "printg", "sam4cmp", "sockett", "testcq",
//       "undem", "zzzzz"
//   };

//
// The base_search_str has been pre-computed for levels 1 to 4 based on the
//   expected distribution of strings to be processed, and are stored in the
//   array ggguniq_str_bal_list of strings.  This int array provides the
//   offset from the current index for the child nodes based on the child
//   level.  The base_search_str for the head of the tree at level 1 is 8 by
//   design, so its level 2 children will have indexes offset 4 from 8, with
//   the left child subtracting 4 from 8 and the right child adding 4 to 8.
//   For levels deeper than 4, the base_search_str needs to be computed
//   (see below).
int new_level_offset_value[ 6 ] { 32, 16, 8, 4, 2, 1};

// GGG - These may not be needed
// name_string_hold::name_string_hold( string nam_strng, int nam_id, uint16_t sz );
// name_string_hold::name_string_hold( string nam_strng, uint16_t sz );

//
// Initialize the static variables for this derived class
//
// This static references the static in utf8-name-store when it is set during
//   the class gbst_interface_type constructor execution.
int utf8_rcrd_type::name_intro_last_idx = 38;
// gbst_interface_type& utf8_rcrd_type::sp_tbl_ref;
scc_idx utf8_rcrd_type::base_search_str_min = {};
// Probably not needed:
//    int utf8_rcrd_type::base_search_str_min_level = 0;
scc_idx utf8_rcrd_type::base_search_str_max = {};
// Probably not needed:
//    int utf8_rcrd_type::base_search_str_max_level = 0;
string utf8_rcrd_type::base_search_str = "";
int utf8_rcrd_type::base_search_last_lvl = 0;
bool utf8_rcrd_type::track_base_search_vars = false;
int utf8_rcrd_type::base_search_str_inf = 0;
scc_idx utf8_rcrd_type::base_search_str_scc = {};
string utf8_rcrd_type::new_name_utf_8 = "";
vector<name_string_hold> utf8_rcrd_type::nmst_hld = {};
vector<utf8_rcrd_type::spr_bss_state>
  utf8_rcrd_type::bss_state_vec = {};
vector<utf8_rcrd_type>
  utf8_rcrd_type::name_string_rcrds = {};
utf8_name_store utf8_rcrd_type::string_table;

void utf8_rcrd_type::init_rcrd()
{
    str_start_idx = 0;
    str_rec_flg = { 1, 1, 1, 0, 0, 0, 3 };
    srchstr_first_ch_idx = 0;
    for ( int idx = 0; idx < 6; idx++ ) srchstr_node_add[ idx ] = { 0, 0, 0 };
}

string utf8_rcrd_type::get_name_string()
{
    // The index for the string table title string end character is
    //   written to the static member name_intro_last_idx by the
    //   gbst_interface_type constructor and should not be
    //   accessable so
    string namstr;
    if ( str_start_idx > name_intro_last_idx )
    {
        namstr = string_table.retrieve_name( str_start_idx );
    }
    else if ( new_name_utf_8.size() > 0 )
    {
         namstr = new_name_utf_8;
    }
    else
    {
        namstr = "No valid string found!";
    }
    return namstr;
}

#ifdef INFOdisplay
void utf8_rcrd_type::push_name_struct( int nm_rc_id )
{
    string t_str( get_name_string() );
    nmst_hld.emplace_back( t_str, lm_nm_str_sz );

#ifdef INdevel    // Declarations for development only
  if ( dbgf.a4 )
  {
    name_string_hold& nm_st_hld = nmst_hld.back();
    dbgs << "Size in push nmst_hld = " << nmst_hld.size() <<
      ", the nam st target is |" << nm_st_hld.nmstr.target <<
      "|, its idxvec is " << nm_st_hld.nmstr.vec_idx <<
      ", the lim nmstr is |" << nm_st_hld.lim_nmstr.target <<
      "|, its idxvec is " << nm_st_hld.lim_nmstr.vec_idx <<
      ", the ivs vector size is " << ivs.size() <<
      ", and ivs[0] is {";
    bool first_out = true;
    for ( auto vnum : ivs[ 0 ] )
    {
        dbgs << ( first_out ? ( first_out = false, " " ) : ", " ) << vnum;
    }
    dbgs << " }." << endl;
  }
#endif // #ifdef INdevel

}

void utf8_rcrd_type::pop_name_struct()
{

#ifdef INdevel    // Declarations for development only
  if ( dbgf.a4 )
  {
    name_string_hold& nm_st_hld = nmst_hld.back();
    dbgs << "Size entering pop nmst_hld = " << nmst_hld.size() <<
      ", the nam st target is |" << nm_st_hld.nmstr.target <<
      "|, its idxvec is " << nm_st_hld.nmstr.vec_idx <<
      ", the lim nmstr is |" << nm_st_hld.lim_nmstr.target <<
      "|, its idxvec is " << nm_st_hld.lim_nmstr.vec_idx <<
      ", the ivs vector size is " << ivs.size() <<
      ", and ivs[0] is {";
    bool first_out = true;
    for ( auto vnum : ivs[ 0 ] )
    {
        dbgs << ( first_out ? ( first_out = false, " " ) : ", " ) << vnum;
    }
    dbgs << " }." << endl;
  }
#endif // #ifdef INdevel

    nmst_hld.pop_back();

#ifdef INdevel    // Declarations for development only
  if ( dbgf.a4 )
  {
    dbgs << "Size in pop nmst_hld = " << nmst_hld.size() <<
      ", the ivs vector size is " << ivs.size() <<
      ", and ivs[0] is {";
    bool first_out = true;
    for ( auto vnum : ivs[ 0 ] )
    {
        dbgs << ( first_out ? ( first_out = false, " " ) : ", " ) << vnum;
    }
    dbgs << " }." << endl;
  }
#endif // #ifdef INdevel

}

void utf8_rcrd_type::get_name_io( string& id_strng )
{
    // Send the first part of the Base search variable setup message
    //   to the info stream.  The rest needs to be sent from the
    //   set_base_srch_var() method before anything else is sent to
    //   the stream.
    int clevel = get_level();
    ostringstream nam_io_strm;
    nam_io_strm << nmst_hld.back().lim_nmstr.target << id_strng << 'u' <<
      setw( lvl_str_siz ) << clevel << " ";
    info_add = nam_io_strm.str();
}

const str_utf8& utf8_rcrd_type::get_rcrd_display_name()
{
    return nmst_hld.back().nmstr;
}
#endif // #ifdef INFOdisplay

bool utf8_rcrd_type::prep4search()
{
    bool ready =
      name_string_rcrds.capacity() > name_string_rcrds.size() + siz_buffer &&
      string_table.name_space_left() > new_name_utf_8.size();
    base_search_last_lvl = 0;
    base_search_str_inf = 0;
    //
    // There may be more that needs to be done, but for now, this will
    //   do.
    return ready;
}

int utf8_rcrd_type::save_bsv_state()
{
    bss_state_vec.emplace_back();
    return bss_state_vec.size() - 1;
}

void utf8_rcrd_type::restore_bsv_state( int sv_idx )
{
    base_search_str_min = bss_state_vec[ sv_idx ].bss_min;
    base_search_str_max = bss_state_vec[ sv_idx ].bss_max;
    base_search_str_scc = bss_state_vec[ sv_idx ].bss_scc;
    //    new_name_utf_8 = bss_state_vec[ sv_idx ].nam_strng;
    base_search_last_lvl = bss_state_vec[ sv_idx ].bs_lst_lvl;
    base_search_str_inf = bss_state_vec[ sv_idx ].bss_inf;
}

void utf8_rcrd_type::release_bsv_state( int sv_idx )
{
    size_t req_idx = sv_idx;
    if ( req_idx == bss_state_vec.size() - 1 )
      bss_state_vec.pop_back();
    else
    {
        errs << "Requested release of bsv_state not at end as expected." <<
          endl;
        bss_state_vec.erase( bss_state_vec.begin() + sv_idx );
    }
}

int utf8_rcrd_type::what_is_my_id()
{
    //
    // GGG - Need to define this method
    size_t myid = name_string_rcrds.size();
    auto arradr = &( name_string_rcrds[ 0 ] );
    auto myadr = this;
    if ( myadr < arradr ||
      !( (myid = (myadr - arradr) ) < name_string_rcrds.size() ) )
    {

#ifdef INdevel   // Declarations/definitions/code for development only
        dbgs << "This record does not belong to the name pointer record set" <<
          " myadr = " << myadr << ", arradr = " << arradr << ", arrsize = " <<
          name_string_rcrds.size() << ", mysize = " << sizeof( *this ) <<
          endl << ", myid = " << myid;
        getout( ".  ", 0 );

#else

        my_exit_msg = "what_is_my_id method called by record that is not"
          " in the record set.";
        myexit();
#endif  //  #ifdef INdevel   // Declarations/definitions/code for development only

    }
    return myid;
}

#ifdef INdevel   // Declarations/definitions/code for development only
// Show the static search parameters
void utf8_rcrd_type::shossp( string intro )
{
    dbgs << intro << ": gbtree level is " << get_level() <<
      endl << ", base_search_str_min \"" <<
      trans_scc( base_search_str_min ) <<
      "\", base_search_str_max \"" <<
      trans_scc( base_search_str_max ) << "\"" << endl <<
      "  str_start_idx " << str_start_idx <<
      ", node 2 base = " << get_nod2bas() <<
      ", base search var count = " << get_b_srch_cnt() <<
      ", base_search_str_inf " << base_search_str_inf <<
      ", base_search_str_scc \"" <<
      trans_scc( base_search_str_scc ) <<
      "\", base_search_str \"" << base_search_str << "\"" << endl <<
      "  srchstr_node_add count = " << get_b_srch_cnt();
    for ( int idx = 1; idx < 6; idx++ ) dbgs << ", el" << idx << ":" <<
      ( srchstr_node_add[ idx ].changed ? 't' : 'f' ) <<
      static_cast<uint16_t>( srchstr_node_add[ idx ].sccidx );
    dbgs << ", new_name_utf_8 \"" << new_name_utf_8 << "\"" << endl;
}

void utf8_rcrd_type::gb_get_out( string intro,
  int nprmt, bool disp_table )
{
    if ( disp_table )
    {
        shofspa( intro );
        getout( "", nprmt );
    }
    else getout( intro, nprmt );
}
#endif // #ifdef INdevel

void utf8_rcrd_type::shofspa( string intro )
{
  struct d
  {
    char t_or_f( uint8_t bit_flag )
    {
        return bit_flag ? 't' : 'f';
    }

    void rcrd( utf8_rcrd_type& forcrd )
    {
        //
        // Using the reference, display the line of information for it
        styp_flags flgs = forcrd.str_rec_flg;
        iout << setw(5) << forcrd.get_parent_idx() <<
        "|" << setw(5) << forcrd.get_child_left_idx() <<
        setw(5) << forcrd.get_child_right_idx() <<
        "|" << setw(6) << forcrd.str_start_idx <<
        " | " << t_or_f( flgs.ASCII_7 ) <<
        "   " << t_or_f( flgs.UTF_8_orig ) <<
        "   " << t_or_f( flgs.UTF_8_compat ) <<
        "   " << t_or_f( flgs.ISO_8859_1 ) <<
        "   " << t_or_f( flgs.UCS_2 ) <<
        "   " << t_or_f( flgs.UTF_16 ) <<
        "   " <<  forcrd.get_nod2bas() <<
        "   " <<  forcrd.get_rt_child_flg() << " " << setw(4) <<
        static_cast<uint16_t>( forcrd.srchstr_first_ch_idx ) <<
        "  " << forcrd.get_asn_cur_search_node() <<
        "  " << forcrd.get_verify_base_srch_var() <<
        "  " << forcrd.get_parent_is_self() <<
        "  " << forcrd.get_new_no_parent() << "|" <<
        setw(2) << forcrd.get_b_srch_cnt() << " :";
        for ( int idx = 0; idx < 6; idx++ ) iout << setw(3) <<
          static_cast<uint16_t>( forcrd.srchstr_node_add[ idx ].sccidx ) <<
          setw(1) <<
          ( forcrd.srchstr_node_add[ idx ].changed ? 't' : 'f' );
        string holdname;
        int snidx = forcrd.str_start_idx;
        if ( snidx > 0 ) holdname = string_table.retrieve_name( snidx );
        else holdname = forcrd.new_name_utf_8;
        iout << ( snidx > 0 ? "|d\"" : "|s\"" ) << holdname << "\"" << endl;
    }
  } disp;

    iout << intro << endl;
    iout <<
    "         |          |string | --------- record flags --- rt" <<
    "-sest as vr pr nw|      srchstr_node_add      |" << endl <<
    "         |  child   |start  |7 b  UTF_8  ISO UCS UTF nd2 ch" <<
    "  1st se bs is no| se| element #[tf](chg flg) |   UTF-8    [d]ynamic" <<
    endl <<
    "Idx  prnt|left right|index  |ASC ori cmp 859  2  16  bas fl" <<
    " cidx nd sv me pr|cnt|  0   1   2   3   4   5 |Name string [s]tatic" <<
    endl;
    // iout <<
    // " 4   5   |  5    5  |  6    | 1   1   1   1   1   1   1   1" <<
    // " [ 4]  1  1  1  1| 1 :  4   4   4   4   4   4 |" << endl;
    int idx = 0;
    for ( utf8_rcrd_type& fo_rec : name_string_rcrds )
    {
        iout << setw(4) << idx++;
        disp.rcrd( fo_rec );
    }
    iout << " new";
    disp.rcrd( *this );
}

void utf8_rcrd_type::disp_name_store()
{
    size_t cur_idx = 0;
    size_t num_chs = 120;
    bool working = pflg.play_name_store_stream;
    while ( working )
    {
        string name_store_segment =
          string_table.get_name_store_chs( cur_idx, num_chs );
        iout << setw( 4 ) << cur_idx << "  ";
        for ( auto ch : name_store_segment )
        {
            if ( isgraph( ch ) ) iout << ch;
            else iout << hex_symbol( cmask & ch );
        }
        iout << endl;
        cur_idx += num_chs;
        if ( name_store_segment.size() < num_chs ) working = false;
    }
}

utf8_rcrd_type::utf8_rcrd_type()
{

#ifdef INdevel
#ifdef USEncurses
    if (foiorf != nullptr ) foiorf->manage_debug_win();
#endif   //  #ifdef USEncurses
#endif // #ifdef INdevel

    // Done below in init_name_str_vector()
    //    if ( in_main == false )
    //    {
    //        const int result_4 = atexit( atexit_handl_4 );
    //        if ( result_4 != 0 ) cout << "atexit reg hdlr 4 fail." << endl;
    //    }
    // Implement a constructor as a test
    init_rcrd();
}

utf8_rcrd_type::utf8_rcrd_type( string new_name,
  styp_flags typ_flgs )
{
    init_rcrd();
    new_name_utf_8 = new_name;
    str_rec_flg = typ_flgs;
}

//
// The get_node method provides a reference to the utf8_rcrd_type
//   node with the requested index so that the appropriate information can be
//   processed as needed to maintain the btree.  That record is a member of
//   the string pointer record array, and the ID is the array index.
//
utf8_rcrd_type& utf8_rcrd_type::get_node( int node_idx )
{
    int val_idx = name_string_rcrds.size();
    if ( node_idx >= 0 && node_idx < val_idx )
      val_idx = node_idx;
    else
    {
        // The node_idx requested was not in the valid range, which makes
        //   the program malformed, so we exit here until the problem is
        //   fixed.
        my_exit_msg = "get_node( int node_idx ) called with invalid node_idx.";
        myexit();
    }
    return name_string_rcrds[ val_idx ];
}

//
// The next three methods perform the necessary function to get the collation
//   order between the respective pairs of strings where new is the new
//   string just read from the system to be inserted into the btree, base is
//   the base_search_str for the current node, and node is the actual string
//   that is currently stored by the node.  The new2base and new2node
//   collations will need to be computed at every new node that the new
//   string arrives at, but the node2base collation only needs to be done once
//   with the result stored as a flag value that can be returned when needed.
// The calls to these methods actually come from the gbtree class which this
//   class inherits from during its search process to find the appropriate
//   resting place for the new string item to be inserted.
//
// This method needs to be called directly from the instance
//   of the node that wants the compare
int utf8_rcrd_type::cmp_node2base( void )
{
    string node_str2cmp = get_name_string();
    int cmp_rslt = strcoll( node_str2cmp.c_str(), base_search_str.c_str() );
    return cmp_rslt;
}

int utf8_rcrd_type::cmp_rcrd2base( int node_idx )
{
    if ( dbgf.a1 )
      dbgs << "where nmst_hld has " << nmst_hld.size() << " elements." << endl;
    if ( dbgf.a1 ) dbgs << "where nmst_hld.back().nmstr.target size is " <<
      nmst_hld.back().nmstr.target.size() << " with value [" <<
      nmst_hld.back().nmstr.target << "]." << endl;
    string st4r2b_cmp = nmst_hld.back().nmstr.target;
    if ( dbgf.a1 ) dbgs << "Got name string [" << st4r2b_cmp << "]." << endl;
    int cmp_rslt = strcoll( st4r2b_cmp.c_str(), base_search_str.c_str() );
    return cmp_rslt;
}

int utf8_rcrd_type::cmp_rcrd2node( int node_idx )
{
    utf8_rcrd_type& node_ref = get_node( node_idx );

#ifdef INdevel
    // Declarations/definitions/code for development only
    if ( node_ref.is_error_set() ) errs << "get_node( " <<
      node_idx << " ) returned an error flag for returned " <<
      "record node_ref to cmp_rcrd2node()." << endl;
#endif // #ifdef INdevel

    string st4r2n_cmp = nmst_hld.back().nmstr.target;
    string node_str2cmp = node_ref.get_name_string();
    int cmp_rslt = strcoll( st4r2n_cmp.c_str(), node_str2cmp.c_str() );
    return cmp_rslt;
}

#ifdef USEmath4base_sss  // Use floating point math method
double utf8_rcrd_type::set_base = 48.0;
double utf8_rcrd_type::set_denom = 1.0;
//
// Gets a floating point value representing the value for the characters
//   in the passed string.  The scc character set contains a set of 48
//   used values, so give the first difference character a ranking from
//   0 to 47 based on the index of the character in the set.  Then the
//   following characters are given decreasing fractional values base
//   48.  At least 7 characters can thus be fully represented in a
//   double precision value.  Since the base search var only needs at
//   most 6 difference characters, this gives an easily implemented
//   method for finding the middle point between the base search
//   variable minimum and maximum values for the new base search var at
//   the current node.
double utf8_rcrd_type::get_dif_value( scc_idx remain_str )
{
    double val = 0.0;
    double denom_val = 1.0;
    if( remain_str.size() > 0 )
    {
        for ( uint16_t sccidv : remain_str )
        {
            if ( sccidv == 0 )
            {
                val = 0.0;
                break;
            }
            val += ( sccidv - 1 ) / denom_val;
            denom_val *= set_base;
        }
    }
    return val;
};
#endif  // #ifdef USEmath4base_sss  // Use floating point math method

//
// The base_search_str has been pre-computed for levels 1 to 5 based on the
//   expected distribution of strings to be processed, and are stored in the
//   arrays ggguniq_str_bal_list and scc_idx_to_str_bal of strings.  The first
//   of the two is the original list of ascii-7 strings, and the second is a
//   version comprised of the scc_set[] indexes for each character in the
//   strings that is computed by the gbst_interface_type constructor.
//   The array int new_level_offset_value[ 6 ] { 32, 16, 8, 4, 2, 1} provides
//   the offset from the current index for the child nodes based on the child
//   level. The base_search_str for the head of the tree at level 1 is 16 by
//   design, so its level 2 children will have indexes offset 8 from 16, with
//   the left child subtracting 8 from 16 and the right child adding 8 to 16.
//   For levels deeper than 5, the base_search_str needs to be computed (see
//   below).
int utf8_rcrd_type::set_base_srch_var()
{
    //
    // Each time the search process goes to a new level, the base_search_str
    //   needs to be computed based on the current btree location.  This is
    //   done by first updating either the base_search_str{_min,_max} and
    //   then by finding a string midway between them.  That can be
    //   accomplished with the aid of the scc_idx_to_str_bal of scc_idx
    //   strings described above for levels 1 - 5, or by computing an
    //   average between the min and max search vars.

#ifdef INdevel    // Declarations for development only
    if ( dbgf.a5 ) dbgs << "set_base_srch_var() process starting:  ";
#endif // #ifdef INdevel

    // Compute these just once in case of error or we need to do some display
    string min_strng = scc_set_array_to_utf8( base_search_str_min );
    string max_strng = scc_set_array_to_utf8( base_search_str_max );
    string grph_min_str = min_strng.size()==1 && !isgraph( min_strng[ 0 ] ) ?
      hex_symbol( min_strng[0] ) : min_strng;

#ifdef INdevel    // Declarations for development only
  if ( dbgf.a5 )
  {
    dbgs << "min_strng: |" << grph_min_str << "|, ";
    dbgs << "max_strng: |" << max_strng << "|, ";
  }
#endif // #ifdef INdevel

    int mnmxcmp = strcoll( min_strng.c_str(), max_strng.c_str() );
    if ( !(mnmxcmp < 0) )
    {
        errs << "Critical error found at " <<
          "utf8_rcrd_type::get_base_srch_var() source line " <<
          __LINE__ << endl << "with the base_search_str_min value \"" <<
          min_strng << "\" not less than base_search_str_max" << endl <<
          "value \"" << max_strng << "\" " << "however, I will allow this to" <<
          endl << "continue to see if the routine below will catch it." << endl;
    }
    scc_idx tmp_base_sss = {};
    // Since the level is a private member of the base class,
    //   get a copy of it
    int clevel = get_level();
    //
    // I don't remember what this one was supposed to be for, but it is
    //   never changed, and it is the value returned at the end of the
    //   method.
    int base_search_supl_cnt = 0;

#ifdef INdevel
    if ( dbgf.a5 || test_bsv_debug )
      dbgs << "In set_base_srch_var() min_strng is [" <<
      min_strng << "], max_strng is [" << max_strng <<
      "], grph_min_str is [" << grph_min_str << "], and level = " <<
      clevel << endl;
#endif  //  #ifdef INdevel

    if ( base_search_last_lvl < clevel )
    {
        bool right_chld = get_rt_child_flg() == 1;
        if ( clevel > 1 )
        {
            //
            // Do common prerequisite operations needed for any level
            //   higher than the initial level one.

#ifdef INdevel    // Declarations for development only
            if ( dbgf.a5 ) dbgs << "current level = " << clevel <<
              ", last level = " << base_search_last_lvl << ", ";
#endif // #ifdef INdevel

            // The base_search_str_min or base_search_str_max and
            //   base_search_str_scc need to be updated and then
            //   base_search_last_lvl incremented once the new base search
            //   variable is found.
            if ( right_chld )
            {
                // Set the base_search_str_min to the value of
                //   base_search_str_scc
                base_search_str_min = base_search_str_scc;
                string min_strng = scc_set_array_to_utf8( base_search_str_min );
            }
            else
            {
                // The new node is a left child so set base_search_str_max
                //   to the value of base_search_str_scc
                base_search_str_max = base_search_str_scc;
                string max_strng = scc_set_array_to_utf8( base_search_str_max );
            }
        }
        //
        // Now proceed with the three base search var setting categories.
        //   Initial level one is unique since it starts the search process
        //   with a clean slate of information.  Then for additional levels
        //   up to level five, a pre-determined set of base search
        //   variables is used, so that is the second category.  For levels
        //   higher than five, a specific algorithm is used to create
        //   successive base search variables that are approximately half
        //   way between the then current base_search_str_min and
        //   base_search_str_max.  The algorithm is intended to be
        //   repeatable for any level given the same set of initial
        //   pre-determined base search variables.
        // If the initial set of pre-determined base search variables is
        //   updated, which may be desireable due to changing distributions
        //   of name strings, the set of existing string pointer records
        //   will need to be refreshed.  Since the existing node IDs will
        //   not change in the refresh process, the entire data base will
        //   not need to be re-done, but the resulting change in the btree
        //   will make it operate more efficiently.
        if ( clevel == 1 )
        {
            // Both the min and max must be set since we are starting at the
            //   head node for the binary tree

#ifdef INdevel    // Declarations for development only
            if ( dbgf.a5 ) dbgs << endl << "level 1 set being initialized, ";
#endif // #ifdef INdevel

            base_search_str_min.clear();
            base_search_str_min.push_back( 0 );
            base_search_str_max.clear();
            base_search_str_max.push_back( scc_set_size - 1 );
            base_search_str_inf = new_level_offset_value[ 1 ];
            base_search_str_scc = scc_idx_to_str_bal[ base_search_str_inf ];

#ifdef INdevel
            if ( dbgf.a5 || test_bsv_debug )
              dbgs << "Level 1 initialize done.  min_strng is [" <<
              min_strng << "], max_strng is [" << max_strng <<
              "], grph_min_str is [" << grph_min_str << "], and level = " <<
              clevel << endl;
#endif  //  #ifdef INdevel

        }
        else if ( clevel <= 5 )
        {
            int lev_offset = new_level_offset_value[ clevel ];
            // Update the index value based on the right child flag
            base_search_str_inf += right_chld ? lev_offset : -lev_offset;
            // Set the new base search scc string value
            base_search_str_scc = scc_idx_to_str_bal[ base_search_str_inf ];
        }
        else
        {
            // This needs to process the levels higher than 5
            // GGG - This development is in process, and is showing signs
            //   of success, but still needs work to insure a repeatable
            //   and accurate base search variable update process.
            // Need to compute the base_search_str from the values in the
            //   variables base_search_str_min and base_search_str_max

#ifdef INdevel    // Declarations for development only
            if ( dbgf.a5 )
              dbgs << endl << "Working at level: " << clevel << ", ";
#endif // #ifdef INdevel

            size_t minsiz = base_search_str_min.size();
            size_t maxsiz = base_search_str_max.size();
            size_t shortest = minsiz < maxsiz ? minsiz : maxsiz;
            size_t longest = minsiz < maxsiz ? maxsiz : minsiz;

#ifdef INdevel    // Declarations for development only
          if ( dbgf.a5 )
          {
            dbgs << "lvl" << clevel << "mnsz" << minsiz << "mxsz" << maxsiz <<
              "bsmin:";
            for ( uint8_t stch : base_search_str_min ) dbgs << (cmask & stch) <<
              ">" << scc_idx_to_UTF_8( stch ) << '|';
            dbgs << "bsmax:";
            for ( uint8_t stch : base_search_str_max ) dbgs << (cmask & stch) <<
              ">" << scc_idx_to_UTF_8( stch ) << '|';
            dbgs << endl;
          }
#endif // #ifdef INdevel

            // Start with finding the first character position where there
            //   is a difference
            size_t difpos = longest + 1;

            for ( size_t idx = 0; idx < shortest && difpos > longest; idx++ )
            {
                if ( base_search_str_min[ idx ] != base_search_str_max[ idx ] )
                  difpos = idx;
                if ( base_search_str_min[ idx ] > base_search_str_max[ idx ] )
                {
                    // Critical error
                    errs << "The get_base_srch_var() method of the" <<
                      " utf8_rcrd_type class found a case where the" <<
                      endl << "base_search_str_min: \"" << min_strng <<
                      "\" is not less than the" << endl <<
                      "base_search_str_max: \"" << max_strng <<
                      "\" which is a critical error.  Exiting at source line " <<
                      __LINE__ << ":" << endl;
                    my_exit_msg = "Search var min not less than max.";
                    myexit();
                }
            }
            if ( difpos > longest )
            {
                // No differences found so far, so any additional characters
                //   need to be in the max search var
                if ( minsiz >= maxsiz )
                {
                    // This case should never happen because it means that
                    //   base_search_str_min is not less than base_search_str_max
                    errs << "The get_base_srch_var() method of the" <<
                      " utf8_rcrd_type class found a case where the" <<
                      endl << "base_search_str_min: \"" << min_strng <<
                      "\" is not less than the" << endl <<
                      "base_search_str_max: \"" << max_strng <<
                      "\" which is a critical error.  Exiting at source line " <<
                      __LINE__ << ":" << endl;
                    my_exit_msg = "Search var min not less than max.";
                    myexit();
                }
                else
                {
                    // since the base_search_str_max has more characters, it
                    //   is larger then base_search_str_min so we need to find a
                    //   base_search_str_scc that is halfway between the two
                    difpos = minsiz;
                    // This will let the calculator function work
                }
            }
            // Start by initializing the base_search_str_scc replacement with
            //   the common characters from both the min and max
            tmp_base_sss = base_search_str_min.substr( 0, difpos );
            //
            // GGG - I want to define an alternate approach to this with
            //   a mathematical approach using a floating point average
            //   between the different substrings for the min and max base
            //   search variables.  I will define a selector macro that
            //   compiles the chosen approach and then add a test method
            //   that can run comparisons of the results as well as the
            //   compute times required for each method by doing a depth
            //   first build of the base search variables for the btree to
            //   any preselected level which can then show the time taken
            //   as well as the differences in the set of base search
            //   variables computed.
            //
            // Now find the scc_set indexes between 1 and 48 that will put the
            //   tmp_base_sss halfway between min and max.

#ifdef USEmath4base_sss  // Use floating point math method
            scc_idx min_remain, max_remain;
            if ( difpos < minsiz ) min_remain =
              base_search_str_min.substr( difpos );
            if ( difpos < maxsiz ) max_remain =
              base_search_str_max.substr( difpos );
            double min_value = get_dif_value( min_remain );
            double max_value = get_dif_value( max_remain );
            if ( !( min_value < max_value ) )
            {
                // This should not happen, but I want to know if it does
                errs << "Variable min_value >= max_value in method " <<
                  "utf8_rcrd_type::get_base_srch_var() level " <<
                  clevel << ".  Exiting at source line " << __LINE__ <<
                  ":" << endl;
                errs <<
                  "with: min search [" << trans_scc( base_search_str_min ) <<
                  "], max search [" << trans_scc( base_search_str_max ) <<
                  "], tmp base sea [" << trans_scc( tmp_base_sss ) << "]," <<
                  endl << "   min remain [" << trans_scc( min_remain ) <<
                  "], max remain [" <<  trans_scc( max_remain ) <<
                  "], min value = " << min_value << ", max value = " <<
                  max_value << ", done." << endl;
                my_exit_msg = "Search var min_value >= max_value.";
                myexit();
            }
            //
            // There are 6 positions in the record to hold base search
            //   variable update values, and when there is an existing
            //   min/max set with a number of common values, having a
            //   couple of those values at the beginning of the set will
            //   provide a confirmation that the set is right, so the
            //   maximum number of new characters will be dependent on
            //   the number of common values.  If there are no common
            //   values, then all 6 positions can be used for difference
            //   and with one common value we can have 5 differences.
            //   With more than 1 common value, only 4 differences will
            //   be used providing the first two positions for common
            //   existing characters.
            //
            int max_newch = difpos < 3 ? 6 - ( difpos + 1 ) / 2 : 4;
            //
            // The increment needs to be one half the maximum for the next
            //   character after max to provide a round up if that
            //   character is more than 24.
            //
            double incrmt = 24.0 / pow( set_base, max_newch );
            //
            //   double incrmt = 0.01 /
            //     ( 48.0 * 48.0 * 48.0 * 48.0 * 48.0 * 48.0 );
            //
            // The average between the min and the max is the best
            //   possible base search variable to promote the
            //   maintenance of a fairly well balanced btree.
            //
            double avg_value = ( min_value + max_value ) / 2 + incrmt;
            int idx;
            for ( idx = 0; idx < max_newch; idx++ )
            {
                uint16_t sccidv = avg_value;
                avg_value -= sccidv;
                tmp_base_sss.push_back( sccidv + 1 );
                // Check to see if all valid info has been used
                if ( avg_value < incrmt * 1.25 ) break;
                incrmt *= set_base;
                avg_value *= set_base;
            }
            // Temporary view of the math processing
            //   grfstrm << "At level " << clevel <<
            //     ", with: min search [" << trans_scc( base_search_str_min ) <<
            //     "], max search [" << trans_scc( base_search_str_max ) <<
            //     "], tmp base sea [" << trans_scc( tmp_base_sss ) <<
            //     "], min remain [" << trans_scc( min_remain ) <<
            //     "], max remain [" <<  trans_scc( max_remain ) <<
            //     "], min value = " << min_value << ", max value = " <<
            //     max_value << ", idx = " << idx << ", avg left = " <<
            //     avg_value << ", and increment = " << incrmt << endl;

#else  // not #ifdef USEmath4base_sss

            int incr = difpos + 1;
            int strength = 0;
            int scc_set_used = scc_set_size - 2;
            int hlf = scc_set_used / 2;
            int hlf1, hlf2, hlf3, tmp;
            int cmp1, cmp2, min1, min2, max1, max2, pushv;
            min1 = difpos < minsiz ? base_search_str_min[ difpos ] : 0;
            max1 = difpos < maxsiz ? base_search_str_max[ difpos ] : 0;
            cmp1 = max1 - min1;

#ifdef INdevel  // Declarations/definitions/code for development only
            if ( dbgf.a5 )
              dbgs << "difpos: " << difpos << ", 1st cmp1: " << cmp1 << ", ";
#endif // #ifdef INdevel

            hlf1 = 0;
            if ( cmp1 & 1 )
            {
                hlf2 = hlf;
                cmp1--;
            }
            else hlf2 = 0;
            while ( incr < longest && strength < 20 )
            {
                min2 = incr < minsiz ? base_search_str_min[ incr ] : 0;
                max2 = incr < maxsiz ? base_search_str_max[ incr ] : 0;

#ifdef INdevel  // Declarations/definitions/code for development only
                if ( dbgf.a5 ) dbgs << "incr: " << incr << ", cmp1: " <<
                  cmp1 << ", min1: " << min1 << ", max1: " << max1 <<
                  ", min2: " << min2 << ", max2: " << max2 << ", ";
#endif // #ifdef INdevel

                cmp1 /= 2;
                min1 += cmp1 + hlf1;
                if ( min1 < 1 || min1 > scc_set_used )
                {
                    // This should not happen, but I want to know if it does
                    errs << "Variable min1 out of range in method " <<
                      "utf8_rcrd_type::get_base_srch_var() level " <<
                      clevel << ".  Exiting at source line " << __LINE__ <<
                      ":" << endl;
                    my_exit_msg = "Search var min1 out of range.";
                    myexit();
                }
                cmp2 = max2 - min2;

#ifdef INdevel  // Declarations/definitions/code for development only
                if ( dbgf.a5 ) dbgs << "calc min1: " << min1 <<
                  ", cmp2: " << cmp2 << ", hlf2: " << hlf2;
#endif // #ifdef INdevel

                if ( cmp2 & 1 )
                {
                    hlf3 = hlf;
                    cmp2--;
                }
                else hlf3 = 0;
                tmp = min2 + cmp2 / 2 + hlf2;

#ifdef INdevel  // Declarations/definitions/code for development only
                if ( dbgf.a5 ) dbgs << ", ini tmp: " << tmp;
#endif // #ifdef INdevel

                if ( tmp < 1 )
                {
                    min1--;
                    min2 += scc_set_used;
                    if ( min1 < 1 )
                    {
                        // Post a temporary error note about the borrow""
                        bsv_note += "Note to user:        A borrow "
                          "is needed and was processed.\n";
                        bsv_carry = true;
                        // Need to borrow from the previous column(s) until
                        //   equilibrium is reached
                        int tstcol = tmp_base_sss.size() - 1;
                        tmp_base_sss[ tstcol ]--;
                        min1 += scc_set_used;
                        while ( tmp_base_sss[ tstcol ] < 1 )
                        {
                            // Need to borrow again from the previous column
                            bsv_note.insert( 15, 1, '+' );
                            if ( tstcol > 0 )
                            {
                                tmp_base_sss[ tstcol ] += scc_set_used;
                                tstcol--;
                                tmp_base_sss[ tstcol ]--;
                            }
                            else
                            {
                                // This should not be possible, so just flag
                                //   it if it happens and get out
                                errs << "Unexpected borrow required which "
                                  "should not be needed which is a critical "
                                  "error." << endl <<
                                  "  Exiting at source line " << __LINE__ <<
                                  ":" << endl;
                                my_exit_msg = "Search var borrow error.";
                                myexit();
                            }
                        }
                    }
                }
                else if ( tmp > scc_set_used )
                {
                    min1++;
                    min2 -= scc_set_used;
                    if ( min1 > scc_set_used )
                    {
                        // Post a temporary error note about the increment
                        bsv_note += "Note to user:        An increment is "
                          "needed and was processed.\n";
                        bsv_carry = true;
                        // Need to increment previous column(s) until
                        //   equilibrium is reached
                        int tstcol = tmp_base_sss.size() - 1;
                        tmp_base_sss[ tstcol ]++;
                        min1 -= scc_set_used;
                        while ( tmp_base_sss[ tstcol ] > scc_set_used )
                        {
                            // Need to increment again from the previous column
                            bsv_note.insert( 15, 1, '+' );
                            if ( tstcol > 0 )
                            {
                                tmp_base_sss[ tstcol ] -= scc_set_used;
                                tstcol--;
                                tmp_base_sss[ tstcol ]++;
                            }
                            else
                            {
                                // This should not be possible, so just flag
                                //   it if it happens and get out
                                errs << "Unexpected increment required which "
                                  "should not be needed which is a critical "
                                  "error." << endl <<
                                  "  Exiting at source line " << __LINE__ <<
                                  ":" << endl;
                                my_exit_msg = "Search var increment error.";
                                myexit();
                            }
                        }
                    }
                }

#ifdef INdevel  // Declarations/definitions/code for development only
                if ( dbgf.a5 ) dbgs << ", vfy tmp: " << tmp <<
                  ", min1: " << min1 << ", min2: " << min2;
#endif // #ifdef INdevel

                tmp_base_sss.push_back( min1 );
                strength += cmp1 < 0 ? -cmp1 : cmp1;

#ifdef INdevel  // Declarations/definitions/code for development only
                if ( dbgf.a5 )
                  dbgs << ", strength: " << strength << "." << endl;
#endif // #ifdef INdevel

                min1 = min2;
                max1 = max2;
                cmp1 = cmp2;
                hlf1 = hlf2;
                hlf2 = hlf3;
                incr++;
            }
            //
            // The following is kind of a kludge, but it is approximately
            //   right.  For the min/max of isvoie/lfcubi it yields k¤¤3fl
            //   when the mathematical average would give k¤¤3eЖ
            cmp1 = max1 - min1;
            min1 += cmp1 / 2;
            tmp_base_sss.push_back( min1 );
            if ( cmp1 & 1 ) tmp_base_sss.push_back( hlf );

#ifdef INdevel  // Declarations/definitions/code for development only
          if ( dbgf.a5 )
          {
            dbgs << "bsscc:";
            for ( uint8_t stch : tmp_base_sss ) dbgs << (cmask & stch) <<
              ">" << scc_idx_to_UTF_8( stch ) << '|';
          }
#endif // #ifdef INdevel
#endif  //  #ifdef USEmath4base_sss  // Use floating point math method

            base_search_str_scc = tmp_base_sss;
        }
        base_search_last_lvl++;
    }
    else return -2; // Unnecessary repeated call to this method

    base_search_str.clear();
    for ( uint8_t blst_ch : base_search_str_scc )
    {
        base_search_str += scc_idx_to_UTF_8( blst_ch );
    }
    if ( clevel == base_search_last_lvl )
    {

#ifdef INdevel    // Declarations for development only
    if ( dbgf.a5 )
      dbgs << "return base search string |" << base_search_str << "|." << endl;
#endif // #ifdef INdevel

    }
    else
    {
        my_exit_msg = "Structural problem with clevel and base_search_last_lvl";
        myexit();
    }

#ifdef INFOdisplay    // Declarations for development only
    //
    // First define the strings
    str_utf8 bsmin( trans_scc( base_search_str_min ), infsea_str_siz,
      fit_center | fit_balance );
    string real_bsmin = scc_set_array_to_utf8( base_search_str_min );
    int cmp_sv2min = strcoll( base_search_str.c_str(), real_bsmin.c_str() );
    str_utf8 bsmax( trans_scc( base_search_str_max ), infsea_str_siz,
      fit_center | fit_balance );
    string real_bsmax = scc_set_array_to_utf8( base_search_str_max );
    int cmp_max2sv = strcoll( real_bsmax.c_str(), base_search_str.c_str() );
    str_utf8 bas_sea_trans( trans_scc( base_search_str_scc ), infsea_str_siz,
      fit_center | fit_balance );
    ostringstream bsv_disp_strm;
    bsv_disp_strm.imbue(std::locale("C"));
    bsv_disp_strm << bsmin.target <<
      ( cmp_sv2min  > 0 ? "↑" : cmp_sv2min  < 0 ? "↓" : "0" ) <<
      bas_sea_trans.target <<
      ( cmp_max2sv  > 0 ? "↑" : cmp_max2sv  < 0 ? "↓" : "0" ) <<
      bsmax.target << setw( id_str_siz ) << what_is_my_id() << " " <<
      get_name_string();
    info_add += bsv_disp_strm.str();

#ifdef INdevel
    if ( dbgf.a5 || test_bsv_debug )
      dbgs << "Finished building info display.  min_strng is [" <<
      min_strng << "], max_strng is [" << max_strng <<
      "], grph_min_str is [" << grph_min_str << "], and level = " <<
      clevel << endl;
#endif  //  #ifdef INdevel

#ifdef USEncurses
    foiorf != nullptr && foiorf->manage_info_win();
    // GGG - May need to add an #else part to handle the info display
    //   when not running under ncurses, or it may just wait until the
    //   end of the run to do the whole display.
#endif   //  #ifdef USEncurses

#endif // #ifdef INFOdisplay

    return base_search_supl_cnt;
}

void utf8_rcrd_type::test_bss_compute()
{
#ifdef INFOdisplay  // test_bsv_compute only works with this set
    new_name_utf_8 = "test";
    base_search_last_lvl = 0;
    base_search_str_inf = 0;

#ifdef INdevel
    if ( test_bsv_debug )
      dbgs << "Calling new_str_ptr.test_bsv_compute()" << endl;
#endif  //  #ifdef INdevel

    test_bsv_compute( nlvl );
#else
    iout << "method test_bss_compute() called without INFOdisplay set." << endl;
#endif // #ifdef INFOdisplay
}

void utf8_rcrd_type::traverse_utf8_records()
{

#ifdef INFOdisplay  // traverse_records will only work with this set
#ifdef INdevel
    test_bsv_debug = dbgf.a0;
#endif  //  #ifdef INdevel

    int saved_state_idx = save_bsv_state();
    base_search_last_lvl = 0;
    base_search_str_inf = 0;
    traverse_records();
    restore_bsv_state( saved_state_idx );
    release_bsv_state( saved_state_idx );

#ifdef INdevel
    test_bsv_debug = false;
#endif  //  #ifdef INdevel
#endif // #ifdef INFOdisplay

}

utf8_rcrd_type&
  utf8_rcrd_type::replace_node_derived( int node_idx )
{
    utf8_rcrd_type& node2replace = get_node( node_idx );

#ifdef INdevel
    // Declarations/definitions/code for development only
    if ( node2replace.is_error_set() ) errs << "get_node( " <<
      node_idx << " ) returned an error flag for returned " <<
      "record node2replace.is_error_set()." << endl;
#endif // #ifdef INdevel

    //
    // GGG - Parts of this base search variable moving need to be done by
    //   the derived class, so need to be done here.
    //
    srchstr_first_ch_idx = node2replace.srchstr_first_ch_idx;
    for ( int idx = 0; idx < 6; idx++ )
      srchstr_node_add[ idx ] = node2replace.srchstr_node_add[ idx ];
    node2replace.srchstr_node_add[ 0 ] = { 0, 0, 0 };
    node2replace.srchstr_node_add[ 1 ] = { 0, 0, 0 };

    return node2replace;
}

int utf8_rcrd_type::add_new_node()
{
    // GGG - To be verified
    //
    // This method adds this node to the data base at the next available
    //   array location by copying the contents of this lone node into
    //   the array at that location.  After this method has executed, no
    //   further changes should be done to this lone node as they will not
    //   appear in any future references to the node actual node.
    str_start_idx = string_table.store_name( new_name_utf_8 );
    int new_str_place = name_string_rcrds.size();
    name_string_rcrds.push_back( *this );
    return new_str_place;
}

void utf8_rcrd_type::init_name_str_vector()
{
    static bool init_done = false;
    if ( init_done )
    {
        // This init has already been done and may only be called once,
        //   print an error message and exit
        my_exit_msg =
          "Repeat UTF-8 Name record init not permitted, exiting program.";
        myexit();
    }
    init_done = true;
    const int result_4 = atexit( atexit_handl_4 );
    if ( result_4 != 0 ) cout << "atexit reg hdlr 4 fail." << endl;

#define BASElistPRINT
#ifdef BASElistPRINT
    // Declarations/definitions/code for development only
    //
    // This section provides a check print out of the reconstructed array
    //   of pre-computed base search strings for the first five levels
    //   that can be enabled when desired by defining the macro specified
    // Since this section will run before ncurses starts, the output can
    //   go to cout without a problem.
    cout << endl;
    bool good_compare = true;
    for ( int idx = 0; idx < ggg_bal_lst_siz; idx++ )
    {
        string tstr;
        string tstro;
        for ( uint8_t blst_ch : scc_idx_to_str_bal[ idx ] )
        {
            string elem = U_code_pt_to_UTF_8( scc_set[ blst_ch ] );
            tstro += elem;
            if ( elem.size() == 1 && !isgraph( elem[ 0 ] ) )
              elem = hex_symbol( cmask & elem[ 0 ] );
            tstr += elem;
        }
        cout << "\"" << tstr <<
          ( idx % 5 == 4 ? "\",\n" : "\", " );
        if ( tstro != ggguniq_str_bal_list[ idx ] ) good_compare = false;
    }
    cout << endl << endl;
    cout << "The reconstructed array " <<
      ( good_compare ? "was" : "was not" ) <<
      " the same as the original" << endl;
#endif // #ifdef BASElistPRINT

    base_search_str_min.push_back( 0 );
    // Probably not needed:
    //    utf8_rcrd_type::base_search_str_min_level = 0;
    base_search_str_max.push_back( scc_set_size - 1 );
    // Probably not needed:
    //    utf8_rcrd_type::base_search_str_max_level = 0;
    base_search_last_lvl = 0;
    base_search_str_inf = 0;
#ifdef INFOdisplay    // Declarations for development only
    track_base_search_vars = true;
    bsvistrm << "Tracking dynamically updated base search variable," <<
      " search node, and searching record at each level." << endl;
    // String above modified from:
    //  bsvistrm << "This stream will keep track of the status of the " <<
    //    "dynamically updated base variable at each level." << endl;
    bsvistrm << " lim name string      id lvl     bsvmin   >≏" <<
      "<   bsvcur   >≏<   bsvmax    snodid node name string" << endl;
    bsvistrm << "--------------------------------------------" <<
      "-----------------------------------------------------" << endl;
#else // Not #ifdef INFOdisplay
    track_base_search_vars = false;
#endif // #ifdef INFOdisplay
    base_search_str_scc = scc_idx_to_str_bal[ base_search_str_inf ];
    base_search_str = ggguniq_str_bal_list[ base_search_str_inf ];

    //
    // initialize fo_string_ptr[ 0 ] as the parent record with a
    //   string equal to the base search string as its string so that
    //   anything else will evaluate as greater since the null string
    //   is not a valid string.
    // The parent record is special in that it doesn't really need to store
    //   a string, it occupies the 0 index of the fo_string_ptr array, it has
    //   no parent, its btree_child_right index points to the head of the
    //   btree, and its btree_child_left index will always be the null value
    //   zero.  Since its only childs parent is 0, when the btree_child_right
    //   node side of tree traversal comes back to the child, the 0 value of
    //   its parent will cause the traversal to terminate which is the proper
    //   response since at that point, the traversal will be complete.
    //
    string bas_srch;
    char u01 = 0x01;
    bas_srch.push_back( u01 );
    str_start_idx = string_table.store_name( bas_srch );
    name_intro_last_idx = string_table.get_intro_last_idx();
    nmst_hld.reserve( 12 );
    bas_srch = "Holding string for safety";
    nmst_hld.emplace_back( bas_srch, 20 );
    bss_state_vec.reserve( 10 );
    name_string_rcrds.reserve( ptr_tbl_max_rcrd );
    name_string_rcrds.push_back( *this );
}

string utf8_rcrd_type::retrieve_orig_name( int fo_spt_idx )
{
    string req_orig_name;
    return req_orig_name;
}

string utf8_rcrd_type::retrieve_utf8_name( int fo_spt_idx )
{
    if ( fo_spt_idx < 0 ||
      static_cast<size_t>( fo_spt_idx ) >= name_string_rcrds.size() )
    {
        //
        // The node_idx requested was not in the valid range, so the node
        //   from the maximum index is provided, but has an error flag set.
        my_exit_msg =
          "retrieve_utf8_name( int fo_spt_idx ) called with invalid node_idx.";
        myexit();
    }
    string req_utf8_name = name_string_rcrds[ fo_spt_idx ].get_name_string();
    return req_utf8_name;
}

