#pragma once

#include "Global/GlobalDefine.h"
#include "Process/ThreadTask.h"
#include "Global/MyData.h"

#define MAX_THREAD_MGR_PROC			4

// CManagerProcedure

class CManagerProcedure : public CWnd
{
	DECLARE_DYNAMIC(CManagerProcedure)

	CWnd* m_pParent;

	void InitThread();
	void KillThread();

	// DoAuto
	void DoAtuoGetMkStSignal();
	void DoAutoSetLastProcAtPlc();
	void DoAutoSetFdOffsetLastProc();
	void DoAutoSetFdOffset();
	void DoAutoSetFdOffsetEngrave();
	void DoAutoChkCycleStop();
	void DoAutoDispMsg();
	void DoAutoChkShareFolder();
	void DoAutoMarking();	// MarkingWith2PointAlign() or MarkingWith4PointAlign()
	void DoAutoMarkingEngrave();

	void MarkingWith2PointAlign();
	void Mk2PtReady();
	void Mk2PtChkSerial();
	void Mk2PtInit();
	void Mk2PtAlignPt0();
	void Mk2PtAlignPt1();
	void Mk2PtMoveInitPos();
	void Mk2PtElecChk();
	void Mk2PtDoMarking();
	void Mk2PtShift2Mk();
	void Mk2PtLotDiff();
	void Mk2PtReject();
	void Mk2PtErrStop();

	void MarkingWith4PointAlign();
	void Mk4PtReady();
	void Mk4PtChkSerial();
	void Mk4PtInit();
	void Mk4PtAlignPt0();
	void Mk4PtAlignPt1();
	void Mk4PtAlignPt2();
	void Mk4PtAlignPt3();
	void Mk4PtMoveInitPos();
	void Mk4PtElecChk();
	void Mk4PtDoMarking();
	void Mk4PtLotDiff();
	void Mk4PtReject();
	void Mk4PtErrStop();

public:
	CManagerProcedure(CWnd* pParent = NULL);
	virtual ~CManagerProcedure();

	DWORD m_dwThreadTick[MAX_THREAD_MGR_PROC];
	BOOL m_bThread[MAX_THREAD_MGR_PROC];
	CThreadTask m_Thread[MAX_THREAD_MGR_PROC];

	void InitVal();
	void DispDefImg();
	void DispDefImgInner();
	BOOL CopyDefImg(int nSerial);
	BOOL CopyDefImg(int nSerial, CString sNewLot);
	BOOL IsDoneDispMkInfo();
	BOOL SetSerialReelmap(int nSerial, BOOL bDumy = FALSE);
	BOOL SetSerialMkInfo(int nSerial, BOOL bDumy = FALSE);
	void SetFixPcs(int nSerial);
	void SwMenu01DispDefImg(BOOL bOn);
	//BOOL ChkCollision();
	//BOOL ChkCollision(int nAxisId, double dTgtPosX);
	//BOOL ChkCollision(int nAxisId, double dTgtPosX, double dTgtNextPos);
	void DoMark0Its();
	void DoMark1Its();
	void DoMark0();
	void DoMark1();
	void DoMark0All();
	void DoMark1All();
	void DoReject0();
	void DoReject1();
	void GetEnc();
	BOOL IsRunAxisX();
	void EStop();
	void Stop();
	void Buzzer(BOOL bOn, int nCh = 0);
	void DispMain(CString sMsg, COLORREF rgb = RGB(0, 255, 0));
	int MsgBox(CString sMsg, int nThreadIdx = 0, int nType = MB_OK, int nTimOut = DEFAULT_TIME_OUT, BOOL bEngave = TRUE);		// SyncMsgBox
	void DispInitRun();
	BOOL IsReady();
	void ClrDispMsg();
	BOOL IsAoiLdRun();
	void ResetWinker(); // 0:Ready, 1:Reset, 2:Run, 3:Stop
	BOOL IsVerify();
	//BOOL IsReview();
	//BOOL IsReview0();
	//BOOL IsReview1();
	int GetVsBufLastSerial();
	int GetVsUpBufLastSerial();
	int GetVsDnBufLastSerial();
	double GetMkFdLen();
	//BOOL IsVs();
	//BOOL IsVsUp();
	//BOOL IsVsDn();
	void SetDummyUp();
	void SetDummyDn();
	BOOL MakeDummyUp(int nErr);
	BOOL MakeDummyDn(int nErr);
	int GetAoiUpSerial();
	int GetAoiDnSerial();
	BOOL GetAoiUpVsStatus();
	BOOL GetAoiDnVsStatus();
	BOOL IsSetLotEnd();
	int GetAoiUpDummyShot();
	int GetAoiDnDummyShot();
	void SetAoiDummyShot(int nAoi, int nDummy);
	//BOOL IsNoMk();
	//BOOL IsNoMk0();
	//BOOL IsNoMk1();
	void SetListBuf();	// pDoc->m_ListBuf에 버퍼 폴더의 시리얼번호를 가지고 재갱신함.
	//BOOL SetMkIts(BOOL bRun = TRUE);	// Marking Start
	//BOOL SetMk(BOOL bRun = TRUE);
	int GetTotDefPcs(int nSerial);
	void UpdateYield();
	BOOL ChkLotCutPos();
	void UpdateRst();
	void UpdateWorking();
	BOOL TwoPointAlign0(int nPos);
	BOOL TwoPointAlign1(int nPos);
	BOOL IsShowLive();
	void ShowLive(BOOL bShow = TRUE);
	void Shift2Mk();
	void Shift2Buf();

