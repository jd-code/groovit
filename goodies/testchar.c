#include <stdio.h>
#include <stdlib.h>
#include "valid-fn.h"

#define XTERM_MOUSE_ON  "\033[?1000h"   /* DECSET with parm 1000 */
#define XTERM_MOUSE_OFF "\033[?1000l"   /* DECRST with parm 1000  */


int main (void)
{
	int i=0, j;
/*
while (cont_valid_font[i+1] != NULL)
{	printf ("\033]50;%s\007 %s\n", cont_valid_font[i], cont_valid_font[i]);
	getchar ();
	i++;
}*/
	printf ("\033)0\n");
	for (i=0 ; i<256 ; i+=16)
	if ((i&0x7f)>31)
	{   for (j=0 ; j<16 ; j++)
			printf ("%c:\016%c\017  ", i+j, i+j);
	    printf ("\n");
	}
	printf ("\n");

	for (j=0 ; j<8 ; j++)
	{
		for (i=0 ; i<8 ; i++)
			printf ("\033[%dm\033[4%dm %d\033[40m\033[m- ", j, i, i);
		for (i=0 ; i<8 ; i++)
			printf ("\033[%dm\033[3%dm %d\033[m - ", j, i, i);
		printf ("\n");
	}

	printf ("%s\n", XTERM_MOUSE_OFF);
	return 0;
}
