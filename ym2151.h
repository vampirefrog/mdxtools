/*
** File: ym2151.h - header file for software implementation of YM2151
**                                            FM Operator Type-M(OPM)
**
** (c) 1997-2002 Jarek Burczynski (s0246@poczta.onet.pl, bujar@mame.net)
** Some of the optimizing ideas by Tatsuyuki Satoh
**
** Version 2.150 final beta May, 11th 2002
**
**
** I would like to thank following people for making this project possible:
**
** Beauty Planets - for making a lot of real YM2151 samples and providing
** additional informations about the chip. Also for the time spent making
** the samples and the speed of replying to my endless requests.
**
** Shigeharu Isoda - for general help, for taking time to scan his YM2151
** Japanese Manual first of all, and answering MANY of my questions.
**
** Nao - for giving me some info about YM2151 and pointing me to Shigeharu.
** Also for creating fmemu (which I still use to test the emulator).
**
** Aaron Giles and Chris Hardy - they made some samples of one of my favourite
** arcade games so I could compare it to my emulator.
**
** Bryan McPhail and Tim (powerjaw) - for making some samples.
**
** Ishmair - for the datasheet and motivation.
*/

#pragma once

/* 16- and 8-bit samples (signed) are supported*/
#define SAMPLE_BITS 16

typedef stream_sample_t SAMP;

/* struct describing a single operator */
struct ym2151_operator {
	UINT32      phase;                  /* accumulated operator phase */
	UINT32      freq;                   /* operator frequency count */
	INT32       dt1;                    /* current DT1 (detune 1 phase inc/decrement) value */
	UINT32      mul;                    /* frequency count multiply */
	UINT32      dt1_i;                  /* DT1 index * 32 */
	UINT32      dt2;                    /* current DT2 (detune 2) value */

	signed int *connect;                /* operator output 'direction' */

	/* only M1 (operator 0) is filled with this data: */
	signed int *mem_connect;            /* where to put the delayed sample (MEM) */
	INT32       mem_value;              /* delayed sample (MEM) value */

	/* channel specific data; note: each operator number 0 contains channel specific data */
	UINT32      fb_shift;               /* feedback shift value for operators 0 in each channel */
	INT32       fb_out_curr;            /* operator feedback value (used only by operators 0) */
	INT32       fb_out_prev;            /* previous feedback value (used only by operators 0) */
	UINT32      kc;                     /* channel KC (copied to all operators) */
	UINT32      kc_i;                   /* just for speedup */
	UINT32      pms;                    /* channel PMS */
	UINT32      ams;                    /* channel AMS */
	/* end of channel specific data */

	UINT32      AMmask;                 /* LFO Amplitude Modulation enable mask */
	UINT32      state;                  /* Envelope state: 4-attack(AR) 3-decay(D1R) 2-sustain(D2R) 1-release(RR) 0-off */
	UINT8       eg_sh_ar;               /*  (attack state) */
	UINT8       eg_sel_ar;              /*  (attack state) */
	UINT32      tl;                     /* Total attenuation Level */
	INT32       volume;                 /* current envelope attenuation level */
	UINT8       eg_sh_d1r;              /*  (decay state) */
	UINT8       eg_sel_d1r;             /*  (decay state) */
	UINT32      d1l;                    /* envelope switches to sustain state after reaching this level */
	UINT8       eg_sh_d2r;              /*  (sustain state) */
	UINT8       eg_sel_d2r;             /*  (sustain state) */
	UINT8       eg_sh_rr;               /*  (release state) */
	UINT8       eg_sel_rr;              /*  (release state) */

	UINT32      key;                    /* 0=last key was KEY OFF, 1=last key was KEY ON */

	UINT32      ks;                     /* key scale    */
	UINT32      ar;                     /* attack rate  */
	UINT32      d1r;                    /* decay rate   */
	UINT32      d2r;                    /* sustain rate */
	UINT32      rr;                     /* release rate */

	UINT32      reserved0;              /**/
	UINT32      reserved1;              /**/
};

struct ym2151 {
	struct ym2151_operator oper[32];    /* the 32 operators */

	UINT32      pan[16];                /* channels output masks (0xffffffff = enable) */
	UINT8       Muted[8];               /* used for muting */

	UINT32      eg_cnt;                 /* global envelope generator counter */
	UINT32      eg_timer;               /* global envelope generator counter works at frequency = chipclock/64/3 */
	UINT32      eg_timer_add;           /* step of eg_timer */
	UINT32      eg_timer_overflow;      /* envelope generator timer overlfows every 3 samples (on real chip) */

