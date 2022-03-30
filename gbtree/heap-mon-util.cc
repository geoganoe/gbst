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

struct mallinfo2 heap_mon_class::mi;
struct mallinfo2 heap_mon_class::minimums[ 5 ];
struct mallinfo2 heap_mon_class::maximums[ 5 ];
struct mallinfo2 heap_mon_class::acc3sec;
struct mallinfo2 heap_mon_class::mnut_stps[ 20 ];
int heap_mon_class::step_idx;
int heap_mon_class::tst_idx;
uint32_t heap_mon_class::smpl_cnt;
struct mallinfo2 heap_mon_class::minute_avg;
info_rmndr heap_mon_class::m_avg_remndr;
chrono::steady_clock::time_point heap_mon_class::strt_tm;
int64_t heap_mon_class::last_run_usec;
int64_t heap_mon_class::intvl_usec;
int64_t heap_mon_class::run_usec;
int64_t heap_mon_class::nxt_step;
int64_t heap_mon_class::mnmx_nxt_step;

heap_mon_class::heap_mon_class()
{
    mi = mallinfo2();
    strt_tm = chrono::steady_clock::now();
    cur_tm = strt_tm;
    last_run_usec = 0;
    for ( int idx = 0; idx < 5; idx++ )
    {
        minimums[ idx ] = mi;
        maximums[ idx ] = mi;
    }
    acc3sec = mi;
    for ( int idx = 0; idx < 20; idx++ ) mnut_stps[ idx ] = mi;
    step_idx = 0;
    tst_idx = 0;
    smpl_cnt = 1;
    nxt_step = usec_per_3sec;
    mnmx_nxt_step = usec_per_3sec * 4;
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
    struct chk_values {
        bool step_time;

        void calc( MA_SZT mallinfo2::* mfld, MA_INT info_rmndr::* rfld )
        {
            if ( mi.*mfld < minimums[ 4 ].*mfld )
            {
                minimums[ 4 ].*mfld = mi.*mfld;
                for ( int idx = 3; idx >= 0 && mi.*mfld < minimums[ idx ].*mfld;
                  idx-- ) minimums[ idx ].*mfld = mi.*mfld;
            }
            else if ( mi.*mfld > maximums[ 4 ].*mfld )
            {
                maximums[ 4 ].*mfld = mi.*mfld;
                for ( int idx = 3; idx >= 0 && mi.*mfld > maximums[ idx ].*mfld;
                  idx-- ) maximums[ idx ].*mfld = mi.*mfld;
            }
            if ( step_time )
            {
                MA_SZT new_mnst = ( acc3sec.*mfld + smpl_cnt / 2 ) / smpl_cnt;
                m_avg_remndr.*rfld += new_mnst - mnut_stps[ step_idx ].*mfld;
                MA_INT avg_incr = m_avg_remndr.*rfld / 20;
                if ( avg_incr > 0 )
                {
                    minute_avg.*mfld += avg_incr;
                    m_avg_remndr.*rfld -= avg_incr * 20;
                }
                else if ( avg_incr < 0 )
                {
                    MA_SZT avgt = -avg_incr;
                    minute_avg.*mfld =
                      minute_avg.*mfld > avgt ? minute_avg.*mfld - avgt : 0;
                    m_avg_remndr.*rfld -= avg_incr * 20;
                }
                else
                {
                    // This m_avg_remndr field remains less than a full
                    //   increment so nothing to do
                }
                mnut_stps[ step_idx ].*mfld = new_mnst;
                acc3sec.*mfld = mi.*mfld;
            }
            else
            {
                acc3sec.*mfld += mi.*mfld;
            }
        }
    } chk;
    cur_tm = chrono::steady_clock::now();
    run_usec =
      chrono::duration_cast<chrono::microseconds>( cur_tm - strt_tm ).count();
    intvl_usec = run_usec - last_run_usec;
    if ( intvl_usec < min_usec_intvl ) return;
    mi = mallinfo2();
    chk.step_time = run_usec > nxt_step;
    chk.calc( &mallinfo2::arena, &info_rmndr::arena );
    chk.calc( &mallinfo2::ordblks, &info_rmndr::ordblks );
    chk.calc( &mallinfo2::smblks, &info_rmndr::smblks );
    chk.calc( &mallinfo2::hblks, &info_rmndr::hblks );
    chk.calc( &mallinfo2::hblkhd, &info_rmndr::hblkhd );
    chk.calc( &mallinfo2::fsmblks, &info_rmndr::fsmblks );
    chk.calc( &mallinfo2::uordblks, &info_rmndr::uordblks );
    chk.calc( &mallinfo2::fordblks, &info_rmndr::fordblks );
    chk.calc( &mallinfo2::keepcost, &info_rmndr::keepcost );
    if ( chk.step_time )
    {
        step_idx = step_idx < 19 ? step_idx + 1 : 0;
        nxt_step += usec_per_3sec;
        smpl_cnt = 1;
        if ( run_usec > mnmx_nxt_step )
        {
            for ( int idx = 1; idx < 5; idx ++ )
            {
                minimums[ idx - 1 ] = minimums[ idx ];
                maximums[ idx - 1 ] = maximums[ idx ];
            }
            minimums[ 4 ] = mi;
            maximums[ 4 ] = mi;
            mnmx_nxt_step += ( usec_per_3sec * 4 );
        }
    }
    else smpl_cnt++;
    last_run_usec = run_usec;
    new_info = true;
}

string heap_mon_class::disp_desc()
{
    ostringstream ostrm;
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
    return ostrm.str();
}

