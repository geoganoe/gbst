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
//
//
// Use the following command to build this object:
//   g++ -std=c++17 -c fo-utils.cc
//   ar -Prs ~/data/lib/libfoutil.a fo-utils.o
//   ar -Ptv ~/data/lib/libfoutil.a

#include "fo-utils.h"
#include <iostream>
#include "../uni-utils/hex-symbol.h"
#include "../uni-utils/uni-utils.h"

#ifdef USEncurses
#include "ncursio.h"
#else
#include <cstring>
#endif   //  #ifdef USEncurses

void atexit_handl_3()
{
    cout << "at exit handler number 3." << endl;
}

//
// These will be kind of crude, as they are geared to identify only the
//   encoding types that I expect to see and do transforms from known
//   transformable data strings.
//
// The identify_encoding function will identify good ASCII7 only strings,
//   good UTF-8 strings with non-ASCII7 characters, strings that include only
//   the eight bit patterns define for ISO-8859-1 are identified as such and
//   will set the appropriate styp_flags without disturbing any of the
//   non-encoding related flags.
//

bool validate_iso8859_1char( uint16_t ch )
{
    //
    // GGG - Need to verify this
    bool checking = true;
    if ( ch > 0x00ff || ch < 0x0007 || ( ch > 0x007f && ch < 0x00a0 ) )
      checking = false;
    return checking;
}

styp_flags identify_encoding( string ch_seq, styp_flags dflt_flg )
{
    // GGG - This function needs to be Verified
    //   Problem found - When an ISO_8859_1 high bit char 0xf1 is seen in an
    //   otherwise ASCII string, it is treated as an UTF-8 character
    styp_flags these_flgs = dflt_flg;
    bool flg_ascii7 = true;
    bool flg_utf8_orig = true;
    bool flg_utf8_compat = true;
    // Not used right now -    bool flg_8859 = false;
    bool flg_8859_inval_found = false;
    uint16_t utf_chs[ 4 ] { 0, 0, 0, 0 };
    int utf_idx = 0;
    int utf_ch_expect = 0;
    // Not used right now -    bool need_utf_validation = false;
    int iter_num = 0;
    bool tmpdbg = false;

// Leave this for now as it may be needed in the future
#ifdef INdevel
    // Declarations/definitions/code for development only
    tmpdbg = dbgf.b2;
#endif // #ifdef INdevel

    if ( tmpdbg ) dbgs << "Chk: A" << ( flg_ascii7 ? '1' : '0' ) << 'U' <<
      ( flg_utf8_orig ? '1' : '0' ) << 'I' <<
      ( flg_8859_inval_found ? '0' : '1' );
    for ( char str_ch : ch_seq )
    {
        if ( tmpdbg ) dbgs << " '" << str_ch << "'";
        uint16_t ch = cmask & str_ch;
        if ( ( ch & 0x0080 ) != 0 )
        {
            if ( tmpdbg ) dbgs << hex_symbol( ch );
            flg_ascii7 = false;
            if ( tmpdbg ) dbgs << "a1";
            if ( utf_idx == 0 )
            {
                int num_high_bits_set = 1;
                uint16_t mask = 0x0040;
                while ( ( ch & mask ) != 0 )
                {
                    num_high_bits_set++;
                    mask >>= 1;
                }
                if ( num_high_bits_set < 2 || num_high_bits_set > 4 )
                {
                    flg_utf8_orig = false;
                    if ( tmpdbg ) dbgs << "u1";
                    flg_utf8_compat = false;
                    if ( !validate_iso8859_1char( ch ) )
                    {
                        flg_8859_inval_found = true;
                        if ( tmpdbg ) dbgs << "i1";
                    }
                }
                else
                {
                    utf_ch_expect = num_high_bits_set;
                    utf_chs[ utf_idx ] = ch;
                    utf_idx++;
                }
            }
            else if ( utf_idx < utf_ch_expect && ( ch & 0x0040 ) == 0 )
            {
                utf_chs[ utf_idx ] = ch;
                utf_idx++;
                if ( utf_idx == utf_ch_expect )
                {
                    // Should have a valid UTF-8 character in utf_chs[] at
                    //   this point, but need to validate it
                    string utf2validate( utf_ch_expect,
                      static_cast<char>( utf_chs[ 0 ] & cmask ) );
                    for ( int idx = 1; idx < utf_ch_expect; idx++ )
                    {
                        utf2validate[ idx ] =
                          static_cast<char>( utf_chs[ idx ] & cmask );
                    }
                    if ( is_ch_set_valid_utf8( utf2validate ) )
                    {
                        // OK, its valid utf8, so reset the check variables
                        // Since UTF-8 sequences are incompatible with
                        //   ISO_8859_1, the 8859 invalid flag needs to be set
                        flg_8859_inval_found = true;
                        if ( tmpdbg ) dbgs << "i2";
                    }
                    else
                    {
                        these_flgs.UTF_8_orig = 0;
                        if ( tmpdbg ) dbgs << "tu1";
                        these_flgs.UTF_8_compat = 0;
                        // Since that character set is not valid UTF-8, check
                        //   to see if they are all valid ISO-8859-1 chars
                        for ( int idx = 0; idx < utf_ch_expect; idx++ )
                        {
                            if ( !validate_iso8859_1char( utf_chs[ idx ] ) )
                            {
                                flg_8859_inval_found = true;
                                if ( tmpdbg ) dbgs << "i3";
                            }
                        }

                    }
                    // Finished with this set, so reset the check variables
                    for ( int idx = 0; idx < 4; idx++ ) utf_chs[ idx ] = 0;
                    utf_idx = 0;
                    utf_ch_expect = 0;
                }
            }
            else
            {
                // high bit set but not UTF-8 compatible
                these_flgs.UTF_8_orig = 0;
                if ( tmpdbg ) dbgs << "tu2";
                these_flgs.UTF_8_compat = 0;
                if ( !validate_iso8859_1char( ch ) )
                {
                    flg_8859_inval_found = true;
                    if ( tmpdbg ) dbgs << "i4";
                }

                if ( utf_idx > 0 )
                {
                    // Need to process the invalid potential UTF-8 chars too
                    for ( int idx = 0; idx < utf_idx; idx++ )
                    {
                        if ( !validate_iso8859_1char( utf_chs[ idx ] ) )
                        {
                            flg_8859_inval_found = true;
                            if ( tmpdbg ) dbgs << "i5";
                        }
                          flg_8859_inval_found = true;
                        utf_chs[ idx ] = 0;
                        utf_idx = 0;
                        utf_ch_expect = 0;
                    }
                }
            }
        }
        else if ( utf_idx != 0 )
        {
            // Not a UTF-8 sequence because expected high bit set char was
            //   not received so need to handle
            flg_utf8_orig = false;
            flg_utf8_compat = false;
            for ( int idx = 0; idx < utf_idx; idx++ )
            {
                if ( !validate_iso8859_1char( utf_chs[ idx ] ) )
                  flg_8859_inval_found = true;
                utf_chs[ idx ] = 0;
                utf_idx = 0;
                utf_ch_expect = 0;
            }
        }
        if ( tmpdbg ) dbgs << 'A' << ( flg_ascii7 ? '1' : '0' ) << 'U' <<
          ( flg_utf8_orig ? '1' : '0' ) << 'I' <<
          ( flg_8859_inval_found ? '0' : '1' );
        if ( tmpdbg && ( ++iter_num % 15 ) == 14 ) dbgs << endl;
    }
    if ( tmpdbg ) dbgs << endl;
    these_flgs.ASCII_7 = flg_ascii7;
    these_flgs.UTF_8_orig = flg_utf8_orig;
    these_flgs.UTF_8_compat = flg_utf8_compat;
    if ( !flg_8859_inval_found ) these_flgs.ISO_8859_1 = true;
    return these_flgs;
}

