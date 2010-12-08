
/*
 * Groovit Copyright (C) 1998 Jean-Daniel PAUGET
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
 *  $Log: semaphore.c,v $
 *  Revision 1.2  2003/03/19 16:45:47  jd
 *  major changes everywhere while retrieving source history up to this almost final release
 *
 *  Revision 1.2.0.24.0.2  1999/10/19 19:29:45  jd
 *  *** empty log message ***
 *
 *  Revision 1.2.0.24.0.1  1999/10/11 18:36:29  jd
 *  *** empty log message ***
 *
 *  Revision 1.2.0.24  1999/10/11 15:30:51  jd
 *  second pre-tel-aviv revision
 *
 *  Revision 1.2.0.23.0.1  1999/09/15 10:35:48  jd
 *  *** empty log message ***
 *
 *  Revision 1.2.0.23  1999/09/15 10:20:39  jd
 *  second pre tel-aviv public revision, for testing
 *
 *  Revision 1.2.0.22.0.1  1999/08/25 09:17:49  jd
 *  Revision 1.1  1999/08/25 09:14:10  jd
 *  Initial revision
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <errno.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
/* union semun is defined by including <sys/sem.h> */
#else
/* according to X/OPEN we have to define it ourselves */
union semun {
        int val;                    /* value for SETVAL */
        struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
        unsigned short int *array;  /* array for GETALL, SETALL */
        struct seminfo *__buf;      /* buffer for IPC_INFO */
};
#endif

int initinewsema ()
{	int semid;
	union semun u;

	if ((semid = semget (IPC_PRIVATE, 1, 0600)) == -1)
	{
#ifdef DEBUGSEM
		fprintf (stderr, "creation de semaphore merdu errno: %d\n", errno);
#endif
		return (-1);
	}
	printf ("semaphore id: %d\n", semid);

	u.val = 0;
	if (semctl (semid, 0, SETVAL, u) < 0)
	{	fprintf (stderr, "semaphore initialisation merdue: errno %d\n", errno);
		fprintf (stderr, "continuons quand meme\n");
	}

	return semid;
}

int deletesema (int semid)
{
	union semun u;
	if (semctl (semid, 0, IPC_RMID, u) < 0)
	{	fprintf (stderr, "erreur a la destruction du semaphore errno: %d\n", errno);
		return (-1);
	}
	return 0;
}

int testlocksema (int semid)
{
	struct sembuf soperations [3];

	soperations[0].sem_num = 0;
	soperations[0].sem_op = 0;	/* on teste */
	soperations[0].sem_flg = IPC_NOWAIT;
	soperations[1].sem_num = 0;
	soperations[1].sem_op = 1;	/* on incremente */
	soperations[1].sem_flg = IPC_NOWAIT;

	if ((semop (semid , soperations, 2) < 0) && (errno==EAGAIN))
	{
#ifdef DEBUGSEM
		fprintf (stderr, "semaphore locke mais on continue\n");
#endif
		return 0;
	}
	else
	{
#ifdef DEBUGSEM
		fprintf (stderr, "semaphore libre, donc j'ai du le locker...\n");
#endif
		return 1;
	}
}

int unlocksema (int semid)
{
	struct sembuf soperations [3];

	soperations[0].sem_num = 0;
	soperations[0].sem_op = -1;	/* on decremente */
	soperations[0].sem_flg = IPC_NOWAIT;

	if (semop (semid , soperations, 1) < 0)
	{
// #ifdef DEBUGSEM
		fprintf (stderr, "semaphore erreur a la decrementation ?? errno: %d\n", errno);
// #endif
		return -1;
	}
	else
	{
#ifdef DEBUGSEM
		fprintf (stderr, "semaphore decremente\n");
#endif
		return 0;
	}
}

/*
int testsemaconcept (void)
{
	int semid;

	if ((semid = initinewsema ()) == -1) return (-1);

	testlocksema (semid);
	if (testlocksema (semid)) unlocksema(semid);
	if (testlocksema (semid)) unlocksema(semid);
	if (testlocksema (semid)) unlocksema(semid);
	unlocksema(semid);
	if (testlocksema (semid)) unlocksema(semid);
	if (testlocksema (semid)) unlocksema(semid);
	if (testlocksema (semid)) unlocksema(semid);
	testlocksema (semid);
	if (testlocksema (semid)) unlocksema(semid);
	if (testlocksema (semid)) unlocksema(semid);
	if (testlocksema (semid)) unlocksema(semid);
	unlocksema(semid);
	if (testlocksema (semid)) unlocksema(semid);
	
	return (deletesema (semid));
}


int main (void)
{
	int i;

	for (i=0 ; 1 ; i++)
		testsemaconcept ();

	return 0;
}
*/
