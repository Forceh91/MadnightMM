#include <Windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <CommCtrl.h>
#include <ShlObj.h>
#include <stdio.h>

#include "mm_mod_list.h"
#include "mm_controls.h"
#include "mm_utils.h"
#include "mm_mod_installer.h"

extern char modPath[MAX_PATH];
extern char gamePath[MAX_PATH];
extern char backupPath[MAX_PATH];

HWND mm_mod_location_group = { 0 };
HWND mm_mod_location_label = { 0 };
HWND mm_mod_location_browse_button = { 0 };
HWND mm_mod_file_list = { 0 };

HWND mm_mod_info_group = { 0 };
HWND mm_mod_info_name_header = { 0 };
HWND mm_mod_info_name_data = { 0 };
HWND mm_mod_info_size_header = { 0 };
HWND mm_mod_info_size_data = { 0 };
HWND mm_mod_info_vehicle_header = { 0 };
HWND mm_mod_info_vehicle_data = { 0 };
HWND mm_mod_info_enabled_header = { 0 };
HWND mm_mod_info_enabled_data = { 0 };
HWND mm_mod_info_file_count_header = { 0 };
HWND mm_mod_info_file_count_data = { 0 };
HWND mm_mod_info_file_list_header = { 0 };
HWND mm_mod_info_file_list_data = { 0 };
HWND mm_mod_info_install_window = { 0 };
HWND mm_mod_info_install_header = { 0 };
HWND mm_mod_info_install_progress = { 0 };
HWND mm_mod_info_install_file = { 0 };

HWND mm_directory_info_group = { 0 };
HWND mm_game_location_label = { 0 };
HWND mm_game_location_browse_button = { 0 };
HWND mm_game_location_browse_label = { 0 };
HWND mm_backup_location_label = { 0 };
HWND mm_backup_location_browse_button = { 0 };
HWND mm_backup_location_browse_label = { 0 };

HWND mm_main_window = { 0 };

bool scanning_mod_archives = false;

