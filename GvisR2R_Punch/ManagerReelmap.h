#pragma once

#include "Process/ReelMap.h"
#include "Process/DataMarking.h"
#include "Global/Yield.h"
#include "Process/CamMaster.h"
#include "Process/MyFile.h"

// CManagerReelmap

#define MAX_THREAD_MGR_RMAP			33

class CManagerReelmap : public CWnd
{
	DECLARE_DYNAMIC(CManagerReelmap)
	CWnd*			m_pParent;

	CReelMap* m_pReelMapDisp;
	CReelMap* m_pReelMap;
	CReelMap *m_pReelMapUp, *m_pReelMapDn, *m_pReelMapAllUp, *m_pReelMapAllDn;
	CDataMarking* m_pPcr[MAX_PCR][MAX_PCR_PNL];	//릴맵화면표시를 위한 데이터	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
	CYield m_Yield[3]; // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-All
	CString *pMkInfo;
	CMyFile *m_pFile;

	// 내층 작업한 데이터구조  ====================================================================
	CCamMaster m_MasterInner[2];
	CReelMap* m_pReelMapInner;
	CReelMap *m_pReelMapInnerUp, *m_pReelMapInnerDn, *m_pReelMapInnerAllUp, *m_pReelMapInnerAllDn;
	CDataMarking* m_pPcrInner[MAX_PCR][MAX_PCR_PNL];	//릴맵화면표시를 위한 데이터	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
	CReelMap* m_pReelMapIts;
	CDataMarking* m_pPcrIts[MAX_PCR_PNL];				//릴맵화면표시를 위한 데이터	// 내외층 merging
	//=============================================================================================

	// For ITS
	BOOL m_bEngDualTest;
	CString m_sItsCode;
	CString m_sEngLotNum, m_sEngProcessNum;
	CString m_sEngModel, m_sEngLayerUp;
	CString m_sEngLayerDn;
	int m_nWritedItsSerial;

	void Init();
	void InitVal();
	void InitMstData();

public:
	CManagerReelmap(CWnd* pParent = NULL);
	virtual ~CManagerReelmap();

	CString GetMkInfo(int nSerial, int nMkPcs); // return Cam0 : "Serial_Strip_Col_Row"
	CfPoint GetMkPnt(int nSerial, int nMkPcs); // pcr 시리얼, pcr 불량 피스 읽은 순서 인덱스
	CfPoint GetMkPnt(int nMkPcs); // pcr 시리얼, pcr 불량 피스 읽은 순서 인덱스
	int GetMkStripIdx(int nDefPcsId); // 0 : Fail , 1~4 : Strip Idx
	int GetMkStripIdx(int nSerial, int nMkPcs); // 0 : Fail , 1~4 : Strip Idx

	void DuplicateRmap();
	void DuplicateRmap(int nRmap);
	void SetMkPnt(int nCam);	// Pin위치에 의한 정렬.
	BOOL GetCurrentInfoEng();
	void UpdateData();

	// [Reelmap]
	CCamMaster m_Master[2];

	BOOL InitReelmap();
	BOOL InitReelmapUp();
	BOOL InitReelmapDn();

	BOOL UpdateReelmap(int nSerial);
	BOOL UpdateReelmapInner(int nSerial);

	BOOL RemakeReelmap();
	BOOL IsDoneRemakeReelmap();
	BOOL RemakeReelmapInner();
	BOOL IsDoneRemakeReelmapInner();

	void ClrFixPcs();

	BOOL ReloadReelmap();
	BOOL IsDoneReloadReelmap(int& nProc);
	void OpenReelmap();
	void OpenReelmapUp();
	void OpenReelmapDn();

	BOOL OpenReelmapFromBuf(int nSerial);
	void SetPathAtBuf();
	void SetPathAtBufUp();
	void SetPathAtBufDn();
	BOOL LoadPcrFromBuf();
	void SetLotSt();
	void SetLotEd();

