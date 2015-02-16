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

#include <hpb.pb-c.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#define DICTSIZE 256 /**< count of elements in dictionary */
#define BUFFERSIZE 512*1024

#define HPB_MESSAGE_MAX BUFFERSIZE*2 /**< to be sure we have enough space for read messages from stream */

#ifdef DEBUG
#define DBGPRINT(...) \
	{ fprintf(stderr, "%s:%d %s(): ", __FILE__, __LINE__, __func__ ); \
	  fprintf(stderr, __VA_ARGS__); }
#define FUNC_ENTER(...) DBGPRINT("Enter\n");
#define FUNC_LEAVE(...) DBGPRINT("Leave\n");
#else
#define DBGPRINT(...)
#define FUNC_ENTER(...)
#define FUNC_LEAVE(...)
#endif

#endif

