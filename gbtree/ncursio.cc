//
// This provides the code to implement some useful Unicode handling
//   utilities.  So far, two utilities have been included, but more may
//   be coming in the future.
//
//
//
// Use the following command to build this object:
//   g++ -std=c++17 -c ncursio.cc
//   ar -Prs ~/data/lib/libfoutil.a ncursio.o
//   ar -Ptv ~/data/lib/libfoutil.a

#include "ncursio.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include "fo-utils.h"

#define COLOR_DEF_BKGRND 8
#define COLOR_DEF_FRGRND 9
#define COLOR_SAT_BKGRND 10

//
// ncurses standard colors
//  COLOR_BLACK
//  COLOR_RED
//  COLOR_GREEN
//  COLOR_YELLOW
//  COLOR_BLUE
//  COLOR_MAGENTA
//  COLOR_CYAN
//  COLOR_WHITE
//  COLOR_DEF_BKGRND
//  COLOR_DEF_FRGRND
//  COLOR_SAT_BKGRND

#ifdef USEncurses
string nc_colors[] {
    "COLOR_BLACK",
    "COLOR_RED",
    "COLOR_GREEN",
    "COLOR_YELLOW",
    "COLOR_BLUE",
    "COLOR_MAGENTA",
    "COLOR_CYAN",
    "COLOR_WHITE",
    "COLOR_DEF_BKGRND",
    "COLOR_DEF_FRGRND",
    "COLOR_SAT_BKGRND"
};

short nc_col_nums[] {
    COLOR_BLACK,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_YELLOW,
    COLOR_BLUE,
    COLOR_MAGENTA,
    COLOR_CYAN,
    COLOR_WHITE,
    COLOR_DEF_BKGRND,
    COLOR_DEF_FRGRND,
    COLOR_SAT_BKGRND
};

// Line box drawing components

//   ACS_ULCORNER
//   ACS_LLCORNER
//   ACS_URCORNER
//   ACS_LRCORNER
//   ACS_RTEE
//   ACS_LTEE
//   ACS_BTEE
//   ACS_TTEE
//   ACS_HLINE
//   ACS_VLINE
//   ACS_PLUS

//     name  graph   cod pt  widx descriptive name
string lhz = "─"; // U+2500    4  light horizontal
string hhz = "━"; // U+2501   10  heavy horizontal
string lvt = "│"; // U+2502    1  light vertical
string hvt = "┃"; // U+2503    7  heavy vertical

string tlc = "┌"; // U+250c    2  top left corner
string trc = "┐"; // U+2510       top right corner
string blc = "└"; // U+2514    3  bottom left corner
string brc = "┘"; // U+2518       bottom right corner

string htlc = "┏"; // U+250f   8  heavy top left corner
string htrc = "┓"; // U+2513      heavy top right corner
string hblc = "┗"; // U+2517   9  heavy bottom left corner
string hbrc = "┛"; // U+251b      heavy bottom right corner

string tpr = "├"; // U+251c       tee pointing right
string tpl = "┤"; // U+2524       tee pointing left
string tpd = "┬"; // U+252c       tee pointing down
string tpu = "┴"; // U+2534       tee pointing up

string htpr = "┣"; // U+2523      heavy tee pointing right
string htpl = "┫"; // U+252b      heavy tee pointing left
string htpd = "┳"; // U+2533      heavy tee pointing down
string htpu = "┻"; // U+253b      heavy tee pointing up

string hvlr = "┠"; // U+2523      heavy vertical light right
string hvll = "┨"; // U+252b      heavy vertical light left

string thd = "┰"; // U+2530       tee heavy down
string thu = "┸"; // U+2538       tee heavy up
string htld = "┯"; // U+252f      heavy tee light down
string htlu = "┷"; // U+2537      heavy tee light up

string thur = "┡"; // U+2521      tee heavy up and right
string thul = "┩"; // U+2529      tee heavy up and left
string thdr = "┢"; // U+2522      tee heavy down and right
string thdl = "┪"; // U+252a      tee heavy down and left

string tdhl = "┭"; // U+252d      tee down heavy left
string tdhr = "┮"; // U+252e      tee down heavy right
string tuhl = "┵"; // U+2535      tee up heavy left
string tuhr = "┶"; // U+2536      tee up heavy right

string tdll = "┲"; // U+2532      tee down light left
string tdlr = "┱"; // U+2531      tee down light right
string tull = "┺"; // U+253a      tee up light left
string tulr = "┹"; // U+2539      tee up light right

string cxl = "┼"; // U+253c    0  cross light

string chl = "┽"; // U+253d       cross heavy left
string chr = "┾"; // U+253e       cross heavy right
string chh = "┿"; // U+253f       cross heavy horizontal
string chu = "╀"; // U+2540       cross heavy up
string chd = "╁"; // U+2541       cross heavy down
string chv = "╂"; // U+2542       cross heavy vertical

string chul = "╃"; // U+2543  12  cross heavy up and left
string chur = "╄"; // U+2544   5  cross heavy up and right
string chdl = "╅"; // U+2545  13  cross heavy down and left
string chdr = "╆"; // U+2546   6  cross heavy down and right

string hcld = "╇"; // U+2547      heavy cross light down
string hclu = "╈"; // U+2548      heavy cross light up
string hclr = "╉"; // U+2549      heavy cross light right
string hcll = "╊"; // U+254a      heavy cross light left

string cxh = "╋"; // U+254b   11  cross heavy

//
// Characters in the wide character array wgr_need[] = " ┼│┌└─╄╆┃┏┗━╋╃╅ "
//   index Char_name
//     0   ASCII space
//     1   light cross - represents node at the current level
//     2   light vertical bar - shows the path to the next level element
//     3   top left corner - shows the end of the path to the left child node
//     4   bottom left corner - end of path to the right child node
//     5   light horizontal bar - path continuity to node information
//     6   cross heavy up and right - shows decorated path to current node
//     7   cross heavy down and right - shows decorated path to current node
//     8   heavy vertical bar - shows decorated path to current node
//     9   heavy top left corner - shows decorated path to current node
//    10   heavy bottom left corner - shows decorated path to current node
//    11   heavy horizontal bar - shows decorated path to current node
//    12   heavy cross - shows decorated path to current node
//    13   cross heavy up and left - shows decorated path to current node
//    14   cross heavy down and left - shows decorated path to current node
//    15   spare element is another space char
//

void atexit_handl_9()
{
    cout << "at exit handler number 9." << endl;
    cout << "Heap info at handler number 9." << endl;
    while ( !heap_mn.is_new_info() ) heap_mn.get_info();
    ostringstream idisp;
    heap_mn.disp_info( idisp );
    cout << idisp.str() << endl;
}

const int16_t main_nlines = 5;
// The main announce line
const int16_t mal = 3;
// The main prompt line
const int16_t mpl = 4;
// The default maximum wait time for input in milliseconds
const int16_t defwait = 50;

