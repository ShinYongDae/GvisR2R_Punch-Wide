/*
 *	$Header: $
 *
 *	$History: $
 */
#include "StdAfx.h"
#include "AsyncMailslotWriter.h"

CAsyncMailslotWriter::CAsyncMailslotWriter(void)
{
	//	This event is signalled whenever a message is added
	//	to the queue
	m_hSignalEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	//	This event is signalled whenever we make a successful 
	//	connection to a mail slot.
	m_hConnectedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	m_bStop = false;

	//	Now set up our handle arrays. The first array is used in the thread to
	//	signal that a new message has arrived or that it's time to exit.
	m_haSignal[0] = m_hStopEvent;
	m_haSignal[1] = m_hSignalEvent;

	//	The m_haConnected array is used to signal the thread when a connection 
	//	has been made.
	m_haConnected[0] = m_hStopEvent;
	m_haConnected[1] = m_hConnectedEvent;

	//	Now create our thread
	m_pThread = new CBaseThread(m_hStopEvent, &m_bStop, ThreadStub, false, this);
	assert(m_pThread);
}

CAsyncMailslotWriter::~CAsyncMailslotWriter(void)
{
	if (m_pThread != (CBaseThread *) NULL)
	{
		m_bStop = true;
		SetEvent(m_hStopEvent);
		m_pThread->Wait();
		delete m_pThread;
	}

	CloseHandle(m_hConnectedEvent);
	CloseHandle(m_hSignalEvent);
}

bool CAsyncMailslotWriter::Connect()
{
	bool bResult = CSyncMailslotWriter::Connect();

	if (bResult)
		SetEvent(m_hConnectedEvent);

	return bResult;
}

//	Wait (possibly forever) for one of the handles in the m_haConnected array to
//	be signaled. Returns true if the signaled handle is the m_hConnectedEvent,
//	false if it's the stop handle.
bool CAsyncMailslotWriter::WaitForConnection()
{
	while (!m_bStop)
	{
		switch (WaitForMultipleObjects(2, m_haConnected, FALSE, 100))
		{
		case WAIT_OBJECT_0:
			return false;

		case WAIT_OBJECT_0 + 1:
			return true;

		case WAIT_TIMEOUT:
			if (Connect())
				return true;
		}
	}

	return false;
}

//	Writes a message to the mail slot. Actually it queues the message for the 
//	mail slot and leaves it to the background thread to actually do the write.
bool CAsyncMailslotWriter::Write(BYTE *pbData, DWORD dwDataLength, BOOL bImportant)
{
	assert(pbData);
	assert(dwDataLength);

	//	If the mail slot is closed attempt to reconnect to it
	if (!IsOpen() && m_pszSlotname != LPTSTR(NULL))
		Connect();

	if (IsOpen())
	{
		//	Grab the mutex first.  You must have the mutex before attempting to
		//	create the QueuedData object else you'll corrupt the program heap
		//	or deadlock on the heap lock.
		if (m_imMutex.AquireMutex(m_hStopEvent) == CInterruptibleMutex::eMutexAquired)
		{
			CQueuedData *pqData = new CQueuedData(pbData, dwDataLength);

			assert(pqData);

			if (bImportant)
				//	High priority message, put it at the head of the queue
				m_highPriorityDataQueue.push_back(pqData);
			else
				//	Normal priority message, put it at the end of the queue
				m_normalPriorityDataQueue.push_back(pqData);

			m_imMutex.ReleaseMutex();

			//	Now signal the queue handler thread...
			SetEvent(m_hSignalEvent);
			return true;
		}
	}

	return false;
}


