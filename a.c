#include <stdio.h>
#include <stdint.h>
#include "utils.h"

static char* file_name = "/Users/mayudong/Movies/1.mp4";
static FILE* pFile = NULL;

#define read_8() read8(pFile)
#define read_16() read16(pFile)
#define read_24() read24(pFile)
#define read_32() read32(pFile)
#define read_64() read64(pFile)
#define skip_n(x) skip(pFile,x)
#define read_n(buf, x) readn(pFile, buf, x)

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

static int parse_tkhd(uint32_t start_pos, uint32_t size)
{
	// int version = read_8();
	// printf("version = %d\n", version);
	// int flags = read_24();
	// printf("flags = %d\n", flags);

	// uint32_t creation_time = read32(pFile);
	// printf("creation_time = %d\n", creation_time);
	// uint32_t modification_time = read32(pFile);
	// printf("modification_time = %d\n", modification_time);
	// int track_id = read_32();
	// printf("track_id = %d\n", track_id);

	// skip(pFile, 8);

	// int layer = read_16();
	// printf("layer = %d\n", layer);
	// int alternate_group = read_16();
	// printf("alternate_group = %d\n", alternate_group);
	// int volume = read_16();
	// printf("volume = 0x%x\n", volume);

	// skip(pFile, 2);
	// skip(pFile, 36);//matrix

	// int width = read_32();
	// printf("width = 0x%x\n", width);
	// int height = read_32();
	// printf("hight = 0x%x\n", height);

	return 0;
}

static int parse_elst(uint32_t start_pos, uint32_t size)
{
	// int version = read_8();
	// printf("version = %d\n", version);
	// int flags = read_24();
	// printf("flags = %d\n", flags);

	// int entry_count = read_32();
	// printf("entry_count = %d\n", entry_count);
	// for(int i=0;i<entry_count;i++)
	// {
	// 	if(version == 1)
	// 	{
	// 		uint64_t segment_duration = read_64();
	// 		printf("%d:segment_duration = %lld\n", i, segment_duration);
	// 		uint64_t media_time = read_64();
	// 		printf("%d:media_time = %lld\n", i, media_time);
	// 	}
	// 	else
	// 	{
	// 		uint32_t segment_duration = read_32();
	// 		printf("%d:segment_duration = %d", i, segment_duration);
	// 		int media_time = read_32();
	// 		printf("%d:media_time = %d\n", i, media_time);
	// 	}
	// 	int media_rate_interge = read_16();
	// 	int media_rate_fraction = read_16();
	// 	printf("media_rate = %d.%d\n", media_rate_interge, media_rate_fraction);
	// }

	return 0;
}

static int parse_mdhd(uint32_t start_pos, uint32_t size)
{
	// int version = read_8();
	// printf("version = %d\n", version);
	// uint32_t flags = read_24();
	// printf("flags = %d\n", flags);

	// uint32_t creation_time = read_32();
	// printf("creation_time = %d\n", creation_time);
	// uint32_t modification_time = read_32();
	// printf("modification_time = %d\n", modification_time);
	// uint32_t time_scale = read_32();
	// printf("time_scale = %d\n", time_scale);
	// uint32_t duration = read_32();
	// printf("duration = %d\n", duration);

	// int language = read_16();
	// printf("language = %x\n", language);//todo
	return 0;
}

static int parse_hdlr(uint32_t start_pos, uint32_t size)
{
	// int version = read_8();
	// printf("version = %d\n", version);
	// uint32_t flags = read_24();
	// printf("flags = %d\n", flags);

	// uint32_t pre_define = read_32();
	// uint32_t handle_type = read_32();
	// print_fourcc(handle_type);
	// skip_n(12);

	// int name_size = size-20;
	// if(name_size > 0)
	// {
	// 	char* name = (char*)malloc(name_size+1);
	// 	memset(name, 0, name_size+1);
	// 	read_n(name, name_size);
	// 	printf("hdlr name = %s\n", name);
	// 	free(name);	
	// }
	
	return 0;
}

static int parse_dref(uint32_t start_pos, uint32_t size)
{
	int version = read_8();
	uint32_t flags = read_24();
	
	uint32_t entry_count = read_32();
	int index = 8;
	for(int i=0;i<entry_count;i++)
	{
		int size = read_box(start_pos+index);
		index += size;
		fseek(pFile, start_pos+index, SEEK_SET);

	}
	return 0;
}

static int parse_stsd(uint32_t start_pos, uint32_t size)
{
	int version = read_8();
	uint32_t flags = read_24();
	
	uint32_t entry_count = read_32();
	int index = 8;
	for(int i=0;i<entry_count;i++)
	{
		int size = read_box(start_pos+index);
		index += size;
		fseek(pFile, start_pos+index, SEEK_SET);

	}
	return 0;
}

static const MOVParseTableEntry mov_default_parse_table[] = {
	{MKTAG('f','t','y','p'), parse_ftyp},
	{MKTAG('m','o','o','v'), default_parse},
	{MKTAG('m','o','o','f'), default_parse},
	{MKTAG('m','v','h','d'), parse_mvhd},
	{MKTAG('t','r','a','k'), default_parse},
	{MKTAG('t','k','h','d'), parse_tkhd},
	{MKTAG('e','d','t','s'), default_parse},
	{MKTAG('e','l','s','t'), parse_elst},
	{MKTAG('m','d','i','a'), default_parse},
	{MKTAG('m','d','h','d'), parse_mdhd},
	{MKTAG('h','d','l','r'), parse_hdlr},
	{MKTAG('m','i','n','f'), default_parse},
	{MKTAG('d','i','n','f'), default_parse},
	{MKTAG('d','r','e','f'), parse_dref},
	{MKTAG('s','t','b','l'), default_parse},
	{MKTAG('s','t','s','d'), parse_stsd},
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