/* advsrv.h */

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

/* Function */
void	LogErr(char *message, STATUS err);
void	UpperCase(char *n);
void	Trim(char* in, char* out);
STATUS	MW_AppendMWTAGFromString(NOTEHANDLE hNote, char* mwtag, char* stritem);





