/*
 * @file   huffman.c
 * @Author Denis Pynkin (d4s), denis.pynkin@t-linux.by
 * @date   2014
 * @brief  Huffman archiver
*/


#include <huffman.h>
#include <hblock.h>

#include <time.h>

int main( int argc, char **argv) {

	uint8_t *buffer;

	int totalsymbols=0;

	int np=1;
	int myid=0;


	buffer = malloc( BUFFERSIZE);
	assert( buffer != NULL);

#ifdef _OPENMP
#pragma omp parallel
	np = omp_get_num_threads();
#endif

	/** Read and count symbols */
	while (1) {
		ssize_t readed;

		/* Read data from stream */
		readed = read (STDIN_FILENO, buffer, BUFFERSIZE);
		if (readed <= 0)
			break;
		DBGPRINT("Read block of %d size\n", readed);
#ifdef DEBUG
		/* probably impossible ? */
		assert (readed <= BUFFERSIZE);
#endif

		hblock_t *block = hblock_create( buffer, readed, RAW_READY);
		assert (block != NULL);

		hblock_compress( block);

		hblock_destroy( block);


//		DBGPRINT("Used symbols = %u\n", totalsymbols);

	}

	free(buffer);
	return 0;
}

