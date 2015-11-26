#include <ShlObj.h>
#include <tchar.h>
#include <direct.h>

#include "mm_window.h"
#include "mm_mod_archive.h"
#include "mm_mod_installer.h"
#include "mm_utils.h"
#include "mm_mod_files_window.h"

bool mm_is_running = false;
TCHAR mm_app_data_loc[MAX_PATH] = { 0 };

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// store where we're gonna keep stuff in the app data
	SHGetSpecialFolderPath(0, mm_app_data_loc, CSIDL_APPDATA, false);

	// create the directory where we will store stuff
	_tcscat(mm_app_data_loc, _TEXT("\\Madnight Software\\MadnightMM"));
	//_tmkdir(mm_app_data_loc);
	mm_ensure_folder_exists(mm_app_data_loc);

	// load a list of previously installed mods and settings
	mm_load_config_file();

	// initialize 7-zip.
	ModArchive::Initialize("./7z.dll");

	// create our window and populate it with available mods if the mod path is stored into the config file
	if (!mm_create_window(hInstance, hPrevInstance, lpCmdLine))
		return 0;

	// create the mod list window
	if (!mm_mod_files_create_window(hInstance, hPrevInstance, lpCmdLine))
		return 0;

	// running
	mm_is_running = true;

	// show the main window
	mm_show_window(nCmdShow);

	// process messages
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	ModArchive::Shutdown();

	// cleanup memory allocated for the installed mod list cache
	mm_cleanup_installed_mods();

	// all done
	return (int)(msg.wParam);
}
