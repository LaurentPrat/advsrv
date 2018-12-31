#ifdef AS400

/********************************************************************/
/* MailWatcher 3.8a (b5).                                           */
/* AS400 - Native API                                               */
/* Define prototypes of external programs.                          */
/********************************************************************/
#pragma linkage (QUSRJOBI,OS)/*Linkage to QUSRJOBI -*/

void QUSRJOBI(char *, int, char *, char *,char *, char *);
/*Prototype for QUSRJOBI */

/* Define the Qwc_JOBI100_t struture */
typedef struct JOBI100
{
   char data[390];  /* jobname [0+9 ->+10] */
} Qwc_JOBI0100_t;


/*
 * Retrieve the current job information.
 */

void jobname(char* jobname)
{
    /* API struct */
    Qwc_JOBI0100_t                info;
    char errorCode [30]="\0";

    /* init */
    memset(&info, 0, sizeof(info));

    QUSRJOBI((char*) &info, sizeof(info),
             "JOBI0100",                   /* Format of returned Data         */
             "*                         ", /* Target Job (26 char)            */
             "                ",           /* Unused  (16 char)               */
             errorCode);                   /* Error Code  (address of 30 char)*/

    if (strlen(errorCode) != 0)
    {
        printf("Error on QUSRJOBI, job name set to ** NULL**");
        strcpy(jobname, "**NULL**");
     }
     /* Return Dump */
     else
     {
              /* Skip the beginning */
              strncpy(jobname, &info.data[8], 10);
              jobname[10]='\0'; /* end */
      }
}

/************** TEST ********************/
int
main(int argc, char** argv)
{
 char LV[390];

 printf("-- Starting --\n");
 jobname(LV);
 printf("jobname  : %s \n", LV);
 printf(" -- Done -- \n");
 return(0);

}

#endif