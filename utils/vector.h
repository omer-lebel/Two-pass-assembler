/*
 Created by OMER on 1/30/2024.
*/

#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <stdlib.h>
#include <stdio.h>
/**
 * a pointer to a function that gets a pointer of generic data type and
 * frees it.
 */

typedef void *(*init_elem_func) (void *);
typedef void (*print_elem_func) (const void *, FILE *stream);
typedef void (*free_elem_func) (void *);

typedef struct vector{
    void* elements;
    size_t elem_size;
    size_t size;
    size_t capacity;

    init_elem_func init_elem;
    print_elem_func print_elem;
    free_elem_func free_elem;

}vector;
vector* create_n_vector(int n, size_t elem_size, init_elem_func init_elem,
                        print_elem_func print_elem , free_elem_func free_elem);

vector* create_vector(size_t elem_size, init_elem_func init_elem,
                      print_elem_func print_elem , free_elem_func free_elem);

void* get(vector * v, int index);

void* get_tail(vector * v);

void* push(vector * v, void* element);

void print_vector (vector *v, FILE *stream);

void free_vector(vector* v);

#endif /* _VECTOR_H_ */
