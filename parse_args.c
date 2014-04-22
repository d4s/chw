/*
 * @file   parse_args.c
 * @Author Denis Pynkin (d4s), denis.pynkin@t-linux.by
 * @date   2014
 * @brief  Check command line options for huffman archiver
 *
*/

#include "parse_args.h"

/* External variables for arguments parsing */
extern char *optarg;
extern int optind,  opterr,  optopt;


void help( char * name) {
	printf( "Stream compressor/decompressor\n");
	printf( "Usage: %s [-d]\n", name);
	printf( "-d -- decompress\n");
}

/**
 * @brief Parse command line arguments
 *
 * @param argc
 * @param argv
 * @param[out] mode Pointer to mode
 *
 * @return zero on success
 */
int parse_args( int argc, char **argv, mode_t *mode) {
	int arg;

	// d -- decompress

	char optstring[]="d";

	(* mode)=COMPRESSOR;

	while (( arg = getopt( argc, argv, optstring)) != -1){
		switch (arg){
			case 'd':
				(* mode) = DECOMPRESSOR;
				break;
			default:
				help( argv[0]);
				exit(1);
		}
	}

	return 0;
}
