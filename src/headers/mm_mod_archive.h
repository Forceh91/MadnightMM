#pragma once
#ifndef _mm_mod_archive_h
#define _mm_mod_archive_h

#include "mm_mod_item.h"
#include <7zip/Archive/IArchive.h>
#include <7zip/IStream.h>
#include <combaseapi.h>

typedef struct _GUID GUID;
typedef unsigned int(__stdcall *CreateObjectFunc)(const GUID *clsID, const GUID *interfaceID, void **outObject);

typedef void (__stdcall *ExtractCallback)(mm_mod_item *mod, mm_mod_file *file);

//
// ModArchive
//
class ModArchive : public IInStream, public IArchiveExtractCallback
{
public:
	static bool Initialize(const char *pathTo7zip);
	static void Shutdown();

	ModArchive(mm_mod_item *modItem);
	virtual ~ModArchive();

	bool IsOpen() const { return (stream != NULL); }

	bool Scan();
	bool Extract(const char *targetFolder, ExtractCallback callback = NULL);

	// IUnknown
	STDMETHOD(QueryInterface)(REFIID iid, void **object);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();

	// IInStream
	STDMETHOD(Read)(void *data, UInt32 size, UInt32 *processedSize);
	STDMETHOD(Seek)(Int64 offset, UInt32 seekOrigin, UInt64 *newPosition);

	// IProgress
	STDMETHOD(SetTotal)(UInt64 size) { return S_OK; }
	STDMETHOD(SetCompleted)(const UInt64 *completeValue) { return S_OK; }

	// IArchiveExtractCallback
	STDMETHOD(GetStream)(UInt32 index, ISequentialOutStream **outStream, Int32 askExtractMode);
	STDMETHOD(PrepareOperation)(Int32 askExtractMode) { return S_OK; }
	STDMETHOD(SetOperationResult)(Int32 resultEOperationResult) { return S_OK; }

private:
	bool Open();
	bool ScanFiles();

private:
	static HMODULE libHandle;
	static CreateObjectFunc CreateObject;

	long refCounter;

	mm_mod_item *mod;
	IStream *stream;
	IInArchive *archive;

	char *extractPath;
	ExtractCallback extractCallback;
};

//
// ModFileStream
//
class ModFileStream : public IOutStream
{
public:
	ModFileStream(IStream *baseStream) { refCounter = 1; stream = baseStream; }
	virtual ~ModFileStream() {}

	STDMETHOD(QueryInterface)(REFIID iid, void **object);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();

	// ISequentialOutStream
	STDMETHOD(Write)(const void *data, UInt32 size, UInt32 *processedSize);

	// IOutStream
	STDMETHOD(Seek)(Int64 offset, UInt32 seekOrigin, UInt64 *newPosition);
	STDMETHOD(SetSize)(UInt64 newSize);

private:
	long refCounter;
	IStream *stream;
};

#endif
