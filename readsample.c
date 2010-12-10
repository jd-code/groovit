/* 
 * $Id: readsample.c,v 1.2 2003/03/19 16:45:47 jd Exp $
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
 * $Log: readsample.c,v $
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
 * Revision 1.2.0.23.0.4  1999/10/11 15:05:55  jd
 * new (nicer) warning and error messages
 *
 * Revision 1.2.0.23.0.3  1999/10/07 14:08:05  jd
 * Revision 1.2.0.23  1999/09/15 10:20:39  jd
 * second pre tel-aviv public revision, for testing
 *
 * Revision 1.2.0.21.0.4  1998/12/08 02:38:27  jd
 * more indianness (only raw to disk missing)
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/* #include <fcntl.h> */
/* #include <math.h> */
#include <string.h>
#include <sys/stat.h>
#include <endian.h>

#include "conv_ieee_ext.h"
#include "config.h"
#include "sample.h"

typedef int32_t Sint32;

/* format AIFF ............................................................ */
#define AIFFBUFSIZE 1024	/* taille du buffer intermediaire pour lecture AIFF */
#define CHUNKFORM 0x4d524f46
#define CHUNKAIFF 0x46464941
#define CHUNKNAME 0x454d414e	/* NAME */
#define CHUNKAUTH 0x48545541	/* AUTHor */
#define CHUNKRIGH 0x20296328	/* "(c) " */
#define CHUNKANNO 0x4f4e4e41	/* ANNOtation */
#define CHUNKCOMT 0x544d4f43	/* COMmenT */
#define CHUNKAPPL 0x4c505041	/* APPLication specifik chunk */
#define CHUNKAESD 0x44534541	/* AESD Audio rEcording SounD chunk */
#define CHUNKMIDI 0x4944494d	/* MIDI chunk */
#define CHUNKINST 0x5453494e	/* INSTrument chunk */
#define CHUNKMARK 0x4b52414d	/* INSTrument chunk */
#define CHUNKCOMM 0x4d4d4f43	/* COMMon chunk */
#define CHUNKSSND 0x444e5353	/* Sampled SouND chunk */

/* format WAV ............................................................. */
#define CHUNKRIFF    0x46464952	/* RIFF container */
#define CHUNKWAVE    0x45564157
#define CHUNKwavFMT  0x20746d66	/* ForMaT definitions */
#define CHUNKwavdata 0x61746164	/* DATA chunk */
#define CHUNKwavLIST 0x5453494c	/* wave-LIST chunk -not handled */
#define CHUNKwavslnt 0x746e6c73	/* silent chunk -not handled */
#define CHUNKwavfact 0x74636166	/* FACT chunk -not handled */
#define CHUNKwavcue  0x20657563	/* CUE-points postions chunk -not handled */
#define CHUNKwavplst 0x74736c70	/* Playing LiST chunk -not handled */

#ifdef SHOWRCS
static char *rcsid = "$Id: readsample.c,v 1.2 2003/03/19 16:45:47 jd Exp $";

#endif
static int firstuse = 1;

#if (BYTE_ORDER != LITTLE_ENDIAN)
#if (BYTE_ORDER != BIG_ENDIAN)
#error your endianess seems not to be known from me
#endif
#endif
#ifndef BYTE_ORDER
#error endianness unknown
#endif
#ifndef LITTLE_ENDIAN
#error endianness unknown
#endif

#ifdef STUPIDEJD
#if (BYTE_ORDER == LITTLE_ENDIAN)
inline Sint32 chunkconv (Sint32 chunk)	/* read chunk mit endianness */
{
    return chunk;
}
#else
inline Sint32 chunkconv (Sint32 chunk)
{
    return (
	       ((l & 0xFF000000) >> 24) +
	       ((l & 0x00FF0000) >> 8) +
	       ((l & 0x0000FF00) << 8) +
	       ((l & 0x000000FF) << 24)
	);
}
#endif
#endif

#if (BYTE_ORDER == LITTLE_ENDIAN)
inline Sint32 CONVlong (Sint32 l)  /* converts 4BYTES read in BIGendian */
{
    return (
	       ((l & 0xFF000000) >> 24) +
	       ((l & 0x00FF0000) >> 8) +
	       ((l & 0x0000FF00) << 8) +
	       ((l & 0x000000FF) << 24)
	);
}
#else
inline Sint32 CONVlong (Sint32 l)
{
    return l;
}
#endif

