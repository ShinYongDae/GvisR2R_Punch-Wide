#pragma once

#include "Process/ReelMap.h"
#include "Process/DataMarking.h"
#include "Global/Yield.h"
#include "Process/CamMaster.h"

// CManagerReelmap

#define MAX_THREAD_MGR_RMAP			33

class CManagerReelmap : public CWnd
{
	DECLARE_DYNAMIC(CManagerReelmap)

	CReelMap* m_pReelMapDisp;
	CReelMap* m_pReelMap;
	CReelMap *m_pReelMapUp, *m_pReelMapDn, *m_pReelMapAllUp, *m_pReelMapAllDn;
	CDataMarking* m_pPcr[MAX_PCR][MAX_PCR_PNL];	//����ȭ��ǥ�ø� ���� ������	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
	//CCamMaster m_Master[2];
	CYield m_Yield[3]; // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-All
	CString *pMkInfo;

	// ���� �۾��� �����ͱ���  ====================================================================
	CCamMaster m_MasterInner[2];

	CReelMap* m_pReelMapInner;
	CReelMap *m_pReelMapInnerUp, *m_pReelMapInnerDn, *m_pReelMapInnerAllUp, *m_pReelMapInnerAllDn;
	CDataMarking* m_pPcrInner[MAX_PCR][MAX_PCR_PNL];	//����ȭ��ǥ�ø� ���� ������	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn

	CReelMap* m_pReelMapIts;
	CDataMarking* m_pPcrIts[MAX_PCR_PNL];				//����ȭ��ǥ�ø� ���� ������	// ������ merging

	//=============================================================================================

	// For ITS
	BOOL m_bEngDualTest;
	CString m_sItsCode;
	CString m_sEngLotNum, m_sEngProcessNum;
	CString m_sEngModel, m_sEngLayerUp;
	CString m_sEngLayerDn;
	int m_nWritedItsSerial;

	//int m_nAoiCamInfoStrPcs[2]; // [0] : Up, [1] : Dn
	//BOOL m_bCamChged;

	void Init();
	void InitVal();
	void InitMstData();

public:
	CManagerReelmap();
	virtual ~CManagerReelmap();

	//BOOL m_bBufEmpty[2];	// [0]: Up, [1]: Dn
	//BOOL m_bBufEmptyF[2];	// [0]: Up, [1]: Dn
	//BOOL m_bChkBufIdx[2];	// [0]: Up, [1]: Dn
	//int m_nChkBufIdx[2];	// [0]: Up, [1]: Dn

	//int m_nShareUpS, m_nShareUpSprev;
	//int m_nShareUpSerial[2]; // [nCam]
	//int m_nShareDnS, m_nShareDnSprev;
	//int m_nShareDnSerial[2]; // [nCam]
	//int m_nShareUpCnt;
	//int m_nShareDnCnt;

	//int m_nBufUpSerial[2];	// [nCam]
	//int m_nBufDnSerial[2];	// [nCam]
	//int m_nBufUpCnt;
	//int m_nBufDnCnt;
	//BOOL m_bIsBuf[2];		// [0]: AOI-Up , [1]: AOI-Dn

	//CString m_sShare[2], m_sBuf[2]; // [0]: AOI-Up , [1]: AOI-Dn
	//int		m_pBufSerial[2][100], m_nBufTot[2]; // [0]: AOI-Up , [1]: AOI-Dn
	//__int64 m_nBufSerialSorting[2][100]; // [0]: AOI-Up , [1]: AOI-Dn
	//int		m_nBufSerial[2][2]; // [0]: AOI-Up , [1]: AOI-Dn // [0]: Cam0, [1]:Cam1

	//BOOL m_bUpdateYield, m_bUpdateYieldOnRmap;
	//BOOL m_bTHREAD_UPDATAE_YIELD[2];		// [0] : Cam0, [1] : Cam1
	//int	m_nSerialTHREAD_UPDATAE_YIELD[2];	// [0] : Cam0, [1] : Cam1