void mm_create_controls(HWND mmWindow, HINSTANCE hInstance)
{
	// store the main window
	mm_main_window = mmWindow;

	// mod group
	mm_mod_location_group = CreateWindowEx(0, WC_BUTTON, _TEXT("Mod Listings"), BS_GROUPBOX | WS_GROUP | WS_CHILD | WS_VISIBLE, 10, 10, 750, 430, mmWindow, 0, hInstance, 0);
	SendMessage(mm_mod_location_group, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), 0);

	mm_mod_location_browse_button = CreateWindowEx(0, WC_BUTTON, _TEXT("Browse..."), BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE, 20, 30, 80, 25, mmWindow, (HMENU)MM_CONTROL_MOD_LOCATION_BROWSE, hInstance, 0);
	SendMessage(mm_mod_location_browse_button, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), 0);

	mm_mod_location_label = CreateWindowEx(0, WC_EDIT, _TEXT(mm_has_mod_directory() ? modPath : "Please select the directory that your mods are stored in..."), WS_CHILD | WS_VISIBLE | SS_LEFT | ES_READONLY, 110, 35, 640, 25, mmWindow, (HMENU)MM_CONTROL_MOD_LOCATION_LABEL, hInstance, 0);
	SendMessage(mm_mod_location_label, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), 0);

	mm_mod_file_list = CreateWindowEx(0, WC_LISTVIEW, _TEXT(""), WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL | LVS_REPORT | LVS_SHOWSELALWAYS, 20, 65, 730, 365, mmWindow, (HMENU)MM_CONTROL_MOD_FILE_LIST, hInstance, 0);
	SendMessage(mm_mod_file_list, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), 0);

	// set up the list view with everything we need
	mm_initialize_mod_list(mm_mod_file_list);
	
	// mod info group
	mm_mod_info_group = CreateWindowEx(0, WC_BUTTON, _TEXT("Mod Information"), BS_GROUPBOX | WS_GROUP | WS_CHILD | WS_VISIBLE, 10, 450, 750, 150, mmWindow, 0, hInstance, 0);
	SendMessage(mm_mod_info_group, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), 0);

	// name info
	mm_mod_info_name_header = CreateWindowEx(0, WC_STATIC, _TEXT("Name:"), WS_CHILD | WS_VISIBLE | SS_LEFT, 20, 470, 50, 25, mmWindow, 0, hInstance, 0);
	SendMessage(mm_mod_info_name_header, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), 0);

	mm_mod_info_name_data = CreateWindowEx(0, WC_STATIC, _TEXT("-"), WS_CHILD | WS_VISIBLE | SS_LEFT, 80, 470, 350, 25, mmWindow, 0, hInstance, 0);
	SendMessage(mm_mod_info_name_data, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), 0);

	// size info
	mm_mod_info_size_header = CreateWindowEx(0, WC_STATIC, _TEXT("Size:"), WS_CHILD | WS_VISIBLE | SS_LEFT, 20, 490, 50, 25, mmWindow, 0, hInstance, 0);
	SendMessage(mm_mod_info_size_header, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), 0);

	mm_mod_info_size_data = CreateWindowEx(0, WC_STATIC, _TEXT("-"), WS_CHILD | WS_VISIBLE | SS_LEFT, 80, 490, 350, 25, mmWindow, 0, hInstance, 0);
	SendMessage(mm_mod_info_size_data, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), 0);

	// file count info
	mm_mod_info_file_count_header = CreateWindowEx(0, WC_STATIC, _TEXT("File Count:"), WS_CHILD | WS_VISIBLE | SS_LEFT, 20, 510, 50, 25, mmWindow, 0, hInstance, 0);
	SendMessage(mm_mod_info_file_count_header, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), 0);

	mm_mod_info_file_count_data = CreateWindowEx(0, WC_STATIC, _TEXT("-"), WS_CHILD | WS_VISIBLE | SS_LEFT, 80, 510, 50, 25, mmWindow, 0, hInstance, 0);
	SendMessage(mm_mod_info_file_count_data, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), 0);

	// vehicle info
	mm_mod_info_vehicle_header = CreateWindowEx(0, WC_STATIC, _TEXT("Vehicle:"), WS_CHILD | WS_VISIBLE | SS_LEFT, 20, 530, 50, 25, mmWindow, 0, hInstance, 0);
	SendMessage(mm_mod_info_vehicle_header, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), 0);

	mm_mod_info_vehicle_data = CreateWindowEx(0, WC_STATIC, _TEXT("-"), WS_CHILD | WS_VISIBLE | SS_LEFT, 80, 530, 350, 25, mmWindow, 0, hInstance, 0);
	SendMessage(mm_mod_info_vehicle_data, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), 0);

	// enabled info
	mm_mod_info_enabled_header = CreateWindowEx(0, WC_STATIC, _TEXT("Enabled:"), WS_CHILD | WS_VISIBLE | SS_LEFT, 20, 550, 50, 25, mmWindow, 0, hInstance, 0);
	SendMessage(mm_mod_info_enabled_header, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), 0);

	mm_mod_info_enabled_data = CreateWindowEx(0, WC_STATIC, _TEXT("-"), WS_CHILD | WS_VISIBLE | SS_LEFT, 80, 550, 350, 25, mmWindow, 0, hInstance, 0);
	SendMessage(mm_mod_info_enabled_data, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), 0);

	// file list info
	mm_mod_info_file_list_header = CreateWindowEx(0, WC_STATIC, _TEXT("Mod files"), WS_CHILD | WS_VISIBLE | SS_LEFT, 440, 470, 50, 25, mmWindow, 0, hInstance, 0);
	SendMessage(mm_mod_info_file_list_header, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), 0);

	mm_mod_info_file_list_data = CreateWindowEx(0, WC_LISTBOX, 0, WS_VISIBLE | WS_CHILD | ES_AUTOVSCROLL | LBS_STANDARD | LBS_NOSEL, 440, 490, 300, 110, mmWindow, 0, hInstance, 0);
	SendMessage(mm_mod_info_file_list_data, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), 0);

	// directory info
	mm_directory_info_group = CreateWindowEx(0, WC_BUTTON, _TEXT("Directory Information"), BS_GROUPBOX | WS_GROUP | WS_CHILD | WS_VISIBLE, 10, 610, 750, 155, mmWindow, 0, hInstance, 0);
	SendMessage(mm_directory_info_group, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), 0);

	// game location header
	mm_game_location_label = CreateWindowEx(0, WC_EDIT, _TEXT("Game Directory"), WS_CHILD | WS_VISIBLE | SS_LEFT | ES_READONLY, 20, 630, 150, 25, mmWindow, 0, hInstance, 0);
	SendMessage(mm_game_location_label, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), 0);

	// game location stuff
	mm_game_location_browse_button = CreateWindowEx(0, WC_BUTTON, _TEXT("Browse..."), BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE, 20, 655, 80, 25, mmWindow, (HMENU)MM_CONTROL_GAME_DIR_LOCATION_BTN, hInstance, 0);
	SendMessage(mm_game_location_browse_button, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), 0);

	mm_game_location_browse_label = CreateWindowEx(0, WC_EDIT, _TEXT(mm_has_game_directory() ? gamePath : "Where is DiRT Rally installed?"), WS_CHILD | WS_VISIBLE | SS_LEFT | ES_READONLY, 110, 660, 640, 25, mmWindow, (HMENU)MM_CONTROL_GAME_DIR_LOCATION_LABEL, hInstance, 0);
	SendMessage(mm_game_location_browse_label, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), 0);

	// backup location header
	mm_backup_location_label = CreateWindowEx(0, WC_EDIT, _TEXT("Backup Directory"), WS_CHILD | WS_VISIBLE | SS_LEFT | ES_READONLY, 20, 700, 150, 25, mmWindow, 0, hInstance, 0);
	SendMessage(mm_backup_location_label, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), 0);

	// backup location stuff
	mm_backup_location_browse_button = CreateWindowEx(0, WC_BUTTON, _TEXT("Browse..."), BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE, 20, 725, 80, 25, mmWindow, (HMENU)MM_CONTROL_BACKUP_DIR_LOCATION_BTN, hInstance, 0);
	SendMessage(mm_backup_location_browse_button, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), 0);

	mm_backup_location_browse_label = CreateWindowEx(0, WC_EDIT, _TEXT(mm_has_backup_directory() ? backupPath : "Where do you want to backup your original files to?"), WS_CHILD | WS_VISIBLE | SS_LEFT | ES_READONLY, 110, 730, 640, 25, mmWindow, (HMENU)MM_CONTROL_BACKUP_DIR_LOCATION_LABEL, hInstance, 0);
	SendMessage(mm_backup_location_browse_label, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), 0);

	// popup to show installing thing
	mm_mod_info_install_window = CreateWindowEx(0, WC_DIALOG, _TEXT(""), WS_CHILD, 225, 225, 350, 100, mmWindow, 0, hInstance, 0);
	SendMessage(mm_mod_info_install_window, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), 0);

	mm_mod_info_install_header = CreateWindowEx(0, WC_STATIC, _TEXT("Installing mod..."), WS_CHILD | WS_VISIBLE | SS_LEFT, 10, 10, 150, 25, mm_mod_info_install_window, (HMENU)MM_CONTROL_PROGRESS_HEADER, hInstance, 0);
	SendMessage(mm_mod_info_install_header, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), 0);

	mm_mod_info_install_progress = CreateWindowEx(0, PROGRESS_CLASS, NULL, WS_CHILD | WS_VISIBLE, 10, 40, 330, 25, mm_mod_info_install_window, (HMENU)MM_CONTROL_PROGRESS_BACKUP, hInstance, 0);
	SendMessage(mm_mod_info_install_progress, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), 0);

	mm_mod_info_install_file = CreateWindowEx(0, WC_STATIC, _TEXT(""), WS_CHILD | WS_VISIBLE | SS_LEFT, 10, 70, 330, 25, mm_mod_info_install_window, (HMENU)MM_CONTROL_PROGRESS_FILE, hInstance, 0);
	SendMessage(mm_mod_info_install_file, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), 0);
}