	BOOL IsShare();
	BOOL IsShareUp();
	BOOL IsShareDn();
	BOOL IsVsShare();
	int GetShareUp();
	int GetShareDn();

	BOOL IsBuffer(int nNum = 0);
	BOOL IsBufferUp();
	BOOL IsBufferDn();
	int GetBuffer(int *pPrevSerial = NULL);
	int GetBufferUp(int *pPrevSerial = NULL);
	int GetBufferDn(int *pPrevSerial = NULL);

	BOOL IsBuffer0();
	BOOL IsBufferUp0();
	BOOL IsBufferDn0();
	int GetBuffer0(int *pPrevSerial = NULL);
	int GetBufferUp0(int *pPrevSerial = NULL);
	int GetBufferDn0(int *pPrevSerial = NULL);

	BOOL IsBuffer1();
	BOOL IsBufferUp1();
	BOOL IsBufferDn1();
	int GetBuffer1(int *pPrevSerial = NULL);
	int GetBufferUp1(int *pPrevSerial = NULL);
	int GetBufferDn1(int *pPrevSerial = NULL);


	int GetLastShotMk();	// m_pDlgFrameHigh에서 얻거나 없으면, sPathOldFile폴더의 ReelMapDataDn.txt에서 _T("Info"), _T("Marked Shot") 찾음.
	int GetLastShotUp();	// pView->m_pDlgFrameHigh->m_nAoiLastShot[0]
	int GetLastShotDn();	// pView->m_pDlgFrameHigh->m_nAoiLastShot[1]

	BOOL SetSerial(int nSerial, BOOL bDumy = FALSE);
	void SetReject();
	void DoShift2Mk();
	void SetMkFdLen();
	void SetLotEnd(int nSerial);
	void SetCycTime();
	int GetCycTime(); // [mSec]
	void SetAlignPos();
	BOOL GetAoiDnInfo(int nSerial, int *pNewLot = NULL, BOOL bFromBuf = FALSE); // TRUE: CHANGED, FALSE: NO CHANGED
	void InitInfo();
	//void ResetMkInfo(int nAoi = 0); // 0 : AOI-Up , 1 : AOI-Dn , 2 : AOI-UpDn
	void ModelChange(int nAoi = 0); // 0 : AOI-Up , 1 : AOI-Dn
	BOOL ChkLastProcFromEng();
	BOOL ChkStShotNum();
	BOOL ChkContShotNum();
	BOOL GetAoiUpInfo(int nSerial, int *pNewLot = NULL, BOOL bFromBuf = FALSE); // TRUE: CHANGED, FALSE: NO CHANGED
	BOOL ChkLastProcFromUp();
	BOOL IsSameUpDnLot();
	int GetAutoStep();
	int LoadPcrUp(int nSerial, BOOL bFromShare = FALSE);	// return : 2(Failed), 1(정상), -1(Align Error, 노광불량), -2(Lot End)
	int LoadPcrDn(int nSerial, BOOL bFromShare = FALSE);	// return : 2(Failed), 1(정상), -1(Align Error, 노광불량), -2(Lot End)
	void CompletedMk(int nCam); // 0: Only Cam0, 1: Only Cam1, 2: Cam0 and Cam1, 3: None
	BOOL IsInitPos0();
	BOOL IsInitPos1();
	BOOL DoElecChk(CString &sRst);  // TRUE: bDone , FALSE: Doing
	BOOL IsMoveDone();
	BOOL IsMkDone();
	void ChgLot();
	BOOL ChkLotEnd(int nSerial);
	BOOL ChkLotEndUp(int nSerial);
	BOOL ChkLotEndDn(int nSerial);
	BOOL MoveAlign0(int nPos);
	BOOL MoveAlign1(int nPos);
	BOOL FourPointAlign0(int nPos, BOOL bDraw = FALSE);
	BOOL FourPointAlign1(int nPos, BOOL bDraw = FALSE);
	void MoveInitPos0(BOOL bWait = TRUE);
	void MoveInitPos1(BOOL bWait = TRUE);
	void MoveMkEdPos1();
	BOOL ChkLightErr();
	CfPoint GetMkPnt(int nSerial, int nMkPcs); // pcr 시리얼, pcr 불량 피스 읽은 순서 인덱스