struct no_group : numpunct<char>
{
    std::string do_grouping() const { return ""; } // groups of 1 digit
};

ncursio::ncursio()
{
    btgr.imbue( locale( btgr.getloc(), new no_group ) );

    const int result_9 = atexit( atexit_handl_9 );
    if ( result_9 != 0 ) cout << "atexit reg hdlr 9 fail." << endl;
    // NCURSES_NO_UTF8_ACS = 1;
    initscr();                   // Start curses mode
    noecho();
    inp_tm_out_ms = defwait;
    // Line buffering disabled, pass on everything to
    cbreak();              //  me, but wait a limited
    timeout( defwait );    //  time for input
    start_color();         // Start color
    use_default_colors();
    short def_fgd, def_bkg, fgr, fgg, fgb, bkr, bkg, bkb;
    init_color( COLOR_CYAN, 0, 500, 500 );
    init_color( COLOR_GREEN, 82, 580, 82 );
    init_color( COLOR_WHITE, 888, 888, 888 );
    init_color( COLOR_DEF_BKGRND, 477, 653, 754 );
    init_color( COLOR_SAT_BKGRND, 383, 618, 754 );
    init_color( COLOR_DEF_FRGRND, 250, 110, 196 );
    init_pair(  1, COLOR_RED, -1 );
    init_pair(  2, COLOR_GREEN, -1 );
    init_pair(  3, COLOR_YELLOW, -1 );
    init_pair(  4, COLOR_BLUE, -1 );
    init_pair(  5, COLOR_MAGENTA, -1 );
    init_pair(  6, COLOR_CYAN, -1 );
    init_pair(  7, COLOR_WHITE, -1 );   //   COLOR_SAT_BKGRND );
    init_pair(  8, COLOR_RED, COLOR_BLACK );
    init_pair(  9, COLOR_GREEN, COLOR_BLACK );
    init_pair( 10, COLOR_YELLOW, COLOR_BLACK );
    init_pair( 11, COLOR_MAGENTA, COLOR_BLACK );
    init_pair( 12, COLOR_CYAN, COLOR_BLACK );
    init_pair( 13, COLOR_WHITE, COLOR_BLACK );
    init_pair( 14, COLOR_WHITE, COLOR_RED );
    init_pair( 15, COLOR_WHITE, COLOR_BLUE );
    init_pair( 16, COLOR_WHITE, COLOR_MAGENTA );
    init_pair( 17, COLOR_WHITE, COLOR_YELLOW );
    init_pair( 18, -1, -1 );
    init_pair( 19, COLOR_BLUE, COLOR_MAGENTA );
    init_pair( 20, COLOR_BLACK, COLOR_RED );
    init_pair( 21, COLOR_BLUE, COLOR_RED );
    init_pair( 22, COLOR_GREEN, COLOR_RED );
    init_pair( 23, COLOR_BLUE, COLOR_WHITE );
    init_pair( 24, COLOR_RED, COLOR_BLUE );
    init_pair( 25, COLOR_MAGENTA, COLOR_BLUE );
    init_pair( 26, COLOR_CYAN, COLOR_BLUE );
    init_pair( 27, COLOR_RED, COLOR_GREEN );
    init_pair( 28, COLOR_BLUE, COLOR_GREEN );
    init_pair( 29, COLOR_BLACK, COLOR_GREEN );
    init_pair( 30, COLOR_MAGENTA, COLOR_GREEN );
    init_pair( 31, COLOR_DEF_FRGRND, COLOR_DEF_BKGRND );
    pair_content( 31, &def_fgd, &def_bkg );
    color_content( def_fgd, &fgr, &fgg, &fgb );
    color_content( def_bkg, &bkr, &bkg, &bkb );
    // init_pair( 7, COLOR_CYAN, COLOR_BLACK );
    //

    //   foreground is rgb 401b31 hsv:324,58,25   curs_rgb:250,108,194
    //   background is rgb 7aa7c1 hsv:202,37,76   curs_rgb:477,656,757
    //         blue is rgb 0000ae hsv:240,100,68  curs_rgb:000,000,680
    //      magenta is rgb ae00ae hsv:300,100,68  curs_rgb:680,000,680
    //        green is rgb 00ae00 hsv:120,100,68  curs_rgb:000,680,000
    //      dkgreen is rgb 009400 hsv:120,100,58  curs_rgb:000,580,000
    //      mdgreen is rgb 159415 hsv:120,86,58   curs_rgb:082,580,082
    //        white is rgb aeaeae hsv:202,00,68   curs_rgb:680,680,680
    //     brtwhite is rgb e3e3e3 hsv:202,00,89   curs_rgb:888,888,888
    //         cyan is rgb 00aeae hsv:180,100,68  curs_rgb:680,000,680
    //       dkcyan is rgb 008080 hsv:180,100,50  curs_rgb:500,000,500
    //
    // For more colors see the file /usr/share/vim/vim82/rgb.txt
    //

    keypad( stdscr, TRUE );

    graph_ht = LINES * 2 / 3;
    graph_wid = COLS;
    gr_pad_ht = 256;
    gr_pad_wid = 200;
    // Starting y and x screen locations for the graph display
    grds_sty = 3;
    grds_stx = 0;
    // and ending y and x locations
    grds_eny = graph_ht - 1;
    grds_enx = graph_wid - 1;
    // Height of the graph display on the screen
    grds_ht = grds_eny - grds_sty + 1;
    // Main window info
    mn_wid = COLS * 2 / 5;
    mn_ht = main_nlines;
    mn_strty = graph_ht + 1;
    mn_strtx = 0;
    mn_tmout_row = 1;
    mn_tmout_col = mn_wid - 12;
    // Debug screen info for the debug pad window
    dbg_strty = graph_ht + mn_ht + 2;
    dbg_strtx = 0;
    dbg_ht = LINES - 2 - graph_ht - mn_ht;
    dbg_wid = mn_wid;
    dbg_endy = dbg_strty + dbg_ht - 1;
    dbg_endx = dbg_strtx + dbg_wid - 1;
    dbg_pad_ht = 1000;
    dbg_pad_wid = dbg_wid + 1;
    dbg_p_mnrow = 0;
    // Info window screen coordinates for displaying application output info
    inf_strty = graph_ht + 3;
    inf_strtx = mn_wid + 1;
    info_ht = LINES - inf_strty;
    info_wid = COLS - 1 - mn_wid;
    hpmn_ht = 12;
    hpmnd_wid = 37;
    hpmnv_wid = v_wid * 4 + 1;
    hpmnd_sty = graph_ht - hpmn_ht;
    hpmnd_stx = graph_wid - hpmnd_wid - hpmnv_wid - 1;
    hpmnv_sty = graph_ht - hpmn_ht;
    hpmnv_stx = graph_wid - hpmnv_wid - 1;
    sstrm_ht = 9;
    sstrmd_wid = 14;    // string stream sizes
    sstrmv_wid = v_wid * 2 + 1;
    sstrmd_sty = hpmnv_sty - sstrm_ht;
    sstrmd_stx = graph_wid - sstrmd_wid - sstrmv_wid - 1;
    sstrmv_sty = hpmnv_sty - sstrm_ht;
    sstrmv_stx = graph_wid - sstrmv_wid - 1;
    dbg_scroll_win = false;
    dbg_continuous_scroll = false;

    //
    // Experiment to try and get the default terminal background and
    //   foreground colors which apparently are not available through
    //   standard ncurses queries.
    string tltmp = "Try to get the default background color:  ";
    mvaddstr( 1, 2, tltmp.c_str() );
    refresh();
    //
    // Send terminal escape sequence requesting the default background
    //   color and store the response in a string
    cout << "\e]11;?\007" << endl;
    int16_t ret_char;
    string bkgd_clst;
    while ( ( ret_char = getch() ) != 7 )
      if ( ret_char > 6 && ret_char < '{' )
    {
        bkgd_clst.push_back( ret_char );
    }
    //
    // Send terminal escape sequence requesting the default foreground
    //   color and store the response in a string
    cout << "\e]10;?\007" << endl;
    string frgd_clst;
    while ( ( ret_char = getch() ) != 7 )
      if ( ret_char > 6 && ret_char < '{' )
    {
        frgd_clst.push_back( ret_char );
    }
    refresh();
    // Don't need this pause now -  mnloop();
    int rpos = bkgd_clst.find( "rgb:" ) + 4;
    uint32_t bkred = stoul( bkgd_clst.substr( rpos, 4 ), nullptr, 16 );
    uint32_t bkgrn = stoul( bkgd_clst.substr( rpos+5, 4 ), nullptr, 16 );
    uint32_t bkblu = stoul( bkgd_clst.substr( rpos+10, 4 ), nullptr, 16 );
    uint32_t fgred = stoul( frgd_clst.substr( rpos, 4 ), nullptr, 16 );
    uint32_t fggrn = stoul( frgd_clst.substr( rpos+5, 4 ), nullptr, 16 );
    uint32_t fgblu = stoul( frgd_clst.substr( rpos+10, 4 ), nullptr, 16 );
    short bkcolred = static_cast<short>( ( bkred * 1000 + 0x8000 ) / 0x10000 );
    short bkcolgrn = static_cast<short>( ( bkgrn * 1000 + 0x8000 ) / 0x10000 );
    short bkcolblu = static_cast<short>( ( bkblu * 1000 + 0x8000 ) / 0x10000 );
    short fgcolred = static_cast<short>( ( fgred * 1000 + 0x8000 ) / 0x10000 );
    short fgcolgrn = static_cast<short>( ( fggrn * 1000 + 0x8000 ) / 0x10000 );
    short fgcolblu = static_cast<short>( ( fgblu * 1000 + 0x8000 ) / 0x10000 );

    //
    // OK, now set up the rest
    string horiz_bar;
    horiz_bar.reserve(graph_wid * 3 + 2);
    for ( int idx = 0; idx < graph_wid; idx++ )
    {
        horiz_bar += lhz;
    }
    mvaddstr( 0, 0, horiz_bar.c_str() );
    mvaddstr( graph_ht, 0, horiz_bar.c_str() );
    mvaddstr( graph_ht+mn_ht+1, 0, horiz_bar.substr(0,mn_wid*3).c_str() );
    mvaddstr( graph_ht, mn_wid, tpd.c_str() );
    for (int idx = graph_ht+1; idx < LINES; idx++)
      mvaddstr( idx, mn_wid, lvt.c_str() );
    mvaddstr( graph_ht+mn_ht+1, mn_wid, tpl.c_str() );
    // attron( COLOR_PAIR( 7 ) );
    attron( A_REVERSE );
    tltmp = "George welcomes you to the display main window!";
    mvaddstr( graph_ht, ( mn_wid - tltmp.size() ) / 2, tltmp.c_str() );
    tltmp = "This is the btree graph part of the display!";
    mvaddstr( 0, ( graph_wid - tltmp.size() ) / 2, tltmp.c_str() );
    tltmp = "This is the debug area for the display!";
    mvaddstr( graph_ht+mn_ht+1, ( mn_wid - tltmp.size() ) / 2, tltmp.c_str() );
    tltmp = "This is the info area for the display!";
    mvaddstr( graph_ht, mn_wid + ( info_wid-tltmp.size() )/2, tltmp.c_str() );
    // attroff( COLOR_PAIR( 7 ) );
    attroff( A_REVERSE );
    // Get the current info area header
    btgr << "Default background color [r:" << hex << bkred <<
      ",g:" << bkgrn << ",b:" << bkblu << dec << "]:  ";
    mvaddstr( 1, 2, btgr.str().c_str() );
    addstr( bkgd_clst.c_str() );
    btgr.str( "" );
    btgr << "  and default foreground color [r:" << hex << fgred <<
      ",g:" << fggrn << ",b:" << fgblu << dec << "]:  ";
    addstr( btgr.str().c_str() );
    addstr( frgd_clst.c_str() );
    refresh();
    // Don't need this pause now -  mnloop();
#ifdef INFOdisplay
    istringstream info_relay( bsvistrm.str() );
    infostrm_pos = bsvistrm.str().size();
    int16_t infrow = 0;
    int16_t infcol = 0;
    string nxt_line;
    getline( info_relay, nxt_line );
    if ( nxt_line.size() < info_wid )
      infcol = ( info_wid - nxt_line.size() ) / 2;
    else nxt_line = nxt_line.substr( 0, info_wid );
    mvaddstr( graph_ht + 1 + infrow, mn_wid + 1 + infcol, nxt_line.c_str() );
    infrow++;
    infcol = 0;
    getline( info_relay, nxt_line );
    if ( nxt_line.size() < info_wid )
      for ( int idx = nxt_line.size(); idx < info_wid; idx++ )
       nxt_line.push_back( ' ' );
    else nxt_line = nxt_line.substr( 0, info_wid );
    attron( A_UNDERLINE );
    mvaddstr( graph_ht + 1 + infrow, mn_wid + 1 + infcol, nxt_line.c_str() );
    attroff( A_UNDERLINE );
#endif // #ifdef INFOdisplay
    // Set up the pads and windows for the file organizer application
    bt_gr_pad = newpad( gr_pad_ht, gr_pad_wid );
    bt_dbg = newpad( dbg_pad_ht, dbg_pad_wid );
    scrollok(bt_dbg,TRUE);
    bt_main = newwin( mn_ht, mn_wid, mn_strty, mn_strtx );
    bt_info = newwin( info_ht, info_wid, inf_strty, inf_strtx );
    hpmn_desc = newwin( hpmn_ht, hpmnd_wid, hpmnd_sty, hpmnd_stx );
    hpmn_vals = newwin( hpmn_ht, hpmnv_wid, hpmnv_sty, hpmnv_stx );
    sstrm_desc = newwin( sstrm_ht, sstrmd_wid, sstrmd_sty, sstrmd_stx );
    sstrm_vals = newwin( sstrm_ht, sstrmv_wid, sstrmv_sty, sstrmv_stx );
    scrollok(bt_info,TRUE);
    wmove( bt_info, 0, 0 );
    refresh();
    //                              1         2         3         4         5
    //                     12345678901234567890123456789012345678901234567890
    // mvwaddstr(bt_main,0,3,"George welcomes you to the display main window!");
    mvwaddstr( bt_main,0,0,"Action commands: 'p'ause, 'c'ontinue, " );
    waddstr( bt_main,"'f'aster, 's'lower, 'q'uit" );
    mvwaddstr( bt_main,1,0,"Debug window commands: 'd' scroll one window\n" );
    waddstr( bt_main,"   'D' toggle continuous scroll " );
    getyx( bt_main, mn_y_D_prmt, mn_x_D_prmt );
    string mnDprmt = dbg_continuous_scroll ? "off" : "on ";
    mvwaddstr( bt_main, mn_y_D_prmt, mn_x_D_prmt, mnDprmt.c_str() );
    wrefresh( bt_main );

    // chr_in = getch();
    string grph_needed = " ┼│┌└─╄╆┃┏┗━╋╃╅";
    mvwaddstr( bt_gr_pad, 2, 3, grph_needed.c_str() );
    numwgr = mvwinnwstr( bt_gr_pad, 2, 3, wgr_need, 16 );

    //
    // GGG - These tests no longer make sense since the bt_dbg window is now
    //   a pad that is 1 column wider than the display space so that when
    //   the display reaches the end of the line, and the cursor goes to the
    //   next character, it doesn't force a scroll to the next line.  So now
    //   all output to the debug screen is planned to avoid writing to that
    //   last character of the pad.  Comment everything except the string
    //   declaration, as it is needed later.
    string tstrng;
    // tstrng = "I wonder what happens| - cut vert bars and text between
    //   | if the string written to this debug";
    // tstrng += " area for the display| - cut vert bars and text between
    //   | overflows the right edge of the window!";
    // mvwaddstr(bt_dbg,3,3, tstrng.c_str() );
    // dbg_refresh();
    // //   prefresh( bt_dbg, 0, 0, dbg_strty, dbg_strtx, dbg_endy, dbg_endx );
    // //   chr_in = getch();
    // tstrng = "\nWhat if a UTF-8| - cut vert bars and text between
    //   | character like π is a part of the string?  Will";
    // tstrng += " it still break at the right edge of the window?";
    // waddstr(bt_dbg, tstrng.c_str() );
    // dbg_refresh();
    // tstrng = "  What about scrolling of the Window?  Will it scroll or";
    // tstrng += " just stop at the right edge of the window?";
    // waddstr(bt_dbg, tstrng.c_str() );
    // dbg_refresh();
    //
    // Don't need this pause now -  mnloop();

    // Get experiment file
    ifstream gr_lines;
    gr_lines.open( "temp/btree-diagram-short.lines" );
    //
    // The string clear() method doesn't do what I expected, so it is
    //   useless here
    //     tstrng.clear();
    tstrng = "";
    tstrng.reserve( 800 );
    gr_lines.read( &tstrng[ 0 ], 783 );
    mvwaddstr(bt_gr_pad, 3, 0, tstrng.c_str() );
    gr_lines.close();
    prefresh( bt_gr_pad, 0, 0, grds_sty, grds_stx, grds_eny, grds_enx );
    chr_in = getch();
    int16_t ty, tx;
    getyx( bt_gr_pad, ty, tx );
    wchar_t wch[ 20 ];
    int16_t numch = mvwinnwstr( bt_gr_pad, 22, 0, wch, 16 );
    tstrng = "";
    if ( numch < 20 )
    {
        int idx;
        //   getcchar( wcval, wch, &attrs_val, &color_pair_val, opts );
        tstrng = "mvwinnwstr returned ";
        tstrng += to_string( numch );
        tstrng += " chars.  wch size = ";
        tstrng += to_string( sizeof( wch[ 0 ] ) );
        tstrng += " bytes.  Win row 12, col 1 = \n ";
        btgr.str( "" );
        //   btgr << locale( btgr.getloc() ).name().c_str() << ", ";
        prefresh( bt_gr_pad, 0, 0, grds_sty, grds_stx, grds_eny, grds_enx );
        btgr << showbase;
        for ( idx = 0; idx < numch; idx++ )
        {
            btgr << hex << setw(4) << static_cast<uint32_t>( wch[ idx ] ) <<
              dec << ( idx == numch-1 ? ".  " : ", " );
            //  tstrng += to_string( static_cast<int>( wch[ idx ] ) );
            //  tstrng += ( idx == numch-1 ? ".  " : ", " );
        }
        tstrng += btgr.str();
        btgr.clear();
        int16_t numax = numwgr > numch ? numwgr : numch;
        for ( idx = 0; idx < numax && wch[ idx ] == wgr_need[ idx ];
          idx++ ) ;
        if ( idx < numax )
        {
            tstrng += "\nWide arrays miscompare at index ";
            tstrng += to_string( idx );
            tstrng += ".  wch = ";
        }
        else
        {
            tstrng += "\nWide arrays compare OK.  wch = ";
        }
    }
    else
    {
        tstrng = "The mvwinnwstr function returned ";
        tstrng += to_string( numch );
        tstrng += " for the number of characters.  ";
    }
    mvwaddstr( bt_gr_pad, ty, tx, tstrng.c_str() );
    //   for ( int idx = 0; idx < 5; idx++ )
    //   {
    //       wadd_wch( bt_gr_pad, &wcval[ idx ] );
    //   }
    waddwstr( bt_gr_pad, wch );
    prefresh( bt_gr_pad, 0, 0, grds_sty, grds_stx, grds_eny, grds_enx );
    chr_in = getch();
    waddstr( bt_gr_pad, ", and wgr_need = " );
    waddwstr( bt_gr_pad, wgr_need );
    waddstr( bt_gr_pad, ".  " );
    prefresh( bt_gr_pad, 0, 0, grds_sty, grds_stx, grds_eny, grds_enx );
    scrollok(bt_gr_pad,TRUE);
    // Check out color capabilities
    // bkcolred
    // bkcolgrn
    // bkcolblu
    // fgcolred
    // fgcolgrn
    // fgcolblu

    waddstr( bt_gr_pad, "\n    Check out color capabilities: \n" );
    waddstr( bt_gr_pad, "\n    The standard colors are: \n" );
    btgr.str( "" );
    btgr << "Terminal can support " << COLORS << " colors maximum and " <<
      COLOR_PAIRS << " color pairs maximum." << endl;
    btgr << "The def_bkg color number is " << def_bkg <<
      " with components r = " << bkr << ", g = " <<
      bkg << ", and b = " << bkb << " and found values of r = " <<
      bkcolred << ", g = " << bkcolgrn << ", b = " << bkcolblu << endl;
    btgr << "The def_fgd color number is " << def_fgd <<
      " with components r = " << fgr << ", g = " <<
      fgg << ", and b = " << fgb << " and found values of r = " <<
      fgcolred << ", g = " << fgcolgrn << ", b = " << fgcolblu << endl;
    wattron( bt_gr_pad, COLOR_PAIR( 31 ) );
    waddstr( bt_gr_pad, btgr.str().c_str() );
    wattroff( bt_gr_pad, COLOR_PAIR( 31 ) );
    for ( int idx = 0; idx < 11; idx++ )
    {
        short cp = idx < 8 ? idx : 31;
        short r, g, b;
        color_content( nc_col_nums[ idx ], &r, &g, &b );
        wattron( bt_gr_pad, COLOR_PAIR( cp ) );
        btgr.str( "" );
        btgr << "The color number " << nc_col_nums[ idx ] << " is " <<
          nc_colors[ idx ] << " with components r = " << r << ", g = " <<
          g << ", and b = " << b << endl;
        waddstr( bt_gr_pad, btgr.str().c_str() );
        wattroff( bt_gr_pad, COLOR_PAIR( cp ) );
    }
    for ( int idx = 1; idx <= 30; idx++ )
    {
        wattron( bt_gr_pad, COLOR_PAIR( idx ) );
        btgr.str( "" );
        btgr << "Color pair # is " << setw( 2 ) << idx;
        waddstr( bt_gr_pad, btgr.str().c_str() );
        wattron( bt_gr_pad, A_REVERSE );
        waddstr( bt_gr_pad, " and Reversed" );
        wattroff( bt_gr_pad, A_REVERSE );
        // prefresh( bt_gr_pad, 0, 0, grds_sty, grds_stx, grds_eny, grds_enx );
        wattroff( bt_gr_pad, COLOR_PAIR( idx ) );
        if ( idx % 5 == 0 ) waddstr( bt_gr_pad, "\n" );
        else waddstr( bt_gr_pad, ", " );
    }
    prefresh( bt_gr_pad, 0, 0, grds_sty, grds_stx, grds_eny, grds_enx );
    //
    // Output the heap monitoring description display
    ostringstream hpdesc;
    heap_mn.disp_desc( hpdesc );
    mvwaddstr( hpmn_desc, 1, 0, hpdesc.str().c_str() );
    wrefresh( hpmn_desc );
    // GGG - Output new heap monitor info to curses screen
    ostringstream hpvals;
    heap_mn.disp_values( hpvals );
    mvwaddstr( hpmn_vals, 0, 0, hpvals.str().c_str() );
    wrefresh( hpmn_vals );
    // Output the String stream sizes description display
    mvwaddstr( sstrm_desc, 0, 0, "String stream\n    sizes\n  infstrm\n  "
      "grfstrm\n  dbstrm\n  erstrm\n  bsvistrm\n  Total bytes" );
    wrefresh( sstrm_desc );
    // GGG - Output current String stream sizes info to curses screen
    hpvals.str( "" );
    hpvals <<
      setw( v_wid ) <<  infstrm.str().size() <<
      setw( v_wid ) <<  infstrm.str().capacity() << endl <<
      setw( v_wid ) <<  grfstrm.str().size() <<
      setw( v_wid ) <<  grfstrm.str().capacity() << endl <<
      setw( v_wid ) <<   dbstrm.str().size() <<
      setw( v_wid ) <<   dbstrm.str().capacity() << endl <<
      setw( v_wid ) <<   erstrm.str().size() <<
      setw( v_wid ) <<   erstrm.str().capacity() << endl <<
      setw( v_wid ) << bsvistrm.str().size() <<
      setw( v_wid ) << bsvistrm.str().capacity() << endl <<
      setw( v_wid ) << infstrm.str().size() + grfstrm.str().size() +
      dbstrm.str().size() + erstrm.str().size() + bsvistrm.str().size() <<
      setw( v_wid ) << infstrm.str().capacity() + grfstrm.str().capacity() +
      dbstrm.str().capacity() + erstrm.str().capacity() +
      bsvistrm.str().capacity();
    mvwaddstr( sstrm_vals, 2, 0, hpvals.str().c_str() );
    mvwaddstr( sstrm_vals, 1, 0, "          Sizes       Capacities" );
    wrefresh( sstrm_vals );
    gr_pd_mnrw = 0;
    gr_pd_mncl = 0;
    set_arg_val = 0;
    in_manage_debug_win = false;
    in_manage_graph_win = false;
    in_manage_info_win = false;
    keyleft_pres = 0;
    keyright_pres = 0;
    keyup_pres = 0;
    keydown_pres = 0;
    num_key_spaces = 1;
    keyresize_seen = false;
    quit_cmd_issued = false;
    show_timeout();
}

