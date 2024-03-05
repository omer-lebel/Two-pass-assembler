/*
 Created by OMER on 1/30/2024.
*/

#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <stdlib.h>

typedef struct vector{
    void* elements;
    size_t elem_size;
    size_t size;
    size_t capacity;
}vector;

vector* create_vector(size_t elem_size);

vector* create_n_vector(size_t elem_size, size_t n);

void* get(vector * v, size_t index);

void* push(vector * v, void* element);

void free_vector(vector* v);

#endif /* _VECTOR_H_ */
