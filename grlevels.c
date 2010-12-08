/*                                  __               
                                   / /\____          
    _____   __  _____    __  ____  \____  /\  ______      ______  __  _____    __  ______    ____
   /__  /\ / /\/__  /\  / /\/ __/\  \__/ / / / __  /\    / __  /\/ /\/__  /\  / /\/___  /\  /_  /\
   \_/ / //_/ /\_/ / / / /_/ /\_\/  / __/ / / /\/ / /   / /\/ / /_/ /\_/ / / / / /\__/ / ___\/ / /
 ___/ /_/ \_\___/ /_/ / __  /_/    / /\_\/_/ / / / / __/ / / / /\_\___/ /_/ / / /   / / / __  / /
/______/\   /______/\/_/\/___/\   /_/ / /___/ /_/ / /___/ /_/ /   /______/\/_/ /   /_/ /_/ /_/ /
\______\/   \______\/\_\/\___\/   \_\/  \___\/\_\/  \___\/\_\/    \______\/\_\/    \_\/\_\/\_\/

 * $Id: grlevels.c,v 1.2 2003/03/19 16:45:47 jd Exp $
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
 * $Log: grlevels.c,v $
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
 * corrected registration count bug
 *
 * Revision 1.2.0.23  1999/09/15 10:20:39  jd
 * second pre tel-aviv public revision, for testing
 *
 * Revision 1.2.0.22.0.2  1999/09/15 09:23:30  jd
 * added indirect and relatives levels
 * added selection menu
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "sample.h"
#include "placement.h"
#include "controls.h"
#include "grmixeur.h"
#define INMAINLEVELS
#include "grlevels.h"

/*------------------------------handles a conserver-----------------------------*/
static
int     whichlevel[MAXDISPLEVELS];	/* quel level a quel position ?         */
static
Levels  levels[MAXLEVELS];	/* les levels proprement dits           */
static
int     nblevels;		/* leur nombre                          */

static
int     ctxmenulev,		/* le handle du menu (flottant !!) des niveaux divers   */
        hmenulevcall[MAXDISPLEVELS],	/* les handles des points d'appel de celui ci JDJDJD
					   a integrer dans les contexts eux-meme */
        hmenulevitem[MAXLEVELS],	/* les handles des entrees du menu. */
        curmenu = -1;		/* le menulevels est-il actif ?                         */

static
Radio   radiomenulev,		/* le radio pour les entrees du menu nibveaux..         */
        radiolevels;		/* le radio des differents boutons d'appel              */

static
char    weakmenubuf[LENMIXNAME + 2];	/* sert au reinitialisations de menus

					 */

static void fillmenu (void)
{
    int     i;

    for (i = 0; i < MAXLEVELS; i++)
	relooksimplebutton (hmenulevitem[i], 0, CONTRED + CONTNOPUCE,
			    (levels[i].nom != NULL) ? levels[i].nom : weakmenubuf);
}

void    show_levels (int p, int l)	/* affecte les niveaux "l" à la position "p" */
{
    int     i;

// JDJDJDJD je m'y remets en debug la....
  // if ((p >= 0) && (p < MAXDISPLEVELS) && (levels[l].nom != NULL))
    if ((p >= 0) && (p < MAXDISPLEVELS) && (l >= 0) && (l < MAXLEVELS) && (levels[l].nom != NULL))
    {
	for (i = 0; i < MAXVOICES; i++)
	    reassigncontrols (hlevels[p][i], (void *) (levels[l].data + i));
	relooksimplebutton (hmenulevcall[p], 0, CONTNOCOL + CONTNOPUCE, levels[l].nom);
	refreshcontrols (hmenulevcall[p]);

	if (whichlevel[p] != LEVUNDISPLAYED)	/* swap,swap! */
	{
	    if (levels[l].pos != LEVUNDISPLAYED)
	    {
		int     oldp = levels[l].pos, oldl = whichlevel[p];

		for (i = 0; i < MAXVOICES; i++)
		    reassigncontrols (hlevels[oldp][i], (void *) (levels[oldl].data + i));
		relooksimplebutton (hmenulevcall[oldp], 0, CONTNOCOL + CONTNOPUCE, levels[oldl].nom);
		refreshcontrols (hmenulevcall[oldp]);
		whichlevel[oldp] = oldl;
		levels[oldl].pos = oldp;
	    }
	    else
	    {
		levels[whichlevel[p]].pos = LEVUNDISPLAYED;
	    }
	}
	else
	{
	    if (levels[l].pos != LEVUNDISPLAYED)
	    {
		int     oldp = levels[l].pos;

		for (i = 0; i < MAXVOICES; i++)
		    reassigncontrols (hlevels[oldp][i], NULL);
		relooksimplebutton (hmenulevcall[oldp], 0, CONTNOCOL + CONTNOPUCE, weakmenubuf);
		refreshcontrols (hmenulevcall[oldp]);
		whichlevel[oldp] = LEVUNDISPLAYED;
	    }
	}
	whichlevel[p] = l;
	levels[l].pos = p;
    }
    else
    {
	if ((l >= 0) && (l < MAXLEVELS))
	    fprintf (stderr, "error: level position %d doen't exist !\n", p);
	else
	    fprintf (stderr, "error: level number %d doen't exist was asked for show !\n", p);
    }
}

