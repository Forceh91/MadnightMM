#pragma once
#ifndef _mm_controls_h
#define _mm_controls_h

#include "mm_mod_item.h"

typedef enum _MMControls
{
	MM_CONTROL_MOD_LOCATION_LABEL = 100,
	MM_CONTROL_MOD_LOCATION_BROWSE,
	MM_CONTROL_MOD_FILE_LIST,
	MM_CONTROL_GAME_DIR_LOCATION_BTN,
	MM_CONTROL_GAME_DIR_LOCATION_LABEL,
	MM_CONTROL_BACKUP_DIR_LOCATION_BTN,
	MM_CONTROL_BACKUP_DIR_LOCATION_LABEL,
	MM_CONTROL_PROGRESS_HEADER,
	MM_CONTROL_PROGRESS_BACKUP,
	MM_CONTROL_PROGRESS_FILE
} MMControls;

extern bool scanning_mod_archives;

void mm_create_controls(HWND mmWindow, HINSTANCE hInstance);
void mm_control_handler(HWND mmWindow, WPARAM wParam);

static int CALLBACK mm_control_mod_browser_handler(HWND hWnd, UINT message, LPARAM lParam, LPARAM lpData);
static int CALLBACK mm_control_game_location_handler(HWND hWnd, UINT message, LPARAM lParam, LPARAM lpData);
static int CALLBACK mm_control_backup_location_handler(HWND hWnd, UINT message, LPARAM lParam, LPARAM lpData);
void mm_handle_mod_directory_found(HWND hWnd, TCHAR* filePath);
void mm_update_mod_information(mm_mod_item* mod_item);
void mm_show_installation_progress(int fileCount, bool isInstalling = true);
void mm_update_installation_file(const char* filename);
void mm_hide_installation_progress(void);

#endif
