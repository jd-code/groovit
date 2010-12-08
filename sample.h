/* 
 * include generique commun pour le type "Sample"
 *
 * $Id: sample.h,v 1.2 2003/03/19 16:45:47 jd Exp $
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
 */

#define SHORTNAMESIZE 11	/* taille des noms courts pour les samples */
#define STRINGSHORTNAME "%11s"	/* comment faire un printf shortname ...   */

typedef struct STSample
{
    short  *data;	       /* pointeur sur le sample au format 16 bit nbchan entrelaces  */
    long    size;	       /* taille. NUL ==> le sample est invalide.        */
    int	    nbch;	       /* nombre de canaux				 */
    int     level;	       /* niveau de sortie                               */
    int     pan;	       /* spatialisation                                 */
    int     revlevel;	       /* niveau de reverb                               */
    int     filtl;	       /* niveau d'entree en filtre                      */
    char    shortname[SHORTNAMESIZE + 1],	/* le nom court du sample pour affichage
						       */
           *name;	       /* le nom de fichier du sample                    */
}
Sample;
