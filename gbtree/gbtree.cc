
//
// This file contains the code to implement the gbtree class
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
// Use the following commands to build this object, save it to the library,
//    and display the library contents:
//
//   g++ -std=c++17 -c gbtree.cc
//   ar -Prs ~/data/lib/libfoutil.a gbtree.o
//   ar -Ptv ~/data/lib/libfoutil.a

#include "gbtree.h"
#include <iostream>

#ifdef USEncurses
#include "ncursio.h"
#endif   //  #ifdef USEncurses

void atexit_handl_7()
{
    cout << "at exit handler number 7." << endl;
}

uint16_t gbtree::btree_level = 0;

#ifdef INFOdisplay
uint16_t gbtree::lm_nm_str_sz = 20;
uint16_t gbtree::id_str_siz = 4;
uint16_t gbtree::lvl_str_siz = 3;
uint16_t gbtree::infsea_str_siz = 14;
bool gbtree::in_traverse_mode = false;
string gbtree::info_add = "";
#endif  //  #ifdef INFOdisplay

int gbtree::attach_to_leaf( int search_node_id )
{
    //
    // This returns a valid node ID that can be attached to the proper leaf
    //   node pointer, and also returned as the found_index.  It is made
    //   into a private method because it needs to be done in multiple
    //   gbtree method places.
    int my_node_id = -2;
    if ( parent_is_self )
    {
        my_node_id = get_parent_idx();
        btree_parent = search_node_id;
        parent_is_self = 0;
        b_srch_cnt = 7;
    }
    else if ( new_no_parent )
    {
        btree_parent = search_node_id;
        new_no_parent = 0;  // This was the last place it was needed
        my_node_id = add_new_node();
    }
    else
    {

#ifdef INdevel    // Declarations for development only
        gb_get_out(
          "attach_to_leaf() call by an improperly configured record", 2, true );

#else

        my_exit_msg =
          "attach_to_leaf() call by an improperly configured record.";
        myexit();
#endif  //  #ifdef INdevel    // Declarations for development only

    }
    return my_node_id;
}

int gbtree::replace_node( int idx2replac )
{
    //
    // The derived method needs to work with the base method to effect the
    //   replacement of an existing node in the btree with the new node
    //   making this method call.  The base method will take care of all
    //   base btree management members, but some derived members are also
    //   affected and they are handled by replace_node_derived() virtual
    //   method.  Each derived class must handle its own base search
    //   string needs in a replacement scenario and return a reference to
    //   the node being replaced whose ID is given as the parameter.
    gbtree& node2replace = replace_node_derived( idx2replac );
    //
    // The replacing node is initialized with the child flag value for the
    //   node to be replaced, and the parent_is_self and new_no_parent
    //   flags indicate whether the replacing node is a new record or an
    //   existing record searching for its new place.
    // Swap items: rt_chld_flg, btree_parent conditional
    // Move items: btree_child_left, btree_child_right, b_srch_cnt, and
    //   verify_base_srch_var flag
    // Initialization items: nod2bas flag for both reset to 3, for replaced
    //   node btree_child_left and btree_child_right init to 0, b_srch_cnt
    //   init to 7, if child nodes are not leaf nodes, set their parent
    //   pointers to the new_node_id. This (replacing nodes) parents child
    //   node will be updated after the nodes ID is identified (see if test
    //   below)
    int exch_flg = rt_chld_flg;
    rt_chld_flg = node2replace.rt_chld_flg;
    node2replace.rt_chld_flg = exch_flg;
    // The nod2bas is just being initialized to the default value because
    //   the new nodes string is different and the compare will need to
    //   be re-done
    nod2bas = 3;
    node2replace.nod2bas = 3;
    //
    // Move replaced node children to the replacing record
    btree_child_left = node2replace.btree_child_left;
    btree_child_right = node2replace.btree_child_right;
    b_srch_cnt = node2replace.b_srch_cnt;
    verify_base_srch_var = node2replace.verify_base_srch_var;
    //
    // The parent situation is dependent on the status of the replacing
    //   node.  If it is an existing node, it will have the parent_is_self
    //   flag set and its btree_parent field will contain its own ID.  If
    //   it is a new record, it will have the new_no_parent flag set, and
    //   its btree_parent field will be the default -1.  If it is a new
    //   record, we now know it is a unique record, so it needs to be added
    //   to the string pointer record array set and then both cases can be
    //   treated equally.
    int replacing_node_id = -2; // Use unique error number here
    if ( new_no_parent == 1 )
    {
        new_no_parent = 0;
        replacing_node_id = add_new_node();
    }
    else if ( parent_is_self )
    {
        replacing_node_id = btree_parent;
        parent_is_self = 0;
    }
    else
    {
        errs << "In method replace_node(), the replacing node is " << endl <<
          "not properly identified as both parent_is_self and new_no_parent " <<
          "flags are set to 0.  ";
        // This needs to be an exit as it is a serious error that should
        //   not ever happen.
        my_exit_msg =
          "Replacing node is not properly identified in method replace_node()";
        myexit();
    }

#ifdef INFOdisplay    // Declarations for development only
    // Need to replace the word new, if that is the case,
    //   with the found_index, and the following part of
    //   the text with the result explanation
    // the node destination
    //
    //  123456789012345678901234567890123456789012345
    // |isvoie        > oclcpp        > sam4cmp      |
    do_node_info_update( replacing_node_id,
       "is replacing the node at this btree place:" );
#endif // #ifdef INFOdisplay

    DBG_loc_str_num_var( a7, "The node2replace ID is ", idx2replac,
      ", and replacing_node_id is " << replacing_node_id << "." << endl );

// GGG - This worked, but try it as above
//    DBG_loc_str_num( a7, "The node2replace ID is ", idx2replac );
//    DBG_add_str_num_end( a7, ", and replacing_node_id is ",
//      replacing_node_id, "." );

// GGG Testing debugging macros  --  See above
//  #ifdef INdevel    // Declarations for development only
//      if ( dbgf.a7 ) dbgs << "The node2replace ID is " << idx2replac <<
//        ", and replacing_node_id is " << replacing_node_id << "." << endl;
//  #endif // #ifdef INdevel
    gbtree& replacing_node = get_node( replacing_node_id ); // In the fail
                        // condition, a reference to the maximum node index
                        // is returned with an error flag set
    if ( replacing_node.spare23flg ) errs << "get_node( " <<
      replacing_node_id << " ) returned an error flag for returned " <<
      "record replacing_node.spare23flg." << endl;
    replacing_node.btree_parent = node2replace.btree_parent;
    node2replace.btree_parent = idx2replac;
    node2replace.parent_is_self = 1;
    node2replace.new_no_parent = 0;
    node2replace.verify_base_srch_var = 0;
    node2replace.asn_cur_search_node = 0;
    node2replace.btree_child_left = 0;
    node2replace.btree_child_right = 0;
    node2replace.b_srch_cnt = 0;  // GGG - I think this should be set to 7 ??
        // but it doesn't make any difference until the related code is written
    if ( replacing_node.btree_child_left != 0 ) get_node(
      replacing_node.btree_child_left ).btree_parent = replacing_node_id;
    if ( replacing_node.btree_child_right != 0 ) get_node(
      replacing_node.btree_child_right ).btree_parent = replacing_node_id;
    if ( replacing_node.rt_chld_flg ) get_node(
      replacing_node.btree_parent ).btree_child_right = replacing_node_id;
    else get_node( replacing_node.btree_parent ).btree_child_left =
      replacing_node_id;

    DBG_loc_str_num_var( a7, "replace_node() method done, returning "
      "replacing_node_id = ", replacing_node_id,
      " after processing the node2replace." << endl );

// GGG Testing debugging macros  --  See above
//  #ifdef INdevel    // Declarations for development only
//      if ( dbgf.a7 )
//        dbgs << "replace_node() method done, returning replacing_node_id = " <<
//        replacing_node_id << " after processing the node2replace." << endl;
//  #endif // #ifdef INdevel

    if ( node2replace.rt_chld_flg )
    {
        if ( replacing_node.btree_child_right == 0 )
        {
            replacing_node.btree_child_right = idx2replac;
            node2replace.btree_parent = replacing_node_id;
            //
            // GGG - Needed for btree graph - At this point, the
            //   idx2replac node is placed at its destination.

#ifdef INFOdisplay    // Declarations for development only
            // Need to replace the 1st id with replaced node id,
            //   the following part of the text with the result, and
            //   swap the name strings at the beginning and end.
            // the node destination
            //  123456789012345678901234567890123456789012345
            // |isvoie        > oclcpp        > sam4cmp      |
            do_node_info_update( idx2replac,
               "is attached to the right child leaf node of", true );
#endif // #ifdef INFOdisplay

        }
        else
        {
            //
            // GGG - Needed for btree graph - At this point, the
            //   idx2replac node is sent to the next level.
            btree_level++;
            node2replace.find_my_place( replacing_node.btree_child_right );
        }
    }
    else
    {
        if ( replacing_node.btree_child_left == 0 )
        {
            replacing_node.btree_child_left = idx2replac;
            node2replace.btree_parent = replacing_node_id;
            //
            // GGG - Needed for btree graph - At this point, the
            //   idx2replac node is placed at its destination.

#ifdef INFOdisplay    // Declarations for development only
            // Need to replace the 1st id with replaced node id,
            //   the following part of the text with the result, and
            //   swap the name strings at the beginning and end.
            // the node destination
            //  123456789012345678901234567890123456789012345
            // |isvoie        > oclcpp        > sam4cmp      |
            do_node_info_update( idx2replac,
               "is attached to the left child leaf node of", true );
#endif // #ifdef INFOdisplay

        }
        else
        {
            //
            // GGG - Needed for btree graph - At this point, the
            //   idx2replac node is sent to the next level.
            btree_level++;
            node2replace.find_my_place( replacing_node.btree_child_left );
        }
    }
    replacing_node.verify_base_srch_var = 0;
    node2replace.parent_is_self = 0;
    return replacing_node_id;
}