	CString GetMkInfo(int nSerial, int nMkPcs); // return Cam0 : "Serial_Strip_Col_Row"
	CfPoint GetMkPnt(int nSerial, int nMkPcs); // pcr �ø���, pcr �ҷ� �ǽ� ���� ���� �ε���
	CfPoint GetMkPnt(int nMkPcs); // pcr �ø���, pcr �ҷ� �ǽ� ���� ���� �ε���
	int GetMkStripIdx(int nDefPcsId); // 0 : Fail , 1~4 : Strip Idx
	int GetMkStripIdx(int nSerial, int nMkPcs); // 0 : Fail , 1~4 : Strip Idx

	void DuplicateRmap();
	void DuplicateRmap(int nRmap);
	void SetMkPnt(int nCam);	// Pin��ġ�� ���� ����.

	// [Reelmap]
	CCamMaster m_Master[2];

	BOOL InitReelmap();
	BOOL InitReelmapUp();
	BOOL InitReelmapDn();

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
	//	int LoadPCR(int nSerial, BOOL bFromShare = FALSE);		// return : 2(Failed), 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
	int LoadPCR0(int nSerial, BOOL bFromShare = FALSE);		// return : 2(Failed), 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
	int LoadPCR1(int nSerial, BOOL bFromShare = FALSE);		// return : 2(Failed), 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
	int LoadPCRUp(int nSerial, BOOL bFromShare = FALSE);	// return : 2(Failed), 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
	int LoadPCRDn(int nSerial, BOOL bFromShare = FALSE);	// return : 2(Failed), 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
	int LoadPCRAllUp(int nSerial, BOOL bFromShare = FALSE);	// return : 2(Failed), 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
	int LoadPCRAllDn(int nSerial, BOOL bFromShare = FALSE);	// return : 2(Failed), 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)

	int GetPcrIdx(int nSerial, BOOL bNewLot = FALSE);
	int GetPcrIdx0(int nSerial, BOOL bNewLot = FALSE); // Up - ����ȭ�� ǥ�ø� ���� Display buffer�� Shot �ε���
	int GetPcrIdx1(int nSerial, BOOL bNewLot = FALSE); // Dn - ����ȭ�� ǥ�ø� ���� Display buffer�� Shot �ε���

	BOOL GetAoiDnInfo(int nSerial, int *pNewLot = NULL, BOOL bFromBuf = FALSE); // TRUE: CHANGED, FALSE: NO CHANGED
	BOOL GetAoiUpInfo(int nSerial, int *pNewLot = NULL, BOOL bFromBuf = FALSE); // TRUE: CHANGED, FALSE: NO CHANGED
	BOOL GetAoiInfoUp(int nSerial, int *pNewLot = NULL, BOOL bFromBuf = FALSE); // TRUE: CHANGED, FALSE: NO CHANGED
	BOOL GetAoiInfoDn(int nSerial, int *pNewLot = NULL, BOOL bFromBuf = FALSE); // TRUE: CHANGED, FALSE: NO CHANGED

	void InitPcr();
	int GetIdxPcrBuf(int nSerial);
	int GetIdxPcrBufUp(int nSerial);
	int GetIdxPcrBufDn(int nSerial);

	BOOL IsFixPcs();
	BOOL IsFixPcsUp();
	BOOL IsFixPcsDn();
	BOOL IsFixPcsUp(int nSerial);
	BOOL IsFixPcsDn(int nSerial);
	void ClrFixPcs();
	void ClrFixPcs(int nCol, int nRow);
	void SetFixPcs(int nSerial, int nCol, int nRow); // nShot : 0 ~ 
	BOOL IsFixPcs(int nSerial, int &Col, int &Row);
	BOOL IsFixPcs(int nSerial, int* pCol, int* pRow, int &nTot, BOOL &bCont);
	int GetRptFixPcs(int nCol, int nRow);

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
	BOOL MakeMkDir(stModelInfo stInfo);
	BOOL MakeMkDir(CString sModel, CString sLot, CString sLayer);
	BOOL MakeMkDir();
	BOOL MakeMkDirUp();
	BOOL MakeMkDirDn();
	BOOL Shift2Mk(int nSerial);

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

	// PCS �ε����� ��������� �ε����� ��ȯ��.
	int MirrorLR(int nPcsId); // �¿� �̷���
	int Rotate180(int nPcsId);// 180�� ȸ�� = �¿� �̷��� & ���� �̷���
	int MirrorUD(int nPcsId); // ���� �̷���

