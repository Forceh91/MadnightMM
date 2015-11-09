#include "mm_mod_installer.h"
#include "mm_mod_item.h"
#include "mm_utils.h"
#include <stdio.h>
#include <string.h>
#include <Windows.h>

// Probably safe to assume no one will manage to install over 1000 mods at once. Probably.
#define MAX_INSTALLED_MODS 1000

// In case we want to change the contents in the future.
#define FILE_VERSION 1

static mm_installed_mod *installed_mods[MAX_INSTALLED_MODS];
static unsigned int num_installed_mods;

//
// Utility file functions. Text functions assume the buffer is always big enough and the strings are less than 255 chars, null terminated!
//
inline unsigned int mm_read_uint(FILE *f)
{
	unsigned int i;
	fread(&i, sizeof(i), 1, f);
	return i;
}

inline unsigned char mm_read_uchar(FILE *f)
{
	unsigned char i;
	fread(&i, sizeof(i), 1, f);
	return i;
}

inline char *mm_read_string(FILE *f, char *buffer)
{
	unsigned char len = mm_read_uchar(f);
	fread(buffer, len, 1, f);
	buffer[len] = 0;

	return buffer;
}

inline void mm_write_uint(FILE *f, unsigned int value)
{
	fwrite(&value, sizeof(value), 1, f);
}

inline void mm_write_uchar(FILE *f, unsigned char value)
{
	fwrite(&value, sizeof(value), 1, f);
}

inline void mm_write_string(FILE *f, const char *text)
{
	size_t len = strlen(text);

	if (len > 0xFF)
		len = 0xFF;

	mm_write_uchar(f, (unsigned char)len);
	fwrite(text, len, 1, f);
}

void mm_cleanup_installed_mods(void)
{
	for (unsigned int i = 0, j = 0; j < num_installed_mods; ++i)
	{
		if (installed_mods[i] == NULL)
			continue;

		mm_installed_mod *mod = installed_mods[i];

		if (mod->file_path != NULL) delete[] mod->file_path;
		if (mod->files != NULL) delete[] mod->files;
		delete mod;

		installed_mods[i] = NULL;
		++j;
	}
}

void mm_load_installed_mod_list(void)
{
	// TODO: Load this file from wherever config files are stored?
	const char *installed_mods_file = "./installed.dat";

	FILE *file = fopen(installed_mods_file, "r");

	// No mods have been installed previously.
	if (file == NULL)
		return;

	// Read the file version. Currently unused.
	unsigned int version = mm_read_uint(file);

	// Read the number of previously installed mods.
	unsigned int mod_count = mm_read_uint(file);
	
	for (unsigned int i = 0; i < mod_count; ++i)
	{
		mm_installed_mod *mod = new mm_installed_mod;

		// Load info about the installed mod.
		char file_path[MAX_PATH];
		mm_read_string(file, file_path);

		mod->file_path = mm_str_duplicate(file_path);
		mod->file_crc = mm_read_uint(file);
		mod->file_count = mm_read_uchar(file);

		// Load a list of files the installed mod has modified.
		mod->files = new mm_installed_file[mod->file_count];

		for (unsigned int f = 0; f < mod->file_count; ++f)
		{
			mm_read_string(file, mod->files[f].vehicle_short);
			mod->files[f].flags = mm_read_uchar(file);
			mod->files[f].livery = mm_read_uchar(file);
		}

		installed_mods[num_installed_mods++] = mod;
	}

	fclose(file);
}

void mm_save_installed_mod_list(void)
{
	// TODO: Save this to wherever config files are stored?
	const char *installed_mods_file = "./installed.dat";

	FILE *file = fopen(installed_mods_file, "w");

	// No write access or some other bad stuff.
	if (file == NULL)
		return;

	// Write the file version in case we want to upgrade the file format in the future.
	mm_write_uint(file, FILE_VERSION);

	// Write the number of installed mods.
	mm_write_uint(file, num_installed_mods);

	for (unsigned int i = 0, j = 0; j < num_installed_mods; ++i)
	{
		if (installed_mods[i] == NULL)
			continue;

		mm_installed_mod *mod = installed_mods[i];

		// Save data about the installed mod.
		mm_write_string(file, mod->file_path);
		mm_write_uint(file, mod->file_crc);
		mm_write_uchar(file, mod->file_count);

		// Save a list of modified files.
		for (unsigned int f = 0; f < mod->file_count; ++f)
		{
			mm_write_string(file, mod->files[f].vehicle_short);
			mm_write_uchar(file, mod->files[f].flags);
			mm_write_uchar(file, mod->files[f].livery);
		}

		++j;
	}

	fclose(file);
}

bool mm_is_mod_installed(mm_mod_item *mod)
{
	for (unsigned int i = 0, j = 0; j < num_installed_mods; ++i)
	{
		if (installed_mods[i] == NULL)
			continue;

		mm_installed_mod *installed_mod = installed_mods[i];

		if (_stricmp(mod->file_path, installed_mod->file_path) &&
			mod->file_crc == installed_mod->file_crc)
		{
			return true;
		}
		
		++j;
	}

	return false;
}
