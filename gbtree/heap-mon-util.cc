//
// This module provides a class that will provide an interface for
//   monitoring heap memory.  Initially it is just a window into the
//   information available from the glibc library mallinfo2() function
//   so that I can easily show the usage as the program runs.
//
// Use the following command to build this object:
//   g++ -std=c++17 -c heap-mon-util.cc

// #include <stdlib.h>
// #include <cstring>

#include "heap-mon-util.h"
#include <iomanip>

struct mallinfo2 heap_mon_class::minimums;
struct mallinfo2 heap_mon_class::maximums;
struct mallinfo2 heap_mon_class::acc3sec;
struct mallinfo2 heap_mon_class::min_steps[ 20 ];
int heap_mon_class::step_idx;
uint32_t heap_mon_class::smpl_cnt;
struct mallinfo2 heap_mon_class::minute_avg;
info_rmndr heap_mon_class::m_avg_remndr;
chrono::steady_clock::time_point heap_mon_class::strt_tm;
chrono::steady_clock::time_point heap_mon_class::last_tm;
int64_t heap_mon_class::intvl_usec;
int64_t heap_mon_class::run_usec;
int64_t heap_mon_class::nxt_step;

heap_mon_class::heap_mon_class()
{
    mi = mallinfo2();
    strt_tm = chrono::steady_clock::now();
    last_tm = strt_tm;
    cur_tm = strt_tm;
    minimums = mi;
    maximums = mi;
    acc3sec = mi;
    for ( int idx = 0; idx < 20; idx++ ) min_steps[ idx ] = mi;
    step_idx = 0;
    smpl_cnt = 1;
    nxt_step = usec_per_3sec;
    minute_avg = mi;
    m_avg_remndr.arena = 0;
    m_avg_remndr.ordblks = 0;
    m_avg_remndr.smblks = 0;
    m_avg_remndr.hblks = 0;
    m_avg_remndr.hblkhd = 0;
    m_avg_remndr.usmblks = 0;
    m_avg_remndr.fsmblks = 0;
    m_avg_remndr.uordblks = 0;
    m_avg_remndr.fordblks = 0;
    m_avg_remndr.keepcost = 0;
    new_info = false;
}

void heap_mon_class::get_info()
{
  // Use different averaging method
  //    struct chk_values {
  //        void calc( size_t &tval, size_t &min, size_t &minavg,
  //          size_t &max, int64_t &rmndr )
  //        {
  //            if ( tval < min ) min = tval;
  //            else if ( tval > max ) max = tval;
  //            int64_t cdif = rmndr + ( tval - minavg ) * intvl_usec;
  //            int64_t chgavg = cdif / usec_per_minute;
  //            rmndr = cdif - chgavg * usec_per_minute;
  //            minavg += chgavg;
  //        }
  //    } chk;
    struct chk_values {
        bool step_time;

        void calc( size_t &tval, size_t &min, size_t &minavg,
          size_t &max, size_t &mnst, size_t &ac3s, int64_t &rmndr )
        {
            if ( tval < min ) min = tval;
            else if ( tval > max ) max = tval;
            if ( step_time )
            {
                size_t new_mnst = ( ac3s + smpl_cnt / 2 ) / smpl_cnt;
                rmndr += new_mnst - mnst;
                int64_t avg_incr = rmndr / 20;
                if ( avg_incr > 0 )
                {
                    minavg += avg_incr;
                    rmndr -= avg_incr * 20;
                }
                else if ( avg_incr < 0 )
                {
                    size_t avgt = -avg_incr;
                    minavg = minavg > avgt ? minavg - avgt : 0;
                    rmndr -= avg_incr * 20;
                }
                else
                {
                    // rmndr remains less than a full increment so nothing
                    //   to do
                }
                mnst = new_mnst;
                ac3s = tval;
            }
            else
            {
                ac3s += tval;
            }
        }
    } chk;
    cur_tm = chrono::steady_clock::now();
    intvl_usec =
      chrono::duration_cast<chrono::microseconds>( cur_tm - last_tm ).count();
    run_usec =
      chrono::duration_cast<chrono::microseconds>( cur_tm - strt_tm ).count();
    if ( intvl_usec < min_usec_intvl ) return;
    mi = mallinfo2();
    chk.step_time = run_usec > nxt_step;
    chk.calc( mi.arena, minimums.arena, minute_avg.arena, maximums.arena,
      min_steps[ step_idx ].arena, acc3sec.arena, m_avg_remndr.arena );
    chk.calc( mi.ordblks, minimums.ordblks, minute_avg.ordblks,
      maximums.ordblks, min_steps[ step_idx ].ordblks, acc3sec.ordblks,
      m_avg_remndr.ordblks );
    chk.calc( mi.smblks, minimums.smblks, minute_avg.smblks, maximums.smblks,
      min_steps[ step_idx ].smblks, acc3sec.smblks, m_avg_remndr.smblks );
    chk.calc( mi.hblks, minimums.hblks, minute_avg.hblks, maximums.hblks,
      min_steps[ step_idx ].hblks, acc3sec.hblks, m_avg_remndr.hblks );
    chk.calc( mi.hblkhd, minimums.hblkhd, minute_avg.hblkhd, maximums.hblkhd,
      min_steps[ step_idx ].hblkhd, acc3sec.hblkhd, m_avg_remndr.hblkhd );
    chk.calc( mi.fsmblks, minimums.fsmblks, minute_avg.fsmblks,
      maximums.fsmblks, min_steps[ step_idx ].fsmblks, acc3sec.fsmblks,
      m_avg_remndr.fsmblks );
    chk.calc( mi.uordblks, minimums.uordblks, minute_avg.uordblks,
      maximums.uordblks, min_steps[ step_idx ].uordblks, acc3sec.uordblks,
      m_avg_remndr.uordblks );
    chk.calc( mi.fordblks, minimums.fordblks, minute_avg.fordblks,
      maximums.fordblks, min_steps[ step_idx ].fordblks, acc3sec.fordblks,
      m_avg_remndr.fordblks );
    chk.calc( mi.keepcost, minimums.keepcost, minute_avg.keepcost,
      maximums.keepcost, min_steps[ step_idx ].keepcost, acc3sec.keepcost,
      m_avg_remndr.keepcost );
    if ( chk.step_time )
    {
        step_idx = step_idx < 19 ? step_idx + 1 : 0;
        nxt_step += usec_per_3sec;
        smpl_cnt = 1;
    }
    else smpl_cnt++;
    last_tm = cur_tm;
    new_info = true;
}