//
// finds the place in the btree where this node should rest.  The node can
//   be either a new string pointer record or it can be a replaced record.
//   It is passed the initial node ID to start the search, with the level
//   already set and needs to return the node id of the place it finds.  If
//   it is a replaced node, it already has a node ID which it will return
//   when it finds its place, and since we know that it is unique
//   as it is a part of the set, it will either replace another
//   node or find a leaf node to bind to.  If it is a new string pointer
//   record, it can find a resting place in three ways.  It can find an
//   equivalent node already in the set, in which case it returns that nodes
//   ID and ends the search as all nodes must be unique.  If the new record
//   finds a node that it needs to replace, it has to be unique since if
//   another equivalent node had done the search, it would have already
//   replaced the node that currently needs to be replaced.  Finally, if the
//   new record finds a leaf node in its search, then it must be unique, and
//   takes its place at the leaf node.  In the process of replacing a node or
//   taking a place at a leaf node, the new record must obtain a node ID.
int gbtree::find_my_place( int init_srch_node )
{

#ifdef INdevel    // Declarations for development only
    struct dbg_ivs_out {
        bool first_out;
        string get_str()
        {
            first_out = true;
            string ivs_str = "{";
            for ( auto vnum : ivs[ 0 ] )
            {
                ivs_str += ( first_out ? ( first_out = false, " " ) : ", " ) +
                to_string( vnum );
            }
            ivs_str += " }.";
            return ivs_str;
        }
    } dio;
#endif // #ifdef INdevel

    int cur_node_id = init_srch_node;
    bool searching = true;
    int found_index = 0;

#ifdef INFOdisplay  //  {
    //
    // GGG - Push this name string onto the name string vector stack to
    //   be used for the necessary display and compares instead of
    //   continually looking it up multiple times for each pass of the
    //   searching while loop below.

    DBG_loc_str_num_var( a4, "Size before push of the ivs vector is ",
      ivs.size(), ", and ivs[0] is " << dio.get_str() << endl );

// GGG Testing debugging macros  --  First test worked but See above
//  #ifdef INdevel    // Declarations for development only
//      bool first_out = true;   // Temporarily needed below
//      if ( dbgf.a4 ) dbgs << "Size before push of the ivs vector is " <<
//        ivs.size() << ", and ivs[0] is " << dio.get_str() << endl;
//  #endif // #ifdef INdevel

// GGG Testing debugging macros  --  See above
//  #ifdef INdevel    // Declarations for development only
      bool first_out = true;   // Temporarily needed below
//    if ( dbgf.a4 )
//    {
//      dbgs << "Size before push of the ivs vector is " << ivs.size() <<
//        ", and ivs[0] is {";
//      for ( auto vnum : ivs[ 0 ] )
//      {
//          dbgs << ( first_out ? ( first_out = false, " " ) : ", " ) << vnum;
//      }
//      dbgs << " }." << endl;
//    }
//  #endif // #ifdef INdevel

    push_name_struct( parent_is_self ? get_parent_idx() : -1 );

#ifdef INdevel    // Declarations for development only
  if ( dbgf.a4 )
  {
    dbgs << "Size after push of the ivs vector is " << ivs.size() <<
      ", and ivs[0] is {";
    first_out = true;
    for ( auto vnum : ivs[ 0 ] )
    {
        dbgs << ( first_out ? ( first_out = false, " " ) : ", " ) << vnum;
    }
    dbgs << " }." << endl;
  }
#endif // #ifdef INdevel
#endif // #ifdef INFOdisplay  }

    //
    // The following declare needs to be one less than the actual level
    //   because it is part of the checking to insure that the level
    //   increases for each while loop iteration.
    int cur_lvl = btree_level - 1;
    while ( searching )
    {
        //
        // The cur_node_id variable contains the ID for the current search
        //   node whether it was set before the loop started, or was found
        //   during a previous iteration of the loop.  We now verify that
        //   the loop is functioning properly then set up and perform this
        //   iterations search.
        if ( cur_lvl == btree_level )
        {

#ifdef INdevel    // Declarations for development only
            errs << "The level " << btree_level <<
              " didn't advance, so in an endless loop. Returning" << endl;
            gb_get_out( "In an endless loop. Returning", 1, true );
            // exit( 1 );
#endif // #ifdef INdevel

            return -1;
        }

#ifdef INdevel    // Declarations for development only
        else
        {
            if ( dbgf.b0 )
              dbgs << "Searching tree at level " << btree_level << " for " <<
              ( new_no_parent ? "new" : "replaced" ) << " record place." <<
              endl;
        }
#endif // #ifdef INdevel

        cur_lvl = btree_level;
        //
        // Start set up for this iteration search.
        gbtree& node_rcrd = get_node( cur_node_id );
        //
        // The base search variable has not yet been updated for this
        //   instance of the assignment as the search node.  The flag
        //   asn_cur_search_node may be useful, but may also just be a
        //   waste of processor time.  It will be implemented for now,
        //   and its justification to stay will be examined as development
        //   progresses.
        if ( node_rcrd.spare23flg ) errs << "get_node( " <<
          cur_node_id << " ) returned an error flag for returned " <<
          "record node_rcrd.spare23flg." << endl;
        node_rcrd.asn_cur_search_node = 1;
        node_rcrd.verify_base_srch_var = 0;
        // At this point, the base search variable is set for the previous
        //   level and needs initialized for the current condition

#ifdef INFOdisplay
        string id_strng;
        if ( new_no_parent ) id_strng = "new";
        else if ( parent_is_self )
        {
            id_strng = to_string( get_parent_idx() );
        }
        int16_t tmp = id_str_siz - id_strng.size();
        if ( tmp > 0 ) id_strng.insert( 0, tmp, ' ' );
        get_name_io( id_strng );
#endif // #ifdef INFOdisplay

        int base_search_var_result = node_rcrd.set_base_srch_var();

#ifdef INFOdisplay    // Declarations for development only
        //
        // Send the line just built by get_name_io and set_base_srch_var
        //   and then hold it in case a test below needs to edit it.
        bsvistrm << info_add << endl;
        if ( bsv_carry )
        {
            errs << bsv_note;
            bsv_carry = false;
            bsv_note = "";

#ifdef USEncurses
            if (foiorf != nullptr ) foiorf->manage_info_win();
#endif   //  #ifdef USEncurses

        }
#endif // #ifdef INFOdisplay

        if ( base_search_var_result < 0 )
        {
            errs << "The set_base_srch_var() method returned error code: " <<
              base_search_var_result << endl;
            if ( base_search_var_result != -2 )
            {
                errs <<
                  "This is not an acceptable error condition and indicates" <<
                  " that code review and correction is needed.  ";

#ifdef INdevel    // Declarations for development only
                gb_get_out( "Please don't continue ", 4 );

#else

                my_exit_msg = "Set base search var critical error detected.";
                myexit();
#endif  //  #ifdef INdevel    // Declarations for development only

            }
        }
        else if ( base_search_var_result < 7 )
        {
            //
            // This could be the count number that is needed to set the
            //   b_srch_cnt field so that future calls to
            //   set_base_srch_var() can be done more efficiently since the
            //   additional characters for the base search var are already
            //   saved. However, that code is not yet written
            //   node_rcrd.b_srch_cnt = base_search_var_result;
            node_rcrd.verify_base_srch_var = 1;
        }

#ifdef NoINdevel    // Declarations for development only
        dbgs << "find_my_place() checking nod2bas ";
#endif // #ifdef NoINdevel

        int node_vs_base;
        int ndrec_nd2bs = node_rcrd.nod2bas;
        if ( ndrec_nd2bs == 3 )
        {
            node_vs_base = node_rcrd.cmp_node2base();

#ifdef INdevel    // Declarations for development only
            if ( dbgf.b0 )
              dbgs << "Called node_rcrd.cmp_node2base() returned " <<
              node_vs_base << "." << endl;
#endif // #ifdef INdevel

            node_rcrd.nod2bas = node_vs_base > 0 ? 1 :
              ( node_vs_base < 0 ? 2 : 0 );
        }
        else
        {

#ifdef INdevel    // Declarations for development only
            if ( dbgf.b0 ) dbgs << "Found nod2bas = " << ndrec_nd2bs << "." << endl;
#endif // #ifdef INdevel

            node_vs_base = ndrec_nd2bs > 1 ? -1 : ndrec_nd2bs;
        }

#ifdef INdevel    // Declarations for development only
        if ( dbgf.b0 ) dbgs << "and returned " << node_vs_base << "." << endl;
        if ( dbgf.b0 ) dbgs << "find_my_place() calling cmp_rcrd2base() ";
#endif // #ifdef INdevel

        int new_vs_base = cmp_rcrd2base( cur_node_id );

#ifdef INdevel    // Declarations for development only
        // int new_vs_base = cmp_rcrd2base( node_rcrd );
        if ( dbgf.b0 ) dbgs << "and returned " << new_vs_base << "." << endl;
        if ( dbgf.b0 ) dbgs << "find_my_place() calling cmp_rcrd2node() ";
#endif // #ifdef INdevel

        int new_vs_node = cmp_rcrd2node( cur_node_id );

#ifdef INdevel    // Declarations for development only
        // int new_vs_node = cmp_rcrd2node( node_rcrd );
        if ( dbgf.b0 ) dbgs << "and returned " << new_vs_node << "." << endl;
        //  dbgs << "Before (loop if node tests), for node_rcrd, its idx is " <<
        //    cur_node_id << ", its parent = " << node_rcrd.btree_parent <<
        //    ", left child = " << node_rcrd.btree_child_left <<
        //    ", and right child = " << node_rcrd.btree_child_right <<
        //    ".  Its parents stats are:" << endl;
#endif // #ifdef INdevel

        gbtree& node_parent_rcrd = get_node( node_rcrd.btree_parent );
        if ( node_parent_rcrd.spare23flg ) errs << "get_node( " <<
          node_rcrd.btree_parent << " ) returned an error flag for returned " <<
          "record node_parent_rcrd.spare23flg." << endl;
        //
        // Ready for if testing sequence to find where the searching record
        //   goes next.
        if ( new_vs_base < 0 && node_vs_base < 0 )
        {
            // since both records are less than the base, the lower of the
            //   two has to be passed to the left child to continue
            //   processing

#ifdef INdevel    // Declarations for development only
            if ( dbgf.b0 )
              dbgs << "find_my_place() both less than base search var." << endl;
#endif // #ifdef INdevel

            rt_chld_flg = 0;
            int nodrec_lfchld = node_rcrd.btree_child_left;
            if ( new_vs_node < 0 )
            {
                // The new node is less so send it on to the left child
                if ( nodrec_lfchld == 0 )
                {
                    // GGG - Needed for btree graph
                    // At a leaf node, attach new node here
                    // Since new string pointer new_str_ptr will be attached
                    //   to this leaf node, its parent will be cur_node_id
                    //   but that needs to be set in different orders
                    //   depending on flags parent_is_self and new_no_parent
                    found_index = attach_to_leaf( cur_node_id );
                    node_rcrd.btree_child_left = found_index;
                    searching = false;

#ifdef INFOdisplay    // Declarations for development only
                    // Need to replace the word new, if that is the case,
                    //   with the found_index, and the following part of
                    //   the text with the result explanation
                    // the node destination
                    //  123456789012345678901234567890123456789012345
                    // |isvoie        > oclcpp        > sam4cmp      |
                    do_node_info_update( found_index,
                       "is attached to the left child leaf node of" );
#endif // #ifdef INFOdisplay

                    node_rcrd.asn_cur_search_node = 0;
                    node_rcrd.verify_base_srch_var = 0;
                }
                else
                {
                    //
                    // GGG - Needed for btree graph - At this point, the
                    //   searching node is sent to the next level.
                    node_rcrd.asn_cur_search_node = 0;
                    node_rcrd.verify_base_srch_var = 0;
                    cur_node_id = nodrec_lfchld;
                    btree_level++;
                }
            }
            else if ( new_vs_node > 0 )
            {
                // GGG - Needed for btree graph
                // The current node is less so make the new node replace
                // the current node and then the current node node_rcrd
                // will initiate its own search
                //
                found_index = replace_node( cur_node_id );
                searching = false;
            }
            else
            {
                // GGG - Needed for btree graph
                // The two node strings are equal, so return to the caller
                //   with the found_index set to the current node id.
                found_index = cur_node_id;
                searching = false;

#ifdef INFOdisplay    // Declarations for development only
                // Need to replace the word new with the found_index,
                //   and the following part of the text with the result
                // the node destination
                //  123456789012345678901234567890123456789012345
                // |isvoie        > oclcpp        > sam4cmp      |
                do_node_info_update( found_index,
                   "not needed as it has same name string as" );
#endif // #ifdef INFOdisplay

            }
        }
        else if ( new_vs_base >= 0 && node_vs_base >= 0 )
        {
            // since both records are greater than the base, the higher of
            //   the two has to be passed to the right child to continue
            //   processing

#ifdef INdevel    // Declarations for development only
            if ( dbgf.b0 ) dbgs << "find_my_place() both greater or"
              " equal base search var." << endl;
#endif // #ifdef INdevel

            //
            // No matter which node moves, this indicator is needed.  If the
            //   new node moves on, it goes to the right child position.  If
            //   the current node is replaced, it needs to move to the right
            //   node position as well, but needs to keep its current child
            //   node flag until it can trade flags at replacement time.
            rt_chld_flg = 1;
            int nodrec_rtchld = node_rcrd.btree_child_right;
            if ( new_vs_node > 0 )
            {
                // The new node is greater so send it on to the right child
                if ( nodrec_rtchld == 0 )
                {
                    // GGG - Needed for btree graph
                    // At a leaf node, attach new node here
                    // Since new string pointer new_str_ptr will be attached
                    //   to this leaf node, its parent will be cur_node_id
                    //   but that needs to be set in different orders
                    //   depending on flags parent_is_self and new_no_parent
                    found_index = attach_to_leaf( cur_node_id );
                    node_rcrd.btree_child_right = found_index;
                    searching = false;

#ifdef INFOdisplay    // Declarations for development only
                    // Need to replace the word new with the found_index,
                    //   and the following part of the text with the result
                    // the node destination
                    //  123456789012345678901234567890123456789012345
                    // |isvoie        > oclcpp        > sam4cmp      |
                    do_node_info_update( found_index,
                       "is attached to the right child leaf node of" );
#endif // #ifdef INFOdisplay

                    node_rcrd.asn_cur_search_node = 0;
                    node_rcrd.verify_base_srch_var = 0;
                }
                else
                {
                    //
                    // GGG - Needed for btree graph - At this point, the
                    //   searching node is sent to the next level.
                    node_rcrd.asn_cur_search_node = 0;
                    node_rcrd.verify_base_srch_var = 0;
                    cur_node_id = nodrec_rtchld;
                    btree_level++;
                }

            }
            else if ( new_vs_node < 0 )
            {
                // GGG - Needed for btree graph
                // The current node is greater so make the new node replace
                // the current node and then the current node node_rcrd
                // will initiate its own search
                found_index = replace_node( cur_node_id );
                searching = false;
            }
            else
            {
                // GGG - Needed for btree graph
                // The two node strings are equal, so return to the caller
                //   with the found_index set to the current node id.
                found_index = cur_node_id;
                searching = false;

#ifdef INFOdisplay    // Declarations for development only
                // Need to replace the word new with the found_index,
                //   and the following part of the text with the result
                // the node destination
                    //  123456789012345678901234567890123456789012345
                    // |isvoie        > oclcpp        > sam4cmp      |
                do_node_info_update( found_index,
                   "not needed as it has same name string as" );
#endif // #ifdef INFOdisplay

            }
        }
        else
        {
            // one record is less than base and the other is greater or
            //   equal, so the lower one must be sent to the left child node
            // No matter whether the new node or the current node must move on,
            //   set the new nodes child flag to left child since the
            //   replace_node() method swaps that status flag and thus if
            //   replace happens both nodes will have the correct flag.

#ifdef INdevel    // Declarations for development only
            if ( dbgf.b0 ) dbgs << "find_my_place() one < and "
              "one >= base search var." << endl;
#endif // #ifdef INdevel

            rt_chld_flg = 0;
            int nodrec_lfchld = node_rcrd.btree_child_left;
            if ( new_vs_node < 0 )
            {
                // The new node is less so send it on to the left child
                if ( nodrec_lfchld == 0 )
                {
                    // GGG - Needed for btree graph
                    // At a leaf node, attach new node here
                    // Since new string pointer new_str_ptr will be attached
                    //   to this leaf node, its parent will be cur_node_id
                    //   but that needs to be set in different orders
                    //   depending on flags parent_is_self and new_no_parent
                    found_index = attach_to_leaf( cur_node_id );
                    node_rcrd.btree_child_left = found_index;
                    searching = false;

#ifdef INFOdisplay    // Declarations for development only
                    // Need to replace the word new with the found_index,
                    //   and the following part of the text with the result
                    // the node destination
                    //  123456789012345678901234567890123456789012345
                    // |isvoie        > oclcpp        > sam4cmp      |
                    do_node_info_update( found_index,
                       "is attached to the left child leaf node of" );
#endif // #ifdef INFOdisplay

                    node_rcrd.asn_cur_search_node = 0;
                    node_rcrd.verify_base_srch_var = 0;
                }
                else
                {
                    //
                    // GGG - Needed for btree graph - At this point, the
                    //   searching node is sent to the next level.
                    node_rcrd.asn_cur_search_node = 0;
                    node_rcrd.verify_base_srch_var = 0;
                    cur_node_id = nodrec_lfchld;
                    btree_level++;
                }
            }
            else
            {
                // GGG - Needed for btree graph
                // The current node is less so make the new node replace
                // the current node and then the current node node_rcrd
                // will initiate its own search
                found_index = replace_node( cur_node_id );
                searching = false;
            }
        }

#ifdef USEncurses
        if (foiorf != nullptr ) foiorf->manage_debug_win();
#endif   //  #ifdef USEncurses

    }

#ifdef INFOdisplay  //  {
#ifdef INdevel    // Declarations for development only
  if ( dbgf.a4 )
  {
    dbgs << "Size before pop of the ivs vector is " << ivs.size() <<
      ", and ivs[0] is {";
    first_out = true;
    for ( auto vnum : ivs[ 0 ] )
    {
        dbgs << ( first_out ? ( first_out = false, " " ) : ", " ) << vnum;
    }
    dbgs << " }." << endl;
  }
#endif // #ifdef INdevel

    pop_name_struct();

#ifdef INdevel    // Declarations for development only
  if ( dbgf.a4 )
  {
    dbgs << "Size after pop of the ivs vector is " << ivs.size() <<
      ", and ivs[0] is {";
    first_out = true;
    for ( auto vnum : ivs[ 0 ] )
    {
        dbgs << ( first_out ? ( first_out = false, " " ) : ", " ) << vnum;
    }
    dbgs << " }." << endl;
  }
#endif // #ifdef INdevel
#endif // #ifdef INFOdisplay  }

    return found_index;
}

