/****************************************************************/
/*                                                              */
/*                         EGRETC.C                             */
/*                                                              */
/*                 ****** Version 2.0 ******                    */
/*                                                              */
/* This is the first version of egretc to use flex rather       */
/* than lex. The first flex script is still called muscles      */
/* but is flexed rather than lexed. The eventual output still   */
/* defaults to egret.                                           */
/*                                                              */
/* Maneesh Sahani   Nov. 1989  for NED                          */
/*
/* Xiuqin Wu        Nov. 1993  add rectify_level01,             */
/*                                 rectify_level2 functions     */
/*                  Nov. 1994 compress the message lines        */
/*                            so we have less REJECT            */
/*                                                              */
/****************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define VERSION "2.0"             

/* Flag value definitions */
#define ON 1
#define OFF 0

/* Egretc defaults */
/* default in NED source was 1, we changed to 0 (20021211) */
# define DEFAULTMINIMUM 0             /* default for minimum of range of numbers */


/* Notes linked list of strings type */
typedef struct notetype_ {
  char *str;
  struct notetype_ *next;
} notetype;



/* External input/output variables */

FILE *infile=NULL;                         /* file being read */ 
char *infilename = "micro_db";        /* name of infile */

FILE *lexfile;                        /* flex script being written to */
char *flexfilename = "muscles";       /* name of *lexfile */
char *flexflags = "-I";               /* flags given to flex on the command line */

FILE *rectfile;                       /* file containing rectification functions */
char *rectfilename = "egret.rect.c";  /* name of *rectfile */

FILE *rectheader;                     /* file containing declarations of rectification functions */
char *rectheadername = "egret.rect.h";/* name of *rectheader */

char *outfilename = "egret.o";          /* name of destination file (for final product) */

char *ccflags = " ";                 /* flags given to cc while compiling lex.yy.c and egret.rect.c */
/*char *ccflags = "-Jc";   */              /* flags given to cc while compiling lex.yy.c and egret.rect.c */

char instring[256];                   /* line read from *infile */
int linecount;                        /* no. of line now being processed */
notetype *notes = NULL;               /* linked list of notes */
char linewarning[256];                /* warning attached to a specific line in the input file */



/* Global Compiler flags : set by default or on command line */

int CaseInsensitiveCompile = ON;          /* case sensitivity */
int IncludeNotesCompile = ON;             /* allow note entry at the end of any pattern */
int CollapseSpacesCompile = ON;           /* reduce all spaces in input to single spaces */
int CompleteCompile = ON;                 /* Run lex and cc on output */
int RemoveIntermediatesCompile = ON;      /* Only meaningful if CompleteCompile is on. Remove 
					     *lexfile and lex.yy.c */

/* Local Compiler flags : set for each line */

int CaseInsensitiveLine = ON;
int IncludeNotesLine = ON;
int CollapseSpacesLine = ON;
int WarningLine;




/* XW 4/14/95, add declaration for compiling under Solaris */

//char *malloc();


/* end of added declaration */

/* reads and interprets command line; runs compilation */ 

main(argc,argv)

