#All objects that are used
OBJECTS            =

#Redefine default definitions using autoconf
-include ../config

# Objects that require wxWidgets CPPFLAGS to compile
WX_OBJECTS         = xsim.o xsimbase.o xsimmain.o xsimtrace.o xsimtracedll.o

# All programs depend on the ATerm library
CPPFLAGS += $(ATERM_CPPFLAGS) 
LDFLAGS  += $(ATERM_LDFLAGS) 

#ATerm definitions
ATERM_NAME         = aterm/libATerm.a
ATERM_OBJECTS      = aterm/afun.o aterm/aterm.o aterm/bafio.o aterm/byteio.o aterm/gc.o aterm/hash.o aterm/list.o aterm/make.o aterm/md5c.o aterm/memory.o aterm/tafio.o aterm/version.o
ifdef BUILD_ATERM
OBJECTS           += $(ATERM_OBJECTS)
ATERM_DEP          = $(ATERM_NAME)
endif

#SVC definitions
LIBSVC_NAME        = svc/libsvc.a
LIBSVC_OBJECTS     = svc/blocklist.o svc/code.o svc/compress.o svc/hashtable.o svc/huffman.o svc/lz.o svc/ptable.o svc/svc1.o svc/svc2.o 
OBJECTS           += $(LIBSVC_OBJECTS)

#svc2fsm definitions
SVC2FSM_NAME       = svc2fsm
SVC2FSM_OBJECTS    = svc2fsm.o libprint_c.o libprint.o libstruct.o liblowlevel.o
OBJECTS           += $(SVC2FSM_OBJECTS)
SVC2FSM_LDFLAGS    = -lrewrite -lsvc -l$(ATERM) -lm $(LIBDL)

#rewrite library definitions
LIBREWR_NAME       = librewrite.a
LIBREWR_OBJECTS    = librewrite_c.o librewrite.o rewr_inner.o rewr_jitty.o rewr_innerc.o rewr_jittyc.o
OBJECTS           += $(LIBREWR_OBJECTS)
LIBREWR_LDFLAGS    = -l$(ATERM) -lm

#enum library definitions
LIBENUM_NAME       = libenum.a
LIBENUM_OBJECTS    = libenum.o enum_standard.o
OBJECTS           += $(LIBENUM_OBJECTS)
LIBENUM_LDFLAGS    = -l$(ATERM) -lm

#enum library definitions
LIBNEXT_NAME       = libnextstate.a
LIBNEXT_OBJECTS    = libnextstate.o nextstate_standard.o
OBJECTS           += $(LIBNEXT_OBJECTS)
LIBNEXT_LDFLAGS    = -l$(ATERM) -lm

#mcrl22lpe definitions
MCRL22LPE_NAME     = mcrl22lpe
MCRL22LPE_OBJECTS  = mcrl22lpe.o lin_std.o lin_alt.o libparse.o mcrl2parser.o mcrl2lexer.o typecheck.o dataimpl.o libprint_c.o libstruct.o liblowlevel.o libalpha.o
OBJECTS           += $(MCRL22LPE_OBJECTS)
MCRL22LPE_LDFLAGS  = -lrewrite -l$(ATERM) -lm $(LIBDL)

#lpepp definitions
LPEPP_NAME         = lpepp
LPEPP_OBJECTS      = lpepp.o libprint_c.o libstruct.o liblowlevel.o
OBJECTS           += $(LPEPP_OBJECTS)
LPEPP_LDFLAGS      = -lrewrite -l$(ATERM) -lm $(LIBDL)

#tracepp definitions
TRACEPP_NAME       = tracepp
TRACEPP_OBJECTS    = tracepp.o libtrace.o libprint.o libprint_c.o libstruct.o liblowlevel.o
OBJECTS           += $(TRACEPP_OBJECTS)
TRACEPP_LDFLAGS    = -lrewrite -l$(ATERM) -lm $(LIBDL)

