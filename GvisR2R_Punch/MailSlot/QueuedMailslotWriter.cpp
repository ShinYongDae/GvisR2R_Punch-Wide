/*
 *	$Header: $
 *
 *	$History: $
 */
#include "StdAfx.h"
#include "queuedmailslotwriter.h"

CQueuedMailslotWriter::CQueuedMailslotWriter(void)
{
	//	This event is signalled when it's time to destroy
	//	the object. It's used to stop the background thread
	m_hStopEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	//	This event is signalled whenever a message is added
	//	to the queue
	m_hSignalEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	m_bStop = false;

	//	Now set up our handle arrays. The first array is used in the thread to
	//	signal that a new message has arrived or that it's time to exit.
	m_haSignal[0] = m_hStopEvent;
	m_haSignal[1] = m_hSignalEvent;
	m_pThread = (CBaseThread *) NULL;
}

CQueuedMailslotWriter::~CQueuedMailslotWriter(void)
{
	StopThread();
	CloseHandle(m_hStopEvent);
	CloseHandle(m_hSignalEvent);
}

void CQueuedMailslotWriter::StopThread()
{
	if (m_pThread != (CBaseThread *) NULL)
	{
		m_bStop = true;
		SetEvent(m_hStopEvent);
		m_pThread->Wait();
		delete m_pThread;
		m_pThread = (CBaseThread *) NULL;
	}
}

bool CQueuedMailslotWriter::Connect(LPCTSTR szSlotName, LPCTSTR szServerName)
{
	//	Just in case we're calling Connect() again on an object
	//	already connected.
	StopThread();

	if (CSyncMailslotWriter::Connect(szSlotName, szServerName))
	{
		//	Now create our thread
		m_pThread = new CBaseThread(m_hStopEvent, &m_bStop, ThreadStub, false, this);
		assert(m_pThread);
		return true;
	}

	return false;
}

//	Writes a message to the mail slot. Actually it queues the message for the 
//	mail slot and leaves it to the background thread to actually do the write.
bool CQueuedMailslotWriter::Write(BYTE *pbData, DWORD dwDataLength, BOOL bImportant)
{
	assert(pbData);
	assert(dwDataLength);

	//	If the mail slot is closed attempt to reconnect to it
	if (!IsOpen() && m_pszSlotname != LPTSTR(NULL))
		CSyncMailslotWriter::Connect();

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
				//	High priority message, put it on the high priority queue
				m_highPriorityDataQueue.push_back(pqData);
			else
				//	Normal priority message, put it on the normal priority queue
				m_normalPriorityDataQueue.push_back(pqData);

			m_imMutex.ReleaseMutex();

			//	Now signal the queue handler thread...
			SetEvent(m_hSignalEvent);
			return true;
		}
	}

	return false;
}

//	This is the thread procedure. It dereferences the thread data
//	and calls the real ThreadProc through that pointer (this is so
//	that the thread procedure gets a valid 'this' pointer).
unsigned __stdcall CQueuedMailslotWriter::ThreadStub(LPVOID data)
{
	{
		//	Dereference the data pointer to get a pointer to our
		//	controlling mail slot client instance
		DEREF(data);
		CQueuedMailslotWriter *pThis = (CQueuedMailslotWriter *) pThread->UserData();
		assert(pThis);

		pThis->ThreadProc(pThread);
	}

	_endthreadex(0);

	//	Not reached
	return 0;
}

//	The real thread procedure, called from ThreadStub.
void CQueuedMailslotWriter::ThreadProc(CBaseThread *pThread)
{
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
					bQueuePriority = false;
				}
				else
				{
					pdqIterator = m_normalPriorityDataQueue.begin();
					bQueuePriority = true;
				}

				pqData = *pdqIterator;

				//	Done for now, release the mutex to give other threads
				//	a chance of queuing data.
				m_imMutex.ReleaseMutex();
				
				if (CSyncMailslotWriter::Write(pqData->Data(), pqData->Length()) == pqData->Length())
				{
					//	Now aquire the mutex again so we can remove the message from the queue
					if (m_imMutex.AquireMutex(m_hStopEvent) == CInterruptibleMutex::eMutexAquired)
					{
						//	The bQueuePriority flag tells us which queue we pulled the message
						//	from. We can't use the queue size here because the queues might have 
						//	changed in the time between now and when we pulled the message to be sent.
						if (bQueuePriority == false)
							m_highPriorityDataQueue.pop_front();
						else
							m_normalPriorityDataQueue.pop_front();

						delete pqData;
						continue;
					}
				}
				else
					//	Failed to write the message, leave it on the queue and
					//	break out of the writer loop
					break;
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

//	Internal data item construction
CQueuedMailslotWriter::CQueuedData::CQueuedData(BYTE *pbData, DWORD dwDataLength)
{
	assert(pbData);
	assert(dwDataLength);

	m_pbData = new BYTE[dwDataLength + 1];
	assert(m_pbData);
	memcpy(m_pbData, pbData, dwDataLength);
	m_pbData[dwDataLength] = 0;
	m_dwDataLength = dwDataLength;
}

CQueuedMailslotWriter::CQueuedData::~CQueuedData()
{
	delete [] m_pbData;
}
