/*
 Created by OMER on 1/24/2024.
*/


#include "memoryImg.h"

#define INIT_SIZE 100

char *text_seg;
DsWord *data_seg;

size_t DC = 0, IC = 100;

/****************** data segment *******************/

exit_code init_data_seg(void){
  DC = 0;
  IC = 100;
  data_seg = (DsWord *) malloc (INIT_SIZE * sizeof (DsWord));
  if (!data_seg) {
    return FAILURE; /* memory error */
  }
  return SUCCESS;
}

exit_code addToDataSeg (LineInfo *line, DsType type, void *arr, size_t size)
{
  size_t target_dc = DC + size;
  int *intArr;
  char *charArr;
  if (target_dc - 1 >= 100) {
    r_error ("adding the variables: ", line, " causes data segment overflow");
    return FAILURE;
  }

  switch (type) {
    case INT_TYPE:
      intArr = (int *) arr;
      for (; DC < target_dc; DC++) {
        data_seg[DC].type = type;
        data_seg[DC].val = *intArr++;
      }
      break;
    case CHAR_TYPE:
      charArr = (char *) arr;
      for (; DC < target_dc; DC++) {
        data_seg[DC].type = type;
        data_seg[DC].val = (int) *charArr++;
      }
      break;
    default:
      r_error ("unknown data type in ", line, "");
      return ERROR;
  }
  return SUCCESS;
}

void printDs (void)
{
  size_t i;
  char c;
  for (i = 0; i < DC; ++i) {
    if (data_seg[i].type == INT_TYPE) {
      printf ("| %d ", data_seg[i].val);
    }
    else if ((c = (char) data_seg[i].val) != '\0') { /* char */
      printf ("| %c ", c);
    }
    else { /* '\0' */
      printf ("|   ");
    }
  }
  if (i != 0) {
    printf ("|\n");
  }
}
