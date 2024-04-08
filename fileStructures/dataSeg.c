/**
 * @file dataSeg.h
 * @brief Defines functions for managing the data segment.
 *
 * Provides functions to create a new data segment, add data to it,
 * display its content, and print it with memory indices in the machine
 * language.
 */

#include "dataSeg.h"

void print_data_word(const void* elem, FILE *stream);

Data_Segment *new_data_segment (int *DC)
{
  *DC = 0;
  return create_vector (sizeof (DsWord), NULL, NULL);
}

Bool add_to_data_segment (Data_Segment *data_segment, int *DC,
                          DsType type, void *arr, int size)
{
  DsWord word;
  int *intArr;
  char *charArr;
  int target_dc = *DC + size;

  switch (type) {
    case INT_TYPE:
      intArr = (int *) arr;
      for (; *DC < target_dc; (*DC)++) {
        word.type = type;
        word.val = *intArr++;
        if (!push (data_segment, &word)) {
          return FALSE;
        }
      }
      break;
    case CHAR_TYPE:
      charArr = (char *) arr;
      for (; *DC < target_dc; (*DC)++) {
        word.type = type;
        word.val = (int) *charArr++;
        if (!push (data_segment, &word)) {
          return FALSE;
        }
      }
      break;
  }
  return TRUE;
}

void display_data_segment (Data_Segment *data_segment, FILE *stream)
{
  fprintf (stream, "\n----------------- data segment -----------------\n");
  print_vector (data_segment, print_data_word, stream, "|", "|\n");
}

void print_data_word(const void* elem, FILE *stream){
  char c;
  DsWord *ds_word = (DsWord*) elem;
  if (ds_word->type == INT_TYPE) {
    fprintf (stream, " %d ", ds_word->val);
  }
  else if ((c = (char) ds_word->val) != '\0') { /* char */
    fprintf (stream, " %c ", c);
  }
  else { /* '\0' */
    fprintf (stream, "   ");
  }
}

void print_data_segment (Data_Segment *data_segment, int memInx,
                         int len,FILE *stream)
{
  DsWord *word;
  int i = 0;
  for (; memInx < len; ++memInx, ++i) {
    word = (DsWord *) get (data_segment, i);
    fprintf (stream, "%04d ", memInx);
    print_special_base_word (word->val, (len - memInx == 1), stream);
  }
}

void free_data_segment (Data_Segment *data_segment)
{
  free_vector (data_segment);
}


