#include <stdio.h>

int main (void)
{
	char c;

	while ((c = fgetc (stdin)) != 'q')
		fprintf (stdout, "%c %20x\n", (c & 0x7f)>32 ? (c&0x7f) : ' ', c);
	return 0;

}
