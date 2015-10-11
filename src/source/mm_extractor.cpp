#include <Windows.h>
#include <atlcomcli.h>
#include <Shlwapi.h>
#include "mm_extractor.h"
#include "mm_extractor_classes.h"
#include <initguid.h>

// GUIDs for file formats supported by 7-zip.
DEFINE_GUID(CLSID_CFormatZip, 0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x01, 0x00, 0x00); // {23170F69-40C1-278A-1000-000110010000}
DEFINE_GUID(CLSID_CFormatBZip2, 0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x02, 0x00, 0x00); // {23170F69-40C1-278A-1000-000110020000}
DEFINE_GUID(CLSID_CFormatRar, 0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x03, 0x00, 0x00); // {23170F69-40C1-278A-1000-000110030000}
DEFINE_GUID(CLSID_CFormat7z, 0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x07, 0x00, 0x00); // {23170F69-40C1-278A-1000-000110070000}
DEFINE_GUID(CLSID_CFormatCab, 0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x08, 0x00, 0x00); // {23170F69-40C1-278A-1000-000110080000}
DEFINE_GUID(CLSID_CFormatLzma, 0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x0A, 0x00, 0x00); // {23170F69-40C1-278A-1000-0001100A0000}
DEFINE_GUID(CLSID_CFormatLzma86, 0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x0B, 0x00, 0x00); // {23170F69-40C1-278A-1000-0001100B0000}
DEFINE_GUID(CLSID_CFormatIso, 0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0xE7, 0x00, 0x00); // {23170F69-40C1-278A-1000-000110E70000}
DEFINE_GUID(CLSID_CFormatTar, 0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0xEE, 0x00, 0x00); // {23170F69-40C1-278A-1000-000110EE0000}
DEFINE_GUID(CLSID_CFormatGZip, 0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0xEF, 0x00, 0x00); // {23170F69-40C1-278A-1000-000110EF0000}


static HMODULE libHandle;

typedef unsigned int (__stdcall *CreateObjectFunc)(const GUID *clsID, const GUID *interfaceID, void **outObject);
static CreateObjectFunc CreateObject;


bool mm_extractor_initialize(const char *pathToLibrary)
{
	libHandle = LoadLibraryA(pathToLibrary);

	if (libHandle != NULL)
	{
		CreateObject = (CreateObjectFunc)GetProcAddress(libHandle, "CreateObject");

		if (CreateObject != NULL)
			return true;
		
		// Something went wrong and we could not initialize the 7-zip library.
		mm_extractor_shutdown();
	}

	return false;
}

void mm_extractor_shutdown()
{
	if (libHandle != NULL)
	{
		FreeLibrary(libHandle);

		libHandle = NULL;
		CreateObject = NULL;
	}
}

static CComPtr<IStream> mm_extractor_open_file(const char *path)
{
	CComPtr<IStream> fileStream;

	WCHAR filePathStr[MAX_PATH];
	MultiByteToWideChar(CP_UTF8, 0, path, (int)strlen(path) + 1, filePathStr, MAX_PATH);

	if (FAILED(SHCreateStreamOnFileEx(filePathStr, STGM_READ, FILE_ATTRIBUTE_NORMAL, FALSE, NULL, &fileStream)))
	{
		return NULL;
	}

	return fileStream;
}

bool mm_extractor_scan(mm_mod_item *mod)
{
	CComPtr<IStream> fileStream = mm_extractor_open_file(mod->file_path);

	// Could not open file for reading.
	if (fileStream == NULL)
		return false;

	const GUID* guid = NULL;

	// We only support mods in .zip, .rar and .7z archives for the time being.
	switch (mod->file_format)
	{
	case FORMAT_7Z:
		guid = &CLSID_CFormat7z;
		break;

	case FORMAT_ZIP:
		guid = &CLSID_CFormatZip;
		break;

	case FORMAT_RAR:
		guid = &CLSID_CFormatRar;
		break;

	default:
		return false;
	}

	CComPtr<IInArchive> archive;
	HRESULT hr = CreateObject(guid, &IID_IInArchive, (void **)&archive);

	// Could not get a handle to archive reader.
	if (FAILED(hr))
		return false;
	
	CComPtr<InStreamWrapper> inFile = new InStreamWrapper(fileStream);
	CComPtr<ArchiveOpenCallback> openCallback = new ArchiveOpenCallback();

	hr = archive->Open(inFile, 0, openCallback);

	// Could not open archive/it is not a valid archive format.
	if (hr != S_OK)
		return false;

	UInt32 fileCount;
	hr = archive->GetNumberOfItems(&fileCount);

	// Could not get file count for the archive.
	if (hr != S_OK)
		return false;

	// Create storage for the file list.
	mod->item_count = (fileCount < 255 ? (unsigned char)fileCount : 255);
	mod->files = new mm_mod_file*[mod->item_count];

	for (UInt32 i = 0; i < fileCount; ++i)
	{
		if (i >= mod->item_count)
			break;

		char path[MAX_PATH];

		PROPVARIANT filePath; // 3 = Path
		filePath.vt = VT_EMPTY;
		filePath.wReserved1 = 0;

		PROPVARIANT isDirectory; // 6 = IsDirectory
		isDirectory.vt = VT_EMPTY;
		isDirectory.wReserved1 = 0;

		// 7 = Size
		// 8 = PackedSize

		archive->GetProperty(i, 3, &filePath);
		archive->GetProperty(i, 6, &isDirectory);

		WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, filePath.bstrVal, -1, path, sizeof(path), NULL, NULL);

		bool directory = (isDirectory.boolVal == VARIANT_TRUE);

		mod->files[i] = mm_create_mod_file((unsigned char)i, path, directory);

		if (!directory)
			mod->file_count++;
	}

	archive->Close();
	return true;
}
