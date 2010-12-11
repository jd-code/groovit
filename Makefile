#
# $Id: Makefile,v 1.2 2003/03/19 16:45:47 jd Exp $
# Groovit Copyright (C) 1998,1999 Jean-Daniel PAUGET
# making accurate and groovy sound/noise
#
# groovit@disjunkt.com  -  http://groovit.disjunkt.com/
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
# 
# you can also try the web at http://www.gnu.org/
#
#
      FINAL = groovit
	SRC =	raw2disk.c readsample.c controls.c fileselect.c alert.c tokenio.c   \
		grov_io.c conv_ieee_ext.c gpm-xterm.c settings.c dialogs.c	    \
		grsel.c grpattern.c granalogik.c menu_board.c resoanlog.c grdelay.c \
		grmixeur.c grlevels.c grdyfilter.c $(FINAL).c			    \
		grderiv.c grevents.c grsequence.c

	INC =	raw2disk.h readsample.h controls.h fileselect.h alert.h tokenio.h   \
		grov_io.h conv_ieee_ext.h gpm-xterm.h settings.h dialogs.h	    \
		grsel.h grpattern.h granalogik.h menu_board.h resoanlog.h grdelay.h \
		grmixeur.h grlevels.h grdyfilter.h $(FINAL).h			    \
		sample.h placement.h radio.h constants.h			    \
		grderiv.h grevents.h grsequence.h 

	OBJ =	raw2disk.o readsample.o controls.o fileselect.o alert.o tokenio.o   \
		grov_io.o conv_ieee_ext.o gpm-xterm.o settings.o dialogs.o	    \
		grsel.o grpattern.o granalogik.o menu_board.o resoanlog.o grdelay.o \
		grmixeur.o grlevels.o grdyfilter.o $(FINAL).o			    \
		grderiv.o grevents.o grsequence.o

	MANS =	groovit.1 grov.5
   ADDITLIB = `cat ./additlib`

#    FINAL1 = tstdrum
#      SRC1 = readsample.c controls.c $(FINAL1).c
#      INC1 = readsample.h controls.h sample.h
#      OBJ1 = readsample.o controls.o $(FINAL1).o

     ALLOBJ = $(OBJ) $(OBJ1)
#  ALLFINAL = $(FINAL) $(FINAL1)
   ALLFINAL = $(FINAL) $(FINAL1)
     
     PREFIX = $(HOME)/local
     BINDIR = $(PREFIX)/bin
#    LIBDIR = $(PREFIX)/lib/$(FINAL)
     LIBDIR = $(PREFIX)/lib/$(FINAL)-$(VERSION)
     MANDIR = $(PREFIX)/man
    DIRNAME = `basename \`pwd\``
    UTILDIR = ./utils
     TARDIR = $(HOME)/tar
    DICTDIR = $(HOME)
         CC = `./.cc`
    VERSION = '1.5'
#    VERSION = `rlog -r RCS/$(FINAL).c,v | grep "revision " | cut -d' ' -f2 | cut -f1`
#    PSEUDO =
#    PSEUDO = "ביבא-לת"
     PSEUDO = walden
     CC_OBJ = -c
#  CC_STAMP = -DCCUNAMED="\"`uname -n` `uname -s` `uname -r`\"" -DCCCONFEDBY="\"`whoami` `date "+%d-%m-%Y %H:%M"`\""
   CC_STAMP = -DCCUNAMED="\"`./.inu`\"" -DCCCONFEDBY="\"`./.woa`\""
 CC_OPTIONS = -g -Winline -Wall -O3

