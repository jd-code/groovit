/*
 * tb.c - Roland TB303 Emulator
 * Lars Hamre, 1995
 *
 * Compiles with: sc tb.c link math=68881
 * Usage: tb >ram:sample
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*
 *   1  3     6  8 10    13 15    18 20 22    25 27    30 32 34
 *   c  d     f  g  a     c  d     f  g  a     c  d     f  g  a
 *  C  D  E  F  G  A  B  C  D  E  F  G  A  B  C  D  E  F  G  A  B  C
 *  0  2  4  5  7  9 11 12 14 16 17 19 21 23 24 26 28 29 31 33 35 36
 */

char Note[16]=
{
/*	24,23,19,17,19, 7,21,22, 10,20, 8,18,17,8,20,12	*/
	7,7,7,7,5,5,8,8,4,3,7,0,7,9,10,8
};

/*
 * 0 = rest
 * 1 = 16th note
 * 2 = tie
 */

char NoteLength[16]=
{
	2,2,2,2,2,2,2,2, 2,0,0,0,1,1,1,1
};

/*
 * 0 = no accent
 * 1 = accent on
 *
 * Accent is not used yet, but increases volume and resonance
 */

char Accent[16] =
{
	0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0
};

/*
 * 0 = go instantly to the next note
 * 1 = slide to the new note
 */

char Slide[16] =
{
	1,0,0,1,0,0,0,0, 0,0,1,0,0,0,0,0
};

/*
 * This program is a mess of strange variable names and commented out code, but
 * it sort of works. It's only something to experiment with.
 */

void main(void)
{
	int tot=0, out, notenum=0;
	float vco2 = 0.0, vco = 4.0;
	float vcophase=0.8;
	float vcofreq = 1, lastvcofreq=0, vcoadd, vcofreq2=0;
	float hig=30.0, mid=8.0, low=5.0, freq = 9.9;
	float hig2=0, mid2=0, low2=0;

	// Note: resonance is "backwards" in this type of filter, so a low
	// value is max resonance. 

	float reso = 0.35;
	float feedbk, flevel=0.61;
	float amp = 0.1, inp;
	int emode = 2;
	//while (tot < 39147)
	while (tot < 90000)
	//while (tot < 32000)
	{

//---------- VCO ----------
// The squarewave rises faster than is falls, and that means that the
// rising edge is sharper and will have more resonance. I guess it is
// caused by the VCO being an integrator with a reset circuit for the
// sawtooth wave, and the squarewave is simply a "hard limited" version
// of the saw by feeding it through an op-amp comparator. This simulation
// only does squarewave, but a sawtooth would be: vcophase/128 - 1.

		if (vco > 0)
			vco2 = vco2 + (vco - vco2) * 0.80;
		else
		//      vco2 = vco2 + (vco - vco2) * 0.2;
			vco2 = vco2 + (vco - vco2) * 0.2;

// Highpass filter to get the "falling down to zero" look.
// This is probably not what really happens, because the 303 has lots of
// bass output - so I guess it is caused by the filter. It is just a hack
// to make the waveform look more like the original. I need a 303 sample
// without resonance!
//		{
//			static float delta=0;
//			delta += vco2;
//			delta = delta * 0.99;
//			inp = delta;
//			delta -= vco2;
//		}

		inp = vco2;

// Update VCO phase

		if (Slide[notenum])
			vcofreq2 = vcofreq2 + (vcofreq - vcofreq2) * 0.052;
		else
			vcofreq2 = vcofreq;
		if (vcofreq2 != lastvcofreq)
			vcoadd = pow(2.0, vcofreq2 + 1.20);
//		      	vcoadd = pow(2.0, vcofreq2 + 0.35);
		lastvcofreq = vcofreq2;
		vcophase += vcoadd;
		if (vcophase >= 256)
		{
			vcophase -= 256;
			vco = - vco;
		}
		if ((tot % 2000) == 0)
		{
			vcofreq = Note[notenum] /  6.0;
		}

//---------- VCF ----------

		// Initial cutoff freq + envelope amount
		freq = 0.08 + 0.8*amp;
		// reso *= 0.99995;
		feedbk = reso*mid;

		/**** Useless (?) hack to simulate diode limiting of resonance ****/
		if (feedbk >flevel)
		{
			float sq = (feedbk-flevel)*2.0;
			feedbk += sq*sq;
		}
		else if (feedbk < -flevel)
		{
			float sq = (feedbk+flevel)*2.0;
			feedbk -= sq*sq;
		}

		/******************************************************************/
		// 2 pole filter #1

		hig = inp - feedbk - low;
		mid += hig * freq;
		low += mid * freq;

		// 2 pole filter #2
		hig2 = low - 1*mid2 - low2;
		mid2 += hig2 * freq;
		low2 += mid2 * freq;

//---------- VCA ----------

		// Trig envelope attack
		if ((tot % 2000) == 0)
			if (NoteLength[notenum] == 1)
				emode = 0;

		// Trig envelope release
		if ((tot % 2000) == 1100)
			if (NoteLength[(notenum+1)%16] != 2)
				emode = 2;
		switch (emode)
		{
			case 0:
				// Attack state
				// Capacitor charge attack...
				//amp = amp + (1.1 - amp) * 0.01;

				// ... but the TB303 attack looks more like this

				amp = amp*1.1 + 0.01;
				if (amp >= 1.0)
				{
					amp = 1.0;
					emode = 1;
				}
				break;
			// Decay state - tweak as you like
			case 1:
				amp = amp * 0.9999;
				break;
			// Release state
			case 2:
				amp = amp * 0.99;
				break;
		}
		// Output is lowpass (try low or low2) multiplied by amplitude.
		// 40 seems like a nice value to avoid clipping with lots of
		// resonance.
		out = low2* amp * 40;

//-------- Output ----------

		// Clip to min/max values
		// Easy to tweak to 16 bit output instead of 8

		if (out > 127)
			out = 127;
		if (out < -128)
			out = -128;

		putc(out,stdout);
		tot++;

//------------------------

		// At the end of a note, start another one
		if ((tot % 2000) == 1999)
		{
			notenum++;
			if (notenum >= 16)
				notenum = 0;
		}
	}
}
