
// GvisR2R_PunchDoc.h : CGvisR2R_PunchDoc 클래스의 인터페이스
//


#pragma once

#include "Global/GlobalDefine.h"
#include "Global/MySpec.h"
#include "Process/MyFile.h"

#define PATH_LOG		_T("C:\R2RSet\Log")
#define PATH_LOG_AUTO	_T("C:\R2RSet\Log\Auto")

class CGvisR2R_PunchDoc : public CDocument
{
	void DeleteFileInFolder(CString sPathDir);
	int CheckPath(CString strPath);
	void StringToChar(CString str, char* pCh); // char* returned must be deleted... 

protected: // serialization에서만 만들어집니다.
	CGvisR2R_PunchDoc();
	DECLARE_DYNCREATE(CGvisR2R_PunchDoc)

// 특성입니다.
public:
	BOOL m_bOffLogAuto;
	BOOL m_bUpdateForNewJob[2]; // [0]: Up, [1]: Dn
	int m_nDelayShow;

	// R2R Y Meander Adjusting....
	BOOL m_bUseDts, m_bUseIts;
	BOOL m_bUseRTRYShiftAdjust;
	double m_dRTRShiftVal;
	double m_dShiftAdjustRatio;

	CMyFile *m_pFile;
	CMySpec *m_pSpecLocal;
	stMkIo MkIo;
	stWorkingInfo WorkingInfo;
	stStatus Status;								// Status 입력신호
	stBtnStatus BtnStatus;
	stMenu01Status Menu01Status;
	stMpeIoWrite m_pIo[TOT_M_IO];

	int m_nPrevSerial;
	int AoiDummyShot[2]; // [Up/Dn]

	CString m_strUserNameList;
	int m_nSliceIo;

	int m_nMpeIo, m_nMpeIb;
	unsigned short *m_pMpeIo, *m_pMpeIb;
	unsigned short *m_pMpeIoF, *m_pMpeIF;

	int m_nMpeSignal;
	unsigned short *m_pMpeSignal;

	int m_nMpeData;
	long **m_pMpeData;

	CString m_sPassword;

	CString m_sAlmMsg, m_sPrevAlmMsg;
	CString m_sIsAlmMsg;

	CString m_sMsgBox, m_sPrevMsgBox;
	CString m_sIsMsgBox;

	stListBuf m_ListBuf[2]; // [0]:AOI-Up , [1]:AOI-Dn

	BOOL m_bNewLotShare[2]; // [0]:AOI-Up , [1]:AOI-Dn
	BOOL m_bNewLotBuf[2]; // [0]:AOI-Up , [1]:AOI-Dn
	int m_nLotLastShot;
	BOOL m_bDoneChgLot;

	char m_cBigDefCode[MAX_DEF];
	char m_cSmallDefCode[MAX_DEF];

	// Sapp3 code
	int m_nSapp3Code[10];

	CString m_sElecChk;

	int m_nOrderNum, m_nShotNum, m_nTestOrderNum, m_nTestShotNum;
	CString m_sOrderNum, m_sShotNum;
	CString m_sTestOrderNum, m_sTestShotNum;

	// Communcation
	int m_nBad[3], m_nGood[3];					// [0]: Up, [1]: Dn, [2]: Total
	double m_dBadRatio[3], m_dGoodRatio[3];		// [0]: Up, [1]: Dn, [2]: Total
	int m_nTestNum[3];							// [0]: Up, [1]: Dn, [2]: Total
	CString m_sLotStTime, m_sLotEdTime, m_sLotRunTime;
	double m_dStripRatio[3][7];					// [3]: Up/Dn/ALL , [7]: Strip 1~6 , ALL
	double m_dTotRatio[3][7];					// [3]: Up/Dn/ALL , [7]: Strip 1~6 , ALL
	int m_nDef[MAX_DEF];						// [3]: Up/Dn/ALL
	double m_dMkBuffCurrPos;

// 작업입니다.
public:
	void Log(CString strMsg, int nType = 0);
	CString GetProcessNum();
	BOOL LoadWorkingInfo();
	void SaveWorkingInfo();