//
// The utf8from8859_1 function will take a known good ISO-8859-1 string and
//   convert it losslessly bi-directional to UTF-8 and return the UTF-8
//   string.
//
string utf8from8859_1( string iso8859_1 )
{
    // GGG - This function needs to be verified
    string transfrm;
    // Reserve enough characters to hold the most likely UTF-8 results.  Just
    //   add 5 to the iso8859 string size since the expected number of high
    //   bit characters is only 2 or 3
    int tr_reserve = iso8859_1.size() + 5;
    transfrm.reserve( tr_reserve );
    for ( auto iso_ch : iso8859_1 )
    {
        int16_t ich = 0x00ff & static_cast<int16_t>( iso_ch );
        if ( ( ich & 0x0080 ) != 0 )
        {
            // Need to convert this character to a UTF-8 string
            int16_t uch = 0x00c0 + ( ich >> 6 );
            string stutf( 2, static_cast<char>( uch & 0x00ff ) );
            uch = ( 0x0080 + ( ich & 0x003f ) );
            stutf[ 1 ]  = static_cast<char>( uch & 0x00ff );
            transfrm += stutf;
        }
        else transfrm.push_back( iso_ch );
    }
    return transfrm;
}

//
// The iso8859_1from_utf8 function will take a UTF-8 string with only
//   ISO-8859-1 characters and convert it to its original ISO-8859-1 form.
//
string iso8859_1from_utf8( string uft8strng )
{
    // GGG - This function needs to be verified.  Be careful as it looks
    //   like a malformed string could be translatewd without detection.
    // Since this function is intended to be used only on strings that
    //   have been properly converted from ISO_8859_1 to UTF-8, it is
    //   appropriate to exit from the program with error when an error is
    //   detected.
    string iso_str_trn;
    // This should be more than enough since the ISO_8859_1 string will have
    //   less than or equal to the same number of characters as the UTF-8
    iso_str_trn.reserve( uft8strng.size() );
    char utf_ch1 = 0;
    for ( auto utf_ch : uft8strng )
    {
        if ( utf_ch1 == 0 )
        {
            if ( ( utf_ch & 0x80) == 0x00 )
              // 7 bit ASCII char is just passed on as is
              iso_str_trn.push_back( utf_ch );
            else if ( ( utf_ch & 0xfe) == 0xc2 )
            {
                // Valid introducer chars must be either 0xc2 or 0xc3 so
                //   it doesn't matter what bit zero is
                utf_ch1 = utf_ch;
            }
            else
            {
                // Any high bit set introducer char that doesn't match the
                //   two valid chars above can not be contained in the ISO
                //   compatible UTF-8 string
                errs << "Invalid UTF-8 initial char in string \n\"" <<
                  uft8strng << "\" or as chars" << endl;
                for ( auto ch : uft8strng )
                {
                    if ( isgraph( ch ) ) errs << ch;
                    else errs << hex_symbol( cmask & ch, true );
                }
                errs << endl << endl << "Exiting from program with error" <<
                  " at fo-string-table.cc source line " << __LINE__ <<
                  "." << endl << endl;
                exit( 1 );
            }
        }
        else if ( ( utf_ch & 0xc0 ) == 0x80 )
        {
            // Second character is good, and all bits except bit 6 are right
            if ( ( utf_ch1 & 0x01 ) != 0 ) utf_ch |= 0x40;
              iso_str_trn.push_back( utf_ch );
            utf_ch1 = 0;
        }
    }
    return iso_str_trn;
}

