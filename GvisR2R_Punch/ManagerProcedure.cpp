// ManagerProcedure.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "GvisR2R_Punch.h"
#include "ManagerProcedure.h"

#include "Process/DataFile.h"

#include "MainFrm.h"
#include "GvisR2R_PunchDoc.h"
#include "GvisR2R_PunchView.h"

extern CMainFrame* pFrm;
extern CGvisR2R_PunchDoc* pDoc;
extern CGvisR2R_PunchView* pView;

// CManagerProcedure

IMPLEMENT_DYNAMIC(CManagerProcedure, CWnd)

CManagerProcedure::CManagerProcedure(CWnd* pParent /*=NULL*/)
{
	m_pParent = pParent;
	m_bShift2Mk = FALSE;

	m_bContEngraveF = FALSE;

	m_nSaveMk0Img = 0;
	m_nSaveMk1Img = 0;

	m_bStopF_Verify = FALSE;
	m_bInitAuto = TRUE;
	m_bInitAutoLoadMstInfo = FALSE;

	m_bBufEmpty[0] = FALSE;
	m_bBufEmpty[1] = FALSE;

	m_nDebugStep = 0;
	m_nNewLot = 0;

	m_nRtnMyMsgBoxIdx = -1;
	m_bDispMyMsgBox = FALSE;

	m_nSelRmap = RMAP_UP;
	m_nSelRmapInner = RMAP_INNER_UP;

	m_nLotEndSerial = 0;

	m_nDummy[0] = 0;
	m_nDummy[1] = 0;
	m_nAoiLastSerial[0] = 0;
	m_nAoiLastSerial[1] = 0;
	m_bChkLastProcVs = FALSE;

	m_dTempPauseLen = 0.0;
	m_dElecChkVal = 0.0;

	m_bInit = FALSE;
	m_sFixMsg[0] = _T("");
	m_sFixMsg[1] = _T("");

	m_bStopFeeding = FALSE;

	m_bAoiFdWriteF[0] = FALSE;
	m_bAoiFdWriteF[1] = FALSE;
	m_bAoiTest[0] = FALSE;
	m_bAoiTest[1] = FALSE;

	m_bEngFdWrite = FALSE;
	m_bEngFdWriteF = FALSE;
	m_bEngTest = FALSE;
	m_bEngTestF = FALSE;

	m_bTIM_CHK_DONE_READY = FALSE;
	
	m_nStepAuto = 0;
	m_nPrevStepAuto = 0;
	m_nPrevMkStAuto = 0;
	m_sTick = _T("");
	m_sDispTime = _T("");

	m_bMkTmpStop = FALSE;
	m_bAoiLdRun = TRUE;
	m_bAoiLdRunF = FALSE;

	m_dwCycSt = 0;
	m_dwCycTim = 0;

	m_bTHREAD_DISP_DEF = FALSE;
	m_nStepTHREAD_DISP_DEF = 0;
	m_bTHREAD_DISP_DEF_INNER = FALSE;
	m_nStepTHREAD_DISP_DEF_INNER = 0;
	m_bTHREAD_SHIFT2MK = FALSE;

	// DispDefImg
	m_bThread[0] = FALSE;
	m_dwThreadTick[0] = 0;

	// Engrave Auto Sequence - Response Check
	m_bThread[1] = FALSE;
	m_dwThreadTick[1] = 0;

	// DispDefImgInner
	m_bThread[2] = FALSE;
	m_dwThreadTick[2] = 0;

	// DoShift2Mk
	m_bThread[3] = FALSE;
	m_dwThreadTick[3] = 0;

	m_bContEngraveF = FALSE;

	m_nSaveMk0Img = 0;
	m_nSaveMk1Img = 0;

	m_bStopF_Verify = FALSE;
	m_bInitAuto = TRUE;
	m_bInitAutoLoadMstInfo = FALSE;

	m_bSerialDecrese = FALSE;
	m_nErrCnt = 0;

	m_bNewModel = FALSE;
	m_dTotVel = 0.0; m_dPartVel = 0.0;
	m_bTIM_CAMMASTER_UPDATE = FALSE;
	m_sMyMsg = _T("");
	m_nTypeMyMsg = IDOK;

	m_dwLotSt = 0; m_dwLotEd = 0;
	m_lFuncId = 0;

	m_bCont = FALSE;

	m_bChkBufIdx[0] = TRUE;
	m_nChkBufIdx[0] = 0;
	m_bChkBufIdx[1] = TRUE;
	m_nChkBufIdx[1] = 0;

	m_dwStMkDn[0] = 0;
	m_dwStMkDn[1] = 0;
	m_nVsBufLastSerial[0] = 0;
	m_nVsBufLastSerial[1] = 0;
	m_bShowModalMyPassword = FALSE;
	m_nRstNum = 0;
	m_bBufHomeDone = FALSE;
	m_bReadyDone = FALSE;

	m_Flag = 0L;
	m_AoiLdRun = 0L;

	m_bDoneDispMkInfo[0][0] = FALSE; // Cam0, Up
	m_bDoneDispMkInfo[0][1] = FALSE; // Cam0, Dn
	m_bDoneDispMkInfo[1][0] = FALSE; // Cam1, Up
	m_bDoneDispMkInfo[1][1] = FALSE; // Cam1, Dn

	m_nShareUpS = 0; m_nShareUpSprev = 0;
	m_nShareUpSerial[0] = 0;
	m_nShareUpSerial[1] = 0;
	m_nShareUpCnt = 0;

	m_nShareDnS = 0; m_nShareDnSprev = 0;
	m_nShareDnSerial[0] = 0;
	m_nShareDnSerial[1] = 0;
	m_nShareDnCnt = 0;

	m_nBufSerial[0][0] = 0; // Up-Cam0
	m_nBufSerial[0][1] = 0; // Up-Cam1
	m_nBufSerial[1][0] = 0; // Dn-Cam0
	m_nBufSerial[1][1] = 0; // Dn-Cam0

	m_bReAlign[0][0] = FALSE; // [nCam][nPos] 
	m_bReAlign[0][1] = FALSE; // [nCam][nPos] 
	m_bReAlign[0][2] = FALSE; // [nCam][nPos] 
	m_bReAlign[0][3] = FALSE; // [nCam][nPos] 
	m_bReAlign[1][0] = FALSE; // [nCam][nPos] 
	m_bReAlign[1][1] = FALSE; // [nCam][nPos] 
	m_bReAlign[1][2] = FALSE; // [nCam][nPos] 
	m_bReAlign[1][3] = FALSE; // [nCam][nPos] 

	m_bSkipAlign[0][0] = FALSE; // [nCam][nPos] 
	m_bSkipAlign[0][1] = FALSE; // [nCam][nPos] 
	m_bSkipAlign[0][2] = FALSE; // [nCam][nPos] 
	m_bSkipAlign[0][3] = FALSE; // [nCam][nPos] 
	m_bSkipAlign[1][0] = FALSE; // [nCam][nPos] 
	m_bSkipAlign[1][1] = FALSE; // [nCam][nPos] 
	m_bSkipAlign[1][2] = FALSE; // [nCam][nPos] 
	m_bSkipAlign[1][3] = FALSE; // [nCam][nPos] 

	m_bFailAlign[0][0] = FALSE; // [nCam][nPos] 
	m_bFailAlign[0][1] = FALSE; // [nCam][nPos] 
	m_bFailAlign[0][2] = FALSE; // [nCam][nPos] 
	m_bFailAlign[0][3] = FALSE; // [nCam][nPos] 
	m_bFailAlign[1][0] = FALSE; // [nCam][nPos] 
	m_bFailAlign[1][1] = FALSE; // [nCam][nPos] 
	m_bFailAlign[1][2] = FALSE; // [nCam][nPos] 
	m_bFailAlign[1][3] = FALSE; // [nCam][nPos] 

	m_nMonAlmF = 0;
	m_nClrAlmF = 0;

	m_bMkSt = FALSE;
	m_bMkStSw = FALSE;
	m_nMkStAuto = 0;

	m_bEngSt = FALSE;
	m_bEngStSw = FALSE;
	m_nEngStAuto = FALSE;

	m_bEng2dSt = FALSE;
	m_bEng2dStSw = FALSE;
	m_nEng2dStAuto = 0;

	m_bLotEnd = FALSE;
	m_nLotEndAuto = 0;

	m_bLastProc = FALSE;
	m_bLastProcFromUp = TRUE;
	m_bLastProcFromEng = TRUE;
	m_nLastProcAuto = 0;

	m_bLoadShare[0] = FALSE;
	m_bLoadShare[1] = FALSE;

	m_sNewLotUp = _T("");
	m_sNewLotDn = _T("");

	m_bAoiFdWrite[0] = FALSE;
	m_bAoiFdWrite[1] = FALSE;
	m_bAoiFdWriteF[0] = FALSE;
	m_bAoiFdWriteF[1] = FALSE;

	m_bReMk = FALSE;
	m_bWaitPcr[0] = FALSE;
	m_bWaitPcr[1] = FALSE;

	m_bShowMyMsg = FALSE;
	m_bContDiffLot = FALSE;

	for (int nAns = 0; nAns < 10; nAns++)
		m_bAnswer[nAns] = FALSE;

	m_bChkLightErr = FALSE;

	TCHAR szData[200];
	CString sVal, sPath = PATH_WORKING_INFO;

	if (0 < ::GetPrivateProfileString(_T("Last Job"), _T("MkSt"), NULL, szData, sizeof(szData), sPath))
		m_bMkSt = _ttoi(szData) > 0 ? TRUE : FALSE;
	else
		m_bMkSt = FALSE;

	if (0 < ::GetPrivateProfileString(_T("Last Job"), _T("MkStAuto"), NULL, szData, sizeof(szData), sPath))
		m_nMkStAuto = _ttoi(szData);
	else
		m_nMkStAuto = 0;

	RECT rt = { 0,0,0,0 };
	if (!Create(NULL, NULL, WS_CHILD, rt, pParent, 0))
		AfxMessageBox(_T("CManagerProcedure::Create() Failed!!!"));

	InitVal();
	InitThread();
}

CManagerProcedure::~CManagerProcedure()
{
	m_bTIM_CHK_DONE_READY = FALSE;
	Sleep(100);
	KillThread();
}


BEGIN_MESSAGE_MAP(CManagerProcedure, CWnd)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CManagerProcedure �޽��� ó�����Դϴ�.

void CManagerProcedure::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CWnd::OnTimer(nIDEvent);
}

void CManagerProcedure::InitVal()
{
	m_bTHREAD_UPDATAE_YIELD[0] = FALSE;
	m_bTHREAD_UPDATAE_YIELD[1] = FALSE;
	m_nSerialTHREAD_UPDATAE_YIELD[0] = 0;
	m_nSerialTHREAD_UPDATAE_YIELD[1] = 0;

	m_bTHREAD_UPDATE_REELMAP_UP = FALSE;
	m_bTHREAD_UPDATE_REELMAP_ALLUP = FALSE;
	m_bTHREAD_UPDATE_REELMAP_DN = FALSE;
	m_bTHREAD_UPDATE_REELMAP_ALLDN = FALSE;
	m_bTHREAD_UPDATE_REELMAP_ITS = FALSE;
	m_bTHREAD_UPDATE_REELMAP_INNER_UP = FALSE;
	m_bTHREAD_UPDATE_REELMAP_INNER_ALLUP = FALSE;
	m_bTHREAD_UPDATE_REELMAP_INNER_DN = FALSE;
	m_bTHREAD_UPDATE_REELMAP_INNER_ALLDN = FALSE;

	m_bTHREAD_REELMAP_YIELD_UP = FALSE;
	m_bTHREAD_REELMAP_YIELD_ALLUP = FALSE;
	m_bTHREAD_REELMAP_YIELD_DN = FALSE;
	m_bTHREAD_REELMAP_YIELD_ALLDN = FALSE;
	m_bTHREAD_REELMAP_YIELD_ITS = FALSE;

	m_nSnTHREAD_UPDATAE_YIELD = -1;
	m_bTHREAD_UPDATE_YIELD_UP = FALSE;
	m_bTHREAD_UPDATE_YIELD_DN = FALSE;
	m_bTHREAD_UPDATE_YIELD_ALLUP = FALSE;
	m_bTHREAD_UPDATE_YIELD_ALLDN = FALSE;

	m_bTHREAD_UPDATE_YIELD_ITS = FALSE;
	m_bTHREAD_UPDATE_YIELD_INNER_UP = FALSE;
	m_bTHREAD_UPDATE_YIELD_INNER_DN = FALSE;
	m_bTHREAD_UPDATE_YIELD_INNER_ALLUP = FALSE;
	m_bTHREAD_UPDATE_YIELD_INNER_ALLDN = FALSE;
}

void CManagerProcedure::ChkBuf()
{
	if (!m_bShift2Mk)
		ChkBufUp();
	if (!m_bShift2Mk)
		ChkBufDn();
}

void CManagerProcedure::ChkBufUp()
{
	CString str, sTemp;

	str = _T("UB: ");
	if (ChkBufUp(m_pBufSerial[0], m_nBufTot[0]))
	{
		for (int i = 0; i < m_nBufTot[0]; i++)
		{
			if (m_bShift2Mk)
				return;

			DelOverLotEndSerialUp(m_pBufSerial[0][i]);

			if (i == m_nBufTot[0] - 1)
				sTemp.Format(_T("%d"), m_pBufSerial[0][i]);
			else
				sTemp.Format(_T("%d,"), m_pBufSerial[0][i]);
			str += sTemp;
		}
	}
	else
	{
		m_nBufTot[0] = 0;
	}

	if (pFrm)
	{
		if (m_sBuf[0] != str)
		{
			m_sBuf[0] = str;
			pFrm->DispStatusBar(str, 3);

			pDoc->SetCurrentInfoBufUpTot(m_nBufTot[0]);
			for (int k = 0; k < m_nBufTot[0]; k++)
				pDoc->SetCurrentInfoBufUp(k, m_pBufSerial[0][k]);

			if (m_nBufTot[0] == 1)
			{
				m_nAoiCamInfoStrPcs[0] = pView->GetAoiUpCamMstInfo();
			}
		}
	}
}

void CManagerProcedure::ChkBufDn()
{
	CString str, sTemp;

	str = _T("DB: ");
	if (ChkBufDn(m_pBufSerial[1], m_nBufTot[1]))
	{
		for (int i = 0; i < m_nBufTot[1]; i++)
		{
			if (m_bShift2Mk)
				return;

			DelOverLotEndSerialDn(m_pBufSerial[1][i]);

			if (i == m_nBufTot[1] - 1)
				sTemp.Format(_T("%d"), m_pBufSerial[1][i]);
			else
				sTemp.Format(_T("%d,"), m_pBufSerial[1][i]);
			str += sTemp;
		}
	}
	else
	{
		m_nBufTot[1] = 0;
	}

	if (pFrm)
	{
		if (m_sBuf[1] != str)
		{
			m_sBuf[1] = str;
			pFrm->DispStatusBar(str, 1);

			pDoc->SetCurrentInfoBufDnTot(m_nBufTot[1]);
			for (int k = 0; k < m_nBufTot[1]; k++)
				pDoc->SetCurrentInfoBufDn(k, m_pBufSerial[1][k]);

			if (m_nBufTot[1] == 1)
			{
				m_nAoiCamInfoStrPcs[1] = pView->GetAoiDnCamMstInfo();
			}
		}
	}
}

void CManagerProcedure::ChkShare()
{
	ChkShareUp();
	ChkShareDn();
}

void CManagerProcedure::ChkShareUp()
{
	CString str, str2;
	int nSerial;
	if (ChkShareUp(nSerial))
	{
		str.Format(_T("US: %d"), nSerial);
		pDoc->Status.PcrShare[0].bExist = TRUE;
		pDoc->Status.PcrShare[0].nSerial = nSerial;
		if (pView)
		{
			str2.Format(_T("PCR���� Received - US: %d"), nSerial);
			pDoc->Log(str2);
			pView->MpeWrite(_T("ML45112"), (long)nSerial);	// �˻��� Panel�� AOI �� Serial
			pView->MpeWrite(_T("MB44012B"), 1);				// AOI �� : PCR���� Received
		}
	}
	else
	{
		pDoc->Status.PcrShare[0].bExist = FALSE;
		pDoc->Status.PcrShare[0].nSerial = -1;
		str.Format(_T("US: "));
	}
	if (pFrm)
	{
		if (m_sShare[0] != str)
		{
			m_sShare[0] = str;
			pFrm->DispStatusBar(str, 4);
		}
	}
}

void CManagerProcedure::ChkShareDn()
{
	CString str, str2;
	int nSerial;
	if (ChkShareDn(nSerial))
	{
		str2.Format(_T("DS: %d"), nSerial);
		pDoc->Status.PcrShare[1].bExist = TRUE;
		pDoc->Status.PcrShare[1].nSerial = nSerial;
		if (pView)
		{
			str2.Format(_T("PCR���� Received - DS: %d"), nSerial);
			pDoc->Log(str2);
			pView->MpeWrite(_T("ML45114"), (long)nSerial);	// �˻��� Panel�� AOI �� Serial
			pView->MpeWrite(_T("MB44012C"), 1); // AOI �� : PCR���� Received
		}
	}
	else
	{
		pDoc->Status.PcrShare[1].bExist = FALSE;
		pDoc->Status.PcrShare[1].nSerial = -1;
		str.Format(_T("DS: "));
	}
	if (pFrm)
	{
		if (m_sShare[1] != str)
		{
			m_sShare[1] = str;
			pFrm->DispStatusBar(str, 2);
		}
	}
}

BOOL CManagerProcedure::ChkBufIdx(int* pSerial, int nTot)
{
	if (nTot < 2)
		return TRUE;

	for (int i = 0; i < (nTot - 1); i++)
	{
		if (pSerial[i + 1] != pSerial[i] + 1)
			return FALSE;
	}
	return TRUE;
}

void CManagerProcedure::SwapUp(__int64 *num1, __int64 *num2) 	// ��ġ �ٲٴ� �Լ�
{
	__int64 temp;

	temp = *num2;
	*num2 = *num1;
	*num1 = temp;
}

BOOL CManagerProcedure::SortingInUp(CString sPath, int nIndex)
{
	struct _stat buf;
	struct tm *t;

	CString sMsg, sFileName, sSerial;
	int nPos, nSerial;
	char filename[MAX_PATH];
	StringToChar(sPath, filename);

	if (_stat(filename, &buf) != 0)
	{
		sMsg.Format(_T("�Ͻ����� - Failed getting information."));
		//MsgBox(sMsg);
		pView->ClrDispMsg();
		AfxMessageBox(sMsg);
		return FALSE;
	}
	else
	{
		sFileName = sPath;
		nPos = sFileName.ReverseFind('.');
		if (nPos > 0)
		{
			sSerial = sFileName.Left(nPos);
			sSerial = sSerial.Right(4);
		}

		nSerial = _tstoi(sSerial);

		t = localtime(&buf.st_mtime);
		//sFileName.Format(_T("%04d%02d%02d%02d%02d%02d%04d"), t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, 
		//														t->tm_hour, t->tm_min, t->tm_sec, nSerial);
		//m_nBufSerialSorting[0][nIndex] = _tstoi(sFileName);

		CString sYear, sMonth, sDay, sHour, sMin, sSec;
		sYear.Format(_T("%04d"), t->tm_year + 1900);
		sMonth.Format(_T("%02d"), t->tm_mon + 1);
		sDay.Format(_T("%02d"), t->tm_mday);
		sHour.Format(_T("%02d"), t->tm_hour);
		sMin.Format(_T("%02d"), t->tm_min);
		sSec.Format(_T("%02d"), t->tm_sec);

		__int64 nYear = _tstoi(sYear);
		__int64 nMonth = _tstoi(sMonth);
		__int64 nDay = _tstoi(sDay);
		__int64 nHour = _tstoi(sHour);
		__int64 nMin = _tstoi(sMin);
		__int64 nSec = _tstoi(sSec);

		m_nBufSerialSorting[0][nIndex] = nYear * 100000000000000 + nMonth * 1000000000000 + nDay * 10000000000 +
			nHour * 100000000 + nMin * 1000000 + nSec * 10000 + nSerial;
	}

	return TRUE;
}

BOOL CManagerProcedure::SortingOutUp(int* pSerial, int nTot)
{
	int i, k;

	for (k = 0; k < nTot; k++) 			// ���� ���� �ҽ� ����
	{
		for (i = 0; i < (nTot - 1) - k; i++)
		{

			if (m_nBufSerialSorting[0][i] > m_nBufSerialSorting[0][i + 1])
			{
				SwapUp(&m_nBufSerialSorting[0][i + 1], &m_nBufSerialSorting[0][i]);
			}
		}
	}									// ���� ���� �ҽ� ��

	for (i = 0; i < nTot; i++)
	{
		pSerial[i] = (int)(m_nBufSerialSorting[0][i] % 10000);
	}
	return TRUE;
}

BOOL CManagerProcedure::ChkBufUp(int* pSerial, int &nTot)
{
	CFileFind cFile;
	BOOL bExist = cFile.FindFile(pDoc->WorkingInfo.System.sPathVrsBufUp + _T("*.pcr"));
	if (!bExist)
	{
		m_bBufEmpty[0] = TRUE;
		if (!m_bBufEmptyF[0])
			m_bBufEmptyF[0] = TRUE;		// ���� �ѹ� ���۰� ���������(�ʱ�ȭ�� �ϰ� �� ����) TRUE.

		return FALSE; // pcr������ �������� ����.
	}

	int nPos, nSerial;

	CString sFileName, sSerial;
	CString sNewName;

	nTot = 0;
	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (!cFile.IsDirectory())
		{
			sFileName = cFile.GetFileName();

			if (!SortingInUp(pDoc->WorkingInfo.System.sPathVrsBufUp + sFileName, nTot))
				return FALSE;

			nTot++;
		}
	}

	BOOL bRtn = SortingOutUp(pSerial, nTot);

	if (nTot == 0)
		m_bBufEmpty[0] = TRUE;
	else
	{
		m_bBufEmpty[0] = FALSE;
		m_bIsBuf[0] = TRUE;
	}

	return (bRtn);
}

BOOL CManagerProcedure::ChkBufDn(int* pSerial, int &nTot)
{
	CFileFind cFile;
	BOOL bExist = cFile.FindFile(pDoc->WorkingInfo.System.sPathVrsBufDn + _T("*.pcr"));
	if (!bExist)
	{
		m_bBufEmpty[1] = TRUE;
		if (!m_bBufEmptyF[1])
			m_bBufEmptyF[1] = TRUE;
		return FALSE; // pcr������ �������� ����.
	}

	int nPos, nSerial;

	CString sFileName, sSerial;
	nTot = 0;
	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (!cFile.IsDirectory())
		{
			sFileName = cFile.GetFileName();

			if (!SortingInDn(pDoc->WorkingInfo.System.sPathVrsBufDn + sFileName, nTot))
				return FALSE;

			nTot++;
		}
	}


	BOOL bRtn = SortingOutDn(pSerial, nTot);

	if (nTot == 0)
		m_bBufEmpty[1] = TRUE;
	else
	{
		m_bBufEmpty[1] = FALSE;
		m_bIsBuf[1] = TRUE;
	}

	return (bRtn);
	//return TRUE;
}

void CManagerProcedure::SwapDn(__int64 *num1, __int64 *num2) 	// ��ġ �ٲٴ� �Լ�
{
	__int64 temp;

	temp = *num2;
	*num2 = *num1;
	*num1 = temp;
}

BOOL CManagerProcedure::SortingInDn(CString sPath, int nIndex)
{
	struct _stat buf;
	struct tm *t;

	CString sMsg, sFileName, sSerial;
	int nPos, nSerial;
	char filename[MAX_PATH];
	StringToChar(sPath, filename);

	if (_stat(filename, &buf) != 0)
	{
		sMsg.Format(_T("�Ͻ����� - Failed getting information."));
		pView->ClrDispMsg();
		AfxMessageBox(sMsg);
		return FALSE;
	}
	else
	{
		sFileName = sPath;
		nPos = sFileName.ReverseFind('.');
		if (nPos > 0)
		{
			sSerial = sFileName.Left(nPos);
			sSerial = sSerial.Right(4);
		}

		nSerial = _tstoi(sSerial);

		t = localtime(&buf.st_mtime);
		//sFileName.Format(_T("%04d%02d%02d%02d%02d%02d%04d"), t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, 
		//														t->tm_hour, t->tm_min, t->tm_sec, nSerial);
		//m_nBufSerialSorting[1][nIndex] = _tstoi(sFileName);


		CString sYear, sMonth, sDay, sHour, sMin, sSec;
		sYear.Format(_T("%04d"), t->tm_year + 1900);
		sMonth.Format(_T("%02d"), t->tm_mon + 1);
		sDay.Format(_T("%02d"), t->tm_mday);
		sHour.Format(_T("%02d"), t->tm_hour);
		sMin.Format(_T("%02d"), t->tm_min);
		sSec.Format(_T("%02d"), t->tm_sec);

		__int64 nYear = _tstoi(sYear);
		__int64 nMonth = _tstoi(sMonth);
		__int64 nDay = _tstoi(sDay);
		__int64 nHour = _tstoi(sHour);
		__int64 nMin = _tstoi(sMin);
		__int64 nSec = _tstoi(sSec);

		m_nBufSerialSorting[1][nIndex] = nYear * 100000000000000 + nMonth * 1000000000000 + nDay * 10000000000 +
			nHour * 100000000 + nMin * 1000000 + nSec * 10000 + nSerial;
	}

	return TRUE;
}

BOOL CManagerProcedure::SortingOutDn(int* pSerial, int nTot)
{
	int i, k;

	for (k = 0; k < nTot; k++) 			// ���� ���� �ҽ� ����
	{
		for (i = 0; i < (nTot - 1) - k; i++)
		{

			if (m_nBufSerialSorting[1][i] > m_nBufSerialSorting[1][i + 1])
			{
				SwapUp(&m_nBufSerialSorting[1][i + 1], &m_nBufSerialSorting[1][i]);
			}
		}
	}									// ���� ���� �ҽ� ��

	for (i = 0; i < nTot; i++)
	{
		pSerial[i] = (int)(m_nBufSerialSorting[1][i] % 10000);
	}
	return TRUE;
}

BOOL CManagerProcedure::ChkShareIdx(int *pBufSerial, int nBufTot, int nShareSerial)
{
	if (nBufTot < 1)
		return TRUE;
	for (int i = 0; i < nBufTot; i++)
	{
		if (pBufSerial[i] == nShareSerial)
			return FALSE;
	}
	return TRUE;
}

BOOL CManagerProcedure::ChkVsShare(int &nSerial)
{
	int nS0, nS1;
	BOOL b0 = ChkVsShareUp(nS0);
	BOOL b1 = ChkVsShareDn(nS1);

	if (!b0 || !b1)
	{
		nSerial = -1;
		return FALSE;
	}
	else if (nS0 != nS1)
	{
		nSerial = -1;
		return FALSE;
	}

	nSerial = nS0;
	return TRUE;
}

BOOL CManagerProcedure::ChkVsShareUp(int &nSerial)
{
	CFileFind cFile;
	BOOL bExist = cFile.FindFile(pDoc->WorkingInfo.System.sPathVsShareUp + _T("*.pcr"));
	if (!bExist)
		return FALSE; // pcr������ �������� ����.

	int nPos;
	CString sFileName, sSerial;
	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (!cFile.IsDirectory())
		{
			// ���ϸ��� ����.
			sFileName = cFile.GetFileName();
			nPos = sFileName.ReverseFind('.');
			if (nPos > 0)
				sSerial = sFileName.Left(nPos);

			nSerial = _tstoi(sSerial);
			if (nSerial > 0)
				return TRUE;
			else
			{
				nSerial = 0;
				return FALSE;
			}
		}
	}

	return FALSE;
}

BOOL CManagerProcedure::ChkVsShareDn(int &nSerial)
{
	CFileFind cFile;
	BOOL bExist = cFile.FindFile(pDoc->WorkingInfo.System.sPathVsShareDn + _T("*.pcr"));
	if (!bExist)
		return FALSE; // pcr������ �������� ����.

	int nPos;
	CString sFileName, sSerial;
	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (!cFile.IsDirectory())
		{
			// ���ϸ��� ����.
			sFileName = cFile.GetFileName();
			nPos = sFileName.ReverseFind('.');
			if (nPos > 0)
				sSerial = sFileName.Left(nPos);

			nSerial = _tstoi(sSerial);
			if (nSerial > 0)
				return TRUE;
			else
			{
				nSerial = 0;
				return FALSE;
			}
		}
	}

	return FALSE;
}

BOOL CManagerProcedure::ChkShare(int &nSerial)
{
	int nS0, nS1;
	BOOL b0 = ChkShareUp(nS0);
	BOOL b1 = ChkShareDn(nS1);

	if (!b0 || !b1)
	{
		nSerial = -1;
		return FALSE;
	}
	else if (nS0 != nS1)
	{
		nSerial = -1;
		return FALSE;
	}

	nSerial = nS0;
	return TRUE;
}

BOOL CManagerProcedure::ChkShareUp(int &nSerial)
{
	CFileFind cFile;
	BOOL bExist = cFile.FindFile(pDoc->WorkingInfo.System.sPathVrsShareUp + _T("*.pcr"));
	if (!bExist)
		return FALSE; // pcr������ �������� ����.

	int nPos;
	CString sFileName, sSerial;
	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (!cFile.IsDirectory())
		{
			// ���ϸ��� ����.
			sFileName = cFile.GetFileName();
			nPos = sFileName.ReverseFind('.');
			if (nPos > 0)
				sSerial = sFileName.Left(nPos);

			nSerial = _tstoi(sSerial);
			if (nSerial > 0)
				return TRUE;
			else
			{
				nSerial = 0;
				return FALSE;
			}
		}
	}

	return FALSE;
}

BOOL CManagerProcedure::ChkShareDn(int &nSerial)
{
	CFileFind cFile;
	BOOL bExist = cFile.FindFile(pDoc->WorkingInfo.System.sPathVrsShareDn + _T("*.pcr"));
	if (!bExist)
		return FALSE; // pcr������ �������� ����.

	int nPos;
	CString sFileName, sSerial;
	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (!cFile.IsDirectory())
		{
			// ���ϸ��� ����.
			sFileName = cFile.GetFileName();
			nPos = sFileName.ReverseFind('.');
			if (nPos > 0)
				sSerial = sFileName.Left(nPos);

			nSerial = _tstoi(sSerial);
			if (nSerial > 0)
				return TRUE;
			else
			{
				nSerial = 0;
				return FALSE;
			}
		}
	}

	return FALSE;
}

void CManagerProcedure::DelOverLotEndSerialUp(int nSerial)
{
	CString sSrc;

	if (nSerial > 0)
	{
		sSrc.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufUp, nSerial);

		if (m_bSerialDecrese)
		{
			if (m_nLotEndSerial > 0 && nSerial < m_nLotEndSerial)
			{
				// Delete PCR File
				pDoc->m_pFile->DeleteFolerOrFile(sSrc);
			}
		}
		else
		{
			if (m_nLotEndSerial > 0 && nSerial > m_nLotEndSerial)
			{
				// Delete PCR File
				pDoc->m_pFile->DeleteFolerOrFile(sSrc);
			}
		}
	}

}

void CManagerProcedure::DelOverLotEndSerialDn(int nSerial)
{
	CString sSrc;

	if (nSerial > 0)
	{
		sSrc.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufDn, nSerial);

		if (m_bSerialDecrese)
		{
			if (m_nLotEndSerial > 0 && nSerial < m_nLotEndSerial)
			{
				// Delete PCR File
				pDoc->m_pFile->DeleteFolerOrFile(sSrc);
			}
		}
		else
		{
			if (m_nLotEndSerial > 0 && nSerial > m_nLotEndSerial)
			{
				// Delete PCR File
				pDoc->m_pFile->DeleteFolerOrFile(sSrc);
			}
		}
	}

}

void CManagerProcedure::InitThread()
{
	// DispDefImg
	if (!m_bThread[0])
		m_Thread[0].Start(GetSafeHwnd(), this, ThreadProc0);

	// Engrave Auto Sequence - Response Check
	if (!m_bThread[1])
		m_Thread[1].Start(GetSafeHwnd(), this, ThreadProc1);

	// DispDefImgInner
	if (!m_bThread[2])
		m_Thread[2].Start(GetSafeHwnd(), this, ThreadProc2);

	// DoShift2Mk
	if (!m_bThread[3])
		m_Thread[3].Start(GetSafeHwnd(), this, ThreadProc3);
}

void CManagerProcedure::KillThread()
{
	if (m_bThread[0])	// DispDefImg
	{
		m_Thread[0].Stop();
		Sleep(100);
		while (m_bThread[0])
		{
			Sleep(20);
		}
	}

	if (m_bThread[1])	// Engrave Auto Sequence - Response Check
	{
		m_Thread[1].Stop();
		Sleep(100);
		while (m_bThread[1])
		{
			Sleep(20);
		}
	}

	if (m_bThread[2])	// DispDefImgInner
	{
		m_Thread[2].Stop();
		Sleep(100);
		while (m_bThread[2])
		{
			Sleep(20);
		}
	}

	if (m_bThread[3])	// DoShift2Mk
	{
		m_Thread[3].Stop();
		Sleep(100);
		while (m_bThread[3])
		{
			Sleep(20);
		}
	}
}

UINT CManagerProcedure::ThreadProc0(LPVOID lpContext)	// DispDefImg()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerProcedure* pThread = reinterpret_cast< CManagerProcedure* >(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[0] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[0].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[0] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (!bLock)
		{
			bLock = TRUE;

			if (pThread->m_bTHREAD_DISP_DEF)
			{
				pThread->DispDefImg();
				Sleep(0);
			}
			else
				Sleep(30);

			bLock = FALSE;
		}
	}
	pThread->m_bThread[0] = FALSE;

	return 0;
}

