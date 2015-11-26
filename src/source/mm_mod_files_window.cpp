#include <Windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <CommCtrl.h>
#include <stdio.h>

#include "mm_controls.h"
#include "mm_mod_item.h"
#include "mm_mod_files_window.h"
#include "mm_mod_list.h"
#include "mm_utils.h"

HWND mm_mod_files_window = { 0 };
HWND mm_mod_files_list = { 0 };
HWND mm_mod_files_install_button = { 0 };
HWND mm_mod_files_cancel_button = { 0 };
HWND mm_mod_files_combobox = { 0 };

mm_mod_item *mm_mod_to_install = 0;
int mm_mod_list_index = -1;
int mm_mod_list_editing_index = -1;

LRESULT CALLBACK mm_mod_wnd_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	NMHDR* hdr = &LPNMLISTVIEW(lParam)->hdr;

	switch (msg)
	{
		case WM_COMMAND:
		{
			mm_mod_files_process_command(hWnd, wParam, lParam);
		}
		break;
		case WM_NOTIFY:
			switch (((LPNMHDR)lParam)->code)
			{			
				case LVN_ITEMCHANGED:
					if (LPNMLISTVIEW(lParam)->uChanged)
					{
						// get the selected index
						mm_mod_list_editing_index = ListView_GetNextItem(hdr->hwndFrom, -1, LVNI_SELECTED);
						if (mm_mod_list_editing_index == -1)
							return 0;

						// reset the selected index
						ComboBox_SetCurSel(mm_mod_files_combobox, 0);

						// get the rectangle for the sub item
						RECT rect = { 0 };
						ListView_GetSubItemRect(hdr->hwndFrom, mm_mod_list_editing_index, MOD_FILE_LIST_COLUMN_FILE_LIVERY_SLOT, LVIR_BOUNDS, &rect);

						// set the pos of the combobox
						SetWindowPos(mm_mod_files_combobox, 0, rect.left + 10, rect.top + 10, (rect.right - rect.left), 15, SWP_SHOWWINDOW);
						
					}
				break;
			}
			break;
		case WM_CLOSE:
		{
			mm_mod_list_cancel_mod_install(mm_mod_to_install, mm_mod_list_index);
		}
		break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}

bool mm_mod_files_create_window(HINSTANCE hinstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine)
{
	WNDCLASSEX wc;
	INITCOMMONCONTROLSEX icex;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = mm_mod_wnd_proc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hinstance;
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wc.lpszMenuName = 0;
	wc.lpszClassName = _TEXT("MadnightMMModFiles");
	wc.hIcon = 0;
	wc.hIconSm = 0;

	if (!RegisterClassEx(&wc))
		return false;

	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_LISTVIEW_CLASSES;

	if (!InitCommonControlsEx(&icex))
		return false;

	// try to create the window
	mm_mod_files_window = CreateWindowEx(WS_EX_CLIENTEDGE, _TEXT("MadnightMMModFiles"), _TEXT("Files included in this mod..."), WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 500, 340, 0, 0, hinstance, 0);
	if (!mm_mod_files_window)
		return false;

	// install mod button
	mm_mod_files_install_button = CreateWindowEx(0, WC_BUTTON, _TEXT("Install mod"), BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE, 10, 255, 80, 25, mm_mod_files_window, (HMENU)MOD_FILES_HMENU_INSTALL_BUTTON, hinstance, 0);
	SendMessage(mm_mod_files_install_button, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), 0);

	// install mod button
	mm_mod_files_cancel_button = CreateWindowEx(0, WC_BUTTON, _TEXT("Cancel"), BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE, 100, 255, 80, 25, mm_mod_files_window, (HMENU)MOD_FILES_HMENU_CANCEL_BUTTON, hinstance, 0);
	SendMessage(mm_mod_files_cancel_button, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), 0);

	// combobox for livery slots
	mm_mod_files_combobox = CreateWindowEx(0, WC_COMBOBOX, _TEXT(""), CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_VISIBLE | SWP_NOZORDER, 0, 0, 0, 0, mm_mod_files_window, 0, hinstance, 0);
	SendMessage(mm_mod_files_combobox, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), 0);

	// create the list view
	mm_mod_files_create_listview();

	return true;
}

