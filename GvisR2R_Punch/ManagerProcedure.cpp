// ManagerProcedure.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "GvisR2R_Punch.h"
#include "ManagerProcedure.h"


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
	m_nWatiDispMain = 0;
	m_nNewLot = 0;

	m_bStopFromThread = FALSE;
	m_bBuzzerFromThread = FALSE;

	m_nRtnMyMsgBoxIdx = -1;
	m_bDispMyMsgBox = FALSE;
	m_bDispMain = FALSE;
	m_bProbDn[0] = m_bProbDn[1] = FALSE;

	m_nSelRmap = RMAP_UP;
	m_nSelRmapInner = RMAP_INNER_UP;

	m_bTIM_INIT_VIEW = FALSE;
	m_nStepInitView = 0;
	m_nLotEndSerial = 0;

	m_nDummy[0] = 0;
	m_nDummy[1] = 0;
	m_nAoiLastSerial[0] = 0;
	m_nAoiLastSerial[1] = 0;
	m_bChkLastProcVs = FALSE;

	m_dTempPauseLen = 0.0;
	m_dElecChkVal = 0.0;

	m_bInit = FALSE;
	m_bDispMsg = FALSE;
	for (int kk = 0; kk < 10; kk++)
	{
		m_bDispMsgDoAuto[kk] = FALSE;
		m_nStepDispMsg[kk] = 0;
	}
	m_sFixMsg[0] = _T("");
	m_sFixMsg[1] = _T("");

	m_bWaitClrDispMsg = FALSE;
	m_bStopFeeding = FALSE;
	m_nCntTowerWinker = 0;
	m_bTimTowerWinker = FALSE;
	m_bTimBtnWinker = FALSE;
	m_bTimBuzzerWarn = FALSE;
	m_bTowerWinker[0] = FALSE; // R
	m_bTowerWinker[1] = FALSE; // G
	m_bTowerWinker[2] = FALSE; // Y
	m_bBtnWinker[0] = FALSE; // Ready
	m_bBtnWinker[1] = FALSE; // Reset
	m_bBtnWinker[2] = FALSE; // Run
	m_bBtnWinker[3] = FALSE; // Stop
	for (int nI = 0; nI < 4; nI++)
	{
		m_nCntBtnWinker[nI] = 0;
		m_nDlyWinker[nI] = 5;
	}

	m_bAoiFdWriteF[0] = FALSE;
	m_bAoiFdWriteF[1] = FALSE;
	m_bAoiTest[0] = FALSE;
	m_bAoiTest[1] = FALSE;

	m_bEngFdWrite = FALSE;
	m_bEngFdWriteF = FALSE;
	m_bEngTest = FALSE;
	m_bEngTestF = FALSE;

	m_bTIM_DISP_STATUS = FALSE;
	m_bTIM_MPE_IO = FALSE;

	m_bTIM_CHK_DONE_READY = FALSE;

	
	m_nStepAuto = 0;
	m_nPrevStepAuto = 0;
	m_nPrevMkStAuto = 0;
	m_nStepMk[0] = 0;
	m_nStepMk[1] = 0;
	m_sTick = _T("");
	m_sDispTime = _T("");

	m_bChkMpeIoOut = FALSE;

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
	//m_bDestroyedView = FALSE;
	m_bContEngraveF = FALSE;

	m_nSaveMk0Img = 0;
	m_nSaveMk1Img = 0;

	m_bStopF_Verify = FALSE;
	m_bInitAuto = TRUE;
	m_bInitAutoLoadMstInfo = FALSE;

	//m_bLoadMstInfo = FALSE;
	//m_bLoadMstInfoF = FALSE;

	//m_sGet2dCodeLot = _T("");
	//m_nGet2dCodeSerial = 0;

	//m_bSetSig = FALSE;
	//m_bSetSigF = FALSE;
	//m_bSetData = FALSE;
	//m_bSetDataF = FALSE;

	//m_bTIM_START_UPDATE = FALSE;
	//m_bEscape = FALSE;
	m_bSerialDecrese = FALSE;

	m_bTHREAD_MK[0] = FALSE;
	m_bTHREAD_MK[1] = FALSE;
	m_bTHREAD_MK[2] = FALSE;
	m_bTHREAD_MK[3] = FALSE;
	m_nMkPcs[0] = 0;
	m_nMkPcs[1] = 0;
	m_nMkPcs[2] = 0;
	m_nMkPcs[3] = 0;

	m_nErrCnt = 0;

	m_bAuto = FALSE;
	m_bManual = FALSE;
	m_bOneCycle = FALSE;

	m_bSwRun = FALSE; m_bSwRunF = FALSE;
	m_bSwStop = FALSE; m_bSwStopF = FALSE;
	m_bSwReset = FALSE; m_bSwResetF = FALSE;
	m_bSwReady = FALSE; m_bSwReadyF = FALSE;

	m_bSwStopNow = FALSE;

	for (int nAxis = 0; nAxis < MAX_AXIS; nAxis++)
		m_dEnc[nAxis] = 0.0;

	for (int i = 0; i < 10; i++)
		m_sDispMsg[i] = _T("");

	m_bNewModel = FALSE;
	m_dTotVel = 0.0; m_dPartVel = 0.0;
	m_bTIM_CHK_TEMP_STOP = FALSE;
	m_bTIM_SAFTY_STOP = FALSE;
	m_bTIM_CAMMASTER_UPDATE = FALSE;
	m_sMyMsg = _T("");
	m_nTypeMyMsg = IDOK;

	m_dwLotSt = 0; m_dwLotEd = 0;

	m_lFuncId = 0;

	m_bDrawGL = TRUE;
	m_bCont = FALSE;
	m_bCam = FALSE;
	m_bReview = FALSE;

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

	ResetPriority();
	m_Flag = 0L;
	m_AoiLdRun = 0L;

	m_bCollision[0] = FALSE;
	m_bCollision[1] = FALSE;

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

	m_bDoMk[0] = TRUE;
	m_bDoMk[1] = TRUE;
	m_bDoneMk[0] = FALSE;
	m_bDoneMk[1] = FALSE;
	m_bReMark[0] = FALSE;
	m_bReMark[1] = FALSE;

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

	m_bCycleStop = FALSE;

	for (int a = 0; a < 2; a++)
	{
		for (int b = 0; b < 4; b++)
		{
			m_nMkStrip[a][b] = 0;
			m_bRejectDone[a][b] = FALSE;
		}
	}

	m_sDispMain = _T("");

	m_bReMk = FALSE;

	m_bWaitPcr[0] = FALSE;
	m_bWaitPcr[1] = FALSE;

	m_bShowMyMsg = FALSE;
	m_pMyMsgForeground = NULL;
	m_bContDiffLot = FALSE;

	m_nStepElecChk = 0;
	// 	m_nMsgShiftX = 0;
	// 	m_nMsgShiftY = 0;

	for (int nAns = 0; nAns < 10; nAns++)
		m_bAnswer[nAns] = FALSE;

	m_bChkLightErr = FALSE;

	InitVal();

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
}

CManagerProcedure::~CManagerProcedure()
{
	m_bTIM_CHK_DONE_READY = FALSE;
}


BEGIN_MESSAGE_MAP(CManagerProcedure, CWnd)
END_MESSAGE_MAP()



// CManagerProcedure 메시지 처리기입니다.

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


