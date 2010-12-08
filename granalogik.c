/* 
 * $Id: granalogik.c,v 1.2 2003/03/19 16:45:47 jd Exp $
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
 * $Log: granalogik.c,v $
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
 * Revision 1.2.0.23  1999/09/15 10:20:39  jd
 * second pre tel-aviv public revision, for testing
 *
 * Revision 1.2.0.22.0.4  1999/09/15 09:23:30  jd
 * corrected for "voice" use
 *
 *
 */

#include <stdio.h>
#include <math.h>

#include "config.h"

#include "controls.h"
#include "sample.h"
#include "placement.h"
#include "grlevels.h"
#include "grmixeur.h"
#include "groovit.h"
#include "menu_board.h"
#include "grsequence.h"
#include "grevents.h"
#define INMAINGRAPATT
#include "granalogik.h"

/* 
 * ---------------------------------- Variables privees ------------------------------
 */
static short qcos[0x8000];	/* tableau de sinus rapide en 1 JHz             */

					/* JDJDJDJD devrait passer par un define        */

/* 
 * ---------------------------------- Screen Part ------------------------------------
 */

void    constructapattern (APattern *apattern, int i, int voice)
{
    int     ligne = 2 * i + LINEANALOG, j;

    for (j = 0; j < MAXDISPLEVELS; j++)
    {
	hlevels[j][voice] = createcontrols (CONTRHSLIDE, ROWLEVEL + j * 9, ligne + 1, 0, CONTUNDEF, NULL);
    }
    hapattern[i] = createpfield (CONTRPFIELD, ROWRYTHM, ligne + 1, 0, hgrows, &apattern->freqbase[0], MAXDISPLAYROW, apattern->nbrow, &radiorow);
    hapatternrow[i] = createrange (CONTRNUMRANGE, ROWNBROWBIS, ligne, 0, CONTUNDEF, &apattern->nbrow, 1, MAXPATTERNROW);
}

int     init_analogik (int voice)
{
    int     pos, j, i;
    AnVoice *anv;

    for (i = 0; i < 0x8000; i++)
	qcos[i] = 0x7FFF * cos ((3.141592654 * 2 * i) / 0x8000);
  /* JDJDJD square generator */
    for (i = 0; i < 0x8000; i++)
	qcos[i] = ((i < (0x8000 >> 1)) ? 1 : -1) * 0x7FFF;
  /* JDJDJD square generator */
    for (i = 0; i < 0x8000; i++)
	qcos[i] = (int) ((double) 0x10000 * ((double) i / (double) 0x8000) - (double) 0x7FFF);
  /* JDJDJD bi-square generator */
//    for (i = 0; i < 0x8000; i++)
//	qcos[i] = (int) ((double) 0x80000 * ((double) i / (double) 0x8000) - (double) 0x7FFF) +
//		    (((i % 0x7FFF) > 0x3FFF) ? 1 : -1) * 0x3FFF;


    for (j = 0; j < MAXANALOG; j++, voice++)
    {
	pos = 2 * j + LINEANALOG;
	anv = &anvoice[j];

	setpression (createcontrols (CONTRHSLIDE, ROWFREQ1, pos,
				     'f', CONTUNDEF, &anv->reso.ctfreqcut),
		     simple_pression, EV_ANVO_F1, voice);

	setpression (createcontrols (CONTRHSLIDE, ROWFREQ2, pos,
				     'f', CONTUNDEF, &anv->reso.ctfreqcut2),
		     simple_pression, EV_ANVO_F2, voice);

	setpression (createcontrols (CONTRHSLIDE, ROWRESO, pos,
				     'f', CONTUNDEF, &anv->reso.ctreso),
		     simple_pression, EV_ANVO_RZ, voice);

	setpression (createcontrols (CONTRHSLIDE, ROWDECAY, pos,
				     'f', CONTUNDEF, &anv->reso.ctdecay),
		     simple_pression, EV_ANVO_DK, voice);

	createpatbut (&anv->patbut, voice, pos, EV_ANVO_PT);

	constructapattern (anv->curapattern, j, voice);
    }
    return (voice);
}

