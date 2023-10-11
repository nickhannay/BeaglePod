/**
 * @file doublyLinkedList.c
 * @brief This is a source file for the doublyLinkedList module.
 *
 * This source file contains the declaration of the functions
 * for the doublyLinkedList module, which provides the utilities
 * for storing and traversing song information.
 *
 * @author Amirhossein Etaati
 * @date 2023-03-17
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "doublyLinkedList.h"

struct Node
{
    void *data;
    struct Node *next;
    struct Node *prev;
};

struct List
{
    struct Node *head;
    struct Node *tail;

    struct Node *current; // keeps track of the current element in the list
    int curentIdx;        // -1 if there is no element in the list

    struct Node *currentIterator;

    int size;
};

static bool is_module_initialized = false;
static struct List *list_ptr = NULL;

// Private functions definitions
static void push_to_head(void *src, unsigned int size);
static void push_to_tail(void *src, unsigned int size);
static void pop_from_head(void);
static bool set_ptr_to_idx(int idx, struct Node *ptr);

//------------------------------------------------
//////////////// Public Functions ////////////////
//------------------------------------------------

void doublyLinkedList_init(void)
{
    list_ptr = (struct List *)malloc(sizeof(struct List));
    list_ptr->head = NULL;
    list_ptr->tail = NULL;
    list_ptr->current = NULL;
    list_ptr->curentIdx = -1;
    list_ptr->currentIterator = NULL;
    list_ptr->size = 0;

    is_module_initialized = true;
}

bool doublyLinkedList_isEmpty(void)
{
    assert(is_module_initialized);
    return (!list_ptr->head || !list_ptr->tail);
}

void doublyLinkedList_appendItem(void *src, unsigned int size)
{
    assert(is_module_initialized);
    push_to_tail(src, size);
}

void doublyLinkedList_prependItem(void *src, unsigned int size)
{
    assert(is_module_initialized);
    push_to_head(src, size);
}

bool doublyLinkedList_next(void)
{
    assert(is_module_initialized);
    if (list_ptr->current == NULL || list_ptr->current->next == NULL)
    {
        return false;
    }
    list_ptr->current = list_ptr->current->next;
    list_ptr->curentIdx += 1;
    return true;
}

bool doublyLinkedList_prev(void)
{
    assert(is_module_initialized);
    if (list_ptr->current == NULL || list_ptr->current->prev == NULL)
    {
        return false;
    }
    list_ptr->current = list_ptr->current->prev;
    list_ptr->curentIdx -= 1;
    return true;
}

void *doublyLinkedList_getCurrentElement(void)
{
    assert(is_module_initialized);
    if (!doublyLinkedList_isEmpty() && list_ptr->current != NULL)
    {
        return list_ptr->current->data;
    }
    return NULL;
}

void doublyLinkedList_cleanup(void)
{
    assert(is_module_initialized);
    while (!doublyLinkedList_isEmpty())
    {
        pop_from_head();
    }
    free(list_ptr);
}

void *doublyLinkedList_getElementAtIndex(int idx)
{
    assert(is_module_initialized);
    if (idx < 0)
        return NULL;

    int counter = 0;
    struct Node *node = list_ptr->head;

    while (counter < idx && node != NULL)
    {
        node = node->next;
        counter++;
    }
    if (node == NULL)
        return NULL;
    return node->data;
}

// Sets the current pointer to point to the element at index "idx"
// Returns false if idx is out of bounds or the list is empty, true if successful
bool doublyLinkedList_setCurrent(int idx)
{
    assert(is_module_initialized);
    return set_ptr_to_idx(idx, list_ptr->current);
}

// Returns the index of the current element
// Note: returns -1 if the list is empty
int doublyLinkedList_getCurrentIdx(void)
{
    assert(is_module_initialized);
    if (list_ptr->head == NULL)
    {
        return -1;
    }
    return list_ptr->curentIdx;
}

// Returns the number of elements currently in the list
int doublyLinkedList_getSize(void)
{
    assert(is_module_initialized);
    return list_ptr->size;
}

// Sets list's position to be displayed from to the Head
// Note: Returns false if the list is empty
bool doublyLinkedList_setIteratorStartPosition(void)
{
    assert(is_module_initialized);
    if (list_ptr->head == NULL)
    {
        return false;
    }
    list_ptr->currentIterator = list_ptr->head;
    return true;
}

// Sets list's position to be displayed from to the Tail
bool doublyLinkedList_setIteratorEndPosition(void)
{
    assert(is_module_initialized);
    if (list_ptr->tail == NULL)
    {
        return false;
    }
    list_ptr->currentIterator = list_ptr->tail;
    return true;
}

// Moves the display one position to the right
// Note: Returns false if the list is empty or there is no next element
bool doublyLinkedList_iteratorNext(void)
{
    assert(is_module_initialized);
    if (list_ptr->size == 0)
    {
        return false;
    }
    if ((list_ptr->currentIterator)->next != NULL)
    {
        list_ptr->currentIterator = (list_ptr->currentIterator)->next;
        return true;
    }
    return false;
}

// Moves the display one position to the left and returns the data at the that position
// Note: Returns false if the list is empty or there is no previous element
bool doublyLinkedList_iteratorPrev(void)
{
    assert(is_module_initialized);
    if (list_ptr->size == 0)
    {
        return false;
    }
    if ((list_ptr->currentIterator)->prev != NULL)
    {
        list_ptr->currentIterator = (list_ptr->currentIterator)->prev;
        return true;
    }
    return false;
}

// Returns the data of the "currentIterator"
void *doublyLinkedList_getCurrentIteratorElement(void)
{
    assert(is_module_initialized);
    if (list_ptr->currentIterator == NULL)
    {
        return NULL;
    }
    return list_ptr->currentIterator->data;
}

bool doublyLinkedList_setIterator(int idx)
{
    assert(is_module_initialized);
    return doublyLinkedList_advanceIteratorNTimes(idx);
}

bool doublyLinkedList_advanceIteratorNTimes(int n)
{
    if (n <= 0 || list_ptr->tail == list_ptr->currentIterator)
    {
        return false;
    }
    int counter = 0;
    while (counter < n && doublyLinkedList_iteratorNext())
    {
        counter++;
    }
    return true;
}

bool doublyLinkedList_rewindIteratorNTimes(int n)
{
    if (n <= 0 || list_ptr->head == list_ptr->currentIterator)
    {
        return false;
    }
    int counter = 0;
    while (counter < n && doublyLinkedList_iteratorPrev())
    {
        counter++;
    }
    return true;
}

bool doublyLinkedList_delete(int idx)
{
    if (list_ptr->size == 0)
    {
        return false;
    }
    if (idx == 0)
    {
        pop_from_head();
        return true;
    }
    struct Node *temp = NULL;
    if (set_ptr_to_idx(idx, temp) == false)
    {
        return false;
    }
    if (temp == list_ptr->current)
    {
        list_ptr->current = list_ptr->current->next;
    }
    struct Node *delete_node = temp;
    struct Node *prev_node = temp->prev;
    struct Node *next_node = NULL;
    if (temp != list_ptr->tail)
    {
        next_node = temp->next;
    }
    prev_node->next = temp->next;
    if (next_node != NULL)
    {
        next_node->prev = prev_node;
    }

    if (list_ptr->tail == temp)
    {
        list_ptr->tail = prev_node;
    }
    free(delete_node->data);
    free(delete_node);
    list_ptr->size -= 1;
    return true;
}

//------------------------------------------------
/////////////// Private Functions ////////////////
//------------------------------------------------

static void push_to_head(void *src, unsigned int size)
{
    struct Node *new_node = malloc(sizeof(struct Node));
    if (new_node == NULL)
    {
        fprintf(stderr, "%s\n", "doublyLinkedList_push_to_head(): Error - There was a problem allocating memory.");
        exit(1);
    }
    (new_node->data) = malloc(size);
    if ((new_node->data) == NULL)
    {
        fprintf(stderr, "%s\n", "doublyLinkedList_push_to_head(): Error - There was a problem allocating memory for t");
        exit(1);
    }

    memcpy((new_node->data), (src), size);
    (new_node->next) = (list_ptr->head);
    (new_node->prev) = NULL;

    if (list_ptr->head != NULL)
    {
        list_ptr->head->prev = new_node;
        (list_ptr->head) = new_node;
    }
    else
    {
        // List was previously empty
        list_ptr->tail = list_ptr->head = list_ptr->current = list_ptr->currentIterator = new_node;
        list_ptr->curentIdx = 0;
    }

    list_ptr->size += 1;
}

static void push_to_tail(void *src, unsigned int size)
{
    struct Node *new_node = malloc(sizeof(struct Node));
    if (new_node == NULL)
    {
        fprintf(stderr, "%s\n", "doublyLinkedList_push_to_tail(): Error - There was a problem allocating memory.");
        exit(1);
    }
    (new_node->data) = malloc(size);
    if ((new_node->data) == NULL)
    {
        fprintf(stderr, "%s\n", "doublyLinkedList_push_to_tail(): Error - There was a problem allocating memory for t");
        exit(1);
    }

    memcpy((new_node->data), (src), size);
    (new_node->next) = NULL;
    (new_node->prev) = list_ptr->tail;

    if (list_ptr->tail != NULL)
    {
        (list_ptr->tail->next) = new_node;
        list_ptr->tail = new_node;
    }
    else
    {
        list_ptr->tail = list_ptr->head = list_ptr->current = list_ptr->currentIterator = new_node;
        list_ptr->curentIdx = 0;
    }

    list_ptr->size += 1;
}

static void pop_from_head(void)
{
    if ((list_ptr->head) == NULL)
    {
        fprintf(stderr, "%s\n", "doublyLinkedList_pop_from_head(): Error - The list is empty");
        exit(1);
    }

    if (list_ptr->current == list_ptr->head)
    {
        list_ptr->current = list_ptr->head->next; // if current is pointing to head, set it to the next element
    }
    if (list_ptr->currentIterator == list_ptr->head)
    {
        list_ptr->currentIterator = list_ptr->head->next; // if currentIterator is pointing to head, set it to the next element
    }

    struct Node *first_node = (list_ptr->head);
    (list_ptr->head) = list_ptr->head->next;
    if (list_ptr->head != NULL)
    {
        list_ptr->head->prev = NULL;
    }
    free(first_node->data);
    free(first_node);

    list_ptr->size -= 0;
}

static bool set_ptr_to_idx(int idx, struct Node *ptr)
{
    int counter = 0;
    struct Node *node = list_ptr->head;

    while (counter < idx && node != NULL)
    {
        node = node->next;
        counter++;
    }
    if (node == NULL)
        return false;

    if (ptr == list_ptr->current)
    {
        list_ptr->curentIdx = idx;
    }
    ptr = node;
    return true;
}

//------------------------------------------------
//////////////// FOR TESTING ONLY ////////////////
//------------------------------------------------

// int main(int argc, char const *argv[])
// {
//     doublyLinkedList_init();

//     doublyLinkedList_appendItem("Hello1", strlen("Hello1") + 1);
//     doublyLinkedList_appendItem("Hello2", strlen("Hello2") + 1);
//     doublyLinkedList_appendItem("Hello3", strlen("Hello3") + 1);

//     // printf("current size if %d\n", doublyLinkedList_getSize());

//     // char *result = doublyLinkedList_getElementAtIndex(3);
//     // printf("==> the result is: %s\n", result);

//     doublyLinkedList_advanceIteratorNTimes(3);
//     if (doublyLinkedList_rewindIteratorNTimes(300))
//     {
//         printf("==> current iterator element is: %s\n", doublyLinkedList_getCurrentIteratorElement());
//     }
//     else
//     {
//         printf("doublyLinkedList_advanceIteratorNTimes() failed\n");
//     }

//     do
//     {
//         if (!doublyLinkedList_isEmpty())
//         {
//             printf("======> %s\n", (char *)doublyLinkedList_getCurrentElement());
//         }
//     } while (doublyLinkedList_next());

//     do
//     {
//         if (!doublyLinkedList_isEmpty())
//         {
//             printf("======> %s\n", (char *)doublyLinkedList_getCurrentElement());
//         }
//     } while (doublyLinkedList_prev());

//     doublyLinkedList_cleanup();

//     return 0;
// }
