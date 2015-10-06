#pragma once
#ifndef _mm_controls_h
#define _mm_controls_h

typedef enum _MMControls
{
	MM_CONTROL_MOD_LOCATION_LABEL = 100,
	MM_CONTROL_MOD_LOCATION_BROWSE,
	MM_CONTROL_MOD_FILE_LIST
} MMControls;

void mm_create_controls(HWND mmWindow, HINSTANCE hInstance);
void mm_control_handler(HWND mmWindow, WPARAM wParam);

static int CALLBACK mm_control_mod_browser_handler(HWND hWnd, UINT message, LPARAM lParam, LPARAM lpData);
void mm_handle_mod_directory_found(HWND hWnd, TCHAR* filePath);

#endif
