/**
 * @file   gen_unbalanced_data.c
 * @Author Denis Pynkin (d4s),  denis.pynkin@t-linux.by
 * @brief  Generate test data for unbalanced Huffman tree creation
 * @copyright Copyright (c) 2015, Denis Pynkin
 * @license This project is released under the GNU Public License.
 *
*/


#include <stdio.h>

int main(void) {

	long long freq=1;

	for( int i='A'; i<'A'+27; i++) {
		fprintf(stderr, "i=%c,  freq=%lld\n", i, freq);
		for( int j=0;j<freq;j++)
		    printf("%c",i);
		freq*=2;
	}
	return 0;
}