#ifdef INFOdisplay
string gbtree::bsv_note = "";
bool gbtree::bsv_carry = false;

void gbtree::do_node_info_update( int nde_id, string updat_str,
  bool replaced_node2leaf )
{
    // This will update the pattern string saved earlier at static var
    //   info_add with the result explanation
    //
    // GGG - This will probably work better, especially once I start dealing
    //   with replaced nodes at the replacing node leaf, if I get the string
    //   segment locations, then create the needed string from substrings
    //   found earlier.  I think I will have enough information to do this
    //   with only one additional optional parameter to this method call
    //   that tells me when I need to swap the names and update the
    //   necessary id information.
    //
    // Need to replace the word new, if that is the case,
    //   with the found_index, and the following part of
    //   the text with the result explanation
    // the node destination
    //  123456789012345678901234567890123456789012345
    // |isvoie        > oclcpp        > sam4cmp      |
    //
    //           1         2         3         4         5         6
    // 0123456789012345678901234567890123456789012345678901234567890
    // lib.mk                  1    3  isvoie        > oclcpp
    //
    //           6         7         8         9
    //           01234567890123456789012345678901234567890
    //             > sam4cmp         3 nldbl-fprintf.c
    //
    str_utf8 info_st( info_add, info_add.size(), fit_wht_trim );
    string& info_cp = info_st.target;
    const index_vec& inf_vec = info_st.get_idxvec();
    // Column position to display the record ID
    uint16_t pos_id = lm_nm_str_sz;
    // Character position to display the record ID
    uint16_t chps_id = inf_vec[ pos_id ];
    // Save the 1st node id in case this is a replaced node attaching to
    //   a replacing node leaf
    string repl_id_str = info_cp.substr( chps_id, 5 ); // gets an extra space
    repl_id_str[ 4 ] = ' ';  // Make sure it is a space
    // update info_add id value
    string nde_id_str = to_string( nde_id );
    if ( nde_id_str.size() < id_str_siz )
      nde_id_str.insert( 0, id_str_siz - nde_id_str.size(), ' ' );
    bool is_a_new_rcrd = repl_id_str.find( "new" ) < repl_id_str.size();
    // Column position for display of the search level
    uint16_t pos_lvl = pos_id + id_str_siz + 1;
    // Column position for display of base search variable info string
    uint16_t pos_bsvstr = pos_lvl + lvl_str_siz + 1;
    // Character position of base search variable info string
    uint16_t chps_bsvstr = inf_vec[ pos_bsvstr ];
    // Number of symbols (columns) for the base search variable info string
    uint16_t nsym_bsv_seg = infsea_str_siz * 3 + 2;
    // Column position for display of search node ID
    uint16_t pos_snid = pos_bsvstr + nsym_bsv_seg;
    // Character position for display of search node ID
    uint16_t chps_snid = inf_vec[ pos_snid ];
    // Number of characters in the base search variable info string
    uint16_t nchr_bsv_seg = chps_snid - chps_bsvstr;
    // Column position for display of search node name string
    uint16_t pos_ndnm = pos_snid + id_str_siz + 1;
    // Character position for display of search node name string
    uint16_t chps_ndnm = inf_vec[ pos_ndnm ];
    // Since the update string is just ASCII characters, this works.  If
    //   an update string with UTF-8 is needed, this will need to change
    uint16_t upd_siz = updat_str.size();
    if ( upd_siz < nsym_bsv_seg )
    {
        int amt_short = nsym_bsv_seg - upd_siz;
        if ( amt_short > 1 )
        {
            updat_str.insert( 0, amt_short / 2, ' ' );
            amt_short = amt_short - amt_short / 2;
        }
        updat_str.append( amt_short, ' ' );
    }
    if ( replaced_node2leaf )
    {
        str_utf8 repl_lim_ns( info_cp.substr( chps_ndnm ), lm_nm_str_sz,
          fit_ends | fit_wht_trim | fit_sp_pad );
        nde_id_str += get_rcrd_type();
        nde_id_str += info_cp.substr( inf_vec[ pos_lvl ], lvl_str_siz + 1 );
        info_add = repl_lim_ns.target;
        info_add += nde_id_str;
        info_add += updat_str;
        info_add += repl_id_str;
        // Can get the whole string by reading it from the derived class
        //   name string hold location through a virtual method call
        info_add += get_rcrd_display_name().target;
    }
    else  // Not a replaced_node2leaf
    {
        info_add = info_cp;
        info_add.replace( chps_id, id_str_siz, nde_id_str );
        info_add.replace( chps_bsvstr, nchr_bsv_seg, updat_str );
    }
    bsvistrm << info_add << endl;
    //
    // This is a very good place to do the play_back_names_read info
    //   output since the repl_id_str string copied above will contain
    //   the word "new" if this is a record that needs to be displayed,
    //   the nde_id parameter that was passed in is the ID for the
    //   record, and the get_rcrd_display_name() method will retrieve a
    //   str_utf8 struct with the full UTF-8 compatible name to be
    //   displayed.
    //
    if ( pflg.play_back_names_read && is_a_new_rcrd )
    {
        const str_utf8& disp_nm = get_rcrd_display_name();
        uint16_t col_num = plbck_strg.numsym;
        if ( col_num > 1 ) col_num += disp_nm.numsym + nde_id_str.size() + 3;
        if ( col_num > inf_wid )
        {
            iout << plbck_strg.target << endl;
            plbck_strg.clear();
            // col_num = nm_frm_file.size() + 10;
        }
        string tmpid = nde_id_str;
        tmpid += " [";
        plbck_strg.append( tmpid );
        plbck_strg.append( disp_nm );
        plbck_strg.append( "]" );
        if ( plbck_strg.numsym > inf_wid - 10 )
        {
            // Not enough space left to wait until the next name is ready
            //   so send it now.
            iout << plbck_strg.target << endl;
            plbck_strg.clear();
        }
    }
}
#endif // #ifdef INFOdisplay

