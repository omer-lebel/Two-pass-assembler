//
// Created by OMER on 1/10/2024.
//

#include <stdio.h>
//#include "preAssembler.h"
#include <ctype.h>
#include "passOne.h"
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"



int main (int argc, char *argv[])
{

  Line curLine;
  char prefix[MAX_LINE_SIZE] = "";
  char token[MAX_LINE_SIZE] = "";
  char postfix[MAX_LINE_SIZE] = " LABEL:     .data 1,xxx,2";
  char *p = postfix;

  curLine.prefix = prefix;
  curLine.token = token;
  curLine.postfix = &p;

  lineTok (&curLine);
  lineTok (&curLine);
  lineTok (&curLine);
  lineTok (&curLine);
  lineTok (&curLine);

  r_error(&curLine, 4, "undeclared (first use in this directive)", 1);



//  FILE *txt_file, *as_file;
//  char pre_output[100];
//  int res;
//
//  if (argc <= 1){
//    printf ("must give at least one file to process\n");
//    return EXIT_FAILURE;
//  }
//
//  txt_file = fopen (argv[1], "r");
//  if (!txt_file) {
//    printf ("error while opening input file\n");
//    return EXIT_FAILURE;
//  }
//  passOne(txt_file);

/*  pre
 strcpy (pre_output, argv[1]);
  strcat (pre_output, ".am");
  as_file = fopen (pre_output, "w");
  if (!as_file) {
    printf ("error while opening %s.as file\n",argv[1]);
    return EXIT_FAILURE;
  }

  res = preAssembler (txt_file, as_file);
  */

//  fclose (txt_file);
//  fclose (as_file);


  return EXIT_SUCCESS;
}

/* printf(RED "red\n"     ); //error
 printf(GRN "green\n"   );
 printf(YEL "yellow\n"  ); //couse for error
 printf(BLU "blue\n"    ); //note
 printf(MAG "magenta\n" );
 printf(CYN "cyan\n"    );
 printf(WHT "white\n"   );
 printf ("reset\n");*/


/*
* first path:
* 1) create symbols table: | label | address | type | external? |
* 2) create data segment, compute its size DC (from 100)
* 3) create partial text segment, compute its size IC (from 0)
* 4) create external table: | label | line that appears in program
* 5.1) create entry table - text: | label | address
* 5.2) create entry table - data: | label | address
* 6) update address of data in the data segment (data is after text)
*
* second path:
* 1) updating address of unknown labels in text segment and in entry tables
* 2) print entry tables
* 3) convert text and data segment to 4 base and print it
*/