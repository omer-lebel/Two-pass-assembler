/* ------------------------------- includes ------------------------------- */
#include "dataSeg.h"
#include "../utils/vector.h"
#include "../utils/machineWord.h"
/* ---------------------- helper function declaration ---------------------- */
void print_data_word (const void *elem, FILE *stream);
/* ------------------------------------------------------------------------- */

/**
 * @struct DataSegment
 * @brief Internal structure representing the data segment, using a vector
 */
struct DataSegment
{
    vector *words;
};

DataSegment *new_data_segment ()
{
  DataSegment *data_segment = (DataSegment *) malloc (sizeof (DataSegment));
  if (data_segment) {
    /* Initialize the vector */
    data_segment->words = create_vector (sizeof (DsWord), NULL, NULL);
    if (!data_segment->words) {
      free (data_segment);
      return NULL;
    }
  }
  return data_segment;
}

Bool add_to_data_segment (DataSegment *data_segment, int *DC,
                          DsType type, void *arr, int size)
{
  DsWord word;
  int target_dc = *DC + size;
  Bool success = TRUE;
  char *charArr = (char *)arr;
  int *intArr = (int *)arr;

  for (; *DC < target_dc && success; (*DC)++) {
    word.type = type;
    word.val = (type == INT_TYPE) ? *intArr++ : (int)*charArr++;

    if (!push(data_segment->words, &word)) {
      success = FALSE;
    }

  }
  return success;
}

void display_data_segment (DataSegment *data_segment, FILE *stream)
{
  fprintf (stream, "\n----------------- data segment -----------------\n");
  print_vector (data_segment->words, print_data_word, stream, "|", "|\n");
}

void print_data_word (const void *elem, FILE *stream)
{
  char c;
  DsWord *ds_word = (DsWord *) elem;
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

void print_data_segment (DataSegment *data_segment, int memInx,
                         int len, FILE *stream)
{
  DsWord *word;
  int i = 0;
  for (; memInx < len; ++memInx, ++i) {
    word = (DsWord *) get (data_segment->words, i);
    fprintf (stream, "%04d ", memInx);
    print_special_base_word (word->val, (len - memInx == 1), stream);
  }
}

void free_data_segment (DataSegment *data_segment)
{
  free_vector (data_segment->words);
  free (data_segment);
}


