#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

char mycurpath[1024];
char *lmycurpath = NULL;

int isagooddir (const struct dirent * thedirenttocheck)
{	
	struct stat bufstat;

	if (lmycurpath != NULL)
	{	strcpy (lmycurpath, thedirenttocheck->d_name);
		stat (mycurpath, &bufstat);
	}
	else
		stat (thedirenttocheck->d_name, &bufstat);

	if (S_ISDIR(bufstat.st_mode))
		return 1;
	else
		return 0;
}

char ** matchesforisagoodfile = NULL;

int isagoodfile (const struct dirent * t)
{	
	char **p = matchesforisagoodfile;
	struct stat bufstat;

	if (lmycurpath != NULL)
	{	strcpy (lmycurpath, t->d_name);
		stat (mycurpath, &bufstat);
	}
	else
		stat (t->d_name, &bufstat);

	if (S_ISREG(bufstat.st_mode))
	{	if (p == NULL) return 1;
		else
			while ((*p != NULL) && (strcmp (t->d_name+strlen(t->d_name)-strlen(*p), *p) != 0)) p++;
			if (*p == NULL) 
				return 0;
			else
				return 1;
	}
	else
		return 0;
}

int main (int nb, char ** cmde)
{

	int i;

	for (i=1 ; i<nb ; i++)
	{	int j, nb;
		struct dirent **namelist;
		char *tomatch [] = {".wav", ".WAV", ".Wav", ".c", NULL};

		strcpy (mycurpath, cmde[i]);
		strcat (mycurpath, "/");
		lmycurpath = mycurpath + strlen(mycurpath);

		nb = scandir (cmde[i], &namelist, isagooddir, alphasort);
		if (nb == -1)
			fprintf (stderr, "scandir a merdu errno: %d\n", errno);
		else
			for (j=0; j<nb ; j++)
			{
				printf ("%s/\n", namelist[j]->d_name);
			}

		matchesforisagoodfile = tomatch;
		nb = scandir (cmde[i], &namelist, isagoodfile, alphasort);
		if (nb == -1)
			fprintf (stderr, "scandir a merdu errno: %d\n", errno);
		else
			for (j=0; j<nb ; j++)
			{
				printf ("%s\n", namelist[j]->d_name);
			}
	}
	return 0;
}
