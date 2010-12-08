/* 
 * $Id: raw2disk.c,v 1.2 2003/03/19 16:45:47 jd Exp $
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
 * $Log: raw2disk.c,v $
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
 *
 */

#include <stdio.h>

#include "config.h"
#include "sample.h"

static char *rcsid = "$Id: raw2disk.c,v 1.2 2003/03/19 16:45:47 jd Exp $";
static int firstuse = 1;

#define CHUNKFMT 0x20746d66	/* chunk for wave file "fmt " */

FILE   *openwaveout (char *nom, long rate, int nbbits)
{
    FILE   *f;
    char    buf[128];

    if (firstuse)
    {
	fprintf (stderr, "%s\n\r", rcsid);
	firstuse = 0;
    }

    f = fopen (nom, "wb");
    if (f == NULL)
    {
	fprintf (stderr, "impossible d'ouvrir %s, aborting", nom);
	return (NULL);
    }

    fwrite ("RIFF\0\0\0\0WAVE", 12, 1, f);	/* container chunk (needs to be completed
						   later) */

    *(long *) (buf) = CHUNKFMT;
    *(long *) (buf + 4) = 16;	/* sizeof chunk */
    *(short *) (buf + 8) = 1;	/* we're writing some PCM */
    *(short *) (buf + 10) = 2;	/* we're writing stereo */
    *(long *) (buf + 12) = rate;	/* sampling rate */
    *(long *) (buf + 16) = rate << 2;	/* nb of bytes/s */
    *(short *) (buf + 20) = 2 * (nbbits >> 3);	/* nb of bytes per block = 2*Xbits */
    *(short *) (buf + 22) = nbbits;	/* nb of bits per sample (relative to the fact we
					   rite PCMs */

    fwrite (buf, 24, 1, f);	/* wave-format chunk wrote */

    fwrite ("data\0\0\0\0", 8, 1, f);	/* beggining of data chunk nedds to be completed
					   later */

    return (f);
}

int     closewaveout (FILE * f)
{
    long    totalsize = 0, offset = 0;

    fseek (f, 0, SEEK_END);
    totalsize = ftell (f);

    fseek (f, 4, SEEK_SET);
    offset = totalsize - 8;
    fwrite (&offset, 4, 1, f);	/* terminating container chunk */

    fseek (f, 40, SEEK_SET);
    offset = totalsize - 44;
    fwrite (&offset, 4, 1, f);	/* terminating data chunk */

    return fclose (f);
}
