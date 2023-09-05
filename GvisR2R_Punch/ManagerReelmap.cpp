// ManagerReelmap.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "GvisR2R_Punch.h"
#include "ManagerReelmap.h"

#include "Process/DataFile.h"
#include "Dialog/DlgProgress.h"

#include "MainFrm.h"
#include "GvisR2R_PunchDoc.h"
#include "GvisR2R_PunchView.h"

extern CMainFrame* pFrm;
extern CGvisR2R_PunchDoc* pDoc;
extern CGvisR2R_PunchView* pView;

// CManagerReelmap

IMPLEMENT_DYNAMIC(CManagerReelmap, CWnd)

CManagerReelmap::CManagerReelmap(CWnd* pParent /*=NULL*/)
{
	int i, k;
	m_pParent = pParent;

	m_pReelMapDisp = NULL;
	m_pReelMap = NULL;
	m_pReelMapUp = NULL;
	m_pReelMapDn = NULL;
	m_pReelMapAllUp = NULL;
	m_pReelMapAllDn = NULL;

	m_pReelMapInner = NULL;
	m_pReelMapInnerUp = NULL;
	m_pReelMapInnerDn = NULL;
	m_pReelMapInnerAllUp = NULL;
	m_pReelMapInnerAllDn = NULL;
	m_pReelMapIts = NULL;

	for (i = 0; i < MAX_THREAD_MGR_RMAP; i++)
	{
		m_bThread[i] = FALSE;
		m_dwThreadTick[i] = 0;
	}

	for (i = 0; i < MAX_PCR; i++)
	{
		for (k = 0; k < MAX_PCR_PNL; k++)
		{
			m_pPcr[i][k] = NULL;
			m_pPcrInner[i][k] = NULL;
		}
	}

	for (k = 0; k < MAX_PCR_PNL; k++)
		m_pPcrIts[k] = NULL;

	pMkInfo = NULL;

	// for ITS
	m_bEngDualTest = TRUE;
	m_sItsCode = _T("");
	m_sEngLotNum = _T("");
	m_sEngProcessNum = _T("");
	m_sEngModel = _T("");
	m_sEngLayerUp = _T("");
	m_sEngLayerDn = _T("");
	m_nWritedItsSerial = 0;

	RECT rt = { 0,0,0,0 };
	if (!Create(NULL, NULL, WS_CHILD, rt, pParent, 0))
		AfxMessageBox(_T("CManagerReelmap::Create() Failed!!!"));

	Init(); // New Reelmap.

	TCHAR szData[200];
	CString sVal, sPath = PATH_WORKING_INFO;

	if (m_pReelMap)
		m_pReelMap->m_bUseTempPause = pDoc->WorkingInfo.LastJob.bTempPause;

	if (0 < ::GetPrivateProfileString(_T("Last Job"), _T("Engrave Its Code"), NULL, szData, sizeof(szData), sPath))
		m_sItsCode = pDoc->WorkingInfo.LastJob.sEngItsCode = CString(szData);
	else
		m_sItsCode = pDoc->WorkingInfo.LastJob.sEngItsCode = _T("");
}

CManagerReelmap::~CManagerReelmap()
{
	int i, k;

	if (m_pReelMapUp)
	{
		delete m_pReelMapUp;
		m_pReelMapUp = NULL;
	}

	if (m_pReelMapDn)
	{
		delete m_pReelMapDn;
		m_pReelMapDn = NULL;
	}

	if (m_pReelMapAllUp)
	{
		delete m_pReelMapAllUp;
		m_pReelMapAllUp = NULL;
	}

	if (m_pReelMapAllDn)
	{
		delete m_pReelMapAllDn;
		m_pReelMapAllDn = NULL;
	}

	if (m_pReelMapIts)
	{
		delete m_pReelMapIts;
		m_pReelMapIts = NULL;
	}

	if (m_pReelMapInnerUp)
	{
		delete m_pReelMapInnerUp;
		m_pReelMapInnerUp = NULL;
	}

	if (m_pReelMapInnerDn)
	{
		delete m_pReelMapInnerDn;
		m_pReelMapInnerDn = NULL;
	}

	if (m_pReelMapInnerAllUp)
	{
		delete m_pReelMapInnerAllUp;
		m_pReelMapInnerAllUp = NULL;
	}

	if (m_pReelMapInnerAllDn)
	{
		delete m_pReelMapInnerAllDn;
		m_pReelMapInnerAllDn = NULL;
	}

	for (i = 0; i < MAX_PCR; i++)
	{
		for (k = 0; k < MAX_PCR_PNL; k++)
		{
			if (m_pPcr[i][k])
			{
				delete m_pPcr[i][k];
				m_pPcr[i][k] = NULL;
			}

			if (m_pPcrInner[i][k])
			{
				delete m_pPcrInner[i][k];
				m_pPcrInner[i][k] = NULL;
			}
		}
	}

	for (k = 0; k < MAX_PCR_PNL; k++)
	{
		if (m_pPcrIts[k])
		{
			delete m_pPcrIts[k];
			m_pPcrIts[k] = NULL;
		}
	}

	if (pMkInfo)
	{
		delete[] pMkInfo;
		pMkInfo = NULL;
	}

	if (m_pFile)
	{
		delete m_pFile;
		m_pFile = NULL;
	}
}

BEGIN_MESSAGE_MAP(CManagerReelmap, CWnd)
END_MESSAGE_MAP()


// CManagerReelmap 메시지 처리기입니다.

void CManagerReelmap::Init()
{
	m_pFile = new CMyFile();

	InitPcr();
	LoadMstInfo();
	InitReelmap();
	StartThread();
}

void  CManagerReelmap::StartThread()
{
	if (!m_bThread[0])
		m_Thread[0].Start(GetSafeHwnd(), this, ThreadProc0);	// UpdateYield
	
	if (!m_bThread[1])
		m_Thread[1].Start(GetSafeHwnd(), this, ThreadProc1);	// UpdataeReelmapUp

	if (!m_bThread[2])
		m_Thread[2].Start(GetSafeHwnd(), this, ThreadProc2);	// UpdataeReelmapAllUp

	if (!m_bThread[3])
		m_Thread[3].Start(GetSafeHwnd(), this, ThreadProc3);	// UpdataeReelmapDn

	if (!m_bThread[4])
		m_Thread[4].Start(GetSafeHwnd(), this, ThreadProc4);	// UpdataeReelmapAllDn

	if (!m_bThread[5])
		m_Thread[5].Start(GetSafeHwnd(), this, ThreadProc5);	// UpdataeRstUp

	if (!m_bThread[6])
		m_Thread[6].Start(GetSafeHwnd(), this, ThreadProc6);	// UpdataeRstDn

	if (!m_bThread[7])
		m_Thread[7].Start(GetSafeHwnd(), this, ThreadProc7);	// UpdataeRstAllUp

	if (!m_bThread[8])
		m_Thread[8].Start(GetSafeHwnd(), this, ThreadProc8);	// UpdataeRstAllDn

	if (!m_bThread[9])
		m_Thread[9].Start(GetSafeHwnd(), this, ThreadProc9);	// UpdataeRstUp

	if (!m_bThread[10])
		m_Thread[10].Start(GetSafeHwnd(), this, ThreadProc10);	// UpdataeRstDn

	if (!m_bThread[11])
		m_Thread[11].Start(GetSafeHwnd(), this, ThreadProc11);	// UpdataeRstAllUp

	if (!m_bThread[12])
		m_Thread[12].Start(GetSafeHwnd(), this, ThreadProc12);	// UpdataeRstAllDn

	if (!m_bThread[13])
		m_Thread[13].Start(GetSafeHwnd(), this, ThreadProc13);	// UpdataeReelmapITS

	if (!m_bThread[14])
		m_Thread[14].Start(GetSafeHwnd(), this, ThreadProc14);	// UpdateRstIts

	if (!m_bThread[15])
		m_Thread[15].Start(GetSafeHwnd(), this, ThreadProc15);	// ReloadReelmapUpInner

	if (!m_bThread[16])
		m_Thread[16].Start(GetSafeHwnd(), this, ThreadProc16);	// ReloadReelmapDnInner

	if (!m_bThread[17])
		m_Thread[17].Start(GetSafeHwnd(), this, ThreadProc17);	// ReloadReelmapAllUpInner

	if (!m_bThread[18])
		m_Thread[18].Start(GetSafeHwnd(), this, ThreadProc18);	// ReloadReelmapAllUpInner

	if (!m_bThread[19])
		m_Thread[19].Start(GetSafeHwnd(), this, ThreadProc19);	// ReloadReelmapIts

	if (!m_bThread[20])
		m_Thread[20].Start(GetSafeHwnd(), this, ThreadProc20);	// UpdataeReelmapUp

	if (!m_bThread[21])
		m_Thread[21].Start(GetSafeHwnd(), this, ThreadProc21);	// UpdataeReelmapAllUp

	if (!m_bThread[22])
		m_Thread[22].Start(GetSafeHwnd(), this, ThreadProc22);	// UpdataeReelmapDn

	if (!m_bThread[23])
		m_Thread[23].Start(GetSafeHwnd(), this, ThreadProc23);	// UpdataeReelmapAllDn

	if (!m_bThread[24])
		m_Thread[24].Start(GetSafeHwnd(), this, ThreadProc24);	// UpdataeYieldUp

	if (!m_bThread[25])
		m_Thread[25].Start(GetSafeHwnd(), this, ThreadProc25);	// UpdataeYieldDn

	if (!m_bThread[26])
		m_Thread[26].Start(GetSafeHwnd(), this, ThreadProc26);	// UpdataeYieldAllUp

	if (!m_bThread[27])
		m_Thread[27].Start(GetSafeHwnd(), this, ThreadProc27);	// UpdataeYieldAllDn

	if (!m_bThread[28])
		m_Thread[28].Start(GetSafeHwnd(), this, ThreadProc28);	// UpdataeYieldInnerUp

	if (!m_bThread[29])
		m_Thread[29].Start(GetSafeHwnd(), this, ThreadProc29);	// UpdataeYieldInnerDn

	if (!m_bThread[30])
		m_Thread[30].Start(GetSafeHwnd(), this, ThreadProc30);	// UpdataeYieldInnerAllUp

	if (!m_bThread[31])
		m_Thread[31].Start(GetSafeHwnd(), this, ThreadProc31);	// UpdataeYieldInnerAllDn

	if (!m_bThread[32])
		m_Thread[32].Start(GetSafeHwnd(), this, ThreadProc32);	// UpdataeYieldIts
}

void CManagerReelmap::StopThread()
{
	int i;
	for (i = 0; i < MAX_THREAD_MGR_RMAP; i++)
	{
		if (m_bThread[i])
		{
			m_Thread[i].Stop();
			Sleep(20);
			while (m_bThread[i])
			{
				Sleep(20);
			}
		}
	}
}

void CManagerReelmap::InitVal()
{
}

void CManagerReelmap::InitMstData()
{
	ResetMkInfo(0); // 0 : AOI-Up , 1 : AOI-Dn , 2 : AOI-UpDn	
	GetCamPxlRes();

	if (pView->IsLastJob(0)) // Up
	{
		m_Master[0].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
			pDoc->WorkingInfo.LastJob.sModelUp,
			pDoc->WorkingInfo.LastJob.sLayerUp);
		m_Master[0].LoadMstInfo();

		if (pDoc->GetTestMode() == MODE_OUTER)
		{
			m_MasterInner[0].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sInnerLayerUp);
			m_MasterInner[0].LoadMstInfo();
		}
	}

	if (pView->IsLastJob(1)) // Dn
	{
		m_Master[1].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
			pDoc->WorkingInfo.LastJob.sModelDn,
			pDoc->WorkingInfo.LastJob.sLayerDn,
			pDoc->WorkingInfo.LastJob.sLayerUp);
		m_Master[1].LoadMstInfo();

		if (pDoc->GetTestMode() == MODE_OUTER)
		{
			m_MasterInner[0].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sInnerLayerDn,
				pDoc->WorkingInfo.LastJob.sInnerLayerUp);
			m_MasterInner[0].LoadMstInfo();
		}
	}

	if(pView->m_mgrProcedure)
		pView->m_mgrReelmap->InitReelmap();

	if (m_Master[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, pDoc->WorkingInfo.LastJob.sInnerLayerUp))
	{
		InitReelmapInner();
		SetReelmapInner(ROT_NONE);
	}
}

BOOL CManagerReelmap::InitReelmap()
{
	if (!m_Master[0].m_pPcsRgn)
	{
		CString strMsg;
		strMsg.Format(_T("피스 영역이 존재하지 않습니다."));
		//pView->MsgBox(strMsg);
		pView->ClrDispMsg();
		AfxMessageBox(strMsg, MB_ICONSTOP);
		return FALSE;
	}

	int nTotPcs = m_Master[0].m_pPcsRgn->nTotPcs;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (m_pReelMapUp)
	{
		delete m_pReelMapUp;
		m_pReelMapUp = NULL;
	}
	m_pReelMapUp = new CReelMap(RMAP_UP, MAX_DISP_PNL, nTotPcs, 0, this);

	if (bDualTest)
	{
		if (m_pReelMapDn)
		{
			delete m_pReelMapDn;
			m_pReelMapDn = NULL;
		}
		m_pReelMapDn = new CReelMap(RMAP_DN, MAX_DISP_PNL, nTotPcs, 0, this);

		if (m_pReelMapAllUp)
		{
			delete m_pReelMapAllUp;
			m_pReelMapAllUp = NULL;
		}
		m_pReelMapAllUp = new CReelMap(RMAP_ALLUP, MAX_DISP_PNL, nTotPcs, 0, this);

		if (m_pReelMapAllDn)
		{
			delete m_pReelMapAllDn;
			m_pReelMapAllDn = NULL;
		}
		m_pReelMapAllDn = new CReelMap(RMAP_ALLDN, MAX_DISP_PNL, nTotPcs, 0, this);

		if (pDoc->GetTestMode() != MODE_OUTER)
			m_pReelMap = m_pReelMapAllUp;
		else
		{
			if (m_pReelMapIts)
			{
				delete m_pReelMapIts;
				m_pReelMapIts = NULL;
			}
			m_pReelMapIts = new CReelMap(RMAP_ITS, MAX_DISP_PNL, nTotPcs, 0, this); // Default: RMAP_NONE (RMAP_INNER -> RMAP_INNER_UP)
			m_pReelMap = m_pReelMapIts;
		}
	}
	else
	{
		if (pDoc->GetTestMode() != MODE_OUTER)
			m_pReelMap = m_pReelMapUp;
		else
		{
			if (m_pReelMapIts)
				m_pReelMap = m_pReelMapIts;
		}
	}

	if (pMkInfo)
	{
		delete[] pMkInfo;
		pMkInfo = NULL;
	}
	if (!pMkInfo)
		pMkInfo = new CString[nTotPcs];

	if (m_pReelMap)
	{
		m_pReelMapDisp = m_pReelMap;
	}
	return TRUE;
}

BOOL CManagerReelmap::InitReelmapUp()
{
	if (!m_Master[0].m_pPcsRgn)
	{
		CString strMsg;
		strMsg.Format(_T("피스 영역이 존재하지 않습니다."));
		pView->MsgBox(strMsg);
		//AfxMessageBox(strMsg,MB_ICONSTOP);
		return FALSE;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nTotPcs = m_Master[0].m_pPcsRgn->nTotPcs;

	if (m_pReelMapUp)
	{
		delete m_pReelMapUp;
		m_pReelMapUp = NULL;
	}
	m_pReelMapUp = new CReelMap(RMAP_UP, MAX_DISP_PNL, nTotPcs, 0, this);

	if (bDualTest)
	{
		if (m_pReelMapAllUp)
		{
			delete m_pReelMapAllUp;
			m_pReelMapAllUp = NULL;
		}
		m_pReelMapAllUp = new CReelMap(RMAP_ALLUP, MAX_DISP_PNL, nTotPcs, 0, this);

		if (pDoc->GetTestMode() != MODE_OUTER)
			m_pReelMap = m_pReelMapAllUp;
		else
		{
			if (m_pReelMapIts)
				m_pReelMap = m_pReelMapIts;
		}
	}
	else
	{
		if (pDoc->GetTestMode() != MODE_OUTER)
			m_pReelMap = m_pReelMapUp;
		else
		{
			if (m_pReelMapIts)
				m_pReelMap = m_pReelMapIts;
		}
	}

	if (m_pReelMap->m_nLayer == RMAP_UP || m_pReelMap->m_nLayer == RMAP_ALLUP)
	{
		if (pMkInfo)
		{
			delete[] pMkInfo;
			pMkInfo = NULL;
		}
		if (!pMkInfo)
			pMkInfo = new CString[nTotPcs];
	}

	if (pView->m_pDlgMenu01 && pView->m_mgrProcedure)
		pView->m_pDlgMenu01->SwitchReelmapDisp(pView->m_mgrProcedure->m_nSelRmap);

	return TRUE;
}

BOOL CManagerReelmap::InitReelmapDn()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return TRUE;

	if (!m_Master[0].m_pPcsRgn)
	{
		CString strMsg;
		strMsg.Format(_T("피스 영역이 존재하지 않습니다."));
		pView->MsgBox(strMsg);
		//AfxMessageBox(strMsg,MB_ICONSTOP);
		return FALSE;
	}

	int nTotPcs = m_Master[0].m_pPcsRgn->nTotPcs;

	if (m_pReelMapDn)
	{
		delete m_pReelMapDn;
		m_pReelMapDn = NULL;
	}
	m_pReelMapDn = new CReelMap(RMAP_DN, MAX_DISP_PNL, nTotPcs, 0, this);

	if (m_pReelMapAllDn)
	{
		delete m_pReelMapAllDn;
		m_pReelMapAllDn = NULL;
	}
	m_pReelMapAllDn = new CReelMap(RMAP_ALLDN, MAX_DISP_PNL, nTotPcs, 0, this);

	if (m_pReelMap->m_nLayer == RMAP_DN || m_pReelMap->m_nLayer == RMAP_ALLDN)
	{
		if (pMkInfo)
		{
			delete[] pMkInfo;
			pMkInfo = NULL;
		}
		if (!pMkInfo)
			pMkInfo = new CString[nTotPcs];
	}

	if (pView->m_pDlgMenu01 && pView->m_mgrProcedure)
		pView->m_pDlgMenu01->SwitchReelmapDisp(pView->m_mgrProcedure->m_nSelRmap);

	return TRUE;
}

BOOL CManagerReelmap::LoadMstInfo()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	CString sLot, sLayerUp, sLayerDn;
	BOOL bDualTestInner, bGetCurrentInfoEng;

	bGetCurrentInfoEng = pDoc->GetCurrentInfoEng();
	GetCamPxlRes();

	if (pView->IsLastJob(0)) // Up
	{
		m_Master[0].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
			pDoc->WorkingInfo.LastJob.sModelUp,
			pDoc->WorkingInfo.LastJob.sLayerUp);
		m_Master[0].LoadMstInfo();

		if (bGetCurrentInfoEng)
		{
			if (GetItsSerialInfo(0, bDualTestInner, sLot, sLayerUp, sLayerDn, 3))
			{
				if (m_Master[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, sLayerUp))
				{
					m_MasterInner[0].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
						pDoc->WorkingInfo.LastJob.sModelUp,
						sLayerUp);
					m_MasterInner[0].LoadMstInfo();
				}
			}
		}
	}

	if (pView->IsLastJob(1)) // Dn
	{
		m_Master[1].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
			pDoc->WorkingInfo.LastJob.sModelUp,
			//pDoc->WorkingInfo.LastJob.sModelDn,
			pDoc->WorkingInfo.LastJob.sLayerDn,
			pDoc->WorkingInfo.LastJob.sLayerUp);

		m_Master[1].LoadMstInfo();

		if (bGetCurrentInfoEng)
		{
			if (GetItsSerialInfo(0, bDualTestInner, sLot, sLayerUp, sLayerDn, 0))
			{
				if (m_Master[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, sLayerDn))
				{
					m_MasterInner[1].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
						pDoc->WorkingInfo.LastJob.sModelUp,
						sLayerDn,
						sLayerUp);
					m_MasterInner[1].LoadMstInfo();
				}
			}
		}
	}

	//SetAlignPos();

	// Reelmap 정보 Loading.....
	InitReelmap(); // Delete & New
	SetReelmap(ROT_NONE);
	UpdateData();

	if (bGetCurrentInfoEng)
	{
		if (GetItsSerialInfo(0, bDualTestInner, sLot, sLayerUp, sLayerDn, 0))
		{
			if (m_Master[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, sLayerUp))
			{
				// Reelmap 정보 Loading.....
				InitReelmapInner(); // Delete & New
				SetReelmapInner(ROT_NONE);
			}
			else
			{
				pView->MsgBox(_T("InitReelmapInner()를 위한 GetItsSerialInfo의 정보가 없습니다."));
				return FALSE;
			}
		}
	}
#ifndef TEST_MODE
	pView->DispMsg(_T("릴맵을 초기화합니다."), _T("알림"), RGB_GREEN, DELAY_TIME_MSG);
	OpenReelmap();
#endif
	SetPathAtBuf(); // Reelmap path를 설정함.

	return TRUE;
}

void CManagerReelmap::ResetMkInfo(int nAoi) // 0 : AOI-Up , 1 : AOI-Dn , 2 : AOI-UpDn
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	BOOL bDualTestInner, bGetCurrentInfoEng;
	CString sLot, sLayerUp, sLayerDn;
	bGetCurrentInfoEng = pDoc->GetCurrentInfoEng();

	// CamMst Info...
	GetCamPxlRes();

	if (nAoi == 0 || nAoi == 2)
	{
		if (pView->IsLastJob(0)) // Up
		{
			m_Master[0].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sLayerUp);
			m_Master[0].LoadMstInfo();

			if (bGetCurrentInfoEng)
			{
				if (GetItsSerialInfo(0, bDualTestInner, sLot, sLayerUp, sLayerDn, 0))
				{
					//if (pDoc->GetTestMode() == MODE_OUTER)
					if (m_Master[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, sLayerUp))
					{
						m_MasterInner[0].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
							pDoc->WorkingInfo.LastJob.sModelUp,
							sLayerUp);
						m_MasterInner[0].LoadMstInfo();

						if (bDualTestInner)
						{
							m_MasterInner[1].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
								pDoc->WorkingInfo.LastJob.sModelUp,
								sLayerDn);
							m_MasterInner[1].LoadMstInfo();
						}
					}
				}
			}
		}
		else
		{
			pView->ClrDispMsg();
			AfxMessageBox(_T("Error - IsLastJob(0)..."));
		}

		InitReelmapUp();

		if (bGetCurrentInfoEng)
		{
			if (GetItsSerialInfo(0, bDualTestInner, sLot, sLayerUp, sLayerDn, 0))
			{
				if (m_Master[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, sLayerUp))
				{
					InitReelmapInnerUp();
					if (bDualTestInner)
						InitReelmapInnerDn();
					SetReelmapInner(ROT_NONE);
				}
			}
		}

		OpenReelmap();
	}


	if (bDualTest)
	{
		if (nAoi == 1 || nAoi == 2)
		{
			if (pView->IsLastJob(1)) // Dn
			{
				m_Master[1].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
					pDoc->WorkingInfo.LastJob.sModelUp,
					pDoc->WorkingInfo.LastJob.sLayerDn,
					pDoc->WorkingInfo.LastJob.sLayerUp);

				m_Master[1].LoadMstInfo();
			}
			else
			{
				AfxMessageBox(_T("Error - IsLastJob(1)..."));
			}

			InitReelmapDn();
		}
	}
}

void CManagerReelmap::InitPcr()
{
	int k, i;
	for (i = 0; i < MAX_PCR; i++)
	{
		for (k = 0; k < MAX_PCR_PNL; k++)
		{
			if (m_pPcr[i][k])
			{
				delete m_pPcr[i][k];
				m_pPcr[i][k] = NULL;
			}

			m_pPcr[i][k] = new CDataMarking();

			if (m_pPcrInner[i][k])
			{
				delete m_pPcrInner[i][k];
				m_pPcrInner[i][k] = NULL;
			}

			m_pPcrInner[i][k] = new CDataMarking();
		}
	}

	for (k = 0; k < MAX_PCR_PNL; k++)
	{
		if (m_pPcrIts[k])
		{
			delete m_pPcrIts[k];
			m_pPcrIts[k] = NULL;
		}

		m_pPcrIts[k] = new CDataMarking();
	}
}

void CManagerReelmap::SetReelmap(int nDir)
{
	if (!m_pReelMap || !m_Master[0].m_pPcsRgn)
		return;

	if (!m_pReelMap->pFrmRgn || !m_pReelMap->pPcsRgn)
		return;

	int i, k;
	double fData1, fData2, fData3, fData4, fDistX, fDistY;
	double fWidth, fHeight, fRight, fBottom;

	m_pReelMap->nDir = nDir;

	int nTotPnl = m_pReelMap->nTotPnl;
	int nTotPcs = m_pReelMap->nTotPcs;

	// 	double dScale = (MasterInfo.dPixelSize/10.0);
	double dScale = (m_Master[0].MasterInfo.dPixelSize / 10.0);
	m_pReelMap->SetAdjRatio(dScale);
	dScale = m_pReelMap->GetAdjRatio();

	for (k = 0; k < nTotPnl; k++)
	{
		switch (nDir)
		{
		case ROT_NONE:
			fWidth = (m_Master[0].m_pPcsRgn->pPcs[0].right - m_Master[0].m_pPcsRgn->pPcs[0].left);
			fHeight = (m_Master[0].m_pPcsRgn->pPcs[0].bottom - m_Master[0].m_pPcsRgn->pPcs[0].top);
			fRight = m_Master[0].m_pPcsRgn->rtFrm.right - fWidth * (1.0 - RMAP_PCS_SCALE);
			fBottom = m_Master[0].m_pPcsRgn->rtFrm.bottom - fHeight * (1.0 - RMAP_PCS_SCALE);;
			m_pReelMap->pFrmRgn[k].left = (m_Master[0].m_pPcsRgn->rtFrm.right + MYGL_GAP_PNL*dScale)*(nTotPnl - 1 - k) + m_Master[0].m_pPcsRgn->rtFrm.left;
			m_pReelMap->pFrmRgn[k].top = m_Master[0].m_pPcsRgn->rtFrm.top;
			m_pReelMap->pFrmRgn[k].right = (m_Master[0].m_pPcsRgn->rtFrm.right + MYGL_GAP_PNL*dScale)*(nTotPnl - 1 - k) + fRight;
			m_pReelMap->pFrmRgn[k].bottom = fBottom;

			if (m_pReelMapDisp)
			{
				m_pReelMapDisp->pFrmRgn[k].left = (m_Master[0].m_pPcsRgn->rtFrm.right + MYGL_GAP_PNL*dScale)*(nTotPnl - 1 - k) + m_Master[0].m_pPcsRgn->rtFrm.left;
				m_pReelMapDisp->pFrmRgn[k].top = m_Master[0].m_pPcsRgn->rtFrm.top;
				m_pReelMapDisp->pFrmRgn[k].right = (m_Master[0].m_pPcsRgn->rtFrm.right + MYGL_GAP_PNL*dScale)*(nTotPnl - 1 - k) + fRight;
				m_pReelMapDisp->pFrmRgn[k].bottom = fBottom;
			}

			for (i = 0; i < nTotPcs; i++)
			{
				fWidth = (m_Master[0].m_pPcsRgn->pPcs[i].right - m_Master[0].m_pPcsRgn->pPcs[i].left);
				fHeight = (m_Master[0].m_pPcsRgn->pPcs[i].bottom - m_Master[0].m_pPcsRgn->pPcs[i].top);
				fData1 = m_Master[0].m_pPcsRgn->pPcs[i].left;	// left
				fData2 = m_Master[0].m_pPcsRgn->pPcs[i].top;	// top
				fData3 = fData1 + fWidth * RMAP_PCS_SCALE; // right
				fData4 = fData2 + fHeight * RMAP_PCS_SCALE; // bottom

				m_pReelMap->pPcsRgn[k][i].left = (m_Master[0].m_pPcsRgn->rtFrm.right + MYGL_GAP_PNL*dScale)*(nTotPnl - 1 - k) + fData1;
				m_pReelMap->pPcsRgn[k][i].top = fData2;
				m_pReelMap->pPcsRgn[k][i].right = (m_Master[0].m_pPcsRgn->rtFrm.right + MYGL_GAP_PNL*dScale)*(nTotPnl - 1 - k) + fData3;
				m_pReelMap->pPcsRgn[k][i].bottom = fData4;

				if (m_pReelMapDisp)
				{
					m_pReelMapDisp->pPcsRgn[k][i].left = (m_Master[0].m_pPcsRgn->rtFrm.right + MYGL_GAP_PNL*dScale)*(nTotPnl - 1 - k) + fData1;
					m_pReelMapDisp->pPcsRgn[k][i].top = fData2;
					m_pReelMapDisp->pPcsRgn[k][i].right = (m_Master[0].m_pPcsRgn->rtFrm.right + MYGL_GAP_PNL*dScale)*(nTotPnl - 1 - k) + fData3;
					m_pReelMapDisp->pPcsRgn[k][i].bottom = fData4;
				}
			}
			break;
			// 		case ROT_NONE:
			// 			m_pReelMap->pFrmRgn[k].left = (m_pPcsRgn->rtFrm.right+MYGL_GAP_PNL*dScale)*(nTotPnl-1-k)+m_pPcsRgn->rtFrm.left;
			// 			m_pReelMap->pFrmRgn[k].top = m_pPcsRgn->rtFrm.top;
			// 			m_pReelMap->pFrmRgn[k].right = (m_pPcsRgn->rtFrm.right+MYGL_GAP_PNL*dScale)*(nTotPnl-1-k)+m_pPcsRgn->rtFrm.right;
			// 			m_pReelMap->pFrmRgn[k].bottom = m_pPcsRgn->rtFrm.bottom;
			// 
			// 			for(i=0; i<nTotPcs; i++)
			// 			{
			// 				fData1 = m_pPcsRgn->pPcs[i].left; // left
			// 				fData2 = m_pPcsRgn->pPcs[i].top; // top
			// 				fData3 = m_pPcsRgn->pPcs[i].right; // right
			// 				fData4 = m_pPcsRgn->pPcs[i].bottom; // bottom
			// 
			// 				m_pReelMap->pPcsRgn[k][i].left = (m_pPcsRgn->rtFrm.right+MYGL_GAP_PNL*dScale)*(nTotPnl-1-k)+fData1;
			// 				m_pReelMap->pPcsRgn[k][i].top = fData2;
			// 				m_pReelMap->pPcsRgn[k][i].right = (m_pPcsRgn->rtFrm.right+MYGL_GAP_PNL*dScale)*(nTotPnl-1-k)+fData3;
			// 				m_pReelMap->pPcsRgn[k][i].bottom = fData4;
			// 			}
			// 			break;
		case ROT_CCW_90: // right->bottom, top->left, bottom->right, left->top ; Dir (x *= 1, y *= -1) 
			fDistX = 0;
			fDistY = m_Master[0].m_pPcsRgn->rtFrm.left + m_Master[0].m_pPcsRgn->rtFrm.right;
			m_pReelMap->pFrmRgn[k].left = (m_Master[0].m_pPcsRgn->rtFrm.bottom + MYGL_GAP_PNL*dScale)*(nTotPnl - 1 - k) + m_Master[0].m_pPcsRgn->rtFrm.top;
			m_pReelMap->pFrmRgn[k].top = fDistY - m_Master[0].m_pPcsRgn->rtFrm.right;
			m_pReelMap->pFrmRgn[k].right = (m_Master[0].m_pPcsRgn->rtFrm.bottom + MYGL_GAP_PNL*dScale)*(nTotPnl - 1 - k) + m_Master[0].m_pPcsRgn->rtFrm.bottom;
			m_pReelMap->pFrmRgn[k].bottom = fDistY - m_Master[0].m_pPcsRgn->rtFrm.left;

			if (m_pReelMapDisp)
			{
				m_pReelMapDisp->pFrmRgn[k].left = (m_Master[0].m_pPcsRgn->rtFrm.bottom + MYGL_GAP_PNL*dScale)*(nTotPnl - 1 - k) + m_Master[0].m_pPcsRgn->rtFrm.top;
				m_pReelMapDisp->pFrmRgn[k].top = fDistY - m_Master[0].m_pPcsRgn->rtFrm.right;
				m_pReelMapDisp->pFrmRgn[k].right = (m_Master[0].m_pPcsRgn->rtFrm.bottom + MYGL_GAP_PNL*dScale)*(nTotPnl - 1 - k) + m_Master[0].m_pPcsRgn->rtFrm.bottom;
				m_pReelMapDisp->pFrmRgn[k].bottom = fDistY - m_Master[0].m_pPcsRgn->rtFrm.left;
			}

			for (i = 0; i < nTotPcs; i++)
			{
				fData1 = m_Master[0].m_pPcsRgn->pPcs[i].top;	// left
				fData2 = fDistY - m_Master[0].m_pPcsRgn->pPcs[i].right;	// top
				fData3 = m_Master[0].m_pPcsRgn->pPcs[i].bottom;	// right
				fData4 = fDistY - m_Master[0].m_pPcsRgn->pPcs[i].left; // bottom

				m_pReelMap->pPcsRgn[k][i].left = (m_Master[0].m_pPcsRgn->rtFrm.bottom + MYGL_GAP_PNL*dScale)*(nTotPnl - 1 - k) + fData1;
				m_pReelMap->pPcsRgn[k][i].top = fData2;
				m_pReelMap->pPcsRgn[k][i].right = (m_Master[0].m_pPcsRgn->rtFrm.bottom + MYGL_GAP_PNL*dScale)*(nTotPnl - 1 - k) + fData3;
				m_pReelMap->pPcsRgn[k][i].bottom = fData4;

				if (m_pReelMapDisp)
				{
					m_pReelMapDisp->pPcsRgn[k][i].left = (m_Master[0].m_pPcsRgn->rtFrm.bottom + MYGL_GAP_PNL*dScale)*(nTotPnl - 1 - k) + fData1;
					m_pReelMapDisp->pPcsRgn[k][i].top = fData2;
					m_pReelMapDisp->pPcsRgn[k][i].right = (m_Master[0].m_pPcsRgn->rtFrm.bottom + MYGL_GAP_PNL*dScale)*(nTotPnl - 1 - k) + fData3;
					m_pReelMapDisp->pPcsRgn[k][i].bottom = fData4;
				}
			}
			break;
		}
	}
}

CString CManagerReelmap::GetCamPxlRes()
{
	CString sRes = _T("");
	CString sPath;
#ifdef TEST_MODE
	sPath = PATH_PIN_IMG_;
#else
	if (pDoc->WorkingInfo.System.sPathCamSpecDir.Right(1) != "\\")
		sPath.Format(_T("%s\\%s\\%s.mst"), pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, pDoc->WorkingInfo.LastJob.sLayerUp);
	else
		sPath.Format(_T("%s%s\\%s.mst"), pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, pDoc->WorkingInfo.LastJob.sLayerUp);
#endif

	int nPos = sPath.ReverseFind('-');
	if (nPos > 0)
	{
		sRes = sPath.Right(sPath.GetLength() - (nPos + 1));
		nPos = sRes.ReverseFind('.');
		if (nPos > 0)
			sRes = sRes.Left(nPos);
		pDoc->WorkingInfo.Vision[0].sCamPxlRes = sRes;
		pDoc->WorkingInfo.Vision[1].sCamPxlRes = sRes;
	}
	return sRes;
}

int CManagerReelmap::GetPcrIdx(int nSerial, BOOL bNewLot)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.9"));
		return 0;
	}

	int nIdx;

	if (!bNewLot)
	{
		if (pDoc->WorkingInfo.LastJob.bLotSep || pDoc->m_bDoneChgLot)
		{
			if (!pDoc->m_bDoneChgLot && nSerial <= pDoc->m_nLotLastShot)
				nIdx = (nSerial - 1) % MAX_PCR_PNL;
			else // New Lot...
				nIdx = (pDoc->m_nLotLastShot + nSerial - 1) % MAX_PCR_PNL;
		}
		else
		{
			nIdx = (nSerial - 1) % MAX_PCR_PNL;
		}
	}
	else
	{
		if (pDoc->WorkingInfo.LastJob.bLotSep || pDoc->m_bDoneChgLot)
			nIdx = (pDoc->m_nLotLastShot + nSerial - 1) % MAX_PCR_PNL;
		else
			nIdx = (nSerial - 1) % MAX_PCR_PNL;
	}
	return nIdx;
}

int CManagerReelmap::GetPcrIdx0(int nSerial, BOOL bNewLot)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.9"));
		return 0;
	}

	int nIdx;

	if (!bNewLot)
	{
		if (pDoc->WorkingInfo.LastJob.bLotSep || pDoc->m_bDoneChgLot)
		{
			if (!pDoc->m_bDoneChgLot && nSerial <= pDoc->m_nLotLastShot)
				nIdx = (nSerial - 1) % MAX_PCR_PNL;
			else // New Lot...
				nIdx = (pDoc->m_nLotLastShot + nSerial - 1) % MAX_PCR_PNL;
		}
		else
		{
			nIdx = (nSerial - 1) % MAX_PCR_PNL;
		}
	}
	else
	{
		if (pDoc->WorkingInfo.LastJob.bLotSep || pDoc->m_bDoneChgLot)
			nIdx = (pDoc->m_nLotLastShot + nSerial - 1) % MAX_PCR_PNL;
		else
			nIdx = (nSerial - 1) % MAX_PCR_PNL;
	}
	return nIdx;
}

int CManagerReelmap::GetPcrIdx1(int nSerial, BOOL bNewLot)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.10"));
		return 0;
	}

	int nIdx;

	if (!bNewLot)
	{
		if (pDoc->WorkingInfo.LastJob.bLotSep || pDoc->m_bDoneChgLot)
		{
			if (!pDoc->m_bDoneChgLot && nSerial <= pDoc->m_nLotLastShot)
				nIdx = (nSerial - 1) % MAX_PCR_PNL;
			else // New Lot...
				nIdx = (pDoc->m_nLotLastShot + nSerial - 1) % MAX_PCR_PNL;
		}
		else
		{
			nIdx = (nSerial - 1) % MAX_PCR_PNL;
		}
	}
	else
	{
		if (pDoc->WorkingInfo.LastJob.bLotSep || pDoc->m_bDoneChgLot)
			nIdx = (pDoc->m_nLotLastShot + nSerial - 1) % MAX_PCR_PNL;
		else
			nIdx = (nSerial - 1) % MAX_PCR_PNL;
	}
	return nIdx;
}

BOOL CManagerReelmap::GetAoiUpInfo(int nSerial, int *pNewLot, BOOL bFromBuf) // TRUE: CHANGED, FALSE: NO CHANGED 
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.11"));
		return 0;
	}

	BOOL Info0;//, Info1;
	Info0 = GetAoiInfoUp(nSerial, pNewLot, bFromBuf);

	if (Info0)
		return TRUE;

	return FALSE;
}

BOOL CManagerReelmap::GetAoiDnInfo(int nSerial, int *pNewLot, BOOL bFromBuf) // TRUE: CHANGED, FALSE: NO CHANGED 
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.12"));
		return 0;
	}

	BOOL Info1;//Info0, 
	Info1 = GetAoiInfoDn(nSerial, pNewLot, bFromBuf);
	if (Info1)
		return TRUE;

	return FALSE;
}

