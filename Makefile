CFLAGS=-ggdb -Wall

PROGS=mdxstat vgmtest mdxdump mdx2mml pdx2wav pdx2sf2 mdx2vgm mdx2opm mdx2midi mididump midi2json mml2mdx
all: $(PROGS)

ifneq (,$(findstring MINGW,$(shell uname -s)))
LIBS=-lz -liconv -lws2_32
else
LIBS=-lz
endif

.SECONDEXPANSION:
mdxdump_SRCS=tools.cpp MDX.cpp
pdx2wav_SRCS=tools.cpp
pdx2sf2_SRCS=tools.cpp Soundfont.c
mididump_SRCS=tools.cpp
midi2json_SRCS=tools.cpp
mdx2vgm_SRCS=tools.cpp MDX.cpp Stream.cpp
mdx2mml_SRCS=tools.cpp MDX.cpp
mdx2opm_SRCS=tools.cpp MDX.cpp
mdx2midi_SRCS=tools.cpp MDX.cpp Stream.cpp
mdxstat_SRCS=MDX.cpp
vgmtest_SRCS=Stream.cpp
mml2mdx_SRCS=tools.cpp MML.cpp mml2mdx.cpp Stream.cpp

OBJS=$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(foreach prog,$(PROGS),$(prog).cpp $($(prog)_SRCS))))

$(OBJS): Makefile

$(PROGS): $$(sort $$@.o $$(patsubst %.c,%.o,$$(patsubst %.cpp,%.o,$$($$@_SRCS))))
	$(CXX) $^ -o $@ $(CFLAGS) $(LIBS)

%.o: %.cpp
	$(CXX) -MMD -c $< -o $@ $(CFLAGS)
%.o: %.c
	$(CC) -MMD -c $< -o $@ $(CFLAGS)

-include $(OBJS:.o=.d)

clean:
	rm -f $(PROGS) $(addsuffix .exe,$(PROGS)) *.o *.d
