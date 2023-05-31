/*
 *	$Header: $
 *
 *	$History: $
 */
#if !defined(AFX_MAILSLOTS_H__ADA1148A_6361_4A0C_AB3F_5A1C821615E5__INCLUDED_)
#define AFX_MAILSLOTS_H__ADA1148A_6361_4A0C_AB3F_5A1C821615E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseThread.h"
#include "InterruptibleMutex.h"
#include "OverlappedIO.h"

//	Base mail slot class, contains stuff common to servers and clients
class CMailslot
{
protected:
					CMailslot();
	virtual			~CMailslot();

	virtual bool	Connect(LPCTSTR szSlotName, LPCTSTR szServerName = _T(".")) = 0;

public:
	virtual void	Disconnect();

	bool			IsOpen() const			{ return m_hMailSlot != INVALID_HANDLE_VALUE; }

protected:
	HANDLE			m_hMailSlot,
					m_hStopEvent;
	bool			m_bStop;
	LPTSTR			m_pszSlotname;
	COverlappedIO	m_overlapped;
};

#endif // !defined(AFX_MAILSLOTS_H__ADA1148A_6361_4A0C_AB3F_5A1C821615E5__INCLUDED_)
