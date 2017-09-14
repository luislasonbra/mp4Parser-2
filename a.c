#include <stdio.h>
#include <stdint.h>
#include "utils.h"
#include "box_define.h"

static char* file_name = "/Users/mayudong/Movies/1.mp4";
static FILE* pFile = NULL;

#define MAX_STREAM_COUNT 10

typedef struct Stream
{
	int index;
	uint32_t type;
	int width;
	int height;
	char language[4];
}Stream;

typedef struct Context
{
	BaseBox root;
	int stream_num;
	Stream streams[MAX_STREAM_COUNT];
}Context;


#define read_8() read8(pFile)
#define read_16() read16(pFile)
#define read_24() read24(pFile)
#define read_32() read32(pFile)
#define read_64() read64(pFile)
#define skip_n(x) skip(pFile,x)
#define read_n(buf, x) readn(pFile, buf, x)

static BaseBox* read_box(Context* c, uint32_t start_pos);
static void add_box(BaseBox* root, BaseBox* new_box);


typedef int (*BOX_PARSER)(Context* c, BaseBox* root, uint32_t start_pos, uint32_t size);

typedef struct MOVParseTableEntry
{
	uint32_t type;
	BOX_PARSER parser;
}MOVParseTableEntry;


static int default_parse(Context* c, BaseBox* root, uint32_t start_pos, uint32_t mov_size)
{
	int index = 0;
	while(index < mov_size)
	{
		BaseBox* box = read_box(c, start_pos+index);
		add_box(root, box);
		index += box->size;
		fseek(pFile, start_pos+index, SEEK_SET);
	}
	return 0;
}

static int parse_ftyp(Context* c, BaseBox* root, uint32_t start_pos, uint32_t mov_size)
{
	if(root == NULL)
		return 0;

	FileTypeBox* file_type_box = (FileTypeBox*)root;
	file_type_box->major_brand = read_32();
	file_type_box->minor_version = read_32();
	int count = (mov_size-8)/4;
	for(int i=0;i<count;i++)
	{
		file_type_box->compatible_brands[i] = read_32();
	}

	return 0;
}

static int parse_mvhd(Context* c, BaseBox* root, uint32_t start_pos, uint32_t mov_size)
{
	MovieHeaderBox* box = (MovieHeaderBox*)root;
	box->version = read_8();
	box->flags = read_24();
	if(box->version == 1)
	{
		box->creation_time = read_64();
		box->modification_time = read_64();
		box->timescale = read_32();
		box->duration = read_64();
	}
	else
	{
		box->creation_time = read_32();
		box->modification_time = read_32();
		box->timescale = read_32();
		box->duration = read_32();	
	}
	box->rate = read_32();
	box->volume = read_16();
	skip_n(10);
	for(int i=0;i<9;i++)
	{
		box->matrix[i] = read_32();
	}
	return 0;
}

static int parse_trak(Context* c, BaseBox* root, uint32_t start_pos, uint32_t mov_size)
{
	if(c->stream_num >= MAX_STREAM_COUNT)
	{
		return -1;
	}

	c->streams[c->stream_num].index = c->stream_num;
	c->stream_num++;

	int index = 0;
	while(index < mov_size)
	{
		BaseBox* box = read_box(c, start_pos+index);
		add_box(root, box);
		index += box->size;
		fseek(pFile, start_pos+index, SEEK_SET);
	}
	return 0;
}

static int parse_tkhd(Context* c, BaseBox* root, uint32_t start_pos, uint32_t mov_size)
{
	TrackHeaderBox* box = (TrackHeaderBox*)root;
	box->version = read_8();
	box->flags = read_24();
	if(box->version == 1)
	{
		box->creation_time = read_64();
		box->modification_time = read_64();
		box->track_id = read_32();
		skip_n(4);
		box->duration = read_64();
	}
	else
	{
		box->creation_time = read_32();
		box->modification_time = read_32();
		box->track_id = read_32();
		skip_n(4);
		box->duration = read_32();	
	}
	skip_n(8);
	box->layer = read_16();
	box->alternate_group = read_16();
	box->volume = read_16();
	skip_n(2);
	for(int i=0;i<9;i++)
	{
		box->matrix[i] = read_32();
	}
	box->width = read_32()>>16;
	box->height = read_32()>>16;
	c->streams[c->stream_num-1].width = box->width;
	c->streams[c->stream_num-1].height = box->height;
	
	return 0;
}

static int parse_elst(Context* c, BaseBox* root, uint32_t start_pos, uint32_t mov_size)
{
	return 0;
}


static void make_language_iso639(uint16_t lang, char language[4])
{
	language[3] = 0;
	language[2] = (lang & 0x001f) + 0x60;
	lang >>= 5;
	language[1] = (lang & 0x001f) + 0x60;
	lang >>= 5;
	language[0] = (lang & 0x001f) + 0x60;
}