void mm_mod_files_show_window(int show)
{
	ShowWindow(mm_mod_files_window, show);
	UpdateWindow(mm_mod_files_window);
}

void mm_mod_files_create_listview(void)
{
	mm_mod_files_list = CreateWindowEx(0, WC_LISTVIEW, _TEXT(""), WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL | LVS_REPORT | LVS_SHOWSELALWAYS, 10, 10, 460, 230, mm_mod_files_window, 0, 0, 0);
	SendMessage(mm_mod_files_list, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), 0);
	
	// update the style of the list view
	ListView_SetExtendedListViewStyle(mm_mod_files_list, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES | LVS_EX_AUTOSIZECOLUMNS);

	// dodgy work around for column 0 always overriding the format
	LVCOLUMN lvColumn = { 0 };
	lvColumn.iSubItem = 0;
	//

	SendMessage(mm_mod_files_list, LVM_INSERTCOLUMN, 0, (LPARAM)&lvColumn);

	// is this enabled?
	lvColumn = { 0 };
	lvColumn.mask = LVCF_WIDTH | LVCF_SUBITEM | LVCF_FMT;
	lvColumn.cx = 20;
	lvColumn.iSubItem = MOD_FILE_LIST_COLUMN_FILE_ENABLED;
	lvColumn.fmt = LVCFMT_FIXED_WIDTH | LVCFMT_NO_TITLE | HDF_CHECKBOX;

	SendMessage(mm_mod_files_list, LVM_INSERTCOLUMN, MOD_FILE_LIST_COLUMN_FILE_ENABLED + 1, (LPARAM)&lvColumn);

	// what is the name of this file?
	lvColumn = { 0 };
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.pszText = "File name";
	lvColumn.cx = 300;
	lvColumn.fmt = LVCFMT_LEFT | LVCFMT_FIXED_WIDTH;
	lvColumn.iSubItem = MOD_FILE_LIST_COLUMN_FILE_NAME;

	SendMessage(mm_mod_files_list, LVM_INSERTCOLUMN, MOD_FILE_LIST_COLUMN_FILE_NAME + 1, (LPARAM)&lvColumn);

	// what is the livery slot of this file?
	lvColumn = { 0 };
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.pszText = "Select livery slot";
	lvColumn.cx = 125;
	lvColumn.fmt = LVCFMT_FIXED_WIDTH;
	lvColumn.iSubItem = MOD_FILE_LIST_COLUMN_FILE_LIVERY_SLOT;

	SendMessage(mm_mod_files_list, LVM_INSERTCOLUMN, MOD_FILE_LIST_COLUMN_FILE_LIVERY_SLOT + 1, (LPARAM)&lvColumn);

	// dodgy work around for column 0 always overriding the format
	lvColumn = { 0 };
	SendMessage(mm_mod_files_list, LVM_DELETECOLUMN, 0, (LPARAM)&lvColumn);
	//
}

void mm_mod_files_clear_list(void)
{
	mm_mod_to_install = 0;
	mm_mod_list_index = -1;

	ListView_DeleteAllItems(mm_mod_files_list);
}

