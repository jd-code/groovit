/* 
 * $Id: menu_board.c,v 1.2 2003/03/19 16:45:47 jd Exp $
 * Groovit Copyright (C) 1998,1999 Jean-Daniel PAUGET
 * making accurate and groovy sound/noise
 *
 * groovit@disjunkt.com  -  http://groovit.disjunkt.com/
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * you can also try the web at http://www.gnu.org/
 *
 * $Log: menu_board.c,v $
 * Revision 1.2  2003/03/19 16:45:47  jd
 * major changes everywhere while retrieving source history up to this almost final release
 *
 * Revision 1.2.0.24.0.3  2000/10/01 21:07:03  jd
 * added Dsp button
 *
 * Revision 1.2.0.24.0.2  1999/10/19 19:29:45  jd
 * *** empty log message ***
 *
 * Revision 1.2.0.24.0.1  1999/10/11 18:36:29  jd
 * *** empty log message ***
 *
 * Revision 1.2.0.24  1999/10/11 15:30:51  jd
 * second pre-tel-aviv revision
 *
 * Revision 1.2.0.23.0.5  1999/10/07 14:08:05  jd
 * Revision 1.2.0.23.0.2  1999/09/27 00:46:09  jd
 * all lot of correctioopn for new filter pattern
 * (should be rewrite completely soon)
 *
 * Revision 1.2.0.23  1999/09/15 10:20:39  jd
 * second pre tel-aviv public revision, for testing
 *
 * Revision 1.2.0.22.0.4  1999/09/15 09:23:30  jd
 * suppressed filters
 * debugged from unasigned value at start in dyfilter vslides
 * corrected syncing levels display at init
 *
 * Revision 1.2.0.22.0.2  1999/09/05 22:30:16  jd
 * imported more code from old main
 *
 * Revision 1.2.0.22.0.1  1999/08/30 22:45:51  jd
 * Revision 1.1  1999/08/30 22:41:12  jd
 * Initial revision
 *
 */

#include <stdio.h>
#include <string.h>

#include "config.h"

#include "sample.h"
#include "placement.h"
#include "controls.h"
#include "dialogs.h"
#include "grsel.h"
#include "settings.h"
#include "grpattern.h"
#include "granalogik.h"
#include "grdyfilter.h"
#include "grdelay.h"
#include "grlevels.h"
#include "grderiv.h"	/* JDJDJDJDJD a nettoyer plus tard... */
#include "groovit.h"
#define INMAINMENU
#include "menu_board.h"


/* ---------------- les handles de controls a conserver ----------------------- */
static 
int ctxmenu,			/* handle du contexts de menu			*/
    hmenuitem [MAXMENUITEM],	/* handles des items d'ycelui			*/
    curmenu,			/* le menu en cour d'activite			*/
    hflapmenu,			/* handle du bouton d'activation du menu	*/
    flapmenu;			/* le bouton d'activation du menu		*/
static
Radio radiomenuitem;			/* la valeur du radio du menu		*/
static
char weakmenubuf [MAXMENULARG+2],	/* sert au réinitialisations de menus	*/
     *version;				/* contient la version de groovit	*/

static
int curline;			/* no radio de la ligne pattern editee en dynamique */



void fillmenu (char ** menu)		/* menu is an NULL ended list of strings */
{	int i=0;

	while ((i<MAXMENUITEM) && (*menu != NULL))
	{	relooksimplebutton (hmenuitem[i], 0, CONTRED+CONTNOPUCE, *menu);
		menu ++;
		i++;
	}
	while (i<MAXMENUITEM)
	{	relooksimplebutton (hmenuitem[i], 0, CONTRED+CONTNOPUCE, weakmenubuf);
		i++;
	}
}

int initmenu (void)
{	int i;

	for (i=0 ; i<MAXMENULARG ; i++) weakmenubuf[i] = ' ';
	weakmenubuf[i] = 0;
	curmenu = -1;
	ctxmenu = createcontexts (ROWMENU, LINEMENU, MAXMENULARG+2, MAXMENUITEM+2, CONTRED, "menu", NULL);
	installcontexts (ctxmenu);
	radiomenuitem.nb = 0;
	radiomenuitem.value = -1;
	for (i=0 ; i<MAXMENUITEM ; i++)
	{	hmenuitem [i] = createsimplebutton (CONTRRADIO, ROWMENU+1, LINEMENU+1+i, 0, CONTRED+CONTNOPUCE, CONTUNDEF, &radiomenuitem, weakmenubuf);
//		registertocontexts (ctxmenu, hmenuitem [i]);
	}
	desinstallcontexts (ctxmenu);
	return (ctxmenu);
}

