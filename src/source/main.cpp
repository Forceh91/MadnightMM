#include <ShlObj.h>
#include <tchar.h>
#include <direct.h>

#include "mm_window.h"
#include "mm_mod_archive.h"

bool mm_is_running = false;
TCHAR mm_app_data_loc[MAX_PATH] = { 0 };

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// store where we're gonna keep stuff in the app data
	SHGetSpecialFolderPath(0, mm_app_data_loc, CSIDL_APPDATA, false);

	// create the directory where we will store stuff
	TCHAR temp_dir[400] = { 0 };
	_tcscpy(temp_dir, mm_app_data_loc);
	_tcscat(temp_dir, _TEXT("/Madnight Software/MadnightMM"));
	_tmkdir(temp_dir);

	// create our window
	if (!mm_create_window(hInstance, hPrevInstance, lpCmdLine))
		return 0;

	// initialize 7-zip.
	ModArchive::Initialize("./7z.dll");

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

	// all done
	return (int)(msg.wParam);
}