int argc;
char *argv[];
{
  /* called functions */
  FILE *fopen();


  /* automatic variables */
  int BadOption = OFF;     /* Incorrect command line flag */
  char *s;
  char command[80];        /* Command to system. Used to run lex, etc. */
  char error_s[101];       /* the error message sent to perror  xw 4/14/95 */



  /* read command line options and set flags and filenames */
  while (--argc > 0 && (*++argv)[0] == '-')
    for ( s = argv[0]+1 ; *s != '\0' ; s++)
      switch (*s) {
      case 'o':   /* output file */
	if ( *(s+1) != '\0') {
	  fprintf(stderr,"egretc: -o option must be followed by a space\n");
	  BadOption = ON;
	}
	else {
	  outfilename = *++argv;
	  --argc;
	}
	break;
      case 'f':   /* add next argument to flags passed to program */
	if ( *(++s) == '\0') {
	  fprintf(stderr,"egretc: illegal -f specification\n");
	  BadOption = ON;
	}
	else switch ( *s++ ) {
	case 'f': /* flex flags */
	  flexflags = strcat(strcpy((char *)malloc(sizeof(flexflags)+sizeof(s)),flexflags),s);
	  s = *argv+strlen(*argv)-1;
	  break;
	case 'c': /* cc flags */
	  ccflags = strcat(strcpy(malloc(sizeof(flexflags)+sizeof(s)),flexflags),s);
	  s = *argv+strlen(*argv)-1;
	  break;
	default:  /* incorrect argument */
	  fprintf(stderr,"egretc: illegal -f specification\n");
	  BadOption = ON;
	  break;
	}
	break;
      case 'F':   /* pass next argument as flags to program ( replaces old flags ) */
	if ( *(++s) == '\0') {
	  fprintf(stderr,"egretc: illegal -F specification\n");
	  BadOption = ON;
	}
	else switch ( *s++ ) {
	case 'f': /* flex flags */
	  flexflags = s;
	  s = *argv+strlen(*argv)-1;
	  break;
	case 'c': /* cc flags */
	  ccflags = s;
	  s = *argv+strlen(*argv)-1;
	  break;
	default: /* incorrect argument */
	  fprintf(stderr,"egretc: illegal -F specification\n");
	  BadOption = ON;
	  break;
	}
	break;
      case 'c':   /* case insensitive mode */
	CaseInsensitiveCompile = ON;
	break;
      case 'C':   /* case sensitive mode */
	CaseInsensitiveCompile = OFF;
	break;
      case 'i':   /* remove intermediate files */
	RemoveIntermediatesCompile = ON;
	break;
      case 'I':   /* keep intermediate files */
	RemoveIntermediatesCompile = OFF;
	break;
      case 'l':   /* lex and compile output */
	CompleteCompile = ON;
	break;
      case 'L':   /* Output *lexfile, *rectfile and *rectheader  and stop */
	CompleteCompile = OFF;
	break;
      case 'n':   /* include notes */
	IncludeNotesCompile = ON;
	break;
      case 'N':   /* do not include notes */
	IncludeNotesCompile = OFF;
	break;
      case 's':   /* collapse spaces */
	CollapseSpacesCompile = ON;
	break;
      case 'S':   /* do not collapse spaces */
	CollapseSpacesCompile = OFF;
	break;
      default:    /* illegal option */
	fprintf(stderr,"egretc: illegal option %c\n",*s);
	BadOption = ON;
	break;
      }

  if ( BadOption || argc > 1) {     /* incorrect command line */
    fprintf(stderr,"Usage: egretc [-o outfile] [-cCiIlLnNsS] [file]\n");
    exit(1);
    }
  else {                            /* correct command line */
    /* read infile name */
    if (argc == 1)
      infilename = *argv;
  }

  /* files  */
  if ((infile = fopen(infilename,"r")) == NULL) {
    sprintf(error_s, "opening %s",infilename);
    perror(error_s);
    exit(1);
  }
  if ((lexfile = fopen(flexfilename,"w")) == NULL) {
    sprintf(error_s, "opening %s",flexfilename);
    perror(error_s);
    exit(1);
  }
  if ((rectfile = fopen("egret.rect.c","w")) == NULL) {
    perror("opening egret.rect.c");
    exit(1);
  }
  if ((rectheader = fopen("egret.rect.h","w")) == NULL) {
    perror("opening egret.rect.h");
    exit(1);
  }

  startlexfile();                   /* insert instructions to lex */
  startrectfile();                  /* write globals to egret.rect.m */
  compile();                        /* compile infile into output files */

  /*  fclose(infile); */
  endlexfile();
  fclose(lexfile);
  fclose(rectfile);
  fclose(rectheader);

  if (CompleteCompile) {
    fprintf(stderr,"egretc (\"%s\"): flexing %s...\n",infilename,flexfilename);
    sprintf(command,"flex %s %s\n",flexflags,flexfilename);
    system(command);
    fprintf(stderr,"egretc (\"%s\"): finished flexing %s.\n",infilename,flexfilename);

    fprintf(stderr,"egretc (\"%s\"): compiling %s...\n",infilename,outfilename);
    sprintf(command,"cc %s lex.yy.c egret.rect.c egret.util.o  -o %s\n",ccflags,outfilename);
    system(command);
    fprintf(stderr,"egretc (\"%s\"): finished compiling %s.\n",infilename,outfilename);

    if (RemoveIntermediatesCompile) {
      sprintf(command,"rm %s %s %s %s %s\n",flexfilename,"lex.yy.*",rectfilename,rectheadername,"egret.rect.o");
      system(command);
    }
  }
  
  exit(0);
}



/*
 * Writes instructions to lex.
 * This should pretty much all be in a header file, but it is just as easy 
 *   to put it here.
 */

startlexfile()

{
  fprintf(lexfile,      "%%{                                           \n");
  fprintf(lexfile,      "#include \"egret.rect.h\"                     \n");
  fprintf(lexfile,      "#include <string.h>                           \n");
  fprintf(lexfile,      "#include <ctype.h>                            \n");

  /* xw 4/17/95 added for the regcmp(), regex() in Solaris */
  fprintf(lexfile,      "#include <libgen.h>                            \n");

  fprintf(lexfile,      "                                              \n");
  fprintf(lexfile,      "typedef struct sl__ {                         \n");
  fprintf(lexfile,      "  char *str;                                  \n");
  fprintf(lexfile,      "  struct sl__ *next;                          \n");
  fprintf(lexfile,      "} sl_;                                        \n");
  fprintf(lexfile,      "typedef sl_ *sl;                              \n");
  fprintf(lexfile,      "                                              \n");
  fprintf(lexfile,      "extern sl names;                              \n");
  fprintf(lexfile,      "extern sl mesgs;                              \n");
  fprintf(lexfile,      "extern sl sp;                                 \n");
  fprintf(lexfile,      "                                              \n");
  fprintf(lexfile,      "extern int match();                           \n");
  fprintf(lexfile,      "extern char *infill();                        \n");
  /* take malloc declaration out,  flex declare it for us
  fprintf(lexfile,      "extern char *malloc();                        \n");
  */
  fprintf(lexfile,      "                                              \n");
  fprintf(lexfile,      "/* Redefinition of YY_INPUT to read from      \n");
  fprintf(lexfile,      " * eg_instring.                               \n");
  fprintf(lexfile,      " */                                           \n");
  fprintf(lexfile,      "int eg_incharcount = 0;                       \n");
  fprintf(lexfile,      "int eg_eof = 0;                               \n");
  fprintf(lexfile,      "extern char *eg_instring;                     \n");
  fprintf(lexfile,      "#undef YY_INPUT                               \n");
  fprintf(lexfile,      "#define YY_INPUT(buf,result,max_size)       \\\n");
  fprintf(lexfile,      "   result = (                               \\\n");
  fprintf(lexfile,      "     buf[0] =                               \\\n");
  fprintf(lexfile,      "       eg_instring[eg_incharcount] ?        \\\n");
  fprintf(lexfile,      "         eg_instring[eg_incharcount++] :    \\\n");
  fprintf(lexfile,      "         eg_eof ?                           \\\n");
  fprintf(lexfile,      "          (eg_eof--,eg_incharcount=0,'\\0'):\\\n");
  fprintf(lexfile,      "          (eg_eof++,'\\n')                  \\\n");
  fprintf(lexfile,      "     ) ?                                    \\\n");
  fprintf(lexfile,      "       1 :                                  \\\n");
  fprintf(lexfile,      "       YY_NULL;                               \n");
  fprintf(lexfile,      "                                              \n");
  fprintf(lexfile,      "%%}\n"                                           );
  fprintf(lexfile,      "%%%%\n"                                          );
  fprintf(lexfile,      "  extern FILE *eg_outputfile;                    \n");
  fprintf(lexfile,      "  extern FILE *eg_inputfile;                     \n");
  fprintf(lexfile,      "  extern FILE *eg_errorfile;                     \n");
  /*  take *t definition out, it is not an external var 
  fprintf(lexfile,      "  extern char *out, *t;                              \n");
  */
  fprintf(lexfile,      "  extern char *out;                              \n");
  fprintf(lexfile,      "  char  *t;                              \n");

  /* xw 4/17/95, to add p for solaris regcmp, regex */
  fprintf(lexfile,      "  char  *p;                              \n");

  fprintf(lexfile,      "  extern char *Note;                       \n");

}