#sim definitions
SIM_NAME           = sim
SIM_OBJECTS        = sim.o libprint_c.o libstruct.o liblowlevel.o
OBJECTS           += $(SIM_OBJECTS)
SIM_LDFLAGS        = -lnextstate -lenum -lrewrite -l$(ATERM) -lm $(LIBDL)

#xsim definitions
XSIM_NAME          = xsim
XSIM_OBJECTS       = xsim.o xsimbase.o xsimmain.o xsimtrace.o libtrace.o libprint.o libprint_c.o libstruct.o liblowlevel.o
ifdef USE_WX
OBJECTS           += $(XSIM_OBJECTS)
endif
XSIM_LDFLAGS       = -lnextstate -lenum -lrewrite -l$(ATERM) -lm $($LIBDL) $(WX_LDFLAGS)

#xsimtrace library definitions
XSIMTRACE_NAME     = xsimtrace.so
XSIMTRACE_OBJECTS  = xsimtracedll.o
ifdef USE_WX
ifneq ($(HOST_OS),MINGW)
OBJECTS           += $(XSIMTRACE_OBJECTS)
endif
endif
ifeq ($(HOST_OS),MACOSX)
  XSIMTRACE_LDFLAGS = -bundle -undefined dynamic_lookup
else
  XSIMTRACE_LDFLAGS = -shared
endif

ifdef USE_BOOST
 #lts library definitions
 LIBLTS_NAME        = liblts.a
 LIBLTS_OBJECTS     = liblts.o liblts_fsm.o liblts_dot.o liblts_red.o
 OBJECTS           += $(LIBLTS_OBJECTS)
 LIBLTS_LDFLAGS     = -l$(ATERM) -lm

 #ltsconvert definitions
 LTSCONVERT_NAME    = ltsconvert
 LTSCONVERT_OBJECTS = ltsconvert.o libprint.o libprint_c.o libstruct.o liblowlevel.o
 OBJECTS           += $(LTSCONVERT_OBJECTS)
 LTSCONVERT_LDFLAGS = -llts -lrewrite -lsvc -l$(ATERM) -lm $(LIBDL)

 #ltsinfo definitions
 LTSINFO_NAME       = ltsinfo
 LTSINFO_OBJECTS    = ltsinfo.o libprint.o libprint_c.o libstruct.o liblowlevel.o
 OBJECTS           += $(LTSINFO_OBJECTS)
 LTSINFO_LDFLAGS    = -llts -lrewrite -lsvc -l$(ATERM) -lm $(LIBDL)

 #lpconstelm definitions
 LPECONSTELM_NAME    = lpeconstelm
 LPECONSTELM_OBJECTS = libprint.o libstruct.o libprint_c.o liblowlevel.o lpeconstelm.o
 OBJECTS            += $(LPECONSTELM_OBJECTS)
 LPECONSTELM_LDFLAGS = -lrewrite -l$(ATERM) -lm $(LIBDL)

 #lpeparelm definitions
 LPEPARELM_NAME      = lpeparelm
 LPEPARELM_OBJECTS   = libprint.o libstruct.o libprint_c.o liblowlevel.o lpeparelm.o
 OBJECTS            += $(LPEPARELM_OBJECTS)
 LPEPARELM_LDFLAGS   = -lrewrite -l$(ATERM) -lm $(LIBDL)

 #lpedataelm definitions
 LPEDATAELM_NAME     = lpedataelm
 LPEDATAELM_OBJECTS  = libprint.o libstruct.o libprint_c.o liblowlevel.o lpedataelm.o libdataelm.o
 OBJECTS            += $(LPEDATAELM_OBJECTS)
 LPEDATAELM_LDFLAGS  = -lrewrite -l$(ATERM) -lm $(LIBDL)

 #lpe2lts definitions
 LPE2LTS_NAME       = lpe2lts
 LPE2LTS_OBJECTS    = lpe2lts.o libtrace.o libdataelm.o libprint.o libprint_c.o libstruct.o liblowlevel.o
 OBJECTS           += $(LPE2LTS_OBJECTS)
 LPE2LTS_LDFLAGS    = -lnextstate -lenum -lrewrite -lsvc -l$(ATERM) -lm $(LIBDL)

 #lpe2lts definitions
 LPE2TORX_NAME      = lpe2torx
 LPE2TORX_OBJECTS   = lpe2torx.o libdataelm.o libprint.o libprint_c.o libstruct.o liblowlevel.o
 OBJECTS           += $(LPE2TORX_OBJECTS)
 LPE2TORX_LDFLAGS   = -lnextstate -lenum -lrewrite -l$(ATERM) -lm $(LIBDL)
