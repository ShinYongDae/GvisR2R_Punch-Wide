/*
 *	$Header: $
 *
 *	$History: $
 */
#pragma once

#include <assert.h>

//	STL Includes
#include <string>
#include <bitset>
#include <vector>
#include <list>
#include <stack>
using namespace std;

#define DEREF(data) \
	CBaseThread *pThread = (CBaseThread *) data; \
	assert(pThread);

//	Forward declarations (only needed for the typedefs below)
class CBaseThread;

//	Typedefs for STL usage.
typedef list<CBaseThread *> THREADLIST;
typedef THREADLIST::const_iterator THREADITER;

typedef vector<HANDLE> HANDLEVECTOR;

class CBaseThread
{
public:
				CBaseThread(HANDLE hStopEvent, volatile bool *bStop, unsigned(__stdcall *thread)(void *), bool bWait = false, LPVOID data = NULL);
				~CBaseThread();

	bool		IsWaiting()	const		{ return m_bWaiting; }
	volatile bool Stop() const			{ return *m_bStop; }
	HANDLE		StopEvent() const		{ return m_hStopEvent; }
	HANDLE		ThreadHandle() const	{ return m_hThreadHandle; }
	LPVOID		UserData() const		{ return m_pvUserData; }

	virtual bool Wait(DWORD dwTimeout = INFINITE) const;
	bool		Run() const				{ return ResumeThread(m_hThreadHandle) == 1; }

	UINT		ThreadID() const		{ return m_uiThreadID; }

private:
	LPVOID		m_pvUserData;
	HANDLE		m_hStopEvent,
				m_hThreadHandle;
	volatile bool *m_bStop,
				m_bWaiting;
	UINT		m_uiThreadID;
};

class CSyncRendevouz
{
public:
				CSyncRendevouz(void);
				~CSyncRendevouz(void);

	void		Stop()					{ m_bStop = TRUE; SetEvent(m_hStopEvent); }
	virtual bool Wait(DWORD dwTimeout = INFINITE) const;

	bool		AddThread(unsigned(__stdcall *thread)(void *), bool bWait = false, LPVOID data = NULL);
	bool		AddHandle(HANDLE hHandle);

protected:
	HANDLEVECTOR m_handleArray;
	THREADLIST	m_threadList;
	HANDLE		m_hStopEvent;
	volatile bool m_bStop;
};

class CAsyncRendevouz : public CSyncRendevouz
{
public:
				CAsyncRendevouz(HWND wndTarget, UINT uiMsg, LPVOID pvUserData = NULL);
				~CAsyncRendevouz();

	virtual bool Wait(DWORD dwTimeout = INFINITE);

private:
	static unsigned __stdcall WaitProc(LPVOID data);

	HWND		m_wndTarget;
	UINT		m_uiMsg;
	DWORD		m_dwTimeout;
	LPVOID		m_pvUserData;
	CBaseThread *m_pThread;
};