int close_menu_main (void)
{	curmenu = -1;
	flapmenu = BUT_OFF;
	refreshcontrols (hflapmenu);
/*	actionnecontexts (ctxmenu, NULL); pourrait etre utile par la suite... */
	return desinstallcontexts (ctxmenu);
}
int Amenu_main (int lastpressed)
{
	if ((lastpressed != -1) && (lastpressed != hflapmenu))
	{   
	    close_menu_main ();

	    switch (lastpressed-hmenuitem[0])
	    {	
		case ITEMNWSG:
		if (wasmodified == 0)
		{	initallvoices ();
			wasmodified = 0;
			resync_displaypatterns ();
			strcpy (cursongfile, "no-title");
			shortcursongfile = cursongfile;
			settitle (gname, shortcursongfile);
			refreshscreen ();
		}
		else
			alert_era ();
		break;

		case ITEMLOAD:
		if (wasmodified == 0)
			grsel_load (cursongfile);
		else
			alert_lany ();
		break;
	
		case ITEMSAVE:
			grsel_save (cursongfile);
		break;

		case ITEMSRAW:
			grsel_rawf ((nfoutbis[0] != 0) ? nfoutbis : "rawfile.wav");
		break;

		case ITEMPREF:
			installcontexts (ctxsttg);
		break;

		case ITEMABOUT:
			alert_about (version);
		break;

		case ITEMQUIT:
		if (wasmodified == 0)
			alert_quit ();
		else
			alert_savequit ();
		break;

		default:
		    ;
	    }
	}
	return 0;
}
int menu_main (void)
{	fillmenu( fmenmain );
	radiomenuitem.value = -1;
	curmenu = MENU_MAIN ;
	actionnecontexts ( ctxmenu , Amenu_main);
	return installcontexts (ctxmenu);
}

/*=====================================================================================*/


void reassignpattern (Pattern *pattern)
{
	int i;

	for (i=0 ; i<MAXSAMPLE ; i++)
	{
		reassigncontrols (hpattern[i], &pattern->start[i][0+currowoffset]);
		refreshpfield (hpattern[i], curpattern->nbrow-currowoffset );
	}
	for (i=0 ; i<MAXDISPLAYROW ; i++)
	{	
		if (curline < MAXSAMPLE)
		{	reassigncontrols (hrows[i], &curpattern->start[curline][i+currowoffset]);
			reassoccontrols (hrows[i], hpattern[curline]);
		}
		else
		{
			if (curline < MAXSAMPLE+MAXANALOG)
			{	reassigncontrols (hrows[i], &anvoice[curline-MAXSAMPLE].curapattern->freqbase[i+currowoffset]);
				reassoccontrols (hrows[i], hapattern[curline-MAXSAMPLE]);
			}
			else
			{	reassigncontrols (hrows[i], &dyfilter[curline-MAXSAMPLE-MAXANALOG].curfpattern->freqbase[i+currowoffset]);
				reassoccontrols (hrows[i], hfpattern[curline-MAXSAMPLE-MAXANALOG]);
			}
		}
	}
	reassigncontrols (hpatternrow, &pattern->nbrow);
}

void reassignapattern (int nvoice, APattern *curapattern)
{
	int i;

	reassigncontrols (hapattern[nvoice], &curapattern->freqbase[0+currowoffset]);
	refreshpfield (hapattern[nvoice], curapattern->nbrow-currowoffset);

	for (i=0 ; i<MAXDISPLAYROW ; i++)
	{	
		if (curline < MAXSAMPLE)
		{	reassigncontrols (hrows[i], &curpattern->start[curline][i+currowoffset]);
			reassoccontrols (hrows[i], hpattern[curline]);
		}
		else
		{
			if (curline < MAXSAMPLE+MAXANALOG)
			{	reassigncontrols (hrows[i], &anvoice[curline-MAXSAMPLE].curapattern->freqbase[i+currowoffset]);
				reassoccontrols (hrows[i], hapattern[curline-MAXSAMPLE]);
			}
			else
			{	reassigncontrols (hrows[i], &dyfilter[curline-MAXSAMPLE-MAXANALOG].curfpattern->freqbase[i+currowoffset]);
				reassoccontrols (hrows[i], hfpattern[curline-MAXSAMPLE-MAXANALOG]);
			}
		}
	}
	reassigncontrols (hapatternrow[nvoice], &curapattern->nbrow);
}

