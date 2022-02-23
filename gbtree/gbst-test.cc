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
// Use the following shell command as needed to build the executable:
//   bld-fo-string
// Whenever a new module is added to the build requirements, the shell
//   script ~/data/shell-script/build-fo-string with a link to it in
//   the user bin folder ~/bin which must be in the $PATH bash variable
//   in order for the command bld-fo-string to work.
//
// The following comments describing the build process have been
//   superceded as the number of modules has increased and a shell
//   script has been built to do the build  --  see above  --
//
// Use the following commands as needed to build the executable:
//   pushd ../unicode-utils
//   ar -Prs ~/data/lib/libfoutil.a hex-symbol.o
//   popd
//   ar -Prs ~/data/lib/libfoutil.a fo-string-table.o gbtree.o
//   g++ -std=c++17 -o fo-string-test fo-string-test.cc -L ~/lib -lfoutil
//
// Use the following command to build the executable from objects:
//   g++ -std=c++17 -o fo-string-test fo-string-test.cc fo-string-table.o
//     gbtree.o ../unicode-utils/hex-symbol.o

#include "gbst-iface.h"
#include "hash-rcrd-type.h"
#include "../uni-utils/hex-symbol.h"
#include "../uni-utils/uni-utils.h"
// #include <iostream>
// #include <cstring>
// #include <ctype.h>
#include <fstream>
// #include <sstream>
// #include <iomanip>
#include <chrono>
#include <ctime>
#include <locale>
#include <sys/ioctl.h>
#include <termios.h>

#ifdef USEncurses
#include "ncursio.h"
#endif   //  #ifdef USEncurses

using namespace std;

void atexit_handl_1()
{
    cout << "at exit handler number 1." << endl;
    cout << "Heap info at handler number 1." << endl;
    while ( !heap_mn.is_new_info() ) heap_mn.get_info();
    ostringstream idisp;
    heap_mn.disp_info( idisp );
    cout << idisp.str() << endl;
}
bool in_main = false;

flag_set pflg;
#ifdef INdevel  // Declarations/definitions/code for development only
debug_flags dbgf;
heap_mon_class heap_mn;
#endif  //  #ifdef INdevel

int run_test_set( ifstream& f2proc, int start_str_num = 0 );