significant_collation_chars scc;
scc_idx scc_idx_to_str_bal[ ggg_bal_lst_siz ];
uint16_t scc_set[ scc_set_size ];

string scc_set_array_to_utf8( scc_idx scc_strng )
{
    string tstr;
    for ( uint8_t scc_idx : scc_strng )
    {
        // Added some error checking
        uint16_t blst_ch = cmask & scc_idx;
        blst_ch < scc_set_size || ( blst_ch = scc_set_size - 1 );
        tstr += U_code_pt_to_UTF_8( scc_set[ blst_ch ] );
    }
    return tstr;
}

string scc_idx_to_UTF_8( uint8_t indx )
{
    //
    // On error return replacement character
    uint16_t blst_ch = cmask & indx;
    blst_ch < scc_set_size || ( blst_ch = scc_set_size - 1 );
    return U_code_pt_to_UTF_8( scc_set[ blst_ch ] );
}

string trans_scc( scc_idx scc_strng )
{
    // I have written this a few times now, so about time I made a function
    //   for it.  This one checks the ascii chars for isgraph and shows
    //   non-graphic chars as their hex-dot equivalent.
    string tstr;
    for ( uint8_t sccidx : scc_strng )
    {
        // Added some error checking
        uint16_t blst_ch = cmask & sccidx;
        blst_ch < scc_set_size || ( blst_ch = scc_set_size - 1 );
        string elem = U_code_pt_to_UTF_8( scc_set[ blst_ch ] );
        if ( elem.size() == 1 && !isgraph( elem[ 0 ] ) )
          elem = hex_symbol( cmask & elem[ 0 ] );
        tstr += elem;
    }
    return tstr;
}

int find_scc_ascii_index( char scc_ch )
{
    bool found = false;
    int found_idx = -1;
    for ( int idx = 0; idx < scc_size_ascii && !found; idx++ )
    {
        if ( scc.ascii_set[ idx ] == scc_ch ) found_idx = idx;
    }
    return found_idx;
}

fit_opts operator|( const fit_opts lft, const fit_opts rt )
{
    fit_opts rslt;
    rslt.plain = lft.plain | rt.plain;
    rslt.space_pad = lft.space_pad | rt.space_pad;
    rslt.centering = lft.centering | rt.centering;
    rslt.end_trim = lft.end_trim | rt.end_trim;
    rslt.show_ends = lft.show_ends | rt.show_ends;
    rslt.balance = lft.balance | rt.balance;
    rslt.trim_trl_wht_space = lft.trim_trl_wht_space | rt.trim_trl_wht_space;
    return rslt;
}

//
// The vector ivs is the holding place for index vectors that keep track
//   of the starting character positions of UTF-8 symbols in str_utf8
//   records.  ivs[0] contains the indexes of vectors that are available
//   for use by str_utf8 instantiations, ivs[1] contains the maximum
//   sizes that each element of the ivs vector has attained, and the rest
//   of the ivs elements are index vectors either in use or available for
//   use by the str_utf8 class.  The main reason for this structure is to
//   minimise heap fragmentation by maintaining a fixed set of commonly
//   used elements thus not having to create new heap allocations every
//   time a called method needs to create one of these vectors.
// GGG - Consider making this a static member of the str_utf8 class.
//   That would require some changes to places outside the class that
//   currently access the vector, but will advance the objective of
//   modularization and minimising global variable usage.
vector<index_vec> ivs __attribute__ ((init_priority (500))) =
  { { 2, 3 }, { 0, 0, 0, 0 }, {}, {} };
// GGG - Define this here for now.  May change it later
str_utf8 plbck_strg __attribute__ ((init_priority (1000)));

str_utf8::str_utf8()
{
    is_nm_str = 0;
    setup_index_vec();
    target = "";
    numchr = 0;
    numsym = 0;
    maxpos = 0;
}

str_utf8::str_utf8( string dflt, uint16_t maxp,
      fit_opts style, bool dbg_out )
{
    target = dflt;
    maxpos = maxp;
    is_nm_str = 0;
    setup_index_vec();
    if ( is_setup ) fit_to_max_cols( style, dbg_out );
    else
    {
        iout << "Invalid attempt to construct a str_utf8 variable," <<
          endl << " exiting." << endl;
        myexit ();
    }
}

str_utf8::str_utf8( string dflt, fit_opts style, bool dbg_out )
{
    target = dflt;
    maxpos = dflt.size();
    is_nm_str = 0;
    setup_index_vec();
    if ( is_setup ) fit_to_max_cols( style, dbg_out );
    else
    {
        iout << "Invalid attempt to construct a str_utf8 variable," <<
          endl << " exiting." << endl;
        myexit ();
    }
}

str_utf8::~str_utf8()
{
    if( is_setup )
    {
        ivs[ vec_idx ].clear();
        ivs[ 0 ].push_back( vec_idx );
    }
}

const uint16_t rescap = 48;

