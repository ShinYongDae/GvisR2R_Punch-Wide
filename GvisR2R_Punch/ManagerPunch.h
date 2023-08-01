#pragma once
#include "Global/GlobalDefine.h"
#include "Process/ThreadTask.h"

#include "Device/Motion.h"
#include "Device/Light.h"
#include "Device/Smac.h"
#include "Device/Vision.h"
#include "Device/MpDevice.h"
#include "Device/SR1000W.h"
#include "Device/Engrave.h"
#include "Device/Dts.h"

#include "Process/PtAlign.h"
#include "Process/ThreadTask.h"


#define MAX_THREAD_MGR_MK			2

// CManagerPunch

class CManagerPunch : public CWnd
{
	DECLARE_DYNAMIC(CManagerPunch)

	CWnd* m_pParent;
	BOOL m_bCycleStop;
	BOOL m_bTIM_MPE_IO;

	DWORD m_dwSetDlySt[10], m_dwSetDlyEd[10];
	DWORD m_dwSetDlySt0[10], m_dwSetDlyEd0[10];
	DWORD m_dwSetDlySt1[10], m_dwSetDlyEd1[10];

	BOOL HwInit();
	void HwKill();
	void InitThread();
	void KillThread();

public:
	CManagerPunch(CWnd* pParent = NULL);
	virtual ~CManagerPunch();

	CMpDevice* m_pMpe;
	CMotion* m_pMotion;
	CLight* m_pLight;
	CSmac* m_pVoiceCoil[2];	// [0] : LeftCam , [1] : RightCam
#ifdef USE_VISION
	CVision* m_pVision[2];	// [0] : LeftCam , [1] : RightCam
	CVision* m_pVisionInner[2];	// [0] : LeftCam , [1] : RightCam
#endif
	//CPtAlign m_Align[2];	// [0] : LeftCam , [1] : RightCam

	CEngrave* m_pEngrave;
	CSr1000w* m_pSr1000w;
	CDts* m_pDts;

	BOOL m_bCam, m_bReview;
	double m_dNextTarget[MAX_AXIS];

	BOOL m_bProbDn[2]; // 좌/우 .
	//double m_dEnc[MAX_AXIS];
	BOOL m_bAuto, m_bManual, m_bOneCycle;
	int m_nStepMk[4], m_nMkPcs[4]; 	// [0] Auto-Left, [1] Auto-Right, [2] Manual-Left, [3] Manual-Right  ; m_nStepMk(마킹Sequence), nMkOrderIdx(마킹한 count)

	BOOL m_bDoMk[2];			// [nCam] : TRUE(Punching), FALSE(Stop Punching)
	BOOL m_bDoneMk[2];			// [nCam] : TRUE(Punching 완료), FALSE(Punching 미완료)
	BOOL m_bReMark[2];			// [nCam] : TRUE(Punching 다시시작), FALSE(pass)
	int m_nTotMk[2], m_nCurMk[2]; // [0]: 좌 MK, [1]: 우 MK
	int m_nPrevTotMk[2], m_nPrevCurMk[2]; // [0]: 좌 MK, [1]: 우 MK
	BOOL m_bTHREAD_MK[4];	// [0] Auto-Left, [1] Auto-Right, [2] Manual-Left, [3] Manual-Right
	BOOL m_bStopFromThread, m_bBuzzerFromThread;

	CString m_sGet2dCodeLot;
	int m_nGet2dCodeSerial;
	BOOL m_bSwStopNow;
	BOOL m_bSwRun, m_bSwRunF;
	int m_nMkStrip[2][4]; // [nCam][nStrip] - [좌/우][] : 스트립에 펀칭한 피스 수 count
	BOOL m_bRejectDone[2][MAX_STRIP_NUM]; // Shot[2], Strip[4] - [좌/우][] : 스트립에 펀칭한 피스 수 count가 스트립 폐기 설정수 완료 여부 
	double m_dEnc[MAX_AXIS], m_dTarget[MAX_AXIS];
	int m_nStepElecChk;
	BOOL m_bCollision[2], m_bPriority[4];


	// [Thread]
	DWORD m_dwThreadTick[MAX_THREAD_MGR_MK];
	BOOL m_bThread[MAX_THREAD_MGR_MK];
	CThreadTask m_Thread[MAX_THREAD_MGR_MK];