void    resync_disp_levels (void)
{
    int     i;

    for (i = 0; i < MAXDISPLEVELS; i++)
	show_levels (i, whichlevel[i]);
}

int     register_levels (char *nom, int *data)
{
    if (nblevels >= MAXLEVELS)
    {
	fprintf (stderr, "error: assigning level %s, no more to offer !\n", nom);
	return -1;
    }
    levels[nblevels].nom = nom;
    levels[nblevels].data = data;
    levels[nblevels].pos = LEVUNDISPLAYED;
    nblevels++;
    return nblevels - 1;
}

int     register_levels_as (int h, char *nom, int *data)
{
    if (levels[h].nom != NULL)
    {
	fprintf (stderr, "error: trying to affect " "%s" " to levels[%d] " "%s" ", already used, not done\n", nom, h, levels[h].nom);
	return -1;
    }
    levels[h].nom = nom;
    levels[h].data = data;
    levels[h].pos = LEVUNDISPLAYED;
    return h;
}

int     initlevels (void)
{
    int     i, j;

    nblevels = MINLEVRESERVED;
    for (i = 0; i < MAXLEVELS; i++)
    {
	levels[i].nom = NULL;
	levels[i].data = NULL;
    }
    for (i = 0; i < MAXDISPLEVELS; i++)
    {
	whichlevel[i] = LEVUNDISPLAYED;
    }

    for (i = 0; i < MAXLEVELS; i++)
    {
	for (j = 0; j < MAXVOICES; j++)
	    hlevels[i][j] = 0;
	for (j = 0; j < MAXMODULES; j++)
	    ctxlevel[i][j] = 0;
    }

    curmenu = -1;
    for (i = 0; i < LENMIXNAME; i++)
	weakmenubuf[i] = ' ';
    weakmenubuf[i] = 0;

    radiolevels.nb = 0;
    radiolevels.value = -1;
    for (i = 0; i < MAXDISPLEVELS; i++)
    {
	hmenulevcall[i] = createsimplebutton (CONTRRADIO, ROWLEVEL + i * 9, LINELEGEND, 0, CONTNOCOL + CONTNOPUCE, CONTUNDEF, &radiolevels, weakmenubuf);
    }

    ctxmenulev = createcontexts (ROWLEVEL, LINEMENU, LENMIXNAME + 2, MAXLEVELS + 2, CONTRED, "levels", NULL);
    installcontexts (ctxmenulev);
    radiomenulev.nb = 0;
    radiomenulev.value = -1;
    for (j = 0; j < MAXLEVELS; j++)
    {
	hmenulevitem[j] = createsimplebutton (CONTRRADIO, ROWLEVEL + 1, LINEMENU + 1 + j, 0, CONTRED + CONTNOPUCE, CONTUNDEF, &radiomenulev, weakmenubuf);
    }
    desinstallcontexts (ctxmenulev);

    return (ctxmenulev);
}

int     close_menu_levels (void)
{
    int     h;

    curmenu = -1;
    h = hmenulevcall[radiolevels.value];
    radiolevels.value = -1;
    refreshcontrols (h);

/* actionnecontexts (ctxmenu, NULL); pourrait etre utile par la suite... */
    return desinstallcontexts (ctxmenulev);
}

int     Amenu_level (int lastpressed)
{
    int     l = curmenu;

    if ((lastpressed != -1) && (lastpressed != hmenulevcall[curmenu]))
    {
	close_menu_levels ();
	if (radiomenulev.value != -1)
	{
#ifdef JDVEUTVOIR
	    fprintf (stderr, "levels %d -> position %d\n", radiomenulev.value, l);
#endif
	    show_levels (l, radiomenulev.value);
	    radiomenulev.value = -1;
	}
    }
    return 0;
}

int     raise_menu_levels (void)
{
    fillmenu ();
    radiomenulev.value = -1;
    curmenu = radiolevels.value;
    actionnecontexts (ctxmenulev, Amenu_level);
    return installcontexts_ici (ctxmenulev,
			ROWLEVEL + 9 * curmenu - ((curmenu == (MAXDISPLEVELS - 1)) ? 2 : 1),
				LINEMENU);
}

int     menu_watcher (void)	/* la fonction de surveillance des menu levels */
{
    if (radiolevels.value != curmenu)
    {
	if (curmenu == -1)
	    raise_menu_levels ();
	else
	{
	    if (radiolevels.value == -1)
		close_menu_levels ();
	    else
	    {
		int     r = radiolevels.value;

		close_menu_levels ();
		radiolevels.value = r;
		raise_menu_levels ();
	    }
	}
    }
    return 0;
}
