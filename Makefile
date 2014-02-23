CFLAGS=-ggdb

PROGS=mdxstat vgmtest vgmdump mdxdump mdx2mml pdx2wav pdx2sf2 mdx2vgm mdx2opm
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

$(PROGS): $$(sort $$@.o $$(patsubst %.cpp,%.o,$$($$@_SRCS)))
	$(CXX) $^ -o $@ $(CFLAGS) $(LIBS)

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CFLAGS)
%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f $(PROGS) $(addsuffix .exe,$(PROGS)) *.o

mdx2mml.o: mdx2mml.cpp MDXMML.h MDX.h exceptionf.h FileStream.h Buffer.h
mdx2vgm.o: mdx2vgm.cpp exceptionf.h MDXSerializer.h MDX.h FileStream.h Buffer.h VGMWriter.h VGM.h
mdxdump.o: mdxdump.cpp MDXDumper.h MDX.h exceptionf.h FileStream.h Buffer.h tools.h
mdxstat.o: mdxstat.cpp MDX.h exceptionf.h FileStream.h Buffer.h
pdx2sf2.o: pdx2sf2.cpp PDX.h FileStream.h exceptionf.h Buffer.h Soundfont.h tools.h
pdx2wav.o: pdx2wav.cpp PDX.h FileStream.h exceptionf.h Buffer.h WAVWriter.h tools.h
tools.o: tools.cpp tools.h
vgmdump.o: vgmdump.cpp VGM.h exceptionf.h FileStream.h Buffer.h
vgmtest.o: vgmtest.cpp VGMWriter.h VGM.h exceptionf.h FileStream.h Buffer.h
