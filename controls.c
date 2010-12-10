/* 
 * $Id: controls.c,v 1.2 2003/03/19 16:45:47 jd Exp $
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
 * $Log: controls.c,v $
 * Revision 1.2  2003/03/19 16:45:47  jd
 * major changes everywhere while retrieving source history up to this almost final release
 *
 * Revision 1.2.0.24.0.3  2000/10/01 21:07:03  jd
 * mhhh I dunno
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
 * Revision 1.2.0.23.0.5  1999/10/11 15:05:55  jd
 * added jmeta, first try
 * corrected F-Keys with console
 *
 * Revision 1.2.0.23.0.3  1999/10/05 23:04:46  jd
 * added member nbmodif and exported function for it
 *
 * Revision 1.2.0.23  1999/09/15 10:20:39  jd
 * second pre tel-aviv public revision, for testing
 *
 * Revision 1.2.0.22.0.6  1999/09/15 09:23:30  jd
 * corrected bug with undefined hslides
 * added positions relative to contexts and moveable contexts
 * corrected CONTNOCOL bug with buttons
 * corrected bug in range of polltime
 *
 * Revision 1.2.0.22.0.4  1999/09/05 22:30:16  jd
 * corrected curses inclusion
 * corrected (one more time ?) handling of back and suppr
 *
 * Revision 1.2.0.22.0.3  1999/08/30 22:16:03  jd
 * added transmission of lastpressed to context's actionnes
 * corrected vslide defaults colors
 *
 * Revision 1.2.0.22.0.2  1999/08/29 16:58:26  jd
 * added dialogs
 *
 */

// #define MEGASTATIC
#define SIMPLE_MEMPROFILE

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "config.h"

#ifdef USEERRNOH
#include <errno.h>
#endif

/* JDJDJDJD this was for gpm excluding gpm-xterm #ifdef HAVE_LIBGPM #include <gpm.h> #else
   #include "gpm-xterm.h" #endif */
#ifdef HAVE_LIBGPM
#include "gpm.h"		/* JDJDJDJD devrait etre dans la distrib... */
#endif
#include "gpm-xterm.h"

#ifdef HAVENCURSES
#include <ncurses.h>
#else
#ifdef HAVECURSES
#include <curses.h>
#else
/* maybe curses.h is somewhere we don't know but cc knows ?? */
#include <curses.h>
#endif
#endif

#include <sys/time.h>

#include "semaphore.h"

#define MAINCONTR
#include "controls.h"

static int lastpressed = -1;	/* le dernier controls utilise  */
int     usetimer = 1;		/* devons-nous utiliser le timer ? */
int     usexterm = 0;		/* sommes-nous dans un xterm ?  */
static int uselibgpm = 0;	/* utilisons-nous libgpm ??? */
int     (*Pgpm_getc) (FILE * f) = NULL;		/* pointeur sur la fonction de lecture de
						   char */
int     wasmodified = 0;	/* un champ pattern modifie ??  */

int     focus = 0;

typedef union tcontain		/* data-container for controls  */
{
    struct
    {
	int     minv,		/* valeur mini ...              */
	        maxv;		/* ... et maxi                  */
    }
    mm;				/* mnemo: mm = "MinMax"         */

    struct
    {
	int     nbrow,		/* nombre courant de colonnes   */
	        maxrow,		/* nombre max de colonnes       */
	        radio;		/* radio handle (-1 if not)     */
	Radio  *rdata;		/* radio pointer                */
    }
    pf;				/* mnemo: pf = "PatternField"   */

    struct
    {
	int     nbrow,		/* nombre courant de colonnes   */
	        maxrow,		/* nombre max de colonnes       */
	        nbligne,	/* nombre de ligne              */
	        radio;		/* radio handle (-1 if not)     */
	Radio  *rdata;		/* radio pointer                */
    }
    es;				/* mnemo: es = "ScoreEdit"      */

    struct
    {
	char   *s[NBBUTSTR];	/* strings for button/state     */
	int     radio;		/* radio handle (-1 if not)     */
    }
    bs;				/* mnemo: bs = "ButtonStrings"  */

    struct
    {
	int     nb,		/* number of handles of group   */
	       *h;		/* address of first handle      */
    }
    gr;				/* mnemo: gr = "GRoup"          */

    struct
    {
	char  **list;		/* pointeur sur tableau de nom d'items */
	int     larg,		/* largeur de la listbox        */
	        haut,		/* hauteur de la listbox        */
	        nbelem,		/* nombre d'elements dans la liste */
	        curoff,		/* offset actuel dans la liste  */
	        cursur;		/* celui actuellement surligne  */
    }
    lb;				/* mnemo: lb = "ListBox"        */

    struct
    {
	int     larg,		/* largeur de la fenetre d'edition */
	        offset,		/* offset actuel du texte       */
	        curpos,		/* position actuelle du curseur */
	        curlen,		/* longueur actuelle du buffer  */
	        len,		/* longueur max du buffer       */
	        defaulth;	/* handle du controls de default action */
    }
    te;				/* mnemo te = "TextEdit"        */

    struct
    {
	int     len,		/* longueur de la chaine texte  */
	        color;		/* la couleur de ladite...      */
	char   *text;		/* la chaine de texte           */
    }
    ts;				/* mnemo ts = "Text String"     */
}
Contain;

typedef struct tcontrol		/* structure de gestion de zone reactives */
{
    int     type;		/* nature du controle */
    int     x, y;		/* coordonees en haut a gauche du controle */
    int     o;			/* handle du contexte proprietaire */
    int     hjmeta;		/* no de jmeta handle qui controle */
    int     toberefreshed;	/* le controls doit-il etre rafraichi ? */
    int     nbmodif;		/* le nombre de modifs subies */
    int     isfreezed;		/* le controle est-il gele par un autre contexte */
    int     associated;		/* handle d'un eventuel controls associe */
    char    c;			/* raccourci clavier ??? JDJDJD */
    Tpression pression;		/* pointeur sur l'action a effectuer en cas de pression */
    int     evtype;		/* type d'evenement */
    int     voice;		/* la voie affectée */
    void   *p;			/* valeur maintenue par le controle */
    Contain d;			/* data contained... */
}
Controls;

static Controls controls[MAXNBCONTROLS];	/* les controls proprement dits */
static short toberefreshed[MAXNBCONTROLS],	/* les controles a rafraichir au prochain
						   polling */
        nbtoberefreshed = 0,	/* combien on doit en rafraichir */
        refreshing = 0;		/* somme nous en phase de refresh ? */

/* --------------------------variables de contexts---------------------------------------- */

#define CONTCTXFREE	0
#define CONTCTXUSED	1

typedef struct tcontext		/* la structure de context */
{
    int     type,		/* type de context / utilisation */
            isinstalled,	/* le context est installe ? */
            x,			/* coordonnee en haut a gauche de la sous fenetre */
            y,			/* coordonnee en haut a gauche de la sous fenetre */
            lx,			/* largeur */
            ly,			/* hauteur */
            color,		/* background color */
            parent,		/* context parent */
  /* nbconttofreeze,      *//* nombre de controls a geler */
            nbconttoinstall;	/* nombre de controls a installer */
  /* quel controls en xy pour ce context ? */
    Tactionne actionne;		/* pointeur sur l'actionnement du contexts */

  /* tofreeze [MAXNBCONTROLS], *//* les controls a geler */
#ifdef MEGASTATIC
    int     whichcontrols[MAXSCREENHEIGHT][MAXSCREENWIDTH];
#else
    int    *whichcontrols;
#endif

    int     toinstall[MAXNBCONTROLS];	/* les controls a installer */
    char    name[CONTEXTNAMESIZE + 1];
}
Contexts;

Contexts contexts[MAXCONTEXTS];
int     curcontext = -1,	/* identifiant du context en cours ??? */
        nbctx = -1;		/* nombre de contextes deja installes ??? */

static int contextfilo[MAXCONTEXTS],	/* empilement des contextes */
        freezedlist[MAXCONTEXTS][MAXNBCONTROLS],	/* controles geles au nieme contexte */
        nbcontfreez[MAXCONTEXTS];	/* nombre de contexte gele */

								/* contextes de clicks */
static int tabwhichcontrols[MAXCONTEXTS][MAXSCREENHEIGHT][MAXSCREENWIDTH],	/* quel
										   controls
										   en xy ? */
        (*whichcontrols)[MAXSCREENHEIGHT][MAXSCREENWIDTH] = &(tabwhichcontrols[0]);
static int keyhandler[MAXKEYHANDLER];	/* quel controls pour la touche z ? */
static char *backstr[MAXBACKSTR];	/* les chaines de char de background */
static int nbbackstr = 0;	/* nombre de chaine de background */

typedef struct tjmeta		/* la structure de jmeta controls    */
{
    int     nb;			/* le nombre de membres             */
    int     member[MAXMETAMEMBER];	/* les membres                      */
}
JMeta;

JMeta   jmeta[8];		/* les handles des jmeta            */

#ifdef SIMPLE_MEMPROFILE
static long totdivs = 0;
#endif

#ifdef SHOWRCS
static char *rcsid = "$Id: controls.c,v 1.2 2003/03/19 16:45:47 jd Exp $";

#endif
static int firstuse = 1;

#define MAXHEADROOM 42
static int scorelook[MAXHEADROOM] =
{0, 0, 0, 0, 0,			/* low note from C      */
 1, 0,				/* low  G on F'score    */
 1, 0,				/* B            */
 1, 0,				/* D            */
 1, 0,				/* F            */
 1, 0,				/* A            */
 0, 0, 0,			/* empty's B,C,D        */
 1, 0,				/* med  E on G'score    */
 1, 0,				/* G            */
 1, 0,				/* G            */
 1, 0,				/* B            */
 1, 0,				/* D            */
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	/* something else to say ??? */
};

/* --------------------------variables de polling----------------------------------------- */

static int fmouse;		/* mouse events file handle for polling */
static Gpm_Event gevent;	/* for mouse coord */

#ifdef JDSEMAPHORE
static int semid;

#else
static int pollreintrance = 0;	/* test de reentrance du polling */

#endif
static fd_set readfdset;	/* pour le polling */

/* --------------------------------------------------------------------------------------- */

char   *bgcol[8][2] =
{
    {"\033[40m", "\033[40m\033[36m"},
    {"\033[41m", "\033[41m\033[36m"},
    {"\033[42m", "\033[42m\033[36m"},
    {"\033[43m", "\033[43m\033[36m"},
    {"\033[44m", "\033[44m\033[36m"},
    {"\033[45m", "\033[45m\033[36m"},
    {"\033[46m", "\033[46m\033[36m"},
    {"\033[47m", "\033[47m\033[36m"},
};

char   *strlevel[10] =
{
    "\033[1m|-------\033[m",
    "\033[1m-|------\033[m",
    "\033[1m--|-----\033[m",
    "\033[1m---|----\033[m",
    "\033[1m----|---\033[m",
    "\033[1m-----|--\033[m",
    "\033[1m------|-\033[m",
    "\033[1m-------|\033[m",
    "\033[1m------->\033[m",
    "\033[m        "
},     *strhlevel[10][2] =
{
    {"\033[m\016 \033[B\010 \033[B\010 \033[B\010s\017",
     "\033[1m\033[36m\016 \033[B\010 \033[B\010 \033[B\010s\017"},
    {"\033[m\016 \033[B\010 \033[B\010 \033[B\010w\017",
     "\033[1m\033[36m\016 \033[B\010 \033[B\010 \033[B\010w\017"},
    {"\033[m\016 \033[B\010 \033[B\010s\033[B\010x\017",
     "\033[1m\033[36m\016 \033[B\010 \033[B\010s\033[B\010x\017"},
    {"\033[m\016 \033[B\010 \033[B\010w\033[B\010x\017",
     "\033[1m\033[36m\016 \033[B\010 \033[B\010w\033[B\010x\017"},
    {"\033[m\016 \033[B\010s\033[B\010x\033[B\010x\017",
     "\033[1m\033[36m\016 \033[B\010s\033[B\010x\033[B\010x\017"},
    {"\033[m\016 \033[B\010w\033[B\010x\033[B\010x\017",
     "\033[1m\033[36m\016 \033[B\010w\033[B\010x\033[B\010x\017"},
    {"\033[m\016s\033[B\010x\033[B\010x\033[B\010x\017",
     "\033[1m\033[36m\016s\033[B\010x\033[B\010x\033[B\010x\017"},
    {"\033[m\016w\033[B\010x\033[B\010x\033[B\010x\017",
     "\033[1m\033[36m\016w\033[B\010x\033[B\010x\033[B\010x\017"},
    {"\033[m\016x\033[B\010x\033[B\010x\033[B\010x\017",
     "\033[1m\033[36m\016x\033[B\010x\033[B\010x\033[B\010x\017"},
    {"\033[m \033[B\010 \033[B\010 \033[B\010 ",
     "\033[1m\033[36m \033[B\010 \033[B\010 \033[B\010 "}
},     *strfocus[2] =
{"", "\033[1m"};

/* 
 *  small debug to screen without scrolling....
 */

