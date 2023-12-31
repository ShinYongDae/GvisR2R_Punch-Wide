// ReelMap.cpp : implementation file
//

#include "stdafx.h"
//#include "gvisr2r.h"
#include "ReelMap.h"
#include "DataFile.h"
#include "IniFile.h"
#include "../Global/GlobalFunc.h"
#include "../Dialog/DlgProgress.h"
#include "../ManagerReelmap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "MyFile.h"
#include "..\\GvisR2R_PunchDoc.h"
#include "..\\GvisR2R_PunchView.h"

extern CGvisR2R_PunchDoc* pDoc;
extern CGvisR2R_PunchView* pView;

/////////////////////////////////////////////////////////////////////////////
// CReelMap

CReelMap::CReelMap(int nLayer, int nPnl, int nPcs, int nDir, CWnd* pParent)
{
	int nC, nR;

	m_pParent = pParent;
	m_nLayer = -1;
	nTotPnl = nPnl;
	nTotPcs = nPcs;
	nDir = ROT_NONE;

	m_dTotLen = 0.0;
	m_bUseLotSep = FALSE;
	m_bContFixDef = FALSE;
	m_dAdjRatio = 1.0;
	m_dTotLen = 0.0;
	m_dPnlLen = 0.0;
	m_dLotLen = 0.0;
	m_dTempPauseLen = 0.0;
	m_dLotCutPosLen = 0.0;
	m_nSerial = 0;
	m_nLastShot = 0;
	m_nCompletedShot = 0;

	//m_nCntFixPcs = 0;
	m_nPrevSerial[0] = 0;	// --
	m_nPrevSerial[1] = 0;	// ++

	m_rgbDef[DEF_NONE]=(RGB_WHITE); // (RGB_GREEN)
	m_rgbDef[DEF_NICK]=(RGB_MAGENTA);
	m_rgbDef[DEF_PROTRUSION]=(RGB_SKYBLUE);
	m_rgbDef[DEF_SPACE]=(RGB_LTGREEN);
	m_rgbDef[DEF_OPEN]=(RGB_LTRED);
	m_rgbDef[DEF_SHORT]=(RGB_RED);
	m_rgbDef[DEF_USHORT]=(RGB_LTCYAN);
	m_rgbDef[DEF_PINHOLE]=(RGB_LLTGREEN);
	m_rgbDef[DEF_HOLE_MISS]=(RGB_LTBLUE);
	m_rgbDef[DEF_EXTRA]=(RGB_CLOUDBLUE);
	m_rgbDef[DEF_PAD]=(RGB_LTPURPLE);
	m_rgbDef[DEF_HOLE_POSITION]=(RGB_PINK);
	m_rgbDef[DEF_POI]=(RGB_LTMAGENTA);
	m_rgbDef[DEF_VH_POSITION]=(RGB_LTYELLOW);
	m_rgbDef[DEF_VH_MISS]=(RGB_BOON);
	m_rgbDef[DEF_HOLE_DEFECT]=(RGB_LTPINK);
	m_rgbDef[DEF_HOLE_OPEN]=(RGB_YELLOWGREEN);
	m_rgbDef[DEF_VH_OPEN]=(RGB_RED);
	m_rgbDef[DEF_VH_DEF]=(RGB_BROWN);
	m_rgbDef[DEF_LIGHT]=(RGB_YELLOW);

	m_cBigDef[0] = '*';		//	NONE
	m_cBigDef[1] = 'N';		//	NICK
	m_cBigDef[2] = 'D';		//	PROTRUSION
	m_cBigDef[3] = 'A';		//	SPACE
	m_cBigDef[4] = 'O';		//	OPEN
	m_cBigDef[5] = 'S';		//	SHORT
	m_cBigDef[6] = 'U';		//	USHORT
	m_cBigDef[7] = 'I';		//	PINHOLE
	m_cBigDef[8] = 'H';		//	HOLE_MISS
	m_cBigDef[9] = 'E';		//	EXTRA
	m_cBigDef[10] = 'P';	//	PAD
	m_cBigDef[11] = 'L';	//	HOLE_POSITION
	m_cBigDef[12] = 'X';	//	POI
	m_cBigDef[13] = 'T';	//	VH_POSITION
	m_cBigDef[14] = 'M';	//	VH_MISS
	m_cBigDef[15] = 'F';	//	HOLE_DEFECT
	m_cBigDef[16] = 'C';	//	HOLE_OPEN
	m_cBigDef[17] = 'G';	//	VH_OPEN
	m_cBigDef[18] = 'V';	//	VH_DEF

	m_cSmallDef[0] = '*';
	m_cSmallDef[1] = 'n';
	m_cSmallDef[2] = 'd';
	m_cSmallDef[3] = 'a';
	m_cSmallDef[4] = 'o';
	m_cSmallDef[5] = 's';
	m_cSmallDef[6] = 'u';
	m_cSmallDef[7] = 'i';
	m_cSmallDef[8] = 'h';
	m_cSmallDef[9] = 'e';
	m_cSmallDef[10] = 'p';
	m_cSmallDef[11] = 'l';
	m_cSmallDef[12] = 'x';
	m_cSmallDef[13] = 't';
	m_cSmallDef[14] = 'm';
	m_cSmallDef[15] = 'f';
	m_cSmallDef[16] = 'c';
	m_cSmallDef[17] = 'g';
	m_cSmallDef[18] = 'v';

	m_pPnlNum = NULL;
	m_pPnlDefNum = NULL;
	pPcsDef = NULL;
	pFrmRgn = NULL;
	pPcsRgn = NULL;
// 	pMkInfo = NULL;

	m_pPnlBuf = NULL;
	m_nPnlBuf = 0;

	if(nPnl>0 && nPcs>0)
	{
		pFrmRgn = new CRect[nPnl];
		m_pPnlNum = new int[nPnl];
		m_pPnlDefNum = new int[nPnl];
	}

	m_bThreadAliveRemakeReelmap = FALSE;
	m_sPathOnThread = _T("");
	
	m_bThreadAliveReloadReelmap = FALSE;
	m_nLastOnThread = 0;
	m_nTotalProgressReloadReelmap = 0;
	m_nProgressReloadReelmap = 0;
	m_bDoneReloadReelmap = FALSE;

	m_nSelMarkingPnl = 2;
	m_nWritedSerial = 0;

	m_nLayer = nLayer;
	m_sPathBuf = GetRmapPath(m_nLayer);
	m_sPathYield = GetYieldPath(m_nLayer);


	LoadConfig();
 	InitRst();
	InitPcs();
	ClrPnlNum();	
	ClrPnlDefNum();
	ClrFixPcs();

	m_nStartSerial = 0;

	// ITS
	//m_pPnlBufIts = NULL;
	//m_nPnlBufIts = 0;

	//ResetYield();

	m_bThreadAliveFinalCopyItsFiles = FALSE;

	for (nC = 0; nC < FIX_PCS_COL_MAX; nC++)
		for (nR = 0; nR < FIX_PCS_ROW_MAX; nR++)
			m_FixPcsRpt[nC][nR] = 0;
}

CReelMap::~CReelMap()
{
	int k;

	m_bThreadAliveReloadReelmap = FALSE;
	m_bThreadAliveRemakeReelmap = FALSE;
	m_bThreadAliveFinalCopyItsFiles = FALSE;


	CloseRst();

// 	if(pMkInfo)
// 	{
// 		delete[] pMkInfo;
// 		pMkInfo = NULL;
// 	}

	if(m_pPnlNum)
	{
		delete[] m_pPnlNum;
		m_pPnlNum = NULL;
	}

	if(m_pPnlDefNum)
	{
		delete[] m_pPnlDefNum;
		m_pPnlDefNum = NULL;
	}

	if(pFrmRgn)
	{
		delete[] pFrmRgn;
		pFrmRgn = NULL;
	}

	if(pPcsRgn)
	{
		for(k=0; k<nTotPnl; k++)
		{
			delete[] pPcsRgn[k];
			pPcsRgn[k] = NULL;
		}
		delete[] pPcsRgn;
		pPcsRgn = NULL;
	}

	if(pPcsDef)
	{
		for(k=0; k<nTotPnl; k++)
		{
			delete[] pPcsDef[k];
			pPcsDef[k] = NULL;
		}

		delete[] pPcsDef;
		pPcsDef = NULL;
	}
}


BEGIN_MESSAGE_MAP(CReelMap, CWnd)
	//{{AFX_MSG_MAP(CReelMap)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CReelMap message handlers
void CReelMap::LoadConfig()
{
	TCHAR szData[MAX_PATH];
	TCHAR sep[] = { _T(",;\r\n\t") };
	CString sVal;

	if (0 < ::GetPrivateProfileString(_T("REELMAP"), _T("BackGround"), NULL, szData, sizeof(szData), PATH_CONFIG))
	{
		sVal = _tcstok(szData, sep);
		m_nBkColor[0] = _tstoi(sVal);
		sVal = _tcstok(NULL,sep);
		m_nBkColor[1] = _tstoi(sVal);
		sVal = _tcstok(NULL,sep);
		m_nBkColor[2] = _tstoi(sVal);
	}

	//if (pView && pView->m_mgrPunch->m_pDts)
	//{
	//	if (pView->m_mgrPunch->m_pDts->IsUseDts())
	//	{
	//		LoadDefectTableDB();
	//		return;
	//	}
	//	else
	//	{
	//		LoadDefectTableIni();
	//	}
	//}
	//else
	{
		LoadDefectTableIni();
	}
}

BOOL CReelMap::LoadDefectTableDB()
{
	BOOL bRtn = FALSE;
	if (pView && pView->m_mgrPunch && pView->m_mgrPunch->m_pDts)
	{
		COLORREF rgbDef[MAX_PATH];
		int nDefCode[MAX_PATH], nMaxR, nMaxC;
		CString sEngN[MAX_PATH], sKorN[MAX_PATH];

		bRtn = pView->m_mgrPunch->m_pDts->LoadDefectTable(nDefCode, rgbDef, sKorN, sEngN, &nMaxR, &nMaxC);
		if (bRtn)
		{			
			TCHAR szData[200];
			TCHAR sep[] = { _T(",;\r\n\t") };
			CString sIdx, sVal;
			int k;

			for (k = 1; k < MAX_DEF; k++)
			{
				sIdx.Format(_T("%d"), k);
				sVal = _tcstok(szData, sep);
				m_sEngDef[nDefCode[k]].Format(_T("%s"), sEngN[nDefCode[k]]);
				sVal = _tcstok(NULL, sep);
				m_sKorDef[nDefCode[k]].Format(_T("%s"), sKorN[nDefCode[k]]);
				sVal = _tcstok(NULL, sep);
				//m_cBigDef[k] = sVal.GetAt(0);
				//sVal = _tcstok(NULL, sep);
				//m_cSmallDef[k] = sVal.GetAt(0);
				//sVal = _tcstok(NULL, sep);
				m_rgbDef[nDefCode[k]] = (COLORREF)rgbDef[nDefCode[k]];
				//sVal = _tcstok(NULL, sep);
				//m_nOdr[k] = _tstoi(sVal);
			}

			return TRUE;
		}

		return bRtn;
	}

	return FALSE;
}

BOOL CReelMap::LoadDefectTableIni()
{
	TCHAR szData[200];
	TCHAR sep[] = { _T(",;\r\n\t") };
	CString sIdx, sVal;
	int k;

	for(k=1; k < MAX_DEF; k++)
	{
		sIdx.Format(_T("%d"), k);
		if (0 < ::GetPrivateProfileString(_T("DEFECT"), sIdx, NULL, szData, sizeof(szData), PATH_CONFIG))
		{
			sVal = _tcstok(szData, sep);
			m_sEngDef[k].Format(_T("%s"), sVal);
			sVal = _tcstok(NULL, sep);
			m_sKorDef[k].Format(_T("%s"), sVal);
			sVal = _tcstok(NULL, sep);
			m_cBigDef[k] = sVal.GetAt(0);
			sVal = _tcstok(NULL, sep);
			m_cSmallDef[k] = sVal.GetAt(0);
			sVal = _tcstok(NULL, sep);
			m_rgbDef[k] = (COLORREF)_tstoi(sVal);
			sVal = _tcstok(NULL, sep);
			m_nOdr[k] = _tstoi(sVal);
		}
		else
		{
			pView->ClrDispMsg();
			AfxMessageBox(_T("Error - LoadDefectTableIni()"));
			return FALSE;
		}
	}

	return TRUE;
}

void CReelMap::SetRgbDef(int nDef, COLORREF rgbVal)
{
	m_rgbDef[nDef]=rgbVal;
}

void CReelMap::SetPnlNum(int *pPnlNum)
{
	if(!m_pPnlNum)
		return;

	int k;
	if(pPnlNum)
	{
		for(k=0; k<nTotPnl; k++)
			m_pPnlNum[k]=pPnlNum[k];
	}
	else
	{
		for(k=0; k<nTotPnl; k++)
			m_pPnlNum[k]=-1;
	}
}

void CReelMap::SetPnlDefNum(int *pPnlDefNum)
{
	if(!m_pPnlDefNum)
		return;

	int k;
	if(pPnlDefNum)
	{
		for(k=0; k<nTotPnl; k++)
			m_pPnlDefNum[k] = pPnlDefNum[k];
	}
	else
	{
		for(k=0; k<nTotPnl; k++)
			m_pPnlDefNum[k] = -1;
	}
}

void CReelMap::ClrPnlNum()
{
	for(int k=0; k<nTotPnl; k++)
		m_pPnlNum[k]=-1;
}

void CReelMap::ClrPnlDefNum()
{
	for(int k=0; k<nTotPnl; k++)
		m_pPnlDefNum[k] = -1;
}

// Panel Index k increases from 0 to (nTotPnl-1)...
void CReelMap::IncPnlNum()
{
	if(!m_pPnlNum)
		return;

	int k;
	for(k=nTotPnl-1; k>0; k--)
		m_pPnlNum[k] = m_pPnlNum[k-1];
	m_pPnlNum[0]++;
}

void CReelMap::AddPnlDefNum(int nDef)
{
	if(!m_pPnlDefNum)
		return;

	int k;
	for(k=nTotPnl-1; k>0; k--)
		m_pPnlDefNum[k] = m_pPnlDefNum[k-1];
	m_pPnlDefNum[0] = nDef;
}

void CReelMap::AddPnlNum(int nNum)
{
	if(!m_pPnlNum)
		return;

	int k;
	for(k=nTotPnl-1; k>0; k--)
		m_pPnlNum[k] = m_pPnlNum[k-1];
	m_pPnlNum[0] = nNum;
}

void CReelMap::SelMarkingPnl(int nNum)
{
	m_nSelMarkingPnl = nNum;
}

void CReelMap::SetAdjRatio(double dRatio)
{
	if (dRatio < 1.0)
		dRatio = 1.0;

	m_dAdjRatio = dRatio;
}

double CReelMap::GetAdjRatio()
{
	return m_dAdjRatio;
}

BOOL CReelMap::Open(CString sPath)
{
	if (pDoc->GetTestMode() == MODE_OUTER)
		MakeItsReelmapHeader();

	return MakeHeader(sPath);
}

BOOL CReelMap::OpenUser(CString sPath)
{
	if (pDoc->GetTestMode() == MODE_OUTER)
		MakeItsReelmapHeader();

	return MakeHeader(sPath);
}

BOOL CReelMap::Open()
{
	CString sPath = GetRmapPath(m_nLayer);

	if (pDoc->GetTestMode() == MODE_OUTER)
		MakeItsReelmapHeader();

	return MakeHeader(sPath);
}

BOOL CReelMap::OpenUser(CString sPath, CString sModel, CString sLayer, CString sLot)
{
	if (pDoc->GetTestMode() == MODE_OUTER)
		MakeItsReelmapHeader();

	return MakeHeader(sPath);
}

BOOL CReelMap::Write(int nSerial)
{
	int nLayer = RMAP_NONE;

	if(nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.62"));
		return 0;
	}

	if (pDoc->GetTestMode() == MODE_OUTER)
		MakeItsReelmapHeader();

	CString sPath = GetRmapPath(m_nLayer);
	MakeHeader(sPath);

	if (pDoc->GetTestMode() == MODE_INNER)
		SetItsSerialInfo(nSerial);

	int nMaxStrip;
#ifdef USE_CAM_MASTER
	nMaxStrip = ((CManagerReelmap*)m_pParent)->m_Master[0].GetStripNum(); // 총 스트립의 갯수
#else
#ifdef TEST_MODE
	nMaxStrip = 4;
#else
	nMaxStrip = MAX_STRIP;
#endif
#endif

	int nIdx = GetPcrIdx(nSerial, pDoc->m_bNewLotShare[1]);
	int nNodeX = ((CManagerReelmap*)m_pParent)->m_Master[0].m_pPcsRgn->nCol;
	int nNodeY = ((CManagerReelmap*)m_pParent)->m_Master[0].m_pPcsRgn->nRow;
	int nStripY = ((CManagerReelmap*)m_pParent)->m_Master[0].m_pPcsRgn->nRow / nMaxStrip; // Strip(1~4or6);
	int nTotDefPcs = 0;

	if (m_nLayer == RMAP_UP || m_nLayer == RMAP_DN || m_nLayer == RMAP_ALLUP || m_nLayer == RMAP_ALLDN)
	{
		nLayer = m_nLayer - RMAP_UP;

		if (((CManagerReelmap*)m_pParent)->m_pPcr[nLayer])
		{
			if (((CManagerReelmap*)m_pParent)->m_pPcr[nLayer][nIdx])
				nTotDefPcs = ((CManagerReelmap*)m_pParent)->m_pPcr[nLayer][nIdx]->m_nTotDef;
		}
	}
	else if (m_nLayer == RMAP_INNER_UP || m_nLayer == RMAP_INNER_DN || m_nLayer == RMAP_INNER_ALLUP || m_nLayer == RMAP_INNER_ALLDN)
	{
		nLayer = m_nLayer - RMAP_INNER_UP;

		if (((CManagerReelmap*)m_pParent)->m_pPcrInner[nLayer])
		{
			if (((CManagerReelmap*)m_pParent)->m_pPcrInner[nLayer][nIdx])
				nTotDefPcs = ((CManagerReelmap*)m_pParent)->m_pPcrInner[nLayer][nIdx]->m_nTotDef;
		}
	}
	else if (m_nLayer == RMAP_ITS)
	{
		if (((CManagerReelmap*)m_pParent)->m_pPcrIts)
		{
			if (((CManagerReelmap*)m_pParent)->m_pPcrIts[nIdx])
				nTotDefPcs = ((CManagerReelmap*)m_pParent)->m_pPcrIts[nIdx]->m_nTotDef;
		}
	}
	else
		return 0;

	short **pPnlBuf;
	int i, nC, nR, nPcsId, nDefCode;//, nTot, nDef, nGood;
	pPnlBuf = new short*[nNodeY];
	for (i = 0; i < nNodeY; i++)
	{
		pPnlBuf[i] = new short[nNodeX];
		memset(pPnlBuf[i], 0, sizeof(short)*nNodeX);
		memset(m_pPnlBuf[nSerial - 1][i], 0, sizeof(short)*nNodeX);
	}

	CString strData, strTemp;

	for (i = 0; i < nTotDefPcs; i++)
	{
		if (m_nLayer == RMAP_UP || m_nLayer == RMAP_DN || m_nLayer == RMAP_ALLUP || m_nLayer == RMAP_ALLDN)
		{
			nLayer = m_nLayer - RMAP_UP;

			if (((CManagerReelmap*)m_pParent)->m_pPcr[nLayer][nIdx]->m_pMk[i] != -2) // -2 (NoMarking)
			{
				if (pDoc->WorkingInfo.System.bStripPcsRgnBin)	// DTS용
				{
					switch (((CManagerReelmap*)m_pParent)->m_Master[0].MasterInfo.nActionCode)	// 0 : Rotation / Mirror 적용 없음(CAM Data 원본), 1 : 좌우 미러, 2 : 상하 미러, 3 : 180 회전, 4 : 270 회전(CCW), 5 : 90 회전(CW)
					{
					case 0:
						nPcsId = ((CManagerReelmap*)m_pParent)->m_pPcr[nLayer][nIdx]->m_pDefPcs[i];
						break;
					case 1:
						nPcsId = ((CManagerReelmap*)m_pParent)->MirrorLR(((CManagerReelmap*)m_pParent)->m_pPcr[nLayer][nIdx]->m_pDefPcs[i]);
						break;
					case 2:
						nPcsId = ((CManagerReelmap*)m_pParent)->MirrorUD(((CManagerReelmap*)m_pParent)->m_pPcr[nLayer][nIdx]->m_pDefPcs[i]);
						break;
					case 3:
						nPcsId = ((CManagerReelmap*)m_pParent)->Rotate180(((CManagerReelmap*)m_pParent)->m_pPcr[nLayer][nIdx]->m_pDefPcs[i]);
						break;
					default:
						nPcsId = ((CManagerReelmap*)m_pParent)->m_pPcr[nLayer][nIdx]->m_pDefPcs[i];
						break;
					}
				}
				else
					nPcsId = ((CManagerReelmap*)m_pParent)->m_pPcr[nLayer][nIdx]->m_pDefPcs[i];

				nDefCode = ((CManagerReelmap*)m_pParent)->m_pPcr[nLayer][nIdx]->m_pDefType[i];

				nC = int(nPcsId / nNodeY);
				if (nC % 2)	// 홀수.
					nR = nNodeY * (nC + 1) - nPcsId - 1;
				else		// 짝수.
					nR = nPcsId - nNodeY * nC;
				pPnlBuf[nR][nC] = (short)nDefCode;	// nPnl의 열 정보.
				if (m_pPnlBuf)
					m_pPnlBuf[nSerial - 1][nR][nC] = pPnlBuf[nR][nC]; // DefCode 3D Array : [nSerial][nRow][nCol] - 릴맵파일 정보용.
			}
		}
		else if (m_nLayer == RMAP_INNER_UP || m_nLayer == RMAP_INNER_DN || m_nLayer == RMAP_INNER_ALLUP || m_nLayer == RMAP_INNER_ALLDN)
		{
			nLayer = m_nLayer - RMAP_INNER_UP;

			if (((CManagerReelmap*)m_pParent)->m_pPcrInner[nLayer][nIdx]->m_pMk[i] != -2) // -2 (NoMarking)
			{
				if (pDoc->WorkingInfo.System.bStripPcsRgnBin)	// DTS용
				{
					switch (((CManagerReelmap*)m_pParent)->m_MasterInner[0].MasterInfo.nActionCode)	// 0 : Rotation / Mirror 적용 없음(CAM Data 원본), 1 : 좌우 미러, 2 : 상하 미러, 3 : 180 회전, 4 : 270 회전(CCW), 5 : 90 회전(CW)
					{
					case 0:
						nPcsId = ((CManagerReelmap*)m_pParent)->m_pPcrInner[nLayer][nIdx]->m_pDefPcs[i];
						break;
					case 1:
						nPcsId = ((CManagerReelmap*)m_pParent)->MirrorLR(((CManagerReelmap*)m_pParent)->m_pPcrInner[nLayer][nIdx]->m_pDefPcs[i]);
						break;
					case 2:
						nPcsId = ((CManagerReelmap*)m_pParent)->MirrorUD(((CManagerReelmap*)m_pParent)->m_pPcrInner[nLayer][nIdx]->m_pDefPcs[i]);
						break;
					case 3:
						nPcsId = ((CManagerReelmap*)m_pParent)->Rotate180(((CManagerReelmap*)m_pParent)->m_pPcrInner[nLayer][nIdx]->m_pDefPcs[i]);
						break;
					default:
						nPcsId = ((CManagerReelmap*)m_pParent)->m_pPcrInner[nLayer][nIdx]->m_pDefPcs[i];
						break;
					}
				}
				else
					nPcsId = ((CManagerReelmap*)m_pParent)->m_pPcrInner[nLayer][nIdx]->m_pDefPcs[i];

				nDefCode = ((CManagerReelmap*)m_pParent)->m_pPcrInner[nLayer][nIdx]->m_pDefType[i];

				nC = int(nPcsId / nNodeY);
				if (nC % 2)	// 홀수.
					nR = nNodeY * (nC + 1) - nPcsId - 1;
				else		// 짝수.
					nR = nPcsId - nNodeY * nC;
				pPnlBuf[nR][nC] = (short)nDefCode;	// nPnl의 열 정보.
				if (m_pPnlBuf)
					m_pPnlBuf[nSerial - 1][nR][nC] = pPnlBuf[nR][nC]; // DefCode 3D Array : [nSerial][nRow][nCol] - 릴맵파일 정보용.
			}
		}
		else if (m_nLayer == RMAP_ITS)
		{
			if (((CManagerReelmap*)m_pParent)->m_pPcrIts[nIdx]->m_pMk[i] != -2) // -2 (NoMarking)
			{
				if (pDoc->WorkingInfo.System.bStripPcsRgnBin)	// DTS용
				{
					switch (((CManagerReelmap*)m_pParent)->m_Master[0].MasterInfo.nActionCode)	// 0 : Rotation / Mirror 적용 없음(CAM Data 원본), 1 : 좌우 미러, 2 : 상하 미러, 3 : 180 회전, 4 : 270 회전(CCW), 5 : 90 회전(CW)
					{
					case 0:
						nPcsId = ((CManagerReelmap*)m_pParent)->m_pPcrIts[nIdx]->m_pDefPcs[i];
						break;
					case 1:
						nPcsId = ((CManagerReelmap*)m_pParent)->MirrorLR(((CManagerReelmap*)m_pParent)->m_pPcrIts[nIdx]->m_pDefPcs[i]);
						break;
					case 2:
						nPcsId = ((CManagerReelmap*)m_pParent)->MirrorUD(((CManagerReelmap*)m_pParent)->m_pPcrIts[nIdx]->m_pDefPcs[i]);
						break;
					case 3:
						nPcsId = ((CManagerReelmap*)m_pParent)->Rotate180(((CManagerReelmap*)m_pParent)->m_pPcrIts[nIdx]->m_pDefPcs[i]);
						break;
					default:
						nPcsId = ((CManagerReelmap*)m_pParent)->m_pPcrIts[nIdx]->m_pDefPcs[i];
						break;
					}
				}
				else
					nPcsId = ((CManagerReelmap*)m_pParent)->m_pPcrIts[nIdx]->m_pDefPcs[i];

				nDefCode = ((CManagerReelmap*)m_pParent)->m_pPcrIts[nIdx]->m_pDefType[i];

				nC = int(nPcsId / nNodeY);
				if (nC % 2)	// 홀수.
					nR = nNodeY * (nC + 1) - nPcsId - 1;
				else		// 짝수.
					nR = nPcsId - nNodeY * nC;
				pPnlBuf[nR][nC] = (short)nDefCode;	// nPnl의 열 정보.
				if (m_pPnlBuf)
					m_pPnlBuf[nSerial - 1][nR][nC] = pPnlBuf[nR][nC]; // DefCode 3D Array : [nSerial][nRow][nCol] - 릴맵파일 정보용.
			}
		}
		else
			return 0;
	}

	CString sPnl, sRow;
	i = 0;
	sPnl.Format(_T("%d"), nSerial);
	strData.Format(_T("%d"), nTotDefPcs);
	::WritePrivateProfileString(sPnl, _T("Total Defects"), strData, sPath);
	//::WritePrivateProfileString(sPnl, _T("Total Defects"), strData, m_sPathShare);

	for(int nRow=0; nRow<nNodeX; nRow++)			// 릴맵 Text(90도 시계방향으로 회전한 모습) 상의 Row : Shot의 첫번째 Col부터 시작해서 밑으로 내려감.
	{
		sRow.Format(_T("%02d"), nRow);
// 		sRow.Format(_T("%2d"), nRow+1);
		strData.Format(_T(""));
		strTemp.Format(_T(""));

// 		if(nRow==26)
// 			int brk=0;

		for(int nCol=0; nCol<nNodeY; nCol++)		// 릴맵 Text(90도 시계방향으로 회전한 모습) 상의 Col : 4열 3열 2열 1열 스트립으로 표시됨.
		{
			nR = (nNodeY-1)-nCol;				// 릴맵상의 Row
			nC = nRow;							// 릴맵상의 Col

			if (m_nLayer == RMAP_UP || m_nLayer == RMAP_DN || m_nLayer == RMAP_ALLUP || m_nLayer == RMAP_ALLDN)
			{
				nLayer = m_nLayer - RMAP_UP;

				if (((CManagerReelmap*)m_pParent)->m_pPcr[nLayer][nIdx]->m_nErrPnl == -1 || ((CManagerReelmap*)m_pParent)->m_pPcr[nLayer][nIdx]->m_nErrPnl == -2)
				{
					nDefCode = DEF_LIGHT;
					m_pPnlBuf[nSerial - 1][nR][nC] = (short)nDefCode;
				}
				else
					nDefCode = (int)pPnlBuf[nR][nC] < 0 ? 0 : (int)pPnlBuf[nR][nC];
			}
			else if (m_nLayer == RMAP_INNER_UP || m_nLayer == RMAP_INNER_DN || m_nLayer == RMAP_INNER_ALLUP || m_nLayer == RMAP_INNER_ALLDN)
			{
				nLayer = m_nLayer - RMAP_INNER_UP;

				if (((CManagerReelmap*)m_pParent)->m_pPcrInner[nLayer][nIdx]->m_nErrPnl == -1 || ((CManagerReelmap*)m_pParent)->m_pPcrInner[nLayer][nIdx]->m_nErrPnl == -2)
				{
					nDefCode = DEF_LIGHT;
					m_pPnlBuf[nSerial - 1][nR][nC] = (short)nDefCode;
				}
				else
					nDefCode = (int)pPnlBuf[nR][nC] < 0 ? 0 : (int)pPnlBuf[nR][nC];
			}
			else if (m_nLayer == RMAP_ITS)
			{
				if (((CManagerReelmap*)m_pParent)->m_pPcrIts[nIdx]->m_nErrPnl == -1 || ((CManagerReelmap*)m_pParent)->m_pPcrIts[nIdx]->m_nErrPnl == -2)
				{
					nDefCode = DEF_LIGHT;
					m_pPnlBuf[nSerial - 1][nR][nC] = (short)nDefCode;
				}
				else
					nDefCode = (int)pPnlBuf[nR][nC] < 0 ? 0 : (int)pPnlBuf[nR][nC];
			}
			else
				return 0;

			strTemp.Format(_T("%2d,"), nDefCode);	// 불량코드를 2칸으로 설정
			
			if(!nCol)								// strData에 처음으로 데이터를 추가
				strData.Insert(0, strTemp);
			else
			{
				int nLen = strData.GetLength();
				if( !(nCol%nStripY) )				// Separate Strip (스트립 마다)
				{
					strData.Insert(nLen, _T("  "));
					nLen = strData.GetLength();
				}
				strData.Insert(nLen, strTemp);
			}
		}

		int nPos = strData.ReverseFind(',');		// 릴맵 Text 맨 우측의 ','를 삭제
		strData.Delete(nPos, 1);
		::WritePrivateProfileString(sPnl, sRow, strData, sPath); // 한 라인씩 릴맵 Text를 기록.
		//::WritePrivateProfileString(sPnl, sRow, strData, m_sPathShare); // 한 라인씩 릴맵 Text를 기록.
	}	// 릴맵 Text(90도 시계방향으로 회전한 모습) 상의 Row : Shot의 마지막 Col까지 기록하고 끝남.

	for(i=0; i<nNodeY; i++)
		delete[]  pPnlBuf[i];
	delete[] pPnlBuf;

	m_nWritedSerial = nSerial;

	return TRUE;
}


