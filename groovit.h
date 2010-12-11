/* 
 * $Id: groovit.h,v 1.2 2003/03/19 16:45:47 jd Exp $
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
 * $Log: groovit.h,v $
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
 * Revision 1.2.0.22.0.8  1999/09/15 09:23:30  jd
 * split in more modules
 * added mono 16 bit handling for fake Maestro driver at OSS
 * added handling of intermediate mono channel for mixing (dyfilter)
 * added an hack for 2.2.x hanging write with timer (for gpm mouse) :
 * non-self-interrupted writes
 *
 * Revision 1.2.0.22.0.6  1999/09/05 22:30:16  jd
 * corrected for granalogik, grdyfilter and resoanlog
 * modules split...
 * still needs a lot of cleaning
 *
 * Revision 1.2.0.22.0.5  1999/08/30 22:16:03  jd
 * corrections for new modules grsel grpattern and board_menu.
 *
 * Revision 1.2.0.22.0.4  1999/08/29 22:49:01  jd
 * added exported function for grsel split
 *
 * Revision 1.2.0.22.0.3  1999/08/29 17:03:57  jd
 * new function exported for splitting
 * corrected export of match strings
 *
 * Revision 1.2.0.21.0.1  1998/12/07 11:43:07  jd
 * une version pour jcd
 *
 *
 */

#ifndef HOPEDRESO16
#ifndef HOPEDRESO8
#ifndef HOPEDMONO16
#error no output bit-witdth set : you must run "make config" or ./Configure
#endif
#endif
#endif

#ifdef INMAINGROOVIT
#define _groovitc
#else
#define _groovitc	extern
#endif

#include "radio.h"

/* les definitions d'alertes */
#define	ALERTNONE	0
#define	ALERTGPL	1
#define	ALERTQUIT	2
#define ALERTLANY	3
#define	ALERTSAVEQUIT	5
#define	ALERTERA	6

/* les definitions de selecteurs de fichiers */
#define FS_CLOSED	0
#define FS_SAMPLE	1
#define FS_SAVE		2
#define FS_LOAD		3
#define FS_RAWF		4
#define FS_SAVEQUIT	5

#include "constants.h"

// #ifdef HOPEDRESO16
// #define SIZEBUFAHEAD (PLAYAHEAD*4)      /* taille du playahead en octets */
// #endif
// #ifdef HOPEDRESO8
// #define SIZEBUFAHEAD (PLAYAHEAD*2)      /* taille du playahead en octets */
// #endif
// #ifdef HOPEDMONO16
// #define SIZEBUFAHEAD (PLAYAHEAD*2)      /* taille du playahead en octets */
// #endif

/* =============================== debut des variables =========================== */

_groovitc char cursamplepath[MAXDIRSIZE],	/* le chemin courant de fileselect sample */
        startdir[MAXDIRSIZE],  /* le repertoire d'ou on a ete lance */
        cursongfile[MAXDIRSIZE],	/* le nom courant de la chanson */
       *shortcursongfile;      /* idem en plus court ?? */

#ifdef SOUND_DEVICE_NAMES
_groovitc char *channelsname[] = SOUND_DEVICE_NAMES;	/* etiquettes des channels */

#endif

/* ---------------- pour la gestion de fichiers et de menus ------------------- */

_groovitc char nfoutbis[MAXDIRSIZE];	/* nom du fichier de sortie secondaire */
_groovitc int numoutbis;       /* numerotation des fichiers raw */
_groovitc FILE *foutbis;       /* sortie secondaire pour faire du raw-to-disk */

#ifdef INMAINGROOVIT		/* tableau de chaine pour "matcher" les fichiers */

				/* de type "son"                                */
char   *match_sound[] =
{".aif", "wav", ".WAV", ".Wav", NULL},	/* de type wav only                             */
       *match_wav[] =
{".wav", ".WAV", NULL},	       /* de type groovit                              */
       *match_grov[] =
{".grov", ".grav", NULL},      /* main menu                                    */
       *gname = "gr" "oo" "vit",
       *instancename;

