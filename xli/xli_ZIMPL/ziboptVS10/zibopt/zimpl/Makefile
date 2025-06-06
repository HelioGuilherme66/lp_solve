# $Id: Makefile,v 1.76 2010/09/30 14:45:12 bzfkocht Exp $
#* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#*                                                                           *
#*   File....: Makefile                                                      *
#*   Name....: Zimpl Makefile                                                *
#*   Author..: Thorsten Koch                                                 *
#*   Copyright by Author, All rights reserved                                *
#*                                                                           *
#* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#*
#* Copyright (C) 2005-2010 by Thorsten Koch <koch@zib.de>
#* 
#* This program is free software; you can redistribute it and/or
#* modify it under the terms of the GNU General Public License
#* as published by the Free Software Foundation; either version 2
#* of the License, or (at your option) any later version.
#* 
#* This program is distributed in the hope that it will be useful,
#* but WITHOUT ANY WARRANTY; without even the implied warranty of
#* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#* GNU General Public License for more details.
#* 
#* You should have received a copy of the GNU General Public License
#* along with this program; if not, write to the Free Software
#* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#*
#
.PHONY:		all depend clean lint doc check valgrind libdbl coverage

ARCH            :=      $(shell uname -m | \
                        sed \
			-e s/sun../sparc/ \
			-e s/i.86/x86/ \
			-e s/i86pc/x86/ \
			-e s/IP../mips/ \
			-e s/9000..../hppa/ \
			-e s/Power\ Macintosh/ppc/ \
			-e s/00........../pwr4/)
OSTYPE          :=      $(shell uname -s | \
                        tr '[:upper:]' '[:lower:]' | \
			tr -d '[:punct:]' | \
                        sed \
			-e s/cygwin.*/cygwin/ \
                        -e s/irix../irix/ )
HOSTNAME	:=      $(shell uname -n | tr '[:upper:]' '[:lower:]')

VERSION		=	3.1.0
VERBOSE		=	false
OPT		=	opt
COMP		=	gnu
CC		=	gcc
YACC		=	bison
LEX		=	flex
DCC		=	gcc
LINT		=	flexelint
AR		=	ar
RANLIB		=	ranlib
VALGRIND	=	valgrind --tool=memcheck --leak-check=full \
			--leak-resolution=high --show-reachable=yes 

SRCDIR		=	src
BINDIR		=	bin
LIBDIR		=	lib

CPPFLAGS	=	-I$(SRCDIR) -DVERSION='"$(VERSION)"'
CFLAGS		=	-O
LDFLAGS		=	-lgmp -lz -lm
YFLAGS		=	-d -t -v  
LFLAGS		=	-d
ARFLAGS		=	cr
DFLAGS		=	-MM

GCCWARN		=	-Wall -W -Wpointer-arith -Wcast-align -Wwrite-strings \
			-Wstrict-prototypes -Wmissing-prototypes -Winline \
			-Wmissing-declarations -Wshadow -Waggregate-return \
			-Wno-unused -Wno-unknown-pragmas 

BASE		=	$(OSTYPE).$(ARCH).$(COMP).$(OPT)
OBJDIR		=	obj/O.$(BASE)
NAME		=	zimpl
BINNAME		=	$(NAME)-$(VERSION).$(BASE)
LIBNAME		=	$(NAME)-$(VERSION).$(BASE)
LIBRARY		=	$(LIBDIR)/lib$(LIBNAME).a
LIBRARYDBL	=	$(LIBDIR)/lib$(LIBNAME).dbl.a
BINARY		=	$(BINDIR)/$(BINNAME)
LIBLINK		=	$(LIBDIR)/lib$(NAME).$(BASE).a
LIBDBLLINK	=	$(LIBDIR)/lib$(NAME).$(BASE).dbl.a
BINLINK		=	$(BINDIR)/$(NAME).$(BASE)
BINSHORTLINK	=	$(BINDIR)/$(NAME)
DEPEND		=	$(SRCDIR)/depend

#-----------------------------------------------------------------------------

OBJECT  	=       zimpl.o xlpglue.o \
			ratlpstore.o ratlpfwrite.o ratmpswrite.o ratmstwrite.o \
			ratordwrite.o ratpresolve.o  
LIBBASE		=	blkmem.o bound.o code.o conname.o define.o elem.o entry.o \
			gmpmisc.o hash.o heap.o idxset.o inst.o iread.o list.o \
			load.o local.o metaio.o mmlparse2.o mmlscan.o mono.o \
			numbgmp.o prog.o random.o rdefpar.o source.o \
			setempty.o setpseudo.o setlist.o setrange.o setprod.o \
			setmulti.o set4.o stmt.o strstore2.o symbol.o term2.o \
			tuple.o vinst.o mshell.o zimpllib.o
LIBOBJ		=	$(LIBBASE) gmpmisc.o numbgmp.o
LIBDBLOBJ	=	$(LIBBASE) numbdbl.o
OBJXXX		=	$(addprefix $(OBJDIR)/,$(OBJECT))
LIBXXX		=	$(addprefix $(OBJDIR)/,$(LIBOBJ))
LIBDBLXXX	=	$(addprefix $(OBJDIR)/,$(LIBDBLOBJ))
OBJSRC		=	$(addprefix $(SRCDIR)/,$(OBJECT:.o=.c))
LIBSRC		=	$(addprefix $(SRCDIR)/,$(LIBOBJ:.o=.c)) #(SRCDIR)/numbdbl.c