ncursio::~ncursio()
{
    struct dummy {
        void sh_heap()
        {
            while ( !heap_mn.is_new_info() ) heap_mn.get_info();
            ostringstream idisp;
            heap_mn.disp_info( idisp );
            gout << idisp.str() << endl;
        }
    } d;
    if ( isendwin() == false )
      endwin();               /* End curses mode        */
    gout << endl << "Running the ncursio destructor." << endl;
    d.sh_heap();
    gout << endl << "Running delwin( bt_gr_pad )." << endl;
    delwin( bt_gr_pad );
    d.sh_heap();
    gout << endl << "Running delwin( bt_dbg )." << endl;
    delwin( bt_dbg );
    d.sh_heap();
    gout << endl << "Running delwin( bt_main )." << endl;
    delwin( bt_main );
    d.sh_heap();
    gout << endl << "Running delwin( bt_info )." << endl;
    delwin( bt_info );
    d.sh_heap();
    gout << endl << "Running delwin( hpmn_desc )." << endl;
    delwin( hpmn_desc );
    d.sh_heap();
    gout << endl << "Running delwin( hpmn_vals )." << endl;
    delwin( hpmn_vals );
    d.sh_heap();
    gout << endl << "Running delwin( sstrm_desc )." << endl;
    delwin( sstrm_desc );
    d.sh_heap();
    gout << endl << "Running delwin( sstrm_vals )." << endl;
    delwin( sstrm_vals );
    d.sh_heap();
    gout << endl << "Running delwin( curscr )." << endl;
    delwin( curscr );
    d.sh_heap();
    foiorf = nullptr;
}

