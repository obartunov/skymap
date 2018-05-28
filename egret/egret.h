/***********************************************************************
* egret.h v2.1     Maneesh Sahani       Sunday 11th February 1990 
*
*    NED  NASA/IPAC Extragalactic Database
*
***********************************************************************/


#ifndef EGRET_H_
#define EGRET_H_

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


extern list_of_name egret();

#endif

