#include <Windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <stdio.h>
#include <tchar.h>

#include "mm_mod_list.h"
#include "mm_controls.h"
#include "mm_mod_archive.h"
#include "mm_mod_installer.h"
#include "mm_mod_files_window.h"

#define MAX_MOD_FILES 1000

HWND mm_mod_list = 0;
mm_mod_item* mm_mod_item_list[MAX_MOD_FILES] = { 0 };
int freeModSlot = 0;

void mm_initialize_mod_list(HWND mmListview)
{
	// store this for future use
	mm_mod_list = mmListview;

	// update the style of the list view
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

void mm_add_mod_item(mm_mod_item* modItem)
{
	mm_mod_item_list[freeModSlot] = modItem;

	// add it to the list
	LVITEM listviewItem = { 0 };
	listviewItem.iItem = freeModSlot;

	ListView_InsertItem(mm_mod_list, &listviewItem);

	// toggle the checkbox if the mod has been installed
	listviewItem = { 0 };
	listviewItem.mask = LVIF_STATE;
	listviewItem.state = modItem->enabled ? 0x2000 : 0x1000; // Cosmic magic constants from hell
	listviewItem.stateMask = 0xF000;
	listviewItem.iItem = freeModSlot;
	listviewItem.iSubItem = MOD_LIST_COLUMN_ENABLED;

	ListView_SetItem(mm_mod_list, &listviewItem);

	// set the name
	listviewItem = { 0 };
	listviewItem.mask = LVIF_TEXT;
	listviewItem.pszText = modItem->mod_name;
	listviewItem.iItem = freeModSlot;
	listviewItem.iSubItem = MOD_LIST_COLUMN_NAME;

	ListView_SetItem(mm_mod_list, &listviewItem);

	// set the size
	listviewItem = { 0 };
	listviewItem.mask = LVIF_TEXT;
	
	char fileSize[128] = { 0 };
	sprintf(fileSize, "%0.2fMB", ((float)modItem->file_size / 1000000));
	listviewItem.pszText = fileSize;

	listviewItem.iItem = freeModSlot;
	listviewItem.iSubItem = MOD_LIST_COLUMN_FILE_SIZE;

	ListView_SetItem(mm_mod_list, &listviewItem);

	// update the free mod slot
	freeModSlot++;
}

void mm_mod_list_handle_item_change(LPNMLISTVIEW lParam)
{
	NMHDR* hdr = &lParam->hdr;
	if (lParam->uChanged & LVIF_STATE)
	{
		// find if the checkbox is checked or not
		int isChecked = ListView_GetCheckState(hdr->hwndFrom, lParam->iItem);

		// make sure we haven't selected a listview item that is out of bounds of what mods we know of
		if (lParam->iItem >= MAX_MOD_FILES)
			return;

		// find the mod item and tell it that enabled is true
		mm_mod_item* mmModItem = mm_mod_item_list[lParam->iItem];
		if (!mmModItem)
			return;

		// if it exists then we can toggle the enabled value of it
		bool modEnabled = (isChecked != 0);

		// The checkbox state has changed, install or uninstall the mod.
		if (modEnabled != mmModItem->enabled && !scanning_mod_archives)
		{
			if (modEnabled)
			{
				// create the window showing our mods files
				mm_mod_files_update_list(mmModItem, lParam->iItem);
				mm_show_mod_install_files();
			}
			else
			{
				char errorMessage[MAX_PATH] = { 0 };

				// check the game directory exists
				if (!mm_has_game_directory())
				{
					// throw an error
					MessageBox(mm_mod_list, _TEXT("Unable to uninstall mod.\nPlease make sure that your game directory has been set."), _TEXT("An error occured"), MB_OK | MB_ICONERROR);

					// check the checkbox again
					ListView_SetCheckState(hdr->hwndFrom, lParam->iItem, !modEnabled);

					return;
				}

				// check the backup directory exists
				if (!mm_has_backup_directory())
				{
					// throw an error
					MessageBox(mm_mod_list, _TEXT("Unable to uninstsall mod.\nPlease make sure that your backup directory has been set."), "An error occured", MB_OK | MB_ICONERROR);

					// check the checkbox again
					ListView_SetCheckState(hdr->hwndFrom, lParam->iItem, !modEnabled);

					return;
				}

				mm_uninstall_mod(mmModItem);
			}
		}

		// check for selected item
		int index = ListView_GetNextItem(hdr->hwndFrom, -1, LVNI_SELECTED);
		if (index == -1)
		{
			mm_update_mod_information(NULL);
			return;
		}

		// find our mod item instance of it
		mm_mod_item* mmSelectedModItem = mm_mod_item_list[index];
		if (!mmSelectedModItem)
			return;

		// update the mod info
		mm_update_mod_information(mmSelectedModItem);
	}
}

void mm_mod_list_install_mod(mm_mod_item *mod_item, int listIndex)
{
	// alright hide the mod file list window thing
	mm_hide_mod_install_files();

	// go through and check for errors
	char errorMessage[MAX_PATH] = { 0 };

	// check the game directory exists
	if (!mm_has_game_directory())
	{
		// throw an error
		MessageBox(mm_mod_list, _TEXT("Unable to install mod.\nPlease make sure that your game directory has been set."), _TEXT("An error occured"), MB_OK | MB_ICONERROR);

		// check the checkbox again
		ListView_SetCheckState(mm_mod_list , listIndex, false);

		return;
	}

	// check the backup directory exists
	if (!mm_has_backup_directory())
	{
		// throw an error
		MessageBox(mm_mod_list, _TEXT("Unable to install mod.\nPlease make sure that your backup directory has been set."), "An error occured", MB_OK | MB_ICONERROR);

		// check the checkbox again
		ListView_SetCheckState(mm_mod_list, listIndex, false);

		return;
	}

	// check that we're not conflicting if we're installing
	mm_installed_mod *conflicting_mod = 0;
	if ((conflicting_mod = mm_is_unique_mod(mod_item)) != NULL)
	{
		// throw an error
		sprintf(errorMessage, "Unable to install %s.\n\nThis mod has files that conflict with:\n%s.\n\nPlease uninstall that mod before installing this one.", mod_item->mod_name, conflicting_mod->file_path);
		MessageBox(mm_mod_list, _TEXT(errorMessage), "Mod conflict", MB_OK | MB_ICONERROR);

		// check the checkbox again
		ListView_SetCheckState(mm_mod_list, listIndex, false);

		return;
	}

	mm_install_mod(mod_item);
}

void mm_mod_list_cancel_mod_install(mm_mod_item *mod_item, int listIndex)
{
	// alright hide the mod file list window thing
	mm_hide_mod_install_files();

	// update the checkbox state so that it goes back to the mod enabled state
	ListView_SetCheckState(mm_mod_list, listIndex, mod_item->enabled);
}
