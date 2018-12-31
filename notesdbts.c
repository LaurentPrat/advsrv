#include <stdio.h>


/* Notes API include files */
#include <global.h>
#include <nsfdb.h>
#include <nsfnote.h>
#include <nsfdata.h>
#include <nsf.h>
#include <nif.h>
#include <osmem.h>
#include <ostime.h>
#include <idtable.h>
#include <globerr.h>
#include <log.h>

/* Local fonctions */
#include "notesdbts.h"


/**************************************************************************
 * Set the Time Stamp for a TimeMark (prog name) in the Chemin (db & path)
 *
 * INPUTS : chemin    - base à traiter
 *			dermod    - TIMEDATE à coder
 *          TimeMark  - Name associated to the Time Stamp.
 *
 * OUTPUT : erreur    - Status
 ***********************************************************************/
STATUS SetTimeStamp(char *chemin,  char * TimeMark, TIMEDATE dermod)
{
  STATUS erreur;
  DBHANDLE hDb;
  NOTEID icon;
  NOTEHANDLE hNote;


  /* Open the DB */
  if (erreur = NSFDbOpen(chemin, &hDb))
  {
	LogEventText("internal error (1) - MailWatcher Disable ", NULLHANDLE, NOERROR);
	/* printf("could not open database '%s'\n", chemin); Error 1 */
    return(erreur);
  }


  /* Time Stamp */
  if (erreur = NSFDbGetSpecialNoteID(hDb, SPECIAL_ID_NOTE | NOTE_CLASS_ICON, &icon))
  {
	LogEventText("internal error (2) - MailWatcher Disable ", NULLHANDLE, NOERROR);
	/*  printf("Error while updating Time Stamp : '%s'\n", chemin); : Error 2 */
    NSFDbClose(hDb);
    return(erreur);
  }

  /* Open Icon Document */
  if (erreur = NSFNoteOpen(hDb, icon, OPEN_SUMMARY, &hNote))
  {
	LogEventText("internal error (3) - MailWatcher Disable ", NULLHANDLE, NOERROR);
	/*  printf("Could not open the Time Stamp doc : '%s'\n", chemin); : Error 3 */
    NSFDbClose(hDb);
    return(erreur);
  }

  NSFItemSetTime(hNote, TimeMark, &dermod);

  /* Update */
  NSFNoteUpdate(hNote, 0);
  NSFNoteClose(hNote);
  NSFDbClose(hDb);

  return(NOERROR);
}



/**********************************************************************/
/* Mark First Use                                                     */
/*                                                                    */ 
/* INPUT  : char *, Database name & path                              */
/* INPUT  : Field Name as mark up                                     */
/* INPUT  : integer (integer is 1 if the Field Name is present)       */ 
/* OUTPUT : STATUS.                                                   */ 
/*                                                                    */ 
/**********************************************************************/
STATUS IsEvalFirstLaunch(char* chemin, char* TimeMark, BOOL* FirstLaunch)
{

  STATUS erreur;
  DBHANDLE hDb;
  NOTEID icon;
  NOTEHANDLE hNote;
  
  /* Open the DB */
  if (erreur = NSFDbOpen(chemin, &hDb))
  {
	LogEventText("internal error (1) - MailWatcher Disable ", NULLHANDLE, NOERROR);
	/*  printf("Could not open '%s' \n", chemin); */
    return(erreur);
  }


  /* Time Stamp */
  if (erreur = NSFDbGetSpecialNoteID(hDb, SPECIAL_ID_NOTE | NOTE_CLASS_ICON, &icon))
  {
	LogEventText("internal error (2) - MailWatcher Disable ", NULLHANDLE, NOERROR);
	/*  printf("Error getting Time Stamp : '%s'\n", chemin); */
    NSFDbClose(hDb);
    return(erreur);
  }

  /* Open Icon Document */
  if (erreur = NSFNoteOpen(hDb, icon, OPEN_SUMMARY, &hNote))
  {
	LogEventText("internal error (3) - MailWatcher Disable ", NULLHANDLE, NOERROR);
	/*  printf("Could not open the Time Stamp doc : '%s'\n", chemin); */
    NSFDbClose(hDb);
    return(erreur);
  }

  /* Item TimeMark is present ? = True if present - so return the '!' */
  *FirstLaunch = !(NSFItemIsPresent(hNote, TimeMark, (WORD) strlen(TimeMark)));
   

  /* Close db */
  NSFDbClose(hDb);
  return(NOERROR);
}





