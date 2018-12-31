/* mwlang.c */


/*  OS includes */
#include <string.h>

/* Notes */
#include <global.h>
#include <osmisc.h>				/* OSLoadString */

/* Local include */
#include "mwlang.h"

/******************************************************************
* PROG		: TranslateCharacterToNative()
*			: OS_TRANSLATE_LMBCS_TO_OSNATIVE

	OS_TRANSLATE_NATIVE_TO_LMBCS	  -  Convert the input string from the machine's native character set to LMBCS (optimized for Group 1).

	OS_TRANSLATE_LMBCS_TO_NATIVE	  -  Convert the input string from LMBCS (optimized for Group 1) to the machine's native character set.

	OS_TRANSLATE_LOWER_TO_UPPER	  -  Current international case table.

	OS_TRANSLATE_UPPER_TO_LOWER	  -  Current international case table.

	OS_TRANSLATE_UNACCENT	  -  International unaccenting table.

	OS_TRANSLATE_OSNATIVE_TO_LMBCS	  -  Convert the input string from DOS (code page) to LMBCS (optimized for Group 1).

	OS_TRANSLATE_LMBCS_TO_OSNATIVE	  -  Convert the input string from LMBCS (optimized for Group 1) to DOS.

	OS_TRANSLATE_LMBCS_TO_ASCII	  -  Convert the input string from LMBCS to character text.

	OS_TRANSLATE_LMBCS_TO_UNICODE	  -  Convert the input string from LMBCS to UNICODE.

	OS_TRANSLATE_LMBCS_TO_UTF8	  -  Convert the input string from LMBCS to UTF8.

	OS_TRANSLATE_UNICODE_TO_LMBCS	  -  Convert the input string from UNICODE to LMBCS.

	OS_TRANSLATE_UTF8_TO_LMBCS	  -  Convert the input string from UTF8 to LMBCS.

***************************************************************/
WORD	TranslateCharacterToNative(char* In, char* Out, WORD MaxOutLength)
{

	WORD	OutLength = 0;
	WORD	InLength = 0;
	WORD	RetLength = 0;

	/* convert to NATIVE */
	InLength = (WORD) strlen(In);

	if ( InLength > 0 )
		RetLength = OSTranslate(OS_TRANSLATE_LMBCS_TO_NATIVE, In, InLength, Out, MaxOutLength);
	
	Out[RetLength]='\0';

	return(RetLength);
}


WORD	TranslateCharacterToNotes(char* In, char* Out, WORD MaxOutLength)
{

	WORD		OutLength = 0;
	WORD		InLength = 0;
	WORD		RetLength = 0;
	NLS_INFO*	pinfo;
	NLS_STATUS	error = NOERROR;

	/* Study current native settings */
    pinfo = OSGetLMBCSCLS();

	/* convert to Notes */
	InLength = (WORD) strlen(In);
	RetLength = MaxOutLength;

	if ( InLength > 0 )
	{
		 error = NLS_translate(In, NLS_NULLTERM, Out, &RetLength, NLS_TARGETISLMBCS, pinfo);
		 if ( error == NLS_BADPARM )
			RetLength = 0;

		/* 		RetLength = OSTranslate(OS_TRANSLATE_NATIVE_TO_LMBCS, In, InLength, Out, MaxOutLength); */
		Out[RetLength]='\0'; 			
	}

	return(RetLength);

}


/* end of source code */