void CReelMap::InitPcs()
{
	int k, i;
	if(nTotPnl>0 && nTotPcs>0)
	{
		if(!pPcsRgn)
		{
			pPcsRgn = new CRect*[nTotPnl];
			for(k=0; k<nTotPnl; k++)
			{
				pPcsRgn[k] = new CRect[nTotPcs];
			}
		}

		if(!pPcsDef)
		{
			pPcsDef = new int*[nTotPnl];
			for(k=0; k<nTotPnl; k++)
			{
				if (nTotPcs > MAX_PCS)
				{
					pView->ClrDispMsg();
					AfxMessageBox(_T("MAX_PCS 초과 Error."));
				}
				pPcsDef[k] = new int[MAX_PCS];
				for(i=0; i<MAX_PCS; i++)
					pPcsDef[k][i] = DEF_NONE;
			}
		}

// 		if(!pMkInfo)
// 			pMkInfo = new CString[nTotPcs];
	}
	else
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Error-InitPcs."));
	}
}

void CReelMap::ClrPcs()
{
	for(int k=0; k<nTotPnl; k++)
	{
		for(int i=0; i<nTotPcs; i++)
		{
			pPcsDef[k][i] = DEF_NONE;
		}
	}
}

BOOL CReelMap::Disp(int nMkPnl, BOOL bDumy)
{
	if (!pView->m_mgrProcedure)
		return FALSE;

	if(!bDumy)
	{
		if(!pPcsDef || !m_pPnlNum || !m_pPnlDefNum)// || !pMkInfo)
		{
			pView->MsgBox(_T("Reelmap Memory Error."));
			return FALSE;
		}
	}

	if(!((CManagerReelmap*)m_pParent)->m_Master[0].m_pPcsRgn)
	{
		pView->MsgBox(_T("PCS RGN Error."));
		return FALSE;
	}

	m_sPathBuf = GetRmapPath(m_nLayer);

	CDataFile *pDataFile = new CDataFile;
	CString sMsg;
	int nRepeat = 0;

	while(!pDataFile->Open(m_sPathBuf))
	{
		Sleep(500);
		if (!pDataFile->Open(m_sPathBuf))
		{
			Sleep(500);
			if (!pDataFile->Open(m_sPathBuf))
			{
				Sleep(500);
				if (!pDataFile->Open(m_sPathBuf))
				{
					if (nRepeat > 20)
					{
						sMsg.Format(_T("릴맵파일을 읽지 못했습니다.\r\n%s\r\n릴맵파일을 다시 읽으시겠습니까?"), m_sPathBuf);
						if (IDNO == pView->MsgBox(sMsg, 0, MB_YESNO))
						{
							;
						}
					}
					else
					{
						nRepeat++;
						Sleep(500);
					}
				}
				else
					break;
			}
			else
				break;
		}
		else
			break;
	}
	
	m_nSerial = nMkPnl; // 8

	int nMkedPnl = nTotPnl-m_nSelMarkingPnl-1; // 8-2-1 = 5
	int nPrevPnl = m_nSelMarkingPnl; // 2

	int nFromPnl = nMkPnl-nMkedPnl-1; // 8-5-1 = 2

	CString sPnl, sRow, sVal;
	TCHAR sep[] = { _T(",/;\r\n\t") };
	TCHAR szData[MAX_PATH];
	int k, i, nR, nC, nP, nInc=0;
	//int nCol, nRow;
	int nLoadPnl, nDefCode;
	int nNodeX = ((CManagerReelmap*)m_pParent)->m_Master[0].m_pPcsRgn->nCol; // 10
	int nNodeY = ((CManagerReelmap*)m_pParent)->m_Master[0].m_pPcsRgn->nRow; // 5

	for(k=nTotPnl-1; k>=0; k--) // k = 7 ~ 0
	{
		nInc++;
		nLoadPnl = nInc+nFromPnl; // 1 + 2 = 3 ~ 8 + 2 = 10

		if(pDoc->WorkingInfo.LastJob.bLotSep)
		{
			if(nLoadPnl > pDoc->m_nLotLastShot && !pDoc->m_bDoneChgLot)
			{
				for(int aa=k; aa>=0; aa--)
				{
					m_pPnlNum[aa] = nLoadPnl = -1;
					m_pPnlDefNum[aa] = -1;
				}
				break;
			}
		}

		if (pView->m_mgrProcedure->m_bSerialDecrese)
		{
			if (nLoadPnl < (pView->m_mgrProcedure->m_nLotEndSerial - pDoc->AoiDummyShot[0]) && pView->m_mgrProcedure->m_nLotEndSerial > 0)
			{
				for (int bb = k; bb >= 0; bb--)
				{
					m_pPnlNum[bb] = nLoadPnl = -1;
					m_pPnlDefNum[bb] = -1;
				}
				break;
			}
			else if (pView->ChkLastProc() && (nLoadPnl < pView->m_mgrProcedure->m_nLotEndSerial && pView->m_mgrProcedure->m_nLotEndSerial > 0))
			{
				for (int bb = k; bb >= 0; bb--)
				{
					m_pPnlNum[bb] = nLoadPnl = -1;
					m_pPnlDefNum[bb] = -1;
				}
				break;
			}
			else if (nLoadPnl < pView->m_mgrProcedure->m_nLotEndSerial && pView->m_mgrProcedure->m_nLotEndSerial > 0)
			{
				for (int cc = k; cc >= 0; cc--)
				{
					m_pPnlDefNum[cc] = -1;
				}
			}
		}
		else
		{
			if (nLoadPnl > (pView->m_mgrProcedure->m_nLotEndSerial + pDoc->AoiDummyShot[0]) && pView->m_mgrProcedure->m_nLotEndSerial > 0)
			{
				for (int bb = k; bb >= 0; bb--)
				{
					m_pPnlNum[bb] = nLoadPnl = -1;
					m_pPnlDefNum[bb] = -1;
				}
				break;
			}
			else if (pView->ChkLastProc() && (nLoadPnl > pView->m_mgrProcedure->m_nLotEndSerial && pView->m_mgrProcedure->m_nLotEndSerial > 0))
			{
				for (int bb = k; bb >= 0; bb--)
				{
					m_pPnlNum[bb] = nLoadPnl = -1;
					m_pPnlDefNum[bb] = -1;
				}
				break;
			}
			else if (nLoadPnl > pView->m_mgrProcedure->m_nLotEndSerial && pView->m_mgrProcedure->m_nLotEndSerial > 0)
			{
				for (int cc = k; cc >= 0; cc--)
				{
					m_pPnlDefNum[cc] = -1;
				}
			}
		}


		if(nLoadPnl <= 0)
		{
			m_pPnlNum[k] = -1;
			m_pPnlDefNum[k] = -1;
			for(i=0; i<nTotPcs; i++)
				pPcsDef[k][i] = DEF_NONE;
		}
		else
		{
			if (pView->m_mgrProcedure->m_bSerialDecrese)
			{
				if (pDoc->WorkingInfo.LastJob.bLotSep && nLoadPnl < pDoc->m_nLotLastShot && !pDoc->m_bDoneChgLot)
				{
					m_pPnlNum[k] = 0;
					m_pPnlDefNum[k] = -1;
					break;
				}
				else if (nLoadPnl < pDoc->m_ListBuf[0].GetLast())
				{
					m_pPnlNum[k] = 0;
					m_pPnlDefNum[k] = -1;
					break;
				}

				m_pPnlNum[k] = nLoadPnl; // 3 ~ 10
				if (nLoadPnl >= pView->m_mgrProcedure->m_nLotEndSerial || pView->m_mgrProcedure->m_nLotEndSerial == 0)
				{
					if (m_nLayer == RMAP_UP || m_nLayer == RMAP_DN || m_nLayer == RMAP_ALLUP || m_nLayer == RMAP_ALLDN)
					{
						//m_pPnlDefNum[k] = ((CManagerReelmap*)m_pParent)->m_pPcr[RMAP_ALLUP][pDoc->GetPcrIdx1(nLoadPnl)]->m_nTotDef;
						m_pPnlDefNum[k] = ((CManagerReelmap*)m_pParent)->m_pPcr[m_nLayer][GetPcrIdx(nLoadPnl)]->m_nTotDef;
					}
					else if (m_nLayer == RMAP_INNER_UP || m_nLayer == RMAP_INNER_DN || m_nLayer == RMAP_INNER_ALLUP || m_nLayer == RMAP_INNER_ALLDN)
					{
						//m_pPnlDefNum[k] = ((CManagerReelmap*)m_pParent)->m_pPcrInner[RMAP_ALLUP][pDoc->GetPcrIdx1(nLoadPnl)]->m_nTotDef;
						m_pPnlDefNum[k] = ((CManagerReelmap*)m_pParent)->m_pPcrInner[m_nLayer][GetPcrIdx(nLoadPnl)]->m_nTotDef;
					}
					else if (m_nLayer == RMAP_ITS)
						m_pPnlDefNum[k] = ((CManagerReelmap*)m_pParent)->m_pPcrIts[GetPcrIdx(nLoadPnl)]->m_nTotDef;
					else
						return 0;
				}
			}
			else
			{
				if (pDoc->WorkingInfo.LastJob.bLotSep && nLoadPnl > pDoc->m_nLotLastShot && !pDoc->m_bDoneChgLot)
				{
					m_pPnlNum[k] = 0;
					m_pPnlDefNum[k] = -1;
					break;
				}
				else if (nLoadPnl > pDoc->m_ListBuf[0].GetLast())
				{
					m_pPnlNum[k] = 0;
					m_pPnlDefNum[k] = -1;
					break;
				}

				m_pPnlNum[k] = nLoadPnl; // 3 ~ 10
				if (nLoadPnl <= pView->m_mgrProcedure->m_nLotEndSerial || pView->m_mgrProcedure->m_nLotEndSerial == 0)
				{
					if (m_nLayer == RMAP_UP || m_nLayer == RMAP_DN || m_nLayer == RMAP_ALLUP || m_nLayer == RMAP_ALLDN)
					{
						//m_pPnlDefNum[k] = ((CManagerReelmap*)m_pParent)->m_pPcr[RMAP_ALLUP][pDoc->GetPcrIdx1(nLoadPnl)]->m_nTotDef;
						m_pPnlDefNum[k] = ((CManagerReelmap*)m_pParent)->m_pPcr[m_nLayer][GetPcrIdx(nLoadPnl)]->m_nTotDef;
					}
					else if (m_nLayer == RMAP_INNER_UP || m_nLayer == RMAP_INNER_DN || m_nLayer == RMAP_INNER_ALLUP || m_nLayer == RMAP_INNER_ALLDN)
					{
						//m_pPnlDefNum[k] = ((CManagerReelmap*)m_pParent)->m_pPcrInner[RMAP_ALLUP][pDoc->GetPcrIdx1(nLoadPnl)]->m_nTotDef;
						m_pPnlDefNum[k] = ((CManagerReelmap*)m_pParent)->m_pPcrInner[m_nLayer][GetPcrIdx(nLoadPnl)]->m_nTotDef;
					}
					else if (m_nLayer == RMAP_ITS)
						m_pPnlDefNum[k] = ((CManagerReelmap*)m_pParent)->m_pPcrIts[GetPcrIdx(nLoadPnl)]->m_nTotDef;
					else
						return 0;
				}
			}

			for(nR=0; nR<nNodeX; nR++)  // nR = 0 ~ 5
			{
				sPnl.Format(_T("%d"), nLoadPnl);
				sRow.Format(_T("%02d"), nR);
				
				if (0 < ::GetPrivateProfileString(sPnl, sRow, NULL, szData, sizeof(szData), m_sPathBuf)) // DefCode 3D Array : [nSerial][nRow][nCol] - 릴맵파일
				{
					for(nC=0; nC<nNodeY; nC++)
					{
						if(nC==0)
							sVal = _tcstok(szData,sep);
						else
							sVal = _tcstok(NULL,sep);
						
						if(nR%2)	// 홀수.
							nP = nC + nNodeY * nR;
						else		// 짝수.				
							nP = nNodeY * (nR+1) - (nC+1);

						if (pDoc->WorkingInfo.System.bStripPcsRgnBin)	// DTS용
						{
							switch (((CManagerReelmap*)m_pParent)->m_Master[1].MasterInfo.nActionCode)	// 0 : Rotation / Mirror 적용 없음(CAM Data 원본), 1 : 좌우 미러, 2 : 상하 미러, 3 : 180 회전, 4 : 270 회전(CCW), 5 : 90 회전(CW)
							{
							case 0:
								break;
							case 1:
								nP = ((CManagerReelmap*)m_pParent)->MirrorLR(nP);
								break;
							case 2:
								nP = ((CManagerReelmap*)m_pParent)->MirrorUD(nP);
								break;
							case 3:
								nP = ((CManagerReelmap*)m_pParent)->Rotate180(nP);
								break;
							default:
								break;
							}
						}

						nDefCode = _tstoi(sVal);
						pPcsDef[k][nP] = nDefCode; // k=7, nP = PcsIdx : 좌상단에서 nP 0이 시작하여 ZigZeg로 우하단으로 증가

					}
				}
				else
				{
					Sleep(300);

					if (0 < ::GetPrivateProfileString(sPnl, sRow, NULL, szData, sizeof(szData), m_sPathBuf))
					{
						for (nC = 0; nC < nNodeY; nC++)
						{
							if (nC == 0)
								sVal = _tcstok(szData, sep);
							else
								sVal = _tcstok(NULL, sep);

							if (nR % 2)	// 홀수.
								nP = nC + nNodeY * nR;
							else		// 짝수.				
								nP = nNodeY * (nR + 1) - (nC + 1);

							if (pDoc->WorkingInfo.System.bStripPcsRgnBin)	// DTS용
							{
								switch (((CManagerReelmap*)m_pParent)->m_Master[1].MasterInfo.nActionCode)	// 0 : Rotation / Mirror 적용 없음(CAM Data 원본), 1 : 좌우 미러, 2 : 상하 미러, 3 : 180 회전, 4 : 270 회전(CCW), 5 : 90 회전(CW)
								{
								case 0:
									break;
								case 1:
									nP = ((CManagerReelmap*)m_pParent)->MirrorLR(nP);
									break;
								case 2:
									nP = ((CManagerReelmap*)m_pParent)->MirrorUD(nP);
									break;
								case 3:
									nP = ((CManagerReelmap*)m_pParent)->Rotate180(nP);
									break;
								default:
									break;
								}
							}

							nDefCode = _tstoi(sVal);
							pPcsDef[k][nP] = nDefCode; // k=7, nP = PcsIdx : 좌상단에서 nP 0이 시작하여 ZigZeg로 우하단으로 증가
						}
					}
					else
					{
						if (!bDumy)
						{
							if (!pView->m_mgrProcedure->m_bLastProc && !pDoc->WorkingInfo.LastJob.bSampleTest)
							{
								{
									m_pPnlNum[k] = -1;
									m_pPnlDefNum[k] = -1;
									for (i = 0; i < nTotPcs; i++)
										pPcsDef[k][i] = DEF_NONE;
								}
							}
							else
							{
								m_pPnlNum[k] = 0;
								m_pPnlDefNum[k] = -1;

								for (nC = 0; nC < nNodeY; nC++)
								{
									if (nR % 2)	// 홀수.
										nP = nC + nNodeY * nR;
									else		// 짝수.				
										nP = nNodeY * (nR + 1) - (nC + 1);

									if (pDoc->WorkingInfo.System.bStripPcsRgnBin)	// DTS용
									{
										switch (((CManagerReelmap*)m_pParent)->m_Master[1].MasterInfo.nActionCode)	// 0 : Rotation / Mirror 적용 없음(CAM Data 원본), 1 : 좌우 미러, 2 : 상하 미러, 3 : 180 회전, 4 : 270 회전(CCW), 5 : 90 회전(CW)
										{
										case 0:
											break;
										case 1:
											nP = ((CManagerReelmap*)m_pParent)->MirrorLR(nP);
											break;
										case 2:
											nP = ((CManagerReelmap*)m_pParent)->MirrorUD(nP);
											break;
										case 3:
											nP = ((CManagerReelmap*)m_pParent)->Rotate180(nP);
											break;
										default:
											break;
										}
									}

									nDefCode = DEF_NONE;
									pPcsDef[k][nP] = nDefCode; // k=7, nP = PcsIdx : 좌상단에서 nP 0이 시작하여 ZigZeg로 우하단으로 증가
								}

							}
						}
						else
						{
							for (nC = 0; nC < nNodeY; nC++)
							{
								if (nC == 0)
									sVal = _tcstok(szData, sep);
								else
									sVal = _tcstok(NULL, sep);

								if (nR % 2)	// 홀수.
									nP = nC + nNodeY * nR;
								else		// 짝수.
									nP = nNodeY * (nR + 1) - (nC + 1);

								if (pDoc->WorkingInfo.System.bStripPcsRgnBin)	// DTS용
								{
									switch (((CManagerReelmap*)m_pParent)->m_Master[1].MasterInfo.nActionCode)	// 0 : Rotation / Mirror 적용 없음(CAM Data 원본), 1 : 좌우 미러, 2 : 상하 미러, 3 : 180 회전, 4 : 270 회전(CCW), 5 : 90 회전(CW)
									{
									case 0:
										break;
									case 1:
										nP = ((CManagerReelmap*)m_pParent)->MirrorLR(nP);
										break;
									case 2:
										nP = ((CManagerReelmap*)m_pParent)->MirrorUD(nP);
										break;
									case 3:
										nP = ((CManagerReelmap*)m_pParent)->Rotate180(nP);
										break;
									default:
										break;
									}
								}

								nDefCode = 0;
								pPcsDef[k][nP] = nDefCode; // k=7, nP = PcsIdx : 좌상단에서 nP 0이 시작하여 ZigZeg로 우하단으로 증가

	 							//if(pDoc->WorkingInfo.LastJob.bContFixDef)
	 							//	SetFixPcs(nC, nR);
							}
						}
					}
				}
			}
		}
	}

	if(pDataFile)
	{
		delete pDataFile;
		pDataFile = NULL;
	}

	return TRUE;
}

void CReelMap::SetLotSt()
{
	CString sData;
	int nYear, nMonth, nDay, nHour, nMin, nSec;
	nYear = pDoc->WorkingInfo.Lot.StTime.nYear; 
	nMonth = pDoc->WorkingInfo.Lot.StTime.nMonth; 
	nDay = pDoc->WorkingInfo.Lot.StTime.nDay; 
	nHour = pDoc->WorkingInfo.Lot.StTime.nHour; 
	nMin = pDoc->WorkingInfo.Lot.StTime.nMin; 
	nSec = pDoc->WorkingInfo.Lot.StTime.nSec;

	if(!nYear && !nMonth && !nDay && !nHour && !nMin && !nSec)
		sData = _T("");
	else
		sData.Format(_T("%04d-%02d-%02d, %02d:%02d:%02d"), nYear, nMonth, nDay, nHour, nMin, nSec);
	::WritePrivateProfileString(_T("Info"), _T("Lot Start"), sData, m_sPathBuf);
}

CString CReelMap::GetLotSt()
{
	CString sLot = _T("");
	//char szData[MAX_PATH];
	TCHAR szData[MAX_PATH];
	if (0 < ::GetPrivateProfileString(_T("Info"), _T("Lot Start"), NULL, szData, sizeof(szData), m_sPathBuf))
		sLot = CString(szData);
	return sLot;
}

void CReelMap::SetLotEd()
{
	if (!pView->m_mgrProcedure)
		return;

	CString sData;
	int nYear, nMonth, nDay, nHour, nMin, nSec;
	nYear = pDoc->WorkingInfo.Lot.EdTime.nYear; 
	nMonth = pDoc->WorkingInfo.Lot.EdTime.nMonth; 
	nDay = pDoc->WorkingInfo.Lot.EdTime.nDay; 
	nHour = pDoc->WorkingInfo.Lot.EdTime.nHour; 
	nMin = pDoc->WorkingInfo.Lot.EdTime.nMin; 
	nSec = pDoc->WorkingInfo.Lot.EdTime.nSec;

	if(!nYear && !nMonth && !nDay && !nHour && !nMin && !nSec)
		sData = _T("");
	else
		sData.Format(_T("%04d-%02d-%02d, %02d:%02d:%02d"), nYear, nMonth, nDay, nHour, nMin, nSec);
	::WritePrivateProfileString(_T("Info"), _T("Lot End"), sData, m_sPathBuf);

	if((pView->m_mgrProcedure->m_dwLotEd-pView->m_mgrProcedure->m_dwLotSt) > 0)
	{
		int nDiff = (pView->m_mgrProcedure->m_dwLotEd-pView->m_mgrProcedure->m_dwLotSt)/1000;
		nHour = int(nDiff/3600);
		nMin = int((nDiff-3600*nHour)/60);
		nSec = nDiff % 60;
		sData.Format(_T("%02d:%02d:%02d"), nHour, nMin, nSec);
		::WritePrivateProfileString(_T("Info"), _T("Lot Run"), sData, m_sPathBuf);
	}
}

CString CReelMap::GetLotEd()
{
	CString sLot = _T("");
	//char szData[MAX_PATH];
	TCHAR szData[MAX_PATH];
	if (0 < ::GetPrivateProfileString(_T("Info"), _T("Lot End"), NULL, szData, sizeof(szData), m_sPathBuf))
		sLot = CString(szData);
	return sLot;
}

void CReelMap::SetLastSerial(int nSerial) 	// 릴맵 텍스트 파일의 수율정보를 업데이트함.
{
	if(nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.64"));
		return;
	}

	m_nLastShot = nSerial;
	m_sPathBuf = GetRmapPath(m_nLayer);

	CString sData;
	sData.Format(_T("%d"), nSerial);
	::WritePrivateProfileString(_T("Info"), _T("Marked Shot"), sData, m_sPathBuf);

	int nYear, nMonth, nDay, nHour, nMin, nSec;
	nYear = pDoc->WorkingInfo.Lot.CurTime.nYear; 
	nMonth = pDoc->WorkingInfo.Lot.CurTime.nMonth; 
	nDay = pDoc->WorkingInfo.Lot.CurTime.nDay; 
	nHour = pDoc->WorkingInfo.Lot.CurTime.nHour; 
	nMin = pDoc->WorkingInfo.Lot.CurTime.nMin; 
	nSec = pDoc->WorkingInfo.Lot.CurTime.nSec;

	if(!nYear && !nMonth && !nDay && !nHour && !nMin && !nSec)
		sData = _T("");
	else
		sData.Format(_T("%04d-%02d-%02d, %02d:%02d:%02d"), nYear, nMonth, nDay, nHour, nMin, nSec);
	::WritePrivateProfileString(_T("Info"), _T("Marked Date"), sData, m_sPathBuf);

	//UpdateRst();
}

void CReelMap::SetCompletedSerial(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.67"));
		return;
	}

	m_nCompletedShot = nSerial;

	CString sData;
	sData.Format(_T("%d"), nSerial);
	::WritePrivateProfileString(_T("Info"), _T("Completed Shot"), sData, m_sPathBuf);

	int nYear, nMonth, nDay, nHour, nMin, nSec;
	nYear = pDoc->WorkingInfo.Lot.CurTime.nYear;
	nMonth = pDoc->WorkingInfo.Lot.CurTime.nMonth;
	nDay = pDoc->WorkingInfo.Lot.CurTime.nDay;
	nHour = pDoc->WorkingInfo.Lot.CurTime.nHour;
	nMin = pDoc->WorkingInfo.Lot.CurTime.nMin;
	nSec = pDoc->WorkingInfo.Lot.CurTime.nSec;

	if (!nYear && !nMonth && !nDay && !nHour && !nMin && !nSec)
		sData = _T("");
	else
		sData.Format(_T("%04d-%02d-%02d, %02d:%02d:%02d"), nYear, nMonth, nDay, nHour, nMin, nSec);
	::WritePrivateProfileString(_T("Info"), _T("Completed Date"), sData, m_sPathBuf);

	//UpdateRst(nSerial);
}

BOOL CReelMap::GetRst(int nFrom, int nTo)
{
	return TRUE;
}

BOOL CReelMap::GetRst(int nSerial)
{
	return TRUE;
}


BOOL CReelMap::InitRst()
{
	int nPnl, nRow;
	int TotPnl = PNL_TOT;
	int nDumyPnl = 20;
// 	int nNodeX = pDoc->m_pPcsRgn->nCol;
// 	int nNodeY = pDoc->m_pPcsRgn->nRow;
	int nNodeX = PNLBUF_X;
	int nNodeY = PNLBUF_Y;

	int k, i;

 	CloseRst();

	m_nPnlBuf = TotPnl+nDumyPnl;
	if(!m_pPnlBuf)
	{
		m_pPnlBuf = new short**[m_nPnlBuf];
		for(nPnl=0; nPnl<m_nPnlBuf; nPnl++)
		{
			m_pPnlBuf[nPnl] = new short*[nNodeY];
			for(nRow=0; nRow<nNodeY; nRow++)
				m_pPnlBuf[nPnl][nRow] = new short[nNodeX];
		}
	}

	m_nTotPcs = 0;
	m_nGoodPcs = 0;
	m_nBadPcs = 0;
	for(i=0; i < MAX_DEF; i++)
		m_nDef[i] = 0;

	m_nTotStOut = 0;
	for(k=0; k < MAX_STRIP_NUM; k++)
	{
		m_nStripOut[k] = 0;
		m_nDefStrip[k] = 0;
		for(i=0; i<MAX_DEF; i++)
			m_nDefPerStrip[k][i] = 0;
	}

	return TRUE;
}

void CReelMap::CloseRst()
{
	int nPnl, nRow;
// 	int nNodeX = pDoc->m_pPcsRgn->nCol;
// 	int nNodeY = pDoc->m_pPcsRgn->nRow;
	int nNodeX = PNLBUF_X;
	int nNodeY = PNLBUF_Y;

	if(m_pPnlBuf)
	{
		for(nPnl=0; nPnl<m_nPnlBuf; nPnl++)
		{
			for(nRow=0; nRow<nNodeY; nRow++)
				delete[] m_pPnlBuf[nPnl][nRow];
			delete[] m_pPnlBuf[nPnl];
		}

		delete[] m_pPnlBuf;
		m_pPnlBuf = NULL;
		m_nPnlBuf = 0;
	}
}

void CReelMap::ClrRst()
{
	m_nTotPcs = 0;
	m_nGoodPcs = 0;
	m_nBadPcs = 0;

	int k, i;
	for(i=1; i < MAX_DEF; i++)
		m_nDef[i] = 0;

	m_nTotStOut = 0;
	for(k=0; k < MAX_STRIP_NUM; k++)
	{
		m_nStripOut[k] = 0;
		m_nDefStrip[k] = 0;
		for(i=1; i < MAX_DEF; i++)
			m_nDefPerStrip[k][i] = 0;
	}

	int nPnl, nRow, nCol;
	int TotPnl = PNL_TOT;
	int nNodeX = PNLBUF_X;
	int nNodeY = PNLBUF_Y;

	if(m_pPnlBuf)
	{
		for(nPnl=0; nPnl<TotPnl; nPnl++)
		{
			for(nRow=0; nRow<nNodeY; nRow++)
			{
				for(nCol=0; nCol<nNodeX; nCol++)
					m_pPnlBuf[nPnl][nRow][nCol] = 0;
			}
		}
	}
}

CString CReelMap::GetYieldPath(int nRmap)
{
	CString sPath;

	CString str;

	switch (nRmap)
	{
	case RMAP_UP:
		str = _T("YieldUp.txt");
		sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			pDoc->WorkingInfo.LastJob.sLotUp,
			pDoc->WorkingInfo.LastJob.sLayerUp,
			str);
		break;
	case RMAP_ALLUP:
		str = _T("YieldAll.txt");
		sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			pDoc->WorkingInfo.LastJob.sLotUp,
			pDoc->WorkingInfo.LastJob.sLayerUp,
			str);
		break;
	case RMAP_DN:
		str = _T("YieldDn.txt");
		sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			pDoc->WorkingInfo.LastJob.sLotUp,
			pDoc->WorkingInfo.LastJob.sLayerDn,
			str);
		break;
	case RMAP_ALLDN:
		str = _T("YieldAll.txt");
		sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			pDoc->WorkingInfo.LastJob.sLotUp,
			pDoc->WorkingInfo.LastJob.sLayerDn,
			str);
		break;
	case RMAP_INNER_UP:
		str = _T("YieldUp.txt");
			sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathItsFile,
				pDoc->WorkingInfo.LastJob.sModelUp, pDoc->WorkingInfo.LastJob.sEngItsCode,
				pDoc->WorkingInfo.LastJob.sInnerLotUp, pDoc->WorkingInfo.LastJob.sInnerLayerUp,
				str);
		break;
	case RMAP_INNER_DN:
		str = _T("YieldDn.txt");
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathItsFile,
			pDoc->WorkingInfo.LastJob.sModelUp, pDoc->WorkingInfo.LastJob.sEngItsCode,
			pDoc->WorkingInfo.LastJob.sInnerLotUp, pDoc->WorkingInfo.LastJob.sInnerLayerDn,
			str);
		break;
	case RMAP_INNER_ALLUP:
		str = _T("YieldAll.txt");
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathItsFile,
			pDoc->WorkingInfo.LastJob.sModelUp, pDoc->WorkingInfo.LastJob.sEngItsCode,
			pDoc->WorkingInfo.LastJob.sInnerLotUp, pDoc->WorkingInfo.LastJob.sInnerLayerUp,
			str);
		break;
	case RMAP_INNER_ALLDN:
		str = _T("YieldAll.txt");
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathItsFile,
			pDoc->WorkingInfo.LastJob.sModelUp, pDoc->WorkingInfo.LastJob.sEngItsCode,
			pDoc->WorkingInfo.LastJob.sInnerLotUp, pDoc->WorkingInfo.LastJob.sInnerLayerDn,
			str);
		break;
	case RMAP_INOUTER_UP:
		break;
	case RMAP_INOUTER_DN:
		break;
	case RMAP_ITS:
		str = _T("YieldIts.txt");
		sPath.Format(_T("%s%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathItsFile,
			pDoc->WorkingInfo.LastJob.sModelUp, pDoc->WorkingInfo.LastJob.sEngItsCode, //pDoc->m_sItsCode,
			str);
		break;
	}

	return sPath;
}