BOOL CManagerReelmap::GetAoiInfoUp(int nSerial, int *pNewLot, BOOL bFromBuf) // TRUE: CHANGED, FALSE: NO CHANGED 
{

	FILE *fp;
	char FileD[200];
	size_t nFileSize, nRSize;
	char *FileData;
	CString strFileData;
	int nTemp;// , i;
	CString strHeaderErrorInfo, strModel, strLayer, strLot, sItsCode, strTotalBadPieceNum;
	CString strCamID, strPieceID, strBadPointPosX, strBadPointPosY, strBadName,
		strCellNum, strImageSize, strImageNum, strMarkingCode;

	if (nSerial < 1)
	{
		strFileData.Format(_T("PCR파일이 설정되지 않았습니다."));
		pView->MsgBox(strFileData);
		//AfxMessageBox(strFileData);
		return(FALSE);
	}

	CString sPath;

#ifdef TEST_MODE
	sPath = PATH_PCR;	// for Test
#else
	//	if(bFromBuf)
	sPath.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufUp, nSerial);
	// 	else
	// 		sPath.Format(_T("%s%04d.pcr"), WorkingInfo.System.sPathVrsShareUp, nSerial);
#endif

	//strcpy(FileD, sPath);
	//_tcscpy(FileD, sPath);
	StringToChar(sPath, FileD);

	if ((fp = fopen(FileD, "r")) != NULL)
	{
		fseek(fp, 0, SEEK_END);
		nFileSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		/* Allocate space for a path name */
		//FileData = (char*)malloc( nFileSize );
		FileData = (char*)calloc(nFileSize + 1, sizeof(char));

		nRSize = fread(FileData, sizeof(char), nFileSize, fp);
		//strFileData.Format(_T("%s"), CharToString(FileData));
		strFileData = CharToString(FileData);
		fclose(fp);
		free(FileData);
	}
	else
	{
		strFileData.Format(_T("PCR 파일이 존재하지 않습니다.\r\n%s"), sPath);
		pView->MsgBox(strFileData);
		return(FALSE);
	}

	// Error Code											// 1(정상), -1(Align Error, 노광불량), -2(Lot End)
	nTemp = strFileData.Find(',', 0);
	strHeaderErrorInfo = strFileData.Left(nTemp);
	strFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;
	// 	m_pPcr[nIdx]->m_nErrPnl = _tstoi(strHeaderErrorInfo);

	// Model
	nTemp = strFileData.Find(',', 0);
	strModel = strFileData.Left(nTemp);
	strFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;
	pDoc->Status.PcrShare[0].sModel = strModel;

	// Layer
	nTemp = strFileData.Find(',', 0);
	strLayer = strFileData.Left(nTemp);
	strFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;
	pDoc->Status.PcrShare[0].sLayer = strLayer;

	if (pDoc->WorkingInfo.System.bUseITS)
	{
		// Lot
		nTemp = strFileData.Find(',', 0);
		strLot = strFileData.Left(nTemp);
		strFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;
		pDoc->Status.PcrShare[0].sLot = strLot;

		// Its Code
		nTemp = strFileData.Find('\n', 0);
		sItsCode = strFileData.Left(nTemp);
		strFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;
		pDoc->Status.PcrShare[0].sItsCode = sItsCode;
		//m_pPcr[0][nIdx]->m_sItsCode = sItsCode;
	}
	else
	{
		// Lot
		nTemp = strFileData.Find(',', 0);
		strLot = strFileData.Left(nTemp);
		strFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;
		pDoc->Status.PcrShare[0].sLot = strLot;

		// Lot
		nTemp = strFileData.Find('\n', 0);
		strLot = strFileData.Left(nTemp);
		strFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;
		pDoc->Status.PcrShare[0].sLot = strLot;
	}

	nTemp = strFileData.Find('\n', 0);
	strTotalBadPieceNum = strFileData.Left(nTemp);;
	strFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;

	int nTotDef = _tstoi(strTotalBadPieceNum);



	if (pDoc->Status.PcrShare[0].sModel.IsEmpty() || pDoc->Status.PcrShare[0].sLayer.IsEmpty() || pDoc->Status.PcrShare[0].sLot.IsEmpty())
	{
		pView->MsgBox(_T("Error - Aoi Information."));
		// 		AfxMessageBox(_T("Error - Aoi Information."));
		return FALSE;
	}

	pDoc->m_bUpdateForNewJob[0] = FALSE;

	if (pDoc->WorkingInfo.LastJob.sLotUp != pDoc->Status.PcrShare[0].sLot || pDoc->WorkingInfo.LastJob.sEngItsCode != pDoc->Status.PcrShare[0].sItsCode)
	{
		pDoc->m_bUpdateForNewJob[0] = TRUE;
		pDoc->WorkingInfo.LastJob.sLotUp = pDoc->Status.PcrShare[0].sLot;
		m_sItsCode = pDoc->WorkingInfo.LastJob.sEngItsCode = pDoc->Status.PcrShare[0].sItsCode;
	}

	if (pDoc->WorkingInfo.LastJob.sModelUp != pDoc->Status.PcrShare[0].sModel 
		|| pDoc->WorkingInfo.LastJob.sLayerUp != pDoc->Status.PcrShare[0].sLayer 
		|| (pView->m_mgrProcedure && pView->m_mgrProcedure->m_bInitAutoLoadMstInfo))
	{
		pDoc->m_bUpdateForNewJob[0] = TRUE;
		pDoc->WorkingInfo.LastJob.sModelUp = pDoc->Status.PcrShare[0].sModel;
		pDoc->WorkingInfo.LastJob.sLayerUp = pDoc->Status.PcrShare[0].sLayer;

		if (pView->m_mgrProcedure && pView->m_mgrProcedure->m_bBufEmptyF[0])
		{
			if (!pView->m_mgrProcedure->m_bBufEmpty[0])
				pView->m_mgrProcedure->m_bBufEmptyF[0] = FALSE;

			pView->m_mgrProcedure->m_nAoiCamInfoStrPcs[0] = GetAoiUpCamMstInfo();
			if (pView->m_mgrProcedure->m_nAoiCamInfoStrPcs[0] > -1)
			{
				if ((pView->m_mgrProcedure->m_nAoiCamInfoStrPcs[0] == 1 ? TRUE : FALSE) != pDoc->WorkingInfo.System.bStripPcsRgnBin)
				{
					//if(m_nAoiCamInfoStrPcs[0])
					//	pView->MsgBox(_T("현재 마킹부는 일반 모드 인데, \r\n상면 AOI는 DTS 모드에서 검사를 진행하였습니다."));
					//else
					//	pView->MsgBox(_T("현재 마킹부는 DTS 모드 인데, \r\n상면 AOI는 일반 모드에서 검사를 진행하였습니다."));
					return FALSE;
				}
			}

			pView->m_mgrProcedure->m_bInitAutoLoadMstInfo = FALSE;
			return TRUE;
		}

		if (pView->m_mgrProcedure && pView->m_mgrProcedure->m_bInitAutoLoadMstInfo)
		{
			pView->m_mgrProcedure->m_bInitAutoLoadMstInfo = FALSE;
			return TRUE;
		}
	}
	  
	if (pDoc->m_bUpdateForNewJob[0])
	{
		pDoc->WriteChangedModel();

		if (m_pReelMapUp)
			m_pReelMapUp->ResetReelmapPath();

		if (pDoc->GetTestMode() == MODE_OUTER)
		{
			BOOL bDualTestInner;
			CString sLot, sLayerUp, sLayerDn, str;
			if (!GetItsSerialInfo(nSerial, bDualTestInner, sLot, sLayerUp, sLayerDn))
			{
				str.Format(_T("It is trouble to read GetItsSerialInfo()."));
				pView->MsgBox(str);
				return FALSE; // TRUE: CHANGED, FALSE: NO CHANGED 
			}

			if (m_pReelMapInnerUp)
				m_pReelMapInnerUp->ResetReelmapPath();

			if (bDualTestInner)
			{
				if (m_pReelMapInnerDn)
					m_pReelMapInnerDn->ResetReelmapPath();
				if (m_pReelMapInnerAllUp)
					m_pReelMapInnerAllUp->ResetReelmapPath();
				if (m_pReelMapInnerAllDn)
					m_pReelMapInnerAllDn->ResetReelmapPath();
			}
		}
	}

	//if (pDoc->m_bUpdateForNewJob[0])
	//{
	//	if (pView->m_pDlgMenu01)
	//		pView->m_pDlgMenu01->UpdateData();
	//}

	return FALSE; // TRUE: CHANGED, FALSE: NO CHANGED 
}

BOOL CManagerReelmap::GetAoiInfoDn(int nSerial, int *pNewLot, BOOL bFromBuf) // TRUE: CHANGED, FALSE: NO CHANGED 
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return TRUE;

	FILE *fp;
	char FileD[200];
	size_t nFileSize, nRSize;
	char *FileData;
	CString strFileData;
	int nTemp;// , i;
	CString strHeaderErrorInfo, strModel, strLayer, strLot, sItsCode, strTotalBadPieceNum;
	CString strCamID, strPieceID, strBadPointPosX, strBadPointPosY, strBadName,
		strCellNum, strImageSize, strImageNum, strMarkingCode;

	if (nSerial < 1)
	{
		strFileData.Format(_T("PCR파일이 설정되지 않았습니다."));
		pView->MsgBox(strFileData);
		//AfxMessageBox(strFileData);
		return(FALSE);
	}

	CString sPath;

#ifdef TEST_MODE
	sPath = PATH_PCR;	// for Test
#else
	// 	if(bFromBuf)
	sPath.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufDn, nSerial);
	// 	else
	// 		sPath.Format(_T("%s%04d.pcr"), WorkingInfo.System.sPathVrsShareDn, nSerial);
#endif

	//strcpy(FileD, sPath);
	//_tcscpy(FileD, sPath);
	StringToChar(sPath, FileD);

	if ((fp = fopen(FileD, "r")) != NULL)
	{
		fseek(fp, 0, SEEK_END);
		nFileSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		/* Allocate space for a path name */
		//FileData = (char*)malloc( nFileSize );
		FileData = (char*)calloc(nFileSize + 1, sizeof(char));

		nRSize = fread(FileData, sizeof(char), nFileSize, fp);
		strFileData.Format(_T("%s"), CharToString(FileData));
		fclose(fp);
		free(FileData);
	}
	else
	{
		strFileData.Format(_T("PCR 파일이 존재하지 않습니다.\r\n%s"), sPath);
		pView->MsgBox(strFileData);
		return(FALSE);
	}

	// Error Code											// 1(정상), -1(Align Error, 노광불량), -2(Lot End)
	nTemp = strFileData.Find(',', 0);
	strHeaderErrorInfo = strFileData.Left(nTemp);
	strFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;
	// 	m_pPcr[nIdx]->m_nErrPnl = _tstoi(strHeaderErrorInfo);

	// Model
	nTemp = strFileData.Find(',', 0);
	strModel = strFileData.Left(nTemp);
	strFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;
	pDoc->Status.PcrShare[1].sModel = strModel;

	// Layer
	nTemp = strFileData.Find(',', 0);
	strLayer = strFileData.Left(nTemp);
	strFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;
	pDoc->Status.PcrShare[1].sLayer = strLayer;

	if (pDoc->WorkingInfo.System.bUseITS)
	{
		// Lot
		nTemp = strFileData.Find(',', 0);
		strLot = strFileData.Left(nTemp);
		strFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;
		pDoc->Status.PcrShare[1].sLot = strLot;

		// Its Code
		nTemp = strFileData.Find('\n', 0);
		sItsCode = strFileData.Left(nTemp);
		strFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;
		pDoc->Status.PcrShare[1].sItsCode = sItsCode;
		//m_pPcr[1][nIdx]->m_sItsCode = sItsCode;
	}
	else
	{
		// Lot
		nTemp = strFileData.Find(',', 0);
		strLot = strFileData.Left(nTemp);
		strFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;
		pDoc->Status.PcrShare[1].sLot = strLot;

		// Lot
		nTemp = strFileData.Find('\n', 0);
		strLot = strFileData.Left(nTemp);
		strFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;
		pDoc->Status.PcrShare[1].sLot = strLot;
	}

	nTemp = strFileData.Find('\n', 0);
	strTotalBadPieceNum = strFileData.Left(nTemp);
	strFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;

	int nTotDef = _tstoi(strTotalBadPieceNum);



	if (pDoc->Status.PcrShare[1].sModel.IsEmpty() || pDoc->Status.PcrShare[1].sLayer.IsEmpty() || pDoc->Status.PcrShare[1].sLot.IsEmpty())
	{
		pView->MsgBox(_T("Error - Aoi Information."));
		// 		AfxMessageBox(_T("Error - Aoi Information."));
		return FALSE;
	}

	//BOOL bUpdate = FALSE;
	pDoc->m_bUpdateForNewJob[1] = FALSE;

	if (pDoc->WorkingInfo.LastJob.sLotDn != pDoc->Status.PcrShare[1].sLot || pDoc->WorkingInfo.LastJob.sEngItsCode != pDoc->Status.PcrShare[1].sItsCode)
	{
		//bUpdate = TRUE;
		pDoc->m_bUpdateForNewJob[1] = TRUE;
		pDoc->WorkingInfo.LastJob.sLotDn = pDoc->Status.PcrShare[1].sLot;
		m_sItsCode = pDoc->WorkingInfo.LastJob.sEngItsCode = pDoc->Status.PcrShare[1].sItsCode;
	}

	if (pDoc->WorkingInfo.LastJob.sModelDn != pDoc->Status.PcrShare[1].sModel || pDoc->WorkingInfo.LastJob.sLayerDn != pDoc->Status.PcrShare[1].sLayer)
	{
		//bUpdate = TRUE;
		pDoc->m_bUpdateForNewJob[1] = TRUE;
		pDoc->WorkingInfo.LastJob.sModelDn = pDoc->Status.PcrShare[1].sModel;
		pDoc->WorkingInfo.LastJob.sLayerDn = pDoc->Status.PcrShare[1].sLayer;

		if (pView->m_mgrProcedure && pView->m_mgrProcedure->m_bBufEmptyF[1])
		{
			if (!pView->m_mgrProcedure->m_bBufEmpty[1])
				pView->m_mgrProcedure->m_bBufEmptyF[1] = FALSE;

			pView->m_mgrProcedure->m_nAoiCamInfoStrPcs[1] = GetAoiDnCamMstInfo();
			if (pView->m_mgrProcedure->m_nAoiCamInfoStrPcs[1] > -1)
			{
				if ((pView->m_mgrProcedure->m_nAoiCamInfoStrPcs[1] == 1 ? TRUE : FALSE) != pDoc->WorkingInfo.System.bStripPcsRgnBin)
				{
					//if (m_nAoiCamInfoStrPcs[1])
					//	pView->MsgBox(_T("현재 마킹부는 일반 모드 인데, \r\n하면 AOI는 DTS 모드에서 검사를 진행하였습니다."));
					//else
					//	pView->MsgBox(_T("현재 마킹부는 DTS 모드 인데, \r\n하면 AOI는 일반 모드에서 검사를 진행하였습니다."));
					return FALSE;
				}
			}

			return TRUE;
		}
	}

	if (pDoc->m_bUpdateForNewJob[1])
	{
		pDoc->WriteChangedModel();

		//if (pView->m_pDlgMenu01)
		//	pView->m_pDlgMenu01->UpdateData();

		if (m_pReelMapDn)
			m_pReelMapDn->ResetReelmapPath();

		if (m_pReelMapAllUp)
			m_pReelMapAllUp->ResetReelmapPath();

		if (m_pReelMapAllDn)
			m_pReelMapAllDn->ResetReelmapPath();
	}

	return FALSE;
}

BOOL CManagerReelmap::LoadPcrFromBuf() // 프로그램 처음 시작시 버퍼에 저장된 파일을 릴맵에 최초로 업데이트 함.
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	CString str, sTemp;
	BOOL bRtn = FALSE;

	if (pView->m_mgrProcedure->ChkBufUp(pView->m_mgrProcedure->m_pBufSerial[0], pView->m_mgrProcedure->m_nBufTot[0]))
	{
		for (int i = 0; i < pView->m_mgrProcedure->m_nBufTot[0]; i++)
		{
			LoadPCRUp(pView->m_mgrProcedure->m_pBufSerial[0][i]);
			if (!bDualTest)
			{
				UpdateReelmap(pView->m_mgrProcedure->m_pBufSerial[0][i]);
				bRtn = TRUE;
			}
		}
	}

	if (bDualTest)
	{
		if (pView->m_mgrProcedure->ChkBufDn(pView->m_mgrProcedure->m_pBufSerial[1], pView->m_mgrProcedure->m_nBufTot[1]))
		{
			for (int i = 0; i < pView->m_mgrProcedure->m_nBufTot[1]; i++)
			{
				LoadPCRDn(pView->m_mgrProcedure->m_pBufSerial[1][i]);
				UpdateReelmap(pView->m_mgrProcedure->m_pBufSerial[1][i]); // After inspect bottom side.
				bRtn = TRUE;
			}
		}
	}

	return bRtn;
}

int CManagerReelmap::LoadPCR0(int nSerial, BOOL bFromShare)	// return : 2(Failed), 1(정상), -1(Align Error, 노광불량), -2(Lot End)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.14"));
		return 0;
	}

	int nRtn[2] = { 1 };
	nRtn[0] = LoadPCRUp(nSerial, bFromShare);

	if (nRtn[0] != 1)
		return nRtn[0];

	if (pDoc->GetTestMode() == MODE_OUTER)
	{
		int nRtnInner[5] = { 1 };
		nRtnInner[0] = LoadPCRUpInner(nSerial, bFromShare);
		nRtnInner[1] = LoadPCRDnInner(nSerial, bFromShare);
		nRtnInner[2] = LoadPCRAllUpInner(nSerial, bFromShare);
		nRtnInner[3] = LoadPCRAllDnInner(nSerial, bFromShare);

		if (nRtnInner[0] != 1)
			return nRtnInner[0];
		if (nRtnInner[1] != 1)
			return nRtnInner[1];
		if (nRtnInner[2] != 1)
			return nRtnInner[2];
		if (nRtnInner[3] != 1)
			return nRtnInner[3];

		if (!bDualTest)
		{
			nRtnInner[4] = LoadPCRIts(nSerial, bFromShare);
			if (nRtnInner[4] != 1)
				return nRtnInner[4];
		}
	}

	return (1); // 1(정상)
}

int CManagerReelmap::LoadPCR1(int nSerial, BOOL bFromShare)	// return : 2(Failed), 1(정상), -1(Align Error, 노광불량), -2(Lot End)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.15"));
		return 0;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return 0;

	int nRtn[4] = { 1 };
	nRtn[0] = LoadPCRDn(nSerial, bFromShare);
	nRtn[1] = LoadPCRAllDn(nSerial, bFromShare);
	nRtn[2] = LoadPCRAllUp(nSerial, bFromShare);

	if (nRtn[0] != 1)
		return nRtn[0];
	if (nRtn[1] != 1)
		return nRtn[1];
	if (nRtn[2] != 1)
		return nRtn[2];

	if (pDoc->GetTestMode() == MODE_OUTER)
	{
		nRtn[3] = LoadPCRIts(nSerial, bFromShare);
		if (nRtn[3] != 1)
			return nRtn[3];
	}

	return (1); // 1(정상)
}

int CManagerReelmap::LoadPCRAllUp(int nSerial, BOOL bFromShare)	// return : 2(Failed), 1(정상), -1(Align Error, 노광불량), -2(Lot End)
{
#ifdef TEST_MODE
	return 0;
#endif

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return 0;

	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.16"));
		return 0;
	}

	int i, idx;//, k
	CString str;

	if (nSerial < 0)
	{
		str.Format(_T("PCR파일이 설정되지 않았습니다."));
		pView->MsgBox(str);
		//AfxMessageBox(strFileData);
		return(2);
	}

	if (!m_pPcr[2])
	{
		str.Format(_T("PCR[2]관련 메모리가 할당되지 않았습니다."));
		pView->MsgBox(str);
		//AfxMessageBox(strFileData);
		return(2);
	}

	int nIdx;
	if (pDoc->m_bNewLotShare[0] && (pDoc->WorkingInfo.LastJob.bLotSep || pDoc->m_bDoneChgLot))
		nIdx = GetPcrIdx0(nSerial, TRUE); // 릴맵화면 표시 인덱스
	else
		nIdx = GetPcrIdx0(nSerial);

	if (!m_pPcr[0] || !m_pPcr[1] || !m_pPcr[2])
		return(2);
	if (!m_pPcr[0][nIdx] || !m_pPcr[1][nIdx] || !m_pPcr[2][nIdx])
		return(2);

	m_pPcr[2][nIdx]->m_nIdx = nIdx;							// m_nIdx : From 0 to nTot.....
	m_pPcr[2][nIdx]->m_nSerial = nSerial;

	// Error Code											// 1(정상), -1(Align Error, 노광불량), -2(Lot End)
	if (m_pPcr[0][nIdx]->m_nErrPnl < 0)
		m_pPcr[2][nIdx]->m_nErrPnl = m_pPcr[0][nIdx]->m_nErrPnl;
	else if (m_pPcr[1][nIdx]->m_nErrPnl < 0)
		m_pPcr[2][nIdx]->m_nErrPnl = m_pPcr[1][nIdx]->m_nErrPnl;
	else
		m_pPcr[2][nIdx]->m_nErrPnl = m_pPcr[0][nIdx]->m_nErrPnl;

	// Model
	m_pPcr[2][nIdx]->m_sModel = m_pPcr[0][nIdx]->m_sModel;

	// Layer
	m_pPcr[2][nIdx]->m_sLayer = m_pPcr[0][nIdx]->m_sLayer;

	// Lot
	m_pPcr[2][nIdx]->m_sLot = m_pPcr[0][nIdx]->m_sLot;

	int nTotDef[3] = { 0 };									// [0]: 상면, [1]: 하면, [2]: 상/하면 Merge
	nTotDef[0] = m_pPcr[0][nIdx]->m_nTotDef;			// 상면 불량 피스 수
	nTotDef[1] = m_pPcr[1][nIdx]->m_nTotDef;			// 하면 불량 피스 수

	int nTotPcs = m_Master[0].m_pPcsRgn->nTotPcs;
	stPcrMerge *pPcrMgr = new stPcrMerge[nTotPcs];

	int nPcsId;												// nPcsId : CamMaster Pcs Index
															//pPcrMgr테이블의 nIdx에 하면의 불량을 먼저 기록하고 상면의 불량을 엎어서 최종 merge불량 테이블을 만듬.
	for (i = 0; i < nTotDef[1]; i++)						// 하면 불량 피스 수
	{
		nPcsId = m_pPcr[1][nIdx]->m_pDefPcs[i];				// nPcsId : CamMaster Pcs Index
		pPcrMgr[nPcsId].nIdx = i;	// Up+Dn				// 상하면 Merge [nPcsId:CamMaster Pcs Index]의 nIdx (0~)는 불량표시순서임.
		pPcrMgr[nPcsId].nIdxDn = i; // Dn					// 상하면 Merge [nPcsId:CamMaster Pcs Index]의 nIdxDn (0~)는 불량표시순서임.
	}
	for (i = 0; i < nTotDef[0]; i++)						// 상면 불량 피스 수
	{
		nPcsId = m_pPcr[0][nIdx]->m_pDefPcs[i];				// nPcsId : CamMaster Pcs Index
		pPcrMgr[nPcsId].nIdx = i;	// Up+Dn				// 상하면 Merge [nPcsId:CamMaster Pcs Index]의 nIdx (0~)는 불량표시순서임.
		pPcrMgr[nPcsId].nIdxUp = i;	// Up					// 상하면 Merge [nPcsId:CamMaster Pcs Index]의 nIdxUp (0~)는 불량표시순서임.
	}


	nTotDef[2] = 0;
	for (i = 0; i < nTotPcs; i++)							// Shot내 총 Piece수
	{
		if (pPcrMgr[i].nIdx > -1)	// Up+Dn				// 상하면 Merge [i:CamMaster Pcs Index]의 nIdx (0~)는 불량표시순서임.
			nTotDef[2]++;									// 상 / 하면 Merge한 총 불량피스수.
	}

	m_pPcr[2][nIdx]->Init(nSerial, nTotDef[2]);				// 제품시리얼, Shot내 총불량 피스수

	int nId[2], Ord; // [0]: 상면 0~불량피스순서, [1]: 하면 0~불량피스순서
	idx = 0; // 마킹순서 0~불량피스수만큼 정하기위해 현시점의 idx를 초기화함.
	if (nTotDef[2] > 0) // 상 / 하면 Merge한 총 불량피스수.
	{
		for (nPcsId = 0; nPcsId < nTotPcs; nPcsId++)
		{
			nId[0] = pPcrMgr[nPcsId].nIdxUp; // 상하면 Merge [nPcsId:CamMaster Pcs Index]의 nIdxUp (0~)는 불량표시순서 임. nId[0]: 상면에서의 PCR파일순서 인덱스
			nId[1] = pPcrMgr[nPcsId].nIdxDn; // 상하면 Merge [nPcsId:CamMaster Pcs Index]의 nIdxDn (0~)는 불량표시순서 임. nId[1]: 하면에서의 PCR파일순서 인덱스

			if (nId[0] > -1)
			{
				Ord = nId[0];
				// Cam ID
				m_pPcr[2][nIdx]->m_nCamId = m_pPcr[0][nIdx]->m_nCamId;
				// Piece Number
				m_pPcr[2][nIdx]->m_pDefPcs[idx] = m_pPcr[0][nIdx]->m_pDefPcs[Ord];
				m_pPcr[2][nIdx]->m_pLayer[idx] = m_pPcr[0][nIdx]->m_pLayer[Ord];
				// BadPointPosX
				m_pPcr[2][nIdx]->m_pDefPos[idx].x = m_pPcr[0][nIdx]->m_pDefPos[Ord].x;
				// BadPointPosY
				m_pPcr[2][nIdx]->m_pDefPos[idx].y = m_pPcr[0][nIdx]->m_pDefPos[Ord].y;
				// BadName
				m_pPcr[2][nIdx]->m_pDefType[idx] = m_pPcr[0][nIdx]->m_pDefType[Ord];
				// CellNum
				m_pPcr[2][nIdx]->m_pCell[idx] = m_pPcr[0][nIdx]->m_pCell[Ord];
				// ImageSize
				m_pPcr[2][nIdx]->m_pImgSz[idx] = m_pPcr[0][nIdx]->m_pImgSz[Ord];
				// ImageNum
				m_pPcr[2][nIdx]->m_pImg[idx] = m_pPcr[0][nIdx]->m_pImg[Ord];
				// strMarkingCode : -2 (NoMarking)
				m_pPcr[2][nIdx]->m_pMk[idx] = m_pPcr[0][nIdx]->m_pMk[Ord];

				idx++;
			}
			else if (nId[1] > -1)
			{
				Ord = nId[1];
				// Cam ID
				m_pPcr[2][nIdx]->m_nCamId = m_pPcr[1][nIdx]->m_nCamId;
				// Piece Number
				m_pPcr[2][nIdx]->m_pDefPcs[idx] = m_pPcr[1][nIdx]->m_pDefPcs[Ord];
				m_pPcr[2][nIdx]->m_pLayer[idx] = m_pPcr[1][nIdx]->m_pLayer[Ord];
				// BadPointPosX
				m_pPcr[2][nIdx]->m_pDefPos[idx].x = m_pPcr[1][nIdx]->m_pDefPos[Ord].x;
				// BadPointPosY
				m_pPcr[2][nIdx]->m_pDefPos[idx].y = m_pPcr[1][nIdx]->m_pDefPos[Ord].y;
				// BadName
				m_pPcr[2][nIdx]->m_pDefType[idx] = m_pPcr[1][nIdx]->m_pDefType[Ord];
				// CellNum
				m_pPcr[2][nIdx]->m_pCell[idx] = m_pPcr[1][nIdx]->m_pCell[Ord];
				// ImageSize
				m_pPcr[2][nIdx]->m_pImgSz[idx] = m_pPcr[1][nIdx]->m_pImgSz[Ord];
				// ImageNum
				m_pPcr[2][nIdx]->m_pImg[idx] = m_pPcr[1][nIdx]->m_pImg[Ord];
				// strMarkingCode : -2 (NoMarking)
				m_pPcr[2][nIdx]->m_pMk[idx] = m_pPcr[1][nIdx]->m_pMk[Ord];

				idx++;
			}
		}
	}

	delete[] pPcrMgr;

	return (1); // 1(정상)
}

int CManagerReelmap::LoadPCRAllDn(int nSerial, BOOL bFromShare)	// return : 2(Failed), 1(정상), -1(Align Error, 노광불량), -2(Lot End)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return 1;

	int i, idx;//k, 
	CString str;

	if (nSerial <= 0)
	{
		str.Format(_T("PCR파일이 설정되지 않았습니다."));
		pView->MsgBox(str);
		//AfxMessageBox(strFileData);
		return(2);
	}

	if (!m_pPcr[3])
	{
		str.Format(_T("PCR[3]관련 메모리가 할당되지 않았습니다."));
		pView->MsgBox(str);
		//AfxMessageBox(strFileData);
		return(2);
	}

	int nIdx;
	if (pDoc->m_bNewLotShare[1] && (pDoc->WorkingInfo.LastJob.bLotSep || pDoc->m_bDoneChgLot))
		nIdx = GetPcrIdx1(nSerial, TRUE);
	else
		nIdx = GetPcrIdx1(nSerial);

	if (!m_pPcr[0] || !m_pPcr[1] || !m_pPcr[3])
		return(2);
	if (!m_pPcr[0][nIdx] || !m_pPcr[1][nIdx] || !m_pPcr[3][nIdx])
		return(2);

	m_pPcr[3][nIdx]->m_nIdx = nIdx;							// m_nIdx : From 0 to nTot.....
	m_pPcr[3][nIdx]->m_nSerial = nSerial;

	// Error Code											// 1(정상), -1(Align Error, 노광불량), -2(Lot End)
	if (m_pPcr[1][nIdx]->m_nErrPnl < 0)
		m_pPcr[3][nIdx]->m_nErrPnl = m_pPcr[1][nIdx]->m_nErrPnl;
	else if (m_pPcr[0][nIdx]->m_nErrPnl < 0)
		m_pPcr[3][nIdx]->m_nErrPnl = m_pPcr[0][nIdx]->m_nErrPnl;
	else
		m_pPcr[3][nIdx]->m_nErrPnl = m_pPcr[1][nIdx]->m_nErrPnl;

	// Model
	m_pPcr[3][nIdx]->m_sModel = m_pPcr[1][nIdx]->m_sModel;

	// Layer
	m_pPcr[3][nIdx]->m_sLayer = m_pPcr[1][nIdx]->m_sLayer;

	// Lot
	m_pPcr[3][nIdx]->m_sLot = m_pPcr[1][nIdx]->m_sLot;

	int nTotDef[3];
	nTotDef[0] = m_pPcr[0][nIdx]->m_nTotDef;
	nTotDef[1] = m_pPcr[1][nIdx]->m_nTotDef;
	//	nTotDef[2] = nTotDef[0] + nTotDef[1];

	int nTotPcs = m_Master[0].m_pPcsRgn->nTotPcs;
	stPcrMerge *pPcrMgr = new stPcrMerge[nTotPcs];

	int nComp, nPcsId;
	if (nTotDef[0] > nTotDef[1])
		nComp = nTotDef[0];
	else
		nComp = nTotDef[1];

	for (i = 0; i < nTotDef[0]; i++)
	{
		nPcsId = m_pPcr[0][nIdx]->m_pDefPcs[i];
		pPcrMgr[nPcsId].nIdx = i;
		pPcrMgr[nPcsId].nIdxUp = i;	// Up
	}
	for (i = 0; i < nTotDef[1]; i++)
	{
		nPcsId = m_pPcr[1][nIdx]->m_pDefPcs[i];
		pPcrMgr[nPcsId].nIdx = i;
		pPcrMgr[nPcsId].nIdxDn = i; // Dn
	}

	nTotDef[2] = 0;
	for (i = 0; i < nTotPcs; i++)
	{
		if (pPcrMgr[i].nIdx > -1)
			nTotDef[2]++;
	}

	m_pPcr[3][nIdx]->Init(nSerial, nTotDef[2]);

	int nId[2], Ord;
	idx = 0;
	if (nTotDef[2] > 0)
	{
		for (nPcsId = 0; nPcsId < nTotPcs; nPcsId++)
		{
			nId[0] = pPcrMgr[nPcsId].nIdxUp;
			nId[1] = pPcrMgr[nPcsId].nIdxDn;

			if (nId[1] > -1)
			{
				Ord = nId[1];
				// Cam ID
				m_pPcr[3][nIdx]->m_nCamId = m_pPcr[1][nIdx]->m_nCamId;
				// Piece Number
				m_pPcr[3][nIdx]->m_pDefPcs[idx] = m_pPcr[1][nIdx]->m_pDefPcs[Ord];
				m_pPcr[3][nIdx]->m_pLayer[idx] = m_pPcr[1][nIdx]->m_pLayer[Ord];
				// BadPointPosX
				m_pPcr[3][nIdx]->m_pDefPos[idx].x = m_pPcr[1][nIdx]->m_pDefPos[Ord].x;
				// BadPointPosY
				m_pPcr[3][nIdx]->m_pDefPos[idx].y = m_pPcr[1][nIdx]->m_pDefPos[Ord].y;
				// BadName
				m_pPcr[3][nIdx]->m_pDefType[idx] = m_pPcr[1][nIdx]->m_pDefType[Ord];
				// CellNum
				m_pPcr[3][nIdx]->m_pCell[idx] = m_pPcr[1][nIdx]->m_pCell[Ord];
				// ImageSize
				m_pPcr[3][nIdx]->m_pImgSz[idx] = m_pPcr[1][nIdx]->m_pImgSz[Ord];
				// ImageNum
				m_pPcr[3][nIdx]->m_pImg[idx] = m_pPcr[1][nIdx]->m_pImg[Ord];
				// strMarkingCode : -2 (NoMarking)
				m_pPcr[3][nIdx]->m_pMk[idx] = m_pPcr[1][nIdx]->m_pMk[Ord];

				idx++;
			}
			else if (nId[0] > -1)
			{
				Ord = nId[0];
				// Cam ID
				m_pPcr[3][nIdx]->m_nCamId = m_pPcr[0][nIdx]->m_nCamId;
				// Piece Number
				m_pPcr[3][nIdx]->m_pDefPcs[idx] = m_pPcr[0][nIdx]->m_pDefPcs[Ord];
				m_pPcr[3][nIdx]->m_pLayer[idx] = m_pPcr[0][nIdx]->m_pLayer[Ord];
				// BadPointPosX
				m_pPcr[3][nIdx]->m_pDefPos[idx].x = m_pPcr[0][nIdx]->m_pDefPos[Ord].x;
				// BadPointPosY
				m_pPcr[3][nIdx]->m_pDefPos[idx].y = m_pPcr[0][nIdx]->m_pDefPos[Ord].y;
				// BadName
				m_pPcr[3][nIdx]->m_pDefType[idx] = m_pPcr[0][nIdx]->m_pDefType[Ord];
				// CellNum
				m_pPcr[3][nIdx]->m_pCell[idx] = m_pPcr[0][nIdx]->m_pCell[Ord];
				// ImageSize
				m_pPcr[3][nIdx]->m_pImgSz[idx] = m_pPcr[0][nIdx]->m_pImgSz[Ord];
				// ImageNum
				m_pPcr[3][nIdx]->m_pImg[idx] = m_pPcr[0][nIdx]->m_pImg[Ord];
				// strMarkingCode : -2 (NoMarking)
				m_pPcr[3][nIdx]->m_pMk[idx] = m_pPcr[0][nIdx]->m_pMk[Ord];

				idx++;
			}
		}
	}

	delete[] pPcrMgr;

	return (1); // 1(정상)
}

int CManagerReelmap::LoadPCRUp(int nSerial, BOOL bFromShare)	// return : 2(Failed), 1(정상), -1(Align Error, 노광불량), -2(Lot End)
{
	FILE *fp;
	char FileD[200];
	size_t nFileSize, nRSize;
	char *FileData;
	CString strFileData;
	int nTemp, i, nC, nR;
	CString strHeaderErrorInfo, strModel, strLayer, strLot, sItsCode, strTotalBadPieceNum;
	CString strCamID, strPieceID, strBadPointPosX, strBadPointPosY, strBadName,
		strCellNum, strImageSize, strImageNum, strMarkingCode;

	if (nSerial <= 0)
	{
		strFileData.Format(_T("PCR파일이 설정되지 않았습니다."));
		pView->MsgBox(strFileData);
		//AfxMessageBox(strFileData);
		return(2);
	}

	if (!m_pPcr[0])
	{
		strFileData.Format(_T("PCR[0]관련 메모리가 할당되지 않았습니다."));
		pView->MsgBox(strFileData);
		//AfxMessageBox(strFileData);
		return(2);
	}

	int nIdx;
	if (pDoc->m_bNewLotShare[0] && (pDoc->WorkingInfo.LastJob.bLotSep || pDoc->m_bDoneChgLot))
		nIdx = GetPcrIdx0(nSerial, TRUE);
	else
		nIdx = GetPcrIdx0(nSerial);

	CString sPath;

#ifdef TEST_MODE
	sPath = PATH_PCR;	// for Test
#else
	if (bFromShare)
		sPath.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsShareUp, nSerial);
	else
		sPath.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufUp, nSerial);
