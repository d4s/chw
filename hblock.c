/**
 * @file   hblock.c
 * @Author Denis Pynkin (d4s), denis.pynkin@t-linux.by
 * @brief  Blocks for huffman algorithm
 * @copyright Copyright (c) 2014, t-linux.by
 * @license This project is released under the GNU Public License.
 *
 */

#include <hpb.h>

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

	/* TODO: check params */

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
		DBGPRINT("Bytes read %d, buffer space left %d\n", rd, buffer_size-offset);

		if ( (msglen - offset) <= 0 )
			break;
	}

	DBGPRINT("Message read %d\n", offset);
	msg = hpb__unpack (NULL, offset, buffer);

	if (msg == NULL) {
		DBGPRINT("Error message detected\n", offset);
		return NULL;
	}
		
	/* Message parsed, hooray! */


	DBGPRINT("Successful read of message %d\n", msgcnt);
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

