/*
 *	$Header: $
 *
 *	$History: $
 */
#include "stdafx.h"
#include "AsyncMailslotReader.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CAsyncMailslotReader::CAsyncMailslotReader() : CSyncMailslotReader()
{
	m_pThread = (CBaseThread *) NULL;
}

CAsyncMailslotReader::~CAsyncMailslotReader()
{
	m_bStop = true;
	SetEvent(m_hStopEvent);

	if (m_pThread != (CBaseThread *) NULL)
		m_pThread->Wait();

	delete m_pThread;
}

bool CAsyncMailslotReader::Connect(LPCTSTR szSlotName)
{
	assert(szSlotName);
	bool bStatus = CSyncMailslotReader::Connect(szSlotName);

	if (bStatus)
	{
		//	If we succeeded in creating the mailslot we start up a thread to
		//	monitor it.
		m_pThread = new CBaseThread(m_hStopEvent, &m_bStop, ThreadStub, false, this);
		assert(m_pThread);
	}

	return bStatus;
}

unsigned int __stdcall CAsyncMailslotReader::ThreadStub(LPVOID data)
{
	{
		DEREF(data);

		CAsyncMailslotReader *pThis = (CAsyncMailslotReader *) pThread->UserData();
		assert(pThis);

		pThis->ThreadProc(pThread);
	}

	_endthreadex(0);

	//	Not reached
	return 0;
}

unsigned int CAsyncMailslotReader::ThreadProc(LPVOID data)
{
	CBaseThread *pThread = (CBaseThread *) data;
	BYTE		*pbMessage;
	DWORD		dwMessageLength = 0;

	assert(pThread);

	while (!pThread->Stop())
	{
		//	Get and dispatch messages
		pbMessage = Read(dwMessageLength);

		if (dwMessageLength)
			OnMessage(pbMessage, dwMessageLength);
	}

	return 0;
}