/* Writes global compiler directives to egret.rect.c */

startrectfile()

{
  fprintf(rectfile,     "#include <stdio.h>                            \n");
  fprintf(rectfile,     "#include <string.h>                           \n");
  fprintf(rectfile,     "#include <ctype.h>                            \n");
  fprintf(rectfile,     "                                              \n");
  fprintf(rectfile,     "char *out;                            \n");
  fprintf(rectfile,     "char *Note = \"\\0\";                       \n");
  fprintf(rectfile,     "                                              \n");

  fprintf(rectfile,     "extern int match();                           \n");
  fprintf(rectfile,     "extern char *intfill();                       \n");
  fprintf(rectfile,     "extern char *malloc();                        \n");

  fprintf(rectfile,     "extern char *mesgs;                           \n");
  fprintf(rectfile,     "extern char *names;                           \n");
  fprintf(rectfile,     "extern char *yytext;                            \n");

  util();

  fprintf(rectfile,      "rectify_level01(func_rec)                       \n");
  fprintf(rectfile,      "char *(*func_rec)();                              \n");
  fprintf(rectfile,      "{                                              \n");

      if ( WarningLine ) {
        fprintf(rectfile,"  sl_add(&mesgs   ,\"%s\");          \n",linewarning);
      }
      fprintf(rectfile,  "  if (!strncmp(out = (*func_rec)(yytext),\"ERROR\",5))\n",linecount);
      fprintf(rectfile,  "    sl_add(&mesgs ,out+5);                       \n");
      fprintf(rectfile,  "  else {                                         \n");
      if ( IncludeNotesLine )
        fprintf(rectfile,"    sl_add(&names,strcat(out,Note));           \n");
      else {
        fprintf(rectfile,"    sl_add(&names,out);                        \n");
        fprintf(rectfile,"    if (!strcmp(Note,\"\\0\"))                    \n");
        fprintf(rectfile,"      sl_add(&mesgs   ,\"Note deleted from input\"); \n");
      }
      fprintf(rectfile,  "  }                                              \n");
      fprintf(rectfile,  "  Note = \"\\0\";                                \n");
      fprintf(rectfile,  " }                                               \n");
      fprintf(rectfile,  "                                                \n");

  fprintf(rectfile,      "rectify_level2(func_rec)                       \n");
  fprintf(rectfile,      "char *(*func_rec)();                              \n");
  fprintf(rectfile,      "{                                              \n");

      if ( WarningLine ) {
        fprintf(rectfile,"  sl_add(&mesgs   ,\"%s\");          \n",linewarning);
      }
      fprintf(rectfile,  "  if (!strncmp(out = (*func_rec)(yytext),\"ERROR\",5))\n",linecount);
      fprintf(rectfile,  "    sl_add(&mesgs ,out+5);                       \n");
      fprintf(rectfile,  "  else {                                         \n");
      if ( IncludeNotesLine )
        fprintf(rectfile,"    sl_add(&names,strcat(out,Note));           \n");
      else {
        fprintf(rectfile,"    sl_add(&names,out);                        \n");
        fprintf(rectfile,"    if (!strcmp(Note,\"\\0\"))                    \n");
        fprintf(rectfile,"      sl_add(&mesgs   ,\"Note deleted from input\"); \n");
      }
      fprintf(rectfile,  "    sl_add(&mesgs ,\"ambiguous input\");         \n");
      fprintf(rectfile,  "  }                                              \n");
      fprintf(rectfile,  " } \n");
      fprintf(rectfile,  "  \n");

}


/* Writes final procedures to flex */

endlexfile()

{
  fprintf(lexfile,      "%%%%\n"                                          );
  fprintf(lexfile,      "void eg_restart()                             \n");
  fprintf(lexfile,      "{                                             \n");
  fprintf(lexfile,      "  yyrestart(stdin);                           \n");
  fprintf(lexfile,      "}                                             \n");
}

/* controls compilation and writes clean up stuff to *flexfile */

compile()