UINT CManagerProcedure::ThreadProc1(LPVOID lpContext)	// GetCurrentInfoSignal()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerProcedure* pThread = reinterpret_cast<CManagerProcedure*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[1] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[1].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[1] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (!pView->m_bDestroyedView)
		{
			if (!bLock)
			{
				bLock = TRUE;
				pThread->GetCurrentInfoSignal();
				bLock = FALSE;
			}
		}
		Sleep(100);
	}
	pThread->m_bThread[1] = FALSE;

	return 0;
}

UINT CManagerProcedure::ThreadProc2(LPVOID lpContext) // DispDefImgInner()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerProcedure* pThread = reinterpret_cast<CManagerProcedure*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[2] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[2].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[2] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (!bLock)
		{
			bLock = TRUE;

			if (pThread->m_bTHREAD_DISP_DEF_INNER)
			{
				pThread->DispDefImgInner();
				Sleep(0);
			}
			else
				Sleep(30);

			bLock = FALSE;
		}
	}
	pThread->m_bThread[2] = FALSE;

	return 0;
}

UINT CManagerProcedure::ThreadProc3(LPVOID lpContext)	// RunShift2Mk()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerProcedure* pThread = reinterpret_cast<CManagerProcedure*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[3] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[3].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[3] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_SHIFT2MK)
		{
			if (!pThread->m_bTHREAD_UPDATAE_YIELD[0] && !pThread->m_bTHREAD_UPDATAE_YIELD[1])
			{
				pThread->RunShift2Mk();
				pThread->m_bTHREAD_SHIFT2MK = FALSE;
				Sleep(0);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[3] = FALSE;

	return 0;
}

void CManagerProcedure::DispDefImg()
{
	if (!pView->m_mgrReelmap)
		return;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	int nSerialL = m_nBufUpSerial[0]; // ���� Camera
	int nSerialR = m_nBufUpSerial[1]; // ���� Camera
	CString sNewLot = m_sNewLotUp;;
	CString str;
	BOOL bNewModel = FALSE;
	int nNewLot = 0;
	int nBreak = 0;
	int nLastShot = 0;

	switch (m_nStepTHREAD_DISP_DEF)
	{
		// CopyDefImg Start ============================================
	case 0:
		m_nStepTHREAD_DISP_DEF++;

		if (nSerialL == m_nLotEndSerial)
			nBreak = 1;

		if (nSerialL > 0)
		{
			if (!CopyDefImg(nSerialL, sNewLot)) // ���� Camera
			{
				sNewLot.Empty();
				pView->m_bDispMsgDoAuto[7] = TRUE;
				pView->m_nStepDispMsg[7] = FROM_DISPDEFIMG + 7;
				break;
			}

			if (m_bSerialDecrese)
			{
				if (m_bLastProc && nSerialR < m_nLotEndSerial)
				{
					nLastShot = nSerialL;
				}
				else
				{
					if (nSerialR > 0)
					{
						if (!CopyDefImg(nSerialR, sNewLot)) // ���� Camera
						{
							sNewLot.Empty();
							pView->m_bDispMsgDoAuto[6] = TRUE;
							pView->m_nStepDispMsg[6] = FROM_DISPDEFIMG + 6;
							break;
						}
					}
				}
			}
			else
			{
				if (m_bLastProc && nSerialR > m_nLotEndSerial)
				{
					nLastShot = nSerialL;
				}
				else
				{
					if (nSerialR > 0)
					{
						if (!CopyDefImg(nSerialR, sNewLot)) // ���� Camera
						{
							sNewLot.Empty();
							pView->m_bDispMsgDoAuto[6] = TRUE;
							pView->m_nStepDispMsg[6] = FROM_DISPDEFIMG + 6;
							break;
						}
					}
				}
			}

		}
		else
		{
			if (!m_bLastProc)
			{
				pView->m_bDispMsgDoAuto[5] = TRUE;
				pView->m_nStepDispMsg[5] = FROM_DISPDEFIMG + 5;
			}
		}
		sNewLot.Empty();
		break;
	case 1:
		//Sleep(300);
		m_nStepTHREAD_DISP_DEF++;
		if (pView->m_mgrReelmap->m_pReelMap)
		{
			str = pView->m_mgrReelmap->m_pReelMap->GetIpPath();
			pDoc->SetMkMenu01(_T("DispDefImg"), _T("ReelmapPath"), str);
		}
		str.Format(_T("%d"), nSerialL);
		pDoc->SetMkMenu01(_T("DispDefImg"), _T("SerialL"), str);
		str.Format(_T("%d"), nSerialR);
		pDoc->SetMkMenu01(_T("DispDefImg"), _T("SerialR"), str);
		break;
	case 2:
		if (IsDoneDispMkInfo())	 // Check �ҷ��̹��� Display End
		{
			if (m_bSerialDecrese)
			{
				if (ChkLastProc() && (nSerialL < m_nLotEndSerial))
				{
					nLastShot = nSerialL;
					SetSerialReelmap(nSerialL, TRUE);	// Reelmap(��) Display Start
					SetSerialMkInfo(nSerialL, TRUE);		// �ҷ��̹���(��) Display Start
				}
				else if (ChkLastProc() && nSerialL < 1)
				{
					SetSerialReelmap(m_nLotEndSerial + 1, TRUE);		// Reelmap(��) Display Start
					SetSerialMkInfo(m_nLotEndSerial + 1, TRUE);		// �ҷ��̹���(��) Display Start
				}
				else
				{
					SetFixPcs(nSerialL);
					SetSerialReelmap(nSerialL);	// Reelmap(��) Display Start
					SetSerialMkInfo(nSerialL);	// �ҷ��̹���(��) Display Start
				}
			}
			else
			{
				if (ChkLastProc() && (nSerialL > m_nLotEndSerial))
				{
					nLastShot = nSerialL;
					SetSerialReelmap(nSerialL, TRUE);	// Reelmap(��) Display Start
					SetSerialMkInfo(nSerialL, TRUE);		// �ҷ��̹���(��) Display Start
				}
				else if (ChkLastProc() && nSerialL < 1)
				{
					SetSerialReelmap(m_nLotEndSerial + 1, TRUE);		// Reelmap(��) Display Start
					SetSerialMkInfo(m_nLotEndSerial + 1, TRUE);		// �ҷ��̹���(��) Display Start
				}
				else
				{
					SetFixPcs(nSerialL);
					SetSerialReelmap(nSerialL);	// Reelmap(��) Display Start
					SetSerialMkInfo(nSerialL);	// �ҷ��̹���(��) Display Start
				}
			}


			if (pView->m_mgrReelmap->IsFixPcsUp(nSerialL))
			{
				pView->m_bDispMsgDoAuto[2] = TRUE;
				pView->m_nStepDispMsg[2] = FROM_DISPDEFIMG + 2;
			}
			if (pView->m_mgrReelmap->IsFixPcsDn(nSerialL))
			{
				pView->m_bDispMsgDoAuto[3] = TRUE;
				pView->m_nStepDispMsg[3] = FROM_DISPDEFIMG + 3;
			}

			m_nStepTHREAD_DISP_DEF++;
		}

		break;

	case 3:
		if (IsDoneDispMkInfo())	 // Check �ҷ��̹���(��) Display End
		{
			if (nSerialR > 0) // ���� Camera
			{
				if (m_bSerialDecrese)
				{
					if (ChkLastProc() && (nSerialR < m_nLotEndSerial))
						SetSerialMkInfo(nSerialR, TRUE);	// �ҷ��̹���(��) Display Start
					else
						SetSerialMkInfo(nSerialR);		// �ҷ��̹���(��) Display Start
				}
				else
				{
					if (ChkLastProc() && (nSerialR > m_nLotEndSerial))
						SetSerialMkInfo(nSerialR, TRUE);	// �ҷ��̹���(��) Display Start
					else
						SetSerialMkInfo(nSerialR);		// �ҷ��̹���(��) Display Start
				}
			}
			else
			{
				if (ChkLastProc())
				{
					if (m_bSerialDecrese)
						SetSerialMkInfo(m_nLotEndSerial - 1, TRUE);	// �ҷ��̹���(��) Display Start
					else
						SetSerialMkInfo(m_nLotEndSerial + 1, TRUE);	// �ҷ��̹���(��) Display Start
				}
			}
			m_nStepTHREAD_DISP_DEF++;
		}
		break;
	case 4:
		if (nSerialR > 0) // ���� Camera
		{
			m_nStepTHREAD_DISP_DEF++;

			if (m_bSerialDecrese)
			{
				if (ChkLastProc() && (nSerialR < m_nLotEndSerial))
				{
					SetSerialReelmap(nSerialR, TRUE);	// Reelmap(��) Display Start
				}
				else
				{
					SetFixPcs(nSerialR);
					SetSerialReelmap(nSerialR);			// Reelmap(��) Display Start
				}
			}
			else
			{
				if (ChkLastProc() && (nSerialR > m_nLotEndSerial))
				{
					SetSerialReelmap(nSerialR, TRUE);	// Reelmap(��) Display Start
				}
				else
				{
					SetFixPcs(nSerialR);
					SetSerialReelmap(nSerialR);			// Reelmap(��) Display Start
				}
			}

			if (bDualTest)
			{
				if (pView->m_mgrReelmap->IsFixPcsUp(nSerialR))
				{
					pView->m_bDispMsgDoAuto[2] = TRUE;
					pView->m_nStepDispMsg[2] = FROM_DISPDEFIMG + 2;
				}
				if (pView->m_mgrReelmap->IsFixPcsDn(nSerialR))
				{
					pView->m_bDispMsgDoAuto[3] = TRUE;
					pView->m_nStepDispMsg[3] = FROM_DISPDEFIMG + 3;
				}
			}
			else
			{
				if (pView->m_mgrReelmap->IsFixPcsUp(nSerialR))
				{
					pView->m_bDispMsgDoAuto[2] = TRUE;
					pView->m_nStepDispMsg[2] = FROM_DISPDEFIMG + 2;
				}
			}
		}
		else
		{
			if (ChkLastProc())
			{
				m_nStepTHREAD_DISP_DEF++;
				if (m_bSerialDecrese)
					SetSerialReelmap(m_nLotEndSerial - 1, TRUE);	// �ҷ��̹���(��) Display Start
				else
					SetSerialReelmap(m_nLotEndSerial + 1, TRUE);	// �ҷ��̹���(��) Display Start
			}
			else
			{
				if (m_bLastProc && nSerialL == m_nLotEndSerial)
					m_nStepTHREAD_DISP_DEF++;
				else
				{
					m_nStepTHREAD_DISP_DEF++;
				}
			}
		}
		break;
	case 5:
		m_nStepTHREAD_DISP_DEF++;
		break;
	case 6:
		m_nStepTHREAD_DISP_DEF++;
		break;
	case 7:
		m_nStepTHREAD_DISP_DEF++;
		break;
	case 8:
		m_nStepTHREAD_DISP_DEF++;
		break;
	case 9:
		m_nStepTHREAD_DISP_DEF++;
		break;
	case 10:
		m_nStepTHREAD_DISP_DEF++;
		break;

	case 11:
		if (IsDoneDispMkInfo() && IsRun())	 // Check �ҷ��̹���(��) Display End
			m_nStepTHREAD_DISP_DEF++;
		break;
	case 12:
		m_bTHREAD_DISP_DEF = FALSE;
		pDoc->SetMkMenu01(_T("Signal"), _T("DispDefImg"), _T("0"));
		//if (m_pEngrave)
		//	m_pEngrave->SwMenu01DispDefImg(TRUE);
		SwMenu01DispDefImg(TRUE);

		break;
		// CopyDefImg End ============================================
	}

}

void CManagerProcedure::DispDefImgInner()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;

	CString sNewLot;
	BOOL bNewModel = FALSE;
	int nSerial = 0;
	int nNewLot = 0;
	int nBreak = 0;

	switch (m_nStepTHREAD_DISP_DEF_INNER)
	{
		// CopyDefImg Start ============================================
	case 0:
		m_nStepTHREAD_DISP_DEF_INNER++;
		break;
	case 1:
		Sleep(300);
		m_nStepTHREAD_DISP_DEF_INNER++;
		break;
	case 2:
		nSerial = m_nBufUpSerial[0]; // ���� Camera

		if (IsDoneDispMkInfoInner())	 // Check �ҷ��̹��� Display End
		{
			if (m_bSerialDecrese)
			{
				if (ChkLastProc() && (nSerial < m_nLotEndSerial))
				{
					SetSerialReelmapInner(nSerial, TRUE);	// Reelmap(��) Display Start
					SetSerialMkInfoInner(nSerial, TRUE);		// �ҷ��̹���(��) Display Start
				}
				else if (ChkLastProc() && nSerial < 1)
				{
					SetSerialReelmapInner(m_nLotEndSerial - 1, TRUE);		// Reelmap(��) Display Start
					SetSerialMkInfoInner(m_nLotEndSerial - 1, TRUE);		// �ҷ��̹���(��) Display Start
				}
				else
				{
					//SetFixPcs(nSerial);
					SetSerialReelmapInner(nSerial);	// Reelmap(��) Display Start
					SetSerialMkInfoInner(nSerial);	// �ҷ��̹���(��) Display Start
				}
			}
			else
			{
				if (ChkLastProc() && (nSerial > m_nLotEndSerial))
				{
					SetSerialReelmapInner(nSerial, TRUE);	// Reelmap(��) Display Start
					SetSerialMkInfoInner(nSerial, TRUE);		// �ҷ��̹���(��) Display Start
				}
				else if (ChkLastProc() && nSerial < 1)
				{
					SetSerialReelmapInner(m_nLotEndSerial + 1, TRUE);		// Reelmap(��) Display Start
					SetSerialMkInfoInner(m_nLotEndSerial + 1, TRUE);		// �ҷ��̹���(��) Display Start
				}
				else
				{
					//SetFixPcs(nSerial);
					SetSerialReelmapInner(nSerial);	// Reelmap(��) Display Start
					SetSerialMkInfoInner(nSerial);	// �ҷ��̹���(��) Display Start
				}
			}

			m_nStepTHREAD_DISP_DEF_INNER++;
		}

		break;

	case 3:
		if (IsDoneDispMkInfoInner())	 // Check �ҷ��̹���(��) Display End
		{
			nSerial = m_nBufUpSerial[1]; // ���� Camera

			if (nSerial > 0)
			{
				if (m_bSerialDecrese)
				{
					if (ChkLastProc() && (nSerial < m_nLotEndSerial))
						SetSerialMkInfoInner(nSerial, TRUE);	// �ҷ��̹���(��) Display Start
					else
						SetSerialMkInfoInner(nSerial);		// �ҷ��̹���(��) Display Start
				}
				else
				{
					if (ChkLastProc() && (nSerial > m_nLotEndSerial))
						SetSerialMkInfoInner(nSerial, TRUE);	// �ҷ��̹���(��) Display Start
					else
						SetSerialMkInfoInner(nSerial);		// �ҷ��̹���(��) Display Start
				}
			}
			else
			{
				if (m_bSerialDecrese)
				{
					if (ChkLastProc())
						SetSerialMkInfoInner(m_nLotEndSerial - 1, TRUE);	// �ҷ��̹���(��) Display Start
				}
				else
				{
					if (ChkLastProc())
						SetSerialMkInfoInner(m_nLotEndSerial + 1, TRUE);	// �ҷ��̹���(��) Display Start
				}
			}
			m_nStepTHREAD_DISP_DEF_INNER++;
		}
		break;
	case 4:
		nSerial = m_nBufUpSerial[1]; // ���� Camera

		if (nSerial > 0)
		{
			m_nStepTHREAD_DISP_DEF_INNER++;

			if (m_bSerialDecrese)
			{
				if (ChkLastProc() && (nSerial < m_nLotEndSerial))
				{
					SetSerialReelmapInner(nSerial, TRUE);	// Reelmap(��) Display Start
				}
				else
				{
					//SetFixPcs(nSerial);
					SetSerialReelmapInner(nSerial);			// Reelmap(��) Display Start
				}
			}
			else
			{
				if (ChkLastProc() && (nSerial > m_nLotEndSerial))
				{
					SetSerialReelmapInner(nSerial, TRUE);	// Reelmap(��) Display Start
				}
				else
				{
					//SetFixPcs(nSerial);
					SetSerialReelmapInner(nSerial);			// Reelmap(��) Display Start
				}
			}
		}
		else
		{
			if (ChkLastProc())
			{
				m_nStepTHREAD_DISP_DEF_INNER++;
				if (m_bSerialDecrese)
					SetSerialReelmapInner(m_nLotEndSerial - 1, TRUE);	// �ҷ��̹���(��) Display Start
				else
					SetSerialReelmapInner(m_nLotEndSerial + 1, TRUE);	// �ҷ��̹���(��) Display Start
			}
			else
			{
				if (bDualTest)
				{
					if (m_bLastProc && m_nBufDnSerial[0] == m_nLotEndSerial)
						m_nStepTHREAD_DISP_DEF_INNER++;
					else
					{
						m_nStepTHREAD_DISP_DEF_INNER++;
					}
				}
				else
				{
					if (m_bLastProc && m_nBufUpSerial[0] == m_nLotEndSerial)
						m_nStepTHREAD_DISP_DEF_INNER++;
					else
					{
						m_nStepTHREAD_DISP_DEF_INNER++;
					}
				}
			}
		}
		break;
	case 5:
		m_nStepTHREAD_DISP_DEF_INNER++;
		break;
	case 6:
		m_nStepTHREAD_DISP_DEF_INNER++;
		break;
	case 7:
		m_nStepTHREAD_DISP_DEF_INNER++;
		break;
	case 8:
		m_nStepTHREAD_DISP_DEF_INNER++;
		break;
	case 9:
		m_nStepTHREAD_DISP_DEF_INNER++;
		break;
	case 10:
		m_nStepTHREAD_DISP_DEF_INNER++;
		break;

	case 11:
		if (IsDoneDispMkInfoInner() && IsRun())	 // Check �ҷ��̹���(��) Display End
			m_nStepTHREAD_DISP_DEF_INNER++;
		break;
	case 12:
		m_bTHREAD_DISP_DEF_INNER = FALSE;
		break;
		// Disp DefImg End ============================================
	}

}

BOOL CManagerProcedure::IsRun()
{
	//return TRUE; // AlignTest
	if (pView->m_sDispMain == _T("������") || pView->m_sDispMain == _T("�ʱ����")
		|| pView->m_sDispMain == _T("�ܸ����") || pView->m_sDispMain == _T("�ܸ�˻�")
		|| pView->m_sDispMain == _T("���˻�") || pView->m_sDispMain == _T("������"))
		return TRUE;
	return FALSE;
}

BOOL CManagerProcedure::IsDoneDispMkInfoInner()
{
	return pView->IsDoneDispMkInfoInner();
}

BOOL CManagerProcedure::SetSerialReelmapInner(int nSerial, BOOL bDumy)
{
	return pView->SetSerialReelmapInner(nSerial, bDumy);
}

BOOL CManagerProcedure::ChkLastProc()
{
	return pView->ChkLastProc();
}

BOOL CManagerProcedure::SetSerialMkInfoInner(int nSerial, BOOL bDumy)
{
	return pView->SetSerialMkInfoInner(nSerial, bDumy);
}

void CManagerProcedure::RunShift2Mk()
{
	pView->RunShift2Mk();
}

BOOL CManagerProcedure::GetCurrentInfoSignal()
{
	return pView->GetCurrentInfoSignal();
}

BOOL CManagerProcedure::CopyDefImg(int nSerial) 
{
	return pView->CopyDefImg(nSerial);
}

BOOL CManagerProcedure::IsDoneDispMkInfo()
{
	return pView->IsDoneDispMkInfo();
}

BOOL CManagerProcedure::SetSerialReelmap(int nSerial, BOOL bDumy)
{
	return pView->SetSerialReelmap(nSerial, bDumy);
}

BOOL CManagerProcedure::SetSerialMkInfo(int nSerial, BOOL bDumy)
{
	return pView->SetSerialMkInfo(nSerial, bDumy);
}

void CManagerProcedure::SetFixPcs(int nSerial)
{
	pView->SetSerialMkInfo(nSerial);
}

void CManagerProcedure::SwMenu01DispDefImg(BOOL bOn)
{
	pView->SwMenu01DispDefImg(bOn);
}

BOOL CManagerProcedure::CopyDefImg(int nSerial, CString sNewLot)
{
	return pView->CopyDefImg(nSerial, sNewLot);
}

void CManagerProcedure::DoMark0Its()
{
	if (!pView->m_mgrPunch)
		return;
	pView->m_mgrPunch->DoMark0Its();
}

void CManagerProcedure::DoMark1Its()
{
	if (!pView->m_mgrPunch)
		return;
	pView->m_mgrPunch->DoMark1Its();
}

void CManagerProcedure::DoMark0()
{
	if (!pView->m_mgrPunch)
		return;
	pView->m_mgrPunch->DoMark0();
}

void CManagerProcedure::DoMark1()
{
	if (!pView->m_mgrPunch)
		return;
	pView->m_mgrPunch->DoMark1();
}

void CManagerProcedure::DoMark0All()
{
	if (!pView->m_mgrPunch)
		return;
	pView->m_mgrPunch->DoMark0All();
}

void CManagerProcedure::DoMark1All()
{
	if (!pView->m_mgrPunch)
		return;
	pView->m_mgrPunch->DoMark1All();
}

void CManagerProcedure::DoReject0()
{
	pView->DoReject0();
}

void CManagerProcedure::DoReject1()
{
	pView->DoReject1();
}

void CManagerProcedure::GetEnc()
{
	pView->GetEnc();
}

BOOL CManagerProcedure::IsRunAxisX()
{
	if (!pView->m_mgrPunch)
		return FALSE;
	return pView->m_mgrPunch->IsRunAxisX();
}

void CManagerProcedure::EStop()
{
	if (!pView->m_mgrPunch)
		return;
	pView->m_mgrPunch->EStop();
}

int CManagerProcedure::GetLastShotMk()	// m_pDlgFrameHigh���� ��ų� ������, sPathOldFile������ ReelMapDataDn.txt���� _T("Info"), _T("Marked Shot") ã��.
{
	return pDoc->GetLastShotMk();
}

int CManagerProcedure::GetLastShotUp()
{
	return pView->GetLastShotUp();
}

int CManagerProcedure::GetLastShotDn()
{
	return pView->GetLastShotDn();
}

void CManagerProcedure::InitAuto(BOOL bInit)
{
	int nCam, nPoint, kk;// , a, b;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	m_sFixMsg[0] = _T("");
	m_sFixMsg[1] = _T("");

	m_bReadyDone = FALSE;
	m_bChkLastProcVs = FALSE;
	m_nDummy[0] = 0;
	m_nDummy[1] = 0;
	m_nAoiLastSerial[0] = 0;
	m_nAoiLastSerial[1] = 0;
	m_nStepAuto = 0;
	m_nPrevStepAuto = 0;
	m_nPrevMkStAuto = 0;
	m_bAoiLdRun = TRUE;
	m_bAoiLdRunF = FALSE;
	m_bNewModel = FALSE;
	m_nLotEndSerial = 0;

	m_bChkBufIdx[0] = TRUE;
	m_bChkBufIdx[1] = TRUE;

	m_nErrCnt = 0;
	m_bMkTmpStop = FALSE;
	m_bWaitPcr[0] = FALSE;
	m_bWaitPcr[1] = FALSE;

	m_nShareUpS = 0;
	m_nShareUpSerial[0] = 0;
	m_nShareUpSerial[1] = 0;
	m_nShareUpCnt = 0;

	m_nShareDnS = 0;
	m_nShareDnSerial[0] = 0;
	m_nShareDnSerial[1] = 0;
	m_nShareDnCnt = 0;

	m_nBufUpSerial[0] = 0;
	m_nBufUpSerial[1] = 0;
	m_nBufUpCnt = 0;

	m_nBufDnSerial[0] = 0;
	m_nBufDnSerial[1] = 0;
	m_nBufDnCnt = 0;

	m_bReAlign[0][0] = FALSE;	// [nCam][nPos]
	m_bReAlign[0][1] = FALSE;	// [nCam][nPos]
	m_bReAlign[0][2] = FALSE;	// [nCam][nPos]
	m_bReAlign[0][3] = FALSE;	// [nCam][nPos]
	m_bReAlign[1][0] = FALSE;	// [nCam][nPos]
	m_bReAlign[1][1] = FALSE;	// [nCam][nPos]
	m_bReAlign[1][2] = FALSE;	// [nCam][nPos]
	m_bReAlign[1][3] = FALSE;	// [nCam][nPos]

	m_bSkipAlign[0][0] = FALSE;	// [nCam][nPos]
	m_bSkipAlign[0][1] = FALSE;	// [nCam][nPos]
	m_bSkipAlign[0][2] = FALSE;	// [nCam][nPos]
	m_bSkipAlign[0][3] = FALSE;	// [nCam][nPos]
	m_bSkipAlign[1][0] = FALSE;	// [nCam][nPos]
	m_bSkipAlign[1][1] = FALSE;	// [nCam][nPos]
	m_bSkipAlign[1][2] = FALSE;	// [nCam][nPos]
	m_bSkipAlign[1][3] = FALSE;	// [nCam][nPos]

	m_bFailAlign[0][0] = FALSE;	// [nCam][nPos]
	m_bFailAlign[0][1] = FALSE;	// [nCam][nPos]
	m_bFailAlign[0][2] = FALSE;	// [nCam][nPos]
	m_bFailAlign[0][3] = FALSE;	// [nCam][nPos]
	m_bFailAlign[1][0] = FALSE;	// [nCam][nPos]
	m_bFailAlign[1][1] = FALSE;	// [nCam][nPos]
	m_bFailAlign[1][2] = FALSE;	// [nCam][nPos]
	m_bFailAlign[1][3] = FALSE;	// [nCam][nPos]

	m_bMkSt = FALSE;
	::WritePrivateProfileString(_T("Last Job"), _T("MkSt"), _T("0"), PATH_WORKING_INFO);
	m_bMkStSw = FALSE;
	m_nMkStAuto = 0;

	m_bEngSt = FALSE;
	m_bEngStSw = FALSE;
	m_nEngStAuto = 0;

	m_bEng2dSt = FALSE;
	m_bEng2dStSw = FALSE;
	m_nEng2dStAuto = 0;

	m_bLotEnd = FALSE;
	m_nLotEndAuto = 0;

	m_bLastProc = FALSE;
	if (MODE_INNER != pDoc->GetTestMode())
		m_bLastProcFromUp = TRUE;
	else
		m_bLastProcFromEng = TRUE;

	m_nLastProcAuto = 0;

	m_dwCycSt = 0;
	m_sNewLotUp = _T("");
	m_sNewLotDn = _T("");

	m_nStop = 0;

	m_nStepTHREAD_DISP_DEF = 0;
	m_bTHREAD_DISP_DEF = FALSE;				// CopyDefImg Stop
	m_nStepTHREAD_DISP_DEF_INNER = 0;
	m_bTHREAD_DISP_DEF_INNER = FALSE;		// DispDefImg Stop

	InitVal();

	m_bSerialDecrese = FALSE;
	m_bStopF_Verify = TRUE;
	m_bInitAuto = TRUE;
	m_bInitAutoLoadMstInfo = TRUE;

	if (bInit) // �̾�Ⱑ �ƴѰ��.
	{
		m_nRstNum = 0;
		m_bCont = FALSE;
		m_dTotVel = 0.0;
		m_dPartVel = 0.0;
		m_dwCycSt = 0;
		m_dwCycTim = 0;

		pDoc->m_nPrevSerial = 0;
		pDoc->m_bNewLotShare[0] = FALSE;
		pDoc->m_bNewLotShare[1] = FALSE;
		pDoc->m_bNewLotBuf[0] = FALSE;
		pDoc->m_bNewLotBuf[1] = FALSE;
		pDoc->m_bDoneChgLot = FALSE;
	}

}

void CManagerProcedure::Stop()
{
	pView->Stop();
}

void CManagerProcedure::Buzzer(BOOL bOn, int nCh)
{
	pView->Buzzer(bOn, nCh);
}

void CManagerProcedure::DispMain(CString sMsg, COLORREF rgb)
{
	pView->DispMain(sMsg, rgb);
}

int CManagerProcedure::MsgBox(CString sMsg, int nThreadIdx, int nType, int nTimOut, BOOL bEngave)
{
	return pView->MsgBox(sMsg, nThreadIdx, nType, nTimOut, bEngave);
}

void CManagerProcedure::DispInitRun()
{
	if (pDoc->WorkingInfo.LastJob.bSampleTest)
	{
		if (pDoc->WorkingInfo.LastJob.bDualTest)
		{
			if (pView->m_sDispMain != _T("������"))
				DispMain(_T("������"), RGB_GREEN);
		}
		else
		{
			if (pView->m_sDispMain != _T("�ܸ����"))
				DispMain(_T("�ܸ����"), RGB_GREEN);
		}
	}
	else if (pDoc->WorkingInfo.LastJob.bDualTest)
	{
		if (pView->m_sDispMain != _T("���˻�"))
			DispMain(_T("���˻�"), RGB_GREEN);
	}
	else
	{
		if (pView->m_sDispMain != _T("�ܸ�˻�"))
			DispMain(_T("�ܸ�˻�"), RGB_GREEN);
		//if(pView->m_sDispMain != _T("�ʱ����")
		//	DispMain(_T("�ʱ����", RGB_GREEN);
	}
}

BOOL CManagerProcedure::IsReady()
{
	return pView->IsReady();
}

void CManagerProcedure::ClrDispMsg()
{
	pView->ClrDispMsg();
}

BOOL CManagerProcedure::IsAoiLdRun()
{
	return pView->IsAoiLdRun();
}

void CManagerProcedure::ResetWinker() // 0:Ready, 1:Reset, 2:Run, 3:Stop
{
	pView->ResetWinker();
}

double CManagerProcedure::GetMkFdLen()
{
	return pView->GetMkFdLen();
}

BOOL CManagerProcedure::IsVerify()
{
	BOOL bVerify = FALSE;
	int nSerial0 = m_nBufUpSerial[0]; // Cam0
	int nSerial1 = m_nBufUpSerial[1]; // Cam1
	int nPeriod = pDoc->WorkingInfo.LastJob.nVerifyPeriod;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;


	if (pDoc->WorkingInfo.LastJob.bVerify)
	{
		double dFdLen = GetMkFdLen();
		double dVerifyLen = _tstof(pDoc->WorkingInfo.LastJob.sVerifyLen)*1000.0;

		if (dFdLen < dVerifyLen)
		{
			if (nSerial0 == 1 || nPeriod == 0 || nPeriod == 1 || nPeriod == 2 || m_bStopF_Verify)
			{
				m_bStopF_Verify = FALSE;
				bVerify = TRUE;
			}
			else
			{
				if (!(nSerial0 % nPeriod) || !(nSerial1 % nPeriod))
					bVerify = TRUE;
			}
		}
		else
		{
			pDoc->WorkingInfo.LastJob.bVerify = FALSE;
			pView->UpdateData(DlgMenu01);
		}
	}

	return bVerify;
}

int CManagerProcedure::GetVsBufLastSerial()
{
	int nLastShot = pDoc->GetLastShotMk();
	if (nLastShot > 0 && m_bCont)
		return (nLastShot + 4);

	return 4;
}

int CManagerProcedure::GetVsUpBufLastSerial()
{
	int nLastShot = GetLastShotUp();
	if (nLastShot > 0 && m_bCont)
		return (nLastShot + 4);

	return 4;
}

int CManagerProcedure::GetVsDnBufLastSerial()
{
	int nLastShot = GetLastShotDn();
	if (nLastShot > 0 && m_bCont)
		return (nLastShot + 4);

	return 4;
}

void CManagerProcedure::SetDummyUp()
{
	m_nDummy[0]++;
	if (m_nDummy[0] == 3)
	{
		if (!MakeDummyUp(-2))
		{
			pView->ClrDispMsg();
			AfxMessageBox(_T("Error - MakeDummyUp(-2)"));
		}
	}
	else
	{
		if (!MakeDummyUp(-1))
		{
			pView->ClrDispMsg();
			AfxMessageBox(_T("Error - MakeDummyUp(-1)"));
		}
	}
}

void CManagerProcedure::SetDummyDn()
{
	m_nDummy[1]++;
	if (m_nDummy[1] == 3)
	{
		if (!MakeDummyDn(-2))
		{
			pView->ClrDispMsg();
			AfxMessageBox(_T("Error - MakeDummyDn(-2)"));
		}
	}
	else
	{
		if (!MakeDummyDn(-1))
		{
			pView->ClrDispMsg();
			AfxMessageBox(_T("Error - MakeDummyDn(-1)"));
		}
	}
}

