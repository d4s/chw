/*
 * @file   huffman.c
 * @Author Denis Pynkin (d4s), denis.pynkin@t-linux.by
 * @date   2014
 * @brief  Huffman archiver
*/


#include <huffman.h>
#include <parse_args.h>
#include <hblock.h>
#include <fqueue.h>

#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main( int argc, char **argv) {

	uint8_t *buffer;

	appmode_t mode;

	parse_args( argc, argv, &mode);
#ifdef DEBUG
	if (mode == COMPRESSOR) {
		DBGPRINT("Starting compressor ... \n");
	} else if (mode == DECOMPRESSOR) {
		DBGPRINT("Starting decompressor ... \n");
	}
#endif

#ifdef _OPENMP
	fqhdr_t *fqueue;
	fqueue = malloc( sizeof(fqhdr_t));
	assert( fqueue != NULL);

	memset( fqueue, 0, sizeof(fqhdr_t));

	int np=1; /**< Threads count */
	int myid=0; /**< ID of current thread */

	uint32_t cnt = 0;

	#pragma omp parallel
	np = omp_get_num_threads();

	np += 4; // 2 additional threads for I/O

	omp_set_dynamic(0);
	omp_set_num_threads( np);

	omp_set_nested(1);

	pstate_t program_state = WORKING;

	uint32_t workers_cnt = 0;

	{
	#pragma omp parallel
		np = omp_get_num_threads();
		if (np<3) {
			fprintf( stderr, "At least 3 threads needed. Please use non-parallel version instead.\n");
			exit (1);
		}
		/* Get a chance to init all threads prior real work started */
		switch (mode) {
			case COMPRESSOR:

#pragma omp parallel private (myid, cnt) shared (fqueue, program_state, workers_cnt) num_threads(3)
#pragma omp single
			{
#pragma omp task private (myid, cnt) shared( fqueue, program_state, workers_cnt)
#pragma omp parallel private (myid, cnt) shared( fqueue, program_state, workers_cnt) num_threads(1)
				{
					myid = omp_get_thread_num();
					/* stream reader */
						DBGPRINT( "My thread is %d and I am reader\n", myid);

						while (1) {
						/* Read data from stream */
//					#pragma omp critical	
							buffer = malloc( BUFFERSIZE);
							assert( buffer != NULL);

							uint32_t readed = rawreader (STDIN_FILENO, buffer, BUFFERSIZE);
							if (readed <= 0) {

#pragma omp atomic write
								program_state = PENDING;
								
								DBGPRINT("Set state PENDING\n");
								break;
							}
							DBGPRINT("Read block of %d size\n", readed);

#ifdef DEBUG
							/* probably impossible ? */
							assert (readed <= BUFFERSIZE);
#endif
							hblock_t *block;
							block = hblock_create( buffer, readed, RAW_READY);
							assert (block != NULL);

#pragma omp critical (fq)
							fqueue_push_node( fqueue, block); 
						}
				}

#pragma omp task private (myid, cnt) shared( fqueue, program_state, workers_cnt)
#pragma omp parallel private (myid, cnt) shared( fqueue, program_state, workers_cnt) num_threads(1)
				{
					myid = omp_get_thread_num();
						DBGPRINT( "My thread is %d and I am writer\n", myid);

						cnt = 0;
						while (1) {

							hblock_t *block;
#pragma omp critical (fq)
							block = fqueue_pop_node( fqueue);
							if (block != NULL) {
								DBGPRINT( "Writer thread %d: pop READY block\n", myid);
								streamwriter( STDOUT_FILENO, block);
								hblock_destroy( block);
							} else {

								pstate_t local_program_state;
								#pragma omp atomic read
								local_program_state = program_state;


								if ( local_program_state == FINISHED) {
								DBGPRINT( "Thread writer %d: iteration %d: FINISHED\n", myid, cnt);
									break;
								}
//DBGPRINT( "Thread %d: iteration %d: READY not found\n", myid, cnt);
								#pragma omp taskyield
							}
							cnt++;

						}
					}

#pragma omp task  private (myid, cnt) shared( fqueue, program_state, workers_cnt)
#pragma omp parallel private (myid, cnt) shared( fqueue, program_state, workers_cnt) num_threads(np-2)
					{
						myid = omp_get_thread_num();
					 /* worker */
						DBGPRINT( "My thread is %d and I am worker\n", myid);

						#pragma omp atomic update
						workers_cnt += 1;

						cnt=0;
						while (1) {

							hblock_t *block;
#pragma omp critical (fq) 
							{
								block = fqueue_get_next_node( fqueue, RAW_READY);
								if ( block != NULL)
									hblock_set_state( block, PROCESSING);
							}
							if (block != NULL) {
								DBGPRINT( "Worker thread %d: found RAW_READY block\n", myid);
								hblock_compress( block);
							} else {
								pstate_t local_program_state;
								#pragma omp atomic read
								local_program_state = program_state;

								if ( local_program_state == PENDING) {
									#pragma omp atomic update
									workers_cnt -= 1;
									break;
								}
//								DBGPRINT( "Thread worker %d: iteration %d: RAW_READY not found\n", myid, cnt);
								#pragma omp taskyield
							}
							cnt++;
						}
#pragma omp barrier
						DBGPRINT( "Thread worker %d: finished\n", myid);

#pragma omp atomic write
						program_state = FINISHED;
						DBGPRINT( "Thread worker %d: set FINISHED\n", myid);
				}

			}
				break;
			case DECOMPRESSOR:
				myid = omp_get_thread_num();
				switch (myid) {
					case 0: /* stream reader */
						DBGPRINT( "My thread is %d and I am reader\n", myid);

						while (1) {
						/* Read data from stream */
//					#pragma omp critical	
							buffer = malloc( HPB_MESSAGE_MAX);
							assert( buffer != NULL);

							uint32_t readed = rawreader (STDIN_FILENO, buffer, HPB_MESSAGE_MAX);
							if (readed <= 0) {
								program_state = PENDING;
								break;
							}
							DBGPRINT("Read block of %d size\n", readed);

#ifdef DEBUG
							/* probably impossible ? */
							assert (readed <= BUFFERSIZE);
#endif

							hblock_t *block = hblock_create( buffer, readed, RAW_READY);
							assert (block != NULL);

							fqueue_push_node( fqueue, block); 
						}
						break;
					case 1: /* stream writer */
						DBGPRINT( "My thread is %d and I am writer\n", myid);

						cnt = 0;
						while (1) {

							hblock_t *block = fqueue_pop_node( fqueue);
							if (block != NULL) {
								DBGPRINT( "Thread %d: pop READY block\n", myid);
								streamwriter( STDOUT_FILENO, block);
								hblock_destroy( block);
							} else {
								if ( program_state == FINISHED) {
									break;
								}
#pragma omp taskyield
								DBGPRINT( "Thread %d: iteration %d: READY not found\n", myid, cnt);
							}
							cnt++;

						}
						break;
					default: /* worker */
						DBGPRINT( "My thread is %d and I am worker\n", myid);

						cnt=0;
						while (1) {

							hblock_t *block = fqueue_get_next_node( fqueue, RAW_READY);
							if (block != NULL) {
								DBGPRINT( "Thread %d: found RAW_READY block\n", myid);
								hblock_compress( block);
							} else {
								if ( program_state == PENDING) {
									program_state = FINISHED;
									break;
								}
#pragma omp taskyield
								DBGPRINT( "Thread %d: iteration %d: RAW_READY not found\n", myid, cnt);
							}
							cnt++;
						}
						break;

				}
		} // mode
	} // #pragma