int16_t ncursio::mnloop( int16_t n_iterations )
{
    bool run_loop = true;
    int16_t iter_left = n_iterations;
    while( run_loop && ( chr_in = getch() ) != KEY_F(1) )
    {
        if ( chr_in >= '0' && chr_in <= '9' )
        {
            // Set up argument value for future command
            set_arg_val = set_arg_val * 10 + ( chr_in - '0' );
        }
        bool cmd_handled = true;
        switch(chr_in)
        {
            case 'c':
                run_loop = false;
                break;
            case 'd':
                dbg_scroll_win = true;
                if ( in_manage_debug_win )
                  run_loop = false;
                break;
            case 'D':
              {
                dbg_continuous_scroll = !dbg_continuous_scroll;
                string mnDprmt = dbg_continuous_scroll ? "off" : "on ";
                mvwaddstr( bt_main, mn_y_D_prmt, mn_x_D_prmt, mnDprmt.c_str() );
                wrefresh( bt_main );
                if ( in_manage_debug_win && dbg_continuous_scroll )
                  run_loop = false;
              }
                break;
            case 'f':
                if ( inp_tm_out_ms > 1 )
                {
                    inp_tm_out_ms = inp_tm_out_ms - ( inp_tm_out_ms / 10 );
                    timeout( inp_tm_out_ms );
                    show_timeout();
                }
                break;
            case 'g':
                in_manage_graph_win = false;
                run_loop = false;
                break;
            case 'n':
                cbreak();
                inp_tm_out_ms = set_arg_val;
                timeout( inp_tm_out_ms );
                show_timeout();
                break;
            case 'p':
                n_iterations = 0;
                break;
            case 'q':
                // Temporary quit when F1 doesn't work
                run_loop = false;
                in_manage_debug_win = false;
                in_manage_graph_win = false;
                in_manage_info_win = false;
                quit_cmd_issued = true;
                endwin();
                break;
            case 's':
                inp_tm_out_ms = inp_tm_out_ms < 3 ? 3 : inp_tm_out_ms * 2;
                timeout( inp_tm_out_ms );
                show_timeout();
                break;
            case 'z':
                run_loop = false;
                break;
            case KEY_LEFT:
                if ( set_arg_val > 0 ) num_key_spaces = set_arg_val;
                keyleft_pres = num_key_spaces;
                break;
            case KEY_RIGHT:
                if ( set_arg_val > 0 ) num_key_spaces = set_arg_val;
                keyright_pres = num_key_spaces;
                break;
            case KEY_UP:
                if ( set_arg_val > 0 ) num_key_spaces = set_arg_val;
                keyup_pres = num_key_spaces;
                break;
            case KEY_DOWN:
                if ( set_arg_val > 0 ) num_key_spaces = set_arg_val;
                keydown_pres = num_key_spaces;
                break;
            case KEY_RESIZE:
                keyresize_seen = true;
                break;
            default:
                cmd_handled = false;
        }
        if ( cmd_handled ) set_arg_val = 0;
        if ( n_iterations > 0 )
        {
            if ( --iter_left == 0 ) run_loop = false;
        }
        heap_mn.get_info();
    }
    return chr_in;
}

