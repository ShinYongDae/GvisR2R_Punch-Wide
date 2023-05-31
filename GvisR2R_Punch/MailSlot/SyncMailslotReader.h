/*
 *	$Header: $
 *
 *	$History: $
 */
#if !defined(AFX_SYNCMAILSLOTREADER_H__90480C03_0DF5_48F4_9613_4A4FE9F6CE60__INCLUDED_)
#define AFX_SYNCMAILSLOTREADER_H__90480C03_0DF5_48F4_9613_4A4FE9F6CE60__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MailSlots.h"

//	Mail slot reader class. Used to read from a mail slot.
class CSyncMailslotReader : public CMailslot
{
public:
					CSyncMailslotReader();
	virtual			~CSyncMailslotReader();

	virtual bool	Connect(LPCTSTR szSlotname, LPCTSTR szServer = _T("."));

	BYTE 			*Read(DWORD& dwBufferLength);
	DWORD			GetMessageCount(DWORD *pdwNextMessageLength = (DWORD *) NULL);
};

#endif // !defined(AFX_SYNCMAILSLOTREADER_H__90480C03_0DF5_48F4_9613_4A4FE9F6CE60__INCLUDED_)