void mm_control_handler(HWND mmWindow, WPARAM wParam)
{
	switch (LOWORD(wParam))
	{
		case MM_CONTROL_MOD_LOCATION_BROWSE:
		{
			OPENFILENAME ofn;
			ZeroMemory(&ofn, sizeof(ofn));

			// get the path that we're gonna start from using the label
			TCHAR filePath[MAX_PATH] = _TEXT("");
			int len = GetWindowTextLength(GetDlgItem(mmWindow, MM_CONTROL_MOD_LOCATION_LABEL));
			if (len > 0)
				GetDlgItemText(mmWindow, MM_CONTROL_MOD_LOCATION_LABEL, filePath, MAX_PATH);

			// the info for the folder browser
			BROWSEINFO browseInfo = { 0 };
			browseInfo.lpszTitle = _T("Where are your mods stored?");
			browseInfo.hwndOwner = mmWindow;
			browseInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_NONEWFOLDERBUTTON | BIF_EDITBOX;
			browseInfo.lpfn = mm_control_mod_browser_handler;
			browseInfo.lParam = (LPARAM)filePath;

			// fire the folder browser and then update our label if we get somewhere
			LPITEMIDLIST lpItemIDList = SHBrowseForFolder(&browseInfo);
			if (lpItemIDList)
			{
				// get the folder we chose and set the label
				SHGetPathFromIDList(lpItemIDList, filePath);
				SetDlgItemText(mmWindow, MM_CONTROL_MOD_LOCATION_LABEL, filePath);
				mm_str_cpy(modPath, filePath, sizeof(modPath));

				// Save the directory into the config file.
				mm_save_config_file();

				// free memory
				CoTaskMemFree(lpItemIDList);

				// go through the directory and find any .zip files we have
				mm_handle_mod_directory_found(mmWindow, filePath);
			}
		}
		break;

		case MM_CONTROL_GAME_DIR_LOCATION_BTN:
		{
			OPENFILENAME ofn;
			ZeroMemory(&ofn, sizeof(ofn));

			// get the path that we're gonna start from using the label
			TCHAR filePath[MAX_PATH] = _TEXT("");
			int len = GetWindowTextLength(GetDlgItem(mmWindow, MM_CONTROL_GAME_DIR_LOCATION_LABEL));
			if (len > 0)
				GetDlgItemText(mmWindow, MM_CONTROL_GAME_DIR_LOCATION_LABEL, filePath, MAX_PATH);

			// the info for the folder browser
			BROWSEINFO browseInfo = { 0 };
			browseInfo.lpszTitle = _T("DiRT Rally Location");
			browseInfo.hwndOwner = mmWindow;
			browseInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_NONEWFOLDERBUTTON | BIF_EDITBOX;
			browseInfo.lpfn = mm_control_game_location_handler;
			browseInfo.lParam = (LPARAM)filePath;

			// fire the folder browser and then update our label if we get somewhere
			LPITEMIDLIST lpItemIDList = SHBrowseForFolder(&browseInfo);
			if (lpItemIDList)
			{
				// get the folder we chose and set the label
				SHGetPathFromIDList(lpItemIDList, filePath);
				SetDlgItemText(mmWindow, MM_CONTROL_GAME_DIR_LOCATION_LABEL, filePath);
				mm_str_cpy(gamePath, filePath, sizeof(gamePath));

				// Save the directory into the config file.
				mm_save_config_file();

				// free memory
				CoTaskMemFree(lpItemIDList);
			}
		}
		break;

		case MM_CONTROL_BACKUP_DIR_LOCATION_BTN:
		{
			OPENFILENAME ofn;
			ZeroMemory(&ofn, sizeof(ofn));

			// get the path that we're gonna start from using the label
			TCHAR filePath[MAX_PATH] = _TEXT("");
			int len = GetWindowTextLength(GetDlgItem(mmWindow, MM_CONTROL_BACKUP_DIR_LOCATION_LABEL));
			if (len > 0)
				GetDlgItemText(mmWindow, MM_CONTROL_BACKUP_DIR_LOCATION_LABEL, filePath, MAX_PATH);

			// the info for the folder browser
			BROWSEINFO browseInfo = { 0 };
			browseInfo.lpszTitle = _T("The directory to store your original DiRT Rally files in");
			browseInfo.hwndOwner = mmWindow;
			browseInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_NONEWFOLDERBUTTON | BIF_EDITBOX;
			browseInfo.lpfn = mm_control_backup_location_handler;
			browseInfo.lParam = (LPARAM)filePath;

			// fire the folder browser and then update our label if we get somewhere
			LPITEMIDLIST lpItemIDList = SHBrowseForFolder(&browseInfo);
			if (lpItemIDList)
			{
				// get the folder we chose and set the label
				SHGetPathFromIDList(lpItemIDList, filePath);
				SetDlgItemText(mmWindow, MM_CONTROL_BACKUP_DIR_LOCATION_LABEL, filePath);
				mm_str_cpy(backupPath, filePath, sizeof(backupPath));

				// Save the directory into the config file.
				mm_save_config_file();

				// free memory
				CoTaskMemFree(lpItemIDList);
			}
		}
		break;
	}
}

