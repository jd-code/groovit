/* 
 * $Id: settings.h,v 1.2 2003/03/19 16:45:47 jd Exp $
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
 * $Log: settings.h,v $
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
 * Revision 1.2.0.23.0.5  1999/10/07 14:08:05  jd
 * Revision 1.2.0.23.0.3  1999/10/02 17:22:04  jd
 * complete rewrite of scan_rcfile
 * handles partial/complete total/local write/read.
 *
 * Revision 1.2.0.23.0.2  1999/09/30 01:09:04  jd
 * added embryonic rcfile handling
 *
 * Revision 1.2.0.23  1999/09/15 10:20:39  jd
 * second pre tel-aviv public revision, for testing
 *
 * Revision 1.2.0.22.0.2  1999/08/30 22:16:03  jd
 * inclusion corrections
 *
 * Revision 1.2.0.22.0.1  1999/08/30 08:49:27  jd
 * Initial revision
 *
 *
 */

#ifndef SETTINGH
#define SETTINGH

#define MAXREADLINE 1024

#ifndef GLOBALRCFILE
#define GLOBALRCFILE "/usr/local/lib/groovit/groovitrc"
#endif

#ifndef RCFILENAME
#define RCFILENAME ".groovitrc"
#endif

#ifdef INMAINSETTING
#define _csetting
#else
#define _csetting	extern
#endif
typedef struct TSettings
{
    char   *tag,	       /* chaine de tag                                */
           *defv;	       /* valeur par defaut                            */
    int     type, portee, checked;
    void   *v;
}
Settings;

#define SETT_READALL		1
#define SETT_READMATCH		2
#define SETT_WRITEALL		3
#define SETT_WRITEMATCH		4
#define SETT_GLOBAL		16

#define SETT_BOOL		1
#define SETT_INT		2
#define SETT_LONG		3
#define SETT_VER		4
#define SETT_STR		5

#define SETT_NORMAL		1
#define SETT_LOCAL		2

_csetting
int     ctxsttg,	       /* handle du context de settings                */
        firsttimeforthis,      /* est-ce le premier usage de groovit ?         */
        gpl_accepted;	       /* est-ce que la gpl a ete acceptee ?           */

_csetting
long    readrevision;	       /* version du HOME/.groovitrc                   */

_csetting
long    kplayahead,	       /* taille du playahead en Ksamples */
        playahead,	       /* taille du playahaed en samples */
        sizebufahead;	       /* taille du playahead en octets */

int     write_pref (void **liste);
int     reset_settings (char *rcfile);
int     initsettings (void);

#endif
