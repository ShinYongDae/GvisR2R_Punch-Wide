// Yield.cpp: implementation of the CYield class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "../stdafx.h"
//#include "../gvisr2r_punch.h"
#include "Yield.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "../MainFrm.h"
#include "../GvisR2R_PunchDoc.h"
#include "../GvisR2R_PunchView.h"

extern CMainFrame* pFrm;
extern CGvisR2R_PunchDoc* pDoc;
extern CGvisR2R_PunchView* pView;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CYield::CYield()
{
	bValid = FALSE;
	nCol=0; nRow=0;
	m_nSerial=0;
	dYieldAll = 0.0;
	for(int i=0; i < MAX_STRIP_NUM; i++)
	{ 
		dYield[i]=0.0;
		nTotDef[i]=0;
	}

	nTot=0;
	nAllDef=0;
}

CYield::~CYield()
{
	bValid = FALSE;
}

void CYield::SetDef(int nSerial, int *pDef)	// int pDef[4];
{
	if(nSerial <= 0)
	{
		AfxMessageBox(_T("Serial Error.66"));
		return;
	}

	int nMaxStrip = MAX_STRIP;
#ifdef USE_CAM_MASTER
	if(pView->m_mgrReelmap)
		nMaxStrip = pView->m_mgrReelmap->m_Master[0].GetStripNum(); // ÃÑ ½ºÆ®¸³ÀÇ °¹¼ö
#else
	nMaxStrip = MAX_STRIP;
#endif

	if(nSerial>m_nSerial)
	{
		bValid = TRUE;
		m_nSerial = nSerial;
		nTot = nCol*nRow*m_nSerial;
		nAllDef=0;
		int nTotSt = nTot / nMaxStrip;
		for(int i=0; i < nMaxStrip; i++)
		{
			nTotDef[i] += pDef[i];
			dYield[i] = 100.0*(double)(nTotSt-nTotDef[i])/(double)nTotSt;
			nAllDef += nTotDef[i];
		}
		dYieldAll = 100.0*(double)(nTot-nAllDef)/(double)nTot;
	}
}

BOOL CYield::IsValid()
{
	return bValid;
}

double CYield::GetYield()
{
	return dYieldAll;
}

double CYield::GetYield(int nStrip)
{
	return dYield[nStrip];
}

int CYield::GetDef()
{
	return nAllDef;
}

int CYield::GetDef(int nStrip)
{
	return nTotDef[nStrip];
}

int CYield::GetGood()
{
	return ((int)(nTot-nAllDef));
}

int CYield::GetGood(int nStrip)
{
	int nMaxStrip = MAX_STRIP;
#ifdef USE_CAM_MASTER
	if(pView->m_mgrReelmap)
		nMaxStrip = pView->m_mgrReelmap->m_Master[0].GetStripNum(); // ÃÑ ½ºÆ®¸³ÀÇ °¹¼ö
#else
	nMaxStrip = MAX_STRIP;
#endif
	int nTotSt = nTot / nMaxStrip;
	return ((int)(nTotSt-nTotDef[nStrip]));
}