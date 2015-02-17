/*
 * @file   fqueue.h
 * @Author Denis Pynkin (d4s), denis.pynkin@t-linux.by
 * @date   2014
 * @brief  FIFO queue for blocks management
*/

#ifndef FQUEUE_H
#define FQUEUE_H

#include <huffman.h>
#include <hblock.h>

#ifdef _OPENMP
#include <omp.h> 
#endif
/**
 * @brief Double linked queue node
*/
typedef struct fqnode {
	struct fqnode *prev; /**< pointer to previous node */
	struct fqnode *next; /**< pointer to next node */
	hblock_t *block; /**< pointer to payload */
} fqnode_t;


enum fq_state {
	FQREADY,
	FQBUSY
};

typedef enum fq_state fq_state_t;

/* 
 * @brief FIFO queue description
*/
typedef struct fqhdr {
	fqnode_t *head; /**< pointer to the head of queue */
	fqnode_t *tail; /**< pointer to the tail of queue */
	uint32_t fqlen; /**< length of queue  */
	fq_state_t state; /**< detect if some thread manipulating with queue  */
} fqhdr_t;

/* Max blocks in queue */
#define FIFO_QUEUE_MAX_LEN 20


/**
 * @brief Push node to tail of queue
 *
 * @param fqhdr Structure with queue description
 * @param block Huffman block to be attached to queue
 *
 * @return Zero on success, any other in case of error
 */
uint32_t fqueue_push_node( fqhdr_t *fqhdr, hblock_t *block);


/**
 * @brief Pop the head of queue 
 *
 * @param fqhdr Structure with queue description
 *
 * @return Pointer to popped huffman block
 */
hblock_t *fqueue_pop_node( fqhdr_t *fqhdr);


/**
 * @brief Find first block in queue with selected status 
 *
 * @param fqhdr Structure with queue description
 * @param hblock_state Status of block we searching in queue
 *
 * @return Pointer to block, NULL otherwise
 */
hblock_t *fqueue_get_next_node( fqhdr_t *fqhdr, hblock_state_t hblock_state);

#endif

