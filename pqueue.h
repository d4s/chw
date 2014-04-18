/*
 * @file   pqueue.h
 * @Author Denis Pynkin (d4s), denis.pynkin@t-linux.by
 * @date   2014
 * @brief  Priority queue
*/

#ifndef PQUEUE_H
#define PQUEUE_H

#include <huffman.h>
#include <htree.h>

/**
 * @brief Double linked queue node
*/
typedef struct pqnode {
	struct pqnode *prev; /**< pointer to previous node */
	struct pqnode *next; /**< pointer to next node */
	hnode_t *hnode; /**< pointer to payload */
} pqnode_t;


/** 
 * @brief Create node for priority queue
 *
 * @param[in] hnode -- huffman tree node to be added in queue
 *
 * @returns pointer to fresh node suitable for priority queue or NULL
*/
pqnode_t *pqueue_create_node( hnode_t *node);


/** 
 * @brief Add node to priority queue
 *
 * Add new node to existing queue or create a new one.
 *
 * @param[in] head -- pointer to head of queue. New queue will be created in case of NULL.
 * @param[in] node -- node to be added in queue
 *
 * @returns pointer to the head of priority queue
*/
pqnode_t *pqueue_add_node( pqnode_t *head, pqnode_t *node);

/** 
 * @brief Delete node from priority queue
 *
 * Remove node from selected queue.
 * If needed to cleanup data -- remove it before function call. 
 *
 * @param[in] head -- pointer to head of queue.
 * @param[in] id -- node ID to be removed from queue
 *
 * @returns pointer to the head of priority queue. NULL if queue is empty.
*/
pqnode_t *pqueue_del_node( pqnode_t *head, uint32_t id);

/** 
 * @brief Destroy priority queue
 *
 * Delete all nodes from queue
 *
 * @param[in] head -- pointer to head of queue.
 *
 * @returns Nothing
*/
void pqueue_destroy( pqnode_t *head);

/** 
 * @brief Print priority queue
 *
 * Print queue
 *
 * @param[in] head -- pointer to head of queue.
 *
 * @returns Nothing
*/
void pqueue_print( pqnode_t *head);


#endif

