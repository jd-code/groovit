/* 
 * $Id: grderiv.c,v 1.2 2003/03/19 16:45:47 jd Exp $
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
 * $Log: grderiv.c,v $
 * Revision 1.2  2003/03/19 16:45:47  jd
 * major changes everywhere while retrieving source history up to this almost final release
 *
 * Revision 1.2.0.23.0.1  1999/09/15 10:35:48  jd
 * Revision 1.2.0.23  1999/09/15 10:20:39  jd
 * second pre tel-aviv public revision, for testing
 *
 * Revision 1.2.0.22.0.2  1999/09/15 09:23:30  jd
 * most features are brand new
 *
 *
 */

#include <stdio.h>
#include <math.h>

#include "config.h"

#include "controls.h"
#include "sample.h"
#include "placement.h"
#include "grlevels.h"
#include "grmixeur.h"
#include "groovit.h"

#define INMAINGRDERIV
#include "grderiv.h"
/* 
 * ---------------------------------- Screen Part ------------------------------------
 */

/* 
 * ---------------------------------- Content Part -----------------------------------
 */

static int distort[32768], volout = 0;

void    reset_deriv (void)
{
    int     i, j = 1;

// for (i=0 ; i<32768 ; i++) distort[i] = i;
  /* 
     for (i=0 ; i<32768 ; i++) {  if (i >= j) j*= 2; distort[i] =
     (int)(sin((double)i*(3.141592654/(double)j))*(double)j); } */
    j = 32767;
    for (i = 0; i < 32768; i++)
	distort[i] = (int) (sin ((double) i * (3.141592654 / (double) j)) * (double) j);
}

static int *mixl, *mixr, *mixm;		/* pointeurs sur les summerisations de mixeurs */

int     generation_deriv (int *outl, int *outr, int voice)
{
    int     temp;

    if (*mixm < -32766)
	*mixm = -32766;
    else if (*mixm > 32767)
	*mixm = 32767;

    temp = (*mixm > 0) ? distort[*mixm] : -distort[-*mixm];
    temp = (((long) temp * volout) >> 8);
    *outl += temp;
    *outr += temp;

    *mixl = *mixr = *mixm = 0;

    return voice;
}

void    init_deriv (void)
{

    createcontrols (CONTRHSLIDE, ROWREV + 9, LINEDIV + 3, 0, CONTUNDEF, &volout);

    registertomixeur (&mixl, &mixr, &mixm, " deriv  ", -1);
/* show_levels (2, 2);  */
}