#endif

	//strcpy(FileD, sPath);
	//_tcscpy(FileD, sPath);
	StringToChar(sPath, FileD);

	if ((fp = fopen(FileD, "r")) != NULL)
	{
		fseek(fp, 0, SEEK_END);
		nFileSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		/* Allocate space for a path name */
		//FileData = (char*)malloc( nFileSize );
		FileData = (char*)calloc(nFileSize + 1, sizeof(char));

		nRSize = fread(FileData, sizeof(char), nFileSize, fp);
		//strFileData.Format(_T("%s"), CharToString(FileData));
		strFileData = CharToString(FileData);
		fclose(fp);
		free(FileData);
	}
	else
	{
		strFileData.Format(_T("PCR[Up] 파일이 존재하지 않습니다.\r\n%s"), sPath);
		pView->MsgBox(strFileData);
		//AfxMessageBox(strFileData);
		return(2);
	}

	if (!m_pPcr[0])
		return(2);
	if (!m_pPcr[0][nIdx])
		return(2);

	BOOL bResetMkInfo = FALSE;

	m_pPcr[0][nIdx]->m_nIdx = nIdx;							// m_nIdx : From 0 to nTot.....
	m_pPcr[0][nIdx]->m_nSerial = nSerial;

	// Error Code											// 1(정상), -1(Align Error, 노광불량), -2(Lot End)
	nTemp = strFileData.Find(',', 0);
	strHeaderErrorInfo = strFileData.Left(nTemp);
	strFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;
	m_pPcr[0][nIdx]->m_nErrPnl = _tstoi(strHeaderErrorInfo);

	if (m_pPcrInner[1][nIdx]->m_nErrPnl == -1)
	{
		int syd = 1;
	}

	// Model
	nTemp = strFileData.Find(',', 0);
	strModel = strFileData.Left(nTemp);
	strFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;
	m_pPcr[0][nIdx]->m_sModel = strModel;

	// Layer
	nTemp = strFileData.Find(',', 0);
	strLayer = strFileData.Left(nTemp);
	strFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;
	m_pPcr[0][nIdx]->m_sLayer = strLayer;

	if (pDoc->WorkingInfo.System.bUseITS)
	{
		// Lot
		nTemp = strFileData.Find(',', 0);
		strLot = strFileData.Left(nTemp);
		strFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;
		m_pPcr[0][nIdx]->m_sLot = strLot;

		// Its Code
		nTemp = strFileData.Find('\n', 0);
		sItsCode = strFileData.Left(nTemp);
		strFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;
		m_pPcr[0][nIdx]->m_sItsCode = sItsCode;
	}
	else
	{
		// Lot
		nTemp = strFileData.Find(',', 0);
		strLot = strFileData.Left(nTemp);
		strFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;
		m_pPcr[0][nIdx]->m_sLot = strLot;

		// Lot
		nTemp = strFileData.Find('\n', 0);
		strLot = strFileData.Left(nTemp);
		strFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;
		m_pPcr[0][nIdx]->m_sLot = strLot;
	}

	nTemp = strFileData.Find('\n', 0);
	strTotalBadPieceNum = strFileData.Left(nTemp);;
	strFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;

	if (pDoc->GetTestMode() == MODE_INNER || pDoc->GetTestMode() == MODE_OUTER)
	{
		m_sEngModel = strModel;
		m_sEngLotNum = strLot;
		m_sEngLayerUp = strLayer;
		//pDoc->m_sEngLayerDn = strModel;
		if (pDoc->WorkingInfo.System.bUseITS)
			m_sItsCode = sItsCode;
	}

	//BOOL bUpdate = FALSE;
	pDoc->m_bUpdateForNewJob[0] = FALSE;

	if (pDoc->WorkingInfo.LastJob.sLotUp != strLot || pDoc->WorkingInfo.LastJob.sEngItsCode != sItsCode)
	{
		pDoc->m_bUpdateForNewJob[0] = TRUE;
		pDoc->WorkingInfo.LastJob.sLotUp = strLot;
		m_sItsCode = pDoc->WorkingInfo.LastJob.sEngItsCode = sItsCode;
	}

	if (pDoc->WorkingInfo.LastJob.sModelUp != strModel || pDoc->WorkingInfo.LastJob.sLayerUp != strLayer)
	{
		pDoc->m_bUpdateForNewJob[0] = TRUE;
		pDoc->WorkingInfo.LastJob.sModelUp = strModel;
		pDoc->WorkingInfo.LastJob.sLayerUp = strLayer;

		pView->ResetMkInfo(0); // CAD 데이터 리로딩   0 : AOI-Up , 1 : AOI-Dn , 2 : AOI-UpDn
	}

	if (pDoc->m_bUpdateForNewJob[0])
	{
		pDoc->WriteChangedModel();

		if (pDoc->GetTestMode() == MODE_OUTER)
		{
			BOOL bDualTestInner;
			CString sLot, sLayerUp, sLayerDn, str;
			if (!GetItsSerialInfo(nSerial, bDualTestInner, sLot, sLayerUp, sLayerDn))
			{
				str.Format(_T("It is trouble to read GetItsSerialInfo()."));
				pView->MsgBox(str);
				return FALSE; // TRUE: CHANGED, FALSE: NO CHANGED 
			}

			if (m_pReelMapInnerUp)
				m_pReelMapInnerUp->ResetReelmapPath();

			if (m_pReelMapIts)
				m_pReelMapIts->ResetReelmapPath();

			if (bDualTestInner)
			{
				if (m_pReelMapInnerDn)
					m_pReelMapInnerDn->ResetReelmapPath();
				if (m_pReelMapInnerAllUp)
					m_pReelMapInnerAllUp->ResetReelmapPath();
				if (m_pReelMapInnerAllDn)
					m_pReelMapInnerAllDn->ResetReelmapPath();
			}
		}
	}

	int nTotDef = _tstoi(strTotalBadPieceNum);

	m_pPcr[0][nIdx]->Init(nSerial, nTotDef);

	if (nTotDef > 0)
	{
		for (i = 0; i < nTotDef; i++)
		{
			// Cam ID
			nTemp = strFileData.Find(',', 0);
			strCamID = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			m_pPcr[0][nIdx]->m_nCamId = _tstoi(strCamID);

			// Piece Number
			nTemp = strFileData.Find(',', 0);
			strPieceID = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;

			// LoadStripPieceRegion_Binary()에 의해 PCS Index가 결정됨.
			if (pDoc->WorkingInfo.System.bStripPcsRgnBin)	// DTS용
			{
				m_pPcr[0][nIdx]->m_pDefPcs[i] = _tstoi(strPieceID);

				//switch (m_Master[0].MasterInfo.nActionCode)	// 0 : Rotation / Mirror 적용 없음(CAM Data 원본), 1 : 좌우 미러, 2 : 상하 미러, 3 : 180 회전, 4 : 270 회전(CCW), 5 : 90 회전(CW)
				//{
				//case 0:
				//	m_pPcr[0][nIdx]->m_pDefPcs[i] = _tstoi(strPieceID);
				//	break;
				//case 1:
				//	m_pPcr[0][nIdx]->m_pDefPcs[i] = MirrorLR(_tstoi(strPieceID));
				//	break;
				//case 3:
				//	m_pPcr[0][nIdx]->m_pDefPcs[i] = Rotate180(_tstoi(strPieceID));
				//	break;
				//default:
				//	m_pPcr[0][nIdx]->m_pDefPcs[i] = _tstoi(strPieceID);
				//	break;
				//}
			}
			else
				m_pPcr[0][nIdx]->m_pDefPcs[i] = _tstoi(strPieceID);

			m_pPcr[0][nIdx]->m_pLayer[i] = 0; // Up

											  // BadPointPosX
			nTemp = strFileData.Find(',', 0);
			strBadPointPosX = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			m_pPcr[0][nIdx]->m_pDefPos[i].x = (long)_tstoi(strBadPointPosX);

			// BadPointPosY
			nTemp = strFileData.Find(',', 0);
			strBadPointPosY = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			m_pPcr[0][nIdx]->m_pDefPos[i].y = (long)_tstoi(strBadPointPosY);

			// BadName
			nTemp = strFileData.Find(',', 0);
			strBadName = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			m_pPcr[0][nIdx]->m_pDefType[i] = _tstoi(strBadName);

			m_Master[0].m_pPcsRgn->GetMkMatrix(m_pPcr[0][nIdx]->m_pDefPcs[i], nC, nR);
			m_pPcr[0][nIdx]->m_arDefType[nR][nC] = m_pPcr[0][nIdx]->m_pDefType[i];

			// CellNum
			nTemp = strFileData.Find(',', 0);
			strCellNum = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			m_pPcr[0][nIdx]->m_pCell[i] = _tstoi(strCellNum);

			// ImageSize
			nTemp = strFileData.Find(',', 0);
			strImageSize = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			m_pPcr[0][nIdx]->m_pImgSz[i] = _tstoi(strImageSize);

			// ImageNum
			nTemp = strFileData.Find(',', 0);
			strImageNum = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			m_pPcr[0][nIdx]->m_pImg[i] = _tstoi(strImageNum);

			// strMarkingCode : -2 (NoMarking)
			nTemp = strFileData.Find('\n', 0);
			strMarkingCode = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			m_pPcr[0][nIdx]->m_pMk[i] = _tstoi(strMarkingCode);
		}
	}

	return (1); // 1(정상)
				// 	return(m_pPcr[0][nIdx]->m_nErrPnl);
}

int CManagerReelmap::LoadPCRDn(int nSerial, BOOL bFromShare)	// return : 2(Failed), 1(정상), -1(Align Error, 노광불량), -2(Lot End)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return 1;

	FILE *fp;
	char FileD[200];
	size_t nFileSize, nRSize;
	char *FileData;
	CString strFileData;
	int nTemp, i, nC, nR;
	CString strHeaderErrorInfo, strModel, strLayer, strLot, sItsCode, strTotalBadPieceNum;
	CString strCamID, strPieceID, strBadPointPosX, strBadPointPosY, strBadName,
		strCellNum, strImageSize, strImageNum, strMarkingCode;

	if (nSerial <= 0)
	{
		strFileData.Format(_T("PCR파일이 설정되지 않았습니다."));
		pView->MsgBox(strFileData);
		// 		AfxMessageBox(strFileData);
		return(2);
	}

	if (!m_pPcr[1])
	{
		strFileData.Format(_T("PCR[1]관련 메모리가 할당되지 않았습니다."));
		pView->MsgBox(strFileData);
		// 		AfxMessageBox(strFileData);
		return(2);
	}

	int nIdx;
	if (pDoc->m_bNewLotShare[1] && (pDoc->WorkingInfo.LastJob.bLotSep || pDoc->m_bDoneChgLot))
		nIdx = GetPcrIdx1(nSerial, TRUE);
	else
		nIdx = GetPcrIdx1(nSerial);
	CString sPath;

#ifdef TEST_MODE
	sPath = PATH_PCR;	// for Test
#else
	if (bFromShare)
		sPath.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsShareDn, nSerial);
	else
		sPath.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufDn, nSerial);
#endif

	//strcpy(FileD, sPath);
	//_tcscpy(FileD, sPath);
	StringToChar(sPath, FileD);

	if ((fp = fopen(FileD, "r")) != NULL)
	{
		fseek(fp, 0, SEEK_END);
		nFileSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		/* Allocate space for a path name */
		//FileData = (char*)malloc( nFileSize );
		FileData = (char*)calloc(nFileSize + 1, sizeof(char));

		nRSize = fread(FileData, sizeof(char), nFileSize, fp);
		//strFileData.Format(_T("%s"), CharToString(FileData));
		strFileData = CharToString(FileData);
		fclose(fp);
		free(FileData);
	}
	else
	{
		strFileData.Format(_T("PCR[Dn] 파일이 존재하지 않습니다.\r\n%s"), sPath);
		pView->MsgBox(strFileData);
		//		AfxMessageBox(strFileData);
		return(2);
	}

	if (!m_pPcr[1])
		return(2);
	if (!m_pPcr[1][nIdx])
		return(2);

	m_pPcr[1][nIdx]->m_nIdx = nIdx;							// m_nIdx : From 0 to nTot.....
	m_pPcr[1][nIdx]->m_nSerial = nSerial;

	// Error Code											// 1(정상), -1(Align Error, 노광불량), -2(Lot End)
	nTemp = strFileData.Find(',', 0);
	strHeaderErrorInfo = strFileData.Left(nTemp);
	strFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;
	m_pPcr[1][nIdx]->m_nErrPnl = _tstoi(strHeaderErrorInfo);

	if (m_pPcrInner[1][nIdx]->m_nErrPnl == -1)
	{
		int syd = 1;
	}


	// Model
	nTemp = strFileData.Find(',', 0);
	strModel = strFileData.Left(nTemp);
	strFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;
	m_pPcr[1][nIdx]->m_sModel = strModel;

	// Layer
	nTemp = strFileData.Find(',', 0);
	strLayer = strFileData.Left(nTemp);
	strFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;
	m_pPcr[1][nIdx]->m_sLayer = strLayer;

	if (pDoc->WorkingInfo.System.bUseITS)
	{
		// Lot
		nTemp = strFileData.Find(',', 0);
		strLot = strFileData.Left(nTemp);
		strFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;
		m_pPcr[1][nIdx]->m_sLot = strLot;

		// Its Code
		nTemp = strFileData.Find('\n', 0);
		sItsCode = strFileData.Left(nTemp);
		strFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;
		m_pPcr[1][nIdx]->m_sItsCode = sItsCode;
	}
	else
	{
		// Lot
		nTemp = strFileData.Find(',', 0);
		strLot = strFileData.Left(nTemp);
		strFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;
		m_pPcr[1][nIdx]->m_sLot = strLot;

		// Lot
		nTemp = strFileData.Find('\n', 0);
		strLot = strFileData.Left(nTemp);
		strFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;
		m_pPcr[1][nIdx]->m_sLot = strLot;
	}

	nTemp = strFileData.Find('\n', 0);
	strTotalBadPieceNum = strFileData.Left(nTemp);;
	strFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;


	if (pDoc->GetTestMode() == MODE_INNER || pDoc->GetTestMode() == MODE_OUTER)
	{
		m_sEngModel = strModel;
		m_sEngLotNum = strLot;
		//pDoc->m_sEngLayerUp = strLayer;
		m_sEngLayerDn = strLayer;
		if (pDoc->WorkingInfo.System.bUseITS)
			m_sItsCode = sItsCode;
	}

	//BOOL bUpdate = FALSE;
	pDoc->m_bUpdateForNewJob[1] = FALSE;

	if (pDoc->WorkingInfo.LastJob.sLotDn != strLot || pDoc->WorkingInfo.LastJob.sEngItsCode != sItsCode)
	{
		pDoc->m_bUpdateForNewJob[1] = TRUE;
		pDoc->WorkingInfo.LastJob.sLotDn = strLot;
		m_sItsCode = pDoc->WorkingInfo.LastJob.sEngItsCode = sItsCode;
	}

	if (pDoc->WorkingInfo.LastJob.sModelDn != strModel || pDoc->WorkingInfo.LastJob.sLayerDn != strLayer)
	{
		pDoc->m_bUpdateForNewJob[1] = TRUE;
		pDoc->WorkingInfo.LastJob.sModelDn = strModel;
		pDoc->WorkingInfo.LastJob.sLayerDn = strLayer;

		//if (WorkingInfo.LastJob.bDualTest)
		//{
		//	pView->ResetMkInfo(0); // CAD 데이터 리로딩 --->  0 : AOI-Up , 1 : AOI-Dn , 2 : AOI-UpDn
		//}
	}

	if (pDoc->m_bUpdateForNewJob[1])
	{
		pDoc->WriteChangedModel();

		//if (pView->m_pDlgMenu01)
		//	pView->m_pDlgMenu01->UpdateData();
	}


	int nTotDef = _tstoi(strTotalBadPieceNum);

	m_pPcr[1][nIdx]->Init(nSerial, nTotDef);

	if (nTotDef > 0)
	{
		for (i = 0; i < nTotDef; i++)
		{
			// Cam ID
			nTemp = strFileData.Find(',', 0);
			strCamID = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			m_pPcr[1][nIdx]->m_nCamId = _tstoi(strCamID);

			// Piece Number
			nTemp = strFileData.Find(',', 0);
			strPieceID = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;

			// LoadStripPieceRegion_Binary()에 의해 PCS Index가 결정됨.
			if (pDoc->WorkingInfo.System.bStripPcsRgnBin)	// DTS용
			{
				m_pPcr[1][nIdx]->m_pDefPcs[i] = _tstoi(strPieceID);

				//switch (m_Master[1].MasterInfo.nActionCode)	// 0 : Rotation / Mirror 적용 없음(CAM Data 원본), 1 : 좌우 미러, 2 : 상하 미러, 3 : 180 회전, 4 : 270 회전(CCW), 5 : 90 회전(CW)
				//{
				//case 0:
				//	m_pPcr[1][nIdx]->m_pDefPcs[i] = _tstoi(strPieceID);
				//	break;
				//case 1:
				//	m_pPcr[1][nIdx]->m_pDefPcs[i] = MirrorLR(_tstoi(strPieceID));
				//	break;
				//case 3:
				//	m_pPcr[1][nIdx]->m_pDefPcs[i] = Rotate180(_tstoi(strPieceID));
				//	break;
				//default:
				//	m_pPcr[1][nIdx]->m_pDefPcs[i] = _tstoi(strPieceID);
				//	break;
				//}
			}
			else
				m_pPcr[1][nIdx]->m_pDefPcs[i] = MirrorLR(_tstoi(strPieceID));	// 초기 양면검사기용

			m_pPcr[1][nIdx]->m_pLayer[i] = 1; // Dn

											  // BadPointPosX
			nTemp = strFileData.Find(',', 0);
			strBadPointPosX = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			m_pPcr[1][nIdx]->m_pDefPos[i].x = (long)_tstoi(strBadPointPosX);

			// BadPointPosY
			nTemp = strFileData.Find(',', 0);
			strBadPointPosY = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			m_pPcr[1][nIdx]->m_pDefPos[i].y = (long)_tstoi(strBadPointPosY);

			// BadName
			nTemp = strFileData.Find(',', 0);
			strBadName = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			m_pPcr[1][nIdx]->m_pDefType[i] = _tstoi(strBadName);

			// Temp for ITS - m_pPcr[0][nIdx]->m_pDefPcs[i] = Rotate180(_tstoi(strPieceID));
			m_Master[0].m_pPcsRgn->GetMkMatrix(Rotate180(m_pPcr[1][nIdx]->m_pDefPcs[i]), nC, nR);
			m_pPcr[1][nIdx]->m_arDefType[nR][nC] = m_pPcr[1][nIdx]->m_pDefType[i];

			// CellNum
			nTemp = strFileData.Find(',', 0);
			strCellNum = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			m_pPcr[1][nIdx]->m_pCell[i] = _tstoi(strCellNum);

			// ImageSize
			nTemp = strFileData.Find(',', 0);
			strImageSize = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			m_pPcr[1][nIdx]->m_pImgSz[i] = _tstoi(strImageSize);

			// ImageNum
			nTemp = strFileData.Find(',', 0);
			strImageNum = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			m_pPcr[1][nIdx]->m_pImg[i] = _tstoi(strImageNum);

			// strMarkingCode : -2 (NoMarking)
			nTemp = strFileData.Find('\n', 0);
			strMarkingCode = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			m_pPcr[1][nIdx]->m_pMk[i] = _tstoi(strMarkingCode);
		}
	}

	return (1); // 1(정상)
				//return(m_pPcr[1][nIdx]->m_nErrPnl);
}

BOOL CManagerReelmap::CopyDefImg(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.17"));
		return 0;
	}

	BOOL bRtn[2];
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (bDualTest)
	{
		bRtn[0] = CopyDefImgUp(nSerial);
		bRtn[1] = CopyDefImgDn(nSerial);

		if (!bRtn[0] || !bRtn[1])
			return FALSE;
	}
	else
	{
		bRtn[0] = CopyDefImgUp(nSerial);

		if (!bRtn[0])
			return FALSE;
	}
	return TRUE;
}

BOOL CManagerReelmap::CopyDefImg(int nSerial, CString sNewLot)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	BOOL bUp, bDn;
	if (bDualTest)
	{
		bUp = CopyDefImgUp(nSerial, sNewLot);
		bDn = CopyDefImgDn(nSerial, sNewLot);

		if (!bUp || !bDn)
			return FALSE;
	}
	else
	{
		bUp = CopyDefImgUp(nSerial, sNewLot);

		if (!bUp)
			return FALSE;
	}

	return TRUE;
}

