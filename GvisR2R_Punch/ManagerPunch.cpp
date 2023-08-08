// ManagerPunch.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "GvisR2R_Punch.h"
#include "ManagerPunch.h"

#include "Device/SR1000WDEF.h"


#include "MainFrm.h"
#include "GvisR2R_PunchDoc.h"
#include "GvisR2R_PunchView.h"

extern CMainFrame* pFrm;
extern CGvisR2R_PunchDoc* pDoc;
extern CGvisR2R_PunchView* pView;

// CManagerPunch

IMPLEMENT_DYNAMIC(CManagerPunch, CWnd)

CManagerPunch::CManagerPunch(CWnd* pParent /*=NULL*/)
{
	m_pParent = pParent;

	m_pLight = NULL;
#ifdef USE_FLUCK
	m_pFluck = NULL;
#endif
	m_pVoiceCoil[0] = NULL;
	m_pVoiceCoil[1] = NULL;
	m_pMotion = NULL;
	m_pMpe = NULL;
	m_pVision[0] = NULL;
	m_pVision[1] = NULL;
	m_pVisionInner[0] = NULL;
	m_pVisionInner[1] = NULL;
	//m_pEngrave = NULL;
	m_pSr1000w = NULL;
	m_pDts = NULL;


	m_bCycleStop = FALSE;
	m_bProbDn[0] = FALSE;
	m_bProbDn[1] = FALSE;
	m_bAuto = FALSE;
	m_bManual = FALSE;
	m_bOneCycle = FALSE;
	m_bDoMk[0] = TRUE;			// [nCam]
	m_bDoMk[1] = TRUE;			// [nCam]
	m_bDoneMk[0] = FALSE;		// [nCam]
	m_bDoneMk[1] = FALSE;		// [nCam]
	m_bReMark[0] = FALSE;		// [nCam]
	m_bReMark[1] = FALSE;		// [nCam]

	m_nTotMk[0] = 0;
	m_nCurMk[0] = 0;
	m_nTotMk[1] = 0;
	m_nCurMk[1] = 0;
	m_nPrevTotMk[0] = 0;
	m_nPrevCurMk[0] = 0;
	m_nPrevTotMk[1] = 0;
	m_nPrevCurMk[1] = 0;
	m_nMkPcs[0] = 0;
	m_nMkPcs[1] = 0;
	m_nMkPcs[2] = 0;
	m_nMkPcs[3] = 0;

	m_nStepMk[0] = 0;
	m_nStepMk[1] = 0;

	m_bTHREAD_MK[0] = FALSE;
	m_bTHREAD_MK[1] = FALSE;
	m_bTHREAD_MK[2] = FALSE;
	m_bTHREAD_MK[3] = FALSE;

	// DoMark0(), DoMark1()
	m_bThread[0] = FALSE;
	m_dwThreadTick[0] = 0;

	// ChkCollision
	m_bThread[1] = FALSE;
	m_dwThreadTick[1] = 0;

	m_bReview = FALSE;
	m_bCam = FALSE;
	m_nStepElecChk = 0;

	m_bTIM_MPE_IO = FALSE;
	m_bStopFromThread = FALSE;
	m_bBuzzerFromThread = FALSE;
	m_bCollision[0] = FALSE;
	m_bCollision[1] = FALSE;

	m_sGet2dCodeLot = _T("");
	m_nGet2dCodeSerial = 0;
	m_bSwStopNow = FALSE;
	m_bSwRun = FALSE; m_bSwRunF = FALSE;

	for (int a = 0; a < 2; a++)
	{
		for (int b = 0; b < MAX_STRIP; b++)
		{
			m_nMkStrip[a][b] = 0;
			m_bRejectDone[a][b] = FALSE;
		}
	}

	RECT rt = { 0,0,0,0 };
	if (!Create(NULL, NULL, WS_CHILD, rt, pParent, 0))
		AfxMessageBox(_T("CManagerPunch::Create() Failed!!!"));

	HwInit();
	InitPLC();
	InitIO();
	InitIoWrite();
	SetMainMc(TRUE);
	TcpIpInit();
	DtsInit();
	InitThread();
	ResetPriority();

	m_bTIM_MPE_IO = TRUE;
	SetTimer(TIM_MPE_IO, 100, NULL);
}

CManagerPunch::~CManagerPunch()
{
	KillThread();
	m_bTIM_MPE_IO = FALSE;

	InitIoWrite();
	SetMainMc(FALSE);
	HwKill();
}


BEGIN_MESSAGE_MAP(CManagerPunch, CWnd)
	ON_WM_TIMER()
	ON_MESSAGE(WM_CLIENT_RECEIVED_SR, wmClientReceivedSr)
	//ON_MESSAGE(WM_CLIENT_RECEIVED, wmClientReceived)
END_MESSAGE_MAP()


// CManagerPunch �޽��� ó�����Դϴ�.

void CManagerPunch::KillThread()
{
	if (m_bThread[0])	// DoMark0(), DoMark1()
	{
		m_Thread[0].Stop();
		Sleep(100);
		while (m_bThread[0])
		{
			Sleep(20);
		}
	}

	if (m_bThread[1])	// ChkCollision
	{
		m_Thread[1].Stop();
		Sleep(100);
		while (m_bThread[1])
		{
			Sleep(20);
		}
	}
}

BOOL CManagerPunch::HwInit()
{
	if (m_pLight)
	{
		delete m_pLight;
		m_pLight = NULL;
	}
	m_pLight = new CLight(this);
	m_pLight->Init();
#ifdef USE_FLUCK
	if (pDoc->WorkingInfo.Fluck.bUse)
	{
		if (m_pFluck)
		{
			delete m_pFluck;
			m_pFluck = NULL;
		}
		m_pFluck = new CFluck(this);
		m_pFluck->Init();
	}
#endif
	if (m_pVoiceCoil[0])
	{
		delete m_pVoiceCoil[0];
		m_pVoiceCoil[0] = NULL;
	}

	m_pVoiceCoil[0] = new CSmac(this);
	m_pVoiceCoil[0]->SetCh(VOICE_COIL_FIRST_CAM);
	m_pVoiceCoil[0]->Init();

	if (m_pVoiceCoil[1])
	{
		delete m_pVoiceCoil[1];
		m_pVoiceCoil[1] = NULL;
	}

	m_pVoiceCoil[1] = new CSmac(this);
	m_pVoiceCoil[1]->SetCh(VOICE_COIL_SECOND_CAM);
	m_pVoiceCoil[1]->Init();

	if (m_pMotion)
	{
		delete m_pMotion;
		m_pMotion = NULL;
	}
	m_pMotion = new CMotion(this);
	if (!m_pMotion->InitBoard())
	{
		MsgBox(_T("XMP ���� �ʱ�ȭ ����, �ٽ� �����ϼ���.!!!"));
		PostMessage(WM_CLOSE);
		return FALSE;
	}

#ifdef USE_MPE
	if (!m_pMpe)
		m_pMpe = new CMpDevice(this);
	if (!m_pMpe->Init(1, 1))
	{
		MsgBox(_T("��īƮ�θ�ũ(MC0)�� ������α׷��� ���� ��, �ٽ� �����ϼ���.!!!"));
		PostMessage(WM_CLOSE);
		return FALSE;
	}
#endif

	return TRUE;
}

void CManagerPunch::HwKill()
{
#ifdef USE_VISION
	if (m_pVisionInner[1])
	{
		delete m_pVisionInner[1];
		m_pVisionInner[1] = NULL;
	}

	if (m_pVisionInner[0])
	{
		delete m_pVisionInner[0];
		m_pVisionInner[0] = NULL;
	}
	if (m_pVision[1])
	{
		delete m_pVision[1];
		m_pVision[1] = NULL;
	}

	if (m_pVision[0])
	{
		delete m_pVision[0];
		m_pVision[0] = NULL;
	}
#endif

	if (m_pMotion)
	{
		delete m_pMotion;
		m_pMotion = NULL;
	}

	if (m_pMpe)
	{
		delete m_pMpe;
		m_pMpe = NULL;
	}

	if (m_pVoiceCoil[0])
	{
		delete m_pVoiceCoil[0];
		m_pVoiceCoil[0] = NULL;
	}

	if (m_pVoiceCoil[1])
	{
		delete m_pVoiceCoil[1];
		m_pVoiceCoil[1] = NULL;
	}

	if (m_pLight)
	{
		m_pLight->Close();
		delete m_pLight;
		m_pLight = NULL;
	}

	//if (m_pEngrave)
	//{
	//	m_pEngrave->Close();
	//	delete m_pEngrave;
	//	m_pEngrave = NULL;
	//}

	if (m_pSr1000w)
	{
		m_pSr1000w->Close();
		delete m_pSr1000w;
		m_pSr1000w = NULL;
	}

	if (m_pDts)
	{
		delete m_pDts;
		m_pDts = NULL;
	}

#ifdef USE_FLUCK
	if (m_pFluck)
	{
		m_pFluck->Close();
		delete m_pFluck;
		m_pFluck = NULL;
	}
#endif
}

BOOL CManagerPunch::DoElecChk(CString &sRst)
{
	BOOL bDone = FALSE;
#ifdef USE_FLUCK
	double dVal;
	if (!pDoc->WorkingInfo.Fluck.bUse)
		return TRUE;

	if (!pDoc->WorkingInfo.Probing[0].bUse)
		return TRUE;

	if (!m_pFluck || !m_pVoiceCoil[0] || !m_pVoiceCoil[1])
		return FALSE;

	switch (m_nStepElecChk)
	{
	case 0: // Move to left point for measuring.
		pDoc->m_sElecChk = _T("");
		m_dElecChkVal = 0.0;
		pDoc->WorkingInfo.Probing[0].nRepeatTest = 0;
		MoveMeasPos(0);
		m_nStepElecChk++;
		break;
	case 1: // Check move done
		if (m_pMotion->IsMotionDone(MS_X0))
			m_nStepElecChk++;
		break;
	case 2: // Move to right point for measuring.
		MoveMeasPos(1);
		m_nStepElecChk++;
		break;
	case 3: // Check move done
		if (m_pMotion->IsMotionDone(MS_X1))
			m_nStepElecChk++;
		break;
	case 4: // Delay
		m_nStepElecChk++;
		break;
	case 5: // left & right Prob down
		if (m_pVoiceCoil[0])
			m_pVoiceCoil[0]->SetProbing(0);
		m_nStepElecChk++;
		break;
	case 6: // Delay
		Sleep(100);
		m_nStepElecChk++;
		break;
	case 7: // left & right Prob down
		if (m_pVoiceCoil[1])
			m_pVoiceCoil[1]->SetProbing(1);
		m_nStepElecChk++;
		break;
	case 8: // Delay
		Sleep(30);
		m_nStepElecChk++;
		break;
	case 9: // Delay
		m_pFluck->Clear();
		Sleep(100);
		m_nStepElecChk++;
		break;
	case 10: // Delay
		m_nStepElecChk++;
		break;
	case 11: // Measure registance
		m_pFluck->Trig();
		m_nStepElecChk++;
		break;
	case 12: // Delay
		Sleep(100);
		if (m_pFluck->Get(dVal))
		{
			m_dElecChkVal += dVal;
			m_nStepElecChk++;
		}
		m_nStepElecChk++;
		break;
	case 13: // Measure registance
			 //m_pFluck->Clear();
		m_pFluck->Trig();
		m_nStepElecChk++;
		break;
	case 14: // Delay

		Sleep(100);
		if (m_pFluck->Get(dVal))
		{
			m_dElecChkVal += dVal;
			m_nStepElecChk++;
		}
		break;
	case 15: // Measure registance
			 //m_pFluck->Clear();
		m_pFluck->Trig();
		m_nStepElecChk++;
		break;
	case 16: // Delay
		Sleep(100);
		if (m_pFluck->Get(dVal))
		{
			m_dElecChkVal += dVal;
			m_nStepElecChk++;
		}
		break;
	case 17: // Result OK , NG
		dVal = m_dElecChkVal / 3.0;

		if (dVal >= FLUCK_OVER)
		{
			sRst.Format(_T("Open"), dVal);
			if (pDoc->WorkingInfo.Probing[0].nRepeatTest < _tstoi(pDoc->WorkingInfo.Probing[0].sRepeatTestNum))
			{
				pDoc->WorkingInfo.Probing[0].nRepeatTest++;
				// 				m_pVoiceCoil[0]->SearchHomeSmac0();
				// 				Sleep(100);
				// 				m_pVoiceCoil[1]->SearchHomeSmac1();
				Sleep(100);
				m_nStepElecChk = 4;
				break;
			}
		}
		else if (dVal == FLUCK_ERROR)
		{
			sRst.Format(_T("Error"), dVal);
			if (pDoc->WorkingInfo.Probing[0].nRepeatTest < _tstoi(pDoc->WorkingInfo.Probing[0].sRepeatTestNum))
			{
				pDoc->WorkingInfo.Probing[0].nRepeatTest++;
				// 				m_pVoiceCoil[0]->SearchHomeSmac0();
				// 				Sleep(100);
				// 				m_pVoiceCoil[1]->SearchHomeSmac1();
				Sleep(100);
				m_nStepElecChk = 4;
				break;
			}
		}
		else
		{
			sRst.Format(_T("%8f"), dVal);
		}

		pDoc->m_sElecChk = sRst;
		m_nStepElecChk++;
		break;
	case 18: // Write Data
		pDoc->WriteElecData(pDoc->m_sElecChk);
		m_nStepElecChk++;
		break;
	case 19: // left & right Prob up
			 //		m_pVoiceCoil[0]->MoveSmacShiftPos0();
		m_pVoiceCoil[1]->SearchHomeSmac1();
		m_nStepElecChk++;
		break;
	case 20: // Delay
		m_nStepElecChk++;
		break;
	case 21: // left & right Prob up
			 //		m_pVoiceCoil[1]->MoveSmacShiftPos1();
		m_pVoiceCoil[0]->SearchHomeSmac0();
		m_nStepElecChk++;
		break;
	case 22: // Delay
		m_nStepElecChk++;
		break;
	case 23: // Move to left init pos
			 //		if(m_pVoiceCoil[0]->IsDoneMoveSmacShiftPos0() && m_pVoiceCoil[1]->IsDoneMoveSmacShiftPos1())
		if (m_pVoiceCoil[0]->IsDoneSearchHomeSmac0() && m_pVoiceCoil[1]->IsDoneSearchHomeSmac1())
		{
			if (!IsInitPos0())
				MoveInitPos0();
			if (!IsInitPos1())
				MoveInitPos1();
			m_nStepElecChk++;
		}
		break;
	case 24: // Move to right init pos
		if (IsMoveDone0() && IsMoveDone1())
		{
			//Sleep(500);
			m_nStepElecChk++;
		}
		break;
	case 25: // Move to left init pos
		m_pVoiceCoil[1]->MoveSmacShiftPos1();
		m_nStepElecChk++;
		break;
	case 26: // Delay
		m_nStepElecChk++;
		break;
	case 27: // Move to left init pos
		m_pVoiceCoil[0]->MoveSmacShiftPos0();
		m_nStepElecChk++;
		break;
	case 28: // Delay
		m_nStepElecChk++;
		break;
	case 29: // Move to left init pos
		if (m_pVoiceCoil[0]->IsDoneMoveSmacShiftPos0() && m_pVoiceCoil[1]->IsDoneMoveSmacShiftPos1())
		{
			m_nStepElecChk++;
		}
		break;
	case 30: // Delay
			 //Sleep(100);
		m_nStepElecChk++;
		break;
	case 31: // Done Elec Check.
		bDone = TRUE;
		break;
	}

	sRst = pDoc->m_sElecChk;
#else
	bDone = TRUE;
#endif
	return bDone;
}

//===> PLC
void CManagerPunch::InitPLC()
{
#ifdef USE_MPE
	long lData;
	lData = (long)(_tstof(pDoc->WorkingInfo.Lot.sTotalReelDist) * 1000.0);
	MpeWrite(_T("ML45000"), lData);	// ��ü Reel ���� (���� M * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Lot.sSeparateDist) * 1000.0);
	MpeWrite(_T("ML45002"), lData);	// Lot �и� ���� (���� M * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Lot.sCuttingDist) * 1000.0);
	MpeWrite(_T("ML45004"), lData);	// Lot �и� �� ������ġ (���� M * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Lot.sStopDist) * 1000.0);
	MpeWrite(_T("ML45006"), lData);	// �Ͻ����� ���� (���� M * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sAoiTq) * 1000.0);
	MpeWrite(_T("ML45042"), lData);	// �˻�� Tension ���� ��ũ�� (���� Kgf * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sMkTq) * 1000.0);
	MpeWrite(_T("ML45044"), lData);	// ��ŷ�� Tension ���� ��ũ�� (���� Kgf * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sEngraveTq) * 1000.0);
	MpeWrite(_T("ML45050"), lData);	// ���κ� Tension ���� ��ũ�� (���� Kgf * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen) * 1000.0);
	MpeWrite(_T("ML45032"), lData);	// �� �ǳ� ���� (���� mm * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sMkFdVel) * 1000.0);
	MpeWrite(_T("ML45034"), lData);	// �� �ǳ� Feeding �ӵ� (���� mm/sec * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sMkJogVel) * 1000.0);
	MpeWrite(_T("ML45038"), lData);	// ���Ӱ��� �ӵ� (���� mm/sec * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sAoiJogAcc) * 1000.0);
	MpeWrite(_T("ML45040"), lData);	// ���Ӱ��� ���ӵ� (���� mm/s^2 * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sMkFdAcc) * 1000.0);
	MpeWrite(_T("ML45036"), lData);	// �� �ǳ� Feeding ���ӵ� (���� mm/s^2 * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sAoiFdLead) * 1000.0);
	MpeWrite(_T("ML45012"), lData);	// �˻�� Feeding �ѷ� Lead Pitch (���� mm * 1000)
	MpeWrite(_T("ML45020"), lData);	// ���κ� Feeding �ѷ� Lead Pitch (���� mm * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sMkFdLead) * 1000.0);
	MpeWrite(_T("ML45014"), lData);	// ��ŷ�� Feeding �ѷ� Lead Pitch (���� mm * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sStBufPos) * 1000.0);
	MpeWrite(_T("ML45016"), lData);	// ���� ���� ���� �ѷ� �ʱ���ġ(���� mm * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sFdMkAoiInitDist) * 1000.0);
	MpeWrite(_T("ML45008"), lData);	// AOI(��)���� ��ŷ���� �Ÿ� (���� mm * 1000)
	lData = (long)(_tstoi(pDoc->WorkingInfo.Motion.sFdAoiAoiDistShot) * 1000);
	MpeWrite(_T("ML45010"), lData);	// AOI(��)���� AOI(��) Shot�� (���� Shot�� * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sFdEngraveAoiInitDist) * 1000.0);
	MpeWrite(_T("ML45024"), lData);	// ���κο��� AOI(��)���� �Ÿ� (���� mm * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sFdBarcodeOffsetVel) * 1000.0);
	MpeWrite(_T("ML45060"), lData);	// 2D ���ڵ� ��������ġ���� Feeding �ӵ� (���� mm/sec * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sFdBarcodeOffsetAcc) * 1000.0);
	MpeWrite(_T("ML45062"), lData);	// 2D ���ڵ� ��������ġ���� Feeding ���ӵ� (���� mm/s^2 * 1000)

	lData = (long)(_tstof(pDoc->WorkingInfo.LastJob.sUltraSonicCleannerStTim) * 100.0);
	MpeWrite(_T("MW05940"), lData);	// AOI_Dn (���� [��] * 100) : 1 is 10 mSec.
	MpeWrite(_T("MW05942"), lData);	// AOI_Up (���� [��] * 100) : 1 is 10 mSec.
#endif
}

void CManagerPunch::InitIoWrite()
{
#ifdef USE_MPE
	if (!m_pMpe)
		return;

	//IoWrite(_T("MB44015E"), 0); // ����1 On  (PC�� ON, OFF) - 20141020
	MpeWrite(_T("MB44015E"), 0);
	//IoWrite(_T("MB44015F"), 0); // ����2 On  (PC�� ON, OFF) - 20141020
	MpeWrite(_T("MB44015F"), 0);

	//IoWrite(_T("MB003828"), 0); // �˻�� �� �˻� ���� <-> Y4368 I/F
	MpeWrite(_T("MB003828"), 0);
	//IoWrite(_T("MB003829"), 0); // �˻�� �� �˻� ���̺� ���� SOL <-> Y4369 I/F
	MpeWrite(_T("MB003829"), 0);
	//IoWrite(_T("MB00382A"), 0); // �˻�� �� Reset <-> Y436A I/F
	MpeWrite(_T("MB00382A"), 0);
	//IoWrite(_T("MB00382B"), 0); // ��ŷ�� Lot End <-> Y436B I/F
	MpeWrite(_T("MB00382B"), 0);

	//IoWrite(_T("MB003928"), 0); // �˻�� �� �˻� ���� <-> Y4468 I/F
	MpeWrite(_T("MB003928"), 0);
	//IoWrite(_T("MB003929"), 0); // �˻�� �� �˻� ���̺� ���� SOL <-> Y4369 I/F
	MpeWrite(_T("MB003929"), 0);
	//IoWrite(_T("MB00392A"), 0); // �˻�� �� Reset <-> Y436A I/F
	MpeWrite(_T("MB00392A"), 0);
	//IoWrite(_T("MB00392B"), 0); // ��ŷ�� Lot End <-> Y436B I/F
	MpeWrite(_T("MB00392B"), 0);

	//IoWrite(_T("MB44015D"), 0); // �ڵ� �ʱ� ��������(PC�� On/Off ��Ŵ, PLC�� ���������� ��ũ����, on->off�� �������� on, �ٽ� ��������ġ�� �������� off) - 20141017
	MpeWrite(_T("MB44015D"), 0);
	//IoWrite(_T("ML45064"), 0); // �˻�� Feeding �ѷ� Offset(*1000, +:�� ����, -�� ����, PC�� ���� PLC���� ����)
	MpeWrite(_T("ML45064"), 0);
	//IoWrite(_T("ML45066"), 0); // ��ŷ�� Feeding �ѷ� Offset(*1000, +:�� ����, -�� ����, PC�� ���� PLC���� ����)
	MpeWrite(_T("ML45066"), 0);

	//IoWrite(_T("MB600000"), 0); // PC�� PLC�� Alarm �߻����θ� Ȯ��
	MpeWrite(_T("MB600000"), 0);
	//IoWrite(_T("MB600008"), 0); // PC�� PLC�� Alarm �߻����θ� Ȯ��
	MpeWrite(_T("MB600008"), 0);
	//IoWrite(_T("ML60002"), 0); // �˶��� �߻��� ������ ����(PLC�� ǥ�� �� �˶��� �������� ����Ŵ).
	MpeWrite(_T("ML60002"), 0);
#endif
}

BOOL CManagerPunch::MpeWrite(CString strRegAddr, long lData, BOOL bCheck)
{
	if (m_pMpe)
		return MpeWrite(strRegAddr, lData, bCheck);
	else
		return FALSE;
}

void CManagerPunch::GetMpeIO()
{
#ifdef USE_MPE
	if (!m_pMpe || !pDoc->m_pMpeIo)
		return;

	m_pMpe->GetMpeIO();
#endif
}

void CManagerPunch::GetMpeSignal()
{
#ifdef USE_MPE
	if (!m_pMpe || !pDoc->m_pMpeSignal)
		return;
	m_pMpe->GetMpeSignal();
#endif
}

void CManagerPunch::GetMpeData()
{
#ifdef USE_MPE
	if (!m_pMpe || !pDoc->m_pMpeData)
		return;

	// MpeData
	int nSize, nIdx, nLoop, nSt, k;
	int nInSeg = pDoc->MkIo.MpeData.nInSeg;
	int nOutSeg = pDoc->MkIo.MpeData.nOutSeg;

	// MpeData - [In]
	int *pSeg0 = new int[nInSeg];
	long **pData0 = new long*[nInSeg];
	for (nIdx = 0; nIdx < nInSeg; nIdx++)
	{
		pData0[nIdx] = new long[16];
		memset(pData0[nIdx], 0, sizeof(long) * 16);
	}

	// Group Read...
	int nGrpStep = pDoc->MkIo.MpeData.nGrpStep;
	nSt = pDoc->MkIo.MpeData.nGrpInSt;
	for (nLoop = 0; nLoop < pDoc->MkIo.MpeData.nGrpIn; nLoop++)
	{
		for (nIdx = 0; nIdx < nGrpStep; nIdx++)
			pSeg0[nIdx] = nIdx + nSt;
		nSize = m_pMpe->ReadData(pSeg0, nGrpStep, pData0);

		for (nIdx = 0; nIdx < nGrpStep; nIdx++)
		{
			for (k = 0; k < 16; k++)
				pDoc->m_pMpeData[nIdx + nSt][k] = pData0[nIdx][k];
		}

		nSt += nGrpStep;
	}
	for (nIdx = 0; nIdx < nInSeg; nIdx++)
		delete[] pData0[nIdx];
	delete[] pData0;
	delete[] pSeg0;

	//pView->m_mgrProcedure->m_bChkMpeIoOut = FALSE;

	// MpeData - [Out]
	int *pSeg2 = new int[nOutSeg];
	long **pData2 = new long*[nOutSeg];
	for (nIdx = 0; nIdx < nOutSeg; nIdx++)
	{
		pData2[nIdx] = new long[16];
		memset(pData2[nIdx], 0, sizeof(unsigned long) * 16);
	}

	// Group Read...
	nSt = pDoc->MkIo.MpeData.nGrpOutSt;
	for (nLoop = 0; nLoop < pDoc->MkIo.MpeData.nGrpOut; nLoop++)
	{
		for (nIdx = 0; nIdx < nGrpStep; nIdx++)
			pSeg2[nIdx] = nIdx + nSt;
		nSize = m_pMpe->ReadData(pSeg2, nGrpStep, pData2);

		for (nIdx = 0; nIdx < nGrpStep; nIdx++)
		{
			for (k = 0; k < 16; k++)
				pDoc->m_pMpeData[nIdx + nSt][k] = pData2[nIdx][k];
		}

		nSt += nGrpStep;
	}
	for (nIdx = 0; nIdx < nOutSeg; nIdx++)
		delete[] pData2[nIdx];
	delete[] pData2;
	delete[] pSeg2;
#endif
}

void CManagerPunch::GetPlcParam()
{
	if (!m_pMpe)
		return;
#ifdef USE_MPE
	// Main
	pDoc->BtnStatus.Main.Ready = m_pMpe->Read(_T("MB005503")) ? TRUE : FALSE;	// ��ŷ�� �����غ� ����ġ
	pDoc->BtnStatus.Main.Run = m_pMpe->Read(_T("MB005501")) ? TRUE : FALSE;		// ��ŷ�� ���� ����ġ
	pDoc->BtnStatus.Main.Reset = m_pMpe->Read(_T("MB005504")) ? TRUE : FALSE;	// ��ŷ�� ���� ����ġ
	pDoc->BtnStatus.Main.Stop = m_pMpe->Read(_T("MB005502")) ? TRUE : FALSE;	// ��ŷ�� ���� ����ġ
																				//pDoc->BtnStatus.Main.Auto = m_pMpe->Read(_T("MB005505")) ? TRUE : FALSE;	// ��ŷ�� �ڵ�/���� (ON)
																				//pDoc->BtnStatus.Main.Manual = m_pMpe->Read(_T("MB005505")) ? FALSE : TRUE;	// ��ŷ�� �ڵ�/���� (OFF)
	pDoc->BtnStatus.Main.Auto = m_pMpe->Read(_T("MB003705")) ? TRUE : FALSE;	// ��ŷ�� �ڵ� ���� ����ġ ����
	pDoc->BtnStatus.Main.Manual = m_pMpe->Read(_T("MB003705")) ? FALSE : TRUE;	// ��ŷ�� �ڵ� ���� ����ġ ����

	if (pDoc->BtnStatus.Main.PrevReady != pDoc->BtnStatus.Main.Ready)
	{
		pDoc->BtnStatus.Main.PrevReady = pDoc->BtnStatus.Main.Ready;
		pDoc->SetMkMenu03(_T("Main"), _T("Ready"), pDoc->BtnStatus.Main.Ready);
	}
	if (pDoc->BtnStatus.Main.PrevRun != pDoc->BtnStatus.Main.Run)
	{
		pDoc->BtnStatus.Main.PrevRun = pDoc->BtnStatus.Main.Run;
		pDoc->SetMkMenu03(_T("Main"), _T("Run"), pDoc->BtnStatus.Main.Run);
		pDoc->SetMkMenu03(_T("Main"), _T("Stop"), !pDoc->BtnStatus.Main.Run);
	}
	if (pDoc->BtnStatus.Main.PrevReset != pDoc->BtnStatus.Main.Reset)
	{
		pDoc->BtnStatus.Main.PrevReset = pDoc->BtnStatus.Main.Reset;
		pDoc->SetMkMenu03(_T("Main"), _T("Reset"), pDoc->BtnStatus.Main.Reset);
	}
	if (pDoc->BtnStatus.Main.PrevStop != pDoc->BtnStatus.Main.Stop)
	{
		pDoc->BtnStatus.Main.PrevStop = pDoc->BtnStatus.Main.Stop;
		pDoc->SetMkMenu03(_T("Main"), _T("Stop"), pDoc->BtnStatus.Main.Stop);
		pDoc->SetMkMenu03(_T("Main"), _T("Run"), !pDoc->BtnStatus.Main.Stop);
	}
	if (pDoc->BtnStatus.Main.PrevAuto != pDoc->BtnStatus.Main.Auto)
	{
		pDoc->BtnStatus.Main.PrevAuto = pDoc->BtnStatus.Main.Auto;
		pDoc->SetMkMenu03(_T("Main"), _T("Auto"), pDoc->BtnStatus.Main.Auto);
		pDoc->SetMkMenu03(_T("Main"), _T("Manual"), !pDoc->BtnStatus.Main.Auto);
	}
	if (pDoc->BtnStatus.Main.PrevManual != pDoc->BtnStatus.Main.Manual)
	{
		pDoc->BtnStatus.Main.PrevManual = pDoc->BtnStatus.Main.Manual;
		pDoc->SetMkMenu03(_T("Main"), _T("Manual"), pDoc->BtnStatus.Main.Manual);
		pDoc->SetMkMenu03(_T("Main"), _T("Auto"), !pDoc->BtnStatus.Main.Manual);
	}

	// TorqueMotor
	pDoc->BtnStatus.Tq.Mk = m_pMpe->Read(_T("MB440155")) ? TRUE : FALSE;
	pDoc->BtnStatus.Tq.Aoi = m_pMpe->Read(_T("MB440156")) ? TRUE : FALSE;
	pDoc->BtnStatus.Tq.Eng = m_pMpe->Read(_T("MB440154")) ? TRUE : FALSE;

	if (pDoc->BtnStatus.Tq.PrevMk != pDoc->BtnStatus.Tq.Mk)
	{
		pDoc->BtnStatus.Tq.PrevMk = pDoc->BtnStatus.Tq.Mk;
		pDoc->SetMkMenu03(_T("TqMotor"), _T("MkTq"), pDoc->BtnStatus.Tq.Mk);
	}
	if (pDoc->BtnStatus.Tq.PrevAoi != pDoc->BtnStatus.Tq.Aoi)
	{
		pDoc->BtnStatus.Tq.PrevAoi = pDoc->BtnStatus.Tq.Aoi;
		pDoc->SetMkMenu03(_T("TqMotor"), _T("AoiTq"), pDoc->BtnStatus.Tq.Aoi);
	}
	if (pDoc->BtnStatus.Tq.PrevEng != pDoc->BtnStatus.Tq.Eng)
	{
		pDoc->BtnStatus.Tq.PrevEng = pDoc->BtnStatus.Tq.Eng;
		pDoc->SetMkMenu03(_T("TqMotor"), _T("EngTq"), pDoc->BtnStatus.Tq.Eng);
	}

	// InductionMotor
	pDoc->BtnStatus.Induct.Rc = m_pMpe->Read(_T("MB44017D")) ? TRUE : FALSE;	//pView->SetTwoMetal(FALSE, TRUE);	// One Metal IDC_CHK_68		
	pDoc->BtnStatus.Induct.Uc = m_pMpe->Read(_T("MB44017C")) ? TRUE : FALSE;	//pView->SetTwoMetal(TRUE, TRUE);	// Two Metal IDC_CHK_69

	if (pDoc->BtnStatus.Induct.PrevRc != pDoc->BtnStatus.Induct.Rc)
	{
		pDoc->BtnStatus.Induct.PrevRc = pDoc->BtnStatus.Induct.Rc;
		pDoc->SetMkMenu03(_T("Induction"), _T("RcCcw"), pDoc->BtnStatus.Induct.Rc);
	}
	if (pDoc->BtnStatus.Induct.PrevUc != pDoc->BtnStatus.Induct.Uc)
	{
		pDoc->BtnStatus.Induct.PrevUc = pDoc->BtnStatus.Induct.Uc;
		pDoc->SetMkMenu03(_T("Induction"), _T("UcCcw"), pDoc->BtnStatus.Induct.Uc);
	}

	// Core150mm
	pDoc->BtnStatus.Core150.Rc = m_pMpe->Read(_T("MB44017E")) ? TRUE : FALSE;	// SetCore150mmRecoiler(TRUE);	// Recoiler IDC_CHK_70	
	pDoc->BtnStatus.Core150.Uc = m_pMpe->Read(_T("MB44017F")) ? TRUE : FALSE;	// SetCore150mmUncoiler(TRUE);	// Uncoiler IDC_CHK_71	

	if (pDoc->BtnStatus.Core150.PrevRc != pDoc->BtnStatus.Core150.Rc)
	{
		pDoc->BtnStatus.Core150.PrevRc = pDoc->BtnStatus.Core150.Rc;
		pDoc->SetMkMenu03(_T("Core"), _T("Rc150"), pDoc->BtnStatus.Core150.Rc);
	}
	if (pDoc->BtnStatus.Core150.PrevUc != pDoc->BtnStatus.Core150.Uc)
	{
		pDoc->BtnStatus.Core150.PrevUc = pDoc->BtnStatus.Core150.Uc;
		pDoc->SetMkMenu03(_T("Core"), _T("Uc150"), pDoc->BtnStatus.Core150.Uc);
	}

	// Etc
	//pDoc->BtnStatus.Etc.EmgAoi = m_pMpe->Read(_T("")) ? TRUE : FALSE;

	// Recoiler
	pDoc->BtnStatus.Rc.Relation = m_pMpe->Read(_T("MB004001")) ? TRUE : FALSE;		// �����Ϸ� ���� ��/���� ����ġ ���� 
	pDoc->BtnStatus.Rc.FdCw = m_pMpe->Read(_T("MB00400C")) ? TRUE : FALSE;			// �����Ϸ� ��ǰ�� ��ȸ�� ����ġ ����
	pDoc->BtnStatus.Rc.FdCcw = m_pMpe->Read(_T("MB00400D")) ? TRUE : FALSE;			// �����Ϸ� ��ǰ�� ��ȸ�� ����ġ ����
	pDoc->BtnStatus.Rc.ReelChuck = m_pMpe->Read(_T("MB00400B")) ? TRUE : FALSE;		// �����Ϸ� ��ǰô Ŭ���� ����ġ ����
	pDoc->BtnStatus.Rc.DcRlUpDn = m_pMpe->Read(_T("MB004002")) ? TRUE : FALSE;		// �����Ϸ� ���� ���/�ϰ� ����ġ ����
	pDoc->BtnStatus.Rc.ReelJoinL = m_pMpe->Read(_T("MB004005")) ? TRUE : FALSE;		// �����Ϸ� ��ǰ ������(��/��) ����ġ ����
	pDoc->BtnStatus.Rc.ReelJoinR = m_pMpe->Read(_T("MB004006")) ? TRUE : FALSE;		// �����Ϸ� ��ǰ ������(��/��) ����ġ ����
	pDoc->BtnStatus.Rc.ReelJoinVac = m_pMpe->Read(_T("MB00400F")) ? TRUE : FALSE;	// �����Ϸ� ��ǰ ������ ���� ����ġ ����
	pDoc->BtnStatus.Rc.PprChuck = m_pMpe->Read(_T("MB004008")) ? TRUE : FALSE;		// �����Ϸ� ����ô Ŭ���� ����ġ ����
	pDoc->BtnStatus.Rc.PprCw = m_pMpe->Read(_T("MB004009")) ? TRUE : FALSE;			// �����Ϸ� ������ ��ȸ�� ����ġ ����
	pDoc->BtnStatus.Rc.PprCcw = m_pMpe->Read(_T("MB00400A")) ? TRUE : FALSE;		// �����Ϸ� ������ ��ȸ�� ����ġ ����
	pDoc->BtnStatus.Rc.Rewine = m_pMpe->Read(_T("MB004003")) ? TRUE : FALSE;		// �����Ϸ� Rewinder ���� ����ġ ����
	pDoc->BtnStatus.Rc.RewineReelPpr = m_pMpe->Read(_T("MB004004")) ? TRUE : FALSE;	// �����Ϸ� Rewinder ��ǰ & ���� ����ġ ����

	if (pDoc->BtnStatus.Rc.PrevRelation != pDoc->BtnStatus.Rc.Relation)
	{
		pDoc->BtnStatus.Rc.PrevRelation = pDoc->BtnStatus.Rc.Relation;
		pDoc->SetMkMenu03(_T("Main"), _T("Relation"), pDoc->BtnStatus.Rc.Relation);
	}
	if (pDoc->BtnStatus.Rc.PrevFdCw != pDoc->BtnStatus.Rc.FdCw)
	{
		pDoc->BtnStatus.Rc.PrevFdCw = pDoc->BtnStatus.Rc.FdCw;
		pDoc->SetMkMenu03(_T("Recoiler"), _T("MvCw"), pDoc->BtnStatus.Rc.FdCw);
	}
	if (pDoc->BtnStatus.Rc.PrevFdCcw != pDoc->BtnStatus.Rc.FdCcw)
	{
		pDoc->BtnStatus.Rc.PrevFdCcw = pDoc->BtnStatus.Rc.FdCcw;
		pDoc->SetMkMenu03(_T("Recoiler"), _T("MvCcw"), pDoc->BtnStatus.Rc.FdCcw);
	}
	if (pDoc->BtnStatus.Rc.PrevReelChuck != pDoc->BtnStatus.Rc.ReelChuck)
	{
		pDoc->BtnStatus.Rc.PrevReelChuck = pDoc->BtnStatus.Rc.ReelChuck;
		pDoc->SetMkMenu03(_T("Recoiler"), _T("PrdChuck"), pDoc->BtnStatus.Rc.ReelChuck);
	}
	if (pDoc->BtnStatus.Rc.PrevDcRlUpDn != pDoc->BtnStatus.Rc.DcRlUpDn)
	{
		pDoc->BtnStatus.Rc.PrevDcRlUpDn = pDoc->BtnStatus.Rc.DcRlUpDn;
		pDoc->SetMkMenu03(_T("Recoiler"), _T("DancerUp"), pDoc->BtnStatus.Rc.DcRlUpDn);
	}
	if (pDoc->BtnStatus.Rc.PrevReelJoinL != pDoc->BtnStatus.Rc.ReelJoinL)
	{
		pDoc->BtnStatus.Rc.PrevReelJoinL = pDoc->BtnStatus.Rc.ReelJoinL;
		pDoc->SetMkMenu03(_T("Recoiler"), _T("PasteUpLf"), pDoc->BtnStatus.Rc.ReelJoinL);
	}
	if (pDoc->BtnStatus.Rc.PrevReelJoinR != pDoc->BtnStatus.Rc.ReelJoinR)
	{
		pDoc->BtnStatus.Rc.PrevReelJoinR = pDoc->BtnStatus.Rc.ReelJoinR;
		pDoc->SetMkMenu03(_T("Recoiler"), _T("PasteUpRt"), pDoc->BtnStatus.Rc.ReelJoinR);
	}
	if (pDoc->BtnStatus.Rc.PrevReelJoinVac != pDoc->BtnStatus.Rc.ReelJoinVac)
	{
		pDoc->BtnStatus.Rc.PrevReelJoinVac = pDoc->BtnStatus.Rc.ReelJoinVac;
		pDoc->SetMkMenu03(_T("Recoiler"), _T("PasteVac"), pDoc->BtnStatus.Rc.ReelJoinVac);
	}
	if (pDoc->BtnStatus.Rc.PrevPprChuck != pDoc->BtnStatus.Rc.PprChuck)
	{
		pDoc->BtnStatus.Rc.PrevPprChuck = pDoc->BtnStatus.Rc.PprChuck;
		pDoc->SetMkMenu03(_T("Recoiler"), _T("PprChuck"), pDoc->BtnStatus.Rc.PprChuck);
	}
	if (pDoc->BtnStatus.Rc.PrevPprCw != pDoc->BtnStatus.Rc.PprCw)
	{
		pDoc->BtnStatus.Rc.PrevPprCw = pDoc->BtnStatus.Rc.PprCw;
		pDoc->SetMkMenu03(_T("Recoiler"), _T("PprCw"), pDoc->BtnStatus.Rc.PprCw);
	}
	if (pDoc->BtnStatus.Rc.PrevPprCcw != pDoc->BtnStatus.Rc.PprCcw)
	{
		pDoc->BtnStatus.Rc.PrevPprCcw = pDoc->BtnStatus.Rc.PprCcw;
		pDoc->SetMkMenu03(_T("Recoiler"), _T("PprCcw"), pDoc->BtnStatus.Rc.PprCcw);
	}
	if (pDoc->BtnStatus.Rc.PrevRewine != pDoc->BtnStatus.Rc.Rewine)
	{
		pDoc->BtnStatus.Rc.PrevRewine = pDoc->BtnStatus.Rc.Rewine;
		pDoc->SetMkMenu03(_T("Recoiler"), _T("DoRewind"), pDoc->BtnStatus.Rc.Rewine);
	}
	if (pDoc->BtnStatus.Rc.PrevRewineReelPpr != pDoc->BtnStatus.Rc.RewineReelPpr)
	{
		pDoc->BtnStatus.Rc.PrevRewineReelPpr = pDoc->BtnStatus.Rc.RewineReelPpr;
		pDoc->SetMkMenu03(_T("Recoiler"), _T("PrdPprRewind"), pDoc->BtnStatus.Rc.RewineReelPpr);
	}

	//pDoc->BtnStatus.Rc.Relation = m_pMpe->Read(_T("MB005801")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Rc.FdCw = m_pMpe->Read(_T("MB00580C")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Rc.FdCcw = m_pMpe->Read(_T("MB00580D")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Rc.ReelChuck = m_pMpe->Read(_T("MB00580B")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Rc.DcRlUpDn = m_pMpe->Read(_T("MB005802")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Rc.ReelJoinL = m_pMpe->Read(_T("MB005805")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Rc.ReelJoinR = m_pMpe->Read(_T("MB005806")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Rc.ReelJoinVac = m_pMpe->Read(_T("MB00580F")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Rc.PprChuck = m_pMpe->Read(_T("MB005808")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Rc.PprCw = m_pMpe->Read(_T("MB005809")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Rc.PprCcw = m_pMpe->Read(_T("MB00580A")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Rc.Rewine = m_pMpe->Read(_T("MB005803")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Rc.RewineReelPpr = m_pMpe->Read(_T("MB005804")) ? TRUE : FALSE;

	// Punch
	pDoc->BtnStatus.Mk.Relation = m_pMpe->Read(_T("MB003711")) ? TRUE : FALSE;	// ��ŷ�� ���� ��/���� ����ġ ����
	pDoc->BtnStatus.Mk.FdCw = m_pMpe->Read(_T("MB003713")) ? TRUE : FALSE;		// ��ŷ�� �ǵ� ��ȸ�� ����ġ ����
	pDoc->BtnStatus.Mk.FdCcw = m_pMpe->Read(_T("MB003714")) ? TRUE : FALSE;		// ��ŷ�� �ǵ� ��ȸ�� ����ġ ����
	pDoc->BtnStatus.Mk.FdVac = m_pMpe->Read(_T("MB003715")) ? TRUE : FALSE;		// ��ŷ�� �ǵ� ���� ����ġ ����
	pDoc->BtnStatus.Mk.PushUp = m_pMpe->Read(_T("MB003712")) ? TRUE : FALSE;	// ��ŷ�� ���̺� ��ο� ����ġ ����
	pDoc->BtnStatus.Mk.TblBlw = m_pMpe->Read(_T("MB003717")) ? TRUE : FALSE;	// ��ŷ�� ���̺� ���� ����ġ ����
	pDoc->BtnStatus.Mk.TblVac = m_pMpe->Read(_T("MB003716")) ? TRUE : FALSE;	// ��ŷ�� ��ũ ���� ����ġ ����
	pDoc->BtnStatus.Mk.FdClp = m_pMpe->Read(_T("MB003719")) ? TRUE : FALSE;		// ��ŷ�� �ǵ� Ŭ���� ����ġ ����  
	pDoc->BtnStatus.Mk.TqClp = m_pMpe->Read(_T("MB00371A")) ? TRUE : FALSE;		// ��ŷ�� �ټ� Ŭ���� ����ġ ����
	pDoc->BtnStatus.Mk.MvOne = m_pMpe->Read(_T("MB440151")) ? TRUE : FALSE;
	pDoc->BtnStatus.Mk.LsrPt = m_pMpe->Read(_T("MB003718")) ? TRUE : FALSE;		// ��ŷ�� ������ ������ ����ġ ����
	pDoc->BtnStatus.Mk.DcRSol = m_pMpe->Read(_T("MB00371B")) ? TRUE : FALSE;	// ��ŷ�� ���� ���/�ϰ� ����ġ ����

	if (pDoc->BtnStatus.Mk.PrevRelation != pDoc->BtnStatus.Mk.Relation)
	{
		pDoc->BtnStatus.Mk.PrevRelation = pDoc->BtnStatus.Mk.Relation;
		pDoc->SetMkMenu03(_T("Main"), _T("Relation"), pDoc->BtnStatus.Mk.Relation);
	}
	if (pDoc->BtnStatus.Mk.PrevFdCw != pDoc->BtnStatus.Mk.FdCw)
	{
		pDoc->BtnStatus.Mk.PrevFdCw = pDoc->BtnStatus.Mk.FdCw;
		pDoc->SetMkMenu03(_T("Punching"), _T("MvCw"), pDoc->BtnStatus.Mk.FdCw);
	}
	if (pDoc->BtnStatus.Mk.PrevFdCcw != pDoc->BtnStatus.Mk.FdCcw)
	{
		pDoc->BtnStatus.Mk.PrevFdCcw = pDoc->BtnStatus.Mk.FdCcw;
		pDoc->SetMkMenu03(_T("Punching"), _T("MvCcw"), pDoc->BtnStatus.Mk.FdCcw);
	}
	if (pDoc->BtnStatus.Mk.PrevFdVac != pDoc->BtnStatus.Mk.FdVac)
	{
		pDoc->BtnStatus.Mk.PrevFdVac = pDoc->BtnStatus.Mk.FdVac;
		pDoc->SetMkMenu03(_T("Punching"), _T("FdVac"), pDoc->BtnStatus.Mk.FdVac);
	}
	if (pDoc->BtnStatus.Mk.PrevPushUp != pDoc->BtnStatus.Mk.PushUp)
	{
		pDoc->BtnStatus.Mk.PrevPushUp = pDoc->BtnStatus.Mk.PushUp;
		pDoc->SetMkMenu03(_T("Punching"), _T("PushUp"), pDoc->BtnStatus.Mk.PushUp);
	}
	if (pDoc->BtnStatus.Mk.PrevTblBlw != pDoc->BtnStatus.Mk.TblBlw)
	{
		pDoc->BtnStatus.Mk.PrevTblBlw = pDoc->BtnStatus.Mk.TblBlw;
		pDoc->SetMkMenu03(_T("Punching"), _T("TblBlw"), pDoc->BtnStatus.Mk.TblBlw);
	}
	if (pDoc->BtnStatus.Mk.PrevTblVac != pDoc->BtnStatus.Mk.TblVac)
	{
		pDoc->BtnStatus.Mk.PrevTblVac = pDoc->BtnStatus.Mk.TblVac;
		pDoc->SetMkMenu03(_T("Punching"), _T("TblVac"), pDoc->BtnStatus.Mk.TblVac);
	}
	if (pDoc->BtnStatus.Mk.PrevFdClp != pDoc->BtnStatus.Mk.FdClp)
	{
		pDoc->BtnStatus.Mk.PrevFdClp = pDoc->BtnStatus.Mk.FdClp;
		pDoc->SetMkMenu03(_T("Punching"), _T("FdClamp"), pDoc->BtnStatus.Mk.FdClp);
	}
	if (pDoc->BtnStatus.Mk.PrevTqClp != pDoc->BtnStatus.Mk.TqClp)
	{
		pDoc->BtnStatus.Mk.PrevTqClp = pDoc->BtnStatus.Mk.TqClp;
		pDoc->SetMkMenu03(_T("Punching"), _T("TensClamp"), pDoc->BtnStatus.Mk.TqClp);
	}
	if (pDoc->BtnStatus.Mk.PrevMvOne != pDoc->BtnStatus.Mk.MvOne)
	{
		pDoc->BtnStatus.Mk.PrevMvOne = pDoc->BtnStatus.Mk.MvOne;
		pDoc->SetMkMenu03(_T("Punching"), _T("OnePnl"), pDoc->BtnStatus.Mk.MvOne);
	}
	if (pDoc->BtnStatus.Mk.PrevLsrPt != pDoc->BtnStatus.Mk.LsrPt)
	{
		pDoc->BtnStatus.Mk.PrevLsrPt = pDoc->BtnStatus.Mk.LsrPt;
		pDoc->SetMkMenu03(_T("Punching"), _T("Lsr"), pDoc->BtnStatus.Mk.LsrPt);
	}
	if (pDoc->BtnStatus.Mk.PrevDcRSol != pDoc->BtnStatus.Mk.DcRSol)
	{
		pDoc->BtnStatus.Mk.PrevDcRSol = pDoc->BtnStatus.Mk.DcRSol;
		pDoc->SetMkMenu03(_T("Punching"), _T("DancerUp"), pDoc->BtnStatus.Mk.DcRSol);
	}

	//pDoc->BtnStatus.Mk.Relation = m_pMpe->Read(_T("MB005511")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Mk.FdCw = m_pMpe->Read(_T("MB005513")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Mk.FdCcw = m_pMpe->Read(_T("MB005514")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Mk.FdVac = m_pMpe->Read(_T("MB005515")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Mk.PushUp = m_pMpe->Read(_T("MB005516")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Mk.TblBlw = m_pMpe->Read(_T("MB005512")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Mk.TblVac = m_pMpe->Read(_T("MB005517")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Mk.FdClp = m_pMpe->Read(_T("MB005519")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Mk.TqClp = m_pMpe->Read(_T("MB00551A")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Mk.MvOne = m_pMpe->Read(_T("MB440151")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Mk.LsrPt = m_pMpe->Read(_T("MB005518")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Mk.DcRSol = m_pMpe->Read(_T("MB00551B")) ? TRUE : FALSE;

	// AOIDn
	pDoc->BtnStatus.AoiDn.Relation = m_pMpe->Read(_T("MB003901")) ? TRUE : FALSE;	// �˻�� �� ���� ��/���� ����ġ ����
	pDoc->BtnStatus.AoiDn.FdCw = m_pMpe->Read(_T("MB003903")) ? TRUE : FALSE;		// �˻�� �� �ǵ� ��ȸ�� ����ġ ����
	pDoc->BtnStatus.AoiDn.FdCcw = m_pMpe->Read(_T("MB003904")) ? TRUE : FALSE;		// �˻�� �� �ǵ� ��ȸ�� ����ġ ����
	pDoc->BtnStatus.AoiDn.FdVac = m_pMpe->Read(_T("MB003905")) ? TRUE : FALSE;		// �˻�� �� �ǵ� ���� ����ġ ����
	pDoc->BtnStatus.AoiDn.PushUp = m_pMpe->Read(_T("MB003906")) ? TRUE : FALSE;		// �˻�� �� ��ũ ���� ����ġ ����
	pDoc->BtnStatus.AoiDn.TblBlw = m_pMpe->Read(_T("MB003902")) ? TRUE : FALSE;		// �˻�� �� ���̺� ��ο� ����ġ ����
	pDoc->BtnStatus.AoiDn.TblVac = m_pMpe->Read(_T("MB003907")) ? TRUE : FALSE;		// �˻�� �� ���̺� ���� ����ġ ����
	pDoc->BtnStatus.AoiDn.FdClp = m_pMpe->Read(_T("MB003909")) ? TRUE : FALSE;		// �˻�� �� �ǵ� Ŭ���� ����ġ ����
	pDoc->BtnStatus.AoiDn.TqClp = m_pMpe->Read(_T("MB00390A")) ? TRUE : FALSE;		// �˻�� �� �ټ� Ŭ���� ����ġ ����
	pDoc->BtnStatus.AoiDn.MvOne = m_pMpe->Read(_T("MB440151")) ? TRUE : FALSE;
	pDoc->BtnStatus.AoiDn.LsrPt = m_pMpe->Read(_T("MB003908")) ? TRUE : FALSE;		// �˻�� �� ������ ������ ����ġ ����
	pDoc->BtnStatus.AoiDn.VelSonicBlw = m_pMpe->Read(_T("MB44014F")) ? TRUE : FALSE;

	if (pDoc->BtnStatus.AoiDn.PrevRelation != pDoc->BtnStatus.AoiDn.Relation)
	{
		pDoc->BtnStatus.AoiDn.PrevRelation = pDoc->BtnStatus.AoiDn.Relation;
		pDoc->SetMkMenu03(_T("Main"), _T("Relation"), pDoc->BtnStatus.AoiDn.Relation);
	}
	if (pDoc->BtnStatus.AoiDn.PrevFdCw != pDoc->BtnStatus.AoiDn.FdCw)
	{
		pDoc->BtnStatus.AoiDn.PrevFdCw = pDoc->BtnStatus.AoiDn.FdCw;
		pDoc->SetMkMenu03(_T("AoiDn"), _T("MvCw"), pDoc->BtnStatus.AoiDn.FdCw);
	}
	if (pDoc->BtnStatus.AoiDn.PrevFdCcw != pDoc->BtnStatus.AoiDn.FdCcw)
	{
		pDoc->BtnStatus.AoiDn.PrevFdCcw = pDoc->BtnStatus.AoiDn.FdCcw;
		pDoc->SetMkMenu03(_T("AoiDn"), _T("MvCcw"), pDoc->BtnStatus.AoiDn.FdCcw);
	}
	if (pDoc->BtnStatus.AoiDn.PrevFdVac != pDoc->BtnStatus.AoiDn.FdVac)
	{
		pDoc->BtnStatus.AoiDn.PrevFdVac = pDoc->BtnStatus.AoiDn.FdVac;
		pDoc->SetMkMenu03(_T("AoiDn"), _T("FdVac"), pDoc->BtnStatus.AoiDn.FdVac);
	}
	if (pDoc->BtnStatus.AoiDn.PrevPushUp != pDoc->BtnStatus.AoiDn.PushUp)
	{
		pDoc->BtnStatus.AoiDn.PrevPushUp = pDoc->BtnStatus.AoiDn.PushUp;
		pDoc->SetMkMenu03(_T("AoiDn"), _T("PushUp"), pDoc->BtnStatus.AoiDn.PushUp);
	}
	if (pDoc->BtnStatus.AoiDn.PrevTblBlw != pDoc->BtnStatus.AoiDn.TblBlw)
	{
		pDoc->BtnStatus.AoiDn.PrevTblBlw = pDoc->BtnStatus.AoiDn.TblBlw;
		pDoc->SetMkMenu03(_T("AoiDn"), _T("TblBlw"), pDoc->BtnStatus.AoiDn.TblBlw);
	}
	if (pDoc->BtnStatus.AoiDn.PrevTblVac != pDoc->BtnStatus.AoiDn.TblVac)
	{
		pDoc->BtnStatus.AoiDn.PrevTblVac = pDoc->BtnStatus.AoiDn.TblVac;
		pDoc->SetMkMenu03(_T("AoiDn"), _T("TblVac"), pDoc->BtnStatus.AoiDn.TblVac);
	}
	if (pDoc->BtnStatus.AoiDn.PrevFdClp != pDoc->BtnStatus.AoiDn.FdClp)
	{
		pDoc->BtnStatus.AoiDn.PrevFdClp = pDoc->BtnStatus.AoiDn.FdClp;
		pDoc->SetMkMenu03(_T("AoiDn"), _T("FdClamp"), pDoc->BtnStatus.AoiDn.FdClp);
	}
	if (pDoc->BtnStatus.AoiDn.PrevTqClp != pDoc->BtnStatus.AoiDn.TqClp)
	{
		pDoc->BtnStatus.AoiDn.PrevTqClp = pDoc->BtnStatus.AoiDn.TqClp;
		pDoc->SetMkMenu03(_T("AoiDn"), _T("TensClamp"), pDoc->BtnStatus.AoiDn.TqClp);
	}
	if (pDoc->BtnStatus.AoiDn.PrevMvOne != pDoc->BtnStatus.AoiDn.MvOne)
	{
		pDoc->BtnStatus.AoiDn.PrevMvOne = pDoc->BtnStatus.AoiDn.MvOne;
		pDoc->SetMkMenu03(_T("AoiDn"), _T("OnePnl"), pDoc->BtnStatus.AoiDn.MvOne);
	}
	if (pDoc->BtnStatus.AoiDn.PrevLsrPt != pDoc->BtnStatus.AoiDn.LsrPt)
	{
		pDoc->BtnStatus.AoiDn.PrevLsrPt = pDoc->BtnStatus.AoiDn.LsrPt;
		pDoc->SetMkMenu03(_T("AoiDn"), _T("Lsr"), pDoc->BtnStatus.AoiDn.LsrPt);
	}
	if (pDoc->BtnStatus.AoiDn.PrevVelSonicBlw != pDoc->BtnStatus.AoiDn.VelSonicBlw)
	{
		pDoc->BtnStatus.AoiDn.PrevVelSonicBlw = pDoc->BtnStatus.AoiDn.VelSonicBlw;
		pDoc->SetMkMenu03(_T("AoiDn"), _T("VelClrSonic"), pDoc->BtnStatus.AoiDn.VelSonicBlw);
	}

	//pDoc->BtnStatus.AoiDn.Relation = m_pMpe->Read(_T("MB005701")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiDn.FdCw = m_pMpe->Read(_T("MB005703")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiDn.FdCcw = m_pMpe->Read(_T("MB005704")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiDn.FdVac = m_pMpe->Read(_T("MB005705")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiDn.PushUp = m_pMpe->Read(_T("MB005706")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiDn.TblBlw = m_pMpe->Read(_T("MB005702")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiDn.TblVac = m_pMpe->Read(_T("MB005707")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiDn.FdClp = m_pMpe->Read(_T("MB005709")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiDn.TqClp = m_pMpe->Read(_T("MB00570A")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiDn.MvOne = m_pMpe->Read(_T("MB440151")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiDn.LsrPt = m_pMpe->Read(_T("MB005708")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiDn.VelSonicBlw = m_pMpe->Read(_T("MB44014F")) ? TRUE : FALSE;

	// AOIUp
	pDoc->BtnStatus.AoiUp.Relation = m_pMpe->Read(_T("MB003801")) ? TRUE : FALSE;	// �˻�� �� ���� ��/���� ����ġ ����
	pDoc->BtnStatus.AoiUp.FdCw = m_pMpe->Read(_T("MB003803")) ? TRUE : FALSE;		// �˻�� �� �ǵ� ��ȸ�� ����ġ ����
	pDoc->BtnStatus.AoiUp.FdCcw = m_pMpe->Read(_T("MB003804")) ? TRUE : FALSE;		// �˻�� �� �ǵ� ��ȸ�� ����ġ ����
	pDoc->BtnStatus.AoiUp.FdVac = m_pMpe->Read(_T("MB003805")) ? TRUE : FALSE;		// �˻�� �� �ǵ� ���� ����ġ ����
	pDoc->BtnStatus.AoiUp.PushUp = m_pMpe->Read(_T("MB003806")) ? TRUE : FALSE;		// �˻�� �� ��ũ ���� ����ġ ����
	pDoc->BtnStatus.AoiUp.TblBlw = m_pMpe->Read(_T("MB003802")) ? TRUE : FALSE;		// �˻�� �� ���̺� ��ο� ����ġ ����
	pDoc->BtnStatus.AoiUp.TblVac = m_pMpe->Read(_T("MB003807")) ? TRUE : FALSE;		// �˻�� �� ���̺� ���� ����ġ ����
	pDoc->BtnStatus.AoiUp.FdClp = m_pMpe->Read(_T("MB003809")) ? TRUE : FALSE;		// �˻�� �� �ǵ� Ŭ���� ����ġ ����
	pDoc->BtnStatus.AoiUp.TqClp = m_pMpe->Read(_T("MB00380A")) ? TRUE : FALSE;		// �˻�� �� �ټ� Ŭ���� ����ġ ����
	pDoc->BtnStatus.AoiUp.MvOne = m_pMpe->Read(_T("MB440151")) ? TRUE : FALSE;
	pDoc->BtnStatus.AoiUp.LsrPt = m_pMpe->Read(_T("MB003808")) ? TRUE : FALSE;		// �˻�� �� ������ ������ ����ġ ����

	if (pDoc->BtnStatus.AoiUp.PrevRelation != pDoc->BtnStatus.AoiUp.Relation)
	{
		pDoc->BtnStatus.AoiUp.PrevRelation = pDoc->BtnStatus.AoiUp.Relation;
		pDoc->SetMkMenu03(_T("Main"), _T("Relation"), pDoc->BtnStatus.AoiUp.Relation);
	}
	if (pDoc->BtnStatus.AoiUp.PrevFdCw != pDoc->BtnStatus.AoiUp.FdCw)
	{
		pDoc->BtnStatus.AoiUp.PrevFdCw = pDoc->BtnStatus.AoiUp.FdCw;
		pDoc->SetMkMenu03(_T("AoiUp"), _T("MvCw"), pDoc->BtnStatus.AoiUp.FdCw);
	}
	if (pDoc->BtnStatus.AoiUp.PrevFdCcw != pDoc->BtnStatus.AoiUp.FdCcw)
	{
		pDoc->BtnStatus.AoiUp.PrevFdCcw = pDoc->BtnStatus.AoiUp.FdCcw;
		pDoc->SetMkMenu03(_T("AoiUp"), _T("MvCcw"), pDoc->BtnStatus.AoiUp.FdCcw);
	}
	if (pDoc->BtnStatus.AoiUp.PrevFdVac != pDoc->BtnStatus.AoiUp.FdVac)
	{
		pDoc->BtnStatus.AoiUp.PrevFdVac = pDoc->BtnStatus.AoiUp.FdVac;
		pDoc->SetMkMenu03(_T("AoiUp"), _T("FdVac"), pDoc->BtnStatus.AoiUp.FdVac);
	}
	if (pDoc->BtnStatus.AoiUp.PrevPushUp != pDoc->BtnStatus.AoiUp.PushUp)
	{
		pDoc->BtnStatus.AoiUp.PrevPushUp = pDoc->BtnStatus.AoiUp.PushUp;
		pDoc->SetMkMenu03(_T("AoiUp"), _T("PushUp"), pDoc->BtnStatus.AoiUp.PushUp);
	}
	if (pDoc->BtnStatus.AoiUp.PrevTblBlw != pDoc->BtnStatus.AoiUp.TblBlw)
	{
		pDoc->BtnStatus.AoiUp.PrevTblBlw = pDoc->BtnStatus.AoiUp.TblBlw;
		pDoc->SetMkMenu03(_T("AoiUp"), _T("TblBlw"), pDoc->BtnStatus.AoiUp.TblBlw);
	}
	if (pDoc->BtnStatus.AoiUp.PrevTblVac != pDoc->BtnStatus.AoiUp.TblVac)
	{
		pDoc->BtnStatus.AoiUp.PrevTblVac = pDoc->BtnStatus.AoiUp.TblVac;
		pDoc->SetMkMenu03(_T("AoiUp"), _T("TblVac"), pDoc->BtnStatus.AoiUp.TblVac);
	}
	if (pDoc->BtnStatus.AoiUp.PrevFdClp != pDoc->BtnStatus.AoiUp.FdClp)
	{
		pDoc->BtnStatus.AoiUp.PrevFdClp = pDoc->BtnStatus.AoiUp.FdClp;
		pDoc->SetMkMenu03(_T("AoiUp"), _T("FdClamp"), pDoc->BtnStatus.AoiUp.FdClp);
	}
	if (pDoc->BtnStatus.AoiUp.PrevTqClp != pDoc->BtnStatus.AoiUp.TqClp)
	{
		pDoc->BtnStatus.AoiUp.PrevTqClp = pDoc->BtnStatus.AoiUp.TqClp;
		pDoc->SetMkMenu03(_T("AoiUp"), _T("TensClamp"), pDoc->BtnStatus.AoiUp.TqClp);
	}
	if (pDoc->BtnStatus.AoiUp.PrevMvOne != pDoc->BtnStatus.AoiUp.MvOne)
	{
		pDoc->BtnStatus.AoiUp.PrevMvOne = pDoc->BtnStatus.AoiUp.MvOne;
		pDoc->SetMkMenu03(_T("AoiUp"), _T("OnePnl"), pDoc->BtnStatus.AoiUp.MvOne);
	}
	if (pDoc->BtnStatus.AoiUp.PrevLsrPt != pDoc->BtnStatus.AoiUp.LsrPt)
	{
		pDoc->BtnStatus.AoiUp.PrevLsrPt = pDoc->BtnStatus.AoiUp.LsrPt;
		pDoc->SetMkMenu03(_T("AoiUp"), _T("Lsr"), pDoc->BtnStatus.AoiUp.LsrPt);
	}

	//pDoc->BtnStatus.AoiUp.Relation = m_pMpe->Read(_T("MB005601")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiUp.FdCw = m_pMpe->Read(_T("MB005603")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiUp.FdCcw = m_pMpe->Read(_T("MB005604")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiUp.FdVac = m_pMpe->Read(_T("MB005605")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiUp.PushUp = m_pMpe->Read(_T("MB005606")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiUp.TblBlw = m_pMpe->Read(_T("MB005602")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiUp.TblVac = m_pMpe->Read(_T("MB005607")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiUp.FdClp = m_pMpe->Read(_T("MB005609")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiUp.TqClp = m_pMpe->Read(_T("MB00560A")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiUp.MvOne = m_pMpe->Read(_T("MB440151")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiUp.LsrPt = m_pMpe->Read(_T("MB005608")) ? TRUE : FALSE;

	// Engrave
	pDoc->BtnStatus.Eng.Relation = m_pMpe->Read(_T("MB004511")) ? TRUE : FALSE;
	pDoc->BtnStatus.Eng.FdCw = m_pMpe->Read(_T("MB004513")) ? TRUE : FALSE;
	pDoc->BtnStatus.Eng.FdCcw = m_pMpe->Read(_T("MB004514")) ? TRUE : FALSE;
	pDoc->BtnStatus.Eng.FdVac = m_pMpe->Read(_T("MB004515")) ? TRUE : FALSE;
	pDoc->BtnStatus.Eng.PushUp = m_pMpe->Read(_T("MB004516")) ? TRUE : FALSE;
	pDoc->BtnStatus.Eng.TblBlw = m_pMpe->Read(_T("MB00451F")) ? TRUE : FALSE;
	pDoc->BtnStatus.Eng.TblVac = m_pMpe->Read(_T("MB004517")) ? TRUE : FALSE;
	pDoc->BtnStatus.Eng.FdClp = m_pMpe->Read(_T("MB004519")) ? TRUE : FALSE;
	pDoc->BtnStatus.Eng.TqClp = m_pMpe->Read(_T("MB00451A")) ? TRUE : FALSE;
	pDoc->BtnStatus.Eng.MvOne = m_pMpe->Read(_T("MB440151")) ? TRUE : FALSE;
	pDoc->BtnStatus.Eng.LsrPt = m_pMpe->Read(_T("MB004518")) ? TRUE : FALSE;
	pDoc->BtnStatus.Eng.DcRSol = m_pMpe->Read(_T("MB00451B")) ? TRUE : FALSE;
	pDoc->BtnStatus.Eng.VelSonicBlw = m_pMpe->Read(_T("MB44014E")) ? TRUE : FALSE;

	if (pDoc->BtnStatus.Eng.PrevRelation != pDoc->BtnStatus.Eng.Relation)
	{
		pDoc->BtnStatus.Eng.PrevRelation = pDoc->BtnStatus.Eng.Relation;
		pDoc->SetMkMenu03(_T("Main"), _T("Relation"), pDoc->BtnStatus.Eng.Relation);
	}
	if (pDoc->BtnStatus.Eng.PrevFdCw != pDoc->BtnStatus.Eng.FdCw)
	{
		pDoc->BtnStatus.Eng.PrevFdCw = pDoc->BtnStatus.Eng.FdCw;
		pDoc->SetMkMenu03(_T("Engraving"), _T("MvCw"), pDoc->BtnStatus.Eng.FdCw);
	}
	if (pDoc->BtnStatus.Eng.PrevFdCcw != pDoc->BtnStatus.Eng.FdCcw)
	{
		pDoc->BtnStatus.Eng.PrevFdCcw = pDoc->BtnStatus.Eng.FdCcw;
		pDoc->SetMkMenu03(_T("Engraving"), _T("MvCcw"), pDoc->BtnStatus.Eng.FdCcw);
	}
	if (pDoc->BtnStatus.Eng.PrevFdVac != pDoc->BtnStatus.Eng.FdVac)
	{
		pDoc->BtnStatus.Eng.PrevFdVac = pDoc->BtnStatus.Eng.FdVac;
		pDoc->SetMkMenu03(_T("Engraving"), _T("FdVac"), pDoc->BtnStatus.Eng.FdVac);
	}
	if (pDoc->BtnStatus.Eng.PrevPushUp != pDoc->BtnStatus.Eng.PushUp)
	{
		pDoc->BtnStatus.Eng.PrevPushUp = pDoc->BtnStatus.Eng.PushUp;
		pDoc->SetMkMenu03(_T("Engraving"), _T("PushUp"), pDoc->BtnStatus.Eng.PushUp);
	}
	if (pDoc->BtnStatus.Eng.PrevTblBlw != pDoc->BtnStatus.Eng.TblBlw)
	{
		pDoc->BtnStatus.Eng.PrevTblBlw = pDoc->BtnStatus.Eng.TblBlw;
		pDoc->SetMkMenu03(_T("Engraving"), _T("TblBlw"), pDoc->BtnStatus.Eng.TblBlw);
	}
	if (pDoc->BtnStatus.Eng.PrevTblVac != pDoc->BtnStatus.Eng.TblVac)
	{
		pDoc->BtnStatus.Eng.PrevTblVac = pDoc->BtnStatus.Eng.TblVac;
		pDoc->SetMkMenu03(_T("Engraving"), _T("TblVac"), pDoc->BtnStatus.Eng.TblVac);
	}
	if (pDoc->BtnStatus.Eng.PrevFdClp != pDoc->BtnStatus.Eng.FdClp)
	{
		pDoc->BtnStatus.Eng.PrevFdClp = pDoc->BtnStatus.Eng.FdClp;
		pDoc->SetMkMenu03(_T("Engraving"), _T("FdClamp"), pDoc->BtnStatus.Eng.FdClp);
	}
	if (pDoc->BtnStatus.Eng.PrevTqClp != pDoc->BtnStatus.Eng.TqClp)
	{
		pDoc->BtnStatus.Eng.PrevTqClp = pDoc->BtnStatus.Eng.TqClp;
		pDoc->SetMkMenu03(_T("Engraving"), _T("TensClamp"), pDoc->BtnStatus.Eng.TqClp);
	}
	if (pDoc->BtnStatus.Eng.PrevMvOne != pDoc->BtnStatus.Eng.MvOne)
	{
		pDoc->BtnStatus.Eng.PrevMvOne = pDoc->BtnStatus.Eng.MvOne;
		pDoc->SetMkMenu03(_T("Engraving"), _T("OnePnl"), pDoc->BtnStatus.Eng.MvOne);
	}
	if (pDoc->BtnStatus.Eng.PrevLsrPt != pDoc->BtnStatus.Eng.LsrPt)
	{
		pDoc->BtnStatus.Eng.PrevLsrPt = pDoc->BtnStatus.Eng.LsrPt;
		pDoc->SetMkMenu03(_T("Engraving"), _T("Lsr"), pDoc->BtnStatus.Eng.LsrPt);
	}
	if (pDoc->BtnStatus.Eng.PrevVelSonicBlw != pDoc->BtnStatus.Eng.VelSonicBlw)
	{
		pDoc->BtnStatus.Eng.PrevVelSonicBlw = pDoc->BtnStatus.Eng.VelSonicBlw;
		pDoc->SetMkMenu03(_T("Engraving"), _T("VelClrSonic"), pDoc->BtnStatus.Eng.VelSonicBlw);
	}
	if (pDoc->BtnStatus.Eng.PrevDcRSol != pDoc->BtnStatus.Eng.DcRSol)
	{
		pDoc->BtnStatus.Eng.PrevDcRSol = pDoc->BtnStatus.Eng.DcRSol;
		pDoc->SetMkMenu03(_T("Engraving"), _T("DancerUp"), pDoc->BtnStatus.Eng.DcRSol);
	}

	// Uncoiler
	pDoc->BtnStatus.Uc.Relation = m_pMpe->Read(_T("MB003601")) ? TRUE : FALSE;	// �����Ϸ� ���� ��/���� ����ġ ����
	pDoc->BtnStatus.Uc.FdCw = m_pMpe->Read(_T("MB00360C")) ? TRUE : FALSE;	// �����Ϸ� ��ǰ�� ��ȸ�� ����ġ ����
	pDoc->BtnStatus.Uc.FdCcw = m_pMpe->Read(_T("MB00360D")) ? TRUE : FALSE;	// �����Ϸ� ��ǰ�� ��ȸ�� ����ġ ����
	pDoc->BtnStatus.Uc.ReelChuck = m_pMpe->Read(_T("MB00360B")) ? TRUE : FALSE;	// �����Ϸ� ��ǰô Ŭ���� ����ġ ����
	pDoc->BtnStatus.Uc.DcRlUpDn = m_pMpe->Read(_T("MB003602")) ? TRUE : FALSE;	// �����Ϸ� ���� ���/�ϰ� ����ġ ����
	pDoc->BtnStatus.Uc.ReelJoinL = m_pMpe->Read(_T("MB003605")) ? TRUE : FALSE;	// �����Ϸ� ��ǰ ������(��/��) ����ġ ����
	pDoc->BtnStatus.Uc.ReelJoinR = m_pMpe->Read(_T("MB003606")) ? TRUE : FALSE;	// �����Ϸ� ��ǰ ������(��/��) ����ġ ����
	pDoc->BtnStatus.Uc.ReelJoinVac = m_pMpe->Read(_T("MB00360F")) ? TRUE : FALSE;	// �����Ϸ� ��ǰ ������ ���� ����ġ ����
	pDoc->BtnStatus.Uc.PprChuck = m_pMpe->Read(_T("MB003608")) ? TRUE : FALSE;	// �����Ϸ� ����ô Ŭ���� ����ġ ����
	pDoc->BtnStatus.Uc.PprCw = m_pMpe->Read(_T("MB003609")) ? TRUE : FALSE;	// �����Ϸ� ������ ��ȸ�� ����ġ ����
	pDoc->BtnStatus.Uc.PprCcw = m_pMpe->Read(_T("MB00360A")) ? TRUE : FALSE;	// �����Ϸ� ������ ��ȸ�� ����ġ ����
	pDoc->BtnStatus.Uc.ClRlUpDn = m_pMpe->Read(_T("MB003603")) ? TRUE : FALSE;	// �����Ϸ� Ŭ���ѷ� ���/�ϰ� ����ġ ����
	pDoc->BtnStatus.Uc.ClRlPshUpDn = m_pMpe->Read(_T("MB003604")) ? TRUE : FALSE;	// �����Ϸ� Ŭ���ѷ����� ���/�ϰ� ����ġ ����

	if (pDoc->BtnStatus.Uc.PrevRelation != pDoc->BtnStatus.Uc.Relation)
	{
		pDoc->BtnStatus.Uc.PrevRelation = pDoc->BtnStatus.Uc.Relation;
		pDoc->SetMkMenu03(_T("Main"), _T("Relation"), pDoc->BtnStatus.Uc.Relation);
	}
	if (pDoc->BtnStatus.Uc.PrevFdCw != pDoc->BtnStatus.Uc.FdCw)
	{
		pDoc->BtnStatus.Uc.PrevFdCw = pDoc->BtnStatus.Uc.FdCw;
		pDoc->SetMkMenu03(_T("Uncoiler"), _T("MvCw"), pDoc->BtnStatus.Uc.FdCw);
	}
	if (pDoc->BtnStatus.Uc.PrevFdCcw != pDoc->BtnStatus.Uc.FdCcw)
	{
		pDoc->BtnStatus.Uc.PrevFdCcw = pDoc->BtnStatus.Uc.FdCcw;
		pDoc->SetMkMenu03(_T("Uncoiler"), _T("MvCcw"), pDoc->BtnStatus.Uc.FdCcw);
	}
	if (pDoc->BtnStatus.Uc.PrevReelChuck != pDoc->BtnStatus.Uc.ReelChuck)
	{
		pDoc->BtnStatus.Uc.PrevReelChuck = pDoc->BtnStatus.Uc.ReelChuck;
		pDoc->SetMkMenu03(_T("Uncoiler"), _T("PrdChuck"), pDoc->BtnStatus.Uc.ReelChuck);
	}
	if (pDoc->BtnStatus.Uc.PrevDcRlUpDn != pDoc->BtnStatus.Uc.DcRlUpDn)
	{
		pDoc->BtnStatus.Uc.PrevDcRlUpDn = pDoc->BtnStatus.Uc.DcRlUpDn;
		pDoc->SetMkMenu03(_T("Uncoiler"), _T("DancerUp"), pDoc->BtnStatus.Uc.DcRlUpDn);
	}
	if (pDoc->BtnStatus.Uc.PrevReelJoinL != pDoc->BtnStatus.Uc.ReelJoinL)
	{
		pDoc->BtnStatus.Uc.PrevReelJoinL = pDoc->BtnStatus.Uc.ReelJoinL;
		pDoc->SetMkMenu03(_T("Uncoiler"), _T("PasteUpLf"), pDoc->BtnStatus.Uc.ReelJoinL);
	}
	if (pDoc->BtnStatus.Uc.PrevReelJoinR != pDoc->BtnStatus.Uc.ReelJoinR)
	{
		pDoc->BtnStatus.Uc.PrevReelJoinR = pDoc->BtnStatus.Uc.ReelJoinR;
		pDoc->SetMkMenu03(_T("Uncoiler"), _T("PasteUpRt"), pDoc->BtnStatus.Uc.ReelJoinR);
	}
	if (pDoc->BtnStatus.Uc.PrevReelJoinVac != pDoc->BtnStatus.Uc.ReelJoinVac)
	{
		pDoc->BtnStatus.Uc.PrevReelJoinVac = pDoc->BtnStatus.Uc.ReelJoinVac;
		pDoc->SetMkMenu03(_T("Uncoiler"), _T("PasteVac"), pDoc->BtnStatus.Uc.ReelJoinVac);
	}
	if (pDoc->BtnStatus.Uc.PrevPprChuck != pDoc->BtnStatus.Uc.PprChuck)
	{
		pDoc->BtnStatus.Uc.PrevPprChuck = pDoc->BtnStatus.Uc.PprChuck;
		pDoc->SetMkMenu03(_T("Uncoiler"), _T("PprChuck"), pDoc->BtnStatus.Uc.PprChuck);
	}
	if (pDoc->BtnStatus.Uc.PrevPprCw != pDoc->BtnStatus.Uc.PprCw)
	{
		pDoc->BtnStatus.Rc.PrevPprCw = pDoc->BtnStatus.Uc.PprCw;
		pDoc->SetMkMenu03(_T("Uncoiler"), _T("PprCw"), pDoc->BtnStatus.Uc.PprCw);
	}
	if (pDoc->BtnStatus.Uc.PrevPprCcw != pDoc->BtnStatus.Uc.PprCcw)
	{
		pDoc->BtnStatus.Uc.PrevPprCcw = pDoc->BtnStatus.Uc.PprCcw;
		pDoc->SetMkMenu03(_T("Uncoiler"), _T("PprCcw"), pDoc->BtnStatus.Uc.PprCcw);
	}
	if (pDoc->BtnStatus.Uc.PrevClRlUpDn != pDoc->BtnStatus.Uc.ClRlUpDn)
	{
		pDoc->BtnStatus.Uc.PrevClRlUpDn = pDoc->BtnStatus.Uc.ClRlUpDn;
		pDoc->SetMkMenu03(_T("Uncoiler"), _T("ClrRollUp"), pDoc->BtnStatus.Uc.ClRlUpDn);
	}
	if (pDoc->BtnStatus.Uc.PrevClRlPshUpDn != pDoc->BtnStatus.Uc.ClRlPshUpDn)
	{
		pDoc->BtnStatus.Uc.PrevClRlPshUpDn = pDoc->BtnStatus.Uc.ClRlPshUpDn;
		pDoc->SetMkMenu03(_T("Uncoiler"), _T("ClrRollPush"), pDoc->BtnStatus.Uc.ClRlPshUpDn);
	}

	//pDoc->BtnStatus.Uc.Relation = m_pMpe->Read(_T("MB005401")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Uc.FdCw = m_pMpe->Read(_T("MB00540C")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Uc.FdCcw = m_pMpe->Read(_T("MB00540D")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Uc.ReelChuck = m_pMpe->Read(_T("MB00540B")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Uc.DcRlUpDn = m_pMpe->Read(_T("MB005402")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Uc.ReelJoinL = m_pMpe->Read(_T("MB005405")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Uc.ReelJoinR = m_pMpe->Read(_T("MB005406")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Uc.ReelJoinVac = m_pMpe->Read(_T("MB00540F")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Uc.PprChuck = m_pMpe->Read(_T("MB005408")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Uc.PprCw = m_pMpe->Read(_T("MB005409")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Uc.PprCcw = m_pMpe->Read(_T("MB00540A")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Uc.ClRlUpDn = m_pMpe->Read(_T("MB005403")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Uc.ClRlPshUpDn = m_pMpe->Read(_T("MB005404")) ? TRUE : FALSE;

#else
	pDoc->BtnStatus.Main.Manual = TRUE;
#endif
}

void CManagerPunch::SetPlcParam()
{
#ifdef USE_MPE
	if (!m_pMpe)
		return;

	//long lData;
	MpeWrite(_T("ML45006"), long(_tstof(pDoc->WorkingInfo.LastJob.sTempPauseLen)*1000.0));	// �Ͻ����� ���� (���� M * 1000)
	MpeWrite(_T("ML45002"), long(_tstof(pDoc->WorkingInfo.LastJob.sLotSepLen)*1000.0));	// Lot �и� ���� (���� M * 1000)
	MpeWrite(_T("ML45004"), long(_tstof(pDoc->WorkingInfo.LastJob.sLotCutPosLen)*1000.0));	// Lot �и� �� ������ġ (���� M * 1000)
	MpeWrite(_T("ML45126"), (long)_tstoi(pDoc->WorkingInfo.LastJob.sSampleTestShotNum));	// ���ð˻� Shot��

	if (pDoc->WorkingInfo.LastJob.bTempPause)
	{
		MpeWrite(_T("MB440183"), 1);	// �Ͻ��������(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141031
		ChkTempStop(TRUE);
	}
	else
	{
		MpeWrite(_T("MB440183"), 0);	// �Ͻ��������(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141031
		ChkTempStop(FALSE);
	}

	if (pDoc->WorkingInfo.LastJob.bLotSep)
	{
		pView->SetLotLastShot();
		MpeWrite(_T("MB440184"), 1);	// ��Ʈ�и����(PC�� On��Ű��, PC�� Ȯ���ϰ� Off��Ŵ)-20141031
	}
	else
		MpeWrite(_T("MB440184"), 0);	// ��Ʈ�и����(PC�� On��Ű��, PC�� Ȯ���ϰ� Off��Ŵ)-20141031

	MpeWrite(_T("MB44017A"), (pDoc->WorkingInfo.LastJob.bDualTest) ? 0 : 1);		// �ܸ� �˻� On
	MpeWrite(_T("MB44017B"), (pDoc->WorkingInfo.LastJob.bSampleTest) ? 1 : 0);		// Sample �˻� On
	MpeWrite(_T("MB44017D"), (pDoc->WorkingInfo.LastJob.bOneMetal) ? 1 : 0);		// One Metal On
	MpeWrite(_T("MB44017C"), (pDoc->WorkingInfo.LastJob.bTwoMetal) ? 1 : 0);		// Two Metal On
	MpeWrite(_T("MB44017E"), (pDoc->WorkingInfo.LastJob.bCore150Recoiler) ? 1 : 0);	// Recoiler Core 150mm On
	MpeWrite(_T("MB44017F"), (pDoc->WorkingInfo.LastJob.bCore150Uncoiler) ? 1 : 0);	// Uncoiler Core 150mm On

	MpeWrite(_T("MB44010E"), (pDoc->WorkingInfo.LastJob.bUseAoiUpCleanRoler ? 1 : 0));
	MpeWrite(_T("MB44010F"), (pDoc->WorkingInfo.LastJob.bUseAoiDnCleanRoler ? 1 : 0));
#endif
}

void CManagerPunch::Winker(int nId, int nDly) // 0:Ready, 1:Reset, 2:Run, 3:Stop
{
#ifdef USE_MPE
	if (nId == MN_RUN)
	{
		if (pView)
		{
			pView->m_bBtnWinker[nId] = TRUE;
			MpeWrite(_T("MB44015D"), 1); // �ڵ� �ʱ� ��������(PC�� On/Off ��Ŵ, PLC�� ���������� ��ũ����, on->off�� �������� on, �ٽ� ��������ġ�� �������� off) - 20141017
		}
	}
#endif
}

void CManagerPunch::IoWrite(CString sMReg, long lData)
{
	int i = 0;
	for (i = 0; i < TOT_M_IO; i++)
	{
		if (pDoc->m_pIo[i].nIdx < 0)
		{
			pDoc->m_pIo[i].nIdx = i;
			pDoc->m_pIo[i].sAddr = sMReg;
			pDoc->m_pIo[i].lData = lData;
			break;
		}
	}
	if (i >= TOT_M_IO)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Not enought TOT_M_IO Num!!!"));
	}
}

void CManagerPunch::MonPlcSignal()
{
	pView->MonPlcSignal();
}

void CManagerPunch::MonPlcAlm()
{
	BOOL bMon, bClr;
	long lOn = m_pMpe->Read(_T("ML60000"));

	bMon = lOn & (0x01 << 0);
	bClr = lOn & (0x01 << 1);
	if (bMon)
		PlcAlm(bMon, 0);
	else if (bClr)
		PlcAlm(0, bClr);
	else
		PlcAlm(bMon, bClr);

	if (!pDoc->m_sAlmMsg.IsEmpty())
	{
		if (pDoc->m_sAlmMsg != pDoc->m_sPrevAlmMsg)
		{
			pDoc->m_sPrevAlmMsg = pDoc->m_sAlmMsg;
			CycleStop();
		}
	}
}

void CManagerPunch::MonDispMain()
{
	BOOL bDispStop = TRUE;

#ifdef USE_MPE
	if (pDoc->m_pMpeSignal[2] & (0x01 << 0))		// ������(PLC�� PC�� �˷��ִ� ���� ����) - 20141031
	{
		if (pView->m_sDispMain != _T("������"))
		{
			DispMain(_T("������"), RGB_GREEN);
			pDoc->SetMkMenu03(_T("Main"), _T("Run"), TRUE);
			pDoc->SetMkMenu03(_T("Main"), _T("Stop"), FALSE);
		}
		else
		{
		}
	}

	if (pDoc->m_pMpeSignal[2] & (0x01 << 2))		// �����غ�(PLC�� PC�� �˷��ִ� ���� ����) - 20141031
	{
		bDispStop = FALSE;
		//if(!WatiDispMain(10))
		{
			if (pView->m_sDispMain != _T("�����غ�"))
			{
				DispMain(_T("�����غ�"), RGB_GREEN);
				pDoc->SetMkMenu03(_T("Main"), _T("Ready"), TRUE);
			}
		}
	}
	else
	{
		if (pDoc->m_pMpeSignal[2] & (0x01 << 3))		// �ʱ����(PLC�� PC�� �˷��ִ� ���� ����) - 20141031
		{
			bDispStop = FALSE;
			//if(!WatiDispMain(10))
			{
				if (pDoc->WorkingInfo.LastJob.bSampleTest)
				{
					if (pDoc->WorkingInfo.LastJob.bDualTest)
					{
						if (pView->m_sDispMain != _T("������"))
						{
							DispMain(_T("������"), RGB_GREEN);
						}
					}
					else
					{
						if (pView->m_sDispMain != _T("�ܸ����"))
						{
							DispMain(_T("�ܸ����"), RGB_GREEN);
						}
					}
				}
				else if (pDoc->WorkingInfo.LastJob.bDualTest)
				{
					if (pView->m_sDispMain != _T("���˻�"))
					{
						DispMain(_T("���˻�"), RGB_GREEN);
					}
				}
				else
				{
					if (pView->m_sDispMain != _T("�ܸ�˻�"))
					{
						DispMain(_T("�ܸ�˻�"), RGB_GREEN);
					}
				}
			}
		}
		else
		{
			if (pView->m_sDispMain != _T("�����غ�"))
			{
				bDispStop = TRUE;
			}
			else
			{
				bDispStop = FALSE;
			}
		}
	}


	if (pDoc->m_pMpeSignal[2] & (0x01 << 1))		// ����(PLC�� PC�� �˷��ִ� ���� ����) - 20141031
	{
		if (bDispStop)
		{
			if (pView->m_sDispMain != _T("�� ��"))
			{
				DispMain(_T("�� ��"), RGB_RED);
				pDoc->SetMkMenu03(_T("Main"), _T("Stop"), TRUE);
				pDoc->SetMkMenu03(_T("Main"), _T("Run"), FALSE);
			}
		}
	}
#endif
}

void CManagerPunch::PlcAlm(BOOL bMon, BOOL bClr)
{
	if (bMon && !pView->m_mgrProcedure->m_nMonAlmF)
	{
		pView->m_mgrProcedure->m_nMonAlmF = 1;
		//ResetMonAlm();
		FindAlarm();
		if (pView->m_pEngrave)
		{
			pDoc->m_sIsAlmMsg = _T("");
			pView->m_pEngrave->SetAlarm(pDoc->m_sAlmMsg);
		}

		Sleep(300);
		MpeWrite(_T("MB600008"), 1);
	}
	else if (!bMon && pView->m_mgrProcedure->m_nMonAlmF)
	{
		pView->m_mgrProcedure->m_nMonAlmF = 0;
		ResetMonAlm();
	}
	else
	{
		if (pView->m_pEngrave)
		{
			if (pDoc->m_sIsAlmMsg != pDoc->m_sAlmMsg)
			{
				if (pView->m_pEngrave)
					pView->m_pEngrave->SetAlarm(pDoc->m_sAlmMsg);
			}
		}
	}


	if (bClr && !pView->m_mgrProcedure->m_nClrAlmF)
	{
		pView->m_mgrProcedure->m_nClrAlmF = 1;
		ClrAlarm();
		if (pView->m_pEngrave)
		{
			pDoc->m_sAlmMsg = _T("");
			pView->m_pEngrave->SetAlarm(pDoc->m_sAlmMsg);
		}
		Sleep(300);
		MpeWrite(_T("MB600009"), 1);

	}
	else if (!bClr && pView->m_mgrProcedure->m_nClrAlmF)
	{
		pView->m_mgrProcedure->m_nClrAlmF = 0;
		ResetClear();
	}
	else
	{
		if (pView->m_pEngrave)
		{
			if (pDoc->m_sIsAlmMsg != pDoc->m_sAlmMsg)
			{
				if (pView->m_pEngrave)
					pView->m_pEngrave->SetAlarm(pDoc->m_sAlmMsg);
			}
		}
	}
}

void CManagerPunch::FindAlarm()
{
	// �˶��� �߻��� ������
	long lAlmPage = -1;
	lAlmPage = m_pMpe->Read(_T("ML60002"));

	//char szData[200];
	TCHAR szData[200];
	CString str1, str2, str3, strM, str, strH = _T("");

	str1.Format(_T("%d"), lAlmPage);
	str2 = _T("Address");
	if (0 < ::GetPrivateProfileString(str1, str2, NULL, szData, sizeof(szData), PATH_ALARM))
		strM = CString(szData);
	else
		strM = _T("");

	if (strM.IsEmpty())
		return;

	long lAlm = m_pMpe->Read(strM); // lAlm : (32Bits << Row)
	for (int i = 0; i < 32; i++)
	{
		if (lAlm & (0x01 << i))
		{
			str3.Format(_T("%d"), i);
			if (0 < ::GetPrivateProfileString(str1, str3, NULL, szData, sizeof(szData), PATH_ALARM))
				strH = CString(szData);
			else
				strH.Format(_T("%s = %d"), strM, lAlm);
			//strH = _T("");

			if (str.IsEmpty())
				str = strH;
			else
			{
				str += _T("\r\n");
				str += strH;
			}
		}
	}

	pDoc->m_sAlmMsg = strH;
}

void CManagerPunch::ResetMonAlm()
{
	MpeWrite(_T("MB600008"), 0);
}

void CManagerPunch::ClrAlarm()
{
	if (!pDoc->m_sAlmMsg.IsEmpty())
	{
		pDoc->m_sAlmMsg = _T("");
		pDoc->m_sIsAlmMsg = _T("");
		pDoc->m_sPrevAlmMsg = _T("");
	}
}

void CManagerPunch::ResetClear()
{
	MpeWrite(_T("MB600009"), 0);
}

void CManagerPunch::DoIO()
{
	DoEmgSens();
	DoSaftySens();
	DoDoorSens();

	DoModeSel();
	DoMainSw();
	DoEngraveSens();

	DoInterlock();

	MonPlcAlm();
	MonDispMain();
	MonPlcSignal();

	if (m_bCycleStop)
	{
		m_bCycleStop = FALSE;
		//TowerLamp(RGB_YELLOW, TRUE);
		Buzzer(TRUE);
		if (!pDoc->m_sAlmMsg.IsEmpty())
		{
			MsgBox(pDoc->m_sAlmMsg, 0, 0, DEFAULT_TIME_OUT, FALSE);

			if (pDoc->m_sAlmMsg == GetAoiUpAlarmRestartMsg())
			{
				ChkReTestAlarmOnAoiUp();
			}
			else if (pDoc->m_sAlmMsg == GetAoiDnAlarmRestartMsg())
			{
				ChkReTestAlarmOnAoiDn();
			}
		}
		pDoc->m_sAlmMsg = _T("");
		pDoc->m_sIsAlmMsg = _T("");
		pDoc->m_sPrevAlmMsg = _T("");
	}

	if (pDoc->Status.bManual)
	{
		DoBoxSw();
	}
	else if (pDoc->Status.bAuto)
	{
		DoAutoEng();
		DoAuto();
	}
}

void CManagerPunch::Ink(BOOL bOn)
{
	if (bOn)
	{
		// Ink Marking ON
		//pDoc->m_pSliceIo[7] |= (0x01<<13);	// ��ŷ�� ��ŷ ��Ʈ�ѷ� ��� 2(��������) -> Sen
	}
	else
	{
		// Ink Marking OFF
		//pDoc->m_pSliceIo[7] &= ~(0x01<<13);	// ��ŷ�� ��ŷ ��Ʈ�ѷ� ��� 2(��������) -> Sen
	}
}

void CManagerPunch::MoveAoi(double dOffset)
{
	long lData = (long)(dOffset * 1000.0);		// �˻�� �ǵ� ON (PLC�� �ǵ��Ϸ� �� OFF)
	MpeWrite(_T("MB440160"), 1);	// �˻�� Feeding �ѷ� Offset(*1000, +:�� ����, -�� ����)
	MpeWrite(_T("ML45064"), lData);
}

void CManagerPunch::MoveMk(double dOffset)
{
#ifdef USE_MPE
	long lData = (long)(dOffset * 1000.0);
	MpeWrite(_T("MB440161"), 1);		// ��ŷ�� �ǵ� CW ON (PLC�� �ǵ��Ϸ� �� OFF))
	MpeWrite(_T("ML45066"), lData);		// ��ŷ�� Feeding �ѷ� Offset(*1000, +:�� ����, -�� ����)
#endif
}

void CManagerPunch::CycleStop(BOOL bStop)
{
	m_bCycleStop = bStop;
}

BOOL CManagerPunch::IsCycleStop()
{
	return m_bCycleStop;
}

 
//===> Motion
void CManagerPunch::InitMotion()
{
	int nAxis;
	if (m_pMotion)
	{
		for (nAxis = 0; nAxis < m_pMotion->m_ParamCtrl.nTotAxis; nAxis++)
		{
			m_pMotion->AmpReset(nAxis);
			Sleep(30);
		}
	}
}

void CManagerPunch::InitIO()
{
	int i, k;

#ifdef USE_MPE
	pDoc->m_nMpeIo = pDoc->MkIo.MpeIo.nMaxSeg;
	pDoc->m_nMpeIb = pDoc->MkIo.MpeIo.nMaxSeg;

	if (!pDoc->m_pMpeIb)
	{
		if (pDoc->m_nMpeIb > 0)
		{
			pDoc->m_pMpeIb = new unsigned short[pDoc->m_nMpeIb];
			for (i = 0; i < pDoc->m_nMpeIb; i++)
				pDoc->m_pMpeIb[i] = 0;
		}
	}

	if (!pDoc->m_pMpeIF)
	{
		if (pDoc->m_nMpeIb > 0)
		{
			pDoc->m_pMpeIF = new unsigned short[pDoc->m_nMpeIb];
			for (i = 0; i < pDoc->m_nMpeIb; i++)
				pDoc->m_pMpeIF[i] = 0;
		}
	}

	if (!pDoc->m_pMpeIo)
	{
		if (pDoc->m_nMpeIo > 0)
		{
			pDoc->m_pMpeIo = new unsigned short[pDoc->m_nMpeIo];
			for (i = 0; i < pDoc->m_nMpeIo; i++)
				pDoc->m_pMpeIo[i] = 0;
		}
	}

	if (!pDoc->m_pMpeIoF)
	{
		if (pDoc->m_nMpeIo > 0)
		{
			pDoc->m_pMpeIoF = new unsigned short[pDoc->m_nMpeIo];
			for (i = 0; i < pDoc->m_nMpeIo; i++)
				pDoc->m_pMpeIoF[i] = 0;
		}
	}


	pDoc->m_nMpeSignal = pDoc->MkIo.MpeSignal.nMaxSeg;
	if (!pDoc->m_pMpeSignal)
	{
		if (pDoc->m_nMpeSignal > 0)
		{
			pDoc->m_pMpeSignal = new unsigned short[pDoc->m_nMpeSignal];
			for (i = 0; i < pDoc->m_nMpeSignal; i++)
				pDoc->m_pMpeSignal[i] = 0;
		}
	}


	pDoc->m_nMpeData = pDoc->MkIo.MpeData.nMaxSeg;
	if (!pDoc->m_pMpeData)
	{
		if (pDoc->m_nMpeData > 0)
		{
			pDoc->m_pMpeData = new long*[pDoc->m_nMpeData];
			for (i = 0; i < pDoc->m_nMpeData; i++)
			{
				pDoc->m_pMpeData[i] = new long[16];
				for (k = 0; k < 16; k++)
					pDoc->m_pMpeData[i][k] = 0;
			}
		}
	}
#endif
}

BOOL CManagerPunch::InitAct()
{
#ifdef USE_XMP
	if (!m_pMotion)
		return FALSE;
#endif

#ifdef USE_LIGHT
	if (!m_pLight)
		return FALSE;
#endif

#ifdef USE_FLUCK
	if (!m_pFluck)
		return FALSE;
#endif

	int nAxis;

	if (m_pMotion)
	{
		// Motor On
		for (nAxis = 0; nAxis < m_pMotion->m_ParamCtrl.nTotAxis; nAxis++)
		{
			m_pMotion->ServoOnOff(nAxis, TRUE);
			Sleep(100);
		}

		double dX[2], dY[2];

		if (pDoc->m_pSpecLocal && IsPinData())
		{
			dX[0] = pDoc->m_pSpecLocal->m_dPinPosX[0];
			dY[0] = pDoc->m_pSpecLocal->m_dPinPosY[0];
			dX[1] = pDoc->m_pSpecLocal->m_dPinPosX[1];
			dY[1] = pDoc->m_pSpecLocal->m_dPinPosY[1];
		}
		else
		{
			dX[0] = _tstof(pDoc->WorkingInfo.Motion.sPinPosX[0]);
			dY[0] = _tstof(pDoc->WorkingInfo.Motion.sPinPosY[0]);
			dX[1] = _tstof(pDoc->WorkingInfo.Motion.sPinPosX[1]);
			dY[1] = _tstof(pDoc->WorkingInfo.Motion.sPinPosY[1]);
		}
		m_pMotion->SetPinPos(0, dX[0], dY[0]);
		m_pMotion->SetPinPos(1, dX[1], dY[1]);
		m_pMotion->m_dStBufPos = _tstof(pDoc->WorkingInfo.Motion.sStBufPos);


		CfPoint ptPnt0(dX[0], dY[0]);
		if (pView->m_mgrReelmap->m_Master[0].m_pPcsRgn)
			pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->SetPinPos(0, ptPnt0);

		CfPoint ptPnt1(dX[1], dY[1]);
		if (pView->m_mgrReelmap->m_Master[0].m_pPcsRgn)
			pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->SetPinPos(1, ptPnt1);

		if (pDoc->m_pSpecLocal)// && IsMkOffsetData())
		{
			pView->m_mgrReelmap->SetMkPnt(CAM_BOTH);
		}

		double dPos = _tstof(pDoc->WorkingInfo.Motion.sStBufPos);
		SetBufInitPos(dPos);
		double dVel = _tstof(pDoc->WorkingInfo.Motion.sBufHomeSpd);
		double dAcc = _tstof(pDoc->WorkingInfo.Motion.sBufHomeAcc);
		SetBufHomeParam(dVel, dAcc);
	}

	// Light On
	SetLight(_tstoi(pDoc->WorkingInfo.Light.sVal[0]));
	SetLight2(_tstoi(pDoc->WorkingInfo.Light.sVal[1]));
	//if (pView->m_pDlgMenu02)
	//{
	//	pView->m_pDlgMenu02->SetLight(_tstoi(pDoc->WorkingInfo.Light.sVal[0]));
	//	pView->m_pDlgMenu02->SetLight2(_tstoi(pDoc->WorkingInfo.Light.sVal[1]));
	//}

	// Homming
	if (m_pVoiceCoil[0])
		m_pVoiceCoil[0]->SearchHomeSmac(0);
	if (m_pVoiceCoil[1])
		m_pVoiceCoil[1]->SearchHomeSmac(1);

	return TRUE;
}

void CManagerPunch::SetMainMc(BOOL bOn)
{
	if (bOn)
	{
		if (m_pMpe)
			MpeWrite(_T("MB440159"), 1); // ��ŷ�� Door Interlock ON
		//MpeWrite(_T("MB440159"), 1); // ��ŷ�� MC ON
	}
	else
	{
		if (m_pMpe)
			MpeWrite(_T("MB440158"), 0); // ��ŷ�� Door Interlock OFF
		//MpeWrite(_T("MB440159"), 0); // ��ŷ�� MC OFF
	}
}

BOOL CManagerPunch::IsMotionEnable()
{
	if (m_pMotion->IsEnable(MS_X0) && m_pMotion->IsEnable(MS_Y0) &&
		m_pMotion->IsEnable(MS_X1) && m_pMotion->IsEnable(MS_Y1))
		return TRUE;

	return FALSE;
}

void CManagerPunch::Move0(CfPoint pt, BOOL bCam)
{
	if (m_bProbDn[0])
	{
		if (m_pVoiceCoil[0])
		{
			m_pVoiceCoil[0]->SearchHomeSmac(0);
			m_pVoiceCoil[0]->MoveSmacShiftPos(0);
			m_bProbDn[0] = FALSE;
			//if (pView->m_pDlgMenu02->m_pDlgUtil06)
			//	pView->m_pDlgMenu02->m_pDlgUtil06->myBtn[2].SetCheck(FALSE);
		}
	}

	double fLen, fVel, fAcc, fJerk;

	double pPos[2];

	if (pDoc->WorkingInfo.System.bNoMk || bCam)
	{
		pPos[0] = pt.x;
		pPos[1] = pt.y;
	}
	else
	{
		pPos[0] = pt.x + _tstof(pDoc->WorkingInfo.Vision[0].sMkOffsetX);
		pPos[1] = pt.y + _tstof(pDoc->WorkingInfo.Vision[0].sMkOffsetY);
	}

	if (pPos[0] < 0.0)
		pPos[0] = 0.0;
	if (pPos[1] < 0.0)
		pPos[1] = 0.0;

	double dCurrX = m_dEnc[AXIS_X0];
	double dCurrY = m_dEnc[AXIS_Y0];
	fLen = sqrt(((pPos[0] - dCurrX) * (pPos[0] - dCurrX)) + ((pPos[1] - dCurrY) * (pPos[1] - dCurrY)));
	if (fLen > 0.001)
	{
		m_pMotion->GetSpeedProfile0(TRAPEZOIDAL, AXIS_X0, fLen, fVel, fAcc, fJerk);
		m_pMotion->Move0(MS_X0Y0, pPos, fVel, fAcc, fAcc, ABS, NO_WAIT);
	}
}

void CManagerPunch::Move1(CfPoint pt, BOOL bCam)
{
	if (m_bProbDn[1])
	{
		if (m_pVoiceCoil[1])
		{
			m_pVoiceCoil[1]->SearchHomeSmac(1);
			m_pVoiceCoil[1]->MoveSmacShiftPos(1);
			m_bProbDn[1] = FALSE;
			//if (pView->m_pDlgMenu02->m_pDlgUtil06)
			//	pView->m_pDlgMenu02->m_pDlgUtil06->myBtn[6].SetCheck(FALSE);
		}
	}

	double fLen, fVel, fAcc, fJerk;

	double pPos[2];

	if (pDoc->WorkingInfo.System.bNoMk || bCam)
	{
		pPos[0] = pt.x;
		pPos[1] = pt.y;
	}
	else
	{
		pPos[0] = pt.x + _tstof(pDoc->WorkingInfo.Vision[1].sMkOffsetX);
		pPos[1] = pt.y + _tstof(pDoc->WorkingInfo.Vision[1].sMkOffsetY);
	}

	if (pPos[0] < 0.0)
		pPos[0] = 0.0;
	if (pPos[1] < 0.0)
		pPos[1] = 0.0;

	double dCurrX = m_dEnc[AXIS_X1];
	double dCurrY = m_dEnc[AXIS_Y1];
	fLen = sqrt(((pPos[0] - dCurrX) * (pPos[0] - dCurrX)) + ((pPos[1] - dCurrY) * (pPos[1] - dCurrY)));
	if (fLen > 0.001)
	{
		m_pMotion->GetSpeedProfile1(TRAPEZOIDAL, AXIS_X1, fLen, fVel, fAcc, fJerk);
		m_pMotion->Move1(MS_X1Y1, pPos, fVel, fAcc, fAcc, ABS, NO_WAIT);
	}
}

BOOL CManagerPunch::IsMoveDone()
{
	if (!m_pMotion)
		return FALSE;

	if (IsMoveDone0() && IsMoveDone1())
		return TRUE;
	return FALSE;
}

BOOL CManagerPunch::IsMoveDone0()
{
	if (!m_pMotion)
		return FALSE;

	if (m_pMotion->IsMotionDone(MS_X0) && m_pMotion->IsMotionDone(MS_Y0))
	{
		//	Sleep(50);
		return TRUE;
	}
	return FALSE;
}

BOOL CManagerPunch::IsMoveDone1()
{
	if (!m_pMotion)
		return FALSE;

	if (m_pMotion->IsMotionDone(MS_X1) && m_pMotion->IsMotionDone(MS_Y1))
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CManagerPunch::MovePinPos()
{
	SetLight();

	if (m_pMotion->m_dPinPosY[0] > 0.0 && m_pMotion->m_dPinPosX[0] > 0.0)
	{
		double dCurrX = pView->m_dEnc[AXIS_X0];
		double dCurrY = pView->m_dEnc[AXIS_Y0];

		double pPos[2];
		pPos[0] = m_pMotion->m_dPinPosX[0];
		pPos[1] = m_pMotion->m_dPinPosY[0];

		if (ChkCollision(AXIS_X0, pPos[0]))
		{
			CfPoint ptPnt;
			ptPnt.x = _tstof(pDoc->WorkingInfo.Motion.sSafeZone);
			ptPnt.y = 0.0;//pView->m_dEnc[AXIS_Y1];
			Move1(ptPnt);
		}

		double fLen, fVel, fAcc, fJerk;
		fLen = sqrt(((pPos[0] - dCurrX) * (pPos[0] - dCurrX)) + ((pPos[1] - dCurrY) * (pPos[1] - dCurrY)));
		if (fLen > 0.001)
		{
			m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_X0, fLen, fVel, fAcc, fJerk);
			if (!m_pMotion->Move(MS_X0Y0, pPos, fVel, fAcc, fAcc))
			{
				if (!m_pMotion->Move(MS_X0Y0, pPos, fVel, fAcc, fAcc))
				{
					pView->ClrDispMsg();
					AfxMessageBox(_T("Move XY Error..."));
				}
			}
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CManagerPunch::MoveAlign0(int nPos)
{
	if (!m_pMotion)
		return FALSE;

	SetLight();
	//if (pView->m_pDlgMenu02)
	//	pView->m_pDlgMenu02->SetLight();

	if (m_pMotion->m_dPinPosY[0] > 0.0 && m_pMotion->m_dPinPosX[0] > 0.0)
	{
		double dCurrX = m_dEnc[AXIS_X0];
		double dCurrY = m_dEnc[AXIS_Y0];

		double pPos[2];
		if (nPos == 0)
		{
			pPos[0] = pView->m_mgrReelmap->m_Master[0].m_stAlignMk.X0 + m_pMotion->m_dPinPosX[0];
			pPos[1] = pView->m_mgrReelmap->m_Master[0].m_stAlignMk.Y0 + m_pMotion->m_dPinPosY[0];
		}
		else if (nPos == 1)
		{
			pPos[0] = pView->m_mgrReelmap->m_Master[0].m_stAlignMk.X1 + m_pMotion->m_dPinPosX[0];
			pPos[1] = pView->m_mgrReelmap->m_Master[0].m_stAlignMk.Y1 + m_pMotion->m_dPinPosY[0];
		}

		if (ChkCollision(AXIS_X0, pPos[0]))
			return FALSE;

		double fLen, fVel, fAcc, fJerk;
		fLen = sqrt(((pPos[0] - dCurrX) * (pPos[0] - dCurrX)) + ((pPos[1] - dCurrY) * (pPos[1] - dCurrY)));
		if (fLen > 0.001)
		{
			m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_X0, fLen, fVel, fAcc, fJerk);
			if (!m_pMotion->Move(MS_X0Y0, pPos, fVel, fAcc, fAcc, ABS, NO_WAIT))
			{
				if (!m_pMotion->Move(MS_X0Y0, pPos, fVel, fAcc, fAcc, ABS, NO_WAIT))
				{
					pView->ClrDispMsg();
					AfxMessageBox(_T("Error - Move MoveAlign0 ..."));
					return FALSE;
				}
			}
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CManagerPunch::MoveAlign1(int nPos)
{
	if (!m_pMotion)
		return FALSE;

	SetLight2();
	//if (pView->m_pDlgMenu02)
	//	pView->m_pDlgMenu02->SetLight2();

	if (m_pMotion->m_dPinPosY[1] > 0.0 && m_pMotion->m_dPinPosX[1] > 0.0)
	{
		double dCurrX = m_dEnc[AXIS_X1];
		double dCurrY = m_dEnc[AXIS_Y1];

		double pPos[2];
		if (nPos == 0)
		{
			pPos[0] = pView->m_mgrReelmap->m_Master[0].m_stAlignMk.X0 + m_pMotion->m_dPinPosX[1];
			pPos[1] = pView->m_mgrReelmap->m_Master[0].m_stAlignMk.Y0 + m_pMotion->m_dPinPosY[1];
		}
		else if (nPos == 1)
		{
			pPos[0] = pView->m_mgrReelmap->m_Master[0].m_stAlignMk.X1 + m_pMotion->m_dPinPosX[1];
			pPos[1] = pView->m_mgrReelmap->m_Master[0].m_stAlignMk.Y1 + m_pMotion->m_dPinPosY[1];
		}

		if (ChkCollision(AXIS_X1, pPos[0]))
			return FALSE;

		double fLen, fVel, fAcc, fJerk;
		fLen = sqrt(((pPos[0] - dCurrX) * (pPos[0] - dCurrX)) + ((pPos[1] - dCurrY) * (pPos[1] - dCurrY)));
		if (fLen > 0.001)
		{
			m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_X1, fLen, fVel, fAcc, fJerk);
			if (!m_pMotion->Move(MS_X1Y1, pPos, fVel, fAcc, fAcc, ABS, NO_WAIT))
			{
				if (!m_pMotion->Move(MS_X1Y1, pPos, fVel, fAcc, fAcc, ABS, NO_WAIT))
				{
					pView->ClrDispMsg();
					AfxMessageBox(_T("Error - Move MoveAlign1 ..."));
					return FALSE;
				}
			}
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CManagerPunch::TwoPointAlign0(int nPos, BOOL bDraw)
{
	return pView->TwoPointAlign0(nPos, bDraw);
}

BOOL CManagerPunch::TwoPointAlign1(int nPos, BOOL bDraw)
{
	return pView->TwoPointAlign1(nPos, bDraw);
}

BOOL CManagerPunch::FourPointAlign0(int nPos, BOOL bDraw)
{
	return pView->FourPointAlign0(nPos, bDraw);
}

BOOL CManagerPunch::FourPointAlign1(int nPos, BOOL bDraw)
{
	return pView->FourPointAlign1(nPos, bDraw);
}

BOOL CManagerPunch::OnePointAlign(CfPoint &ptPnt)
{
	return pView->OnePointAlign(ptPnt);
}

BOOL CManagerPunch::TwoPointAlign() // return FALSE; �ΰ�� ����.
{
	return pView->TwoPointAlign();
}

BOOL CManagerPunch::TwoPointAlign(int nPos)
{
	BOOL bRtn[2];

	MoveAlign0(nPos);
	bRtn[0] = TwoPointAlign0(nPos);

	MoveAlign1(nPos);
	bRtn[1] = TwoPointAlign1(nPos);

	if (bRtn[0] && bRtn[1])
		return TRUE;
	return FALSE;
}

BOOL CManagerPunch::IsHomeDone(int nMsId)
{
	if (!m_pMotion)
		return FALSE;

	return m_pMotion->IsHomeDone(nMsId);
}

void CManagerPunch::MoveInitPos0(BOOL bWait)
{
	if (!m_pMotion)
		return;

	double pTgtPos[2];
	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[0]);
	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[0]);
	double dCurrX = m_dEnc[AXIS_X0];
	double dCurrY = m_dEnc[AXIS_Y0];

	double fLen, fVel, fAcc, fJerk;
	fLen = sqrt(((pTgtPos[0] - dCurrX) * (pTgtPos[0] - dCurrX)) + ((pTgtPos[1] - dCurrY) * (pTgtPos[1] - dCurrY)));
	if (fLen > 0.001)
	{
		m_pMotion->GetSpeedProfile0(TRAPEZOIDAL, AXIS_X0, fLen, fVel, fAcc, fJerk);
		if (bWait)
			m_pMotion->Move0(MS_X0Y0, pTgtPos, fVel, fAcc, fAcc, ABS, WAIT);
		else
			m_pMotion->Move0(MS_X0Y0, pTgtPos, fVel, fAcc, fAcc, ABS, NO_WAIT);
	}
}

void CManagerPunch::MoveInitPos1(BOOL bWait)
{
	if (!m_pMotion)
		return;

	double pTgtPos[2];
	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[1]);
	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[1]);
	double dCurrX = m_dEnc[AXIS_X1];
	double dCurrY = m_dEnc[AXIS_Y1];

	double fLen, fVel, fAcc, fJerk;
	fLen = sqrt(((pTgtPos[0] - dCurrX) * (pTgtPos[0] - dCurrX)) + ((pTgtPos[1] - dCurrY) * (pTgtPos[1] - dCurrY)));
	if (fLen > 0.001)
	{
		m_pMotion->GetSpeedProfile1(TRAPEZOIDAL, AXIS_X1, fLen, fVel, fAcc, fJerk);
		if (bWait)
			m_pMotion->Move1(MS_X1Y1, pTgtPos, fVel, fAcc, fAcc, ABS, WAIT);
		else
			m_pMotion->Move1(MS_X1Y1, pTgtPos, fVel, fAcc, fAcc, ABS, NO_WAIT);
	}
}

void CManagerPunch::MoveMkEdPos1()
{
	if (!m_pMotion)
		return;

	double pTgtPos[2];
	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sMkEdPosX[1]);
	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sMkEdPosY[1]);
	double dCurrX = m_dEnc[AXIS_X1];
	double dCurrY = m_dEnc[AXIS_Y1];

	double fLen, fVel, fAcc, fJerk;
	fLen = sqrt(((pTgtPos[0] - dCurrX) * (pTgtPos[0] - dCurrX)) + ((pTgtPos[1] - dCurrY) * (pTgtPos[1] - dCurrY)));
	if (fLen > 0.001)
	{
		m_pMotion->GetSpeedProfile1(TRAPEZOIDAL, AXIS_X1, fLen, fVel, fAcc, fJerk);
		m_pMotion->Move1(MS_X1Y1, pTgtPos, fVel, fAcc, fAcc, ABS, NO_WAIT);
	}
}

BOOL CManagerPunch::IsInitPos0()
{
	if (!m_pMotion)
		return FALSE;

	double pTgtPos[2];
	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[0]);
	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[0]);
	double dCurrX = m_dEnc[AXIS_X0];
	double dCurrY = m_dEnc[AXIS_Y0];

	if (dCurrX < pTgtPos[0] - 2.0 || dCurrX > pTgtPos[0] + 2.0)
		return FALSE;
	if (dCurrY < pTgtPos[1] - 2.0 || dCurrY > pTgtPos[1] + 2.0)
		return FALSE;

	return TRUE;
}

BOOL CManagerPunch::IsInitPos1()
{
	if (!m_pMotion)
		return FALSE;

	double pTgtPos[2];
	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[1]);
	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[1]);
	double dCurrX = m_dEnc[AXIS_X1];
	double dCurrY = m_dEnc[AXIS_Y1];

	if (dCurrX < pTgtPos[0] - 2.0 || dCurrX > pTgtPos[0] + 2.0)
		return FALSE;
	if (dCurrY < pTgtPos[1] - 2.0 || dCurrY > pTgtPos[1] + 2.0)
		return FALSE;

	return TRUE;
}

BOOL CManagerPunch::IsMkEdPos1()
{
	if (!m_pMotion)
		return FALSE;

	double pTgtPos[2];
	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sMkEdPosX[1]);
	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sMkEdPosY[1]);
	double dCurrX = m_dEnc[AXIS_X1];
	double dCurrY = m_dEnc[AXIS_Y1];

	if (dCurrX < pTgtPos[0] - 2.0 || dCurrX > pTgtPos[0] + 2.0)
		return FALSE;
	if (dCurrY < pTgtPos[1] - 2.0 || dCurrY > pTgtPos[1] + 2.0)
		return FALSE;

	return TRUE;
}

void CManagerPunch::MoveMkInitPos()
{
	MoveMk0InitPos();
	MoveMk1InitPos();
}

void CManagerPunch::MoveMk0InitPos()
{
	double pTgtPos[2];
	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[0]);
	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[0]);
	double dCurrX = m_dEnc[AXIS_X0];
	double dCurrY = m_dEnc[AXIS_Y0];

	double fLen, fVel, fAcc, fJerk;
	fLen = sqrt(((pTgtPos[0] - dCurrX) * (pTgtPos[0] - dCurrX)) + ((pTgtPos[1] - dCurrY) * (pTgtPos[1] - dCurrY)));
	if (fLen > 0.001)
	{
		m_pMotion->GetSpeedProfile0(TRAPEZOIDAL, AXIS_X0, fLen, fVel, fAcc, fJerk);
		if (!m_pMotion->Move0(MS_X0Y0, pTgtPos, fVel / 2.0, fAcc / 2.0, fAcc / 2.0))
		{
			if (!m_pMotion->Move0(MS_X0Y0, pTgtPos, fVel / 2.0, fAcc / 2.0, fAcc / 2.0))
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Move X0Y0 Error..."));
			}
		}
	}
}

void CManagerPunch::MoveMk1InitPos()
{
	double pTgtPos[2];
	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[1]);
	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[1]);
	double dCurrX = m_dEnc[AXIS_X1];
	double dCurrY = m_dEnc[AXIS_Y1];

	double fLen, fVel, fAcc, fJerk;
	fLen = sqrt(((pTgtPos[0] - dCurrX) * (pTgtPos[0] - dCurrX)) + ((pTgtPos[1] - dCurrY) * (pTgtPos[1] - dCurrY)));
	if (fLen > 0.001)
	{
		m_pMotion->GetSpeedProfile1(TRAPEZOIDAL, AXIS_X1, fLen, fVel, fAcc, fJerk);
		if (!m_pMotion->Move1(MS_X1Y1, pTgtPos, fVel / 2.0, fAcc / 2.0, fAcc / 2.0))
		{
			if (!m_pMotion->Move1(MS_X1Y1, pTgtPos, fVel / 2.0, fAcc / 2.0, fAcc / 2.0))
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Move X1Y1 Error..."));
			}
		}
	}
}

void CManagerPunch::ResetMotion()
{
	for (int i = 0; i < MAX_MS; i++)
	{
		ResetMotion(i);

		if (i < MAX_AXIS)
		{
			while (!m_pMotion->IsServoOn(i))
			{
				if (i == MS_X0 || i == MS_Y0)
					m_pMotion->Clear(MS_X0Y0);
				else if (i == MS_X1 || i == MS_Y1)
					m_pMotion->Clear(MS_X1Y1);
				else
					m_pMotion->Clear(i);
				Sleep(30);
				m_pMotion->ServoOnOff(i, TRUE);
				Sleep(30);
			}
		}
	}
}

void CManagerPunch::ResetMotion(int nMsId)
{
	//if (pView->m_pDlgMenu02)
	//	pView->m_pDlgMenu02->ResetMotion(nMsId);
	if (!m_pMotion)
		return;

	long lRtn = m_pMotion->GetState(nMsId);  // -1 : MPIStateERROR, 0 : MPIStateIDLE, 1 : MPIStateSTOPPING, 2 : MPIStateMOVING
	if (lRtn == 2)
	{
		if (nMsId == MS_X0 || nMsId == MS_Y0)
			m_pMotion->Abort(MS_X0Y0);
		else if (nMsId == MS_X1 || nMsId == MS_Y1)
			m_pMotion->Abort(MS_X1Y1);
		else
			m_pMotion->Abort(nMsId);
		Sleep(30);
	}

	if (nMsId == MS_X0 || nMsId == MS_Y0)
		m_pMotion->Clear(MS_X0Y0);
	else if (nMsId == MS_X1 || nMsId == MS_Y1)
		m_pMotion->Clear(MS_X1Y1);
	else
		m_pMotion->Clear(nMsId);

	Sleep(30);

	if (!m_pMotion->IsEnable(nMsId))
	{
		if (nMsId == MS_X0Y0 || nMsId == MS_X0 || nMsId == MS_Y0)
		{
			m_pMotion->ServoOnOff(AXIS_X0, TRUE);
			Sleep(30);
			m_pMotion->ServoOnOff(AXIS_Y0, TRUE);
		}
		else if (nMsId == MS_X1Y1 || nMsId == MS_X1 || nMsId == MS_Y1)
		{
			m_pMotion->ServoOnOff(AXIS_X1, TRUE);
			Sleep(30);
			m_pMotion->ServoOnOff(AXIS_Y1, TRUE);
		}
		else
			m_pMotion->ServoOnOff(nMsId, TRUE);

		Sleep(30);
	}
}

BOOL CManagerPunch::IsRunAxisX()
{
	if (m_pMotion->IsMotionDone(MS_X0) && m_pMotion->IsMotionDone(MS_X1))
		return FALSE;
	return TRUE;
}

BOOL CManagerPunch::IsMotionDone(int nAxisId)
{
	BOOL bDone = FALSE;
	if (m_pMotion)
		bDone = m_pMotion->IsMotionDone(nAxisId);

	return bDone;
}

void CManagerPunch::EStop()
{
	if (m_pMotion)
	{
		m_pMotion->EStop(MS_X0Y0);
		m_pMotion->EStop(MS_X1Y1);
		Sleep(30);
		ResetMotion(MS_X0Y0);
		Sleep(30);
		ResetMotion(MS_X1Y1);
		Sleep(30);
		// 		DispMsg(_T("X�� �浹 ������ ���� �����Դϴ�."), _T("�˸�"), RGB_GREEN, DELAY_TIME_MSG);
		pView->ClrDispMsg();
		AfxMessageBox(_T("X�� �浹 ������ ���� �����Դϴ�."));

		double dCurrX = m_dEnc[AXIS_X1];
		double dCurrY = m_dEnc[AXIS_Y1];
		double pPos[2], fVel, fAcc, fJerk;
		double fLen = 2.0;
		pPos[0] = dCurrX + fLen;
		pPos[1] = dCurrY;
		m_pMotion->GetSpeedProfile1(TRAPEZOIDAL, AXIS_X1, fLen, fVel, fAcc, fJerk);
		m_pMotion->Move1(MS_X1Y1, pPos, fVel, fAcc, fAcc, ABS, NO_WAIT);
	}
}

void CManagerPunch::SetAlignPos()
{
	if (m_pMotion)
	{
		CfPoint ptPos1, ptPos2;
		pView->m_mgrReelmap->GetAlignPos(ptPos1, ptPos2);

		m_pMotion->m_dAlignPosX[0][0] = ptPos1.x + m_pMotion->m_dPinPosX[0];
		m_pMotion->m_dAlignPosY[0][0] = ptPos1.y + m_pMotion->m_dPinPosY[0];
		m_pMotion->m_dAlignPosX[0][1] = ptPos2.x + m_pMotion->m_dPinPosX[0];
		m_pMotion->m_dAlignPosY[0][1] = ptPos2.y + m_pMotion->m_dPinPosY[0];

		m_pMotion->m_dAlignPosX[1][0] = ptPos1.x + m_pMotion->m_dPinPosX[1];
		m_pMotion->m_dAlignPosY[1][0] = ptPos1.y + m_pMotion->m_dPinPosY[1];
		m_pMotion->m_dAlignPosX[1][1] = ptPos2.x + m_pMotion->m_dPinPosX[1];
		m_pMotion->m_dAlignPosY[1][1] = ptPos2.y + m_pMotion->m_dPinPosY[1];
	}
}

void CManagerPunch::SetAlignPosUp()
{
	if (m_pMotion)
	{
		CfPoint ptPos1, ptPos2;
		pView->m_mgrReelmap->GetAlignPos(ptPos1, ptPos2);

		m_pMotion->m_dAlignPosX[0][0] = ptPos1.x + m_pMotion->m_dPinPosX[0];
		m_pMotion->m_dAlignPosY[0][0] = ptPos1.y + m_pMotion->m_dPinPosY[0];
		m_pMotion->m_dAlignPosX[0][1] = ptPos2.x + m_pMotion->m_dPinPosX[0];
		m_pMotion->m_dAlignPosY[0][1] = ptPos2.y + m_pMotion->m_dPinPosY[0];
	}
}

void CManagerPunch::SetAlignPosDn()
{
	if (m_pMotion)
	{
		CfPoint ptPos1, ptPos2;
		pView->m_mgrReelmap->GetAlignPos(ptPos1, ptPos2);

		m_pMotion->m_dAlignPosX[1][0] = ptPos1.x + m_pMotion->m_dPinPosX[1];
		m_pMotion->m_dAlignPosY[1][0] = ptPos1.y + m_pMotion->m_dPinPosY[1];
		m_pMotion->m_dAlignPosX[1][1] = ptPos2.x + m_pMotion->m_dPinPosX[1];
		m_pMotion->m_dAlignPosY[1][1] = ptPos2.y + m_pMotion->m_dPinPosY[1];
	}
}

void CManagerPunch::ChkTempStop(BOOL bChk)
{
	pView->ChkTempStop(bChk);
	//if (bChk)
	//{
	//	if (!m_bTIM_CHK_TEMP_STOP)
	//	{
	//		m_bTIM_CHK_TEMP_STOP = TRUE;
	//		SetTimer(TIM_CHK_TEMP_STOP, 500, NULL);
	//	}
	//}
	//else
	//{
	//	m_bTIM_CHK_TEMP_STOP = FALSE;
	//}
}

BOOL CManagerPunch::MoveMeasPos(int nId)
{
	if (!m_pMotion)
		return FALSE;

	if (!m_pLight)
		return FALSE;

	if (nId == 0)
	{
		SetLight();
		//if (pView->m_pDlgMenu02)
		//	pView->m_pDlgMenu02->SetLight();

		double dMkOffsetX, dMkOffsetY;
		if (pDoc->WorkingInfo.Vision[0].sMkOffsetX.IsEmpty())
			dMkOffsetX = 0.0;
		else
			dMkOffsetX = _tstof(pDoc->WorkingInfo.Vision[0].sMkOffsetX);

		if (pDoc->WorkingInfo.Vision[0].sMkOffsetY.IsEmpty())
			dMkOffsetY = 0.0;
		else
			dMkOffsetY = _tstof(pDoc->WorkingInfo.Vision[0].sMkOffsetY);


		double pPos[2];
		if (pView->m_mgrProcedure->m_bFailAlign[0][0] || pView->m_mgrProcedure->m_bFailAlign[0][1])
		{
			pPos[0] = _tstof(pDoc->WorkingInfo.Probing[nId].sMeasurePosX) + dMkOffsetX;
			pPos[1] = _tstof(pDoc->WorkingInfo.Probing[nId].sMeasurePosY) + dMkOffsetY;
		}
		else
		{
			pPos[0] = _tstof(pDoc->WorkingInfo.Probing[nId].sMeasurePosX) + dMkOffsetX - pView->m_dMkFdOffsetX[0][0];
			pPos[1] = _tstof(pDoc->WorkingInfo.Probing[nId].sMeasurePosY) + dMkOffsetY - pView->m_dMkFdOffsetY[0][0];
		}

		if (pPos[1] > 0.0 && pPos[0] > 0.0)
		{
			double dCurrX = m_dEnc[AXIS_X0];
			double dCurrY = m_dEnc[AXIS_Y0];

			if (ChkCollision(AXIS_X0, pPos[0]))
			{
				CfPoint ptPnt;
				ptPnt.x = _tstof(pDoc->WorkingInfo.Motion.sSafeZone);
				ptPnt.y = 0.0; // m_dEnc[AXIS_Y1];
				Move1(ptPnt);
			}

			double fLen, fVel, fAcc, fJerk;
			fLen = sqrt(((pPos[0] - dCurrX) * (pPos[0] - dCurrX)) + ((pPos[1] - dCurrY) * (pPos[1] - dCurrY)));
			if (fLen > 0.001)
			{
				m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_X0, fLen, fVel, fAcc, fJerk);
				if (!m_pMotion->Move(MS_X0Y0, pPos, fVel, fAcc, fAcc))
				{
					if (!m_pMotion->Move(MS_X0Y0, pPos, fVel, fAcc, fAcc))
					{
						pView->ClrDispMsg();
						AfxMessageBox(_T("Move XY Error..."));
					}
				}
			}

			return TRUE;
		}
	}
	else if (nId == 1)
	{
		if (pView->m_pDlgMenu02)
			pView->m_pDlgMenu02->SetLight2();

		double dMkOffsetX, dMkOffsetY;
		if (pDoc->WorkingInfo.Vision[1].sMkOffsetX.IsEmpty())
			dMkOffsetX = 0.0;
		else
			dMkOffsetX = _tstof(pDoc->WorkingInfo.Vision[1].sMkOffsetX);

		if (pDoc->WorkingInfo.Vision[1].sMkOffsetY.IsEmpty())
			dMkOffsetY = 0.0;
		else
			dMkOffsetY = _tstof(pDoc->WorkingInfo.Vision[1].sMkOffsetY);


		double pPos[2];
		if (pView->m_mgrProcedure->m_bFailAlign[1][0] || pView->m_mgrProcedure->m_bFailAlign[1][1])
		{
			pPos[0] = _tstof(pDoc->WorkingInfo.Probing[nId].sMeasurePosX) + dMkOffsetX;
			pPos[1] = _tstof(pDoc->WorkingInfo.Probing[nId].sMeasurePosY) + dMkOffsetY;
		}
		else
		{
			pPos[0] = _tstof(pDoc->WorkingInfo.Probing[nId].sMeasurePosX) + dMkOffsetX - pView->m_dMkFdOffsetX[1][0];
			pPos[1] = _tstof(pDoc->WorkingInfo.Probing[nId].sMeasurePosY) + dMkOffsetY - pView->m_dMkFdOffsetY[1][0];
		}

		if (pPos[1] > 0.0 && pPos[0] > 0.0)
		{
			double dCurrX = m_dEnc[AXIS_X1];
			double dCurrY = m_dEnc[AXIS_Y1];

			if (ChkCollision(AXIS_X1, pPos[0]))
			{
				CfPoint ptPnt;
				ptPnt.x = 0.0;
				ptPnt.y = 0.0;//m_dEnc[AXIS_Y0];
				Move0(ptPnt);
			}

			double fLen, fVel, fAcc, fJerk;
			fLen = sqrt(((pPos[0] - dCurrX) * (pPos[0] - dCurrX)) + ((pPos[1] - dCurrY) * (pPos[1] - dCurrY)));
			if (fLen > 0.001)
			{
				m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_X0, fLen, fVel, fAcc, fJerk);
				if (!m_pMotion->Move(MS_X1Y1, pPos, fVel, fAcc, fAcc))
				{
					if (!m_pMotion->Move(MS_X1Y1, pPos, fVel, fAcc, fAcc))
					{
						pView->ClrDispMsg();
						AfxMessageBox(_T("Move XY Error..."));
					}
				}
			}

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CManagerPunch::IsPinPos0()
{
	if (!m_pMotion)
		return FALSE;

	double pPos[2];
	pPos[0] = m_pMotion->m_dPinPosX[0];
	pPos[1] = m_pMotion->m_dPinPosY[0];

	double dCurrX = m_dEnc[AXIS_X0];
	double dCurrY = m_dEnc[AXIS_Y0];

	if (dCurrX < pPos[0] - 4.0 || dCurrX > pPos[0] + 4.0)
		return FALSE;
	if (dCurrY < pPos[1] - 4.0 || dCurrY > pPos[1] + 4.0)
		return FALSE;

	return TRUE;
}

BOOL CManagerPunch::IsPinPos1()
{
	if (!m_pMotion)
		return FALSE;

	double pPos[2];
	pPos[0] = m_pMotion->m_dPinPosX[1];
	pPos[1] = m_pMotion->m_dPinPosY[1];

	double dCurrX = m_dEnc[AXIS_X1];
	double dCurrY = m_dEnc[AXIS_Y1];

	if (dCurrX < pPos[0] - 4.0 || dCurrX > pPos[0] + 4.0)
		return FALSE;
	if (dCurrY < pPos[1] - 4.0 || dCurrY > pPos[1] + 4.0)
		return FALSE;

	return TRUE;
}

void CManagerPunch::GetEnc()
{
	if (!m_pMotion)
		return;

	m_dEnc[AXIS_X0] = pView->m_dEnc[AXIS_X0] = m_pMotion->GetActualPosition(AXIS_X0);
	m_dEnc[AXIS_Y0] = pView->m_dEnc[AXIS_Y0] = m_pMotion->GetActualPosition(AXIS_Y0);
	m_dEnc[AXIS_X1] = pView->m_dEnc[AXIS_X1] = m_pMotion->GetActualPosition(AXIS_X1);
	m_dEnc[AXIS_Y1] = pView->m_dEnc[AXIS_Y1] = m_pMotion->GetActualPosition(AXIS_Y1);
}


//===> Vision
BOOL CManagerPunch::StartLive()
{
	if (StartLive0() && StartLive1())
		return TRUE;

	return  FALSE;
}

BOOL CManagerPunch::StartLive0()
{
	BOOL bRtn0 = FALSE;

#ifdef USE_VISION
	if (m_pVision[0])
		bRtn0 = m_pVision[0]->StartLive();
#endif
	if (bRtn0)
		return TRUE;

	return  FALSE;
}

BOOL CManagerPunch::StartLive1()
{
	BOOL bRtn1 = FALSE;

#ifdef USE_VISION
	if (m_pVision[1])
		bRtn1 = m_pVision[1]->StartLive();
#endif

	if (bRtn1)
		return TRUE;

	return  FALSE;
}

BOOL CManagerPunch::StopLive()
{
#ifdef TEST_MODE
	return TRUE;
#endif

	if (StopLive0() && StopLive1())
		return TRUE;

	return FALSE;
}

BOOL CManagerPunch::StopLive0()
{
#ifdef TEST_MODE
	return TRUE;
#endif

	BOOL bRtn0 = FALSE;

#ifdef USE_VISION
	if (m_pVision[0])
		bRtn0 = m_pVision[0]->StopLive();
#endif
	if (bRtn0)
		return TRUE;

	return FALSE;
}

BOOL CManagerPunch::StopLive1()
{
#ifdef TEST_MODE
	return TRUE;
#endif

	BOOL bRtn1 = FALSE;

#ifdef USE_VISION
	if (m_pVision[1])
		bRtn1 = m_pVision[1]->StopLive();
#endif

	if (bRtn1)
		return TRUE;

	return FALSE;
}

BOOL CManagerPunch::ChkLightErr()
{
	int nSerial, nErrCode;
	BOOL bError = FALSE;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Cam0

	if (pView->m_mgrProcedure->m_bSerialDecrese)
	{
		if (nSerial > 0 && nSerial >= pView->m_mgrProcedure->m_nLotEndSerial)
		{
			if ((nErrCode = pView->GetErrCode(nSerial)) != 1)
			{
				if (nErrCode == -1) // -1(Align Error, �뱤�ҷ�)
					bError = TRUE;
			}
		}

		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

		if (nSerial > 0 && nSerial >= pView->m_mgrProcedure->m_nLotEndSerial)
		{
			if ((nErrCode = pView->GetErrCode(nSerial)) != 1)
			{
				if (nErrCode == -1) // -1(Align Error, �뱤�ҷ�)
					bError = TRUE;
			}
		}
	}
	else
	{
		if (nSerial > 0 && nSerial <= pView->m_mgrProcedure->m_nLotEndSerial)
		{
			if ((nErrCode = pView->GetErrCode(nSerial)) != 1)
			{
				if (nErrCode == -1) // -1(Align Error, �뱤�ҷ�)
					bError = TRUE;
			}
		}

		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

		if (nSerial > 0 && nSerial <= pView->m_mgrProcedure->m_nLotEndSerial)
		{
			if ((nErrCode = pView->GetErrCode(nSerial)) != 1)
			{
				if (nErrCode == -1) // -1(Align Error, �뱤�ҷ�)
					bError = TRUE;
			}
		}
	}

	if (bError)
	{
		Stop();
		//TowerLamp(RGB_RED, TRUE);
		Buzzer(TRUE, 0);
		DispMain(_T("�� ��"), RGB_RED);
	}

	return bError;
}


//===> Punching
void CManagerPunch::DoMark0()
{
#ifdef TEST_MODE
	return;
#endif

	if (!m_bAuto)
		return;

	int nMaxStrip;
#ifdef USE_CAM_MASTER
	nMaxStrip = pView->m_mgrReelmap->m_Master[0].GetStripNum(); // �� ��Ʈ���� ����
#else
	nMaxStrip = MAX_STRIP;
#endif

	int nSerial, nIdx, nErrCode, nRtn;
	CfPoint ptPnt;
	CString sMsg;
	double dStripOut = (pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs / nMaxStrip) * _tstof(pDoc->WorkingInfo.LastJob.sStripOutRatio) / 100.0;
	int nStripOut = int(dStripOut);
	if (dStripOut > nStripOut)
		nStripOut++;			// ��Ʈ�� ���� ����

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (!pView->IsRun())																		// �������¿���
	{
		if (IsOnMarking0())																// ��ŷ�߿�
		{
			if (m_pMotion->IsEnable(MS_X0) && m_pMotion->IsEnable(MS_Y0))				// ����� Enable�����̰�
			{
				if (m_pMotion->IsMotionDone(MS_X0) && m_pMotion->IsMotionDone(MS_Y0))	// ��� Done�����̸�,
				{
					if (!IsInitPos0() && !IsPinPos0())									// �ʱ���ġ�� �ƴϰų�, ����ġ�� �ƴҶ�
						MoveInitPos0();													// �ʱ���ġ�� �̵�
				}
			}

			if (m_nStepMk[0] < 13 && m_nStepMk[0] > 8) // Mk0();
			{
				m_nStepMk[0] = 8;	// ��ŷ�Ϸ�Check
			}
		}
		return;
	}

	pView->m_mgrProcedure->m_sDispSts[0].Format(_T("%d"), m_nStepMk[0]);

	switch (m_nStepMk[0])
	{
	case 0:
		if (IsNoMk())
			pView->ShowLive();
		m_nStepMk[0]++;
		break;
	case 1:
		if (!IsInitPos0())
			MoveInitPos0();
		m_nStepMk[0]++;
		break;
	case 2:

		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Cam0

													 //nSerial = 1;

		if (nSerial > 0)
		{
			//if ((nErrCode = pView->GetErrCode0(nSerial)) != 1)
			if ((nErrCode = pView->GetErrCode(nSerial)) != 1)
			{
				m_nMkPcs[0] = pView->GetTotDefPcs(nSerial);
				m_nStepMk[0] = MK_END;
			}
			else
			{
				m_nStepMk[0]++;
			}
		}
		else
		{
			StopFromThread();
			pView->AsyncMsgBox(_T("������ �ø����� �����ʽ��ϴ�."), 1);
			BuzzerFromThread(TRUE, 0);
			//pView->DispStsBar(_T("����-31"), 0);
			DispMain(_T("�� ��"), RGB_RED);
		}
		break;
	case 3:
		m_nStepMk[0]++;
		break;
	case 4:
		m_nStepMk[0]++;
		break;
	case 5:
		m_nStepMk[0]++;
		break;
	case 6:

		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Cam0

		if (m_nMkPcs[0] < pView->GetTotDefPcs(nSerial))
		{
			if (!IsNoMk0())
			{
				;
			}
			else
			{
				if (!IsReview0())
				{
					if (m_bReview)
					{
						m_nMkPcs[0] = pView->GetTotDefPcs(nSerial);
						m_nStepMk[0] = MK_END;
						break;
					}
				}
			}
			// Punching On�̰ų� Review�̸� �������� ����
			SetDelay0(100, 1);		// [mSec]
			m_nStepMk[0]++;
		}
		else
		{
			m_nStepMk[0] = MK_END;
		}
		break;
	case 7:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Cam0

		if (!WaitDelay0(1))		// F:Done, T:On Waiting....		// Delay�Ŀ�
		{
			m_nMkPcs[0] = 0;

			if (!IsNoMk0())										// Punching On�̸�
			{
				m_nStepMk[0]++;
			}
			else												// Punching�� Off�̰�
			{
				if (IsReview0())								// Review�̸� ��������
				{
					m_nStepMk[0]++;
				}
				else											// Review�� �ƴϸ�
				{
					if (m_bReview)
					{
						m_nMkPcs[0] = pView->GetTotDefPcs(nSerial);
						m_nStepMk[0] = MK_END;
					}
					else
						m_nStepMk[0]++;
				}
			}
		}
		break;
	case 8:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Cam0

		if (m_nMkPcs[0] < pView->GetTotDefPcs(nSerial))	// ��ŷ�Ϸ�Check
		{
			if (m_nMkPcs[0] + 1 < pView->GetTotDefPcs(nSerial))		// ���� ��ŷ��ġ�� ������
			{
				ptPnt = pView->GetMkPnt(nSerial, m_nMkPcs[0] + 1);	// ���� ��ŷ��ġ
				m_dNextTarget[AXIS_X0] = ptPnt.x;
				m_dNextTarget[AXIS_Y0] = ptPnt.y;
			}
			else												// ���� ��ŷ��ġ�� ������
			{
				m_dNextTarget[AXIS_X0] = -1.0;
				m_dNextTarget[AXIS_Y0] = -1.0;
			}

			ptPnt = pView->GetMkPnt(nSerial, m_nMkPcs[0]);			// �̹� ��ŷ��ġ
			if (ptPnt.x < 0.0 && ptPnt.y < 0.0) // ��ǰȭ. (��ŷ���� ����)
			{
				m_nMkPcs[0]++;
				m_nStepMk[0] = MK_DONE_CHECK;
				break;
			}

			nIdx = pView->GetMkStripIdx(nSerial, m_nMkPcs[0]);		// 1 ~ 4 : strip index
			if (nIdx > 0)										// Strip index�� �����̸�,
			{
				if (!pView->IsMkStrip(nIdx)) // Strip[] Mk Off
				{
					m_nMkPcs[0]++;
					m_nStepMk[0] = MK_DONE_CHECK;
					break;
				}
				else
				{
					if (m_nMkStrip[0][nIdx - 1] >= nStripOut)
					{
						m_nMkPcs[0]++;
						m_nStepMk[0] = MK_DONE_CHECK;
						break;
					}
					else
						m_nMkStrip[0][nIdx - 1]++;
				}
			}
			else
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Strip Index Failed."));
				break;
			}

			m_dTarget[AXIS_X0] = ptPnt.x;
			m_dTarget[AXIS_Y0] = ptPnt.y;

			m_nStepMk[0]++;
		}
		else
		{
			m_nStepMk[0] = MK_END;
		}
		break;
	case 9:
		// Cam0 : m_bPriority[0] <-, m_bPriority[3] ->
		// Cam1 : m_bPriority[1] ->, m_bPriority[2] <-
		ptPnt.x = m_dTarget[AXIS_X0];
		ptPnt.y = m_dTarget[AXIS_Y0];

		if (m_dNextTarget[AXIS_X0] < 0)
			m_bCollision[0] = ChkCollision(AXIS_X0, m_dTarget[AXIS_X0]);
		else
			m_bCollision[0] = ChkCollision(AXIS_X0, m_dTarget[AXIS_X0], m_dNextTarget[AXIS_X0]);

		if (!m_bCollision[0])
		{
			if (IsMoveDone0())
			{
				Move0(ptPnt, m_bCam);
				m_nStepMk[0]++;
			}
		}
		else if (m_bPriority[0])
		{
			if (IsMoveDone0())
			{
				m_bCollision[0] = FALSE;
				m_bPriority[0] = FALSE;
				Move0(ptPnt, m_bCam);
				m_nStepMk[0]++;
			}
		}
		else if (m_bPriority[2])
		{
			if (IsMoveDone0())
			{
				m_bCollision[0] = FALSE;
				m_bPriority[2] = FALSE;
				ptPnt.x = 0.0;
				ptPnt.y = 0.0;//m_dEnc[AXIS_Y0];
				Move0(ptPnt, FALSE);
			}
		}
		break;
	case 10:
		m_nStepMk[0]++;
		break;
	case 11:
		m_nStepMk[0]++;
		break;
	case 12:
		if (IsMoveDone0())
			m_nStepMk[0]++;
		break;
	case 13:
		if (!IsNoMk0())
		{
			pView->m_mgrProcedure->m_dwStMkDn[0] = GetTickCount();
			Mk0();
		}
		else
		{
			// Verify - Mk0
			SetDelay0(pDoc->m_nDelayShow, 1);		// [mSec]
			if (!SaveMk0Img(m_nMkPcs[0]))
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Error-SaveMk0Img()"));
			}
			//m_nDebugStep = m_nMkPcs[0]; DispThreadTick();
		}
		m_nStepMk[0]++;
		break;
	case 14:
		m_nStepMk[0]++;
		break;
	case 15:
		m_nStepMk[0]++;
		break;
	case 16:
		if (IsNoMk0())
		{
			if (!WaitDelay0(1))		// F:Done, T:On Waiting....
				m_nStepMk[0]++;
		}
		else
			m_nStepMk[0]++;
		break;
	case 17:
		if (!IsNoMk0())
		{
			if (IsMk0Done())
			{
				// One more MK On Start....
				if (!m_nMkPcs[0] && !pView->m_mgrProcedure->m_bAnswer[2])
				{
					pView->m_mgrProcedure->m_bAnswer[2] = TRUE;
					Mk0();
				}
				else
				{
					pView->m_mgrProcedure->m_bAnswer[2] = FALSE;
					m_nMkPcs[0]++;
					m_nStepMk[0]++;
					m_nStepMk[0]++;
				}
			}
			else
			{
				if (pView->m_mgrProcedure->m_dwStMkDn[0] + 5000 < GetTickCount())
				{
					BuzzerFromThread(TRUE, 0);
					DispMain(_T("�� ��"), RGB_RED);
					m_pVoiceCoil[0]->SearchHomeSmac0();

					nRtn = pView->AsyncMsgBox(_T("���̽�����(��) ��ſϷᰡ �ʵ˴ϴ�.\r\n��ŷ�� �ٽ� �õ��Ͻðڽ��ϱ�?"), 1, MB_YESNO);
					if (IDYES == nRtn)
					{
						DispMain(_T("������"), RGB_RED);
						m_nStepMk[0] = 13;
					}
					else if (nRtn < 0)
						m_nStepMk[0]++; // Wait...
					else
					{
						pView->m_mgrProcedure->m_bAnswer[2] = FALSE;
						m_nMkPcs[0]++;
						m_nStepMk[0]++;
						m_nStepMk[0]++;
						Stop();
					}
				}
			}
		}
		else
		{
			m_nMkPcs[0]++;
			m_nStepMk[0]++;
			m_nStepMk[0]++;
		}
		break;
	case 18:
		if ((nRtn = WaitRtnVal(1)) > -1)
		{
			if (IDYES == nRtn)
			{
				DispMain(_T("������"), RGB_RED);
				m_nStepMk[0] = 13;
			}
			else
			{
				pView->m_mgrProcedure->m_bAnswer[2] = FALSE;
				m_nMkPcs[0]++;
				m_nStepMk[0]++;
				StopFromThread();
			}
		}
		break;
	case 19:
		m_nStepMk[0] = MK_DONE_CHECK;
		break;
	case MK_DONE_CHECK:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Cam0

		if (m_nMkPcs[0] < pView->GetTotDefPcs(nSerial))
		{
			if (IsNoMk0())
			{
				if (!pDoc->WorkingInfo.System.bNoMk)
				{
					if (IsReview0())
					{
						if (pView->IsJogRtDn0())
							m_nStepMk[0]++;
					}
					else if (!pDoc->WorkingInfo.LastJob.bVerify)
					{
						m_nMkPcs[0] = pView->GetTotDefPcs(nSerial);
						m_nStepMk[0] = MK_END;
					}
					else
						m_nStepMk[0] = 8;	// ��ŷ�Ϸ�Check
				}
				else
					m_nStepMk[0] = 8;	// ��ŷ�Ϸ�Check
			}
			else
				m_nStepMk[0] = 8;	// ��ŷ�Ϸ�Check
		}
		else
		{
			if (IsNoMk0())
			{
				if (IsReview0())
				{
					if (pView->IsJogRtDn0())
						m_nStepMk[0] = MK_END;
				}
				else
				{
					m_nStepMk[0] = MK_END;
				}
			}
			else
				m_nStepMk[0] = MK_END;
		}
		break;
	case MK_DONE_CHECK + 1:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Cam0

		if (m_nMkPcs[0] < pView->GetTotDefPcs(nSerial))
		{
			if (IsNoMk0())
			{
				if (IsReview0())
				{
					if (pView->IsJogRtUp0())
						m_nStepMk[0] = 8;	// ��ŷ�Ϸ�Check
				}
				else
				{
					m_nMkPcs[0] = pView->GetTotDefPcs(nSerial);
					m_nStepMk[0] = MK_END;
				}
			}
			else
				m_nStepMk[0] = 8;	// ��ŷ�Ϸ�Check
		}
		else
		{
			m_nStepMk[0] = MK_END;
		}
		break;

	case MK_END:
		if (IsMoveDone0())
			m_nStepMk[0]++;
		break;
	case 101:
		SetDelay0(100, 1);		// [mSec]
		m_nStepMk[0]++;
		break;
	case 102:
		if (!WaitDelay0(1))		// F:Done, T:On Waiting....
		{
			m_nStepMk[0]++;
		}
		break;
	case 103:
		if (!IsInitPos0())
		{
			m_dTarget[AXIS_X0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[0]);
			m_dTarget[AXIS_Y0] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[0]);
			m_dNextTarget[AXIS_X0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[0]);
			m_dNextTarget[AXIS_Y0] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[0]);

			MoveInitPos0();
		}

		pDoc->SaveMkCntL();
		m_nStepMk[0]++;
		break;
	case 104:
		if (IsMoveDone0())
		{
			m_nStepMk[0]++;
			SetDelay0(10000, 1);		// [mSec]
		}
		break;
	case 105:
		if (IsInitPos0())
		{
			m_nStepMk[0]++;
		}
		else
		{
			if (!WaitDelay0(1))		// F:Done, T:On Waiting....
			{
				m_nStepMk[0] = ERR_PROC;
			}
		}
		break;
	case 106: // MK Done....
		m_bDoneMk[0] = TRUE;
		m_bTHREAD_MK[0] = FALSE;
		break;

	case ERR_PROC:
		DispMain(_T("�� ��"), RGB_RED);
		m_pVoiceCoil[0]->SearchHomeSmac0();
		pView->AsyncMsgBox(_T("���̽�����(��) �ʱ���ġ �̵��� ���� �ʽ��ϴ�.\r\n��ŷ���¸� Ȯ���ϼ���."), 1);
		m_nStepMk[0]++;
		break;
	case ERR_PROC + 1:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Cam0

		if (m_bCam)
			sMsg.Format(_T("%d�� Shot�� �ٽ� �ҷ�Ȯ���� �Ͻðڽ��ϱ�?"), nSerial);
		else
			sMsg.Format(_T("%d�� Shot�� �ٽ� ��ŷ�Ͻðڽ��ϱ�?"), nSerial);

		pView->m_mgrProcedure->m_nRtnMyMsgBoxIdx = 0;
		pView->m_mgrProcedure->m_bRtnMyMsgBox[0] = FALSE;
		pView->m_mgrProcedure->m_nRtnMyMsgBox[0] = -1;
		pView->AsyncMsgBox(sMsg, 1, MB_YESNO);
		sMsg.Empty();
		m_nStepMk[0]++;
		break;
	case ERR_PROC + 2:
		if ((nRtn = WaitRtnVal()) > -1)
		{
			if (IDYES == nRtn)
			{
				m_nStepMk[0] = ERR_PROC + 10;
			}
			else
			{
				pView->m_mgrProcedure->m_nRtnMyMsgBoxIdx = 0;
				pView->m_mgrProcedure->m_bRtnMyMsgBox[0] = FALSE;
				pView->m_mgrProcedure->m_nRtnMyMsgBox[0] = -1;
				sMsg.Format(_T("��� ���� �۾��� �����Ͻðڽ��ϱ�?"), nSerial);
				pView->AsyncMsgBox(sMsg, 1, MB_YESNO);
				sMsg.Empty();

				m_nStepMk[0]++;
			}
		}
		break;
	case ERR_PROC + 3:
		if ((nRtn = WaitRtnVal()) > -1)
		{
			if (IDYES == nRtn)
			{
				m_nStepMk[0] = ERR_PROC + 20;
			}
			else
			{
				pView->m_bDispMsgDoAuto[8] = TRUE;
				pView->m_nStepDispMsg[8] = FROM_DOMARK0;
			}
		}
		break;
	case ERR_PROC + 10:
		m_bReMark[0] = TRUE;
		m_bTHREAD_MK[0] = FALSE;
		m_nStepMk[0] = 0;
		break;
	case ERR_PROC + 20: // MK Done....
		m_bDoneMk[0] = TRUE;
		m_bTHREAD_MK[0] = FALSE;
		break;
	}
}

void CManagerPunch::DoMark1()
{
#ifdef TEST_MODE
	return;
#endif

	if (!m_bAuto)
		return;

	int nMaxStrip;
#ifdef USE_CAM_MASTER
	nMaxStrip = pView->m_mgrReelmap->m_Master[0].GetStripNum(); // �� ��Ʈ���� ����
#else
	nMaxStrip = MAX_STRIP;
#endif

	int nSerial, nIdx, nErrCode, nRtn;
	CfPoint ptPnt;
	CString sMsg;
	double dStripOut = (pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs / nMaxStrip) * _tstof(pDoc->WorkingInfo.LastJob.sStripOutRatio) / 100.0;
	int nStripOut = int(dStripOut);
	if (dStripOut > nStripOut)
		nStripOut++;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (!pView->IsRun())
	{
		if (IsOnMarking1())
		{
			if (m_pMotion->IsEnable(MS_X1) && m_pMotion->IsEnable(MS_Y1))
			{
				if (m_pMotion->IsMotionDone(MS_X1) && m_pMotion->IsMotionDone(MS_Y1))
				{
					if (!IsMkEdPos1() && !IsPinPos1())
						MoveMkEdPos1();
				}
			}

			if (m_nStepMk[1] < 13 && m_nStepMk[1] > 8) // Mk1();
			{
				m_nStepMk[1] = 8;	// ��ŷ�Ϸ�Check
			}
		}
		return;
	}


	pView->m_mgrProcedure->m_sDispSts[1].Format(_T("%d"), m_nStepMk[1]);

	switch (m_nStepMk[1])
	{
	case 0:
		if (IsNoMk())
			pView->ShowLive();
		m_nStepMk[1]++;
		break;
	case 1:
		if (!IsInitPos1())
			MoveInitPos1();
		m_nStepMk[1]++;
		break;
	case 2:

		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

		if (nSerial > 0)
		{
			//if ((nErrCode = pView->GetErrCode1(nSerial)) != 1)
			if ((nErrCode = pView->GetErrCode(nSerial)) != 1)
			{
				m_nMkPcs[1] = pView->GetTotDefPcs(nSerial);
				m_nStepMk[1] = MK_END;
			}
			else
			{
				m_nStepMk[1]++;
			}
		}
		else
		{
			StopFromThread();
			pView->AsyncMsgBox(_T("������ �ø����� �����ʽ��ϴ�."), 2);
			BuzzerFromThread(TRUE, 0);
			DispMain(_T("�� ��"), RGB_RED);
		}
		break;
	case 3:
		m_nStepMk[1]++;
		break;
	case 4:
		m_nStepMk[1]++;
		break;
	case 5:
		m_nStepMk[1]++;
		break;
	case 6:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

		if (m_nMkPcs[1] < pView->GetTotDefPcs(nSerial))
		{
			if (!IsNoMk1())
			{
				;
			}
			else
			{
				if (!IsReview1())
				{
					if (m_bReview)
					{
						m_nMkPcs[1] = pView->GetTotDefPcs(nSerial);
						m_nStepMk[1] = MK_END;
						break;
					}
				}
			}
			SetDelay1(100, 6);		// [mSec]
			m_nStepMk[1]++;
		}
		else
		{
			m_nStepMk[1] = MK_END;
		}
		break;
	case 7:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

		if (!WaitDelay1(6))		// F:Done, T:On Waiting....
		{
			m_nMkPcs[1] = 0;

			if (!IsNoMk1())
			{
				m_nStepMk[1]++;
			}
			else
			{
				if (IsReview1())
				{
					m_nStepMk[1]++;
				}
				else
				{
					if (m_bReview)
					{
						m_nMkPcs[1] = pView->GetTotDefPcs(nSerial);
						m_nStepMk[1] = MK_END;
					}
					else
						m_nStepMk[1]++;
				}
			}
		}
		break;
	case 8:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

		if (m_nMkPcs[1] < pView->GetTotDefPcs(nSerial))	// ��ŷ�Ϸ�Check
		{
			if (m_nMkPcs[1] + 1 < pView->GetTotDefPcs(nSerial))
			{
				ptPnt = pView->GetMkPnt(nSerial, m_nMkPcs[1] + 1);
				m_dNextTarget[AXIS_X1] = ptPnt.x;
				m_dNextTarget[AXIS_Y1] = ptPnt.y;
			}
			else
			{
				m_dNextTarget[AXIS_X1] = -1.0;
				m_dNextTarget[AXIS_Y1] = -1.0;
			}

			ptPnt = pView->GetMkPnt(nSerial, m_nMkPcs[1]);
			if (ptPnt.x < 0.0 && ptPnt.y < 0.0) // ��ǰȭ.
			{
				m_nMkPcs[1]++;
				m_nStepMk[1] = MK_DONE_CHECK;
				break;
			}

			nIdx = pView->GetMkStripIdx(nSerial, m_nMkPcs[1]);
			if (nIdx > 0)
			{
				if (!pView->IsMkStrip(nIdx)) // Strip[] Mk Off
				{
					m_nMkPcs[1]++;
					m_nStepMk[1] = MK_DONE_CHECK;
					break;
				}
				else
				{
					if (m_nMkStrip[1][nIdx - 1] >= nStripOut)
					{
						m_nMkPcs[1]++;
						m_nStepMk[1] = MK_DONE_CHECK;
						break;
					}
					else
						m_nMkStrip[1][nIdx - 1]++;
				}
			}
			else
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Strip Index Failed."));
				break;
			}

			m_dTarget[AXIS_X1] = ptPnt.x;
			m_dTarget[AXIS_Y1] = ptPnt.y;

			m_nStepMk[1]++;
		}
		else
		{
			m_nStepMk[1] = MK_END;
		}
		break;
	case 9:
		// Cam0 : m_bPriority[0] <-, m_bPriority[3] ->
		// Cam1 : m_bPriority[1] ->, m_bPriority[2] <-
		ptPnt.x = m_dTarget[AXIS_X1];
		ptPnt.y = m_dTarget[AXIS_Y1];

		if (m_dNextTarget[AXIS_X1] < 0)
			m_bCollision[1] = ChkCollision(AXIS_X1, m_dTarget[AXIS_X1]);
		else
			m_bCollision[1] = ChkCollision(AXIS_X1, m_dTarget[AXIS_X1], m_dNextTarget[AXIS_X1]);

		if (!m_bCollision[1])
		{
			if (IsMoveDone1())
			{
				Move1(ptPnt, m_bCam);
				m_nStepMk[1]++;
			}
		}
		else if (m_bPriority[1])
		{
			if (IsMoveDone1())
			{
				m_bCollision[1] = FALSE;
				m_bPriority[1] = FALSE;
				Move1(ptPnt, m_bCam);
				m_nStepMk[1]++;
			}
		}
		else if (m_bPriority[3])
		{
			if (IsMoveDone1())
			{
				m_bCollision[1] = FALSE;
				m_bPriority[3] = FALSE;
				ptPnt.x = _tstof(pDoc->WorkingInfo.Motion.sSafeZone);
				ptPnt.y = 0.0;//m_dEnc[AXIS_Y1];
				Move1(ptPnt, FALSE);
			}
		}
		break;
	case 10:
		m_nStepMk[1]++;
		break;
	case 11:
		m_nStepMk[1]++;
		break;
	case 12:
		if (IsMoveDone1())
			m_nStepMk[1]++;
		break;
	case 13:
		if (!IsNoMk1())
		{
			pView->m_mgrProcedure->m_dwStMkDn[1] = GetTickCount();
			Mk1();
		}
		else
		{
			// Verify - Mk1
			SetDelay1(pDoc->m_nDelayShow, 6);		// [mSec]
			if (!SaveMk1Img(m_nMkPcs[1]))
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Error-SaveMk1Img()"));
			}
		}
		m_nStepMk[1]++;
		break;
	case 14:
		m_nStepMk[1]++;
		break;
	case 15:
		m_nStepMk[1]++;
		break;
	case 16:
		if (IsNoMk1())
		{
			if (!WaitDelay1(6))		// F:Done, T:On Waiting....
				m_nStepMk[1]++;
		}
		else
			m_nStepMk[1]++;
		break;
	case 17:
		if (!IsNoMk1())
		{
			if (IsMk1Done())
			{
				// One more MK On Start....
				if (!m_nMkPcs[1] && !pView->m_mgrProcedure->m_bAnswer[3])
				{
					pView->m_mgrProcedure->m_bAnswer[3] = TRUE;
					Mk1();
				}
				else
				{
					pView->m_mgrProcedure->m_bAnswer[3] = FALSE;
					m_nMkPcs[1]++;
					m_nStepMk[1]++;
					m_nStepMk[1]++;
				}

			}
			else
			{
				if (pView->m_mgrProcedure->m_dwStMkDn[1] + 5000 < GetTickCount())
				{
					BuzzerFromThread(TRUE, 0);
					DispMain(_T("�� ��"), RGB_RED);
					m_pVoiceCoil[1]->SearchHomeSmac1();

					nRtn = pView->AsyncMsgBox(_T("���̽�����(��) ��ſϷᰡ �ʵ˴ϴ�.\r\n��ŷ�� �ٽ� �õ��Ͻðڽ��ϱ�?"), 2, MB_YESNO);
					if (IDYES == nRtn)
					{
						DispMain(_T("������"), RGB_RED);
						m_nStepMk[1] = 13;
					}
					else if (nRtn < 0)
						m_nStepMk[1]++; // Wait...
					else
					{
						pView->m_mgrProcedure->m_bAnswer[3] = FALSE;
						m_nMkPcs[1]++;
						m_nStepMk[1]++;
						m_nStepMk[1]++;
						Stop();
					}
				}
			}
		}
		else
		{
			m_nMkPcs[1]++;
			m_nStepMk[1]++;
			m_nStepMk[1]++;
		}
		break;
	case 18:
		if ((nRtn = WaitRtnVal(2)) > -1)
		{
			if (IDYES == nRtn)
			{
				DispMain(_T("������"), RGB_RED);
				m_nStepMk[1] = 13;
			}
			else
			{
				pView->m_mgrProcedure->m_bAnswer[3] = FALSE;
				m_nMkPcs[1]++;
				m_nStepMk[1]++;
				StopFromThread();
			}
		}
		break;
	case 19:
		m_nStepMk[1] = MK_DONE_CHECK;
		break;
	case MK_DONE_CHECK:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

		if (m_nMkPcs[1] < pView->GetTotDefPcs(nSerial))
		{
			if (IsNoMk1())
			{
				if (!pDoc->WorkingInfo.System.bNoMk)
				{
					if (IsReview1())
					{
						if (pView->IsJogRtDn1())
							m_nStepMk[1]++;
					}
					else if (!pDoc->WorkingInfo.LastJob.bVerify)
					{
						m_nMkPcs[1] = pView->GetTotDefPcs(nSerial);
						m_nStepMk[1] = MK_END;
					}
					else
						m_nStepMk[1] = 8;	// ��ŷ�Ϸ�Check
				}
				else
					m_nStepMk[1] = 8;	// ��ŷ�Ϸ�Check
			}
			else
				m_nStepMk[1] = 8;	// ��ŷ�Ϸ�Check
		}
		else
		{
			if (IsNoMk1())
			{
				if (IsReview1())
				{
					if (pView->IsJogRtDn1())
						m_nStepMk[1] = MK_END;
				}
				else
				{
					m_nStepMk[1] = MK_END;
				}
			}
			else
				m_nStepMk[1] = MK_END;
		}
		break;
	case MK_DONE_CHECK + 1:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

		if (m_nMkPcs[1] < pView->GetTotDefPcs(nSerial))
		{
			if (IsNoMk1())
			{
				if (IsReview1())
				{
					if (pView->IsJogRtUp1())
						m_nStepMk[1] = 8;	// ��ŷ�Ϸ�Check
				}
				else
				{
					m_nMkPcs[1] = pView->GetTotDefPcs(nSerial);
					m_nStepMk[1] = MK_END;
				}
			}
			else
				m_nStepMk[1] = 8;	// ��ŷ�Ϸ�Check
		}
		else
		{
			m_nStepMk[1] = MK_END;
		}
		break;


	case MK_END:
		if (IsMoveDone1())
			m_nStepMk[1]++;
		break;
	case 101:
		SetDelay1(100, 6);		// [mSec]
		m_nStepMk[1]++;
		break;
	case 102:
		if (!WaitDelay1(6))		// F:Done, T:On Waiting....
			m_nStepMk[1]++;
		break;
	case 103:
		if (!IsMkEdPos1())
		{
			m_dTarget[AXIS_X1] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[1]);
			m_dTarget[AXIS_Y1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[1]);
			m_dNextTarget[AXIS_X1] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[1]);
			m_dNextTarget[AXIS_Y1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[1]);

			MoveMkEdPos1();
		}

		pDoc->SaveMkCntR();
		m_nStepMk[1]++;
		break;
	case 104:
		if (IsMoveDone1())
		{
			m_nStepMk[1]++;
			SetDelay1(10000, 6);		// [mSec]
		}
		break;
	case 105:
		if (IsMkEdPos1())
		{
			m_nStepMk[1]++;
		}
		else
		{
			if (!WaitDelay1(6))		// F:Done, T:On Waiting....
			{
				m_nStepMk[1] = ERR_PROC;
			}
		}
		break;
	case 106: // MK Done....
		m_bDoneMk[1] = TRUE;
		m_bTHREAD_MK[1] = FALSE;
		break;

	case ERR_PROC:
		DispMain(_T("�� ��"), RGB_RED);
		m_pVoiceCoil[1]->SearchHomeSmac1();
		pView->AsyncMsgBox(_T("���̽�����(��) �ʱ���ġ �̵��� ���� �ʽ��ϴ�.\r\n��ŷ���¸� Ȯ���ϼ���."), 2);
		m_nStepMk[1]++;
		break;
	case ERR_PROC + 1:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

		if (m_bCam)
			sMsg.Format(_T("%d�� Shot�� �ٽ� �ҷ�Ȯ���� �Ͻðڽ��ϱ�?"), nSerial);
		else
			sMsg.Format(_T("%d�� Shot�� �ٽ� ��ŷ�Ͻðڽ��ϱ�?"), nSerial);

		pView->m_mgrProcedure->m_nRtnMyMsgBoxIdx = 1;
		pView->m_mgrProcedure->m_bRtnMyMsgBox[1] = FALSE;
		pView->m_mgrProcedure->m_nRtnMyMsgBox[1] = -1;
		pView->AsyncMsgBox(sMsg, 2, MB_YESNO);
		sMsg.Empty();
		m_nStepMk[1]++;
		break;
	case ERR_PROC + 2:
		if ((nRtn = WaitRtnVal()) > -1)
		{
			if (IDYES == nRtn)
			{
				m_nStepMk[1] = ERR_PROC + 10;
			}
			else
			{
				pView->m_mgrProcedure->m_nRtnMyMsgBoxIdx = 1;
				pView->m_mgrProcedure->m_bRtnMyMsgBox[1] = FALSE;
				pView->m_mgrProcedure->m_nRtnMyMsgBox[1] = -1;
				sMsg.Format(_T("��� ���� �۾��� �����Ͻðڽ��ϱ�?"), nSerial);
				pView->AsyncMsgBox(sMsg, 2, MB_YESNO);
				sMsg.Empty();

				m_nStepMk[1]++;

			}
		}
		break;
	case ERR_PROC + 3:
		if ((nRtn = WaitRtnVal()) > -1)
		{
			if (IDYES == nRtn)
			{
				m_nStepMk[1] = ERR_PROC + 20;
			}
			else
			{
				pView->m_bDispMsgDoAuto[9] = TRUE;
				pView->m_nStepDispMsg[9] = FROM_DOMARK1;
			}
		}
		break;
	case ERR_PROC + 10:
		m_bReMark[1] = TRUE;
		m_bTHREAD_MK[1] = FALSE;
		m_nStepMk[1] = 0;
		break;
	case ERR_PROC + 20: // MK Done....
		m_bDoneMk[1] = TRUE;
		m_bTHREAD_MK[1] = FALSE;
		break;
	}
}

void CManagerPunch::DoMark0All()
{
	CfPoint ptPnt;

	if (!pView->IsRun())
	{
		if (IsOnMarking0())
		{
			if (m_pMotion->IsEnable(MS_X0) && m_pMotion->IsEnable(MS_Y0))
			{
				if (m_pMotion->IsMotionDone(MS_X0) && m_pMotion->IsMotionDone(MS_Y0))
				{
					if (!IsInitPos0() && !IsPinPos0())
						MoveInitPos0();
				}
			}

			if (m_nStepMk[2] < 5 && m_nStepMk[2] > 3) // Mk0();
			{
				m_nStepMk[2] = 3;	// ��ŷ�Ϸ�Check
			}
		}
		return;
	}


	switch (m_nStepMk[2])
	{
	case 0:
		m_nStepMk[2]++;
		break;
	case 1:
		if (m_nMkPcs[2] < pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)
		{
			m_nStepMk[2]++;
		}
		else
		{
			m_nStepMk[2] = MK_END;
		}
		break;
	case 2:
		m_nStepMk[2]++;
		break;
	case 3:
		if (m_nMkPcs[2] < pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)	// ��ŷ�Ϸ�Check
		{
			ptPnt = pView->GetMkPnt(m_nMkPcs[2]);

			if (ptPnt.x < 0.0 && ptPnt.y < 0.0) // ��ǰȭ.
			{
				m_nMkPcs[2]++;
				break;
			}

			m_dTarget[AXIS_X0] = ptPnt.x;

			// Cam0 : m_bPriority[0] <-, m_bPriority[3] ->
			// Cam1 : m_bPriority[1] ->, m_bPriority[2] <-

			m_bCollision[0] = ChkCollision(AXIS_X0, ptPnt.x);
			if (!m_bCollision[0])
			{
				if (IsMoveDone0())
				{
					Move0(ptPnt, m_bCam);
					m_nStepMk[2]++;
				}
			}
			else if (m_bPriority[0])
			{
				if (IsMoveDone0())
				{
					m_bPriority[0] = FALSE;
					Move0(ptPnt, m_bCam);
					m_nStepMk[2]++;
				}
			}
			else if (m_bPriority[2])
			{
				if (IsMoveDone0())
				{
					m_bPriority[2] = FALSE;
					ptPnt.x = 0.0;
					ptPnt.y = 0.0;//m_dEnc[AXIS_Y0];
					Move0(ptPnt, FALSE);
				}
			}
		}
		else
		{
			m_nStepMk[2] = MK_END;
		}
		break;
	case 4:
		if (IsMoveDone0())
			m_nStepMk[2]++;
		break;
	case 5:
		if (!IsNoMk0())
			Mk0();
		else
			SetDelay0(pDoc->m_nDelayShow, 1);		// [mSec]
		m_nStepMk[2]++;
		break;
	case 6:
		if (IsNoMk0())
		{
			if (!WaitDelay0(1))		// F:Done, T:On Waiting....
				m_nStepMk[2]++;
		}
		else
			m_nStepMk[2]++;
		break;
	case 7:
		m_nMkPcs[2]++;
		m_nStepMk[2]++;
		break;
	case 8:
		if (m_nMkPcs[2] < pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)	// ��ŷ�Ϸ�Check
		{
			m_nStepMk[2] = 3;
		}
		else
		{
			m_nStepMk[2] = MK_END;
		}
		break;
	case MK_END:
		SetDelay0(100, 1);		// [mSec]
		m_nStepMk[2]++;
		break;
	case 101:
		if (!WaitDelay0(1))		// F:Done, T:On Waiting....
		{
			if (IsMoveDone0())
			{
				MoveInitPos0();
				m_nStepMk[2]++;
			}
		}
		break;
	case 102:
		if (IsMoveDone0())
			m_nStepMk[2]++;
		break;
	case 103:
		m_bDoneMk[0] = TRUE;
		m_bTHREAD_MK[2] = FALSE;
		break;
	}
}

void CManagerPunch::DoMark1All()
{
	CfPoint ptPnt;

	if (!pView->IsRun())
	{
		if (IsOnMarking1())
		{
			if (m_pMotion->IsEnable(MS_X1) && m_pMotion->IsEnable(MS_Y1))
			{
				if (m_pMotion->IsMotionDone(MS_X1) && m_pMotion->IsMotionDone(MS_Y1))
				{
					if (!IsMkEdPos1() && !IsPinPos1())
						MoveMkEdPos1();
				}
			}

			if (m_nStepMk[3] < 5 && m_nStepMk[3] > 3) // Mk1();
			{
				m_nStepMk[3] = 3;	// ��ŷ�Ϸ�Check
			}
		}
		return;
	}


	switch (m_nStepMk[3])
	{
	case 0:
		m_nStepMk[3]++;
		break;
	case 1:
		if (m_nMkPcs[3] < pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)
		{
			m_nStepMk[3]++;
		}
		else
		{
			m_nStepMk[3] = MK_END;
		}
		break;
	case 2:
		m_nStepMk[3]++;
		break;
	case 3:
		if (m_nMkPcs[3] < pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)	// ��ŷ�Ϸ�Check
		{
			ptPnt = pView->GetMkPnt(m_nMkPcs[3]);

			if (ptPnt.x < 0.0 && ptPnt.y < 0.0) // ��ǰȭ.
			{
				m_nMkPcs[3]++;
				break;
			}

			m_dTarget[AXIS_X1] = ptPnt.x;

			// Cam0 : m_bPriority[0] <-, m_bPriority[3] ->
			// Cam1 : m_bPriority[1] ->, m_bPriority[2] <-

			m_bCollision[1] = ChkCollision(AXIS_X1, ptPnt.x);
			if (!m_bCollision[1])
			{
				if (IsMoveDone1())
				{
					Move1(ptPnt, m_bCam);
					m_nStepMk[3]++;
				}
			}
			else if (m_bPriority[1])
			{
				if (IsMoveDone1())
				{
					m_bPriority[1] = FALSE;
					Move1(ptPnt, m_bCam);
					m_nStepMk[3]++;
				}
			}
			else if (m_bPriority[3])
			{
				if (IsMoveDone1())
				{
					m_bPriority[3] = FALSE;
					ptPnt.x = _tstof(pDoc->WorkingInfo.Motion.sSafeZone);
					ptPnt.y = 0.0;//m_dEnc[AXIS_Y1];
					Move1(ptPnt, FALSE);
				}
			}
		}
		else
		{
			m_nStepMk[3] = MK_END;
		}
		break;
	case 4:
		if (IsMoveDone1())
			m_nStepMk[3]++;
		break;
	case 5:
		if (!IsNoMk1())
			Mk1();
		else
			SetDelay1(pDoc->m_nDelayShow, 1);		// [mSec]
		m_nStepMk[3]++;
		break;
	case 6:
		if (IsNoMk1())
		{
			if (!WaitDelay1(1))		// F:Done, T:On Waiting....
				m_nStepMk[3]++;
		}
		else
			m_nStepMk[3]++;
		break;
	case 7:
		m_nMkPcs[3]++;
		m_nStepMk[3]++;
		break;
	case 8:
		if (m_nMkPcs[3] < pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)	// ��ŷ�Ϸ�Check
		{
			m_nStepMk[3] = 3;
		}
		else
		{
			//StopLive1();
			m_nStepMk[3] = MK_END;
		}
		break;
	case MK_END:
		SetDelay1(100, 1);		// [mSec]
		m_nStepMk[3]++;
		break;
	case 101:
		if (!WaitDelay1(1))		// F:Done, T:On Waiting....
		{
			if (m_nMkPcs[2] < pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)
			{
				if (m_nMkPcs[2] > 0)
				{
					if (!(pView->m_mgrProcedure->m_Flag & (0x01 << 0)))
					{
						if (IsMoveDone1())
						{
							pView->m_mgrProcedure->m_Flag |= (0x01 << 0);
							ptPnt.x = _tstof(pDoc->WorkingInfo.Motion.sSafeZone);
							ptPnt.y = 0.0;//m_dEnc[AXIS_Y1];
							Move1(ptPnt, FALSE);
						}
					}
				}
				else
				{
					if (IsMoveDone1())
					{
						pView->m_mgrProcedure->m_Flag &= ~(0x01 << 0);
						MoveMkEdPos1();
						m_nStepMk[3]++;
					}
				}
			}
			else
			{
				if (IsMoveDone1())
				{
					pView->m_mgrProcedure->m_Flag &= ~(0x01 << 0);
					MoveMkEdPos1();
					m_nStepMk[3]++;
				}
			}
		}
		break;
	case 102:
		if (IsMoveDone0())
			m_nStepMk[3]++;
		break;
	case 103:
		m_bDoneMk[1] = TRUE;
		m_bTHREAD_MK[3] = FALSE;
		break;
	}
}

void CManagerPunch::DoAllMk(int nCam)
{
	if (nCam == CAM_LF)
	{
		double dCurrX = m_dEnc[AXIS_X1];
		double dCurrY = m_dEnc[AXIS_Y1];

		double pPos[2];
		pPos[0] = _tstof(pDoc->WorkingInfo.Motion.sSafeZone);
		pPos[1] = 0.0;

		double fLen, fVel, fAcc, fJerk;
		fLen = sqrt(((pPos[0] - dCurrX) * (pPos[0] - dCurrX)) + ((pPos[1] - dCurrY) * (pPos[1] - dCurrY)));
		if (fLen > 0.001)
		{
			m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_X1, fLen, fVel, fAcc, fJerk);
			if (!m_pMotion->Move(MS_X1Y1, pPos, fVel, fAcc, fAcc))
			{
				if (!m_pMotion->Move(MS_X1Y1, pPos, fVel, fAcc, fAcc))
				{
					pView->ClrDispMsg();
					AfxMessageBox(_T("Move X1Y1 Error..."));
				}
			}
		}
		if (!m_bTHREAD_MK[2])
		{
			m_nStepMk[2] = 0;
			m_nMkPcs[2] = 0;
			m_bTHREAD_MK[2] = TRUE;
		}
		else
		{
			m_bTHREAD_MK[2] = FALSE;
		}
	}
	else if (nCam == CAM_RT)
	{
		double dCurrX = m_dEnc[AXIS_X0];
		double dCurrY = m_dEnc[AXIS_Y0];

		double pPos[2];
		pPos[0] = 0.0;
		pPos[1] = 0.0;

		double fLen, fVel, fAcc, fJerk;
		fLen = sqrt(((pPos[0] - dCurrX) * (pPos[0] - dCurrX)) + ((pPos[1] - dCurrY) * (pPos[1] - dCurrY)));
		if (fLen > 0.001)
		{
			m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_X0, fLen, fVel, fAcc, fJerk);
			if (!m_pMotion->Move(MS_X0Y0, pPos, fVel, fAcc, fAcc))
			{
				if (!m_pMotion->Move(MS_X0Y0, pPos, fVel, fAcc, fAcc))
				{
					pView->ClrDispMsg();
					AfxMessageBox(_T("Move X0Y0 Error..."));
				}
			}
		}
		if (!m_bTHREAD_MK[3])
		{
			m_nStepMk[3] = 0;
			m_nMkPcs[3] = 0;
			m_bTHREAD_MK[3] = TRUE;
		}
		else
		{
			m_bTHREAD_MK[3] = FALSE;
		}
	}
	else if (nCam == CAM_BOTH)
	{
		if (!m_bTHREAD_MK[2])
		{
			m_nStepMk[2] = 0;
			m_nMkPcs[2] = 0;
			m_bTHREAD_MK[2] = TRUE;
		}
		else
		{
			m_bTHREAD_MK[2] = FALSE;
		}

		if (!m_bTHREAD_MK[3])
		{
			m_nStepMk[3] = 0;
			m_nMkPcs[3] = 0;
			m_bTHREAD_MK[3] = TRUE;
		}
		else
		{
			m_bTHREAD_MK[3] = FALSE;
		}
	}
}

void CManagerPunch::StopAllMk()
{
	// Mk0
	m_nStepMk[2] = MK_END;

	// Mk1
	m_nStepMk[3] = MK_END;
}

BOOL CManagerPunch::IsMk0Done()
{
	BOOL bDone = FALSE;

	if (m_pVoiceCoil[0])
		bDone = m_pVoiceCoil[0]->IsDoneMark(0);

	return bDone;
}

BOOL CManagerPunch::IsMk1Done()
{
	BOOL bDone = FALSE;

	if (m_pVoiceCoil[1])
		bDone = m_pVoiceCoil[1]->IsDoneMark(1);

	return bDone;
}

BOOL CManagerPunch::IsOnMarking0()
{
	if (m_nMkPcs[0] < pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)	// ��ŷ�Ϸ�Check
		return TRUE;

	return FALSE;
}

BOOL CManagerPunch::IsOnMarking1()
{
	if (m_nMkPcs[1] < pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)	// ��ŷ�Ϸ�Check
		return TRUE;

	return FALSE;
}

void CManagerPunch::DoMark0Its()
{
#ifdef TEST_MODE
	return;
#endif

	if (!m_bAuto)
		return;

	int nMaxStrip;
#ifdef USE_CAM_MASTER
	nMaxStrip = pView->m_mgrReelmap->m_Master[0].GetStripNum(); // �� ��Ʈ���� ����
#else
	nMaxStrip = MAX_STRIP;
#endif

	int nSerial, nIdx, nErrCode, nRtn;
	CfPoint ptPnt;
	CString sMsg;
	double dStripOut = (pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs / nMaxStrip) * _tstof(pDoc->WorkingInfo.LastJob.sStripOutRatio) / 100.0;
	int nStripOut = int(dStripOut);
	if (dStripOut > nStripOut)
		nStripOut++;			// ��Ʈ�� ���� ����

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (!pView->IsRun())																		// �������¿���
	{
		if (IsOnMarking0())																// ��ŷ�߿�
		{
			if (m_pMotion->IsEnable(MS_X0) && m_pMotion->IsEnable(MS_Y0))				// ����� Enable�����̰�
			{
				if (m_pMotion->IsMotionDone(MS_X0) && m_pMotion->IsMotionDone(MS_Y0))	// ��� Done�����̸�,
				{
					if (!IsInitPos0() && !IsPinPos0())									// �ʱ���ġ�� �ƴϰų�, ����ġ�� �ƴҶ�
						MoveInitPos0();													// �ʱ���ġ�� �̵�
				}
			}

			if (m_nStepMk[0] < 13 && m_nStepMk[0] > 8) // Mk0();
			{
				m_nStepMk[0] = 8;	// ��ŷ�Ϸ�Check
			}
		}
		return;
	}

	pView->m_mgrProcedure->m_sDispSts[0].Format(_T("%d"), m_nStepMk[0]);

	switch (m_nStepMk[0])
	{
	case 0:
		if (IsNoMk())
			pView->ShowLive();
		m_nStepMk[0]++;
		break;
	case 1:
		if (!IsInitPos0())
			MoveInitPos0();
		m_nStepMk[0]++;
		break;
	case 2:

		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Cam0

		if (nSerial > 0)
		{
			if ((nErrCode = pView->m_mgrReelmap->GetErrCode0Its(nSerial)) != 1)
			{
				m_nMkPcs[0] = pView->m_mgrReelmap->GetTotDefPcsIts(nSerial);
				m_nStepMk[0] = MK_END;
			}
			else
			{
				m_nStepMk[0]++;
			}
		}
		else
		{
			StopFromThread();
			pView->AsyncMsgBox(_T("������ �ø����� �����ʽ��ϴ�."), 1);
			BuzzerFromThread(TRUE, 0);
			DispMain(_T("�� ��"), RGB_RED);
		}
		break;
	case 3:
		m_nStepMk[0]++;
		break;
	case 4:
		m_nStepMk[0]++;
		break;
	case 5:
		m_nStepMk[0]++;
		break;
	case 6:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Cam0

		if (m_nMkPcs[0] < pView->m_mgrReelmap->GetTotDefPcsIts(nSerial))
		{
			if (!IsNoMk0())
			{
				;
			}
			else
			{
				if (!IsReview0())
				{
					if (m_bReview)
					{
						m_nMkPcs[0] = pView->m_mgrReelmap->GetTotDefPcsIts(nSerial);
						m_nStepMk[0] = MK_END;
						break;
					}
				}
			}
			// Punching On�̰ų� Review�̸� �������� ����
			SetDelay0(100, 1);		// [mSec]
			m_nStepMk[0]++;
		}
		else
		{
			m_nStepMk[0] = MK_END;
		}
		break;
	case 7:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Cam0

		if (!WaitDelay0(1))		// F:Done, T:On Waiting....		// Delay�Ŀ�
		{
			m_nMkPcs[0] = 0;

			if (!IsNoMk0())										// Punching On�̸�
			{
				m_nStepMk[0]++;
			}
			else												// Punching�� Off�̰�
			{
				if (IsReview0())								// Review�̸� ��������
				{
					m_nStepMk[0]++;
				}
				else											// Review�� �ƴϸ�
				{
					if (m_bReview)
					{
						m_nMkPcs[0] = pView->m_mgrReelmap->GetTotDefPcsIts(nSerial);
						m_nStepMk[0] = MK_END;
					}
					else
						m_nStepMk[0]++;
				}
			}
		}
		break;
	case 8:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Cam0

		if (m_nMkPcs[0] < pView->m_mgrReelmap->GetTotDefPcsIts(nSerial))	// ��ŷ�Ϸ�Check
		{
			if (m_nMkPcs[0] + 1 < pView->m_mgrReelmap->GetTotDefPcsIts(nSerial))		// ���� ��ŷ��ġ�� ������
			{
				ptPnt = pView->m_mgrReelmap->GetMkPntIts(nSerial, m_nMkPcs[0] + 1);	// ���� ��ŷ��ġ
				m_dNextTarget[AXIS_X0] = ptPnt.x;
				m_dNextTarget[AXIS_Y0] = ptPnt.y;
			}
			else												// ���� ��ŷ��ġ�� ������
			{
				m_dNextTarget[AXIS_X0] = -1.0;
				m_dNextTarget[AXIS_Y0] = -1.0;
			}

			ptPnt = pView->m_mgrReelmap->GetMkPntIts(nSerial, m_nMkPcs[0]);			// �̹� ��ŷ��ġ
			if (ptPnt.x < 0.0 && ptPnt.y < 0.0) // ��ǰȭ. (��ŷ���� ����)
			{
				m_nMkPcs[0]++;
				m_nStepMk[0] = MK_DONE_CHECK;
				break;
			}

			nIdx = pView->m_mgrReelmap->GetMkStripIdxIts(nSerial, m_nMkPcs[0]);		// 1 ~ 4 : strip index
			if (nIdx > 0)										// Strip index�� �����̸�,
			{
				if (!pView->IsMkStrip(nIdx)) // Strip[] Mk Off
				{
					m_nMkPcs[0]++;
					m_nStepMk[0] = MK_DONE_CHECK;
					break;
				}
				else
				{
					if (m_nMkStrip[0][nIdx - 1] >= nStripOut)
					{
						m_nMkPcs[0]++;
						m_nStepMk[0] = MK_DONE_CHECK;
						break;
					}
					else
						m_nMkStrip[0][nIdx - 1]++;
				}
			}
			else
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Strip Index Failed."));
				break;
			}

			m_dTarget[AXIS_X0] = ptPnt.x;
			m_dTarget[AXIS_Y0] = ptPnt.y;

			m_nStepMk[0]++;
		}
		else
		{
			m_nStepMk[0] = MK_END;
		}
		break;
	case 9:
		// Cam0 : m_bPriority[0] <-, m_bPriority[3] ->
		// Cam1 : m_bPriority[1] ->, m_bPriority[2] <-
		ptPnt.x = m_dTarget[AXIS_X0];
		ptPnt.y = m_dTarget[AXIS_Y0];

		if (m_dNextTarget[AXIS_X0] < 0)
			m_bCollision[0] = ChkCollision(AXIS_X0, m_dTarget[AXIS_X0]);
		else
			m_bCollision[0] = ChkCollision(AXIS_X0, m_dTarget[AXIS_X0], m_dNextTarget[AXIS_X0]);

		if (!m_bCollision[0])
		{
			if (IsMoveDone0())
			{
				Move0(ptPnt, m_bCam);
				m_nStepMk[0]++;
			}
		}
		else if (m_bPriority[0])
		{
			if (IsMoveDone0())
			{
				m_bCollision[0] = FALSE;
				m_bPriority[0] = FALSE;
				Move0(ptPnt, m_bCam);
				m_nStepMk[0]++;
			}
		}
		else if (m_bPriority[2])
		{
			if (IsMoveDone0())
			{
				m_bCollision[0] = FALSE;
				m_bPriority[2] = FALSE;
				ptPnt.x = 0.0;
				ptPnt.y = 0.0;//m_dEnc[AXIS_Y0];
				Move0(ptPnt, FALSE);
			}
		}
		break;
	case 10:
		m_nStepMk[0]++;
		break;
	case 11:
		m_nStepMk[0]++;
		break;
	case 12:
		if (IsMoveDone0())
			m_nStepMk[0]++;
		break;
	case 13:
		if (!IsNoMk0())
		{
			pView->m_mgrProcedure->m_dwStMkDn[0] = GetTickCount();
			Mk0();
		}
		else
		{
			// Verify - Mk0
			SetDelay0(pDoc->m_nDelayShow, 1);		// [mSec]
			if (!SaveMk0Img(m_nMkPcs[0]))
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Error-SaveMk0Img()"));
			}
		}
		m_nStepMk[0]++;
		break;
	case 14:
		m_nStepMk[0]++;
		break;
	case 15:
		m_nStepMk[0]++;
		break;
	case 16:
		if (IsNoMk0())
		{
			if (!WaitDelay0(1))		// F:Done, T:On Waiting....
				m_nStepMk[0]++;
		}
		else
			m_nStepMk[0]++;
		break;
	case 17:
		if (!IsNoMk0())
		{
			if (IsMk0Done())
			{
				// One more MK On Start....
				if (!m_nMkPcs[0] && !pView->m_mgrProcedure->m_bAnswer[2])
				{
					pView->m_mgrProcedure->m_bAnswer[2] = TRUE;
					Mk0();
				}
				else
				{
					pView->m_mgrProcedure->m_bAnswer[2] = FALSE;
					m_nMkPcs[0]++;
					m_nStepMk[0]++;
					m_nStepMk[0]++;
				}
			}
			else
			{
				if (pView->m_mgrProcedure->m_dwStMkDn[0] + 5000 < GetTickCount())
				{
					BuzzerFromThread(TRUE, 0);
					DispMain(_T("�� ��"), RGB_RED);
					m_pVoiceCoil[0]->SearchHomeSmac0();

					nRtn = pView->AsyncMsgBox(_T("���̽�����(��) ��ſϷᰡ �ʵ˴ϴ�.\r\n��ŷ�� �ٽ� �õ��Ͻðڽ��ϱ�?"), 1, MB_YESNO);
					if (IDYES == nRtn)
					{
						DispMain(_T("������"), RGB_RED);
						m_nStepMk[0] = 13;
					}
					else if (nRtn < 0)
						m_nStepMk[0]++; // Wait...
					else
					{
						pView->m_mgrProcedure->m_bAnswer[2] = FALSE;
						m_nMkPcs[0]++;
						m_nStepMk[0]++;
						m_nStepMk[0]++;
						Stop();
					}
				}
			}
		}
		else
		{
			m_nMkPcs[0]++;
			m_nStepMk[0]++;
			m_nStepMk[0]++;
		}
		break;
	case 18:
		if ((nRtn = WaitRtnVal(1)) > -1)
		{
			if (IDYES == nRtn)
			{
				DispMain(_T("������"), RGB_RED);
				m_nStepMk[0] = 13;
			}
			else
			{
				pView->m_mgrProcedure->m_bAnswer[2] = FALSE;
				m_nMkPcs[0]++;
				m_nStepMk[0]++;
				StopFromThread();
			}
		}
		break;
	case 19:
		m_nStepMk[0] = MK_DONE_CHECK;
		break;
	case MK_DONE_CHECK:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Cam0

		if (m_nMkPcs[0] < pView->m_mgrReelmap->GetTotDefPcsIts(nSerial))
		{
			if (IsNoMk0())
			{
				if (!pDoc->WorkingInfo.System.bNoMk)
				{
					if (IsReview0())
					{
						if (pView->IsJogRtDn0())
							m_nStepMk[0]++;
					}
					else if (!pDoc->WorkingInfo.LastJob.bVerify)
					{
						m_nMkPcs[0] = pView->m_mgrReelmap->GetTotDefPcsIts(nSerial);
						m_nStepMk[0] = MK_END;
					}
					else
						m_nStepMk[0] = 8;	// ��ŷ�Ϸ�Check
				}
				else
					m_nStepMk[0] = 8;	// ��ŷ�Ϸ�Check
			}
			else
				m_nStepMk[0] = 8;	// ��ŷ�Ϸ�Check
		}
		else
		{
			if (IsNoMk0())
			{
				if (IsReview0())
				{
					if (pView->IsJogRtDn0())
						m_nStepMk[0] = MK_END;
				}
				else
				{
					m_nStepMk[0] = MK_END;
				}
			}
			else
				m_nStepMk[0] = MK_END;
		}
		break;
	case MK_DONE_CHECK + 1:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Cam0

		if (m_nMkPcs[0] < pView->m_mgrReelmap->GetTotDefPcsIts(nSerial))
		{
			if (IsNoMk0())
			{
				if (IsReview0())
				{
					if (pView->IsJogRtUp0())
						m_nStepMk[0] = 8;	// ��ŷ�Ϸ�Check
				}
				else
				{
					m_nMkPcs[0] = pView->m_mgrReelmap->GetTotDefPcsIts(nSerial);
					m_nStepMk[0] = MK_END;
				}
			}
			else
				m_nStepMk[0] = 8;	// ��ŷ�Ϸ�Check
		}
		else
		{
			m_nStepMk[0] = MK_END;
		}
		break;

	case MK_END:
		if (IsMoveDone0())
			m_nStepMk[0]++;
		break;
	case 101:
		SetDelay0(100, 1);		// [mSec]
		m_nStepMk[0]++;
		break;
	case 102:
		if (!WaitDelay0(1))		// F:Done, T:On Waiting....
		{
			m_nStepMk[0]++;
		}
		break;
	case 103:
		if (!IsInitPos0())
		{
			m_dTarget[AXIS_X0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[0]);
			m_dTarget[AXIS_Y0] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[0]);
			m_dNextTarget[AXIS_X0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[0]);
			m_dNextTarget[AXIS_Y0] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[0]);

			MoveInitPos0();
		}

		pDoc->SaveMkCntL();
		m_nStepMk[0]++;
		break;
	case 104:
		if (IsMoveDone0())
		{
			m_nStepMk[0]++;
			SetDelay0(10000, 1);		// [mSec]
		}
		break;
	case 105:
		if (IsInitPos0())
		{
			m_nStepMk[0]++;
		}
		else
		{
			if (!WaitDelay0(1))		// F:Done, T:On Waiting....
			{
				m_nStepMk[0] = ERR_PROC;
			}
		}
		break;
	case 106: // MK Done....
		m_bDoneMk[0] = TRUE;
		m_bTHREAD_MK[0] = FALSE;
		break;

	case ERR_PROC:
		DispMain(_T("�� ��"), RGB_RED);
		m_pVoiceCoil[0]->SearchHomeSmac0();
		pView->AsyncMsgBox(_T("���̽�����(��) �ʱ���ġ �̵��� ���� �ʽ��ϴ�.\r\n��ŷ���¸� Ȯ���ϼ���."), 1);
		m_nStepMk[0]++;
		break;
	case ERR_PROC + 1:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0];

		if (m_bCam)
			sMsg.Format(_T("%d�� Shot�� �ٽ� �ҷ�Ȯ���� �Ͻðڽ��ϱ�?"), nSerial);
		else
			sMsg.Format(_T("%d�� Shot�� �ٽ� ��ŷ�Ͻðڽ��ϱ�?"), nSerial);

		pView->m_mgrProcedure->m_nRtnMyMsgBoxIdx = 0;
		pView->m_mgrProcedure->m_bRtnMyMsgBox[0] = FALSE;
		pView->m_mgrProcedure->m_nRtnMyMsgBox[0] = -1;
		pView->AsyncMsgBox(sMsg, 1, MB_YESNO);
		sMsg.Empty();
		m_nStepMk[0]++;
		break;
	case ERR_PROC + 2:
		if ((nRtn = WaitRtnVal()) > -1)
		{
			if (IDYES == nRtn)
			{
				m_nStepMk[0] = ERR_PROC + 10;
			}
			else
			{
				pView->m_mgrProcedure->m_nRtnMyMsgBoxIdx = 0;
				pView->m_mgrProcedure->m_bRtnMyMsgBox[0] = FALSE;
				pView->m_mgrProcedure->m_nRtnMyMsgBox[0] = -1;
				sMsg.Format(_T("��� ���� �۾��� �����Ͻðڽ��ϱ�?"), nSerial);
				pView->AsyncMsgBox(sMsg, 1, MB_YESNO);
				sMsg.Empty();

				m_nStepMk[0]++;
			}
		}
		break;
	case ERR_PROC + 3:
		if ((nRtn = WaitRtnVal()) > -1)
		{
			if (IDYES == nRtn)
			{
				m_nStepMk[0] = ERR_PROC + 20;
			}
			else
			{
				pView->m_bDispMsgDoAuto[8] = TRUE;
				pView->m_nStepDispMsg[8] = FROM_DOMARK0;
			}
		}
		break;
	case ERR_PROC + 10:
		m_bReMark[0] = TRUE;
		m_bTHREAD_MK[0] = FALSE;
		m_nStepMk[0] = 0;
		break;
	case ERR_PROC + 20: // MK Done....
		m_bDoneMk[0] = TRUE;
		m_bTHREAD_MK[0] = FALSE;
		break;
	}
}

void CManagerPunch::DoMark1Its()
{
#ifdef TEST_MODE
	return;
#endif

	if (!m_bAuto)
		return;

	int nMaxStrip;
#ifdef USE_CAM_MASTER
	nMaxStrip = pView->m_mgrReelmap->m_Master[0].GetStripNum(); // �� ��Ʈ���� ����
#else
	nMaxStrip = MAX_STRIP;
#endif

	//BOOL bOn;
	int nSerial, nIdx, nErrCode, nRtn;
	CfPoint ptPnt;
	CString sMsg;
	double dStripOut = (pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs / nMaxStrip) * _tstof(pDoc->WorkingInfo.LastJob.sStripOutRatio) / 100.0;
	int nStripOut = int(dStripOut);
	if (dStripOut > nStripOut)
		nStripOut++;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;


	if (!pView->IsRun())
	{
		if (IsOnMarking1())
		{
			if (m_pMotion->IsEnable(MS_X1) && m_pMotion->IsEnable(MS_Y1))
			{
				if (m_pMotion->IsMotionDone(MS_X1) && m_pMotion->IsMotionDone(MS_Y1))
				{
					if (!IsMkEdPos1() && !IsPinPos1())
						MoveMkEdPos1();
				}
			}

			if (m_nStepMk[1] < 13 && m_nStepMk[1] > 8) // Mk1();
			{
				m_nStepMk[1] = 8;	// ��ŷ�Ϸ�Check
			}
		}
		return;
	}


	pView->m_mgrProcedure->m_sDispSts[1].Format(_T("%d"), m_nStepMk[1]);

	switch (m_nStepMk[1])
	{
	case 0:
		if (IsNoMk())
			pView->ShowLive();
		m_nStepMk[1]++;
		break;
	case 1:
		if (!IsInitPos1())
			MoveInitPos1();
		m_nStepMk[1]++;
		break;
	case 2:

		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

		if (nSerial > 0)
		{
			if ((nErrCode = pView->m_mgrReelmap->GetErrCode1Its(nSerial)) != 1)
			{
				m_nMkPcs[1] = pView->m_mgrReelmap->GetTotDefPcsIts(nSerial);
				m_nStepMk[1] = MK_END;
			}
			else
			{
				m_nStepMk[1]++;
			}
		}
		else
		{
			StopFromThread();
			pView->AsyncMsgBox(_T("������ �ø����� �����ʽ��ϴ�."), 2);
			BuzzerFromThread(TRUE, 0);
			//pView->DispStsBar(_T("����-34"), 0);
			DispMain(_T("�� ��"), RGB_RED);
		}
		break;
	case 3:
		m_nStepMk[1]++;
		break;
	case 4:
		m_nStepMk[1]++;
		break;
	case 5:
		m_nStepMk[1]++;
		break;
	case 6:

		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

		if (m_nMkPcs[1] < pView->m_mgrReelmap->GetTotDefPcsIts(nSerial))
		{
			if (!IsNoMk1())
			{
				;
			}
			else
			{
				if (!IsReview1())
				{
					if (m_bReview)
					{
						m_nMkPcs[1] = pView->m_mgrReelmap->GetTotDefPcsIts(nSerial);
						m_nStepMk[1] = MK_END;
						break;
					}
				}
			}
			SetDelay1(100, 6);		// [mSec]
			m_nStepMk[1]++;
		}
		else
		{
			m_nStepMk[1] = MK_END;
		}
		break;
	case 7:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

		if (!WaitDelay1(6))		// F:Done, T:On Waiting....
		{
			m_nMkPcs[1] = 0;

			if (!IsNoMk1())
			{
				m_nStepMk[1]++;
			}
			else
			{
				if (IsReview1())
				{
					m_nStepMk[1]++;
				}
				else
				{
					if (m_bReview)
					{
						m_nMkPcs[1] = pView->m_mgrReelmap->GetTotDefPcsIts(nSerial);
						m_nStepMk[1] = MK_END;
					}
					else
						m_nStepMk[1]++;
				}
			}
		}
		break;
	case 8:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

		if (m_nMkPcs[1] < pView->m_mgrReelmap->GetTotDefPcsIts(nSerial))	// ��ŷ�Ϸ�Check
		{
			if (m_nMkPcs[1] + 1 < pView->m_mgrReelmap->GetTotDefPcsIts(nSerial))
			{
				ptPnt = pView->m_mgrReelmap->GetMkPntIts(nSerial, m_nMkPcs[1] + 1);
				m_dNextTarget[AXIS_X1] = ptPnt.x;
				m_dNextTarget[AXIS_Y1] = ptPnt.y;
			}
			else
			{
				m_dNextTarget[AXIS_X1] = -1.0;
				m_dNextTarget[AXIS_Y1] = -1.0;
			}

			ptPnt = pView->m_mgrReelmap->GetMkPntIts(nSerial, m_nMkPcs[1]);
			if (ptPnt.x < 0.0 && ptPnt.y < 0.0) // ��ǰȭ.
			{
				m_nMkPcs[1]++;
				m_nStepMk[1] = MK_DONE_CHECK;
				break;
			}

			nIdx = pView->m_mgrReelmap->GetMkStripIdxIts(nSerial, m_nMkPcs[1]);
			if (nIdx > 0)
			{
				if (!pView->IsMkStrip(nIdx)) // Strip[] Mk Off
				{
					m_nMkPcs[1]++;
					m_nStepMk[1] = MK_DONE_CHECK;
					break;
				}
				else
				{
					if (m_nMkStrip[1][nIdx - 1] >= nStripOut)
					{
						m_nMkPcs[1]++;
						m_nStepMk[1] = MK_DONE_CHECK;
						break;
					}
					else
						m_nMkStrip[1][nIdx - 1]++;
				}
			}
			else
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Strip Index Failed."));
				break;
			}

			m_dTarget[AXIS_X1] = ptPnt.x;
			m_dTarget[AXIS_Y1] = ptPnt.y;

			m_nStepMk[1]++;
		}
		else
		{
			m_nStepMk[1] = MK_END;
		}
		break;
	case 9:
		// Cam0 : m_bPriority[0] <-, m_bPriority[3] ->
		// Cam1 : m_bPriority[1] ->, m_bPriority[2] <-
		ptPnt.x = m_dTarget[AXIS_X1];
		ptPnt.y = m_dTarget[AXIS_Y1];

		if (m_dNextTarget[AXIS_X1] < 0)
			m_bCollision[1] = ChkCollision(AXIS_X1, m_dTarget[AXIS_X1]);
		else
			m_bCollision[1] = ChkCollision(AXIS_X1, m_dTarget[AXIS_X1], m_dNextTarget[AXIS_X1]);

		if (!m_bCollision[1])
		{
			if (IsMoveDone1())
			{
				Move1(ptPnt, m_bCam);
				m_nStepMk[1]++;
			}
		}
		else if (m_bPriority[1])
		{
			if (IsMoveDone1())
			{
				m_bCollision[1] = FALSE;
				m_bPriority[1] = FALSE;
				Move1(ptPnt, m_bCam);
				m_nStepMk[1]++;
			}
		}
		else if (m_bPriority[3])
		{
			if (IsMoveDone1())
			{
				m_bCollision[1] = FALSE;
				m_bPriority[3] = FALSE;
				ptPnt.x = _tstof(pDoc->WorkingInfo.Motion.sSafeZone);
				ptPnt.y = 0.0;//m_dEnc[AXIS_Y1];
				Move1(ptPnt, FALSE);
			}
		}
		break;
	case 10:
		m_nStepMk[1]++;
		break;
	case 11:
		m_nStepMk[1]++;
		break;
	case 12:
		if (IsMoveDone1())
			m_nStepMk[1]++;
		break;
	case 13:
		if (!IsNoMk1())
		{
			pView->m_mgrProcedure->m_dwStMkDn[1] = GetTickCount();
			Mk1();
		}
		else
		{
			// Verify - Mk1
			SetDelay1(pDoc->m_nDelayShow, 6);		// [mSec]
			if (!SaveMk1Img(m_nMkPcs[1]))
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Error-SaveMk1Img()"));
			}
		}
		m_nStepMk[1]++;
		break;
	case 14:
		m_nStepMk[1]++;
		break;
	case 15:
		m_nStepMk[1]++;
		break;
	case 16:
		if (IsNoMk1())
		{
			if (!WaitDelay1(6))		// F:Done, T:On Waiting....
				m_nStepMk[1]++;
		}
		else
			m_nStepMk[1]++;
		break;
	case 17:
		if (!IsNoMk1())
		{
			if (IsMk1Done())
			{
				// One more MK On Start....
				if (!m_nMkPcs[1] && !pView->m_mgrProcedure->m_bAnswer[3])
				{
					pView->m_mgrProcedure->m_bAnswer[3] = TRUE;
					Mk1();
				}
				else
				{
					pView->m_mgrProcedure->m_bAnswer[3] = FALSE;
					m_nMkPcs[1]++;
					m_nStepMk[1]++;
					m_nStepMk[1]++;
				}

			}
			else
			{
				if (pView->m_mgrProcedure->m_dwStMkDn[1] + 5000 < GetTickCount())
				{
					BuzzerFromThread(TRUE, 0);
					DispMain(_T("�� ��"), RGB_RED);
					m_pVoiceCoil[1]->SearchHomeSmac1();

					nRtn = pView->AsyncMsgBox(_T("���̽�����(��) ��ſϷᰡ �ʵ˴ϴ�.\r\n��ŷ�� �ٽ� �õ��Ͻðڽ��ϱ�?"), 2, MB_YESNO);
					if (IDYES == nRtn)
					{
						DispMain(_T("������"), RGB_RED);
						m_nStepMk[1] = 13;
					}
					else if (nRtn < 0)
						m_nStepMk[1]++; // Wait...
					else
					{
						pView->m_mgrProcedure->m_bAnswer[3] = FALSE;
						m_nMkPcs[1]++;
						m_nStepMk[1]++;
						m_nStepMk[1]++;
						Stop();
					}
				}
			}
		}
		else
		{
			m_nMkPcs[1]++;
			m_nStepMk[1]++;
			m_nStepMk[1]++;
		}
		break;
	case 18:
		if ((nRtn = WaitRtnVal(2)) > -1)
		{
			if (IDYES == nRtn)
			{
				DispMain(_T("������"), RGB_RED);
				m_nStepMk[1] = 13;
			}
			else
			{
				pView->m_mgrProcedure->m_bAnswer[3] = FALSE;
				m_nMkPcs[1]++;
				m_nStepMk[1]++;
				StopFromThread();
			}
		}
		break;
	case 19:
		m_nStepMk[1] = MK_DONE_CHECK;
		break;
	case MK_DONE_CHECK:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

		if (m_nMkPcs[1] < pView->m_mgrReelmap->GetTotDefPcsIts(nSerial))
		{
			if (IsNoMk1())
			{
				if (!pDoc->WorkingInfo.System.bNoMk)
				{
					if (IsReview1())
					{
						if (pView->IsJogRtDn1())
							m_nStepMk[1]++;
					}
					else if (!pDoc->WorkingInfo.LastJob.bVerify)
					{
						m_nMkPcs[1] = pView->m_mgrReelmap->GetTotDefPcsIts(nSerial);
						m_nStepMk[1] = MK_END;
					}
					else
						m_nStepMk[1] = 8;	// ��ŷ�Ϸ�Check
				}
				else
					m_nStepMk[1] = 8;	// ��ŷ�Ϸ�Check
			}
			else
				m_nStepMk[1] = 8;	// ��ŷ�Ϸ�Check
		}
		else
		{
			if (IsNoMk1())
			{
				if (IsReview1())
				{
					if (pView->IsJogRtDn1())
						m_nStepMk[1] = MK_END;
				}
				else
				{
					m_nStepMk[1] = MK_END;
				}
			}
			else
				m_nStepMk[1] = MK_END;
		}
		break;
	case MK_DONE_CHECK + 1:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

		if (m_nMkPcs[1] < pView->m_mgrReelmap->GetTotDefPcsIts(nSerial))
		{
			if (IsNoMk1())
			{
				if (IsReview1())
				{
					if (pView->IsJogRtUp1())
						m_nStepMk[1] = 8;	// ��ŷ�Ϸ�Check
				}
				else
				{
					m_nMkPcs[1] = pView->m_mgrReelmap->GetTotDefPcsIts(nSerial);
					m_nStepMk[1] = MK_END;
				}
			}
			else
				m_nStepMk[1] = 8;	// ��ŷ�Ϸ�Check
		}
		else
		{
			m_nStepMk[1] = MK_END;
		}
		break;


	case MK_END:
		if (IsMoveDone1())
			m_nStepMk[1]++;
		break;
	case 101:
		SetDelay1(100, 6);		// [mSec]
		m_nStepMk[1]++;
		break;
	case 102:
		if (!WaitDelay1(6))		// F:Done, T:On Waiting....
			m_nStepMk[1]++;
		break;
	case 103:
		if (!IsMkEdPos1())
		{
			m_dTarget[AXIS_X1] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[1]);
			m_dTarget[AXIS_Y1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[1]);
			m_dNextTarget[AXIS_X1] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[1]);
			m_dNextTarget[AXIS_Y1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[1]);

			MoveMkEdPos1();
		}

		pDoc->SaveMkCntR();
		m_nStepMk[1]++;
		break;
	case 104:
		if (IsMoveDone1())
		{
			m_nStepMk[1]++;
			SetDelay1(10000, 6);		// [mSec]
		}
		break;
	case 105:
		if (IsMkEdPos1())
		{
			m_nStepMk[1]++;
		}
		else
		{
			if (!WaitDelay1(6))		// F:Done, T:On Waiting....
			{
				m_nStepMk[1] = ERR_PROC;
			}
		}
		break;
	case 106: // MK Done....
		m_bDoneMk[1] = TRUE;
		m_bTHREAD_MK[1] = FALSE;
		break;

	case ERR_PROC:
		DispMain(_T("�� ��"), RGB_RED);
		m_pVoiceCoil[1]->SearchHomeSmac1();
		pView->AsyncMsgBox(_T("���̽�����(��) �ʱ���ġ �̵��� ���� �ʽ��ϴ�.\r\n��ŷ���¸� Ȯ���ϼ���."), 2);
		m_nStepMk[1]++;
		break;
	case ERR_PROC + 1:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

		if (m_bCam)
			sMsg.Format(_T("%d�� Shot�� �ٽ� �ҷ�Ȯ���� �Ͻðڽ��ϱ�?"), nSerial);
		else
			sMsg.Format(_T("%d�� Shot�� �ٽ� ��ŷ�Ͻðڽ��ϱ�?"), nSerial);

		pView->m_mgrProcedure->m_nRtnMyMsgBoxIdx = 1;
		pView->m_mgrProcedure->m_bRtnMyMsgBox[1] = FALSE;
		pView->m_mgrProcedure->m_nRtnMyMsgBox[1] = -1;
		pView->AsyncMsgBox(sMsg, 2, MB_YESNO);
		sMsg.Empty();
		m_nStepMk[1]++;
		break;
	case ERR_PROC + 2:
		if ((nRtn = WaitRtnVal()) > -1)
		{
			if (IDYES == nRtn)
			{
				m_nStepMk[1] = ERR_PROC + 10;
			}
			else
			{
				pView->m_mgrProcedure->m_nRtnMyMsgBoxIdx = 1;
				pView->m_mgrProcedure->m_bRtnMyMsgBox[1] = FALSE;
				pView->m_mgrProcedure->m_nRtnMyMsgBox[1] = -1;
				sMsg.Format(_T("��� ���� �۾��� �����Ͻðڽ��ϱ�?"), nSerial);
				pView->AsyncMsgBox(sMsg, 2, MB_YESNO);
				sMsg.Empty();

				m_nStepMk[1]++;

			}
		}
		break;
	case ERR_PROC + 3:
		if ((nRtn = WaitRtnVal()) > -1)
		{
			if (IDYES == nRtn)
			{
				m_nStepMk[1] = ERR_PROC + 20;
			}
			else
			{
				pView->m_bDispMsgDoAuto[9] = TRUE;
				pView->m_nStepDispMsg[9] = FROM_DOMARK1;
			}
		}
		break;
	case ERR_PROC + 10:
		m_bReMark[1] = TRUE;
		m_bTHREAD_MK[1] = FALSE;
		m_nStepMk[1] = 0;
		break;
	case ERR_PROC + 20: // MK Done....
		m_bDoneMk[1] = TRUE;
		m_bTHREAD_MK[1] = FALSE;
		break;
	}
}

void CManagerPunch::DoReject0()
{
#ifdef TEST_MODE
	return;
#endif

	//BOOL bOn;
	int nIdx, nRtn;//nSerial, 
	CfPoint ptPnt;

	if (!pView->IsRun() && m_bAuto)
	{
		if (IsOnMarking0())
		{
			if (m_pMotion->IsEnable(MS_X0) && m_pMotion->IsEnable(MS_Y0))
			{
				if (m_pMotion->IsMotionDone(MS_X0) && m_pMotion->IsMotionDone(MS_Y0))
				{
					if (!IsInitPos0() && !IsPinPos0())
						MoveInitPos0();
				}
			}
		}
		return;
	}


	switch (m_nStepMk[2])
	{
	case 0:
		if (IsNoMk())
			pView->ShowLive();
		for (nIdx = 0; nIdx < MAX_STRIP; nIdx++)
		{
			m_nMkStrip[0][nIdx] = 0;
			m_bRejectDone[0][nIdx] = FALSE;
		}
		m_nStepMk[2]++;
		break;
	case 1:
		if (m_nMkPcs[2] < pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)
		{
			m_nStepMk[2]++;
		}
		else
		{
			m_nStepMk[2] = MK_END;
		}
		break;
	case 2:
		m_nStepMk[2]++;
		break;
	case 3:
		if (m_nMkPcs[2] < pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)	// ��ŷ�Ϸ�Check
		{
			ptPnt = pView->GetMkPnt(m_nMkPcs[2]);

			if (ptPnt.x < 0.0 && ptPnt.y < 0.0) // ��ǰȭ.
			{
				m_nMkPcs[2]++;
				break;
			}

			nIdx = pView->GetMkStripIdx(m_nMkPcs[2]);
			if (nIdx > 0)
			{
				if (!pView->IsMkStrip(nIdx)) // Strip[] Mk Off
				{
					m_nMkPcs[2]++;
					break;
				}
				else
				{
					if (m_nMkStrip[0][nIdx - 1] > pDoc->GetStripRejectMkNum())
					{
						m_bRejectDone[0][nIdx - 1] = TRUE;
						m_nMkPcs[2]++;
						break;
					}
					else
						m_nMkStrip[0][nIdx - 1]++;
				}
			}
			else
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Strip Index Failed."));
				break;
			}

			if (m_bRejectDone[0][0] && m_bRejectDone[0][1] &&
				m_bRejectDone[0][2] && m_bRejectDone[0][3])
			{
				m_nStepMk[2] = MK_END;
				break;
			}

			m_dTarget[AXIS_X0] = ptPnt.x;

			// Cam0 : m_bPriority[0], m_bPriority[3]
			// Cam1 : m_bPriority[1], m_bPriority[2]

			m_bCollision[0] = ChkCollision(AXIS_X0, ptPnt.x);
			if (!m_bCollision[0])
			{
				Move0(ptPnt, m_bCam);
				m_nStepMk[2]++;
			}
			else if (m_bPriority[0])
			{
				m_bPriority[0] = FALSE;
				Move0(ptPnt, m_bCam);
				m_nStepMk[2]++;
			}
			else if (m_bPriority[2])
			{
				m_bPriority[2] = FALSE;
				ptPnt.x = 0.0;
				ptPnt.y = 0.0;//m_dEnc[AXIS_Y0];
				Move0(ptPnt, m_bCam);
			}
		}
		else
		{
			m_nStepMk[2] = MK_END;
		}
		break;
	case 4:
		m_nStepMk[2]++;
		break;
	case 5:
		m_nStepMk[2]++;
		break;
	case 6:
		if (IsMoveDone0())
			m_nStepMk[2]++;
		break;
	case 7:
		if (!IsNoMk0())
		{
			pView->m_mgrProcedure->m_dwStMkDn[0] = GetTickCount();
			Mk0();
		}
		else
			SetDelay0(pDoc->m_nDelayShow, 1);		// [mSec]
		m_nStepMk[2]++;
		break;
	case 8:
		if (IsNoMk0())
		{
			if (!WaitDelay0(1))		// F:Done, T:On Waiting....
				m_nStepMk[2]++;
		}
		else
			m_nStepMk[2]++;
		break;
	case 9:
		m_nStepMk[2]++;
		break;
	case 10:
		if (!IsNoMk0())
		{
			//Mk0(FALSE);
			if (IsMk0Done())
			{
				m_nMkPcs[2]++;
				m_nStepMk[2]++;
				m_nStepMk[2]++;
			}
			else
			{
				if (pView->m_mgrProcedure->m_dwStMkDn[0] + 5000 < GetTickCount())
				{
					Buzzer(TRUE, 0);
					//pView->DispStsBar(_T("����-29"), 0);
					DispMain(_T("�� ��"), RGB_RED);
					m_pVoiceCoil[0]->SearchHomeSmac0();

					nRtn = pView->AsyncMsgBox(_T("���̽�����(��) ��ſϷᰡ �ʵ˴ϴ�.\r\n��ŷ�� �ٽ� �õ��Ͻðڽ��ϱ�?"), 1, MB_YESNO);
					if (IDYES == nRtn)
					{
						DispMain(_T("������"), RGB_RED);
						m_nStepMk[2] = 7;
					}
					else if (nRtn < 0)
						m_nStepMk[2]++;
					else
					{
						m_nMkPcs[2]++;
						m_nStepMk[2]++;
						m_nStepMk[2]++;
						Stop();
					}
				}
			}
		}
		else
		{
			m_nMkPcs[2]++;
			m_nStepMk[2]++;
			m_nStepMk[2]++;
		}
		break;
	case 11:
		//m_nStepMk[2]++;
		//if(m_bRtnMyMsgBox[2])
		if ((nRtn = WaitRtnVal(3)) > -1)
		{
			//if(IDYES == m_nRtnMyMsgBox[2])
			if (IDYES == nRtn)
			{
				DispMain(_T("������"), RGB_RED);
				m_nStepMk[2] = 7;
			}
			else
			{
				m_nMkPcs[2]++;
				m_nStepMk[2]++;
				Stop();
			}
		}
		break;
	case 12:
		m_nStepMk[2]++;
		break;
	case 13:
		if (m_nMkPcs[2] < pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)	// ��ŷ�Ϸ�Check
		{
			m_nStepMk[2] = 3;
		}
		else
		{
			m_nStepMk[2] = MK_END;
		}
		break;
	case MK_END:
		m_pVoiceCoil[0]->SearchHomeSmac0();
		SetDelay0(500, 1);		// [mSec]
		m_nStepMk[2]++;
		break;
	case 101:
		if (!WaitDelay0(1))		// F:Done, T:On Waiting....
		{
			MoveInitPos0();
			m_nStepMk[2]++;
		}
		break;
	case 102:
		if (IsMoveDone0())
		{
			m_bTHREAD_MK[2] = FALSE;
			m_bDoneMk[0] = TRUE;
			m_nStepMk[2] = 0;
			for(nIdx=0; nIdx<MAX_STRIP; nIdx++)
				m_nMkStrip[0][nIdx] = 0;
		}
		break;
	}
}

void CManagerPunch::DoReject1()
{
#ifdef TEST_MODE
	return;
#endif

	//BOOL bOn;
	int nIdx, nRtn;//nSerial, 
	CfPoint ptPnt;

	if (!pView->IsRun() && m_bAuto)
	{
		if (IsOnMarking1())
		{
			if (m_pMotion->IsEnable(MS_X1) && m_pMotion->IsEnable(MS_Y1))
			{
				if (m_pMotion->IsMotionDone(MS_X1) && m_pMotion->IsMotionDone(MS_Y1))
				{
					if (!IsMkEdPos1() && !IsPinPos1())
						MoveMkEdPos1();
				}
			}
		}
		return;
	}


	switch (m_nStepMk[3])
	{
	case 0:
		if (IsNoMk())
			pView->ShowLive();
		for (nIdx = 0; nIdx < MAX_STRIP; nIdx++)
		{
			m_nMkStrip[1][nIdx] = 0;
			m_bRejectDone[1][nIdx] = FALSE;
		}
		m_nStepMk[3]++;
		break;
	case 1:
		if (m_nMkPcs[3] < pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)
		{
			m_nStepMk[3]++;
		}
		else
		{
			m_nStepMk[3] = MK_END;
		}
		break;
	case 2:
		m_nStepMk[3]++;
		break;
	case 3:
		if (m_nMkPcs[3] < pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)	// ��ŷ�Ϸ�Check
		{
			ptPnt = pView->GetMkPnt(m_nMkPcs[3]);

			if (ptPnt.x < 0.0 && ptPnt.y < 0.0) // ��ǰȭ.
			{
				m_nMkPcs[3]++;
				break;
			}

			nIdx = pView->GetMkStripIdx(m_nMkPcs[3]);
			if (nIdx > 0)
			{
				if (!pView->IsMkStrip(nIdx)) // Strip[] Mk Off
				{
					m_nMkPcs[3]++;
					break;
				}
				else
				{
					if (m_nMkStrip[1][nIdx - 1] > pDoc->GetStripRejectMkNum())
					{
						m_bRejectDone[1][nIdx - 1] = TRUE;
						m_nMkPcs[3]++;
						break;
					}
					else
						m_nMkStrip[1][nIdx - 1]++;
				}
			}
			else
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Strip Index Failed."));
				break;
			}

			if (m_bRejectDone[1][0] && m_bRejectDone[1][1] &&
				m_bRejectDone[1][2] && m_bRejectDone[1][3])
			{
				m_nStepMk[2] = MK_END;
				break;
			}

			m_dTarget[AXIS_X1] = ptPnt.x;

			// Cam0 : m_bPriority[0], m_bPriority[3]
			// Cam1 : m_bPriority[1], m_bPriority[2]

			m_bCollision[1] = ChkCollision(AXIS_X1, ptPnt.x);
			if (!m_bCollision[1])
			{
				Move1(ptPnt, m_bCam);
				m_nStepMk[3]++;
			}
			else if (m_bPriority[1])
			{
				m_bPriority[1] = FALSE;
				Move1(ptPnt, m_bCam);
				m_nStepMk[3]++;
			}
			else if (m_bPriority[3])
			{
				m_bPriority[3] = FALSE;
				ptPnt.x = _tstof(pDoc->WorkingInfo.Motion.sSafeZone);
				ptPnt.y = 0.0;//m_dEnc[AXIS_Y1];
				Move1(ptPnt, m_bCam);
			}
		}
		else
		{
			m_nStepMk[3] = MK_END;
		}
		break;
	case 4:
		m_nStepMk[3]++;
		break;
	case 5:
		m_nStepMk[3]++;
		break;
	case 6:
		if (IsMoveDone1())
			m_nStepMk[3]++;
		break;
	case 7:
		if (!IsNoMk1())
		{
			pView->m_mgrProcedure->m_dwStMkDn[1] = GetTickCount();
			Mk1();
		}
		else
			SetDelay1(pDoc->m_nDelayShow, 1);		// [mSec]
		m_nStepMk[3]++;
		break;
	case 8:
		if (IsNoMk1())
		{
			if (!WaitDelay1(1))		// F:Done, T:On Waiting....
				m_nStepMk[3]++;
		}
		else
			m_nStepMk[3]++;
		break;
	case 9:
		m_nStepMk[3]++;
		break;
	case 10:
		if (!IsNoMk1())
		{
			if (IsMk1Done())
			{
				m_nMkPcs[3]++;
				m_nStepMk[3]++;
				m_nStepMk[3]++;
			}
			else
			{
				if (pView->m_mgrProcedure->m_dwStMkDn[1] + 5000 < GetTickCount())
				{
					Buzzer(TRUE, 0);
					//pView->DispStsBar(_T("����-30"), 0);
					DispMain(_T("�� ��"), RGB_RED);
					m_pVoiceCoil[1]->SearchHomeSmac1();

					nRtn = pView->AsyncMsgBox(_T("���̽�����(��) ��ſϷᰡ �ʵ˴ϴ�.\r\n��ŷ�� �ٽ� �õ��Ͻðڽ��ϱ�?"), 2, MB_YESNO);
					if (IDYES == nRtn)
					{
						DispMain(_T("������"), RGB_RED);
						m_nStepMk[3] = 7;
					}
					else if (nRtn < 0)
						m_nStepMk[3]++;
					else
					{
						m_nMkPcs[3]++;
						m_nStepMk[3]++;
						m_nStepMk[3]++;
						Stop();
					}
				}
			}
		}
		else
		{
			m_nMkPcs[3]++;
			m_nStepMk[3]++;
			m_nStepMk[3]++;
		}
		break;
	case 11:
		//m_nStepMk[3]++;
		//if(m_bRtnMyMsgBox[3])
		if ((nRtn = WaitRtnVal(4)) > -1)
		{
			//if(IDYES == m_nRtnMyMsgBox[3])
			if (IDYES == nRtn)
			{
				DispMain(_T("������"), RGB_RED);
				m_nStepMk[3] = 7;
			}
			else
			{
				m_nMkPcs[3]++;
				m_nStepMk[3]++;
				Stop();
			}
		}
		break;
	case 12:
		m_nStepMk[3]++;
		break;
	case 13:
		if (m_nMkPcs[3] < pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)	// ��ŷ�Ϸ�Check
		{
			m_nStepMk[3] = 3;
		}
		else
		{
			m_nStepMk[3] = MK_END;
		}
		break;
	case MK_END:
		m_pVoiceCoil[1]->SearchHomeSmac1();
		SetDelay1(500, 1);		// [mSec]
		m_nStepMk[3]++;
		break;
	case 101:
		if (!WaitDelay1(1))		// F:Done, T:On Waiting....
		{
			MoveMkEdPos1();
			m_nStepMk[3]++;
		}
		break;
	case 102:
		if (IsMoveDone1())
		{
			m_bTHREAD_MK[3] = FALSE;
			m_bDoneMk[1] = TRUE;
			m_nStepMk[3] = 0;
			for (nIdx = 0; nIdx < MAX_STRIP; nIdx++)
				m_nMkStrip[1][nIdx] = 0;
		}
		break;
	}
}


//===> TCP/IP
BOOL CManagerPunch::TcpIpInit()
{
#ifdef USE_SR1000W
	if (!m_pSr1000w)
	{
		m_pSr1000w = new CSr1000w(pDoc->WorkingInfo.System.sIpClient[ID_SR1000W], pDoc->WorkingInfo.System.sIpServer[ID_SR1000W], pDoc->WorkingInfo.System.sPort[ID_SR1000W], this);
		//m_pSr1000w->SetHwnd(this->GetSafeHwnd());
	}
#endif	

//#ifdef USE_TCPIP
//	if (!m_pEngrave)
//	{
//		m_pEngrave = new CEngrave(pDoc->WorkingInfo.System.sIpClient[ID_PUNCH], pDoc->WorkingInfo.System.sIpServer[ID_ENGRAVE], pDoc->WorkingInfo.System.sPort[ID_ENGRAVE], this);
//		m_pEngrave->SetHwnd(this->GetSafeHwnd());
//	}
//#endif

	return TRUE;
}

void CManagerPunch::DtsInit()
{
#ifdef USE_DTS
	if (pDoc->m_bUseDts)
	{
		if (!m_pDts)
		{
			m_pDts = new CDts(this);
		}
	}
#endif
}


//==>Etc
int CManagerPunch::MsgBox(CString sMsg, int nThreadIdx, int nType, int nTimOut, BOOL bEngave)
{
	return pView->MsgBox(sMsg, nThreadIdx, nType, nTimOut, bEngave);
}

void CManagerPunch::DoModeSel()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)
		return;

	BOOL bMode;// [2];
	bMode = pDoc->m_pMpeIo[36] & (0x01 << 5) ? TRUE : FALSE;	// ��ŷ�� �ڵ� ���� ����ġ ����

	if (bMode)
	{
		pDoc->Status.bAuto = TRUE;
		pDoc->Status.bManual = FALSE;
		pDoc->Status.bOneCycle = FALSE;

		m_bAuto = TRUE;
		m_bManual = FALSE;
		m_bOneCycle = FALSE;
	}
	else
	{
		pDoc->Status.bManual = TRUE;
		pDoc->Status.bAuto = FALSE;
		pDoc->Status.bOneCycle = FALSE;

		m_bManual = TRUE;
		m_bAuto = FALSE;
		m_bOneCycle = FALSE;
	}

#else
	pDoc->Status.bAuto = FALSE;
	pDoc->Status.bManual = TRUE;
	pDoc->Status.bOneCycle = FALSE;
#endif

}

void CManagerPunch::DoMainSw()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIb || !pDoc->m_pMpeIF)
		return;

	unsigned short usIn = pDoc->m_pMpeIb[4];
	unsigned short *usInF = &pDoc->m_pMpeIF[4];

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
	{
		*usInF |= (0x01 << 0);								// ��ŷ�� ������� ����ġ(����ͺ�)
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
	}

	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
	{
		*usInF |= (0x01 << 1);								// ��ŷ�� ���� ����ġ
		m_bSwStopNow = FALSE;
		if (pView->m_pDlgMenu03)
			pView->m_pDlgMenu03->SwRun();
	}
	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
	{
		*usInF &= ~(0x01 << 1);
	}

	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
	{
		*usInF |= (0x01 << 2);								// ��ŷ�� ���� ����ġ
	}
	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
	{
		*usInF &= ~(0x01 << 2);
	}

	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
	{
		*usInF |= (0x01 << 3);								// ��ŷ�� �����غ� ����ġ
		m_bSwStopNow = FALSE;
		if (pView->m_pDlgMenu03)
			pView->m_pDlgMenu03->SwReady();
	}
	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
	{
		*usInF &= ~(0x01 << 3);
	}

	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// ��ŷ�� ���� ����ġ
		m_bSwStopNow = FALSE;
		if (pView->m_pDlgMenu03)
			pView->m_pDlgMenu03->SwReset();
}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
	}

	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
	{
		*usInF |= (0x01 << 7);								// ��ŷ�� JOG ��ư(��)
		if (pDoc->Status.bSwJogLeft)
			SwJog(AXIS_Y0, M_CCW, TRUE);
		else
			SwJog(AXIS_Y1, M_CCW, TRUE);
	}
	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
	{
		*usInF &= ~(0x01 << 7);
		if (pDoc->Status.bSwJogLeft)
			SwJog(AXIS_Y0, M_CCW, FALSE);
		else
			SwJog(AXIS_Y1, M_CCW, FALSE);
	}

	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
	{
		*usInF |= (0x01 << 8);								// ��ŷ�� JOG ��ư(��)
		if (pDoc->Status.bSwJogLeft)
			SwJog(AXIS_Y0, M_CW, TRUE);
		else
			SwJog(AXIS_Y1, M_CW, TRUE);
	}
	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
	{
		*usInF &= ~(0x01 << 8);
		if (pDoc->Status.bSwJogLeft)
			SwJog(AXIS_Y0, M_CW, FALSE);
		else
			SwJog(AXIS_Y1, M_CW, FALSE);
	}

	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
	{
		*usInF |= (0x01 << 9);								// ��ŷ�� JOG ��ư(��)
		if (pDoc->Status.bSwJogLeft)
			SwJog(AXIS_X0, M_CCW, TRUE);
		else
			SwJog(AXIS_X1, M_CCW, TRUE);
	}
	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
	{
		*usInF &= ~(0x01 << 9);
		if (pDoc->Status.bSwJogLeft)
			SwJog(AXIS_X0, M_CCW, FALSE);
		else
			SwJog(AXIS_X1, M_CCW, FALSE);
	}

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
	{
		*usInF |= (0x01 << 10);								// ��ŷ�� JOG ��ư(��)
		if (pDoc->Status.bSwJogLeft)
			SwJog(AXIS_X0, M_CW, TRUE);
		else
			SwJog(AXIS_X1, M_CW, TRUE);
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
	{
		*usInF &= ~(0x01 << 10);
		if (pDoc->Status.bSwJogLeft)
			SwJog(AXIS_X0, M_CW, FALSE);
		else
			SwJog(AXIS_X1, M_CW, FALSE);
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
	{
		*usInF |= (0x01 << 11);								// ��ŷ�� ��� ����(LEFT)
		pDoc->Status.bSwJogLeft = TRUE;
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
	{
		*usInF &= ~(0x01 << 11);
		pDoc->Status.bSwJogLeft = FALSE;
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
	{
		*usInF |= (0x01 << 12);								// ��ŷ�� �ӵ� ����
		pDoc->Status.bSwJogFast = TRUE;
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
	{
		*usInF &= ~(0x01 << 12);
		pDoc->Status.bSwJogFast = FALSE;
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
	{
		*usInF |= (0x01 << 13);								// ��ŷ�� ���� ����(INDEX)
		pDoc->Status.bSwJogStep = TRUE;
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
	{
		*usInF &= ~(0x01 << 13);
		pDoc->Status.bSwJogStep = FALSE;
	}

	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))
	{
		*usInF |= (0x01 << 14);								// SPARE	
															// No Use....
	}
	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))
	{
		*usInF &= ~(0x01 << 14);
		// No Use....
	}

	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
	{
		*usInF |= (0x01 << 15);								// SPARE	
															// No Use....
	}
	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
	{
		*usInF &= ~(0x01 << 15);
		// No Use....
	}

	if (!pDoc->m_pMpeIo)// || !pDoc->m_pMpeIoF)
		return;

#endif
}

void CManagerPunch::DoMkSens()
{
	unsigned short usIn = 0;
	unsigned short *usInF = NULL;

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
	{
		*usInF |= (0x01 << 0);								// ��ŷ�� ���̺� ���� ����
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
	}

	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
	{
		*usInF |= (0x01 << 1);								// ��ŷ�� ���̺� �з� ����ġ
	}
	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
	{
		*usInF &= ~(0x01 << 1);
	}

	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
	{
		*usInF |= (0x01 << 2);								// ��ŷ�� �ǵ� ���� ����
	}
	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
	{
		*usInF &= ~(0x01 << 2);
	}

	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
	{
		*usInF |= (0x01 << 3);								// ��ŷ�� �ǵ� Ŭ���� ��� ����
	}
	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
	{
		*usInF &= ~(0x01 << 3);
	}

	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// ��ŷ�� �ǵ� Ŭ���� �ϰ� ����
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
	}

	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
	{
		*usInF |= (0x01 << 5);								// ��ŷ�� ��ũ ���� ����
	}
	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
	{
		*usInF &= ~(0x01 << 5);
	}

	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
	{
		*usInF |= (0x01 << 6);								// ��ŷ�� ��ũ Ŭ���� ��� ����
	}
	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
	{
		*usInF &= ~(0x01 << 6);
	}

	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
	{
		*usInF |= (0x01 << 7);								// ��ŷ�� ��ũ Ŭ���� �ϰ� ����
	}
	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
	{
		*usInF &= ~(0x01 << 7);
	}

	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
	{
		*usInF |= (0x01 << 8);								// ��ŷ�� ���� ��� ����
	}
	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
	{
		*usInF &= ~(0x01 << 8);
	}

	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
	{
		*usInF |= (0x01 << 9);								// ��ŷ�� ���� �ϰ� ����
	}
	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
	{
		*usInF &= ~(0x01 << 9);
	}

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
	{
		*usInF |= (0x01 << 10);								// �˻�� �ǵ� ���� ����
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
	{
		*usInF &= ~(0x01 << 10);
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
	{
		*usInF |= (0x01 << 11);								// �˻�� �ǵ� Ŭ���� ��� ����
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
	{
		*usInF &= ~(0x01 << 11);
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
	{
		*usInF |= (0x01 << 12);								// �˻�� �ǵ� Ŭ���� �ϰ� ����
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
	{
		*usInF &= ~(0x01 << 12);
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
	{
		*usInF |= (0x01 << 13);								// �˻�� ��ũ ���� ����
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
	{
		*usInF &= ~(0x01 << 13);
	}

	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))
	{
		*usInF |= (0x01 << 14);								// �˻�� ��ũ Ŭ���� ��� ����
	}
	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))
	{
		*usInF &= ~(0x01 << 14);
	}

	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
	{
		*usInF |= (0x01 << 15);								// �˻�� ��ũ Ŭ���� �ϰ� ����
	}
	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
	{
		*usInF &= ~(0x01 << 15);
	}
}

void CManagerPunch::DoAoiBoxSw()
{
	unsigned short usIn = 0;
	unsigned short *usInF = NULL;

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
	{
		*usInF |= (0x01 << 0);								// �˻�� ���� ��/���� ����ġ
		SwAoiRelation();
		//if (pView->m_pDlgMenu03)
		//	pView->m_pDlgMenu03->SwAoiRelation();
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
	}

	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
	{
		*usInF |= (0x01 << 1);								// �˻�� ���̺� ��ο� ����ġ
		SwAoiTblBlw();
		//if (pView->m_pDlgMenu03)
		//	pView->m_pDlgMenu03->SwAoiTblBlw();
	}
	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
	{
		*usInF &= ~(0x01 << 1);
	}

	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
	{
		*usInF |= (0x01 << 2);								// �˻�� �ǵ� ��ȸ�� ����ġ
	}
	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
	{
		*usInF &= ~(0x01 << 2);
	}

	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
	{
		*usInF |= (0x01 << 3);								// �˻�� �ǵ� ��ȸ�� ����ġ
	}
	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
	{
		*usInF &= ~(0x01 << 3);
	}

	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// �˻�� �ǵ� ���� ����ġ
		SwAoiFdVac();
		//if (pView->m_pDlgMenu03)
		//	pView->m_pDlgMenu03->SwAoiFdVac();
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
	}

	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
	{
		*usInF |= (0x01 << 5);								// �˻�� ��ũ ���� ����ġ
		SwAoiTqVac();
		//if (pView->m_pDlgMenu03)
		//	pView->m_pDlgMenu03->SwAoiTqVac();
	}
	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
	{
		*usInF &= ~(0x01 << 5);
	}

	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
	{
		*usInF |= (0x01 << 6);								// �˻�� ���̺� ���� ����ġ
		if (pView->m_pDlgMenu03)
			pView->m_pDlgMenu03->SwAoiTblVac();
	}
	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
	{
		*usInF &= ~(0x01 << 6);
	}

	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
	{
		*usInF |= (0x01 << 7);								// �˻�� ������ ������ ����ġ
		if (pView->m_pDlgMenu03)
			pView->m_pDlgMenu03->SwAoiLsrPt(TRUE);
	}
	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
	{
		*usInF &= ~(0x01 << 7);
		if (pView->m_pDlgMenu03)
			pView->m_pDlgMenu03->SwAoiLsrPt(FALSE);
	}

	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
	{
		*usInF |= (0x01 << 8);								// �˻�� �ǵ� Ŭ���� ����ġ
		if (pView->m_pDlgMenu03)
			pView->m_pDlgMenu03->SwAoiFdClp();
	}
	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
	{
		*usInF &= ~(0x01 << 8);
	}

	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
	{
		*usInF |= (0x01 << 9);								// �˻�� ��ũ Ŭ���� ����ġ
		if (pView->m_pDlgMenu03)
			pView->m_pDlgMenu03->SwAoiTqClp();
	}
	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
	{
		*usInF &= ~(0x01 << 9);
	}
}

void CManagerPunch::DoEmgSens()
{
#ifdef USE_MPE
	unsigned short usIn, *usInF;

	if (!pDoc->m_pMpeIb || !pDoc->m_pMpeIF)
		return;

	usIn = pDoc->m_pMpeIb[0];
	usInF = &pDoc->m_pMpeIF[0];

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))		// �����Ϸ� ������� ����ġ
	{
		*usInF |= (0x01 << 0);
		pDoc->Status.bEmgUc = TRUE;
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
		pDoc->Status.bEmgUc = FALSE;
	}

	usIn = pDoc->m_pMpeIb[4];
	usInF = &pDoc->m_pMpeIF[4];

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))		// ��ŷ�� ������� ����ġ(����ͺ�)
	{
		*usInF |= (0x01 << 0);
		pDoc->Status.bEmgMk[EMG_M_MK] = TRUE;
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
		pDoc->Status.bEmgMk[EMG_M_MK] = FALSE;
	}

	usIn = pDoc->m_pMpeIb[5];
	usInF = &pDoc->m_pMpeIF[5];

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))		// ��ŷ�� ������� ����ġ(����ġ��)	
	{
		*usInF |= (0x01 << 0);
		pDoc->Status.bEmgMk[EMG_B_MK] = TRUE;
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
		pDoc->Status.bEmgMk[EMG_B_MK] = FALSE;
	}

	usIn = pDoc->m_pMpeIb[8];
	usInF = &pDoc->m_pMpeIF[8];

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))		// �˻�� �� ������� ����ġ(�ĸ�) 
	{
		*usInF |= (0x01 << 0);
		pDoc->Status.bEmgAoi[EMG_B_AOI_UP] = TRUE;
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
		pDoc->Status.bEmgAoi[EMG_B_AOI_UP] = FALSE;
	}

	usIn = pDoc->m_pMpeIb[12];
	usInF = &pDoc->m_pMpeIF[12];

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))		// �˻�� �� ������� ����ġ(�ĸ�) 
	{
		*usInF |= (0x01 << 0);
		pDoc->Status.bEmgAoi[EMG_B_AOI_DN] = TRUE;
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
		pDoc->Status.bEmgAoi[EMG_B_AOI_DN] = FALSE;
	}

	usIn = pDoc->m_pMpeIb[16];
	usInF = &pDoc->m_pMpeIF[16];

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))		// �����Ϸ� ������� ����ġ
	{
		*usInF |= (0x01 << 0);
		pDoc->Status.bEmgRc = TRUE;
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
		pDoc->Status.bEmgRc = FALSE;
	}
#endif
}

void CManagerPunch::DoSignal()
{
	unsigned short usIn = 0;
	unsigned short *usInF = NULL;

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
	{
		*usInF |= (0x01 << 0);								// �˻�� �˻� �Ϸ�
		pDoc->Status.bSigTestDoneAoi = TRUE;
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
		pDoc->Status.bSigTestDoneAoi = FALSE;
	}

	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
	{
		*usInF |= (0x01 << 1);								// �˻�� ���̺� ���� �Ϸ�
		pDoc->Status.bSigTblAirAoi = TRUE;
	}
	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
	{
		*usInF &= ~(0x01 << 1);
		pDoc->Status.bSigTblAirAoi = FALSE;
	}

	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
	{
		*usInF |= (0x01 << 2);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
	{
		*usInF &= ~(0x01 << 2);
		// No Use....
	}

	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
	{
		*usInF |= (0x01 << 3);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
	{
		*usInF &= ~(0x01 << 3);
		// No Use....
	}

	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
		// No Use....
	}

	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
	{
		*usInF |= (0x01 << 5);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
	{
		*usInF &= ~(0x01 << 5);
		// No Use....
	}

	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
	{
		*usInF |= (0x01 << 6);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
	{
		*usInF &= ~(0x01 << 6);
		// No Use....
	}

	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
	{
		*usInF |= (0x01 << 7);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
	{
		*usInF &= ~(0x01 << 7);
		// No Use....
	}

	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
	{
		*usInF |= (0x01 << 8);								// �˻�� �ñ׳� Ÿ��-����
															//TowerLamp(RGB_RED, TRUE, FALSE);
	}
	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
	{
		*usInF &= ~(0x01 << 8);
		//TowerLamp(RGB_RED, FALSE, FALSE);
	}

	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
	{
		*usInF |= (0x01 << 9);								// �˻�� �ñ׳� Ÿ��-Ȳ��
															//TowerLamp(RGB_YELLOW, TRUE, FALSE);
	}
	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
	{
		*usInF &= ~(0x01 << 9);
		//TowerLamp(RGB_YELLOW, FALSE, FALSE);
	}

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
	{
		*usInF |= (0x01 << 10);								// �˻�� �ñ׳� Ÿ��-���
															//TowerLamp(RGB_GREEN, TRUE, FALSE);
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
	{
		*usInF &= ~(0x01 << 10);
		//TowerLamp(RGB_GREEN, FALSE, FALSE);
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
	{
		*usInF |= (0x01 << 11);								// �˻�� ���� 1
		Buzzer(TRUE, 0);
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
	{
		*usInF &= ~(0x01 << 11);
		Buzzer(FALSE, 0);
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
	{
		*usInF |= (0x01 << 12);								// �˻�� ���� 2
		Buzzer(TRUE, 1);
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
	{
		*usInF &= ~(0x01 << 12);
		Buzzer(FALSE, 1);
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
	{
		*usInF |= (0x01 << 13);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
	{
		*usInF &= ~(0x01 << 13);
		// No Use....
	}

	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))
	{
		*usInF |= (0x01 << 14);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))
	{
		*usInF &= ~(0x01 << 14);
		// No Use....
	}

	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
	{
		*usInF |= (0x01 << 15);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
	{
		*usInF &= ~(0x01 << 15);
		// No Use....
	}
}

void CManagerPunch::DoUcBoxSw()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)// || !pDoc->m_pMpeIoF)
		return;

	unsigned short usIn = 0;
	unsigned short *usInF = NULL;

	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
	{
		*usInF |= (0x01 << 1);								// �����Ϸ� ���� ��/���� ����ġ
		if (pView->m_pDlgMenu03)
			pView->m_pDlgMenu03->SwUcRelation();
	}
	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
	{
		*usInF &= ~(0x01 << 1);
	}

	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
	{
		*usInF |= (0x01 << 2);								// �����Ϸ� ���� ���/�ϰ� ����ġ
		if (pView->m_pDlgMenu03)
			pView->m_pDlgMenu03->SwUcDcRlUpDn();
	}
	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
	{
		*usInF &= ~(0x01 << 2);
	}

	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
	{
		*usInF |= (0x01 << 3);								// �����Ϸ� Ŭ���ѷ� ���/�ϰ� ����ġ
		if (pView->m_pDlgMenu03)
			pView->m_pDlgMenu03->SwUcClRlUpDn();
	}
	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
	{
		*usInF &= ~(0x01 << 3);
	}

	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// �����Ϸ� Ŭ���ѷ����� ���/�ϰ� ����ġ
		if (pView->m_pDlgMenu03)
			pView->m_pDlgMenu03->SwUcClRlPshUpDn();
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
	}

	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
	{
		*usInF |= (0x01 << 5);								// �����Ϸ� ��ǰ ������(��) ����ġ
		if (pView->m_pDlgMenu03)
			pView->m_pDlgMenu03->SwUcReelJoinL();
	}
	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
	{
		*usInF &= ~(0x01 << 5);
	}

	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
	{
		*usInF |= (0x01 << 6);								// �����Ϸ� ��ǰ ������(��) ����ġ	
		if (pView->m_pDlgMenu03)
			pView->m_pDlgMenu03->SwUcReelJoinR();
	}
	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
	{
		*usInF &= ~(0x01 << 6);
	}

	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
	{
		*usInF |= (0x01 << 7);								// �����Ϸ� ��ǰ�� ���� ����ġ
		if (pView->m_pDlgMenu03)
			pView->m_pDlgMenu03->SwUcReelWheel();
	}
	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
	{
		*usInF &= ~(0x01 << 7);
	}

	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
	{
		*usInF |= (0x01 << 8);								// �����Ϸ� ����ô Ŭ���� ����ġ
		if (pView->m_pDlgMenu03)
			pView->m_pDlgMenu03->SwUcPprChuck();
	}
	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
	{
		*usInF &= ~(0x01 << 8);
	}

	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
	{
		*usInF |= (0x01 << 9);								// �����Ϸ� ������ ��ȸ�� ����ġ
	}
	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
	{
		*usInF &= ~(0x01 << 9);
	}

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
	{
		*usInF |= (0x01 << 10);								// �����Ϸ� ������ ��ȸ�� ����ġ
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
	{
		*usInF &= ~(0x01 << 10);
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
	{
		*usInF |= (0x01 << 11);								// �����Ϸ� ��ǰô Ŭ���� ����ġ
		if (pView->m_pDlgMenu03)
			pView->m_pDlgMenu03->SwUcReelChuck();
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
	{
		*usInF &= ~(0x01 << 11);
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
	{
		*usInF |= (0x01 << 12);								// �����Ϸ� ��ǰ�� ��ȸ�� ����ġ
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
	{
		*usInF &= ~(0x01 << 12);
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
	{
		*usInF |= (0x01 << 13);								// �����Ϸ� ��ǰ�� ��ȸ�� ����ġ
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
	{
		*usInF &= ~(0x01 << 13);
	}

	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
	{
		*usInF |= (0x01 << 15);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
	{
		*usInF &= ~(0x01 << 15);
		// No Use....
	}
#endif
}

void CManagerPunch::DoUcSens1()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)// || !pDoc->m_pMpeIoF)
		return;

	unsigned short usIn = 0;
	unsigned short *usInF = NULL;

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
	{
		*usInF |= (0x01 << 0);								// �����Ϸ� ������������ 1	
															// Late....
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
	}

	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
	{
		*usInF |= (0x01 << 1);								// �����Ϸ� ������������ 2	
															// Late....
	}
	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
	{
		*usInF &= ~(0x01 << 1);
	}

	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
	{
		*usInF |= (0x01 << 2);								// �����Ϸ� ������������ 3	
															// Late....
	}
	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
	{
		*usInF &= ~(0x01 << 2);
	}

	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
	{
		*usInF |= (0x01 << 3);								// �����Ϸ� ������������ 4	
															// Late....
	}
	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
	{
		*usInF &= ~(0x01 << 3);
	}

	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// �����Ϸ� ��ǰ�������� 1	
															// Late....
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
	}

	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
	{
		*usInF |= (0x01 << 5);								// �����Ϸ� ��ǰ�������� 2
															// Late....
	}
	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
	{
		*usInF &= ~(0x01 << 5);
	}

	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
	{
		*usInF |= (0x01 << 6);								// �����Ϸ� ��ǰ�������� 3	
															// Late....
	}
	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
	{
		*usInF &= ~(0x01 << 6);
	}

	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
	{
		*usInF |= (0x01 << 7);								// �����Ϸ� ��ǰ�������� 4	
															// Late....
	}
	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
	{
		*usInF &= ~(0x01 << 7);
	}

	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
	{
		*usInF |= (0x01 << 8);								// �����Ϸ� ��ǰ EPC POS ����Ʈ ����
															// Late....
	}
	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
	{
		*usInF &= ~(0x01 << 8);
	}

	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
	{
		*usInF |= (0x01 << 9);								// �����Ϸ� ��ǰ EPC NEG ����Ʈ ����
															// Late....
	}
	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
	{
		*usInF &= ~(0x01 << 9);
	}

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
	{
		*usInF |= (0x01 << 10);								// �����Ϸ� ��ǰ EPC ���� ���� 
															// Late....
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
	{
		*usInF &= ~(0x01 << 10);
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
	{
		*usInF |= (0x01 << 11);								// �����Ϸ� ���� ���� ����(��)
		pDoc->Status.bDoorUc[DOOR_FL_UC] = TRUE;
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
	{
		*usInF &= ~(0x01 << 11);
		pDoc->Status.bDoorUc[DOOR_FL_UC] = FALSE;
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
	{
		*usInF |= (0x01 << 12);								// �����Ϸ� ���� ���� ����(��)
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
	{
		*usInF &= ~(0x01 << 12);
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
	{
		*usInF |= (0x01 << 13);								// �����Ϸ� ���� ���� ����
		pDoc->Status.bDoorUc[DOOR_FR_UC] = TRUE;
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
	{
		*usInF &= ~(0x01 << 13);
		pDoc->Status.bDoorUc[DOOR_FR_UC] = FALSE;
	}

	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))
	{
		*usInF |= (0x01 << 14);								// �����Ϸ� �ĸ� ���� ����(��)
		pDoc->Status.bDoorUc[DOOR_BL_UC] = TRUE;
	}
	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))
	{
		*usInF &= ~(0x01 << 14);
		pDoc->Status.bDoorUc[DOOR_BL_UC] = FALSE;
	}

	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
	{
		*usInF |= (0x01 << 15);								// �����Ϸ� �ĸ� ���� ����(��)
		pDoc->Status.bDoorUc[DOOR_BR_UC] = TRUE;
	}
	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
	{
		*usInF &= ~(0x01 << 15);
		pDoc->Status.bDoorUc[DOOR_BR_UC] = FALSE;
	}
#endif
}

void CManagerPunch::DoUcSens2()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)
		return;

	unsigned short usIn = 0;
	unsigned short *usInF = NULL;

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
	{
		*usInF |= (0x01 << 0);								// �����Ϸ� �����ټ� POS ����Ʈ ����
															// Late....
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
	}

	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
	{
		*usInF |= (0x01 << 1);								// �����Ϸ� �����ټ� ���� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
	{
		*usInF &= ~(0x01 << 1);
	}

	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
	{
		*usInF |= (0x01 << 2);								// �����Ϸ� �����ټ� 2/3 ���� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
	{
		*usInF &= ~(0x01 << 2);
	}

	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
	{
		*usInF |= (0x01 << 3);								// �����Ϸ� �����ټ� 1/3 ���� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
	{
		*usInF &= ~(0x01 << 3);
	}

	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// �����Ϸ� �����ټ� ���� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
	}

	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
	{
		*usInF |= (0x01 << 5);								// �����Ϸ� �����ټ� NEG ����Ʈ ����
															// Late....
	}
	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
	{
		*usInF &= ~(0x01 << 5);
	}

	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
	{
		*usInF |= (0x01 << 6);								// �����Ϸ� ����ô ���� ����ġ
															// Late....
	}
	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
	{
		*usInF &= ~(0x01 << 6);
	}

	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
	{
		*usInF |= (0x01 << 7);								// �����Ϸ� �������� �з� ����ġ	
															// Late....
	}
	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
	{
		*usInF &= ~(0x01 << 7);
	}

	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
	{
		*usInF |= (0x01 << 8);								// �����Ϸ� Ŭ���ѷ� ��� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
	{
		*usInF &= ~(0x01 << 8);
	}

	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
	{
		*usInF |= (0x01 << 9);								// �����Ϸ� Ŭ���ѷ� �ϰ� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
	{
		*usInF &= ~(0x01 << 9);
	}

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
	{
		*usInF |= (0x01 << 10);								// �����Ϸ� Ŭ���ѷ� ����(����) ��� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
	{
		*usInF &= ~(0x01 << 10);
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
	{
		*usInF |= (0x01 << 11);								// �����Ϸ� Ŭ���ѷ� ����(����) �ϰ� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
	{
		*usInF &= ~(0x01 << 11);
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
	{
		*usInF |= (0x01 << 12);								// �����Ϸ� Ŭ���ѷ� ����(�ĸ�) ��� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
	{
		*usInF &= ~(0x01 << 12);
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
	{
		*usInF |= (0x01 << 13);								// �����Ϸ� Ŭ���ѷ� ����(�ĸ�) �ϰ� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
	{
		*usInF &= ~(0x01 << 13);
	}

	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))
	{
		*usInF |= (0x01 << 14);								// �����Ϸ� ���� ��� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))
	{
		*usInF &= ~(0x01 << 14);
	}

	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
	{
		*usInF |= (0x01 << 15);								// �����Ϸ� ���� �ϰ� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
	{
		*usInF &= ~(0x01 << 15);
	}
#endif
}

void CManagerPunch::DoUcSens3()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)
		return;

	unsigned short usIn = 0;
	unsigned short *usInF = NULL;

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
	{
		*usInF |= (0x01 << 0);								// �����Ϸ� ���� POS ����Ʈ ����
		pDoc->Status.bSensLmtBufUc[LMT_POS] = TRUE;
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
		pDoc->Status.bSensLmtBufUc[LMT_POS] = FALSE;
	}

	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
	{
		*usInF |= (0x01 << 1);								// �����Ϸ� ���� ���� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
	{
		*usInF &= ~(0x01 << 1);
	}

	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
	{
		*usInF |= (0x01 << 2);								// �����Ϸ� ���� 2/3 ���� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
	{
		*usInF &= ~(0x01 << 2);
	}

	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
	{
		*usInF |= (0x01 << 3);								// �����Ϸ� ���� 1/3 ���� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
	{
		*usInF &= ~(0x01 << 3);
	}

	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// �����Ϸ� ���� ���� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
	}

	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
	{
		*usInF |= (0x01 << 5);								// �����Ϸ� ���� NEG ����Ʈ ����
		pDoc->Status.bSensLmtBufUc[LMT_NEG] = TRUE;
	}
	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
	{
		*usInF &= ~(0x01 << 5);
		pDoc->Status.bSensLmtBufUc[LMT_NEG] = FALSE;
	}

	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
	{
		*usInF |= (0x01 << 6);								// �����Ϸ� ��ǰô ���� ����ġ
															// Late....
	}
	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
	{
		*usInF &= ~(0x01 << 6);
	}

	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
	{
		*usInF |= (0x01 << 7);								// �����Ϸ� ���� ���� 
															// Late....
	}
	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
	{
		*usInF &= ~(0x01 << 7);
	}

	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
	{
		*usInF |= (0x01 << 8);								// �����Ϸ� ��ǰ�� ���� ��� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
	{
		*usInF &= ~(0x01 << 8);
	}

	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
	{
		*usInF |= (0x01 << 9);								// �����Ϸ� ��ǰ�� ���� �ϰ� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
	{
		*usInF &= ~(0x01 << 9);
	}

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
	{
		*usInF |= (0x01 << 10);								// �����Ϸ� ��ǰ ������(��) ��� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
	{
		*usInF &= ~(0x01 << 10);
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
	{
		*usInF |= (0x01 << 11);								// �����Ϸ� ��ǰ ������(��) �ϰ� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
	{
		*usInF &= ~(0x01 << 11);
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
	{
		*usInF |= (0x01 << 12);								// �����Ϸ� ��ǰ ������(��) ��� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
	{
		*usInF &= ~(0x01 << 12);
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
	{
		*usInF |= (0x01 << 13);								// �����Ϸ� ��ǰ ������(��) �ϰ� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
	{
		*usInF &= ~(0x01 << 13);
	}

	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))
	{
		*usInF |= (0x01 << 14);								// SPARE	
															// No Use....
	}
	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))
	{
		*usInF &= ~(0x01 << 14);
		// No Use....
	}

	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
	{
		*usInF |= (0x01 << 15);								// �����Ϸ� ��ǰ�� ���� ����ġ ����
															// Late....
	}
	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
	{
		*usInF &= ~(0x01 << 15);
	}
#endif
}

void CManagerPunch::DoRcBoxSw()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)
		return;

	unsigned short usIn = 0;
	unsigned short *usInF = NULL;

	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
	{
		*usInF |= (0x01 << 1);								// �����Ϸ� ���� ��/���� ����ġ
	}
	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
	{
		*usInF &= ~(0x01 << 1);
	}

	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
	{
		*usInF |= (0x01 << 2);								// �����Ϸ� ���� ���/�ϰ� ����ġ
	}
	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
	{
		*usInF &= ~(0x01 << 2);
	}

	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
	{
		*usInF |= (0x01 << 3);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
	{
		*usInF &= ~(0x01 << 3);
		// No Use....
	}

	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
		// No Use....
	}

	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
	{
		*usInF |= (0x01 << 5);								// �����Ϸ� ��ǰ ������(��) ����ġ
	}
	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
	{
		*usInF &= ~(0x01 << 5);
	}

	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
	{
		*usInF |= (0x01 << 6);								// �����Ϸ� ��ǰ ������(��) ����ġ
	}
	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
	{
		*usInF &= ~(0x01 << 6);
	}

	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
	{
		*usInF |= (0x01 << 7);								// �����Ϸ� ��ǰ�� ���� ����ġ
	}
	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
	{
		*usInF &= ~(0x01 << 7);
	}

	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
	{
		*usInF |= (0x01 << 8);								// �����Ϸ� ����ô Ŭ���� ����ġ
	}
	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
	{
		*usInF &= ~(0x01 << 8);
	}

	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
	{
		*usInF |= (0x01 << 9);								// �����Ϸ� ������ ��ȸ�� ����ġ
	}
	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
	{
		*usInF &= ~(0x01 << 9);
	}

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
	{
		*usInF |= (0x01 << 10);								// �����Ϸ� ������ ��ȸ�� ����ġ
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
	{
		*usInF &= ~(0x01 << 10);
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
	{
		*usInF |= (0x01 << 11);								// �����Ϸ� ��ǰô Ŭ���� ����ġ
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
	{
		*usInF &= ~(0x01 << 11);
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
	{
		*usInF |= (0x01 << 12);								// �����Ϸ� ��ǰ�� ��ȸ�� ����ġ
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
	{
		*usInF &= ~(0x01 << 12);
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
	{
		*usInF |= (0x01 << 13);								// �����Ϸ� ��ǰ�� ��ȸ�� ����ġ
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
	{
		*usInF &= ~(0x01 << 13);
	}

	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
	{
		*usInF |= (0x01 << 15);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
	{
		*usInF &= ~(0x01 << 15);
		// No Use....
	}
#endif
}

void CManagerPunch::DoRcSens1()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)
		return;

	unsigned short usIn = 0;
	unsigned short *usInF = NULL;

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
	{
		*usInF |= (0x01 << 0);								// �����Ϸ� ������������ 1
															// Late....
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
	}

	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
	{
		*usInF |= (0x01 << 1);								// �����Ϸ� ������������ 2
															// Late....
	}
	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
	{
		*usInF &= ~(0x01 << 1);
	}

	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
	{
		*usInF |= (0x01 << 2);								// �����Ϸ� ������������ 3
															// Late....
	}
	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
	{
		*usInF &= ~(0x01 << 2);
	}

	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
	{
		*usInF |= (0x01 << 3);								// �����Ϸ� ������������ 4
															// Late....
	}
	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
	{
		*usInF &= ~(0x01 << 3);
	}

	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// �����Ϸ� ��ǰ�������� 1
															// Late....
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
	}

	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
	{
		*usInF |= (0x01 << 5);								// �����Ϸ� ��ǰ�������� 2
															// Late....
	}
	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
	{
		*usInF &= ~(0x01 << 5);
	}

	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
	{
		*usInF |= (0x01 << 6);								// �����Ϸ� ��ǰ�������� 3
															// Late....
	}
	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
	{
		*usInF &= ~(0x01 << 6);
	}

	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
	{
		*usInF |= (0x01 << 7);								// �����Ϸ� ��ǰ�������� 4
															// Late....
	}
	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
	{
		*usInF &= ~(0x01 << 7);
	}

	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
	{
		*usInF |= (0x01 << 8);								// �����Ϸ� ��ǰ EPC POS ����Ʈ ����
															// Late....
	}
	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
	{
		*usInF &= ~(0x01 << 8);
	}

	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
	{
		*usInF |= (0x01 << 9);								// �����Ϸ� ��ǰ EPC NEG ����Ʈ ����
															// Late....
	}
	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
	{
		*usInF &= ~(0x01 << 9);
	}

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
	{
		*usInF |= (0x01 << 10);								// �����Ϸ� ��ǰ EPC ���� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
	{
		*usInF &= ~(0x01 << 10);
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
	{
		*usInF |= (0x01 << 11);								// �����Ϸ� ���� ���� ����(��)
		pDoc->Status.bDoorRe[DOOR_FL_RC] = TRUE;
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
	{
		*usInF &= ~(0x01 << 11);
		pDoc->Status.bDoorRe[DOOR_FL_RC] = FALSE;
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
	{
		*usInF |= (0x01 << 12);								// �����Ϸ� ���� ���� ����(��)
		pDoc->Status.bDoorRe[DOOR_FR_RC] = TRUE;
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
	{
		*usInF &= ~(0x01 << 12);
		pDoc->Status.bDoorRe[DOOR_FR_RC] = FALSE;
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
	{
		*usInF |= (0x01 << 13);								// �����Ϸ� ���� ���� ����
		pDoc->Status.bDoorRe[DOOR_S_RC] = TRUE;
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
	{
		*usInF &= ~(0x01 << 13);
		pDoc->Status.bDoorRe[DOOR_S_RC] = FALSE;
	}

	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))
	{
		*usInF |= (0x01 << 14);								// �����Ϸ� �ĸ� ���� ����(��)
		pDoc->Status.bDoorRe[DOOR_BL_RC] = TRUE;
	}
	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))
	{
		*usInF &= ~(0x01 << 14);
		pDoc->Status.bDoorRe[DOOR_BL_RC] = FALSE;
	}

	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
	{
		*usInF |= (0x01 << 15);								// �����Ϸ� �ĸ� ���� ����(��)
		pDoc->Status.bDoorRe[DOOR_BR_RC] = TRUE;
	}
	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
	{
		*usInF &= ~(0x01 << 15);
		pDoc->Status.bDoorRe[DOOR_BR_RC] = FALSE;
	}
#endif
}

void CManagerPunch::DoRcSens2()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)
		return;

	unsigned short usIn = 0;
	unsigned short *usInF = NULL;

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
	{
		*usInF |= (0x01 << 0);								// �����Ϸ� �����ټ� POS ����Ʈ ����
															// Late....
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
	}

	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
	{
		*usInF |= (0x01 << 1);								// �����Ϸ� �����ټ� ���� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
	{
		*usInF &= ~(0x01 << 1);
	}

	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
	{
		*usInF |= (0x01 << 2);								// �����Ϸ� �����ټ� 2/3 ���� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
	{
		*usInF &= ~(0x01 << 2);
	}

	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
	{
		*usInF |= (0x01 << 3);								// �����Ϸ� �����ټ� 1/3 ���� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
	{
		*usInF &= ~(0x01 << 3);
	}

	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// �����Ϸ� �����ټ� ���� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
	}

	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
	{
		*usInF |= (0x01 << 5);								// �����Ϸ� �����ټ� NEG ����Ʈ ����
															// Late....
	}
	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
	{
		*usInF &= ~(0x01 << 5);
	}

	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
	{
		*usInF |= (0x01 << 6);								// �����Ϸ� ����ô ���� ����ġ
															// Late....
	}
	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
	{
		*usInF &= ~(0x01 << 6);
	}

	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
	{
		*usInF |= (0x01 << 7);								// �����Ϸ� �������� �з� ����ġ
															// Late....
	}
	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
	{
		*usInF &= ~(0x01 << 7);
	}

	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
	{
		*usInF |= (0x01 << 8);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
	{
		*usInF &= ~(0x01 << 8);
		// No Use....
	}

	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
	{
		*usInF |= (0x01 << 9);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
	{
		*usInF &= ~(0x01 << 9);
		// No Use....
	}

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
	{
		*usInF |= (0x01 << 10);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
	{
		*usInF &= ~(0x01 << 10);
		// No Use....
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
	{
		*usInF |= (0x01 << 11);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
	{
		*usInF &= ~(0x01 << 11);
		// No Use....
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
	{
		*usInF |= (0x01 << 12);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
	{
		*usInF &= ~(0x01 << 12);
		// No Use....
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
	{
		*usInF |= (0x01 << 13);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
	{
		*usInF &= ~(0x01 << 13);
		// No Use....
	}

	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))
	{
		*usInF |= (0x01 << 14);								// �����Ϸ� ���� ��� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))
	{
		*usInF &= ~(0x01 << 14);
	}

	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
	{
		*usInF |= (0x01 << 15);								// �����Ϸ� ���� �ϰ� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
	{
		*usInF &= ~(0x01 << 15);
	}
#endif
}

void CManagerPunch::DoRcSens3()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)
		return;

	unsigned short usIn = 0;
	unsigned short *usInF = NULL;

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
	{
		*usInF |= (0x01 << 0);								// �����Ϸ� ���� POS ����Ʈ ����
		pDoc->Status.bSensLmtBufRc[LMT_POS] = TRUE;
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
		pDoc->Status.bSensLmtBufRc[LMT_POS] = FALSE;
	}

	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
	{
		*usInF |= (0x01 << 1);								// �����Ϸ� ���� ���� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
	{
		*usInF &= ~(0x01 << 1);
	}

	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
	{
		*usInF |= (0x01 << 2);								// �����Ϸ� ���� 2/3 ���� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
	{
		*usInF &= ~(0x01 << 2);
	}

	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
	{
		*usInF |= (0x01 << 3);								// �����Ϸ� ���� 1/3 ���� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
	{
		*usInF &= ~(0x01 << 3);
	}

	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// �����Ϸ� ���� ���� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
	}

	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
	{
		*usInF |= (0x01 << 5);								// �����Ϸ� ���� NEG ����Ʈ ����
		pDoc->Status.bSensLmtBufRc[LMT_NEG] = TRUE;
	}
	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
	{
		*usInF &= ~(0x01 << 5);
		pDoc->Status.bSensLmtBufRc[LMT_NEG] = FALSE;
	}

	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
	{
		*usInF |= (0x01 << 6);								// �����Ϸ� ��ǰô ���� ����ġ
															// Late....
	}
	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
	{
		*usInF &= ~(0x01 << 6);
	}

	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
	{
		*usInF |= (0x01 << 7);								// �����Ϸ� ���� ���� 
															// Late....
	}
	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
	{
		*usInF &= ~(0x01 << 7);
	}

	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
	{
		*usInF |= (0x01 << 8);								// �����Ϸ� ��ǰ�� ���� ��� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
	{
		*usInF &= ~(0x01 << 8);
	}

	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
	{
		*usInF |= (0x01 << 9);								// �����Ϸ� ��ǰ�� ���� �ϰ� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
	{
		*usInF &= ~(0x01 << 9);
	}

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
	{
		*usInF |= (0x01 << 10);								// �����Ϸ� ��ǰ ������(��) ��� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
	{
		*usInF &= ~(0x01 << 10);
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
	{
		*usInF |= (0x01 << 11);								// �����Ϸ� ��ǰ ������(��) �ϰ� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
	{
		*usInF &= ~(0x01 << 11);
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
	{
		*usInF |= (0x01 << 12);								// �����Ϸ� ��ǰ ������(��) ��� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
	{
		*usInF &= ~(0x01 << 12);
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
	{
		*usInF |= (0x01 << 13);								// �����Ϸ� ��ǰ ������(��) �ϰ� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
	{
		*usInF &= ~(0x01 << 13);
	}

	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))
	{
		*usInF |= (0x01 << 14);								// �����Ϸ� ��ǰ �ܷ� ���� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))
	{
		*usInF &= ~(0x01 << 14);
	}

	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
	{
		*usInF |= (0x01 << 15);								// �����Ϸ� ��ǰ�� ���� ����ġ ����
															// Late....
	}
	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
	{
		*usInF &= ~(0x01 << 15);
	}
#endif
}

void CManagerPunch::DoEngraveSens()
{
#ifdef USE_MPE
	unsigned short usIn;
	unsigned short *usInF;

	if (!pDoc->m_pMpeIb || !pDoc->m_pMpeIF)
		return;

	usIn = pDoc->m_pMpeIb[28];
	usInF = &pDoc->m_pMpeIF[28];


	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// 2D ���� ����ġ
		m_bSwStopNow = FALSE;
		if (pView->m_pDlgMenu03)
			pView->m_pDlgMenu03->SwReset();
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
	}
#endif
}

void CManagerPunch::DoSens()
{
}

void CManagerPunch::DoBoxSw()
{
}

void CManagerPunch::DoEmgSw()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)
		return;
#endif
}

void CManagerPunch::DoAuto()
{
	if (!IsAuto())
		return;

	CString str;
	str.Format(_T("%d : %d"), pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF, pView->m_mgrProcedure->m_bTHREAD_DISP_DEF ? 1 : 0);
	pView->DispStsBar(str, 6);

	// LotEnd Start
	if (DoAutoGetLotEndSignal())
		return;

	if (pView->m_mgrProcedure)
		pView->m_mgrProcedure->DoAuto();
}

BOOL CManagerPunch::DoAutoGetLotEndSignal()
{
	int nSerial;

	if (pView->m_pDlgMenu01)
	{
		if (pView->m_pDlgMenu01->m_bLotEnd && pView->m_mgrProcedure->m_nStepAuto < LOT_END)
		{
			pView->m_mgrProcedure->m_bLotEnd = TRUE;
			pView->m_mgrProcedure->m_nLotEndAuto = LOT_END;
		}
	}

	if (!pView->m_mgrProcedure->IsBuffer(0) && pView->m_mgrProcedure->m_bLastProc && pView->m_mgrProcedure->m_nLotEndAuto < LOT_END)
	{
		pView->m_mgrProcedure->m_bLotEnd = TRUE;
		pView->m_mgrProcedure->m_nLotEndAuto = LOT_END;
	}
	else if (!pView->m_mgrProcedure->IsBuffer(0) && pView->m_mgrProcedure->m_nMkStAuto > MK_ST + (Mk2PtIdx::DoneMk) + 4)
	{
		pView->m_mgrProcedure->m_nMkStAuto = 0;
		pView->m_mgrProcedure->m_bLotEnd = TRUE;
		pView->m_mgrProcedure->m_nLotEndAuto = LOT_END;
	}


	if (pView->m_mgrProcedure->m_bLotEnd)
	{
		nSerial = pDoc->GetLastShotMk();

		switch (pView->m_mgrProcedure->m_nLotEndAuto)
		{
		case LOT_END:
			if (!pView->m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_UP && !pView->m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_DN
				&& !pView->m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_ALLUP && !pView->m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_ALLDN) // Yield Reelmap
			{
				pView->m_mgrReelmap->ReloadReelmap(nSerial);
				UpdateRst();
				//pDoc->UpdateYieldOnRmap(); // 20230614
				pView->m_mgrProcedure->m_nLotEndAuto++;
			}
			break;
		case LOT_END + 1:
			MpeWrite(_T("MB440180"), 1);			// �۾�����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141031
			DispMain(_T("�۾�����"), RGB_RED);
			pView->m_mgrProcedure->m_nLotEndAuto++;
			break;
		case LOT_END + 2:
			Stop();
			//TowerLamp(RGB_YELLOW, TRUE);
			Buzzer(TRUE, 0);
			pView->LotEnd();									// MakeResultMDS
			pView->m_mgrProcedure->m_nLotEndAuto++;
			break;

		case LOT_END + 3:
			MsgBox(_T("�۾��� ����Ǿ����ϴ�."));
			//m_nStepAuto = 0; // �ڵ�����
			pView->m_mgrProcedure->m_nLotEndAuto++;
			//m_bLotEnd = FALSE;
			pView->m_mgrProcedure->m_bLastProc = FALSE;
			pView->m_mgrProcedure->m_bMkSt = FALSE;
			::WritePrivateProfileString(_T("Last Job"), _T("MkSt"), _T("0"), PATH_WORKING_INFO);
			break;
		case LOT_END + 4:
			break;
		}
	}

	return pView->m_mgrProcedure->m_bLotEnd;
}

void CManagerPunch::DoInterlock()
{
	if (m_dEnc[AXIS_Y0] < 20.0 && m_dEnc[AXIS_Y1] < 20.0)
	{
		if (pView->m_mgrProcedure->m_bStopFeeding)
		{
			pView->m_mgrProcedure->m_bStopFeeding = FALSE;
			MpeWrite(_T("MB440115"), 0); // ��ŷ��Feeding����
		}
	}
	else
	{
		if (!pView->m_mgrProcedure->m_bStopFeeding)
		{
			pView->m_mgrProcedure->m_bStopFeeding = TRUE;
			MpeWrite(_T("MB440115"), 1); // ��ŷ��Feeding����
		}
	}
}

void CManagerPunch::DoSaftySens()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIb || !pDoc->m_pMpeIF)
		return;

	if (!pView->IsRun())
		return;

	unsigned short usIn = pDoc->m_pMpeIb[7];
	unsigned short *usInF = &pDoc->m_pMpeIF[7];

	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))		// ��ŷ�� ���� ����
	{
		*usInF |= (0x01 << 8);
		pDoc->Status.bSensSaftyMk = TRUE;
	}
	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))	// ��ŷ�� ���� ����
	{
		*usInF &= ~(0x01 << 8);
		pDoc->Status.bSensSaftyMk = FALSE;
	}
#endif
}

void CManagerPunch::DoDoorSens()
{
#ifdef USE_MPE
	unsigned short usIn;
	unsigned short *usInF;

	if (!pDoc->m_pMpeIb || !pDoc->m_pMpeIF)
		return;

	if (!pView->IsRun())
		return;

	usIn = pDoc->m_pMpeIb[1];
	usInF = &pDoc->m_pMpeIF[1];

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))		// �����Ϸ� ���� ���� ����
	{
		*usInF |= (0x01 << 12);
		pDoc->Status.bDoorUc[DOOR_FL_UC] = TRUE;
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))	// �����Ϸ� ���� ���� ����
	{
		*usInF &= ~(0x01 << 12);
		pDoc->Status.bDoorUc[DOOR_FL_UC] = FALSE;
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))		// �����Ϸ� ���� ���� ����
	{
		*usInF |= (0x01 << 13);
		pDoc->Status.bDoorUc[DOOR_FR_UC] = TRUE;
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))	// �����Ϸ� ���� ���� ����
	{
		*usInF &= ~(0x01 << 13);
		pDoc->Status.bDoorUc[DOOR_FR_UC] = FALSE;
	}

	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))		// �����Ϸ� �ĸ� ���� ����(��)
	{
		*usInF |= (0x01 << 14);
		pDoc->Status.bDoorUc[DOOR_BL_UC] = TRUE;
	}
	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))	// �����Ϸ� �ĸ� ���� ����(��)
	{
		*usInF &= ~(0x01 << 14);
		pDoc->Status.bDoorUc[DOOR_BL_UC] = FALSE;
	}

	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))		// �����Ϸ� �ĸ� ���� ����(��)
	{
		*usInF |= (0x01 << 15);
		pDoc->Status.bDoorUc[DOOR_BR_UC] = TRUE;
	}
	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))	// �����Ϸ� �ĸ� ���� ����(��)
	{
		*usInF &= ~(0x01 << 15);
		pDoc->Status.bDoorUc[DOOR_BR_UC] = FALSE;
	}

	usIn = pDoc->m_pMpeIb[7];
	usInF = &pDoc->m_pMpeIF[7];

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))		// ��ŷ�� ���� ���� 1 
	{
		*usInF |= (0x01 << 10);
		pDoc->Status.bDoorMk[DOOR_FL_MK] = TRUE;
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))	// ��ŷ�� ���� ���� 1 
	{
		*usInF &= ~(0x01 << 10);
		pDoc->Status.bDoorMk[DOOR_FL_MK] = FALSE;
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))		// ��ŷ�� ���� ���� 2 
	{
		*usInF |= (0x01 << 11);
		pDoc->Status.bDoorMk[DOOR_FR_MK] = TRUE;
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))	// ��ŷ�� ���� ���� 2
	{
		*usInF &= ~(0x01 << 11);
		pDoc->Status.bDoorMk[DOOR_FR_MK] = FALSE;
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))		// ��ŷ�� ���� ���� 3 
	{
		*usInF |= (0x01 << 12);
		pDoc->Status.bDoorMk[DOOR_BL_MK] = TRUE;
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))	// ��ŷ�� ���� ���� 3
	{
		*usInF &= ~(0x01 << 12);
		pDoc->Status.bDoorMk[DOOR_BL_MK] = FALSE;
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))		// ��ŷ�� ���� ���� 4 
	{
		*usInF |= (0x01 << 13);
		pDoc->Status.bDoorMk[DOOR_BR_MK] = TRUE;
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))	// ��ŷ�� ���� ���� 4
	{
		*usInF &= ~(0x01 << 13);
		pDoc->Status.bDoorMk[DOOR_BR_MK] = FALSE;
	}

	usIn = pDoc->m_pMpeIb[11];
	usInF = &pDoc->m_pMpeIF[11];

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))		// �˻�� �� ���� ���� 1 
	{
		*usInF |= (0x01 << 10);
		pDoc->Status.bDoorMk[DOOR_FL_AOI_UP] = TRUE;
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))	// �˻�� �� ���� ���� 1
	{
		*usInF &= ~(0x01 << 10);
		pDoc->Status.bDoorMk[DOOR_FL_AOI_UP] = FALSE;
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))		// �˻�� �� ���� ���� 2 
	{
		*usInF |= (0x01 << 11);
		pDoc->Status.bDoorMk[DOOR_FR_AOI_UP] = TRUE;
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))	// �˻�� �� ���� ���� 2
	{
		*usInF &= ~(0x01 << 11);
		pDoc->Status.bDoorMk[DOOR_FR_AOI_UP] = FALSE;
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))		// �˻�� �� ���� ���� 3 
	{
		*usInF |= (0x01 << 12);
		pDoc->Status.bDoorMk[DOOR_BL_AOI_UP] = TRUE;
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))	// �˻�� �� ���� ���� 3
	{
		*usInF &= ~(0x01 << 12);
		pDoc->Status.bDoorMk[DOOR_BL_AOI_UP] = FALSE;
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))		// �˻�� �� ���� ���� 4 
	{
		*usInF |= (0x01 << 13);
		pDoc->Status.bDoorMk[DOOR_BR_AOI_UP] = TRUE;
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))	// �˻�� �� ���� ���� 4
	{
		*usInF &= ~(0x01 << 13);
		pDoc->Status.bDoorMk[DOOR_BR_AOI_UP] = FALSE;
	}

	usIn = pDoc->m_pMpeIb[15];
	usInF = &pDoc->m_pMpeIF[15];

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))		// �˻�� �� ���� ���� 1 
	{
		*usInF |= (0x01 << 10);
		pDoc->Status.bDoorAoi[DOOR_FL_AOI_UP] = TRUE;
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))	// �˻�� �� ���� ���� 1
	{
		*usInF &= ~(0x01 << 11);
		pDoc->Status.bDoorAoi[DOOR_FL_AOI_UP] = FALSE;
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))		// �˻�� �� ���� ���� 2 
	{
		*usInF |= (0x01 << 11);
		pDoc->Status.bDoorAoi[DOOR_FR_AOI_UP] = TRUE;
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))	// �˻�� �� ���� ���� 2
	{
		*usInF &= ~(0x01 << 11);
		pDoc->Status.bDoorAoi[DOOR_FR_AOI_UP] = FALSE;
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))		// �˻�� �� ���� ���� 3 
	{
		*usInF |= (0x01 << 12);
		pDoc->Status.bDoorAoi[DOOR_BL_AOI_UP] = TRUE;
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))	// �˻�� �� ���� ���� 3
	{
		*usInF &= ~(0x01 << 12);
		pDoc->Status.bDoorAoi[DOOR_BL_AOI_UP] = FALSE;
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))		// �˻�� �� ���� ���� 4 
	{
		*usInF |= (0x01 << 13);
		pDoc->Status.bDoorAoi[DOOR_BR_AOI_UP] = TRUE;
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))	// �˻�� �� ���� ���� 4
	{
		*usInF &= ~(0x01 << 13);
		pDoc->Status.bDoorAoi[DOOR_BR_AOI_UP] = FALSE;
	}

	usIn = pDoc->m_pMpeIb[15];
	usInF = &pDoc->m_pMpeIF[15];

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))		// �˻�� �� ���� ���� 1 
	{
		*usInF |= (0x01 << 10);
		pDoc->Status.bDoorAoi[DOOR_FL_AOI_DN] = TRUE;
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))	// �˻�� �� ���� ���� 1
	{
		*usInF &= ~(0x01 << 10);
		pDoc->Status.bDoorAoi[DOOR_FL_AOI_DN] = FALSE;
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))		// �˻�� �� ���� ���� 2 
	{
		*usInF |= (0x01 << 11);
		pDoc->Status.bDoorAoi[DOOR_FR_AOI_DN] = TRUE;
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))	// �˻�� �� ���� ���� 2
	{
		*usInF &= ~(0x01 << 11);
		pDoc->Status.bDoorAoi[DOOR_FR_AOI_DN] = FALSE;
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))		// �˻�� �� ���� ���� 3 
	{
		*usInF |= (0x01 << 12);
		pDoc->Status.bDoorAoi[DOOR_BL_AOI_DN] = TRUE;
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))	// �˻�� �� ���� ���� 3
	{
		*usInF &= ~(0x01 << 12);
		pDoc->Status.bDoorAoi[DOOR_BL_AOI_DN] = FALSE;
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))		// �˻�� �� ���� ���� 4 
	{
		*usInF |= (0x01 << 13);
		pDoc->Status.bDoorAoi[DOOR_BR_AOI_DN] = TRUE;
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))	// �˻�� �� ���� ���� 4
	{
		*usInF &= ~(0x01 << 13);
		pDoc->Status.bDoorAoi[DOOR_BR_AOI_DN] = FALSE;
	}

	usIn = pDoc->m_pMpeIb[17];
	usInF = &pDoc->m_pMpeIF[17];

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))		// �����Ϸ� ���� ���� ����
	{
		*usInF |= (0x01 << 12);
		pDoc->Status.bDoorRe[DOOR_FL_RC] = TRUE;
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))	// �����Ϸ� ���� ���� ����
	{
		*usInF &= ~(0x01 << 12);
		pDoc->Status.bDoorRe[DOOR_FL_RC] = FALSE;
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))		// �����Ϸ� ���� ���� ����
	{
		*usInF |= (0x01 << 13);
		pDoc->Status.bDoorRe[DOOR_FR_RC] = TRUE;
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))	// �����Ϸ� ���� ���� ����
	{
		*usInF &= ~(0x01 << 13);
		pDoc->Status.bDoorRe[DOOR_FR_RC] = FALSE;
	}

	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))		// �����Ϸ� �ĸ� ���� ����
	{
		*usInF |= (0x01 << 14);
		pDoc->Status.bDoorRe[DOOR_BL_RC] = TRUE;
	}
	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))	// �����Ϸ� �ĸ� ���� ����(��)
	{
		*usInF &= ~(0x01 << 14);
		pDoc->Status.bDoorRe[DOOR_BL_RC] = FALSE;
	}

	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))		// �����Ϸ� �ĸ� ���� ����(��)
	{
		*usInF |= (0x01 << 15);
		pDoc->Status.bDoorRe[DOOR_BR_RC] = TRUE;
	}
	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))	// �����Ϸ� �ĸ� ���� ����(��)
	{
		*usInF &= ~(0x01 << 15);
		pDoc->Status.bDoorRe[DOOR_BR_RC] = FALSE;
	}

	usIn = pDoc->m_pMpeIb[27];
	usInF = &pDoc->m_pMpeIF[27];

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))		// ���κ� ���� ���� 1
	{
		*usInF |= (0x01 << 10);
		pDoc->Status.bDoorEngv[DOOR_FL_ENGV] = TRUE;
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))	// ���κ� ���� ���� 1
	{
		*usInF &= ~(0x01 << 10);
		pDoc->Status.bDoorEngv[DOOR_FL_ENGV] = FALSE;
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))		// ���κ� ���� ���� 2
	{
		*usInF |= (0x01 << 11);
		pDoc->Status.bDoorEngv[DOOR_FR_ENGV] = TRUE;
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))	// ���κ� ���� ���� 2
	{
		*usInF &= ~(0x01 << 11);
		pDoc->Status.bDoorEngv[DOOR_FR_ENGV] = FALSE;
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))		// ���κ� ���� ���� 3
	{
		*usInF |= (0x01 << 12);
		pDoc->Status.bDoorEngv[DOOR_BL_ENGV] = TRUE;
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))	// ���κ� ���� ���� 3
	{
		*usInF &= ~(0x01 << 12);
		pDoc->Status.bDoorEngv[DOOR_BL_ENGV] = FALSE;
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))		// ���κ� ���� ���� 4
	{
		*usInF |= (0x01 << 13);
		pDoc->Status.bDoorEngv[DOOR_BR_ENGV] = TRUE;
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))	// ���κ� ���� ���� 4
	{
		*usInF &= ~(0x01 << 13);
		pDoc->Status.bDoorEngv[DOOR_BR_ENGV] = FALSE;
	}

	usIn = pDoc->m_pMpeIb[28];
	usInF = &pDoc->m_pMpeIF[28];


	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// 2D ���� ����ġ
		m_bSwStopNow = FALSE;
		if (pView->m_pDlgMenu03)
			pView->m_pDlgMenu03->SwReset();
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
	}

#endif
}

void CManagerPunch::SwJog(int nAxisID, int nDir, BOOL bOn)
{
	if (!m_pMotion)
		return;

	double fVel, fAcc, fJerk;
	double dStep;
	if (pDoc->Status.bSwJogFast)
		dStep = 0.5;
	else
		dStep = 0.1;

	if (pDoc->Status.bSwJogStep)
	{
		if (bOn)
		{
			double dPos = m_dEnc[nAxisID]; // m_pMotion->GetActualPosition(nAxisID);
			if (nDir == M_CW)
				dPos += dStep;
			else if (nDir == M_CCW)
				dPos -= dStep;

			if (nAxisID == AXIS_X0)
			{
				if (m_bAuto && m_bTHREAD_MK[0] && m_bTHREAD_MK[1] && IsReview())
				{
					if (nDir == M_CW) // �� Jog ��ư.
						return;
				}

				if (m_pMotion->IsLimit(MS_X0, nDir))
					return;
				m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_X0, dStep, fVel, fAcc, fJerk);
				m_pMotion->Move(MS_X0, dPos, fVel, fAcc, fAcc);
			}
			else if (nAxisID == AXIS_Y0)
			{
				if (m_pMotion->IsLimit(MS_Y0, nDir))
					return;
				m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_Y0, dStep, fVel, fAcc, fJerk);
				m_pMotion->Move(MS_Y0, dPos, fVel, fAcc, fAcc);
			}
			else if (nAxisID == AXIS_X1)
			{
				if (m_bAuto && m_bTHREAD_MK[0] && m_bTHREAD_MK[1] && IsReview())
				{
					if (nDir == M_CW) // �� Jog ��ư.
						return;
				}

				if (m_pMotion->IsLimit(MS_X1, nDir))
					return;
				m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_X1, dStep, fVel, fAcc, fJerk);
				m_pMotion->Move(MS_X1, dPos, fVel, fAcc, fAcc);
			}
			else if (nAxisID == AXIS_Y1)
			{
				if (m_pMotion->IsLimit(MS_Y1, nDir))
					return;
				m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_Y1, dStep, fVel, fAcc, fJerk);
				m_pMotion->Move(MS_Y1, dPos, fVel, fAcc, fAcc);
			}
		}
	}
	else	// Jog Mode
	{
		if (!pView->m_pDlgMenu02)
			return;

		if (nAxisID == AXIS_Y0)
		{
			if (nDir == M_CCW)		// Up
			{
				if (bOn)
					pView->m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_UP);
				else
					pView->m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_UP);
			}
			else if (nDir == M_CW)	// Dn
			{
				if (bOn)
					pView->m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_DN);
				else
					pView->m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_DN);
			}
		}
		else if (nAxisID == AXIS_X0)
		{
			if (m_bAuto && m_bTHREAD_MK[0] && m_bTHREAD_MK[1] && IsReview())
			{
				if (nDir == M_CW) // �� Jog ��ư.
					return;
			}

			if (nDir == M_CW)		// Rt
			{
				if (bOn)
					pView->m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_RT);
				else
					pView->m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_RT);
			}
			else if (nDir == M_CCW)	// Lf
			{
				if (bOn)
					pView->m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_LF);
				else
					pView->m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_LF);
			}
		}
		else if (nAxisID == AXIS_Y1)
		{
			if (nDir == M_CCW)		// Up
			{
				if (bOn)
					pView->m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_UP2);
				else
					pView->m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_UP2);
			}
			else if (nDir == M_CW)	// Dn
			{
				if (bOn)
					pView->m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_DN2);
				else
					pView->m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_DN2);
			}
		}
		else if (nAxisID == AXIS_X1)
		{
			if (m_bAuto && m_bTHREAD_MK[0] && m_bTHREAD_MK[1] && IsReview())
			{
				if (nDir == M_CW) // �� Jog ��ư.
					return;
			}

			if (nDir == M_CW)		// Rt
			{
				if (bOn)
					pView->m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_RT2);
				else
					pView->m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_RT2);
			}
			else if (nDir == M_CCW)	// Lf
			{
				if (bOn)
					pView->m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_LF2);
				else
					pView->m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_LF2);
			}
		}
	}
}

BOOL CManagerPunch::ChkSaftySen() // ���� : TRUE , ���� : FALSE
{
	if (pDoc->WorkingInfo.LastJob.bMkSftySen)
	{
		if (pDoc->Status.bSensSaftyMk && !pDoc->Status.bSensSaftyMkF)
		{
			pDoc->Status.bSensSaftyMkF = TRUE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("�� ��"), RGB_RED);
			//TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
			pView->m_bTIM_SAFTY_STOP = TRUE;//MsgBox(_T("�Ͻ����� - ��ŷ�� ���������� �����Ǿ����ϴ�."));
			SetTimer(TIM_SAFTY_STOP, 100, NULL);
		}
		else if (!pDoc->Status.bSensSaftyMk && pDoc->Status.bSensSaftyMkF)
		{
			pDoc->Status.bSensSaftyMkF = FALSE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
	}

	return (pDoc->Status.bSensSaftyMk);
}

unsigned long CManagerPunch::ChkDoor() // 0: All Closed , Open Door Index : Doesn't all closed. (Bit3: F, Bit2: L, Bit1: R, Bit0; B)
{
	unsigned long ulOpenDoor = 0;

	if (pDoc->WorkingInfo.LastJob.bAoiUpDrSen)
	{
		if (pDoc->Status.bDoorAoi[DOOR_FM_AOI_UP] && !pDoc->Status.bDoorAoiF[DOOR_FM_AOI_UP])
		{
			ulOpenDoor |= (0x01 << 0);
			pDoc->Status.bDoorAoiF[DOOR_FM_AOI_UP] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_FM_AOI_UP] && pDoc->Status.bDoorAoiF[DOOR_FM_AOI_UP])
		{
			ulOpenDoor &= ~(0x01 << 0);
			pDoc->Status.bDoorAoiF[DOOR_FM_AOI_UP] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �˻�� �� ���� �߾� ���� Open"));
			//TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorAoi[DOOR_FL_AOI_UP] && !pDoc->Status.bDoorAoiF[DOOR_FL_AOI_UP])
		{
			ulOpenDoor |= (0x01 << 1);
			pDoc->Status.bDoorAoiF[DOOR_FL_AOI_UP] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_FL_AOI_UP] && pDoc->Status.bDoorAoiF[DOOR_FL_AOI_UP])
		{
			ulOpenDoor &= ~(0x01 << 1);
			pDoc->Status.bDoorAoiF[DOOR_FL_AOI_UP] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �˻�� �� ���� ���� ���� Open"));
			//TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorAoi[DOOR_FR_AOI_UP] && !pDoc->Status.bDoorAoiF[DOOR_FR_AOI_UP])
		{
			ulOpenDoor |= (0x01 << 2);
			pDoc->Status.bDoorAoiF[DOOR_FR_AOI_UP] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_FR_AOI_UP] && pDoc->Status.bDoorAoiF[DOOR_FR_AOI_UP])
		{
			ulOpenDoor &= ~(0x01 << 2);
			pDoc->Status.bDoorAoiF[DOOR_FR_AOI_UP] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �˻�� �� ���� ���� ���� Open"));
			//TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorAoi[DOOR_BM_AOI_UP] && !pDoc->Status.bDoorAoiF[DOOR_BM_AOI_UP])
		{
			ulOpenDoor |= (0x01 << 3);
			pDoc->Status.bDoorAoiF[DOOR_BM_AOI_UP] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_BM_AOI_UP] && pDoc->Status.bDoorAoiF[DOOR_BM_AOI_UP])
		{
			ulOpenDoor &= ~(0x01 << 3);
			pDoc->Status.bDoorAoiF[DOOR_BM_AOI_UP] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �˻�� �� �ĸ� �߾� ���� Open"));
			//TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorAoi[DOOR_BL_AOI_UP] && !pDoc->Status.bDoorAoiF[DOOR_BL_AOI_UP])
		{
			ulOpenDoor |= (0x01 << 4);
			pDoc->Status.bDoorAoiF[DOOR_BL_AOI_UP] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_BL_AOI_UP] && pDoc->Status.bDoorAoiF[DOOR_BL_AOI_UP])
		{
			ulOpenDoor &= ~(0x01 << 4);
			pDoc->Status.bDoorAoiF[DOOR_BL_AOI_UP] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �˻�� �� �ĸ� ���� ���� Open"));
			//TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorAoi[DOOR_BR_AOI_UP] && !pDoc->Status.bDoorAoiF[DOOR_BR_AOI_UP])
		{
			ulOpenDoor |= (0x01 << 5);
			pDoc->Status.bDoorAoiF[DOOR_BR_AOI_UP] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_BR_AOI_UP] && pDoc->Status.bDoorAoiF[DOOR_BR_AOI_UP])
		{
			ulOpenDoor &= ~(0x01 << 5);
			pDoc->Status.bDoorAoiF[DOOR_BR_AOI_UP] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �˻�� �� �ĸ� ���� ���� Open"));
			//TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}
	}

	if (pDoc->WorkingInfo.LastJob.bAoiDnDrSen)
	{
		if (pDoc->Status.bDoorAoi[DOOR_FM_AOI_DN] && !pDoc->Status.bDoorAoiF[DOOR_FM_AOI_DN])
		{
			ulOpenDoor |= (0x01 << 0);
			pDoc->Status.bDoorAoiF[DOOR_FM_AOI_DN] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_FM_AOI_DN] && pDoc->Status.bDoorAoiF[DOOR_FM_AOI_DN])
		{
			ulOpenDoor &= ~(0x01 << 0);
			pDoc->Status.bDoorAoiF[DOOR_FM_AOI_DN] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �˻�� �� ���� �߾� ���� Open"));
			//TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorAoi[DOOR_FL_AOI_DN] && !pDoc->Status.bDoorAoiF[DOOR_FL_AOI_DN])
		{
			ulOpenDoor |= (0x01 << 1);
			pDoc->Status.bDoorAoiF[DOOR_FL_AOI_DN] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_FL_AOI_DN] && pDoc->Status.bDoorAoiF[DOOR_FL_AOI_DN])
		{
			ulOpenDoor &= ~(0x01 << 1);
			pDoc->Status.bDoorAoiF[DOOR_FL_AOI_DN] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �˻�� �� ���� ���� ���� Open"));
			//TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorAoi[DOOR_FR_AOI_DN] && !pDoc->Status.bDoorAoiF[DOOR_FR_AOI_DN])
		{
			ulOpenDoor |= (0x01 << 2);
			pDoc->Status.bDoorAoiF[DOOR_FR_AOI_DN] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_FR_AOI_DN] && pDoc->Status.bDoorAoiF[DOOR_FR_AOI_DN])
		{
			ulOpenDoor &= ~(0x01 << 2);
			pDoc->Status.bDoorAoiF[DOOR_FR_AOI_DN] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �˻�� �� ���� ���� ���� Open"));
			//TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorAoi[DOOR_BM_AOI_DN] && !pDoc->Status.bDoorAoiF[DOOR_BM_AOI_DN])
		{
			ulOpenDoor |= (0x01 << 3);
			pDoc->Status.bDoorAoiF[DOOR_BM_AOI_DN] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_BM_AOI_DN] && pDoc->Status.bDoorAoiF[DOOR_BM_AOI_DN])
		{
			ulOpenDoor &= ~(0x01 << 3);
			pDoc->Status.bDoorAoiF[DOOR_BM_AOI_DN] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �˻�� �� �ĸ� �߾� ���� Open"));
			//TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorAoi[DOOR_BL_AOI_DN] && !pDoc->Status.bDoorAoiF[DOOR_BL_AOI_DN])
		{
			ulOpenDoor |= (0x01 << 4);
			pDoc->Status.bDoorAoiF[DOOR_BL_AOI_DN] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_BL_AOI_DN] && pDoc->Status.bDoorAoiF[DOOR_BL_AOI_DN])
		{
			ulOpenDoor &= ~(0x01 << 4);
			pDoc->Status.bDoorAoiF[DOOR_BL_AOI_DN] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �˻�� �� �ĸ� ���� ���� Open"));
			//TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorAoi[DOOR_BR_AOI_DN] && !pDoc->Status.bDoorAoiF[DOOR_BR_AOI_DN])
		{
			ulOpenDoor |= (0x01 << 5);
			pDoc->Status.bDoorAoiF[DOOR_BR_AOI_DN] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_BR_AOI_DN] && pDoc->Status.bDoorAoiF[DOOR_BR_AOI_DN])
		{
			ulOpenDoor &= ~(0x01 << 5);
			pDoc->Status.bDoorAoiF[DOOR_BR_AOI_DN] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �˻�� �� �ĸ� ���� ���� Open"));
			//TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}
	}

	if (pDoc->WorkingInfo.LastJob.bMkDrSen)
	{
		if (pDoc->Status.bDoorMk[DOOR_FL_MK] && !pDoc->Status.bDoorMkF[DOOR_FL_MK])
		{
			ulOpenDoor |= (0x01 << 6);
			pDoc->Status.bDoorMkF[DOOR_FL_MK] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorMk[DOOR_FL_MK] && pDoc->Status.bDoorMkF[DOOR_FL_MK])
		{
			ulOpenDoor &= ~(0x01 << 6);
			pDoc->Status.bDoorMkF[DOOR_FL_MK] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - ��ŷ�� ���� ���� ���� Open"));
			//TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorMk[DOOR_FR_MK] && !pDoc->Status.bDoorMkF[DOOR_FR_MK])
		{
			ulOpenDoor |= (0x01 << 7);
			pDoc->Status.bDoorMkF[DOOR_FR_MK] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorMk[DOOR_FR_MK] && pDoc->Status.bDoorMkF[DOOR_FR_MK])
		{
			ulOpenDoor &= ~(0x01 << 7);
			pDoc->Status.bDoorMkF[DOOR_FR_MK] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - ��ŷ�� ���� ���� ���� Open"));
			//TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorMk[DOOR_BL_MK] && !pDoc->Status.bDoorMkF[DOOR_BL_MK])
		{
			ulOpenDoor |= (0x01 << 8);
			pDoc->Status.bDoorMkF[DOOR_BL_MK] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorMk[DOOR_BL_MK] && pDoc->Status.bDoorMkF[DOOR_BL_MK])
		{
			ulOpenDoor &= ~(0x01 << 8);
			pDoc->Status.bDoorMkF[DOOR_BL_MK] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - ��ŷ�� �ĸ� ���� ���� Open"));
			//TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorMk[DOOR_BR_MK] && !pDoc->Status.bDoorMkF[DOOR_BR_MK])
		{
			ulOpenDoor |= (0x01 << 9);
			pDoc->Status.bDoorMkF[DOOR_BR_MK] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorMk[DOOR_BR_MK] && pDoc->Status.bDoorMkF[DOOR_BR_MK])
		{
			ulOpenDoor &= ~(0x01 << 9);
			pDoc->Status.bDoorMkF[DOOR_BR_MK] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - ��ŷ�� �ĸ� ���� ���� Open"));
			//TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}
	}

	if (pDoc->WorkingInfo.LastJob.bEngvDrSen)
	{
		if (pDoc->Status.bDoorEngv[DOOR_FL_ENGV] && !pDoc->Status.bDoorEngvF[DOOR_FL_ENGV])
		{
			ulOpenDoor |= (0x01 << 6);
			pDoc->Status.bDoorEngvF[DOOR_FL_ENGV] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorEngv[DOOR_FL_ENGV] && pDoc->Status.bDoorEngvF[DOOR_FL_ENGV])
		{
			ulOpenDoor &= ~(0x01 << 6);
			pDoc->Status.bDoorEngvF[DOOR_FL_ENGV] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - ���κ� ���� ���� ���� Open"));
			//TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorEngv[DOOR_FR_ENGV] && !pDoc->Status.bDoorEngvF[DOOR_FR_ENGV])
		{
			ulOpenDoor |= (0x01 << 7);
			pDoc->Status.bDoorEngvF[DOOR_FR_ENGV] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorEngv[DOOR_FR_ENGV] && pDoc->Status.bDoorEngvF[DOOR_FR_ENGV])
		{
			ulOpenDoor &= ~(0x01 << 7);
			pDoc->Status.bDoorEngvF[DOOR_FR_ENGV] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - ���κ� ���� ���� ���� Open"));
			//TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorEngv[DOOR_BL_ENGV] && !pDoc->Status.bDoorEngvF[DOOR_BL_ENGV])
		{
			ulOpenDoor |= (0x01 << 8);
			pDoc->Status.bDoorEngvF[DOOR_BL_ENGV] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorEngv[DOOR_BL_ENGV] && pDoc->Status.bDoorEngvF[DOOR_BL_ENGV])
		{
			ulOpenDoor &= ~(0x01 << 8);
			pDoc->Status.bDoorEngvF[DOOR_BL_ENGV] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - ���κ� �ĸ� ���� ���� Open"));
			//TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorEngv[DOOR_BR_ENGV] && !pDoc->Status.bDoorEngvF[DOOR_BR_ENGV])
		{
			ulOpenDoor |= (0x01 << 9);
			pDoc->Status.bDoorEngvF[DOOR_BR_ENGV] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorEngv[DOOR_BR_ENGV] && pDoc->Status.bDoorEngvF[DOOR_BR_ENGV])
		{
			ulOpenDoor &= ~(0x01 << 9);
			pDoc->Status.bDoorEngvF[DOOR_BR_ENGV] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - ���κ� �ĸ� ���� ���� Open"));
			//TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}
	}

	if (pDoc->WorkingInfo.LastJob.bUclDrSen)
	{
		if (pDoc->Status.bDoorUc[DOOR_FL_UC] && !pDoc->Status.bDoorUcF[DOOR_FL_UC])
		{
			ulOpenDoor |= (0x01 << 10);
			pDoc->Status.bDoorUcF[DOOR_FL_UC] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorUc[DOOR_FL_UC] && pDoc->Status.bDoorUcF[DOOR_FL_UC])
		{
			ulOpenDoor &= ~(0x01 << 10);
			pDoc->Status.bDoorUcF[DOOR_FL_UC] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �����Ϸ��� ���� ���� ���� Open"));
			//TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorUc[DOOR_FR_UC] && !pDoc->Status.bDoorUcF[DOOR_FR_UC])
		{
			ulOpenDoor |= (0x01 << 11);
			pDoc->Status.bDoorUcF[DOOR_FR_UC] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorUc[DOOR_FR_UC] && pDoc->Status.bDoorUcF[DOOR_FR_UC])
		{
			ulOpenDoor &= ~(0x01 << 11);
			pDoc->Status.bDoorUcF[DOOR_FR_UC] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �����Ϸ��� ���� ���� Open"));
			//TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorUc[DOOR_BL_UC] && !pDoc->Status.bDoorUcF[DOOR_BL_UC])
		{
			ulOpenDoor |= (0x01 << 12);
			pDoc->Status.bDoorUcF[DOOR_BL_UC] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorUc[DOOR_BL_UC] && pDoc->Status.bDoorUcF[DOOR_BL_UC])
		{
			ulOpenDoor &= ~(0x01 << 12);
			pDoc->Status.bDoorUcF[DOOR_BL_UC] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �����Ϸ��� �ĸ� ���� ���� Open"));
			//TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorUc[DOOR_BR_UC] && !pDoc->Status.bDoorUcF[DOOR_BR_UC])
		{
			ulOpenDoor |= (0x01 << 13);
			pDoc->Status.bDoorUcF[DOOR_BR_UC] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorUc[DOOR_BR_UC] && pDoc->Status.bDoorUcF[DOOR_BR_UC])
		{
			ulOpenDoor &= ~(0x01 << 13);
			pDoc->Status.bDoorUcF[DOOR_BR_UC] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �����Ϸ��� �ĸ� ���� ���� Open"));
			//TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}
	}

	if (pDoc->WorkingInfo.LastJob.bRclDrSen)
	{
		if (pDoc->Status.bDoorRe[DOOR_FR_RC] && !pDoc->Status.bDoorReF[DOOR_FR_RC])
		{
			ulOpenDoor |= (0x01 << 15);
			pDoc->Status.bDoorReF[DOOR_FR_RC] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorRe[DOOR_FR_RC] && pDoc->Status.bDoorReF[DOOR_FR_RC])
		{
			ulOpenDoor &= ~(0x01 << 15);
			pDoc->Status.bDoorReF[DOOR_FR_RC] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			pView->DispStsBar(_T("����-19"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �����Ϸ��� ���� ���� ���� Open"));
			//TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorRe[DOOR_S_RC] && !pDoc->Status.bDoorReF[DOOR_S_RC])
		{
			ulOpenDoor |= (0x01 << 16);
			pDoc->Status.bDoorReF[DOOR_S_RC] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorRe[DOOR_S_RC] && pDoc->Status.bDoorReF[DOOR_S_RC])
		{
			ulOpenDoor &= ~(0x01 << 16);
			pDoc->Status.bDoorReF[DOOR_S_RC] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �����Ϸ��� ���� ���� Open"));
			//TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorRe[DOOR_BL_RC] && !pDoc->Status.bDoorReF[DOOR_BL_RC])
		{
			ulOpenDoor |= (0x01 << 17);
			pDoc->Status.bDoorReF[DOOR_BL_RC] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorRe[DOOR_BL_RC] && pDoc->Status.bDoorReF[DOOR_BL_RC])
		{
			ulOpenDoor &= ~(0x01 << 17);
			pDoc->Status.bDoorReF[DOOR_BL_RC] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �����Ϸ��� �ĸ� ���� ���� Open"));
			//TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorRe[DOOR_BR_RC] && !pDoc->Status.bDoorReF[DOOR_BR_RC])
		{
			ulOpenDoor |= (0x01 << 18);
			pDoc->Status.bDoorReF[DOOR_BR_RC] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorRe[DOOR_BR_RC] && pDoc->Status.bDoorReF[DOOR_BR_RC])
		{
			ulOpenDoor &= ~(0x01 << 18);
			pDoc->Status.bDoorReF[DOOR_BR_RC] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �����Ϸ��� �ĸ� ���� ���� Open"));
			//TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}
	}

	return ulOpenDoor;
}

void CManagerPunch::Buzzer(BOOL bOn, int nCh)
{
	//	return; // PLC����
#ifdef USE_MPE
	if (!m_pMpe)
		return;

	if (!bOn)
	{
		switch (nCh)
		{
		case 0:
			MpeWrite(_T("MB44015E"), 0);
			break;
		case 1:
			MpeWrite(_T("MB44015F"), 0);
			break;
		}
	}
	else
	{
		switch (nCh)
		{
		case 0:
			MpeWrite(_T("MB44015E"), 0);
			Sleep(300);
			MpeWrite(_T("MB44015E"), 1);
			break;
		case 1:
			MpeWrite(_T("MB44015E"), 0);
			Sleep(300);
			MpeWrite(_T("MB44015F"), 1);
			break;
		}
	}
#endif
}

CString CManagerPunch::GetAoiUpAlarmRestartMsg()
{
	CString sMsg = _T("Not Find Message.");
	TCHAR szData[512];
	CString sPath = PATH_ALARM;
	if (0 < ::GetPrivateProfileString(_T("11"), _T("27"), NULL, szData, sizeof(szData), sPath))
		sMsg = CString(szData);

	return sMsg;
}

void CManagerPunch::ChkReTestAlarmOnAoiUp()
{
	int nSerial = pView->m_mgrProcedure->m_pBufSerial[0][pView->m_mgrProcedure->m_nBufTot[0] - 1];

	if (pView->m_mgrProcedure->m_bSerialDecrese)
	{
		if (pView->m_mgrProcedure->m_nLotEndSerial > 0 && nSerial > pView->m_mgrProcedure->m_nLotEndSerial)
		{
			//if (pDoc->m_sAlmMsg == GetAoiUpAlarmRestartMsg())
			{
				pView->SetAoiUpAutoStep(2); // Wait for AOI �˻���� ��ȣ.
				Sleep(300);
				if (m_pMpe)
					MpeWrite(_T("MB44013B"), 1); // �˻�� ��� ���۾� (���۽�ȣ) : PC�� On��Ű�� PLC�� Off
			}
		}
		else if (pView->m_mgrProcedure->m_nLotEndSerial > 0 && nSerial <= pView->m_mgrProcedure->m_nLotEndSerial)
		{
			if (m_pMpe)
				MpeWrite(_T("MB44012B"), 1); // AOI �� : PCR���� Received
		}
	}
	else
	{
		if (pView->m_mgrProcedure->m_nLotEndSerial > 0 && nSerial < pView->m_mgrProcedure->m_nLotEndSerial)
		{
			//if (pDoc->m_sAlmMsg == GetAoiUpAlarmRestartMsg())
			{
				pView->SetAoiUpAutoStep(2); // Wait for AOI �˻���� ��ȣ.
				Sleep(300);
				if (m_pMpe)
					MpeWrite(_T("MB44013B"), 1); // �˻�� ��� ���۾� (���۽�ȣ) : PC�� On��Ű�� PLC�� Off
			}
		}
		else if (pView->m_mgrProcedure->m_nLotEndSerial > 0 && nSerial >= pView->m_mgrProcedure->m_nLotEndSerial)
		{
			if (m_pMpe)
				MpeWrite(_T("MB44012B"), 1); // AOI �� : PCR���� Received
		}
	}

}

CString CManagerPunch::GetAoiDnAlarmRestartMsg()
{
	CString sMsg = _T("Not Find Message.");
	TCHAR szData[512];
	CString sPath = PATH_ALARM;
	if (0 < ::GetPrivateProfileString(_T("11"), _T("28"), NULL, szData, sizeof(szData), sPath))
		sMsg = CString(szData);

	return sMsg;
}

void CManagerPunch::ChkReTestAlarmOnAoiDn()
{
	int nSerial = pView->m_mgrProcedure->m_pBufSerial[1][pView->m_mgrProcedure->m_nBufTot[1] - 1];

	if (pView->m_mgrProcedure->m_bSerialDecrese)
	{
		if (pView->m_mgrProcedure->m_nLotEndSerial > 0 && nSerial > pView->m_mgrProcedure->m_nLotEndSerial)
		{
			//if (pDoc->m_sAlmMsg == GetAoiDnAlarmRestartMsg())
			{
				pView->SetAoiDnAutoStep(2); // Wait for AOI �˻���� ��ȣ.
				Sleep(300);
				if (m_pMpe)
					MpeWrite(_T("MB44013C"), 1); // �˻�� �Ϻ� ���۾� (���۽�ȣ) : PC�� On��Ű�� PLC�� Off
			}
		}
		else if (pView->m_mgrProcedure->m_nLotEndSerial > 0 && nSerial <= pView->m_mgrProcedure->m_nLotEndSerial)
		{
			//if (m_pMpe)
			//	MpeWrite(_T("MB44012C"), 1); // AOI �� : PCR���� Received
		}
	}
	else
	{
		if (pView->m_mgrProcedure->m_nLotEndSerial > 0 && nSerial < pView->m_mgrProcedure->m_nLotEndSerial)
		{
			//if (pDoc->m_sAlmMsg == GetAoiDnAlarmRestartMsg())
			{
				pView->SetAoiDnAutoStep(2); // Wait for AOI �˻���� ��ȣ.
				Sleep(300);
				if (m_pMpe)
					MpeWrite(_T("MB44013C"), 1); // �˻�� �Ϻ� ���۾� (���۽�ȣ) : PC�� On��Ű�� PLC�� Off
			}
		}
		else if (pView->m_mgrProcedure->m_nLotEndSerial > 0 && nSerial >= pView->m_mgrProcedure->m_nLotEndSerial)
		{
			//if (m_pMpe)
			//	MpeWrite(_T("MB44012C"), 1); // AOI �� : PCR���� Received
		}
	}

}

void CManagerPunch::DoAutoEng()
{
	//if (!IsAuto() || (MODE_INNER != pDoc->WorkingInfo.LastJob.nTestMode))
	if (!IsAuto() || (MODE_INNER != pDoc->GetTestMode()))
		return;

	// ���κ� ��ŷ���� ��ȣ�� Ȯ��
	DoAtuoGetEngStSignal();

	// ���κ� 2D �ڵ� Reading��ȣ�� Ȯ��
	DoAtuoGet2dReadStSignal();
}

BOOL CManagerPunch::IsPinData()
{
	return pDoc->IsPinData();
}

void CManagerPunch::SetBufInitPos(double dPos)
{
	pDoc->SetBufInitPos(dPos);
}

void CManagerPunch::SetBufHomeParam(double dVel, double dAcc)
{
	long lVel = long(dVel*1000.0);
	long lAcc = long(dAcc*1000.0);
	//	MpeWrite(_T("ML00000"), lVel); // ��ŷ�� ���� Ȩ �ӵ�
	//	MpeWrite(_T("ML00000"), lAcc); // ��ŷ�� ���� Ȩ ���ӵ�
	//	pDoc->SetBufInitPos(dVel, dAcc);
}

void CManagerPunch::SetLight(int nVal)
{
	if (m_pLight)
	{
		m_pLight->Set(_tstoi(pDoc->WorkingInfo.Light.sCh[0]), nVal);

		//if (nVal)
		//{
		//	if (!myBtn[0].GetCheck())
		//		myBtn[0].SetCheck(TRUE);
		//}
		//else
		//{
		//	if (myBtn[0].GetCheck())
		//		myBtn[0].SetCheck(FALSE);
		//}

		nVal = m_pLight->Get(_tstoi(pDoc->WorkingInfo.Light.sCh[0]));

		//int nPos;
		//nPos = int(100.0*(1.0 - (nVal - 0.5) / 255.0));
		//m_LightSlider.SetPos(nPos);
		//CString str;
		//str.Format(_T("%d"), nVal);
		//myStcData[2].SetText(str);

		//pDoc->WorkingInfo.Light.sVal[0] = str;
		//::WritePrivateProfileString(_T("Light0"), _T("LIGHT_VALUE"), str, PATH_WORKING_INFO);
	}

}

void CManagerPunch::SetLight2(int nVal)
{
	if (m_pLight)
	{
		m_pLight->Set(_tstoi(pDoc->WorkingInfo.Light.sCh[1]), nVal);

		//if (nVal)
		//{
		//	if (!myBtn2[0].GetCheck())
		//		myBtn2[0].SetCheck(TRUE);
		//}
		//else
		//{
		//	if (myBtn2[0].GetCheck())
		//		myBtn2[0].SetCheck(FALSE);
		//}

		nVal = m_pLight->Get(_tstoi(pDoc->WorkingInfo.Light.sCh[1]));

		//int nPos;
		//nPos = int(100.0*(1.0 - (nVal - 0.5) / 255.0));
		//m_LightSlider2.SetPos(nPos);
		//CString str;
		//str.Format(_T("%d"), nVal);
		//myStcData2[2].SetText(str);

		//pDoc->WorkingInfo.Light.sVal[1] = str;
		//::WritePrivateProfileString(_T("Light1"), _T("LIGHT_VALUE"), str, PATH_WORKING_INFO);
	}
}

void CManagerPunch::DispMain(CString sMsg, COLORREF rgb)
{
	pView->DispMain(sMsg, rgb);
}

void CManagerPunch::Stop()
{
	pView->Stop();
}

void CManagerPunch::SetDelay(int mSec, int nId)
{
	if (nId > 10)
		nId = 9;
	else if (nId < 0)
		nId = 0;

	m_dwSetDlySt[nId] = GetTickCount();
	m_dwSetDlyEd[nId] = m_dwSetDlySt[nId] + mSec;
}

void CManagerPunch::SetDelay0(int mSec, int nId)
{
	if (nId > 10)
		nId = 9;
	else if (nId < 0)
		nId = 0;

	m_dwSetDlySt0[nId] = GetTickCount();
	m_dwSetDlyEd0[nId] = m_dwSetDlySt0[nId] + mSec;
}

void CManagerPunch::SetDelay1(int mSec, int nId)
{
	if (nId > 10)
		nId = 9;
	else if (nId < 0)
		nId = 0;

	m_dwSetDlySt1[nId] = GetTickCount();
	m_dwSetDlyEd1[nId] = m_dwSetDlySt1[nId] + mSec;
}

BOOL CManagerPunch::WaitDelay(int nId) // F:Done, T:On Waiting....
{
	if (nId > 10)
		nId = 9;
	else if (nId < 0)
		nId = 0;

	DWORD dwCur = GetTickCount();
	if (dwCur < m_dwSetDlyEd[nId])
		return TRUE;
	return FALSE;
}

BOOL CManagerPunch::WaitDelay0(int nId) // F:Done, T:On Waiting....
{
	if (nId > 10)
		nId = 9;
	else if (nId < 0)
		nId = 0;

	DWORD dwCur = GetTickCount();
	if (dwCur < m_dwSetDlyEd0[nId])
		return TRUE;
	return FALSE;
}

BOOL CManagerPunch::WaitDelay1(int nId) // F:Done, T:On Waiting....
{
	if (nId > 10)
		nId = 9;
	else if (nId < 0)
		nId = 0;

	DWORD dwCur = GetTickCount();
	if (dwCur < m_dwSetDlyEd1[nId])
		return TRUE;
	return FALSE;
}

BOOL CManagerPunch::GetDelay(int &mSec, int nId) // F:Done, T:On Waiting....
{
	DWORD dwCur = GetTickCount();
	mSec = int(dwCur - m_dwSetDlySt[nId]);
	if (dwCur < m_dwSetDlyEd[nId])
		return TRUE;
	return FALSE;
}

BOOL CManagerPunch::GetDelay0(int &mSec, int nId) // F:Done, T:On Waiting....
{
	DWORD dwCur = GetTickCount();
	mSec = int(dwCur - m_dwSetDlySt0[nId]);
	if (dwCur < m_dwSetDlyEd0[nId])
		return TRUE;
	return FALSE;
}

BOOL CManagerPunch::GetDelay1(int &mSec, int nId) // F:Done, T:On Waiting....
{
	DWORD dwCur = GetTickCount();
	mSec = int(dwCur - m_dwSetDlySt1[nId]);
	if (dwCur < m_dwSetDlyEd1[nId])
		return TRUE;
	return FALSE;
}

void CManagerPunch::CntMk()
{
#ifdef USE_MPE
	if (m_nPrevTotMk[0] != m_nTotMk[0])
	{
		m_nPrevTotMk[0] = m_nTotMk[0];
		MpeWrite(_T("ML45096"), (long)m_nTotMk[0]);	// ��ŷ�� (��) �� ��ŷ�� 
	}
	if (m_nPrevCurMk[0] != m_nMkPcs[0])//m_nCurMk[0])
	{
		m_nPrevCurMk[0] = m_nMkPcs[0];//m_nCurMk[0];
		MpeWrite(_T("ML45098"), (long)m_nMkPcs[0]);	// ��ŷ�� (��) ���� ��ŷ�� ��
	}

	if (m_nPrevTotMk[1] != m_nTotMk[1])
	{
		m_nPrevTotMk[1] = m_nTotMk[1];
		MpeWrite(_T("ML45100"), (long)m_nTotMk[1]);	// ��ŷ�� (��) �� ��ŷ�� 
	}
	if (m_nPrevCurMk[1] != m_nMkPcs[1])//m_nCurMk[1])
	{
		m_nPrevCurMk[1] = m_nMkPcs[1];//m_nCurMk[1];
		MpeWrite(_T("ML45102"), (long)m_nMkPcs[1]);	// ��ŷ�� (��) ���� ��ŷ�� ��
	}
#endif
}


void CManagerPunch::InitThread()
{
	// DoMark0(), DoMark1()                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
	if (!m_bThread[0])
		m_Thread[0].Start(GetSafeHwnd(), this, ThreadProc0);

	// ChkCollision
	if (!m_bThread[1])
		m_Thread[1].Start(GetSafeHwnd(), this, ThreadProc1);
}

UINT CManagerPunch::ThreadProc0(LPVOID lpContext)	// DoMark0(), DoMark1()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerPunch* pThread = reinterpret_cast<CManagerPunch*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[0] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[0].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[0] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (!bLock)
		{
			bLock = TRUE;
#ifndef TEST_MODE
			//if (m_Master[0].MasterInfo.nActionCode == 1 || m_Master[0].MasterInfo.nActionCode == 3)	// 0 : Rotation / Mirror ���� ����(CAM Data ����), 1 : �¿� �̷�, 2 : ���� �̷�, 3 : 180 ȸ��, 4 : 270 ȸ��(CCW), 5 : 90 ȸ��(CW)
			if (pView->GetCamMstActionCode() == 1 || pView->GetCamMstActionCode() == 3)	// 0 : Rotation / Mirror ���� ����(CAM Data ����), 1 : �¿� �̷�, 2 : ���� �̷�, 3 : 180 ȸ��, 4 : 270 ȸ��(CCW), 5 : 90 ȸ��(CW)
			{
				if (pThread->m_bTHREAD_MK[0])
				{
					if (pView->m_mgrProcedure)
					{
						if (pView->m_mgrProcedure->m_nBufUpSerial[0] > 0)
						{
							if (pDoc->GetTestMode() == MODE_OUTER)
								pThread->DoMark0Its();
							else
								pThread->DoMark0();
						}
						else
						{
							pThread->m_bDoneMk[0] = TRUE;
							pThread->m_bTHREAD_MK[0] = FALSE;
						}
					}
				}
				if (pThread->m_bTHREAD_MK[1])
				{
					if (pView->m_mgrProcedure)
					{
						if (pView->m_mgrProcedure->m_nBufUpSerial[1] > 0)
						{
							if (pDoc->GetTestMode() == MODE_OUTER)
								pThread->DoMark1Its();
							else
								pThread->DoMark1();
						}
						else
						{
							pThread->m_bDoneMk[1] = TRUE;
							pThread->m_bTHREAD_MK[1] = FALSE;
						}
					}
				}
			}
			else
			{
				if (pThread->m_bTHREAD_MK[1])
				{
					if (pView->m_mgrProcedure)
					{
						if (pView->m_mgrProcedure->m_nBufUpSerial[1] > 0)
							pThread->DoMark1();
						else
						{
							pThread->m_bDoneMk[1] = TRUE;
							pThread->m_bTHREAD_MK[1] = FALSE;
						}
					}
				}
				if (pThread->m_bTHREAD_MK[0])
				{
					if (pView->m_mgrProcedure)
					{
						if (pView->m_mgrProcedure->m_nBufUpSerial[0] > 0)
							pThread->DoMark0();
						else
						{
							pThread->m_bDoneMk[0] = TRUE;
							pThread->m_bTHREAD_MK[0] = FALSE;
						}
					}
				}
			}
#else
			pThread->DoMark1();
			pThread->DoMark0();
			Sleep(100);
#endif
			if (pThread->m_bTHREAD_MK[3])
				pThread->DoReject1();

			if (pThread->m_bTHREAD_MK[2])
				pThread->DoReject0();

			if (!pThread->m_bTHREAD_MK[0] && !pThread->m_bTHREAD_MK[1] &&
				!pThread->m_bTHREAD_MK[2] && !pThread->m_bTHREAD_MK[3])
				Sleep(50);

			bLock = FALSE;
		}
		Sleep(10);
	}
	pThread->m_bThread[0] = FALSE;

	return 0;
}

UINT CManagerPunch::ThreadProc1(LPVOID lpContext)	// ChkCollision()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CManagerPunch* pThread = reinterpret_cast<CManagerPunch*>(lpContext);

	BOOL bLock = FALSE, bEStop = FALSE, bCollision = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[1] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[1].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[1] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (!bLock)
		{
			bLock = TRUE;

			//pThread->ChkCollision();
			pThread->GetEnc();

			if (!pThread->m_bTHREAD_MK[0] && !pThread->m_bTHREAD_MK[1] &&
				!pThread->m_bTHREAD_MK[2] && !pThread->m_bTHREAD_MK[3])
			{
				if (pThread->ChkCollision() && !bEStop)
				{
					if (pThread->IsRunAxisX())
					{
						bEStop = TRUE;
						pThread->EStop();
					}
				}
				else if (!pThread->ChkCollision() && bEStop)
				{
					bEStop = FALSE;
				}
			}
			else
			{
				if (pThread->m_bCollision[0] && pThread->m_bCollision[1])
				{
					if (!bCollision)
					{
						bCollision = TRUE;
						pThread->SetPriority();
					}
				}
				else
					bCollision = FALSE;
			}

			bLock = FALSE;
		}
		Sleep(10);
	}
	pThread->m_bThread[1] = FALSE;

	return 0;
}


void CManagerPunch::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if (nIDEvent == TIM_MPE_IO)
	{
		KillTimer(TIM_MPE_IO);

		CntMk();
		GetMpeIO();
		GetMpeSignal();
		DoIO();
		 
		if (m_bStopFromThread)
		{
			m_bStopFromThread = FALSE;
			Stop();
		}
		if (m_bBuzzerFromThread)
		{
			m_bBuzzerFromThread = FALSE;
			Buzzer(TRUE, 0);
		}

		ChkEmg();
		ChkSaftySen();
		ChkDoor();

		if (m_bTIM_MPE_IO)
			SetTimer(TIM_MPE_IO, 100, NULL);
	}

	CWnd::OnTimer(nIDEvent);
}

BOOL CManagerPunch::IsReMk()
{
	if (IsMoveDone())
		return FALSE;

	if (m_bReMark[0] && m_bDoneMk[1])
	{
		m_bReMark[0] = FALSE;
		SetReMk(TRUE, FALSE);
		return TRUE;
	}
	else if (m_bDoneMk[0] && m_bReMark[1])
	{
		m_bReMark[1] = FALSE;
		SetReMk(FALSE, TRUE);
		return TRUE;
	}
	else if (m_bReMark[0] && m_bReMark[1])
	{
		m_bReMark[0] = FALSE;
		m_bReMark[1] = FALSE;
		SetReMk(TRUE, TRUE);
		return TRUE;
	}

	return FALSE;
}

void CManagerPunch::SetReMk(BOOL bMk0, BOOL bMk1)
{
	CfPoint ptPnt;
	int nSerial, nTot;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (bMk0)
	{
		if (!m_bTHREAD_MK[0])
		{
			m_nStepMk[0] = 0;
			m_nMkPcs[0] = 0;
			m_bDoneMk[0] = FALSE;
			//m_bReMark[0] = FALSE;
			m_bTHREAD_MK[0] = TRUE;

			nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Cam0

			nTot = GetTotDefPcs(nSerial);
			if (nTot > 0)
			{
				ptPnt = pView->GetMkPnt(nSerial, 0);
				m_dTarget[AXIS_X0] = ptPnt.x;
				m_dTarget[AXIS_Y0] = ptPnt.y;
				if (nTot > 1)
				{
					ptPnt = pView->GetMkPnt(nSerial, 1);
					m_dNextTarget[AXIS_X0] = ptPnt.x;
					m_dNextTarget[AXIS_Y0] = ptPnt.y;
				}
				else
				{
					m_dNextTarget[AXIS_X0] = -1.0;
					m_dNextTarget[AXIS_Y0] = -1.0;
				}
			}
			else
			{
				m_dTarget[AXIS_X0] = -1.0;
				m_dTarget[AXIS_Y0] = -1.0;
				m_dNextTarget[AXIS_X0] = -1.0;
				m_dNextTarget[AXIS_Y0] = -1.0;
			}
		}
	}

	if (bMk1)
	{
		if (!m_bTHREAD_MK[1])
		{
			m_nStepMk[1] = 0;
			m_nMkPcs[1] = 0;
			m_bDoneMk[1] = FALSE;
			//m_bReMark[1] = FALSE;
			m_bTHREAD_MK[1] = TRUE;

			nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

			nTot = GetTotDefPcs(nSerial);
			if (nTot > 0)
			{
				ptPnt = pView->GetMkPnt(nSerial, 0);
				m_dTarget[AXIS_X1] = ptPnt.x;
				m_dTarget[AXIS_Y1] = ptPnt.y;
				if (nTot > 1)
				{
					ptPnt = pView->GetMkPnt(nSerial, 1);
					m_dNextTarget[AXIS_X1] = ptPnt.x;
					m_dNextTarget[AXIS_Y1] = ptPnt.y;
				}
				else
				{
					m_dNextTarget[AXIS_X1] = -1.0;
					m_dNextTarget[AXIS_Y1] = -1.0;
				}
			}
			else
			{
				m_dTarget[AXIS_X1] = -1.0;
				m_dTarget[AXIS_Y1] = -1.0;
				m_dNextTarget[AXIS_X1] = -1.0;
				m_dNextTarget[AXIS_Y1] = -1.0;
			}
		}
	}
}

BOOL CManagerPunch::InitMk()
{
	m_nStepElecChk = 0;
	int nRSer = pView->ChkSerial(); // (nSerial0 - nLastShot) -> 0: Same Serial, -: Decrese Serial, +: Increase Serial 
	int nSerial = pView->m_mgrProcedure->GetBuffer0();

	if (nRSer)
	{
		pView->m_mgrProcedure->m_bSerialDecrese = FALSE;

		if (pDoc->m_bUseRTRYShiftAdjust)
		{
			if (nSerial > 2)
				pView->AdjPinPos();
		}

		return TRUE;
	}

	return FALSE;
}

void CManagerPunch::Mk0()
{
	if (m_pVoiceCoil[0])
	{
		m_pVoiceCoil[0]->SetMark(0);
		pDoc->AddMkCntL();
		m_nCurMk[0]++;
	}
}

void CManagerPunch::Mk1()
{
	if (m_pVoiceCoil[1])
	{
		m_pVoiceCoil[1]->SetMark(1);
		pDoc->AddMkCntR();
		m_nCurMk[1]++;
	}
}

BOOL CManagerPunch::SetMk(BOOL bRun)	// Marking Start
{
	CfPoint ptPnt;
	int nSerial, nTot, a, b;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	for (a = 0; a < 2; a++)
	{
		for (b = 0; b < MAX_STRIP_NUM; b++)
		{
			m_nMkStrip[a][b] = 0;			// [nCam][nStrip]:[2][4] - [��/��][] : ��Ʈ���� ��Ī�� �ǽ� �� count
			m_bRejectDone[a][b] = FALSE;	// [nCam][nStrip]:[2][4] - [��/��][] : ��Ʈ���� ��Ī�� �ǽ� �� count�� ��Ʈ�� ��� ������ �Ϸ� ���� 
		}
	}

	if (bRun)
	{
		if (m_bDoMk[0])
		{
			if (!m_bTHREAD_MK[0])
			{
				m_nStepMk[0] = 0;
				m_nMkPcs[0] = 0;
				m_bDoneMk[0] = FALSE;
				//m_bReMark[0] = FALSE;
				m_bTHREAD_MK[0] = TRUE;

				nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Cam0

				m_nTotMk[0] = nTot = GetTotDefPcs(nSerial);
				m_nCurMk[0] = 0;
				if (nTot > 0)
				{
					ptPnt = pView->GetMkPnt(nSerial, 0);

					m_dTarget[AXIS_X0] = ptPnt.x;
					m_dTarget[AXIS_Y0] = ptPnt.y;
					if (nTot > 1)
					{
						ptPnt = pView->GetMkPnt(nSerial, 1);
						m_dNextTarget[AXIS_X0] = ptPnt.x;
						m_dNextTarget[AXIS_Y0] = ptPnt.y;
					}
					else
					{
						m_dNextTarget[AXIS_X0] = -1.0;
						m_dNextTarget[AXIS_Y0] = -1.0;
					}
				}
				else
				{
					m_dTarget[AXIS_X0] = -1.0;
					m_dTarget[AXIS_Y0] = -1.0;
					m_dNextTarget[AXIS_X0] = -1.0;
					m_dNextTarget[AXIS_Y0] = -1.0;
				}
			}
		}

		if (m_bDoMk[1])
		{
			if (!m_bTHREAD_MK[1])
			{
				m_nStepMk[1] = 0;
				m_nMkPcs[1] = 0;
				m_bDoneMk[1] = FALSE;
				//m_bReMark[1] = FALSE;
				m_bTHREAD_MK[1] = TRUE;

				nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

				m_nTotMk[1] = nTot = GetTotDefPcs(nSerial);
				m_nCurMk[1] = 0;
				if (nTot > 0)
				{
					ptPnt = pView->GetMkPnt(nSerial, 0);
					m_dTarget[AXIS_X1] = ptPnt.x;
					m_dTarget[AXIS_Y1] = ptPnt.y;
					if (nTot > 1)
					{
						ptPnt = pView->GetMkPnt(nSerial, 1);
						m_dNextTarget[AXIS_X1] = ptPnt.x;
						m_dNextTarget[AXIS_Y1] = ptPnt.y;
					}
					else
					{
						m_dNextTarget[AXIS_X1] = -1.0;
						m_dNextTarget[AXIS_Y1] = -1.0;
					}
				}
				else
				{
					m_dTarget[AXIS_X1] = -1.0;
					m_dTarget[AXIS_Y1] = -1.0;
					m_dNextTarget[AXIS_X1] = -1.0;
					m_dNextTarget[AXIS_Y1] = -1.0;
				}
			}
		}
	}
	else
	{
		m_bTHREAD_MK[0] = FALSE;
		m_bTHREAD_MK[1] = FALSE;
	}
	return TRUE;
}

BOOL CManagerPunch::SetMkIts(BOOL bRun)	// Marking Start
{
	CfPoint ptPnt;
	int nSerial, nTot, a, b;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	for (a = 0; a < 2; a++)
	{
		for (b = 0; b < MAX_STRIP_NUM; b++)
		{
			m_nMkStrip[a][b] = 0;			// [nCam][nStrip]:[2][4] - [��/��][] : ��Ʈ���� ��Ī�� �ǽ� �� count
			m_bRejectDone[a][b] = FALSE;	// [nCam][nStrip]:[2][4] - [��/��][] : ��Ʈ���� ��Ī�� �ǽ� �� count�� ��Ʈ�� ��� ������ �Ϸ� ���� 
		}
	}

	if (bRun)
	{
		if (m_bDoMk[0])
		{
			if (!m_bTHREAD_MK[0])
			{
				m_nStepMk[0] = 0;
				m_nMkPcs[0] = 0;
				m_bDoneMk[0] = FALSE;
				m_bTHREAD_MK[0] = TRUE;

				nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Cam0

				m_nTotMk[0] = nTot = pView->GetTotDefPcsIts(nSerial);
				m_nCurMk[0] = 0;
				if (nTot > 0)
				{
					ptPnt = pView->GetMkPntIts(nSerial, 0);
					//ptPnt = pView->GetMkPntIts(nSerial, 0);
					m_dTarget[AXIS_X0] = ptPnt.x;
					m_dTarget[AXIS_Y0] = ptPnt.y;
					if (nTot > 1)
					{
						ptPnt = pView->GetMkPntIts(nSerial, 1);
						m_dNextTarget[AXIS_X0] = ptPnt.x;
						m_dNextTarget[AXIS_Y0] = ptPnt.y;
					}
					else
					{
						m_dNextTarget[AXIS_X0] = -1.0;
						m_dNextTarget[AXIS_Y0] = -1.0;
					}
				}
				else
				{
					m_dTarget[AXIS_X0] = -1.0;
					m_dTarget[AXIS_Y0] = -1.0;
					m_dNextTarget[AXIS_X0] = -1.0;
					m_dNextTarget[AXIS_Y0] = -1.0;
				}
			}
		}

		if (m_bDoMk[1])
		{
			if (!m_bTHREAD_MK[1])
			{
				m_nStepMk[1] = 0;
				m_nMkPcs[1] = 0;
				m_bDoneMk[1] = FALSE;
				m_bTHREAD_MK[1] = TRUE;

				nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

				m_nTotMk[1] = nTot = pView->GetTotDefPcsIts(nSerial);
				m_nCurMk[1] = 0;
				if (nTot > 0)
				{
					ptPnt = pView->GetMkPntIts(nSerial, 0);
					m_dTarget[AXIS_X1] = ptPnt.x;
					m_dTarget[AXIS_Y1] = ptPnt.y;
					if (nTot > 1)
					{
						ptPnt = pView->GetMkPntIts(nSerial, 1);
						m_dNextTarget[AXIS_X1] = ptPnt.x;
						m_dNextTarget[AXIS_Y1] = ptPnt.y;
					}
					else
					{
						m_dNextTarget[AXIS_X1] = -1.0;
						m_dNextTarget[AXIS_Y1] = -1.0;
					}
				}
				else
				{
					m_dTarget[AXIS_X1] = -1.0;
					m_dTarget[AXIS_Y1] = -1.0;
					m_dNextTarget[AXIS_X1] = -1.0;
					m_dNextTarget[AXIS_Y1] = -1.0;
				}
			}
		}
	}
	else
	{
		m_bTHREAD_MK[0] = FALSE;
		m_bTHREAD_MK[1] = FALSE;
	}
	return TRUE;
}

BOOL CManagerPunch::SaveMk0Img(int nMkPcsIdx) // Cam0
{
	if (!pDoc->WorkingInfo.System.bSaveMkImg)
		return TRUE;

	int nSerial;
	nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Cam0

	CString sSrc, sDest, sPath;
	stModelInfo stInfo;

	sSrc.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufUp, nSerial);
	if (!pView->m_mgrReelmap->GetPcrInfo(sSrc, stInfo))
	{
		pView->DispStsBar(_T("E(2)"), 5);
		pView->ClrDispMsg();
		AfxMessageBox(_T("Error-GetPcrInfo(2)"));
		return FALSE;
	}

	if (!pDoc->MakeMkDir(stInfo))
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Error-MakeMkDir()"));
		return FALSE;
	}

	sDest.Format(_T("%s%s\\%s\\%s\\Punching"), pDoc->WorkingInfo.System.sPathOldFile, stInfo.sModel,
		stInfo.sLot, stInfo.sLayer);

	if (!pDoc->DirectoryExists(sDest))
		CreateDirectory(sDest, NULL);

	if (GetTotDefPcs(nSerial) > 0)
	{
		sPath.Format(_T("%s\\%s.tif"), sDest, pView->GetMkInfo(nSerial, nMkPcsIdx));

#ifdef USE_VISION
		if (m_pVision[0])
			return m_pVision[0]->SaveMkImg(sPath);
#endif
	}
	else
		return TRUE;

	return FALSE;
}

BOOL CManagerPunch::SaveMk1Img(int nMkPcsIdx) // Cam1
{
	if (!pDoc->WorkingInfo.System.bSaveMkImg)
		return TRUE;

	int nSerial;
	nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

	CString sSrc, sDest, sPath;
	stModelInfo stInfo;

	sSrc.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufUp, nSerial);
	if (!pView->m_mgrReelmap->GetPcrInfo(sSrc, stInfo))
	{
		pView->DispStsBar(_T("E(2)"), 5);
		pView->ClrDispMsg();
		AfxMessageBox(_T("Error-GetPcrInfo(2)"));
		return FALSE;
	}

	if (!pDoc->MakeMkDir(stInfo))
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Error-MakeMkDir()"));
		return FALSE;
	}

	//sDest.Format(_T("%s%s\\%s\\%s\\Punching\\%04d"), pDoc->WorkingInfo.System.sPathOldFile, stInfo.sModel,
	//	stInfo.sLot, stInfo.sLayer, nSerial);

	//if (!pDoc->DirectoryExists(sDest))
	//	CreateDirectory(sDest, NULL);

	//sPath.Format(_T("%s\\%d.tif"), sDest, ++pView->m_mgrProcedure->m_nSaveMk1Img);

	sDest.Format(_T("%s%s\\%s\\%s\\Punching"), pDoc->WorkingInfo.System.sPathOldFile, stInfo.sModel,
		stInfo.sLot, stInfo.sLayer);

	if (!pDoc->DirectoryExists(sDest))
		CreateDirectory(sDest, NULL);

	if (GetTotDefPcs(nSerial) > 0) // Cam1
	{
		sPath.Format(_T("%s\\%s.tif"), sDest, pView->GetMkInfo(nSerial, nMkPcsIdx));

#ifdef USE_VISION
		if (m_pVision[1])
			return m_pVision[1]->SaveMkImg(sPath);
#endif
	}
	else
		return TRUE;

	return FALSE;
}

void CManagerPunch::SetReject()
{
	CfPoint ptPnt;

	if (m_bDoMk[0])
	{
		if (!m_bTHREAD_MK[2])
		{
			m_bDoneMk[0] = FALSE;
			m_nStepMk[2] = 0;
			m_nMkPcs[2] = 0;
			m_bTHREAD_MK[2] = TRUE;
		}
	}

	if (m_bDoMk[1])
	{
		if (!m_bTHREAD_MK[3])
		{
			m_bDoneMk[1] = FALSE;
			m_nStepMk[3] = 0;
			m_nMkPcs[3] = 0;
			m_bTHREAD_MK[3] = TRUE;
		}
	}
}

void CManagerPunch::InitAuto(BOOL bInit)
{
	int a, b;

	m_nMkPcs[0] = 0;
	m_nMkPcs[1] = 0;
	m_nMkPcs[2] = 0;
	m_nMkPcs[3] = 0;
	m_nStepMk[0] = 0;
	m_nStepMk[1] = 0;
	m_nStepMk[2] = 0;
	m_nStepMk[3] = 0;
	m_bTHREAD_MK[0] = FALSE;
	m_bTHREAD_MK[1] = FALSE;
	m_bTHREAD_MK[2] = FALSE;
	m_bTHREAD_MK[3] = FALSE;
	m_bReview = FALSE;
	m_bCam = FALSE;

	for (a = 0; a < 2; a++)
	{
		for (b = 0; b < MAX_STRIP; b++)
		{
			m_nMkStrip[a][b] = 0;
			m_bRejectDone[a][b] = FALSE;
		}
	}
}

BOOL CManagerPunch::IsNoMk()
{
	BOOL bNoMk = (pDoc->WorkingInfo.System.bNoMk | m_bCam); // pDoc->WorkingInfo.LastJob.bVerify
	return bNoMk;
}

BOOL CManagerPunch::IsNoMk0()
{
	BOOL bNoMk = (pDoc->WorkingInfo.System.bNoMk | m_bCam); // pDoc->WorkingInfo.LastJob.bVerify
	return bNoMk;
}

BOOL CManagerPunch::IsNoMk1()
{
	BOOL bNoMk = (pDoc->WorkingInfo.System.bNoMk | m_bCam); // pDoc->WorkingInfo.LastJob.bVerify
	return bNoMk;
}

BOOL CManagerPunch::ChkCollision()
{
	double dMg = _tstof(pDoc->WorkingInfo.Motion.sCollisionLength) - _tstof(pDoc->WorkingInfo.Motion.sCollisionMargin);

	if (m_bTHREAD_MK[0] || m_bTHREAD_MK[1]
		|| m_bTHREAD_MK[2] || m_bTHREAD_MK[3])
	{
		if (pDoc->WorkingInfo.System.bNoMk || m_bCam)
			dMg += (_tstof(pDoc->WorkingInfo.Vision[0].sMkOffsetX) - _tstof(pDoc->WorkingInfo.Vision[1].sMkOffsetX));
	}

	if ((m_dEnc[AXIS_X0] - dMg) > m_dEnc[AXIS_X1])
		return TRUE;

	if (m_dEnc[AXIS_X0] < -1.0 || m_dEnc[AXIS_X1] < -1.0)
		return TRUE;

	return FALSE;
}

BOOL CManagerPunch::ChkCollision(int nAxisId, double dTgtPosX)
{
	double dMg = _tstof(pDoc->WorkingInfo.Motion.sCollisionLength) - _tstof(pDoc->WorkingInfo.Motion.sCollisionMargin);


	if (m_bTHREAD_MK[0] || m_bTHREAD_MK[1]
		|| m_bTHREAD_MK[2] || m_bTHREAD_MK[3])
	{
		if (pDoc->WorkingInfo.System.bNoMk || m_bCam)
			dMg += (_tstof(pDoc->WorkingInfo.Vision[0].sMkOffsetX) - _tstof(pDoc->WorkingInfo.Vision[1].sMkOffsetX));
	}

	if (nAxisId == AXIS_X0)
	{
		if (dTgtPosX > m_dEnc[AXIS_X1] + dMg)
			return TRUE;
		if (m_bDoMk[1] && !m_bDoneMk[1])
		{
			if (!m_bDoneMk[0] && m_bTHREAD_MK[0])
			{
				if (m_dTarget[AXIS_X1] > 0.0)
				{
					if (dTgtPosX > m_dTarget[AXIS_X1] + dMg && m_dTarget[AXIS_X1] > 0.0)
						return TRUE;
					else if (m_dNextTarget[AXIS_X1] > 0.0)
					{
						if (dTgtPosX > m_dNextTarget[AXIS_X1] + dMg)
							return TRUE;
					}
				}
			}
		}
	}
	else if (nAxisId == AXIS_X1)
	{
		if (dTgtPosX < m_dEnc[AXIS_X0] - dMg)
			return TRUE;
	}

	return FALSE;
}

BOOL CManagerPunch::ChkCollision(int nAxisId, double dTgtPosX, double dTgtNextPosX)
{
	double dMg = _tstof(pDoc->WorkingInfo.Motion.sCollisionLength) - _tstof(pDoc->WorkingInfo.Motion.sCollisionMargin);

	if (m_bTHREAD_MK[0] || m_bTHREAD_MK[1]
		|| m_bTHREAD_MK[2] || m_bTHREAD_MK[3])
	{
		if (pDoc->WorkingInfo.System.bNoMk || m_bCam)
			dMg += (_tstof(pDoc->WorkingInfo.Vision[0].sMkOffsetX) - _tstof(pDoc->WorkingInfo.Vision[1].sMkOffsetX));
	}

	if (nAxisId == AXIS_X0)
	{
		if (dTgtPosX > m_dEnc[AXIS_X1] + dMg)
			return TRUE;
		if (m_bDoMk[1] && !m_bDoneMk[1])
		{
			if (!m_bDoneMk[0] && m_bTHREAD_MK[0])
			{
				if (dTgtPosX > m_dTarget[AXIS_X1] + dMg && m_dTarget[AXIS_X1] > 0.0)
					return TRUE;
				else if (dTgtPosX > m_dNextTarget[AXIS_X1] + dMg && m_dNextTarget[AXIS_X1] > 0.0)
					return TRUE;
			}
		}
	}
	else if (nAxisId == AXIS_X1)
	{
		if (dTgtPosX < m_dEnc[AXIS_X0] - dMg)
			return TRUE;
		if (m_bDoMk[0])
		{
			if (!m_bDoneMk[1] && m_bTHREAD_MK[1])
			{
				if (!IsMotionDone(MS_X0))
				{
					if (dTgtPosX < m_dTarget[AXIS_X0] - dMg && m_dTarget[AXIS_X0] > 0.0)
						return TRUE;
					else if (dTgtPosX < m_dNextTarget[AXIS_X0] - dMg && m_dNextTarget[AXIS_X0] > 0.0)
						return TRUE;
				}
			}
		}
	}

	return FALSE;
}

BOOL CManagerPunch::IsReview()
{
	return (pDoc->WorkingInfo.LastJob.bReview);
}

BOOL CManagerPunch::IsReview0()
{
	return (pDoc->WorkingInfo.LastJob.bReview);
}

BOOL CManagerPunch::IsReview1()
{
	return (pDoc->WorkingInfo.LastJob.bReview);
}

BOOL CManagerPunch::IsVs()
{
	if (!pView->m_mgrProcedure->m_bChkLastProcVs)
	{
		BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
		if (bDualTest)
		{
			if (pView->m_mgrProcedure->GetAoiDnVsStatus())
				return TRUE;
			else if (pView->m_mgrProcedure->GetAoiUpVsStatus())
				return TRUE;
		}
		else
		{
			if (pView->m_mgrProcedure->GetAoiUpVsStatus())
				return TRUE;
		}
	}

	return FALSE;
}

BOOL CManagerPunch::IsVsUp()
{
	return pView->m_mgrProcedure->GetAoiUpVsStatus();
}

BOOL CManagerPunch::IsVsDn()
{
	return pView->m_mgrProcedure->GetAoiDnVsStatus();
}

int CManagerPunch::WaitRtnVal(int nThreadIdx)
{
	return pView->WaitRtnVal(nThreadIdx);
}

BOOL CManagerPunch::IsAuto()
{
	if (pDoc->Status.bAuto)
		return TRUE;
	return FALSE;
}

//LRESULT CManagerPunch::wmClientReceived(WPARAM wParam, LPARAM lParam)
//{
//	if (!m_pEngrave)
//		return (LRESULT)0;
//
//	int nAcceptId = (int)wParam;
//	SOCKET_DATA sSockData;
//	SOCKET_DATA *pSocketData = (SOCKET_DATA*)lParam;
//	SOCKET_DATA rSockData = *pSocketData;
//	int nCmdCode = rSockData.nCmdCode;
//	int nMsgId = rSockData.nMsgID;
//
//	switch (nCmdCode)
//	{
//	case _GetSig:
//		break;
//	case _GetData:
//		break;
//	case _SetSig:
//		if (m_pEngrave && m_pEngrave->IsConnected())
//			m_pEngrave->GetSysSignal(rSockData);
//
//		pView->m_bSetSig = TRUE;
//		break;
//	case _SetData:
//		if (m_pEngrave && m_pEngrave->IsConnected())
//			m_pEngrave->GetSysData(rSockData);
//
//		pView->m_bSetData = TRUE;
//		break;
//	default:
//		break;
//	}
//
//
//	return (LRESULT)1;
//}

LRESULT CManagerPunch::wmClientReceivedSr(WPARAM wParam, LPARAM lParam)
{
	int nCmd = (int)wParam;
	CString* sReceived = (CString*)lParam;

	switch (nCmd)
	{
	case SrTriggerInputOn:
		pView->Get2dCode(m_sGet2dCodeLot, m_nGet2dCodeSerial);
		break;
	default:
		break;
	}

	return (LRESULT)1;
}

void CManagerPunch::UpdateRst()
{
	pView->UpdateRst();
}

void CManagerPunch::ChkEmg()
{
	if (pDoc->Status.bEmgAoi[EMG_F_AOI_UP] && !pDoc->Status.bEmgAoiF[EMG_F_AOI_UP])
	{
		pDoc->Status.bEmgAoiF[EMG_F_AOI_UP] = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Stop();
		DispMain(_T("�� ��"), RGB_RED);
		MsgBox(_T("������� - �˻�� �� ���� ����ġ"));
		//TowerLamp(RGB_RED, TRUE);
		Buzzer(TRUE, 0);
	}
	else if (!pDoc->Status.bEmgAoi[EMG_F_AOI_UP] && pDoc->Status.bEmgAoiF[EMG_F_AOI_UP])
	{
		pDoc->Status.bEmgAoiF[EMG_F_AOI_UP] = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}

	if (pDoc->Status.bEmgAoi[EMG_B_AOI_UP] && !pDoc->Status.bEmgAoiF[EMG_B_AOI_UP])
	{
		pDoc->Status.bEmgAoiF[EMG_B_AOI_UP] = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Stop();
		DispMain(_T("�� ��"), RGB_RED);
		MsgBox(_T("������� - �˻�� �� �ĸ� ����ġ"));
		//TowerLamp(RGB_RED, TRUE);
		Buzzer(TRUE, 0);
	}
	else if (!pDoc->Status.bEmgAoi[EMG_B_AOI_UP] && pDoc->Status.bEmgAoiF[EMG_B_AOI_UP])
	{
		pDoc->Status.bEmgAoiF[EMG_B_AOI_UP] = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}

	if (pDoc->Status.bEmgAoi[EMG_F_AOI_DN] && !pDoc->Status.bEmgAoiF[EMG_F_AOI_DN])
	{
		pDoc->Status.bEmgAoiF[EMG_F_AOI_DN] = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Stop();
		DispMain(_T("�� ��"), RGB_RED);
		MsgBox(_T("������� - �˻�� �� ���� ����ġ"));
		//TowerLamp(RGB_RED, TRUE);
		Buzzer(TRUE, 0);
	}
	else if (!pDoc->Status.bEmgAoi[EMG_F_AOI_DN] && pDoc->Status.bEmgAoiF[EMG_F_AOI_DN])
	{
		pDoc->Status.bEmgAoiF[EMG_F_AOI_DN] = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}

	if (pDoc->Status.bEmgAoi[EMG_B_AOI_DN] && !pDoc->Status.bEmgAoiF[EMG_B_AOI_DN])
	{
		pDoc->Status.bEmgAoiF[EMG_B_AOI_DN] = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Stop();
		DispMain(_T("�� ��"), RGB_RED);
		MsgBox(_T("������� - �˻�� �� �ĸ� ����ġ"));
		//TowerLamp(RGB_RED, TRUE);
		Buzzer(TRUE, 0);
	}
	else if (!pDoc->Status.bEmgAoi[EMG_B_AOI_DN] && pDoc->Status.bEmgAoiF[EMG_B_AOI_DN])
	{
		pDoc->Status.bEmgAoiF[EMG_B_AOI_DN] = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}

	if (pDoc->Status.bEmgMk[EMG_M_MK] && !pDoc->Status.bEmgMkF[EMG_M_MK])
	{
		pDoc->Status.bEmgMkF[EMG_M_MK] = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Stop();
		DispMain(_T("�� ��"), RGB_RED);
		MsgBox(_T("������� - ��ŷ�� ���� ����ġ"));
		//TowerLamp(RGB_RED, TRUE);
		Buzzer(TRUE, 0);
	}
	else if (!pDoc->Status.bEmgMk[EMG_M_MK] && pDoc->Status.bEmgMkF[EMG_M_MK])
	{
		pDoc->Status.bEmgMkF[EMG_M_MK] = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}

	if (pDoc->Status.bEmgMk[EMG_B_MK] && !pDoc->Status.bEmgMkF[EMG_B_MK])
	{
		pDoc->Status.bEmgMkF[EMG_B_MK] = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Stop();
		DispMain(_T("�� ��"), RGB_RED);
		MsgBox(_T("������� - ��ŷ�� ����ġ"));
		//TowerLamp(RGB_RED, TRUE);
		Buzzer(TRUE, 0);
	}
	else if (!pDoc->Status.bEmgMk[EMG_B_MK] && pDoc->Status.bEmgMkF[EMG_B_MK])
	{
		pDoc->Status.bEmgMkF[EMG_B_MK] = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}

	if (pDoc->Status.bEmgUc && !pDoc->Status.bEmgUcF)
	{
		pDoc->Status.bEmgUcF = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Stop();
		DispMain(_T("�� ��"), RGB_RED);
		MsgBox(_T("������� - �����Ϸ��� ����ġ"));
		//TowerLamp(RGB_RED, TRUE);
		Buzzer(TRUE, 0);
	}
	else if (!pDoc->Status.bEmgUc && pDoc->Status.bEmgUcF)
	{
		pDoc->Status.bEmgUcF = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}

	if (pDoc->Status.bEmgRc && !pDoc->Status.bEmgRcF)
	{
		pDoc->Status.bEmgRcF = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Stop();
		DispMain(_T("�� ��"), RGB_RED);
		MsgBox(_T("������� - �����Ϸ��� ����ġ"));
		//TowerLamp(RGB_RED, TRUE);
		Buzzer(TRUE, 0);
	}
	else if (!pDoc->Status.bEmgRc && pDoc->Status.bEmgRcF)
	{
		pDoc->Status.bEmgRcF = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}

	if (pDoc->Status.bEmgEngv[0] && !pDoc->Status.bEmgEngvF[0])
	{
		pDoc->Status.bEmgEngvF[0] = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Stop();
		DispMain(_T("�� ��"), RGB_RED);
		MsgBox(_T("������� - ���κ� �����"));
		//TowerLamp(RGB_RED, TRUE);
		Buzzer(TRUE, 0);
	}
	else if (!pDoc->Status.bEmgEngv[0] && pDoc->Status.bEmgEngvF[0])
	{
		pDoc->Status.bEmgEngvF[0] = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}

	if (pDoc->Status.bEmgEngv[1] && !pDoc->Status.bEmgEngvF[1])
	{
		pDoc->Status.bEmgEngvF[1] = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Stop();
		DispMain(_T("�� ��"), RGB_RED);
		MsgBox(_T("������� - ���κ� ����ġ"));
		//TowerLamp(RGB_RED, TRUE);
		Buzzer(TRUE, 0);
	}
	else if (!pDoc->Status.bEmgEngv[1] && pDoc->Status.bEmgEngvF[1])
	{
		pDoc->Status.bEmgEngvF[1] = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}
}

void CManagerPunch::StopFromThread()
{
	m_bStopFromThread = TRUE;
}

//BOOL CManagerPunch::pView->Get2dCode(CString &sLot, int &nSerial)
//{
//	if (!m_pSr1000w)
//		return FALSE;
//
//	CString sData;
//	if (m_pSr1000w->pView->Get2dCode(sData))
//	{
//		int nPos = sData.ReverseFind('-');
//		if (nPos != -1)
//		{
//			pDoc->m_sOrderNum = sData.Left(nPos);
//			pDoc->m_sShotNum = sData.Right(sData.GetLength() - nPos - 1);
//			pDoc->m_nShotNum = _tstoi(pDoc->m_sShotNum);
//			sLot = pDoc->m_sOrderNum;
//			nSerial = pDoc->m_nShotNum;
//		}
//		else
//		{
//			pView->MsgBox(sData);
//		}
//
//		return TRUE;
//	}
//
//	return FALSE;
//}

BOOL CManagerPunch::CheckConectionSr1000w()
{
#ifdef USE_SR1000W
	if (pDoc->GetTestMode() == MODE_INNER || pDoc->GetTestMode() == MODE_OUTER)
	{
		if (m_pSr1000w && m_pSr1000w->IsConnected())
		{
			m_sGet2dCodeLot = _T("");
			m_nGet2dCodeSerial = 0;
			//Set2dRead(TRUE);
		}
		else
		{
			Stop();
			pView->ClrDispMsg();
			sMsg.Format(_T("2D �����Ⱑ ������ ���� �ʾҽ��ϴ�."));
			AfxMessageBox(sMsg);
			m_nMkStAuto = MK_ST;
			return FALSE;
		}
	}
#endif
	return TRUE;
}


void CManagerPunch::SwAoiRelation()
{
	pView->SwAoiRelation();
}

void CManagerPunch::SwAoiTblBlw()
{
	pView->SwAoiTblBlw();
}

void CManagerPunch::SwAoiFdVac()
{
	pView->SwAoiFdVac();
}

void CManagerPunch::SwAoiTqVac()
{
	pView->SwAoiTqVac();
}

void CManagerPunch::ResetPriority()
{
	m_bPriority[0] = FALSE;
	m_bPriority[1] = FALSE;
	m_bPriority[2] = FALSE;
	m_bPriority[3] = FALSE;
}

void CManagerPunch::SetPriority()
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

void CManagerPunch::BuzzerFromThread(BOOL bOn, int nCh)
{
	m_bBuzzerFromThread = TRUE;
}

int CManagerPunch::GetTotDefPcs(int nSerial)
{
	return pView->GetTotDefPcs(nSerial);
}

void CManagerPunch::DoAtuoGetEngStSignal()
{
#ifdef USE_MPE
	if ((pDoc->m_pMpeSignal[0] & (0x01 << 3) || pView->m_mgrProcedure->m_bEngStSw) && !pDoc->BtnStatus.EngAuto.MkStF)// 2D(GUI) ���� ���� Start��ȣ(PLC On->PC Off)
	{
		pDoc->BtnStatus.EngAuto.MkStF = TRUE;
		pView->m_mgrProcedure->m_bEngStSw = FALSE;

		pDoc->BtnStatus.EngAuto.IsMkSt = FALSE;
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqMkSt, TRUE);
	}
	else if (pDoc->BtnStatus.EngAuto.IsMkSt && pDoc->BtnStatus.EngAuto.MkStF)
	{
		pDoc->BtnStatus.EngAuto.MkStF = FALSE;
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqMkSt, FALSE);
		//if (m_pMpe)
		{
			MpeWrite(_T("MB440103"), 0);			// 2D(GUI) ���� ���� Start��ȣ(PLC On->PC Off)

													//if (pDoc->m_pMpeSignal[0] & (0x01 << 2))	// ���κ� Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)
													//	MpeWrite(_T("MB440102"), 0);		// ���κ� Feeding�Ϸ�
		}
	}

	if (pDoc->m_pMpeSignal[0] & (0x01 << 2) && !pDoc->BtnStatus.EngAuto.FdDoneF)	// ���κ� Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)
	{
		pDoc->BtnStatus.EngAuto.FdDoneF = TRUE;

		pDoc->BtnStatus.EngAuto.IsFdDone = FALSE;
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqFdDone, TRUE);
	}
	else if (pDoc->BtnStatus.EngAuto.IsFdDone && pDoc->BtnStatus.EngAuto.FdDoneF)
	{
		pDoc->BtnStatus.EngAuto.FdDoneF = FALSE;
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqFdDone, FALSE);
		//if (m_pMpe)
		{
			MpeWrite(_T("MB440102"), 0);		// ���κ� Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)
		}
	}

#endif
}

void CManagerPunch::DoAtuoGet2dReadStSignal()
{
	if ((pDoc->m_pMpeSignal[0] & (0x01 << 5) || pView->m_mgrProcedure->m_bEng2dStSw) && !pDoc->BtnStatus.EngAuto.Read2dStF)// ���κ� 2D ���� ���۽�ȣ(PLC On->PC Off)
	{
		pDoc->BtnStatus.EngAuto.Read2dStF = TRUE;
		pView->m_mgrProcedure->m_bEng2dStSw = FALSE;

		pDoc->BtnStatus.EngAuto.IsRead2dSt = FALSE;
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeq2dReadSt, TRUE);
	}
	else if (pDoc->BtnStatus.EngAuto.IsRead2dSt && pDoc->BtnStatus.EngAuto.Read2dStF)
	{
		pDoc->BtnStatus.EngAuto.Read2dStF = FALSE;
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeq2dReadSt, FALSE);
#ifdef USE_MPE
		//if (m_pMpe)
		{
			MpeWrite(_T("MB440105"), 0);			// ���κ� 2D ���� ���۽�ȣ(PLC On->PC Off)
													//if (pDoc->m_pMpeSignal[0] & (0x01 << 2))	// ���κ� Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)
													//	MpeWrite(_T("MB440102"), 0);		// ���κ� Feeding�Ϸ�
		}
#endif
	}
}