int genfreeze_analogik	(int voice)
{
    int     j;
    AnVoice *anv;

    for (j = 0; j < MAXANALOG; j++, voice++)
    {
	anv = &anvoice[j];
	simple_pression (EV_ANVO_F1, voice, anv->reso.ctfreqcut);
	simple_pression (EV_ANVO_F2, voice, anv->reso.ctfreqcut2);
	simple_pression (EV_ANVO_RZ, voice, anv->reso.ctreso);
	simple_pression (EV_ANVO_DK, voice, anv->reso.ctdecay);

	simple_pression (EV_ANVO_PI, voice, anv->patbut.ncurpattern);
	simple_pression (EV_ANVO_PN, voice, anv->patbut.tp);
    }
    return (voice);
}

int play_event_analogik (Event * ev)
{
#ifdef DEBUGVOICEMATCH
    if ((ev->voice < MAXSAMPLE) || (ev->voice > MAXSAMPLE+MAXANALOG))
    {	fprintf (stderr, "error: event type %04x voice %d sent to analogik\n",
		ev->type, ev->voice);
	return -1;
    }
#endif
    /* JDJDJDJD on suppose que le tri des mauvaises voies a été fait */
    /* en plus il faudrait faire bouger eventuellement les controles correspondants ? */
    switch (ev->type)
    {
	case EV_ANVO_F1:
	    anvoice[ev->voice-MAXSAMPLE].reso.ctfreqcut = ev->val;
	    return 0;
	case EV_ANVO_F2:
	    anvoice[ev->voice-MAXSAMPLE].reso.ctfreqcut2= ev->val;
	    return 0;
	case EV_ANVO_RZ:
	    anvoice[ev->voice-MAXSAMPLE].reso.ctreso = ev->val;
	    return 0;
	case EV_ANVO_DK:
	    anvoice[ev->voice-MAXSAMPLE].reso.ctdecay = ev->val;
	    return 0;
	case EV_ANVO_PT:
//	    anvoice[ev->voice-MAXSAMPLE].patbut.nncurpattern = ev->val;
	    refrsyncPatbut (gpatbut[ev->voice], ev->val);
//	    initPatbut (gpatbut[voice], -2, -1, -1);
	    return 0;
	case EV_ANVO_PI:
//	    anvoice[ev->voice-MAXSAMPLE].patbut.nncurpattern = ev->val;
	    refrsyncPatbut (gpatbut[ev->voice], ev->val);
	    initPatbut (gpatbut[ev->voice], -2, -1, -1);
	    anvoice[ev->voice-MAXSAMPLE].curapattern = &apattern[ev->val];
	    return 0;
	case EV_ANVO_PN:
	    anvoice[ev->voice-MAXSAMPLE].patbut.tp = ev->val;
	    return 0;
	default:
	    fprintf (stderr, "warning: unknown event type %04x in play_event_analogik\n", 
		    ev->type);
	    return -1;
    }
}
void abitr_analogik (void)
{   int i;

    for (i = 0; i < MAXANALOG; i++)
    {
	AnVoice *anv = &anvoice[i];

	initPatbut (&anv->patbut, i, 0, apattern[i].nbrow);
#ifdef DEBUGANVOICEINIT
	fprintf (stderr, "anvoice [%2d ] = %d \n", i, anv->ncurpattern);
#endif
	anv->curapattern = &apattern[anv->patbut.ncurpattern];
	anv->curapattern->isnew = 0;
    }
}
void    refreshapatternrow (int i)
{
    refreshpfield (hapattern[i], anvoice[i].curapattern->nbrow - currowoffset);
}

/* 
 * ---------------------------------- Generation Part --------------------------------
 */

