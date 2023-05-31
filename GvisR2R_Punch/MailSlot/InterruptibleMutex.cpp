/*
 *	$Header: $
 *
 *	$History: $
 */
#include "stdafx.h"
#include <assert.h>
#include "InterruptibleMutex.h"

CInterruptibleMutex::CInterruptibleMutex()
{
	m_hMutex = CreateMutex(NULL, FALSE, NULL);
}

CInterruptibleMutex::~CInterruptibleMutex(void)
{
	if (m_hMutex != INVALID_HANDLE_VALUE)
		CloseHandle(m_hMutex);
}

CInterruptibleMutex::eMutexState CInterruptibleMutex::AquireMutex(HANDLE hStopEvent, DWORD dwTimeout)
{
	assert(IsValid());

	HANDLE hArray[2] = { hStopEvent, m_hMutex };

	switch (WaitForMultipleObjects(2, hArray, FALSE, dwTimeout))
	{
	case WAIT_OBJECT_0:
		return eStopped;

	case WAIT_OBJECT_0 + 1:
		return eMutexAquired;

	default:
		return eTimedOut;
	}
}
