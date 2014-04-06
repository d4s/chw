/*
 * =====================================================================================
 *       Filename:  htree.c
 *    Description:  
 *         Author:  Denis Pynkin (d4s), denis.pynkin@t-linux.by
 *        Company:  t-linux.by
 * ===================================================================================== */

#include <htree.h>

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


hnode_t *htree_add_node( hnode_t *head, hnode_t *node) {

	static hnode_t *current;
	static int rc;

	if (node == NULL)
		return head;

	node->up = NULL;
	node->left = NULL;
	node->right = NULL;

	if (head == NULL)
		return node;

	current = head;
#if 0
	while (1) {
		 if ((current->left == NULL) && (current->right == NULL)) {

		 }
		 rc = current->code - node->code;

		 if ((rc == 0) && (current->left == NULL) && (current->right == NULL) /* Do not add  */
			 break;

		 if (rc > 0) {
			 if (current->
		 }

	}
#endif

	return head;
}


static int hnode_cmp( const hnode_t *first, const hnode_t *second) {
	return  (first->freq - second->freq);
}

int htree_buffer_sort( hnode_t *table, uint32_t table_size) {

	qsort( table, (size_t) table_size, sizeof(hnode_t), hnode_cmp);
}


/*  */
hnode_t *htree_create( hnode_t *table, uint32_t table_size) {

	hnode_t *head=NULL;

	assert( table != NULL);


	/* TODO: drop empty from initial set */
	htree_buffer_sort( table, table_size);

	for (int i=0; i<table_size; i++) {
	
		if (table[i].freq == 0)
			continue;

		head = htree_add_node( head, &table[i]);
		assert(head != NULL);

	}

	return head;
}

int htree_destroy(hnode_t *head) {


	return 0;
}