void mm_mod_files_update_list(mm_mod_item *mm_mod, int listIndex)
{
	// clear the list first
	mm_mod_files_clear_list();

	// store these for later use
	mm_mod_to_install = mm_mod;
	mm_mod_list_index = listIndex;

	// add the livery indexes into the combobox
	mm_vehicle_data *vehicle_data = mm_mod->vehicle;
	if (vehicle_data == 0)
		return;

	// clear combobox data
	ComboBox_ResetContent(mm_mod_files_combobox);

	// update the livery list
	char comboboxItem[64] = { 0 };
	for (int k = 0; k < vehicle_data->livery_count; k++)
	{
		if (vehicle_data->liveries[k] == 0)
			continue;

		sprintf(comboboxItem, "Livery %d (%d)", k, vehicle_data->liveries[k]);
		ComboBox_AddString(mm_mod_files_combobox, (LPARAM)comboboxItem);
	}

	// focus on the first input (or at some point the already selected livery slot)
	ComboBox_SetCurSel(mm_mod_files_combobox, 0);

	// go through each file in the mod
	for (int i = 0, j = 0; i < mm_mod->item_count && j < mm_mod->file_count; i++)
	{
		mm_mod_file *mod_file = mm_mod->files[i];
		if (mod_file == NULL)
			continue;

		// ignore directory
		if ((mod_file->flags & FFLAG_MOD_FILE) == 0)
			continue;

		// add it to the list
		LVITEM listviewItem = { 0 };
		listviewItem.iItem = j;

		ListView_InsertItem(mm_mod_files_list, &listviewItem);

		// toggle the checkbox if the mod has been installed
		listviewItem = { 0 };
		listviewItem.mask = LVIF_STATE;
		listviewItem.state = 0x2000; //modItem->enabled ? 0x2000 : 0x1000; // Cosmic magic constants from hell
		listviewItem.stateMask = 0xF000;
		listviewItem.iItem = j;
		listviewItem.iSubItem = MOD_FILE_LIST_COLUMN_FILE_ENABLED;

		ListView_SetItem(mm_mod_files_list, &listviewItem);

		// set the name
		listviewItem = { 0 };
		listviewItem.mask = LVIF_TEXT;
		listviewItem.pszText = mod_file->name;
		listviewItem.iItem = j;
		listviewItem.iSubItem = MOD_FILE_LIST_COLUMN_FILE_NAME;

		ListView_SetItem(mm_mod_files_list, &listviewItem);

		// set the type
		listviewItem = { 0 };
		listviewItem.mask = LVIF_TEXT;

		if ((mod_file->flags & FFLAG_TEXTURE_LIVERY) != 0)
			listviewItem.pszText = "Livery x";
		else
			listviewItem.pszText = "Not used";

		listviewItem.iItem = j;
		listviewItem.iSubItem = MOD_FILE_LIST_COLUMN_FILE_LIVERY_SLOT;

		ListView_SetItem(mm_mod_files_list, &listviewItem);

		j++;
	}
}

void mm_mod_files_process_command(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
		case MOD_FILES_HMENU_INSTALL_BUTTON:
			mm_mod_list_install_mod(mm_mod_to_install, mm_mod_list_index);
		break;
		case MOD_FILES_HMENU_CANCEL_BUTTON:
			mm_mod_list_cancel_mod_install(mm_mod_to_install, mm_mod_list_index);
		break;
	}

	char liveryString[64] = { 0 };
	LVITEM listviewItem = { 0 };
	int selectedIndex = -1;

	switch (HIWORD(wParam))
	{
		case CBN_SELCHANGE:
			selectedIndex = ComboBox_GetCurSel(mm_mod_files_combobox);
			if (selectedIndex == -1)
				return;

			listviewItem.mask = LVIF_TEXT;

			sprintf(liveryString, "Livery %d", (selectedIndex + 1));

			listviewItem.iItem = mm_mod_list_editing_index;
			listviewItem.mask = LVIF_TEXT;
			listviewItem.iSubItem = MOD_FILE_LIST_COLUMN_FILE_LIVERY_SLOT;
			listviewItem.pszText = liveryString;

			SendMessage(mm_mod_files_list, LVM_SETITEMTEXT, mm_mod_list_editing_index, (WPARAM)&listviewItem);
			SetWindowPos(mm_mod_files_combobox, 0, 0, 0, 0, 0, 0);
		break;
	}
}
