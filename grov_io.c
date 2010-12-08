/* 
 * $Id: grov_io.c,v 1.3 2003/03/19 21:56:36 jd Exp $
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
 *
 * $Log: grov_io.c,v $
 * Revision 1.3  2003/03/19 21:56:36  jd
 * small rcs to cvs typo corrected
 *
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
 * Revision 1.2.0.23.0.6  1999/10/11 15:05:55  jd
 * corrected mismatch with dy-filter pattern number
 *
 * Revision 1.2.0.23.0.2  1999/09/27 00:46:09  jd
 * added multiple pattern for dy-filter
 *
 * Revision 1.2.0.23  1999/09/15 10:20:39  jd
 * second pre tel-aviv public revision, for testing
 *
 * Revision 1.2.0.22.0.7  1999/09/15 09:23:30  jd
 * handles the reseverd mixers and primary handling of voices
 *
 * Revision 1.2.0.22.0.5  1999/09/05 22:30:16  jd
 * corrected for granalogik, grdyfilter and resoanlog modules split...
 *
 * Revision 1.2.0.22.0.4  1999/08/30 22:16:03  jd
 * inclusion corrections
 *
 * Revision 1.2.0.22.0.2  1999/08/29 16:58:26  jd
 * added readable file stating
 *
 * Revision 1.2.0.21.0.7  1998/12/08 02:38:27  jd
 * more indianness (only raw to disk missing)
 *
 * Revision 1.2.0.21.0.6  1998/12/07 22:14:37  jd
 * validated version
 *
 * Revision 1.2.0.21.0.4  1998/12/07 20:57:08  jd
 * corrected rcs Id presence handling
 *
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <endian.h>

#include "config.h"
#include "sample.h"
#include "readsample.h"
#include "tokenio.h"
#include "grov_io.h"
#include "grpattern.h"
#include "granalogik.h"
#include "grdyfilter.h"
#include "grdelay.h"
#include "controls.h"
#include "groovit.h"
#include "menu_board.h"
#include "grmixeur.h"
#include "voncendian.h"

#define MAXDIRSIZE 256		/* longueur maximale d'un path JDJDJDJDJD */

/* static char * rcsid = "$Id: grov_io.c,v 1.3 2003/03/19 21:56:36 jd Exp $"; */
static int firstuse = 1;

long    vernumber = 0;
long long runningk = 0, riddenk = 0, generatedk = 0;

char   *relativename (char *ref, char *name)
{
    char    curdir[MAXDIRSIZE], dirname2[MAXDIRSIZE], *p;
    static char dirname1[MAXDIRSIZE];
    int     l;

    if (name[0] == '/')
	strcpy (dirname2, name);
    else
    {
	strcpy (dirname2, ref);
	strcat (dirname2, "/");
	strcat (dirname2, name);
    }

    p = strrchr (dirname2, '/');
    if (p == NULL)
	return NULL;
    *p = 0;

    if (getcwd (curdir, MAXDIRSIZE) == NULL)
    {
	fprintf (stderr, "current dir is very big pathname ? aborting\n");
	return NULL;
    }
    chdir (dirname2);
    if (getcwd (dirname1, MAXDIRSIZE) == NULL)
    {
	fprintf (stderr, "%s very big pathname ? aborting\n", name);
	chdir (curdir);
	return NULL;
    }
    strcat (dirname1, "/");
    strcat (dirname1, p + 1);

    strcpy (dirname2, ref);
    chdir (dirname2);
    if (getcwd (dirname2, MAXDIRSIZE) == NULL)
    {
	fprintf (stderr, "%s very big pathname ? aborting\n", ref);
	chdir (curdir);
	return NULL;
    }
    l = strlen (dirname2);

    chdir (curdir);

    if (strncmp (dirname1, dirname2, l) == 0)
	return (dirname1 + l + 1);
    else
	return (dirname1);
}

int     isreallyafile (char *name)
{
    FILE   *f;

    if ((f = fopen (name, "rb")) == NULL)
    {
	fprintf (stderr, "%s cannot be read: %s\n", name, strerror (errno));
	return -1;
    }
    fclose (f);
    return 0;
}