static int CALLBACK mm_control_mod_browser_handler(HWND hWnd, UINT message, LPARAM lParam, LPARAM lpData)
{
	switch (message)
	{
		case BFFM_INITIALIZED:
		{
			// update our selection if we passed data
			if (lpData)
				SendMessage(hWnd, BFFM_SETSELECTION, TRUE, lpData);
		}
		break;

		case BFFM_SELCHANGED:
		{
			TCHAR filePath[MAX_PATH] = { 0 };
			SHGetPathFromIDList(reinterpret_cast<LPITEMIDLIST>(lParam), filePath);

			// enable/disable ok button if we haven't got invalid file attributes
			if (GetFileAttributes(filePath) != INVALID_FILE_ATTRIBUTES)
				SendMessage(hWnd, BFFM_ENABLEOK, 0, TRUE);
			else
				SendMessage(hWnd, BFFM_ENABLEOK, 0, FALSE);
		}
		break;
	}

	return 0;
}

static int CALLBACK mm_control_game_location_handler(HWND hWnd, UINT message, LPARAM lParam, LPARAM lpData)
{
	switch (message)
	{
		case BFFM_INITIALIZED:
		{
			// update our selection if we passed data
			if (lpData)
				SendMessage(hWnd, BFFM_SETSELECTION, TRUE, lpData);
		}
		break;

		case BFFM_SELCHANGED:
		{
			TCHAR file_path[MAX_PATH] = { 0 };
			SHGetPathFromIDList(reinterpret_cast<LPITEMIDLIST>(lParam), file_path);

			_tcscat(file_path, _TEXT("\\drt.exe"));

			if (GetFileAttributes(file_path) != INVALID_FILE_ATTRIBUTES)
				SendMessage(hWnd, BFFM_ENABLEOK, 0, TRUE);
			else
				SendMessage(hWnd, BFFM_ENABLEOK, 0, FALSE);
		}
		break;
	}

	return 0;
}

