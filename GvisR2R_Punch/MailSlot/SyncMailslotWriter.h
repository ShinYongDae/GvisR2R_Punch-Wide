/*
 *	$Header: $
 *
 *	$History: $
 */
#if !defined(AFX_SYNCMAILSLOTWWRITER_H__ADA1148A_6361_4A0C_AB3F_5A1C821615E5__INCLUDED_)
#define AFX_SYNCMAILSLOTWWRITER_H__ADA1148A_6361_4A0C_AB3F_5A1C821615E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MailSlots.h"

//	Mail slot writer class. Used to write to a mail slot.
//	The class creates an asynchronous mail slot handle which is 
//	used with overlapped I/O to write queued messages.
class CSyncMailslotWriter : public CMailslot
{
public:
					CSyncMailslotWriter();
	virtual			~CSyncMailslotWriter();

	virtual bool	Connect(LPCTSTR szSlot, LPCTSTR szServer = _T("."));

	virtual DWORD	Write(BYTE *pbData, DWORD dwDataLength);

protected:
	virtual bool	Connect();
};

#endif // !defined(AFX_SYNCMAILSLOTWWRITER_H__ADA1148A_6361_4A0C_AB3F_5A1C821615E5__INCLUDED_)
