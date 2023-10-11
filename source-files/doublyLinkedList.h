/**
 * @file doublyLinkedList.h
 * @brief This is a header file for the doublyLinkedList module.
 *
 * This source file contains the definition of the functions
 * for the doublyLinkedList module, which provides the utilities
 * for storing and traversing song information.
 *
 * @author Amirhossein Etaati
 * @date 2023-03-17
 */

#if !defined(DOUBLY_LINKED_LIST_H)
#define DOUBLY_LINKED_LIST_H

#define bool _Bool

// Returns a pointer to the newly allocated empty list
// Note: caller should call doublyLinkedList_cleanup() to free the memory
void doublyLinkedList_init(void);

// Returns true if the list is empty
bool doublyLinkedList_isEmpty(void);

// struct Node* doublyLinkedList_getHead(void);

// Adds the item "src" with size of "size" to the head of the list
void doublyLinkedList_prependItem(void *src, unsigned int size);

// Adds the item "src" with size of "size" to the tail of the list
void doublyLinkedList_appendItem(void *src, unsigned int size);

// Updates the "current" field of the list to the next node
// Returns true if update is successful, false if the list is empty
bool doublyLinkedList_next(void);

// Updates the "current" field of the list to the previous node
// Returns true if update is successful, false if the list is empty
bool doublyLinkedList_prev(void);

// Returns the element at index "idx" or NULL if idx is out of bounds
// Note: the list is zero-indexed
void *doublyLinkedList_getElementAtIndex(int idx);

// Returns the data of the "current"
void *doublyLinkedList_getCurrentElement(void);

// Frees the memory for all nodes, the data, and the List struct
void doublyLinkedList_cleanup(void);

// Resets the current pointer to point to the element
// Returns false if idx is out of bounds or the list is empty
bool doublyLinkedList_setCurrent(int idx);

// Returns the index of the current element
// Note: returns -1 if the list is empty
int doublyLinkedList_getCurrentIdx(void);

// Returns the number of elements currently in the list
int doublyLinkedList_getSize(void);

//////////// Iterator functions to maintain the modularity according to songManager's needs ////////////

// Sets list's position to be displayed from to the Head
// Note: Returns false if the list is empty
bool doublyLinkedList_setIteratorStartPosition(void);

// Sets list's position to be displayed from to the Tail
bool doublyLinkedList_setIteratorEndPosition(void);

// Moves the display one position to the right
// Note: Returns false if the list is empty or there is no next element
bool doublyLinkedList_iteratorNext(void);

// Moves the display one position to the left and returns the data at the that position
// Note: Returns false if the list is empty or there is no previous element
bool doublyLinkedList_iteratorPrev(void);

// Resets the current pointer to point to the element
// Returns false if idx is out of bounds or the list is empty
// TODO: Remove??
bool doublyLinkedList_setIterator(int idx);

// Returns the data of the "currentDisplay"
void *doublyLinkedList_getCurrentIteratorElement(void);

// Moves the iterator "n" positions forward or to the last element if there aren't enough elements
// Returns false if the iterator cannot be moved
bool doublyLinkedList_advanceIteratorNTimes(int n);

// Moves the iterator "n" positions backwards or to the last element if there aren't enough elements
// Returns false if the iterator cannot be moved
bool doublyLinkedList_rewindIteratorNTimes(int n);

bool doublyLinkedList_delete(int idx);

#endif // DOUBLY_LINKED_LIST_H