#ifdef INdevel
void gbtree::set_error_flag()
{
    // This flag will serve as an error flag for now as it will only
    //   be queried internally
    spare23flg = 1;
}

bool gbtree::is_error_set()
{
    return spare23flg == 1;
}
#endif // #ifdef INdevel

#ifdef INFOdisplay  // traverse_records will only work with this set
//
// This method can traverse records from any position in the gbtree so long
//   as the state of the gbtree is compatible with the starting record.  If
//   the gbtree_level is 0, the gbtree will be initialized, and the entire
//   tree will be traversed.  If the current record is a found record as a
//   result of a find_my_place() search, the remainder of the tree beyond
//   it will be searched.  Upon entry, if in_traverse_mode is true, the
//   traverse will continue to perform an in progress traverse.  The result
//   is undefined if in_traverse_mode is true and the gbtree state is
//   incompatible with the actual tree position of the current record.
void gbtree::traverse_records()
{
    static string position = "";
    static int counter = 0;
    counter++;

#ifdef INdevel
    if ( test_bsv_debug )
      dbgs << "Entered traverse_records() with counter = " << counter << endl;
#endif  //  #ifdef INdevel

    //   if ( counter > 2500 )
    //   {
    //       my_exit_msg = "traverse_records exceeded the max count of 2500 so "
    //         "exiting the program.";
    //       myexit();
    //   }
    if ( in_traverse_mode )
    {
        string strt_pos = position;
        position.push_back( rt_chld_flg ? 'r' : 'l' );
        str_utf8 pos_fixed( position, 32, fit_center );
        ostringstream bsv_test_disp;
        bsv_test_disp << setw( 4 ) << btree_level <<
          " " << pos_fixed.target;

#ifdef INdevel
        if ( test_bsv_debug )
          dbgs << "Ready to call set_base_srch_var() counter = " <<
          counter << endl;
#endif  //  #ifdef INdevel

        info_add = " ";
        int sbsv_result = set_base_srch_var();
        bsv_test_disp << info_add;
        info_add = "";

#ifdef INdevel
        if ( test_bsv_debug )
          dbgs << "Set bsv result = " <<
          sbsv_result << ", ready to call save_bsv_state() counter = " <<
          counter << endl;
#endif  //  #ifdef INdevel

        int saved_state_idx = save_bsv_state();
        int saved_level = btree_level;
        if ( btree_child_left != 0 )
        {
            // traverse the left child side of the node

#ifdef INdevel
            if ( test_bsv_debug )
              dbgs << "Ready to traverse left child side  counter = " <<
              counter << endl;
#endif  //  #ifdef INdevel

            btree_level++;
            gbtree& nxt_node = get_node( btree_child_left );
            nxt_node.traverse_records();
            restore_bsv_state( saved_state_idx );
            btree_level = saved_level;
        }
        // Now send this nodes data
        iout << bsv_test_disp.str() << endl;

#ifdef USEncurses
        if ( foiorf != nullptr )
        {
            foiorf->manage_debug_win();
            foiorf->manage_info_win();
        }
#endif   //  #ifdef USEncurses

        if ( btree_child_right != 0 )
        {
            // traverse the right child side of the node

#ifdef INdevel
            if ( test_bsv_debug )
              dbgs << "Ready to traverse right child side  counter = " <<
              counter << endl;
#endif  //  #ifdef INdevel

            btree_level++;
            gbtree& nxt_node = get_node( btree_child_right );
            nxt_node.traverse_records();
            restore_bsv_state( saved_state_idx );
            btree_level = saved_level;
        }
        release_bsv_state( saved_state_idx );
        position = strt_pos;
    }
    else
    {
        gbtree& base_parent = get_node( 0 );
        if ( base_parent.btree_child_right == 0 )
        {
            // There is no gbtree to traverse so send the message and return
            iout << "Method traverse_records called with no gbtree"
              " available, so we just return." << endl;
        }
        else
        {
            iout << "Method traverse_records() called with the following"
              " records found" << endl <<
              "Level     gbtree position                 bsvmin   >≏<   "
              "bsvcur   >≏<   bsvmax    snodid node name string" << endl <<
              "-------------------------------------------------------------"
              "-----------------------------------------------------" << endl;
            int saved_state_idx = save_bsv_state();
            int saved_level = btree_level;

#ifdef INdevel
            if ( test_bsv_debug )
              dbgs << "Init traverse, saved_state_idx = " << saved_state_idx <<
              ", ready to call traverse_records() counter = " <<
              counter << endl;
#endif  //  #ifdef INdevel

            btree_level = 1;
            gbtree& top_node = get_node( base_parent.btree_child_right );
            in_traverse_mode = true;
            top_node.traverse_records();
            in_traverse_mode = false;
            restore_bsv_state( saved_state_idx );
            release_bsv_state( saved_state_idx );
            btree_level = saved_level;
        }
    }
}
#endif // #ifdef INFOdisplay