void CReelMap::ResetYield()
{
	m_stYield.nTot = 0; m_stYield.nGood = 0; m_stYield.nDef = 0;
	m_stYield.nTotSriptOut = 0;
	m_stYield.nDefStrip[0] = 0; m_stYield.nDefStrip[1] = 0; m_stYield.nDefStrip[2] = 0; m_stYield.nDefStrip[3] = 0;
	m_stYield.nStripOut[0] = 0; m_stYield.nStripOut[1] = 0; m_stYield.nStripOut[2] = 0; m_stYield.nStripOut[3] = 0;

	for (int k = 0; k < MAX_DEF; k++)
	{
		m_stYield.nDefA[k] = 0;

		for (int i = 0; i < MAX_STRIP_NUM; i++)
		{
			m_stYield.nDefPerStrip[i][k] = 0;
		}
	}
}

BOOL CReelMap::UpdateYield(int nSerial)
{
	if (nSerial <= 0)
	{
		AfxMessageBox(_T("Serial Error.68"));
		return 0;
	}

	m_sPathYield = GetYieldPath(m_nLayer);
	CString sPath = m_sPathYield;

	BOOL bExist = FALSE;
	CFileFind findfile;
	if (findfile.FindFile(sPath))
		bExist = TRUE;

	int nPnl = nSerial - 1;

	if (bExist && nPnl) // After first shot
	{
		if(ReadYield(nPnl, sPath))
			WriteYield(nSerial, sPath);
		else
		{
			ResetYield();
			WriteYield(nSerial, sPath);
		}
	}
	else // First Shot
	{
		ResetYield();
		WriteYield(nSerial, sPath);
	}
	Sleep(10);

	return TRUE;
}



BOOL CReelMap::UpdateReelmapYield()
{
	int k, i;
	CString strMenu, strItem, sCode, sDefNum, strData;

	int nMaxStrip;
#ifdef USE_CAM_MASTER
	nMaxStrip = ((CManagerReelmap*)m_pParent)->m_Master[0].GetStripNum(); // 총 스트립의 갯수
#else
#ifdef TEST_MODE
	nMaxStrip = 4;
#else
	nMaxStrip = MAX_STRIP;
#endif
#endif

	m_nGoodPcs = m_stYield.nGood;
	m_nBadPcs =  m_stYield.nDef;

	strData.Format(_T("%d"), m_stYield.nTot);
	::WritePrivateProfileString(_T("Info"), _T("Total Pcs"), strData, m_sPathBuf);
	strData.Format(_T("%d"), m_nGoodPcs);
	::WritePrivateProfileString(_T("Info"), _T("Good Pcs"), strData, m_sPathBuf);
	strData.Format(_T("%d"), m_nBadPcs);
	::WritePrivateProfileString(_T("Info"), _T("Bad Pcs"), strData, m_sPathBuf);

	for (i = 1; i < MAX_DEF; i++)
	{
		m_nDef[i] = m_stYield.nDefA[i];
		sCode.Format(_T("%d"), i);
		sDefNum.Format(_T("%d"), m_nDef[i]);
		::WritePrivateProfileString(_T("Info"), sCode, sDefNum, m_sPathBuf);
	}

	int nTotStOut = 0;
	for (k = 0; k < nMaxStrip; k++)
	{
		strItem.Format(_T("Strip%d"), k);
		m_nDefStrip[k] = m_stYield.nDefStrip[k];
		strData.Format(_T("%d"), m_nDefStrip[k]);
		::WritePrivateProfileString(_T("Info"), strItem, strData, m_sPathBuf);

		strItem.Format(_T("%d"), k);
		m_nStripOut[k] = m_stYield.nStripOut[k];
		strData.Format(_T("%d"), m_nStripOut[k]);
		::WritePrivateProfileString(_T("StripOut"), strItem, strData, m_sPathBuf);

		for (i = 1; i < MAX_DEF; i++)
		{
			strMenu.Format(_T("Strip%d"), k);
			strItem.Format(_T("%d"), i);
			m_nDefPerStrip[k][i] = m_stYield.nDefPerStrip[k][i];
			strData.Format(_T("%d"), m_nDefPerStrip[k][i]);
			::WritePrivateProfileString(strMenu, strItem, strData, m_sPathBuf);
		}
	}
	strData.Format(_T("%d"), m_stYield.nTotSriptOut);
	::WritePrivateProfileString(_T("StripOut"), _T("Total"), strData, m_sPathBuf);

	return TRUE;
}

int CReelMap::GetDefNum(int nDefCode)
{
	return m_stYield.nDefA[nDefCode];
	//return m_nDef[nDefCode];
}

int CReelMap::GetDefStrip(int nStrip)
{
	return m_stYield.nDefStrip[nStrip];
	//return m_nDefStrip[nStrip];
}

int CReelMap::GetDefStrip(int nStrip, int nDefCode)
{
	return m_stYield.nDefPerStrip[nStrip][nDefCode];
	//return m_nDefPerStrip[nStrip][nDefCode];
}

int CReelMap::GetStripOut(int nStrip)
{
	return m_stYield.nStripOut[nStrip];
	//return m_nStripOut[nStrip];
}

void CReelMap::GetPcsNum(int &nGood, int &nBad)
{
	nGood = m_stYield.nGood;
	nBad = m_stYield.nDef;
	//nGood = m_nGoodPcs;
	//nBad = m_nBadPcs;
}

void CReelMap::Clear()
{
	m_nSerial = 0;
	m_nLastShot = 0;
	m_nWritedSerial = 0;
	ClrFixPcs();
	ClrRst();
	ClrPcs();
	ClrPnlNum();
	ClrPnlDefNum();
}

int CReelMap::GetLastSerial()
{
	return m_nWritedSerial;
}

void CReelMap::SetFixPcs(int nSerial)
{
// 	if(!pDoc->WorkingInfo.LastJob.bContFixDef)
// 		return;

	int nNodeX = ((CManagerReelmap*)m_pParent)->m_Master[0].m_pPcsRgn->nCol;
	int nNodeY = ((CManagerReelmap*)m_pParent)->m_Master[0].m_pPcsRgn->nRow;
	int nR, nC, nDefCode;

	for(nR=0; nR<nNodeY; nR++)
	{
		for(nC=0; nC<nNodeX; nC++)
		{
			nDefCode = m_pPnlBuf[nSerial-1][nR][nC]; // DefCode
			if(nDefCode > 0)
				SetFixPcs(nSerial, nC, nR);
		}
	}
}

void CReelMap::SetFixPcs(int nSerial, int nCol, int nRow) // nShot : 0 ~ 
{
	if(nSerial < 1)
		return;

	int nRange = _tstoi(pDoc->WorkingInfo.LastJob.sNumRangeFixDef);
	int nS = (nSerial-1) % nRange;
	m_FixPcs[nS][nCol][nRow] = TRUE;
}

void CReelMap::ClrFixPcs()
{
	m_nPrevSerial[0] = 0;	// --
	m_nPrevSerial[1] = 0;	// ++

	for(int nS=0; nS<FIX_PCS_SHOT_MAX; nS++)
	{
		if(!nS)
		{
			m_nPrevSerial[0] = 0; // [0] : --
			m_nPrevSerial[1] = 0; // [1] : ++
		}

		for(int nC=0; nC<FIX_PCS_COL_MAX; nC++)
		{
			for(int nR=0; nR<FIX_PCS_ROW_MAX; nR++)
			{
				m_FixPcs[nS][nC][nR] = FALSE;
				if(!nS)
				{
					m_FixPcsPrev[nC][nR] = 0;
					m_FixPcsPrevStSerial[nC][nR] = 0;
				}		
			}
		}
	}
}

void CReelMap::ClrFixPcs(int nCol, int nRow)
{
	for(int nS=0; nS<FIX_PCS_SHOT_MAX; nS++)
	{
		m_FixPcs[nS][nCol][nRow] = FALSE;
	}
	m_FixPcsPrev[nCol][nRow] = 0;
	m_FixPcsPrevStSerial[nCol][nRow] = 0;
}

BOOL CReelMap::IsFixPcs(int nSerial, int &Col, int &Row)
{
	BOOL bRtn = FALSE;
	int nNodeX = ((CManagerReelmap*)m_pParent)->m_Master[0].m_pPcsRgn->nCol;
	int nNodeY = ((CManagerReelmap*)m_pParent)->m_Master[0].m_pPcsRgn->nRow;

	BOOL bOn = pDoc->WorkingInfo.LastJob.bContFixDef;
	int nRange = _tstoi(pDoc->WorkingInfo.LastJob.sNumRangeFixDef);
	int nJudge = _tstoi(pDoc->WorkingInfo.LastJob.sNumContFixDef);

// 	if(nSerial < nJudge)
// 		return bRtn;

	if(m_nPrevSerial[0] == nSerial)
		return bRtn;
	m_nPrevSerial[0] = nSerial;

	int nS;

	for(int nC=0; nC<nNodeX; nC++)
	{
		for(int nR=0; nR<nNodeY; nR++)
		{
			if(nSerial >= nRange)
			{
				nS = (nSerial-1) % nRange;
				if(nS == nRange-1)
					nS = 0;
				else
					nS++;

				if(m_FixPcs[nS][nC][nR]) // Defect PCS
				{
					if(m_FixPcsPrev[nC][nR] > 0)
						m_FixPcsPrev[nC][nR]--;
				}
				m_FixPcs[nS][nC][nR] = FALSE;
			}

			nS = (nSerial-1) % nRange;
			if(m_FixPcs[nS][nC][nR]) // Defect PCS
			{
				m_FixPcsPrev[nC][nR]++;
			}

			if(bOn)
			{
				if(m_FixPcsPrev[nC][nR] >= nJudge)
				{
					m_FixPcsRpt[nC][nR]++;
					Col = nC;
					Row = nR;
					ClrFixPcs(nC, nR);
					bRtn = TRUE;
				}				
			}
		}
	}

	return bRtn;
}

BOOL CReelMap::IsFixPcs(int nSerial, int* pCol, int* pRow, int &nTot, BOOL &bCont) // nTot : total of PCS Over nJudge
{
	BOOL bRtn = FALSE;
	int nNodeX = ((CManagerReelmap*)m_pParent)->m_Master[0].m_pPcsRgn->nCol;
	int nNodeY = ((CManagerReelmap*)m_pParent)->m_Master[0].m_pPcsRgn->nRow;

	BOOL bOn = pDoc->WorkingInfo.LastJob.bContFixDef;
	int nRange = _tstoi(pDoc->WorkingInfo.LastJob.sNumRangeFixDef);
	int nJudge = _tstoi(pDoc->WorkingInfo.LastJob.sNumContFixDef);

	if(m_nPrevSerial[0] == nSerial)
		return bRtn;
	m_nPrevSerial[0] = nSerial;

	int nS = (nSerial-1) % nRange;
	nTot = 0;

	for(int nC=0; nC<nNodeX; nC++)
	{
		for(int nR=0; nR<nNodeY; nR++)
		{
			if(nSerial >= nRange)
			{
				nS = (nSerial-1) % nRange;
				if(nS == nRange-1)
					nS = 0;
				else
					nS++;

				if(m_FixPcs[nS][nC][nR]) // Defect PCS
				{
					if(m_FixPcsPrev[nC][nR] > 0)
						m_FixPcsPrev[nC][nR]--;
				}
				m_FixPcs[nS][nC][nR] = FALSE;
			}

			nS = (nSerial-1) % nRange;
			if(m_FixPcs[nS][nC][nR]) // Defect PCS
			{
				if (!m_FixPcsPrev[nC][nR])
					m_FixPcsPrevStSerial[nC][nR] = nSerial;
				m_FixPcsPrev[nC][nR]++;
			}

			if(bOn)
			{
				if(m_FixPcsPrev[nC][nR] >= nJudge)
				{
					if (nSerial - m_FixPcsPrevStSerial[nC][nR] == nJudge - 1)
						bCont = TRUE;
					m_FixPcsRpt[nC][nR]++;
					pCol[nTot] = nC;
					pRow[nTot] = nR;
					nTot++;
					ClrFixPcs(nC, nR);
					bRtn = TRUE;
				}				
			}
		}
	}

	return bRtn;
}


