/******************************************************************************************************
 *	Trapper de messages pour MAIL.BOX. Cet Ext Mngr bloque les
 *	messages déposés dans mail.box en ajoutant 'RoutingState =
 *	DEAD'. Ainsi le routeur n'y touche pas et on a tout notre
 *	temps pour les bidouiller via un addin-task ou autre....
 *
 *	API Notes 4.6 / Win NT 4.0
 * ----------------------------
 * Project settings, linker directives
 * INTEL  : 1 byte alignment	=> /Zp1
 * OTHERS : 8 bytes
 * LIB	  : mono thread (NT).	=> /ML
 * ---------------------------------------------------------------------------------
 * LP - 02/02/2000.
 *
 * Make it for Unix & Other plateforms
 *
 *	20/04/2000	rattachement de l'EM à server.exe + smtp.exe
 *							(modifs à faire pour plate-formes != WIN32)
 * ---------------------------------------------------------------------------------
 * 18/05/2000 - Fix DEAD Mail pb.
 * ---------------------------------------------------------------------------------
 * 07/06/2000 - Delete the NOT_EM flag...to check "reply" with MailWatcher
 * ---------------------------------------------------------------------------------
 * 27/06/2000 - Fix DEAD mail, Remove ADVSRV flag in the EM is not catching the mail
 * because of ScanMail that might wait for MailWatcher to release the "DEAD" flag
 * ---------------------------------------------------------------------------------
 * 08/12/2000.
 * Compile OS/390.
 * > OSGetModuleFileName (NULL, procname, sizeof(procname)-1) instead of
 * procnamelen = GetModuleFileName(NULL, procname, sizeof(procname)-1);
 * 11/12/2000.
 * Use of LoadLibrary & GetProcAddress.
 * --------------------------------------------------------------------------------
 * (Comments)
 * for N.T    : notes.lib DOES NOT contain OSGetModuleFilename
 * for OS.390 : libnotes.x is containing the OSGetModuleFilename
 * for Linux  : libnotes.so is containing the OSGetModuleFilename
 * --------------------------------------------------------------------------------
 * 19/03/2001 :
 * Special build : Notes traffic only
 * add and SMTP (#ifdef)
 * --------------------------------------------------------------------------------
 * 17/07/2001 :
 * call Free(hDLL) just after the use of the DLL proc.
 * --------------------------------------------------------------------------------
 * 17/09/2001 :
 * Step back, remove the #ifdef SMTP etc... and the Notes API OsGetModuleFilename()
 * Add anew module : procname.c
 * use extern char* procname(pid_t pid) to get the name associated to the PID number
 * Test : Linux - 17/09/2001.
 * Remove the test on mail.box... if the file exists or not... Multiple mail.box oblige
 * Read the notes.ini : MWDLLVERBOSE for Verbose !
 * --------------------------------------------------------------------------------
 * 19/10/2001 : (3.8) Build 2
 * in mw_EMHandlerProc
 * Put ADVSRV test before NOT_EM.
 * if (verbose)..
 * --------------------------------------------------------------------------------
 * 24/10/2001. : Fix
 * the mail.box title are used to capture e.mail
 * it works for multi mail box because they all have the same title.
 * ---------------------------------------------------------------------------------
 * 11/12/2001	: Support EBCDIC processname.
 * Complete for OS390 and AS400
 * --------------------------------------------------------------------------------
 * 03/01/2002	: Fix / Unix
 * Version		:3.8ab6
 * Compatibility with ScanMail.Rename Global var & Callback proc.
 * => mw_gHooksRegistered
 * => mw_EMHandlerProc
 * --------------------------------------------------------------------------------
 * 07/01/2002	: Special Call back proc for EVAL
 * => mw_EMHandlerProcEval
 * ifdef EVAL => include "NotesDbTimeStamp.h"
 * --------------------------------------------------------------------------------
 * 11/06/2002	: Add "HTTP" task to be attached by the extension manager
 *				: INotes, Webbased traffic.
 * --------------------------------------------------------------------------------
 * 25/07/2002	: 3.9 Build 10
 *				: Test Error first !
 *				: if ( theData->Status != NOERROR )
 *				: goto Done (new exit process);
 * ---------------------------------------------------------------------------------
 * 29/07/2002	: 	STATUS error = NOERROR;
 *				: return(error) or ERR_EM_CONTINUE;
 *				: void LogInFile(char *)
 * ---------------------------------------------------------------------------------
 * 01/08/2002	: Replace NSFNoteUpdate & NSFNoteUpdateExtended by a special V5
 *              : Extension manager.
 *				: #define EM_NSFNOTEUPDATEMAILBOX				164
 * !!!!!        : THIS DLL IS NOW SUPPORTING 5.0 and above NOT V4 !!!
 * ---------------------------------------------------------------------------------
 * 03/10/2002	: Attach a Dummy extension manager when PrcName is not in the list
 *				: This is due to have the Fixup or other task to be launched when
 *				: the notes server is down.
 * ---------------------------------------------------------------------------------
 * 24/10/2002	: 3.91 release. Change program name + rebuild.
 * ---------------------------------------------------------------------------------
 * 17/03/2003	: Generic anti-virus support.
 *				: cleanup : remove of the verbose code mode
 *              : Add MWADVSRVOTHERSERVICES notes.ini entry
 * 				: fix : increase db_info buffer to 512
 * ---------------------------------------------------------------------------------
 * 18/04/2003   : Add notes.ini entry for configuring 'MWTAG' that are to be executed
 *              : before the default 'MWADVT'. : MWADVSRVMWTAG :
 *				: This a temporary implementation - this notes.ini will not be documented -
 * ---------------------------------------------------------------------------------
 * 04/05/2003	: Support Norton A.V
 *				: Workout even DEAD MAIL as Norton still flag "DEAD"
 *				: Control IsPresentRoutingState but not nil (strlen>0)
 * ---------------------------------------------------------------------------------
 * 12/05/2003	: Because of ScanMail 2.51 bug regarding ADVSRV field
 *				: Rename 'ADVSRV' to 'MWADVSRV'
 * ---------------------------------------------------------------------------------
 * 15/07/2003	: Support Norton A.V
 *				: AS NORTON DO NOT PROCESS doc with no attachment !!
 *				: MWADVT Starts on all the task (standard) + MWADVSRVOTHERSERVICES
 *				: "HOLD" message with NO Attachment.
 * ---------------------------------------------------------------------------------
 * 02/04/2004	: Zlinux issue. Remove the usage of VARARG_PTR in mw_EMHandlerProc
 * 18/05/2004	: Release 3.93 : change release name. that's it :)
 * ----------------------------------------------------------------------------------
 * 13/10/2004	: Revisions 3.93.1
 *				: change in procname (Pb with STDOUT and NETBACKUP / VERITAS)
 * ----------------------------------------------------------------------------------
 * 26/03/2005	: revision 3.94
 *				: Can have more than on MWTAG in the notes.ini ( cf "MWADVSRVMWTAG" )
 *				: revision 3.94.1
 *				: change in MW_AppendMWTAGFromString : Trim(token,token)
 * ----------------------------------------------------------------------------------
 * 26/10/2005	: revision 3.94.2
 *				: MWADVSRV is remaing in mails.
 *				: FORWARD/REPLY/CUT & PASTE are not doing to well with the EM
 *				: on the last server seen by MWADVT - ( same server if unique ) -
 *				: FLAG HOLD + MWTAG default if MWADVSRV is there but without MWTAG item.
 * ----------------------------------------------------------------------------------
 *				: revision 3.94.3
 *				: add goto Done if hNote== NULLHANDLE;
 * ----------------------------------------------------------------------------------
 *				: revision 3.94.4
 *				: use gHandlerProc in EMRegister.
 * ----------------------------------------------------------------------------------
 * 21/02/2006	: revison 3.94.5-7
 *				: change in procname.c ( UNIX only  Not win32 )
 *				: Declare signal proc : do not wait for DEAD children !! no more zombie please
 *				: signal(SIGCHLD, SIG_IGN);
 * -------------------------------------------------------------------------------------------
 * 13/12/2006	: revision 3.94.8-9
 *				: change in EMhandler.
 *				: Check if MWADVSRV == current servername.
 *				: Avoid infinite loop with TrendMicro ScanMail ( smdreal )
 * 07/02/2007	: revision 3.94.10
 *				: SMDReal form TrendMicro is now able to exclude certain task
 *				: To avoid infinite loop MW test the MWADVSRV = CurrentServerName
 *				: Have this test only if notes.ini force it.
 *				: Notes.ini env : MWADVSRVOTHERSERVICESNOEXCLUDE =1;
 * 20/07/2007	: revision  3.94.11
 *				: Fix AIR FRANCE MASS MAILING : Keep "NOT_EM" if "NOT_EM_PERM" is present
 *				:
 * 18/09/2007	: revision 3.94.12
 *				: Update LOG - remove (V5/V6) text.
 *				: for win32 - add version support
 *				:
 * 26/02/2008	: revision 3.94.13
 *				: test : if ( verbose == 1 ) - instead of if ( verbose )
 *				: ----------------------------------- GOES V4 ----------------------------------
 * 02/04/2008	: revsion 4.0.13
 *				: Rebuild with new version for display
 *				:
 * 08/09/2008   : revision 4.0.14
 *				: if defined into notes.ini - can skip email based of SMTPOriginator or FROM.
 *				: using pattern matching algo.
 *				: new notes.ini entries:
 *				: MWADVSRVSKIPSPECIALFIELD ( TEXT field only : 256 char)
 *				: MWADVSRVSKIPSPECIALFIELDPATTERN ( TEXT only : 256 char )
 *				: revision 4.0.15
 *				: warn all the other servers to not process without the deploy the notes.ini config
 *				: NSFItemSetText(hNote, "NOT_EM_PERM", "1", 1);
 *				: revision 4.0.16
 *				: FIX Eval evaluation
 *				: FIX strstr - updates from 4.0.14-15
 * 				: ----------------------------------------------------------------------------
 *	14/09/2008	: revision 4.0.16
 *				: STATS ONLY PROC
 *				: Create TXT file into specified path - does not create directories.
 *				: output file is servername ( with =;/,\ converted to _)
 *				: use notes.ini config:
 *				: MWADVSRVSTATSONLY=1
 *				: MWADVSRVSTATSFILEPATH=C:\Lotus801\Domino\data\mwstats\
 *				: file structure.
 *				: field sep '|', multi value sep 'µ'
 *				: sProcessed [ YYMMDDHHMMSSCC ] 
 *				: sFrom - char - MAXUSERNAME ( 256 ) 
 *				: NbRec - int  - 
 *				: sRecipients - char - 30 * MAXUSERNAMe 
 * 				: nGlobalSize - float .2 - 
 *				: nbAttach - int - number of attachment
 *				: sAttachExtension - char - 1024 - list of extension - separated by 'µ' 
 *				: sAttachSize - char - 1024 - list of attachment size in KB  - 0.2f - separated by 'µ' -same or as sAttachExtension
 *				:
 * 14/09/2008	: revision 4.0.17
 *				: use mwlang.c/h
 *				: ConvertToNative ( addresses ).
 *				: sRecipientsNative[30*MAXUSERNAME];
 *				:
 *				: revision 4.0.18-20
 *				: Register EM AFTER for STATS.
 *				: RAZ global var.
 *				: Support native - sFrom.
 *				: complete for !Win32.
 *				:
 * 12/11/2008	: revision 4.0.21
 *				: Add EXCLUDE LIST.
 *				: Notes ini entry : MWADVSRVEXCLUDETASKS=smtptest,http  ( use comma as separator )
 *				: SPR/CA-AMT. 'HTTPADMIN.EXE'
 *				:
 * 26/11/2008	: revision 4.0.21.2
 *				: X64 compilation.
 *				:
 * 04/02/2009	: revision 4.0.22
 *				: Log When DLL unload.
 *				:
 * 16/06/2009	: revision 4.0.0.23
 *				: Log DLL unload only in 'verbose' mode
 *				:
 * 26/03/2010	: revision 4.0.0.24
 *				: Add a list of task that are "hook" with the regular
 *				: new var	: MB_IncludeTaskList
 *				: notes.ini : MWADVSRVINCLUDETASKS
 *				:
 * 20/08/2010	: revision 4.0.0.25
 *				: Display / no Display 'MailWatcher' as cie for HP EASD software EOM.
 *				:
 * 25/08/2011	: revision 4.0.0.26
 *				: STAT ONLY COLLECT
 *				: Filename, put a '$' between servername & date
 *				:
 * 30/08/2011	: revision 4.0.0.27
 *				: Save the stats file into {DATA}+Path, if there is not specification of a file path
 *				: Create the directory when necessary.
 *				:
 * 15/09/2011	: revision 4.0.0.28 
 *				: Change Stats dump file, the file name with =,/,\ conversion.
 *				: new 'void ConvertSerNameforFileSystem2()'
 *				: QA
 * 09/01/2012	: revision 4.0.0.29
 *				: add _sys_errlist[ENOPATH]) to the log
 *				: revision 4.0.0.30
 *				: add int CreateMWDefaultPath(char* DominoDataDir, char* Path)
 *				: revision 4.0.0.31
 *				: Fix, GVariable modification breaks code. Stat Dump filename.
 * 13/03/2012	: revision 4.0.0.32
 *				: Test if ( m_RegistrationHandle != 0) before Deregistering
 * 05/04/2012	: revision 4.0.0.35
 *				: Disable the Dummy EM for task not to be registered
 * 26/06/2012	: revision 4.1.0.35
 *				: Realign versionning
 *				: revision 4.0.1.36
 *				: Check Path for Directory/unix
 *				: Fix version and use strerror(errno)); 
 * 03/07/2012	: revision 4.0.1.37
 *				: Collect Subject for Stats - StatsOnly activated
 *				: new notes.ini : 1/0 : MWADVSRVSTATSEXTENDED
 *				: use StatsOnly = 2 ( 0, 1, 2 )
 *				:
 * 04/09/2012	: revision 4.0.1.38
 *				: fix mkdir, errno for Unix.
 *				: mkdir(char*, 0777) : drwxr-xr-x    2 notes    notes           256 04 sep 12:43 dumpstats
 *				: fix checkpath for Unix.
 *				: print size even when 0 into stats dump file
 *				:
 * 29/05/2013	: revision 4.0.1.39
 *				: new CheckMailBoxOK()
 *				: Skip EM registration if there is an ERROR on mail.box or mail1.box opening at startup
 *				: enabled if [MWADVSRVNOSTRICT=1 ]
 *				: need to specify [MWADVSRVMONOMAILBOX=1] for Mono mail.box config
 *              :
 * 22/03/2017   : revision 4.0.1.41
 *              : Extend filter to prevent holding message = avoid MWADVT process on all servers
 *              :
 * 22/03/2017   : revision 4.0.1.42
 *              : prevent empty pattern, add strlen() to enable it
 ****************************************************************************************************************************************/
