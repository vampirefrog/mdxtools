/*  File: adpcm.c
    Description: Routines to convert 12 bit linear samples to the
       Dialogic or Oki ADPCM coding format.
       I copied the algorithms out of the book "PC Telephony - The
       complete guide to designing, building and programming systems
       using Dialogic and Related Hardware" by Bob Edgar. pg 272-276.

*/

# include "adpcm.h"

/* Note: Edgar's book says that the second to last value is 1408; however,
 * The standard says it is 1411.
 * Changed on 1/17/2003.
*/

static short step_size[49] = { 16, 17, 19, 21, 23, 25, 28, 31, 34, 37, 41,
     45, 50, 55, 60, 66, 73, 80, 88, 97, 107, 118, 130, 143, 157, 173,
     190, 209, 230, 253, 279, 307, 337, 371, 408, 449, 494, 544, 598, 658,
     724, 796, 876, 963, 1060, 1166, 1282, 1411, 1552 };
/*
* one function local to this file only.
*/
static short step_adjust ( char );

/*
* Initialze the data used by the coder.
*/
void adpcm_init( struct adpcm_status *stat ) {
    stat->last = 0;
    stat->step_index = 0;
    return;
}

/*
* Encode linear to ADPCM
*/
char adpcm_encode( short samp, struct adpcm_status *stat ) {
    short code;
    short diff, E, SS;

    /* printf( "%d\t", samp );
    */
    SS = step_size[stat->step_index];
    code = 0x00;
    if( (diff = samp - stat->last) < 0 )
        code = 0x08;
    E = diff < 0 ? -diff : diff;
    if( E >= SS ) {
        code = code | 0x04;
        E -= SS;
    }
    if( E >= SS/2 ) {
        code = code | 0x02;
        E -= SS/2;
    }
    if( E >= SS/4 ) {
        code = code | 0x01;
    }
/*    stat->step_index += step_adjust( code );
    if( stat->step_index < 0 ) stat->step_index = 0;
    if( stat->step_index > 48 ) stat->step_index = 48;
*/
    /*
    * Use the decoder to set the estimate of last sample.
    * It also will adjust the step_index for us.
    */
    stat->last = adpcm_decode(code, stat);
    return( code );
}

/*
* Decode Linear to ADPCM
*/
short adpcm_decode( char code, struct adpcm_status *stat ) {
    short diff, E, SS, samp;

    /* printf( "%x\t", code );
    */
    SS = step_size[stat->step_index];
    E = SS/8;
    if ( code & 0x01 )
        E += SS/4;
    if ( code & 0x02 )
        E += SS/2;
    if ( code & 0x04 )
        E += SS;
    diff = (code & 0x08) ? -E : E;
    samp = stat->last + diff;

    /*
    *  Clip the values to +(2^11)-1 to -2^11. (12 bits 2's
    *  compelement)
    *  Note: previous version errantly clipped at +2048, which could
    *  cause a 2's complement overflow and was likely the source of
    *  clipping problems in the previous version.  Thanks to Frank
    *  van Dijk for the correction.  TLB 3/30/04
    */
    if( samp > 2047 )
    {
      samp = 2047;
    }
    if( samp < -2048 )
    {
      samp = -2048;
    }

    stat->last = samp;
    stat->step_index += step_adjust( code );
    if( stat->step_index < 0 ) stat->step_index = 0;
    if( stat->step_index > 48 ) stat->step_index = 48;

    /* printf( "%d\n", samp );
    */
    return( samp );
}

/*
* adjust the step for use on the next sample.
*/
short step_adjust ( char code ) {
    short adjust[8] = {
        -1, -1, -1, -1,
         2,  4,  6,  8,
    };
    return adjust[code & 0x07];
}