void str_utf8::setup_index_vec()
{
    if ( ivs.size() < 2 )
    {
        vec_idx = 0x1ff;
        is_setup = 0;
        return;
    }
    if ( ivs[ 0 ].size() < 1 )
    {
        // There are no available index_vec items, so we need to make one
        uint16_t nxt_avl = ivs.size();
        index_vec& new_vec = ivs.emplace_back();
        new_vec.reserve( rescap );
        while ( ivs[ 1 ].size() < ivs.size() ) ivs[ 1 ].push_back( 0 );
        vec_idx = nxt_avl;
    }
    else
    {
        vec_idx = ivs[ 0 ].back();
        ivs[ 0 ].pop_back();
    }
    if ( ivs[ vec_idx ].size() > 0 || ivs[ vec_idx ].capacity() < rescap )
    {
        // The vector is not properly initialized, so set it up
        if ( ivs[ vec_idx ].capacity() < rescap )
          ivs[ vec_idx ].reserve( rescap );
        ivs[ vec_idx ].clear();
    }
    uint16_t vsiz = ivs[ 0 ].size();
    if ( ivs[ 1 ].at( 0 ) < vsiz ) ivs[ 1 ].at( 0 ) = vsiz;
    vsiz = ivs[ 1 ].size();
    if ( ivs[ 1 ].at( 1 ) < vsiz ) ivs[ 1 ].at( 1 ) = vsiz;

#ifdef INdevel  // Declarations/definitions/code for development only
  if ( dbgf.b3 )
  {
    //
    // Change this section to only display when a size increases.
    static index_vec vchk = ivs[ 1 ];
    if ( vchk != ivs[ 1 ] )
    {
        dbgs << "Set ivec vec_idx = " << vec_idx <<
          ", and ivs size = " << ivs.size() << ", ivs[0] = {";
        bool first_out = true;
        for ( auto vnum : ivs[ 0 ] )
        {
            dbgs << ( first_out ? ( first_out = false, " " ) : ", " ) << vnum;
        }
        dbgs << " }." << endl;
        dbgs << "Get ivs[1] = {";
        first_out = true;
        for ( auto vnum : ivs[ 1 ] )
        {
            dbgs << ( first_out ? ( first_out = false, " " ) : ", " ) << vnum;
        }
        dbgs << " }." << endl;
        vchk = ivs[ 1 ];
    }
  }
#endif // #ifdef INdevel

    is_setup = 1;
}

index_vec& str_utf8::get_index_vec()
{
    if ( !is_setup )
    {
        setup_index_vec();
        if ( !is_setup )
        {
            iout << "Invalid attempt to get an index vec, exiting." <<
              endl;
            myexit ();
        }
    }
    index_vec& vget = ivs[ vec_idx ];
    return vget;
}

const index_vec& str_utf8::get_idxvec()
{
    if ( is_setup )
    {
        const index_vec& ivec = ivs[ vec_idx ];
        return ivec;
    }
    else
    {
        iout << "Invalid attempt to get an index vec, exiting." <<
          endl;
        myexit ();
    }
    const index_vec& ivec = { 0 };
    return ivec;
}

void str_utf8::init_str( string dflt, fit_opts style, bool dbg_out )
{
    target = dflt;
    maxpos = dflt.size();
    if ( !is_setup )
    {
        setup_index_vec();
        if ( !is_setup )
        {
            iout << "Invalid attempt to initialize a str_utf8, exiting." <<
              endl;
            myexit ();
        }
    }

#ifdef INdevel
    if ( dbgf.b4 )
      dbgs << " init_str has index_vec idx = " << vec_idx << ". " << endl;
#endif // #ifdef INdevel

    fit_to_max_cols( style, dbg_out );
}

