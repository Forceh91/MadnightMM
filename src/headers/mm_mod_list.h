#pragma once
#ifndef _mm_mod_list_h
#define _mm_mod_list_h

#include "mm_mod_item.h"

void mm_initialize_mod_list(HWND mmListview);
void mm_add_mod_item(mm_mod_item* modItem);
void mm_clear_mod_list(void);
void mm_mod_list_handle_item_change(LPNMLISTVIEW lParam);

typedef enum _mod_list_columns {
	MOD_LIST_COLUMN_ENABLED,
	MOD_LIST_COLUMN_NAME,
	MOD_LIST_COLUMN_FILE_SIZE
} mod_list_columns;

void mm_mod_list_install_mod(mm_mod_item *mod_item, int listIndex);
void mm_mod_list_cancel_mod_install(mm_mod_item *mod_item, int listIndex);

#endif
