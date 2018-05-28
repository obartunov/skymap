/**************************************************************************
*  egret.util.c   for NED
*
*  Maneesh Sahani     Aug. 1989
*  Xiuqin Wu          Jan. 1990
*
*  egret utilities 
**************************************************************************/


#include <stdlib.h>  /* malloc */
#include <stdio.h>
#include <ctype.h>
#include <string.h>

typedef struct sl__ {
  char *str;
  struct sl__ *next;
} sl_;
typedef sl_ *sl;

typedef struct eg_outstruct__ {
     int  status;  /* status < 0 ; internal errors the rest of the struct is */
                   /*              junk                                      */
                   /* status == 1; Case [234]:User confirmation is essential */
                   /* status == 0; Case [1]: Confirmation non-essential      */
     sl names;     /* Access to the actual matched names                     */
     sl mesgs;     /* Descriptive messages for the user to see concerning    */
                   /* what he did wrong                                      */
   } eg_outstruct_; 
typedef eg_outstruct_ *eg_outstruct;
typedef eg_outstruct list_of_name;

extern char *yytext;
extern yylex();
extern void eg_restart();


sl names = NULL;
sl mesgs = NULL;

sl sp = NULL;

FILE *eg_outputfile;
FILE *eg_inputfile;
FILE *eg_errorfile;


char *eg_instring;

/* xw 4/14/95 added for Solaris compiling */

/* end of added declaration 
char *malloc();
*/

/********************************************** sl functions *****************/


/* clear list and return pointer to NULL */

sl_clear(slptr)

sl *slptr;
{
  while ((sp = *slptr) != NULL) {
    *slptr = (*slptr)->next;
    free(sp->str);
    free(sp);
  }
}



/* add string to head of list */

void sl_add(list,string)

sl *list;
char *string;
{

  sp = (sl_ *)malloc(sizeof(sl_));
  sp->str = strdup(string);
  sp->next = *list;
  *list = sp;
}


/* invert list */

sl_invert(list)

sl *list;
{
  sl last,cur,nextel;
  
  for(last = NULL, cur = *list ; cur != NULL ; last = cur, cur = nextel) {
    nextel = cur->next;
    cur->next = last;
  }
  *list = last;
}

/********************************************** functions ********************/


/* Return a 1 if the beginning of s[] matches pattern[], else return a 0 */


int match(s,pattern)

char *s,*pattern;
{
  static int i;

  switch (pattern[0]) {
  case '(':
    if (!strncmp(s,pattern+1,strlen(pattern)-2))
      return(1);
    else
      return(0);
  case '[':
    for (i=0 ; pattern[i] != ']' ; i++ ) {
      if (pattern[i] == '-') {
        if (*s >= pattern[i?i-1:0] && *s <= pattern[(i<(int)strlen(pattern)-2)?i+1:i])
          return(1);
      }
      else if (pattern[i] == *s)
        return(1);
    }
    return(0);
  }
}



/* Return a string containing a left filled integer less than max, greater than min
   and with the same number of digits */

char *intfill(s,max,min,maxdigits,ndigits)

char *s;
int  max,min,maxdigits,ndigits;
{
  char *out;
  int i,value;
  char format[7];

  out = (char *)malloc(10);
  sprintf(format,"%%%dd",ndigits);
  sscanf(s,format,&value);
  if (value > max || value < min)
    sprintf(out,"%s","error");
  else  {
    sprintf(format,"%%%dd",maxdigits);
    sprintf(out,format,value);
    out[maxdigits] = '\0';
    for ( i = 0 ; i < maxdigits ; i++ )
      if ( out[i] == ' ')
        out[i] = '0';
  }
  return(out);
}


/***************************************************** egret() ***************/
/*
 * This is the procedure called by the interface to run egret.
 */

list_of_name egret(instring)

char *instring;
{
  static list_of_name ret;
  static int   first = 1;

  if (first)
     ret = (list_of_name) malloc(sizeof(eg_outstruct_));
  if (*instring == '\0')
    ret->status = -1;
  else {
    sl_clear(&names);
    sl_clear(&mesgs);
    eg_instring = instring;
    if (first)
      first = 0;
    else
       eg_restart();
    yylex();
    sl_invert(&names);
    sl_invert(&mesgs);
    ret->names = names;
    ret->mesgs = mesgs;
    ret->status = 1;
  }
  return(ret);
}
