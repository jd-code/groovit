/* 
 * $Id: grdyfilter.c,v 1.2 2003/03/19 16:45:47 jd Exp $
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
 * $Log: grdyfilter.c,v $
 * Revision 1.2  2003/03/19 16:45:47  jd
 * major changes everywhere while retrieving source history up to this almost final release
 *
 * Revision 1.2.0.24.0.3  2000/10/01 21:07:03  jd
 * I dunno
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
 * Revision 1.2.0.23.0.4  1999/10/07 14:08:05  jd
 *
 * Revision 1.2.0.23.0.2  1999/09/27 00:46:09  jd
 * added multiple pattern (was promised from so long !)
 * added multiple dy-filter
 * corrected with API
 *
 * Revision 1.2.0.23  1999/09/15 10:20:39  jd
 * second pre tel-aviv public revision, for testing
 *
 * Revision 1.2.0.22.0.3  1999/09/15 09:23:30  jd
 * new module extracted from main
 * amplitude control missing
 * added handling of dynamic from samples
 *
 *
 */

#include <stdio.h>

#include "config.h"

#include "controls.h"
#include "sample.h"
#include "placement.h"
#include "groovit.h"
#include "menu_board.h"
#include "grmixeur.h"
#include "grlevels.h"
#include "granalogik.h"
#include "grevents.h"

#define INMAINGRDYFILTER
#include "grdyfilter.h"

void    initDyFilter (DyFilter *p)
{
    initPatbut (&p->patbut, -1, -1, -1);    /* initialisation a partir de p->patbut.ncurpattern */
    p->curfpattern = &fpattern[p->patbut.ncurpattern];
    initAnReso (&p->reso);
    p->patbut.curnbrow = p->curfpattern->nbrow;

    p->genfreq = 0;
    p->freqbase = 0;		/* JDJDJDJD a verifier ??? */
    p->freqbase2 = 0;		/* JDJDJDJD a verifier */
    p->ctfreqbase2 = 0;		/* JDJDJDJD a verifier */
}

void    reset_dyfilter (void)
{
    int     i;

    for (i = 0; i < MAXDYFILT; i++)
    {
	initAnalogik (&filter);	/* JDJDJDJD reecrire cette partie !! */
	initDyFilter (&dyfilter[i]);
    }

    for (i = 0; i < MAXPATTERN; i++)
	initAPattern (&fpattern[i]);

    fpattern[0].isnew = 0;
}

void    refreshfpatternrow (int i)
{
    refreshpfield (hfpattern[i], dyfilter[i].curfpattern->nbrow - currowoffset);
}

/* 
 * ---------------------------------- Generation Part --------------------------------
 */

static int *mixl[MAXDYFILT],	/* pointeurs sur les summerisations de mixeurs */
       *mixr[MAXDYFILT], *mixm[MAXDYFILT];

int     tickchange_dyfilter (int voice)
{
    int     i;

    for (i = 0; i < MAXDYFILT; i++, voice++)
    {
	DyFilter *fil = &dyfilter[i];

	if (fil->curfpattern->freqbase[fil->patbut.tp] != 0)
	{
	    fil->reso.ampmode = 0;
	    fil->reso.freq = 10 + tabfreq[fil->reso.ctfreqcut2] + (((tabfreq[fil->reso.ctfreqcut] - tabfreq[fil->reso.ctfreqcut2]) * fil->reso.amp) >> 16);
	  /* conserve au cas ou celle de ci-dessus serait mauvaise */
	  /* fil->reso.freq =
	     10+tabfreq[fil->reso.freqcut2]+(((tabfreq[fil->reso.freqcut]-tabfreq[fil->reso.freqcut2]) 

	     * fil->reso.amp) >> 16); */
	}

	flevell[i] = (vlevel[voice] * (256 - vpan[voice])) >> 6;	/* JDJDJD 6 au lieu
									   de 8 */
	flevelr[i] = (vlevel[voice] * vpan[voice]) >> 6;

	fil->patbut.tp++;
	if (fil->patbut.tp == fil->patbut.curnbrow)	/* la on passe a la fin d'une fpattern */
	{
	    fil->patbut.tp = 0;

	    if (fil->patbut.nncurpattern != fil->patbut.ncurpattern)
	    {
		fil->patbut.ncurpattern = fil->patbut.nncurpattern;
		if ((fpattern[fil->patbut.ncurpattern].isnew) || duppattern)
		{
		    int     i;

		    if (duppattern)
		    {
			duppattern = 0;
			refreshcontrols (hduppattern);
		    }

		    fpattern[fil->patbut.ncurpattern].nbrow = fil->curfpattern->nbrow;
		    fpattern[fil->patbut.ncurpattern].isnew = 0;
		    for (i = 0; i < MAXPATTERNROW; i++)
			fpattern[fil->patbut.ncurpattern].freqbase[i] = fil->curfpattern->freqbase[i];
		}
		fil->curfpattern = &fpattern[fil->patbut.ncurpattern];
		fil->patbut.curnbrow = fil->curfpattern->nbrow;
/* A reorganiser */ reassignfpattern (i, fil->curfpattern);
	    }
	    else if (fil->patbut.curnbrow != fil->curfpattern->nbrow)
	    {
		fil->patbut.curnbrow = fil->curfpattern->nbrow;
		refreshfpatternrow (i);
	    }
	}
    }
    return voice;
}
void abitr_dyfilter (void)
{   int i;

    for (i = 0; i < MAXDYFILT; i++)
    {
	DyFilter *fil = &dyfilter[i];

	initPatbut (&fil->patbut, i, 0, fpattern[i].nbrow);
#ifdef DEBUGANVOICEINIT
	fprintf (stderr, "dyfilter [%2d ] = %d \n", i, fil->ncurpattern);
#endif
	fil->curfpattern = &fpattern[fil->patbut.ncurpattern];
	fil->curfpattern->isnew = 0;
    }
}