#else
extern char *match_sound[], *match_wav[], *match_grov[], *fmenmain[], *gname, *instancename;

#endif

/* ---------------- les variables d'etat -------------------------------------- */

_groovitc
int     allpower,	       /* le bouton on/off general                     */
        bpm,		       /* tickBPM en beat per minute                   */
// JDJDJDJD a supprimer pour de bon bientot...
//        ncurpattern,	       /* no de pattern en cour de jeu (Cf curpattern) */
        myrawingtodisk;	       /* sommes nous en train de rawer sur disque ?   */

/* ---------------------------------------------------------------------------- */

_groovitc
long    ts,		       /* OBSOLETE "timesample" nb de sample depuis le */
				/* debut du morceau, par increments             */
        tss,		       /* OBSOLETE "timesample" nb de sample depuis le */
				/* debut du morceau, varie continuement         */
        endtss,		       /* decompte du PLAYAHEAD en cour par sample     */
        nss,		       /* decompte du prochain TickBPM en sample       */
        lnss,		       /* duree d'un TickBPM en sample                 */
        t,		       /* current time en TickBPM                      */
        totleft,	       /* valeur moyenne de l'output gauche            */
        totright;	       /* valeur moyenne de l'output droite            */
// JDJDJDJD ca devrait pouvoir disparaitre...
//        tp,		       /* current pattern time en TickBPM              */
//        curnbrow;	       /* current pattern length (TickBPM of course)   */

_groovitc
long    cursamp[MAXSAMPLE];		/* duree restante du sample de la ieme voie     */

_groovitc
#ifdef HOPEDMONO16
unsigned short reserve[MAXPLAYAHEAD][1],	/* buffer de PLAYAHEAD          */
       *pbuf;		       /* pointeur sur outsample en cours              */

#endif
#ifdef HOPEDRESO16
unsigned short reserve[MAXPLAYAHEAD][2],	/* buffer de PLAYAHEAD          */
       *pbuf;		       /* pointeur sur outsample en cours              */

#endif
#ifdef HOPEDRESO8
unsigned char reserve[MAXPLAYAHEAD][2],		/* buffer de PLAYAHEAD          */
       *pbuf;		       /* pointeur sur outsample en cours              */

#endif

_groovitc
unsigned short
        maxleft,	       /* valeur max de l'output a gauche              */
        maxright;	       /* valeur max de l'output a droite              */

_groovitc
int     tlev[MAXSAMPLE],       /* niveau dynamique mixage du ieme voix sample  */
        levell[MAXSAMPLE],     /* niveau de mixage de la ieme voie a gauche    */
        levelr[MAXSAMPLE],     /* niveau de mixage de la ieme voie a droite    */
        revlevel[MAXSAMPLE],   /* niveau d'injection en reverb de la ieme voie */
        filtl[MAXSAMPLE],      /* niveau d'injection dans le filtre ieme voie  */
        alevell[MAXANALOG],    /* niveau de mixage de la ieme voie analogique a gauche */
        alevelr[MAXANALOG],    /* niveau de mixage de la ieme voie analogique a droite */
        arevlevel[MAXANALOG],  /* niveau d'injection en reverb de la ieme voie analogique */
        flevell[MAXDYFILT],    /* niveau de mixage du ieme filtre a gauche     */
        flevelr[MAXDYFILT],    /* niveau de mixage du ieme filtre a droite     */
        l,		       /* construction de l'outsample gauche           */
        r,		       /* construction de l'outsample droit            */
        FLtmp,		       /* construction de l'entree du filtre           */
        absl,		       /* absolute val left                            */
        absr;		       /* absolute val right                           */

/*----------------------------definitions pour le calcul de frequences----------*/
_groovitc
int     tabfreq[257];	       /* tableau de 257 frequences etagees par quarts de tons */

/*----------------------------buffers et pointeurs de sons----------------------*/

_groovitc
short  *psamp[MAXSAMPLE];      /* pointeur de sample en cours pour ieme voie   */

/*------------------------------------------------------------------------------*/

#ifndef INMAINGROOVIT		/* des fonctions exportees                        */
void    initallvoices (void);

#endif
