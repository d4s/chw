/*
 * =====================================================================================
 *       Filename:  huffman.c
 *    Description:  
 *         Author:  Denis Pynkin (d4s), denis.pynkin@t-linux.by
 *        Company:  t-linux.by
 * ===================================================================================== */

#include <htree.h>

#include <time.h>
#include <string.h>


#ifdef OPENMP
#include <omp.h>
#endif

#define DICTSIZE 256 /**< count of elements in dictionary */
#define DICTWORD uint8_t /**< size of one element */
#define BUFFERSIZE 1024

int main( int argc, char **argv) {

	hnode_t dictionary[DICTSIZE];
	uint8_t buffer[BUFFERSIZE];

	/* for return codes */
	int rc;
	void *rcpnt;

	int totalsymbols=0;

	hnode_t *tree_head=NULL;

	rcpnt = memset (dictionary, 0, DICTSIZE * sizeof (hnode_t));
	assert (rcpnt != NULL);

	for (int i=0; i<DICTSIZE; i++)
		dictionary[i].code = i;


	/** Read and count symbols */
	while (1) {
	
		ssize_t readed;

		readed = read (STDIN_FILENO, buffer, BUFFERSIZE);
		if (readed <= 0)
			break;

#pragma omp  
		/** count readed symbols */
		for (int cnt=0; (cnt < BUFFERSIZE) && (cnt < readed); cnt++) {
			dictionary[buffer[cnt]].freq += 1;
		}

	}

	// Create Huffman tree from collected info
	tree_head = htree_create(dictionary, DICTSIZE);
	assert(tree_head != NULL);
	htree_destroy(tree_head);


	/** Print statistics */
	for (int i = 0; i<DICTSIZE; i++)
		if (dictionary[i].freq) {
			printf("Symbol %.3d = %lu\n", dictionary[i].code, dictionary[i].freq);
			totalsymbols++;
		}

	printf("Used symbols = %lu\n", totalsymbols);

	return 0;
}