#if (BYTE_ORDER == LITTLE_ENDIAN)
inline int CONVint (int i)     /* converts 2BYTES read in BIGendian */
{
    return (
	       ((i & 0xFF00) >> 8) +
	       ((i & 0x00FF) << 8)
	);
}
#else
inline int CONVint (int i)
{
    return i;
}
#endif

#ifdef STUPIDEJD
#if (BYTE_ORDER == LITTLE_ENDIAN)
inline Sint32 VONClong (Sint32 l)
{
    return l;
}
#else
inline Sint32 VONClong (Sint32 l)  /* converts 4BYTES read in LITTLEendian */
{
    return (
	       ((l & 0xFF000000) >> 24) +
	       ((l & 0x00FF0000) >> 8) +
	       ((l & 0x0000FF00) << 8) +
	       ((l & 0x000000FF) << 24)
	);
}
#endif

#if (BYTE_ORDER == LITTLE_ENDIAN)
inline int VONCint (int i)
{
    return i;
}
#else
inline int VONCint (int i)     /* converts 2BYTES read in LITTLEendian */
{
    return (
	       ((i & 0xFF00) >> 8) +
	       ((i & 0x00FF) << 8)
	);
}
#endif
#else
#include "voncendian.h"
#endif

/* USELESS because we need to read a 80bit (!) ieee 754 float */
char   *CONVdouble (char *p)
{
    char    s, *q = p, *r = p + 7;
    int     i;

    for (i = 0; i < 4; i++)
    {
	s = *p;
	*q++ = *r;
	*r-- = s;
    }
    return p;
}

int     initsample (Sample *sample)
{
    sample->data = NULL;
    sample->size = 0;
    sample->nbch = 0;
    sprintf (sample->shortname, STRINGSHORTNAME, "--empty--");
    sample->name = NULL;
/* sample->level = 192; sample->pan = 127; sample->revlevel = 0; */
    return 0;
}

int     freesample (Sample *sample)
{
    sample->size = 0;
    if (sample->data != NULL)
	free (sample->data);
    sample->data = NULL;	/* later we may avoid this JDJDJDJD ? */
    if (sample->name != NULL)
	free (sample->name);
    sample->name = NULL;	/* later we may avoid this JDJDJDJD ? */
    sample->nbch = 0;
    return 0;
}