## CC_DEBUG = -g -DFAKEDSP
##   CC_DEBUG =  -DFAKEDSP
##   CC_DEBUG = -g -DDEBSEQU -DDEBEVENTADD -DDEBEVMERGE # -DFAKEDSP
#  CC_DEBUG = -g -DDEB_MIXER -DDEB_READSAMPLE -DSHOWRCS -DDEB_VOICEAFF -DDEB_RCFILE
#  CC_DEBUG = -g -DDEBLEVREG
#  CC_DEBUG = -g -DNEVERINTER -DDEBUGNBCONTR 
#   CC_TEST = -DRELATFREQ -DRINGMODULATOR
#  CC_DEBUG = -DDEBUGMOUSE -DTESTDELETE -DDEBUGKEYB -DDEBUGTOKEN -DDEBUGIOERRS -DTESTDELETE -DDEBUGNBCONTR -DDEBUGGROVIO
#  CC_DEBUG = -DDEBUGTOKEN  -DDEBUGIOERRS -DDEBUGGROVIO -DVERYNASTYDEBUG
#  CC_DEBUG = -g -DFAKEDSP -DDEBUGNBCONTR -DDDD_DEBUG
#  CC_DEBUG =  -DDEBUGKEYB  -DDEBUGMOUSE -DDDD_DEBUG

	LIB = -lm
      CTAGS = ctags
#      CTAGS = /usr/local/bin/ctags
#     CTAGS = ccproc -t
      SHELL = /bin/sh

default: $(FINAL)

# .h.c:
# 	ls $@ | grep $@ > /dev/null 2> /dev/null || co $@

.c.o:	
	@echo
	$(CC) $(CC_OBJ) $(CC_OPTIONS) $(CC_DEBUG) $(CC_STAMP) $<

all:	mans $(ALLFINAL)

testdsp: testdsp.c
	$(CC)  -Wall -o testdsp testdsp.c

test: all
	@echo
	@echo -e "\033[44m-------------end of compilation-------------------------------------------------\033[m"
	@echo
	./$(FINAL) -fixedpos 40+40 test.grov && echo fini || ./$(FINAL) -fixedpos 40+40 test.grov -fakedsp

vimtest: all
	# xterm +lc +u8 -fn 10x20 -bg black -fg white -geometry 80x25+40+40 -e ./groovit -noX
	./$(FINAL) -fixedpos 40+40 -jack # 2>&1 | tr ':' '='

$(FINAL): $(OBJ) additlib $(OBJBIS)
	@echo
	$(CC) $(CC_OPTIONS) $(CC_STAMP) -o $(FINAL) $(OBJ) $(ADDITLIB) $(LIB)
	@echo
	@(      [ -f .usecolor ] && echo -n -e "\033[1m\033[33m" || echo -n )
	@echo "all is compiled."
	@echo "you now can try to install with su-ing to root then typing (make install)"
	@(      [ -f .usecolor ] && echo -n -e "\033[m" || echo -n )

$(FINAL1): $(OBJ1)
	@echo
	$(CC) $(CC_OPTIONS) $(CC_STAMP) -o $(FINAL1) $(OBJ1) $(LIB)

installer: Configure
	@/bin/bash ./Configure --frommakefile

config.h: Configure
	@/bin/bash ./Configure --frommakefile

advanced: Configure
	@/bin/bash ./Configure --advanced

config: Configure
	@/bin/bash ./Configure

fastconfig: Configure
	@/bin/bash ./Configure --fastconfig

fast: fastconfig
	@make all

additlib: config.h

groovit.1:  installer groovit.man1
	./installer groovit.1

grov.5:  installer grov.man5
	./installer grov.5

mans:	$(MANS)

clean:
	rm -f $(ALLFINAL) testdsp testdirent.c testdirent
	rm -f testdirent testdirent.c testdirent.out testcc testcc.c
	rm -f testtermios testtermios.c testtermios.out
	rm -f $(ALLOBJ) gpm-xterm.o
	rm -f $(MANS)
	rm -f core error.file
	rm -f .depend.bak

distclean: clean
	rm -f .usecolor
	rm -f ./raw*wav
	rm -f .testdspout
	rm -f groovit.1
	rm -f .woa .inu .cc .sdis
	rm -f config.h installer installer.bak config.bak additlib additlib.bak

mrproper: distclean

Configure:
	co RCS/Configure,v


