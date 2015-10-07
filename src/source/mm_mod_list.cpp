#include <Windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <stdio.h>

#include "mm_mod_item.h"
#include "mm_mod_list.h"

HWND mm_mod_list = 0;
mm_mod_item mm_mod_item_list[1000] = { 0 };
int freeModSlot = 0;

void mm_initialize_mod_list(HWND mmListview)
{
	mm_mod_list = mmListview;

	ListView_SetExtendedListViewStyle(mm_mod_list, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES | LVS_EX_AUTOSIZECOLUMNS);

	// dodgy work around for column 0 always overriding the format
	LVCOLUMN lvColumn = { 0 };
	lvColumn.iSubItem = 0;
	//

	SendMessage(mm_mod_list, LVM_INSERTCOLUMN, 0, (LPARAM)&lvColumn);

	// is this enabled?
	lvColumn = { 0 };
	lvColumn.mask = LVCF_WIDTH | LVCF_SUBITEM | LVCF_FMT;
	lvColumn.cx = 20;
	lvColumn.iSubItem = MOD_LIST_COLUMN_ENABLED;
	lvColumn.fmt = LVCFMT_FIXED_WIDTH | LVCFMT_NO_TITLE | HDF_CHECKBOX;

	SendMessage(mm_mod_list, LVM_INSERTCOLUMN, MOD_LIST_COLUMN_ENABLED + 1, (LPARAM)&lvColumn);

	// what is the name of this mod?
	lvColumn = { 0 };
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.pszText = "Name";
	lvColumn.cx = 500;
	lvColumn.fmt = LVCFMT_LEFT | LVCFMT_FIXED_WIDTH;
	lvColumn.iSubItem = MOD_LIST_COLUMN_NAME;

	SendMessage(mm_mod_list, LVM_INSERTCOLUMN, MOD_LIST_COLUMN_NAME + 1, (LPARAM)&lvColumn);

	// how big is it?
	lvColumn = { 0 };
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.pszText = "File Size";
	lvColumn.cx = 75;
	lvColumn.fmt = LVCFMT_RIGHT | LVCFMT_FIXED_WIDTH;
	lvColumn.iSubItem = MOD_LIST_COLUMN_FILE_SIZE;

	SendMessage(mm_mod_list, LVM_INSERTCOLUMN, MOD_LIST_COLUMN_FILE_SIZE + 1, (LPARAM)&lvColumn);

	// dodgy work around for column 0 always overriding the format
	lvColumn = { 0 };
	SendMessage(mm_mod_list, LVM_DELETECOLUMN, 0, (LPARAM)&lvColumn);
	//
}

void mm_clear_mod_list(void)
{
	if (freeModSlot == 0)
		return;

	ListView_DeleteAllItems(mm_mod_list);
	freeModSlot = 0;
}

void mm_add_mod_item(mm_mod_item& modItem)
{
	mm_mod_item_list[freeModSlot] = modItem;

	// add it to the list
	LVITEM listviewItem = { 0 };
	listviewItem.iItem = freeModSlot;

	ListView_InsertItem(mm_mod_list, &listviewItem);

	// set the name
	listviewItem = { 0 };
	listviewItem.mask = LVIF_TEXT;
	listviewItem.pszText = modItem.mod_name;
	listviewItem.iItem = freeModSlot;
	listviewItem.iSubItem = MOD_LIST_COLUMN_NAME;

	ListView_SetItem(mm_mod_list, &listviewItem);

	// set the size
	listviewItem = { 0 };
	listviewItem.mask = LVIF_TEXT;
	
	char fileSize[128] = { 0 };
	sprintf(fileSize, "%0.2fMB", ((float)modItem.fileSize / 1000000));
	listviewItem.pszText = fileSize;

	listviewItem.iItem = freeModSlot;
	listviewItem.iSubItem = MOD_LIST_COLUMN_FILE_SIZE;

	ListView_SetItem(mm_mod_list, &listviewItem);

	// update the free mod slot
	freeModSlot++;
}