BOOL CManagerProcedure::MakeDummyUp(int nErr) // AOI ��� ����.
{
	int nSerial = m_nAoiLastSerial[0] + m_nDummy[0];
	FILE *fpPCR = NULL;

	CString sMsg, strRstPath, strRstPath2, sDummyRst;
	strRstPath.Format(_T("%s%s\\%s\\%s\\%04d.RST"), pDoc->WorkingInfo.System.sPathAoiUpVrsData,
		pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLayerUp,
		pDoc->WorkingInfo.LastJob.sLotUp,
		m_nAoiLastSerial[0]);

	CDataFile *pDataFile = new CDataFile;

	if (!pDataFile->Open(strRstPath))
	{
		sMsg.Format(_T("%s File not found."), strRstPath);
		pView->MsgBox(sMsg);
		delete pDataFile;
		return FALSE;
	}
	CString sLine, sRemain, sNewLine;
	sLine = pDataFile->GetLineString(1);
	int nPos = sLine.Find(',', 0);
	sLine.Delete(0, nPos);
	sNewLine.Format(_T("%d%s"), 0, sLine);
	pDataFile->ReplaceLine(1, sNewLine);

	strRstPath2.Format(_T("%s%s\\%s\\%s\\%04d.RST"), pDoc->WorkingInfo.System.sPathAoiUpVrsData,
		pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLayerUp,
		pDoc->WorkingInfo.LastJob.sLotUp,
		nSerial);
	char* pRtn = NULL;
	fpPCR = fopen(pRtn = StringToChar(strRstPath2), "w+"); if (pRtn) delete pRtn; pRtn = NULL;
	if (fpPCR == NULL)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("TROUBLE_SAVE_PIECEOUT_VRS"), MB_ICONWARNING | MB_OK);
	}
	fprintf(fpPCR, pRtn = StringToChar(pDataFile->GetAllString())); if (pRtn) delete pRtn; pRtn = NULL;
	if (fpPCR != NULL)
		fclose(fpPCR);

	delete pDataFile;


	CString sDummyPath;
	sDummyPath.Format(_T("%s%s\\%s\\%s\\%04d.PCR"), pDoc->WorkingInfo.System.sPathAoiUpVrsData,
		pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLayerUp,
		pDoc->WorkingInfo.LastJob.sLotUp,
		nSerial);

	fpPCR = fopen(pRtn = StringToChar(sDummyPath), "w+"); if (pRtn) delete pRtn; pRtn = NULL;
	if (fpPCR == NULL)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("TROUBLE_SAVE_PIECEOUT_VRS"), MB_ICONWARNING | MB_OK);
	}

	fprintf(fpPCR, "%04d,%s,%s,%s\n", nErr, pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLayerUp,
		pDoc->WorkingInfo.LastJob.sLotUp);

	fprintf(fpPCR, "%04d\n", 0);

	fprintf(fpPCR, "%s", pRtn = StringToChar(_T("���ϸ�����"))); if (pRtn) delete pRtn; pRtn = NULL;
	fprintf(fpPCR, "\n");

	if (fpPCR != NULL)
		fclose(fpPCR);



	sDummyPath.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVsShareUp, nSerial);

	fpPCR = fopen(pRtn = StringToChar(sDummyPath), "w+"); if (pRtn) delete pRtn; pRtn = NULL;
	if (fpPCR == NULL)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("TROUBLE_SAVE_PIECEOUT_VRS"), MB_ICONWARNING | MB_OK);
	}

	fprintf(fpPCR, "%04d,%s,%s,%s\n", nErr, pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLayerUp,
		pDoc->WorkingInfo.LastJob.sLotUp);

	fprintf(fpPCR, "%04d\n", 0);

	fprintf(fpPCR, "%s", pRtn = StringToChar(_T("���ϸ�����"))); if (pRtn) delete pRtn; pRtn = NULL;
	fprintf(fpPCR, "\n");

	if (fpPCR != NULL)
		fclose(fpPCR);

	return TRUE;
}

BOOL CManagerProcedure::MakeDummyDn(int nErr) // AOI ��� ����.
{
	m_nAoiLastSerial[1] = m_nAoiLastSerial[0] - 3;
	int nSerial = m_nAoiLastSerial[0] + m_nDummy[1];
	FILE *fpPCR = NULL;

	CString sMsg, strRstPath, strRstPath2, sDummyRst;
	strRstPath.Format(_T("%s%s\\%s\\%s\\%04d.RST"), pDoc->WorkingInfo.System.sPathAoiDnVrsData,
		pDoc->WorkingInfo.LastJob.sModelUp,
		//pDoc->WorkingInfo.LastJob.sModelDn,
		pDoc->WorkingInfo.LastJob.sLayerDn,
		pDoc->WorkingInfo.LastJob.sLotUp,
		//pDoc->WorkingInfo.LastJob.sLotDn,
		m_nAoiLastSerial[1]);

	CDataFile *pDataFile = new CDataFile;
	char* pRtn = NULL;
	if (!pDataFile->Open(strRstPath))
	{
		sMsg.Format(_T("%s File not found."), strRstPath);
		pView->MsgBox(sMsg);
		delete pDataFile;
		return FALSE;
	}
	CString sLine, sRemain, sNewLine;
	sLine = pDataFile->GetLineString(1);
	int nPos = sLine.Find(',', 0);
	sLine.Delete(0, nPos);
	sNewLine.Format(_T("%d%s"), 0, sLine);
	pDataFile->ReplaceLine(1, sNewLine);

	strRstPath2.Format(_T("%s%s\\%s\\%s\\%04d.RST"), pDoc->WorkingInfo.System.sPathAoiDnVrsData,
		pDoc->WorkingInfo.LastJob.sModelUp,
		//pDoc->WorkingInfo.LastJob.sModelDn,
		pDoc->WorkingInfo.LastJob.sLayerDn,
		pDoc->WorkingInfo.LastJob.sLotUp,
		//pDoc->WorkingInfo.LastJob.sLotDn,
		nSerial);
	fpPCR = fopen(pRtn = StringToChar(strRstPath2), "w+"); if (pRtn) delete pRtn; pRtn = NULL;
	if (fpPCR == NULL)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("TROUBLE_SAVE_PIECEOUT_VRS"), MB_ICONWARNING | MB_OK);
	}
	fprintf(fpPCR, pRtn = StringToChar(pDataFile->GetAllString())); if (pRtn) delete pRtn; pRtn = NULL;
	if (fpPCR != NULL)
		fclose(fpPCR);

	delete pDataFile;


	CString sDummyPath;
	sDummyPath.Format(_T("%s%s\\%s\\%s\\%04d.PCR"), pDoc->WorkingInfo.System.sPathAoiDnVrsData,
		pDoc->WorkingInfo.LastJob.sModelUp,
		//pDoc->WorkingInfo.LastJob.sModelDn,
		pDoc->WorkingInfo.LastJob.sLayerDn,
		pDoc->WorkingInfo.LastJob.sLotUp,
		//pDoc->WorkingInfo.LastJob.sLotDn,
		nSerial);

	fpPCR = fopen(pRtn = StringToChar(sDummyPath), "w+"); if (pRtn) delete pRtn; pRtn = NULL;
	if (fpPCR == NULL)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("TROUBLE_SAVE_PIECEOUT_VRS"), MB_ICONWARNING | MB_OK);
	}

	fprintf(fpPCR, "%04d,%s,%s,%s\n", nErr, pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLayerDn,
		pDoc->WorkingInfo.LastJob.sLotUp);
	//fprintf(fpPCR, "%04d,%s,%s,%s\n", nErr, pDoc->WorkingInfo.LastJob.sModelDn,
	//	pDoc->WorkingInfo.LastJob.sLayerDn,
	//	pDoc->WorkingInfo.LastJob.sLotDn);

	fprintf(fpPCR, "%04d\n", 0);

	fprintf(fpPCR, "%s", pRtn = StringToChar(_T("���ϸ�����"))); if (pRtn) delete pRtn; pRtn = NULL;
	fprintf(fpPCR, "\n");

	if (fpPCR != NULL)
		fclose(fpPCR);



	sDummyPath.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVsShareDn, nSerial);

	fpPCR = fopen(pRtn = StringToChar(sDummyPath), "w+"); delete pRtn;
	if (fpPCR == NULL)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("TROUBLE_SAVE_PIECEOUT_VRS"), MB_ICONWARNING | MB_OK);
	}

	fprintf(fpPCR, "%04d,%s,%s,%s\n", nErr, pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLayerDn,
		pDoc->WorkingInfo.LastJob.sLotUp);
	//fprintf(fpPCR, "%04d,%s,%s,%s\n", nErr, pDoc->WorkingInfo.LastJob.sModelDn,
	//	pDoc->WorkingInfo.LastJob.sLayerDn,
	//	pDoc->WorkingInfo.LastJob.sLotDn);

	fprintf(fpPCR, "%04d\n", 0);

	fprintf(fpPCR, "%s", pRtn = StringToChar(_T("���ϸ�����"))); if (pRtn) delete pRtn; pRtn = NULL;
	fprintf(fpPCR, "\n");

	if (fpPCR != NULL)
		fclose(fpPCR);

	return TRUE;
}

void CManagerProcedure::SetAoiDummyShot(int nAoi, int nDummy)
{
	pDoc->AoiDummyShot[nAoi] = nDummy;
	switch (nAoi)
	{
	case 0:
		pView->MpeWrite(_T("ML45068"), (long)nDummy);	// �˻�� (��) �۾����� ���� Shot�� - 20141104
		break;
	case 1:
		pView->MpeWrite(_T("ML45070"), (long)nDummy);	// �˻�� (��) �۾����� ���� Shot�� - 20141104
		break;
	}
}

int CManagerProcedure::GetAoiUpDummyShot()
{
	int nDummy = 0;
	TCHAR szData[200];
	CString sPath = pDoc->WorkingInfo.System.sPathAoiUpCurrInfo;
	if (0 < ::GetPrivateProfileString(_T("Dummy"), _T("Shot"), NULL, szData, sizeof(szData), sPath))
		nDummy = _tstoi(szData);
	else
		nDummy = 0;

	return nDummy;
}

int CManagerProcedure::GetAoiDnDummyShot()
{
	int nDummy = 0;
	TCHAR szData[200];
	CString sPath = pDoc->WorkingInfo.System.sPathAoiDnCurrInfo;
	if (0 < ::GetPrivateProfileString(_T("Dummy"), _T("Shot"), NULL, szData, sizeof(szData), sPath))
		nDummy = _tstoi(szData);
	else
		nDummy = 0;

	return nDummy;
}

int CManagerProcedure::GetAoiUpSerial()
{
	int nSerial = 0;
	TCHAR szData[200];
	CString sPath = pDoc->WorkingInfo.System.sPathAoiUpCurrInfo;
	if (0 < ::GetPrivateProfileString(_T("Infomation"), _T("Current Serial"), NULL, szData, sizeof(szData), sPath))
		nSerial = _tstoi(szData);
	else
		nSerial = 0;
	if (nSerial > 1)
		nSerial--;

	return nSerial;
}

BOOL CManagerProcedure::GetAoiUpVsStatus()
{
	BOOL bVsStatus = FALSE;
	TCHAR szData[200];
	CString sPath = pDoc->WorkingInfo.System.sPathAoiUpCurrInfo;
	if (0 < ::GetPrivateProfileString(_T("Infomation"), _T("Current VS Status"), NULL, szData, sizeof(szData), sPath))
		bVsStatus = _tstoi(szData) > 0 ? TRUE : FALSE;

	return bVsStatus;
}

int CManagerProcedure::GetAoiDnSerial()
{
	int nSerial = 0;
	TCHAR szData[200];
	CString sPath = pDoc->WorkingInfo.System.sPathAoiDnCurrInfo;
	if (0 < ::GetPrivateProfileString(_T("Infomation"), _T("Current Serial"), NULL, szData, sizeof(szData), sPath))
		nSerial = _tstoi(szData);
	else
		nSerial = 0;
	if (nSerial > 1)
		nSerial--;

	return nSerial;
}

BOOL CManagerProcedure::GetAoiDnVsStatus()
{
	BOOL bVsStatus = FALSE;
	TCHAR szData[200];
	CString sPath = pDoc->WorkingInfo.System.sPathAoiDnCurrInfo;
	if (0 < ::GetPrivateProfileString(_T("Infomation"), _T("Current VS Status"), NULL, szData, sizeof(szData), sPath))
		bVsStatus = _tstoi(szData) > 0 ? TRUE : FALSE;

	return bVsStatus;
}

BOOL CManagerProcedure::IsSetLotEnd()
{
	if (m_nLotEndSerial > 0)
		return TRUE;
	return FALSE;
}

void CManagerProcedure::SetListBuf()	// pDoc->m_ListBuf�� ���� ������ �ø����ȣ�� ������ �簻����.
{
	pDoc->m_ListBuf[0].Clear();
	if (ChkBufUp(m_pBufSerial[0], m_nBufTot[0]))
	{
		for (int i = 0; i < m_nBufTot[0]; i++)
			pDoc->m_ListBuf[0].Push(m_pBufSerial[0][i]);
	}

	pDoc->m_ListBuf[1].Clear();
	if (ChkBufDn(m_pBufSerial[1], m_nBufTot[1]))
	{
		for (int i = 0; i < m_nBufTot[1]; i++)
			pDoc->m_ListBuf[1].Push(m_pBufSerial[1][i]);
	}
}

int CManagerProcedure::GetTotDefPcs(int nSerial)
{
	return pView->GetTotDefPcs(nSerial);
}

void CManagerProcedure::UpdateYield()
{
	pView->UpdateYield();
}

BOOL CManagerProcedure::ChkLotCutPos()
{
	return pView->ChkLotCutPos();
}

void CManagerProcedure::UpdateRst()
{
	pView->UpdateRst();
}

void CManagerProcedure::UpdateWorking()
{
	pView->UpdateWorking();
}

BOOL CManagerProcedure::TwoPointAlign0(int nPos)
{
	return pView->TwoPointAlign0(nPos);
}

BOOL CManagerProcedure::TwoPointAlign1(int nPos)
{
	return pView->TwoPointAlign1(nPos);
}

BOOL CManagerProcedure::IsShowLive()
{
	return pView->IsShowLive();
}

void CManagerProcedure::ShowLive(BOOL bShow)
{
	pView->ShowLive(bShow);
}

void CManagerProcedure::Shift2Mk()
{
	pView->Shift2Mk();
}

void CManagerProcedure::Shift2Buf()	// ���������� ������ �ø���� Share������ �ø����� �������� Ȯ�� �� �ű�.
{
	pView->Shift2Buf();
}

BOOL CManagerProcedure::IsBuffer(int nNum)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (bDualTest)
	{
		if (m_nBufTot[0] > nNum && m_nBufTot[1] > nNum) // [0]: AOI-Up , [1]: AOI-Dn
			return TRUE;

		if (m_bLastProc)
		{
			if ((m_nBufTot[0] > nNum || !m_bIsBuf[0])
				&& (m_nBufTot[1] > nNum || !m_bIsBuf[1])) // [0]: AOI-Up , [1]: AOI-Dn
				return TRUE;
		}
	}
	else
	{
		if (m_nBufTot[0] > nNum) // [0]: AOI-Up
			return TRUE;
	}

	return FALSE;
}

BOOL CManagerProcedure::IsBufferUp()
{
	if (m_nBufTot[0] > 0)
		return TRUE;
	return FALSE;
}

BOOL CManagerProcedure::IsBufferDn()
{
	if (m_nBufTot[1] > 0)
		return TRUE;
	return FALSE;
}

int CManagerProcedure::GetBuffer(int *pPrevSerial)
{
	int nS0 = GetBufferUp(pPrevSerial);
	int nS1 = GetBufferDn(pPrevSerial);
	if (nS0 != nS1)
		return 0;
	return nS0;
}

int CManagerProcedure::GetBufferUp(int *pPrevSerial)
{
	if (IsBufferUp())
		return m_pBufSerial[0][0];
	else if (pPrevSerial)
		*pPrevSerial = m_pBufSerial[0][0];
	return 0;
}

int CManagerProcedure::GetBufferDn(int *pPrevSerial)
{
	if (IsBufferDn())
		return m_pBufSerial[1][0];
	else if (pPrevSerial)
		*pPrevSerial = m_pBufSerial[1][0];
	return 0;
}

BOOL CManagerProcedure::IsBuffer0()
{
	if (m_nBufTot[0] > 0 && m_nBufTot[1] > 0)
		return TRUE;
	return FALSE;
}

BOOL CManagerProcedure::IsBufferUp0()
{
	if (m_nBufTot[0] > 0)
		return TRUE;
	return FALSE;
}

BOOL CManagerProcedure::IsBufferDn0()
{
	if (m_nBufTot[1] > 0)
		return TRUE;
	return FALSE;
}

int CManagerProcedure::GetBuffer0(int *pPrevSerial)
{
	int nS0 = GetBufferUp0(pPrevSerial);
	int nS1 = GetBufferDn0(pPrevSerial);
	if (nS0 != nS1)
		return 0;
	return nS0;
}

int CManagerProcedure::GetBufferUp0(int *pPrevSerial)
{
	if (IsBufferUp0())
		return m_pBufSerial[0][0];
	else if (pPrevSerial)
		*pPrevSerial = m_pBufSerial[0][0];
	return 0;
}

int CManagerProcedure::GetBufferDn0(int *pPrevSerial)
{
	if (IsBufferDn0())
		return m_pBufSerial[1][0];
	else if (pPrevSerial)
		*pPrevSerial = m_pBufSerial[1][0];
	return 0;
}

BOOL CManagerProcedure::IsBuffer1()
{
	if (m_nBufTot[0] > 1 && m_nBufTot[1] > 1)
		return TRUE;
	return FALSE;
}

BOOL CManagerProcedure::IsBufferUp1()
{
	if (m_nBufTot[0] > 1)
		return TRUE;
	return FALSE;
}

BOOL CManagerProcedure::IsBufferDn1()
{
	if (m_nBufTot[1] > 1)
		return TRUE;
	return FALSE;
}

int CManagerProcedure::GetBuffer1(int *pPrevSerial)
{
	int nS0 = GetBufferUp1(pPrevSerial);
	int nS1 = GetBufferDn1(pPrevSerial);
	if (nS0 != nS1)
		return 0;
	return nS0;
}

int CManagerProcedure::GetBufferUp1(int *pPrevSerial)
{
	if (IsBufferUp1())
		return m_pBufSerial[0][1];
	else if (pPrevSerial)
		*pPrevSerial = m_pBufSerial[0][1];
	return 0;
}

int CManagerProcedure::GetBufferDn1(int *pPrevSerial)
{
	if (IsBufferDn1())
		return m_pBufSerial[1][1];
	else if (pPrevSerial)
		*pPrevSerial = m_pBufSerial[1][1];
	return 0;
}

BOOL CManagerProcedure::IsShare()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (bDualTest)
	{
		if (m_bWaitPcr[0] && m_bWaitPcr[1])
		{
			if (IsShareUp() && IsShareDn())
			{
				m_bWaitPcr[0] = FALSE;
				m_bWaitPcr[1] = FALSE;
				return TRUE;
			}
		}
		else if (m_bWaitPcr[0] && !m_bWaitPcr[1])
		{
			if (IsShareUp())
			{
				m_bWaitPcr[0] = FALSE;
				return TRUE;
			}
		}
		else if (!m_bWaitPcr[0] && m_bWaitPcr[1])
		{
			if (IsShareDn())
			{
				m_bWaitPcr[1] = FALSE;
				return TRUE;
			}
		}
		else
		{
			if (IsShareUp() || IsShareDn())
				return TRUE;
		}
	}
	else
	{
		if (m_bWaitPcr[0])
		{
			if (IsShareUp())
			{
				m_bWaitPcr[0] = FALSE;
				return TRUE;
			}
		}
		else
		{
			if (IsShareUp())
				return TRUE;
		}
	}
	return FALSE;
}

BOOL CManagerProcedure::IsShareUp()
{
	return pDoc->Status.PcrShare[0].bExist;
}

BOOL CManagerProcedure::IsShareDn()
{
	return pDoc->Status.PcrShare[1].bExist;
}

BOOL CManagerProcedure::IsVsShare()
{
	int nSerial;
	BOOL bExist = ChkVsShare(nSerial);
	return bExist;
}

int CManagerProcedure::GetShareUp()
{
	return pDoc->Status.PcrShare[0].nSerial;
}

int CManagerProcedure::GetShareDn()
{
	return pDoc->Status.PcrShare[1].nSerial;
}

BOOL CManagerProcedure::SetSerial(int nSerial, BOOL bDumy)
{
	return pView->SetSerial(nSerial, bDumy);
}

void CManagerProcedure::SetReject()
{
	if (!pView->m_mgrPunch)
		return;
	return pView->m_mgrPunch->SetReject();
}

void CManagerProcedure::DoShift2Mk()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nSerial;

	if (m_bLastProc && m_nBufUpSerial[0] == m_nLotEndSerial)
	{
		nSerial = m_nBufUpSerial[0];
		//if (nSerial > 0 && (nSerial % 2)) // First Shot number must be odd.
		if (nSerial > 0) // 20130202
			m_bTHREAD_SHIFT2MK = TRUE;
	}
	else
	{
		nSerial = m_nBufUpSerial[0];
		if (!m_bCont)
		{
			//if (nSerial > 0 && (nSerial % 2)) // First Shot number must be odd.
			if (nSerial > 0)
				m_bTHREAD_SHIFT2MK = TRUE;
			else
				Stop();
		}
		else
		{
			if (nSerial > 0)
				m_bTHREAD_SHIFT2MK = TRUE;
			else
				Stop();
		}
	}
}

void CManagerProcedure::SetMkFdLen()
{
	int nLast = pDoc->GetLastShotMk();
	pDoc->WorkingInfo.Motion.sMkFdTotLen.Format(_T("%.1f"), (double)nLast * _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen));

	CString sData, sPath = PATH_WORKING_INFO;
	sData = pDoc->WorkingInfo.Motion.sMkFdTotLen;
	::WritePrivateProfileString(_T("Motion"), _T("MARKING_FEEDING_SERVO_TOTAL_DIST"), sData, sPath);
}

void CManagerProcedure::SetLotEnd(int nSerial)
{
	pView->SetLotEnd(nSerial);
}

void CManagerProcedure::SetCycTime()
{
	DWORD dCur = GetTickCount();
	if (m_dwCycSt > 0)
	{
		m_dwCycTim = (double)(dCur - m_dwCycSt);
		if (m_dwCycTim < 0.0)
			m_dwCycTim *= (-1.0);
	}
	else
		m_dwCycTim = 0.0;
}

int CManagerProcedure::GetCycTime()
{
	if (m_dwCycTim < 0)
		m_dwCycTim = 0;

	int nTim = int(m_dwCycTim);
	return nTim;
}

void CManagerProcedure::SetAlignPos()
{
	pView->SetAlignPos();
}

BOOL CManagerProcedure::GetAoiDnInfo(int nSerial, int *pNewLot, BOOL bFromBuf)
{
	return pView->GetAoiDnInfo(nSerial, pNewLot, bFromBuf);
}

void CManagerProcedure::InitInfo()
{
	pView->InitInfo();
}

void CManagerProcedure::ModelChange(int nAoi) // 0 : AOI-Up , 1 : AOI-Dn 
{
	pView->ModelChange(nAoi);
}

BOOL CManagerProcedure::ChkLastProcFromEng()
{
	return pView->ChkLastProcFromEng();
}

BOOL CManagerProcedure::ChkStShotNum()
{
	CString sMsg;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pDoc->m_ListBuf[0].nTot == 0)
	{
		if (m_nShareUpS > 0 && !(m_nShareUpS % 2))
		{
			sMsg.Format(_T("AOI ����� �ø����� ¦���� �����Ͽ����ϴ�.\r\n- �ø��� ��ȣ: %d"), m_nShareUpS);
			MsgBox(sMsg);
			return FALSE;
		}
	}

	if (bDualTest)
	{
		if (pDoc->m_ListBuf[1].nTot == 0)
		{
			if (m_nShareDnS > 0 && !(m_nShareDnS % 2))
			{
				sMsg.Format(_T("AOI �ϸ��� �ø����� ¦���� �����Ͽ����ϴ�.\r\n- �ø��� ��ȣ: %d"), m_nShareDnS);
				MsgBox(sMsg);
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL CManagerProcedure::ChkContShotNum()
{
	CString sMsg;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nLastShot = pView->GetLastShotMk();

	if (pDoc->m_ListBuf[0].nTot == 0)
	{
		if (m_nShareUpS > 0 && nLastShot + 1 != m_nShareUpS)
		{
			sMsg.Format(_T("AOI ����� ����Shot(%d)�� ������Shot(%d)�� �ҿ����Դϴ�.\r\n��� �����Ͻðڽ��ϱ�?"), m_nShareUpS, nLastShot);
			if (IDNO == MsgBox(sMsg, 0, MB_YESNO))
				return FALSE;
		}
	}

	return TRUE;
}

BOOL CManagerProcedure::GetAoiUpInfo(int nSerial, int *pNewLot, BOOL bFromBuf)
{
	return pView->GetAoiUpInfo(nSerial, pNewLot, bFromBuf);
}

BOOL CManagerProcedure::ChkLastProcFromUp()
{
	return pView->ChkLastProcFromUp();
}

BOOL CManagerProcedure::IsSameUpDnLot()
{
	if (pDoc->Status.PcrShare[0].sLot == pDoc->Status.PcrShare[1].sLot)
		return TRUE;

	return FALSE;
}

int CManagerProcedure::GetAutoStep()
{
	return m_nStepAuto;
}

BOOL CManagerProcedure::LoadPcrUp(int nSerial, BOOL bFromShare)
{
	return pView->LoadPcrUp(nSerial, bFromShare);
}

BOOL CManagerProcedure::LoadPcrDn(int nSerial, BOOL bFromShare)
{
	return pView->LoadPcrDn(nSerial, bFromShare);
}

void CManagerProcedure::CompletedMk(int nCam) // 0: Only Cam0, 1: Only Cam1, 2: Cam0 and Cam1, 3: None
{
	int nSerial = -1;

	switch (nCam)
	{
	case 0:
		nSerial = m_nBufUpSerial[0];
		break;
	case 1:
		nSerial = m_nBufUpSerial[1];
		break;
	case 2:
		nSerial = m_nBufUpSerial[1];
		break;
	case 3:
		return;
	default:
		return;
	}

	pDoc->SetCompletedSerial(nSerial);
}

BOOL CManagerProcedure::IsInitPos0()
{
	if (!pView->m_mgrPunch)
		return FALSE;
	return pView->m_mgrPunch->IsInitPos0();
}

BOOL CManagerProcedure::IsInitPos1()
{
	if (!pView->m_mgrPunch)
		return FALSE;
	return pView->m_mgrPunch->IsInitPos1();
}

BOOL CManagerProcedure::DoElecChk(CString &sRst)
{
	return pView->DoElecChk(sRst);
}

BOOL CManagerProcedure::IsMoveDone()
{
	if (!pView->m_mgrPunch)
		return FALSE;
	return pView->m_mgrPunch->IsMoveDone();
}

BOOL CManagerProcedure::IsMkDone()
{
	if (!pView->m_mgrPunch)
		return FALSE;
	if (pView->m_mgrPunch->m_bDoneMk[0] && pView->m_mgrPunch->m_bDoneMk[1] && !m_bTHREAD_DISP_DEF)
		return TRUE;

	return FALSE;
}

void CManagerProcedure::ChgLot()
{
	pView->ChgLot();
}

BOOL CManagerProcedure::ChkLotEnd(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.25"));
		return FALSE;
	}

	BOOL bEnd0 = ChkLotEndUp(nSerial);
	BOOL bEnd1 = ChkLotEndDn(nSerial);
	if (bEnd0 || bEnd1)
		return TRUE;
	return FALSE;
}

BOOL CManagerProcedure::ChkLotEndUp(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.26"));
		return 0;
	}

	CString sPath;
	sPath.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufUp, nSerial);
	return pDoc->ChkLotEnd(sPath);
}

BOOL CManagerProcedure::ChkLotEndDn(int nSerial)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return FALSE;

	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.27"));
		return 0;
	}

	CString sPath;
	sPath.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufDn, nSerial);
	return pDoc->ChkLotEnd(sPath);
}

BOOL CManagerProcedure::MoveAlign0(int nPos)
{
	if (!pView->m_mgrPunch)
		return FALSE;
	return pView->m_mgrPunch->MoveAlign0(nPos);
}

BOOL CManagerProcedure::MoveAlign1(int nPos)
{
	if (!pView->m_mgrPunch)
		return FALSE;
	return pView->m_mgrPunch->MoveAlign1(nPos);
}

BOOL CManagerProcedure::FourPointAlign0(int nPos, BOOL bDraw)
{
	return pView->FourPointAlign0(nPos);
}

BOOL CManagerProcedure::FourPointAlign1(int nPos, BOOL bDraw)
{
	return pView->FourPointAlign1(nPos);
}

void CManagerProcedure::DoAuto()
{
	// ��ŷ���� ��ȣ�� Ȯ��
	DoAtuoGetMkStSignal();

	// LastProc Start
	DoAutoSetLastProcAtPlc();

	// AOI Feeding Offset Start on LastProc
	DoAutoSetFdOffsetLastProc();

	// AOI Feeding Offset Start
	DoAutoSetFdOffset();

	// Engrave Feeding Offset Start
	DoAutoSetFdOffsetEngrave();

	// CycleStop
	DoAutoChkCycleStop();

	// DispMsg
	DoAutoDispMsg();

	// Check Share Folder Start
	DoAutoChkShareFolder();

	// Marking Start
	DoAutoMarking();

	// Engrave Marking Start
	DoAutoMarkingEngrave();

}

void CManagerProcedure::DoAtuoGetMkStSignal()
{
#ifdef USE_MPE
	if (!m_bMkSt)
	{
		if (IsRun())
		{
			if (pDoc->m_pMpeSignal[1] & (0x01 << 0) || m_bMkStSw) // AlignTest		// ��ŷ����(PC�� Ȯ���ϰ� Reset��Ŵ.)-20141029
			{
				pDoc->Log(_T("PLC: ��ŷ����(PC�� Ȯ���ϰ� Reset��Ŵ.)"));
				m_bMkStSw = FALSE;
				pView->MpeWrite(_T("MB440110"), 0);			// ��ŷ����(PC�� Ȯ���ϰ� Reset��Ŵ.)-20141029

				if (pDoc->m_pMpeSignal[0] & (0x01 << 1))	// ��ŷ�� Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)-20141030
				{
					pDoc->Log(_T("PLC: Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)"));
					pView->MpeWrite(_T("MB440101"), 0);		// ��ŷ�� Feeding�Ϸ�
				}

				m_bMkSt = TRUE;
				m_nMkStAuto = MK_ST;
				::WritePrivateProfileString(_T("Last Job"), _T("MkSt"), _T("1"), PATH_WORKING_INFO);

				if (!pDoc->GetLastShotMk())
					pView->SetLotSt();		// ���ο� ��Ʈ�� ���۽ð��� ������. // ��ü�ӵ��� ó�� ���۵Ǵ� �ð� ����.
			}
		}
	}
#endif
}

void CManagerProcedure::DoAutoSetLastProcAtPlc() 
{
	if (m_bLastProc)
	{
		switch (m_nLastProcAuto)
		{
		case LAST_PROC:	// �ܷ�ó�� 1
			if (IsRun())
			{
				if (MODE_INNER != pDoc->GetTestMode())
				{
					if (pView->ChkLastProcFromUp())
					{
						pDoc->Log(_T("PC: �ܷ�ó�� AOI(��) ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)"));
						pView->MpeWrite(_T("MB440185"), 1);			// �ܷ�ó�� AOI(��) ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141112
						pView->MpeWrite(_T("MB44012B"), 1);			// AOI �� : PCR���� Received
					}
					else
					{
						pDoc->Log(_T("PC: �ܷ�ó�� AOI(��) ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)"));
						pView->MpeWrite(_T("MB440186"), 1);			// �ܷ�ó�� AOI(��) ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141112
					}
				}
				else
				{
					if (pView->ChkLastProcFromEng())
					{
						pDoc->Log(_T("PC: �ܷ�ó�� ���κ� ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)"));
						pView->MpeWrite(_T("MB44019D"), 1);			// �ܷ�ó�� ���κ� ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141112
					}
					else if (pView->ChkLastProcFromUp())
					{
						pDoc->Log(_T("PC: �ܷ�ó�� AOI(��) ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)"));
						pView->MpeWrite(_T("MB440185"), 1);			// �ܷ�ó�� AOI(��) ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141112
						pView->MpeWrite(_T("MB44012B"), 1);			// AOI �� : PCR���� Received
					}
					else
					{
						pDoc->Log(_T("PC: �ܷ�ó�� AOI(��) ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)"));
						pView->MpeWrite(_T("MB440186"), 1);			// �ܷ�ó�� AOI(��) ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141112
					}
				}

				m_nLastProcAuto++;
			}
			break;
		case LAST_PROC + 1:
			pDoc->Log(_T("PC: �ܷ�ó��(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)"));
			pView->MpeWrite(_T("MB440181"), 1);			// �ܷ�ó��(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141031
			m_nLastProcAuto++;
			break;
		case LAST_PROC + 2:
			;
			break;
		}
	}

}

void CManagerProcedure::DoAutoSetFdOffsetLastProc()
{
#ifdef USE_MPE
	BOOL bOn0 = (pDoc->m_pMpeIb[10] & (0x01 << 11)) ? TRUE : FALSE;		// �˻�� �� �ڵ� ���� <-> X432B I/F
	BOOL bOn1 = (pDoc->m_pMpeIb[14] & (0x01 << 11)) ? TRUE : FALSE;		// �˻�� �� �ڵ� ���� <-> X442B I/F

	if (bOn0 && !(m_Flag & (0x01 << 2)))
	{
		m_Flag |= (0x01 << 2);
	}
	else if (!bOn0 && (m_Flag & (0x01 << 2)))
	{
		m_Flag &= ~(0x01 << 2);

		m_bAoiTest[0] = FALSE;
		m_bWaitPcr[0] = FALSE;
		m_bAoiTestF[0] = FALSE;
		m_bAoiFdWriteF[0] = FALSE;
		pView->MpeWrite(_T("MB440111"), 0); // �˻��(��) Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)-20141103
		pDoc->Log(_T("PLC: �˻��(��) Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� MB440111 Reset��Ŵ.)"));
	}

	if (bOn1 && !(m_Flag & (0x01 << 3)))
	{
		m_Flag |= (0x01 << 3);
	}
	else if (!bOn1 && (m_Flag & (0x01 << 3)))
	{
		m_Flag &= ~(0x01 << 3);

		m_bAoiTest[1] = FALSE;
		m_bWaitPcr[1] = FALSE;
		m_bAoiTestF[1] = FALSE;
		m_bAoiFdWriteF[1] = FALSE;
		pView->MpeWrite(_T("MB440112"), 0); // �˻��(��) Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)-20141103
		pDoc->Log(_T("PLC: �˻��(��) Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� MB440112 Reset��Ŵ.)"));
	}
#endif
}

