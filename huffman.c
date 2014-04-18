/*
 * =====================================================================================
 *       Filename:  huffman.c
 *    Description:  
 *         Author:  Denis Pynkin (d4s), denis.pynkin@t-linux.by
 *        Company:  t-linux.by
 * ===================================================================================== */

#include <huffman.h>
#include <htree.h>

#include <time.h>


#define DICTSIZE 256 /**< count of elements in dictionary */
#define BUFFERSIZE 1024*1024

int main( int argc, char **argv) {

	hnode_t **dictionary; // array of pointers to hnode_t elements
	uint8_t buffer[BUFFERSIZE];


	int totalsymbols=0;

	hnode_t *tree_head=NULL;

	/* Create memory region for dictionary */
	dictionary = malloc( DICTSIZE * sizeof( hnode_t *));
	assert( dictionary != NULL);


	/** Read and count symbols */
	while (1) {
		/* for return codes */
		void *rcpnt;
		rcpnt = memset (dictionary, 0, DICTSIZE * sizeof (hnode_t *));
		assert (rcpnt != NULL);

		ssize_t readed;

		/* Read data from stream */
		readed = read (STDIN_FILENO, buffer, BUFFERSIZE);
		if (readed <= 0)
			break;

		/** count readed symbols */
		for (int cnt=0; (cnt < BUFFERSIZE) && (cnt < readed); cnt++) {
			uint8_t code = buffer[cnt];
			if( dictionary[code] == NULL)
				dictionary[code] = hnode_create( 0, code);
			dictionary[code]->freq += 1;
		}


		/* Create Huffman tree from collected info */
		tree_head = htree_create(dictionary, DICTSIZE);
		assert(tree_head != NULL);
	
		/* Add codes to symbols and count compressed size */
		uint32_t coded_size =  htree_add_codes( tree_head, 0, 0);

		DBGPRINT("buffer with %db (%dB) symbols compressed to %db (%dB):\n", 
				tree_head->freq * 8, tree_head->freq, coded_size, coded_size%8?(1 + coded_size/8):(coded_size/8));
		//htree_print( head, 0);
		
		htree_destroy(tree_head);

		DBGPRINT("Used symbols = %u\n", totalsymbols);

	}

	free( dictionary);
	return 0;
}

