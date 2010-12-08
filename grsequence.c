/* 
 * $Id: grsequence.c,v 1.1 2003/03/19 21:56:36 jd Exp $
 * Groovit Copyright (C) 1999 Jean-Daniel PAUGET
 * making accurate and groovy sound/noise
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
 * $Log: grsequence.c,v $
 * Revision 1.1  2003/03/19 21:56:36  jd
 * small rcs to cvs typo corrected
 *
 * Revision 1.2.0.24.0.3  2000/10/01 21:07:03  jd
 * I dunno
 *
 * Revision 1.2.0.24.0.2  1999/10/19 19:33:49  jd
 * *** empty log message ***
 *
 * Revision 1.2  1999/10/19 19:32:42  jd
 * *** empty log message ***
 *
 * Revision 1.1  1999/10/19 19:31:35  jd
 * Initial revision
 *
 */

#include <stdlib.h>
#include <stdio.h>

#include "config.h"
#include "placement.h"
#include "controls.h"
#include "groovit.h"
#include "sample.h"

#include "grpattern.h"
#include "granalogik.h"
#include "grdyfilter.h"

#include "grevents.h"
#define INMAINSEQ
#include "grsequence.h"

EventBuf *curEVreg,		/* l'event buffer en cours d'enregistrement             */
       *initcurEVreg,		/* l'event buffer d'enregistrement il sera fusionne     */
       *curEVplay,		/* celui en cours de restitution   */
       *initcurEVplay;		/* l'event buffer de restitution il sera fusionne */

static long lasteventtime = 0;
static int istaping;		/* sommes-nous en train d'enegistrer ?                  */

int     simple_pression (int evtype, int voice, int data)
{
    Event   ev;

    if (istaping)
    {
	ev.dtime = t - lasteventtime;
	lasteventtime = t;
	ev.type = evtype;
	ev.voice = voice;
	ev.val = data;
	addevent (&curEVreg, &ev);
    }
    return 0;			/* JDJDJDJD pas d'API pour ça ? */
}

/* JDJDJDJD vaudrait mieux mettre ca au moment exact du changement de pattern, ou pas ?? */
int     pattern_pression (int evtype, int voice, int data)
{
    Event   ev;
    Patbut *pat = gpatbut[voice];

    pat->nncurpattern = (pat->radiobank.value << 3) + pat->radiopattern.value;

    if (istaping)
    {
	ev.dtime = t - lasteventtime;
	lasteventtime = t;
	ev.type = evtype;
	ev.voice = voice;
	ev.val = pat->nncurpattern;
	addevent (&curEVreg, &ev);
    }
    return 0;			/* JDJDJDJD pas d'API pour ça ? */
}


void    initPatbut (Patbut *pat, int ncurpattern, int tp, int curnbrow)
{
    if (ncurpattern == -1)	/* initialisation a partir de la valeur en cours */
	ncurpattern = pat->ncurpattern;
    else if (ncurpattern == -2)	/* initialisation a partir de la valeur prochaine */
    	ncurpattern = pat->nncurpattern;

    if (curnbrow != -1)
    {
	pat->curnbrow = curnbrow;	/* JDJDJD on fait aveuglement confiance sur ce coup
					   la */
	if (tp != -1)
	{   if (tp < curnbrow)
		pat->tp = tp;
	    else
	    {
		pat->tp = 0;
		fprintf (stderr, "warning: assigning tp=%d >= curnbrow=%d, ignired\n",
			 tp, curnbrow);
	    }
	}
    }
    else if (tp != -1)
	fprintf (stderr, "error: assigning tp without setting curnbrow !! call the U.N. !!\n");

    pat->nncurpattern = ncurpattern;
    pat->radiopattern.value = ncurpattern & 0x7;
    pat->radiobank.value = ncurpattern >> 3;
}

void refrsyncPatbut (Patbut *pat, int nncurpattern)
{   pat->nncurpattern = nncurpattern;
    pat->radiopattern.value = nncurpattern & 0x7;
    pat->radiobank.value = nncurpattern >> 3;
}

