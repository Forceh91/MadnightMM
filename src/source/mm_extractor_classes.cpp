#include "mm_extractor_classes.h"
#include <initguid.h>

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

HRESULT STDMETHODCALLTYPE InStreamWrapper::QueryInterface(REFIID iid, void **object)
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

	if (iid == IID_IStreamGetSize)
	{
		*object = static_cast<IStreamGetSize*>(this);
		AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE InStreamWrapper::AddRef()
{
	return static_cast< ULONG >(InterlockedIncrement(&refCount));
}

ULONG STDMETHODCALLTYPE InStreamWrapper::Release()
{
	ULONG res = static_cast< ULONG >(InterlockedDecrement(&refCount));
	if (res == 0)
	{
		delete this;
	}
	return res;
}

STDMETHODIMP InStreamWrapper::Read(void *data, UInt32 size, UInt32 *processedSize)
{
	ULONG read = 0;
	HRESULT hr = stream->Read(data, size, &read);

	if (processedSize != NULL)
	{
		*processedSize = read;
	}
	
	return SUCCEEDED(hr) ? S_OK : hr;
}

STDMETHODIMP InStreamWrapper::Seek(Int64 offset, UInt32 seekOrigin, UInt64 *newPosition)
{
	LARGE_INTEGER move;
	ULARGE_INTEGER newPos;

	move.QuadPart = offset;
	HRESULT hr = stream->Seek(move, seekOrigin, &newPos);

	if (newPosition != NULL)
	{
		*newPosition = newPos.QuadPart;
	}

	return hr;
}

STDMETHODIMP InStreamWrapper::GetSize(UInt64* size)
{
	STATSTG statInfo;
	HRESULT hr = stream->Stat(&statInfo, STATFLAG_NONAME);

	if (SUCCEEDED(hr))
	{
		*size = statInfo.cbSize.QuadPart;
	}

	return hr;
}

STDMETHODIMP ArchiveOpenCallback::QueryInterface(REFIID iid, void **object)
{
	if (iid == __uuidof(IUnknown))
	{
		*object = reinterpret_cast<IUnknown*>(this);
		AddRef();
		return S_OK;
	}

	if (iid == IID_IArchiveOpenCallback)
	{
		*object = static_cast<IArchiveOpenCallback*>(this);
		AddRef();
		return S_OK;
	}

	if (iid == IID_ICryptoGetTextPassword)
	{
		*object = static_cast<ICryptoGetTextPassword*>(this);
		AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) ArchiveOpenCallback::AddRef()
{
	return static_cast<ULONG>(InterlockedIncrement(&refCount));
}

STDMETHODIMP_(ULONG) ArchiveOpenCallback::Release()
{
	ULONG res = static_cast<ULONG>(InterlockedDecrement(&refCount));

	if (res == 0)
	{
		delete this;
	}

	return res;
}