void CManagerProcedure::DoAutoSetFdOffset()
{
#ifdef USE_MPE
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	double dAveX, dAveY;
	CfPoint OfStUp, OfStDn;

	if (pDoc->m_pMpeSignal[1] & (0x01 << 3) && !m_bAoiTestF[0])		// �˻��(��) �˻���-20141103
	{
		m_bAoiTestF[0] = TRUE;
		m_bAoiTest[0] = TRUE;
		m_bWaitPcr[0] = TRUE;
	}
	else if (!(pDoc->m_pMpeSignal[1] & (0x01 << 3)) && m_bAoiTestF[0])
	{
		m_bAoiTestF[0] = FALSE;
		m_bAoiTest[0] = FALSE;//
		m_bAoiFdWriteF[0] = FALSE;

		pView->MpeWrite(_T("MB440111"), 0); // �˻��(��) Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)-20141103
		pDoc->Log(_T("PLC: �˻��(��) Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� MB440111 Reset��Ŵ.)"));
	}

	if (pDoc->m_pMpeSignal[1] & (0x01 << 4) && !m_bAoiTestF[1])		// �˻��(��) �˻���-20141103
	{
		m_bAoiTestF[1] = TRUE;
		m_bAoiTest[1] = TRUE;
		m_bWaitPcr[1] = TRUE;
	}
	else if (!(pDoc->m_pMpeSignal[1] & (0x01 << 4)) && m_bAoiTestF[1])
	{
		m_bAoiTestF[1] = FALSE;
		m_bAoiTest[1] = FALSE;//
		m_bAoiFdWriteF[1] = FALSE;
		pView->MpeWrite(_T("MB440112"), 0); // �˻��(��) Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)-20141103
		pDoc->Log(_T("PLC: �˻��(��) Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� MB440112 Reset��Ŵ.)"));
	}


	if (pDoc->m_pMpeSignal[1] & (0x01 << 1) && !m_bAoiFdWrite[0])		// �˻��(��) Feeding Offset Write �Ϸ�-20141103
		m_bAoiFdWrite[0] = TRUE;
	else if (!(pDoc->m_pMpeSignal[1] & (0x01 << 1)) && m_bAoiFdWrite[0])
		m_bAoiFdWrite[0] = FALSE;

	if (pDoc->m_pMpeSignal[1] & (0x01 << 2) && !m_bAoiFdWrite[1])		// �˻��(��) Feeding Offset Write �Ϸ�-20141103
		m_bAoiFdWrite[1] = TRUE;
	else if (!(pDoc->m_pMpeSignal[1] & (0x01 << 2)) && m_bAoiFdWrite[1])
		m_bAoiFdWrite[1] = FALSE;


	if (bDualTest) // 20160721-syd-temp
	{
		if ((m_bAoiFdWrite[0] && m_bAoiFdWrite[1])
			&& (!m_bAoiFdWriteF[0] && !m_bAoiFdWriteF[1]))
		{
			m_bAoiFdWriteF[0] = TRUE;
			m_bAoiFdWriteF[1] = TRUE;

			pView->GetAoiUpOffset(OfStUp);
			pView->GetAoiDnOffset(OfStDn);

			dAveX = OfStUp.x;
			dAveY = OfStUp.y; // syd - 20230327
							  //dAveX = (OfStUp.x + OfStDn.x) / 2.0;
							  //dAveY = (OfStUp.y + OfStDn.y) / 2.0;

			pView->m_dAoiUpFdOffsetX = OfStUp.x;
			pView->m_dAoiUpFdOffsetY = OfStUp.y;
			pView->m_dAoiDnFdOffsetX = OfStDn.x;
			pView->m_dAoiDnFdOffsetY = OfStDn.y;

			pView->MpeWrite(_T("ML45064"), (long)(-1.0*dAveX*1000.0));
			pView->MpeWrite(_T("MB440111"), 0); // �˻��(��) Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)-20141103
			pView->MpeWrite(_T("MB440112"), 0); // �˻��(��) Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)-20141103
			pDoc->Log(_T("PLC: �˻��(��MB440111,��MB440112) Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)"));

		}
		else if ((!m_bAoiFdWrite[0] && !m_bAoiFdWrite[1])
			&& (m_bAoiFdWriteF[0] && m_bAoiFdWriteF[1]))
		{
			m_bAoiFdWriteF[0] = FALSE;
			m_bAoiFdWriteF[1] = FALSE;
			m_bAoiTest[0] = FALSE;
			m_bAoiTest[1] = FALSE;
		}

		if (m_bAoiTest[0] && !m_bAoiTest[1])
		{
			if (m_bAoiFdWrite[0] && !m_bAoiFdWriteF[0])
			{
				m_bAoiFdWriteF[0] = TRUE;

				pView->GetAoiUpOffset(OfStUp);

				pView->m_dAoiUpFdOffsetX = OfStUp.x;
				pView->m_dAoiUpFdOffsetY = OfStUp.y;
				pView->m_dAoiDnFdOffsetX = 0.0;
				pView->m_dAoiDnFdOffsetY = 0.0;

				pView->MpeWrite(_T("ML45064"), (long)(-1.0*OfStUp.x*1000.0));
				pView->MpeWrite(_T("MB440111"), 0); // �˻��(��) Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)-20141103
				pDoc->Log(_T("PLC: �˻��(��MB440111) Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)"));
			}
			else if (!m_bAoiFdWrite[0] && m_bAoiFdWriteF[0])
			{
				m_bAoiFdWriteF[0] = FALSE;
				m_bAoiTest[0] = FALSE;
			}
		}

		if (!m_bAoiTest[0] && m_bAoiTest[1])
		{
			if (m_bAoiFdWrite[1] && !m_bAoiFdWriteF[1])
			{
				m_bAoiFdWriteF[1] = TRUE;

				pView->GetAoiDnOffset(OfStDn);

				pView->m_dAoiUpFdOffsetX = 0.0;
				pView->m_dAoiUpFdOffsetY = 0.0;
				pView->m_dAoiDnFdOffsetX = OfStDn.x;
				pView->m_dAoiDnFdOffsetY = OfStDn.y;

				pView->MpeWrite(_T("ML45064"), (long)(-1.0*OfStDn.x*1000.0));
				pView->MpeWrite(_T("MB440112"), 0); // �˻��(��) Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)-20141103
				pDoc->Log(_T("PLC: �˻��(��MB440112) Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)"));
			}
			else if (!m_bAoiFdWrite[1] && m_bAoiFdWriteF[1])
			{
				m_bAoiFdWriteF[1] = FALSE;
				m_bAoiTest[1] = FALSE;
			}
		}
	}
	else // 20160721-syd-temp
	{
		if (m_bAoiFdWrite[0] && !m_bAoiFdWriteF[0])
		{
			m_bAoiFdWriteF[0] = TRUE;

			pView->GetAoiUpOffset(OfStUp);

			dAveX = (OfStUp.x);
			dAveY = (OfStUp.y);

			pView->m_dAoiUpFdOffsetX = dAveX;
			pView->m_dAoiUpFdOffsetY = dAveY;

			pView->MpeWrite(_T("ML45064"), (long)(-1.0*dAveX*1000.0));	// �˻�� Feeding �ѷ� Offset(*1000, +:�� ����, -�� ����)
			pView->MpeWrite(_T("MB440111"), 0); // �˻��(��) Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)-20141103
			pView->MpeWrite(_T("MB440112"), 0); // �˻��(��) Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)-20141103  // 20160721-syd-temp
			pDoc->Log(_T("PLC: �˻��(��MB440111,��MB440112) Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)"));
		}
		else if (!m_bAoiFdWrite[0] && m_bAoiFdWriteF[0])
		{
			m_bAoiFdWriteF[0] = FALSE;
			m_bAoiTest[0] = FALSE;
		}
	}
#endif

}

void CManagerProcedure::DoAutoSetFdOffsetEngrave()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	double dAveX, dAveY;
	CfPoint OfSt;

	if ((pDoc->BtnStatus.EngAuto.IsOnMking && !m_bEngTestF)
		|| (pDoc->BtnStatus.EngAuto.IsOnRead2d && !m_bEngTestF)) // ���κ� �˻���
	{
		m_bEngTestF = TRUE;
		m_bEngTest = TRUE;
	}
	else if ((!pDoc->BtnStatus.EngAuto.IsOnMking && m_bEngTestF)
		|| (!pDoc->BtnStatus.EngAuto.IsOnRead2d && m_bEngTestF))
	{
		m_bEngTestF = FALSE;
		m_bEngTest = FALSE;
		m_bEngFdWriteF = FALSE;
#ifdef USE_MPE
		pView->MpeWrite(_T("MB44011A"), 0);							// ���κ� Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)
		pDoc->Log(_T("PC: ���κ� Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)"));
#endif
	}

	if (pDoc->m_pMpeSignal[1] & (0x01 << 10) && !m_bEngFdWrite)		// ���κ� Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)
	{
		m_bEngFdWrite = TRUE;
	}
	else if (!(pDoc->m_pMpeSignal[1] & (0x01 << 10)) && m_bEngFdWrite)
	{
		m_bEngFdWrite = FALSE;
	}


	if (m_bEngFdWrite && !m_bEngFdWriteF)
	{
		m_bEngFdWriteF = TRUE;

		//if (MODE_INNER == pDoc->GetTestMode())
		//	GetCurrentInfoEng();

		pView->GetEngOffset(OfSt);

		dAveX = OfSt.x;
		dAveY = OfSt.y;

		pView->m_dEngFdOffsetX = OfSt.x;
		pView->m_dEngFdOffsetY = OfSt.y;
		pDoc->Log(_T("PC: ���κ� Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)"));
#ifdef USE_MPE
		pView->MpeWrite(_T("ML45078"), (long)(dAveX*1000.0));	// ���κ� Feeding �ѷ� Offset(*1000, +:�� ����, -�� ����, PC�� ���� PLC���� ����)
		pView->MpeWrite(_T("MB44011A"), 0);						// ���κ� Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)
		Sleep(10);
#endif
	}
	else if (!m_bEngFdWrite && m_bEngFdWriteF)
	{
		m_bEngFdWriteF = FALSE;
		m_bEngTest = FALSE;
	}
}

void CManagerProcedure::DoAutoChkCycleStop()
{
	if (!pView->m_mgrPunch)
		return;
	if (pView->IsCycleStop())
	{
		pView->CycleStop(FALSE);
		//TowerLamp(RGB_YELLOW, TRUE);
		pView->Buzzer(TRUE);
		//MyMsgBox(pDoc->m_sAlmMsg);
		if (!pDoc->m_sAlmMsg.IsEmpty())
		{
			pDoc->Log(pDoc->m_sAlmMsg);
			pView->MsgBox(pDoc->m_sAlmMsg, 0, 0, DEFAULT_TIME_OUT, FALSE);

			if (pDoc->m_sAlmMsg == pView->m_mgrPunch->GetAoiUpAlarmRestartMsg())
			{
				pView->SetAoiUpAutoStep(2); // Wait for AOI �˻���� ��ȣ.
				Sleep(300);
				pView->MpeWrite(_T("MB44013B"), 1); // �˻�� ��� ���۾� (���۽�ȣ) : PC�� On��Ű�� PLC�� Off
				pDoc->Log(_T("PC: �˻�� ��� ���۾� (���۽�ȣ) : PC�� On��Ű�� PLC�� Off"));
			}
			else if (pDoc->m_sAlmMsg == pView->m_mgrPunch->GetAoiDnAlarmRestartMsg())
			{
				pView->SetAoiDnAutoStep(2); // Wait for AOI �˻���� ��ȣ.
				Sleep(300);
				pView->MpeWrite(_T("MB44013C"), 1); // �˻�� �Ϻ� ���۾� (���۽�ȣ) : PC�� On��Ű�� PLC�� Off
				pDoc->Log(_T("PC: �˻�� �Ϻ� ���۾� (���۽�ȣ) : PC�� On��Ű�� PLC�� Off"));
			}
		}
		pDoc->m_sAlmMsg = _T("");
		pDoc->m_sIsAlmMsg = _T("");
		pDoc->m_sPrevAlmMsg = _T("");
	}
}

void CManagerProcedure::DoAutoDispMsg()
{
	if (!pView->m_mgrPunch)
		return;

	BOOL bDispMsg = FALSE;
	int idx, nStepDispMsg;

	// [2] : �����ҷ�-��, [3] : �����ҷ�-��, [5] : nSerialL <= 0, [6] : CopyDefImg ���� Camera,  
	// [7] : CopyDefImg ���� Camera, [8] : ���̽�����(��) �ʱ���ġ �̵�, [9] : ���̽�����(��) �ʱ���ġ �̵�
	for (idx = 0; idx < 10; idx++)
	{
		if (pView->m_bDispMsgDoAuto[idx])
		{
			bDispMsg = TRUE;
			nStepDispMsg = pView->m_nStepDispMsg[idx];
			break;
		}
	}
	if (bDispMsg && IsRun())
	{
		switch (nStepDispMsg)
		{
		case 0:
			break;
		case FROM_DOMARK0:
			pView->m_bDispMsgDoAuto[8] = FALSE;
			pView->m_nStepDispMsg[8] = 0;
			Stop();
			//TowerLamp(RGB_YELLOW, TRUE);
			Buzzer(TRUE, 0);
			//pView->DispStsBar(_T("����-37"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			break;
		case FROM_DOMARK1:
			pView->m_bDispMsgDoAuto[9] = FALSE;
			pView->m_nStepDispMsg[9] = 0;
			Stop();
			//TowerLamp(RGB_YELLOW, TRUE);
			Buzzer(TRUE, 0);
			//pView->DispStsBar(_T("����-38"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			break;
		case FROM_DISPDEFIMG:
			pView->m_bDispMsgDoAuto[0] = FALSE;
			pView->m_nStepDispMsg[0] = 0;
			Stop();
			MsgBox(_T("����(��) Serial�� �����ʽ��ϴ�."));
			//TowerLamp(RGB_YELLOW, TRUE);
			break;
		case FROM_DISPDEFIMG + 1:
			pView->m_bDispMsgDoAuto[1] = FALSE;
			pView->m_nStepDispMsg[1] = 0;
			Stop();
			MsgBox(_T("����(��) Serial�� �����ʽ��ϴ�."));
			//TowerLamp(RGB_YELLOW, TRUE);
			break;
		case FROM_DISPDEFIMG + 2: // IsFixUp
			pView->m_bDispMsgDoAuto[2] = FALSE;
			pView->m_nStepDispMsg[2] = 0;
			Stop();
			//TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
			pView->m_mgrPunch->m_bSwStopNow = TRUE;
			pView->m_mgrPunch->m_bSwRunF = FALSE;
			//pView->DispStsBar(_T("����-39"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(m_sFixMsg[0]);
			m_sFixMsg[0] = _T("");
			break;
		case FROM_DISPDEFIMG + 3: // IsFixDn
			pView->m_bDispMsgDoAuto[3] = FALSE;
			pView->m_nStepDispMsg[3] = 0;
			Stop();
			//TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
			pView->m_mgrPunch->m_bSwStopNow = TRUE;
			pView->m_mgrPunch->m_bSwRunF = FALSE;
			//pView->DispStsBar(_T("����-40"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(m_sFixMsg[1]);
			m_sFixMsg[1] = _T("");
			break;
		case FROM_DISPDEFIMG + 4:
			//m_bDispMsgDoAuto[4] = FALSE;
			//m_nStepDispMsg[4] = 0;
			//Stop();
			//TowerLamp(RGB_RED, TRUE);
			//Buzzer(TRUE, 0);
			//m_bSwStopNow = TRUE;
			//m_bSwRunF = FALSE;
			//pView->DispStsBar(_T("����-41"), 0);
			//DispMain(_T("�� ��"), RGB_RED);	
			//MsgBox(m_sFixMsg);
			//m_sFixMsg = _T("");
			break;
		case FROM_DISPDEFIMG + 5:
			pView->m_bDispMsgDoAuto[5] = FALSE;
			pView->m_nStepDispMsg[5] = 0;
			Stop();
			MsgBox(_T("����(��) Serial�� �����ʽ��ϴ�."));
			//TowerLamp(RGB_YELLOW, TRUE);
			break;
		case FROM_DISPDEFIMG + 6:
			pView->m_bDispMsgDoAuto[6] = FALSE;
			pView->m_nStepDispMsg[6] = 0;
			Stop();
			//TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
			pView->m_mgrPunch->m_bSwStopNow = TRUE;
			pView->m_mgrPunch->m_bSwRunF = FALSE;
			//pView->DispStsBar(_T("����-42"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			break;
		case FROM_DISPDEFIMG + 7:
			pView->m_bDispMsgDoAuto[7] = FALSE;
			pView->m_nStepDispMsg[7] = 0;
			Stop();
			//TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
			pView->m_mgrPunch->m_bSwStopNow = TRUE;
			pView->m_mgrPunch->m_bSwRunF = FALSE;
			//pView->DispStsBar(_T("����-43"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			break;
		}
	}
}

void CManagerProcedure::DoAutoChkShareFolder()
{
	if (!pView->m_mgrPunch || !pView->m_mgrReelmap)
		return;

	CString sLot, sLayerUp, sLayerDn;
	BOOL bDualTestInner;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nSerial = 0;
	CString sNewLot;
	int nNewLot = 0;
	BOOL bPcrInShare[2];
	BOOL bNewModel = FALSE;

	switch (m_nStepAuto)
	{
	case 0:
		pView->m_mgrPunch->m_bSwRun = FALSE;
		pView->m_bSwStop = TRUE;
		m_nStepAuto++;
		break;
	case 1:
		if (IsReady())		// �����غ�
		{
			//TowerLamp(RGB_YELLOW, TRUE, TRUE);
			m_nStepAuto++;
		}
		break;
	case 2:
		if (IsRun())
			m_nStepAuto++;
		break;
	case 3:
		ClrDispMsg();
		//TowerLamp(RGB_YELLOW, TRUE, FALSE);
		m_nStepAuto++;
		break;
	case 4:
		if (IsRun())		// �ʱ����
		{
			if (!IsAoiLdRun())
			{
				Stop();
				//TowerLamp(RGB_YELLOW, TRUE);
			}
			else
			{
				ResetWinker(); // 20151126 : ��������ġ ���� ���� ������ ����.

				DispInitRun();

				m_nVsBufLastSerial[0] = GetVsUpBufLastSerial();
				if (bDualTest)
					m_nVsBufLastSerial[1] = GetVsDnBufLastSerial();

				SetListBuf();

				if (MODE_INNER == pDoc->GetTestMode() || MODE_OUTER == pDoc->GetTestMode()) // Please modify for outer mode.-20221226
				{
					pDoc->GetCurrentInfoEng();
					pView->UpdateData(DlgMenu01);
				}

				m_nStepAuto = AT_LP;
			}
		}
		else
			pView->m_mgrPunch->Winker(MN_RUN); // Run Button - 20151126 : ��������ġ ���� ���� ������ ����.
		break;

	case AT_LP:
		if (IsShare())
		{
			bPcrInShare[0] = FALSE;
			bPcrInShare[1] = FALSE;

			//if (IsShareUp() && IsTestDoneUp() && !m_bAoiTestF[0]) // �˻��(��) �˻���
			if (IsShareUp())
			{
				nSerial = GetShareUp();
				if (nSerial > 0)
				{
					if (m_bSerialDecrese)
					{
						if (m_nLotEndSerial > 0 && nSerial < m_nLotEndSerial)
						{
							// Delete PCR File
							pView->DelSharePcrUp();
						}
						else
						{
							m_nShareUpS = nSerial;
							bPcrInShare[0] = TRUE;
						}
					}
					else
					{
						if (m_nLotEndSerial > 0 && nSerial > m_nLotEndSerial)
						{
							// Delete PCR File
							pView->DelSharePcrUp();
						}
						else
						{
							m_nShareUpS = nSerial;
							bPcrInShare[0] = TRUE;
						}
					}
				}
				else
				{
					m_bLoadShare[0] = FALSE;
				}
			}
			else
				m_bLoadShare[0] = FALSE;


			if (bDualTest)
			{
				if (IsShareDn())
				{
					nSerial = GetShareDn();
					if (nSerial > 0)
					{
						if (m_bSerialDecrese)
						{
							if (m_nLotEndSerial > 0 && nSerial < m_nLotEndSerial)
							{
								// Delete PCR File
								pView->DelSharePcrDn();
							}
							else
							{
								m_nShareDnS = nSerial;
								bPcrInShare[1] = TRUE;
							}
						}
						else
						{
							if (m_nLotEndSerial > 0 && nSerial > m_nLotEndSerial)
							{
								// Delete PCR File
								pView->DelSharePcrDn();
							}
							else
							{
								m_nShareDnS = nSerial;
								bPcrInShare[1] = TRUE;
							}
						}
					}
					else
					{
						m_bLoadShare[1] = FALSE;
					}
				}
				else
					m_bLoadShare[1] = FALSE;

				if (bPcrInShare[0] || bPcrInShare[1])
					m_nStepAuto++;
			}
			else
			{
				if (bPcrInShare[0])
					m_nStepAuto++;
			}
		}
		else
		{
			if (!m_bLastProc)
			{
				if (ChkLastProc())
				{
					m_nLastProcAuto = LAST_PROC;
					m_bLastProc = TRUE;
					nSerial = GetShareUp();

					if (pView->m_mgrPunch->IsVs())
					{
						if (m_nAoiLastSerial[0] < 1)
							m_nAoiLastSerial[0] = nSerial;

						m_nPrevStepAuto = m_nStepAuto;
						m_nStepAuto = LAST_PROC_VS_ALL;		 // �ܷ�ó�� 3
						break;
					}
					else
					{
						if (bDualTest)
						{
							if (ChkLastProcFromEng())
								nSerial = pDoc->GetCurrentInfoEngShotNum();
							else if (ChkLastProcFromUp())
								nSerial = pDoc->m_ListBuf[0].GetLast();
							else
								nSerial = pDoc->m_ListBuf[1].GetLast();
						}
						else
						{
							if (ChkLastProcFromEng())
								nSerial = pDoc->GetCurrentInfoEngShotNum();
							else
								nSerial = pDoc->m_ListBuf[0].GetLast();
						}

						if (!IsSetLotEnd()) // 20160810
						{
							SetLotEnd(nSerial);//+pDoc->AoiDummyShot[1]); // 3
							m_nAoiLastSerial[0] = nSerial;
						}

						m_nStepAuto++;
					}
				}
			}
			else
			{
				if (ChkLastProcFromEng())
				{
					nSerial = pDoc->GetCurrentInfoEngShotNum();

					if (!IsSetLotEnd())
					{
						SetLotEnd(nSerial);
						m_nAoiLastSerial[0] = nSerial;
					}
				}

				m_bWaitPcr[0] = FALSE;
				m_bWaitPcr[1] = FALSE;
				m_nStepAuto++;
			}

			if (MODE_INNER == pDoc->GetTestMode())
			{
				nSerial = pDoc->GetCurrentInfoEngShotNum();

				if (ChkLastProc())
				{
					if (ChkLastProcFromEng())
					{
						if (!IsSetLotEnd())
							SetLotEnd(nSerial);
					}
				}
			}

		}
		break;

	case AT_LP + 1:
		if (!m_bCont) // �̾�� �ƴ� ���.
		{
			if (!ChkStShotNum())
			{
				Stop();
				//TowerLamp(RGB_YELLOW, TRUE);
			}
		}
		else
		{
			//if (!ChkContShotNum())
			//{
			//	Stop();
			//	//TowerLamp(RGB_YELLOW, TRUE);
			//}
		}
		m_nStepAuto++;
		break;
	case AT_LP + 2:
		if (IsRun())
		{
			m_bBufEmpty[0] = m_bBufEmpty[0]; // Up
			m_nStepAuto++;
		}
		break;

	case AT_LP + 3:
		Shift2Buf();			// PCR �̵�(Share->Buffer)
		m_nStepAuto++;
		break;

	case AT_LP + 4:
		if (!IsRun())
			break;

		if (m_bTHREAD_UPDATE_YIELD_UP || m_bTHREAD_UPDATE_YIELD_DN || m_bTHREAD_UPDATE_YIELD_ALLUP || m_bTHREAD_UPDATE_YIELD_ALLDN) // Write Reelmap
		{
			Sleep(100);
			break;
		}

		if (m_bTHREAD_UPDATE_REELMAP_UP || m_bTHREAD_UPDATE_REELMAP_DN || m_bTHREAD_UPDATE_REELMAP_ALLUP || m_bTHREAD_UPDATE_REELMAP_ALLDN) // Write Reelmap
		{
			Sleep(100);
			break;
		}

		if (!bDualTest)
		{
			if (pDoc->GetTestMode() == MODE_OUTER)
			{
				if (m_bTHREAD_UPDATE_REELMAP_INNER_UP) // Write Reelmap
					break;

				if (pDoc->WorkingInfo.LastJob.bDualTestInner)
				{
					if (m_bTHREAD_UPDATE_REELMAP_INNER_DN || m_bTHREAD_UPDATE_REELMAP_INNER_ALLUP
						|| m_bTHREAD_UPDATE_REELMAP_INNER_ALLDN) // Write Reelmap
						break;
				}
			}
		}

		m_nStepAuto++;

		if (m_nShareUpS > 0)
		{
			//if (m_nShareUpS % 2)
			//	m_nShareUpSerial[0] = m_nShareUpS; // Ȧ��
			//else
			//	m_nShareUpSerial[1] = m_nShareUpS; // ¦��
			m_nShareUpCnt++;

			if (pDoc->GetCurrentInfoEng())
				pView->GetItsSerialInfo(m_nShareUpS, bDualTestInner, sLot, sLayerUp, sLayerDn, 0);

			bNewModel = GetAoiUpInfo(m_nShareUpS, &nNewLot); // Buffer���� PCR������ ��� ������ ����.

			if (bNewModel)	// AOI ����(AoiCurrentInfoPath) -> AOI Feeding Offset
			{
				m_bNewModel = TRUE;
				pView->ChangeModel();
			}
			else
			{
				if (m_nShareUpS == 1)
				{
					m_nAoiCamInfoStrPcs[0] = pView->GetAoiUpCamMstInfo();
					if ((m_nAoiCamInfoStrPcs[0] == 1 ? TRUE : FALSE) != pDoc->WorkingInfo.System.bStripPcsRgnBin)
					{
						if (m_nAoiCamInfoStrPcs[0])
							pView->MsgBox(_T("���� ��ŷ�δ� �Ϲ� ��� �ε�, \r\n��� AOI�� DTS ��忡�� �˻縦 �����Ͽ����ϴ�."));
						else
							pView->MsgBox(_T("���� ��ŷ�δ� DTS ��� �ε�, \r\n��� AOI�� �Ϲ� ��忡�� �˻縦 �����Ͽ����ϴ�."));

						Stop();
						//TowerLamp(RGB_RED, TRUE);
						break;
					}
				}
			}

			if (nNewLot)
			{
				if (!pDoc->m_bNewLotShare[0])
				{
					pDoc->m_bNewLotShare[0] = TRUE;// Lot Change.
					if (!bDualTest)
						pView->OpenReelmapFromBuf(m_nShareUpS);
				}
			}

			LoadPcrUp(m_nShareUpS);				// Default: From Buffer, TRUE: From Share
			if (pDoc->m_bUpdateForNewJob[0])
			{
				pDoc->m_bUpdateForNewJob[0] = FALSE;
				//if (m_pDlgMenu01)
				//	m_pDlgMenu01->UpdateData();
				pView->UpdateData(DlgMenu01);
			}

			if (!bDualTest)
			{
				if (m_nShareUpS != m_nShareUpSprev)
				{
					m_nShareUpSprev = m_nShareUpS;
					pView->UpdateReelmap(m_nShareUpS); // �ø��������� ������ ������ ���� 
				}
			}

			if (!m_bLastProc)
			{
				if (!IsSetLotEnd())
				{
					if (ChkLotEndUp(m_nShareUpS))// ������ ���� �߿� Lot End (-2) �ܷ�ó���� üũ��. (���� 3Pnl:-2) -> ��Ʈ�Ϸ� 
					{
						SetLotEnd(m_nShareUpS - pDoc->AoiDummyShot[0]);
						if (m_nAoiLastSerial[0] < 1)
							m_nAoiLastSerial[0] = m_nShareUpS;

						if (!bDualTest)
						{
							m_bLastProc = TRUE;
							m_nLastProcAuto = LAST_PROC;
						}
					}
				}
				if (ChkLastProc())
				{
					m_nLastProcAuto = LAST_PROC;
					m_bLastProc = TRUE;

					if (pView->m_mgrPunch->IsVs())
					{
						if (m_nAoiLastSerial[0] < 1)
							m_nAoiLastSerial[0] = m_nShareUpS;

						m_nPrevStepAuto = m_nStepAuto;
						m_nStepAuto = LAST_PROC_VS_ALL;		 // �ܷ�ó�� 3
						break;
					}
					else
					{
						if (bDualTest)
						{
							if (ChkLastProcFromEng())
								nSerial = pDoc->GetCurrentInfoEngShotNum();
							else if (ChkLastProcFromUp())
								nSerial = pDoc->m_ListBuf[0].GetLast();
							else
								nSerial = pDoc->m_ListBuf[1].GetLast();
						}
						else
						{
							if (ChkLastProcFromEng())
								nSerial = pDoc->GetCurrentInfoEngShotNum();
							else
								nSerial = pDoc->m_ListBuf[0].GetLast();
						}

						if (!IsSetLotEnd()) // 20160810
						{
							SetLotEnd(nSerial);//+pDoc->AoiDummyShot[1]); // 3
							m_nAoiLastSerial[0] = nSerial;
						}
					}
				}
			}
			else
			{
				if (ChkLastProcFromEng())
				{
					nSerial = pDoc->GetCurrentInfoEngShotNum();

					if (!IsSetLotEnd())
					{
						SetLotEnd(nSerial);
						m_nAoiLastSerial[0] = nSerial;
					}
				}
			}
		}
		break;

	case AT_LP + 5:
		if (!IsRun())
			break;

		if (!bDualTest)
		{
			m_nStepAuto++;
			break;
		}

		if (m_bTHREAD_UPDATE_REELMAP_DN || m_bTHREAD_UPDATE_REELMAP_ALLUP || m_bTHREAD_UPDATE_REELMAP_ALLDN) // Write Reelmap
		{
			Sleep(100);
			break;
		}

		if (m_bTHREAD_REELMAP_YIELD_DN || m_bTHREAD_REELMAP_YIELD_ALLUP || m_bTHREAD_REELMAP_YIELD_ALLDN) // Write Reelmap
		{
			Sleep(100);
			break;
		}

		m_nStepAuto++;

		if (m_bChkLastProcVs)
		{
			if (m_nShareDnS > m_nAoiLastSerial[0]
				&& m_nAoiLastSerial[0] > 0)
				break;
		}
		else
		{
			if (IsSetLotEnd())
			{
				if (m_nShareDnS > m_nAoiLastSerial[0]
					&& m_nAoiLastSerial[0] > 0)
					break;
			}
		}


		if (m_nShareDnS > 0)
		{
			if (m_nShareDnS % 2)
				m_nShareDnSerial[0] = m_nShareDnS; // Ȧ��
			else
				m_nShareDnSerial[1] = m_nShareDnS; // ¦��
			m_nShareDnCnt++;


			bNewModel = GetAoiDnInfo(m_nShareDnS, &nNewLot);

			if (bNewModel)	// AOI ����(AoiCurrentInfoPath) -> AOI Feeding Offset
			{
				//MsgBox(_T("�ű� �𵨿� ���� AOI(��)���� ��Ʈ �и��� �Ǿ����ϴ�.\r\n���� ��Ʈ�� �ܷ�ó�� �մϴ�.");
				InitInfo();
				pView->m_mgrReelmap->ResetMkInfo(1); // 0 : AOI-Up , 1 : AOI-Dn , 2 : AOI-UpDn	
				ModelChange(1); // 0 : AOI-Up , 1 : AOI-Dn
			}
			else
			{
				if (m_nShareDnS == 1)
				{
					m_nAoiCamInfoStrPcs[1] = pView->GetAoiDnCamMstInfo();
					if ((m_nAoiCamInfoStrPcs[1] == 1 ? TRUE : FALSE) != pDoc->WorkingInfo.System.bStripPcsRgnBin)
					{
						if (m_nAoiCamInfoStrPcs[1])
							pView->MsgBox(_T("���� ��ŷ�δ� �Ϲ� ��� �ε�, \r\n�ϸ� AOI�� DTS ��忡�� �˻縦 �����Ͽ����ϴ�."));
						else
							pView->MsgBox(_T("���� ��ŷ�δ� DTS ��� �ε�, \r\n�ϸ� AOI�� �Ϲ� ��忡�� �˻縦 �����Ͽ����ϴ�."));

						Stop();
						//TowerLamp(RGB_RED, TRUE);
						break;
					}
				}
			}

			if (nNewLot)
			{
				if (!pDoc->m_bNewLotShare[1])
				{
					pDoc->m_bNewLotShare[1] = TRUE;// Lot Change.				
					if (bDualTest)
						pView->OpenReelmapFromBuf(m_nShareDnS);
				}
			}

			LoadPcrDn(m_nShareDnS);
			if (pDoc->m_bUpdateForNewJob[1])
			{
				pDoc->m_bUpdateForNewJob[1] = FALSE;
				pView->UpdateData(DlgMenu01);
				//if (m_pDlgMenu01)
				//	m_pDlgMenu01->UpdateData();
			}


			if (bDualTest)
			{
				if (m_nShareDnS != m_nShareDnSprev)
				{
					m_nShareDnSprev = m_nShareDnS;
					//pView->UpdateReelmap(m_nShareDnS);  // �ø��������� ������ ������ ����  // After inspect bottom side.
					pView->UpdateReelmap(m_nShareDnS);
				}
			}


			if (!m_bLastProc)
			{
				if (!IsSetLotEnd())
				{
					if (ChkLotEndDn(m_nShareDnS))// ������ ���� �߿� Lot End (-2) �ܷ�ó���� üũ��. (���� 3Pnl:-2) -> ��Ʈ�Ϸ� 
					{
						if (!IsSetLotEnd())
							SetLotEnd(m_nShareDnS - pDoc->AoiDummyShot[1]);
						if (m_nAoiLastSerial[0] < 1)
							m_nAoiLastSerial[0] = m_nShareDnS;
						if (bDualTest)
						{
							m_bLastProc = TRUE;
							m_nLastProcAuto = LAST_PROC;
						}
					}
				}
				if (ChkLastProc())
				{
					m_nLastProcAuto = LAST_PROC;
					m_bLastProc = TRUE;

					if (pView->m_mgrPunch->IsVs())
					{
						if (m_nAoiLastSerial[0] < 1)
							m_nAoiLastSerial[0] = m_nShareDnS;

						m_nPrevStepAuto = m_nStepAuto;
						m_nStepAuto = LAST_PROC_VS_ALL;		 // �ܷ�ó�� 3
						break;
					}
					else
					{
						if (ChkLastProcFromEng())
							nSerial = pDoc->GetCurrentInfoEngShotNum();
						else if (ChkLastProcFromUp())
							nSerial = pDoc->m_ListBuf[0].GetLast();
						else
							nSerial = pDoc->m_ListBuf[1].GetLast();

						if (!IsSetLotEnd()) // 20160810
						{
							SetLotEnd(nSerial);//+pDoc->AoiDummyShot[1]); // 3
							m_nAoiLastSerial[0] = nSerial;
						}
					}
				}
			}
			else
			{
				if (ChkLastProcFromEng())
				{
					nSerial = pDoc->GetCurrentInfoEngShotNum();

					if (!IsSetLotEnd())
					{
						SetLotEnd(nSerial);
						m_nAoiLastSerial[0] = nSerial;
					}
				}
			}
		}
		break;

	case AT_LP + 6:
		m_nStepAuto++;
		if (m_nShareUpS > 0)
		{
			pView->SetAoiLastShot(0, m_nShareUpS);
			//if (m_pDlgFrameHigh)
			//	m_pDlgFrameHigh->SetAoiLastShot(0, m_nShareUpS);
		}
		if (bDualTest)
		{
			if (m_nShareDnS > 0)
			{
				if (m_bChkLastProcVs)
				{
					if (m_nShareDnS > m_nAoiLastSerial[0]
						&& m_nAoiLastSerial[0] > 0)
						break;
				}
				else
				{
					if (IsSetLotEnd())
					{
						if (m_nShareDnS > m_nAoiLastSerial[0]
							&& m_nAoiLastSerial[0] > 0)
							break;
					}
				}

				pView->SetAoiLastShot(1, m_nShareDnS);
				//if (m_pDlgFrameHigh)
				//	m_pDlgFrameHigh->SetAoiLastShot(1, m_nShareDnS);
			}
		}
		break;

	case AT_LP + 7:
		m_nShareUpS = 0;
		m_nShareDnS = 0;
		m_nStepAuto++;
		break;

	case AT_LP + 8:
		if (IsRun())
		{
			if (pDoc->GetTestMode() == MODE_OUTER)
			{
				if (m_bTHREAD_UPDATE_REELMAP_INNER_UP) // Write Reelmap
					break;

				if (pDoc->WorkingInfo.LastJob.bDualTestInner)
				{
					if (m_bTHREAD_UPDATE_REELMAP_INNER_DN || m_bTHREAD_UPDATE_REELMAP_INNER_ALLUP
						|| m_bTHREAD_UPDATE_REELMAP_INNER_ALLDN) // Write Reelmap
						break;
				}
			}

			if (bDualTest)
			{
				if (m_bTHREAD_UPDATE_REELMAP_UP || m_bTHREAD_UPDATE_REELMAP_DN
					|| m_bTHREAD_UPDATE_REELMAP_ALLUP || m_bTHREAD_UPDATE_REELMAP_ALLDN) // Write Reelmap
					break;
			}
			else
			{
				if (m_bTHREAD_UPDATE_REELMAP_UP) // Write Reelmap
					break;
			}
			//SetListBuf(); // 20170727-�ܷ�ó�� �� ��������� �ݺ��ؼ� ���Լ��� ȣ������� �¿� ��ŷ �ε��� ���� ���� �߻�.(case AT_LP + 8:)
			m_bLoadShare[0] = FALSE;
			m_bLoadShare[1] = FALSE;
			m_nStepAuto = AT_LP;
		}
		break;


	case LAST_PROC_VS_ALL:			 // �ܷ�ó�� 3
		m_nDummy[0] = 0;
		m_nDummy[1] = 0;
		m_bChkLastProcVs = TRUE;
		//TowerLamp(RGB_GREEN, TRUE);
		DispMain(_T("���VS�ܷ�"), RGB_GREEN);
		if (m_nAoiLastSerial[0] < 1)
			m_nAoiLastSerial[0] = GetAoiUpSerial();
		if (!IsSetLotEnd())
			SetLotEnd(m_nAoiLastSerial[0]);
		//m_nAoiLastSerial[1] = GetAoiDnSerial();
		m_nStepAuto++;
		break;

	case LAST_PROC_VS_ALL + 1:
		if (pView->m_mgrPunch->IsVsUp())
			m_nStepAuto++;
		else
			m_nStepAuto = m_nPrevStepAuto;
		break;

	case LAST_PROC_VS_ALL + 2:
		SetDummyUp();
		m_nStepAuto++;
		break;

	case LAST_PROC_VS_ALL + 3:
		SetDummyUp();
		m_nStepAuto++;
		break;

	case LAST_PROC_VS_ALL + 4:
		SetDummyUp();
		m_nStepAuto++;
		break;

	case LAST_PROC_VS_ALL + 5:
		m_nStepAuto = m_nPrevStepAuto;
		break;
	}
	
}

