/*
*	$Header: $
*
*	$History: $
*/
#if !defined(AFX_QUEUEDMAILSLOTCLIENT_H__ADA1148A_6361_4A0C_AB3F_5A1C821615E5__INCLUDED_)
#define AFX_QUEUEDMAILSLOTCLIENT_H__ADA1148A_6361_4A0C_AB3F_5A1C821615E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SyncMailslotWriter.h"

class CAsyncMailslotWriter : public CSyncMailslotWriter
{
	class CQueuedData
	{
	public:
		CQueuedData(BYTE *pbData, DWORD dwDataLength);
		~CQueuedData();

		DWORD		Length() const		{ return m_dwDataLength; }
		BYTE		*Data()	const		{ return m_pbData; }

	private:
		BYTE		*m_pbData;
		DWORD		m_dwDataLength;
	};

	typedef deque<CQueuedData *> DATAQUEUE;
	typedef DATAQUEUE::const_iterator DQITER;

public:
					CAsyncMailslotWriter(void);
	virtual			~CAsyncMailslotWriter(void);

	virtual bool	Write(BYTE *pbData, DWORD dwDataLength, BOOL bImportant);

private:
	virtual bool	Connect();
	bool			WaitForConnection();
	static unsigned __stdcall ThreadStub(LPVOID data);
	virtual void	ThreadProc(CBaseThread *pThread);
	bool			Write(CQueuedData *pqData);

	HANDLE			m_hSignalEvent,
					m_hConnectedEvent,
					m_haConnected[2],
					m_haSignal[2];
	CInterruptibleMutex m_imMutex;
	CBaseThread		*m_pThread;
	volatile bool	m_bStop;
	DATAQUEUE		m_normalPriorityDataQueue,
					m_highPriorityDataQueue;
};

#endif // !defined(AFX_QUEUEDMAILSLOTCLIENT_H__ADA1148A_6361_4A0C_AB3F_5A1C821615E5__INCLUDED_)