#ifndef UNIX   
#include <windows.h>
#include <direct.h>			/* 4.0.0.27 : _mkdir */
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>		/* 4.0.0.27 : mkdir */
#include <errno.h>			/* 4.0.0.27 */

#ifndef WIN32
#include <unistd.h>
#endif

/* Notes include */
#include <time.h>
#include <global.h>
#include <nsfnote.h>
#include <nsferr.h>
#include <extmgr.h>
#include <misc.h>
#include <nsfdb.h>
#include <osmisc.h>
#include <log.h>
#include <kfm.h>
#include <osenv.h>		/* Notes.ini variables : OSGetEnvironmentString */
#include <ostime.h>		/* OSCurrentTIMEDATE */
#include <osmem.h>		/* OSLock, OSUnlockObject */
#include <nsfobjec.h>	/* NSFDbGetObjectSize */
#include <ods.h>		/* ODSReadMemory */


/* Log temp */
#include <time.h>

/* Local Include */
#include "mwlang.h"


/*********************************** AS400 Patch *******************************/
#ifdef OS400
#define MW_EBCDIC
#define __STRING_CODE_SET__ 850
#pragma convert(850)
/* no more include with QADRT package from IBM  : QADRT package is required a top lib in QCLSRC */
#endif

/************************************* OS390 Patch ******************************/
#ifdef OS390
#define MW_EBCDIC
#include "_Ascii_a.h"   /* must be the last include ! */
#pragma export (MainEntryPoint)
#pragma export (mw_EMHandlerProc)
#pragma export (mw_EMHandlerProcEval)
#pragma export (StatsEMHandlerProc)
#endif

/********************************** EVAL ***************************************/
#ifdef EVAL
#include "notesdbts.h"
#define VersionCode "V4.0.1.42 - EVAL"
#else
	/* Version Code */
#define VersionCode "V4.0.1.42"
#endif

/*********************** Variables globales **************************/
#ifdef MW_EBCDIC
void convertASCII2EBCDIC(char* s);
void convertEBCDIC2ASCII(char* s);
#endif


#ifndef UNIX
EMHANDLER			gHandlerProc;
EMHANDLER			gHandlerDummy;		/* 3.94.4 */
EMHANDLER			gHandlerProcEval;	/* 4.0.15 */
EMHANDLER			gHandlerStatProc;	/* 4.0.16 */
#endif

BOOL				mw_gHooksRegistered = FALSE;
WORD				gRecursionID		= 0;
HEMREGISTRATION		m_RegistrationHandle = 0;		/* 4.0.0.32 */


/* debug mode */
static char servername[MAXUSERNAME];			/* nom du serveur */
int					verbose = 0;
char				debugverbose[1024];

/* For Anti-virus & other products compatibility */
int					OtherServices = 0;
int					OtherServices_AttachOnly = 0;
int					OtherServicesCannotExcludeMWEngine = 0;
char				MB_OtherServices[512];				/* Other process that do 'updates' within mail.box */
char				MB_MWTAG[512];
int					bMWTAG = 0;

/* 4.0.14-15 */
int					SkipSpecialFieldValues = 0;
int					SkipSpecialFieldValues1 = 0;
int					SkipSpecialFieldValues2 = 0;
int					SkipSpecialFieldValues3 = 0;
int					SkipSpecialFieldValues4 = 0;
int					SkipSpecialFieldValues5 = 0;
char				MB_SkipFieldName[MAXUSERNAME];
char				MB_SkipFieldPattern[MAXUSERNAME];

