#pragma once

#include "record.hh"

// For benchmark
#define NUM_RECORD 10
#define NUM_THREAD 4
#define NUM_TRANSACTION 10000
#define NUM_OPERATION 16
#define R_RATIO 0

// For debug
#define CCC(val)  do {fprintf(stderr, "%16s %4d %16s %16s: %c\n", __FILE__, __LINE__, __func__, #val, val); fflush(stderr);} while (0)
#define DDD(val)  do {fprintf(stderr, "%16s %4d %16s %16s: %d\n", __FILE__, __LINE__, __func__, #val, val); fflush(stderr);} while (0)
#define PPP(val)  do {fprintf(stderr, "%16s %4d %16s %16s: %p\n", __FILE__, __LINE__, __func__, #val, val); fflush(stderr);} while (0)
#define LLL(val)  do {fprintf(stderr, "%16s %4d %16s %16s: %ld\n", __FILE__, __LINE__, __func__, #val, val); fflush(stderr);} while (0)
#define SSS(val)  do {fprintf(stderr, "%16s %4d %16s %16s: %s\n", __FILE__, __LINE__, __func__, #val, val); fflush(stderr);} while (0)
#define FFF(val)  do {fprintf(stderr, "%16s %4d %16s %16s: %f\n", __FILE__, __LINE__, __func__, #val, val); fflush(stderr);} while (0)
#define NNN       do {fprintf(stderr, "%16s %4d %16s\n", __FILE__, __LINE__, __func__); fflush(stderr);} while (0)
#define ERR     do {perror("ERROR"); NNN; exit(1);} while (0)
#define RERR(fd) do {perror("Recv Error"); NNN; close(fd); pthread_exit(NULL);} while (0)
#define ERR2    do {perror("ERROR"); NNN; pthread_exit(NULL);} while (0)

// Database
std::vector<DATA> record_set(NUM_RECORD);