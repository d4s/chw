/*
 * =====================================================================================
 *       Filename:  haffman.c
 *    Description:  
 *         Author:  Denis Pynkin (d4s), denis.pynkin@t-linux.by
 *        Company:  t-linux.by
 * ===================================================================================== */

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

#include <time.h>
#include <assert.h>

#define DICTSIZE 256
#define BUFFERSIZE 1024

int main( int argc, char **argv) {

	uint32_t dictionary[DICTSIZE];
	uint8_t buffer[BUFFERSIZE];

	int rc;
	void *rcpnt;

	int totalsymbols;


	rcpnt = memset (dictionary, 0, DICTSIZE * sizeof (uint32_t));
	assert (rcpnt != NULL);


	/** Read and count symbols */
	while (1) {
	
		ssize_t readed;

		readed = read (STDIN_FILENO, buffer, BUFFERSIZE);
		if (readed <= 0)
			break;

		/** count readed symbols */
		for (int cnt=0; (cnt < BUFFERSIZE) && (cnt < readed); cnt++) {
			dictionary[buffer[cnt]] += 1;
		}

	}



	/** Print statistics */
	for (int i = 0; i<DICTSIZE; i++)
		if (dictionary[i]) {
			printf("Symbol %.3d = %lu\n", i, dictionary[i]);
			totalsymbols++;
		}

	printf("Used symbols = %lu\n", totalsymbols);

	return 0;
}