	static UINT ThreadProc0(LPVOID lpContext); // DoMark0(), DoMark1()
	static UINT ThreadProc1(LPVOID lpContext); // ChkCollision()

	void SetPriority();
	void ResetPriority();
	void BuzzerFromThread(BOOL bOn, int nCh = 0);


	BOOL DoElecChk(CString &sRst);  // TRUE: bDone , FALSE: Doing
	void InitAuto(BOOL bInit = TRUE);

	//===> PLC
	void InitPLC();
	void InitIoWrite();
	BOOL MpeWrite(CString strRegAddr, long lData, BOOL bCheck = FALSE);
	void GetMpeIO();
	void GetMpeSignal();
	void GetMpeData();
	void GetPlcParam();
	void SetPlcParam();
	void Winker(int nId, int nDly = 20); // 0:Ready, 1:Reset, 2:Run, 3:Stop
	void IoWrite(CString sMReg, long lData);
	void MonPlcSignal();
	void MonPlcAlm();
	void MonDispMain();
	void PlcAlm(BOOL bMon, BOOL bClr);
	void FindAlarm();
	void ResetMonAlm();
	void ClrAlarm();
	void ResetClear();
	void DoIO();
	void Ink(BOOL bOn = TRUE);
	void MoveAoi(double dOffset);
	void MoveMk(double dOffset);
	void CycleStop(BOOL bStop=TRUE);
	BOOL IsCycleStop();

	//===> Motion
	void InitMotion();
	void InitIO();
	BOOL InitAct();
	void SetMainMc(BOOL bOn);
	BOOL IsMotionEnable();
	void Move0(CfPoint pt, BOOL bCam = FALSE);
	void Move1(CfPoint pt, BOOL bCam = FALSE);
	BOOL IsMoveDone();
	BOOL IsMoveDone0();
	BOOL IsMoveDone1();
	BOOL MovePinPos();
	BOOL MoveAlign0(int nPos);
	BOOL MoveAlign1(int nPos);
	BOOL TwoPointAlign0(int nPos, BOOL bDraw = FALSE);
	BOOL TwoPointAlign1(int nPos, BOOL bDraw = FALSE);
	BOOL FourPointAlign0(int nPos, BOOL bDraw = FALSE);
	BOOL FourPointAlign1(int nPos, BOOL bDraw = FALSE);
	BOOL OnePointAlign(CfPoint &ptPnt);
	BOOL TwoPointAlign();
	BOOL TwoPointAlign(int nPos);
	BOOL IsHomeDone(int nMsId);
	void MoveInitPos0(BOOL bWait = TRUE);
	void MoveInitPos1(BOOL bWait = TRUE);
	void MoveMkEdPos1();
	BOOL IsInitPos0();
	BOOL IsInitPos1();
	BOOL IsMkEdPos1();
	void MoveMkInitPos();
	void MoveMk0InitPos();
	void MoveMk1InitPos();
	void ResetMotion();
	void ResetMotion(int nMsId);
	BOOL IsRunAxisX();
	BOOL IsMotionDone(int nAxisId);
	void EStop();
	void SetAlignPos();
	void SetAlignPosUp();
	void SetAlignPosDn();
	void ChkTempStop(BOOL bChk);
	BOOL MoveMeasPos(int nId); // Elec Chk
	BOOL IsPinPos0();
	BOOL IsPinPos1();
	void GetEnc();

	//===> Vision
	BOOL StartLive();
	BOOL StartLive0();
	BOOL StartLive1();
	BOOL StopLive();
	BOOL StopLive0();
	BOOL StopLive1();
	BOOL ChkLightErr();
	BOOL SaveMk0Img(int nMkPcsIdx);
	BOOL SaveMk1Img(int nMkPcsIdx);

	//===> Punching
	void DoMark0();
	void DoMark1();
	void DoMark0All();
	void DoMark1All();
	void DoAllMk(int nCam);
	void StopAllMk();
	BOOL IsMk0Done();
	BOOL IsMk1Done();
	BOOL IsOnMarking0();
	BOOL IsOnMarking1();
	void DoMark0Its();
	void DoMark1Its();
	void DoReject0();
	void DoReject1();
	void CntMk();
	void SetReMk(BOOL bMk0 = FALSE, BOOL bMk1 = FALSE);
	BOOL IsReMk();
	BOOL InitMk();
	void Mk0();
	void Mk1();
	BOOL SetMk(BOOL bRun = TRUE);
	BOOL SetMkIts(BOOL bRun = TRUE);	// Marking Start
	void SetReject();
	BOOL IsNoMk();
	BOOL IsNoMk0();
	BOOL IsNoMk1();
	BOOL ChkCollision();
	BOOL ChkCollision(int nAxisId, double dTgtPosX);
	BOOL ChkCollision(int nAxisId, double dTgtPosX, double dTgtNextPos);
	BOOL IsReview();
	BOOL IsReview0();
	BOOL IsReview1();
	BOOL IsVs();
	BOOL IsVsUp();
	BOOL IsVsDn();

