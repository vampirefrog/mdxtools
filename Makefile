CFLAGS=-ggdb -Wall -DFIXED_POINT -DOUTSIDE_SPEEX -DRANDOM_PREFIX=speex -DEXPORT=
CC=gcc
YACC=bison
LEX=flex

PROGS=mdxinfo pdxinfo \
mdxplay \
mdx2vgm mdx2pcm pdx2wav mdx2mml mml2mdx \
adpcm-encode adpcm-decode
all: $(PROGS)

LIBS=-lz
ifneq (,$(findstring MINGW,$(shell uname -s)))
LIBS+=-liconv -lws2_32
endif

.SECONDEXPANSION:
mdxplay_SRCS=mdx.c pdx.c mdx_driver.c adpcm_driver.c adpcm.c mdx_player.c mdx_renderer.c timer.c tools.c sjis_unicode.c sjis.c ym2151.c okim6258.c cmdline.c resample.c
mdxplay_LIBS=-lao
mdx2pcm_SRCS=mdx.c mdx_driver.c adpcm_driver.c mdx_player.c mdx_renderer.c timer.c adpcm_driver.c adpcm.c pdx.c tools.c ym2151.c okim6258.c wav.c resample.c cmdline.c
mdx2vgm_SRCS=mdx.c mdx_driver.c adpcm_driver.c adpcm.c resample.c timer.c tools.c sjis_unicode.c sjis.c ym2151.c okim6258.c vgm.c
mdxinfo_SRCS=mdx.c tools.c sjis_unicode.c sjis.c cmdline.c md5.c
pdxinfo_SRCS=pdx.c tools.c cmdline.c md5.c adpcm.c
mdxdump_SRCS=mdx.c mdx_driver.c tools.c sjis_unicode.c sjis.c
pdx2wav_SRCS=pdx.c wav.c tools.c adpcm.c
mdx2mml_SRCS=mdx.c mml.c tools.c cmdline.c
mml2mdx_SRCS=mml2mdx.c mml.tab.c mml.yy.c buffer.c
adpcm-encode_SRCS=adpcm.c
adpcm-decode_SRCS=adpcm.c

OBJS=$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(foreach prog,$(PROGS),$(prog).cpp $($(prog)_SRCS))))

$(OBJS): Makefile

$(PROGS): $$(sort $$@.o $$(patsubst %.c,%.o,$$(patsubst %.cpp,%.o,$$($$@_SRCS))))
	$(CXX) $^ -o $@ $(CFLAGS) $($@_CFLAGS) $(LIBS) $($@_LIBS)

mml.tab.c: mml.y
	$(YACC) -v -o $@ $^ --defines=mml.tab.h

mml.yy.c: mml.l
	$(LEX) -o $@ $^

%.o: %.cpp
	$(CXX) -MMD -c $< -o $@ $(CFLAGS)
%.o: %.c
	$(CC) -MMD -c $< -o $@ $(CFLAGS)

-include $(OBJS:.o=.d)

clean:
	rm -f $(PROGS) $(addsuffix .exe,$(PROGS)) *.o *.d