void CManagerReelmap::MakeImageDirUp(int nSerial)
{
	CString strDefImgPathS, strDefImgPathD, strMakeFolderPath;
	CString sLot = pDoc->WorkingInfo.LastJob.sLotUp;


	if (pDoc->WorkingInfo.System.sPathOldFile.Right(1) != "\\")
		strMakeFolderPath.Format(_T("%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp);
	else
		strMakeFolderPath.Format(_T("%s%s"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp);

	if (!pDoc->DirectoryExists(strMakeFolderPath))
		CreateDirectory(strMakeFolderPath, NULL);

	if (pDoc->WorkingInfo.System.sPathOldFile.Right(1) != "\\")
		strMakeFolderPath.Format(_T("%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			sLot);
	else
		strMakeFolderPath.Format(_T("%s%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			sLot);

	if (!pDoc->DirectoryExists(strMakeFolderPath))
		CreateDirectory(strMakeFolderPath, NULL);

	if (pDoc->WorkingInfo.System.sPathOldFile.Right(1) != "\\")
		strMakeFolderPath.Format(_T("%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			sLot,
			pDoc->WorkingInfo.LastJob.sLayerUp);
	else
		strMakeFolderPath.Format(_T("%s%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			sLot,
			pDoc->WorkingInfo.LastJob.sLayerUp);

	if (!pDoc->DirectoryExists(strMakeFolderPath))
		CreateDirectory(strMakeFolderPath, NULL);


	if (pDoc->WorkingInfo.System.sPathOldFile.Right(1) != "\\")
		strMakeFolderPath.Format(_T("%s\\%s\\%s\\%s\\DefImage"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			sLot,
			pDoc->WorkingInfo.LastJob.sLayerUp);
	else
		strMakeFolderPath.Format(_T("%s%s\\%s\\%s\\DefImage"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			sLot,
			pDoc->WorkingInfo.LastJob.sLayerUp);

	if (!pDoc->DirectoryExists(strMakeFolderPath))
		CreateDirectory(strMakeFolderPath, NULL);

	if (pDoc->WorkingInfo.System.sPathOldFile.Right(1) != "\\")
		strMakeFolderPath.Format(_T("%s\\%s\\%s\\%s\\DefImagePos"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			sLot,
			pDoc->WorkingInfo.LastJob.sLayerUp);
	else
		strMakeFolderPath.Format(_T("%s%s\\%s\\%s\\DefImagePos"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			sLot,
			pDoc->WorkingInfo.LastJob.sLayerUp);

	if (!pDoc->DirectoryExists(strMakeFolderPath))
		CreateDirectory(strMakeFolderPath, NULL);

	if (pDoc->WorkingInfo.System.sPathOldFile.Right(1) != "\\")
		strMakeFolderPath.Format(_T("%s\\%s\\%s\\%s\\CadImage"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			sLot,
			pDoc->WorkingInfo.LastJob.sLayerUp);
	else
		strMakeFolderPath.Format(_T("%s%s\\%s\\%s\\CadImage"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			sLot,
			pDoc->WorkingInfo.LastJob.sLayerUp);

	if (!pDoc->DirectoryExists(strMakeFolderPath))
		CreateDirectory(strMakeFolderPath, NULL);

	if (pDoc->WorkingInfo.System.sPathOldFile.Right(1) != "\\")
		strMakeFolderPath.Format(_T("%s\\%s\\%s\\%s\\DefImage\\%d"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			sLot,
			pDoc->WorkingInfo.LastJob.sLayerUp,
			nSerial);
	else
		strMakeFolderPath.Format(_T("%s%s\\%s\\%s\\DefImage\\%d"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			sLot,
			pDoc->WorkingInfo.LastJob.sLayerUp,
			nSerial);

	if (!pDoc->DirectoryExists(strMakeFolderPath))
		CreateDirectory(strMakeFolderPath, NULL);

	if (pDoc->WorkingInfo.System.sPathOldFile.Right(1) != "\\")
		strMakeFolderPath.Format(_T("%s\\%s\\%s\\%s\\DefImagePos\\%d"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			sLot,
			pDoc->WorkingInfo.LastJob.sLayerUp,
			nSerial);
	else
		strMakeFolderPath.Format(_T("%s%s\\%s\\%s\\DefImagePos\\%d"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			sLot,
			pDoc->WorkingInfo.LastJob.sLayerUp,
			nSerial);

	if (!pDoc->DirectoryExists(strMakeFolderPath))
		CreateDirectory(strMakeFolderPath, NULL);

	if (pDoc->WorkingInfo.System.sPathOldFile.Right(1) != "\\")
		strMakeFolderPath.Format(_T("%s\\%s\\%s\\%s\\CadImage\\%d"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			sLot,
			pDoc->WorkingInfo.LastJob.sLayerUp,
			nSerial);
	else
		strMakeFolderPath.Format(_T("%s%s\\%s\\%s\\CadImage\\%d"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			sLot,
			pDoc->WorkingInfo.LastJob.sLayerUp,
			nSerial);

	if (!pDoc->DirectoryExists(strMakeFolderPath))
		CreateDirectory(strMakeFolderPath, NULL);
}

BOOL CManagerReelmap::CopyDefImgUp(int nSerial, CString sNewLot)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.18"));
		return 0;
	}

	CString strDefImgPathS, strDefImgPathD;// , strMakeFolderPath;
	int i;
	CFileFind finder;
	CString strTemp;
	CString sLot;

	CString strAOIImgDataPath;
	strAOIImgDataPath.Format(_T("%s\\VRSImage"), pDoc->WorkingInfo.System.sPathAoiUpDefImg);

	if (sNewLot.IsEmpty())
		sLot = pDoc->WorkingInfo.LastJob.sLotUp;
	else
		sLot = sNewLot;

	MakeImageDirUp(nSerial);

	int nIdx = GetIdxPcrBufUp(nSerial);
	if (nIdx < 0)
		return FALSE;

	int nTotDef = 0;
	if (m_pPcr[0])
	{
		if (m_pPcr[0][nIdx])
			nTotDef = m_pPcr[0][nIdx]->m_nTotDef;
	}
	if (nTotDef <= 0)
		return TRUE;

	int nErrorCnt = 0;
	for (i = 0; i < nTotDef; i++)
	{
		if (m_pPcr[0][nIdx]->m_pMk[i] != -2) // -2 (NoMarking)
		{
			int nDefImg = m_pPcr[0][nIdx]->m_pImg[i];
			if (strAOIImgDataPath.Right(1) != "\\")
				strDefImgPathS.Format(_T("%s\\%s\\%s\\%s\\%d\\%05d.tif"), strAOIImgDataPath,
					pDoc->WorkingInfo.LastJob.sModelUp,
					pDoc->WorkingInfo.LastJob.sLayerUp,
					sLot,
					nSerial,
					nDefImg);
			else
				strDefImgPathS.Format(_T("%s%s\\%s\\%s\\%d\\%05d.tif"), strAOIImgDataPath,
					pDoc->WorkingInfo.LastJob.sModelUp,
					pDoc->WorkingInfo.LastJob.sLayerUp,
					sLot,
					nSerial,
					nDefImg);

			if (pDoc->WorkingInfo.System.sPathOldFile.Right(1) != "\\")
				strDefImgPathD.Format(_T("%s\\%s\\%s\\%s\\DefImage\\%d\\%05d.tif"), pDoc->WorkingInfo.System.sPathOldFile,
					pDoc->WorkingInfo.LastJob.sModelUp,
					sLot,
					pDoc->WorkingInfo.LastJob.sLayerUp,
					nSerial,
					nDefImg);
			else
				strDefImgPathD.Format(_T("%s%s\\%s\\%s\\DefImage\\%d\\%05d.tif"), pDoc->WorkingInfo.System.sPathOldFile,
					pDoc->WorkingInfo.LastJob.sModelUp,
					sLot,
					pDoc->WorkingInfo.LastJob.sLayerUp,
					nSerial,
					nDefImg);

			if (finder.FindFile(strDefImgPathS))
			{
				if (!CopyFile((LPCTSTR)strDefImgPathS, (LPCTSTR)strDefImgPathD, FALSE))
				{
					if (!CopyFile((LPCTSTR)strDefImgPathS, (LPCTSTR)strDefImgPathD, FALSE))
					{
						strTemp.Format(_T("%s \r\n: Defect Image File Copy Fail"), strDefImgPathS);
						pView->MsgBox(strTemp);
						return FALSE;
					}
				}
			}
			else
			{
				Sleep(30);
				if (nErrorCnt > 10)
				{
					nErrorCnt = 0;
					strTemp.Format(_T("%s \r\n: Defect Image File Not Exist"), strDefImgPathS);
					//AfxMessageBox(strTemp);
					return TRUE;
				}
				else
				{
					nErrorCnt++;
					i--;
					continue;
				}
			}


			int nStrip = -1, nCol = -1, nRow = -1;
			int nPcrIdx = GetPcrIdx0(nSerial);
			int nPcsIdx = m_pPcr[0][nPcrIdx]->m_pDefPcs[i];
			int nDefCode = m_pPcr[0][nPcrIdx]->m_pDefType[i];
			if (m_Master[0].m_pPcsRgn)
				m_Master[0].m_pPcsRgn->GetMkMatrix(nPcsIdx, nStrip, nCol, nRow);

			if (pDoc->WorkingInfo.System.sPathOldFile.Right(1) != "\\")
			{
				strDefImgPathD.Format(_T("%s\\%s\\%s\\%s\\DefImagePos\\%d\\%05d_%s_%c_%d_%d.tif"), pDoc->WorkingInfo.System.sPathOldFile,
					pDoc->WorkingInfo.LastJob.sModelUp,
					sLot,
					pDoc->WorkingInfo.LastJob.sLayerUp,
					nSerial,
					nDefImg, m_pReelMap->m_sKorDef[nDefCode], nStrip + 'A', nCol + 1, nRow + 1);
			}
			else
			{
				strDefImgPathD.Format(_T("%s\\%s\\%s\\%s\\DefImagePos\\%d\\%05d_%s_%c_%d_%d.tif"), pDoc->WorkingInfo.System.sPathOldFile,
					pDoc->WorkingInfo.LastJob.sModelUp,
					sLot,
					pDoc->WorkingInfo.LastJob.sLayerUp,
					nSerial,
					nDefImg, m_pReelMap->m_sKorDef[nDefCode], nStrip + 'A', nCol + 1, nRow + 1);
			}

			if (finder.FindFile(strDefImgPathS))
			{
				if (!CopyFile((LPCTSTR)strDefImgPathS, (LPCTSTR)strDefImgPathD, FALSE))
				{
					if (!CopyFile((LPCTSTR)strDefImgPathS, (LPCTSTR)strDefImgPathD, FALSE))
					{
						strTemp.Format(_T("%s \r\n: Defect Image Position File Copy Fail"), strDefImgPathS);
						pView->MsgBox(strTemp);
						return FALSE;
					}
				}
			}
			else
			{
				Sleep(30);
				if (nErrorCnt > 10)
				{
					nErrorCnt = 0;
					strTemp.Format(_T("%s \r\n: Defect Image Position File Not Exist"), strDefImgPathS);
					//AfxMessageBox(strTemp);
					return TRUE;
				}
				else
				{
					nErrorCnt++;
					i--;
					continue;
				}
			}
		}
	}

	return TRUE;
}

void CManagerReelmap::MakeImageDirDn(int nSerial)
{
	CString strDefImgPathS, strDefImgPathD, strMakeFolderPath;
	CString sLot = pDoc->WorkingInfo.LastJob.sLotDn;
	if (pDoc->WorkingInfo.System.sPathOldFile.Right(1) != "\\")
		strMakeFolderPath.Format(_T("%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp);
	//WorkingInfo.LastJob.sModelDn);
	else
		strMakeFolderPath.Format(_T("%s%s"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp);
	//WorkingInfo.LastJob.sModelDn);

	if (!pDoc->DirectoryExists(strMakeFolderPath))
		CreateDirectory(strMakeFolderPath, NULL);

	if (pDoc->WorkingInfo.System.sPathOldFile.Right(1) != "\\")
		strMakeFolderPath.Format(_T("%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			//WorkingInfo.LastJob.sModelDn,
			sLot);
	else
		strMakeFolderPath.Format(_T("%s%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			//WorkingInfo.LastJob.sModelDn,
			sLot);

	if (!pDoc->DirectoryExists(strMakeFolderPath))
		CreateDirectory(strMakeFolderPath, NULL);

	if (pDoc->WorkingInfo.System.sPathOldFile.Right(1) != "\\")
		strMakeFolderPath.Format(_T("%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			//WorkingInfo.LastJob.sModelDn,
			sLot,
			pDoc->WorkingInfo.LastJob.sLayerDn);
	else
		strMakeFolderPath.Format(_T("%s%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			//WorkingInfo.LastJob.sModelDn,
			sLot,
			pDoc->WorkingInfo.LastJob.sLayerDn);

	if (!pDoc->DirectoryExists(strMakeFolderPath))
		CreateDirectory(strMakeFolderPath, NULL);

	if (pDoc->WorkingInfo.System.sPathOldFile.Right(1) != "\\")
		strMakeFolderPath.Format(_T("%s\\%s\\%s\\%s\\DefImage"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			//WorkingInfo.LastJob.sModelDn,
			sLot,
			pDoc->WorkingInfo.LastJob.sLayerDn);
	else
		strMakeFolderPath.Format(_T("%s%s\\%s\\%s\\DefImage"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			//WorkingInfo.LastJob.sModelDn,
			sLot,
			pDoc->WorkingInfo.LastJob.sLayerDn);

	if (!pDoc->DirectoryExists(strMakeFolderPath))
		CreateDirectory(strMakeFolderPath, NULL);

	if (pDoc->WorkingInfo.System.sPathOldFile.Right(1) != "\\")
		strMakeFolderPath.Format(_T("%s\\%s\\%s\\%s\\DefImagePos"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			//WorkingInfo.LastJob.sModelDn,
			sLot,
			pDoc->WorkingInfo.LastJob.sLayerDn);
	else
		strMakeFolderPath.Format(_T("%s%s\\%s\\%s\\DefImagePos"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			//WorkingInfo.LastJob.sModelDn,
			sLot,
			pDoc->WorkingInfo.LastJob.sLayerDn);

	if (!pDoc->DirectoryExists(strMakeFolderPath))
		CreateDirectory(strMakeFolderPath, NULL);


	if (pDoc->WorkingInfo.System.sPathOldFile.Right(1) != "\\")
		strMakeFolderPath.Format(_T("%s\\%s\\%s\\%s\\CadImage"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			sLot,
			pDoc->WorkingInfo.LastJob.sLayerDn);
	else
		strMakeFolderPath.Format(_T("%s%s\\%s\\%s\\CadImage"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			sLot,
			pDoc->WorkingInfo.LastJob.sLayerDn);

	if (!pDoc->DirectoryExists(strMakeFolderPath))
		CreateDirectory(strMakeFolderPath, NULL);

	if (pDoc->WorkingInfo.System.sPathOldFile.Right(1) != "\\")
		strMakeFolderPath.Format(_T("%s\\%s\\%s\\%s\\DefImage\\%d"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			//WorkingInfo.LastJob.sModelDn,
			sLot,
			pDoc->WorkingInfo.LastJob.sLayerDn,
			nSerial);
	else
		strMakeFolderPath.Format(_T("%s%s\\%s\\%s\\DefImage\\%d"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			//WorkingInfo.LastJob.sModelDn,
			sLot,
			pDoc->WorkingInfo.LastJob.sLayerDn,
			nSerial);

	if (!pDoc->DirectoryExists(strMakeFolderPath))
		CreateDirectory(strMakeFolderPath, NULL);

	if (pDoc->WorkingInfo.System.sPathOldFile.Right(1) != "\\")
		strMakeFolderPath.Format(_T("%s\\%s\\%s\\%s\\DefImagePos\\%d"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			//WorkingInfo.LastJob.sModelDn,
			sLot,
			pDoc->WorkingInfo.LastJob.sLayerDn,
			nSerial);
	else
		strMakeFolderPath.Format(_T("%s%s\\%s\\%s\\DefImagePos\\%d"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			//WorkingInfo.LastJob.sModelDn,
			sLot,
			pDoc->WorkingInfo.LastJob.sLayerDn,
			nSerial);

	if (!pDoc->DirectoryExists(strMakeFolderPath))
		CreateDirectory(strMakeFolderPath, NULL);


	if (pDoc->WorkingInfo.System.sPathOldFile.Right(1) != "\\")
		strMakeFolderPath.Format(_T("%s\\%s\\%s\\%s\\CadImage\\%d"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			//WorkingInfo.LastJob.sModelDn,
			sLot,
			pDoc->WorkingInfo.LastJob.sLayerDn,
			nSerial);
	else
		strMakeFolderPath.Format(_T("%s%s\\%s\\%s\\CadImage\\%d"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			//WorkingInfo.LastJob.sModelDn,
			sLot,
			pDoc->WorkingInfo.LastJob.sLayerDn,
			nSerial);

	if (!pDoc->DirectoryExists(strMakeFolderPath))
		CreateDirectory(strMakeFolderPath, NULL);
}

BOOL CManagerReelmap::CopyDefImgDn(int nSerial, CString sNewLot)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return 0;

	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.19"));
		return 0;
	}

	CString strDefImgPathS, strDefImgPathD;// , strMakeFolderPath;
	int i;
	CFileFind finder;
	CString strTemp;
	CString sLot;

	if (sNewLot.IsEmpty())
		sLot = pDoc->WorkingInfo.LastJob.sLotDn;
	else
		sLot = sNewLot;

	CString strAOIImgDataPath;
	strAOIImgDataPath.Format(_T("%s\\VRSImage"), pDoc->WorkingInfo.System.sPathAoiDnDefImg);

	MakeImageDirDn(nSerial);

	int nIdx = GetIdxPcrBufDn(nSerial);
	if (nIdx < 0)
		return FALSE;

	int nTotDef = 0;
	if (m_pPcr[1])
	{
		if (m_pPcr[1][nIdx])
			nTotDef = m_pPcr[1][nIdx]->m_nTotDef;
	}
	if (nTotDef <= 0)
		return TRUE;

	int nErrorCnt = 0;
	for (i = 0; i < nTotDef; i++)
	{
		if (m_pPcr[1][nIdx]->m_pMk[i] != -2) // -2 (NoMarking)
		{
			int nDefImg = m_pPcr[1][nIdx]->m_pImg[i];

			if (strAOIImgDataPath.Right(1) != "\\")
				strDefImgPathS.Format(_T("%s\\%s\\%s\\%s\\%d\\%05d.tif"), strAOIImgDataPath,
					pDoc->WorkingInfo.LastJob.sModelUp,
					//WorkingInfo.LastJob.sModelDn,
					pDoc->WorkingInfo.LastJob.sLayerDn,
					sLot,
					nSerial,
					nDefImg);
			else
				strDefImgPathS.Format(_T("%s%s\\%s\\%s\\%d\\%05d.tif"), strAOIImgDataPath,
					pDoc->WorkingInfo.LastJob.sModelUp,
					//WorkingInfo.LastJob.sModelDn,
					pDoc->WorkingInfo.LastJob.sLayerDn,
					sLot,
					nSerial,
					nDefImg);

			if (pDoc->WorkingInfo.System.sPathOldFile.Right(1) != "\\")
				strDefImgPathD.Format(_T("%s\\%s\\%s\\%s\\DefImage\\%d\\%05d.tif"), pDoc->WorkingInfo.System.sPathOldFile,
					pDoc->WorkingInfo.LastJob.sModelUp,
					//WorkingInfo.LastJob.sModelDn,
					sLot,
					pDoc->WorkingInfo.LastJob.sLayerDn,
					nSerial,
					nDefImg);
			else
				strDefImgPathD.Format(_T("%s%s\\%s\\%s\\DefImage\\%d\\%05d.tif"), pDoc->WorkingInfo.System.sPathOldFile,
					pDoc->WorkingInfo.LastJob.sModelUp,
					//WorkingInfo.LastJob.sModelDn,
					sLot,
					pDoc->WorkingInfo.LastJob.sLayerDn,
					nSerial,
					nDefImg);

			if (finder.FindFile(strDefImgPathS))
			{
				if (!CopyFile((LPCTSTR)strDefImgPathS, (LPCTSTR)strDefImgPathD, FALSE))
				{
					if (!CopyFile((LPCTSTR)strDefImgPathS, (LPCTSTR)strDefImgPathD, FALSE))
					{
						strTemp.Format(_T("%s \r\n: Defect Image File Copy Fail"), strDefImgPathS);
						pView->MsgBox(strTemp);
						return FALSE;
					}
				}
			}
			else
			{
				Sleep(30);
				if (nErrorCnt > 10)
				{
					nErrorCnt = 0;
					strTemp.Format(_T("%s \r\n: Defect Image File Not Exist"), strDefImgPathS);
					//AfxMessageBox(strTemp);
					return TRUE;
				}
				else
				{
					nErrorCnt++;
					i--;
					continue;
				}
			}


			int nStrip = -1, nCol = -1, nRow = -1;
			int nPcrIdx = GetPcrIdx1(nSerial);
			int nPcsIdx = m_pPcr[1][nPcrIdx]->m_pDefPcs[i];
			int nDefCode = m_pPcr[1][nPcrIdx]->m_pDefType[i];
			if (m_Master[0].m_pPcsRgn)
				m_Master[0].m_pPcsRgn->GetMkMatrix(nPcsIdx, nStrip, nCol, nRow);

			if (pDoc->WorkingInfo.System.sPathOldFile.Right(1) != "\\")
			{
				strDefImgPathD.Format(_T("%s\\%s\\%s\\%s\\DefImagePos\\%d\\%05d_%s_%c_%d_%d.tif"), pDoc->WorkingInfo.System.sPathOldFile,
					pDoc->WorkingInfo.LastJob.sModelUp,
					sLot,
					pDoc->WorkingInfo.LastJob.sLayerDn,
					nSerial,
					nDefImg, m_pReelMap->m_sKorDef[nDefCode], nStrip + 'A', nCol + 1, nRow + 1);
			}
			else
			{
				strDefImgPathD.Format(_T("%s\\%s\\%s\\%s\\DefImagePos\\%d\\%05d_%s_%c_%d_%d.tif"), pDoc->WorkingInfo.System.sPathOldFile,
					pDoc->WorkingInfo.LastJob.sModelUp,
					sLot,
					pDoc->WorkingInfo.LastJob.sLayerDn,
					nSerial,
					nDefImg, m_pReelMap->m_sKorDef[nDefCode], nStrip + 'A', nCol + 1, nRow + 1);
			}

			if (finder.FindFile(strDefImgPathS))
			{
				if (!CopyFile((LPCTSTR)strDefImgPathS, (LPCTSTR)strDefImgPathD, FALSE))
				{
					if (!CopyFile((LPCTSTR)strDefImgPathS, (LPCTSTR)strDefImgPathD, FALSE))
					{
						strTemp.Format(_T("%s \r\n: Defect Image Position File Copy Fail"), strDefImgPathS);
						pView->MsgBox(strTemp);
						return FALSE;
					}
				}
			}
			else
			{
				Sleep(30);
				if (nErrorCnt > 10)
				{
					nErrorCnt = 0;
					strTemp.Format(_T("%s \r\n: Defect Image Position File Not Exist"), strDefImgPathS);
					//AfxMessageBox(strTemp);
					return TRUE;
				}
				else
				{
					nErrorCnt++;
					i--;
					continue;
				}
			}
		}
	}

	return TRUE;
}

int CManagerReelmap::GetIdxPcrBuf(int nSerial) // 릴맵화면표시의 판넬 인덱스를 반환(MAX_PCR_PNL)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.20"));
		return 0;
	}

	int i, nIdx = -1;

	if (m_pPcr[0])
	{
		for (i = 0; i < MAX_PCR_PNL; i++)
		{
			if (m_pPcr[0][i]->m_nSerial == nSerial)
			{
				nIdx = i;
				break;
			}
		}
	}

	return nIdx;
}

int CManagerReelmap::GetIdxPcrBufUp(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.21"));
		return 0;
	}

	int i, nIdx = -1;

	if (m_pPcr[0])
	{
		for (i = 0; i < MAX_PCR_PNL; i++)
		{
			if (m_pPcr[0][i]->m_nSerial == nSerial)
			{
				nIdx = i;
				break;
			}
		}
	}

	return nIdx;
}

int CManagerReelmap::GetIdxPcrBufDn(int nSerial)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return 0;

	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.22"));
		return 0;
	}

	int i, nIdx = -1;

	if (m_pPcr[1])
	{
		for (i = 0; i < MAX_PCR_PNL; i++)
		{
			if (m_pPcr[1][i]->m_nSerial == nSerial)
			{
				nIdx = i;
				break;
			}
		}
	}

	return nIdx;
}

int CManagerReelmap::GetErrCode0Its(int nSerial) // 1(정상), -1(Align Error, 노광불량), -2(Lot End)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.32"));
		return 0;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

#ifndef	TEST_MODE
	int nErr[2];
	nErr[0] = GetErrCodeUp0Its(nSerial);
	if (nErr[0] != 1)
		return nErr[0];
#endif

	return 1;
}

int CManagerReelmap::GetErrCodeUp0Its(int nSerial) // 1(정상), -1(Align Error, 노광불량), -2(Lot End)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.33"));
		return 0;
	}

	int nIdx = GetPcrIdx0(nSerial);
	int nErrCode = 1;

#ifndef	TEST_MODE
	if (m_pPcrIts)
	{
		if (m_pPcrIts[nIdx])
			nErrCode = m_pPcrIts[nIdx]->m_nErrPnl;
		else
			return 2;	// Code Setting Error.
	}
	else
		return 2;	// Code Setting Error.
#endif

	return nErrCode;
}

int CManagerReelmap::GetErrCodeDn0Its(int nSerial) // 1(정상), -1(Align Error, 노광불량), -2(Lot End)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return 1;

	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.34"));
		return 0;
	}

	int nIdx = GetPcrIdx0(nSerial);
	int nErrCode = 1;

#ifndef	TEST_MODE
	if (m_pPcrIts)
	{
		if (m_pPcrIts[nIdx])
			nErrCode = m_pPcrIts[nIdx]->m_nErrPnl;
	}
#endif

	return nErrCode;
}

int CManagerReelmap::GetErrCode1Its(int nSerial) // 1(정상), -1(Align Error, 노광불량), -2(Lot End)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.35"));
		return 0;
	}

#ifndef	TEST_MODE
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	int nErr[2];
	nErr[0] = GetErrCodeUp1Its(nSerial);
	if (nErr[0] != 1)
		return nErr[0];
#endif

	return 1;
}

int CManagerReelmap::GetErrCodeUp1Its(int nSerial) // 1(정상), -1(Align Error, 노광불량), -2(Lot End)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.36"));
		return 0;
	}

	int nIdx = GetPcrIdx1(nSerial);
	int nErrCode = 1;

#ifndef	TEST_MODE
	if (m_pPcrIts)
	{
		if (m_pPcrIts[nIdx])
			nErrCode = m_pPcrIts[nIdx]->m_nErrPnl;
		else
			return 2;	// Code Setting Error.
	}
	else
		return 2;	// Code Setting Error.
#endif

	return nErrCode;
}

int CManagerReelmap::GetErrCodeDn1Its(int nSerial) // 1(정상), -1(Align Error, 노광불량), -2(Lot End)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return 1;

	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.37"));
		return 0;
	}

	int nIdx = GetPcrIdx1(nSerial);
	int nErrCode = 1;

#ifndef	TEST_MODE
	if (m_pPcrIts)
	{
		if (m_pPcrIts[nIdx])
			nErrCode = m_pPcrIts[nIdx]->m_nErrPnl;
		else
			return 2;	// Code Setting Error.
	}
	else
		return 2;	// Code Setting Error.
#endif

	return nErrCode;
}

int CManagerReelmap::GetTotDefPcsIts(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.41"));
		return 0;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	int nIdx = GetPcrIdx0(nSerial);
	int nTotDef = 0;

#ifndef TEST_MODE
	if (bDualTest)
	{
		if (m_pPcrIts)
		{
			if (m_pPcrIts[nIdx])
				nTotDef = m_pPcrIts[nIdx]->m_nTotDef;
		}
	}
	else
	{
		if (m_pPcrIts)
		{
			if (m_pPcrIts[nIdx])
				nTotDef = m_pPcrIts[nIdx]->m_nTotDef;
		}
	}
#else
	nTotDef = 1;
#endif

	return nTotDef;
}

int CManagerReelmap::GetTotDefPcsUpIts(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.42"));
		return 0;
	}

	int nIdx = GetPcrIdx0(nSerial);
	int nTotDef = 0;

#ifndef TEST_MODE
	if (m_pPcrIts)
	{
		if (m_pPcrIts[nIdx])
			nTotDef = m_pPcrIts[nIdx]->m_nTotDef;
	}
#else
	nTotDef = 1;
#endif

	return nTotDef;
}

int CManagerReelmap::GetTotDefPcsDnIts(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.43"));
		return 0;
	}

	int nIdx = GetPcrIdx0(nSerial);
	int nTotDef = 0;

#ifndef TEST_MODE
	if (m_pPcrIts)
	{
		if (m_pPcrIts[nIdx])
			nTotDef = m_pPcrIts[nIdx]->m_nTotDef;
	}
#else
	nTotDef = 1;
#endif

	return nTotDef;
}

CfPoint CManagerReelmap::GetMkPnt(int nMkPcs)
{
	CfPoint ptPnt;
	ptPnt.x = -1.0;
	ptPnt.y = -1.0;

#ifndef TEST_MODE
	if (m_Master[0].m_pPcsRgn)
		ptPnt = m_Master[0].m_pPcsRgn->GetMkPnt(nMkPcs); // Cam0의 Mk 포인트.
#else
	ptPnt.x = 1.0;
	ptPnt.y = 1.0;
#endif

	return ptPnt;
}

CfPoint CManagerReelmap::GetMkPnt(int nSerial, int nMkPcs) // pcr 시리얼, pcr 불량 피스 읽은 순서 인덱스
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nIdx = GetPcrIdx(nSerial);
	CfPoint ptPnt;
	ptPnt.x = -1.0;
	ptPnt.y = -1.0;

#ifndef TEST_MODE
	int nDefPcsId = 0;

	if (bDualTest)
	{
		if (m_pPcr[2])	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (m_pPcr[2][nIdx])
			{
				if (m_pPcr[2][nIdx]->m_pDefPcs)
				{
					if (m_pPcr[2][nIdx]->m_pMk[nMkPcs] != -2) // -2 (NoMarking)
					{
						nDefPcsId = m_pPcr[2][nIdx]->m_pDefPcs[nMkPcs];
						if (m_Master[0].m_pPcsRgn)
							ptPnt = m_Master[0].m_pPcsRgn->GetMkPnt(nDefPcsId); // Cam0의 Mk 포인트.
					}
				}
			}
		}
	}
	else
	{
		if (m_pPcr[0])	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (m_pPcr[0][nIdx])
			{
				if (m_pPcr[0][nIdx]->m_pDefPcs)
				{
					if (m_pPcr[0][nIdx]->m_pMk[nMkPcs] != -2) // -2 (NoMarking)
					{
						nDefPcsId = m_pPcr[0][nIdx]->m_pDefPcs[nMkPcs];
						if (m_Master[0].m_pPcsRgn)
							ptPnt = m_Master[0].m_pPcsRgn->GetMkPnt(nDefPcsId); // Cam0의 Mk 포인트.
					}
				}
			}
		}
	}
#else
	ptPnt.x = 1.0;
	ptPnt.y = 1.0;
#endif

	return ptPnt;
}

int CManagerReelmap::GetMkStripIdx(int nDefPcsId) // 0 : Fail , 1~4 : Strip Idx
{
	int nMaxStrip;
#ifdef USE_CAM_MASTER
	nMaxStrip = m_Master[0].GetStripNum(); // 총 스트립의 갯수
#else
#ifdef TEST_MODE
	nMaxStrip = 4;
#else
	nMaxStrip = MAX_STRIP;
#endif
#endif
	int nNodeX = m_Master[0].m_pPcsRgn->nCol;
	int nNodeY = m_Master[0].m_pPcsRgn->nRow;
	int nStripY = int(nNodeY / nMaxStrip);
	int nStripIdx = 0;

#ifndef TEST_MODE
	int nRow = 0, nNum = 0, nMode = 0;
	nNum = int(nDefPcsId / nNodeY);
	nMode = nDefPcsId % nNodeY;
	if (nNum % 2) 	// 홀수.
		nRow = nNodeY - (nMode + 1);
	else		// 짝수.
		nRow = nMode;

	nStripIdx = int(nRow / nStripY) + 1;
#else
	nStripIdx = 1;
#endif

	return nStripIdx;
}

int CManagerReelmap::GetMkStripIdx(int nSerial, int nMkPcs) // 0 : Fail , 1~4 : Strip Idx
{
	int nMaxStrip;
#ifdef USE_CAM_MASTER
	nMaxStrip = m_Master[0].GetStripNum(); // 총 스트립의 갯수
#else
#ifdef TEST_MODE
	nMaxStrip = 4;
#else
	nMaxStrip = MAX_STRIP;
#endif
#endif
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nIdx = GetPcrIdx0(nSerial);
	int nNodeX = m_Master[0].m_pPcsRgn->nCol;
	int nNodeY = m_Master[0].m_pPcsRgn->nRow;
	int nStripY = int(nNodeY / nMaxStrip);
	int nStripIdx = 0;

#ifndef TEST_MODE
	int nDefPcsId = 0, nNum = 0, nMode = 0, nRow = 0;

	if (bDualTest)
	{
		if (m_pPcr[2])	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (m_pPcr[2][nIdx])
			{
				if (m_pPcr[2][nIdx]->m_pDefPcs)
				{
					nDefPcsId = m_pPcr[2][nIdx]->m_pDefPcs[nMkPcs];
					nNum = int(nDefPcsId / nNodeY);
					nMode = nDefPcsId % nNodeY;
					if (nNum % 2) 	// 홀수.
						nRow = nNodeY - (nMode + 1);
					else		// 짝수.
						nRow = nMode;

					nStripIdx = int(nRow / nStripY) + 1;
				}
			}
		}
	}
	else
	{
		if (m_pPcr[0])	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (m_pPcr[0][nIdx])
			{
				if (m_pPcr[0][nIdx]->m_pDefPcs)
				{
					nDefPcsId = m_pPcr[0][nIdx]->m_pDefPcs[nMkPcs];
					nNum = int(nDefPcsId / nNodeY);
					nMode = nDefPcsId % nNodeY;
					if (nNum % 2) 	// 홀수.
						nRow = nNodeY - (nMode + 1);
					else		// 짝수.
						nRow = nMode;

					nStripIdx = int(nRow / nStripY) + 1;
				}
			}
		}
	}
#else
	nStripIdx = 1;
#endif

	return nStripIdx;
}

CfPoint CManagerReelmap::GetMkPntIts(int nSerial, int nMkPcs) // pcr 시리얼, pcr 불량 피스 읽은 순서 인덱스
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.47"));
		return 0;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nIdx = GetPcrIdx0(nSerial);
	CfPoint ptPnt;
	ptPnt.x = -1.0;
	ptPnt.y = -1.0;

#ifndef TEST_MODE
	int nDefPcsId = 0;

	if (bDualTest)
	{
		if (m_pPcrIts)	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (m_pPcrIts[nIdx])
			{
				if (m_pPcrIts[nIdx]->m_pDefPcs)
				{
					if (m_pPcrIts[nIdx]->m_pMk[nMkPcs] != -2) // -2 (NoMarking)
					{
						nDefPcsId = m_pPcrIts[nIdx]->m_pDefPcs[nMkPcs];
						if (m_Master[0].m_pPcsRgn)
							ptPnt = m_Master[0].m_pPcsRgn->GetMkPnt0(nDefPcsId); // Cam0의 Mk 포인트.
					}
				}
			}
		}
	}
	else
	{
		if (m_pPcrIts)	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (m_pPcrIts[nIdx])
			{
				if (m_pPcrIts[nIdx]->m_pDefPcs)
				{
					if (m_pPcrIts[nIdx]->m_pMk[nMkPcs] != -2) // -2 (NoMarking)
					{
						nDefPcsId = m_pPcrIts[nIdx]->m_pDefPcs[nMkPcs];
						if (m_Master[0].m_pPcsRgn)
							ptPnt = m_Master[0].m_pPcsRgn->GetMkPnt0(nDefPcsId); // Cam0의 Mk 포인트.
					}
				}
			}
		}
	}
#else
	ptPnt.x = 1.0;
	ptPnt.y = 1.0;
#endif

	return ptPnt;
}

int CManagerReelmap::GetMkStripIdxIts(int nSerial, int nMkPcs) // 0 : Fail , 1~4 : Strip Idx
{
	if (nSerial <= 0)
	{
		AfxMessageBox(_T("Serial Error.48"));
		return 0;
	}

	int nMaxStrip;
#ifdef USE_CAM_MASTER
	nMaxStrip = m_Master[0].GetStripNum(); // 총 스트립의 갯수
#else
#ifdef TEST_MODE
	nMaxStrip = 4;
#else
	nMaxStrip = MAX_STRIP;
#endif
#endif

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nIdx = GetPcrIdx0(nSerial);
	int nNodeX = m_Master[0].m_pPcsRgn->nCol;
	int nNodeY = m_Master[0].m_pPcsRgn->nRow;
	int nStripY = int(nNodeY / nMaxStrip);
	int nStripIdx = 0;

#ifndef TEST_MODE
	int nDefPcsId = 0, nNum = 0, nMode = 0, nRow = 0;

	if (bDualTest)
	{
		if (m_pPcrIts)	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (m_pPcrIts[nIdx])
			{
				if (m_pPcrIts[nIdx]->m_pDefPcs)
				{
					nDefPcsId = m_pPcrIts[nIdx]->m_pDefPcs[nMkPcs];
					nNum = int(nDefPcsId / nNodeY);
					nMode = nDefPcsId % nNodeY;
					if (nNum % 2) 	// 홀수.
						nRow = nNodeY - (nMode + 1);
					else		// 짝수.
						nRow = nMode;

					nStripIdx = int(nRow / nStripY) + 1;
				}
			}
		}
	}
	else
	{
		if (m_pPcrIts)	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (m_pPcrIts[nIdx])
			{
				if (m_pPcrIts[nIdx]->m_pDefPcs)
				{
					nDefPcsId = m_pPcrIts[nIdx]->m_pDefPcs[nMkPcs];
					nNum = int(nDefPcsId / nNodeY);
					nMode = nDefPcsId % nNodeY;
					if (nNum % 2) 	// 홀수.
						nRow = nNodeY - (nMode + 1);
					else		// 짝수.
						nRow = nMode;

					nStripIdx = int(nRow / nStripY) + 1;
				}
			}
		}
	}
#else
	nStripIdx = 1;
#endif

	return nStripIdx;
}

BOOL CManagerReelmap::IsFixPcsUp(int nSerial)
{
	if (!m_pReelMapUp)
		return FALSE;

	int nMaxStrip;
#ifdef USE_CAM_MASTER
	nMaxStrip = m_Master[0].GetStripNum(); // 총 스트립의 갯수
#else
#ifdef TEST_MODE
	nMaxStrip = 4;
#else
	nMaxStrip = MAX_STRIP;
#endif
#endif

	CString sMsg = _T(""), str = _T("");
	int nStrip, pCol[2500], pRow[2500], nTot, nRepeat;
	BOOL bCont = FALSE;

	if (m_pReelMapUp->IsFixPcs(nSerial, pCol, pRow, nTot, bCont))
	{
		int nNodeX = m_Master[0].m_pPcsRgn->nCol;
		int nNodeY = m_Master[0].m_pPcsRgn->nRow;
		int nStPcsY = nNodeY / nMaxStrip;

		if (bCont)
			sMsg.Format(_T("상면 연속 고정불량 발생"));
		else
			sMsg.Format(_T("상면 누적 고정불량 발생"));

		for (int i = 0; i < nTot; i++)
		{
			nStrip = int(pRow[i] / nStPcsY);
			nRepeat = m_pReelMapUp->GetRptFixPcs(pCol[i], pRow[i]);
			if (!(i % 5))
			{
				if (nRepeat > 1)
					str.Format(_T("\r\n[%d:%c-%d,%d] (%d회)"), nSerial, 'A' + nStrip, pCol[i] + 1, (pRow[i] % nStPcsY) + 1, nRepeat);
				else
					str.Format(_T("\r\n[%d:%c-%d,%d]"), nSerial, 'A' + nStrip, pCol[i] + 1, (pRow[i] % nStPcsY) + 1);
			}
			else
			{
				if (nRepeat > 1)
					str.Format(_T(" , [%d:%c-%d,%d] (%d회)"), nSerial, 'A' + nStrip, pCol[i] + 1, (pRow[i] % nStPcsY) + 1, nRepeat);
				else
					str.Format(_T(" , [%d:%c-%d,%d]"), nSerial, 'A' + nStrip, pCol[i] + 1, (pRow[i] % nStPcsY) + 1);
			}

			sMsg += str;
		}
		if (pView && pView->m_mgrProcedure)
			pView->m_mgrProcedure->m_sFixMsg[0] = sMsg;
		// 		TowerLamp(RGB_RED, TRUE);
		// 		Buzzer(TRUE, 0);
		// 		MsgBox(sMsg);
		return TRUE;
	}

	return FALSE;
}

BOOL CManagerReelmap::IsFixPcsDn(int nSerial)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return FALSE;

	if (!m_pReelMapUp)
		return FALSE;

	int nMaxStrip;
#ifdef USE_CAM_MASTER
	nMaxStrip = m_Master[0].GetStripNum(); // 총 스트립의 갯수
#else
#ifdef TEST_MODE
	nMaxStrip = 4;
#else
	nMaxStrip = MAX_STRIP;
#endif
#endif

	CString sMsg = _T(""), str = _T("");
	int nStrip, pCol[2500], pRow[2500], nTot, nRepeat;
	BOOL bCont = FALSE;

	if (m_pReelMapDn->IsFixPcs(nSerial, pCol, pRow, nTot, bCont))
	{
		int nNodeX = m_Master[0].m_pPcsRgn->nCol;
		int nNodeY = m_Master[0].m_pPcsRgn->nRow;
		int nStPcsY = nNodeY / nMaxStrip;

		if (bCont)
			sMsg.Format(_T("하면 연속 고정불량 발생"));
		else
			sMsg.Format(_T("하면 누적 고정불량 발생"));

		for (int i = 0; i < nTot; i++)
		{
			nStrip = int(pRow[i] / nStPcsY);
			nRepeat = m_pReelMapDn->GetRptFixPcs(pCol[i], pRow[i]);

			if (!(i % 5))
			{
				if (nRepeat > 1)
					str.Format(_T("\r\n[%d:%c-%d,%d] (%d회)"), nSerial, 'A' + nStrip, pCol[i] + 1, (pRow[i] % nStPcsY) + 1, nRepeat);
				else
					str.Format(_T("\r\n[%d:%c-%d,%d]"), nSerial, 'A' + nStrip, pCol[i] + 1, (pRow[i] % nStPcsY) + 1);
			}
			else
			{
				if (nRepeat > 1)
					str.Format(_T(" , [%d:%c-%d,%d] (%d회)"), nSerial, 'A' + nStrip, pCol[i] + 1, (pRow[i] % nStPcsY) + 1, nRepeat);
				else
					str.Format(_T(" , [%d:%c-%d,%d]"), nSerial, 'A' + nStrip, pCol[i] + 1, (pRow[i] % nStPcsY) + 1);
			}

			sMsg += str;
		}
		// 		TowerLamp(RGB_RED, TRUE);
		// 		Buzzer(TRUE, 0);
		// 		MsgBox(sMsg);
		if(pView && pView->m_mgrProcedure)
			pView->m_mgrProcedure->m_sFixMsg[1] = sMsg;

		return TRUE;
	}

	return FALSE;
}

int CManagerReelmap::GetLastItsSerial()
{
	return m_nWritedItsSerial;
}

int CManagerReelmap::LoadPCRIts(int nSerial, BOOL bFromShare)	// return : 2(Failed), 1(정상), -1(Align Error, 노광불량), -2(Lot End)
{
#ifdef TEST_MODE
	return 0;
#endif
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.16"));
		return 0;
	}

	if (!m_pPcrIts)
	{
		pView->ClrDispMsg();
		pView->MsgBox(_T("PCR[2]관련 메모리가 할당되지 않았습니다."));
		//AfxMessageBox(strFileData);
		return(2);
	}

	BOOL bDualTestInner;
	CString sLot, sLayerUp, sLayerDn;
	if (!GetItsSerialInfo(nSerial, bDualTestInner, sLot, sLayerUp, sLayerDn, 0))
	{
		CString str;
		str.Format(_T("It is trouble to read GetItsSerialInfo()."));
		pView->MsgBox(str);
		return FALSE;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (bDualTest && bDualTestInner)	// 11 -> 외층 : 양면, 내층 : 양면
		LoadPCRIts11(nSerial);
	else if (bDualTest && !bDualTestInner)	// 10 -> 외층 : 양면, 내층 : 단면
		LoadPCRIts10(nSerial);
	else if (!bDualTest && bDualTestInner)	// 01 -> 외층 : 단면, 내층 : 양면
		LoadPCRIts01(nSerial);
	else 								// 00 -> 외층 : 단면, 내층 : 단면
		LoadPCRIts00(nSerial);

	return (1); // 1(정상)
}

void CManagerReelmap::LoadPCRIts11(int nSerial) // 11 -> 외층 : 양면, 내층 : 양면
{
	int i, idx;//, k
	CString str;
	int nIdx;

	if (pDoc->m_bNewLotShare[0] && (pDoc->WorkingInfo.LastJob.bLotSep || pDoc->m_bDoneChgLot))
		nIdx = GetPcrIdx0(nSerial, TRUE); // 릴맵화면 표시 인덱스
	else
		nIdx = GetPcrIdx0(nSerial);

	//if (!m_pPcr[0] || !m_pPcr[1] || !m_pPcr[2])
	//	return(2);
	//if (!m_pPcr[0][nIdx] || !m_pPcr[1][nIdx] || !m_pPcr[2][nIdx])
	//	return(2);

	m_pPcrIts[nIdx]->m_nIdx = nIdx;							// m_nIdx : From 0 to nTot.....
	m_pPcrIts[nIdx]->m_nSerial = nSerial;

	// Error Code											// 1(정상), -1(Align Error, 노광불량), -2(Lot End)
	if (m_pPcr[2][nIdx]->m_nErrPnl < 0)
		m_pPcrIts[nIdx]->m_nErrPnl = m_pPcr[2][nIdx]->m_nErrPnl;
	else if (m_pPcrInner[2][nIdx]->m_nErrPnl < 0)
		m_pPcrIts[nIdx]->m_nErrPnl = m_pPcrInner[2][nIdx]->m_nErrPnl;
	else
		m_pPcrIts[nIdx]->m_nErrPnl = m_pPcr[2][nIdx]->m_nErrPnl;

	// Model
	m_pPcrIts[nIdx]->m_sModel = m_pPcr[0][nIdx]->m_sModel;

	// Layer
	m_pPcrIts[nIdx]->m_sLayerOutUp = m_pPcr[0][nIdx]->m_sLayer;
	m_pPcrIts[nIdx]->m_sLayerOutDn = m_pPcr[1][nIdx]->m_sLayer;
	m_pPcrIts[nIdx]->m_sLayerInUp = m_pPcrInner[0][nIdx]->m_sLayer;
	m_pPcrIts[nIdx]->m_sLayerInDn = m_pPcrInner[1][nIdx]->m_sLayer;

	// Lot
	m_pPcrIts[nIdx]->m_sLot = m_pPcr[0][nIdx]->m_sLot;

	int nTotDef[3] = { 0 };									// [0]: 외층, [1]: 내층, [2]: 내/외층 Merge
	nTotDef[0] = m_pPcr[2][nIdx]->m_nTotDef;			// 외층 불량 피스 수
	nTotDef[1] = m_pPcrInner[2][nIdx]->m_nTotDef;		// 내층 불량 피스 수

	int nTotPcs = m_Master[0].m_pPcsRgn->nTotPcs;
	stPcrMerge *pPcrMgr = new stPcrMerge[nTotPcs];

	int nPcsId;												// nPcsId : CamMaster Pcs Index
															// pPcrMgr테이블의 nIdx에 내층의 불량을 먼저 기록하고 외층의 불량을 엎어서 최종 merge불량 테이블을 만듬.
	for (i = 0; i < nTotDef[1]; i++)						// 내층 불량 피스 수
	{
		nPcsId = m_pPcrInner[2][nIdx]->m_pDefPcs[i];		// nPcsId : CamMaster Pcs Index
		pPcrMgr[nPcsId].nIdx = i;	// 외층+내층			// 내외층 Merge [nPcsId:CamMaster Pcs Index]의 nIdx (0~)는 불량표시순서임.
		pPcrMgr[nPcsId].nIdxDn = i; // 내층					// 내외층 Merge [nPcsId:CamMaster Pcs Index]의 nIdxDn (0~)는 불량표시순서임.
	}
	for (i = 0; i < nTotDef[0]; i++)						// 외층 불량 피스 수
	{
		nPcsId = m_pPcr[2][nIdx]->m_pDefPcs[i];				// nPcsId : CamMaster Pcs Index
		pPcrMgr[nPcsId].nIdx = i;	// 외층+내층			// 내외층 Merge [nPcsId:CamMaster Pcs Index]의 nIdx (0~)는 불량표시순서임.
		pPcrMgr[nPcsId].nIdxUp = i;	// 외층					// 내외층 Merge [nPcsId:CamMaster Pcs Index]의 nIdxUp (0~)는 불량표시순서임.
	}

	nTotDef[2] = 0;
	for (i = 0; i < nTotPcs; i++)							// Shot내 총 Piece수
	{
		if (pPcrMgr[i].nIdx > -1)	// 외층+내층			// 내외층 Merge [i:CamMaster Pcs Index]의 nIdx (0~)는 불량표시순서임.
			nTotDef[2]++;									// 내외층 Merge한 총 불량피스수.
	}

	m_pPcrIts[nIdx]->Init(nSerial, nTotDef[2]);				// 제품시리얼, Shot내 총불량 피스수

	int nId[2], Ord;										// [0]: 외층 0~불량피스순서, [1]: 내층 0~불량피스순서
	idx = 0;												// 마킹순서 0~불량피스수만큼 정하기위해 현시점의 idx를 초기화함.
	if (nTotDef[2] > 0)										// 내외층 Merge한 총 불량피스수.
	{
		for (nPcsId = 0; nPcsId < nTotPcs; nPcsId++)
		{
			nId[0] = pPcrMgr[nPcsId].nIdxUp;				// 내외층 Merge [nPcsId:CamMaster Pcs Index]의 nIdxUp (0~)는 불량표시순서 임. nId[0]: 외층에서의 PCR파일순서 인덱스
			nId[1] = pPcrMgr[nPcsId].nIdxDn;				// 내외층 Merge [nPcsId:CamMaster Pcs Index]의 nIdxDn (0~)는 불량표시순서 임. nId[1]: 내층에서의 PCR파일순서 인덱스

			if (nId[0] > -1)
			{
				Ord = nId[0];
				// Cam ID
				m_pPcrIts[nIdx]->m_nCamId = m_pPcr[2][nIdx]->m_nCamId;
				// Piece Number
				m_pPcrIts[nIdx]->m_pDefPcs[idx] = m_pPcr[2][nIdx]->m_pDefPcs[Ord];
				m_pPcrIts[nIdx]->m_pLayer[idx] = m_pPcr[2][nIdx]->m_pLayer[Ord];
				// BadPointPosX
				m_pPcrIts[nIdx]->m_pDefPos[idx].x = m_pPcr[2][nIdx]->m_pDefPos[Ord].x;
				// BadPointPosY
				m_pPcrIts[nIdx]->m_pDefPos[idx].y = m_pPcr[2][nIdx]->m_pDefPos[Ord].y;
				// BadName
				m_pPcrIts[nIdx]->m_pDefType[idx] = m_pPcr[2][nIdx]->m_pDefType[Ord];
				// CellNum
				m_pPcrIts[nIdx]->m_pCell[idx] = m_pPcr[2][nIdx]->m_pCell[Ord];
				// ImageSize
				m_pPcrIts[nIdx]->m_pImgSz[idx] = m_pPcr[2][nIdx]->m_pImgSz[Ord];
				// ImageNum
				m_pPcrIts[nIdx]->m_pImg[idx] = m_pPcr[2][nIdx]->m_pImg[Ord];
				// strMarkingCode : -2 (NoMarking)
				m_pPcrIts[nIdx]->m_pMk[idx] = m_pPcr[2][nIdx]->m_pMk[Ord];

				idx++;
			}
			else if (nId[1] > -1)
			{
				Ord = nId[1];
				// Cam ID
				m_pPcrIts[nIdx]->m_nCamId = m_pPcrInner[2][nIdx]->m_nCamId;
				// Piece Number
				m_pPcrIts[nIdx]->m_pDefPcs[idx] = m_pPcrInner[2][nIdx]->m_pDefPcs[Ord];
				m_pPcrIts[nIdx]->m_pLayer[idx] = m_pPcrInner[2][nIdx]->m_pLayer[Ord];
				// BadPointPosX
				m_pPcrIts[nIdx]->m_pDefPos[idx].x = m_pPcrInner[2][nIdx]->m_pDefPos[Ord].x;
				// BadPointPosY
				m_pPcrIts[nIdx]->m_pDefPos[idx].y = m_pPcrInner[2][nIdx]->m_pDefPos[Ord].y;
				// BadName
				m_pPcrIts[nIdx]->m_pDefType[idx] = m_pPcrInner[2][nIdx]->m_pDefType[Ord];
				// CellNum
				m_pPcrIts[nIdx]->m_pCell[idx] = m_pPcrInner[2][nIdx]->m_pCell[Ord];
				// ImageSize
				m_pPcrIts[nIdx]->m_pImgSz[idx] = m_pPcrInner[2][nIdx]->m_pImgSz[Ord];
				// ImageNum
				m_pPcrIts[nIdx]->m_pImg[idx] = m_pPcrInner[2][nIdx]->m_pImg[Ord];
				// strMarkingCode : -2 (NoMarking)
				m_pPcrIts[nIdx]->m_pMk[idx] = m_pPcrInner[2][nIdx]->m_pMk[Ord];

				idx++;
			}
		}
	}
	delete[] pPcrMgr;
}

void CManagerReelmap::LoadPCRIts10(int nSerial) // 10 -> 외층 : 양면, 내층 : 단면
{
	int i, idx;//, k
	CString str;
	int nIdx;

	if (pDoc->m_bNewLotShare[0] && (pDoc->WorkingInfo.LastJob.bLotSep || pDoc->m_bDoneChgLot))
		nIdx = GetPcrIdx0(nSerial, TRUE); // 릴맵화면 표시 인덱스
	else
		nIdx = GetPcrIdx0(nSerial);

	m_pPcrIts[nIdx]->m_nIdx = nIdx;							// m_nIdx : From 0 to nTot.....
	m_pPcrIts[nIdx]->m_nSerial = nSerial;

	// Error Code											// 1(정상), -1(Align Error, 노광불량), -2(Lot End)
	if (m_pPcr[2][nIdx]->m_nErrPnl < 0)
		m_pPcrIts[nIdx]->m_nErrPnl = m_pPcr[2][nIdx]->m_nErrPnl;
	else if (m_pPcrInner[0][nIdx]->m_nErrPnl < 0)
		m_pPcrIts[nIdx]->m_nErrPnl = m_pPcrInner[0][nIdx]->m_nErrPnl;
	else
		m_pPcrIts[nIdx]->m_nErrPnl = m_pPcr[2][nIdx]->m_nErrPnl;

	// Model
	m_pPcrIts[nIdx]->m_sModel = m_pPcr[0][nIdx]->m_sModel;

	// Layer
	m_pPcrIts[nIdx]->m_sLayerOutUp = m_pPcr[0][nIdx]->m_sLayer;
	m_pPcrIts[nIdx]->m_sLayerOutDn = m_pPcr[1][nIdx]->m_sLayer;
	m_pPcrIts[nIdx]->m_sLayerInUp = m_pPcrInner[0][nIdx]->m_sLayer;
	m_pPcrIts[nIdx]->m_sLayerInDn = _T(""); // m_pPcrInner[1][nIdx]->m_sLayer;

	// Lot
	m_pPcrIts[nIdx]->m_sLot = m_pPcr[0][nIdx]->m_sLot;

	int nTotDef[3] = { 0 };									// [0]: 외층, [1]: 내층, [2]: 내/외층 Merge
	nTotDef[0] = m_pPcr[2][nIdx]->m_nTotDef;			// 외층 불량 피스 수
	nTotDef[1] = m_pPcrInner[0][nIdx]->m_nTotDef;		// 내층 불량 피스 수

	int nTotPcs = m_Master[0].m_pPcsRgn->nTotPcs;
	stPcrMerge *pPcrMgr = new stPcrMerge[nTotPcs];

	int nPcsId;												// nPcsId : CamMaster Pcs Index
															// pPcrMgr테이블의 nIdx에 내층의 불량을 먼저 기록하고 외층의 불량을 엎어서 최종 merge불량 테이블을 만듬.
	for (i = 0; i < nTotDef[1]; i++)						// 내층 불량 피스 수
	{
		nPcsId = m_pPcrInner[0][nIdx]->m_pDefPcs[i];		// nPcsId : CamMaster Pcs Index
		pPcrMgr[nPcsId].nIdx = i;	// 외층+내층			// 내외층 Merge [nPcsId:CamMaster Pcs Index]의 nIdx (0~)는 불량표시순서임.
		pPcrMgr[nPcsId].nIdxDn = i; // 내층					// 내외층 Merge [nPcsId:CamMaster Pcs Index]의 nIdxDn (0~)는 불량표시순서임.
	}
	for (i = 0; i < nTotDef[0]; i++)						// 외층 불량 피스 수
	{
		nPcsId = m_pPcr[2][nIdx]->m_pDefPcs[i];				// nPcsId : CamMaster Pcs Index
		pPcrMgr[nPcsId].nIdx = i;	// 외층+내층			// 내외층 Merge [nPcsId:CamMaster Pcs Index]의 nIdx (0~)는 불량표시순서임.
		pPcrMgr[nPcsId].nIdxUp = i;	// 외층					// 내외층 Merge [nPcsId:CamMaster Pcs Index]의 nIdxUp (0~)는 불량표시순서임.
	}


	nTotDef[2] = 0;
	for (i = 0; i < nTotPcs; i++)							// Shot내 총 Piece수
	{
		if (pPcrMgr[i].nIdx > -1)	// 외층+내층			// 내외층 Merge [i:CamMaster Pcs Index]의 nIdx (0~)는 불량표시순서임.
			nTotDef[2]++;									// 내외층 Merge한 총 불량피스수.
	}

	m_pPcrIts[nIdx]->Init(nSerial, nTotDef[2]);				// 제품시리얼, Shot내 총불량 피스수

	int nId[2], Ord;										// [0]: 외층 0~불량피스순서, [1]: 내층 0~불량피스순서
	idx = 0;												// 마킹순서 0~불량피스수만큼 정하기위해 현시점의 idx를 초기화함.
	if (nTotDef[2] > 0)										// 내외층 Merge한 총 불량피스수.
	{
		for (nPcsId = 0; nPcsId < nTotPcs; nPcsId++)
		{
			nId[0] = pPcrMgr[nPcsId].nIdxUp;				// 내외층 Merge [nPcsId:CamMaster Pcs Index]의 nIdxUp (0~)는 불량표시순서 임. nId[0]: 외층에서의 PCR파일순서 인덱스
			nId[1] = pPcrMgr[nPcsId].nIdxDn;				// 내외층 Merge [nPcsId:CamMaster Pcs Index]의 nIdxDn (0~)는 불량표시순서 임. nId[1]: 내층에서의 PCR파일순서 인덱스

			if (nId[0] > -1)
			{
				Ord = nId[0];
				// Cam ID
				m_pPcrIts[nIdx]->m_nCamId = m_pPcr[2][nIdx]->m_nCamId;
				// Piece Number
				m_pPcrIts[nIdx]->m_pDefPcs[idx] = m_pPcr[2][nIdx]->m_pDefPcs[Ord];
				m_pPcrIts[nIdx]->m_pLayer[idx] = m_pPcr[2][nIdx]->m_pLayer[Ord];
				// BadPointPosX
				m_pPcrIts[nIdx]->m_pDefPos[idx].x = m_pPcr[2][nIdx]->m_pDefPos[Ord].x;
				// BadPointPosY
				m_pPcrIts[nIdx]->m_pDefPos[idx].y = m_pPcr[2][nIdx]->m_pDefPos[Ord].y;
				// BadName
				m_pPcrIts[nIdx]->m_pDefType[idx] = m_pPcr[2][nIdx]->m_pDefType[Ord];
				// CellNum
				m_pPcrIts[nIdx]->m_pCell[idx] = m_pPcr[2][nIdx]->m_pCell[Ord];
				// ImageSize
				m_pPcrIts[nIdx]->m_pImgSz[idx] = m_pPcr[2][nIdx]->m_pImgSz[Ord];
				// ImageNum
				m_pPcrIts[nIdx]->m_pImg[idx] = m_pPcr[2][nIdx]->m_pImg[Ord];
				// strMarkingCode : -2 (NoMarking)
				m_pPcrIts[nIdx]->m_pMk[idx] = m_pPcr[2][nIdx]->m_pMk[Ord];

				idx++;
			}
			else if (nId[1] > -1)
			{
				Ord = nId[1];
				// Cam ID
				m_pPcrIts[nIdx]->m_nCamId = m_pPcrInner[0][nIdx]->m_nCamId;
				// Piece Number
				m_pPcrIts[nIdx]->m_pDefPcs[idx] = m_pPcrInner[0][nIdx]->m_pDefPcs[Ord];
				m_pPcrIts[nIdx]->m_pLayer[idx] = m_pPcrInner[0][nIdx]->m_pLayer[Ord];
				// BadPointPosX
				m_pPcrIts[nIdx]->m_pDefPos[idx].x = m_pPcrInner[0][nIdx]->m_pDefPos[Ord].x;
				// BadPointPosY
				m_pPcrIts[nIdx]->m_pDefPos[idx].y = m_pPcrInner[0][nIdx]->m_pDefPos[Ord].y;
				// BadName
				m_pPcrIts[nIdx]->m_pDefType[idx] = m_pPcrInner[0][nIdx]->m_pDefType[Ord];
				// CellNum
				m_pPcrIts[nIdx]->m_pCell[idx] = m_pPcrInner[0][nIdx]->m_pCell[Ord];
				// ImageSize
				m_pPcrIts[nIdx]->m_pImgSz[idx] = m_pPcrInner[0][nIdx]->m_pImgSz[Ord];
				// ImageNum
				m_pPcrIts[nIdx]->m_pImg[idx] = m_pPcrInner[0][nIdx]->m_pImg[Ord];
				// strMarkingCode : -2 (NoMarking)
				m_pPcrIts[nIdx]->m_pMk[idx] = m_pPcrInner[0][nIdx]->m_pMk[Ord];

				idx++;
			}
		}
	}
	delete[] pPcrMgr;
}

void CManagerReelmap::LoadPCRIts01(int nSerial) // 11 -> 외층 : 단면, 내층 : 양면
{
	int i, idx;//, k
	CString str;
	int nIdx;

	if (pDoc->m_bNewLotShare[0] && (pDoc->WorkingInfo.LastJob.bLotSep || pDoc->m_bDoneChgLot))
		nIdx = GetPcrIdx0(nSerial, TRUE); // 릴맵화면 표시 인덱스
	else
		nIdx = GetPcrIdx0(nSerial);

	m_pPcrIts[nIdx]->m_nIdx = nIdx;							// m_nIdx : From 0 to nTot.....
	m_pPcrIts[nIdx]->m_nSerial = nSerial;

	// Error Code											// 1(정상), -1(Align Error, 노광불량), -2(Lot End)
	if (m_pPcr[0][nIdx]->m_nErrPnl < 0)
		m_pPcrIts[nIdx]->m_nErrPnl = m_pPcr[0][nIdx]->m_nErrPnl;
	else if (m_pPcrInner[2][nIdx]->m_nErrPnl < 0)
		m_pPcrIts[nIdx]->m_nErrPnl = m_pPcrInner[2][nIdx]->m_nErrPnl;
	else
		m_pPcrIts[nIdx]->m_nErrPnl = m_pPcr[0][nIdx]->m_nErrPnl;

	// Model
	m_pPcrIts[nIdx]->m_sModel = m_pPcr[0][nIdx]->m_sModel;

	// Layer
	m_pPcrIts[nIdx]->m_sLayerOutUp = m_pPcr[0][nIdx]->m_sLayer;
	m_pPcrIts[nIdx]->m_sLayerOutDn = _T(""); // m_pPcr[1][nIdx]->m_sLayer;
	m_pPcrIts[nIdx]->m_sLayerInUp = m_pPcrInner[0][nIdx]->m_sLayer;
	m_pPcrIts[nIdx]->m_sLayerInDn = m_pPcrInner[1][nIdx]->m_sLayer;

	// Lot
	m_pPcrIts[nIdx]->m_sLot = m_pPcr[0][nIdx]->m_sLot;

	int nTotDef[3] = { 0 };									// [0]: 외층, [1]: 내층, [2]: 내/외층 Merge
	nTotDef[0] = m_pPcr[0][nIdx]->m_nTotDef;			// 외층 불량 피스 수
	nTotDef[1] = m_pPcrInner[2][nIdx]->m_nTotDef;		// 내층 불량 피스 수

	int nTotPcs = m_Master[0].m_pPcsRgn->nTotPcs;
	stPcrMerge *pPcrMgr = new stPcrMerge[nTotPcs];

	int nPcsId;												// nPcsId : CamMaster Pcs Index
															// pPcrMgr테이블의 nIdx에 내층의 불량을 먼저 기록하고 외층의 불량을 엎어서 최종 merge불량 테이블을 만듬.
	for (i = 0; i < nTotDef[1]; i++)						// 내층 불량 피스 수
	{
		nPcsId = m_pPcrInner[2][nIdx]->m_pDefPcs[i];		// nPcsId : CamMaster Pcs Index
		pPcrMgr[nPcsId].nIdx = i;	// 외층+내층			// 내외층 Merge [nPcsId:CamMaster Pcs Index]의 nIdx (0~)는 불량표시순서임.
		pPcrMgr[nPcsId].nIdxDn = i; // 내층					// 내외층 Merge [nPcsId:CamMaster Pcs Index]의 nIdxDn (0~)는 불량표시순서임.
	}
	for (i = 0; i < nTotDef[0]; i++)						// 외층 불량 피스 수
	{
		nPcsId = m_pPcr[0][nIdx]->m_pDefPcs[i];				// nPcsId : CamMaster Pcs Index
		pPcrMgr[nPcsId].nIdx = i;	// 외층+내층			// 내외층 Merge [nPcsId:CamMaster Pcs Index]의 nIdx (0~)는 불량표시순서임.
		pPcrMgr[nPcsId].nIdxUp = i;	// 외층					// 내외층 Merge [nPcsId:CamMaster Pcs Index]의 nIdxUp (0~)는 불량표시순서임.
	}


	nTotDef[2] = 0;
	for (i = 0; i < nTotPcs; i++)							// Shot내 총 Piece수
	{
		if (pPcrMgr[i].nIdx > -1)	// 외층+내층			// 내외층 Merge [i:CamMaster Pcs Index]의 nIdx (0~)는 불량표시순서임.
			nTotDef[2]++;									// 내외층 Merge한 총 불량피스수.
	}

	m_pPcrIts[nIdx]->Init(nSerial, nTotDef[2]);				// 제품시리얼, Shot내 총불량 피스수

	int nId[2], Ord;										// [0]: 외층 0~불량피스순서, [1]: 내층 0~불량피스순서
	idx = 0;												// 마킹순서 0~불량피스수만큼 정하기위해 현시점의 idx를 초기화함.
	if (nTotDef[2] > 0)										// 내외층 Merge한 총 불량피스수.
	{
		for (nPcsId = 0; nPcsId < nTotPcs; nPcsId++)
		{
			nId[0] = pPcrMgr[nPcsId].nIdxUp;				// 내외층 Merge [nPcsId:CamMaster Pcs Index]의 nIdxUp (0~)는 불량표시순서 임. nId[0]: 외층에서의 PCR파일순서 인덱스
			nId[1] = pPcrMgr[nPcsId].nIdxDn;				// 내외층 Merge [nPcsId:CamMaster Pcs Index]의 nIdxDn (0~)는 불량표시순서 임. nId[1]: 내층에서의 PCR파일순서 인덱스

			if (nId[0] > -1)
			{
				Ord = nId[0];
				// Cam ID
				m_pPcrIts[nIdx]->m_nCamId = m_pPcr[0][nIdx]->m_nCamId;
				// Piece Number
				m_pPcrIts[nIdx]->m_pDefPcs[idx] = m_pPcr[0][nIdx]->m_pDefPcs[Ord];
				m_pPcrIts[nIdx]->m_pLayer[idx] = m_pPcr[0][nIdx]->m_pLayer[Ord];
				// BadPointPosX
				m_pPcrIts[nIdx]->m_pDefPos[idx].x = m_pPcr[0][nIdx]->m_pDefPos[Ord].x;
				// BadPointPosY
				m_pPcrIts[nIdx]->m_pDefPos[idx].y = m_pPcr[0][nIdx]->m_pDefPos[Ord].y;
				// BadName
				m_pPcrIts[nIdx]->m_pDefType[idx] = m_pPcr[0][nIdx]->m_pDefType[Ord];
				// CellNum
				m_pPcrIts[nIdx]->m_pCell[idx] = m_pPcr[0][nIdx]->m_pCell[Ord];
				// ImageSize
				m_pPcrIts[nIdx]->m_pImgSz[idx] = m_pPcr[0][nIdx]->m_pImgSz[Ord];
				// ImageNum
				m_pPcrIts[nIdx]->m_pImg[idx] = m_pPcr[0][nIdx]->m_pImg[Ord];
				// strMarkingCode : -2 (NoMarking)
				m_pPcrIts[nIdx]->m_pMk[idx] = m_pPcr[0][nIdx]->m_pMk[Ord];

				idx++;
			}
			else if (nId[1] > -1)
			{
				Ord = nId[1];
				// Cam ID
				m_pPcrIts[nIdx]->m_nCamId = m_pPcrInner[2][nIdx]->m_nCamId;
				// Piece Number
				m_pPcrIts[nIdx]->m_pDefPcs[idx] = m_pPcrInner[2][nIdx]->m_pDefPcs[Ord];
				m_pPcrIts[nIdx]->m_pLayer[idx] = m_pPcrInner[2][nIdx]->m_pLayer[Ord];
				// BadPointPosX
				m_pPcrIts[nIdx]->m_pDefPos[idx].x = m_pPcrInner[2][nIdx]->m_pDefPos[Ord].x;
				// BadPointPosY
				m_pPcrIts[nIdx]->m_pDefPos[idx].y = m_pPcrInner[2][nIdx]->m_pDefPos[Ord].y;
				// BadName
				m_pPcrIts[nIdx]->m_pDefType[idx] = m_pPcrInner[2][nIdx]->m_pDefType[Ord];
				// CellNum
				m_pPcrIts[nIdx]->m_pCell[idx] = m_pPcrInner[2][nIdx]->m_pCell[Ord];
				// ImageSize
				m_pPcrIts[nIdx]->m_pImgSz[idx] = m_pPcrInner[2][nIdx]->m_pImgSz[Ord];
				// ImageNum
				m_pPcrIts[nIdx]->m_pImg[idx] = m_pPcrInner[2][nIdx]->m_pImg[Ord];
				// strMarkingCode : -2 (NoMarking)
				m_pPcrIts[nIdx]->m_pMk[idx] = m_pPcrInner[2][nIdx]->m_pMk[Ord];

				idx++;
			}
		}
	}
	delete[] pPcrMgr;
}

void CManagerReelmap::LoadPCRIts00(int nSerial) // 10 -> 외층 : 단면, 내층 : 단면
{
	pView->ClrDispMsg();
	pView->MsgBox(_T("LoadPCRIts00 - 내외층 모두 단면인 경우는 프로그램이 없습니다."));
	return;
}

BOOL CManagerReelmap::InitReelmapInner()
{
	if (!m_MasterInner[0].m_pPcsRgn)
	{
		CString strMsg;
		strMsg.Format(_T("피스 영역이 존재하지 않습니다."));
		//pView->MsgBox(strMsg);
		pView->ClrDispMsg();
		AfxMessageBox(strMsg, MB_ICONSTOP);
		return FALSE;
	}
	
	//GetCurrentInfoEng();
	int nTotPcs = m_MasterInner[0].m_pPcsRgn->GetTotPcs();
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;

	if (m_pReelMapInnerUp)
	{
		delete m_pReelMapInnerUp;
		m_pReelMapInnerUp = NULL;
	}
	m_pReelMapInnerUp = new CReelMap(RMAP_INNER_UP, MAX_DISP_PNL, nTotPcs, 0, this);

	if (bDualTest)
	{
		if (m_pReelMapInnerDn)
		{
			delete m_pReelMapInnerDn;
			m_pReelMapInnerDn = NULL;
		}
		m_pReelMapInnerDn = new CReelMap(RMAP_INNER_DN, MAX_DISP_PNL, nTotPcs, 0, this);

		if (m_pReelMapInnerAllUp)
		{
			delete m_pReelMapInnerAllUp;
			m_pReelMapInnerAllUp = NULL;
		}
		m_pReelMapInnerAllUp = new CReelMap(RMAP_INNER_ALLUP, MAX_DISP_PNL, nTotPcs, 0, this);

		if (m_pReelMapInnerAllDn)
		{
			delete m_pReelMapInnerAllDn;
			m_pReelMapInnerAllDn = NULL;
		}
		m_pReelMapInnerAllDn = new CReelMap(RMAP_INNER_ALLDN, MAX_DISP_PNL, nTotPcs, 0, this);
		m_pReelMapInner = m_pReelMapInnerAllUp;
	}
	else
	{
		m_pReelMapInner = m_pReelMapInnerUp;
	}

	if (pDoc->GetTestMode() == MODE_OUTER)
	{
		if(m_pReelMapIts)
			m_pReelMap = m_pReelMapIts;
	}

	return TRUE;
}

BOOL CManagerReelmap::InitReelmapInnerUp()
{
	if (!m_MasterInner[0].m_pPcsRgn)
	{
		CString strMsg;
		strMsg.Format(_T("피스 영역이 존재하지 않습니다."));
		pView->MsgBox(strMsg);
		//AfxMessageBox(strMsg,MB_ICONSTOP);
		return FALSE;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;
	int nTotPcs = m_MasterInner[0].m_pPcsRgn->GetTotPcs();

	if (m_pReelMapInnerUp)
	{
		//m_pReelMapUp->ResetReelmap();
		delete m_pReelMapInnerUp;
		m_pReelMapInnerUp = NULL;
	}
	m_pReelMapInnerUp = new CReelMap(RMAP_INNER_UP, MAX_DISP_PNL, nTotPcs, 0, this);

	if (bDualTest)
	{

		if (m_pReelMapInnerAllUp)
		{
			delete m_pReelMapInnerAllUp;
			m_pReelMapInnerAllUp = NULL;
		}
		m_pReelMapInnerAllUp = new CReelMap(RMAP_INNER_ALLUP, MAX_DISP_PNL, nTotPcs, 0, this);
		m_pReelMapInner = m_pReelMapInnerAllUp;
	}
	else
	{
		m_pReelMapInner = m_pReelMapInnerUp;
	}

	if (pDoc->GetTestMode() == MODE_OUTER)
	{
		m_pReelMap = m_pReelMapIts;
	}

	return TRUE;
}

BOOL CManagerReelmap::InitReelmapInnerDn()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;
	if (!bDualTest)
		return TRUE;

	if (!m_MasterInner[0].m_pPcsRgn)
	{
		CString strMsg;
		strMsg.Format(_T("피스 영역이 존재하지 않습니다."));
		pView->MsgBox(strMsg);
		//AfxMessageBox(strMsg,MB_ICONSTOP);
		return FALSE;
	}

	int nTotPcs = m_MasterInner[0].m_pPcsRgn->GetTotPcs();

	if (m_pReelMapInnerDn)
	{
		delete m_pReelMapInnerDn;
		m_pReelMapInnerDn = NULL;
	}
	m_pReelMapInnerDn = new CReelMap(RMAP_INNER_DN, MAX_DISP_PNL, nTotPcs, 0, this);

	if (m_pReelMapInnerAllDn)
	{
		delete m_pReelMapInnerAllDn;
		m_pReelMapInnerAllDn = NULL;
	}
	m_pReelMapInnerAllDn = new CReelMap(RMAP_INNER_ALLDN, MAX_DISP_PNL, nTotPcs, 0, this);

	return TRUE;
}

void CManagerReelmap::SetReelmapInner(int nDir)
{
	if (!m_pReelMapInner || !m_MasterInner[0].m_pPcsRgn)
		return;

	if (!m_pReelMapInner->pFrmRgn || !m_pReelMapInner->pPcsRgn)
		return;

	int i, k;
	double fData1, fData2, fData3, fData4, fDistX, fDistY;
	double fWidth, fHeight, fRight, fBottom;

	m_pReelMapInner->nDir = nDir;

	int nTotPnl = m_pReelMapInner->nTotPnl;
	int nTotPcs = m_pReelMapInner->nTotPcs;

	double dScale = (m_MasterInner[0].MasterInfo.dPixelSize / 10.0);
	m_pReelMapInner->SetAdjRatio(dScale);
	dScale = m_pReelMapInner->GetAdjRatio();

	for (k = 0; k < nTotPnl; k++)
	{
		switch (nDir)
		{
		case ROT_NONE:
			fWidth = (m_MasterInner[0].m_pPcsRgn->pPcs[0].right - m_MasterInner[0].m_pPcsRgn->pPcs[0].left);
			fHeight = (m_MasterInner[0].m_pPcsRgn->pPcs[0].bottom - m_MasterInner[0].m_pPcsRgn->pPcs[0].top);
			fRight = m_MasterInner[0].m_pPcsRgn->rtFrm.right - fWidth * (1.0 - RMAP_PCS_SCALE);
			fBottom = m_MasterInner[0].m_pPcsRgn->rtFrm.bottom - fHeight * (1.0 - RMAP_PCS_SCALE);;
			m_pReelMapInner->pFrmRgn[k].left = (m_MasterInner[0].m_pPcsRgn->rtFrm.right + MYGL_GAP_PNL*dScale)*(nTotPnl - 1 - k) + m_MasterInner[0].m_pPcsRgn->rtFrm.left;
			m_pReelMapInner->pFrmRgn[k].top = m_MasterInner[0].m_pPcsRgn->rtFrm.top;
			m_pReelMapInner->pFrmRgn[k].right = (m_MasterInner[0].m_pPcsRgn->rtFrm.right + MYGL_GAP_PNL*dScale)*(nTotPnl - 1 - k) + fRight;
			m_pReelMapInner->pFrmRgn[k].bottom = fBottom;
			//m_pReelMap->pFrmRgn[k].left = (m_pPcsRgn->rtFrm.right+MYGL_GAP_PNL*dScale)*(nTotPnl-1-k)+m_pPcsRgn->rtFrm.left;
			//m_pReelMap->pFrmRgn[k].top = m_pPcsRgn->rtFrm.top;
			//m_pReelMap->pFrmRgn[k].right = (m_pPcsRgn->rtFrm.right+MYGL_GAP_PNL*dScale)*(nTotPnl-1-k)+m_pPcsRgn->rtFrm.right;
			//m_pReelMap->pFrmRgn[k].bottom = m_pPcsRgn->rtFrm.bottom;

			for (i = 0; i < nTotPcs; i++)
			{
				fWidth = (m_MasterInner[0].m_pPcsRgn->pPcs[i].right - m_MasterInner[0].m_pPcsRgn->pPcs[i].left);
				fHeight = (m_MasterInner[0].m_pPcsRgn->pPcs[i].bottom - m_MasterInner[0].m_pPcsRgn->pPcs[i].top);
				fData1 = m_MasterInner[0].m_pPcsRgn->pPcs[i].left;	// left
				fData2 = m_MasterInner[0].m_pPcsRgn->pPcs[i].top;	// top
				fData3 = fData1 + fWidth * RMAP_PCS_SCALE; // right
				fData4 = fData2 + fHeight * RMAP_PCS_SCALE; // bottom

				m_pReelMapInner->pPcsRgn[k][i].left = (m_MasterInner[0].m_pPcsRgn->rtFrm.right + MYGL_GAP_PNL*dScale)*(nTotPnl - 1 - k) + fData1;
				m_pReelMapInner->pPcsRgn[k][i].top = fData2;
				m_pReelMapInner->pPcsRgn[k][i].right = (m_MasterInner[0].m_pPcsRgn->rtFrm.right + MYGL_GAP_PNL*dScale)*(nTotPnl - 1 - k) + fData3;
				m_pReelMapInner->pPcsRgn[k][i].bottom = fData4;
			}
			break;
		//case ROT_NONE:
		//	m_pReelMap->pFrmRgn[k].left = (m_pPcsRgn->rtFrm.right+MYGL_GAP_PNL*dScale)*(nTotPnl-1-k)+m_pPcsRgn->rtFrm.left;
		//	m_pReelMap->pFrmRgn[k].top = m_pPcsRgn->rtFrm.top;
		//	m_pReelMap->pFrmRgn[k].right = (m_pPcsRgn->rtFrm.right+MYGL_GAP_PNL*dScale)*(nTotPnl-1-k)+m_pPcsRgn->rtFrm.right;
		//	m_pReelMap->pFrmRgn[k].bottom = m_pPcsRgn->rtFrm.bottom;
		//	 
		//	for(i=0; i<nTotPcs; i++)
		//	{
		//	 	fData1 = m_pPcsRgn->pPcs[i].left; // left
		//	 	fData2 = m_pPcsRgn->pPcs[i].top; // top
		//	 	fData3 = m_pPcsRgn->pPcs[i].right; // right
		//	 	fData4 = m_pPcsRgn->pPcs[i].bottom; // bottom
		//	 
		//	 	m_pReelMap->pPcsRgn[k][i].left = (m_pPcsRgn->rtFrm.right+MYGL_GAP_PNL*dScale)*(nTotPnl-1-k)+fData1;
		//	 	m_pReelMap->pPcsRgn[k][i].top = fData2;
		//	 	m_pReelMap->pPcsRgn[k][i].right = (m_pPcsRgn->rtFrm.right+MYGL_GAP_PNL*dScale)*(nTotPnl-1-k)+fData3;
		//	 	m_pReelMap->pPcsRgn[k][i].bottom = fData4;
		//	}
		//	break;
		case ROT_CCW_90: // right->bottom, top->left, bottom->right, left->top ; Dir (x *= 1, y *= -1) 
			fDistX = 0;
			fDistY = m_MasterInner[0].m_pPcsRgn->rtFrm.left + m_MasterInner[0].m_pPcsRgn->rtFrm.right;
			m_pReelMapInner->pFrmRgn[k].left = (m_MasterInner[0].m_pPcsRgn->rtFrm.bottom + MYGL_GAP_PNL*dScale)*(nTotPnl - 1 - k) + m_MasterInner[0].m_pPcsRgn->rtFrm.top;
			m_pReelMapInner->pFrmRgn[k].top = fDistY - m_Master[0].m_pPcsRgn->rtFrm.right;
			m_pReelMapInner->pFrmRgn[k].right = (m_MasterInner[0].m_pPcsRgn->rtFrm.bottom + MYGL_GAP_PNL*dScale)*(nTotPnl - 1 - k) + m_MasterInner[0].m_pPcsRgn->rtFrm.bottom;
			m_pReelMapInner->pFrmRgn[k].bottom = fDistY - m_MasterInner[0].m_pPcsRgn->rtFrm.left;

			for (i = 0; i < nTotPcs; i++)
			{
				fData1 = m_MasterInner[0].m_pPcsRgn->pPcs[i].top;	// left
				fData2 = fDistY - m_MasterInner[0].m_pPcsRgn->pPcs[i].right;	// top
				fData3 = m_MasterInner[0].m_pPcsRgn->pPcs[i].bottom;	// right
				fData4 = fDistY - m_MasterInner[0].m_pPcsRgn->pPcs[i].left; // bottom

				m_pReelMapInner->pPcsRgn[k][i].left = (m_MasterInner[0].m_pPcsRgn->rtFrm.bottom + MYGL_GAP_PNL*dScale)*(nTotPnl - 1 - k) + fData1;
				m_pReelMapInner->pPcsRgn[k][i].top = fData2;
				m_pReelMapInner->pPcsRgn[k][i].right = (m_MasterInner[0].m_pPcsRgn->rtFrm.bottom + MYGL_GAP_PNL*dScale)*(nTotPnl - 1 - k) + fData3;
				m_pReelMapInner->pPcsRgn[k][i].bottom = fData4;
			}
			break;
		}
	}
}

CString CManagerReelmap::GetItsTargetPath(int nSerial, int nLayer)	// RMAP_UP, RMAP_DN, RMAP_INNER_UP, RMAP_INNER_DN
{
	CString sPath, str;
	CString sItsFolderPath = GetItsTargetFolderPath();
	CString sTime = pView->GetTimeIts();

	switch (nLayer)
	{
	case RMAP_UP: // 외층 Top
		str.Format(_T("%s_L1_%04d_T_%s_%s_AVR01_%s.dat"), m_sItsCode, nSerial, pDoc->WorkingInfo.LastJob.sSelUserName, pDoc->WorkingInfo.System.sMcName, sTime);
		sPath.Format(_T("%s\\%s"), sItsFolderPath, str);
		break;
	case RMAP_DN: // 외층 Bottom
		str.Format(_T("%s_L4_%04d_B_%s_%s_AVR01_%s.dat"), m_sItsCode, nSerial, pDoc->WorkingInfo.LastJob.sSelUserName, pDoc->WorkingInfo.System.sMcName, sTime);
		sPath.Format(_T("%s\\%s"), sItsFolderPath, str);
		break;
	case RMAP_INNER_UP: // 내층 Top
		str.Format(_T("%s_L2_%04d_T_%s_%s_AVR01_%s.dat"), m_sItsCode, nSerial, pDoc->WorkingInfo.LastJob.sSelUserName, pDoc->WorkingInfo.System.sMcName, sTime);
		sPath.Format(_T("%s\\%s"), sItsFolderPath, str);
		break;
	case RMAP_INNER_DN: // 내층 Bottom
		str.Format(_T("%s_L3_%04d_B_%s_%s_AVR01_%s.dat"), m_sItsCode, nSerial, pDoc->WorkingInfo.LastJob.sSelUserName, pDoc->WorkingInfo.System.sMcName, sTime);
		sPath.Format(_T("%s\\%s"), sItsFolderPath, str);
		break;
	}

	return sPath;
}

int CManagerReelmap::GetItsDefCode(int nDefCode)
{
	CString sDefCode;
	sDefCode.Format(_T("%c"), pDoc->m_cBigDefCode[nDefCode]);

	// [Sapp3Code]
	if (sDefCode == _T("N"))		//1 NICK = 137 -> m_nSapp3Code[SAPP3_NICK]
		return pDoc->m_nSapp3Code[SAPP3_NICK];
	else if (sDefCode == _T("D"))	//2 SPACE_EXTRA_PROTRUSION = 160 -> m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] : PROTRUSION
		return pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION];
	else if (sDefCode == _T("A"))	//3 SPACE_EXTRA_PROTRUSION = 160 -> m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] : SPACE 
		return pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION];
	else if (sDefCode == _T("O"))	//4 OPEN = 102 -> m_nSapp3Code[SAPP3_OPEN]
		return pDoc->m_nSapp3Code[SAPP3_OPEN];
	else if (sDefCode == _T("S"))	//5 SHORT = 129 -> m_nSapp3Code[SAPP3_SHORT]
		return pDoc->m_nSapp3Code[SAPP3_SHORT];
	else if (sDefCode == _T("U"))	//6 USHORT = 129 -> m_nSapp3Code[SAPP3_USHORT]
		return pDoc->m_nSapp3Code[SAPP3_USHORT];
	else if (sDefCode == _T("I"))	//7 PINHOLE = 134 -> m_nSapp3Code[SAPP3_PINHOLE]
		return pDoc->m_nSapp3Code[SAPP3_PINHOLE];
	else if (sDefCode == _T("H"))	//8 HMISS_HPOS_HBAD = 309 -> m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] : No Hole
		return pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD];
	else if (sDefCode == _T("E"))	//9 SPACE_EXTRA_PROTRUSION = 160 -> m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] : EXTRA
		return pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION];
	else if (sDefCode == _T("P"))	//10 PAD = 316 -> m_nSapp3Code[SAPP3_PAD]
		return pDoc->m_nSapp3Code[SAPP3_PAD];
	else if (sDefCode == _T("L"))	//11 HMISS_HPOS_HBAD = 309 -> m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] : Hole Align
		return pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD];
	else if (sDefCode == _T("X"))	//12 : POI -> m_nSapp3Code[SAPP3_SHORT]
		return pDoc->m_nSapp3Code[SAPP3_SHORT];
	else if (sDefCode == _T("T"))	//13 HMISS_HPOS_HBAD = 309 -> m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] : VH Align
		return pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD];
	else if (sDefCode == _T("M"))	//14 HMISS_HPOS_HBAD = 309 -> m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] : No VH
		return pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD];
	else if (sDefCode == _T("F"))	//15 HMISS_HPOS_HBAD = 309 -> m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] : Hole Defect
		return pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD];
	else if (sDefCode == _T("C"))	//16 HOPEN = 308 -> m_nSapp3Code[SAPP3_HOPEN]
		return pDoc->m_nSapp3Code[SAPP3_HOPEN];
	else if (sDefCode == _T("G"))	//17 HMISS_HPOS_HBAD = 309 -> m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] : VH Open
		return pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD];
	else if (sDefCode == _T("V"))	//18 HMISS_HPOS_HBAD = 309 -> m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] : VH Def
		return pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD];
	else if (sDefCode == _T("K"))	//19 E.Nick = 137 -> m_nSapp3Code[SAPP3_NICK]
		return pDoc->m_nSapp3Code[SAPP3_NICK];
	else if (sDefCode == _T("R"))	//20 E.Prot = 160 -> m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION]
		return pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION];
	else if (sDefCode == _T("B"))	//21 E.Space = 160 -> m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION]
		return pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION];
	else if (sDefCode == _T("J"))	//22 UDD1 = 160 -> m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION]
		return pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION];
	else if (sDefCode == _T("Q"))	//23 Narrow = 160 -> m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION]
		return pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION];
	else if (sDefCode == _T("W"))	//24 Wide = 160 -> m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION]
		return pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION];
	else if (sDefCode == _T("?"))	//25 Light = 160 -> m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION]
		return pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION];

	return 0;
}

