/* 
 * $Id: testdsp.c,v 1.2 2003/03/19 16:45:47 jd Exp $
 * Groovit Copyright (C) 1998 Jean-Daniel PAUGET
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
 */

#include <stdio.h>
#include <endian.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/soundcard.h>

int main (int nb, char **cmde)
{
    int	fdsp,			/* dsp file handle */
	reso = 16,		/* 8 or 16 bit resolution */
	ster = 2,		/* mono or stereo */
	rate = 44100,		/* taux echantillonnage */

	is22 = 0,
	is44 = 0,
	isst = 0,
	is16 = 0,
	is8  = 0;
	
	if (nb != 2)
	{	printf ("ECHEC=BADUSE\n");
		return -1;
	}

	fdsp = open (cmde[1], O_WRONLY);

	if (fdsp == -1)
	{	int e = errno;
		switch (e)
		{	case EACCES:
				printf ("ECHEC=DENIED\n");
				break;
			case EBUSY:
				printf ("ECHEC=BUSY\n");
				break;
		}
		printf ("ECHEC=ERR%d\n", e);
		return (-1);
	}
	printf ("ECHEC=0\n");

	reso = 16;
	if (ioctl (fdsp, SOUND_PCM_WRITE_BITS, &reso) == -1)
		is16 = 0;
	else	if (reso != 16) is16 = 0;
		else 		is16 = 1;

	if (!is16)
	{	reso = 8;
		if (ioctl (fdsp, SOUND_PCM_WRITE_BITS, &reso) == -1)
			is8 = 0;
		else	if (reso != 16) is8 = 0;
			else 		is8 = 1;
	}

	ster = 2;	
	if (ioctl (fdsp, SOUND_PCM_WRITE_CHANNELS, &ster) == -1)
		isst = 0;
	else	if (ster != 2) isst =0;
		else	       isst = 1;

	rate = 44100;
	if (ioctl (fdsp, SOUND_PCM_WRITE_RATE, &rate) == -1)
		is44 = 0;
	else	if (((rate-44100*100)/44100) > 5 )
			is44 = 0;
		else
			is44 = 1;
	if (!is44)
	{	rate = 22050;
		if (ioctl (fdsp, SOUND_PCM_WRITE_RATE, &rate) == -1)
			is22 = 0;
		else	if (((rate-22100*100)/22100) > 5 )
				is22 = 0;
			else
				is22 = 1;
	}

	if (is16) printf ("HOPEDRESO=16\nHOPEDRESOBIS=HOPEDRESO16\n");
	else if (is8) printf ("HOPEDRESO=8\nHOPEDRESOBIS=HOPEDRESO8\n");

	if (isst) printf ("STEREO=2\n");
	else	  printf ("STEREO=1\n");

	if (is44) printf ("HOPEDSPRATE=44100\n");
	else	  printf ("HOPEDSPRATE=22050\n");

/* why groovit will fail ... */
	if (	(!isst) 
             || ((!is44) && (!is22)) 
             || ((!is16) && (!is8)) )
	{
		printf ("ECHEC=NOTENOUGH\nREASON=\"");
		if (!isst) printf ("no stereo ");
		if ((!is44) && (!is22)) printf ("handles neither 44KHz nor 22KHz ");
		if ((!is16) && (!is8)) printf ("handles neither 8bits nor 16bits sounds ");
		printf ("\"\n");
	}

	return 0;
}