void reassignfpattern (int nvoice, APattern *curfpattern)
{
	int i;

	reassigncontrols (hfpattern[nvoice], &curfpattern->freqbase[0+currowoffset]);
	refreshpfield (hfpattern[nvoice], curfpattern->nbrow-currowoffset);

	for (i=0 ; i<MAXDISPLAYROW ; i++)
	{
		if (curline < MAXSAMPLE)
		{	reassigncontrols (hrows[i], &curpattern->start[curline][i+currowoffset]);
			reassoccontrols (hrows[i], hpattern[curline]);
		}
		else
		{
			if (curline < MAXSAMPLE+MAXANALOG)
			{	reassigncontrols (hrows[i], &anvoice[curline-MAXSAMPLE].curapattern->freqbase[i+currowoffset]);
				reassoccontrols (hrows[i], hapattern[curline-MAXSAMPLE]);
			}
			else
			{	reassigncontrols (hrows[i], &dyfilter[curline-MAXSAMPLE-MAXANALOG].curfpattern->freqbase[i+currowoffset]);
				reassoccontrols (hrows[i], hfpattern[curline-MAXSAMPLE-MAXANALOG]);
			}
		}
	}
	reassigncontrols (hfpatternrow[nvoice], &curfpattern->nbrow);
}

void resync_displaypatterns (void)	/* exportee */	/* JDJDJDJD cette partie est incomprehensible, a refaire */
{	int i;
	reassignpattern (curpattern);
	for (i=0 ; i<MAXANALOG ; i++)
	{	AnVoice *anv = &anvoice[i];
		reassignapattern (i, anv->curapattern);
	}
	for (i=0 ; i<MAXDYFILT ; i++)
	{	DyFilter *fil = &dyfilter[i];
		reassignfpattern (i, fil->curfpattern);
	}
}

int Aactionboard (int lastpressed)
{
	/* open and close menu with button */
	if ((flapmenu == BUT_ON) && (curmenu == -1))
		menu_main ();
	if ((flapmenu == BUT_OFF) && (curmenu != -1))
		close_menu_main ();

	/* watch the levels menu */
	menu_watcher ();

	/* raise file selection for samples */
	if ((radiosample.value != -1) && (curcontext == 0))
		grsel_sample (sample[radiosample.value].name);

	/* move [A/F/_]patterns to left or right */
	if (goright == BUT_ON)
	{	goright = BUT_OFF;
		refreshcontrols (hgoright);
		currowoffset += 8;
		if (currowoffset > MAXPATTERNROW-MAXDISPLAYROW)
			currowoffset = MAXPATTERNROW-MAXDISPLAYROW;

		resync_displaypatterns ();
	}
	if (goleft == BUT_ON)
	{	goleft = BUT_OFF;
		refreshcontrols (hgoleft);
		currowoffset -= 8;
		if (currowoffset < 0)
			currowoffset = 0;

		resync_displaypatterns ();
	}

	/* sync levels editor with appropriate values when change occurs */
	if (curline != radiorow.value)
	{	int i;

		curline = radiorow.value;
		for (i=0 ; i<MAXDISPLAYROW ; i++)
		{	
			if (curline < MAXSAMPLE)
			{	reassigncontrols (hrows[i], &curpattern->start[curline][i+currowoffset]);
				reassoccontrols (hrows[i], hpattern[curline]);
			}
			else
			{
				if (curline < MAXSAMPLE+MAXANALOG)
				{	reassigncontrols (hrows[i], &anvoice[curline-MAXSAMPLE].curapattern->freqbase[i+currowoffset]);
					reassoccontrols (hrows[i], hapattern[curline-MAXSAMPLE]);
				}
				else
				{	reassigncontrols (hrows[i], &dyfilter[curline-MAXSAMPLE-MAXANALOG].curfpattern->freqbase[i+currowoffset]);
					reassoccontrols (hrows[i], hfpattern[curline-MAXSAMPLE-MAXANALOG]);
				}
			}
		}
	}
	return 0;
}

void init_volumeandbalance (void)
{	char *surprise = "surprising ! big headeach to come ! let's try the common issue...\n";
	int l;

	if ((l = register_levels_as (0, " output ", &vlevel[0])) != 0)
	{	fprintf (stderr, "%s", surprise);
		l = register_levels (" output ", &vlevel[0]);
	}
	show_levels (0, l);
	if ((l = register_levels_as (1, "  pan   ", &vpan[0])) != 1)
	{	fprintf (stderr, "%s", surprise);
		l = register_levels ("  pan   ", &vpan[0]);
	}
	show_levels (1, l);
}