static int CALLBACK mm_control_backup_location_handler(HWND hWnd, UINT message, LPARAM lParam, LPARAM lpData)
{
	switch (message)
	{
		case BFFM_INITIALIZED:
		{
			// update our selection if we passed data
			if (lpData)
				SendMessage(hWnd, BFFM_SETSELECTION, TRUE, lpData);
		}
		break;

		case BFFM_SELCHANGED:
		{
			TCHAR file_path[MAX_PATH] = { 0 };
			SHGetPathFromIDList(reinterpret_cast<LPITEMIDLIST>(lParam), file_path);

			if (GetFileAttributes(file_path) != INVALID_FILE_ATTRIBUTES)
				SendMessage(hWnd, BFFM_ENABLEOK, 0, TRUE);
			else
				SendMessage(hWnd, BFFM_ENABLEOK, 0, FALSE);
		}
		break;
	}

	return 0;
}

int progressInstallationFileCount = 0;
int progressInstallationFile = 0;

void mm_show_installation_progress(int fileCount, bool isInstalling)
{
	if (!mm_mod_info_install_window)
		return;

	// toggle the usability of the main window
	EnableWindow(mm_main_window, false);

	// toggle the visibility of the popover progress bar
	ShowWindow(mm_mod_info_install_window, true);

	// reset the position
	// update the text
	SetDlgItemTextA(mm_mod_info_install_window, MM_CONTROL_PROGRESS_HEADER, (isInstalling ? "Installing mod, please wait..." : "Uninstalling mod, please wait..."));
	SendMessage(mm_mod_info_install_progress, PBM_SETPOS, 0, 0);

	// file count
	progressInstallationFile = 0;
	progressInstallationFileCount = fileCount;
}

