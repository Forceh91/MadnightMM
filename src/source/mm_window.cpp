#include <Windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <CommCtrl.h>

#include "mm_window.h"
#include "mm_controls.h"
#include "mm_mod_list.h"
#include "mm_mod_installer.h"
#include "mm_utils.h"

// make this look at least somewhat nice (we're programmers, not designers)
#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HWND mm_window = 0;

extern bool mm_is_running;

LRESULT CALLBACK mm_wnd_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_COMMAND:
		{
			mm_control_handler(hWnd, wParam);
		}
		break;
		case WM_NOTIFY:
			switch (((LPNMHDR)lParam)->code)
			{
				case LVN_ITEMCHANGED:
					mm_mod_list_handle_item_change((LPNMLISTVIEW)lParam);
					break;
			}
		break;
		case WM_CLOSE:
		{
			mm_is_running = false;
			DestroyWindow(mm_window);
		}
		break;

		case WM_DESTROY:
			PostQuitMessage(0);
		break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}

bool mm_create_window(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine)
{
	WNDCLASSEX wc;
	INITCOMMONCONTROLSEX icex;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = mm_wnd_proc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wc.lpszMenuName = 0;
	wc.lpszClassName = _TEXT("MadnightMM");
	wc.hIcon = 0;
	wc.hIconSm = 0;

	if (!RegisterClassEx(&wc))
		return false;

	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_LISTVIEW_CLASSES;

	if (!InitCommonControlsEx(&icex))
		return false;

	// try to create the window
	mm_window = CreateWindowEx(WS_EX_CLIENTEDGE, _TEXT("MadnightMM"), _TEXT("Madnight Mod Manager"), WS_CAPTION | WS_SYSMENU | WS_MINIMIZE, CW_USEDEFAULT, CW_USEDEFAULT, 800, 830, 0, 0, hInstance, 0);
	if (!mm_window)
		return false;

	// create the controls
	mm_create_controls(mm_window, hInstance);

	// set the font
	SendMessage(mm_window, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), 0);

	// Scan the game directory to find out valid livery slots for each vehicle.
	if (mm_has_game_directory())
	{
		extern char gamePath[MAX_PATH];
		mm_scan_livery_list(gamePath);
	}

	// If the mod file has been selected previously, scan it for a list of installable mods.
	if (mm_has_mod_directory())
	{
		extern char modPath[MAX_PATH];
		mm_handle_mod_directory_found(mm_window, modPath);
	}

	// woot, everything is done
	return true;
}

void mm_show_window(int nCmdShow)
{
	ShowWindow(mm_window, nCmdShow);
	UpdateWindow(mm_window);
}
