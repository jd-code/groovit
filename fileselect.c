/* 
 * $Id: fileselect.c,v 1.2 2003/03/19 16:45:47 jd Exp $
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
 *
 * $Log: fileselect.c,v $
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
 * Revision 1.2.0.22.0.3  1999/08/29 22:49:01  jd
 * added actionne usage
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

#include "config.h"

#include "placement.h"
#include "sample.h"
#include "controls.h"
#include "alert.h"

#define JDFILESELECT
#include "fileselect.h"

#ifdef SHOWRCS
static char *rcsid = "$Id: fileselect.c,v 1.2 2003/03/19 16:45:47 jd Exp $";

#endif
static int firstuse = 1;

static char mycurpath[MAXDIRSIZE], *lmycurpath = NULL;
static char **matchesforisagoodfile = NULL;	/* as is "isagoodfile" is used by

						   func-pointers, */
						/* we need a global var to transmit
						   parameters   */
int     isagooddir (CONSTDIRENT struct dirent *thedirenttocheck)
{
    struct stat bufstat;

    if (lmycurpath != NULL)
    {
	strcpy (lmycurpath, thedirenttocheck->d_name);
	if (lstat (mycurpath, &bufstat) == -1)
	    return 0;
    }
    else if (lstat (thedirenttocheck->d_name, &bufstat) == -1)
	return 0;

    if (S_ISLNK (bufstat.st_mode))
	return 1;

    if (S_ISDIR (bufstat.st_mode))
	return 1;
    else
	return 0;
}

int     isagoodfile (CONSTDIRENT struct dirent *t)
{
    char  **p = matchesforisagoodfile;
    struct stat bufstat;

    if (lmycurpath != NULL)
    {
	strcpy (lmycurpath, t->d_name);
	if (lstat (mycurpath, &bufstat) == -1)
	    return 0;
    }
    else if (lstat (t->d_name, &bufstat) == -1)
	return 0;

    if (S_ISREG (bufstat.st_mode) || S_ISLNK (bufstat.st_mode))
    {
	if (p == NULL)
	    return 1;
	else
	    while ((*p != NULL) && (strcmp (t->d_name + strlen (t->d_name) - strlen (*p), *p) != 0))
		p++;
	if (*p == NULL)
	    return 0;
	else
	    return 1;
    }
    else
	return 0;
}

static int ctx = -1,	       /* contexts du selecteur de fichier     */
        alerts = -1,	       /* handle de l'alerts locale            */
        nbdirs = 0,	       /* nombre de repertoires dans la liste  */
        hlb_dirs = -1,	       /* handle de la listbox des repertoires */
        sel_dir = -1,	       /* le repertoire en cour de selection   */
        last_sel_dir = -1,     /* le meme en valeur preserve pour test */
        hte_dir = -1,	       /* handle du textedit du nom de repertoire */
        hbt_chdir = -1,	       /* handle du boutton de changement de rep */
        bt_chdir = BUT_OFF,    /* son etat                             */
        nbfiles = 0,	       /* nombre de fichiers dans la liste     */
        hlb_files = -1,	       /* handle de la listbox des fichiers    */
        sel_file = -1,	       /* le fichier en cour de selection      */
        last_sel_file = -1,    /* le meme en valeur preserve pour test */
        hte_file = -1,	       /* handle du textedit du nom de fichier */
        hbt_action1 = -1,      /* handle du bouton d'action1 (default) */
        bt_action1 = BUT_DIS,  /* valeur du bouton d'action1           */
        hbt_action2 = -1,      /* handle du bouton d'action2 (default) */
        bt_action2 = BUT_DIS,  /* valeur du bouton d'action2           */
        hbt_cancel = -1,       /* handle du bouton de cancel           */
        bt_cancel = BUT_OFF;   /* valeur du bouton de cancel           */

static char **files_tomatch = NULL,	/* the match strings for files... */
        fsel_curpath[MAXDIRSIZE];	/* our current dir */

char    nom_dir[MAXDIRSIZE],   /* nom du repertoire */
        nom_file[MAXDIRSIZE];  /* nom du fichier */

static
char   *emptylist[] =
{"empty"},		       /* used for empty list box */
      **dirslist = NULL,       /* la liste des repertoires */
      **fileslist = NULL;      /* la liste des fichiers */