//unsigned long CManagerProcedure::ChkDoor() // 0: All Closed , Open Door Index : Doesn't all closed. (Bit3: F, Bit2: L, Bit1: R, Bit0; B)
//{
//	unsigned long ulOpenDoor = 0;
//
//	if (pDoc->WorkingInfo.LastJob.bAoiUpDrSen)
//	{
//		if (pDoc->Status.bDoorAoi[DOOR_FM_AOI_UP] && !pDoc->Status.bDoorAoiF[DOOR_FM_AOI_UP])
//		{
//			ulOpenDoor |= (0x01 << 0);
//			pDoc->Status.bDoorAoiF[DOOR_FM_AOI_UP] = TRUE;
//			Buzzer(FALSE, 0);
//			Sleep(300);
//		}
//		else if (!pDoc->Status.bDoorAoi[DOOR_FM_AOI_UP] && pDoc->Status.bDoorAoiF[DOOR_FM_AOI_UP])
//		{
//			ulOpenDoor &= ~(0x01 << 0);
//			pDoc->Status.bDoorAoiF[DOOR_FM_AOI_UP] = FALSE;
//			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//			pView->m_mgrProcedure->m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("정 지"), RGB_RED);
//			MsgBox(_T("일시정지 - 검사부 상 전면 중앙 도어 Open"));
//			TowerLamp(RGB_RED, TRUE);
//			Buzzer(TRUE, 0);
//		}
//
//		if (pDoc->Status.bDoorAoi[DOOR_FL_AOI_UP] && !pDoc->Status.bDoorAoiF[DOOR_FL_AOI_UP])
//		{
//			ulOpenDoor |= (0x01 << 1);
//			pDoc->Status.bDoorAoiF[DOOR_FL_AOI_UP] = TRUE;
//			Buzzer(FALSE, 0);
//			Sleep(300);
//		}
//		else if (!pDoc->Status.bDoorAoi[DOOR_FL_AOI_UP] && pDoc->Status.bDoorAoiF[DOOR_FL_AOI_UP])
//		{
//			ulOpenDoor &= ~(0x01 << 1);
//			pDoc->Status.bDoorAoiF[DOOR_FL_AOI_UP] = FALSE;
//			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//			pView->m_mgrProcedure->m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("정 지"), RGB_RED);
//			MsgBox(_T("일시정지 - 검사부 상 전면 좌측 도어 Open"));
//			TowerLamp(RGB_RED, TRUE);
//			Buzzer(TRUE, 0);
//		}
//
//		if (pDoc->Status.bDoorAoi[DOOR_FR_AOI_UP] && !pDoc->Status.bDoorAoiF[DOOR_FR_AOI_UP])
//		{
//			ulOpenDoor |= (0x01 << 2);
//			pDoc->Status.bDoorAoiF[DOOR_FR_AOI_UP] = TRUE;
//			Buzzer(FALSE, 0);
//			Sleep(300);
//		}
//		else if (!pDoc->Status.bDoorAoi[DOOR_FR_AOI_UP] && pDoc->Status.bDoorAoiF[DOOR_FR_AOI_UP])
//		{
//			ulOpenDoor &= ~(0x01 << 2);
//			pDoc->Status.bDoorAoiF[DOOR_FR_AOI_UP] = FALSE;
//			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//			pView->m_mgrProcedure->m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("정 지"), RGB_RED);
//			MsgBox(_T("일시정지 - 검사부 상 전면 우측 도어 Open"));
//			TowerLamp(RGB_RED, TRUE);
//			Buzzer(TRUE, 0);
//		}
//
//		if (pDoc->Status.bDoorAoi[DOOR_BM_AOI_UP] && !pDoc->Status.bDoorAoiF[DOOR_BM_AOI_UP])
//		{
//			ulOpenDoor |= (0x01 << 3);
//			pDoc->Status.bDoorAoiF[DOOR_BM_AOI_UP] = TRUE;
//			Buzzer(FALSE, 0);
//			Sleep(300);
//		}
//		else if (!pDoc->Status.bDoorAoi[DOOR_BM_AOI_UP] && pDoc->Status.bDoorAoiF[DOOR_BM_AOI_UP])
//		{
//			ulOpenDoor &= ~(0x01 << 3);
//			pDoc->Status.bDoorAoiF[DOOR_BM_AOI_UP] = FALSE;
//			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//			pView->m_mgrProcedure->m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("정 지"), RGB_RED);
//			MsgBox(_T("일시정지 - 검사부 상 후면 중앙 도어 Open"));
//			TowerLamp(RGB_RED, TRUE);
//			Buzzer(TRUE, 0);
//		}
//
//		if (pDoc->Status.bDoorAoi[DOOR_BL_AOI_UP] && !pDoc->Status.bDoorAoiF[DOOR_BL_AOI_UP])
//		{
//			ulOpenDoor |= (0x01 << 4);
//			pDoc->Status.bDoorAoiF[DOOR_BL_AOI_UP] = TRUE;
//			Buzzer(FALSE, 0);
//			Sleep(300);
//		}
//		else if (!pDoc->Status.bDoorAoi[DOOR_BL_AOI_UP] && pDoc->Status.bDoorAoiF[DOOR_BL_AOI_UP])
//		{
//			ulOpenDoor &= ~(0x01 << 4);
//			pDoc->Status.bDoorAoiF[DOOR_BL_AOI_UP] = FALSE;
//			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//			pView->m_mgrProcedure->m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("정 지"), RGB_RED);
//			MsgBox(_T("일시정지 - 검사부 상 후면 좌측 도어 Open"));
//			TowerLamp(RGB_RED, TRUE);
//			Buzzer(TRUE, 0);
//		}
//
//		if (pDoc->Status.bDoorAoi[DOOR_BR_AOI_UP] && !pDoc->Status.bDoorAoiF[DOOR_BR_AOI_UP])
//		{
//			ulOpenDoor |= (0x01 << 5);
//			pDoc->Status.bDoorAoiF[DOOR_BR_AOI_UP] = TRUE;
//			Buzzer(FALSE, 0);
//			Sleep(300);
//		}
//		else if (!pDoc->Status.bDoorAoi[DOOR_BR_AOI_UP] && pDoc->Status.bDoorAoiF[DOOR_BR_AOI_UP])
//		{
//			ulOpenDoor &= ~(0x01 << 5);
//			pDoc->Status.bDoorAoiF[DOOR_BR_AOI_UP] = FALSE;
//			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//			pView->m_mgrProcedure->m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("정 지"), RGB_RED);
//			MsgBox(_T("일시정지 - 검사부 상 후면 우측 도어 Open"));
//			TowerLamp(RGB_RED, TRUE);
//			Buzzer(TRUE, 0);
//		}
//	}
//
//	if (pDoc->WorkingInfo.LastJob.bAoiDnDrSen)
//	{
//		if (pDoc->Status.bDoorAoi[DOOR_FM_AOI_DN] && !pDoc->Status.bDoorAoiF[DOOR_FM_AOI_DN])
//		{
//			ulOpenDoor |= (0x01 << 0);
//			pDoc->Status.bDoorAoiF[DOOR_FM_AOI_DN] = TRUE;
//			Buzzer(FALSE, 0);
//			Sleep(300);
//		}
//		else if (!pDoc->Status.bDoorAoi[DOOR_FM_AOI_DN] && pDoc->Status.bDoorAoiF[DOOR_FM_AOI_DN])
//		{
//			ulOpenDoor &= ~(0x01 << 0);
//			pDoc->Status.bDoorAoiF[DOOR_FM_AOI_DN] = FALSE;
//			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//			pView->m_mgrProcedure->m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("정 지"), RGB_RED);
//			MsgBox(_T("일시정지 - 검사부 하 전면 중앙 도어 Open"));
//			TowerLamp(RGB_RED, TRUE);
//			Buzzer(TRUE, 0);
//		}
//
//		if (pDoc->Status.bDoorAoi[DOOR_FL_AOI_DN] && !pDoc->Status.bDoorAoiF[DOOR_FL_AOI_DN])
//		{
//			ulOpenDoor |= (0x01 << 1);
//			pDoc->Status.bDoorAoiF[DOOR_FL_AOI_DN] = TRUE;
//			Buzzer(FALSE, 0);
//			Sleep(300);
//		}
//		else if (!pDoc->Status.bDoorAoi[DOOR_FL_AOI_DN] && pDoc->Status.bDoorAoiF[DOOR_FL_AOI_DN])
//		{
//			ulOpenDoor &= ~(0x01 << 1);
//			pDoc->Status.bDoorAoiF[DOOR_FL_AOI_DN] = FALSE;
//			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//			pView->m_mgrProcedure->m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("정 지"), RGB_RED);
//			MsgBox(_T("일시정지 - 검사부 하 전면 좌측 도어 Open"));
//			TowerLamp(RGB_RED, TRUE);
//			Buzzer(TRUE, 0);
//		}
//
//		if (pDoc->Status.bDoorAoi[DOOR_FR_AOI_DN] && !pDoc->Status.bDoorAoiF[DOOR_FR_AOI_DN])
//		{
//			ulOpenDoor |= (0x01 << 2);
//			pDoc->Status.bDoorAoiF[DOOR_FR_AOI_DN] = TRUE;
//			Buzzer(FALSE, 0);
//			Sleep(300);
//		}
//		else if (!pDoc->Status.bDoorAoi[DOOR_FR_AOI_DN] && pDoc->Status.bDoorAoiF[DOOR_FR_AOI_DN])
//		{
//			ulOpenDoor &= ~(0x01 << 2);
//			pDoc->Status.bDoorAoiF[DOOR_FR_AOI_DN] = FALSE;
//			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//			pView->m_mgrProcedure->m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("정 지"), RGB_RED);
//			MsgBox(_T("일시정지 - 검사부 하 전면 우측 도어 Open"));
//			TowerLamp(RGB_RED, TRUE);
//			Buzzer(TRUE, 0);
//		}
//
//		if (pDoc->Status.bDoorAoi[DOOR_BM_AOI_DN] && !pDoc->Status.bDoorAoiF[DOOR_BM_AOI_DN])
//		{
//			ulOpenDoor |= (0x01 << 3);
//			pDoc->Status.bDoorAoiF[DOOR_BM_AOI_DN] = TRUE;
//			Buzzer(FALSE, 0);
//			Sleep(300);
//		}
//		else if (!pDoc->Status.bDoorAoi[DOOR_BM_AOI_DN] && pDoc->Status.bDoorAoiF[DOOR_BM_AOI_DN])
//		{
//			ulOpenDoor &= ~(0x01 << 3);
//			pDoc->Status.bDoorAoiF[DOOR_BM_AOI_DN] = FALSE;
//			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//			pView->m_mgrProcedure->m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("정 지"), RGB_RED);
//			MsgBox(_T("일시정지 - 검사부 하 후면 중앙 도어 Open"));
//			TowerLamp(RGB_RED, TRUE);
//			Buzzer(TRUE, 0);
//		}
//
//		if (pDoc->Status.bDoorAoi[DOOR_BL_AOI_DN] && !pDoc->Status.bDoorAoiF[DOOR_BL_AOI_DN])
//		{
//			ulOpenDoor |= (0x01 << 4);
//			pDoc->Status.bDoorAoiF[DOOR_BL_AOI_DN] = TRUE;
//			Buzzer(FALSE, 0);
//			Sleep(300);
//		}
//		else if (!pDoc->Status.bDoorAoi[DOOR_BL_AOI_DN] && pDoc->Status.bDoorAoiF[DOOR_BL_AOI_DN])
//		{
//			ulOpenDoor &= ~(0x01 << 4);
//			pDoc->Status.bDoorAoiF[DOOR_BL_AOI_DN] = FALSE;
//			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//			pView->m_mgrProcedure->m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("정 지"), RGB_RED);
//			MsgBox(_T("일시정지 - 검사부 하 후면 좌측 도어 Open"));
//			TowerLamp(RGB_RED, TRUE);
//			Buzzer(TRUE, 0);
//		}
//
//		if (pDoc->Status.bDoorAoi[DOOR_BR_AOI_DN] && !pDoc->Status.bDoorAoiF[DOOR_BR_AOI_DN])
//		{
//			ulOpenDoor |= (0x01 << 5);
//			pDoc->Status.bDoorAoiF[DOOR_BR_AOI_DN] = TRUE;
//			Buzzer(FALSE, 0);
//			Sleep(300);
//		}
//		else if (!pDoc->Status.bDoorAoi[DOOR_BR_AOI_DN] && pDoc->Status.bDoorAoiF[DOOR_BR_AOI_DN])
//		{
//			ulOpenDoor &= ~(0x01 << 5);
//			pDoc->Status.bDoorAoiF[DOOR_BR_AOI_DN] = FALSE;
//			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//			pView->m_mgrProcedure->m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("정 지"), RGB_RED);
//			MsgBox(_T("일시정지 - 검사부 하 후면 우측 도어 Open"));
//			TowerLamp(RGB_RED, TRUE);
//			Buzzer(TRUE, 0);
//		}
//	}
//
//	if (pDoc->WorkingInfo.LastJob.bMkDrSen)
//	{
//		if (pDoc->Status.bDoorMk[DOOR_FL_MK] && !pDoc->Status.bDoorMkF[DOOR_FL_MK])
//		{
//			ulOpenDoor |= (0x01 << 6);
//			pDoc->Status.bDoorMkF[DOOR_FL_MK] = TRUE;
//			Buzzer(FALSE, 0);
//			Sleep(300);
//		}
//		else if (!pDoc->Status.bDoorMk[DOOR_FL_MK] && pDoc->Status.bDoorMkF[DOOR_FL_MK])
//		{
//			ulOpenDoor &= ~(0x01 << 6);
//			pDoc->Status.bDoorMkF[DOOR_FL_MK] = FALSE;
//			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//			pView->m_mgrProcedure->m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("정 지"), RGB_RED);
//			MsgBox(_T("일시정지 - 마킹부 전면 좌측 도어 Open"));
//			TowerLamp(RGB_RED, TRUE);
//			Buzzer(TRUE, 0);
//		}
//
//		if (pDoc->Status.bDoorMk[DOOR_FR_MK] && !pDoc->Status.bDoorMkF[DOOR_FR_MK])
//		{
//			ulOpenDoor |= (0x01 << 7);
//			pDoc->Status.bDoorMkF[DOOR_FR_MK] = TRUE;
//			Buzzer(FALSE, 0);
//			Sleep(300);
//		}
//		else if (!pDoc->Status.bDoorMk[DOOR_FR_MK] && pDoc->Status.bDoorMkF[DOOR_FR_MK])
//		{
//			ulOpenDoor &= ~(0x01 << 7);
//			pDoc->Status.bDoorMkF[DOOR_FR_MK] = FALSE;
//			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//			pView->m_mgrProcedure->m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("정 지"), RGB_RED);
//			MsgBox(_T("일시정지 - 마킹부 전면 우측 도어 Open"));
//			TowerLamp(RGB_RED, TRUE);
//			Buzzer(TRUE, 0);
//		}
//
//		if (pDoc->Status.bDoorMk[DOOR_BL_MK] && !pDoc->Status.bDoorMkF[DOOR_BL_MK])
//		{
//			ulOpenDoor |= (0x01 << 8);
//			pDoc->Status.bDoorMkF[DOOR_BL_MK] = TRUE;
//			Buzzer(FALSE, 0);
//			Sleep(300);
//		}
//		else if (!pDoc->Status.bDoorMk[DOOR_BL_MK] && pDoc->Status.bDoorMkF[DOOR_BL_MK])
//		{
//			ulOpenDoor &= ~(0x01 << 8);
//			pDoc->Status.bDoorMkF[DOOR_BL_MK] = FALSE;
//			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//			pView->m_mgrProcedure->m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("정 지"), RGB_RED);
//			MsgBox(_T("일시정지 - 마킹부 후면 좌측 도어 Open"));
//			TowerLamp(RGB_RED, TRUE);
//			Buzzer(TRUE, 0);
//		}
//
//		if (pDoc->Status.bDoorMk[DOOR_BR_MK] && !pDoc->Status.bDoorMkF[DOOR_BR_MK])
//		{
//			ulOpenDoor |= (0x01 << 9);
//			pDoc->Status.bDoorMkF[DOOR_BR_MK] = TRUE;
//			Buzzer(FALSE, 0);
//			Sleep(300);
//		}
//		else if (!pDoc->Status.bDoorMk[DOOR_BR_MK] && pDoc->Status.bDoorMkF[DOOR_BR_MK])
//		{
//			ulOpenDoor &= ~(0x01 << 9);
//			pDoc->Status.bDoorMkF[DOOR_BR_MK] = FALSE;
//			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//			pView->m_mgrProcedure->m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("정 지"), RGB_RED);
//			MsgBox(_T("일시정지 - 마킹부 후면 우측 도어 Open"));
//			TowerLamp(RGB_RED, TRUE);
//			Buzzer(TRUE, 0);
//		}
//	}
//
//	if (pDoc->WorkingInfo.LastJob.bEngvDrSen)
//	{
//		if (pDoc->Status.bDoorEngv[DOOR_FL_ENGV] && !pDoc->Status.bDoorEngvF[DOOR_FL_ENGV])
//		{
//			ulOpenDoor |= (0x01 << 6);
//			pDoc->Status.bDoorEngvF[DOOR_FL_ENGV] = TRUE;
//			Buzzer(FALSE, 0);
//			Sleep(300);
//		}
//		else if (!pDoc->Status.bDoorEngv[DOOR_FL_ENGV] && pDoc->Status.bDoorEngvF[DOOR_FL_ENGV])
//		{
//			ulOpenDoor &= ~(0x01 << 6);
//			pDoc->Status.bDoorEngvF[DOOR_FL_ENGV] = FALSE;
//			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//			pView->m_mgrProcedure->m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("정 지"), RGB_RED);
//			MsgBox(_T("일시정지 - 각인부 전면 좌측 도어 Open"));
//			TowerLamp(RGB_RED, TRUE);
//			Buzzer(TRUE, 0);
//		}
//
//		if (pDoc->Status.bDoorEngv[DOOR_FR_ENGV] && !pDoc->Status.bDoorEngvF[DOOR_FR_ENGV])
//		{
//			ulOpenDoor |= (0x01 << 7);
//			pDoc->Status.bDoorEngvF[DOOR_FR_ENGV] = TRUE;
//			Buzzer(FALSE, 0);
//			Sleep(300);
//		}
//		else if (!pDoc->Status.bDoorEngv[DOOR_FR_ENGV] && pDoc->Status.bDoorEngvF[DOOR_FR_ENGV])
//		{
//			ulOpenDoor &= ~(0x01 << 7);
//			pDoc->Status.bDoorEngvF[DOOR_FR_ENGV] = FALSE;
//			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//			pView->m_mgrProcedure->m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("정 지"), RGB_RED);
//			MsgBox(_T("일시정지 - 각인부 전면 우측 도어 Open"));
//			TowerLamp(RGB_RED, TRUE);
//			Buzzer(TRUE, 0);
//		}
//
//		if (pDoc->Status.bDoorEngv[DOOR_BL_ENGV] && !pDoc->Status.bDoorEngvF[DOOR_BL_ENGV])
//		{
//			ulOpenDoor |= (0x01 << 8);
//			pDoc->Status.bDoorEngvF[DOOR_BL_ENGV] = TRUE;
//			Buzzer(FALSE, 0);
//			Sleep(300);
//		}
//		else if (!pDoc->Status.bDoorEngv[DOOR_BL_ENGV] && pDoc->Status.bDoorEngvF[DOOR_BL_ENGV])
//		{
//			ulOpenDoor &= ~(0x01 << 8);
//			pDoc->Status.bDoorEngvF[DOOR_BL_ENGV] = FALSE;
//			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//			pView->m_mgrProcedure->m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("정 지"), RGB_RED);
//			MsgBox(_T("일시정지 - 각인부 후면 좌측 도어 Open"));
//			TowerLamp(RGB_RED, TRUE);
//			Buzzer(TRUE, 0);
//		}
//
//		if (pDoc->Status.bDoorEngv[DOOR_BR_ENGV] && !pDoc->Status.bDoorEngvF[DOOR_BR_ENGV])
//		{
//			ulOpenDoor |= (0x01 << 9);
//			pDoc->Status.bDoorEngvF[DOOR_BR_ENGV] = TRUE;
//			Buzzer(FALSE, 0);
//			Sleep(300);
//		}
//		else if (!pDoc->Status.bDoorEngv[DOOR_BR_ENGV] && pDoc->Status.bDoorEngvF[DOOR_BR_ENGV])
//		{
//			ulOpenDoor &= ~(0x01 << 9);
//			pDoc->Status.bDoorEngvF[DOOR_BR_ENGV] = FALSE;
//			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//			pView->m_mgrProcedure->m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("정 지"), RGB_RED);
//			MsgBox(_T("일시정지 - 각인부 후면 우측 도어 Open"));
//			TowerLamp(RGB_RED, TRUE);
//			Buzzer(TRUE, 0);
//		}
//	}
//
//	if (pDoc->WorkingInfo.LastJob.bUclDrSen)
//	{
//		if (pDoc->Status.bDoorUc[DOOR_FL_UC] && !pDoc->Status.bDoorUcF[DOOR_FL_UC])
//		{
//			ulOpenDoor |= (0x01 << 10);
//			pDoc->Status.bDoorUcF[DOOR_FL_UC] = TRUE;
//			Buzzer(FALSE, 0);
//			Sleep(300);
//		}
//		else if (!pDoc->Status.bDoorUc[DOOR_FL_UC] && pDoc->Status.bDoorUcF[DOOR_FL_UC])
//		{
//			ulOpenDoor &= ~(0x01 << 10);
//			pDoc->Status.bDoorUcF[DOOR_FL_UC] = FALSE;
//			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//			pView->m_mgrProcedure->m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("정 지"), RGB_RED);
//			MsgBox(_T("일시정지 - 언코일러부 전면 좌측 도어 Open"));
//			TowerLamp(RGB_RED, TRUE);
//			Buzzer(TRUE, 0);
//		}
//
//		if (pDoc->Status.bDoorUc[DOOR_FR_UC] && !pDoc->Status.bDoorUcF[DOOR_FR_UC])
//		{
//			ulOpenDoor |= (0x01 << 11);
//			pDoc->Status.bDoorUcF[DOOR_FR_UC] = TRUE;
//			Buzzer(FALSE, 0);
//			Sleep(300);
//		}
//		else if (!pDoc->Status.bDoorUc[DOOR_FR_UC] && pDoc->Status.bDoorUcF[DOOR_FR_UC])
//		{
//			ulOpenDoor &= ~(0x01 << 11);
//			pDoc->Status.bDoorUcF[DOOR_FR_UC] = FALSE;
//			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//			pView->m_mgrProcedure->m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("정 지"), RGB_RED);
//			MsgBox(_T("일시정지 - 언코일러부 측면 도어 Open"));
//			TowerLamp(RGB_RED, TRUE);
//			Buzzer(TRUE, 0);
//		}
//
//		if (pDoc->Status.bDoorUc[DOOR_BL_UC] && !pDoc->Status.bDoorUcF[DOOR_BL_UC])
//		{
//			ulOpenDoor |= (0x01 << 12);
//			pDoc->Status.bDoorUcF[DOOR_BL_UC] = TRUE;
//			Buzzer(FALSE, 0);
//			Sleep(300);
//		}
//		else if (!pDoc->Status.bDoorUc[DOOR_BL_UC] && pDoc->Status.bDoorUcF[DOOR_BL_UC])
//		{
//			ulOpenDoor &= ~(0x01 << 12);
//			pDoc->Status.bDoorUcF[DOOR_BL_UC] = FALSE;
//			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//			pView->m_mgrProcedure->m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("정 지"), RGB_RED);
//			MsgBox(_T("일시정지 - 언코일러부 후면 좌측 도어 Open"));
//			TowerLamp(RGB_RED, TRUE);
//			Buzzer(TRUE, 0);
//		}
//
//		if (pDoc->Status.bDoorUc[DOOR_BR_UC] && !pDoc->Status.bDoorUcF[DOOR_BR_UC])
//		{
//			ulOpenDoor |= (0x01 << 13);
//			pDoc->Status.bDoorUcF[DOOR_BR_UC] = TRUE;
//			Buzzer(FALSE, 0);
//			Sleep(300);
//		}
//		else if (!pDoc->Status.bDoorUc[DOOR_BR_UC] && pDoc->Status.bDoorUcF[DOOR_BR_UC])
//		{
//			ulOpenDoor &= ~(0x01 << 13);
//			pDoc->Status.bDoorUcF[DOOR_BR_UC] = FALSE;
//			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//			pView->m_mgrProcedure->m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("정 지"), RGB_RED);
//			MsgBox(_T("일시정지 - 언코일러부 후면 우측 도어 Open"));
//			TowerLamp(RGB_RED, TRUE);
//			Buzzer(TRUE, 0);
//		}
//	}
//
//	if (pDoc->WorkingInfo.LastJob.bRclDrSen)
//	{
//		if (pDoc->Status.bDoorRe[DOOR_FR_RC] && !pDoc->Status.bDoorReF[DOOR_FR_RC])
//		{
//			ulOpenDoor |= (0x01 << 15);
//			pDoc->Status.bDoorReF[DOOR_FR_RC] = TRUE;
//			Buzzer(FALSE, 0);
//			Sleep(300);
//		}
//		else if (!pDoc->Status.bDoorRe[DOOR_FR_RC] && pDoc->Status.bDoorReF[DOOR_FR_RC])
//		{
//			ulOpenDoor &= ~(0x01 << 15);
//			pDoc->Status.bDoorReF[DOOR_FR_RC] = FALSE;
//			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//			pView->m_mgrProcedure->m_bSwRunF = FALSE;
//			Stop();
//			pView->DispStsBar(_T("정지-19"), 0);
//			DispMain(_T("정 지"), RGB_RED);
//			MsgBox(_T("일시정지 - 리코일러부 전면 우측 도어 Open"));
//			TowerLamp(RGB_RED, TRUE);
//			Buzzer(TRUE, 0);
//		}
//
//		if (pDoc->Status.bDoorRe[DOOR_S_RC] && !pDoc->Status.bDoorReF[DOOR_S_RC])
//		{
//			ulOpenDoor |= (0x01 << 16);
//			pDoc->Status.bDoorReF[DOOR_S_RC] = TRUE;
//			Buzzer(FALSE, 0);
//			Sleep(300);
//		}
//		else if (!pDoc->Status.bDoorRe[DOOR_S_RC] && pDoc->Status.bDoorReF[DOOR_S_RC])
//		{
//			ulOpenDoor &= ~(0x01 << 16);
//			pDoc->Status.bDoorReF[DOOR_S_RC] = FALSE;
//			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//			pView->m_mgrProcedure->m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("정 지"), RGB_RED);
//			MsgBox(_T("일시정지 - 리코일러부 측면 도어 Open"));
//			TowerLamp(RGB_RED, TRUE);
//			Buzzer(TRUE, 0);
//		}
//
//		if (pDoc->Status.bDoorRe[DOOR_BL_RC] && !pDoc->Status.bDoorReF[DOOR_BL_RC])
//		{
//			ulOpenDoor |= (0x01 << 17);
//			pDoc->Status.bDoorReF[DOOR_BL_RC] = TRUE;
//			Buzzer(FALSE, 0);
//			Sleep(300);
//		}
//		else if (!pDoc->Status.bDoorRe[DOOR_BL_RC] && pDoc->Status.bDoorReF[DOOR_BL_RC])
//		{
//			ulOpenDoor &= ~(0x01 << 17);
//			pDoc->Status.bDoorReF[DOOR_BL_RC] = FALSE;
//			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//			pView->m_mgrProcedure->m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("정 지"), RGB_RED);
//			MsgBox(_T("일시정지 - 리코일러부 후면 좌측 도어 Open"));
//			TowerLamp(RGB_RED, TRUE);
//			Buzzer(TRUE, 0);
//		}
//
//		if (pDoc->Status.bDoorRe[DOOR_BR_RC] && !pDoc->Status.bDoorReF[DOOR_BR_RC])
//		{
//			ulOpenDoor |= (0x01 << 18);
//			pDoc->Status.bDoorReF[DOOR_BR_RC] = TRUE;
//			Buzzer(FALSE, 0);
//			Sleep(300);
//		}
//		else if (!pDoc->Status.bDoorRe[DOOR_BR_RC] && pDoc->Status.bDoorReF[DOOR_BR_RC])
//		{
//			ulOpenDoor &= ~(0x01 << 18);
//			pDoc->Status.bDoorReF[DOOR_BR_RC] = FALSE;
//			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//			pView->m_mgrProcedure->m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("정 지"), RGB_RED);
//			MsgBox(_T("일시정지 - 리코일러부 후면 우측 도어 Open"));
//			TowerLamp(RGB_RED, TRUE);
//			Buzzer(TRUE, 0);
//		}
//	}
//
//	return ulOpenDoor;
//}

