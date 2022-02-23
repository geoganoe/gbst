//
// The gbtree class is the base class for a set of binary tree search
//   and place capabilities.  The goal is to create a generic btree
//   implementation that can handle any type of data easily in a
//   derived class with little extra work.  The first two derived
//   classes will be for 1 > a set of binary byte arrays, and 2 > a
//   set of UTF-8 compatible strings.  It is possible that this
//   should be a template, but I am not comfortable with defining
//   templates, so I want to try this first.  The main enhancement of
//   this effort is the use of a base sorting item for each node in
//   the set which is defined to maintain balance in the tree.  The
//   base sorting item must be a potential member of the data set
//   that is being sorted, and will be the determining factor for
//   choosing the left or right child when a new node is needed.  If
//   the new data set item and the current item in the node are both
//   less than the base sorting item then the lesser of the two is
//   sent to the left child node.  If only the new data set item is
//   less than the base sorting item then it is sent to the left
//   child node.  If the new data set item and the current item in the
//   node are both greater than the base sorting item then the
//   greater of the two is sent to the right child node.  While the
//   base sorting item at any particular node is a constant value, it
//   is a deterministic value that can be dynamically determined
//   whenever the node is designated as the current search node.  The
//   base sorting item can then be a static variable of the class and
//   thus not need to have dedicated space in each record.  At the
//   point where a node is declared to be the current search node, it
//   will be necessary to update the value of the base sorting item
//   to its unique value for that particular btree node position.
//   The base class will need to invoke a virtual method at
//   the point where the base sorting item is needed, and the
//   derived class will need to supply the item.  The collating method
//   will be another virtual method call, as UTF-8 strings will need
//   to use the strcoll() function, but binary byte arrays will need
//   something more like memcmp().
//
// So, a lot of things to get done.  A little long winded, but I think
//   this comment will be very helpful for me.
//
//
//

#ifndef GBTREE_H
#define GBTREE_H

#include "fo-common.h"
#include <iomanip>

using namespace std;

#ifdef INdevel    // Declarations for development only

#endif // #ifdef INdevel

#if defined (INFOdisplay) || defined (INdevel)
#include "fo-utils.h"
#endif  //  defined (INFOdisplay) || defined (INdevel)

#include <cstdint>

// If the number bar28 is subtracted from one of the three node IDs (I. E. -
//   See the bit field declarations below for 28 bit uint32_t variables)
//   when its value is higher than maxid, it will provide a negative integer
//   that can be used as an error indicator or other non_ID purpose.
// This provides the potential for negative values from -1024 to -1.
const int bar28 = 0x10000000;
const int maxid = 0x0ffffbff;

class gbtree
{
    static uint16_t btree_level;

    //
    // There may be a question about why to do this bit field stuff.  The
    //   main reason is the file size consideration.  There is a potential
    //   that as many as 268,434,430 of these records could be used (though
    //   I can not foresee using anywhere near that many myself), and each
    //   time the size of this record increases beyond a 64 bit boundary a
    //   new 64 bit allocation is added to the size of each record.  In a
    //   file with a million records, that is a file size increase of eight
    //   million bytes.  If by chance, 268 million records is not enough
    //   for someone, it would not be hard to expand the number of bits in
    //   the ID bit fields to as much as 32 bits which allows more than
    //   four billion records.  If that is not enough, the declaration
    //   could be changed to uint64_t instead of uint32_t, and with a few
    //   changes to some constants, you can have ID fields with any number
    //   of bits you need. Using 40 bits would give the capability to
    //   handle up to 1.099 trillion records.  However, each record would
    //   take considerably more storage space.
    uint32_t rt_chld_flg : 1;
    uint32_t new_no_parent : 1;
    uint32_t parent_is_self : 1;
    uint32_t spare23flg : 1;
    uint32_t btree_parent : 28;
    uint32_t asn_cur_search_node : 1;  // This may be useful, we will see
    // This is set when the base search variable has been verified for the
    //   current search node.  It is cleared regardless of its setting when
    //   the node is assigned as the search node.
    uint32_t verify_base_srch_var : 1;
    // nod2bas interpretation: nod2bas = [ 0 -> cmp==0 | 1 -> cmp>0 |
    //   2 -> cmp<0 | 3 -> cmp has not been done ]
    uint32_t nod2bas : 2;
    uint32_t btree_child_left : 28;
    // Count of symbols to be copied on to the base search variable
    uint32_t b_srch_cnt : 3;
    uint32_t spare24flg : 1;
    uint32_t btree_child_right : 28;