int     fsel_scandir (char *path, char **tomatch)
{
    static struct dirent **namedirlist = NULL, **namefilelist = NULL;

    struct stat bufstat;

    int     j, nb, nbinlist;
    struct dirent **sauve1;
    char  **sauve2;

    strcpy (fsel_curpath, path);
    if (fsel_curpath[strlen (fsel_curpath) - 1] != '/')
	strcat (fsel_curpath, "/");
    strcpy (mycurpath, fsel_curpath);
    lmycurpath = mycurpath + strlen (mycurpath);
    matchesforisagoodfile = NULL;

    sauve1 = namedirlist;
    sauve2 = dirslist;
    nb = scandir (path, &namedirlist, &isagooddir, alphasort);
    if (nb == -1)
    {
	fprintf (stderr, "scandir [1] a merdu errno: %d\n", errno);
	namedirlist = sauve1;
	dirslist = sauve2;
	return 1;
    }
    else
    {
	if (sauve1 != NULL)
	{
	    for (j = 0; j < nbdirs; j++)
		free (sauve1[j]);
	    free (sauve1);
	}
	if (sauve2 != NULL)
	    free (sauve2);
	nbdirs = nb;
	dirslist = (char **) malloc (nbdirs * sizeof (char *));		/* JDJDJDJD test-it ? 

									 */
	nbinlist = 0;
	for (j = 0; j < nbdirs; j++)
	{
	    strcpy (lmycurpath, namedirlist[j]->d_name);
	    if (stat (mycurpath, &bufstat) == -1)
		continue;
	    if (S_ISDIR (bufstat.st_mode))
		dirslist[nbinlist++] = namedirlist[j]->d_name;
	}
	reassignlistbox (hlb_dirs, nbinlist, dirslist);
    }

    if (tomatch == NULL)
	matchesforisagoodfile = files_tomatch;
    else
    {
	matchesforisagoodfile = tomatch;
	files_tomatch = tomatch;
    }
    sauve1 = namefilelist;
    sauve2 = fileslist;
    nb = scandir (path, &namefilelist, isagoodfile, alphasort);
    if (nb == -1)
    {
	fprintf (stderr, "scandir [2] a merdu errno: %d\n", errno);
	namefilelist = sauve1;
	fileslist = sauve2;
	return 2;		/* JDJDJDJD quand on arrive la, on est bien dans la merde... */
    }
    else
    {
	if (sauve1 != NULL)
	{
	    for (j = 0; j < nbfiles; j++)
		free (sauve1[j]);
	    free (sauve1);
	}
	if (sauve2 != NULL)
	    free (sauve2);
	nbfiles = nb;
	fileslist = (char **) malloc (nbfiles * sizeof (char *));

	nbinlist = 0;
	for (j = 0; j < nbfiles; j++)
	{
	    strcpy (lmycurpath, namefilelist[j]->d_name);
	    if (stat (mycurpath, &bufstat) == -1)
		continue;
	    if (S_ISREG (bufstat.st_mode))
		fileslist[nbinlist++] = namefilelist[j]->d_name;
	}
	reassignlistbox (hlb_files, nbinlist, fileslist);
    }
    bt_action1 = BUT_OFF;
    return 0;
}

int     Aalert_simple (int c)
{
    if (poll_alerts (alerts))
	quit_alerts (alerts);
    return (0);
}

int     alert_noaccess (char *tent_newdir)
{
    return open_alerts (alerts, "error", "   OK    ", NULL, NULL,
			"    do not have permission to access :",
			tent_newdir,
			NULL, NULL, NULL,
			Aalert_simple);
}
int     alert_cannotcd (char *tent_newdir)
{
    return open_alerts (alerts, "error",
			"   OK    ", NULL, NULL,
			"    could not change to dir :",
			tent_newdir,
			NULL, NULL, NULL,
			Aalert_simple);
}