#else

	switch (mode) {
	
		case COMPRESSOR: /* Compress input stream */
			buffer = malloc( BUFFERSIZE);
			assert( buffer != NULL);

			while (1) {

				/* Read data from stream */
				uint32_t readed = rawreader (fd_input, buffer, BUFFERSIZE);

				DBGPRINT("Read block of %d size\n", readed);

				if (readed <= 0)
					break;

				#ifdef DEBUG
				/* probably impossible ? */
				assert (readed <= BUFFERSIZE);
				#endif

				hblock_t *block = hblock_create( buffer, readed, RAW_READY);
				assert (block != NULL);

				hblock_compress( block);

				streamwriter( fd_output, block);

				hblock_destroy( block);
			};
			break;
		
		case DECOMPRESSOR: /* Compress input stream */
			buffer = malloc( HPB_MESSAGE_MAX);
			assert( buffer != NULL);

			while (1) {
				hpb_t *hpb = hpb_reader( fd_input, buffer, HPB_MESSAGE_MAX);
				if (hpb == NULL) {
					break;
				}

				DBGPRINT("Successful read of message with %d b compressed\n", hpb->bits_len);


				
				hblock_t *block = streamreader( STDIN_FILENO);

				hblock_decompress( block);

				rawwriter( fd_output, block);

				hblock_destroy( block);
			};
			break;

		default:
			break;
	}

#endif // OMP


#ifdef _OPENMP
	free(fqueue);
#endif


	close( fd_input);
	close( fd_output);

	free(buffer);
	return 0;
}