	void SetReelmap(int nDir = ROT_NONE);
	CString GetCamPxlRes();
	//	int LoadPCR(int nSerial, BOOL bFromShare = FALSE);		// return : 2(Failed), 1(정상), -1(Align Error, 노광불량), -2(Lot End)
	int LoadPCR0(int nSerial, BOOL bFromShare = FALSE);		// return : 2(Failed), 1(정상), -1(Align Error, 노광불량), -2(Lot End)
	int LoadPCR1(int nSerial, BOOL bFromShare = FALSE);		// return : 2(Failed), 1(정상), -1(Align Error, 노광불량), -2(Lot End)
	int LoadPCRUp(int nSerial, BOOL bFromShare = FALSE);	// return : 2(Failed), 1(정상), -1(Align Error, 노광불량), -2(Lot End)
	int LoadPCRDn(int nSerial, BOOL bFromShare = FALSE);	// return : 2(Failed), 1(정상), -1(Align Error, 노광불량), -2(Lot End)
	int LoadPCRAllUp(int nSerial, BOOL bFromShare = FALSE);	// return : 2(Failed), 1(정상), -1(Align Error, 노광불량), -2(Lot End)
	int LoadPCRAllDn(int nSerial, BOOL bFromShare = FALSE);	// return : 2(Failed), 1(정상), -1(Align Error, 노광불량), -2(Lot End)

	int GetPcrIdx(int nSerial, BOOL bNewLot = FALSE);
	int GetPcrIdx0(int nSerial, BOOL bNewLot = FALSE); // Up - 릴맵화면 표시를 위한 Display buffer의 Shot 인덱스
	int GetPcrIdx1(int nSerial, BOOL bNewLot = FALSE); // Dn - 릴맵화면 표시를 위한 Display buffer의 Shot 인덱스

	BOOL GetAoiDnInfo(int nSerial, int *pNewLot = NULL, BOOL bFromBuf = FALSE); // TRUE: CHANGED, FALSE: NO CHANGED
	BOOL GetAoiUpInfo(int nSerial, int *pNewLot = NULL, BOOL bFromBuf = FALSE); // TRUE: CHANGED, FALSE: NO CHANGED
	BOOL GetAoiInfoUp(int nSerial, int *pNewLot = NULL, BOOL bFromBuf = FALSE); // TRUE: CHANGED, FALSE: NO CHANGED
	BOOL GetAoiInfoDn(int nSerial, int *pNewLot = NULL, BOOL bFromBuf = FALSE); // TRUE: CHANGED, FALSE: NO CHANGED

	void InitPcr();
	int GetIdxPcrBuf(int nSerial);
	int GetIdxPcrBufUp(int nSerial);
	int GetIdxPcrBufDn(int nSerial);

	BOOL IsFixPcsUp(int nSerial);
	BOOL IsFixPcsDn(int nSerial);
	void SetFixPcs(int nSerial);

	BOOL GetPcrInfo(CString sPath, stModelInfo &stInfo);

	void ClrPcr();
	int CopyPcrAll();
	int CopyPcrUp();
	int CopyPcrDn();
	void DelPcrAll();
	void DelPcrUp();
	void DelPcrDn();
	void DelSharePcr();
	void DelSharePcrUp();
	void DelSharePcrDn();

	void UpdateYieldOnThread(int nSerial);
	void UpdateYield(int nSerial);
	void UpdateYieldUp(int nSerial);
	void UpdateYieldDn(int nSerial);
	void UpdateYieldAllUp(int nSerial);
	void UpdateYieldAllDn(int nSerial);
	void UpdateYieldInnerUp(int nSerial);
	void UpdateYieldInnerDn(int nSerial);
	void UpdateYieldInnerAllUp(int nSerial);
	void UpdateYieldInnerAllDn(int nSerial);
	void UpdateYieldIts(int nSerial);
	void UpdateYieldOnRmap();

	BOOL CopyDefImg(int nSerial);
	BOOL CopyDefImg(int nSerial, CString sNewLot);
	BOOL CopyDefImgUp(int nSerial, CString sNewLot = _T(""));
	BOOL CopyDefImgDn(int nSerial, CString sNewLot = _T(""));
	void MakeImageDirUp(int nSerial);
	void MakeImageDirDn(int nSerial);

	// PCS 인덱스를 예전방식의 인덱스로 변환함.
	int MirrorLR(int nPcsId); // 좌우 미러링
	int Rotate180(int nPcsId);// 180도 회전 = 좌우 미러링 & 상하 미러링
	int MirrorUD(int nPcsId); // 상하 미러링

	int SearchFirstShotOnIts();
	BOOL GetItsSerialInfo(int nItsSerial, BOOL &bDualTest, CString &sLot, CString &sLayerUp, CString &sLayerDn, int nOption = 0);		// 내층에서의 ITS 시리얼의 정보
	int GetLastItsSerial();																											// 내외층 머징된 릴맵 데이타의 Last 시리얼
	CString GetItsFolderPath();
	CString GetItsTargetFolderPath();
	CString GetItsReelmapPath();
	BOOL GetInnerFolderPath(int nItsSerial, CString  &sUp, CString &sDn);

