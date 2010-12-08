/* 
 * $Id: grdyfilter.h,v 1.2 2003/03/19 16:45:47 jd Exp $
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
 * $Log: grdyfilter.h,v $
 * Revision 1.2  2003/03/19 16:45:47  jd
 * major changes everywhere while retrieving source history up to this almost final release
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
 * Revision 1.2.0.22.0.2  1999/09/05 22:30:16  jd
 * Initial revision
 *
 *
 */

#ifndef GRDYFILTERH
#define GRDYFILTERH

#include "resoanlog.h"

#ifdef INMAINGRDYFILTER
#define _grdyfilter
#else
#define _grdyfilter
#endif

/*----------------------------les donnees des voies filtre dynamique------------*/
typedef struct STCtDyFilter	/* structure de controle d'un filtre dynamique  */
{
    AnReso  reso;		/* le resonateur analogique de la voie          */

    int     genfreq,		/* ioncrementeur de frequence de base           */
            genfreq2,		/* ioncrementeur de frequence de base bis       */
            freqbase,		/* frequence de base en multiple de 1,3458 Hz aprox */
            freqbase2,		/* frequence de base bis                        */
            ctfreqbase2;	/* controle de la frequence de base bis         */

// int     tp,                 /* current pattern time en TickBPM              */
  // JDJDJDJD on devrait pour supprimer ca bientot...
  // ncurpattern,       /* no de pattern en cour de jeu (Cf curfpattern) */
  // curnbrow;           /* current pattern length (TickBPM of course)   */

    Patbut  patbut;		/* de quoi gerer le no de pattern               */
    APattern *curfpattern;	/* la pattern analogique en cour de jeu         */
  // Radio   radiopattern;      /* le bloc radiobouton de pattern               */
  // Radio   radiobank;          /* le bloc radiobouton de bank                  */
}
DyFilter;

/*--------------------------------les handles de controles a conserver----------*/

_grdyfilter
int     hfpattern[MAXDYFILT],		/* controls handle pour les patterns analogiques */
        hfpatternrow[MAXDYFILT];	/* controls handle pour range de taille de pat */

/*--------------------------------les contenants--------------------------------*/

_grdyfilter
Analogik filter;		/* les structures de mixage du filtre           */

/* _grdyfilter JDJDJDJD a supprimer en toute logique */
									/* AnReso       FL;
															      JDJDJD idem *//* les 

															      resonateurs 

															      de 

															      filtre 

															    */

_grdyfilter
DyFilter dyfilter[MAXDYFILT];		/* les controles d'un filtre dynamique          */

_grdyfilter
APattern fpattern[MAXPATTERN];		/* patterns de filtres                          */

/*-------------------------------pour manipuler tout ca-------------------------*/

void    reset_dyfilter (void);

int     tickchange_dyfilter (int voice);

int     generation_dyfilter (int *outl, int *outr, int voice);

int     play_event_dyfilter (Event *ev);

int     genfreeze_dyfilter (int voice);

void    abitr_dyfilter (void);

int     init_dyfilter (int voice);

#endif