int     fsel_poll (void)
{
    char    buf[MAXDIRSIZE], newdirname[MAXDIRSIZE], *tent_newdir = NULL, *p = NULL;

    if (bt_action1 == BUT_ON)
    {
	bt_action1 = BUT_OFF;
	refreshcontrols (hbt_action1);
	return 1;
    }
    if (bt_action2 == BUT_ON)
    {
	bt_action2 = BUT_OFF;
	refreshcontrols (hbt_action2);
	return 2;
    }
    if (bt_cancel == BUT_ON)
    {
	bt_cancel = BUT_OFF;
	refreshcontrols (hbt_cancel);
	return -1;
    }

    if (bt_chdir == BUT_ON)
    {
	bt_chdir = BUT_OFF;
	refreshcontrols (hbt_chdir);

	if (getcwd (buf, MAXDIRSIZE) == NULL)
	{
	    fprintf (stderr, "current dir is very big pathname ? aborting\n");
	    return 0;
	}

	if (nom_dir[0] == '/')
	{
	    p = NULL;
	    tent_newdir = nom_dir;
	}
	else
	{
	    p = fsel_curpath + strlen (fsel_curpath);
	    strcat (fsel_curpath, nom_dir);
	    tent_newdir = fsel_curpath;
	}

	if (chdir (tent_newdir) != 0)
	{
	    switch (errno)
	      {
	      case EPERM:
	      case EACCES:
/* if (curcontext == ctx) */
		  alert_noaccess (tent_newdir);
/* else fprintf (stderr, "do not have permission to access %s\n", tent_newdir); */
		  break;
	      default:
/* if (curcontext == ctx) */
		  alert_cannotcd (tent_newdir);
/* else fprintf (stderr, "could not change to dir %s errno: %d\n", tent_newdir, errno); */
	      }
	    last_sel_dir = sel_dir;
	    if (p != NULL)
		*p = 0;
	    return 0;
	}
	if (getcwd (newdirname, MAXDIRSIZE) == NULL)
	{
	    fprintf (stderr, "%s very big pathname ? aborting\n", dirslist[sel_dir]);
	    chdir (buf);	/* JDJDJD perhaps should check here too ?... */
	    if (p != NULL)
		*p = 0;
	    reassigntextedit (hte_dir, -1, newdirname, -1);
	    return 0;
	}
	if (fsel_scandir (newdirname, NULL) == 1)
	{
	    if (p != NULL)
		*p = 0;
	}
	else
	    reassigntextedit (hte_dir, -1, newdirname, -1);

	last_sel_dir = sel_dir;
	chdir (buf);		/* JDJDJD perhaps should check here too ?... */
    }

    if (sel_dir != last_sel_dir)	/* JDJDJDJD maybe should test that sel_dir is valid ? 
					 */
    {
	if (getcwd (buf, MAXDIRSIZE) == NULL)
	{
	    fprintf (stderr, "current dir is very big pathname ? aborting\n");
	    return 0;
	}
	p = fsel_curpath + strlen (fsel_curpath);
	strcat (fsel_curpath, dirslist[sel_dir]);
	if (chdir (fsel_curpath) != 0)
	{
	    switch (errno)
	      {
	      case EPERM:
	      case EACCES:
		  alert_noaccess (fsel_curpath);
/* JDJDJD on peut supprimer ce stderr ... */
		  fprintf (stderr, "do not have permission to access %s\n", fsel_curpath);
		  break;
	      default:
/* if (curcontext == ctx) */
		  alert_cannotcd (fsel_curpath);
/* else fprintf (stderr, "could not change to dir %s errno: %d\n", fsel_curpath, errno); */
	      }
	    last_sel_dir = sel_dir;
	    *p = 0;
	  /* reassigntextedit (hte_dir, -1, newdirname, -1); */
	    reassigntextedit (hte_dir, -1, fsel_curpath, -1);
	    return 0;
	}
	if (getcwd (newdirname, MAXDIRSIZE) == NULL)
	{
	    fprintf (stderr, "%s very big pathname ? aborting\n", dirslist[sel_dir]);
	    chdir (buf);	/* JDJDJD perhaps should check here too ?... */
	    *p = 0;
	    reassigntextedit (hte_dir, -1, newdirname, -1);
	    return 0;
	}
	reassigntextedit (hte_dir, -1, newdirname, -1);
	if (fsel_scandir (newdirname, NULL) == 1)
	{
	    *p = 0;
	}

	last_sel_dir = sel_dir;
	chdir (buf);		/* JDJDJD perhaps should check here too ?... */
    }
    if (sel_file != last_sel_file)
    {
	if (sel_file != -1)
	{
	    reassigntextedit (hte_file, -1, fileslist[sel_file], -1);
	    changefocus (hte_file);
	}
	last_sel_file = sel_file;
    }
    return 0;
}

