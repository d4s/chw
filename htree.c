/*
 * =====================================================================================
 *       Filename:  htree.c
 *    Description:  
 *         Author:  Denis Pynkin (d4s), denis.pynkin@t-linux.by
 *        Company:  t-linux.by
 * ===================================================================================== */

#include <htree.h>
#include <pqueue.h>

/**
 * @brief Create node structure
 *
 * Should call hnode_destroy() or free memory by hands.
 *
 * @param frequency The frequency of code  
 * @param code Symbol code
 *
 * @returns Pointer to created structure on success, NULL on error. 
 * 
*/
hnode_t *hnode_create( uint32_t frequency, uint8_t code) {
	hnode_t *node;
	
	node = (hnode_t *) malloc( sizeof(hnode_t));

	if (node == NULL)
	   return NULL;	

	node->freq = frequency;
	node->code = code;
	node->up = NULL;
	node->left = NULL;
	node->right = NULL;

	return node;
}

/**
 * @brief Destroy node structure
 *
 * @param Pointer to structure
 *
 * @returns Zero on success, non-zero if problem occured. 
*/
int hnode_destroy( hnode_t * node) {

	if (node == NULL)
	   return 0;	

	free(node);

	return 0;	
}

#if 0
/* TODO: version without priority queues */
hnode_t *htree_add_node( hnode_t *head, hnode_t *node) {

	if (node == NULL)
		return head;

	printf("     Adding %d = %d\n", node->code, node->freq);

	node->up = NULL;
	node->left = NULL;
	node->right = NULL;
	node->type = LEAF;


	if (head == NULL) {
		head = hnode_create( 0, 0);
		assert( head != NULL);
		head->type = NODE;
		head->freq = node->freq;
		head->left = node;

		node->up = head;

		return head;
	}

	while (1) {

		if (head->type == NODE){
			/* NODE type */
			printf("         node %d = %d\n", head->code, head->freq);
			head->freq += node->freq;

			if (head->left == NULL) {
				printf("			adding to left  node %d = %d\n", head->code, head->freq);
				head->left = node;
				node->up = head;
				break;
			}
	
			if (head->right == NULL) {
				printf("			adding to right node %d = %d\n", head->code, head->freq);
				head->right = node;
				node->up = head;
				break;
			}

/*  			if ( head->->freq > head->freq) {
				head = head->left;
				printf("			going left to node %d = %d\n", head->code, head->freq);
			} else {
*/
				head = head->right;
				printf("			going right to node %d = %d\n", head->code, head->freq);
//			}

		} else {
			printf("         current %d = %d\n", head->code, head->freq);
			static hnode_t *new_node; 	
			/* Insert new_node empty node */
			new_node = hnode_create( 0, 0);
			assert( new_node != NULL);

			new_node->up = head->up;
			new_node->type = NODE;
			new_node->freq = head->freq;
			new_node->left = head;

			if ( head->up != NULL) {
				if (head->up->left == head) {
					head->up->left = new_node;
				} else {
					head->up->right = new_node;
				}
			}

			head->up = new_node;

			printf("         leaf up       %d = %d\n", head->up->code, head->up->freq);
//			printf("         leaf up left  %d = %d\n", head->up->left->code, head->up->left->freq);
//			printf("         leaf up right %d = %d\n", head->up->right->code, head->up->right->freq);


			head = new_node;
		}
	} // while NODE	

	printf("	HEAD now:  %d = %d\n", head->code, head->freq);
	while (head->up != NULL)
		head = head->up;

	return head;
}
#endif

/**
 * @brief add node to head
 *
 * Version for working with prioritized queues
 * expected to work with only 1-level!!!
 *
 * @param head High-level node, NULL means a new high-level node will be created
 * @param node Low-level node to be added to High-level
 *
 * @returns High-level node 
*/
hnode_t *htree_add_node( hnode_t *head, hnode_t *node) {

	if (node == NULL)
		return head;

	printf("     Adding %d = %d\n", node->code, node->freq);

	node->up = NULL;
//	node->left = NULL;
//	node->right = NULL;


	if (head == NULL) {
		head = hnode_create( 0, 0);
		assert( head != NULL);
	}


	if (head->right == NULL) {
		printf("			adding to right node %d = %d\n", head->code, head->freq);
		head->right = node;
	} else if (head->left == NULL) {
		printf("			adding to left  node %d = %d\n", head->code, head->freq);
		head->left = node;
	} else {
		/* Something goes wrong. WTF??? */
		assert(0);
	}

	head->freq += node->freq;

	node->up = head;
	return head;
}


static int hnode_cmp( const void *first, const void *second) {

	static hnode_t ** s1, **s2;

	s1 = (hnode_t **)first;
	s2 = (hnode_t **)second;

	if ( *s1 == *s2)
		return 0;
	if ( *s1 == NULL)
		return 1;
	if ( *s2 == NULL)
		return -1;

	return  ((*s2)->freq - (*s1)->freq);
}

int htree_buffer_sort( hnode_t **table, uint32_t table_size) {

	qsort( table, (size_t) table_size, sizeof(hnode_t*), hnode_cmp);
	return 0;
}


/*  */
hnode_t *htree_create( hnode_t **table, uint32_t table_size) {

	hnode_t *head=NULL;
	pqnode_t *pqhead=NULL;

	assert( table != NULL);
	assert( table_size != 0);

#if 0
	/* TODO: used sorting before adding just for fun */
	/* drop empty entries from initial set */
	for (int i=0; i<table_size; i++) {
		if ( table[i] == NULL) {
			do {
				table_size--;
				if (table[table_size] != NULL)
					break;
			} while (table_size>i);
			table[i]=table[table_size];
			table[table_size] = NULL;
		}
	}


	htree_buffer_sort( table, table_size);
#endif

	/* Create prioritized queue */
	for (int i=0; i<table_size; i++) {
	
		if (table[i] == NULL)
			continue;

		pqnode_t *pqnode = pqueue_create_node(table[i]);
		assert(pqnode != NULL);

		pqhead = pqueue_add_node( pqhead, pqnode);
		assert(pqhead != NULL);
	
	}

	// pqueue_print(pqhead);


	while (1) {
	pqueue_print(pqhead);
		hnode_t *hnode_new;
		pqnode_t *pqnode_new;;
		
		/* Pop the 1-st */
		hnode_new = htree_add_node( NULL, pqhead->hnode);
		assert( hnode_new != NULL);

		head = hnode_new;

		pqhead = pqueue_del_node( pqhead, 0);
		if (pqhead == NULL)
			break;


		/* Pop the 2-nd */
		hnode_new = htree_add_node( hnode_new, pqhead->hnode);
		assert( hnode_new != NULL);

		pqhead = pqueue_del_node( pqhead, 0);
		if (pqhead == NULL)
			break;

		pqnode_new = pqueue_create_node( hnode_new);
		assert(pqnode_new != NULL);
		
		pqhead = pqueue_add_node( pqhead, pqnode_new);
	}

	printf("###  Huffman tree:\n");
	htree_print( head, 0);
	

	return head;
}

int htree_destroy(hnode_t *head) {


	return 0;
}

void htree_print(const hnode_t *node, int level) {

	if (node == NULL)
		return;

	if ((node->left == NULL) && (node->right == NULL)) {
		printf("(%d) Leaf with code=%.3d and frequency=%u\n", level, node->code, node->freq);
	} else {
		printf("(%d) Node with weight=%u\n", level, node->freq);
	}

	level++;

	if (node->left != NULL)
		htree_print(node->left, level);

	if (node->right != NULL)
		htree_print(node->right, level);

}

