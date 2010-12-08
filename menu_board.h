/* 
 * $Id: menu_board.h,v 1.2 2003/03/19 16:45:47 jd Exp $
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
 * $Log: menu_board.h,v $
 * Revision 1.2  2003/03/19 16:45:47  jd
 * major changes everywhere while retrieving source history up to this almost final release
 *
 * Revision 1.2.0.24.0.3  2000/10/01 21:07:03  jd
 * added Dsp button
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
 * all lot of correctioopn for new filter pattern
 * (should be rewrite completely soon)
 *
 * Revision 1.2.0.23  1999/09/15 10:20:39  jd
 * second pre tel-aviv public revision, for testing
 *
 * Revision 1.2.0.22.0.2  1999/09/05 22:30:16  jd
 * imported more code from old main
 *
 * Revision 1.2.0.22.0.1  1999/08/30 22:45:51  jd
 * Revision 1.1  1999/08/30 22:42:46  jd
 * Initial revision
 *
 */

#include "resoanlog.h"
#include "grpattern.h"		/* c'est pas terrible, faudrait modifier ca... JDJDJDJDJD */
			/* rapport aux fonctions reassignXYpatterns...          */
#include "grdyfilter.h"		/* c'est pas terrible, faudrait modifier ca... JDJDJDJDJD */
			/* rapport aux fonctions reassignXYpatterns...          */

#ifdef INMAINMENU
#define _mainmenuc
#else
#define _mainmenuc	extern
#endif

/* les definitions de menus */
#define MENUMAIN	1
#define MAXMENUITEM	7
#define MAXMENULARG	13

#ifdef INMAINMENU
/* 
 * ------------------------ definition du contenu du menu principal --------------
 */
#define MENU_MAIN 1
/* static char *fmenmain[] = */
char *fmenmain[] =
{
#define ITEMABOUT 0
    "about groovit",
#define ITEMNWSG 1
    "new song     ",
#define ITEMLOAD 2
    "load song    ",
#define ITEMSAVE 3
    "save song    ",
#define ITEMPREF 4
    "preferences  ",
#define ITEMSRAW 5
    "define raw   ",
#define ITEMQUIT 6
    "quit         ",
    NULL};

#endif

_mainmenuc
Radio   radiosample,	       /* le bloc boutons de samples                   */
        radiorow;	       /* le bloc radiobouton de patternrow            */

/* ---------------- les handles de controls a conserver ----------------------- */
_mainmenuc
int     hrows[MAXDISPLAYROW],		/* handles des controls pour la dynamique       */
        hgrows,		       /* handle du groupe des dynamiques              */
        hduppattern,	       /* handle du switch de duplication de pattern   */
        hgoleft,	       /* handle du bouton pour aller a gauche         */
        hgoright,	       /* handle du bouton pour aller a droite         */
        hpatternrow,	       /* handle du pattern nbrow                      */
	hopendsp,	       /* handle du reouverture dsp		       */
        hrawing2disk;	       /* handle du bouton d'ecriture en raw sur disk  */

/* ---------------- les variables d'etat -------------------------------------- */
_mainmenuc
int     isplayin,	       /* sommes-nous en train de jouer ?              */
	isopendsp,	       /* sommes-nous en train d'ecrire dans le dsp ?  */
	opendsp,	       /* open/re-open dsp			       */
	rawing2disk,	       /* rawing button                                */
        saving,		       /* saving button                                */
				/* loading, *//* loading button                               */
        duppattern,	       /* duplication de patterns                      */
        currowoffset,	       /* current offset for displaying patterns       */
        goleft,		       /* going left                                   */
        goright;	       /* going right                                  */

/* ---------------- les variables de son ! ------------------------------------ */
_mainmenuc
int     vlevel[MAXVOICES];     /* niveaux de sortie primaire de chaque voix    */
int     vpan[MAXVOICES];       /* le panoramique de chaque voix                */

/* ---------------- pour manipuler tout ca : ---------------------------------- */

					/* void fillmenu (char ** menu); *//* menu is an NULL ended list of strings */
int     initmenu (void);

/* int close_menu_main (void); */

void    reassignpattern (Pattern *pattern);
void    reassignapattern (int nvoice, APattern * curapattern);
void    reassignfpattern (int nvoice, APattern * curfpattern);
void    resync_displaypatterns (void);
int     menu_main (void);
void    initboard (char *version);