//void CManagerProcedure::ChkEmg()
//{
//	if (pDoc->Status.bEmgAoi[EMG_F_AOI_UP] && !pDoc->Status.bEmgAoiF[EMG_F_AOI_UP])
//	{
//		pDoc->Status.bEmgAoiF[EMG_F_AOI_UP] = TRUE;
//		pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//		pView->m_mgrProcedure->m_bSwRunF = FALSE;
//		Stop();
//		DispMain(_T("정 지"), RGB_RED);
//		MsgBox(_T("비상정지 - 검사부 상 전면 스위치"));
//		TowerLamp(RGB_RED, TRUE);
//		Buzzer(TRUE, 0);
//	}
//	else if (!pDoc->Status.bEmgAoi[EMG_F_AOI_UP] && pDoc->Status.bEmgAoiF[EMG_F_AOI_UP])
//	{
//		pDoc->Status.bEmgAoiF[EMG_F_AOI_UP] = FALSE;
//		Buzzer(FALSE, 0);
//		Sleep(300);
//		ResetMotion();
//	}
//
//	if (pDoc->Status.bEmgAoi[EMG_B_AOI_UP] && !pDoc->Status.bEmgAoiF[EMG_B_AOI_UP])
//	{
//		pDoc->Status.bEmgAoiF[EMG_B_AOI_UP] = TRUE;
//		pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//		pView->m_mgrProcedure->m_bSwRunF = FALSE;
//		Stop();
//		DispMain(_T("정 지"), RGB_RED);
//		MsgBox(_T("비상정지 - 검사부 상 후면 스위치"));
//		TowerLamp(RGB_RED, TRUE);
//		Buzzer(TRUE, 0);
//	}
//	else if (!pDoc->Status.bEmgAoi[EMG_B_AOI_UP] && pDoc->Status.bEmgAoiF[EMG_B_AOI_UP])
//	{
//		pDoc->Status.bEmgAoiF[EMG_B_AOI_UP] = FALSE;
//		Buzzer(FALSE, 0);
//		Sleep(300);
//		ResetMotion();
//	}
//
//	if (pDoc->Status.bEmgAoi[EMG_F_AOI_DN] && !pDoc->Status.bEmgAoiF[EMG_F_AOI_DN])
//	{
//		pDoc->Status.bEmgAoiF[EMG_F_AOI_DN] = TRUE;
//		pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//		pView->m_mgrProcedure->m_bSwRunF = FALSE;
//		Stop();
//		DispMain(_T("정 지"), RGB_RED);
//		MsgBox(_T("비상정지 - 검사부 하 전면 스위치"));
//		TowerLamp(RGB_RED, TRUE);
//		Buzzer(TRUE, 0);
//	}
//	else if (!pDoc->Status.bEmgAoi[EMG_F_AOI_DN] && pDoc->Status.bEmgAoiF[EMG_F_AOI_DN])
//	{
//		pDoc->Status.bEmgAoiF[EMG_F_AOI_DN] = FALSE;
//		Buzzer(FALSE, 0);
//		Sleep(300);
//		ResetMotion();
//	}
//
//	if (pDoc->Status.bEmgAoi[EMG_B_AOI_DN] && !pDoc->Status.bEmgAoiF[EMG_B_AOI_DN])
//	{
//		pDoc->Status.bEmgAoiF[EMG_B_AOI_DN] = TRUE;
//		pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//		pView->m_mgrProcedure->m_bSwRunF = FALSE;
//		Stop();
//		DispMain(_T("정 지"), RGB_RED);
//		MsgBox(_T("비상정지 - 검사부 하 후면 스위치"));
//		TowerLamp(RGB_RED, TRUE);
//		Buzzer(TRUE, 0);
//	}
//	else if (!pDoc->Status.bEmgAoi[EMG_B_AOI_DN] && pDoc->Status.bEmgAoiF[EMG_B_AOI_DN])
//	{
//		pDoc->Status.bEmgAoiF[EMG_B_AOI_DN] = FALSE;
//		Buzzer(FALSE, 0);
//		Sleep(300);
//		ResetMotion();
//	}
//
//	if (pDoc->Status.bEmgMk[EMG_M_MK] && !pDoc->Status.bEmgMkF[EMG_M_MK])
//	{
//		pDoc->Status.bEmgMkF[EMG_M_MK] = TRUE;
//		pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//		pView->m_mgrProcedure->m_bSwRunF = FALSE;
//		Stop();
//		DispMain(_T("정 지"), RGB_RED);
//		MsgBox(_T("비상정지 - 마킹부 메인 스위치"));
//		TowerLamp(RGB_RED, TRUE);
//		Buzzer(TRUE, 0);
//	}
//	else if (!pDoc->Status.bEmgMk[EMG_M_MK] && pDoc->Status.bEmgMkF[EMG_M_MK])
//	{
//		pDoc->Status.bEmgMkF[EMG_M_MK] = FALSE;
//		Buzzer(FALSE, 0);
//		Sleep(300);
//		ResetMotion();
//	}
//
//	if (pDoc->Status.bEmgMk[EMG_B_MK] && !pDoc->Status.bEmgMkF[EMG_B_MK])
//	{
//		pDoc->Status.bEmgMkF[EMG_B_MK] = TRUE;
//		pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//		pView->m_mgrProcedure->m_bSwRunF = FALSE;
//		Stop();
//		DispMain(_T("정 지"), RGB_RED);
//		MsgBox(_T("비상정지 - 마킹부 스위치"));
//		TowerLamp(RGB_RED, TRUE);
//		Buzzer(TRUE, 0);
//	}
//	else if (!pDoc->Status.bEmgMk[EMG_B_MK] && pDoc->Status.bEmgMkF[EMG_B_MK])
//	{
//		pDoc->Status.bEmgMkF[EMG_B_MK] = FALSE;
//		Buzzer(FALSE, 0);
//		Sleep(300);
//		ResetMotion();
//	}
//
//	if (pDoc->Status.bEmgUc && !pDoc->Status.bEmgUcF)
//	{
//		pDoc->Status.bEmgUcF = TRUE;
//		pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//		pView->m_mgrProcedure->m_bSwRunF = FALSE;
//		Stop();
//		DispMain(_T("정 지"), RGB_RED);
//		MsgBox(_T("비상정지 - 언코일러부 스위치"));
//		TowerLamp(RGB_RED, TRUE);
//		Buzzer(TRUE, 0);
//	}
//	else if (!pDoc->Status.bEmgUc && pDoc->Status.bEmgUcF)
//	{
//		pDoc->Status.bEmgUcF = FALSE;
//		Buzzer(FALSE, 0);
//		Sleep(300);
//		ResetMotion();
//	}
//
//	if (pDoc->Status.bEmgRc && !pDoc->Status.bEmgRcF)
//	{
//		pDoc->Status.bEmgRcF = TRUE;
//		pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//		pView->m_mgrProcedure->m_bSwRunF = FALSE;
//		Stop();
//		DispMain(_T("정 지"), RGB_RED);
//		MsgBox(_T("비상정지 - 리코일러부 스위치"));
//		TowerLamp(RGB_RED, TRUE);
//		Buzzer(TRUE, 0);
//	}
//	else if (!pDoc->Status.bEmgRc && pDoc->Status.bEmgRcF)
//	{
//		pDoc->Status.bEmgRcF = FALSE;
//		Buzzer(FALSE, 0);
//		Sleep(300);
//		ResetMotion();
//	}
//
//	if (pDoc->Status.bEmgEngv[0] && !pDoc->Status.bEmgEngvF[0])
//	{
//		pDoc->Status.bEmgEngvF[0] = TRUE;
//		pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//		pView->m_mgrProcedure->m_bSwRunF = FALSE;
//		Stop();
//		DispMain(_T("정 지"), RGB_RED);
//		MsgBox(_T("비상정지 - 각인부 모니터"));
//		TowerLamp(RGB_RED, TRUE);
//		Buzzer(TRUE, 0);
//	}
//	else if (!pDoc->Status.bEmgEngv[0] && pDoc->Status.bEmgEngvF[0])
//	{
//		pDoc->Status.bEmgEngvF[0] = FALSE;
//		Buzzer(FALSE, 0);
//		Sleep(300);
//		ResetMotion();
//	}
//
//	if (pDoc->Status.bEmgEngv[1] && !pDoc->Status.bEmgEngvF[1])
//	{
//		pDoc->Status.bEmgEngvF[1] = TRUE;
//		pView->m_mgrProcedure->m_bSwStopNow = TRUE;
//		pView->m_mgrProcedure->m_bSwRunF = FALSE;
//		Stop();
//		DispMain(_T("정 지"), RGB_RED);
//		MsgBox(_T("비상정지 - 각인부 스위치"));
//		TowerLamp(RGB_RED, TRUE);
//		Buzzer(TRUE, 0);
//	}
//	else if (!pDoc->Status.bEmgEngv[1] && pDoc->Status.bEmgEngvF[1])
//	{
//		pDoc->Status.bEmgEngvF[1] = FALSE;
//		Buzzer(FALSE, 0);
//		Sleep(300);
//		ResetMotion();
//	}
//}