void    scrflush (void);	/* small anti-declaration for coherence */
int     registertocontexts (int ctx, int ctl);	/* another one ... JDJDJDJD */
void    pdebug (int n, char *s)
{
    if (usexterm)
	printf ("\033]2; %04x %-60s\007", n, s);
#ifdef DEBUGINCONSOLE
    else
	printf ("\033[%d;%dH\033[1m\033[40m\033[33m %04x \033[36m%-60s", 26 + MAXSAMPLE - 16, 1, n, s);
#endif
    scrflush ();
}

inline int controlsx (h)
{
    return controls[h].x + contexts[controls[h].o].x;
}
inline int controlsy (h)
{
    return controls[h].y + contexts[controls[h].o].y;
}

void    settitle (char *title1, char *title2)
{
    if (usexterm)
	printf ("\033]2;%s - %s\007", title1, title2);
}

int     refreshcontrols (int h)	/* rafraichit l'affichage d'un controls */
{
    if ((refreshing) && (!controls[h].isfreezed))
    {
	controls[h].toberefreshed = 0;
	switch (controls[h].type)
	{
	    case CONTRHSLIDE:	/* slide horizontal - refreshcontrols */
		if (controls[h].p == NULL)
		    printf ("\033[%d;%dH%s%s", controlsy (h), controlsx (h),
			    bgcol[controls[h].hjmeta][focus == h],
			    strlevel[9]);
		else
		    printf ("\033[%d;%dH%s%s", controlsy (h), controlsx (h),
			    bgcol[controls[h].hjmeta][focus == h],
			    strlevel[(*(int *) controls[h].p) >> 5]);
		break;

	    case CONTRVSLIDE:	/* slide vertical - refreshcontrols */
		if (controls[h].p == NULL)
		    printf ("\033[%d;%dH%s", controlsy (h), controlsx (h),
			    strhlevel[9][focus == h]);
		else
		    printf ("\033[%d;%dH%s", controlsy (h), controlsx (h),
			    strhlevel[(*(int *) controls[h].p) >> 5][focus == h]);
		break;

	    case CONTRNUMRANGE:	/* numerical range - refreshcontrols */
		if (controls[h].p == NULL)
		    printf ("\033[%d;%dH%s«\033[41m\033[33m\033[1m---\033[m%s»\033[m", controlsy (h), controlsx (h),
			    strfocus[focus == h], strfocus[focus == h]);
		else
		    printf ("\033[%d;%dH%s«\033[41m\033[33m\033[1m%3d\033[m%s»\033[m", controlsy (h), controlsx (h),
			    strfocus[focus == h],
			    *(int *) controls[h].p, strfocus[focus == h]);
		break;

	    case CONTRPFIELD:	/* pattern row - refreshcontrols */
		{
		    int     i, *p;

		    if (controls[h].p == NULL)
		    {
			printf ("\033[%d;%dH%s", controlsy (h), controlsx (h), strfocus[focus == h]);
			for (i = 0; i < controls[h].d.pf.maxrow; i++)
			    printf (" ");
			printf ("\033[m");
		    }
		    else
		    {
			p = controls[h].p;
/* printf ("\033[%d;%dH\016%s", controls[h].y, controls[h].x, strfocus[focus == h]); */
			printf ("\033[%d;%dH\016%s", controlsy (h), controlsx (h),
				(controls[h].d.pf.rdata != NULL) ?
			   strfocus[controls[h].d.pf.rdata->value == controls[h].d.pf.radio]
				: strfocus[focus == h]);
			for (i = 0; i < controls[h].d.pf.maxrow; i++)
			    printf ("%s%c",
				    (i < controls[h].d.pf.nbrow) ?
				    ((i % 4) ? "\033[37m" : "\033[33m") :
				    "\033[34m",
				    *p++ ? '`' : '·');
			printf ("\017\033[m");
		    }
		}
		break;

	    case CONTNOTEEDIT:	/* editable score - refreshcontrols */
		{
		    int     i, j, k, *p;

		    if (controls[h].p == NULL)
		    {
			for (j = 0; j < controls[h].d.es.nbligne; j++)
			{
			    printf ("\033[%d;%dH%s", controlsy (h) + j, controlsx (h), strfocus[0]);
			    for (i = 0; i < controls[h].d.pf.maxrow; i++)
				printf ("+");	/* JDJDJDJD juste pour tester */
			}
			printf ("\033[m");
		    }
		    else
		    {
			k = controls[h].d.es.nbligne;
			for (j = 0; j < controls[h].d.es.nbligne; j++, k--)
			{
			    printf ("\033[%d;%dH%s", controlsy (h) + j, controlsx (h), strfocus[0]);
			    p = controls[h].p;
			    for (i = 0; i < controls[h].d.pf.maxrow; i++)
				printf ("%s",
					(((*p) >> 1) % 30) == j ?
					"\033[33mO" :
					scorelook[k] ?
					"\033[m-" : "\033[m "
				    );
			}
			printf ("\033[m");
		    }
		}
		break;

	    case CONTRBUTTON:	/* button - refreshcontrols */
		{
		    int    *p = controls[h].p;

		    if ((p == NULL) || (*p < 0) || (*p > NBBUTSTR))
			printf ("\033[%d;%dH%s", controlsy (h), controlsx (h), controls[h].d.bs.s[BUT_DIS + ((focus == h) ? BUT_ISFOC : 0)]);
		    else
			printf ("\033[%d;%dH%s", controlsy (h), controlsx (h),
				controls[h].d.bs.s[*p + ((focus == h) ? BUT_ISFOC : 0)]);
		}
		break;

	    case CONTRRADIO:	/* radio button - refreshcontrols */
		{
		    Radio  *p = controls[h].p;

		    if ((p == NULL) || (p->value > p->nb))
			printf ("\033[%d;%dH%s", controlsy (h), controlsx (h), controls[h].d.bs.
				s[BUT_DIS + ((focus == h) ? BUT_ISFOC : 0)]);
		    else
			printf ("\033[%d;%dH%s", controlsy (h), controlsx (h),
				controls[h].d.bs.s[((p->value == controls[h].d.bs.radio) ? BUT_ON : BUT_OFF) + ((focus == h) ? BUT_ISFOC : 0)]);
		}
		break;

	    case CONTRGROUP:	/* groupe de controls - refreshcontrols */
		{
		    int     i;

		    for (i = 0; i < controls[h].d.gr.nb; i++)
			refreshcontrols (controls[h].d.gr.h[i]);
		}
		break;

	    case CONTBGND:	/* le fond d'ecran - refreshcontrols */
		{
		    int     i;

		    printf ("\033[2m");
		    for (i = 0; i < nbbackstr; i++)
			printf ("%s", backstr[i]);
		}
		break;

	    case CONTLISTBOX:	/* list box... - refreshcontrols */
		{
		    int     i, *p = (int *) controls[h].p, curoff = controls[h].d.lb.curoff;
		    char    format[28] = "\033[%d;%dH%s%s%-10.10s    ";

		    sprintf (format, "\033[%%d;%%dH%%s%%s%%-%d.%ds", controls[h].d.lb.larg, controls[h].d.lb.larg);

		    for (i = 0; i < controls[h].d.lb.haut; i++)
		    {
			printf (format,
				controlsy (h) + i,
				controlsx (h),
				(focus == h) ? "\033[1m" : "\033[0m",
				((i + curoff) == *p) ? "\033[41m\033[33m" :
				((i + curoff) == controls[h].d.lb.cursur) ? "\033[46m\033[37m" : "\033[44m\033[37m",
				((i + curoff) < controls[h].d.lb.nbelem) ?
				controls[h].d.lb.list[i + curoff] :
				""
			    );
			if (i == 0)
			    printf ("\033[1m\033[37m\033[40m^\033[m");
		    }
		    printf ("\033[1m\033[37m\033[40mv\033[m");
		}
		break;

	    case CONTTEXTSTR:	/* text string - refreshcontrols */
		printf ("\033[%d;%dH\033[4%cm\033[3%cm%s", controlsy (h), controlsx (h),
			'0' + contexts[controls[h].o].color,
			'0' + controls[h].d.ts.color,
			controls[h].d.ts.text);
		break;

	    case CONTTEXTEDIT:	/* editable text - refreshcontrols */
		{
		    int     i, t;
		    char   *p = (char *) controls[h].p + controls[h].d.te.offset;

		    printf ("\033[%d;%dH%s", controlsy (h), controlsx (h), (focus == h) ? "\033[1m" : "\033[0m");
		    for (i = 0, t = controls[h].d.te.offset; i < controls[h].d.te.larg; i++, t++, p++)
			printf ("%s%c",
				(t == controls[h].d.te.curpos) ? "\033[41m\033[33m" : "\033[44m\033[37m",
				(t < controls[h].d.te.curlen) ? *p : ' ');
		    printf ("\033[m");
		}
		break;

	    default:
		return -1;
	}
	return 0;
    }
    else
    {
	if (!controls[h].toberefreshed)
	{
	    toberefreshed[nbtoberefreshed++] = h;
	    controls[h].toberefreshed++;
	}

	return 0;
    }
}

void    refreshscreen (void)
{
    int     i;

    for (i = 0; i < MAXNBCONTROLS; i++)
	if (controls[i].type != CONTUNDEF)
	    refreshcontrols (i);
}

void    clearscreen (void)
{
    printf ("\033[H\033[2J");	/* on efface l'ecran */
    refreshscreen ();
}

int     refreshpfield (int h, int nbrow)	/* rafraichit l'affichage d'un pattern field */
{
    controls[h].d.pf.nbrow = nbrow;
    return (refreshcontrols (h));
}

int     changefocus (int newfocus)
{
    int     oldfocus = focus;

    if (newfocus != focus)
    {
	focus = newfocus;
	if (oldfocus != 0)
	    refreshcontrols (oldfocus);
	if (focus != 0)
	    refreshcontrols (focus);
	return 1;
    }
    else
	return 0;
}

void    clearjmeta (int m)
{
    jmeta[m].nb = 0;
}
void    initjmeta (void)
{
    int     i;

    for (i = 0; i < 8; i++)
	clearjmeta (i);
    keyhandler[CONTKEY_F1] = 0;
    keyhandler[CONTKEY_F2] = 0;
    keyhandler[CONTKEY_F3] = 0;
    keyhandler[CONTKEY_F4] = 0;
    keyhandler[CONTKEY_F5] = 0;
    keyhandler[CONTKEY_F6] = 0;
    keyhandler[CONTKEY_F7] = 0;
    keyhandler[CONTKEY_F8] = 0;
    keyhandler[CONTKEY_F9] = 0;
    keyhandler[CONTKEY_F10] = 0;
    keyhandler[CONTKEY_F11] = 0;
    keyhandler[CONTKEY_F12] = 0;
}
void    removejmeta (int m, int h)
{
    int     i;

    if (m == 0)
    {
	refreshcontrols (h);
	return;
    }
    for (i = 0; i < jmeta[m].nb; i++)
	if (jmeta[m].member[i] == h)
	{
	    jmeta[m].member[i] = jmeta[m].member[jmeta[m].nb];
	    jmeta[m].nb--;
	    controls[h].hjmeta = 0;
	    refreshcontrols (h);
	    break;
	}
}
void    addjmeta (int m, int h)
{
    if (controls[h].hjmeta == m)
	return;

    if (m == 0)
    {
	controls[h].hjmeta = m;
	refreshcontrols (h);
	return;
    }

    if (jmeta[m].nb >= MAXMETAMEMBER)
	removejmeta (m, jmeta[m].member[jmeta[m].nb - 1]);

    jmeta[m].member[jmeta[m].nb] = h;
    controls[h].hjmeta = m;
    jmeta[m].nb++;
    refreshcontrols (h);
}
void    actionnejmeta (int m, int c)
{
    int     i;

    for (i = 0; i < jmeta[m].nb; i++)
	keycontrols (jmeta[m].member[i], c);
}

void    actionnedirectjmeta (int m, int val)
{
    int     i;

    for (i = 0; i < jmeta[m].nb; i++)
	keydirectcontrols (jmeta[m].member[i], val);
}

int     keydirectcontrols (int h, int val)	/* returns 0 if the control cannot support direct */
{
    int    *p = (int *) controls[h].p;

    lastpressed = h;
    switch (controls[h].type)
    {
	case CONTRVSLIDE:	/* slide vertical - keycontrols */
	case CONTRHSLIDE:	/* slide horizontal - keycontrols */
	    if (p != NULL)
	    {
		if (val > 256)
		    *p = 256;
		else if (val < 0)
		    *p = 0;
		else
		    *p = val;

		controls[h].nbmodif++;
		if (controls[h].pression != NULL)
		    controls[h].pression (controls[h].evtype, controls[h].voice, *p);

		if (controls[h].associated != CONTUNDEF)
		    refreshcontrols (controls[h].associated);
		refreshcontrols (h);
		return (1);
	    }
	    break;
	default:
	    return 0;
    }

    return (0);
}

