
#include "utils.h"

uint8_t read8(FILE* fd)
{
	uint8_t ret;
	fread(&ret, 1, 1, fd);
	return ret;
}

uint16_t read16(FILE* fd)
{
	uint8_t buffer[2];
	fread(buffer, 2, 1, fd);
	return ((buffer[0]<<8) | buffer[1]);
}

uint32_t read24(FILE* fd)
{
	uint8_t buffer[3];
	fread(buffer, 3, 1, fd);
	return ((buffer[1]<<16) | (buffer[1]<<8) | buffer[2]);
}

uint32_t read32(FILE* fd)
{
	uint8_t buffer[4] = {0};
	fread(buffer, 4, 1, fd);
	return (buffer[0]<<24) | (buffer[1]<<16) | (buffer[2]<<8) | (buffer[3]);
}

uint64_t read64(FILE* fd)
{
	uint64_t h = read32(fd);
	uint64_t l = read32(fd);
	return (h<<32) | l;
}

int skip(FILE* fd, int len)
{
	return fseek(fd, len, SEEK_CUR);	
}

int readn(FILE* fd, char* buf, int size)
{
	return fread(buf, 1, size, fd);
}


void print_box_type(uint32_t type)
{
	char buffer[5] = {0};
	buffer[0] = type>>24;
	buffer[1] = (type>>16)&0xff;
	buffer[2] = (type>>8)&0xff;
	buffer[3] = type&0xff;
	buffer[4] = 0;
	printf("type = %s\n", buffer);
}

void print_fourcc(uint32_t fourcc)
{
	char buffer[5] = {0};
	buffer[0] = fourcc>>24;
	buffer[1] = (fourcc>>16)&0xff;
	buffer[2] = (fourcc>>8)&0xff;
	buffer[3] = fourcc&0xff;
	buffer[4] = 0;
	printf("fourcc = %s\n", buffer);
}
