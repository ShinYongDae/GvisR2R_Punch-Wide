/*
 *	$Header: $
 *
 *	$History: $
 */
#pragma once

class CInterruptibleMutex
{
public:
	enum eMutexState
	{
		eStopped,
		eMutexAquired,
		eTimedOut,
	};
					CInterruptibleMutex();
	virtual			~CInterruptibleMutex(void);

	bool			IsValid() const				{ return m_hMutex != INVALID_HANDLE_VALUE; }

	eMutexState		AquireMutex(HANDLE hStopEvent, DWORD dwTimeout = INFINITE);
	void			ReleaseMutex() const		{ ::ReleaseMutex(m_hMutex); }

private:
	HANDLE			m_hMutex;
};
