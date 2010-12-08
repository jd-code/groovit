/* 
 * $Id: grpattern.c,v 1.2 2003/03/19 16:45:47 jd Exp $
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
 * $Log: grpattern.c,v $
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
 * suppressed decorum strings
 * debugged from unasigned value at start in dyfilter vslides
 * corrected for levels
 *
 * Revision 1.2.0.22.0.2  1999/09/05 22:30:16  jd
 * more code imported from old main
 *
 * Revision 1.2.0.22.0.1  1999/08/30 22:45:51  jd
 * Revision 1.1  1999/08/30 22:41:12  jd
 * Initial revision
 *
 */

#include <stdio.h>
#include <string.h>

#include "config.h"

#define INMAINGRPATT

#include "controls.h"
#include "sample.h"
#include "readsample.h"
#include "placement.h"
#include "groovit.h"
#include "grlevels.h"
#include "grmixeur.h"
#include "menu_board.h"
#include "grevents.h"

#include "grpattern.h"

static char *nom[] =
{			       /* les fichiers de sample defaut */
    "808CH.WAV",
    "CAVEBD.WAV",
    "909RIDE.WAV",
    "CLAVES.WAV",
    "FNGRSNAP.WAV",
    "VIBRASLP.WAV",
    "626SHAKE.WAV",
    "STIK.WAV",
    "909CLAP.WAV",
    "KICK_11J.WAV",
    "KICK_11K.WAV",
    "SNAR_13C.WAV",
    "SNAR_13D.WAV",
    "SNAR_13H.WAV",
    "CLAP01.WAV",
    "KICK_11G.WAV",
    NULL
};

int genfreeze_pattern	(int voice)
{
    simple_pression (EV_RYVO_PI, voice, pattpatbut.ncurpattern);
    simple_pression (EV_RYVO_PN, voice, pattpatbut.tp);

    return (voice + MAXSAMPLE);
}

int play_event_pattern (Event * ev)
{
#ifdef DEBUGVOICEMATCH
    if (ev->voice != 0)
    {	fprintf (stderr, "error: event type %04x voice %d sent to rythm pattern\n",
		ev->type, ev->voice);
	return -1;
    }
#endif
    /* JDJDJDJD on suppose que le tri des mauvaises voies a été fait */
    /* en plus il faudrait faire bouger eventuellement les controles correspondants ? */
    switch (ev->type)
    {
	case EV_RYVO_PT:
//	    pattpatbut.nncurpattern = ev->val;
	    refrsyncPatbut (gpatbut[0], ev->val);
	    return 0;
	case EV_RYVO_PN:
	    pattpatbut.tp = ev->val;
//	    initPatbut (&pattpatbut, -2, -1, -1);
	    return 0;
	case EV_RYVO_PI:
//	    pattpatbut.nncurpattern = ev->val;
	    refrsyncPatbut (gpatbut[0], ev->val);
	    initPatbut (&pattpatbut, -2, -1, -1);
	    curpattern = &pattern[pattpatbut.ncurpattern];
	    return 0;
	default:
	    fprintf (stderr, "warning: unknown event type %04x in play_event_pattern\n", 
		    ev->type);
	    return -1;
    }
}
/* 
 * ---------------------------------- Screen Part ------------------------------------
 */