int     createpatbut (Patbut *pat, int voice, int pos, int evtype)
{
    int     ligne, colonne, i;
#ifdef DEBSEQU
fprintf (stderr, "voice %d creation du patbut...\n", voice);
#endif
    gpatbut[voice] = pat;
    pat->radiopattern.nb = 0;
    i = 0;
    for (ligne = 0; ligne < 2; ligne++)
	for (colonne = 0; colonne < 4; colonne++)
	    setpression (createsimplebutton (CONTRRADIO, 2 * colonne + ROWNAME, ligne + pos,
					     '1' + i++, CONTYELLOW, CONTUNDEF,
					     &pat->radiopattern, NULL),
			 pattern_pression, evtype, voice);

    pat->radiobank.nb = 0;
    i = 0;
    for (ligne = 0; ligne < 2; ligne++)
	for (colonne = 0; colonne < 2; colonne++)
/* JDJDJDJD vaudrait mieux mettre ca au moment exact du changement de pattern, ou pas ?? */
	    setpression (createsimplebutton (CONTRRADIO, 2 * colonne + 8 + ROWNAME, ligne + pos,
					     'A' + i++, CONTBLUE, CONTUNDEF,
					     &pat->radiobank, NULL),
			 pattern_pression, evtype, voice);
    return 0;
}

int     generate_freeze (void)
{
    int     voice = 0;

    voice = genfreeze_pattern (voice);
    voice = genfreeze_analogik (voice);
    voice = genfreeze_dyfilter (voice);

    return 0;
}

int     start_recording (void)
{
    initcurEVreg = evbuf_new (NULL);
    curEVreg = initcurEVreg;

    if (curEVreg == NULL)
    {
	fprintf (stderr, "error: could not start recording events, aborted\n");
	return -1;
    }
    return 0;
}

int     play_event (Event *ev)
{
    switch (ev->type & 0xFFF0)
    {
	case EV_ANVO_PT:
	case EV_ANVO:		/* base des evenements voie analogiques     */
	    return play_event_analogik (ev);
	case EV_DYFI_PT:
	case EV_DYFI:
	    return play_event_dyfilter (ev);
	case EV_RYVO_PT:
	    return play_event_pattern (ev);
	case EV_SIGNS:
	    fprintf (stderr, "debug: ignoring event type %04x\n", ev->type);
	    return 0;
	default:
	    fprintf (stderr, "warning: unknown event type %04x\n", ev->type);
	    return -1;
    }
}

static Event nplay;		/* prochain event a jouer */

void    tickchange_play (void)
{
    nplay.dtime--;
    if (nplay.dtime != 0)
	return;

    if (nplay.type != GREV_END)
	play_event (&nplay);
    else
    {
	isreplaying = 0;
	return;
    }

    while ((getnextevent (&curEVplay, &nplay) == 0) && (nplay.dtime == 0))
	play_event (&nplay);
}

int     start_playing (void)
{
    int     i;

#ifdef DEBSEQU
	fprintf (stderr, "-->start_playing\n");
#endif
    if (initcurEVplay == NULL)	/* il faut avoir qqchose a rejouer... */
    {
#ifdef DEBSEQU
	fprintf (stderr, "-->start_playing curEVplay == NULL on arrete de jouer\n");
#endif
	isreplaying = 0;
	return 0;
    }
    curEVplay = initcurEVplay;
    curEVplay->cur = 0;		/* JDJDJDJD un rewind violent */

#ifdef DEBSEQU
	fprintf (stderr, "-->start_playing debut lecture des freezes\n");
#endif
    while ((getnextevent (&curEVplay, &nplay) == 0) && (nplay.dtime == 0))	/* lire les freezes */
	play_event (&nplay);
#ifdef DEBSEQU
	fprintf (stderr, "-->start_playing fin lecture des freezes\n");
	fprintf (stderr, "-->start_playing debut resync patterns\n");
#endif
//    initPatbut (gpatbut[0], -2, -1, -1);
// fprintf (stderr, "fini i=0\n");
//     for (i = MAXSAMPLE; i < MAXVOICES; i++)	/* initialisation a partir de la valeur prochaine */
//     {
// fprintf (stderr, "on est en i=%d\n", i);
// 	initPatbut (gpatbut[i], -2, -1, -1);
//     }	    
	{   int voice = 0;
		    voice = tickchange_pattern (voice);
		    voice = tickchange_analogik (voice);
		    voice = tickchange_dyfilter (voice);
		    voice = tickchange_delay (voice);
	}
#ifdef DEBSEQU
	fprintf (stderr, "-->start_playing fin resync patterns\n");
#endif

    if (nplay.type != GREV_END)
	isreplaying = 1;
#ifdef DEBSEQU
	fprintf (stderr, "-->start_playing ca joue avec isreplaying=%d\n", isreplaying);
#endif
    return isreplaying;
}

