#pragma once

#include "Global/GlobalDefine.h"

// CManagerProcedure

class CManagerProcedure : public CWnd
{
	DECLARE_DYNAMIC(CManagerProcedure)

public:
	CManagerProcedure();
	virtual ~CManagerProcedure();

	void InitVal();

	int m_nAoiCamInfoStrPcs[2]; // [0] : Up, [1] : Dn
	//BOOL m_bCamChged;
	BOOL m_bTIM_CHK_DONE_READY;


	BOOL m_bBufEmpty[2];	// [0]: Up, [1]: Dn
	BOOL m_bBufEmptyF[2];	// [0]: Up, [1]: Dn
	BOOL m_bChkBufIdx[2];	// [0]: Up, [1]: Dn
	int m_nChkBufIdx[2];	// [0]: Up, [1]: Dn

	int m_nShareUpS, m_nShareUpSprev;
	int m_nShareUpSerial[2]; // [nCam]
	int m_nShareDnS, m_nShareDnSprev;
	int m_nShareDnSerial[2]; // [nCam]
	int m_nShareUpCnt;
	int m_nShareDnCnt;

	int m_nBufUpSerial[2];	// [nCam]
	int m_nBufDnSerial[2];	// [nCam]
	int m_nBufUpCnt;
	int m_nBufDnCnt;
	BOOL m_bIsBuf[2];		// [0]: AOI-Up , [1]: AOI-Dn

	CString m_sShare[2], m_sBuf[2]; // [0]: AOI-Up , [1]: AOI-Dn
	int		m_pBufSerial[2][100], m_nBufTot[2]; // [0]: AOI-Up , [1]: AOI-Dn
	__int64 m_nBufSerialSorting[2][100]; // [0]: AOI-Up , [1]: AOI-Dn
	int		m_nBufSerial[2][2]; // [0]: AOI-Up , [1]: AOI-Dn // [0]: Cam0, [1]:Cam1

	BOOL m_bTHREAD_UPDATE_REELMAP_INNER_UP, m_bTHREAD_UPDATE_REELMAP_INNER_ALLUP;
	BOOL m_bTHREAD_UPDATE_REELMAP_INNER_DN, m_bTHREAD_UPDATE_REELMAP_INNER_ALLDN;
	BOOL m_bTHREAD_UPDATE_REELMAP_ITS;

	BOOL m_bTHREAD_UPDATE_REELMAP_UP, m_bTHREAD_UPDATE_REELMAP_ALLUP;
	BOOL m_bTHREAD_UPDATE_REELMAP_DN, m_bTHREAD_UPDATE_REELMAP_ALLDN;
	BOOL m_bTHREAD_REELMAP_YIELD_UP, m_bTHREAD_REELMAP_YIELD_ALLUP;
	BOOL m_bTHREAD_REELMAP_YIELD_DN, m_bTHREAD_REELMAP_YIELD_ALLDN;
	BOOL m_bTHREAD_RELOAD_RST_UP, m_bTHREAD_RELOAD_RST_ALLUP;
	BOOL m_bTHREAD_RELOAD_RST_DN, m_bTHREAD_RELOAD_RST_ALLDN;
	BOOL m_bTHREAD_RELOAD_RST_UP_INNER, m_bTHREAD_RELOAD_RST_ALLUP_INNER;
	BOOL m_bTHREAD_RELOAD_RST_DN_INNER, m_bTHREAD_RELOAD_RST_ALLDN_INNER;
	BOOL m_bTHREAD_RELOAD_RST_ITS, m_bTHREAD_REELMAP_YIELD_ITS;

	BOOL m_bTHREAD_UPDATE_YIELD_UP, m_bTHREAD_UPDATE_YIELD_ALLUP;
	BOOL m_bTHREAD_UPDATE_YIELD_DN, m_bTHREAD_UPDATE_YIELD_ALLDN;
	BOOL m_bTHREAD_UPDATE_YIELD_ITS;
	BOOL m_bTHREAD_UPDATE_YIELD_INNER_UP, m_bTHREAD_UPDATE_YIELD_INNER_ALLUP;
	BOOL m_bTHREAD_UPDATE_YIELD_INNER_DN, m_bTHREAD_UPDATE_YIELD_INNER_ALLDN;

	BOOL m_bTHREAD_MK[4];	// [0] Auto-Left, [1] Auto-Right, [2] Manual-Left, [3] Manual-Right
	BOOL m_bTHREAD_DISP_DEF;
	BOOL m_bTHREAD_SHIFT2MK;// [2];		// [0] : Cam0, [1] : Cam1

	BOOL m_bTHREAD_DISP_DEF_INNER;

	BOOL m_bStopFromThread, m_bBuzzerFromThread;


