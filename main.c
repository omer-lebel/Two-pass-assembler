//
// Created by OMER on 1/10/2024.
//

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "preAssembler.h"
#include "firstPass.h"

enum mod{
    PRE=0, FIRST, SEC
};

#define INPUT_IND 1
#define MOD_IND 2


int main (int argc, char *argv[])
{

  FILE *src_file, *am_file, *fp_src;
  char fileName[100];
  int res;

  if (argc <= 1){
    printf ("must give at least one file to process\n");
    return EXIT_FAILURE;
  }

  src_file = fopen (argv[INPUT_IND], "r");
  if (!src_file) {
    printf ("error while opening input file\n");
    return EXIT_FAILURE;
  }
  strcpy (fileName, argv[INPUT_IND]);

  //pre
  if (argv[MOD_IND] == PRE){
    strcat (fileName, ".am");
    am_file = fopen (fileName, "w");
    if (!am_file) {
      printf ("error while opening %s.am file\n",argv[1]);
      fclose (src_file);
      return EXIT_FAILURE;
    }

    res = preAssembler (src_file, am_file);
    if (res != TRUE){
      fclose (src_file);
      fclose (am_file);
      return EXIT_FAILURE;
    }
    fp_src = am_file;
  }
  else{
    fp_src = src_file;
  }

  //first
  res = firstPass (fp_src, fileName);
  fclose (src_file);

  if (argc == 2){
    fclose (fp_src);
  }


  if (res == FATAL){
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

/*

LineInfo curLine;
char prefix[MAX_LINE_SIZE] = "";
char token[MAX_LINE_SIZE] = "";
char postfix[MAX_LINE_SIZE] = " LABEL:     .data 1,xxx,2";
char file_Name[MAX_LINE_SIZE] = "fileName";
char *p = postfix;

curLine.prefix = prefix;
curLine.token = token;
curLine.postfix = &p;
curLine.file = file_Name;
curLine.num = 4;

printf ("pre: \"%s\" \t tok:\"%s\" \t post:\"%s\"\n", curLine.prefix,
curLine.token, *curLine.postfix);

lineTok (&curLine);
printf ("pre: \"%s\" \t tok:\"%s\" \t post:\"%s\"\n", curLine.prefix,
curLine.token, *curLine.postfix);

lineTok (&curLine);
printf ("pre: \"%s\" \t tok:\"%s\" \t post:\"%s\"\n", curLine.prefix,
curLine.token, *curLine.postfix);

lineTok (&curLine);
printf ("pre: \"%s\" \t tok:\"%s\" \t post:\"%s\"\n", curLine.prefix,
curLine.token, *curLine.postfix);

lineTok (&curLine);
printf ("pre: \"%s\" \t tok:\"%s\" \t post:\"%s\"\n", curLine.prefix,
curLine.token, *curLine.postfix);
r_error("argument", &curLine, "undeclared (first use in this directive)");

lineTok (&curLine);
printf ("pre: \"%s\" \t tok:\"%s\" \t post:\"%s\"\n", curLine.prefix,
curLine.token, *curLine.postfix);
r_error("argument", &curLine, "undeclared (first use in this directive)");

lineTok (&curLine);
printf ("pre: \"%s\" \t tok:\"%s\" \t post:\"%s\"\n", curLine.prefix,
curLine.token, *curLine.postfix);
r_warning("argument", &curLine, "undeclared (first use in this directive)");

lineTok (&curLine);
printf ("pre: \"%s\" \t tok:\"%s\" \t post:\"%s\"\n", curLine.prefix,
curLine.token, *curLine.postfix);

lineTok (&curLine);
printf ("pre: \"%s\" \t tok:\"%s\" \t post:\"%s\"\n", curLine.prefix,
curLine.token, *curLine.postfix);

lineTok (&curLine);
printf ("pre: \"%s\" \t tok:\"%s\" \t post:\"%s\"\n", curLine.prefix,
curLine.token, *curLine.postfix);
*/



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