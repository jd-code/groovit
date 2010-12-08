/* 
 * $Id: grdelay.h,v 1.2 2003/03/19 16:45:47 jd Exp $
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
 * $Log: grdelay.h,v $
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
 * Revision 1.2.0.23.0.2  1999/09/27 00:46:09  jd
 * correction from API
 *
 * Revision 1.2.0.23  1999/09/15 10:20:39  jd
 * second pre tel-aviv public revision, for testing
 *
 * Revision 1.2.0.22.0.2  1999/09/15 09:23:30  jd
 * most features are brand new
 *
 * Revision 1.2.0.22.0.1  1999/09/06 14:06:24  jd
 * Initial revision
 *
 *
 */

#ifdef INMAINGRDELAY
#define _grdelay
#else
#define _grdelay
#endif

#include "constants.h"

#define MINREV  -16		/* duree de reverb la plus petite */
#define MAXREVERB (MAXREV*UNJBPM/(MINBPM-1))	/* taille du buffer de reverberation en
						   samples */

/* ---------------- les variables d'etat -------------------------------------- */
_grdelay
int     decay,		       /* decay de la reverb                           */
        delaytick,	       /* DELAYloop en TickBPM                         */
	decay2,			/* decay de la nouvelle reverb			*/
	delay2,			/* delay de la nouvelle reverb			*/
	decay3,			/* decay de la nouvelle reverb			*/
	delay3,			/* delay de la nouvelle reverb			*/
	inject2;		/* injection de la nouvelle reverb dans l'ancienne */

_grdelay
long    rdelay,		       /* duree du prochain DELAYloop                  */
        tdelay;		       /* decompte du DELAYloop courant en sample      */

_grdelay
int     lrev,		       /* construction du reverbsample gauche          */
        rrev;		       /* construction du reverbsample droit           */

/*----------------------------buffers et pointeurs de sons----------------------*/
_grdelay
short   reverbbuf[MAXREVERB][2],	/* buffer de revberation        */
	reverbbuf2[MAXREVERB][2],	/* second buffer		*/
	reverbbuf3[MAXREVERB][2],	/* second buffer		*/
       *rbuf;		       /* pointeur sur reverbsample en cours           */

void    reset_delay (void);
int     tickchange_delay (int voice);
int     generation_delay (int *outl, int *outr, int voice);	/* a inliner plus tard */
void    resetdelay (void);
void    init_delay (void);