void initboard (char *ver)
{
    int	i, 
	voice = 0;

	version = ver;
	clearscreen ();

	radiorow.nb = 0;
	radiorow.value = 0;	/* JDJDJDJD a voir.... */
	currowoffset = 0;

	init_sequence ();

	/* c'est a faire avant les construction de pattern pour que 
	   les associations hpattern/group hrow marchent 		*/
	hpatternrow = createrange  (CONTRNUMRANGE, ROWNBROW, LINELEGEND, 0, CONTUNDEF, &pattern->nbrow, 1, MAXPATTERNROW);
	for (i=0 ; i<MAXDISPLAYROW ; i++)
		hrows[i] = createcontrols (CONTRVSLIDE, ROWRYTHM+i, LINEDIV, 0, CONTUNDEF, NULL);
	hgrows = creategroup (CONTRGROUP, 0, CONTUNDEF, MAXDISPLAYROW, hrows);
	curline = 0;

	initlevels ();
	voice = init_pattern (curpattern, voice);
	voice = init_analogik (voice);


	flapmenu = BUT_OFF;
	hflapmenu = 
	createbutton (CONTRBUTTON,   ROWNAME,  LINELEGEND,  '#', CONTUNDEF, &flapmenu, 3,
			"\033[1m\033[42m\033[33m o \033[m",
			"\033[1m\033[41m\033[33m x \033[m",
			"\033[43m\033[30m---\033[m", NULL, NULL, NULL);

	rawing2disk = BUT_DIS;
        hrawing2disk =
	createbutton (CONTRBUTTON, ROWNAME+4,  LINEDIV,   'r', CONTUNDEF, &rawing2disk, 4,
			"\033[37mRaw\016`\017",
			"\033[1m\033[37mRaw\033[31m\016`\017",
			"    ", NULL, NULL, NULL);

	opendsp = BUT_DIS;
        hopendsp =
	createbutton (CONTRBUTTON, ROWNAME+4,  LINEDIV+1,   'T', CONTUNDEF, &opendsp, 4,
			"\033[37mDsp\016`\017",
			"\033[1m\033[37mDsp\033[31m\016`\017",
			"    ", NULL, NULL, NULL);


/*	loading = BUT_OFF;	*/
#ifdef RINGMODULATOR
	saving = BUT_OFF;
        createbutton (CONTRBUTTON, ROWNAME+9,  LINEDIV,    's', CONTUNDEF, &saving, 1,
			"\033[37mS",
			"\033[1m\033[37mS",
			" ", NULL, NULL, NULL);
#endif

	isplayin = BUT_ON;
        createbutton (CONTRBUTTON, ROWNAME+9, LINEDIV+1,   'P', CONTUNDEF, &isplayin, 1,
			"\033[37mP",
			"\033[1m\033[37mP",
			" ", NULL, NULL, NULL);

	createrange  (CONTRNUMRANGE, ROWREV+2, LINEDIV+1,   0, CONTUNDEF, &bpm, MINBPM, MAXBPM);

	goleft = BUT_OFF;
	goright = BUT_OFF;
	hgoleft =
	createbutton (CONTRBUTTON, ROWRYTHM, LINELEGEND, '(', CONTUNDEF, &goleft, 2,
			"\033[41m\033[1m\033[33m«-\033[m",
			"\033[42m\033[1m\033[33m«-\033[m",
			"\033[41m\033[1m\033[33m--\033[m",
			NULL, NULL, NULL);
	hgoright =
// JDJD 2001-04-21
//	createbutton (CONTRBUTTON, ROWLEVEL-3, LINELEGEND, '(', CONTUNDEF, &goright, 2,
	createbutton (CONTRBUTTON, ROWRYTHM+4, LINELEGEND, '(', CONTUNDEF, &goright, 2,
			"\033[41m\033[1m\033[33m-»\033[m",
			"\033[42m\033[1m\033[33m-»\033[m",
			"\033[41m\033[1m\033[33m--\033[m",
			NULL, NULL, NULL);

	hduppattern = createbutton (CONTRBUTTON, ROWNAME, LINEDIV+1, '&', CONTUNDEF, &duppattern, 4,
			"\033[37m\016`\017dup",
			"\033[1m\033[31m\016`\017\033[37mdup",
			"    ", NULL, NULL, NULL);
	duppattern = BUT_OFF;

	init_delay ();
	voice = init_dyfilter (voice);
	
	init_deriv ();

	init_volumeandbalance ();

	resync_disp_levels ();
	actionnecontexts (curcontext, Aactionboard);
}
