/*
 * @file   parse_args.h
 * @Author Denis Pynkin (d4s), denis.pynkin@t-linux.by
 * @date   2014
 * @brief  Check command line options for huffman archiver
 *
*/


#ifndef PARSE_ARGS_H
#define PARSE_ARGS_H

#include <huffman.h>

enum appmode {
	COMPRESSOR, 
	DECOMPRESSOR
};

typedef enum appmode appmode_t;

extern int fd_input, fd_output;

/**
 * @brief Parse command line arguments
 *
 * @param argc
 * @param argv
 * @param[out] mode Pointer to mode
 *
 * @return zero on success
 */
int parse_args( int argc, char **argv, appmode_t *mode);

#endif /* PARSE_ARGS_H */