//int CManagerProcedure::ChkSerial() // // 0: Continue, -: Previous, +: Discontinue --> 0: Same Serial, -: Decrese Serial, +: Increase Serial
//{
//	int nSerial0 = GetBuffer0(); // 첫번째 버퍼 시리얼 : 상하 시리얼이 다르면 0
//	int nSerial1 = GetBuffer1(); // 두번째 버퍼 시리얼 : 상하 시리얼이 다르면 0
//	int nLastShot = pDoc->GetLastShotMk();
//
//	// Last shot 다음 연속 시리얼 체크
//	//if (nSerial0 == nLastShot + 1 || nSerial1 == nLastShot + 2)
//	//	return 0;
//
//	//if (pDoc->WorkingInfo.LastJob.bLotSep || pDoc->m_bDoneChgLot)
//	//{
//	//	if (nLastShot == pDoc->m_nLotLastShot && (nSerial0 == 1 || nSerial1 == 2))
//	//		return 0;
//	//}
//	//if (m_bLastProc && nSerial0 == m_nLotEndSerial)
//	//	return 0;
//
//	return (nSerial0 - nLastShot);
//}

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
	if (ChkBufUp(pView->m_mgrProcedure->m_pBufSerial[0], pView->m_mgrProcedure->m_nBufTot[0]))
	{
		for (int i = 0; i < pView->m_mgrProcedure->m_nBufTot[0]; i++)
		{
			if (m_bShift2Mk)
				return;

			DelOverLotEndSerialUp(pView->m_mgrProcedure->m_pBufSerial[0][i]);

			if (i == m_nBufTot[0] - 1)
				sTemp.Format(_T("%d"), m_pBufSerial[0][i]);
			else
				sTemp.Format(_T("%d,"), m_pBufSerial[0][i]);
			str += sTemp;
		}
	}
	else
	{
		pView->m_mgrProcedure->m_nBufTot[0] = 0;
	}

	if (pFrm)
	{
		if (pView->m_mgrProcedure->m_sBuf[0] != str)
		{
			m_sBuf[0] = str;
			pFrm->DispStatusBar(str, 3);

			pDoc->SetCurrentInfoBufUpTot(pView->m_mgrProcedure->m_nBufTot[0]);
			for (int k = 0; k < m_nBufTot[0]; k++)
				pDoc->SetCurrentInfoBufUp(k, pView->m_mgrProcedure->m_pBufSerial[0][k]);
		}
	}
}

