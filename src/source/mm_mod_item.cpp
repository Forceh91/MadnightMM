#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mm_mod_item.h"
#include "mm_mod_archive.h"
#include "mm_utils.h"

mm_mod_item *mm_create_mod_item(const char *path, const char *file)
{
	size_t len = strlen(file);

	// Figure out the file extension so we can open the archive.
	const char *extension = mm_get_file_extension(file, len);
	ModFileFormat format = FORMAT_UNKNOWN;

	if (strcmp(extension, "zip") == 0) format = FORMAT_ZIP;
	else if (strcmp(extension, "rar") == 0) format = FORMAT_RAR;
	else if (strcmp(extension, "7z") == 0) format = FORMAT_7Z;

	// We currently support mods in .zip, .rar and .7z archives.
	if (format == FORMAT_UNKNOWN)
		return NULL;

	// create a new mm_mod_item
	mm_mod_item *item = new mm_mod_item();
	
	item->enabled = false;
	item->file_size = 0;
	item->file_format = format;
	item->file_count = 0;
	item->item_count = 0;
	item->files = NULL;

	// store the mod file path
	size_t len2 = len + strlen(path) + 2;
	item->file_path = new char[len2];
	sprintf(item->file_path, "%s/%s", path, file);

	// store the mod name
	len2 = len - strlen(extension);
	item->mod_name = new char[len2];
	strncpy(item->mod_name, file, len2 - 1);
	item->mod_name[len2 - 1] = '\0';

	// do some other shit (such as figuring out what files go in it)
	ModArchive *archive = new ModArchive(item);

	if (!archive->Scan())
	{
		mm_destroy_mod_item(item);
		item = NULL;
	}
	
	archive->Release();
	return item;
}

void mm_destroy_mod_item(mm_mod_item *item)
{
	if (item->mod_name != NULL) delete[] item->mod_name;
	if (item->file_path != NULL) delete[] item->file_path;

	mm_destroy_mod_item_files(item);
	
	delete item;
}

void mm_destroy_mod_item_files(mm_mod_item *item)
{
	for (int i = 0; i < item->item_count; ++i)
	{
		mm_destroy_mod_file(item->files[i]);
	}

	delete[] item->files;
	item->files = NULL;

	item->item_count = 0;
	item->file_count = 0;
}

static void mm_parse_livery_file(mm_mod_file *file)
{
	char vehicle[4], name[128];
	char *livery = NULL, *quality = NULL;
	unsigned char livery_index = 0xFF;
	
	mm_str_cpy(name, file->name, sizeof(name));
	mm_str_cpy(vehicle, name, sizeof(vehicle));

	size_t len = strlen(name);
	bool is_numeric = true;

	// Ignore the extension.
	while (--len)
	{
		if (name[len] == '.')
		{
			name[len] = '\0';
			break;
		}
	}

	if (len == 0)
		return;

	// Try to look for the livery ID.
	while (--len)
	{
		char c = name[len];

		if (c == '_')
		{
			name[len] = '\0';
			break;
		}

		livery = &name[len];
		is_numeric = (is_numeric && c >= '0' && c <= '9');
	}

	if (!is_numeric || livery == NULL || len == 0)
	{
		// The file is not a valid livery file because it does not define the livery index at the end of the name.
		return;
	}

	// Try to look for the quality definition.
	while (--len)
	{
		char c = name[len];

		if (c == '_')
			break;

		quality = &name[len];
	}

	if (quality == NULL)
	{
		// Quality has not been defined, ignore the file.
		return;
	}

	file->livery = (unsigned char)atoi(livery);

	if (strcmp(quality, "high") == 0)
		file->flags |= FFLAG_QUALITY_HIGH;

	file->flags |= FFLAG_TEXTURE_LIVERY;
	file->vehicle = mm_get_vehicle_data(vehicle);
}

static void mm_parse_interior_file(mm_mod_file *file)
{
	char vehicle[4];

	mm_str_cpy(vehicle, &file->name[4], sizeof(vehicle));

	file->flags |= FFLAG_TEXTURE_INTERIOR;
	file->vehicle = mm_get_vehicle_data(vehicle);
}

mm_mod_file *mm_create_mod_file(unsigned char index, const char *file, bool directory)
{
	mm_mod_file *mod_file = new mm_mod_file();
	mod_file->index = index;
	mod_file->flags = FFLAG_NONE;
	
	if (directory)
		mod_file->flags |= FFLAG_DIRECTORY;

	const char *extension = mm_get_file_extension(file);
	
	if (strcmp(extension, "pssg") == 0)
		mod_file->flags |= FFLAG_GAME_FILE;

	// Store the path to the mod file.
	size_t len = strlen(file);

	mod_file->path = new char[len + 1];
	strcpy(mod_file->path, file);

	// Store the name of the file without the path.
	const char *name = mm_get_token_from_end(file, '\\', len);
	len = strlen(name);

	if (len == 0)
	{
		len = strlen(file);
		name = file;
	}

	mod_file->name = new char[++len];
	mm_str_cpy(mod_file->name, name, len);

	// If the file is a game file, figure out its type.
	if ((mod_file->flags & FFLAG_GAME_FILE) != 0)
	{
		if (strncmp(mod_file->name, "int_", 4) == 0)
		{
			// File is likely an interior.
			mm_parse_interior_file(mod_file);
		}
		else
		{
			// File is likely a livery.
			mm_parse_livery_file(mod_file);
		}

		// Make sure the vehicle data is valid. If not, the file is not a mod file we support at the moment.
		if (mod_file->vehicle == NULL)
		{
			mod_file->flags &= ~FFLAG_MOD_FILE;
		}
	}

	return mod_file;
}

void mm_destroy_mod_file(mm_mod_file *file)
{
	if (file->name != NULL) delete[] file->name;
	if (file->path != NULL) delete[] file->path;

	delete file;
}

void mm_get_mod_file_path(mm_mod_file *file, char *buffer, size_t buflen, const char *base_path /* = NULL */, bool include_file /* = true */)
{
	if ((file->flags & FFLAG_TEXTURE_LIVERY) != 0)
	{
		// File is a car livery.
		sprintf_s(buffer, buflen, "%s\\cars\\models\\%s\\livery_%02u\\textures_%s\\%s",
			(base_path ? base_path : ""),
			file->vehicle->short_name,
			file->livery,
			((file->flags & FFLAG_QUALITY_HIGH) != 0 ? "high" : "low"),
			(include_file ? file->name : ""));
	}

	else if ((file->flags & FFLAG_TEXTURE_INTERIOR) != 0)
	{
		// File is a car interior
		sprintf_s(buffer, buflen, "%s\\cars\\interiors\\models\\%s\\%s",
			(base_path ? base_path : ""),
			file->vehicle->short_name,
			(include_file ? file->name : ""));
	}

	else
	{
		// Someone passed this function a file that's not part of the mod, return the base path.
		sprintf_s(buffer, buflen, "%s\\", (base_path ? base_path : ""));
	}
}
