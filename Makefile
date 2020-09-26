CFLAGS=-ggdb -Wall -DFIXED_POINT -DOUTSIDE_SPEEX -DRANDOM_PREFIX=speex -DEXPORT= -D_GNU_SOURCE -DHAVE_MEMCPY -DSAMPLE_BITS=16
CC=gcc
YACC=bison
LEX=flex

PROGS=\
	adpcm-decode \
	adpcm-encode \
	mdx2midi \
	mdx2mml \
	mdx2opm \
	mdx2pcm \
	mdx2vgm \
	mdxdump \
	mdxinfo \
	mdxplay \
	mididump \
	mkpdx \
	mml2mdx \
	pdx2wav \
	pdxinfo \
	gensinc \
	resample-test \
	adpcm-driver-test \
	fm-driver-test \
	timer-driver-test \
	mdx-driver-test

all: $(PROGS)

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
mdx2mml_SRCS=mdx.c mml.c tools.c cmdline.c sjis.c sjis_unicode.c
mdx2opm_SRCS=mdx2opm.c tools.c mdx.c
mdx2pcm_SRCS=mdx.c mdx_driver.c adpcm_driver.c mdx_pcm_driver.c mdx_pcm_renderer.c timer.c adpcm_driver.c adpcm.c pdx.c tools.c ym2151.c okim6258.c wav.c resample.c cmdline.c
mdx2vgm_SRCS=mdx.c mdx_driver.c adpcm_driver.c adpcm.c resample.c timer.c tools.c sjis_unicode.c sjis.c ym2151.c okim6258.c vgm.c
mdxdump_SRCS=mdx.c tools.c
mdxinfo_SRCS=mdx.c tools.c sjis_unicode.c sjis.c cmdline.c md5.c
mdxplay_SRCS=mdx.c pdx.c mdx_driver.c adpcm_driver.c adpcm.c mdx_pcm_driver.c mdx_pcm_renderer.c timer.c tools.c sjis_unicode.c sjis.c ym2151.c okim6258.c cmdline.c resample.c
ifneq (,$(findstring MINGW,$(shell uname -s)))
mdxplay_LIBS=../portaudio/lib/.libs/libportaudio.a -lwinmm
else
mdxplay_LIBS=$(shell pkg-config portaudio-2.0 --libs)
endif
mididump_SRCS=mididump.c midi.c midi_reader.c stream.c tools.c buffer.c
mml2mdx_SRCS=mml2mdx.c mmlc.tab.c mmlc.yy.c buffer.c cmdline.c tools.c
pdx2wav_SRCS=pdx.c wav.c tools.c adpcm.c
pdxinfo_SRCS=pdx.c tools.c cmdline.c md5.c adpcm.c
resample-test_SRCS=resample-test.c fixed_resampler.c
gensinc_SRCS=gensinc.c cmdline.c

fm-driver-test_SRCS=tools.c ym2151.c wav.c fm_driver.c mdx.c
adpcm-driver-test_SRCS=fixed_resampler.c tools.c adpcm.c resample.c okim6258.c wav.c adpcm_driver.c
timer-driver-test_SRCS=timer_driver.c tools.c
mdx-driver-test_SRCS=mdx_driver.c timer_driver.c adpcm_driver.c fm_driver.c tools.c adpcm.c resample.c ym2151.c fixed_resampler.c mdx.c pdx.c wav.c

OBJS=$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(foreach prog,$(PROGS),$(prog).cpp $($(prog)_SRCS))))

$(OBJS): Makefile

$(PROGS): $$(sort $$@.o $$(patsubst %.c,%.o,$$(patsubst %.cpp,%.o,$$($$@_SRCS))))
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
	rm -f $(PROGS) $(addsuffix .exe,$(PROGS)) *.o *.d