zarbi: orig-tb.c
	@echo
	$(CC) -o orig-tb orig-tb.c -lm
	./orig-tb > blubbis
	cat ./blubbis > /dev/dsp

zip:	clean ver distclean
#	rm -f $(TARDIR)/$(FINAL)-*.tar $(TARDIR)/$(FINAL)-*.tar.gz
	( NBUNWILL=`ls *swp .*swp *.o 2> /dev/null | wc -l ` ;	\
	  NBUNWILL=`echo $$NBUNWILL` ;				\
	echo $$NBUNWILL ; \
	  [ E"$$NBUNWILL" = "E0" ] && echo -n ||	(	\
	    echo -e "\033[41m\n\nATTENTION ZIP NON CREE !!!!!!!!!!!!\n\033[m\n" ;	\
	    ls *swp .*swp *.o 2> /dev/null ) ;			\
	  [ E"$$NBUNWILL" = "E0" ] && echo -n ||	exit 2 ;\
	  VERSION=$(VERSION) ;					\
	  VERSION=`[ E"$(PSEUDO)"E = "EE" ] 			\
		&& echo $$VERSION 				\
		|| (echo -n $(PSEUDO)- ; date +%Y%m%d-%H%M )` ;	\
	  echo -e "\033[33m"$$VERSION"\033[m" ;			\
	  echo $$VERSION > .VERSION_INTERNAL ;			\
	  DIRNAME=$(DIRNAME) ;					\
	  cd .. ; chmod 700 $$DIRNAME ;				\
	  mv $$DIRNAME $(FINAL)-$$VERSION ;			\
	  tar -cf $(TARDIR)/$(FINAL)-$$VERSION-nodist.tar	\
		$(FINAL)-$$VERSION/* 				\
		$(FINAL)-$$VERSION/.??* ;			\
	  gzip -9 $(TARDIR)/$(FINAL)-$$VERSION-nodist.tar ;	\
	chmod 600 $(TARDIR)/$(FINAL)-$$VERSION-nodist.tar.gz ;	\
	  echo ;						\
	  cd $(TARDIR) ;					\
	  ls -l $(FINAL)-$$VERSION-nodist.*			\
	)

zips:	zip
	( echo ----------------------------------------------- ;\
	  VERSION=`cat VERSION` ;				\
	  VINTERNAL=`cat .VERSION_INTERNAL` ;			\
	  SAMPLEDIR=`./.sdis` ;					\
	  echo -e "\033[33m"$$VERSION $$VINTERNAL"\033[m" ;	\
	  echo "                        DEBUG =" $(CC_DEBUG) ;	\
	  sleep 2 ;						\
	  cd .. ;						\
	  rm -rf temp ;						\
	  mkdir temp ;						\
	  cd temp ;						\
	  tar -zxf $(TARDIR)/$(FINAL)-$$VINTERNAL-nodist.tar.gz ;	\
	  cd $(FINAL)-$$VINTERNAL ; pwd ; sleep 5 ;				\
	  co -kv RCS/* ;					\
	  rm -rf RCS ;						\
	  cd doc-groovit ;					\
	  co -kv RCS/* ;					\
	  rm -rf RCS ;						\
	  cd .. ;						\
	  echo erasing config.h ;				\
	  echo '#error no config file, run make config' > config.h ; \
	  echo forking making depend ;				\
	  make depend ;						\
	  rm config.h ;						\
	  echo 'copying samples' ;				\
	  mkdir SAMPLES ;					\
	  cat samples.lst | grep -v "^#" | while read SAMP ;	\
	  do							\
		cp $$SAMPLEDIR/$$SAMP SAMPLES ;	\
	  done ;						\
	  cd .. ;						\
	  mv $(FINAL)-$$VINTERNAL $(FINAL)-$$VERSION ;		\
	  tar -cf $(TARDIR)/$(FINAL)-$$VERSION.tar		\
	  	$(FINAL)-$$VERSION/*				\
		$(FINAL)-$$VERSION/.??*	;			\
	  gzip -9 $(TARDIR)/$(FINAL)-$$VERSION.tar ;		\
	  chmod 600 $(TARDIR)/$(FINAL)-$$VERSION.tar.gz ;	\
	  cd  $(FINAL)-$$VERSION ;				\
	  rm -rf SAMPLES ;					\
	  cd .. ;						\
	  tar -cf $(TARDIR)/$(FINAL)-$$VERSION-nosamples.tar	\
	  	$(FINAL)-$$VERSION/*				\
		$(FINAL)-$$VERSION/.??*	;			\
	  gzip -9 $(TARDIR)/$(FINAL)-$$VERSION-nosamples.tar ;	\
	  chmod 600 $(TARDIR)/$(FINAL)-$$VERSION-nosamples.tar.gz ;\
	  echo ;						\
	  cd $(TARDIR) ;					\
	  ls -l $(FINAL)-$$VERSION* ;				\
	)

ver:
	@echo $(VERSION) > VERSION
	@cat VERSION

install: installer $(FINAL)
	./installer

tag:	tags types

tags:	$(SRC) $(INC) config.h
	rm -f tags
	$(CTAGS) $(SRC) $(INC) config.h ### -o tags

types: types.vim

types.vim: $(SRC) $(INC) config.h
	ctags -i=gstuS -o- $(SRC) $(INC) config.h |\
	awk 'BEGIN{printf("syntax keyword Function\t")}\
            {printf("%s ", $$1)}END{print ""}' > $@
	(   echo -n "-l94 -i4 -ci4 -npsl -bad -bap -fc1 -d2 -fca -ncdb -bl -bli0 -cli4 -ss -pcs -cs -bs -cd33 -di8 -nbc -lp -sob " ; \
	ctags -i=gstuS -o- $(SRC) $(INC) config.h | cut -f1 | while read NOM ; do echo -n "-T $$NOM " ; done ) > .indent.pro
	echo 'syn keyword     cJDJD           contained JDJD JDJDJD JDJDJDJD' >> $@
	echo 'hi  link        cJDJD           cError' >> $@

vimall:
	@(   export SHELL=/usr/bin/tcsh ; \
	     vi -o2 ` ls -t $(SRC) $(INC) Makefile Configure` \
	)
#	xterm +sb -name vimall -geometry 106x45+0+0 -bg black -fg white -fn -misc-fixed-bold-r-normal--12-0-75-75-c-0-iso8859-1 -e vi -o2 ` ls -t $(SRC) $(INC) Makefile Configure` \
#	)
#	vi -o2 ` ls -t $(SRC) $(INC)` \
#	xterm +sb -name vimall -geometry 106x45+0+0 -bg black -fg white -e vi -o2 ` ls -t $(SRC) $(INC)` \
#	xterm +sb -name vimall -geometry 0+0+80x25 -e vi -o2 ` ls -t $(SRC) $(INC)` \

dict:	tags
	@cp -f $(DICTDIR)/dict $(DICTDIR)/dict.bak
	@(cat $(DICTDIR)/dict.bak ; cut -f1 tags) | sort -u > $(DICTDIR)/dict
	@diff $(DICTDIR)/dict $(DICTDIR)/dict.bak ; true
	@wc $(SRC) $(INC) | sort -n | grep -v total | percent
	@echo dict: `wc -w $(DICTDIR)/dict` words, `diff $(DICTDIR)/dict $(DICTDIR)/dict.bak | grep '<' | wc -l` changes

goomies:	dict
	@( TTY=`tty` ;							\
	   TMPFILE=/tmp/goomies ;					\
	  cat $(DICTDIR)/dict | while read IDENT ; 			\
	  do								\
		echo -n $$IDENT" " > $$TTY ;				\
		grep -w $$IDENT $(SRC) $(INC) > $$TMPFILE ;		\
		OCCUR=`cat $$TMPFILE | wc -l` ;				\
		RICHIDENT=`echo $$IDENT"                " | cut -b1-15`;\
		echo "$$OCCUR $$RICHIDENT " 				\
		`cut -d':' -f1 $$TMPFILE | countsort | sort -nr` ;	\
	  done | sort -nr > $(DICTDIR)/dict.cross ;			\
	)
	@echo done in $(DICTDIR)/dict.cross

diffstat: distclean
	( touch . ;							\
	  cd .. ;							\
	  REV1=`ls -d groovit-tel* | tail -2 | head -1` ;		\
	  REV2=`ls -d groovit-tel* | tail -1` ;				\
	  cd $$REV1; make distclean ; cd .. ;				\
	  cd $$REV2; make distclean ; cd .. ;				\
	  echo -e diffstat from "\033[33m"				\
		`echo $$REV1 | cut -d'-' -f3,4`				\
	"\033[m to \033[34m"						\
		`echo $$REV2 | cut -d'-' -f3,4`				\
	"\033[m" ;							\
	  diff -r --exclude=RCS --new-file $$REV1 $$REV2		\
	    | diffstat | grep -v "binary" ;				\
	)

	

dep:	depend

depend: $(SRC) $(INC)
	makedepend -f.depend.new -Y. $(SRC) 2> /dev/null 
# it's boring the way it doesn't find includes !
	@( diff .depend.new .depend 					\
		&& echo no changes in dependencies 			\
		|| (cp .depend .depend.bak ;				\
		    rm -f .depend ;					\
		    mv .depend.new .depend ;				\
		    : > .depend.new					\
	   	   )							\
	)
# this last one is for future generations...

include	.depend

#
# $Log: Makefile,v $
# Revision 1.2  2003/03/19 16:45:47  jd
# major changes everywhere while retrieving source history up to this almost final release
#
# Revision 1.2.0.24.0.3  2000/10/01 21:07:03  jd
# corrected SHELL var
#
# Revision 1.2.0.24.0.2  1999/10/19 19:29:45  jd
# *** empty log message ***
#
# Revision 1.2.0.24.0.1  1999/10/11 18:36:29  jd
# *** empty log message ***
#
# Revision 1.2.0.24  1999/10/11 15:55:35  jd
# second pre-tel-aviv version
#
# Revision 1.2.0.24  1999/10/11 15:30:51  jd
# second pre-tel-aviv revision
#
#
# Revision 1.2.0.23.0.6  1999/10/07 14:08:05  jd
# corrected man-pages generation
# modified localistation of shared sample
#
# Revision 1.2.0.23.0.4  1999/10/02 17:22:04  jd
# added time recption in make vimall
#
# Revision 1.2.0.23.0.3  1999/09/30 01:09:04  jd
# corrected diffstat stuff
#
# Revision 1.2.0.23.0.2  1999/09/27 00:46:09  jd
# added MAXDYFILT
# adjusted MAXLEVELS, MAXVOICES calulations
#
# Revision 1.2.0.23  1999/09/15 10:20:39  jd
# second pre tel-aviv public revision, for testing
#
# Revision 1.2.0.22.0.7  1999/09/15 09:23:30  jd
# added new modules
# added .swp detection in zip
# corrected make depend
# some color
#
# Revision 1.2.0.22.0.5  1999/09/05 22:30:16  jd
# added termios test
# added lot of mudules split from groovit:
# granalogik, grdyfilter, and resoanlog
# added a "make fast" and "make fastconfig"
#
# Revision 1.2.0.22.0.4  1999/08/30 22:16:03  jd
# added goomies generation (needs the countsort package from jd)
# added grsel module from groovit split
# added grpattern module from groovit split
# added menu_board module from groovit split at last,
# all of them with brand new actionne...
#
# Revision 1.2.0.22.0.3  1999/08/29 22:49:01  jd
# added grsel module
#
# Revision 1.2.0.22.0.2  1999/08/29 16:58:26  jd
# added dialogs
#
# Revision 1.2.0.22  1999/08/24 22:42:20  jd
# telaviv pre-release
#
# Revision 1.2.0.21.1.10  1999/08/24 22:33:19  jd
# added automated sample addition when making dists
# added simple C compiler detection
# corrected dependency generation before packaging distribution
# corrected installation hooked because of missing man page
#
# Revision 1.2.0.21.1.9  1999/07/28 19:08:31  jd
# fake revision
#
# Revision 1.2.0.21.1.8  1998/12/15 16:16:48  jd
# corrected additlib and (n)curses detection
# corrected gpmlib test
#
# Revision 1.2.0.21.1.4  1998/12/15 00:51:18  jd
# collapse from test branch
#
# Revision 1.2.0.21.1.3  1998/12/14 14:04:17  jd
# gpm indep test
#
# Revision 1.2.0.21.0.6  1998/12/10 14:29:46  jd
# added new binaries to handle
#
# Revision 1.2.0.21.0.5  1998/12/08 02:38:27  jd
# more indianness (only raw to disk missing)
#
# Revision 1.2.0.21.0.4  1998/12/07 22:19:27  jd
# removed verbose and annoying list when making dists
#
# Revision 1.2.0.21.0.1  1998/12/07 11:43:07  jd
# une version pour jcd
#
# Revision 1.2.0.21  1998/11/23 21:52:11  jd
# corrected char insertion bug in contredittext (!)
# added automatic ".grov" ending concatenation when saving if no dot in file name
# added menu for ponctual actions (save / about / quit and so on)
# added slightly more friendly alerts
# added quit warning
# corrected menu inconsistance in linux console
# added selection of raw-to-disk files
# corrected case of reading less voices than present
# added "wasmodified" flag and waning before erasing in memory...
# added bar man page installation refering to HTML
# added HTML manual pages + installation
# added doc-groovit directory + rcs handling
# corrected handling of suppr key in linux console
#
# Revision 1.2.0.20  1998/11/03 14:43:04  jd
# corrected delete and backspace bug in contredittext
# corrected overflow in sample dist reading when loading a song with
# more voices than handled
# handling of symbolic links to nowhere corrected in file selection
# corrected freezed list-box when there are exactly "lines plus one" elements
# corrected arrows bug in textedit
# corrected alerts overlapped by leds in fileselect
#
# Revision 1.2.0.20  1998/11/02 20:38:29  jd
# corrected delete and backspace bug in contredittext
# corrected overflow in sample dist reading when loading a song with
# more voices than handled
# handling of symbolic links to nowhere corrected in file selection
# corrected freezed list-box when there are exactly "lines plus one" elements
# corrected arrows bug in textedit
# corrected alerts overlapped by leds in fileselect
#
# Revision 1.2.0.19  1998/10/29 19:48:22  jd
# added bare INSTALL doc
# added bare public SAMPLES
# reducted # of sample voice to fit back in linux console by default
#
# Revision 1.2.0.18  1998/10/28 23:11:44  jd
# reforming positions definitions and created placement header file
# added precalculated pos for led-bars
# added generation of installer
# added make install via installer
# added fileselection of load save grov files
# added detection of stdout+stderr collisions
# added usage
# added make zips
# added samples installation
#
# Revision 1.2.0.17  1998/10/23 17:13:04  jd
# corrected includes in tokenio
# separated globals var for export across modules
# corrected unsigned 8bits data in .wav reading
# added loading and saving .grov
#
# Revision 1.2.0.16  1998/10/06 22:05:42  jd
# added keeping the last working dir for samples...
# added curses lib and corrected minor warning in libgpm detection Configure
# added correction of the "I forgot to stop the timer" bug
# added embryo of input-output for .grov files
# added token and chunk output
#
# Revision 1.2.0.15  1998/09/09 22:15:18  jd
# debugged the end of controls in tty-mode
# added flushing all bufs before finishing
# added multi anologik voices
# added patterns handling for a-voices
#
# Revision 1.2.0.14  1998/09/08 00:44:07  jd
# second collapse before multi analogic-voices
#
# Revision 1.2.0.13  1998/09/06 23:13:29  jd
# main collapse before adding multiple analog voices and pre-module organisation
#
# Revision 1.2.0.12  1998/09/01 01:32:58  jd
# added alerts
# added minimal stating sample-files
# added gpl warning
# corrected contexts usage
# modified contexts changing redraw algorithme
#
# Revision 1.2.0.11  1998/08/18 04:22:52  jd
# added reassignlistbox
# added fileselect
# added editable text
# added reassigntextedit
# added relooksimplebutton
# added zeroradio
# correct sample->name
#
# Revision 1.2.0.10  1998/08/15 00:31:37  jd
# added removestrback
# corrected deletecontrols
# detected an inconsistancy in libgpm+xterm, suggested use of patched libgpm
# added patched libgpm detection and proposal
# tested and fixed use of gpm-xterm
# added listbox
#
# Revision 1.2.0.9  1998/08/03 00:43:28  jd
# added wave input
#
# Revision 1.2.0.8  1998/07/31 23:52:27  jd
# added gpl, uname and who traced in config.h, bare joystick release, bare raw2dis
#
# Revision 1.2.0.7  1998/07/28 15:11:39  jd
# collapse before adding joystick
#
# Revision 1.2.0.6  1998/06/29 18:01:36  jd
# dissociated filter-pat/analog-pat
#
# Revision 1.2.0.5  1998/06/25 22:05:50  jd
# special CB release
#
# Revision 1.2.0.4  1998/06/15 20:05:04  jd
# added second cutoff
#
# Revision 1.2.0.3  1998/06/14 18:51:47  jd
# collapse from previous branch
#
# Revision 1.2.0.2.0.2  1998/06/14 17:09:34  jd
# loss of the e
#
# Revision 1.2.0.2.0.1  1998/06/05 22:18:38  jd
# fork
#
# Revision 1.2.0.2  1998/06/05 22:17:23  jd
# fork for mono-reverb test
#
# Revision 1.2.0.1  1998/06/03 14:38:55  jd
# analog development initial release
#
# Revision 1.2  1998/06/03 14:13:16  jd
# minimal analog part
#
# Revision 1.1.0.15  1998/06/03 14:00:15  jd
# transition to analog part
#
# Revision 1.1.0.14  1998/06/02 21:53:09  jd
# version de transition vers la synthese analogique...
#
# Revision 1.1.0.13  1998/05/27 22:05:48  jd
# added locking mem against swap
#
# Revision 1.1.0.12  1998/05/25 23:35:19  jd
# added vertical slide
# added radio component for rythm parts
# added dynamic part
# analog sound embryo
#
# Revision 1.1.0.11  1998/05/17 21:47:07  jd
# no more timer used in xterm
# pattern bank radio button
# dup-pattern button
# no more rev-video for focused controls
#
# Revision 1.1.0.10  1998/05/17 00:12:20  jd
# added Configure keeped out of RCS
#
# Revision 1.1.0.9  1998/05/17 00:07:45  jd
# added interrupted select call handling
# cleaned select return treatment
#
# Revision 1.1.0.8  1998/05/16 01:26:42  jd
# added buttons and corrected more stalling process.
# also completed configue script
#
# Revision 1.1.0.7  1998/05/12 15:22:07  jd
# added interrupted write handling
#
# Revision 1.1.0.6  1998/05/11 11:00:58  jd
# release with pattern row
#
# Revision 1.1.0.4  1998/05/04 23:23:42  jd
# first release mit patternfield
#
# Revision 1.1.0.3  1998/04/28 17:59:55  jd
# first gpm minimal version
#
# Revision 1.1.0.2  1998/04/17 14:24:02  jd
# added RCS
#
# Revision 1.1.0.1  1998/04/17 14:13:51  jd
# first branch release
#
# Revision 1.1  1998/04/17 14:13:21  jd
# Initial revision
#
#
