CFLAGS=-ggdb -Wall

PROGS=mdxstat vgmtest mdxdump mdx2mml pdx2wav pdx2sf2 mdx2vgm mdx2opm mdx2midi mididump midi2json mml2mdx
PLIBS=libmdx.so.0
all: $(PLIBS) $(PROGS)

ifneq (,$(findstring MINGW,$(shell uname -s)))
LIBS=-lz -liconv -lws2_32
else
LIBS=-lz
endif

.SECONDEXPANSION:
libmdx.so.0_SRCS=tools.cpp MDX.cpp MML.cpp Soundfont.c Stream.cpp

OBJS=$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(foreach prog,$(PROGS),$(prog).cpp $($(prog)_SRCS))))

$(OBJS): Makefile

$(PLIBS): $$(sort $$(patsubst %.c,%.o,$$(patsubst %.cpp,%.o,$$($$@_SRCS))))
	$(CXX) -shared $^ -o $@ $(CFLAGS) -s

$(PROGS): $$@.o $(PLIBS)
	$(CXX) $^ -o $@ $(CFLAGS) $(LIBS) -s

%.o: %.cpp
	$(CXX) -MMD -c $< -o $@ $(CFLAGS)
%.o: %.c
	$(CC) -MMD -c $< -o $@ $(CFLAGS)

-include $(OBJS:.o=.d)

clean:
	rm -f $(PLIBS) $(PROGS) $(addsuffix .exe,$(PROGS)) *.o *.d