short  *readsample_wav (char *nomfile, Sample *sample)
{
    FILE   *f;
    struct stat Sfstat;
    unsigned char buf[AIFFBUFSIZE],
	    *bbuf;
    int     skipped = 0;
    long    taille = 0;
    short  *ps = NULL;
    long    nbsample = 0, n;
    int     realsamplesize = 0, samplesize = 0, nbchannel = 0, framesize = 0;
    long    rate = 0;
    int	    nbreadch = 0;
    int	    ch;

    if ((sample->data != NULL) || (sample->size != 0))
    {
	fprintf (stderr, "I dont overwrite samples, it must be freed before !\n\r");
	return sample->data;
    }
  /* par securite je le fais qd meme .... */
    initsample (sample);

    stat (nomfile, &Sfstat);

    if ((f = fopen (nomfile, "rb")) == NULL)
    {
	fprintf (stderr, "impossible d'ouvrir %s\n\r", nomfile);
	return NULL;
    }
    fread (buf, 1, 12, f);	/* lecture RIFF container chunck */
    if ((chunkconv (*(Sint32 *) buf) != CHUNKRIFF) || (chunkconv (*(Sint32 *) (buf + 8)) != CHUNKWAVE))
    {
	fprintf (stderr, "%s isnt wave compliant: first chunk not RIFFxxxxWAVE\n\r", nomfile);
	return NULL;
    }
    if (VONClong ((*(Sint32 *) (buf + 4))) + 8 != Sfstat.st_size)
	fprintf (stderr, "%s hasnt expected size: %ld instead of %ld\n\r", nomfile, Sfstat.st_size, VONClong ((*(Sint32 *) (buf + 4)) + 8));
    if ((sample->name = (char *) malloc (1 + strlen (nomfile))) == NULL)
    {
	fprintf (stderr, "%s: impossible d'allouer %ld bytes ??? \n\r", nomfile, strlen (nomfile));
	initsample (sample);
	return (NULL);
    }
    strcpy (sample->name, nomfile);

  /* on genere le nom court a partir du nom de fichier */
    {
	char   *p;

	p = strrchr (nomfile, '/');
	strcpy ((char *)buf, (p == NULL) ? nomfile : p + 1);
	p = strrchr ((char *)buf, '.');
	if (p != NULL)
	    *p = 0;
	buf[SHORTNAMESIZE] = 0;
	sprintf (sample->shortname, STRINGSHORTNAME, buf);
    }

    while (!feof (f))
    {
	if (fread (buf, 1, 8, f) != 8)
	    break;
	skipped = 0;
	taille = VONClong ((*(Sint32 *) (buf + 4)));
	switch (chunkconv (*(Sint32 *) buf))
	  {

	  case CHUNKwavFMT:
	      if (taille > AIFFBUFSIZE)
	      {
		  fprintf (stderr, "%s: FMT chunk too big, skipping %ld byte\n\r", nomfile, taille);
		  skipped = 1;
		  break;
	      }
	      fread (buf, 1, taille, f);
	      if (VONCint (*(short *) buf) != 1)
		  fprintf (stderr, "%s data type others than PCM not handled\n", nomfile);
	      nbchannel = VONCint ((*(short *) (buf + 2)));
	      if ((nbchannel < 1) || (nbchannel > 2))
		  fprintf (stderr, "%s: warning: %d data-channels, only the two firsts will be used\n\r", nomfile, nbchannel);
	      nbreadch = nbchannel > 2 ? 2 : nbchannel;
	      sample->nbch = nbreadch;
	      rate = VONClong ((*(Sint32 *) (buf + 4)));
	      realsamplesize = VONCint ((*(short *) (buf + 14)));
	      framesize = VONCint ((*(short *) (buf + 12)));

	      samplesize = realsamplesize;
	      if ((samplesize <= 32) && (samplesize > 0))
		  if (samplesize < 9)
		      samplesize = 1;
		  else if (samplesize < 17)
		      samplesize = 2;
		  else if (samplesize < 25)
		      samplesize = 3;
		  else
		      samplesize = 4;
	      else
	      {
		  fprintf (stderr, "%s: cannot handle samplesize, considering wrong FMT_ chunk\n\r", nomfile);
		  samplesize = 0;
	      }
	      if (framesize != samplesize * nbchannel)
		  fprintf (stderr, "%s: warning: framesize %d is different from samplesize %d * nbchannels %d ???\n", nomfile, framesize, samplesize, nbchannel);

	      break;

	  case CHUNKwavdata:
	      if (samplesize == 0)
	      {
		  fprintf (stderr, "%s: entering data chunk without FMT_ chunk, skipped %ld bytes\n\r", nomfile, taille);
		  skipped = 1;
		  break;
	      }
	      if (sample->data != NULL)
	      {
		  fprintf (stderr, "%s: a second sample sound data chunk was found and is skipped %ld bytes\n\r", nomfile, taille);
		  skipped = 1;
		  break;
	      }
	      nbsample = taille / framesize;
#ifdef DEB_READSAMPLE
/* JDJDJDJD */ fprintf (stderr, "%s %2d channel of  %7ld x %3d bits at %ld Hz (framesize %d)\n\r", sample->shortname, nbchannel, nbsample, realsamplesize, rate, framesize);
#endif
	    /* allocation du sample qui sera converti */
	      if ((sample->data = (short *) malloc (nbsample * nbchannel * sizeof (short))) == NULL)
	      {
		  fprintf (stderr, "%s: probleme pour allouer %ld octets ??\n\r", nomfile, nbsample * nbchannel * sizeof (short));

		  skipped = 1;
		  break;
	      }
	      ps = sample->data;
	      switch (samplesize)
		{
		case 1:
		    for (n = 0; n < nbsample; n++)
		    {
			if (fread (buf, 1, framesize, f) != framesize)
			    break;
			for (ch=0 ; ch<nbreadch ; ch++)
			{   bbuf = &buf[ch * samplesize];
			    bbuf[0] ^= 0x80;
#ifndef DOWNSAMPLE
			    *ps++ = bbuf[0] << 8;
#else
			    if ((n & 1) == 0)
				*ps++ = bbuf[0] << 8;
#endif
			}
		    }
		    break;
		case 2:
		case 3:
		case 4:
		    for (n = 0; n < nbsample; n++)
		    {
			if (fread (buf, 1, framesize, f) != framesize)
			    break;
			for (ch=0 ; ch<nbreadch ; ch++)
			{   bbuf = &buf[ch * samplesize];
#if (BYTE_ORDER == LITTLE_ENDIAN)
#ifndef DOWNSAMPLE
			*ps++ = *(short *) bbuf;
#else
			if ((n & 1) == 0)
			    *ps++ = *(short *) bbuf;
#endif
#else
#ifndef DOWNSAMPLE
			*ps++ = (bbuf[0] << 8) | bbuf[1];
#else
			if ((n & 1) == 0)
			    *ps++ = (bbuf[0] << 8) | bbuf[1];
#endif
#endif
			}
		    }
		    break;
		default:
		    fprintf (stderr, "big internal problem #0001\n\r");
		    exit (1);
		}
#ifndef DOWNSAMPLE
	      sample->size = n;
#else
	      sample->size = n >> 1;
#endif
	      break;

	  case CHUNKwavLIST:
	  case CHUNKwavslnt:
	  case CHUNKwavfact:
	  case CHUNKwavcue:
	  case CHUNKwavplst:
	      skipped = 1;
	      break;

	  default:
	      fprintf (stderr, "%s: %c%c%c%c (0x%08lx) invalid chunk mark, skipping %ld bytes\n\r",
		       nomfile,
		       buf[0] >= 32 ? buf[0] : '?',
		       buf[1] >= 32 ? buf[1] : '?',
		       buf[2] >= 32 ? buf[2] : '?',
		       buf[3] >= 32 ? buf[3] : '?',
		       (long unsigned int) *(Sint32 *) buf,
		       taille);
	      skipped = 1;
	  }
	if (skipped)
	    fseek (f, taille, SEEK_CUR);
    }
    fclose (f);
    return sample->data;
}

