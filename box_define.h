#ifndef _BOX_DEFINE__
#define _BOX_DEFINE__

#include <stdint.h>

typedef struct BaseBox
{
	uint32_t size;
	uint32_t type;
	char name[5];
	struct BaseBox* next;
	struct BaseBox* child;
}BaseBox;

typedef struct FullBox
{
	BaseBox header;
	uint8_t version;
	uint32_t flags;
}FullBox;

#define MAX_COMPATIBLE_BRANDS_COUNT 16
typedef struct FileTypeBox
{
	BaseBox header;
	uint32_t major_brand;
	uint32_t minor_version;
	uint32_t compatible_brands[MAX_COMPATIBLE_BRANDS_COUNT];
}FileTypeBox;



#endif