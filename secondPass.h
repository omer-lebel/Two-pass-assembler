/*
 Created by OMER on 3/5/2024.
*/


#ifndef _SECOND_PASS_H_
#define _SECOND_PASS_H_
#include "setting.h"
#include "utils/text.h"
#include "utils/vector.h"
#include "utils/linkedList.h"
#include "fileStructures/symbolTable.h"
#include "fileStructures/ast.h"
#include "fileStructures/memoryImg.h"
#include "fileStructures/entryTable.h"
#include "fileStructures/externTable.h"

exit_code secondPass (file_analyze *f);

#endif /* _SECOND_PASS_H_ */
