#pragma once

#include <stdint.h>
#include <string.h>
#include <math.h>

#define SAMPLE_BITS 16

#define FREQ_SH         16  /* 16.16 fixed point (frequency calculations) */
#define EG_SH           16  /* 16.16 fixed point (envelope generator timing) */
#define LFO_SH          10  /* 22.10 fixed point (LFO calculations)       */
#define TIMER_SH        16  /* 16.16 fixed point (timers calculations)    */

#define FREQ_MASK       ((1<<FREQ_SH)-1)

#define ENV_BITS        10
#define ENV_LEN         (1<<ENV_BITS)
#define ENV_STEP        (128.0/ENV_LEN)

#define MAX_ATT_INDEX   (ENV_LEN-1) /* 1023 */
#define MIN_ATT_INDEX   (0)         /* 0 */

#define EG_ATT          4
#define EG_DEC          3
#define EG_SUS          2
#define EG_REL          1
#define EG_OFF          0

#define SIN_BITS        10
#define SIN_LEN         (1<<SIN_BITS)
#define SIN_MASK        (SIN_LEN-1)

#define TL_RES_LEN      (256) /* 8 bits addressing (real chip) */

#if (SAMPLE_BITS==16)
	#define FINAL_SH    (0)
	#define MAXOUT      (+32767)
	#define MINOUT      (-32768)
#else
	#define FINAL_SH    (8)
	#define MAXOUT      (+127)
	#define MINOUT      (-128)
#endif

/* translate from D1L to volume index (16 D1L levels) */
#define RATE_STEPS (8)

struct YM2151Operator {
	uint32_t    phase;              /* accumulated operator phase */
	uint32_t    freq;               /* operator frequency count */
	int32_t     dt1;                /* current DT1 (detune 1 phase inc/decrement) value */
	uint32_t    mul;                /* frequency count multiply */
	uint32_t    dt1_i;              /* DT1 index * 32 */
	uint32_t    dt2;                /* current DT2 (detune 2) value */

	uint32_t     *connect;           /* operator output 'direction' */

	/* only M1 (operator 0) is filled with this data: */
	uint32_t     *mem_connect;       /* where to put the delayed sample (MEM) */
	int32_t     mem_value;          /* delayed sample (MEM) value */

	/* channel specific data; note: each operator number 0 contains channel specific data */
	uint32_t    fb_shift;           /* feedback shift value for operators 0 in each channel */
	int32_t     fb_out_curr;        /* operator feedback value (used only by operators 0) */
	int32_t     fb_out_prev;        /* previous feedback value (used only by operators 0) */
	uint32_t    kc;                 /* channel KC (copied to all operators) */
	uint32_t    kc_i;               /* just for speedup */
	uint32_t    pms;                /* channel PMS */
	uint32_t    ams;                /* channel AMS */
	/* end of channel specific data */

	uint32_t    AMmask;             /* LFO Amplitude Modulation enable mask */
	uint32_t    state;              /* Envelope state: 4-attack(AR) 3-decay(D1R) 2-sustain(D2R) 1-release(RR) 0-off */
	uint8_t     eg_sh_ar;           /*  (attack state) */
	uint8_t     eg_sel_ar;          /*  (attack state) */
	uint32_t    tl;                 /* Total attenuation Level */
	int32_t     volume;             /* current envelope attenuation level */
	uint8_t     eg_sh_d1r;          /*  (decay state) */
	uint8_t     eg_sel_d1r;         /*  (decay state) */
	uint32_t    d1l;                /* envelope switches to sustain state after reaching this level */
	uint8_t     eg_sh_d2r;          /*  (sustain state) */
	uint8_t     eg_sel_d2r;         /*  (sustain state) */
	uint8_t     eg_sh_rr;           /*  (release state) */
	uint8_t     eg_sel_rr;          /*  (release state) */

	uint32_t    key;                /* 0=last key was KEY OFF, 1=last key was KEY ON */

	uint32_t    ks;                 /* key scale    */
	uint32_t    ar;                 /* attack rate  */
	uint32_t    d1r;                /* decay rate   */
	uint32_t    d2r;                /* sustain rate */
	uint32_t    rr;                 /* release rate */

	uint32_t    reserved0;          /**/
	uint32_t    reserved1;          /**/
};

class YM2151 {
public:
	YM2151(uint32_t clk, uint32_t rate);
	void reset();
	void writeReg(uint8_t r, uint8_t v);
	void update_one(int32_t **buffers, uint32_t length);

private:
	static const uint16_t phaseIncROM[768];
	static const uint8_t  lfo_noise_waveform[256];
	static const uint8_t  dt1Table[4*32];
	static const uint8_t  eg_inc[19*RATE_STEPS];
	static const uint8_t  egRateSelect[32+64+32];
	static const uint8_t  egRateShift[32+64+32];
	static const uint32_t dt2Table[4];

	YM2151Operator oper[32];        /* the 32 operators */

	uint32_t  pan[16];              /* channels output masks (0xffffffff = enable) */
	uint8_t   Muted[8];             /* used for muting */