	int SearchFirstShotOnIts();
	BOOL GetItsSerialInfo(int nItsSerial, BOOL &bDualTest, CString &sLot, CString &sLayerUp, CString &sLayerDn, int nOption = 0);		// ���������� ITS �ø����� ����
	//BOOL SetItsSerialInfo(int nItsSerial);																							// ���������� ITS �ø����� ����
	int GetLastItsSerial();																											// ������ ��¡�� ���� ����Ÿ�� Last �ø���
	CString GetItsFolderPath();
	CString GetItsTargetFolderPath();
	CString GetItsReelmapPath();
	BOOL GetInnerFolderPath(int nItsSerial, CString  &sUp, CString &sDn);

	char* StrToChar(CString str);
	void StrToChar(CString str, char* pCh);

	// For MODE_OUTER ============================================
	int LoadPCRAllUpInner(int nSerial, BOOL bFromShare = FALSE);	// return : 2(Failed), 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
	int LoadPCRAllDnInner(int nSerial, BOOL bFromShare = FALSE);	// return : 2(Failed), 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
	int LoadPCRUpInner(int nSerial, BOOL bFromShare = FALSE);	// return : 2(Failed), 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
	int LoadPCRDnInner(int nSerial, BOOL bFromShare = FALSE);	// return : 2(Failed), 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)

	
	// ITS
	//BOOL m_bTHREAD_UPDATE_REELMAP_INNER_UP, m_bTHREAD_UPDATE_REELMAP_INNER_ALLUP;
	//BOOL m_bTHREAD_UPDATE_REELMAP_INNER_DN, m_bTHREAD_UPDATE_REELMAP_INNER_ALLDN;
	//BOOL m_bTHREAD_UPDATE_REELMAP_ITS;

	BOOL WriteReelmapIts();
	void SetInnerPathAtBuf();
	void SetInnerPathAtBufUp();
	void SetInnerPathAtBufDn();

	int LoadPCRIts(int nSerial, BOOL bFromShare = FALSE);	// return : 2(Failed), 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
	void LoadPCRIts11(int nSerial); // 11 -> ���� : ���, ���� : ���
	void LoadPCRIts10(int nSerial); // 10 -> ���� : ���, ���� : �ܸ�
	void LoadPCRIts01(int nSerial); // 11 -> ���� : �ܸ�, ���� : ���
	void LoadPCRIts00(int nSerial); // 10 -> ���� : �ܸ�, ���� : �ܸ�

	int GetErrCode0Its(int nSerial); // 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
	int GetErrCodeUp0Its(int nSerial); // 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
	int GetErrCodeDn0Its(int nSerial); // 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)

	int GetErrCode1Its(int nSerial); // 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
	int GetErrCodeUp1Its(int nSerial); // 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
	int GetErrCodeDn1Its(int nSerial); // 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)

	int GetTotDefPcs0Its(int nSerial);
	int GetTotDefPcsUp0Its(int nSerial);
	int GetTotDefPcsDn0Its(int nSerial);

	int GetTotDefPcs1Its(int nSerial);
	int GetTotDefPcsUp1Its(int nSerial);
	int GetTotDefPcsDn1Its(int nSerial);

	CfPoint GetMkPnt0Its(int nSerial, int nMkPcs); // pcr �ø���, pcr �ҷ� �ǽ� ���� ���� �ε���
	CfPoint GetMkPnt1Its(int nSerial, int nMkPcs); // pcr �ø���, pcr �ҷ� �ǽ� ���� ���� �ε���

	int GetMkStripIdx0Its(int nSerial, int nMkPcs); // 0 : Fail , 1~4 : Strip Idx
	int GetMkStripIdx1Its(int nSerial, int nMkPcs); // 0 : Fail , 1~4 : Strip Idx


	BOOL InitReelmapInner();
	BOOL InitReelmapInnerUp();
	BOOL InitReelmapInnerDn();
	void SetReelmapInner(int nDir = ROT_NONE);
	//CString GetItsPath(int nSerial, int nLayer);	// RMAP_UP, RMAP_DN, RMAP_INNER_UP, RMAP_INNER_DN
	CString GetItsTargetPath(int nSerial, int nLayer);	// RMAP_UP, RMAP_DN, RMAP_INNER_UP, RMAP_INNER_DN
	int GetItsDefCode(int nDefCode);				// return to [Sapp3Code]
	BOOL MakeItsDir(CString sModel, CString sLot, CString sLayer);
	//void WriteChangedModel();