void str_utf8::fit_to_max_cols( fit_opts style, bool dbg_out )
{
    if ( style.trim_trl_wht_space )
    {
        while ( target.size() > 1 && isspace( target.back() ) )
          target.pop_back();
    }
    string my_name = target;
    uint16_t maxp = maxpos;

#ifdef INdevel
    if ( dbgf.b4 && dbg_out ) dbgs << "fit_to_max_cols maxpos = " << maxp <<
      " and name: |" << my_name << "|.  ";
#endif // #ifdef INdevel

    index_vec& ivec = get_index_vec();
    target = "";
    string first_part = "";
    uint16_t nsymb1st = 0;
    uint16_t nchar1st = 0;
    string second_part = "";
    string h_ellips = "…";
    // Not used right now -    int nsymb2nd = 0;
    // Not used right now -    int nchar2nd = 0;
    uint16_t nsymb = 0;
    uint16_t nchar = 0;
    if ( ivec.size() > 0 ) ivec.clear();
    ivec.push_back( 0 );
    uint16_t utflft = 0;
    uint16_t stage = maxp;
    uint16_t cod_pt_16;
    bool nsym_adv = false;
    if ( style.end_trim )
    {
        if ( maxp > 5 )
        {
            stage = maxp - 1;
        }
    }
    else if ( style.show_ends )
    {
        if ( maxp > 5 )
        {
            stage = maxp - maxp / 4 - 1;
        }
    }
    else if ( style.balance )
    {
        if ( maxp > 5 )
        {
            stage = maxp / 2;
        }
    }
    // Not used right now -    int mynsiz = my_name.size();
    for ( char ach : my_name )
    {
        if ( nsymb < stage ) ;
        else if ( stage < maxp )
        {
            first_part = target;
            nsymb1st = nsymb;
            nchar1st = nchar;
            stage = style.end_trim ? maxp + 1 : my_name.size();
        }
        else break;
        uint16_t ch = cmask & ach;
        if ( utflft > 0 )
        {
            uint16_t mask = 0x003f;
            utflft--;
            if ( utflft == 0 )
            {
                cod_pt_16 += mask & ch;
                if ( !is_cod_pt_combining( cod_pt_16 ) ) nsym_adv = true;
            }
            else
            {
                cod_pt_16 = ( cod_pt_16 + ( mask & ch ) ) << ( utflft * 6 );
            }
        }
        else
        {
            if ( ( ch & 0x0080 ) != 0 )
            {
                // start a UTF-8 character symbol
                uint16_t num_high_bits_set = 1;
                uint16_t mask = 0x0040;
                while ( ( ch & mask ) != 0 )
                {
                    num_high_bits_set++;
                    mask >>= 1;
                }
                if ( num_high_bits_set < 2 || num_high_bits_set > 4 )
                {
                    my_exit_msg = "Illegal UTF-8 char with ";
                    my_exit_msg += to_string( num_high_bits_set );
                    my_exit_msg += " num_high_bits_set.  ";
                    myexit();
                }
                else
                {
                    utflft = num_high_bits_set - 1;
                    mask = 0x7f >> num_high_bits_set;
                    cod_pt_16 = ( mask & ch ) << ( utflft * 6 );
                }
            }
            else nsym_adv = true;
        }
        target.push_back( ach );
        nchar++;
        if ( nsym_adv )
        {
            nsymb++;
            ivec.push_back( nchar );
            nsym_adv = false;
        }
    }
    if ( ivec.size() > ivs[ 1 ].at( vec_idx ) )
      ivs[ 1 ].at( vec_idx ) = ivec.size();
    if ( style.end_trim )
    {
        if ( nsymb > maxp && nsymb1st > 0 )
        {
            // Too many symbols, so add h_ellips to end of first_part
            target = first_part;
            target.append( h_ellips );
            nsymb = nsymb1st + 1;
            nchar = nchar1st + h_ellips.size();
            ivec[ nsymb ] = nchar;
            if ( ivec.size() > nsymb + 1u ) ivec.resize( nsymb + 1 );
        }
        else if ( nsymb == maxp )
        {
            // Just use the target as is

            // Need to get the single symbol from the second part
            //    target = first_part;
            //    target.append( second_part );
            //    nsymb += 1;
            //    nchar = nchar1st + second_part.size();
        }
    }
    else if ( nsymb > maxp && nsymb1st > 0 )
    {
        uint16_t sym_need = maxp - nsymb1st - 1;
        uint16_t chr_need = ivec[ nsymb - sym_need ];
        second_part = target.substr( chr_need );
        target = first_part;
        target.append( h_ellips );
        uint16_t nchbase = nchar1st + h_ellips.size();
        uint16_t symbase = nsymb1st + 1;
        ivec[ symbase ] = nchbase;
        uint16_t chr_rmov = chr_need - nchbase;
        // Not used right now -    uint16_t sym_rmov = nsymb - sym_need - symbase;
        for ( uint16_t idx = 1; idx <= sym_need; idx ++ )
          ivec[ symbase + idx ] = ivec[ nsymb - sym_need + idx ] - chr_rmov;
        target.append( second_part );
        nsymb = symbase + sym_need;
        nchar = nchbase + second_part.size();
        if ( ivec.size() > nsymb + 1u ) ivec.resize( nsymb + 1 );
    }
    if ( nsymb < maxp )
    {
        if ( style.centering )
        {
            uint16_t ld_spc_nd = ( maxp - nsymb ) / 2;
            uint16_t trl_spc_nd = maxp - nsymb - ld_spc_nd;
            if ( ld_spc_nd > 0 ) target.insert( 0, ld_spc_nd, ' ' );
            target.append( trl_spc_nd, ' ' );
            nchar += ld_spc_nd + trl_spc_nd;
            nsymb += ld_spc_nd + trl_spc_nd;
            for ( auto val : ivec ) val += ld_spc_nd;
            for ( uint16_t idx = 0; idx < ld_spc_nd; idx ++ )
              ivec.insert( ivec.begin() + idx, idx );
            uint16_t basechpos = ivec.back();
            for ( uint16_t idx = 1; idx <= trl_spc_nd; idx ++ )
              ivec.push_back( basechpos + idx );
        }
        else if ( style.space_pad )
        {
            uint16_t spc_ndd = maxp - nsymb;
            target.append( spc_ndd, ' ' );
            nchar += spc_ndd;
            nsymb += spc_ndd;
            uint16_t basechpos = ivec.back();
            for ( uint16_t idx = 1; idx <= spc_ndd; idx ++ )
              ivec.push_back( basechpos + idx );
        }
    }

#ifdef INdevel
    if ( dbgf.b4 && dbg_out ) dbgs << "Returning target |" << target <<
      "|, target size = " << target.size() << ", ivec size = " <<
      ivec.size() << ", ivec nchars = " << ivec.back() <<
      ", nchar = " << nchar << " and nsymb = " << nsymb << "." << endl;
#endif // #ifdef INdevel

    numchr = nchar;
    numsym = nsymb;
}

void str_utf8::append( const string& s )
{
    str_utf8 utst( s );
    append( utst );
}

