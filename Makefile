PROGS=mdxstat vgmtest vgmdump mdxdump mdx2mml
CFLAGS=-ggdb
all: $(PROGS)

ifneq (,$(findstring MINGW,$(shell uname -s)))
LIBS=-lz -liconv -lws2_32
else
LIBS=-lz
endif

.SECONDEXPANSION:
mdxdump_SRCS=mdxdump.cpp tools.cpp

$(PROGS): $$(sort $$@.o $$(patsubst %.cpp,%.o,$$($$@_SRCS)))
	$(CXX) $^ -o $@ $(CFLAGS) $(LIBS)

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CFLAGS)

clean:
	rm -f $(PROGS) $(addsuffix .exe,$(PROGS)) *.o

mdx2mml.o: mdx2mml.cpp MDXMML.h MDX.h exceptionf.h FileStream.h Buffer.h
mdxdump.o: mdxdump.cpp MDXDumper.h MDX.h exceptionf.h FileStream.h Buffer.h tools.h
mdxstat.o: mdxstat.cpp MDX.h exceptionf.h FileStream.h Buffer.h
tools.o: tools.cpp tools.h
vgmdump.o: vgmdump.cpp VGM.h exceptionf.h FileStream.h Buffer.h
vgmtest.o: vgmtest.cpp VGMWriter.h VGM.h exceptionf.h FileStream.h Buffer.h