BOOL CManagerReelmap::MakeItsDir(CString sModel, CString sLot, CString sLayer)
{
	CString sMsg = _T("");
	CFileFind finder;
	CString sPath;

	sPath.Format(_T("%s"), pDoc->WorkingInfo.System.sPathItsFile);
	int pos = sPath.ReverseFind('\\');
	if (pos != -1)
		sPath.Delete(pos, sPath.GetLength() - pos);

	if (!pDoc->DirectoryExists(sPath))
		CreateDirectory(sPath, NULL);

	if (sModel.IsEmpty() || sLot.IsEmpty() || sLayer.IsEmpty())
	{
		sMsg.Format(_T("모델이나 로뜨 또는 레이어명이 없습니다."));
		pView->ClrDispMsg();
		AfxMessageBox(sMsg);
		return FALSE;
	}

	sPath.Format(_T("%s%s"), pDoc->WorkingInfo.System.sPathItsFile, sModel);

	if (!pDoc->DirectoryExists(sPath))
		CreateDirectory(sPath, NULL);
	
	sPath.Format(_T("%s%s\\%s"), pDoc->WorkingInfo.System.sPathItsFile, sModel, pDoc->WorkingInfo.LastJob.sEngItsCode);
	if (!pDoc->DirectoryExists(sPath))
		CreateDirectory(sPath, NULL);

	sPath.Format(_T("%s%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathItsFile, sModel, pDoc->WorkingInfo.LastJob.sEngItsCode, sLot);
	if (!pDoc->DirectoryExists(sPath))
		CreateDirectory(sPath, NULL);

	sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathItsFile, sModel, pDoc->WorkingInfo.LastJob.sEngItsCode, sLot, sLayer);
	if (!pDoc->DirectoryExists(sPath))
		CreateDirectory(sPath, NULL);

	return TRUE;
}

int CManagerReelmap::GetAoiUpCamMstInfo()
{
	TCHAR szData[200];
	CString sPath;
	sPath.Format(_T("%s%s\\%s\\%s\\DataOut.ini"), pDoc->WorkingInfo.System.sPathAoiUpVrsData,
		pDoc->WorkingInfo.LastJob.sModelUp, pDoc->WorkingInfo.LastJob.sLayerUp, pDoc->WorkingInfo.LastJob.sLotUp);
		
	if (0 < ::GetPrivateProfileString(_T("Region"), _T("Piece Region Type"), NULL, szData, sizeof(szData), sPath))
		m_Master[0].MasterInfo.nOutFileOnAoi = _ttoi(szData);
	else
		m_Master[0].MasterInfo.nOutFileOnAoi = -1;

	return m_Master[0].MasterInfo.nOutFileOnAoi;
}

int CManagerReelmap::GetAoiDnCamMstInfo()
{
	TCHAR szData[200];
	CString sPath;
	sPath.Format(_T("%s%s\\%s\\%s\\DataOut.ini"), pDoc->WorkingInfo.System.sPathAoiDnVrsData,
		pDoc->WorkingInfo.LastJob.sModelUp, pDoc->WorkingInfo.LastJob.sLayerDn, pDoc->WorkingInfo.LastJob.sLotUp);

	if (0 < ::GetPrivateProfileString(_T("Region"), _T("Piece Region Type"), NULL, szData, sizeof(szData), sPath))
		m_Master[1].MasterInfo.nOutFileOnAoi = _ttoi(szData);
	else
		m_Master[1].MasterInfo.nOutFileOnAoi = -1;

	return m_Master[1].MasterInfo.nOutFileOnAoi;
}

int CManagerReelmap::SearchFirstShotOnIts()
{
	CString sName, sPath, Path[3], sMsg;

	Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
	Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
	//Path[1] = pDoc->m_sEngModel;
	Path[2] = m_sItsCode;

	sName.Format(_T("%s.txt"), m_sItsCode);
	sPath.Format(_T("%s%s\\%s\\%s"), Path[0], Path[1], Path[2], sName); // ITS_Code.txt

	CDataFile *pDataFile = new CDataFile;

	if (!pDataFile->Open(sPath))
	{
		sMsg.Format(_T("%s File not found."), sPath);
		pView->MsgBox(sMsg);
		delete pDataFile;
		return 0;
	}

	int i = 0, nLastShot = 0;
	CString sLine;
	int nTotLine = pDataFile->GetTotalLines();
	for (i = 1; i <= nTotLine; i++)
	{
		sLine = pDataFile->GetLineString(i);
		int nPos = sLine.Find(_T('['), 0);
		if (nPos >= 0)
		{
			sLine.Delete(0, nPos + 1);
			nPos = sLine.ReverseFind(_T(']'));
			sLine = sLine.Left(nPos);
			nLastShot = _ttoi(sLine);
			break;
		}
	}

	delete pDataFile;

	return nLastShot;
}

BOOL CManagerReelmap::GetItsSerialInfo(int nItsSerial, BOOL &bDualTest, CString &sLot, CString &sLayerUp, CString &sLayerDn, int nOption)		// 내층에서의 ITS 시리얼의 정보
{
	TCHAR szData[512];
	CString str, sName, sPath, Path[3];

	Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
	Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
	//Path[1] = pDoc->m_sEngModel;
	Path[2] = m_sItsCode;

	sName.Format(_T("%s.txt"), m_sItsCode);
	sPath.Format(_T("%s%s\\%s\\%s"), Path[0], Path[1], Path[2], sName); // ITS_Code.txt

	if (sPath.IsEmpty())
		return FALSE;

	CString strTemp;
	CFileFind finder;
	if (finder.FindFile(sPath) == FALSE)
	{
		strTemp.Format(_T("GetItsSerialInfo - Didn't find file.\r\n%s"), sPath);
		pView->MsgBox(strTemp);
		return FALSE;
	}

	if (nItsSerial == 0)
		nItsSerial = SearchFirstShotOnIts();

	CString sItsSerail;
	sItsSerail.Format(_T("%d"), nItsSerial);

	// ITS_Code.txt 파일의 정보
	//int nTestMode;
	//CString sProcessCode;


	// Option 1
	if (nOption == 0 || nOption == 1)
	{
		if (0 < ::GetPrivateProfileString(sItsSerail, _T("Dual Test"), NULL, szData, sizeof(szData), sPath))
			bDualTest = (_ttoi(szData) > 0) ? TRUE : FALSE;
		else
			bDualTest = FALSE;

		pDoc->WorkingInfo.LastJob.bDualTestInner = bDualTest;
	}

	// Option 2
	if (nOption == 0 || nOption == 2)
	{
		if (0 < ::GetPrivateProfileString(sItsSerail, _T("Current Lot"), NULL, szData, sizeof(szData), sPath))
			sLot = CString(szData);
		else
			sLot = _T("");

		pDoc->WorkingInfo.LastJob.sInnerLotUp = pDoc->WorkingInfo.LastJob.sInnerLotDn = sLot;
	}

	// Option 3
	if (nOption == 0 || nOption == 3)
	{
		if (0 < ::GetPrivateProfileString(sItsSerail, _T("Current Layer Up"), NULL, szData, sizeof(szData), sPath))
			sLayerUp = CString(szData);
		else
			sLayerUp = _T("");

		pDoc->WorkingInfo.LastJob.sInnerLayerUp = sLayerUp;
	}

	// Option 4
	if (nOption == 0 || nOption == 4)
	{
		if (bDualTest)
		{
			if (0 < ::GetPrivateProfileString(sItsSerail, _T("Current Layer Dn"), NULL, szData, sizeof(szData), sPath))
				sLayerDn = CString(szData);
			else
				sLayerDn = _T("");
		}
		else
			sLayerDn = _T("");

		pDoc->WorkingInfo.LastJob.sInnerLayerDn = sLayerDn;
	}

	return TRUE;
}

void CManagerReelmap::GetAlignPos(CfPoint &Pos1, CfPoint &Pos2)
{
	Pos1.x = m_Master[0].m_stAlignMk.X0;
	Pos1.y = m_Master[0].m_stAlignMk.Y0;
	Pos2.x = m_Master[0].m_stAlignMk.X1;
	Pos2.y = m_Master[0].m_stAlignMk.Y1;
}

BOOL CManagerReelmap::ReloadReelmapInner()
{
	double dRatio = 0.0;
	CString sVal = _T("");

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;

	BOOL bRtn[7] = { 1 };

	if (m_pReelMapInnerUp)
		bRtn[1] = m_pReelMapInnerUp->ReloadReelmap();

	if (m_pReelMapIts)
		bRtn[2] = m_pReelMapIts->ReloadReelmap();


	if (bDualTest)
	{
		if (m_pReelMapInnerDn)
			bRtn[4] = m_pReelMapInnerDn->ReloadReelmap();
		if (m_pReelMapInnerAllUp)
			bRtn[5] = m_pReelMapInnerAllUp->ReloadReelmap();
		if (m_pReelMapInnerAllDn)
			bRtn[6] = m_pReelMapInnerAllDn->ReloadReelmap();
	}

	for (int i = 1; i < 7; i++)
	{
		if (!bRtn[i])
			return FALSE;
	}

	return TRUE;
}

BOOL CManagerReelmap::IsDoneReloadReelmapInner(int& nProc)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;

	int nRatio[7] = { 0 };
	BOOL bDone[7] = { 0 };

	bDone[0] = TRUE;

	if (m_pReelMapInnerUp)
	{
		nRatio[1] = m_pReelMapInnerUp->GetProgressReloadReelmap();
		bDone[1] = m_pReelMapInnerUp->IsDoneReloadReelmap();
	}
	else
		bDone[1] = TRUE;

	if (m_pReelMapIts)
	{
		nRatio[2] = m_pReelMapIts->GetProgressReloadReelmap();
		bDone[2] = m_pReelMapIts->IsDoneReloadReelmap();
	}
	else
		bDone[2] = TRUE;

	bDone[3] = TRUE;

	if (bDualTest)
	{
		if (m_pReelMapInnerDn)
		{
			nRatio[4] = m_pReelMapInnerDn->GetProgressReloadReelmap();
			bDone[4] = m_pReelMapInnerDn->IsDoneReloadReelmap();
		}
		else
			bDone[4] = TRUE;

		if (m_pReelMapInnerAllUp)
		{
			nRatio[5] = m_pReelMapInnerAllUp->GetProgressReloadReelmap();
			bDone[5] = m_pReelMapInnerAllUp->IsDoneReloadReelmap();
		}
		else
			bDone[5] = TRUE;

		if (m_pReelMapAllDn)
		{
			nRatio[6] = m_pReelMapInnerAllDn->GetProgressReloadReelmap();
			bDone[6] = m_pReelMapInnerAllDn->IsDoneReloadReelmap();
		}
		else
			bDone[6] = TRUE;
	}
	else
	{
		bDone[4] = TRUE;
		bDone[5] = TRUE;
		bDone[6] = TRUE;
	}

	nProc = nRatio[0] + nRatio[1] + nRatio[2] + nRatio[3] + nRatio[4] + nRatio[5] + nRatio[6];

	if (bDone[0] && bDone[1] && bDone[2] && bDone[3] && bDone[4] && bDone[5] && bDone[6])
		return TRUE;

	return FALSE;
}