void    reset_analogik (void)
{
    int     i;

    for (i = 0; i < MAXANALOG; i++)
    {
	initAnalogik (&analog[i]);
	anvoice[i].patbut.ncurpattern = 0;	/* JDJDJDJD LASTDEBUG */
	initAnVoice (&anvoice[i]);

    }
    for (i = 0; i < MAXPATTERN; i++)
	initAPattern (&apattern[i]);
    apattern[0].isnew = 0;
}

int     tickchange_analogik (int voice)
{
    int     i;

    for (i = 0; i < MAXANALOG; i++, voice++)
    {
	AnVoice *anv = &anvoice[i];

	if (anv->curapattern->freqbase[anv->patbut.tp] != 0)
	{
	    anv->reso.ampmode = 0;
	    anv->freqbase = tabfreq[anv->curapattern->freqbase[anv->patbut.tp]];
	    anv->freqbase2 = anv->curapattern->freqbase[anv->patbut.tp] - 64;
/* 1.2.0.20 JDJDJD      anv->freqbase2 = anv->freqbase + anv->ctfreqbase2; */
/* ici on squize-down tout ce qui pourrait poser des problemes ulterieurs de resonance ... */
/* ALhig = 0, ALmid = 0, ALlow = 0, ALfeedbk = 0, ALhig2 = 0, ALmid2 = 0, ALlow2 = 0;   */
#ifdef RELATFREQ
	    anv->reso.freq = 10 + tabfreq[anv->freqbase2 + anv->reso.ctfreqcut2] + (((tabfreq[anv->freqbase2 + anv->reso.ctfreqcut] - tabfreq[anv->freqbase2 + anv->reso.ctfreqcut2]) * anv->reso.amp) >> 16);
#else
	    anv->reso.freq = 10 + tabfreq[anv->reso.ctfreqcut2] + (((tabfreq[anv->reso.ctfreqcut] - tabfreq[anv->reso.ctfreqcut2]) * anv->reso.amp) >> 16);
#endif
/* ALfreq = 10+((tabfreq[ctfreqcut] * amp) >> 16); */
/* ALfreq = freqbase+ctfreqcut; */
/* ALfreq = tabfreq[ctfreqcut]; */
	}
	alevell[i] = (vlevel[voice] * (256 - vpan[voice])) >> 8;
	alevelr[i] = (vlevel[voice] * vpan[voice]) >> 8;
	arevlevel[i] = analog[i].revlevel;

	anv->patbut.tp++;
	if (anv->patbut.tp == anv->patbut.curnbrow)	/* la on passe a la fin d'une apattern */
	{
	    anv->patbut.tp = 0;

	    if (anv->patbut.nncurpattern != anv->patbut.ncurpattern)
	    {
		anv->patbut.ncurpattern = anv->patbut.nncurpattern;
		if ((apattern[anv->patbut.ncurpattern].isnew) || duppattern)
		{
		    int     i;

		    if (duppattern)
		    {
			duppattern = 0;
			refreshcontrols (hduppattern);
		    }

		    apattern[anv->patbut.ncurpattern].nbrow = anv->curapattern->nbrow;
		    apattern[anv->patbut.ncurpattern].isnew = 0;
		    for (i = 0; i < MAXPATTERNROW; i++)
			apattern[anv->patbut.ncurpattern].freqbase[i] = anv->curapattern->freqbase[i];
		}
		anv->curapattern = &apattern[anv->patbut.ncurpattern];
		anv->patbut.curnbrow = anv->curapattern->nbrow;
/* A reorganiser */ reassignapattern (i, anv->curapattern);
	    }
	    else if (anv->patbut.curnbrow != anv->curapattern->nbrow)
	    {
		anv->patbut.curnbrow = anv->curapattern->nbrow;
		refreshapatternrow (i);
	    }
	}
    }
    return voice;
}

