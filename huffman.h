/* =====================================================================================
 *       Filename:  huffman.h
 *    Description:  
 *         Author:  Denis Pynkin (d4s), denis.pynkin@t-linux.by
 *        Company:  t-linux.by
 * ===================================================================================== */

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
#define DBGPRINT(fmt, args...) \
	{ fprintf(stderr, "%s:%d %s(): ", __FILE__, __LINE__, __func__ ); \
	  fprintf(stderr, fmt, ##args); }
#else
#define DBGPRINT(fmt,  args...)
#endif

#endif