void CManagerProcedure::DoAutoMarkingEngrave()
{
	// ���κ� ��ŷ�� ON (PC�� ON, OFF)
	if (pDoc->BtnStatus.EngAuto.IsOnMking && !(pDoc->m_pMpeSignal[6] & (0x01 << 3))) // ���κ� ��ŷ�� ON (PC�� ON, OFF)
	{
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqOnMkIng, TRUE);
#ifdef USE_MPE
		pView->MpeWrite(_T("MB440173"), 1); // 2D(GUI) ���� ����Running��ȣ(PC On->PC Off)
		pDoc->Log(_T("PC: 2D(GUI) ���� ����Running��ȣ ON (PC On->PC Off)"));
#endif
	}
	else if (!pDoc->BtnStatus.EngAuto.IsOnMking && (pDoc->m_pMpeSignal[6] & (0x01 << 3)))
	{
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqOnMkIng, FALSE);
#ifdef USE_MPE
		pView->MpeWrite(_T("MB440173"), 0); // 2D(GUI) ���� ����Running��ȣ(PC On->PC Off)
		pDoc->Log(_T("PC: 2D(GUI) ���� ����Running��ȣ OFF (PC On->PC Off)"));
#endif
	}

	// ���κ� ��ŷ�Ϸ� ON (PC�� ON, OFF)
	if (pDoc->BtnStatus.EngAuto.IsMkDone && !(pDoc->m_pMpeSignal[6] & (0x01 << 4))) // ���κ� �۾��Ϸ�.(PC�� On, PLC�� Ȯ�� �� Off)
	{
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqMkDone, TRUE);
#ifdef USE_MPE
		pView->MpeWrite(_T("MB440174"), 1); // ���κ� �۾��Ϸ�.(PC�� On, PLC�� Ȯ�� �� Off)
		pDoc->Log(_T("PC: ���κ� �۾��Ϸ� ON (PC�� On, PLC�� Ȯ�� �� Off)"));
#endif
	}
	else if (!pDoc->BtnStatus.EngAuto.IsMkDone && (pDoc->m_pMpeSignal[6] & (0x01 << 4))) // ���κ� �۾��Ϸ�.(PC�� On, PLC�� Ȯ�� �� Off)
	{
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqMkDone, FALSE);
		//#ifdef USE_MPE
		//		pView->MpeWrite(_T("MB440174"), 0); // ���κ� �۾��Ϸ�.(PC�� On, PLC�� Ȯ�� �� Off)
		//#endif
	}

	// ���κ� 2D ���� �۾��� ��ȣ
	if (pDoc->BtnStatus.EngAuto.IsOnRead2d && !(pDoc->m_pMpeSignal[6] & (0x01 << 8))) // ���κ� 2D ���� �۾��� ��ȣ(PC On->PC Off)
	{
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqOnReading2d, TRUE);
#ifdef USE_MPE
		pView->MpeWrite(_T("MB440178"), 1); // ���κ� 2D ���� �۾��� ��ȣ(PC On->PC Off)
		pDoc->Log(_T("PC: ���κ� 2D ���� �۾��� ��ȣ ON (PC On->PC Off)"));
#endif
	}
	else if (!pDoc->BtnStatus.EngAuto.IsOnRead2d && (pDoc->m_pMpeSignal[6] & (0x01 << 8)))
	{
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqOnReading2d, FALSE);
#ifdef USE_MPE
		pView->MpeWrite(_T("MB440178"), 0); // ���κ� 2D ���� �۾��� ��ȣ(PC On->PC Off)
		pDoc->Log(_T("PC: ���κ� 2D ���� �۾��� ��ȣ OFF (PC On->PC Off)"));
#endif
	}

	// ���κ� 2D ���� �۾��Ϸ� ��ȣ
	if (pDoc->BtnStatus.EngAuto.IsRead2dDone && !(pDoc->m_pMpeSignal[6] & (0x01 << 9))) // ���κ� 2D ���� �۾��Ϸ� ��ȣ.(PC�� On, PLC�� Ȯ�� �� Off)
	{
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeq2dReadDone, TRUE);
#ifdef USE_MPE
		pView->MpeWrite(_T("MB440179"), 1); // ���κ� 2D ���� �۾��Ϸ� ��ȣ.(PC�� On, PLC�� Ȯ�� �� Off)
		pDoc->Log(_T("PC: ���κ� 2D ���� �۾��Ϸ� ��ȣ ON (PC�� On, PLC�� Ȯ�� �� Off)"));
#endif
	}
	else if (!pDoc->BtnStatus.EngAuto.IsRead2dDone && (pDoc->m_pMpeSignal[6] & (0x01 << 9)))
	{
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeq2dReadDone, FALSE);
		//#ifdef USE_MPE
		//		pView->MpeWrite(_T("MB440179"), 0); // ���κ� 2D ���� �۾��Ϸ� ��ȣ.(PC�� On, PLC�� Ȯ�� �� Off)
		//#endif
	}

}


void CManagerProcedure::DoAutoMarking()
{
	if (pDoc->WorkingInfo.LastJob.nAlignMethode == TWO_POINT)
		MarkingWith2PointAlign();
	else if (pDoc->WorkingInfo.LastJob.nAlignMethode == FOUR_POINT)
		MarkingWith4PointAlign();
	else
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("��ŷ�� ���� Align����� �������� �ʾҽ��ϴ�."));
	}
}

void CManagerProcedure::Mk2PtReady()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	CString sMsg;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST:	// PLC MK ��ȣ Ȯ��	
			if (IsRun())
			{
				pDoc->Log(_T("PC: ��ŷ�� ��ŷ�� ON (PC�� ON, OFF)"));
				pView->MpeWrite(_T("MB440150"), 1);// ��ŷ�� ��ŷ�� ON (PC�� ON, OFF)
				if (!pView->CheckConectionSr1000w())
					break;
				m_nMkStAuto++;
			}
			break;
		case MK_ST + 1:
			if (!m_bTHREAD_SHIFT2MK)
			{
				SetListBuf();
				m_nMkStAuto = MK_ST + (Mk2PtIdx::Start);
			}
			break;
		case MK_ST + (Mk2PtIdx::Start) :	// 2
			if (bDualTest)
			{
				if (pDoc->m_ListBuf[1].nTot > 0) // AOI-Dn
				{
					m_nMkStAuto++;

					m_nBufDnSerial[0] = pDoc->m_ListBuf[1].Pop();
					m_nBufUpSerial[0] = m_nBufDnSerial[0];
					if (pDoc->m_ListBuf[1].nTot > 0) // AOI-Dn
					{
						m_nBufDnSerial[1] = pDoc->m_ListBuf[1].Pop();
						m_nBufUpSerial[1] = m_nBufDnSerial[1];
					}
					else
					{
						m_nBufDnSerial[1] = 0;
						m_nBufUpSerial[1] = 0;
					}
				}
				else
				{
					m_bLotEnd = TRUE;
					m_nLotEndAuto = LOT_END;
				}

				if (pDoc->WorkingInfo.LastJob.bSampleTest)
				{
					if (m_nBufUpSerial[0] == 1)
					{
						m_nLotEndSerial = _tstoi(pDoc->WorkingInfo.LastJob.sSampleTestShotNum);
						m_bLastProcFromUp = FALSE;
						m_bLastProcFromEng = FALSE;
						m_bLastProc = TRUE;
						pView->SetLastProc();
						pView->MpeWrite(_T("MB440186"), 1);			// �ܷ�ó�� AOI(��) ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141112
						pView->MpeWrite(_T("MB440181"), 1);			// �ܷ�ó��(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141031
						pDoc->Log(_T("PC: �ܷ�ó�� AOI(��) ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)"));
					}
				}
			}
			else
			{
				if (pDoc->m_ListBuf[0].nTot > 0) // AOI-Up
				{
					m_nMkStAuto++;
					m_nBufUpSerial[0] = pDoc->m_ListBuf[0].Pop();
					if (pDoc->m_ListBuf[0].nTot > 0) // AOI-Up
						m_nBufUpSerial[1] = pDoc->m_ListBuf[0].Pop();
					else
						m_nBufUpSerial[1] = 0;
				}
				else
				{
					m_bLotEnd = TRUE;
					m_nLotEndAuto = LOT_END;
				}

				if (pDoc->WorkingInfo.LastJob.bSampleTest)
				{
					if (m_nBufUpSerial[0] == 1)
					{
						m_nLotEndSerial = _tstoi(pDoc->WorkingInfo.LastJob.sSampleTestShotNum);
						m_bLastProcFromUp = FALSE;
						m_bLastProcFromEng = FALSE;
						m_bLastProc = TRUE;
						//if (m_pDlgMenu01)
						//	m_pDlgMenu01->m_bLastProc = TRUE;
						pView->SetLastProc();
						pView->MpeWrite(_T("MB440186"), 1);			// �ܷ�ó�� AOI(��) ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141112
						pView->MpeWrite(_T("MB440181"), 1);			// �ܷ�ó��(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141031
						pDoc->Log(_T("PC: �ܷ�ó�� AOI(��) ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)"));
					}
				}
			}
											break;
		case MK_ST + (Mk2PtIdx::Start) + 1:
			if (pDoc->GetTestMode() == MODE_OUTER)
			{
				if (!m_bTHREAD_UPDATE_REELMAP_UP && !m_bTHREAD_UPDATE_REELMAP_DN
					&& !m_bTHREAD_UPDATE_REELMAP_ALLUP && !m_bTHREAD_UPDATE_REELMAP_ALLDN
					&& !m_bTHREAD_UPDATE_REELMAP_INNER_UP && !m_bTHREAD_UPDATE_REELMAP_INNER_DN
					&& !m_bTHREAD_UPDATE_REELMAP_INNER_ALLUP && !m_bTHREAD_UPDATE_REELMAP_INNER_ALLDN) // Write Reelmap
					m_nMkStAuto = MK_ST + (Mk2PtIdx::ChkSn);
			}
			else
			{
				if (!m_bTHREAD_UPDATE_REELMAP_UP && !m_bTHREAD_UPDATE_REELMAP_DN
					&& !m_bTHREAD_UPDATE_REELMAP_ALLUP && !m_bTHREAD_UPDATE_REELMAP_ALLDN) // Write Reelmap
					m_nMkStAuto = MK_ST + (Mk2PtIdx::ChkSn);
			}
			break;
		}
	}
}

void CManagerProcedure::Mk2PtChkSerial()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	CString sNewLot;
	BOOL bNewModel = FALSE;
	int nSerial = 0;
	int nNewLot = 0;
	int nLastShot = 0;
	double dFdEnc;
	CString sLot, sMsg;

	if (m_bTHREAD_SHIFT2MK)
		return;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk2PtIdx::ChkSn) :
			if (m_nBufUpSerial[0] == m_nBufUpSerial[1])
			{
				Stop();
				pView->ClrDispMsg();
				AfxMessageBox(_T("��/�� ��ŷ �ø����� �����ϴ�."));
				SetListBuf();
				m_nMkStAuto = MK_ST + (Mk2PtIdx::Start);
				break;
			}

			if (pDoc->GetTestMode() == MODE_OUTER)
			{
				if (!m_bTHREAD_UPDATE_REELMAP_UP && !m_bTHREAD_UPDATE_REELMAP_DN
					&& !m_bTHREAD_UPDATE_REELMAP_ALLUP && !m_bTHREAD_UPDATE_REELMAP_ALLDN
					&& !m_bTHREAD_UPDATE_REELMAP_INNER_UP && !m_bTHREAD_UPDATE_REELMAP_INNER_DN
					&& !m_bTHREAD_UPDATE_REELMAP_INNER_ALLUP && !m_bTHREAD_UPDATE_REELMAP_INNER_ALLDN) // Write Reelmap
				{
					if (!m_bTHREAD_DISP_DEF && !m_bTHREAD_DISP_DEF_INNER)
					{
						m_nMkStAuto++;
						m_nStepTHREAD_DISP_DEF = 0;
						m_nStepTHREAD_DISP_DEF_INNER = 0;

						m_bTHREAD_DISP_DEF = TRUE;				// DispDefImg() : CopyDefImg Start -> Disp Reelmap Start
						m_bTHREAD_DISP_DEF_INNER = TRUE;		// DispDefImgInner() : Disp Reelmap Start

						pDoc->SetMkMenu01(_T("Signal"), _T("DispDefImg"), _T("1"));
					}
				}
			}
			else
			{
				if (!m_bTHREAD_UPDATE_REELMAP_UP && !m_bTHREAD_UPDATE_REELMAP_DN &&
					!m_bTHREAD_UPDATE_REELMAP_ALLUP && !m_bTHREAD_UPDATE_REELMAP_ALLDN) // Write Reelmap
				{
					if (!m_bTHREAD_DISP_DEF)
					{
						m_nMkStAuto++;
						m_nStepTHREAD_DISP_DEF = 0;

						m_bTHREAD_DISP_DEF = TRUE;		// DispDefImg() : CopyDefImg Start -> Disp Reelmap Start
						pDoc->SetMkMenu01(_T("Signal"), _T("DispDefImg"), _T("1"));
					}
				}
			}

			break;

		case MK_ST + (Mk2PtIdx::ChkSn) + 1:
			m_nMkStAuto++;
			nSerial = m_nBufUpSerial[0];
			sNewLot = m_sNewLotUp;

			if (nSerial > 0)
			{
				if (m_bSerialDecrese)
				{
					if (m_bLastProc && m_nBufUpSerial[1] < m_nLotEndSerial)
						nLastShot = m_nBufUpSerial[0];
				}
				else
				{
					if (m_bLastProc && m_nBufUpSerial[1] > m_nLotEndSerial)
					{
						nLastShot = m_nBufUpSerial[0];
						//nSerial = m_nBufUpSerial[0];
					}
				}

				bNewModel = GetAoiUpInfo(nSerial, &nNewLot, TRUE);
				if (bDualTest)
				{
					bNewModel = GetAoiDnInfo(nSerial, &nNewLot, TRUE);

					if (!IsSameUpDnLot() && !m_bContDiffLot)
					{
						m_nMkStAuto = MK_ST + (Mk2PtIdx::LotDiff);
						break;
					}
				}

				if (bNewModel)	// AOI ����(AoiCurrentInfoPath) -> AOI Feeding Offset
				{
					;
				}
				if (nNewLot)
				{
					// Lot Change.
					ChgLot();
#ifdef USE_MPE
					dFdEnc = (double)pDoc->m_pMpeData[0][0];	// ��ŷ�� Feeding ���ڴ� ��(���� mm )
					if ((pDoc->WorkingInfo.LastJob.bLotSep || pDoc->m_bDoneChgLot) && (dFdEnc + _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen)*2.0) > _tstof(pDoc->WorkingInfo.LastJob.sLotSepLen)*1000.0)
					{
						pDoc->m_bDoneChgLot = TRUE;
						//pView->SetPathAtBuf();
						pView->SetPathAtBuf();
					}
#endif
				}
			}
			else
			{
				Stop();
				MsgBox(_T("����(��) Serial�� �����ʽ��ϴ�."));
				//TowerLamp(RGB_YELLOW, TRUE);
			}
			//sNewLot.Empty();
			break;

		case MK_ST + (Mk2PtIdx::ChkSn) + 2:
			//pDoc->UpdateYieldOnRmap(); // 20230614

#ifdef USE_SR1000W
			if (pDoc->GetTestMode() == MODE_INNER || pDoc->GetTestMode() == MODE_OUTER)
			{
				if (m_pSr1000w && m_pSr1000w->IsConnected())
				{
					//Set2dRead(TRUE);
					//Get2dCode(sLot, nSerial);
					if (m_sGet2dCodeLot != _T("") && m_nGet2dCodeSerial != 0)
					{
						if (m_nBufUpSerial[0] == m_nGet2dCodeSerial)
						{
							m_nMkStAuto = MK_ST + (Mk2PtIdx::InitMk);			// InitMk()
						}
						else
						{
							Stop();
							pView->ClrDispMsg();
							sMsg.Format(_T("2D���� �ø���(%d)�� �˻����� �ø���(%d)�� �ٸ��ϴ�."), m_nGet2dCodeSerial, m_nBufUpSerial[0]);
							AfxMessageBox(sMsg);
							m_nMkStAuto = MK_ST + (Mk2PtIdx::Start);
							break;
						}
					}
					else
						m_nMkStAuto = MK_ST + (Mk2PtIdx::InitMk);			// InitMk()

				}
				else
				{
					Stop();
					pView->ClrDispMsg();
					sMsg.Format(_T("2D �����Ⱑ ������ ���� �ʾҽ��ϴ�."));
					AfxMessageBox(sMsg);
					m_nMkStAuto = MK_ST + (Mk2PtIdx::Start);
					break;
				}
			}
			else
				m_nMkStAuto = MK_ST + (Mk2PtIdx::InitMk);					// InitMk()
#else
			m_nMkStAuto = MK_ST + (Mk2PtIdx::InitMk);					// InitMk()
#endif
			break;
		}
	}
}

void CManagerProcedure::Mk2PtInit()
{
	if (!pView->m_mgrPunch)
		return;
	if (m_bTHREAD_SHIFT2MK)
		return;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk2PtIdx::InitMk) :
			if (pView->m_mgrPunch->InitMk()) // �ø����� ���� �Ǵ� ���� : TRUE �ƴϰ� ������ : FALSE
			{
				if (m_bSerialDecrese)
				{
					if ((m_nBufUpSerial[0] <= m_nLotEndSerial
						|| m_nBufUpSerial[1] <= m_nLotEndSerial)
						&& m_nLotEndSerial > 0)
					{
						pView->MpeWrite(_T("MB440171"), 1); // ��ŷ�� �۾��Ϸ�.(PC�� On, PLC�� Ȯ�� �� Off) - 20160718
						pDoc->Log(_T("PC: ��ŷ�� �۾��Ϸ�.(PC�� On, PLC�� Ȯ�� �� Off)"));
					}
				}
				else
				{
					if ((m_nBufUpSerial[0] >= m_nLotEndSerial
						|| m_nBufUpSerial[1] >= m_nLotEndSerial)
						&& m_nLotEndSerial > 0)
					{
						pView->MpeWrite(_T("MB440171"), 1); // ��ŷ�� �۾��Ϸ�.(PC�� On, PLC�� Ȯ�� �� Off) - 20160718
						pDoc->Log(_T("PC: ��ŷ�� �۾��Ϸ�.(PC�� On, PLC�� Ȯ�� �� Off)"));
					}
				}
			}
			else // Same Serial
			{
				Stop();
				MsgBox(_T("Serial ���� �����ʽ��ϴ�."));
				//TowerLamp(RGB_YELLOW, TRUE);
			}
										m_nMkStAuto++;
										break;

		case MK_ST + (Mk2PtIdx::InitMk) + 1:
			if (IsRun())
			{
				if (MODE_INNER != pDoc->GetTestMode())
					m_nMkStAuto = MK_ST + (Mk2PtIdx::Move0Cam1);	// Move - Cam1 - Pt0
				else
					m_nMkStAuto = MK_ST + (Mk2PtIdx::MoveInitPt);
			}
			break;
		}
	}
}

void CManagerProcedure::Mk2PtAlignPt0()
{
	if (!pView->m_mgrPunch)
		return;
	if (!IsRun())
		return;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk2PtIdx::Move0Cam1) :	// Move - Cam1 - Pt0
			if (bDualTest)
			{
				if (m_bSerialDecrese)
				{
					if (m_bLastProc && m_nBufDnSerial[1] < m_nLotEndSerial)	// AOI�ϸ� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrPunch->m_bDoMk[1] = FALSE;
						pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else if (m_nBufDnSerial[1] == 0)
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrPunch->m_bDoMk[1] = FALSE;
						pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(0))	// Move - Cam1 - Pt0
							m_nMkStAuto++;
					}
				}
				else
				{
					if (m_bLastProc && m_nBufDnSerial[1] > m_nLotEndSerial)	// AOI�ϸ� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrPunch->m_bDoMk[1] = FALSE;
						pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else if (m_nBufDnSerial[1] == 0)
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrPunch->m_bDoMk[1] = FALSE;
						pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(0))	// Move - Cam1 - Pt0
							m_nMkStAuto++;
					}
				}
			}
			else
			{
				if (m_bSerialDecrese)
				{
					if (m_bLastProc && m_nBufUpSerial[1] < m_nLotEndSerial)	// AOI��� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrPunch->m_bDoMk[1] = FALSE;
						pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else if (m_nBufUpSerial[1] == 0)
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrPunch->m_bDoMk[1] = FALSE;
						pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(0)) 	// Move - Cam1 - Pt0
							m_nMkStAuto++;
					}
				}
				else
				{
					if (m_bLastProc && m_nBufUpSerial[1] > m_nLotEndSerial)	// AOI��� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrPunch->m_bDoMk[1] = FALSE;
						pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else if (m_nBufUpSerial[1] == 0)
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrPunch->m_bDoMk[1] = FALSE;
						pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(0)) 	// Move - Cam1 - Pt0
							m_nMkStAuto++;
					}
				}
			}
												break;
		case MK_ST + (Mk2PtIdx::Move0Cam1) + 1:
			if (IsRun())
				m_nMkStAuto = MK_ST + (Mk2PtIdx::Move0Cam0);
			break;
		case MK_ST + (Mk2PtIdx::Move0Cam0) :	// Move - Cam0 - Pt0
			if (MoveAlign0(0))
				m_nMkStAuto++;
			break;
		case MK_ST + (Mk2PtIdx::Move0Cam0) + 1:
			if (IsRun())
				m_nMkStAuto++;
			break;
		case MK_ST + (Mk2PtIdx::Move0Cam0) + 2:
			if (IsMoveDone())
			{
				Sleep(100);
				m_nMkStAuto = MK_ST + (Mk2PtIdx::Align1_0);
			}
			break;
		case MK_ST + (Mk2PtIdx::Align1_0) :	// 2PtAlign - Cam1 - Pt0
			if (!m_bSkipAlign[1][0])
			{
				if (TwoPointAlign1(0))
					m_bFailAlign[1][0] = FALSE;
				else
					m_bFailAlign[1][0] = TRUE;
			}
											m_nMkStAuto = MK_ST + (Mk2PtIdx::Align0_0);
											break;
		case MK_ST + (Mk2PtIdx::Align0_0) :	// 2PtAlign - Cam0 - Pt0
			if (!m_bSkipAlign[0][0])
			{
				if (TwoPointAlign0(0))
					m_bFailAlign[0][0] = FALSE;
				else
					m_bFailAlign[0][0] = TRUE;
			}
											m_nMkStAuto++;
											break;
		case MK_ST + (Mk2PtIdx::Align0_0) + 1:
			if (m_bFailAlign[0][0])
			{
				Buzzer(TRUE, 0);
				if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ٽ� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);

					if (IDYES == MsgBox(_T("ī�޶�(��)�� �˻��ǳ� ������ ���� ���Ͻðڽ��ϱ�?"), 0, MB_YESNO))
					{
						m_bReAlign[0][0] = FALSE;
						m_bSkipAlign[0][0] = TRUE;
						m_bSkipAlign[0][1] = TRUE;
						m_bSkipAlign[0][2] = TRUE;
						m_bSkipAlign[0][3] = TRUE;
						if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ҷ���ŷ �Ͻðڽ��ϱ�?"), 0, MB_YESNO))
						{
							pView->m_mgrPunch->m_bDoMk[0] = FALSE;
							pView->m_mgrPunch->m_bDoneMk[0] = TRUE;
						}
						else
						{
							pView->m_mgrPunch->m_bDoMk[0] = TRUE;
							pView->m_mgrPunch->m_bDoneMk[0] = FALSE;
						}
					}
					else
					{
						m_bReAlign[0][0] = TRUE;
						m_bSkipAlign[0][0] = FALSE;
						m_bSkipAlign[0][1] = FALSE;
						m_bSkipAlign[0][2] = FALSE;
						m_bSkipAlign[0][3] = FALSE;
						m_nMkStAuto = MK_ST + (Mk2PtIdx::Move0Cam0); // TwoPointAlign0(0) ���� ����. - ī�޶� ������
						Stop();
						//TowerLamp(RGB_YELLOW, TRUE);
					}
				}
				else
				{
					Buzzer(FALSE, 0);

					m_bReAlign[0][0] = TRUE;
					m_bSkipAlign[0][0] = FALSE;
					m_bSkipAlign[0][1] = FALSE;
					m_nMkStAuto = MK_ST + (Mk2PtIdx::Move0Cam0); // TwoPointAlign0(0) ���� ����. - ī�޶� ������
					Stop();
					//TowerLamp(RGB_YELLOW, TRUE);
				}
			}
			if (m_bFailAlign[1][0])
			{
				Buzzer(TRUE, 0);

				if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ٽ� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);

					if (IDYES == MsgBox(_T("ī�޶�(��)�� �˻��ǳ� ������ ���� ���Ͻðڽ��ϱ�?"), 0, MB_YESNO))
					{
						m_bReAlign[1][0] = FALSE;
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ҷ���ŷ �Ͻðڽ��ϱ�?"), 0, MB_YESNO))
						{
							pView->m_mgrPunch->m_bDoMk[1] = FALSE;
							pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						}
						else
						{
							pView->m_mgrPunch->m_bDoMk[1] = TRUE;
							pView->m_mgrPunch->m_bDoneMk[1] = FALSE;
						}
					}
					else
					{
						m_bReAlign[1][0] = TRUE;
						m_bSkipAlign[1][0] = FALSE;
						m_bSkipAlign[1][1] = FALSE;
						m_bSkipAlign[1][2] = FALSE;
						m_bSkipAlign[1][3] = FALSE;
						m_nMkStAuto = MK_ST + (Mk2PtIdx::Move0Cam1); // TwoPointAlign1(0) ���� ����. - ī�޶� ������
						Stop();
						//TowerLamp(RGB_YELLOW, TRUE);
					}
				}
				else
				{
					Buzzer(FALSE, 0);

					m_bReAlign[1][0] = TRUE;
					m_bSkipAlign[1][0] = FALSE;
					m_bSkipAlign[1][1] = FALSE;
					m_bSkipAlign[1][2] = FALSE;
					m_bSkipAlign[1][3] = FALSE;
					m_nMkStAuto = MK_ST + (Mk2PtIdx::Move0Cam1); // TwoPointAlign1(0) ���� ����. - ī�޶� ������
					Stop();
					//TowerLamp(RGB_YELLOW, TRUE);
				}
			}

			if (m_bFailAlign[0][0] || m_bFailAlign[1][0])
			{
				if (!m_bReAlign[0][0] && !m_bReAlign[1][0])
				{
					if (pView->m_mgrPunch->m_bDoMk[0] || pView->m_mgrPunch->m_bDoMk[1])
						m_nMkStAuto++; //m_nMkStAuto = MK_ST + 27; // MoveInitPos0()
					else
					{
						if (!IsInitPos0())
							MoveInitPos0();
						if (!IsInitPos1())
							MoveInitPos1();

						m_nMkStAuto = MK_ST + (Mk2PtIdx::DoneMk); // // �ҷ��̹��� Display, Align���� �ʱ�ȭ (Skip 65 : Mk())
					}
				}
				else
				{
					m_nMkStAuto = MK_ST + (Mk2PtIdx::Move0Cam1); // TwoPointAlign1(0) ���� ����. - ī�޶� ������
				}
			}
			else
				m_nMkStAuto++;

			break;
		case MK_ST + (Mk2PtIdx::Align0_0) + 2:
			if (IsRun())
				m_nMkStAuto = MK_ST + (Mk2PtIdx::Move1Cam1);
			break;
		}
	}
}

