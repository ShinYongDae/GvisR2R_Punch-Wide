/*
 *	$Header: $
 *
 *	$History: $
 */
#include "stdafx.h"
#include "SyncMailslotReader.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif

// Construction/Destruction
CSyncMailslotReader::CSyncMailslotReader() : CMailslot()
{
}

CSyncMailslotReader::~CSyncMailslotReader()
{
}

//	Create a named mail slot. This must be done on the local machine
//	thus we don't use the server name parameter.
bool CSyncMailslotReader::Connect(LPCTSTR szSlotname, LPCTSTR /*szServer*/)
{
	assert(szSlotname);
	
	if (IsOpen())
	{
		TCHAR szTempSlotname[_MAX_PATH];

		//	If we get here it means the mailslot handle might be valid so
		//	let's check that the m_pszSlotname variable isn't a NULL pointer
		//	If it is then we've got an inconsistency that oughtn't to happen.
		assert(m_pszSlotname);
		_sntprintf(szTempSlotname, _MAX_PATH, _T("\\\\.\\mailslot\\%s"), szSlotname);
		
		if (_tcsicmp(m_pszSlotname, szTempSlotname) == 0)
			return true;
		else
			Disconnect();
	}

	//	Delete any previously created slot name
	delete m_pszSlotname;
	m_pszSlotname = new TCHAR[_MAX_PATH];
	assert(m_pszSlotname);

	//	Create our mail slot name
	_sntprintf(m_pszSlotname, _MAX_PATH, _T("\\\\.\\mailslot\\%s"), szSlotname);
	m_pszSlotname[_MAX_PATH - sizeof(TCHAR)] = TCHAR(0);
	
	if ((m_hMailSlot = CreateMailslot(m_pszSlotname, 0, MAILSLOT_WAIT_FOREVER, NULL)) != INVALID_HANDLE_VALUE)
	{
		//	Attach the mailslot handle to the overlapped
		//	I/O object.
		m_overlapped.Attach(m_hMailSlot);
		return true;
	}

	return false;
}

//	Return a count of how many messages are waiting in the mailslot.
DWORD CSyncMailslotReader::GetMessageCount(DWORD *pdwNextMessageLength)
{
	DWORD dwCount = 0;

	if (IsOpen())
		GetMailslotInfo(m_hMailSlot, NULL, pdwNextMessageLength, &dwCount, NULL);

	return dwCount;
}

//	Read a message from the mailslot and return it in a buffer allocated
//	on the heap. The caller must delete the buffer once it's done with it.
BYTE *CSyncMailslotReader::Read(DWORD& dwBufferLength)
{
	//	We have to allocate a large buffer for incoming messages because
	//	we have no idea how much data is coming...
	BYTE  *pbData = (BYTE *) NULL,
		  *pbTemp = (BYTE *) NULL;

	dwBufferLength = 0;

	if (IsOpen())
	{
		pbData = new BYTE[65536];
		assert(pbData);

		//	Now read the data
		if (m_overlapped.Read(pbData, 65536 - sizeof(TCHAR), &dwBufferLength, m_hStopEvent) && dwBufferLength)
		{
			//	If we read a message it's time to copy the data into a buffer 
			//	of the correct length to hold the message.  Actually we add 
			//	one character to the buffer so that, if the message is really
			//	a string, it'll be correctly terminated and maintain string
			//	semantics.
			pbTemp = new BYTE[dwBufferLength + sizeof(TCHAR)];
			assert(pbTemp);
			memcpy(pbTemp, pbData, dwBufferLength);
			pbTemp[dwBufferLength] = TCHAR(0);
		}
	}

	delete [] pbData;
	return pbTemp;
}
