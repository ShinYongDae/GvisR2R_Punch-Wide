
// GvisR2R_PunchView.h : CGvisR2R_PunchView 클래스의 인터페이스
//

#pragma once

#include "Dialog/DlgMyMsg.h"
#include "Dialog/DlgMsgBox.h"
#include "Dialog/DlgFrameHigh.h"
#include "Dialog/DlgInfo.h"
#include "Dialog/DlgMenu01.h"
#include "Dialog/DlgMenu02.h"
#include "Dialog/DlgMenu03.h"
#include "Dialog/DlgMenu04.h"
#include "Dialog/DlgMenu05.h"
#include "Dialog/DlgMenu06.h"
#include "Dialog/DlgUtil01.h"
#include "Dialog/DlgUtil02.h"
//#include "Dialog/DlgUtil03.h"

#include "Device/Engrave.h"

//#include "Device/Motion.h"
//#include "Device/Light.h"
//#include "Device/Smac.h"
//#include "Device/Vision.h"
//#include "Device/MpDevice.h"
//#include "Device/SR1000W.h"
//#include "Device/Dts.h"
//
//#include "Process/PtAlign.h"
#include "Process/ThreadTask.h"

#ifdef USE_FLUCK
#include "Device/Fluck.h"
#endif

#include "ManagerReelmap.h"
#include "ManagerProcedure.h"
#include "ManagerPunch.h"

#include "GvisR2R_PunchDoc.h"


#define LAMP_DELAY				5	// 5 * 100[mSec]
#define BUZZER_DELAY			2	// 5 * 100[mSec]

#define FROM_DOMARK0			10
#define FROM_DOMARK1			50
#define FROM_DISPDEFIMG			100

#define AT_LP					10
#define MK_DONE_CHECK			30	// m_nStepMk
#define MK_LIGHT_ERR			50	// m_nStepMk
#define MK_END					100	// m_nStepMk

#define MK_ST					100	// m_nMkStAuto
#define ERR_PROC				120	// m_nMkStAuto
#define REJECT_ST				200	// m_nMkStAuto
#define ERROR_ST				250	// m_nMkStAuto

#define MK_VERIFY				200	// m_nSetpAuto
#define MK_REVIEW				220	// m_nSetpAuto
#define LAST_PROC				250	// m_nSetpAuto
#define LAST_PROC_VS_ALL		300	// m_nSetpAuto
#define LAST_PROC_VS_UP			350	// m_nSetpAuto
#define LAST_PROC_VS_DN			400	// m_nSetpAuto
#define LOT_END					500	// m_nSetpAuto

#define ENG_ST					100	// m_nEngStAuto
#define ENG_2D_ST				150	// m_nEngStAuto

#define TIM_INIT_VIEW			0
//#define TIM_TOWER_WINKER		10
#define TIM_BTN_WINKER			11
#define TIM_BUZZER_WARN			12
#define TIM_MB_TIME_OUT			13
#define TIM_DISP_STATUS			14
#define TIM_MPE_IO				15
// #define TIM_MK_START			16
#define TIM_SHOW_MENU01			18
#define TIM_SHOW_MENU02			19
#define TIM_CHK_TEMP_STOP		20
#define TIM_SAFTY_STOP			21
#define TIM_CAMMASTER_UPDATE	22
#define TIM_START_UPDATE		100

//#define MAX_THREAD				6

namespace Read2dIdx
{
	typedef enum Index {
		Start = 2, ChkSn = 4, InitRead = 10, Move0Cam1 = 12, Move0Cam0 = 14, Align1_0 = 17, Align0_0 = 18,
		Move1Cam1 = 21, Move1Cam0 = 23, Align1_1 = 26, Align0_1 = 27, MoveInitPt = 29, ChkElec = 32, DoRead = 35,
		Verify = 37, DoneRead = 38, LotDiff = 50
	};
}

