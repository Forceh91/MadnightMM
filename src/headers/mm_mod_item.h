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
};

typedef struct _mm_mod_file
{
	char *name; // name/path of the file inside the archive
	unsigned char index; // index of the file within the archive
	unsigned char flags; // flags for this mod file (see ModFileFlags above)

} mm_mod_file;

typedef struct _mm_mod_item
{
	char *mod_name; // the name of the mod
	bool enabled; // whether or not it is enabled
	char *file_path; // file path to the mod
	unsigned int file_size; // the size of this mod
	unsigned char file_format; // the format of the mod file archive (see ModFileFormat above)
	unsigned char file_count; // the number of files inside the mod archive
	unsigned char item_count; // the number of files and directories inside the mod archive
	mm_mod_file **files; // an array containing info about each file and directory inside the mod, length item_count

} mm_mod_item;


mm_mod_item *mm_create_mod_item(const char *path, const char *file);
void mm_destroy_mod_item(mm_mod_item *item);

mm_mod_file *mm_create_mod_file(unsigned char index, const char *file, bool directory);
void mm_destroy_mod_file(mm_mod_file *file);

#endif
