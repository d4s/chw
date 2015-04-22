/**
 * @file   gen_unbalanced_data.c
 * @Author Denis Pynkin (d4s),  denis.pynkin@t-linux.by
 * @brief  Generate test data for unbalanced Huffman tree creation
 * @copyright Copyright (c) 2015, Denis Pynkin
 * @license This project is released under the GNU Public License.
 *
*/


#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {

	long long freq=1;
	
	int seq = 22;

	/* Sorry, too lazy to check ;-) */
	if ( argc == 2 ) {
		seq = atoi( argv[1]);
	}
	
	for( int i='A'; i<'A'+seq; i++) {
		fprintf(stderr, "i=%c,  freq=%lld\n", i, freq);
		for( int j=0;j<freq;j++)
		    printf("%c",i);
		freq*=2;
	}
	return 0;
}

