#include "mm_mod_installer.h"
#include "mm_mod_item.h"
#include "mm_utils.h"
#include "mm_mod_archive.h"
#include "mm_controls.h"
#include <stdio.h>
#include <string.h>
#include <tchar.h>
#include <Windows.h>

// Probably safe to assume no one will manage to install over 1000 mods at once. Probably.
#define MAX_INSTALLED_MODS 1000

// In case we want to change the contents in the future.
#define FILE_VERSION 1

static mm_installed_mod *installed_mods[MAX_INSTALLED_MODS];
static unsigned int num_installed_mods;

char modPath[MAX_PATH] = { 0 };
char gamePath[MAX_PATH] = { 0 };
char backupPath[MAX_PATH] = { 0 };

static char config_file_path[MAX_PATH] = { 0 };

extern TCHAR mm_app_data_loc[MAX_PATH];

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

static bool mm_cleanup_installed_mod(unsigned int index)
{
	mm_installed_mod *mod = installed_mods[index];

	if (mod == NULL)
		return false;

	for (unsigned int f = 0; f < mod->file_count; ++f)
	{
		if (mod->files[f].file_name != NULL) delete[] mod->files[f].file_name;
	}

	if (mod->file_path != NULL) delete[] mod->file_path;
	if (mod->files != NULL) delete[] mod->files;
	delete mod;

	installed_mods[index] = NULL;
	
	return true;
}

void mm_cleanup_installed_mods(void)
{
	for (unsigned int i = 0, j = 0; i < MAX_INSTALLED_MODS && j < num_installed_mods; ++i)
	{
		if (mm_cleanup_installed_mod(i))
			++j;
	}

	num_installed_mods = 0;
}

void mm_load_config_file(void)
{
	if (!*config_file_path) {
		_tcscat(config_file_path, mm_app_data_loc);
		_tcscat(config_file_path, _TEXT("\\config.dat"));
	}
	
	FILE *file = fopen(config_file_path, "r");

	// No mods have been installed previously.
	if (file == NULL)
		return;

	// Read the file version. Currently unused.
	unsigned int version = mm_read_uint(file);

	// Read directories.
	mm_read_string(file, modPath);
	mm_read_string(file, gamePath);
	mm_read_string(file, backupPath);

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
			char file_name[MAX_PATH];
			mm_read_string(file, file_name);
			mod->files[f].file_name = mm_str_duplicate(file_name);

			mm_read_string(file, mod->files[f].vehicle_short);
			mod->files[f].flags = mm_read_uchar(file);
			mod->files[f].livery = mm_read_uchar(file);
		}

		installed_mods[num_installed_mods++] = mod;
	}

	fclose(file);
}

void mm_save_config_file(void)
{
	if (!*config_file_path) {
		_tcscat(config_file_path, mm_app_data_loc);
		_tcscat(config_file_path, _TEXT("\\config.dat"));
	}

	FILE *file = fopen(config_file_path, "w");

	// No write access or some other bad stuff.
	if (file == NULL)
		return;

	// Write the file version in case we want to upgrade the file format in the future.
	mm_write_uint(file, FILE_VERSION);

	// Write the folders used for installing/backing up mods.
	mm_write_string(file, modPath);
	mm_write_string(file, gamePath);
	mm_write_string(file, backupPath);

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
			mm_write_string(file, mod->files[f].file_name);
			mm_write_string(file, mod->files[f].vehicle_short);
			mm_write_uchar(file, mod->files[f].flags);
			mm_write_uchar(file, mod->files[f].livery);
		}

		++j;
	}

	fclose(file);
}

static mm_installed_mod *mm_find_installed_mod(mm_mod_item *mod, unsigned int *index = NULL)
{
	for (unsigned int i = 0, j = 0; j < num_installed_mods; ++i)
	{
		if (installed_mods[i] == NULL)
			continue;

		mm_installed_mod *installed_mod = installed_mods[i];

		if (_stricmp(mod->mod_name, installed_mod->file_path) == 0 &&
			mod->file_crc == installed_mod->file_crc)
		{
			if (index != NULL)
				*index = i;

			return installed_mod;
		}

		++j;
	}

	if (index != NULL)
		*index = MAX_INSTALLED_MODS;

	return NULL;
}

bool mm_is_mod_installed(mm_mod_item *mod)
{
	return (mm_find_installed_mod(mod) != NULL);
}

static unsigned int mm_find_free_mod_index(void)
{
	for (unsigned int i = 0; i < MAX_INSTALLED_MODS; ++i)
	{
		if (installed_mods[i] == NULL)
			return i;
	}

	return MAX_INSTALLED_MODS;
}

