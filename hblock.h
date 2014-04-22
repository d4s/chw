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


/**
 * @brief States of block processing
 *
 * Description of processing steps.
 * For future parallel processing.
 */
enum hblock_state {
	EMPTY,       /**< No any data available */
	PROCESSING,  /** Block is in use. Do not touch it!  */
	RAW_READY,   /**< Raw data available */
	ZDATA_READY, /**< Compressed data and Huffman's tree available */
	READY, 		 /**< All data available and Huffman's tree is ready */
	ERROR        /**< Error. If any.  */
};

typedef enum hblock_state hblock_state_t;


/**
 * @brief Structure for data management
 *
 * Contain all needed information for independent processing:
 * raw and compressed data, associated Huffman tree
 */
struct hblock {
	hblock_state_t state;
	uint8_t   * raw; /**< Raw (uncompressed data) */
	uint32_t  raw_size; /**< Raw data size in bytes */
	uint8_t   * zdata; /**< Compressed data with Huffman's algorithm */
	uint32_t  zdata_size; /**< Compressed data size in bits */
	hnode_t *head; /**< Pointer to head of Huffman tree */
	hnode_t **dictionary; /**< Need for speedup serialization (direct pointers to nodes in tree) */
};

typedef struct hblock hblock_t;

typedef struct _Hpb hpb_t;

/**
 * @brief Create hblock structure from raw data block
 *
 * @param buffer Pointer to buffer
 * @param size Size of data in buffer
 * @param state Shows data type in buffer
 *
 * @return Pointer to hblock structure associated with data or NULL for error
 */
hblock_t *hblock_create( uint8_t *buffer, uint32_t size, hblock_state_t state );

/**
 * @brief Destroy node and associated resources
 *
 * @param block Pointer to block to be destroyed
 */
void hblock_destroy( hblock_t *block);

/**
 * @brief Get state of block
 *
 * @param block Pointer to block
 *
 * @return State of block.
 */
hblock_state_t hblock_get_state( hblock_t *block);


/**
 * @brief Set state for block
 *
 * @param block Pointer to processing block
 * @param state Desirable state
 *
 * @return State of block. ERROR if can not set state for block.
 */
hblock_state_t hblock_set_state( hblock_t *block, hblock_state_t state);

/**
 * @brief Compress data block
 *
 * @param buffer Pointer to block with raw data
 * @param size Size of data in buffer
 *
 * @return zero on success 
 */
int hblock_compress( hblock_t *block);

/**
 * @brief Decompress data block
 *
 * @param buffer Pointer to block with compressed data and huffman tree
 *
 * @return zero on success 
 */
int hblock_decompress( hblock_t *block);

/**
 * @brief Read raw data
 *
 * Function tries to fill buffer from stream
 * return count of bytes in buffer
 *
 * @param fd File descriptor to read
 * @param buffer Buffer for stream data
 * @param buffer_size Size of buffer
 *
 * @return Bytes count in buffer
 */
uint32_t rawreader ( int fd, uint8_t *buffer, size_t buffer_size);


/**
 * @brief Read one message from stream
 *
 * Trying to read 1 message from stream in format:
 * uint32_t length of following message in network order
 * hpb_t    data block in protocol buffer format
 *
 * @param fd File descriptor to read
 * @param buffer Buffer for stream data
 * @param buffer_size Size of buffer
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