{
  char *t, *eg_regex(), *ed_regex(), *strconst();
  char pattern[256], last_pattern[256];  /* XW 11/2/94 */




/*
 * Compile Notes.
 */


  for (linecount = 1 ; fgets(instring,256,infile) != NULL ; linecount++)
    if (instring[0] == 'n') {
      t = instring;
      setflags(&t);
      rectify(t);
      fprintf(lexfile,  "^.*%s$  { \n"                              ,eg_regex(t));

/*  modify this part for solaris, re_comp=>regcmp, re_exec =>regex 
 */
      fprintf(lexfile,  "  re_comp(\"^%s$\");                 \n",ed_regex(t));
      fprintf(lexfile,  "  for ( t = yytext + strlen(yytext);!re_exec(t);t--)\n");
      fprintf(lexfile,  "    ;                                            \n");
      fprintf(lexfile,  "  for ( ; re_exec(t) ; t-- )                     \n");
      /*
	xw,  4/17/95


      fprintf(lexfile,  "  p = regcmp(\"^%s$\", (char *) 0);  \n",ed_regex(t));
      fprintf(lexfile,  "  for ( t = yytext + strlen(yytext)-1;!regex(p, t);t--)\n");
      fprintf(lexfile,  "    ;                                            \n");
      fprintf(lexfile,  "  for ( ; regex(p, t) ; t-- )                     \n");
      */
      fprintf(lexfile,  "    ;                                            \n");
      fprintf(lexfile,  "  free(p);                                       \n");
      fprintf(lexfile,  "  if (!strncmp(Note = rectify_%d(t+1),\"ERROR\",5)){\n",linecount);
      fprintf(lexfile,  "    sl_add(&mesgs ,Note+5);                      \n");
      fprintf(lexfile,  "    Note = \"\\0\";                              \n");
      fprintf(lexfile,  "  }                                              \n");
      fprintf(lexfile,  "  for ( ; t >= yytext ; t-- )                    \n");
      fprintf(lexfile,  "    unput(*t);                                   \n");
      fprintf(lexfile,  "  unput('\\n');                                  \n");
      fprintf(lexfile,  " }                                               \n");
    }




/*
 * Compile 0> level lines.
 */

  infile = fopen(infilename,"r");
  for (linecount = 1 ; fgets(instring,256,infile) != NULL ; linecount++) 
    if (instring[0] == '0') {
      t = instring;
      setflags(&t);
      rectify(t);
      fprintf(lexfile,  "^%s$  { \n",eg_regex(t));
      fprintf(lexfile,  "  rectify_level01(rectify_%d); \n", linecount);
      fprintf(lexfile,  " }                                               \n");

    }
  fclose(infile);



/* 
 * Compile 1> level lines.
 */

  
  infile = fopen(infilename,"r");
  for (linecount = 1 ; fgets(instring,256,infile) != NULL ; linecount++)
    if (instring[0] == '1') {
      t = instring;
      setflags(&t);
      rectify(t);
      fprintf(lexfile,  "^%s$  { \n",eg_regex(t));
      fprintf(lexfile,  "  rectify_level01(rectify_%d); \n", linecount);
      fprintf(lexfile,  " }                                               \n");

    }
  fclose(infile);
  


/*
 * Compile 2> level lines.
 */


  infile = fopen(infilename,"r");
  for (linecount = 1 ; fgets(instring,256,infile) != NULL ; linecount++ )
    if (instring[0] == '2') {
      t = instring;
      setflags(&t);
      rectify(t);
      fprintf(lexfile,  "^%s$  {                                 \n",eg_regex(t));
      fprintf(lexfile,  "  rectify_level2(rectify_%d); \n", linecount);
      fprintf(lexfile,  "  REJECT;                                        \n");
      fprintf(lexfile,  "}   \n");

    }
  fclose(infile);





/* 
 * Check for erroneous lines.
 */


  infile = fopen(infilename,"r");
  for (linecount = 1 ; fgets(instring,256,infile) != NULL ; linecount++ )
    if ((instring[0] != 'n') &&
	(instring[0] != '0') &&
	(instring[0] != '1') &&
	(instring[0] != '2') &&
	(instring[0] != '<') &&
	(instring[0] != '\n') &&
	(instring[0] != '#') &&
	(instring[0] != '%'))
      fprintf(stderr,"\"%s\": line %d: unrecognized type-specifying character: %c\n",infilename,linecount,instring[0]);
  fclose(infile);

  fprintf(lexfile,      "\\n  ;                                           \n");
  fprintf(lexfile,      "^.*$  {                                          \n");
  fprintf(lexfile,      "  Note = \"\\0\";                                \n");
  fprintf(lexfile,      "  if ( names == NULL && mesgs == NULL )          \n");
  fprintf(lexfile,      "    REJECT;                                      \n");
  fprintf(lexfile,      " }                                               \n");





/* 
 * Compile final sweep lines.
 */


  pattern[0] = '\0';
  last_pattern[0] = '\0';
  infile = fopen(infilename ,"r");
  for (linecount = 1 ; fgets(instring,256,infile) != NULL ; linecount++ )
    if (instring[0] == '<') {
      if (*(t = instring+1) == '>')
	sprintf(pattern,    "^");
      else {
	sprintf(pattern,    "^");
	for (t = instring + 1 ; *t != '>' ; t++ ) {
	  if ( *t == ',' ) 
	    sprintf(pattern,"%s.*$ |\n", pattern);
	  else if (!isdigit(*t))
	    sprintf(pattern,"%s[%c%c]" ,pattern, *t,islower(*t)?toupper(*t):isupper(*t)?tolower(*t):*t);
	  else
	    sprintf(pattern, "%s[%c]", pattern, *t);
	}
      }
      sprintf(pattern,  "%s.*$  {\n" , pattern);
      if (strcmp(pattern, last_pattern) != 0) {
	 if (last_pattern[0]) {
	    fprintf(lexfile,  "  REJECT;\n" );
	    fprintf(lexfile,  " }\n" );
	    }
	 fprintf(lexfile, "%s", pattern);
	 strcpy(last_pattern, pattern);
	 fprintf(lexfile,  "  sl_add(&mesgs,\"%s\");\n",
		strconst(strtok(t+1,"\n")));
	 }
      else
	 fprintf(lexfile,  "  sl_add(&mesgs,\"%s\");\n",
		strconst(strtok(t+1,"\n")));
    }
   
   fprintf(lexfile,  "  REJECT;\n" );
   fprintf(lexfile,  " }\n" );

  fclose(infile);

  fprintf(lexfile,      "^.*$\n"                                             );
}



