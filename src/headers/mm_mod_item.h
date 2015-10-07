#pragma once
#ifndef _mm_mod_item_h
#define _mm_mod_item_h

typedef struct _mm_mod_item
{
	char file_path[MAX_PATH]; // file path to the mod
	char mod_name[MAX_PATH]; // the name of the mod
	char* files[100][MAX_PATH]; // an array of file paths inside the .zip file
	bool enabled; // whether or not it is enabled
	DWORD fileSize; // the size of this mod

} mm_mod_item;

#endif
