#include "mm_mod_archive.h"
#include "mm_utils.h"
#include <Windows.h>
#include <initguid.h>
#include <Shlwapi.h>
#include <stdio.h>

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

// GUIDs for 7-zip classes.
DEFINE_GUID(IID_IInArchive, 0x23170F69, 0x40C1, 0x278A, 0x00, 0x00, 0x00, 0x06, 0x00, 0x60, 0x00, 0x00); // {23170F69-40C1-278A-0000-000600600000}
DEFINE_GUID(IID_ISequentialInStream, 0x23170F69, 0x40C1, 0x278A, 0x00, 0x00, 0x00, 0x03, 0x00, 0x01, 0x00, 0x00); // {23170F69-40C1-278A-0000-000300010000}
DEFINE_GUID(IID_ISequentialOutStream, 0x23170F69, 0x40C1, 0x278A, 0x00, 0x00, 0x00, 0x03, 0x00, 0x02, 0x00, 0x00); // {23170F69-40C1-278A-0000-000300010000}
DEFINE_GUID(IID_IInStream, 0x23170F69, 0x40C1, 0x278A, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 0x00); // {23170F69-40C1-278A-0000-000300030000}
DEFINE_GUID(IID_IOutStream, 0x23170F69, 0x40C1, 0x278A, 0x00, 0x00, 0x00, 0x03, 0x00, 0x04, 0x00, 0x00); // {23170F69-40C1-278A-0000-000300040000}
DEFINE_GUID(IID_IStreamGetSize, 0x23170F69, 0x40C1, 0x278A, 0x00, 0x00, 0x00, 0x03, 0x00, 0x06, 0x00, 0x00); // {23170F69-40C1-278A-0000-000300060000}
DEFINE_GUID(IID_ICompressProgressInfo, 0x23170F69, 0x40C1, 0x278A, 0x00, 0x00, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00); // {23170F69-40C1-278A-0000-000400040000}
DEFINE_GUID(IID_ICryptoGetTextPassword, 0x23170F69, 0x40C1, 0x278A, 0x00, 0x00, 0x00, 0x05, 0x00, 0x10, 0x00, 0x00); // {23170F69-40C1-278A-0000-000500100000}
DEFINE_GUID(IID_ICryptoGetTextPassword2, 0x23170F69, 0x40C1, 0x278A, 0x00, 0x00, 0x00, 0x05, 0x00, 0x11, 0x00, 0x00); // {23170F69-40C1-278A-0000-000500110000}
DEFINE_GUID(IID_IArchiveOpenCallback, 0x23170F69, 0x40C1, 0x278A, 0x00, 0x00, 0x00, 0x06, 0x00, 0x10, 0x00, 0x00); // {23170F69-40C1-278A-0000-000600100000}
DEFINE_GUID(IID_IArchiveExtractCallback, 0x23170F69, 0x40C1, 0x278A, 0x00, 0x00, 0x00, 0x06, 0x00, 0x20, 0x00, 0x00); // {23170F69-40C1-278A-0000-000600200000}


//
// ModArchive
//

HMODULE ModArchive::libHandle = NULL;
CreateObjectFunc ModArchive::CreateObject = NULL;

bool ModArchive::Initialize(const char *pathTo7zip)
{
	libHandle = LoadLibraryA(pathTo7zip);

	if (libHandle != NULL)
	{
		CreateObject = (CreateObjectFunc)GetProcAddress(libHandle, "CreateObject");

		if (CreateObject != NULL)
			return true;

		// Something went wrong and we could not initialize the 7-zip library.
		Shutdown();
	}

	return false;
}

void ModArchive::Shutdown()
{
	if (libHandle != NULL)
	{
		FreeLibrary(libHandle);

		libHandle = NULL;
		CreateObject = NULL;
	}
}

ModArchive::ModArchive(mm_mod_item *modItem)
{
	refCounter = 0;

	mod = modItem;
	stream = NULL;
	
	extractPath = NULL;
}

ModArchive::~ModArchive()
{
	if (archive != NULL)
	{
		archive->Close();
		archive = NULL;
	}

	if (stream != NULL)
	{
		stream->Release();
		stream = NULL;
	}
}

