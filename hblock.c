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
 * @brief Read raw data
 *
 * Function tries to fill buffer from stream
 * return count of bytes in buffer
 *
 * @param fd File descriptor to read
 * @param buffer Buffer for stream data
 * @param buffer_size Size of buffer
 *
 * @return Bytes count in buffer, 0 in case EOF, -1 in case of error
 */
uint32_t rawreader ( int fd, uint8_t *buffer, size_t buffer_size) {
	
		ssize_t readed;

		uint32_t size = 0;

		assert( buffer != NULL);
		assert( buffer_size >= 0);

		while (1) {
			readed = read (STDIN_FILENO, buffer+size, buffer_size - size);
			if (readed <= 0)
				break;

			size += readed;
			if (size >= buffer_size) {
				if (size == buffer_size) {
					/* Buffer full */
					return size;
				}
				/* something going wrong here!!! */
				return -1;
			}
		}
		return size;
}


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
			return;

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

	/* Should not happen but who cares? */
	if (block->zdata != NULL)
		free( block->zdata);


	/* Create memory region for dictionary */
	dictionary = malloc( DICTSIZE * sizeof( hnode_t *));
	assert( dictionary != NULL);


	/* Cleanup */
	memset (dictionary, 0, DICTSIZE * sizeof (hnode_t *));

	memset (histogram, 0, DICTSIZE*sizeof(uint32_t));

	/* Get some statistics */
	for (int cnt=0; cnt < block->raw_size; cnt++) {
		histogram[block->raw[cnt]] += 1;
	}

	/* Create nodes */
	for (int cnt=0; cnt < DICTSIZE; cnt++) {

		uint32_t value = histogram[cnt];
		if (value == 0)
			continue;

		if (dictionary[cnt] == NULL) 
			dictionary[cnt] = hnode_create( 0, cnt);

		dictionary[cnt]->freq = value;
	}

	/* Create Huffman tree from collected info */
	block->head = htree_create(dictionary, DICTSIZE);
	assert( block->head != NULL);


//	htree_print( block->head, 0);

	/* Add codes to symbols and count compressed size in bits */
	block->zdata_size =  htree_add_codes( block->head, 0, 0);

	block->zdata = malloc( block->zdata_size%8 ? (1 + block->zdata_size/8) : (block->zdata_size/8));
	assert( block->zdata != NULL);

	DBGPRINT("buffer with %d b (%d B) symbols compressed to %d b (%d B):\n", 
			block->raw_size * 8, block->raw_size, 
			block->zdata_size, block->zdata_size%8?(1 + block->zdata_size/8):(block->zdata_size/8));
		//htree_print( head, 0);
	
	/* Comression */
	uint32_t bits = 0; /* bits collector */
	uint32_t maskedbits; /* align data here for byte writing */
	int shift = 0; 
	uint32_t zpos=0; /* position in compressed buffer */	
	for (int cnt=0; cnt < block->raw_size; cnt++) {
		uint8_t code = block->raw[cnt];
		uint32_t codebits = dictionary[code]->bits;
		uint32_t codelen = dictionary[code]->blen;
		
		DBGPRINT("%d symbol %d -> code %d (%d)\n", cnt, code, codebits, codelen);

		bits <<= codelen;
		bits |= codebits;

		shift += codelen;

		while (shift > 7) {
			maskedbits = bits;
			shift -= 8;

			maskedbits >>= shift;
			block->zdata[zpos] = (uint8_t) maskedbits;
			DBGPRINT("%d zpos: Added byte 0x%X\n", zpos, (uint8_t) maskedbits);
			zpos++;
		}

	}
	/* last bits should be added too */
	DBGPRINT("rest: shift = %d, zpos = %d, bits = 0x%X\n", 
			shift, zpos, bits);

	if( shift != 0) {
		bits <<= (8-shift);
		block->zdata[zpos] = (uint8_t) bits;
		DBGPRINT("%d zpos: Added byte 0x%X\n", zpos, (uint8_t) bits);
	}
	/* Do not need dictionary */
	free( dictionary);
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


