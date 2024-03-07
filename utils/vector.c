/*
 Created by OMER on 1/30/2024.
*/


#include "vector.h"
#include <string.h>

#define INITIAL_CAPACITY 100

vector* create_vector(size_t elem_size) {
  vector* new_vector = (vector*)malloc(sizeof(vector));
  if (!new_vector) {
    return NULL;
  }

  new_vector->elements = malloc(INITIAL_CAPACITY * elem_size);
  if (!new_vector->elements) {
    free(new_vector);
    return NULL;
  }

  new_vector->size = 0;
  new_vector->capacity = INITIAL_CAPACITY;
  new_vector->elem_size = elem_size;

  return new_vector;
}

vector* create_n_vector(size_t elem_size, size_t n) {
  vector* new_vector = (vector*)malloc(sizeof(vector));
  if (!new_vector) {
    return NULL;
  }

  new_vector->elements = malloc(n* elem_size);
  if (!new_vector->elements) {
    free(new_vector);
    return NULL;
  }

  new_vector->size = 0;
  new_vector->capacity = n;
  new_vector->elem_size = elem_size;

  return new_vector;
}

void* resize(vector * v) {
  void* tmp = realloc(v->elements, (2* v->capacity));
  if (!tmp) {
    return NULL;
  }
  v->elements = tmp;
  v->capacity *= 2;
  return v->elements;
}

void* get(vector * v, size_t index){
  if (index > v->size){
    return NULL; /* index out of range */
  }
  return v->elements + (index * v->elem_size);
}

void* get_tail(vector * v){
  if (v->size == 0){ //vector is empty
    return NULL;
  }
  return v->elements + ((v->size - 1) * v->elem_size);
}

void* push(vector * v, void* element) {
  void* p_new_elem;

  if (element == NULL){
    return NULL;
  }

  if (v->size == v->capacity) {
    if (!resize(v)){
      return NULL; /* Failed to resize */
    }
  }
  /* Copy the element into the list */
  p_new_elem = v->elements + (v->size*v->elem_size);
  memcpy(p_new_elem , element, v->elem_size);
  v->size++;
  return p_new_elem;
}


void free_vector(vector* v) {
  free(v->elements);
  free(v);
}
