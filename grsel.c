/* 
 * $Id: grsel.c,v 1.2 2003/03/19 16:45:47 jd Exp $
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
 * $Log: grsel.c,v $
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
 * Revision 1.2.0.22.0.3  1999/08/30 22:16:03  jd
 * inclusion corrections
 *
 * Revision 1.2.0.22.0.2  1999/08/29 22:47:58  jd
 * initial release
 *
 */

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "config.h"

#ifdef USEERRNOH
#include <errno.h>
#endif

#include "sample.h"
#include "placement.h"
#include "controls.h"
#include "alert.h"
#include "fileselect.h"
#include "readsample.h"
#include "grov_io.h"
#include "raw2disk.h"
#include "menu_board.h"
#include "grpattern.h"
#include "groovit.h"

char    cursamplepath[MAXDIRSIZE];	/* le chemin courant de fileselect sample */

/* ---------------- les handles de controls a conserver ----------------------- */
static int fileselectctx;      /* le contexts du selecteur de fichier          */
static int statwin;	       /* handle de la fenetre de stat-file            */

static int statingfile = 0;    /* on est en train de stater un fichier         */
static int saveandquit = 0;    /* on est en train de sauver avant de quitter   */

int     initgrsel (void)
{
    fileselectctx = initfileselect ();
    strcpy (cursamplepath, SAMPLEPATH);
  /* those commented lines should be useless now .. */
  /* open_fileselect (cursamplepath, match_sound, sample[0].name,  " set sample ", " stat
     file  "); */
    statwin = create_alerts (ROWLEVEL, LINERYTHM, 35, 9);
  /* desinstallcontexts (fileselectctx); */
    return fileselectctx;
}

int     Aalert_stat (int c)
{
    if (poll_alerts (statwin))
    {
	quit_alerts (statwin);
	statingfile = 0;
    }
    return 0;
}

int     alert_stat (char *nom_file)
{
    statingfile = 1;
    return open_alerts (statwin, "stat file", "   OK   ", "  play  ", NULL,
			nom_file,
			NULL, NULL, NULL, NULL,
			NULL);
}

int     Agrsel_save (int c)
{
    int     action = fsel_poll ();

    if (action)
	switch (action)
	  {
	  case 1:		/* save/load current song */
	      {
		  char    buf[MAXDIRSIZE];

		  if (statingfile)
		  {
		      quit_alerts (statwin);
		      statingfile = 0;
		  }
		  quitfileselect (fileselectctx);
		  if (nom_file[0] != '/')
		  {
		      strcpy (buf, nom_dir);
		      strcat (buf, "/");
		      strcat (buf, nom_file);
		  }
		  else
		      strcpy (buf, nom_file);

		  {
		      char   *p;

		      if ((p = strrchr (buf, '/')) == NULL)
			  p = buf;
		      if ((p = strrchr (p, '.')) == NULL)
		      {
			  strcat (buf, ".grov");
		      }
		      fprintf (stderr, "saving current song to %s\n", buf);
		      save_cur_song (buf, 1);
		      wasmodified = 0;
		      strcpy (cursongfile, buf);
		      shortcursongfile = strrchr (cursongfile, '/');
		      if (shortcursongfile == NULL)
			  shortcursongfile = cursongfile;
		      else
			  shortcursongfile++;
		      settitle (gname, shortcursongfile);
		      if (saveandquit)
			  allpower = BUT_OFF;
		  }
	      }
	      break;

	  case 2:		/* stat sample-file */
	      {
		  char    buf[MAXDIRSIZE];

		  if (nom_file[0] != '/')
		  {
		      strcpy (buf, nom_dir);
		      strcat (buf, "/");
		      strcat (buf, nom_file);
		  }
		  else
		      strcpy (buf, nom_file);

		  alert_stat (nom_file);
	      }
	      break;

	  case -1:		/* cancel */
	      if (statingfile)
	      {
		  quit_alerts (statwin);
		  statingfile = 0;
	      }
	      quitfileselect (fileselectctx);
	      break;

	  default:
	    ;
	  }
    return 0;
}
int     grsel_save (char *cursongfile)
{
    return open_fileselect (startdir, match_grov, cursongfile,
			    " save song  ",
			    " stat song  ",
			    Agrsel_save);
}