/* 4.0.1.41 */
char				MB_SkipFieldPattern1[MAXUSERNAME];
char				MB_SkipFieldPattern2[MAXUSERNAME];
char				MB_SkipFieldPattern3[MAXUSERNAME];
char				MB_SkipFieldPattern4[MAXUSERNAME];
char				MB_SkipFieldPattern5[MAXUSERNAME];


/* 4.0.16 */
int					StatsOnly = 0;
char				StatsFilePath[MAXPATH];
char				servername_aliased[MAXUSERNAME * 2];	/* 4.0.16, 4.0.0.28 : extend size for character conversion */

char				filename[MAXPATH];
TIME				current;
char				sProcessed[30];
int					NbRec;
char				sFrom[MAXUSERNAME];
char				sFromNative[2*MAXUSERNAME];
char				sRecipients[30*MAXUSERNAME];
char				sAttachExtension[1024];
char				sAttachSize[1024];
char				sSubject[1024];					/* 4.0.1.37 */
char				sSubjectNative[2*1024];			/* 4.0.1.37 */
char				StatOutPutStream[15000];		/* need to be computed !!  - 4.0.1.37 : Extended : 3000 -> 15000 */

#ifndef UNIX
BOOL				vUnix = FALSE;					/* default value; 4.0.1.36 */
#else
BOOL				vUnix = TRUE;
#endif

/************************** Fcts prototypes ***************************/
#ifndef UNIX
BOOL WINAPI DllMain(HINSTANCE hInstance,DWORD fdwReason,LPVOID lpReserved);
#endif

/* 3.8a */
STATUS	GetMailBoxTitle();
STATUS	CheckMonoMailBox();

/* Exported */
STATUS LNPUBLIC MainEntryPoint(void);
STATUS LNPUBLIC mw_EMHandlerProc(EMRECORD *theData);
STATUS LNPUBLIC mw_EMHandlerProcEval(EMRECORD *theData);
STATUS LNPUBLIC dummy_EMHandlerProc(EMRECORD *theData);
STATUS LNPUBLIC StatsEMHandlerProc(EMRECORD *theData);

#ifndef WIN32

        #ifndef AS400
             extern char* procname(int);
        #else
             extern void jobname(char *);
        #endif
#endif

#ifdef WIN32
/* Storage needed for synchronization under Windows NT */
CRITICAL_SECTION    gCriticalSection; /* new in fix 3.9b8 */
#endif

/* internal */
void	LogErr(char *message, STATUS err);
void	UpperCase(char *n);
void	LogInFile(char *);
int		MWGetProcessName(char* processname, int verbose);
void	Trim(char* in, char* out);
STATUS	MW_AppendMWTAGFromString(NOTEHANDLE hNote, char* mwtag, char* stritem);
int		GetNamesList(NOTEHANDLE	hNote, char* fieldname, char* sep, WORD MaxSize, int* NbItem, char* ItemConcatList);
void	LogInStatsFile(char * filename, char* line);
void	ConvertServerNameforFileSystem(char *servername, char *servername_aliased);
NUMBER	mw_NSFNoteGetSize(DBHANDLE hMb, NOTEHANDLE nh, int *pj, char* sAttachExt, char* sAttachSize, WORD MAXRECLEN);
char*	GetWord(char * string, const char *seps, int a);
int		isInString(char* processname, char* szExcludeTask, char* seps);
void	ConvertSerNameforFileSystem2(char* servername, char* servername_aliased );
/* 4.0.0.30 */
int		CreateMWDefaultPath(char* DominoDataDir, char* Path);
void	CheckPath(char *Filename);

/* 4.0.1.39 */
STATUS	CheckMailBoxState(DBHANDLE hDB);
STATUS	CheckMailBoxOK();

/* 4.0.1.41 */
STATUS SkipSpecialFieldNotification(NOTEHANDLE hNote, char* MB_SkipFieldPattern, char* tempo);

#ifndef UNIX
/********************************************************/
/*   DllMain() : Fonction d'entrée/sortie dans la DLL.  */
/********************************************************/
BOOL WINAPI DllMain(HINSTANCE hInstance,DWORD fdwReason,LPVOID lpReserved)
{
	char	db_infos[1024];						/* 4.0.21 : 512 -> 1024 */
	DWORD	procnamelen;	
	char	processname[512];
	
	switch(fdwReason)
	{
		case DLL_PROCESS_ATTACH:

	      InitializeCriticalSection(&gCriticalSection);	/* new in fix 3.9b8 */

		    gHandlerDummy		= (EMHANDLER) MakeProcInstance((FARPROC) dummy_EMHandlerProc, hInstance); /* 3.94.4 */
			gHandlerStatProc	= (EMHANDLER) MakeProcInstance((FARPROC) StatsEMHandlerProc,   hInstance);	/* 4.0.16 */

#ifdef EVAL
			gHandlerProc		= (EMHANDLER) MakeProcInstance((FARPROC) mw_EMHandlerProc,     hInstance);
			gHandlerProcEval	= (EMHANDLER) MakeProcInstance((FARPROC) mw_EMHandlerProcEval, hInstance);	/* 4.0.15 */		
#else
			gHandlerProc		= (EMHANDLER) MakeProcInstance((FARPROC) mw_EMHandlerProc, hInstance);
#endif
			break;

		case DLL_PROCESS_DETACH:
			if ( m_RegistrationHandle != 0 )
				EMDeregister(m_RegistrationHandle);
			else
			{
				if ( verbose )
				{
					sprintf(db_infos,"extension unloaded - m_RegistrationHandle = 0");			
					LogErr(db_infos, NOERROR);
				}
			}

			mw_gHooksRegistered = FALSE;

			FreeProcInstance(gHandlerDummy);			/* 3.94.4 */
			FreeProcInstance(gHandlerStatProc);
			FreeProcInstance(gHandlerProc);
			
			/* Log unloading DLL : 4.0.22  - 4.0.0.23 */
			if ( verbose )
			{
				procnamelen = (DWORD) MWGetProcessName(processname, 0 /* verbose */ );
				if ( procnamelen )
					sprintf(db_infos,"extension unloaded from %s", processname);
				else
					sprintf(db_infos,"extension unloaded");
			
				LogErr(db_infos, NOERROR);
				/* end - 4.0.22 */
			}

			DeleteCriticalSection(&gCriticalSection); /* new in fix 3.9b8 */	

			break;
	}	/* end switch */


	UNREFERENCED_PARAMETER(lpReserved);  /* new in fix 3.9b8 */
	return(TRUE);
}

#endif