namespace Mk1PtIdx
{
	typedef enum Index {
		Start = 2, ChkSn = 4, InitMk = 10, Move0Cam1 = 12, Move0Cam0 = 14, Align1_0 = 17, Align0_0 = 18,
		Move1Cam1 = 21, Move1Cam0 = 23, Align1_1 = 26, Align0_1 = 27, MoveInitPt = 29, ChkElec = 32, DoMk = 35,
		Verify = 37, DoneMk = 38, LotDiff = 50
	};
}

namespace Mk2PtIdx
{
	typedef enum Index {
		Start = 2, ChkSn = 4, InitMk = 10, Move0Cam1 = 12, Move0Cam0 = 14, Align1_0 = 17, Align0_0 = 18,
		Move1Cam1 = 21, Move1Cam0 = 23, Align1_1 = 26, Align0_1 = 27, MoveInitPt = 29, ChkElec = 32, DoMk = 35,
		Verify = 38, DoneMk = 39, LotDiff = 50, Shift2Mk = 60
	}; 
}

namespace Mk4PtIdx
{
	typedef enum Index {
		Start = 2, ChkSn = 4, InitMk = 10, Move0Cam1 = 12, Move0Cam0 = 14, Align1_0 = 17, Align0_0 = 18,
		Move1Cam1 = 21, Move1Cam0 = 23, Align1_1 = 26, Align0_1 = 27, Move2Cam1 = 29, Move2Cam0 = 31,
		Align1_2 = 34, Align0_2 = 35, Move3Cam1 = 37, Move3Cam0 = 39, Align1_3 = 42, Align0_3 = 43,
		MoveInitPt = 45, ChkElec = 48, DoMk = 51,
		Verify = 53, DoneMk = 54, LotDiff = 60, Shift2Mk = 70
	};
}

typedef enum DlgId {DlgInfo=0, DlgFrameHigh, DlgMenu01, DlgMenu02, DlgMenu03, DlgMenu04, DlgMenu05, DlgMenu06};

typedef struct _DispMain
{
	CString strMsg;
	COLORREF rgb;

	_DispMain()
	{
		Init();
	}
	_DispMain(CString sMsg, COLORREF Rgb)
	{
		strMsg = sMsg;
		rgb = Rgb;
	}

	void Init()
	{
		strMsg = _T("");
		rgb = RGB_WHITE;
	}

}stDispMain;
typedef CArray<stDispMain, stDispMain> CArDispMain;



class CGvisR2R_PunchView : public CFormView
{
	CDlgMyMsg* m_pDlgMyMsg;
	CCriticalSection m_csMyMsgBox;
	CCriticalSection m_csDispMain;
	int m_nStepInitView;

	BOOL m_bDispMsg, m_bWaitClrDispMsg;

	int m_nCntBz;
	BOOL m_bTimBuzzerWarn;
	BOOL m_bTIM_DISP_STATUS, m_bTIM_MPE_IO;

	CString m_sDispMsg[10];
	
	void InitMgrProcedure();
	void InitMgrReelmap();
	void InitMgrPunch();

	void InitMyMsg();
	void CloseMyMsg();

	void DispStsBar();
	void UpdateLotTime();

	void ExitProgram();
	void HideAllDlg();
	void DelAllDlg();
	void DispTime();

	BOOL SetCollision(double dCollisionMargin);
	void DispStsMainMsg(int nIdx = 0);

protected: // serialization에서만 만들어집니다.
	CGvisR2R_PunchView();
	DECLARE_DYNCREATE(CGvisR2R_PunchView)

public:
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_GVISR2R_PUNCH_FORM };
#endif

