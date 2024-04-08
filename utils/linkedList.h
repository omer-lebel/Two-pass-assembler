/**
 * @file linkedList.h
 * @brief Implementation of a generic linked list designed for word nodes,
 * each capable of holding additional generic data.
 *
 * Handles standard list operations like initialization, deletion, tail
 * insertion, sorted insertion and printing.
 */

#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* *********** Function pointer types for linked list operations */

/**
 * @typedef init_data_func
 * @brief pointer to function that init the data field when creating a new node.
 * gets generic pointer to data to be initialized in the new node
 * and returns a newly allocated data.
 *
 * @returns - generic pointer if succeed
 *          - NULL if memory allocation happened
 */
typedef void *(*init_data_func) (const void *);

/**
 * @typedef print_func
 * @brief pointer to function that receives a word and its data from a
 * generic type and prints it to stdout.
 *
 * @param word  The word associated with the data.
 * @param data  Pointer to the data to print.
 * @param pf    File stream to print to.
 */
typedef void (*print_func) (const char *, const void *, FILE *pf);

/**
 * @typedef free_data_func
 * @brief pointer to a function that gets a pointer of generic data type and
 * frees it.
 *
 * @param data Pointer to the data to free.
 */
typedef void (*free_data_func) (void *);

/**
 * @struct Node
 * @brief Struct representing a node in the linked list. */
typedef struct Node
{
    char *word;           /**< The word associated with the node. */
    void *data;           /**< Pointer to the additional data associated with the node. */
    struct Node *next;    /**< Pointer to the next node in the list. */
} Node;

/**
 * @struct LinkedList
 * @brief LinkedList structure representing the linked list itself
 * */
typedef struct LinkedList
{
    Node *head;                       /**< Pointer to the first node in the list. */
    Node *tail;                       /**< Pointer to the last node in the list. */
    int size;                         /**< Number of nodes in the list. */

    init_data_func init_data_func;    /**< Function pointer to initialize node's data. */
    print_func print_func;            /**< Function pointer to print node's data. */
    free_data_func free_data;         /**< Function pointer to free node's data. */
} LinkedList;

/**
 * @brief Creates a new linked list.
 *
 * @param print     Function to print the node.
 * @param free_data  Function to free the data when freeing the list.
 * @return          A pointer to the created linked list or NULL on failure.
 */
LinkedList *create_list (init_data_func init_data, print_func print,
                         free_data_func free_data);

/**
 * @brief Creates a new node with the provided data.
 *
 * @param linked_list   Pointer to the linked list.
 * @param data          Data to be stored in the new node.
 * @return              A pointer to the created node or NULL on failure.
 */
Node *create_node (LinkedList *linked_list, const char *word, void *data);

/**
 * @brief Retrieves the node containing the specified data.
 *
 * @param linked_list   Pointer to the linked list.
 * @param word          Data to search for in the linked list.
 * @return              A pointer to the node containing the data or NULL if not found.
 */
Node *find_node (const LinkedList *linked_list, const char *word);

/**
 * @brief Adds a new node to the end of the linked list.
 *
 * @param linked_list   Pointer to the linked list.
 * @param new_node      Pointer to the new node to be added.
 */
void append_to_tail (LinkedList *linked_list, Node *new_node);

/**
 * @brief Adds a new node to the linked list in sorted order. \n
 * This function assumes that the linked list is already sorted
 * in ascending order according to the specified comparison function.
 *
 * @param linked_list   Pointer to the linked list.
 * @param new_node      Pointer to the new node to be added.
 */
void append_sorted (LinkedList *linked_list, Node *new_node);

/**
 * @brief Prints the linked list elements to the specified stream.
 *
 * @param linked_list   Pointer to the linked list.
 * @param stream        File stream where the output will be printed.
 */
void print_list (LinkedList *linked_list, FILE *fp);

/**
 * @brief Frees the memory allocated for the linked list and its nodes.
 *
 * @param linked_list Pointer to the linked list.
 */
void free_list (LinkedList *linked_list);

#endif /* _LINKED_LIST_H_ */