/*************************************************************************/
/* MainEntryPoint() : Fonction Appelée par Notes au chargement de la DLL */
/*************************************************************************/
STATUS LNPUBLIC MainEntryPoint(void)
{
	STATUS	error = NOERROR;
	char	db_infos[1024];						/* 4.0.21 : 512 -> 1024 */
	DWORD	procnamelen;	
	char	processname[512];					
	char	MB_ExcludeTaskList[512];			/* 4.0.21	*/
	char	MB_IncludeTaskList[512];			/* 4.0.0.24 */

    /* Special Var for Evaluation Version */
#ifdef EVAL
	char	path_name[MAXPATH];
#endif
	int		Eval = -1;	/* Eval : OK if Eval < ZERO ! */


	/* Extension is only attached to certain task */
	char	*procok[] = {"SERVER", "SMTP", "HTTP", NULL};
	char	*loc;
	int		i;
	int		CheckExcludeTask = 0;		/* 4.0.21	*/
	int		CheckIncludeTask = 0;		/* 4.0.0.24 */
	


	/* ------------------------------------------------- Init -----------------------------------------------------------------*/
	if ( OSGetEnvironmentString("MWADVSRVNOSTRICT", db_infos, sizeof(db_infos)-1))
	{
		if ( error = CheckMailBoxOK() )
			return(error);
	}


	/*--------------------------------------------------- Beg ------------------------------------------------------------------*/
	/* When run on a server the dll is called multiple times. this flag keeps the main code from being executed more than once. */

	/* Register only one time */
	if ( mw_gHooksRegistered )
	{
		LogErr("mw_gHooksRegistered - skip - ", NOERROR);
		return(NOERROR);
	}

	/* 4.0.16 */
	/* MWADVSRVSTATSONLY=1; MWADVSRVSTATSFILEPATH=C:\Lotus801\Domino\data\statmw\ */
	if ( OSGetEnvironmentString("MWADVSRVSTATSONLY", db_infos, sizeof(db_infos)-1))
	{
		StatsOnly = 1;

		if ( ! OSGetEnvironmentString("MWADVSRVSTATSFILEPATH", StatsFilePath, MAXPATH -1) )
		{
			/* 4.0.0.27 : Get DATA path instead */
			if ( OSGetEnvironmentString("DIRECTORY", db_infos, sizeof(db_infos)-1) )
			{
				if ( CreateMWDefaultPath(db_infos, "mwatcher") == 0 )
				{
					/* 4.0.0.30 : mwatcher dir is there */
					sprintf(StatsFilePath,"%s\\mwatcher", db_infos);

					/* Create sub dir for stats */
					CreateMWDefaultPath(StatsFilePath, "dumpstats");

					/* 4.0.0.31 : Restore StatsFilePath with the complete path, as it's a GV for the EMHandler */
					sprintf(StatsFilePath,"%s\\mwatcher\\dumpstats\\", db_infos);
					
					/* 4.0.1.38 : Unix like ? */
					CheckPath(StatsFilePath);
				}
			}
			else
				StatsFilePath[0]='\0';
		}

		/* 4.0.1.37 Collect subject ? */
		if ( OSGetEnvironmentString("MWADVSRVSTATSEXTENDED", db_infos, sizeof(db_infos)-1) )
			StatsOnly = 2;
		

	}


	/* Check out variable in the notes.ini */
	if ( OSGetEnvironmentString("MWADVSRVVERBOSE", db_infos, sizeof(db_infos)-1) )
		verbose = 1;

	/* Read the Notes.ini flag to get the list of other add in task to listen to */
	if (OSGetEnvironmentString("MWADVSRVOTHERSERVICES", MB_OtherServices, sizeof(MB_OtherServices)-1))
	{
		UpperCase(MB_OtherServices);
		OtherServices = 1;
	}


	/* 4.0.14 : Skip based on FROM/SmtpOriginator/x field */
	if ( OSGetEnvironmentString("MWADVSRVSKIPSPECIALFIELD", MB_SkipFieldName, sizeof(MB_SkipFieldName)-1) )
	{
		if ( OSGetEnvironmentString("MWADVSRVSKIPSPECIALFIELDPATTERN", MB_SkipFieldPattern, sizeof(MB_SkipFieldPattern)-1) )
		{
			if (strlen(MB_SkipFieldPattern))
				SkipSpecialFieldValues = 1;
		}
		
		/* 4.0.1.41, 4.0.1.42 */
		if (OSGetEnvironmentString("MWADVSRVSKIPSPECIALFIELDPATTERN1", MB_SkipFieldPattern1, sizeof(MB_SkipFieldPattern1) - 1))
		{
			if (strlen(MB_SkipFieldPattern1))
			{
				SkipSpecialFieldValues1 = 1;
				SkipSpecialFieldValues++;
			}
		}

		if (OSGetEnvironmentString("MWADVSRVSKIPSPECIALFIELDPATTERN2", MB_SkipFieldPattern2, sizeof(MB_SkipFieldPattern2) - 1))
		{
			if (strlen(MB_SkipFieldPattern2))
			{
				SkipSpecialFieldValues2 = 1;
				SkipSpecialFieldValues++;
			}
		}

		if (OSGetEnvironmentString("MWADVSRVSKIPSPECIALFIELDPATTERN3", MB_SkipFieldPattern3, sizeof(MB_SkipFieldPattern3) - 1))
		{
			if (strlen(MB_SkipFieldPattern3))
			{
				SkipSpecialFieldValues3 = 1;
				SkipSpecialFieldValues++;
			}
		}


		if (OSGetEnvironmentString("MWADVSRVSKIPSPECIALFIELDPATTERN4", MB_SkipFieldPattern4, sizeof(MB_SkipFieldPattern4) - 1))
		{
			if (strlen(MB_SkipFieldPattern4))
			{
				SkipSpecialFieldValues4 = 1;
				SkipSpecialFieldValues++;
			}
		}

		if (OSGetEnvironmentString("MWADVSRVSKIPSPECIALFIELDPATTERN5", MB_SkipFieldPattern5, sizeof(MB_SkipFieldPattern5) - 1))
		{
			if (strlen(MB_SkipFieldPattern5))
			{
				SkipSpecialFieldValues5 = 1;
				SkipSpecialFieldValues++;
			}
		}


		if (SkipSpecialFieldValues)
		{
			sprintf(db_infos, "Skip pattern for %s => %i", MB_SkipFieldName, SkipSpecialFieldValues);
			LogErr(db_infos, NOERROR);
		}

		
	}

	/* Read the Notes.ini flag to see if the OtherServices has some limit */
	if (OtherServices == 1)
	{
		if (OSGetEnvironmentString("MWADVSRVOTHERSERVICESAO", db_infos, sizeof(db_infos)-1))
			OtherServices_AttachOnly = 1;

		/* Check if the OtherServices is not able to Exclude MW updates - so there is a risk of infinite loop */
		if (OSGetEnvironmentString("MWADVSRVOTHERSERVICESNOEXCLUDE", db_infos, sizeof(db_infos)-1))
			OtherServicesCannotExcludeMWEngine = 1;
	}

	/* Read the Notes.ini flag to get the list of other MWTAG to are to be executed before default */
	if (OSGetEnvironmentString("MWADVSRVMWTAG", MB_MWTAG, sizeof(MB_MWTAG)-1))
	{
		UpperCase(MB_MWTAG);
		bMWTAG = 1;
	}


	/* 4.0.21 : Add exclusion list of process for which EM does not attach itself - as env var into notes.ini */
	if (OSGetEnvironmentString("MWADVSRVEXCLUDETASKS", MB_ExcludeTaskList, sizeof(MB_ExcludeTaskList)-1))
	{
		UpperCase(MB_ExcludeTaskList);
		CheckExcludeTask = 1;
	}

	/* 4.0.0.24 : Add Inclusion list of proces */
	if (OSGetEnvironmentString("MWADVSRVINCLUDETASKS", MB_IncludeTaskList, sizeof(MB_IncludeTaskList)-1))
	{
		UpperCase(MB_IncludeTaskList);
		CheckIncludeTask = 1;
	}
	
	/* get calling module name */
	procnamelen = (DWORD) MWGetProcessName(processname, verbose);

    /* ProcName */
	if (procnamelen)
	{
		loc = 0;

		/* if OtherServices_AttachOnly = 1 then catch all the process + the OtherServices :> CheckIncludeTask <:*/
		if ( ( OtherServices == 0 ) || (OtherServices_AttachOnly) )
		{
			for (i=0; procok[i] != NULL; i++)
			{
				loc = strstr(processname, procok[i]);
				if (loc)
					break;
			}
		}

		/* Test if the current process is in the list of it is the other task to listen */
		if ( (!loc) & (OtherServices) )
		{
			loc = strstr(processname, MB_OtherServices);
			if ( loc )
			{
				sprintf(db_infos, "Other services : %s", processname);
				LogErr(db_infos, NOERROR);			
			}
		}

		/* 4.0.0.24 : test if the current process is in the list of 'include' */
		if ( (!loc) & (CheckIncludeTask) )
		{
			loc = strstr(processname, MB_IncludeTaskList);
			if ( loc )
			{
				sprintf(db_infos, "Include task : %s", processname);
				LogErr(db_infos, NOERROR);
			}
		}

		/* 4.0.21 : Skip registration if found into a list */
		if ( (!loc) & (CheckExcludeTask) )
		{
			if ( isInString(processname, MB_ExcludeTaskList, ",") != 0 )
			{
				sprintf(db_infos, "Exclude task : %s", processname);
				LogErr(db_infos, NOERROR);			
				loc = NULL;
			}
		}
		

		if (!loc)
		{
			/* printf("EM NOT attached to '%s'\n", procname); */
#ifdef WIN32
			/* ------------------------------------------------------------------------*/
			/* ----------- Required to have single task running SOLO ------------------*/
			/* ------ Attach a dummy EM that does nothing -----------------------------*/

			/* --- 4.0.0.33 : disable the dummy EM for Win32 --- */
			/* mw_gHooksRegistered = TRUE; */

			/* Need a RecursionID */
			/* error = EMCreateRecursionID(&gRecursionID); */

			/* if(error)
			{
				LogErr("Err while calling EMCreateRecursionID()", error);
				return(error);
			}
			*/

			/* Attach a dummy EM */
			/*if (error = EMRegister((EID)EM_NSFNOTEUPDATEMAILBOX,
			                                        (WORD) EM_REG_BEFORE,
													gHandlerDummy,
													gRecursionID,
													&m_RegistrationHandle))
			{
				LogErr("error in EMRegister (before).", error);
				m_RegistrationHandle = 0;
				return(error);
			}
			*/
#endif
			/* ------------------------------------------------------------------------*/

			return(NOERROR);
		}
	}
	/* : 3.94.5 : Procname is "" */
	else
	{
		return(NOERROR);
	}



/* ===(START)=========================== Evaluation Period control =======================*/
#ifdef EVAL

	/* check if mail.box is there or multi mail.box : multi by default */
	strcpy(path_name,"mail1.box");

	if (CheckMonoMailBox())
		strcpy(path_name,"mail.box");

	EvalProc(&Eval, path_name, EvalPeriodDuration); /* see header file : NotesDbTimeStamp.h */

	if (Eval > 0)
	{
		sprintf(db_infos, "MailWatcher evaluation period is now over.");
		LogErr(db_infos, NOERROR);
	}
	else
	{
		sprintf(db_infos, "MailWatcher evaluation going for %d day(s).",-(Eval/86400) + 1);
		LogErr(db_infos, NOERROR);
	}

#endif
/* ===(END)============================= Evaluation Period control =======================*/

    /* OK - registered */
	mw_gHooksRegistered = TRUE;

	/* Need a RecursionID */
	error = EMCreateRecursionID(&gRecursionID);

	if(error)
	{
		LogErr("Err while calling EMCreateRecursionID()", error);
		return(error);
	}

/*************************************************************************************************/
/*                                        WIN32                                                  */
/*************************************************************************************************/

#ifndef UNIX

	/* On rattache la fonction pour NSFNOTEUPDATEXTENDED() before */
	if (Eval > 0)
	{
		if (error = EMRegister((EID)EM_NSFNOTEUPDATEMAILBOX, (WORD) EM_REG_BEFORE,
														gHandlerProcEval, /* 3.94.4 : (EMHANDLER) mw_EMHandlerProcEval, */
														gRecursionID,
														&m_RegistrationHandle))
			{
				LogErr("error in EM Register Evaluation.", error);
				m_RegistrationHandle = 0;
				return(error);
			}
	}
	else
	{
			if ( StatsOnly == 0 )
			{
				if (error = EMRegister((EID)EM_NSFNOTEUPDATEMAILBOX, (WORD) EM_REG_BEFORE,
														gHandlerProc, /* 3.94.4 :(EMHANDLER) mw_EMHandlerProc, */
														gRecursionID,
														&m_RegistrationHandle))
				{
					LogErr("error in EMRegister(before).", error);
					m_RegistrationHandle = 0;
					return(error);
				}
			}

			/* STATS : 4.0.1.37 */
			if ( StatsOnly > 0 )
			{
				if ( error = EMRegister((EID)EM_NSFNOTEUPDATEMAILBOX, 
														(WORD) EM_REG_AFTER,		/* 4.0.17 - AFTER */
														gHandlerStatProc,			/* 4.0.16  */
														gRecursionID,
														&m_RegistrationHandle))
				{
					LogErr("error in EMRegister(after).", error);
					m_RegistrationHandle = 0;
					return(error);
				}
			}

	} /* Eval < 0 */

/****************************************************************************************/
/*                    Déclaration des fcts à trapper (UNIX)                             */
/****************************************************************************************/
#else

	if (Eval > 0)
	{
		if (error = EMRegister((EID)EM_NSFNOTEUPDATEMAILBOX, (WORD) EM_REG_BEFORE,
														mw_EMHandlerProcEval,
														gRecursionID,
														&m_RegistrationHandle))
		{
				LogErr("error EM Register Evaluation.", error);
				m_RegistrationHandle = 0;
				return(error);
		}
	}
	else
	{

		if ( StatsOnly == 0 )
		{
			/* On rattache la fonction pour  EM_NSFNOTEUPDATEMAILBOX	 before */
			if ( error = EMRegister((EID)EM_NSFNOTEUPDATEMAILBOX, (WORD) EM_REG_BEFORE,
														 mw_EMHandlerProc,
														 gRecursionID,
														 &m_RegistrationHandle))
			{
				LogErr("error in EMRegister(before).", error);
				m_RegistrationHandle = 0;
				return(error);
			}
		}
		/* 4.0.1.37 */
		if ( StatsOnly > 0 )
		{
			if ( error = EMRegister((EID)EM_NSFNOTEUPDATEMAILBOX, 
														(WORD) EM_REG_AFTER,		/* 4.0.17 - AFTER */
														StatsEMHandlerProc,			/* 4.0.16  */
														gRecursionID,
														&m_RegistrationHandle))
				{
					LogErr("error in EMRegister(after).", error);
					m_RegistrationHandle = 0;
					return(error);
				}
		}


	} /* Eval < 0 */

#endif

	/* On récupère le nom du serveur */
	SECKFMGetUserName(servername);
	ConvertSerNameforFileSystem2(servername, servername_aliased);		/* 4.0.16, 4.0.0.28 : call new function */
	UpperCase(servername);

	if (Eval < 0)
	{
		if ( StatsOnly == 0)
			sprintf(db_infos, "(%s) Email Control %s on '%s' is active", processname, VersionCode, servername);
		else
			sprintf(db_infos, "(%s) MailWatcher [ STATS %s ] on '%s' is active", processname, VersionCode, servername);

		LogErr(db_infos, NOERROR);

	}

	/* Normal End */
	return(NOERROR);
}