int     save_cur_song (char *name, int mode)
{
    TokFile *f;
    long    dummyv;

    if ((f = ftok_open (name, FTOK_WRITE)) == NULL)	/* JDJDJDJD gerer les erreurs ici */
    {
	fprintf (stderr, "error: unable to open %s, aborting\n", name);
    }

    ftok_startraw (f, CHUNKgrov);
    ftok_startraw (f, CHUNKable);
    dummyv = VONClong (vernumber);
    ckfwrite (&dummyv, sizeof (long), f->f);	/* 4 bytes arbitrary */

    ftok_endraw (f, CHUNKable);

    {
	ftok_startraw (f, CHUNKgprm);	/* General PaRaMeters */
	ckfwrite_short (f->f, bpm);
	ckfwrite_short (f->f, pattpatbut.ncurpattern);

	ftok_endraw (f, CHUNKgprm);
    }

    {
	ftok_startraw (f, CHUNKdelp);	/* DELay Parameters */
	ckfwrite_short (f->f, decay);
	ckfwrite_short (f->f, delaytick);
	ftok_endraw (f, CHUNKdelp);
    }

    ftok_startraw (f, CHUNKsdst);	/* Sample DiSTribution */
    {
	short   sh = 0;
	int     i;
	int     voice;
	char   *p;

	ckfwrite_short (f->f, MAXSAMPLE);

	voice = 0;		/* JDJDJDJD a parfaire */

	for (i = 0; i < MAXSAMPLE; i++, voice++)
	    if (sample[i].data != NULL)
	    {
		ckfwrite_short (f->f, i);
		p = relativename (SAMPLEPATH, sample[i].name);
		sh = strlen (p);
		sh += (sh & 1) ? 1 : 0;
		ckfwrite_short (f->f, sh);
		ckfwrite (p, sh, f->f);		/* no endianness needed here !! */
/* ckfwrite_short (f->f, sample[i].level); ckfwrite_short (f->f, sample[i].pan);
   ckfwrite_short (f->f, sample[i].revlevel); ckfwrite_short (f->f, sample[i].filtl);
        */
		ckfwrite_short (f->f, vlevel[voice]);
		ckfwrite_short (f->f, vpan[voice]);
		ckfwrite_short (f->f, mixeur[0].level[voice]);	/* JDJDJDJD a parfaire */
		ckfwrite_short (f->f, mixeur[1].level[voice]);

	    }
    }
    ftok_endraw (f, CHUNKsdst);

  /* Rythms (samples) PaTerN */
    {
	int     i, j;

	for (i = 0; i < MAXPATTERN; i++)
	    if (!pattern[i].isnew)
	    {
		ftok_startraw (f, CHUNKrptn);
		ckfwrite_short (f->f, MAXSAMPLE);
		ckfwrite_short (f->f, MAXPATTERNROW);
		ckfwrite_short (f->f, i);
		ckfwrite_short (f->f, pattern[i].nbrow);
		for (j = 0; j < MAXSAMPLE; j++)
		    ckfwrite_datas (f->f, pattern[i].start[j], MAXPATTERNROW);

		ftok_endraw (f, CHUNKrptn);
	    }
    }

  /* Analog PaTerN */
    {
	int     i;

	for (i = 0; i < MAXPATTERN; i++)
	    if (!apattern[i].isnew)
	    {
		ftok_startraw (f, CHUNKaptn);
		ckfwrite_short (f->f, MAXPATTERNROW);
		ckfwrite_short (f->f, i);
		ckfwrite_short (f->f, apattern[i].nbrow);
		ckfwrite_datas (f->f, apattern[i].freqbase, MAXPATTERNROW);
		ftok_endraw (f, CHUNKaptn);
	    }
    }

  /* Analog PaRaMeters */
    {
	int     i;
	int     voice;

	voice = MAXSAMPLE;	/* JDJDJDJD a parfaire */

	for (i = 0; i < MAXANALOG; i++, voice++)
	{
	    ftok_startraw (f, CHUNKaprm);
	    ckfwrite_short (f->f, i);
	    ckfwrite_short (f->f, anvoice[i].patbut.ncurpattern);
	    ckfwrite_short (f->f, anvoice[i].reso.ctfreqcut);
	    ckfwrite_short (f->f, anvoice[i].reso.ctfreqcut2);
	    ckfwrite_short (f->f, anvoice[i].reso.ctreso);
	    ckfwrite_short (f->f, anvoice[i].reso.ctdecay);
/* ckfwrite_short (f->f, analog[i].level); ckfwrite_short (f->f, analog[i].pan);
   ckfwrite_short (f->f, analog[i].revlevel);   */
	    ckfwrite_short (f->f, vlevel[voice]);	/* JDJDJDJD a parfaire */
	    ckfwrite_short (f->f, vpan[voice]);
	    ckfwrite_short (f->f, mixeur[0].level[voice]);
	    ckfwrite_short (f->f, mixeur[1].level[voice]);
	    ftok_endraw (f, CHUNKaprm);
	}
    }

  /* Filter PaTerN */
    {
	int     i;

	for (i = 0; i < MAXPATTERN; i++)
	    if (!fpattern[i].isnew)
	    {
		ftok_startraw (f, CHUNKfptn);
		ckfwrite_short (f->f, MAXPATTERNROW);
		ckfwrite_short (f->f, i);	/* idem JDJDJDJD */
		ckfwrite_short (f->f, fpattern[i].nbrow);
		ckfwrite_datas (f->f, fpattern[i].freqbase, MAXPATTERNROW);
		ftok_endraw (f, CHUNKfptn);
	    }
    }

  /* Filter PaRaMeters */
    {
	int     i;
	int     voice;

	voice = MAXSAMPLE + MAXANALOG;	/* JDJDJDJD a parfaire */

	for (i = 0; i < MAXDYFILT; i++, voice++)
	{
	    ftok_startraw (f, CHUNKfprm);
	    ckfwrite_short (f->f, i);
	    ckfwrite_short (f->f, dyfilter[i].patbut.ncurpattern);
	    ckfwrite_short (f->f, dyfilter[i].reso.ctfreqcut);
	    ckfwrite_short (f->f, dyfilter[i].reso.ctfreqcut2);
	    ckfwrite_short (f->f, dyfilter[i].reso.ctreso);
	    ckfwrite_short (f->f, dyfilter[i].reso.ctdecay);
/* ckfwrite_short (f->f, filter.level); ckfwrite_short (f->f, filter.pan); ckfwrite_short
   (f->f, filter.revlevel);     */
	    ckfwrite_short (f->f, vlevel[voice]);	/* JDJDJDJD a parfaire */
	    ckfwrite_short (f->f, vpan[voice]);
	    ckfwrite_short (f->f, mixeur[0].level[voice]);
	    ckfwrite_short (f->f, mixeur[1].level[voice]);
	    ftok_endraw (f, CHUNKfprm);
	}
    }

    ftok_startraw (f, 0x6d746167);
    {
	long    k = VONClong (runningk);
	ckfwrite (&k, sizeof (long), f->f);
    }
    ftok_endraw (f, 0x6d746167);
    ftok_endraw (f, CHUNKgrov);

    ftok_close (f);
    return (0);
}

