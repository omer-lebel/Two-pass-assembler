/*
 * Created by Omer Lebel
 *
 * This program offers a  specialized implementation of a linked list
 * designed for word nodes, each capable of holding additional generic data.
 *  It efficiently handles standard list operations like initialization,
 *  deletion, tail insertion, sorted insertion and printing.
 */

#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* todo const?? */

/* Function pointer types for linked list operations */

/**
 * pointer to function that init the data field when creating a new node.
 * gets generic pointer to data to be initialized in the new node
 * and returns a newly allocated data.
 * @returns - generic pointer if succeed
 *          - NULL if memory allocation happened
 */
typedef void *(*init_data_func) (const void *);

/**
 * pointer to function that receives a word and its data from a generic type
 * and prints it to stdout.
 */
typedef void (*print_func) (const char*, const void *, FILE *pf);

/**
 * a pointer to a function that gets a pointer of generic data type and
 * frees it.
 */
typedef void (*free_data_func) (void *);

/** Node structure representing an element in the linked list */
typedef struct Node
{
    char *word;
    void *data;
    struct Node *next;
} Node;

/** LinkedList structure representing the linked list itself */
typedef struct LinkedList
{
    Node *head;
    Node *tail;
    int size;

    init_data_func init_data_func;
    print_func print_func;
    free_data_func free_data;
} LinkedList;

/**
 * Creates a new linked list.
 *

 * @param print Function to print the node.
 * @param freeData Function to free the data when freeing the list.
 * @return A pointer to the created linked list or NULL on failure.
 */
LinkedList *createList (init_data_func init_data, print_func print,
                        free_data_func freeData);

/**
 * Creates a new node with the provided data.
 *
 * @param linked_list Pointer to the linked list.
 * @param data Data to be stored in the new node.
 * @return A pointer to the created node or NULL on failure.
 */
Node *createNode (LinkedList *linked_list, const char* word, void *data);

/**
 * Retrieves the node containing the specified data.
 *
 * @param linked_list Pointer to the linked list.
 * @param word Data to search for in the linked list.
 * @return A pointer to the node containing the data or NULL if not found.
 */
Node *findNode (const LinkedList *linked_list, const char *word);

/**
 * Adds a new node to the end of the linked list.
 *
 * @param linked_list Pointer to the linked list.
 * @param new_node Pointer to the new node to be added.
 */
void appendToTail (LinkedList *linked_list, Node *new_node);

/**
 * Adds a new node to the linked list in sorted order. \n
 * This function assumes that the linked list is already sorted
 * in ascending order according to the specified comparison function.
 *
 * @param linked_list Pointer to the linked list.
 * @param new_node Pointer to the new node to be added.
 */
void appendSorted (LinkedList *linked_list, Node *new_node);

/**
 * Prints the linked list elements to the specified stream.
 *
 * @param linked_list Pointer to the linked list.
 * @param stream File stream where the output will be printed.
 */
void printList (LinkedList *linked_list, FILE *fp);

/**
 * Frees the memory allocated for the linked list and its nodes.
 *
 * @param linked_list Pointer to the linked list.
 */
void freeList (LinkedList *linked_list);

#endif /* _LINKED_LIST_H_ */