	UINT32      lfo_phase;              /* accumulated LFO phase (0 to 255) */
	UINT32      lfo_timer;              /* LFO timer                        */
	UINT32      lfo_timer_add;          /* step of lfo_timer                */
	UINT32      lfo_overflow;           /* LFO generates new output when lfo_timer reaches this value */
	UINT32      lfo_counter;            /* LFO phase increment counter      */
	UINT32      lfo_counter_add;        /* step of lfo_counter              */
	UINT8       lfo_wsel;               /* LFO waveform (0-saw, 1-square, 2-triangle, 3-random noise) */
	UINT8       amd;                    /* LFO Amplitude Modulation Depth   */
	INT8        pmd;                    /* LFO Phase Modulation Depth       */
	UINT32      lfa;                    /* LFO current AM output            */
	INT32       lfp;                    /* LFO current PM output            */

	UINT8       test;                   /* TEST register */
	UINT8       ct;                     /* output control pins (bit1-CT2, bit0-CT1) */

	UINT32      noise;                  /* noise enable/period register (bit 7 - noise enable, bits 4-0 - noise period */
	UINT32      noise_rng;              /* 17 bit noise shift register */
	UINT32      noise_p;                /* current noise 'phase'*/
	UINT32      noise_f;                /* current noise period */

	UINT32      csm_req;                /* CSM  KEY ON / KEY OFF sequence request */

	UINT32      irq_enable;             /* IRQ enable for timer B (bit 3) and timer A (bit 2); bit 7 - CSM mode (keyon to all slots, everytime timer A overflows) */
	UINT32      status;                 /* chip status (BUSY, IRQ Flags) */
	UINT8       connect[8];             /* channels connections */

#ifdef USE_MAME_TIMERS
	/* ASG 980324 -- added for tracking timers */
	emu_timer   *timer_A;
	emu_timer   *timer_B;
	attotime    timer_A_time[1024];     /* timer A times for MAME */
	attotime    timer_B_time[256];      /* timer B times for MAME */
	int         irqlinestate;
#else
	UINT8       tim_A;                  /* timer A enable (0-disabled) */
	UINT8       tim_B;                  /* timer B enable (0-disabled) */
	INT32       tim_A_val;              /* current value of timer A */
	INT32       tim_B_val;              /* current value of timer B */
	UINT32      tim_A_tab[1024];        /* timer A deltas */
	UINT32      tim_B_tab[256];         /* timer B deltas */
#endif
	UINT32      timer_A_index;          /* timer A index */
	UINT32      timer_B_index;          /* timer B index */
	UINT32      timer_A_index_old;      /* timer A previous index */
	UINT32      timer_B_index_old;      /* timer B previous index */

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
	UINT32      freq[11*768];           /* 11 octaves, 768 'cents' per octave */

	/*  Frequency deltas for DT1. These deltas alter operator frequency
	*   after it has been taken from frequency-deltas table.
	*/
	INT32       dt1_freq[8*32];         /* 8 DT1 levels, 32 KC values */

	UINT32      noise_tab[32];          /* 17bit Noise Generator periods */

	//void (*irqhandler)(const device_config *device, int irq);     /* IRQ function handler */
	//write8_device_func porthandler;       /* port write function handler */

	//const device_config *device;
	unsigned int clock;                 /* chip clock in Hz (passed from 2151intf.c) */
	unsigned int sampfreq;              /* sampling frequency in Hz (passed from 2151intf.c) */

	/* these variables stay here for speedup purposes only */
	signed int chanout[8];
	signed int m2,c1,c2; /* Phase Modulation input for operators 2,3,4 */
	signed int mem;		/* one sample delay memory */
};

/*
 * Initialize YM2151 emulator.
 *
 * 'clock' is the chip clock in Hz
 * 'rate' is sampling rate
 */
void ym2151_init(struct ym2151 *ym2151, int clock, int rate);

/*
 *  Allocate and initialize YM2151 emulator instance.
 *  @see ym2151_init
 */
struct ym2151 *ym2151_new(int clock, int rate);

/* shutdown the YM2151 emulators*/
void ym2151_shutdown(struct ym2151 *ym2151);

/* reset all chip registers for YM2151 number 'num'*/
void ym2151_reset_chip(struct ym2151 *ym2151);

/*
 * Generate samples
 *
 * '**buffers' is table of pointers to the buffers: left and right
 * 'length' is the number of samples that should be generated
 */
void ym2151_update_one(struct ym2151 *ym2151, SAMP **buffers, int length);

/* write 'v' to register 'r' on YM2151 */
void ym2151_write_reg(struct ym2151 *ym2151, int r, int v);

/* read status register on YM2151 */
int ym2151_read_status(struct ym2151 *ym2151);

/* set interrupt handler on YM2151 chip number 'n'*/
//void ym2151_set_irq_handler(void *chip, void (*handler)(int irq));

/* set port write handler on YM2151 chip number 'n'*/
//void ym2151_set_port_write_handler(void *chip, write8_device_func handler);

/* refresh chip when load state */
void ym2151_postload(struct ym2151 *ym2151);

void ym2151_set_mutemask(struct ym2151 *ym2151, UINT32 MuteMask);
