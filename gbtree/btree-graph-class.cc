//
// The btree_graph class is the class for developing a graphic representation
//   of the btree being created by the application using the class
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
// This file provides the implementation for the class defined in the file
//   btree-graph-class.h
//
//   g++ -std=c++17 -c btree-graph-class.cc
//
//

#include "btree-graph-class.h"

//
// The TEST_temp macro when defined is intended to provide a testing
//   playground main program and associated test code for experimenting
//   with ideas related to this module.  The macro definition should be
//   commented out whenever the file organizer programs using this code
//   are being built.
//
// #define TEST_temp
#ifdef TEST_temp  // Macro out this test code
// For testing, use:
//     g++ -std=c++17 -o btree-graph-class btree-graph-class.cc
#include <iostream>
#endif  //  #if ( TEST_temp )  // Macro out this test code

#define DOatexit
#ifdef DOatexit
#include <iostream>

void atexit_handl_8()
{
    cout << "at exit handler number 8." << endl;
}
#endif  //    #ifdef DOatexit

path_chain_struct::path_chain_struct()
{
    chn_spare = 0;
    orig_color_idx = 0;
    gr_el_idx = 0;
    vec_idx = 0;
    orig_char = 0;
}

graph_record::graph_record()
{
    new_rec_looking = 0;
    new_rec_replacing = 0;
    exist_replaced_rec = 0;
    exi_rec_replacing = 0;
    spare04flag = 0;
    spare05flag = 0;
    spare06flag = 0;
    spare07flag = 0;
    spare08flag = 0;
    spare09flag = 0;
    explevel = 0;
    path_chain_idx = 0;
    node_id = -1;  // with -1 indicating a new node until it gets a real ID
}

graph_record::graph_record( const graph_record& c )
{  // Need a copy constructor
    new_rec_looking = c.new_rec_looking;
    new_rec_replacing = c.new_rec_replacing;
    exist_replaced_rec = c.exist_replaced_rec;
    exi_rec_replacing = c.exi_rec_replacing;
    spare04flag = c.spare04flag;
    spare05flag = c.spare05flag;
    spare06flag = c.spare06flag;
    spare07flag = c.spare07flag;
    spare08flag = c.spare08flag;
    spare09flag = c.spare09flag;
    explevel = c.explevel;
    path_chain_idx = c.path_chain_idx;
    node_id = c.node_id;  // with -1 indicating a new node until it gets a real ID

#ifdef TEST_temp  // Macro out this test code
    cout << "Ready to copy the gr_elements vector" << endl;
#endif  //  #ifdef TEST_temp  // Macro out this test code

    gr_elements = c.gr_elements;
    base_sea_var = c.base_sea_var;
    utf_8_name = c.utf_8_name;
}

btree_graph::btree_graph()
{
#ifdef TEST_temp  // Macro out this test code
    cout << "Ready to copy the gr_rec_list vector to the union item" << endl;
    void *tstp = &gr_rec_list;
    uint64_t *tstbu = reinterpret_cast<uint64_t *>( tstp );
    cout << "The gr_rec_list vector has a capacity of " <<
      gr_rec_list.capacity() << " by default, a size of " <<
      sizeof( gr_rec_list ) << " bytes, values" <<
      hex;
    for ( int idx = 0; idx < 3; idx++ )
      cout << " idx: " << idx << " val: " << *(tstbu + idx );
    cout << ", and location: " << &gr_rec_list << dec << "." << endl;
    gr_rec_list.push_back( searching_rec );
    // bsu.tstbv = gr_rec_list;
    cout << "The gr_rec_list vector has a capacity of " <<
      gr_rec_list.capacity() << " after added element, a size of " <<
      sizeof( gr_rec_list ) << " bytes, values" <<
      hex;
    for ( int idx = 0; idx < 3; idx++ )
      cout << " idx: " << idx << " val: " << *(tstbu + idx );
    cout << ", and location: " << &gr_rec_list << dec << "." << endl;
#endif  //  #if ( TEST_temp )  // Macro out this test code
}

#ifdef TEST_temp  // Macro out this test code
// For testing, use:
//     g++ -std=c++17 -o btree-graph-class btree-graph-class.cc

