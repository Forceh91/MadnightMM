#pragma once
#ifndef _mm_utils_h
#define _mm_utils_h

#include <stddef.h>

enum DiRTVehicleClass {
	CLASS_60,
	CLASS_70,
	CLASS_80,
	CLASS_GRB,
	CLASS_GRB_RWD,
	CLASS_GRA,
	CLASS_F2,
	CLASS_WRC00,
	CLASS_WRC10,
	CLASS_R4,
	CLASS_RALLYX,
	CLASS_PIKESPEAK,

	NUM_VEH_CLASSES
};

typedef struct _mm_vehicle_data
{
	const char *short_name;
	const char *name;
	unsigned char vehicle_class;

} mm_vehicle_data;

const char *mm_get_file_extension(const char *file, size_t length = 0);
const char *mm_get_token_from_end(const char *str, char token, size_t length = 0);

void mm_str_cpy(char *dest, const char *src, size_t size);
char *mm_str_duplicate(const char *text);

void mm_ensure_folder_exists(const char *path);
bool mm_file_exists(const char *path);

mm_vehicle_data *mm_get_vehicle_data(const char *short_name);

unsigned int mm_calculate_file_crc32(const char *file);

#endif
