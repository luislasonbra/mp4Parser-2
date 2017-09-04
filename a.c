#include <stdio.h>
#include <stdint.h>
#include "utils.h"

static char* file_name = "/Users/mayudong/Movies/1.mp4";
static FILE* pFile = NULL;

static int read_box(uint32_t start_pos);

typedef struct MOVParseTableEntry
{
	uint32_t type;
	int (*parser)(uint32_t start_pos, uint32_t size);
}MOVParseTableEntry;


static int default_parse(uint32_t start_pos, uint32_t mov_size)
{
	int index = 0;
	while(index < mov_size)
	{
		int size = read_box(start_pos+index);
		index += size;
		fseek(pFile, start_pos+index, SEEK_SET);
	}
	return 0;
}

static int parse_meta(uint32_t start_pos, uint32_t mov_size)
{
	int index = 0;
	while(mov_size > 8)
	{
		uint32_t tag = read32(pFile);
		mov_size -= 4;
		index += 4;
		if(tag == MKTAG('h', 'd', 'l', 'r'))
		{
			fseek(pFile, -8, SEEK_CUR);
			index -= 8;
			mov_size += 8;
			return default_parse(start_pos+index, mov_size);
		}
	}
	return 0;
}

static int parse_ftyp(uint32_t start_pos, uint32_t size)
{
	return 0;
}

static int parse_mvhd(uint32_t start_pos, uint32_t mov_size)
{
	// int version = read8(pFile);
	// printf("version = %d\n", version);
	// uint32_t flags = read24(pFile);
	// printf("flags = %d\n", flags);
	// uint32_t creation_time = read32(pFile);
	// printf("creation_time = %d\n", creation_time);
	// uint32_t modification_time = read32(pFile);
	// printf("modification_time = %d\n", modification_time);
	// uint32_t time_scale = read32(pFile);
	// printf("time_scale = %d\n", time_scale);
	// uint32_t duration = read32(pFile);
	// printf("duration = %d\n", duration);
	// uint32_t rate = read32(pFile);
	// printf("rate = 0x%x\n", rate);
	// uint32_t volume = read16(pFile);
	// printf("volume = 0x%x\n", volume);

	// skip(pFile, 10);

	return 0;
}



static const MOVParseTableEntry mov_default_parse_table[] = {
	{MKTAG('f','t','y','p'), parse_ftyp},
	{MKTAG('m','o','o','v'), default_parse},
	{MKTAG('m','o','o','f'), default_parse},
	{MKTAG('m','v','h','d'), parse_mvhd},
	{MKTAG('t','r','a','k'), default_parse},
	{MKTAG('m','d','i','a'), default_parse},
	{MKTAG('m','i','n','f'), default_parse},
	{MKTAG('d','i','n','f'), default_parse},
	{MKTAG('s','t','b','l'), default_parse},
	{MKTAG('m','v','e','x'), default_parse},
	{MKTAG('u','d','t','a'), default_parse},
	{MKTAG('m','e','t','a'), parse_meta},
	{0, NULL}
};


static int read_box(uint32_t start_pos)
{
	int b_large_size = 0;
	uint32_t size = read32(pFile);
	uint32_t type = read32(pFile);
	print_box_type(type);
	if(size == 1)
	{
		uint64_t large_size = read64(pFile);
		b_large_size = 1;
	}

	int (*parser)(uint32_t,uint32_t) = NULL;
	for(int i=0;mov_default_parse_table[i].type != 0;i++)
	{
		if(mov_default_parse_table[i].type == type)
		{
			parser = mov_default_parse_table[i].parser;
			break;
		}
	}
	if(parser)
	{
		parser(start_pos+8, size-8);
	}

	return size;
}

int main()
{
	uint32_t cur_pos = 0;
	pFile = fopen(file_name, "rb");
	if(pFile == NULL)
	{
		printf("open file failed\n");
		return -1;
	}
	fseek(pFile, 0, SEEK_END);
	int file_size = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	while(cur_pos < file_size)
	{
		uint32_t type;
		int size = read_box(cur_pos);
		cur_pos += size;
		fseek(pFile, cur_pos, SEEK_SET);
	}

	fclose(pFile);
	pFile = NULL;
	return 0;
}