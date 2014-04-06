/* =====================================================================================
 *       Filename:  htree.h
 *    Description:  Huffman tree
 *         Author:  Denis Pynkin (d4s), denis.pynkin@t-linux.by
 *        Company:  t-linux.by
 * ===================================================================================== */

#ifndef HTREE_H
#define HTREE_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include <assert.h>


/**
 * Huffman tree node description  */
struct hnode {
	uint32_t freq;   /**< frequency */;
	uint8_t  code;   /**< code of symbol */
	struct hnode *up;;
	struct hnode *left;
	struct hnode *right;
};

typedef struct hnode hnode_t;

/** 
 * @brief Create huffman tree 
 *
 * Create huffman tree from array with collected statistics.
 * htree_destroy() should be called to free associated resources. 
 *
 * @param[in] table -- pointer to collected statistics
 * @param[in] table_size -- size of table
 *
 * @returns pointer to tree header
*/
hnode_t *htree_create(hnode_t *table, uint32_t table_size);

/**
 * @brief Destroy huffman tree
 *
 * Destroy huffman tree starting with head.
 * Release all resources associated with this tree.
 *
 * @param head Pointer to huffman tree head
 *
 * @returns Zero on success, non-zero if problem occured. 
 * 
*/
int htree_destroy(hnode_t *head);

#endif /* HTREE_H */