endif

#lperewr definitions
LPEREWR_NAME       = lperewr
LPEREWR_OBJECTS    = lperewr.o libprint_c.o libstruct.o liblowlevel.o
OBJECTS           += $(LPEREWR_OBJECTS)
LPEREWR_LDFLAGS    = -lrewrite -l$(ATERM) -lm $(LIBDL)

#tbf2lpe definitions
TBF2LPE_NAME       = tbf2lpe
TBF2LPE_OBJECTS    = tbf2lpe.o lpetrans.o libprint_c.o libstruct.o liblowlevel.o
OBJECTS           += $(TBF2LPE_OBJECTS)
TBF2LPE_LDFLAGS    = -lrewrite -l$(ATERM) -lm $(LIBDL)

#pnml2mcrl2 definitions
PNML2MCRL2_NAME    = pnml2mcrl2
PNML2MCRL2_OBJECTS = pnml2mcrl2.o typecheck.o libprint_c.o libstruct.o liblowlevel.o
ifdef USE_LIBXML2
OBJECTS           += $(PNML2MCRL2_OBJECTS)
endif
PNML2MCRL2_LDFLAGS = -lrewrite -l$(ATERM) $(LIBXML2_LDFLAGS) $(LIBDL)
$(PNML2MCRL2_OBJECTS) $(patsubst %.o,%.d,$(PNML2MCRL2_OBJECTS)): CPPFLAGS += $(LIBXML2_CPPFLAGS)

#ltsmin definitions
LTSMIN_NAME        = ltsmin
LTSMIN_OBJECTS     = ltsmin.o ltsmindata.o ltsminkernel.o libprint_c.o libstruct.o liblowlevel.o
OBJECTS           += $(LTSMIN_OBJECTS)
LTSMIN_LDFLAGS     = -lrewrite -lsvc -l$(ATERM) -lm $(LIBDL)

#alpha definitions
ALPHA_NAME         = alpha
ALPHA_OBJECTS      = alpha.o libalpha.o mcrl2parser.o libparse.o mcrl2lexer.o typecheck.o libprint_c.o libstruct.o liblowlevel.o
OBJECTS           += $(ALPHA_OBJECTS)
ALPHA_LDFLAGS      = -lrewrite -l$(ATERM) -lm $(LIBDL)

#lin definitions
LIN_NAME           = lin
LIN_OBJECTS        = lin.o lin_alt.o mcrl2parser.o libparse.o mcrl2lexer.o typecheck.o dataimpl.o libprint_c.o libstruct.o liblowlevel.o
OBJECTS           += $(LIN_OBJECTS)
LIN_LDFLAGS        = -lrewrite -l$(ATERM) -lm $(LIBDL)

#Group definitions
EXPERIMENTALS      = $(LIN_NAME) $(ALPHA_NAME)
EXE_NAMES          = $(MCRL22LPE_NAME) $(LPEDATAELM_NAME) $(LPEPARELM_NAME) $(LPECONSTELM_NAME) $(LPEPP_NAME) $(TRACEPP_NAME) $(LPE2LTS_NAME) $(LPE2TORX_NAME) $(SIM_NAME) $(XSIM_NAME) $(TBF2LPE_NAME) $(PNML2MCRL2_NAME) $(LPEREWR_NAME) $(LTSMIN_NAME) $(LTSCONVERT_NAME) $(LTSINFO_NAME) $(SVC2FSM_NAME)
LIB_NAMES          = $(XSIMTRACE_NAME)
OBJECTS           := $(sort $(OBJECTS))

