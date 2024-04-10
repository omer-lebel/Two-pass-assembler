

#ifndef _SECOND_PASS_H_
#define _SECOND_PASS_H_

#include "setting.h"
#include "utils/errors.h"
#include "utils/text.h"
#include "utils/vector.h"
#include "fileStructures/symbolTable.h"
#include "fileStructures/codeSeg.h"
#include "fileStructures/dataSeg.h"
#include "fileStructures/entryLines.h"
#include "fileStructures/fileStructures.h"
#include "fileStructures/externUsages.h"

exit_code secondPass (file_analyze *f);



#endif /* _SECOND_PASS_H_ */
