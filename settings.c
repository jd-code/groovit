/* 
 * $Id: settings.c,v 1.2 2003/03/19 16:45:47 jd Exp $
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
 * $Log: settings.c,v $
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
 * Revision 1.2.0.23.0.6  1999/10/07 14:08:05  jd
 * Revision 1.2.0.23.0.5  1999/10/05 23:04:46  jd
 * corrected bug saving un-prcedently written options
 * added playahead
 * finished fontnames
 * first saverc revision
 *
 * Revision 1.2.0.23.0.3  1999/10/02 17:22:04  jd
 * complete rewrite of scan_rcfile
 * handles partial/complete total/local write/read.
 *
 * Revision 1.2.0.23.0.2  1999/09/30 01:09:04  jd
 * added embryonic rcfile handling
 *
 * Revision 1.2.0.23.0.1  1999/09/15 10:35:48  jd
 * *** empty log message ***
 *
 * Revision 1.2.0.23  1999/09/15 10:20:39  jd
 * second pre tel-aviv public revision, for testing
 *
 * Revision 1.2.0.22.0.2  1999/08/30 22:16:03  jd
 * inclusion corrections
 *
 * Revision 1.2.0.22.0.1  1999/08/30 08:49:27  jd
 * Initial revision
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pwd.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>

#include "config.h"

#include "sample.h"
#include "grov_io.h"
#include "placement.h"
#include "controls.h"
#include "fileselect.h"

#define INMAINSETTING
#include "settings.h"

int     ctxsttg,	       /* handle du context de settings                */
        hsttgcancel,	       /* handle du bouton cancel de settings          */
        hsttgapply,	       /* handle du bouton apply de settings           */
        hsttgsave,	       /* handle du bouton save de settings            */
        hfont;		       /* handle du text de font                       */
Radio   radiosttg;	       /* la valeur de sortie de setting               */

#define MAXNBFONT 14
int     fontindex = 8,	       /* fonte en cours d'usage                       */
        curfontindex,	       /* fonte en cours d'usage pour comparaison      */
        lastfontindex;	       /* sauvegarde de fonte en cours d'usage         */
char    nomfont[MAXNBFONT][MAXDIRSIZE];		/* le nom des fontes

						 */

int     curkplayahead;	       /* playahead copie de circulation               */

Settings settings[] =	       /* la famille settings et leurs voisins d'en face */
{
    {"Groovit*gpl_accepted", "0", SETT_INT, SETT_LOCAL, 0, &gpl_accepted},
    {"Groovit*revision", "-1", SETT_VER, SETT_LOCAL, 0, &readrevision},
    {"Groovit*numfont", "8", SETT_INT, SETT_NORMAL, 0, &fontindex},
    {"Groovit*Kplayahead", "8", SETT_LONG, SETT_NORMAL, 0, &kplayahead},

    {"Groovit*font0", "5x7", SETT_STR, SETT_NORMAL, 0, nomfont[0]},
    {"Groovit*font1", "5x8", SETT_STR, SETT_NORMAL, 0, nomfont[1]},
    {"Groovit*font2", "6x9", SETT_STR, SETT_NORMAL, 0, nomfont[2]},
    {"Groovit*font3", "6x10", SETT_STR, SETT_NORMAL, 0, nomfont[3]},
    {"Groovit*font4", "6x12", SETT_STR, SETT_NORMAL, 0, nomfont[4]},
    {"Groovit*font5", "6x13", SETT_STR, SETT_NORMAL, 0, nomfont[5]},
    {"Groovit*font6", "7x13", SETT_STR, SETT_NORMAL, 0, nomfont[6]},
    {"Groovit*font7", "7x14", SETT_STR, SETT_NORMAL, 0, nomfont[7]},
    {"Groovit*font8", "8x13", SETT_STR, SETT_NORMAL, 0, nomfont[8]},
    {"Groovit*font9", "8x16", SETT_STR, SETT_NORMAL, 0, nomfont[9]},
    {"Groovit*font10", "9x15", SETT_STR, SETT_NORMAL, 0, nomfont[10]},
    {"Groovit*font11", "10x20", SETT_STR, SETT_NORMAL, 0, nomfont[11]},
    {"Groovit*font12", "12x24", SETT_STR, SETT_NORMAL, 0, nomfont[12]},
    {"Groovit*font13", "user", SETT_STR, SETT_NORMAL, 0, nomfont[13]},

    {NULL, NULL, 0, 0, 0, NULL}
};