/***********************************************************
 *	LogErr() : log Notes d'une erreur
 *	Inputs : char * = message à afficher
 *					 STATUS = code erreur interne Notes
 ***********************************************************/
void LogErr(char *message, STATUS err)
{
 	char meserr[150];

	if (err)
	{
		OSLoadString(NULLHANDLE, ERR(err), meserr, sizeof(meserr)-1);
		LogEventText("err ADVSRV : <%s> - <%s>", NULLHANDLE, NOERROR, message, meserr);
	}
	else
		LogEventText("ADVSRV : %s", NULLHANDLE, NOERROR, message);
	return;
}


/*******************
* Convert UpperCase
*********************/
void UpperCase(char *n)
{
	while (*n)
	{
	  *n = toupper(*n);
	  n++;
	}
}


/************************
* Remove leading blank
**************************/
void Trim(char* in, char* out)
{
	int	run = 0;

	/* go one by one until no more blank or end of string */
	while ((*in == ' ') && (*in != NULL) )
	{
		run = 1;
		in++;
	}

	/* no more blank or nothing ;) */
	if ( run == 1 )
		strcpy(out, in);
}




/********************************************************************************/
/*   dummy_EMHandlerProc() : Fonction Appellée par Notes (Extension Manager).   */
/*******************************************************************************/
STATUS LNPUBLIC dummy_EMHandlerProc(EMRECORD *theData)
{
	/* Do nothing  !*/
	return(ERR_EM_CONTINUE);
}



/**************************************************************************/
/*   mw_EMHandlerProc() : Fonction Appellée par Notes (Extension Manager).   */
/**************************************************************************/
STATUS LNPUBLIC mw_EMHandlerProcEval(EMRECORD *theData)
{

	/* Do nothing  !*/
	LogErr("MailWatcher evaluation is now over - please remove : 'extmgr_addins=advsrv' from your notes.ini", NOERROR);
	return(ERR_EM_CONTINUE);
}





