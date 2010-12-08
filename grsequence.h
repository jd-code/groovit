/* 
 * $Id: grsequence.h,v 1.1 2003/03/19 21:56:36 jd Exp $
 * Groovit Copyright (C) 1999 Jean-Daniel PAUGET
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
 * $Log: grsequence.h,v $
 * Revision 1.1  2003/03/19 21:56:36  jd
 * small rcs to cvs typo corrected
 *
 * Revision 1.2.0.24.0.3  2000/10/01 21:07:03  jd
 * I dunno
 *
 * Revision 1.2.0.24.0.2  1999/10/19 19:33:49  jd
 * *** empty log message ***
 *
 * Revision 1.2  1999/10/19 19:32:42  jd
 * *** empty log message ***
 *
 * Revision 1.1  1999/10/19 19:31:35  jd
 * Initial revision
 *
 */

#ifndef GRSEQUENCEH
#define GRSEQUENCEH

#ifndef INMAINSEQ
#define _sequencec
#else
#define _sequencec   extern
#endif

typedef struct STPatbut		/* gestion des boutons de pattern               */
{
    int     tp,			/* current pattern time en TickBPM              */
            curnbrow,		/* current pattern length (TickBPM of course)   */
            ncurpattern,	/* no de pattern en cour de jeu                 */
            nncurpattern,	/* la prochaine pattern a jouer selon les boutons */
	    nbrep,		/* le nombre de répétitions avant passage au prochain pattern */
	    nextpat;		/* la pattern suivante au normal des choses */
    Radio   radiopattern;	/* le bloc radiobouton de pattern               */
    Radio   radiobank;		/* le bloc radiobouton de bank                  */
}
Patbut;

_sequencec
Patbut *gpatbut[MAXVOICES];	/* les pointeurs sur les structures utilisees   */

_sequencec
int     isreplaying,		/* sommes-nous en train de rejouer un enregistrement */
        hplay;			/* handle du bouton play(/pause JDJDJDJD)           */

int     simple_pression (int evtype, int voice, int data);
int     pattern_pression (int evtype, int voice, int data);
void    initPatbut (Patbut *pat, int ncurpattern, int tp, int curnbrow);
void    refrsyncPatbut (Patbut *pat, int nncurpattern);
int     createpatbut (Patbut *pat, int voice, int ligne, int evtype);
int     generate_freeze (void);
int     start_recording (void);
int     play_event (Event *ev);	/* JDJDJDJD private ? */
void    tickchange_play (void);
int     start_playing (void);
void    init_sequence (void);

#endif