void CManagerProcedure::ChkBufDn()
{
	CString str, sTemp;

	str = _T("DB: ");
	if (ChkBufDn(pView->m_mgrProcedure->m_pBufSerial[1], pView->m_mgrProcedure->m_nBufTot[1]))
	{
		for (int i = 0; i < pView->m_mgrProcedure->m_nBufTot[1]; i++)
		{
			if (m_bShift2Mk)
				return;

			DelOverLotEndSerialDn(pView->m_mgrProcedure->m_pBufSerial[1][i]);

			if (i == m_nBufTot[1] - 1)
				sTemp.Format(_T("%d"), m_pBufSerial[1][i]);
			else
				sTemp.Format(_T("%d,"), m_pBufSerial[1][i]);
			str += sTemp;
		}
	}
	else
	{
		pView->m_mgrProcedure->m_nBufTot[1] = 0;
	}

	if (pFrm)
	{
		if (pView->m_mgrProcedure->m_sBuf[1] != str)
		{
			m_sBuf[1] = str;
			pFrm->DispStatusBar(str, 1);

			pDoc->SetCurrentInfoBufDnTot(pView->m_mgrProcedure->m_nBufTot[1]);
			for (int k = 0; k < m_nBufTot[1]; k++)
				pDoc->SetCurrentInfoBufDn(k, pView->m_mgrProcedure->m_pBufSerial[1][k]);
		}
	}
}

