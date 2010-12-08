/* 
 *  les fonctions de gestion de controles
 *
 * $Id: controls.h,v 1.2 2003/03/19 16:45:47 jd Exp $
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
 *
 */

#ifndef JDCONTROLSH
#define JDCONTROLSH

#define MAXCONTEXTS	16	/* nombre maxi de contexts                */
#define MAXSCREENWIDTH  160	/* largeur maxi en caracteres             */
#define MAXSCREENHEIGHT 80	/* hauteur maxi                           */
#define MAXNBCONTROLS   484	/* nombre max de controls         */

#define MAXMETAMEMBER	64	/* nombre maxi de metahandlers            */

/* liste des types de controls */

#define CONTUNDEF	0	/* pour verification: controle indefini */
#define CONTBGND	1	/* pour verification: fond d'ecran      */
				/* ne pas modifier l'ordre jusque la <= */
#define CONTRHSLIDE	2	/* slide horizontal                       */
#define CONTRNUMRANGE	3	/* numeric range                  */
#define CONTRPFIELD 	4	/* pattern field                  */
#define CONTRBUTTON	5	/* bouton pression                        */
#define CONTRRADIO	6	/* bouton radio                           */
#define CONTRVSLIDE	7	/* slide vertical                 */
#define CONTRGROUP	8	/* groupe de controls                     */
#define CONTLISTBOX	9	/* list-box                               */
#define CONTTEXTSTR	10	/* text string                            */
#define CONTTEXTEDIT	11	/* editable text                  */
#define CONTNOTEEDIT	12	/* editable musical score         */

/* liste des etats de bouttons */

#define NBBUTSTR	8	/* nombre d'etat de boutons (pour allocation) */

#define BUT_OFF		0	/* eteint                         */
#define BUT_ON		1	/* allume                         */
#define BUT_DIS		2	/* innoperant                             */
#define BUT_INV		3	/* invisible                              */
#define BUT_ISFOC	4	/* additif de focus                       */

/* liste des couleurs de controles */

#define CONTNOCOL	16	/* JDJDJDJD use to be -1, beware !        */
#define CONTBLACK	0
#define CONTRED		1
#define CONTGREEN	2
#define CONTYELLOW	3
#define CONTBLUE	4
#define CONTMAGENTA	5	/* beurk ! */
#define CONTCYAN	6	/* that's not the worse ~~~ */
#define CONTWHITE	7
#define CONTNOPUCE	32	/* no "puce" in front of buttons  */

/* special keys binding */

#define CONTKEY_BACK	0x7f
#define CONTKEY_RETURN	0xd
#define CONTKEY_SUPR	0x17f

#define CONTKEY_F5	0x101
#define CONTKEY_F6	0x102
#define CONTKEY_F7	0x103
#define CONTKEY_F8	0x104
#define CONTKEY_INS	0x105
#define CONTKEY_F9	0x106
#define CONTKEY_F10	0x107
#define CONTKEY_F11	0x108
#define CONTKEY_F12	0x109
#define CONTKEY_PAGU	0x10a
#define CONTKEY_PAGD	0x10b
#define CONTKEY_UARR	0x10c
#define CONTKEY_DARR	0x10d
#define CONTKEY_RARR	0x10e
#define CONTKEY_LARR	0x10f
#define CONTKEY_END	0x110
#define CONTKEY_HOME	0x111
#define CONTKEY_F1	0x112
#define CONTKEY_F2	0x113
#define CONTKEY_F3	0x114
#define CONTKEY_F4	0x115
#define MAXKEYHANDLER   0x116	/* this one is the latest plus one of course */

#define MAXBACKSTR	64	/* nombre max de chaine de background (pour allocation) */
/* JDJDJD #define CONTMAXRADIO  32 */
/* #define CONTMAXRADIO 64 */
/* nombre max de controls dans une struct Radio (pour allocation) */
				/* se trouve dans radio.h */
#define CONTEXTNAMESIZE	8	/* taille d'u nom de context pour alloc */

#include "radio.h"

typedef int (*Tactionne) (int key);	/* le pointeur d'actionnement d'un contexts */
typedef int (*Tpression) (int evtype, int voice, int data);	/* le pointeur de pression
								   d'un controls */
extern int curcontext;

void    scrflush (void);
void    pdebug (int n, char *s);
void    settitle (char *title1, char *title2);
void    refreshscreen (void);
void    clearscreen (void);
int     changefocus (int newfocus);
int     initcontrols (void);
int     resetcontrols (void);
int     refreshcontrols (int h);	/* rafraichit l'affichage d'un controls */
int     createcontrols (int type, int x, int y, int c, int assoc, void *data);	/* cree un

										   controls */
int     keycontrols (int h, int c);	/* returns 0 if the char is NOT handled */
int	setpression (int h, Tpression pression, int type, int voice);

int     reassigncontrols (int h, void *data);
int     reassoccontrols (int h, int assoc);
int     is_control_modified (int h);	/* savoir le nombre de modifs subies */
int     clearmodif (int h);	/* effacer le nombre de modifs subies */
int     creategroup (int type, int c, int assoc, int nb, int *listeh);
int     createtextedit (int type, int x, int y, int c, int assoc, int larg, size_t len, void *data, int defaulth);
int     reassigntextedit (int h, size_t len, void *data, int defaulth);
int     createlistbox (int type, int x, int y, int c, int assoc, int larg, int haut, void *data, int nbelem, char **list);
int     reassignlistbox (int h, int nbelem, char **list);
int     createtextstr (int type, int x, int y, int color, int len, char *text);
int     relooktextstr (int h, int color, char *text);
int     createbutton (int type, int x, int y, int c, int assoc, void *data, int size, char *off, char *on, char *disabled, char *off_foc, char *on_foc, char *dis_foc);
int     createsimplebutton (int type, int x, int y, int c, int color, int assoc, void *data, char *content);
int     relooksimplebutton (int h, int c, int color, char *content);
int     zeroradio (Radio *p);
int     createrange (int type, int x, int y, int c, int assoc, void *data, int minv, int maxv);
int     refreshpfield (int h, int nbrow);
int     createpfield (int type, int x, int y, int c, int assoc, void *data, int maxrow, int nbrow, Radio *radio);
int     createscore (int type, int x, int y, int c, int assoc, void *data, int maxrow, int nbrow, int nbligne, Radio *rdata);
int     addstrback (char *s);	/* add a string in the list of background display */
int     deletecontrols (int h);
void    pollmouse (void);
int     pollcontrols (int milliwait);

int     createcontexts (int x, int y, int lx, int ly, int col, char *name, Tactionne actionne);

		/* creer un contexts de ss fenetre */
void    actionnecontexts (int h, Tactionne actionne);

// int registertocontexts (int ctx, int ctl);                   /* ajouter un controls au
// contexts */
/* l'enregistrement est desormais automatique */
int     installcontexts_ici (int h, int x, int y);	/* l'installer precisement */
int     installcontexts (int h);	/* l'installer */
int     desinstallcontexts (int h);	/* le desinstaller */

#ifdef SIMPLE_MEMPROFILE
void info_mem_controls (void);		/* donne une vague idee de la memoire en terme de controls */
#endif

#ifndef MAINCONTR
extern int usetimer, wasmodified;

#endif
#endif /* JDCONTROLSH */