BOOL CReelMap::MakeDir()
{
	CString str, sPath, Path[5];

	switch(m_nLayer)
	{
	case RMAP_UP:
		str = _T("ReelMapDataUp.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
		Path[3] = pDoc->WorkingInfo.LastJob.sLayerUp;
		Path[4] = _T("");
		break;
	case RMAP_ALLUP:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
		Path[3] = pDoc->WorkingInfo.LastJob.sLayerUp;
		Path[4] = _T("");
		break;
	case RMAP_DN:
		str = _T("ReelMapDataDn.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
		Path[3] = pDoc->WorkingInfo.LastJob.sLayerDn;
		Path[4] = _T("");
		break;
	case RMAP_ALLDN:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
		Path[3] = pDoc->WorkingInfo.LastJob.sLayerDn;
		Path[4] = _T("");
		break;
	case RMAP_INNER_UP:
		str = _T("ReelMapDataUp.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerUp;
		break;
	case RMAP_INNER_DN:
		str = _T("ReelMapDataDn.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerDn;
		break;
	case RMAP_INNER_ALLUP:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerUp;
		break;
	case RMAP_INNER_ALLDN:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerDn;
		break;
	case RMAP_INOUTER_UP:
		break;
	case RMAP_INOUTER_DN:
		break;
	case RMAP_ITS:
		str = _T("ReelMapDataIts.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = _T("");
		Path[4] = _T("");
		break;
	}

	if (!Path[0].IsEmpty() && !Path[1].IsEmpty())
	{
		sPath.Format(_T("%s%s"), Path[0], Path[1]);
		if (!pDoc->DirectoryExists(sPath))
			CreateDirectory(sPath, NULL);

		if (!Path[2].IsEmpty())
		{
			sPath.Format(_T("%s%s\\%s"), Path[0], Path[1], Path[2]);
			if (!pDoc->DirectoryExists(sPath))
				CreateDirectory(sPath, NULL);

			if (!Path[3].IsEmpty())
			{
				sPath.Format(_T("%s%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3]);
				if (!pDoc->DirectoryExists(sPath))
					CreateDirectory(sPath, NULL);

				if (!Path[4].IsEmpty())
				{
					sPath.Format(_T("%s%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], Path[4]);
					if (!pDoc->DirectoryExists(sPath))
						CreateDirectory(sPath, NULL);
				}
			}
		}
	}

	return TRUE;
}

BOOL CReelMap::MakeDirRmap()
{
	CString str, sPath, Path[5];

	switch (m_nLayer)
	{
	case RMAP_UP:
		str = _T("ReelMapDataUp.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
		Path[3] = pDoc->WorkingInfo.LastJob.sLayerUp;
		Path[4] = _T("");
		break;
	case RMAP_ALLUP:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
		Path[3] = pDoc->WorkingInfo.LastJob.sLayerUp;
		Path[4] = _T("");
		break;
	case RMAP_DN:
		str = _T("ReelMapDataDn.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
		Path[3] = pDoc->WorkingInfo.LastJob.sLayerDn;
		Path[4] = _T("");
		break;
	case RMAP_ALLDN:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
		Path[3] = pDoc->WorkingInfo.LastJob.sLayerDn;
		Path[4] = _T("");
		break;
	case RMAP_INNER_UP:
		str = _T("YieldUp.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerUp;
		break;
	case RMAP_INNER_DN:
		str = _T("YieldDn.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerDn;
		break;
	case RMAP_INNER_ALLUP:
		str = _T("YieldAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerUp;
		break;
	case RMAP_INNER_ALLDN:
		str = _T("YieldAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerDn;
		break;
	case RMAP_INOUTER_UP:
		break;
	case RMAP_INOUTER_DN:
		break;
	case RMAP_ITS:
		str = _T("YieldIts.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = _T("");
		Path[4] = _T("");
		break;
	}

	if (!Path[0].IsEmpty() && !Path[1].IsEmpty())
	{
		sPath.Format(_T("%s%s"), Path[0], Path[1]);
		if (!pDoc->DirectoryExists(sPath))
			CreateDirectory(sPath, NULL);

		if (!Path[2].IsEmpty())
		{
			sPath.Format(_T("%s%s\\%s"), Path[0], Path[1], Path[2]);
			if (!pDoc->DirectoryExists(sPath))
				CreateDirectory(sPath, NULL);

			if (!Path[3].IsEmpty())
			{
				sPath.Format(_T("%s%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3]);
				if (!pDoc->DirectoryExists(sPath))
					CreateDirectory(sPath, NULL);

				if (!Path[4].IsEmpty())
				{
					sPath.Format(_T("%s%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], Path[4]);
					if (!pDoc->DirectoryExists(sPath))
						CreateDirectory(sPath, NULL);
				}
			}
		}
	}

	return TRUE;
}

CString CReelMap::MakeDirRmapRestore()
{
	CString str, sPath, Path[5];

	switch (m_nLayer)
	{
	case RMAP_UP:
		str = _T("ReelMapDataUp.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
		Path[3] = pDoc->WorkingInfo.LastJob.sLayerUp;
		Path[4] = _T("");
		break;
	case RMAP_ALLUP:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
		Path[3] = pDoc->WorkingInfo.LastJob.sLayerUp;
		Path[4] = _T("");
		break;
	case RMAP_DN:
		str = _T("ReelMapDataDn.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
		Path[3] = pDoc->WorkingInfo.LastJob.sLayerDn;
		Path[4] = _T("");
		break;
	case RMAP_ALLDN:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
		Path[3] = pDoc->WorkingInfo.LastJob.sLayerDn;
		Path[4] = _T("");
		break;
	case RMAP_INNER_UP:
		str = _T("YieldUp.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerUp;
		break;
	case RMAP_INNER_DN:
		str = _T("YieldDn.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerDn;
		break;
	case RMAP_INNER_ALLUP:
		str = _T("YieldAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerUp;
		break;
	case RMAP_INNER_ALLDN:
		str = _T("YieldAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerDn;
		break;
	case RMAP_INOUTER_UP:
		break;
	case RMAP_INOUTER_DN:
		break;
	case RMAP_ITS:
		str = _T("YieldIts.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = _T("");
		Path[4] = _T("");
		break;
	}

	if (!Path[0].IsEmpty() && !Path[1].IsEmpty())
	{
		sPath.Format(_T("%s%s"), Path[0], Path[1]);
		if (!pDoc->DirectoryExists(sPath))
			CreateDirectory(sPath, NULL);

		if (!Path[2].IsEmpty())
		{
			sPath.Format(_T("%s%s\\%s"), Path[0], Path[1], Path[2]);
			if (!pDoc->DirectoryExists(sPath))
				CreateDirectory(sPath, NULL);

			if (!Path[3].IsEmpty())
			{
				sPath.Format(_T("%s%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3]);
				if (!pDoc->DirectoryExists(sPath))
					CreateDirectory(sPath, NULL);

				if (!Path[4].IsEmpty())
				{
					sPath.Format(_T("%s%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], Path[4]);
					if (!pDoc->DirectoryExists(sPath))
						CreateDirectory(sPath, NULL);

					sPath.Format(_T("%s%s\\%s\\%s\\%s\\Restore"), Path[0], Path[1], Path[2], Path[3], Path[4]);
					if (!pDoc->DirectoryExists(sPath))
						CreateDirectory(sPath, NULL);
				}
				else
				{
					sPath.Format(_T("%s%s\\%s\\%s\\Restore"), Path[0], Path[1], Path[2], Path[3]);
					if (!pDoc->DirectoryExists(sPath))
						CreateDirectory(sPath, NULL);
				}
			}
			else
			{
				sPath.Format(_T("%s%s\\%s\\Restore"), Path[0], Path[1], Path[2]);
				if (!pDoc->DirectoryExists(sPath))
					CreateDirectory(sPath, NULL);
			}
		}
		else
		{
			sPath.Format(_T("%s%s\\Restore"), Path[0], Path[1]);
			if (!pDoc->DirectoryExists(sPath))
				CreateDirectory(sPath, NULL);
		}
	}
	else
	{
		pView->MsgBox(_T("Error- Path[0].IsEmpty() || Path[1].IsEmpty()"));
		sPath = _T("");
	}

	return sPath;
}

BOOL CReelMap::MakeDirRmap(CString sModel, CString sLayer, CString sLot)
{
	CString str, sPath, Path[5];

	switch (m_nLayer)
	{
	case RMAP_UP:
		str = _T("ReelMapDataUp.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = sModel;
		Path[2] = sLot;
		Path[3] = sLayer;
		Path[4] = _T("");
		break;
	case RMAP_ALLUP:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = sModel;
		Path[2] = sLot;
		Path[3] = sLayer;
		Path[4] = _T("");
		break;
	case RMAP_DN:
		str = _T("ReelMapDataDn.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = sModel;
		Path[2] = sLot;
		Path[3] = sLayer;
		Path[4] = _T("");
		break;
	case RMAP_ALLDN:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = sModel;
		Path[2] = sLot;
		Path[3] = sLayer;
		Path[4] = _T("");
		break;
	case RMAP_INNER_UP:
		str = _T("YieldUp.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = sModel;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = sLot;
		Path[4] = sLayer;
		break;
	case RMAP_INNER_DN:
		str = _T("YieldDn.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = sModel;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = sLot;
		Path[4] = sLayer;
		break;
	case RMAP_INNER_ALLUP:
		str = _T("YieldAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = sModel;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = sLot;
		Path[4] = sLayer;
		break;
	case RMAP_INNER_ALLDN:
		str = _T("YieldAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = sModel;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = sLot;
		Path[4] = sLayer;
		break;
	case RMAP_INOUTER_UP:
		break;
	case RMAP_INOUTER_DN:
		break;
	case RMAP_ITS:
		str = _T("YieldIts.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = Path[1] = sModel;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = _T("");
		Path[4] = _T("");
		break;
	}

	if (!Path[0].IsEmpty() && !Path[1].IsEmpty())
	{
		sPath.Format(_T("%s%s"), Path[0], Path[1]);
		if (!pDoc->DirectoryExists(sPath))
			CreateDirectory(sPath, NULL);

		if (!Path[2].IsEmpty())
		{
			sPath.Format(_T("%s%s\\%s"), Path[0], Path[1], Path[2]);
			if (!pDoc->DirectoryExists(sPath))
				CreateDirectory(sPath, NULL);

			if (!Path[3].IsEmpty())
			{
				sPath.Format(_T("%s%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3]);
				if (!pDoc->DirectoryExists(sPath))
					CreateDirectory(sPath, NULL);

				if (!Path[4].IsEmpty())
				{
					sPath.Format(_T("%s%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], Path[4]);
					if (!pDoc->DirectoryExists(sPath))
						CreateDirectory(sPath, NULL);
				}
			}
		}
	}

	return TRUE;
}

CString CReelMap::MakeDirRmapRestore(CString sModel, CString sLayer, CString sLot)
{
	CString str, sPath, Path[5];

	switch (m_nLayer)
	{
	case RMAP_UP:
		str = _T("ReelMapDataUp.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = sModel;
		Path[2] = sLot;
		Path[3] = sLayer;
		Path[4] = _T("");
		break;
	case RMAP_ALLUP:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = sModel;
		Path[2] = sLot;
		Path[3] = sLayer;
		Path[4] = _T("");
		break;
	case RMAP_DN:
		str = _T("ReelMapDataDn.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = sModel;
		Path[2] = sLot;
		Path[3] = sLayer;
		Path[4] = _T("");
		break;
	case RMAP_ALLDN:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = sModel;
		Path[2] = sLot;
		Path[3] = sLayer;
		Path[4] = _T("");
		break;
	case RMAP_INNER_UP:
		str = _T("YieldUp.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = sModel;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = sLot;
		Path[4] = sLayer;
		break;
	case RMAP_INNER_DN:
		str = _T("YieldDn.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = sModel;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = sLot;
		Path[4] = sLayer;
		break;
	case RMAP_INNER_ALLUP:
		str = _T("YieldAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = sModel;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = sLot;
		Path[4] = sLayer;
		break;
	case RMAP_INNER_ALLDN:
		str = _T("YieldAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = sModel;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = sLot;
		Path[4] = sLayer;
		break;
	case RMAP_INOUTER_UP:
		break;
	case RMAP_INOUTER_DN:
		break;
	case RMAP_ITS:
		str = _T("YieldIts.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = Path[1] = sModel;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = _T("");
		Path[4] = _T("");
		break;
	}

	if (!Path[0].IsEmpty() && !Path[1].IsEmpty())
	{
		sPath.Format(_T("%s%s"), Path[0], Path[1]);
		if (!pDoc->DirectoryExists(sPath))
			CreateDirectory(sPath, NULL);

		if (!Path[2].IsEmpty())
		{
			sPath.Format(_T("%s%s\\%s"), Path[0], Path[1], Path[2]);
			if (!pDoc->DirectoryExists(sPath))
				CreateDirectory(sPath, NULL);

			if (!Path[3].IsEmpty())
			{
				sPath.Format(_T("%s%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3]);
				if (!pDoc->DirectoryExists(sPath))
					CreateDirectory(sPath, NULL);

				if (!Path[4].IsEmpty())
				{
					sPath.Format(_T("%s%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], Path[4]);
					if (!pDoc->DirectoryExists(sPath))
						CreateDirectory(sPath, NULL);
				}
			}
		}
	}

	return sPath;
}

void CReelMap::SetPathAtBuf()
{
	m_sPathBuf = GetRmapPath(m_nLayer);
}

CString CReelMap::GetRmapPath(int nRmap)
{
	CString sPath = _T("");
	CString Path[5], str;

	switch (nRmap)
	{
	case RMAP_UP:
		str = _T("ReelMapDataUp.txt");
		if (pDoc->m_bDoneChgLot || !pDoc->m_bNewLotShare[0])
		{
			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sLotUp,
				pDoc->WorkingInfo.LastJob.sLayerUp,
				str);
		}
		else if (!pDoc->m_bDoneChgLot && pDoc->m_bNewLotShare[0])
		{
			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->Status.PcrShare[0].sLot,
				pDoc->WorkingInfo.LastJob.sLayerUp,
				str);
		}
		break;
	case RMAP_ALLUP:
		str = _T("ReelMapDataAll.txt");
		if (pDoc->m_bDoneChgLot || !pDoc->m_bNewLotShare[0])
		{
			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sLotUp,
				pDoc->WorkingInfo.LastJob.sLayerUp,
				str);
		}
		else if (!pDoc->m_bDoneChgLot && pDoc->m_bNewLotShare[0])
		{
			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->Status.PcrShare[0].sLot,
				pDoc->WorkingInfo.LastJob.sLayerUp,
				str);
		}
		break;
	case RMAP_DN:
		str = _T("ReelMapDataDn.txt");
		if (pDoc->m_bDoneChgLot || !pDoc->m_bNewLotShare[1])
		{
			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sLotUp,
				pDoc->WorkingInfo.LastJob.sLayerDn,
				str);
		}
		else if (!pDoc->m_bDoneChgLot && pDoc->m_bNewLotShare[1])
		{
			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->Status.PcrShare[0].sLot,
				pDoc->WorkingInfo.LastJob.sLayerDn,
				str);
		}
		break;
	case RMAP_ALLDN:
		str = _T("ReelMapDataAll.txt");
		if (pDoc->m_bDoneChgLot || !pDoc->m_bNewLotShare[1])
		{
			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sLotUp,
				pDoc->WorkingInfo.LastJob.sLayerDn,
				str);
		}
		else if (!pDoc->m_bDoneChgLot && pDoc->m_bNewLotShare[1])
		{
			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->Status.PcrShare[0].sLot,
				pDoc->WorkingInfo.LastJob.sLayerDn,
				str);
		}
		break;
	case RMAP_INNER_UP:
		str = _T("ReelMapDataUp.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerUp;
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], Path[4], str);
		break;
	case RMAP_INNER_DN:
		str = _T("ReelMapDataDn.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerDn;
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], Path[4], str);
		break;
	case RMAP_INNER_ALLUP:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerUp;
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], Path[4], str);
		break;
	case RMAP_INNER_ALLDN:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerDn;
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], Path[4], str);
		break;
	case RMAP_INOUTER_UP:
		str = _T("ReelMapDataIO.txt");
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathItsFile,
			pDoc->WorkingInfo.LastJob.sModelUp, pDoc->WorkingInfo.LastJob.sEngItsCode,
			pDoc->WorkingInfo.LastJob.sLotUp, pDoc->WorkingInfo.LastJob.sLayerUp,
			str);
		break;
	case RMAP_INOUTER_DN:
		str = _T("ReelMapDataIO.txt");
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathItsFile,
			pDoc->WorkingInfo.LastJob.sModelUp, pDoc->WorkingInfo.LastJob.sEngItsCode,
			pDoc->WorkingInfo.LastJob.sLotUp, pDoc->WorkingInfo.LastJob.sLayerDn,
			str);
		break;
	case RMAP_ITS:
		pDoc->GetCurrentInfoEng();
		str = _T("ReelMapDataIts.txt");
		sPath.Format(_T("%s%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathItsFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			pDoc->WorkingInfo.LastJob.sEngItsCode,	//pDoc->m_sItsCode,
			str);
		break;
	}

	return sPath;
}

void CReelMap::UpdateProcessNum(CString sProcessNum, int nLayer)
{
	::WritePrivateProfileString(_T("Info"), _T("Process Code"), sProcessNum, GetRmapPath(nLayer));
}

void CReelMap::UpdateTotVel(CString sVel, int nLayer)
{
	::WritePrivateProfileString(_T("Info"), _T("Entire Speed"), sVel, GetRmapPath(nLayer));
}



void CReelMap::StartThreadRemakeReelmap() 
{
	m_bRtnThreadRemakeReelmap = FALSE;
	m_bThreadAliveRemakeReelmap = TRUE;	
	m_ThreadTaskRemakeReelmap.Start( GetSafeHwnd(),this,ThreadProcRemakeReelmap);// Start the thread
}

void CReelMap::StopThreadRemakeReelmap() 
{
	m_ThreadTaskRemakeReelmap.Stop();// Stop the thread
}

// Home thread body
BOOL CReelMap::ThreadProcRemakeReelmap( LPVOID lpContext )
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CReelMap* pThread = reinterpret_cast< CReelMap* >( lpContext );
//	DWORD dwTimePeriod = 10; // 10ms sec sleep

	pThread->m_bRtnThreadRemakeReelmap = TRUE;
	pThread->m_bThreadAliveRemakeReelmap = TRUE;	
	
	//pThread->m_cs.Lock();
	pThread->m_bRtnThreadRemakeReelmap = pThread->RemakeReelmap();
	//pThread->m_cs.Unlock();

	pThread->m_bThreadAliveRemakeReelmap = FALSE;

	return (pThread->m_bRtnThreadRemakeReelmap);
}


BOOL CReelMap::GetNodeXYonRmap(int &nNodeX, int &nNodeY, CString sPath)
{
	TCHAR szData[MAX_PATH];
	CString sPnl, sRow, sLine;
	int nX, nY, nSt = 0;

	nNodeX = 0;
	nNodeY = 0;

	for (nY = 0; nY<10000; nY++)
	{
		sPnl = _T("1");
		sRow.Format(_T("%02d"), nY);
		if (0 >= ::GetPrivateProfileString(sPnl, sRow, NULL, szData, sizeof(szData), sPath))
		{
			nNodeX = nY;
			break;
		}
		else
		{
			sLine = CString(szData);
			for (nX = 0; nX<100000; nX++)
			{
				nSt = sLine.Find(',', nSt + 1);
				if (nSt < 0)
					break;
				else
					nNodeY++;
			}
			nNodeY++;
		}
	}

	if (!nNodeX || !nNodeY)
	{
		nNodeX = ((CManagerReelmap*)m_pParent)->m_Master[0].m_pPcsRgn->nCol; // on Cam
		nNodeY = ((CManagerReelmap*)m_pParent)->m_Master[0].m_pPcsRgn->nRow; // on Cam
	}

	return TRUE;
}

void CReelMap::RestoreReelmap()
{
	CString sPath = MakeDirRmapRestore();
	int nLastDir = GetLastRmapRestoreDir(sPath);

	CString sUserRestoreDir;
	sUserRestoreDir.Format(_T("%s\\%d"), sPath, nLastDir+1);
	
	CFileFind finder;
	if (!pDoc->DirectoryExists(sUserRestoreDir))
		CreateDirectory(sUserRestoreDir, NULL);

	CString Path[5], str, sSrc, sDest;

	switch (m_nLayer)
	{
	case RMAP_UP:
		str = _T("ReelMapDataUp.txt");
		if (pDoc->m_bDoneChgLot || !pDoc->m_bNewLotShare[0])
		{
			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sLotUp,
				pDoc->WorkingInfo.LastJob.sLayerUp,
				str);
		}
		else if (!pDoc->m_bDoneChgLot && pDoc->m_bNewLotShare[0])
		{
			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->Status.PcrShare[0].sLot,
				pDoc->WorkingInfo.LastJob.sLayerUp,
				str);
		}
		break;
	case RMAP_ALLUP:
		str = _T("ReelMapDataAll.txt");
		if (pDoc->m_bDoneChgLot || !pDoc->m_bNewLotShare[0])
		{
			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sLotUp,
				pDoc->WorkingInfo.LastJob.sLayerUp,
				str);
		}
		else if (!pDoc->m_bDoneChgLot && pDoc->m_bNewLotShare[0])
		{
			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->Status.PcrShare[0].sLot,
				pDoc->WorkingInfo.LastJob.sLayerUp,
				str);
		}
		break;
	case RMAP_DN:
		str = _T("ReelMapDataDn.txt");
		if (pDoc->m_bDoneChgLot || !pDoc->m_bNewLotShare[1])
		{
			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sLotUp,
				pDoc->WorkingInfo.LastJob.sLayerDn,
				str);
		}
		else if (!pDoc->m_bDoneChgLot && pDoc->m_bNewLotShare[1])
		{
			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->Status.PcrShare[0].sLot,
				pDoc->WorkingInfo.LastJob.sLayerDn,
				str);
		}
		break;
	case RMAP_ALLDN:
		str = _T("ReelMapDataAll.txt");
		if (pDoc->m_bDoneChgLot || !pDoc->m_bNewLotShare[1])
		{
			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sLotUp,
				pDoc->WorkingInfo.LastJob.sLayerDn,
				str);
		}
		else if (!pDoc->m_bDoneChgLot && pDoc->m_bNewLotShare[1])
		{
			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->Status.PcrShare[0].sLot,
				pDoc->WorkingInfo.LastJob.sLayerDn,
				str);
		}
		break;
	case RMAP_INNER_UP:
		str = _T("ReelMapDataUp.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerUp;
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], Path[4], str);
		break;
	case RMAP_INNER_DN:
		str = _T("ReelMapDataDn.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerDn;
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], Path[4], str);
		break;
	case RMAP_INNER_ALLUP:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerUp;
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], Path[4], str);
		break;
	case RMAP_INNER_ALLDN:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerDn;
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], Path[4], str);
		break;
	case RMAP_INOUTER_UP:
		str = _T("ReelMapDataIO.txt");
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathItsFile,
			pDoc->WorkingInfo.LastJob.sModelUp, pDoc->WorkingInfo.LastJob.sEngItsCode,
			pDoc->WorkingInfo.LastJob.sLotUp, pDoc->WorkingInfo.LastJob.sLayerUp,
			str);
		break;
	case RMAP_INOUTER_DN:
		str = _T("ReelMapDataIO.txt");
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathItsFile,
			pDoc->WorkingInfo.LastJob.sModelUp, pDoc->WorkingInfo.LastJob.sEngItsCode,
			pDoc->WorkingInfo.LastJob.sLotUp, pDoc->WorkingInfo.LastJob.sLayerDn,
			str);
		break;
	case RMAP_ITS:
		pDoc->GetCurrentInfoEng();
		str = _T("ReelMapDataIts.txt");
		sPath.Format(_T("%s%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathItsFile,
			pDoc->WorkingInfo.LastJob.sModelUp, pDoc->WorkingInfo.LastJob.sEngItsCode,	//pDoc->m_sItsCode,
			str);
		break;
	}

	sSrc = sPath;
	sDest.Format(_T("%s\\%s"), sUserRestoreDir, str);

	CMyFile *pFile;
	pFile = new CMyFile();

	BOOL bRst = pFile->Copy(sSrc, sDest);			// 파일을 복사한다.

	delete pFile;
}

int CReelMap::GetLastRmapRestoreDir(CString strPath)
{
	CString strFileName;

	CFileFind cFile;
	BOOL bExist = cFile.FindFile(strPath);
	if(!bExist)
	{
		return 0; // 존재하지 않음.
	}

	int nFirstFolder = 1000000;
	int nLastFolder = 0;

	strPath += _T("\\*.*");
	bExist = cFile.FindFile(strPath);

	while(bExist)
	{
		bExist = cFile.FindNextFile();
		if(cFile.IsDots()) continue;
		if(cFile.IsDirectory())
		{
			// 폴더명을 얻음.
			strFileName = cFile.GetFileName();

			int nFileNum = _tstoi(strFileName);
			if(nFileNum < nFirstFolder)
				nFirstFolder = nFileNum;
			if(nFileNum > nLastFolder)
				nLastFolder = nFileNum;
		}
	}

	return nLastFolder;
}

void CReelMap::StartThreadReloadReelmap()
{
	m_bRtnThreadReloadReelmap = FALSE;
	m_bThreadAliveReloadReelmap = TRUE;
	m_ThreadTaskReloadReelmap.Start( GetSafeHwnd(),this,ThreadProcReloadReelmap);// Start the thread
}

void CReelMap::StopThreadReloadReelmap()
{
	m_ThreadTaskReloadReelmap.Stop();// Stop the thread
}

// Home thread body
BOOL CReelMap::ThreadProcReloadReelmap( LPVOID lpContext )
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CReelMap* pThread = reinterpret_cast< CReelMap* >( lpContext );
//	DWORD dwTimePeriod = 10; // 10ms sec sleep

	pThread->m_bRtnThreadReloadReelmap = FALSE;
	pThread->m_bThreadAliveReloadReelmap = TRUE;	
	
	//pThread->m_cs.Lock();
	int nSerial = pDoc->GetLastShotMk();	// m_pDlgFrameHigh에서 얻거나 없으면, sPathOldFile폴더의 ReelMapDataDn.txt에서 _T("Info"), _T("Marked Shot") 찾음.
	pThread->m_bRtnThreadReloadReelmap = pThread->ReloadReelmap(pThread->m_nLastOnThread);
	//pThread->m_cs.Unlock();

	pThread->m_bThreadAliveReloadReelmap = FALSE;

	return (pThread->m_bRtnThreadReloadReelmap);
}


BOOL CReelMap::IsDoneReloadReelmap()
{
	return m_bDoneReloadReelmap;
}

int CReelMap::GetProgressReloadReelmap()
{
	if(m_nTotalProgressReloadReelmap <= 0)
		return 0;

	double dA = (double)m_nProgressReloadReelmap;
	double dB = (double)m_nTotalProgressReloadReelmap;
	double dC = 100.0 * dA / dB;
	int nC = int(dC);

	return nC;
}

BOOL CReelMap::ReloadReelmap()
{
	//int nSerial;
	BOOL bRtn;
	
	//nSerial = pDoc->GetLastShotMk();
	int nLastOnThread = pDoc->GetLastShotMk();	// m_pDlgFrameHigh에서 얻거나 없으면, sPathOldFile폴더의 ReelMapDataDn.txt에서 _T("Info"), _T("Marked Shot") 찾음.
	m_nLastOnThread = nLastOnThread;	// m_pDlgFrameHigh에서 얻거나 없으면, sPathOldFile폴더의 ReelMapDataDn.txt에서 _T("Info"), _T("Marked Shot") 찾음.

	//if(nSerial > 0)
	if(m_nLastOnThread > 0)
	{
		//bRtn = ReloadReelmap(nSerial);
		m_bDoneReloadReelmap = FALSE;
		m_nProgressReloadReelmap = 0;
		m_nTotalProgressReloadReelmap = 0;
		StartThreadReloadReelmap();
		bRtn = TRUE;
	}
	else 
		bRtn = FALSE;

	return bRtn;
}

BOOL CReelMap::ReloadReelmap(int nTo)
{
	if(!m_pPnlBuf)
	{
		AfxMessageBox(_T("Memory is not allocated.- PnlBuf"));
		m_bDoneReloadReelmap = TRUE;
		return FALSE;
	}

	int nMaxStrip;
#ifdef USE_CAM_MASTER
	nMaxStrip = ((CManagerReelmap*)m_pParent)->m_Master[0].GetStripNum(); // 총 스트립의 갯수
#else
#ifdef TEST_MODE
	nMaxStrip = 4;
#else
	nMaxStrip = MAX_STRIP;
#endif
#endif

	int nPnl, nRow, nCol, nDefCode, nStrip, nC, nR;
	CString sPnl, sRow, sVal;
	TCHAR sep[] = { _T(",/;\r\n\t") };
	TCHAR szData[MAX_PATH];

	int nNodeX = ((CManagerReelmap*)m_pParent)->m_Master[0].m_pPcsRgn->nCol;
	int nNodeY = ((CManagerReelmap*)m_pParent)->m_Master[0].m_pPcsRgn->nRow;
	int nTotPcs = nNodeX * nNodeY;
	int nStripPcs = nTotPcs / nMaxStrip;

	int nDefStrip[MAX_STRIP];
	

//  InitRst();
	ClrRst();
	m_nTotPcs = 0;
	m_nGoodPcs = 0;
	m_nBadPcs = 0;
	
	m_nTotalProgressReloadReelmap = nTo*(nNodeX*nNodeY+4)+4*MAX_DEF+MAX_DEF;
	m_nProgressReloadReelmap=0;

	for(nPnl=0; nPnl<nTo; nPnl++)
	{
		if (!m_bThreadAliveReloadReelmap)
		{
			AfxMessageBox(_T("Thread is stopped on reloading reelmap.- PnlBuf"));
			m_bDoneReloadReelmap = TRUE;
			return FALSE;
		}

		nDefStrip[0] = 0; nDefStrip[1] = 0; nDefStrip[2] = 0; nDefStrip[3] = 0;

		for(nRow=0; nRow<nNodeX; nRow++)
		{
			sPnl.Format(_T("%d"), nPnl+1);
			sRow.Format(_T("%02d"), nRow);
// 			sRow.Format(_T("%d"), nRow+1);
			if (0 < ::GetPrivateProfileString(sPnl, sRow, NULL, szData, sizeof(szData), m_sPathBuf))
			{
				for(nCol=0; nCol<nNodeY; nCol++)
				{
					m_nProgressReloadReelmap++;

					if(nCol==0)
						sVal = _tcstok(szData,sep);
					else
						sVal = _tcstok(NULL,sep);//strtok

					nDefCode = _tstoi(sVal);

					nR = (nNodeY-1)-nCol;
					nC = nRow;

					m_pPnlBuf[nPnl][nR][nC] = (short)nDefCode;
					m_nDef[nDefCode]++;

					nStrip = int(nR / (nNodeY / nMaxStrip));
					if(nStrip > -1 && nStrip < nMaxStrip)
					{
						if(nDefCode > 0)
						{
							nDefStrip[nStrip]++;
							m_nDefStrip[nStrip]++;
							m_nDefPerStrip[nStrip][nDefCode]++;
						}
					}

					m_nTotPcs++;
					if(nDefCode > 0)
						m_nBadPcs++;
					else
						m_nGoodPcs++;
				}
			}
		}

		double dStOutRto = _tstof(pDoc->WorkingInfo.LastJob.sStripOutRatio) / 100.0; //atof
		for(nStrip=0; nStrip < nMaxStrip; nStrip++)
		{
			m_nProgressReloadReelmap++;

			if(nDefStrip[nStrip] >= nStripPcs * dStOutRto)
				m_nStripOut[nStrip]++;
		}
	}


	// 수율 데이타를 갱신함.

	// Piece infomation..............
	CString strData, strMenu, strItem;
	int k, i;

	strData.Format(_T("%d"), m_nTotPcs);
	::WritePrivateProfileString(_T("Info"), _T("Total Pcs"), strData, m_sPathBuf);

	strData.Format(_T("%d"), m_nGoodPcs);
	::WritePrivateProfileString(_T("Info"), _T("Good Pcs"), strData, m_sPathBuf);

	strData.Format(_T("%d"), m_nBadPcs);
	::WritePrivateProfileString(_T("Info"), _T("Bad Pcs"), strData, m_sPathBuf);

	int nTotStOut = 0;
	for(k=0; k < nMaxStrip; k++)
	{
		strMenu.Format(_T("Strip%d"), k);
		strData.Format(_T("%d"), m_nDefStrip[k]);
		::WritePrivateProfileString(_T("Info"), strMenu, strData, m_sPathBuf);
		
		strMenu.Format(_T("%d"), k);
		strData.Format(_T("%d"), m_nStripOut[k]);
		::WritePrivateProfileString(_T("StripOut"), strMenu, strData, m_sPathBuf);
		nTotStOut += m_nStripOut[k];

		for(i=1; i < MAX_DEF; i++)
		{
			m_nProgressReloadReelmap++;

			strItem.Format(_T("Strip%d"), k);
			strMenu.Format(_T("%d"), i);
			strData.Format(_T("%d"), m_nDefPerStrip[k][i]);
			::WritePrivateProfileString(strItem, strMenu, strData, m_sPathBuf);
		}			
	}
	strData.Format(_T("%d"), m_nTotStOut=nTotStOut);
	::WritePrivateProfileString(_T("StripOut"), _T("Total"), strData, m_sPathBuf);

	for(i=1; i < MAX_DEF; i++)
	{
		m_nProgressReloadReelmap++;

		strMenu.Format(_T("%d"), i);
		strData.Format(_T("%d"), m_nDef[i]); // 불량이름별 불량수
		::WritePrivateProfileString(_T("Info"), strMenu, strData, m_sPathBuf);
	}

	m_bDoneReloadReelmap = TRUE;
	return TRUE;
}


char* CReelMap::StrToChar(CString str) // char* returned must be deleted... 
{
	char*		szStr = NULL;
	wchar_t*	wszStr;
	int				nLenth;

	USES_CONVERSION;
	//1. CString to wchar_t* conversion
	wszStr = T2W(str.GetBuffer(str.GetLength()));

	//2. wchar_t* to char* conversion
	nLenth = WideCharToMultiByte(CP_ACP, 0, wszStr, -1, NULL, 0, NULL, NULL); //char* 형에 대한길이를 구함 
	szStr = new char[nLenth];  //메모리 할당 

							   //3. wchar_t* to char* conversion
	WideCharToMultiByte(CP_ACP, 0, wszStr, -1, szStr, nLenth, 0, 0);
	return szStr;
}

void CReelMap::StrToChar(CString str, char* pCh) // char* returned must be deleted... 
{
	wchar_t*	wszStr;
	int				nLenth;

	USES_CONVERSION;
	//1. CString to wchar_t* conversion
	wszStr = T2W(str.GetBuffer(str.GetLength()));

	//2. wchar_t* to char* conversion
	nLenth = WideCharToMultiByte(CP_ACP, 0, wszStr, -1, NULL, 0, NULL, NULL); //char* 형에 대한길이를 구함 

																			  //3. wchar_t* to char* conversion
	WideCharToMultiByte(CP_ACP, 0, wszStr, -1, pCh, nLenth, 0, 0);
	return;
}

BOOL CReelMap::MakeDirYield(CString sPath)
{
	CString str, Path[5];

	switch (m_nLayer)
	{
	case RMAP_UP:
		str = _T("YieldUp.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
		Path[3] = pDoc->WorkingInfo.LastJob.sLayerUp;
		Path[4] = _T("");
		break;
	case RMAP_ALLUP:
		str = _T("YieldAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
		Path[3] = pDoc->WorkingInfo.LastJob.sLayerUp;
		Path[4] = _T("");
		break;
	case RMAP_DN:
		str = _T("YieldDn.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
		Path[3] = pDoc->WorkingInfo.LastJob.sLayerDn;
		Path[4] = _T("");
		break;
	case RMAP_ALLDN:
		str = _T("YieldAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
		Path[3] = pDoc->WorkingInfo.LastJob.sLayerDn;
		Path[4] = _T("");
		break;
	case RMAP_INNER_UP:
		str = _T("YieldUp.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerUp;
		break;
	case RMAP_INNER_DN:
		str = _T("YieldDn.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerDn;
		break;
	case RMAP_INNER_ALLUP:
		str = _T("YieldAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerUp;
		break;
	case RMAP_INNER_ALLDN:
		str = _T("YieldAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerDn;
		break;
	case RMAP_INOUTER_UP:
		break;
	case RMAP_INOUTER_DN:
		break;
	case RMAP_ITS:
		str = _T("YieldIts.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = _T("");
		Path[4] = _T("");
		break;
	}

	if (!Path[0].IsEmpty() && !Path[1].IsEmpty())
	{
		sPath.Format(_T("%s%s"), Path[0], Path[1]);
		if (!pDoc->DirectoryExists(sPath))
			CreateDirectory(sPath, NULL);

		if (!Path[2].IsEmpty())
		{
			sPath.Format(_T("%s%s\\%s"), Path[0], Path[1], Path[2]);
			if (!pDoc->DirectoryExists(sPath))
				CreateDirectory(sPath, NULL);

			if (!Path[3].IsEmpty())
			{
				sPath.Format(_T("%s%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3]);
				if (!pDoc->DirectoryExists(sPath))
					CreateDirectory(sPath, NULL);

				if (!Path[4].IsEmpty())
				{
					sPath.Format(_T("%s%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], Path[4]);
					if (!pDoc->DirectoryExists(sPath))
						CreateDirectory(sPath, NULL);
				}
			}
		}
	}

	return TRUE;
}

BOOL CReelMap::ReadYield(int nSerial, CString sPath)
{
	int dwStart = GetTickCount();

	int nMaxStrip;
#ifdef USE_CAM_MASTER
	nMaxStrip = ((CManagerReelmap*)m_pParent)->m_Master[0].GetStripNum(); // 총 스트립의 갯수
#else
#ifdef TEST_MODE
	nMaxStrip = 4;
#else
	nMaxStrip = MAX_STRIP;
#endif
#endif

	int i, k;
	TCHAR szData[200];

	CString sCode, strMenu, strItem, sMsg;

	strMenu.Format(_T("%d"), nSerial);
	for (i = 1; i < MAX_DEF; i++)
	{
		sCode.Format(_T("%d"), i);
		if (0 < ::GetPrivateProfileString(strMenu, sCode, NULL, szData, sizeof(szData), sPath))
		{
			m_stYield.nDefA[i] = _ttoi(szData);
		}
		else
		{
			if (nSerial == 1)
			{
				m_stYield.nDefA[i] = 0;
			}
			else
			{
				//sMsg.Format(_T("이전 수율 읽기 오류 : Shot(%d)-불량(%d)\r\n%s"), nSerial, i, sPath);
				//AfxMessageBox(sMsg, MB_ICONWARNING | MB_OK);
				return FALSE;
			}
		}
	}

	if (0 < ::GetPrivateProfileString(strMenu, _T("Total Pcs"), NULL, szData, sizeof(szData), sPath))
	{
		m_stYield.nTot = _ttoi(szData);
	}
	else
	{
		if (nSerial == 1)
		{
			m_stYield.nTot = 0;
		}
		else
		{
			sMsg.Format(_T("이전 수율 읽기 오류 : Shot(%d)-Total Pcs\r\n%s"), nSerial, sPath);
			AfxMessageBox(sMsg, MB_ICONWARNING | MB_OK);
			return FALSE;
		}
	}

	if (0 < ::GetPrivateProfileString(strMenu, _T("Good Pcs"), NULL, szData, sizeof(szData), sPath))
	{
		m_stYield.nGood = _ttoi(szData);
	}
	else
	{
		if (nSerial == 1)
		{
			m_stYield.nGood = 0;
		}
		else
		{
			sMsg.Format(_T("이전 수율 읽기 오류 : Shot(%d)-Good Pcs\r\n%s"), nSerial, sPath);
			AfxMessageBox(sMsg, MB_ICONWARNING | MB_OK);
			return FALSE;
		}
	}

	if (0 < ::GetPrivateProfileString(strMenu, _T("Bad Pcs"), NULL, szData, sizeof(szData), sPath))
	{
		m_stYield.nDef = _ttoi(szData);
	}
	else
	{
		if (nSerial == 1)
		{
			m_stYield.nDef = 0;
		}
		else
		{
			sMsg.Format(_T("이전 수율 읽기 오류 : Shot(%d)-Bad Pcs\r\n%s"), nSerial, sPath);
			AfxMessageBox(sMsg, MB_ICONWARNING | MB_OK);
			return FALSE;
		}
	}

	for (k = 0; k < nMaxStrip; k++)
	{
		strItem.Format(_T("Strip%d"), k);
		if (0 < ::GetPrivateProfileString(strMenu, strItem, NULL, szData, sizeof(szData), sPath))
		{
			m_stYield.nDefStrip[k] = _ttoi(szData);
		}
		else
		{
			if (nSerial == 1)
			{
				m_stYield.nDefStrip[k] = 0;
			}
			else
			{
				sMsg.Format(_T("이전 수율 읽기 오류 : Shot(%d)-Strip%d\r\n%s"), nSerial, k, sPath);
				AfxMessageBox(sMsg, MB_ICONWARNING | MB_OK);
				return FALSE;
			}
		}

		strItem.Format(_T("StripOut_%d"), k);
		if (0 < ::GetPrivateProfileString(strMenu, strItem, NULL, szData, sizeof(szData), sPath))
		{
			m_stYield.nStripOut[k] = _ttoi(szData);
		}
		else
		{
			if (nSerial == 1)
			{
				m_stYield.nStripOut[k] = 0;
			}
			else
			{
				sMsg.Format(_T("이전 수율 읽기 오류 : Shot(%d)-StripOut_%d\r\n%s"), nSerial, k, sPath);
				AfxMessageBox(sMsg, MB_ICONWARNING | MB_OK);
				return FALSE;
			}
		}

		for (i = 1; i < MAX_DEF; i++)
		{
			strItem.Format(_T("Strip%d_%d"), k, i);
			if (0 < ::GetPrivateProfileString(strMenu, strItem, NULL, szData, sizeof(szData), sPath))
			{
				m_stYield.nDefPerStrip[k][i] = _ttoi(szData);
			}
			else
			{
				if (nSerial == 1)
				{
					m_stYield.nDefPerStrip[k][i] = 0;
				}
				else
				{
					sMsg.Format(_T("이전 수율 읽기 오류 : Shot(%d)-Strip%d_%d\r\n%s"), nSerial, k, i, sPath);
					AfxMessageBox(sMsg, MB_ICONWARNING | MB_OK);
					return FALSE;
				}
			}
		}
	}

	if (0 < ::GetPrivateProfileString(strMenu, _T("StripOut_Total"), NULL, szData, sizeof(szData), sPath))
	{
		m_stYield.nTotSriptOut = _ttoi(szData);
	}
	else
	{
		if (nSerial == 1)
		{
			m_stYield.nTotSriptOut = 0;
		}
		else
		{
			sMsg.Format(_T("이전 수율 읽기 오류 : Shot(%d)-StripOut_Total\r\n%s"), nSerial, sPath);
			AfxMessageBox(sMsg, MB_ICONWARNING | MB_OK);
			return FALSE;
		}
	}
	
	int dwEnd = GetTickCount();
	int dwElapsed = dwEnd - dwStart;

	return TRUE;
}

BOOL CReelMap::WriteYield(int nSerial, CString sPath)
{
	int dwStart = GetTickCount();

	int nMaxStrip;
#ifdef USE_CAM_MASTER
	nMaxStrip = ((CManagerReelmap*)m_pParent)->m_Master[0].GetStripNum(); // 총 스트립의 갯수
#else
#ifdef TEST_MODE
	nMaxStrip = 4;
#else
	nMaxStrip = MAX_STRIP;
#endif
#endif

	int nNodeX = 0, nNodeY = 0;
#ifndef TEST_MODE
	nNodeX = ((CManagerReelmap*)m_pParent)->m_Master[0].m_pPcsRgn->nCol;
	nNodeY = ((CManagerReelmap*)m_pParent)->m_Master[0].m_pPcsRgn->nRow;
#endif
	CString sDefNum, strData;
	int nPnl, nRow, nCol, nDefCode, nStrip;
	int nTotPcs = nNodeX * nNodeY;
	int nStripPcs = nTotPcs / nMaxStrip;
	double dStOutRto = _tstof(pDoc->WorkingInfo.LastJob.sStripOutRatio) / 100.0;
	nPnl = nSerial - 1;

	int i, k;
	TCHAR szData[200];

	CString sCode, strMenu, strItem, sMsg;
	int nTot = 0, nGood = 0, nDef = 0;
	int nTotSriptOut = 0;

	int nDefStrip[MAX_STRIP], nDefA[MAX_DEF], nDefPerStrip[MAX_STRIP][MAX_DEF], nStripOut[MAX_STRIP];
	nDefStrip[0] = 0; nDefStrip[1] = 0; nDefStrip[2] = 0; nDefStrip[3] = 0;
	nStripOut[0] = 0; nStripOut[1] = 0; nStripOut[2] = 0; nStripOut[3] = 0;

	nTot = nNodeX * nNodeY;

	for (k = 0; k < MAX_DEF; k++)
	{
		nDefA[k] = 0;

		for (i = 0; i < nMaxStrip; i++)
		{
			nDefPerStrip[i][k] = 0;
		}
	}

	for (nRow = 0; nRow < nNodeY; nRow++)
	{
		for (nCol = 0; nCol < nNodeX; nCol++)
		{
			if (m_pPnlBuf)
			{
				nDefCode = (int)m_pPnlBuf[nPnl][nRow][nCol] < 0 ? 0 : (int)m_pPnlBuf[nPnl][nRow][nCol];
				nDefA[nDefCode]++;

				nStrip = int(nRow / (nNodeY / nMaxStrip));
				if (nStrip > -1 && nStrip < nMaxStrip)
				{
					if (nDefCode > 0)
					{
						nDefStrip[nStrip]++;
						nDefPerStrip[nStrip][nDefCode]++;
					}
				}
			}
		}
	}

	for (nStrip = 0; nStrip < nMaxStrip; nStrip++)
	{
		if (nDefStrip[nStrip] >= nStripPcs * dStOutRto)
			nStripOut[nStrip]++;
	}

	for (i = 1; i < MAX_DEF; i++)
	{
		m_stYield.nDefA[i] = m_stYield.nDefA[i] + nDefA[i];
		nDef += nDefA[i];
	}
	nGood = nTot - nDef;

	m_stYield.nTot = m_stYield.nTot + nTot;
	m_stYield.nGood = m_stYield.nGood + nGood;
	m_stYield.nDef = m_stYield.nDef + nDef;

	for (k = 0; k < nMaxStrip; k++)
	{
		m_stYield.nDefStrip[k] = m_stYield.nDefStrip[k] + nDefStrip[k];
		m_stYield.nStripOut[k] = m_stYield.nStripOut[k] + nStripOut[k];
		nTotSriptOut += nStripOut[k];
		for (i = 1; i < MAX_DEF; i++)
			m_stYield.nDefPerStrip[k][i] = m_stYield.nDefPerStrip[k][i] + nDefPerStrip[k][i];
	}
	m_stYield.nTotSriptOut = m_stYield.nTotSriptOut + nTotSriptOut;


	FILE *fp = NULL;
	char FileName[MAX_PATH];

	BOOL bExist = FALSE;
	CFileFind findfile;
	if (findfile.FindFile(sPath))
		bExist = TRUE;
	else
		MakeDirYield(sPath);

	//_tcscpy(FileName, sPath);
	StrToChar(sPath, FileName);

	fp = fopen(FileName, "a+");
	if (fp == NULL)
	{
		pView->MsgBox(_T("It is trouble to open ReelMap.txt"));
		//AfxMessageBox(_T("It is trouble to open ReelMap.txt"),MB_ICONWARNING|MB_OK);
		return FALSE;
	}

	if (!bExist)
	{
		m_nStartSerial = nSerial;

		fprintf(fp, "[Info]\n");
		fprintf(fp, "Total Shot = \n\n");
		fprintf(fp, "Total Pcs = \n");
		fprintf(fp, "Good Pcs = \n");
		fprintf(fp, "Bad Pcs = \n\n");
		fprintf(fp, "Start Shot=%d\n", m_nStartSerial);
		fprintf(fp, "End Shot = \n\n");

		for (i = 1; i <= MAX_DEF; i++)
			fprintf(fp, "%d=\n", i);
		fprintf(fp, "\n");

		for (k = 0; k < nMaxStrip; k++)
			fprintf(fp, "Strip%d = \n", k);
		fprintf(fp, "\n");

		for (k = 0; k < nMaxStrip; k++)
		{
			for (i = 1; i <= MAX_DEF; i++)
				fprintf(fp, "Strip%d_%d = \n", k, i);
			fprintf(fp, "\n");
		}

		fprintf(fp, "StripOut_Total = \n");
		fprintf(fp, "StripOut_0 = \n");
		fprintf(fp, "StripOut_1 = \n");
		fprintf(fp, "StripOut_2 = \n");
		fprintf(fp, "StripOut_3 = \n");
		if (nMaxStrip == 6)
		{
			fprintf(fp, "StripOut_4 = \n");
			fprintf(fp, "StripOut_5 = \n");
		}
		fprintf(fp, "\n");
	}
	
	fclose(fp);

	strMenu.Format(_T("%d"), nSerial);

	for (i = 1; i < MAX_DEF; i++)
	{
		sCode.Format(_T("%d"), i);
		sDefNum.Format(_T("%d"), m_stYield.nDefA[i]);

		// [Info]
		::WritePrivateProfileString(_T("Info"), sCode, sDefNum, sPath);

		// [Serial]
		::WritePrivateProfileString(strMenu, sCode, sDefNum, sPath);
	}

	// [Info]
	strData.Format(_T("%d"), nSerial);
	::WritePrivateProfileString(_T("Info"), _T("End Shot"), strData, sPath);

	strData.Format(_T("%d"), nSerial - m_nStartSerial + 1);
	::WritePrivateProfileString(_T("Info"), _T("Total Shot"), strData, sPath);

	strData.Format(_T("%d"), m_stYield.nTot);
	::WritePrivateProfileString(_T("Info"), _T("Total Pcs"), strData, sPath);

	strData.Format(_T("%d"), m_stYield.nGood);
	::WritePrivateProfileString(_T("Info"), _T("Good Pcs"), strData, sPath);

	strData.Format(_T("%d"), m_stYield.nDef);
	::WritePrivateProfileString(_T("Info"), _T("Bad Pcs"), strData, sPath);

	// [Serial]
	strData.Format(_T("%d"), m_stYield.nTot);
	::WritePrivateProfileString(strMenu, _T("Total Pcs"), strData, sPath);

	strData.Format(_T("%d"), m_stYield.nGood);
	::WritePrivateProfileString(strMenu, _T("Good Pcs"), strData, sPath);

	strData.Format(_T("%d"), m_stYield.nDef);
	::WritePrivateProfileString(strMenu, _T("Bad Pcs"), strData, sPath);

	for (k = 0; k < nMaxStrip; k++)
	{
		strItem.Format(_T("Strip%d"), k);
		strData.Format(_T("%d"), m_stYield.nDefStrip[k]);
		// [Info]
		::WritePrivateProfileString(_T("Info"), strItem, strData, sPath);
		// [Serial]
		::WritePrivateProfileString(strMenu, strItem, strData, sPath);

		strItem.Format(_T("StripOut_%d"), k);
		strData.Format(_T("%d"), m_stYield.nStripOut[k]);
		// [Info]
		::WritePrivateProfileString(_T("Info"), strItem, strData, sPath);
		// [Serial]
		::WritePrivateProfileString(strMenu, strItem, strData, sPath);

		for (i = 1; i < MAX_DEF; i++)
		{
			strItem.Format(_T("Strip%d_%d"), k, i);
			strData.Format(_T("%d"), m_stYield.nDefPerStrip[k][i]);
			// [Info]
			::WritePrivateProfileString(_T("Info"), strItem, strData, sPath);
			// [Serial]
			::WritePrivateProfileString(strMenu, strItem, strData, sPath);
		}
	}

	strData.Format(_T("%d"), m_stYield.nTotSriptOut);
	// [Info]
	::WritePrivateProfileString(_T("Info"), _T("StripOut_Total"), strData, sPath);
	// [Serial]
	::WritePrivateProfileString(strMenu, _T("StripOut_Total"), strData, sPath);

	int dwEnd = GetTickCount();
	int dwElapsed = dwEnd - dwStart;

	return TRUE;
}

BOOL CReelMap::MakeHeader(CString sPath)
{
	FILE *fp = NULL;
	char FileName[MAX_PATH];
	BOOL bExist = FALSE;

	CFileFind findfile;
	if (findfile.FindFile(sPath))
	{
		bExist = TRUE;
		return TRUE;
	}

	//m_sPathShare = sPath;
	int nMaxStrip;
#ifdef USE_CAM_MASTER
	nMaxStrip = ((CManagerReelmap*)m_pParent)->m_Master[0].GetStripNum(); // 총 스트립의 갯수
#else
#ifdef TEST_MODE
	nMaxStrip = 4;
#else
	nMaxStrip = MAX_STRIP;
#endif
#endif

	MakeDirRmap();
	StrToChar(sPath, FileName);

	fp = fopen(FileName, "w+");
	if (fp == NULL)
	{
		pView->MsgBox(_T("It is trouble to open ReelMap.txt"));
		return FALSE;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	char* pRtn = NULL;
	int nNodeX = 0, nNodeY = 0, i = 0, k = 0;
#ifdef USE_CAM_MASTER
	nNodeX = ((CManagerReelmap*)m_pParent)->m_Master[0].m_pPcsRgn->nCol;
	nNodeY = ((CManagerReelmap*)m_pParent)->m_Master[0].m_pPcsRgn->nRow;
#endif

	//InitRst();
	//InitPcs();

	fprintf(fp, "[Info]\n");
	fprintf(fp, "설  비  명 = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.System.sMcName)); if (pRtn) delete pRtn; pRtn = NULL;
	fprintf(fp, "운  용  자 = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sSelUserName)); if (pRtn) delete pRtn; pRtn = NULL;
	fprintf(fp, "모      델 = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sModelUp)); if (pRtn) delete pRtn; pRtn = NULL;
	fprintf(fp, "로      트 = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sLotUp)); if (pRtn) delete pRtn; pRtn = NULL;
	fprintf(fp, "상면레이어 = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sLayerUp)); if (pRtn) delete pRtn; pRtn = NULL;
	if (bDualTest)
	{
		fprintf(fp, "하면레이어 = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sLayerDn)); if (pRtn) delete pRtn; pRtn = NULL;
	}
	fprintf(fp, "\n");
	fprintf(fp, "Process Code = \n");
	fprintf(fp, "Entire Speed = \n");
	fprintf(fp, "\n");

	fprintf(fp, "CamPcsX = %d\n", nNodeX);
	fprintf(fp, "CamPcsY = %d\n", nNodeY);

	fprintf(fp, "\n");
	fprintf(fp, "0 -> 양품\n");
	for (i = 1; i < MAX_DEF; i++)
	{
		fprintf(fp, "%d -> %s\n", i, pRtn = StrToChar(m_sKorDef[i])); if (pRtn) delete pRtn; pRtn = NULL; // m_cBigDef[i]
	}
	fprintf(fp, "? - Missed Align Panel(i.e 노광불량)\n");
	fprintf(fp, "\n");
	fprintf(fp, "Lot Start=\n");
	fprintf(fp, "Lot Run=\n");
	fprintf(fp, "Lot End=\n");
	fprintf(fp, "\n");
	fprintf(fp, "Marked Shot=\n");
	fprintf(fp, "Marked Date=\n");
	fprintf(fp, "\n");
	fprintf(fp, "\n");
	fprintf(fp, "// < 수율 정보 > \n");
	fprintf(fp, "\n");
	fprintf(fp, "Start Serial=\n");
	fprintf(fp, "End Serial=\n");
	fprintf(fp, "\n");
	fprintf(fp, "Total Pcs=\n");
	fprintf(fp, "Good Pcs=\n");
	fprintf(fp, "Bad Pcs=\n");
	fprintf(fp, "\n");
	fprintf(fp, "Strip0=\n");
	fprintf(fp, "Strip1=\n");
	fprintf(fp, "Strip2=\n");
	fprintf(fp, "Strip3=\n");
	if (nMaxStrip == 6)
	{
		fprintf(fp, "Strip4=\n");
		fprintf(fp, "Strip5=\n");
	}
	for (i = 1; i < MAX_DEF; i++)
		fprintf(fp, "%d=\n", i); // m_cBigDef[i]
	fprintf(fp, "\n");

	for (k = 0; k < nMaxStrip; k++)
	{
		fprintf(fp, "[Strip%d]\n", k);
		for (i = 1; i < MAX_DEF; i++)
			fprintf(fp, "%d=\n", i); // m_cBigDef[i]
		fprintf(fp, "\n");
	}

	fprintf(fp, "[StripOut]\n");
	fprintf(fp, "Total=\n");
	for (k = 0; k < nMaxStrip; k++)
		fprintf(fp, "%d=\n", k);
	fprintf(fp, "\n");

	fclose(fp);

	return TRUE;
}


// ITS

CString CReelMap::GetPathReelmapIts()
{
	CString sName = _T("ReelMapDataIts.txt");
	CString str, sPath, Path[3];

	sPath = _T("");
	Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
	Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
	Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode; // pDoc->m_sItsCode;

	if (Path[0].IsEmpty() || Path[1].IsEmpty() || Path[2].IsEmpty())
		return _T("");

	sPath.Format(_T("%s%s\\%s\\%s"), Path[0], Path[1], Path[2], sName); // ReelmapIts.txt
	return sPath;
}

BOOL CReelMap::MakeItsReelmapHeader()
{
	FILE *fp = NULL;
	char FileName[MAX_PATH];
	BOOL bExist = FALSE;
	CString sName = _T("ReelMapDataIts.txt");
	CString str, sPath, Path[3];

	int nMaxStrip;
#ifdef USE_CAM_MASTER
	nMaxStrip = ((CManagerReelmap*)m_pParent)->m_Master[0].GetStripNum(); // 총 스트립의 갯수
#else
#ifdef TEST_MODE
	nMaxStrip = 4;
#else
	nMaxStrip = MAX_STRIP;
#endif
#endif

	sPath = _T("");
	Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
	Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
	Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode; // pDoc->m_sItsCode;

	if (Path[0].IsEmpty() || Path[1].IsEmpty() || Path[2].IsEmpty())
		return FALSE;

	sPath.Format(_T("%s%s\\%s\\%s"), Path[0], Path[1], Path[2], sName); // ReelmapIts.txt

	CFileFind findfile;
	if (findfile.FindFile(sPath))
	{
		bExist = TRUE;
		return TRUE;
	}

	sPath.Format(_T("%s%s"), Path[0], Path[1]);					// 모델 폴더
	if (!pDoc->DirectoryExists(sPath))
		CreateDirectory(sPath, NULL);

	sPath.Format(_T("%s%s\\%s"), Path[0], Path[1], Path[2]);	// ITS 코드 폴더
	if (!pDoc->DirectoryExists(sPath))
		CreateDirectory(sPath, NULL);

	sPath.Format(_T("%s%s\\%s\\%s"), Path[0], Path[1], Path[2], sName); // ReelmapIts.txt

	StrToChar(sPath, FileName);

	fp = fopen(FileName, "a+");
	if (fp == NULL)
	{
		str.Format(_T("It is trouble to open ReelmapIts.txt\r\n%s"), sPath);
		pView->MsgBox(str);
		return FALSE;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	char* pRtn = NULL;
	int nNodeX = 0, nNodeY = 0, i = 0, k = 0;
#ifdef USE_CAM_MASTER
	nNodeX = ((CManagerReelmap*)m_pParent)->m_Master[0].m_pPcsRgn->nCol;
	nNodeY = ((CManagerReelmap*)m_pParent)->m_Master[0].m_pPcsRgn->nRow;
#endif

	fprintf(fp, "[Info]\n");
	fprintf(fp, "설  비  명 = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.System.sMcName)); if (pRtn) delete pRtn; pRtn = NULL;
	fprintf(fp, "운  용  자 = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sSelUserName)); if (pRtn) delete pRtn; pRtn = NULL;
	fprintf(fp, "모      델 = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sModelUp)); if (pRtn) delete pRtn; pRtn = NULL;
	fprintf(fp, " ITS Code  = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sEngItsCode)); if (pRtn) delete pRtn; pRtn = NULL;
	//fprintf(fp, " ITS Code  = %s\n", pRtn = StrToChar(pDoc->m_sItsCode)); if (pRtn) delete pRtn; pRtn = NULL;
	fprintf(fp, "\n");
	fprintf(fp, "Process Code = \n");
	fprintf(fp, "Entire Speed = \n");
	fprintf(fp, "\n");

	fprintf(fp, "CamPcsX = %d\n", nNodeX);
	fprintf(fp, "CamPcsY = %d\n", nNodeY);

	fprintf(fp, "\n");
	fprintf(fp, "0 -> 양품\n");
	for (i = 1; i < MAX_DEF; i++)
	{
		fprintf(fp, "%d -> %s\n", i, pRtn = StrToChar(m_sKorDef[i])); if (pRtn) delete pRtn; pRtn = NULL; // m_cBigDef[i]
	}
	fprintf(fp, "? - Missed Align Panel(i.e 노광불량)\n");
	fprintf(fp, "\n");
	fprintf(fp, "Lot Start=\n");
	fprintf(fp, "Lot Run=\n");
	fprintf(fp, "Lot End=\n");
	fprintf(fp, "\n");
	fprintf(fp, "Marked Shot=\n");
	fprintf(fp, "Marked Date=\n");
	fprintf(fp, "\n");
	fprintf(fp, "\n");
	fprintf(fp, "// < 수율 정보 > \n");
	fprintf(fp, "\n");
	fprintf(fp, "Start Serial=\n");
	fprintf(fp, "End Serial=\n");
	fprintf(fp, "\n");
	fprintf(fp, "Total Pcs=\n");
	fprintf(fp, "Good Pcs=\n");
	fprintf(fp, "Bad Pcs=\n");
	fprintf(fp, "\n");
	fprintf(fp, "Strip0=\n");
	fprintf(fp, "Strip1=\n");
	fprintf(fp, "Strip2=\n");
	fprintf(fp, "Strip3=\n");
	if (nMaxStrip == 6)
	{
		fprintf(fp, "Strip4=\n");
		fprintf(fp, "Strip5=\n");
	}
	for (i = 1; i < MAX_DEF; i++)
		fprintf(fp, "%d=\n", i); // m_cBigDef[i]
	fprintf(fp, "\n");

	for (k = 0; k < nMaxStrip; k++)
	{
		fprintf(fp, "[Strip%d]\n", k);
		for (i = 1; i < MAX_DEF; i++)
			fprintf(fp, "%d=\n", i); // m_cBigDef[i]
		fprintf(fp, "\n");
	}

	fprintf(fp, "[StripOut]\n");
	fprintf(fp, "Total=\n");
	for (k = 0; k < nMaxStrip; k++)
		fprintf(fp, "%d=\n", k);
	fprintf(fp, "\n");

	fclose(fp);

	return TRUE;
}

BOOL CReelMap::WriteIts(int nItsSerial)
{
	if (nItsSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.163"));
		return FALSE;
	}

	if (pDoc->GetTestMode() != MODE_OUTER)
		return FALSE;

	int nMaxStrip;
#ifdef USE_CAM_MASTER
	nMaxStrip = ((CManagerReelmap*)m_pParent)->m_Master[0].GetStripNum(); // 총 스트립의 갯수
#else
#ifdef TEST_MODE
	nMaxStrip = 4;
#else
	nMaxStrip = MAX_STRIP;
#endif
#endif

	CString str, sPath;
	sPath = GetPathReelmapIts(); // pDoc->GetItsReelmapPath();
	if (sPath.IsEmpty())
	{
		str.Format(_T("ReelmapITS Path is emptied."));
		pView->MsgBox(str);
		return FALSE;
	}

	if (!MakeItsReelmapHeader())	// 내외층 머징된 릴맵 헤드
	{
		str.Format(_T("It is trouble to open ReelmapIts.txt\r\n%s"), sPath);
		pView->MsgBox(str);
		return FALSE;
	}

	int nIdx = GetPcrIdx(nItsSerial);
	int nNodeX = ((CManagerReelmap*)m_pParent)->m_MasterInner[0].m_pPcsRgn->nCol;
	int nNodeY = ((CManagerReelmap*)m_pParent)->m_MasterInner[0].m_pPcsRgn->nRow;
	int nStripY = ((CManagerReelmap*)m_pParent)->m_MasterInner[0].m_pPcsRgn->nRow / nMaxStrip; // Strip(1 ~ 4 or 6);
	int nTotDefPcs = 0;
	if (((CManagerReelmap*)m_pParent)->m_pPcrIts)
	{
		if (((CManagerReelmap*)m_pParent)->m_pPcrIts[nIdx])
			nTotDefPcs = ((CManagerReelmap*)m_pParent)->m_pPcrIts[nIdx]->m_nTotDef;
	}

	short **pPnlBuf;
	int i, nC, nR, nPcsId, nDefCode;//, nTot, nDef, nGood;
	pPnlBuf = new short*[nNodeY];
	for (i = 0; i < nNodeY; i++)
	{
		pPnlBuf[i] = new short[nNodeX];
		memset(pPnlBuf[i], 0, sizeof(short)*nNodeX);
		memset(m_pPnlBuf[nItsSerial - 1][i], 0, sizeof(short)*nNodeX);
	}

	CString strData, strTemp;

	for (i = 0; i < nTotDefPcs; i++)
	{
		if (((CManagerReelmap*)m_pParent)->m_pPcrIts[nIdx]->m_pMk[i] != -2) // -2 (NoMarking)
		{
			if (pDoc->WorkingInfo.System.bStripPcsRgnBin)	// DTS용
			{
				switch (((CManagerReelmap*)m_pParent)->m_MasterInner[0].MasterInfo.nActionCode)	// 0 : Rotation / Mirror 적용 없음(CAM Data 원본), 1 : 좌우 미러, 2 : 상하 미러, 3 : 180 회전, 4 : 270 회전(CCW), 5 : 90 회전(CW)
				{
				case 0:
					nPcsId = ((CManagerReelmap*)m_pParent)->m_pPcrIts[nIdx]->m_pDefPcs[i];
					break;
				case 1:
					nPcsId = ((CManagerReelmap*)m_pParent)->MirrorLR(((CManagerReelmap*)m_pParent)->m_pPcrIts[nIdx]->m_pDefPcs[i]);
					break;
				case 2:
					nPcsId = ((CManagerReelmap*)m_pParent)->MirrorUD(((CManagerReelmap*)m_pParent)->m_pPcrIts[nIdx]->m_pDefPcs[i]);
					break;
				case 3:
					nPcsId = ((CManagerReelmap*)m_pParent)->Rotate180(((CManagerReelmap*)m_pParent)->m_pPcrIts[nIdx]->m_pDefPcs[i]);
					break;
				default:
					nPcsId = ((CManagerReelmap*)m_pParent)->m_pPcrIts[nIdx]->m_pDefPcs[i];
					break;
				}
			}
			else
				nPcsId = ((CManagerReelmap*)m_pParent)->m_pPcrIts[nIdx]->m_pDefPcs[i];

			nDefCode = ((CManagerReelmap*)m_pParent)->m_pPcrIts[nIdx]->m_pDefType[i];

			nC = int(nPcsId / nNodeY);
			if (nC % 2)	// 홀수.
				nR = nNodeY * (nC + 1) - nPcsId - 1;
			else		// 짝수.
				nR = nPcsId - nNodeY * nC;
			pPnlBuf[nR][nC] = (short)nDefCode;	// nPnl의 열 정보.
			if (m_pPnlBuf)
				m_pPnlBuf[nItsSerial - 1][nR][nC] = pPnlBuf[nR][nC];   // DefCode 3D Array : [nSerial][nRow][nCol] - 릴맵파일 정보용. #define PNL_TOT
		}
	}

	CString sPnl, sRow;
	i = 0;
	sPnl.Format(_T("%d"), nItsSerial);
	strData.Format(_T("%d"), nTotDefPcs);
	::WritePrivateProfileString(sPnl, _T("Total Defects"), strData, sPath);

	for (int nRow = 0; nRow < nNodeX; nRow++)
	{
		sRow.Format(_T("%02d"), nRow);
		//sRow.Format(_T("%2d"), nRow+1);
		strData.Format(_T(""));
		strTemp.Format(_T(""));

		//if(nRow==26)
		//	int brk=0;

		for (int nCol = 0; nCol < nNodeY; nCol++)
		{
			nR = (nNodeY - 1) - nCol;
			nC = nRow;

			if (((CManagerReelmap*)m_pParent)->m_pPcrIts[nIdx]->m_nErrPnl == -1 || ((CManagerReelmap*)m_pParent)->m_pPcrIts[nIdx]->m_nErrPnl == -2)
			{
				nDefCode = DEF_LIGHT;
				m_pPnlBuf[nItsSerial - 1][nR][nC] = (short)nDefCode;
			}
			else
				nDefCode = (int)pPnlBuf[nR][nC] < 0 ? 0 : (int)pPnlBuf[nR][nC];

			strTemp.Format(_T("%2d,"), nDefCode);

			if (!nCol)
				strData.Insert(0, strTemp);
			else
			{
				int nLen = strData.GetLength();
				if (!(nCol%nStripY)) // Separate Strip
				{
					strData.Insert(nLen, _T("  "));
					nLen = strData.GetLength();
				}
				strData.Insert(nLen, strTemp);
			}
		}

		int nPos = strData.ReverseFind(',');
		strData.Delete(nPos, 1);
		::WritePrivateProfileString(sPnl, sRow, strData, sPath);
	}

	for (i = 0; i < nNodeY; i++)
		delete[]  pPnlBuf[i];
	delete[] pPnlBuf;

	((CManagerReelmap*)m_pParent)->m_nWritedItsSerial = nItsSerial;
	return TRUE;
}

BOOL CReelMap::MakeItsFile(int nSerial, int nLayer) // RMAP_UP, RMAP_DN, RMAP_INNER_UP, RMAP_INNER_DN
{
	MakeDirIts();

	CFileFind cFile;
	CString sPath = GetItsPath(nSerial, nLayer);

	if (cFile.FindFile(sPath))
		DeleteFile(sPath);

	char FileName[MAX_PATH];
	StringToChar(sPath, FileName);

	char* pRtn = NULL;
	FILE *fp = NULL;
	fp = fopen(FileName, "w+");
	if (fp != NULL)
	{
		fprintf(fp, "%s", pRtn = StringToChar(GetItsFileData(nSerial, nLayer)));
		if (pRtn)
		{
			delete pRtn;
			pRtn = NULL;
		}
	}
	else
	{
		pView->MsgBox(_T("It is trouble to MakeItsFile."), MB_ICONWARNING | MB_OK);
		return FALSE;
	}

	fclose(fp);

	//CString sDestPath = pDoc->GetItsTargetPath(nSerial, nLayer);
	//BOOL bRtn = pDoc->m_pFile->Copy(sPath, sDestPath);			// ITS 파일을 복사한다.

	return TRUE;
}

CString CReelMap::GetItsFileData(int nSerial, int nLayer) // RMAP_UP, RMAP_DN, RMAP_INNER_UP, RMAP_INNER_DN
{
	int nMaxStrip;
#ifdef USE_CAM_MASTER
	nMaxStrip = ((CManagerReelmap*)m_pParent)->m_Master[0].GetStripNum(); // 총 스트립의 갯수
#else
#ifdef TEST_MODE
	nMaxStrip = 4;
#else
	nMaxStrip = MAX_STRIP;
#endif
#endif

	CString sPath = GetItsPath(nSerial, nLayer);
	CString str = _T(""), sSide = _T(""), sTemp = _T(""), sItsData = _T("");
	CString sItsCode = pDoc->WorkingInfo.LastJob.sEngItsCode;
	//CString sItsCode = pDoc->m_sItsCode;

	int nNodeX = ((CManagerReelmap*)m_pParent)->m_Master[0].m_pPcsRgn->nCol;
	int nNodeY = ((CManagerReelmap*)m_pParent)->m_Master[0].m_pPcsRgn->nRow;
	int nStripY = ((CManagerReelmap*)m_pParent)->m_Master[0].m_pPcsRgn->nRow / nMaxStrip; // Strip(1~4 or 6);
	int nIdx = GetPcrIdx(nSerial);

	int nTotDefPcs = 0;

	switch (nLayer)
	{
	case RMAP_UP:
		sSide = _T("T");
		if (((CManagerReelmap*)m_pParent)->m_pPcr[nLayer])
		{
			if (((CManagerReelmap*)m_pParent)->m_pPcr[nLayer][nIdx])
				nTotDefPcs = ((CManagerReelmap*)m_pParent)->m_pPcr[nLayer][nIdx]->m_nTotDef;
		}
		break;
	case RMAP_DN:
		sSide = _T("B");
		if (((CManagerReelmap*)m_pParent)->m_pPcr[nLayer])
		{
			if (((CManagerReelmap*)m_pParent)->m_pPcr[nLayer][nIdx])
				nTotDefPcs = ((CManagerReelmap*)m_pParent)->m_pPcr[nLayer][nIdx]->m_nTotDef;
		}
		break;
	case RMAP_INNER_UP:
		sSide = _T("T");
		if (((CManagerReelmap*)m_pParent)->m_pPcrInner[0])
		{
			if (((CManagerReelmap*)m_pParent)->m_pPcrInner[0][nIdx])
				nTotDefPcs = ((CManagerReelmap*)m_pParent)->m_pPcrInner[0][nIdx]->m_nTotDef;
		}
		//if (((CManagerReelmap*)m_pParent)->m_pPcr[nLayer])
		//{
		//	if (((CManagerReelmap*)m_pParent)->m_pPcr[nLayer][nIdx])
		//		nTotDefPcs = ((CManagerReelmap*)m_pParent)->m_pPcr[nLayer][nIdx]->m_nTotDef;
		//}
		break;
	case RMAP_INNER_DN:
		sSide = _T("B");
		if (((CManagerReelmap*)m_pParent)->m_pPcrInner[1])
		{
			if (((CManagerReelmap*)m_pParent)->m_pPcrInner[1][nIdx])
				nTotDefPcs = ((CManagerReelmap*)m_pParent)->m_pPcrInner[1][nIdx]->m_nTotDef;
		}
		//if (((CManagerReelmap*)m_pParent)->m_pPcr[nLayer])
		//{
		//	if (((CManagerReelmap*)m_pParent)->m_pPcr[nLayer][nIdx])
		//		nTotDefPcs = ((CManagerReelmap*)m_pParent)->m_pPcr[nLayer][nIdx]->m_nTotDef;
		//}
		break;
	}

	str.Format(_T("%d,%s,%04d\n"), nTotDefPcs, sItsCode, nSerial);
	sItsData = str;

	int nR, nRow, nCol, nDefCode;
	int nTotStrip = 0;

	// Strip A
	for (nRow = 0; nRow < nStripY; nRow++)
	{
		nR = nRow;
		for (nCol = 0; nCol < nNodeX; nCol++)
		{
			nDefCode = m_pPnlBuf[nSerial - 1][nR][nCol]; // DefCode 3D Array : [nSerial][nRow][nCol] - 릴맵파일 정보용.
			if (nDefCode > 0)
			{
				nTotStrip++;
				str.Format(_T("%s,%04d,%s,A,%d,%d,B%d\n"), sItsCode, nSerial, sSide, nCol + 1, nRow + 1, ((CManagerReelmap*)m_pParent)->GetItsDefCode(nDefCode));
				sTemp += str;
			}
		}
	}
	str.Format(_T("%d,%s,%04d,%s,A\n"), nTotStrip, sItsCode, nSerial, sSide);
	sItsData += str;
	sItsData += sTemp;
	str.Format(_T("%s,%04d,%s,A,EOS\n"), sItsCode, nSerial, sSide);
	sItsData += str;
	sTemp = _T("");
	nTotStrip = 0;

	// Strip B
	for (nRow = 0; nRow < nStripY; nRow++)
	{
		nR = nRow + nStripY;
		for (nCol = 0; nCol < nNodeX; nCol++)
		{
			nDefCode = m_pPnlBuf[nSerial - 1][nR][nCol]; // DefCode 3D Array : [nSerial][nRow][nCol] - 릴맵파일 정보용.
			if (nDefCode > 0)
			{
				nTotStrip++;
				str.Format(_T("%s,%04d,%s,B,%d,%d,B%d\n"), sItsCode, nSerial, sSide, nCol + 1, nRow + 1, ((CManagerReelmap*)m_pParent)->GetItsDefCode(nDefCode));
				sTemp += str;
			}
		}
	}
	str.Format(_T("%d,%s,%04d,%s,B\n"), nTotStrip, sItsCode, nSerial, sSide);
	sItsData += str;
	sItsData += sTemp;
	str.Format(_T("%s,%04d,%s,B,EOS\n"), sItsCode, nSerial, sSide);
	sItsData += str;
	sTemp = _T("");
	nTotStrip = 0;

	// Strip C
	for (nRow = 0; nRow < nStripY; nRow++)
	{
		nR = nRow + nStripY * 2;
		for (nCol = 0; nCol < nNodeX; nCol++)
		{
			nDefCode = m_pPnlBuf[nSerial - 1][nR][nCol]; // DefCode 3D Array : [nSerial][nRow][nCol] - 릴맵파일 정보용.
			if (nDefCode > 0)
			{
				nTotStrip++;
				str.Format(_T("%s,%04d,%s,B,%d,%d,C%d\n"), sItsCode, nSerial, sSide, nCol + 1, nRow + 1, ((CManagerReelmap*)m_pParent)->GetItsDefCode(nDefCode));
				sTemp += str;
			}
		}
	}
	str.Format(_T("%d,%s,%04d,%s,C\n"), nTotStrip, sItsCode, nSerial, sSide);
	sItsData += str;
	sItsData += sTemp;
	str.Format(_T("%s,%04d,%s,C,EOS\n"), sItsCode, nSerial, sSide);
	sItsData += str;
	sTemp = _T("");
	nTotStrip = 0;

	// Strip D
	for (nRow = 0; nRow < nStripY; nRow++)
	{
		nR = nRow + nStripY * 3;
		for (nCol = 0; nCol < nNodeX; nCol++)
		{
			nDefCode = m_pPnlBuf[nSerial - 1][nR][nCol]; // DefCode 3D Array : [nSerial][nRow][nCol] - 릴맵파일 정보용.
			if (nDefCode > 0)
			{
				nTotStrip++;
				str.Format(_T("%s,%04d,%s,B,%d,%d,D%d\n"), sItsCode, nSerial, sSide, nCol + 1, nRow + 1, ((CManagerReelmap*)m_pParent)->GetItsDefCode(nDefCode));
				sTemp += str;
			}
		}
	}
	str.Format(_T("%d,%s,%04d,%s,D\n"), nTotStrip, sItsCode, nSerial, sSide);
	sItsData += str;
	sItsData += sTemp;
	str.Format(_T("%s,%04d,%s,D,EOS\n"), sItsCode, nSerial, sSide);
	sItsData += str;
	str.Format(_T("%s,%04d,%s,EOP\n"), sItsCode, nSerial, sSide);
	sItsData += str;

	if (nMaxStrip == 6)
	{
		// Strip E
		for (nRow = 0; nRow < nStripY; nRow++)
		{
			nR = nRow + nStripY * 4;
			for (nCol = 0; nCol < nNodeX; nCol++)
			{
				nDefCode = m_pPnlBuf[nSerial - 1][nR][nCol]; // DefCode 3D Array : [nSerial][nRow][nCol] - 릴맵파일 정보용.
				if (nDefCode > 0)
				{
					nTotStrip++;
					str.Format(_T("%s,%04d,%s,B,%d,%d,E%d\n"), sItsCode, nSerial, sSide, nCol + 1, nRow + 1, ((CManagerReelmap*)m_pParent)->GetItsDefCode(nDefCode));
					sTemp += str;
				}
			}
		}
		str.Format(_T("%d,%s,%04d,%s,E\n"), nTotStrip, sItsCode, nSerial, sSide);
		sItsData += str;
		sItsData += sTemp;
		str.Format(_T("%s,%04d,%s,E,EOS\n"), sItsCode, nSerial, sSide);
		sItsData += str;
		str.Format(_T("%s,%04d,%s,EOP\n"), sItsCode, nSerial, sSide);
		sItsData += str;

		// Strip F
		for (nRow = 0; nRow < nStripY; nRow++)
		{
			nR = nRow + nStripY * 5;
			for (nCol = 0; nCol < nNodeX; nCol++)
			{
				nDefCode = m_pPnlBuf[nSerial - 1][nR][nCol]; // DefCode 3D Array : [nSerial][nRow][nCol] - 릴맵파일 정보용.
				if (nDefCode > 0)
				{
					nTotStrip++;
					str.Format(_T("%s,%04d,%s,B,%d,%d,F%d\n"), sItsCode, nSerial, sSide, nCol + 1, nRow + 1, ((CManagerReelmap*)m_pParent)->GetItsDefCode(nDefCode));
					sTemp += str;
				}
			}
		}
		str.Format(_T("%d,%s,%04d,%s,F\n"), nTotStrip, sItsCode, nSerial, sSide);
		sItsData += str;
		sItsData += sTemp;
		str.Format(_T("%s,%04d,%s,F,EOS\n"), sItsCode, nSerial, sSide);
		sItsData += str;
		str.Format(_T("%s,%04d,%s,EOP\n"), sItsCode, nSerial, sSide);
		sItsData += str;
	}

	return sItsData;
}

BOOL CReelMap::MakeDirIts()
{
	CFileFind finder;
	CString  Path[3], sItsPath;
	CString sPath = _T("");

	sItsPath = pDoc->WorkingInfo.System.sPathIts;
	Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
	Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
	Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
	//Path[2] = pDoc->m_sItsCode;

	if (Path[0].IsEmpty() || Path[1].IsEmpty() || Path[2].IsEmpty())
		return FALSE;

	sPath.Format(_T("%s%s"), Path[0], Path[1]);
	if (!pDoc->DirectoryExists(sPath))
		CreateDirectory(sPath, NULL);

	sPath.Format(_T("%s%s\\%s"), Path[0], Path[1], Path[2]);
	if (!pDoc->DirectoryExists(sPath))
		CreateDirectory(sPath, NULL);

	sPath.Format(_T("%s%s\\%s\\Inner"), Path[0], Path[1], Path[2]);
	if (!pDoc->DirectoryExists(sPath))
		CreateDirectory(sPath, NULL);

	sPath.Format(_T("%s%s\\%s\\Outer"), Path[0], Path[1], Path[2]);
	if (!pDoc->DirectoryExists(sPath))
		CreateDirectory(sPath, NULL);


	if (sItsPath.IsEmpty())
		return FALSE;

	int pos = sItsPath.ReverseFind('\\');
	if (pos != -1)
		sItsPath.Delete(pos, sItsPath.GetLength() - pos);

	if (!pDoc->DirectoryExists(sItsPath))
		CreateDirectory(sItsPath, NULL);

	return TRUE;
}

void CReelMap::ResetReelmapPath()
{
	m_sPathBuf = GetRmapPath(m_nLayer);
	m_sPathYield = GetYieldPath(m_nLayer);
}


BOOL CReelMap::RemakeReelmap()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	CString sPath = GetRmapPath(m_nLayer);

	FILE *fp = NULL;
	char FileName[MAX_PATH];
	CString strFileName, strPathName;
	CFileFind findfile;
	int nStripNumY, nPieceNumPerStrip;

	CString sModel, sLot, sLayer[2];
	int i, nLastShot, nPnl, nRow, nCol, nDefCode, nCompletedShot;
	CString sPnl, sRow, sVal;
	TCHAR sep[] = { _T(",/;\r\n\t") };
	TCHAR szData[MAX_PATH];

	if (0 < ::GetPrivateProfileString(_T("Info"), _T("Completed Shot"), NULL, szData, sizeof(szData), sPath))
		nCompletedShot = _tstoi(szData);
	else
		nCompletedShot = 0; // Failed.

	if (0 < ::GetPrivateProfileString(_T("Info"), _T("Marked Shot"), NULL, szData, sizeof(szData), sPath))
		nLastShot = _tstoi(szData);
	else
	{
		nLastShot = 0; // Failed.
		pView->MsgBox(_T("릴맵에 Marked Shot 정보가 없습니다."));
		return FALSE;
	}

	if (0 < ::GetPrivateProfileString(_T("Info"), _T("모      델"), NULL, szData, sizeof(szData), sPath))
		sModel = CString(szData);
	else
	{
		pView->MsgBox(_T("Model 정보가 없습니다."));
		return FALSE;
	}

	MakeDirRmap();

	CString sFile = _T(""), sRmapPath = sPath;

	int nPos = sRmapPath.ReverseFind('\\');
	if (nPos != -1)
	{
		sFile = sRmapPath.Right(sRmapPath.GetLength() - nPos - 1);
		sRmapPath.Delete(nPos, sPath.GetLength() - nPos);
	}

	CString sPathRmapF = sRmapPath + _T("\\Reelmap");

	if (!pDoc->DirectoryExists(sPathRmapF))
		CreateDirectory(sPathRmapF, NULL);



	sFile.MakeUpper();

	if (sFile == _T("REELMAPDATAALL.TXT"))
		strPathName = sPathRmapF + _T("\\ReelMap-TOTAL_SIDE.txt");
	else if (sFile == _T("REELMAPDATAUP.TXT"))
		strPathName = sPathRmapF + _T("\\ReelMap-TOP_SIDE.txt");
	else if (sFile == _T("REELMAPDATADN.TXT"))
		strPathName = sPathRmapF + _T("\\ReelMap-BOTTOM_SIDE.txt");
	else if (sFile == _T("REELMAPDATAITS.TXT"))
		strPathName = sPathRmapF + _T("\\ReelMap-ITS.txt");
	else
		strPathName = sPathRmapF + _T("\\ReelMap.txt");

	if (findfile.FindFile(strPathName))
		DeleteFile(strPathName);
	else
	{
		if (!pDoc->DirectoryExists(sPathRmapF))
			CreateDirectory(sPath, NULL);
	}

	int nNodeX = 0;// = pDoc->m_pPcsRgn->nCol; // on Cam
	int nNodeY = 0;// = pDoc->m_pPcsRgn->nRow; // on Cam

	if (0 < ::GetPrivateProfileString(_T("Info"), _T("CamPcsX"), NULL, szData, sizeof(szData), sPath))
		nNodeX = _tstoi(szData);
	if (0 < ::GetPrivateProfileString(_T("Info"), _T("CamPcsY"), NULL, szData, sizeof(szData), sPath))
		nNodeY = _tstoi(szData);

	if (!nNodeX || !nNodeY)
		GetNodeXYonRmap(nNodeX, nNodeY, sPath);

	//nStripNumY = MAX_STRIP_NUM;
#ifdef USE_CAM_MASTER
	nStripNumY = ((CManagerReelmap*)m_pParent)->m_Master[0].GetStripNum(); // 총 스트립의 갯수
#else
	nStripNumY = MAX_STRIP;
#endif

	nPieceNumPerStrip = nNodeY / nStripNumY;

	m_dProgressRatio = 0.0;

	StringToChar(strPathName, FileName);

	fp = fopen(FileName, "a+");
	char* pRtn = NULL;
	if (fp != NULL)
	{
		fprintf(fp, "장비호기명 : %s\n", pRtn = StrToChar(pDoc->WorkingInfo.System.sMcName)); if (pRtn) delete pRtn; pRtn = NULL;
		fprintf(fp, "운  용  자 : %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sSelUserName)); if (pRtn) delete pRtn; pRtn = NULL;
		fprintf(fp, "모      델 : %s\n", pRtn = StrToChar(sModel)); if (pRtn) delete pRtn; pRtn = NULL;
		fprintf(fp, "로      트 : %s\n", pRtn = StrToChar(sLot)); if (pRtn) delete pRtn; pRtn = NULL;
		fprintf(fp, "상면레이어 : %s\n", pRtn = StrToChar(sLayer[0])); if (pRtn) delete pRtn; pRtn = NULL;
		//if (bDualTest)
		if(!sLayer[1].IsEmpty())
		{
			fprintf(fp, "하면레이어 : %s\n", pRtn = StrToChar(sLayer[1])); if (pRtn) delete pRtn; pRtn = NULL;
		}
		fprintf(fp, "양폐 스트립 수율[%%] : %.1f\n", _tstof(pDoc->WorkingInfo.LastJob.sStripOutRatio)); if (pRtn) delete pRtn; pRtn = NULL;//atof

		for (i = 1; i < DEF_UPPER; i++)
		{
			fprintf(fp, "%c - %s\n", pDoc->m_cBigDefCode[i], pRtn = StrToChar(m_sKorDef[i])); if (pRtn) delete pRtn; pRtn = NULL;
		}
		fprintf(fp, "? - Missed Align Panel(i.e 노광불량)\r\n\n");

		// Reload Reelmap.....
		for (nPnl = 0; nPnl < nLastShot; nPnl++)
		{
			if (!m_bThreadAliveRemakeReelmap)
				break;

			m_dProgressRatio = double(nPnl + 1) / double(nLastShot) * 100.0;

			fprintf(fp, "%d\n", nPnl + 1);
			for (nRow = 0; nRow < nNodeX; nRow++)
			{
				sPnl.Format(_T("%d"), nPnl + 1);
				sRow.Format(_T("%02d"), nRow);
				if (0 < ::GetPrivateProfileString(sPnl, sRow, NULL, szData, sizeof(szData), sPath))
				{
					for (nCol = 0; nCol<nNodeY; nCol++)
					{
						if (nCol == 0)
							sVal = _tcstok(szData, sep);
						else
							sVal = _tcstok(NULL, sep);

						nDefCode = _tstoi(sVal);

						if (nCol % nPieceNumPerStrip == 0)
							fprintf(fp, " ");

						fprintf(fp, "%c", pDoc->m_cBigDefCode[nDefCode]);
					}
					fprintf(fp, "\n");
				}
			}
			fprintf(fp, "\n");
		}
	}
	else
	{
		pView->MsgBox(_T("It is trouble to remake ReelMap."), MB_ICONWARNING | MB_OK);
		return FALSE;
	}

	fclose(fp);

	return TRUE;
}

BOOL CReelMap::GetResult() // TRUE: Make Result, FALSE: Load Result or Failed.
{
	int nMaxStrip;
#ifdef USE_CAM_MASTER
	nMaxStrip = ((CManagerReelmap*)m_pParent)->m_Master[0].GetStripNum(); // 총 스트립의 갯수
#else
#ifdef TEST_MODE
	nMaxStrip = 4;
#else
	nMaxStrip = MAX_STRIP;
#endif
#endif

	//파일을 읽어옴. ======================================================================

	// 작업 시간을 읽어옴.
	// 	LoadLotWorkingTime();

	// 불량관련 데이타를 읽어옴.
	int i, k;
	CString strAddedDefectFP;
	CString strMsg, strMenu, strItem;
	TCHAR szData[MAX_PATH];
	CFileFind findfile;

	m_stResult.nEntireStripNum = 0;

	strAddedDefectFP = GetRmapPath(m_nLayer);

	if (!findfile.FindFile(strAddedDefectFP)) // Can not find file.
	{
		strMsg.Format(_T("%s file open is failed"), strAddedDefectFP);
		pView->ClrDispMsg();
		AfxMessageBox(strMsg);
	}
	else
	{
		if (0 < ::GetPrivateProfileString(_T("Info"), _T("설  비  명"), NULL, szData, sizeof(szData), strAddedDefectFP))
			m_stResult.sMachin = CString(szData);
		else
			m_stResult.sMachin = _T("");

		if (0 < ::GetPrivateProfileString(_T("Info"), _T("운  용  자"), NULL, szData, sizeof(szData), strAddedDefectFP))
			m_stResult.sOpName = CString(szData);
		else
			m_stResult.sOpName = _T("");

		if (0 < ::GetPrivateProfileString(_T("Info"), _T("모      델"), NULL, szData, sizeof(szData), strAddedDefectFP))
			m_stResult.sModel = CString(szData);
		else
			m_stResult.sModel = _T("");

		if (0 < ::GetPrivateProfileString(_T("Info"), _T("로      트"), NULL, szData, sizeof(szData), strAddedDefectFP))
			m_stResult.sLot = CString(szData);
		else
			m_stResult.sLot = _T("");

		if (0 < ::GetPrivateProfileString(_T("Info"), _T("상면레이어"), NULL, szData, sizeof(szData), strAddedDefectFP))
			m_stResult.sLayerUp = CString(szData);
		else
			m_stResult.sLayerUp = _T("");

		if (0 < ::GetPrivateProfileString(_T("Info"), _T("하면레이어"), NULL, szData, sizeof(szData), strAddedDefectFP))
			m_stResult.sLayerDn = CString(szData);
		else
			m_stResult.sLayerDn = _T("");

		if (0 < ::GetPrivateProfileString(_T("Info"), _T("Lot Start"), NULL, szData, sizeof(szData), strAddedDefectFP))
			m_stResult.strLotStartTime = CString(szData);
		else
			m_stResult.strLotStartTime = _T("");

		if (0 < ::GetPrivateProfileString(_T("Info"), _T("Lot Run"), NULL, szData, sizeof(szData), strAddedDefectFP))
			m_stResult.strLotWorkingTime = CString(szData);
		else
			m_stResult.strLotWorkingTime = _T("");

		if (0 < ::GetPrivateProfileString(_T("Info"), _T("Lot End"), NULL, szData, sizeof(szData), strAddedDefectFP))
			m_stResult.strLotEndTime = CString(szData);
		else
			m_stResult.strLotEndTime = _T("");

		// 공종코드
		if (0 < ::GetPrivateProfileString(_T("Info"), _T("Process Code"), NULL, szData, sizeof(szData), strAddedDefectFP))
			m_stResult.sProcessNum = CString(szData);
		else
			m_stResult.sProcessNum = _T("");

		// 속도
		if (0 < ::GetPrivateProfileString(_T("Info"), _T("Entire Speed"), NULL, szData, sizeof(szData), strAddedDefectFP))
			m_stResult.dEntireSpeed = _tstof(szData);
		else
			m_stResult.dEntireSpeed = 0.0;

		// 수율 데이타를 읽어옴.

		// Read Piece infomation..............
		if (0 < ::GetPrivateProfileString(_T("Info"), _T("Total Pcs"), NULL, szData, sizeof(szData), strAddedDefectFP))
			m_stResult.nEntirePieceNum = _tstoi(szData);
		else
			m_stResult.nEntirePieceNum = 0;

		if (0 < ::GetPrivateProfileString(_T("Info"), _T("Good Pcs"), NULL, szData, sizeof(szData), strAddedDefectFP))
			m_stResult.nGoodPieceNum = _tstoi(szData);
		else
			m_stResult.nGoodPieceNum = 0;

		if (0 < ::GetPrivateProfileString(_T("Info"), _T("Bad Pcs"), NULL, szData, sizeof(szData), strAddedDefectFP))
			m_stResult.nDefectPieceNum = _tstoi(szData);
		else
			m_stResult.nDefectPieceNum = 0;

		m_stResult.nTotStOut = 0;
		for (k = 0; k < nMaxStrip; k++)
		{
			strMenu.Format(_T("Strip%d"), k);
			if (0 < ::GetPrivateProfileString(_T("Info"), strMenu, NULL, szData, sizeof(szData), strAddedDefectFP))
				m_stResult.nDefStrip[k] = _tstoi(szData);
			else
				m_stResult.nDefStrip[k] = 0;

			strMenu.Format(_T("%d"), k);
			if (0 < ::GetPrivateProfileString(_T("StripOut"), strMenu, NULL, szData, sizeof(szData), strAddedDefectFP))
				m_stResult.nStripOut[k] = _tstoi(szData);
			else
				m_stResult.nStripOut[k] = 0;

			m_stResult.nTotStOut += m_stResult.nStripOut[k];

			for (i = 1; i<MAX_DEF; i++)
			{
				strItem.Format(_T("Strip%d"), k);
				strMenu.Format(_T("%d"), i);
				if (0 < ::GetPrivateProfileString(strItem, strMenu, NULL, szData, sizeof(szData), strAddedDefectFP))
					m_stResult.nDefPerStrip[k][i] = _tstoi(szData);
				else
					m_stResult.nDefPerStrip[k][i] = 0;
			}
		}

		for (i = 1; i<MAX_DEF; i++)
		{
			strMenu.Format(_T("%d"), i);
			if (0 < ::GetPrivateProfileString(_T("Info"), strMenu, NULL, szData, sizeof(szData), strAddedDefectFP))
				m_stResult.nEntireAddedDefect[i] = _tstoi(szData);  // 불량이름별 불량수를 파일에서 읽어옴.
			else
			{
				m_stResult.nEntireAddedDefect[i] = 0;
			}
		}

		m_stResult.nEntireStripNum = (m_stResult.nSerialEd - m_stResult.nSerialSt + 1) * nMaxStrip;

	}

	return TRUE;
}

CString CReelMap::GetResultTxt()
{
	int nMaxStrip;
#ifdef USE_CAM_MASTER
	nMaxStrip = ((CManagerReelmap*)m_pParent)->m_Master[0].GetStripNum(); // 총 스트립의 갯수
#else
#ifdef TEST_MODE
	nMaxStrip = 4;
#else
	nMaxStrip = MAX_STRIP;
#endif
#endif

	CString strFileData, strData;
	int nTot = 0;
	int i, k;

	//리포트 작성. =====================================================================
	int nEntireGoodPcs = m_stResult.nEntirePieceNum - m_stResult.nDefectPieceNum;
	int nEntireDefPcs = m_stResult.nDefectPieceNum;
	double dEntireGoodPcsRto = 100.0*((double)nEntireGoodPcs / (double)m_stResult.nEntirePieceNum);
	double dEntireDefPcsRto = 100.0*((double)nEntireDefPcs / (double)m_stResult.nEntirePieceNum);
	int nEntirePcsEachStrip = m_stResult.nEntirePieceNum / nMaxStrip;
	int nEntireGoodPcsEachStrip[MAX_STRIP], nEntireDefPcsEachStrip[MAX_STRIP];
	double dEntireGoodPcsRtoEachStrip[MAX_STRIP], dEntireDefPcsRtoEachStrip[MAX_STRIP];
	int nDefPerStrip[MAX_STRIP][MAX_DEF], nEntirePcsEachDef[MAX_DEF];

	int nEp = m_stResult.nEntirePieceNum;
	int nEpg = nEntireGoodPcs;
	int nEpo = nEntireDefPcs;
	double dEpgr = dEntireGoodPcsRto;
	double dEpor = dEntireDefPcsRto;

	int nEsp = nEntirePcsEachStrip;
	int nEspge[MAX_STRIP] = { 0 }, nEspoe[MAX_STRIP] = { 0 };
	double dEspgre[MAX_STRIP] = { 0.0 }, dEspore[MAX_STRIP] = { 0.0 };
	int nEsdp[MAX_STRIP][MAX_DEF], nEdp[MAX_DEF];

	for (i = 0; i < nMaxStrip; i++)
	{
		nTot += m_stResult.nDefPerStrip[i][DEF_LIGHT];
		nEspge[i] = nEntireGoodPcsEachStrip[i] = nEntirePcsEachStrip - m_stResult.nDefStrip[i];
		nEspoe[i] = nEntireDefPcsEachStrip[i] = m_stResult.nDefStrip[i];
		dEspgre[i] = dEntireGoodPcsRtoEachStrip[i] = 100.0*(double)nEntireGoodPcsEachStrip[i] / (double)nEntirePcsEachStrip;
		dEspore[i] = dEntireDefPcsRtoEachStrip[i] = 100.0*(double)nEntireDefPcsEachStrip[i] / (double)nEntirePcsEachStrip;
		for (k = 0; k < MAX_DEF; k++)
		{
			nEsdp[i][k] = nDefPerStrip[i][k] = m_stResult.nDefPerStrip[i][k];

			if (i == 0)
				nEdp[k] = nEntirePcsEachDef[k] = m_stResult.nEntireAddedDefect[k];
		}
	}


	strFileData = _T("");
	strFileData += _T("1. 일반 정보\r\n");
	strData.Format(_T("    모 델 명 :  %s\r\n"), m_stResult.sModel);
	strFileData += strData;
	strData.Format(_T("    로 트 명 :  %s\r\n"), m_stResult.sLot);
	strFileData += strData;
	strData.Format(_T("    시작시간 :  %s\r\n"), m_stResult.strLotStartTime);
	strFileData += strData;
	strData.Format(_T("    진행시간 :  %s\r\n"), m_stResult.strLotWorkingTime);
	strFileData += strData;
	strData.Format(_T("    종료시간 :  %s\r\n"), m_stResult.strLotEndTime);
	strFileData += strData;
	strFileData += _T("\r\n");

	strFileData += _T("2. 제품 검사 결과\r\n");
	strData.Format(_T("    총 검 사 Unit 수 : %9d\r\n"), nEp);
	strFileData += strData;
	strData.Format(_T("    양    품 Unit 수 : %9d        양  품  율(%%) : %9.2f\r\n"), nEpg, dEpgr);
	strFileData += strData;
	strData.Format(_T("    불    량 Unit 수 : %9d        불  량  율(%%) : %9.2f\r\n"), nEpo, dEpor);
	strFileData += strData;
	strFileData += _T("\r\n");

	strFileData += _T("3. 열별 검사 수율\r\n");
	strData.Format(_T("    1열 검사 Unit 수 : %9d\r\n"), nEntirePcsEachStrip);
	strFileData += strData;
	strData.Format(_T("    1열 양품 Unit 수 : %9d        1열 양품율(%%) : %9.2f\r\n"), nEspge[0], dEspgre[0]);
	strFileData += strData;
	strData.Format(_T("    1열 불량 Unit 수 : %9d        1열 불량율(%%) : %9.2f\r\n"), nEspoe[0], dEspore[0]);
	strFileData += strData;
	strFileData += _T("\r\n");
	strData.Format(_T("    2열 검사 Unit 수 : %9d\r\n"), nEntirePcsEachStrip);
	strFileData += strData;
	strData.Format(_T("    2열 양품 Unit 수 : %9d        2열 양품율(%%) : %9.2f\r\n"), nEspge[1], dEspgre[1]);
	strFileData += strData;
	strData.Format(_T("    2열 불량 Unit 수 : %9d        2열 불량율(%%) : %9.2f\r\n"), nEspoe[1], dEspore[1]);
	strFileData += strData;
	strFileData += _T("\r\n");
	strData.Format(_T("    3열 검사 Unit 수 : %9d\r\n"), nEntirePcsEachStrip);
	strFileData += strData;
	strData.Format(_T("    3열 양품 Unit 수 : %9d        3열 양품율(%%) : %9.2f\r\n"), nEspge[2], dEspgre[2]);
	strFileData += strData;
	strData.Format(_T("    3열 불량 Unit 수 : %9d        3열 불량율(%%) : %9.2f\r\n"), nEspoe[2], dEspore[2]);
	strFileData += strData;
	strFileData += _T("\r\n");
	strData.Format(_T("    4열 검사 Unit 수 : %9d\r\n"), nEntirePcsEachStrip);
	strFileData += strData;
	strData.Format(_T("    4열 양품 Unit 수 : %9d        4열 양품율(%%) : %9.2f\r\n"), nEspge[3], dEspgre[3]);
	strFileData += strData;
	strData.Format(_T("    4열 불량 Unit 수 : %9d        4열 불량율(%%) : %9.2f\r\n"), nEspoe[3], dEspore[3]);
	strFileData += strData;
	strFileData += _T("\r\n");
	if (nMaxStrip == 6)
	{
		strData.Format(_T("    5열 검사 Unit 수 : %9d\r\n"), nEntirePcsEachStrip);
		strFileData += strData;
		strData.Format(_T("    5열 양품 Unit 수 : %9d        5열 양품율(%%) : %9.2f\r\n"), nEspge[4], dEspgre[4]);
		strFileData += strData;
		strData.Format(_T("    5열 불량 Unit 수 : %9d        5열 불량율(%%) : %9.2f\r\n"), nEspoe[4], dEspore[4]);
		strFileData += strData;
		strFileData += _T("\r\n");
		strData.Format(_T("    6열 검사 Unit 수 : %9d\r\n"), nEntirePcsEachStrip);
		strFileData += strData;
		strData.Format(_T("    6열 양품 Unit 수 : %9d        6열 양품율(%%) : %9.2f\r\n"), nEspge[5], dEspgre[5]);
		strFileData += strData;
		strData.Format(_T("    6열 불량 Unit 수 : %9d        6열 불량율(%%) : %9.2f\r\n"), nEspoe[5], dEspore[5]);
		strFileData += strData;
		strFileData += _T("\r\n");
	}

	strFileData += _T("4. 불량 내역\r\n");
	if (nMaxStrip == 6)
	{
		strFileData += _T("    -------------------------------------------------------------------------------------------------\r\n");
		strFileData += _T("    번호     불량유형       1열       2열       3열       4열       5열       6열       유형별 불량수\r\n");
		strFileData += _T("    -------------------------------------------------------------------------------------------------\r\n");
		strData.Format(_T("     1         오픈  %10d%10d%10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_OPEN], nEsdp[1][DEF_OPEN], nEsdp[2][DEF_OPEN], nEsdp[3][DEF_OPEN], nEsdp[4][DEF_OPEN], nEsdp[5][DEF_OPEN], nEdp[DEF_OPEN]);
		strFileData += strData;
		strData.Format(_T("     2         쇼트  %10d%10d%10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_SHORT], nEsdp[1][DEF_SHORT], nEsdp[2][DEF_SHORT], nEsdp[3][DEF_SHORT], nEsdp[4][DEF_SHORT], nEsdp[5][DEF_SHORT], nEdp[DEF_SHORT]);
		strFileData += strData;
		strData.Format(_T("     3        U-쇼트 %10d%10d%10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_USHORT], nEsdp[1][DEF_USHORT], nEsdp[2][DEF_USHORT], nEsdp[3][DEF_USHORT], nEsdp[4][DEF_USHORT], nEsdp[5][DEF_USHORT], nEdp[DEF_USHORT]);
		strFileData += strData;
		strData.Format(_T("     4         결손  %10d%10d%10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_NICK], nEsdp[1][DEF_NICK], nEsdp[2][DEF_NICK], nEsdp[3][DEF_NICK], nEsdp[4][DEF_NICK], nEsdp[5][DEF_NICK], nEdp[DEF_NICK]);
		strFileData += strData;
		strData.Format(_T("     5        선간폭 %10d%10d%10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_SPACE], nEsdp[1][DEF_SPACE], nEsdp[2][DEF_SPACE], nEsdp[3][DEF_SPACE], nEsdp[4][DEF_SPACE], nEsdp[5][DEF_SPACE], nEdp[DEF_SPACE]);
		strFileData += strData;
		strData.Format(_T("     6         잔동  %10d%10d%10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_EXTRA], nEsdp[1][DEF_EXTRA], nEsdp[2][DEF_EXTRA], nEsdp[3][DEF_EXTRA], nEsdp[4][DEF_EXTRA], nEsdp[5][DEF_EXTRA], nEdp[DEF_EXTRA]);
		strFileData += strData;
		strData.Format(_T("     7         돌기  %10d%10d%10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_PROTRUSION], nEsdp[1][DEF_PROTRUSION], nEsdp[2][DEF_PROTRUSION], nEsdp[3][DEF_PROTRUSION], nEsdp[4][DEF_PROTRUSION], nEsdp[5][DEF_PROTRUSION], nEdp[DEF_PROTRUSION]);
		strFileData += strData;
		strData.Format(_T("     8         핀홀  %10d%10d%10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_PINHOLE], nEsdp[1][DEF_PINHOLE], nEsdp[2][DEF_PINHOLE], nEsdp[3][DEF_PINHOLE], nEsdp[4][DEF_PINHOLE], nEsdp[5][DEF_PINHOLE], nEdp[DEF_PINHOLE]);
		strFileData += strData;
		strData.Format(_T("     9       패드결함%10d%10d%10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_PAD], nEsdp[1][DEF_PAD], nEsdp[2][DEF_PAD], nEsdp[3][DEF_PAD], nEsdp[4][DEF_PAD], nEsdp[5][DEF_PAD], nEdp[DEF_PAD]);
		strFileData += strData;
		strData.Format(_T("    10        홀오픈 %10d%10d%10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_HOLE_OPEN], nEsdp[1][DEF_HOLE_OPEN], nEsdp[2][DEF_HOLE_OPEN], nEsdp[3][DEF_HOLE_OPEN], nEsdp[4][DEF_HOLE_OPEN], nEsdp[5][DEF_HOLE_OPEN], nEdp[DEF_HOLE_OPEN]);
		strFileData += strData;
		strData.Format(_T("    11        홀없음 %10d%10d%10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_HOLE_MISS], nEsdp[1][DEF_HOLE_MISS], nEsdp[2][DEF_HOLE_MISS], nEsdp[3][DEF_HOLE_MISS], nEsdp[4][DEF_HOLE_MISS], nEsdp[5][DEF_HOLE_MISS], nEdp[DEF_HOLE_MISS]);
		strFileData += strData;
		strData.Format(_T("    12        홀편심 %10d%10d%10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_HOLE_POSITION], nEsdp[1][DEF_HOLE_POSITION], nEsdp[2][DEF_HOLE_POSITION], nEsdp[3][DEF_HOLE_POSITION], nEsdp[4][DEF_HOLE_POSITION], nEsdp[5][DEF_HOLE_POSITION], nEdp[DEF_HOLE_POSITION]);
		strFileData += strData;
		strData.Format(_T("    13       홀내불량%10d%10d%10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_HOLE_DEFECT], nEsdp[1][DEF_HOLE_DEFECT], nEsdp[2][DEF_HOLE_DEFECT], nEsdp[3][DEF_HOLE_DEFECT], nEsdp[4][DEF_HOLE_DEFECT], nEsdp[5][DEF_HOLE_DEFECT], nEdp[DEF_HOLE_DEFECT]);
		strFileData += strData;
		strData.Format(_T("    14          POI  %10d%10d%10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_POI], nEsdp[1][DEF_POI], nEsdp[2][DEF_POI], nEsdp[3][DEF_POI], nEsdp[4][DEF_POI], nEsdp[5][DEF_POI], nEdp[DEF_POI]);
		strFileData += strData;
		strData.Format(_T("    15        VH오픈 %10d%10d%10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_VH_OPEN], nEsdp[1][DEF_VH_OPEN], nEsdp[2][DEF_VH_OPEN], nEsdp[3][DEF_VH_OPEN], nEsdp[4][DEF_VH_OPEN], nEsdp[5][DEF_VH_OPEN], nEdp[DEF_VH_OPEN]);
		strFileData += strData;
		strData.Format(_T("    16        VH없음 %10d%10d%10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_VH_MISS], nEsdp[1][DEF_VH_MISS], nEsdp[2][DEF_VH_MISS], nEsdp[3][DEF_VH_MISS], nEsdp[4][DEF_VH_MISS], nEsdp[5][DEF_VH_MISS], nEdp[DEF_VH_MISS]);
		strFileData += strData;
		strData.Format(_T("    17        VH편심 %10d%10d%10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_VH_POSITION], nEsdp[1][DEF_VH_POSITION], nEsdp[2][DEF_VH_POSITION], nEsdp[3][DEF_VH_POSITION], nEsdp[4][DEF_VH_POSITION], nEsdp[5][DEF_VH_POSITION], nEdp[DEF_VH_POSITION]);
		strFileData += strData;
		strData.Format(_T("    18        VH결함 %10d%10d%10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_VH_DEF], nEsdp[1][DEF_VH_DEF], nEsdp[2][DEF_VH_DEF], nEsdp[3][DEF_VH_DEF], nEsdp[4][DEF_VH_DEF], nEsdp[5][DEF_VH_DEF], nEdp[DEF_VH_DEF]);
		strFileData += strData;
		strData.Format(_T("    19         노광  %10d%10d%10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_LIGHT], nEsdp[1][DEF_LIGHT], nEsdp[2][DEF_LIGHT], nEsdp[3][DEF_LIGHT], nEsdp[4][DEF_LIGHT], nEsdp[5][DEF_LIGHT], nTot);
		strFileData += strData;
		strFileData += _T("    -------------------------------------------------------------------------------------------------\r\n");
	}
	else
	{
		strFileData += _T("    -----------------------------------------------------------------------------\r\n");
		strFileData += _T("    번호     불량유형       1열       2열       3열       4열       유형별 불량수\r\n");
		strFileData += _T("    -----------------------------------------------------------------------------\r\n");
		strData.Format(_T("     1         오픈  %10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_OPEN], nEsdp[1][DEF_OPEN], nEsdp[2][DEF_OPEN], nEsdp[3][DEF_OPEN], nEdp[DEF_OPEN]);
		strFileData += strData;
		strData.Format(_T("     2         쇼트  %10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_SHORT], nEsdp[1][DEF_SHORT], nEsdp[2][DEF_SHORT], nEsdp[3][DEF_SHORT], nEdp[DEF_SHORT]);
		strFileData += strData;
		strData.Format(_T("     3        U-쇼트 %10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_USHORT], nEsdp[1][DEF_USHORT], nEsdp[2][DEF_USHORT], nEsdp[3][DEF_USHORT], nEdp[DEF_USHORT]);
		strFileData += strData;
		strData.Format(_T("     4         결손  %10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_NICK], nEsdp[1][DEF_NICK], nEsdp[2][DEF_NICK], nEsdp[3][DEF_NICK], nEdp[DEF_NICK]);
		strFileData += strData;
		strData.Format(_T("     5        선간폭 %10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_SPACE], nEsdp[1][DEF_SPACE], nEsdp[2][DEF_SPACE], nEsdp[3][DEF_SPACE], nEdp[DEF_SPACE]);
		strFileData += strData;
		strData.Format(_T("     6         잔동  %10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_EXTRA], nEsdp[1][DEF_EXTRA], nEsdp[2][DEF_EXTRA], nEsdp[3][DEF_EXTRA], nEdp[DEF_EXTRA]);
		strFileData += strData;
		strData.Format(_T("     7         돌기  %10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_PROTRUSION], nEsdp[1][DEF_PROTRUSION], nEsdp[2][DEF_PROTRUSION], nEsdp[3][DEF_PROTRUSION], nEdp[DEF_PROTRUSION]);
		strFileData += strData;
		strData.Format(_T("     8         핀홀  %10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_PINHOLE], nEsdp[1][DEF_PINHOLE], nEsdp[2][DEF_PINHOLE], nEsdp[3][DEF_PINHOLE], nEdp[DEF_PINHOLE]);
		strFileData += strData;
		strData.Format(_T("     9       패드결함%10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_PAD], nEsdp[1][DEF_PAD], nEsdp[2][DEF_PAD], nEsdp[3][DEF_PAD], nEdp[DEF_PAD]);
		strFileData += strData;
		strData.Format(_T("    10        홀오픈 %10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_HOLE_OPEN], nEsdp[1][DEF_HOLE_OPEN], nEsdp[2][DEF_HOLE_OPEN], nEsdp[3][DEF_HOLE_OPEN], nEdp[DEF_HOLE_OPEN]);
		strFileData += strData;
		strData.Format(_T("    11        홀없음 %10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_HOLE_MISS], nEsdp[1][DEF_HOLE_MISS], nEsdp[2][DEF_HOLE_MISS], nEsdp[3][DEF_HOLE_MISS], nEdp[DEF_HOLE_MISS]);
		strFileData += strData;
		strData.Format(_T("    12        홀편심 %10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_HOLE_POSITION], nEsdp[1][DEF_HOLE_POSITION], nEsdp[2][DEF_HOLE_POSITION], nEsdp[3][DEF_HOLE_POSITION], nEdp[DEF_HOLE_POSITION]);
		strFileData += strData;
		strData.Format(_T("    13       홀내불량%10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_HOLE_DEFECT], nEsdp[1][DEF_HOLE_DEFECT], nEsdp[2][DEF_HOLE_DEFECT], nEsdp[3][DEF_HOLE_DEFECT], nEdp[DEF_HOLE_DEFECT]);
		strFileData += strData;
		strData.Format(_T("    14          POI  %10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_POI], nEsdp[1][DEF_POI], nEsdp[2][DEF_POI], nEsdp[3][DEF_POI], nEdp[DEF_POI]);
		strFileData += strData;
		strData.Format(_T("    15        VH오픈 %10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_VH_OPEN], nEsdp[1][DEF_VH_OPEN], nEsdp[2][DEF_VH_OPEN], nEsdp[3][DEF_VH_OPEN], nEdp[DEF_VH_OPEN]);
		strFileData += strData;
		strData.Format(_T("    16        VH없음 %10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_VH_MISS], nEsdp[1][DEF_VH_MISS], nEsdp[2][DEF_VH_MISS], nEsdp[3][DEF_VH_MISS], nEdp[DEF_VH_MISS]);
		strFileData += strData;
		strData.Format(_T("    17        VH편심 %10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_VH_POSITION], nEsdp[1][DEF_VH_POSITION], nEsdp[2][DEF_VH_POSITION], nEsdp[3][DEF_VH_POSITION], nEdp[DEF_VH_POSITION]);
		strFileData += strData;
		strData.Format(_T("    18        VH결함 %10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_VH_DEF], nEsdp[1][DEF_VH_DEF], nEsdp[2][DEF_VH_DEF], nEsdp[3][DEF_VH_DEF], nEdp[DEF_VH_DEF]);
		strFileData += strData;
		strData.Format(_T("    19         노광  %10d%10d%10d%10d%20d\r\n"), nEsdp[0][DEF_LIGHT], nEsdp[1][DEF_LIGHT], nEsdp[2][DEF_LIGHT], nEsdp[3][DEF_LIGHT], nTot);
		strFileData += strData;
		strFileData += _T("    -----------------------------------------------------------------------------\r\n");
	}
	strFileData += _T("                                                                                 \r\n");
	strFileData += _T("                                                                                 \r\n");
	strFileData += _T("                                                                                 \r\n");
	strFileData += _T("\r\n");

	return strFileData;
}

CString CReelMap::GetSapp3Txt()
{
	CString strFileData = _T("");
	CString strData;
	int nSum, nStripPcs;
	double dRateBeforeVerify, dRateAfterVerify;

	int nMaxStrip;
#ifdef USE_CAM_MASTER
	nMaxStrip = ((CManagerReelmap*)m_pParent)->m_Master[0].GetStripNum(); // 총 스트립의 갯수
#else
#ifdef TEST_MODE
	nMaxStrip = 4;
#else
	nMaxStrip = MAX_STRIP;
#endif
#endif

	nStripPcs = m_stResult.nEntirePieceNum / nMaxStrip;

	int i, nGoodPerStrip[MAX_STRIP];
	for (i = 0; i < nMaxStrip; i++)
	{
		nGoodPerStrip[i] = nStripPcs - m_stResult.nDefStrip[i];
	}

	// 파일 이름.
	strFileData.Format(_T("FileName : %9s_%4s_%5s.txt\r\n\r\n"), m_stResult.sLot, m_stResult.sProcessNum, m_stResult.sMachin);

	// 열별 투입/완성/수율 Data.
	strFileData += _T("1Q\r\n");
	dRateBeforeVerify = 100.0 * (double)nGoodPerStrip[0] / (double)nStripPcs;
	dRateAfterVerify = 100.0 * (double)nGoodPerStrip[0] / (double)nStripPcs;
	strData.Format(_T("%d,%d,%.1f,%.1f\r\n"), nStripPcs, nGoodPerStrip[0], dRateBeforeVerify, dRateAfterVerify); // 투입수량, 완성수량, Verify전 수량, Verify후 수량
	strFileData += strData;

	strFileData += _T("2Q\r\n");
	dRateBeforeVerify = 100.0 * nGoodPerStrip[1] / nStripPcs;
	dRateAfterVerify = 100.0 * nGoodPerStrip[1] / nStripPcs;
	strData.Format(_T("%d,%d,%.1f,%.1f\r\n"), nStripPcs, nGoodPerStrip[1], dRateBeforeVerify, dRateAfterVerify); // 투입수량, 완성수량, Verify전 수량, Verify후 수량
	strFileData += strData;

	strFileData += _T("3Q\r\n");
	dRateBeforeVerify = 100.0 * nGoodPerStrip[2] / nStripPcs;
	dRateAfterVerify = 100.0 * nGoodPerStrip[2] / nStripPcs;
	strData.Format(_T("%d,%d,%.1f,%.1f\r\n"), nStripPcs, nGoodPerStrip[2], dRateBeforeVerify, dRateAfterVerify); // 투입수량, 완성수량, Verify전 수량, Verify후 수량
	strFileData += strData;

	strFileData += _T("4Q\r\n");
	dRateBeforeVerify = 100.0 * nGoodPerStrip[3] / nStripPcs;
	dRateAfterVerify = 100.0 * nGoodPerStrip[3] / nStripPcs;
	strData.Format(_T("%d,%d,%.1f,%.1f\r\n"), nStripPcs, nGoodPerStrip[3], dRateBeforeVerify, dRateAfterVerify); // 투입수량, 완성수량, Verify전 수량, Verify후 수량
	strFileData += strData;

	if (nMaxStrip == 6)
	{
		strFileData += _T("5Q\r\n");
		dRateBeforeVerify = 100.0 * nGoodPerStrip[4] / nStripPcs;
		dRateAfterVerify = 100.0 * nGoodPerStrip[4] / nStripPcs;
		strData.Format(_T("%d,%d,%.1f,%.1f\r\n"), nStripPcs, nGoodPerStrip[4], dRateBeforeVerify, dRateAfterVerify); // 투입수량, 완성수량, Verify전 수량, Verify후 수량
		strFileData += strData;

		strFileData += _T("6Q\r\n");
		dRateBeforeVerify = 100.0 * nGoodPerStrip[5] / nStripPcs;
		dRateAfterVerify = 100.0 * nGoodPerStrip[5] / nStripPcs;
		strData.Format(_T("%d,%d,%.1f,%.1f\r\n"), nStripPcs, nGoodPerStrip[5], dRateBeforeVerify, dRateAfterVerify); // 투입수량, 완성수량, Verify전 수량, Verify후 수량
		strFileData += strData;
	}

	strFileData += _T("\r\n");


	// 열별 불량 Data.
	strFileData += _T("1X\r\n");

	if (m_stResult.nDefPerStrip[0][DEF_OPEN] > 0 && pDoc->m_nSapp3Code[SAPP3_OPEN] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_OPEN], m_stResult.nDefPerStrip[0][DEF_OPEN]); // 오픈(B102)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[0][DEF_SHORT];// + m_stResult.nDefPerStrip[0][DEF_USHORT];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SHORT] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SHORT], nSum); // 쇼트(B129) // +u쇼트
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[0][DEF_USHORT];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_USHORT] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_USHORT], nSum); // u쇼트(B314)
		strFileData += strData;
	}

	if (m_stResult.nDefPerStrip[0][DEF_NICK] > 0 && pDoc->m_nSapp3Code[SAPP3_NICK] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_NICK], m_stResult.nDefPerStrip[0][DEF_NICK]); // 결손(B137)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[0][DEF_SPACE] + m_stResult.nDefPerStrip[0][DEF_EXTRA] + m_stResult.nDefPerStrip[0][DEF_PROTRUSION];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION], nSum); // 선간폭+잔동+돌기(B160)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[0][DEF_PINHOLE];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PINHOLE], nSum); // 핀홀(B134)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[0][DEF_PAD];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PAD] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PAD], nSum); // 패드(B316)
		strFileData += strData;
	}

	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HOPEN] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HOPEN], m_stResult.nDefPerStrip[0][DEF_HOLE_OPEN]);
		strFileData += strData;
	}
	nSum = m_stResult.nDefPerStrip[0][DEF_HOLE_MISS] + m_stResult.nDefPerStrip[0][DEF_HOLE_POSITION] + m_stResult.nDefPerStrip[0][DEF_HOLE_DEFECT];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD], nSum);
		strFileData += strData;
	}


	strFileData += _T("2X\r\n");

	if (m_stResult.nDefPerStrip[1][DEF_OPEN] > 0 && pDoc->m_nSapp3Code[SAPP3_OPEN] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_OPEN], m_stResult.nDefPerStrip[1][DEF_OPEN]); // 오픈(B102)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[1][DEF_SHORT];// + m_stResult.nDefPerStrip[2][DEF_USHORT];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SHORT] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SHORT], nSum); // 쇼트(B129) // +u쇼트
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[1][DEF_USHORT];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_USHORT] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_USHORT], nSum); // u쇼트(B314)
		strFileData += strData;
	}

	if (m_stResult.nDefPerStrip[1][DEF_NICK] > 0 && pDoc->m_nSapp3Code[SAPP3_NICK] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_NICK], m_stResult.nDefPerStrip[1][DEF_NICK]); // 결손(B137)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[1][DEF_SPACE] + m_stResult.nDefPerStrip[1][DEF_EXTRA] + m_stResult.nDefPerStrip[1][DEF_PROTRUSION];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION], nSum); // 선간폭+잔동+돌기(B160)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[1][DEF_PINHOLE];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PINHOLE], nSum); // 핀홀(B134)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[1][DEF_PAD];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PAD] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PAD], nSum); // 패드(B316)
		strFileData += strData;
	}

	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HOPEN] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HOPEN], m_stResult.nDefPerStrip[2][DEF_HOLE_OPEN]);
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[1][DEF_HOLE_MISS] + m_stResult.nDefPerStrip[1][DEF_HOLE_POSITION] + m_stResult.nDefPerStrip[1][DEF_HOLE_DEFECT];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD], nSum);
		strFileData += strData;
	}


	strFileData += _T("3X\r\n");

	if (m_stResult.nDefPerStrip[2][DEF_OPEN] > 0 && pDoc->m_nSapp3Code[SAPP3_OPEN] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_OPEN], m_stResult.nDefPerStrip[2][DEF_OPEN]); // 오픈(B102)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[2][DEF_SHORT];// + m_stResult.nDefPerStrip[1][DEF_USHORT];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SHORT] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SHORT], nSum); // 쇼트(B129) // +u쇼트
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[2][DEF_USHORT];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_USHORT] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_USHORT], nSum); // u쇼트(B314)
		strFileData += strData;
	}

	if (m_stResult.nDefPerStrip[2][DEF_NICK] > 0 && pDoc->m_nSapp3Code[SAPP3_NICK] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_NICK], m_stResult.nDefPerStrip[2][DEF_NICK]); // 결손(B137)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[2][DEF_SPACE] + m_stResult.nDefPerStrip[2][DEF_EXTRA] + m_stResult.nDefPerStrip[2][DEF_PROTRUSION];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION], nSum); // 선간폭+잔동+돌기(B160)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[2][DEF_PINHOLE];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PINHOLE], nSum); // 핀홀(B134)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[2][DEF_PAD];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PAD] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PAD], nSum); // 패드(B316)
		strFileData += strData;
	}

	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HOPEN] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HOPEN], m_stResult.nDefPerStrip[2][DEF_HOLE_OPEN]);
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[2][DEF_HOLE_MISS] + m_stResult.nDefPerStrip[2][DEF_HOLE_POSITION] + m_stResult.nDefPerStrip[2][DEF_HOLE_DEFECT];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD], nSum);
		strFileData += strData;
	}


	strFileData += _T("4X\r\n");

	if (m_stResult.nDefPerStrip[3][DEF_OPEN] > 0 && pDoc->m_nSapp3Code[SAPP3_OPEN] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_OPEN], m_stResult.nDefPerStrip[3][DEF_OPEN]); // 오픈(B102)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[3][DEF_SHORT];// + m_stResult.nDefPerStrip[3][DEF_USHORT];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SHORT] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SHORT], nSum); // 쇼트(B129) // +u쇼트
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[3][DEF_USHORT];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_USHORT] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_USHORT], nSum); // u쇼트(B314)
		strFileData += strData;
	}

	if (m_stResult.nDefPerStrip[3][DEF_NICK] > 0 && pDoc->m_nSapp3Code[SAPP3_NICK] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_NICK], m_stResult.nDefPerStrip[3][DEF_NICK]); // 결손(B137)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[3][DEF_SPACE] + m_stResult.nDefPerStrip[3][DEF_EXTRA] + m_stResult.nDefPerStrip[3][DEF_PROTRUSION];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION], nSum); // 선간폭+잔동+돌기(B160)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[3][DEF_PINHOLE];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PINHOLE], nSum); // 핀홀(B134)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[3][DEF_PAD];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PAD] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PAD], nSum); // 패드(B316)
		strFileData += strData;
	}

	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HOPEN] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HOPEN], m_stResult.nDefPerStrip[3][DEF_HOLE_OPEN]);
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[3][DEF_HOLE_MISS] + m_stResult.nDefPerStrip[3][DEF_HOLE_POSITION] + m_stResult.nDefPerStrip[3][DEF_HOLE_DEFECT];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD], nSum);
		strFileData += strData;
	}

	if (nMaxStrip == 6)
	{
		strFileData += _T("5X\r\n");

		if (m_stResult.nDefPerStrip[4][DEF_OPEN] > 0 && pDoc->m_nSapp3Code[SAPP3_OPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_OPEN], m_stResult.nDefPerStrip[4][DEF_OPEN]); // 오픈(B102)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[4][DEF_SHORT];// + m_stResult.nDefPerStrip[3][DEF_USHORT];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SHORT], nSum); // 쇼트(B129) // +u쇼트
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[4][DEF_USHORT];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_USHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_USHORT], nSum); // u쇼트(B314)
			strFileData += strData;
		}

		if (m_stResult.nDefPerStrip[4][DEF_NICK] > 0 && pDoc->m_nSapp3Code[SAPP3_NICK] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_NICK], m_stResult.nDefPerStrip[4][DEF_NICK]); // 결손(B137)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[4][DEF_SPACE] + m_stResult.nDefPerStrip[4][DEF_EXTRA] + m_stResult.nDefPerStrip[4][DEF_PROTRUSION];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION], nSum); // 선간폭+잔동+돌기(B160)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[4][DEF_PINHOLE];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PINHOLE], nSum); // 핀홀(B134)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[4][DEF_PAD];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PAD], nSum); // 패드(B316)
			strFileData += strData;
		}

		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HOPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HOPEN], m_stResult.nDefPerStrip[4][DEF_HOLE_OPEN]);
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[4][DEF_HOLE_MISS] + m_stResult.nDefPerStrip[4][DEF_HOLE_POSITION] + m_stResult.nDefPerStrip[4][DEF_HOLE_DEFECT];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD], nSum);
			strFileData += strData;
		}


		strFileData += _T("6X\r\n");

		if (m_stResult.nDefPerStrip[5][DEF_OPEN] > 0 && pDoc->m_nSapp3Code[SAPP3_OPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_OPEN], m_stResult.nDefPerStrip[5][DEF_OPEN]); // 오픈(B102)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[5][DEF_SHORT];// + m_stResult.nDefPerStrip[3][DEF_USHORT];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SHORT], nSum); // 쇼트(B129) // +u쇼트
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[5][DEF_USHORT];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_USHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_USHORT], nSum); // u쇼트(B314)
			strFileData += strData;
		}

		if (m_stResult.nDefPerStrip[5][DEF_NICK] > 0 && pDoc->m_nSapp3Code[SAPP3_NICK] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_NICK], m_stResult.nDefPerStrip[5][DEF_NICK]); // 결손(B137)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[5][DEF_SPACE] + m_stResult.nDefPerStrip[5][DEF_EXTRA] + m_stResult.nDefPerStrip[5][DEF_PROTRUSION];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION], nSum); // 선간폭+잔동+돌기(B160)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[5][DEF_PINHOLE];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PINHOLE], nSum); // 핀홀(B134)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[5][DEF_PAD];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PAD], nSum); // 패드(B316)
			strFileData += strData;
		}

		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HOPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HOPEN], m_stResult.nDefPerStrip[5][DEF_HOLE_OPEN]);
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[5][DEF_HOLE_MISS] + m_stResult.nDefPerStrip[5][DEF_HOLE_POSITION] + m_stResult.nDefPerStrip[5][DEF_HOLE_DEFECT];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD], nSum);
			strFileData += strData;
		}
	}

	// 속도.
	strFileData += _T("\r\nS\r\n");
	strData.Format(_T("%.2f"), m_stResult.dEntireSpeed);
	strFileData += strData;
	strFileData += _T("\r\n");


	return strFileData;
}

CString CReelMap::GetSapp3TxtReverse()
{
	CString strFileData = _T("");
	CString strData;
	int nSum, nStripPcs;
	double dRateBeforeVerify, dRateAfterVerify;

	int nMaxStrip;
#ifdef USE_CAM_MASTER
	nMaxStrip = ((CManagerReelmap*)m_pParent)->m_Master[0].GetStripNum(); // 총 스트립의 갯수
#else
#ifdef TEST_MODE
	nMaxStrip = 4;
#else
	nMaxStrip = MAX_STRIP;
#endif
#endif

	nStripPcs = m_stResult.nEntirePieceNum / nMaxStrip;

	int i, nGoodPerStrip[MAX_STRIP];
	for (i = 0; i < nMaxStrip; i++)
	{
		nGoodPerStrip[i] = nStripPcs - m_stResult.nDefStrip[i];
	}

	// 파일 이름.
	strFileData.Format(_T("FileName : %9s_%4s_%5s.txt\r\n\r\n"), m_stResult.sLot, m_stResult.sProcessNum, m_stResult.sMachin);

	if (nMaxStrip == 6)
	{
		// 열별 투입/완성/수율 Data.
		strFileData += _T("1Q\r\n");
		dRateBeforeVerify = 100.0 * nGoodPerStrip[5] / nStripPcs;
		dRateAfterVerify = 100.0 * nGoodPerStrip[5] / nStripPcs;
		strData.Format(_T("%d,%d,%.1f,%.1f\r\n"), nStripPcs, nGoodPerStrip[5], dRateBeforeVerify, dRateAfterVerify); // 투입수량, 완성수량, Verify전 수량, Verify후 수량
		strFileData += strData;

		strFileData += _T("2Q\r\n");
		dRateBeforeVerify = 100.0 * nGoodPerStrip[4] / nStripPcs;
		dRateAfterVerify = 100.0 * nGoodPerStrip[4] / nStripPcs;
		strData.Format(_T("%d,%d,%.1f,%.1f\r\n"), nStripPcs, nGoodPerStrip[4], dRateBeforeVerify, dRateAfterVerify); // 투입수량, 완성수량, Verify전 수량, Verify후 수량
		strFileData += strData;

		strFileData += _T("3Q\r\n");
		dRateBeforeVerify = 100.0 * nGoodPerStrip[3] / nStripPcs;
		dRateAfterVerify = 100.0 * nGoodPerStrip[3] / nStripPcs;
		strData.Format(_T("%d,%d,%.1f,%.1f\r\n"), nStripPcs, nGoodPerStrip[3], dRateBeforeVerify, dRateAfterVerify); // 투입수량, 완성수량, Verify전 수량, Verify후 수량
		strFileData += strData;

		strFileData += _T("4Q\r\n");
		dRateBeforeVerify = 100.0 * nGoodPerStrip[2] / nStripPcs;
		dRateAfterVerify = 100.0 * nGoodPerStrip[2] / nStripPcs;
		strData.Format(_T("%d,%d,%.1f,%.1f\r\n"), nStripPcs, nGoodPerStrip[2], dRateBeforeVerify, dRateAfterVerify); // 투입수량, 완성수량, Verify전 수량, Verify후 수량
		strFileData += strData;

		strFileData += _T("5Q\r\n");
		dRateBeforeVerify = 100.0 * nGoodPerStrip[1] / nStripPcs;
		dRateAfterVerify = 100.0 * nGoodPerStrip[1] / nStripPcs;
		strData.Format(_T("%d,%d,%.1f,%.1f\r\n"), nStripPcs, nGoodPerStrip[1], dRateBeforeVerify, dRateAfterVerify); // 투입수량, 완성수량, Verify전 수량, Verify후 수량
		strFileData += strData;

		strFileData += _T("6Q\r\n");
		dRateBeforeVerify = 100.0 * nGoodPerStrip[0] / nStripPcs;
		dRateAfterVerify = 100.0 * nGoodPerStrip[0] / nStripPcs;
		strData.Format(_T("%d,%d,%.1f,%.1f\r\n"), nStripPcs, nGoodPerStrip[0], dRateBeforeVerify, dRateAfterVerify); // 투입수량, 완성수량, Verify전 수량, Verify후 수량
		strFileData += strData;

		strFileData += _T("\r\n");


		// 열별 불량 Data.
		strFileData += _T("1X\r\n");

		if (m_stResult.nDefPerStrip[5][DEF_OPEN] > 0 && pDoc->m_nSapp3Code[SAPP3_OPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_OPEN], m_stResult.nDefPerStrip[5][DEF_OPEN]); // 오픈(B102)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[5][DEF_SHORT];// + m_stResult.nDefPerStrip[0][DEF_USHORT];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SHORT], nSum); // 쇼트(B129) // +u쇼트
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[5][DEF_USHORT];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_USHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_USHORT], nSum); // u쇼트(B314)
			strFileData += strData;
		}

		if (m_stResult.nDefPerStrip[5][DEF_NICK] > 0 && pDoc->m_nSapp3Code[SAPP3_NICK] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_NICK], m_stResult.nDefPerStrip[5][DEF_NICK]); // 결손(B137)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[5][DEF_SPACE] + m_stResult.nDefPerStrip[5][DEF_EXTRA] + m_stResult.nDefPerStrip[5][DEF_PROTRUSION];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION], nSum); // 선간폭+잔동+돌기(B160)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[5][DEF_PINHOLE];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PINHOLE], nSum); // 핀홀(B134)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[5][DEF_PAD];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PAD], nSum); // 패드(B316)
			strFileData += strData;
		}

		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HOPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HOPEN], m_stResult.nDefPerStrip[5][DEF_HOLE_OPEN]);
			strFileData += strData;
		}
		nSum = m_stResult.nDefPerStrip[5][DEF_HOLE_MISS] + m_stResult.nDefPerStrip[5][DEF_HOLE_POSITION] + m_stResult.nDefPerStrip[5][DEF_HOLE_DEFECT];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD], nSum);
			strFileData += strData;
		}


		strFileData += _T("2X\r\n");

		if (m_stResult.nDefPerStrip[4][DEF_OPEN] > 0 && pDoc->m_nSapp3Code[SAPP3_OPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_OPEN], m_stResult.nDefPerStrip[4][DEF_OPEN]); // 오픈(B102)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[4][DEF_SHORT];// + m_stResult.nDefPerStrip[2][DEF_USHORT];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SHORT], nSum); // 쇼트(B129) // +u쇼트
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[4][DEF_USHORT];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_USHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_USHORT], nSum); // u쇼트(B314)
			strFileData += strData;
		}

		if (m_stResult.nDefPerStrip[4][DEF_NICK] > 0 && pDoc->m_nSapp3Code[SAPP3_NICK] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_NICK], m_stResult.nDefPerStrip[4][DEF_NICK]); // 결손(B137)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[4][DEF_SPACE] + m_stResult.nDefPerStrip[4][DEF_EXTRA] + m_stResult.nDefPerStrip[4][DEF_PROTRUSION];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION], nSum); // 선간폭+잔동+돌기(B160)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[4][DEF_PINHOLE];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PINHOLE], nSum); // 핀홀(B134)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[4][DEF_PAD];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PAD], nSum); // 패드(B316)
			strFileData += strData;
		}

		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HOPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HOPEN], m_stResult.nDefPerStrip[4][DEF_HOLE_OPEN]);
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[4][DEF_HOLE_MISS] + m_stResult.nDefPerStrip[4][DEF_HOLE_POSITION] + m_stResult.nDefPerStrip[4][DEF_HOLE_DEFECT];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD], nSum);
			strFileData += strData;
		}


		strFileData += _T("3X\r\n");

		if (m_stResult.nDefPerStrip[3][DEF_OPEN] > 0 && pDoc->m_nSapp3Code[SAPP3_OPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_OPEN], m_stResult.nDefPerStrip[3][DEF_OPEN]); // 오픈(B102)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[3][DEF_SHORT];// + m_stResult.nDefPerStrip[1][DEF_USHORT];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SHORT], nSum); // 쇼트(B129) // +u쇼트
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[3][DEF_USHORT];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_USHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_USHORT], nSum); // u쇼트(B314)
			strFileData += strData;
		}

		if (m_stResult.nDefPerStrip[3][DEF_NICK] > 0 && pDoc->m_nSapp3Code[SAPP3_NICK] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_NICK], m_stResult.nDefPerStrip[3][DEF_NICK]); // 결손(B137)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[3][DEF_SPACE] + m_stResult.nDefPerStrip[3][DEF_EXTRA] + m_stResult.nDefPerStrip[3][DEF_PROTRUSION];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION], nSum); // 선간폭+잔동+돌기(B160)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[3][DEF_PINHOLE];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PINHOLE], nSum); // 핀홀(B134)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[3][DEF_PAD];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PAD], nSum); // 패드(B316)
			strFileData += strData;
		}

		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HOPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HOPEN], m_stResult.nDefPerStrip[3][DEF_HOLE_OPEN]);
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[3][DEF_HOLE_MISS] + m_stResult.nDefPerStrip[3][DEF_HOLE_POSITION] + m_stResult.nDefPerStrip[3][DEF_HOLE_DEFECT];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD], nSum);
			strFileData += strData;
		}


		strFileData += _T("4X\r\n");

		if (m_stResult.nDefPerStrip[2][DEF_OPEN] > 0 && pDoc->m_nSapp3Code[SAPP3_OPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_OPEN], m_stResult.nDefPerStrip[2][DEF_OPEN]); // 오픈(B102)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[2][DEF_SHORT];// + m_stResult.nDefPerStrip[3][DEF_USHORT];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SHORT], nSum); // 쇼트(B129) // +u쇼트
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[2][DEF_USHORT];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_USHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_USHORT], nSum); // u쇼트(B314)
			strFileData += strData;
		}

		if (m_stResult.nDefPerStrip[2][DEF_NICK] > 0 && pDoc->m_nSapp3Code[SAPP3_NICK] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_NICK], m_stResult.nDefPerStrip[2][DEF_NICK]); // 결손(B137)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[2][DEF_SPACE] + m_stResult.nDefPerStrip[2][DEF_EXTRA] + m_stResult.nDefPerStrip[2][DEF_PROTRUSION];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION], nSum); // 선간폭+잔동+돌기(B160)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[2][DEF_PINHOLE];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PINHOLE], nSum); // 핀홀(B134)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[2][DEF_PAD];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PAD], nSum); // 패드(B316)
			strFileData += strData;
		}

		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HOPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HOPEN], m_stResult.nDefPerStrip[2][DEF_HOLE_OPEN]);
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[2][DEF_HOLE_MISS] + m_stResult.nDefPerStrip[2][DEF_HOLE_POSITION] + m_stResult.nDefPerStrip[2][DEF_HOLE_DEFECT];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD], nSum);
			strFileData += strData;
		}


		strFileData += _T("5X\r\n");

		if (m_stResult.nDefPerStrip[1][DEF_OPEN] > 0 && pDoc->m_nSapp3Code[SAPP3_OPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_OPEN], m_stResult.nDefPerStrip[1][DEF_OPEN]); // 오픈(B102)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[1][DEF_SHORT];// + m_stResult.nDefPerStrip[3][DEF_USHORT];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SHORT], nSum); // 쇼트(B129) // +u쇼트
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[1][DEF_USHORT];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_USHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_USHORT], nSum); // u쇼트(B314)
			strFileData += strData;
		}

		if (m_stResult.nDefPerStrip[1][DEF_NICK] > 0 && pDoc->m_nSapp3Code[SAPP3_NICK] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_NICK], m_stResult.nDefPerStrip[1][DEF_NICK]); // 결손(B137)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[1][DEF_SPACE] + m_stResult.nDefPerStrip[1][DEF_EXTRA] + m_stResult.nDefPerStrip[1][DEF_PROTRUSION];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION], nSum); // 선간폭+잔동+돌기(B160)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[1][DEF_PINHOLE];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PINHOLE], nSum); // 핀홀(B134)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[1][DEF_PAD];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PAD], nSum); // 패드(B316)
			strFileData += strData;
		}

		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HOPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HOPEN], m_stResult.nDefPerStrip[1][DEF_HOLE_OPEN]);
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[1][DEF_HOLE_MISS] + m_stResult.nDefPerStrip[1][DEF_HOLE_POSITION] + m_stResult.nDefPerStrip[1][DEF_HOLE_DEFECT];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD], nSum);
			strFileData += strData;
		}

		strFileData += _T("6X\r\n");

		if (m_stResult.nDefPerStrip[0][DEF_OPEN] > 0 && pDoc->m_nSapp3Code[SAPP3_OPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_OPEN], m_stResult.nDefPerStrip[0][DEF_OPEN]); // 오픈(B102)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[0][DEF_SHORT];// + m_stResult.nDefPerStrip[3][DEF_USHORT];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SHORT], nSum); // 쇼트(B129) // +u쇼트
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[0][DEF_USHORT];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_USHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_USHORT], nSum); // u쇼트(B314)
			strFileData += strData;
		}

		if (m_stResult.nDefPerStrip[0][DEF_NICK] > 0 && pDoc->m_nSapp3Code[SAPP3_NICK] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_NICK], m_stResult.nDefPerStrip[0][DEF_NICK]); // 결손(B137)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[0][DEF_SPACE] + m_stResult.nDefPerStrip[0][DEF_EXTRA] + m_stResult.nDefPerStrip[0][DEF_PROTRUSION];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION], nSum); // 선간폭+잔동+돌기(B160)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[0][DEF_PINHOLE];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PINHOLE], nSum); // 핀홀(B134)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[0][DEF_PAD];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PAD], nSum); // 패드(B316)
			strFileData += strData;
		}

		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HOPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HOPEN], m_stResult.nDefPerStrip[0][DEF_HOLE_OPEN]);
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[0][DEF_HOLE_MISS] + m_stResult.nDefPerStrip[0][DEF_HOLE_POSITION] + m_stResult.nDefPerStrip[0][DEF_HOLE_DEFECT];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD], nSum);
			strFileData += strData;
		}
	}
	else
	{
		// 열별 투입/완성/수율 Data.
		strFileData += _T("1Q\r\n");
		dRateBeforeVerify = 100.0 * nGoodPerStrip[3] / nStripPcs;
		dRateAfterVerify = 100.0 * nGoodPerStrip[3] / nStripPcs;
		strData.Format(_T("%d,%d,%.1f,%.1f\r\n"), nStripPcs, nGoodPerStrip[3], dRateBeforeVerify, dRateAfterVerify); // 투입수량, 완성수량, Verify전 수량, Verify후 수량
		strFileData += strData;

		strFileData += _T("2Q\r\n");
		dRateBeforeVerify = 100.0 * nGoodPerStrip[2] / nStripPcs;
		dRateAfterVerify = 100.0 * nGoodPerStrip[2] / nStripPcs;
		strData.Format(_T("%d,%d,%.1f,%.1f\r\n"), nStripPcs, nGoodPerStrip[2], dRateBeforeVerify, dRateAfterVerify); // 투입수량, 완성수량, Verify전 수량, Verify후 수량
		strFileData += strData;

		strFileData += _T("3Q\r\n");
		dRateBeforeVerify = 100.0 * nGoodPerStrip[1] / nStripPcs;
		dRateAfterVerify = 100.0 * nGoodPerStrip[1] / nStripPcs;
		strData.Format(_T("%d,%d,%.1f,%.1f\r\n"), nStripPcs, nGoodPerStrip[1], dRateBeforeVerify, dRateAfterVerify); // 투입수량, 완성수량, Verify전 수량, Verify후 수량
		strFileData += strData;

		strFileData += _T("4Q\r\n");
		dRateBeforeVerify = 100.0 * nGoodPerStrip[0] / nStripPcs;
		dRateAfterVerify = 100.0 * nGoodPerStrip[0] / nStripPcs;
		strData.Format(_T("%d,%d,%.1f,%.1f\r\n"), nStripPcs, nGoodPerStrip[0], dRateBeforeVerify, dRateAfterVerify); // 투입수량, 완성수량, Verify전 수량, Verify후 수량
		strFileData += strData;

		strFileData += _T("\r\n");


		// 열별 불량 Data.
		strFileData += _T("1X\r\n");

		if (m_stResult.nDefPerStrip[3][DEF_OPEN] > 0 && pDoc->m_nSapp3Code[SAPP3_OPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_OPEN], m_stResult.nDefPerStrip[3][DEF_OPEN]); // 오픈(B102)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[3][DEF_SHORT];// + m_stResult.nDefPerStrip[0][DEF_USHORT];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SHORT], nSum); // 쇼트(B129) // +u쇼트
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[3][DEF_USHORT];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_USHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_USHORT], nSum); // u쇼트(B314)
			strFileData += strData;
		}

		if (m_stResult.nDefPerStrip[3][DEF_NICK] > 0 && pDoc->m_nSapp3Code[SAPP3_NICK] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_NICK], m_stResult.nDefPerStrip[3][DEF_NICK]); // 결손(B137)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[3][DEF_SPACE] + m_stResult.nDefPerStrip[3][DEF_EXTRA] + m_stResult.nDefPerStrip[3][DEF_PROTRUSION];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION], nSum); // 선간폭+잔동+돌기(B160)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[3][DEF_PINHOLE];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PINHOLE], nSum); // 핀홀(B134)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[3][DEF_PAD];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PAD], nSum); // 패드(B316)
			strFileData += strData;
		}

		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HOPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HOPEN], m_stResult.nDefPerStrip[3][DEF_HOLE_OPEN]);
			strFileData += strData;
		}
		nSum = m_stResult.nDefPerStrip[3][DEF_HOLE_MISS] + m_stResult.nDefPerStrip[3][DEF_HOLE_POSITION] + m_stResult.nDefPerStrip[3][DEF_HOLE_DEFECT];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD], nSum);
			strFileData += strData;
		}


		strFileData += _T("2X\r\n");

		if (m_stResult.nDefPerStrip[2][DEF_OPEN] > 0 && pDoc->m_nSapp3Code[SAPP3_OPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_OPEN], m_stResult.nDefPerStrip[2][DEF_OPEN]); // 오픈(B102)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[2][DEF_SHORT];// + m_stResult.nDefPerStrip[2][DEF_USHORT];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SHORT], nSum); // 쇼트(B129) // +u쇼트
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[2][DEF_USHORT];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_USHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_USHORT], nSum); // u쇼트(B314)
			strFileData += strData;
		}

		if (m_stResult.nDefPerStrip[2][DEF_NICK] > 0 && pDoc->m_nSapp3Code[SAPP3_NICK] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_NICK], m_stResult.nDefPerStrip[2][DEF_NICK]); // 결손(B137)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[2][DEF_SPACE] + m_stResult.nDefPerStrip[2][DEF_EXTRA] + m_stResult.nDefPerStrip[2][DEF_PROTRUSION];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION], nSum); // 선간폭+잔동+돌기(B160)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[2][DEF_PINHOLE];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PINHOLE], nSum); // 핀홀(B134)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[2][DEF_PAD];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PAD], nSum); // 패드(B316)
			strFileData += strData;
		}

		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HOPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HOPEN], m_stResult.nDefPerStrip[2][DEF_HOLE_OPEN]);
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[2][DEF_HOLE_MISS] + m_stResult.nDefPerStrip[2][DEF_HOLE_POSITION] + m_stResult.nDefPerStrip[2][DEF_HOLE_DEFECT];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD], nSum);
			strFileData += strData;
		}


		strFileData += _T("3X\r\n");

		if (m_stResult.nDefPerStrip[1][DEF_OPEN] > 0 && pDoc->m_nSapp3Code[SAPP3_OPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_OPEN], m_stResult.nDefPerStrip[1][DEF_OPEN]); // 오픈(B102)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[1][DEF_SHORT];// + m_stResult.nDefPerStrip[1][DEF_USHORT];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SHORT], nSum); // 쇼트(B129) // +u쇼트
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[1][DEF_USHORT];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_USHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_USHORT], nSum); // u쇼트(B314)
			strFileData += strData;
		}

		if (m_stResult.nDefPerStrip[1][DEF_NICK] > 0 && pDoc->m_nSapp3Code[SAPP3_NICK] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_NICK], m_stResult.nDefPerStrip[1][DEF_NICK]); // 결손(B137)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[1][DEF_SPACE] + m_stResult.nDefPerStrip[1][DEF_EXTRA] + m_stResult.nDefPerStrip[1][DEF_PROTRUSION];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION], nSum); // 선간폭+잔동+돌기(B160)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[1][DEF_PINHOLE];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PINHOLE], nSum); // 핀홀(B134)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[1][DEF_PAD];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PAD], nSum); // 패드(B316)
			strFileData += strData;
		}

		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HOPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HOPEN], m_stResult.nDefPerStrip[1][DEF_HOLE_OPEN]);
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[1][DEF_HOLE_MISS] + m_stResult.nDefPerStrip[1][DEF_HOLE_POSITION] + m_stResult.nDefPerStrip[1][DEF_HOLE_DEFECT];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD], nSum);
			strFileData += strData;
		}


		strFileData += _T("4X\r\n");

		if (m_stResult.nDefPerStrip[0][DEF_OPEN] > 0 && pDoc->m_nSapp3Code[SAPP3_OPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_OPEN], m_stResult.nDefPerStrip[0][DEF_OPEN]); // 오픈(B102)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[0][DEF_SHORT];// + m_stResult.nDefPerStrip[3][DEF_USHORT];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SHORT], nSum); // 쇼트(B129) // +u쇼트
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[0][DEF_USHORT];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_USHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_USHORT], nSum); // u쇼트(B314)
			strFileData += strData;
		}

		if (m_stResult.nDefPerStrip[0][DEF_NICK] > 0 && pDoc->m_nSapp3Code[SAPP3_NICK] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_NICK], m_stResult.nDefPerStrip[0][DEF_NICK]); // 결손(B137)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[0][DEF_SPACE] + m_stResult.nDefPerStrip[0][DEF_EXTRA] + m_stResult.nDefPerStrip[0][DEF_PROTRUSION];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION], nSum); // 선간폭+잔동+돌기(B160)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[0][DEF_PINHOLE];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PINHOLE], nSum); // 핀홀(B134)
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[0][DEF_PAD];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PAD], nSum); // 패드(B316)
			strFileData += strData;
		}

		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HOPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HOPEN], m_stResult.nDefPerStrip[0][DEF_HOLE_OPEN]);
			strFileData += strData;
		}

		nSum = m_stResult.nDefPerStrip[0][DEF_HOLE_MISS] + m_stResult.nDefPerStrip[0][DEF_HOLE_POSITION] + m_stResult.nDefPerStrip[0][DEF_HOLE_DEFECT];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD], nSum);
			strFileData += strData;
		}
	}

	// 속도.
	strFileData += _T("\r\nS\r\n");
	strData.Format(_T("%.2f"), m_stResult.dEntireSpeed);
	strFileData += strData;
	strFileData += _T("\r\n");


	return strFileData;
}