	char* StrToChar(CString str);
	void StrToChar(CString str, char* pCh);

	// For MODE_OUTER ============================================
	int LoadPCRAllUpInner(int nSerial, BOOL bFromShare = FALSE);	// return : 2(Failed), 1(정상), -1(Align Error, 노광불량), -2(Lot End)
	int LoadPCRAllDnInner(int nSerial, BOOL bFromShare = FALSE);	// return : 2(Failed), 1(정상), -1(Align Error, 노광불량), -2(Lot End)
	int LoadPCRUpInner(int nSerial, BOOL bFromShare = FALSE);	// return : 2(Failed), 1(정상), -1(Align Error, 노광불량), -2(Lot End)
	int LoadPCRDnInner(int nSerial, BOOL bFromShare = FALSE);	// return : 2(Failed), 1(정상), -1(Align Error, 노광불량), -2(Lot End)

	
	// ITS
	BOOL WriteReelmapIts();
	void SetInnerPathAtBufUp();
	void SetInnerPathAtBufDn();

	int LoadPCRIts(int nSerial, BOOL bFromShare = FALSE);	// return : 2(Failed), 1(정상), -1(Align Error, 노광불량), -2(Lot End)
	void LoadPCRIts11(int nSerial); // 11 -> 외층 : 양면, 내층 : 양면
	void LoadPCRIts10(int nSerial); // 10 -> 외층 : 양면, 내층 : 단면
	void LoadPCRIts01(int nSerial); // 11 -> 외층 : 단면, 내층 : 양면
	void LoadPCRIts00(int nSerial); // 10 -> 외층 : 단면, 내층 : 단면

	int GetErrCode0Its(int nSerial); // 1(정상), -1(Align Error, 노광불량), -2(Lot End)
	int GetErrCodeUp0Its(int nSerial); // 1(정상), -1(Align Error, 노광불량), -2(Lot End)
	int GetErrCodeDn0Its(int nSerial); // 1(정상), -1(Align Error, 노광불량), -2(Lot End)

	int GetErrCode1Its(int nSerial); // 1(정상), -1(Align Error, 노광불량), -2(Lot End)
	int GetErrCodeUp1Its(int nSerial); // 1(정상), -1(Align Error, 노광불량), -2(Lot End)
	int GetErrCodeDn1Its(int nSerial); // 1(정상), -1(Align Error, 노광불량), -2(Lot End)

	int GetTotDefPcsIts(int nSerial);
	int GetTotDefPcsUpIts(int nSerial);
	int GetTotDefPcsDnIts(int nSerial);

	CfPoint GetMkPntIts(int nSerial, int nMkPcs); // pcr 시리얼, pcr 불량 피스 읽은 순서 인덱스

	int GetMkStripIdxIts(int nSerial, int nMkPcs); // 0 : Fail , 1~4 : Strip Idx


	BOOL InitReelmapInner();
	BOOL InitReelmapInnerUp();
	BOOL InitReelmapInnerDn();
	void SetReelmapInner(int nDir = ROT_NONE);
	CString GetItsTargetPath(int nSerial, int nLayer);	// RMAP_UP, RMAP_DN, RMAP_INNER_UP, RMAP_INNER_DN
	int GetItsDefCode(int nDefCode);				// return to [Sapp3Code]
	BOOL MakeItsDir(CString sModel, CString sLot, CString sLayer);

	int GetAoiUpCamMstInfo();
	int GetAoiDnCamMstInfo();

	BOOL ReloadReelmapInner();
	BOOL ReloadReelmapInner(int nSerial);
	BOOL IsDoneReloadReelmapInner(int& nProc);
	void OpenReelmapInner();
	void OpenReelmapInnerUp();
	void OpenReelmapInnerDn();

	void ReloadReelmapUp();
	void ReloadReelmapAllUp();
	void ReloadReelmapDn();
	void ReloadReelmapAllDn();

	void ReloadReelmapUpInner();
	void ReloadReelmapAllUpInner();
	void ReloadReelmapDnInner();
	void ReloadReelmapAllDnInner();
	void ReloadReelmapIts();
	BOOL ReloadReelmap(int nSerial);

	void RestoreReelmap();
	void ClearReelmap();
	int GetErrCodeUp(int nSerial); // 1(정상), -1(Align Error, 노광불량), -2(Lot End)
	int GetErrCodeDn(int nSerial); // 1(정상), -1(Align Error, 노광불량), -2(Lot End)
	int GetTotDefPcs(int nSerial);
	int GetTotDefPcsUp(int nSerial);
	int GetTotDefPcsDn(int nSerial);

