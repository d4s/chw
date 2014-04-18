/* =====================================================================================
 *       Filename:  htree.h
 *    Description:  Huffman tree
 *         Author:  Denis Pynkin (d4s), denis.pynkin@t-linux.by
 *        Company:  t-linux.by
 * ===================================================================================== */

#ifndef HTREE_H
#define HTREE_H

#include <huffman.h>
#include <assert.h>


typedef enum {
	LEAF=0, 
	NODE
} hnode_type;

/**
 * Huffman tree node description  */
struct hnode {
	uint32_t freq;   /**< frequency */
	uint8_t  code;   /**< code of symbol */
	uint32_t bits;    /**< binary representation */
	uint32_t blen; /**< count of bits in  binary representation */
	hnode_type type;
	struct hnode *up;
	struct hnode *left;
	struct hnode *right;
};

typedef struct hnode hnode_t;

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
hnode_t *hnode_create( uint32_t frequency, uint8_t code);

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
hnode_t *htree_create(hnode_t **table, uint32_t table_size);

/**
 * @brief Destroy huffman tree
 *
 * Destroy huffman tree starting with head.
 * Release all resources associated with this tree.
 *
 * @param head Pointer to huffman tree head
 *
 * @returns Nothing
 * 
*/
void htree_destroy(hnode_t *head);

/**
 * @brief Calculate bits 
 *
 * Recursive walk through huffman tree and calculate 
 * codes for corresponding leaf 
 *
 * @param head Pointer to huffman tree head
 * @param level Current depth 
 * @param hcode Huffman code from previous level
 *
 * @returns Calculated size of compressed data
 * 
*/
uint32_t htree_add_codes(hnode_t *head, int level, uint32_t hcode);


/**
 * @brief Print huffman tree
 *
 * Recursive walk through huffman tree
 *
 * @param head Pointer to huffman tree head
 * @param level Current depth 
 *
 * @returns Nothing
 * 
*/
void htree_print(const hnode_t *head, int level);

#endif /* HTREE_H */

