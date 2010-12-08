/* 
 * $Id: placement.h,v 1.2 2003/03/19 16:45:47 jd Exp $
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
 * $Log: placement.h,v $
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
 * Revision 1.2.0.23.0.4  1999/10/07 14:08:05  jd
 * Revision 1.2.0.23  1999/09/15 10:20:39  jd
 * second pre tel-aviv public revision, for testing
 *
 * Revision 1.2.0.22.0.2  1999/08/29 16:58:26  jd
 * added fake settings
 *
 * Revision 1.2.0.22  1999/08/24 22:42:20  jd
 * telaviv pre-release
 *
 * Revision 1.2.0.21.1.5  1998/12/21 16:46:09  jd
 * added scrolling inside patterns
 *
 * Revision 1.2.0.21.0.1  1998/12/07 11:43:07  jd
 * une version pour jcd
 *
 */

/* constantes de placement des controls */
#define ROWNAME		1
#define ROWMENU         ROWNAME+1
#define ROWRYTHM	SHORTNAMESIZE+2
#define ROWLEVEL	ROWRYTHM+1+MAXDISPLAYROW
#define ROWPAN		ROWLEVEL+9
#define ROWREV		ROWPAN+9
#define ROWSPECIAL	ROWREV+9

#define ROWNBROW	ROWRYTHM-7
#define ROWFREQ1	ROWRYTHM
#define ROWFREQ2	ROWFREQ1+9
#define ROWRESO		ROWFREQ2+9
#define ROWDECAY	ROWRESO+9
#define ROWNBROWBIS	ROWDECAY+9

#define LINELEGEND	1
#define LINEMENU 	LINELEGEND+1
#define LINERYTHM	LINELEGEND+1
#define LINEDIV		LINERYTHM+MAXSAMPLE
#define LINEFILTER	LINEDIV+4
#define LINEANALOG	LINEFILTER+2*MAXDYFILT

#define ROWFILESEL	ROWRYTHM
#define LINEFILESEL	LINERYTHM
#define LARGFILESEL	MAXDISPLAYROW
#define HAUTFILESEL	MAXSAMPLE
#define LARGDIRNAME	14

#define ROWSTTG		ROWRYTHM
#define LINESTTG	LINERYTHM
#define LARGSTTG	40
#define HAUTSTTG	10
/* #define LARGSTTG     MAXDISPLAYROW #define HAUTSTTG  MAXSAMPLE */