void mm_update_installation_file(const char* filename)
{
	if (!mm_mod_info_install_window)
		return;

	// update the file we're on
	SetDlgItemTextA(mm_mod_info_install_window, MM_CONTROL_PROGRESS_FILE, filename);
	
	// update what step we're on
	SendMessage(mm_mod_info_install_progress, PBM_SETPOS, ((++progressInstallationFile * 100) / progressInstallationFileCount), 0);

	// close if it's the last file
	if (progressInstallationFile == progressInstallationFileCount)
		mm_hide_installation_progress();
}

void mm_hide_installation_progress(void)
{
	if (!mm_mod_info_install_window)
		return;

	// toggle the usability of the main window
	EnableWindow(mm_main_window, true);

	// toggle the visibility of the popover progress bar
	ShowWindow(mm_mod_info_install_window, false);
}

void mm_handle_mod_directory_found(HWND hWnd, TCHAR* filePath)
{
	// Global state so we can ignore stuff when windows sends bs messages during list view initialization.
	scanning_mod_archives = true;

	// clear the mod list
	mm_clear_mod_list();

#ifdef _WIN32
	WIN32_FIND_DATA findData;
	char fileName[MAX_PATH];

	// find all files in the folder
	sprintf_s(fileName, sizeof(fileName), "%s/*", filePath);
	HANDLE hFind = FindFirstFile(fileName, &findData);
	
	if (hFind != INVALID_HANDLE_VALUE)
	{
		// while we find files, go through them
		do
		{
			mm_mod_item *modItem = mm_create_mod_item(filePath, findData.cFileName);

			// if the mod item is valid (our program supports the mod format), add it to the list
			if (modItem != NULL)
			{
				modItem->file_size = findData.nFileSizeLow;
				mm_add_mod_item(modItem);
			}
		
		} while (FindNextFile(hFind, &findData));

		// close the handle
		FindClose(hFind);
	}
#endif

	scanning_mod_archives = false;
}

void mm_update_mod_information(mm_mod_item* mod_item)
{
	// double check that this is valid
	if (!mod_item)
	{
		Static_SetText(mm_mod_info_name_data, "-");
		Static_SetText(mm_mod_info_size_data, "-");
		Static_SetText(mm_mod_info_file_count_data, "-");
		Static_SetText(mm_mod_info_vehicle_data, "-");
		Static_SetText(mm_mod_info_enabled_data, "-");

		SendMessage(mm_mod_info_file_list_data, LB_RESETCONTENT, 0, 0);

		return;
	}

	Static_SetText(mm_mod_info_name_data, mod_item->mod_name);

	char fileInfo[128] = { 0 };

	sprintf(fileInfo, "%0.2fMB", ((float)mod_item->file_size / 1000000));
	Static_SetText(mm_mod_info_size_data, fileInfo);

	sprintf(fileInfo, "%u", mod_item->file_count);
	Static_SetText(mm_mod_info_file_count_data, fileInfo);

	Static_SetText(mm_mod_info_vehicle_data, (mod_item->vehicle != NULL ? mod_item->vehicle->name : "-"));

	Static_SetText(mm_mod_info_enabled_data, (mod_item->enabled ? "Yes" : "No"));

	// List mod files
	SendMessage(mm_mod_info_file_list_data, LB_RESETCONTENT, 0, 0);

	for (int i = 0; i < mod_item->item_count; ++i)
	{
		mm_mod_file *file = mod_item->files[i];

		// Show game files only.
		if ((file->flags & FFLAG_MOD_FILE) == 0)
			continue;

		SendMessage(mm_mod_info_file_list_data, LB_ADDSTRING, 0, (LPARAM)file->name);
		SendMessage(mm_mod_info_file_list_data, LB_SETITEMDATA, 0, (LPARAM)i);
	}
}
