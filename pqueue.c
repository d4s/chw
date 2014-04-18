/*
 * @file   pqueue.c
 * @Author Denis Pynkin (d4s), denis.pynkin@t-linux.by
 * @date   2014
 * @brief  Priority queue
*/

#include <pqueue.h>

/** 
 * @brief Create node for priority queue
 *
 * @param[in] hnode -- huffman tree node to be added in queue
 *
 * @returns pointer to fresh node suitable for priority queue or NULL
*/
pqnode_t *pqueue_create_node( hnode_t *node) {
	if (node == NULL)
		return NULL;

	pqnode_t *pqnode = malloc(sizeof(hnode_t));
	if (pqnode == NULL)
		return NULL;

	pqnode->prev = NULL;
	pqnode->next = NULL;
	pqnode->hnode = node;

	return pqnode;
}

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
pqnode_t *pqueue_add_node( pqnode_t *head, pqnode_t *node) {
	
	pqnode_t *current;

	if (head == NULL) {
		DBGPRINT("New queue created\n");
		return node;
	}

	if (node == NULL)
		return head;

	assert( node->hnode != NULL);

//	DBGPRINT("adding node %d with frequency %d to queue\n", node->hnode->code, node->hnode->freq);
	current = head;

	while (current->hnode->freq < node->hnode->freq) {
		
		if ( current->hnode == NULL)
			return head; /* 3-rd party force from outer space was here??? */		
		
		if ( current->next == NULL)
			break;

		current = current->next;
	} 

	if (current->hnode->freq >= node->hnode->freq) {
		node->prev = current->prev;
		node->next = current;
		current->prev = node;
	} else if (current->next == NULL) {
	/* border case */
//		DBGPRINT("adding node to the tail\n");
		node->next = NULL;
		node->prev = current;
		current->next = node;
	} else {
		DBGPRINT("Shouldn't happen\n");
		return head;
	}


	if (node->prev == NULL) {
//		DBGPRINT("adding node to the head\n");
		head = node;
	}
	else {
//		DBGPRINT("previous node %d with freq %d\n", node->prev->hnode->code, node->prev->hnode->freq);
		node->prev->next = node;
	}

	return head;
}


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
pqnode_t *pqueue_del_node( pqnode_t *head, uint32_t id) {
	
	pqnode_t *current = head;

	for (uint32_t i=0; i<id; i++) {
		if (current == NULL)
			return head;

		current = current->next;
	}

	if (current->prev != NULL)
		current->prev->next = current->next;
	else
		head = current->next;

	if (current->next != NULL)
		current->next->prev = current->prev;

	free(current);

	return head;
}

/** 
 * @brief Destroy priority queue
 *
 * Delete all nodes from queue
 *
 * @param[in] head -- pointer to head of queue.
 *
 * @returns Nothing
*/
void pqueue_destroy( pqnode_t *head) {

	while (head != NULL) {
		pqnode_t *tmp = head;
		head = head->next;
		free(tmp);
	}	
}

/** 
 * @brief Print priority queue
 *
 * @param[in] head -- pointer to head of queue.
 *
 * @returns Nothing
*/
void pqueue_print( pqnode_t *head) {

	int i=0;

	if (head == NULL)
		return;

	do {
		if (head->hnode != NULL) {
			printf("(%d) node %.3d with frequency %u\n", i, head->hnode->code, head->hnode->freq);
		} else {
			printf("Node without payload!!!\n");
		}

		i++;
		head = head->next;
	} while (head != NULL);
}

