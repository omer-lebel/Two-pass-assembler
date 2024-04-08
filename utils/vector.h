/**
 * @file vector.h
 * @brief Implementation of a vector (dynamic array) data structure capable
 * of storing generic elements.
 *
 * Supports operations such as initialization, appending, getting element
 * from a specific index, printing and freeing.
 */


#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <stdlib.h>
#include <stdio.h>

/* ******** Function pointer types for vector operations ***** */

/**
 * @typedef init_elem_func
 * @brief Typedef for a function pointer used to initialize elements.
 *
 * @param elem Pointer to the element to initialize.
 * @return Pointer to the initialized element.
 */
typedef void *(*init_elem_func) (void *);

/**
 * @typedef free_elem_func
 * @brief Typedef for a function pointer used to free elements.
 *
 * @param elem Pointer to the element to free.
 */
typedef void (*free_elem_func) (void *);


/**
 * @typedef print_elem_func
 * @brief Typedef for a function pointer used to print elements.
 *
 * @param elem   Pointer to the constant element to print.
 * @param stream File stream to print the element to.
 */
typedef void (*print_elem_func) (const void *, FILE *stream);

/**
 * @struct vector
 * @brief Struct representing a vector data structure ( dynamic array)
 */
typedef struct vector{
    void* elements;               /**< Pointer to the array storing elements. */
    size_t elem_size;             /**< Size of each element in bytes. */
    int size;                     /**< Number of elements currently stored in the vector. */
    int capacity;                 /**< Capacity of the vector. */

    init_elem_func init_elem;    /**< Function pointer to initialize elements. */
    free_elem_func free_elem;    /**< Function pointer to free elements. */
} vector;

/**
* @brief Creates a new vector with a specified initial capacity.
*
* @param n           The initial capacity of the vector.
* @param elemSize    Size of each element in bytes.
* @param initElem    Function pointer to initialize elements (can be null).
* @param freeElem    Function pointer to free elements (can be null).
* @return            A pointer to the newly created vector, or null if memory allocation fails.
*/
vector* create_n_vector(int n, size_t elem_size, init_elem_func init_elem,
                        free_elem_func free_elem);

/**
* @brief Creates a new vector with an initial capacity of INITIAL_CAPACITY.
*
* @param elemSize    Size of each element in bytes.
* @param initElem    Function pointer to initialize elements (can be null).
* @param freeElem    Function pointer to free elements (can be null).
* @return            A pointer to the newly created vector, or null if memory allocation fails.
*/
vector* create_vector(size_t elem_size, init_elem_func init_elem,
                      free_elem_func free_elem);

/**
 * @brief Retrieves an element at the specified index in the vector.
 *
 * @param v       Pointer to the vector.
 * @param index   Index of the element to retrieve.
 * @return        A pointer to the element at the specified index, or null if index is out of range.
 */
void* get(vector * v, int index);

/**
 * @brief Retrieves the last element in the vector.
 *
 * @param v   Pointer to the vector.
 * @return    A pointer to the last element, or null if the vector is empty.
*/
void* get_tail(vector * v);

/**
 * @brief Adds a new element to the end of the vector.
 *
 * @param v     Pointer to the vector.
 * @param elem  Pointer to the element to be added (add copy of the element)
 * @return      A pointer to the added element, or null if memory allocation fails.
*/
void* push(vector * v, void* element);

/**
 * @brief Prints the elements of the vector.
 *
 * @param v           Pointer to the vector.
 * @param printElem   Function pointer to print each element.
 * @param stream      File stream to print to.
 * @param separator   Separator string between elements.
 * @param ending      Ending string after the last element.
 */
void print_vector (vector *v, print_elem_func print_elem, FILE *stream,
              char* separator, char *ending);

/**
 * @brief Frees the memory allocated for the vector and its elements.
 *
 * @param v   Pointer to the vector to be freed.
*/
void free_vector(vector* v);

#endif /* _VECTOR_H_ */