/* reads flag settings to egret written on the instruction line and sets the appropriate flags */

setflags(tptr)

char **tptr;
{
  int i;

  CaseInsensitiveLine = CaseInsensitiveCompile;
  IncludeNotesLine = IncludeNotesCompile;
  CollapseSpacesLine = CollapseSpacesCompile;
  WarningLine = OFF;

  while (*(*tptr)++ != '>') 
    switch ( **tptr ) {
    case '*':
      CaseInsensitiveLine = !CaseInsensitiveCompile;
      break;
    case 'c':
      CaseInsensitiveLine = ON;
      break;
    case 'C':
      CaseInsensitiveLine = OFF;
      break;
    case 'n':
      IncludeNotesLine = ON;
      break;
    case 'N':
      IncludeNotesLine = OFF;
      break;
    case 's':
      CollapseSpacesLine = ON;
      break;
    case 'S':
      CollapseSpacesLine = OFF;
      break;
    case '"': 
      WarningLine = ON;
      for ( i = 0 ; (linewarning[i] = *++(*tptr)) != '"' ; i++ )
	;
      linewarning[i] = '\0';
      break;
    default:
      break;
    }
}



/* returns an "ed" regular expression for the input line */

char *ed_regex(s)

char *s;
{
  char  *eg_regex(); /* xw 4/16/95 */
  static char out[256];
  char *in;
  int i = 0, j = 0;
 
  in = eg_regex(s);             /* to save work we use eg_regex, and translate */
  for ( i = 0 ; in[i] != '\0' ; i++)
    switch (in[i]) {
    case '"': 
      break;
    case '+':
      out[j++] = out[j-1];
      out[j++] = '*';
      break;
    case '{':
      while ( in[i] != '}' ) ++i;
      out[j++] = '*';
      break;
    case '?':
      if (out[j-1] == '*' && out[j-2] == out[j-3]) {
	j -= 2;
	out[j++] = '*';
      }
      else 
	out[j++] = '*';
      break;
    case '(': 
      break;
    case ')':
      break;
    case '|':
      if ( in[i-1] != '\\' ) {
	fprintf(stderr,"egretc (\"%s\", line %d): ERROR: {...} construction not permitted in notes.\n",infilename,linecount);
	exit(1);
      }
    default:
      out[j++] = in[i];
      break;
    }
  out[j++] = '\0';
  return(out);
}
      
      

/* returns a "lex" regular expression matching the input line */

char *eg_regex(s)

char *s;                      /* input expression */
{
  static char out[256];     
  int digits,i,j;       

  j = 0;
  for ( i = 0 ; s[i] != '\0' ; ) /* generates the regular expression. see egret.doc for an explanation */
    switch (s[i]) {              /* of the commands */
    case '{':
      out[j++] = '(';
      while ( s[i++] != '}' ) {
        if (s[i] == '(')
          copy(out,&j,s,&i,'(');
        else 
	  copy(out,&j,s,&i,'[');
        do {
        } while ( s[i++] != ')' );
	if ( s[i] == ':' ) 
	  out[j++] = '|';
        if ( s[i] == '*' ) {
	  i++;
	  do {
	  } while ( s[i++] != ')' );
	  out[j++] = ')';
	  out[j++] = '?';
        }
      } 
      if ( out[j-1] != '?' )
	out[j++] = ')';
      break;
    case '[':
      copy(out,&j,s,&i,'[');
      break;
    case '(':
      copy(out,&j,s,&i,'(');
      break;
    case 'd':
      i++;
      if (s[i] == '(')
	copy(out,&j,s,&i,'(');
      else 
	copy(out,&j,s,&i,'[');
      break;
    case 'r':
      i++;
      if (s[i] == '(')
	copy(out,&j,s,&i,'(');
      else
	copy(out,&j,s,&i,'[');
      do {
      } while (s[i++] != ')');
      break;
    case '?':
    case '<':
    case '>':
      i++;
      if (s[i] == '(')
	copy(out,&j,s,&i,'(');
      else 
	copy(out,&j,s,&i,'[');
      out[j++] = '?';
      break;
    case '/':
      i++;
      if (s[i] == '(')
	copy(out,&j,s,&i,'(');
      else 
	copy(out,&j,s,&i,'[');
      out[j++] = '?';
      do {
      } while (s[i++] != ')');
      break;
    case '#':
      i++;
      for ( digits = 0; isdigit(s[i]); digits++)
	i++;
      out[j++] = '(';
      out[j++] = '[';
      out[j++] = '0';
      out[j++] = '-';
      out[j++] = '9';
      out[j++] = ']';
      out[j++] = '{';
      out[j++] = '1';
      out[j++] = ',';
      out[j++] = '0' + digits;
      out[j++] = '}';
      out[j++] = ')';
      if ( s[i] == ',' ) {
	i++;
	for ( digits = 0; isdigit(s[i]); digits++ )
	  i++;
      }
      break;
    case '\n':
      out[j] = '\0';
      i++;
      break;
    default:
      fprintf(stderr,"egretc, line %d: ERROR: bad line: %s",linecount,instring);
      exit(1);
    }
  fprintf(stderr,"%s\n",out);
  return(out);
}