	BOOL ChkYield();// (double &dTotLmt, double &dPrtLmt, double &dRatio);
	void UpdateReelmapYieldUp();
	void UpdateReelmapYieldAllUp();
	void UpdateReelmapYieldDn();
	void UpdateReelmapYieldAllDn();
	void UpdateReelmapYieldIts();

	// [CamMaster]
	BOOL LoadMstInfo();
	void GetAlignPos(CfPoint &Pos1, CfPoint &Pos2);

	// [Punching]
	void ResetMkInfo(int nAoi); // 0 : AOI-Up , 1 : AOI-Dn , 2 : AOI-UpDn

	// [Thread]
	DWORD m_dwThreadTick[MAX_THREAD_MGR_RMAP];
	BOOL m_bThread[MAX_THREAD_MGR_RMAP];
	CThreadTask m_Thread[MAX_THREAD_MGR_RMAP];

	void StartThread();
	void StopThread();

	static UINT ThreadProc0(LPVOID lpContext); // UpdateYield()

	static UINT ThreadProc1(LPVOID lpContext); // UpdateRMapUp()
	static UINT ThreadProc2(LPVOID lpContext); // UpdateRMapDn()
	static UINT ThreadProc3(LPVOID lpContext); // UpdateRMapAllUp()
	static UINT ThreadProc4(LPVOID lpContext); // UpdateRMapAllDn()

	static UINT ThreadProc5(LPVOID lpContext); // UpdateReelmapYieldUp()
	static UINT ThreadProc6(LPVOID lpContext); // UpdateReelmapYieldDn()
	static UINT ThreadProc7(LPVOID lpContext); // UpdateReelmapYieldAllUp()
	static UINT ThreadProc8(LPVOID lpContext); // UpdateReelmapYieldAllDn()

	static UINT ThreadProc9(LPVOID lpContext); // ReloadReelmapUp()
	static UINT ThreadProc10(LPVOID lpContext); // ReloadReelmapDn()
	static UINT ThreadProc11(LPVOID lpContext); // ReloadReelmapAllUp()
	static UINT ThreadProc12(LPVOID lpContext); // ReloadReelmapAllDn()
	static UINT ThreadProc13(LPVOID lpContext); // WriteReelmapIts()

	static UINT ThreadProc14(LPVOID lpContext); // UpdateReelmapYieldIts()
	static UINT ThreadProc15(LPVOID lpContext); // ReloadReelmapUpInner()
	static UINT ThreadProc16(LPVOID lpContext); // ReloadReelmapDnInner()
	static UINT ThreadProc17(LPVOID lpContext); // ReloadReelmapAllUpInner()
	static UINT ThreadProc18(LPVOID lpContext); // ReloadReelmapAllDnInner()
	static UINT ThreadProc19(LPVOID lpContext); // ReloadReelmapIts()

	static UINT ThreadProc20(LPVOID lpContext); // UpdateRMapInnerUp()
	static UINT ThreadProc21(LPVOID lpContext); // UpdateRMapInnerDn()
	static UINT ThreadProc22(LPVOID lpContext); // UpdateRMapInnerAllUp()
	static UINT ThreadProc23(LPVOID lpContext); // UpdateRMapInnerAllDn()

	static UINT ThreadProc24(LPVOID lpContext); // UpdateYieldUp()
	static UINT ThreadProc25(LPVOID lpContext); // UpdateYieldDn()
	static UINT ThreadProc26(LPVOID lpContext); // UpdateYieldAllUp()
	static UINT ThreadProc27(LPVOID lpContext); // UpdateYieldAllDn()

	static UINT ThreadProc28(LPVOID lpContext); // UpdateYieldInnerUp()
	static UINT ThreadProc29(LPVOID lpContext); // UpdateYieldInnerDn()
	static UINT ThreadProc30(LPVOID lpContext); // UpdateYieldInnerAllUp()
	static UINT ThreadProc31(LPVOID lpContext); // UpdateYieldInnerAllDn()
	static UINT ThreadProc32(LPVOID lpContext); // UpdateYieldIts()

	void UpdateRMapUp();
	void UpdateRMapAllUp();
	void UpdateRMapDn();
	void UpdateRMapAllDn();

	void UpdateRMapInnerUp();
	void UpdateRMapInnerAllUp();
	void UpdateRMapInnerDn();
	void UpdateRMapInnerAllDn();

protected:
	DECLARE_MESSAGE_MAP()
};


