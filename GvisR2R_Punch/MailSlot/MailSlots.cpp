/*
 *	$Header: $
 *
 *	$History: $
 */
#include "stdafx.h"
#include "Mailslots.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif

// Construction/Destruction
CMailslot::CMailslot()
{
	m_bStop = false;
	m_hMailSlot = INVALID_HANDLE_VALUE;

	//	This event is signalled when it's time for the class
	//	to terminate.
	m_hStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_pszSlotname = LPTSTR(NULL);
}

CMailslot::~CMailslot()
{
	Disconnect();
	CloseHandle(m_hStopEvent);
	delete m_pszSlotname;
}

// Closes a connection to a mail slot.
void CMailslot::Disconnect()
{
	if (m_hMailSlot != INVALID_HANDLE_VALUE)
		CloseHandle(m_hMailSlot);

	m_hMailSlot = INVALID_HANDLE_VALUE;
}