void str_utf8::append( const str_utf8& utst, uint16_t sym_pos, uint16_t sym_nm )
{
    if ( !is_setup )
    {
        setup_index_vec();
        if ( !is_setup )
        {
            iout << "Invalid attempt to initialize a str_utf8, exiting." <<
              endl;
            myexit ();
        }
    }
    if ( utst.vec_idx < 2 || utst.vec_idx > ivs.size() - 1 )
    {
        iout << "Invalid str_utf8 reference parameter passed to "
            "str_utf8::append(), exiting." << endl;
        myexit ();
    }
    const index_vec& uivec = ivs [ utst.vec_idx ];
    index_vec& ivec = get_index_vec();
    if ( sym_pos > uivec.size() - 1 )
    {
        // Request is past the end of the appending string, so just return
        return;
    }
    if ( utst.numsym == 0 || !(sym_pos < utst.numsym ) ||
      utst.numsym != uivec.size() - 1 )
      // There is nothing to append or a faulty utst parameter,
      //   so just return
      return;

    uint16_t st_chr_pos = uivec[ sym_pos ];
    uint16_t end_sym_pos = sym_pos + sym_nm;
    end_sym_pos = end_sym_pos > uivec.size() ? uivec.size() : end_sym_pos;
    uint16_t chr_nm = ( end_sym_pos > uivec.size() - 1 ?
      utst.numchr : uivec[ end_sym_pos ] ) - st_chr_pos;
    target.append( utst.target, st_chr_pos, chr_nm );
    for ( uint16_t sym_idx = sym_pos; sym_idx < end_sym_pos; sym_idx++ )
      ivec.push_back( numchr + uivec[ sym_idx ] - st_chr_pos );
    numchr += chr_nm;
    numsym += end_sym_pos - 1;
    maxpos = numsym;
}

void str_utf8::clear()
{
    is_nm_str = 0;
    ivs[ vec_idx ].clear();
    target = "";
    numchr = 0;
    numsym = 0;
    maxpos = 0;
}

#if defined (INFOdisplay) || defined (USEncurses)
uint16_t find_safe_break( string subj_str, int min_sym_p, int max_sym_p,
  bool dbg_out )
{
    uint16_t min_sym, max_sym;
    if ( min_sym_p > 0 && max_sym_p > min_sym_p && max_sym_p < ssiz_max )
    {
        min_sym = min_sym_p;
        max_sym = max_sym_p;
    }
    else
    {
        uint16_t rval = max_sym_p < 0 ? 0 :
          max_sym_p < ssiz_max ? max_sym_p : ssiz_max;
        rval = rval > min_sym_p ? rval :
          min_sym_p < ssiz_max ? min_sym_p : ssiz_max;
        // return static_cast<uint16_t>( max_sym_p > min_sym_p ? );
        return rval;
    }
    //
    // Find a safe line breaking place in string subj_str in between
    //   symbol position min_sym and position max_sym.  The preferred
    //   position is the last white space in the range.  If no white
    //   space position is available in the range, then the break should
    //   be done after the middle symbol in the range.  The string
    //   character position for the break is returned.
    str_utf8 find_max_sym( subj_str, max_sym, fit_plain, dbg_out );
    if ( find_max_sym.numsym < max_sym ) return find_max_sym.numchr;
    str_utf8 find_min_sym( subj_str, min_sym, fit_plain, dbg_out );
    // Try to find white space in range
    int idx;
    for ( idx = find_max_sym.numchr;
      idx > find_min_sym.numchr && subj_str[ idx ] != ' '; idx-- ) ;
    if ( subj_str[ idx ] == ' ' ) return idx;
    find_max_sym.maxpos = ( find_max_sym.numsym + find_min_sym.numsym ) / 2;
    find_max_sym.fit_to_max_cols( dbg_out );
    return find_max_sym.numchr;
}
#endif  //  #if defined (INFOdisplay) || defined (USEncurses)

bool test_scc_set_validity()
{
    bool test_success = true;
    scc_idx scc_test_str( 3, 1 );
    //    scc_test_str[ 1 ] = 1;
    //    scc_test_str[ 2 ] = 20;
    scc_idx ss1 = scc_test_str;
    string ts1 = scc_set_array_to_utf8( scc_test_str );
    str_utf8 check;
    for ( int idx = 1; idx <= scc_set_size - 2; idx++ )
    {
        scc_test_str[ 0 ] = idx;
        for ( int jdx = 1; jdx <= scc_set_size - 2; jdx++ )
        {
            scc_test_str[ 1 ] = jdx;
            for ( int kdx = idx > 1 || jdx > 1 ? 1 : 2;
              kdx <= scc_set_size - 2; kdx++ )
            {
                scc_test_str[ 2 ] = kdx;
                scc_idx ss2 = scc_test_str;
                string ts2 = scc_set_array_to_utf8( scc_test_str );
                int tscmp = strcoll( ts1.c_str(), ts2.c_str() );
                if ( tscmp >= 0 )
                {
                    if ( test_success )
                      check.init_str( "Error in scc sequence at compare of [" );
                    else
                    {
                        if ( check.numsym > 100 )
                        {
                            cout << check.target << "," << endl;
                            check.init_str( "    [" );
                        }
                        else
                          check.append( ", [" );
                    }
                    check.append( trans_scc( ss1 ) + "] ≮ [" +
                      trans_scc( ss2 ) + "]" );
                    test_success = false;
                }
                ss1 = ss2;
                ts1 = ts2;
            }
        }
    }
    if ( check.numsym > 0 )
      cout << check.target << "." << endl;
    return test_success;
}

