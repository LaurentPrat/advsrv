/* mwlang.h */

/* Notes header */
#include "global.h"		/* typedef WORD */

#ifndef _MWLANG_H
#define _MWLANG_H


WORD	TranslateCharacterToNative(char* In, char* Out, WORD MaxOutLength);
WORD	TranslateCharacterToNotes(char* In, char* Out, WORD MaxOutLength);
WORD	TranslateCharacterUNICODE_MMBCS(char* In, char* Out, WORD MaxOutLength);


#endif /* mwlang.h */

/* end of source code */