CString CReelMap::GetPath()
{
	return m_sPathBuf;
}

CString CReelMap::GetIpPath()
{
	//return m_sIpPath;
	CString sPath = _T("");
	CString Path[5], str;

	switch (m_nLayer)
	{
	case RMAP_UP:
		str = _T("ReelMapDataUp.txt");
		sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sIpPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			pDoc->WorkingInfo.LastJob.sLotUp,
			pDoc->WorkingInfo.LastJob.sLayerUp,
			str);
		break;
	case RMAP_ALLUP:
		str = _T("ReelMapDataAll.txt");
		sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sIpPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			pDoc->WorkingInfo.LastJob.sLotUp,
			pDoc->WorkingInfo.LastJob.sLayerUp,
			str);
		break;
	case RMAP_DN:
		str = _T("ReelMapDataDn.txt");
		sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sIpPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			pDoc->WorkingInfo.LastJob.sLotUp,
			pDoc->WorkingInfo.LastJob.sLayerDn,
			str);
		break;
	case RMAP_ALLDN:
		str = _T("ReelMapDataAll.txt");
		sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sIpPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			pDoc->WorkingInfo.LastJob.sLotUp,
			pDoc->WorkingInfo.LastJob.sLayerDn,
			str);
		break;
	case RMAP_INNER_UP:
		str = _T("ReelMapDataUp.txt");
		Path[0] = pDoc->WorkingInfo.System.sIpPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerUp;
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], Path[4], str);
		break;
	case RMAP_INNER_DN:
		str = _T("ReelMapDataDn.txt");
		Path[0] = pDoc->WorkingInfo.System.sIpPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerDn;
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], Path[4], str);
		break;
	case RMAP_INNER_ALLUP:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sIpPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerUp;
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], Path[4], str);
		break;
	case RMAP_INNER_ALLDN:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sIpPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerDn;
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], Path[4], str);
		break;
	case RMAP_INOUTER_UP:
		str = _T("ReelMapDataIO.txt");
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sIpPathItsFile,
			pDoc->WorkingInfo.LastJob.sModelUp, pDoc->WorkingInfo.LastJob.sEngItsCode,
			pDoc->WorkingInfo.LastJob.sLotUp, pDoc->WorkingInfo.LastJob.sLayerUp,
			str);
		break;
	case RMAP_INOUTER_DN:
		str = _T("ReelMapDataIO.txt");
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sIpPathItsFile,
			pDoc->WorkingInfo.LastJob.sModelUp, pDoc->WorkingInfo.LastJob.sEngItsCode,
			pDoc->WorkingInfo.LastJob.sLotUp, pDoc->WorkingInfo.LastJob.sLayerDn,
			str);
		break;
	case RMAP_ITS:
		pDoc->GetCurrentInfoEng();
		str = _T("ReelMapDataIts.txt");
		sPath.Format(_T("%s%s\\%s\\%s"), pDoc->WorkingInfo.System.sIpPathItsFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			pDoc->WorkingInfo.LastJob.sEngItsCode,	//pDoc->m_sItsCode,
			str);
		break;
	}

	m_sIpPath = sPath;
	return sPath;
}


