/* 
 * $Id: grmixeur.c,v 1.2 2003/03/19 16:45:47 jd Exp $
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
 * $Log: grmixeur.c,v $
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
 * corrected name allocation bug
 *
 * Revision 1.2.0.23.0.1  1999/09/15 10:35:48  jd
 * *** empty log message ***
 *
 * Revision 1.2.0.23  1999/09/15 10:20:39  jd
 * second pre tel-aviv public revision, for testing
 *
 * Revision 1.2.0.22.0.2  1999/09/15 09:23:30  jd
 * added levels handling
 * corrected initialisation (zeroing)
 *
 *
 */

#include <stdio.h>

#include "config.h"

#include "grlevels.h"
#define INMAINMIXEUR
#include "grmixeur.h"

int     registertomixeur (int **mixl, int **mixr, int **mixm, char *nom, int ici)
{
    if (nbmixeurused >= NBMIXMAX)
    {
	fprintf (stderr, "error: no more mixeurs available for %s\n", nom);
	return -1;
    }
    *mixl = &mixeur[nbmixeurused].l;
    *mixr = &mixeur[nbmixeurused].r;
    *mixm = &mixeur[nbmixeurused].m;
    sprintf (mixeur[nbmixeurused].nom, "%8s", nom);

    mixeur[nbmixeurused].l = mixeur[nbmixeurused].r = 0;
#ifdef DEBLEVREG
    fprintf (stderr, "registered mixeur #%d >%s<\n", nbmixeurused, mixeur[nbmixeurused].nom);
#endif
    if (ici != -1)
	register_levels_as (ici, mixeur[nbmixeurused].nom, mixeur[nbmixeurused].level);
    else
	register_levels (mixeur[nbmixeurused].nom, mixeur[nbmixeurused].level);

    nbmixeurused++;
    return 0;
}

int     init_mixeur (void)
{
    int     m;

    nbmixeurused = 0;

  /* mise a zero des cumuls de mixeurs (laissee ensuite aux bon soin des abonnes */
    for (m = 0; m < NBMIXMAX; m++)
	mixeur[m].l = mixeur[m].r = 0;

    return 0;
}
