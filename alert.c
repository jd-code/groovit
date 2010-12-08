/* 
 * $Id: alert.c,v 1.2 2003/03/19 16:45:47 jd Exp $
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
 * $Log: alert.c,v $
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
 * added dialogs
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

#include "controls.h"
#define INALERTMAIN
#include "alert.h"

#ifdef SHOWRCS
static char *rcsid = "$Id: alert.c,v 1.2 2003/03/19 16:45:47 jd Exp $";

#endif
static int firstuse = 1;

typedef struct talerts
{
    int     used,	       /* is the alert used ?                  */
            ctx,	       /* le contexts de l'alert               */
            hbt_def,	       /* le handle du boutton de defaut       */
            bt_def,	       /* sa valeur                            */
            hbt_alt1,	       /* le handle du boutton alternatif 1    */
            bt_alt1,	       /* sa valeur                            */
            hbt_alt2,	       /* le handle du boutton alternatif 2    */
            bt_alt2,	       /* sa valeur                            */
            hts_title,	       /* le handle du titre                   */
            hts_1,	       /* la premiere ligne                    */
            hts_2,	       /* la deuxieme ligne                    */
            hts_3,	       /* la troisieme ligne                   */
            hts_4,	       /* la troisieme ligne                   */
            hts_5;	       /* la troisieme ligne                   */
}
Alerts;

Alerts  alerts[NBMAXALERT];

int     init_alerts (void)
{
    int     i;

  /* JDJDJD : firstuse also used for checking reentrance */
    if (firstuse)
    {
#ifdef SHOWRCS
	fprintf (stderr, "%s\n\r", rcsid);
#endif
	firstuse = 0;
    }
    else
	return -1;

    for (i = 0; i < NBMAXALERT; i++)
    {
	alerts[i].used = 0;
    }

    return 0;
}

int     quit_alerts (int h)
{
    if (alerts[h].used == 0)
    {
	fprintf (stderr, "trying to poll an unallocated alerts ???\n");
	return -1;
    }

    actionnecontexts (alerts[h].ctx, NULL);
    desinstallcontexts (alerts[h].ctx);
    return 0;
}

int     poll_alerts (int h)
{
    if (alerts[h].used == 0)
    {
	fprintf (stderr, "trying to poll an unallocated alerts ???\n");
	return -1;
    }

    if (alerts[h].bt_def == BUT_ON)
    {
	alerts[h].bt_def = BUT_OFF;
	refreshcontrols (alerts[h].hbt_def);
	return 1;
    }
    if (alerts[h].bt_alt1 == BUT_ON)
    {
	alerts[h].bt_alt1 = BUT_OFF;
	refreshcontrols (alerts[h].hbt_alt1);
	return 2;
    }
    if (alerts[h].bt_alt2 == BUT_ON)
    {
	alerts[h].bt_alt2 = BUT_OFF;
	refreshcontrols (alerts[h].hbt_alt2);
	return 3;
    }
    return 0;
}

int     open_alerts (int h, char *title, char *def, char *alt1, char *alt2,
		     char *str1, char *str2, char *str3, char *str4, char *str5,
		     Tactionne actionne)
{
    if (alerts[h].used == 0)
    {
	fprintf (stderr, "trying to use an unallocated alerts ???\n");
	return -1;
    }

    relooktextstr (alerts[h].hts_title, CONTRED, title);
    relooktextstr (alerts[h].hts_1, CONTBLACK, str1);
    relooktextstr (alerts[h].hts_2, CONTBLACK, str2);
    relooktextstr (alerts[h].hts_3, CONTBLACK, str3);
    relooktextstr (alerts[h].hts_4, CONTBLACK, str4);
    relooktextstr (alerts[h].hts_5, CONTBLACK, str5);

    relooksimplebutton (alerts[h].hbt_def, '@', CONTGREEN, def);
    alerts[h].bt_def = BUT_OFF;

    if (alt1 != NULL)
    {
	relooksimplebutton (alerts[h].hbt_alt1, '@', CONTBLUE, alt1);
	alerts[h].bt_alt1 = BUT_OFF;
    }
    else
    {
	relooksimplebutton (alerts[h].hbt_alt1, '@', CONTBLUE, "        ");
	alerts[h].bt_alt1 = BUT_INV;
    }

    if (alt2 != NULL)
    {
	relooksimplebutton (alerts[h].hbt_alt2, '@', CONTBLUE, alt2);
	alerts[h].bt_alt2 = BUT_OFF;
    }
    else
    {
	relooksimplebutton (alerts[h].hbt_alt2, '@', CONTBLUE, "        ");
	alerts[h].bt_alt2 = BUT_INV;
    }

    actionnecontexts (alerts[h].ctx, actionne);
    installcontexts (alerts[h].ctx);
    changefocus (alerts[h].hbt_def);
    return (0);
}

int     create_alerts (int x, int y, int larg, int haut)
{
    int     h;
    char    name[10];

    for (h = 1; h < NBMAXALERT; h++)
	if (alerts[h].used == 0)
	    break;

    if (h == NBMAXALERT)
    {
	fprintf (stderr, "no more free alerts\n");
	return 0;
    }

    alerts[h].used = 1;
    sprintf (name, "alert%-2d", h);
    alerts[h].ctx = createcontexts (x, y, larg, haut, CONTWHITE, name, NULL);
    installcontexts (alerts[h].ctx);

    alerts[h].hbt_def = createsimplebutton (CONTRBUTTON, x + ((larg - 2) / 6) - 4, y + haut - 2, 0xd, CONTGREEN, CONTUNDEF, &alerts[h].bt_def, "1234567890");

    alerts[h].hbt_alt1 = createsimplebutton (CONTRBUTTON, x + ((larg - 2) / 6) + 1 * (larg / 3) - 4, y + haut - 2, 0, CONTYELLOW, CONTUNDEF, &alerts[h].bt_alt1, "1234567890");

    alerts[h].hbt_alt2 = createsimplebutton (CONTRBUTTON, x + ((larg - 2) / 6) + 2 * (larg / 3) - 4, y + haut - 2, 0, CONTYELLOW, CONTUNDEF, &alerts[h].bt_alt2, "1234567890");

    alerts[h].hts_title = createtextstr (CONTTEXTSTR, x + 1, y, CONTRED, larg - 2, "alert");
    alerts[h].hts_1 = createtextstr (CONTTEXTSTR, x + 1, y + 1, CONTBLACK, larg - 2, "line 1");
    alerts[h].hts_2 = createtextstr (CONTTEXTSTR, x + 1, y + 2, CONTBLACK, larg - 2, "line 2");
    alerts[h].hts_3 = createtextstr (CONTTEXTSTR, x + 1, y + 3, CONTBLACK, larg - 2, "line 3");
    alerts[h].hts_4 = createtextstr (CONTTEXTSTR, x + 1, y + 4, CONTBLACK, larg - 2, "line 4");
    alerts[h].hts_5 = createtextstr (CONTTEXTSTR, x + 1, y + 5, CONTBLACK, larg - 2, "line 5");

    desinstallcontexts (alerts[h].ctx);

    return h;
}
