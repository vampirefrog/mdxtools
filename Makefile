CFLAGS=-ggdb -Wall

PROGS=mdxstat vgmtest mdxdump mdx2mml pdx2wav pdx2sf2 mdx2vgm mdx2opm mdx2midi mididump
all: $(PROGS)

ifneq (,$(findstring MINGW,$(shell uname -s)))
LIBS=-lz -liconv -lws2_32
else
LIBS=-lz
endif

.SECONDEXPANSION:
mdxdump_SRCS=tools.cpp
pdx2wav_SRCS=tools.cpp
pdx2sf2_SRCS=tools.cpp Soundfont.c
mididump_SRCS=tools.cpp
mdx2vgm_SRCS=tools.cpp

OBJS=$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(foreach prog,$(PROGS),$(prog).cpp $($(prog)_SRCS))))

$(PROGS): $$(sort $$@.o $$(patsubst %.c,%.o,$$(patsubst %.cpp,%.o,$$($$@_SRCS))))
	$(CXX) $^ -o $@ $(CFLAGS) $(LIBS)

%.o: %.cpp
	$(CXX) -MMD -c $< -o $@ $(CFLAGS)
%.o: %.c
	$(CC) -MMD -c $< -o $@ $(CFLAGS)

-include $(OBJS:.o=.d)


clean:
	rm -f $(PROGS) $(addsuffix .exe,$(PROGS)) *.o
