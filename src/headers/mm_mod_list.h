#pragma once
#ifndef _mm_mod_list_h
#define _mm_mod_list_h

void mm_initialize_mod_list(HWND mmListview);
void mm_add_mod_item(mm_mod_item& modItem);
void mm_clear_mod_list(void);

typedef enum _mod_list_columns {
	MOD_LIST_COLUMN_ENABLED,
	MOD_LIST_COLUMN_NAME,
	MOD_LIST_COLUMN_FILE_SIZE
} mod_list_columns;

#endif