static char myrcfile[MAXDIRSIZE];	/* le rcfile ou on sauve                */

char   *ltover (long vernumber)
{
    static char buf[30];

    if (vernumber & 0xFFFF)
	snprintf (buf, 30, "%ld.%ld.%ld.%ld",
		  ((vernumber & 0xFF000000) >> 24),
		  ((vernumber & 0x00FF0000) >> 16),
		  ((vernumber & 0x0000FF00) >> 8),
		  (vernumber & 0x000000FF)
	    );
    else
	snprintf (buf, 30, "%ld.%ld",
		  ((vernumber & 0xFF000000) >> 24),
		  ((vernumber & 0x00FF0000) >> 16)
	    );
    return buf;
}

int     getboolean (char *buf)
{
    if (buf == NULL)
	return 0;
    if (strncasecmp (buf, "TRUE", 4) == 0)
	return 1;
    if (strncasecmp (buf, "YES", 3) == 0)
	return 1;
    if (strncasecmp (buf, "OUI", 3) == 0)
	return 1;
    return atoi (buf);
}

void    istoupdate (void *v)
{
    int     i = 0;

    while ((settings[i].tag != NULL) && (settings[i].v != v))
	i++;
    if (settings[i].tag != NULL)
    {
#ifdef DEB_RCFILE
	fprintf (stderr, "tags %s is marked changed\n", settings[i].tag);
#endif
	settings[i].checked = 1;
    }
    else
	fprintf (stderr, "warning: no setting match addr: %p\n", v);
}

int     Asettings (int c)
{
    int     i;

    if (fontindex != curfontindex)
    {
	if (is_control_modified (hfont))
	    istoupdate ((void *) &nomfont[curfontindex]);
	curfontindex = fontindex;
	reassigntextedit (hfont, 40, &nomfont[fontindex], -1);
	clearmodif (hfont);
    }
    if (radiosttg.value != -1)
    {

	switch (radiosttg.value)
	{
	    case 0:		/* cancel */
		i = 0;
		while (settings[i].tag != NULL)
		{
		    settings[i].checked = 0;
		    i++;
		}
		desinstallcontexts (ctxsttg);

		curkplayahead = kplayahead;
		curfontindex = lastfontindex;
		fontindex = lastfontindex;
		reassigntextedit (hfont, 40, &nomfont[fontindex], -1);
		radiosttg.value = -1;
		break;

	    case 2:		/* save */
	    case 1:		/* apply */
		if (is_control_modified (hfont))
		    istoupdate ((void *) &nomfont[curfontindex]);
		if (curkplayahead != kplayahead)
		{
		    if ((kplayahead < 0) || (kplayahead > (MAXPLAYAHEAD >> 10)))
			kplayahead = (MAXPLAYAHEAD >> 10);
		    kplayahead = curkplayahead;
		    playahead = (kplayahead << 10);
#ifdef HOPEDRESO16
		    sizebufahead = (playahead << 2);
#endif
#ifdef HOPEDRESO8
		    sizebufahead = (playahead << 1);
#endif
#ifdef HOPEDMONO16
		    sizebufahead = (playahead << 1);
#endif
		    istoupdate ((void *) &kplayahead);
		}
		if (lastfontindex != fontindex)
		{
		    lastfontindex = fontindex;
		    printf ("\033]50;%s\007", nomfont[fontindex]);	/* setfont */
		    istoupdate ((void *) &fontindex);
		}
		refreshcontrols (radiosttg.h[1]);
		if (radiosttg.value != 2)   /* c'est pas du save, on quitte */
		    break;

		{
		    void  **tab;
		    int     j = 0;

		    i = 0;
		    while (settings[i].tag != NULL)
			i++;
		    tab = (void **) malloc ((i + 1) * sizeof (void *));

		    if (tab == NULL)
		    {
			fprintf (stderr, "error: allocation failed in rcsave, save failed\n");
			break;
		    }

		    i = 0;
		    while (settings[i].tag != NULL)
		    {
			if (settings[i].checked)
			{
#ifdef DEB_RCFILE
			    fprintf (stderr, "tag %s to be saved\n", settings[i].tag);
#endif
			    tab[j] = settings[i].v;
			    j++;
			}
			i++;
		    }
		    if (j != 0)
		    {
			tab[j] = NULL;
			write_pref (&tab[0]);
		    }
		    free (tab);
		    i = 0;
		    while (settings[i].tag != NULL)
			settings[i++].checked = 0;
		    desinstallcontexts (ctxsttg);
		}
	}
    }
    radiosttg.value = -1;
    return (0);
}

