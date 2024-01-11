//
// Created by OMER on 1/10/2024.
//

#include "passOne.h"
#include <stdlib.h>
#include <ctype.h>
#include "input.h"

/*
* first path:
* 1) symbols table:
 label | address (IC/DC) or val | type (cmd/data/mdefine) | external | entry

* 2) create data segment, compute its size DC (from 100)
* 3) create partial text segment, compute its size IC (from 0)
* 4) create external table: | label | line that appears in program
* 5.1) create entry table - text: | label | address
* 5.2) create entry table - data: | label | address
* 6) update address of data in the data segment (data is after text)
*/

f_processLine (char *line, int line_num, char* first_word,
               LinkedList *symbols_table, char *curr_symbol);

int isSymbol (const char *first_word)
{
  return first_word[strlen (first_word) - 1] == ':';
}


int isValidStr(char *line) {

  return FALSE;
}


int passOne (FILE *input)
{
  char line[MAX_LINE_SIZE], line_copy[MAX_LINE_SIZE];
  char *first_word = NULL, *curr_label = NULL;
  int line_num = 1, IC = 100, DC = 0;
  LinkedList *symbols_table = createList (NULL, NULL, free);
  if (!symbols_table) {
    return EXIT_FAILURE; /* memory error */
  }

  while (fgets (line, MAX_LINE_SIZE, input)) {
    first_word = strtok (line, DELIM);
    curr_label = NULL;

    if (isSymbol (first_word)) {
      curr_label = first_word;
      first_word = strtok (NULL, DELIM);
      f_processLine (line, line_num, first_word, symbols_table,
                     curr_label);
      // if label - pass the line without the label (cut first word)
    }
    else {
      f_processLine (line, line_num, first_word, symbols_table, curr_label);
      //else pass the origin line (don't cut the first word)
    }

    return EXIT_SUCCESS;
  }
}


  int f_processLine (char *line, int line_num, char* first_word,
                     LinkedList *symbols_table, char *curr_symbol)
  {
  if (strcmp (".string", first_word) == 0){
    if (isValidStr (line)){

    }
  }
    return TRUE;
  }