int16_t ncursio::show_timeout()
{
    string tmout_str = "timout: ";
    tmout_str += to_string( inp_tm_out_ms );
    int16_t ts;
    if ( ( ts = tmout_str.size() ) < 12 )
      tmout_str.insert( 0, 12 - ts, ' ' );
    mvwaddstr( bt_main, mn_tmout_row, mn_tmout_col, tmout_str.c_str() );
    wrefresh( bt_main );
    mnloop( 1 );
    return 0;
}

int16_t ncursio::manage_main_win()
{
    mvwaddstr( bt_main, mal, 0, "All name strings have been read. Exiting:  " );
    wrefresh( bt_main );
    mnloop();
    return 0;
}

int16_t ncursio::manage_info_win()
{
    in_manage_info_win = true;
    mvwaddstr( bt_main, mal, 0, "In manage info window:                     " );
    wrefresh( bt_main );
    mnloop( 1 );
    //  wmove( bt_info, 0, 0 );
#ifdef INFOdisplay
    istringstream info_relay( bsvistrm.str().substr( infostrm_pos ) );
    infostrm_pos = bsvistrm.str().size();
    for ( string nxt_line; getline( info_relay, nxt_line ); )
    {
        // Read lines from the base search variable info stream and display
        //   them in the info window after trimming any characters that
        //   exceed the number of columns allocated to the info window.
        int16_t infy __attribute__(( unused )), infx;
        getyx( bt_info, infy, infx );
        if ( infx > 5 ) waddstr( bt_info, "\n" );
        str_utf8 ilin( nxt_line, info_wid, fit_trim_end ); // .. fit_ends );
        waddstr( bt_info, ilin.target.c_str() );
        wrefresh( bt_info );
        mnloop( 1 );
    }
#endif // #ifdef INFOdisplay
    mvwaddstr( bt_main, mal, 0, "                                           " );
    wrefresh( bt_main );
    mnloop( 1 );
    in_manage_info_win = false;
    return 0;
}