int     keycontrols (int h, int c)	/* returns 0 if the char is NOT handled */
{
    int    *p = (int *) controls[h].p;

    lastpressed = h;
    switch (controls[h].type)
    {
	case CONTRVSLIDE:	/* slide vertical - keycontrols */
	case CONTRHSLIDE:	/* slide horizontal - keycontrols */
	    if (p != NULL)
	    {
		if ((c >= '1') && (c <= '9'))
		    *p = (c - '1') << 5;
		else
		    switch (c)
		    {
			case 'M':
			    controls[h].nbmodif++;
			    *p = 256;	/* JDJDJDJD */
			    if (controls[h].pression != NULL)
				controls[h].pression (controls[h].evtype, controls[h].voice, *p);
			    break;
			case '-':	/* decrement */
			    if (*p != 0)
			    {
				controls[h].nbmodif++;
				(*p)--;
				if (controls[h].pression != NULL)
				    controls[h].pression (controls[h].evtype, controls[h].voice, *p);
			    }
			    break;
			case '+':	/* increment */
			    if (*p != 256)
			    {
				(*p)++;		/* JDJDJDJD */
				controls[h].nbmodif++;
				if (controls[h].pression != NULL)
				    controls[h].pression (controls[h].evtype, controls[h].voice, *p);
			    }
			    break;
			default:
			    return 0;
		    }
		if (controls[h].associated != CONTUNDEF)
		    refreshcontrols (controls[h].associated);
		refreshcontrols (h);
		return (1);
	    }
	    break;

	case CONTRNUMRANGE:	/* numerical range - keycontrols */
	    if (p != NULL)
	    {
		switch (c)
		{
		    case '-':	/* decrement */
			(*p)--;
			if (*p < controls[h].d.mm.minv)
			    *p = controls[h].d.mm.minv;
			else
			    controls[h].nbmodif++;
			if (controls[h].pression != NULL)
			    controls[h].pression (controls[h].evtype, controls[h].voice, *p);
			break;
		    case '+':	/* increment */
			(*p)++;
			if (*p > controls[h].d.mm.maxv)
			    *p = controls[h].d.mm.maxv;
			else
			    controls[h].nbmodif++;
			if (controls[h].pression != NULL)
			    controls[h].pression (controls[h].evtype, controls[h].voice, *p);
			break;
		    default:
			return (0);
		}
		if (controls[h].associated != CONTUNDEF)
		    refreshcontrols (controls[h].associated);
		refreshcontrols (h);
		return (1);
	    }
	    break;

	case CONTRBUTTON:	/* button - keycontrols */
	    if ((p != NULL) && ((c == controls[h].c) || (c == ' ')))
	    {
		switch (*p)
		{
		    case BUT_DIS:
		    case BUT_INV:
			return (0);
		    case BUT_ON:
			controls[h].nbmodif++;
			*p = BUT_OFF;
			if (controls[h].pression != NULL)
			    controls[h].pression (controls[h].evtype, controls[h].voice, *p);
			break;
		    case BUT_OFF:
			controls[h].nbmodif++;
			*(int *) controls[h].p = BUT_ON;
			if (controls[h].pression != NULL)
			    controls[h].pression (controls[h].evtype, controls[h].voice, *p);
			break;
		}
		if (controls[h].associated != CONTUNDEF)
		    refreshcontrols (controls[h].associated);
		refreshcontrols (h);
		return (1);
	    }
	    else
		return (0);

	case CONTRRADIO:	/* radio button - keycontrols */
	    {
		Radio  *p = (Radio *) controls[h].p;
		int     oldv;

		if ((p != NULL) && ((c == controls[h].c) || (c == ' ') || (c == 0xd)))
		{
		    oldv = p->value;
		    if (controls[h].d.bs.radio != oldv)
		    {
			controls[h].nbmodif++;
			p->value = controls[h].d.bs.radio;
			if (controls[h].pression != NULL)
			    controls[h].pression (controls[h].evtype, controls[h].voice, p->value);
			refreshcontrols (p->h[oldv]);
			refreshcontrols (h);
		    }
		    return (1);
		}
	    }
	    break;

	case CONTBGND:		/* fond d'ecran - keycontrols */
	    switch (c)
	    {
		case 0xC:	/* CTRL-L */
		    clearscreen ();
		    return (1);
		case CONTKEY_F1:
		    return (keycontrols (focus, '-'));
		case CONTKEY_F2:
		    return (keycontrols (focus, '+'));

		case CONTKEY_F3:
		    actionnejmeta (1, '-');
		    return 1;
		case CONTKEY_F4:
		    actionnejmeta (1, '+');
		    return 1;

		case CONTKEY_F5:
		    actionnejmeta (2, '-');
		    return 1;
		case CONTKEY_F6:
		    actionnejmeta (2, '+');
		    return 1;

		case CONTKEY_F7:
		    actionnejmeta (3, '-');
		    return 1;
		case CONTKEY_F8:
		    actionnejmeta (3, '+');
		    return 1;

		case CONTKEY_F9:
		    actionnejmeta (4, '-');
		    return 1;
		case CONTKEY_F10:
		    actionnejmeta (4, '+');
		    return 1;

		case CONTKEY_F11:
// fprintf (stderr, "F11\n");
		    actionnejmeta (5, '-');
		    return 1;
		case CONTKEY_F12:
// fprintf (stderr, "F12\n");
		    actionnejmeta (5, '+');
		    return 1;
	    }
	    return 0;

	case CONTLISTBOX:	/* list box - keycontrols */
	    switch (c)
	    {
		case CONTKEY_UARR:	/* one-up               */
		    if (controls[h].d.lb.cursur > 0)
		    {
			controls[h].nbmodif++;
			controls[h].d.lb.cursur--;
			if (controls[h].d.lb.cursur < controls[h].d.lb.curoff)
			    controls[h].d.lb.curoff = controls[h].d.lb.cursur;
			else if (controls[h].d.lb.cursur > controls[h].d.lb.curoff + controls[h].d.lb.haut - 1)
			    controls[h].d.lb.curoff = controls[h].d.lb.cursur - controls[h].d.lb.haut + 1;
			if (controls[h].pression != NULL)
			    controls[h].pression (controls[h].evtype, controls[h].voice, *p);
			refreshcontrols (h);
		    }
		    return 1;

		case CONTKEY_DARR:	/* one-down             */
		    if (controls[h].d.lb.cursur < controls[h].d.lb.nbelem - 1)
		    {
			controls[h].nbmodif++;
			controls[h].d.lb.cursur++;
			if (controls[h].d.lb.cursur < controls[h].d.lb.curoff)
			    controls[h].d.lb.curoff = controls[h].d.lb.cursur;
			else if (controls[h].d.lb.cursur > controls[h].d.lb.curoff + controls[h].d.lb.haut - 1)
			    controls[h].d.lb.curoff = controls[h].d.lb.cursur - controls[h].d.lb.haut + 1;

			if (controls[h].pression != NULL)
			    controls[h].pression (controls[h].evtype, controls[h].voice, *p);
			refreshcontrols (h);
		    }
		    return 1;

		case ' ':	/* space */
		case 0xd:	/* return */
		    controls[h].nbmodif++;
		    *(int *) controls[h].p = controls[h].d.lb.cursur;
		    if (controls[h].pression != NULL)
			controls[h].pression (controls[h].evtype, controls[h].voice, *p);
		    refreshcontrols (h);
		    return 1;
	    }
	    break;

	case CONTTEXTEDIT:	/* editable text - keycontrols */
	    switch (c)
	    {
		case CONTKEY_RETURN:	/* default action       */
		    if (controls[h].d.te.defaulth != CONTUNDEF)
		    {
			changefocus (controls[h].d.te.defaulth);
			return 1;
		    }
		    return 0;

		case CONTKEY_LARR:	/* going left           */
		    if (controls[h].d.te.curpos > 0)
		    {
			controls[h].d.te.curpos--;
			if (controls[h].d.te.curpos < controls[h].d.te.offset)
			    controls[h].d.te.offset--;
			refreshcontrols (h);
		    }
		    return 1;

		case CONTKEY_BACK:	/* deleting back        */
		    if (controls[h].d.te.curpos > 0)
		    {
			char   *ps = (char *) controls[h].p;

			controls[h].nbmodif++;
			memmove (ps + controls[h].d.te.curpos - 1,
				 ps + controls[h].d.te.curpos,
				 controls[h].d.te.curlen - controls[h].d.te.curpos + 2
			    );	/* JDJDJDJD use to be 1 */
			controls[h].d.te.curlen--;
			controls[h].d.te.curpos--;
			if (controls[h].d.te.curpos < controls[h].d.te.offset)
			    controls[h].d.te.offset--;
			if (controls[h].pression != NULL)
			    controls[h].pression (controls[h].evtype, controls[h].voice, *p);
			refreshcontrols (h);
		    }
		    return 1;

		case CONTKEY_SUPR:	/* deleting left        */
		    if (controls[h].d.te.curpos < controls[h].d.te.curlen)
		    {
			char   *ps = (char *) controls[h].p;

			controls[h].nbmodif++;
			memmove (ps + controls[h].d.te.curpos,
				 ps + controls[h].d.te.curpos + 1,
				 controls[h].d.te.curlen - controls[h].d.te.curpos + 1
			    );	/* JDJDJDJD use to be 0 */
			controls[h].d.te.curlen--;
			if (controls[h].pression != NULL)
			    controls[h].pression (controls[h].evtype, controls[h].voice, *p);
			refreshcontrols (h);
		    }
		    return 1;

		case CONTKEY_RARR:	/* going right          */
		    if (controls[h].d.te.curpos < controls[h].d.te.curlen)
		    {
			controls[h].d.te.curpos++;
			if (controls[h].d.te.curpos > controls[h].d.te.offset + controls[h].d.te.larg - 1)
			    controls[h].d.te.offset++;
			refreshcontrols (h);
		    }
		    return 1;

		case CONTKEY_HOME:	/* going home           */
		    if (controls[h].d.te.curpos != 0)
		    {
			controls[h].d.te.curpos = 0;
			controls[h].d.te.offset = 0;
			refreshcontrols (h);
		    }
		    return 1;

		case CONTKEY_END:	/* going end            */
		    if (controls[h].d.te.curpos != controls[h].d.te.curlen)
		    {
			controls[h].d.te.curpos = controls[h].d.te.curlen;
			if (controls[h].d.te.curlen >= controls[h].d.te.larg)
			    controls[h].d.te.offset = 1 + controls[h].d.te.curlen - controls[h].d.te.larg;
			else
			    controls[h].d.te.offset = 0;
			refreshcontrols (h);
		    }
		    return 1;

		default:	/* JDJDJDJD rajouter ici in controle de la validite des char */
		    if ((c > 31) && (c < 0x100) && (controls[h].d.te.curlen < controls[h].d.te.len))
		    {
			char   *ps = (char *) controls[h].p;

			controls[h].nbmodif++;
			memmove (ps + controls[h].d.te.curpos + 1,
				 ps + controls[h].d.te.curpos,
				 controls[h].d.te.curlen - controls[h].d.te.curpos + 1
			    );
			*(ps + controls[h].d.te.curpos) = c;
			controls[h].d.te.curlen++;
			controls[h].d.te.curpos++;
			if (controls[h].d.te.curpos > controls[h].d.te.offset + controls[h].d.te.larg - 1)
			    controls[h].d.te.offset++;
			if (controls[h].pression != NULL)
			    controls[h].pression (controls[h].evtype, controls[h].voice, *p);
			refreshcontrols (h);
		    }
		    return 1;
	    }
	    break;

	default:
	    return (0);
    }
    return (0);
}

int     clearmodif (int h)
{
    switch (controls[h].type)
    {
	case CONTRHSLIDE:	/* slide horizontal - clearmodif */
	case CONTRVSLIDE:	/* slide vertical - clearmodif */
	case CONTRNUMRANGE:	/* numerical range - clearmodif */
	case CONTRPFIELD:	/* pattern row - clearmodif */
	case CONTRBUTTON:	/* button - clearmodif */
	case CONTRRADIO:	/* radio button - clearmodif */
	case CONTLISTBOX:	/* list box - clearmodif */
	case CONTTEXTEDIT:	/* editable text - clearmodif */
	case CONTRGROUP:	/* groupe ??? - clearmodif */
	    controls[h].nbmodif = 0;
	    return controls[h].nbmodif;
	case CONTBGND:		/* fond d'ecran - clearmodif */
	case CONTUNDEF:
	default:
	    return -1;
    }
    return 0;
}