	//===> TCP/IP
	BOOL TcpIpInit();
	void DtsInit();

	//==>Etc
	int MsgBox(CString sMsg, int nThreadIdx = 0, int nType = MB_OK, int nTimOut = DEFAULT_TIME_OUT, BOOL bEngave = TRUE);		// SyncMsgBox

	void DoModeSel();		// SliceIo[0]
	void DoMainSw();		// SliceIo[0]
	void DoMkSens();		// SliceIo[2]
	void DoAoiBoxSw();		// SliceIo[3]
	void DoEmgSens();		// SliceIo[4]
	void DoSignal();		// SliceIo[5]

	void DoUcBoxSw();		// MpeIo[0]
	void DoUcSens1();		// MpeIo[1]
	void DoUcSens2();		// MpeIo[2]
	void DoUcSens3();		// MpeIo[3]
	void DoRcBoxSw();		// MpeIo[4]
	void DoRcSens1();		// MpeIo[5]
	void DoRcSens2();		// MpeIo[6]
	void DoRcSens3();		// MpeIo[7]

	void DoEngraveSens();	// MpeIo[28]

	void DoBoxSw();
	void DoEmgSw();
	void DoSens();
	void DoAuto();
	BOOL DoAutoGetLotEndSignal();
	void DoInterlock();

	void DoSaftySens();
	void DoDoorSens();

	void SwJog(int nAxisID, int nDir, BOOL bOn = TRUE);
	BOOL ChkSaftySen();
	unsigned long ChkDoor(); // 0: All Closed , Open Door Index : Doesn't all closed. (Bit3: F, Bit2: L, Bit1: R, Bit0; B)
	void Buzzer(BOOL bOn, int nCh = 0);
	CString GetAoiUpAlarmRestartMsg();
	void ChkReTestAlarmOnAoiUp();
	CString GetAoiDnAlarmRestartMsg();
	void ChkReTestAlarmOnAoiDn();
	void DoAutoEng();
	BOOL IsPinData();
	void SetBufInitPos(double dPos);
	void SetBufHomeParam(double dVel, double dAcc);
	void SetLight(int nVal = -1);
	void SetLight2(int nVal = -1);
	void Stop();
	void DispMain(CString sMsg, COLORREF rgb = RGB(0, 255, 0));
	void SetDelay(int mSec, int nId = 0);
	void SetDelay0(int mSec, int nId = 0);
	void SetDelay1(int mSec, int nId = 0);
	BOOL WaitDelay(int nId = 0);				// F:Done, T:On Waiting....
	BOOL WaitDelay0(int nId = 0);				// F:Done, T:On Waiting....
	BOOL WaitDelay1(int nId = 0);				// F:Done, T:On Waiting....
	BOOL GetDelay(int &mSec, int nId = 0);		// F:Done, T:On Waiting....
	BOOL GetDelay0(int &mSec, int nId = 0);		// F:Done, T:On Waiting....
	BOOL GetDelay1(int &mSec, int nId = 0);		// F:Done, T:On Waiting....
	int WaitRtnVal(int nThreadIdx = 1);
	BOOL IsAuto();
	void UpdateRst();
	void ChkEmg();
	void StopFromThread();
	//BOOL Get2dCode(CString &sLot, int &nSerial);
	BOOL CheckConectionSr1000w();
	int GetTotDefPcs(int nSerial);
	void DoAtuoGetEngStSignal();
	void DoAtuoGet2dReadStSignal();


	void SwAoiRelation();
	void SwAoiTblBlw();
	void SwAoiFdVac();
	void SwAoiTqVac();

	afx_msg LRESULT wmClientReceived(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT wmClientReceivedSr(WPARAM wParam, LPARAM lParam);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};


