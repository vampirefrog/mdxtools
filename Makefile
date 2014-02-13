PROGS=mdxstat vgmtest
all: $(PROGS)

ifneq (,$(findstring MINGW,$(shell uname -s)))
LIBS=-lz -liconv -lws2_32
else
LIBS=-lz
endif

mdxstat: mdxstat.cpp MDX.h exceptionf.h FileStream.h
	$(CXX) $< -o $@ $(LIBS)

vgmtest: vgmtest.cpp VGMWriter.h VGM.h exceptionf.h FileStream.h Buffer.h
	$(CXX) $< -o $@ $(LIBS)

clean:
	rm -f $(PROGS) $(addsuffix .exe,$(PROGS)) *.o