// 특성입니다.
public:
	CGvisR2R_PunchDoc* GetDocument() const;

	CEngrave* m_pEngrave;
	CManagerReelmap *m_mgrReelmap;
	CManagerProcedure *m_mgrProcedure;
	CManagerPunch *m_mgrPunch;

	BOOL m_bTIM_CHK_TEMP_STOP;
	BOOL m_bTIM_SAFTY_STOP;
	BOOL m_bDestroyedView;
	double m_dMkFdOffsetX[2][4], m_dMkFdOffsetY[2][4];	// 2Cam / 4Point Align
	double m_dAoiUpFdOffsetX, m_dAoiUpFdOffsetY;
	double m_dAoiDnFdOffsetX, m_dAoiDnFdOffsetY;
	double m_dEngFdOffsetX, m_dEngFdOffsetY;
	BOOL m_bBtnWinker[4]; // [R/G/Y] , [Ready/Reset/Run/Stop]

	BOOL DoReset();
	BOOL IsRun();

	BOOL DoElecChk(CString &sRst);  // TRUE: bDone , FALSE: Doing
	BOOL MpeWrite(CString strRegAddr, long lData, BOOL bCheck = FALSE);
	BOOL MpeReadBit(CString strRegAddr);

	void DispThreadTick();
	int GetCamMstActionCode();
	void SetPinPos(int nCam, CfPoint ptPnt);

	BOOL m_bDispMsgDoAuto[10];
	int m_nStepDispMsg[10];

	void CycleStop(BOOL bStop = TRUE);
	BOOL IsCycleStop();

	CPtAlign m_Align[2];	// [0] : LeftCam , [1] : RightCam
	CDlgMsgBox* m_pDlgMsgBox;
	CEvent      m_evtWaitClrDispMsg;

	CDlgInfo *m_pDlgInfo;
	CDlgFrameHigh *m_pDlgFrameHigh;
	CDlgMenu01 *m_pDlgMenu01;
	CDlgMenu02 *m_pDlgMenu02;
	CDlgMenu03 *m_pDlgMenu03;
	CDlgMenu04 *m_pDlgMenu04;
	CDlgMenu05 *m_pDlgMenu05;
	CDlgMenu06 *m_pDlgMenu06;

	BOOL m_bTIM_INIT_VIEW;
	double m_dEnc[MAX_AXIS];

	BOOL m_bSwStop, m_bSwStopF;
	BOOL m_bSwReset, m_bSwResetF;
	BOOL m_bSwReady, m_bSwReadyF;
	BOOL m_bDrawGL;
	BOOL m_bSwStopNow;
	CString m_sDispMain;
	CArMyMsgBox  m_ArrayMyMsgBox;
	BOOL m_bDispMain;
	CArDispMain  m_ArrayDispMain;
	int m_nWatiDispMain;