#Make rules
.PHONY: all install clean distclean boost boost_relay documentation squadt ltsview experimental
.PRECIOUS: mcrl2parser.cpp mcrl2lexer.cpp

all: $(EXE_NAMES) $(LIB_NAMES) $(BUILD_EXPERIMENTAL)

# Rules for building the maximum_revision tool and to generate a revision file
-include ../utility/revision.mk

experimental: $(EXPERIMENTALS)

# Build bjam and local boost 
ifdef BUILD_BOOST
all : boost
distclean install: boost_relay

../boost/build_completed:
	@$(MAKE) -C ../boost
endif

# Build/clean/install documentation
ifdef USE_XMLTO
all install: documentation
endif

# Build/clean/install libxml2 dependent targets
ifdef BUILD_EXPERIMENTAL
ifdef USE_WX
all install: ltsview
ifdef USE_LIBXML2
all install: squadt
endif
endif
endif

ifdef BUILD_ATERM
install: aterm_install
endif

clean distclean: squadt boost_relay ltsview documentation

# For building documentation
documentation: $(CONFIG)
	@$(MAKE) -C doc $(MAKECMDGOALS)

boost_relay:
	@$(MAKE) -C ../boost $(MAKECMDGOALS)

ltsview: $(CONFIG) 
	@$(MAKE) -C ltsview $(MAKECMDGOALS)

# For building squadt
squadt: boost $(CONFIG)
	@$(MAKE) -C squadt $(MAKECMDGOALS)

# For building bjam and boost libraries
ifeq (,$(findstring $(MAKECMDGOALS),clean distclean revision))
boost: $(BUILD_BOOST)
else
boost:
endif

install: all
	$(INSTALL) -d $(bindir)
	@for f in $(EXE_NAMES); do\
	  if test -f $$f; then\
	    c="$(INSTALL) $$f $(bindir)";\
	    echo $$c;\
	    $$c;\
	  fi;\
	done
	$(INSTALL) -d $(libdir)
	@for f in $(LIB_NAMES); do\
	  if test -f $$f; then\
	    c="$(INSTALL) $$f $(libdir)";\
	    echo $$c;\
	    $$c;\
	  fi;\
	done
ifdef USE_WX
ifeq ($(HOST_OS),MACOSX)
	/Developer/Tools/Rez -t APPL -o xsim sample.r
	/Developer/Tools/SetFile -a C xsim   
	mkdir -p ../xsim.app/Contents
	mkdir -p ../xsim.app/Contents/MacOS
	mkdir -p ../xsim.app/Contents/Resources
	cp xsim.plist ../xsim.app/Contents/Info.plist
	echo -n "APPL????" > ../xsim.app/Contents/PkgInfo
	cp xsim $(bindir)/xsim
	ln -f $(bindir)/xsim ../xsim.app/Contents/MacOS/xsim
	cp -f xsim.icns ../xsim.app/Contents/Resources/xsim.icns
endif
endif