int     generation_dyfilter (int *outl, int *outr, int voice)
{
    static int ltmp, rtmp, m;
    int     i;

    for (i = 0; i < MAXDYFILT; i++, voice++)
    {
	DyFilter *fil = &dyfilter[i];

#ifndef RESOFILTER
	fil->reso.feedbk = (fil->reso.mid * (1 + fil->reso.ctreso)) >> 8;

/* fil->reso.hig = (((*mixm[i]*fil->reso.amp)>>16) - fil->reso.feedbk - fil->reso.low)>>2; */
	fil->reso.hig = (*mixm[i] - fil->reso.feedbk - fil->reso.low) >> 2;	/* JDJDJD
										   essayer
										   sans amp */
	fil->reso.mid += (fil->reso.hig * fil->reso.freq) >> RESOFREQCUT;
	if (fil->reso.mid > 32768)
	    fil->reso.mid = 32768;
	else if (fil->reso.mid < -32768)
	    fil->reso.mid = -32768;

	fil->reso.low += (fil->reso.mid * fil->reso.freq) >> RESOFREQCUT;
/* ici y'a encore un amp... */
/* fil->reso.hig2 = (((fil->reso.mid*fil->reso.amp)>>16) - fil->reso.mid2 -
   fil->reso.low2)>>2;  */
	fil->reso.hig2 = (fil->reso.mid - fil->reso.mid2 - fil->reso.low2) >> 2;
	fil->reso.mid2 += (fil->reso.hig2 * fil->reso.freq) >> RESOFREQCUT;
	fil->reso.low2 += (fil->reso.mid2 * fil->reso.freq) >> RESOFREQCUT;

	if (fil->reso.low2 > 32768)
	    fil->reso.low2 = 32768;
	else if (fil->reso.low2 < -32768)
	    fil->reso.low2 = -32768;

	ltmp = (fil->reso.low2 * flevell[i]) >> 8;
	rtmp = (fil->reso.low2 * flevelr[i]) >> 8;
	*outl += ltmp;
	*outr += rtmp;
	for (m = 0; m < nbmixeurused; m++)
	{
	    mixeur[m].m += ((long) fil->reso.low2 * mixeur[m].level[voice]) >> 8;
	    mixeur[m].l += ((long) ltmp * mixeur[m].level[voice]) >> 8;
	    mixeur[m].r += ((long) rtmp * mixeur[m].level[voice]) >> 8;
	}
#else
	fil->reso.freq = 10 + tabfreq[ctFfreqcut];
	fil->reso.mid = ((fil->reso.mid * (65536 - fil->reso.freq)) >> 16) + ((*mixm[i] * fil->reso.freq) >> 16);

	ltmp = (fil->reso.mid * flevell[i]) >> 8;
	rtmp = (fil->reso.mid * flevelr[i]) >> 8;
	*outl += ltmp;
	*outr += rtmp;
	for (m = 0; m < nbmixeurused; m++)
	{
	    mixeur[m].m += ((long) fil->reso.mid * mixeur[m].level[voice]) >> 8;
	    mixeur[m].l += ((long) ltmp * mixeur[m].level[voice]) >> 8;
	    mixeur[m].r += ((long) rtmp * mixeur[m].level[voice]) >> 8;
	}
#endif
	*mixl[i] = *mixr[i] = *mixm[i] = 0;
    }

    return voice;
}

int genfreeze_dyfilter	(int voice)
{
    int     j;
    DyFilter *fil;

    for (j = 0; j < MAXDYFILT; j++, voice++)
    {
	fil = &dyfilter[j];
	simple_pression (EV_DYFI_F1, voice, fil->reso.ctfreqcut);
	simple_pression (EV_DYFI_F2, voice, fil->reso.ctfreqcut2);
	simple_pression (EV_DYFI_RZ, voice, fil->reso.ctreso);
	simple_pression (EV_DYFI_DK, voice, fil->reso.ctdecay);

	simple_pression (EV_DYFI_PI, voice, fil->patbut.ncurpattern);
	simple_pression (EV_DYFI_PN, voice, fil->patbut.tp);
    }
    return (voice);
}