// 작업입니다.
public:
	int GetLastShotMk();
	int GetLastShotUp();	// pView->m_pDlgFrameHigh->m_nAoiLastShot[0]
	int GetLastShotDn();	// pView->m_pDlgFrameHigh->m_nAoiLastShot[1]
	void SwMenu01DispDefImg(BOOL bOn);

	void SetLastSerialEng(int nSerial);
	int MsgBox(CString sMsg, int nThreadIdx = 0, int nType = MB_OK, int nTimOut = DEFAULT_TIME_OUT, BOOL bEngave = TRUE);		// SyncMsgBox
	int AsyncMsgBox(CString sMsg, int nThreadIdx = 1, int nType = MB_OK, int nTimOut = DEFAULT_TIME_OUT);						// AsyncMsgBox
	int WaitRtnVal(int nThreadIdx = 1);
	afx_msg LRESULT OnMyMsgExit(WPARAM wPara, LPARAM lPara);

	BOOL GetItsSerialInfo(int nItsSerial, BOOL &bDualTest, CString &sLot, CString &sLayerUp, CString &sLayerDn, int nOption = 0);		// 내층에서의 ITS 시리얼의 정보
	void InitMstData();
	void SwMenu01UpdateWorking(BOOL bOn);
	void ChangeModel();
	BOOL WatiDispMain(int nDelay);
	void SetAoiLastShot(int nAoi, int nSerial);
	CString GetProcessNum();
	void DispMain(CString sMsg, COLORREF rgb = RGB(0, 255, 0));
	int DoDispMain();
	void GetDispMsg(CString &strMsg, CString &strTitle);
	void DispMsg(CString strMsg, CString strTitle = _T(""), COLORREF color = RGB(255, 0, 0), DWORD dwDispTime = 0, BOOL bOverWrite = TRUE);
	void DoDispMsg(CString strMsg, CString strTitle = _T(""), COLORREF color = RGB(255, 0, 0), DWORD dwDispTime = 0, BOOL bOverWrite = TRUE);
	void ClrDispMsg();
	BOOL WaitClrDispMsg();
	LONG OnQuitDispMsg(UINT wParam, LONG lParam);
	void ShowDlg(int nID);
	void DispIo();
	void DispDatabaseConnection();

	int MyPassword(CString strMsg, int nCtrlId = 0);
	void SetAoiFdPitch(double dPitch);
	void SetMkFdPitch(double dPitch);

	void GetEnc();
	void Buzzer(BOOL bOn, int nCh = 0);

	void DoIO();
	BOOL IsNoMk();

	void ShowLive(BOOL bShow = TRUE);

	CString GetTime();
	CString GetTime(stLotTime &LotTime);
	CString GetTime(int &nHour, int &nMinute, int &nSec);
	int GetTime(int nSel);
	void SetBufInitPos(double dPos);
	void SetEngBufInitPos(double dPos);
	void DispLotStTime();
	void SetListBuf();
	int GetTotDefPcsIts(int nSerial);
	void UpdateData(int nDlgId);

	// Auto Sequence
	BOOL IsReady();
	void RunShift2Mk();
	void Shift2Buf();
	void Shift2Mk();
	BOOL IsMkFdSts();		// not used
	void SetAoiFd();		// not used
	void SetMkFd();			// not used
	BOOL IsMkFd();			// not used
	BOOL IsAoiFd();			// not used
	void SetMkFd(double dDist);
	BOOL IsMkFdDone();
	BOOL IsAoiFdDone();
	double GetAoi2InitDist();
	double GetMkInitDist();
	void UpdateWorking();
	double GetRemain();
	void Stop();
	void SetLotSt();
	void SetLotEd();
	void DispLotTime();
	void SetTest(BOOL bOn);
	void SetTest0(BOOL bOn);
	void SetTest1(BOOL bOn);
	void InitAuto(BOOL bInit = TRUE);
	BOOL IsAoiTblVac();
	BOOL IsAoiTblVacDone();
	BOOL IsTest();
	BOOL IsTestUp();
	BOOL IsTestDn();
	BOOL IsTestDone();
	BOOL IsTestDoneUp();
	BOOL IsTestDoneDn();
	BOOL IsStop();
	int GetSerial();
	double GetMkFdLen();
	double GetTotVel();
	double GetPartVel();
	BOOL IsShare();
	BOOL IsShareUp();
	BOOL IsShareDn();
	BOOL IsVsShare();
	int GetShareUp();
	int GetShareDn();
	void DelSharePcrUp();
	void DelSharePcrDn();

	BOOL IsBuffer(int nNum = 0);
	BOOL IsBufferUp();
	BOOL IsBufferDn();
	BOOL IsAuto();
	void DoReject0();
	void DoReject1();
	BOOL SetSerial(int nSerial, BOOL bDumy = FALSE);

	int GetErrCode(int nSerial);
	int GetErrCodeUp(int nSerial);
	int GetErrCodeDn(int nSerial);

	int GetTotDefPcs(int nSerial);
	int GetTotDefPcsUp(int nSerial);
	int GetTotDefPcsDn(int nSerial);

	CfPoint GetMkPnt(int nMkPcs);
	CfPoint GetMkPnt(int nSerial, int nMkPcs);
	void Move0(CfPoint pt, BOOL bCam = FALSE);
	void Move1(CfPoint pt, BOOL bCam = FALSE);
	BOOL UpdateReelmap(int nSerial);
	BOOL UpdateReelmapInner(int nSerial);
	CfPoint GetMkPntIts(int nSerial, int nMkPcs); // pcr 시리얼, pcr 불량 피스 읽은 순서 인덱스

	void InitInfo();
	void InitReelmap();
	void InitReelmapUp();
	void InitReelmapDn();
	BOOL IsPinMkData();
	BOOL CopyDefImg(int nSerial);
	BOOL CopyDefImg(int nSerial, CString sNewLot);
	BOOL CopyDefImgUp(int nSerial, CString sNewLot = _T(""));
	BOOL CopyDefImgDn(int nSerial, CString sNewLot = _T(""));
	BOOL Grab0(int nPos, BOOL bDraw = TRUE);
	BOOL Grab1(int nPos, BOOL bDraw = TRUE);
	BOOL TwoPointAlign0(int nPos, BOOL bDraw = FALSE);
	BOOL TwoPointAlign1(int nPos, BOOL bDraw = FALSE);
	BOOL FourPointAlign0(int nPos, BOOL bDraw = FALSE);
	BOOL FourPointAlign1(int nPos, BOOL bDraw = FALSE);
	BOOL OnePointAlign(CfPoint &ptPnt);
	BOOL TwoPointAlign();
	BOOL GetAoiUpInfo(int nSerial, int *pNewLot = NULL, BOOL bFromBuf = FALSE); // TRUE: CHANGED, FALSE: NO CHANGED
	BOOL GetAoiDnInfo(int nSerial, int *pNewLot = NULL, BOOL bFromBuf = FALSE); // TRUE: CHANGED, FALSE: NO CHANGED
	BOOL LoadMySpec();
	BOOL MemChk();
	BOOL GetAoiUpOffset(CfPoint &OfSt);
	BOOL GetAoiDnOffset(CfPoint &OfSt);
	BOOL GetMkOffset(CfPoint &OfSt);
	BOOL IsAoiLdRun();
	void LotEnd();
	void ResetWinker(); // 0:Ready, 1:Reset, 2:Run, 3:Stop
	void SetLotEnd(int nSerial);
	int GetLotEndSerial();
	void ResetMkInfo(int nAoi = 0); // 0 : AOI-Up , 1 : AOI-Dn , 2 : AOI-UpDn
	void ModelChange(int nAoi = 0); // 0 : AOI-Up , 1 : AOI-Dn
	void UpdateRst();
	void ClrMkInfo();
	BOOL IsShowLive();
	BOOL IsChkTmpStop();
	BOOL ChkLastProc();
	double GetAoiUpFdLen();
	double GetAoiDnFdLen();
	BOOL IsReview();
	BOOL IsJogRtDn();
	BOOL IsJogRtUp();
	BOOL IsJogRtDn0();
	BOOL IsJogRtUp0();
	BOOL IsJogRtDn1();
	BOOL IsJogRtUp1();
	void SwAoiEmg(BOOL bOn);
	BOOL IsDoneDispMkInfo();

	afx_msg LRESULT OnBufThreadDone(WPARAM wPara, LPARAM lPara);
	afx_msg LRESULT wmClientReceived(WPARAM wParam, LPARAM lParam);

	int ChkSerial(); // 0: Continue, -: Previous, +:Discontinue
	void SetAlignPos();
	BOOL IsRdyTest();
	BOOL IsRdyTest0();
	BOOL IsRdyTest1();
	BOOL LoadPcrUp(int nSerial, BOOL bFromShare = FALSE);
	BOOL LoadPcrDn(int nSerial, BOOL bFromShare = FALSE);

	void SetLastProc();
	BOOL IsLastProc();
	BOOL IsLastJob(int nAoi); // 0 : AOI-Up , 1 : AOI-Dn , 2 : AOI-UpDn

	void MonPlcSignal();
	void ClrAlarm();

	void ChkTempStop(BOOL bChk);
	void ChgLot();

	void SetLotLastShot();
	int GetMkStripIdx(int nDefPcsId); // 0 : Fail , 1~4 : Strip Idx
	int GetMkStripIdx(int nSerial, int nMkPcs); // 0 : Fail , 1~4 : Strip Idx
	CString GetMkInfo(int nSerial, int nMkPcs);

	BOOL IsMkStrip(int nStripIdx);
	BOOL ChkLotCutPos();
	BOOL ChkYield();// (double &dTotLmt, double &dPrtLmt, double &dRatio);
	void UpdateYieldOnRmap();
	BOOL OpenReelmapFromBuf(int nSerial);
	void SetPathAtBuf();

	BOOL SetSerialReelmap(int nSerial, BOOL bDumy = FALSE);
	BOOL SetSerialMkInfo(int nSerial, BOOL bDumy = FALSE);
	BOOL ChkLastProcFromUp();
	BOOL ChkLastProcFromEng();

	void SetFixPcs(int nSerial);
	void MakeSapp3();
	void GetResult();
	void MakeResult();
	void MakeResultIts();
	void MakeResultMDS();

	void SetDualTest(BOOL bOn = TRUE);
	void SetTwoMetal(BOOL bSel, BOOL bOn = TRUE);

	void DispStsBar(CString sMsg, int nIdx = 0);
	void AdjPinPos();

	BOOL IsEngraveFdSts();
	BOOL IsEngraveFd();
	void SetEngraveFdSts();
	void SetEngraveStopSts();
	void SetEngraveSts(int nStep);
	void SetEngraveFd();
	void SetEngraveFd(double dDist);
	void MoveEngrave(double dOffset);

	double GetEngraveFdLen();
	double GetAoiInitDist();
	double GetAoiRemain();
	void SetEngraveFdPitch(double dPitch);
	void DestroyView();
	BOOL LoadMasterSpec();

	void UpdateYield();
	void UpdateYield(int nSerial);
	void SetEngFd();
	void MoveEng(double dOffset);
	BOOL GetEngOffset(CfPoint &OfSt);

	void SetMyMsgYes();
	void SetMyMsgNo();
	void SetMyMsgOk();

	BOOL GetCurrentInfoSignal();
	void InitAutoEngSignal();
	BOOL GetCurrentInfoEng();
	BOOL IsConnectedSr();
	BOOL IsConnectedEng();

	BOOL Get2dCode(CString &sLot, int &nSerial);
	BOOL Set2dRead(BOOL bRun = TRUE);
	BOOL Is2dReadDone();

	BOOL CheckConectionSr1000w();
	void SwAoiRelation();
	void SwAoiTblBlw();
	void SwAoiFdVac();
	void SwAoiTqVac();

	BOOL m_bSetSig, m_bSetSigF, m_bSetData, m_bSetDataF;
	BOOL m_bLoadMstInfo, m_bLoadMstInfoF;
	BOOL m_bTIM_START_UPDATE;

	BOOL m_bEscape;

	// ITS
	void UpdateRstInner();
	BOOL IsDoneDispMkInfoInner();
	BOOL SetSerialReelmapInner(int nSerial, BOOL bDumy = FALSE);
	BOOL SetSerialMkInfoInner(int nSerial, BOOL bDumy = FALSE);

	CString GetTimeIts();

	int GetAoiUpAutoStep();
	void SetAoiUpAutoStep(int nStep);

	int GetAoiDnAutoStep();
	void SetAoiDnAutoStep(int nStep);
	void ShowMsgBox();
	BOOL FinalCopyItsFiles();

	// DTS
	BOOL GetDtsPieceOut(int nSerial, int* pPcsOutIdx, int& nTotPcsOut);
	CString GetCurrentDBName();
	int GetAoiUpCamMstInfo();
	int GetAoiDnCamMstInfo();

	int GetMkStAuto();
	void SetMkStAuto();
	BOOL GetMkStSignal();
	void LoadSerial();

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnInitialUpdate(); // 생성 후 처음 호출되었습니다.

// 구현입니다.
public:
	virtual ~CGvisR2R_PunchView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnDlgInfo(WPARAM wParam, LPARAM lParam);
};

#ifndef _DEBUG  // GvisR2R_PunchView.cpp의 디버그 버전
inline CGvisR2R_PunchDoc* CGvisR2R_PunchView::GetDocument() const
   { return reinterpret_cast<CGvisR2R_PunchDoc*>(m_pDocument); }
#endif

