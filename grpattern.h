/* 
 * $Id: grpattern.h,v 1.2 2003/03/19 16:45:47 jd Exp $
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
 * $Log: grpattern.h,v $
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
 * Revision 1.2.0.23  1999/09/15 10:20:39  jd
 * second pre tel-aviv public revision, for testing
 *
 * Revision 1.2.0.22.0.2  1999/09/05 22:30:16  jd
 * more code imported from old main
 *
 * Revision 1.2.0.22.0.1  1999/08/30 22:45:51  jd
 * Revision 1.1  1999/08/30 22:42:46  jd
 * Initial revision
 *
 */

#ifndef GRPATTERNH
#define GRPATTERNH

#include "radio.h"
#include "grevents.h"
#include "grsequence.h"

#ifdef INMAINGRPATT
#define _grpatternc
#else
#define _grpatternc	extern
#endif

typedef struct STPattern	/* pattern rythmique            */
{
    int     nbrow;		/* nombre de colonnes           */
    int     isnew;		/* never used pattern           */
    int     start[MAXSAMPLE][MAXPATTERNROW];	/* datas ...                    */
    char    name[24];		/* nom de la pattern            */
				// ajouts jd 2001_04-21
    int	    nextpat;		// pattern suivante à jouer...
    int	    nbrep;		// nombre de répétitions à effectuer...
				// ...avant d'aller à la suivante
}
Pattern;

_grpatternc
Pattern pattern[MAXPATTERN],		/* les patterns                 */
       *curpattern;		/* la pattern en cours de jeu   */

_grpatternc
Sample  sample[MAXSAMPLE];	/* les samples                                  */

/* ---------------- les handles de controls a conserver ----------------------- */

_grpatternc
Patbut  pattpatbut;		/* de quoi gerer le no de pattern               */

_grpatternc
int     hpattern[MAXSAMPLE],		/* handles des controls pour la rythmique       */
        hbt_sample[MAXSAMPLE];	/* handles des buttons de noms pour les samples */

/*==============================================================================*/

void    abitr_pattern (void);
int     init_pattern (Pattern *pattern, int voice);
void    refreshpatternrow (void);
void    cleanPattern (Pattern *p);

int     play_event_pattern (Event *ev);
int     genfreeze_pattern (int voice);
void    reset_pattern (void);
int     tickchange_pattern (int voice);
int     generation_pattern (int *outl, int *outr, int voice);

#endif
