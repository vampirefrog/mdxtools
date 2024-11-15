CC=gcc
YACC=bison
LEX=flex

CFLAGS=-ggdb -Wall -DFIXED_POINT -DOUTSIDE_SPEEX -DRANDOM_PREFIX=speex -DEXPORT= -D_GNU_SOURCE -DHAVE_MEMCPY -DSAMPLE_BITS=16 -Ix68ksjis $(shell pkg-config portaudio-2.0 sndfile --cflags)
LIBS=-lz $(shell pkg-config portaudio-2.0 sndfile --libs)
ifneq (,$(findstring MINGW,$(shell uname -s)))
LIBS+=-liconv -lws2_32 -static-libgcc
endif

PROGS=\
	adpcm-decode \
	adpcm-encode \
	mdx2midi \
	mdx2mml \
	mdx2opm \
	mdxdump \
	mdxinfo \
	mdxplay \
	mdx2pcm \
	mkpdx \
	mml2mdx \
	pdx2sf2 \
	pdx2wav \
	pdxinfo \
	gensinc

TESTS=\
	resample-test \
	adpcm-driver-test \
	fm-driver-test \
	timer-driver-test \
	mdx-driver-test

TARGETS=$(PROGS) $(TESTS)

all: $(PROGS)
tests: $(TESTS)

.SECONDEXPANSION:
adpcm-decode_SRCS=adpcm.c
adpcm-encode_SRCS=adpcm.c
mdx2midi_SRCS=mdx.c tools.c cmdline.c
mdx2mml_SRCS=mdx.c mdx_decompiler.c tools.c cmdline.c x68ksjis/sjis.c x68ksjis/sjis_unicode.c x68ksjis/utf8.c
mdx2opm_SRCS=mdx2opm.c tools.c mdx.c
mdx2vgm_SRCS=mdx.c mdx_driver.c adpcm_driver.c adpcm.c speex_resampler.c timer.c tools.c x68ksjis/sjis_unicode.c x68ksjis/sjis.c ym2151.c okim6258.c vgm.c
mdxdump_SRCS=mdx.c tools.c
mdxinfo_SRCS=mdx.c tools.c x68ksjis/sjis_unicode.c x68ksjis/sjis.c x68ksjis/utf8.c cmdline.c md5.c
mdxplay_SRCS=mdx_driver.c timer_driver.c adpcm_driver.c fm_driver.c tools.c adpcm.c speex_resampler.c ym2151.c fixed_resampler.c mdx.c pdx.c cmdline.c adpcm_pcm_mix_driver.c fm_opm_emu_driver.c pcm_timer_driver.c fm_opm_driver.c sinctbl4.h sinctbl3.h vgm_logger.c
mdxplay_LIBS=$(shell pkg-config portaudio-2.0 --libs)
mdx2pcm_SRCS=mdx_driver.c timer_driver.c adpcm_driver.c fm_driver.c tools.c adpcm.c speex_resampler.c ym2151.c fixed_resampler.c mdx.c pdx.c cmdline.c adpcm_pcm_mix_driver.c fm_opm_emu_driver.c pcm_timer_driver.c fm_opm_driver.c sinctbl4.h sinctbl3.h vgm_logger.c
mml2mdx_SRCS=mml2mdx.c mmlc.tab.c mmlc.yy.c cmdline.c tools.c mdx_compiler.c mmlc.yy.h mmlc.tab.h
pdx2wav_SRCS=pdx.c tools.c adpcm.c
pdx2sf2_SRCS=pdx.c tools.c adpcm.c Soundfont.c
pdxinfo_SRCS=pdx.c tools.c cmdline.c md5.c adpcm.c
gensinc_SRCS=gensinc.c cmdline.c

mdx2midi: midilib/libmidi.a

mdx2midi_LIBS=midilib/libmidi.a
mml2mdx_LIBS=midilib/libmidi.a

# Tests
resample-test_SRCS=resample-test.c fixed_resampler.c sinctbl4.h sinctbl3.h
fm-driver-test_SRCS=tools.c ym2151.c fm_driver.c fm_opm_driver.c fm_opm_emu_driver.c mdx.c vgm_logger.c vgm_logger.h
adpcm-driver-test_SRCS=fixed_resampler.c tools.c adpcm.c speex_resampler.c okim6258.c adpcm_driver.c adpcm_pcm_mix_driver.c
timer-driver-test_SRCS=timer_driver.c tools.c pcm_timer_driver.c
mdx-driver-test_SRCS=mdx_driver.c timer_driver.c adpcm_driver.c fm_driver.c tools.c adpcm.c speex_resampler.c ym2151.c fixed_resampler.c mdx.c pdx.c adpcm_pcm_mix_driver.c fm_opm_emu_driver.c pcm_timer_driver.c fm_opm_driver.c vgm_logger.c vgm_logger.h

adpcm_pcm_mix_driver.c resample-test.c: sinctbl3.h sinctbl4.h

CPPOBJS=$(patsubst %.cpp,%.o,$(filter %.cpp,$(TARGETS)))
COBJS=$(patsubst %.c,%.o,$(filter %.c,$(TARGETS)))
OBJS=$(CPPOBJS) $(COBJS)

$(OBJS): Makefile

$(TARGETS):$$(sort $$@.o $$(patsubst %.cpp,%.o,$$(patsubst %.c,%.o,$$($$@_SRCS))))
	$(CC) $(filter %.o, $^) -o $@ $(CFLAGS) $(LIBS) $($@_LIBS)

mmlc.tab.c mmlc.tab.h: mmlc.y
	$(YACC) -v -o mmlc.tab.c --defines=mmlc.tab.h $(filter %.y,$^)

mdx_compiler.o: mmlc.yy.h
mmlc.yy.h: mmlc.tab.h

mmlc.yy.c mmlc.yy.h: mmlc.l
	$(LEX) -o mmlc.yy.c --header-file=$(patsubst %.c,%.h,$@) $(filter %.l,$^)

$(COBJS): %.c
	$(CC) -MMD -c $< -o $@ $(CFLAGS)

sinctbl4.h: gensinc
	./gensinc --zero-crossings 26 --denominator 4 --alpha 5.0 > $@

sinctbl3.h: gensinc
	./gensinc --zero-crossings 26 --denominator 3 --alpha 5.0 > $@

midilib/libmidi.a:
	cd midilib && make CC=gcc libmidi.a

-include $(OBJS:.o=.d)

clean:
	rm -f $(TARGETS) $(addsuffix .exe,$(TARGETS)) *.o *.d
	cd midilib && make clean