int main(int argc, char* argv[])
{
    in_main = true;
    const int result_1 = atexit( atexit_handl_1 );
    const int result_3 = atexit( atexit_handl_3 );
    const int result_8 = atexit( atexit_handl_8 );

    if ( (result_1 != 0) ||  //    (result_2 != 0) ||
      (result_3 != 0) ||
      //   (result_4 != 0) ||      (result_5 != 0) ||
      //   (result_6 != 0) ||      (result_7 != 0) ||
      (result_8 != 0)    //     || (result_9 != 0)
       )
    {
        std::cerr << "Registration failed\n";
        return EXIT_FAILURE;
    }

    using namespace std::chrono;
    using std::chrono::system_clock;
    if (argc < 3)
    {
        //
        //   ͏₁   ⁰̸₁   ¹̸₀  ⁰̷₁   ¹̷₀
        // GGG - Try some fractions here in place of the binary 1s ͘⁰₁ ⁰ ¹ ₀ ₁
        cout << "Not enough args specified.  Must have " <<    // pre-ncurses
          "a binary number representing the set of " << endl <<
          "operations wanted, the file name to process, " <<
#ifdef INdevel
          "the desired" << endl <<
          "starting line in the file if [Change start string number]" <<
          " is set, and the desired" << endl <<
          "debug flag set as a hex value 0xnnnnnnnn " <<
          "defining desired bits if [Debugging on] is set." <<
#else
          "and the desired starting line in" << endl <<
          "the file if [Change start string number] is set." <<
#endif  //  #ifdef INdevel
          endl <<
          " hex                                  0421842184218421" << endl <<
          " code  where the avail operations are 01⅟₀⁰̸₁0/1¹̸₀⁰̷₁¹̷₀1" << endl <<
          "0x0000 To be defined ─────────────────┴┴┘│││││││││││││" << endl <<
          "0x1000 Show gbtree traverse ─────────────┘││││││││││││" << endl <<
          "0x0800 Test base search vars ─────────────┘│││││││││││" << endl <<
          "0x0400 Show Btree graph display ───────────┘││││││││││" << endl <<
          "0x0200 Change start string number ──────────┘│││││││││" << endl <<
          "0x0100 Show default base search vars ────────┘││││││││" << endl <<
          "0x0080 Track base search vars by level ───────┘│││││││" << endl <<
#ifdef INdevel
          "0x0040 Debugging on - flag set rqrd ───────────┘││││││" << endl <<
#else
          "0x0040 No debugging in this build ─────────────┘││││││" << endl <<
#endif  //  #ifdef INdevel
          "0x0020 Read strings from file ──────────────────┘│││││" << endl <<
          "0x0010 Show info output ─────────────────────────┘││││" << endl <<
          "0x0008 Play back names read ──────────────────────┘│││" << endl <<
          "0x0004 Show data record sizes ─────────────────────┘││" << endl <<
          "0x0002 Show strings put in name store ──────────────┘│" << endl <<
          "0x0001 Play back name store as stream ───────────────┘" << endl <<
          "         Exiting ..." << endl;
        exit(1);
        cout << "Went past the exit(1) statement, why?" << endl;
    }
    string arg1 = argv[1];
    size_t fpos;
    long int numxform;
    if ( ( fpos = arg1.find_first_not_of( "01" ) ) < arg1.size() )
    {
        // non binary digit or character found    // pre-ncurses
        // cout << "arg1 is " << arg1 << " and fpos is " << fpos <<
        //   " and arg1 at fpos is " << ( arg1[ fpos ] | 0x20 ) << endl;
        if ( ( arg1[ fpos ] | 0x20 ) == 'x' )
        {
            // assume hex string follows the 'x'
            numxform = stoul( arg1.substr( fpos+1 ), nullptr, 16 );
        }
        else
        {
                // pre-ncurses
            cout << "Argument 1 was not in an accepted form, exiting" << endl;
            exit ( 1 );
        }
    }
    else
    {
        // OK, read the binary string
        numxform = stoul( argv[1], nullptr, 2 );
    }
    pflg.show_gbtree_traverse = ( numxform & 0x1000 ) == 0x1000;
    pflg.test_base_search_vars = ( numxform & 0x0800 ) == 0x0800;
    pflg.change_start_str_num = ( numxform & 0x0200 ) == 0x0200;

    pflg.track_base_search_vars = ( numxform & 0x0080 ) == 0x0080;
    pflg.show_debugging_output = ( numxform & 0x0040 ) == 0x0040;
    pflg.read_strings_from_file = ( numxform & 0x0020 ) == 0x0020;
    pflg.show_info_output = ( numxform & 0x0010 ) == 0x0010;
    pflg.play_back_names_read = ( numxform & 0x0008 ) == 0x0008;
    pflg.show_data_record_sizes = ( numxform & 0x0004 ) == 0x0004;
    pflg.show_name_store_strings = ( numxform & 0x0002 ) == 0x0002;
    pflg.play_name_store_stream = ( numxform & 0x0001 ) == 0x0001;
    pflg.show_any = numxform != 0;
    auto file2process = argv[2];
    int start_str_num = 0;
    // use currently commented out  -    char *entry_locale;
    locale at_start_locale;
    int nxtarg = 3;
    if ( pflg.change_start_str_num && argc > nxtarg )
    {
        start_str_num = stoul( argv[nxtarg], nullptr, 10 );
        nxtarg++;
    }
    else pflg.change_start_str_num = false;
    ifstream f2proc;
    f2proc.open(file2process);
    if (!f2proc.is_open())
    {
        cout << "Could not open file " << file2process <<
          "for reading.  Exiting ..." << endl;    // pre-ncurses
        exit(1);
    }
    else if ( pflg.show_any )
    {
        heap_mn.get_info();
        ostringstream idisp;
        heap_mn.disp_info( idisp );
        cout << idisp.str() << endl;
        cout << "File " << file2process << " is open." << endl;
    }
#ifdef INdevel
    union {
        uint32_t dbg_arg;
        debug_flags tmpf;
    };

    if ( pflg.show_debugging_output && argc > nxtarg )
    {
        string dbg_flg_bits = argv[nxtarg];
        if ( dbg_flg_bits[ 0 ] == '0' && dbg_flg_bits[ 1 ] == 'x' )
        {
            dbg_arg = stoul( dbg_flg_bits.substr( 2 ), nullptr, 16 );
            dbgf = tmpf;
            nxtarg++;

        // This has been changed and moved below to show the final
        //   debug_flags status.
        //    if( dbgf.a0 ) { flg_set += ", a0"; nflgs++; }
        //    if( dbgf.a1 ) { flg_set += ", a1"; nflgs++; }
        //    if( dbgf.a2 ) { flg_set += ", a2"; nflgs++; }
        //    if( dbgf.a3 ) { flg_set += ", a3"; nflgs++; }
        //    if( dbgf.a4 ) { flg_set += ", a4"; nflgs++; }
        //    if( dbgf.a5 ) { flg_set += ", a5"; nflgs++; }
        //    if( dbgf.a6 ) { flg_set += ", a6"; nflgs++; }
        //    if( dbgf.a7 ) { flg_set += ", a7"; nflgs++; }
        //    if( dbgf.b0 ) { flg_set += ", b0"; nflgs++; }
        //    if( dbgf.b1 ) { flg_set += ", b1"; nflgs++; }
        //    if( dbgf.b2 ) { flg_set += ", b2"; nflgs++; }
        //    if( dbgf.b3 ) { flg_set += ", b3"; nflgs++; }
        //    if( dbgf.b4 ) { flg_set += ", b4"; nflgs++; }
        //    if( dbgf.b5 ) { flg_set += ", b5"; nflgs++; }
        //    if( dbgf.b6 ) { flg_set += ", b6"; nflgs++; }
        //    if( dbgf.b7 ) { flg_set += ", b7"; nflgs++; }
        //    if( dbgf.c0 ) { flg_set += ", c0"; nflgs++; }
        //    if( dbgf.c1 ) { flg_set += ", c1"; nflgs++; }
        //    if( dbgf.c2 ) { flg_set += ", c2"; nflgs++; }
        //    if( dbgf.c3 ) { flg_set += ", c3"; nflgs++; }
        //    if( dbgf.c4 ) { flg_set += ", c4"; nflgs++; }
        //    if( dbgf.c5 ) { flg_set += ", c5"; nflgs++; }
        //    if( dbgf.c6 ) { flg_set += ", c6"; nflgs++; }
        //    if( dbgf.c7 ) { flg_set += ", c7"; nflgs++; }
        //    if( dbgf.d0 ) { flg_set += ", d0"; nflgs++; }
        //    if( dbgf.d1 ) { flg_set += ", d1"; nflgs++; }
        //    if( dbgf.d2 ) { flg_set += ", d2"; nflgs++; }
        //    if( dbgf.d3 ) { flg_set += ", d3"; nflgs++; }
        //    if( dbgf.d4 ) { flg_set += ", d4"; nflgs++; }
        //    if( dbgf.d5 ) { flg_set += ", d5"; nflgs++; }
        //    if( dbgf.d6 ) { flg_set += ", d6"; nflgs++; }
        //    if( dbgf.d7 ) { flg_set += ", d7"; nflgs++; }
        //    cout << flg_set << ( nflgs > 1 ? " flags are set." :
        //      nflgs == 1 ? " flag is set." : " no flags are set." ) << endl;
        }
        else
        {
            cout << "Invalid debug flag input argument [" << dbg_flg_bits <<
              "] received.  Exiting ..." << endl;    // pre-ncurses
            exit(1);
        }
    }
    else
    {
        cout << "No debugging has been selected for this run." << endl;
        dbg_arg = 0;
        dbgf = tmpf;
    }
    cout << "The dbg_arg = 0x" <<
      hex << setw( 8 ) << setfill( '0' ) << dbg_arg;
    int nflgs = 0;
    string flg_set = "";
    if( dbgf.a0 ) { flg_set.insert( 0, ", a0" ); nflgs++; }
    if( dbgf.a1 ) { flg_set.insert( 0, ", a1" ); nflgs++; }
    if( dbgf.a2 ) { flg_set.insert( 0, ", a2" ); nflgs++; }
    if( dbgf.a3 ) { flg_set.insert( 0, ", a3" ); nflgs++; }
    if( dbgf.a4 ) { flg_set.insert( 0, ", a4" ); nflgs++; }
    if( dbgf.a5 ) { flg_set.insert( 0, ", a5" ); nflgs++; }
    if( dbgf.a6 ) { flg_set.insert( 0, ", a6" ); nflgs++; }
    if( dbgf.a7 ) { flg_set.insert( 0, ", a7" ); nflgs++; }
    if( dbgf.b0 ) { flg_set.insert( 0, ", b0" ); nflgs++; }
    if( dbgf.b1 ) { flg_set.insert( 0, ", b1" ); nflgs++; }
    if( dbgf.b2 ) { flg_set.insert( 0, ", b2" ); nflgs++; }
    if( dbgf.b3 ) { flg_set.insert( 0, ", b3" ); nflgs++; }
    if( dbgf.b4 ) { flg_set.insert( 0, ", b4" ); nflgs++; }
    if( dbgf.b5 ) { flg_set.insert( 0, ", b5" ); nflgs++; }
    if( dbgf.b6 ) { flg_set.insert( 0, ", b6" ); nflgs++; }
    if( dbgf.b7 ) { flg_set.insert( 0, ", b7" ); nflgs++; }
    if( dbgf.c0 ) { flg_set.insert( 0, ", c0" ); nflgs++; }
    if( dbgf.c1 ) { flg_set.insert( 0, ", c1" ); nflgs++; }
    if( dbgf.c2 ) { flg_set.insert( 0, ", c2" ); nflgs++; }
    if( dbgf.c3 ) { flg_set.insert( 0, ", c3" ); nflgs++; }
    if( dbgf.c4 ) { flg_set.insert( 0, ", c4" ); nflgs++; }
    if( dbgf.c5 ) { flg_set.insert( 0, ", c5" ); nflgs++; }
    if( dbgf.c6 ) { flg_set.insert( 0, ", c6" ); nflgs++; }
    if( dbgf.c7 ) { flg_set.insert( 0, ", c7" ); nflgs++; }
    if( dbgf.d0 ) { flg_set.insert( 0, ", d0" ); nflgs++; }
    if( dbgf.d1 ) { flg_set.insert( 0, ", d1" ); nflgs++; }
    if( dbgf.d2 ) { flg_set.insert( 0, ", d2" ); nflgs++; }
    if( dbgf.d3 ) { flg_set.insert( 0, ", d3" ); nflgs++; }
    if( dbgf.d4 ) { flg_set.insert( 0, ", d4" ); nflgs++; }
    if( dbgf.d5 ) { flg_set.insert( 0, ", d5" ); nflgs++; }
    if( dbgf.d6 ) { flg_set.insert( 0, ", d6" ); nflgs++; }
    if( dbgf.d7 ) { flg_set.insert( 0, ", d7" ); nflgs++; }
    cout << flg_set << ( nflgs > 1 ? " flags are set." :
      nflgs == 1 ? " flag is set." : " no flags are set." ) << endl;
#endif  //  #ifdef INdevel
    //
    // GGG - This section needs to be reviewed as there appears to be some
    //   settings made in the if statement [if ( pflg.show_any )] that will
    //   be needed by the program even when the if statment is false.  The
    //   code will likely need to be divided into two if statements with
    //   set up statements in between them.
    //
    // Display the opening announcement
    auto ct1 = chrono::system_clock::now();
    time_t tt = system_clock::to_time_t (ct1);
    struct tm * ptm = std::localtime(&tt);
    if ( pflg.show_any )
    {
            // pre-ncurses
        cout << endl << endl << "                                         " <<
          "Welcome to the File Organizer Name String test program" << endl <<
          "                                                 " <<
          "Current time:    " << put_time(ptm,"%F  %T") << endl <<
          "                                    " <<
          "Using locale: default " << locale().name() << ", default set " <<
          locale("").name() << ", read after set " << locale().name() << endl;
        at_start_locale = locale::global( locale( "en_US.UTF-8" ) );
        // entry_locale = setlocale(LC_ALL, "en_US.UTF-8");
            // pre-ncurses
        cout << "                                " <<
          "After set locale: default " << locale().name() << ", default set " <<
          locale("").name() << ", read after set " << locale().name() <<
          endl << endl;
    }

    uint16_t cmask = 0x00ff;
    if ( pflg.show_info_output )
    {
            // pre-ncurses
        while ( !heap_mn.is_new_info() ) heap_mn.get_info();
        ostringstream idisp;
        heap_mn.disp_info( idisp );
        cout << idisp.str() << endl;
        cout << endl <<
          "This is the fo-string-test program designed to test " << endl <<
          "the name string management for the file organizer." << endl;
        cout << "File to process is " <<  file2process << endl << "and" <<
          " performing operations for the highlighted bits in dot pattern ";
        if ( numxform > 255 )
          cout << hex_symbol( cmask & (numxform >> 8) );
        cout << hex_symbol( cmask & numxform ) << " hex code 0x" <<
          hex << numxform << dec << endl;
    }
    if ( pflg.change_start_str_num )
      run_test_set( f2proc, start_str_num );
    else
      run_test_set( f2proc );

#ifdef INFOdisplay    // Declarations for development only
    ostringstream stime;
    stime << "temp/fo_info_" << put_time(ptm,"%F_%H-%M-%S") << ".txt";
    ofstream f4info;
    f4info.open(stime.str());
    if ( pflg.track_base_search_vars && bsvistrm.tellp() > 0 ) f4info <<
      "Saved Base Search variable monitor info is as follows:" << endl <<
      bsvistrm.str() << endl;
    // Temporary view of the math processing
    //   if ( grfstrm.str().size() > 5 ) f4info << grfstrm.str() << endl;
#endif // #ifdef INFOdisplay
    cout << grfstrm.str() << endl;
    cout << "Heap info at end of fo-string-test program." << endl;
    while ( !heap_mn.is_new_info() ) heap_mn.get_info();
    ostringstream idisp;
    heap_mn.disp_info( idisp );
    cout << idisp.str() << endl;
    cout << "ending program and returning to the terminal prompt after"
      " running all destructors and cleaning memory." << endl;
    in_main = false;
    return 0;
}