int     open_fileselect (char *initdir, char **tomatch, char *curfile, char *action1, char *action2, Tactionne actionne)
{
    if (curfile != NULL)
	reassigntextedit (hte_file, -1, curfile, -1);
    else
	reassigntextedit (hte_file, -1, "", -1);

    if (action1 != NULL)
	relooksimplebutton (hbt_action1, 0xd, CONTYELLOW, action1);
    else
	relooksimplebutton (hbt_action1, 0xd, CONTYELLOW, "   select   ");
    bt_action1 = BUT_OFF;

    if (action2 != NULL)
    {
	relooksimplebutton (hbt_action2, 0xd, CONTYELLOW, action2);
	bt_action2 = BUT_OFF;
    }
    else
    {
	relooksimplebutton (hbt_action2, 0xd, CONTYELLOW, "            ");
	bt_action2 = BUT_DIS;
    }
    bt_cancel = BUT_OFF;

    reassigntextedit (hte_dir, -1, initdir, -1);

    fsel_scandir (initdir, tomatch);
    actionnecontexts (ctx, actionne);
    installcontexts (ctx);

    return 0;
}

int     initfileselect (void)
{
    if (firstuse)
    {
#ifdef SHOWRCS
	fprintf (stderr, "%s\n\r", rcsid);
#endif
	firstuse = 0;
    }

    if (ctx != -1)
    {
	fprintf (stderr, "initfileselect called without closing the previous one ???\n");
	return -1;
    }
    ctx = createcontexts (ROWFILESEL, LINEFILESEL, LARGFILESEL, HAUTFILESEL, CONTWHITE, "selector", NULL);
    installcontexts (ctx);

    hlb_dirs = createlistbox (CONTLISTBOX, ROWFILESEL, LINEFILESEL + 2, '@', CONTUNDEF, LARGDIRNAME, HAUTFILESEL - 2 - 8, &sel_dir, 1, emptylist);

    hbt_chdir = createsimplebutton (CONTRBUTTON, ROWFILESEL + LARGFILESEL - 8, LINEFILESEL, 0xd, CONTYELLOW, CONTUNDEF, &bt_chdir, "change");

    hte_dir = createtextedit (CONTTEXTEDIT, ROWFILESEL, LINEFILESEL, '@', CONTUNDEF, LARGFILESEL - 8, sizeof (nom_dir), nom_dir, hbt_chdir);

    hlb_files = createlistbox (CONTLISTBOX, ROWFILESEL + LARGDIRNAME + 2, LINEFILESEL + 2, '@', CONTUNDEF, LARGFILESEL - 3 - 14, HAUTFILESEL - 4, &sel_file, 1, emptylist);

    hbt_action1 = createsimplebutton (CONTRBUTTON, ROWFILESEL + 1, LINEFILESEL + HAUTFILESEL - 3, 0xd, CONTYELLOW, CONTUNDEF, &bt_action1, "-EMPTY------");

    hbt_action2 = createsimplebutton (CONTRBUTTON, ROWFILESEL + 1, LINEFILESEL + HAUTFILESEL - 5, ' ', CONTYELLOW, CONTUNDEF, &bt_action2, "-EMPTY------");

    hbt_cancel = createsimplebutton (CONTRBUTTON, ROWFILESEL + 1, LINEFILESEL + HAUTFILESEL - 7, '\033', CONTYELLOW, CONTUNDEF, &bt_cancel, "   cancel   ");

    strcpy (nom_file, "");
    hte_file = createtextedit (CONTTEXTEDIT, ROWFILESEL + 1, LINEFILESEL + HAUTFILESEL - 1, '@', CONTUNDEF, LARGFILESEL - 2, sizeof (nom_file), nom_file, hbt_action1);

    init_alerts ();
    alerts = create_alerts (8, 7, 80 - 16, 9);

    desinstallcontexts (ctx);
    return ctx;
}

void    quitfileselect (int h)
{
    actionnecontexts (ctx, NULL);
    desinstallcontexts (ctx);
}