//int CManagerProcedure::GetAoiUpAutoStep()
//{
//	TCHAR szData[512];
//	CString sPath = pDoc->WorkingInfo.System.sPathAoiUpStatusInfo;
//	if (0 < ::GetPrivateProfileString(_T("Auto"), _T("nStep"), NULL, szData, sizeof(szData), sPath))
//		return _ttoi(szData);
//
//	return (-1);
//}
//
//void CManagerProcedure::SetAoiUpAutoStep(int nStep)
//{
//	CString sPath = pDoc->WorkingInfo.System.sPathAoiUpStatusInfo;
//	CString str;
//	str.Format(_T("%d"), nStep);
//	::WritePrivateProfileString(_T("Set"), _T("nStep"), str, sPath);
//}
//
//int CManagerProcedure::GetAoiDnAutoStep()
//{
//	TCHAR szData[512];
//	CString sPath = pDoc->WorkingInfo.System.sPathAoiDnStatusInfo;
//	if (0 < ::GetPrivateProfileString(_T("Auto"), _T("nStep"), NULL, szData, sizeof(szData), sPath))
//		return _ttoi(szData);
//
//	return (-1);
//}
//
//void CManagerProcedure::SetAoiDnAutoStep(int nStep)
//{
//	CString sPath = pDoc->WorkingInfo.System.sPathAoiDnStatusInfo;
//	CString str;
//	str.Format(_T("%d"), nStep);
//	::WritePrivateProfileString(_T("Set"), _T("nStep"), str, sPath);
//}
//
//CString CManagerProcedure::GetAoiUpAlarmRestartMsg()
//{
//	CString sMsg = _T("Not Find Message.");
//	TCHAR szData[512];
//	CString sPath = PATH_ALARM;
//	if (0 < ::GetPrivateProfileString(_T("11"), _T("27"), NULL, szData, sizeof(szData), sPath))
//		sMsg = CString(szData);
//
//	return sMsg;
//}
//
//CString CManagerProcedure::GetAoiDnAlarmRestartMsg()
//{
//	CString sMsg = _T("Not Find Message.");
//	TCHAR szData[512];
//	CString sPath = PATH_ALARM;
//	if (0 < ::GetPrivateProfileString(_T("11"), _T("28"), NULL, szData, sizeof(szData), sPath))
//		sMsg = CString(szData);
//
//	return sMsg;
//}
//
//void CManagerProcedure::DoIO()
//{
//	DoEmgSens();	//20220607
//	DoSaftySens();	//20220603
//	DoDoorSens();	//20220607
//
//	DoModeSel();
//	DoMainSw();
//	DoEngraveSens();
//
//	DoInterlock();
//
//	MonPlcAlm();
//	MonDispMain();
//	MonPlcSignal();
//
//	if (pView->m_mgrProcedure->m_bCycleStop)
//	{
//		m_bCycleStop = FALSE;
//		TowerLamp(RGB_YELLOW, TRUE);
//		Buzzer(TRUE);
//		if (!pDoc->m_sAlmMsg.IsEmpty())
//		{
//			MsgBox(pDoc->m_sAlmMsg, 0, 0, DEFAULT_TIME_OUT, FALSE);
//
//			if (pDoc->m_sAlmMsg == GetAoiUpAlarmRestartMsg())
//			{
//				ChkReTestAlarmOnAoiUp();
//			}
//			else if (pDoc->m_sAlmMsg == GetAoiDnAlarmRestartMsg())
//			{
//				ChkReTestAlarmOnAoiDn();
//			}
//		}
//		pDoc->m_sAlmMsg = _T("");
//		pDoc->m_sIsAlmMsg = _T("");
//		pDoc->m_sPrevAlmMsg = _T("");
//	}
//
//	if (pDoc->Status.bManual)
//	{
//		DoBoxSw();
//	}
//	else if (pDoc->Status.bAuto)
//	{
//		DoAutoEng();
//		DoAuto();
//	}
//
//	if (IsRun())
//	{
//		if (m_pDlgMenu01)
//		{
//			if (m_pDlgMenu01->IsEnableBtn())
//				m_pDlgMenu01->EnableBtn(FALSE);
//		}
//	}
//	else
//	{
//		if (m_pDlgMenu01)
//		{
//			if (!m_pDlgMenu01->IsEnableBtn())
//				m_pDlgMenu01->EnableBtn(TRUE);
//		}
//	}
//
//}
//
//void CManagerProcedure::GetEnc()
//{
//	if (!m_pMotion)
//		return;
//
//	m_dEnc[AXIS_X0] = m_pMotion->GetActualPosition(AXIS_X0);
//	m_dEnc[AXIS_Y0] = m_pMotion->GetActualPosition(AXIS_Y0);
//	m_dEnc[AXIS_X1] = m_pMotion->GetActualPosition(AXIS_X1);
//	m_dEnc[AXIS_Y1] = m_pMotion->GetActualPosition(AXIS_Y1);
//}
//
//void CManagerProcedure::ChkMRegOut()
//{
//	pView->m_mgrProcedure->m_bChkMpeIoOut = TRUE;
//}

