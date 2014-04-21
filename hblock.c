/**
 * @file   hblock.c
 * @Author Denis Pynkin (d4s), denis.pynkin@t-linux.by
 * @brief  Blocks for huffman algorithm
 * @copyright Copyright (c) 2014, t-linux.by
 * @license This project is released under the GNU Public License.
 *
 */

#include <hblock.h>
#include <netinet/in.h>

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
hpb_t * streamreader ( int fd, uint8_t *buffer, size_t buffer_size) {

	hpb_t *msg;
	static uint32_t msglen;
	static size_t rd;
	static uint32_t offset;

	/* check params */
	assert( buffer != NULL);
	assert( buffer_size > sizeof(uint32_t));

	/* Read size of message */
	offset = 0;
	while (1) {

		rd = read( fd, buffer+offset, sizeof(uint32_t)-offset);
		if (rd <= 0) {
			/* problems detected */
			return NULL;
		}
		offset += rd;
		if (( sizeof(uint32_t) - offset) <= 0 )
			break;
	}

	msglen = ntohl(*(uint32_t *) buffer);

	if (msglen > buffer_size) {
		return NULL;
	}

	DBGPRINT("Trying to read message with size %u\n", msglen);

	offset = 0;
	while (1) {

		rd = read( fd, buffer+offset, msglen-offset);

		if (rd <= 0) {
			/* problems detected */
			return NULL;
		}

		offset += rd;
		DBGPRINT("Bytes read %d, buffer space left %d\n", (int)rd, (int)(buffer_size-offset));

		if ( (msglen - offset) <= 0 )
			break;
	}

	DBGPRINT("Message read %d\n", offset);
	msg = hpb__unpack (NULL, offset, buffer);

	if (msg == NULL) {
		DBGPRINT("Error message detected\n");
		return NULL;
	}
		
	/* Message parsed, hooray! */


	DBGPRINT("Successful read of message\n");
	return msg;
}

/**
 * @brief Serialize huffman block
 *
 * @param fd File descriptor
 * @param block Pointer to huffman block structure 
 *
 * @return Size of data written or 0 on error
 */
size_t streamwriter ( int fd, hblock_t *block) {

	return 0;
}


/**
 * @brief Create hblock structure from raw data block
 *
 * @param buffer Pointer to buffer
 * @param size Size of data in buffer
 * @param state Shows data type in buffer
 *
 * @return Pointer to hblock structure associated with data or NULL for error
 */
hblock_t *hblock_create( uint8_t *buffer, uint32_t size, hblock_state_t state ) {

	hblock_t *block;
	uint8_t *data;

	block = malloc( sizeof(hblock_t));
	if (block == NULL) {
		DBGPRINT("Out of memory\n");
		return NULL;
	}

	/* Block cleanup */
	memset( block, 0, sizeof(hblock_t)); 

	if (buffer == NULL) {
		/*  Create an empty block */
		hblock_set_state( block, EMPTY);
		return block;
	}


	/* Create memory region for data from buffer */
	data = malloc( size);
	assert( data != NULL);

	memcpy( data, buffer, size);

	switch( state) {
		case RAW_READY:
			block->raw = data;
			block->raw_size = size;
			hblock_set_state( block, RAW_READY);
			break;
		case ZDATA_READY:
			block->zdata = data;
			block->zdata_size = size;
			hblock_set_state( block, ZDATA_READY);
			break;
		default:
			free( data);
			free( block);
			return NULL;
	}

	return block;
}



/**
 * @brief Destroy node and associated resources
 *
 * @param block Pointer to block to be destroyed
 */
void hblock_destroy( hblock_t *block) {

		if (block == NULL)
			return NULL;

		hblock_set_state( block, PROCESSING);
		if (block->raw != NULL)
			free( block->raw);

		if (block->zdata != NULL)
			free( block->zdata);

		if (block->head != NULL)
			htree_destroy( block->head);

		free( block);
}

/**
 * @brief Compress data block
 *
 * @param buffer Pointer to block with raw data
 * @param size Size of data in buffer
 *
 * @return zero on success 
 */
int hblock_compress( hblock_t *block) {

	hnode_t **dictionary; // array of pointers to hnode_t elements

	/* Frequency collector */
	uint32_t histogram[DICTSIZE];

	assert( block != NULL);
	assert( block->raw != NULL);

	hblock_set_state( block, PROCESSING);

	block->raw = buffer;
	block->raw_size = size;

	/* Should not happen but who cares? */
	if (block->zdata != NULL)
		free( block->zdata);


	


	hblock_set_state( block, READY);
	return 0;

}

/**
 * @brief Decompress data block
 *
 * @param buffer Pointer to block with compressed data and huffman tree
 *
 * @return zero on success 
 */
int hblock_decompress( hblock_t *block) {

	return 0;
}




/**
 * @brief Get state of block
 *
 * @param block Pointer to processing block
 *
 * @return State of block.
 */
hblock_state_t hblock_get_state( hblock_t *block) {

	assert( block != NULL);

	/* TODO: add openmp  */
	return block->state;
}


/**
 * @brief Set state for block
 *
 * @param block Pointer to processing block
 * @param state Desirable state
 *
 * @return State of block. ERROR if can not set state for block.
 */
hblock_state_t hblock_set_state( hblock_t *block, hblock_state_t state) {

	assert( block != NULL);

	/* TODO: add openmp  */
	block->state = state;

	return block->state;
}