	void InitAuto(BOOL bInit = TRUE);

	int m_nAoiCamInfoStrPcs[2]; // [0] : Up, [1] : Dn
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

	//BOOL m_bTHREAD_MK[4];	// [0] Auto-Left, [1] Auto-Right, [2] Manual-Left, [3] Manual-Right
	BOOL m_bTHREAD_DISP_DEF;
	BOOL m_bTHREAD_SHIFT2MK;// [2];		// [0] : Cam0, [1] : Cam1

	BOOL m_bTHREAD_DISP_DEF_INNER;

	int	m_nStepTHREAD_DISP_DEF_INNER;
	int	m_nSnTHREAD_UPDATAE_YIELD;

	//static UINT ThreadProc0(LPVOID lpContext); // DoMark0(), DoMark1()
	//static UINT ThreadProc1(LPVOID lpContext); // ChkCollision()
	static UINT ThreadProc0(LPVOID lpContext); // DispDefImg()
	static UINT ThreadProc1(LPVOID lpContext); // GetCurrentInfoSignal()
	static UINT ThreadProc2(LPVOID lpContext); // DispDefImgInner()
	static UINT ThreadProc3(LPVOID lpContext); // RunShift2Mk()
	

	BOOL m_bAnswer[10];
	int m_nDummy[2], m_nAoiLastSerial[2]; //[0]: Up, [1]: Dn
	BOOL m_bChkLastProcVs;

	double m_dTempPauseLen;
	DWORD m_dwCycSt, m_dwCycTim;

	BOOL m_bRtnMyMsgBox[4]; 	// [0] mk0, [1] mk1, [2] reject0, [3] reject1
	int m_nRtnMyMsgBox[4]; 		// [0] mk0, [1] mk1, [2] reject0, [3] reject1
	int m_nRtnMyMsgBoxIdx;

	int m_nPrevStepAuto, m_nPrevMkStAuto;
	//int m_nStepMk[4], m_nMkPcs[4]; 	// [0] Auto-Left, [1] Auto-Right, [2] Manual-Left, [3] Manual-Right  ; m_nStepMk(마킹Sequence), nMkOrderIdx(마킹한 count)
	//int m_nMkStrip[2][4]; // [nCam][nStrip] - [좌/우][] : 스트립에 펀칭한 피스 수 count
	int m_nErrCnt;
	int m_nDebugStep;
	CString m_sFixMsg[2]; //[0]:up , [1]:dn

	CString m_sPrevMyMsg;
	CString m_sTick, m_sDispTime;
	//DWORD m_dwSetDlySt[10], m_dwSetDlyEd[10];
	//DWORD m_dwSetDlySt0[10], m_dwSetDlyEd0[10];
	//DWORD m_dwSetDlySt1[10], m_dwSetDlyEd1[10];

	double m_dTotVel, m_dPartVel;
	BOOL m_bTIM_CAMMASTER_UPDATE;
	CString m_sMyMsg; int m_nTypeMyMsg;
	int m_nVsBufLastSerial[2];

