/* 
 * $Id: groovit.c,v 1.2 2003/03/19 16:45:47 jd Exp $
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
 * $Log: groovit.c,v $
 * Revision 1.2  2003/03/19 16:45:47  jd
 * major changes everywhere while retrieving source history up to this almost final release
 *
 * Revision 1.2.0.24.0.3  2000/10/01 21:07:03  jd
 * added Dsp button bare release
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
 * Revision 1.2.0.23.0.7  1999/10/11 15:05:55  jd
 * new led-rampe, just to try
 *
 * Revision 1.2.0.23.0.5  1999/10/05 23:04:46  jd
 * adaptation to customised on-the-fly play-ahead
 *
 * Revision 1.2.0.23.0.3  1999/09/30 01:09:04  jd
 * added -rc option
 *
 * Revision 1.2.0.23.0.2  1999/09/27 00:46:09  jd
 * almost complete signal generation moved to appropriate module
 *
 * Revision 1.2.0.23  1999/09/15 10:20:39  jd
 * second pre tel-aviv public revision, for testing
 *
 * Revision 1.2.0.22.0.7  1999/09/15 09:23:30  jd
 * split in more modules
 * added mono 16 bit handling for fake Maestro driver at OSS
 * added handling of intermediate mono channel for mixing (dyfilter)
 * added an hack for 2.2.x hanging write with timer (for gpm mouse) :
 * non-self-interrupted writes
 *
 * Revision 1.2.0.22.0.5  1999/09/05 22:30:16  jd
 * splitted in several modules more :
 * granalogik, grdyfilter and resoanlog
 * added a fakedsp mode in runtime
 * added a too much error detection that switch to fakedsp
 *
 * Revision 1.2.0.22.0.4  1999/08/30 22:16:03  jd
 * pattern part split to grpattern module
 * fileselect part split to grsel module
 * board and menu part split to...
 *                       ...board_menu module !
 * added a lot of shared inclusions
 *
 * Revision 1.2.0.22.0.3  1999/08/29 22:49:01  jd
 * split for fileselect in gsel
 * split for raw filename in grsel
 *
 * Revision 1.2.0.22.0.2  1999/08/29 16:58:26  jd
 * split with alerts in dialog.c for consistancy
 * added fake settings for testing Tactionne first release
 * added file loading at start
 * added cleaning before loading new song
 *
 * Revision 1.2.0.21.0.7
 * corrected bug with 2.2.x kernels and big write auto interrupted
 * added no sliced write possibility
 * added polling time duration for preserving cpu when idle
 * corrected handling of songs with more pattern rows
 * added refreshing screen after "new-song"
 * corrected sample name atttribution after "new-song"
 * corrected song name after "new-song"
 * added short song name in win-title
 * added sound card capabilities checking during "Configure/make config"
 * added 8 bits sound card handling (so poor...)
 * corrected labels on led bars. :)
 * added leds in for pattern sync
 * added scrolling inside patterns
 * corrected pattern positioning when loading/newing song
 *
 */

#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>

#include <jack/jack.h>
#include <jack/midiport.h>

#include <linux/soundcard.h>
/* #include <sys/soundcard.h> */

#include "config.h"

#ifdef USEERRNOH
#include <errno.h>
#endif

#ifdef ALARMHANDLER
#include <signal.h>
#include <sys/time.h>
#endif

#ifdef HANDLE_JOY
#include <linux/joystick.h>
#endif

#include "placement.h"
#include "tokenio.h"
#include "grov_io.h"
#include "raw2disk.h"
#include "sample.h"
#include "readsample.h"
#include "controls.h"
/* #include "fileselect.h" */
#include "menu_board.h"
#include "settings.h"
#include "dialogs.h"
#include "grsel.h"
#include "grmixeur.h"
#include "grpattern.h"
#include "granalogik.h"
#include "grdyfilter.h"
#include "grdelay.h"
#include "grderiv.h"

#define INMAINGROOVIT
#include "groovit.h"

#define JDMAXARGV	64	/* nombre d'arguments retransmis en sous shell */

/* =============================== debut des variables =========================== */

static char *rcsid = "$Id: groovit.c,v 1.2 2003/03/19 16:45:47 jd Exp $";

#ifndef MAXERRTOLERATE
#define MAXERRTOLERATE 320	/* nombre d'erreurs cumulees autorise     */
#endif

#define STDPOLLTIME 100
#define FAKEPOLLTIME 100000
#define LONGPOLLTIME 500000
#ifdef FAKEDSP
	/* comme on ne fait pas de calcul, on attends */
	/* as nothing's computed, we must wait some time */
#define MINIPOLLTIME FAKEPOLLTIME
#else
	/* on se presse quand on fait du calcul, on n'attends pas ! */
	/* when we compute we aint wait !! */
#define MINIPOLLTIME 0
#endif

#ifdef ALARMHANDLER
static
struct itimerval polltime =
{
    {0, STDPOLLTIME},
    {0, STDPOLLTIME}},	       /* le timer de recharge polling */
        stoptime =
{
    {0, 0},
    {0, 0}};		       /* arret du timer               */

#endif
int     minipolltime = MINIPOLLTIME;

#ifdef USELOCKMEM
int     uselockmem = USELOCKMEM;

#else
int     uselockmem = 0;

#endif

char   *barlevel[17] =
#ifdef NONOPTICAL
{
    "\033[7m\033[2m-\033[m---------------",
    "\033[7m\033[2m--\033[m--------------",
    "\033[7m\033[2m---\033[m-------------",
    "\033[7m\033[2m----\033[m------------",
    "\033[7m\033[2m-----\033[m-----------",
    "\033[7m\033[2m------\033[m----------",
    "\033[7m\033[2m-------\033[m---------",
    "\033[7m\033[2m--------\033[m--------",
    "\033[7m\033[2m---------\033[m-------",
    "\033[7m\033[2m----------\033[m------",
    "\033[7m\033[2m-----------\033[m-----",
    "\033[7m\033[2m------------\033[m----",
    "\033[7m\033[2m-------------\033[m---",
    "\033[7m\033[2m--------------\033[m--",
    "\033[7m\033[2m---------------\033[m-",
    "\033[7m\033[2m----------------\033[m",
    "\033[7m\033[2m--------------**\033[m",
}
#else
{
    "\033[1m\033[32m·\033[m              ",
    "\033[1m\033[32m\016`\033[m              \017",
    "\033[1m\033[32m\016``\033[m             \017",
    "\033[1m\033[32m\016```\033[m            \017",
    "\033[1m\033[32m\016````\033[m           \017",
    "\033[1m\033[32m\016`````\033[m          \017",
    "\033[1m\033[32m\016`````\033[33m`\033[m         \017",
    "\033[1m\033[32m\016`````\033[33m``\033[m        \017",
    "\033[1m\033[32m\016`````\033[33m```\033[m       \017",
    "\033[1m\033[32m\016`````\033[33m````\033[m      \017",
    "\033[1m\033[32m\016`````\033[33m````\033[31m`\033[m     \017",
    "\033[1m\033[32m\016`````\033[33m````\033[31m``\033[m    \017",
    "\033[1m\033[32m\016`````\033[33m````\033[31m```\033[m   \017",
    "\033[1m\033[32m\016`````\033[33m````\033[31m```¤\033[m  \017",
    "\033[1m\033[32m\016`````\033[33m````\033[31m```¤¤\033[m \017",
    "\033[1m\033[32m\016`````\033[33m````\033[31m```¤¤¤\033[m\017",
    "\033[1m\033[32m\016`````\033[33m````\033[31m``\033[41m`¤¤¤\033[m\017"
}
#endif
       ;