void CReelMap::StartThreadFinalCopyItsFiles()
{
	m_bRtnThreadFinalCopyItsFiles = FALSE;
	m_bThreadAliveFinalCopyItsFiles = TRUE;
	m_ThreadTaskFinalCopyItsFiles.Start(GetSafeHwnd(), this, ThreadProcFinalCopyItsFiles);// Start the thread
}

void CReelMap::StopThreadFinalCopyItsFiles()
{
	m_ThreadTaskFinalCopyItsFiles.Stop();// Stop the thread
}

// Home thread body
BOOL CReelMap::ThreadProcFinalCopyItsFiles(LPVOID lpContext)
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CReelMap* pThread = reinterpret_cast< CReelMap* >(lpContext);
	//	DWORD dwTimePeriod = 10; // 10ms sec sleep

	pThread->m_bRtnThreadFinalCopyItsFiles = TRUE;
	pThread->m_bThreadAliveFinalCopyItsFiles = TRUE;

	//pThread->m_cs.Lock();
	pThread->m_bRtnThreadFinalCopyItsFiles = pThread->FinalCopyItsFiles();
	//pThread->m_cs.Unlock();

	pThread->m_bThreadAliveFinalCopyItsFiles = FALSE;

	return (pThread->m_bRtnThreadFinalCopyItsFiles);
}

