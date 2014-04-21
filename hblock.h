/**
 * @file   hblock.h
 * @Author Denis Pynkin (d4s), denis.pynkin@t-linux.by
 * @brief  Blocks for huffman algorithm
 * @copyright Copyright (c) 2014, t-linux.by
 * @license This project is released under the GNU Public License.
 *
 */

#include <huffman.h>
#include <htree.h>


enum hblock_state {
	EMPTY,
	PROCESSING,
	READY
};

struct hblock {
	hblock_state state;
	uint8_t   * raw; /**< Raw (uncompressed data) */
	uint32_t  raw_size; /**< Raw data size */
	uint8_t   * zdata; /**< Compressed with Huffman's algorithm data */
	uint32_t  zdata_size; /**< Compressed data size */
	htree_t *head; /**< Pointer to head of Huffman tree */ 
};

typedef struct hblock hblock_t;

typedef struct _Hpb hpb_t;


/**
 * @brief Read one message from stream
 *
 * Trying to read 1 message from stream in format:
 * uint32_t length of following message in network order
 * hpb_t    data block in protocol buffer format
 *
 * @param fd File descriptor to read
 * @param buffer Buffer for stream data
 * @param offset Size of buffer
 * @param out Pointer to unpacked message
 *
 * @return Pointer to unpacked message or NULL
 */
hpb_t * streamreader ( int fd, uint8_t *buffer, size_t buffer_size);


/**
 * @brief Serialize huffman block
 *
 * @param fd File descriptor
 * @param block Pointer to huffman block structure 
 *
 * @return Size of data written or 0 on error
 */
size_t streamwriter ( int fd, hblock_t *block);

