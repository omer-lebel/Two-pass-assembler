/**
 * @file dataSeg.h
 * @brief Defines functions for managing the data segment.
 *
 * Provides functions to create a new data segment, add data to it,
 * display its content, and print it with memory indices in the machine
 * language.
 */

#ifndef _DATA_SEG_H_
#define _DATA_SEG_H_

#include "../setting.h"

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

/**
 * @typedef DataSegment
 * @brief Represents the data segment in the assembler.
 *
 */
typedef struct DataSegment DataSegment;

/**
 * @brief Creates a new data segment.
 *
 * @return      Pointer to the newly created data segment,
 *              or null if memory allocation fails.
 */
DataSegment* new_data_segment();

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
Bool add_to_data_segment (DataSegment *data_segment, int *DC,
                          DsType type, void *arr, int size);

/**
 * @brief Displays the content of the data segment.
 *
 * @param data_segment  Pointer to the data segment.
 * @param stream        File stream to print to.
 */
void display_data_segment (DataSegment *data_segment, FILE *stream);

/**
 * @brief Prints the data segment in the machine language with memory indices.
 *
 * @param data_segment  Pointer to the data segment.
 * @param memInx        Starting memory index.
 * @param len           Length of the memory image.
 * @param stream        File stream to print to (typicality .ob file)
 */
void print_data_segment (DataSegment *data_segment, int memInx,
                         int len, FILE *stream);

/**
 * @brief Frees the memory allocated for the data segment.
 *
 * @param data_segment Pointer to the data segment.
 */
void free_data_segment (DataSegment *data_segment);



#endif /* _DATA_SEG_H_ */
