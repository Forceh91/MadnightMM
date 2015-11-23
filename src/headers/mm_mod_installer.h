#pragma once
#ifndef _mm_mod_installer_h
#define _mm_mod_installer_h

typedef struct _mm_mod_item mm_mod_item;

typedef struct _mm_installed_file
{
	char *file_name;
	char vehicle_short[4]; // the vehicle this file modifies (short name)
	unsigned char flags; // mod file flags (see ModFileFlags in mm_mod_item.h)
	unsigned char livery; // index of the livery

} mm_installed_file;

typedef struct _mm_installed_mod
{
	char *file_path; // path to the mod (including file name and extension)
	unsigned int file_crc; // CRC32 of the installed archive
	unsigned char file_count; // the number of files installed from the archive
	mm_installed_file *files; // an array listing all the files the mod has modified, length file_count

} mm_installed_mod;

void mm_cleanup_installed_mods(void);
void mm_load_config_file(void);
void mm_save_config_file(void);

bool mm_is_mod_installed(mm_mod_item *mod);
bool mm_install_mod(mm_mod_item *mod);
bool mm_uninstall_mod(mm_mod_item *mod);

bool mm_has_mod_directory();
bool mm_has_game_directory();
bool mm_has_backup_directory();
mm_installed_mod *mm_is_unique_mod(mm_mod_item *checking_mod);

#endif