/* This is the main copy loop for eg_regex() */

copy(out,j,s,i,c)

char *out,*s;
int *j,*i;
char c;
{
  if (c == '[') {
    for (;s[*i] != ']';(*i)++) {
      if (s[*i] == ' '){
	out[(*j)++] = s[*i];
      }
      else if (strchr("\"\\.^<$?*+|(/{}",s[*i]) != NULL) {          /* These are Lex operators */
	out[(*j)++] = '\\' ; out[(*j)++] = s[*i];
      }
      else if ( CaseInsensitiveLine && isupper(s[*i])) {
	out[(*j)++] = s[*i] ; out[(*j)++] = tolower(s[*i]);
      }
      else if ( CaseInsensitiveLine && islower(s[*i])) {
	out[(*j)++] = s[*i] ; out[(*j)++] = toupper(s[*i]);
      }
      else if ( CaseInsensitiveLine && s[*i] == '-' && s[*i-1] != '[' && s[*i+1] != ']' )  {
	out[(*j)++] = s[*i-1] ; out[(*j)++] = s[*i] ; out[(*j)++] = s[*i+1] ;
	if (islower(s[*i-1]) && islower(s[*i+1]))  {
	  out[(*j)++] = toupper(s[*i-1]) ; out[(*j)++] = s[*i] ; out[(*j)++] = toupper(s[*i+1]) ;
	}
	else if (isupper(s[*i-1]) && isupper(s[*i+1]))  {
	  out[(*j)++] = tolower(s[*i-1]) ; out[(*j)++] = s[*i] ; out[(*j)++] = tolower(s[*i+1]) ;
	}
	else if (isalpha(s[*i-1]) && isalpha(s[*i+1]))  {
	  fprintf(stderr,"egretc: warning: nonportable range specification\n");
	}
      }
      else 
	out[(*j)++] = s[*i];
    } 
    out[(*j)++] = s[(*i)++];
  }
  else {
    for (;s[*i] != ')';(*i)++) {
	if (s[*i] == ' '){
	  out[(*j)++] = '(';out[(*j)++] = '"'; out[(*j)++] = s[*i]; out[(*j)++] = '"'; out[(*j)++] = ')'; out[(*j)++] = '+';
	}
	else if (strchr("\"\\.[]^<$?*+|/{}",s[*i]) != NULL) {        /* These are Lex operators */
	  out[(*j)++] = '\\' ; out[(*j)++] = s[*i];
	}
	else if ( CaseInsensitiveLine && isupper(s[*i])) {
	  out[(*j)++] = '['; out[(*j)++] = s[*i] ; out[(*j)++] = tolower(s[*i]); out[(*j)++] = ']';
	}
	else if ( CaseInsensitiveLine && islower(s[*i])) {
	  out[(*j)++] = '['; out[(*j)++] = s[*i] ; out[(*j)++] = toupper(s[*i]); out[(*j)++] = ']';
	}
	else
	  out[(*j)++] = s[*i];
      } 
    out[(*j)++] = s[(*i)++];
  }
}


util()
{
    fprintf(rectfile,      "rect_util(s)                                         \n");
    fprintf(rectfile,      "char *s;                                             \n");
    fprintf(rectfile,      "{                                                    \n");
  fprintf(rectfile,        "  int i,j,k;                                         \n");
    fprintf(rectfile,      "  for ( i = 0 ; s[i] != '\\0' ; i++ )                \n");
    fprintf(rectfile,      "    if (islower(s[i]))                               \n");
    fprintf(rectfile,      "      s[i] = toupper(s[i]);                          \n");
    fprintf(rectfile,      "  for ( i = 0 ; s[i] != '\\0' ; i++ )                \n");
    fprintf(rectfile,      "    if ( s[i] == ' ' ) {                             \n");
    fprintf(rectfile,      "      for ( j = i+1 ; s[j] == ' ' ; j++ );           \n");
    fprintf(rectfile,      "      for ( k = 0;(s[i+1+k] = s[j+k]) != '\\0';k++ ) \n");
    fprintf(rectfile,      "        ;                                            \n");
    fprintf(rectfile,      "    }                                                \n");
    fprintf(rectfile,      "}                                                    \n");

  fprintf(rectfile,        " \n");
  fprintf(rectfile, "char *rect_util2(errorformat, error, out)\n");
  fprintf(rectfile, "char  *errorformat, *error, *out; \n");
  fprintf(rectfile,      "{                                                    \n");
  fprintf(rectfile,        "  if (errorformat == NULL) {                         \n");
  fprintf(rectfile,        "    return(out);                                     \n");
  fprintf(rectfile,        "  }                                                  \n");
  fprintf(rectfile,        "  else {                                             \n");
  fprintf(rectfile,        "    sprintf(error,errorformat,out);                  \n");
  fprintf(rectfile,        "    free(errorformat);                               \n");
  fprintf(rectfile,        "    return(error);                                   \n");
  fprintf(rectfile,        "  }                                                  \n");
  fprintf(rectfile,        "}                                                  \n");
  fprintf(rectfile,        " \n");



fprintf(rectfile, "rect_utildigit(s, out, errorformat, max, min, maxdigits)\n");

   fprintf(rectfile, "char *s, *out, **errorformat;\n");
   fprintf(rectfile, "int max, min, maxdigits; \n");
   fprintf(rectfile, "{  \n");
   fprintf(rectfile,    "  int i, len, digits, moves;  \n");
   fprintf(rectfile,    "  char *t, tmps[100];  \n");
   fprintf(rectfile,    "  moves = 0;  \n");
   fprintf(rectfile,    "  for(digits=0;isdigit(*s)&&digits<maxdigits;digits++) { \n" );
   fprintf(rectfile,    "    s++;  moves++;   }    \n");
   fprintf(rectfile,    "  t = intfill(s-digits,max,min,maxdigits,digits); \n");
   fprintf(rectfile,    "  if (!strcmp(t,\"error\")) {  \n");
   fprintf(rectfile,    "    strncat(out,s - digits,digits);  \n");
   fprintf(rectfile,    "    *errorformat = malloc(256);       \n");
   fprintf(rectfile,    "    sprintf(*errorformat,\"ERRORNumber out of range: %%%%s\\n\");\n");
   fprintf(rectfile,    "    len = strlen(\"Number out of range: \"); \n");

   /*xw, 4/17/95   cast the value of strlen() into int explicitly for ANSI 
   fprintf(rectfile,    "    for(i = 0 ; i < len+strlen(out)-1 ; i++ )         \n");
   */
   fprintf(rectfile,    "    for(i = 0 ; i < len+(int)strlen(out)-1 ; i++ )         \n");
   fprintf(rectfile,    "      strcat(*errorformat,\" \");     \n");
   fprintf(rectfile,    "    sprintf(tmps,\"^ range = [%%d,%%d]\", min,max);  \n");
   fprintf(rectfile,    "    strcat(*errorformat,tmps);  \n");
   fprintf(rectfile,    "  }                                 \n");
   fprintf(rectfile,    "  else                              \n");
   fprintf(rectfile,    "    strncat(out,t,maxdigits);       \n");
   fprintf(rectfile,    "  free(t);       \n");
   fprintf(rectfile,    "  return(moves);       \n");
   fprintf(rectfile,    "}       \n");
   fprintf(rectfile,    "\n");
}

