/* 
 * $Id: tokenio.c,v 1.2 2003/03/19 16:45:47 jd Exp $
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
 * $Log: tokenio.c,v $
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
 * Revision 1.2.0.22.0.5  1999/09/15 09:23:30  jd
 * corrected wipping memory when read fails
 *
 * Revision 1.2.0.21.0.4  1998/12/08 02:38:27  jd
 * more indianness (only raw to disk missing)
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <endian.h>

#define FTOK_INMAIN
#include "tokenio.h"
#include "controls.h"
#include "voncendian.h"

static int nbtokfile = 0;
TokFile tokfile[FTOK_MAXFILE];

#ifdef SHOWRCS
static char *rcsid = "$Id: tokenio.c,v 1.2 2003/03/19 16:45:47 jd Exp $";

#endif
static int firstuse = 1;

int     ckfwrite (const void *ptr, size_t size, FILE * stream)
{
    while (fwrite (ptr, size, 1, stream) != 1)
    {
	switch (errno)
	  {
	  case EINTR:
	      continue;
	  case ENOSPC:
#ifdef DEBUGIOERRS
	      pdebug (errno, strerror (errno));
#endif
	      return (FTOK_NOSPACE);
	  default:
#ifdef DEBUGIOERRS
	      pdebug (errno, strerror (errno));
#endif
	      return (FTOK_ERROR);
	  }
    }
    return 0;
}

int     ckfread (void *ptr, size_t size, FILE * stream)
{
    while (fread (ptr, size, 1, stream) != 1)
    {
	switch (errno)
	  {
	  case EINTR:
	      continue;
	  case ENOSPC:
#ifdef DEBUGIOERRS
	      pdebug (errno, strerror (errno));
#endif
	      return (FTOK_NOSPACE);
	  default:
#ifdef DEBUGIOERRS
	      pdebug (errno, strerror (errno));
#endif
	      return (FTOK_ERROR);
	  }
    }
    return 0;
}

void    sweep (void *ptr, size_t size)
{
    int     i;

    for (i = 0; i < size; i++)
	*(char *) ptr++ = 0;
}

int     cktread (void *ptr, size_t size, TokFile *tf)
{
    int     i, r;

    if (tf->deep < 0)
    {
	sweep (ptr, size);
	return (FTOK_EXCEED);
    }
    for (i = 0; i < tf->deep + 1; i++)
    {
	if (tf->remain[i] < size)
	{
#ifdef DEBUGTOKEN
	    fprintf (stderr, "warning: token %4s exceeded...\n", (char *) &tf->curtok[i]);
#endif
	    sweep (ptr, size);
	    return (FTOK_EXCEED);
	}
    }

    if ((r = ckfread (ptr, size, tf->f)) != 0)
    {
	sweep (ptr, size);
	return r;
    }

    for (i = 0; i < tf->deep + 1; i++)
	tf->remain[i] -= size;
    return 0;
}

int     ckfwrite_short (FILE * stream, int v)
{
    short   sh = VONCint (v);

    return ckfwrite (&sh, sizeof (short), stream);
}

int     cktread_short (TokFile *tf, int *psh)
{
    int     r;
    short   sh;

    r = cktread (&sh, 2, tf);
    *psh = VONCint (sh);
    return r;
}

int     ckfwrite_datas (FILE * stream, const int *p, int nb)
{
    int     i = 0, n, r;

    while (i < nb)
    {
	if (*p)
	{
	    if ((r = ckfwrite_short (stream, *p++)) != 0)
		return r;
	    i++;
	}
	else
	{
	    if ((r = ckfwrite_short (stream, 0)) != 0)
		return r;
	    n = 0;
	    while ((*p == 0) && (i < nb))
		p++, n++, i++;
	    if ((r = ckfwrite_short (stream, n)) != 0)
		return r;
	}
    }
    return 0;
}

int     ckfread_datas (TokFile *tf, int *p, int nb)
{
    int     i = 0, j, l = 0, n, r;

    while (i < nb)
    {
	if ((r = cktread_short (tf, &l)) != 0)
	    return r;
	if (l != 0)
	{
	    *p++ = l;
	    i++;
	}
	else
	{
	    if ((r = cktread_short (tf, &n)) != 0)
		return r;
	    for (j = 0; (j < n) & (i < nb); j++, i++)
		*p++ = 0;
	    if ((j != n) || (i > nb))
		fprintf (stderr, "warning, malformed compression in file ????\n");
	}
    }
    return 0;
}

int     ftok_init (void)
{
    int     i;

    if (firstuse)
    {
#ifdef SHOWRCS
	fprintf (stderr, "%s\n\r", rcsid);
#endif
	firstuse = 0;
	for (i = 0; i < FTOK_MAXFILE; i++)
	    tokfile[i].f = NULL;
	return 0;
    }
    else
    {
	fprintf (stderr, "warning: ftok_init called one time excessively\n");
	return 0;		/* JDJDJDJDJD mouai... */
    }
}