int     init_pattern (Pattern *pattern, int voice)
{
/* char buf[132];        JDJDJDJD verifier la taille (ici elle est arbitraire) */
    int     i, j;

    createpatbut (&pattpatbut, voice, LINEDIV+2, EV_RYVO_PT);

    createbutton (CONTRBUTTON, ROWRYTHM+4, LINELEGEND, '(', CONTUNDEF, &goright, 2,
		    "\033[41m\033[1m\033[33mA0\033[m",
		    "\033[42m\033[1m\033[33mA0\033[m",
		    "\033[41m\033[1m\033[33m--\033[m",
		    NULL, NULL, NULL);

    radiosample.nb = 0;
    radiosample.value = -1;
    for (i = 0; i < MAXSAMPLE; i++, voice++)
    {
	hbt_sample[i] = createsimplebutton (CONTRRADIO, ROWNAME, LINERYTHM + i, '@', CONTNOCOL + CONTNOPUCE, CONTUNDEF, &radiosample, sample[i].shortname);

	for (j = 0; j < MAXDISPLEVELS; j++)
	{
	    hlevels[j][voice] = createcontrols (CONTRHSLIDE, ROWLEVEL + j * 9, LINERYTHM + i, 0, CONTUNDEF, NULL);
	}


	hpattern[i] = createpfield (CONTRPFIELD, ROWRYTHM, LINERYTHM + i, 0, hgrows, &pattern->start[i][0], MAXDISPLAYROW, pattern->nbrow, &radiorow);
    }

#ifdef TESTSCORE
    createscore (CONTNOTEEDIT, ROWRYTHM, LINERYTHM, 0, hgrows, &pattern->start[i][0], MAXDISPLAYROW, pattern->nbrow, MAXSAMPLE, NULL);
#endif
/* generation de l'echelle a gauche (supprimee par manque de place  a partir de v1.2.0.7 */
/* sprintf (buf, "\033[2m\033[%d;%dH0\016k\033[B\010u\033[B\010u\033[B\010j\017",
   MAXSAMPLE+2*MAXANALOG+4 , SHORTNAMESIZE); addstrback (buf); */
    return voice;
}

void    refreshpatternrow (void)
{
    int     i;

    for (i = 0; i < MAXSAMPLE; i++)
	refreshpfield (hpattern[i], curpattern->nbrow - currowoffset);
}

/* 
 * ---------------------------------- Generation Part --------------------------------
 */

void    reset_pattern (void)
{
    int     i;

  /* --------- chargement des samples ------------------------------------------ */
    for (i = 0; i < MAXSAMPLE; i++)
	initsample (&sample[i]);
    {
	int     i = 0, nb = 0;
	char    buf[MAXDIRSIZE];

      /* JDJDJDJD remplacer par la lecture d'un .groovitrc */
	while (nom[nb] != NULL)
	    nb++;

	for (i = 0; (i < nb) && (i < MAXSAMPLE); i++)
	{
	    strcpy (buf, SAMPLEPATH);
	    strcat (buf, nom[i]);
	    if (readsample (buf, &sample[i]) == NULL)
		freesample (&sample[i]);
	    relooksimplebutton (hbt_sample[i], '@', CONTBLACK + CONTNOPUCE, sample[i].shortname);
	    vlevel[i] = 0;
/* trick again ... */ if (i)
		vpan[i] = 0xFF & (int) ((255.0 * i) / ((nb > MAXSAMPLE) ? MAXSAMPLE : nb));
	    else
		vpan[i] = 127;
	    sample[i].revlevel = 0;
	    sample[i].filtl = 0;
	}
    }

  /* ---------------- initialisation des patterns -------------------------------- */
    {
	int     i;

	for (i = 0; i < MAXPATTERN; i++)
	{
	    cleanPattern (&pattern[i]);
	    sprintf (pattern[i].name, "%-23d", i);
	}
	pattern[0].isnew = 0;
    }
}

