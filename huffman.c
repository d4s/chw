/*
 * @file   huffman.c
 * @Author Denis Pynkin (d4s), denis.pynkin@t-linux.by
 * @date   2014
 * @brief  Huffman archiver
*/


#include <huffman.h>
#include <parse_args.h>
#include <hblock.h>

#include <time.h>


int main( int argc, char **argv) {

	uint8_t *buffer;

	int totalsymbols=0;

	appmode_t mode;

	parse_args( argc, argv, &mode);
#ifdef DEBUG
	if (mode == COMPRESSOR) {
		DBGPRINT("Starting compressor ... \n");
	} else if (mode == DECOMPRESSOR) {
		DBGPRINT("Starting decompressor ... \n");
	}
#endif
	buffer = malloc( BUFFERSIZE);
	assert( buffer != NULL);

	#ifdef _OPENMP

	int np=1;
	int myid=0;

	#pragma omp parallel
	np = omp_get_num_threads();

	np += 2; // 2 additional threads for I/O

	omp_set_dynamic(0);
	omp_set_num_threads( np);

	#pragma omp parallel private (np, myid)
	{
		np = omp_get_num_threads();
		myid = omp_get_thread_num();
		if (np<3) {
			fprintf( stderr, "At least 3 threads needed. Please use non-parallel version instead.\n");
			exit (1);
		}
			
		switch (myid) {
			case 0: /* stream reader */
				DBGPRINT( "My thread is %d and I am reader\n", myid);
				break;
			case 1: /* stream writer */
				DBGPRINT( "My thread is %d and I am writer\n", myid);
				break;
			default: /* worker */
				DBGPRINT( "My thread is %d and I am worker\n", myid);
				break;

		}
	}
	#else

	switch (mode) {
	
		case COMPRESSOR: /* Compress input stream */
			while (1) {

				/* Read data from stream */
				uint32_t readed = rawreader (STDIN_FILENO, buffer, BUFFERSIZE);
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

				streamwriter( STDOUT_FILENO, block);

				hblock_destroy( block);
			};
			break;
		
		case DECOMPRESSOR: /* Compress input stream */
			while (1) {
				hblock_t *block = streamreader( STDIN_FILENO);
				if (block == NULL)
					break;

				hblock_decompress( block);

				rawwriter( STDOUT_FILENO, block);

				hblock_destroy( block);
			};
			break;

		default:
			break;
	}

	#endif // OMP

	free(buffer);
	return 0;
}