int     generation_analogik (int *outl, int *outr, int voice)
{
    int     i,			/* compteur a tout faire                        */
            m;			/* compteur de canal de mixeur                  */
    long    mtmp,		/* temporaire pour l'injection en mono          */
            ltmp,		/* temporaire pour l'injection en stereo mixee  */
            rtmp;		/* temporaire pour l'injection en stereo mixee  */

/* RING MODULATOR TEST */
#ifdef RINGMODULATOR
    lring = 0x10000;
    rring = 0x10000;
#endif
    for (i = 0; i < MAXANALOG; i++, voice++)
    {
	AnVoice *anv = &anvoice[i];

	anv->genfreq += anv->freqbase;
	anv->genfreq &= 0x7FFF;

	anv->reso.feedbk = (anv->reso.mid * (1 + anv->reso.ctreso)) >> 8;

      anv->reso.hig = (((qcos[anv->genfreq] * anv->reso.amp) >> 16) - anv->reso.feedbk - anv->reso.low) >> 2;
	anv->reso.mid += (anv->reso.hig * anv->reso.freq) >> RESOFREQCUT;
	anv->reso.low += (anv->reso.mid * anv->reso.freq) >> RESOFREQCUT;

	anv->reso.hig2 = (((anv->reso.mid * anv->reso.amp) >> 16) - anv->reso.mid2 - anv->reso.low2) >> 2;
	anv->reso.mid2 += (anv->reso.hig2 * anv->reso.freq) >> RESOFREQCUT;
	anv->reso.low2 += (anv->reso.mid2 * anv->reso.freq) >> RESOFREQCUT;

	if (anv->reso.low2 > 32768)
	    anv->reso.low2 = 32768;
	else if (anv->reso.low2 < -32768)
	    anv->reso.low2 = -32768;

	mtmp = anv->reso.low2;
	ltmp = (mtmp * alevell[i]) >> 8;
	rtmp = (mtmp * alevelr[i]) >> 8;
/* RING MODULATOR TEST */
#ifdef RINGMODULATOR
	if (saving == BUT_OFF)
	{
#endif
	    *outl += ltmp;
	    *outr += rtmp;
	    for (m = 0; m < nbmixeurused; m++)
	    {
		mixeur[m].m += ((long) mtmp * mixeur[m].level[voice]) >> 8;
		mixeur[m].l += ((long) ltmp * mixeur[m].level[voice]) >> 8;
		mixeur[m].r += ((long) rtmp * mixeur[m].level[voice]) >> 8;
	    }
#ifdef RINGMODULATOR
	}
	else
	{
	    if (i == 0)
	    {
		lring = anv->reso.low2;
		rring = anv->reso.low2;
	    }
	    else
	    {
		lring *= ltmp;
		lring >>= 16;
		rring *= rtmp;
		rring >>= 16;
	    }
	}
#endif
    }

#ifdef RINGMODULATOR
    if (saving == BUT_ON)
    {
	*outl += lring;
	*outr += rring;
    }
#endif
    return voice;
}

/* 
 * ---------------------------------- Content Part -----------------------------------
 */

void    initAPattern (APattern *p)
{
    int     i;

    p->isnew = 1;
    p->nbrow = MAXDISPLAYROW;	/* arbitrary fit the display... should be in .rcfile */
    for (i = 0; i < MAXPATTERNROW; i++)
	p->freqbase[i] = 0;
}
void    initAnVoice (AnVoice *p)
{
    initPatbut (&p->patbut, -1, -1, -1);    /* initialisation a partir de p->patbut.ncurpattern */
    p->curapattern = &apattern[p->patbut.ncurpattern];
    initAnReso (&p->reso);
    p->genfreq = 0;
    p->freqbase = 0;		/* JDJDJDJD a verifier ??? */
    p->freqbase2 = 0;		/* JDJDJDJD a verifier */
    p->ctfreqbase2 = 0;		/* JDJDJDJD a verifier */
}