/********************************************************************/
/* mw_EMHandleProc                                                  */
/********************************************************************/
STATUS LNPUBLIC mw_EMHandlerProc(EMRECORD *theData)
{
	STATUS			error = NOERROR;
	char			tempo[MAXUSERNAME];
	NOTEHANDLE		hNote	= NULLHANDLE;
	DWORD			flg		= 0;
	DWORD			procnamelen;
	char			processname[512];



	/* Test Error first ! */
	if ( theData->Status != NOERROR )
	{
		error = theData->Status;
		goto Done;
	}

	/* log des arguments */
	hNote = VARARG_GET(theData->Ap, NOTEHANDLE);

	/* 3.94.3 */
	if ( hNote == NULLHANDLE )
		goto Done;

#ifdef LINUX
	flg   = VARARG_GET(theData->Ap, int);
#else
	flg   = VARARG_GET(theData->Ap, WORD);
#endif

	/* Continue if update <> delete */
	if ((flg & UPDATE_DELETED) == UPDATE_DELETED)
		goto Done;


	/* -- BLOCK of 1+ 5 pattern --4.0.1.41 ---------------------------------*/

	/* ------------------------------------------------------------------*/
	/* DO NOT STOP : 4.0.14												 */
	/* char				MB_SkipFieldName[MAXUSERNAME];					 */
	/* char				MB_SkipFieldPattern[MAXUSERNAME];				 */
	/* ------------------------------------------------------------------*/
	if ( SkipSpecialFieldValues )
	{
		/* Get TEXT value - if match goes out with NO STOP */
		if ( NSFItemGetText(hNote, MB_SkipFieldName, tempo, sizeof(tempo)-1 ) )
		{
			if ( strstr(tempo, MB_SkipFieldPattern) )
			{	
				/* 4.0.1.41 */
				SkipSpecialFieldNotification(hNote, MB_SkipFieldPattern, tempo);
				goto Done;
			}

			/* ---------- BLOCK 1 ----------------------------------------------*/
			if (SkipSpecialFieldValues1)
				if (strstr(tempo, MB_SkipFieldPattern1))
				{
					/* 4.0.1.41 */
					SkipSpecialFieldNotification(hNote, MB_SkipFieldPattern1, tempo);
					goto Done;
				}

			/* ---------- BLOCK 2 ----------------------------------------------*/
			if (SkipSpecialFieldValues2)
				if (strstr(tempo, MB_SkipFieldPattern2))
				{
					/* 4.0.1.41 */
					SkipSpecialFieldNotification(hNote, MB_SkipFieldPattern2, tempo);
					goto Done;
				}

			/* ---------- BLOCK 3 ----------------------------------------------*/
			if (SkipSpecialFieldValues3)
				if (strstr(tempo, MB_SkipFieldPattern3))
				{
					/* 4.0.1.41 */
					SkipSpecialFieldNotification(hNote, MB_SkipFieldPattern3, tempo);
					goto Done;
				}

			/* ---------- BLOCK 4 ----------------------------------------------*/
			if (SkipSpecialFieldValues4)
				if (strstr(tempo, MB_SkipFieldPattern4))
				{
					/* 4.0.1.41 */
					SkipSpecialFieldNotification(hNote, MB_SkipFieldPattern4, tempo);
					goto Done;
				}

			/* ---------- BLOCK 5 ----------------------------------------------*/
			if (SkipSpecialFieldValues5)
				if (strstr(tempo, MB_SkipFieldPattern5))
				{
					/* 4.0.1.41 */
					SkipSpecialFieldNotification(hNote, MB_SkipFieldPattern5, tempo);
					goto Done;
				}
		}
	}



	/* -------------------------------------------------------------------*/
	/*--                    DO NOT STOP                                 --*/
	/* -- Other Services                                                --*/
	/* -------------------------------------------------------------------*/

	if ( OtherServices_AttachOnly == 1 )
	{
		/* get calling module name */
		procnamelen = (DWORD) MWGetProcessName(processname, verbose );

		/* if some attached files are in skip it when task are NOT EQUAL to OtherServices */
		if ( procnamelen > 0 )
		{
			if ( ! strstr(processname, MB_OtherServices) )
			{
				if (NSFItemIsPresent(hNote, "$FILE", 5))
					goto Done;
			}
		}
	}


	/* -------------------------------------------------------------------*/
	/*--                    DO NOT STOP                                 --*/
	/* -------------------------------------------------------------------*/

	/* Skip if the current process is not done yet : RoutingState is set to HOLD or DEAD */
	if (NSFItemIsPresent(hNote, "RoutingState", 12))
	{
		NSFItemGetText(hNote, "RoutingState", tempo, sizeof(tempo)-1);
		if (strlen(tempo))
		{
			UpperCase(tempo);
			if ( (strcmp(tempo,"HOLD") == 0) || (strcmp(tempo,"DEAD") == 0 ) )
			 goto Done;
		}
	}


	/* (0) -- Internal MailWatcher Extension manager skip */
	if (NSFItemIsPresent(hNote, "NOT_EM", 6))
	{
		/*-- delete the NOT_EM flag...to check "reply" with MailWatcher --*/
		/* 3.94.11 : Keep this field if NOT_EM_PERM is present */
		if (! NSFItemIsPresent(hNote, "NOT_EM_PERM", 11) )
			NSFItemDelete(hNote,"NOT_EM",6);

		/* continue */
		goto Done;
	}

	/* --------- 3.94.2 : disable the MWADVSRV control as the mail is not HOLD or DEAD when it comes here ----------*/

	/* 
	   (1) --  3.94.9 : MailWatcher server mark is set - antivirus may catch up after mailwatcher task does the final update --
	       --  but if OtherServices does a new update mailwatcher will catch up again and here we go for an infinite loop    -- 
		   -- well done OtherServices should not do an Update over again - so this case is really to prevent infinite loop   -- 
	       -- 3.94.10 : Test only if MWADVSRVOTHERSERVICESNOEXCLUDE is not set => OtherServicesCannotExcludeMWEngine ==1     --
	*/

	if ( ( OtherServices == 1  ) && ( OtherServicesCannotExcludeMWEngine  == 1) )
	{
		/* not null ? */
		if ( NSFItemGetText(hNote, "MWADVSRV", tempo, sizeof(tempo)-1) )
		{
			/* Looping ? */	
			if ( strcmp(tempo, servername) == 0 )
				goto Done;
		}
	}


	/* -------------------------------------------------------------------------*/
	/* -- Message without the MWADVSRV flag and the MWADVSRV => Stop them !!! --*/
	/* -------------------------------------------------------------------------*/

	/* check if mwtag is there ! */
	if (NSFItemIsPresent(hNote, "MWTAG", 5))
	{
		if (error = NSFItemSetText(hNote, "RoutingState", "HOLD", 4))
			goto Done;

		if (error = NSFItemSetText(hNote, "MWADVSRV", servername, (WORD) strlen(servername)))
			goto Done;
	}

	/*-----------------------------------------*/
	/* --  Set the final flags                 */
	/* ----------------------------------------*/
	if (error = NSFItemSetText(hNote, "RoutingState", "HOLD", 4))
		goto Done;

	if (error = NSFItemSetText(hNote, "MWADVSRV", servername, (WORD) strlen(servername)))
		goto Done;

	/* Append MWTAG if set in the notes.ini before "MWADVT" */
	if (bMWTAG)
	{
		if ( error = MW_AppendMWTAGFromString(hNote, "MWTAG", MB_MWTAG) )
		goto Done;
	}

	/* Add MWADVT in the MWTAG list */
	if (error = NSFItemAppendTextList(hNote, "MWTAG", "MWADVT", 6, FALSE))
		goto Done;

	/* Whether or not the MailWatcher Flags were written, return the original
    NSFNoteUpdateExtended (EM_AFTER) status back to Notes */

Done:
    if (error == NOERROR)
    {
		return( ERR_EM_CONTINUE );
    }
   else
   {
	   return(error);
   }
}




/********************************************************************************************
* CheckMonoMailBox
* Return : STATUS (NOERROR => FALSE => MULTI
* ERROR => mail1.box does not exist => mono mail.box
*
*********************************************************************************************/
STATUS CheckMonoMailBox()
{

	STATUS erreur;
	DBHANDLE hMb;

	/* Check if Mail1.Box exists */
	if (erreur = NSFDbOpen("mail1.box", &hMb))
	{
		/* Error => mono */
		return(erreur);
	}

	/* NOERROR => multi */
	NSFDbClose(hMb);
	return(NOERROR);

}


/* Fonction : convert_ASCII2EBCDIC            */
/* input    : ASCII string                    */
/* output   : EBCDIC string                   */
#ifdef MW_EBCDIC

void convertASCII2EBCDIC(char* s)
{
#ifdef OS390
	__atoe(s);
#endif

}


/* input : EBCDIC => ASCII */
void convertEBCDIC2ASCII(char* s)
{

  int i;
  int k;
  char t[512];
  char ascii;

  static unsigned char e2a[256] = {
            0,  1,  2,  3,156,  9,134,127,151,141,142, 11, 12, 13, 14, 15,
           16, 17, 18, 19,157,133,  8,135, 24, 25,146,143, 28, 29, 30, 31,
          128,129,130,131,132, 10, 23, 27,136,137,138,139,140,  5,  6,  7,
          144,145, 22,147,148,149,150,  4,152,153,154,155, 20, 21,158, 26,
           32,160,161,162,163,164,165,166,167,168, 91, 46, 60, 40, 43, 33,
           38,169,170,171,172,173,174,175,176,177, 93, 36, 42, 41, 59, 94,
           45, 47,178,179,180,181,182,183,184,185,124, 44, 37, 95, 62, 63,
          186,187,188,189,190,191,192,193,194, 96, 58, 35, 64, 39, 61, 34,
          195, 97, 98, 99,100,101,102,103,104,105,196,197,198,199,200,201,
          202,106,107,108,109,110,111,112,113,114,203,204,205,206,207,208,
          209,126,115,116,117,118,119,120,121,122,210,211,212,213,214,215,
          216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,
          123, 65, 66, 67, 68, 69, 70, 71, 72, 73,232,233,234,235,236,237,
          125, 74, 75, 76, 77, 78, 79, 80, 81, 82,238,239,240,241,242,243,
           92,159, 83, 84, 85, 86, 87, 88, 89, 90,244,245,246,247,248,249,
           48, 49, 50, 51, 52, 53, 54, 55, 56, 57,250,251,252,253,254,255
};


/************************* Specific OS390 *******************************/
#ifdef OS390
	__etoa(s);
#endif





/************************ Specific OS400  ********************************/
#ifdef OS400

k = 0;
i = 0;
ascii='1';


while(ascii)
{

   k     = s[i];
   ascii = e2a[k];
   s[i]  = ascii;

   /* next */
   i = i + 1;
}


#endif
}
#endif



/**********--------------------------------------------------***********/
void
LogInFile(char * line)
{
	FILE*	stream;

	/* open file */
	stream = fopen("mwadvsrv.log", "a");

	if(stream)
	{
		fprintf(stream, line);
		fclose(stream);
	}
}



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
	char	db_infos[1024];		/* 4.0.21 */

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

	if ( verbose == 1 )
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

	if ( verbose == 1 )
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

/************************************************************************
* MW_AppendMWTAGFromString(hNote, "MWTAG", MB_MWTAG)
* PURPOSE	: write on MWTAG the list of tag in MB_MWTAG,
* INPUT		: current note handle
*			: MWTAG field name
*			: string of TAG separated by ","..
* OUTPUT	: STATUS
**************************************************************************/
STATUS	MW_AppendMWTAGFromString(NOTEHANDLE hNote, char* mwtag, char* sTAG)
{
	char*	token;
	char	stritem[512];
	STATUS	error = NOERROR;

	/* parse string with separator */
	strcpy(stritem, sTAG);
	token = strtok(stritem, ",");

	while( token != NULL )
	{
		Trim(token, token);

		if ( error = NSFItemAppendTextList(hNote, mwtag, token, (WORD) strlen(token), FALSE) )
			break;

		/* next token => mwtag item */
		token = strtok(NULL, ",");
	}

	return(error);
}