string heap_mon_class::disp_values()
{
    ostringstream ostrm;
    int tst = tst_idx;
    ostrm << "      Run time µsec = " << run_usec <<
      ", tst index = " << tst << endl << "  Recent Minimum          latest"
     // 2345678901234567890123456789012345678901234567890
      "      Minute avg  Recent Maximum" << endl <<
      setw( v_wid ) << minimums[ tst ].arena << setw( v_wid ) << mi.arena <<
      setw( v_wid ) << minute_avg.arena << setw( v_wid ) <<
      maximums[ tst ].arena << endl <<
      setw( v_wid ) << minimums[ tst ].ordblks << setw( v_wid ) << mi.ordblks <<
      setw( v_wid ) << minute_avg.ordblks << setw( v_wid ) <<
      maximums[ tst ].ordblks << endl <<
      setw( v_wid ) << minimums[ tst ].smblks << setw( v_wid ) << mi.smblks <<
      setw( v_wid ) << minute_avg.smblks << setw( v_wid ) <<
      maximums[ tst ].smblks << endl <<
      setw( v_wid ) << minimums[ tst ].hblks << setw( v_wid ) << mi.hblks <<
      setw( v_wid ) << minute_avg.hblks << setw( v_wid ) <<
      maximums[ tst ].hblks << endl <<
      setw( v_wid ) << minimums[ tst ].hblkhd << setw( v_wid ) << mi.hblkhd <<
      setw( v_wid ) << minute_avg.hblkhd << setw( v_wid ) <<
      maximums[ tst ].hblkhd << endl <<
      setw( v_wid ) << minimums[ tst ].fsmblks << setw( v_wid ) << mi.fsmblks <<
      setw( v_wid ) << minute_avg.fsmblks << setw( v_wid ) <<
      maximums[ tst ].fsmblks << endl <<
      setw( v_wid ) << minimums[ tst ].uordblks << setw( v_wid ) <<
      mi.uordblks << setw( v_wid ) << minute_avg.uordblks << setw( v_wid ) <<
      maximums[ tst ].uordblks << endl <<
      setw( v_wid ) << minimums[ tst ].fordblks << setw( v_wid ) <<
      mi.fordblks << setw( v_wid ) << minute_avg.fordblks << setw( v_wid ) <<
      maximums[ tst ].fordblks << endl <<
      setw( v_wid ) << minimums[ tst ].keepcost << setw( v_wid ) <<
      mi.keepcost << setw( v_wid ) << minute_avg.keepcost << setw( v_wid ) <<
      maximums[ tst ].keepcost;
    new_info = false;
    return ostrm.str();
}

string heap_mon_class::disp_info()
{
    ostringstream ostrm;
    int tst = 0;
    ostrm << "                         Run time µsec = " << run_usec << endl <<
      setw( 35 ) << "    Heap monitor info display" << "  Recent Minimum"
      "          latest      Minute avg  Recent Maximum" << endl <<
      setw( 35 ) << "Normal heap space allocated (bytes)" <<
      setw( v_wid ) << minimums[ tst ].arena << setw( v_wid ) << mi.arena <<
      setw( v_wid ) << minute_avg.arena << setw( v_wid ) <<
      maximums[ tst ].arena << endl <<
      setw( 35 ) << "Number of free chunks"               <<
      setw( v_wid ) << minimums[ tst ].ordblks << setw( v_wid ) << mi.ordblks <<
      setw( v_wid ) << minute_avg.ordblks << setw( v_wid ) <<
      maximums[ tst ].ordblks << endl <<
      setw( 35 ) << "Number of free fastbin blocks"       <<
      setw( v_wid ) << minimums[ tst ].smblks << setw( v_wid ) << mi.smblks <<
      setw( v_wid ) << minute_avg.smblks << setw( v_wid ) <<
      maximums[ tst ].smblks << endl <<
      setw( 35 ) << "Number of mmapped regions"           <<
      setw( v_wid ) << minimums[ tst ].hblks << setw( v_wid ) << mi.hblks <<
      setw( v_wid ) << minute_avg.hblks << setw( v_wid ) <<
      maximums[ tst ].hblks << endl <<
      setw( 35 ) << "Space allocd in mmapped reg (bytes)" <<
      setw( v_wid ) << minimums[ tst ].hblkhd << setw( v_wid ) << mi.hblkhd <<
      setw( v_wid ) << minute_avg.hblkhd << setw( v_wid ) <<
      maximums[ tst ].hblkhd << endl <<
      setw( 35 ) << "Space in freed fastbin blks (bytes)" <<
      setw( v_wid ) << minimums[ tst ].fsmblks << setw( v_wid ) << mi.fsmblks <<
      setw( v_wid ) << minute_avg.fsmblks << setw( v_wid ) <<
      maximums[ tst ].fsmblks << endl <<
      setw( 35 ) << "Total allocated space (bytes)" << setw( v_wid ) <<
      minimums[ tst ].uordblks << setw( v_wid ) << mi.uordblks <<
      setw( v_wid ) << minute_avg.uordblks << setw( v_wid ) <<
      maximums[ tst ].uordblks << endl <<
      setw( 35 ) << "Total free space (bytes)" << setw( v_wid ) <<
      minimums[ tst ].fordblks << setw( v_wid ) << mi.fordblks <<
      setw( v_wid ) << minute_avg.fordblks << setw( v_wid ) <<
      maximums[ tst ].fordblks << endl <<
      setw( 35 ) << "Top-most, releasable space (bytes)"  << setw( v_wid ) <<
      minimums[ tst ].keepcost << setw( v_wid ) << mi.keepcost <<
      setw( v_wid ) << minute_avg.keepcost << setw( v_wid ) <<
      maximums[ tst ].keepcost;
    new_info = false;
    return ostrm.str();
}

bool heap_mon_class::is_new_info()
{
    return new_info;
}

//
// The following is a copy of an example use of the mallinfo2 struct from
//   the man page, and is here only as a reference.
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