void CManagerProcedure::Mk2PtAlignPt1()
{
	if (!pView->m_mgrPunch)
		return;
	if (!IsRun())
		return;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk2PtIdx::Move1Cam1) :
			if (bDualTest)
			{
				if (m_bSerialDecrese)
				{
					if (m_bLastProc && m_nBufDnSerial[1] < m_nLotEndSerial)	// AOI�ϸ� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrPunch->m_bDoMk[1] = FALSE;
						pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else if (m_nBufDnSerial[1] == 0)
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrPunch->m_bDoMk[1] = FALSE;
						pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(1))	// Move - Cam1 - Pt1
							m_nMkStAuto++;
					}
				}
				else
				{
					if (m_bLastProc && m_nBufDnSerial[1] > m_nLotEndSerial)	// AOI�ϸ� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrPunch->m_bDoMk[1] = FALSE;
						pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else if (m_nBufDnSerial[1] == 0)
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrPunch->m_bDoMk[1] = FALSE;
						pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(1))	// Move - Cam1 - Pt1
							m_nMkStAuto++;
					}
				}
			}
			else
			{
				if (m_bSerialDecrese)
				{
					if (m_bLastProc && m_nBufUpSerial[1] < m_nLotEndSerial)	// AOI��� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrPunch->m_bDoMk[1] = FALSE;
						pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else if (m_nBufUpSerial[1] == 0)
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrPunch->m_bDoMk[1] = FALSE;
						pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(1))	// Move - Cam1 - Pt1
							m_nMkStAuto++;
					}
				}
				else
				{
					if (m_bLastProc && m_nBufUpSerial[1] > m_nLotEndSerial)	// AOI��� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrPunch->m_bDoMk[1] = FALSE;
						pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else if (m_nBufUpSerial[1] == 0)
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrPunch->m_bDoMk[1] = FALSE;
						pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(1))	// Move - Cam1 - Pt1
							m_nMkStAuto++;
					}
				}
			}
										   break;
		case MK_ST + (Mk2PtIdx::Move1Cam1) + 1:
			if (IsRun())
				m_nMkStAuto++;
			break;
		case MK_ST + (Mk2PtIdx::Move1Cam0) :
			if (MoveAlign0(1))	// Move - Cam0 - Pt1
				m_nMkStAuto++;
			break;
		case MK_ST + (Mk2PtIdx::Move1Cam0) + 1:
			if (IsRun())
				m_nMkStAuto++;
			break;
		case MK_ST + (Mk2PtIdx::Move1Cam0) + 2:
			if (IsMoveDone())
			{
				Sleep(100);
				m_nMkStAuto = MK_ST + (Mk2PtIdx::Align1_1);
			}
			break;
		case MK_ST + (Mk2PtIdx::Align1_1) :	// 2PtAlign - Cam1 - Pt1
			if (!m_bFailAlign[1][0])
			{
				if (!m_bSkipAlign[1][1])
				{
					if (!TwoPointAlign1(1))
						m_bFailAlign[1][1] = TRUE;
					else
						m_bFailAlign[1][1] = FALSE;
				}
				else
					m_bFailAlign[1][1] = FALSE;
			}
			else
				m_bFailAlign[1][1] = FALSE;

			m_nMkStAuto = MK_ST + (Mk2PtIdx::Align0_1);
			break;
		case MK_ST + (Mk2PtIdx::Align0_1) :	// 2PtAlign - Cam0 - Pt1
			if (!m_bFailAlign[0][0])
			{
				if (!m_bSkipAlign[0][1])
				{
					if (!TwoPointAlign0(1))
						m_bFailAlign[0][1] = TRUE;
					else
						m_bFailAlign[0][1] = FALSE;
				}
				else
					m_bFailAlign[0][1] = FALSE;
			}
			else
				m_bFailAlign[0][1] = FALSE;

			m_nMkStAuto++;
			break;
		case MK_ST + (Mk2PtIdx::Align0_1) + 1:
			if (m_bFailAlign[0][1])
			{
				Buzzer(TRUE, 0);

				if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ٽ� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);

					if (IDYES == MsgBox(_T("ī�޶�(��)�� �˻��ǳ� ������ ���� ���Ͻðڽ��ϱ�?"), 0, MB_YESNO))
					{
						m_bReAlign[0][1] = FALSE;
						m_bSkipAlign[0][1] = TRUE;
						if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ҷ���ŷ �Ͻðڽ��ϱ�?"), 0, MB_YESNO))
						{
							pView->m_mgrPunch->m_bDoMk[0] = FALSE;
							pView->m_mgrPunch->m_bDoneMk[0] = TRUE;
						}
						else
						{
							pView->m_mgrPunch->m_bDoMk[0] = TRUE;
							pView->m_mgrPunch->m_bDoneMk[0] = FALSE;
						}
					}
					else
					{
						m_bReAlign[0][1] = TRUE;
						m_bSkipAlign[0][1] = FALSE;
						m_nMkStAuto = MK_ST + (Mk2PtIdx::Move1Cam0); // TwoPointAlign0(1) ���� ����. - ī�޶� ������
						Stop();
						//TowerLamp(RGB_YELLOW, TRUE);
					}
				}
				else
				{
					Buzzer(FALSE, 0);

					m_bReAlign[0][1] = TRUE;
					m_bSkipAlign[0][1] = FALSE;
					m_nMkStAuto = MK_ST + (Mk2PtIdx::Move1Cam0); // TwoPointAlign1(1) ���� ����. - ī�޶� ������
					Stop();
					//TowerLamp(RGB_YELLOW, TRUE);
				}
			}
			if (m_bFailAlign[1][1])
			{
				Buzzer(TRUE, 0);

				if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ٽ� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);

					if (IDYES == MsgBox(_T("ī�޶�(��)�� �˻��ǳ� ������ ���� ���Ͻðڽ��ϱ�?"), 0, MB_YESNO))
					{
						m_bReAlign[1][1] = FALSE;
						m_bSkipAlign[1][1] = TRUE;
						if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ҷ���ŷ �Ͻðڽ��ϱ�?"), 0, MB_YESNO))
						{
							pView->m_mgrPunch->m_bDoMk[1] = FALSE;
							pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						}
						else
						{
							pView->m_mgrPunch->m_bDoMk[1] = TRUE;
							pView->m_mgrPunch->m_bDoneMk[1] = FALSE;
						}
					}
					else
					{
						m_bReAlign[1][1] = TRUE;
						m_bSkipAlign[1][1] = FALSE;
						m_nMkStAuto = MK_ST + (Mk2PtIdx::Move1Cam1); // TwoPointAlign1(1) ���� ����. - ī�޶� ������
						Stop();
						//TowerLamp(RGB_YELLOW, TRUE);
					}
				}
				else
				{
					Buzzer(FALSE, 0);

					m_bReAlign[1][1] = TRUE;
					m_bSkipAlign[1][1] = FALSE;
					m_nMkStAuto = MK_ST + (Mk2PtIdx::Move1Cam1); // TwoPointAlign1(1) ���� ����. - ī�޶� ������
					Stop();
					//TowerLamp(RGB_YELLOW, TRUE);
				}
			}

			if (m_bFailAlign[0][1] || m_bFailAlign[1][1])
			{
				if (!m_bReAlign[0][1] && !m_bReAlign[1][1])
				{
					if (pView->m_mgrPunch->m_bDoMk[0] || pView->m_mgrPunch->m_bDoMk[1])
						m_nMkStAuto = MK_ST + (Mk2PtIdx::MoveInitPt); //m_nMkStAuto = MK_ST + 29;  // MoveInitPos0()
					else
					{
						if (!IsInitPos0())
							MoveInitPos0();
						if (!IsInitPos1())
							MoveInitPos1();

						m_nMkStAuto = MK_ST + (Mk2PtIdx::DoneMk); // �ҷ��̹��� Display, Align���� �ʱ�ȭ (Skip 65 : Mk())
					}
				}
				else
					m_nMkStAuto = MK_ST + (Mk2PtIdx::Move1Cam1); // TwoPointAlign1(1) ���� ����. - ī�޶� ������
			}
			else
				m_nMkStAuto = MK_ST + (Mk2PtIdx::MoveInitPt);

			break;
		}
	}
}

void CManagerProcedure::Mk2PtMoveInitPos()
{
	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk2PtIdx::MoveInitPt) :
			MoveInitPos0(FALSE);
			MoveInitPos1(FALSE); // 20220526
			m_nMkStAuto++;
			break;
		case MK_ST + (Mk2PtIdx::MoveInitPt) + 1:
			m_nMkStAuto++;
			break;
		case MK_ST + (Mk2PtIdx::MoveInitPt) + 2:
			if (IsMoveDone())
				m_nMkStAuto = MK_ST + (Mk2PtIdx::ChkElec);
			break;
		}
	}
}

void CManagerProcedure::Mk2PtElecChk()
{
	if (!pView->m_mgrPunch)
		return;

	CString sRst;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk2PtIdx::ChkElec) : // DoElecChk
			if (DoElecChk(sRst))
			{
				if (pDoc->WorkingInfo.Probing[0].bUse)
				{
					if (sRst == _T("Open"))
					{
						if (pDoc->WorkingInfo.Probing[0].bStopOnOpen)
							m_nMkStAuto = REJECT_ST;
						else
							m_nMkStAuto++;
					}
					else if (sRst == _T("Error"))
					{
						m_nMkStAuto = ERROR_ST;
					}
					else
					{
						m_nMkStAuto++;
					}
				}
				else
					m_nMkStAuto++;
			}
										   break;

		case MK_ST + (Mk2PtIdx::ChkElec) + 1:
			if (ChkLightErr())
			{
				m_bChkLightErr = FALSE;
				m_nMkStAuto++;
			}
			else
			{
				if (MODE_INNER != pDoc->GetTestMode())
					m_nMkStAuto = MK_ST + (Mk2PtIdx::DoMk); 	// Mk ��ŷ ����
				else
					m_nMkStAuto = MK_ST + (Mk2PtIdx::Shift2Mk);
			}
			break;

		case MK_ST + (Mk2PtIdx::ChkElec) + 2:
			if (IsRun())
			{
				/*if (m_pMotion->IsEnable(MS_X0) && m_pMotion->IsEnable(MS_Y0) &&
					m_pMotion->IsEnable(MS_X1) && m_pMotion->IsEnable(MS_Y1))*/
				if(pView->m_mgrPunch->IsMotionEnable())
				{
					if (MODE_INNER != pDoc->GetTestMode())
						m_nMkStAuto = MK_ST + (Mk2PtIdx::DoMk); 	// Mk ��ŷ ����
					else
						m_nMkStAuto = MK_ST + (Mk2PtIdx::Shift2Mk);
				}
				else
				{
					Stop();
					MsgBox(_T("��ŷ�� ����� ��Ȱ��ȭ �Ǿ����ϴ�."));
					//TowerLamp(RGB_RED, TRUE);
				}
			}
			else
			{
				if (!m_bChkLightErr)
				{
					m_bChkLightErr = TRUE;
					MsgBox(_T("�뱤�ҷ� ���� - ������ Ȯ���ϼ���.\r\n��������Ϸ��� ��������ġ�� ��������."));
				}
			}
			break;
		}
	}
}

void CManagerProcedure::Mk2PtDoMarking()
{
	if (!pView->m_mgrPunch || !pView->m_mgrReelmap)
		return;
	int nMaxStrip;
#ifdef USE_CAM_MASTER
	nMaxStrip = pView->m_mgrReelmap->m_Master[0].GetStripNum(); // �� ��Ʈ���� ����
#else
#ifdef TEST_MODE
	nMaxStrip = 4;
#else
	nMaxStrip = MAX_STRIP;
#endif
#endif

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	CString sRst, sMsg;
	int a, b, nSerial, nPrevSerial;
	double dTotLmt, dPrtLmt, dRatio;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk2PtIdx::DoMk) :				// Mk ��ŷ ����
			if (pDoc->GetTestMode() == MODE_OUTER)
				pView->m_mgrPunch->SetMkIts(TRUE);						// ITS ��ŷ ����
			else
				pView->m_mgrPunch->SetMk(TRUE);						// Mk ��ŷ ����

			m_nMkStAuto++;
			break;

		case MK_ST + (Mk2PtIdx::DoMk) + 1:
			if (!m_bUpdateYield)
			{
				if (!m_bTHREAD_UPDATAE_YIELD[0] && !m_bTHREAD_UPDATAE_YIELD[1])
				{
					m_bUpdateYield = TRUE;
					UpdateYield(); // Cam[0],  Cam[1]
					m_nMkStAuto++;
				}
			}
			else
			{
				Sleep(100);
				m_nMkStAuto++;
			}
			break;

		case MK_ST + (Mk2PtIdx::DoMk) + 2:
			if (!m_bTHREAD_UPDATAE_YIELD[0] && !m_bTHREAD_UPDATAE_YIELD[1])
			{
				if (!m_bUpdateYieldOnRmap)
				{
					if (!m_bTHREAD_UPDATE_REELMAP_UP && !m_bTHREAD_UPDATE_REELMAP_DN && !m_bTHREAD_UPDATE_REELMAP_ALLUP && !m_bTHREAD_UPDATE_REELMAP_ALLDN)
					{
						if (!m_bTHREAD_UPDATE_YIELD_UP && !m_bTHREAD_UPDATE_YIELD_DN && !m_bTHREAD_UPDATE_YIELD_ALLUP && !m_bTHREAD_UPDATE_YIELD_ALLDN)
						{
							m_bUpdateYieldOnRmap = TRUE;
							pView->UpdateYieldOnRmap(); // 20230614
							m_nMkStAuto++;
						}
						else
							Sleep(100);
					}
					else
						Sleep(100);
				}
				else
				{
					Sleep(100);
					m_nMkStAuto++; // ��ŷ �� verify�� ������ ������ ���� ���.
				}
			}
			break;

		case MK_ST + (Mk2PtIdx::Verify) :
			if (IsMkDone() && IsMoveDone())
			{
				if (IsVerify() && !m_nPrevMkStAuto)
				{
					m_nPrevMkStAuto = MK_ST + (Mk2PtIdx::Verify);
					m_nMkStAuto = MK_ST + (Mk2PtIdx::DoMk);		// Mk ��ŷ ����
					pView->m_mgrPunch->m_bCam = TRUE;
				}
				else
				{
					if (pView->m_mgrPunch->IsReview())
					{
						if (!pView->m_mgrPunch->m_bCam)
						{
							m_nPrevStepAuto = MK_ST + (Mk2PtIdx::Verify);
							m_nMkStAuto = MK_ST + (Mk2PtIdx::DoMk);		// Mk ��ŷ ����
							pView->m_mgrPunch->m_bCam = TRUE;
							MsgBox(_T("�� Jog ��ư�� �̿��Ͽ� ��ŷ��ġ�� Ȯ���Ͽ� �ּ���."));
						}
						else
						{
							pView->m_mgrPunch->m_bCam = FALSE;
							m_nMkStAuto = MK_ST + (Mk2PtIdx::DoneMk);	// Mk ��ŷ �Ϸ�
						}
					}
					else
					{
						m_nMkStAuto = MK_ST + (Mk2PtIdx::DoneMk);	// Mk ��ŷ �Ϸ�
					}
				}
			}
			else if (pView->m_mgrPunch->IsReMk())
			{
				m_nPrevMkStAuto = MK_ST + (Mk2PtIdx::Verify);
				m_nMkStAuto = MK_ST + (Mk2PtIdx::DoMk);		// Mk �����
			}
			else
			{
				sMsg = _T("");
				sMsg += m_sDispSts[0];
				sMsg += _T(",");
				sMsg += m_sDispSts[1];
			}
										break;

		case MK_ST + (Mk2PtIdx::DoneMk) :	 // Align���� �ʱ�ȭ
			if (!IsRun())
				break;

			//if (m_bTHREAD_UPDATE_YIELD_UP || m_bTHREAD_UPDATE_YIELD_DN || m_bTHREAD_UPDATE_YIELD_ALLUP || m_bTHREAD_UPDATE_YIELD_ALLDN)
			//	break;

			if (m_bInitAuto)
			{
				m_bInitAuto = FALSE;
				MsgBox(_T("��ŷ��ġ�� Ȯ���ϼ���."));
				Stop();
				//TowerLamp(RGB_YELLOW, TRUE);
				break;
			}

			if (m_nBufUpSerial[0] == 0)
			{
				m_bSkipAlign[0][0] = TRUE;
				m_bSkipAlign[0][1] = TRUE;
			}
			if (m_nBufUpSerial[1] == 0)
			{
				m_bSkipAlign[1][0] = TRUE;
				m_bSkipAlign[1][1] = TRUE;
			}

			if ((!m_bSkipAlign[0][0] && !m_bSkipAlign[0][1])
				&& (!m_bSkipAlign[1][0] && !m_bSkipAlign[1][1]))
				CompletedMk(2); // 0: Only Cam0, 1: Only Cam1, 2: Cam0 and Cam1, 3: None
			else if ((m_bSkipAlign[0][0] || m_bSkipAlign[0][1])
				&& (!m_bSkipAlign[1][0] && !m_bSkipAlign[1][1]))
				CompletedMk(1); // 0: Only Cam0, 1: Only Cam1, 2: Cam0 and Cam1, 3: None
			else if ((!m_bSkipAlign[0][0] && !m_bSkipAlign[0][1])
				&& (m_bSkipAlign[1][0] || m_bSkipAlign[1][1]))
				CompletedMk(0); // 0: Only Cam0, 1: Only Cam1, 2: Cam0 and Cam1, 3: None
			else
				CompletedMk(3); // 0: Only Cam0, 1: Only Cam1, 2: Cam0 and Cam1, 3: None

			m_nMkStAuto++;
			break;
		case MK_ST + (Mk2PtIdx::DoneMk) + 1:  // Mk���� �ʱ�ȭ
			m_bReAlign[0][0] = FALSE; // [nCam][nPos] 
			m_bReAlign[0][1] = FALSE; // [nCam][nPos] 
			m_bReAlign[1][0] = FALSE; // [nCam][nPos] 
			m_bReAlign[1][1] = FALSE; // [nCam][nPos] 

			m_bSkipAlign[0][0] = FALSE; // [nCam][nPos] 
			m_bSkipAlign[0][1] = FALSE; // [nCam][nPos] 
			m_bSkipAlign[1][0] = FALSE; // [nCam][nPos] 
			m_bSkipAlign[1][1] = FALSE; // [nCam][nPos] 

			m_bFailAlign[0][0] = FALSE; // [nCam][nPos] 
			m_bFailAlign[0][1] = FALSE; // [nCam][nPos] 
			m_bFailAlign[1][0] = FALSE; // [nCam][nPos] 
			m_bFailAlign[1][1] = FALSE; // [nCam][nPos] 

			pView->m_mgrPunch->m_bDoMk[0] = TRUE;
			pView->m_mgrPunch->m_bDoMk[1] = TRUE;
			pView->m_mgrPunch->m_bDoneMk[0] = FALSE;
			pView->m_mgrPunch->m_bDoneMk[1] = FALSE;
			pView->m_mgrPunch->m_bReMark[0] = FALSE;
			pView->m_mgrPunch->m_bReMark[1] = FALSE;
			pView->m_mgrPunch->m_bCam = FALSE;
			m_nPrevMkStAuto = 0;

			m_bUpdateYield = FALSE;
			m_bUpdateYieldOnRmap = FALSE;

			for (a = 0; a < 2; a++)
			{
				for (b = 0; b < MAX_STRIP; b++)
				{
					pView->m_mgrPunch->m_nMkStrip[a][b] = 0;
					pView->m_mgrPunch->m_bRejectDone[a][b] = FALSE;
				}
			}

			m_nSaveMk0Img = 0;
			m_nSaveMk1Img = 0;

			pView->MpeWrite(_T("MB440150"), 0);	// ��ŷ�� ��ŷ�� ON (PC�� ON, OFF)
			pView->MpeWrite(_T("MB440170"), 1);	// ��ŷ�Ϸ�(PLC�� Ȯ���ϰ� Reset��Ŵ.)-20141029
			pDoc->Log(_T("PC: ��ŷ�Ϸ�(PLC�� Ȯ���ϰ� Reset��Ŵ.)"));
			if (pView->IsNoMk() || IsShowLive())
				ShowLive(FALSE);

			m_nMkStAuto++;
			break;

		case MK_ST + (Mk2PtIdx::DoneMk) + 2:
			if (pDoc->m_pMpeSignal[0] & (0x01 << 1))	// ��ŷ�� Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)-20141030
			{
				pDoc->Log(_T("PLC: ��ŷ�� Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)"));
				m_nMkStAuto++;
			}
			break;

		case MK_ST + (Mk2PtIdx::DoneMk) + 3:
#ifdef USE_MPE
			//if (pDoc->m_pMpeSignal[0] & (0x01 << 1))	// ��ŷ�� Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)-20141030
		{
			if (!m_bTHREAD_SHIFT2MK)
			{
				pView->MpeWrite(_T("MB440101"), 0);	// ��ŷ�� Feeding�Ϸ�
				pDoc->Log(_T("PC: ��ŷ�� Feeding�Ϸ� OFF"));

													//Shift2Mk();			// PCR �̵�(Buffer->Marked) // ���(WorkingInfo.LastJob.sSerial)
				m_bShift2Mk = TRUE;
				DoShift2Mk();

				SetMkFdLen();
				SetCycTime();
				m_dwCycSt = GetTickCount();

				UpdateRst();

				//UpdateWorking();	// Update Working Info...
				m_nMkStAuto++;
			}
		}
#endif
		break;
		case MK_ST + (Mk2PtIdx::DoneMk) + 4:
			//sMsg.Format(_T("%d%d%d%d%d"), m_bTHREAD_SHIFT2MK ? 1 : 0, m_bTHREAD_REELMAP_YIELD_UP ? 1 : 0, m_bTHREAD_REELMAP_YIELD_DN ? 1 : 0, m_bTHREAD_REELMAP_YIELD_ALLUP ? 1 : 0, m_bTHREAD_REELMAP_YIELD_ALLDN ? 1 : 0);
			//DispStsBar(sMsg, 0);

			if (!m_bTHREAD_SHIFT2MK && !m_bTHREAD_REELMAP_YIELD_UP
				&& !m_bTHREAD_REELMAP_YIELD_DN && !m_bTHREAD_REELMAP_YIELD_ALLUP
				&& !m_bTHREAD_REELMAP_YIELD_ALLDN) // Yield Reelmap
			{
				if (pDoc->GetTestMode() == MODE_OUTER)
				{
					if (m_bTHREAD_REELMAP_YIELD_ITS) // Yield Reelmap
						break;
				}

				m_nMkStAuto++;
				//UpdateRst();
				//UpdateWorking();	// Update Working Info...
				if (!pView->ChkYield())// (dTotLmt, dPrtLmt, dRatio))
				{
					if (dTotLmt == 0.0 && dPrtLmt == 0.0)
					{
						Stop();
						sMsg.Format(_T("�Ͻ����� - Failed ChkYield()."));
						MsgBox(sMsg);
						//TowerLamp(RGB_RED, TRUE);
						Buzzer(TRUE, 0);

					}
					else if (dPrtLmt == 0.0)
					{
						Stop();
						sMsg.Format(_T("�Ͻ����� - ��ü ���� ���ѹ��� : %.1f �̴� ( %.1f )"), dTotLmt, dRatio);
						MsgBox(sMsg);
						//TowerLamp(RGB_RED, TRUE);
						Buzzer(TRUE, 0);
					}
					else
					{
						Stop();
						sMsg.Format(_T("�Ͻ����� - ���� ���� ���ѹ��� : %.1f �̴� ( %.1f )"), dPrtLmt, dRatio);
						MsgBox(sMsg);
						//TowerLamp(RGB_RED, TRUE);
						Buzzer(TRUE, 0);
					}
				}
			}
			break;
		case MK_ST + (Mk2PtIdx::DoneMk) + 5:
			if (!m_bTHREAD_SHIFT2MK)
			{
				SetListBuf();
				ChkLotCutPos();
				UpdateWorking();	// Update Working Info...
				m_nMkStAuto++;
			}
			break;
		case MK_ST + (Mk2PtIdx::DoneMk) + 6:
			m_nMkStAuto++;
			if (!IsBuffer(0))
			{
				m_bLotEnd = TRUE;
				m_nLotEndAuto = LOT_END;
			}
			else
			{
				// [0]: AOI-Up , [1]: AOI-Dn
				if (bDualTest)
				{
					nSerial = pDoc->m_ListBuf[0].Pop(); // Up (��)
					nSerial = pDoc->m_ListBuf[0].Pop(); // Up (��)

					SetListBuf();
				}

				if (m_nLotEndSerial > 0)
				{
					nSerial = GetBufferUp(&nPrevSerial);

					if (m_bSerialDecrese)
					{
						if (nSerial < m_nLotEndSerial || nSerial <= 0) // �뱤�ҷ� 3��° Lot End ( -1, -1, -2)
						{
							if (IsDoneDispMkInfo())
							{
								if (IsBuffer() && nSerial > 0)
									SetSerial(nSerial);
								else
									SetSerial(nPrevSerial - 1, TRUE);

								if (IsBuffer() && nSerial > 0)
									SetSerial(nSerial - 1);
								else
									SetSerial(nPrevSerial - 2, TRUE);

								//m_nStepAuto = LOT_END;
								m_bLotEnd = TRUE;
								m_nLotEndAuto = LOT_END;
							}
						}
					}
					else
					{
						if (nSerial > m_nLotEndSerial || nSerial <= 0) // �뱤�ҷ� 3��° Lot End ( -1, -1, -2)
						{
							if (IsDoneDispMkInfo())
							{
								if (IsBuffer() && nSerial > 0)
									SetSerial(nSerial);
								else
									SetSerial(nPrevSerial + 1, TRUE);

								if (IsBuffer() && nSerial > 0)
									SetSerial(nSerial + 1);
								else
									SetSerial(nPrevSerial + 2, TRUE);

								//m_nStepAuto = LOT_END;
								m_bLotEnd = TRUE;
								m_nLotEndAuto = LOT_END;
							}
						}
					}

					if (bDualTest)
					{
						nSerial = GetBufferDn(&nPrevSerial);

						if (m_bSerialDecrese)
						{
							if (nSerial < m_nLotEndSerial || nSerial <= 0) // �뱤�ҷ� 3��° Lot End ( -1, -1, -2)
							{
								if (IsDoneDispMkInfo())
								{
									if (IsBuffer() && nSerial > 0)
										SetSerial(nSerial);
									else
										SetSerial(nPrevSerial - 1, TRUE);

									if (IsBuffer() && nSerial > 0)
										SetSerial(nSerial - 1);
									else
										SetSerial(nPrevSerial - 2, TRUE);

									m_bLotEnd = TRUE;
									m_nLotEndAuto = LOT_END;
								}
							}
						}
						else
						{
							if (nSerial > m_nLotEndSerial || nSerial <= 0) // �뱤�ҷ� 3��° Lot End ( -1, -1, -2)
							{
								if (IsDoneDispMkInfo())
								{
									if (IsBuffer() && nSerial > 0)
										SetSerial(nSerial);
									else
										SetSerial(nPrevSerial + 1, TRUE);

									if (IsBuffer() && nSerial > 0)
										SetSerial(nSerial + 1);
									else
										SetSerial(nPrevSerial + 2, TRUE);

									m_bLotEnd = TRUE;
									m_nLotEndAuto = LOT_END;
								}
							}
						}
					}
				}
			}

			break;
		case MK_ST + (Mk2PtIdx::DoneMk) + 7:
			m_bMkSt = FALSE;
			::WritePrivateProfileString(_T("Last Job"), _T("MkSt"), _T("0"), PATH_WORKING_INFO);
			break;
		}
	}
}

void CManagerProcedure::Mk2PtShift2Mk() // MODE_INNER
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	CString sRst, sMsg;
	int a, b, nSerial, nPrevSerial;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk2PtIdx::Shift2Mk) :
			pView->MpeWrite(_T("MB440150"), 0);	// ��ŷ�� ��ŷ�� ON (PC�� ON, OFF)
			pView->MpeWrite(_T("MB440170"), 1);	// ��ŷ�Ϸ�(PLC�� Ȯ���ϰ� Reset��Ŵ.)-20141029
			pDoc->Log(_T("PC: ��ŷ�Ϸ�(PLC�� Ȯ���ϰ� Reset��Ŵ.)"));
			m_nMkStAuto++;
			break;

		case MK_ST + (Mk2PtIdx::Shift2Mk) + 1:
			if (pDoc->m_pMpeSignal[0] & (0x01 << 1))	// ��ŷ�� Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)-20141030
			{
				pDoc->Log(_T("PLC: ��ŷ�� Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)"));
				m_nMkStAuto++;
				//if (!m_bTHREAD_UPDATAE_YIELD[0] && !m_bTHREAD_UPDATAE_YIELD[1])
				//{
				//	UpdateYield(); // Cam[0],  Cam[1]
				//	m_nMkStAuto++;
				//}
			}
			break;

		case MK_ST + (Mk2PtIdx::Shift2Mk) + 2:
#ifdef USE_MPE
			//if (pDoc->m_pMpeSignal[0] & (0x01 << 1))	// ��ŷ�� Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)-20141030
		{
			if (!m_bTHREAD_SHIFT2MK)
			{
				pDoc->Log(_T("PC: ��ŷ�� Feeding�Ϸ� OFF"));
				pView->MpeWrite(_T("MB440101"), 0);	// ��ŷ�� Feeding�Ϸ�
				m_bShift2Mk = TRUE;
				DoShift2Mk();

				SetMkFdLen();
				SetCycTime();
				m_dwCycSt = GetTickCount();

				m_nMkStAuto++;
			}
		}
#endif
		break;
		case MK_ST + (Mk2PtIdx::Shift2Mk) + 3:
			if (!m_bTHREAD_SHIFT2MK && !m_bTHREAD_REELMAP_YIELD_UP
				&& !m_bTHREAD_REELMAP_YIELD_DN && !m_bTHREAD_REELMAP_YIELD_ALLUP
				&& !m_bTHREAD_REELMAP_YIELD_ALLDN) // Yield Reelmap
			{
				if (pDoc->GetTestMode() == MODE_OUTER)
				{
					if (m_bTHREAD_REELMAP_YIELD_ITS) // Yield Reelmap
						break;
				}

				m_nMkStAuto++;
				UpdateRst();
				UpdateWorking();	// Update Working Info...
				pView->ChkYield();// (dTotLmt, dPrtLmt, dRatio);
			}
			break;
		case MK_ST + (Mk2PtIdx::Shift2Mk) + 4:
			if (!m_bTHREAD_SHIFT2MK)
			{
				SetListBuf();
				ChkLotCutPos();
				m_nMkStAuto++;
			}
			break;
		case MK_ST + (Mk2PtIdx::Shift2Mk) + 5:
			m_nMkStAuto++;
			if (!IsBuffer(0))
			{
				m_bLotEnd = TRUE;
				m_nLotEndAuto = LOT_END;
			}
			else
			{
				// [0]: AOI-Up , [1]: AOI-Dn
				nSerial = pDoc->m_ListBuf[0].Pop(); // Up (��)
				nSerial = pDoc->m_ListBuf[0].Pop(); // Up (��)

				SetListBuf();

				if (m_bSerialDecrese)
				{
					if (m_nLotEndSerial > 0)
					{
						nSerial = GetBufferUp(&nPrevSerial); // m_pBufSerial[0][0] : ���� ù��° �ø���

						if (nSerial < m_nLotEndSerial || nSerial <= 0) // �뱤�ҷ� 3��° Lot End ( -1, -1, -2)
						{
							if (IsDoneDispMkInfo())
							{
								if (IsBuffer() && nSerial > 0)
									SetSerial(nSerial);
								//else
								//	SetSerial(nPrevSerial + 1, TRUE);

								nSerial = m_pBufSerial[0][1]; // m_pBufSerial[0][1] : ���� �ι�° �ø���
								if (IsBuffer() && nSerial > 0)
									SetSerial(nSerial);	//SetSerial(nSerial + 1);
														//else
														//	SetSerial(nPrevSerial + 2, TRUE);

								m_bLotEnd = TRUE;
								m_nLotEndAuto = LOT_END;
							}
						}
					}
				}
				else
				{
					if (m_nLotEndSerial > 0)
					{
						nSerial = GetBufferUp(&nPrevSerial); // m_pBufSerial[0][0] : ���� ù��° �ø���

						if (nSerial > m_nLotEndSerial || nSerial <= 0) // �뱤�ҷ� 3��° Lot End ( -1, -1, -2)
						{
							if (IsDoneDispMkInfo())
							{
								if (IsBuffer() && nSerial > 0)
									SetSerial(nSerial);
								//else
								//	SetSerial(nPrevSerial + 1, TRUE);

								nSerial = m_pBufSerial[0][1]; // m_pBufSerial[0][1] : ���� �ι�° �ø���
								if (IsBuffer() && nSerial > 0)
									SetSerial(nSerial);	//SetSerial(nSerial + 1);
														//else
														//	SetSerial(nPrevSerial + 2, TRUE);

								m_bLotEnd = TRUE;
								m_nLotEndAuto = LOT_END;
							}
						}
					}
				}
			}

			break;
		case MK_ST + (Mk2PtIdx::Shift2Mk) + 6:
			m_bMkSt = FALSE;
			::WritePrivateProfileString(_T("Last Job"), _T("MkSt"), _T("0"), PATH_WORKING_INFO);
			break;
		}
	}
}