/* creates the rectify function for string (s) */

rectify(s)

char *s;
{



  fprintf(rectheader,      "extern char *rectify_%d();                           \n",linecount);


  fprintf(rectfile,        "char *rectify_%d(s)                                  \n",linecount);
  fprintf(rectfile,        "                                                     \n");
  fprintf(rectfile,        "char *s;                                             \n");
  fprintf(rectfile,        "{                                                    \n");
  fprintf(rectfile,        "  static char out[256], error[256];                  \n");
  fprintf(rectfile,        "  int i,j,k, moves;                                         \n");
  fprintf(rectfile,        "  char *errorformat;                                 \n");
  fprintf(rectfile,        "  char *t;                                           \n");
  fprintf(rectfile,        "  int digits;                                        \n");

  fprintf(rectfile,        "  errorformat = NULL;                                \n");
  fprintf(rectfile,        "  *out = '\\0';                                      \n");
  fprintf(rectfile,        "  rect_util(s);                                      \n");

  expand(&s);

  fprintf(rectfile,        "  return(rect_util2(errorformat, error, out)); \n");
  fprintf(rectfile,        "}                                                    \n");

}


/* prints the body of the rectify function  */

expand(sptr)

char **sptr;
{
  char *s;
  char *nextstring(), *tempstring, *strconst();
  int i, max, maxdigits, min, mindigits; 
  
  
  s = *sptr;
  
  i = 0;
  while((s[i] != '\0') && (s[i] != '\n'))
    switch (s[i]) {
    case '{':  /* The generalized replacement form. All the commands below 
		  are a specialized version of this. */
      fprintf(rectfile,    "  if (0) {}                 \n");
      while ( s[i++] != '}' ) {
	switch (s[i]) {
	case '(':
	  tempstring = nextstring(s+i);
	  fprintf(rectfile,"  else if (match(s,\"(%s)\")) {\n",strconst(tempstring));
	  fprintf(rectfile,"    s += %d;           \n",strlen(tempstring));
	  i += strlen(tempstring) + 2;
	  tempstring = nextstring(s+i);
	  fprintf(rectfile,"    strcat(out,\"%s\"); \n",strconst(tempstring));
	  fprintf(rectfile,"  }                     \n");
	  i += strlen(tempstring) + 2;
	  break;
        case '[':
	  tempstring = nextstring(s+i);
	  fprintf(rectfile,"  else if (match(s,\"[%s]\")) {\n",strconst(tempstring));
	  fprintf(rectfile,"    s++;                 \n");
	  i += strlen(tempstring) + 2;
	  tempstring = nextstring(s+i);
	  fprintf(rectfile,"    strcat(out,\"%s\"); \n",strconst(tempstring));
	  fprintf(rectfile,"  }                     \n");
	  i += strlen(tempstring) + 2;
	  break;
        default:
	  fprintf(stderr,"egretc: ERROR: missing paranthesis at postion %d: %s\n",i,instring);
        }
	if (s[i] == ':') {
	}
	else if (s[i] == '}') {
	}
	else if (s[i] == '*') {
	  tempstring = nextstring(s+(++i));
	  fprintf(rectfile,"  else {               \n");
	  fprintf(rectfile,"    strcat(out,\"%s\");\n",strconst(tempstring));
	  fprintf(rectfile,"  }                    \n");
	  i += strlen(tempstring) + 2;
        }
	else {
	  fprintf(stderr,"egret: ERROR: missing ':' at position %d: %s\n",i,instring);
        }
      }
      break;
    case '[':  /* Direct match */
      tempstring = nextstring(s + i);
      fprintf(rectfile,    "  strncat(out,s,1);    \n");
      fprintf(rectfile,    "  s++;                 \n");
      i += strlen(tempstring) + 2;
      break;
    case '(':  /* Direct match */
      tempstring = nextstring(s + i);
      fprintf(rectfile,    "  strcat(out,\"%s\");  \n",strconst(tempstring));
      fprintf(rectfile,    "  s += %d;             \n",strlen(tempstring));
      i += strlen(tempstring) + 2;
      break;
    case 'd':  /* Direct match, delete */
      tempstring = nextstring(s + (++i));
      if (s[i] == '[')
	fprintf(rectfile,  "  s++;                 \n");
      else
	fprintf(rectfile,  "  s += %d;             \n",strlen(tempstring));
      i += strlen(tempstring) + 2;
      break;
    case 'r':  /* Direct match, replace */
      tempstring = nextstring(s + (++i));
      if (s[i] == '[')
	fprintf(rectfile,  "  s++;                 \n");
      else
	fprintf(rectfile,  "  s += %d;             \n",strlen(tempstring));
      i += strlen(tempstring) + 2;
      tempstring = nextstring(s + i);
      fprintf(rectfile,    "  strcat(out,\"%s\");  \n",strconst(tempstring));
      i += strlen(tempstring) +2;
      break;
    case '?':  /* Optional match */
      tempstring = nextstring(s + (++i));
      if (s[i] == '(') {
	fprintf(rectfile,  "  if (match(s,\"(%s)\")) {\n",strconst(tempstring));
	fprintf(rectfile,  "    strncat(out,s,%d);    \n",strlen(tempstring));
	fprintf(rectfile,  "    s += %d;              \n",strlen(tempstring));
	fprintf(rectfile,  "  }                       \n");
      }
      else {
	fprintf(rectfile,  "  if (match(s,\"[%s]\")) {\n",strconst(tempstring));
	fprintf(rectfile,  "    strncat(out,s,1);     \n");
	fprintf(rectfile,  "    s++;\n  }\n");
      }
            i += strlen(tempstring) + 2;
      break;
    case '<':  /* Optional match, delete */
      tempstring = nextstring(s + (++i));
      if (s[i] == '(') {
	fprintf(rectfile,  "  if (match(s,\"(%s)\"))  \n",strconst(tempstring));
	fprintf(rectfile,  "    s += %d;              \n",strlen(tempstring));
      }
      else {
	fprintf(rectfile,  "  if (match(s,\"[%s]\")) \n",strconst(tempstring));
	fprintf(rectfile,  "    s++;                 \n");
      }
      i += strlen(tempstring) + 2;
      break;
    case '>':  /* Optional match, include */
      tempstring = nextstring(s + (++i));
      fprintf(rectfile,    "  strcat(out,\"%s\");    \n",strconst(tempstring));
      fprintf(rectfile,    "  if (!strncmp(s,\"%s\",%d)) \n",strconst(tempstring),strlen(tempstring));
      fprintf(rectfile,    "    s += %d;               \n",strlen(tempstring));
      i += strlen(tempstring) + 2;
      break;
    case '/':  /* Optional match, replace */
      tempstring = nextstring(s + (++i));
      if (s[i] == '(') {
	fprintf(rectfile,  "  if (match(s,\"(%s)\")) \n",strconst(tempstring));
	fprintf(rectfile,  "    s += %d;             \n",strlen(tempstring));
      }
      else {
	fprintf(rectfile,  "  if (match(s,\"[%s]\")) \n",strconst(tempstring));
	fprintf(rectfile,  "    s++;                 \n");
      }
      i += strlen(tempstring) + 2;
      tempstring = nextstring(s + i);
      fprintf(rectfile,    "  strcat(out,\"%s\");    \n",strconst(tempstring));
      i += strlen(tempstring) + 2;
      break;
    case '#':  /* numbers */
      i++;
      for ( maxdigits = 0 ; isdigit(s[i + maxdigits]) ; maxdigits++ )
	;
      sscanf(s + i,"%d",&max);
      if ( s[i + maxdigits] == ',' ) {
	for ( mindigits = 0 ; isdigit(s[i + maxdigits + mindigits + 1]) ; mindigits ++ )
	  ;
	sscanf(s + i + maxdigits ,",%d",&min);
      }
      else {
	mindigits = 0;
	min = DEFAULTMINIMUM;
      }
      fprintf(rectfile, "  moves = rect_utildigit(s, out, &errorformat, %d, %d, %d); \n", max, min, maxdigits);
      fprintf(rectfile, "  s += moves; \n");

      i += maxdigits + ( mindigits ? mindigits+1 : 0 );
      break;
    case '\n':
      break;
    default:
      return;
    }
  return;
}