#-----------------------------------------------------------------------------
include make/make.$(BASE)
-include make/local/make.$(HOSTNAME)
-include make/local/make.$(HOSTNAME).$(COMP)
-include make/local/make.$(HOSTNAME).$(COMP).$(OPT)
#-----------------------------------------------------------------------------

FLAGS           +=      $(USRFLAGS)
OFLAGS          +=      $(USROFLAGS)
CFLAGS          +=      $(USRCFLAGS) $(USROFLAGS)
LDFLAGS         +=      $(USRLDFLAGS)
ARFLAGS         +=      $(USRARFLAGS)
DFLAGS          +=      $(USRDFLAGS)


ifeq ($(VERBOSE),false)
.SILENT:	$(LIBRARY) $(LIBLINK) $(BINARY) $(BINLINK) $(BINSHORTLINK) \
		$(SRCDIR)/mmlparse2.c $(SRCDIR)/mmlscan.c $(OBJXXX) $(LIBXXX) $(LIBDBLXXX)
endif

all:		$(LIBRARY) $(LIBLINK) $(BINARY) $(BINLINK) $(BINSHORTLINK)

$(LIBLINK):	$(LIBRARY)
		@rm -f $@
		cd $(dir $@) && ln -s $(notdir $(LIBRARY)) $(notdir $@)

$(LIBDBLLINK):	$(LIBRARYDBL)
		@rm -f $@
		cd $(dir $@) && ln -s $(notdir $(LIBRARYDBL)) $(notdir $@)

$(BINLINK) $(BINSHORTLINK):	$(BINARY)
		@rm -f $@
		cd $(dir $@) && ln -s $(notdir $(BINARY)) $(notdir $@)

$(BINARY):	$(OBJDIR) $(BINDIR) $(OBJXXX) $(LIBRARY) 
		@echo "-> linking $@"
		$(CC) $(CFLAGS) $(OBJXXX) -L$(LIBDIR) -l$(LIBNAME) $(LDFLAGS) -o $@

$(LIBRARY):	$(OBJDIR) $(LIBDIR) $(LIBXXX) 
		@echo "-> generating library $@"
		-rm -f $(LIBRARY)
		$(AR) $(ARFLAGS) $@ $(LIBXXX)
		$(RANLIB) $@

libdbl:		$(LIBRARYDBL) $(LIBDBLLINK)

$(LIBRARYDBL):	$(OBJDIR) $(LIBDIR) $(LIBDBLXXX) 
		@echo "-> generating library $@"
		-rm -f $(LIBRARYDBL)
		$(AR) $(ARFLAGS) $@ $(LIBDBLXXX)
		$(RANLIB) $@

$(SRCDIR)/mmlparse2.c:	$(SRCDIR)/mmlparse2.y $(SRCDIR)/mme.h
		@echo "-> generating yacc parser $@"
		$(YACC) $(YFLAGS) -o $@ $<

$(SRCDIR)/mmlscan.c:	$(SRCDIR)/mmlscan.l $(SRCDIR)/mme.h
		@echo "-> generating lex scanner $@"
		$(LEX) $(LFLAGS) -o$@ $< 

lint:		$(OBJSRC) $(LIBSRC)
		$(LINT) $(SRCDIR)/project.lnt \
		$(CPPFLAGS) -UNDEBUG -Dinline= -DNO_MSHELL $^

doc:
		cd doc; make -f Makefile

check:
		cd check; \
		/bin/sh ./check.sh ../$(BINARY) 

valgrind:
		cd check; \
		/bin/sh ./check.sh "$(VALGRIND) ../$(BINARY)" 

coverage:
		-ln -s ../../src $(OBJDIR)
		-mkdir -p gcov
		lcov -d $(OBJDIR) -z
		make OPT=gcov check
		lcov -d $(OBJDIR) -c >gcov/z.capture
		lcov -d $(OBJDIR) -r gcov/z.capture "*mmlscan.c" "*mmlparse2.c" >gcov/zimpl.capture
		genhtml -o gcov gcov/zimpl.capture
		-rm gcov/z.capture

$(OBJDIR):	
		@echo "** creating directory \"$@\""
		@-mkdir -p $(OBJDIR)

$(LIBDIR):
		@echo "** creating directory \"$@\""
		@-mkdir -p $(LIBDIR)

$(BINDIR):
		@echo "** creating directory \"$@\""
		@-mkdir -p $(BINDIR)

clean:
		-rm -rf $(OBJDIR)/* $(BINARY) $(LIBRARY) $(LIBRARYDBL) $(LIBLINK) $(BINLINK) $(BINSHORTLINK)

depend:
		$(SHELL) -ec '$(DCC) $(DFLAGS) $(CPPFLAGS) $(OBJSRC) $(LIBSRC) \
		| sed '\''s|^\([0-9A-Za-z\_]\{1,\}\)\.o|$$\(OBJDIR\)/\1.o|g'\'' \
		>$(DEPEND)'

-include	$(DEPEND)

$(OBJDIR)/%.o:	$(SRCDIR)/%.c
		@echo "-> compiling $@"
		$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# --- EOF ---------------------------------------------------------------------