int     scan_rcfile (char *rcfile, int mode, void **liste)
{
    int     i;
    FILE   *f, *new = NULL;
    long    ligne = 0;

    char    buf[MAXREADLINE], orig[MAXREADLINE], *p, *q, *r;
    Settings *match;
    long    lv;
    int     v;
    int global;

    global = (mode & SETT_GLOBAL);

    mode &= (~SETT_GLOBAL);

    if (((mode == SETT_WRITEALL) || (mode == SETT_WRITEMATCH)) && (liste != NULL))
    {
	i = 0;
	while (settings[i].tag != NULL)
	{
	    int     j = 0;

	    settings[i].checked = 0;
	    while (liste[j] != NULL)
	    {
		if (liste[j] == settings[i].v)
		    settings[i].checked = 1;
		j++;
	    }
	    i++;
	}
    }
    else
    {
	i = 0;
	while (settings[i].tag != NULL)
	    settings[i++].checked = 1;
    }

    switch (mode)
    {
	case SETT_READALL:
	case SETT_READMATCH:
	    if ((f = fopen (rcfile, "rt")) == NULL)
	    {
		perror (rcfile);
		fprintf (stderr, "could not open rcfile: %s, skipped\n", rcfile);
		if (!global)
		    return -1;
	    }
	    break;
	case SETT_WRITEALL:
	case SETT_WRITEMATCH:
	    {
		char    newname[MAXDIRSIZE];

		strncpy (newname, rcfile, MAXDIRSIZE);
		strncat (newname, ".bak", MAXDIRSIZE - strlen (newname));

		if (unlink (newname))
		    if (errno != ENOENT)
		    {
			perror (newname);
			fprintf (stderr, "error removing %s\n", newname);
		    }
		if (rename (rcfile, newname))
		{
		    perror ("in scan_rcfile: ");
		    fprintf (stderr, "error moving %s to %s\n", rcfile, newname);
		    fprintf (stderr, "rcfile saving aborted\n");
		    if (!global)
			return -1;
		}
		if ((f = fopen (newname, "rt")) == NULL)
		{
		    perror (newname);
		    fprintf (stderr, "could not open rcfile: %s, skipped\n", newname);
		    if (!global)
			return -1;
		}
		if ((new = fopen (rcfile, "wt")) == NULL)
		{
		    perror (rcfile);
		    fprintf (stderr, "could not open rcfile: %s, skipped\n", rcfile);
		    if (!global)
			return -1;
		}
		fprintf (stderr, "saving prefs to %s, backup is %s\n", rcfile, newname);
	    }
	    break;
	default:
	    fprintf (stderr, "call to scan_rcfile with unknown mode : %d. skipped\n", mode);
	    return -1;
    }
    if (f != NULL)
	while (fgets (buf, MAXREADLINE, f) == buf)
	{
	    ligne++;
	    p = buf;
	    strcpy (orig, buf);

	    while (isspace (*p))
		p++;
	    switch (*p)
	    {
		case 0:	/* end of line */
		case '!':	/* comment */
		    switch (mode)
		    {
			case SETT_WRITEALL:
			case SETT_WRITEMATCH:
			    fprintf (new, "%s", buf);
			    break;
		    }
		    continue;
	    }
	    q = p;
	    while ((*q != ':') && (*q != 0))
		q++;
	    if (*q == 0)
		r = q;
	    else
	    {
		r = q + 1;
		while (isspace (*r))
		    r++;
	    }
	    *q = 0;

	    i = 0;
	    while ((settings[i].tag != NULL) && (strcasecmp (p, settings[i].tag) != 0))
		i++;
	    if (settings[i].tag == NULL)
	    {
		switch (mode)
		{
		    case SETT_WRITEALL:
		    case SETT_WRITEMATCH:
			fprintf (new, "%s", orig);
		}
		continue;	/* on passe a la ligne suivante */
	    }
	    match = &settings[i];
	    if (match->checked == -1)
	    {
		fprintf (stderr, "%s: line %ld: redundant tag skipped\n", rcfile, ligne);
		switch (mode)
		{
		    case SETT_WRITEALL:
		    case SETT_WRITEMATCH:
			fprintf (new, "! line below is commented because it was redundant\n");
			fprintf (new, "!%s", orig);
		}
		continue;	/* on passe a la ligne suivante */
	    }
	    switch (match->type)
	    {
		case SETT_VER:
		    {
			int     i;

			lv = 0;
			for (i = 0; i < 4; i++)
			{
			    lv <<= 8;
			    lv += atoi (r);
			    while (isdigit (*r) && (*r != 0))
				r++;
			    if (*r != 0)
				r++;
			}
			switch (mode)
			{
			    case SETT_WRITEMATCH:
				if (match->checked == 0)
				{
				    fprintf (new, "%s", orig);
				    break;
				}
			    case SETT_WRITEALL:
				if (vernumber > lv)
				    fprintf (new, "%s:\t%s\n", buf, ltover (vernumber));
				else
				    fprintf (new, "%s", orig);
				break;
			    case SETT_READMATCH:
				if (match->checked == 0)
				    break;
			    case SETT_READALL:
				if (!global)
				    readrevision = lv;
				else
				{
				    if (lv > vernumber)
				    {
					fprintf (stderr, "\nglobal rc file show newer revision (%s) than yout groovit (%s) !\n", ltover (lv), ltover (vernumber));
					fprintf (stderr, "are you sure to use the latest groovit ?\n\n");
				    }
				}
			}
		    }
		    break;
		case SETT_BOOL:
		    v = getboolean (r);
		    switch (mode)
		    {
			case SETT_WRITEMATCH:
			    if (match->checked == 0)
			    {
				fprintf (new, "%s", orig);
				break;
			    }
			case SETT_WRITEALL:
			    if (*(int *) match->v != v)
				fprintf (new, "%s:\t%s\n", buf,
					 (*(int *) match->v) ? "TRUE" : "FALSE");
			    else
				fprintf (new, "%s", orig);
			    break;
			case SETT_READMATCH:
			    if (match->checked == 0)
				break;
			case SETT_READALL:
			    if ((global) &&(match->portee == SETT_LOCAL))
				break;
			    *(int *) match->v = v;
		    }
		    break;
		case SETT_INT:
		    v = atoi (r);
		    switch (mode)
		    {
			case SETT_WRITEMATCH:
			    if (match->checked == 0)
			    {
				fprintf (new, "%s", orig);
				break;
			    }
			case SETT_WRITEALL:
			    if (*(int *) match->v != v)
			    {
				fprintf (new, "%s:\t%d\n", buf, *(int *) match->v);
			    }
			    else
			    {
				fprintf (new, "%s", orig);
			    }
			    break;
			case SETT_READMATCH:
			    if (match->checked == 0)
				break;
			case SETT_READALL:
			    if ((global) &&(match->portee == SETT_LOCAL))
				break;
			    *(int *) match->v = v;
		    }
		    break;
		case SETT_LONG:
		    lv = atol (r);
		    switch (mode)
		    {
			case SETT_WRITEMATCH:
			    if (match->checked == 0)
			    {
				fprintf (new, "%s", orig);
				break;
			    }
			case SETT_WRITEALL:
			    if (*(long *) match->v != lv)
				fprintf (new, "%s:\t%ld\n", buf, *(long *) match->v);
			    else
				fprintf (new, "%s", orig);
			    break;
			case SETT_READMATCH:
			    if (match->checked == 0)
				break;
			case SETT_READALL:
			    if ((global) &&(match->portee == SETT_LOCAL))
				break;
			    *(long *) match->v = lv;
		    }
		    break;
		case SETT_STR:
		    {
			char   *q = &r[strlen (r) - 1];

#ifdef DEB_RCFILE
			fprintf (stderr, "une chaine grasse: >%s<\n", r);
#endif
			while ((q != r) && isspace (*q))
			    q--;
			if (q == r)
			    *q = 0;
			else
			    *(q + 1) = 0;
#ifdef DEB_RCFILE
			fprintf (stderr, "une chaine maigre: >%s<\n", r);
#endif
		    }
		    switch (mode)
		    {
			case SETT_WRITEMATCH:
			    if (match->checked == 0)
			    {
				fprintf (new, "%s", orig);
				break;
			    }
			case SETT_WRITEALL:
			    if (strcmp ((char *) match->v, r) != 0)
				fprintf (new, "%s:\t%s\n", buf, (char *) match->v);
			    else
				fprintf (new, "%s", orig);
			    break;
			case SETT_READMATCH:
			    if (match->checked == 0)
				break;
			case SETT_READALL:
			    if ((global) &&(match->portee == SETT_LOCAL))
				break;
			    strncpy ((char *) match->v, r, MAXDIRSIZE);
		    }
		    break;
		default:
		    fprintf (stderr, "reading %s: line %ld, unkown setting type %d for tag %s\n", rcfile, ligne, match->type, match->tag);
		    switch (mode)
		    {
			case SETT_WRITEMATCH:
			case SETT_WRITEALL:
			    fprintf (new, "%s", orig);
		    }
	    }
	    match->checked = -1;
	}
    if ((mode == SETT_WRITEALL) || (mode == SETT_WRITEMATCH))
    {
	i = 0;
	while (settings[i].tag != NULL)
	{
	    match = &settings[i];
	    if (match->checked == 1)
		switch (match->type)
		{
		    case SETT_VER:
			fprintf (new, "%s:\t%s\n", match->tag, ltover (vernumber));
			break;
		    case SETT_BOOL:
			fprintf (new, "%s:\t%s\n", match->tag,
				 (*(int *) match->v) ? "TRUE" : "FALSE");
			break;
		    case SETT_INT:
			fprintf (new, "%s:\t%d\n", match->tag, *(int *) match->v);
			break;
		    case SETT_LONG:
			fprintf (new, "%s:\t%ld\n", match->tag, *(long *) match->v);
			break;
		    case SETT_STR:
			fprintf (new, "%s:\t%s\n", match->tag, (char *) match->v);
			break;
		    default:
			fprintf (stderr, "reading %s: line %ld, unkown setting type %d for tag %s\n", rcfile, ligne, match->type, match->tag);
			switch (mode)
			{
			    case SETT_WRITEMATCH:
			    case SETT_WRITEALL:
				fprintf (new, "%s", orig);
			}
		}
	    i++;
	}
    }
    if ((mode == SETT_READALL) && (global))	/* affectation des valeurs par defaut */
    {
	i = 0;
	while (settings[i].tag != NULL)
	{
	    match = &settings[i];
	    if (match->checked == 1)
#ifdef DEB_RCFILE
	    {
		char   *type[] =
		{"undefined", "boolean", "int", "long", "vernumber", "string"};

		fprintf (stderr, "defaulted tag %s (%s) = \"%s\"\n",
			 match->tag,
			 ((match->type >= 0) && (match->type < 6)) ? type[match->type] : "unknown type",
			 match->defv);
#endif
		switch (match->type)
		{
		    case SETT_VER:	/* pas de valeur par defaut ici */
			break;
		    case SETT_BOOL:
			*(int *) match->v = getboolean (match->defv);
			break;
		    case SETT_INT:
			*(int *) match->v = atoi (match->defv);
			break;
		    case SETT_LONG:
			*(long *) match->v = atol (match->defv);
			break;
		    case SETT_STR:
			strncpy ((char *) match->v, match->defv, MAXDIRSIZE);
			break;
		    default:
			fprintf (stderr, "reading %s: line %ld, unkown setting type %d for tag %s\n", rcfile, ligne, match->type, match->tag);
#ifdef DEB_RCFILE
		}
#endif
	    }
	    i++;
	}
    }
    switch (mode)
    {
	case SETT_WRITEALL:
	case SETT_WRITEMATCH:
	    if (new != NULL)
		fclose (new);
	case SETT_READALL:
	case SETT_READMATCH:
	    if (f != NULL)
		fclose (f);
    }
    i = 0;
    while (settings[i].tag != NULL)
    {
	settings[i].checked = 0;
	i++;
    }
    if ((f == NULL) || (new == NULL))
	return -1;
    else
	return 0;
}