BOOL CReelMap::FinalCopyItsFiles()
{
	CString sPathSrc, sPathDest;
	CString sPathSrcDir, sPathDestDir;
	CString sItsFolderSrcPath = ((CManagerReelmap*)m_pParent)->GetItsFolderPath();
	CString sItsFolderDestPath = ((CManagerReelmap*)m_pParent)->GetItsTargetFolderPath();

	if (pDoc->GetTestMode() == MODE_INNER)
		sPathSrcDir.Format(_T("%s\\Inner"), sItsFolderSrcPath);
	else if (pDoc->GetTestMode() == MODE_OUTER)
		sPathSrcDir.Format(_T("%s\\Outer"), sItsFolderSrcPath);
	else
		return FALSE;

	CFileFind cFile;
	BOOL bExist = cFile.FindFile(sPathSrcDir + _T("\\*.dat"));
	if (!bExist)
	{
		return FALSE; // dat파일이 존재하지 않음.
	}

	CString sFileName;
	int nTot = 0;
	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (!cFile.IsDirectory())
		{
			sFileName = cFile.GetFileName();
			sPathSrc.Format(_T("%s\\%s"), sPathSrcDir, sFileName);
			sPathDest.Format(_T("%s\\%s"), sItsFolderDestPath, sFileName);
			CopyItsFile(sPathSrc, sPathDest);
			nTot++;
		}
		if (!m_bThreadAliveFinalCopyItsFiles)
			break;
	}

	return TRUE;
}

