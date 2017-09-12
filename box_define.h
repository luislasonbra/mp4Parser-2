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

#define MAX_COMPATIBLE_BRANDS_COUNT 16
typedef struct FileTypeBox
{
	BaseBox header;
	uint32_t major_brand;
	uint32_t minor_version;
	uint32_t compatible_brands[MAX_COMPATIBLE_BRANDS_COUNT];
}FileTypeBox;

typedef struct MovieHeaderBox
{
	BaseBox header;
	uint8_t version;
	uint32_t flags;
	uint64_t creation_time;
	uint64_t modification_time;
	uint32_t timescale;
	uint64_t duration;
	int32_t rate;
	int16_t volume;
	int32_t matrix[9];
}MovieHeaderBox;

typedef struct TrackHeaderBox
{
	BaseBox header;
	uint8_t version;
	uint32_t flags;
	uint64_t creation_time;
	uint64_t modification_time;
	uint32_t track_id;
	uint64_t duration;
	int16_t layer;
	int16_t alternate_group;
	int16_t volume;
	int32_t matrix[9];
	uint32_t width;
	uint32_t height;
}TrackHeaderBox;

#endif