void heap_mon_class::disp_desc( ostream& ostrm )
{
    ostrm << "    Heap monitor info display" << endl <<
      "Normal heap space allocated (bytes)" << endl <<  //    arena
      "Number of free chunks"               << endl <<  //    ordblks
      "Number of free fastbin blocks"       << endl <<  //    smblks
      "Number of mmapped regions"           << endl <<  //    hblks
      "Space allocd in mmapped reg (bytes)" << endl <<  //    hblkhd
      "Space in freed fastbin blks (bytes)" << endl <<  //    fsmblks
      "Total allocated space (bytes)"       << endl <<  //    uordblks
      "Total free space (bytes)"            << endl <<  //    fordblks
      "Top-most, releasable space (bytes)";             //    keepcost
}

void heap_mon_class::disp_values( ostream& ostrm )
{
    ostrm << "      Run time µsec = " << run_usec << endl <<
     // 2345678901234567890123456789012345678901234567890
      "   Minimum      latest    Minute avg   Maximum" << endl <<
      setw( v_wid ) << minimums.arena << setw( v_wid ) << mi.arena <<
      setw( v_wid ) << minute_avg.arena << setw( v_wid ) << maximums.arena << endl <<
      setw( v_wid ) << minimums.ordblks << setw( v_wid ) << mi.ordblks <<
      setw( v_wid ) << minute_avg.ordblks << setw( v_wid ) << maximums.ordblks << endl <<
      setw( v_wid ) << minimums.smblks << setw( v_wid ) << mi.smblks <<
      setw( v_wid ) << minute_avg.smblks << setw( v_wid ) << maximums.smblks << endl <<
      setw( v_wid ) << minimums.hblks << setw( v_wid ) << mi.hblks <<
      setw( v_wid ) << minute_avg.hblks << setw( v_wid ) << maximums.hblks << endl <<
      setw( v_wid ) << minimums.hblkhd << setw( v_wid ) << mi.hblkhd <<
      setw( v_wid ) << minute_avg.hblkhd << setw( v_wid ) << maximums.hblkhd << endl <<
      setw( v_wid ) << minimums.fsmblks << setw( v_wid ) << mi.fsmblks <<
      setw( v_wid ) << minute_avg.fsmblks << setw( v_wid ) << maximums.fsmblks << endl <<
      setw( v_wid ) << minimums.uordblks << setw( v_wid ) << mi.uordblks <<
      setw( v_wid ) << minute_avg.uordblks << setw( v_wid ) << maximums.uordblks << endl <<
      setw( v_wid ) << minimums.fordblks << setw( v_wid ) << mi.fordblks <<
      setw( v_wid ) << minute_avg.fordblks << setw( v_wid ) << maximums.fordblks << endl <<
      setw( v_wid ) << minimums.keepcost << setw( v_wid ) << mi.keepcost <<
      setw( v_wid ) << minute_avg.keepcost << setw( v_wid ) << maximums.keepcost;
    new_info = false;
}