BOOL CManagerReelmap::ReloadReelmap()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	BOOL bDualTestInner = pDoc->WorkingInfo.LastJob.bDualTestInner;


	BOOL bRtn[5] = { 1 };
	int nSt = 0;

	if (m_pReelMapUp)
		bRtn[1] = m_pReelMapUp->ReloadReelmap();

	if (bDualTest)
	{
		if (m_pReelMapDn)
			bRtn[2] = m_pReelMapDn->ReloadReelmap();

		if (m_pReelMapAllUp)
			bRtn[3] = m_pReelMapAllUp->ReloadReelmap();

		if (m_pReelMapAllDn)
			bRtn[4] = m_pReelMapAllDn->ReloadReelmap();
	}

	if (pDoc->GetTestMode() == MODE_OUTER)
	{
		bRtn[0] = ReloadReelmapInner();
	}
	else
		nSt = 1;

	for (int i = nSt; i < 5; i++)
	{
		if (!bRtn[i])
		{
			AfxMessageBox(_T("ReloadReelmap() is Failed."));
			return FALSE;
		}
	}

	double dRatio = 0.0;
	CString sVal = _T("");
	CDlgProgress dlg;
	sVal.Format(_T("On Reloading Reelmap."));
	dlg.Create(sVal);

	int nRatio[5] = { 0 };
	BOOL bDone[5] = { 0 };
	int nTo = 0;
	if (bDualTest)
	{
		if (pDoc->GetTestMode() == MODE_OUTER)
		{
			if (bDualTestInner)
				nTo = 400 + 500; //[%]
			else
				nTo = 400 + 200; //[%]
		}
		else
		{
			nTo = 400; //[%]
		}
	}
	else
	{
		if (pDoc->GetTestMode() == MODE_OUTER)
		{
			if (bDualTestInner)
				nTo = 100 + 500; //[%]
			else
				nTo = 100 + 200; //[%]
		}
		else
		{
			nTo = 100; //[%]
		}
	}


	dlg.SetRange(0, nTo);
	dlg.SetPos(1);

	int nProc = 0, nProcOutter = 0, nProcInner = 0;
	for (nProc = 0; nProc < nTo;)
	{
		if (pDoc->GetTestMode() == MODE_OUTER)
		{
			if ((bDone[0] = IsDoneReloadReelmap(nProcOutter)) && (bDone[1] = IsDoneReloadReelmapInner(nProcInner)))
				break;
			nProc = nProcOutter + nProcInner;
		}
		else
		{
			if (bDone[0] = IsDoneReloadReelmap(nProc))
				break;
		}
		dlg.SetPos(nProc);
		Sleep(100);
	}

	dlg.DestroyWindow();


	if (pDoc->GetTestMode() == MODE_OUTER)
	{
		if (!bDone[0] || !bDone[1])
		{
			AfxMessageBox(_T("IsDoneReloadReelmap or IsDoneReloadReelmapInner is FALSE."));
			return FALSE;
		}
	}
	else
	{
		if (!bDone[0])
		{
			AfxMessageBox(_T("IsDoneReloadReelmap is FALSE."));
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CManagerReelmap::IsDoneReloadReelmap(int& nProc)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	int nRatio[5] = { 0 };
	BOOL bDone[5] = { 0 };

	if (m_pReelMapUp)
	{
		nRatio[1] = m_pReelMapUp->GetProgressReloadReelmap();
		bDone[1] = m_pReelMapUp->IsDoneReloadReelmap();
	}
	else
		bDone[1] = TRUE;

	if (bDualTest)
	{
		if (m_pReelMapDn)
		{
			nRatio[2] = m_pReelMapDn->GetProgressReloadReelmap();
			bDone[2] = m_pReelMapDn->IsDoneReloadReelmap();
		}
		else
			bDone[2] = TRUE;
		if (m_pReelMapAllUp)
		{
			nRatio[3] = m_pReelMapAllUp->GetProgressReloadReelmap();
			bDone[3] = m_pReelMapAllUp->IsDoneReloadReelmap();
		}
		else
			bDone[3] = TRUE;

		if (m_pReelMapAllDn)
		{
			nRatio[4] = m_pReelMapAllDn->GetProgressReloadReelmap();
			bDone[4] = m_pReelMapAllDn->IsDoneReloadReelmap();
		}
		else
			bDone[4] = TRUE;
	}

	nProc = nRatio[1] + nRatio[2] + nRatio[3] + nRatio[4];

	if (bDone[1] && bDone[2] && bDone[3] && bDone[4])
		return TRUE;

	return FALSE;
}

void CManagerReelmap::ReloadReelmapUp()
{
	if (m_pReelMapUp)
		m_pReelMapUp->ReloadReelmap();
}

void CManagerReelmap::ReloadReelmapAllUp()
{
	if (m_pReelMapAllUp)
		m_pReelMapAllUp->ReloadReelmap();
}

void CManagerReelmap::ReloadReelmapDn()
{
	if (m_pReelMapDn)
		m_pReelMapDn->ReloadReelmap();
}

void CManagerReelmap::ReloadReelmapAllDn()
{
	if (m_pReelMapAllDn)
		m_pReelMapAllDn->ReloadReelmap();
}

BOOL CManagerReelmap::ReloadReelmap(int nSerial)
{
	if (!pView || !pView->m_mgrProcedure)
		return FALSE;

	return pView->m_mgrProcedure->ReloadReelmap(nSerial);
}

void CManagerReelmap::ReloadReelmapUpInner()
{
	if (m_pReelMapInnerUp)
		m_pReelMapInnerUp->ReloadReelmap();
}

void CManagerReelmap::ReloadReelmapAllUpInner()
{
	if (m_pReelMapInnerAllUp)
		m_pReelMapInnerAllUp->ReloadReelmap();
}

void CManagerReelmap::ReloadReelmapDnInner()
{
	if (m_pReelMapInnerDn)
		m_pReelMapInnerDn->ReloadReelmap();
}

void CManagerReelmap::ReloadReelmapAllDnInner()
{
	if (m_pReelMapInnerAllDn)
		m_pReelMapInnerAllDn->ReloadReelmap();
}

void CManagerReelmap::ReloadReelmapIts()
{
	if (m_pReelMapIts)
		m_pReelMapIts->ReloadReelmap();
}

BOOL CManagerReelmap::OpenReelmapFromBuf(int nSerial)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	stModelInfo stInfoUp, stInfoDn;
	CString sSrc;
	sSrc.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufUp, nSerial);
	if (!GetPcrInfo(sSrc, stInfoUp))
	{
		pView->DispStsBar(_T("E(6)"), 5);
		pView->ClrDispMsg();
		AfxMessageBox(_T("Error-GetPcrInfo(6)"));
		return FALSE;
	}
	if (bDualTest)
	{
		sSrc.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufDn, nSerial);
		if (!GetPcrInfo(sSrc, stInfoDn))
		{
			pView->DispStsBar(_T("E(7)"), 5);
			pView->ClrDispMsg();
			AfxMessageBox(_T("Error-GetPcrInfo(7)"));
			return FALSE;
		}
	}

	if (m_pReelMapUp)
		m_pReelMapUp->Open();

	if (bDualTest)
	{
		if (m_pReelMapDn)
			m_pReelMapDn->Open();
		if (m_pReelMapAllUp)
			m_pReelMapAllUp->Open();
		if (m_pReelMapAllDn)
			m_pReelMapAllDn->Open();
	}

	return TRUE;
}

void CManagerReelmap::OpenReelmap()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (m_pReelMapUp)
		m_pReelMapUp->Open();

	if (bDualTest)
	{
		if (m_pReelMapDn)
			m_pReelMapDn->Open();
		if (m_pReelMapAllUp)
			m_pReelMapAllUp->Open();
		if (m_pReelMapAllDn)
			m_pReelMapAllDn->Open();
	}
}

void CManagerReelmap::OpenReelmapUp()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (m_pReelMapUp)
		m_pReelMapUp->Open();

	if (bDualTest)
	{
		if (m_pReelMapAllUp)
			m_pReelMapAllUp->Open();
	}
}

void CManagerReelmap::OpenReelmapDn()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return;

	if (m_pReelMapDn)
		m_pReelMapDn->Open();
	if (m_pReelMapAllDn)
		m_pReelMapAllDn->Open();
}

void CManagerReelmap::SetPathAtBuf()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (m_pReelMapUp)
		m_pReelMapUp->SetPathAtBuf();
	if (bDualTest)
	{
		if (m_pReelMapDn)
			m_pReelMapDn->SetPathAtBuf();
		if (m_pReelMapAllUp)
			m_pReelMapAllUp->SetPathAtBuf();
		if (m_pReelMapAllDn)
			m_pReelMapAllDn->SetPathAtBuf();
	}

	if (pDoc->GetTestMode() == MODE_OUTER)
	{
		if (m_pReelMapIts)
			m_pReelMapIts->SetPathAtBuf();
	}
}

void CManagerReelmap::SetPathAtBufUp()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (m_pReelMapUp)
		m_pReelMapUp->SetPathAtBuf();

	if (bDualTest)
	{
		if (m_pReelMapAllUp)
			m_pReelMapAllUp->SetPathAtBuf();
	}

	if (pDoc->GetTestMode() == MODE_OUTER)
	{
		SetInnerPathAtBufUp();
	}
}

void CManagerReelmap::SetPathAtBufDn()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return;

	if (m_pReelMapDn)
		m_pReelMapDn->SetPathAtBuf();
	if (m_pReelMapAllDn)
		m_pReelMapAllDn->SetPathAtBuf();

	if (pDoc->GetTestMode() == MODE_OUTER)
	{
		SetInnerPathAtBufDn();
	}
}

void CManagerReelmap::SetInnerPathAtBufUp()
{
	//BOOL bDualTest = pDoc->WorkingInfo.LastJob.bInnerDualTest;

	//if (m_pReelMapInnerUp)
	//	m_pReelMapInnerUp->SetPathAtBuf(GetRmapPath(RMAP_INNER_UP));

	//if (bDualTest)
	//{
	//	if (m_pReelMapInnerAllUp)
	//		m_pReelMapInnerAllUp->SetPathAtBuf(GetRmapPath(RMAP_INNER_ALLUP));
	//}

	//if (m_pReelMapInOuterUp)
	//	m_pReelMapInOuterUp->SetPathAtBuf(GetRmapPath(RMAP_INOUTER_UP));

}

void CManagerReelmap::SetInnerPathAtBufDn()
{
	//BOOL bDualTest = pDoc->WorkingInfo.LastJob.bInnerDualTest;

	//if (bDualTest)
	//{
	//	if (m_pReelMapInnerDn)
	//		m_pReelMapInnerDn->SetPathAtBuf(GetRmapPath(RMAP_INNER_DN));
	//	if (m_pReelMapInnerAllDn)
	//		m_pReelMapInnerAllDn->SetPathAtBuf(GetRmapPath(RMAP_INNER_ALLDN));
	//}

	//bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	//if (m_pReelMapInOuterUp)
	//	m_pReelMapInOuterUp->SetPathAtBuf(GetRmapPath(RMAP_INOUTER_UP));

	//if (bDualTest)
	//{
	//	if (m_pReelMapInOuterDn)
	//		m_pReelMapInOuterDn->SetPathAtBuf(GetRmapPath(RMAP_INOUTER_DN));
	//}

}

BOOL CManagerReelmap::ReloadReelmapInner(int nSerial)
{
	//GetCurrentInfoEng();
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;

	BOOL bRtn[7] = { 0 };
	if (m_pReelMapInner)
		bRtn[0] = m_pReelMapInner->ReloadReelmap(nSerial);
	if (m_pReelMapInnerUp)
		bRtn[1] = m_pReelMapInnerUp->ReloadReelmap(nSerial);

	//if (m_pReelMapInOuterUp)
	//	bRtn[2] = m_pReelMapInOuterUp->ReloadReelmap(nSerial);

	//if (m_pReelMapInOuterDn)
	//	bRtn[3] = m_pReelMapInOuterDn->ReloadReelmap(nSerial);
	bRtn[3] = TRUE;

	if (m_pReelMapIts)
		bRtn[2] = m_pReelMapIts->ReloadReelmap(nSerial);

	if (bDualTest)
	{
		if (m_pReelMapInnerDn)
			bRtn[4] = m_pReelMapInnerDn->ReloadReelmap(nSerial);
		if (m_pReelMapInnerAllUp)
			bRtn[5] = m_pReelMapInnerAllUp->ReloadReelmap(nSerial);
		if (m_pReelMapInnerAllDn)
			bRtn[6] = m_pReelMapInnerAllDn->ReloadReelmap(nSerial);

		for (int i = 0; i < 7; i++)
		{
			if (!bRtn[i])
				return FALSE;
		}
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			if (!bRtn[i])
				return FALSE;
		}
	}

	return TRUE;
}

void CManagerReelmap::OpenReelmapInner()
{
	//GetCurrentInfoEng();
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;

	if (m_pReelMapInnerUp)
		m_pReelMapInnerUp->Open();

	if (m_pReelMapIts)
		m_pReelMapIts->Open();

	if (bDualTest)
	{
		if (m_pReelMapInnerDn)
			m_pReelMapInnerDn->Open();
		if (m_pReelMapInnerAllUp)
			m_pReelMapInnerAllUp->Open();
		if (m_pReelMapInnerAllDn)
			m_pReelMapInnerAllDn->Open();
	}

	//if (m_pReelMapInner)
	//{
	//	if (m_pReelMapInner->m_nLayer < 0)
	//		m_pReelMapInner->m_nLayer = pView->m_nSelRmapInner;
	//	m_pReelMapInner->Open();
	//}

	//if (m_pReelMapInOuterUp)
	//	m_pReelMapInOuterUp->Open(GetRmapPath(RMAP_INOUTER_UP));

	//bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	//if (bDualTest)
	//{
	//	if (m_pReelMapInOuterDn)
	//		m_pReelMapInOuterDn->Open(GetRmapPath(RMAP_INOUTER_UP));
	//}
}

void CManagerReelmap::OpenReelmapInnerUp()
{
	//GetCurrentInfoEng();
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;

	if (m_pReelMapInnerUp)
		m_pReelMapInnerUp->Open();

	if (bDualTest)
	{
		if (m_pReelMapInnerAllUp)
			m_pReelMapInnerAllUp->Open();
	}
}

void CManagerReelmap::OpenReelmapInnerDn()
{
	//GetCurrentInfoEng();
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;

	if (!bDualTest)
		return;

	if (m_pReelMapInnerDn)
		m_pReelMapInnerDn->Open();
	if (m_pReelMapInnerAllDn)
		m_pReelMapInnerAllDn->Open();
}

BOOL CManagerReelmap::ChkYield() //(double &dTotLmt, double &dPrtLmt, double &dRatio) // 수율 양호 : TRUE , 수율 불량 : FALSE
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	int nGood = 0, nBad = 0, nTot;
	double dRatio;
	//dRatio = 0.0; dTotLmt = 0.0; dPrtLmt = 0.0;
	CString sMsg;

	if (bDualTest)
	{
		if (m_pReelMapAllDn)
			m_pReelMapAllDn->GetPcsNum(nGood, nBad);
		else
		{
			pView->Stop();
			sMsg.Format(_T("일시정지 - Failed ChkYield()."));
			pView->MsgBox(sMsg);
			//pView->TowerLamp(RGB_RED, TRUE);
			pView->Buzzer(TRUE, 0);
			return FALSE;
		}
	}
	else
	{
		if (m_pReelMapUp)
			m_pReelMapUp->GetPcsNum(nGood, nBad);
		else
		{
			pView->Stop();
			sMsg.Format(_T("일시정지 - Failed ChkYield()."));
			pView->MsgBox(sMsg);
			//pView->TowerLamp(RGB_RED, TRUE);
			pView->Buzzer(TRUE, 0);
			return FALSE;
		}
	}

	nTot = (nGood + nBad);
	double dTotLmt = _tstof(pDoc->WorkingInfo.LastJob.sLmtTotYld);
	if (dTotLmt > 0.0)
	{
		if (nTot > 0)
			dRatio = ((double)nGood / (double)nTot) * 100.0;
		else
			dRatio = 0.0;

		if (dRatio < dTotLmt)
		{
			pView->Stop();
			sMsg.Format(_T("일시정지 - 전체 수율 제한범위 : %.1f 미달 ( %.1f )"), dTotLmt, dRatio);
			pView->MsgBox(sMsg);
			//pView->TowerLamp(RGB_RED, TRUE);
			pView->Buzzer(TRUE, 0);
			return FALSE;
		}
	}
	dTotLmt = 0.0;

	double dPrtLmt = _tstof(pDoc->WorkingInfo.LastJob.sLmtPatlYld);

	if (dPrtLmt > 0.0)
	{
		if (bDualTest)
		{
			if (m_Yield[2].IsValid())
			{
				dRatio = m_Yield[2].GetYield();

				if (dRatio < dPrtLmt)
				{
					pView->Stop();
					sMsg.Format(_T("일시정지 - 구간 수율 제한범위 : %.1f 미달 ( %.1f )"), dPrtLmt, dRatio);
					pView->MsgBox(sMsg);
					//pView->TowerLamp(RGB_RED, TRUE);
					pView->Buzzer(TRUE, 0);
					return FALSE;
				}
			}
		}
		else
		{
			if (m_Yield[0].IsValid())
			{
				dRatio = m_Yield[0].GetYield();

				if (dRatio < dPrtLmt)
				{
					pView->Stop();
					sMsg.Format(_T("일시정지 - 구간 수율 제한범위 : %.1f 미달 ( %.1f )"), dPrtLmt, dRatio);
					pView->MsgBox(sMsg);
					//pView->TowerLamp(RGB_RED, TRUE);
					pView->Buzzer(TRUE, 0);
					return FALSE;
				}
			}
		}
	}
	dPrtLmt = 0.0;

	return TRUE;
}