bool CAsyncMailslotWriter::Write(CQueuedData *pqData)
{

	//	If the mail slot is closed attempt to reconnect to it
	if (!IsOpen() && m_pszSlotname != LPTSTR(NULL))
		Connect();

	if (IsOpen())
	{
		//	Grab the mutex first.  You must have the mutex before attempting to
		//	create the QueuedData object else you'll corrupt the program heap
		//	or deadlock on the heap lock.
		if (m_imMutex.AquireMutex(m_hStopEvent) == CInterruptibleMutex::eMutexAquired)
		{
			m_highPriorityDataQueue.push_back(pqData);

			m_imMutex.ReleaseMutex();

			//	Now signal the queue handler thread...
			SetEvent(m_hSignalEvent);
			return true;
		}
	}

	return false;
}

//	The real thread procedure, called from ThreadStub.
void CAsyncMailslotWriter::ThreadProc(CBaseThread *pThread)
{
	WaitForConnection();

	CQueuedData *pqData;
	DQITER		pdqIterator;
	bool		bQueuePriority;

	while (!pThread->Stop())
	{
		switch (WaitForMultipleObjects(2, m_haSignal, FALSE, INFINITE))
		{
		case WAIT_OBJECT_0:
			//	Told to stop, so stop
			break;

		case WAIT_OBJECT_0 + 1:
			//	Grab the mutex before we fall into the loop
			if (m_imMutex.AquireMutex(m_hStopEvent) != CInterruptibleMutex::eMutexAquired)
				//	Signalled to stop, so stop...
				break;

			//	New message added to the queue, send it
			while ((m_highPriorityDataQueue.size() || m_normalPriorityDataQueue.size()) && !pThread->Stop())
			{
				//	Keep looping until either the queue is empty or 
				//	we've been signalled to stop.
				if (m_highPriorityDataQueue.size())
				{
					pdqIterator = m_highPriorityDataQueue.begin();
					assert(pdqIterator != m_highPriorityDataQueue.end());
					bQueuePriority = false;
				}
				else
				{
					pdqIterator = m_normalPriorityDataQueue.begin();
					assert(pdqIterator != m_normalPriorityDataQueue.end());
					bQueuePriority = true;
				}

				pqData = *pdqIterator;

				//	Done for now, release the mutex to give other threads
				//	a chance of queuing data.
				m_imMutex.ReleaseMutex();
				Write(pqData);

				//	Now aquire the mutex again so we can remove the message from the queue
				if (m_imMutex.AquireMutex(m_hStopEvent) == CInterruptibleMutex::eMutexAquired)
				{
					//	The bQueuePriority flag tells us which queue we pulled the message
					//	from. We can't use the queue size here because the queues might have
					//	changed in the time between now and when we pulled the message to be
					//	sent.
					if (bQueuePriority == false)
						m_highPriorityDataQueue.pop_front();
					else
						m_normalPriorityDataQueue.pop_front();

					delete pqData;
				}
			}

			//	Finished the loop, so release the mutex.
			m_imMutex.ReleaseMutex();
			break;
		}
	}

	//	Make sure we cancel any pending I/O before exiting the thread.
	if (IsOpen())
		CancelIo(m_hMailSlot);
}


//	This is the thread procedure. It dereferences the thread data
//	and calls the real ThreadProc through that pointer (this is so
//	that the thread procedure gets a valid 'this' pointer).
unsigned __stdcall CAsyncMailslotWriter::ThreadStub(LPVOID data)
{
	{
		//	Dereference the data pointer to get a pointer to our
		//	controlling mail slot client instance
		DEREF(data);
		CAsyncMailslotWriter *pThis = (CAsyncMailslotWriter *) pThread->UserData();
		assert(pThis);

		pThis->ThreadProc(pThread);
	}

	return 0;
}

//	Internal data item construction
CAsyncMailslotWriter::CQueuedData::CQueuedData(BYTE *pbData, DWORD dwDataLength)
{
	assert(pbData);
	assert(dwDataLength);

	m_pbData = new BYTE[dwDataLength + 1];
	assert(m_pbData);
	memcpy(m_pbData, pbData, dwDataLength);
	m_pbData[dwDataLength] = 0;
	m_dwDataLength = dwDataLength;
}

CAsyncMailslotWriter::CQueuedData::~CQueuedData()
{
	delete [] m_pbData;
}
