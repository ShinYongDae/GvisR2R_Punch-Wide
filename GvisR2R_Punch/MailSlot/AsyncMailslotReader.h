/*
 *	$Header: $
 *
 *	$History: $
 */
#if !defined(AFX_ASYNCMAILSLOTREADER_H__4409B73C_561B_428E_B839_1F32CD9B82F7__INCLUDED_)
#define AFX_ASYNCMAILSLOTREADER_H__4409B73C_561B_428E_B839_1F32CD9B82F7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MailSlots.h"
#include "SyncMailslotReader.h"

class CAsyncMailslotReader : public CSyncMailslotReader
{
public:
					CAsyncMailslotReader();
	virtual			~CAsyncMailslotReader();

	virtual bool	OnMessage(BYTE *pbMessage, DWORD dwMessageLength) = 0;
	virtual bool	Connect(LPCTSTR szSlotName);

protected:
	static unsigned int __stdcall ThreadStub(LPVOID data);
	unsigned int	ThreadProc(LPVOID data);

	CBaseThread		*m_pThread;
};

#endif // !defined(AFX_ASYNCMAILSLOTREADER_H__4409B73C_561B_428E_B839_1F32CD9B82F7__INCLUDED_)