int     is_control_modified (int h)
{
    switch (controls[h].type)
    {
	case CONTRHSLIDE:	/* slide horizontal - is_control_modified */
	case CONTRVSLIDE:	/* slide vertical - is_control_modified */
	case CONTRNUMRANGE:	/* numerical range - is_control_modified */
	case CONTRPFIELD:	/* pattern row - is_control_modified */
	case CONTRBUTTON:	/* button - is_control_modified */
	case CONTRRADIO:	/* radio button - is_control_modified */
	case CONTLISTBOX:	/* list box - is_control_modified */
	case CONTTEXTEDIT:	/* editable text - is_control_modified */
	case CONTRGROUP:	/* groupe ??? - is_control_modified */
	    return controls[h].nbmodif;
	case CONTBGND:		/* fond d'ecran - is_control_modified */
	case CONTUNDEF:
	default:
	    return -1;
    }
    return 0;
}
int     clickcontrols (int h, int x, int y, int buttons, int focuschanged)
{
/* buttons: 4=left 2=middle 5=middle 1=right */
    int     xrev = x - controlsx (h);
    int     yrev = y - controlsy (h);

    lastpressed = h;
    switch (controls[h].type)
    {
	case CONTRHSLIDE:	/* slide horizontal - clickcontrols */
	    if (buttons == 1)
	    {
		int     sauveh = controls[h].hjmeta;

		removejmeta (sauveh, h);
		if (focuschanged && sauveh)
		    addjmeta (0, h);
		else
		    addjmeta ((sauveh + 1) % 8, h);
		break;
	    }
	    if (focuschanged && (buttons != 4))
		break;
	    if (controls[h].p != NULL)
	    {
		*(int *) controls[h].p = xrev << 5;
		controls[h].nbmodif++;
		if (controls[h].associated != CONTUNDEF)
		    refreshcontrols (controls[h].associated);
		if (controls[h].pression != NULL)
		    controls[h].pression (controls[h].evtype, controls[h].voice, *(int *) controls[h].p);
		refreshcontrols (h);
	    }
	    break;

	case CONTRVSLIDE:	/* slide vertical - clickcontrols */
	    if (focuschanged && (buttons != 4))
		break;
	    if (controls[h].p != NULL)
	    {
		controls[h].nbmodif++;
		if (*(int *) controls[h].p == (3 - yrev) << 6)
		    *(int *) controls[h].p = ((3 - yrev) << 6) + (1 << 5);
		else
		    *(int *) controls[h].p = (3 - yrev) << 6;
		if (controls[h].associated != CONTUNDEF)
		    refreshcontrols (controls[h].associated);
		if (controls[h].pression != NULL)
		    controls[h].pression (controls[h].evtype, controls[h].voice, *(int *) controls[h].p);
		refreshcontrols (h);
	    }
	    break;

	case CONTRNUMRANGE:	/* numerical range - clickcontrols */
	    if (controls[h].p != NULL)
	    {
		int    *p = controls[h].p;

		switch (xrev)
		{
		    case 0:	/* decrement */
			(*p)--;
			if (*p < controls[h].d.mm.minv)
			    *p = controls[h].d.mm.minv;
			else
			    controls[h].nbmodif++;
			if (controls[h].associated != CONTUNDEF)
			    refreshcontrols (controls[h].associated);
			if (controls[h].pression != NULL)
			    controls[h].pression (controls[h].evtype, controls[h].voice, *p);
			refreshcontrols (h);
			break;
		    case 4:	/* increment */
			(*p)++;
			if (*p > controls[h].d.mm.maxv)
			    *p = controls[h].d.mm.maxv;
			else
			    controls[h].nbmodif++;
			if (controls[h].associated != CONTUNDEF)
			    refreshcontrols (controls[h].associated);
			if (controls[h].pression != NULL)
			    controls[h].pression (controls[h].evtype, controls[h].voice, *p);
			refreshcontrols (h);
			break;
		    default:
			break;
		}
	    }
	    break;

	case CONTRPFIELD:	/* pattern row - clickcontrols */
	    if (controls[h].p != NULL)
	    {
		int    *p = controls[h].p;

		if ((buttons == 4) || !focuschanged)
		{
		    wasmodified++;
		    p[xrev] = p[xrev] ? 0 : 128;
		    controls[h].nbmodif++;
		}
		if (controls[h].associated != CONTUNDEF)
		{
		    if (controls[h].associated == CONTRGROUP)
			refreshcontrols (controls[controls[h].associated].d.gr.h[xrev]);
		    else
			refreshcontrols (controls[h].associated);
		}
		if (controls[h].pression != NULL)
		    controls[h].pression (controls[h].evtype, controls[h].voice, *p);
		refreshcontrols (h);
	    }
	    {
		Radio  *p = controls[h].d.pf.rdata;
		int     oldv;

		if (p != NULL)
		{
		    oldv = p->value;
		    if (controls[h].d.pf.radio != oldv)
		    {
			controls[h].nbmodif++;
			p->value = controls[h].d.pf.radio;
			if (controls[h].pression != NULL)
			    controls[h].pression (controls[h].evtype, controls[h].voice, p->value);
			refreshcontrols (h);
			refreshcontrols (p->h[oldv]);
		    }
		}
	    }
	    break;

	case CONTRBUTTON:	/* button - clickcontrols */
	    if (controls[h].p != NULL)
	    {
		switch (*(int *) controls[h].p)
		{
		    case BUT_DIS:
		    case BUT_INV:
			break;
		    case BUT_ON:
			controls[h].nbmodif++;
			*(int *) controls[h].p = BUT_OFF;
			if (controls[h].associated != CONTUNDEF)
			    refreshcontrols (controls[h].associated);
			if (controls[h].pression != NULL)
			    controls[h].pression (controls[h].evtype, controls[h].voice, *(int *) controls[h].p);
			refreshcontrols (h);
			break;
		    case BUT_OFF:
			controls[h].nbmodif++;
			*(int *) controls[h].p = BUT_ON;
			if (controls[h].associated != CONTUNDEF)
			    refreshcontrols (controls[h].associated);
			if (controls[h].pression != NULL)
			    controls[h].pression (controls[h].evtype, controls[h].voice, *(int *) controls[h].p);
			refreshcontrols (h);
			break;
		}
	    }
	    break;

	case CONTRRADIO:	/* radio button - clickcontrols */
	    {
		Radio  *p = (Radio *) controls[h].p;
		int     oldv;

		if (p != NULL)
		{
		    oldv = p->value;
		    if (controls[h].d.bs.radio != oldv)
		    {
			controls[h].nbmodif++;
			p->value = controls[h].d.bs.radio;
			if (controls[h].pression != NULL)
			    controls[h].pression (controls[h].evtype, controls[h].voice, p->value);
			refreshcontrols (h);
			refreshcontrols (p->h[oldv]);
		    }
		}
	    }
	    break;

	case CONTLISTBOX:	/* list box - clickcontrols */
	    if (xrev == controls[h].d.lb.larg)	/* on est dans la marge */
	    {
		if (yrev == 0)	/* one-up               */
		{
		    if (controls[h].d.lb.curoff > 0)
		    {
			controls[h].d.lb.curoff--;
			refreshcontrols (h);
		    }
		}
		if (yrev == controls[h].d.lb.haut - 1)
		{
		    if (controls[h].d.lb.curoff < controls[h].d.lb.nbelem - controls[h].d.lb.haut)
		    {
			controls[h].d.lb.curoff++;
			refreshcontrols (h);
		    }
		}
	    }
	    else
	      /* on est dans les champs */
	    {
		int     n = controls[h].d.lb.curoff + yrev;

		if (n < controls[h].d.lb.nbelem)
		{
		    controls[h].nbmodif++;
		    if (n == controls[h].d.lb.cursur)
			*(int *) controls[h].p = n;
		    else
			controls[h].d.lb.cursur = n;
		    if (controls[h].pression != NULL)
			controls[h].pression (controls[h].evtype, controls[h].voice, *(int *) controls[h].p);
		    refreshcontrols (h);
		}
	    }
	    break;

	case CONTTEXTEDIT:	/* editable text - clickcontrols */
	    if (xrev <= controls[h].d.te.curlen - controls[h].d.te.offset)
	    {
		controls[h].d.te.curpos = controls[h].d.te.offset + xrev;
		refreshcontrols (h);
	    }
	    else
	    {
		controls[h].d.te.curpos = controls[h].d.te.curlen;
		refreshcontrols (h);
	    }
	    break;

	case CONTBGND:		/* fond d'ecran - clickcontrols */
	case CONTUNDEF:
	case CONTRGROUP:	/* groupe ??? - clickcontrols */
	default:
	    return -1;
    }
    return 0;
}

int     setpression (int h, Tpression pression, int evtype, int voice)
{
    switch (controls[h].type)
    {
	case CONTRHSLIDE:	/* slide horizontal - clearmodif */
	case CONTRVSLIDE:	/* slide vertical - clearmodif */
	case CONTRNUMRANGE:	/* numerical range - clearmodif */
	case CONTRPFIELD:	/* pattern row - clearmodif */
	case CONTRBUTTON:	/* button - clearmodif */
	case CONTRRADIO:	/* radio button - clearmodif */
	case CONTLISTBOX:	/* list box - clearmodif */
	case CONTTEXTEDIT:	/* editable text - clearmodif */
	case CONTRGROUP:	/* groupe ??? - clearmodif */
	    controls[h].pression = pression;
	    controls[h].evtype = evtype;
	    controls[h].voice = voice;
	    return 0;
	case CONTBGND:		/* fond d'ecran - clearmodif */
	case CONTUNDEF:
	default:
	    return -1;
    }
}

int     createcontrols (int type, int x, int y, int c, int assoc, void *data)	/* cree un
										   controls */
{
    int     i, h = 0;

    while ((controls[h].type != CONTUNDEF) && (h < MAXNBCONTROLS))
	h++;
    if (h == MAXNBCONTROLS)
    {
	fprintf (stderr, "no more free controls !!!?\n\r");
	return (0);
    }

    switch (type)
    {
	case CONTRGROUP:	/* groupe de controls - createcontrols */
	    controls[h].type = CONTRGROUP;
	    controls[h].o = curcontext;
	    controls[h].isfreezed = 0;
	    controls[h].nbmodif = 0;
	    controls[h].hjmeta = 0;
	    controls[h].pression = NULL;
	    controls[h].evtype = -1;
	    controls[h].voice = -1;
	    controls[h].c = c;
	    controls[h].p = NULL;
	    controls[h].associated = assoc;
	    break;

	case CONTRHSLIDE:	/* slide horizontal - createcontrols */
	    controls[h].type = CONTRHSLIDE;
	    controls[h].o = curcontext;
	    controls[h].isfreezed = 0;
	    controls[h].nbmodif = 0;
	    controls[h].hjmeta = 0;
	    controls[h].pression = NULL;
	    controls[h].evtype = -1;
	    controls[h].voice = -1;
	    controls[h].x = x - contexts[curcontext].x;
	    controls[h].y = y - contexts[curcontext].y;
	    controls[h].p = data;
	    controls[h].c = c;
	    controls[h].associated = assoc;
	    for (i = 0; i < 8; i++)
		(*whichcontrols)[y][x + i] = h;
	    refreshcontrols (h);
	    break;

	case CONTRVSLIDE:	/* slide vertical - createcontrols */
	    controls[h].type = CONTRVSLIDE;
	    controls[h].o = curcontext;
	    controls[h].isfreezed = 0;
	    controls[h].nbmodif = 0;
	    controls[h].hjmeta = 0;
	    controls[h].pression = NULL;
	    controls[h].evtype = -1;
	    controls[h].voice = -1;
	    controls[h].x = x - contexts[curcontext].x;
	    controls[h].y = y - contexts[curcontext].y;
	    controls[h].p = data;
	    controls[h].c = c;
	    controls[h].associated = assoc;
	    for (i = 0; i < 4; i++)
		(*whichcontrols)[y + i][x] = h;
	    refreshcontrols (h);
	    break;

	case CONTRNUMRANGE:	/* numerik range - createcontrols */
	    controls[h].type = CONTRNUMRANGE;
	    controls[h].o = curcontext;
	    controls[h].isfreezed = 0;
	    controls[h].nbmodif = 0;
	    controls[h].hjmeta = 0;
	    controls[h].pression = NULL;
	    controls[h].evtype = -1;
	    controls[h].voice = -1;
	    controls[h].x = x - contexts[curcontext].x;
	    controls[h].y = y - contexts[curcontext].y;
	    controls[h].p = data;
	    controls[h].c = c;
	    controls[h].associated = assoc;
	    for (i = 0; i < 5; i++)
		(*whichcontrols)[y][x + i] = h;
	    break;

	case CONTRRADIO:	/* radio button - createcontrols */
	    {
		Radio  *rdata = (Radio *) data;

		if ((rdata->nb >= 0) && (rdata->nb < CONTMAXRADIO))
		{
		    rdata->h[rdata->nb] = h;
		    controls[h].d.bs.radio = rdata->nb;
		    (rdata->nb)++;
		}
		else
		{
		    fprintf (stderr, "no more free radio button in this radio!!!?\n\r");
		    return (0);
		}
	    }			/* it's NORMAL there's no break here... */
	case CONTTEXTEDIT:	/* editable text - createcontrols */
	case CONTRPFIELD:	/* pattern row - createcontrols */
	case CONTNOTEEDIT:	/* editable score - owned creation */
	case CONTRBUTTON:	/* button - createcontrols */
	case CONTLISTBOX:	/* text box - createcontrols */
	case CONTTEXTSTR:	/* text box - createcontrols */
	    controls[h].type = type;
	    controls[h].o = curcontext;
	    controls[h].isfreezed = 0;
	    controls[h].nbmodif = 0;
	    controls[h].hjmeta = 0;
	    controls[h].pression = NULL;
	    controls[h].evtype = -1;
	    controls[h].voice = -1;
	    controls[h].x = x - contexts[curcontext].x;
	    controls[h].y = y - contexts[curcontext].y;
	    controls[h].p = data;
	    controls[h].c = c;
	    controls[h].associated = assoc;
	  /* le (*whichcontrols) est mis en place dans le create spezial ... */
	    break;

	case CONTBGND:		/* on ne peut pas creer de controles de ces types */
	case CONTUNDEF:
	default:
	    return (0);
    }

    if (c != 0)
	keyhandler[c] = h;
    registertocontexts (curcontext, h);		/* JDJDJDJD a parfaire ABSOLUMENT */
    return (h);
}