/**********************************************************************/
/* Mark First Use                                                     */
/*                                                                    */ 
/* INPUT  : char *, Database name & path                              */
/* INPUT  : Field Name as mark up                                     */
/* INPUT  : integer (integer is 1 if the Field Name is present)       */ 
/* OUTPUT : STATUS.                                                   */ 
/*                                                                    */ 
/**********************************************************************/
STATUS SetEvalFirstLaunch(char* chemin, char* TimeMark)
{

  STATUS erreur;
  DBHANDLE hDb;
  NOTEID icon;
  NOTEHANDLE hNote;
  char TimeMarkVal[2];
  
  /* Open the DB */
  if (erreur = NSFDbOpen(chemin, &hDb))
  {
	LogEventText("internal error (1) - MailWatcher Disable ", NULLHANDLE, NOERROR);
	/*  printf("Could not open '%s' \n", chemin); */
    return(erreur);
  }

  /* Time Stamp */
  if (erreur = NSFDbGetSpecialNoteID(hDb, SPECIAL_ID_NOTE | NOTE_CLASS_ICON, &icon))
  {
	LogEventText("internal error (2) - MailWatcher Disable ", NULLHANDLE, NOERROR);
	/*  printf("Error getting Time Stamp : '%s'\n", chemin); */
    NSFDbClose(hDb);
    return(erreur);
  }

  /* Open Icon Document */
  if (erreur = NSFNoteOpen(hDb, icon, OPEN_SUMMARY, &hNote))
  {
	LogEventText("internal error (3) - MailWatcher Disable ", NULLHANDLE, NOERROR);
	/*  printf("Could not open the Time Stamp doc : '%s'\n", chemin); */
    NSFDbClose(hDb);
    return(erreur);
  }

  /* Item TimeMark is present ? */
  strcpy(TimeMarkVal,"1");
  erreur = NSFItemSetText(hNote, TimeMark, TimeMarkVal,(WORD) strlen(TimeMarkVal)); 

  /* Update */
  NSFNoteUpdate(hNote, 0);
  NSFNoteClose(hNote);
  NSFDbClose(hDb);

  return(erreur);
}



/*********************************************************************/
/* Get Time Mark                                                     */
/*                                                                   */
/* INPUT  : char * , Database name & path                            */
/*        : char * , Time Mark                                       */
/* OUTPUT : Time                                                     */
/*********************************************************************/  
STATUS GetTimeStamp(char* chemin, char* TimeMark, TIME* dermod)
{
  STATUS erreur;
  DBHANDLE hDb;
  NOTEID icon;
  NOTEHANDLE hNote;
  TIMEDATE Localdermod;
  
  /* Open the DB */
  if (erreur = NSFDbOpen(chemin, &hDb))
  {
	LogEventText("internal error (1) - MailWatcher Disable ", NULLHANDLE, NOERROR);
	/*  printf("Could not open '%s' \n", chemin); */
    return(erreur);
  }


  /* Time Stamp */
  if (erreur = NSFDbGetSpecialNoteID(hDb, SPECIAL_ID_NOTE | NOTE_CLASS_ICON, &icon))
  {
	LogEventText("internal error (2) - MailWatcher Disable ", NULLHANDLE, NOERROR);
	/*  printf("Error while updating Time Stamp : '%s'\n", chemin); */
    NSFDbClose(hDb);
    return(erreur);
  }

  /* Open Icon Document */
  if (erreur = NSFNoteOpen(hDb, icon, OPEN_SUMMARY, &hNote))
  {
	LogEventText("internal error (3) - MailWatcher Disable ", NULLHANDLE, NOERROR);
	/*  printf("Could not open the Time Stamp doc : '%s'\n", chemin); */
    NSFDbClose(hDb);
    return(erreur);
  }

  NSFItemGetTime(hNote, TimeMark, &Localdermod);
 
  /* Convert to Time */
  dermod->GM = Localdermod;
  if(erreur  = TimeGMToLocal(dermod))
  {
    NSFDbClose(hDb);
    return(erreur);
  }

  /* Close */
  NSFNoteClose(hNote);
  NSFDbClose(hDb);

  /* Return */
  return(NOERROR);
}