#ifdef USEncurses
ncursio *foiorf = nullptr;
#endif   //  #ifdef USEncurses

// This has now been declared extern in gbtree.h, but is defined here
//   in function run_test_set() before being used in gbtree.cc
int16_t inf_ht, inf_wid;
md5dgstArrayType mdtmp;

int run_test_set( ifstream& f2proc, int start_str_num )
{
    gbst_interface_type gbst_iface;
    const int siz_index_list = ptr_tbl_max_rcrd - 1;
    int index_list[ siz_index_list ];
    int line_no = 0;

    if ( pflg.show_data_record_sizes )
    {
        // Test to show difference between instantiating the ostringstream
        //   before and after setting up the locale.  The locale specific
        //   side effects don't appear in streams set up before the locale
        //   is specified.
        //          1         2         3         4         5         6
        // 1234567890123456789012345678901234567890123456789012345678901234
        while ( !heap_mn.is_new_info() ) heap_mn.get_info();
        ostringstream idisp;
        heap_mn.disp_info( idisp );
        cout << idisp.str() << endl;
        int a = 4;
        string fspt_text =
          "The next available element in the fo_string_ptr array is ";
        void *bv = static_cast<void *>( &fspt_text );
        string tstr = "abcdef";
        tstr.reserve( 15 );
        void *cv = static_cast<void *>( &tstr );
        uint64_t *b = static_cast<uint64_t *>( bv );
        uint64_t *c = static_cast<uint64_t *>( cv );
        ostringstream drsiz;
        drsiz.imbue(std::locale("C"));
        cout << "Made string stream drsiz imbued with the C locale" << endl;

        union tu
        {
            uint16_t tflgtst;
            flag_set tflag;
        } u;
        u.tflag = pflg;
        u.tflgtst = 0x0001;
        for ( int i = 0; i < 16; i++ )
        {
            // tflag = reinterpret_cast<flag_set>( tflgtst )
            drsiz << "For flag test = 0x" << hex << setw(4) <<
              setfill( '0' ) << u.tflgtst << setfill( ' ' ) << dec;
            if ( u.tflag.track_base_search_vars )
              drsiz << ", track_base_search_vars is set";
            if ( u.tflag.change_start_str_num )
              drsiz << ", change_start_str_num is set";
            if ( u.tflag.temp1bit > 0 )
              drsiz << ", temp1bit = " << u.tflag.temp1bit;
            if ( u.tflag.read_strings_from_file )
              drsiz << ", read_strings_from_file is set";
            if ( u.tflag.show_info_output )
              drsiz << ", show_info_output is set";
            if ( u.tflag.play_back_names_read )
              drsiz << ", play_back_names_read is set";
            if ( u.tflag.temp2bit > 0 )
              drsiz << ", temp2bit= " << u.tflag.temp2bit;
            if ( u.tflag.show_data_record_sizes )
              drsiz << ", show_data_record_sizes is set";
            if ( u.tflag.show_name_store_strings )
              drsiz << ", show_name_store_strings is set";
            if ( u.tflag.play_name_store_stream )
              drsiz << ", play_name_store_stream is set";
            if ( u.tflag.show_any )
              drsiz << ", show_any is set";
            drsiz << "." << endl;
            u.tflgtst <<= 1;
        }
        drsiz << "The size of flag_set struct is " << sizeof(flag_set) <<
          " bytes long." << endl;
        cout << "Flag test done and sent to drsiz." << endl;

        mdtmp.fill( 0xa5 );
        mdtmp[ 0 ] = 0x80;
        mdtmp[ 15 ] = 0xff;
        union gu
        {
            uint32_t tflgtst[ 10 ];
            md5_rcrd_type gflag;

            gu() : gflag( mdtmp ) {  };
        } g;
        drsiz << "The values for";
        for ( int i = 0; i < 10; i++ )  drsiz <<
          (i % 4 > 0 ? ", " : ( i != 0 ? ",\n" : "\n" ) ) <<
          "flag test[ " << i << " ] = 0x" << hex << setw(8) <<
          setfill( '0' ) << g.tflgtst[ i ] << setfill( ' ' ) << dec;
        drsiz << "." << endl;

        drsiz << "The size of bool variables is " << sizeof(bool) <<
          " bytes long." << endl;
        drsiz << "The size of str_utf8 variables is " << sizeof(str_utf8) <<
          " bytes long." << endl;
        drsiz << "The size of int variables is " << sizeof(a) <<
          " bytes long." << endl;
#ifdef INdevel
        union {
            uint32_t dbg_arg;
            debug_flags tmpf;
        };
        drsiz << "The size of the debug_flags struct is " <<
          sizeof(debug_flags) << " bytes long." << endl;
        // not needed now -   bool dstrt = false;
        drsiz << "Displaying debug_flag name relative to uint32_t bits.";
        dbg_arg = 0x00000001;
        for ( int idx = 0; idx < 32; idx++ )
        {
            drsiz << ( idx % 4 == 0 ? "\ndbg_arg = 0x" : "; dbg_arg = 0x" ) <<
              hex << setw( 8 ) << setfill( '0' ) << dbg_arg;
            if( tmpf.a0 ) drsiz << ", a0";
            if( tmpf.a1 ) drsiz << ", a1";
            if( tmpf.a2 ) drsiz << ", a2";
            if( tmpf.a3 ) drsiz << ", a3";
            if( tmpf.a4 ) drsiz << ", a4";
            if( tmpf.a5 ) drsiz << ", a5";
            if( tmpf.a6 ) drsiz << ", a6";
            if( tmpf.a7 ) drsiz << ", a7";
            if( tmpf.b0 ) drsiz << ", b0";
            if( tmpf.b1 ) drsiz << ", b1";
            if( tmpf.b2 ) drsiz << ", b2";
            if( tmpf.b3 ) drsiz << ", b3";
            if( tmpf.b4 ) drsiz << ", b4";
            if( tmpf.b5 ) drsiz << ", b5";
            if( tmpf.b6 ) drsiz << ", b6";
            if( tmpf.b7 ) drsiz << ", b7";
            if( tmpf.c0 ) drsiz << ", c0";
            if( tmpf.c1 ) drsiz << ", c1";
            if( tmpf.c2 ) drsiz << ", c2";
            if( tmpf.c3 ) drsiz << ", c3";
            if( tmpf.c4 ) drsiz << ", c4";
            if( tmpf.c5 ) drsiz << ", c5";
            if( tmpf.c6 ) drsiz << ", c6";
            if( tmpf.c7 ) drsiz << ", c7";
            if( tmpf.d0 ) drsiz << ", d0";
            if( tmpf.d1 ) drsiz << ", d1";
            if( tmpf.d2 ) drsiz << ", d2";
            if( tmpf.d3 ) drsiz << ", d3";
            if( tmpf.d4 ) drsiz << ", d4";
            if( tmpf.d5 ) drsiz << ", d5";
            if( tmpf.d6 ) drsiz << ", d6";
            if( tmpf.d7 ) drsiz << ", d7";
            dbg_arg <<= 1;
        }
        drsiz << endl;
        cout << "Debug arg test done and sent to drsiz." << endl;
#endif  //  #ifdef INdevel
        code_point_cmpct tst_ary[ 8 ];
        drsiz << "The size of code_point_cmpct struct is " <<
          sizeof(code_point_cmpct) << " bytes long, 8 element array is " <<
          sizeof(tst_ary) << " bytes long." <<  endl;
        cout << "code_point_cmpct test done and sent to drsiz." << endl;
        utf8_rcrd_type tmp_rcrd;
        cout << "utf8_rcrd_type tmp_rcrd has been defined." << endl;
        drsiz << "The addresses of variables are:  " <<
          "function run_test_set is " << addressof( run_test_set ) <<
          ", instance gbst_iface is " << &gbst_iface <<
        // string_table is private so
        //   ", gbst_iface.string_table is " << &tmp_rcrd.string_table <<
          endl; //  <<  "tmp_rcrd.name_string_rcrds[ 0 ] is " <<
        //   &tmp_rcrd.name_string_rcrds[ 0 ] << endl;
        drsiz << "and:  string fspt_text is " << &fspt_text <<
          ", string tstr is " << &tstr << ", and stream drsiz is " <<
           &drsiz << ".  The contents of the strings are:" << endl;
        drsiz << "pos 0 = " << *b << ", and " << *c << endl;
        for ( a = 1; a < 4; a++ )
        {
            drsiz << "pos " << a << " = " << *(b+a) << " , and " << *(c+a) <<
              endl;
        }
        cout << "Addresses of strings test done and sent to drsiz." << endl;
        drsiz << "The size of the string class is " << sizeof(string) <<
          " and the size of the tstr string variable is " <<
          sizeof(tstr) << " and the size of the fspt_text string is " <<
           sizeof(fspt_text) << "." << endl;
        drsiz << "The size of the str_utf8 struct is " << sizeof(str_utf8) <<
          " and the size of the index_vec vector type is " <<
          sizeof(index_vec) << " and the size of the ivs vector is " <<
           sizeof(ivs) << "." << endl;
        cout << "Tests through str_utf8 size done and sent to drsiz." << endl;
        drsiz << "The ivs vector size = " << ivs.size() << ", capacity = " <<
          ivs.capacity() << ", and available indexes = {";
        bool first_out = true;
        for ( auto vnum : ivs[ 0 ] )
        {
            drsiz << ( first_out ? ( first_out = false, " " ) : ", " ) << vnum;
        }
        cout << "Tests done through ivs vector and sent to drsiz." << endl;
        drsiz << " }." << endl;
        drsiz << "The size of the gbtree class is " << sizeof(gbtree) <<
          " and the size of the utf8_rcrd_type is " <<
          sizeof(utf8_rcrd_type) << endl <<
          "The size of the utf8_name_store class is " << sizeof(utf8_name_store) <<
          " and the size of the gbst_interface_type is " <<
          sizeof(gbst_interface_type) << endl;
        drsiz << "The size of the gu union is " << sizeof(gu) <<
          ", the size of the md5_rcrd_type is " <<
          sizeof(md5_rcrd_type)  <<
          " and the size of the sha1_rcrd_type is " <<
          sizeof(sha1_rcrd_type) << endl;
        // Show utilization of the data structures
        drsiz << "The next free element in the name_string_rcrds vector is " <<
          gbst_iface.nxt_tbl_index << endl <<
          "stored in the gbst_iface.nxt_tbl_index variable." << endl <<
          "The fo_string_ptr array has " << ptr_tbl_max_rcrd <<
          " total records available." << endl; //  <<
        cout << drsiz.str();    // pre-ncurses
        while ( !heap_mn.is_new_info() ) heap_mn.get_info();
        idisp.str( "" );
        heap_mn.disp_info( idisp );
        cout << idisp.str() << endl;
    }
    if ( pflg.test_base_search_vars )
    {

#ifdef INdevel
        test_bsv_debug = dbgf.a1;
        if ( test_bsv_debug )
          dbgs << "Calling gbst_iface.test_btree_bsv()" << endl;
#endif  //  #ifdef INdevel

        gbst_iface.test_btree_bsv();
        return 0;
    }

#ifdef USEncurses
    cout << "Ready to start interactive screen mode." << endl <<
      "    Do you want to continue? [y or n] ";
    string respns;
    cin >> respns;
    if ( respns.find_first_of( "Nn" ) < respns.size() ) return 0;
    ncursio fo_io;
    fo_io.mnloop( 20 );
    fo_io.manage_debug_win();
    fo_io.mnloop( 50 );
    foiorf = &fo_io;
#endif   //  #ifdef USEncurses

    string nm_frm_file;
    int lines_read = 0;

    // Get the column width limit for info text line display
#ifdef USEncurses
    fo_io.get_info_win_size( inf_ht, inf_wid );

#else

    struct winsize term_wind_sizes;
    int ioresp = ioctl( 0, TIOCGWINSZ, &term_wind_sizes );
    if ( ioresp != 0 )
    {
        errs << "The request for the terminal window size returned error." <<
          endl;
    }
    inf_ht = term_wind_sizes.ws_row;
    inf_wid = term_wind_sizes.ws_col;
#endif   //  #ifdef USEncurses

    // When getting lines from an actual file system, a delimiter
    //   character will need to be set reflecting the file system
    //   name string delimiter (for linux ext - '\0')
    while (

#ifdef USEncurses
      !fo_io.quit_cmd_issued &&
#endif   //  #ifdef USEncurses

      getline( f2proc, nm_frm_file ) && line_no < siz_index_list )
    {
        if ( !pflg.change_start_str_num ||
          ( pflg.change_start_str_num && ++lines_read > start_str_num )  )
        // hold -- if ( true )
        {
            //
            // GGG - this line is the one that needs to be changed to enable
            //   the binary tree search and place for the names in the file.
            //
            // Switching to calling search_place_name
            // int stind = gbst_iface.string_table.store_name( nm_frm_file );
            int stind = gbst_iface.search_place_name( nm_frm_file );
            if ( stind > 0 )
            {
                // Check out file gbtree.cc in the do_node_info_update()
                //   function for the code to do
                //      if ( pflg.play_back_names_read )
                index_list[ line_no ] = stind;
                line_no++;
            }
            else errs << "Invalid input line detected" << endl;
        }
    }
    if ( pflg.play_back_names_read && plbck_strg.numsym > 5 )
      // There is at least one name string left that needs to be
      //   displayed for completeness.
      iout << plbck_strg.target << endl;
    bool dup_found = false;
    int last_nondup = 0;
    for ( int idx = 1; idx <line_no; idx++ )
    {
        if ( !( index_list[ idx ] > index_list[ last_nondup ] ) )
        {
            if ( dup_found == false )
            {
                iout << "Duplicate(s) found at: ";
                dup_found = true;
            }
            else iout << ", ";
            iout << index_list[ idx ];
        }
        else last_nondup = idx;
    }

#ifdef USEncurses
    fo_io.manage_info_win();
    // Not needed now  fo_io.manage_graph_win();
    fo_io.manage_debug_win();
    fo_io.manage_main_win();
#endif   //  #ifdef USEncurses

    if ( pflg.play_back_names_read )
    {
        iout << endl;
    }
    if ( pflg.show_gbtree_traverse )
    {
        iout << endl << "Show gbtree traverse for UTF-8, sha-1,"
          " and md5 records" << endl << endl;
        utf8_rcrd_type tmp_rec;
        tmp_rec.traverse_utf8_records();
        sha1_rcrd_type sha_rec;
        sha_rec.traverse_hash_records();
        md5_rcrd_type md5_rec;
        md5_rec.traverse_hash_records();
    }

#ifdef USEncurses
    fo_io.manage_main_win();
#endif   //  #ifdef USEncurses

    //
    // Show some information about the programs data requirements
    //   Sizes of the various records are important to keep minimised.
    if ( pflg.show_data_record_sizes )
    {
        iout << "The size of int variables is " << sizeof(int) <<
        " bytes long." << endl;
        utf8_rcrd_type tmp_rcrd;
        iout << "The size of the gbtree class is " << sizeof(gbtree) <<
          " and the size of the utf8_rcrd_type is " <<
          sizeof(utf8_rcrd_type) << endl <<
          "The size of the utf8_name_store class is " << sizeof(utf8_name_store) <<
          " and the size of the gbst_interface_type is " <<
          sizeof(gbst_interface_type) << endl;  //  <<
        // Show utilization of the data structures
        iout << "The next available element in the fo_string_ptr array is " <<
          gbst_iface.nxt_tbl_index << endl <<
          "stored in the gbst_iface.nxt_tbl_index variable." << endl <<
          "The fo_string_ptr array has " << ptr_tbl_max_rcrd <<
          " total records available." << endl;  //  <<
        iout << "The ivs vector size = " << ivs.size() << ", capacity = " <<
          ivs.capacity() << ", and available indexes = {";
        bool first_out = true;
        for ( auto vnum : ivs[ 0 ] )
        {
            iout << ( first_out ? ( first_out = false, " " ) : ", " ) << vnum;
        }
        iout << " }." << endl;
        iout << "The ivs max sizes = {";
        first_out = true;
        for ( auto vnum : ivs[ 1 ] )
        {
            iout << ( first_out ? ( first_out = false, " " ) : ", " ) << vnum;
        }
        iout << " }." << endl;
        iout << "The sizes and capacities of the index_vec members"
          " of ivs are: { mem_idx, size, capacity }, next" << endl;
        uint16_t mem_num = 0;
        for ( index_vec& ivec : ivs )
        {
            iout << ( mem_num % 8 == 0 ?
              ( mem_num == 0 ? "{ " : " },\n{ " ) : " }, { " ) << mem_num <<
              ", " << ivec.size() << ", " << ivec.capacity();
            mem_num++;
        }
        iout << " }." << endl;
        gout << endl << "Heap info at end of run_test_set() function." << endl;
        while ( !heap_mn.is_new_info() ) heap_mn.get_info();
        ostringstream idisp;
        heap_mn.disp_info( idisp );
        gout << idisp.str() << endl;
    }
    utf8_rcrd_type tmp_rcrd;
    if ( pflg.show_name_store_strings )
    {
        tmp_rcrd.shofspa( "Status of the btree at the end of run:" );
        tmp_rcrd.disp_name_store();
    }
    return 0;
}
