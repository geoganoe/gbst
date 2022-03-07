//
// This include provides the declaration for a class that providing an
//   interface for monitoring heap memory.

#include <malloc.h>
#include <chrono>
#include <ostream>

#ifndef HEAP_MON_CLASS_type_h
#define HEAP_MON_CLASS_type_h

#if __GLIBC_PREREQ(2, 33)
    // This system meets the stated GNU lib C requirements.
#define MA_SZT size_t
#define MA_INT int64_t
#else
    // This system does not meet the GNU lib C requirements, so we must
    //   reconfigure to be compatible.  Since the mallinfo2 struct uses
    //   64 bit variables, and the previous mallinfo struct used just int
    //   variables, it should work with these changes.
#define MA_SZT int
#define MA_INT int
#define mallinfo2 mallinfo
#endif

// This struct has same members as mallinfo2 struct except that the
//   mallinfo2 struct members are size_t instead of int64_t
struct info_rmndr {
    MA_INT arena;     // Normal heap space allocated (bytes)
    MA_INT ordblks;   // Number of free chunks
    MA_INT smblks;    // Number of free fastbin blocks
    MA_INT hblks;     // Number of mmapped regions
    MA_INT hblkhd;    // Space allocd in mmapped reg (bytes)
    MA_INT usmblks;   // See below
    MA_INT fsmblks;   // Space in freed fastbin blks (bytes)
    MA_INT uordblks;  // Total allocated space (bytes)
    MA_INT fordblks;  // Total free space (bytes)
    MA_INT keepcost;  // Top-most, releasable space (bytes)
};

using namespace std;

const int64_t min_usec_intvl = 25000;  // minimum read interval
const int64_t usec_per_minute = 1000000 * 60;  // microseconds per minute
const int64_t usec_per_3sec = 1000000 * 3;  // microseconds per 3 seconds
const uint16_t v_wid = 16;

class heap_mon_class
{
    static struct mallinfo2 mi;
    chrono::steady_clock::time_point cur_tm;
    bool new_info;
    static struct mallinfo2 minimums[ 5 ];
    static struct mallinfo2 maximums[ 5 ];
    static struct mallinfo2 acc3sec;
    static struct mallinfo2 mnut_stps[ 20 ];
    static int step_idx;
    static int tst_idx;
    static uint32_t smpl_cnt;
    static struct mallinfo2 minute_avg;
    static info_rmndr m_avg_remndr;
    static chrono::steady_clock::time_point strt_tm;
    static int64_t last_run_usec;
    static int64_t intvl_usec;
    static int64_t run_usec;
    static int64_t nxt_step;
    static int64_t mnmx_nxt_step;

public:
    heap_mon_class();
    void get_info();
    void disp_desc( ostream& ostrm );
    void disp_values( ostream& ostrm );
    void disp_info( ostream& ostrm );
    bool is_new_info();
};

#endif  //  #ifndef HEAP_MON_CLASS_type_h
