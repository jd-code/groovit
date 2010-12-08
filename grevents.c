/* 
 * $Id: grevents.c,v 1.1 2003/03/19 21:56:36 jd Exp $
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
 * $Log: grevents.c,v $
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define INMAINGREVENT
#include "grevents.h"

Event   event_end =		/* sert a la terminaison des getnext            */
{0x8FFFFFFF, GREV_END, -1 - 1};

EventBuf *evbuf_new (EventBuf *pred)
{
    EventBuf *p;
    if ((p = (EventBuf *) malloc (sizeof (EventBuf))) == NULL)
    {
	fprintf (stderr, "error: allocation failed for EventBuf (%ld bytes)\n",
		 (long) sizeof (EventBuf));

	return p;
    }
    if ((p->e = (Event *) malloc (GRANUL_EB * sizeof (Event))) == NULL)
    {
	fprintf (stderr, "error: allocation failed for EventBuf (%ld bytes)\n",
		 (long) (GRANUL_EB * sizeof (Event)));

	free (p);
	return NULL;
    }
    p->nb = 0;
    p->cur = 0;
    p->nbmax = GRANUL_EB;
    p->pred = pred;
    p->next = NULL;
    return p;
}


int	evbuf_remove (EventBuf *eb)
{   if (eb->e == NULL)
    {	fprintf (stderr, "warning: attempt to remove an empty event-buffer\n");
	return -1;
    }
    if (eb != NULL)
	free (eb->e);
    free (eb);
    return 0;
}

void    addevent (EventBuf **peb, Event *ev)	/* attention *peb peut etre mis a jours ! */
{
    EventBuf *eb = *peb;

#ifdef DEBEVENTADD
fprintf (stderr, "addevent dtime:%5ld type:%04x voice:%2d val:%4d\n", 
	ev->dtime, ev->type, ev->voice, ev->val);
#endif
    if (eb == NULL)
	return;
    if (eb->nb >= eb->nbmax)
    {
	if (eb->next == NULL)
	{
	    eb->next = evbuf_new (eb);
	    eb = eb->next;
	    *peb = eb;
	    if (eb == NULL)
	    {
		fprintf (stderr, "adding events cancelled\n");
		return;
	    }
	}
	else
	{
	    eb = eb->next;
	    *peb = eb;
	}
    }
    memcpy (&eb->e[eb->nb], ev, sizeof (Event));

    eb->nb++;
}

int     getnextevent (EventBuf **peb, Event *ev)
{
    EventBuf *eb = *peb;

    if (eb == NULL)
	return 1;
    if (ev == NULL)
    {
	fprintf (stderr, "error: getnextevent with NULL as destination ?\n");
	return 1;
    }

    if (eb->cur >= eb->nbmax)
    {
	eb = eb->next;
	*peb = eb;
	if (eb == NULL)
	{
	    fprintf (stderr, "warning: getnextevent: end of buffer reached without notice ?\n");
	  /* JDJDJDJD pour que ce warning ne sorte pas en usage normal il devrait tjrs y
	     avoir un buffer d'avance. A mediter ! */

	    memcpy (ev, &event_end, sizeof (Event));

#ifdef DEBEVENTADD
fprintf (stderr, "GETevent dtime:%5ld type:%04x voice:%2d val:%4d (exit 1)\n", 
	ev->dtime, ev->type, ev->voice, ev->val);
#endif
	    return 1;
	}
	eb->cur = 0;		/* on rewind */
    }

    if (eb->cur >= eb->nb)
    {
	memcpy (ev, &event_end, sizeof (Event));

#ifdef DEBEVENTADD
fprintf (stderr, "GETevent dtime:%5ld type:%04x voice:%2d val:%4d (exit 2)\n", 
	ev->dtime, ev->type, ev->voice, ev->val);
#endif
	return 2;		/* normal end of operation */
    }

    memcpy (ev, &eb->e[eb->cur], sizeof (Event));
    eb->cur ++;

#ifdef DEBEVENTADD
fprintf (stderr, "GETevent dtime:%5ld type:%04x voice:%2d val:%4d\n", 
	ev->dtime, ev->type, ev->voice, ev->val);
#endif
    return 0;
}

int     getremovenextevent (EventBuf **peb, Event *ev)
{
    EventBuf *eb = *peb;

    if (eb == NULL)
	return 1;
    if (ev == NULL)
    {
	fprintf (stderr, "error: getnextevent with NULL as destination ?\n");
	return 1;
    }

    if (eb->cur >= eb->nbmax)
    {	EventBuf *next = eb->next;

	evbuf_remove (eb);

	eb = next;
	*peb = eb;
	if (eb == NULL)
	{
	    fprintf (stderr, "warning: getnextevent: end of buffer reached without notice ?\n");
	  /* JDJDJDJD pour que ce warning ne sorte pas en usage normal il devrait tjrs y
	     avoir un buffer d'avance. A mediter ! */

	    memcpy (ev, &event_end, sizeof (Event));

	    return 1;
	}
	eb->cur = 0;		/* on rewind */
    }

    if (eb->cur >= eb->nb)
    {
	memcpy (ev, &event_end, sizeof (Event));

	return 2;		/* normal end of operation */
    }

    memcpy (ev, &eb->e[eb->cur], sizeof (Event));

    return 0;
}

EventBuf *evbuf_fusion (EventBuf *s1, EventBuf *s2)
{
    EventBuf *out,		/* l'eventbuf de sortie */
           *outinit, *c1 = s1,	/* pointeur en cours de lecture */
           *c2 = s2;
    Event   e1, e2;		/* les evenements en cours de comparaison */
    long    relatd1 = 0,	/* temps relatif au dernier event enregistre */
            relatd2 = 0;
#ifdef DEBEVMERGE
fprintf (stderr, "entering merging machine\n");
#endif

    outinit = evbuf_new (NULL);
    out = outinit;
    if (out == NULL)
    {
	fprintf (stderr, "error: buffer fusion failed !\n");
	return NULL;
    }

    c1->cur = 0;		/* JDJDJDJD un rewind serait a ecrire */

    c2->cur = 0;

    getnextevent (&c1, &e1);
    getnextevent (&c2, &e1);

    while ((getnextevent (&c1, &e1) == 0) && (getnextevent (&c2, &e1) == 0))
    {
	if (relatd1 + e1.dtime <= relatd2 + e2.dtime)	/* le 1 est prioritaire au 2 */
	{
	    e1.dtime -= relatd1;
	    relatd1 = 0;
	    relatd2 += e1.dtime;
	    addevent (&out, &e1);
	}
	else
	{
	    e2.dtime -= relatd2;
	    relatd2 = 0;
	    relatd1 += e2.dtime;
	    addevent (&out, &e2);
	}
    }

    if (e1.type != GREV_END)
	if (e2.type != GREV_END)
	{
	    fprintf (stderr, "warning: event buffer abnormal termination\n");
	}
	else
	{			/* continuation avec le buffer 1 */
	    e1.dtime -= relatd1;
	    while (getnextevent (&c1, &e1) == 0)
	    {	addevent (&out, &e1);
	    }
	}
    else if (e2.type != GREV_END)
    {				/* continuation avec le buffer 2 */
	    e2.dtime -= relatd2;
	    while (getnextevent (&c2, &e2) == 0)
	    {	addevent (&out, &e2);
	    }
    }

    return (outinit);
}