short  *readsample_aiff (char *nomfile, Sample *sample)
{
    FILE   *f;
    struct stat Sfstat;
    unsigned char buf[AIFFBUFSIZE],
	    * bbuf;
    int     skipped = 0;
    long    taille = 0;
    short  *ps = NULL;
    long    nbsample = 0, n;
    int     samplesize = 0, nbchannel = 0, framesize = 0;
    long    rate = 0.0;
    int	    nbreadch = 0;
    int	    ch;

    if ((sample->data != NULL) || (sample->size != 0))
    {
	fprintf (stderr, "I dont overwrite samples, it must be freed before !\n\r");
	return sample->data;
    }
  /* par securite je le fais qd meme .... */
    initsample (sample);

    stat (nomfile, &Sfstat);

    if ((f = fopen (nomfile, "rb")) == NULL)
    {
	fprintf (stderr, "can't open %s\n\r", nomfile);
	return NULL;
    }
    fread (buf, 1, 12, f);	/* lecture chunk FORM */
    if ((chunkconv (*(Sint32 *) buf) != CHUNKFORM) || (chunkconv (*(Sint32 *) (buf + 8)) != CHUNKAIFF))
    {
	fprintf (stderr, "%s isnt aiff compliant: first chunk not COMMxxxxAIFF\n\r", nomfile);
	return NULL;
    }
    if (CONVlong (*(Sint32 *) (buf + 4)) + 8 != Sfstat.st_size)
	fprintf (stderr, "%s hasnt expected size: %ld instead of %ld\n\r", nomfile, Sfstat.st_size, CONVlong (*(Sint32 *) (buf + 4)) + 8);
    if ((sample->name = (char *) malloc (1 + strlen (nomfile))) == NULL)
    {
	fprintf (stderr, "impossible d'allouer %ld bytes ??? \n\r", strlen (nomfile));
	initsample (sample);
	return (NULL);
    }
    strcpy (sample->name, nomfile);

  /* on genere le nom court a partir du nom de fichier */
    {
	char   *p;

	p = strrchr (nomfile, '/');
	strcpy ((char *)buf, (p == NULL) ? nomfile : p + 1);
	p = strrchr ((char *)buf, '.');
	if (p != NULL)
	    *p = 0;
	buf[SHORTNAMESIZE] = 0;
	sprintf (sample->shortname, STRINGSHORTNAME, buf);
    }

    while (!feof (f))
    {
	if (fread (buf, 1, 8, f) != 8)
	    break;
	skipped = 0;
	taille = CONVlong (*(Sint32 *) (buf + 4));
	switch (chunkconv (*(Sint32 *) buf))
	  {
	  case CHUNKNAME:
	      if (taille > AIFFBUFSIZE - 1)
	      {
		  fprintf (stderr, "%s, name chunk too big, skipped\n\r", nomfile);
		  skipped = 1;
	      }
	      else
	      {
		  fread (buf, 1, taille, f);
		  buf[taille] = 0;
		  fprintf (stderr, "NAME: %s\n\r", buf);
		  buf[SHORTNAMESIZE] = 0;
		  sprintf (sample->shortname, STRINGSHORTNAME, buf);
	      }
	      break;
	  case CHUNKAUTH:
	      if (taille > AIFFBUFSIZE - 1)
	      {
		  fprintf (stderr, "%s, author chunk too big, skipped\n\r", nomfile);
		  skipped = 1;
	      }
	      else
	      {
		  fread (buf, 1, taille, f);
		  buf[taille] = 0;
		  fprintf (stderr, "%s: author: %s\n\r", nomfile, buf);
	      }
	      break;
	  case CHUNKRIGH:
	      if (taille > AIFFBUFSIZE - 1)
	      {
		  fprintf (stderr, "%s, copyright chunk too big, skipped\n\r", nomfile);
		  skipped = 1;
	      }
	      else
	      {
		  fread (buf, 1, taille, f);
		  buf[taille] = 0;
		  fprintf (stderr, "%s: (c) %s\n\r", nomfile, buf);
	      }
	      break;
	  case CHUNKANNO:
	      if (taille > AIFFBUFSIZE - 1)
	      {
		  fprintf (stderr, "%s, annotation chunk too big, skipped\n\r", nomfile);
		  skipped = 1;
	      }
	      else
	      {
		  fread (buf, 1, taille, f);
		  buf[taille] = 0;
		  fprintf (stderr, "%s: Annotation: %s\n\r", nomfile, buf);
	      }
	      break;
	  case CHUNKCOMT:
	    /* BUG: the comment chunk isnt handled */
	      skipped = 1;
	      break;
	  case CHUNKAPPL:
	  case CHUNKAESD:
	  case CHUNKMIDI:
	  case CHUNKINST:
	  case CHUNKMARK:
	      skipped = 1;
	      break;
	  case CHUNKCOMM:
	      if (taille > AIFFBUFSIZE)
	      {
		  fprintf (stderr, "%s, COMM chunk too big, skipping %ld byte\n\r", nomfile, taille);
		  skipped = 1;
		  break;
	      }
	      fread (buf, 1, taille, f);
	      nbchannel = CONVint (*(short *) buf);
	      if ((nbchannel < 1) || (nbchannel > 2))
		  fprintf (stderr, "%s: warning: %d data-channels, only the first will be used\n\r", nomfile, nbchannel);
	      nbreadch = nbchannel > 2 ? 2 : nbchannel;
	      sample->nbch = nbreadch;
	      nbsample = CONVlong (*(Sint32 *) (buf + 2));
	      samplesize = CONVint (*(short *) (buf + 6));
	      rate = (long) ConvertFromIeeeExtended (buf + 8);
/* JDJDJDJDJD should go to info ? */
#ifdef DEB_READSAMPLE
	      fprintf (stderr, "%s %2d channel of %7ld x %3d bits at %ld Hz\n\r", sample->shortname, nbchannel, nbsample, samplesize, rate);
#endif
	      if ((samplesize <= 32) && (samplesize > 0))
		  if (samplesize < 9)
		      samplesize = 1;
		  else if (samplesize < 17)
		      samplesize = 2;
		  else if (samplesize < 25)
		      samplesize = 3;
		  else
		      samplesize = 4;
	      else
	      {
		  fprintf (stderr, "%s: cannot handle samplesize, considering wrong COMM chunk\n\r", nomfile);
		  samplesize = 0;
	      }
	      framesize = samplesize * nbchannel;
/* JDJDJDJDJD should go to info ? */
/* fprintf (stderr, "samplesize %d bytes - framesize %d bytes\n\r", samplesize, framesize); */

	      break;
	  case CHUNKSSND:
	      if (samplesize == 0)
	      {
		  fprintf (stderr, "%s: entering sound chunk without COMM chunk, skipped %ld bytes\n\r", nomfile, taille);
		  skipped = 1;
		  break;
	      }
	      if (sample->data != NULL)
	      {
		  fprintf (stderr, "%s: a second sample sound chunk was found and is skipped %ld bytes\n\r", nomfile, taille);
		  skipped = 1;
		  break;
	      }
	    /* allocation du sample qui sera converti */
	      if ((sample->data = (short *) malloc (nbsample * nbreadch * sizeof (short))) == NULL)
	      {
		  fprintf (stderr, "%s: probleme pour allouer %ld octets ??\n\r", nomfile, nbsample * nbreadch * sizeof (short));

		  skipped = 1;
		  break;
	      }
	      fread (buf, 1, 8, f);
	      if ((*(Sint32 *) buf != 0) || (*(Sint32 *) (buf + 4) != 0))
		  fprintf (stderr, "%s: non-null offset or blocksize, ignored\n\r", nomfile);
	      ps = sample->data;
	      switch (samplesize)
		{
		case 1:
		    for (n = 0; n < nbsample; n++)
		    {
			if (fread (buf, 1, framesize, f) != framesize)
			    break;
			for (ch=0 ; ch<nbreadch ; ch++)
			{   bbuf = &buf[ch * samplesize];
#ifndef DOWNSAMPLE
			    *ps++ = bbuf[0] << 8;
#else
			    if ((n & 1) == 0)
				*ps++ = bbuf[0] << 8;
#endif
			}
		    }
		    break;
		case 2:
		case 3:
		case 4:
		    for (n = 0; n < nbsample; n++)
		    {
			if (fread (buf, 1, framesize, f) != framesize)
			    break;
			for (ch=0 ; ch<nbreadch ; ch++)
			{   bbuf = &buf[ch * samplesize];
#if (BYTE_ORDER == LITTLE_ENDIAN)
#ifndef DOWNSAMPLE
			    *ps++ = (bbuf[0] << 8) | bbuf[1];
#else
			    if ((n & 1) == 0)
				*ps++ = (bbuf[0] << 8) | bbuf[1];
#endif
#else
#ifndef DOWNSAMPLE
			    *ps++ = *(short *) bbuf;
#else
			    if ((n & 1) == 0)
				*ps++ = *(short *) bbuf;
#endif
#endif
			}
		    }
		    break;
		default:
		    fprintf (stderr, "big internal problem #0001\n\r");
		    exit (1);
		}
#ifndef DOWNSAMPLE
	      sample->size = n;
#else
	      sample->size = n >> 1;
#endif
	      break;
	  default:
	      fprintf (stderr, "%s: %c%c%c%c (0x%08lx) invalid chunk mark, skipping %ld bytes\n\r",
		       nomfile,
		       buf[0] >= 32 ? buf[0] : '?',
		       buf[1] >= 32 ? buf[1] : '?',
		       buf[2] >= 32 ? buf[2] : '?',
		       buf[3] >= 32 ? buf[3] : '?',
		       *(Sint32 *) buf,
		       taille);
	      skipped = 1;
	  }
	if (skipped)
	    fseek (f, taille, SEEK_CUR);
    }
    fclose (f);
    return sample->data;
}

