/*
 Created by OMER on 1/15/2024.
*/


#ifndef _FIRST_PASS_H_
#define _FIRST_PASS_H_

#include "utils/text.h"
#include "fileStructures/symbolTable.h"
#include "fileStructures/memoryImg.h"


#define MAX_INT 8191
#define MIN_INT (-8191)


int firstPass(FILE *input_file, char* file_name, exit_code *no_error);


#endif /*_FIRST_PASS_H_ */