void heap_mon_class::disp_info( ostream& ostrm )
{
    ostrm << "                         Run time µsec = " << run_usec << endl <<
      setw( 35 ) << "    Heap monitor info display"       <<
      "     Minimum          latest        Minute avg       Maximum" << endl <<
      setw( 35 ) << "Normal heap space allocated (bytes)" <<
      setw( v_wid ) << minimums.arena << setw( v_wid ) << mi.arena <<
      setw( v_wid ) << minute_avg.arena << setw( v_wid ) << maximums.arena << endl <<
      setw( 35 ) << "Number of free chunks"               <<
      setw( v_wid ) << minimums.ordblks << setw( v_wid ) << mi.ordblks <<
      setw( v_wid ) << minute_avg.ordblks << setw( v_wid ) << maximums.ordblks << endl <<
      setw( 35 ) << "Number of free fastbin blocks"       <<
      setw( v_wid ) << minimums.smblks << setw( v_wid ) << mi.smblks <<
      setw( v_wid ) << minute_avg.smblks << setw( v_wid ) << maximums.smblks << endl <<
      setw( 35 ) << "Number of mmapped regions"           <<
      setw( v_wid ) << minimums.hblks << setw( v_wid ) << mi.hblks <<
      setw( v_wid ) << minute_avg.hblks << setw( v_wid ) << maximums.hblks << endl <<
      setw( 35 ) << "Space allocd in mmapped reg (bytes)" <<
      setw( v_wid ) << minimums.hblkhd << setw( v_wid ) << mi.hblkhd <<
      setw( v_wid ) << minute_avg.hblkhd << setw( v_wid ) << maximums.hblkhd << endl <<
      setw( 35 ) << "Space in freed fastbin blks (bytes)" <<
      setw( v_wid ) << minimums.fsmblks << setw( v_wid ) << mi.fsmblks <<
      setw( v_wid ) << minute_avg.fsmblks << setw( v_wid ) << maximums.fsmblks << endl <<
      setw( 35 ) << "Total allocated space (bytes)"       <<
      setw( v_wid ) << minimums.uordblks << setw( v_wid ) << mi.uordblks <<
      setw( v_wid ) << minute_avg.uordblks << setw( v_wid ) << maximums.uordblks << endl <<
      setw( 35 ) << "Total free space (bytes)"            <<
      setw( v_wid ) << minimums.fordblks << setw( v_wid ) << mi.fordblks <<
      setw( v_wid ) << minute_avg.fordblks << setw( v_wid ) << maximums.fordblks << endl <<
      setw( 35 ) << "Top-most, releasable space (bytes)"  <<
      setw( v_wid ) << minimums.keepcost << setw( v_wid ) << mi.keepcost <<
      setw( v_wid ) << minute_avg.keepcost << setw( v_wid ) << maximums.keepcost;
    new_info = false;
}

bool heap_mon_class::is_new_info()
{
    return new_info;
}

//  #define HEAP_MON_Test
#ifdef HEAP_MON_Test  // Test program for the heap monitor class

#endif  // #ifdef HEAP_MON_Test

// #define MALLINFO2sample
#ifdef MALLINFO2sample  // Normally not defined as this section is just
                        //   for reference
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

static void
display_mallinfo2(void)
{
    struct mallinfo2 mi;

    mi = mallinfo2();

    printf("Total non-mmapped bytes (arena):       %zu\n", mi.arena);
    printf("# of free chunks (ordblks):            %zu\n", mi.ordblks);
    printf("# of free fastbin blocks (smblks):     %zu\n", mi.smblks);
    printf("# of mapped regions (hblks):           %zu\n", mi.hblks);
    printf("Bytes in mapped regions (hblkhd):      %zu\n", mi.hblkhd);
    printf("Max. total allocated space (usmblks):  %zu\n", mi.usmblks);
    printf("Free bytes held in fastbins (fsmblks): %zu\n", mi.fsmblks);
    printf("Total allocated space (uordblks):      %zu\n", mi.uordblks);
    printf("Total free space (fordblks):           %zu\n", mi.fordblks);
    printf("Topmost releasable block (keepcost):   %zu\n", mi.keepcost);
}

int main(int argc, char *argv[])
{
#define MAX_ALLOCS 2000000
    char *alloc[MAX_ALLOCS];
    int numBlocks, freeBegin, freeEnd, freeStep;
    size_t blockSize;

    if (argc < 3 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s num-blocks block-size [free-step "
                "[start-free [end-free]]]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    numBlocks = atoi(argv[1]);
    blockSize = atoi(argv[2]);
    freeStep = (argc > 3) ? atoi(argv[3]) : 1;
    freeBegin = (argc > 4) ? atoi(argv[4]) : 0;
    freeEnd = (argc > 5) ? atoi(argv[5]) : numBlocks;

    printf("============== Before allocating blocks ==============\n");
    display_mallinfo2();

    for (int j = 0; j < numBlocks; j++) {
        if (numBlocks >= MAX_ALLOCS) {
            fprintf(stderr, "Too many allocations\n");
            exit(EXIT_FAILURE);
        }

        alloc[j] = malloc(blockSize);
        if (alloc[j] == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
    }

    printf("\n============== After allocating blocks ==============\n");
    display_mallinfo2();

    for (int j = freeBegin; j < freeEnd; j += freeStep)
        free(alloc[j]);

    printf("\n============== After freeing blocks ==============\n");
    display_mallinfo2();

    exit(EXIT_SUCCESS);
}
#endif  // #ifdef MALLINFO2sample  // Normally not defined