char    ramppos1[15],	       /* les chaines de postionnement des diodes */
        ramppos2[15], ramppos3[15], ramppos4[15];

static int old_d = ROWRYTHM;
void    allumediode (int d)
{
    d += ROWRYTHM;

    printf ("\033[%d;%dH\033m ", LINELEGEND, old_d);
    printf ("\033[%d;%dH\033[1m\033[32m\016`\033[m\017", LINELEGEND, d);
    old_d = d;
}

void rampe (int l, int r)
/*
{
    int i;

    char gauche[4] = "~mlt",
	 milieu[4] = " vwn",
	 droite[4] = " jku";

    printf ("\016\033[1m\033[32m");
    for (i=0 ; i<15 ; i++)
    {	if (i==5)
	    printf ("\033[33m");
	if (i==9)
	    printf ("\033[31m");
	if (l>r)
	{   if (l<i)
		printf (" ");
	    else
		if (l==i)
		    printf ("p");
		else
		    if (r>=i)
			printf ("=");
		    else
			printf ("p");
	}
	if (l==r)
	{   if (l<i)
		printf (" ");
	    else
		if (l==i)
		    printf ("=");
		else
		    printf ("=");
	}
	if (l<r)
	{   if (r<i)
		printf (" ");
	    else
		if (r==i)
		    printf ("r");
		else
		    if (l>=i)
			printf ("=");
		    else
			printf ("r");
	}
    }
    printf ("\033[m\017");
}
*/
{
    int i;

    printf ("\016\033[1m\033[32mt");
    for (i=1 ; i<15 ; i++)
    {	if (i==5)
	    printf ("\033[33m");
	if (i==9)
	    printf ("\033[31m");
	if (l>r)
	{   if (l<i)
		printf (" ");
	    else
		if (l==i)
		    printf ("j");
		else
		    if (r>=i)
			printf ("n");
		    else
			printf ("v");
	}
	if (l==r)
	{   if (l<i)
		printf (" ");
	    else
		if (l==i)
		    printf ("u");
		else
		    printf ("n");
	}
	if (l<r)
	{   if (r<i)
		printf (" ");
	    else
		if (r==i)
		    printf ("k");
		else
		    if (l>=i)
			printf ("n");
		    else
			printf ("w");
	}
    }
    printf ("\033[m\017");
}

/* 
 * ---------------------------------- integer part -----------------------------------
 */

inline int iabs (int i)
{
    return (i < 0) ? -i : i;
}

/* 
 * ---------------------------------- DSP and MIXER part -----------------------------
 */
void    setvolume (int fmixer, int channel, int vol)
{
    vol += (vol << 8);
#ifdef DEB_MIXER
#ifdef SOUND_DEVICE_NAMES
    fprintf (stderr, "setting channel %s volume to %d ", channelsname[channel], vol & 0xFF);
#else
    fprintf (stderr, "setting channel %d volume to %d ", channel, vol & 0xFF);
#endif
#endif
    if (ioctl (fmixer, MIXER_WRITE (channel), &vol) == -1)
    {
#ifdef SOUND_DEVICE_NAMES
	fprintf (stderr, "setting channel %s volume to %d ", channelsname[channel], vol & 0xFF);
#else
	fprintf (stderr, "setting channel %d volume to %d ", channel, vol & 0xFF);
#endif
	fprintf (stderr, "problematic ?\n");
    }
#ifdef DEB_MIXER
    else
	fprintf (stderr, "done.\n");
#endif
}

int     initmixer (void)
{
    int     fmixer = -1, allchannels = 0, recchannels = 0;

    fmixer = open (MIXER, O_WRONLY);

    if (fmixer == -1)
    {	int e = errno;
	fprintf (stderr, "impossible d'ouvrir %s : %s\n", MIXER, strerror (e));
	return (-1);
    }

/* getting list of all channels */
    if (ioctl (fmixer, SOUND_MIXER_READ_DEVMASK, &allchannels) == -1)
    {
	close (fmixer);
	if (errno == ENXIO)
	    return (-2);	/* no mix on this device */
	else
	    return (-1);
    }

/* getting list of recording channels */
    if (ioctl (fmixer, SOUND_MIXER_READ_RECMASK, &recchannels) == -1)
    {
	close (fmixer);
	return (-1);
    }

#ifdef ZEROINGINPUT
    {	int i;
/* zeroing all input devices */
	for (i = 0; i < 16; i++)	/* should use SOUND_MIXER_NRDEVICES ??? */
	{
	    if (recchannels & (1 << i))
/* JDJD            setvolume (fmixer, i, 50); */
		setvolume (fmixer, i, 0);
	}
    }
#endif

    if (allchannels & (1 << SOUND_MIXER_VOLUME))
	setvolume (fmixer, SOUND_MIXER_VOLUME, 100);

    if (allchannels & (1 << SOUND_MIXER_PCM))
	setvolume (fmixer, SOUND_MIXER_PCM, 100);

    if (allchannels & (1 << SOUND_MIXER_SPEAKER))
	setvolume (fmixer, SOUND_MIXER_SPEAKER, 30);

    close (fmixer);

    return (0);
}

int     initfakedsp (void)
{
    int     fdsp;

    fdsp = open ("/dev/null", O_WRONLY);
    return (fdsp);
}