string my_exit_msg;

void myexit()
{

#ifdef USEncurses
    if ( foiorf != nullptr )
    {
        if ( isendwin() == false ) endwin();
    }
#endif   //  #ifdef USEncurses

#ifdef INFOdisplay
    if ( bsvistrm.tellp() > 0 ) cout <<
      "Saved info stream is as follows:" << endl <<
      bsvistrm.str() << endl << "     ----" << endl;

#else

    cout << "Performing requested exit with error." << endl;
#endif // #ifdef INFOdisplay

    //   if ( dbstrm.tellp() > 0 ) cout <<
    //     "Saved debug info is as follows:" << endl <<
    //     dbstrm.str() << endl;
    cout << endl << my_exit_msg << endl;
    exit( 1 );
}

#ifdef INdevel    // Declarations for development only
void getout( string intro, int contprmt )
{

#ifdef USEncurses
    if ( foiorf != nullptr )
    {
        if ( isendwin() == false ) endwin();
    }
#endif   //  #ifdef USEncurses

    string ans;

#ifdef USEncurses
    cout <<

#else

    errs <<
#endif   //  #ifdef USEncurses

            intro <<
      endl << "Do you want to continue?  [y/N]  ";
    cin >> ans;
    if ( !( ans.find_first_of( "Yy" ) < ans.size() ) ) myexit();
    int ansyes = 0;
    while ( ansyes < contprmt )
    {
        if ( ans.find_first_of( "Yy" ) < ans.size() )
        {

#ifdef USEncurses
            cout <<

#else

            errs <<
#endif   //  #ifdef USEncurses

                    "Proceeding is not recommended.  Are you sure? ";
            cin >> ans;
            ansyes++;
        }
        else
        {
            myexit();
        }
    }
}

//
// Print the significant_collation_chars set and the string array
//   ggguniq_str_bal_list to the console one item per line
// GGG - Not currently used, but may be desired in the future
void display_scc_and_strbal( bool enable )
{
    if ( !enable ) return;
    // Already set up so not needed here - uint16_t scc_set[ scc_set_size ];
    //  for ( int idx = 0; idx < 40; idx++ )
    //    scc_set[ idx ] = cmask & scc.ascii_set[ idx ];
    //  for ( int idx = 40; idx < 47; idx++ )
    //    scc_set[ idx ] = scc.utf_16_set[ idx - 40 ];
    iout << "This is the significant_collation_chars and" <<
      " ggguniq_str_bal_list" << endl;
    for ( int idx = 0; idx < scc_set_size; idx++ )
      iout << U_code_pt_to_UTF_8( scc_set[ idx ] ) << endl;
    for ( int idx = 0; idx < ggg_bal_lst_siz; idx++ )
      iout << ggguniq_str_bal_list[ idx ] << endl;
    iout << endl;
    iout << U_code_pt_to_UTF_8( 0x01fa7a ) << endl;
    iout << U_code_pt_to_UTF_8( 0x01fa81 ) << endl;
    iout << U_code_pt_to_UTF_8( 0x02000b ) << endl;
}
#endif // #ifdef INdevel

void shoflgs( string intro, styp_flags flgs )
{

#ifdef INdevel
    dbgs << intro << flgs.ASCII_7 <<
      ", UTF_8_orig = " << flgs.UTF_8_orig <<
      ", UTF_8_compat = " << flgs.UTF_8_compat <<
      ", ISO_8859_1 = " << flgs.ISO_8859_1 <<
      ", UCS_2 = " << flgs.UCS_2 <<
      ", UTF_16 = " << flgs.UTF_16 << endl;
#endif // #ifdef INdevel

}

//
// Comment these out for now as they are not currently being used
//     #define BUILD_COLOR_CONVERSIONS
#ifdef BUILD_COLOR_CONVERSIONS

//
// Color conversion found on https://www.cs.rit.edu/~ncs/color/t_convert.html
// r,g,b values are from 0 to 1
// h = [0,360], s = [0,1], v = [0,1]
//      if s == 0, then h = -1 (undefined)
//
// GGG - Has a problem where delta can be zero and appears in a denominator
//   and if delta is = 0 then the color is a gray scale, so h is undefined,
//   s is 0, and v is the r, g, and b value which are all the same.  Also,
//   the ranges above should be
//     h = [0,360), s = [0,1), v = [0,1) because the maximum values should
//   be h < 360.00, s < 1.00, and v < 1.0
// GGG - Also, I have changed the arguments from pointers to references
//   to make things easier and safer

void RGBtoHSV( float r, float g, float b, float& h, float& s, float& v )
{
    float min, max, delta;

    //
    // GGG - Do some bounds checking before processing the conversion
    if ( r < 0 || r >= 1 || g < 0 || g >= 1 || b < 0 || b >= 1 )
    {
        // Any value outside the bounds tested is malformed, so exit
        exit( 1 );
    }
    // GGG - These macros aren't defined here, so spell it out
    //    min = MIN( r, g, b );
    //    max = MAX( r, g, b );
    min = r < g ? r : g;
    min = min < b ? min  : b;
    max = r > g ? r : g;
    max = max > b ? max  : b;
    v = max;               // v

    delta = max - min;

    // GGG - Unreliable test -- if( max != 0 )
    if( max > 0 )
        s = delta / max;       // s
    else {
        // r = g = b = 0        // s = 0, v is undefined
        s = 0;
        h = -1;
        return;
    }

    // GGG - The == tests here will only work reliably because max is set
    //   to one of the three values
    if( r == max )
        h = ( g - b ) / delta;     // between yellow & magenta
    else if( g == max )
        h = 2 + ( b - r ) / delta; // between cyan & yellow
    else
        h = 4 + ( r - g ) / delta; // between magenta & cyan

    h *= 60;               // degrees
    if( h < 0 )
        h += 360;

}