	int	m_nStepTHREAD_DISP_DEF_INNER;
	int	m_nSnTHREAD_UPDATAE_YIELD;

	BOOL m_bAnswer[10];
	int m_nDummy[2], m_nAoiLastSerial[2]; //[0]: Up, [1]: Dn
	BOOL m_bChkLastProcVs;

	double m_dTempPauseLen;
	DWORD m_dwCycSt, m_dwCycTim;

	BOOL m_bRtnMyMsgBox[4]; 	// [0] mk0, [1] mk1, [2] reject0, [3] reject1
	int m_nRtnMyMsgBox[4]; 		// [0] mk0, [1] mk1, [2] reject0, [3] reject1
	int m_nRtnMyMsgBoxIdx;

	int m_nPrevStepAuto, m_nPrevMkStAuto;
	int m_nStepMk[4], m_nMkPcs[4]; 	// [0] Auto-Left, [1] Auto-Right, [2] Manual-Left, [3] Manual-Right  ; m_nStepMk(마킹Sequence), nMkOrderIdx(마킹한 count)
	int m_nMkStrip[2][4]; // [nCam][nStrip] - [좌/우][] : 스트립에 펀칭한 피스 수 count
	int m_nErrCnt;
	int m_nStepInitView;

	BOOL m_bDispMsg, m_bWaitClrDispMsg;
	BOOL m_bDispMsgDoAuto[10];
	int m_nStepDispMsg[10];
	int m_nDebugStep;
	CString m_sFixMsg[2]; //[0]:up , [1]:dn

	int m_nCntTowerWinker, m_nCntBtnWinker[4], m_nDlyWinker[4], m_nCntBz;
	BOOL m_bTimTowerWinker, m_bTimBtnWinker, m_bTimBuzzerWarn;
	BOOL m_bTowerWinker[3], m_bBtnWinker[4]; // [R/G/Y] , [Ready/Reset/Run/Stop]
	BOOL m_bTIM_DISP_STATUS, m_bTIM_MPE_IO;

	CString m_sPrevMyMsg;
	CString m_sTick, m_sDispTime;
	DWORD m_dwSetDlySt[10], m_dwSetDlyEd[10];
	DWORD m_dwSetDlySt0[10], m_dwSetDlyEd0[10];
	DWORD m_dwSetDlySt1[10], m_dwSetDlyEd1[10];

	CString m_sDispMsg[10];

	double m_dTotVel, m_dPartVel;
	BOOL m_bTIM_CHK_TEMP_STOP;
	BOOL m_bTIM_SAFTY_STOP;
	BOOL m_bTIM_CAMMASTER_UPDATE;
	CString m_sMyMsg; int m_nTypeMyMsg;
	int m_nVsBufLastSerial[2];

	int m_nStepElecChk;
	BOOL m_bStopFeeding;
	BOOL m_bChkLightErr;

	int m_nTotMk[2], m_nCurMk[2]; // [0]: 좌 MK, [1]: 우 MK
	int m_nPrevTotMk[2], m_nPrevCurMk[2]; // [0]: 좌 MK, [1]: 우 MK

	double m_dElecChkVal;
	BOOL m_bContEngraveF;

	int m_nLotEndSerial;

	BOOL m_bTIM_INIT_VIEW;
	BOOL m_bCam, m_bReview;
	double m_dEnc[MAX_AXIS], m_dTarget[MAX_AXIS];
	double m_dNextTarget[MAX_AXIS];
	int m_nSelRmap, m_nSelRmapInner;
	int m_nStepAuto;

	int m_nStop;
	BOOL m_bReMk;
	BOOL m_bChkMpeIoOut;

	BOOL m_bProbDn[2]; // 좌/우 .

	// Auto Sequence
	BOOL m_bAuto, m_bManual, m_bOneCycle;
	BOOL m_bMkTmpStop, m_bAoiLdRun, m_bAoiLdRunF;
	int	m_nStepTHREAD_DISP_DEF;

	BOOL m_bSwRun, m_bSwRunF;
	BOOL m_bSwStop, m_bSwStopF;
	BOOL m_bSwReset, m_bSwResetF;
	BOOL m_bSwReady, m_bSwReadyF;

	BOOL m_bNewModel;
	DWORD m_dwLotSt, m_dwLotEd;
	long m_lFuncId;
	BOOL m_bDrawGL;
	BOOL m_bCont;	// 이어가기
	DWORD m_dwStMkDn[2];
	BOOL m_bInit;
	BOOL m_bSwStopNow;
	BOOL m_bShowModalMyPassword;

	int m_nRstNum;