void CManagerProcedure::Mk2PtLotDiff()
{
	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk2PtIdx::LotDiff) :
			Stop();
			//TowerLamp(RGB_YELLOW, TRUE);
			m_nMkStAuto++;
			break;
		case MK_ST + (Mk2PtIdx::LotDiff) + 1:
			//if(IDYES == DoMyMsgBox(_T("���� �ϸ��� Lot�� �ٸ��ϴ�.\r\n��� �۾��� �����Ͻðڽ��ϱ�?"), MB_YESNO))
			if (IDYES == MsgBox(_T("���� �ϸ��� Lot�� �ٸ��ϴ�.\r\n��� �۾��� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
			{
				m_bContDiffLot = TRUE;
			}
			else
			{
				m_bContDiffLot = FALSE;
				m_bLotEnd = TRUE;
				m_nLotEndAuto = LOT_END;
			}
			m_nMkStAuto++;
			break;
		case MK_ST + (Mk2PtIdx::LotDiff) + 2:
			if (IsRun())
			{
				if (m_bContDiffLot)
					m_nMkStAuto = MK_ST + (Mk2PtIdx::ChkSn);
				else
					m_nMkStAuto++;
			}
			break;
		case MK_ST + (Mk2PtIdx::LotDiff) + 3:
			m_bContDiffLot = FALSE;
			m_bLotEnd = TRUE;
			m_nLotEndAuto = LOT_END;
			break;
		}
	}
}

void CManagerProcedure::Mk2PtReject()
{
	if (!pView->m_mgrPunch)
		return;
	int a, b;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case REJECT_ST:
			Stop();
			//TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);

			if (IDYES == MsgBox(_T("��Ʈ üũ �ҷ��Դϴ�.\r\n���� ó���� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
			{
				m_bAnswer[0] = TRUE;
				m_nMkStAuto++;
			}
			else
			{
				if (IDYES == MsgBox(_T("���� ó���� ����Ͽ����ϴ�.\r\n�ҷ��� ��ŷ ó���� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
				{
					m_bAnswer[1] = TRUE;
					m_nMkStAuto++;
				}
			}

			Buzzer(FALSE, 0);
			break;
		case REJECT_ST + 1:
			if (IsRun())
			{
				if (m_bAnswer[0])
				{
					m_bAnswer[0] = FALSE;
					m_nMkStAuto++;
				}
				else if (m_bAnswer[1])
				{
					m_bAnswer[1] = FALSE;
					m_nMkStAuto = MK_ST + (Mk2PtIdx::DoMk);	// Mk ��ŷ ����
				}
				else
					m_nMkStAuto = REJECT_ST;
			}
			break;
		case REJECT_ST + 2:
			SetReject();
			m_nMkStAuto++;
			break;
		case REJECT_ST + 3:
			m_nMkStAuto++;
			break;
		case REJECT_ST + 4:
			if (IsMkDone() && IsMoveDone())
			{
				if (IsVerify() && !m_nPrevMkStAuto)
				{
					m_nPrevMkStAuto = REJECT_ST + 4;
					m_nMkStAuto = REJECT_ST + 2;		// Mk ��ŷ ����
					pView->m_mgrPunch->m_bCam = TRUE;

					pView->m_mgrPunch->m_bDoneMk[0] = FALSE;
					pView->m_mgrPunch->m_bDoneMk[1] = FALSE;

					for (a = 0; a < 2; a++)
					{
						for (b = 0; b < MAX_STRIP_NUM; b++)
						{
							pView->m_mgrPunch->m_nMkStrip[a][b] = 0;
							pView->m_mgrPunch->m_bRejectDone[a][b] = FALSE;
						}
					}
				}
				else
				{
					if (pView->m_mgrPunch->IsReview())
					{
						if (!pView->m_mgrPunch->m_bCam)
						{
							m_nPrevStepAuto = REJECT_ST + 4;
							m_nMkStAuto = REJECT_ST + 2;		// Mk ��ŷ ����
							pView->m_mgrPunch->m_bCam = TRUE;
							MsgBox(_T("�� Jog ��ư�� �̿��Ͽ� ��ŷ��ġ�� Ȯ���Ͽ� �ּ���."));
						}
						else
						{
							pView->m_mgrPunch->m_bCam = FALSE;
							m_nMkStAuto++;	// Mk ��ŷ �Ϸ�
						}
					}
					else
					{
						m_nMkStAuto++;	// Mk ��ŷ �Ϸ�
					}
				}
			}
			break;
		case REJECT_ST + 5:
			m_nMkStAuto = MK_ST + (Mk2PtIdx::DoneMk);				// Align���� �ʱ�ȭ
			break;
		}
	}
}

void CManagerProcedure::Mk2PtErrStop()
{
	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case ERROR_ST:
			Stop();
			//TowerLamp(RGB_RED, TRUE);
			if (IDYES == MsgBox(_T("��Ʈ üũ Error�Դϴ�.\r\n�ٽ� ��Ʈ üũ�� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
			{
				m_bAnswer[0] = TRUE;
				m_nMkStAuto++;
			}
			else
			{
				if (IDYES == MsgBox(_T("��Ʈ üũ�� ����Ͽ����ϴ�.\r\n�ҷ��� ��ŷ ó���� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
				{
					m_bAnswer[1] = TRUE;
					m_nMkStAuto++;
				}
				else
					m_nMkStAuto++;
			}
			break;
		case ERROR_ST + 1:
			m_nMkStAuto++;
			break;
		case ERROR_ST + 2:
			if (IsRun())
			{
				if (m_bAnswer[0])
				{
					m_bAnswer[0] = FALSE;
					m_nMkStAuto = MK_ST + (Mk2PtIdx::ChkElec); // DoElecChk
				}
				else if (m_bAnswer[1])
				{
					m_bAnswer[1] = FALSE;
					m_nMkStAuto = MK_ST + (Mk2PtIdx::DoMk);	// Mk ��ŷ ����
				}
				else
					m_nMkStAuto++;
			}
			break;
		case ERROR_ST + 3:
			m_nMkStAuto = ERROR_ST;
			break;
		}
	}
}

void CManagerProcedure::MarkingWith2PointAlign()
{
	Mk2PtReady();
	Mk2PtChkSerial();
	Mk2PtInit();
	Mk2PtAlignPt0();
	Mk2PtAlignPt1();
	Mk2PtMoveInitPos();
	Mk2PtElecChk();
	if (MODE_INNER != pDoc->GetTestMode())
		Mk2PtDoMarking();
	else
		Mk2PtShift2Mk();
	Mk2PtLotDiff();
	Mk2PtReject();
	Mk2PtErrStop();
}

void CManagerProcedure::Mk4PtReady()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST:	// PLC MK ��ȣ Ȯ��	
			if (IsRun())
			{
				SetListBuf();
				m_nMkStAuto++;
			}
			break;
		case MK_ST + 1:
			pDoc->Log(_T("PC: ��ŷ�� ��ŷ�� ON (PC�� ON, OFF)"));
			pView->MpeWrite(_T("MB440150"), 1);// ��ŷ�� ��ŷ�� ON (PC�� ON, OFF)
			m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Start) :	// 2
			if (bDualTest)
			{
				if (pDoc->m_ListBuf[1].nTot > 0) // AOI-Dn
				{
					m_nMkStAuto++;

					m_nBufDnSerial[0] = pDoc->m_ListBuf[1].Pop();
					m_nBufUpSerial[0] = m_nBufDnSerial[0];
					if (pDoc->m_ListBuf[1].nTot > 0) // AOI-Dn
					{
						m_nBufDnSerial[1] = pDoc->m_ListBuf[1].Pop();
						m_nBufUpSerial[1] = m_nBufDnSerial[1];
					}
					else
					{
						m_nBufDnSerial[1] = 0;
						m_nBufUpSerial[1] = 0;
					}
				}
				else
				{
					m_bLotEnd = TRUE;
					m_nLotEndAuto = LOT_END;
				}

				if (pDoc->WorkingInfo.LastJob.bSampleTest)
				{
					if (m_nBufUpSerial[0] == 1)
					{
						m_nLotEndSerial = _tstoi(pDoc->WorkingInfo.LastJob.sSampleTestShotNum);
						m_bLastProcFromUp = FALSE;
						m_bLastProcFromEng = FALSE;
						m_bLastProc = TRUE;
						//if (m_pDlgMenu01)
						//	m_pDlgMenu01->m_bLastProc = TRUE;
						pView->SetLastProc();
						pView->MpeWrite(_T("MB440186"), 1);			// �ܷ�ó�� AOI(��) ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141112
						pView->MpeWrite(_T("MB440181"), 1);			// �ܷ�ó��(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141031
						pDoc->Log(_T("PC: �ܷ�ó�� AOI(��) ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)"));
					}
				}
			}
			else
			{
				if (pDoc->m_ListBuf[0].nTot > 0) // AOI-Up
				{
					m_nMkStAuto++;
					m_nBufUpSerial[0] = pDoc->m_ListBuf[0].Pop();
					if (pDoc->m_ListBuf[0].nTot > 0) // AOI-Up
						m_nBufUpSerial[1] = pDoc->m_ListBuf[0].Pop();
					else
						m_nBufUpSerial[1] = 0;
				}
				else
				{
					m_bLotEnd = TRUE;
					m_nLotEndAuto = LOT_END;
				}

				if (pDoc->WorkingInfo.LastJob.bSampleTest)
				{
					if (m_nBufUpSerial[0] == 1)
					{
						m_nLotEndSerial = _tstoi(pDoc->WorkingInfo.LastJob.sSampleTestShotNum);
						m_bLastProcFromUp = FALSE;
						m_bLastProcFromEng = FALSE;
						m_bLastProc = TRUE;
						//if (m_pDlgMenu01)
						//	m_pDlgMenu01->m_bLastProc = TRUE;
						pView->SetLastProc();
						pView->MpeWrite(_T("MB440186"), 1);			// �ܷ�ó�� AOI(��) ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141112
						pView->MpeWrite(_T("MB440181"), 1);			// �ܷ�ó��(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141031
						pDoc->Log(_T("PC: �ܷ�ó�� AOI(��) ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)"));
					}
				}
			}
											break;
		case MK_ST + (Mk4PtIdx::Start) + 1:
			m_nMkStAuto++;
			break;
		}
	}
}

void CManagerProcedure::Mk4PtChkSerial()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	CString sNewLot;
	BOOL bNewModel = FALSE;
	int nSerial = 0;
	int nNewLot = 0;
	double dFdEnc;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk4PtIdx::ChkSn) :
			if (m_nBufUpSerial[0] == m_nBufUpSerial[1])
			{
				Stop();
				pView->ClrDispMsg();
				AfxMessageBox(_T("��/�� ��ŷ �ø����� �����ϴ�."));
				SetListBuf();
				m_nMkStAuto = MK_ST + (Mk4PtIdx::Start);
				break;
			}

									   if (!m_bTHREAD_DISP_DEF)
									   {
										   m_nMkStAuto++;
										   m_nStepTHREAD_DISP_DEF = 0;
										   m_bTHREAD_DISP_DEF = TRUE;		// DispDefImg() : CopyDefImg Start
										   pDoc->SetMkMenu01(_T("Signal"), _T("DispDefImg"), _T("1"));
									   }
									   break;

		case MK_ST + (Mk4PtIdx::ChkSn) + 1:
			m_nMkStAuto = MK_ST + (Mk4PtIdx::InitMk);			// InitMk()
			nSerial = m_nBufUpSerial[0];
			sNewLot = m_sNewLotUp;

			if (nSerial > 0)
			{
				if (m_bSerialDecrese)
				{
					if (m_bLastProc && m_nBufUpSerial[1] < m_nLotEndSerial)
					{
						nSerial = m_nBufUpSerial[0]; // Test
					}
				}
				else
				{
					if (m_bLastProc && m_nBufUpSerial[1] > m_nLotEndSerial)
					{
						nSerial = m_nBufUpSerial[0]; // Test
					}
				}

				bNewModel = GetAoiUpInfo(nSerial, &nNewLot, TRUE);
				if (bDualTest)
				{
					bNewModel = GetAoiDnInfo(nSerial, &nNewLot, TRUE);

					if (!IsSameUpDnLot() && !m_bContDiffLot)
					{
						//sNewLot.Empty();
						m_nMkStAuto = MK_ST + (Mk4PtIdx::LotDiff);
						break;
					}
				}

				if (bNewModel)	// AOI ����(AoiCurrentInfoPath) -> AOI Feeding Offset
				{
					;
				}
				if (nNewLot)
				{
					// Lot Change.
					ChgLot();
#ifdef USE_MPE
					dFdEnc = (double)pDoc->m_pMpeData[0][0];	// ��ŷ�� Feeding ���ڴ� ��(���� mm )
					if ((pDoc->WorkingInfo.LastJob.bLotSep || pDoc->m_bDoneChgLot) && (dFdEnc + _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen)*2.0) > _tstof(pDoc->WorkingInfo.LastJob.sLotSepLen)*1000.0)
					{
						pDoc->m_bDoneChgLot = TRUE;
						pView->SetPathAtBuf();
					}
#endif
				}
			}
			else
			{
				Stop();
				MsgBox(_T("����(��) Serial�� �����ʽ��ϴ�."));
				//TowerLamp(RGB_YELLOW, TRUE);
			}
			//sNewLot.Empty();
			break;
		}
	}
}

void CManagerProcedure::Mk4PtInit()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk4PtIdx::InitMk) :
			if (pView->m_mgrPunch->InitMk())	// �ø����� ���� �Ǵ� ���� : TRUE �ƴϰ� ������ : FALSE
			{
				if (m_bSerialDecrese)
				{
					if ((m_nBufUpSerial[0] <= m_nLotEndSerial
						|| m_nBufUpSerial[1] <= m_nLotEndSerial)
						&& m_nLotEndSerial > 0)
					{
						pView->MpeWrite(_T("MB440171"), 1); // ��ŷ�� �۾��Ϸ�.(PC�� On, PLC�� Ȯ�� �� Off) - 20160718
						pDoc->Log(_T("PC: ��ŷ�� �۾��Ϸ�.(PC�� On, PLC�� Ȯ�� �� Off)"));
					}
				}
				else
				{
					if ((m_nBufUpSerial[0] >= m_nLotEndSerial
						|| m_nBufUpSerial[1] >= m_nLotEndSerial)
						&& m_nLotEndSerial > 0)
					{
						pView->MpeWrite(_T("MB440171"), 1); // ��ŷ�� �۾��Ϸ�.(PC�� On, PLC�� Ȯ�� �� Off) - 20160718
						pDoc->Log(_T("PC: ��ŷ�� �۾��Ϸ�.(PC�� On, PLC�� Ȯ�� �� Off)"));
					}
				}
			}
			else
			{
				Stop();
				MsgBox(_T("Serial ���� �����ʽ��ϴ�."));
				//TowerLamp(RGB_YELLOW, TRUE);
			}
										m_nMkStAuto++;
										break;

		case MK_ST + (Mk4PtIdx::InitMk) + 1:
			if (IsRun())
				m_nMkStAuto++;
			break;
		}
	}
}

void CManagerProcedure::Mk4PtAlignPt0()
{
	if (!pView->m_mgrPunch)
		return;
	if (!IsRun())
		return;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk4PtIdx::Move0Cam1) :	// Move - Cam1 - Pt0
			if (bDualTest)
			{
				if (m_bSerialDecrese)
				{
					if (m_bLastProc && m_nBufDnSerial[0] - 1 < m_nLotEndSerial)	// AOI�ϸ� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrPunch->m_bDoMk[1] = FALSE;
						pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(0))
							m_nMkStAuto++;
					}
				}
				else
				{
					if (m_bLastProc && m_nBufDnSerial[0] + 1 > m_nLotEndSerial)	// AOI�ϸ� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrPunch->m_bDoMk[1] = FALSE;
						pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(0))
							m_nMkStAuto++;
					}
				}
			}
			else
			{
				if (m_bSerialDecrese)
				{
					if (m_bLastProc && m_nBufUpSerial[0] - 1 < m_nLotEndSerial)	// AOI��� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrPunch->m_bDoMk[1] = FALSE;
						pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(0))
							m_nMkStAuto++;
					}
				}
				else
				{
					if (m_bLastProc && m_nBufUpSerial[0] + 1 > m_nLotEndSerial)	// AOI��� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrPunch->m_bDoMk[1] = FALSE;
						pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(0))
							m_nMkStAuto++;
					}
				}
			}
												break;
		case MK_ST + (Mk4PtIdx::Move0Cam1) + 1:
			if (IsRun())
				m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Move0Cam0) :	// Move - Cam0 - Pt0
			if (MoveAlign0(0))
				m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Move0Cam0) + 1:
			if (IsRun())
				m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Move0Cam0) + 2:
			if (IsMoveDone())
			{
				Sleep(100);
				m_nMkStAuto++;
			}
			break;
		case MK_ST + (Mk4PtIdx::Align1_0) :	// 4PtAlign - Cam1 - Pt0
			if (!m_bSkipAlign[1][0])
			{
				if (FourPointAlign1(0))
					m_bFailAlign[1][0] = FALSE;
				else
					m_bFailAlign[1][0] = TRUE;
			}
											m_nMkStAuto++;
											break;
		case MK_ST + (Mk4PtIdx::Align0_0) :	// 4PtAlign - Cam0 - Pt0
			if (!m_bSkipAlign[0][0])
			{
				if (FourPointAlign0(0))
					m_bFailAlign[0][0] = FALSE;
				else
					m_bFailAlign[0][0] = TRUE;
			}
											m_nMkStAuto++;
											break;
		case MK_ST + (Mk4PtIdx::Align0_0) + 1:
			if (m_bFailAlign[0][0])
			{
				Buzzer(TRUE, 0);

				if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ٽ� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);

					if (IDYES == MsgBox(_T("ī�޶�(��)�� �˻��ǳ� ������ ���� ���Ͻðڽ��ϱ�?"), 0, MB_YESNO))
					{
						m_bReAlign[0][0] = FALSE;
						m_bSkipAlign[0][0] = TRUE;
						m_bSkipAlign[0][1] = TRUE;
						m_bSkipAlign[0][2] = TRUE;
						m_bSkipAlign[0][3] = TRUE;

						if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ҷ���ŷ �Ͻðڽ��ϱ�?"), 0, MB_YESNO))
						{
							pView->m_mgrPunch->m_bDoMk[0] = FALSE;
							pView->m_mgrPunch->m_bDoneMk[0] = TRUE;
						}
						else
						{
							pView->m_mgrPunch->m_bDoMk[0] = TRUE;
							pView->m_mgrPunch->m_bDoneMk[0] = FALSE;
						}
					}
					else
					{
						m_bReAlign[0][0] = TRUE;
						m_bSkipAlign[0][0] = FALSE;
						m_bSkipAlign[0][1] = FALSE;
						m_bSkipAlign[0][2] = FALSE;
						m_bSkipAlign[0][3] = FALSE;

						m_nMkStAuto = MK_ST + (Mk4PtIdx::Move0Cam0); // FourPointAlign0(0) ���� ����. - ī�޶� ������
						Stop();
						//TowerLamp(RGB_YELLOW, TRUE);
					}
				}
				else
				{
					Buzzer(FALSE, 0);

					m_bReAlign[0][0] = TRUE;
					m_bSkipAlign[0][0] = FALSE;
					m_bSkipAlign[0][1] = FALSE;

					m_nMkStAuto = MK_ST + (Mk4PtIdx::Move0Cam0); // FourPointAlign0(0) ���� ����. - ī�޶� ������
					Stop();
					//TowerLamp(RGB_YELLOW, TRUE);
				}
			}
			if (m_bFailAlign[1][0])
			{
				Buzzer(TRUE, 0);

				if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ٽ� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);

					if (IDYES == MsgBox(_T("ī�޶�(��)�� �˻��ǳ� ������ ���� ���Ͻðڽ��ϱ�?"), 0, MB_YESNO))
					{
						m_bReAlign[1][0] = FALSE;
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;

						if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ҷ���ŷ �Ͻðڽ��ϱ�?"), 0, MB_YESNO))
						{
							pView->m_mgrPunch->m_bDoMk[1] = FALSE;
							pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						}
						else
						{
							pView->m_mgrPunch->m_bDoMk[1] = TRUE;
							pView->m_mgrPunch->m_bDoneMk[1] = FALSE;
						}
					}
					else
					{
						m_bReAlign[1][0] = TRUE;
						m_bSkipAlign[1][0] = FALSE;
						m_bSkipAlign[1][1] = FALSE;
						m_bSkipAlign[1][2] = FALSE;
						m_bSkipAlign[1][3] = FALSE;

						m_nMkStAuto = MK_ST + (Mk4PtIdx::Move0Cam1); // FourPointAlign1(0) ���� ����. - ī�޶� ������
						Stop();
						//TowerLamp(RGB_YELLOW, TRUE);
					}
				}
				else
				{
					Buzzer(FALSE, 0);

					m_bReAlign[1][0] = TRUE;
					m_bSkipAlign[1][0] = FALSE;
					m_bSkipAlign[1][1] = FALSE;
					m_bSkipAlign[1][2] = FALSE;
					m_bSkipAlign[1][3] = FALSE;

					m_nMkStAuto = MK_ST + (Mk4PtIdx::Move0Cam1); // FourPointAlign1(0) ���� ����. - ī�޶� ������
					Stop();
					//TowerLamp(RGB_YELLOW, TRUE);
				}
			}

			if (m_bFailAlign[0][0] || m_bFailAlign[1][0])
			{
				if (!m_bReAlign[0][0] && !m_bReAlign[1][0])
				{
					if (pView->m_mgrPunch->m_bDoMk[0] || pView->m_mgrPunch->m_bDoMk[1])
						m_nMkStAuto++; //m_nMkStAuto = MK_ST + 27; // MoveInitPos0()
					else
					{
						if (!IsInitPos0())
							MoveInitPos0();
						if (!IsInitPos1())
							MoveInitPos1();

						m_nMkStAuto = MK_ST + (Mk4PtIdx::DoneMk); // // �ҷ��̹��� Display, Align���� �ʱ�ȭ (Skip 65 : Mk())
					}
				}
				else
				{
					//m_nMkStAuto = MK_ST + (Mk4PtIdx::Align1_0); // TwoPointAlign1(0) ���� ����. - ī�޶� ������
					m_nMkStAuto = MK_ST + (Mk4PtIdx::Move0Cam1); // TwoPointAlign1(0) ���� ����. - ī�޶� ������
				}
			}
			else
				m_nMkStAuto++;

			break;
		case MK_ST + (Mk4PtIdx::Align0_0) + 2:
			if (IsRun())
				m_nMkStAuto++;
			break;
		}
	}
}

void CManagerProcedure::Mk4PtAlignPt1()
{
	if (!pView->m_mgrPunch)
		return;
	if (!IsRun())
		return;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk4PtIdx::Move1Cam1) :
			if (bDualTest)
			{
				if (m_bSerialDecrese)
				{
					if (m_bLastProc && m_nBufDnSerial[0] - 1 < m_nLotEndSerial)	// AOI�ϸ� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrPunch->m_bDoMk[1] = FALSE;
						pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(1))	// Move - Cam1 - Pt1
							m_nMkStAuto++;
					}
				}
				else
				{
					if (m_bLastProc && m_nBufDnSerial[0] + 1 > m_nLotEndSerial)	// AOI�ϸ� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrPunch->m_bDoMk[1] = FALSE;
						pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(1))	// Move - Cam1 - Pt1
							m_nMkStAuto++;
					}
				}
			}
			else
			{
				if (m_bSerialDecrese)
				{
					if (m_bLastProc && m_nBufUpSerial[0] - 1 < m_nLotEndSerial)	// AOI��� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrPunch->m_bDoMk[1] = FALSE;
						pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(1))	// Move - Cam1 - Pt1
							m_nMkStAuto++;
					}
				}
				else
				{
					if (m_bLastProc && m_nBufUpSerial[0] + 1 > m_nLotEndSerial)	// AOI��� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrPunch->m_bDoMk[1] = FALSE;
						pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(1))	// Move - Cam1 - Pt1
							m_nMkStAuto++;
					}
				}
			}
										   break;
		case MK_ST + (Mk4PtIdx::Move1Cam1) + 1:
			if (IsRun())
				m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Move1Cam0) :
			if (MoveAlign0(1))	// Move - Cam0 - Pt1
				m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Move1Cam0) + 1:
			if (IsRun())
				m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Move1Cam0) + 2:
			if (IsMoveDone())
			{
				Sleep(100);
				m_nMkStAuto++;
			}
			break;
		case MK_ST + (Mk4PtIdx::Align1_1) :	// 4PtAlign - Cam1 - Pt1
			if (!m_bFailAlign[1][0])
			{
				if (!m_bSkipAlign[1][1])
				{
					if (!FourPointAlign1(1))
						m_bFailAlign[1][1] = TRUE;
					else
						m_bFailAlign[1][1] = FALSE;
				}
				else
					m_bFailAlign[1][1] = FALSE;
			}
			else
				m_bFailAlign[1][1] = FALSE;

			m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Align0_1) :	// 4PtAlign - Cam0 - Pt1
			if (!m_bFailAlign[0][0])
			{
				if (!m_bSkipAlign[0][1])
				{
					if (!FourPointAlign0(1))
						m_bFailAlign[0][1] = TRUE;
					else
						m_bFailAlign[0][1] = FALSE;
				}
				else
					m_bFailAlign[0][1] = FALSE;
			}
			else
				m_bFailAlign[0][1] = FALSE;

			m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Align0_1) + 1:
			if (m_bFailAlign[0][1])
			{
				Buzzer(TRUE, 0);

				if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ٽ� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);

					if (IDYES == MsgBox(_T("ī�޶�(��)�� �˻��ǳ� ������ ���� ���Ͻðڽ��ϱ�?"), 0, MB_YESNO))
					{
						m_bReAlign[0][1] = FALSE;
						m_bSkipAlign[0][1] = TRUE;

						if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ҷ���ŷ �Ͻðڽ��ϱ�?"), 0, MB_YESNO))
						{
							pView->m_mgrPunch->m_bDoMk[0] = FALSE;
							pView->m_mgrPunch->m_bDoneMk[0] = TRUE;
						}
						else
						{
							pView->m_mgrPunch->m_bDoMk[0] = TRUE;
							pView->m_mgrPunch->m_bDoneMk[0] = FALSE;
						}
					}
					else
					{
						m_bReAlign[0][1] = TRUE;
						m_bSkipAlign[0][1] = FALSE;

						m_nMkStAuto = MK_ST + (Mk4PtIdx::Move1Cam0); // FourPointAlign0(1) ���� ����. - ī�޶� ������
						Stop();
						//TowerLamp(RGB_YELLOW, TRUE);
					}
				}
				else
				{
					Buzzer(FALSE, 0);

					m_bReAlign[0][1] = TRUE;
					m_bSkipAlign[0][1] = FALSE;

					m_nMkStAuto = MK_ST + (Mk4PtIdx::Move1Cam0); // FourPointAlign1(1) ���� ����. - ī�޶� ������
					Stop();
					//TowerLamp(RGB_YELLOW, TRUE);
				}
			}
			if (m_bFailAlign[1][1])
			{
				Buzzer(TRUE, 0);

				if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ٽ� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);

					if (IDYES == MsgBox(_T("ī�޶�(��)�� �˻��ǳ� ������ ���� ���Ͻðڽ��ϱ�?"), 0, MB_YESNO))
					{
						m_bReAlign[1][1] = FALSE;
						m_bSkipAlign[1][1] = TRUE;

						if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ҷ���ŷ �Ͻðڽ��ϱ�?"), 0, MB_YESNO))
						{
							pView->m_mgrPunch->m_bDoMk[1] = FALSE;
							pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						}
						else
						{
							pView->m_mgrPunch->m_bDoMk[1] = TRUE;
							pView->m_mgrPunch->m_bDoneMk[1] = FALSE;
						}
					}
					else
					{
						m_bReAlign[1][1] = TRUE;
						m_bSkipAlign[1][1] = FALSE;

						m_nMkStAuto = MK_ST + (Mk4PtIdx::Move1Cam1); // FourPointAlign1(1) ���� ����. - ī�޶� ������
						Stop();
						//TowerLamp(RGB_YELLOW, TRUE);
					}
				}
				else
				{
					Buzzer(FALSE, 0);

					m_bReAlign[1][1] = TRUE;
					m_bSkipAlign[1][1] = FALSE;

					m_nMkStAuto = MK_ST + (Mk4PtIdx::Move1Cam1); // FourPointAlign1(1) ���� ����. - ī�޶� ������
					Stop();
					//TowerLamp(RGB_YELLOW, TRUE);
				}
			}

			if (m_bFailAlign[0][1] || m_bFailAlign[1][1])
			{
				if (!m_bReAlign[0][1] && !m_bReAlign[1][1])
				{
					if (pView->m_mgrPunch->m_bDoMk[0] || pView->m_mgrPunch->m_bDoMk[1])
						m_nMkStAuto++;//m_nMkStAuto = MK_ST + 29;  // MoveInitPos0()
					else
					{
						if (!IsInitPos0())
							MoveInitPos0();
						if (!IsInitPos1())
							MoveInitPos1();

						m_nMkStAuto = MK_ST + (Mk4PtIdx::DoneMk); // // �ҷ��̹��� Display, Align���� �ʱ�ȭ (Skip 65 : Mk())
					}
				}
				else
					m_nMkStAuto = MK_ST + (Mk4PtIdx::Move1Cam1); // FourPointAlign1(1) ���� ����. - ī�޶� ������
			}
			else
				m_nMkStAuto++;

			break;
		}
	}
}

void CManagerProcedure::Mk4PtAlignPt2()
{
	if (!pView->m_mgrPunch)
		return;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk4PtIdx::Move2Cam1) :
			if (bDualTest)
			{
				if (m_bSerialDecrese)
				{
					if (m_bLastProc && m_nBufDnSerial[0] - 1 < m_nLotEndSerial)	// AOI�ϸ� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrPunch->m_bDoMk[1] = FALSE;
						pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(2))	// Move - Cam1 - Pt2
							m_nMkStAuto++;
					}
				}
				else
				{
					if (m_bLastProc && m_nBufDnSerial[0] + 1 > m_nLotEndSerial)	// AOI�ϸ� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrPunch->m_bDoMk[1] = FALSE;
						pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(2))	// Move - Cam1 - Pt2
							m_nMkStAuto++;
					}
				}
			}
			else
			{
				if (m_bSerialDecrese)
				{
					if (m_bLastProc && m_nBufUpSerial[0] - 1 < m_nLotEndSerial)	// AOI��� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrPunch->m_bDoMk[1] = FALSE;
						pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(2))	// Move - Cam1 - Pt2
							m_nMkStAuto++;
					}
				}
				else
				{
					if (m_bLastProc && m_nBufUpSerial[0] + 1 > m_nLotEndSerial)	// AOI��� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrPunch->m_bDoMk[1] = FALSE;
						pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(2))	// Move - Cam1 - Pt2
							m_nMkStAuto++;
					}
				}
			}
										   break;
		case MK_ST + (Mk4PtIdx::Move2Cam1) + 1:
			if (IsRun())
				m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Move2Cam0) :
			if (MoveAlign0(2))	// Move - Cam0 - Pt2
				m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Move2Cam0) + 1:
			if (IsRun())
				m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Move2Cam0) + 2:
			if (IsMoveDone())
			{
				Sleep(100);
				m_nMkStAuto++;
			}
			break;
		case MK_ST + (Mk4PtIdx::Align1_2) :	// 4PtAlign - Cam1 - Pt2
			if (!m_bFailAlign[1][0])
			{
				if (!m_bSkipAlign[1][1])
				{
					if (!m_bSkipAlign[1][2])
					{
						if (!FourPointAlign1(2))
							m_bFailAlign[1][2] = TRUE;
						else
							m_bFailAlign[1][2] = FALSE;
					}
					else
						m_bFailAlign[1][2] = FALSE;
				}
				else
					m_bFailAlign[1][2] = FALSE;
			}
			else
				m_bFailAlign[1][2] = FALSE;

			m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Align0_2) :	// 4PtAlign - Cam0 - Pt2
			if (!m_bFailAlign[0][0])
			{
				if (!m_bSkipAlign[0][1])
				{
					if (!m_bSkipAlign[0][2])
					{
						if (!FourPointAlign0(2))
							m_bFailAlign[0][2] = TRUE;
						else
							m_bFailAlign[0][2] = FALSE;
					}
					else
						m_bFailAlign[0][2] = FALSE;
				}
				else
					m_bFailAlign[0][2] = FALSE;
			}
			else
				m_bFailAlign[0][2] = FALSE;

			m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Align0_2) + 1:
			if (m_bFailAlign[0][2])
			{
				Buzzer(TRUE, 0);

				if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ٽ� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);

					if (IDYES == MsgBox(_T("ī�޶�(��)�� �˻��ǳ� ������ ���� ���Ͻðڽ��ϱ�?"), 0, MB_YESNO))
					{
						m_bReAlign[0][2] = FALSE;
						m_bSkipAlign[0][2] = TRUE;

						if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ҷ���ŷ �Ͻðڽ��ϱ�?"), 0, MB_YESNO))
						{
							pView->m_mgrPunch->m_bDoMk[0] = FALSE;
							pView->m_mgrPunch->m_bDoneMk[0] = TRUE;
						}
						else
						{
							pView->m_mgrPunch->m_bDoMk[0] = TRUE;
							pView->m_mgrPunch->m_bDoneMk[0] = FALSE;
						}
					}
					else
					{
						m_bReAlign[0][2] = TRUE;
						m_bSkipAlign[0][2] = FALSE;
						m_nMkStAuto = MK_ST + (Mk4PtIdx::Move2Cam0); // FourPointAlign0(2) ���� ����. - ī�޶� ������
						Stop();
						//TowerLamp(RGB_YELLOW, TRUE);
					}
				}
				else
				{
					Buzzer(FALSE, 0);

					m_bReAlign[0][2] = TRUE;
					m_bSkipAlign[0][2] = FALSE;
					m_nMkStAuto = MK_ST + (Mk4PtIdx::Move2Cam0); // FourPointAlign0(2) ���� ����. - ī�޶� ������
					Stop();
					//TowerLamp(RGB_YELLOW, TRUE);
				}
			}
			if (m_bFailAlign[1][2])
			{
				Buzzer(TRUE, 0);

				if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ٽ� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);

					if (IDYES == MsgBox(_T("ī�޶�(��)�� �˻��ǳ� ������ ���� ���Ͻðڽ��ϱ�?"), 0, MB_YESNO))
					{
						m_bReAlign[1][2] = FALSE;
						m_bSkipAlign[1][2] = TRUE;

						if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ҷ���ŷ �Ͻðڽ��ϱ�?"), 0, MB_YESNO))
						{
							pView->m_mgrPunch->m_bDoMk[1] = FALSE;
							pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						}
						else
						{
							pView->m_mgrPunch->m_bDoMk[1] = TRUE;
							pView->m_mgrPunch->m_bDoneMk[1] = FALSE;
						}
					}
					else
					{
						m_bReAlign[1][2] = TRUE;
						m_bSkipAlign[1][2] = FALSE;

						m_nMkStAuto = MK_ST + (Mk4PtIdx::Move2Cam1); // FourPointAlign1(2) ���� ����. - ī�޶� ������
						Stop();
						//TowerLamp(RGB_YELLOW, TRUE);
					}
				}
				else
				{
					Buzzer(FALSE, 0);

					m_bReAlign[1][2] = TRUE;
					m_bSkipAlign[1][2] = FALSE;

					m_nMkStAuto = MK_ST + (Mk4PtIdx::Move2Cam1); // FourPointAlign1(2) ���� ����. - ī�޶� ������
					Stop();
					//TowerLamp(RGB_YELLOW, TRUE);
				}
			}

			if (m_bFailAlign[0][2] || m_bFailAlign[1][2])
			{
				if (!m_bReAlign[0][2] && !m_bReAlign[1][2])
				{
					if (pView->m_mgrPunch->m_bDoMk[0] || pView->m_mgrPunch->m_bDoMk[1])
						m_nMkStAuto++;//m_nMkStAuto = MK_ST + 29;  // MoveInitPos0()
					else
					{
						if (!IsInitPos0())
							MoveInitPos0();
						if (!IsInitPos1())
							MoveInitPos1();

						m_nMkStAuto = MK_ST + (Mk4PtIdx::DoneMk); // // �ҷ��̹��� Display, Align���� �ʱ�ȭ (Skip 65 : Mk())
					}
				}
				else
					m_nMkStAuto = MK_ST + (Mk4PtIdx::Move2Cam1); // FourPointAlign1(2) ���� ����. - ī�޶� ������
			}
			else
				m_nMkStAuto++;

			break;
		}
	}
}