int     initoutdsp (void)
{
#ifdef FAKEDSP
    int     fdsp;

    fdsp = open ("/dev/null", O_WRONLY);
    fprintf (stderr, "WARNING WERE RUNNING IN FAKEDSP DEFINED AT COMPILE TIME.\n");
    fprintf (stderr, "GROOVIT WILL PRODUCE NO SOUND.\n\n");
    return (fdsp);
#else
    int     fdsp,	       /* dsp file handle */
            reso = 16,	       /* 8 or 16 bit resolution */
            ster = 2,	       /* mono or stereo */
            rate = HOPEDSPRATE;		/* taux echantillonnage */

    fdsp = open (DSP, O_WRONLY);

    if (fdsp == -1)
    {
	int     e = errno;

	fprintf (stderr, "impossible d'ouvrir %s\n", DSP);
	switch (e)
	{
	    case EACCES:
		fprintf (stderr, "permission denied\n");
		break;
	    case EBUSY:
		fprintf (stderr, "device busy\n");
		break;
	}
	return (-1);
    }

  /* we always try 16 bits first */
    if (ioctl (fdsp, SOUND_PCM_WRITE_BITS, &reso) == -1)
    {
	fprintf (stderr, "ioctl merdu 16 bits\n");
	close (fdsp);
	return (-1);
    }
    if ((reso != 16) && (HOPEDRESO == 16))
    {
	fprintf (stderr, "this hardware doesn't handle 16 bits sound ??\n");
	fprintf (stderr, "groovit should be rebuild to handle your hardware.\n");
	close (fdsp);
	return (-1);
    }
    if ((reso == 16) && (HOPEDRESO < 16))
    {
	fprintf (stderr, "this hardware can handle 16 bits sound !!!\n");
	fprintf (stderr, "but this groovit binary only handles %d bits...\n", HOPEDRESO);
	fprintf (stderr, "groovit should be rebuild appropriatly to your hardware\n");
#ifndef ONESTCOOL
	close (fdsp);
	return (-1);
#endif
    }

    if (HOPEDRESO != 16)
    {
	reso = HOPEDRESO;
	if (ioctl (fdsp, SOUND_PCM_WRITE_BITS, &reso) == -1)
	{
	    fprintf (stderr, "ioctl merdu %d bits\n", HOPEDRESO);
	    close (fdsp);
	    return (-1);
	}
	if (reso != HOPEDRESO)
	{
	    fprintf (stderr, "this hardware doesn't handle neither 16bits or %dbits sound\n", HOPEDRESO);
	    fprintf (stderr, "you should try to rebuild groovit, maybe...\n");
	    close (fdsp);
	    return (-1);
	}
    }

/* setting stereo */
#ifndef HOPEDMONO16
    if (ioctl (fdsp, SOUND_PCM_WRITE_CHANNELS, &ster) == -1)
    {
	fprintf (stderr, "ioctl merdu stereo\n");
	close (fdsp);
	return (-1);
    }
    if (ster != 2)
    {
	fprintf (stderr, "I'm expecting stereo hardware ??\n");
	close (fdsp);
	return (-1);
    }
#else
    ster = 1;			/* mono ! */
    if (ioctl (fdsp, SOUND_PCM_WRITE_CHANNELS, &ster) == -1)
    {
	fprintf (stderr, "ioctl merdu mono\n");
	close (fdsp);
	return (-1);
    }
    if (ster != 1)
    {
	fprintf (stderr, "I'm expecting mono hardware ??\n");
	close (fdsp);
	return (-1);
    }
#endif

/* setting sample rate */
    if (ioctl (fdsp, SOUND_PCM_WRITE_RATE, &rate) == -1)
    {
	fprintf (stderr, "ioctl merdu sample rate\n");
	close (fdsp);
	return (-1);
    }
    if ((((rate - HOPEDSPRATE) * 100) / HOPEDSPRATE) > 5)	/* si le taux retourne est
								   diferent de plus de 5% */
    {
	fprintf (stderr, "rate handle is %d instead of the %d required??\n", rate, HOPEDSPRATE);
	fprintf (stderr, "groovit should be rebuild appropriatly to your hardware\n");
	return (-1);
    }
    return (fdsp);
#endif
}

int     resetdsp (int fdsp)
{
    return close (fdsp);
}

int     enterfakedsp (int fdsp)
{
    if (fdsp != -1)
	resetdsp (fdsp);
    minipolltime = FAKEPOLLTIME;
    fprintf (stderr, "entering fake dsp mode\n");
    return initfakedsp ();
}
int     leavefakedsp (int fdsp)
{
    resetdsp (fdsp);
    if ((fdsp = initoutdsp ()) == -1)
    {
	fprintf (stderr, "we stay in fake dsp mode\n");
	minipolltime = FAKEPOLLTIME;
	return initfakedsp ();
    }
    else
	minipolltime = MINIPOLLTIME;
    return fdsp;
}

/* 
 * ---------------------------------- Screen Part ------------------------------------
 */

#ifdef ALARMHANDLER
void    catchALRM (int sig)
{
    static int jdisdumb = 0;

    if (jdisdumb)
    {
	polltime.it_interval.tv_sec = 0;
	polltime.it_interval.tv_usec = STDPOLLTIME;
	setitimer (ITIMER_REAL, &polltime, NULL);
	return;
    }
    jdisdumb++;
    pollmouse ();
    polltime.it_interval.tv_sec = 0;
    polltime.it_interval.tv_usec = STDPOLLTIME;
    setitimer (ITIMER_REAL, &polltime, NULL);
    jdisdumb--;
  /* ben la on va poller regulierement quoi ... */
}
#endif

void    init_arbitr (void)
{
    abitr_pattern ();
    abitr_analogik ();
    abitr_dyfilter ();

    currowoffset = 0;
}

void    initallvoices (void)   /* exportee */
{
    reset_analogik ();
    reset_dyfilter ();
    reset_pattern ();		/* chargement des samples ET initialisation des patterns */
    reset_delay ();
    reset_deriv ();
    init_arbitr ();		/* mise a zero arbitraire de la position de jeu */
}

void    rustine (void)	       /* cette fonction initialise les variables globales en
			          attendant un systeme de chargement de defaut  Il faut
			          etudier ceux qui doivent etre fait de ceux qui sont globaux 

			        */
{
    allpower = BUT_ON;
    myrawingtodisk = 0;
    numoutbis = 0;
    foutbis = NULL;
    ts = 0;
    tss = 0;
    endtss = 0;
    nss = UNJBPM / 140;
    lnss = UNJBPM / 140;
    t = 0;
    totleft = 0;
    totright = 0;
    bpm = 140;
    instancename = gname;

    if (getcwd (startdir, MAXDIRSIZE) == NULL)
    {
	fprintf (stderr, "warning, cannot stat curent directory ??\n");
	strcpy (startdir, ".");
    }
    strcpy (cursongfile, "no-title");
    shortcursongfile = cursongfile;

#ifdef HOPEDMONO16
    pbuf = (unsigned short *) &reserve[0][0];
#endif
#ifdef HOPEDRESO16
    pbuf = (unsigned short *) &reserve[0][0];
#endif
#ifdef HOPEDRESO8
    pbuf = (unsigned char *) &reserve[0][0];
#endif
    maxleft = 0;
    maxright = 0;

    sprintf (ramppos1, "\033[%d;%dHl:", LINEDIV, ROWLEVEL);
    sprintf (ramppos2, "\033[%d;%dHr:", LINEDIV + 1, ROWLEVEL);
    sprintf (ramppos3, "\033[%d;%dHl:", LINEDIV + 2, ROWLEVEL);
    sprintf (ramppos4, "\033[%d;%dHr:", LINEDIV + 3, ROWLEVEL);
}