static void __stdcall mm_backup_mod_file(mm_mod_item *mod, mm_mod_file *file)
{
	// Figure out where this particular file should go.
	char filePath[MAX_PATH];
	mm_get_mod_file_path(file, filePath, sizeof(filePath), backupPath);

	// show the file we're on
	mm_update_installation_file(file->name);

	// If the file has been backed up already, don't do anything.
	if (mm_file_exists(filePath))
		return;

	// Make sure the entire backup path exists.
	char backupFileLocation[MAX_PATH];
	mm_get_mod_file_path(file, backupFileLocation, sizeof(backupFileLocation), backupPath, false);

	mm_ensure_folder_exists(backupFileLocation);

	// Copy the file from the game folder.
	char originalFile[MAX_PATH];
	mm_get_mod_file_path(file, originalFile, sizeof(originalFile), gamePath);

	CopyFile(originalFile, filePath, TRUE);
}

bool mm_install_mod(mm_mod_item *mod)
{
	// Make sure we're not overwriting our installed mod list. 1000 _should_ be safe, but you never know...
	unsigned int mod_index = mm_find_free_mod_index();

	if (mod_index == MAX_INSTALLED_MODS)
		return false;

	// Open the mod archive and extract it to the game folder. Our callback function will take care of backing up original files.
	ModArchive *archive = new ModArchive(mod);

	bool result = archive->Extract(gamePath, mm_backup_mod_file);
	archive->Release();

	if (result)
	{
		// Mod was installed successfully, add it to the installed mod list.
		mm_installed_mod *inst_mod = new mm_installed_mod;

		inst_mod->file_path = mm_str_duplicate(mod->mod_name);
		inst_mod->file_crc = mod->file_crc;
		inst_mod->file_count = mod->install_file_count;

		inst_mod->files = new mm_installed_file[mod->install_file_count];

		for (unsigned int i = 0, j = 0; i < mod->item_count && j < mod->file_count; ++i)
		{
			if ((mod->files[i]->flags & FFLAG_MOD_FILE) == 0)
				continue;

			if ((mod->files[i]->flags & FFLAG_INSTALL) == 0)
				continue;

			inst_mod->files[j].file_name = mm_str_duplicate(mod->files[i]->name);

			mm_str_cpy(inst_mod->files[j].vehicle_short, mod->files[i]->vehicle->short_name, sizeof(inst_mod->files[i].vehicle_short));
			inst_mod->files[j].flags = mod->files[i]->flags;
			inst_mod->files[j].livery = mod->files[i]->livery;

			++j;
		}

		installed_mods[mod_index] = inst_mod;
		num_installed_mods++;

		// Save the installed mod list.
		mm_save_config_file();

		mod->enabled = true;
	}

	return result;
}

bool mm_uninstall_mod(mm_mod_item *mod)
{
	unsigned int mod_index;
	mm_installed_mod *installed = mm_find_installed_mod(mod, &mod_index);

	if (installed == NULL)
	{
		// Can't find install data for this mod, so we can't uninstall it either!
		return false;
	}

	// toggle the progress bar
	mm_show_installation_progress(installed->file_count, false);

	for (unsigned int i = 0; i < installed->file_count; ++i)
	{
		mm_installed_file &mod_file = installed->files[i];

		// Figure out where the backed up file would be.
		char filePath[MAX_PATH];
		mm_get_mod_file_path(mod_file.file_name, mod_file.vehicle_short, mod_file.flags, mod_file.livery, filePath, sizeof(filePath), backupPath, true);

		// If the backed up file doesn't exist for some reason (user has been messing with our backups?), don't do anything. (or maybe show the user a warning?)
		if (!mm_file_exists(filePath))
			continue;

		// The restore path should exists as we're restoring the file into the game folder. Format the full file path for restoration.
		char restorePath[MAX_PATH];
		mm_get_mod_file_path(mod_file.file_name, mod_file.vehicle_short, mod_file.flags, mod_file.livery, restorePath, sizeof(restorePath), gamePath, true);

		// Copy the file back into the game folder.
		CopyFile(filePath, restorePath, FALSE);

		// show the file we're on
		mm_update_installation_file(mod_file.file_name);
	}

	mm_cleanup_installed_mod(mod_index);
	num_installed_mods--;

	mod->enabled = false;

	// Save the installed mod list.
	mm_save_config_file();

	return true;
}

bool mm_has_mod_directory()
{
	return (*modPath != 0);
}

bool mm_has_game_directory()
{
	return (*gamePath != 0);
}

bool mm_has_backup_directory()
{
	return (*backupPath != 0);
}

mm_installed_mod *mm_is_unique_mod(mm_mod_item *checking_mod)
{
	for (unsigned int i = 0, j = 0; i < MAX_INSTALLED_MODS && j < num_installed_mods; ++i)
	{
		if (installed_mods[i] == NULL)
			continue;

		mm_installed_mod *mod = installed_mods[i];

		// check through the files of the installed mod
		for (unsigned int f = 0; f < mod->file_count; ++f)
		{
			// comparing each one to whats installed in this mod
			for (unsigned int g = 0; g < checking_mod->file_count; ++g)
			{
				if (strcmp(checking_mod->files[g]->name, mod->files[f].file_name))
					continue;

				return mod;
			}
		}

		++j;
	}

	return NULL;
}