TokFile *ftok_open (char *nomfile, int mode)
{
    TokFile *tf;
    int     i = 0;

    if (nbtokfile >= FTOK_MAXFILE)
    {
	fprintf (stderr, "error, max opened tokfile reached :%d\n", FTOK_MAXFILE);
	return NULL;
    }
    nbtokfile++;

    while ((i < FTOK_MAXFILE) && (tokfile[i].f != NULL))
	i++;
    if (i >= FTOK_MAXFILE)
    {
	fprintf (stderr, "error, internal mismatch for nbtofile ????\n");
	return NULL;
    }
    tf = &tokfile[i];

    switch (mode)
      {
      case FTOK_WRITE:
	  if ((tf->f = fopen (nomfile, "wb")) == NULL)
	  {
	      nbtokfile--;
	      return NULL;
	  }
	  tf->deep = -1;
	  break;
      case FTOK_READ:
	  if ((tf->f = fopen (nomfile, "rb")) == NULL)
	  {
	      nbtokfile--;
	      return NULL;
	  }
	  tf->deep = -1;
	  break;
      default:
	  fprintf (stderr, "error: unknown mode %d for ftok_open\n", mode);
	  return NULL;
      }
    return tf;
}

int     ftok_close (TokFile *tf)
{
    if (tf->deep != -1)
	fprintf (stderr, "warning, all token not closed in tokenfile...\n");
    fclose (tf->f);
    tf->f = NULL;
    nbtokfile--;
    return 0;
}

int     ftok_writetoken (TokFile *tf, long tokident, size_t size, void *data)
{
    long    stokident = chunkconv (tokident);
    long    ssize = VONClong (size), r;

    if ((r = ckfwrite (&stokident, 4, tf->f)) != 0)
	return (r);
    if ((r = ckfwrite (&ssize, 4, tf->f)) != 0)
	return (r);
    if ((r = ckfwrite (data, size, tf->f)) != 0)
	return (r);
    return 0;
}

int     ftok_startraw (TokFile *tf, long tokident)
{
    long    stokident = chunkconv (tokident);
    long    ssize = VONClong (0);	/* je le mets pour pas oublier... */
    int     r;

    if (tokident < 0)
    {
	fprintf (stderr, "error trying to use a negative valued token ident: 0x%08lx\n", tokident);
	return (FTOK_LIMIT);
    }
    if (tf->deep < FTOK_MAXDEEP - 1)
    {
	tf->deep++;
	if ((r = ckfwrite (&stokident, 4, tf->f)) != 0)
	    return (r);
#ifdef USEFGETPOS
	fgetpos (tf->f, &tf->offset[tf->deep]);
#else
	tf->offset[tf->deep] = ftell (tf->f);
#endif
	if ((r = ckfwrite (&ssize, 4, tf->f)) != 0)
	    return (r);
	tf->curtok[tf->deep] = tokident;
	return 0;
    }
    else
    {
	fprintf (stderr, "too much recursive token... rewinded and skipped\n");
	return (FTOK_LIMIT);
    }
}

