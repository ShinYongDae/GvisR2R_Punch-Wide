/*
*	$Header: $
*
*	$History: $
*/
#if !defined(AFX_QUEUEDMAILSLOTWRITER_H__ADA1148A_6361_4A0C_AB3F_5A1C821615E5__INCLUDED_)
#define AFX_QUEUEDMAILSLOTWRITER_H__ADA1148A_6361_4A0C_AB3F_5A1C821615E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "syncmailslotwriter.h"

class CQueuedMailslotWriter : public CSyncMailslotWriter
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
					CQueuedMailslotWriter(void);
	virtual			~CQueuedMailslotWriter(void);

	virtual bool	Write(BYTE *pbData, DWORD dwDataLength, BOOL bImportant);
	virtual bool	Connect(LPCTSTR szSlotname, LPCTSTR szServername = _T("."));

private:
	static unsigned __stdcall ThreadStub(LPVOID data);
	virtual void	ThreadProc(CBaseThread *pThread);
	void			StopThread();

	HANDLE			m_hStopEvent,
					m_hSignalEvent,
					m_haSignal[2];
	CInterruptibleMutex m_imMutex;
	CBaseThread		*m_pThread;
	volatile bool	m_bStop;
	DATAQUEUE		m_highPriorityDataQueue,
					m_normalPriorityDataQueue;
};

#endif // !defined(AFX_QUEUEDMAILSLOTWRITER_H__ADA1148A_6361_4A0C_AB3F_5A1C821615E5__INCLUDED_)
