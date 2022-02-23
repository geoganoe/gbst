//
// This include provides the declaration for a class that providing an
//   interface for monitoring heap memory.

#include <malloc.h>
#include <chrono>
#include <ostream>

#ifndef HEAP_MON_CLASS_type_h
#define HEAP_MON_CLASS_type_h

// This struct has same members as mallinfo2 struct except that the
//   mallinfo2 struct members are size_t instead of int64_t
struct info_rmndr {
    int64_t arena;     // Normal heap space allocated (bytes)
    int64_t ordblks;   // Number of free chunks
    int64_t smblks;    // Number of free fastbin blocks
    int64_t hblks;     // Number of mmapped regions
    int64_t hblkhd;    // Space allocd in mmapped reg (bytes)
    int64_t usmblks;   // See below
    int64_t fsmblks;   // Space in freed fastbin blks (bytes)
    int64_t uordblks;  // Total allocated space (bytes)
    int64_t fordblks;  // Total free space (bytes)
    int64_t keepcost;  // Top-most, releasable space (bytes)
};

using namespace std;

const int64_t min_usec_intvl = 25000;  // minimum read interval
const int64_t usec_per_minute = 1000000 * 60;  // microseconds per minute
const int64_t usec_per_3sec = 1000000 * 3;  // microseconds per minute
const uint16_t v_wid = 16;

class heap_mon_class
{
    struct mallinfo2 mi;
    chrono::steady_clock::time_point cur_tm;
    bool new_info;
    static struct mallinfo2 minimums;
    static struct mallinfo2 maximums;
    static struct mallinfo2 acc3sec;
    static struct mallinfo2 min_steps[ 20 ];
    static int step_idx;
    static uint32_t smpl_cnt;
    static struct mallinfo2 minute_avg;
    static info_rmndr m_avg_remndr;
    static chrono::steady_clock::time_point strt_tm;
    static chrono::steady_clock::time_point last_tm;
    static int64_t intvl_usec;
    static int64_t run_usec;
    static int64_t nxt_step;

public:
    heap_mon_class();
    void get_info();
    void disp_desc( ostream& ostrm );
    void disp_values( ostream& ostrm );
    void disp_info( ostream& ostrm );
    bool is_new_info();
};

#endif  //  #ifndef HEAP_MON_CLASS_type_h