	BOOL LoadIoInfo();
	BOOL LoadSignalInfo();
	BOOL LoadDataInfo();
	BOOL LoadMySpec();
	void UpdateData();
	int GetLastSerial();
	int GetLotSerial();
	void SetOnePnlLen(double dLen);
	double GetOnePnlLen();
	void SetOnePnlVel(double dVel);
	void SetFdJogVel(double dVel);
	void SetFdJogAcc(double dVel);
	double GetOnePnlVel();
	void SetOnePnlAcc(double dAcc);
	double GetOnePnlAcc();
	void SetAoiFdPitch(double dPitch);
	double GetAoiFdPitch();
	void SetMkFdPitch(double dPitch);
	double GetMkFdPitch();
	void SetFdErrLmt(double dLmt);
	double GetFdErrLmt();
	void SetFdErrRng(double dRng);
	double GetFdErrRng();
	void SetFdErrNum(int nNum);
	int GetFdErrNum();
	void SetBufInitPos(double dPos);
	void SetBufInitPos(double dVel, double dAcc);
	void SetEngBufInitPos(double dPos);
	double GetBuffInitPos();
	void SetAoiMkDist(double dPos);
	double GetAoiMkDist();
	void SetAoiAoiDist(int nShot);
	int GetAoiAoiDist();
	void SaveLotTime(DWORD dwStTick);

	void SetWaitPos1(double dPos);
	double GetWaitPos1();
	void SetWaitVel1(double dVel);
	double GetWaitVel1();
	void SetWaitAcc1(double dAcc);
	double GetWaitAcc1();

	void SetMarkingPos1(double dPos);
	double GetMarkingPos1();
	void SetMarkingVel1(double dVel);
	double GetMarkingVel1();
	void SetMarkingAcc1(double dAcc);
	double GetMarkingAcc1();
	void SetMarkingToq1(double dToq);
	void SetMarkingToq1Offset(double dToqOffset);
	double GetMarkingToq1();

	void SetPosX1_1(double dPosX);
	double GetPosX1_1();
	void SetPosY1_1(double dPosY);
	double GetPosY1_1();

	void SetPosX1_2(double dPosX);
	double GetPosX1_2();
	void SetPosY1_2(double dPosY);
	double GetPosY1_2();

	void SetPosX1_3(double dPosX);
	double GetPosY1_3();
	void SetPosY1_3(double dPosY);
	double GetPosX1_3();

	void SetPosX1_4(double dPosX);
	double GetPosX1_4();
	void SetPosY1_4(double dPosY);
	double GetPosY1_4();

	void SetAverDist1(double dDist);
	double GetAverDist1();

	void SetWaitPos2(double dPos);
	double GetWaitPos2();
	void SetWaitVel2(double dVel);
	double GetWaitVel2();
	void SetWaitAcc2(double dAcc);
	double GetWaitAcc2();

	void SetMarkingPos2(double dPos);
	double GetMarkingPos2();
	void SetMarkingVel2(double dVel);
	double GetMarkingVel2();
	void SetMarkingAcc2(double dAcc);
	double GetMarkingAcc2();
	void SetMarkingToq2(double dToq);
	void SetMarkingToq2Offset(double dToqOffset);
	double GetMarkingToq2();

	void SetPosX2_1(double dPosX);
	double GetPosX2_1();
	void SetPosY2_1(double dPosY);
	double GetPosY2_1();

	void SetPosX2_2(double dPosX);
	double GetPosX2_2();
	void SetPosY2_2(double dPosY);
	double GetPosY2_2();

	void SetPosX2_3(double dPosX);
	double GetPosY2_3();
	void SetPosY2_3(double dPosY);
	double GetPosX2_3();

	void SetPosX2_4(double dPosX);
	double GetPosX2_4();
	void SetPosY2_4(double dPosY);
	double GetPosY2_4();

	void SetAverDist2(double dDist);
	double GetAverDist2();

	void SetTotalReelDist(double dDist);
	double GetTotalReelDist();
	void SetSeparateDist(double dDist);
	double GetSeparateDist();
	void SetCuttingDist(double dDist);
	double GetCuttingDist();
	void SetStopDist(double dDist);
	double GetStopDist();

	void SetAOIToq(double dToq);
	double GetAOIToq();
	void SetMarkingToq(double dToq);
	double GetMarkingToq();
	void SetEngraveToq(double dToq);
	double GetEngraveToq();

	BOOL MakeMkDir(stModelInfo stInfo);
	BOOL MakeMkDir(CString sModel, CString sLot, CString sLayer);
	BOOL MakeMkDir();
	BOOL Shift2Mk(int nSerial);
	void SetLastSerial(int nSerial);								// 릴맵 텍스트 파일의 수율정보를 업데이트함.
	void SetCompletedSerial(int nSerial);
	BOOL ChkLotEnd(CString sPath);
	BOOL IsPinMkData();
	BOOL IsPinData();

	double GetFdJogVel();
	double GetFdJogAcc();
	void SetModelInfoUp();
	void SetModelInfoDn();
	BOOL GetAoiUpOffset(CfPoint &OfSt);
	BOOL GetAoiDnOffset(CfPoint &OfSt);

	CString GetMin(int nDlgId, int nCtrlId);
	CString GetMax(int nDlgId, int nCtrlId);