int gbtree::get_parent_idx()
{
    return get_id_value( btree_parent );
}

int gbtree::get_child_left_idx()
{
    return get_id_value( btree_child_left );
}

int gbtree::get_child_right_idx()
{
    return get_id_value( btree_child_right );
}

gbtree::gbtree()
{
    //
    // These members are temporarily being set to artificial values to
    //   check on when initialization is done
    if ( in_main == false )
    {
        const int result_7 = atexit( atexit_handl_7 );
        if ( result_7 != 0 ) cout << "atexit reg hdlr 7 fail." << endl;
    }
    rt_chld_flg = 0;    // It does not really matter, but set anyway
    new_no_parent = 1;
    parent_is_self = 0;
    spare23flg = 0;  // This is a spare for now
    btree_parent = bar28 - 1;
    asn_cur_search_node = 0;  // This node not currently base node for search
    verify_base_srch_var = 0; // Not verified
    nod2bas = 3;        // Compare is not done.
    btree_child_left = 0;
    b_srch_cnt = 7;     // 7 indicates that the base search string needs work
    spare24flg = 0;  // This is a spare for now
    btree_child_right = 0;
}

int gbtree::get_level()
{
    return btree_level;
}

int gbtree::place_new_node()
{
    //
    // The gbtree::place_new_node() method knows that the node with
    //   index zero is the main management node for the data base and
    //   its btree_child_right member houses the index for the node that
    //   is the head of the binary tree, and will start any searches
    //   with the node represented by that index and update that
    //   variable whenever the node index of the head of the tree
    //   changes.
    // The initial level is level 1 which is occupied uniquely by the
    //   head of the tree.  That node will be the starting place for the
    //   search to find the place for this node, which can be a duplicate
    //   of another node and thus not need to be added to the data base,
    //   or it could be attached as a leaf node to an existing node, or
    //   it could be a replacement for a node somewhere in the tree
    //   where it fits better than the node that is already there.  For
    //   the final case, the replaced node will need to start from its
    //   former position and search the remainder of the tree to find
    //   its new place.
    //
    // Initialize the variables that must be maintained throughout
    //   the search process
    // GGG - There may need to be more variables added to this list
    btree_level = 1;
    gbtree& base_parent = get_node( 0 );
    if ( base_parent.btree_child_right == 0 )
    {
        //
        // If we are here, this node is the first to be added to the data
        //   base and thus is a special case.  Once this node is placed in
        //   the head node position, the following nodes can be placed
        //   according to the normal procedure.
        btree_level = 0;
        btree_parent = 0;

#ifdef INFOdisplay
        base_parent.push_name_struct( 0 );
        iout << "The " << get_type_strng() <<
          " base parent display name is [" <<
          get_rcrd_display_name().target << "]." << endl;
        base_parent.pop_name_struct();
#endif  //  #ifdef INFOdisplay

        rt_chld_flg = 1;
        //
        // Doesn't have a base search variable, and doesn't need one
        base_parent.b_srch_cnt = 0;
        base_parent.btree_child_right = add_new_node();

#ifdef INdevel    // Declarations for development only
        if ( dbgf.b1 )
          dbgs << "Btree base node number " << base_parent.btree_child_right <<
          " placed." << endl;
        //  errs << "This is a test to see if errs will be displayed!" << endl;
#endif // #ifdef INdevel

#ifdef INFOdisplay
        push_name_struct( base_parent.btree_child_right );
        if ( pflg.play_back_names_read )
        {
            // This is the first name, so put it in the plbck_strg display
            //   variable to be output when the line fills up
            plbck_strg.append( "   1 [" );

#ifdef INdevel  // Declarations/definitions/code for development only
            if ( dbgf.b1 ) dbgs << "plbck_strg syms 1 = " << plbck_strg.numsym;
#endif // #ifdef INdevel

            plbck_strg.append( get_rcrd_display_name() );

#ifdef INdevel  // Declarations/definitions/code for development only
            if ( dbgf.b1 ) dbgs << ", syms 2 = " << plbck_strg.numsym;
#endif // #ifdef INdevel

            plbck_strg.append( "]" );

#ifdef INdevel  // Declarations/definitions/code for development only
            if ( dbgf.b1 )
              dbgs << ", syms 3 = " << plbck_strg.numsym << "." << endl;
#endif // #ifdef INdevel

        }
        string ts = "   1";
        get_name_io( ts );
        bsvistrm << "Initial name placed from level 0" << endl;
        pop_name_struct();
#endif // #ifdef INFOdisplay

        return base_parent.btree_child_right;
    }

    //
    // This is the main core for normal search and place.  The while loop
    //   in the find_my_place() method that is called searches
    //   down through the levels of the btree until it locates the position
    //   for this node.
    //
    // GGG - I need to think about this.  The constructor for gbtree now
    //   sets the new_no_parent flag to true, so maybe I should just verify
    //   and do something if this method is called from a record that is
    //   already in the gbtree.  While that should not happen and would be
    //   a program bug, checking for it could improve reliability at the
    //   cost of some cpu time.
    // Set new string pointer record flag to indicate that this node has no
    //   parent and is not a member of the string pointer record array set
    //   yet.
    if( new_no_parent != 1 )
    {
        // For now just exit the program since this should not happen
        my_exit_msg = "The place_new_node() method was called from a node "
          "that is not marked new_no_parent.";
        myexit();
    }
    if ( prep4search() != true )
    {
        // Return with 0 status indicates that the search can not be done
        //   most likely because a limit has been reached in an associated
        //   data element.
        return 0;
    }
    int search_node_idx = base_parent.btree_child_right;
    //
    // OK, all set up so call the generic node placement method which
    //   can find the proper place for either new records or replaced
    //   records.

#ifdef USEncurses
    if (foiorf != nullptr ) foiorf->manage_debug_win();
#endif   //  #ifdef USEncurses

    int found_index = find_my_place( search_node_idx );

#ifdef INdevel    // Declarations for development only
    if ( dbgf.b1 ) dbgs << "Finished finding a place for found_index = " <<
      found_index << "." << endl;
#endif // #ifdef INdevel

    return found_index;
}

