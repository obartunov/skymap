/*****************************************************************************
*
* egret.c  Xiuqin Wu         Jan. 24, 1990
*
* This is a test routine for EGRET name checking in NED.
* It reads string from stdin, passes it to egret checking subroutine. 
*
* EOF(^D) to quit.
*
* status:
*         -1       internal error
*                  (one case is the length of the input string is 0)
*          0       right input
*          1       ambiguous input, ask user making selection
*          2       messages
*
*
*****************************************************************************/

#include <stdio.h>
#include <math.h>
#include "egret.h"

main(argc,argv)
{
   char                 s[80];
   int                  status;

   printf("Please input an object name. Ctnl-D to exit.\n");
   while (gets(s) != (char *)NULL){
      if (strcasecmp(s, "quit") == 0)
	 break;
      status = name_chk(s);
      printf("\nPlease input an object name. Ctnl-D to exit.\n");
      } 
}

name_chk(s)
char   *s;
{
   list_of_name         lof;
   sl                   sp;
   int                  status;


   lof = egret(s);
   if (lof->status < 0){
      status = -1;
      printf("Internal Problem\n");
      fflush(stdout);
      }
   else if (lof->names != NULL && lof->mesgs == NULL){
      status = 0;
      printf("TEST1: %d\n%s\n", status, lof->names->str);
      fflush(stdout);
      strcpy(s, lof->names->str);
      }
   else  if (lof->names == NULL) {
      status = 2;
      printf("%d, \n", status);
      for( sp = lof->mesgs; sp != NULL ; sp = sp->next )
         printf("TEST2: %s\n", sp->str);
      fflush(stdout);
      }
   else {
      status = 1;
      status = make_selection(lof, s);
      }

   return(status);
}


make_selection(lof, s)
list_of_name         lof;
char                 *s;
{
   int     count, continu, i, select;
   int     status;
   char    tmps[100];
   sl      sp;

   count = 1;

   for( sp = lof->names; sp != NULL ; sp = sp->next, count++ )
      printf("%1d.  %s\n", count, sp->str);
   printf("%1d.  %s\n", count, "None of the above.");
   printf("Please input the number of your selection.\n");
   continu= 1;
   while (continu== 1) {
      if (gets(tmps) !=(char *)NULL) {
	 select = atoi(tmps);
	 if (select >count || select < 1) {
	    printf("Please select one of the above list.\n");
	    continu= 1;
	    }
	 else continu= 0;
	 }
      else {
	 continu = 0;
	 select = count;
	 }
      }

   if (select == count) {
      printf("you canceled the input.\n");
      status = -1;
      }
   else{
      for (sp = lof->names, i=1; i != select; i++, sp = sp->next);
      printf("your selection: %s\n", sp->str);
      strcpy(s, sp->str);
      status = 0;
      }
   return(status);
}