void HSVtoRGB( float& r, float& g, float& b, float h, float s, float v )
{
    int i;
    float f, p, q, t;

    //
    // GGG - Do some bounds checking before processing the conversion
    if ( s < 0 || s >= 1 || h < 0 || h >= 1 || v < 0 || v >= 360 )
    {
        // Any value outside the bounds tested is malformed, so exit
        exit( 1 );
    }
    else if( s < 0.00001 ) {  // change to a small epsilon
        // GGG - since we already know that it is >= 0
        // achromatic (grey)
        r = g = b = v;
        return;
    }

    h /= 60;            // sector 0 to 5
    // GGG - i = floor( h );    // I don't want to include math.h, and I
    i = static_cast<int>( h );  //   know that h >=0 so the cast will do
                                //   the same thing as floor
                        // GGG fractional not factorial
    f = h - i;          // factorial part of h
    p = v * ( 1 - s );
    q = v * ( 1 - s * f );
    t = v * ( 1 - s * ( 1 - f ) );

    switch( i ) {
        case 0:
            r = v;
            g = t;
            b = p;
            break;
        case 1:
            r = q;
            g = v;
            b = p;
            break;
        case 2:
            r = p;
            g = v;
            b = t;
            break;
        case 3:
            r = p;
            g = q;
            b = v;
            break;
        case 4:
            r = t;
            g = p;
            b = v;
            break;
        default:        // case 5:
            r = v;
            g = p;
            b = q;
            break;
    }

}

//
// Another color conversion found on
//   https://stackoverflow.com/questions/3018313/algorithm-to-convert-
//     rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both

//
// Poster "David H - Senior Mobile Engineer" comment:
// I've used these for a long time - no idea where they came from at this
//    point... Note that the inputs and outputs, except for the angle in
//    degrees, are in the range of 0 to 1.0.
//
// NOTE: this code does no real sanity checking on inputs. Proceed with
//   caution!

typedef struct {
    double r;       // a fraction between 0 and 1
    double g;       // a fraction between 0 and 1
    double b;       // a fraction between 0 and 1
} rgb;

typedef struct {
    double h;       // angle in degrees
    double s;       // a fraction between 0 and 1
    double v;       // a fraction between 0 and 1
} hsv;

static hsv   rgb2hsv(rgb in);
static rgb   hsv2rgb(hsv in);

hsv rgb2hsv(rgb in)
{
    hsv         out;
    double      min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min  < in.b ? min  : in.b;
    max = in.r > in.g ? in.r : in.g;
    max = max  > in.b ? max  : in.b;
    //
    // GGG - Do some range checking
    if ( min < 0.0 || max >= 1.0 )
    {
        // GGG - Some value is out of range so exit due to malformed program
        errs << "Values r, g, and b must be greater than or equal to 0.0" <<
          " and less than 1.0, and are:" << endl <<
          " r = " << in.r << ", g = " << in.g << ", b = " << in.b << endl <<
          "Exiting due to malformed program!" << endl;
        exit( 1 );
    }
    out.v = max;                                // v
    delta = max - min;
    if (delta < 0.00001)
    {
        out.s = 0;
        out.h = 0; // undefined, maybe nan?
        return out;
    }
    if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
        out.s = (delta / max);                  // s
    } else {
        // if max is 0, then r = g = b = 0
        // s = 0, h is undefined
        out.s = 0.0;
        out.h = 0.0;  // GGG - No math.h   NAN; // its now undefined
        return out;
    }
    if( in.r >= max )                           // > is bogus, just keeps compilor happy
        out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
    else
    if( in.g >= max )
        out.h = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
    else
        out.h = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan

    out.h *= 60.0;                              // degrees

    if( out.h < 0.0 )
        out.h += 360.0;

    return out;
}


rgb hsv2rgb(hsv in)
{
    double      hh, p, q, t, ff;
    long        i;
    rgb         out;

    //
    // GGG - Do some range checking
    if ( in.v < 0.0 || in.v >= 1.0 || in.s < 0.0 || in.s >= 1.0 ||
      in.h < 0.0 || in.h >= 360.0 )
    {
        // GGG - Some value is out of range so exit due to malformed program
        errs << "Value h must be greater than or equal to 0.0 and" <<
          " less than 360.0, and is " << in.h << endl <<
          "Value s must be greater than or equal to 0.0 and" <<
          " less than 1.0, and is " << in.s << endl <<
          "Value s must be greater than or equal to 0.0 and" <<
          " less than 1.0, and is " << in.s << endl <<
          "Exiting due to malformed program!" << endl;
        exit( 1 );
    }
    if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = in.v;
        out.g = in.v;
        out.b = in.v;
        return out;
    }
    hh = in.h;
    //
    // GGG - This has already been checked so no need to do it again
    //   if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = static_cast<long>( hh );
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }
    return out;
}

#endif  //  #ifdef BUILD_COLOR_CONVERSIONS