void ncursio::get_info_win_size( int16_t &ht, int16_t &wid )
{
    ht = info_ht;
    wid = info_wid;
}

int16_t ncursio::manage_graph_win()
{
    // Manage the implementation of the graph pad window and its display
    //   in the reserved btree graph area of the screen.
    // Temporarily it is being used to show the full debugging lines that
    //   are being displayed in the debug window as line segments that fit
    //   the window.
    in_manage_graph_win = true;
    mvwaddstr( bt_main, mal, 0, "In graph window: 'g' contin, [arrow] scroll" );
    wrefresh( bt_main );
    while ( in_manage_graph_win )
    {
        mnloop( 1 );
        bool key_action = true;
        if ( keyup_pres > 0 )
        {
            gr_pd_mnrw = gr_pd_mnrw > keyup_pres ? gr_pd_mnrw - keyup_pres : 0;
            keyup_pres = 0;
        }
        else if ( keydown_pres > 0 )
        {
            gr_pd_mnrw = gr_pd_mnrw < gr_pad_ht - grds_ht - keydown_pres ?
              gr_pd_mnrw + keydown_pres : gr_pad_ht - grds_ht;
            keydown_pres = 0;
        }
        else if ( keyleft_pres > 0 )
        {
            gr_pd_mncl = gr_pd_mncl > keyleft_pres ?
              gr_pd_mncl - keyleft_pres : 0;
            keyleft_pres = 0;
        }
        else if ( keyright_pres > 0 )
        {
            gr_pd_mncl = gr_pd_mncl < gr_pad_wid - graph_wid - keyright_pres ?
              gr_pd_mncl + keyright_pres : gr_pad_wid - graph_wid;
            keyright_pres = 0;
        }
        else key_action = false;
        if ( key_action )
        {
            prefresh( bt_gr_pad, gr_pd_mnrw, gr_pd_mncl,
              grds_sty, grds_stx, grds_eny, grds_enx );
        }
    }
    //   mvwaddstr( bt_gr_pad, 1, 0, "Temporary display of debug lines:  " );
    //   prefresh( bt_gr_pad, 0, 0, grds_sty, grds_stx, grds_eny, grds_enx );
    mvwaddstr( bt_main, mal, 0, "                                           " );
    wrefresh( bt_main );
    mnloop( 1 );
    return 0;
}