void    showusage (void)
{
    fprintf (stderr, "\nusage: groovit [-fakedsp] [-noX] [-rc `rcfile'] [-err`file_name']\n");
    fprintf (stderr,   "               [-raw `file_name.wav'] [... `startfile' ...]\n");
    fprintf (stderr,   " -rc      use rcfile instead of standard ones.\n");
    fprintf (stderr,   " -err     redirects stderr.\n");
    fprintf (stderr,   " -raw     attributes a filename and enable the raw ouput.\n");
    fprintf (stderr,   " -noX     disable forking into an Xtermish window when DISPLAY is set.\n");
    fprintf (stderr,   " -fakedsp disable dsp usage (useful for testing).\n");
    fprintf (stderr,   " -jack[=instance_name] jackd mode, being client with instance_name\n");
    fprintf (stderr,   "                       for multi-instance needs.\n");
    fprintf (stderr,   "          the last valid `startfile' is opened\n");
}

int     fdsp,	       /* dsp file handle pour ecriture                */
	nbconterr = 0;     /* nombre d'erreur cumulees                     */
#ifdef HANDLE_JOY
int     fjs1,	       /* joystick #1                                  */
	fjs2;	       /* joystick #2                                  */
#endif

#ifdef ALARMHANDLER
    struct sigaction signal_alrm;	/* structure de gestion du timer de polling     */
#endif

jack_port_t *output_portl, /* les sorties jackd                            */
	    *output_portr,
	    *midi_input;   /* l'entree midi                                */

jack_client_t *client;     /* le handle client aupres du serveur jackd     */

int usejack = 0;	   /* shall we use jack ?                          */
int leavenow = 0;	   /* should we leave groovit now (when usejack)   */

void jack_shutdown (void *arg)
{
    client = NULL;
}


void dumpmidi (FILE * std, jack_midi_event_t * event)
{   int j;
    fprintf (std, " midi @%15lu %ld ", (unsigned long) event->time, event->size);
    for (j=0 ; j<event->size ; j++) {
	fprintf (std, "%02x ", ((unsigned char *)event->buffer)[j]);
	if (j == 0)
	    fprintf (std, "(%1x %02d) ",(0xf0 & ((unsigned char *)event->buffer)[j]) >> 4, (0x0f & ((unsigned char *)event->buffer)[j]) );
    }
    fprintf (std, "\n");
}