void CManagerProcedure::ChkShare()
{
	ChkShareUp();
	ChkShareDn();
}

void CManagerProcedure::ChkShareUp()
{
	CString str;
	int nSerial;
	if (ChkShareUp(nSerial))
	{
		str.Format(_T("US: %d"), nSerial);
		pDoc->Status.PcrShare[0].bExist = TRUE;
		pDoc->Status.PcrShare[0].nSerial = nSerial;
		if (pView->m_pMpe)
		{
			pView->m_pMpe->Write(_T("ML45112"), (long)nSerial);	// 검사한 Panel의 AOI 상 Serial
			pView->m_pMpe->Write(_T("MB44012B"), 1); // AOI 상 : PCR파일 Received
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
		if (pView->m_mgrProcedure->m_sShare[0] != str)
		{
			pView->m_mgrProcedure->m_sShare[0] = str;
			pFrm->DispStatusBar(str, 4);
		}
	}
}

void CManagerProcedure::ChkShareDn()
{
	CString str;
	int nSerial;
	if (ChkShareDn(nSerial))
	{
		str.Format(_T("DS: %d"), nSerial);
		pDoc->Status.PcrShare[1].bExist = TRUE;
		pDoc->Status.PcrShare[1].nSerial = nSerial;
		if (pView->m_pMpe)
		{
			pView->m_pMpe->Write(_T("ML45114"), (long)nSerial);	// 검사한 Panel의 AOI 하 Serial
			pView->m_pMpe->Write(_T("MB44012C"), 1); // AOI 하 : PCR파일 Received
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
		if (pView->m_mgrProcedure->m_sShare[1] != str)
		{
			pView->m_mgrProcedure->m_sShare[1] = str;
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

void CManagerProcedure::SwapUp(__int64 *num1, __int64 *num2) 	// 위치 바꾸는 함수
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
		sMsg.Format(_T("일시정지 - Failed getting information."));
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

		pView->m_mgrProcedure->m_nBufSerialSorting[0][nIndex] = nYear * 100000000000000 + nMonth * 1000000000000 + nDay * 10000000000 +
			nHour * 100000000 + nMin * 1000000 + nSec * 10000 + nSerial;
	}

	return TRUE;
}

BOOL CManagerProcedure::SortingOutUp(int* pSerial, int nTot)
{
	int i, k;

	for (k = 0; k < nTot; k++) 			// 버블 정렬 소스 시작
	{
		for (i = 0; i < (nTot - 1) - k; i++)
		{

			if (pView->m_mgrProcedure->m_nBufSerialSorting[0][i] > pView->m_mgrProcedure->m_nBufSerialSorting[0][i + 1])
			{
				SwapUp(&pView->m_mgrProcedure->m_nBufSerialSorting[0][i + 1], &pView->m_mgrProcedure->m_nBufSerialSorting[0][i]);
			}
		}
	}									// 버블 정렬 소스 끝

	for (i = 0; i < nTot; i++)
	{
		pSerial[i] = (int)(pView->m_mgrProcedure->m_nBufSerialSorting[0][i] % 10000);
	}
	return TRUE;
}

BOOL CManagerProcedure::ChkBufUp(int* pSerial, int &nTot)
{
	CFileFind cFile;
	BOOL bExist = cFile.FindFile(pDoc->WorkingInfo.System.sPathVrsBufUp + _T("*.pcr"));
	if (!bExist)
	{
		pView->m_mgrProcedure->m_bBufEmpty[0] = TRUE;
		if (!pView->m_mgrProcedure->m_bBufEmptyF[0])
			pView->m_mgrProcedure->m_bBufEmptyF[0] = TRUE;		// 최초 한번 버퍼가 비어있으면(초기화를 하고 난 이후) TRUE.

		return FALSE; // pcr파일이 존재하지 않음.
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
		pView->m_mgrProcedure->m_bBufEmpty[0] = TRUE;
	else
	{
		pView->m_mgrProcedure->m_bBufEmpty[0] = FALSE;
		pView->m_mgrProcedure->m_bIsBuf[0] = TRUE;
	}

	return (bRtn);
}

BOOL CManagerProcedure::ChkBufDn(int* pSerial, int &nTot)
{
	CFileFind cFile;
	BOOL bExist = cFile.FindFile(pDoc->WorkingInfo.System.sPathVrsBufDn + _T("*.pcr"));
	if (!bExist)
	{
		pView->m_mgrProcedure->m_bBufEmpty[1] = TRUE;
		if (!pView->m_mgrProcedure->m_bBufEmptyF[1])
			pView->m_mgrProcedure->m_bBufEmptyF[1] = TRUE;
		return FALSE; // pcr파일이 존재하지 않음.
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
			//nPos = sFileName.ReverseFind('.');
			//if (nPos > 0)
			//	sSerial = sFileName.Left(nPos);

			//nSerial = _tstoi(sSerial);
			//if (nSerial > 0)
			//{
			//	pSerial[nTot] = nSerial;
			//	nTot++;
			//}

			if (!SortingInDn(pDoc->WorkingInfo.System.sPathVrsBufDn + sFileName, nTot))
				return FALSE;

			nTot++;
		}
	}


	BOOL bRtn = SortingOutDn(pSerial, nTot);

	if (nTot == 0)
		pView->m_mgrProcedure->m_bBufEmpty[1] = TRUE;
	else
	{
		pView->m_mgrProcedure->m_bBufEmpty[1] = FALSE;
		pView->m_mgrProcedure->m_bIsBuf[1] = TRUE;
	}

	return (bRtn);
	//return TRUE;
}

void CManagerProcedure::SwapDn(__int64 *num1, __int64 *num2) 	// 위치 바꾸는 함수
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
		sMsg.Format(_T("일시정지 - Failed getting information."));
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

		pView->m_mgrProcedure->m_nBufSerialSorting[1][nIndex] = nYear * 100000000000000 + nMonth * 1000000000000 + nDay * 10000000000 +
			nHour * 100000000 + nMin * 1000000 + nSec * 10000 + nSerial;
	}

	return TRUE;
}

BOOL CManagerProcedure::SortingOutDn(int* pSerial, int nTot)
{
	int i, k;

	for (k = 0; k < nTot; k++) 			// 버블 정렬 소스 시작
	{
		for (i = 0; i < (nTot - 1) - k; i++)
		{

			if (pView->m_mgrProcedure->m_nBufSerialSorting[1][i] > pView->m_mgrProcedure->m_nBufSerialSorting[1][i + 1])
			{
				SwapUp(&m_nBufSerialSorting[1][i + 1], &m_nBufSerialSorting[1][i]);
			}
		}
	}									// 버블 정렬 소스 끝

	for (i = 0; i < nTot; i++)
	{
		pSerial[i] = (int)(pView->m_mgrProcedure->m_nBufSerialSorting[1][i] % 10000);
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
		return FALSE; // pcr파일이 존재하지 않음.

	int nPos;
	CString sFileName, sSerial;
	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (!cFile.IsDirectory())
		{
			// 파일명을 얻음.
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
		return FALSE; // pcr파일이 존재하지 않음.

	int nPos;
	CString sFileName, sSerial;
	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (!cFile.IsDirectory())
		{
			// 파일명을 얻음.
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
		return FALSE; // pcr파일이 존재하지 않음.

	int nPos;
	CString sFileName, sSerial;
	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (!cFile.IsDirectory())
		{
			// 파일명을 얻음.
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
		return FALSE; // pcr파일이 존재하지 않음.

	int nPos;
	CString sFileName, sSerial;
	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (!cFile.IsDirectory())
		{
			// 파일명을 얻음.
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


void CManagerProcedure::ResetPriority()
{
	m_bPriority[0] = FALSE;
	m_bPriority[1] = FALSE;
	m_bPriority[2] = FALSE;
	m_bPriority[3] = FALSE;
}

void CManagerProcedure::SetPriority()
{
	int nDir[2];
	nDir[0] = (m_dTarget[AXIS_X0] - m_dEnc[AXIS_X0]) >= 0.0 ? 1 : -1;
	nDir[1] = (m_dTarget[AXIS_X1] - m_dEnc[AXIS_X1]) >= 0.0 ? 1 : -1;

	// Cam0 : m_bPriority[0], m_bPriority[3]
	// Cam1 : m_bPriority[1], m_bPriority[2]
	if (nDir[1] > 0) // Cam1 ->
	{
		m_bPriority[0] = FALSE;
		m_bPriority[1] = TRUE;
		m_bPriority[2] = FALSE;
		m_bPriority[3] = FALSE;
	}
	else if (nDir[0] < 0) // Cam0 <-
	{
		m_bPriority[0] = TRUE;
		m_bPriority[1] = FALSE;
		m_bPriority[2] = FALSE;
		m_bPriority[3] = FALSE;
	}
	else if (nDir[1] < 0) // Cam1 <-
	{
		m_bPriority[0] = FALSE;
		m_bPriority[1] = FALSE;
		m_bPriority[2] = TRUE;
		m_bPriority[3] = FALSE;
	}
	else // Cam0 ->
	{
		m_bPriority[0] = FALSE;
		m_bPriority[1] = FALSE;
		m_bPriority[2] = FALSE;
		m_bPriority[3] = TRUE;
	}
}

void CManagerProcedure::DelOverLotEndSerialUp(int nSerial)
{
	CString sSrc;

	if (nSerial > 0)
	{
		sSrc.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufUp, nSerial);

		if (pView->m_mgrProcedure->m_bSerialDecrese)
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

		if (pView->m_mgrProcedure->m_bSerialDecrese)
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