/* ------------- STATS ONLY ----------------------------------*/
/********************************************************************/
/* StatsEMHandlerProc                                              */
/********************************************************************/
STATUS LNPUBLIC StatsEMHandlerProc(EMRECORD *theData)
{
	STATUS			error			= NOERROR;
	DBHANDLE		hUdb			= NULLHANDLE;
	NOTEHANDLE		hNote			= NULLHANDLE;
	DWORD			flg				= 0;
	int				ListOverFlow	= 0;
	NUMBER			nGlobalSize		= 0;
	int				nbAttach		= 0;
	


	/* Test Error first ! */
	if ( theData->Status != NOERROR )
	{
		error = theData->Status;
		goto Done;
	}

	/* log des arguments */
	hNote = VARARG_GET(theData->Ap, NOTEHANDLE);

	/* 3.94.3 */
	if ( hNote == NULLHANDLE )
		goto Done;

#ifdef LINUX
	flg   = VARARG_GET(theData->Ap, int);
#else
	flg   = VARARG_GET(theData->Ap, WORD);
#endif

	/* Continue if update <> delete */
	if ((flg & UPDATE_DELETED) == UPDATE_DELETED)
		goto Done;

	/* Get DB for Object size function */
	NSFNoteGetInfo(hNote, _NOTE_DB, &hUdb);

	/* ======= Collec Stats Data  ========= */
	NbRec				 = 0;
	ListOverFlow		 = 0;
	sFrom[0]			 = '\0';
	sFromNative[0]		 = '\0';
	sRecipients[0]		 = '\0';
	sAttachExtension[0]  = '\0';
	sAttachSize[0]		 = '\0';
	sSubject[0]			 = '\0';
	sSubjectNative[0]	 = '\0';	/* 4.0.1.37 */

	/* 1 -Processed Time  : sProcessed      */

	/* 2 - ADDRESS : From     */
	if ( NSFItemGetText(hNote, "From", sFrom, MAXUSERNAME -1) )
		TranslateCharacterToNative(sFrom, sFromNative, sizeof(sFromNative) -1);

	/* 3 - ADDRESS : Recipients(s)    */
	GetNamesList(hNote, "recipients","µ", sizeof(sRecipients), &NbRec, sRecipients);
	
	/* 4 - Global Size - Extension(s), Size(s)      */
	nGlobalSize = mw_NSFNoteGetSize(hUdb, hNote, &nbAttach, sAttachExtension, sAttachSize, 1024);
	 

	/* 
	   Save to file : Append / Close : Calc name based on clock ' .. 
	   FileName is : ServerName_YYMMDD_HH.txt 
	 */
	
	OSCurrentTIMEDATE(&current.GM);
	TimeGMToLocal(&current);

	/* 4.0.0.26 : put a '$' between server and date */
	sprintf(filename, "%s%s$%04d%02d%02d_%02d.txt", StatsFilePath, servername_aliased, current.year, current.month, current.day, current.hour);
	
	/* PostedDate, From, nbrec, Recipients, Size, Attachment name(s) - 'µ' , Attachment Size(s) 'µ' */
	sprintf(sProcessed, "%04d%02d%02d%02d%02d%02d%02d", current.year, current.month, current.day, current.hour, current.minute, current.second, current.hundredth);

	/* 4.0.1.37 */
	if ( StatsOnly > 1 )
	{
		if ( NSFItemGetText(hNote, "Subject", sSubject, sizeof(sSubject) -1) )
			TranslateCharacterToNative(sSubject, sSubjectNative, sizeof(sSubjectNative) -1);	
	}


	sprintf(StatOutPutStream,"%s|%s|%i|%s|%.2f|%i|%s|%s|%s\n", sProcessed, 
														  sFromNative, 
														  NbRec, 
														  sRecipients, 
														  nGlobalSize, 
														  nbAttach, 
														  sAttachExtension, 
														  sAttachSize,
														  sSubjectNative);		/* 4.0.1.37 */
	LogInStatsFile(filename, StatOutPutStream);


Done:
    if (error == NOERROR)
    {
		return( ERR_EM_CONTINUE );
    }
   else
   {
	   return(error);
   }
}

/****************************************************************************
* PROG		: GetNameList
*			: Transfrom a NAMES LIST to a string with sep for multi values
* INPUT		: NOTEHANDLE
*			: fieldname
*			: Maxsize for result string
* OUTPUT	: Nbitem found
*			: Result string
*			: Complete or not ( 1 / O)
*****************************************************************************/
int GetNamesList( NOTEHANDLE		hNote,
					 char*			fieldname,
					 char*			sep,
					 WORD			MaxSize,
					 int*			NbItem,
					 char*			ItemConcatList
					 )
{
	
	char	ItemName[MAXUSERNAME];
	char	ItemNameNative[MAXUSERNAME * 2];
	WORD	CurrentLenght = 0;
	int		i = 0;
	WORD	wlen = 0;
	int		ListOverFlow = 0;



	/* Loop over TEXTLIST - NAMES */
	*NbItem = NSFItemGetTextListEntries(hNote, fieldname);


	for ( i = 0; i < (*NbItem); i++ )
	{
		if ( wlen = NSFItemGetTextListEntry(hNote, fieldname, i, ItemName, MAXUSERNAME -1) )
			wlen = TranslateCharacterToNative(ItemName, ItemNameNative, sizeof(ItemNameNative) -1);

		if ( CurrentLenght + wlen > MaxSize )
		{
			ListOverFlow = 1;
			break;
		}
		else
		{
			if ( CurrentLenght == 0 )
			{
				strcpy( ItemConcatList, ItemName);
				CurrentLenght = CurrentLenght + wlen;
			}		
			else
			{
				strcat(ItemConcatList, sep );
				strcat(ItemConcatList, ItemName );
				CurrentLenght = CurrentLenght + 1 + wlen;
			}
		}

	} /* end for */

	return(ListOverFlow);
}

/***********************************************/
void LogInStatsFile(char * filename, char* line)
{
	FILE*	stream;

	/* open file */
	stream = fopen(filename, "a");

	if ( stream )
	{
		fprintf(stream, line);
		fclose(stream);
	}
	else /* Log Error : 4.0.0.31 */
		LogErr(filename, NOERROR );
}


/************************************************************
* PROG		:
* PURPOSE	: replace '=', '\' or '/' by '_'
* 
* **********************************************************/
void ConvertServerNameforFileSystem(char *servername, char *servername_aliased)
{
	strcpy(servername_aliased, servername);	
	
	while ( *servername_aliased++ )
	{
		if ( servername_aliased[0] == '/' || servername_aliased[0] == '\\' || servername_aliased[0] == '=' )
			servername_aliased[0] = '_'; 
	}

}

/********************************************************************************************************
* PROG		: ConvertServerNameforFileSystem2
* PURPOSE	: Convert some character to different code, the length of result is potentialy 4 times larger
*			: '=' is {eq}
*			: '/' is {as}
*			: '\' is {sl}
***********************************************************************************************************/
void ConvertSerNameforFileSystem2(char* servername, char* servername_aliased )
{
	int	i = 0;		/* using as string location */

	/* Step over each character of source string */
	while ( *servername )
	{
		if ( servername[0] == '/' || servername[0] == '\\' || servername[0] == '=' )
		{
			if ( servername[0] == '=' )
			{
				strcat(servername_aliased, "{eq}");
				i = i + 4;
			}
			else if ( servername[0] == '\\' )
			{
				strcat(servername_aliased, "{as}");
				i = i + 4;
			}
			else
			{
				strcat(servername_aliased, "{sl}");
				i = i + 4;
			}
		}
		else
		{
			servername_aliased[i] = servername[0];		/* unchanged */
			i++;
		}

		/* next character */
		 *servername++;
	}

}


/********************************************************************
 * Fct qui calcule la taille d'un message
 * Inputs : handle du document
 * Ouput : NUMBER = taille en octets
 *				 int pj = nb d'objet (pièces jointes, liens DDE, OLE, etc)
 ********************************************************************/
