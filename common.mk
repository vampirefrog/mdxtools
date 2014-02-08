# Common makefile. Makes it easy to set up projects, without the use of autotools

ifndef DEBUG
DEBUG=false
endif

RESETC    = $(shell tput sgr0)
COMPILING = $(shell tput bold; tput setaf 2)
COMPILERC = $(shell tput setaf 2)
LINKING   = $(shell tput bold; tput setaf 3; tput setab 4)
LINKERC   = $(shell tput setaf 3)
ERRORC    = $(shell tput bold; tput setaf 1)
SUCCESSC  = $(shell tput bold; tput setaf 2; tput setab 4)

.PHONY: all clean $(subdirs)

all: $(subdirs) $(libraries) $(programs) $(extra)
	@echo "$(SUCCESSC)All targets done!$(RESETC)"

##
## Subdir template
##
define subdir_template
$(1):
	@if [ -x $(1) ]; then \
	cd $(1) && $$(MAKE); \
	else echo "$$(ERRORC)Directory $(1) not found!$$(RESETC)"; \
	fi
endef

##
## Program template
##
define program_template

ifeq (,$$($(1)_SRCS))
$(1)_SRCS = $$(notdir $$(firstword $$(shell find -maxdepth 1 -name $(1).c))) $$(notdir $$(firstword $$(shell find -maxdepth 1 -name $(1).cpp)))
ifneq (,$$(EXTRA_SRCS))
$(1)_SRCS += $$(EXTRA_SRCS)
endif
ifneq (,$$($(1)_EXTRA_SRCS))
$(1)_SRCS += $$($(1)_EXTRA_SRCS)
endif
endif

$(1)_SRCS := $$(join $$(dir $$(realpath $$($(1)_SRCS))),$$(addprefix $(1)_, $$(notdir $$($(1)_SRCS))))
$(1)_CC_SRCS := $$(filter %.c,$$($(1)_SRCS))
$(1)_CXX_SRCS := $$(filter %.cpp,$$($(1)_SRCS))
$(1)_OBJS := $$($(1)_CC_SRCS:.c=_c.o) $$($(1)_CXX_SRCS:.cpp=_cpp.o)

$(1): $$(EXTRA_DEPS) $$($(1)_EXTRA_DEPS) $$($(1)_OBJS)
	@echo "$$(LINKING)Linking $$@$$(RESETC)"
	@if [ "x$$(DEBUG)" = "xtrue" ]; then echo "$$(LINKING)$$(CXX)$$(RESETC) $$(LINKERC)$$($(1)_OBJS) $$(LDFLAGS) $$($(1)_LDFLAGS) $$($(1)_LIBS:%=-l%) -o $$(LINKING)$$@$$(RESETC)"; fi
	@if ! $$(CXX) $$($(1)_OBJS) $$(LDFLAGS) $$($(1)_LDFLAGS) $$($(1)_LIBS:%=-l%) -o $$@; then \
	echo "$$(ERRORC)Linking failed for $$@ $$(RESETC)"; \
	exit 1; \
	fi

$(1)_%_cpp.o: %.cpp
	@if [ "x$$(DEBUG)" = "xtrue" ]; then echo "$$(COMPILING)$$(CXX)$$(RESETC) $$(COMPILERC)$$(CXXFLAGS) $$($(1)_CXXFLAGS) -MMD -c $$(COMPILING)$$<$$(RESETC)$$(COMPILERC) -o $$@$$(RESETC)"; \
	 else echo "$$(COMPILING)Compiling $$<$$(RESETC)"; fi
	@if $$(CXX) $$(CXXFLAGS) $$($(1)_CXXFLAGS) -MMD -c $$< -o $$@; then \
	 cp $$(dir $$*_cpp.d)$(1)_$$(notdir $$*_cpp.d) $$(dir $$*_cpp.d)$(1)_$$(notdir $$*_cpp.P); \
	 sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$$$//' -e '/^$$$$/ d' -e 's/$$$$/ :/' < $$(dir $$*_cpp.d)$(1)_$$(notdir $$*_cpp.d) >> $$(dir $$*_cpp.d)$(1)_$$(notdir $$*_cpp.P); \
	 rm -f $$(dir $$*_cpp.d)$(1)_$$(notdir $$*_cpp.d); \
	else \
	 echo "$$(ERRORC)Compilation failed for $$@: $$(RESETC)"; \
	 rm -f $$(dir $$*_cpp.d)$(1)_$$(notdir $$*_cpp.d); \
	 exit 1; \
	fi

