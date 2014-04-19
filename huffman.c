/*
 * @file   huffman.c
 * @Author Denis Pynkin (d4s), denis.pynkin@t-linux.by
 * @date   2014
 * @brief  Huffman archiver
*/


#include <huffman.h>
#include <htree.h>

#include <time.h>


#define DICTSIZE 256 /**< count of elements in dictionary */
#define BUFFERSIZE 64*1024

int main( int argc, char **argv) {

	hnode_t **dictionary; // array of pointers to hnode_t elements
	uint8_t *buffer;

	int totalsymbols=0;

	int np=1;
	int myid=0;

	hnode_t *tree_head=NULL;

	/* Create memory region for dictionary */
	dictionary = malloc( DICTSIZE * sizeof( hnode_t *));
	assert( dictionary != NULL);

	buffer = malloc( BUFFERSIZE);
	assert( buffer != NULL);

#ifdef _OPENMP
#pragma omp parallel
	np = omp_get_num_threads();
#endif

	uint32_t histogram[np][DICTSIZE];

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
//		DBGPRINT("Readed block of %d size\n", readed);
#ifdef DEBUG
		/* probably impossible ? */
		assert (readed <= BUFFERSIZE);
#endif


		memset (histogram, 0, np*DICTSIZE*sizeof(uint32_t));

		/** count readed symbols */
#ifdef _OPENMP
#pragma omp parallel private(myid)
		{
			myid = omp_get_thread_num();

#pragma omp for
#endif
			for (int cnt=0; cnt < readed; cnt++) {
				histogram[myid][buffer[cnt]] += 1;
			}

#ifdef _OPENMP
#pragma omp for 
#endif
			for (int cnt=0; cnt < DICTSIZE; cnt++) {
#ifdef _OPENMP
				uint32_t value = 0;
				for (int i=0; i<np; i++) {
					value = value + histogram[i][cnt];
				}
#else
				uint32_t value = histogram[myid][cnt];
#endif
				if (value == 0)
					continue;

				if (dictionary[cnt] == NULL) 
					dictionary[cnt] = hnode_create( 0, cnt);

				dictionary[cnt]->freq = value;
			}
#ifdef _OPENMP
		} /* parallel section */
#endif

		/* Create Huffman tree from collected info */
		tree_head = htree_create(dictionary, DICTSIZE);
		assert(tree_head != NULL);

//		htree_print( tree_head, 0);
		/* Add codes to symbols and count compressed size */
		uint32_t coded_size =  htree_add_codes( tree_head, 0, 0);

		DBGPRINT("buffer with %db (%dB) symbols compressed to %db (%dB):\n", 
				tree_head->freq * 8, tree_head->freq, coded_size, coded_size%8?(1 + coded_size/8):(coded_size/8));
		//htree_print( head, 0);
		
		htree_destroy(tree_head);

//		DBGPRINT("Used symbols = %u\n", totalsymbols);

	}

	free(buffer);
	free( dictionary);
	return 0;
}

