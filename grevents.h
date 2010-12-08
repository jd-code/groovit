/* 
 * $Id: grevents.h,v 1.1 2003/03/19 21:56:36 jd Exp $
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
 * $Log: grevents.h,v $
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
 *
 */

#ifndef GREVENTS_H
#define GREVENTS_H

#ifndef GRANUL_EB
#define GRANUL_EB   (512)
#endif
/* 
 * ------------------------- liste des evenements --------------------------------
 */
#define EV_SIGNS	0x0000		/* base des evenements de signaux	    */
#define	GREV_END	(EV_SIGNS+2)	/* terminaison de getnext		    */

#define EV_ANVO		0xFE10		/* base des evenements voie analogiques	    */
#define EV_ANVO_F1	EV_ANVO		/* freqcut1				    */
#define EV_ANVO_F2	(EV_ANVO+1)	/* freqcut2				    */
#define EV_ANVO_RZ	(EV_ANVO+2)	/* resonance				    */
#define EV_ANVO_DK	(EV_ANVO+3)	/* decay				    */
#define EV_ANVO_PT	0xFD10		/* base des  ev pattern voie analog.	    */
#define EV_ANVO_PI	(EV_ANVO_PT+1)	/* changement immediat de pattern           */
#define EV_ANVO_PN	(EV_ANVO_PT+2)	/* position dans la pattern		    */

#define EV_DYFI		0xFE20		/* base des evenements filtre dynamique	    */
#define EV_DYFI_F1	EV_DYFI		/* freqcut1				    */
#define EV_DYFI_F2	(EV_DYFI+1)	/* freqcut2				    */
#define EV_DYFI_RZ	(EV_DYFI+2)	/* resonance				    */
#define EV_DYFI_DK	(EV_DYFI+3)	/* decay				    */
#define EV_DYFI_PT	0xFD20		/* base des  ev pattern dyn. filter	    */
#define EV_DYFI_PI	(EV_DYFI_PT+1)	/* changement immediat de pattern           */
#define EV_DYFI_PN	(EV_DYFI_PT+2)	/* position dans la pattern		    */

#define EV_RYVO_PT	0xFD00		/* base des  ev pattern voir rythmique	    */
#define EV_RYVO_PI	(EV_RYVO_PT+1)	/* changement immediat de pattern	    */
#define EV_RYVO_PN	(EV_RYVO_PT+2)	/* position dans la pattern		    */

typedef struct STevent		/* typedef Event                                */
{
    long    dtime;		/* delta time                                   */
    int     type;		/* type d'evenement                             */
    int     voice;		/* la voie affectée                             */
    int     val;		/* valeur                                       */
}
Event;

typedef struct STeventbuf	/* typedef EventBuf                             */
{
    int     cur;		/* evenement en cour de lecture                 */
    int     nb;			/* nombre actuel d'evenements                   */
    int     nbmax;		/* nombre max d'evenement dans ce buf           */
    Event  *e;			/* tableau de nbmax elements                    */
    struct STeventbuf *pred,	/* buffer d'evenement precedent                 */
           *next;		/* idem suivant                                 */
}
EventBuf;

#ifndef INMAINGREVENT
extern Event event_end;		/* sert a la terminaison des getnext            */

#endif

EventBuf *evbuf_new (EventBuf *pred);
int     evbuf_remove (EventBuf *eb);
void    addevent (EventBuf **peb, Event *ev);	/* attention *peb peut etre mis a jours ! */
int     getnextevent (EventBuf **peb, Event *ev);
int     getremovenextevent (EventBuf **peb, Event *ev);
EventBuf *evbuf_fusion (EventBuf *s1, EventBuf *s2);

#endif
