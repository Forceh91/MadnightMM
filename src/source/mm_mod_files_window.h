#pragma once

typedef enum _mm_mod_files_list_columns {
	MOD_FILE_LIST_COLUMN_FILE_ENABLED = 0,
	MOD_FILE_LIST_COLUMN_FILE_NAME,
	MOD_FILE_LIST_COLUMN_FILE_LIVERY_SLOT
} mm_mod_files_list_columns;

typedef enum _mm_mod_files_hmenu {
	MOD_FILES_HMENU_INSTALL_BUTTON = 200,
	MOD_FILES_HMENU_CANCEL_BUTTON
} mm_mod_files_hmenu;

bool mm_mod_files_create_window(HINSTANCE hinstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine);
void mm_mod_files_create_listview(void);
void mm_mod_files_show_window(int show);
void mm_mod_files_update_list(mm_mod_item *mm_mod, int listIndex);
void mm_mod_files_clear_list(void);
void mm_mod_files_process_command(HWND hWnd, WPARAM wParam, LPARAM lParam);