	//int m_nStepElecChk;
	BOOL m_bStopFeeding;
	BOOL m_bChkLightErr;

	//int m_nTotMk[2], m_nCurMk[2]; // [0]: 좌 MK, [1]: 우 MK
	//int m_nPrevTotMk[2], m_nPrevCurMk[2]; // [0]: 좌 MK, [1]: 우 MK

	double m_dElecChkVal;
	BOOL m_bContEngraveF;

	int m_nLotEndSerial;

	//BOOL m_bCam, m_bReview;
	//double m_dEnc[MAX_AXIS], m_dTarget[MAX_AXIS];
	//double m_dNextTarget[MAX_AXIS];
	int m_nSelRmap, m_nSelRmapInner;
	int m_nStepAuto;

	int m_nStop;
	BOOL m_bReMk;
	//BOOL m_bProbDn[2]; // 좌/우 .

	// Auto Sequence
	//BOOL m_bAuto, m_bManual, m_bOneCycle;
	BOOL m_bMkTmpStop, m_bAoiLdRun, m_bAoiLdRunF;
	int	m_nStepTHREAD_DISP_DEF;

	BOOL m_bNewModel;
	DWORD m_dwLotSt, m_dwLotEd;
	long m_lFuncId;
	BOOL m_bCont;	// 이어가기
	DWORD m_dwStMkDn[2];
	BOOL m_bInit;
	BOOL m_bShowModalMyPassword;

	int m_nRstNum;

	BOOL m_bBufHomeDone, m_bReadyDone;
	//BOOL m_bCollision[2], m_bPriority[4];
	BOOL m_bEngBufHomeDone;

	unsigned long m_Flag;
	unsigned long m_AoiLdRun;
	BOOL m_bDoneDispMkInfo[2][2]; // [nCam][Up/Dn]

	BOOL m_bFailAlign[2][4]; // [nCam][nPos] 
	BOOL m_bReAlign[2][4]; // [nCam][nPos] 
	BOOL m_bSkipAlign[2][4]; // [nCam][nPos] 

	//BOOL m_bDoMk[2];			// [nCam] : TRUE(Punching), FALSE(Stop Punching)
	//BOOL m_bDoneMk[2];			// [nCam] : TRUE(Punching 완료), FALSE(Punching 미완료)
	//BOOL m_bReMark[2];			// [nCam] : TRUE(Punching 다시시작), FALSE(pass)

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

	BOOL m_bContDiffLot;
	BOOL m_bStopF_Verify;
	BOOL m_bInitAuto, m_bInitAutoLoadMstInfo;

	BOOL m_bShowMyMsg;

	//BOOL m_bRejectDone[2][MAX_STRIP_NUM]; // Shot[2], Strip[4] - [좌/우][] : 스트립에 펀칭한 피스 수 count가 스트립 폐기 설정수 완료 여부 

	CString m_sDispSts[2];

	BOOL m_bDispMyMsgBox;
	int m_nNewLot;
	int m_nSaveMk0Img, m_nSaveMk1Img;

	//int m_nSerialRmapUpdate;
	//int m_nSerialRmapInnerUpdate;

	BOOL m_bShift2Mk;
	BOOL m_bSerialDecrese;
	
	BOOL m_bUpdateYield, m_bUpdateYieldOnRmap;
	BOOL m_bTHREAD_UPDATAE_YIELD[2];		// [0] : Cam0, [1] : Cam1
	int	m_nSerialTHREAD_UPDATAE_YIELD[2];	// [0] : Cam0, [1] : Cam1

	//void SetPriority();
	//void ResetPriority();
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

	BOOL IsRun();
	BOOL IsDoneDispMkInfoInner();
	BOOL SetSerialReelmapInner(int nSerial, BOOL bDumy = FALSE);
	BOOL ChkLastProc();
	BOOL SetSerialMkInfoInner(int nSerial, BOOL bDumy = FALSE);
	void RunShift2Mk();
	BOOL GetCurrentInfoSignal();

	void DoAuto();
	void UpdateYieldOnThread(int nSerial);
	BOOL UpdateReelmap(int nSerial);
	BOOL UpdateReelmapInner(int nSerial);
	BOOL ReloadReelmap(int nSerial);
	BOOL IsVs();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};