int     load_cur_song (char *name, int mode)
{
    TokFile *f;
    long    generaltok, curtoken, filevernumber = -1,	/* no de version du fichier */
            error = 0, reste;
    int     filenbsample = -1, filemaxpatternrow = -1, filemaxpattern = -1, sampletoread,
            i, nosdst = 1, warnremain = 1;

    if ((f = ftok_open (name, FTOK_READ)) == NULL)
    {
	fprintf (stderr, "error: unable to open %s, aborting\n", name);
	return -1;
    }

    generaltok = ftok_readraw (f);
    if (generaltok != CHUNKgrov)
    {
	if (generaltok < 0)
	{
	    fprintf (stderr, "error reading %s, could not read first token, aborting\n", name);
	    ftok_close (f);
	    return -1;
	}
	fprintf (stderr, "error reading %s, first token 0x%08lx inappropriate, aborting\n", name, generaltok);
	ftok_close (f);
	return -1;
    }

    while ((error == 0) && (curtoken = ftok_readraw (f)) > 0)
    {
	char    tokname[6];

	*(long *) tokname = curtoken;
	tokname[4] = 0;
#ifdef DEBUGGROVIO
	fprintf (stderr, "entering token [%s]\n", tokname);
#endif
	warnremain = 1;

	switch (curtoken)
	{
	    case CHUNKable:
		if (cktread (&filevernumber, 4, f))
		    filevernumber = -1;
		filevernumber = VONClong (filevernumber);
		if (filevernumber != vernumber)
		{
		    char   *p = (char *) &filevernumber;

		    if (filevernumber > vernumber)
			fprintf (stderr, "warning: %s has been writen by a newer version groovit-%d.%d.%d.%d\n", name, p[3], p[2], p[1], p[0]);
		    else
			fprintf (stderr, "warning: %s has been writen by an older version groovit-%d.%d.%d.%d\n", name, p[3], p[2], p[1], p[0]);
		}
		error += cktread_short (f, &filemaxpattern);
		if (error == FTOK_EXCEED)
		{
		    error = 0;
		    filemaxpattern = MAXPATTERN;
		    break;
		}
		if (error)
		    break;
		break;

	    case CHUNKgprm:	/* General PaRaMeters */
		error += cktread_short (f, &bpm);
		error += cktread_short (f, &pattpatbut.ncurpattern);
		break;

	    case CHUNKdelp:	/* DELay Parameters */
		error += cktread_short (f, &decay);
		error += cktread_short (f, &delaytick);
		break;

	    case CHUNKrptn:	/* Rythms (samples) PaTerN */
		error += cktread_short (f, &filenbsample);
		if (filenbsample > MAXSAMPLE)
		{
		    fprintf (stderr, "warning: file %s uses %d samples and I handle only %d, truncate !\n", name, filenbsample, MAXSAMPLE);
		    sampletoread = MAXSAMPLE;
		}
		else
		    sampletoread = filenbsample;

		error += cktread_short (f, &filemaxpatternrow);
		if (filemaxpatternrow > MAXPATTERNROW)
		{
		    fprintf (stderr, "warning: file %s uses %d rows in patterns and I handle only %d, truncate !\n", name, filemaxpatternrow, MAXPATTERNROW);
		}
		if (filemaxpatternrow > 10 * MAXPATTERNROW)
		{
		    fprintf (stderr, "error: reading %s I cannot read %d raws in a pattern yet, sorry an aborting! \n", name, filemaxpatternrow);
		    break;
		}
		{
		    int     numo[10 * MAXPATTERNROW], no, filenbrow;

		    for (i = 0; i < MAXPATTERNROW; i++)
			numo[i] = 0;

		    error += cktread_short (f, &no);
		    if (error)
			break;
		    if ((no < 0) || (no > filemaxpattern))
		    {
			fprintf (stderr, "error reading pattern in %s: %d is an invalid patnumber, skipped\n", name, no);
			break;
		    }
		    if (no >= MAXPATTERN)
		    {
			fprintf (stderr, "warning reading pattern in %s: dont have enough pattern to read one numbered %d ! skipped\n", name, no);
			break;
		    }
		    error += cktread_short (f, &filenbrow);
		    if (error)
			break;
		    if ((filenbrow > filemaxpatternrow) || (filenbrow < 0))
		    {
			fprintf (stderr, "warning reading %s: pattern %d has %d rows which is more than the declared max (%d) of that file ?\nignored\n", name, no, filenbrow, filemaxpatternrow);
			filenbrow = (filenbrow > MAXPATTERNROW) ? MAXPATTERNROW : filenbrow;
			filenbrow = (filenbrow < 0) ? MAXPATTERNROW : filenbrow;
		    }
		    for (i = 0; i < sampletoread; i++)
		    {
			error += ckfread_datas (f, numo, filemaxpatternrow);
			if (error)
			    break;
			if (i < MAXSAMPLE)
			{
			    memcpy (pattern[no].start[i], numo, MAXPATTERNROW * sizeof (int));

			    pattern[no].isnew = 0;
			    pattern[no].nbrow = filenbrow > MAXPATTERNROW ? MAXPATTERNROW : filenbrow;
			}
		    }
		    for (i = 0; i < MAXPATTERNROW; i++)
			numo[i] = 0;
		    for (i = sampletoread; i < MAXSAMPLE; i++)	/* clearing voices remaining */
			memcpy (pattern[no].start[i], numo, MAXPATTERNROW * sizeof (int));
		}
		break;

	    case CHUNKaptn:	/* Analog PaTerN */
		error += cktread_short (f, &filemaxpatternrow);
		if (error)
		    break;
		if (filemaxpatternrow > MAXPATTERNROW)
		{
		    fprintf (stderr, "warning: file %s uses %d rows in patterns and I handle only %d, truncate !\n", name, filemaxpatternrow, MAXPATTERNROW);
		}
		if (filemaxpatternrow > 10 * MAXPATTERNROW)
		{
		    fprintf (stderr, "error: reading %s I cannot read %d raws in a pattern yet, sorry an aborting! \n", name, filemaxpatternrow);
		    break;
		}
		{
		    int     numo[10 * MAXPATTERNROW], no, filenbrow;

		    for (i = 0; i < MAXPATTERNROW; i++)
			numo[i] = 0;

		    error += cktread_short (f, &no);
		    if (error)
			break;
		    if ((no < 0) || (no > filemaxpattern))
		    {
			fprintf (stderr, "error reading pattern in %s: %d is an invalid patnumber, skipped\n", name, no);
			break;
		    }
		    if (no >= MAXPATTERN)
		    {
			fprintf (stderr, "warning reading pattern in %s: dont have enough pattern to read one numbered %d ! skipped\n", name, no);
			break;
		    }
		    error += cktread_short (f, &filenbrow);
		    if (error)
			break;
		    if ((filenbrow > filemaxpatternrow) || (filenbrow < 0))
		    {
			fprintf (stderr, "warning reading %s: pattern %d has %d rows which is more than the declared max (%d) of that file ?\nignored\n", name, no, filenbrow, filemaxpatternrow);
			filenbrow = (filenbrow > MAXPATTERNROW) ? MAXPATTERNROW : filenbrow;
			filenbrow = (filenbrow < 0) ? MAXPATTERNROW : filenbrow;
		    }
		    error += ckfread_datas (f, numo, filemaxpatternrow);
		    if (error)
			break;
		    memcpy (apattern[no].freqbase, numo, MAXPATTERNROW * sizeof (int));

		    apattern[no].isnew = 0;
		    apattern[no].nbrow = filenbrow > MAXPATTERNROW ? MAXPATTERNROW : filenbrow;
		}
		break;

	    case CHUNKfptn:	/* Filter PaTerN */
		error += cktread_short (f, &filemaxpatternrow);
		if (error)
		    break;
		if (filemaxpatternrow > MAXPATTERNROW)
		{
		    fprintf (stderr, "warning: file %s uses %d rows in patterns and I handle only %d, truncate !\n", name, filemaxpatternrow, MAXPATTERNROW);
		}
		if (filemaxpatternrow > 10 * MAXPATTERNROW)
		{
		    fprintf (stderr, "error: reading %s I cannot read %d raws in a pattern yet, sorry an aborting! \n", name, filemaxpatternrow);
		    break;
		}
		{
		    int     numo[10 * MAXPATTERNROW], no, filenbrow;

		    for (i = 0; i < MAXPATTERNROW; i++)
			numo[i] = 0;

		    error += cktread_short (f, &no);
		    if (error)
			break;
		    if (no < 0)
		    {
			fprintf (stderr, "error reading pattern in %s: %d is an invalid patnumber, skipped\n", name, no);
			break;
		    }
		    if (no >= MAXPATTERN)
		    {
			fprintf (stderr, "warning reading filter pattern in %s: dont have enough filter pattern to read one numbered %d ! skipped\n", name, no);
			break;
		    }
		    error += cktread_short (f, &filenbrow);
		    if (error)
			break;
		    if ((filenbrow > filemaxpatternrow) || (filenbrow < 0))
		    {
			fprintf (stderr, "warning reading %s: pattern %d has %d rows which is more than the declared max (%d) of that file ?\nignored\n", name, no, filenbrow, filemaxpatternrow);
			filenbrow = (filenbrow > MAXPATTERNROW) ? MAXPATTERNROW : filenbrow;
			filenbrow = (filenbrow < 0) ? MAXPATTERNROW : filenbrow;
		    }
		    error += ckfread_datas (f, numo, filemaxpatternrow);
		    if (error)
			break;
		    memcpy (fpattern[no].freqbase, numo, MAXPATTERNROW * sizeof (int));

		    fpattern[no].isnew = 0;
		    fpattern[no].nbrow = filenbrow > MAXPATTERNROW ? MAXPATTERNROW : filenbrow;
		}
		break;

	    case CHUNKfprm:	/* Filter PaRaMeters */
		{
		    int     no;
		    int     voice;

		    error += cktread_short (f, &no);
		    if (error)
			break;
		    if (no < 0)
		    {
			fprintf (stderr, "error reading %s: invalid filter voice number %d, skipped\n", name, no);
			break;
		    }
		    if (no >= MAXDYFILT)
		    {
			fprintf (stderr, "warning reading %s: cannot handle filter voices numbered %d, will steer only %d...\n", name, no, MAXANALOG);
			break;
		    }
		    voice = MAXSAMPLE + MAXANALOG + no;		/* JDJDJDJD a parfaire */
#ifdef DEB_VOICEAFF
		    fprintf (stderr, "filter no:%d voice:%d\n", no, voice);
#endif

		    error += cktread_short (f, &dyfilter[no].patbut.ncurpattern);	
		    error += cktread_short (f, &dyfilter[no].reso.ctfreqcut);
		    error += cktread_short (f, &dyfilter[no].reso.ctfreqcut2);
		    error += cktread_short (f, &dyfilter[no].reso.ctreso);
		    error += cktread_short (f, &dyfilter[no].reso.ctdecay);
/* error += cktread_short (f, &filter.level); error += cktread_short (f, &filter.pan); error
   += cktread_short (f, &filter.revlevel);      */
		    error += cktread_short (f, &vlevel[voice]);		/* JDJDJDJD a
									   parfaire */
		    error += cktread_short (f, &vpan[voice]);
		    error += cktread_short (f, &mixeur[0].level[voice]);
		    error += cktread_short (f, &mixeur[1].level[voice]);
		    if (error == FTOK_EXCEED)
			error = 0;
		}
		break;

	    case 0x6D746167:
		{
		    long    dummy = -1, last = -1, k = runningk;
		    while ((error = cktread (&dummy, sizeof (long), f)) == 0)
			        last = dummy;

		    if (error != FTOK_EXCEED)
			break;
		    error = 0;
		    last = VONClong (last);
		    if (last != k)
			fprintf (stderr, "%ld != %ld\n", last, k);
		}
		break;

	    case CHUNKaprm:	/* Analog PaRaMeters */
		{
		    int     no;
		    int     voice;

		    error += cktread_short (f, &no);
		    if (error)
			break;
		    if (no < 0)
			break;
		    if (no >= MAXANALOG)
		    {
			fprintf (stderr, "warning reading %s: cannot handle analog voices numbered %d, will steer only %d...\n", name, no, MAXANALOG);
			break;
		    }
		    voice = MAXSAMPLE + no;	/* JDJDJDJDJD a parfaire */
#ifdef DEB_VOICEAFF
		    fprintf (stderr, "analog no:%d voice:%d\n", no, voice);
#endif

		    error += cktread_short (f, &anvoice[no].patbut.ncurpattern);
		    error += cktread_short (f, &anvoice[no].reso.ctfreqcut);
		    error += cktread_short (f, &anvoice[no].reso.ctfreqcut2);
		    error += cktread_short (f, &anvoice[no].reso.ctreso);
		    error += cktread_short (f, &anvoice[no].reso.ctdecay);
/* error += cktread_short (f, &analog[no].level); error += cktread_short (f,
   &analog[no].pan); error += cktread_short (f, &analog[no].revlevel);  */
		    error += cktread_short (f, &vlevel[voice]);		/* JDJDJDJD a
									   parfaire */
		    error += cktread_short (f, &vpan[voice]);
		    error += cktread_short (f, &mixeur[0].level[voice]);
		    error += cktread_short (f, &mixeur[1].level[voice]);
		    if (error == FTOK_EXCEED)
			error = 0;
		}
		break;

	    case CHUNKsdst:	/* Sample DiSTribution */
		if (nosdst == 0)
		    fprintf (stderr, "warning: reading %s a second sample dist chunk !!!???\n", name);
		nosdst = 0;
		error += cktread_short (f, &filenbsample);
		if (error)
		    break;
		if (filenbsample < 0)
		{
		    filenbsample = -1;
		    break;
		}
		if (filenbsample > MAXSAMPLE)
		{
		    fprintf (stderr, "warning: file %s uses %d samples and I handle only %d, truncate !\n", name, filenbsample, MAXSAMPLE);
		    sampletoread = MAXSAMPLE;
		}
		else
		    sampletoread = filenbsample;

		for (i = 0; i < filenbsample; i++)
		{
		    int     no, lname;
		    int     voice;
		    char    samplename[MAXDIRSIZE];
		    int     level, pan, revlevel, filtl;

		    error += cktread_short (f, &no);
		    if (error == FTOK_EXCEED)
		    {
			error = 0;
			break;
		    }
		    error += cktread_short (f, &lname);
		    if (error)
			break;
		    if ((lname > MAXDIRSIZE - 2) || (lname < 0))
		    {
			fprintf (stderr, "error: reading %s, mispelled sample entry len=%d\n", name, lname);
			break;
		    }
		    error += cktread (samplename, lname, f);	/* no endianess needed here
								   neither */
		    samplename[lname] = 0;
		    if (error)
			break;
		    error += cktread_short (f, &level);
		    error += cktread_short (f, &pan);
		    error += cktread_short (f, &revlevel);
		    error += cktread_short (f, &filtl);

		    if (no < sampletoread)
		    {
#ifdef VERYNASTYDEBUG
			fprintf (stderr, "attributing levels to sample[%d]\n", no);
#endif
/* sample[no].level = level; sample[no].pan = pan; sample[no].revlevel = revlevel;
   sample[no].filtl = filtl;    */
			voice = no;	/* JDJDJDJD a parfaire */
			vlevel[voice] = level;
			vpan[voice] = pan;
			mixeur[0].level[voice] = revlevel;
			mixeur[1].level[voice] = filtl;		/* autant dire qu'on n'est
								   pas dans la merde ! */

			{
			    char    buf[MAXDIRSIZE];
			    Sample *newsample = &sample[no];

			    cursamp[no] = 0;
			    freesample (newsample);
			    if (samplename[0] != '/')
			    {
				strcpy (buf, SAMPLEPATH);
				strcat (buf, "/");
				strcat (buf, samplename);
			    }
			    else
				strcpy (buf, samplename);

#ifdef VERYNASTYDEBUG
			    fprintf (stderr, "attributing %s to sample[%d]\n", buf, no);
#endif
			    if (readsample (buf, newsample) == NULL)
				freesample (newsample);
			    relooksimplebutton (hbt_sample[no], '@', CONTBLUE + CONTNOPUCE, newsample->shortname);
			}
		    }
		}
		break;

	    default:
		fprintf (stderr, "warning: unknown token %s in %s\n", tokname, name);
		warnremain = 0;
	}

	ftok_closereadraw (f, curtoken, &reste);
#ifdef DEBUGGROVIO
	fprintf (stderr, "      gone out [%s] size: %ld\n", tokname, reste);
#endif
    }

    ftok_closereadraw (f, generaltok, &reste);
    if (reste != 0)
	fprintf (stderr, "warning reading %s, %ld bytes miscounted at end of file, skipped\n", name, reste);
    ftok_close (f);
    return 0;
}

