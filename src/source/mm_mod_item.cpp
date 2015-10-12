#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include "mm_mod_item.h"
#include "mm_extractor.h"
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
	if (!mm_extractor_scan(item))
	{
		mm_destroy_mod_item(item);
		return NULL;
	}

	return item;
}

void mm_destroy_mod_item(mm_mod_item *item)
{
	if (item->mod_name != NULL) delete[] item->mod_name;
	if (item->file_path != NULL) delete[] item->file_path;

	for (int i = 0; i < item->item_count; ++i)
	{
		mm_destroy_mod_file(item->files[i]);
	}

	delete[] item->files;
	delete item;
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

	mod_file->name = new char[len + 1];
	strncpy(mod_file->name, name, len);
	mod_file->name[len] = '\0';

	return mod_file;
}

void mm_destroy_mod_file(mm_mod_file *file)
{
	if (file->name != NULL) delete[] file->name;
	if (file->path != NULL) delete[] file->path;

	delete file;
}
