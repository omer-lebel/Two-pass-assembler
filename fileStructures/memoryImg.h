/*
 Created by OMER on 1/24/2024.
*/


#ifndef _MEMORY_IMG_H_
#define _MEMORY_IMG_H_

#include "../setting.h"
#include "../utils/text.h"


/****************** data segment *******************/

typedef enum DsType {
    CHAR_TYPE,
    INT_TYPE
} DsType;

typedef struct DsWord{
    DsType type;
    int val;
} DsWord;

exit_code init_data_seg(void);

exit_code addToDataSeg (LineInfo *line, DsType type, void *arr, size_t size);

void printDs (void);

/****************** text segment *******************/




/****************** extern *******************/
extern char *text_seg;
extern DsWord *data_seg;
extern size_t DC, IC;

#endif /*_MEMORY_IMG_H_ */