/* Takes a string of the form "(text1)text2" or "[text1]text2 and returns a
   string containing "text1". If either parenthesis or bracket is absent an 
   error message is generated and compilation aborted */
  
char *nextstring(s)

char *s;
{
  static char out[256];
  char str_type;
  int i;

  if (s[0] == '(') {
    str_type = ')';
  }
  else if (s[0] == '[') {
    str_type = ']';
  }
  else { 
    fprintf(stderr,"egretc, line %d: ERROR: missing '(' in line.",linecount);
    exit(1);
  }


  for ( i = 1 ; (s[i] != str_type) && (s[i] != '\0') ; i++ ) 
    out[i-1] = s[i];
  
  if (s[i] == '\0') { 
    fprintf(stderr,"egretc, line %d: ERROR: mismatched parentheses.",linecount);
    exit(1);
  }

  out[i-1] = '\0';
  return(out);
}




/* Inserts '\' characters at judicious points a string so as to allow it to be
   printed out and read as a string constant */

char *strconst(s)

char *s;
{
  static char out[256];
  int i = 0;

  for( ; *s != '\0' ; s++ ) {
    if ( *s == '\\' || *s == '"' )
      out[i++] = '\\';
    out[i++] = *s;
  }
  out[i++] = '\0';
  return(out);
}