$(1)_%_c.o: %.c
	@if [ "x$$(DEBUG)" = "xtrue" ]; then echo "$$(COMPILING)$$(CC)$$(RESETC) $$(COMPILERC)$$(CFLAGS) $$($(1)_CFLAGS) -MMD -c $$(COMPILING)$$<$$(RESETC)$$(COMPILERC) -o $$@$$(RESETC)"; \
	 else echo "$$(COMPILING)Compiling $$<$$(RESETC)"; fi
	@if $$(CC) $$(CFLAGS) $$($(1)_CFLAGS) -MMD -c $$< -o $$@; then \
	 cp $$(dir $$*_c.d)$(1)_$$(notdir $$*_c.d) $$(dir $$*_c.d)$(1)_$$(notdir $$*_c.P); \
	 sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$$$//' -e '/^$$$$/ d' -e 's/$$$$/ :/' < $$(dir $$*_c.d)$(1)_$$(notdir $$*_c.d) >> $$(dir $$*_c.d)$(1)_$$(notdir $$*_c.P); \
	 rm -f $$(dir $$*_c.d)$(1)_$$(notdir $$*_c.d); \
	else \
	 echo "$$(ERRORC)Compilation failed for $$@: $$(RESETC)"; \
	 rm -f $$(dir $$*_c.d)$(1)_$$(notdir $$*_c.d); \
	 exit 1; \
	fi

-include $$($(1)_OBJS:.o=.P)
endef

##
## Library template
##
define library_template
$(1)_OBJS = $$(addprefix $(1)_, $$($(1)_SRCS:.cpp=.o))

$(2): $$(EXTRA_DEPS) $$($(1)_EXTRA_DEPS) $$($(1)_OBJS)
	@echo "$$(LINKING)Archiving $$@$$(RESETC)"
	@if [ "x$$(DEBUG)" = "xtrue" ]; then echo "$$(LINKERC)ar crus $$@ $$($(1)_OBJS)$$(RESETC)"; fi
	@ar crus $$@ $$^

$(1)_%_cpp.o: %.cpp
	@if [ "x$$(DEBUG)" = "xtrue" ]; then echo "$$(COMPILING)$$(CXX)$$(RESETC) $$(COMPILERC)$$(CXXFLAGS) $$($(1)_CXXFLAGS) -MMD -c $$(COMPILING)$$<$$(RESETC)$$(COMPILERC) -o $$@$$(RESETC)"; \
	 else echo "$$(COMPILING)Compiling $$<$$(RESETC)"; fi
	@if $$(CXX) $$(CXXFLAGS) $$($(1)_CXXFLAGS) -MMD -c $$< -o $$@; then \
	 cp $$(dir $$*_cpp.d)$(1)_$$(notdir $$*_cpp.d) $$(dir $$*_cpp.d)$(1)_$$(notdir $$*_cpp.P); \
	 sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$$$//' -e '/^$$$$/ d' -e 's/$$$$/ :/' < $$(dir $$*_cpp.d)$(1)_$$(notdir $$*_cpp.d) >> $$(dir $$*_cpp.d)$(1)_$$(notdir $$*_cpp.P); \
	 rm -f $$(dir $$*_cpp.d)$(1)_$$(notdir $$*_cpp.d); \
	else \
	 echo "$$(ERRORC)Compilation failed for $$@: $$(RESETC)"; \
	 rm -f $$(dir $$*_cpp.d)$(1)_$$(notdir $$*_cpp.d); \
	 exit 1; \
	fi


$(1)_%_c.o: %.c
	@if [ "x$$(DEBUG)" = "xtrue" ]; then echo "$$(COMPILING)$$(CC)$$(RESETC) $$(COMPILERC)$$(CFLAGS) $$($(1)_CFLAGS) -MMD -c $$(COMPILING)$$<$$(RESETC)$$(COMPILERC) -o $$@$$(RESETC)"; \
	 else echo "$$(COMPILING)Compiling $$<$$(RESETC)"; fi
	@if $$(CC) $$(CFLAGS) $$($(1)_CFLAGS) -MMD -c $$< -o $$@; then \
	 cp $$(dir $$*_c.d)$(1)_$$(notdir $$*_c.d) $$(dir $$*_c.d)$(1)_$$(notdir $$*_c.P); \
	 sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$$$//' -e '/^$$$$/ d' -e 's/$$$$/ :/' < $$(dir $$*_c.d)$(1)_$$(notdir $$*_c.d) >> $$(dir $$*_c.d)$(1)_$$(notdir $$*_c.P); \
	 rm -f $$(dir $$*_c.d)$(1)_$$(notdir $$*_c.d); \
	else \
	 echo "$$(ERRORC)Compilation failed for $$@: $$(RESETC)"; \
	 rm -f $$(dir $$*_c.d)$(1)_$$(notdir $$*_c.d); \
	 exit 1; \
	fi

-include $$($(1)_OBJS:.o=.P)
endef

$(foreach subdir,$(subdirs),$(eval $(call subdir_template,$(subdir))))

$(foreach prog,$(programs),$(eval $(call program_template,$(prog))))

$(foreach lib,$(libraries),$(eval $(call library_template,$(subst .,_,$(lib)),$(lib))))

clean:
	@echo "$(ERRORC)Cleaning...$(RESETC)"
	rm -f *.[aodP] *~ $(programs)
	$(foreach prog,$(programs), rm -f $(addsuffix *.[aodP],$(sort $(dir $($(prog)_SRCS)))))
	@if [ "x$(subdirs)" != "x" ]; then \
	 for d in $(subdirs); do \
	  (cd $$d && $(MAKE) clean); \
	 done; \
	fi