bool ModArchive::Scan()
{
	if (!IsOpen() && !Open())
	{
		// Could not open archive file for scanning (perhaps it is an invalid file format?)
		return false;
	}

	// Scan the files within the archive.
	return ScanFiles();
}

bool ModArchive::Extract(const char *targetFolder, ExtractCallback callback)
{
	if (targetFolder == NULL ||
		*targetFolder == 0)
	{
		// Invalid destination folder.
		return false;
	}

	if (!IsOpen() && !Open())
	{
		// Could not open archive file for reading (perhaps it is an invalid file format?)
		return false;
	}

	// Re-scan the archive to make sure we have the correct file indices.
	mm_destroy_mod_item_files(mod);
	ScanFiles();

	if (mod->file_count == 0)
	{
		// Nothing to extract -> our job here is done!
		return true;
	}

	size_t len = strlen(targetFolder) + 1;

	extractPath = new char[len];
	mm_str_cpy(extractPath, targetFolder, len);

	// Compile a list of file indices for the files we're going to extract. These would be the actual mod files.
	UInt32 *indices = new UInt32[mod->file_count];

	for (unsigned int i = 0, j = 0; i < mod->item_count && j < mod->file_count; ++i)
	{
		if ((mod->files[i]->flags & FFLAG_MOD_FILE) != 0)
			indices[j++] = i;
	}

	// Use this callback to back up any files that would be overwritten.
	extractCallback = callback;

	// Now extract the files into the given folder.
	bool succeeded = (archive->Extract(indices, mod->file_count, 0, this) == S_OK);

	delete[] indices;
	delete[] extractPath;

	return succeeded;
}

bool ModArchive::Open()
{
	if (stream != NULL)
	{
		// The archive has already been opened.
		return false;
	}

	const GUID *guid = NULL;

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

	if (FAILED(CreateObject(guid, &IID_IInArchive, (void **)&archive)))
	{
		// Could not create an archive reader.
		return false;
	}

	WCHAR path[MAX_PATH];
	MultiByteToWideChar(CP_UTF8, 0, mod->file_path, (int)strlen(mod->file_path) + 1, path, MAX_PATH);

	if (FAILED(SHCreateStreamOnFileEx(path, STGM_READ, FILE_ATTRIBUTE_NORMAL, FALSE, NULL, &stream)))
	{
		// Could not open file for reading.
		return false;
	}

	if (archive->Open(this, 0, NULL) != S_OK)
	{
		// Could not open archive/it is not a valid archive format.
		return false;
	}

	return true;
}

bool ModArchive::ScanFiles()
{
	UInt32 fileCount;

	if (archive->GetNumberOfItems(&fileCount) != S_OK)
	{
		// Could not get file count for the archive.
		mod->item_count = 0;
		return false;
	}

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

		if ((mod->files[i]->flags & FFLAG_MOD_FILE) != 0)
		{
			mod->file_count++;

			if (mod->vehicle == NULL)
				mod->vehicle = mod->files[i]->vehicle;
		}
	}

	return true;
}

