/* 
 * $Id: granalogik.h,v 1.2 2003/03/19 16:45:47 jd Exp $
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
 * $Log: granalogik.h,v $
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
 * moved almost complete generation from old' main
 *
 * Revision 1.2.0.23  1999/09/15 10:20:39  jd
 * second pre tel-aviv public revision, for testing
 *
 * Revision 1.2.0.22.0.3  1999/09/15 09:23:30  jd
 * corrected for "voice" use
 *
 * Revision 1.2.0.22.0.2  1999/09/05 22:30:16  jd
 * Initial revision
 *
 *
 */

#include "resoanlog.h"
#include "grevents.h"
#include "grsequence.h"

#ifdef INMAINGRAPATT
#define _granalogik
#else
#define _granalogik
#endif

/*----------------------------les donnees des voies analogiques-----------------*/
typedef struct STCtAnVoice	/* structure de controle d'une voie analogique  */
{
    AnReso  reso;		/* le resonateur analogique de la voie          */

    int     genfreq,		/* ioncrementeur de frequence de base           */
            genfreq2,		/* ioncrementeur de frequence de base bis       */
            freqbase,		/* frequence de base en multiple de 1,3458 Hz aprox */
            freqbase2,		/* frequence de base bis                        */
            ctfreqbase2;	/* controle de la frequence de base bis         */

// int     curnbrow;           /* current pattern length (TickBPM of course)   */
  // tp,                 /* current pattern time en TickBPM              */
  // ncurpattern;       /* no de pattern en cour de jeu (Cf curapattern) */

    Patbut  patbut;		/* de quoi gerer le no de pattern        */
    APattern *curapattern;	/* la pattern analogique en cours de jeu  */

// JDJDJDJD a supprimer desormais....
  // Radio   radiopattern;      /* le bloc radiobouton de pattern               */
  // Radio   radiobank;        /* le bloc radiobouton de bank                  */
}
AnVoice;

_granalogik
Analogik analog[MAXANALOG];	/* les structures de mixage analogiques         */

_granalogik
AnVoice anvoice[MAXANALOG];	/* les controles d'une voie analogique          */

_granalogik
APattern apattern[MAXPATTERN];		/* patterns analogiques                         */

_granalogik
int     hapattern[MAXANALOG],		/* controls handle pour les patterns analogiques */
        hapatternrow[MAXANALOG];	/* handle du apattern nbrow                     */

int     genfreeze_analogik (int voice);
int     init_analogik (int voice);
void    constructapattern (APattern *apattern, int i, int voice);
void    refreshapatternrow (int i);

int     play_event_analogik (Event *ev);
void    reset_analogik (void);
int     tickchange_analogik (int voice);
int     generation_analogik (int *outl, int *outr, int voice);

void    abitr_analogik (void);
void    initAPattern (APattern *p);
void    initAnVoice (AnVoice *p);