int     init_grov_io (char *groovit_rcsid)
{
    char   *p, *q, buf[200];   /* JDJDJDJD this is arbitrary */

    if (firstuse)
    {
/* fprintf (stderr, "%s\n\r", rcsid);   */
	firstuse = 0;
    }

    vernumber = 0;
    while (1)
    {
	strncpy (buf, groovit_rcsid, 200);

	p = strchr (buf, ' ');
	if (*buf == '$')
	    p = (p == NULL) ? p : strchr (p + 1, ' ');
	if (p == NULL)
	    break;
	p++;

	q = strchr (p, '.');
	if (q == NULL)
	    break;
	*q = 0;
	vernumber += atoi (p);
	p = q + 1;

	q = strchr (p, '.');
	if (q == NULL) {
	    vernumber <<= 8;
	} else {
	    *q = 0;
	    vernumber <<= 8;
	    vernumber += atoi (p);
	    p = q + 1;
	}

	q = strchr (p, '.');
	if (q == NULL) {
	    vernumber <<= 8;
	} else {
	    *q = 0;
	    vernumber <<= 8;
	    vernumber += atoi (p);
	    p = q + 1;
	}

	q = strchr (p, ' ');
	if (q == NULL)
	    q = strchr (p, '.');
	if (q == NULL) {
	    vernumber <<= 8;
	} else {
	    *q = 0;
	    vernumber <<= 8;
	    vernumber += atoi (p);
	}
/* fprintf (stderr, "vernumber=0x%08lx\n", vernumber);  */

	// is this completely buggy JDJDJD 20100421 ????
//	if (0)
//	{
//	    struct utsname unam;
//	    long    l = 0, *p = (long *) &unam;
//	    int     i = 0;
//	    for (i = 0; i < ((sizeof (struct utsname)) >> 2); i++)
//
//		*p++ = 0;
//	    uname (&unam);
//	    p = (long *) &unam;
//	    for (i = 0; i < ((sizeof (unam)) >> 2); i++)
//		l += *p++;
//	    runningk = (l % 3171) ? l : l + 1;
//	}
	return 0;
    }
    fprintf (stderr, ">%s<\n", p);
    fprintf (stderr, "error, could not determine version number.\n");
    return -1;
}
