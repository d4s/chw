/*
 * @file   parse_args.c
 * @Author Denis Pynkin (d4s), denis.pynkin@t-linux.by
 * @date   2014
 * @brief  Check command line options for huffman archiver
 *
*/

#include "parse_args.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

/* External variables for arguments parsing */
extern char *optarg;
extern int optind,  opterr,  optopt;

int fd_input, fd_output;

void help( char * name) {
	printf( "Stream compressor/decompressor\n");
	printf( "Usage: %s [-dxc] [infile] [outfile]\n", name);
	printf( "-c -- compress\n");
	printf( "-d|-x -- decompress\n");
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

	char optstring[]="dxc";

	(* mode)=COMPRESSOR;
	/* Default stdin/stdout */
	fd_input = STDIN_FILENO;
	fd_output = STDOUT_FILENO;

	while (( arg = getopt( argc, argv, optstring)) != -1){
		switch (arg){
			case 'c':
				(* mode) = COMPRESSOR;
				break;
			case 'x':
			case 'd':
				(* mode) = DECOMPRESSOR;
				break;
			default:
				help( argv[0]);
				exit(1);
		}
	}

	/* Do not care about security here, huh */
	/* Check if we have input filename */
	if ( optind < argc ) {

		fd_input = open( argv[optind], O_RDONLY);

		if ( fd_input == -1 ) {
			perror("Failed to open input file");
			exit(1);
		}


		DBGPRINT("Input file: %s (%d)\n", argv[optind], fd_input);

		optind++;

	}

	/* Check if we have output filename */
	if ( optind < argc ) {

		fd_output = open( argv[optind], O_CREAT|O_WRONLY, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
		if ( fd_output == -1 ) {
			close( fd_input);
			perror("Failed to create output file");
			exit(1);
		}

		DBGPRINT("Output file: %s (%d)\n", argv[optind], fd_output);

		optind++;
	}

	DBGPRINT("Options parsed %d of %d\n", optind, argc);
	if (optind != argc) {
		help( argv[0]);
		close( fd_input);
		close( fd_output);
		exit( 1);
	}

	return 0;
}