HRESULT STDMETHODCALLTYPE ModArchive::QueryInterface(REFIID iid, void **object)
{
	if (iid == __uuidof(IUnknown))
	{
		*object = reinterpret_cast<IUnknown*>(this);

		AddRef();
		return S_OK;
	}

	if (iid == IID_ISequentialInStream)
	{
		*object = static_cast<ISequentialInStream*>(this);

		AddRef();
		return S_OK;
	}

	if (iid == IID_IInStream)
	{
		*object = static_cast<IInStream*>(this);

		AddRef();
		return S_OK;
	}

	if (iid == IID_IArchiveExtractCallback)
	{
		*object = static_cast<IArchiveExtractCallback*>(this);

		AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) ModArchive::AddRef()
{
	return static_cast<unsigned long>(InterlockedIncrement(&refCounter));
}

STDMETHODIMP_(ULONG) ModArchive::Release()
{
	unsigned long count = static_cast<unsigned long>(InterlockedDecrement(&refCounter));

	if (count == 0)
		delete this;

	return count;
}

STDMETHODIMP ModArchive::Read(void *data, UInt32 size, UInt32 *processedSize)
{
	ULONG read = 0;
	HRESULT hr = stream->Read(data, size, &read);

	if (processedSize != NULL)
		*processedSize = read;

	return SUCCEEDED(hr) ? S_OK : hr;
}

STDMETHODIMP ModArchive::Seek(Int64 offset, UInt32 seekOrigin, UInt64 *newPosition)
{
	LARGE_INTEGER move;
	ULARGE_INTEGER newPos;

	move.QuadPart = offset;
	HRESULT hr = stream->Seek(move, seekOrigin, &newPos);

	if (newPosition != NULL)
		*newPosition = newPos.QuadPart;

	return hr;
}

STDMETHODIMP ModArchive::GetStream(UInt32 index, ISequentialOutStream** outStream, Int32 askExtractMode)
{
	if (askExtractMode != NArchive::NExtract::NAskMode::kExtract)
	{
		// In case someone wants to do something else than extract these files.
		return S_OK;
	}

	if (index >= mod->item_count)
	{
		// This should never happen because we give 7-zip a list of file indices for the files we want to extract, but just in case...
		return S_OK;
	}

	mm_mod_file *modFile = mod->files[index];

	// Call the extract callback, which should take care of backing up the original file if necessary.
	if (extractCallback != NULL)
		extractCallback(mod, modFile);

	// Figure out where this particular file should go.
	char filePath[MAX_PATH];
	mm_get_mod_file_path(modFile, filePath, sizeof(filePath), extractPath, false);

	// Make sure the target folder exists.
	mm_ensure_folder_exists(filePath);

	// Get the complete path for the file to be extracted.
	mm_get_mod_file_path(modFile, filePath, sizeof(filePath), extractPath);

	WCHAR path[MAX_PATH];
	MultiByteToWideChar(CP_UTF8, 0, filePath, (int)strlen(filePath) + 1, path, MAX_PATH);

	IStream *stream;

	// Create a write stream for 7-zip to use.
	if (FAILED(SHCreateStreamOnFileEx(path, STGM_CREATE | STGM_WRITE, FILE_ATTRIBUTE_NORMAL, TRUE, NULL, &stream)))
	{
		// Could not open file for writing.
		return HRESULT_FROM_WIN32(GetLastError());;
	}

	ModFileStream *file = new ModFileStream(stream);
	*outStream = file;

	return S_OK;
}


//
// ModFileStream
//

HRESULT STDMETHODCALLTYPE ModFileStream::QueryInterface(REFIID iid, void** object)
{
	if (iid == __uuidof(IUnknown))
	{
		*object = static_cast<IUnknown*>(this);
		AddRef();

		return S_OK;
	}

	if (iid == IID_ISequentialOutStream)
	{
		*object = static_cast<ISequentialOutStream*>(this);
		AddRef();

		return S_OK;
	}

	if (iid == IID_IOutStream)
	{
		*object = static_cast<IOutStream*>(this);
		AddRef();

		return S_OK;
	}

	return E_NOINTERFACE;
}

unsigned long STDMETHODCALLTYPE ModFileStream::AddRef()
{
	return static_cast<unsigned long>(InterlockedIncrement(&refCounter));
}

unsigned long STDMETHODCALLTYPE ModFileStream::Release()
{
	unsigned long res = static_cast<unsigned long>(InterlockedDecrement(&refCounter));

	if (res == 0)
	{
		stream->Release();
		delete this;
	}

	return res;
}

STDMETHODIMP ModFileStream::Write(const void* data, UInt32 size, UInt32* processedSize)
{
	unsigned long written = 0;

	HRESULT hr = stream->Write(data, size, &written);

	if (processedSize != NULL)
		*processedSize = written;
	
	return hr;
}

STDMETHODIMP ModFileStream::Seek(Int64 offset, UInt32 seekOrigin, UInt64* newPosition)
{
	LARGE_INTEGER move;
	ULARGE_INTEGER newPos;

	move.QuadPart = offset;

	HRESULT hr = stream->Seek(move, seekOrigin, &newPos);

	if (newPosition != NULL)
		*newPosition = newPos.QuadPart;

	return hr;
}

STDMETHODIMP ModFileStream::SetSize(UInt64 newSize)
{
	ULARGE_INTEGER size;
	size.QuadPart = newSize;

	return stream->SetSize(size);
}
