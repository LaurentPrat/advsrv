#ifndef UNIX
#include <windows.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifndef WIN32
#include <unistd.h>
#endif




/* ---------------
  To be use with

  procname.c
  jobname.c
------------------*/


/***********************************************************************
* PURPOSE	: Get calling program name *
* IN/OUT	: processname
* IN		: verbose (on/off = 1/0)
* RETURN	: 1 = OK /O = not found
************************************************************************/
int MWGetProcessName(char* processname, int verbose)
{
	int		returnval = 0;
	int		procnamelen;
	char	db_infos[512];

    /***********************************************************************/
	/*                     Non Windows Systems                             */
	/***********************************************************************/

#ifndef WIN32

        #ifndef AS400
	       strcpy(processname, procname(getpid())); /* Unix */
        #else
            jobname(processname); /* AS400 */
        #endif

		/* Convert to ASCII */
		#ifdef MW_EBCDIC
			convertEBCDIC2ASCII(processname);
		#endif

	if (verbose)
	{
        #ifndef AS400
		      sprintf(db_infos, "PID <%i> Procname : %s \n", getpid(), processname);
        #else
 		      sprintf(db_infos, "jobname : %s \n", processname);
        #endif

		LogErr(db_infos, NOERROR);
	}

	/* set return val */
	returnval = strlen(processname);

	/***********************************************************************/
	/*                       Windows Systems                               */
	/***********************************************************************/
#else
	procnamelen = GetModuleFileName( NULL, processname, 512 );

	returnval = procnamelen;
	if (verbose)
	{
		sprintf(db_infos, "Procname : %s \n", processname);
		LogErr(db_infos, NOERROR);
	}

#endif


	/* end */
	if (returnval > 0)
		UpperCase(processname);

	return(returnval);

}