int16_t ncursio::db_wait4scroll()
{
    // Out of scrolling lines, so wait until next set available
    int16_t llft = 0;
    // Unless continuous scroll is true, then just return a set of lines
    if ( dbg_continuous_scroll ) return dbg_ht / 2;
    if ( dbg_scroll_win )
    {
        llft = dbg_ht;
        dbg_scroll_win = false;
        return llft;
    }
    mvwaddstr( bt_main, mpl, 0, "Waiting on debug window:  cmd 'd' or 'D' " );
    wrefresh( bt_main );
    mnloop();
    if ( dbg_scroll_win )
    {
        llft = dbg_ht;
        dbg_scroll_win = false;
    }
    mvwaddstr( bt_main, mpl, 0, "                                         " );
    wrefresh( bt_main );
    return llft;
}

void ncursio::dbg_refresh()
{
    int16_t dby, dbx;
    getyx( bt_dbg, dby, dbx );
    dbg_p_mnrow = dby > dbg_ht - 1 ? dby - dbg_ht + 1 : 0;
    prefresh( bt_dbg, dbg_p_mnrow, 0,
      dbg_strty, dbg_strtx, dbg_endy, dbg_endx );
    mnloop( 1 );
    if ( keyup_pres > 0 )
    {
        bool manual_scroll_dbg_win = true;
        while ( manual_scroll_dbg_win )
        {
            if ( keyup_pres > 0 )
            {
                dbg_p_mnrow = dbg_p_mnrow > keyup_pres ?
                  dbg_p_mnrow - keyup_pres : 0;
                keyup_pres = 0;
            }
            else if ( keydown_pres > 0 )
            {
                dbg_p_mnrow =
                  dbg_p_mnrow < dby - dbg_ht + 1 + keydown_pres ?
                  dbg_p_mnrow + keydown_pres : dby - dbg_ht + 1;
                keydown_pres = 0;
                if ( dbg_p_mnrow > dby - dbg_ht )
                  manual_scroll_dbg_win = false;
            }
            prefresh( bt_dbg, dbg_p_mnrow, 0,
              dbg_strty, dbg_strtx, dbg_endy, dbg_endx );
            mnloop( 1 );
        }
    }
}

