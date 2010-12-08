/* 
 * $Id: grdelay.c,v 1.2 2003/03/19 16:45:47 jd Exp $
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
 * $Log: grdelay.c,v $
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
 * Revision 1.2.0.23.0.5  1999/10/07 14:08:05  jd
 * Revision 1.2.0.23.0.2  1999/09/27 00:46:09  jd
 * correction from API
 *
 * Revision 1.2.0.23  1999/09/15 10:20:39  jd
 * second pre tel-aviv public revision, for testing
 *
 * Revision 1.2.0.22.0.2  1999/09/15 09:23:30  jd
 * most features are brand new
 *
 *
 */

#include <stdio.h>

#include "config.h"

#include "controls.h"
#include "sample.h"
#include "placement.h"
#include "grlevels.h"
#include "grmixeur.h"
#include "groovit.h"

#define INMAINGRDELAY
#include "grdelay.h"
/* 
 * ---------------------------------- Screen Part ------------------------------------
 */

/* 
 * ---------------------------------- Content Part -----------------------------------
 */

void    reset_delay (void)
{
    delaytick = 4;
    rdelay = 4 * 2000;
    tdelay = 4 * 2000;
    decay = 125;
}

static short	
	*recharge = & reverbbuf2 [MAXREVERB][0],
	*nbufin, 
	*nbufout,
	*recharge3 = & reverbbuf3 [MAXREVERB][0],
	*nbufin3, 
	*nbufout3;

int     tickchange_delay (int voice)
{
    if (delaytick > 0)
	rdelay = delaytick * lnss;
    else
    {
	if (delaytick == 0)
	    rdelay = lnss;
	else
	    rdelay = lnss / -delaytick;
    }

    nbufout = nbufin - (delay2 << 4)  ;
    while (nbufout < &reverbbuf2 [0][0])
	nbufout += 2 * MAXREVERB;

    nbufout3 = nbufin3 - (delay3 << 4);
    while (nbufout3 < &reverbbuf3 [0][0])
	nbufout3 += 2 * MAXREVERB;

    return voice;
}

static int *mixl, *mixr, *mixm;		/* pointeurs sur les summerisations de mixeurs */

int     generation_delay (int *outl, int *outr, int voice)
{
    long tmp;
static long tmp2r, tmp2l, tmp3l, tmp3r;

    tdelay--;
    if (!tdelay)
    {
	rbuf = (short *) reverbbuf;
	tdelay = rdelay;
    }

    if (nbufin >= recharge) nbufin = & reverbbuf2 [0][0];
    if (nbufout >= recharge) nbufout = & reverbbuf2 [0][0];
    if (nbufin3 >= recharge3) nbufin3 = & reverbbuf3 [0][0];
    if (nbufout3 >= recharge3) nbufout3 = & reverbbuf3 [0][0];

    *outl += *rbuf;

    *outl += *nbufout;
    tmp2l = (((long) *nbufout * decay2) >> 8);
    tmp = *mixl + tmp2l + ((tmp3l * inject2) >> 8);
    nbufout++;
    if (tmp < -32766)
	tmp = -32766;
    else if (tmp > 32767)
	tmp = 32767;
    *nbufin++ = tmp;

    *outl += *nbufout3;
    tmp3l = (((long) *nbufout3 * decay3) >> 8);
//    tmp = tmp3l + ((tmp2l * inject2) >> 8);
    tmp = *mixl + tmp3l;
    nbufout3++;
    if (tmp < -32766)
	tmp = -32766;
    else if (tmp > 32767)
	tmp = 32767;
    *nbufin3++ = tmp;


    *mixl += ((long) *rbuf * decay) >> 8;
    if (*mixl < -32766)
	*mixl = -32766;
    else if (*mixl > 32767)
	*mixl = 32767;
    *rbuf++ = *mixl;


    *outr += *rbuf;

    *outr += *nbufout;
    tmp2r = (((long) *nbufout * decay2) >> 8);
    tmp = *mixr + tmp2r + ((tmp3r * inject2) >> 8);
    nbufout++;
    if (tmp < -32766)
	tmp = -32766;
    else if (tmp > 32767)
	tmp = 32767;
    *nbufin++ = tmp;

    *outr += *nbufout3;
    tmp3r = (((long) *nbufout3 * decay2) >> 8);
//    tmp = tmp3r + ((tmp2r * inject2) >> 8);
    tmp = *mixr + tmp3r;
    nbufout3++;
    if (tmp < -32766)
	tmp = -32766;
    else if (tmp > 32767)
	tmp = 32767;
    *nbufin3++ = tmp;

    *mixr += ((long) *rbuf * decay) >> 8;
    if (*mixr < -32766)
	*mixr = -32766;
    else if (*mixr > 32767)
	*mixr = 32767;
    *rbuf++ = *mixr;

    *mixl = *mixr = *mixm = 0;

    return voice;
}

void    init_delay (void)
{
    rbuf = (short *) reverbbuf;

    nbufin =  & reverbbuf2 [0][0];
    nbufout =  & reverbbuf2 [0][0];
    nbufin3 =  & reverbbuf3 [0][0];
    nbufout3 =  & reverbbuf3 [0][0];

    decay2 = 0;
    inject2 = 0;
    decay3 = 0;
//    createrange (CONTRNUMRANGE, ROWREV + 2, LINEDIV + 2, 0, CONTUNDEF, &delaytick, MINREV, MAXREV);
//    createcontrols (CONTRHSLIDE, ROWREV, LINEDIV + 3, 0, CONTUNDEF, &decay);
    createrange (CONTRNUMRANGE, ROWREV + 2, LINEDIV , 0, CONTUNDEF, &delaytick, MINREV, MAXREV);
    createcontrols (CONTRHSLIDE, ROWREV+ 9, LINEDIV , 0, CONTUNDEF, &decay);

    createrange (CONTRNUMRANGE, ROWREV + 2, LINEDIV + 2, 0, CONTUNDEF, &delay2, 0, 2048);
    createcontrols (CONTRHSLIDE, ROWREV + 9, LINEDIV + 2, 0, CONTUNDEF, &decay2);
    createcontrols (CONTRHSLIDE, ROWREV + 9, LINEDIV+1 , 0, CONTUNDEF, &inject2);

    createrange (CONTRNUMRANGE, ROWREV + 2, LINEDIV+4 , 0, CONTUNDEF, &delay3, 0, 2048);
    createcontrols (CONTRHSLIDE, ROWREV + 9, LINEDIV+4 , 0, CONTUNDEF, &decay3);

    registertomixeur (&mixl, &mixr, &mixm, " delay  ", 2);
    show_levels (2, 2);
}
