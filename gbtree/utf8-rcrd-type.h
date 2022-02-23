//
// This file is a part of the file organizer string handling classes
//   that are needed for handling the folder and file name strings.
//   These strings can potentially be made from a number of string
//   encoding types, or even no formal encoding at all given the
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
// This file contains the declaration for the string pointer record
//   class that manages the strings saved by the name store class.  The
//   class provides the mechanisms necessary to receive any valid file
//   name string from any of the supported file system types and store
//   them in a UTF-8 compatible and fully bi-directionally convertable
//   null terminated string, while providing a guarantee that only
//   unique string values are stored in the name store table.  It does
//   this by using the gbtree binary tree class to manage the search to
//   find if the string already exists in the name store table and if
//   found returns the id of the existing copy instead of creating a
//   new record.
//

#ifndef UTF8_RCRD_TYPE_H
#define UTF8_RCRD_TYPE_H

// #include <string>    in common included by fo-utils
// #include <iostream>
// #include <sstream>   in common included by fo-utils
// #include <cstdint>   included by fo-utils
#include "fo-utils.h"
#include "gbtree.h"
#include "utf8-name-store.h"

#ifdef INdevel
    // Declarations/definitions/code for development only

#endif // #ifdef INdevel

using namespace std;

struct b2s6 {
    uint8_t changed : 1;
    uint8_t spare : 1;
    uint8_t sccidx : 6;
};

class utf8_rcrd_type : public gbtree
{
    struct spr_bss_state {
        scc_idx bss_min;
        scc_idx bss_max;
        scc_idx bss_scc;
        // probably don't need this    string nam_strng;
        int bs_lst_lvl;
        int bss_inf;

        spr_bss_state()
        {
            bss_min = base_search_str_min;
            bss_max = base_search_str_max;
            bss_scc = base_search_str_scc;
            //    nam_strng = new_name_utf_8;
            bs_lst_lvl = base_search_last_lvl;
            bss_inf = base_search_str_inf;
        }
    };

    int str_start_idx;
    styp_flags str_rec_flg;
    // GGG - Note that the following index has a max value of 255
    uint8_t srchstr_first_ch_idx;
    // Set of indexes into the scc_set array that can provide an addition
    //   of up to five UTF-8 characters to the base_search_str.  The first
    //   element is the number of characters to be added.
    b2s6 srchstr_node_add[ 6 ];
    //
    // GGG - Consider moving the record data from gbst_interface_type
    //   record members to utf8_rcrd_type static data members
    //   such as the vector members used by the hash gbtree derived type.
    //   Such a move could make the code considerably simpler and less
    //   error prone.
    static int name_intro_last_idx;
    static vector<name_string_hold> nmst_hld;
    static vector<spr_bss_state> bss_state_vec;
    static vector<utf8_rcrd_type> name_string_rcrds;
    static utf8_name_store string_table;
    // May not be needed   static int nxt_tbl_index;
    //
    // Try this to use a reference instead of the pointer above
    // Doesn't work, but there should be a way
    //   static gbst_interface_type& sp_tbl_ref;
    // GGG - These five static declarations are meant to be used as the
    //   Values for the lowest, highest, and current base_search_str at the
    //   current node.  They need to be continually updated by the search
    //   code as it searches down into deeper levels.  The _level variables
    //   indicate the level that the related base_search_str is pinned to.
    // Don't care about this   static int base_search_str_min_level;
    // Don't care about this   static int base_search_str_max_level;
    // The base_search_str_{min,max,scc} are sequences of indexes into the
    //   base_search_str values.  Since there is no real need to have actual
    //   character strings for base_search_str_{min,max}, they are only stored
    //   as those index sequences.  However, the base_search_str string needs
    //   to be a legitimate UTF-8 string since it is compared with node
    //   strings during binary tree searches, so it is initialized whenever
    //   the base_search_str_scc changes.
    static scc_idx base_search_str_min;
    static scc_idx base_search_str_max;
    // When at levels 1-5 the base_search_str_inf static variable holds the
    //   current index into the scc_idx_to_str_bal of the base_search_str_scc
    //   string.  For deeper levels, it is used for ... GGG - TBD
    static int base_search_last_lvl;
    static int base_search_str_inf;
    static bool track_base_search_vars;
    static scc_idx base_search_str_scc;
    static string base_search_str;
    // Need to have the new name that is associated with the new_str_ptr
    //   record since it can not be added to the string_table until it is
    //   verified to be a unique new string
    static string new_name_utf_8;

    void init_rcrd();

protected:
    string get_name_string();
#define USEmath4base_sss
#ifdef USEmath4base_sss  // Use floating point math method
    static double set_base;
    static double set_denom;
    double get_dif_value( scc_idx remain_str );
#endif  // #ifdef USEmath4base_sss  // Use floating point math method

#ifdef INFOdisplay

    void push_name_struct( int nm_rc_id );
    void pop_name_struct();
    void get_name_io( string& id_strng );
    const str_utf8& get_rcrd_display_name();
    char get_rcrd_type() { return 'u'; };
    string get_type_strng() { return "UTF-8"; };

#endif  // #ifdef INFOdisplay

    bool prep4search();
    int save_bsv_state();
    void restore_bsv_state( int sv_idx );
    void release_bsv_state( int sv_idx );
    static const int nlvl = 14;

public:
    int what_is_my_id();

#ifdef INdevel   // Declarations/definitions/code for development only
    // Show the static search parameters
    void shossp( string intro );
    void gb_get_out( string intro, int nprmt, bool disp_table = false );
#endif // #ifdef INdevel

    void shofspa( string intro );
    void disp_name_store();

    utf8_rcrd_type();
    utf8_rcrd_type( string new_name, styp_flags typ_flgs );
    //
    // Returns a reference to the record ID passed as the parameter
    // This reference is used to access member and method information about
    //   the various class record instances that are related to the process
    //   being done on the current default class instance.
    utf8_rcrd_type& get_node( int node_idx );
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
    // The set_base_srch_var() method will return the number of characters
    //   stored in the srchstr_node_add array (0-6) when successful, and a
    //   negative value if it detects an error.  This may be changed once
    //   the reliability of the process has been proven
    int set_base_srch_var();
    void test_bss_compute();
    void traverse_utf8_records();
    //
    // The derived class needs to do the replace part for the base search
    //   variable then return the reference it obtains for the node to be
    //   replaced
    utf8_rcrd_type& replace_node_derived( int node_idx );
    //
    // Initializes a node record for this derived class in the data base
    //   array and copies the calling records data to it then returns the
    //   id of the new data base node created.
    int add_new_node();
    void init_name_str_vector();
    string retrieve_orig_name( int fo_spt_idx );
    string retrieve_utf8_name( int fo_spt_idx );
};


#endif  // UTF8_RCRD_TYPE_H
