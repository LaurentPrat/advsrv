#ifndef WIN32

/* Reading the process table using popen and ps */
#include <stdio.h>      /* FILE, sprintf, fgets, puts */
#include <stdlib.h>     /* atoi, exit, EXIT_SUCCESS */
#include <string.h>     /* strtok, strcmp */
#include <sys/types.h>  /* pid_t */
#include <fcntl.h>		/* O_RDONLY */
#include <signal.h>		/* signal */
#include <sys/wait.h>   /* wait() */ 

/* proto */
void	reaper(int i);
char*	procname(pid_t);
void 	mw_wait_child();


/*********************************************************
*
* Wait for child to DIED properly
* Avoid ZOMBIE proces or <defunct>
*
************************************************************/
void reaper(int i)
{
  pid_t pid;
  int statusp;

  while((pid = waitpid(-1, &statusp, WNOHANG)) > 0)
  {
	  printf("reap child : %i  status : %i\n", pid, statusp);
  }

  return;
}


/******************************************************************************
* PROC		: procname
* PURPOSE	: Use UNIX shell to get with the PID the name of the calling process
* 			: wait for child to be finished : no zombie.
*
* NB		: DO NOT THE SIGNAL as this prog is called from a shared lib.
*********************************************************************************/
char *procname(pid_t pid)
{
   char 	line[133];
   char		command[80];
   char *	linep;
   char *	token;
   char * 	cmd;
   FILE *	fp;
   int 		fmw;
   int 		std_out;
   int 		status;

   /*
    * signal(SIGCHLD, SIG_IGN);
    */

    /*
    if( (signal(SIGCHLD, reaper)) == SIG_ERR )
    {
         perror("signal disposition error");
         return (char* ) 0;
    }
    */

   if (0 == pid)
   	return (char *)0;

   /* Establish new open descriptors for stdout  */
   fmw = open("/dev/mwatcher", O_RDWR);
   if (0 == fmw)
	    return(char *) 0;

   /* dummy file */
   std_out = open("/dev/mwatchero", O_RDWR);
   if (0 == std_out)
   {
 	   close(fmw);     			/* close mw temporary output ! */
   	   return(char *) 0;
   }

   (void) dup2(1, std_out);					/* save STDOUT file descriptor */
   (void) dup2(fmw, 1); 	    			/* point stdout to mwatcher output */

   /* STDOUT is now /dev/mwatcher */

   /* ---------- call the shell --------------- */
   sprintf(command, "ps -p %d 2>/dev/null", pid);
   fp = popen(command, "r");

   /* Do not use signal but wait for the child to be finihsed */
   mw_wait_child();

   if ((FILE *)0 == fp)
   {
	  /* restore stdout */
	  (void) dup2(std_out, 1);
       close(fmw);     			/* close mw temporary output ! */
   	   return (char *)0;
   }


   /* read the header line */
   if ((char *)0 == fgets(line, sizeof line, fp))
   {
      /* flush & close the pipe */
	  status = fflush(fp);
	  status = pclose(fp);

	  /* restore stdout */
	  (void) dup2(std_out, 1);
      close(fmw);     			/* close mw temporary output ! */
      return (char *)0;
   }

   /* figure out where the command name is from the column headings.
    * (BSD-ish machines put the COMMAND in the 5th column, while SysV
    * seems to put CMD or COMMAND in the 4th column.)
    */
   for (linep = line; ; linep = (char *)0)
   {
      if ((char *)0 == (token = strtok(linep, " \t\n")))
      {
		/* flush & close the pipe */
	 	 status = fflush(fp);
	 	 status = pclose(fp);

	  	/* restore stdout */
	  	(void) dup2(std_out, 1);
      	close(fmw);     			/* close mw temporary output ! */
      	return (char *)0;
      }
      if (0 == strcmp("COMMAND", token) || 0 == strcmp("CMD", token))
      { /*  we found the COMMAND column */
         cmd = token;
         break;
      }
   }

   /* read the ps(1) output line */
   if ((char *)0 == fgets(line, sizeof line, fp))
   {
	  /* flush & close the pipe */
	  status = fflush(fp);
	  status = pclose(fp);

	  /* restore stdout */
	  (void) dup2(std_out, 1);
      close(fmw);     			/* close mw temporary output ! */
      return (char *)0;
   }

   /* grab the "word" underneath the command heading... */
   if ((char *)0 == (token = strtok(cmd, " \t\n")))
   {
      /* flush & close the pipe */
   	  status = fflush(fp);
   	  status = pclose(fp);

   	  /* restore stdout */
   	  (void) dup2(std_out, 1);
      close(fmw);     			/* close mw temporary output ! */
      return (char *)0;
   }


   /* flush & close the pipe */
   status = fflush(fp);
   status = pclose(fp);

   /* restore stdout */
   (void) dup2(std_out, 1);
    close(fmw);     /* close mw temporary output ! */

   return token;
}

/*******************************************************************
*
* PROG		: mw_wait_child
* PURPOSE	: Loop until one child is finished - quit if no child
*
*******************************************************************/
void mw_wait_child()
{
	pid_t 	pid;
  	int 	statusp;

	 while ( (pid = waitpid(-1, &statusp, WNOHANG) ) > 0);

	/* printf("end of child : %i  status : %i\n", pid, statusp); */
}


/* -  Test - */
int main(int argc, char *argv[])
{

   int 		LoopArg 	= 1;
   int 		i 		= 0;
   char		pname[512];
   int		Pid = 0;

   if ( argc > 2 )
   	LoopArg = atoi(argv[2]);

	Pid = atoi(argv[1]);

   /* Argt2 to stress the process run x time */
     for ( i=0 ; i < LoopArg ; i++)
     {
  		strcpy(pname, procname(Pid) );
  		printf("%i : PID : %i - %s \n", i, Pid, pname );
  	 }

   exit(EXIT_SUCCESS);
}



#endif /* code for Unix only */

