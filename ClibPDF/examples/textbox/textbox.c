/* textbox.c -- Program for testing cpdf_rawTextBox() function - Also a TEXT2PDF program.
 * Copyright (C) 1998, 1999 FastIO Systems, All Rights Reserved.
 * For conditions of use, license, and distribution, see LICENSE.pdf
 * included in the distribution or http://www.fastio.com/LICENSE.pdf

OPENSTEP
cc -Wall -DUNIX -o textbox textbox.c -I/usr/local/include -lcpdfpm
NEXTSTEP
cc -Wall -DUNIX -s -object -o textbox textbox.c -lcpdfpm

BSDI/FreeBSD/gcc
gcc -Wall -DUNIX -o textbox textbox.c -L/usr/local/lib -lcpdfm -lm

HP-UX B.11.00 (assuming header and lib are installed, or in examples directory below):
cc -O -Aa -Ae +Z +DA1.0 -DHPUX -DUNIX -I /usr/local/include -I.. -L.. -o textbox textbox.c -lcpdfm -lm

1999-08-22 [io] for v2.00
1999-06-05 [IO] First version.

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "cpdflib.h"

#define NUMCOLS		2	/* === Number of text columns per page (1 or 2) */

#ifdef UNIX
#define TEXTFILE	"textUnix.txt"
#endif

#if defined(WIN32) || defined(_WIN32)
#define TEXTFILE	"textWindows.txt"
#endif

#if defined(__MWERKS__) || defined(THINK_C)
#define TEXTFILE	"textMac.txt"
#endif

#ifndef TEXTFILE
#define TEXTFILE	"textUnix.txt"
#endif

#define LLM		72.0
#define RLM		315.0
#define BM		72.0
#define COLUMN1WIDTH	468.0
#define COLUMN2WIDTH	225.0
#define COLUMNHEIGHT	648.0

int main(int argc, char *argv[])
{
CPDFdoc *pdf;
FILE *fp;
char *textdata, *currtext;
long filesize, nbread;
int page =1;
int nColumns = NUMCOLS;
float wColumn;
float fontsize = 12.0;
float linespace = 14.0;
CPDFtboxAttr tboxatr;
char numbuf[64];

    if(nColumns == 2)  wColumn = COLUMN2WIDTH;
    else               wColumn = COLUMN1WIDTH;

    filesize = getFileSize(TEXTFILE);
    if(filesize <= 0) {
	fprintf(stderr, "Zero-size file or no such file: %s\n", TEXTFILE);
	exit(1);
    }
    /* Open the text file in its respective native format, sigh. */
    if((fp = fopen(TEXTFILE, "r")) == NULL) {
	fprintf(stderr, "Can't open file: %s\n", TEXTFILE);
	exit(1);
    }

    /* Get the whole text from file into a memory buffer.  Memory map if you have decent VM. */
    textdata = (char *)malloc(filesize+1024);
    if(textdata == NULL) {
	fprintf(stderr, "Can't malloc() memory\n");
	exit(1);
    }
    nbread = fread((void *)textdata, 1, (size_t)filesize, fp);
    fclose(fp);
    textdata[nbread] = '\0';	/* terminate as string */
    currtext = textdata;	/* initialize pointer */

    /* == Initialization == */
    pdf = cpdf_open(0, NULL);
    cpdf_enableCompression(pdf, YES);		/* use Flate/Zlib compression */
    cpdf_setOutputFilename(pdf, "textbox.pdf");
    cpdf_init(pdf);

    /* PDF Info object */
    cpdf_setCreator(pdf, "textbox.c");
    cpdf_setTitle(pdf, "Example for cpdf_textBox() function");
    cpdf_setSubject(pdf, "ANSI C Library for Direct PDF Generation");
    cpdf_setKeywords(pdf, "ClibPDF, FastIO Systems, ANSI C Library, Acrobat, PDF, text2pdf, Dynamic PDF, CGI");

    tboxatr.alignmode = TBOX_JUSTIFY;
    tboxatr.NLmode = 0;			/* reformat */
    tboxatr.paragraphSpacing = fontsize;
    tboxatr.noMark = 0;			/* reformat */

    /* ====== MAIN TEXT2PDF LOOP ===================================================== */
    while(currtext) {
        cpdf_pageInit(pdf, page, PORTRAIT, LETTER, LETTER);			/* create page */
	cpdf_beginText(pdf, 0);
	cpdf_setFont(pdf, "Times-Roman", "MacRomanEncoding", fontsize);
	/* left column */
	currtext = cpdf_rawTextBox(pdf, LLM, BM, wColumn, COLUMNHEIGHT, 0.0, linespace, &tboxatr, currtext);
	/* right column */
	if(currtext && nColumns > 1)
	    currtext = cpdf_rawTextBox(pdf, RLM, BM, wColumn, COLUMNHEIGHT, 0.0, linespace, &tboxatr, currtext);
	sprintf(numbuf, "page %d", page);
	cpdf_rawTextAligned(pdf,  540.0, 40.0, 0.0, TEXTPOS_LR, numbuf);	/* page number */
        cpdf_endText(pdf);
	cpdf_finalizePage(pdf, page);					/* close page */
	page++;
    }
    /* ====== That's it for multi-column, multi-page text formatting. Cool? === */

    /* === All drawing done.  Finalize the content === */
    cpdf_finalizeAll(pdf);			/* PDF file/memstream is actually written here */
    /* cpdf_savePDFmemoryStreamToFile(pdf, "textbox.pdf"); */

    /* == Clean up == */
    cpdf_launchPreview(pdf);		/* launch Acrobat/PDF viewer on the output file */
    cpdf_close(pdf);			/* shut down */
    return(0);
}