	int GetAoiUpCamMstInfo();
	int GetAoiDnCamMstInfo();

	BOOL ReloadReelmapInner();
	BOOL ReloadReelmapInner(int nSerial);
	BOOL IsDoneReloadReelmapInner(int& nProc);
	//void UpdateRstInner();
	void OpenReelmapInner();
	void OpenReelmapInnerUp();
	void OpenReelmapInnerDn();

	//int m_nReloadReelmapSerial;
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
	int GetErrCodeUp(int nSerial); // 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
	int GetErrCodeDn(int nSerial); // 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
	int GetTotDefPcs(int nSerial);
	int GetTotDefPcsUp(int nSerial);
	int GetTotDefPcsDn(int nSerial);

	//BOOL m_bTHREAD_UPDATE_REELMAP_UP, m_bTHREAD_UPDATE_REELMAP_ALLUP;
	//BOOL m_bTHREAD_UPDATE_REELMAP_DN, m_bTHREAD_UPDATE_REELMAP_ALLDN;
	//BOOL m_bTHREAD_REELMAP_YIELD_UP, m_bTHREAD_REELMAP_YIELD_ALLUP;
	//BOOL m_bTHREAD_REELMAP_YIELD_DN, m_bTHREAD_REELMAP_YIELD_ALLDN;
	//BOOL m_bTHREAD_RELOAD_RST_UP, m_bTHREAD_RELOAD_RST_ALLUP;
	//BOOL m_bTHREAD_RELOAD_RST_DN, m_bTHREAD_RELOAD_RST_ALLDN;
	//BOOL m_bTHREAD_RELOAD_RST_UP_INNER, m_bTHREAD_RELOAD_RST_ALLUP_INNER;
	//BOOL m_bTHREAD_RELOAD_RST_DN_INNER, m_bTHREAD_RELOAD_RST_ALLDN_INNER;
	//BOOL m_bTHREAD_RELOAD_RST_ITS, m_bTHREAD_REELMAP_YIELD_ITS;

	//BOOL m_bTHREAD_UPDATE_YIELD_UP, m_bTHREAD_UPDATE_YIELD_ALLUP;
	//BOOL m_bTHREAD_UPDATE_YIELD_DN, m_bTHREAD_UPDATE_YIELD_ALLDN;
	//BOOL m_bTHREAD_UPDATE_YIELD_ITS;
	//BOOL m_bTHREAD_UPDATE_YIELD_INNER_UP, m_bTHREAD_UPDATE_YIELD_INNER_ALLUP;
	//BOOL m_bTHREAD_UPDATE_YIELD_INNER_DN, m_bTHREAD_UPDATE_YIELD_INNER_ALLDN;
	//int	m_nSnTHREAD_UPDATAE_YIELD;

	BOOL ChkYield(double &dTotLmt, double &dPrtLmt, double &dRatio);
	void UpdateReelmapYieldUp();
	void UpdateReelmapYieldAllUp();
	void UpdateReelmapYieldDn();
	void UpdateReelmapYieldAllDn();
	void UpdateReelmapYieldIts();

	//BOOL ChkVsShare(int &nSerial);
	//BOOL ChkVsShareUp(int &nSerial);
	//BOOL ChkVsShareDn(int &nSerial);
	//void ChkShare();
	//void ChkShareUp();
	//void ChkShareDn();
	//BOOL ChkShare(int &nSerial);
	//BOOL ChkShareUp(int &nSerial);
	//BOOL ChkShareDn(int &nSerial);
	//BOOL ChkShareIdx(int *pBufSerial, int nBufTot, int nShareSerial);

	//BOOL ChkBufUp(int* pSerial, int &nTot);
	//BOOL ChkBufDn(int* pSerial, int &nTot);
	//BOOL ChkBufIdx(int* pSerial, int nTot = 0);

	//BOOL SortingInUp(CString sPath, int nIndex);
	//BOOL SortingOutUp(int* pSerial, int nTot);
	//void SwapUp(__int64 *num1, __int64 *num2);
	//BOOL SortingInDn(CString sPath, int nIndex);
	//BOOL SortingOutDn(int* pSerial, int nTot);
	//void SwapDn(__int64 *num1, __int64 *num2);

	int GetTotDefPcsIts(int nSerial);

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

