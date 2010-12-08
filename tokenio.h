/* 
 * $Id: tokenio.h,v 1.2 2003/03/19 16:45:47 jd Exp $
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
 * $Log: tokenio.h,v $
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
 */

#include <stdio.h>

#ifndef FTOK_MAXDEEP
#define FTOK_MAXDEEP	4	/* maximum recursivity for token-writing */
#endif

#ifndef FTOK_MAXFILE
#define FTOK_MAXFILE	4	/* maximum number of tokfile at a glance... */
#endif
				/* errors MUST be negative values */
#define FTOK_NOSPACE	-2	/* MOL no more space on device */
#define FTOK_LIMIT	-3	/* internal allocation error */
#define FTOK_EXCEED	-4	/* over sized token */
#define FTOK_ERROR	-1	/* general error :) huh huh huh ! */

#define FTOK_READ	1
#define FTOK_WRITE	2

typedef struct STtokfile
{
    FILE   *f;
    int     deep;
#ifdef USEFGETPOS
    fpos_t  offset[FTOK_MAXDEEP];
#else
    long    offset[FTOK_MAXDEEP];
#endif
    long    remain[FTOK_MAXDEEP];
    long    curtok[FTOK_MAXDEEP];
}
TokFile;

int     ckfwrite (const void *ptr, size_t size, FILE * stream);		/* pour ecrire en

									   interruptions ... */
int     ckfwrite_short (FILE * stream, int v);	/* idem pour un short       */
int     ckfwrite_datas (FILE * stream, const int *p, int nb);	/* idem pour des short avec

								   compression fake rle */

int     ckfread (void *ptr, size_t size, FILE * stream);	/* pour lire en interruptions 

								   ...   */
int     cktread (void *ptr, size_t size, TokFile *tf);	/* pour lire dans un token avec

							   controle */
int     cktread_short (TokFile *tf, int *psh);	/* idem pour lire un short  */
int     ckfread_datas (TokFile *tf, int *p, int nb);	/* idem pour des short avec

							   compression fake rle */
long    ftok_readraw (TokFile *tf);	/* lecture d'un token en mode raw   */
int     ftok_closereadraw (TokFile *tf, long tokident, long *reste);	/* fermeture d'un

									   token en lecture */

int     ftok_init (void);      /* initialisation du module */
TokFile *ftok_open (char *nomfile, int mode);	/* ouvrir un fichier ftok   */
int     ftok_close (TokFile *tf);	/* le fermer...     */
int     ftok_writetoken (TokFile *tf, long tokident, size_t size, void *data);	/* ecrire un

										   tok    */
int     ftok_startraw (TokFile *tf, long tokident);	/* ecriture raw d'un tok    */
int     ftok_endraw (TokFile *tf, long tokident);	/* fermeture de celle-ci    */
