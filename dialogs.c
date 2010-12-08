/* 
 * $Id: dialogs.c,v 1.2 2003/03/19 16:45:47 jd Exp $
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
 * $Log: dialogs.c,v $
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
 * Revision 1.2.0.23.0.5  1999/10/07 14:08:05  jd
 * Revision 1.2.0.23.0.3  1999/10/02 17:22:04  jd
 * modified gpl handling
 *
 * Revision 1.2.0.23.0.2  1999/09/30 01:09:04  jd
 * added gpl handling with rc file
 * corrected gpl announces
 *
 * Revision 1.2.0.23  1999/09/15 10:20:39  jd
 * second pre tel-aviv public revision, for testing
 *
 * Revision 1.2.0.22.0.3  1999/08/30 22:16:03  jd
 * adapted includes
 *
 * Revision 1.2.0.22.0.2  1999/08/29 22:49:01  jd
 * adapted to fileselect's actionne
 *
 * Revision 1.1  1999/08/29 16:54:59  jd
 * Initial revision
 *
 */

#include <stdio.h>
#include <string.h>

#include "config.h"
#include "sample.h"
#include "placement.h"
#include "controls.h"
#include "alert.h"
#include "fileselect.h"
#include "grsel.h"
#include "menu_board.h"
#include "settings.h"
#include "groovit.h"

/* ---------------- les handles de controls a conserver ----------------------- */
static int alerts;	       /* handle de la fenetre d'alerte                */

int     initdialogs (void)
{
    alerts = create_alerts (8, 7, 80 - 16, 9);
    return alerts;
}

int     Aalert_nothing (int c)
{
    if (poll_alerts (alerts))
	quit_alerts (alerts);

    return (0);
}
int     alert_about (char *rcsid)
{
    return open_alerts (alerts, "groovit", " agreed ", "   of   ", "course !",
			"    groovit - alpha version - (C) 1999 Jean-Daniel Pauget.",
			"           Making accurate and groovy sound/noise.",
			rcsid,
			"   groovit comes with absolutely NO WARRANTY. Please refer",
			"   to the file GPL.txt or try http://www.gnu.org/ .",
			Aalert_nothing
	);
}
int     Aalert_gpl (int c)
{
    switch (poll_alerts (alerts))
      {
      case 0:
	  break;
	  break;
      case 3:
	  allpower = BUT_OFF;
      case 1:
      default:
	  {
	      void   *tab[] =
	      {(void *) &gpl_accepted,
	       (void *) &readrevision,
	       NULL
	      };

	      gpl_accepted = 1;
	      write_pref (&tab[0]);
	      quit_alerts (alerts);
	  }
      }
    return (0);
}
int     alert_gpl (void)
{
    return open_alerts (alerts, "groovit", " agreed ", NULL, "   no   ",
			"   groovit         Making accurate and groovy sound/noise.",
			"   alpha version              (C) 1999 Jean-Daniel Pauget.",
			"   groovit comes with absolutely NO WARRANTY. Please refer",
			"   to the file GPL.txt or try http://www.gnu.org/ . If you",
			"   agree with the license conditions, press <agree>.",
			Aalert_gpl
	);
}
int     Aalert_era (int c)
{
    switch (poll_alerts (alerts))
      {
      case 0:
	  break;
      case 3:
	  initallvoices ();
	  resync_displaypatterns ();
	  strcpy (cursongfile, "no-title");
	  shortcursongfile = cursongfile;
	  settitle (gname, shortcursongfile);
	  refreshscreen ();
	  quit_alerts (alerts);
	  break;
      default:
	  quit_alerts (alerts);
	  break;
      }
    return (0);
}
int     alert_era (void)
{
    return open_alerts (alerts, "groovit", "  No !  ", NULL, "  Yes ! ",
			"    current song :",
			cursongfile,
			"    has been modified",
			NULL,
			"   Proceed with erasing anyway ?",
			Aalert_era
	);
}

/* load song anyway ? */
int     Aalert_lany (int c)
{
    switch (poll_alerts (alerts))
      {
      case 0:
	  break;
      case 3:
	  quit_alerts (alerts);
	  grsel_load (cursongfile);
	  break;
      case 1:
	  quit_alerts (alerts);
	  grsel_save (cursongfile);
      }
    return (0);
}
int     alert_lany (void)
{
    return open_alerts (alerts, "groovit", "  No !  ", NULL, "  Yes ! ",
			"    current song :",
			cursongfile,
			"    has been modified",
			NULL,
			"   Proceed with loading anyway ?",
			Aalert_lany
	);
}

int     Aalert_quit (int c)
{
    switch (poll_alerts (alerts))
      {
      case 0:
	  break;
      case 3:
	  allpower = BUT_OFF;
      default:
	  quit_alerts (alerts);
      }
    return (0);
}
int     alert_quit (void)
{
    return open_alerts (alerts, "groovit", " Nooo ! ", NULL, "  Yes.  ",
			NULL,
			"           You're going to leave groovit and you",
			"                  REALLY want to QUIT ?",
			NULL,
			NULL,
			Aalert_quit
	);
}

int     Aalert_savequit (int c)
{
    switch (poll_alerts (alerts))
      {
      case 0:
	  break;
      case 3:
	  allpower = BUT_OFF;
	  break;
      default:
	  quit_alerts (alerts);
	  grsel_save (cursongfile);
      }
    return (0);
}
int     alert_savequit (void)
{
    return open_alerts (alerts, "groovit", "  No !  ", NULL, "  Yes ! ",
			"    current song :",
			cursongfile,
			"    has been modified",
			NULL,
			"   Proceed with quiting anyway ?",
			Aalert_savequit
	);
}