#ifdef INdevel
bool test_bsv_debug = false;
#endif  //  #ifdef INdevel

#ifdef INFOdisplay  // test_bsv_compute only works with this set
//
// It is assumed that this function is run immediately after the construction
//   of the gbst_interface_type which initializes the btree classes to
//   default initial values.  If called at other times, the results are
//   undefined.
void gbtree::test_bsv_compute( int nlvl )
{
    static string position = "";
    static int counter = 0;
    counter++;

#ifdef INdevel
    if ( test_bsv_debug )
      dbgs << "Entered test_bsv_compute() with counter = " << counter << endl;
#endif  //  #ifdef INdevel

    if ( counter > 20000 )
    {
        my_exit_msg = "test_bsv_compute exceeded the max count of 10000 so "
          "exiting the program.";
        myexit();
    }
    if ( btree_level > 0 )
    {
        string strt_pos = position;
        position.push_back( rt_chld_flg ? 'r' : 'l' );
        str_utf8 pos_fixed( position, 20, fit_center );
        ostringstream bsv_test_disp;
        bsv_test_disp << setw( 4 ) << btree_level <<
          ",  " << pos_fixed.target;

#ifdef INdevel
        if ( test_bsv_debug )
          dbgs << "Ready to call set_base_srch_var() counter = " <<
          counter << endl;
#endif  //  #ifdef INdevel

        info_add = " ";
        set_base_srch_var();
        bsv_test_disp << info_add;
        info_add = "";

#ifdef INdevel
        if ( test_bsv_debug )
          dbgs << "Ready to call save_bsv_state() counter = " <<
          counter << endl;
#endif  //  #ifdef INdevel

        //
        // Need to get the next node then run the test on its left then
        //   right children making sure the rt_chld_flg is properly set
        int saved_state_idx = save_bsv_state();
        int saved_level = btree_level;
        gbtree& nxt_node = get_node( btree_child_left );
        if ( btree_child_left != 0 )
        {
            nxt_node.rt_chld_flg = 0;
            btree_level++;

#ifdef INdevel
            if ( test_bsv_debug )
              dbgs << "Ready to call nxt_node.test_bsv_compute() counter = " <<
              counter << endl;
#endif  //  #ifdef INdevel

            nxt_node.test_bsv_compute( nlvl );
            restore_bsv_state( saved_state_idx );
            btree_level = saved_level;
        }
        iout << bsv_test_disp.str() << endl;
        if ( btree_child_left != 0 )
        {
            nxt_node.rt_chld_flg = 1;
            btree_level++;
            nxt_node.test_bsv_compute( nlvl );
            restore_bsv_state( saved_state_idx );
            btree_level = saved_level;
        }
        release_bsv_state( saved_state_idx );
        position = strt_pos;
    }
    else
    {
        iout << "Method test_bsv_compute() called with the following"
          " positions generated:" << endl <<
          "Level     gbtree position       bsvmin   >≏<   "
          "bsvcur   >≏<   bsvmax    snodid node name string" << endl <<
          "--------------------------------------------------"
          "---------------------------------------------" << endl;
        // Set up everything for doing the testing
        // create the root node
        in_traverse_mode = true;
        int root_idx = add_new_node();
        int parent = root_idx;
        for ( int idx = 1; idx <= nlvl; idx++ )
        {
            // add a node for each level with parent at next lower numbered
            //   level, and both child pointers to the next higher numbered
            //   level.
            int nxt_idx = add_new_node();
            get_node( nxt_idx ).btree_parent = parent;
            gbtree& parent_node = get_node( parent );
            parent_node.btree_child_left = nxt_idx;
            parent_node.btree_child_right = nxt_idx;
            parent = nxt_idx;
        }
        btree_level++;
        gbtree& root_node = get_node( root_idx );
        gbtree& nxt_node = get_node( root_node.btree_child_right );

#ifdef INdevel
        if ( test_bsv_debug )
          dbgs << "Finished setting up testing nodes counter = " <<
          counter << endl;
#endif  //  #ifdef INdevel

        nxt_node.rt_chld_flg = 1;
        nxt_node.test_bsv_compute( nlvl );
    }
}
#endif // #ifdef INFOdisplay

int gbtree::get_rt_child_flg()
{
    return rt_chld_flg;
}

int gbtree::get_b_srch_cnt()
{
    return b_srch_cnt;
}

int gbtree::get_nod2bas()
{
    return nod2bas;
}

int gbtree::get_asn_cur_search_node()
{
    return asn_cur_search_node;
}

int gbtree::get_verify_base_srch_var()
{
    return verify_base_srch_var;
}

int gbtree::get_parent_is_self()
{
    return parent_is_self;
}

int gbtree::get_new_no_parent()
{
    return new_no_parent;
}