	int GetLastShotMk();	// m_pDlgFrameHigh에서 얻거나 없으면, sPathOldFile폴더의 ReelMapDataDn.txt에서 _T("Info"), _T("Marked Shot") 찾음.

	int GetMkCntL();
	void AddMkCntL();
	void SaveMkCntL();
	void SetMkCntL(int nNum);
	int GetMkLimitL();
	void SetMkLimitL(int nNum = 31536000); // 1[year] = 31536000[sec]
	int GetMkCntR();
	void AddMkCntR();
	void SaveMkCntR();
	void SetMkCntR(int nNum);
	int GetMkLimitR();
	void SetMkLimitR(int nNum = 31536000); // 1[year] = 31536000[sec]

	void UpdateProcessNum(CString sProcessNum);

	void SetProbWaitPos(int nProb, double dPos);
	double GetProbWaitPos(int nProb);
	void SetProbWaitVel(int nProb, double dVel);
	double GetProbWaitVel(int nProb);
	void SetProbWaitAcc(int nProb, double dAcc);
	double GetProbWaitAcc(int nProb);
	void SetProbingPos(int nProb, double dPos);
	double GetProbingPos(int nProb);
	void SetProbingVel(int nProb, double dVel);
	double GetProbingVel(int nProb);
	void SetProbingAcc(int nProb, double dAcc);
	double GetProbingAcc(int nProb);
	void SetProbingToq(int nProb, double dToq);
	double GetProbingToq(int nProb);
	void SetProbPosX(int nProb, double dPosX);
	void SetProbPosY(int nProb, double dPosY);
	double GetProbPosX(int nProb);
	double GetProbPosY(int nProb);

	void SetProbingThreshold(double dThreshold);
	double GetProbingThreshold();

	void SetStripRejectMkNum(int nNum);
	int GetStripRejectMkNum();

	void WriteElecData(CString sData);

	int GetLastShotEngrave();
	void SetEngraveReaderDist(double dLen);
	double GetEngraveReaderDist();
	void SetEngraveAoiDist(double dLen);
	double GetEngraveAoiDist();
	void SetAoiReaderDist(double dLen);
	double GetAoiReaderDist();
	void SetMkReaderDist(double dLen);
	double GetMkReaderDist();
	void SetOffsetInitPos(double dLen);
	double GetOffsetInitPos();
	void Set2DReaderPosMoveVel(double dVel);
	void Set2DReaderPosMoveAcc(double dAcc);
	double Get2DReaderPosMoveVel();
	double Get2DReaderPosMoveAcc();

	void SetEngraveFdPitch(double dPitch);
	double GetEngraveFdPitch();
	void SetEngraveFdErrLmt(double dLmt);
	double GetEngraveFdErrLmt();
	void SetEngraveFdErrRng(double dRng);
	double GetEngraveFdErrRng();
	void SetEngraveFdErrNum(int nNum);
	int GetEngraveFdErrNum();
	double GetEngraveBuffInitPos();

	BOOL DirectoryExists(LPCTSTR szPath);
	void SetTestMode(int nMode);
	int GetTestMode();
	BOOL GetEngOffset(CfPoint &OfSt);

	void SetEngItsCode(CString sItsCode);

	void SetCurrentInfoSignal(int nIdxSig, BOOL bOn);
	BOOL GetCurrentInfoSignal(int nIdxSig);
	void SetLastSerialEng(int nSerial);
	BOOL GetCurrentInfoEng();
	int GetCurrentInfoEngShotNum();
	void SetCurrentInfoBufUpTot(int nTotal);
	void SetCurrentInfoBufUp(int nIdx, int nData);
	void SetCurrentInfoBufDnTot(int nTotal);
	void SetCurrentInfoBufDn(int nIdx, int nData);

	CString GetMonDispMain();
	void SetMonDispMain(CString sDisp);
	void GetMkMenu01();
	void SetMkMenu01(CString sMenu, CString sItem, CString sData);
	void GetMkMenu03();
	void GetMkMenu03Main();
	void SetMkMenu03(CString sMenu, CString sItem, BOOL bOn);
	void GetMkInfo();
	void SetMkInfo(CString sMenu, CString sItem, BOOL bOn);
	void SetMkInfo(CString sMenu, CString sItem, CString sData);

	char* StrToChar(CString str);
	void StrToChar(CString str, char* pCh);
	void WriteChangedModel();

	int m_nEjectBufferLastShot;
	BOOL m_bDebugGrabAlign;
	void DestroyDoc();

// 재정의입니다.
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 구현입니다.
public:
	virtual ~CGvisR2R_PunchDoc();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 검색 처리기에 대한 검색 콘텐츠를 설정하는 도우미 함수
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