int     playing_pression (int evtype, int voice, int data)
{
    if (isreplaying)
    {
#ifdef DEBSEQU
	fprintf (stderr, "entering playing mode\n");
#endif
	t = 0;			/* la faudrait tester si on est en train d'enregistrer etc... */
	if (initcurEVplay != NULL)
	    start_playing ();
	else
	{			/* rien a jouer... */
	    isreplaying = 0;
	    refreshcontrols (hplay);
	}
    }
    return 0;
}

int     taping_pression (int evtype, int voice, int data)
{
    if (istaping)
    {
#ifdef DEBSEQU
	fprintf (stderr, "entering recording mode\n");
#endif
	t = 0;			/* JDJDJD c'est violent ca */
	if (start_recording ())
	    return -1;
	if (initcurEVplay == NULL)
	    generate_freeze ();
	else
	    start_playing ();
    }
    else
    {
#ifdef DEBSEQU
	fprintf (stderr, "quiting recording mode\n");
#endif
	addevent (&curEVreg, &event_end);
	if (initcurEVplay != NULL)
	{
	    EventBuf *temp = evbuf_fusion (initcurEVplay, initcurEVreg);

	    evbuf_remove (initcurEVplay);
	    evbuf_remove (initcurEVreg);
	    initcurEVplay = temp;
	    initcurEVreg = NULL;
	}
	else
	{
	    initcurEVplay = initcurEVreg;
	    initcurEVreg = NULL;
	}
#ifdef DEBSEQU
	fprintf (stderr, "quiting recording mode...done\n");
#endif
    }
    return 0;
}

void    init_sequence (void)
{
    setpression (createbutton (CONTRBUTTON, ROWLEVEL, LINEDIV, 'e', CONTUNDEF, &istaping, 3,
			       "\033[1m\033[44m\033[31m \016`\017 \033[m",
			       "\033[1m\033[46m\033[31m \016`\017 \033[m",
			       "\033[43m\033[30m---\033[m", NULL, NULL, NULL),
		 taping_pression, 0, 0);

    hplay = createbutton (CONTRBUTTON, ROWLEVEL + 8, LINEDIV, 'p', CONTUNDEF, &isreplaying, 3,
			  "\033[1m\033[44m\033[32m > \033[m",
			  "\033[1m\033[46m\033[32m > \033[m",
			  "\033[43m\033[30m---\033[m", NULL, NULL, NULL);
    setpression (hplay, playing_pression, 0, 0);

    createbutton (CONTRBUTTON, ROWLEVEL + 4, LINEDIV, 'p', CONTUNDEF, NULL, 3,
		  "\033[1m\033[44m\033[32m « \033[m",
		  "\033[1m\033[46m\033[32m « \033[m",
		  "\033[43m\033[30m---\033[m", NULL, NULL, NULL);
    createbutton (CONTRBUTTON, ROWLEVEL + 12, LINEDIV, 'p', CONTUNDEF, NULL, 3,
		  "\033[1m\033[44m\033[32m » \033[m",
		  "\033[1m\033[46m\033[32m » \033[m",
		  "\033[43m\033[30m---\033[m", NULL, NULL, NULL);
    createbutton (CONTRBUTTON, ROWLEVEL + 0, LINEDIV + 1, 'p', CONTUNDEF, NULL, 3,
		  "\033[1m\033[44m\033[32m = \033[m",
		  "\033[1m\033[46m\033[32m = \033[m",
		  "\033[43m\033[30m---\033[m", NULL, NULL, NULL);

}