int     reassigncontrols (int h, void *data)
{
    switch (controls[h].type)
    {
	case CONTRHSLIDE:
	case CONTRNUMRANGE:
	case CONTRPFIELD:
	case CONTRRADIO:
	case CONTRVSLIDE:
	    controls[h].p = data;
	    refreshcontrols (h);
	    return (0);
	default:
	    fprintf (stderr, "reassigning an unassignable controls\r\n");
	    return (-1);
    }
}

int     reassoccontrols (int h, int assoc)
{
    switch (controls[h].type)
    {
	case CONTUNDEF:
	case CONTBGND:
	    fprintf (stderr, "reassociating an unassociable controls\r\n");
	    return (-1);

	default:
	    controls[h].associated = assoc;
	    return (0);
    }
}

int     creategroup (int type, int c, int assoc, int nb, int *listeh)
{
    int     h = 0;

    switch (type)
    {
	case CONTRGROUP:
	    h = createcontrols (type, 0, 0, c, assoc, NULL);
	    if (h == 0)
		return (0);
	    controls[h].d.gr.nb = nb;
	    controls[h].d.gr.h = listeh;
	    break;

	default:
	    return (0);
    }
    return (h);
}

int     createtextedit (int type, int x, int y, int c, int assoc, int larg, size_t len, void *data, int defaulth)
{
    int     h = 0, i;

    if (type != CONTTEXTEDIT)
	return 0;

    if ((h = createcontrols (type, x, y, c, assoc, data)) == 0)
	return 0;
    controls[h].d.te.larg = larg;
    controls[h].d.te.len = len;
    controls[h].d.te.curlen = strlen ((char *) data);
    controls[h].d.te.curpos = controls[h].d.te.curlen;
    if (controls[h].d.te.curlen > controls[h].d.te.larg)
	controls[h].d.te.offset = controls[h].d.te.offset = 1 + controls[h].d.te.curlen - controls[h].d.te.larg;
    else
	controls[h].d.te.offset = 0;

    controls[h].d.te.defaulth = defaulth;

    for (i = 0; i < larg + 1; i++)
	(*whichcontrols)[y][x + i] = h;

    refreshcontrols (h);
    return h;
}

int     reassigntextedit (int h, size_t len, void *data, int defaulth)
{
    if (controls[h].type != CONTTEXTEDIT)
	return -1;

    if (len == -1)
	strcpy ((char *) controls[h].p, data);
    else
    {
	controls[h].d.te.len = len;
	controls[h].p = data;
    }

    controls[h].d.te.curlen = strlen ((char *) data);
    controls[h].d.te.curpos = controls[h].d.te.curlen;
    if (controls[h].d.te.curlen > controls[h].d.te.larg)
	controls[h].d.te.offset = controls[h].d.te.offset = 1 + controls[h].d.te.curlen - controls[h].d.te.larg;
    else
	controls[h].d.te.offset = 0;

    if (defaulth != -1)
	controls[h].d.te.defaulth = defaulth;

    refreshcontrols (h);
    return 0;
}

int     createlistbox (int type, int x, int y, int c, int assoc, int larg, int haut, void *data, int nbelem, char **list)
{
    int     h = 0, i, j;

    if (type != CONTLISTBOX)
	return 0;

    if ((h = createcontrols (type, x, y, c, assoc, data)) == 0)
	return 0;
    controls[h].d.lb.list = list;
    controls[h].d.lb.haut = haut;
    controls[h].d.lb.larg = larg;
    controls[h].d.lb.nbelem = nbelem;
    controls[h].d.lb.curoff = 0;
    controls[h].d.lb.cursur = 0;

    for (j = 0; j < haut; j++)
	for (i = 0; i < larg + 1; i++)
	    (*whichcontrols)[y + j][x + i] = h;

    refreshcontrols (h);
    return h;
}

int     createtextstr (int type, int x, int y, int color, int len, char *text)
{
    int     h = 0, i;

    if (type != CONTTEXTSTR)
	return 0;

    if ((h = createcontrols (type, x, y, 0, CONTUNDEF, NULL)) == 0)
	return 0;
    if ((controls[h].d.ts.text = malloc (len + 2)) == NULL)
    {
	fprintf (stderr, "insuficient memory for %d bytes ?\n", len + 2);
	return 0;
    }
    strncpy (controls[h].d.ts.text, text, len);
    controls[h].d.ts.len = len;
    controls[h].d.ts.color = color;

    for (i = 0; i < len; i++)
	(*whichcontrols)[y][x + i] = h;

    refreshcontrols (h);
    return (h);
}

int     relooktextstr (int h, int color, char *text)
{
    if (controls[h].type != CONTTEXTSTR)
	return -1;

    if (text != NULL)
	strncpy (controls[h].d.ts.text, text, controls[h].d.ts.len);
    else
	strcpy (controls[h].d.ts.text, "");
    controls[h].d.ts.color = color;
    refreshcontrols (h);
    return (h);
}

int     reassignlistbox (int h, int nbelem, char **list)
{
    if (controls[h].type != CONTLISTBOX)
	return -1;
    controls[h].d.lb.list = list;
    controls[h].d.lb.nbelem = nbelem;
    controls[h].d.lb.curoff = 0;
    controls[h].d.lb.cursur = 0;
    *(int *) controls[h].p = -1;
    refreshcontrols (h);
    return 0;
}

int     createbutton (int type, int x, int y, int c, int assoc, void *data, int size, char *off, char *on, char *disabled, char *off_foc, char *on_foc, char *dis_foc)
{
    int     h = 0, i;

    switch (type)
    {
	case CONTRBUTTON:	/* button - createbutton */
	case CONTRRADIO:	/* radio button - createbutton */
	    h = createcontrols (type, x, y, c, assoc, data);
	    if (h == 0)
		return (0);
	    controls[h].d.bs.s[BUT_INV] = malloc (size + 6);
	    strcpy (controls[h].d.bs.s[BUT_INV], "\033[40m");
	    controls[h].d.bs.s[BUT_INV][3] = '0' + contexts[curcontext].color;
	    for (i = 0; i < size; i++)
		controls[h].d.bs.s[BUT_INV][5 + i] = ' ';
	    controls[h].d.bs.s[BUT_INV][5 + i] = 0;
	    controls[h].d.bs.s[BUT_INV + BUT_ISFOC] = controls[h].d.bs.s[BUT_INV];

	    controls[h].d.bs.s[BUT_ON] = malloc (strlen (on) + 1);
	    strcpy (controls[h].d.bs.s[BUT_ON], on);

	    controls[h].d.bs.s[BUT_OFF] = malloc (strlen (off) + 1);
	    strcpy (controls[h].d.bs.s[BUT_OFF], off);

	    controls[h].d.bs.s[BUT_DIS] = malloc (strlen (disabled) + 1);
	    strcpy (controls[h].d.bs.s[BUT_DIS], disabled);

	    if (off_foc != NULL)
	    {
		controls[h].d.bs.s[BUT_OFF + BUT_ISFOC] = malloc (strlen (off_foc) + 1);
		strcpy (controls[h].d.bs.s[BUT_OFF + BUT_ISFOC], off_foc);
	    }
	    else
		controls[h].d.bs.s[BUT_OFF + BUT_ISFOC] = controls[h].d.bs.s[BUT_OFF];

	    if (on_foc != NULL)
	    {
		controls[h].d.bs.s[BUT_ON + BUT_ISFOC] = malloc (strlen (on_foc) + 1);
		strcpy (controls[h].d.bs.s[BUT_ON + BUT_ISFOC], on_foc);
	    }
	    else
		controls[h].d.bs.s[BUT_ON + BUT_ISFOC] = controls[h].d.bs.s[BUT_ON];

	    if (dis_foc != NULL)
	    {
		controls[h].d.bs.s[BUT_DIS + BUT_ISFOC] = malloc (strlen (dis_foc) + 1);
		strcpy (controls[h].d.bs.s[BUT_DIS + BUT_ISFOC], dis_foc);
	    }
	    else
		controls[h].d.bs.s[BUT_DIS + BUT_ISFOC] = controls[h].d.bs.s[BUT_DIS];

	    for (i = 0; i < size; i++)	/* there, we trust the caller .... */
		(*whichcontrols)[y][x + i] = h;
	    refreshcontrols (h);
	    break;

	default:
	    return (0);
    }

    return (h);
}

int     createsimplebutton (int type, int x, int y, int c, int color, int assoc, void *data, char *content)
{
    char    on[48], foc_on[48], off[48], foc_off[48], dis[48], foc_dis[48], small[] = "c";
    int     len, puce = (color & CONTNOPUCE) ? 0 : 1;

    switch (type)
    {
	case CONTRBUTTON:	/* button - createsimplebutton */
	case CONTRRADIO:	/* radio button - createsimplebutton */

	    if ((color & (~(CONTNOPUCE))) != CONTNOCOL)
	    {
		strcpy (on, "\033[1m\033[41m");
		strcpy (off, "\033[43m");
		strcpy (dis, "\033[43m");
		strcpy (foc_on, "\033[1m\033[41m");
		strcpy (foc_off, "\033[1m\033[43m");
		strcpy (foc_dis, "\033[1m\033[43m");
	    }
	    else
	    {
		strcpy (on, "\033[1m\033[41m");
		strcpy (off, "\033[m");
		strcpy (dis, "\033[m");
		strcpy (foc_on, "\033[1m\033[41m");
		strcpy (foc_off, "\033[m\033[1m");
		strcpy (foc_dis, "\033[m\033[1m");
	    }

	    if (puce)
	    {
		strcat (on, "\033[31m\016`\017\033[33m");
		strcat (off, "\033[37m\016`\017");
		strcat (dis, "\033[30m ");
		strcat (foc_on, "\033[33m\016`\017");
		strcat (foc_off, "\033[37m\016`\017");
		strcat (foc_dis, "\033[30m ");
	    }
	    else
	    {
		strcat (on, "\033[33m");
		strcat (off, "\033[37m");
		strcat (dis, "\033[30m");
		strcat (foc_on, "\033[33m");
		strcat (foc_off, "\033[37m");
		strcat (foc_dis, "\033[30m");
	    }

	    if (content == NULL)
	    {
		small[0] = c;
		strcat (on, small);
		strcat (foc_on, small);
		strcat (off, small);
		strcat (foc_off, small);
		strcat (dis, small);
		strcat (foc_dis, small);
		len = 1 + puce;
	    }
	    else
	    {
		len = strlen (content) + puce;
		strcat (on, content);
		strcat (foc_on, content);
		strcat (off, content);
		strcat (foc_off, content);
		strcat (dis, content);
		strcat (foc_dis, content);
	    }

	    strcat (on, "\033[m");
	    strcat (foc_on, "\033[m");
	    strcat (off, "\033[m");
	    strcat (foc_off, "\033[m");
	    strcat (dis, "\033[m");
	    strcat (foc_dis, "\033[m");

	    color &= ~(CONTNOPUCE);
	    if (color != CONTNOCOL)
	    {
		off[3] = color + '0';
		dis[3] = color + '0';
		foc_off[7] = color + '0';
		foc_dis[7] = color + '0';
	    }

	    return (createbutton (type, x, y, c, assoc, data, len, off, on, dis, foc_off, foc_on, foc_dis));

	default:
	    return (0);
    }
}

int     relooksimplebutton (int h, int c, int color, char *content)
{				/* JDJDJDJD on suppose que l'on reassigne de la meme
				   taille... */
    char   *on = controls[h].d.bs.s[BUT_ON], *foc_on = controls[h].d.bs.s[BUT_ON + BUT_ISFOC],
           *off = controls[h].d.bs.s[BUT_OFF], *foc_off = controls[h].d.bs.s[BUT_OFF + BUT_ISFOC],
           *dis = controls[h].d.bs.s[BUT_DIS], *foc_dis = controls[h].d.bs.s[BUT_DIS + BUT_ISFOC],
            small[] = "c";
    int     len, puce = (color & CONTNOPUCE) ? 0 : 1;

    switch (controls[h].type)
    {
	case CONTRBUTTON:	/* button - createsimplebutton */
	case CONTRRADIO:	/* radio button - createsimplebutton */

	    if ((color & (~(CONTNOPUCE))) != CONTNOCOL)
	    {
		strcpy (on, "\033[1m\033[41m");
		strcpy (off, "\033[43m");
		strcpy (dis, "\033[43m");
		strcpy (foc_on, "\033[1m\033[41m");
		strcpy (foc_off, "\033[1m\033[43m");
		strcpy (foc_dis, "\033[1m\033[43m");
	    }
	    else
	    {
		strcpy (on, "\033[1m\033[41m");
		strcpy (off, "\033[m");
		strcpy (dis, "\033[m");
		strcpy (foc_on, "\033[1m\033[41m");
		strcpy (foc_off, "\033[m\033[1m");
		strcpy (foc_dis, "\033[m\033[1m");
	    }

	    if (puce)
	    {
		strcat (on, "\033[31m\016`\017\033[33m");
		strcat (off, "\033[37m\016`\017");
		strcat (dis, "\033[30m ");
		strcat (foc_on, "\033[33m\016`\017");
		strcat (foc_off, "\033[37m\016`\017");
		strcat (foc_dis, "\033[30m ");
	    }
	    else
	    {
		strcat (on, "\033[33m");
		strcat (off, "\033[37m");
		strcat (dis, "\033[30m");
		strcat (foc_on, "\033[33m");
		strcat (foc_off, "\033[37m");
		strcat (foc_dis, "\033[30m");
	    }

	    if (content == NULL)
	    {
		small[0] = c;
		strcat (on, small);
		strcat (foc_on, small);
		strcat (off, small);
		strcat (foc_off, small);
		strcat (dis, small);
		strcat (foc_dis, small);
		len = 1 + puce;
	    }
	    else
	    {
		len = strlen (content) + puce;
		strcat (on, content);
		strcat (foc_on, content);
		strcat (off, content);
		strcat (foc_off, content);
		strcat (dis, content);
		strcat (foc_dis, content);
	    }

	    strcat (on, "\033[m");
	    strcat (foc_on, "\033[m");
	    strcat (off, "\033[m");
	    strcat (foc_off, "\033[m");
	    strcat (dis, "\033[m");
	    strcat (foc_dis, "\033[m");

	    color &= ~(CONTNOPUCE);
	    if (color != CONTNOCOL)
	    {
		off[3] = color + '0';
		dis[3] = color + '0';
		foc_off[7] = color + '0';
		foc_dis[7] = color + '0';
	    }

	default:
	    return (0);
    }
}