	BOOL m_bBufHomeDone, m_bReadyDone;
	BOOL m_bCollision[2], m_bPriority[4];
	BOOL m_bEngBufHomeDone;

	unsigned long m_Flag;
	unsigned long m_AoiLdRun;
	BOOL m_bDoneDispMkInfo[2][2]; // [nCam][Up/Dn]

	BOOL m_bFailAlign[2][4]; // [nCam][nPos] 
	BOOL m_bReAlign[2][4]; // [nCam][nPos] 
	BOOL m_bSkipAlign[2][4]; // [nCam][nPos] 

	BOOL m_bDoMk[2];			// [nCam] : TRUE(Punching), FALSE(Stop Punching)
	BOOL m_bDoneMk[2];			// [nCam] : TRUE(Punching 완료), FALSE(Punching 미완료)
	BOOL m_bReMark[2];			// [nCam] : TRUE(Punching 다시시작), FALSE(pass)

	int m_nMonAlmF, m_nClrAlmF;
	BOOL m_bLotEnd, m_bLastProc, m_bLastProcFromUp, m_bLastProcFromEng;
	BOOL m_bMkSt, m_bMkStSw;
	BOOL m_bEngSt, m_bEngStSw;
	BOOL m_bEng2dSt, m_bEng2dStSw;
	int m_nMkStAuto, m_nEngStAuto, m_nEng2dStAuto;
	int m_nLotEndAuto, m_nLastProcAuto;
	BOOL m_bLoadShare[2]; // [Up/Dn]
	CString m_sNewLotUp, m_sNewLotDn;

	BOOL m_bAoiFdWrite[2], m_bAoiFdWriteF[2]; // [Up/Dn]
	BOOL m_bAoiTest[2], m_bAoiTestF[2], m_bWaitPcr[2]; // [Up/Dn]

	BOOL m_bEngFdWrite, m_bEngFdWriteF;
	BOOL m_bEngTest, m_bEngTestF;

	BOOL m_bCycleStop, m_bContDiffLot;
	CString m_sDispMain;
	BOOL m_bStopF_Verify;
	BOOL m_bInitAuto, m_bInitAutoLoadMstInfo;

	BOOL m_bShowMyMsg;
	CWnd *m_pMyMsgForeground;

	BOOL m_bRejectDone[2][MAX_STRIP_NUM]; // Shot[2], Strip[4] - [좌/우][] : 스트립에 펀칭한 피스 수 count가 스트립 폐기 설정수 완료 여부 

	CString m_sDispSts[2];

	BOOL m_bDispMyMsgBox;
	//CArMyMsgBox  m_ArrayMyMsgBox;
	BOOL m_bDispMain;
	//CArDispMain  m_ArrayDispMain;

	int m_nWatiDispMain;

	int m_nNewLot;
	int m_nSaveMk0Img, m_nSaveMk1Img;

	int m_nSerialRmapUpdate;
	int m_nSerialRmapInnerUpdate;

	BOOL m_bShift2Mk;

	BOOL m_bSerialDecrese;
	
	BOOL m_bUpdateYield, m_bUpdateYieldOnRmap;
	BOOL m_bTHREAD_UPDATAE_YIELD[2];		// [0] : Cam0, [1] : Cam1
	int	m_nSerialTHREAD_UPDATAE_YIELD[2];	// [0] : Cam0, [1] : Cam1

	void SetPriority();
	void ResetPriority();
	void DelOverLotEndSerialUp(int nSerial);
	void DelOverLotEndSerialDn(int nSerial);


	BOOL ChkVsShare(int &nSerial);
	BOOL ChkVsShareUp(int &nSerial);
	BOOL ChkVsShareDn(int &nSerial);
	void ChkShare();
	void ChkShareUp();
	void ChkShareDn();
	BOOL ChkShare(int &nSerial);
	BOOL ChkShareUp(int &nSerial);
	BOOL ChkShareDn(int &nSerial);
	BOOL ChkShareIdx(int *pBufSerial, int nBufTot, int nShareSerial);
	void ChkBuf();
	void ChkBufUp();
	void ChkBufDn();
	BOOL ChkBufUp(int* pSerial, int &nTot);
	BOOL ChkBufDn(int* pSerial, int &nTot);
	BOOL ChkBufIdx(int* pSerial, int nTot = 0);

	BOOL SortingInUp(CString sPath, int nIndex);
	BOOL SortingOutUp(int* pSerial, int nTot);
	void SwapUp(__int64 *num1, __int64 *num2);
	BOOL SortingInDn(CString sPath, int nIndex);
	BOOL SortingOutDn(int* pSerial, int nTot);
	void SwapDn(__int64 *num1, __int64 *num2);

protected:
	DECLARE_MESSAGE_MAP()
};


