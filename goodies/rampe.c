#include <stdio.h>
#include <stdlib.h>
#include "valid-fn.h"

#define XTERM_MOUSE_ON  "\033[?1000h"   /* DECSET with parm 1000 */
#define XTERM_MOUSE_OFF "\033[?1000l"   /* DECRST with parm 1000  */

void rampe (int l, int r)
{
    int i;

    char gauche[4] = "~mlt",
	 milieu[4] = " vwn",
	 droite[4] = " jku";

    printf ("\016\033[32m");
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
}

int main (void)
{
    int l, r;
	printf ("\033)0\n");

printf ("\016\033[32mtnn\033[33mnw\033[31mwwk\033[m\017\n");
printf ("\016\033[32mtnn\033[33mww\033[31mk  \033[m\017\n");

    for (l=0 ; l<15 ; l++)
    for (r=0 ; r<15 ; r++)
    {
	rampe (l,r);
	printf ("\n");
	getchar ();
    }
}
