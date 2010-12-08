/* 
 * $Id: grlevels.h,v 1.2 2003/03/19 16:45:47 jd Exp $
 * Groovit Copyright (C) 1998,1999 Jean-Daniel PAUGET
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
 * $Log: grlevels.h,v $
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
 * Revision 1.2.0.23.0.3  1999/10/07 14:08:05  jd
 * Revision 1.2.0.23  1999/09/15 10:20:39  jd
 * second pre tel-aviv public revision, for testing
 *
 * Revision 1.2.0.22.0.2  1999/09/15 09:23:30  jd
 * added indirect and relatives levels
 * added selection menu
 *
 * Revision 1.2.0.22.0.1  1999/09/12 18:00:50  jd
 * Initial revision
 *
 */

#ifdef INMAINLEVELS
#define _grlevelsc
#else
#define _grlevelsc	extern
#endif

#ifndef MAXDISPLEVELS
#warning definition arbitraire de MAXDISPLEVELS, maybe should make config
#define MAXDISPLEVELS 4
#endif

#ifndef MAXLEVELS
#warning definition arbitraire de MAXLEVELS, maybe should make config
#define MAXLEVELS 4
#endif

#define MINLEVRESERVED	4	/* four levels reserved for backward compatibilty */

#define LEVUNDISPLAYED	-1

/*------------------------------handles a conserver-----------------------------*/

_grlevelsc
// int     hlevels[MAXDISPLEVELS][MAXVOICES];	/* les handles des differents boutons   */
int     hlevels[MAXLEVELS][MAXVOICES];	/* les handles des differents boutons   */
_grlevelsc
int	ctxlevel[MAXLEVELS][MAXMODULES];	/* les handles des contexts		*/

/*------------------------------les pointeurs sur le contenu--------------------*/

typedef struct STLevels	       /* la structure d'un levels arbitraire  */
{
    char   *nom;	       /* son nom (JDJDJD a voir....)          */
    int     pos;	       /* sa position courante d'affectation   */
    int    *data;	       /* un ptr sur le tableau de valeurs     */
}
Levels;

/*------------------------------pour manipuler tout ca :------------------------*/

int     register_levels (char *nom, int *data);
int     register_levels_as (int h, char *nom, int *data);
int     initlevels (void);
int     menu_watcher (void);   /* la fonction de surveillance des menu levels          */
void    show_levels (int position, int l);	/* affecte les niveaux "l" à la "position"

						        */
void    resync_disp_levels (void);	/* resynchronise l'affichage

					   */
