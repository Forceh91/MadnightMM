#pragma once
#ifndef _mm_extractor_classes_h
#define _mm_extractor_classes_h

#include <7zip/Archive/IArchive.h>
#include <7zip/IPassword.h>
#include <7zip/IStream.h>
#include <atlcomcli.h>

//
// Minimal implementations for some classes required to use the 7-zip library.
// Adapted for Madnight Mod Manager from SevenZip++ https://bitbucket.org/cmcnab/sevenzip
//

class InStreamWrapper : public IInStream, public IStreamGetSize
{
public:
	InStreamWrapper(const CComPtr<IStream> &s) { refCount = 0; stream = s; }
	virtual ~InStreamWrapper() {}

	STDMETHOD(QueryInterface)(REFIID iid, void **object);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();
	STDMETHOD(Read)(void *data, UInt32 size, UInt32 *processedSize);
	STDMETHOD(Seek)(Int64 offset, UInt32 seekOrigin, UInt64 *newPosition);
	STDMETHOD(GetSize)(UInt64 *size);

private:
	long refCount;
	CComPtr<IStream> stream;
};

class ArchiveOpenCallback : public IArchiveOpenCallback, public ICryptoGetTextPassword
{
public:
	ArchiveOpenCallback() { refCount = 0; }
	virtual ~ArchiveOpenCallback() {}

	STDMETHOD(QueryInterface)(REFIID iid, void **object);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();
	STDMETHOD(SetTotal)(const UInt64 *files, const UInt64 *bytes) { return S_OK; }
	STDMETHOD(SetCompleted)(const UInt64 *files, const UInt64 *bytes) { return S_OK; }
	STDMETHOD(CryptoGetTextPassword)(BSTR *password) { return E_ABORT; }

private:
	long refCount;
};

#endif