int     zeroradio (Radio *p)
{
    int     oldv;

    if (p != NULL)
    {
	oldv = p->value;
	p->value = -1;
	refreshcontrols (p->h[oldv]);
	return 0;
    }
    else
	return 1;
}

int     createpfield (int type, int x, int y, int c, int assoc, void *data, int maxrow, int nbrow, Radio *rdata)
{
    int     h = 0, i;

    switch (type)
    {
	case CONTRPFIELD:	/* pattern row - owned creation */
	    h = createcontrols (type, x, y, c, assoc, data);
	    if (h == 0)
		return 0;
	    controls[h].d.pf.maxrow = maxrow;
	    controls[h].d.pf.nbrow = nbrow;
	    controls[h].d.pf.rdata = rdata;
	    for (i = 0; i < maxrow; i++)
		(*whichcontrols)[y][x + i] = h;
	    if (rdata != NULL)
	    {
		if ((rdata->nb >= 0) && (rdata->nb < CONTMAXRADIO))
		{
		    rdata->h[rdata->nb] = h;
		    controls[h].d.pf.radio = rdata->nb;
		    (rdata->nb)++;
		}
		else
		{
		    fprintf (stderr, "no more free radio button in this radio!!!?\n\r");
		    return (0);
		}
	    }
	    refreshcontrols (h);
	    break;

	default:
	    return (0);
    }

    return (h);
}

int     createscore (int type, int x, int y, int c, int assoc, void *data, int maxrow, int nbrow, int nbligne, Radio *rdata)
{
    int     h = 0, i, j;

    switch (type)
    {
	case CONTNOTEEDIT:	/* editable score - owned creation */
	    fprintf (stderr, "bwen on cree un score...\n");
	    h = createcontrols (type, x, y, c, assoc, data);
	    if (h == 0)
		return 0;
	    controls[h].d.es.maxrow = maxrow;
	    controls[h].d.es.nbrow = nbrow;
	    controls[h].d.es.rdata = rdata;
	    controls[h].d.es.nbligne = nbligne;
	    for (i = 0; i < maxrow; i++)
		for (j = 0; j < nbligne; j++)
		    (*whichcontrols)[y + j][x + i] = h;
	    if (rdata != NULL)
	    {
		if ((rdata->nb >= 0) && (rdata->nb < CONTMAXRADIO))
		{
		    rdata->h[rdata->nb] = h;
		    controls[h].d.es.radio = rdata->nb;
		    (rdata->nb)++;
		}
		else
		{
		    fprintf (stderr, "no more free radio button in this radio!!!?\n\r");
		    return (0);
		}
	    }
	    refreshcontrols (h);
	    break;

	default:
	    return (0);
    }

    return (h);
}

int     createrange (int type, int x, int y, int c, int assoc, void *data, int minv, int maxv)
{
    int     h = 0;

    switch (type)
    {
	case CONTRNUMRANGE:	/* numerik range - owned creation */
	    h = createcontrols (type, x, y, c, assoc, data);
	    if (h == 0)
		return 0;
	    controls[h].d.mm.minv = minv;
	    controls[h].d.mm.maxv = maxv;
	    refreshcontrols (h);
	    break;

	default:
	    return (0);
    }

    return (h);
}

/* missing : deletecontexts or freecontexts ... JDJDJDJD */
int     createcontexts (int x, int y, int lx, int ly, int color, char *name, Tactionne actionne)
{
    int     i, h = -1;

    for (i = 1; i < MAXCONTEXTS; i++)
	if (contexts[i].type == CONTCTXFREE)
	{
	    h = i;
	    break;
	}
    if (h == -1)
    {
	fprintf (stderr, "no more context available, aborting operation\n");
	return -1;
    }

    contexts[h].type = CONTCTXUSED;
    contexts[h].x = x;
    contexts[h].y = y;
    contexts[h].lx = lx;
    contexts[h].ly = ly;
    contexts[h].color = color;
    contexts[h].nbconttoinstall = 0;
    contexts[h].actionne = actionne;
    strncpy (contexts[h].name, name, CONTEXTNAMESIZE);

#ifndef MEGASTATIC
// JDJDJDJD version semi statique (alloc 1x pour toutes)
    contexts[h].whichcontrols = (int *) malloc ((lx * ly) * sizeof (int));
#ifdef SIMPLE_MEMPROFILE
    totdivs += (long) sizeof (int) * (lx * ly);
    fprintf (stderr, "context %s allocated %dx%d = %ld bytes\n",
		    name,
		    lx, ly,
		    (long) sizeof (int) * (lx * ly));
#endif

#endif
    if (contexts[h].whichcontrols == NULL)
    {
	fprintf (stderr, "error: creating context %s couldn't allocate %ld bytes ?\n", name, (long) (lx * ly * sizeof (int)));

	fprintf (stderr, "error: really don't know what to do... The worse may occurs !\n");
	return -1;
    }

    return h;
}

void    actionnecontexts (int h, Tactionne actionne)
{
    contexts[h].actionne = actionne;	/* JDJDJDJD on devrait reflechir a deux fois et
					   tester un poil plus avant d'affecter par ici */
}
/* JDJDJDJDJD registration to appropriate context should be automatic.... should deregister
   when deleting a controls, maybe adding a "registeredto" var ??? */

int     registertocontexts (int ctx, int ctl)
{
    if (contexts[ctx].type == CONTCTXFREE)
    {
	fprintf (stderr, "attempt to register controls %d to empty context %d\n", ctl, ctx);
	return -1;
    }
    if (contexts[ctx].nbconttoinstall >= MAXNBCONTROLS)
    {
	fprintf (stderr, "max # of controls reached in context %d\n", ctx);
	return -1;
    }
    if (controls[ctl].type == CONTUNDEF)
    {
	fprintf (stderr, "attempt to register undefined controls %d to context %d\n", ctl, ctx);
	return -1;
    }
    if (controls[ctl].o != ctx)
    {
	int     xabs = controlsx (ctl), yabs = controlsy (ctl);

	fprintf (stderr, "warning, REregistration to context isn't mature yet !!!\n");
	controls[ctl].x = xabs - contexts[ctx].x;
	controls[ctl].y = yabs - contexts[ctx].y;
    }
    contexts[ctx].toinstall[contexts[ctx].nbconttoinstall] = ctl;
    contexts[ctx].nbconttoinstall++;
    return 0;
}

void    refreshcontexts (int h)
{
    int     i, j;

    if (h == 0)
    {
	clearscreen ();
	return;
    }

    if (contexts[h].type == CONTCTXFREE)
	return;

    printf ("\033[%d;%dH\033[4%cm\016l", contexts[h].y, contexts[h].x, contexts[h].color + '0');
    for (i = 1; i < contexts[h].lx - 1; i++)
	printf ("q");
    printf ("k");
    for (j = 1; j < contexts[h].ly - 1; j++)
    {
	printf ("\033[%d;%dH\033[4%cmx", contexts[h].y + j, contexts[h].x, contexts[h].color + '0');
	for (i = 1; i < contexts[h].lx - 1; i++)
	    printf (" ");
	printf ("x");
    }
    printf ("\033[%d;%dH\033[4%cm\016m", contexts[h].y + j, contexts[h].x, contexts[h].color + '0');
    for (i = 1; i < contexts[h].lx - 1; i++)
	printf ("q");
    printf ("j");
    printf ("\017\033[m");

    for (i = 0; i < contexts[h].nbconttoinstall; i++)
	refreshcontrols (contexts[h].toinstall[i]);
}

int     raisecontext (int h);

int     installcontexts_ici (int h, int x, int y)
{
    int     i, j, ctemp;

    if (contexts[h].type == CONTCTXFREE)
    {
	fprintf (stderr, "attempt to install an empty context ???\n");
	return curcontext;
    }
    if (contexts[h].isinstalled != 0)
    {
	return (raisecontext (h));
    }
  /* la on determine les contexts geles par le nouveau context */
    memcpy (&tabwhichcontrols[nbctx], whichcontrols,
	    sizeof (short) * MAXSCREENWIDTH * MAXSCREENHEIGHT);

    if (x == -1)
	x = contexts[h].x;
    else
	contexts[h].x = x;

    if (y == -1)
	y = contexts[h].y;
    else
	contexts[h].y = y;

    nbcontfreez[nbctx] = 0;
    for (j = 0; j < contexts[h].ly; j++)
	for (i = 0; i < contexts[h].lx; i++)
	{
	    tabwhichcontrols[nbctx][y + j][x + i] = CONTUNDEF;
	    ctemp = (*whichcontrols)[y + j][x + i];

	    if ((ctemp <= CONTBGND) || (controls[ctemp].isfreezed))
		continue;

	    if (nbcontfreez[nbctx] == MAXNBCONTROLS)
	    {
		fprintf (stderr, "number of controls exceeded in contexts %d installation\n", h);
		break;
	    }
	    freezedlist[nbctx][nbcontfreez[nbctx]] = ctemp;
	    nbcontfreez[nbctx]++;
	    controls[ctemp].isfreezed = 1;
	}
    for (j = 0; j < contexts[h].ly; j++)
#ifdef MEGASTATIC
	memcpy (&tabwhichcontrols[nbctx][y + j][x],
		&contexts[h].whichcontrols[j][0], sizeof (int) * contexts[h].lx);

#else
// version semi statique (alloc 1x pour toutes)
	memcpy (&tabwhichcontrols[nbctx][y + j][x],
	     contexts[h].whichcontrols + j * contexts[h].lx, sizeof (int) * contexts[h].lx);

#endif

#ifdef DEBUGNBCONTR
    {
	int     i;

	for (i = 0; i < nbctx; i++)
	    fprintf (stderr, "  ");
    }
    fprintf (stderr, "-> context %2d-%-10s, rank %-2d: %3d controls freezed more\n", h, contexts[h].name, nbctx, nbcontfreez[nbctx]);
#endif

    for (i = 0; i < contexts[h].nbconttoinstall; i++)
	controls[contexts[h].toinstall[i]].isfreezed = 0;

    contexts[h].isinstalled = 1;
    refreshcontexts (h);

    contextfilo[nbctx] = h;
    whichcontrols = &tabwhichcontrols[nbctx];
    nbctx++;
    curcontext = h;
    return curcontext;
}

int     installcontexts (int h)
{
    return installcontexts_ici (h, -1, -1);
}

/* JDJDJD should treat this :  handle the case of allready installed context or not in
   install/desinstall */

/* JDJDJDJD JENSUISLA */
/* a reecrire avec une meilleur gestion vis a vis de nbctx... */
/* penser au probleme de desinstallation successives puis reinstallations... */
int     desinstallcontexts (int h)
{
    int     i, j, x, y;

    if (!contexts[h].isinstalled)
    {
	fprintf (stderr, "trying to desinstall a context that's not installed %d\n", h);
	return curcontext;
    }

    if (h != curcontext)
#ifdef KEEPCTXORDER
    {
	fprintf (stderr, "trying to leave context %d(asked) but we're in %d!\n", h, curcontext);
	return curcontext;
    }
#else
	raisecontext (h);
#endif

    x = contexts[h].x;
    y = contexts[h].y;

    for (i = 0; i < contexts[h].nbconttoinstall; i++)
	controls[contexts[h].toinstall[i]].isfreezed = 1;

    for (j = 0; j < contexts[h].ly; j++)
    {
	printf ("\033[%d;%dH", y + j, x);
	for (i = 0; i < contexts[h].lx; i++)
	    printf (" ");
    }
    for (j = 0; j < contexts[h].ly; j++)
#ifdef MEGASTATIC
// JDJDJDJD version en allocation ultrastatique
	memcpy (&contexts[h].whichcontrols[j][0],
		&tabwhichcontrols[nbctx - 1][y + j][x], sizeof (int) * contexts[h].lx);

#else
// version semi statique (alloc 1x pour toutes)
	memcpy (contexts[h].whichcontrols + j * contexts[h].lx,
		&tabwhichcontrols[nbctx - 1][y + j][x], sizeof (int) * contexts[h].lx);

#endif

    if (nbctx - 2 > 0)
	refreshcontexts (contextfilo[nbctx - 2]);

    for (i = 0; i < nbcontfreez[nbctx - 1]; i++)
    {
	controls[freezedlist[nbctx - 1][i]].isfreezed = 0;
	refreshcontrols (freezedlist[nbctx - 1][i]);
    }

    contexts[h].isinstalled = 0;
    whichcontrols = &tabwhichcontrols[nbctx - 2];
    curcontext = contextfilo[nbctx - 2];
    nbctx--;
#ifdef DEBUGNBCONTR
    {
	int     i;

	for (i = 0; i < nbctx; i++)
	    fprintf (stderr, "  ");
    }
    fprintf (stderr, "<- context %2d-%-10s, rank %-2d: %3d controls released (back to %s)\n", h, contexts[h].name, nbctx, nbcontfreez[nbctx], contexts[curcontext].name);
#endif

    return curcontext;
}

