/*
 * @file   huffman.h
 * @Author Denis Pynkin (d4s), denis.pynkin@t-linux.by
 * @date   2014
 * @brief  Commons for project
*/

#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#ifdef DEBUG
#define DBGPRINT(...) \
	{ fprintf(stderr, "%s:%d %s(): ", __FILE__, __LINE__, __func__ ); \
	  fprintf(stderr, __VA_ARGS__); }
#else
#define DBGPRINT(fmt,  args...)
#endif

#endif