static int parse_mdhd(Context* c, BaseBox* root, uint32_t start_pos, uint32_t mov_size)
{
	MediaHeaderBox* box = (MediaHeaderBox*)root;
	box->version = read_8();
	box->flags = read_24();
	if(box->version == 1)
	{
		box->creation_time = read_64();
		box->modification_time = read_64();
		box->timescale = read_32();
		box->duration = read_64();
	}
	else
	{
		box->creation_time = read_32();
		box->modification_time = read_32();
		box->timescale = read_32();
		box->duration = read_32();	
	}
	uint16_t language = read_16();
	make_language_iso639(language, box->language);
	strcpy(c->streams[c->stream_num-1].language, box->language);
	return 0;
}

static int parse_hdlr(Context* c, BaseBox* root, uint32_t start_pos, uint32_t mov_size)
{
	HandlerBox* box = (HandlerBox*)root;
	box->version = read_8();
	box->flags = read_24();
	read_32();
	box->handler_type = read_32();
	c->streams[c->stream_num-1].type = box->handler_type;
	skip_n(12);

	int name_len = mov_size-20;
	if(name_len > 0)
	{
		char* name = (char*)malloc(name_len+1);
		memset(name, 0, name_len+1);
		read_n(name, name_len);
		box->name = name;
	}
	return 0;
}

static int parse_stsd(Context* c, BaseBox* root, uint32_t start_pos, uint32_t mov_size)
{
	int version = read_8();
	uint32_t flags = read_24();
	
	uint32_t entry_count = read_32();
	int index = 8;
	for(int i=0;i<entry_count;i++)
	{
		BaseBox* box = read_box(c, start_pos+index);
		add_box(root, box);
		index += box->size;
		fseek(pFile, start_pos+index, SEEK_SET);
	}
	return 0;
}

static const MOVParseTableEntry mov_default_parse_table[] = {
	{MKTAG('f','t','y','p'), parse_ftyp},
	{MKTAG('m','o','o','v'), default_parse},
	{MKTAG('m','v','h','d'), parse_mvhd},
	// {MKTAG('m','o','o','f'), default_parse},
	{MKTAG('t','r','a','k'), parse_trak},
	{MKTAG('t','k','h','d'), parse_tkhd},
	{MKTAG('e','d','t','s'), default_parse},
	{MKTAG('e','l','s','t'), parse_elst},
	{MKTAG('m','d','i','a'), default_parse},
	{MKTAG('m','d','h','d'), parse_mdhd},
	{MKTAG('h','d','l','r'), parse_hdlr},
	{MKTAG('m','i','n','f'), default_parse},
	// {MKTAG('d','i','n','f'), default_parse},
	// {MKTAG('d','r','e','f'), parse_dref},
	{MKTAG('s','t','b','l'), default_parse},
	{MKTAG('s','t','s','d'), parse_stsd},
	// {MKTAG('m','v','e','x'), default_parse},
	// {MKTAG('u','d','t','a'), default_parse},
	// {MKTAG('m','e','t','a'), parse_meta},
	{0, NULL}
};


static BaseBox* read_box(Context* c, uint32_t start_pos)
{
	int b_large_size = 0;
	uint32_t size = read32(pFile);
	uint32_t type = read32(pFile);
	BaseBox* new_box = malloc_box(type, size);
	if(size == 1)
	{
		uint64_t large_size = read64(pFile);
		b_large_size = 1;
	}

	BOX_PARSER parser = NULL;
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
		parser(c, new_box, start_pos+8, size-8);
	}

	return new_box;
}

static void show_box(BaseBox* root, int depth)
{
	BaseBox* box = root;
	for(int i=0;i<depth;i++)
	{
		printf("    ");
	}

	if(root->type != 0)
	{
		printf("%s\n", fourcc2str(root->type));
	}

	if(box->child != NULL)
	{
		show_box(box->child, depth+1);
	}
	if(box->next)
	{
		box = box->next;
		show_box(box, depth);
	}
}

static void add_box(BaseBox* root, BaseBox* new_box)
{
	if(root == NULL)
		return;

	if(root->child == NULL)
	{
		root->child = new_box;
		return;
	}

	BaseBox* tmp = root->child;
	while(tmp->next != NULL)
	{
		tmp = tmp->next;
	}
	tmp->next = new_box;
	return;
}

int main(int argc, char** argv)
{
	if(argc > 1)
	{
		file_name = argv[1];
	}

	Context* c = (Context*)mallocz(sizeof(Context));

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

	BaseBox* root_box = &c->root;
	while(cur_pos < file_size)
	{
		BaseBox* box = read_box(c, cur_pos);
		add_box(root_box, box);
		cur_pos += box->size;
		fseek(pFile, cur_pos, SEEK_SET);
	}

	fclose(pFile);
	pFile = NULL;

	show_box(&c->root, -1);

	for(int i=0;i<c->stream_num;i++)
	{
		printf("--------------------------------\n");
		printf("index = %d\n", c->streams[i].index);
		printf("type = %s\n", fourcc2str(c->streams[i].type));
		printf("width = %d, height = %d\n", c->streams[i].width, c->streams[i].height);
		printf("language = %s\n", c->streams[i].language);
	}

	free(c);
	return 0;
}