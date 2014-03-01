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

$(PROGS): $$(sort $$@.o $$(patsubst %.cpp,%.o,$$($$@_SRCS)))
	$(CXX) $^ -o $@ $(CFLAGS) $(LIBS)

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CFLAGS)
%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f $(PROGS) $(addsuffix .exe,$(PROGS)) *.o

mdx2midi.o: mdx2midi.cpp MDXMidi.h MDX.h exceptionf.h Stream.h FS.h
mdx2mml.o: mdx2mml.cpp MDXMML.h MDX.h exceptionf.h Stream.h FS.h
mdx2opm.o: mdx2opm.cpp MDX.h exceptionf.h Stream.h FS.h
mdx2vgm.o: mdx2vgm.cpp exceptionf.h MDXVGM.h MDXSerializer.h MDX.h Stream.h FS.h VGMWriter.h VGM.h PDX.h tools.h
mdxdump.o: mdxdump.cpp MDXDumper.h MDX.h exceptionf.h Stream.h FS.h tools.h
mdxstat.o: mdxstat.cpp MDX.h exceptionf.h Stream.h FS.h
mididump.o: mididump.cpp Midi.h FS.h Stream.h exceptionf.h
pdx2sf2.o: pdx2sf2.cpp PDX.h FS.h Stream.h exceptionf.h Soundfont.h ADPCMDecoder.h
pdx2wav.o: pdx2wav.cpp PDX.h FS.h Stream.h exceptionf.h WAVWriter.h ADPCMDecoder.h
testStream.o: testStream.cpp
tools.o: tools.cpp tools.h
vgmdump.o: vgmdump.cpp VGM.h exceptionf.h FS.h Stream.h
vgmtest.o: vgmtest.cpp VGMWriter.h VGM.h exceptionf.h FS.h Stream.h
xdf.o: xdf.cpp FS.h Stream.h exceptionf.h
Soundfont.o: Soundfont.c Soundfont.h
