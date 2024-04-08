/**
 * @file dataSeg.h
 * @brief Defines functions for managing the data segment.
 */

#ifndef _DATA_SEG_H_
#define _DATA_SEG_H_

#include "../setting.h"
#include "../utils/vector.h"
#include "../utils/machineWord.h"

/** Enum representing the types of data in the data segment. */
typedef enum DsType
{
    CHAR_TYPE,
    INT_TYPE
} DsType;

/**
 * @Struct DsWord
 * @brief representing a word in the data segment.
 */
typedef struct DsWord
{
    DsType type;
    int val: 14;
} DsWord;

typedef vector Data_Segment;

/**
 * @brief Creates a new data segment.
 *
 * @param DC    Pointer to the data counter.
 * @return      Pointer to the newly created data segment,
 *              or null if memory allocation fails.
 */
Data_Segment* new_data_segment(int *DC);

/**
 * @brief Adds data to the data segment.
 *
 * @param data_segment  Pointer to the data segment.
 * @param DC            Pointer to the data counter.
 * @param type          Type of the data to add (CHAR_TYPE or INT_TYPE)
 * @param arr           Pointer to the array containing the data.
 * @param size          Size of the data to add.
 * @return              TRUE if the data is successfully added, FALSE otherwise.
 */
Bool add_to_data_segment (Data_Segment *data_segment, int *DC,
                          DsType type, void *arr, int size);

/**
 * @brief Displays the content of the data segment.
 *
 * @param data_segment  Pointer to the data segment.
 * @param stream        File stream to print to.
 */
void display_data_segment (Data_Segment *data_segment, FILE *stream);

/**
 * @brief Prints the data segment in the machine language with memory indices.
 *
 * @param data_segment  Pointer to the data segment.
 * @param memInx        Starting memory index.
 * @param len           Length of the memory image.
 * @param stream        File stream to print to (typicality .ob file)
 */
void print_data_segment (Data_Segment *data_segment, int memInx,
                         int len,FILE *stream);

/**
 * @brief Frees the memory allocated for the data segment.
 *
 * @param data_segment Pointer to the data segment.
 */
void free_data_segment (Data_Segment *data_segment);



#endif /* _DATA_SEG_H_ */
