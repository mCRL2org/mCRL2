#ATerm definitions
ATERM_ROOT      = /home/amathijs/asfsdf
ATERM_INCLUDE   = $(ATERM_ROOT)/include
ATERM_LIB_PATH  = $(ATERM_ROOT)/lib

#Redefine default definitions
CFLAGS          = -std=c99 -I$(ATERM_INCLUDE)
ARFLAGS         = crs
LEX             = flex
LFLAGS          = -I -Pgsyy
YACC            = bison
YFLAGS          = -d -pgsyy

#Library definitions
LIB_NAME        = libgsparse.a
LIB_OBJECTS     = libgsparse.o gsfunc.o gsparser.o gslexer.o
LIB_LDFLAGS     = -L$(ATERM_LIB_PATH) -lATerm

#Parser definitions
PARSER_NAME     = gsparse
PARSER_OBJECTS  = gsparse.o
PARSER_LDFLAGS  = -L. -lgsparse -lATerm

#Make rules
.PHONY: all clean
.PRECIOUS: gsparser.c gslexer.c

all : $(LIB_NAME) $(PARSER_NAME) Makefile

$(LIB_NAME) : $(LIB_OBJECTS) Makefile
	$(AR) $(ARFLAGS) $@ $(LIB_OBJECTS)

$(PARSER_NAME) : $(PARSER_OBJECTS) $(LIB_NAME) Makefile
	$(CC) -o $@ $(PARSER_OBJECTS) $(PARSER_LDFLAGS)

gslexer.o      : gslexer.h gsparser.h gsfunc.h
gsparser.o     : gsfunc.h
gsfunc.o       : gsfunc.h
libgsparse.o   : libgsparse.h gslexer.h gsfunc.h 
gsparse.o      : gsparse.h libgsparse.h gsfunc.h

%.c : %.y
	$(YACC) $(YFLAGS) -o $@ $<

%.c : %.l
	$(LEX) $(LFLAGS) -o$@ $<

clean :
	$(RM) $(LIB_NAME) $(PARSER_NAME) *.o gslexer.c gsparser.c gsparser.h gsparser.output *~ core core.*

#Note: to make a shared version of the gsparse library instead of a static one, do the following:
#- add the option `-fpic' to CFLAGS
#- change the `a' suffix to `so' in LIB_NAME
#- in the rule with target $(LIB_NAME), replace the command by
#    $(CC) -shared -o $@ $(LIB_OBJECTS) $(LIB_LDFLAGS)
#- extend the environment variable LD_LIBRARY_PATH with `.'
