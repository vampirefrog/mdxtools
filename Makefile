CFLAGS=-ggdb -Wall -DFIXED_POINT -DOUTSIDE_SPEEX -DRANDOM_PREFIX=speex -DEXPORT= -D_GNU_SOURCE -DHAVE_MEMCPY -DSAMPLE_BITS=16 -Ix68ksjis
CC=gcc
YACC=bison
LEX=flex

#	mdx2vgm \

PROGS=\
	adpcm-decode \
	adpcm-encode \
	mdx2midi \
	mdx2mml \
	mdx2opm \
	mdxdump \
	mdxinfo \
	mdxplay \
	mididump \
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

all: $(PROGS)
tests: $(TESTS)

ifneq (,$(findstring MINGW,$(shell uname -s)))
CFLAGS+=-I../portaudio/include -static-libgcc
LIBS=-lz -liconv -lws2_32 -static-libgcc
else
CFLAGS+=$(shell pkg-config portaudio-2.0 --cflags)
LIBS=-lz
endif

.SECONDEXPANSION:
adpcm-decode_SRCS=adpcm.c
adpcm-encode_SRCS=adpcm.c
mdx2midi_SRCS=mdx.c buffer.c stream.c midi.c tools.c
mdx2mml_SRCS=mdx.c mdx_decompiler.c tools.c cmdline.c x68ksjis/sjis.c x68ksjis/sjis_unicode.c x68ksjis/utf8.c
mdx2opm_SRCS=mdx2opm.c tools.c mdx.c
mdx2pcm_SRCS=mdx.c mdx_driver.c adpcm_driver.c mdx_pcm_driver.c mdx_pcm_renderer.c timer.c adpcm_driver.c adpcm.c pdx.c tools.c ym2151.c okim6258.c wav.c speex_resampler.c cmdline.c
mdx2vgm_SRCS=mdx.c mdx_driver.c adpcm_driver.c adpcm.c speex_resampler.c timer.c tools.c x68ksjis/sjis_unicode.c x68ksjis/sjis.c ym2151.c okim6258.c vgm.c
mdxdump_SRCS=mdx.c tools.c
mdxinfo_SRCS=mdx.c tools.c x68ksjis/sjis_unicode.c x68ksjis/sjis.c x68ksjis/utf8.c cmdline.c md5.c
mdxplay_SRCS=mdx_driver.c timer_driver.c adpcm_driver.c fm_driver.c tools.c adpcm.c speex_resampler.c ym2151.c fixed_resampler.c mdx.c pdx.c wav.c cmdline.c adpcm_pcm_mix_driver.c fm_opm_emu_driver.c pcm_timer_driver.c fm_opm_driver.c
ifneq (,$(findstring MINGW,$(shell uname -s)))
mdxplay_LIBS=../portaudio/lib/.libs/libportaudio.dll.a -lwinmm
else
mdxplay_LIBS=$(shell pkg-config portaudio-2.0 --libs)
endif
mididump_SRCS=mididump.c midi.c midi_reader.c stream.c tools.c buffer.c
mml2mdx_SRCS=mml2mdx.c mmlc.tab.c mmlc.yy.c buffer.c cmdline.c tools.c
pdx2wav_SRCS=pdx.c wav.c tools.c adpcm.c
pdx2sf2_SRCS=pdx.c tools.c adpcm.c Soundfont.c
pdxinfo_SRCS=pdx.c tools.c cmdline.c md5.c adpcm.c
gensinc_SRCS=gensinc.c cmdline.c

# Tests
resample-test_SRCS=resample-test.c fixed_resampler.c
fm-driver-test_SRCS=tools.c ym2151.c wav.c fm_driver.c fm_opm_driver.c fm_opm_emu_driver.c mdx.c
adpcm-driver-test_SRCS=fixed_resampler.c tools.c adpcm.c speex_resampler.c okim6258.c wav.c adpcm_driver.c adpcm_pcm_mix_driver.c
timer-driver-test_SRCS=timer_driver.c tools.c pcm_timer_driver.c
mdx-driver-test_SRCS=mdx_driver.c timer_driver.c adpcm_driver.c fm_driver.c tools.c adpcm.c speex_resampler.c ym2151.c fixed_resampler.c mdx.c pdx.c wav.c adpcm_pcm_mix_driver.c fm_opm_emu_driver.c pcm_timer_driver.c fm_opm_driver.c

OBJS=$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(foreach prog,$(PROGS) $(TESTS),$(prog).cpp $($(prog)_SRCS))))

$(OBJS): Makefile

$(PROGS) $(TESTS): $$(sort $$@.o $$(patsubst %.c,%.o,$$(patsubst %.cpp,%.o,$$($$@_SRCS))))
	$(CXX) $^ -o $@ $(CFLAGS) $(LIBS) $($@_LIBS)

mmlc.tab.c: mmlc.y
	$(YACC) -v -o $@ $^ --defines=mmlc.tab.h

mmlc.yy.c: mmlc.l
	$(LEX) -o $@ $^

%.o: %.cpp
	$(CXX) -MMD -c $< -o $@ $(CFLAGS)
%.o: %.c
	$(CC) -MMD -c $< -o $@ $(CFLAGS)

sinctbl4.h: gensinc
	./gensinc --zero-crossings 26 --denominator 4 --alpha 5.0 > $@

sinctbl3.h: gensinc
	./gensinc --zero-crossings 26 --denominator 3 --alpha 5.0 > $@

-include $(OBJS:.o=.d)

clean:
	rm -f $(PROGS) $(TESTS) $(addsuffix .exe,$(PROGS) $(TESTS)) *.o *.d
