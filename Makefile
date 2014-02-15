PROGS=mdxstat vgmtest vgmdump mdxdump
CFLAGS=-ggdb
all: $(PROGS)

ifneq (,$(findstring MINGW,$(shell uname -s)))
LIBS=-lz -liconv -lws2_32
else
LIBS=-lz
endif

%: %.cpp
	$(CXX) $(CFLAGS) $< -o $@ $(LIBS)

clean:
	rm -f $(PROGS) $(addsuffix .exe,$(PROGS)) *.o

mdxstat: mdxstat.cpp MDX.h exceptionf.h FileStream.h
vgmtest: vgmtest.cpp VGMWriter.h VGM.h exceptionf.h FileStream.h Buffer.h
vgmdump: vgmdump.cpp VGM.h exceptionf.h FileStream.h Buffer.h
mdxdump: mdxdump.cpp MDXDumper.h MDX.h exceptionf.h FileStream.h Buffer.h
