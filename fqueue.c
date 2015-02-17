/**
 * @file   fqueue.c
 * @Author Denis Pynkin (d4s), denis.pynkin@t-linux.by
 * @brief  FIFO queue for blocks 
 * @copyright Copyright (c) 2014, Denis Pynkin
 * @license This project is released under the GNU Public License.
 *
 */
#include <fqueue.h>


#ifdef DEBUG
void fqueue_print( fqhdr_t *fqhdr) {
	hblock_t *block = NULL;
	fqnode_t *node;

	int cnt=0;

	assert( fqhdr != NULL);


#pragma omp critical (queue)
	{
	node = fqhdr->head;
	while ( node != NULL) {
		block = node->block;

		switch ( hblock_get_state( block)) {

			case EMPTY:
				/** No any data available */
				DBGPRINT("Node %d: state EMPTY\n", cnt);
				break;
			case PROCESSING:
				/** Block is in use. Do not touch it!  */
				DBGPRINT("Node %d: state PROCESSING\n", cnt);
				break;
			case RAW_READY:
				/**< Raw data available */
				DBGPRINT("Node %d: state RAW_READY\n", cnt);
				break;
			case ZDATA_READY:
				/**< Compressed data and Huffman's tree available */
				DBGPRINT("Node %d: state ZDATA_READY\n", cnt);
				break;
			case READY: 
				/**< All data available and Huffman's tree is ready */
				DBGPRINT("Node %d: state READY\n", cnt);
				break;
			case ERROR: 
				/**< Error. If any.  */
				DBGPRINT("Node %d: state ERROR\n", cnt);
				break;
			default:
				DBGPRINT("Node %d: state UNDEFINED\n", cnt);
		}
		node = node->next;
		cnt++;
	}
	}
	
}
#else
#define fqueue_print(...) {}
#endif

/**
 * @brief Push node to tail of queue
 *
 * @param fqhdr Structure with queue description
 * @param block Huffman block to be attached to queue
 *
 * @return Zero on success, any other in case of error
 */
uint32_t fqueue_push_node( fqhdr_t *fqhdr, hblock_t *block) {

	FUNC_ENTER();

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

#pragma omp critical (queue)
	{
#pragma omp atomic read
	node->prev = fqhdr->tail;
#pragma omp atomic write
	fqhdr->tail = node;
#pragma omp atomic update
	fqhdr->fqlen += 1;

	if (fqhdr->head == NULL) {
		fqhdr->head = node; }
	else {
		node->prev->next=node;
	}
	}

	DBGPRINT("New node added. fifo length = %d\n", fqhdr->fqlen);

	fqueue_print( fqhdr);

	FUNC_LEAVE();

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

	FUNC_ENTER();

	hblock_t *block = NULL;
	fqnode_t *node;

	fqueue_print( fqhdr);

	assert( fqhdr != NULL);

	if (fqhdr->head == NULL)
		return NULL;

#pragma omp critical (queue)
	{
#pragma omp atomic read
	node = fqhdr->head;
#pragma omp atomic read
	block = node->block;

	if ( hblock_get_state( block) == READY){

#pragma omp atomic write
		fqhdr->head = node->next;
#pragma omp atomic update
		fqhdr->fqlen -= 1;

		free( node);

		node = fqhdr->head;

		if ( node == NULL) {
			fqhdr->tail = NULL;
		} else {
			node->prev = NULL;
		}
	DBGPRINT("Node deleted. fifo length = %d\n", fqhdr->fqlen);
	}
	else {
		block = NULL;
	}
	}
	FUNC_LEAVE();

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

	FUNC_ENTER();

	assert( fqhdr != NULL);
	hblock_t *block = NULL;

	fqueue_print( fqhdr);

#pragma omp critical (queue)
	{
	fqnode_t *node;
#pragma omp atomic read
	node = fqhdr->head;
	
	while (node != NULL) {
#pragma omp atomic read
		block = node->block;

	 	if (hblock_get_state( block) == hblock_state) {
			DBGPRINT("Node with state %d found. fifo length = %d\n", hblock_state, fqhdr->fqlen);
			break;
		}

#pragma omp atomic read
		node = node->next;
		block = NULL;
	}
	}

	if (block == NULL){
		DBGPRINT("Node with state %d not found. fifo length = %d\n", hblock_state, fqhdr->fqlen);
	}

	FUNC_LEAVE();

	return block;
}


