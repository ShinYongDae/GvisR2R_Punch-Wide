/*
 *	$Header: $
 *
 *	$History: $
 */
#include "StdAfx.h"
#include "BaseThread.h"

CBaseThread::CBaseThread(HANDLE hStopEvent, volatile bool *bStop, unsigned(__stdcall *thread)(void *), bool bWait, LPVOID data)
{
	assert(bStop);
	assert(thread);

	m_bStop = bStop;
	m_hStopEvent = hStopEvent;
	m_pvUserData = data;
	m_bWaiting = bWait;
	m_hThreadHandle = HANDLE(_beginthreadex(NULL, 0, thread, this, bWait ? CREATE_SUSPENDED : 0, &m_uiThreadID));
}

CBaseThread::~CBaseThread()
{
	CloseHandle(m_hThreadHandle);
}

bool CBaseThread::Wait(DWORD dwTimeout) const
{
	Run();
	return WaitForSingleObject(m_hThreadHandle, dwTimeout) != WAIT_TIMEOUT;
}

CSyncRendevouz::CSyncRendevouz(void)
{
	m_bStop = FALSE;
	m_hStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

CSyncRendevouz::~CSyncRendevouz(void)
{
	CBaseThread *pThread;
	THREADITER  pThreads;

	//	Stop all threads under our control and make
	//	sure they've exited.
	Stop();
	Wait();

	//	Now release resources
	CloseHandle(m_hStopEvent);

	//	And delete each thread object
	for (pThreads = m_threadList.begin(); pThreads != m_threadList.end(); pThreads++)
	{
		pThread = *pThreads;
		assert(pThread);
		delete pThread;
	}
}

bool CSyncRendevouz::AddThread(unsigned(__stdcall *thread)(void *), volatile bool bWait, LPVOID data)
{
	if (m_handleArray.size() > MAXIMUM_WAIT_OBJECTS - 1)
		return false;

	assert(thread);

	CBaseThread *pThread = new CBaseThread(m_hStopEvent, &m_bStop, thread, bWait, data);

	assert(pThread);
	m_threadList.push_back(pThread);
	m_handleArray.push_back(pThread->ThreadHandle());
	return true;
}

bool CSyncRendevouz::AddHandle(HANDLE hHandle)
{
	assert(hHandle);

	if (m_handleArray.size() > MAXIMUM_WAIT_OBJECTS - 1)
		return false;

	m_handleArray.push_back(hHandle);
	return true;
}

bool CSyncRendevouz::Wait(DWORD dwTimeout) const
{
	CBaseThread *pThread;
	THREADITER  pThreads = m_threadList.begin();

	while (pThreads != m_threadList.end())
	{
		//	Iterate over the thread list starting any threads
		//	that were created suspended...
		pThread = *pThreads++;

		assert(pThread);

		if (pThread->IsWaiting())
			pThread->Run();
	}

	//	Now that all threads are running it's time to wait on em...
	return WaitForMultipleObjects(DWORD(m_handleArray.size()), &m_handleArray[0], TRUE, dwTimeout) != WAIT_TIMEOUT;
}

CAsyncRendevouz::CAsyncRendevouz(HWND wndTarget, UINT uiMsg, LPVOID pvUserData) : CSyncRendevouz()
{
	assert(wndTarget);
	assert(IsWindow(wndTarget));

	m_wndTarget = wndTarget;
	m_uiMsg = uiMsg;
	m_pvUserData = pvUserData;
	m_pThread = (CBaseThread *) NULL;
}

CAsyncRendevouz::~CAsyncRendevouz()
{
	delete m_pThread;
}

bool CAsyncRendevouz::Wait(DWORD dwTimeout)
{
	m_dwTimeout = dwTimeout;
	m_pThread = new CBaseThread(m_hStopEvent, &m_bStop, WaitProc, 0, LPVOID(this));
	return TRUE;
}

unsigned __stdcall CAsyncRendevouz::WaitProc(LPVOID data)
{
	{
		DEREF(data);

		CAsyncRendevouz *pThis = (CAsyncRendevouz *) pThread->UserData();

		assert(pThis);

		bool bResult = pThis->CSyncRendevouz::Wait(pThis->m_dwTimeout);

		if (IsWindow(pThis->m_wndTarget))
			::PostMessage(pThis->m_wndTarget, pThis->m_uiMsg, WPARAM(bResult), LPARAM(pThis->m_pvUserData));
	}

	_endthreadex(0);

	//	Not reached
	return 0;
}
