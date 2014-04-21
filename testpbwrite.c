/**
 * @file   testpbwrite.c
 * @Author Denis Pynkin (d4s), denis.pynkin@t-linux.by
 * @brief  
 * @copyright Copyright (c) 2014, t-linux.by
 * @license This project is released under the GNU Public License.
 *
 */

#include <huffman.h>

typedef struct _Hpb hpb_t;

#define TABLESIZE 10

/**
 * @brief 
 */
int main() {
	
	hpb_t msg = HPB__INIT;

	uint8_t *buffer;
	ProtobufCBinaryData *payload;

	buffer = malloc( BUFFERSIZE);
	assert( buffer != NULL);

	msg.symbols_table = malloc( TABLESIZE * sizeof(uint32_t ));
	assert( msg.symbols_table != NULL);
	msg.lengths_table = malloc( TABLESIZE * sizeof(uint32_t ));
	assert( msg.lengths_table != NULL);
	msg.codes_table = malloc( TABLESIZE * sizeof(uint32_t ));
	assert( msg.codes_table != NULL);

	/* Create simple structure */


	int msg_len = hpb__get_packed_size (&msg);
//		uint8_t *msgbuf = malloc (msglen);
//		hpb__pack (&msg, msgbuf);

	DBGPRINT("Data packed to %d bytes\n", msg_len);

	/** fill table of submessages */
	for (int i=0; i<TABLESIZE; i++) {
		msg.symbols_table[i] = i; 
		msg.codes_table[i] = i;
		msg.lengths_table[i] = i;
	}

	/* Create simple structure */
	msg.bits_len = 1024;
	msg.n_symbols_table = TABLESIZE;
	msg.n_lengths_table = TABLESIZE;
	msg.n_codes_table = TABLESIZE;

	/** Checkaccess  */
	for (int i=0; i<TABLESIZE; i++) {
		DBGPRINT("Symbol = %u, code = %u, codelen = %u\n", 
			msg.symbols_table[i], 
			msg.codes_table[i],
			msg.lengths_table[i]
		);
	}

	/** Prepare payload structure */
/* 	payload = malloc( sizeof(ProtobufCBinaryData));
	assert( payload != NULL);

	msg.payload = payload;
*/
	while (1) {

		/* Read data from stream */
		msg.payload.len = read (STDIN_FILENO, buffer, BUFFERSIZE);
		if (msg.payload.len <= 0)
			break;
		
 		msg.payload.data = malloc( msg.payload.len);
		assert( msg.payload.data != NULL);
		memcpy( msg.payload.data, buffer, msg.payload.len);

		DBGPRINT("Data count in block: %d\n", msg.payload.len);
/** pack start  */
		
		int msglen = hpb__get_packed_size (&msg);
		uint8_t *msgbuf = malloc (msglen);
		hpb__pack (&msg, msgbuf);

		DBGPRINT("Data packed to %d bytes\n", msglen);

		write ( STDOUT_FILENO, msgbuf, msglen);
		//hpb__free_unpacked( &msg, NULL);
/** pack end */
		free(msgbuf);
		free(msg.payload.data);
	
	}
	free(msg.symbols_table);
	free(msg.lengths_table);
	free(msg.codes_table);

	free (buffer);
	return 0;
}