	uint32_t  eg_cnt;               /* global envelope generator counter */
	uint32_t  eg_timer;             /* global envelope generator counter works at frequency = chipclock/64/3 */
	uint32_t  eg_timer_add;         /* step of eg_timer */
	uint32_t  eg_timer_overflow;    /* envelope generator timer overlfows every 3 samples (on real chip) */

	uint32_t  lfo_phase;            /* accumulated LFO phase (0 to 255) */
	uint32_t  lfo_timer;            /* LFO timer                        */
	uint32_t  lfo_timer_add;        /* step of lfo_timer                */
	uint32_t  lfo_overflow;         /* LFO generates new output when lfo_timer reaches this value */
	uint32_t  lfo_counter;          /* LFO phase increment counter      */
	uint32_t  lfo_counter_add;      /* step of lfo_counter              */
	uint8_t   lfo_wsel;             /* LFO waveform (0-saw, 1-square, 2-triangle, 3-random noise) */
	uint8_t   amd;                  /* LFO Amplitude Modulation Depth   */
	int8_t    pmd;                  /* LFO Phase Modulation Depth       */
	uint32_t  lfa;                  /* LFO current AM output            */
	int32_t   lfp;                  /* LFO current PM output            */

	uint8_t   test;                 /* TEST register */
	uint8_t   ct;                   /* output control pins (bit1-CT2, bit0-CT1) */

	uint32_t  noise;                /* noise enable/period register (bit 7 - noise enable, bits 4-0 - noise period */
	uint32_t  noise_rng;            /* 17 bit noise shift register */
	uint32_t  noise_p;              /* current noise 'phase'*/
	uint32_t  noise_f;              /* current noise period */

	uint32_t  csm_req;              /* CSM  KEY ON / KEY OFF sequence request */

	uint32_t  irq_enable;           /* IRQ enable for timer B (bit 3) and timer A (bit 2); bit 7 - CSM mode (keyon to all slots, everytime timer A overflows) */
	uint32_t  status;               /* chip status (BUSY, IRQ Flags) */
	uint8_t   connect[8];           /* channels connections */

	uint8_t   tim_A;                /* timer A enable (0-disabled) */
	uint8_t   tim_B;                /* timer B enable (0-disabled) */
	int32_t   tim_A_val;            /* current value of timer A */
	int32_t   tim_B_val;            /* current value of timer B */
	uint32_t  tim_A_tab[1024];      /* timer A deltas */
	uint32_t  tim_B_tab[256];           /* timer B deltas */

	uint32_t  timer_A_index;        /* timer A index */
	uint32_t  timer_B_index;        /* timer B index */
	uint32_t  timer_A_index_old;    /* timer A previous index */
	uint32_t  timer_B_index_old;    /* timer B previous index */

	uint32_t chanout[8];
	uint32_t m2,c1,c2; /* Phase Modulation input for operators 2,3,4 */
	uint32_t mem;      /* one sample delay memory */

	/*  Frequency-deltas to get the closest frequency possible.
	*   There are 11 octaves because of DT2 (max 950 cents over base frequency)
	*   and LFO phase modulation (max 800 cents below AND over base frequency)
	*   Summary:   octave  explanation
	*              0       note code - LFO PM
	*              1       note code
	*              2       note code
	*              3       note code
	*              4       note code
	*              5       note code
	*              6       note code
	*              7       note code
	*              8       note code
	*              9       note code + DT2 + LFO PM
	*              10      note code + DT2 + LFO PM
	*/
	uint32_t  freq[11*768];         /* 11 octaves, 768 'cents' per octave */

	/*  Frequency deltas for DT1. These deltas alter operator frequency
	*   after it has been taken from frequency-deltas table.
	*/
	int32_t   dt1_freq[8*32];       /* 8 DT1 levels, 32 KC values */

	uint32_t  noiseTable[32];        /* 17bit Noise Generator periods */

	uint32_t  clock;                /* chip clock in Hz (passed from 2151intf.c) */
	uint32_t  sampfreq;             /* sampling frequency in Hz (passed from 2151intf.c) */

	static uint32_t d1l_tab[16];

	/*  TL_TAB_LEN is calculated as:
	*   13 - sinus amplitude bits     (Y axis)
	*   2  - sinus sign bit           (Y axis)
	*   TL_RES_LEN - sinus resolution (X axis)
	*/
	#define TL_TAB_LEN (13*2*TL_RES_LEN)
	static int32_t tl_tab[TL_TAB_LEN];

	#define ENV_QUIET       (TL_TAB_LEN>>3)

	/* sin waveform table in 'decibel' scale */
	static uint32_t sin_tab[SIN_LEN];
	static bool tablesInitialized;

	void init_tables(void);
	void init_chip_tables();
	void envelope_KONKOFF(YM2151Operator *op, int v);
	void set_connect( YM2151Operator *om1, int cha, int v);

	void refresh_EG(YM2151Operator * op);
	signed int op_calc(YM2151Operator * OP, unsigned int env, signed int pm);
	signed int op_calc1(YM2151Operator * OP, unsigned int env, signed int pm);

	void chan_calc(uint8_t chan);
	void chan7_calc();
	void advance_eg();
	void advance();
};
