#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "box_define.h"

#define MKTAG(a,b,c,d) ((d) | ((c) << 8) | ((b) << 16) | ((unsigned)(a) << 24))

uint8_t read8(FILE* fd);
uint16_t read16(FILE* fd);
uint32_t read24(FILE* fd);
uint32_t read32(FILE* fd);
uint64_t read64(FILE* fd);
int readn(FILE* fd, char* buf, int size);

int skip(FILE* fd, int len);


void print_box_type(uint32_t type);
void print_fourcc(uint32_t fourcc);


void *mallocz(size_t size);
BaseBox* malloc_box(uint32_t type);

#endif