void CManagerProcedure::Mk4PtAlignPt3()
{
	if (!pView->m_mgrPunch)
		return;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk4PtIdx::Move3Cam1) :
			if (bDualTest)
			{
				if (m_bSerialDecrese)
				{
					if (m_bLastProc && m_nBufDnSerial[0] - 1 < m_nLotEndSerial)	// AOI�ϸ� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrPunch->m_bDoMk[1] = FALSE;
						pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(3))	// Move - Cam1 - Pt3
							m_nMkStAuto++;
					}
				}
				else
				{
					if (m_bLastProc && m_nBufDnSerial[0] + 1 > m_nLotEndSerial)	// AOI�ϸ� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrPunch->m_bDoMk[1] = FALSE;
						pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(3))	// Move - Cam1 - Pt3
							m_nMkStAuto++;
					}
				}
			}
			else
			{
				if (m_bSerialDecrese)
				{
					if (m_bLastProc && m_nBufUpSerial[0] - 1 < m_nLotEndSerial)	// AOI��� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrPunch->m_bDoMk[1] = FALSE;
						pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(3))	// Move - Cam1 - Pt3
							m_nMkStAuto++;
					}
				}
				else
				{
					if (m_bLastProc && m_nBufUpSerial[0] + 1 > m_nLotEndSerial)	// AOI��� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrPunch->m_bDoMk[1] = FALSE;
						pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(3))	// Move - Cam1 - Pt3
							m_nMkStAuto++;
					}
				}
			}
										   break;
		case MK_ST + (Mk4PtIdx::Move3Cam1) + 1:
			if (IsRun())
				m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Move3Cam0) :
			if (MoveAlign0(3))	// Move - Cam0 - Pt3
				m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Move3Cam0) + 1:
			if (IsRun())
				m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Move3Cam0) + 2:
			if (IsMoveDone())
			{
				Sleep(100);
				m_nMkStAuto++;
			}
			break;
		case MK_ST + (Mk4PtIdx::Align1_3) :	// 4PtAlign - Cam1 - Pt3
			if (!m_bFailAlign[1][0])
			{
				if (!m_bSkipAlign[1][1])
				{
					if (!m_bSkipAlign[1][2])
					{
						if (!m_bSkipAlign[1][3])
						{
							if (!FourPointAlign1(3))
								m_bFailAlign[1][3] = TRUE;
							else
								m_bFailAlign[1][3] = FALSE;
						}
						else
							m_bFailAlign[1][3] = FALSE;
					}
					else
						m_bFailAlign[1][3] = FALSE;
				}
				else
					m_bFailAlign[1][3] = FALSE;
			}
			else
				m_bFailAlign[1][3] = FALSE;

			m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Align0_3) :	// 4PtAlign - Cam0 - Pt3
			if (!m_bFailAlign[0][0])
			{
				if (!m_bSkipAlign[0][1])
				{
					if (!m_bSkipAlign[0][2])
					{
						if (!m_bSkipAlign[0][3])
						{
							if (!FourPointAlign0(3))
								m_bFailAlign[0][3] = TRUE;
							else
								m_bFailAlign[0][3] = FALSE;
						}
						else
							m_bFailAlign[0][3] = FALSE;
					}
					else
						m_bFailAlign[0][3] = FALSE;
				}
				else
					m_bFailAlign[0][3] = FALSE;
			}
			else
				m_bFailAlign[0][3] = FALSE;

			m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Align0_3) + 1:
			if (m_bFailAlign[0][3])
			{
				Buzzer(TRUE, 0);

				if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ٽ� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);

					if (IDYES == MsgBox(_T("ī�޶�(��)�� �˻��ǳ� ������ ���� ���Ͻðڽ��ϱ�?"), 0, MB_YESNO))
					{
						m_bReAlign[0][3] = FALSE;
						m_bSkipAlign[0][3] = TRUE;

						if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ҷ���ŷ �Ͻðڽ��ϱ�?"), 0, MB_YESNO))
						{
							pView->m_mgrPunch->m_bDoMk[0] = FALSE;
							pView->m_mgrPunch->m_bDoneMk[0] = TRUE;
						}
						else
						{
							pView->m_mgrPunch->m_bDoMk[0] = TRUE;
							pView->m_mgrPunch->m_bDoneMk[0] = FALSE;
						}
					}
					else
					{
						m_bReAlign[0][3] = TRUE;
						m_bSkipAlign[0][3] = FALSE;

						m_nMkStAuto = MK_ST + (Mk4PtIdx::Move3Cam0); // FourPointAlign0(3) ���� ����. - ī�޶� ������
						Stop();
						//TowerLamp(RGB_YELLOW, TRUE);
					}
				}
				else
				{
					Buzzer(FALSE, 0);

					m_bReAlign[0][3] = TRUE;
					m_bSkipAlign[0][3] = FALSE;

					m_nMkStAuto = MK_ST + (Mk4PtIdx::Move3Cam0); // FourPointAlign0(3) ���� ����. - ī�޶� ������
					Stop();
					//TowerLamp(RGB_YELLOW, TRUE);
				}
			}
			if (m_bFailAlign[1][3])
			{
				Buzzer(TRUE, 0);

				if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ٽ� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);

					if (IDYES == MsgBox(_T("ī�޶�(��)�� �˻��ǳ� ������ ���� ���Ͻðڽ��ϱ�?"), 0, MB_YESNO))
					{
						m_bReAlign[1][3] = FALSE;
						m_bSkipAlign[1][3] = TRUE;

						if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ҷ���ŷ �Ͻðڽ��ϱ�?"), 0, MB_YESNO))
						{
							pView->m_mgrPunch->m_bDoMk[1] = FALSE;
							pView->m_mgrPunch->m_bDoneMk[1] = TRUE;
						}
						else
						{
							pView->m_mgrPunch->m_bDoMk[1] = TRUE;
							pView->m_mgrPunch->m_bDoneMk[1] = FALSE;
						}
					}
					else
					{
						m_bReAlign[1][3] = TRUE;
						m_bSkipAlign[1][3] = FALSE;

						m_nMkStAuto = MK_ST + (Mk4PtIdx::Move3Cam1); // FourPointAlign1(3) ���� ����. - ī�޶� ������
						Stop();
						//TowerLamp(RGB_YELLOW, TRUE);
					}
				}
				else
				{
					Buzzer(FALSE, 0);

					m_bReAlign[1][3] = TRUE;
					m_bSkipAlign[1][3] = FALSE;

					m_nMkStAuto = MK_ST + (Mk4PtIdx::Move3Cam1); // FourPointAlign1(3) ���� ����. - ī�޶� ������
					Stop();
					//TowerLamp(RGB_YELLOW, TRUE);
				}
			}

			if (m_bFailAlign[0][3] || m_bFailAlign[1][3])
			{
				if (!m_bReAlign[0][3] && !m_bReAlign[1][3])
				{
					if (pView->m_mgrPunch->m_bDoMk[0] || pView->m_mgrPunch->m_bDoMk[1])
						m_nMkStAuto++;//m_nMkStAuto = MK_ST + 29;  // MoveInitPos0()
					else
					{
						if (!IsInitPos0())
							MoveInitPos0();
						if (!IsInitPos1())
							MoveInitPos1();

						m_nMkStAuto = MK_ST + (Mk4PtIdx::DoneMk); // // �ҷ��̹��� Display, Align���� �ʱ�ȭ (Skip 65 : Mk())
					}
				}
				else
					m_nMkStAuto = MK_ST + (Mk4PtIdx::Move3Cam1); // FourPointAlign1(3) ���� ����. - ī�޶� ������
			}
			else
				m_nMkStAuto++;

			break;
		}
	}
}

void CManagerProcedure::Mk4PtMoveInitPos()
{
	if (!pView->m_mgrPunch)
		return;
	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk4PtIdx::MoveInitPt) :
			MoveInitPos0();
			m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::MoveInitPt) + 1:
			if (pView->m_mgrPunch->m_bDoMk[1])
				MoveInitPos1();
			else
				MoveMkEdPos1();
			m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::MoveInitPt) + 2:
			if (IsMoveDone())
				m_nMkStAuto++;
			break;
		}
	}
}

void CManagerProcedure::Mk4PtElecChk()
{
	CString sRst;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk4PtIdx::ChkElec) : // DoElecChk
			if (DoElecChk(sRst))
			{
				if (pDoc->WorkingInfo.Probing[0].bUse)
				{
					if (sRst == _T("Open"))
					{
						if (pDoc->WorkingInfo.Probing[0].bStopOnOpen)
							m_nMkStAuto = REJECT_ST;
						else
							m_nMkStAuto++;
					}
					else if (sRst == _T("Error"))
					{
						m_nMkStAuto = ERROR_ST;
					}
					else
					{
						m_nMkStAuto++;
					}
				}
				else
					m_nMkStAuto++;
			}
			break;

		case MK_ST + (Mk4PtIdx::ChkElec) + 1:
			if (ChkLightErr())
			{
				m_bChkLightErr = FALSE;
				m_nMkStAuto++;
			}
			else
				m_nMkStAuto = MK_ST + (Mk4PtIdx::DoMk);	// Mk ��ŷ ����
			break;

		case MK_ST + (Mk4PtIdx::ChkElec) + 2:
			if (IsRun())
			{
				//if (m_pMotion->IsEnable(MS_X0) && m_pMotion->IsEnable(MS_Y0) &&
				//	m_pMotion->IsEnable(MS_X1) && m_pMotion->IsEnable(MS_Y1))
				if(pView->m_mgrPunch->IsMotionEnable())
				{
					if (MODE_INNER != pDoc->GetTestMode())
						m_nMkStAuto = MK_ST + (Mk4PtIdx::DoMk); 	// Mk ��ŷ ����
					else
						m_nMkStAuto = MK_ST + (Mk4PtIdx::Shift2Mk);
				}
				else
				{
					Stop();
					MsgBox(_T("��ŷ�� ����� ��Ȱ��ȭ �Ǿ����ϴ�."));
					//TowerLamp(RGB_RED, TRUE);
				}
			}
			else
			{
				if (!m_bChkLightErr)
				{
					m_bChkLightErr = TRUE;
					MsgBox(_T("�뱤�ҷ� ���� - ������ Ȯ���ϼ���.\r\n��������Ϸ��� ��������ġ�� ��������."));
				}
			}
			break;
		}
	}
}

void CManagerProcedure::Mk4PtDoMarking()
{
	if (!pView->m_mgrPunch)
		return;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	CString sRst, sMsg;
	int a, b, nSerial, nPrevSerial;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk4PtIdx::DoMk) :				// Mk ��ŷ ����
			if (pDoc->GetTestMode() == MODE_OUTER)
				pView->m_mgrPunch->SetMkIts(TRUE);						// ITS ��ŷ ����
			else
				pView->m_mgrPunch->SetMk(TRUE);						// Mk ��ŷ ����

			m_nMkStAuto++;
			break;

		case MK_ST + (Mk4PtIdx::DoMk) + 1:
			Sleep(100);
			m_nMkStAuto++;
			break;

		case MK_ST + (Mk4PtIdx::Verify) :
			if (IsMkDone() && IsMoveDone())
			{
				if (IsVerify() && !m_nPrevMkStAuto)
				{
					m_nPrevMkStAuto = MK_ST + (Mk4PtIdx::Verify);
					m_nMkStAuto = MK_ST + (Mk4PtIdx::DoMk);		// Mk ��ŷ ����
					pView->m_mgrPunch->m_bCam = TRUE;
				}
				else
				{
					if (pView->m_mgrPunch->IsReview())
					{
						if (!pView->m_mgrPunch->m_bCam)
						{
							m_nPrevStepAuto = MK_ST + (Mk4PtIdx::Verify);
							m_nMkStAuto = MK_ST + (Mk4PtIdx::DoMk);		// Mk ��ŷ ����
							pView->m_mgrPunch->m_bCam = TRUE;
							MsgBox(_T("�� Jog ��ư�� �̿��Ͽ� ��ŷ��ġ�� Ȯ���Ͽ� �ּ���."));
						}
						else
						{
							pView->m_mgrPunch->m_bCam = FALSE;
							m_nMkStAuto++;	// Mk ��ŷ �Ϸ�
						}
					}
					else
					{
						m_nMkStAuto++;	// Mk ��ŷ �Ϸ�
					}
				}
			}
			else if (pView->m_mgrPunch->IsReMk())
			{
				m_nPrevMkStAuto = MK_ST + (Mk4PtIdx::Verify);
				m_nMkStAuto = MK_ST + (Mk4PtIdx::DoMk);		// Mk �����
			}
			else
			{
				sMsg = _T("");
				sMsg += m_sDispSts[0];
				sMsg += _T(",");
				sMsg += m_sDispSts[1];
				//DispStsBar(sMsg, 0);
			}
										break;

		case MK_ST + (Mk4PtIdx::DoneMk) :	 // Align���� �ʱ�ȭ
			m_bReAlign[0][0] = FALSE; // [nCam][nPos] 
			m_bReAlign[0][1] = FALSE; // [nCam][nPos] 
			m_bReAlign[1][0] = FALSE; // [nCam][nPos] 
			m_bReAlign[1][1] = FALSE; // [nCam][nPos] 

			m_bSkipAlign[0][0] = FALSE; // [nCam][nPos] 
			m_bSkipAlign[0][1] = FALSE; // [nCam][nPos] 
			m_bSkipAlign[1][0] = FALSE; // [nCam][nPos] 
			m_bSkipAlign[1][1] = FALSE; // [nCam][nPos] 

			m_bFailAlign[0][0] = FALSE; // [nCam][nPos] 
			m_bFailAlign[0][1] = FALSE; // [nCam][nPos] 
			m_bFailAlign[1][0] = FALSE; // [nCam][nPos] 
			m_bFailAlign[1][1] = FALSE; // [nCam][nPos] 
			m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::DoneMk) + 1:  // Mk���� �ʱ�ȭ
			pView->m_mgrPunch->m_bDoMk[0] = TRUE;
			pView->m_mgrPunch->m_bDoMk[1] = TRUE;
			pView->m_mgrPunch->m_bDoneMk[0] = FALSE;
			pView->m_mgrPunch->m_bDoneMk[1] = FALSE;
			pView->m_mgrPunch->m_bReMark[0] = FALSE;
			pView->m_mgrPunch->m_bReMark[1] = FALSE;
			pView->m_mgrPunch->m_bCam = FALSE;
			m_nPrevMkStAuto = 0;

			for (a = 0; a < 2; a++)
			{
				for (b = 0; b < MAX_STRIP_NUM; b++)
				{
					pView->m_mgrPunch->m_nMkStrip[a][b] = 0;
					pView->m_mgrPunch->m_bRejectDone[a][b] = FALSE;
				}
			}

			m_nMkStAuto++;
			break;

		case MK_ST + (Mk4PtIdx::DoneMk) + 2:
			pView->MpeWrite(_T("MB440150"), 0);	// ��ŷ�� ��ŷ�� ON (PC�� ON, OFF)
			pView->MpeWrite(_T("MB440170"), 1);	// ��ŷ�Ϸ�(PLC�� Ȯ���ϰ� Reset��Ŵ.)-20141029
			pDoc->Log(_T("PC: ��ŷ�Ϸ�(PLC�� Ȯ���ϰ� Reset��Ŵ.)"));
			if (pView->IsNoMk() || IsShowLive())
				ShowLive(FALSE);

			m_nMkStAuto++;
			break;

		case MK_ST + (Mk4PtIdx::DoneMk) + 3:
#ifdef USE_MPE
			if (pDoc->m_pMpeSignal[0] & (0x01 << 1))	// ��ŷ�� Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)-20141030
			{
				pDoc->Log(_T("PLC: ��ŷ�� Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)"));
				pView->MpeWrite(_T("MB440101"), 0);	// ��ŷ�� Feeding�Ϸ�

				Shift2Mk();			// PCR �̵�(Buffer->Marked) // ���(WorkingInfo.LastJob.sSerial)
				UpdateRst();
				SetMkFdLen();

				SetCycTime();
				m_dwCycSt = GetTickCount();

				UpdateWorking();	// Update Working Info...
				pView->ChkYield();// (dTotLmt, dPrtLmt, dRatio);
				m_nMkStAuto++;
			}
#endif
			break;
		case MK_ST + (Mk4PtIdx::DoneMk) + 4:
			ChkLotCutPos();
			m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::DoneMk) + 5:
			m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::DoneMk) + 6:
			m_nMkStAuto++;
			if (!IsBuffer(0))
			{
				m_bLotEnd = TRUE;
				m_nLotEndAuto = LOT_END;
			}
			else
			{
				// [0]: AOI-Up , [1]: AOI-Dn
				if (bDualTest)
				{
					nSerial = pDoc->m_ListBuf[0].Pop(); // Up (��)
					nSerial = pDoc->m_ListBuf[0].Pop(); // Up (��)

					SetListBuf();
				}

				if (m_nLotEndSerial > 0)
				{
					nSerial = GetBufferUp(&nPrevSerial);

					if (m_bSerialDecrese)
					{
						if (nSerial < m_nLotEndSerial || nSerial <= 0) // �뱤�ҷ� 3��° Lot End ( -1, -1, -2)
						{
							if (IsDoneDispMkInfo())
							{
								if (IsBuffer() && nSerial > 0)
									SetSerial(nSerial);
								else
									SetSerial(nPrevSerial - 1, TRUE);

								if (IsBuffer() && nSerial > 0)
									SetSerial(nSerial + 1);
								else
									SetSerial(nPrevSerial - 2, TRUE);

								m_bLotEnd = TRUE;
								m_nLotEndAuto = LOT_END;
							}
						}
					}
					else
					{
						if (nSerial > m_nLotEndSerial || nSerial <= 0) // �뱤�ҷ� 3��° Lot End ( -1, -1, -2)
						{
							if (IsDoneDispMkInfo())
							{
								if (IsBuffer() && nSerial > 0)
									SetSerial(nSerial);
								else
									SetSerial(nPrevSerial + 1, TRUE);

								if (IsBuffer() && nSerial > 0)
									SetSerial(nSerial + 1);
								else
									SetSerial(nPrevSerial + 2, TRUE);

								m_bLotEnd = TRUE;
								m_nLotEndAuto = LOT_END;
							}
						}
					}

					if (bDualTest)
					{
						nSerial = GetBufferDn(&nPrevSerial);

						if (m_bSerialDecrese)
						{
							if (nSerial < m_nLotEndSerial || nSerial <= 0) // �뱤�ҷ� 3��° Lot End ( -1, -1, -2)
							{
								if (IsDoneDispMkInfo())
								{
									if (IsBuffer() && nSerial > 0)
										SetSerial(nSerial);
									else
										SetSerial(nPrevSerial - 1, TRUE);

									if (IsBuffer() && nSerial > 0)
										SetSerial(nSerial + 1);
									else
										SetSerial(nPrevSerial - 2, TRUE);

									m_bLotEnd = TRUE;
									m_nLotEndAuto = LOT_END;
								}
							}
						}
						else
						{
							if (nSerial > m_nLotEndSerial || nSerial <= 0) // �뱤�ҷ� 3��° Lot End ( -1, -1, -2)
							{
								if (IsDoneDispMkInfo())
								{
									if (IsBuffer() && nSerial > 0)
										SetSerial(nSerial);
									else
										SetSerial(nPrevSerial + 1, TRUE);

									if (IsBuffer() && nSerial > 0)
										SetSerial(nSerial + 1);
									else
										SetSerial(nPrevSerial + 2, TRUE);

									m_bLotEnd = TRUE;
									m_nLotEndAuto = LOT_END;
								}
							}
						}
					}
				}
			}

			break;
		case MK_ST + (Mk4PtIdx::DoneMk) + 7:
			m_bMkSt = FALSE;
			::WritePrivateProfileString(_T("Last Job"), _T("MkSt"), _T("0"), PATH_WORKING_INFO);
			break;
		}
	}
}

void CManagerProcedure::Mk4PtLotDiff()
{
	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk4PtIdx::LotDiff) :
			Stop();
			//TowerLamp(RGB_YELLOW, TRUE);
			m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::LotDiff) + 1:
			if (IDYES == MsgBox(_T("���� �ϸ��� Lot�� �ٸ��ϴ�.\r\n��� �۾��� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
			{
				m_bContDiffLot = TRUE;
			}
			else
			{
				m_bContDiffLot = FALSE;
				m_bLotEnd = TRUE;
				m_nLotEndAuto = LOT_END;
			}
			m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::LotDiff) + 2:
			if (IsRun())
			{
				if (m_bContDiffLot)
					m_nMkStAuto = MK_ST + (Mk4PtIdx::ChkSn);
				else
					m_nMkStAuto++;
			}
			break;
		case MK_ST + (Mk4PtIdx::LotDiff) + 3:
			m_bContDiffLot = FALSE;
			m_bLotEnd = TRUE;
			m_nLotEndAuto = LOT_END;
			break;
		}
	}
}

void CManagerProcedure::Mk4PtReject()
{
	if (!pView->m_mgrPunch)
		return;

	int a, b;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case REJECT_ST:
			Stop();
			//TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);

			if (IDYES == MsgBox(_T("��Ʈ üũ �ҷ��Դϴ�.\r\n���� ó���� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
			{
				m_bAnswer[0] = TRUE;
				m_nMkStAuto++;
			}
			else
			{
				if (IDYES == MsgBox(_T("���� ó���� ����Ͽ����ϴ�.\r\n�ҷ��� ��ŷ ó���� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
				{
					m_bAnswer[1] = TRUE;
					m_nMkStAuto++;
				}
			}

			Buzzer(FALSE, 0);
			break;
		case REJECT_ST + 1:
			if (IsRun())
			{
				if (m_bAnswer[0])
				{
					m_bAnswer[0] = FALSE;
					m_nMkStAuto++;
				}
				else if (m_bAnswer[1])
				{
					m_bAnswer[1] = FALSE;
					m_nMkStAuto = MK_ST + (Mk4PtIdx::DoMk);	// Mk ��ŷ ����
				}
				else
					m_nMkStAuto = REJECT_ST;
			}
			break;
		case REJECT_ST + 2:
			SetReject();
			m_nMkStAuto++;
			break;
		case REJECT_ST + 3:
			m_nMkStAuto++;
			break;
		case REJECT_ST + 4:
			if (IsMkDone() && IsMoveDone())
			{
				if (IsVerify() && !m_nPrevMkStAuto)
				{
					m_nPrevMkStAuto = REJECT_ST + 4;
					m_nMkStAuto = REJECT_ST + 2;		// Mk ��ŷ ����
					pView->m_mgrPunch->m_bCam = TRUE;

					pView->m_mgrPunch->m_bDoneMk[0] = FALSE;
					pView->m_mgrPunch->m_bDoneMk[1] = FALSE;

					for (a = 0; a < 2; a++)
					{
						for (b = 0; b < MAX_STRIP_NUM; b++)
						{
							pView->m_mgrPunch->m_nMkStrip[a][b] = 0;
							pView->m_mgrPunch->m_bRejectDone[a][b] = FALSE;
						}
					}
				}
				else
				{
					if (pView->m_mgrPunch->IsReview())
					{
						if (!pView->m_mgrPunch->m_bCam)
						{
							m_nPrevStepAuto = REJECT_ST + 4;
							m_nMkStAuto = REJECT_ST + 2;		// Mk ��ŷ ����
							pView->m_mgrPunch->m_bCam = TRUE;
							MsgBox(_T("�� Jog ��ư�� �̿��Ͽ� ��ŷ��ġ�� Ȯ���Ͽ� �ּ���."));
						}
						else
						{
							pView->m_mgrPunch->m_bCam = FALSE;
							m_nMkStAuto++;	// Mk ��ŷ �Ϸ�
						}
					}
					else
					{
						m_nMkStAuto++;	// Mk ��ŷ �Ϸ�
					}
				}
			}
			break;
		case REJECT_ST + 5:
			m_nMkStAuto = MK_ST + (Mk4PtIdx::DoneMk);				// Align���� �ʱ�ȭ
			break;
		}
	}
}

void CManagerProcedure::Mk4PtErrStop()
{
	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case ERROR_ST:
			Stop();
			//TowerLamp(RGB_RED, TRUE);

			if (IDYES == MsgBox(_T("��Ʈ üũ Error�Դϴ�.\r\n�ٽ� ��Ʈ üũ�� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
			{
				m_bAnswer[0] = TRUE;
				m_nMkStAuto++;
			}
			else
			{
				if (IDYES == MsgBox(_T("��Ʈ üũ�� ����Ͽ����ϴ�.\r\n�ҷ��� ��ŷ ó���� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
				{
					m_bAnswer[1] = TRUE;
					m_nMkStAuto++;
				}
				else
					m_nMkStAuto++;
			}
			break;
		case ERROR_ST + 1:
			m_nMkStAuto++;
			break;
		case ERROR_ST + 2:
			if (IsRun())
			{
				if (m_bAnswer[0])
				{
					m_bAnswer[0] = FALSE;
					m_nMkStAuto = MK_ST + (Mk4PtIdx::ChkElec); // DoElecChk
				}
				else if (m_bAnswer[1])
				{
					m_bAnswer[1] = FALSE;
					m_nMkStAuto = MK_ST + (Mk4PtIdx::DoMk);	// Mk ��ŷ ����
				}
				else
					m_nMkStAuto++;
			}
			break;
		case ERROR_ST + 3:
			m_nMkStAuto = ERROR_ST;
			break;
		}
	}
}

void CManagerProcedure::MarkingWith4PointAlign()
{
	Mk4PtReady();
	Mk4PtChkSerial();
	Mk4PtInit();
	Mk4PtAlignPt0();
	Mk4PtAlignPt1();
	Mk4PtAlignPt2();
	Mk4PtAlignPt3();
	Mk4PtMoveInitPos();
	Mk4PtElecChk();
	Mk4PtDoMarking();
	Mk4PtLotDiff();
	Mk4PtReject();
	Mk4PtErrStop();
}

void CManagerProcedure::MoveInitPos0(BOOL bWait)
{
	if (!pView->m_mgrPunch)
		return;
	pView->m_mgrPunch->MoveInitPos0(bWait);
}

void CManagerProcedure::MoveInitPos1(BOOL bWait)
{
	if (!pView->m_mgrPunch)
		return;
	pView->m_mgrPunch->MoveInitPos1(bWait);
}

void CManagerProcedure::MoveMkEdPos1()
{
	if (!pView->m_mgrPunch)
		return;
	pView->m_mgrPunch->MoveMkEdPos1();
}

BOOL CManagerProcedure::ChkLightErr()
{
	if (!pView->m_mgrPunch)
		return FALSE;
	return pView->m_mgrPunch->ChkLightErr();
}

CfPoint CManagerProcedure::GetMkPnt(int nSerial, int nMkPcs) // pcr �ø���, pcr �ҷ� �ǽ� ���� ���� �ε���
{
	return pView->GetMkPnt(nSerial, nMkPcs);
}

BOOL CManagerProcedure::UpdateReelmap(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.54"));
		return 0;
	}

	if (!pDoc->MakeMkDir())
		return FALSE;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if(pView && pView->m_mgrReelmap)
		pView->m_mgrReelmap->m_nSerialRmapUpdate = nSerial;

	m_bTHREAD_UPDATE_REELMAP_UP = TRUE;
	if (bDualTest)
	{
		m_bTHREAD_UPDATE_REELMAP_DN = TRUE;
		m_bTHREAD_UPDATE_REELMAP_ALLUP = TRUE;
		m_bTHREAD_UPDATE_REELMAP_ALLDN = TRUE;
	}

	if (pDoc->GetTestMode() == MODE_INNER || pDoc->GetTestMode() == MODE_OUTER)
		UpdateReelmapInner(nSerial);

	Sleep(100);
	return TRUE;
}

BOOL CManagerProcedure::UpdateReelmapInner(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.54"));
		return 0;
	}

	if (!pDoc->MakeMkDir())
		return FALSE;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if(pView && pView->m_mgrReelmap)
		pView->m_mgrReelmap->m_nSerialRmapInnerUpdate = nSerial;

	m_bTHREAD_UPDATE_REELMAP_INNER_UP = TRUE;
	if (bDualTest)
	{
		m_bTHREAD_UPDATE_REELMAP_INNER_DN = TRUE;
		m_bTHREAD_UPDATE_REELMAP_INNER_ALLUP = TRUE;
		m_bTHREAD_UPDATE_REELMAP_INNER_ALLDN = TRUE;
	}

	m_bTHREAD_UPDATE_REELMAP_ITS = TRUE;

	Sleep(100);
	return TRUE;
}

void CManagerProcedure::UpdateYieldOnThread(int nSerial)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (m_bLastProc && m_nBufUpSerial[0] == m_nLotEndSerial)
	{
		m_nSerialTHREAD_UPDATAE_YIELD[0] = nSerial;
		m_bTHREAD_UPDATAE_YIELD[0] = TRUE; // Cam[0]
		Sleep(30);
	}
	else
	{
		m_nSerialTHREAD_UPDATAE_YIELD[0] = nSerial;
		m_bTHREAD_UPDATAE_YIELD[0] = TRUE; // Cam[0]
		Sleep(30);

		if (m_nBufUpSerial[1] > 0)
		{
			m_nSerialTHREAD_UPDATAE_YIELD[1] = m_nBufUpSerial[1];
			m_bTHREAD_UPDATAE_YIELD[1] = TRUE; // Cam[1]
			Sleep(30);
		}
		else if (m_nBufUpSerial[1] < 0)
		{
			return;
		}
	}
}

BOOL CManagerProcedure::ReloadReelmap(int nSerial)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	m_bTHREAD_RELOAD_RST_UP = TRUE;
	if (bDualTest)
	{
		m_bTHREAD_RELOAD_RST_DN = TRUE;
		m_bTHREAD_RELOAD_RST_ALLUP = TRUE;
		m_bTHREAD_RELOAD_RST_ALLDN = TRUE;
	}

	if (pDoc->GetTestMode() == MODE_OUTER)
	{
		m_bTHREAD_RELOAD_RST_UP_INNER = TRUE;
		m_bTHREAD_RELOAD_RST_ITS = TRUE;
		if (pDoc->WorkingInfo.LastJob.bDualTestInner)
		{
			m_bTHREAD_RELOAD_RST_DN_INNER = TRUE;
			m_bTHREAD_RELOAD_RST_ALLUP_INNER = TRUE;
			m_bTHREAD_RELOAD_RST_ALLDN_INNER = TRUE;
		}
	}

	Sleep(100);

	return TRUE;
}

BOOL CManagerProcedure::IsVs()
{
	if (!m_bChkLastProcVs)
	{
		BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
		if (bDualTest)
		{
			if (GetAoiDnVsStatus())
				return TRUE;
			else if (GetAoiUpVsStatus())
				return TRUE;
		}
		else
		{
			if (GetAoiUpVsStatus())
				return TRUE;
		}
	}

	return FALSE;
}