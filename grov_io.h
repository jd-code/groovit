/* 
 * $Id: grov_io.h,v 1.2 2003/03/19 16:45:47 jd Exp $
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
 * $Log: grov_io.h,v $
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
 * Revision 1.2.0.22.0.5  1999/09/15 09:23:30  jd
 * handles the reseverd mixers and primary handling of voices
 *
 */

	/* tokens def for .grov file format */

#define CHUNKgrov 0x766f7267
#define CHUNKable 0x656c6261
#define CHUNKgprm 0x6d727067	/* General PaRaMeters */
#define CHUNKdelp 0x706c6564	/* DELay Parameters */
#define CHUNKsdst 0x74736473	/* Sample DiSTribution */
#define CHUNKrptn 0x6e747072	/* Rythms (samples) PaTerN */
#define CHUNKaptn 0x6e747061	/* Analog PaTerN */
#define CHUNKaprm 0x6d727061	/* Analog PaRaMeters */
#define CHUNKfptn 0x6e747066	/* Filter PaTerN */
#define CHUNKfprm 0x6d727066	/* Filter PaRaMeters */

extern long vernumber;

char   *relativename (char *ref, char *name);
int     isreallyafile (char *name);
int     save_cur_song (char *name, int mode);
int     load_cur_song (char *name, int mode);
int     init_grov_io (char *groovit_rcsid);
