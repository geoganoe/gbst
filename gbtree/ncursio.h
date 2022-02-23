//
// This provides the declaration for a class to be used as the gateway
//   to using ncurses I/O for the file organizer application.
//   Initially, it will be used to finish the btree design and
//   implementation by providing a btree graph display that is
//   supported by the various information needed to solve the current
//   problems. However, the first thing I need to do is decide on the
//   best approach toward getting the displays that I need for a good
//   visualization.

#ifndef NCURS_IO_H
#define NCURS_IO_H

#include <ncurses.h>
//  #include <string>     in common
//  #include <sstream>    in common
#include "fo-common.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>

using namespace std;

#ifdef INdevel
    // Declarations/definitions/code for development only

#endif // #ifdef INdevel

#ifdef USEncurses
class ncursio {
    WINDOW *bt_gr_pad;
    WINDOW *bt_dbg;
    WINDOW *bt_main;
    WINDOW *bt_info;
    ostringstream btgr;
    ostringstream dbg;
    ostringstream btmn;
    ostringstream btinf;
    wchar_t wgr_need[ 20 ];
    int infostrm_pos;
    int16_t numwgr;
    int16_t chr_in;
    int16_t set_arg_val;
    int16_t graph_ht;
    int16_t graph_wid;
    int16_t gr_pad_ht;
    int16_t gr_pad_wid;
    int16_t gr_pd_mnrw;
    int16_t gr_pd_mncl;
    int16_t grds_sty;
    int16_t grds_stx;
    int16_t grds_eny;
    int16_t grds_enx;
    int16_t grds_ht;
    int16_t mn_strty;
    int16_t mn_strtx;
    int16_t mn_wid;
    int16_t mn_ht;
    int16_t mn_y_D_prmt;
    int16_t mn_x_D_prmt;
    int16_t mn_tmout_row;
    int16_t mn_tmout_col;
    int16_t dbg_strty;
    int16_t dbg_strtx;
    int16_t dbg_endy;
    int16_t dbg_endx;
    int16_t dbg_ht;
    uint16_t dbg_wid;
    int16_t dbg_pad_ht;
    int16_t dbg_pad_wid;
    int16_t dbg_p_mnrow;
    int16_t inf_strty;
    int16_t inf_strtx;
    uint16_t info_ht;
    uint16_t info_wid;
    int16_t inp_tm_out_ms;
    int16_t keyleft_pres;
    int16_t keyright_pres;
    int16_t keyup_pres;
    int16_t keydown_pres;
    int16_t num_key_spaces;
    bool keyresize_seen;

    bool dbg_scroll_win;
    bool dbg_continuous_scroll;
    bool in_manage_debug_win;
    bool in_manage_graph_win;
    bool in_manage_info_win;
    int16_t db_wait4scroll();
    void dbg_refresh();

    int16_t show_timeout();

public:
    bool quit_cmd_issued;

    ncursio();
    ~ncursio();
    // Number of iterations with 0 = unlimited
    int16_t mnloop( int16_t n_iterations = 0 );
    int16_t manage_debug_win();
    int16_t manage_graph_win();
    int16_t manage_info_win();
    void get_info_win_size( int16_t &ht, int16_t &wid );
    int16_t manage_main_win();
};

extern ncursio *foiorf;

extern string lhz;
extern string hhz;
extern string lvt;
extern string hvt;

extern string tlc;
extern string trc;
extern string blc;
extern string brc;

extern string htlc;
extern string htrc;
extern string hblc;
extern string hbrc;

extern string tpr;
extern string tpl;
extern string tpd;
extern string tpu;

extern string htpr;
extern string htpl;
extern string htpd;
extern string htpu;

extern string hvlr;
extern string hvll;

extern string thd;
extern string thu;
extern string htld;
extern string htlu;

extern string thur;
extern string thul;
extern string thdr;
extern string thdl;

extern string tdhl;
extern string tdhr;
extern string tuhl;
extern string tuhr;

extern string tdll;
extern string tdlr;
extern string tull;
extern string tulr;

extern string cxl;

extern string chl;
extern string chr;
extern string chh;
extern string chu;
extern string chd;
extern string chv;

extern string chul;
extern string chur;
extern string chdl;
extern string chdr;

extern string hcld;
extern string hclu;
extern string hclr;
extern string hcll;

extern string cxh;
#endif // #ifdef USEncurses

#endif  // NCURS_IO_H