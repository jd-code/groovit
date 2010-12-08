/* 
 * $Id: grmixeur.h,v 1.2 2003/03/19 16:45:47 jd Exp $
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
 * $Log: grmixeur.h,v $
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
 * Revision 1.2.0.23.0.3  1999/10/07 14:08:05  jd
 * Revision 1.2.0.23  1999/09/15 10:20:39  jd
 * second pre tel-aviv public revision, for testing
 *
 * Revision 1.2.0.22.0.2  1999/09/15 09:23:30  jd
 * added levels handling
 * corrected initialisation (zeroing)
 *
 * Revision 1.2.0.22.0.1  1999/09/07 12:02:17  jd
 * Initial revision
 *
 *
 */

#ifndef JDGRMIXEURH
#define JDGRMIXEURH

#define LENMIXNAME	8
#ifndef NBMIXMAX
#warning NBMIXMAX se voit definir une valeur par defaut !!!
#define NBMIXMAX	10	/* une valeur par defaut balaise ! */
#endif

#ifdef INMAINMIXEUR
#define _grmixeurc
#else
#define _grmixeurc	extern
#endif

typedef struct STMixeur	       /* structure de donnees d'un mixeur moulinex */
{
    int     l,		       /* le cumul en voie gauche              */
            r,		       /* le cumul en voie droite              */
            m,		       /* le cumul en mono avant mixage        */
            level[MAXVOICES];  /* le niveau de mixage de la ieme voie  */

    char    nom[LENMIXNAME + 1];	/* le nom de facade                     */
}
Mixeur;

_grmixeurc
Mixeur  mixeur[NBMIXMAX];      /* les mixeurs proprement dits */

_grmixeurc
int     nbmixeurused;	       /* le nombre courant de mixeurs actifs */

/*------------------les fonctions exportees----------------------------*/

int     registertomixeur (int **mixl, int **mixr, int **mixm, char *nom, int ici);
int     init_mixeur (void);

#endif
