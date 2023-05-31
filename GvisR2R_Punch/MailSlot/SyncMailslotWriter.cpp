/*
 *	$Header: $
 *
 *	$History: $
 */
#include "stdafx.h"
#include "SyncMailslotWriter.h"

// Construction/Destruction
CSyncMailslotWriter::CSyncMailslotWriter() : CMailslot()
{
}

CSyncMailslotWriter::~CSyncMailslotWriter()
{
}

//	Creates a connection to a mail slot.
//	Returns true on success, false on failure.
bool CSyncMailslotWriter::Connect(LPCTSTR szSlotname, LPCTSTR szServer)
{
	assert(szServer);
	assert(szSlotname);

	//	Delete any previous mail slot name
	delete m_pszSlotname;
	m_pszSlotname = new TCHAR[_MAX_PATH];
	assert(m_pszSlotname);

	//	Create our mail slot name
	_sntprintf(m_pszSlotname, _MAX_PATH, _T("\\\\%s\\mailslot\\%s"), szServer, szSlotname);
	m_pszSlotname[_MAX_PATH - sizeof(TCHAR)] = TCHAR(0);
	
	//	Now connect...
	return Connect();
}

bool CSyncMailslotWriter::Connect()
{
	//	Close any existing mail slot
	Disconnect();

	//	Now open the mail slot for overlapped I/O
	if ((m_hMailSlot = CreateFile(m_pszSlotname, 
							GENERIC_WRITE, 
							FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 
							FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, &m_overlapped)
							) != INVALID_HANDLE_VALUE)
	{
		m_overlapped.Attach(m_hMailSlot);
		return true;
	}

	return false;
}

//	Writes a message to the mail slot.
DWORD CSyncMailslotWriter::Write(BYTE *pbData, DWORD dwDataLength)
{
	assert(pbData);
	assert(dwDataLength);

	int nRetries = 2;

	while (nRetries--)
	{
		//	If the mail slot is closed attempt to reconnect to it
		if (!IsOpen() && m_pszSlotname != LPTSTR(NULL))
			Connect();

		DWORD dwWrittenLength = 0;

		if (IsOpen())
		{
			//	Write using overlapped I/O. We have to use overlapped 
			//	I/O if we want to be able to interrupt the write. If we use 
			//	synchronous I/O there's a high chance the operation will stall 
			//	inside the WriteFile call.  See 
			//	http://www.codeproject.com/win32/overlappedio.asp
			//	for a more detailed explanation.
			if (m_overlapped.Write(pbData, dwDataLength, &dwWrittenLength, m_hStopEvent) && dwWrittenLength == dwDataLength)
				//	The I/O completed so return success (true).
				return dwWrittenLength;
			else
				//	If the write failed discard it but also force a disconnect so
				//	that the next write will attempt a connection.
				Disconnect();
		}
	}

	return 0;
}
