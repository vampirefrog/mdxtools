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

#include <stdint.h>
#include "mamedef.h"

/* 16- and 8-bit samples (signed) are supported*/
#define SAMPLE_BITS 16

typedef stream_sample_t SAMP;

/* struct describing a single operator */
struct ym2151_operator {
	uint32_t      phase;                  /* accumulated operator phase */
	uint32_t      freq;                   /* operator frequency count */
	int32_t       dt1;                    /* current DT1 (detune 1 phase inc/decrement) value */
	uint32_t      mul;                    /* frequency count multiply */
	uint32_t      dt1_i;                  /* DT1 index * 32 */
	uint32_t      dt2;                    /* current DT2 (detune 2) value */

	signed int *connect;                /* operator output 'direction' */

	/* only M1 (operator 0) is filled with this data: */
	signed int *mem_connect;            /* where to put the delayed sample (MEM) */
	int32_t       mem_value;              /* delayed sample (MEM) value */

	/* channel specific data; note: each operator number 0 contains channel specific data */
	uint32_t      fb_shift;               /* feedback shift value for operators 0 in each channel */
	int32_t       fb_out_curr;            /* operator feedback value (used only by operators 0) */
	int32_t       fb_out_prev;            /* previous feedback value (used only by operators 0) */
	uint32_t      kc;                     /* channel KC (copied to all operators) */
	uint32_t      kc_i;                   /* just for speedup */
	uint32_t      pms;                    /* channel PMS */
	uint32_t      ams;                    /* channel AMS */
	/* end of channel specific data */

	uint32_t      AMmask;                 /* LFO Amplitude Modulation enable mask */
	uint32_t      state;                  /* Envelope state: 4-attack(AR) 3-decay(D1R) 2-sustain(D2R) 1-release(RR) 0-off */
	uint8_t       eg_sh_ar;               /*  (attack state) */
	uint8_t       eg_sel_ar;              /*  (attack state) */
	uint32_t      tl;                     /* Total attenuation Level */
	int32_t       volume;                 /* current envelope attenuation level */
	uint8_t       eg_sh_d1r;              /*  (decay state) */
	uint8_t       eg_sel_d1r;             /*  (decay state) */
	uint32_t      d1l;                    /* envelope switches to sustain state after reaching this level */
	uint8_t       eg_sh_d2r;              /*  (sustain state) */
	uint8_t       eg_sel_d2r;             /*  (sustain state) */
	uint8_t       eg_sh_rr;               /*  (release state) */
	uint8_t       eg_sel_rr;              /*  (release state) */

	uint32_t      key;                    /* 0=last key was KEY OFF, 1=last key was KEY ON */

	uint32_t      ks;                     /* key scale    */
	uint32_t      ar;                     /* attack rate  */
	uint32_t      d1r;                    /* decay rate   */
	uint32_t      d2r;                    /* sustain rate */
	uint32_t      rr;                     /* release rate */

	uint32_t      reserved0;              /**/
	uint32_t      reserved1;              /**/
};

struct ym2151 {
	struct ym2151_operator oper[32];    /* the 32 operators */

	uint32_t      pan[16];                /* channels output masks (0xffffffff = enable) */
	uint8_t       Muted[8];               /* used for muting */

	uint32_t      eg_cnt;                 /* global envelope generator counter */
	uint32_t      eg_timer;               /* global envelope generator counter works at frequency = chipclock/64/3 */
	uint32_t      eg_timer_add;           /* step of eg_timer */
	uint32_t      eg_timer_overflow;      /* envelope generator timer overlfows every 3 samples (on real chip) */

	uint32_t      lfo_phase;              /* accumulated LFO phase (0 to 255) */
	uint32_t      lfo_timer;              /* LFO timer                        */
	uint32_t      lfo_timer_add;          /* step of lfo_timer                */
	uint32_t      lfo_overflow;           /* LFO generates new output when lfo_timer reaches this value */
	uint32_t      lfo_counter;            /* LFO phase increment counter      */
	uint32_t      lfo_counter_add;        /* step of lfo_counter              */
	uint8_t       lfo_wsel;               /* LFO waveform (0-saw, 1-square, 2-triangle, 3-random noise) */
	uint8_t       amd;                    /* LFO Amplitude Modulation Depth   */
	int8_t        pmd;                    /* LFO Phase Modulation Depth       */
	uint32_t      lfa;                    /* LFO current AM output            */
	int32_t       lfp;                    /* LFO current PM output            */

	uint8_t       test;                   /* TEST register */
	uint8_t       ct;                     /* output control pins (bit1-CT2, bit0-CT1) */

	uint32_t      noise;                  /* noise enable/period register (bit 7 - noise enable, bits 4-0 - noise period */
	uint32_t      noise_rng;              /* 17 bit noise shift register */
	uint32_t      noise_p;                /* current noise 'phase'*/
	uint32_t      noise_f;                /* current noise period */

	uint32_t      csm_req;                /* CSM  KEY ON / KEY OFF sequence request */

	uint32_t      irq_enable;             /* IRQ enable for timer B (bit 3) and timer A (bit 2); bit 7 - CSM mode (keyon to all slots, everytime timer A overflows) */
	uint32_t      status;                 /* chip status (BUSY, IRQ Flags) */
	uint8_t       connect[8];             /* channels connections */

#ifdef USE_MAME_TIMERS
	/* ASG 980324 -- added for tracking timers */
	emu_timer   *timer_A;
	emu_timer   *timer_B;
	attotime    timer_A_time[1024];     /* timer A times for MAME */
	attotime    timer_B_time[256];      /* timer B times for MAME */
	int         irqlinestate;
#else
	uint8_t       tim_A;                  /* timer A enable (0-disabled) */
	uint8_t       tim_B;                  /* timer B enable (0-disabled) */
	int32_t       tim_A_val;              /* current value of timer A */
	int32_t       tim_B_val;              /* current value of timer B */
	uint32_t      tim_A_tab[1024];        /* timer A deltas */
	uint32_t      tim_B_tab[256];         /* timer B deltas */
#endif
	uint32_t      timer_A_index;          /* timer A index */
	uint32_t      timer_B_index;          /* timer B index */
	uint32_t      timer_A_index_old;      /* timer A previous index */
	uint32_t      timer_B_index_old;      /* timer B previous index */

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
	uint32_t      freq[11*768];           /* 11 octaves, 768 'cents' per octave */

	/*  Frequency deltas for DT1. These deltas alter operator frequency
	*   after it has been taken from frequency-deltas table.
	*/
	int32_t       dt1_freq[8*32];         /* 8 DT1 levels, 32 KC values */

	uint32_t      noise_tab[32];          /* 17bit Noise Generator periods */

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

void ym2151_set_mutemask(struct ym2151 *ym2151, uint32_t MuteMask);