/*********************************************************************/
/* Get TimeDate Mark                                                 */
/* INPUT  : char * , Database name & path                            */
/*        : char * , Time Mark                                       */
/* OUTPUT : TimeDate                                                 */
/*********************************************************************/  
STATUS GetTimeDateStamp(char* chemin, char* TimeMark, TIMEDATE* dermod)
{
  STATUS erreur;
  DBHANDLE hDb;
  NOTEID icon;
  NOTEHANDLE hNote;

  /* Open the DB */
  if (erreur = NSFDbOpen(chemin, &hDb))
  {
	LogEventText("internal error (1) - MailWatcher Disable ", NULLHANDLE, NOERROR);
	/*  printf("Could not open '%s' \n", chemin); */
    return(erreur);
  }

  /* Time Stamp */
  if (erreur = NSFDbGetSpecialNoteID(hDb, SPECIAL_ID_NOTE | NOTE_CLASS_ICON, &icon))
  {
	LogEventText("internal error (2) - MailWatcher Disable ", NULLHANDLE, NOERROR);
	/*  printf("Error while updating Time Stamp : '%s'\n", chemin); */
    NSFDbClose(hDb);
    return(erreur);
  }

  /* Open Icon Document */
  if (erreur = NSFNoteOpen(hDb, icon, OPEN_SUMMARY, &hNote))
  {
	LogEventText("internal error (3) - MailWatcher Disable ", NULLHANDLE, NOERROR);
	/*  printf("Could not open the Time Stamp doc : '%s'\n", chemin); */
    NSFDbClose(hDb);
    return(erreur);
  }

  NSFItemGetTime(hNote, TimeMark, dermod); 

  /* Close */
  NSFNoteClose(hNote);
  NSFDbClose(hDb);

  /* Return */
  return(NOERROR);

}


/**********************************************/
/* INPUT  : Start Date                        */
/*        : nb of days  (positive int )       */
/* OUTPUT : 1 ou 0 (1 => Eval not finished    */
/*                                            */
/**********************************************/   
int CheckEvalPeriod(TIMEDATE FirstUse_td, int DayDuration)
{

	TIMEDATE	today_td;

    /* Get current TimeDate */
    OSCurrentTIMEDATE(&today_td);
  
	/* Adjust before - System date in the past (-nb of day) */
	TimeDateAdjust(&today_td, 0, 0, 0, -DayDuration, 0, 0); /* S, min, Hour, day, Month, Y */

    /* Obtain the difference in SECONDS between two TIMEDATEs 
	this difference > 0 if today minus DayDuration > FirstUse_td */
    return TimeDateDifference(&today_td,&FirstUse_td);   
}



/******************************************************************************/
STATUS EvalProc(int* EvalOK, char * path_name, int EvalDuration)
{
	/* Lv */
	STATUS		error = NOERROR;
	TIMEDATE	binary_td1;
	TIMEDATE	binary_td2;
	BOOL		FirstLaunch;


	/* Get today Time/Date values */
	OSCurrentTIMEDATE(&binary_td1); 

	/* First time ? */
	if(!IsEvalFirstLaunch(path_name, EFirstLaunch, &FirstLaunch))	
	  if (FirstLaunch)
	  {	 
		  SetEvalFirstLaunch(path_name, EFirstLaunch);		/* Mark as Eval period is starting ! */
		  SetTimeStamp(path_name, ETimeStamp, binary_td1);	/* and Mark the clock as Today ! */	 

		/* phase 1 : OK */
		*EvalOK = -1;
		return(NOERROR);				
	  }

	/**************************************************/
	/* Control Period compare to system date          */
	/**************************************************/
	else
	{
		/* Get the Time/Date stored in Notes Db : path_name */
		error = GetTimeDateStamp(path_name, ETimeStamp, &binary_td2);

		/* Check - > positive => period is over */
		if (!error)
			*EvalOK = CheckEvalPeriod(binary_td2, EvalDuration);
		else
			*EvalOK = 1; /* end of Eval => return an error */
	}

	return(error);
}