void disp( path_chain_struct& pc, graph_record& gr )
{
    cout << "path_chain_struct: chn_spare " << pc.chn_spare <<
      ", orig_color_idx " << pc.orig_color_idx <<
      ", gr_el_idx " << pc.gr_el_idx << ", vec_idx " <<
      pc.vec_idx << ", orig_char " << pc.orig_char << endl;
    cout << "graph_record: new_rec_looking " << gr.new_rec_looking <<
      ", new_rec_replacing " << gr.new_rec_replacing <<
      ", exist_replaced_rec " << gr.exist_replaced_rec <<
      ", exi_rec_replacing " << gr.exi_rec_replacing <<
      ", spare04flag " << gr.spare04flag <<
      ", spare05flag " << gr.spare05flag <<
      ", spare06flag " << gr.spare06flag << "," << endl <<
      "            spare07flag " << gr.spare07flag <<
      ", spare08flag " << gr.spare08flag <<
      ", spare09flag " << gr.spare09flag <<
      ", explevel " << gr.explevel <<
      ", path_chain_idx " << gr.path_chain_idx <<
      ", node_id " << gr.node_id << endl <<
      ", gr_elements size = " << sizeof( gr.gr_elements ) <<
      ", base_sea_var size = " << sizeof( gr.base_sea_var ) <<
      ", utf_8_name size = " << sizeof( gr.utf_8_name ) <<
      ", gr_elements {";   //  << gr.gr_elements <<
    bool first_out = true;
    for ( auto vnum : gr.gr_elements )
    {
        cout << ( first_out ? ( first_out = false, " " ) : ", " ) << vnum;
    }
    cout << " }, base_sea_var |" << gr.base_sea_var <<
      "|, utf_8_name |" << gr.utf_8_name << "|." << endl;
}

int main()
{
    path_chain_struct pc1, pc2, pc3, pc4, pc5, pc6;
    graph_record gr1, gr2, gr3, gr4, gr5, gr6;
    cout << "Display for pc1 and gr1:" << endl;
    disp( pc1, gr1 );
    cout << "Display for pc2 and gr2:" << endl;
    disp( pc2, gr2 );
    cout << "Display for pc3 and gr3:" << endl;
    disp( pc3, gr3 );
    cout << "Display for pc4 and gr4:" << endl;
    disp( pc4, gr4 );
    cout << "Display for pc5 and gr5:" << endl;
    disp( pc5, gr5 );
    cout << "Display for pc6 and gr6:" << endl;
    disp( pc6, gr6 );
    btree_graph utf8graph;
    void *tstgp = &gr1.gr_elements;
    uint64_t *tstge = reinterpret_cast<uint64_t *>( tstgp );
    // Analyze the memory for the gr1.gr_elements vector
    cout << "The gr1.gr_elements vector has a capacity of " <<
      gr1.gr_elements.capacity() << " by default, a size of " <<
      sizeof( gr1.gr_elements ) << " bytes, values" <<
      hex;
    for ( int idx = 0; idx < 3; idx++ )
      cout << " idx: " << idx << " val: " << *( tstge + idx );
    cout << ", and location: " << &gr1.gr_elements << dec << "." << endl;
    gr1.gr_elements.push_back( 5 );
    // Analyze the memory for the gr1.gr_elements vector after
    //   adding an element
    uint16_t tstge1 = *( reinterpret_cast<unsigned char *>( *tstge ) );
    cout << "The gr1.gr_elements vector has a capacity of " <<
      gr1.gr_elements.capacity() << " after added element, a size of " <<
      sizeof( gr1.gr_elements ) << " bytes, values" <<
      hex;
    for ( int idx = 0; idx < 3; idx++ )
      cout << " idx: " << idx << " val: " << *( tstge + idx );
    cout << ", and location: " << &gr1.gr_elements << dec <<
      ", gev " << tstge1 << endl;
    // Analyze the memory for the testv boolean vector
    vector<bool> testv;
    void *tstp = &testv;
    uint64_t *tstvu = reinterpret_cast<uint64_t *>( tstp );
    cout << "The testv vector has a capacity of " << testv.capacity() <<
      " by default, a size of " << sizeof( testv ) << " bytes, values" <<
      hex;
    for ( int idx = 0; idx < 5; idx++ )
      cout << " idx: " << idx << " val: " << *( tstvu + idx );
    cout << ", and location: " << &testv << dec << "," << endl;
    testv.push_back( true );
    // Analyze the memory for the testv boolean vector after
    //   adding an element
    uint64_t tstba1 = *( reinterpret_cast<uint64_t *>( *tstvu ) );
    cout << "    has a capacity of " << testv.capacity() <<
      " after pushing a single element, with size " << sizeof( testv ) <<
      " bytes, values" << hex;
    for ( int idx = 0; idx < 5; idx++ )
      cout << " idx: " << idx << " val: " << *( tstvu + idx );
    cout << ", and ary " << tstba1 << dec << endl;
    testv.insert( testv.end(), 16, false );
    testv.insert( testv.end(), 16, true );
    // Analyze the memory for the testv boolean vector after
    //   adding 32 more elements
    tstba1 = *( reinterpret_cast<uint64_t *>( *tstvu ) );
    cout << "    and has a capacity of " << testv.capacity() <<
      " after inserting 32 elements, with size " << sizeof( testv ) <<
      " bytes, values" << hex;
    for ( int idx = 0; idx < 5; idx++ )
      cout << " idx: " << idx << " val: " << *( tstvu + idx );
    cout << ", and ary " << tstba1 << dec << endl;
}

#endif  //  #if ( TEST_temp )  // Macro out this test code
