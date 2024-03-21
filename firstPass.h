/*
 Created by OMER on 1/15/2024.
*/
#include "setting.h"
#include "utils/text.h"
#include "fileStructures/fileStructures.h"
#include "utils/vector.h"
#include "fileStructures/symbolTable.h"
#include "fileStructures/memoryImg.h"
#include "fileStructures/analyzer.h"
#include "fsm.h"

#ifndef _FIRST_PASS_H_
#define _FIRST_PASS_H_


#define MAX_INT 8191
#define MIN_INT (-8191)


exit_code firstPass (FILE *input_file, file_analyze* f);


#endif /*_FIRST_PASS_H_ */
