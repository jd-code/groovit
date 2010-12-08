/* 
 * $Id: resoanlog.c,v 1.2 2003/03/19 16:45:47 jd Exp $
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
 * $Log: resoanlog.c,v $
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
 * Revision 1.2.0.22.0.2  1999/09/05 22:30:16  jd
 * creation
 *
 *
 */

#include "stdio.h"

#include "config.h"

#include "resoanlog.h"
#include "sample.h"
#include "placement.h"
#include "groovit.h"

void    initAnReso (AnReso * reso)
{
    reso->hig = 0;
    reso->mid = 0;
    reso->low = 0;
    reso->feedbk = 0;
    reso->hig2 = 0;
    reso->mid2 = 0;
    reso->low2 = 0;
    reso->freq = 0;

    reso->ctfreqcut = 0;
    reso->ctfreqcut2 = 0;
    reso->ctreso = 255;
    reso->ctdecay = 255;
    reso->ampmode = 0;

    reso->amp = 0;
}

void    initAnalogik (Analogik * p)
{
    p->pan = 128;
    p->level = 0;
    p->revlevel = 0;
}