int     write_pref (void **liste)
{
    return scan_rcfile (myrcfile, SETT_WRITEMATCH, liste);
}

int     reset_settings (char *rcfile)
{
    uid_t   u, e;
    struct passwd *p;
    char    name[MAXDIRSIZE];
    int     r = -1;

    firsttimeforthis = 0;
    gpl_accepted = 0;
    r = scan_rcfile (GLOBALRCFILE, SETT_READALL + SETT_GLOBAL, NULL);

    u = getuid ();
    e = geteuid ();

    p = getpwuid (u);
    if ((p != NULL) && (p->pw_dir != NULL) && (p->pw_name != NULL))
    {
	if (u != e)
	    fprintf (stderr, "reading rc from ~%s.\n", p->pw_name);

	strncpy (name, p->pw_dir, MAXDIRSIZE);
	strncat (name, "/", MAXDIRSIZE - strlen (name));
	strncat (name, RCFILENAME, MAXDIRSIZE - strlen (name));

	if (rcfile != NULL)
	{
	    fprintf (stderr, "reading %s instead of %s\n", rcfile, name);
	    r = scan_rcfile (rcfile, SETT_READALL, NULL);
	}
	else
	{
	    FILE   *f;

	    if ((f = fopen (name, "rt")) == NULL)
	    {
		firsttimeforthis = 1;
		gpl_accepted = 0;
		fprintf (stderr, "creating initial %s file\n", name);
		if ((f = fopen (name, "wt")) == NULL)
		{
		    fprintf (stderr, "creation of %s failed, skipped\n", name);
		}
		else
		{
		    fprintf (f, "! your own configuration file for groovit\n");
		    fprintf (f, "! only personnal settings should be wrote here\n");
		    fprintf (f, "! global settings should be in %s\n", GLOBALRCFILE);
		    fprintf (f, "! most settings can be set in groovit itself\n");
		    fprintf (f, "!\n\n\n");
		    fprintf (f, "Groovit*revision:\t%s\n", ltover (vernumber));
		    fprintf (f, "Groovit*gpl_accepted:\t%d\n\n\n", gpl_accepted);
		    fclose (f);
		}
	    }
	    else
		fclose (f);

	    r = scan_rcfile (name, SETT_READALL, NULL);
	    strcpy (myrcfile, name);

	    if (readrevision > vernumber)
	    {
		fprintf (stderr, "your %s has revision %s ", name, ltover (readrevision));
		fprintf (stderr, "and mine is %s ...\n", ltover (vernumber));
		fprintf (stderr, "was groovit-%s so buggy ", ltover (readrevision));
		fprintf (stderr, "you had to come back to groovit-%s ?\n", ltover (vernumber));
	    }
	    if (readrevision < vernumber)
	    {
		firsttimeforthis = 1;
		gpl_accepted = 0;
	    }
	}
    }
    else
    {
	fprintf (stderr, "could not resolv uid=%d entry in /etc/passwd ?\n", u);
	if (rcfile != NULL)
	{
	    fprintf (stderr, "reading %s instead of uid=%d's rcfile\n", rcfile, u);
	    r = scan_rcfile (rcfile, SETT_READALL, NULL);
	}
    }
    return r;
}

