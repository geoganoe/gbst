//
// The btree_graph class is the class for developing a graphic representation
//   of the btree being created by the application using the class
//
// This file defines the structs and classes needed to implement the required
//   functionality.  See below for that description.
//

#ifndef BTREE_GRAPH_H
#define BTREE_GRAPH_H

#include "fo-common.h"
#include <vector>
#include <iomanip>

// not needed - is in common - using namespace std;

const int path_chn_siz = 500;

struct path_chain_struct
{
    uint32_t chn_spare : 3;
    uint32_t orig_color_idx : 5;
    uint32_t gr_el_idx : 6;
    uint32_t vec_idx : 18;
    wchar_t orig_char;

    path_chain_struct();
};

struct graph_record
{
    uint16_t new_rec_looking : 1;
    uint16_t new_rec_replacing : 1;
    uint16_t exist_replaced_rec : 1;
    uint16_t exi_rec_replacing : 1;
    uint16_t spare04flag : 1;
    uint16_t spare05flag : 1;
    uint16_t spare06flag : 1;
    uint16_t spare07flag : 1;
    uint16_t spare08flag : 1;
    uint16_t spare09flag : 1;
    uint16_t explevel : 6;
    uint16_t path_chain_idx;
    int node_id;  // with -1 indicating a new node until it gets a real ID
    vector<unsigned char> gr_elements;  // string of indexes to wgr_need[] with graph
    string base_sea_var;  // base search variable for this btree location
    string utf_8_name;  // Current name string in this location

    graph_record();
    graph_record( const graph_record& c );
};

class btree_graph
{
    //
    // This class provides the core for generating the btree graphic for
    //   the data set being worked in this invocation of the program.
    //
    // The class will basically use a vector of structs to build up the
    //   information needed for the graphic as the data is read in and
    //   inserted into the gbtree.  For each new entry passed into the
    //   gbtree processing class, this class will accept method calls from
    //   the program at significant places along its route to finding its
    //   position in the btree which will describe the path being taken and
    //   thus tell btree_graph how to draw the section of the tree leading
    //   to the current record position.  That record could be a duplicate
    //   and thus not generate a new node, or it could find a node where it
    //   fits better than the record that is currently there.  In that case,
    //   it would replace that record and the replaced record would need to
    //   continue searching the btree until it finds a new place for itself.
    //   In that process, the replaced node could also find a node where it
    //   fits better than the record that is already there, and thus do its
    //   own replacement.  The process will continue until all of the
    //   current btree records are in their best position for the current
    //   state of the data set.  The process ends when the new record finds
    //   its duplicate that already exists in the btree, when it reaches
    //   a leaf node and is attached there with its own node id, or when a
    //   replaced node finds a leaf node to attach to.
    vector<graph_record> gr_rec_list;
    int lvl1_rcrd_idx;
    graph_record searching_rec;
    path_chain_struct path_chain[ path_chn_siz ];

public:
    btree_graph();
    void initialize( int max_records, int init_node_id,
      string initial_rec_name );
    int start_a_search( int node_id, string record_name );
    int find_next_test_pt( bool is_rt_chld );
    int add_record_here( int node_id, string record_name );
    int replace_this_record( int node_id, string record_name );
};

#endif   //  BTREE_GRAPH_H