int play_event_dyfilter (Event * ev)
{
#ifdef DEBUGVOICEMATCH
    if ((ev->voice < MAXSAMPLE+MAXANALOG) || (ev->voice > MAXSAMPLE+MAXANALOG+MAXDYFILT))
    {	fprintf (stderr, "error: event type %04x voice %d sent to dyn.filter\n",
		ev->type, ev->voice);
	return -1;
    }
#endif
    /* JDJDJDJD on suppose que le tri des mauvaises voies a été fait */
    /* en plus il faudrait faire bouger eventuellement les controles correspondants ? */
    switch (ev->type)
    {
	case EV_DYFI_F1:
	    dyfilter[ev->voice-MAXSAMPLE].reso.ctfreqcut = ev->val;
	    return 0;
	case EV_DYFI_F2:
	    dyfilter[ev->voice-MAXSAMPLE].reso.ctfreqcut2= ev->val;
	    return 0;
	case EV_DYFI_RZ:
	    dyfilter[ev->voice-MAXSAMPLE].reso.ctreso = ev->val;
	    return 0;
	case EV_DYFI_DK:
	    dyfilter[ev->voice-MAXSAMPLE].reso.ctdecay = ev->val;
	    return 0;
	case EV_DYFI_PT:
//	    dyfilter[ev->voice-MAXSAMPLE].patbut.nncurpattern = ev->val;
	    refrsyncPatbut (gpatbut[ev->voice], ev->val);
	    return 0;
	case EV_DYFI_PI:
//	    dyfilter[ev->voice-MAXSAMPLE].patbut.nncurpattern = ev->val;
	    refrsyncPatbut (gpatbut[ev->voice], ev->val);
	    initPatbut (gpatbut[ev->voice], -2, -1, -1);
	    dyfilter[ev->voice-MAXSAMPLE].curfpattern = &fpattern[ev->val];
	    return 0;
	case EV_DYFI_PN:
	    dyfilter[ev->voice-MAXSAMPLE].patbut.tp = ev->val;
	    return 0;
	default:
	    fprintf (stderr, "warning: unknown event type %04x in play_event_dyfilter\n", 
		    ev->type);
	    return -1;
    }
}
int     init_dyfilter (int voice)
{
    int     pos, j;
    DyFilter *fil;
    char    nom[9];

    for (j = 0; j < MAXDYFILT; j++, voice++)
    {
	pos = 2 * j + LINEFILTER;
	fil = &dyfilter[j];

	setpression (createcontrols (CONTRHSLIDE, ROWFREQ1, pos,
				     'f', CONTUNDEF, &fil->reso.ctfreqcut),
		     simple_pression, EV_DYFI_F1, voice);

	setpression (createcontrols (CONTRHSLIDE, ROWFREQ2, pos,
				     'f', CONTUNDEF, &fil->reso.ctfreqcut2),
		     simple_pression, EV_DYFI_F2, voice);

	setpression (createcontrols (CONTRHSLIDE, ROWRESO, pos,
				     'f', CONTUNDEF, &fil->reso.ctreso),
		     simple_pression, EV_DYFI_RZ, voice);

	setpression (createcontrols (CONTRHSLIDE, ROWDECAY, pos,
				     'f', CONTUNDEF, &fil->reso.ctdecay),
		     simple_pression, EV_DYFI_DK, voice);

	{
	    int     j;

	    for (j = 0; j < MAXDISPLEVELS; j++)
	    {
		hlevels[j][voice] = createcontrols (CONTRHSLIDE, ROWLEVEL + j * 9, pos + 1, 0, CONTUNDEF, NULL);
	    }
	}

	hfpattern[j] = createpfield (CONTRPFIELD, ROWRYTHM, pos + 1, 0, hgrows, &fpattern->freqbase, MAXDISPLAYROW, fil->curfpattern->nbrow, &radiorow);
	hfpatternrow[j] = createrange (CONTRNUMRANGE, ROWNBROWBIS, pos, 0, CONTUNDEF, &fil->curfpattern->nbrow, 1, MAXPATTERNROW);

	createpatbut (&fil->patbut, voice, pos, EV_DYFI_PT);

	if (MAXDYFILT > 1)
	    sprintf (nom, "dy-filt%c", '0' + j);
	else
	    sprintf (nom, "dy-filt ");
	registertomixeur (&mixl[j], &mixr[j], &mixm[j], nom, (j == 0) ? 3 : -1);	
/* JDJDJDJD a corriger */
	if (j == 0)
	    show_levels (3, 3);
    }

    return voice;
}
