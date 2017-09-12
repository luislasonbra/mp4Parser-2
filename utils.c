#include "utils.h"

typedef struct BoxSizeTable
{
	uint32_t type;
	int size;
}BoxSizeTable;

static const BoxSizeTable box_size_table[] = 
{
	{MKTAG('f','t','y','p'), sizeof(FileTypeBox)},
	{0, 0}
};

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

char *fourcc_make_string(char *buf, uint32_t fourcc)
{
	buf[0] = fourcc>>24;
	buf[1] = (fourcc>>16)&0xff;
	buf[2] = (fourcc>>8)&0xff;
	buf[3] = fourcc&0xff;
	buf[4] = 0;
	return buf;
}


void *mallocz(size_t size)
{
    void *ptr = malloc(size);
    if (ptr)
        memset(ptr, 0, size);
    return ptr;
}

static void fill_box(BaseBox* box, uint32_t type, uint32_t size)
{
	box->type = type;
	box->size = size;
	box->name[0] = type>>24;
	box->name[1] = (type>>16)&0x00ff;
	box->name[2] = (type>>8)&0x0000ff;
	box->name[3] = type&0x000000ff;
	box->name[4] = 0;
}

BaseBox* malloc_box(uint32_t type, uint32_t size)
{
	BaseBox* box = NULL;
	int malloc_size = sizeof(BaseBox);
	for(int i=0;box_size_table[i].type != 0;i++)
	{
		if(box_size_table[i].type == type)
		{
			malloc_size = box_size_table[i].size;
			break;
		}
	}
	box = mallocz(malloc_size);
	fill_box(box, type, size);
	return box;
}