BOOL CReelMap::CopyItsFile(CString sPathSrc, CString sPathDest)
{
	BOOL bRtn = FALSE;

	CFileFind cFile;

	if (!cFile.FindFile(sPathSrc))
	{
		return FALSE;
	}

	if(pDoc->m_pFile)
		bRtn = pDoc->m_pFile->Copy(sPathSrc, sPathDest);			// ITS 파일을 복사한다.

	return bRtn;
}


int CReelMap::GetRptFixPcs(int nCol, int nRow)
{
	return m_FixPcsRpt[nCol][nRow];;
}

BOOL CReelMap::SetItsSerialInfo(int nItsSerial)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	CString sName = _T("");
	CString str, sPath, Path[6];

	Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
	Path[1] = ((CManagerReelmap*)m_pParent)->m_sEngModel;
	Path[2] = ((CManagerReelmap*)m_pParent)->m_sItsCode;
	Path[3] = ((CManagerReelmap*)m_pParent)->m_sEngLotNum;
	Path[4] = ((CManagerReelmap*)m_pParent)->m_sEngLayerUp;
	Path[5] = ((CManagerReelmap*)m_pParent)->m_sEngLayerDn;

	if (Path[0].IsEmpty() || Path[1].IsEmpty() || Path[2].IsEmpty() || Path[3].IsEmpty() || Path[4].IsEmpty())
	{
		sPath.Format(_T("%s%s\\%s\\%s"), Path[0], Path[1], Path[2], sName); // ITS_Code.txt
		str.Format(_T("It is trouble to write SetItsSerialInfo.txt\r\n%s"), sPath);
		pView->MsgBox(str);
		return FALSE;
	}

	if (bDualTest)
	{
		if (Path[5].IsEmpty())
			return FALSE;
	}

	sPath.Format(_T("%s%s"), Path[0], Path[1]);					// 모델 폴더
	if (!pDoc->DirectoryExists(sPath))
		CreateDirectory(sPath, NULL);

	sPath.Format(_T("%s%s\\%s"), Path[0], Path[1], Path[2]);	// ITS 코드 폴더
	if (!pDoc->DirectoryExists(sPath))
		CreateDirectory(sPath, NULL);

	sName.Format(_T("%s.txt"), ((CManagerReelmap*)m_pParent)->m_sItsCode);
	sPath.Format(_T("%s%s\\%s\\%s"), Path[0], Path[1], Path[2], sName); // ITS_Code.txt

	CString sItsSerail;
	sItsSerail.Format(_T("%d"), nItsSerial);

	// ITS_Code.txt 파일의 정보
	str.Format(_T("%d"), pDoc->GetTestMode());
	::WritePrivateProfileString(sItsSerail, _T("Test Mode"), str, sPath);
	str.Format(_T("%d"), (bDualTest ? 1 : 0));
	::WritePrivateProfileString(sItsSerail, _T("Dual Test"), str, sPath);
	::WritePrivateProfileString(sItsSerail, _T("Process Unit Code"), ((CManagerReelmap*)m_pParent)->m_sEngProcessNum, sPath);
	::WritePrivateProfileString(sItsSerail, _T("Current Model"), ((CManagerReelmap*)m_pParent)->m_sEngModel, sPath);
	::WritePrivateProfileString(sItsSerail, _T("Its Code"), ((CManagerReelmap*)m_pParent)->m_sItsCode, sPath);
	::WritePrivateProfileString(sItsSerail, _T("Current Lot"), ((CManagerReelmap*)m_pParent)->m_sEngLotNum, sPath);
	::WritePrivateProfileString(sItsSerail, _T("Current Layer Up"), ((CManagerReelmap*)m_pParent)->m_sEngLayerUp, sPath);

	if (bDualTest)
		::WritePrivateProfileString(sItsSerail, _T("Current Layer Dn"), ((CManagerReelmap*)m_pParent)->m_sEngLayerDn, sPath);
	else
		::WritePrivateProfileString(sItsSerail, _T("Current Layer Dn"), _T(""), sPath);

	return TRUE;
}

int CReelMap::GetPcrIdx(int nSerial, BOOL bNewLot)
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

CString CReelMap::GetItsPath(int nSerial, int nLayer)	// RMAP_UP, RMAP_DN, RMAP_INNER_UP, RMAP_INNER_DN
{
	CString sPath, str;
	CString sItsFolderPath = ((CManagerReelmap*)m_pParent)->GetItsFolderPath();
	CString sTime = pView->GetTimeIts();

	switch (nLayer)
	{
	case RMAP_UP: // 외층 Top
		str.Format(_T("%s_L1_%04d_T_%s_%s_AVR01_%s.dat"), ((CManagerReelmap*)m_pParent)->m_sItsCode, nSerial, pDoc->WorkingInfo.LastJob.sSelUserName, pDoc->WorkingInfo.System.sMcName, sTime);
		sPath.Format(_T("%s\\Outer\\%s"), sItsFolderPath, str);
		break;
	case RMAP_DN: // 외층 Bottom
		str.Format(_T("%s_L4_%04d_B_%s_%s_AVR01_%s.dat"), ((CManagerReelmap*)m_pParent)->m_sItsCode, nSerial, pDoc->WorkingInfo.LastJob.sSelUserName, pDoc->WorkingInfo.System.sMcName, sTime);
		sPath.Format(_T("%s\\Outer\\%s"), sItsFolderPath, str);
		break;
	case RMAP_INNER_UP: // 내층 Top
		str.Format(_T("%s_L2_%04d_T_%s_%s_AVR01_%s.dat"), ((CManagerReelmap*)m_pParent)->m_sItsCode, nSerial, pDoc->WorkingInfo.LastJob.sSelUserName, pDoc->WorkingInfo.System.sMcName, sTime);
		sPath.Format(_T("%s\\Inner\\%s"), sItsFolderPath, str);
		break;
	case RMAP_INNER_DN: // 내층 Bottom
		str.Format(_T("%s_L3_%04d_B_%s_%s_AVR01_%s.dat"), ((CManagerReelmap*)m_pParent)->m_sItsCode, nSerial, pDoc->WorkingInfo.LastJob.sSelUserName, pDoc->WorkingInfo.System.sMcName, sTime);
		sPath.Format(_T("%s\\Inner\\%s"), sItsFolderPath, str);
		break;
	}

	return sPath;
}