int16_t ncursio::manage_debug_win()
{
    // This method manages the transfer of the debug output to the ncurses
    //   debug window by receiving the outputs to the dbstrm ostringstream,
    //   and diverting them to the bt_dbg pad which is then shown in the
    //   debug area of the main screen.  Between each output line, the
    //   mnloop is called for one iteration until the screen is filled.
    //   The output will then be paused until the user types a lower case
    //   'd', which will allow a new window full of output to be displayed,
    //   or until the dbg_continuous_scroll flag is toggled true by the
    //   upper case 'D' command which will allow continuous scroll of the
    //   debug window.  That scroll can be slowed down by the 's'lower
    //   command, or speeded up by the 'f'aster command.  It can also be
    //   'p'aused until the 'c'ontinue command is sent.
    static int dbstrm_pos = 0;
    static int16_t lines_left = 0;
    if ( heap_mn.is_new_info() )
    {
        // GGG - Output new heap monitor info to curses screen
        ostringstream hpvals;
        heap_mn.disp_values( hpvals );
        mvwaddstr( hpmn_vals, 0, 0, hpvals.str().c_str() );
        wrefresh( hpmn_vals );
        // GGG - Output current String stream sizes info to curses screen
        hpvals.str( "" );
        hpvals <<
          setw( v_wid ) <<  infstrm.str().size() <<
          setw( v_wid ) <<  infstrm.str().capacity() << endl <<
          setw( v_wid ) <<  grfstrm.str().size() <<
          setw( v_wid ) <<  grfstrm.str().capacity() << endl <<
          setw( v_wid ) <<   dbstrm.str().size() <<
          setw( v_wid ) <<   dbstrm.str().capacity() << endl <<
          setw( v_wid ) <<   erstrm.str().size() <<
          setw( v_wid ) <<   erstrm.str().capacity() << endl <<
          setw( v_wid ) << bsvistrm.str().size() <<
          setw( v_wid ) << bsvistrm.str().capacity() << endl <<
          setw( v_wid ) << infstrm.str().size() + grfstrm.str().size() +
          dbstrm.str().size() + erstrm.str().size() + bsvistrm.str().size() <<
          setw( v_wid ) << infstrm.str().capacity() + grfstrm.str().capacity() +
          dbstrm.str().capacity() + erstrm.str().capacity() +
          bsvistrm.str().capacity();
        mvwaddstr( sstrm_vals, 2, 0, hpvals.str().c_str() );
        wrefresh( sstrm_vals );
    }
    int new_dbstrm_count = dbstrm.str().size() - dbstrm_pos;
    if ( new_dbstrm_count < 10 ) return new_dbstrm_count;
    int16_t dby __attribute__(( unused )), dbx;
    // Sending debug lines to graph pad when defined
//  #define DBG_TO_GRAPH_PAD
#ifdef DBG_TO_GRAPH_PAD
    int16_t gry, grx;
    waddstr( bt_gr_pad, "\n\nTemporary display of debug lines:  " );
    getyx( bt_gr_pad, gry, grx );
    //
    // Do this differently so that more of the history is saved
    //     werase( bt_gr_pad );
    int16_t grds_dif = grds_ht - 1;
    gr_pd_mnrw = gry > grds_dif ? gry - grds_dif : 0;
    gr_pd_mncl = 0;
    // prefresh( bt_gr_pad, 0, 0, grds_sty, grds_stx, grds_eny, grds_enx );
    // mvwaddstr( bt_gr_pad, 1, 0, "\n\nTemporary display of debug lines:  " );
    prefresh( bt_gr_pad, gr_pd_mnrw, gr_pd_mncl,
      grds_sty, grds_stx, grds_eny, grds_enx );
    manage_graph_win();
#endif  //  #ifdef DBG_TO_GRAPH_PAD
    in_manage_debug_win = true;
    mvwaddstr( bt_main, 4, 0, "In manage debug window:                     " );
    wrefresh( bt_main );
    mnloop( 1 );
    istringstream dbg_relay( dbstrm.str().substr( dbstrm_pos ) );
    if ( dbg_relay.str().size() > 10 && ( dbstrm_pos = dbstrm.str().size() ) )
      for ( string nxt_line; getline( dbg_relay, nxt_line ); )
    {
        // Read and process lines from relay buffer stream
        //
        // Sending debug lines to graph pad when defined. See above
#ifdef DBG_TO_GRAPH_PAD

        waddstr( bt_gr_pad, "\n" );
        waddstr( bt_gr_pad, nxt_line.c_str() );
        getyx( bt_gr_pad, gry, grx );
        gr_pd_mnrw = gry > grds_dif ? gry - grds_dif : 0;
        prefresh( bt_gr_pad, gr_pd_mnrw, 0,
          grds_sty, grds_stx, grds_eny, grds_enx );
#endif  //  #ifdef DBG_TO_GRAPH_PAD
        // See if it will fit into the space remaining on this line plus
        //    the next line
        getyx( bt_dbg, dby, dbx );
        uint16_t lookp = dbg_wid < ( dbx + 3 ) ? 0 : dbg_wid - dbx - 3;
        // If the lookp space is negative or too small to be useful, set to 0
        if ( lookp < dbg_wid / 9 ) lookp = 0;
        int idx;
        //
        // GGG - probably should find the number of columns needed for the
        //   line instead of just arbitrarily using the character size.
        // Also, if the number of columns needed is equal to dbg_wid, the
        //   line would fit, but not going to change it now, so keep in
        //   mind for when I start to use the symbol column info to fix
        //   this section.
        if ( nxt_line.size() < dbg_wid )
        {
            if ( nxt_line.size() < lookp )
            {
                waddstr(bt_dbg, " § " );
                waddstr(bt_dbg, nxt_line.c_str() );
            }
            else
            {
                if ( lines_left < 1 ) lines_left = db_wait4scroll() - 1;
                waddstr( bt_dbg, "\n" );
                waddstr(bt_dbg, nxt_line.c_str() );
                lines_left--;
            }
            dbg_refresh();
        }
        else if ( nxt_line.size() < lookp + dbg_wid )
        {
            // find white space before lookp
            int16_t minidx = nxt_line.size() - dbg_wid;
            idx = find_safe_break( nxt_line, minidx, lookp, false );
            //    for ( idx = lookp; idx > minidx && nxt_line[ idx ] != ' ';
            //      idx-- ) ;
            if ( nxt_line[ idx ] != ' ' )
            {
                // No usable breaking whitespace was found, so just break the
                //   line at the point where it will end at the same column
                //   on both lines which is the average of minidx and lookp.
                //   minidx = ( minidx + lookp ) / 2;
                //
                // Need to make sure we are not breaking up a UTF-8 character
                //   when we make this break.  If it is, we can safely
                //   advance to the end of the character.
                // while ( ( nxt_line[ minidx - 1 ] & 0xc0 ) == 0x80 ) minidx++;
                waddstr(bt_dbg, " § " );
                waddstr(bt_dbg, nxt_line.substr( 0, idx ).c_str() );
                if ( lines_left < 1 ) lines_left = db_wait4scroll() - 1;
                waddstr( bt_dbg, "\n" );
                waddstr(bt_dbg, nxt_line.substr( idx ).c_str() );
                lines_left--;
            }
            else if ( nxt_line[ idx ] == ' ' &&
              nxt_line.size() - idx < dbg_wid )
            {
                // Can put it on this line and next line
                waddstr(bt_dbg, " § " );
                waddstr(bt_dbg, nxt_line.substr( 0, idx ).c_str() );
                if ( lines_left < 1 ) lines_left = db_wait4scroll() - 1;
                waddstr( bt_dbg, "\n" );
                waddstr(bt_dbg, nxt_line.substr( idx + 1 ).c_str() );
                lines_left--;
            }
            else
            {
                // won't fit, so use next two lines
                if ( lines_left < 1 ) lines_left = db_wait4scroll();
                idx = find_safe_break( nxt_line, minidx, dbg_wid, false );
                //  for ( idx = dbg_wid; idx > minidx && nxt_line[ idx ] != ' ';
                //    idx-- ) ;
                // if ( nxt_line[ idx ] != ' ' )
                // {
                //     // No usable breaking whitespace was found, so just break
                //     //   the line at the point where it will end at the same
                //     //   column on both lines which is the average of minidx
                //     //   and dbg_wid.
                //     idx = ( minidx + dbg_wid ) /2;
                //     //
                //     // Need to make sure we are not breaking up a UTF-8
                //     //   character when we make this break.  If it is, we
                //     //   can safely advance to the end of the character.
                //     while ( ( nxt_line[ idx - 1 ] & 0xc0 ) == 0x80 ) idx++;
                // }
                waddstr( bt_dbg, "\n" );
                waddstr(bt_dbg, nxt_line.substr( 0, idx ).c_str() );
                lines_left--;
                dbg_refresh();
                if ( lines_left < 1 ) lines_left = db_wait4scroll() - 1;
                waddstr( bt_dbg, "\n" );
                if ( nxt_line[ idx ] == ' ' ) idx++;
                waddstr(bt_dbg, nxt_line.substr( idx ).c_str() );
                lines_left--;
            }
            dbg_refresh();
        }
        else while ( nxt_line.size() > 0 )
        {
            // This is a long line that will need to be done in parts
            //
            // Use at least 3/5th of each line even if no spaces are found
            //   in the needed segment
            const int16_t minidx = dbg_wid * 3 / 5;
            static int16_t part_numbr = 0;
            if ( lines_left < 1 ) lines_left = db_wait4scroll() - part_numbr;
            waddstr( bt_dbg, "\n" );
            idx = find_safe_break( nxt_line, minidx, dbg_wid, false );
            //   for ( idx = dbg_wid; idx > minidx && nxt_line[ idx ] != ' ';
            //     idx-- ) ;
            //   if ( nxt_line[ idx ] != ' ' )
            //   {
            //       idx = dbg_wid;
            //       //
            //       // Need to make sure we are not breaking up a UTF-8
            //       //   character when we make this break.  If it is, we
            //       //   can safely advance to the end of the character.
            //       while ( ( nxt_line[ idx - 1 ] & 0xc0 ) == 0x80 ) idx++;
            //   }
            //   else nxt_line.erase( idx, 1 );
            if ( nxt_line[ idx ] == ' ' ) nxt_line.erase( idx, 1 );
            waddstr( bt_dbg, nxt_line.substr( 0, idx ).c_str() );
            lines_left--;
            part_numbr++;
            dbg_refresh();
            if ( lines_left < 1 ) lines_left = db_wait4scroll() - part_numbr;
            nxt_line.erase( 0, idx );
            if ( nxt_line.size() < dbg_wid )
            {
                waddstr( bt_dbg, "\n" );
                waddstr( bt_dbg, nxt_line.c_str() );
                lines_left--;
                dbg_refresh();
                nxt_line.clear();
                part_numbr = 0;
            }
        }
    }
    mvwaddstr( bt_main, 4, 0, "                                            " );
    wrefresh( bt_main );
    in_manage_debug_win = false;
    mnloop( 1 );
    return 0;
    // this is for when the debug window is full and not able to scroll
    // mvwaddstr( bt_main,5,0,"Waiting on debug window:  cmd 'd' or 'D' " );
}
#endif // #ifdef USEncurses
