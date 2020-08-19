CFLAGS=-ggdb -Wall -DFIXED_POINT -DOUTSIDE_SPEEX -DRANDOM_PREFIX=speex -DEXPORT= -D_GNU_SOURCE -DHAVE_MEMCPY
CC=gcc
YACC=bison
LEX=flex

PROGS=mdxinfo pdxinfo \
mdxplay \
mdx2vgm mdx2pcm pdx2wav mdx2mml mml2mdx mdx2opm mdxdump mdx2midi \
adpcm-encode adpcm-decode \
mididump
all: $(PROGS)

ifneq (,$(findstring MINGW,$(shell uname -s)))
CFLAGS+=-I../portaudio/include
LIBS=-lz -liconv -lws2_32
else
CFLAGS+=$(shell pkg-config portaudio-2.0 --cflags)
LIBS=-lz
endif

.SECONDEXPANSION:
mdxplay_SRCS=mdx.c pdx.c mdx_driver.c adpcm_driver.c adpcm.c mdx_player.c mdx_renderer.c timer.c tools.c sjis_unicode.c sjis.c ym2151.c okim6258.c cmdline.c resample.c
ifneq (,$(findstring MINGW,$(shell uname -s)))
mdxplay_LIBS=../portaudio/lib/.libs/libportaudio.a -lwinmm
else
mdxplay_LIBS=$(shell pkg-config portaudio-2.0 --libs)
endif
mdx2pcm_SRCS=mdx.c mdx_driver.c adpcm_driver.c mdx_player.c mdx_renderer.c timer.c adpcm_driver.c adpcm.c pdx.c tools.c ym2151.c okim6258.c wav.c resample.c cmdline.c
mdx2vgm_SRCS=mdx.c mdx_driver.c adpcm_driver.c adpcm.c resample.c timer.c tools.c sjis_unicode.c sjis.c ym2151.c okim6258.c vgm.c
mdxinfo_SRCS=mdx.c tools.c sjis_unicode.c sjis.c cmdline.c md5.c
pdxinfo_SRCS=pdx.c tools.c cmdline.c md5.c adpcm.c
mdxdump_SRCS=mdx.c tools.c
pdx2wav_SRCS=pdx.c wav.c tools.c adpcm.c
mdx2mml_SRCS=mdx.c mml.c tools.c cmdline.c sjis.c sjis_unicode.c
mml2mdx_SRCS=mml2mdx.c mmlc.tab.c mmlc.yy.c buffer.c
mdx2opm_SRCS=mdx2opm.c tools.c mdx.c
adpcm-encode_SRCS=adpcm.c
adpcm-decode_SRCS=adpcm.c
mididump_SRCS=mididump.c midi.c midi_reader.c stream.c tools.c buffer.c
mdx2midi_SRCS=mdx.c buffer.c stream.c midi.c tools.c

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

-include $(OBJS:.o=.d)

clean:
	rm -f $(PROGS) $(addsuffix .exe,$(PROGS)) *.o *.d
