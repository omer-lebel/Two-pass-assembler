/*
 Created by OMER on 1/30/2024.
*/


#include "vector.h"
#include <string.h>

#define INITIAL_CAPACITY 16

vector *create_n_vector (int n, size_t elem_size, init_elem_func init_elem,
                         free_elem_func free_elem)
{
  vector *new_vector = (vector *) malloc (sizeof (vector));
  if (!new_vector) {
    return NULL;
  }

  new_vector->elements = malloc (n * elem_size);
  if (!new_vector->elements) {
    free (new_vector);
    return NULL;
  }

  new_vector->size = 0;
  new_vector->capacity = n;
  new_vector->elem_size = elem_size;

  new_vector->init_elem = init_elem;
  new_vector->free_elem = free_elem;

  return new_vector;
}


vector *create_vector (size_t elem_size, init_elem_func init_elem,
                       free_elem_func free_elem)
{
  return create_n_vector(INITIAL_CAPACITY, elem_size, init_elem, free_elem);
}


void *resize (vector *v)
{
  void *tmp = realloc (v->elements, (2 * v->capacity * v->elem_size));
  if (!tmp) {
    return NULL;
  }
  v->elements = tmp;
  v->capacity *= 2;
  return v->elements;
}

void *get (vector *v, int index)
{
  if (index < 0 && index >= v->size) {
    return NULL; /* index out of range */
  }
  return ((char *) v->elements) + (index * v->elem_size);
}

void *get_tail (vector *v)
{
  if (v->size == 0) { /* vector is empty */
    return NULL;
  }
  return ((char *) v->elements) + ((v->size - 1) * v->elem_size);
}

void *push (vector *v, void *elem)
{
  void *p_new_elem;

  if (elem == NULL) {
    return NULL;
  }

  if (v->size == v->capacity) {
    if (!resize (v)) {
      return NULL; /* Failed to resize */
    }
  }
  /* Copy the element into the list */
  p_new_elem = ((char *) v->elements) + (v->size * v->elem_size);
  memcpy (p_new_elem, elem, v->elem_size);

  if (v->init_elem != NULL) {
    if (!v->init_elem (p_new_elem)) {
      return NULL;
    }
  }
  ++v->size;
  return p_new_elem;
}

void free_vector (vector *v)
{
  int i;
  if (v->free_elem) {
    for (i = 0; i < v->size; ++i) {
      v->free_elem (get (v, i));
    }
  }
  free (v->elements);
  free (v);
}

void print_vector (vector *v, print_elem_func print_elem,FILE *stream,
                   char* separator, char *ending)
{
  int i;
  if ( print_elem) {
    for (i = 0; i < v->size; ++i) {
      print_elem (get (v, i), stream);
      if (i < v->size - 1){
        fprintf (stream, "%s", separator);
      }
      else{
        fprintf (stream, "%s", ending);
      }
    }
  }
}