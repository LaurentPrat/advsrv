/*=========================================================================*/
/* 03/01/2002                                                              */
/* file		: NotesDbTimeStamp.h                                           */
/* Created	: for 3.8a build 6 - For Eval                                  */
/* ------------------------------------------------------------------------*/
/* 3.8a b5 Eval : ETimeStamp => "R1", EFirstLaunch => "ODS"                */
/* 3.9  b1 Eval : ETimeStamp => "R2", EFirstLaunch => "ODS1"               */
/* 3.91 b7 Eval : ETimeStamp => "R3", EFirstLaunch => "ODS2"               */
/* 3.92 10 Eval	: ETimeStamp => "R4", EFirstLaunch => "ODS3"
/* 3.94    Eval : ETimeStamp => "R5", EFirstLaunch => "ODS4"
/* 4.0	   Eval : ETimeStamp => "R7", EFirstLaunch => "ODS7"
/* 4.0.1   Eval : ETimeStanp => "R8", EFirstLaunch => "ODS8"
/*=========================================================================*/

#define EFirstLaunch		"ODS8"		/* this field needs to be changed for the next Eval version */
#define ETimeStamp			"R8"		/* this field needs to be changed for the next Eval version */
#define EvalPeriodDuration	45			/* 45 days */


/* Simple function */
STATUS	GetTimeStamp(char* chemin, char* TimeMark, TIME* dermod);
STATUS	IsEvalFirstLaunch(char* chemin, char* TimeMark, BOOL* FirstLaunch);
STATUS	SetEvalFirstLaunch(char* chemin, char* TimeMark);
int		CheckEvalPeriod(TIMEDATE FirstUse_td, int DayDuration);
STATUS	GetTimeDateStamp(char* chemin, char* TimeMark, TIMEDATE* dermod);
STATUS	GetTimeStamp(char* chemin, char* TimeMark, TIME* dermod);


/* Composite using the above  */
/* Set the Date and return if Eval integer > 0 => eval finished */
STATUS EvalProc(int* EvalOK, char * path_name, int EvalDuration);