int     Agrsel_load (int c)
{
    int     action = fsel_poll ();

    if (action)
	switch (action)
	  {
	  case 1:		/* save/load current song */
	      {
		  char    buf[MAXDIRSIZE];

		  if (statingfile)	/* JDJDJDJDJDJDJD */
		  {
		      quit_alerts (statwin);
		      statingfile = 0;
		  }
		  quitfileselect (fileselectctx);
		  if (nom_file[0] != '/')
		  {
		      strcpy (buf, nom_dir);
		      strcat (buf, "/");
		      strcat (buf, nom_file);
		  }
		  else
		      strcpy (buf, nom_file);
		  {
		      fprintf (stderr, "loading song %s to current\n", buf);
		      initallvoices ();
		      if (load_cur_song (buf, 1) != 0)
		      {
			  fprintf (stderr, "error song %s not loaded !\n", buf);
			/* JDJDJDJD come sanity jobs needed here !!! */
			  strcpy (cursongfile, "no-title");
			  shortcursongfile = cursongfile;
			  settitle (gname, shortcursongfile);
			  wasmodified = 0;
		      }
		      else
		      {
			  wasmodified = 0;
			  strcpy (cursongfile, buf);
			  shortcursongfile = strrchr (cursongfile, '/');
			  if (shortcursongfile == NULL)
			      shortcursongfile = cursongfile;
			  else
			      shortcursongfile++;
			  settitle (gname, shortcursongfile);
		      }
		      resync_displaypatterns ();
		      refreshscreen ();
		  }
	      }
	      break;

	  case 2:		/* stat sample-file */
	      {
		  char    buf[MAXDIRSIZE];

		  if (nom_file[0] != '/')
		  {
		      strcpy (buf, nom_dir);
		      strcat (buf, "/");
		      strcat (buf, nom_file);
		  }
		  else
		      strcpy (buf, nom_file);

		  alert_stat (nom_file);
	      }
	      break;

	  case -1:		/* cancel */
	      if (statingfile)
	      {
		  quit_alerts (statwin);
		  statingfile = 0;
	      }
	      quitfileselect (fileselectctx);
	      break;

	  default:
	    ;
	  }
    return 0;
}
int     grsel_load (char *cursongfile)
{
    return open_fileselect (startdir, match_grov, cursongfile,
			    " load song  ",
			    " stat song  ",
			    Agrsel_load);
}

