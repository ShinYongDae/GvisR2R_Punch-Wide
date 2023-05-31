/*
 *	$Header: $
 *
 *	$History: $
 */
#include "StdAfx.h"
#include "overlappedio.h"

COverlappedIO::COverlappedIO()
{
	m_hIO = INVALID_HANDLE_VALUE;

	//	Initialise our overlapped structure and create the event
	memset(&m_op, 0, sizeof(m_op));
	m_op.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}

COverlappedIO::~COverlappedIO(void)
{
	CloseHandle(m_op.hEvent);
}

void COverlappedIO::Attach(HANDLE hIO)
{
	m_hIO = hIO;

	//	Reset the OVERLAPPED structure (but don't kill the event handle)
	m_op.Internal = m_op.InternalHigh = m_op.Offset = m_op.OffsetHigh = 0;
}

bool COverlappedIO::Read(BYTE *pbData, DWORD dwBufferSize, LPDWORD pdwBytesRead, HANDLE hStopEvent)
{
	assert(pbData);
	assert(dwBufferSize);
	assert(pdwBytesRead);

	if (m_hIO != INVALID_HANDLE_VALUE)
	{
		::ReadFile(m_hIO, pbData, dwBufferSize, pdwBytesRead, &m_op);

		switch (GetLastError())
		{
		case 0:
		case ERROR_IO_PENDING:
			break;

		default:
			return false;
		}

		return Wait(pdwBytesRead, hStopEvent);
	}

	//	Something went wrong
	return false;
}

bool COverlappedIO::Write(BYTE *pbData, DWORD dwBufferSize, LPDWORD pdwBytesWritten, HANDLE hStopEvent)
{
	assert(pbData);
	assert(dwBufferSize);
	assert(pdwBytesWritten);

	if (m_hIO != INVALID_HANDLE_VALUE)
	{
		::WriteFile(m_hIO, pbData, dwBufferSize, pdwBytesWritten, &m_op);

		switch (GetLastError())
		{
		case 0:
		case ERROR_IO_PENDING:
			break;

		default:
			return false;
		}

		return Wait(pdwBytesWritten, hStopEvent);
	}

	//	Something went wrong
	return false;
}

bool COverlappedIO::Wait(LPDWORD pdwBytesTransferred, HANDLE hStopEvent)
{
	HANDLE haOverlapped[2] = { m_op.hEvent, hStopEvent };

	if (m_hIO != INVALID_HANDLE_VALUE)
	{
		if (WaitForMultipleObjects(hStopEvent == INVALID_HANDLE_VALUE ? 1 : 2, haOverlapped, FALSE, INFINITE) == WAIT_OBJECT_0)
			if (GetOverlappedResult(m_hIO, &m_op, pdwBytesTransferred, FALSE))
				return true;
	}

	return false;
}
