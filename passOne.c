//
// Created by OMER on 1/10/2024.
//

#include "passOne.h"
#include <stdlib.h>
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

int passOne(FILE *input){

  return EXIT_SUCCESS;
}