FILE   *genrawname (char *rawname, int *numoutbis)
{
    char    b[MAXDIRSIZE];
    struct stat bs;
    int     n;
    FILE   *f = NULL;

    for (n = *numoutbis; n < 100; n++)
    {
	sprintf (b, "%s%d.wav", rawname, n);
	if (stat (b, &bs) != 0)
	    if (errno == ENOENT)
	    {
		f = openwaveout (b, HOPEDSPRATE, HOPEDRESO);
		if (f == NULL)
		{
		    fprintf (stderr, "impossible d'ouvrir %s aborting\n", b);
		}
		else
		{
		    fprintf (stderr, "ouverture de %s en mode raw\n", b);
		    break;
		}
	    }
    }
    *numoutbis = n;
    return f;
}
int     Agrsel_rawf (int c)    /* selection of raw file family */
{
    int     action = fsel_poll ();

    if (action)
	switch (action)
	  {
	  case 1:		/* set raw file */
	      {
		  char    buf[MAXDIRSIZE], *p;

		  if (statingfile)	/* JDJDJDJDJDJDJD */
		  {
		      quit_alerts (statwin);
		      statingfile = 0;
		  }
		  quitfileselect (fileselectctx);
		  if (nom_file[0] != '/')
		  {
		      strcpy (buf, nom_dir);
		      strcat (buf, "/");
		      strcat (buf, nom_file);
		  }
		  else
		      strcpy (buf, nom_file);

		  if ((p = strrchr (buf, '/')) == NULL)
		      p = buf;
		  if ((p = strrchr (p, '.')) != NULL)
		      if ((strcmp (p, ".wav") == 0) || (strcmp (p, ".WAV") == 0))
			  *p = 0;

		  fprintf (stderr, "set raw file to %s\n", buf);
		  strcpy (nfoutbis, buf);

		  if (nfoutbis[0] != 0)
		  {
		      numoutbis = 0;
		      if (foutbis != NULL)
			  closewaveout (foutbis);
		      foutbis = genrawname (nfoutbis, &numoutbis);
		      if (foutbis == NULL)
			  rawing2disk = BUT_DIS;
		      else
			  rawing2disk = BUT_OFF;
		      refreshcontrols (hrawing2disk);
		  }
	      }
	      break;

	  case 2:		/* stat sample-file */
	      {
		  char    buf[MAXDIRSIZE];

		  if (nom_file[0] != '/')
		  {
		      strcpy (buf, nom_dir);
		      strcat (buf, "/");
		      strcat (buf, nom_file);
		  }
		  else
		      strcpy (buf, nom_file);

		  alert_stat (nom_file);
	      }
	      break;

	  case -1:		/* cancel */
	      if (statingfile)
	      {
		  quit_alerts (statwin);
		  statingfile = 0;
	      }
	      quitfileselect (fileselectctx);
	      break;

	  default:
	    ;
	  }
    return 0;
}
int     grsel_rawf (char *currawfile)
{
    return open_fileselect (startdir, match_wav, currawfile,
			    " set rawfile",
			    NULL,
			    Agrsel_rawf);
}

int     Agrsel_sample (int c)
{
    int     action = fsel_poll ();

    if (action)
	switch (action)
	  {
	  case 1:		/* set file to sample */
	      {
		  char    buf[MAXDIRSIZE];
		  Sample *newsample = &sample[radiosample.value];

		  if (statingfile)
		  {
		      quit_alerts (statwin);
		      statingfile = 0;
		  }
		  strcpy (cursamplepath, nom_dir);
		  quitfileselect (fileselectctx);
		  cursamp[radiosample.value] = 0;
		  freesample (newsample);
		  if (nom_file[0] != '/')
		  {
		      strcpy (buf, nom_dir);
		      strcat (buf, "/");
		      strcat (buf, nom_file);
		  }
		  else
		      strcpy (buf, nom_file);
#ifdef VERYNASTYDEBUG
		  fprintf (stderr, "attributing %s to sample[%d]\n", buf, radiosample.value);
#endif
		  if (readsample (buf, newsample) == NULL)
		      freesample (newsample);
		  relooksimplebutton (hbt_sample[radiosample.value], '@', CONTBLUE + CONTNOPUCE, newsample->shortname);
		  zeroradio (&radiosample);
		  break;
	      }
	  case 2:		/* stat sample-file */
	      {
		  char    buf[MAXDIRSIZE];

		  if (nom_file[0] != '/')
		  {
		      strcpy (buf, nom_dir);
		      strcat (buf, "/");
		      strcat (buf, nom_file);
		  }
		  else
		      strcpy (buf, nom_file);

		  alert_stat (nom_file);
		  break;
	      }
	  case -1:		/* cancel */
	      if (statingfile)
	      {
		  quit_alerts (statwin);
		  statingfile = 0;
	      }
	      strcpy (cursamplepath, nom_dir);
	      quitfileselect (fileselectctx);
	      zeroradio (&radiosample);
	  default:
	    ;
	  }
    return 0;
}
int     grsel_sample (char *samplefname)
{
    return open_fileselect (cursamplepath, match_sound, samplefname,
			    " set sample ",
			    " stat file  ",
			    Agrsel_sample);
}