short  *readsample (char *nomfile, Sample *sample)
{
    char   *p = NULL;
    short  *tempo;

    if (firstuse)
    {
#ifdef SHOWRCS
	fprintf (stderr, "%s\n\r", rcsid);
#endif
	firstuse = 0;
    }

    if (nomfile == NULL)
	return NULL;

    p = strrchr (nomfile, '.');
    if (p != NULL)
	switch (*(Sint32 *) p)
	  {
	  case 0x7661772e:	/* .wav */
	  case 0x5641572e:	/* .WAV */
	  case 0x7661572e:	/* .Wav */
	      return readsample_wav (nomfile, sample);
	  case 0x6669612e:	/* .aif */
	  case 0x4649412e:	/* .AIF */
	  case 0x6669412e:	/* .Aif */
	      return readsample_aiff (nomfile, sample);
	  default:
	      break;
	  }

    fprintf (stderr, "could not find out nature of %s, trying wav...\n", nomfile);
    if ((tempo = readsample_wav (nomfile, sample)) != NULL)
	return tempo;
    fprintf (stderr, "not good, trying aiff...\n");
    if ((tempo = readsample_aiff (nomfile, sample)) != NULL)
	return tempo;
    fprintf (stderr, "not good, aborting...\n");
    return NULL;
}
