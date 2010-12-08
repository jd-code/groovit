/* 
 * $Id: gpm-xterm.h,v 1.2 2003/03/19 16:45:47 jd Exp $
 * Groovit Copyright (C) 1998,1999 Jean-Daniel PAUGET
 * Copyright 1994,1995   rubini@ipvvis.unipv.it (Alessandro Rubini)
 * Copyright 1994        miguel@roxanne.nuclecu.unam.mx (Miguel de Icaza)
 * 
 * this source is a special adaptation for groovit.
 * you should check for the original version from gpm project.
 *
 * gpm-xterm.h - pseudo client for non-Linux xterm only mouse support.
 *               This code has been extracted from libgpm-0.18 and then
 *               took its own way.
 *
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 ********/

#ifndef _GPM_XTERM_H_
#define _GPM_XTERM_H_

#ifndef _GPM_H_
/* ....................................... Xtermish stuff */
#define GPM_XTERM_ON \
  printf("%c[?1001s",27), fflush(stdout), /* save old hilit tracking */ \
  printf("%c[?1000h",27), fflush(stdout)	/* enable mouse tracking */

#define GPM_XTERM_OFF \
  printf("%c[?10001",27), fflush(stdout), /* disable mouse tracking */ \
  printf("%c[?1001r",27), fflush(stdout)	/* restore old hilittracking */

/* ....................................... Cfg buttons */

#define GPM_B_LEFT      4
#define GPM_B_MIDDLE    2
#define GPM_B_RIGHT     1
#endif
/* ....................................... The event types */

#ifndef _GPM_H_
enum Gpm_Etype
{
    GPM_MOVE = 1,
    GPM_DRAG = 2,	       /* exactly one in four is active at a time */
    GPM_DOWN = 4,
    GPM_UP = 8,

/* JDJDJDJD test this suppression */
/* #define GPM_BARE_EVENTS(type) ((type)&(0xF|GPM_ENTER|GPM_LEAVE)) */

    GPM_SINGLE = 16,	       /* at most one in three is set */
    GPM_DOUBLE = 32,
    GPM_TRIPLE = 64,	       /* WARNING: I depend on the values */

    GPM_MFLAG = 128,	       /* motion during click? */
    GPM_HARD = 256,	       /* if set in the defaultMask, force an already used event to
			          pass over to another handler */
    GPM_ENTER = 512,	       /* enter event, user in Roi's */
    GPM_LEAVE = 1024	       /* leave event, used in Roi's */
};

#endif
/* ....................................... The event data structure */

#ifndef _GPM_H_
enum Gpm_Margin
{
    GPM_TOP = 1, GPM_BOT = 2, GPM_LFT = 4, GPM_RGT = 8
};

#endif

#ifndef _GPM_H_
typedef struct Gpm_Event
{
    unsigned char buttons, modifiers;	/* try to be a multiple of 4 */
    unsigned short vc;
    short   dx, dy, x, y;
    enum Gpm_Etype type;
    int     clicks;
    enum Gpm_Margin margin;
}
Gpm_Event;

#endif

/* ....................................... The connection data structure */

#ifndef _GPM_H_
#define GPM_MAGIC 0x47706D4C	/* "GpmL" */
#endif
typedef struct JDGpm_Connect
{
    unsigned short eventMask, defaultMask;
    unsigned short minMod, maxMod;
    int     pid;
    int     vc;
}
JDGpm_Connect;

/* ....................................... Global variables for the client */

extern int JDgpm_flag, JDgpm_ctlfd, JDgpm_fd, JDgpm_hflag, JDgpm_morekeys;

typedef int JDGpm_Handler (Gpm_Event *event, void *clientdata);

extern JDGpm_Handler *JDgpm_handler;
extern void *JDgpm_data;

extern int JDGpm_Open (JDGpm_Connect *, int);
extern int JDGpm_Close (void);
extern int JDGpm_Getc (FILE *);

#define    JDGpm_Getchar() JDGpm_Getc(stdin)
extern int JDGpm_Repeat (int millisec);

/* #include <cjdurses.h> Hmm... risky... */

extern int JDGpm_Wgetch ();

#define JDGpm_Getch() (JDGpm_Wgetch(NULL))

/* disable the functions available in libgpm but not here */
/* 
   #define    JDGpm_FitValuesM(x, y, margin) #define    JDGpm_FitValues(x,y) #define
   JDGpm_FitEvent(ePtr)

   #define JDGpm_DrawPointer(x,y,fd) #define GPM_DRAWPOINTER(ePtr) */
#define JDGPM_DRAWPOINTER(ePtr)

/* This material comes from gpmCfg.h */
#ifndef _GPM_H_
#define SELECT_TIME 86400	/* one day */
#endif

#endif /* _GPM_XTERM_H_ */