    //
    // Move these methods to be private as only class members should be
    //   calling the methods.
    int attach_to_leaf( int search_node_id );
    int replace_node( int node_idx );
    int find_my_place( int init_srch_node );
    void do_node_info_update( int nde_id, string updat_str,
      bool replaced_node2leaf = false );

#if defined (INFOdisplay) || defined (INdevel)
protected:
#endif  //  defined (INFOdisplay) || defined (INdevel)

#ifdef INFOdisplay
    virtual void push_name_struct( int rcrd_id ) = 0;
    virtual void pop_name_struct() = 0;
    virtual void get_name_io( string& id_strng ) = 0;
    virtual const str_utf8& get_rcrd_display_name() = 0;
    virtual char get_rcrd_type() = 0;
    virtual string get_type_strng() = 0;
    static uint16_t lm_nm_str_sz;
    static uint16_t id_str_siz;
    static uint16_t lvl_str_siz;
    static uint16_t infsea_str_siz;

    //
    // String needed by the info display to show when a node record is being
    //   replaced in the btree, or when a new or replaced node find their
    //   logical place in the tree
    static string info_add;

    static string bsv_note;
    static bool bsv_carry;
#endif // #ifdef INFOdisplay

#ifdef INdevel
    void set_error_flag();
    bool is_error_set();
#endif // #ifdef INdevel

    static bool in_traverse_mode;
#ifdef INFOdisplay  // traverse_records will only work with this set
    void traverse_records();
#endif // #ifdef INFOdisplay
    //
    // This virtual method must be implemented by all classes derived
    //   from gbtree.  The derived class must do any necessary
    //   preparation that is needed prior to conducting a search and
    //   return the status true if everything is ready.  Items that may
    //   need to be checked include available space for a new gbtree
    //   record in the derived storage spaces, set up of state variables
    //   needed by the derived class, and any other initial preparations
    //   that may be needed.
    virtual bool prep4search() = 0;
    virtual int save_bsv_state() = 0;
    virtual void restore_bsv_state( int sv_idx ) = 0;
    virtual void release_bsv_state( int sv_idx ) = 0;

public:
    int get_parent_idx();
    int get_child_left_idx();
    int get_child_right_idx();
    virtual int what_is_my_id() = 0;

#ifdef INdevel   // Declarations for development only
    virtual void gb_get_out( string intro, int nprmt,
      bool disp_table = false ) = 0;
#endif // #ifdef INdevel

    gbtree();
    int get_level();
    int place_new_node();
#ifdef INFOdisplay  // test_bsv_compute will only work with this set
    void test_bsv_compute( int nlvl );
#endif // #ifdef INFOdisplay

    int get_rt_child_flg();
    int get_b_srch_cnt();
    int get_nod2bas();
    int get_asn_cur_search_node();
    int get_verify_base_srch_var();
    int get_parent_is_self();
    int get_new_no_parent();
    virtual gbtree& get_node( int node_idx ) = 0;
    //
    // cmp_node2base() must be called from the node to be compared
    virtual int cmp_node2base( void ) = 0;
    virtual int cmp_rcrd2base( int node_idx ) = 0;
    virtual int cmp_rcrd2node( int node_idx ) = 0;
    // gbtree will now control this and it should be treated as a command to
    //   the derived class, maybe change the name to set_base_srch_var and
    //   possibly need to have parameters and return variable
    // GGG - Set up for this to be the base search variable control point
    virtual int set_base_srch_var() = 0;
    //
    // The derived class needs to do the replace part for the base search
    //   variable then return the reference it obtains for the node to be
    //   replaced
    virtual gbtree& replace_node_derived( int node_idx ) = 0;
    virtual int add_new_node() = 0;
};

#endif //GBTREE_H