int     mainloop (jack_nframes_t nframes, void *jack_arg)
{

    int     i,		       /* compteur a tout faire                        */
            voice;	       /* compteur de voix                             */

    int     lastpressed = -1;  /* dernier controle presse                      */
#ifdef HANDLE_JOY
    struct JS_DATA_TYPE js;    /* buffer de lecture des joysticks              */
#endif

    jack_default_audio_sample_t *jackoutr =
	(jack_default_audio_sample_t *) jack_port_get_buffer (output_portr, nframes);
    jack_default_audio_sample_t *jackoutl =
	(jack_default_audio_sample_t *) jack_port_get_buffer (output_portl, nframes);

    if (1)
    {	jack_midi_event_t event;
	long i, n;
	void* midiin_buf = jack_port_get_buffer(midi_input, nframes);
	n = (long) jack_midi_get_event_count(midiin_buf);
	for (i=0 ; i<n ; i++) {
	    if (jack_midi_event_get (&event, midiin_buf, i) != ENODATA) {
		if (event.size > 0) {
		    int took_care = 0;
		    if ( (((unsigned char *)event.buffer)[0] & 0xf0 ) == 0xb0 )	{   /* a continuous midi control */
			int control = ((unsigned char *)event.buffer)[1];
			if ((control >= 1) && (control <=7)) {
			    actionnedirectjmeta (control, (((unsigned char *)event.buffer)[2]) << 1);	/* value are 7 bits, multiply them ... */
			    took_care = 1;
			}
			else if (control == 8) {
			    keydirectcontrols (focus, (((unsigned char *)event.buffer)[2]) << 1);	/* value are 7 bits, multiply them ... */
			    took_care = 1;
			}
		    } else if ( (((unsigned char *)event.buffer)[0] & 0xf0 ) == 0x90 ) { /* a pad begin sound */
			int channel = ((unsigned char *)event.buffer)[0] & 0x0f;
			int control = ((unsigned char *)event.buffer)[1];
			if ((channel == 0) && (control>=0x24) && (control<=0x2b)) {
fprintf (stderr, "next pattern : %d\n", control - 0x24);
			    pattpatbut.nncurpattern = control - 0x24;
			    took_care = 1;
			}
		    }
		    if (!took_care)
			dumpmidi (stderr, &event);
		}
	    }
	}
    }

    {
#ifdef HOPEDMONO16
	pbuf = (unsigned short *) &reserve[0][0];
#endif
#ifdef HOPEDRESO16
	pbuf = (unsigned short *) &reserve[0][0];
#endif
#ifdef HOPEDRESO8
	pbuf = (unsigned char *) &reserve[0][0];
#endif
	totleft = 0;
	totright = 0;
	maxleft = 0;
	maxright = 0;
	if (isplayin == BUT_ON)
	{
	    long theplayahead;
	    if (usejack)
		theplayahead = nframes;
	    else
		theplayahead = playahead;

	    for (tss = ts, endtss = theplayahead; endtss; endtss--, tss++, nss--)
	    {
		if (!nss)	/* la on passe d'un tick a l'autre */
		{
/* JDJDJDJD l'avenir dira si cette methode est bone... */
		    if (isreplaying) tickchange_play ();

		    voice = 0;

		    lnss = UNJBPM / bpm;
		    nss = lnss;
/* later (in a few) thoses functions would have been registered */
		    voice = tickchange_pattern (voice);
		    voice = tickchange_analogik (voice);
		    voice = tickchange_dyfilter (voice);
		    voice = tickchange_delay (voice);

		    t++;
		}

/* mise a zero des cumuls generaux */
		l = 0;
		r = 0;

/* ----------controles des amplitudes
   (envellopes)-------------------------------------------- */
/* JDJDJDJD remplacer par un generateur d'envellopes */

		if ((tss & AMPTESTMASK) == 0)
		{
		    for (i = 0; i < MAXANALOG; i++)
		    {
			AnVoice *anv = &anvoice[i];

			if (anv->reso.ampmode)
			{
//			    anv->reso.amp = (anv->reso.amp * (0x8000 - anv->reso.ctdecay)) >> 15;
			    anv->reso.amp = (anv->reso.amp * (0x7FFC - anv->reso.ctdecay)) >> 15;
			}
			else
			{
			    anv->reso.amp += 0x1 + anv->reso.amp;
			    if (anv->reso.amp > 0x10000)	/* we switch to decay mode */
			    {
				anv->reso.amp = 0x10000;
				anv->reso.ampmode = 1;
			    }
			}
#ifdef RELATFREQ
			anv->reso.freq = 10 + tabfreq[anv->freqbase2 + anv->reso.ctfreqcut2] + (((tabfreq[anv->freqbase2 + anv->reso.ctfreqcut] - tabfreq[anv->freqbase2 + anv->reso.ctfreqcut2]) * anv->reso.amp) >> 16);
#else
			anv->reso.freq = 10 + tabfreq[anv->reso.ctfreqcut2] + (((tabfreq[anv->reso.ctfreqcut] - tabfreq[anv->reso.ctfreqcut2]) * anv->reso.amp) >> 16);
#endif
		    }
		    for (i = 0; i < MAXDYFILT; i++)
		    {
			DyFilter *fil = &dyfilter[i];

			if (fil->reso.ampmode)
			{
			    fil->reso.amp = (fil->reso.amp * (0x7FFC - fil->reso.ctdecay)) >> 15;
			}
			else
			{
			    fil->reso.amp += 0x1 + fil->reso.amp;
			    if (fil->reso.amp > 0x10000)	/* we switch to decay mode */
			    {
				fil->reso.amp = 0x10000;
				fil->reso.ampmode = 1;
			    }
			}
			fil->reso.freq = 10 + tabfreq[fil->reso.ctfreqcut2] + (((tabfreq[fil->reso.ctfreqcut] - tabfreq[fil->reso.ctfreqcut2]) * fil->reso.amp) >> 16);
		    }
		}

		voice = 0;
		voice = generation_pattern (&l, &r, voice);	/* JDJDJD inliner plus tard */
		voice = generation_analogik (&l, &r, voice);	/* JDJDJD inliner plus tard */
		voice = generation_dyfilter (&l, &r, voice);	/* JDJDJD inliner plus tard */
		voice = generation_delay (&l, &r, voice);	/* JDJDJD inliner plus tard */
		voice = generation_deriv (&l, &r, voice);	/* JDJDJD inliner plus tard */

/* ------------ ecretage, pasteurisation et camembert ------------------------------ */

		if (l < -32766)
		    l = -32766;
		else if (l > 32767)
		    l = 32767;
		if (r < -32766)
		    r = -32766;
		else if (r > 32767)
		    r = 32767;
	    if (usejack) {
		*jackoutl ++ = ((jack_default_audio_sample_t) l)/32768.0;
		*jackoutr ++ = ((jack_default_audio_sample_t) r)/32768.0;
		absl = iabs ((short) l);
		absr = iabs ((short) r);
		totleft += absl;
		totright += absr;
		if (absl > maxleft)
		    maxleft = absl;
		if (absr > maxright)
		    maxright = absr;
	    } else {
#ifdef HOPEDMONO16
		absl = iabs ((short) l);
		absr = iabs ((short) r);
		*pbuf++ = ((l + r) >> 1);
#endif

#ifdef HOPEDRESO16
		absl = iabs ((short) (*pbuf++ = l));
#endif
#ifdef HOPEDRESO8
		absl = iabs ((short) l);
		*pbuf++ = (l >> 8) ^ 0x80;
#endif

#ifdef HOPEDRESO16
		absr = iabs ((short) (*pbuf++ = r));
#endif
#ifdef HOPEDRESO8
		absr = iabs ((short) r);
		*pbuf++ = (r >> 8) ^ 0x80;
#endif

		totleft += absl;
		totright += absr;
		if (absl > maxleft)
		    maxleft = absl;
		if (absr > maxright)
		    maxright = absr;
	    } /* else if usejack ....*/
	    }

	if (!usejack) {
#ifdef NEVERINTER		/* JDJDJDJD a enlever car c'est du debug... */
#ifdef ALARMHANDLER		/* stopping the timer */
	    if (usetimer)
	    {
		setitimer (ITIMER_REAL, &stoptime, NULL);
		signal_alrm.sa_flags = SA_NODEFER;
		signal_alrm.sa_handler = catchALRM;
		sigemptyset (&signal_alrm.sa_mask);
		sigaction (SIGALRM, &signal_alrm, NULL);
	    }
#endif
#endif
#ifdef SLICEDOUT		/* on ecrit par petits morceaux car en 2.2.x les gros writes */
	  /* ne passent plus avec un timer en usage */
	    {
		long    l;

		for (l = 0; l < sizebufahead; l += SLICEDOUT)
		{
#ifdef NEVERINTER		/* JDJDJDJD a enlever car c'est du debug... */
#ifdef ALARMHANDLER		/* stopping the timer */
/* alors la c'est le ponpon !!! */
		    pollmouse ();
#endif
#endif
		    while (write (fdsp, l + (char *) &reserve[0][0], SLICEDOUT) != SLICEDOUT)
		    {
			switch (errno)
			{
			    case EINTR:
				continue;
			    default:
				nbconterr += 2;
/* fprintf (stderr, "write: offset %ld errno %d %s\n", l, errno, strerror(errno)); */
				fprintf (stderr, "write: offset %ld (tot:%ld) errno %d %s\n", l, sizebufahead, errno, strerror (errno));
				if (nbconterr > MAXERRTOLERATE)
				{
				    fprintf (stderr, "too much spooled errors...\n");
				    fdsp = enterfakedsp (fdsp);
				    nbconterr = 0;
				}
			}
			break;
		    }
		    if (nbconterr > 0)
			nbconterr--;
		}
	    }
#else
	    while (write (fdsp, &reserve[0][0], sizebufahead) != sizebufahead)
	    {
		switch (errno)
		{
		    case EINTR:
			continue;
		    default:
			nbconterr += 2;
			fprintf (stderr, "write: offset %ld errno %d %s\n", (long) l, errno, strerror (errno));
/* pdebug (errno, strerror(errno));     */
			if (nbconterr > MAXERRTOLERATE)
			{
			    fprintf (stderr, "too much spooled errors...\n");
			    fdsp = enterfakedsp (fdsp);
			}
		}
		break;
	    }
	    if (nbconterr > 0)
		nbconterr--;
#endif
#ifdef NEVERINTER		/* JDJDJDJD a enlever car c'est du debug... */
#ifdef ALARMHANDLER
	    if (usetimer)
	    {
		polltime.it_interval.tv_sec = 0;
		polltime.it_interval.tv_usec = STDPOLLTIME;
		setitimer (ITIMER_REAL, &polltime, NULL);
		signal_alrm.sa_flags = SA_NODEFER;
		signal_alrm.sa_handler = catchALRM;
		sigemptyset (&signal_alrm.sa_mask);
		sigaction (SIGALRM, &signal_alrm, NULL);
	    }
#endif
#endif
	    if (myrawingtodisk)
	    {
		while (fwrite (&reserve[0][0], sizebufahead, 1, foutbis) != 1)
		{
		    switch (errno)
		    {
			case EINTR:
			    continue;
			case ENOSPC:
			    closewaveout (foutbis);
			    foutbis = NULL;
			    fprintf (stderr, "close the raw to disk because no more space left\n");
			default:
			    pdebug (errno, strerror (errno));
		    }
		    break;
		}
		if ((rawing2disk != BUT_ON) && (foutbis != NULL))
		{
		    myrawingtodisk = 0;
		    closewaveout (foutbis);
		    foutbis = genrawname (nfoutbis, &numoutbis);
		    if (foutbis == NULL)
			rawing2disk = BUT_DIS;
		    else
			rawing2disk = BUT_OFF;
		    refreshcontrols (hrawing2disk);
		}
	    }
	} /* if not usejack */

	  /* affichage de la diode de pattern */
#ifdef DIODEINPAT
	    allumediode (tp);
#endif

	  /* affichage level output */

	    totleft = (totleft / playahead) >> 11;
	    totleft = totleft > 15 ? 16 : totleft;
	    totright = (totright / playahead) >> 11;
	    totright = totright > 15 ? 16 : totright;
	    maxleft >>= 11;
	    maxleft = maxleft > 15 ? 16 : maxleft;
	    maxright >>= 11;
	    maxright = maxright > 15 ? 16 : maxright;

	    printf ("%s", ramppos3);
	    rampe (totleft,totright);
	    printf ("%s", ramppos4);
	    rampe (maxleft,maxright);
/*	    printf ("%s%s", ramppos1, barlevel[totleft]);
	    printf ("%s%s", ramppos2, barlevel[totright]);
	    printf ("%s%s", ramppos3, barlevel[maxleft]);
	    printf ("%s%s", ramppos4, barlevel[maxright]);
*/
	}
/********************* polling *********************************/

	scrflush ();

#ifdef HANDLE_JOY
	if ((fjs1 != -1) && (fjs2 != -1))
	{			/* on fait 4 mesures pour chaques histoire de faire une
				   moyenne */
	    int     x1 = 0, y1 = 0, n1 = 0, x2 = 0, y2 = 0 , n2 = 0;

	    if (read (fjs1, &js, JS_RETURN) == JS_RETURN)
		x1 += js.x, y1 += js.y, n1++;
	    if (read (fjs2, &js, JS_RETURN) == JS_RETURN)
		x2 += js.x, y2 += js.y, n2++;
	    if (read (fjs1, &js, JS_RETURN) == JS_RETURN)
		x1 += js.x, y1 += js.y, n1++;
	    if (read (fjs2, &js, JS_RETURN) == JS_RETURN)
		x2 += js.x, y2 += js.y, n2++;
	    if (read (fjs1, &js, JS_RETURN) == JS_RETURN)
		x1 += js.x, y1 += js.y, n1++;
	    if (read (fjs2, &js, JS_RETURN) == JS_RETURN)
		x2 += js.x, y2 += js.y, n2++;
	    if (read (fjs1, &js, JS_RETURN) == JS_RETURN)
		x1 += js.x, y1 += js.y, n1++;
	    if (read (fjs2, &js, JS_RETURN) == JS_RETURN)
		x2 += js.x, y2 += js.y, n2++;

	  /* JDJDJDJD */
	  /* ces affectations sont pour le moins arbitraires... */
	    if (n1 == 4) {
		dyfilter[0].reso.ctfreqcut = (256 * (x1 - 80)) / 1950;
		dyfilter[0].reso.ctfreqcut2 = (256 * (y1 - 80)) / 1950;
	    }
	    if (n2 == 4) {
		dyfilter[0].reso.ctreso = (256 * (y2 - 80)) / 1950;
		dyfilter[0].reso.ctdecay = (256 * (x2 - 80)) / 1950;
	    }
	}
#endif

	lastpressed = pollcontrols (isplayin == BUT_ON ? minipolltime : LONGPOLLTIME);
	if (allpower == BUT_OFF)
	{   if (usejack)
		leavenow = 1;
	    else
		return 1;	/* when it was inside a loop, it was :  break; */
	}

      /* because of its immediate action on outputs, it is kept here */
	if ((rawing2disk != BUT_OFF) && (foutbis != NULL))
	    myrawingtodisk = 1;

	if (opendsp != isopendsp)
	{
	    switch (opendsp)
	    {
		case BUT_OFF:   // on doit fermer le dsp
		    fdsp = enterfakedsp (fdsp);
		    nbconterr = 0;
		    isopendsp = opendsp;
		    refreshcontrols (hopendsp);
		    break;

		case BUT_ON:	// on doit ouvrir le dsp
		    fdsp = leavefakedsp (fdsp);
		    if (fdsp == -1)
		    {
			fdsp = enterfakedsp (fdsp);
			opendsp = BUT_OFF;
		    }
		    isopendsp = opendsp;
		    refreshcontrols (hopendsp);
		    break;

		default:
		    fprintf (stderr, "warning: internal error for treating \"opendsp\", continuing anyway\n");
		    isopendsp = opendsp;
	    }
	}
    }
    return 0;
}