int     raisecontext (int h)
{
    int     areinstaller[MAXCONTEXTS], i = 0;

    if (contexts[h].isinstalled == 0)
    {
	fprintf (stderr, "attempt to raise an uninstalled context %d\n", h);
	return curcontext;
    }
    if (curcontext == h)
    {
	fprintf (stderr, "raising current context (%d) ? strange...\n", h);
	return curcontext;
    }

    while (curcontext != h)
    {
	areinstaller[i++] = curcontext;
	desinstallcontexts (curcontext);
    }
    desinstallcontexts (h);
    while (i > 0)
    {
	installcontexts (areinstaller[--i]);
    }
    return installcontexts (h);
}

int     deletecontrols (int h)
{
    int     i, j;

    if ((h <= 0) || (h >= MAXNBCONTROLS))
	return (-1);

    switch (controls[h].type)
    {
	case CONTRHSLIDE:	/* slide horizontal - deletecontrols */
	    for (i = 0; i < 8; i++)
		(*whichcontrols)[controlsy (h)][controlsx (h) + i] = 0;
	    printf ("\033[%d;%dH        ", controlsy (h), controlsx (h));
	    break;

	case CONTRVSLIDE:	/* slide vertical - deletecontrols */
	    for (i = 0; i < 8; i++)
		(*whichcontrols)[controlsy (h)][controlsx (h) + i] = 0;
	    printf ("\033[%d;%dH%s", controlsy (h), controlsx (h), strhlevel[9][0]);
	    break;

	case CONTRNUMRANGE:	/* numerical range - deletecontrols */
	    for (i = 0; i < 5; i++)
		(*whichcontrols)[controlsy (h)][controlsx (h) + i] = 0;
	    printf ("\033[%d;%dH     ", controlsy (h), controlsx (h));
	    break;

	case CONTRBUTTON:	/* button  - deletecontrols */
	case CONTRRADIO:	/* radio  - deletecontrols */
	    if (controls[h].d.bs.s[BUT_ON] != controls[h].d.bs.s[BUT_ON + BUT_ISFOC])
		free (controls[h].d.bs.s[BUT_ON + BUT_ISFOC]);
	    if (controls[h].d.bs.s[BUT_OFF] != controls[h].d.bs.s[BUT_OFF + BUT_ISFOC])
		free (controls[h].d.bs.s[BUT_OFF + BUT_ISFOC]);
	    if (controls[h].d.bs.s[BUT_DIS] != controls[h].d.bs.s[BUT_DIS + BUT_ISFOC])
		free (controls[h].d.bs.s[BUT_DIS + BUT_ISFOC]);
	    free (controls[h].d.bs.s[BUT_ON]);
	    free (controls[h].d.bs.s[BUT_OFF]);
	    free (controls[h].d.bs.s[BUT_DIS]);
	    free (controls[h].d.bs.s[BUT_INV]);
	  /* JDJDJD its normal there's no break here... */
	case CONTRPFIELD:	/* pattern field  - deletecontrols */
	    i = 0;
	    printf ("\033[%d;%dH", controlsy (h), controlsx (h));
	    while ((*whichcontrols)[controlsy (h)][controlsx (h) + i] == h)
	    {
		(*whichcontrols)[controlsy (h)][controlsx (h) + i] = 0;
		printf (" ");
		i++;
	    }
	    break;

	case CONTLISTBOX:	/* list box  - deletecontrols */
	    j = 0;		/* JDJDJD on pourrait utiliser lb.larg et lb.haut.... */
	    while ((*whichcontrols)[controlsy (h) + j][controlsx (h)] == h)
	    {
		printf ("\033[%d;%dH", controlsy (h) + j, controlsx (h));
		i = 0;
		while ((*whichcontrols)[controlsy (h) + j][controlsx (h) + i] == h)
		{
		    (*whichcontrols)[controlsy (h) + j][controlsx (h) + i] = 0;
		    printf (" ");
		    i++;
		}
	    }
	    break;

	case CONTTEXTSTR:	/* text string - deletecontrols */
	    if (controls[h].d.ts.text != NULL)
		free (controls[h].d.ts.text);
	    for (i = 0; i < controls[h].d.ts.len; i++)
		(*whichcontrols)[controlsy (h)][controlsx (h) + i] = 0;
	    break;

	case CONTBGND:
	case CONTUNDEF:
	default:
	    return -1;
    }
    controls[h].type = CONTUNDEF;
    return 0;
}

int     GetTransC (FILE * f)
{
    int     c = Pgpm_getc (f);

  /* juste pour memoire... #ifdef MY_MAMA_DONT_TOLD_ME #define MATCH_F5        0x1b5b31357e
     #define MATCH_F6        0x1b5b31377e #define MATCH_F7        0x1b5b31387e #define
     MATCH_F8        0x1b5b31397e #define MATCH_INS       0x1b5b327e #define MATCH_F9
     0x1b5b32307e #define MATCH_F10       0x1b5b32317e #define MATCH_F11       0x1b5b32337e
     #define MATCH_F12       0x1b5b32347e #define MATCH_PAGU      0x1b5b357e #define
     MATCH_PAGD      0x1b5b367e #define MATCH_UARR      0x1b5b41 #define MATCH_DARR 0x1b5b42
     #define MATCH_RARR      0x1b5b43 #define MATCH_LARR      0x1b5b44 #define MATCH_END
     0x1b5b46 #define MATCH_HOME      0x1b5b48 #define MATCH_F1 0x1b4f50 #define MATCH_F2
     0x1b4f51 #define MATCH_F3        0x1b4f52 #define MATCH_F4        0x1b4f53 #endif */

    if (c == 0x1b)
    {
	switch (c = Pgpm_getc (f))
	{
	    case 0x5b:
		switch (c = Pgpm_getc (f))
		{
		    case 0x31:
			switch (c = Pgpm_getc (f))
			{
			    case 0x35:
				if ((c = Pgpm_getc (f)) == 0x7e)
				    c = CONTKEY_F5;
				break;
			    case 0x37:
				if ((c = Pgpm_getc (f)) == 0x7e)
				    c = CONTKEY_F6;
				break;
			    case 0x38:
				if ((c = Pgpm_getc (f)) == 0x7e)
				    c = CONTKEY_F7;
				break;
			    case 0x39:
				if ((c = Pgpm_getc (f)) == 0x7e)
				    c = CONTKEY_F8;
				break;
			    default:
				break;
			}
			break;
		    case 0x32:
			switch (c = Pgpm_getc (f))
			{
			    case 0x7e:
				c = CONTKEY_INS;
				break;
			    case 0x30:
				if ((c = Pgpm_getc (f)) == 0x7e)
				    c = CONTKEY_F9;
				break;
			    case 0x31:
				if ((c = Pgpm_getc (f)) == 0x7e)
				    c = CONTKEY_F10;
				break;
			    case 0x33:
				if ((c = Pgpm_getc (f)) == 0x7e)
				    c = CONTKEY_F11;
				break;
			    case 0x34:
				if ((c = Pgpm_getc (f)) == 0x7e)
				    c = CONTKEY_F12;
				break;
			}
			break;
		    case 0x33:
			if ((c = Pgpm_getc (f)) == 0x7e)
			    c = CONTKEY_SUPR;
			break;
		    case 0x35:
			if ((c = Pgpm_getc (f)) == 0x7e)
			    c = CONTKEY_PAGU;
			break;
		    case 0x36:
			if ((c = Pgpm_getc (f)) == 0x7e)
			    c = CONTKEY_PAGD;
			break;
		    case 0x41:
			c = CONTKEY_UARR;
			break;
		    case 0x42:
			c = CONTKEY_DARR;
			break;
		    case 0x43:
			c = CONTKEY_RARR;
			break;
		    case 0x44:
			c = CONTKEY_LARR;
			break;
		    case 0x46:
			c = CONTKEY_END;
			break;
		    case 0x48:
			c = CONTKEY_HOME;
			break;
		    case 0x5b:
			switch (c = Pgpm_getc (f))
			{
			    case 0x41:
				c = CONTKEY_F1;
				break;
			    case 0x42:
				c = CONTKEY_F2;
				break;
			    case 0x43:
				c = CONTKEY_F3;
				break;
			    case 0x44:
				c = CONTKEY_F4;
				break;
			    case 0x45:
				c = CONTKEY_F5;
				break;
			    default:
				;
			}
		    default:
			break;
		}
		break;
	    case 0x4f:
		switch (c = Pgpm_getc (f))
		{
		    case 0x50:
			c = CONTKEY_F1;
			break;
		    case 0x51:
			c = CONTKEY_F2;
			break;
		    case 0x52:
			c = CONTKEY_F3;
			break;
		    case 0x53:
			c = CONTKEY_F4;
			break;
		    default:
			break;
		}
		break;
	    default:
		break;
	}
    }
    return c;
}

void    pollmouse (void)
{
    int     c, r;

    static struct timeval pollmtime;	/* pour le polling */

    static int reentre = 0;

#ifdef JDSEMAPHORE
    if (testlocksema (semid))
	return;
#else
    if (pollreintrance)
	return;			/* verification de la reentrance du bordel */
    pollreintrance = 1;		/* JDJDJDJD une fonction atomique serait meilleure */
#endif
    reentre++;
  /* rearmement du polling */
    FD_ZERO (&readfdset);
    FD_SET (0, &readfdset);
    FD_SET (fmouse, &readfdset);
    pollmtime.tv_sec = 0;	/* on attend 0 sec...           */
    pollmtime.tv_usec = 0;

    while ((r = select (fmouse + 1, &readfdset, NULL, NULL, &pollmtime)))
    {

	if ((r == -1) && (errno == EINTR))
	{
	    FD_ZERO (&readfdset);
	    FD_SET (0, &readfdset);
	    FD_SET (fmouse, &readfdset);
	    pollmtime.tv_sec = 0;	/* on attend 0 sec...           */
	    pollmtime.tv_usec = 0;
	    continue;
	}

	if (FD_ISSET (fmouse, &readfdset) || FD_ISSET (0, &readfdset))
	{
	    c = GetTransC (stdin);
#ifdef DEBUGKEYB
	    pdebug (c, "keypressed");
	    fprintf (stderr, "%02x key\n", c);
#endif
/* JDJDJDJDJD LASTPRESS */
	    if (c > 1)
		if (!keycontrols (focus, c))
		    if (keyhandler[c] != -1)
			keycontrols (keyhandler[c], c);
	}

	FD_ZERO (&readfdset);
	FD_SET (0, &readfdset);
	FD_SET (fmouse, &readfdset);
	pollmtime.tv_sec = 0;	/* on attend 0 sec...           */
	pollmtime.tv_usec = 0;
    }
    reentre--;
#ifdef JDSEMAPHORE
    unlocksema (semid);
#else
    pollreintrance = 0;
#endif
}

