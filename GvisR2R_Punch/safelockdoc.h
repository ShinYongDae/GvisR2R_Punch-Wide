// **************************************************************************
// safelockdoc.h
//
// Description:
//	An auto releasing synchronization object using MFC.
// **************************************************************************

#ifndef _SAFELOCKDOC_H
#define _SAFELOCKDOC_H

// **************************************************************************
class CSafeLockDoc
	{
	public:
		CSafeLockDoc(CSyncObject *pso)
			{
			ASSERT (pso);
			pso->Lock ();
			m_pso = pso;
			}

		~CSafeLockDoc()
			{
			m_pso->Unlock ();
			}
	
	private:
		CSyncObject *m_pso;
	};


#endif	// _SAFELOCKDOC_H
