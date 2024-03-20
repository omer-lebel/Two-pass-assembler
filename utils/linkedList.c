#include "linkedList.h"

LinkedList* createList(init_data_func init_data, print_func print,
                       free_data_func free_data)
{
  LinkedList *new_list = (LinkedList*)malloc (sizeof (LinkedList));
  if (!new_list)
  {
    return NULL;
  }
  new_list->head = NULL;
  new_list->tail = NULL;
  new_list->size = 0;

  new_list->init_data_func = init_data;
  new_list->print_func = print;
  new_list->free_data = free_data;

  return new_list;
}


Node *createNode (LinkedList *linked_list, const char *word, void *data)
{
  Node *node = (Node*)malloc (sizeof (Node));
  if (!node) {
    return NULL;
  }

  node->word = (char*) malloc (strlen (word)+1);
  if (!node->word){
    free (node);
    return NULL;
  }
  node->word = strcpy (node->word, word);

  node->data = linked_list->init_data_func(data);
  if (!node->data) {
    free(node->word);
    free (node);
    return NULL;
  }
  node->next = NULL;
  return node;
}

Node *findNode (const LinkedList *linked_list, const char *word)
{
  Node *node = linked_list->head;
  while (node) {
    if (strcmp(node->word, word) == 0) {
      return node;
    }
    node = node->next;
  }
  return NULL;
}

void appendToTail (LinkedList *linked_list, Node *new_node)
{
  if (!linked_list->head) {
    linked_list->head = new_node;
  }
  else {
    linked_list->tail->next = new_node;
  }
  linked_list->tail = new_node;
  linked_list->size++;
}

void appendSorted (LinkedList *linked_list, Node *new_node)
{
  Node *prev = NULL;
  Node *curr = linked_list->head;

  while (curr && strcmp(curr->word, new_node->word) < 0) {
    prev = curr;
    curr = curr->next;
  }

  if (!curr) { /* add as tail */
    linked_list->tail = new_node;
    new_node->next = NULL;
  }

  if (!prev) { /* add as head */
    new_node->next = linked_list->head;
    linked_list->head = new_node;
  }

  else { /* add in the middle */
    prev->next = new_node;
    new_node->next = curr;
  }
  linked_list->size++;
}

void printList(LinkedList *linked_list, FILE *fp)
{
  Node *node = linked_list->head;
  while (node)
  {
    linked_list->print_func(node->word, node->data, fp);
    node = node->next;
  }
}

void freeList(LinkedList *linked_list)
{
  Node *node = linked_list->head;
  Node *tmp = NULL;
  while (node)
  {
    tmp = node->next;
    linked_list->free_data(node->data);
    free(node->word);
    free(node);
    node = tmp;
  }
  free(linked_list);
}