long    ftok_readraw (TokFile *tf)
{
    long    tokident = 0;      /* return a negative or zero value if errors */
    int     r;

    if (tf->deep < FTOK_MAXDEEP - 1)
    {
	if (tf->deep != -1)
	{
	    if ((r = cktread (&tokident, 4, tf)) != 0)
		return r;
	    tokident = chunkconv (tokident);
	    tf->curtok[tf->deep + 1] = tokident;
	    if ((r = cktread (&tf->remain[tf->deep + 1], 4, tf)) != 0)
		return r;
	    tf->remain[tf->deep + 1] = VONClong (tf->remain[tf->deep + 1]);
	    tf->deep++;
#ifdef DEBUGDEEPTOKEN
	    fprintf (stderr, "current token size: %ld\n", tf->remain[tf->deep]);
#endif
	}
	else
	{
	    if ((r = ckfread (&tokident, 4, tf->f)) != 0)
		return r;
	    tokident = chunkconv (tokident);
	    tf->curtok[tf->deep + 1] = tokident;
	    if ((r = ckfread (&tf->remain[tf->deep + 1], 4, tf->f)) != 0)
		return r;
	    tf->remain[tf->deep + 1] = VONClong (tf->remain[tf->deep + 1]);
	    tf->deep++;
#ifdef DEBUGDEEPTOKEN
	    fprintf (stderr, "current token size: %ld\n", tf->remain[tf->deep]);
#endif
	}
	return tokident;
    }
    else
    {
	fprintf (stderr, "too much recursive token at reading ???\ngeee! where does-it come from ?\n");
	return (FTOK_LIMIT);
    }
}

int     ftok_closereadraw (TokFile *tf, long tokident, long *reste)
{
    int     i;
    long    remain = tf->remain[tf->deep];

    if (tf->deep < 0)
    {
	fprintf (stderr, "error: too much token close, skipped\n");
	return (FTOK_LIMIT);
    }
    if (tokident != tf->curtok[tf->deep])
	fprintf (stderr, "warning, token mismatch when closing token, closing anyway...\n");
    if (reste != NULL)
	*reste = remain;
#ifdef DEBUGTOKEN
    if (remain != 0)
	fprintf (stderr, "warning: token %4s closed with with %ld bytes remaining\n", (char *) &tf->curtok[tf->deep], remain);
#endif

    if (remain > 0)
    {
	fseek (tf->f, remain, SEEK_CUR);
	for (i = 0; i < tf->deep + 1; i++)
	    tf->remain[i] -= remain;
    }
    tf->deep--;
    for (i = 0; i < tf->deep + 1; i++)
	if (tf->remain[i] < 0)
	{
#ifdef DEBUGTOKEN
	    fprintf (stderr, "warning: token %4s exceeded...\n", (char *) &tf->curtok[i]);
#endif
	    return (FTOK_EXCEED);
	}
    return 0;
}

int     ftok_endraw (TokFile *tf, long tokident)
{
#ifdef USEFGETPOS
    fpos_t  cur;
#else
    long    cur;
#endif
    long     ssize, r;

    if (tf->deep < 0)
    {
	fprintf (stderr, "error: too much token close, skipped\n");
	return (FTOK_LIMIT);
    }

    if (tokident != tf->curtok[tf->deep])
	fprintf (stderr, "warning, token mismatch when closing token, closing anyway...\n");

#ifdef USEFGETPOS
    fgetpos (tf->f, &cur);
    ssize = cur - tf->offset[tf->deep] - 4;	/* JDJDJDJD here we assume that fpos_t is calculable */

    fsetpos (tf->f, &tf->offset[tf->deep]);
    if ((r = ckfwrite (&ssize, 4, tf->f)) != 0)
	return (r);
    fsetpos (tf->f, &cur);
#else
    cur = ftell (tf->f);
    ssize = cur - tf->offset[tf->deep] - 4;

    fseek (tf->f, tf->offset[tf->deep], SEEK_SET);
    if ((r = ckfwrite (&ssize, 4, tf->f)) != 0)
	return (r);
    fseek (tf->f, cur, SEEK_SET);
#endif
    tf->deep--;
    return (0);
}