UINT CManagerReelmap::ThreadProc0(LPVOID lpContext)	// UpdateYield()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerReelmap* pThread = reinterpret_cast<CManagerReelmap*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[0] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[3].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[0] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pView->m_mgrProcedure)
		{
			if (pView->m_mgrProcedure->m_bTHREAD_UPDATAE_YIELD[0])
			{
				pThread->UpdateYield(pView->m_mgrProcedure->m_nSerialTHREAD_UPDATAE_YIELD[0]);
				pView->m_mgrProcedure->m_bTHREAD_UPDATAE_YIELD[0] = FALSE;
				Sleep(0);
			}
			else if (pView->m_mgrProcedure->m_bTHREAD_UPDATAE_YIELD[1] && !pView->m_mgrProcedure->m_bTHREAD_UPDATE_YIELD_UP)
			{
				if (pDoc->WorkingInfo.LastJob.bDualTest)
				{
					if (!pView->m_mgrProcedure->m_bTHREAD_UPDATE_YIELD_DN && !pView->m_mgrProcedure->m_bTHREAD_UPDATE_YIELD_ALLUP && !pView->m_mgrProcedure->m_bTHREAD_UPDATE_YIELD_ALLDN)
					{
						pThread->UpdateYield(pView->m_mgrProcedure->m_nSerialTHREAD_UPDATAE_YIELD[1]);
						pView->m_mgrProcedure->m_bTHREAD_UPDATAE_YIELD[1] = FALSE;
						Sleep(0);
					}
				}
				else
				{
					pView->m_mgrProcedure->m_bTHREAD_UPDATAE_YIELD[1] = FALSE;
					pThread->UpdateYield(pView->m_mgrProcedure->m_nSerialTHREAD_UPDATAE_YIELD[1]);
					Sleep(0);
				}
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[0] = FALSE;

	return 0;
}

UINT CManagerReelmap::ThreadProc1(LPVOID lpContext)	// UpdateRMapUp()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerReelmap* pThread = reinterpret_cast<CManagerReelmap*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[1] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[1].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[1] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pView->m_mgrProcedure)
		{
			if (pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_UP) // Write Reelmap
			{
				if (!pView->m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_UP) // Yield Reelmap
				{
					pThread->UpdateRMapUp(); // Write Reelmap
					pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_UP = FALSE;
					Sleep(0);
				}
				else
					Sleep(30);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[1] = FALSE;

	return 0;
}

UINT CManagerReelmap::ThreadProc2(LPVOID lpContext)	// UpdateRMapDn()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerReelmap* pThread = reinterpret_cast<CManagerReelmap*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[2] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[2].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[2] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pView->m_mgrProcedure)
		{
			if (pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_DN) // Write Reelmap
			{
				//pThread->m_bTHREAD_UPDATE_REELMAP_ALLUP = TRUE;
				//pThread->m_bTHREAD_UPDATE_REELMAP_ALLDN = TRUE;
				if (!pView->m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_DN) // Yield Reelmap
				{
					pThread->UpdateRMapDn(); // Write Reelmap
					pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_DN = FALSE;
					Sleep(0);
				}
				else
					Sleep(30);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[2] = FALSE;

	return 0;
}

UINT CManagerReelmap::ThreadProc3(LPVOID lpContext)	// UpdateRMapAllUp()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerReelmap* pThread = reinterpret_cast<CManagerReelmap*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[3] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[3].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[3] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pView->m_mgrProcedure)
		{
			if (pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_ALLUP) // Write Reelmap
			{
				if (!pView->m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_ALLUP) // Yield Reelmap
				{
					pThread->UpdateRMapAllUp(); // Write Reelmap
					pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_ALLUP = FALSE;
					Sleep(0);
				}
				else
					Sleep(30);
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

UINT CManagerReelmap::ThreadProc4(LPVOID lpContext)	// UpdateRMapAllDn()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerReelmap* pThread = reinterpret_cast<CManagerReelmap*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[4] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[4].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[4] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pView->m_mgrProcedure)
		{
			if (pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_ALLDN) // Write Reelmap
			{
				if (!pView->m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_ALLDN) // Yield Reelmap
				{
					pThread->UpdateRMapAllDn(); // Write Reelmap
					pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_ALLDN = FALSE;
					Sleep(0);
				}
				else
					Sleep(30);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[4] = FALSE;

	return 0;
}

UINT CManagerReelmap::ThreadProc5(LPVOID lpContext)	// UpdateReelmapYieldUp()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerReelmap* pThread = reinterpret_cast<CManagerReelmap*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[5] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[5].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[5] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pView->m_mgrProcedure)
		{
			if (pView->m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_UP) // Yield Reelmap
			{
				if (!pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_UP) // Write Reelmap
				{
					pThread->UpdateReelmapYieldUp(); // Yield Reelmap
					pView->m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_UP = FALSE;
					if (!pDoc->WorkingInfo.LastJob.bDualTest)
					{
						if (pDoc->GetTestMode() == MODE_OUTER)
							pView->m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_ITS = TRUE;
					}

					Sleep(0);
				}
				else
					Sleep(30);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[5] = FALSE;

	return 0;
}

UINT CManagerReelmap::ThreadProc6(LPVOID lpContext)	// UpdateReelmapYieldDn()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerReelmap* pThread = reinterpret_cast<CManagerReelmap*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[6] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[6].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[6] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pView->m_mgrProcedure)
		{
			if (pView->m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_DN) // Yield Reelmap
			{
				if (!pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_DN) // Write Reelmap
				{
					pThread->UpdateReelmapYieldDn(); // Yield Reelmap
					pView->m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_DN = FALSE;
					pView->m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_ALLUP = TRUE;
					pView->m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_ALLDN = TRUE;
					Sleep(0);
				}
				else
					Sleep(30);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[6] = FALSE;

	return 0;
}

UINT CManagerReelmap::ThreadProc7(LPVOID lpContext)	// UpdateReelmapYieldAllUp()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerReelmap* pThread = reinterpret_cast<CManagerReelmap*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[7] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[7].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[7] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pView->m_mgrProcedure)
		{
			if (pView->m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_ALLUP) // Yield Reelmap
			{
				if (!pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_ALLUP) // Write Reelmap
				{
					pThread->UpdateReelmapYieldAllUp(); // Yield Reelmap
					pView->m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_ALLUP = FALSE;
					if (pDoc->GetTestMode() == MODE_OUTER)
						pView->m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_ITS = TRUE;
					Sleep(0);
				}
				else
					Sleep(30);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[7] = FALSE;

	return 0;
}

UINT CManagerReelmap::ThreadProc8(LPVOID lpContext)	// UpdateReelmapYieldAllDn()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerReelmap* pThread = reinterpret_cast<CManagerReelmap*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[8] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[8].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[8] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pView->m_mgrProcedure)
		{
			if (pView->m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_ALLDN) // Yield Reelmap
			{
				if (!pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_ALLDN) // Write Reelmap
				{
					pThread->UpdateReelmapYieldAllDn(); // Yield Reelmap
					pView->m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_ALLDN = FALSE;
					Sleep(0);
				}
				else
					Sleep(30);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[8] = FALSE;

	return 0;
}

UINT CManagerReelmap::ThreadProc9(LPVOID lpContext)	// ReloadReelmapUp()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerReelmap* pThread = reinterpret_cast<CManagerReelmap*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[9] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[9].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[9] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pView->m_mgrProcedure)
		{
			if (pView->m_mgrProcedure->m_bTHREAD_RELOAD_RST_UP)
			{
				pThread->ReloadReelmapUp();
				pView->m_mgrProcedure->m_bTHREAD_RELOAD_RST_UP = FALSE;
				Sleep(0);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[9] = FALSE;

	return 0;
}

UINT CManagerReelmap::ThreadProc10(LPVOID lpContext)	// ReloadReelmapDn()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerReelmap* pThread = reinterpret_cast<CManagerReelmap*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[10] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[10].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[10] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pView->m_mgrProcedure)
		{
			if (pView->m_mgrProcedure->m_bTHREAD_RELOAD_RST_DN)
			{
				pThread->ReloadReelmapDn();
				pView->m_mgrProcedure->m_bTHREAD_RELOAD_RST_DN = FALSE;
				Sleep(0);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[10] = FALSE;

	return 0;
}

UINT CManagerReelmap::ThreadProc11(LPVOID lpContext)	// ReloadReelmapAllUp()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerReelmap* pThread = reinterpret_cast<CManagerReelmap*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[11] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[11].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[11] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pView->m_mgrProcedure)
		{
			if (pView->m_mgrProcedure->m_bTHREAD_RELOAD_RST_ALLUP)
			{
				pThread->ReloadReelmapAllUp();
				pView->m_mgrProcedure->m_bTHREAD_RELOAD_RST_ALLUP = FALSE;
				Sleep(0);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[11] = FALSE;

	return 0;
}

UINT CManagerReelmap::ThreadProc12(LPVOID lpContext)	// ReloadReelmapAllDn()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerReelmap* pThread = reinterpret_cast<CManagerReelmap*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[12] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[12].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[12] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pView->m_mgrProcedure)
		{
			if (pView->m_mgrProcedure->m_bTHREAD_RELOAD_RST_ALLDN)
			{
				pThread->ReloadReelmapAllDn();
				pView->m_mgrProcedure->m_bTHREAD_RELOAD_RST_ALLDN = FALSE;
				Sleep(0);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[12] = FALSE;

	return 0;
}

UINT CManagerReelmap::ThreadProc13(LPVOID lpContext) // WriteReelmapIts()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerReelmap* pThread = reinterpret_cast<CManagerReelmap*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[13] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[13].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[13] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pView->m_mgrProcedure)
		{
			if (pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_ITS) // Write Reelmap
			{
				pThread->WriteReelmapIts();
				pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_ITS = FALSE;
				Sleep(0);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[13] = FALSE;

	return 0;
}

UINT CManagerReelmap::ThreadProc14(LPVOID lpContext)	// UpdateReelmapYieldIts()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerReelmap* pThread = reinterpret_cast<CManagerReelmap*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[14] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[14].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[14] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pView->m_mgrProcedure)
		{
			if (pView->m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_ITS) // Yield Reelmap
			{
				pThread->UpdateReelmapYieldIts(); // Yield Reelmap
				pView->m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_ITS = FALSE;
				Sleep(0);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[14] = FALSE;

	return 0;
}

UINT CManagerReelmap::ThreadProc15(LPVOID lpContext)	// ReloadReelmapUpInner()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerReelmap* pThread = reinterpret_cast<CManagerReelmap*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[15] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[15].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[15] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pView->m_mgrProcedure)
		{
			if (pView->m_mgrProcedure->m_bTHREAD_RELOAD_RST_UP_INNER)
			{
				pThread->ReloadReelmapUpInner();
				pView->m_mgrProcedure->m_bTHREAD_RELOAD_RST_UP_INNER = FALSE;
				Sleep(0);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[15] = FALSE;

	return 0;
}

UINT CManagerReelmap::ThreadProc16(LPVOID lpContext)	// ReloadReelmapDnInner()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerReelmap* pThread = reinterpret_cast<CManagerReelmap*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[16] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[16].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[16] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pView->m_mgrProcedure)
		{
			if (pView->m_mgrProcedure->m_bTHREAD_RELOAD_RST_DN_INNER)
			{
				pThread->ReloadReelmapDnInner();
				pView->m_mgrProcedure->m_bTHREAD_RELOAD_RST_DN_INNER = FALSE;
				Sleep(0);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[16] = FALSE;

	return 0;
}

UINT CManagerReelmap::ThreadProc17(LPVOID lpContext)	// ReloadReelmapAllUpInner()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerReelmap* pThread = reinterpret_cast<CManagerReelmap*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[17] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[17].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[17] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pView->m_mgrProcedure)
		{
			if (pView->m_mgrProcedure->m_bTHREAD_RELOAD_RST_ALLUP_INNER)
			{
				pThread->ReloadReelmapAllUpInner();
				pView->m_mgrProcedure->m_bTHREAD_RELOAD_RST_ALLUP_INNER = FALSE;
				Sleep(0);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[17] = FALSE;

	return 0;
}

UINT CManagerReelmap::ThreadProc18(LPVOID lpContext)	// ReloadReelmapAllDnInner()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerReelmap* pThread = reinterpret_cast<CManagerReelmap*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[18] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[18].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[18] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pView->m_mgrProcedure)
		{
			if (pView->m_mgrProcedure->m_bTHREAD_RELOAD_RST_ALLDN_INNER)
			{
				pThread->ReloadReelmapAllDnInner();
				pView->m_mgrProcedure->m_bTHREAD_RELOAD_RST_ALLDN_INNER = FALSE;
				Sleep(0);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[18] = FALSE;

	return 0;
}

UINT CManagerReelmap::ThreadProc19(LPVOID lpContext)	// ReloadReelmapIts()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerReelmap* pThread = reinterpret_cast<CManagerReelmap*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[19] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[19].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[19] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pView->m_mgrProcedure)
		{
			if (pView->m_mgrProcedure->m_bTHREAD_RELOAD_RST_ITS)
			{
				pThread->ReloadReelmapIts();
				pView->m_mgrProcedure->m_bTHREAD_RELOAD_RST_ITS = FALSE;
				Sleep(0);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[19] = FALSE;

	return 0;
}

UINT CManagerReelmap::ThreadProc20(LPVOID lpContext)	// UpdateRMapInnerUp()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerReelmap* pThread = reinterpret_cast<CManagerReelmap*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[20] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[20].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[20] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pView->m_mgrProcedure)
		{
			if (pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_INNER_UP) // Write Reelmap
			{
				pThread->UpdateRMapInnerUp();
				pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_INNER_UP = FALSE;
				Sleep(0);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[20] = FALSE;

	return 0;
}

UINT CManagerReelmap::ThreadProc21(LPVOID lpContext)	// UpdateRMapInnerDn()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerReelmap* pThread = reinterpret_cast<CManagerReelmap*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[21] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[21].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[21] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pView->m_mgrProcedure)
		{
			if (pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_INNER_DN) // Write Reelmap
			{
				//pThread->m_bTHREAD_UPDATE_REELMAP_ALLUP = TRUE;
				//pThread->m_bTHREAD_UPDATE_REELMAP_ALLDN = TRUE;
				pThread->UpdateRMapInnerDn();
				pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_INNER_DN = FALSE;
				Sleep(0);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[21] = FALSE;

	return 0;
}

UINT CManagerReelmap::ThreadProc22(LPVOID lpContext)	// UpdateRMapInnerAllUp()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerReelmap* pThread = reinterpret_cast<CManagerReelmap*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[22] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[22].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[22] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pView->m_mgrProcedure)
		{
			if (pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_INNER_ALLUP) // Write Reelmap
			{
				pThread->UpdateRMapInnerAllUp();
				pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_INNER_ALLUP = FALSE;
				Sleep(0);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[22] = FALSE;

	return 0;
}

UINT CManagerReelmap::ThreadProc23(LPVOID lpContext)	// UpdateRMapInnerAllDn()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerReelmap* pThread = reinterpret_cast<CManagerReelmap*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[23] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[23].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[23] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pView->m_mgrProcedure)
		{
			if (pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_INNER_ALLDN) // Write Reelmap
			{
				pThread->UpdateRMapInnerAllDn();
				pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_INNER_ALLDN = FALSE;
				Sleep(0);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[23] = FALSE;

	return 0;
}

UINT CManagerReelmap::ThreadProc24(LPVOID lpContext)	// UpdateYieldUp()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerReelmap* pThread = reinterpret_cast<CManagerReelmap*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[24] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[24].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[24] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pView->m_mgrProcedure)
		{
			if (pView->m_mgrProcedure->m_bTHREAD_UPDATE_YIELD_UP)
			{
				pThread->UpdateYieldUp(pView->m_mgrProcedure->m_nSnTHREAD_UPDATAE_YIELD);
				pView->m_mgrProcedure->m_bTHREAD_UPDATE_YIELD_UP = FALSE;
				Sleep(0);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[24] = FALSE;

	return 0;
}

UINT CManagerReelmap::ThreadProc25(LPVOID lpContext)	// UpdateYieldDn()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerReelmap* pThread = reinterpret_cast<CManagerReelmap*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[25] = TRUE; 
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[25].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[25] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pView->m_mgrProcedure)
		{
			if (pView->m_mgrProcedure->m_bTHREAD_UPDATE_YIELD_DN)
			{
				pThread->UpdateYieldDn(pView->m_mgrProcedure->m_nSnTHREAD_UPDATAE_YIELD);
				pView->m_mgrProcedure->m_bTHREAD_UPDATE_YIELD_DN = FALSE;
				pView->m_mgrProcedure->m_bTHREAD_UPDATE_YIELD_ALLUP = TRUE;
				pView->m_mgrProcedure->m_bTHREAD_UPDATE_YIELD_ALLDN = TRUE;
				Sleep(0);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[25] = FALSE;

	return 0;
}

UINT CManagerReelmap::ThreadProc26(LPVOID lpContext)	// UpdateYieldAllUp()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerReelmap* pThread = reinterpret_cast<CManagerReelmap*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[26] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[26].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[26] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pView->m_mgrProcedure)
		{
			if (pView->m_mgrProcedure->m_bTHREAD_UPDATE_YIELD_ALLUP)
			{
				pThread->UpdateYieldAllUp(pView->m_mgrProcedure->m_nSnTHREAD_UPDATAE_YIELD);
				pView->m_mgrProcedure->m_bTHREAD_UPDATE_YIELD_ALLUP = FALSE;

				if (pDoc->GetTestMode() == MODE_OUTER)
				{
					pView->m_mgrProcedure->m_bTHREAD_UPDATE_YIELD_ITS = TRUE;
					//pThread->m_bTHREAD_UPDATE_YIELD_INNER_UP = TRUE;
					//if (pDoc->WorkingInfo.LastJob.bDualTestInner)
					//{
					//	pThread->m_bTHREAD_UPDATE_YIELD_INNER_DN = TRUE;
					//}
				}

				Sleep(0);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[26] = FALSE;

	return 0;
}

UINT CManagerReelmap::ThreadProc27(LPVOID lpContext)	// UpdateYieldAllDn()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerReelmap* pThread = reinterpret_cast<CManagerReelmap*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[27] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[27].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[27] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pView->m_mgrProcedure)
		{
			if (pView->m_mgrProcedure->m_bTHREAD_UPDATE_YIELD_ALLDN)
			{
				pThread->UpdateYieldAllDn(pView->m_mgrProcedure->m_nSnTHREAD_UPDATAE_YIELD);
				pView->m_mgrProcedure->m_bTHREAD_UPDATE_YIELD_ALLDN = FALSE;
				Sleep(0);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[27] = FALSE;

	return 0;
}

UINT CManagerReelmap::ThreadProc28(LPVOID lpContext)	// UpdateYieldInnerUp()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerReelmap* pThread = reinterpret_cast<CManagerReelmap*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[28] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[28].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[28] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pView->m_mgrProcedure)
		{
			if (pView->m_mgrProcedure->m_bTHREAD_UPDATE_YIELD_INNER_UP)
			{
				pThread->UpdateYieldInnerUp(pView->m_mgrProcedure->m_nSnTHREAD_UPDATAE_YIELD);
				pView->m_mgrProcedure->m_bTHREAD_UPDATE_YIELD_INNER_UP = FALSE;
				Sleep(0);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[28] = FALSE;

	return 0;
}

UINT CManagerReelmap::ThreadProc29(LPVOID lpContext)	// UpdateYieldInnerDn()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerReelmap* pThread = reinterpret_cast<CManagerReelmap*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[29] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[29].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[29] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pView->m_mgrProcedure)
		{
			if (pView->m_mgrProcedure->m_bTHREAD_UPDATE_YIELD_INNER_DN)
			{
				pThread->UpdateYieldInnerDn(pView->m_mgrProcedure->m_nSnTHREAD_UPDATAE_YIELD);
				pView->m_mgrProcedure->m_bTHREAD_UPDATE_YIELD_INNER_DN = FALSE;
				pView->m_mgrProcedure->m_bTHREAD_UPDATE_YIELD_INNER_ALLUP = TRUE;
				pView->m_mgrProcedure->m_bTHREAD_UPDATE_YIELD_INNER_ALLDN = TRUE;
				Sleep(0);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[29] = FALSE;

	return 0;
}

UINT CManagerReelmap::ThreadProc30(LPVOID lpContext)	// UpdateYieldInnerAllUp()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerReelmap* pThread = reinterpret_cast<CManagerReelmap*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[30] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[30].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[30] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pView->m_mgrProcedure)
		{
			if (pView->m_mgrProcedure->m_bTHREAD_UPDATE_YIELD_INNER_ALLUP)
			{
				pThread->UpdateYieldInnerAllUp(pView->m_mgrProcedure->m_nSnTHREAD_UPDATAE_YIELD);
				pView->m_mgrProcedure->m_bTHREAD_UPDATE_YIELD_INNER_ALLUP = FALSE;
				Sleep(0);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[30] = FALSE;

	return 0;
}

UINT CManagerReelmap::ThreadProc31(LPVOID lpContext)	// UpdateYieldInnerAllDn()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerReelmap* pThread = reinterpret_cast<CManagerReelmap*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[31] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[31].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[31] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pView->m_mgrProcedure)
		{
			if (pView->m_mgrProcedure->m_bTHREAD_UPDATE_YIELD_INNER_ALLDN)
			{
				pThread->UpdateYieldInnerAllDn(pView->m_mgrProcedure->m_nSnTHREAD_UPDATAE_YIELD);
				pView->m_mgrProcedure->m_bTHREAD_UPDATE_YIELD_INNER_ALLDN = FALSE;
				Sleep(0);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[31] = FALSE;

	return 0;
} 

UINT CManagerReelmap::ThreadProc32(LPVOID lpContext)	// UpdateYieldIts()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerReelmap* pThread = reinterpret_cast<CManagerReelmap*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[32] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[32].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[32] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pView->m_mgrProcedure)
		{
			if (pView->m_mgrProcedure->m_bTHREAD_UPDATE_YIELD_ITS)
			{
				pThread->UpdateYieldIts(pView->m_mgrProcedure->m_nSnTHREAD_UPDATAE_YIELD);
				pView->m_mgrProcedure->m_bTHREAD_UPDATE_YIELD_ITS = FALSE;
				Sleep(0);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[32] = FALSE;

	return 0;
}

void CManagerReelmap::UpdateRMapUp()
{
	if (m_pReelMapUp)
	{
		m_pReelMapUp->Write(m_nSerialRmapUpdate); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		//m_pReelMapUp->Write(m_nSerialRmapUpdate, 0, m_sPathRmapUpdate[0]); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn

		if (m_pReelMap == m_pReelMapUp)
		{
			DuplicateRmap(RMAP_UP);
		}
	}
}

void CManagerReelmap::UpdateRMapDn()
{
	if (m_pReelMapDn)
	{
		m_pReelMapDn->Write(m_nSerialRmapUpdate); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		//m_pReelMapDn->Write(m_nSerialRmapUpdate, 1, m_sPathRmapUpdate[1]); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
	}
}

void CManagerReelmap::UpdateRMapAllUp()
{
	if (m_pReelMapAllUp)
	{
		m_pReelMapAllUp->Write(m_nSerialRmapUpdate); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		//m_pReelMapAllUp->Write(m_nSerialRmapUpdate, 2, m_sPathRmapUpdate[2]); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn

		if (m_pReelMap == m_pReelMapAllUp)
		{
			DuplicateRmap(RMAP_ALLUP);
		}
	}
}

void CManagerReelmap::UpdateRMapAllDn()
{
	if (m_pReelMapAllDn)
	{
		m_pReelMapAllDn->Write(m_nSerialRmapUpdate); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		//m_pReelMapAllDn->Write(m_nSerialRmapUpdate, 3, m_sPathRmapUpdate[3]); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
	}
}

BOOL CManagerReelmap::WriteReelmapIts()
{
	//return pDoc->WriteReelmapIts(m_nSerialRmapUpdate);
	BOOL bRtn = m_pReelMapIts->WriteIts(m_nSerialRmapUpdate);

	if (m_pReelMap == m_pReelMapIts)
	{
		DuplicateRmap(RMAP_ITS);
	}

	return bRtn;
}

void CManagerReelmap::UpdateReelmapYieldUp()
{
	if (m_pReelMapUp)
		m_pReelMapUp->UpdateReelmapYield();					// 릴맵 텍스트 파일의 수율정보를 업데이트함.
}

void CManagerReelmap::UpdateReelmapYieldAllUp()
{
	if (m_pReelMapAllUp)
		m_pReelMapAllUp->UpdateReelmapYield();					// 릴맵 텍스트 파일의 수율정보를 업데이트함.
}

void CManagerReelmap::UpdateReelmapYieldDn()
{
	if (m_pReelMapDn)
		m_pReelMapDn->UpdateReelmapYield();					// 릴맵 텍스트 파일의 수율정보를 업데이트함.
}

void CManagerReelmap::UpdateReelmapYieldAllDn()
{
	if (m_pReelMapAllDn)
		m_pReelMapAllDn->UpdateReelmapYield();					// 릴맵 텍스트 파일의 수율정보를 업데이트함.
}

void CManagerReelmap::UpdateReelmapYieldIts()
{
	if (m_pReelMapIts)
		m_pReelMapIts->UpdateReelmapYield();					// 릴맵 텍스트 파일의 수율정보를 업데이트함.
}

void CManagerReelmap::UpdateRMapInnerUp()
{
	if (pDoc->GetTestMode() == MODE_INNER)
	{
		if (m_pReelMapUp)
			m_pReelMapUp->MakeItsFile(m_nSerialRmapInnerUpdate, RMAP_INNER_UP);
	}

	if (pDoc->GetTestMode() == MODE_OUTER)
	{
		if (m_pReelMapInnerUp)
			m_pReelMapInnerUp->Write(m_nSerialRmapInnerUpdate); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		if (m_pReelMapInnerUp)
			m_pReelMapInnerUp->MakeItsFile(m_nSerialRmapInnerUpdate, RMAP_UP);
	}
}

void CManagerReelmap::UpdateRMapInnerDn()
{
	if (pDoc->GetTestMode() == MODE_INNER)
	{
		if (m_pReelMapDn)
			m_pReelMapDn->MakeItsFile(m_nSerialRmapInnerUpdate, RMAP_INNER_DN);
	}

	if (pDoc->GetTestMode() == MODE_OUTER)
	{
		if (m_pReelMapInnerDn)
			m_pReelMapInnerDn->Write(m_nSerialRmapInnerUpdate); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		if (m_pReelMapInnerDn)
			m_pReelMapInnerDn->MakeItsFile(m_nSerialRmapInnerUpdate, RMAP_DN);
	}
}

void CManagerReelmap::UpdateRMapInnerAllUp()
{
	if (pDoc->GetTestMode() == MODE_OUTER)
	{
		if (m_pReelMapInnerAllUp)
			m_pReelMapInnerAllUp->Write(m_nSerialRmapInnerUpdate); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
	}
}

void CManagerReelmap::UpdateRMapInnerAllDn()
{
	if (pDoc->GetTestMode() == MODE_OUTER)
	{
		if (m_pReelMapInnerAllDn)
			m_pReelMapInnerAllDn->Write(m_nSerialRmapInnerUpdate); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
	}
}

void CManagerReelmap::UpdateYieldOnThread(int nSerial)
{
	if (pView && pView->m_mgrProcedure)
		pView->m_mgrProcedure->UpdateYieldOnThread(nSerial);
}

void CManagerReelmap::UpdateYield(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.66"));
		return;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (m_pReelMapUp)
		m_pReelMapUp->UpdateYield(nSerial);
	if (bDualTest)
	{
		if (m_pReelMapDn)
			m_pReelMapDn->UpdateYield(nSerial);
		if (m_pReelMapAllUp)
			m_pReelMapAllUp->UpdateYield(nSerial);
		if (m_pReelMapAllDn)
			m_pReelMapAllDn->UpdateYield(nSerial);
	}

	if (pDoc->GetTestMode() == MODE_OUTER)
	{
		if (m_pReelMapInnerUp)
			m_pReelMapInnerUp->UpdateYield(nSerial);

		if (m_pReelMapIts)
			m_pReelMapIts->UpdateYield(nSerial);

		if (pDoc->WorkingInfo.LastJob.bDualTestInner)
		{
			if (m_pReelMapInnerDn)
				m_pReelMapInnerDn->UpdateYield(nSerial);
			if (m_pReelMapInnerAllUp)
				m_pReelMapInnerAllUp->UpdateYield(nSerial);
			if (m_pReelMapInnerAllDn)
				m_pReelMapInnerAllDn->UpdateYield(nSerial);
		}
	}

}

void CManagerReelmap::UpdateYieldUp(int nSerial)
{
	if (m_pReelMapUp)
		m_pReelMapUp->UpdateYield(nSerial);
}

void CManagerReelmap::UpdateYieldDn(int nSerial)
{
	if (m_pReelMapDn)
		m_pReelMapDn->UpdateYield(nSerial);
}

void CManagerReelmap::UpdateYieldAllUp(int nSerial)
{
	if (m_pReelMapAllUp)
		m_pReelMapAllUp->UpdateYield(nSerial);
}

void CManagerReelmap::UpdateYieldAllDn(int nSerial)
{
	if (m_pReelMapAllDn)
		m_pReelMapAllDn->UpdateYield(nSerial);
}

void CManagerReelmap::UpdateYieldInnerUp(int nSerial)
{
	if (m_pReelMapInnerUp)
		m_pReelMapInnerUp->UpdateYield(nSerial);
}

void CManagerReelmap::UpdateYieldInnerDn(int nSerial)
{
	if (m_pReelMapInnerDn)
		m_pReelMapInnerDn->UpdateYield(nSerial);
}

void CManagerReelmap::UpdateYieldInnerAllUp(int nSerial)
{
	if (m_pReelMapInnerAllUp)
		m_pReelMapInnerAllUp->UpdateYield(nSerial);
}

void CManagerReelmap::UpdateYieldInnerAllDn(int nSerial)
{
	if (m_pReelMapInnerAllDn)
		m_pReelMapInnerAllDn->UpdateYield(nSerial);
}

void CManagerReelmap::UpdateYieldIts(int nSerial)
{
	if (m_pReelMapIts)
		m_pReelMapIts->UpdateYield(nSerial);
}

BOOL CManagerReelmap::RemakeReelmap()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	CString sReelmapSrc, str;

	if (m_pReelMapUp)
		m_pReelMapUp->StartThreadRemakeReelmap();

	if (bDualTest)
	{
		if (m_pReelMapDn)
			m_pReelMapDn->StartThreadRemakeReelmap();

		if (m_pReelMapAllUp)
			m_pReelMapAllUp->StartThreadRemakeReelmap();
	}

	return TRUE;
}

BOOL CManagerReelmap::IsDoneRemakeReelmap()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	BOOL bSuccess[3] = { FALSE };
	DWORD dwSt = GetTickCount();

	Sleep(100);

	do
	{
		if (GetTickCount() - dwSt > 600000)
			break;
	} while (!m_pReelMapUp->m_bThreadAliveRemakeReelmap && !m_pReelMapDn->m_bThreadAliveRemakeReelmap && !m_pReelMapAllUp->m_bThreadAliveRemakeReelmap);

	if (bDualTest)
	{
		bSuccess[0] = m_pReelMapUp->m_bRtnThreadRemakeReelmap;
		bSuccess[1] = m_pReelMapDn->m_bRtnThreadRemakeReelmap;
		bSuccess[2] = m_pReelMapAllUp->m_bRtnThreadRemakeReelmap;

		if (!bSuccess[0] || !bSuccess[2] || !bSuccess[1])
		{
			pView->MsgBox(_T("ReelMap Converting Failed."));
			return FALSE;
		}
	}
	else
	{
		if (!m_pReelMapUp->m_bRtnThreadRemakeReelmap)
		{
			pView->MsgBox(_T("ReelMap Converting Failed."));
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CManagerReelmap::RemakeReelmapInner()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	CString sReelmapSrc, str;

	//if (m_pReelMapInnerUp)
	//	m_pReelMapInnerUp->StartThreadRemakeReelmap();

	if (m_pReelMapIts)
		m_pReelMapIts->StartThreadRemakeReelmap();

	//if (pDoc->WorkingInfo.LastJob.bDualTestInner)
	//{
	//	if (m_pReelMapInnerDn)
	//		m_pReelMapInnerDn->StartThreadRemakeReelmap();

	//	if (m_pReelMapInnerAllUp)
	//		m_pReelMapInnerAllUp->StartThreadRemakeReelmap();
	//}

	return TRUE;
}

BOOL CManagerReelmap::IsDoneRemakeReelmapInner()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;
	BOOL bAlive[4] = { FALSE };
	BOOL bSuccess[4] = { TRUE };
	BOOL bWait = TRUE;
	DWORD dwSt = GetTickCount();

	Sleep(100);

	do
	{
		if (GetTickCount() - dwSt > 600000)
			break;

		bAlive[0] = m_pReelMapInnerUp->m_bThreadAliveRemakeReelmap;
		bAlive[1] = m_pReelMapInnerDn->m_bThreadAliveRemakeReelmap;
		bAlive[2] = m_pReelMapInnerAllUp->m_bThreadAliveRemakeReelmap;
		bAlive[3] = m_pReelMapIts->m_bThreadAliveRemakeReelmap;

		if (bAlive[0] || bAlive[1] || bAlive[2] || bAlive[3])
			bWait = TRUE;
		else
			bWait = FALSE;

	} while (bWait);

	if (bDualTest)
	{
		bSuccess[0] = m_pReelMapInnerUp->m_bRtnThreadRemakeReelmap;
		bSuccess[1] = m_pReelMapInnerDn->m_bRtnThreadRemakeReelmap;
		bSuccess[2] = m_pReelMapInnerAllUp->m_bRtnThreadRemakeReelmap;
		bSuccess[3] = m_pReelMapIts->m_bRtnThreadRemakeReelmap;

		if (!bSuccess[0] || !bSuccess[1] || !bSuccess[2] || !bSuccess[3])
		{
			pView->MsgBox(_T("ReelMap Converting Failed."));
			return FALSE;
		}
	}
	else
	{
		bSuccess[0] = m_pReelMapInnerUp->m_bRtnThreadRemakeReelmap;
		bSuccess[1] = m_pReelMapIts->m_bRtnThreadRemakeReelmap;

		if (!bSuccess[0] || !bSuccess[1])
		{
			pView->MsgBox(_T("ReelMap Converting Failed."));
			return FALSE;
		}
	}

	return TRUE;
}

int CManagerReelmap::CopyPcrAll()  // return : Serial
{
	if (!pView || !pView->m_mgrProcedure)
		return FALSE;

	int nS0, nS1;
	if (pView->m_mgrProcedure->m_bLoadShare[0])
	{
		nS0 = CopyPcrUp();
	}
	if (pView->m_mgrProcedure->m_bLoadShare[1])
	{
		nS1 = CopyPcrDn();
	}

	if (pView->m_mgrProcedure->m_bLoadShare[0] && pView->m_mgrProcedure->m_bLoadShare[1])
	{
		if (nS0 > 0 && nS1 > 0)
			return TRUE;
	}
	else if (pView->m_mgrProcedure->m_bLoadShare[0] && !pView->m_mgrProcedure->m_bLoadShare[1])
	{
		if (nS0 > 0)
			return TRUE;
	}
	else if (!pView->m_mgrProcedure->m_bLoadShare[0] && pView->m_mgrProcedure->m_bLoadShare[1])
	{
		if (nS1 > 0)
			return TRUE;
	}

	return FALSE;
}

int CManagerReelmap::CopyPcrUp()  // return : Serial
{
	int nSerial;
	CString sSrc = pDoc->WorkingInfo.System.sPathVrsShareUp;
	CString sDest = pDoc->WorkingInfo.System.sPathVrsBufUp;

	if (m_pFile)
		nSerial = m_pFile->CopyPcrAll(sSrc, sDest);

	return nSerial;
}

int CManagerReelmap::CopyPcrDn()  // return : Serial
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return 0;

	int nSerial;
	CString sSrc = pDoc->WorkingInfo.System.sPathVrsShareDn;
	CString sDest = pDoc->WorkingInfo.System.sPathVrsBufDn;

	if (m_pFile)
		nSerial = m_pFile->CopyPcrAll(sSrc, sDest);

	return nSerial;
}

void CManagerReelmap::DelSharePcr()
{
	if (!pView || !pView->m_mgrProcedure)
		return;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (bDualTest)
	{
		if (pView->m_mgrProcedure->m_bLoadShare[0])
			DelSharePcrUp();
		if (pView->m_mgrProcedure->m_bLoadShare[1])
			DelSharePcrDn();
	}
	else
	{
		if (pView->m_mgrProcedure->m_bLoadShare[0])
			DelSharePcrUp();
		if (pView->IsShareDn())
			DelSharePcrDn();
	}
}

void CManagerReelmap::DelSharePcrUp()
{
	CString sPath = pDoc->WorkingInfo.System.sPathVrsShareUp;

	if (m_pFile)
	{
		while (m_pFile->IsPcrExist(sPath))
		{
			m_pFile->DelPcrAll(sPath);
			Sleep(30);
		}
	}
}

void CManagerReelmap::DelSharePcrDn()
{
	CString sPath = pDoc->WorkingInfo.System.sPathVrsShareDn;

	while (m_pFile->IsPcrExist(sPath))
	{
		if (m_pFile)
			m_pFile->DelPcrAll(sPath);
		Sleep(30);
	}
}

void CManagerReelmap::DelPcrAll()
{
	DelSharePcrUp();
	DelSharePcrDn();

	DelPcrUp();
	DelPcrDn();

	if (pView && pView->m_mgrProcedure)
	{
		pView->m_mgrProcedure->m_bIsBuf[0] = FALSE;
		pView->m_mgrProcedure->m_bIsBuf[1] = FALSE;
	}
}

void CManagerReelmap::DelPcrUp()
{
	CString sPath;

	if (m_pFile)
	{
		sPath = pDoc->WorkingInfo.System.sPathVrsShareUp;
		m_pFile->DelPcrAll(sPath);
		sPath = pDoc->WorkingInfo.System.sPathVrsBufUp;
		m_pFile->DelPcrAll(sPath);
	}
}

void CManagerReelmap::DelPcrDn()
{
	CString sPath;

	if (m_pFile)
	{
		sPath = pDoc->WorkingInfo.System.sPathVrsShareDn;
		m_pFile->DelPcrAll(sPath);
		sPath = pDoc->WorkingInfo.System.sPathVrsBufDn;
		m_pFile->DelPcrAll(sPath);
	}
}

void CManagerReelmap::SetLotSt()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (m_pReelMap)
		m_pReelMap->SetLotSt();
	if (m_pReelMapUp)
		m_pReelMapUp->SetLotSt();
	if (bDualTest)
	{
		if (m_pReelMapDn)
			m_pReelMapDn->SetLotSt();
		if (m_pReelMapAllUp)
			m_pReelMapAllUp->SetLotSt();
		if (m_pReelMapAllDn)
			m_pReelMapAllDn->SetLotSt();
	}
}

void CManagerReelmap::SetLotEd()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (m_pReelMap)
		m_pReelMap->SetLotEd();
	if (m_pReelMapUp)
		m_pReelMapUp->SetLotEd();
	if (bDualTest)
	{
		if (m_pReelMapDn)
			m_pReelMapDn->SetLotEd();
		if (m_pReelMapAllUp)
			m_pReelMapAllUp->SetLotEd();
		if (m_pReelMapAllDn)
			m_pReelMapAllDn->SetLotEd();
	}
}

void CManagerReelmap::ClrFixPcs()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (m_pReelMap)
		m_pReelMap->ClrFixPcs();
	if (m_pReelMapUp)
		m_pReelMapUp->ClrFixPcs();

	if (bDualTest)
	{
		if (m_pReelMapDn)
			m_pReelMapDn->ClrFixPcs();
		if (m_pReelMapAllUp)
			m_pReelMapAllUp->ClrFixPcs();
		if (m_pReelMapAllDn)
			m_pReelMapAllDn->ClrFixPcs();
	}

	if (pDoc->GetTestMode() == MODE_OUTER)
	{
		if (m_pReelMapInnerUp)
			m_pReelMapInnerUp->ClrFixPcs();

		if (pDoc->WorkingInfo.LastJob.bDualTestInner)
		{
			if (m_pReelMapInnerDn)
				m_pReelMapInnerDn->ClrFixPcs();
			if (m_pReelMapInnerAllUp)
				m_pReelMapInnerAllUp->ClrFixPcs();
			if (m_pReelMapInnerAllDn)
				m_pReelMapInnerAllDn->ClrFixPcs();
		}
	}

}

void CManagerReelmap::RestoreReelmap()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (m_pReelMapUp)
		m_pReelMapUp->RestoreReelmap();

	if (bDualTest)
	{
		if (m_pReelMapDn)
			m_pReelMapDn->RestoreReelmap();
		if (m_pReelMapAllUp)
			m_pReelMapAllUp->RestoreReelmap();
		if (m_pReelMapAllDn)
			m_pReelMapAllDn->RestoreReelmap();
	}
}

void CManagerReelmap::ClearReelmap()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	ClrPcr();

	if (m_pReelMap)
		m_pReelMap->Clear();
	if (m_pReelMapUp)
		m_pReelMapUp->Clear();
	if (bDualTest)
	{
		if (m_pReelMapDn)
			m_pReelMapDn->Clear();
		if (m_pReelMapAllUp)
			m_pReelMapAllUp->Clear();
		if (m_pReelMapAllDn)
			m_pReelMapAllDn->Clear();
	}
}

int CManagerReelmap::GetErrCodeUp(int nSerial) // 1(정상), -1(Align Error, 노광불량), -2(Lot End)
{
	int nIdx = GetPcrIdx0(nSerial);
	int nErrCode = 1;

#ifndef	TEST_MODE
	if (m_pPcr[0])
	{
		if (m_pPcr[0][nIdx])
			nErrCode = m_pPcr[0][nIdx]->m_nErrPnl;
		else
			return 2;	// Code Setting Error.
	}
	else
		return 2;	// Code Setting Error.
#endif

	return nErrCode;
}

int CManagerReelmap::GetErrCodeDn(int nSerial) // 1(정상), -1(Align Error, 노광불량), -2(Lot End)
{
	if (nSerial <= 0)
	{
		AfxMessageBox(_T("Serial Error.31"));
		return 0;
	}

	int nIdx = GetPcrIdx1(nSerial);
	int nErrCode = 1;

#ifndef	TEST_MODE
	if (m_pPcr[1])
	{
		if (m_pPcr[1][nIdx])
			nErrCode = m_pPcr[1][nIdx]->m_nErrPnl;
	}
#endif

	return nErrCode;
}

int CManagerReelmap::GetTotDefPcs(int nSerial)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	int nIdx = GetPcrIdx(nSerial);
	int nTotDef = 0;

#ifndef TEST_MODE
	if (bDualTest)
	{
		if (m_pPcr[2])
		{
			if (m_pPcr[2][nIdx])
				nTotDef = m_pPcr[2][nIdx]->m_nTotDef;
		}
	}
	else
	{
		if (m_pPcr[0])
		{
			if (m_pPcr[0][nIdx])
				nTotDef = m_pPcr[0][nIdx]->m_nTotDef;
		}
	}
#else
	nTotDef = 1;
#endif

	return nTotDef;
}

int CManagerReelmap::GetTotDefPcsUp(int nSerial)
{
	int nIdx = GetPcrIdx(nSerial);
	int nTotDef = 0;

#ifndef TEST_MODE
	if (m_pPcr[0])
	{
		if (m_pPcr[0][nIdx])
			nTotDef = m_pPcr[0][nIdx]->m_nTotDef;
	}
#else
	nTotDef = 1;
#endif

	return nTotDef;
}

int CManagerReelmap::GetTotDefPcsDn(int nSerial)
{
		int nIdx = GetPcrIdx(nSerial);
		int nTotDef = 0;
	
	#ifndef TEST_MODE
		if (m_pPcr[1])
		{
			if (m_pPcr[1][nIdx])
				nTotDef = m_pPcr[1][nIdx]->m_nTotDef;
		}
	#else
		nTotDef = 1;
	#endif
	
		return nTotDef;
}

int CManagerReelmap::MirrorLR(int nPcsId) // 좌우 미러링
{
#ifdef TEST_MODE
	return 0;
#endif

	int nId, nCol, nRow, nC, nR;
	int nNodeX = m_Master[0].m_pPcsRgn->nCol; // 1 ~
	int nNodeY = m_Master[0].m_pPcsRgn->nRow; // 1 ~

	nCol = int(nPcsId / nNodeY); // 0 ~
	if (nCol % 2)
		nRow = nNodeY*(nCol + 1) - nPcsId - 1;
	else
		nRow = nPcsId - nNodeY*nCol; // 0 ~

	nR = nRow; // 0 ~
	nC = (nNodeX - 1) - nCol; // 0 ~
	if (nC % 2) // 홀수 : 시작 감소
		nId = nNodeY*(nC + 1) - nR - 1;
	else		// 짝수 : 시작 증가
		nId = nR + nNodeY*nC;

	return nId;
}

int CManagerReelmap::MirrorUD(int nPcsId) // 상하 미러링
{
#ifdef TEST_MODE
	return 0;
#endif

	int nId, nCol, nRow, nC, nR;
	int nNodeX = m_Master[0].m_pPcsRgn->nCol; // 1 ~
	int nNodeY = m_Master[0].m_pPcsRgn->nRow; // 1 ~

	nCol = int(nPcsId / nNodeY); // 0 ~
	if (nCol % 2)
		nRow = nPcsId - nNodeY*nCol; // 0 ~
	else
		nRow = nNodeY*(nCol + 1) - nPcsId - 1;

	nR = nRow; // 0 ~
	nC = nCol; // 0 ~
	if (nC % 2) // 홀수 : 시작 감소
		nId = nNodeY*(nC + 1) - nR - 1;
	else		// 짝수 : 시작 증가
		nId = nR + nNodeY*nC;

	return nId;
}

int CManagerReelmap::Rotate180(int nPcsId) // 180도 회전
{
#ifdef TEST_MODE
	return 0;
#endif

	int nId, nCol, nRow, nC, nR;
	int nNodeX = m_Master[0].m_pPcsRgn->nCol; // 1 ~
	int nNodeY = m_Master[0].m_pPcsRgn->nRow; // 1 ~

	if (nNodeX % 2)		// 홀수 : 시작 감소
	{
		nCol = (nNodeX - 1) - int(nPcsId / nNodeY); // 0 ~
		if (nCol % 2)
			nRow = nPcsId - nNodeY * (nNodeX - nCol - 1); // 0 ~
		else
			nRow = nNodeY * (nNodeX - nCol) - nPcsId - 1; // 0 ~

														  //nR = (nNodeY - 1) - nRow; // 0 ~
														  //nC = (nNodeX - 1) - nCol; // 0 ~
		nR = nRow;
		nC = nCol;

		if (nC % 2)
			nId = nNodeY*(nC + 1) - nR - 1; // 0 ~
		else
			nId = nR + nNodeY*nC;
	}
	else				// 짝수 : 시작 증가
	{
		nCol = (nNodeX - 1) - int(nPcsId / nNodeY); // 0 ~
		if (nCol % 2)
			nRow = nNodeY * (nNodeX - nCol) - nPcsId - 1; // 0 ~
		else
			nRow = nPcsId - nNodeY * (nNodeX - nCol - 1); // 0 ~

														  //nR = (nNodeY - 1) - nRow; // 0 ~
														  //nC = (nNodeX - 1) - nCol; // 0 ~
		nR = nRow;
		nC = nCol;

		if (nC % 2)
			nId = nNodeY*(nC + 1) - nR - 1; // 0 ~
		else
			nId = nR + nNodeY*nC;
	}
	return nId;
}

CString CManagerReelmap::GetMkInfo(int nSerial, int nMkPcs) // return Cam0 : "Serial_Strip_Col_Row"
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	CString sInfo;
	int nStrip = -1, nCol = -1, nRow = -1;
	int nPcsIdx, nDefCode;
	int nPcrIdx = GetPcrIdx(nSerial);
	if (bDualTest)
	{
		nPcsIdx = m_pPcr[2][nPcrIdx]->m_pDefPcs[nMkPcs];
		nDefCode = m_pPcr[2][nPcrIdx]->m_pDefType[nMkPcs];
	}
	else
	{
		nPcsIdx = m_pPcr[0][nPcrIdx]->m_pDefPcs[nMkPcs];
		nDefCode = m_pPcr[0][nPcrIdx]->m_pDefType[nMkPcs];
	}

	if (m_Master[0].m_pPcsRgn)
		m_Master[0].m_pPcsRgn->GetMkMatrix(nPcsIdx, nStrip, nCol, nRow);

	sInfo.Format(_T("%04d_%c_%d_%d"), nSerial, nStrip + 'A', nCol + 1, nRow + 1);

	return sInfo;
}

void CManagerReelmap::DuplicateRmap()
{
	if (m_pReelMap == m_pReelMapUp)
	{
		DuplicateRmap(RMAP_UP);
	}
	else if (m_pReelMap == m_pReelMapAllUp)
	{
		DuplicateRmap(RMAP_ALLUP);
	}
	else if (m_pReelMap == m_pReelMapIts)
	{
		DuplicateRmap(RMAP_ITS);
	}
}

void CManagerReelmap::DuplicateRmap(int nRmap)
{
	CFileFind finder;
	CString strTemp, sSrcPath, sDstPath;
	sDstPath = _T("C:\\PunchWork\\Reelmap.txt");

	switch (nRmap)
	{
	case RMAP_UP:
		sSrcPath = m_pReelMapUp->GetRmapPath(RMAP_UP);
		break;
	case RMAP_ALLUP:
		sSrcPath = m_pReelMapAllUp->GetRmapPath(RMAP_ALLUP);
		break;
	case RMAP_ITS:
		sSrcPath = m_pReelMapUp->GetRmapPath(RMAP_ITS);
		break;
	}

	if (finder.FindFile(sSrcPath))
	{
		if (!CopyFile((LPCTSTR)sSrcPath, (LPCTSTR)sDstPath, FALSE))
		{
			Sleep(100);
			if (!CopyFile((LPCTSTR)sSrcPath, (LPCTSTR)sDstPath, FALSE))
			{
				Sleep(300);
				if (!CopyFile((LPCTSTR)sSrcPath, (LPCTSTR)sDstPath, FALSE))
				{
					Sleep(500);
					//strTemp.Format(_T("%s \r\n: Reelmap File Copy Fail"), sSrcPath);
					//pView->MsgBox(strTemp);
					if (!CopyFile((LPCTSTR)sSrcPath, (LPCTSTR)sDstPath, FALSE))
					{
						return;
					}
				}
			}
		}
	}
	else
	{
		Sleep(30);
		strTemp.Format(_T("%s \r\n: Reelmap File Not Exist"), sSrcPath);
		//AfxMessageBox(strTemp);
		pView->MsgBox(strTemp);
		return;
	}

}

void CManagerReelmap::SetMkPnt(int nCam)	// Pin위치에 의한 정렬.
{
	if (m_Master[0].m_pPcsRgn)
		m_Master[0].m_pPcsRgn->SetMkPnt(nCam);
}

void CManagerReelmap::ClrPcr()
{
	int nIdx, i, k;
	for (i = 0; i < MAX_PCR; i++)
	{
		for (nIdx = 0; nIdx < MAX_PCR_PNL; nIdx++)
		{
			if (m_pPcr[i][0])
			{
				m_pPcr[i][nIdx]->m_nIdx = 0;							// m_nIdx : From 0 to nTot.....
				m_pPcr[i][nIdx]->m_nSerial = 0;
				m_pPcr[i][nIdx]->m_nErrPnl = 0;
				m_pPcr[i][nIdx]->m_sModel = _T("");
				m_pPcr[i][nIdx]->m_sLayer = _T("");
				m_pPcr[i][nIdx]->m_sLot = _T("");

				m_pPcr[i][nIdx]->m_nCamId = 0;
				m_pPcr[i][nIdx]->m_nTotDef = 0;
				m_pPcr[i][nIdx]->m_nTotRealDef = 0;
			}

			if (m_pPcrInner[i][0])
			{
				m_pPcrInner[i][nIdx]->m_nIdx = 0;							// m_nIdx : From 0 to nTot.....
				m_pPcrInner[i][nIdx]->m_nSerial = 0;
				m_pPcrInner[i][nIdx]->m_nErrPnl = 0;
				m_pPcrInner[i][nIdx]->m_sModel = _T("");
				m_pPcrInner[i][nIdx]->m_sLayer = _T("");
				m_pPcrInner[i][nIdx]->m_sLot = _T("");

				m_pPcrInner[i][nIdx]->m_nCamId = 0;
				m_pPcrInner[i][nIdx]->m_nTotDef = 0;
				m_pPcrInner[i][nIdx]->m_nTotRealDef = 0;
			}
		}
	}

	for (k = 0; k < MAX_PCR_PNL; k++)
	{
		if (m_pPcrIts[k])
		{
			m_pPcrIts[k]->m_nIdx = 0;							// m_nIdx : From 0 to nTot.....
			m_pPcrIts[k]->m_nSerial = 0;
			m_pPcrIts[k]->m_nErrPnl = 0;
			m_pPcrIts[k]->m_sModel = _T("");
			m_pPcrIts[k]->m_sLayer = _T("");
			m_pPcrIts[k]->m_sLot = _T("");

			m_pPcrIts[k]->m_nCamId = 0;
			m_pPcrIts[k]->m_nTotDef = 0;
			m_pPcrIts[k]->m_nTotRealDef = 0;
		}
	}
}

BOOL CManagerReelmap::GetInnerFolderPath(int nItsSerial, CString  &sUp, CString &sDn)
{
	BOOL bDualTest;
	CString sLot, sLayerUp, sLayerDn;
	if (!GetItsSerialInfo(nItsSerial, bDualTest, sLot, sLayerUp, sLayerDn, 0))
	{
		CString str;
		str.Format(_T("It is trouble to read GetItsSerialInfo()."));
		pView->MsgBox(str);
		return FALSE;
	}

	CString  Path[5];
	CString sPath = _T("");

	Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
	Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
	Path[2] = sLot;
	Path[3] = sLayerUp;
	Path[4] = sLayerDn;

	if (Path[0].IsEmpty() || Path[1].IsEmpty() || Path[2].IsEmpty() || Path[3].IsEmpty())
		return FALSE;

	sUp.Format(_T("%s%s\\%s\\%s\\"), Path[0], Path[1], Path[2], Path[3]); // ITS Inner Dn Folder Path
	if (bDualTest)
	{
		if (Path[4].IsEmpty())
			return FALSE;

		sDn.Format(_T("%s%s\\%s\\%s\\"), Path[0], Path[1], Path[2], Path[4]); // ITS Inner Dn Folder Path
	}
	else
		sDn = _T("");

	return TRUE;
}

CString CManagerReelmap::GetItsFolderPath()
{
	CString  Path[3];
	CString sPath = _T("");

	Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
	Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
	//Path[1] = pDoc->m_sEngModel;
	Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
	//Path[2] = pDoc->m_sItsCode;

	if (Path[0].IsEmpty() || Path[1].IsEmpty() || Path[2].IsEmpty())
		return sPath;

	sPath.Format(_T("%s%s\\%s"), Path[0], Path[1], Path[2]); // ITS Folder Path

	return sPath;
}

CString CManagerReelmap::GetItsTargetFolderPath()
{
	CString sItsPath = pDoc->WorkingInfo.System.sPathIts;

	if (sItsPath.IsEmpty())
		return _T("");

	int pos = sItsPath.ReverseFind('\\');
	if (pos != -1)
		sItsPath.Delete(pos, sItsPath.GetLength() - pos);

	return sItsPath;
}

CString CManagerReelmap::GetItsReelmapPath()
{
	CString  Path[3];
	CString sPath = _T("");
	CString sName = _T("ReelMapDataIts.txt");

	Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
	Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
	//Path[1] = pDoc->m_sEngModel;
	Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
	//Path[2] = pDoc->m_sItsCode;

	if (Path[0].IsEmpty() || Path[1].IsEmpty() || Path[2].IsEmpty())
		return sPath;

	sPath.Format(_T("%s%s\\%s\\%s"), Path[0], Path[1], Path[2], sName); // ReelmapIts.txt

	return sPath;
}

BOOL CManagerReelmap::UpdateReelmap(int nSerial)
{
	if (!pView || !pView->m_mgrProcedure)
		return FALSE;

	return pView->m_mgrProcedure->UpdateReelmap(nSerial);
}

BOOL CManagerReelmap::UpdateReelmapInner(int nSerial)
{
	if (!pView || !pView->m_mgrProcedure)
		return FALSE;

	return pView->m_mgrProcedure->UpdateReelmapInner(nSerial);
}

void CManagerReelmap::SetFixPcs(int nSerial)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (m_pReelMapUp)
		m_pReelMapUp->SetFixPcs(nSerial);
	if (bDualTest)
	{
		if (m_pReelMapDn)
			m_pReelMapDn->SetFixPcs(nSerial);
	}
}

BOOL CManagerReelmap::GetPcrInfo(CString sPath, stModelInfo &stInfo)
{

	FILE *fp;
	char FileD[200];
	size_t nFileSize, nRSize;
	char *FileData;
	CString strFileData;
	int nTemp;// , i;
	CString strHeaderErrorInfo, strModel, strLayer, strLot, sItsCode, strTotalBadPieceNum;


	//strcpy(FileD, sPath);
	StringToChar(sPath, FileD);

	if ((fp = fopen(FileD, "r")) != NULL)
	{
		fseek(fp, 0, SEEK_END);
		nFileSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		/* Allocate space for a path name */
		//FileData = (char*)malloc( nFileSize );
		FileData = (char*)calloc(nFileSize + 1, sizeof(char));

		nRSize = fread(FileData, sizeof(char), nFileSize, fp);
		//strFileData.Format(_T("%s"), CharToString(FileData));
		strFileData = CharToString(FileData);
		fclose(fp);
		free(FileData);
	}
	else
	{
		strFileData.Format(_T("PCR 파일이 존재하지 않습니다.\r\n%s"), sPath);
		pView->MsgBox(strFileData);
		return(FALSE);
	}

	// Error Code											// 1(정상), -1(Align Error, 노광불량), -2(Lot End)
	nTemp = strFileData.Find(_T(','), 0);
	strHeaderErrorInfo = strFileData.Left(nTemp);
	strFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;
	//m_pPcr[nIdx]->m_nErrPnl = _tstoi(strHeaderErrorInfo);

	// Model
	nTemp = strFileData.Find(_T(','), 0);
	strModel = strFileData.Left(nTemp);
	strFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;
	//Status.PcrShare[1].sModel = strModel;

	// Layer
	nTemp = strFileData.Find(_T(','), 0);
	strLayer = strFileData.Left(nTemp);
	strFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;
	//Status.PcrShare[1].sLayer = strLayer;

	if (pDoc->WorkingInfo.System.bUseITS)
	{
		// Lot
		nTemp = strFileData.Find(_T(','), 0);
		strLot = strFileData.Left(nTemp);
		strFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;
		//Status.PcrShare[1].sLot = strLot;

		// Its Code
		nTemp = strFileData.Find(_T('\n'), 0);
		sItsCode = strFileData.Left(nTemp);
		strFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;
	}
	else
	{
		// Lot
		nTemp = strFileData.Find(_T(','), 0);
		strLot = strFileData.Left(nTemp);
		strFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;
		//Status.PcrShare[1].sLot = strLot;

		// Lot
		nTemp = strFileData.Find(_T('\n'), 0);
		strLot = strFileData.Left(nTemp);
		strFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;
		//Status.PcrShare[1].sLot = strLot;
	}

	nTemp = strFileData.Find(_T('\n'), 0);
	strTotalBadPieceNum = strFileData.Left(nTemp);
	strFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;

	int nTotDef = _tstoi(strTotalBadPieceNum);

	stInfo.sModel = strModel;
	stInfo.sLayer = strLayer;
	stInfo.sLot = strLot;
	stInfo.sItsCode = sItsCode;

	return TRUE;
}

BOOL CManagerReelmap::GetCurrentInfoEng()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	CString sPath = pDoc->WorkingInfo.System.sPathEngCurrInfo;
	TCHAR szData[512];

	if (sPath.IsEmpty() || (pDoc->GetTestMode() != MODE_INNER && pDoc->GetTestMode() != MODE_OUTER))
		return FALSE;

	if (0 < ::GetPrivateProfileString(_T("Infomation"), _T("Dual Test"), NULL, szData, sizeof(szData), sPath))
		m_bEngDualTest = _ttoi(szData) > 0 ? TRUE : FALSE;

	if (0 < ::GetPrivateProfileString(_T("Infomation"), _T("Its Code"), NULL, szData, sizeof(szData), sPath))
		m_sItsCode = CString(szData);

	if (0 < ::GetPrivateProfileString(_T("Infomation"), _T("Current Lot"), NULL, szData, sizeof(szData), sPath))
		m_sEngLotNum = CString(szData);
	//WorkingInfo.LastJob.sLotUp = CString(szData);

	if (0 < ::GetPrivateProfileString(_T("Infomation"), _T("Process Unit Code"), NULL, szData, sizeof(szData), sPath))
		m_sEngProcessNum = CString(szData);
	//WorkingInfo.LastJob.sProcessNum = CString(szData);

	if (0 < ::GetPrivateProfileString(_T("Infomation"), _T("Current Model Up"), NULL, szData, sizeof(szData), sPath))
		m_sEngModel = CString(szData);
	//WorkingInfo.LastJob.sModelUp = CString(szData);

	if (0 < ::GetPrivateProfileString(_T("Infomation"), _T("Current Layer Up"), NULL, szData, sizeof(szData), sPath))
		m_sEngLayerUp = CString(szData);
	//WorkingInfo.LastJob.sLayerUp = CString(szData);

	if (bDualTest)
	{
		pDoc->WorkingInfo.LastJob.sLotDn = pDoc->WorkingInfo.LastJob.sLotUp;

		//if (0 < ::GetPrivateProfileString(_T("Infomation"), _T("Current Model Dn"), NULL, szData, sizeof(szData), sPath))
		//WorkingInfo.LastJob.sModelDn = CString(szData);

		if (0 < ::GetPrivateProfileString(_T("Infomation"), _T("Current Layer Dn"), NULL, szData, sizeof(szData), sPath))
			m_sEngLayerDn = CString(szData);
		//WorkingInfo.LastJob.sLayerDn = CString(szData);
	}

	return TRUE;
}

// For MODE_OUTER ============================================

int CManagerReelmap::LoadPCRAllUpInner(int nSerial, BOOL bFromShare)	// return : 2(Failed), 1(정상), -1(Align Error, 노광불량), -2(Lot End)
{
#ifdef TEST_MODE
	return 0;
#endif
	BOOL bDualTest;
	CString sLot, sLayerUp, sLayerDn;
	if (!GetItsSerialInfo(nSerial, bDualTest, sLot, sLayerUp, sLayerDn, 1))
	{
		CString str;
		str.Format(_T("It is trouble to read GetItsSerialInfo()."));
		pView->MsgBox(str);
		return 0;
	}

	if (!bDualTest)
		return 1;

	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.16"));
		return 0;
	}

	int i, idx;//, k
	CString str;

	if (nSerial < 0)
	{
		str.Format(_T("PCR파일이 설정되지 않았습니다."));
		pView->MsgBox(str);
		//AfxMessageBox(strFileData);
		return(2);
	}

	if (!m_pPcrInner[2])
	{
		str.Format(_T("PCR[2]관련 메모리가 할당되지 않았습니다."));
		pView->MsgBox(str);
		//AfxMessageBox(strFileData);
		return(2);
	}

	int nIdx;
	if (pDoc->m_bNewLotShare[0] && (pDoc->WorkingInfo.LastJob.bLotSep || pDoc->m_bDoneChgLot))
		nIdx = GetPcrIdx0(nSerial, TRUE); // 릴맵화면 표시 인덱스
	else
		nIdx = GetPcrIdx0(nSerial);

	if (!m_pPcrInner[0] || !m_pPcrInner[1] || !m_pPcrInner[2])
		return(2);
	if (!m_pPcrInner[0][nIdx] || !m_pPcrInner[1][nIdx] || !m_pPcrInner[2][nIdx])
		return(2);

	m_pPcrInner[2][nIdx]->m_nIdx = nIdx;							// m_nIdx : From 0 to nTot.....
	m_pPcrInner[2][nIdx]->m_nSerial = nSerial;

	// Error Code											// 1(정상), -1(Align Error, 노광불량), -2(Lot End)
	if (m_pPcrInner[0][nIdx]->m_nErrPnl < 0)
		m_pPcrInner[2][nIdx]->m_nErrPnl = m_pPcrInner[0][nIdx]->m_nErrPnl;
	else if (m_pPcrInner[1][nIdx]->m_nErrPnl < 0)
		m_pPcrInner[2][nIdx]->m_nErrPnl = m_pPcrInner[1][nIdx]->m_nErrPnl;
	else
		m_pPcrInner[2][nIdx]->m_nErrPnl = m_pPcrInner[0][nIdx]->m_nErrPnl;

	// Model
	m_pPcrInner[2][nIdx]->m_sModel = m_pPcrInner[0][nIdx]->m_sModel;

	// Layer
	m_pPcrInner[2][nIdx]->m_sLayer = m_pPcrInner[0][nIdx]->m_sLayer;

	// Lot
	m_pPcrInner[2][nIdx]->m_sLot = m_pPcrInner[0][nIdx]->m_sLot;

	int nTotDef[3] = { 0 };										// [0]: 상면, [1]: 하면, [2]: 상/하면 Merge
	nTotDef[0] = m_pPcrInner[0][nIdx]->m_nTotDef;			// 상면 불량 피스 수
	nTotDef[1] = m_pPcrInner[1][nIdx]->m_nTotDef;			// 하면 불량 피스 수

	int nTotPcs = m_MasterInner[0].m_pPcsRgn->nTotPcs;
	stPcrMerge *pPcrMgr = new stPcrMerge[nTotPcs];

	int nPcsId;												// nPcsId : CamMaster Pcs Index
															//pPcrMgr테이블의 nIdx에 하면의 불량을 먼저 기록하고 상면의 불량을 엎어서 최종 merge불량 테이블을 만듬.
	for (i = 0; i < nTotDef[1]; i++)						// 하면 불량 피스 수
	{
		nPcsId = m_pPcrInner[1][nIdx]->m_pDefPcs[i];				// nPcsId : CamMaster Pcs Index
		pPcrMgr[nPcsId].nIdx = i;	// Up+Dn				// 상하면 Merge [nPcsId:CamMaster Pcs Index]의 nIdx (0~)는 불량표시순서임.
		pPcrMgr[nPcsId].nIdxDn = i; // Dn					// 상하면 Merge [nPcsId:CamMaster Pcs Index]의 nIdxDn (0~)는 불량표시순서임.
	}
	for (i = 0; i < nTotDef[0]; i++)						// 상면 불량 피스 수
	{
		nPcsId = m_pPcrInner[0][nIdx]->m_pDefPcs[i];				// nPcsId : CamMaster Pcs Index
		pPcrMgr[nPcsId].nIdx = i;	// Up+Dn				// 상하면 Merge [nPcsId:CamMaster Pcs Index]의 nIdx (0~)는 불량표시순서임.
		pPcrMgr[nPcsId].nIdxUp = i;	// Up					// 상하면 Merge [nPcsId:CamMaster Pcs Index]의 nIdxUp (0~)는 불량표시순서임.
	}


	nTotDef[2] = 0;
	for (i = 0; i < nTotPcs; i++)							// Shot내 총 Piece수
	{
		if (pPcrMgr[i].nIdx > -1)	// Up+Dn				// 상하면 Merge [i:CamMaster Pcs Index]의 nIdx (0~)는 불량표시순서임.
			nTotDef[2]++;									// 상 / 하면 Merge한 총 불량피스수.
	}

	m_pPcrInner[2][nIdx]->Init(nSerial, nTotDef[2]);				// 제품시리얼, Shot내 총불량 피스수

	int nId[2], Ord; // [0]: 상면 0~불량피스순서, [1]: 하면 0~불량피스순서
	idx = 0; // 마킹순서 0~불량피스수만큼 정하기위해 현시점의 idx를 초기화함.
	if (nTotDef[2] > 0) // 상 / 하면 Merge한 총 불량피스수.
	{
		for (nPcsId = 0; nPcsId < nTotPcs; nPcsId++)
		{
			nId[0] = pPcrMgr[nPcsId].nIdxUp; // 상하면 Merge [nPcsId:CamMaster Pcs Index]의 nIdxUp (0~)는 불량표시순서 임. nId[0]: 상면에서의 PCR파일순서 인덱스
			nId[1] = pPcrMgr[nPcsId].nIdxDn; // 상하면 Merge [nPcsId:CamMaster Pcs Index]의 nIdxDn (0~)는 불량표시순서 임. nId[1]: 하면에서의 PCR파일순서 인덱스

			if (nId[0] > -1)
			{
				Ord = nId[0];
				// Cam ID
				m_pPcrInner[2][nIdx]->m_nCamId = m_pPcrInner[0][nIdx]->m_nCamId;
				// Piece Number
				m_pPcrInner[2][nIdx]->m_pDefPcs[idx] = m_pPcrInner[0][nIdx]->m_pDefPcs[Ord];
				m_pPcrInner[2][nIdx]->m_pLayer[idx] = m_pPcrInner[0][nIdx]->m_pLayer[Ord];
				// BadPointPosX
				m_pPcrInner[2][nIdx]->m_pDefPos[idx].x = m_pPcrInner[0][nIdx]->m_pDefPos[Ord].x;
				// BadPointPosY
				m_pPcrInner[2][nIdx]->m_pDefPos[idx].y = m_pPcrInner[0][nIdx]->m_pDefPos[Ord].y;
				// BadName
				m_pPcrInner[2][nIdx]->m_pDefType[idx] = m_pPcrInner[0][nIdx]->m_pDefType[Ord];
				// CellNum
				m_pPcrInner[2][nIdx]->m_pCell[idx] = m_pPcrInner[0][nIdx]->m_pCell[Ord];
				// ImageSize
				m_pPcrInner[2][nIdx]->m_pImgSz[idx] = m_pPcrInner[0][nIdx]->m_pImgSz[Ord];
				// ImageNum
				m_pPcrInner[2][nIdx]->m_pImg[idx] = m_pPcrInner[0][nIdx]->m_pImg[Ord];
				// strMarkingCode : -2 (NoMarking)
				m_pPcrInner[2][nIdx]->m_pMk[idx] = m_pPcrInner[0][nIdx]->m_pMk[Ord];

				idx++;
			}
			else if (nId[1] > -1)
			{
				Ord = nId[1];
				// Cam ID
				m_pPcrInner[2][nIdx]->m_nCamId = m_pPcrInner[1][nIdx]->m_nCamId;
				// Piece Number
				m_pPcrInner[2][nIdx]->m_pDefPcs[idx] = m_pPcrInner[1][nIdx]->m_pDefPcs[Ord];
				m_pPcrInner[2][nIdx]->m_pLayer[idx] = m_pPcrInner[1][nIdx]->m_pLayer[Ord];
				// BadPointPosX
				m_pPcrInner[2][nIdx]->m_pDefPos[idx].x = m_pPcrInner[1][nIdx]->m_pDefPos[Ord].x;
				// BadPointPosY
				m_pPcrInner[2][nIdx]->m_pDefPos[idx].y = m_pPcrInner[1][nIdx]->m_pDefPos[Ord].y;
				// BadName
				m_pPcrInner[2][nIdx]->m_pDefType[idx] = m_pPcrInner[1][nIdx]->m_pDefType[Ord];
				// CellNum
				m_pPcrInner[2][nIdx]->m_pCell[idx] = m_pPcrInner[1][nIdx]->m_pCell[Ord];
				// ImageSize
				m_pPcrInner[2][nIdx]->m_pImgSz[idx] = m_pPcrInner[1][nIdx]->m_pImgSz[Ord];
				// ImageNum
				m_pPcrInner[2][nIdx]->m_pImg[idx] = m_pPcrInner[1][nIdx]->m_pImg[Ord];
				// strMarkingCode : -2 (NoMarking)
				m_pPcrInner[2][nIdx]->m_pMk[idx] = m_pPcrInner[1][nIdx]->m_pMk[Ord];

				idx++;
			}
		}
	}

	delete[] pPcrMgr;

	return (1); // 1(정상)
}

int CManagerReelmap::LoadPCRAllDnInner(int nSerial, BOOL bFromShare)	// return : 2(Failed), 1(정상), -1(Align Error, 노광불량), -2(Lot End)
{
	BOOL bDualTest;
	CString sLot, sLayerUp, sLayerDn;
	if (!GetItsSerialInfo(nSerial, bDualTest, sLot, sLayerUp, sLayerDn, 1))
	{
		CString str;
		str.Format(_T("It is trouble to read GetItsSerialInfo()."));
		pView->MsgBox(str);
		return 0;
	}

	if (!bDualTest)
		return 1;

	int i, idx;//k, 
	CString str;

	if (nSerial <= 0)
	{
		str.Format(_T("PCR파일이 설정되지 않았습니다."));
		pView->MsgBox(str);
		// 		AfxMessageBox(strFileData);
		return(2);
	}

	if (!m_pPcr[3])
	{
		str.Format(_T("PCR[3]관련 메모리가 할당되지 않았습니다."));
		pView->MsgBox(str);
		// 		AfxMessageBox(strFileData);
		return(2);
	}

	int nIdx;
	if (pDoc->m_bNewLotShare[1] && (pDoc->WorkingInfo.LastJob.bLotSep || pDoc->m_bDoneChgLot))
		nIdx = GetPcrIdx1(nSerial, TRUE);
	else
		nIdx = GetPcrIdx1(nSerial);

	if (!m_pPcrInner[0] || !m_pPcrInner[1] || !m_pPcrInner[3])
		return(2);
	if (!m_pPcrInner[0][nIdx] || !m_pPcrInner[1][nIdx] || !m_pPcrInner[3][nIdx])
		return(2);

	m_pPcrInner[3][nIdx]->m_nIdx = nIdx;							// m_nIdx : From 0 to nTot.....
	m_pPcrInner[3][nIdx]->m_nSerial = nSerial;

	// Error Code											// 1(정상), -1(Align Error, 노광불량), -2(Lot End)
	if (m_pPcrInner[1][nIdx]->m_nErrPnl < 0)
		m_pPcrInner[3][nIdx]->m_nErrPnl = m_pPcrInner[1][nIdx]->m_nErrPnl;
	else if (m_pPcrInner[0][nIdx]->m_nErrPnl < 0)
		m_pPcrInner[3][nIdx]->m_nErrPnl = m_pPcrInner[0][nIdx]->m_nErrPnl;
	else
		m_pPcrInner[3][nIdx]->m_nErrPnl = m_pPcrInner[1][nIdx]->m_nErrPnl;

	// Model
	m_pPcrInner[3][nIdx]->m_sModel = m_pPcrInner[1][nIdx]->m_sModel;

	// Layer
	m_pPcrInner[3][nIdx]->m_sLayer = m_pPcrInner[1][nIdx]->m_sLayer;

	// Lot
	m_pPcrInner[3][nIdx]->m_sLot = m_pPcrInner[1][nIdx]->m_sLot;

	int nTotDef[3];
	nTotDef[0] = pView->m_mgrReelmap->m_pPcrInner[0][nIdx]->m_nTotDef;
	nTotDef[1] = pView->m_mgrReelmap->m_pPcrInner[1][nIdx]->m_nTotDef;
	//	nTotDef[2] = nTotDef[0] + nTotDef[1];

	int nTotPcs = m_MasterInner[0].m_pPcsRgn->nTotPcs;
	stPcrMerge *pPcrMgr = new stPcrMerge[nTotPcs];

	int nComp, nPcsId;
	if (nTotDef[0] > nTotDef[1])
		nComp = nTotDef[0];
	else
		nComp = nTotDef[1];

	for (i = 0; i < nTotDef[0]; i++)
	{
		nPcsId = m_pPcrInner[0][nIdx]->m_pDefPcs[i];
		pPcrMgr[nPcsId].nIdx = i;
		pPcrMgr[nPcsId].nIdxUp = i;	// Up
	}
	for (i = 0; i < nTotDef[1]; i++)
	{
		nPcsId = m_pPcrInner[1][nIdx]->m_pDefPcs[i];
		pPcrMgr[nPcsId].nIdx = i;
		pPcrMgr[nPcsId].nIdxDn = i; // Dn
	}

	nTotDef[2] = 0;
	for (i = 0; i < nTotPcs; i++)
	{
		if (pPcrMgr[i].nIdx > -1)
			nTotDef[2]++;
	}

	m_pPcrInner[3][nIdx]->Init(nSerial, nTotDef[2]);

	int nId[2], Ord;
	idx = 0;
	if (nTotDef[2] > 0)
	{
		for (nPcsId = 0; nPcsId < nTotPcs; nPcsId++)
		{
			nId[0] = pPcrMgr[nPcsId].nIdxUp;
			nId[1] = pPcrMgr[nPcsId].nIdxDn;

			if (nId[1] > -1)
			{
				Ord = nId[1];
				// Cam ID
				m_pPcrInner[3][nIdx]->m_nCamId = m_pPcrInner[1][nIdx]->m_nCamId;
				// Piece Number
				m_pPcrInner[3][nIdx]->m_pDefPcs[idx] = m_pPcrInner[1][nIdx]->m_pDefPcs[Ord];
				m_pPcrInner[3][nIdx]->m_pLayer[idx] = m_pPcrInner[1][nIdx]->m_pLayer[Ord];
				// BadPointPosX
				m_pPcrInner[3][nIdx]->m_pDefPos[idx].x = m_pPcrInner[1][nIdx]->m_pDefPos[Ord].x;
				// BadPointPosY
				m_pPcrInner[3][nIdx]->m_pDefPos[idx].y = m_pPcrInner[1][nIdx]->m_pDefPos[Ord].y;
				// BadName
				m_pPcrInner[3][nIdx]->m_pDefType[idx] = m_pPcrInner[1][nIdx]->m_pDefType[Ord];
				// CellNum
				m_pPcrInner[3][nIdx]->m_pCell[idx] = m_pPcrInner[1][nIdx]->m_pCell[Ord];
				// ImageSize
				m_pPcrInner[3][nIdx]->m_pImgSz[idx] = m_pPcrInner[1][nIdx]->m_pImgSz[Ord];
				// ImageNum
				m_pPcrInner[3][nIdx]->m_pImg[idx] = m_pPcrInner[1][nIdx]->m_pImg[Ord];
				// strMarkingCode : -2 (NoMarking)
				m_pPcrInner[3][nIdx]->m_pMk[idx] = m_pPcrInner[1][nIdx]->m_pMk[Ord];

				idx++;
			}
			else if (nId[0] > -1)
			{
				Ord = nId[0];
				// Cam ID
				m_pPcrInner[3][nIdx]->m_nCamId = m_pPcrInner[0][nIdx]->m_nCamId;
				// Piece Number
				m_pPcrInner[3][nIdx]->m_pDefPcs[idx] = m_pPcrInner[0][nIdx]->m_pDefPcs[Ord];
				m_pPcrInner[3][nIdx]->m_pLayer[idx] = m_pPcrInner[0][nIdx]->m_pLayer[Ord];
				// BadPointPosX
				m_pPcrInner[3][nIdx]->m_pDefPos[idx].x = m_pPcrInner[0][nIdx]->m_pDefPos[Ord].x;
				// BadPointPosY
				m_pPcrInner[3][nIdx]->m_pDefPos[idx].y = m_pPcrInner[0][nIdx]->m_pDefPos[Ord].y;
				// BadName
				m_pPcrInner[3][nIdx]->m_pDefType[idx] = m_pPcrInner[0][nIdx]->m_pDefType[Ord];
				// CellNum
				m_pPcrInner[3][nIdx]->m_pCell[idx] = m_pPcrInner[0][nIdx]->m_pCell[Ord];
				// ImageSize
				m_pPcrInner[3][nIdx]->m_pImgSz[idx] = m_pPcrInner[0][nIdx]->m_pImgSz[Ord];
				// ImageNum
				m_pPcrInner[3][nIdx]->m_pImg[idx] = m_pPcrInner[0][nIdx]->m_pImg[Ord];
				// strMarkingCode : -2 (NoMarking)
				m_pPcrInner[3][nIdx]->m_pMk[idx] = m_pPcrInner[0][nIdx]->m_pMk[Ord];

				idx++;
			}
		}
	}

	delete[] pPcrMgr;

	return (1); // 1(정상)
}

int CManagerReelmap::LoadPCRUpInner(int nSerial, BOOL bFromShare)	// return : 2(Failed), 1(정상), -1(Align Error, 노광불량), -2(Lot End)
{
	FILE *fp;
	char FileD[200];
	size_t nFileSize, nRSize;
	char *FileData;
	CString strFileData;
	int nTemp, i;
	CString strHeaderErrorInfo, strModel, strLayer, strLot, sItsCode, strTotalBadPieceNum;
	CString strCamID, strPieceID, strBadPointPosX, strBadPointPosY, strBadName,
		strCellNum, strImageSize, strImageNum, strMarkingCode;

	if (nSerial <= 0)
	{
		strFileData.Format(_T("PCR파일이 설정되지 않았습니다."));
		pView->MsgBox(strFileData);
		//AfxMessageBox(strFileData);
		return(2);
	}

	if (!m_pPcrInner[0])
	{
		strFileData.Format(_T("PCR[0]관련 메모리가 할당되지 않았습니다."));
		pView->MsgBox(strFileData);
		//AfxMessageBox(strFileData);
		return(2);
	}

	int nIdx;
	if (pDoc->m_bNewLotShare[0] && (pDoc->WorkingInfo.LastJob.bLotSep || pDoc->m_bDoneChgLot))
		nIdx = GetPcrIdx0(nSerial, TRUE);
	else
		nIdx = GetPcrIdx0(nSerial);

	CString sPath, sUpPath, sDnPath;

	if (!GetInnerFolderPath(nSerial, sUpPath, sDnPath))
	{
		strFileData.Format(_T("GetInnerFolderPath가 설정되지 않았습니다."));
		pView->MsgBox(strFileData);
		//AfxMessageBox(strFileData);
		return(2);
	}
#ifdef TEST_MODE
	sPath = PATH_PCR;	// for Test
#else
	//if (bFromShare)
	//	sPath.Format(_T("%s%04d.pcr"), sUpPath, nSerial);
	//else
	sPath.Format(_T("%s%04d.pcr"), sUpPath, nSerial);
#endif

	//strcpy(FileD, sPath);
	//_tcscpy(FileD, sPath);
	StringToChar(sPath, FileD);

	if ((fp = fopen(FileD, "r")) != NULL)
	{
		fseek(fp, 0, SEEK_END);
		nFileSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		/* Allocate space for a path name */
		//FileData = (char*)malloc( nFileSize );
		FileData = (char*)calloc(nFileSize + 1, sizeof(char));

		nRSize = fread(FileData, sizeof(char), nFileSize, fp);
		//strFileData.Format(_T("%s"), CharToString(FileData));
		strFileData = CharToString(FileData);
		fclose(fp);
		free(FileData);
	}
	else
	{
		strFileData.Format(_T("PCRInner[Up] 파일이 존재하지 않습니다.\r\n%s"), sPath);
		pView->MsgBox(strFileData);
		// 		AfxMessageBox(strFileData);
		return(2);
	}

	if (!m_pPcrInner[0])
		return(2);
	if (!m_pPcrInner[0][nIdx])
		return(2);

	BOOL bResetMkInfo = FALSE;

	m_pPcrInner[0][nIdx]->m_nIdx = nIdx;							// m_nIdx : From 0 to nTot.....
	m_pPcrInner[0][nIdx]->m_nSerial = nSerial;

	// Error Code											// 1(정상), -1(Align Error, 노광불량), -2(Lot End)
	nTemp = strFileData.Find(',', 0);
	strHeaderErrorInfo = strFileData.Left(nTemp);
	strFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;
	m_pPcrInner[0][nIdx]->m_nErrPnl = _tstoi(strHeaderErrorInfo);

	if (m_pPcrInner[1][nIdx]->m_nErrPnl == -1)
	{
		int syd = 1;
	}

	// Model
	nTemp = strFileData.Find(',', 0);
	strModel = strFileData.Left(nTemp);
	strFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;
	m_pPcrInner[0][nIdx]->m_sModel = strModel;

	// Layer
	nTemp = strFileData.Find(',', 0);
	strLayer = strFileData.Left(nTemp);
	strFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;
	m_pPcrInner[0][nIdx]->m_sLayer = strLayer;

	if (pDoc->WorkingInfo.System.bUseITS)
	{
		// Lot
		nTemp = strFileData.Find(',', 0);
		strLot = strFileData.Left(nTemp);
		strFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;
		m_pPcrInner[0][nIdx]->m_sLot = strLot;

		// Its Code
		nTemp = strFileData.Find('\n', 0);
		sItsCode = strFileData.Left(nTemp);
		strFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;
		m_pPcr[0][nIdx]->m_sItsCode = sItsCode;
	}
	else
	{
		// Lot
		nTemp = strFileData.Find(',', 0);
		strLot = strFileData.Left(nTemp);
		strFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;
		m_pPcrInner[0][nIdx]->m_sLot = strLot;

		// Lot
		nTemp = strFileData.Find('\n', 0);
		strLot = strFileData.Left(nTemp);
		strFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;
		m_pPcrInner[0][nIdx]->m_sLot = strLot;
	}

	nTemp = strFileData.Find('\n', 0);
	strTotalBadPieceNum = strFileData.Left(nTemp);;
	strFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;

	//if (!strModel.IsEmpty() && !strLot.IsEmpty() && !strLayer.IsEmpty())
	//{
	//	if (WorkingInfo.LastJob.sModelUp.IsEmpty() || WorkingInfo.LastJob.sLotUp.IsEmpty() || WorkingInfo.LastJob.sLayerUp.IsEmpty())
	//	{
	//		WorkingInfo.LastJob.sModelUp = strModel;
	//		WorkingInfo.LastJob.sLotUp = strLot;
	//		WorkingInfo.LastJob.sLayerUp = strLayer;

	//		if (!WorkingInfo.LastJob.bDualTest)
	//		{
	//			pView->ResetMkInfo(0); // 0 : AOI-Up , 1 : AOI-Dn , 2 : AOI-UpDn
	//		}
	//	}
	//	else if (WorkingInfo.LastJob.sModelUp != strModel || WorkingInfo.LastJob.sLayerUp != strLayer || WorkingInfo.LastJob.sLotUp != strLot)
	//	{
	//		WorkingInfo.LastJob.sModelUp = strModel;
	//		WorkingInfo.LastJob.sLotUp = strLot;
	//		WorkingInfo.LastJob.sLayerUp = strLayer;

	//		if (!WorkingInfo.LastJob.bDualTest)
	//		{
	//			pView->ResetMkInfo(0); // 0 : AOI-Up , 1 : AOI-Dn , 2 : AOI-UpDn
	//		}
	//	}
	//}

	int nTotDef = _tstoi(strTotalBadPieceNum);

	m_pPcrInner[0][nIdx]->Init(nSerial, nTotDef);

	if (nTotDef > 0)
	{
		for (i = 0; i < nTotDef; i++)
		{
			// Cam ID
			nTemp = strFileData.Find(',', 0);
			strCamID = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			m_pPcrInner[0][nIdx]->m_nCamId = _tstoi(strCamID);

			// Piece Number
			nTemp = strFileData.Find(',', 0);
			strPieceID = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;

			// LoadStripPieceRegion_Binary()에 의해 PCS Index가 결정됨.
			if (pDoc->WorkingInfo.System.bStripPcsRgnBin)	// DTS용
				m_pPcrInner[0][nIdx]->m_pDefPcs[i] = _tstoi(strPieceID);
			else
				m_pPcrInner[0][nIdx]->m_pDefPcs[i] = _tstoi(strPieceID);

			m_pPcrInner[0][nIdx]->m_pLayer[i] = 0; // Up

												   // BadPointPosX
			nTemp = strFileData.Find(',', 0);
			strBadPointPosX = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			m_pPcrInner[0][nIdx]->m_pDefPos[i].x = (long)_tstoi(strBadPointPosX);

			// BadPointPosY
			nTemp = strFileData.Find(',', 0);
			strBadPointPosY = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			m_pPcrInner[0][nIdx]->m_pDefPos[i].y = (long)_tstoi(strBadPointPosY);

			// BadName
			nTemp = strFileData.Find(',', 0);
			strBadName = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			m_pPcrInner[0][nIdx]->m_pDefType[i] = _tstoi(strBadName);

			// CellNum
			nTemp = strFileData.Find(',', 0);
			strCellNum = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			m_pPcrInner[0][nIdx]->m_pCell[i] = _tstoi(strCellNum);

			// ImageSize
			nTemp = strFileData.Find(',', 0);
			strImageSize = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			m_pPcrInner[0][nIdx]->m_pImgSz[i] = _tstoi(strImageSize);

			// ImageNum
			nTemp = strFileData.Find(',', 0);
			strImageNum = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			m_pPcrInner[0][nIdx]->m_pImg[i] = _tstoi(strImageNum);

			// strMarkingCode : -2 (NoMarking)
			nTemp = strFileData.Find('\n', 0);
			strMarkingCode = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			m_pPcrInner[0][nIdx]->m_pMk[i] = _tstoi(strMarkingCode);
		}
	}

	return (1); // 1(정상)
}

int CManagerReelmap::LoadPCRDnInner(int nSerial, BOOL bFromShare)	// return : 2(Failed), 1(정상), -1(Align Error, 노광불량), -2(Lot End)
{
	BOOL bDualTest;
	CString sLot, sLayerUp, sLayerDn;
	if (!GetItsSerialInfo(nSerial, bDualTest, sLot, sLayerUp, sLayerDn, 1))
	{
		CString str;
		str.Format(_T("It is trouble to read GetItsSerialInfo()."));
		pView->MsgBox(str);
		return 0;
	}

	if (!bDualTest)
		return 1;

	FILE *fp;
	char FileD[200];
	size_t nFileSize, nRSize;
	char *FileData;
	CString strFileData;
	int nTemp, i;
	CString strHeaderErrorInfo, strModel, strLayer, strLot, sItsCode, strTotalBadPieceNum;
	CString strCamID, strPieceID, strBadPointPosX, strBadPointPosY, strBadName,
		strCellNum, strImageSize, strImageNum, strMarkingCode;

	if (nSerial <= 0)
	{
		strFileData.Format(_T("PCR파일이 설정되지 않았습니다."));
		pView->MsgBox(strFileData);
		// 		AfxMessageBox(strFileData);
		return(2);
	}

	if (!m_pPcrInner[1])
	{
		strFileData.Format(_T("PCR[1]관련 메모리가 할당되지 않았습니다."));
		pView->MsgBox(strFileData);
		// 		AfxMessageBox(strFileData);
		return(2);
	}

	int nIdx;
	if (pDoc->m_bNewLotShare[1] && (pDoc->WorkingInfo.LastJob.bLotSep || pDoc->m_bDoneChgLot))
		nIdx = GetPcrIdx1(nSerial, TRUE);
	else
		nIdx = GetPcrIdx1(nSerial);

	CString sPath, sUpPath, sDnPath;

	if (!GetInnerFolderPath(nSerial, sUpPath, sDnPath))
	{
		strFileData.Format(_T("GetInnerFolderPath가 설정되지 않았습니다."));
		pView->MsgBox(strFileData);
		//AfxMessageBox(strFileData);
		return(2);
	}

#ifdef TEST_MODE
	sPath = PATH_PCR;	// for Test
#else
	//if (bFromShare)
	//	sPath.Format(_T("%s%04d.pcr"), WorkingInfo.System.sPathVrsShareDn, nSerial);
	//else
	sPath.Format(_T("%s%04d.pcr"), sDnPath, nSerial);
#endif

	//strcpy(FileD, sPath);
	//_tcscpy(FileD, sPath);
	StringToChar(sPath, FileD);

	if ((fp = fopen(FileD, "r")) != NULL)
	{
		fseek(fp, 0, SEEK_END);
		nFileSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		/* Allocate space for a path name */
		//FileData = (char*)malloc( nFileSize );
		FileData = (char*)calloc(nFileSize + 1, sizeof(char));

		nRSize = fread(FileData, sizeof(char), nFileSize, fp);
		//strFileData.Format(_T("%s"), CharToString(FileData));
		strFileData = CharToString(FileData);
		fclose(fp);
		free(FileData);
	}
	else
	{
		strFileData.Format(_T("PCRInner[Dn] 파일이 존재하지 않습니다.\r\n%s"), sPath);
		pView->MsgBox(strFileData);
		//		AfxMessageBox(strFileData);
		return(2);
	}

	if (!m_pPcrInner[1])
		return(2);
	if (!m_pPcrInner[1][nIdx])
		return(2);

	m_pPcrInner[1][nIdx]->m_nIdx = nIdx;							// m_nIdx : From 0 to nTot.....
	m_pPcrInner[1][nIdx]->m_nSerial = nSerial;

	// Error Code											// 1(정상), -1(Align Error, 노광불량), -2(Lot End)
	nTemp = strFileData.Find(',', 0);
	strHeaderErrorInfo = strFileData.Left(nTemp);
	strFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;
	m_pPcrInner[1][nIdx]->m_nErrPnl = _tstoi(strHeaderErrorInfo);

	if (m_pPcrInner[1][nIdx]->m_nErrPnl == -1)
	{
		int syd = 1;
	}

	// Model
	nTemp = strFileData.Find(',', 0);
	strModel = strFileData.Left(nTemp);
	strFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;
	m_pPcrInner[1][nIdx]->m_sModel = strModel;

	// Layer
	nTemp = strFileData.Find(',', 0);
	strLayer = strFileData.Left(nTemp);
	strFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;
	m_pPcrInner[1][nIdx]->m_sLayer = strLayer;

	if (pDoc->WorkingInfo.System.bUseITS)
	{
		// Lot
		nTemp = strFileData.Find(',', 0);
		strLot = strFileData.Left(nTemp);
		strFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;
		m_pPcrInner[1][nIdx]->m_sLot = strLot;

		// Its Code
		nTemp = strFileData.Find('\n', 0);
		sItsCode = strFileData.Left(nTemp);
		strFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;
		m_pPcr[0][nIdx]->m_sItsCode = sItsCode;
	}
	else
	{
		// Lot
		nTemp = strFileData.Find(',', 0);
		strLot = strFileData.Left(nTemp);
		strFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;
		m_pPcrInner[1][nIdx]->m_sLot = strLot;

		// Lot
		nTemp = strFileData.Find('\n', 0);
		strLot = strFileData.Left(nTemp);
		strFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;
		m_pPcrInner[1][nIdx]->m_sLot = strLot;
	}

	nTemp = strFileData.Find('\n', 0);
	strTotalBadPieceNum = strFileData.Left(nTemp);;
	strFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;

	int nTotDef = _tstoi(strTotalBadPieceNum);

	m_pPcrInner[1][nIdx]->Init(nSerial, nTotDef);

	if (nTotDef > 0)
	{
		for (i = 0; i < nTotDef; i++)
		{
			// Cam ID
			nTemp = strFileData.Find(',', 0);
			strCamID = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			m_pPcrInner[1][nIdx]->m_nCamId = _tstoi(strCamID);

			// Piece Number
			nTemp = strFileData.Find(',', 0);
			strPieceID = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;

			// LoadStripPieceRegion_Binary()에 의해 PCS Index가 결정됨.
			if (pDoc->WorkingInfo.System.bStripPcsRgnBin)	// DTS용
				m_pPcrInner[1][nIdx]->m_pDefPcs[i] = _tstoi(strPieceID);
			else
				m_pPcrInner[1][nIdx]->m_pDefPcs[i] = MirrorLR(_tstoi(strPieceID));	// 초기 양면검사기용

			m_pPcrInner[1][nIdx]->m_pLayer[i] = 1; // Dn

												   // BadPointPosX
			nTemp = strFileData.Find(',', 0);
			strBadPointPosX = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			m_pPcrInner[1][nIdx]->m_pDefPos[i].x = (long)_tstoi(strBadPointPosX);

			// BadPointPosY
			nTemp = strFileData.Find(',', 0);
			strBadPointPosY = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			m_pPcrInner[1][nIdx]->m_pDefPos[i].y = (long)_tstoi(strBadPointPosY);

			// BadName
			nTemp = strFileData.Find(',', 0);
			strBadName = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			m_pPcrInner[1][nIdx]->m_pDefType[i] = _tstoi(strBadName);

			// CellNum
			nTemp = strFileData.Find(',', 0);
			strCellNum = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			m_pPcrInner[1][nIdx]->m_pCell[i] = _tstoi(strCellNum);

			// ImageSize
			nTemp = strFileData.Find(',', 0);
			strImageSize = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			m_pPcrInner[1][nIdx]->m_pImgSz[i] = _tstoi(strImageSize);

			// ImageNum
			nTemp = strFileData.Find(',', 0);
			strImageNum = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			m_pPcrInner[1][nIdx]->m_pImg[i] = _tstoi(strImageNum);

			// strMarkingCode : -2 (NoMarking)
			nTemp = strFileData.Find('\n', 0);
			strMarkingCode = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			m_pPcrInner[1][nIdx]->m_pMk[i] = _tstoi(strMarkingCode);
		}
	}

	return (1); // 1(정상)
				//return(m_pPcr[1][nIdx]->m_nErrPnl);
}

void CManagerReelmap::UpdateYieldOnRmap()
{
	if (!pView || !pView->m_mgrProcedure)
		return;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	pView->m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_UP = TRUE;		// UpdateReelmapYieldUp(); // Yield Reelmap
	if (bDualTest)
	{
		pView->m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_DN = TRUE;	// UpdateReelmapYieldDn(); // Yield Reelmap
	}
	Sleep(100);
}

void CManagerReelmap::UpdateData()
{
	if (!m_pReelMap)
		return;

	m_pReelMap->m_sMc = pDoc->WorkingInfo.System.sMcName;
	m_pReelMap->m_sUser = pDoc->WorkingInfo.LastJob.sSelUserName;

	m_pReelMap->m_bUseLotSep = pDoc->WorkingInfo.LastJob.bLotSep;
	m_pReelMap->m_bUseTempPause = pDoc->WorkingInfo.LastJob.bTempPause;

	m_pReelMap->m_dTotLen = _tstof(pDoc->WorkingInfo.LastJob.sReelTotLen)*1000.0;
	m_pReelMap->m_dPnlLen = _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen);
	m_pReelMap->m_dLotCutPosLen = _tstof(pDoc->WorkingInfo.LastJob.sLotCutPosLen);
	if (m_pReelMap->m_bUseLotSep)
		m_pReelMap->m_dLotLen = _tstof(pDoc->WorkingInfo.LastJob.sLotSepLen);
	else
		m_pReelMap->m_dLotLen = m_pReelMap->m_dTotLen;
	m_pReelMap->m_dTempPauseLen = _tstof(pDoc->WorkingInfo.LastJob.sTempPauseLen);

}
