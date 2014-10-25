/**
 * @file   fqueue.c
 * @Author Denis Pynkin (d4s), denis.pynkin@t-linux.by
 * @brief  FIFO queue for blocks 
 * @copyright Copyright (c) 2014, Denis Pynkin
 * @license This project is released under the GNU Public License.
 *
 */
#include <fqueue.h>


/**
 * @brief Push node to tail of queue
 *
 * @param fqhdr Structure with queue description
 * @param block Huffman block to be attached to queue
 *
 * @return Zero on success, any other in case of error
 */
uint32_t fqueue_push_node( fqhdr_t *fqhdr, hblock_t *block) {

	assert( fqhdr != NULL);
	assert ( block != NULL);

	fqnode_t *node = malloc( sizeof(fqnode_t));
	assert( node != NULL);

	memset( node, 0, sizeof(fqnode_t));

	node->block = block;
	
	while (fqhdr->fqlen >= FIFO_QUEUE_MAX_LEN) {
		#pragma omp taskyield 
		;
	}

	#pragma omp atomic read
	node->prev = fqhdr->tail;
	#pragma omp atomic write
	fqhdr->tail = node;
	#pragma omp atomic update
	fqhdr->fqlen += 1;

//	#pragma omp atomic capture
	{ if (fqhdr->head == NULL) { fqhdr->head = node; } }

	DBGPRINT("New node added. fifo length = %d\n", fqhdr->fqlen);
	return 0;
}


/**
 * @brief Pop the head of queue 
 *
 * @param fqhdr Structure with queue description
 *
 * @return Pointer to popped huffman block or NULL in case if no blocks
 */
hblock_t *fqueue_pop_node( fqhdr_t *fqhdr) {

	hblock_t *block;
	fqnode_t *node;

	assert( fqhdr != NULL);

//#pragma omp atomic read
	if (fqhdr->head == NULL)
		return NULL;

#pragma omp atomic read
	node = fqhdr->head;
	block = node->block;

	if ( hblock_get_state( block) != READY)
	   return NULL;	

#pragma omp atomic write
	fqhdr->head = node->next;
#pragma omp atomic update
	fqhdr->fqlen -= 1;

	free( node);

#pragma omp atomic read
	node = fqhdr->head;

	if ( node == NULL) {
//#pragma omp atomic update
		fqhdr->tail = NULL;
	} else {
		node->prev = NULL;
	}

	DBGPRINT("Node deleted. fifo length = %d\n", fqhdr->fqlen);
	return block;
}

/**
 * @brief Find first block in queue with selected status 
 *
 * @param fqhdr Structure with queue description
 * @param hblock_state Status of block we searching in queue
 *
 * @return Pointer to block, NULL otherwise
 */
//#pragma omp critical
hblock_t *fqueue_get_next_node( fqhdr_t *fqhdr, hblock_state_t hblock_state) {

	assert( fqhdr != NULL);

	fqnode_t *node = fqhdr->head;
	
	while (node != NULL) {
		hblock_t *block = node->block;

	 	if (hblock_get_state( block) == hblock_state) {
			DBGPRINT("Node with state %d found. fifo length = %d\n", hblock_state, fqhdr->fqlen);
			return block;
		}

		node = node->next;
	}

	DBGPRINT("Node with state %d not found. fifo length = %d\n", hblock_state, fqhdr->fqlen);
	return NULL;
}