int     pollcontrols (int milliwait)
{
    static struct timeval pollctime;	/* pour le polling */

/* #ifndef ALARMHANDLER */
    int     c = -1;
    int     r, lpressed;

/* #endif */
    int     i;
    int     newnbtoberefreshed = 0;

#ifdef JDSEMAPHORE
    if (testlocksema (semid))
	return -1;
#else
    if (pollreintrance)
	return -1;		/* verification de la reentrance du bordel */
    pollreintrance = 1;		/* JDJDJDJD une fonction atomique serait meilleure */
#endif
    refreshing = 1;
    lpressed = lastpressed;
    lastpressed = -1;

/* #ifndef ALARMHANDLER */
/* if (!usetimer) {                     enleve pour le polling en pause sous console */
  /* rearmement du polling */
    FD_ZERO (&readfdset);
    FD_SET (0, &readfdset);
    FD_SET (fmouse, &readfdset);
    pollctime.tv_sec = 0;	/* on attend 0 sec...           */
    pollctime.tv_usec = milliwait;	/* et des poussieres    */

    while ((r = select (fmouse + 1, &readfdset, NULL, NULL, &pollctime)))
    {
	if ((r == -1) && (errno == EINTR))
	    continue;

	if (FD_ISSET (fmouse, &readfdset) || FD_ISSET (0, &readfdset))
	{
	    c = GetTransC (stdin);
#ifdef DEBUGKEYB
	    pdebug (c, "keypressed");
#endif
/* JDJDJDJDJD LASTPRESS */
	    if (c > 1)
		if (!keycontrols (focus, c))
		    if (keyhandler[c] != -1)
			keycontrols (keyhandler[c], c);
	}

      /* rearmement du polling */
	FD_ZERO (&readfdset);
	FD_SET (0, &readfdset);
	FD_SET (fmouse, &readfdset);
	pollctime.tv_sec = 0;	/* on attend 0 sec...           */
	pollctime.tv_usec = 0;
    }
/* }    */
/* #endif */

    for (i = 0; i < nbtoberefreshed; i++)
    {
	if (controls[toberefreshed[i]].isfreezed)
	{
	    toberefreshed[newnbtoberefreshed] = toberefreshed[i];
	    newnbtoberefreshed++;
	}
	else if (controls[toberefreshed[i]].toberefreshed)
	    refreshcontrols (toberefreshed[i]);
    }
    nbtoberefreshed = newnbtoberefreshed;
    refreshing = 0;

#ifdef JDSEMAPHORE
    unlocksema (semid);
#else
    pollreintrance = 0;
#endif
    {
	Tactionne g, d[MAXCONTEXTS];
	int     k = 0;

	for (i = 0; i < nbctx; i++)
	{
	    if ((g = contexts[contextfilo[i]].actionne) != NULL)
		d[k++] = g;
	}
	for (k--; k >= 0; k--)
	    d[k] (lpressed);
    }

    return lpressed;
}

int     addstrback (char *s)	/* add a string in the list of background display */
{
    if ((nbbackstr < MAXBACKSTR) && (s != NULL))
    {
	if ((backstr[nbbackstr] = (char *) malloc (strlen (s) + 1)) != NULL)
	{
	    strcpy (backstr[nbbackstr], s);
	    printf ("\033[2m%s", s);
	    nbbackstr++;
	    return (nbbackstr - 1);
	}
	else
	    return (-1);
    }
    else
	return (-1);
}

void    removeallstrback ()
{
    int     i;

    for (i = 0; i < nbbackstr; i++)
	free (backstr[i]);
}

/* 
 * ---------------------------------- GPM Part ---------------------------------------
 */

void    scrflush (void)
{
    fflush (stdout);		/* JDJDJDJD was before GPM_DRAW is it good now ? */
#ifdef HAVE_LIBGPM
    if (uselibgpm)
	GPM_DRAWPOINTER (&gevent);
#endif
}

void    resetscreen (void)	/* for internal use only */
{
    printf ("\033[?25h");	/* show cursor on linux console (and some xterm) */
    echo ();
    nl ();
    noraw ();
    endwin ();
}

#ifdef HAVE_LIBGPM
int     traitemouselib (Gpm_Event *event, void *data)
{
    int     h;

    gevent.x = event->x;
    gevent.y = event->y;

/* if ((event->type&GPM_DOWN) || (event->type&GPM_UP)) */
    if (event->type & GPM_DOWN)
    {
#ifdef DEBUGMOUSE
	char    debugbuf[80];

	sprintf (debugbuf, "mouse: %s, at %2i,%2i, butt %i, mod %02X",
		 event->type & GPM_DOWN ? "press  "
		 : (event->type & GPM_UP ? "release" : "motion "),
		 event->x, event->y,
		 event->buttons, event->modifiers);
#endif
	if (event->x < 0)
	    event->x += 256;
	if (event->y < 0)
	    event->y = 0;
	h = (*whichcontrols)[event->y][event->x];
#ifdef DEBUGMOUSE
	pdebug (h, debugbuf);
#endif
	clickcontrols (h, event->x, event->y, event->buttons, changefocus (h));
	GPM_DRAWPOINTER (event);
	fflush (stdout);

	gpm_hflag = 1;
	return 1;
    }
    else
    {
	GPM_DRAWPOINTER (event);
	fflush (stdout);

	event->dx = event->dy = 0;	/* prepare for repetitions */
      /* if (event->type&GPM_DOWN) event->type=GPM_DRAG; */
    }
    gpm_hflag = 1;
    return 1;
}

int     initlibgpm (void)
{
    Gpm_Connect conn;

    conn.eventMask = ~0;
    conn.defaultMask = 0;
    conn.maxMod = ~0;
    conn.minMod = 0;

    initscr ();
    refresh ();
    raw ();
    nonl ();
    noecho ();
    printf ("\033[?25l");	/* hide cursor on linux console (and some xterm) */
    printf ("\033)0\n");	/* enable alternate charset     */

    if (Gpm_Open (&conn, 0) == -1)
    {
	fprintf (stderr, "no mouse connection, abort\n");
	return (-1);
    }
    gpm_handler = traitemouselib;
    return (gpm_fd == -2 ? 0 : gpm_fd);
}

int     resetlibgpm (void)
{
    Gpm_Close ();
    return (0);
}
#endif

int     traitemousexterm (Gpm_Event *event, void *data)
{
    int     h;

    gevent.x = event->x;
    gevent.y = event->y;

/* if ((event->type&GPM_DOWN) || (event->type&GPM_UP)) */
    if (event->type & GPM_DOWN)
    {
#ifdef DEBUGMOUSE
	char    debugbuf[80];

	sprintf (debugbuf, "mouse: %s, at %2i,%2i, butt %i, mod %02X",
		 event->type & GPM_DOWN ? "press  "
		 : (event->type & GPM_UP ? "release" : "motion "),
		 event->x, event->y,
		 event->buttons, event->modifiers);
#endif
	if (event->x < 0)
	    event->x += 256;
	if (event->y < 0)
	    event->y = 0;
	h = (*whichcontrols)[event->y][event->x];
#ifdef DEBUGMOUSE
	pdebug (h, debugbuf);
#endif
	clickcontrols (h, event->x, event->y, event->buttons, changefocus (h));
	JDGPM_DRAWPOINTER (event);
	fflush (stdout);

	JDgpm_hflag = 1;
	return 1;
    }
    else
    {
	JDGPM_DRAWPOINTER (event);
	fflush (stdout);

	event->dx = event->dy = 0;	/* prepare for repetitions */
      /* if (event->type&GPM_DOWN) event->type=GPM_DRAG; */
    }
    JDgpm_hflag = 1;
    return 1;
}
int     initgpmxterm (void)
{
    JDGpm_Connect conn;

    conn.eventMask = ~0;
    conn.defaultMask = 0;
    conn.maxMod = ~0;
    conn.minMod = 0;

    initscr ();
    refresh ();
    raw ();
    nonl ();
    noecho ();

    printf ("\033[?25l");	/* hide cursor on linux console (and some xterm) */
    printf ("\033)0\n");	/* enable alternate charset     */

    if (JDGpm_Open (&conn, 0) == -1)
    {
	fprintf (stderr, "no mouse connection, abort\n");
	return (-1);
    }
    JDgpm_handler = traitemousexterm;
    return (JDgpm_fd == -2 ? 0 : JDgpm_fd);
}
int     resetgpmxterm (void)
{
    JDGpm_Close ();
    return (0);
}

int     initgpm (void)
{
    int     f;

#ifdef HAVE_LIBGPM
    int     n;

    Gpm_GetLibVersion (&n);
    f = initlibgpm ();
    uselibgpm = 1;
    Pgpm_getc = Gpm_Getc;
    if ((f == 0) && (n < 11506))
    {
	fprintf (stderr, "your libgpm doesn't handle completly xterms, use my own lib\n");
	resetlibgpm ();
	f = initgpmxterm ();
	uselibgpm = 0;
	Pgpm_getc = JDGpm_Getc;
    }
#else
    f = initgpmxterm ();
    uselibgpm = 0;
    Pgpm_getc = JDGpm_Getc;
#endif
    return f;
}

int     resetgpm (void)
{
    int     r;

#ifdef HAVE_LIBGPM
    if (uselibgpm)
	r = resetlibgpm ();
    else
	r = resetgpmxterm ();
#else
    return r = resetgpmxterm ();
#endif
    resetscreen ();
    return r;
}

/* 
 * -------------------------- end of GPM part --------------------------------------
 */

int     initcontrols (void)
{
    int     i, j;

    focus = 0;
    curcontext = 0;

    if (firstuse)
    {
#ifdef SHOWRCS
	fprintf (stderr, "%s\n\r", rcsid);
#endif
	firstuse = 0;
    }

    strncpy (contexts[0].name, "board", CONTEXTNAMESIZE);
    contexts[0].type = CONTCTXUSED;
    nbctx = 1;

    for (i = 0; i < MAXKEYHANDLER; i++)
	keyhandler[i] = -1;

    for (i = 0; i < MAXSCREENWIDTH; i++)
	for (j = 0; j < MAXSCREENHEIGHT; j++)
	    (*whichcontrols)[j][i] = CONTUNDEF;
    for (i = 0; i < MAXNBCONTROLS; i++)
	controls[i].type = CONTUNDEF;

    controls[0].type = CONTBGND;
    keyhandler[0xC] = 0;	/* CTRL-L */

    initjmeta ();

#ifdef JDSEMAPHORE
  /* initialisation des semaphores */
    if ((semid = initinewsema ()) == -1)
    {
	resetscreen ();
	return (-2);
    }
#endif
  /* on active la souris et les variables de polling */
    gevent.x = 1;
    gevent.y = 1;
    if ((fmouse = initgpm ()) == -1)
    {
	resetscreen ();
	return (-2);
    }

    if (fmouse == 0)
	usetimer = 0;
    else
	usetimer = 1;

    if (strstr (getenv ("TERM"), "xterm") != NULL)
	usexterm = 1;
    else
	usexterm = 0;
  /* on efface l'ecran */
    printf ("\033[H\033[2J");

    return 0;
}

int     resetcontrols (void)
{
    int     h;

#ifdef SIMPLE_MEMPROFILE
    info_mem_controls ();
#endif

    for (h = 0; h < MAXNBCONTROLS; h++)
    {
	if (controls[h].type != CONTUNDEF)
	{
	    deletecontrols (h);
#ifdef TESTDELETE
	    Pgpm_getc (stdin);
	    fflush (stdout);
#endif
	}
    }
    removeallstrback ();
#ifdef TESTDELETE
    Pgpm_getc (stdin);
#endif
    fflush (stdout);
    resetgpm ();

#ifdef JDSEMAPHORE
  /* liberation du semaphore JDJDJD on devrait tester toutes les exits possibles pour ca */
    deletesema (semid);
#endif

#ifdef TESTDELETE
    fprintf (stderr, "all freed.\n");
#endif
/* fprintf (stderr, "...\n...flushing stdout\n");               */
    fflush (stdout);
/* fprintf (stderr, "entering stdin's flush...\n");     */
/* if (!feof (stdin))

   {    int c; while ((c=fgetc (stdin)) != EOF) fprintf (stderr, "char %0x2d flushed on
   stdin\n", c); } */
/* fprintf (stderr, "done...\n");                               */
    printf ("\033[H\033[2J\n");
    return (0);
}

#ifdef SIMPLE_MEMPROFILE
void info_mem_controls (void)
{   int i,
	nb = 0,
	holes = 0;

    fprintf (stderr, "Controls: %ld x %ld bytes = %ld\n", (long) MAXNBCONTROLS, (long) sizeof (Controls), (long) sizeof (controls));

    for (i=0 ; i<MAXNBCONTROLS ; i++)
    {	if (controls[i].type != CONTUNDEF)
	{   nb ++;
	    if (i > 0)
	    {
		if ((controls[i].type != CONTUNDEF) && (controls[i-1].type == CONTUNDEF))
		    holes ++;
		if ((controls[i].type == CONTUNDEF) && (controls[i-1].type != CONTUNDEF))
		    holes ++;
	    }
	}
    }

    fprintf (stderr, "          %d used (%ld %% = %ld bytes)\n", 
		    nb,
		    ((long)nb * 100) / (long) MAXNBCONTROLS,
		    (long)nb * sizeof (Controls));
    fprintf (stderr, "          %d holes\n\n", holes);

    
    fprintf (stderr, "Contexts: %ld x %ld bytes = %ld\n", (long) MAXCONTEXTS, (long) sizeof (Contexts), (long) sizeof (contexts));
    nb = 0;
#ifdef MEGASTATIC
    totdivs = 0;
#endif
    for (i=0 ; i<MAXCONTEXTS ; i++)
    {	if (contexts[i].type != CONTCTXFREE)
	{
	    nb ++;
#ifdef MEGASTATIC
	    totdivs += (long) sizeof (contexts[i].whichcontrols);
#endif
	}
    }
    fprintf (stderr, "          %d used\n", nb);
    fprintf (stderr, "          inside wichcontrols : %ld bytes\n\n", totdivs);

    fprintf (stderr, "freezedlist: %ld bytes\n", (long) sizeof (freezedlist));
    fprintf (stderr, "tabwhichcontrols: %d x %ld bytes = %ld bytes\n", MAXCONTEXTS, (long) sizeof (tabwhichcontrols[0]), (long) sizeof (tabwhichcontrols));
}
#endif