int     initsettings (void)
{
    printf ("\033]50;%s\007", nomfont[fontindex]);	/* setfont */

    ctxsttg = createcontexts (ROWSTTG, LINESTTG, LARGSTTG, HAUTSTTG, CONTWHITE, "settings", Asettings);
    installcontexts (ctxsttg);
    radiosttg.nb = 0;
    radiosttg.value = -1;

    hsttgcancel = createsimplebutton (CONTRRADIO, ROWSTTG + 2, LINESTTG + HAUTSTTG - 2, 0, CONTGREEN, CONTUNDEF, &radiosttg, "cancel");

    hsttgapply = createsimplebutton (CONTRRADIO, ROWSTTG + 10, LINESTTG + HAUTSTTG - 2, 0, CONTGREEN, CONTUNDEF, &radiosttg, " apply");

    hsttgsave = createsimplebutton (CONTRRADIO, ROWSTTG + 18, LINESTTG + HAUTSTTG - 2, 0, CONTGREEN, CONTUNDEF, &radiosttg, "  save");

    curfontindex = fontindex;
    lastfontindex = fontindex;
    createrange (CONTRNUMRANGE, ROWSTTG + 1, LINESTTG + 2, 0, CONTUNDEF, &fontindex, 0, MAXNBFONT-1);
    hfont = createtextedit (CONTTEXTEDIT, ROWSTTG + 6, LINESTTG + 2, '@', CONTUNDEF, LARGSTTG - 10, 40, nomfont[fontindex], hsttgapply);
    clearmodif (hfont);

    curkplayahead = (int) kplayahead;

    createrange (CONTRNUMRANGE, ROWSTTG + 1, LINESTTG + 4, 0, CONTUNDEF, &curkplayahead, 1, (int) (MAXPLAYAHEAD >> 10));
    desinstallcontexts (ctxsttg);
    return (ctxsttg);
}
