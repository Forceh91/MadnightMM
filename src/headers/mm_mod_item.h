#pragma once
#ifndef _mm_mod_item_h
#define _mm_mod_item_h

enum ModFileFormat {
	FORMAT_UNKNOWN,
	FORMAT_7Z,
	FORMAT_ZIP,
	FORMAT_RAR,
};

enum ModFileFlags {
	FFLAG_NONE,
	FFLAG_DIRECTORY = 1 << 0,
	FFLAG_GAME_FILE = 1 << 1,
	FFLAG_TEXTURE_LIVERY = 1 << 2,
	FFLAG_TEXTURE_INTERIOR = 1 << 3,
	FFLAG_QUALITY_HIGH = 1 << 4,
	FFLAG_LOCATION_FILE = 1 << 5,
	FFLAG_INSTALL = 1 << 6,
	FFLAG_MOD_FILE = (FFLAG_TEXTURE_LIVERY|FFLAG_TEXTURE_INTERIOR|FFLAG_LOCATION_FILE),
};

typedef struct _mm_vehicle_data mm_vehicle_data;
typedef struct _mm_stage_data mm_stage_data;

typedef struct _mm_mod_file
{
	char *name; // name of the file inside the archive (without the path)
	char *path; // full name of the file, including path and extension
	unsigned char index; // index of the file within the archive
	unsigned char flags; // flags for this mod file (see ModFileFlags above)
	unsigned char livery; // index of the livery
	mm_vehicle_data *vehicle; // vehicle this file changes
	mm_stage_data *stage; // stage this file changes

} mm_mod_file;

typedef struct _mm_mod_install_file
{
	char *file_name; // name of the file that should be installed (including path and extension)

} mm_mod_install_file;

typedef struct _mm_mod_item
{
	char *mod_name; // the name of the mod
	bool enabled; // whether or not it is enabled
	char *file_path; // file path to the mod
	unsigned int file_size; // the size of this mod
	unsigned int file_crc; // CRC32 of the entire archive
	unsigned char file_format; // the format of the mod file archive (see ModFileFormat above)
	unsigned char file_count; // the number of mod files inside the mod archive
	unsigned char item_count; // the number of all files and directories inside the mod archive
	unsigned char install_file_count; // the number of files the user has chosen to install
	bool install_list_modified; // true if the user has selected files to be installed (as opposed to installing everything by default)
	mm_vehicle_data *vehicle; // data of the vehicle this mod alters
	mm_stage_data *stage; // data of the stage this mod alters
	mm_mod_file **files; // an array containing info about each file and directory inside the mod, length item_count
	mm_mod_install_file *install_files; // an array containing all the files the user has chosen to install, length install_file_count
} mm_mod_item;


mm_mod_item *mm_create_mod_item(const char *path, const char *file);
void mm_destroy_mod_item(mm_mod_item *item);
void mm_destroy_mod_item_files(mm_mod_item *item, bool list_install_files);

mm_mod_file *mm_create_mod_file(unsigned char index, const char *file, bool directory);
void mm_destroy_mod_file(mm_mod_file *file);
void mm_get_mod_file_path(mm_mod_file *file, char *buffer, size_t buflen, const char *base_path = 0, bool include_file = true);
void mm_get_mod_file_path(const char *file_name, const char *vehicle_name_short, unsigned char file_flags, unsigned char livery_index, char *buffer, size_t buflen, const char *base_path = 0, bool include_file = true);
bool mm_is_file_chosen_for_install(mm_mod_item *mod, mm_mod_file *file);

bool mm_is_valid_location(const char* location_name);

#endif