int     main (int nbcm, char **cmde)
{
#ifdef RINGMODULATOR
/* RING MODULATOR TEST */
    int     lring = 1, rring = 1;

#endif

    int     forcedfakedsp = 0, /* mode fakedsp force                           */
            passe,	       /* compteur de passe pour la ligne de commande  */
            i;		       /* compteur a tout faire                        */

    int     noX = 0,	       /* on ne veut pas forker de X                   */
            usage = 0,	       /* afficher l'usage                             */
            letsreadfile = 0;  /* on nous demande de lire un fichier           */


    char   *fixedpos = NULL;   /* coordonnees de la fenetre en position fixee  */
    char   *nftobeload = NULL; /* le fichier a lire au demarrage               */
    char   *rcfile = NULL;     /* le rcfile en cours (null=default)            */

#ifdef DDD_DEBUG
    fprintf (stderr, "debugging with ddd my PID is %d !\n", getpid ());
    getchar ();
#endif

  /* JDJDJDJD il faudrait enlever ca au plus vite ... */
    rustine ();
    nfoutbis[0] = 0;

/* analyse de la ligne de commande */
    for (passe = 0; passe < 2; passe++)
    {
      /* passe -1 : la plus prioritaire : redirection de stderr */
      /* passe  0 : rcfile overide ? */
      /* passe  1 : les affaire courantes */
	for (i = 1; i < nbcm; i++)
	{
	    switch (cmde[i][0])
	    {
		case '-':
		    if (strncmp (cmde[i], "--h", 3) == 0)
		    {
			usage = 1;
		    }
		    else if (strncmp (cmde[i], "-h", 2) == 0)
		    {
			usage = 1;
		    }
		    else if (strncmp (cmde[i], "-noX", 4) == 0)
		    {
			if (passe == 1)
			    noX = 1;
		    }
		    else if (strncmp (cmde[i], "-err", 4) == 0)
		    {
			if (passe == 0) {
			    if (freopen (cmde[i] + 4, "a", stderr) == NULL) {
				int e = errno;
				fprintf (stderr, "could not open %s : %s\n", cmde[i] + 4, strerror(e));
			    }
			}
		    }
		    else if (strncmp (cmde[i], "-rc", 3) == 0)
		    {
			if (passe == 0)
			    rcfile = cmde[i + 1];
			i++;
		    }
		    else if (strncmp (cmde[i], "-raw", 4) == 0)
		    {
			if (passe == 1)
			{
			    strcpy (nfoutbis, cmde[i + 1]);
			    foutbis = genrawname (nfoutbis, &numoutbis);
			    if (foutbis == NULL)
				rawing2disk = BUT_DIS;
			    else
				rawing2disk = BUT_OFF;
			}
			i++;
		    }
		    else if (strncmp (cmde[i], "-jack", 5) == 0)
		    {
			usejack = 1;
			if (cmde[i][5] == '=') {
			    if (strlen (cmde[i]+6) == 0) {
				fprintf (stderr, "error : \"-jack=name\" needs a name\n");
				exit (1);
			    } else {
				instancename = cmde[i]+6;
			    }
			}
		    }
		    else if (strncmp (cmde[i], "-fakedsp", 8) == 0)
		    {
			if (passe == 1)
			    forcedfakedsp = 1;
		    }
		    else if (strncmp (cmde[i], "-fixedpos", 9) == 0)
		    {
			if (passe == 1)
			    fixedpos = cmde[i + 1];
			i++;
		    }
		    else if (passe == 1)
			fprintf (stderr, "unrecognised option %s\n", cmde[i]);
		    break;
		default:
		    if (passe == 1)
		    {
			letsreadfile++;
			if (isreallyafile (cmde[i]) == 0)
			{
			    if (nftobeload == NULL)
				fprintf (stderr, "let's load %s to start...\n", cmde[i]);
			    else
				fprintf (stderr, "no finally, i'll load %s instead of %s\n", cmde[i], nftobeload);
			    nftobeload = cmde[i];
			}
		    }
	    }
	}
	if (passe == 0)
	{
	    if (init_grov_io (rcsid))
		exit (-1);
	    reset_settings (rcfile);
	}
    }

#ifndef CCUNAMED
#error possibly incorrect use of Makefile
#endif
    fprintf (stderr, "%s\n", rcsid);
    fprintf (stderr, "configured on %s by %s\n", UNAMED, CONFEDBY);
    fprintf (stderr, "  compiled on %s by %s\n", CCUNAMED, CCCONFEDBY);
    if (usage)
    {
	showusage ();
	exit (0);
    }
    if (letsreadfile && (nftobeload == NULL))
    {
	fprintf (stderr, "none of the %d files could be read, quiting.\n", letsreadfile);
	exit (-1);
    }

/* on teste une premiere fois le dsp */
    if (!forcedfakedsp)
    {
	if (usejack) {
	    if ((client = jack_client_new (instancename)) == NULL) {
		fprintf (stderr, "could not connect to jackd server\n");
	    }
	    jack_client_close (client);
	    client = NULL;
	} else {
	    fdsp = initoutdsp ();
	    if (fdsp == -1)
	    {
		exit (1);
	    }
	    resetdsp (fdsp);
	}
    }

    if ((getenv ("DISPLAY") != NULL) && !noX)
    {
	char   *gnob = XTERMCMD, *xtermcmd = NULL, *morearg = NULL, *argv[JDMAXARGV], localbuf[1024],
	       *p, *q;
	int     argc = 0;

	xtermcmd = malloc (strlen (gnob) + 2);
	if (xtermcmd == NULL)
	{
	    fprintf (stderr, "unable to alloc 'xtermcmd'... no more memory ??? big shame !\n");
	    exit (-1);
	}
	strcpy (xtermcmd, gnob);
	q = p = xtermcmd;
	while (*p != 0)
	{
	    if ((*p == ' ') && (argc < JDMAXARGV - 4))
	    {
		argv[argc++] = q;
		*p = 0;
		q = p + 1;
	    }
	    p++;
	}
	if (isatty (2))		/* isatty (stderr) */
	{
	    morearg = malloc (strlen (ttyname (2)) + 6);
	    if (morearg == NULL)
	    {
		fprintf (stderr, "unable to alloc 'morearg'... no more memory ??? big shame !\n");
		exit (-1);
	    }
	    strcpy (morearg, "-err");
	    strcat (morearg, ttyname (2));
	}
	argv[argc++] = "-geometry";
	argv[argc] = localbuf;
	sprintf (argv[argc], "%dx%d", 80 + MAXDISPLAYROW - 32, 5 + MAXSAMPLE + MAXANALOG * 2 + MAXDYFILT * 2);
	if (fixedpos != NULL)
	{
	    strcat (argv[argc], "+");
	    strcat (argv[argc], fixedpos);
	}
	argc++;
      /* argv [argc++] = "80x24"; *//* JDJDJD add a modulable size */
	argv[argc++] = "-e";
	/* tries to check around ldpreload : LD_PRELOAD=libpulsedsp.so */
	if (getenv("LD_PRELOAD") != NULL) {
	    fprintf (stderr, "some preloaded libraruies ? let's check ...\n");
	    char * ldpreload = getenv("LD_PRELOAD");
	    if (strstr (ldpreload, "pulse") != NULL) {
		fprintf (stderr, "seems to be using pulse tweaking, will fork with padsp\n");
		unsetenv ("LD_PRELOAD");
		argv[argc++] = "padsp";
	    }
	}
	argv[argc++] = cmde[0];
	argv[argc++] = "-noX";
	if (morearg != NULL)
	    argv[argc++] = morearg;
	for (i = 1; i < nbcm; i++)
	    argv[argc++] = cmde[i];
	argv[argc++] = NULL;
      /* {    int i; for (i=0 ; i<argc-1 ; i++) printf ("%s ",argv[i]); printf("\n"); } */
	execvp (argv[0], argv);
	fprintf (stderr, "execlp failed, trying to continue anyway ...\n");
	free (morearg);
	free (xtermcmd);
    }

  /* this is for avoiding the stderr output over the stdout !!!! */
    {
	if (isatty (2) && isatty (1))
	{
	    char   *a = malloc (strlen (ttyname (1)) + 1);

	    if (a == NULL)
		fprintf (stderr, "warning: allocation problems ???\n");
	    else
		strcpy (a, ttyname (1));
	    if (strcmp (a, ttyname (2)) == 0) {
		if (freopen ("/dev/null", "a", stderr) == NULL) {
		    int e = errno;
		    fprintf (stderr, "could not open %s : %s\n", "/dev/null", strerror(e));
		}
	    }
	    free (a);
	}
    }

#ifdef HANDLE_JOY
    fjs1 = open (F__JS1, O_RDONLY);
    if (fjs1 == -1)
    {
	switch (errno)
	{
	    case ENODEV:
		fprintf (stderr, "%s isn't handled by your kernel, continuing without it.\n\ncheck the joystick/Jbox part of groovit man-page\n\n", F__JS1);
		break;
	    default:
		fprintf (stderr, "could not open %s, continuing without joysticks\n", F__JS1);
	}
    }
    fjs2 = open (F__JS2, O_RDONLY);
    if (fjs2 == -1)
    {
	switch (errno)
	{
	    case ENODEV:
		fprintf (stderr, "%s isn't handled by your kernel, continuing without it.\n\ncheck the joystick/Jbox part of groovit man-page\n\n", F__JS2);
		break;
	    default:
		fprintf (stderr, "could not open %s, continuing without joysticks\n", F__JS2);
	}
    }
#endif

/* on met a jour les niveaux de sortie */

    switch (initmixer ())
    {
	case -1:
	    fprintf (stderr, "probleme avec le mixer\n");
	    break;
	case -2:
	    fprintf (stderr, "no mixer available on this device\n");
	    break;
	default:
	    ;
    }


/* initialisation du lock des pages en memoire */
/* JDJDJD pourrait eventuellement etre affine ??? */
/* devrait etre liberer des que l'on ne joue pas */
    if (uselockmem)
    {
	if (mlockall (MCL_FUTURE))
	    switch (errno)
	    {
		case ENOMEM:
		    fprintf (stderr, "could not lock memory (too much mem) will continue without lock\n");
		    break;
		case EPERM:
		    fprintf (stderr, "this process cannot lock memory (need to be suid root) will continue without locking\n");
		    break;
		case EINVAL:
		default:
		    fprintf (stderr, "bad value transmitted to memory-lock or unknown error, I'm trying to ignore...\n");
	    }
    }

/* initialisation des controls */
    {
	char   *err1 = "no mouse connection ?\nending.\n", *err2 = "unknown error in initcontrols ?\nending.\n";

	switch (initcontrols ())
	{
	    case 0:
		break;
	    case -2:		/* on ecrit stdout ET stderr au cas stderr=/dev/null */
		fprintf (stderr, "%s", err1);
		fprintf (stdout, "%s", err1);
		exit (1);
	    default:
		fprintf (stderr, "%s", err2);
		fprintf (stdout, "%s", err2);
		exit (1);
	}
    }
    if (ftok_init ())
    {
	resetcontrols ();
	exit (1);
    }
    if (init_grov_io (rcsid))
    {
	resetcontrols ();
	exit (1);
    }

/* mise a zero initiale : analog part -------------------------------------------------- */
    for (i = 0; i < 257; i++)
	tabfreq[i] = pow (2, (double) (i - 255) / 24.0) * LA_MAX_JHZ;

    initallvoices ();
    if (nftobeload != NULL)
    {
	fprintf (stderr, "loading song %s to current\n", nftobeload);
	if (load_cur_song (nftobeload, 1) != 0)
#ifndef NOFILECONTINUES
	{
	    fprintf (stderr, "could not load song %s, quiting\n", nftobeload);
	    resetcontrols ();
	    exit (1);
	}
#else
	{
	    fprintf (stderr, "error song %s not loaded !\n", nftobeload);
	  /* JDJDJDJD come sanity jobs needed here !!! */
	    strcpy (cursongfile, "no-title");
	    shortcursongfile = cursongfile;
	    settitle (instancename, shortcursongfile);
	}
#endif
	else
	{
	    wasmodified = 0;
	    strcpy (cursongfile, nftobeload);
	    shortcursongfile = strrchr (cursongfile, '/');
	    if (shortcursongfile == NULL)
		shortcursongfile = cursongfile;
	    else
		shortcursongfile++;
	    settitle (instancename, shortcursongfile);
	}
    }

    if (forcedfakedsp)
    {
	fdsp = enterfakedsp (-1);
    }
    else
    {
	if (usejack) {
	    if ((client = jack_client_new (instancename)) == NULL) {
		fprintf (stderr, "could not connect to jackd server\n");
	    }
	    if (client != NULL) {
		jack_set_process_callback (client, mainloop, 0);
		jack_on_shutdown (client, jack_shutdown, 0);

		fprintf (stderr, "engine sample rate: %" PRIu32 "\n", jack_get_sample_rate (client));

		output_portl = jack_port_register (client, "outputl", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
		output_portr = jack_port_register (client, "outputr", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

		midi_input = jack_port_register (client, "midi_input", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
	    }
	} else {
	    fdsp = initoutdsp ();
	    if (fdsp == -1)
	    {
		resetcontrols ();
		exit (1);
	    }
	}
    }
    nbconterr = 0;

    settitle (instancename, shortcursongfile);
    init_mixeur ();
    initboard (rcsid);
    resync_displaypatterns ();

    initgrsel ();
    initdialogs ();
    initmenu ();
    initsettings ();

  /* JDJDJDJD this is probably not needed... */
    for (i = 0; i < MAXSAMPLE; i++)
	cursamp[i] = 0;

    if (fdsp == -1)		/* we set the button according to the mode */
	opendsp = BUT_OFF;
    else
	opendsp = BUT_ON;
    isopendsp = opendsp;

    pollcontrols (0);		/* to force a first redraw... */

    if (!gpl_accepted)
	alert_gpl ();

/*---------------------------------------------------------------------*/
/* boucle principale - main loop */

#ifdef ALARMHANDLER
    if (usetimer)
    {
	polltime.it_interval.tv_sec = 0;
	polltime.it_interval.tv_usec = STDPOLLTIME;
	setitimer (ITIMER_REAL, &polltime, NULL);
	signal_alrm.sa_flags = SA_NODEFER;
	signal_alrm.sa_handler = catchALRM;
	sigemptyset (&signal_alrm.sa_mask);
	sigaction (SIGALRM, &signal_alrm, NULL);
    }
#endif

    if ((kplayahead < 0) || (kplayahead > (MAXPLAYAHEAD >> 10)))
	kplayahead = (MAXPLAYAHEAD >> 10);

    playahead = (kplayahead << 10);
#ifdef HOPEDRESO16
    sizebufahead = (playahead << 2);
#endif
#ifdef HOPEDRESO8
    sizebufahead = (playahead << 1);
#endif
#ifdef HOPEDMONO16
    sizebufahead = (playahead << 1);
#endif

    if (usejack) {
	if (jack_activate (client)) {
	    fprintf (stderr, "cannot activate jack as client");
	}
    }

    leavenow = 0;
    {	useconds_t usleeptime = 100 * 1000;
	for (ts = 0; 1; ts += playahead) {
	    if (!usejack) {
		if (mainloop (0, NULL)) break;
	    } else {
		usleep (usleeptime);
		if (leavenow) break;
	    }
	}
    }

    if (foutbis != NULL)
	closewaveout (foutbis);
    resetdsp (fdsp);

  /* desallocation generale */
    for (i = 0; i < MAXSAMPLE; i++)
	freesample (&sample[i]);

#ifdef ALARMHANDLER		/* stopping the timer */
    if (usetimer)
    {
	setitimer (ITIMER_REAL, &stoptime, NULL);
	signal_alrm.sa_flags = SA_NODEFER;
	signal_alrm.sa_handler = catchALRM;
	sigemptyset (&signal_alrm.sa_mask);
	sigaction (SIGALRM, &signal_alrm, NULL);
    }
#endif
    resetcontrols ();
    exit (0);
}