aterm_install:
	install -d $(includedir)/aterm
	install -m 444 aterm/*.h $(includedir)/aterm
	install -d $(libdir)/aterm
	install -m 444 aterm/libATerm.a $(libdir)/aterm

clean:
	$(RM) $(EXE_NAMES) $(EXPERIMENTALS) $(LIB_NAMES) *.o *.a *.d* *.dpp* lpe/*.o lpe/*.dpp aterm/*.o aterm/*.a aterm/*.d svc/*.o svc/*.a svc/*.d svc/*.dpp mcrl2lexer.cpp mcrl2parser.cpp mcrl2parser.hpp mcrl2parser.output mcrl2de_lexer.cpp mcrl2de_parser.cpp mcrl2de_parser.hpp mcrl2de_parser.output autom4te.cache *~ core core.* utility/maximum_revision mcrl2lexer.c mcrl2parser.c mcrl2parser.h

distclean: clean

mcrl2lexer.o: mcrl2parser.cpp

$(ATERM_NAME): $(ATERM_OBJECTS)
	$(AR) $(ARFLAGS) $@ $(ATERM_OBJECTS)

$(LIBSVC_NAME): $(LIBSVC_OBJECTS)
	$(AR) $(ARFLAGS) $@ $(LIBSVC_OBJECTS)

$(LIBREWR_NAME) : $(LIBREWR_OBJECTS)
	$(AR) $(ARFLAGS) $@ $(LIBREWR_OBJECTS)
#	$(CC) -shared -o $@ $(LIBREWR_OBJECTS)

$(LIBENUM_NAME) : $(LIBENUM_OBJECTS)
	$(AR) $(ARFLAGS) $@ $(LIBENUM_OBJECTS)

$(LIBNEXT_NAME) : $(LIBNEXT_OBJECTS)
	$(AR) $(ARFLAGS) $@ $(LIBNEXT_OBJECTS)

$(MCRL22LPE_NAME) : $(MCRL22LPE_OBJECTS) $(ATERM_DEP) $(LIBREWR_NAME)
	$(CXX) -o $@ $(MCRL22LPE_OBJECTS) $(LDFLAGS) $(MCRL22LPE_LDFLAGS)

$(LPEPP_NAME) : $(LPEPP_OBJECTS) $(ATERM_DEP) $(LIBREWR_NAME)
	$(CXX) -o $@ $(LPEPP_OBJECTS) $(LDFLAGS) $(LPEPP_LDFLAGS)

$(TRACEPP_NAME) : $(TRACEPP_OBJECTS) $(ATERM_DEP) $(LIBREWR_NAME)
	$(CXX) -o $@ $(TRACEPP_OBJECTS) $(LDFLAGS) $(TRACEPP_LDFLAGS)

$(SIM_NAME) : $(SIM_OBJECTS) $(ATERM_DEP) $(LIBREWR_NAME) $(LIBENUM_NAME) $(LIBNEXT_NAME)
	$(CXX) -o $@ $(SIM_OBJECTS) $(LDFLAGS) $(SIM_LDFLAGS)

ifdef USE_WX
$(WX_OBJECTS) $(patsubst %o,%dpp,$(WX_OBJECTS)): CPPFLAGS += $(WX_CPPFLAGS)
xsimtracedll.o: CXXFLAGS += -fPIC

$(XSIM_NAME): $(XSIM_OBJECTS) $(ATERM_DEP) $(LIBREWR_NAME) $(LIBENUM_NAME) $(LIBNEXT_NAME)
	$(CXX) -o $@ $(XSIM_OBJECTS) $(LDFLAGS) $(XSIM_LDFLAGS)

$(XSIMTRACE_NAME): $(XSIMTRACE_OBJECTS) $(ATERM_DEP) $(LIBREWR_NAME)
ifneq ($(HOST_OS),MINGW)
	$(CXX) -o $@ $(XSIMTRACE_OBJECTS) $(LDFLAGS) $(XSIMTRACE_LDFLAGS)
endif
else
$(XSIM_NAME):
	@echo cannot make target $@ because the wxWidgets library cannot be used

$(XSIMTRACE_NAME):
	@echo cannot make target $@ because the wxWidgets library cannot be used
endif

svc/%: CPPFLAGS += -I. -Iaterm

# Targets that depend on any of the boost libraries
ifdef USE_BOOST
lpe% libdataelm.% liblts% ltsconvert.% ltsinfo.%: CPPFLAGS += $(BOOST_CPPFLAGS) -I. -Iaterm
lpe% libdataelm.% liblts% ltsconvert.% ltsinfo.%: CXXFLAGS += -ftemplate-depth-256

$(LIBLTS_NAME): $(LIBLTS_OBJECTS)
	$(AR) $(ARFLAGS) $@ $(LIBLTS_OBJECTS)

$(LTSCONVERT_NAME) : $(LTSCONVERT_OBJECTS) $(ATERM_DEP) $(LIBSVC_NAME) $(LIBLTS_NAME) $(LIBREWR_NAME)
	$(CXX) -o $@ $(LTSCONVERT_OBJECTS) $(BOOST_LDFLAGS) $(LDFLAGS) $(LTSCONVERT_LDFLAGS)

$(LTSINFO_NAME) : $(LTSINFO_OBJECTS) $(ATERM_DEP) $(LIBSVC_NAME) $(LIBLTS_NAME) $(LIBREWR_NAME)
	$(CXX) -o $@ $(LTSINFO_OBJECTS) $(BOOST_LDFLAGS) $(LDFLAGS) $(LTSINFO_LDFLAGS)

$(LPECONSTELM_NAME) : $(LPECONSTELM_OBJECTS) $(ATERM_DEP) $(LIBREWR_NAME) $(BUILD_BOOST)
	$(CXX) -o $@ $(LPECONSTELM_OBJECTS) $(BOOST_LDFLAGS) $(BOOST_PROGRAM_OPTIONS) $(LDFLAGS) $(LPECONSTELM_LDFLAGS)

$(LPEPARELM_NAME) : $(LPEPARELM_OBJECTS) $(ATERM_DEP) $(LIBREWR_NAME) $(BUILD_BOOST)
	$(CXX) -o $@ $(LPEPARELM_OBJECTS) $(BOOST_LDFLAGS) $(BOOST_PROGRAM_OPTIONS) $(LDFLAGS) $(LPEPARELM_LDFLAGS)

$(LPEDATAELM_NAME) : $(LPEDATAELM_OBJECTS) $(ATERM_DEP) $(LIBREWR_NAME) $(BUILD_BOOST)
	$(CXX) -o $@ $(LPEDATAELM_OBJECTS) $(BOOST_LDFLAGS) $(BOOST_PROGRAM_OPTIONS) $(LDFLAGS) $(LPEDATAELM_LDFLAGS)

$(LPE2LTS_NAME) : $(LPE2LTS_OBJECTS) $(ATERM_DEP) $(LIBSVC_NAME) $(LIBREWR_NAME) $(LIBENUM_NAME) $(LIBNEXT_NAME)
	$(CXX) -o $@ $(LPE2LTS_OBJECTS) $(LDFLAGS) $(LPE2LTS_LDFLAGS)

$(LPE2TORX_NAME) : $(LPE2TORX_OBJECTS) $(ATERM_DEP) $(LIBREWR_NAME) $(LIBENUM_NAME) $(LIBNEXT_NAME)
	$(CXX) -o $@ $(LPE2TORX_OBJECTS) $(LDFLAGS) $(LPE2TORX_LDFLAGS)
else
$(LIBLTS_NAME) :
	@echo "Missing Boost libraries so skipping build of liblts."
$(LTSCONVERT_NAME) :
	@echo "Missing Boost libraries so skipping build of ltsconvert."
$(LTSINFO_NAME) :
	@echo "Missing Boost libraries so skipping build of ltsinfo."
$(LPECONSTELM_NAME) :
	@echo "Missing Boost libraries so skipping build of lpeconstelm."
$(LPEPARELM_NAME) :
	@echo "Missing Boost libraries so skipping build of lpeparelm."
$(LPEDATAELM_NAME) :
	@echo "Missing Boost libraries so skipping build of lpedataelm."
$(LPE2LTS_NAME) :
	@echo "Missing Boost libraries so skipping build of lpe2lts."
$(LPE2TORX_NAME) :
	@echo "Missing Boost libraries so skipping build of lpe2torx."
endif

$(SVC2FSM_NAME) : $(SVC2FSM_OBJECTS) $(ATERM_DEP) $(LIBSVC_NAME) $(LIBREWR_NAME)
	$(CXX) -o $@ $(SVC2FSM_OBJECTS) $(LDFLAGS) $(SVC2FSM_LDFLAGS)

$(LPEREWR_NAME) : $(LPEREWR_OBJECTS) $(ATERM_DEP) $(LIBREWR_NAME)
	$(CXX) -o $@ $(LPEREWR_OBJECTS) $(LDFLAGS) $(LPEREWR_LDFLAGS)

$(TBF2LPE_NAME) : $(TBF2LPE_OBJECTS) $(ATERM_DEP) $(LIBREWR_NAME)
	$(CXX) -o $@ $(TBF2LPE_OBJECTS) $(LDFLAGS) $(TBF2LPE_LDFLAGS)

ifdef USE_LIBXML2
$(patsubst %.o, %.dpp, $(PNML2MCRL2_OBJECTS)): CPPFLAGS+=$(LIBXML2_CPPFLAGS)

$(PNML2MCRL2_NAME) : $(PNML2MCRL2_OBJECTS) $(ATERM_DEP) $(LIBREWR_NAME)
	$(CXX) -o $@ $(PNML2MCRL2_OBJECTS) $(LDFLAGS) $(PNML2MCRL2_LDFLAGS)
else
$(PNML2MCRL2_NAME) :
	@echo cannot make target $@ because the xml2 library cannot be used
endif

$(LTSMIN_NAME) : $(LTSMIN_OBJECTS) $(ATERM_DEP) $(LIBSVC_NAME) $(LIBREWR_NAME)
	$(CXX) -o $@ $(LTSMIN_OBJECTS) $(LDFLAGS) $(LTSMIN_LDFLAGS)

$(ALPHA_NAME) : $(ALPHA_OBJECTS) $(ATERM_DEP) $(LIBREWR_NAME)
	$(CXX) -o $@ $(ALPHA_OBJECTS) $(LDFLAGS) $(ALPHA_LDFLAGS)

$(LIN_NAME) : $(LIN_OBJECTS) $(ATERM_DEP) $(LIBREWR_NAME)
	$(CXX) -o $@ $(LIN_OBJECTS) $(LDFLAGS) $(LIN_LDFLAGS)

%.cpp : %.y
	$(BISON) $(BFLAGS) -o $@ $<

%.cpp : %.l
	$(FLEX) $(LFLAGS) -o$@ $<

# *.cpp is needed for generated code (by flex and bison)
*.cpp *.o aterm/*.o svc/*.o: Makefile $(CONFIG)

%.d : %.c $(REVISION_HEADER)
	@echo generating dependencies for $<
	@$(CC) -MT '$*.o' -MM $(CPPFLAGS) $(ATERM_CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	$(RM) $@.$$$$

%.dpp : %.cpp $(REVISION_HEADER)
	@echo generating dependencies for $<
	@$(CXX) -MT '$*.o' -MM $(CPPFLAGS) $(ATERM_CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	$(RM) $@.$$$$

ifeq (,$(findstring $(MAKECMDGOALS),clean distclean revision))
Makefile: ../config

../config: ../config.in ../configure
	$(error You must (re)configure your source tree first)

-include $(foreach s,$(OBJECTS),$(shell if test -f $(basename $s).c; then echo -n $(basename $s).d; fi)) \
         $(foreach s,$(OBJECTS),$(shell if test -f $(basename $s).cpp; then echo -n $(basename $s).dpp; fi))
endif

#Note: to make a shared version of the gsrewr library instead of a static one, do the following:
#- add the option `-fpic' to CFLAGS
#- change the `a' suffix to `so' in LIBREWR_NAME
#- in the rule with target $(LIBREWR_NAME), replace the command by
#    $(CC) -shared -o $@ $(LIBREWR_OBJECTS) $(LIBREWR_LDFLAGS)
#- extend the environment variable LD_LIBRARY_PATH with `.'