NUMBER mw_NSFNoteGetSize(DBHANDLE hMb, NOTEHANDLE nh, int *pj, char* sAttachExt, char* sAttachSize, WORD MAXRECLEN)
{
	/* 20/11/01.- LP - */
	/* Change for Non Intel machine use ODS */

	STATUS		error = NOERROR;
	BLOCKID		bidh, bidv;
	WORD		wType, wlen;
	DWORD		dwLen, wsize;
	NUMBER		Total=0;
	float		kbsize = 0;
	void		*ptrItemValue;
	FILEOBJECT	pFileObject;
	int			nbpj = 0;
	char		*loc;
	char		*filename;
	size_t		xLen, yLen, SizeLen, ExtLen, currentAttachExtLen, currentAttachSizeLen;
	char		ssize[15];

	/* temp, because of Unix compilation... */
	WORD *ptrItemValueWORD ;

	/* Loop !! */
	error = NSFItemInfo(nh, NULL, 0, &bidh, &wType, &bidv, &dwLen);
	if ( error )
	{
		*pj = nbpj;			/* 4.0.17 */
		return(Total);
	}

	/* init */
	xLen	= 0;
	yLen	= 0;
	SizeLen = 0;
	ExtLen	= 0;
	currentAttachExtLen = 0;
	currentAttachSizeLen = 0;

	while( error == NOERROR )
	{
		BLOCKID bidPrev;

		bidPrev = bidh;	/* save the original pointer for the GetNext */
		Total += dwLen;

		/* Si objet ==> on va chercher sa taille */
		if ((wType & TYPE_OBJECT) == TYPE_OBJECT)
		{
			/* Save the original pointer */
			ptrItemValue = OSLockBlock(void, bidv);

			/*  Step over the data type word to point to the actual data */
			ptrItemValueWORD = (WORD *) ptrItemValue ;
			ptrItemValueWORD++ ;
			ptrItemValue = ptrItemValueWORD ;

			/* Use ODSReadMemory */
			ODSReadMemory( &ptrItemValueWORD, _FILEOBJECT, &pFileObject, 1 );

			error = NSFDbGetObjectSize(hMb, pFileObject.Header.RRV, pFileObject.Header.ObjectType, &dwLen, NULL, NULL);

			/* Free memory : no !! */
			OSUnlockBlock(bidv);

			/* Skip the rest if error on NSFDbGetObjectSize !! */
			if (error)
				continue;

			/* Total */
			Total += dwLen;
			nbpj++;

			/* Extension du fichier */
			if ( pFileObject.Header.ObjectType == OBJECT_FILE )
			{
			 	wlen  = pFileObject.FileNameLength;
				filename = (char *) malloc(wlen+1);

				/* 3.94.50 */
				if ( filename != NULL )
				{
					wsize = pFileObject.FileSize;

					/* 4.0.1.38 : display even when 0 */
					if ( wsize >= 0 )
					{
						kbsize = wsize / 1024.0;
						sprintf(ssize,"%.2f", kbsize);
					}

					
					/* Read the data the comes after the pFileObject */
					ptrItemValue = (char *) ptrItemValue + ODSLength(_FILEOBJECT);

					memcpy(filename, (char *) ptrItemValue, wlen);
					filename[wlen] = '\0';

					/* get File extension : if none log the filename */
					loc = GetWord(filename, ".", 0);
					if ( loc == NULL )
						loc = filename;

					/* 3.94.32 : Buffer size with "..." - DO NOT OVERFLOW ! */
					ExtLen	= strlen(loc);
					SizeLen	= strlen(ssize); 
					xLen	= currentAttachExtLen	+ ExtLen;
					yLen	= currentAttachSizeLen	+ SizeLen;
					
					if ( (xLen + 1 < MAXRECLEN) && (yLen + 1 < MAXRECLEN) )
					{
						if ( currentAttachExtLen > 0 )
						{
						 	/* ext */
							strcat(sAttachExt, "µ");
							strcat(sAttachExt, loc);
							currentAttachExtLen = currentAttachExtLen + ExtLen + 1;

							/* size */
							strcat(sAttachSize, "µ");
							strcat(sAttachSize, ssize);
							currentAttachSizeLen = currentAttachSizeLen + SizeLen + 1;
						}
						else
						{
							/* ext */
							strcpy(sAttachExt, loc);							
							/* Size */
							strcpy(sAttachSize, ssize);
							currentAttachExtLen = ExtLen;
							currentAttachSizeLen =  SizeLen;
						}
					}

					/* Free memory */
					free(filename);

				}/* enough memory */
			}
		}

		error = NSFItemInfoNext(nh, bidPrev, NULL, 0, &bidh, &wType, &bidv, &dwLen);
	}

	*pj = nbpj;
	return(Total);
}


/*****************************************************************************
* GetWord(string, sep, pos)
* INPUT			: string, Separator
* OUTPUT		: (char*)string or token if found.
* ----------------- argt3 : a -------------------------------------------------
* if pos = 0 => last word
* if pos = 1 => first word and so on pos = 2 => second word...
*******************************************************************************/
char* GetWord(char * string, const char *seps, int a)
{
    int t		= 0;
	int	cond	= 1;
	char *token;
	char* savetoken = NULL;

	/* first token */
	token = strtok(string, seps );

	while( ( token != NULL ) & ( cond ) )
	{
      /* inc nb of tokens in the "string" */
		t = t+1;

	   /* Save then Get next token: */
	  savetoken = token;

	  /* test cond : counter (pos) */
	  if (a)
		  cond = t < a;

      token = strtok( NULL, seps );
	}


	return(savetoken);
} /* end of GetWord */



/* Parse second string (A,B,C) searching for first string (...A) */
int isInString(char* processname, char* szExcludeTask, char* seps)
{
	int		cond	= 0;
	char *	token;

	/* first token */
	token = strtok(szExcludeTask, seps );

	while( ( token != NULL ) & ( cond == 0 ) )
	{
		if ( strstr(processname, token) )
			cond = 1;

		token = strtok( NULL, seps );
	}

	return(cond);
}

/**********************************************************
* Create a directory and log if error.
* One directory at a time.. a\b\c. to create c, you need to call a then b first : 3 calls
* This functions is really simple, not for general purpose, not waste in parsing for system separator
*******************************************************************************************************/
int CreateMWDefaultPath(char* DominoDataDir, char* Path)
{
	int		i = 0;
	char	StatsFilePath[MAXPATH];
	char	db_infos[MAXPATH*4];

	sprintf(StatsFilePath,"%s\\%s\\", DominoDataDir, Path);

	/* 4.0.1.36 Linux */
	CheckPath(StatsFilePath);

#ifndef UNIX
	i = mkdir(StatsFilePath);
#else
	i = mkdir(StatsFilePath, 0777);
#endif

	/* log error */
	if ( i )
	{				
		/* 4.0.1.38 : _get_errno( &i ); */
		i = errno;

		if ( i != EEXIST )
		{
			/* 4.0.0.29 : Extend log */
			sprintf(db_infos, "Error during creation of '%s' directory, [%s]", StatsFilePath, strerror(i)); /* _sys_errlist[i]); */
			LogErr(db_infos, NOERROR);
		}
		else
			i = 0;
	}

	return(i);

}

/*************************************************************************************/
/** For Unix Path, Filename, reverse "\" to "/"                                     **/
void CheckPath(char *Filename)
{

	if (vUnix == TRUE)
	{
		while (*Filename++)

		  if (*Filename == '\\')
		 			   *Filename = '/';

	}
}



/*********************************************************************************************
* PROG		: CheckMailBoxState()
*			: if return TRUE then do not hold the messages
*			: This code is trying to prevent message holding if the mail.box has some pb
*			: for example if [ NSFDbGetOptions ]
*			: returns DBOPTION_OUT_OF_SERVICE
*			:
********************************************************************************************/
STATUS	CheckMailBoxState(DBHANDLE hDB)
{
	STATUS	error = NOERROR;
	DWORD	retDbOptions = 0;

	
	error = NSFDbGetOptions(hDB, &retDbOptions);

	/* in case of error - return it - the code should not hold the messages is the return is not NOERROR */
	if ( error )
	{
		if ( error != ERR_SPECIAL_ID )
			return(error);
	}


	/* NOERROR : Parse retDbOptions */


	/* : DBOPTION_OUT_OF_SERVICE	  -  TRUE if database is out-of-service, no new opens allowed. */
	
	/* : DBOPTION_MARKED_FOR_DELETE	  -  TRUE if database is marked for delete. No new opens are allowed; the database will be deleted when the reference count = 0. */


	return(TRUE);
}



/*********************************************************************************
* PROG		: CheckMailBoxOK()
*			: Try to open mail.box or mail1.box if this is a multi-mail.box
*			: if error then disable EM
*			: for example : errortext(ERR_NEEDS_FIXUP,	"This database cannot be opened because a consistency check of it is needed.") 
**********************************************************************************/
STATUS	CheckMailBoxOK()
{	
	STATUS		error = NOERROR;
	DBHANDLE	hDB = NULLHANDLE;
	char		db_infos[10];

	/* if Config is mono mail.box */
	if ( OSGetEnvironmentString("MWADVSRVMONOMAILBOX", db_infos, sizeof(db_infos)-1))
	{
		if (error = NSFDbOpen("mail.box", &hDB))
		{
			LogErr("Mono Mail.box : Extension Manager is now disabled", error);
			return(error);
		}
		else
		{
			NSFDbClose(hDB);
			return(error);
		}
	}

	/* Multi */
	if (error = NSFDbOpen("mail1.box", &hDB))
	{
		LogErr("Multi Mail.box : Extension Manager is now disabled", error);
		return(error);
	}
	else
	{
		NSFDbClose(hDB);
		return(error);
	}

}



/***************************************************
* PROG		: (NOTEHANDLE, string)
*			: Log to the console of the pattern matching
*           : set the field to prevent ADVSRV catching
***************************************************/
STATUS SkipSpecialFieldNotification(NOTEHANDLE hNote, char* MB_SkipFieldPattern, char* tempo)
{
	STATUS	error = NOERROR;
	//char	debugverbose[1024];   <= use global to save time...

	/* 4.0.15 : warn all the other servers to not process without the deploy the notes.ini config */
	error = NSFItemSetText(hNote, "NOT_EM_PERM", "1", 1);
	sprintf(debugverbose, "SKIPING : '%s' found in '%s'", MB_SkipFieldPattern, tempo);
	LogErr(debugverbose, NOERROR);
	return(error);
}



/* end of file */