int     tickchange_pattern (int voice)
{
    int     i;

    for (i = 0; i < MAXSAMPLE; i++, voice++)
	if (curpattern->start[i][pattpatbut.tp])
	{
	    cursamp[i] = sample[i].size;
	    psamp[i] = sample[i].data;
	    tlev[i] = curpattern->start[i][pattpatbut.tp];
	    levell[i] = ((long) vlevel[voice] * (256 - vpan[voice])) >> 8;
	    levelr[i] = ((long) vlevel[voice] * vpan[voice]) >> 8;
	}

    pattpatbut.tp++;
    if (pattpatbut.tp == pattpatbut.curnbrow)		/* la on passe a la fin d'une pattern */
    {
	pattpatbut.tp = 0;

	if (pattpatbut.nncurpattern != pattpatbut.ncurpattern)
	{
#ifdef DEBSEQU
fprintf (stderr, "rythm voice: entering pattern %d\n", pattpatbut.nncurpattern);
#endif
	    pattpatbut.ncurpattern = pattpatbut.nncurpattern;
	    if ((pattern[pattpatbut.ncurpattern].isnew) || duppattern)
	    {
		int     i, j;

		if (duppattern)
		{
		    duppattern = 0;
		    refreshcontrols (hduppattern);
		}

		pattern[pattpatbut.ncurpattern].nbrow = curpattern->nbrow;
		pattern[pattpatbut.ncurpattern].isnew = 0;
		for (i = 0; i < MAXPATTERNROW; i++)
		    for (j = 0; j < MAXSAMPLE; j++)
			pattern[pattpatbut.ncurpattern].start[j][i] = curpattern->start[j][i];
	    }
	    curpattern = &pattern[pattpatbut.ncurpattern];
	    pattpatbut.curnbrow = curpattern->nbrow;
/* A reorganiser */ reassignpattern (curpattern);
	}
	else if (pattpatbut.curnbrow != curpattern->nbrow)
	{
	    pattpatbut.curnbrow = curpattern->nbrow;
	    refreshpatternrow ();
	}
    }
    return voice;
}

int     generation_pattern (int *outl, int *outr, int voice)
{
    int     i,		       /* compteur a tout faire                        */
            m;		       /* compteur de canal de mixeur                  */
    long    mtmp,	       /* temporaire pour l'injection en mono          */
            ltmp,	       /* temporaire pour l'injection en stereo mixee  */
            rtmp;	       /* temporaire pour l'injection en stereo mixee  */

    for (i = 0; i < MAXSAMPLE; i++, voice++)
	if (cursamp[i])
	{
	    cursamp[i]--;
	    if (sample[i].nbch == 1)  /* sample mono */
	    {
		mtmp = ((long) (*(psamp[i]++)) * tlev[i]) >> 8;
		ltmp = ((long) mtmp * levell[i]) >> 8;
		rtmp = ((long) mtmp * levelr[i]) >> 8;
/* inclu dans cumul             FLtmp += ((long)(*(psamp[i]++))*filtl[i]) >>8; */
	    }
	    else		    /* sample stereo */
	    {	
		rtmp = ((long) (*(psamp[i]++)) * tlev[i]) >> 8;
		ltmp = ((long) (*(psamp[i]++)) * tlev[i]) >> 8;
		mtmp = (ltmp >> 1) + (rtmp >> 1);
		ltmp = ((long) ltmp * levell[i]) >> 8;
		rtmp = ((long) rtmp * levelr[i]) >> 8;
		
	    }
	    *outl += ltmp;
	    *outr += rtmp;

	    for (m = 0; m < nbmixeurused; m++)
	    {
		mixeur[m].m += ((long) mtmp * mixeur[m].level[voice]) >> 8;
		mixeur[m].l += ((long) ltmp * mixeur[m].level[voice]) >> 8;
		mixeur[m].r += ((long) rtmp * mixeur[m].level[voice]) >> 8;
	    }
	}

    return voice;
}
/* 
 * ---------------------------------- Content Part -----------------------------------
 */
void abitr_pattern (void)
{
    initPatbut (&pattpatbut, 0, 0, pattern[0].nbrow);
    curpattern = &pattern[pattpatbut.ncurpattern];
    curpattern->isnew = 0;
}

void    cleanPattern (Pattern *p)
{
    int     i, j;

    p->nbrow = MAXDISPLAYROW;	/* arbitrary fit the display... should be in .rcfile */
    p->isnew = 1;
    p->nextpat = -1;		/* let's say it points to ourselves... */
    p->nbrep = 1;
    strcpy (p->name, "");
    for (i = 0; i < MAXPATTERNROW; i++)
	for (j = 0; j < MAXSAMPLE; j++)
	    p->start[j][i] = 0;

}
