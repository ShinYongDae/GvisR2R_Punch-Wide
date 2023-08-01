
// GvisR2R_PunchView.cpp : CGvisR2R_PunchView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "GvisR2R_Punch.h"
#endif

#include "GvisR2R_PunchDoc.h"
#include "GvisR2R_PunchView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#include "MainFrm.h"

#include "Dialog/DlgUtil04.h"
#include "Dialog/DlgUtil07.h"
#include "Dialog/DlgMyPassword.h"
#include "Dialog/DlgProgress.h"
#include "Dialog/DlgMyMsgSub00.h"
#include "Dialog/DlgMyMsgSub01.h"
#include "Dialog/DlgMyMsgSub02.h"

//#include "Device/SR1000WDEF.h"
#include "Process/DataFile.h"

extern CMainFrame* pFrm;
extern CGvisR2R_PunchDoc* pDoc;
CGvisR2R_PunchView* pView;


// CGvisR2R_PunchView

IMPLEMENT_DYNCREATE(CGvisR2R_PunchView, CFormView)

BEGIN_MESSAGE_MAP(CGvisR2R_PunchView, CFormView)
	ON_WM_TIMER()
	ON_MESSAGE(WM_DLG_INFO, OnDlgInfo)
	ON_MESSAGE(WM_BUF_THREAD_DONE, OnBufThreadDone)
	ON_MESSAGE(WM_MYMSG_EXIT, OnMyMsgExit)
	//ON_MESSAGE(WM_CLIENT_RECEIVED_SR, wmClientReceivedSr)
	//ON_MESSAGE(WM_CLIENT_RECEIVED, wmClientReceived)
END_MESSAGE_MAP()

// CGvisR2R_PunchView ����/�Ҹ�

CGvisR2R_PunchView::CGvisR2R_PunchView()
	: CFormView(IDD_GVISR2R_PUNCH_FORM)
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	int nCam, nPoint;

	pView = this;
	m_mgrReelmap = NULL;
	m_mgrProcedure = NULL;
	m_mgrPunch = NULL;

	m_bTIM_INIT_VIEW = FALSE;
	m_bDispMsg = FALSE;
	m_bWaitClrDispMsg = FALSE;
	for (int i = 0; i < 10; i++)
		m_sDispMsg[i] = _T("");
	m_bTimBuzzerWarn = FALSE;
	m_bBtnWinker[0] = FALSE; // Ready
	m_bBtnWinker[1] = FALSE; // Reset
	m_bBtnWinker[2] = FALSE; // Run
	m_bBtnWinker[3] = FALSE; // Stop
	//m_bSwStopNow = FALSE;
	//m_bSwRun = FALSE; m_bSwRunF = FALSE;
	m_bSwStop = FALSE; m_bSwStopF = FALSE;
	m_bSwReset = FALSE; m_bSwResetF = FALSE;
	m_bSwReady = FALSE; m_bSwReadyF = FALSE;
	m_bTIM_SAFTY_STOP = FALSE;
	//m_bCycleStop = FALSE;
	//m_bStopFromThread = FALSE;
	//m_bBuzzerFromThread = FALSE;
	m_nWatiDispMain = 0;
	m_bDispMain = FALSE;
	m_sDispMain = _T("");
	m_dAoiUpFdOffsetX = 0.0;
	m_dAoiUpFdOffsetY = 0.0;
	m_dAoiDnFdOffsetX = 0.0;
	m_dAoiDnFdOffsetY = 0.0;
	m_dEngFdOffsetX = 0.0;
	m_dEngFdOffsetY = 0.0;
	for (nCam = 0; nCam < 2; nCam++)
	{
		for (nPoint = 0; nPoint < 4; nPoint++)
		{
			m_dMkFdOffsetX[nCam][nPoint] = 0.0;
			m_dMkFdOffsetY[nCam][nPoint] = 0.0;
		}
	}

	//m_pMpe = NULL;
	m_pDlgMyMsg = NULL;
	m_pDlgMsgBox = NULL;

	m_bDrawGL = TRUE;

	m_pDlgInfo = NULL;
	m_pDlgFrameHigh = NULL;
	m_pDlgMenu01 = NULL;
	m_pDlgMenu02 = NULL;
	m_pDlgMenu03 = NULL;
	m_pDlgMenu04 = NULL;
	m_pDlgMenu05 = NULL;
	m_pDlgMenu06 = NULL;
	//m_pDlgUtil01 = NULL;
	//m_pDlgUtil02 = NULL;
	//m_pDlgUtil03 = NULL;

	// H/W Device �ʱ�ȭ.....
	//m_pMpe = NULL;
	//m_mgrPunch->m_pMotion = NULL;
	//m_mgrPunch->m_pVoiceCoil[0] = NULL;
	//m_mgrPunch->m_pVoiceCoil[1] = NULL;
	//m_pLight = NULL;
//#ifdef USE_FLUCK
//	m_pFluck = NULL;
//#endif

//#ifdef USE_VISION
//	m_mgrPunch->m_pVision[0] = NULL;
//	m_mgrPunch->m_pVision[1] = NULL;
//	m_mgrPunch->m_pVisionInner[0] = NULL;
//	m_mgrPunch->m_pVisionInner[1] = NULL;
//#endif

	m_bTIM_MPE_IO = FALSE;
	m_bTIM_DISP_STATUS = FALSE;
	
	// client for SR-1000W
	//m_mgrPunch->m_pSr1000w = NULL;

	// client for engrave
	//m_mgrPunch->m_pEngrave = NULL;
	//m_pDts = NULL;

	m_bDestroyedView = FALSE;
	m_bLoadMstInfo = FALSE;
	m_bLoadMstInfoF = FALSE;

	//m_sGet2dCodeLot = _T("");
	//m_nGet2dCodeSerial = 0;

	m_bSetSig = FALSE;
	m_bSetSigF = FALSE;
	m_bSetData = FALSE;
	m_bSetDataF = FALSE;

	m_bTIM_START_UPDATE = FALSE;
	m_bEscape = FALSE;
}

CGvisR2R_PunchView::~CGvisR2R_PunchView()
{
	m_bTIM_MPE_IO = FALSE;
	m_bTIM_DISP_STATUS = FALSE;
	m_bTIM_INIT_VIEW = FALSE;

#ifdef USE_VISION
	if (m_mgrPunch->m_pVisionInner[1])
	{
		delete m_mgrPunch->m_pVisionInner[1];
		m_mgrPunch->m_pVisionInner[1] = NULL;
	}

	if (m_mgrPunch->m_pVisionInner[0])
	{
		delete m_mgrPunch->m_pVisionInner[0];
		m_mgrPunch->m_pVisionInner[0] = NULL;
	}
	if (m_mgrPunch->m_pVision[1])
	{
		delete m_mgrPunch->m_pVision[1];
		m_mgrPunch->m_pVision[1] = NULL;
	}

	if (m_mgrPunch->m_pVision[0])
	{
		delete m_mgrPunch->m_pVision[0];
		m_mgrPunch->m_pVision[0] = NULL;
	}
#endif


	//InitIoWrite();
	//SetMainMc(FALSE);

	// H/W Device �Ҹ�.....
	//HwKill();

	CloseMyMsg();

	if (m_ArrayMyMsgBox.GetSize() > 0)
	{
		m_ArrayMyMsgBox.RemoveAll();
	}

	if (m_mgrProcedure)
	{
		delete m_mgrProcedure;
		m_mgrProcedure = NULL;
	}

	if (m_mgrReelmap)
	{
		delete m_mgrReelmap;
		m_mgrReelmap = NULL;
	}

	if (m_mgrPunch)
	{
		delete m_mgrPunch; // H/W Device �Ҹ�.....
		m_mgrPunch = NULL;
	}

}

void CGvisR2R_PunchView::DestroyView()
{
	CString sData;

	if (!m_bDestroyedView)
	{
		m_bDestroyedView = TRUE;

		sData.Format(_T("%d"), m_mgrProcedure->m_nMkStAuto);
		::WritePrivateProfileString(_T("Last Job"), _T("MkStAuto"), sData, PATH_WORKING_INFO);

		//ThreadKill();
		Sleep(30);

		DelAllDlg();
		Sleep(100);

		Buzzer(FALSE, 0);
		Buzzer(FALSE, 1);
	}
}

void CGvisR2R_PunchView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BOOL CGvisR2R_PunchView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CFormView::PreCreateWindow(cs);
}

void CGvisR2R_PunchView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	InitMyMsg();

	pDoc->LoadWorkingInfo();
	pDoc->LoadIoInfo();
	pDoc->LoadSignalInfo();
	pDoc->LoadDataInfo();
	if (!LoadMySpec())
		LoadMySpec();

#ifdef USE_CAM_MASTER
	CFileFind finder;
	CString sDir, sMsg;
	sDir = pDoc->WorkingInfo.System.sPathCamSpecDir;
	sDir.Delete(sDir.GetLength() - 1, 1);
	sDir.ReleaseBuffer();

	if(!pDoc->DirectoryExists(sDir))
	{
		sMsg.Format(_T("ķ�����Ϳ� ���������� �����ϴ�. : \n 1.SpecFolder : %s"), sDir);
		pView->ClrDispMsg();
		AfxMessageBox(sMsg, MB_ICONSTOP | MB_OK);
		ExitProgram();
		return;
	}
#endif

	InitMgrProcedure();

	if (!m_bTIM_INIT_VIEW)
	{
		m_nStepInitView = 0;
		m_bTIM_INIT_VIEW = TRUE;
		SetTimer(TIM_INIT_VIEW, 300, NULL);
	}

	pDoc->SetMonDispMain(_T(""));
}

// CGvisR2R_PunchView ����

#ifdef _DEBUG
void CGvisR2R_PunchView::AssertValid() const
{
	CFormView::AssertValid();
}

BOOL CGvisR2R_PunchView::CheckConectionSr1000w()
{
	return m_mgrPunch->CheckConectionSr1000w();
}

void CGvisR2R_PunchView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CGvisR2R_PunchDoc* CGvisR2R_PunchView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGvisR2R_PunchDoc)));
	return (CGvisR2R_PunchDoc*)m_pDocument;
}
#endif //_DEBUG


// CGvisR2R_PunchView �޽��� ó����

void CGvisR2R_PunchView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	int nInc = 0, nSrl = 0;
	CString str, sMsg, sPath, sLot, sLayerUp, sLayerDn;
	BOOL bDualTestInner;
	BOOL bExist = FALSE; 
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	BOOL bRtn = FALSE;

	if (nIDEvent == TIM_INIT_VIEW)
	{
		KillTimer(TIM_INIT_VIEW);

		switch (m_nStepInitView)
		{
		case 0:
			m_nStepInitView++;
			DoDispMsg(_T("���α׷��� �ʱ�ȭ�մϴ�."), _T("�˸�"), RGB_GREEN, DELAY_TIME_MSG);
			InitMgrPunch(); //HwInit();	
			InitMgrReelmap();
			//DtsInit();
			// H/W Device �ʱ�ȭ.....
			break;
		case 1:
			m_nStepInitView++;
			//InitIO();
			break;
		case 2:
			m_nStepInitView++;
			//InitIoWrite();
			//SetMainMc(TRUE);
			m_mgrProcedure->m_nMonAlmF = 0;
			m_mgrProcedure->m_nClrAlmF = 0;
			break;
		case 3:
			m_nStepInitView++;
			//ThreadInit();
			break;
		case 4:
			m_nStepInitView++;
			break;
		case 5:
			m_nStepInitView++;
			DispMsg(_T("ȭ�鱸���� �����մϴ�.- 1"), _T("�˸�"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_MENU_02);
			//ShowDlg(IDD_DLG_UTIL_01);
			break;
		case 6:
			m_nStepInitView++;
			//ShowDlg(IDD_DLG_UTIL_02);
			break;
		case 7:
			m_nStepInitView++;
			break;
		case 8:
			m_nStepInitView++;
			DispMsg(_T("ȭ�鱸���� �����մϴ�.-2"), _T("�˸�"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_MENU_01);

			if (bDualTest)
				m_pDlgMenu01->SelMap(ALL);
			else
				m_pDlgMenu01->SelMap(UP);
			break;
		case 9:
			m_nStepInitView++;
			DispMsg(_T("ȭ�鱸���� �����մϴ�.- 3"), _T("�˸�"), RGB_GREEN, DELAY_TIME_MSG);
			//ShowDlg(IDD_DLG_MENU_02);
			break;
		case 10:
			m_nStepInitView++;
			DispMsg(_T("ȭ�鱸���� �����մϴ�.- 4"), _T("�˸�"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_MENU_03);
			break;
		case 11:
			m_nStepInitView++;
			DispMsg(_T("ȭ�鱸���� �����մϴ�.- 5"), _T("�˸�"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_MENU_04);
			break;
		case 12:
			m_nStepInitView++;
			DispMsg(_T("ȭ�鱸���� �����մϴ�.- 6"), _T("�˸�"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_MENU_05);
			break;
		case 13:
			m_nStepInitView++;
			DispMsg(_T("ȭ�鱸���� �����մϴ�.- 7"), _T("�˸�"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_MENU_06);
			break;
		case 14:
			m_nStepInitView++;
			DispMsg(_T("ȭ�鱸���� �����մϴ�.- 8"), _T("�˸�"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_FRAME_HIGH);
			if (m_pDlgFrameHigh)
				m_pDlgFrameHigh->ChkMenu01();
			SetDualTest(pDoc->WorkingInfo.LastJob.bDualTest);

			if (pDoc->GetCurrentInfoEng())
			{
				if (m_mgrReelmap->GetItsSerialInfo(0, bDualTestInner, sLot, sLayerUp, sLayerDn, 0))
				{
					//if (pDoc->GetTestMode() == MODE_OUTER)
					if (m_mgrReelmap->m_Master[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, sLayerUp))
					{
						if (m_pDlgMenu06)
							m_pDlgMenu06->RedrawWindow();
					}
				}
			}

			m_mgrPunch->InitMotion();
			Sleep(300);
			break;
		case 15:
			m_nStepInitView++;
			m_bLoadMstInfo = TRUE;
			DispMsg(_T("H/W�� �ʱ�ȭ�մϴ�."), _T("�˸�"), RGB_GREEN, DELAY_TIME_MSG);
			m_mgrPunch->InitAct();
			m_mgrProcedure->m_bStopFeeding = TRUE;
			MpeWrite(_T("MB440115"), 1); // ��ŷ��Feeding����
			Sleep(300);
			break;
		case 16:
			if (m_mgrPunch->m_pMotion)
			{
				DispMsg(_T("Homming"), _T("Searching Home Position..."), RGB_GREEN, 2000, TRUE);
				m_mgrPunch->m_pMotion->SearchHome();
				m_nStepInitView++;
			}
			else
			{
				m_bTIM_INIT_VIEW = FALSE;
				pView->ClrDispMsg();
				AfxMessageBox(_T("Motion is failed."));
				PostMessage(WM_CLOSE);
			}
			break;
		case 17:
			if (m_mgrPunch->m_pMotion)
			{
				if (m_mgrPunch->m_pMotion->IsHomeDone())// && m_mgrPunch->m_pMotion->IsHomeDone(MS_MKFD))
				{
					m_nStepInitView++;
					Sleep(300);
				}

				sMsg.Format(_T("X0(%s) , Y0(%s)\r\nX1(%s) , Y1(%s)"), m_mgrPunch->m_pMotion->IsHomeDone(MS_X0) ? _T("Done") : _T("Doing"),
					m_mgrPunch->m_pMotion->IsHomeDone(MS_Y0) ? _T("Done") : _T("Doing"),
					m_mgrPunch->m_pMotion->IsHomeDone(MS_X1) ? _T("Done") : _T("Doing"),
					m_mgrPunch->m_pMotion->IsHomeDone(MS_Y1) ? _T("Done") : _T("Doing"));
				DispMsg(sMsg, _T("Homming"), RGB_GREEN, 2000, TRUE);
			}
			else
			{
				m_bTIM_INIT_VIEW = FALSE;
				pView->ClrDispMsg();
				AfxMessageBox(_T("Motion is failed."));
				PostMessage(WM_CLOSE);
			}
			break;
		case 18:
			m_nStepInitView++;
			break;
		case 19:
			m_nStepInitView++;
			DispMsg(_T("Completed Searching Home Pos..."), _T("Homming"), RGB_GREEN, 2000, TRUE);
			m_mgrPunch->m_pMotion->ObjectMapping();
			m_bEscape = FALSE;

			if (m_mgrPunch->m_pVoiceCoil[0])
			{
				m_mgrPunch->m_pVoiceCoil[0]->SetMarkFinalData(0);
			}

			if (m_mgrPunch->m_pVoiceCoil[1])
			{
				m_mgrPunch->m_pVoiceCoil[1]->SetMarkFinalData(1);
			}

			break;
		case 20:
			m_mgrProcedure->m_bStopFeeding = FALSE;
			MpeWrite(_T("MB440115"), 0); // ��ŷ��Feeding����
			m_nStepInitView++;
			if(m_pDlgMenu02)
				m_pDlgMenu02->SetJogSpd(_tstoi(pDoc->WorkingInfo.LastJob.sJogSpd));
			if (m_pDlgMenu03)
				m_pDlgMenu03->InitRelation();
			m_mgrPunch->m_pMotion->SetR2RConf();
			//TowerLamp(RGB_YELLOW, TRUE);

			if (!SetCollision(-1.0*_tstof(pDoc->WorkingInfo.Motion.sCollisionLength)))
			{
				DispMsg(_T("Collision"), _T("Failed to Set Collision ..."), RGB_GREEN, 2000, TRUE);
			}

			break;
		case 21:
			m_nStepInitView++;
			//InitPLC();
			m_mgrPunch->SetPlcParam();
			m_mgrPunch->GetPlcParam();
			//TcpIpInit();
			//DtsInit();
			m_bTIM_DISP_STATUS = TRUE;
			SetTimer(TIM_DISP_STATUS, 100, NULL);

			m_bTIM_START_UPDATE = TRUE;
			SetTimer(TIM_START_UPDATE, 500, NULL);
			break;
		case 22:
			if (m_bLoadMstInfo || m_bLoadMstInfoF)
				break;

			m_nStepInitView++;
			ClrDispMsg();

			bRtn = m_mgrReelmap->LoadPcrFromBuf();

			if (m_pDlgMenu01)
			{
				m_pDlgMenu01->SetStripAllMk();
				m_pDlgMenu01->RefreshRmap();
				m_pDlgMenu01->ResetLastProc();
			}

			if (!MemChk())
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Memory Error - Cam Spec Data : PCR[0] or PCR[1] or Reelmap"));
			}
			else
			{
				if (m_mgrReelmap->m_pReelMap && bRtn)
				{
					m_mgrReelmap->ReloadReelmap();
					UpdateRst();
					UpdateLotTime();
				}

				//if (pDoc->GetTestMode() == MODE_OUTER)
				if (pDoc->GetCurrentInfoEng())
				{
					if (m_mgrReelmap->GetItsSerialInfo(0, bDualTestInner, sLot, sLayerUp, sLayerDn, 3))
					{
						if (m_mgrReelmap->m_Master[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, sLayerUp))
						{
							if (m_pDlgMenu06)
							{
								m_pDlgMenu06->SetStripAllMk();
								m_pDlgMenu06->RefreshRmap();
								m_pDlgMenu06->ResetLastProc();
							}

							if (m_mgrReelmap->m_pReelMapInner)
							{
								m_mgrReelmap->ReloadReelmapInner();
								UpdateRstInner();
							}
						}
					}
				}

			}

			m_mgrPunch->MoveInitPos1();
			Sleep(30);
			m_mgrPunch->MoveInitPos0();

			SetLotLastShot();
			m_mgrPunch->StartLive();

			if (pDoc->GetTestMode() == MODE_OUTER)
			{
				if (m_pDlgMenu01)
					m_pDlgMenu01->EnableItsMode();
			}

			m_bTIM_MPE_IO = TRUE;
			SetTimer(TIM_MPE_IO, 50, NULL);

			if(m_mgrProcedure->m_bMkSt)
				LoadSerial();

			m_bTIM_INIT_VIEW = FALSE;
			break;
		}

		if (m_bTIM_INIT_VIEW)
			SetTimer(TIM_INIT_VIEW, 100, NULL);
	}

	if (nIDEvent == TIM_MPE_IO)
	{
		KillTimer(TIM_MPE_IO);
		DoIO();
		if (m_bTIM_MPE_IO)
			SetTimer(TIM_MPE_IO, 100, NULL);
	}

	if (nIDEvent == TIM_BUZZER_WARN)
	{
		KillTimer(TIM_BUZZER_WARN);
		m_nCntBz++;
		if (m_nCntBz > BUZZER_DELAY)
		{
			m_bTimBuzzerWarn = FALSE;
			Buzzer(FALSE);
		}
		if (m_bTimBuzzerWarn)
			SetTimer(TIM_BUZZER_WARN, 100, NULL);
	}

	if (nIDEvent == TIM_DISP_STATUS)
	{
		KillTimer(TIM_DISP_STATUS);

		DispStsBar();
		DoDispMain();
		//DispMyMsgBox();

		//if (m_bStopFromThread)
		//{
		//	m_bStopFromThread = FALSE;
		//	Stop();
		//}
		//if (m_bBuzzerFromThread)
		//{
		//	m_bBuzzerFromThread = FALSE;
		//	Buzzer(TRUE, 0);
		//}

		//ChkEmg();
		//ChkSaftySen();
		//ChkDoor();

		if (m_bTIM_DISP_STATUS)
			SetTimer(TIM_DISP_STATUS, 100, NULL);
	}


	if (nIDEvent == TIM_SHOW_MENU01)
	{
		KillTimer(TIM_SHOW_MENU01);
		if (m_pDlgFrameHigh)
			m_pDlgFrameHigh->ChkMenu01();
	}

	if (nIDEvent == TIM_SHOW_MENU02)
	{
		KillTimer(TIM_SHOW_MENU02);
		if (m_pDlgFrameHigh)
			m_pDlgFrameHigh->ChkMenu02();
	}

	if (nIDEvent == TIM_CHK_TEMP_STOP)
	{
		KillTimer(TIM_CHK_TEMP_STOP);
#ifdef USE_MPE
		if (!(pDoc->m_pMpeSignal[7] & (0x01 << 3)))	// �Ͻ��������(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141031
		{
			m_bTIM_CHK_TEMP_STOP = FALSE;
			m_pDlgMenu01->SetTempStop(FALSE);
		}
#endif
		if (m_bTIM_CHK_TEMP_STOP)
			SetTimer(TIM_CHK_TEMP_STOP, 500, NULL);
	}

	if (nIDEvent == TIM_SAFTY_STOP)
	{
		KillTimer(TIM_SAFTY_STOP);
		MsgBox(_T("�Ͻ����� - ��ŷ�� ���������� �����Ǿ����ϴ�."));
		m_bTIM_SAFTY_STOP = FALSE;
	}

	if (nIDEvent == TIM_START_UPDATE)
	{
		KillTimer(TIM_START_UPDATE);

		if (m_bLoadMstInfo && !m_bLoadMstInfoF)
		{
			if (!pDoc->WorkingInfo.LastJob.sModelUp.IsEmpty() && !pDoc->WorkingInfo.LastJob.sLayerUp.IsEmpty())
			{
				m_bLoadMstInfoF = TRUE;
				SetTimer(TIM_CAMMASTER_UPDATE, 500, NULL);
			}
			else
				m_bLoadMstInfo = FALSE;
		}

		if (m_bSetSig && !m_bSetSigF)
		{
			m_bSetSigF = TRUE;

			if (m_mgrPunch->m_pEngrave->m_bGetOpInfo || m_mgrPunch->m_pEngrave->m_bGetInfo)
			{
				if (m_pDlgInfo)
					m_pDlgInfo->UpdateData();

				if (m_pDlgMenu01)
					m_pDlgMenu01->UpdateData();

				m_mgrPunch->m_pEngrave->m_bGetOpInfo = FALSE;
				m_mgrPunch->m_pEngrave->m_bGetInfo = FALSE;
			}
			else
			{
				if (m_pDlgMenu03)
					m_pDlgMenu03->UpdateSignal();
			}

			m_bSetSig = FALSE;
		}
		else if (!m_bSetSig && m_bSetSigF)
		{
			m_bSetSigF = FALSE;
		}

		if (m_bSetData && !m_bSetDataF)
		{
			m_bSetDataF = TRUE;

			if (m_mgrPunch->m_pEngrave->m_bGetOpInfo || m_mgrPunch->m_pEngrave->m_bGetInfo)
			{
				if (m_pDlgInfo)
					m_pDlgInfo->UpdateData();

				if (m_pDlgMenu01)
					m_pDlgMenu01->UpdateData();

				m_mgrPunch->m_pEngrave->m_bGetOpInfo = FALSE;
				m_mgrPunch->m_pEngrave->m_bGetInfo = FALSE;
			}

			if (m_pDlgMenu02)
				m_pDlgMenu02->UpdateData();

			if (m_pDlgMenu03)
				m_pDlgMenu03->UpdateData();

			if (m_pDlgMenu04)
				m_pDlgMenu04->UpdateData();

			m_bSetData = FALSE;
		}
		else if (!m_bSetData && m_bSetDataF)
		{
			m_bSetDataF = FALSE;
		}

		if (m_bTIM_START_UPDATE)
			SetTimer(TIM_START_UPDATE, 100, NULL);
	}

	if (nIDEvent == TIM_CAMMASTER_UPDATE)
	{
		KillTimer(TIM_CAMMASTER_UPDATE);
		if (m_mgrReelmap->LoadMstInfo())
		{
			SetAlignPos();

			if (m_pDlgMenu01)
				m_pDlgMenu01->UpdateData();

			if (pDoc->GetCurrentInfoEng())
			{
				if (pDoc->GetTestMode() == MODE_OUTER)
				{
					if (m_pDlgMenu06)
					{
						m_pDlgMenu06->InitGL();
						m_pDlgMenu06->RefreshRmap();
						m_pDlgMenu06->InitCadImg();
						m_pDlgMenu06->SetPnlNum();
						m_pDlgMenu06->SetPnlDefNum();

						if (pDoc->WorkingInfo.LastJob.bDualTestInner)
							m_pDlgMenu06->SelMap(ALL);
						else
							m_pDlgMenu06->SelMap(UP);
					}
				}
			}

			if (m_pDlgMenu01)
			{
				m_pDlgMenu01->InitGL();
				m_bDrawGL = TRUE;
				m_pDlgMenu01->RefreshRmap();
				m_pDlgMenu01->InitCadImg();
				m_pDlgMenu01->SetPnlNum();
				m_pDlgMenu01->SetPnlDefNum();
			}

			if (m_pDlgMenu02)
			{
				m_pDlgMenu02->ChgModelUp(); // PinImg, AlignImg�� Display��.
				m_pDlgMenu02->InitCadImg();
			}

			if (m_pDlgMenu01)
				m_pDlgMenu01->RedrawWindow();

			int nSrl = pDoc->GetLastShotMk();
			m_mgrProcedure->SetMkFdLen();
			if (nSrl >= 0)
			{
				if (bDualTest)
					m_pDlgMenu01->SelMap(ALL);
				else
					m_pDlgMenu01->SelMap(UP);
			}
		}

		m_bLoadMstInfoF = FALSE;
		m_bLoadMstInfo = FALSE;
	}

	CFormView::OnTimer(nIDEvent);
}


void CGvisR2R_PunchView::InitMyMsg()
{
	if (m_pDlgMyMsg)
		CloseMyMsg();

	m_pDlgMyMsg = new CDlgMyMsg(this);
	m_pDlgMyMsg->Create();
}

void CGvisR2R_PunchView::CloseMyMsg()
{
	if (m_pDlgMyMsg)
	{
		delete m_pDlgMyMsg;
		m_pDlgMyMsg = NULL;
	}
}

LRESULT CGvisR2R_PunchView::OnMyMsgExit(WPARAM wPara, LPARAM lPara)
{
	Buzzer(FALSE, 0);
	ClrAlarm();
	return 0L;
}

int CGvisR2R_PunchView::MsgBox(CString sMsg, int nThreadIdx, int nType, int nTimOut, BOOL bEngave)
{
	int nRtnVal = -1; // Reply(-1) is None.

	if (bEngave)
	{
		if (m_mgrPunch->m_pEngrave)
		{
			pDoc->m_sMsgBox = sMsg;
			if (pDoc->m_sIsMsgBox != pDoc->m_sMsgBox)
			{
				if (m_mgrPunch->m_pEngrave)
					m_mgrPunch->m_pEngrave->SetMsgBox(pDoc->m_sMsgBox, nType);
			}
		}
	}

	if (m_pDlgMyMsg)
		nRtnVal = m_pDlgMyMsg->SyncMsgBox(sMsg, nThreadIdx, nType, nTimOut);

	return nRtnVal;
}

int CGvisR2R_PunchView::AsyncMsgBox(CString sMsg, int nThreadIdx, int nType, int nTimOut)
{
	int nRtn = -1;
	if (m_pDlgMyMsg)
		m_pDlgMyMsg->AsyncMsgBox(sMsg, nThreadIdx, nType, nTimOut);
	return nRtn;
}

int CGvisR2R_PunchView::WaitRtnVal(int nThreadIdx)
{
	int nRtn = -1;
	if (m_pDlgMyMsg)
		nRtn = m_pDlgMyMsg->WaitRtnVal(nThreadIdx);
	return nRtn;
}

void CGvisR2R_PunchView::UpdateLotTime()
{
	m_mgrProcedure->m_dwLotSt = (DWORD)pDoc->WorkingInfo.Lot.dwStTick;

	if (m_pDlgMenu01)
		m_pDlgMenu01->UpdateLotTime();
}

void CGvisR2R_PunchView::DispStsBar(CString sMsg, int nIdx)
{
	if (m_sDispMsg[nIdx] != sMsg)
		m_sDispMsg[nIdx] = sMsg;
	sMsg.Empty();
}

void CGvisR2R_PunchView::DispStsBar()
{
	DispStsMainMsg(); // 0
	//DispStsMainMsg(6); // 6
	//DispStsMainMsg(5); // 5
	//DispThreadTick(); // 5, 6
	DispTime(); // 7
	m_mgrProcedure->ChkShare(); // 2, 4
	if (!m_mgrProcedure->m_bShift2Mk && !m_mgrProcedure->m_bTHREAD_SHIFT2MK 
		&& (m_mgrProcedure->m_nMkStAuto < MK_ST + (Mk2PtIdx::DoneMk) 
			|| m_mgrProcedure->m_nMkStAuto > MK_ST + (Mk2PtIdx::DoneMk) + 5 
			|| m_bTIM_INIT_VIEW))
	{
		m_mgrProcedure->ChkBuf(); // 1, 3
		if (m_bTIM_INIT_VIEW)
			SetListBuf();
	}
}

BOOL CGvisR2R_PunchView::MemChk() // ����ȭ�鿡 ǥ���� �����͸� ���� ����ü �Ҵ��� Ȯ����.
{
	if (!m_mgrReelmap->m_pPcr[0] || !m_mgrReelmap->m_pPcr[1])// || !m_mgrReelmap->m_pReelMap)
		return FALSE;
	return TRUE;
}

//void CGvisR2R_PunchView::SetMainMc(BOOL bOn)
//{
//	if (bOn)
//	{
//		if (m_pMpe)
//			MpeWrite(_T("MB440159"), 1); // ��ŷ�� Door Interlock ON
//			//MpeWrite(_T("MB440159"), 1); // ��ŷ�� MC ON
//	}
//	else
//	{
//		if (m_pMpe)
//			MpeWrite(_T("MB440158"), 0); // ��ŷ�� Door Interlock OFF
//			//MpeWrite(_T("MB440159"), 0); // ��ŷ�� MC OFF
//	}
//}

void CGvisR2R_PunchView::ExitProgram()
{
	long lParam = 0;
	long lData = 1;
	lParam = lParam | lData;
	lData = 0x00 << 16;
	lParam = lParam | lData;
	lData = 1 << 29;
	lParam = lParam | lData;
	AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_APP_EXIT, 0);
}

//void CGvisR2R_PunchView::InitMotion()
//{
//	int nAxis;
//	if (m_mgrPunch->m_pMotion)
//	{
//		for (nAxis = 0; nAxis<m_mgrPunch->m_pMotion->m_ParamCtrl.nTotAxis; nAxis++)
//		{
//			m_mgrPunch->m_pMotion->AmpReset(nAxis);
//			Sleep(30);
//		}
//	}
//}

//void CGvisR2R_PunchView::InitIO()
//{
//	int i, k;
//
//#ifdef USE_MPE
//	pDoc->m_nMpeIo = pDoc->MkIo.MpeIo.nMaxSeg;
//	pDoc->m_nMpeIb = pDoc->MkIo.MpeIo.nMaxSeg;
//
//	if (!pDoc->m_pMpeIb)
//	{
//		if (pDoc->m_nMpeIb > 0)
//		{
//			pDoc->m_pMpeIb = new unsigned short[pDoc->m_nMpeIb];
//			for (i = 0; i < pDoc->m_nMpeIb; i++)
//				pDoc->m_pMpeIb[i] = 0;
//		}
//	}
//
//	if (!pDoc->m_pMpeIF)
//	{
//		if (pDoc->m_nMpeIb>0)
//		{
//			pDoc->m_pMpeIF = new unsigned short[pDoc->m_nMpeIb];
//			for (i = 0; i<pDoc->m_nMpeIb; i++)
//				pDoc->m_pMpeIF[i] = 0;
//		}
//	}
//
//	if (!pDoc->m_pMpeIo)
//	{
//		if (pDoc->m_nMpeIo>0)
//		{
//			pDoc->m_pMpeIo = new unsigned short[pDoc->m_nMpeIo];
//			for (i = 0; i<pDoc->m_nMpeIo; i++)
//				pDoc->m_pMpeIo[i] = 0;
//		}
//	}
//
//	if (!pDoc->m_pMpeIoF)
//	{
//		if (pDoc->m_nMpeIo>0)
//		{
//			pDoc->m_pMpeIoF = new unsigned short[pDoc->m_nMpeIo];
//			for (i = 0; i<pDoc->m_nMpeIo; i++)
//				pDoc->m_pMpeIoF[i] = 0;
//		}
//	}
//
//
//	pDoc->m_nMpeSignal = pDoc->MkIo.MpeSignal.nMaxSeg;
//	if (!pDoc->m_pMpeSignal)
//	{
//		if (pDoc->m_nMpeSignal>0)
//		{
//			pDoc->m_pMpeSignal = new unsigned short[pDoc->m_nMpeSignal];
//			for (i = 0; i<pDoc->m_nMpeSignal; i++)
//				pDoc->m_pMpeSignal[i] = 0;
//		}
//	}
//
//
//	pDoc->m_nMpeData = pDoc->MkIo.MpeData.nMaxSeg;
//	if (!pDoc->m_pMpeData)
//	{
//		if (pDoc->m_nMpeData>0)
//		{
//			pDoc->m_pMpeData = new long*[pDoc->m_nMpeData];
//			for (i = 0; i<pDoc->m_nMpeData; i++)
//			{
//				pDoc->m_pMpeData[i] = new long[16];
//				for (k = 0; k<16; k++)
//					pDoc->m_pMpeData[i][k] = 0;
//			}
//		}
//	}
//#endif
//}

//BOOL CGvisR2R_PunchView::InitAct()
//{
//#ifdef USE_XMP
//	if (!m_mgrPunch->m_pMotion)
//		return FALSE;
//#endif
//
//#ifdef USE_LIGHT
//	if (!m_pLight)
//		return FALSE;
//#endif
//
//#ifdef USE_FLUCK
//	if (!m_pFluck)
//		return FALSE;
//#endif
//
//	int nAxis;
//
//	if (m_mgrPunch->m_pMotion)
//	{
//		// Motor On
//		for (nAxis = 0; nAxis<m_mgrPunch->m_pMotion->m_ParamCtrl.nTotAxis; nAxis++)
//		{
//			m_mgrPunch->m_pMotion->ServoOnOff(nAxis, TRUE);
//			Sleep(100);
//		}
//
//		double dX[2], dY[2];
//
//		if (pDoc->m_pSpecLocal && IsPinData())
//		{
//			dX[0] = pDoc->m_pSpecLocal->m_dPinPosX[0];
//			dY[0] = pDoc->m_pSpecLocal->m_dPinPosY[0];
//			dX[1] = pDoc->m_pSpecLocal->m_dPinPosX[1];
//			dY[1] = pDoc->m_pSpecLocal->m_dPinPosY[1];
//		}
//		else
//		{
//			dX[0] = _tstof(pDoc->WorkingInfo.Motion.sPinPosX[0]);
//			dY[0] = _tstof(pDoc->WorkingInfo.Motion.sPinPosY[0]);
//			dX[1] = _tstof(pDoc->WorkingInfo.Motion.sPinPosX[1]);
//			dY[1] = _tstof(pDoc->WorkingInfo.Motion.sPinPosY[1]);
//		}
//		m_mgrPunch->m_pMotion->SetPinPos(0, dX[0], dY[0]);
//		m_mgrPunch->m_pMotion->SetPinPos(1, dX[1], dY[1]);
//		m_mgrPunch->m_pMotion->m_dStBufPos = _tstof(pDoc->WorkingInfo.Motion.sStBufPos);
//
//
//		CfPoint ptPnt0(dX[0], dY[0]);
//		if (m_mgrReelmap->m_Master[0].m_pPcsRgn)
//			m_mgrReelmap->m_Master[0].m_pPcsRgn->SetPinPos(0, ptPnt0);
//
//		CfPoint ptPnt1(dX[1], dY[1]);
//		if (m_mgrReelmap->m_Master[0].m_pPcsRgn)
//			m_mgrReelmap->m_Master[0].m_pPcsRgn->SetPinPos(1, ptPnt1);
//
//		if (pDoc->m_pSpecLocal)// && IsMkOffsetData())
//		{
//			m_mgrReelmap->SetMkPnt(CAM_BOTH);
//		}
//
//		double dPos = _tstof(pDoc->WorkingInfo.Motion.sStBufPos);
//		SetBufInitPos(dPos);
//		double dVel = _tstof(pDoc->WorkingInfo.Motion.sBufHomeSpd);
//		double dAcc = _tstof(pDoc->WorkingInfo.Motion.sBufHomeAcc);
//		SetBufHomeParam(dVel, dAcc);
//	}
//
//	// Light On
//	if (m_pDlgMenu02)
//	{
//		m_pDlgMenu02->SetLight(_tstoi(pDoc->WorkingInfo.Light.sVal[0]));
//		m_pDlgMenu02->SetLight2(_tstoi(pDoc->WorkingInfo.Light.sVal[1]));
//	}
//
//	// Homming
//	if (m_mgrPunch->m_pVoiceCoil[0])
//		m_mgrPunch->m_pVoiceCoil[0]->SearchHomeSmac(0);
//	if (m_mgrPunch->m_pVoiceCoil[1])
//		m_mgrPunch->m_pVoiceCoil[1]->SearchHomeSmac(1);
//
//	return TRUE;
//}

//BOOL CGvisR2R_PunchView::TcpIpInit()
//{
//#ifdef USE_SR1000W
//	if (!m_mgrPunch->m_pSr1000w)
//	{
//		m_mgrPunch->m_pSr1000w = new CSr1000w(pDoc->WorkingInfo.System.sIpClient[ID_SR1000W], pDoc->WorkingInfo.System.sIpServer[ID_SR1000W], pDoc->WorkingInfo.System.sPort[ID_SR1000W], this);
//		//m_mgrPunch->m_pSr1000w->SetHwnd(this->GetSafeHwnd());
//	}
//#endif	
//
//#ifdef USE_TCPIP
//	if (!m_mgrPunch->m_pEngrave)
//	{
//		m_mgrPunch->m_pEngrave = new CEngrave(pDoc->WorkingInfo.System.sIpClient[ID_PUNCH], pDoc->WorkingInfo.System.sIpServer[ID_ENGRAVE], pDoc->WorkingInfo.System.sPort[ID_ENGRAVE], this);
//		m_mgrPunch->m_pEngrave->SetHwnd(this->GetSafeHwnd());
//	}
//#endif
//
//	return TRUE;
//}

//void CGvisR2R_PunchView::DtsInit()
//{
//#ifdef USE_DTS
//	if (pDoc->m_bUseDts)
//	{
//		if (!m_pDts)
//		{
//			m_pDts = new CDts(this);
//		}
//	}
//#endif
//}

//BOOL CGvisR2R_PunchView::HwInit()
//{
//	if (m_pLight)
//	{
//		delete m_pLight;
//		m_pLight = NULL;
//	}
//	m_pLight = new CLight(this);
//	m_pLight->Init();
//#ifdef USE_FLUCK
//	if (pDoc->WorkingInfo.Fluck.bUse)
//	{
//		if (m_pFluck)
//		{
//			delete m_pFluck;
//			m_pFluck = NULL;
//		}
//		m_pFluck = new CFluck(this);
//		m_pFluck->Init();
//	}
//#endif
//	if (m_mgrPunch->m_pVoiceCoil[0])
//	{
//		delete m_mgrPunch->m_pVoiceCoil[0];
//		m_mgrPunch->m_pVoiceCoil[0] = NULL;
//	}
//
//	m_mgrPunch->m_pVoiceCoil[0] = new CSmac(this);
//	m_mgrPunch->m_pVoiceCoil[0]->SetCh(VOICE_COIL_FIRST_CAM);
//	m_mgrPunch->m_pVoiceCoil[0]->Init();
//
//	if (m_mgrPunch->m_pVoiceCoil[1])
//	{
//		delete m_mgrPunch->m_pVoiceCoil[1];
//		m_mgrPunch->m_pVoiceCoil[1] = NULL;
//	}
//
//	m_mgrPunch->m_pVoiceCoil[1] = new CSmac(this);
//	m_mgrPunch->m_pVoiceCoil[1]->SetCh(VOICE_COIL_SECOND_CAM);
//	m_mgrPunch->m_pVoiceCoil[1]->Init();
//
//	if (m_mgrPunch->m_pMotion)
//	{
//		delete m_mgrPunch->m_pMotion;
//		m_mgrPunch->m_pMotion = NULL;
//	}
//	m_mgrPunch->m_pMotion = new CMotion(this);
//	if (!m_mgrPunch->m_pMotion->InitBoard())
//	{
//		MsgBox(_T("XMP ���� �ʱ�ȭ ����, �ٽ� �����ϼ���.!!!"));
//		PostMessage(WM_CLOSE);
//		return FALSE;
//	}
//
//#ifdef USE_MPE
//	if (!m_pMpe)
//		m_pMpe = new CMpDevice(this);
//	if (!m_pMpe->Init(1, 1))
//	{
//		MsgBox(_T("��īƮ�θ�ũ(MC0)�� ������α׷��� ���� ��, �ٽ� �����ϼ���.!!!"));
//		PostMessage(WM_CLOSE);
//		return FALSE;
//	}
//#endif
//
//	return TRUE;
//}
//
//void CGvisR2R_PunchView::HwKill()
//{
//	if (m_mgrPunch->m_pMotion)
//	{
//		delete m_mgrPunch->m_pMotion;
//		m_mgrPunch->m_pMotion = NULL;
//	}
//
//	if (m_pMpe)
//	{
//		delete m_pMpe;
//		m_pMpe = NULL;
//	}
//
//	if (m_mgrPunch->m_pVoiceCoil[0])
//	{
//		delete m_mgrPunch->m_pVoiceCoil[0];
//		m_mgrPunch->m_pVoiceCoil[0] = NULL;
//	}
//
//	if (m_mgrPunch->m_pVoiceCoil[1])
//	{
//		delete m_mgrPunch->m_pVoiceCoil[1];
//		m_mgrPunch->m_pVoiceCoil[1] = NULL;
//	}
//
//	if (m_pLight)
//	{
//		m_pLight->Close();
//		delete m_pLight;
//		m_pLight = NULL;
//	}
//
//	if (m_mgrPunch->m_pEngrave)
//	{
//		m_mgrPunch->m_pEngrave->Close();
//		delete m_mgrPunch->m_pEngrave;
//		m_mgrPunch->m_pEngrave = NULL;
//	}
//
//	if (m_mgrPunch->m_pSr1000w)
//	{
//		m_mgrPunch->m_pSr1000w->Close();
//		delete m_mgrPunch->m_pSr1000w;
//		m_mgrPunch->m_pSr1000w = NULL;
//	}
//
//	if (m_mgrPunch->m_pDts)
//	{
//		delete m_pDts;
//		m_pDts = NULL;
//	}
//
//#ifdef USE_FLUCK
//	if (m_pFluck)
//	{
//		m_pFluck->Close();
//		delete m_pFluck;
//		m_pFluck = NULL;
//	}
//#endif
//}

void CGvisR2R_PunchView::GetDispMsg(CString &strMsg, CString &strTitle)
{
	if (m_pDlgMsgBox)
		m_pDlgMsgBox->GetDispMsg(strMsg, strTitle);
}

void CGvisR2R_PunchView::DispMsg(CString strMsg, CString strTitle, COLORREF color, DWORD dwDispTime, BOOL bOverWrite)
{
	if (!m_mgrProcedure)
		return;

	if (m_bDispMsg)
		return;

	if (m_mgrPunch->m_bAuto)
	{
		return;
	}

	m_bDispMsg = TRUE;
	DoDispMsg(strMsg, strTitle, color, dwDispTime, bOverWrite);
	m_bDispMsg = FALSE;
}

void CGvisR2R_PunchView::DoDispMsg(CString strMsg, CString strTitle, COLORREF color, DWORD dwDispTime, BOOL bOverWrite)
{
	if (dwDispTime == 0)
	{
		dwDispTime = 24 * 3600 * 1000;
	}

	if (m_pDlgMsgBox != NULL)
	{
		if (bOverWrite)
		{
			if(m_pDlgMsgBox)
				m_pDlgMsgBox->SetDispMsg(strMsg, strTitle, dwDispTime, color);
		}
		if (m_pDlgMsgBox)
			m_pDlgMsgBox->ShowWindow(SW_SHOW);
		if (m_pDlgMsgBox)
			m_pDlgMsgBox->SetFocus();
		if (m_pDlgMsgBox)
			((CButton*)m_pDlgMsgBox->GetDlgItem(IDOK))->SetCheck(TRUE);
	}
	else
	{
		m_pDlgMsgBox = new CDlgMsgBox(this, strTitle, strMsg, dwDispTime, color);
		if (m_pDlgMsgBox->GetSafeHwnd() == 0)
		{
			m_pDlgMsgBox->Create();
			m_pDlgMsgBox->ShowWindow(SW_SHOW);
			m_pDlgMsgBox->SetDispMsg(strMsg, strTitle, dwDispTime, color);
			m_pDlgMsgBox->SetFocus();
			((CButton*)m_pDlgMsgBox->GetDlgItem(IDOK))->SetCheck(TRUE);
		}
	}
}

void CGvisR2R_PunchView::ClrDispMsg()
{
	OnQuitDispMsg(NULL, NULL);
}

BOOL CGvisR2R_PunchView::WaitClrDispMsg()
{
	m_bWaitClrDispMsg = TRUE;
	MSG message;

	DWORD dwMilliseconds = 0; // 100ms sec sleep
	while (WAIT_OBJECT_0 != ::WaitForSingleObject(m_evtWaitClrDispMsg, dwMilliseconds) && m_pDlgMsgBox != NULL)
	{
		if (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&message);
			::DispatchMessage(&message);
		}
	};
	Sleep(10);
	m_bWaitClrDispMsg = FALSE;
	return TRUE;
}

LONG CGvisR2R_PunchView::OnQuitDispMsg(UINT wParam, LONG lParam)
{
	if (m_pDlgMsgBox)
	{
		if (m_pDlgMsgBox->GetSafeHwnd())
			m_pDlgMsgBox->DestroyWindow();
		delete m_pDlgMsgBox;
		m_pDlgMsgBox = NULL;
	}

	return 0L;
}

void CGvisR2R_PunchView::ShowDlg(int nID)
{
	HideAllDlg();

	switch (nID)
	{
	case IDD_DLG_FRAME_HIGH:
		if (!m_pDlgFrameHigh)
		{
			m_pDlgFrameHigh = new CDlgFrameHigh(this);
			if (m_pDlgFrameHigh->GetSafeHwnd() == 0)
			{
				m_pDlgFrameHigh->Create();
				m_pDlgFrameHigh->ShowWindow(SW_SHOW);
			}
		}
		else
		{
			m_pDlgFrameHigh->ShowWindow(SW_SHOW);
		}
		break;

	case IDD_DLG_MENU_01:
		if (!m_pDlgMenu01)
		{
			m_pDlgMenu01 = new CDlgMenu01(this);
			if (m_pDlgMenu01->GetSafeHwnd() == 0)
			{
				m_pDlgMenu01->Create();
				m_pDlgMenu01->ShowWindow(SW_SHOW);
			}
		}
		else
		{
			m_pDlgMenu01->ShowWindow(SW_SHOW);
		}
		break;

	case IDD_DLG_MENU_02:
		if (!m_pDlgMenu02)
		{
			m_pDlgMenu02 = new CDlgMenu02(this);
			if (m_pDlgMenu02->GetSafeHwnd() == 0)
			{
				m_pDlgMenu02->Create();
				m_pDlgMenu02->ShowWindow(SW_SHOW);
			}
		}
		else
		{
			m_pDlgMenu02->ShowWindow(SW_SHOW);
		}
		break;

	case IDD_DLG_MENU_03:
		if (!m_pDlgMenu03)
		{
			m_pDlgMenu03 = new CDlgMenu03(this);
			if (m_pDlgMenu03->GetSafeHwnd() == 0)
			{
				m_pDlgMenu03->Create();
				m_pDlgMenu03->ShowWindow(SW_SHOW);
			}
		}
		else
		{
			m_pDlgMenu03->ShowWindow(SW_SHOW);
		}
		break;

	case IDD_DLG_MENU_04:
		if (!m_pDlgMenu04)
		{
			m_pDlgMenu04 = new CDlgMenu04(this);
			if (m_pDlgMenu04->GetSafeHwnd() == 0)
			{
				m_pDlgMenu04->Create();
				m_pDlgMenu04->ShowWindow(SW_SHOW);
			}
		}
		else
		{
			m_pDlgMenu04->ShowWindow(SW_SHOW);
		}
		break;

	case IDD_DLG_MENU_05:
		if (!m_pDlgMenu05)
		{
			m_pDlgMenu05 = new CDlgMenu05(this);
			if (m_pDlgMenu05->GetSafeHwnd() == 0)
			{
				m_pDlgMenu05->Create();
				m_pDlgMenu05->ShowWindow(SW_SHOW);
			}
		}
		else
		{
			m_pDlgMenu05->ShowWindow(SW_SHOW);
		}
		break;

	case IDD_DLG_MENU_06:
		if (!m_pDlgMenu06)
		{
			m_pDlgMenu06 = new CDlgMenu06(this);
			if (m_pDlgMenu06->GetSafeHwnd() == 0)
			{
				m_pDlgMenu06->Create();
				m_pDlgMenu06->ShowWindow(SW_SHOW);
			}
		}
		else
		{
			m_pDlgMenu06->ShowWindow(SW_SHOW);
		}
		break;
	}
}

void CGvisR2R_PunchView::HideAllDlg()
{
	if (m_pDlgMenu01 && m_pDlgMenu01->GetSafeHwnd())
	{
		if (m_pDlgMenu01->IsWindowVisible())
			m_pDlgMenu01->ShowWindow(SW_HIDE);
	}
	if (m_pDlgMenu02 && m_pDlgMenu02->GetSafeHwnd())
	{
		if (m_pDlgMenu02->IsWindowVisible())
			m_pDlgMenu02->ShowWindow(SW_HIDE);
	}
	if (m_pDlgMenu03 && m_pDlgMenu03->GetSafeHwnd())
	{
		if (m_pDlgMenu03->IsWindowVisible())
			m_pDlgMenu03->ShowWindow(SW_HIDE);
	}
	if (m_pDlgMenu04 && m_pDlgMenu04->GetSafeHwnd())
	{
		if (m_pDlgMenu04->IsWindowVisible())
			m_pDlgMenu04->ShowWindow(SW_HIDE);
	}
	if (m_pDlgMenu05 && m_pDlgMenu05->GetSafeHwnd())
	{
		if (m_pDlgMenu05->IsWindowVisible())
			m_pDlgMenu05->ShowWindow(SW_HIDE);
	}
	if (m_pDlgMenu06 && m_pDlgMenu06->GetSafeHwnd())
	{
		if (m_pDlgMenu06->IsWindowVisible())
			m_pDlgMenu06->ShowWindow(SW_HIDE);
	}
}

void CGvisR2R_PunchView::DelAllDlg()
{
	if (m_pDlgMenu06 != NULL)
	{
		delete m_pDlgMenu06;
		m_pDlgMenu06 = NULL;
	}
	if (m_pDlgMenu05 != NULL)
	{
		delete m_pDlgMenu05;
		m_pDlgMenu05 = NULL;
	}
	if (m_pDlgMenu04 != NULL)
	{
		delete m_pDlgMenu04;
		m_pDlgMenu04 = NULL;
	}
	if (m_pDlgMenu03 != NULL)
	{
		delete m_pDlgMenu03;
		m_pDlgMenu03 = NULL;
	}
	if (m_pDlgMenu02 != NULL)
	{
		delete m_pDlgMenu02;
		m_pDlgMenu02 = NULL;
	}
	if (m_pDlgMenu01 != NULL)
	{
		delete m_pDlgMenu01;
		m_pDlgMenu01 = NULL;
	}
	if (m_pDlgFrameHigh != NULL)
	{
		delete m_pDlgFrameHigh;
		m_pDlgFrameHigh = NULL;
	}

	if (m_pDlgMsgBox != NULL)
	{
		if (m_pDlgMsgBox->GetSafeHwnd())
			m_pDlgMsgBox->DestroyWindow();
		delete m_pDlgMsgBox;
		m_pDlgMsgBox = NULL;
	}
}

LRESULT CGvisR2R_PunchView::OnDlgInfo(WPARAM wParam, LPARAM lParam)
{
	ClrDispMsg();
	CDlgInfo Dlg;
	m_pDlgInfo = &Dlg;
	Dlg.DoModal();
	m_pDlgInfo = NULL;

	if (m_pDlgMenu01)
		m_pDlgMenu01->ChkUserInfo(FALSE);

	return 0L;
}

void CGvisR2R_PunchView::DispIo()
{
	ClrDispMsg();
	CDlgUtil04 Dlg;
	Dlg.DoModal();
}

void CGvisR2R_PunchView::DispDatabaseConnection()
{
	//ClrDispMsg();
	//CDlgUtil07 Dlg;
	//Dlg.DoModal();
}

//void CGvisR2R_PunchView::BuzzerFromThread(BOOL bOn, int nCh)
//{
//	m_bBuzzerFromThread = TRUE;
//}

void CGvisR2R_PunchView::Buzzer(BOOL bOn, int nCh)
{
	m_mgrPunch->Buzzer(bOn, nCh);
}
//void CGvisR2R_PunchView::Buzzer(BOOL bOn, int nCh)
//{
//	//	return; // PLC����
//#ifdef USE_MPE
//	if (!m_pMpe)
//		return;
//
//	if (!bOn)
//	{
//		switch (nCh)
//		{
//		case 0:
//			MpeWrite(_T("MB44015E"), 0);
//			break;
//		case 1:
//			MpeWrite(_T("MB44015F"), 0);
//			break;
//		}
//	}
//	else
//	{
//		switch (nCh)
//		{
//		case 0:
//			MpeWrite(_T("MB44015E"), 0);
//			Sleep(300);
//			MpeWrite(_T("MB44015E"), 1);
//			break;
//		case 1:
//			MpeWrite(_T("MB44015E"), 0);
//			Sleep(300);
//			MpeWrite(_T("MB44015F"), 1);
//			break;
//		}
//	}
//#endif
//}

int CGvisR2R_PunchView::GetCamMstActionCode()
{
	if (!m_mgrReelmap)
		return 0;
	return m_mgrReelmap->m_Master[0].MasterInfo.nActionCode;
}

void CGvisR2R_PunchView::DispStsMainMsg(int nIdx)
{
	CString str;
	str = m_sDispMsg[nIdx];
	pFrm->DispStatusBar(str, nIdx);
}

void CGvisR2R_PunchView::DispThreadTick()
{
	if (m_bDestroyedView)
		return;

	CString str;
	str.Format(_T("%d"), pDoc->m_nShotNum); // "m_sOrderNum-m_sShotNum" : "9-3"
	pFrm->DispStatusBar(str, 5);
#ifdef USE_IDS
	double dFPS[2];
	if (m_mgrPunch->m_pVision[0])
		m_mgrPunch->m_pVision[0]->GetFramesPerSecond(&dFPS[0]);
	if (m_mgrPunch->m_pVision[1])
		m_mgrPunch->m_pVision[1]->GetFramesPerSecond(&dFPS[1]);
	str.Format(_T("%.1f,%.1f"), dFPS[0], dFPS[1]);
	pFrm->DispStatusBar(str, 6);
#else
	str.Format(_T("%d,%d,%d"), m_mgrProcedure->m_nStepAuto, m_mgrProcedure->m_nMkStAuto, m_mgrProcedure->m_nLotEndAuto);
	pFrm->DispStatusBar(str, 6);
#endif
}

//void CGvisR2R_PunchView::SwJog(int nAxisID, int nDir, BOOL bOn)
//{
//	if (!m_mgrPunch->m_pMotion)
//		return;
//
//	double fVel, fAcc, fJerk;
//	double dStep;
//	if (pDoc->Status.bSwJogFast)
//		dStep = 0.5;
//	else
//		dStep = 0.1;
//
//	if (pDoc->Status.bSwJogStep)
//	{
//		if (bOn)
//		{
//			double dPos = m_dEnc[nAxisID]; // m_mgrPunch->m_pMotion->GetActualPosition(nAxisID);
//			if (nDir == M_CW)
//				dPos += dStep;
//			else if (nDir == M_CCW)
//				dPos -= dStep;
//
//			if (nAxisID == AXIS_X0)
//			{
//				if (m_mgrPunch->m_bAuto && m_mgrProcedure->m_bTHREAD_MK[0] && m_mgrProcedure->m_bTHREAD_MK[1] && IsReview())
//				{
//					if (nDir == M_CW) // �� Jog ��ư.
//						return;
//				}
//
//				if (m_mgrPunch->m_pMotion->IsLimit(MS_X0, nDir))
//					return;
//				m_mgrPunch->m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_X0, dStep, fVel, fAcc, fJerk);
//				m_mgrPunch->m_pMotion->Move(MS_X0, dPos, fVel, fAcc, fAcc);
//			}
//			else if (nAxisID == AXIS_Y0)
//			{
//				if (m_mgrPunch->m_pMotion->IsLimit(MS_Y0, nDir))
//					return;
//				m_mgrPunch->m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_Y0, dStep, fVel, fAcc, fJerk);
//				m_mgrPunch->m_pMotion->Move(MS_Y0, dPos, fVel, fAcc, fAcc);
//			}
//			else if (nAxisID == AXIS_X1)
//			{
//				if (m_mgrPunch->m_bAuto && m_mgrProcedure->m_bTHREAD_MK[0] && m_mgrProcedure->m_bTHREAD_MK[1] && IsReview())
//				{
//					if (nDir == M_CW) // �� Jog ��ư.
//						return;
//				}
//
//				if (m_mgrPunch->m_pMotion->IsLimit(MS_X1, nDir))
//					return;
//				m_mgrPunch->m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_X1, dStep, fVel, fAcc, fJerk);
//				m_mgrPunch->m_pMotion->Move(MS_X1, dPos, fVel, fAcc, fAcc);
//			}
//			else if (nAxisID == AXIS_Y1)
//			{
//				if (m_mgrPunch->m_pMotion->IsLimit(MS_Y1, nDir))
//					return;
//				m_mgrPunch->m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_Y1, dStep, fVel, fAcc, fJerk);
//				m_mgrPunch->m_pMotion->Move(MS_Y1, dPos, fVel, fAcc, fAcc);
//			}
//		}
//	}
//	else	// Jog Mode
//	{
//		if (!m_pDlgMenu02)
//			return;
//
//		if (nAxisID == AXIS_Y0)
//		{
//			if (nDir == M_CCW)		// Up
//			{
//				if (bOn)
//					m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_UP);
//				else
//					m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_UP);
//			}
//			else if (nDir == M_CW)	// Dn
//			{
//				if (bOn)
//					m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_DN);
//				else
//					m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_DN);
//			}
//		}
//		else if (nAxisID == AXIS_X0)
//		{
//			if (m_mgrPunch->m_bAuto && m_mgrProcedure->m_bTHREAD_MK[0] && m_mgrProcedure->m_bTHREAD_MK[1] && IsReview())
//			{
//				if (nDir == M_CW) // �� Jog ��ư.
//					return;
//			}
//
//			if (nDir == M_CW)		// Rt
//			{
//				if (bOn)
//					m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_RT);
//				else
//					m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_RT);
//			}
//			else if (nDir == M_CCW)	// Lf
//			{
//				if (bOn)
//					m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_LF);
//				else
//					m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_LF);
//			}
//		}
//		else if (nAxisID == AXIS_Y1)
//		{
//			if (nDir == M_CCW)		// Up
//			{
//				if (bOn)
//					m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_UP2);
//				else
//					m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_UP2);
//			}
//			else if (nDir == M_CW)	// Dn
//			{
//				if (bOn)
//					m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_DN2);
//				else
//					m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_DN2);
//			}
//		}
//		else if (nAxisID == AXIS_X1)
//		{
//			if (m_mgrPunch->m_bAuto && m_mgrProcedure->m_bTHREAD_MK[0] && m_mgrProcedure->m_bTHREAD_MK[1] && IsReview())
//			{
//				if (nDir == M_CW) // �� Jog ��ư.
//					return;
//			}
//
//			if (nDir == M_CW)		// Rt
//			{
//				if (bOn)
//					m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_RT2);
//				else
//					m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_RT2);
//			}
//			else if (nDir == M_CCW)	// Lf
//			{
//				if (bOn)
//					m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_LF2);
//				else
//					m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_LF2);
//			}
//		}
//	}
//}

//BOOL CGvisR2R_PunchView::ChkSaftySen() // ���� : TRUE , ���� : FALSE
//{
//	if (pDoc->WorkingInfo.LastJob.bMkSftySen)
//	{
//		if (pDoc->Status.bSensSaftyMk && !pDoc->Status.bSensSaftyMkF)
//		{
//			pDoc->Status.bSensSaftyMkF = TRUE;
//			m_bSwStopNow = TRUE;
//			m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("�� ��"), RGB_RED);			
//			//TowerLamp(RGB_RED, TRUE);
//			Buzzer(TRUE, 0);
//			m_bTIM_SAFTY_STOP = TRUE;//MsgBox(_T("�Ͻ����� - ��ŷ�� ���������� �����Ǿ����ϴ�."));
//			SetTimer(TIM_SAFTY_STOP, 100, NULL);
//		}
//		else if (!pDoc->Status.bSensSaftyMk && pDoc->Status.bSensSaftyMkF)
//		{
//			pDoc->Status.bSensSaftyMkF = FALSE;
//			Buzzer(FALSE, 0);
//			Sleep(300);
//		}
//	}
//
//	return (pDoc->Status.bSensSaftyMk);
//}

//void CGvisR2R_PunchView::ResetMotion()
//{
//	for (int i = 0; i<MAX_MS; i++)
//	{
//		ResetMotion(i);
//
//		if (i<MAX_AXIS)
//		{
//			while (!m_mgrPunch->m_pMotion->IsServoOn(i))
//			{
//				if (i == MS_X0 || i == MS_Y0)
//					m_mgrPunch->m_pMotion->Clear(MS_X0Y0);
//				else if (i == MS_X1 || i == MS_Y1)
//					m_mgrPunch->m_pMotion->Clear(MS_X1Y1);
//				else
//					m_mgrPunch->m_pMotion->Clear(i);
//				Sleep(30);
//				m_mgrPunch->m_pMotion->ServoOnOff(i, TRUE);
//				Sleep(30);
//			}
//		}
//	}
//}
//
//void CGvisR2R_PunchView::ResetMotion(int nMsId)
//{
//	if (m_pDlgMenu02)
//		m_pDlgMenu02->ResetMotion(nMsId);
//}

//unsigned long CGvisR2R_PunchView::ChkDoor() // 0: All Closed , Open Door Index : Doesn't all closed. (Bit3: F, Bit2: L, Bit1: R, Bit0; B)
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
//			m_bSwStopNow = TRUE;
//			m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("�� ��"), RGB_RED);
//			MsgBox(_T("�Ͻ����� - �˻�� �� ���� �߾� ���� Open"));
//			//TowerLamp(RGB_RED, TRUE);
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
//			m_bSwStopNow = TRUE;
//			m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("�� ��"), RGB_RED);
//			MsgBox(_T("�Ͻ����� - �˻�� �� ���� ���� ���� Open"));
//			//TowerLamp(RGB_RED, TRUE);
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
//			m_bSwStopNow = TRUE;
//			m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("�� ��"), RGB_RED);
//			MsgBox(_T("�Ͻ����� - �˻�� �� ���� ���� ���� Open"));
//			//TowerLamp(RGB_RED, TRUE);
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
//			m_bSwStopNow = TRUE;
//			m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("�� ��"), RGB_RED);
//			MsgBox(_T("�Ͻ����� - �˻�� �� �ĸ� �߾� ���� Open"));
//			//TowerLamp(RGB_RED, TRUE);
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
//			m_bSwStopNow = TRUE;
//			m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("�� ��"), RGB_RED);
//			MsgBox(_T("�Ͻ����� - �˻�� �� �ĸ� ���� ���� Open"));
//			//TowerLamp(RGB_RED, TRUE);
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
//			m_bSwStopNow = TRUE;
//			m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("�� ��"), RGB_RED);
//			MsgBox(_T("�Ͻ����� - �˻�� �� �ĸ� ���� ���� Open"));
//			//TowerLamp(RGB_RED, TRUE);
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
//			m_bSwStopNow = TRUE;
//			m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("�� ��"), RGB_RED);
//			MsgBox(_T("�Ͻ����� - �˻�� �� ���� �߾� ���� Open"));
//			//TowerLamp(RGB_RED, TRUE);
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
//			m_bSwStopNow = TRUE;
//			m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("�� ��"), RGB_RED);
//			MsgBox(_T("�Ͻ����� - �˻�� �� ���� ���� ���� Open"));
//			//TowerLamp(RGB_RED, TRUE);
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
//			m_bSwStopNow = TRUE;
//			m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("�� ��"), RGB_RED);
//			MsgBox(_T("�Ͻ����� - �˻�� �� ���� ���� ���� Open"));
//			//TowerLamp(RGB_RED, TRUE);
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
//			m_bSwStopNow = TRUE;
//			m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("�� ��"), RGB_RED);
//			MsgBox(_T("�Ͻ����� - �˻�� �� �ĸ� �߾� ���� Open"));
//			//TowerLamp(RGB_RED, TRUE);
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
//			m_bSwStopNow = TRUE;
//			m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("�� ��"), RGB_RED);
//			MsgBox(_T("�Ͻ����� - �˻�� �� �ĸ� ���� ���� Open"));
//			//TowerLamp(RGB_RED, TRUE);
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
//			m_bSwStopNow = TRUE;
//			m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("�� ��"), RGB_RED);
//			MsgBox(_T("�Ͻ����� - �˻�� �� �ĸ� ���� ���� Open"));
//			//TowerLamp(RGB_RED, TRUE);
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
//			m_bSwStopNow = TRUE;
//			m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("�� ��"), RGB_RED);
//			MsgBox(_T("�Ͻ����� - ��ŷ�� ���� ���� ���� Open"));
//			//TowerLamp(RGB_RED, TRUE);
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
//			m_bSwStopNow = TRUE;
//			m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("�� ��"), RGB_RED);
//			MsgBox(_T("�Ͻ����� - ��ŷ�� ���� ���� ���� Open"));
//			//TowerLamp(RGB_RED, TRUE);
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
//			m_bSwStopNow = TRUE;
//			m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("�� ��"), RGB_RED);
//			MsgBox(_T("�Ͻ����� - ��ŷ�� �ĸ� ���� ���� Open"));
//			//TowerLamp(RGB_RED, TRUE);
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
//			m_bSwStopNow = TRUE;
//			m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("�� ��"), RGB_RED);
//			MsgBox(_T("�Ͻ����� - ��ŷ�� �ĸ� ���� ���� Open"));
//			//TowerLamp(RGB_RED, TRUE);
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
//			m_bSwStopNow = TRUE;
//			m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("�� ��"), RGB_RED);
//			MsgBox(_T("�Ͻ����� - ���κ� ���� ���� ���� Open"));
//			//TowerLamp(RGB_RED, TRUE);
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
//			m_bSwStopNow = TRUE;
//			m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("�� ��"), RGB_RED);
//			MsgBox(_T("�Ͻ����� - ���κ� ���� ���� ���� Open"));
//			//TowerLamp(RGB_RED, TRUE);
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
//			m_bSwStopNow = TRUE;
//			m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("�� ��"), RGB_RED);
//			MsgBox(_T("�Ͻ����� - ���κ� �ĸ� ���� ���� Open"));
//			//TowerLamp(RGB_RED, TRUE);
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
//			m_bSwStopNow = TRUE;
//			m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("�� ��"), RGB_RED);
//			MsgBox(_T("�Ͻ����� - ���κ� �ĸ� ���� ���� Open"));
//			//TowerLamp(RGB_RED, TRUE);
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
//			m_bSwStopNow = TRUE;
//			m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("�� ��"), RGB_RED);
//			MsgBox(_T("�Ͻ����� - �����Ϸ��� ���� ���� ���� Open"));
//			//TowerLamp(RGB_RED, TRUE);
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
//			m_bSwStopNow = TRUE;
//			m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("�� ��"), RGB_RED);
//			MsgBox(_T("�Ͻ����� - �����Ϸ��� ���� ���� Open"));
//			//TowerLamp(RGB_RED, TRUE);
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
//			m_bSwStopNow = TRUE;
//			m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("�� ��"), RGB_RED);
//			MsgBox(_T("�Ͻ����� - �����Ϸ��� �ĸ� ���� ���� Open"));
//			//TowerLamp(RGB_RED, TRUE);
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
//			m_bSwStopNow = TRUE;
//			m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("�� ��"), RGB_RED);
//			MsgBox(_T("�Ͻ����� - �����Ϸ��� �ĸ� ���� ���� Open"));
//			//TowerLamp(RGB_RED, TRUE);
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
//			m_bSwStopNow = TRUE;
//			m_bSwRunF = FALSE;
//			Stop();
//			pView->DispStsBar(_T("����-19"), 0);
//			DispMain(_T("�� ��"), RGB_RED);
//			MsgBox(_T("�Ͻ����� - �����Ϸ��� ���� ���� ���� Open"));
//			//TowerLamp(RGB_RED, TRUE);
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
//			m_bSwStopNow = TRUE;
//			m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("�� ��"), RGB_RED);
//			MsgBox(_T("�Ͻ����� - �����Ϸ��� ���� ���� Open"));
//			//TowerLamp(RGB_RED, TRUE);
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
//			m_bSwStopNow = TRUE;
//			m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("�� ��"), RGB_RED);
//			MsgBox(_T("�Ͻ����� - �����Ϸ��� �ĸ� ���� ���� Open"));
//			//TowerLamp(RGB_RED, TRUE);
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
//			m_bSwStopNow = TRUE;
//			m_bSwRunF = FALSE;
//			Stop();
//			DispMain(_T("�� ��"), RGB_RED);
//			MsgBox(_T("�Ͻ����� - �����Ϸ��� �ĸ� ���� ���� Open"));
//			//TowerLamp(RGB_RED, TRUE);
//			Buzzer(TRUE, 0);
//		}
//	}
//
//	return ulOpenDoor;
//}

//void CGvisR2R_PunchView::ChkEmg()
//{
//	if (pDoc->Status.bEmgAoi[EMG_F_AOI_UP] && !pDoc->Status.bEmgAoiF[EMG_F_AOI_UP])
//	{
//		pDoc->Status.bEmgAoiF[EMG_F_AOI_UP] = TRUE;
//		m_bSwStopNow = TRUE;
//		m_bSwRunF = FALSE;
//		Stop();
//		DispMain(_T("�� ��"), RGB_RED);
//		MsgBox(_T("������� - �˻�� �� ���� ����ġ"));
//		//TowerLamp(RGB_RED, TRUE);
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
//		m_bSwStopNow = TRUE;
//		m_bSwRunF = FALSE;
//		Stop();
//		DispMain(_T("�� ��"), RGB_RED);
//		MsgBox(_T("������� - �˻�� �� �ĸ� ����ġ"));
//		//TowerLamp(RGB_RED, TRUE);
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
//		m_bSwStopNow = TRUE;
//		m_bSwRunF = FALSE;
//		Stop();
//		DispMain(_T("�� ��"), RGB_RED);
//		MsgBox(_T("������� - �˻�� �� ���� ����ġ"));
//		//TowerLamp(RGB_RED, TRUE);
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
//		m_bSwStopNow = TRUE;
//		m_bSwRunF = FALSE;
//		Stop();
//		DispMain(_T("�� ��"), RGB_RED);
//		MsgBox(_T("������� - �˻�� �� �ĸ� ����ġ"));
//		//TowerLamp(RGB_RED, TRUE);
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
//		m_bSwStopNow = TRUE;
//		m_bSwRunF = FALSE;
//		Stop();
//		DispMain(_T("�� ��"), RGB_RED);
//		MsgBox(_T("������� - ��ŷ�� ���� ����ġ"));
//		//TowerLamp(RGB_RED, TRUE);
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
//		m_bSwStopNow = TRUE;
//		m_bSwRunF = FALSE;
//		Stop();
//		DispMain(_T("�� ��"), RGB_RED);
//		MsgBox(_T("������� - ��ŷ�� ����ġ"));
//		//TowerLamp(RGB_RED, TRUE);
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
//		m_bSwStopNow = TRUE;
//		m_bSwRunF = FALSE;
//		Stop();
//		DispMain(_T("�� ��"), RGB_RED);
//		MsgBox(_T("������� - �����Ϸ��� ����ġ"));
//		//TowerLamp(RGB_RED, TRUE);
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
//		m_bSwStopNow = TRUE;
//		m_bSwRunF = FALSE;
//		Stop();
//		DispMain(_T("�� ��"), RGB_RED);
//		MsgBox(_T("������� - �����Ϸ��� ����ġ"));
//		//TowerLamp(RGB_RED, TRUE);
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
//		m_bSwStopNow = TRUE;
//		m_bSwRunF = FALSE;
//		Stop();
//		DispMain(_T("�� ��"), RGB_RED);
//		MsgBox(_T("������� - ���κ� �����"));
//		//TowerLamp(RGB_RED, TRUE);
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
//		m_bSwStopNow = TRUE;
//		m_bSwRunF = FALSE;
//		Stop();
//		DispMain(_T("�� ��"), RGB_RED);
//		MsgBox(_T("������� - ���κ� ����ġ"));
//		//TowerLamp(RGB_RED, TRUE);
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

int CGvisR2R_PunchView::ChkSerial() // // 0: Continue, -: Previous, +: Discontinue --> 0: Same Serial, -: Decrese Serial, +: Increase Serial
{
	int nSerial0 = m_mgrProcedure->GetBuffer0(); // ù��° ���� �ø��� : ���� �ø����� �ٸ��� 0
	int nSerial1 = m_mgrProcedure->GetBuffer1(); // �ι�° ���� �ø��� : ���� �ø����� �ٸ��� 0
	int nLastShot = pDoc->GetLastShotMk();

	// Last shot ���� ���� �ø��� üũ
	//if (nSerial0 == nLastShot + 1 || nSerial1 == nLastShot + 2)
	//	return 0;

	//if (pDoc->WorkingInfo.LastJob.bLotSep || pDoc->m_bDoneChgLot)
	//{
	//	if (nLastShot == pDoc->m_nLotLastShot && (nSerial0 == 1 || nSerial1 == 2))
	//		return 0;
	//}
	//if (m_bLastProc && nSerial0 == m_nLotEndSerial)
	//	return 0;

	return (nSerial0 - nLastShot);
}

int CGvisR2R_PunchView::GetAoiUpAutoStep()
{
	TCHAR szData[512];
	CString sPath = pDoc->WorkingInfo.System.sPathAoiUpStatusInfo;
	if (0 < ::GetPrivateProfileString(_T("Auto"), _T("nStep"), NULL, szData, sizeof(szData), sPath))
		return _ttoi(szData);

	return (-1);
}

void CGvisR2R_PunchView::SetAoiUpAutoStep(int nStep)
{
	CString sPath = pDoc->WorkingInfo.System.sPathAoiUpStatusInfo;
	CString str;
	str.Format(_T("%d"), nStep);
	::WritePrivateProfileString(_T("Set"), _T("nStep"), str, sPath);
}

int CGvisR2R_PunchView::GetAoiDnAutoStep()
{
	TCHAR szData[512];
	CString sPath = pDoc->WorkingInfo.System.sPathAoiDnStatusInfo;
	if (0 < ::GetPrivateProfileString(_T("Auto"), _T("nStep"), NULL, szData, sizeof(szData), sPath))
		return _ttoi(szData);

	return (-1);
}

void CGvisR2R_PunchView::SetAoiDnAutoStep(int nStep)
{
	CString sPath = pDoc->WorkingInfo.System.sPathAoiDnStatusInfo;
	CString str;
	str.Format(_T("%d"), nStep);
	::WritePrivateProfileString(_T("Set"), _T("nStep"), str, sPath);
}

//CString CGvisR2R_PunchView::GetAoiUpAlarmRestartMsg()
//{
//	CString sMsg = _T("Not Find Message.");
//	TCHAR szData[512];
//	CString sPath = PATH_ALARM;
//	if (0 < ::GetPrivateProfileString(_T("11"), _T("27"), NULL, szData, sizeof(szData), sPath))
//		sMsg = CString(szData);
//
//	return sMsg;
//}

//CString CGvisR2R_PunchView::GetAoiDnAlarmRestartMsg()
//{
//	CString sMsg = _T("Not Find Message.");
//	TCHAR szData[512];
//	CString sPath = PATH_ALARM;
//	if (0 < ::GetPrivateProfileString(_T("11"), _T("28"), NULL, szData, sizeof(szData), sPath))
//		sMsg = CString(szData);
//
//	return sMsg;
//}

void CGvisR2R_PunchView::DoIO()
{
	if (IsRun())
	{
		if (m_pDlgMenu01)
		{
			if (m_pDlgMenu01->IsEnableBtn())
				m_pDlgMenu01->EnableBtn(FALSE);
		}
	}
	else
	{
		if (m_pDlgMenu01)
		{
			if (!m_pDlgMenu01->IsEnableBtn())
				m_pDlgMenu01->EnableBtn(TRUE);
		}
	}
}
//void CGvisR2R_PunchView::DoIO()
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
//	if (m_bCycleStop)
//	{
//		m_bCycleStop = FALSE;
//		//TowerLamp(RGB_YELLOW, TRUE);
//		Buzzer(TRUE);
//		if (!pDoc->m_sAlmMsg.IsEmpty())
//		{
//			MsgBox(pDoc->m_sAlmMsg, 0, 0, DEFAULT_TIME_OUT, FALSE);
//
//			if (pDoc->m_sAlmMsg == GetAoiUpAlarmRestartMsg())
//			{
//				ChkReTestAlarmOnAoiUp();
//			}
//			else if(pDoc->m_sAlmMsg == GetAoiDnAlarmRestartMsg())
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

void CGvisR2R_PunchView::GetEnc()
{
	m_mgrPunch->GetEnc();
}
//void CGvisR2R_PunchView::GetEnc()
//{
//	if (!m_mgrPunch->m_pMotion)
//		return;
//
//	if (m_mgrProcedure)
//	{
//		m_dEnc[AXIS_X0] = m_dEnc[AXIS_X0] = m_mgrPunch->m_pMotion->GetActualPosition(AXIS_X0);
//		m_dEnc[AXIS_Y0] = m_dEnc[AXIS_Y0] = m_mgrPunch->m_pMotion->GetActualPosition(AXIS_Y0);
//		m_dEnc[AXIS_X1] = m_dEnc[AXIS_X1] = m_mgrPunch->m_pMotion->GetActualPosition(AXIS_X1);
//		m_dEnc[AXIS_Y1] = m_dEnc[AXIS_Y1] = m_mgrPunch->m_pMotion->GetActualPosition(AXIS_Y1);
//	}
//	else
//	{
//		m_dEnc[AXIS_X0] = m_mgrPunch->m_pMotion->GetActualPosition(AXIS_X0);
//		m_dEnc[AXIS_Y0] = m_mgrPunch->m_pMotion->GetActualPosition(AXIS_Y0);
//		m_dEnc[AXIS_X1] = m_mgrPunch->m_pMotion->GetActualPosition(AXIS_X1);
//		m_dEnc[AXIS_Y1] = m_mgrPunch->m_pMotion->GetActualPosition(AXIS_Y1);
//	}
//}

//void CGvisR2R_PunchView::DoSens()
//{
//}
//
//void CGvisR2R_PunchView::DoBoxSw()
//{
//}
//
//void CGvisR2R_PunchView::DoEmgSw()
//{
//#ifdef USE_MPE
//	if (!pDoc->m_pMpeIo)
//		return;
//#endif
//}

//void CGvisR2R_PunchView::GetMpeIO()
//{
//#ifdef USE_MPE
//	if (!m_pMpe || !pDoc->m_pMpeIo)
//		return;
//
//	m_pMpe->GetMpeIO();
//#endif
//}

//void CGvisR2R_PunchView::GetMpeSignal()
//{
//#ifdef USE_MPE
//	if (!m_pMpe || !pDoc->m_pMpeSignal)
//		return;
//	m_pMpe->GetMpeSignal();
//#endif
//}

//void CGvisR2R_PunchView::GetMpeData()
//{
//#ifdef USE_MPE
//	if (!m_pMpe || !pDoc->m_pMpeData)
//		return;
//
//	// MpeData
//	int nSize, nIdx, nLoop, nSt, k;
//	int nInSeg = pDoc->MkIo.MpeData.nInSeg;
//	int nOutSeg = pDoc->MkIo.MpeData.nOutSeg;
//
//	// MpeData - [In]
//	int *pSeg0 = new int[nInSeg];
//	long **pData0 = new long*[nInSeg];
//	for (nIdx = 0; nIdx<nInSeg; nIdx++)
//	{
//		pData0[nIdx] = new long[16];
//		memset(pData0[nIdx], 0, sizeof(long) * 16);
//	}
//
//	// Group Read...
//	int nGrpStep = pDoc->MkIo.MpeData.nGrpStep;
//	nSt = pDoc->MkIo.MpeData.nGrpInSt;
//	for (nLoop = 0; nLoop<pDoc->MkIo.MpeData.nGrpIn; nLoop++)
//	{
//		for (nIdx = 0; nIdx<nGrpStep; nIdx++)
//			pSeg0[nIdx] = nIdx + nSt;
//		nSize = m_pMpe->ReadData(pSeg0, nGrpStep, pData0);
//
//		for (nIdx = 0; nIdx<nGrpStep; nIdx++)
//		{
//			for (k = 0; k<16; k++)
//				pDoc->m_pMpeData[nIdx + nSt][k] = pData0[nIdx][k];
//		}
//
//		nSt += nGrpStep;
//	}
//	for (nIdx = 0; nIdx<nInSeg; nIdx++)
//		delete[] pData0[nIdx];
//	delete[] pData0;
//	delete[] pSeg0;
//
//	//m_mgrProcedure->m_bChkMpeIoOut = FALSE;
//
//	// MpeData - [Out]
//	int *pSeg2 = new int[nOutSeg];
//	long **pData2 = new long*[nOutSeg];
//	for (nIdx = 0; nIdx<nOutSeg; nIdx++)
//	{
//		pData2[nIdx] = new long[16];
//		memset(pData2[nIdx], 0, sizeof(unsigned long) * 16);
//	}
//
//	// Group Read...
//	nSt = pDoc->MkIo.MpeData.nGrpOutSt;
//	for (nLoop = 0; nLoop<pDoc->MkIo.MpeData.nGrpOut; nLoop++)
//	{
//		for (nIdx = 0; nIdx<nGrpStep; nIdx++)
//			pSeg2[nIdx] = nIdx + nSt;
//		nSize = m_pMpe->ReadData(pSeg2, nGrpStep, pData2);
//
//		for (nIdx = 0; nIdx<nGrpStep; nIdx++)
//		{
//			for (k = 0; k<16; k++)
//				pDoc->m_pMpeData[nIdx + nSt][k] = pData2[nIdx][k];
//		}
//
//		nSt += nGrpStep;
//	}
//	for (nIdx = 0; nIdx<nOutSeg; nIdx++)
//		delete[] pData2[nIdx];
//	delete[] pData2;
//	delete[] pSeg2;
//#endif
//}

int CGvisR2R_PunchView::GetTime(int nSel)
{

	time_t osBinTime;			// C run-time time (defined in <time.h>)
	time(&osBinTime);			// Get the current time from the 
								// operating system.
	CTime Tim(osBinTime);

	switch (nSel)
	{
	case YEAR:
		return Tim.GetYear();
	case MONTH:
		return Tim.GetMonth();
	case DAY:
		return Tim.GetDay();
	case HOUR:
		return Tim.GetHour();
	case MINUTE:
		return Tim.GetMinute();
	case SEC:
		return Tim.GetSecond();
	}

	return 0;
}

CString CGvisR2R_PunchView::GetTime()
{
	CString strVal;
	time_t osBinTime;			// C run-time time (defined in <time.h>)
	time(&osBinTime);			// Get the current time from the 
								// operating system.
	CTime Tim(osBinTime);

	int nYear = Tim.GetYear();
	int nMonth = Tim.GetMonth();
	int nDay = Tim.GetDay();
	int nHour = Tim.GetHour();
	int nMinute = Tim.GetMinute();
	int nSec = Tim.GetSecond();

	strVal.Format(_T("%04d-%02d-%02d, %02d:%02d:%02d"), nYear, nMonth, nDay, nHour, nMinute, nSec);
	return strVal;
}

CString CGvisR2R_PunchView::GetTime(stLotTime &LotTime)
{
	CString strVal;
	time_t osBinTime;			// C run-time time (defined in <time.h>)
	time(&osBinTime);			// Get the current time from the 
								// operating system.
	CTime Tim(osBinTime);

	LotTime.nYear = Tim.GetYear();
	LotTime.nMonth = Tim.GetMonth();
	LotTime.nDay = Tim.GetDay();
	LotTime.nHour = Tim.GetHour();
	LotTime.nMin = Tim.GetMinute();
	LotTime.nSec = Tim.GetSecond();

	strVal.Format(_T("%04d-%02d-%02d,%02d:%02d:%02d"), LotTime.nYear, LotTime.nMonth, LotTime.nDay,
		LotTime.nHour, LotTime.nMin, LotTime.nSec);
	return strVal;
}

CString CGvisR2R_PunchView::GetTime(int &nHour, int &nMinute, int &nSec)
{
	CString strVal;
	time_t osBinTime;			// C run-time time (defined in <time.h>)
	time(&osBinTime);			// Get the current time from the 
								// operating system.
	CTime Tim(osBinTime);

	int nYear = Tim.GetYear();
	int nMonth = Tim.GetMonth();
	int nDay = Tim.GetDay();
	nHour = Tim.GetHour();
	nMinute = Tim.GetMinute();
	nSec = Tim.GetSecond();

	strVal.Format(_T("%04d-%02d-%02d, %02d:%02d:%02d"), nYear, nMonth, nDay, nHour, nMinute, nSec);
	return strVal;
}

void CGvisR2R_PunchView::DispTime()
{
	stLotTime LotTime;
	CString str;
	str = GetTime(LotTime);
	if (m_mgrProcedure->m_sDispTime != str)
	{
		m_mgrProcedure->m_sDispTime = str;
		pFrm->DispStatusBar(str, 7);

		pDoc->WorkingInfo.Lot.CurTime.nYear = LotTime.nYear;
		pDoc->WorkingInfo.Lot.CurTime.nMonth = LotTime.nMonth;
		pDoc->WorkingInfo.Lot.CurTime.nDay = LotTime.nDay;
		pDoc->WorkingInfo.Lot.CurTime.nHour = LotTime.nHour;
		pDoc->WorkingInfo.Lot.CurTime.nMin = LotTime.nMin;
		pDoc->WorkingInfo.Lot.CurTime.nSec = LotTime.nSec;

		if (m_pDlgMenu01)
			m_pDlgMenu01->DispRunTime();
	}
}

// System Input IO .......................................................................
//void CGvisR2R_PunchView::DoSaftySens()
//{
//#ifdef USE_MPE
//	if (!pDoc->m_pMpeIb || !pDoc->m_pMpeIF)
//		return;
//
//	if (!IsRun())
//		return;
//
//	unsigned short usIn = pDoc->m_pMpeIb[7];
//	unsigned short *usInF = &pDoc->m_pMpeIF[7];
//
//	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))		// ��ŷ�� ���� ����
//	{
//		*usInF |= (0x01 << 8);								
//		pDoc->Status.bSensSaftyMk = TRUE;
//	}
//	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))	// ��ŷ�� ���� ����
//	{
//		*usInF &= ~(0x01 << 8);
//		pDoc->Status.bSensSaftyMk = FALSE;
//	}
//#endif
//}
//
//void CGvisR2R_PunchView::DoDoorSens()
//{
//#ifdef USE_MPE
//	unsigned short usIn;
//	unsigned short *usInF;
//
//	if (!pDoc->m_pMpeIb || !pDoc->m_pMpeIF)
//		return;
//
//	if (!IsRun())
//		return;
//
//	usIn = pDoc->m_pMpeIb[1];
//	usInF = &pDoc->m_pMpeIF[1];
//
//	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))		// �����Ϸ� ���� ���� ����
//	{
//		*usInF |= (0x01 << 12);
//		pDoc->Status.bDoorUc[DOOR_FL_UC] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))	// �����Ϸ� ���� ���� ����
//	{
//		*usInF &= ~(0x01 << 12);
//		pDoc->Status.bDoorUc[DOOR_FL_UC] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))		// �����Ϸ� ���� ���� ����
//	{
//		*usInF |= (0x01 << 13);
//		pDoc->Status.bDoorUc[DOOR_FR_UC] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))	// �����Ϸ� ���� ���� ����
//	{
//		*usInF &= ~(0x01 << 13);
//		pDoc->Status.bDoorUc[DOOR_FR_UC] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))		// �����Ϸ� �ĸ� ���� ����(��)
//	{
//		*usInF |= (0x01 << 14);
//		pDoc->Status.bDoorUc[DOOR_BL_UC] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))	// �����Ϸ� �ĸ� ���� ����(��)
//	{
//		*usInF &= ~(0x01 << 14);
//		pDoc->Status.bDoorUc[DOOR_BL_UC] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))		// �����Ϸ� �ĸ� ���� ����(��)
//	{
//		*usInF |= (0x01 << 15);
//		pDoc->Status.bDoorUc[DOOR_BR_UC] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))	// �����Ϸ� �ĸ� ���� ����(��)
//	{
//		*usInF &= ~(0x01 << 15);
//		pDoc->Status.bDoorUc[DOOR_BR_UC] = FALSE;
//	}
//
//	usIn = pDoc->m_pMpeIb[7];
//	usInF = &pDoc->m_pMpeIF[7];
//
//	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))		// ��ŷ�� ���� ���� 1 
//	{
//		*usInF |= (0x01 << 10);
//		pDoc->Status.bDoorMk[DOOR_FL_MK] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))	// ��ŷ�� ���� ���� 1 
//	{
//		*usInF &= ~(0x01 << 10);
//		pDoc->Status.bDoorMk[DOOR_FL_MK] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))		// ��ŷ�� ���� ���� 2 
//	{
//		*usInF |= (0x01 << 11);
//		pDoc->Status.bDoorMk[DOOR_FR_MK] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))	// ��ŷ�� ���� ���� 2
//	{
//		*usInF &= ~(0x01 << 11);
//		pDoc->Status.bDoorMk[DOOR_FR_MK] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))		// ��ŷ�� ���� ���� 3 
//	{
//		*usInF |= (0x01 << 12);
//		pDoc->Status.bDoorMk[DOOR_BL_MK] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))	// ��ŷ�� ���� ���� 3
//	{
//		*usInF &= ~(0x01 << 12);
//		pDoc->Status.bDoorMk[DOOR_BL_MK] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))		// ��ŷ�� ���� ���� 4 
//	{
//		*usInF |= (0x01 << 13);
//		pDoc->Status.bDoorMk[DOOR_BR_MK] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))	// ��ŷ�� ���� ���� 4
//	{
//		*usInF &= ~(0x01 << 13);
//		pDoc->Status.bDoorMk[DOOR_BR_MK] = FALSE;
//	}
//
//	usIn = pDoc->m_pMpeIb[11];
//	usInF = &pDoc->m_pMpeIF[11];
//
//	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))		// �˻�� �� ���� ���� 1 
//	{
//		*usInF |= (0x01 << 10);
//		pDoc->Status.bDoorMk[DOOR_FL_AOI_UP] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))	// �˻�� �� ���� ���� 1
//	{
//		*usInF &= ~(0x01 << 10);
//		pDoc->Status.bDoorMk[DOOR_FL_AOI_UP] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))		// �˻�� �� ���� ���� 2 
//	{
//		*usInF |= (0x01 << 11);
//		pDoc->Status.bDoorMk[DOOR_FR_AOI_UP] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))	// �˻�� �� ���� ���� 2
//	{
//		*usInF &= ~(0x01 << 11);
//		pDoc->Status.bDoorMk[DOOR_FR_AOI_UP] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))		// �˻�� �� ���� ���� 3 
//	{
//		*usInF |= (0x01 << 12);
//		pDoc->Status.bDoorMk[DOOR_BL_AOI_UP] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))	// �˻�� �� ���� ���� 3
//	{
//		*usInF &= ~(0x01 << 12);
//		pDoc->Status.bDoorMk[DOOR_BL_AOI_UP] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))		// �˻�� �� ���� ���� 4 
//	{
//		*usInF |= (0x01 << 13);
//		pDoc->Status.bDoorMk[DOOR_BR_AOI_UP] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))	// �˻�� �� ���� ���� 4
//	{
//		*usInF &= ~(0x01 << 13);
//		pDoc->Status.bDoorMk[DOOR_BR_AOI_UP] = FALSE;
//	}
//
//	usIn = pDoc->m_pMpeIb[15];
//	usInF = &pDoc->m_pMpeIF[15];
//
//	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))		// �˻�� �� ���� ���� 1 
//	{
//		*usInF |= (0x01 << 10);
//		pDoc->Status.bDoorAoi[DOOR_FL_AOI_UP] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))	// �˻�� �� ���� ���� 1
//	{
//		*usInF &= ~(0x01 << 11);
//		pDoc->Status.bDoorAoi[DOOR_FL_AOI_UP] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))		// �˻�� �� ���� ���� 2 
//	{
//		*usInF |= (0x01 << 11);
//		pDoc->Status.bDoorAoi[DOOR_FR_AOI_UP] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))	// �˻�� �� ���� ���� 2
//	{
//		*usInF &= ~(0x01 << 11);
//		pDoc->Status.bDoorAoi[DOOR_FR_AOI_UP] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))		// �˻�� �� ���� ���� 3 
//	{
//		*usInF |= (0x01 << 12);
//		pDoc->Status.bDoorAoi[DOOR_BL_AOI_UP] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))	// �˻�� �� ���� ���� 3
//	{
//		*usInF &= ~(0x01 << 12);
//		pDoc->Status.bDoorAoi[DOOR_BL_AOI_UP] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))		// �˻�� �� ���� ���� 4 
//	{
//		*usInF |= (0x01 << 13);
//		pDoc->Status.bDoorAoi[DOOR_BR_AOI_UP] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))	// �˻�� �� ���� ���� 4
//	{
//		*usInF &= ~(0x01 << 13);
//		pDoc->Status.bDoorAoi[DOOR_BR_AOI_UP] = FALSE;
//	}
//
//	usIn = pDoc->m_pMpeIb[15];
//	usInF = &pDoc->m_pMpeIF[15];
//
//	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))		// �˻�� �� ���� ���� 1 
//	{
//		*usInF |= (0x01 << 10);
//		pDoc->Status.bDoorAoi[DOOR_FL_AOI_DN] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))	// �˻�� �� ���� ���� 1
//	{
//		*usInF &= ~(0x01 << 10);
//		pDoc->Status.bDoorAoi[DOOR_FL_AOI_DN] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))		// �˻�� �� ���� ���� 2 
//	{
//		*usInF |= (0x01 << 11);
//		pDoc->Status.bDoorAoi[DOOR_FR_AOI_DN] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))	// �˻�� �� ���� ���� 2
//	{
//		*usInF &= ~(0x01 << 11);
//		pDoc->Status.bDoorAoi[DOOR_FR_AOI_DN] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))		// �˻�� �� ���� ���� 3 
//	{
//		*usInF |= (0x01 << 12);
//		pDoc->Status.bDoorAoi[DOOR_BL_AOI_DN] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))	// �˻�� �� ���� ���� 3
//	{
//		*usInF &= ~(0x01 << 12);
//		pDoc->Status.bDoorAoi[DOOR_BL_AOI_DN] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))		// �˻�� �� ���� ���� 4 
//	{
//		*usInF |= (0x01 << 13);
//		pDoc->Status.bDoorAoi[DOOR_BR_AOI_DN] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))	// �˻�� �� ���� ���� 4
//	{
//		*usInF &= ~(0x01 << 13);
//		pDoc->Status.bDoorAoi[DOOR_BR_AOI_DN] = FALSE;
//	}
//
//	usIn = pDoc->m_pMpeIb[17];
//	usInF = &pDoc->m_pMpeIF[17];
//
//	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))		// �����Ϸ� ���� ���� ����
//	{
//		*usInF |= (0x01 << 12);
//		pDoc->Status.bDoorRe[DOOR_FL_RC] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))	// �����Ϸ� ���� ���� ����
//	{
//		*usInF &= ~(0x01 << 12);
//		pDoc->Status.bDoorRe[DOOR_FL_RC] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))		// �����Ϸ� ���� ���� ����
//	{
//		*usInF |= (0x01 << 13);
//		pDoc->Status.bDoorRe[DOOR_FR_RC] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))	// �����Ϸ� ���� ���� ����
//	{
//		*usInF &= ~(0x01 << 13);
//		pDoc->Status.bDoorRe[DOOR_FR_RC] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))		// �����Ϸ� �ĸ� ���� ����
//	{
//		*usInF |= (0x01 << 14);
//		pDoc->Status.bDoorRe[DOOR_BL_RC] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))	// �����Ϸ� �ĸ� ���� ����(��)
//	{
//		*usInF &= ~(0x01 << 14);
//		pDoc->Status.bDoorRe[DOOR_BL_RC] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))		// �����Ϸ� �ĸ� ���� ����(��)
//	{
//		*usInF |= (0x01 << 15);
//		pDoc->Status.bDoorRe[DOOR_BR_RC] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))	// �����Ϸ� �ĸ� ���� ����(��)
//	{
//		*usInF &= ~(0x01 << 15);
//		pDoc->Status.bDoorRe[DOOR_BR_RC] = FALSE;
//	}
//
//	usIn = pDoc->m_pMpeIb[27];
//	usInF = &pDoc->m_pMpeIF[27];
//
//	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))		// ���κ� ���� ���� 1
//	{
//		*usInF |= (0x01 << 10);
//		pDoc->Status.bDoorEngv[DOOR_FL_ENGV] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))	// ���κ� ���� ���� 1
//	{
//		*usInF &= ~(0x01 << 10);
//		pDoc->Status.bDoorEngv[DOOR_FL_ENGV] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))		// ���κ� ���� ���� 2
//	{
//		*usInF |= (0x01 << 11);
//		pDoc->Status.bDoorEngv[DOOR_FR_ENGV] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))	// ���κ� ���� ���� 2
//	{
//		*usInF &= ~(0x01 << 11);
//		pDoc->Status.bDoorEngv[DOOR_FR_ENGV] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))		// ���κ� ���� ���� 3
//	{
//		*usInF |= (0x01 << 12);
//		pDoc->Status.bDoorEngv[DOOR_BL_ENGV] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))	// ���κ� ���� ���� 3
//	{
//		*usInF &= ~(0x01 << 12);
//		pDoc->Status.bDoorEngv[DOOR_BL_ENGV] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))		// ���κ� ���� ���� 4
//	{
//		*usInF |= (0x01 << 13);
//		pDoc->Status.bDoorEngv[DOOR_BR_ENGV] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))	// ���κ� ���� ���� 4
//	{
//		*usInF &= ~(0x01 << 13);
//		pDoc->Status.bDoorEngv[DOOR_BR_ENGV] = FALSE;
//	}
//
//	usIn = pDoc->m_pMpeIb[28];
//	usInF = &pDoc->m_pMpeIF[28];
//
//
//	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
//	{
//		*usInF |= (0x01 << 4);								// 2D ���� ����ġ
//		m_bSwStopNow = FALSE;
//		if (m_pDlgMenu03)
//			m_pDlgMenu03->SwReset();
//	}
//	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
//	{
//		*usInF &= ~(0x01 << 4);
//	}
//
//#endif
//}

//void CGvisR2R_PunchView::DoEngraveSens()
//{
//#ifdef USE_MPE
//	unsigned short usIn;
//	unsigned short *usInF;
//
//	if (!pDoc->m_pMpeIb || !pDoc->m_pMpeIF)
//		return;
//
//	usIn = pDoc->m_pMpeIb[28];
//	usInF = &pDoc->m_pMpeIF[28];
//
//
//	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
//	{
//		*usInF |= (0x01 << 4);								// 2D ���� ����ġ
//		m_bSwStopNow = FALSE;
//		if (m_pDlgMenu03)
//			m_pDlgMenu03->SwReset();
//	}
//	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
//	{
//		*usInF &= ~(0x01 << 4);
//	}
//#endif
//}

//void CGvisR2R_PunchView::DoModeSel()
//{
//#ifdef USE_MPE
//	if (!pDoc->m_pMpeIo)
//		return;
//
//	BOOL bMode;// [2];
//	bMode = pDoc->m_pMpeIo[36] & (0x01 << 5) ? TRUE : FALSE;	// ��ŷ�� �ڵ� ���� ����ġ ����
//
//	if (bMode)
//	{
//		pDoc->Status.bAuto = TRUE;
//		pDoc->Status.bManual = FALSE;
//		pDoc->Status.bOneCycle = FALSE;
//
//		m_mgrPunch->m_bAuto = TRUE;
//		m_mgrProcedure->m_bManual = FALSE;
//		m_mgrProcedure->m_bOneCycle = FALSE;
//	}
//	else
//	{
//		pDoc->Status.bManual = TRUE;
//		pDoc->Status.bAuto = FALSE;
//		pDoc->Status.bOneCycle = FALSE;
//
//		m_mgrProcedure->m_bManual = TRUE;
//		m_mgrPunch->m_bAuto = FALSE;
//		m_mgrProcedure->m_bOneCycle = FALSE;
//	}
//
//#else
//	pDoc->Status.bAuto = FALSE;
//	pDoc->Status.bManual = TRUE;
//	pDoc->Status.bOneCycle = FALSE;
//#endif
//
//}
//
//void CGvisR2R_PunchView::DoMainSw()
//{
//#ifdef USE_MPE
//	if (!pDoc->m_pMpeIb || !pDoc->m_pMpeIF)
//		return;
//
//	unsigned short usIn = pDoc->m_pMpeIb[4];
//	unsigned short *usInF = &pDoc->m_pMpeIF[4];
//
//	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
//	{
//		*usInF |= (0x01 << 0);								// ��ŷ�� ������� ����ġ(����ͺ�)
//	}
//	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
//	{
//		*usInF &= ~(0x01 << 0);
//	}
//
//	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
//	{
//		*usInF |= (0x01 << 1);								// ��ŷ�� ���� ����ġ
//		m_bSwStopNow = FALSE;
//		if (m_pDlgMenu03)
//			m_pDlgMenu03->SwRun();
//	}
//	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
//	{
//		*usInF &= ~(0x01 << 1);
//	}
//
//	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
//	{
//		*usInF |= (0x01 << 2);								// ��ŷ�� ���� ����ġ
//	}
//	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
//	{
//		*usInF &= ~(0x01 << 2);
//	}
//
//	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
//	{
//		*usInF |= (0x01 << 3);								// ��ŷ�� �����غ� ����ġ
//		m_bSwStopNow = FALSE;
//		if (m_pDlgMenu03)
//			m_pDlgMenu03->SwReady();
//	}
//	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
//	{
//		*usInF &= ~(0x01 << 3);
//	}
//
//	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
//	{
//		*usInF |= (0x01 << 4);								// ��ŷ�� ���� ����ġ
//		m_bSwStopNow = FALSE;
//		if (m_pDlgMenu03)
//			m_pDlgMenu03->SwReset();
//	}
//	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
//	{
//		*usInF &= ~(0x01 << 4);
//	}
//
//	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
//	{
//		*usInF |= (0x01 << 7);								// ��ŷ�� JOG ��ư(��)
//		if (pDoc->Status.bSwJogLeft)
//			SwJog(AXIS_Y0, M_CCW, TRUE);
//		else
//			SwJog(AXIS_Y1, M_CCW, TRUE);
//	}
//	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
//	{
//		*usInF &= ~(0x01 << 7);
//		if (pDoc->Status.bSwJogLeft)
//			SwJog(AXIS_Y0, M_CCW, FALSE);
//		else
//			SwJog(AXIS_Y1, M_CCW, FALSE);
//	}
//
//	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
//	{
//		*usInF |= (0x01 << 8);								// ��ŷ�� JOG ��ư(��)
//		if (pDoc->Status.bSwJogLeft)
//			SwJog(AXIS_Y0, M_CW, TRUE);
//		else
//			SwJog(AXIS_Y1, M_CW, TRUE);
//	}
//	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
//	{
//		*usInF &= ~(0x01 << 8);
//		if (pDoc->Status.bSwJogLeft)
//			SwJog(AXIS_Y0, M_CW, FALSE);
//		else
//			SwJog(AXIS_Y1, M_CW, FALSE);
//	}
//
//	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
//	{
//		*usInF |= (0x01 << 9);								// ��ŷ�� JOG ��ư(��)
//		if (pDoc->Status.bSwJogLeft)
//			SwJog(AXIS_X0, M_CCW, TRUE);
//		else
//			SwJog(AXIS_X1, M_CCW, TRUE);
//	}
//	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
//	{
//		*usInF &= ~(0x01 << 9);
//		if (pDoc->Status.bSwJogLeft)
//			SwJog(AXIS_X0, M_CCW, FALSE);
//		else
//			SwJog(AXIS_X1, M_CCW, FALSE);
//	}
//
//	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
//	{
//		*usInF |= (0x01 << 10);								// ��ŷ�� JOG ��ư(��)
//		if (pDoc->Status.bSwJogLeft)
//			SwJog(AXIS_X0, M_CW, TRUE);
//		else
//			SwJog(AXIS_X1, M_CW, TRUE);
//	}
//	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
//	{
//		*usInF &= ~(0x01 << 10);
//		if (pDoc->Status.bSwJogLeft)
//			SwJog(AXIS_X0, M_CW, FALSE);
//		else
//			SwJog(AXIS_X1, M_CW, FALSE);
//	}
//
//	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
//	{
//		*usInF |= (0x01 << 11);								// ��ŷ�� ��� ����(LEFT)
//		pDoc->Status.bSwJogLeft = TRUE;
//	}
//	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
//	{
//		*usInF &= ~(0x01 << 11);
//		pDoc->Status.bSwJogLeft = FALSE;
//	}
//
//	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
//	{
//		*usInF |= (0x01 << 12);								// ��ŷ�� �ӵ� ����
//		pDoc->Status.bSwJogFast = TRUE;
//	}
//	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
//	{
//		*usInF &= ~(0x01 << 12);
//		pDoc->Status.bSwJogFast = FALSE;
//	}
//
//	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
//	{
//		*usInF |= (0x01 << 13);								// ��ŷ�� ���� ����(INDEX)
//		pDoc->Status.bSwJogStep = TRUE;
//	}
//	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
//	{
//		*usInF &= ~(0x01 << 13);
//		pDoc->Status.bSwJogStep = FALSE;
//	}
//
//	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))
//	{
//		*usInF |= (0x01 << 14);								// SPARE	
//															// No Use....
//	}
//	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))
//	{
//		*usInF &= ~(0x01 << 14);
//		// No Use....
//	}
//
//	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
//	{
//		*usInF |= (0x01 << 15);								// SPARE	
//															// No Use....
//	}
//	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
//	{
//		*usInF &= ~(0x01 << 15);
//		// No Use....
//	}
//
//	if (!pDoc->m_pMpeIo)// || !pDoc->m_pMpeIoF)
//		return;
//
//#endif
//}
//
//void CGvisR2R_PunchView::DoMkSens()
//{
//	unsigned short usIn = 0;
//	unsigned short *usInF = NULL;
//
//	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
//	{
//		*usInF |= (0x01 << 0);								// ��ŷ�� ���̺� ���� ����
//	}
//	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
//	{
//		*usInF &= ~(0x01 << 0);
//	}
//
//	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
//	{
//		*usInF |= (0x01 << 1);								// ��ŷ�� ���̺� �з� ����ġ
//	}
//	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
//	{
//		*usInF &= ~(0x01 << 1);
//	}
//
//	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
//	{
//		*usInF |= (0x01 << 2);								// ��ŷ�� �ǵ� ���� ����
//	}
//	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
//	{
//		*usInF &= ~(0x01 << 2);
//	}
//
//	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
//	{
//		*usInF |= (0x01 << 3);								// ��ŷ�� �ǵ� Ŭ���� ��� ����
//	}
//	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
//	{
//		*usInF &= ~(0x01 << 3);
//	}
//
//	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
//	{
//		*usInF |= (0x01 << 4);								// ��ŷ�� �ǵ� Ŭ���� �ϰ� ����
//	}
//	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
//	{
//		*usInF &= ~(0x01 << 4);
//	}
//
//	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
//	{
//		*usInF |= (0x01 << 5);								// ��ŷ�� ��ũ ���� ����
//	}
//	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
//	{
//		*usInF &= ~(0x01 << 5);
//	}
//
//	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
//	{
//		*usInF |= (0x01 << 6);								// ��ŷ�� ��ũ Ŭ���� ��� ����
//	}
//	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
//	{
//		*usInF &= ~(0x01 << 6);
//	}
//
//	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
//	{
//		*usInF |= (0x01 << 7);								// ��ŷ�� ��ũ Ŭ���� �ϰ� ����
//	}
//	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
//	{
//		*usInF &= ~(0x01 << 7);
//	}
//
//	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
//	{
//		*usInF |= (0x01 << 8);								// ��ŷ�� ���� ��� ����
//	}
//	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
//	{
//		*usInF &= ~(0x01 << 8);
//	}
//
//	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
//	{
//		*usInF |= (0x01 << 9);								// ��ŷ�� ���� �ϰ� ����
//	}
//	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
//	{
//		*usInF &= ~(0x01 << 9);
//	}
//
//	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
//	{
//		*usInF |= (0x01 << 10);								// �˻�� �ǵ� ���� ����
//	}
//	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
//	{
//		*usInF &= ~(0x01 << 10);
//	}
//
//	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
//	{
//		*usInF |= (0x01 << 11);								// �˻�� �ǵ� Ŭ���� ��� ����
//	}
//	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
//	{
//		*usInF &= ~(0x01 << 11);
//	}
//
//	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
//	{
//		*usInF |= (0x01 << 12);								// �˻�� �ǵ� Ŭ���� �ϰ� ����
//	}
//	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
//	{
//		*usInF &= ~(0x01 << 12);
//	}
//
//	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
//	{
//		*usInF |= (0x01 << 13);								// �˻�� ��ũ ���� ����
//	}
//	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
//	{
//		*usInF &= ~(0x01 << 13);
//	}
//
//	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))
//	{
//		*usInF |= (0x01 << 14);								// �˻�� ��ũ Ŭ���� ��� ����
//	}
//	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))
//	{
//		*usInF &= ~(0x01 << 14);
//	}
//
//	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
//	{
//		*usInF |= (0x01 << 15);								// �˻�� ��ũ Ŭ���� �ϰ� ����
//	}
//	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
//	{
//		*usInF &= ~(0x01 << 15);
//	}
//}
//
//void CGvisR2R_PunchView::DoAoiBoxSw()
//{
//	unsigned short usIn = 0;
//	unsigned short *usInF = NULL;
//
//	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
//	{
//		*usInF |= (0x01 << 0);								// �˻�� ���� ��/���� ����ġ
//		if (m_pDlgMenu03)
//			m_pDlgMenu03->SwAoiRelation();
//	}
//	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
//	{
//		*usInF &= ~(0x01 << 0);
//	}
//
//	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
//	{
//		*usInF |= (0x01 << 1);								// �˻�� ���̺� ��ο� ����ġ
//		if (m_pDlgMenu03)
//			m_pDlgMenu03->SwAoiTblBlw();
//	}
//	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
//	{
//		*usInF &= ~(0x01 << 1);
//	}
//
//	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
//	{
//		*usInF |= (0x01 << 2);								// �˻�� �ǵ� ��ȸ�� ����ġ
//	}
//	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
//	{
//		*usInF &= ~(0x01 << 2);
//	}
//
//	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
//	{
//		*usInF |= (0x01 << 3);								// �˻�� �ǵ� ��ȸ�� ����ġ
//	}
//	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
//	{
//		*usInF &= ~(0x01 << 3);
//	}
//
//	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
//	{
//		*usInF |= (0x01 << 4);								// �˻�� �ǵ� ���� ����ġ
//		if (m_pDlgMenu03)
//			m_pDlgMenu03->SwAoiFdVac();
//	}
//	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
//	{
//		*usInF &= ~(0x01 << 4);
//	}
//
//	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
//	{
//		*usInF |= (0x01 << 5);								// �˻�� ��ũ ���� ����ġ
//		if (m_pDlgMenu03)
//			m_pDlgMenu03->SwAoiTqVac();
//	}
//	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
//	{
//		*usInF &= ~(0x01 << 5);
//	}
//
//	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
//	{
//		*usInF |= (0x01 << 6);								// �˻�� ���̺� ���� ����ġ
//		if (m_pDlgMenu03)
//			m_pDlgMenu03->SwAoiTblVac();
//	}
//	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
//	{
//		*usInF &= ~(0x01 << 6);
//	}
//
//	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
//	{
//		*usInF |= (0x01 << 7);								// �˻�� ������ ������ ����ġ
//		if (m_pDlgMenu03)
//			m_pDlgMenu03->SwAoiLsrPt(TRUE);
//	}
//	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
//	{
//		*usInF &= ~(0x01 << 7);
//		if (m_pDlgMenu03)
//			m_pDlgMenu03->SwAoiLsrPt(FALSE);
//	}
//
//	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
//	{
//		*usInF |= (0x01 << 8);								// �˻�� �ǵ� Ŭ���� ����ġ
//		if (m_pDlgMenu03)
//			m_pDlgMenu03->SwAoiFdClp();
//	}
//	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
//	{
//		*usInF &= ~(0x01 << 8);
//	}
//
//	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
//	{
//		*usInF |= (0x01 << 9);								// �˻�� ��ũ Ŭ���� ����ġ
//		if (m_pDlgMenu03)
//			m_pDlgMenu03->SwAoiTqClp();
//	}
//	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
//	{
//		*usInF &= ~(0x01 << 9);
//	}
//}

//void CGvisR2R_PunchView::DoEmgSens()
//{
//#ifdef USE_MPE
//	unsigned short usIn, *usInF;
//
//	if (!pDoc->m_pMpeIb || !pDoc->m_pMpeIF)
//		return;
//
//	usIn = pDoc->m_pMpeIb[0];
//	usInF = &pDoc->m_pMpeIF[0];
//
//	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))		// �����Ϸ� ������� ����ġ
//	{
//		*usInF |= (0x01 << 0);						
//		pDoc->Status.bEmgUc = TRUE;
//	}
//	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
//	{
//		*usInF &= ~(0x01 << 0);
//		pDoc->Status.bEmgUc = FALSE;
//	}
//
//	usIn = pDoc->m_pMpeIb[4];
//	usInF = &pDoc->m_pMpeIF[4];
//
//	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))		// ��ŷ�� ������� ����ġ(����ͺ�)
//	{
//		*usInF |= (0x01 << 0);						
//		pDoc->Status.bEmgMk[EMG_M_MK] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
//	{
//		*usInF &= ~(0x01 << 0);
//		pDoc->Status.bEmgMk[EMG_M_MK] = FALSE;
//	}
//
//	usIn = pDoc->m_pMpeIb[5];
//	usInF = &pDoc->m_pMpeIF[5];
//
//	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))		// ��ŷ�� ������� ����ġ(����ġ��)	
//	{
//		*usInF |= (0x01 << 0);						
//		pDoc->Status.bEmgMk[EMG_B_MK] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
//	{
//		*usInF &= ~(0x01 << 0);
//		pDoc->Status.bEmgMk[EMG_B_MK] = FALSE;
//	}
//
//	usIn = pDoc->m_pMpeIb[8];
//	usInF = &pDoc->m_pMpeIF[8];
//
//	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))		// �˻�� �� ������� ����ġ(�ĸ�) 
//	{
//		*usInF |= (0x01 << 0);						
//		pDoc->Status.bEmgAoi[EMG_B_AOI_UP] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
//	{
//		*usInF &= ~(0x01 << 0);
//		pDoc->Status.bEmgAoi[EMG_B_AOI_UP] = FALSE;
//	}
//
//	usIn = pDoc->m_pMpeIb[12];
//	usInF = &pDoc->m_pMpeIF[12];
//
//	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))		// �˻�� �� ������� ����ġ(�ĸ�) 
//	{
//		*usInF |= (0x01 << 0);						
//		pDoc->Status.bEmgAoi[EMG_B_AOI_DN] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
//	{
//		*usInF &= ~(0x01 << 0);
//		pDoc->Status.bEmgAoi[EMG_B_AOI_DN] = FALSE;
//	}
//
//	usIn = pDoc->m_pMpeIb[16];
//	usInF = &pDoc->m_pMpeIF[16];
//
//	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))		// �����Ϸ� ������� ����ġ
//	{
//		*usInF |= (0x01 << 0);
//		pDoc->Status.bEmgRc = TRUE;
//	}
//	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
//	{
//		*usInF &= ~(0x01 << 0);
//		pDoc->Status.bEmgRc = FALSE;
//	}
//#endif
//}

//void CGvisR2R_PunchView::DoSignal()
//{
//	unsigned short usIn = 0;
//	unsigned short *usInF = NULL;
//
//	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
//	{
//		*usInF |= (0x01 << 0);								// �˻�� �˻� �Ϸ�
//		pDoc->Status.bSigTestDoneAoi = TRUE;
//	}
//	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
//	{
//		*usInF &= ~(0x01 << 0);
//		pDoc->Status.bSigTestDoneAoi = FALSE;
//	}
//
//	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
//	{
//		*usInF |= (0x01 << 1);								// �˻�� ���̺� ���� �Ϸ�
//		pDoc->Status.bSigTblAirAoi = TRUE;
//	}
//	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
//	{
//		*usInF &= ~(0x01 << 1);
//		pDoc->Status.bSigTblAirAoi = FALSE;
//	}
//
//	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
//	{
//		*usInF |= (0x01 << 2);								// SPARE
//															// No Use....
//	}
//	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
//	{
//		*usInF &= ~(0x01 << 2);
//		// No Use....
//	}
//
//	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
//	{
//		*usInF |= (0x01 << 3);								// SPARE
//															// No Use....
//	}
//	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
//	{
//		*usInF &= ~(0x01 << 3);
//		// No Use....
//	}
//
//	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
//	{
//		*usInF |= (0x01 << 4);								// SPARE
//															// No Use....
//	}
//	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
//	{
//		*usInF &= ~(0x01 << 4);
//		// No Use....
//	}
//
//	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
//	{
//		*usInF |= (0x01 << 5);								// SPARE
//															// No Use....
//	}
//	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
//	{
//		*usInF &= ~(0x01 << 5);
//		// No Use....
//	}
//
//	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
//	{
//		*usInF |= (0x01 << 6);								// SPARE
//															// No Use....
//	}
//	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
//	{
//		*usInF &= ~(0x01 << 6);
//		// No Use....
//	}
//
//	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
//	{
//		*usInF |= (0x01 << 7);								// SPARE
//															// No Use....
//	}
//	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
//	{
//		*usInF &= ~(0x01 << 7);
//		// No Use....
//	}
//
//	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
//	{
//		*usInF |= (0x01 << 8);								// �˻�� �ñ׳� Ÿ��-����
//		//TowerLamp(RGB_RED, TRUE, FALSE);
//	}
//	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
//	{
//		*usInF &= ~(0x01 << 8);
//		//TowerLamp(RGB_RED, FALSE, FALSE);
//	}
//
//	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
//	{
//		*usInF |= (0x01 << 9);								// �˻�� �ñ׳� Ÿ��-Ȳ��
//		//TowerLamp(RGB_YELLOW, TRUE, FALSE);
//	}
//	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
//	{
//		*usInF &= ~(0x01 << 9);
//		//TowerLamp(RGB_YELLOW, FALSE, FALSE);
//	}
//
//	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
//	{
//		*usInF |= (0x01 << 10);								// �˻�� �ñ׳� Ÿ��-���
//		//TowerLamp(RGB_GREEN, TRUE, FALSE);
//	}
//	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
//	{
//		*usInF &= ~(0x01 << 10);
//		//TowerLamp(RGB_GREEN, FALSE, FALSE);
//	}
//
//	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
//	{
//		*usInF |= (0x01 << 11);								// �˻�� ���� 1
//		Buzzer(TRUE, 0);
//	}
//	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
//	{
//		*usInF &= ~(0x01 << 11);
//		Buzzer(FALSE, 0);
//	}
//
//	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
//	{
//		*usInF |= (0x01 << 12);								// �˻�� ���� 2
//		Buzzer(TRUE, 1);
//	}
//	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
//	{
//		*usInF &= ~(0x01 << 12);
//		Buzzer(FALSE, 1);
//	}
//
//	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
//	{
//		*usInF |= (0x01 << 13);								// SPARE
//															// No Use....
//	}
//	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
//	{
//		*usInF &= ~(0x01 << 13);
//		// No Use....
//	}
//
//	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))
//	{
//		*usInF |= (0x01 << 14);								// SPARE
//															// No Use....
//	}
//	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))
//	{
//		*usInF &= ~(0x01 << 14);
//		// No Use....
//	}
//
//	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
//	{
//		*usInF |= (0x01 << 15);								// SPARE
//															// No Use....
//	}
//	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
//	{
//		*usInF &= ~(0x01 << 15);
//		// No Use....
//	}
//}

//void CGvisR2R_PunchView::DoUcBoxSw()
//{
//#ifdef USE_MPE
//	if (!pDoc->m_pMpeIo)// || !pDoc->m_pMpeIoF)
//		return;
//
//	unsigned short usIn = 0;
//	unsigned short *usInF = NULL;
//
//	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
//	{
//		*usInF |= (0x01 << 1);								// �����Ϸ� ���� ��/���� ����ġ
//		if (m_pDlgMenu03)
//			m_pDlgMenu03->SwUcRelation();
//	}
//	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
//	{
//		*usInF &= ~(0x01 << 1);
//	}
//
//	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
//	{
//		*usInF |= (0x01 << 2);								// �����Ϸ� ���� ���/�ϰ� ����ġ
//		if (m_pDlgMenu03)
//			m_pDlgMenu03->SwUcDcRlUpDn();
//	}
//	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
//	{
//		*usInF &= ~(0x01 << 2);
//	}
//
//	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
//	{
//		*usInF |= (0x01 << 3);								// �����Ϸ� Ŭ���ѷ� ���/�ϰ� ����ġ
//		if (m_pDlgMenu03)
//			m_pDlgMenu03->SwUcClRlUpDn();
//	}
//	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
//	{
//		*usInF &= ~(0x01 << 3);
//	}
//
//	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
//	{
//		*usInF |= (0x01 << 4);								// �����Ϸ� Ŭ���ѷ����� ���/�ϰ� ����ġ
//		if (m_pDlgMenu03)
//			m_pDlgMenu03->SwUcClRlPshUpDn();
//	}
//	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
//	{
//		*usInF &= ~(0x01 << 4);
//	}
//
//	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
//	{
//		*usInF |= (0x01 << 5);								// �����Ϸ� ��ǰ ������(��) ����ġ
//		if (m_pDlgMenu03)
//			m_pDlgMenu03->SwUcReelJoinL();
//	}
//	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
//	{
//		*usInF &= ~(0x01 << 5);
//	}
//
//	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
//	{
//		*usInF |= (0x01 << 6);								// �����Ϸ� ��ǰ ������(��) ����ġ	
//		if (m_pDlgMenu03)
//			m_pDlgMenu03->SwUcReelJoinR();
//	}
//	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
//	{
//		*usInF &= ~(0x01 << 6);
//	}
//
//	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
//	{
//		*usInF |= (0x01 << 7);								// �����Ϸ� ��ǰ�� ���� ����ġ
//		if (m_pDlgMenu03)
//			m_pDlgMenu03->SwUcReelWheel();
//	}
//	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
//	{
//		*usInF &= ~(0x01 << 7);
//	}
//
//	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
//	{
//		*usInF |= (0x01 << 8);								// �����Ϸ� ����ô Ŭ���� ����ġ
//		if (m_pDlgMenu03)
//			m_pDlgMenu03->SwUcPprChuck();
//	}
//	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
//	{
//		*usInF &= ~(0x01 << 8);
//	}
//
//	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
//	{
//		*usInF |= (0x01 << 9);								// �����Ϸ� ������ ��ȸ�� ����ġ
//	}
//	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
//	{
//		*usInF &= ~(0x01 << 9);
//	}
//
//	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
//	{
//		*usInF |= (0x01 << 10);								// �����Ϸ� ������ ��ȸ�� ����ġ
//	}
//	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
//	{
//		*usInF &= ~(0x01 << 10);
//	}
//
//	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
//	{
//		*usInF |= (0x01 << 11);								// �����Ϸ� ��ǰô Ŭ���� ����ġ
//		if (m_pDlgMenu03)
//			m_pDlgMenu03->SwUcReelChuck();
//	}
//	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
//	{
//		*usInF &= ~(0x01 << 11);
//	}
//
//	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
//	{
//		*usInF |= (0x01 << 12);								// �����Ϸ� ��ǰ�� ��ȸ�� ����ġ
//	}
//	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
//	{
//		*usInF &= ~(0x01 << 12);
//	}
//
//	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
//	{
//		*usInF |= (0x01 << 13);								// �����Ϸ� ��ǰ�� ��ȸ�� ����ġ
//	}
//	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
//	{
//		*usInF &= ~(0x01 << 13);
//	}
//
//	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
//	{
//		*usInF |= (0x01 << 15);								// SPARE
//		// No Use....
//	}
//	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
//	{
//		*usInF &= ~(0x01 << 15);
//		// No Use....
//	}
//#endif
//}
//
//void CGvisR2R_PunchView::DoUcSens1()
//{
//#ifdef USE_MPE
//	if (!pDoc->m_pMpeIo)// || !pDoc->m_pMpeIoF)
//		return;
//
//	unsigned short usIn = 0;
//	unsigned short *usInF = NULL;
//
//	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
//	{
//		*usInF |= (0x01 << 0);								// �����Ϸ� ������������ 1	
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
//	{
//		*usInF &= ~(0x01 << 0);
//	}
//
//	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
//	{
//		*usInF |= (0x01 << 1);								// �����Ϸ� ������������ 2	
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
//	{
//		*usInF &= ~(0x01 << 1);
//	}
//
//	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
//	{
//		*usInF |= (0x01 << 2);								// �����Ϸ� ������������ 3	
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
//	{
//		*usInF &= ~(0x01 << 2);
//	}
//
//	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
//	{
//		*usInF |= (0x01 << 3);								// �����Ϸ� ������������ 4	
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
//	{
//		*usInF &= ~(0x01 << 3);
//	}
//
//	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
//	{
//		*usInF |= (0x01 << 4);								// �����Ϸ� ��ǰ�������� 1	
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
//	{
//		*usInF &= ~(0x01 << 4);
//	}
//
//	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
//	{
//		*usInF |= (0x01 << 5);								// �����Ϸ� ��ǰ�������� 2
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
//	{
//		*usInF &= ~(0x01 << 5);
//	}
//
//	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
//	{
//		*usInF |= (0x01 << 6);								// �����Ϸ� ��ǰ�������� 3	
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
//	{
//		*usInF &= ~(0x01 << 6);
//	}
//
//	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
//	{
//		*usInF |= (0x01 << 7);								// �����Ϸ� ��ǰ�������� 4	
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
//	{
//		*usInF &= ~(0x01 << 7);
//	}
//
//	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
//	{
//		*usInF |= (0x01 << 8);								// �����Ϸ� ��ǰ EPC POS ����Ʈ ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
//	{
//		*usInF &= ~(0x01 << 8);
//	}
//
//	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
//	{
//		*usInF |= (0x01 << 9);								// �����Ϸ� ��ǰ EPC NEG ����Ʈ ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
//	{
//		*usInF &= ~(0x01 << 9);
//	}
//
//	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
//	{
//		*usInF |= (0x01 << 10);								// �����Ϸ� ��ǰ EPC ���� ���� 
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
//	{
//		*usInF &= ~(0x01 << 10);
//	}
//
//	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
//	{
//		*usInF |= (0x01 << 11);								// �����Ϸ� ���� ���� ����(��)
//		pDoc->Status.bDoorUc[DOOR_FL_UC] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
//	{
//		*usInF &= ~(0x01 << 11);
//		pDoc->Status.bDoorUc[DOOR_FL_UC] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
//	{
//		*usInF |= (0x01 << 12);								// �����Ϸ� ���� ���� ����(��)
//	}
//	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
//	{
//		*usInF &= ~(0x01 << 12);
//	}
//
//	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
//	{
//		*usInF |= (0x01 << 13);								// �����Ϸ� ���� ���� ����
//		pDoc->Status.bDoorUc[DOOR_FR_UC] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
//	{
//		*usInF &= ~(0x01 << 13);
//		pDoc->Status.bDoorUc[DOOR_FR_UC] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))
//	{
//		*usInF |= (0x01 << 14);								// �����Ϸ� �ĸ� ���� ����(��)
//		pDoc->Status.bDoorUc[DOOR_BL_UC] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))
//	{
//		*usInF &= ~(0x01 << 14);
//		pDoc->Status.bDoorUc[DOOR_BL_UC] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
//	{
//		*usInF |= (0x01 << 15);								// �����Ϸ� �ĸ� ���� ����(��)
//		pDoc->Status.bDoorUc[DOOR_BR_UC] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
//	{
//		*usInF &= ~(0x01 << 15);
//		pDoc->Status.bDoorUc[DOOR_BR_UC] = FALSE;
//	}
//#endif
//}
//
//void CGvisR2R_PunchView::DoUcSens2()
//{
//#ifdef USE_MPE
//	if (!pDoc->m_pMpeIo)
//		return;
//
//	unsigned short usIn = 0;
//	unsigned short *usInF = NULL;
//
//	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
//	{
//		*usInF |= (0x01 << 0);								// �����Ϸ� �����ټ� POS ����Ʈ ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
//	{
//		*usInF &= ~(0x01 << 0);
//	}
//
//	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
//	{
//		*usInF |= (0x01 << 1);								// �����Ϸ� �����ټ� ���� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
//	{
//		*usInF &= ~(0x01 << 1);
//	}
//
//	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
//	{
//		*usInF |= (0x01 << 2);								// �����Ϸ� �����ټ� 2/3 ���� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
//	{
//		*usInF &= ~(0x01 << 2);
//	}
//
//	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
//	{
//		*usInF |= (0x01 << 3);								// �����Ϸ� �����ټ� 1/3 ���� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
//	{
//		*usInF &= ~(0x01 << 3);
//	}
//
//	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
//	{
//		*usInF |= (0x01 << 4);								// �����Ϸ� �����ټ� ���� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
//	{
//		*usInF &= ~(0x01 << 4);
//	}
//
//	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
//	{
//		*usInF |= (0x01 << 5);								// �����Ϸ� �����ټ� NEG ����Ʈ ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
//	{
//		*usInF &= ~(0x01 << 5);
//	}
//
//	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
//	{
//		*usInF |= (0x01 << 6);								// �����Ϸ� ����ô ���� ����ġ
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
//	{
//		*usInF &= ~(0x01 << 6);
//	}
//
//	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
//	{
//		*usInF |= (0x01 << 7);								// �����Ϸ� �������� �з� ����ġ	
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
//	{
//		*usInF &= ~(0x01 << 7);
//	}
//
//	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
//	{
//		*usInF |= (0x01 << 8);								// �����Ϸ� Ŭ���ѷ� ��� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
//	{
//		*usInF &= ~(0x01 << 8);
//	}
//
//	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
//	{
//		*usInF |= (0x01 << 9);								// �����Ϸ� Ŭ���ѷ� �ϰ� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
//	{
//		*usInF &= ~(0x01 << 9);
//	}
//
//	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
//	{
//		*usInF |= (0x01 << 10);								// �����Ϸ� Ŭ���ѷ� ����(����) ��� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
//	{
//		*usInF &= ~(0x01 << 10);
//	}
//
//	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
//	{
//		*usInF |= (0x01 << 11);								// �����Ϸ� Ŭ���ѷ� ����(����) �ϰ� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
//	{
//		*usInF &= ~(0x01 << 11);
//	}
//
//	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
//	{
//		*usInF |= (0x01 << 12);								// �����Ϸ� Ŭ���ѷ� ����(�ĸ�) ��� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
//	{
//		*usInF &= ~(0x01 << 12);
//	}
//
//	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
//	{
//		*usInF |= (0x01 << 13);								// �����Ϸ� Ŭ���ѷ� ����(�ĸ�) �ϰ� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
//	{
//		*usInF &= ~(0x01 << 13);
//	}
//
//	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))
//	{
//		*usInF |= (0x01 << 14);								// �����Ϸ� ���� ��� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))
//	{
//		*usInF &= ~(0x01 << 14);
//	}
//
//	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
//	{
//		*usInF |= (0x01 << 15);								// �����Ϸ� ���� �ϰ� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
//	{
//		*usInF &= ~(0x01 << 15);
//	}
//#endif
//}
//
//void CGvisR2R_PunchView::DoUcSens3()
//{
//#ifdef USE_MPE
//	if (!pDoc->m_pMpeIo)
//		return;
//
//	unsigned short usIn = 0;
//	unsigned short *usInF = NULL;
//
//	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
//	{
//		*usInF |= (0x01 << 0);								// �����Ϸ� ���� POS ����Ʈ ����
//		pDoc->Status.bSensLmtBufUc[LMT_POS] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
//	{
//		*usInF &= ~(0x01 << 0);
//		pDoc->Status.bSensLmtBufUc[LMT_POS] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
//	{
//		*usInF |= (0x01 << 1);								// �����Ϸ� ���� ���� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
//	{
//		*usInF &= ~(0x01 << 1);
//	}
//
//	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
//	{
//		*usInF |= (0x01 << 2);								// �����Ϸ� ���� 2/3 ���� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
//	{
//		*usInF &= ~(0x01 << 2);
//	}
//
//	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
//	{
//		*usInF |= (0x01 << 3);								// �����Ϸ� ���� 1/3 ���� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
//	{
//		*usInF &= ~(0x01 << 3);
//	}
//
//	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
//	{
//		*usInF |= (0x01 << 4);								// �����Ϸ� ���� ���� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
//	{
//		*usInF &= ~(0x01 << 4);
//	}
//
//	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
//	{
//		*usInF |= (0x01 << 5);								// �����Ϸ� ���� NEG ����Ʈ ����
//		pDoc->Status.bSensLmtBufUc[LMT_NEG] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
//	{
//		*usInF &= ~(0x01 << 5);
//		pDoc->Status.bSensLmtBufUc[LMT_NEG] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
//	{
//		*usInF |= (0x01 << 6);								// �����Ϸ� ��ǰô ���� ����ġ
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
//	{
//		*usInF &= ~(0x01 << 6);
//	}
//
//	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
//	{
//		*usInF |= (0x01 << 7);								// �����Ϸ� ���� ���� 
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
//	{
//		*usInF &= ~(0x01 << 7);
//	}
//
//	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
//	{
//		*usInF |= (0x01 << 8);								// �����Ϸ� ��ǰ�� ���� ��� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
//	{
//		*usInF &= ~(0x01 << 8);
//	}
//
//	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
//	{
//		*usInF |= (0x01 << 9);								// �����Ϸ� ��ǰ�� ���� �ϰ� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
//	{
//		*usInF &= ~(0x01 << 9);
//	}
//
//	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
//	{
//		*usInF |= (0x01 << 10);								// �����Ϸ� ��ǰ ������(��) ��� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
//	{
//		*usInF &= ~(0x01 << 10);
//	}
//
//	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
//	{
//		*usInF |= (0x01 << 11);								// �����Ϸ� ��ǰ ������(��) �ϰ� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
//	{
//		*usInF &= ~(0x01 << 11);
//	}
//
//	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
//	{
//		*usInF |= (0x01 << 12);								// �����Ϸ� ��ǰ ������(��) ��� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
//	{
//		*usInF &= ~(0x01 << 12);
//	}
//
//	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
//	{
//		*usInF |= (0x01 << 13);								// �����Ϸ� ��ǰ ������(��) �ϰ� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
//	{
//		*usInF &= ~(0x01 << 13);
//	}
//
//	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))
//	{
//		*usInF |= (0x01 << 14);								// SPARE	
//		// No Use....
//	}
//	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))
//	{
//		*usInF &= ~(0x01 << 14);
//		// No Use....
//	}
//
//	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
//	{
//		*usInF |= (0x01 << 15);								// �����Ϸ� ��ǰ�� ���� ����ġ ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
//	{
//		*usInF &= ~(0x01 << 15);
//	}
//#endif
//}
//
//void CGvisR2R_PunchView::DoRcBoxSw()
//{
//#ifdef USE_MPE
//	if (!pDoc->m_pMpeIo)
//		return;
//
//	unsigned short usIn = 0;
//	unsigned short *usInF = NULL;
//
//	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
//	{
//		*usInF |= (0x01 << 1);								// �����Ϸ� ���� ��/���� ����ġ
//	}
//	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
//	{
//		*usInF &= ~(0x01 << 1);
//	}
//
//	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
//	{
//		*usInF |= (0x01 << 2);								// �����Ϸ� ���� ���/�ϰ� ����ġ
//	}
//	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
//	{
//		*usInF &= ~(0x01 << 2);
//	}
//
//	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
//	{
//		*usInF |= (0x01 << 3);								// SPARE
//		// No Use....
//	}
//	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
//	{
//		*usInF &= ~(0x01 << 3);
//		// No Use....
//	}
//
//	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
//	{
//		*usInF |= (0x01 << 4);								// SPARE
//		// No Use....
//	}
//	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
//	{
//		*usInF &= ~(0x01 << 4);
//		// No Use....
//	}
//
//	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
//	{
//		*usInF |= (0x01 << 5);								// �����Ϸ� ��ǰ ������(��) ����ġ
//	}
//	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
//	{
//		*usInF &= ~(0x01 << 5);
//	}
//
//	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
//	{
//		*usInF |= (0x01 << 6);								// �����Ϸ� ��ǰ ������(��) ����ġ
//	}
//	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
//	{
//		*usInF &= ~(0x01 << 6);
//	}
//
//	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
//	{
//		*usInF |= (0x01 << 7);								// �����Ϸ� ��ǰ�� ���� ����ġ
//	}
//	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
//	{
//		*usInF &= ~(0x01 << 7);
//	}
//
//	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
//	{
//		*usInF |= (0x01 << 8);								// �����Ϸ� ����ô Ŭ���� ����ġ
//	}
//	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
//	{
//		*usInF &= ~(0x01 << 8);
//	}
//
//	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
//	{
//		*usInF |= (0x01 << 9);								// �����Ϸ� ������ ��ȸ�� ����ġ
//	}
//	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
//	{
//		*usInF &= ~(0x01 << 9);
//	}
//
//	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
//	{
//		*usInF |= (0x01 << 10);								// �����Ϸ� ������ ��ȸ�� ����ġ
//	}
//	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
//	{
//		*usInF &= ~(0x01 << 10);
//	}
//
//	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
//	{
//		*usInF |= (0x01 << 11);								// �����Ϸ� ��ǰô Ŭ���� ����ġ
//	}
//	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
//	{
//		*usInF &= ~(0x01 << 11);
//	}
//
//	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
//	{
//		*usInF |= (0x01 << 12);								// �����Ϸ� ��ǰ�� ��ȸ�� ����ġ
//	}
//	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
//	{
//		*usInF &= ~(0x01 << 12);
//	}
//
//	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
//	{
//		*usInF |= (0x01 << 13);								// �����Ϸ� ��ǰ�� ��ȸ�� ����ġ
//	}
//	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
//	{
//		*usInF &= ~(0x01 << 13);
//	}
//
//	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
//	{
//		*usInF |= (0x01 << 15);								// SPARE
//		// No Use....
//	}
//	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
//	{
//		*usInF &= ~(0x01 << 15);
//		// No Use....
//	}
//#endif
//}
//
//void CGvisR2R_PunchView::DoRcSens1()
//{
//#ifdef USE_MPE
//	if (!pDoc->m_pMpeIo)
//		return;
//
//	unsigned short usIn = 0;
//	unsigned short *usInF = NULL;
//
//	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
//	{
//		*usInF |= (0x01 << 0);								// �����Ϸ� ������������ 1
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
//	{
//		*usInF &= ~(0x01 << 0);
//	}
//
//	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
//	{
//		*usInF |= (0x01 << 1);								// �����Ϸ� ������������ 2
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
//	{
//		*usInF &= ~(0x01 << 1);
//	}
//
//	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
//	{
//		*usInF |= (0x01 << 2);								// �����Ϸ� ������������ 3
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
//	{
//		*usInF &= ~(0x01 << 2);
//	}
//
//	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
//	{
//		*usInF |= (0x01 << 3);								// �����Ϸ� ������������ 4
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
//	{
//		*usInF &= ~(0x01 << 3);
//	}
//
//	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
//	{
//		*usInF |= (0x01 << 4);								// �����Ϸ� ��ǰ�������� 1
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
//	{
//		*usInF &= ~(0x01 << 4);
//	}
//
//	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
//	{
//		*usInF |= (0x01 << 5);								// �����Ϸ� ��ǰ�������� 2
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
//	{
//		*usInF &= ~(0x01 << 5);
//	}
//
//	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
//	{
//		*usInF |= (0x01 << 6);								// �����Ϸ� ��ǰ�������� 3
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
//	{
//		*usInF &= ~(0x01 << 6);
//	}
//
//	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
//	{
//		*usInF |= (0x01 << 7);								// �����Ϸ� ��ǰ�������� 4
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
//	{
//		*usInF &= ~(0x01 << 7);
//	}
//
//	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
//	{
//		*usInF |= (0x01 << 8);								// �����Ϸ� ��ǰ EPC POS ����Ʈ ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
//	{
//		*usInF &= ~(0x01 << 8);
//	}
//
//	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
//	{
//		*usInF |= (0x01 << 9);								// �����Ϸ� ��ǰ EPC NEG ����Ʈ ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
//	{
//		*usInF &= ~(0x01 << 9);
//	}
//
//	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
//	{
//		*usInF |= (0x01 << 10);								// �����Ϸ� ��ǰ EPC ���� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
//	{
//		*usInF &= ~(0x01 << 10);
//	}
//
//	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
//	{
//		*usInF |= (0x01 << 11);								// �����Ϸ� ���� ���� ����(��)
//		pDoc->Status.bDoorRe[DOOR_FL_RC] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
//	{
//		*usInF &= ~(0x01 << 11);
//		pDoc->Status.bDoorRe[DOOR_FL_RC] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
//	{
//		*usInF |= (0x01 << 12);								// �����Ϸ� ���� ���� ����(��)
//		pDoc->Status.bDoorRe[DOOR_FR_RC] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
//	{
//		*usInF &= ~(0x01 << 12);
//		pDoc->Status.bDoorRe[DOOR_FR_RC] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
//	{
//		*usInF |= (0x01 << 13);								// �����Ϸ� ���� ���� ����
//		pDoc->Status.bDoorRe[DOOR_S_RC] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
//	{
//		*usInF &= ~(0x01 << 13);
//		pDoc->Status.bDoorRe[DOOR_S_RC] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))
//	{
//		*usInF |= (0x01 << 14);								// �����Ϸ� �ĸ� ���� ����(��)
//		pDoc->Status.bDoorRe[DOOR_BL_RC] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))
//	{
//		*usInF &= ~(0x01 << 14);
//		pDoc->Status.bDoorRe[DOOR_BL_RC] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
//	{
//		*usInF |= (0x01 << 15);								// �����Ϸ� �ĸ� ���� ����(��)
//		pDoc->Status.bDoorRe[DOOR_BR_RC] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
//	{
//		*usInF &= ~(0x01 << 15);
//		pDoc->Status.bDoorRe[DOOR_BR_RC] = FALSE;
//	}
//#endif
//}
//
//void CGvisR2R_PunchView::DoRcSens2()
//{
//#ifdef USE_MPE
//	if (!pDoc->m_pMpeIo)
//		return;
//
//	unsigned short usIn = 0;
//	unsigned short *usInF = NULL;
//
//	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
//	{
//		*usInF |= (0x01 << 0);								// �����Ϸ� �����ټ� POS ����Ʈ ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
//	{
//		*usInF &= ~(0x01 << 0);
//	}
//
//	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
//	{
//		*usInF |= (0x01 << 1);								// �����Ϸ� �����ټ� ���� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
//	{
//		*usInF &= ~(0x01 << 1);
//	}
//
//	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
//	{
//		*usInF |= (0x01 << 2);								// �����Ϸ� �����ټ� 2/3 ���� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
//	{
//		*usInF &= ~(0x01 << 2);
//	}
//
//	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
//	{
//		*usInF |= (0x01 << 3);								// �����Ϸ� �����ټ� 1/3 ���� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
//	{
//		*usInF &= ~(0x01 << 3);
//	}
//
//	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
//	{
//		*usInF |= (0x01 << 4);								// �����Ϸ� �����ټ� ���� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
//	{
//		*usInF &= ~(0x01 << 4);
//	}
//
//	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
//	{
//		*usInF |= (0x01 << 5);								// �����Ϸ� �����ټ� NEG ����Ʈ ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
//	{
//		*usInF &= ~(0x01 << 5);
//	}
//
//	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
//	{
//		*usInF |= (0x01 << 6);								// �����Ϸ� ����ô ���� ����ġ
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
//	{
//		*usInF &= ~(0x01 << 6);
//	}
//
//	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
//	{
//		*usInF |= (0x01 << 7);								// �����Ϸ� �������� �з� ����ġ
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
//	{
//		*usInF &= ~(0x01 << 7);
//	}
//
//	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
//	{
//		*usInF |= (0x01 << 8);								// SPARE
//		// No Use....
//	}
//	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
//	{
//		*usInF &= ~(0x01 << 8);
//		// No Use....
//	}
//
//	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
//	{
//		*usInF |= (0x01 << 9);								// SPARE
//		// No Use....
//	}
//	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
//	{
//		*usInF &= ~(0x01 << 9);
//		// No Use....
//	}
//
//	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
//	{
//		*usInF |= (0x01 << 10);								// SPARE
//		// No Use....
//	}
//	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
//	{
//		*usInF &= ~(0x01 << 10);
//		// No Use....
//	}
//
//	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
//	{
//		*usInF |= (0x01 << 11);								// SPARE
//		// No Use....
//	}
//	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
//	{
//		*usInF &= ~(0x01 << 11);
//		// No Use....
//	}
//
//	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
//	{
//		*usInF |= (0x01 << 12);								// SPARE
//		// No Use....
//	}
//	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
//	{
//		*usInF &= ~(0x01 << 12);
//		// No Use....
//	}
//
//	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
//	{
//		*usInF |= (0x01 << 13);								// SPARE
//		// No Use....
//	}
//	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
//	{
//		*usInF &= ~(0x01 << 13);
//		// No Use....
//	}
//
//	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))
//	{
//		*usInF |= (0x01 << 14);								// �����Ϸ� ���� ��� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))
//	{
//		*usInF &= ~(0x01 << 14);
//	}
//
//	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
//	{
//		*usInF |= (0x01 << 15);								// �����Ϸ� ���� �ϰ� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
//	{
//		*usInF &= ~(0x01 << 15);
//	}
//#endif
//}
//
//void CGvisR2R_PunchView::DoRcSens3()
//{
//#ifdef USE_MPE
//	if (!pDoc->m_pMpeIo)
//		return;
//
//	unsigned short usIn = 0;
//	unsigned short *usInF = NULL;
//
//	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
//	{
//		*usInF |= (0x01 << 0);								// �����Ϸ� ���� POS ����Ʈ ����
//		pDoc->Status.bSensLmtBufRc[LMT_POS] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
//	{
//		*usInF &= ~(0x01 << 0);
//		pDoc->Status.bSensLmtBufRc[LMT_POS] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
//	{
//		*usInF |= (0x01 << 1);								// �����Ϸ� ���� ���� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
//	{
//		*usInF &= ~(0x01 << 1);
//	}
//
//	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
//	{
//		*usInF |= (0x01 << 2);								// �����Ϸ� ���� 2/3 ���� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
//	{
//		*usInF &= ~(0x01 << 2);
//	}
//
//	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
//	{
//		*usInF |= (0x01 << 3);								// �����Ϸ� ���� 1/3 ���� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
//	{
//		*usInF &= ~(0x01 << 3);
//	}
//
//	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
//	{
//		*usInF |= (0x01 << 4);								// �����Ϸ� ���� ���� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
//	{
//		*usInF &= ~(0x01 << 4);
//	}
//
//	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
//	{
//		*usInF |= (0x01 << 5);								// �����Ϸ� ���� NEG ����Ʈ ����
//		pDoc->Status.bSensLmtBufRc[LMT_NEG] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
//	{
//		*usInF &= ~(0x01 << 5);
//		pDoc->Status.bSensLmtBufRc[LMT_NEG] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
//	{
//		*usInF |= (0x01 << 6);								// �����Ϸ� ��ǰô ���� ����ġ
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
//	{
//		*usInF &= ~(0x01 << 6);
//	}
//
//	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
//	{
//		*usInF |= (0x01 << 7);								// �����Ϸ� ���� ���� 
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
//	{
//		*usInF &= ~(0x01 << 7);
//	}
//
//	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
//	{
//		*usInF |= (0x01 << 8);								// �����Ϸ� ��ǰ�� ���� ��� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
//	{
//		*usInF &= ~(0x01 << 8);
//	}
//
//	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
//	{
//		*usInF |= (0x01 << 9);								// �����Ϸ� ��ǰ�� ���� �ϰ� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
//	{
//		*usInF &= ~(0x01 << 9);
//	}
//
//	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
//	{
//		*usInF |= (0x01 << 10);								// �����Ϸ� ��ǰ ������(��) ��� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
//	{
//		*usInF &= ~(0x01 << 10);
//	}
//
//	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
//	{
//		*usInF |= (0x01 << 11);								// �����Ϸ� ��ǰ ������(��) �ϰ� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
//	{
//		*usInF &= ~(0x01 << 11);
//	}
//
//	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
//	{
//		*usInF |= (0x01 << 12);								// �����Ϸ� ��ǰ ������(��) ��� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
//	{
//		*usInF &= ~(0x01 << 12);
//	}
//
//	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
//	{
//		*usInF |= (0x01 << 13);								// �����Ϸ� ��ǰ ������(��) �ϰ� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
//	{
//		*usInF &= ~(0x01 << 13);
//	}
//
//	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))
//	{
//		*usInF |= (0x01 << 14);								// �����Ϸ� ��ǰ �ܷ� ���� ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))
//	{
//		*usInF &= ~(0x01 << 14);
//	}
//
//	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
//	{
//		*usInF |= (0x01 << 15);								// �����Ϸ� ��ǰ�� ���� ����ġ ����
//		// Late....
//	}
//	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
//	{
//		*usInF &= ~(0x01 << 15);
//	}
//#endif
//}

void CGvisR2R_PunchView::SetAoiFdPitch(double dPitch)
{
	pDoc->SetAoiFdPitch(dPitch);
}

void CGvisR2R_PunchView::SetMkFdPitch(double dPitch)
{
	pDoc->SetMkFdPitch(dPitch);
	long lData = long(dPitch*1000.0);
#ifdef USE_MPE
	MpeWrite(_T("ML45014"), lData); // ��ŷ�� Feeding �ѷ� Lead Pitch
#endif
}

void CGvisR2R_PunchView::SetBufInitPos(double dPos)
{
	pDoc->SetBufInitPos(dPos);
}

void CGvisR2R_PunchView::SetEngBufInitPos(double dPos)
{
	pDoc->SetEngBufInitPos(dPos);
}

//void CGvisR2R_PunchView::SetBufHomeParam(double dVel, double dAcc)
//{
//	long lVel = long(dVel*1000.0);
//	long lAcc = long(dAcc*1000.0);
//	//	MpeWrite(_T("ML00000"), lVel); // ��ŷ�� ���� Ȩ �ӵ�
//	//	MpeWrite(_T("ML00000"), lAcc); // ��ŷ�� ���� Ȩ ���ӵ�
//	//	pDoc->SetBufInitPos(dVel, dAcc);
//}

LRESULT CGvisR2R_PunchView::OnBufThreadDone(WPARAM wPara, LPARAM lPara)
{
	//if(m_mgrPunch->m_pMotion)
	//{
	// 	m_mgrPunch->m_pMotion->SetOriginPos(AXIS_AOIFD);
	// 	m_mgrPunch->m_pMotion->SetOriginPos(AXIS_BUF);
	// 	m_mgrPunch->m_pMotion->SetOriginPos(AXIS_RENC);
	//}
	// 
	//if(m_mgrPunch->m_pVision[0])
	// 	m_mgrPunch->m_pVision[0]->SetClrOverlay();
	// 
	//if(m_mgrPunch->m_pVision[1])
	// 	m_mgrPunch->m_pVision[1]->SetClrOverlay();

	return 0L;
}

//.........................................................................................

BOOL CGvisR2R_PunchView::WatiDispMain(int nDelay)
{
	if (m_nWatiDispMain % nDelay)
	{
		m_nWatiDispMain++;
		return TRUE;
	}

	m_nWatiDispMain = 0;
	m_nWatiDispMain++;
	return FALSE;
}

void CGvisR2R_PunchView::DispMain(CString sMsg, COLORREF rgb)
{
	pDoc->SetMonDispMain(sMsg);

	m_csDispMain.Lock();
	m_bDispMain = FALSE;
	stDispMain stData(sMsg, rgb);
	m_ArrayDispMain.Add(stData);
	m_bDispMain = TRUE;

	if (sMsg == _T("�� ��"))
	{
		pDoc->SetMkMenu03(_T("Main"), _T("Stop"), TRUE);
		pDoc->SetMkMenu03(_T("Main"), _T("Run"), FALSE);
	}
	else
	{
		pDoc->SetMkMenu03(_T("Main"), _T("Run"), TRUE);
		pDoc->SetMkMenu03(_T("Main"), _T("Stop"), FALSE);
	}

	sMsg.Empty();
	m_csDispMain.Unlock();
}

int CGvisR2R_PunchView::DoDispMain()
{
	int nRtn = -1;

	if (!m_bDispMain)
		return nRtn;

	int nCount = m_ArrayDispMain.GetSize();
	if (nCount>0)
	{
		stDispMain stDispMsg;

		m_csDispMain.Lock();
		stDispMsg = m_ArrayDispMain.GetAt(0);
		m_ArrayDispMain.RemoveAt(0);
		m_csDispMain.Unlock();

		if (m_pDlgMenu01)
		{
			CString sMsg = stDispMsg.strMsg;
			COLORREF rgb = stDispMsg.rgb;
			m_sDispMain = sMsg;
			m_pDlgMenu01->DispMain(sMsg, rgb);
			return 0;
		}
	}

	return nRtn;
}

BOOL CGvisR2R_PunchView::IsReady()
{
	if (m_pDlgMenu03)
		return m_pDlgMenu03->GetReady();

	return FALSE;
}

BOOL CGvisR2R_PunchView::IsAuto()
{
	if (pDoc->Status.bAuto)
		return TRUE;
	return FALSE;
}

void CGvisR2R_PunchView::Shift2Buf()	// ���������� ������ �ø���� Share������ �ø����� �������� Ȯ�� �� �ű�.
{
	int nLastListBuf;
	if (m_mgrProcedure->m_nShareUpS > 0)
	{
		m_mgrProcedure->m_bLoadShare[0] = TRUE;
		pDoc->m_ListBuf[0].Push(m_mgrProcedure->m_nShareUpS);
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (bDualTest)
	{
		if (m_mgrProcedure->m_nShareDnS > 0)
		{
			m_mgrProcedure->m_bLoadShare[1] = TRUE;
			pDoc->m_ListBuf[1].Push(m_mgrProcedure->m_nShareDnS);

			if (m_mgrProcedure->m_nShareDnS == m_mgrProcedure->m_nAoiLastSerial[0] - 3 && m_mgrProcedure->m_nAoiLastSerial[0] > 0)
			{
				if (m_mgrPunch->IsVsDn())
				{
					m_mgrProcedure->SetDummyDn();
					Sleep(30);
					m_mgrProcedure->SetDummyDn();
					Sleep(30);
					m_mgrProcedure->SetDummyDn();
					Sleep(30);
				}
			}
		}
	}

	m_mgrReelmap->CopyPcrAll();
	m_mgrReelmap->DelSharePcr();
}

BOOL CGvisR2R_PunchView::IsMkFdSts()
{
	if (!m_pDlgMenu03)
		return FALSE;

	BOOL bOn[4] = { 0 };
	// ��ŷ�� - FD/TQ ���� ON, TBL���� OFF, TBL�ı� ON, 
	bOn[0] = m_pDlgMenu03->IsMkFdVac(); // TRUE
	bOn[1] = m_pDlgMenu03->IsMkTqVac(); // TRUE
	bOn[2] = m_pDlgMenu03->IsMkTblVac(); // FALSE
	bOn[3] = m_pDlgMenu03->IsMkTblBlw(); // TRUE

	if (bOn[0] && bOn[1] && !bOn[2] && bOn[3])
		return TRUE;

	return FALSE;
}

//void CGvisR2R_PunchView::SetDelay(int mSec, int nId)
//{
//	if (nId > 10)
//		nId = 9;
//	else if (nId < 0)
//		nId = 0;
//
//	m_mgrProcedure->m_dwSetDlySt[nId] = GetTickCount();
//	m_mgrProcedure->m_dwSetDlyEd[nId] = m_mgrProcedure->m_dwSetDlySt[nId] + mSec;
//}
//
//void CGvisR2R_PunchView::SetDelay0(int mSec, int nId)
//{
//	if (nId > 10)
//		nId = 9;
//	else if (nId < 0)
//		nId = 0;
//
//	m_mgrProcedure->m_dwSetDlySt0[nId] = GetTickCount();
//	m_mgrProcedure->m_dwSetDlyEd0[nId] = m_mgrProcedure->m_dwSetDlySt0[nId] + mSec;
//}
//
//void CGvisR2R_PunchView::SetDelay1(int mSec, int nId)
//{
//	if (nId > 10)
//		nId = 9;
//	else if (nId < 0)
//		nId = 0;
//
//	m_mgrProcedure->m_dwSetDlySt1[nId] = GetTickCount();
//	m_mgrProcedure->m_dwSetDlyEd1[nId] = m_mgrProcedure->m_dwSetDlySt1[nId] + mSec;
//}

//BOOL CGvisR2R_PunchView::WaitDelay(int nId) // F:Done, T:On Waiting....
//{
//	if (nId > 10)
//		nId = 9;
//	else if (nId < 0)
//		nId = 0;
//
//	DWORD dwCur = GetTickCount();
//	if (dwCur < m_mgrProcedure->m_dwSetDlyEd[nId])
//		return TRUE;
//	return FALSE;
//}
//
//BOOL CGvisR2R_PunchView::WaitDelay0(int nId) // F:Done, T:On Waiting....
//{
//	if (nId > 10)
//		nId = 9;
//	else if (nId < 0)
//		nId = 0;
//
//	DWORD dwCur = GetTickCount();
//	if (dwCur < m_mgrProcedure->m_dwSetDlyEd0[nId])
//		return TRUE;
//	return FALSE;
//}
//
//BOOL CGvisR2R_PunchView::WaitDelay1(int nId) // F:Done, T:On Waiting....
//{
//	if (nId > 10)
//		nId = 9;
//	else if (nId < 0)
//		nId = 0;
//
//	DWORD dwCur = GetTickCount();
//	if (dwCur < m_mgrProcedure->m_dwSetDlyEd1[nId])
//		return TRUE;
//	return FALSE;
//}
//
//BOOL CGvisR2R_PunchView::GetDelay(int &mSec, int nId) // F:Done, T:On Waiting....
//{
//	DWORD dwCur = GetTickCount();
//	mSec = int(dwCur - m_mgrProcedure->m_dwSetDlySt[nId]);
//	if (dwCur <m_mgrProcedure->m_dwSetDlyEd[nId])
//		return TRUE;
//	return FALSE;
//}
//
//BOOL CGvisR2R_PunchView::GetDelay0(int &mSec, int nId) // F:Done, T:On Waiting....
//{
//	DWORD dwCur = GetTickCount();
//	mSec = int(dwCur - m_mgrProcedure->m_dwSetDlySt0[nId]);
//	if (dwCur < m_mgrProcedure->m_dwSetDlyEd0[nId])
//		return TRUE;
//	return FALSE;
//}
//
//BOOL CGvisR2R_PunchView::GetDelay1(int &mSec, int nId) // F:Done, T:On Waiting....
//{
//	DWORD dwCur = GetTickCount();
//	mSec = int(dwCur - m_mgrProcedure->m_dwSetDlySt1[nId]);
//	if (dwCur < m_mgrProcedure->m_dwSetDlyEd1[nId])
//		return TRUE;
//	return FALSE;
//}

void CGvisR2R_PunchView::SetAoiFd()
{
	CfPoint OfSt;
	if (GetAoiUpOffset(OfSt))
	{
		if (m_pDlgMenu02)
		{
			m_dAoiUpFdOffsetX = OfSt.x;
			m_dAoiUpFdOffsetY = OfSt.y;
		}
	}

	m_mgrPunch->MoveAoi(-1.0*OfSt.x);
	if (m_pDlgMenu03)
		m_pDlgMenu03->ChkDoneAoi();

	if (!pDoc->WorkingInfo.LastJob.bAoiOnePnl)
	{
#ifdef USE_MPE
		MpeWrite(_T("MB440151"), 1);	// ���ǳ� �̼ۻ��� ON (PC�� ON, OFF)
#endif
		CString sData, sPath = PATH_WORKING_INFO;
		pDoc->WorkingInfo.LastJob.bMkOnePnl = pDoc->WorkingInfo.LastJob.bAoiOnePnl = TRUE;
		sData.Format(_T("%d"), pDoc->WorkingInfo.LastJob.bMkOnePnl ? 1 : 0);
		::WritePrivateProfileString(_T("Last Job"), _T("Marking One Pannel Move On"), sData, sPath);
		::WritePrivateProfileString(_T("Last Job"), _T("AOI One Pannel Move On"), sData, sPath);
	}
}

//void CGvisR2R_PunchView::m_mgrPunch->MoveAoi(double dOffset)
//{
//	long lData = (long)(dOffset * 1000.0);		// �˻�� �ǵ� ON (PLC�� �ǵ��Ϸ� �� OFF)
//	MpeWrite(_T("MB440160"), 1);	// �˻�� Feeding �ѷ� Offset(*1000, +:�� ����, -�� ����)
//	MpeWrite(_T("ML45064"), lData);
//}

void CGvisR2R_PunchView::SetMkFd()
{
	CfPoint OfSt;
	GetMkOffset(OfSt);

	if (m_mgrProcedure->m_nShareDnCnt > 0)
	{
		if (!(m_mgrProcedure->m_nShareDnCnt % 2))
			m_mgrPunch->MoveMk(-1.0*OfSt.x);
	}
	else
	{
		if (m_mgrProcedure->m_nShareUpCnt > 0)
		{
			if (!(m_mgrProcedure->m_nShareUpCnt % 2))
				m_mgrPunch->MoveMk(-1.0*OfSt.x);
		}
	}
	if (m_pDlgMenu03)
		m_pDlgMenu03->ChkDoneMk();

	if (!pDoc->WorkingInfo.LastJob.bAoiOnePnl)
	{
		MpeWrite(_T("MB440151"), 1);	// ���ǳ� �̼ۻ��� ON (PC�� ON, OFF)

		CString sData, sPath = PATH_WORKING_INFO;
		pDoc->WorkingInfo.LastJob.bMkOnePnl = pDoc->WorkingInfo.LastJob.bAoiOnePnl = TRUE;
		sData.Format(_T("%d"), pDoc->WorkingInfo.LastJob.bMkOnePnl ? 1 : 0);
		::WritePrivateProfileString(_T("Last Job"), _T("Marking One Pannel Move On"), sData, sPath);
		::WritePrivateProfileString(_T("Last Job"), _T("AOI One Pannel Move On"), sData, sPath);
	}
}

//void CGvisR2R_PunchView::m_mgrPunch->MoveMk(double dOffset)
//{
//#ifdef USE_MPE
//	long lData = (long)(dOffset * 1000.0);
//	MpeWrite(_T("MB440161"), 1);		// ��ŷ�� �ǵ� CW ON (PLC�� �ǵ��Ϸ� �� OFF))
//	MpeWrite(_T("ML45066"), lData);		// ��ŷ�� Feeding �ѷ� Offset(*1000, +:�� ����, -�� ����)
//#endif
//}

BOOL CGvisR2R_PunchView::IsMkFd()
{
	if (m_mgrProcedure->m_nShareDnCnt > 0)
	{
		if (!(m_mgrProcedure->m_nShareDnCnt % 2))
		{
#ifdef USE_MPE
			if (pDoc->m_pMpeSignal[5] & (0x01 << 1))	// ��ŷ�� �ǵ� CW ON (PLC�� �ǵ��Ϸ� �� OFF)
				return TRUE;
#endif
			return FALSE;
		}
	}
	else
	{
		if (m_mgrProcedure->m_nShareUpCnt > 0)
		{
			if (!(m_mgrProcedure->m_nShareUpCnt % 2))
			{
#ifdef USE_MPE
				if (pDoc->m_pMpeSignal[5] & (0x01 << 1))	// ��ŷ�� �ǵ� CW ON (PLC�� �ǵ��Ϸ� �� OFF)
					return TRUE;
#endif
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL CGvisR2R_PunchView::IsAoiFd()
{
#ifdef USE_MPE
	if (pDoc->m_pMpeSignal[5] & (0x01 >> 0))	// �˻�� �ǵ� ON (PLC�� �ǵ��Ϸ� �� OFF)
		return TRUE;
#endif
	return FALSE;
}

void CGvisR2R_PunchView::SetMkFd(double dDist)
{
	double fLen = pDoc->GetOnePnlLen();
	double dOffset = dDist - (fLen*2.0);
	m_mgrPunch->MoveMk(dOffset);
}

BOOL CGvisR2R_PunchView::IsMkFdDone()
{
	if (m_mgrProcedure->m_nShareDnCnt > 0)
	{
		if (m_mgrProcedure->m_nShareDnCnt % 2)
			return TRUE;
	}
#ifdef USE_MPE
	if (!(pDoc->m_pMpeSignal[5] & (0x01 << 1)))	// ��ŷ�� �ǵ� ON (PLC�� �ǵ��Ϸ� �� OFF)
		return TRUE;
#endif
	return FALSE;
}

BOOL CGvisR2R_PunchView::IsAoiFdDone()
{
#ifdef USE_MPE
	if (!(pDoc->m_pMpeSignal[5] & (0x01 << 0)))	// �˻�� �ǵ� ON (PLC�� �ǵ��Ϸ� �� OFF)
		return TRUE;
#endif
	return FALSE;
}

double CGvisR2R_PunchView::GetAoi2InitDist()
{
	double dInitD = (_tstof(pDoc->WorkingInfo.Motion.sFdAoiAoiDistShot) - 1.0) * _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen);
	return dInitD;
}

double CGvisR2R_PunchView::GetMkInitDist()
{
	double dInitD0 = _tstof(pDoc->WorkingInfo.Motion.sFdAoiAoiDistShot) * _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen);
	double dInitD1 = (_tstof(pDoc->WorkingInfo.Motion.sFdMkAoiInitDist) + dInitD0) - (_tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen)*2.0);
	return dInitD1;
}

double CGvisR2R_PunchView::GetRemain()
{
#ifdef USE_MPE
	double dCurPosMkFd = (double)pDoc->m_pMpeData[0][0];	// ��ŷ�� Feeding ���ڴ� ��(���� mm )
	double dInitD0 = _tstof(pDoc->WorkingInfo.Motion.sFdAoiAoiDistShot) * _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen);
	double dRemain = _tstof(pDoc->WorkingInfo.Motion.sFdMkAoiInitDist) + dInitD0 - dCurPosMkFd;
	return dRemain;
#else
	return 0.0;
#endif
}

void CGvisR2R_PunchView::UpdateWorking()
{
	if (m_pDlgMenu01)
		m_pDlgMenu01->UpdateWorking();
	if (m_mgrPunch->m_pEngrave)
		m_mgrPunch->m_pEngrave->SwMenu01UpdateWorking(TRUE);
}

//void CGvisR2R_PunchView::StopFromThread()
//{
//	m_mgrPunch->m_bStopFromThread = TRUE;
//}

void CGvisR2R_PunchView::Stop()
{
	CString sMsg;
	if (m_pDlgMenu03)
	{
		if (IsAuto() && IsRun())
		{
			m_mgrProcedure->m_bStopF_Verify = TRUE;
		}

		m_pDlgMenu03->SwStop();
	}
}

BOOL CGvisR2R_PunchView::IsStop()
{
	if (m_sDispMain == _T("�� ��"))
	{
		return TRUE;
	}
	return FALSE;
}

void CGvisR2R_PunchView::ShowLive(BOOL bShow)
{
	if (bShow)
	{
		if (!IsShowLive())
			SetTimer(TIM_SHOW_MENU02, 30, NULL);
	}
	else
	{
		if (IsShowLive())
			SetTimer(TIM_SHOW_MENU01, 30, NULL);
	}
}

BOOL CGvisR2R_PunchView::IsShowLive()
{
	if (m_pDlgMenu02)
	{
		if (m_pDlgMenu02->IsWindowVisible())
			return TRUE;
	}

	return FALSE;
}

void CGvisR2R_PunchView::SetLotSt()
{
	stLotTime LotTime;
	GetTime(LotTime);

	pDoc->WorkingInfo.Lot.StTime.nYear = LotTime.nYear;
	pDoc->WorkingInfo.Lot.StTime.nMonth = LotTime.nMonth;
	pDoc->WorkingInfo.Lot.StTime.nDay = LotTime.nDay;
	pDoc->WorkingInfo.Lot.StTime.nHour = LotTime.nHour;
	pDoc->WorkingInfo.Lot.StTime.nMin = LotTime.nMin;
	pDoc->WorkingInfo.Lot.StTime.nSec = LotTime.nSec;

	pDoc->WorkingInfo.Lot.CurTime.nYear = LotTime.nYear;
	pDoc->WorkingInfo.Lot.CurTime.nMonth = LotTime.nMonth;
	pDoc->WorkingInfo.Lot.CurTime.nDay = LotTime.nDay;
	pDoc->WorkingInfo.Lot.CurTime.nHour = LotTime.nHour;
	pDoc->WorkingInfo.Lot.CurTime.nMin = LotTime.nMin;
	pDoc->WorkingInfo.Lot.CurTime.nSec = LotTime.nSec;

	pDoc->WorkingInfo.Lot.EdTime.nYear = 0;
	pDoc->WorkingInfo.Lot.EdTime.nMonth = 0;
	pDoc->WorkingInfo.Lot.EdTime.nDay = 0;
	pDoc->WorkingInfo.Lot.EdTime.nHour = 0;
	pDoc->WorkingInfo.Lot.EdTime.nMin = 0;
	pDoc->WorkingInfo.Lot.EdTime.nSec = 0;

	m_mgrProcedure->m_dwLotSt = GetTickCount();
	pDoc->SaveLotTime(m_mgrProcedure->m_dwLotSt);
	DispLotTime();

	m_mgrReelmap->SetLotSt();
}

void CGvisR2R_PunchView::SetLotEd()
{
	stLotTime LotTime;
	GetTime(LotTime);

	pDoc->WorkingInfo.Lot.EdTime.nYear = LotTime.nYear;
	pDoc->WorkingInfo.Lot.EdTime.nMonth = LotTime.nMonth;
	pDoc->WorkingInfo.Lot.EdTime.nDay = LotTime.nDay;
	pDoc->WorkingInfo.Lot.EdTime.nHour = LotTime.nHour;
	pDoc->WorkingInfo.Lot.EdTime.nMin = LotTime.nMin;
	pDoc->WorkingInfo.Lot.EdTime.nSec = LotTime.nSec;

	pDoc->WorkingInfo.Lot.CurTime.nYear = LotTime.nYear;
	pDoc->WorkingInfo.Lot.CurTime.nMonth = LotTime.nMonth;
	pDoc->WorkingInfo.Lot.CurTime.nDay = LotTime.nDay;
	pDoc->WorkingInfo.Lot.CurTime.nHour = LotTime.nHour;
	pDoc->WorkingInfo.Lot.CurTime.nMin = LotTime.nMin;
	pDoc->WorkingInfo.Lot.CurTime.nSec = LotTime.nSec;

	m_mgrProcedure->m_dwLotEd = GetTickCount();

	pDoc->SaveLotTime(pDoc->WorkingInfo.Lot.dwStTick);
	DispLotTime();
	m_mgrReelmap->SetLotEd();
}

void CGvisR2R_PunchView::DispLotTime()
{
	if (m_pDlgMenu01)
		m_pDlgMenu01->DispLotTime();
}

BOOL CGvisR2R_PunchView::IsTest()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)
		return FALSE;

	// MpeIO
	int nInSeg = pDoc->MkIo.MpeIo.nInSeg;
	int nOutSeg = pDoc->MkIo.MpeIo.nOutSeg;

	BOOL bOn0 = (pDoc->m_pMpeIo[nInSeg + 10] & 0x01 << 8) ? TRUE : FALSE; //[34] �˻�� �� �˻� ���� <-> Y4368 I/F
	BOOL bOn1 = (pDoc->m_pMpeIo[nInSeg + 14] & 0x01 << 8) ? TRUE : FALSE; //[38] �˻�� �� �˻� ���� <-> Y4468 I/F

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (bDualTest)
	{
		if (bOn0 || bOn1)
			return TRUE;
	}
	else
	{
		if (bOn0)
			return TRUE;
	}
#endif
	return FALSE;
}

BOOL CGvisR2R_PunchView::IsTestUp()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)
		return FALSE;

	// MpeIO
	int nInSeg = pDoc->MkIo.MpeIo.nInSeg;
	int nOutSeg = pDoc->MkIo.MpeIo.nOutSeg;

	BOOL bOn0 = (pDoc->m_pMpeIo[nInSeg + 10] & 0x01 << 8) ? TRUE : FALSE; //[34] �˻�� �� �˻� ���� <-> Y4368 I/F
	if (bOn0)
		return TRUE;
#endif
	return FALSE;
}

BOOL CGvisR2R_PunchView::IsTestDn()
{
#ifdef USE_MPE
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return FALSE;

	if (!pDoc->m_pMpeIo)
		return FALSE;

	// MpeIO
	int nInSeg = pDoc->MkIo.MpeIo.nInSeg;
	int nOutSeg = pDoc->MkIo.MpeIo.nOutSeg;

	BOOL bOn1 = (pDoc->m_pMpeIo[nInSeg + 14] & 0x01 << 8) ? TRUE : FALSE; //[38] �˻�� �� �˻� ���� <-> Y4468 I/F
	if (bOn1)
		return TRUE;
#endif
	return FALSE;
}

BOOL CGvisR2R_PunchView::IsAoiTblVac()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)
		return FALSE;

	// MpeIO
	int nInSeg = pDoc->MkIo.MpeIo.nInSeg;
	int nOutSeg = pDoc->MkIo.MpeIo.nOutSeg;

	BOOL bOn0 = (pDoc->m_pMpeIo[nInSeg + 10] & 0x01 << 9) ? TRUE : FALSE; //[34] �˻�� �� �˻� ���̺� ���� SOL <-> Y4469 I/F
	BOOL bOn1 = (pDoc->m_pMpeIo[nInSeg + 14] & 0x01 << 9) ? TRUE : FALSE; //[38] �˻�� �� �˻� ���̺� ���� SOL <-> Y4469 I/F
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (bDualTest)
	{
		if (bOn0 || bOn1)
			return TRUE;
	}
	else
	{
		if (bOn0)
			return TRUE;
	}
#endif
	return FALSE;
}

void CGvisR2R_PunchView::SetTest(BOOL bOn)
{
#ifdef USE_MPE
	if (bOn)
	{
		MpeWrite(_T("MB003828"), 1); // �˻�� �� �˻� ���� <-> Y4368 I/F
		MpeWrite(_T("MB003928"), 1); // �˻�� �� �˻� ���� <-> Y4468 I/F
	}
	else
	{
		MpeWrite(_T("MB003828"), 0); // �˻�� �� �˻� ���� <-> Y4368 I/F
		MpeWrite(_T("MB003928"), 0); // �˻�� �� �˻� ���� <-> Y4468 I/F
	}
#endif
}

void CGvisR2R_PunchView::SetTest0(BOOL bOn)
{
#ifdef USE_MPE
	if (bOn)
		MpeWrite(_T("MB003828"), 1); // �˻�� �� �˻� ���� <-> Y4368 I/F
	else
		MpeWrite(_T("MB003828"), 0); // �˻�� �� �˻� ���� <-> Y4368 I/F
#endif
}

void CGvisR2R_PunchView::SetTest1(BOOL bOn)
{
#ifdef USE_MPE
	if (bOn)
		MpeWrite(_T("MB003928"), 1); // �˻�� �� �˻� ���� <-> Y4468 I/F
	else
		MpeWrite(_T("MB003928"), 0); // �˻�� �� �˻� ���� <-> Y4468 I/F
#endif
}

BOOL CGvisR2R_PunchView::IsTestDone()
{
#ifdef USE_MPE
	BOOL bOn0 = (pDoc->m_pMpeIb[10] & (0x01 << 8)) ? TRUE : FALSE;	// �˻�� �� �˻� �Ϸ� <-> X4328 I/F
	BOOL bOn1 = (pDoc->m_pMpeIb[14] & (0x01 << 8)) ? TRUE : FALSE;	// �˻�� �� �˻� �Ϸ� <-> X4428 I/F
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (bDualTest)
	{
		if (bOn0 && bOn1)
			return TRUE;
	}
	else
	{
		if (bOn0)
			return TRUE;
	}

	double dCurPosMkFd = (double)pDoc->m_pMpeData[0][0];	// ��ŷ�� Feeding ���ڴ� ��(���� mm )
	double dTgtFd = _tstof(pDoc->WorkingInfo.Motion.sFdAoiAoiDistShot) * _tstof(pDoc->WorkingInfo.Motion.sAoiFdDist);
	if (dCurPosMkFd + 10.0 < dTgtFd)
	{
		if (bOn0)
			return TRUE;
	}
#endif
	return FALSE;
}

BOOL CGvisR2R_PunchView::IsAoiTblVacDone()
{
#ifdef USE_MPE
	BOOL bOn0 = (pDoc->m_pMpeIb[10] & (0x01 << 9)) ? TRUE : FALSE;	// �˻�� �� ���̺� ���� �Ϸ� <-> X4329 I/F
	BOOL bOn1 = (pDoc->m_pMpeIb[14] & (0x01 << 9)) ? TRUE : FALSE;	// �˻�� �� ���̺� ���� �Ϸ� <-> X4329 I/F
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (bDualTest)
	{
		if (bOn0 && bOn1)
			return TRUE;
	}
	else
	{
		if (bOn0)
			return TRUE;
	}

	double dCurPosMkFd = (double)pDoc->m_pMpeData[0][0];	// ��ŷ�� Feeding ���ڴ� ��(���� mm )
	double dTgtFd = _tstof(pDoc->WorkingInfo.Motion.sFdAoiAoiDistShot) * _tstof(pDoc->WorkingInfo.Motion.sAoiFdDist);
	if (dCurPosMkFd + 10.0 < dTgtFd)
	{
		if (bOn0)
			return TRUE;
	}
#endif
	return FALSE;
}

BOOL CGvisR2R_PunchView::IsTestDoneUp()
{
#ifdef USE_MPE
	BOOL bOn0 = (pDoc->m_pMpeIb[10] & (0x01 << 8)) ? TRUE : FALSE;	// �˻�� �� �˻� �Ϸ� <-> X4328 I/F
	if (bOn0)
		return TRUE;
#endif
	return TRUE;
}

BOOL CGvisR2R_PunchView::IsTestDoneDn()
{
#ifdef USE_MPE
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return TRUE;

	BOOL bOn1 = (pDoc->m_pMpeIb[14] & (0x01 << 8)) ? TRUE : FALSE;	// �˻�� �� �˻� �Ϸ� <-> X4428 I/F
	if (bOn1)
		return TRUE;
#endif
	return TRUE;
}

//void CGvisR2R_PunchView::SetReMk(BOOL bMk0, BOOL bMk1)
//{
//	CfPoint ptPnt;
//	int nSerial, nTot;
//
//	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
//
//	if (bMk0)
//	{
//		if (!m_mgrProcedure->m_bTHREAD_MK[0])
//		{
//			m_mgrProcedure->m_nStepMk[0] = 0;
//			m_mgrProcedure->m_nMkPcs[0] = 0;
//			m_mgrProcedure->m_bDoneMk[0] = FALSE;
//			//m_bReMark[0] = FALSE;
//			m_mgrProcedure->m_bTHREAD_MK[0] = TRUE;
//
//			nSerial = m_mgrProcedure->m_nBufUpSerial[0]; // Cam0
//
//			nTot = GetTotDefPcs(nSerial);
//			if (nTot>0)
//			{
//				ptPnt = GetMkPnt(nSerial, 0);
//				m_mgrProcedure->m_dTarget[AXIS_X0] = ptPnt.x;
//				m_mgrProcedure->m_dTarget[AXIS_Y0] = ptPnt.y;
//				if (nTot>1)
//				{
//					ptPnt = GetMkPnt(nSerial, 1);
//					m_mgrProcedure->m_dNextTarget[AXIS_X0] = ptPnt.x;
//					m_mgrProcedure->m_dNextTarget[AXIS_Y0] = ptPnt.y;
//				}
//				else
//				{
//					m_mgrProcedure->m_dNextTarget[AXIS_X0] = -1.0;
//					m_mgrProcedure->m_dNextTarget[AXIS_Y0] = -1.0;
//				}
//			}
//			else
//			{
//				m_mgrProcedure->m_dTarget[AXIS_X0] = -1.0;
//				m_mgrProcedure->m_dTarget[AXIS_Y0] = -1.0;
//				m_mgrProcedure->m_dNextTarget[AXIS_X0] = -1.0;
//				m_mgrProcedure->m_dNextTarget[AXIS_Y0] = -1.0;
//			}
//		}
//	}
//
//	if (bMk1)
//	{
//		if (!m_mgrProcedure->m_bTHREAD_MK[1])
//		{
//			m_mgrProcedure->m_nStepMk[1] = 0;
//			m_mgrProcedure->m_nMkPcs[1] = 0;
//			m_mgrProcedure->m_bDoneMk[1] = FALSE;
//			//m_bReMark[1] = FALSE;
//			m_mgrProcedure->m_bTHREAD_MK[1] = TRUE;
//
//			nSerial = m_mgrProcedure->m_nBufUpSerial[1]; // Cam1
//
//			nTot = GetTotDefPcs(nSerial);
//			if (nTot>0)
//			{
//				ptPnt = GetMkPnt(nSerial, 0);
//				m_mgrProcedure->m_dTarget[AXIS_X1] = ptPnt.x;
//				m_mgrProcedure->m_dTarget[AXIS_Y1] = ptPnt.y;
//				if (nTot>1)
//				{
//					ptPnt = GetMkPnt(nSerial, 1);
//					m_mgrProcedure->m_dNextTarget[AXIS_X1] = ptPnt.x;
//					m_mgrProcedure->m_dNextTarget[AXIS_Y1] = ptPnt.y;
//				}
//				else
//				{
//					m_mgrProcedure->m_dNextTarget[AXIS_X1] = -1.0;
//					m_mgrProcedure->m_dNextTarget[AXIS_Y1] = -1.0;
//				}
//			}
//			else
//			{
//				m_mgrProcedure->m_dTarget[AXIS_X1] = -1.0;
//				m_mgrProcedure->m_dTarget[AXIS_Y1] = -1.0;
//				m_mgrProcedure->m_dNextTarget[AXIS_X1] = -1.0;
//				m_mgrProcedure->m_dNextTarget[AXIS_Y1] = -1.0;
//			}
//		}
//	}
//}

//BOOL CGvisR2R_PunchView::IsReMk()
//{
//	if (IsMoveDone())
//		return FALSE;
//
//	if (m_mgrProcedure->m_bReMark[0] && m_mgrProcedure->m_bDoneMk[1])
//	{
//		m_mgrProcedure->m_bReMark[0] = FALSE;
//		SetReMk(TRUE, FALSE);
//		return TRUE;
//	}
//	else if (m_mgrProcedure->m_bDoneMk[0] && m_mgrProcedure->m_bReMark[1])
//	{
//		m_mgrProcedure->m_bReMark[1] = FALSE;
//		SetReMk(FALSE, TRUE);
//		return TRUE;
//	}
//	else if (m_mgrProcedure->m_bReMark[0] && m_mgrProcedure->m_bReMark[1])
//	{
//		m_mgrProcedure->m_bReMark[0] = FALSE;
//		m_mgrProcedure->m_bReMark[1] = FALSE;
//		SetReMk(TRUE, TRUE);
//		return TRUE;
//	}
//
//	return FALSE;
//}

int CGvisR2R_PunchView::GetSerial()
{
	int nSerial = 0;
	if (m_pDlgMenu01)
		nSerial = m_pDlgMenu01->GetSerial();
	return nSerial;
}

double CGvisR2R_PunchView::GetMkFdLen()
{
	int nLast = pDoc->GetLastShotMk();
	double dLen = (double)nLast * _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen);
	return dLen;
}

double CGvisR2R_PunchView::GetAoiUpFdLen()
{
	int nLast = GetLastShotUp();
	double dLen = (double)nLast * _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen);
	return dLen;
}

double CGvisR2R_PunchView::GetAoiDnFdLen()
{
	int nLast = GetLastShotDn();
	double dLen = (double)nLast * _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen);
	return dLen;
}

double CGvisR2R_PunchView::GetTotVel()
{
	CString str, sPrev;
	int nDiff;
	int nHour, nMin, nSec;
	int nStYear, nStMonth, nStDay, nStHour, nStMin, nStSec;
	int nCurYear, nCurMonth, nCurDay, nCurHour, nCurMin, nCurSec;
	int nEdYear, nEdMonth, nEdDay, nEdHour, nEdMin, nEdSec;

	nStYear = pDoc->WorkingInfo.Lot.StTime.nYear;
	nStMonth = pDoc->WorkingInfo.Lot.StTime.nMonth;
	nStDay = pDoc->WorkingInfo.Lot.StTime.nDay;
	nStHour = pDoc->WorkingInfo.Lot.StTime.nHour;
	nStMin = pDoc->WorkingInfo.Lot.StTime.nMin;
	nStSec = pDoc->WorkingInfo.Lot.StTime.nSec;

	nCurYear = pDoc->WorkingInfo.Lot.CurTime.nYear;
	nCurMonth = pDoc->WorkingInfo.Lot.CurTime.nMonth;
	nCurDay = pDoc->WorkingInfo.Lot.CurTime.nDay;
	nCurHour = pDoc->WorkingInfo.Lot.CurTime.nHour;
	nCurMin = pDoc->WorkingInfo.Lot.CurTime.nMin;
	nCurSec = pDoc->WorkingInfo.Lot.CurTime.nSec;

	nEdYear = pDoc->WorkingInfo.Lot.EdTime.nYear;
	nEdMonth = pDoc->WorkingInfo.Lot.EdTime.nMonth;
	nEdDay = pDoc->WorkingInfo.Lot.EdTime.nDay;
	nEdHour = pDoc->WorkingInfo.Lot.EdTime.nHour;
	nEdMin = pDoc->WorkingInfo.Lot.EdTime.nMin;
	nEdSec = pDoc->WorkingInfo.Lot.EdTime.nSec;

	int nTotSec = 0;
	double dMkFdLen = GetMkFdLen();
	if (!nStYear && !nStMonth && !nStDay && !nStHour && !nStMin && !nStSec)
	{
		return 0.0;
	}
	else if (!nEdYear && !nEdMonth && !nEdDay && !nEdHour && !nEdMin && !nEdSec)
	{
		nDiff = (GetTickCount() - m_mgrProcedure->m_dwLotSt) / 1000;
		nHour = int(nDiff / 3600);
		nMin = int((nDiff - 3600 * nHour) / 60);
		nSec = nDiff % 60;
	}
	else
	{
		nDiff = (m_mgrProcedure->m_dwLotEd - m_mgrProcedure->m_dwLotSt) / 1000;
		nHour = int(nDiff / 3600);
		nMin = int((nDiff - 3600 * nHour) / 60);
		nSec = nDiff % 60;
	}

	nTotSec = nHour * 3600 + nMin * 60 + nSec;
	double dVel = 0.0;
	if (nTotSec > 0)
		dVel = dMkFdLen / (double)nTotSec; // [mm/sec]

	m_mgrProcedure->m_dTotVel = dVel;
	return dVel;
}

double CGvisR2R_PunchView::GetPartVel()
{
	double dLen = _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen) * 2.0;
	double dSec = (double)m_mgrProcedure->GetCycTime() / 1000.0;
	double dVel = 0.0;
	if (dSec > 0.0)
		dVel = dLen / dSec; // [mm/sec]
	m_mgrProcedure->m_dPartVel = dVel;
	return dVel;
}

BOOL CGvisR2R_PunchView::IsBufferUp()
{
	return m_mgrProcedure->IsBufferUp();
}

BOOL CGvisR2R_PunchView::IsBufferDn()
{
	return m_mgrProcedure->IsBufferDn();
}

BOOL CGvisR2R_PunchView::IsShare()
{
	return m_mgrProcedure->IsShare();
}

BOOL CGvisR2R_PunchView::IsShareUp()
{
	return m_mgrProcedure->IsShareUp();
}

BOOL CGvisR2R_PunchView::IsShareDn()
{
	return m_mgrProcedure->IsShareDn();
}

BOOL CGvisR2R_PunchView::IsVsShare()
{
	return m_mgrProcedure->IsVsShare();
}

int CGvisR2R_PunchView::GetShareUp()
{
	return m_mgrProcedure->GetShareUp();
}

int CGvisR2R_PunchView::GetShareDn()
{
	return m_mgrProcedure->GetShareDn();
}

BOOL CGvisR2R_PunchView::ChkLastProc()
{
	BOOL bRtn = FALSE;
	if (m_pDlgMenu01)
		bRtn = (m_pDlgMenu01->m_bLastProc);
	return bRtn;
}

BOOL CGvisR2R_PunchView::ChkLastProcFromUp()
{
	BOOL bRtn = TRUE;
	if (m_pDlgMenu01)
		bRtn = (m_pDlgMenu01->m_bLastProcFromUp);
	return bRtn;
}

BOOL CGvisR2R_PunchView::ChkLastProcFromEng()
{
	BOOL bRtn = TRUE;
	if (m_pDlgMenu01)
		bRtn = (m_pDlgMenu01->m_bLastProcFromEng);
	return bRtn;
}

BOOL CGvisR2R_PunchView::SetSerial(int nSerial, BOOL bDumy)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.28"));
		return 0;
	}

	if (!m_pDlgMenu01)
		return FALSE;

	int nPrevSerial = m_pDlgMenu01->GetCurSerial();

	if (nPrevSerial == nSerial)
		return TRUE;

	BOOL bRtn[2] = {1};
	bRtn[0] = m_pDlgMenu01->SetSerial(nSerial, bDumy);
	if (pDoc->GetTestMode() == MODE_OUTER)
	{
		if (!m_pDlgMenu06)
			bRtn[1] = m_pDlgMenu06->SetSerial(nSerial, bDumy);
	}

	return (bRtn[0] && bRtn[1]);
}

BOOL CGvisR2R_PunchView::SetSerialReelmap(int nSerial, BOOL bDumy)
{
	if (!m_pDlgMenu01)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Error - SetSerialReelmap : m_pDlgMenu01 is NULL."));
		return FALSE;
	}

	//if (pDoc->GetTestMode() == MODE_OUTER)
	//{
	//	if (!m_pDlgMenu06)
	//	{
	//		pView->ClrDispMsg();
	//		AfxMessageBox(_T("Error - SetSerialReelmap : m_pDlgMenu06 is NULL."));
	//		return FALSE;
	//	}

	//	m_pDlgMenu06->SetSerialReelmap(nSerial, bDumy);
	//}

	return m_pDlgMenu01->SetSerialReelmap(nSerial, bDumy);
}

BOOL CGvisR2R_PunchView::SetSerialMkInfo(int nSerial, BOOL bDumy)
{
	if (!m_pDlgMenu01)
		return FALSE;
	return m_pDlgMenu01->SetSerialMkInfo(nSerial, bDumy);
}

void CGvisR2R_PunchView::InitAuto(BOOL bInit)
{
	int nCam, nPoint, kk, a, b;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	m_mgrProcedure->InitAuto(bInit);
	m_mgrPunch->InitAuto(bInit);

	for (kk = 0; kk < 10; kk++)
	{
		m_bDispMsgDoAuto[kk] = FALSE;
		m_nStepDispMsg[kk] = 0;
	}

	if(m_pDlgMenu01)
		m_pDlgMenu01->DispLotEndSerial(0);

	for (nCam = 0; nCam < 2; nCam++)
	{
		for (nPoint = 0; nPoint < 4; nPoint++)
		{
			m_dMkFdOffsetX[nCam][nPoint] = 0.0;
			m_dMkFdOffsetY[nCam][nPoint] = 0.0;
		}
	}

	m_dAoiUpFdOffsetX = 0.0;
	m_dAoiUpFdOffsetY = 0.0;
	m_dAoiDnFdOffsetX = 0.0;
	m_dAoiDnFdOffsetY = 0.0;

	pDoc->m_sAlmMsg = _T("");
	pDoc->m_sIsAlmMsg = _T("");
	pDoc->m_sPrevAlmMsg = _T("");

	pDoc->SetMkMenu01(_T("Signal"), _T("DispDefImg"), _T("0"));

	MpeWrite(_T("MB440100"), 0); // PLC �����غ� �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)
	MpeWrite(_T("MB440110"), 0); // ��ŷ����(PC�� Ȯ���ϰ� Reset��Ŵ.)-20141029
	MpeWrite(_T("MB440150"), 0); // ��ŷ�� ��ŷ�� ON (PC�� ON, OFF)
	MpeWrite(_T("MB440170"), 0); // ��ŷ�Ϸ�(PLC�� Ȯ���ϰ� Reset��Ŵ.)-20141029

	InitAutoEngSignal();

	m_mgrPunch->MoveInitPos1();
	Sleep(30);
	m_mgrPunch->MoveInitPos0();

	m_mgrPunch->InitIoWrite();

	SetTest(FALSE);	// �˻�� ��/�� �˻� ���� (Off)
	if (m_pDlgMenu01)
	{
		m_pDlgMenu01->m_bLastProc = FALSE;

		if (MODE_INNER != pDoc->GetTestMode())
			m_pDlgMenu01->m_bLastProcFromUp = TRUE;
		else
			m_pDlgMenu01->m_bLastProcFromEng = TRUE;

		m_pDlgMenu01->ResetSerial();
		m_pDlgMenu01->ResetLastProc();
	}

	pDoc->m_nEjectBufferLastShot = -1;

	pDoc->m_bUpdateForNewJob[0] = FALSE;
	pDoc->m_bUpdateForNewJob[1] = FALSE;

	if (bInit) // �̾�Ⱑ �ƴѰ��.
	{
		MpeWrite(_T("MB440187"), 0); // �̾��(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141121

		pDoc->m_nPrevSerial = 0;
		pDoc->m_bNewLotShare[0] = FALSE;
		pDoc->m_bNewLotShare[1] = FALSE;
		pDoc->m_bNewLotBuf[0] = FALSE;
		pDoc->m_bNewLotBuf[1] = FALSE;
		pDoc->m_bDoneChgLot = FALSE;

		m_pDlgFrameHigh->m_nMkLastShot = 0;
		m_pDlgFrameHigh->m_nAoiLastShot[0] = 0;
		m_pDlgFrameHigh->m_nAoiLastShot[1] = 0;

		if (m_pDlgMenu01)
			m_pDlgMenu01->ResetLotTime();

		ClrMkInfo(); // 20220420 - Happen Release Trouble

		if (m_pDlgFrameHigh)
		{
			m_pDlgFrameHigh->SetMkLastShot(0);
			m_pDlgFrameHigh->SetAoiLastShot(0, 0);
			m_pDlgFrameHigh->SetAoiLastShot(1, 0);
			m_pDlgFrameHigh->SetEngraveLastShot(0);
		}
	}
	else
	{
		MpeWrite(_T("MB440187"), 1); // �̾��(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141121
		m_mgrReelmap->ClrFixPcs();

#ifndef TEST_MODE
		m_mgrReelmap->ReloadReelmap();
		UpdateRst();
#endif
		DispLotStTime();
		m_mgrReelmap->RestoreReelmap();
	}
}

void CGvisR2R_PunchView::SetListBuf()	// pDoc->m_ListBuf�� ���� ������ �ø����ȣ�� ������ �簻����.
{
	m_mgrProcedure->SetListBuf();
}

void CGvisR2R_PunchView::DispLotStTime()
{
	TCHAR szData[MAX_PATH];
	CString sPath = PATH_WORKING_INFO;

	// [Lot]
	if (0 < ::GetPrivateProfileString(_T("Lot"), _T("Start Tick"), NULL, szData, sizeof(szData), sPath))
		pDoc->WorkingInfo.Lot.dwStTick = _tstoi(szData);
	else
		pDoc->WorkingInfo.Lot.dwStTick = 0;

	if (0 < ::GetPrivateProfileString(_T("Lot"), _T("Start Year"), NULL, szData, sizeof(szData), sPath))
		pDoc->WorkingInfo.Lot.StTime.nYear = _tstoi(szData);
	else
		pDoc->WorkingInfo.Lot.StTime.nYear = 0;

	if (0 < ::GetPrivateProfileString(_T("Lot"), _T("Start Month"), NULL, szData, sizeof(szData), sPath))
		pDoc->WorkingInfo.Lot.StTime.nMonth = _tstoi(szData);
	else
		pDoc->WorkingInfo.Lot.StTime.nMonth = 0;

	if (0 < ::GetPrivateProfileString(_T("Lot"), _T("Start Day"), NULL, szData, sizeof(szData), sPath))
		pDoc->WorkingInfo.Lot.StTime.nDay = _tstoi(szData);
	else
		pDoc->WorkingInfo.Lot.StTime.nDay = 0;

	if (0 < ::GetPrivateProfileString(_T("Lot"), _T("Start Hour"), NULL, szData, sizeof(szData), sPath))
		pDoc->WorkingInfo.Lot.StTime.nHour = _tstoi(szData);
	else
		pDoc->WorkingInfo.Lot.StTime.nHour = 0;

	if (0 < ::GetPrivateProfileString(_T("Lot"), _T("Start Minute"), NULL, szData, sizeof(szData), sPath))
		pDoc->WorkingInfo.Lot.StTime.nMin = _tstoi(szData);
	else
		pDoc->WorkingInfo.Lot.StTime.nMin = 0;

	if (0 < ::GetPrivateProfileString(_T("Lot"), _T("Start Second"), NULL, szData, sizeof(szData), sPath))
		pDoc->WorkingInfo.Lot.StTime.nSec = _tstoi(szData);
	else
		pDoc->WorkingInfo.Lot.StTime.nSec = 0;

	m_mgrProcedure->m_dwLotSt = (DWORD)pDoc->WorkingInfo.Lot.dwStTick;
	DispLotTime();
}

void CGvisR2R_PunchView::ClrMkInfo()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	m_mgrReelmap->ClearReelmap();
	if (m_pDlgMenu01)
	{
		m_pDlgMenu01->ResetMkInfo();
		m_pDlgMenu01->SetPnlNum();
		m_pDlgMenu01->SetPnlDefNum();
		m_pDlgMenu01->RefreshRmap();
		m_pDlgMenu01->UpdateRst();
	}


	CString sLot, sLayerUp, sLayerDn;
	BOOL bDualTestInner;
	if (pDoc->GetCurrentInfoEng())
	{
		if (m_mgrReelmap->GetItsSerialInfo(0, bDualTestInner, sLot, sLayerUp, sLayerDn, 3))
		{
			if (m_mgrReelmap->m_Master[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, sLayerUp))
			{
				if (m_pDlgMenu06)
				{
					m_pDlgMenu06->ResetMkInfo();
					m_pDlgMenu06->RefreshRmap();
					m_pDlgMenu06->UpdateRst();
				}
			}
		}
	}

	UpdateWorking();
}

void CGvisR2R_PunchView::ModelChange(int nAoi) // 0 : AOI-Up , 1 : AOI-Dn 
{
	if (nAoi == 0)
	{
		pDoc->SetModelInfoUp();
		m_mgrReelmap->OpenReelmapUp(); // At Start...
		m_mgrReelmap->SetPathAtBufUp();
		if (m_pDlgMenu01)
			m_pDlgMenu01->UpdateData();

		pDoc->m_pSpecLocal->MakeDir(pDoc->Status.PcrShare[0].sModel, pDoc->Status.PcrShare[0].sLayer);

		if (pDoc->GetTestMode() == MODE_OUTER)
			m_mgrReelmap->OpenReelmapInner();
	}
	else if (nAoi == 1)
	{
		pDoc->SetModelInfoDn();
		m_mgrReelmap->OpenReelmapDn(); // At Start...
		m_mgrReelmap->SetPathAtBufDn();
		if (m_pDlgMenu01)
			m_pDlgMenu01->UpdateData();
		pDoc->m_pSpecLocal->MakeDir(pDoc->Status.PcrShare[1].sModel, pDoc->Status.PcrShare[1].sLayer);
	}
}

void CGvisR2R_PunchView::ResetMkInfo(int nAoi) // 0 : AOI-Up , 1 : AOI-Dn , 2 : AOI-UpDn
{
	m_mgrReelmap->ResetMkInfo(nAoi);

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	BOOL bDualTestInner, bGetCurrentInfoEng;
	CString sLot, sLayerUp, sLayerDn;
	bGetCurrentInfoEng = pDoc->GetCurrentInfoEng();

	if (nAoi == 0 || nAoi == 2)
	{
		if (!bDualTest)
		{
			m_bDrawGL = FALSE;
			if (m_pDlgMenu01)
				m_pDlgMenu01->ResetMkInfo();
		}

		if (bGetCurrentInfoEng)
		{
			if (m_mgrReelmap->GetItsSerialInfo(0, bDualTestInner, sLot, sLayerUp, sLayerDn, 3))
			{
				if (m_mgrReelmap->m_Master[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, sLayerUp))
				{
					if (!bDualTestInner)
					{
						if (m_pDlgMenu06)
							m_pDlgMenu06->ResetMkInfo();
					}
				}
			}
		}

		if (pView->IsLastJob(0)) // Up
		{
			if (m_mgrPunch->m_pEngrave)
				m_mgrPunch->m_pEngrave->SwMenu01UpdateWorking(TRUE);
		}

		m_mgrPunch->SetAlignPosUp();

		if (m_pDlgMenu02)
		{
			m_pDlgMenu02->ChgModelUp();

			if (bDualTest)
				m_pDlgMenu02->ChgModelDn();
		}

		if (m_pDlgMenu01)
		{
			m_pDlgMenu01->InitCadImgUp();
		}

		if (bGetCurrentInfoEng)
		{
			if (m_mgrReelmap->GetItsSerialInfo(0, bDualTestInner, sLot, sLayerUp, sLayerDn, 0))
			{
				if (m_mgrReelmap->m_Master[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, sLayerUp))
				{
					if (m_pDlgMenu06)
					{
						m_pDlgMenu06->InitCadImgUp();
						if (bDualTestInner)
							m_pDlgMenu06->InitCadImgDn();

						m_pDlgMenu06->InitGL();
						m_pDlgMenu06->RefreshRmap();
					}

				}
			}
		}

		if (m_pDlgMenu01)
		{
			if (!bDualTest)
			{
				m_pDlgMenu01->InitGL();
				m_bDrawGL = TRUE;
				m_pDlgMenu01->RefreshRmap();
			}
		}
	}

	if (bDualTest)
	{
		if (nAoi == 1 || nAoi == 2)
		{
			m_bDrawGL = FALSE;
			if (m_pDlgMenu01)
				m_pDlgMenu01->ResetMkInfo();

			m_mgrPunch->SetAlignPosDn();

			if (m_pDlgMenu02)
				m_pDlgMenu02->ChgModelDn();

			if (m_pDlgMenu01)
			{
				m_pDlgMenu01->InitCadImgDn();
				m_pDlgMenu01->InitGL();
				m_bDrawGL = TRUE;
				m_pDlgMenu01->RefreshRmap();
			}
		}
	}
}

void CGvisR2R_PunchView::SetAlignPos()
{
	m_mgrPunch->SetAlignPos();
}
//void CGvisR2R_PunchView::SetAlignPos()
//{
//	if (m_mgrPunch->m_pMotion)
//	{
//		CfPoint ptPos1, ptPos2;
//		m_mgrReelmap->GetAlignPos(ptPos1, ptPos2);
//
//		m_mgrPunch->m_pMotion->m_dAlignPosX[0][0] = ptPos1.x + m_mgrPunch->m_pMotion->m_dPinPosX[0];
//		m_mgrPunch->m_pMotion->m_dAlignPosY[0][0] = ptPos1.y + m_mgrPunch->m_pMotion->m_dPinPosY[0];
//		m_mgrPunch->m_pMotion->m_dAlignPosX[0][1] = ptPos2.x + m_mgrPunch->m_pMotion->m_dPinPosX[0];
//		m_mgrPunch->m_pMotion->m_dAlignPosY[0][1] = ptPos2.y + m_mgrPunch->m_pMotion->m_dPinPosY[0];
//
//		m_mgrPunch->m_pMotion->m_dAlignPosX[1][0] = ptPos1.x + m_mgrPunch->m_pMotion->m_dPinPosX[1];
//		m_mgrPunch->m_pMotion->m_dAlignPosY[1][0] = ptPos1.y + m_mgrPunch->m_pMotion->m_dPinPosY[1];
//		m_mgrPunch->m_pMotion->m_dAlignPosX[1][1] = ptPos2.x + m_mgrPunch->m_pMotion->m_dPinPosX[1];
//		m_mgrPunch->m_pMotion->m_dAlignPosY[1][1] = ptPos2.y + m_mgrPunch->m_pMotion->m_dPinPosY[1];
//	}
//}
//
//void CGvisR2R_PunchView::m_mgrPunch->SetAlignPosUp()
//{
//	if (m_mgrPunch->m_pMotion)
//	{
//		CfPoint ptPos1, ptPos2;
//		m_mgrReelmap->GetAlignPos(ptPos1, ptPos2);
//
//		m_mgrPunch->m_pMotion->m_dAlignPosX[0][0] = ptPos1.x + m_mgrPunch->m_pMotion->m_dPinPosX[0];
//		m_mgrPunch->m_pMotion->m_dAlignPosY[0][0] = ptPos1.y + m_mgrPunch->m_pMotion->m_dPinPosY[0];
//		m_mgrPunch->m_pMotion->m_dAlignPosX[0][1] = ptPos2.x + m_mgrPunch->m_pMotion->m_dPinPosX[0];
//		m_mgrPunch->m_pMotion->m_dAlignPosY[0][1] = ptPos2.y + m_mgrPunch->m_pMotion->m_dPinPosY[0];
//	}
//}
//
//void CGvisR2R_PunchView::m_mgrPunch->SetAlignPosDn()
//{
//	if (m_mgrPunch->m_pMotion)
//	{
//		CfPoint ptPos1, ptPos2;
//		m_mgrReelmap->GetAlignPos(ptPos1, ptPos2);
//
//		m_mgrPunch->m_pMotion->m_dAlignPosX[1][0] = ptPos1.x + m_mgrPunch->m_pMotion->m_dPinPosX[1];
//		m_mgrPunch->m_pMotion->m_dAlignPosY[1][0] = ptPos1.y + m_mgrPunch->m_pMotion->m_dPinPosY[1];
//		m_mgrPunch->m_pMotion->m_dAlignPosX[1][1] = ptPos2.x + m_mgrPunch->m_pMotion->m_dPinPosX[1];
//		m_mgrPunch->m_pMotion->m_dAlignPosY[1][1] = ptPos2.y + m_mgrPunch->m_pMotion->m_dPinPosY[1];
//	}
//}

//BOOL CGvisR2R_PunchView::InitMk()
//{
//	m_mgrProcedure->m_nStepElecChk = 0;
//	int nRSer = ChkSerial(); // (nSerial0 - nLastShot) -> 0: Same Serial, -: Decrese Serial, +: Increase Serial 
//	int nSerial = m_mgrProcedure->GetBuffer0();
//
//	if (nRSer)
//	{
//		m_mgrProcedure->m_bSerialDecrese = FALSE;
//
//		if (pDoc->m_bUseRTRYShiftAdjust)
//		{
//			if (nSerial > 2)
//				AdjPinPos();
//		}
//
//		return TRUE;
//	}
//
//	return FALSE;
//}

int CGvisR2R_PunchView::GetErrCode(int nSerial) // 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.29"));
		return 0;
	}

#ifndef	TEST_MODE
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	int nErr[2];
	nErr[0] = GetErrCodeUp(nSerial);
	if (nErr[0] != 1)
		return nErr[0];
	if (bDualTest)
	{
		nErr[1] = GetErrCodeDn(nSerial);
		if (nErr[1] != 1)
			return nErr[1];
	}
#endif

	return 1;
}

int CGvisR2R_PunchView::GetErrCodeUp(int nSerial) // 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.30"));
		return 0;
	}

	return m_mgrReelmap->GetErrCodeUp(nSerial);
}

int CGvisR2R_PunchView::GetErrCodeDn(int nSerial) // 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return 1;

	return m_mgrReelmap->GetErrCodeDn(nSerial);
}

int CGvisR2R_PunchView::GetTotDefPcs(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.38"));
		return 0;
	}

	return m_mgrReelmap->GetTotDefPcs(nSerial);
}

int CGvisR2R_PunchView::GetTotDefPcsUp(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.39"));
		return 0;
	}

	return m_mgrReelmap->GetTotDefPcsUp(nSerial);
}

int CGvisR2R_PunchView::GetTotDefPcsDn(int nSerial)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return 0;

	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.40"));
		return 0;
	}

	return m_mgrReelmap->GetTotDefPcsDn(nSerial);
}

CfPoint CGvisR2R_PunchView::GetMkPnt(int nMkPcs)
{
	return m_mgrReelmap->GetMkPnt(nMkPcs);
}

CfPoint CGvisR2R_PunchView::GetMkPnt(int nSerial, int nMkPcs) // pcr �ø���, pcr �ҷ� �ǽ� ���� ���� �ε���
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.47"));
		return 0;
	}

	return m_mgrReelmap->GetMkPnt(nSerial, nMkPcs);
}

int CGvisR2R_PunchView::GetMkStripIdx(int nDefPcsId) // 0 : Fail , 1~4 : Strip Idx
{
	return m_mgrReelmap->GetMkStripIdx(nDefPcsId);
}

CString CGvisR2R_PunchView::GetMkInfo(int nSerial, int nMkPcs) // return Cam0 : "Serial_Strip_Col_Row"
{
	if (nSerial <= 0)
	{
		AfxMessageBox(_T("Serial Error.48"));
		return _T("");
	}

	return m_mgrReelmap->GetMkInfo(nSerial, nMkPcs);
}

int CGvisR2R_PunchView::GetMkStripIdx(int nSerial, int nMkPcs) // 0 : Fail , 1~4 : Strip Idx
{
	if (nSerial <= 0)
	{
		AfxMessageBox(_T("Serial Error.48"));
		return 0;
	}

	return m_mgrReelmap->GetMkStripIdx(nSerial, nMkPcs);
}

void CGvisR2R_PunchView::Move0(CfPoint pt, BOOL bCam)
{
	m_mgrPunch->Move0(pt, bCam);
}
//void CGvisR2R_PunchView::Move0(CfPoint pt, BOOL bCam)
//{
//	if (m_mgrProcedure->m_bProbDn[0])
//	{
//		if (m_mgrPunch->m_pVoiceCoil[0])
//		{
//			m_mgrPunch->m_pVoiceCoil[0]->SearchHomeSmac(0);
//			m_mgrPunch->m_pVoiceCoil[0]->MoveSmacShiftPos(0);
//			m_mgrProcedure->m_bProbDn[0] = FALSE;
//			//if (m_pDlgMenu02->m_pDlgUtil06)
//			//	m_pDlgMenu02->m_pDlgUtil06->myBtn[2].SetCheck(FALSE);
//		}
//	}
//
//	double fLen, fVel, fAcc, fJerk;
//
//	double pPos[2];
//
//	if (pDoc->WorkingInfo.System.bNoMk || bCam)
//	{
//		pPos[0] = pt.x;
//		pPos[1] = pt.y;
//	}
//	else
//	{
//		pPos[0] = pt.x + _tstof(pDoc->WorkingInfo.Vision[0].sMkOffsetX);
//		pPos[1] = pt.y + _tstof(pDoc->WorkingInfo.Vision[0].sMkOffsetY);
//	}
//
//	if (pPos[0] < 0.0)
//		pPos[0] = 0.0;
//	if (pPos[1] < 0.0)
//		pPos[1] = 0.0;
//
//	double dCurrX = m_dEnc[AXIS_X0];
//	double dCurrY = m_dEnc[AXIS_Y0];
//	fLen = sqrt(((pPos[0] - dCurrX) * (pPos[0] - dCurrX)) + ((pPos[1] - dCurrY) * (pPos[1] - dCurrY)));
//	if (fLen > 0.001)
//	{
//		m_mgrPunch->m_pMotion->GetSpeedProfile0(TRAPEZOIDAL, AXIS_X0, fLen, fVel, fAcc, fJerk);
//		m_mgrPunch->m_pMotion->Move0(MS_X0Y0, pPos, fVel, fAcc, fAcc, ABS, NO_WAIT);
//	}
//}

void CGvisR2R_PunchView::Move1(CfPoint pt, BOOL bCam)
{
	m_mgrPunch->Move1(pt, bCam);
}
//void CGvisR2R_PunchView::Move1(CfPoint pt, BOOL bCam)
//{
//	if (m_mgrProcedure->m_bProbDn[1])
//	{
//		if (m_mgrPunch->m_pVoiceCoil[1])
//		{
//			m_mgrPunch->m_pVoiceCoil[1]->SearchHomeSmac(1);
//			m_mgrPunch->m_pVoiceCoil[1]->MoveSmacShiftPos(1);
//			m_mgrProcedure->m_bProbDn[1] = FALSE;
//			//if (m_pDlgMenu02->m_pDlgUtil06)
//			//	m_pDlgMenu02->m_pDlgUtil06->myBtn[6].SetCheck(FALSE);
//		}
//	}
//
//	double fLen, fVel, fAcc, fJerk;
//
//	double pPos[2];
//
//	if (pDoc->WorkingInfo.System.bNoMk || bCam)
//	{
//		pPos[0] = pt.x;
//		pPos[1] = pt.y;
//	}
//	else
//	{
//		pPos[0] = pt.x + _tstof(pDoc->WorkingInfo.Vision[1].sMkOffsetX);
//		pPos[1] = pt.y + _tstof(pDoc->WorkingInfo.Vision[1].sMkOffsetY);
//	}
//
//	if (pPos[0] < 0.0)
//		pPos[0] = 0.0;
//	if (pPos[1] < 0.0)
//		pPos[1] = 0.0;
//
//	double dCurrX = m_dEnc[AXIS_X1];
//	double dCurrY = m_dEnc[AXIS_Y1];
//	fLen = sqrt(((pPos[0] - dCurrX) * (pPos[0] - dCurrX)) + ((pPos[1] - dCurrY) * (pPos[1] - dCurrY)));
//	if (fLen > 0.001)
//	{
//		m_mgrPunch->m_pMotion->GetSpeedProfile1(TRAPEZOIDAL, AXIS_X1, fLen, fVel, fAcc, fJerk);
//		m_mgrPunch->m_pMotion->Move1(MS_X1Y1, pPos, fVel, fAcc, fAcc, ABS, NO_WAIT);
//	}
//}

//BOOL CGvisR2R_PunchView::IsMoveDone()
//{
//	if (!m_mgrPunch->m_pMotion)
//		return FALSE;
//
//	if (IsMoveDone0() && IsMoveDone1())
//		return TRUE;
//	return FALSE;
//}
//
//BOOL CGvisR2R_PunchView::IsMoveDone0()
//{
//	if (!m_mgrPunch->m_pMotion)
//		return FALSE;
//
//	if (m_mgrPunch->m_pMotion->IsMotionDone(MS_X0) && m_mgrPunch->m_pMotion->IsMotionDone(MS_Y0))
//	{
//		//	Sleep(50);
//		return TRUE;
//	}
//	return FALSE;
//}
//
//BOOL CGvisR2R_PunchView::IsMoveDone1()
//{
//	if (!m_mgrPunch->m_pMotion)
//		return FALSE;
//
//	if (m_mgrPunch->m_pMotion->IsMotionDone(MS_X1) && m_mgrPunch->m_pMotion->IsMotionDone(MS_Y1))
//	{
//		return TRUE;
//	}
//	return FALSE;
//}

//void CGvisR2R_PunchView::Mk0()
//{
//	if (pView->m_mgrPunch->m_pVoiceCoil[0])
//	{
//		pView->m_mgrPunch->m_pVoiceCoil[0]->SetMark(0);
//		pDoc->AddMkCntL();
//		m_mgrProcedure->m_nCurMk[0]++;
//	}
//}

//void CGvisR2R_PunchView::Mk1()
//{
//	if (pView->m_mgrPunch->m_pVoiceCoil[1])
//	{
//		pView->m_mgrPunch->m_pVoiceCoil[1]->SetMark(1);
//		pDoc->AddMkCntR();
//		m_mgrProcedure->m_nCurMk[1]++;
//	}
//}

//void CGvisR2R_PunchView::Ink(BOOL bOn)
//{
//	if (bOn)
//	{
//		// Ink Marking ON
//		//pDoc->m_pSliceIo[7] |= (0x01<<13);	// ��ŷ�� ��ŷ ��Ʈ�ѷ� ��� 2(��������) -> Sen
//	}
//	else
//	{
//		// Ink Marking OFF
//		//pDoc->m_pSliceIo[7] &= ~(0x01<<13);	// ��ŷ�� ��ŷ ��Ʈ�ѷ� ��� 2(��������) -> Sen
//	}
//}

BOOL CGvisR2R_PunchView::LoadPcrUp(int nSerial, BOOL bFromShare)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.52"));
		return 0;
	}

	int nHeadInfo = m_mgrReelmap->LoadPCR0(nSerial); // 2(Failed), 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
	if (nHeadInfo >= 2)
	{
		MsgBox(_T("Error-LoadPCR0()"));
		return FALSE;
	}
	return TRUE;
}

BOOL CGvisR2R_PunchView::LoadPcrDn(int nSerial, BOOL bFromShare)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return 0;

	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.53"));
		return 0;
	}

	int nHeadInfo = m_mgrReelmap->LoadPCR1(nSerial); // 2(Failed), 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
	if (nHeadInfo >= 2)
	{
		MsgBox(_T("Error-LoadPCR1()"));
		return FALSE;
	}
	return TRUE;
}

void CGvisR2R_PunchView::InitInfo()
{
	if (m_pDlgMenu01)
		m_pDlgMenu01->UpdateData();

	if (m_pDlgMenu05)
	{
		m_pDlgMenu05->InitModel();
		if (m_pDlgMenu05->IsWindowVisible())
			m_pDlgMenu05->AtDlgShow();
	}
}

void CGvisR2R_PunchView::InitReelmap()
{
	m_mgrReelmap->InitReelmap();
	m_mgrReelmap->SetReelmap(ROT_NONE);
	//pDoc->SetReelmap(ROT_CCW_90);
	pDoc->UpdateData();
}

void CGvisR2R_PunchView::InitReelmapUp()
{
	m_mgrReelmap->InitReelmapUp();
	m_mgrReelmap->SetReelmap(ROT_NONE);
	//pDoc->SetReelmap(ROT_CCW_90);
	pDoc->UpdateData();
}

void CGvisR2R_PunchView::InitReelmapDn()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (bDualTest)
	{
		m_mgrReelmap->InitReelmapDn();
		m_mgrReelmap->SetReelmap(ROT_NONE);
		// 	pDoc->SetReelmap(ROT_CCW_90);
		pDoc->UpdateData();
	}
}

BOOL CGvisR2R_PunchView::IsPinMkData()
{
	if (pDoc->IsPinMkData())
	{
		return TRUE;
	}
	return FALSE;
}

//BOOL CGvisR2R_PunchView::IsPinData()
//{
//	return pDoc->IsPinData();
//}

BOOL CGvisR2R_PunchView::CopyDefImg(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.55"));
		return 0;
	}

	return m_mgrReelmap->CopyDefImg(nSerial);
}

BOOL CGvisR2R_PunchView::CopyDefImg(int nSerial, CString sNewLot)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.56"));
		return 0;
	}

	return m_mgrReelmap->CopyDefImg(nSerial, sNewLot);
}

BOOL CGvisR2R_PunchView::CopyDefImgUp(int nSerial, CString sNewLot)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.57"));
		return 0;
	}

	return m_mgrReelmap->CopyDefImgUp(nSerial, sNewLot);
}

BOOL CGvisR2R_PunchView::CopyDefImgDn(int nSerial, CString sNewLot)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return FALSE;

	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.58"));
		return FALSE;
	}

	return m_mgrReelmap->CopyDefImgDn(nSerial, sNewLot);
}

//BOOL CGvisR2R_PunchView::MovePinPos()
//{
//	if (!m_pDlgMenu02)
//		return FALSE;
//
//	return m_pDlgMenu02->MovePinPos();
//}

BOOL CGvisR2R_PunchView::Grab0(int nPos, BOOL bDraw)
{
	return m_mgrPunch->m_pVision[0]->Grab(nPos, bDraw);
}

BOOL CGvisR2R_PunchView::Grab1(int nPos, BOOL bDraw)
{
	return m_mgrPunch->m_pVision[1]->Grab(nPos, bDraw);
}

BOOL CGvisR2R_PunchView::TwoPointAlign0(int nPos, BOOL bDraw)
{
	//return m_mgrPunch->TwoPointAlign0(nPos, bDraw);
	if (!m_pDlgMenu02)
		return FALSE;
	return m_pDlgMenu02->Do2PtAlign0(nPos);
}

BOOL CGvisR2R_PunchView::TwoPointAlign1(int nPos, BOOL bDraw)
{
	//return m_mgrPunch->TwoPointAlign1(nPos, bDraw);
	if (!m_pDlgMenu02)
		return FALSE;
	return m_pDlgMenu02->Do2PtAlign1(nPos);
}

BOOL CGvisR2R_PunchView::FourPointAlign0(int nPos, BOOL bDraw)
{
	if (!m_pDlgMenu02)
		return FALSE;
	return m_pDlgMenu02->Do4PtAlign0(nPos);
}

BOOL CGvisR2R_PunchView::FourPointAlign1(int nPos, BOOL bDraw)
{
	if (!m_pDlgMenu02)
		return FALSE;
	return m_pDlgMenu02->Do4PtAlign1(nPos);
}

BOOL CGvisR2R_PunchView::OnePointAlign(CfPoint &ptPnt)
{
	if (!m_pDlgMenu02)
		return FALSE;
	return m_pDlgMenu02->OnePointAlign(ptPnt);
}

BOOL CGvisR2R_PunchView::TwoPointAlign() // return FALSE; �ΰ�� ����.
{
	if (!m_pDlgMenu02)
		return FALSE;

	BOOL bRtn[2];

	// TwoPointAlign(0)
	bRtn[0] = m_pDlgMenu02->TwoPointAlign(0);

	if (!bRtn[0])
		return FALSE;

	// TwoPointAlign(1)
	bRtn[1] = m_pDlgMenu02->TwoPointAlign(1); // Align ����.

	if (bRtn[0] && bRtn[1])
		return TRUE;

	return FALSE;
}

//BOOL CGvisR2R_PunchView::IsHomeDone(int nMsId)
//{
//	if (!m_mgrPunch->m_pMotion)
//		return FALSE;
//
//	return m_mgrPunch->m_pMotion->IsHomeDone(nMsId);
//}

BOOL CGvisR2R_PunchView::GetAoiUpInfo(int nSerial, int *pNewLot, BOOL bFromBuf)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.59"));
		return 0;
	}

	BOOL bRtn = m_mgrReelmap->GetAoiUpInfo(nSerial, pNewLot, bFromBuf);

	if (pDoc->m_bUpdateForNewJob[0])
	{
		pDoc->m_bUpdateForNewJob[0] = FALSE;

		if (m_pDlgMenu01)
			m_pDlgMenu01->UpdateData();
	}
	return bRtn;
}

BOOL CGvisR2R_PunchView::GetAoiDnInfo(int nSerial, int *pNewLot, BOOL bFromBuf)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.60"));
		return 0;
	}

	return m_mgrReelmap->GetAoiDnInfo(nSerial, pNewLot, bFromBuf);

	if (pDoc->m_bUpdateForNewJob[1])
	{
		pDoc->m_bUpdateForNewJob[1] = FALSE;

		if (m_pDlgMenu01)
			m_pDlgMenu01->UpdateData();
	}

}

BOOL CGvisR2R_PunchView::LoadMySpec()
{
	return pDoc->LoadMySpec();
}

CString CGvisR2R_PunchView::GetProcessNum()
{
	return pDoc->GetProcessNum();
}

BOOL CGvisR2R_PunchView::GetAoiUpOffset(CfPoint &OfSt)
{
	return pDoc->GetAoiUpOffset(OfSt);
}

BOOL CGvisR2R_PunchView::GetAoiDnOffset(CfPoint &OfSt)
{
	return pDoc->GetAoiDnOffset(OfSt);
}

BOOL CGvisR2R_PunchView::GetMkOffset(CfPoint &OfSt)
{
	if (m_pDlgMenu02)
	{
		OfSt.x = m_dMkFdOffsetX[0][0]; // -: ��ǰ ����, +: ��ǰ ����.
		OfSt.y = m_dMkFdOffsetY[0][0]; // -: ��ǰ ����, +: ��ǰ ����.
	}

	return TRUE;
}

BOOL CGvisR2R_PunchView::IsAoiLdRun()
{
	return TRUE;

	BOOL bRtn = FALSE;
	if (m_pDlgMenu03)
		bRtn = m_pDlgMenu03->IsAoiLdRun();

	return bRtn;
}

//BOOL CGvisR2R_PunchView::IsInitPos0()
//{
//	if (!m_mgrPunch->m_pMotion)
//		return FALSE;
//
//	double pTgtPos[2];
//	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[0]);
//	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[0]);
//	double dCurrX = m_dEnc[AXIS_X0];
//	double dCurrY = m_dEnc[AXIS_Y0];
//
//	if (dCurrX < pTgtPos[0] - 2.0 || dCurrX > pTgtPos[0] + 2.0)
//		return FALSE;
//	if (dCurrY < pTgtPos[1] - 2.0 || dCurrY > pTgtPos[1] + 2.0)
//		return FALSE;
//
//	return TRUE;
//}

//void CGvisR2R_PunchView::m_mgrPunch->MoveInitPos0(BOOL bWait)
//{
//	if (!m_mgrPunch->m_pMotion)
//		return;
//
//	double pTgtPos[2];
//	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[0]);
//	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[0]);
//	double dCurrX = m_dEnc[AXIS_X0];
//	double dCurrY = m_dEnc[AXIS_Y0];
//
//	double fLen, fVel, fAcc, fJerk;
//	fLen = sqrt(((pTgtPos[0] - dCurrX) * (pTgtPos[0] - dCurrX)) + ((pTgtPos[1] - dCurrY) * (pTgtPos[1] - dCurrY)));
//	if (fLen > 0.001)
//	{
//		m_mgrPunch->m_pMotion->GetSpeedProfile0(TRAPEZOIDAL, AXIS_X0, fLen, fVel, fAcc, fJerk);
//		if(bWait)
//			m_mgrPunch->m_pMotion->Move0(MS_X0Y0, pTgtPos, fVel, fAcc, fAcc, ABS, WAIT);
//		else
//			m_mgrPunch->m_pMotion->Move0(MS_X0Y0, pTgtPos, fVel, fAcc, fAcc, ABS, NO_WAIT);
//	}
//}

//BOOL CGvisR2R_PunchView::IsInitPos1()
//{
//	if (!m_mgrPunch->m_pMotion)
//		return FALSE;
//
//	double pTgtPos[2];
//	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[1]);
//	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[1]);
//	double dCurrX = m_dEnc[AXIS_X1];
//	double dCurrY = m_dEnc[AXIS_Y1];
//
//	if (dCurrX < pTgtPos[0] - 2.0 || dCurrX > pTgtPos[0] + 2.0)
//		return FALSE;
//	if (dCurrY < pTgtPos[1] - 2.0 || dCurrY > pTgtPos[1] + 2.0)
//		return FALSE;
//
//	return TRUE;
//}

//BOOL CGvisR2R_PunchView::IsMkEdPos1()
//{
//	if (!m_mgrPunch->m_pMotion)
//		return FALSE;
//
//	double pTgtPos[2];
//	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sMkEdPosX[1]);
//	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sMkEdPosY[1]);
//	double dCurrX = m_dEnc[AXIS_X1];
//	double dCurrY = m_dEnc[AXIS_Y1];
//
//	if (dCurrX < pTgtPos[0] - 2.0 || dCurrX > pTgtPos[0] + 2.0)
//		return FALSE;
//	if (dCurrY < pTgtPos[1] - 2.0 || dCurrY > pTgtPos[1] + 2.0)
//		return FALSE;
//
//	return TRUE;
//}

//void CGvisR2R_PunchView::m_mgrPunch->MoveInitPos1(BOOL bWait)
//{
//	if (!m_mgrPunch->m_pMotion)
//		return;
//
//	double pTgtPos[2];
//	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[1]);
//	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[1]);
//	double dCurrX = m_dEnc[AXIS_X1];
//	double dCurrY = m_dEnc[AXIS_Y1];
//
//	double fLen, fVel, fAcc, fJerk;
//	fLen = sqrt(((pTgtPos[0] - dCurrX) * (pTgtPos[0] - dCurrX)) + ((pTgtPos[1] - dCurrY) * (pTgtPos[1] - dCurrY)));
//	if (fLen > 0.001)
//	{
//		m_mgrPunch->m_pMotion->GetSpeedProfile1(TRAPEZOIDAL, AXIS_X1, fLen, fVel, fAcc, fJerk);
//		if(bWait)
//			m_mgrPunch->m_pMotion->Move1(MS_X1Y1, pTgtPos, fVel, fAcc, fAcc, ABS, WAIT);
//		else
//			m_mgrPunch->m_pMotion->Move1(MS_X1Y1, pTgtPos, fVel, fAcc, fAcc, ABS, NO_WAIT);
//	}
//}

//void CGvisR2R_PunchView::m_mgrPunch->MoveMkEdPos1()
//{
//	if (!m_mgrPunch->m_pMotion)
//		return;
//
//	double pTgtPos[2];
//	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sMkEdPosX[1]);
//	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sMkEdPosY[1]);
//	double dCurrX = m_dEnc[AXIS_X1];
//	double dCurrY = m_dEnc[AXIS_Y1];
//
//	double fLen, fVel, fAcc, fJerk;
//	fLen = sqrt(((pTgtPos[0] - dCurrX) * (pTgtPos[0] - dCurrX)) + ((pTgtPos[1] - dCurrY) * (pTgtPos[1] - dCurrY)));
//	if (fLen > 0.001)
//	{
//		m_mgrPunch->m_pMotion->GetSpeedProfile1(TRAPEZOIDAL, AXIS_X1, fLen, fVel, fAcc, fJerk);
//		m_mgrPunch->m_pMotion->Move1(MS_X1Y1, pTgtPos, fVel, fAcc, fAcc, ABS, NO_WAIT);
//	}
//}

void CGvisR2R_PunchView::LotEnd()
{
	if (m_pDlgMenu01)
		m_pDlgMenu01->LotEnd();
	if (m_pDlgMenu03)
		m_pDlgMenu03->SwAoiLotEnd(TRUE);

	m_mgrProcedure->m_bCont = FALSE;
	SetLotEd();

	MakeResultMDS();

	if (pDoc->GetTestMode() == MODE_INNER || pDoc->GetTestMode() == MODE_OUTER)
	{
		FinalCopyItsFiles();
	}

	m_mgrReelmap->DuplicateRmap();

	//if (m_mgrPunch->m_pEngrave)
	//	m_mgrPunch->m_pEngrave->SwMenu01DispDefImg(TRUE);
	SwMenu01DispDefImg(TRUE);
}

//void CGvisR2R_PunchView::Winker(int nId, int nDly) // 0:Ready, 1:Reset, 2:Run, 3:Stop
//{
//#ifdef USE_MPE
//	if (nId == MN_RUN)
//	{
//		if (pView->m_pMpe)
//		{
//			m_bBtnWinker[nId] = TRUE;
//			MpeWrite(_T("MB44015D"), 1); // �ڵ� �ʱ� ��������(PC�� On/Off ��Ŵ, PLC�� ���������� ��ũ����, on->off�� �������� on, �ٽ� ��������ġ�� �������� off) - 20141017
//		}
//	}
//#endif
//}

void CGvisR2R_PunchView::ResetWinker() // 0:Ready, 1:Reset, 2:Run, 3:Stop
{
#ifdef USE_MPE
	MpeWrite(_T("MB44015D"), 0);
#endif
	for (int i = 0; i<4; i++)
	{
		m_bBtnWinker[i] = FALSE;
		//m_nCntBtnWinker[i] = FALSE;
	}
}

void CGvisR2R_PunchView::SetLotEnd(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.61"));
		return;
	}
	m_mgrProcedure->m_nLotEndSerial = nSerial;

	CString str;
	str.Format(_T("%d"), m_mgrProcedure->m_nLotEndSerial);
	DispStsBar(str, 0);
	if (m_pDlgMenu01)
		m_pDlgMenu01->DispLotEndSerial(m_mgrProcedure->m_nLotEndSerial);
}

int CGvisR2R_PunchView::GetLotEndSerial()
{
	return m_mgrProcedure->m_nLotEndSerial; // ���̺�� �����ϴ� Serial.
}

//BOOL CGvisR2R_PunchView::m_mgrPunch->StartLive()
//{
//	if (m_mgrPunch->StartLive0() && m_mgrPunch->StartLive1())
//		return TRUE;
//
//	return  FALSE;
//}
//
//BOOL CGvisR2R_PunchView::m_mgrPunch->StartLive0()
//{
//	BOOL bRtn0 = FALSE;
//
//#ifdef USE_VISION
//	if (m_mgrPunch->m_pVision[0])
//		bRtn0 = m_mgrPunch->m_pVision[0]->m_mgrPunch->StartLive();
//#endif
//	if (bRtn0)
//		return TRUE;
//
//	return  FALSE;
//}
//
//BOOL CGvisR2R_PunchView::m_mgrPunch->StartLive1()
//{
//	BOOL bRtn1 = FALSE;
//
//#ifdef USE_VISION
//	if (m_mgrPunch->m_pVision[1])
//		bRtn1 = m_mgrPunch->m_pVision[1]->m_mgrPunch->StartLive();
//#endif
//
//	if (bRtn1)
//		return TRUE;
//
//	return  FALSE;
//}
//
//BOOL CGvisR2R_PunchView::StopLive()
//{
//#ifdef TEST_MODE
//	return TRUE;
//#endif
//
//	if (StopLive0() && StopLive1())
//		return TRUE;
//
//	return FALSE;
//}
//
//BOOL CGvisR2R_PunchView::StopLive0()
//{
//#ifdef TEST_MODE
//	return TRUE;
//#endif
//
//	BOOL bRtn0 = FALSE;
//
//#ifdef USE_VISION
//	if (m_mgrPunch->m_pVision[0])
//		bRtn0 = m_mgrPunch->m_pVision[0]->StopLive();
//#endif
//	if (bRtn0)
//		return TRUE;
//
//	return FALSE;
//}
//
//BOOL CGvisR2R_PunchView::StopLive1()
//{
//#ifdef TEST_MODE
//	return TRUE;
//#endif
//
//	BOOL bRtn1 = FALSE;
//
//#ifdef USE_VISION
//	if (m_mgrPunch->m_pVision[1])
//		bRtn1 = m_mgrPunch->m_pVision[1]->StopLive();
//#endif
//
//	if (bRtn1)
//		return TRUE;
//
//	return FALSE;
//}

void CGvisR2R_PunchView::UpdateRst()
{
	if (m_pDlgMenu01)
		m_pDlgMenu01->UpdateRst();
}

BOOL CGvisR2R_PunchView::IsChkTmpStop()
{
	if (pDoc->WorkingInfo.LastJob.bTempPause)
	{
		double dFdLen = GetMkFdLen();
		if (dFdLen >= _tstof(pDoc->WorkingInfo.LastJob.sTempPauseLen)*1000.0)
		{
			pDoc->WorkingInfo.LastJob.bTempPause = FALSE;
			if (m_pDlgMenu01)
				m_pDlgMenu01->UpdateData();
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CGvisR2R_PunchView::IsReview()
{
	return (pDoc->WorkingInfo.LastJob.bReview);
}

BOOL CGvisR2R_PunchView::IsJogRtDn()
{
	BOOL bOn = FALSE;
#ifdef USE_MPE
	bOn = pDoc->m_pMpeIb[4] & (0x01 << 10) ? TRUE : FALSE;	// ��ŷ�� JOG ��ư(��)
#endif
	return bOn;
}

BOOL CGvisR2R_PunchView::IsJogRtUp()
{
	BOOL bOn = FALSE;
#ifdef USE_MPE
	bOn = pDoc->m_pMpeIb[4] & (0x01 << 10) ? FALSE : TRUE;	// ��ŷ�� JOG ��ư(��)
#endif
	return bOn;
}

BOOL CGvisR2R_PunchView::IsJogRtDn0()
{
	BOOL bOn = FALSE;
#ifdef USE_MPE
	bOn = pDoc->m_pMpeIb[4] & (0x01 << 10) ? TRUE : FALSE;	// ��ŷ�� JOG ��ư(��)
#endif
	return bOn;
}

BOOL CGvisR2R_PunchView::IsJogRtUp0()
{
	BOOL bOn = FALSE;
#ifdef USE_MPE
	bOn = pDoc->m_pMpeIb[4] & (0x01 << 10) ? FALSE : TRUE;	// ��ŷ�� JOG ��ư(��)
#endif
	return bOn;
}

BOOL CGvisR2R_PunchView::IsJogRtDn1()
{
	BOOL bOn = FALSE;
#ifdef USE_MPE
	bOn = pDoc->m_pMpeIb[4] & (0x01 << 10) ? TRUE : FALSE;	// ��ŷ�� JOG ��ư(��)
#endif
	return bOn;
}

BOOL CGvisR2R_PunchView::IsJogRtUp1()
{
	BOOL bOn = FALSE;
#ifdef USE_MPE
	bOn = pDoc->m_pMpeIo[4] & (0x01 << 10) ? FALSE : TRUE;	// ��ŷ�� JOG ��ư(��)
#endif
	return bOn;
}

void CGvisR2R_PunchView::SwAoiEmg(BOOL bOn)
{
	if (m_pDlgMenu03)
		m_pDlgMenu03->SwAoiEmg(bOn);

	if (bOn)
	{
		// 		pDoc->m_pMpeIo[8] |= (0x01<<0);		// �����Ϸ� ������� ����ġ ����
		// 		pDoc->m_pMpeIo[12] |= (0x01<<0);	// �����Ϸ� ������� ����ġ ����

		// 		pDoc->m_pSliceIo[9] |= (0x01<<14);	// �˻�� ������� ����ġ(����ġ)
		// 		pDoc->m_pSliceIo[9] |= (0x01<<15);	// �˻�� ������� ����ġ(�ĸ�)
		// 		pDoc->m_pSliceIo[6] |= (0x01<<0);	// ��ŷ�� ������� ����ġ(�����)
		// 		pDoc->m_pSliceIo[7] |= (0x01<<0);	// ��ŷ�� ������� ����ġ(����ġ)
	}
	else
	{
		// 		pDoc->m_pMpeIo[8] &= ~(0x01<<0);	// �����Ϸ� ������� ����ġ ����
		// 		pDoc->m_pMpeIo[12] &= ~(0x01<<0);	// �����Ϸ� ������� ����ġ ����

		// 		pDoc->m_pSliceIo[9] &= ~(0x01<<14);	// �˻�� ������� ����ġ(����ġ)
		// 		pDoc->m_pSliceIo[9] &= ~(0x01<<15);	// �˻�� ������� ����ġ(�ĸ�)
		// 		pDoc->m_pSliceIo[6] &= ~(0x01<<0);	// ��ŷ�� ������� ����ġ(�����)
		// 		pDoc->m_pSliceIo[7] &= ~(0x01<<0);	// ��ŷ�� ������� ����ġ(����ġ)
	}

}

BOOL CGvisR2R_PunchView::IsDoneDispMkInfo()
{
	BOOL bRtn = FALSE;
	if (m_pDlgMenu01)
		bRtn = m_pDlgMenu01->IsDoneDispMkInfo();
	return bRtn;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void CGvisR2R_PunchView::DoReject0()
{
	m_mgrPunch->DoReject0();
}

void CGvisR2R_PunchView::DoReject1()
{
	m_mgrPunch->DoReject1();
}

//void CGvisR2R_PunchView::DoReject0()
//{
//#ifdef TEST_MODE
//	return;
//#endif
//
//	//BOOL bOn;
//	int nIdx, nRtn;//nSerial, 
//	CfPoint ptPnt;
//
//	if (!IsRun() && m_mgrPunch->m_bAuto)
//	{
//		if (IsOnMarking0())
//		{
//			if (m_mgrPunch->m_pMotion->IsEnable(MS_X0) && m_mgrPunch->m_pMotion->IsEnable(MS_Y0))
//			{
//				if (m_mgrPunch->m_pMotion->IsMotionDone(MS_X0) && m_mgrPunch->m_pMotion->IsMotionDone(MS_Y0))
//				{
//					if (!IsInitPos0() && !IsPinPos0())
//						m_mgrPunch->MoveInitPos0();
//				}
//			}
//		}
//		return;
//	}
//
//
//	switch (m_mgrProcedure->m_nStepMk[2])
//	{
//	case 0:
//		if (m_mgrProcedure->IsNoMk())
//			ShowLive();
//		m_mgrProcedure->m_nMkStrip[0][0] = 0;
//		m_mgrProcedure->m_nMkStrip[0][1] = 0;
//		m_mgrProcedure->m_nMkStrip[0][2] = 0;
//		m_mgrProcedure->m_nMkStrip[0][3] = 0;
//		m_mgrProcedure->m_bRejectDone[0][0] = FALSE;
//		m_mgrProcedure->m_bRejectDone[0][1] = FALSE;
//		m_mgrProcedure->m_bRejectDone[0][2] = FALSE;
//		m_mgrProcedure->m_bRejectDone[0][3] = FALSE;
//		m_mgrProcedure->m_nStepMk[2]++;
//		break;
//	case 1:
//		if (m_mgrProcedure->m_nMkPcs[2] < m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)
//		{
//			m_mgrProcedure->m_nStepMk[2]++;
//		}
//		else
//		{
//			m_mgrProcedure->m_nStepMk[2] = MK_END;
//		}
//		break;
//	case 2:
//		m_mgrProcedure->m_nStepMk[2]++;
//		break;
//	case 3:
//		if (m_mgrProcedure->m_nMkPcs[2] < m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)	// ��ŷ�Ϸ�Check
//		{
//			ptPnt = GetMkPnt(m_mgrProcedure->m_nMkPcs[2]);
//
//			if (ptPnt.x < 0.0 && ptPnt.y < 0.0) // ��ǰȭ.
//			{
//				m_mgrProcedure->m_nMkPcs[2]++;
//				break;
//			}
//
//			nIdx = GetMkStripIdx(m_mgrProcedure->m_nMkPcs[2]);
//			if (nIdx > 0)
//			{
//				if (!IsMkStrip(nIdx)) // Strip[] Mk Off
//				{
//					m_mgrProcedure->m_nMkPcs[2]++;
//					break;
//				}
//				else
//				{
//					if (m_mgrProcedure->m_nMkStrip[0][nIdx - 1] > pDoc->GetStripRejectMkNum())
//					{
//						m_mgrProcedure->m_bRejectDone[0][nIdx - 1] = TRUE;
//						m_mgrProcedure->m_nMkPcs[2]++;
//						break;
//					}
//					else
//						m_mgrProcedure->m_nMkStrip[0][nIdx - 1]++;
//				}
//			}
//			else
//			{
//				pView->ClrDispMsg();
//				AfxMessageBox(_T("Strip Index Failed."));
//				break;
//			}
//
//			if (m_mgrProcedure->m_bRejectDone[0][0] && m_mgrProcedure->m_bRejectDone[0][1] &&
//				m_mgrProcedure->m_bRejectDone[0][2] && m_mgrProcedure->m_bRejectDone[0][3])
//			{
//				m_mgrProcedure->m_nStepMk[2] = MK_END;
//				break;
//			}
//
//			m_mgrProcedure->m_dTarget[AXIS_X0] = ptPnt.x;
//
//			// Cam0 : m_bPriority[0], m_bPriority[3]
//			// Cam1 : m_bPriority[1], m_bPriority[2]
//
//			m_mgrProcedure->m_bCollision[0] = m_mgrProcedure->ChkCollision(AXIS_X0, ptPnt.x);
//			if (!m_mgrProcedure->m_bCollision[0])
//			{
//				Move0(ptPnt, m_mgrProcedure->m_bCam);
//				m_mgrProcedure->m_nStepMk[2]++;
//			}
//			else if (m_mgrProcedure->m_bPriority[0])
//			{
//				m_mgrProcedure->m_bPriority[0] = FALSE;
//				Move0(ptPnt, m_mgrProcedure->m_bCam);
//				m_mgrProcedure->m_nStepMk[2]++;
//			}
//			else if (m_mgrProcedure->m_bPriority[2])
//			{
//				m_mgrProcedure->m_bPriority[2] = FALSE;
//				ptPnt.x = 0.0;
//				ptPnt.y = 0.0;//m_dEnc[AXIS_Y0];
//				Move0(ptPnt, m_mgrProcedure->m_bCam);
//			}
//		}
//		else
//		{
//			m_mgrProcedure->m_nStepMk[2] = MK_END;
//		}
//		break;
//	case 4:
//		m_mgrProcedure->m_nStepMk[2]++;
//		break;
//	case 5:
//		m_mgrProcedure->m_nStepMk[2]++;
//		break;
//	case 6:
//		if (IsMoveDone0())
//			m_mgrProcedure->m_nStepMk[2]++;
//		break;
//	case 7:
//		if (!m_mgrProcedure->IsNoMk0())
//		{
//			m_mgrProcedure->m_dwStMkDn[0] = GetTickCount();
//			Mk0();
//		}
//		else
//			SetDelay0(pDoc->m_nDelayShow, 1);		// [mSec]
//		m_mgrProcedure->m_nStepMk[2]++;
//		break;
//	case 8:
//		if (m_mgrProcedure->IsNoMk0())
//		{
//			if (!WaitDelay0(1))		// F:Done, T:On Waiting....
//				m_mgrProcedure->m_nStepMk[2]++;
//		}
//		else
//			m_mgrProcedure->m_nStepMk[2]++;
//		break;
//	case 9:
//		m_mgrProcedure->m_nStepMk[2]++;
//		break;
//	case 10:
//		if (!m_mgrProcedure->IsNoMk0())
//		{
//			//Mk0(FALSE);
//			if (IsMk0Done())
//			{
//				m_mgrProcedure->m_nMkPcs[2]++;
//				m_mgrProcedure->m_nStepMk[2]++;
//				m_mgrProcedure->m_nStepMk[2]++;
//			}
//			else
//			{
//				if (m_mgrProcedure->m_dwStMkDn[0] + 5000 < GetTickCount())
//				{
//					Buzzer(TRUE, 0);
//					//pView->DispStsBar(_T("����-29"), 0);
//					DispMain(_T("�� ��"), RGB_RED);
//					m_mgrPunch->m_pVoiceCoil[0]->SearchHomeSmac0();
//
//					nRtn = AsyncMsgBox(_T("���̽�����(��) ��ſϷᰡ �ʵ˴ϴ�.\r\n��ŷ�� �ٽ� �õ��Ͻðڽ��ϱ�?"), 1, MB_YESNO);
//					if (IDYES == nRtn)
//					{
//						DispMain(_T("������"), RGB_RED);
//						m_mgrProcedure->m_nStepMk[2] = 7;
//					}
//					else if (nRtn < 0)
//						m_mgrProcedure->m_nStepMk[2]++;
//					else
//					{
//						m_mgrProcedure->m_nMkPcs[2]++;
//						m_mgrProcedure->m_nStepMk[2]++;
//						m_mgrProcedure->m_nStepMk[2]++;
//						Stop();
//					}
//				}
//			}
//		}
//		else
//		{
//			m_mgrProcedure->m_nMkPcs[2]++;
//			m_mgrProcedure->m_nStepMk[2]++;
//			m_mgrProcedure->m_nStepMk[2]++;
//		}
//		break;
//	case 11:
//		//m_nStepMk[2]++;
//		//if(m_bRtnMyMsgBox[2])
//		if ((nRtn = WaitRtnVal(3)) > -1)
//		{
//			//if(IDYES == m_nRtnMyMsgBox[2])
//			if (IDYES == nRtn)
//			{
//				DispMain(_T("������"), RGB_RED);
//				m_mgrProcedure->m_nStepMk[2] = 7;
//			}
//			else
//			{
//				m_mgrProcedure->m_nMkPcs[2]++;
//				m_mgrProcedure->m_nStepMk[2]++;
//				Stop();
//			}
//		}
//		break;
//	case 12:
//		m_mgrProcedure->m_nStepMk[2]++;
//		break;
//	case 13:
//		if (m_mgrProcedure->m_nMkPcs[2] < m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)	// ��ŷ�Ϸ�Check
//		{
//			m_mgrProcedure->m_nStepMk[2] = 3;
//		}
//		else
//		{
//			m_mgrProcedure->m_nStepMk[2] = MK_END;
//		}
//		break;
//	case MK_END:
//		m_mgrPunch->m_pVoiceCoil[0]->SearchHomeSmac0();
//		SetDelay0(500, 1);		// [mSec]
//		m_mgrProcedure->m_nStepMk[2]++;
//		break;
//	case 101:
//		if (!WaitDelay0(1))		// F:Done, T:On Waiting....
//		{
//			m_mgrPunch->MoveInitPos0();
//			m_mgrProcedure->m_nStepMk[2]++;
//		}
//		break;
//	case 102:
//		if (IsMoveDone0())
//		{
//			m_mgrProcedure->m_bTHREAD_MK[2] = FALSE;
//			m_mgrProcedure->m_bDoneMk[0] = TRUE;
//			m_mgrProcedure->m_nStepMk[2] = 0;
//			m_mgrProcedure->m_nMkStrip[0][0] = 0;
//			m_mgrProcedure->m_nMkStrip[0][1] = 0;
//			m_mgrProcedure->m_nMkStrip[0][2] = 0;
//			m_mgrProcedure->m_nMkStrip[0][3] = 0;
//		}
//		break;
//	}
//}
//
//void CGvisR2R_PunchView::DoReject1()
//{
//#ifdef TEST_MODE
//	return;
//#endif
//
//	//BOOL bOn;
//	int nIdx, nRtn;//nSerial, 
//	CfPoint ptPnt;
//
//	if (!IsRun() && m_mgrPunch->m_bAuto)
//	{
//		if (IsOnMarking1())
//		{
//			if (m_mgrPunch->m_pMotion->IsEnable(MS_X1) && m_mgrPunch->m_pMotion->IsEnable(MS_Y1))
//			{
//				if (m_mgrPunch->m_pMotion->IsMotionDone(MS_X1) && m_mgrPunch->m_pMotion->IsMotionDone(MS_Y1))
//				{
//					if (!IsMkEdPos1() && !IsPinPos1())
//						m_mgrPunch->MoveMkEdPos1();
//				}
//			}
//		}
//		return;
//	}
//
//
//	switch (m_mgrProcedure->m_nStepMk[3])
//	{
//	case 0:
//		if (m_mgrProcedure->IsNoMk())
//			ShowLive();
//		m_mgrProcedure->m_nMkStrip[1][0] = 0;
//		m_mgrProcedure->m_nMkStrip[1][1] = 0;
//		m_mgrProcedure->m_nMkStrip[1][2] = 0;
//		m_mgrProcedure->m_nMkStrip[1][3] = 0;
//		m_mgrProcedure->m_bRejectDone[1][0] = FALSE;
//		m_mgrProcedure->m_bRejectDone[1][1] = FALSE;
//		m_mgrProcedure->m_bRejectDone[1][2] = FALSE;
//		m_mgrProcedure->m_bRejectDone[1][3] = FALSE;
//		m_mgrProcedure->m_nStepMk[3]++;
//		break;
//	case 1:
//		if (m_mgrProcedure->m_nMkPcs[3] < m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)
//		{
//			m_mgrProcedure->m_nStepMk[3]++;
//		}
//		else
//		{
//			m_mgrProcedure->m_nStepMk[3] = MK_END;
//		}
//		break;
//	case 2:
//		m_mgrProcedure->m_nStepMk[3]++;
//		break;
//	case 3:
//		if (m_mgrProcedure->m_nMkPcs[3] < m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)	// ��ŷ�Ϸ�Check
//		{
//			ptPnt = GetMkPnt(m_mgrProcedure->m_nMkPcs[3]);
//
//			if (ptPnt.x < 0.0 && ptPnt.y < 0.0) // ��ǰȭ.
//			{
//				m_mgrProcedure->m_nMkPcs[3]++;
//				break;
//			}
//
//			nIdx = GetMkStripIdx(m_mgrProcedure->m_nMkPcs[3]);
//			if (nIdx > 0)
//			{
//				if (!IsMkStrip(nIdx)) // Strip[] Mk Off
//				{
//					m_mgrProcedure->m_nMkPcs[3]++;
//					break;
//				}
//				else
//				{
//					if (m_mgrProcedure->m_nMkStrip[1][nIdx - 1] > pDoc->GetStripRejectMkNum())
//					{
//						m_mgrProcedure->m_bRejectDone[1][nIdx - 1] = TRUE;
//						m_mgrProcedure->m_nMkPcs[3]++;
//						break;
//					}
//					else
//						m_mgrProcedure->m_nMkStrip[1][nIdx - 1]++;
//				}
//			}
//			else
//			{
//				pView->ClrDispMsg();
//				AfxMessageBox(_T("Strip Index Failed."));
//				break;
//			}
//
//			if (m_mgrProcedure->m_bRejectDone[1][0] && m_mgrProcedure->m_bRejectDone[1][1] &&
//				m_mgrProcedure->m_bRejectDone[1][2] && m_mgrProcedure->m_bRejectDone[1][3])
//			{
//				m_mgrProcedure->m_nStepMk[2] = MK_END;
//				break;
//			}
//
//			m_mgrProcedure->m_dTarget[AXIS_X1] = ptPnt.x;
//
//			// Cam0 : m_bPriority[0], m_bPriority[3]
//			// Cam1 : m_bPriority[1], m_bPriority[2]
//
//			m_mgrProcedure->m_bCollision[1] = m_mgrProcedure->ChkCollision(AXIS_X1, ptPnt.x);
//			if (!m_mgrProcedure->m_bCollision[1])
//			{
//				Move1(ptPnt, m_mgrProcedure->m_bCam);
//				m_mgrProcedure->m_nStepMk[3]++;
//			}
//			else if (m_mgrProcedure->m_bPriority[1])
//			{
//				m_mgrProcedure->m_bPriority[1] = FALSE;
//				Move1(ptPnt, m_mgrProcedure->m_bCam);
//				m_mgrProcedure->m_nStepMk[3]++;
//			}
//			else if (m_mgrProcedure->m_bPriority[3])
//			{
//				m_mgrProcedure->m_bPriority[3] = FALSE;
//				ptPnt.x = _tstof(pDoc->WorkingInfo.Motion.sSafeZone);
//				ptPnt.y = 0.0;//m_dEnc[AXIS_Y1];
//				Move1(ptPnt, m_mgrProcedure->m_bCam);
//			}
//		}
//		else
//		{
//			m_mgrProcedure->m_nStepMk[3] = MK_END;
//		}
//		break;
//	case 4:
//		m_mgrProcedure->m_nStepMk[3]++;
//		break;
//	case 5:
//		m_mgrProcedure->m_nStepMk[3]++;
//		break;
//	case 6:
//		if (IsMoveDone1())
//			m_mgrProcedure->m_nStepMk[3]++;
//		break;
//	case 7:
//		if (!m_mgrProcedure->IsNoMk1())
//		{
//			m_mgrProcedure->m_dwStMkDn[1] = GetTickCount();
//			Mk1();
//		}
//		else
//			SetDelay1(pDoc->m_nDelayShow, 1);		// [mSec]
//		m_mgrProcedure->m_nStepMk[3]++;
//		break;
//	case 8:
//		if (m_mgrProcedure->IsNoMk1())
//		{
//			if (!WaitDelay1(1))		// F:Done, T:On Waiting....
//				m_mgrProcedure->m_nStepMk[3]++;
//		}
//		else
//			m_mgrProcedure->m_nStepMk[3]++;
//		break;
//	case 9:
//		m_mgrProcedure->m_nStepMk[3]++;
//		break;
//	case 10:
//		if (!m_mgrProcedure->IsNoMk1())
//		{
//			if (IsMk1Done())
//			{
//				m_mgrProcedure->m_nMkPcs[3]++;
//				m_mgrProcedure->m_nStepMk[3]++;
//				m_mgrProcedure->m_nStepMk[3]++;
//			}
//			else
//			{
//				if (m_mgrProcedure->m_dwStMkDn[1] + 5000 < GetTickCount())
//				{
//					Buzzer(TRUE, 0);
//					//pView->DispStsBar(_T("����-30"), 0);
//					DispMain(_T("�� ��"), RGB_RED);
//					m_mgrPunch->m_pVoiceCoil[1]->SearchHomeSmac1();
//
//					nRtn = AsyncMsgBox(_T("���̽�����(��) ��ſϷᰡ �ʵ˴ϴ�.\r\n��ŷ�� �ٽ� �õ��Ͻðڽ��ϱ�?"), 2, MB_YESNO);
//					if (IDYES == nRtn)
//					{
//						DispMain(_T("������"), RGB_RED);
//						m_mgrProcedure->m_nStepMk[3] = 7;
//					}
//					else if (nRtn < 0)
//						m_mgrProcedure->m_nStepMk[3]++;
//					else
//					{
//						m_mgrProcedure->m_nMkPcs[3]++;
//						m_mgrProcedure->m_nStepMk[3]++;
//						m_mgrProcedure->m_nStepMk[3]++;
//						Stop();
//					}
//				}
//			}
//		}
//		else
//		{
//			m_mgrProcedure->m_nMkPcs[3]++;
//			m_mgrProcedure->m_nStepMk[3]++;
//			m_mgrProcedure->m_nStepMk[3]++;
//		}
//		break;
//	case 11:
//		//m_nStepMk[3]++;
//		//if(m_bRtnMyMsgBox[3])
//		if ((nRtn = WaitRtnVal(4)) > -1)
//		{
//			//if(IDYES == m_nRtnMyMsgBox[3])
//			if (IDYES == nRtn)
//			{
//				DispMain(_T("������"), RGB_RED);
//				m_mgrProcedure->m_nStepMk[3] = 7;
//			}
//			else
//			{
//				m_mgrProcedure->m_nMkPcs[3]++;
//				m_mgrProcedure->m_nStepMk[3]++;
//				Stop();
//			}
//		}
//		break;
//	case 12:
//		m_mgrProcedure->m_nStepMk[3]++;
//		break;
//	case 13:
//		if (m_mgrProcedure->m_nMkPcs[3] < m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)	// ��ŷ�Ϸ�Check
//		{
//			m_mgrProcedure->m_nStepMk[3] = 3;
//		}
//		else
//		{
//			m_mgrProcedure->m_nStepMk[3] = MK_END;
//		}
//		break;
//	case MK_END:
//		m_mgrPunch->m_pVoiceCoil[1]->SearchHomeSmac1();
//		SetDelay1(500, 1);		// [mSec]
//		m_mgrProcedure->m_nStepMk[3]++;
//		break;
//	case 101:
//		if (!WaitDelay1(1))		// F:Done, T:On Waiting....
//		{
//			m_mgrPunch->MoveMkEdPos1();
//			m_mgrProcedure->m_nStepMk[3]++;
//		}
//		break;
//	case 102:
//		if (IsMoveDone1())
//		{
//			m_mgrProcedure->m_bTHREAD_MK[3] = FALSE;
//			m_mgrProcedure->m_bDoneMk[1] = TRUE;
//			m_mgrProcedure->m_nStepMk[3] = 0;
//			m_mgrProcedure->m_nMkStrip[1][0] = 0;
//			m_mgrProcedure->m_nMkStrip[1][1] = 0;
//			m_mgrProcedure->m_nMkStrip[1][2] = 0;
//			m_mgrProcedure->m_nMkStrip[1][3] = 0;
//		}
//		break;
//	}
//}

//void CGvisR2R_PunchView::DoMark0All()
//{
//	CfPoint ptPnt;
//
//	if (!IsRun())
//	{
//		if (IsOnMarking0())
//		{
//			if (m_mgrPunch->m_pMotion->IsEnable(MS_X0) && m_mgrPunch->m_pMotion->IsEnable(MS_Y0))
//			{
//				if (m_mgrPunch->m_pMotion->IsMotionDone(MS_X0) && m_mgrPunch->m_pMotion->IsMotionDone(MS_Y0))
//				{
//					if (!IsInitPos0() && !IsPinPos0())
//						m_mgrPunch->MoveInitPos0();
//				}
//			}
//
//			if (m_mgrProcedure->m_nStepMk[2] < 5 && m_mgrProcedure->m_nStepMk[2] > 3) // Mk0();
//			{
//				m_mgrProcedure->m_nStepMk[2] = 3;	// ��ŷ�Ϸ�Check
//			}
//		}
//		return;
//	}
//
//
//	switch (m_mgrProcedure->m_nStepMk[2])
//	{
//	case 0:
//		m_mgrProcedure->m_nStepMk[2]++;
//		break;
//	case 1:
//		if (m_mgrProcedure->m_nMkPcs[2] < m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)
//		{
//			m_mgrProcedure->m_nStepMk[2]++;
//		}
//		else
//		{
//			m_mgrProcedure->m_nStepMk[2] = MK_END;
//		}
//		break;
//	case 2:
//		m_mgrProcedure->m_nStepMk[2]++;
//		break;
//	case 3:
//		if (m_mgrProcedure->m_nMkPcs[2] < m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)	// ��ŷ�Ϸ�Check
//		{
//			ptPnt = GetMkPnt(m_mgrProcedure->m_nMkPcs[2]);
//
//			if (ptPnt.x < 0.0 && ptPnt.y < 0.0) // ��ǰȭ.
//			{
//				m_mgrProcedure->m_nMkPcs[2]++;
//				break;
//			}
//
//			m_mgrProcedure->m_dTarget[AXIS_X0] = ptPnt.x;
//
//			// Cam0 : m_bPriority[0] <-, m_bPriority[3] ->
//			// Cam1 : m_bPriority[1] ->, m_bPriority[2] <-
//
//			m_mgrProcedure->m_bCollision[0] = m_mgrProcedure->ChkCollision(AXIS_X0, ptPnt.x);
//			if (!m_mgrProcedure->m_bCollision[0])
//			{
//				if (IsMoveDone0())
//				{
//					Move0(ptPnt, m_mgrProcedure->m_bCam);
//					m_mgrProcedure->m_nStepMk[2]++;
//				}
//			}
//			else if (m_mgrProcedure->m_bPriority[0])
//			{
//				if (IsMoveDone0())
//				{
//					m_mgrProcedure->m_bPriority[0] = FALSE;
//					Move0(ptPnt, m_mgrProcedure->m_bCam);
//					m_mgrProcedure->m_nStepMk[2]++;
//				}
//			}
//			else if (m_mgrProcedure->m_bPriority[2])
//			{
//				if (IsMoveDone0())
//				{
//					m_mgrProcedure->m_bPriority[2] = FALSE;
//					ptPnt.x = 0.0;
//					ptPnt.y = 0.0;//m_dEnc[AXIS_Y0];
//					Move0(ptPnt, FALSE);
//				}
//			}
//		}
//		else
//		{
//			m_mgrProcedure->m_nStepMk[2] = MK_END;
//		}
//		break;
//	case 4:
//		if (IsMoveDone0())
//			m_mgrProcedure->m_nStepMk[2]++;
//		break;
//	case 5:
//		if (!m_mgrProcedure->IsNoMk0())
//			Mk0();
//		else
//			SetDelay0(pDoc->m_nDelayShow, 1);		// [mSec]
//		m_mgrProcedure->m_nStepMk[2]++;
//		break;
//	case 6:
//		if (m_mgrProcedure->IsNoMk0())
//		{
//			if (!WaitDelay0(1))		// F:Done, T:On Waiting....
//				m_mgrProcedure->m_nStepMk[2]++;
//		}
//		else
//			m_mgrProcedure->m_nStepMk[2]++;
//		break;
//	case 7:
//		m_mgrProcedure->m_nMkPcs[2]++;
//		m_mgrProcedure->m_nStepMk[2]++;
//		break;
//	case 8:
//		if (m_mgrProcedure->m_nMkPcs[2] < m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)	// ��ŷ�Ϸ�Check
//		{
//			m_mgrProcedure->m_nStepMk[2] = 3;
//		}
//		else
//		{
//			m_mgrProcedure->m_nStepMk[2] = MK_END;
//		}
//		break;
//	case MK_END:
//		SetDelay0(100, 1);		// [mSec]
//		m_mgrProcedure->m_nStepMk[2]++;
//		break;
//	case 101:
//		if (!WaitDelay0(1))		// F:Done, T:On Waiting....
//		{
//			if (IsMoveDone0())
//			{
//				m_mgrPunch->MoveInitPos0();
//				m_mgrProcedure->m_nStepMk[2]++;
//			}
//		}
//		break;
//	case 102:
//		if (IsMoveDone0())
//			m_mgrProcedure->m_nStepMk[2]++;
//		break;
//	case 103:
//		m_mgrProcedure->m_bDoneMk[0] = TRUE;
//		m_mgrProcedure->m_bTHREAD_MK[2] = FALSE;
//		break;
//	}
//}

//void CGvisR2R_PunchView::DoMark1All()
//{
//	CfPoint ptPnt;
//
//	if (!IsRun())
//	{
//		if (IsOnMarking1())
//		{
//			if (m_mgrPunch->m_pMotion->IsEnable(MS_X1) && m_mgrPunch->m_pMotion->IsEnable(MS_Y1))
//			{
//				if (m_mgrPunch->m_pMotion->IsMotionDone(MS_X1) && m_mgrPunch->m_pMotion->IsMotionDone(MS_Y1))
//				{
//					if (!IsMkEdPos1() && !IsPinPos1())
//						m_mgrPunch->MoveMkEdPos1();
//				}
//			}
//
//			if (m_mgrProcedure->m_nStepMk[3] < 5 && m_mgrProcedure->m_nStepMk[3] > 3) // Mk1();
//			{
//				m_mgrProcedure->m_nStepMk[3] = 3;	// ��ŷ�Ϸ�Check
//			}
//		}
//		return;
//	}
//
//
//	switch (m_mgrProcedure->m_nStepMk[3])
//	{
//	case 0:
//		m_mgrProcedure->m_nStepMk[3]++;
//		break;
//	case 1:
//		if (m_mgrProcedure->m_nMkPcs[3] < m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)
//		{
//			m_mgrProcedure->m_nStepMk[3]++;
//		}
//		else
//		{
//			m_mgrProcedure->m_nStepMk[3] = MK_END;
//		}
//		break;
//	case 2:
//		m_mgrProcedure->m_nStepMk[3]++;
//		break;
//	case 3:
//		if (m_mgrProcedure->m_nMkPcs[3] < m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)	// ��ŷ�Ϸ�Check
//		{
//			ptPnt = GetMkPnt(m_mgrProcedure->m_nMkPcs[3]);
//
//			if (ptPnt.x < 0.0 && ptPnt.y < 0.0) // ��ǰȭ.
//			{
//				m_mgrProcedure->m_nMkPcs[3]++;
//				break;
//			}
//
//			m_mgrProcedure->m_dTarget[AXIS_X1] = ptPnt.x;
//
//			// Cam0 : m_bPriority[0] <-, m_bPriority[3] ->
//			// Cam1 : m_bPriority[1] ->, m_bPriority[2] <-
//
//			m_mgrProcedure->m_bCollision[1] = m_mgrProcedure->ChkCollision(AXIS_X1, ptPnt.x);
//			if (!m_mgrProcedure->m_bCollision[1])
//			{
//				if (IsMoveDone1())
//				{
//					Move1(ptPnt, m_mgrProcedure->m_bCam);
//					m_mgrProcedure->m_nStepMk[3]++;
//				}
//			}
//			else if (m_mgrProcedure->m_bPriority[1])
//			{
//				if (IsMoveDone1())
//				{
//					m_mgrProcedure->m_bPriority[1] = FALSE;
//					Move1(ptPnt, m_mgrProcedure->m_bCam);
//					m_mgrProcedure->m_nStepMk[3]++;
//				}
//			}
//			else if (m_mgrProcedure->m_bPriority[3])
//			{
//				if (IsMoveDone1())
//				{
//					m_mgrProcedure->m_bPriority[3] = FALSE;
//					ptPnt.x = _tstof(pDoc->WorkingInfo.Motion.sSafeZone);
//					ptPnt.y = 0.0;//m_dEnc[AXIS_Y1];
//					Move1(ptPnt, FALSE);
//				}
//			}
//		}
//		else
//		{
//			m_mgrProcedure->m_nStepMk[3] = MK_END;
//		}
//		break;
//	case 4:
//		if (IsMoveDone1())
//			m_mgrProcedure->m_nStepMk[3]++;
//		break;
//	case 5:
//		if (!m_mgrProcedure->IsNoMk1())
//			Mk1();
//		else
//			SetDelay1(pDoc->m_nDelayShow, 1);		// [mSec]
//		m_mgrProcedure->m_nStepMk[3]++;
//		break;
//	case 6:
//		if (m_mgrProcedure->IsNoMk1())
//		{
//			if (!WaitDelay1(1))		// F:Done, T:On Waiting....
//				m_mgrProcedure->m_nStepMk[3]++;
//		}
//		else
//			m_mgrProcedure->m_nStepMk[3]++;
//		break;
//	case 7:
//		m_mgrProcedure->m_nMkPcs[3]++;
//		m_mgrProcedure->m_nStepMk[3]++;
//		break;
//	case 8:
//		if (m_mgrProcedure->m_nMkPcs[3] < m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)	// ��ŷ�Ϸ�Check
//		{
//			m_mgrProcedure->m_nStepMk[3] = 3;
//		}
//		else
//		{
//			//StopLive1();
//			m_mgrProcedure->m_nStepMk[3] = MK_END;
//		}
//		break;
//	case MK_END:
//		SetDelay1(100, 1);		// [mSec]
//		m_mgrProcedure->m_nStepMk[3]++;
//		break;
//	case 101:
//		if (!WaitDelay1(1))		// F:Done, T:On Waiting....
//		{
//			if (m_mgrProcedure->m_nMkPcs[2] < m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)
//			{
//				if (m_mgrProcedure->m_nMkPcs[2] > 0)
//				{
//					if (!(m_mgrProcedure->m_Flag & (0x01 << 0)))
//					{
//						if (IsMoveDone1())
//						{
//							m_mgrProcedure->m_Flag |= (0x01 << 0);
//							ptPnt.x = _tstof(pDoc->WorkingInfo.Motion.sSafeZone);
//							ptPnt.y = 0.0;//m_dEnc[AXIS_Y1];
//							Move1(ptPnt, FALSE);
//						}
//					}
//				}
//				else
//				{
//					if (IsMoveDone1())
//					{
//						m_mgrProcedure->m_Flag &= ~(0x01 << 0);
//						m_mgrPunch->MoveMkEdPos1();
//						m_mgrProcedure->m_nStepMk[3]++;
//					}
//				}
//			}
//			else
//			{
//				if (IsMoveDone1())
//				{
//					m_mgrProcedure->m_Flag &= ~(0x01 << 0);
//					m_mgrPunch->MoveMkEdPos1();
//					m_mgrProcedure->m_nStepMk[3]++;
//				}
//			}
//		}
//		break;
//	case 102:
//		if (IsMoveDone0())
//			m_mgrProcedure->m_nStepMk[3]++;
//		break;
//	case 103:
//		m_mgrProcedure->m_bDoneMk[1] = TRUE;
//		m_mgrProcedure->m_bTHREAD_MK[3] = FALSE;
//		break;
//	}
//}

//void CGvisR2R_PunchView::DoMark0()
//{
//#ifdef TEST_MODE
//	return;
//#endif
//
//	if (!m_mgrPunch->m_bAuto)
//		return;
//
//	int nSerial, nIdx, nErrCode, nRtn;
//	CfPoint ptPnt;
//	CString sMsg;
//	double dStripOut = (m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs / MAX_STRIP_NUM) * _tstof(pDoc->WorkingInfo.LastJob.sStripOutRatio) / 100.0;
//	int nStripOut = int(dStripOut);
//	if (dStripOut > nStripOut)
//		nStripOut++;			// ��Ʈ�� ���� ����
//
//	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
//
//	if (!IsRun())																		// �������¿���
//	{
//		if (IsOnMarking0())																// ��ŷ�߿�
//		{
//			if (m_mgrPunch->m_pMotion->IsEnable(MS_X0) && m_mgrPunch->m_pMotion->IsEnable(MS_Y0))				// ����� Enable�����̰�
//			{
//				if (m_mgrPunch->m_pMotion->IsMotionDone(MS_X0) && m_mgrPunch->m_pMotion->IsMotionDone(MS_Y0))	// ��� Done�����̸�,
//				{
//					if (!IsInitPos0() && !IsPinPos0())									// �ʱ���ġ�� �ƴϰų�, ����ġ�� �ƴҶ�
//						m_mgrPunch->MoveInitPos0();													// �ʱ���ġ�� �̵�
//				}
//			}
//
//			if (m_mgrProcedure->m_nStepMk[0] < 13 && m_mgrProcedure->m_nStepMk[0] > 8) // Mk0();
//			{
//				m_mgrProcedure->m_nStepMk[0] = 8;	// ��ŷ�Ϸ�Check
//			}
//		}
//		return;
//	}
//
//	m_mgrProcedure->m_sDispSts[0].Format(_T("%d"), m_mgrProcedure->m_nStepMk[0]);
//
//	switch (m_mgrProcedure->m_nStepMk[0])
//	{
//	case 0:
//		if (m_mgrProcedure->IsNoMk())
//			ShowLive();
//		m_mgrProcedure->m_nStepMk[0]++;
//		break;
//	case 1:
//		if (!IsInitPos0())
//			m_mgrPunch->MoveInitPos0();
//		m_mgrProcedure->m_nStepMk[0]++;
//		break;
//	case 2:
//
//		nSerial = m_mgrProcedure->m_nBufUpSerial[0]; // Cam0
//
//		//nSerial = 1;
//
//		if (nSerial > 0)
//		{
//			//if ((nErrCode = GetErrCode0(nSerial)) != 1)
//			if ((nErrCode = GetErrCode(nSerial)) != 1)
//			{
//				m_mgrProcedure->m_nMkPcs[0] = GetTotDefPcs(nSerial);
//				m_mgrProcedure->m_nStepMk[0] = MK_END;
//			}
//			else
//			{
//				m_mgrProcedure->m_nStepMk[0]++;
//			}
//		}
//		else
//		{
//			StopFromThread();
//			AsyncMsgBox(_T("������ �ø����� �����ʽ��ϴ�."), 1);
//			BuzzerFromThread(TRUE, 0);
//			//pView->DispStsBar(_T("����-31"), 0);
//			DispMain(_T("�� ��"), RGB_RED);
//		}
//		break;
//	case 3:
//		m_mgrProcedure->m_nStepMk[0]++;
//		break;
//	case 4:
//		m_mgrProcedure->m_nStepMk[0]++;
//		break;
//	case 5:
//		m_mgrProcedure->m_nStepMk[0]++;
//		break;
//	case 6:
//
//		nSerial = m_mgrProcedure->m_nBufUpSerial[0]; // Cam0
//
//		if (m_mgrProcedure->m_nMkPcs[0] < GetTotDefPcs(nSerial))
//		{
//			if (!m_mgrProcedure->IsNoMk0())
//			{
//				;
//			}
//			else
//			{
//				if (!m_mgrProcedure->IsReview0())
//				{
//					if (m_mgrProcedure->m_bReview)
//					{
//						m_mgrProcedure->m_nMkPcs[0] = GetTotDefPcs(nSerial);
//						m_mgrProcedure->m_nStepMk[0] = MK_END;
//						break;
//					}
//				}
//			}
//			// Punching On�̰ų� Review�̸� �������� ����
//			SetDelay0(100, 1);		// [mSec]
//			m_mgrProcedure->m_nStepMk[0]++;
//		}
//		else
//		{
//			m_mgrProcedure->m_nStepMk[0] = MK_END;
//		}
//		break;
//	case 7:
//		nSerial = m_mgrProcedure->m_nBufUpSerial[0]; // Cam0
//
//		if (!WaitDelay0(1))		// F:Done, T:On Waiting....		// Delay�Ŀ�
//		{
//			m_mgrProcedure->m_nMkPcs[0] = 0;
//
//			if (!m_mgrProcedure->IsNoMk0())										// Punching On�̸�
//			{
//				m_mgrProcedure->m_nStepMk[0]++;
//			}
//			else												// Punching�� Off�̰�
//			{
//				if (m_mgrProcedure->IsReview0())								// Review�̸� ��������
//				{
//					m_mgrProcedure->m_nStepMk[0]++;
//				}
//				else											// Review�� �ƴϸ�
//				{
//					if (m_mgrProcedure->m_bReview)
//					{
//						m_mgrProcedure->m_nMkPcs[0] = GetTotDefPcs(nSerial);
//						m_mgrProcedure->m_nStepMk[0] = MK_END;
//					}
//					else
//						m_mgrProcedure->m_nStepMk[0]++;
//				}
//			}
//		}
//		break;
//	case 8:
//		nSerial = m_mgrProcedure->m_nBufUpSerial[0]; // Cam0
//
//		if (m_mgrProcedure->m_nMkPcs[0] < GetTotDefPcs(nSerial))	// ��ŷ�Ϸ�Check
//		{
//			if (m_mgrProcedure->m_nMkPcs[0] + 1 < GetTotDefPcs(nSerial))		// ���� ��ŷ��ġ�� ������
//			{
//				ptPnt = GetMkPnt(nSerial, m_mgrProcedure->m_nMkPcs[0] + 1);	// ���� ��ŷ��ġ
//				m_mgrProcedure->m_dNextTarget[AXIS_X0] = ptPnt.x;
//				m_mgrProcedure->m_dNextTarget[AXIS_Y0] = ptPnt.y;
//			}
//			else												// ���� ��ŷ��ġ�� ������
//			{
//				m_mgrProcedure->m_dNextTarget[AXIS_X0] = -1.0;
//				m_mgrProcedure->m_dNextTarget[AXIS_Y0] = -1.0;
//			}
//
//			ptPnt = GetMkPnt(nSerial, m_mgrProcedure->m_nMkPcs[0]);			// �̹� ��ŷ��ġ
//			if (ptPnt.x < 0.0 && ptPnt.y < 0.0) // ��ǰȭ. (��ŷ���� ����)
//			{
//				m_mgrProcedure->m_nMkPcs[0]++;
//				m_mgrProcedure->m_nStepMk[0] = MK_DONE_CHECK;
//				break;
//			}
//
//			nIdx = GetMkStripIdx(nSerial, m_mgrProcedure->m_nMkPcs[0]);		// 1 ~ 4 : strip index
//			if (nIdx > 0)										// Strip index�� �����̸�,
//			{
//				if (!IsMkStrip(nIdx)) // Strip[] Mk Off
//				{
//					m_mgrProcedure->m_nMkPcs[0]++;
//					m_mgrProcedure->m_nStepMk[0] = MK_DONE_CHECK;
//					break;
//				}
//				else
//				{
//					if (m_mgrProcedure->m_nMkStrip[0][nIdx - 1] >= nStripOut)
//					{
//						m_mgrProcedure->m_nMkPcs[0]++;
//						m_mgrProcedure->m_nStepMk[0] = MK_DONE_CHECK;
//						break;
//					}
//					else
//						m_mgrProcedure->m_nMkStrip[0][nIdx - 1]++;
//				}
//			}
//			else
//			{
//				pView->ClrDispMsg();
//				AfxMessageBox(_T("Strip Index Failed."));
//				break;
//			}
//
//			m_mgrProcedure->m_dTarget[AXIS_X0] = ptPnt.x;
//			m_mgrProcedure->m_dTarget[AXIS_Y0] = ptPnt.y;
//
//			m_mgrProcedure->m_nStepMk[0]++;
//		}
//		else
//		{
//			m_mgrProcedure->m_nStepMk[0] = MK_END;
//		}
//		break;
//	case 9:
//		// Cam0 : m_bPriority[0] <-, m_bPriority[3] ->
//		// Cam1 : m_bPriority[1] ->, m_bPriority[2] <-
//		ptPnt.x = m_mgrProcedure->m_dTarget[AXIS_X0];
//		ptPnt.y = m_mgrProcedure->m_dTarget[AXIS_Y0];
//
//		if (m_mgrProcedure->m_dNextTarget[AXIS_X0] < 0)
//			m_mgrProcedure->m_bCollision[0] = m_mgrProcedure->ChkCollision(AXIS_X0, m_mgrProcedure->m_dTarget[AXIS_X0]);
//		else
//			m_mgrProcedure->m_bCollision[0] = m_mgrProcedure->ChkCollision(AXIS_X0, m_mgrProcedure->m_dTarget[AXIS_X0], m_mgrProcedure->m_dNextTarget[AXIS_X0]);
//
//		if (!m_mgrProcedure->m_bCollision[0])
//		{
//			if (IsMoveDone0())
//			{
//				Move0(ptPnt, m_mgrProcedure->m_bCam);
//				m_mgrProcedure->m_nStepMk[0]++;
//			}
//		}
//		else if (m_mgrProcedure->m_bPriority[0])
//		{
//			if (IsMoveDone0())
//			{
//				m_mgrProcedure->m_bCollision[0] = FALSE;
//				m_mgrProcedure->m_bPriority[0] = FALSE;
//				Move0(ptPnt, m_mgrProcedure->m_bCam);
//				m_mgrProcedure->m_nStepMk[0]++;
//			}
//		}
//		else if (m_mgrProcedure->m_bPriority[2])
//		{
//			if (IsMoveDone0())
//			{
//				m_mgrProcedure->m_bCollision[0] = FALSE;
//				m_mgrProcedure->m_bPriority[2] = FALSE;
//				ptPnt.x = 0.0;
//				ptPnt.y = 0.0;//m_dEnc[AXIS_Y0];
//				Move0(ptPnt, FALSE);
//			}
//		}
//		break;
//	case 10:
//		m_mgrProcedure->m_nStepMk[0]++;
//		break;
//	case 11:
//		m_mgrProcedure->m_nStepMk[0]++;
//		break;
//	case 12:
//		if (IsMoveDone0())
//			m_mgrProcedure->m_nStepMk[0]++;
//		break;
//	case 13:
//		if (!m_mgrProcedure->IsNoMk0())
//		{
//			m_mgrProcedure->m_dwStMkDn[0] = GetTickCount();
//			Mk0();
//		}
//		else
//		{
//			// Verify - Mk0
//			SetDelay0(pDoc->m_nDelayShow, 1);		// [mSec]
//			if(!SaveMk0Img(m_mgrProcedure->m_nMkPcs[0]))
//			{
//				pView->ClrDispMsg();
//				AfxMessageBox(_T("Error-SaveMk0Img()"));
//			}
//			//m_nDebugStep = m_nMkPcs[0]; DispThreadTick();
//		}
//		m_mgrProcedure->m_nStepMk[0]++;
//		break;
//	case 14:
//		m_mgrProcedure->m_nStepMk[0]++;
//		break;
//	case 15:
//		m_mgrProcedure->m_nStepMk[0]++;
//		break;
//	case 16:
//		if (m_mgrProcedure->IsNoMk0())
//		{
//			if (!WaitDelay0(1))		// F:Done, T:On Waiting....
//				m_mgrProcedure->m_nStepMk[0]++;
//		}
//		else
//			m_mgrProcedure->m_nStepMk[0]++;
//		break;
//	case 17:
//		if (!m_mgrProcedure->IsNoMk0())
//		{
//			if (IsMk0Done())
//			{
//				// One more MK On Start....
//				if (!m_mgrProcedure->m_nMkPcs[0] && !m_mgrProcedure->m_bAnswer[2])
//				{
//					m_mgrProcedure->m_bAnswer[2] = TRUE;
//					Mk0();
//				}
//				else
//				{
//					m_mgrProcedure->m_bAnswer[2] = FALSE;
//					m_mgrProcedure->m_nMkPcs[0]++;
//					m_mgrProcedure->m_nStepMk[0]++;
//					m_mgrProcedure->m_nStepMk[0]++;
//				}
//			}
//			else
//			{
//				if (m_mgrProcedure->m_dwStMkDn[0] + 5000 < GetTickCount())
//				{
//					BuzzerFromThread(TRUE, 0);
//					DispMain(_T("�� ��"), RGB_RED);
//					m_mgrPunch->m_pVoiceCoil[0]->SearchHomeSmac0();
//
//					nRtn = AsyncMsgBox(_T("���̽�����(��) ��ſϷᰡ �ʵ˴ϴ�.\r\n��ŷ�� �ٽ� �õ��Ͻðڽ��ϱ�?"), 1, MB_YESNO);
//					if (IDYES == nRtn)
//					{
//						DispMain(_T("������"), RGB_RED);
//						m_mgrProcedure->m_nStepMk[0] = 13;
//					}
//					else if (nRtn < 0)
//						m_mgrProcedure->m_nStepMk[0]++; // Wait...
//					else
//					{
//						m_mgrProcedure->m_bAnswer[2] = FALSE;
//						m_mgrProcedure->m_nMkPcs[0]++;
//						m_mgrProcedure->m_nStepMk[0]++;
//						m_mgrProcedure->m_nStepMk[0]++;
//						Stop();
//					}
//				}
//			}
//		}
//		else
//		{
//			m_mgrProcedure->m_nMkPcs[0]++;
//			m_mgrProcedure->m_nStepMk[0]++;
//			m_mgrProcedure->m_nStepMk[0]++;
//		}
//		break;
//	case 18:
//		if ((nRtn = WaitRtnVal(1)) > -1)
//		{
//			if (IDYES == nRtn)
//			{
//				DispMain(_T("������"), RGB_RED);
//				m_mgrProcedure->m_nStepMk[0] = 13;
//			}
//			else
//			{
//				m_mgrProcedure->m_bAnswer[2] = FALSE;
//				m_mgrProcedure->m_nMkPcs[0]++;
//				m_mgrProcedure->m_nStepMk[0]++;
//				StopFromThread();
//			}
//		}
//		break;
//	case 19:
//		m_mgrProcedure->m_nStepMk[0] = MK_DONE_CHECK;
//		break;
//	case MK_DONE_CHECK:
//		nSerial = m_mgrProcedure->m_nBufUpSerial[0]; // Cam0
//
//		if (m_mgrProcedure->m_nMkPcs[0] < GetTotDefPcs(nSerial))
//		{
//			if (m_mgrProcedure->IsNoMk0())
//			{
//				if (!pDoc->WorkingInfo.System.bNoMk)
//				{
//					if (m_mgrProcedure->IsReview0())
//					{
//						if (IsJogRtDn0())
//							m_mgrProcedure->m_nStepMk[0]++;
//					}
//					else if (!pDoc->WorkingInfo.LastJob.bVerify)
//					{
//						m_mgrProcedure->m_nMkPcs[0] = GetTotDefPcs(nSerial);
//						m_mgrProcedure->m_nStepMk[0] = MK_END;
//					}
//					else
//						m_mgrProcedure->m_nStepMk[0] = 8;	// ��ŷ�Ϸ�Check
//				}
//				else
//					m_mgrProcedure->m_nStepMk[0] = 8;	// ��ŷ�Ϸ�Check
//			}
//			else
//				m_mgrProcedure->m_nStepMk[0] = 8;	// ��ŷ�Ϸ�Check
//		}
//		else
//		{
//			if (m_mgrProcedure->IsNoMk0())
//			{
//				if (m_mgrProcedure->IsReview0())
//				{
//					if (IsJogRtDn0())
//						m_mgrProcedure->m_nStepMk[0] = MK_END;
//				}
//				else
//				{
//					m_mgrProcedure->m_nStepMk[0] = MK_END;
//				}
//			}
//			else
//				m_mgrProcedure->m_nStepMk[0] = MK_END;
//		}
//		break;
//	case MK_DONE_CHECK + 1:
//		nSerial = m_mgrProcedure->m_nBufUpSerial[0]; // Cam0
//
//		if (m_mgrProcedure->m_nMkPcs[0] < GetTotDefPcs(nSerial))
//		{
//			if (m_mgrProcedure->IsNoMk0())
//			{
//				if (m_mgrProcedure->IsReview0())
//				{
//					if (IsJogRtUp0())
//						m_mgrProcedure->m_nStepMk[0] = 8;	// ��ŷ�Ϸ�Check
//				}
//				else
//				{
//					m_mgrProcedure->m_nMkPcs[0] = GetTotDefPcs(nSerial);
//					m_mgrProcedure->m_nStepMk[0] = MK_END;
//				}
//			}
//			else
//				m_mgrProcedure->m_nStepMk[0] = 8;	// ��ŷ�Ϸ�Check
//		}
//		else
//		{
//			m_mgrProcedure->m_nStepMk[0] = MK_END;
//		}
//		break;
//
//	case MK_END:
//		if (IsMoveDone0())
//			m_mgrProcedure->m_nStepMk[0]++;
//		break;
//	case 101:
//		SetDelay0(100, 1);		// [mSec]
//		m_mgrProcedure->m_nStepMk[0]++;
//		break;
//	case 102:
//		if (!WaitDelay0(1))		// F:Done, T:On Waiting....
//		{
//			m_mgrProcedure->m_nStepMk[0]++;
//		}
//		break;
//	case 103:
//		if (!IsInitPos0())
//		{
//			m_mgrProcedure->m_dTarget[AXIS_X0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[0]);
//			m_mgrProcedure->m_dTarget[AXIS_Y0] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[0]);
//			m_mgrProcedure->m_dNextTarget[AXIS_X0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[0]);
//			m_mgrProcedure->m_dNextTarget[AXIS_Y0] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[0]);
//
//			m_mgrPunch->MoveInitPos0();
//		}
//
//		pDoc->SaveMkCntL();
//		m_mgrProcedure->m_nStepMk[0]++;
//		break;
//	case 104:
//		if (IsMoveDone0())
//		{
//			m_mgrProcedure->m_nStepMk[0]++;
//			SetDelay0(10000, 1);		// [mSec]
//		}
//		break;
//	case 105:
//		if (IsInitPos0())
//		{
//			m_mgrProcedure->m_nStepMk[0]++;
//		}
//		else
//		{
//			if (!WaitDelay0(1))		// F:Done, T:On Waiting....
//			{
//				m_mgrProcedure->m_nStepMk[0] = ERR_PROC;
//			}
//		}
//		break;
//	case 106: // MK Done....
//		m_mgrProcedure->m_bDoneMk[0] = TRUE;
//		m_mgrProcedure->m_bTHREAD_MK[0] = FALSE;
//		break;
//
//	case ERR_PROC:
//		DispMain(_T("�� ��"), RGB_RED);
//		m_mgrPunch->m_pVoiceCoil[0]->SearchHomeSmac0();
//		AsyncMsgBox(_T("���̽�����(��) �ʱ���ġ �̵��� ���� �ʽ��ϴ�.\r\n��ŷ���¸� Ȯ���ϼ���."), 1);
//		m_mgrProcedure->m_nStepMk[0]++;
//		break;
//	case ERR_PROC + 1:
//		nSerial = m_mgrProcedure->m_nBufUpSerial[0]; // Cam0
//
//		if (m_mgrProcedure->m_bCam)
//			sMsg.Format(_T("%d�� Shot�� �ٽ� �ҷ�Ȯ���� �Ͻðڽ��ϱ�?"), nSerial);
//		else
//			sMsg.Format(_T("%d�� Shot�� �ٽ� ��ŷ�Ͻðڽ��ϱ�?"), nSerial);
//
//		m_mgrProcedure->m_nRtnMyMsgBoxIdx = 0;
//		m_mgrProcedure->m_bRtnMyMsgBox[0] = FALSE;
//		m_mgrProcedure->m_nRtnMyMsgBox[0] = -1;
//		pView->AsyncMsgBox(sMsg, 1, MB_YESNO);
//		sMsg.Empty();
//		m_mgrProcedure->m_nStepMk[0]++;
//		break;
//	case ERR_PROC + 2:
//		if ((nRtn = WaitRtnVal()) > -1)
//		{
//			if (IDYES == nRtn)
//			{
//				m_mgrProcedure->m_nStepMk[0] = ERR_PROC + 10;
//			}
//			else
//			{
//				m_mgrProcedure->m_nRtnMyMsgBoxIdx = 0;
//				m_mgrProcedure->m_bRtnMyMsgBox[0] = FALSE;
//				m_mgrProcedure->m_nRtnMyMsgBox[0] = -1;
//				sMsg.Format(_T("��� ���� �۾��� �����Ͻðڽ��ϱ�?"), nSerial);
//				pView->AsyncMsgBox(sMsg, 1, MB_YESNO);
//				sMsg.Empty();
//
//				m_mgrProcedure->m_nStepMk[0]++;
//			}
//		}
//		break;
//	case ERR_PROC + 3:
//		if ((nRtn = WaitRtnVal()) > -1)
//		{
//			if (IDYES == nRtn)
//			{
//				m_mgrProcedure->m_nStepMk[0] = ERR_PROC + 20;
//			}
//			else
//			{
//				m_bDispMsgDoAuto[8] = TRUE;
//				m_nStepDispMsg[8] = FROM_DOMARK0;
//			}
//		}
//		break;
//	case ERR_PROC + 10:
//		m_mgrProcedure->m_bReMark[0] = TRUE;
//		m_mgrProcedure->m_bTHREAD_MK[0] = FALSE;
//		m_mgrProcedure->m_nStepMk[0] = 0;
//		break;
//	case ERR_PROC + 20: // MK Done....
//		m_mgrProcedure->m_bDoneMk[0] = TRUE;
//		m_mgrProcedure->m_bTHREAD_MK[0] = FALSE;
//		break;
//	}
//}

//BOOL CGvisR2R_PunchView::SaveMk0Img(int nMkPcsIdx) // Cam0
//{
//	if (!pDoc->WorkingInfo.System.bSaveMkImg)
//		return TRUE;
//
//	int nSerial;
//	nSerial = m_mgrProcedure->m_nBufUpSerial[0]; // Cam0
//
//	CString sSrc, sDest, sPath;
//	stModelInfo stInfo;
//
//	sSrc.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufUp, nSerial);
//	if (!m_mgrReelmap->GetPcrInfo(sSrc, stInfo))
//	{
//		pView->DispStsBar(_T("E(2)"), 5);
//		pView->ClrDispMsg();
//		AfxMessageBox(_T("Error-GetPcrInfo(2)"));
//		return FALSE;
//	}
//
//	if (!pDoc->MakeMkDir(stInfo))
//	{
//		pView->ClrDispMsg();
//		AfxMessageBox(_T("Error-MakeMkDir()"));
//		return FALSE;
//	}
//
//	sDest.Format(_T("%s%s\\%s\\%s\\Punching"), pDoc->WorkingInfo.System.sPathOldFile, stInfo.sModel,
//		stInfo.sLot, stInfo.sLayer);
//
//	if (!pDoc->DirectoryExists(sDest))
//		CreateDirectory(sDest, NULL);
//
//	if (GetTotDefPcs(nSerial) > 0)
//	{
//		sPath.Format(_T("%s\\%s.tif"), sDest, GetMkInfo(nSerial, nMkPcsIdx));
//
//#ifdef USE_VISION
//		if (m_mgrPunch->m_pVision[0])
//			return m_mgrPunch->m_pVision[0]->SaveMkImg(sPath);
//#endif
//	}
//	else
//		return TRUE;
//
//	return FALSE;
//}

//void CGvisR2R_PunchView::DoMark1()
//{
//#ifdef TEST_MODE
//	return;
//#endif
//
//	if (!m_mgrPunch->m_bAuto)
//		return;
//
//	int nSerial, nIdx, nErrCode, nRtn;
//	CfPoint ptPnt;
//	CString sMsg;
//	double dStripOut = (m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs / MAX_STRIP_NUM) * _tstof(pDoc->WorkingInfo.LastJob.sStripOutRatio) / 100.0;
//	int nStripOut = int(dStripOut);
//	if (dStripOut > nStripOut)
//		nStripOut++;
//
//	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
//
//	if (!IsRun())
//	{
//		if (IsOnMarking1())
//		{
//			if (m_mgrPunch->m_pMotion->IsEnable(MS_X1) && m_mgrPunch->m_pMotion->IsEnable(MS_Y1))
//			{
//				if (m_mgrPunch->m_pMotion->IsMotionDone(MS_X1) && m_mgrPunch->m_pMotion->IsMotionDone(MS_Y1))
//				{
//					if (!IsMkEdPos1() && !IsPinPos1())
//						m_mgrPunch->MoveMkEdPos1();
//				}
//			}
//
//			if (m_mgrProcedure->m_nStepMk[1] < 13 && m_mgrProcedure->m_nStepMk[1] > 8) // Mk1();
//			{
//				m_mgrProcedure->m_nStepMk[1] = 8;	// ��ŷ�Ϸ�Check
//			}
//		}
//		return;
//	}
//
//
//	m_mgrProcedure->m_sDispSts[1].Format(_T("%d"), m_mgrProcedure->m_nStepMk[1]);
//
//	switch (m_mgrProcedure->m_nStepMk[1])
//	{
//	case 0:
//		if (m_mgrProcedure->IsNoMk())
//			ShowLive();
//		m_mgrProcedure->m_nStepMk[1]++;
//		break;
//	case 1:
//		if (!IsInitPos1())
//			m_mgrPunch->MoveInitPos1();
//		m_mgrProcedure->m_nStepMk[1]++;
//		break;
//	case 2:
//
//		nSerial = m_mgrProcedure->m_nBufUpSerial[1]; // Cam1
//
//		if (nSerial > 0)
//		{
//			//if ((nErrCode = GetErrCode1(nSerial)) != 1)
//			if ((nErrCode = GetErrCode(nSerial)) != 1)
//			{
//				m_mgrProcedure->m_nMkPcs[1] = GetTotDefPcs(nSerial);
//				m_mgrProcedure->m_nStepMk[1] = MK_END;
//			}
//			else
//			{
//				m_mgrProcedure->m_nStepMk[1]++;
//			}
//		}
//		else
//		{
//			StopFromThread();
//			AsyncMsgBox(_T("������ �ø����� �����ʽ��ϴ�."), 2);
//			BuzzerFromThread(TRUE, 0);
//			DispMain(_T("�� ��"), RGB_RED);
//		}
//		break;
//	case 3:
//		m_mgrProcedure->m_nStepMk[1]++;
//		break;
//	case 4:
//		m_mgrProcedure->m_nStepMk[1]++;
//		break;
//	case 5:
//		m_mgrProcedure->m_nStepMk[1]++;
//		break;
//	case 6:
//		nSerial = m_mgrProcedure->m_nBufUpSerial[1]; // Cam1
//
//		if (m_mgrProcedure->m_nMkPcs[1] < GetTotDefPcs(nSerial))
//		{
//			if (!m_mgrProcedure->IsNoMk1())
//			{
//				;
//			}
//			else
//			{
//				if (!m_mgrProcedure->IsReview1())
//				{
//					if (m_mgrProcedure->m_bReview)
//					{
//						m_mgrProcedure->m_nMkPcs[1] = GetTotDefPcs(nSerial);
//						m_mgrProcedure->m_nStepMk[1] = MK_END;
//						break;
//					}
//				}
//			}
//			SetDelay1(100, 6);		// [mSec]
//			m_mgrProcedure->m_nStepMk[1]++;
//		}
//		else
//		{
//			m_mgrProcedure->m_nStepMk[1] = MK_END;
//		}
//		break;
//	case 7:
//		nSerial = m_mgrProcedure->m_nBufUpSerial[1]; // Cam1
//
//		if (!WaitDelay1(6))		// F:Done, T:On Waiting....
//		{
//			m_mgrProcedure->m_nMkPcs[1] = 0;
//
//			if (!m_mgrProcedure->IsNoMk1())
//			{
//				m_mgrProcedure->m_nStepMk[1]++;
//			}
//			else
//			{
//				if (m_mgrProcedure->IsReview1())
//				{
//					m_mgrProcedure->m_nStepMk[1]++;
//				}
//				else
//				{
//					if (m_mgrProcedure->m_bReview)
//					{
//						m_mgrProcedure->m_nMkPcs[1] = GetTotDefPcs(nSerial);
//						m_mgrProcedure->m_nStepMk[1] = MK_END;
//					}
//					else
//						m_mgrProcedure->m_nStepMk[1]++;
//				}
//			}
//		}
//		break;
//	case 8:
//		nSerial = m_mgrProcedure->m_nBufUpSerial[1]; // Cam1
//
//		if (m_mgrProcedure->m_nMkPcs[1] < GetTotDefPcs(nSerial))	// ��ŷ�Ϸ�Check
//		{
//			if (m_mgrProcedure->m_nMkPcs[1] + 1 < GetTotDefPcs(nSerial))
//			{
//				ptPnt = GetMkPnt(nSerial, m_mgrProcedure->m_nMkPcs[1] + 1);
//				m_mgrProcedure->m_dNextTarget[AXIS_X1] = ptPnt.x;
//				m_mgrProcedure->m_dNextTarget[AXIS_Y1] = ptPnt.y;
//			}
//			else
//			{
//				m_mgrProcedure->m_dNextTarget[AXIS_X1] = -1.0;
//				m_mgrProcedure->m_dNextTarget[AXIS_Y1] = -1.0;
//			}
//
//			ptPnt = GetMkPnt(nSerial, m_mgrProcedure->m_nMkPcs[1]);
//			if (ptPnt.x < 0.0 && ptPnt.y < 0.0) // ��ǰȭ.
//			{
//				m_mgrProcedure->m_nMkPcs[1]++;
//				m_mgrProcedure->m_nStepMk[1] = MK_DONE_CHECK;
//				break;
//			}
//
//			nIdx = GetMkStripIdx(nSerial, m_mgrProcedure->m_nMkPcs[1]);
//			if (nIdx > 0)
//			{
//				if (!IsMkStrip(nIdx)) // Strip[] Mk Off
//				{
//					m_mgrProcedure->m_nMkPcs[1]++;
//					m_mgrProcedure->m_nStepMk[1] = MK_DONE_CHECK;
//					break;
//				}
//				else
//				{
//					if (m_mgrProcedure->m_nMkStrip[1][nIdx - 1] >= nStripOut)
//					{
//						m_mgrProcedure->m_nMkPcs[1]++;
//						m_mgrProcedure->m_nStepMk[1] = MK_DONE_CHECK;
//						break;
//					}
//					else
//						m_mgrProcedure->m_nMkStrip[1][nIdx - 1]++;
//				}
//			}
//			else
//			{
//				pView->ClrDispMsg();
//				AfxMessageBox(_T("Strip Index Failed."));
//				break;
//			}
//
//			m_mgrProcedure->m_dTarget[AXIS_X1] = ptPnt.x;
//			m_mgrProcedure->m_dTarget[AXIS_Y1] = ptPnt.y;
//
//			m_mgrProcedure->m_nStepMk[1]++;
//		}
//		else
//		{
//			m_mgrProcedure->m_nStepMk[1] = MK_END;
//		}
//		break;
//	case 9:
//		// Cam0 : m_bPriority[0] <-, m_bPriority[3] ->
//		// Cam1 : m_bPriority[1] ->, m_bPriority[2] <-
//		ptPnt.x = m_mgrProcedure->m_dTarget[AXIS_X1];
//		ptPnt.y = m_mgrProcedure->m_dTarget[AXIS_Y1];
//
//		if (m_mgrProcedure->m_dNextTarget[AXIS_X1] < 0)
//			m_mgrProcedure->m_bCollision[1] = m_mgrProcedure->ChkCollision(AXIS_X1, m_mgrProcedure->m_dTarget[AXIS_X1]);
//		else
//			m_mgrProcedure->m_bCollision[1] = m_mgrProcedure->ChkCollision(AXIS_X1, m_mgrProcedure->m_dTarget[AXIS_X1], m_mgrProcedure->m_dNextTarget[AXIS_X1]);
//
//		if (!m_mgrProcedure->m_bCollision[1])
//		{
//			if (IsMoveDone1())
//			{
//				Move1(ptPnt, m_mgrProcedure->m_bCam);
//				m_mgrProcedure->m_nStepMk[1]++;
//			}
//		}
//		else if (m_mgrProcedure->m_bPriority[1])
//		{
//			if (IsMoveDone1())
//			{
//				m_mgrProcedure->m_bCollision[1] = FALSE;
//				m_mgrProcedure->m_bPriority[1] = FALSE;
//				Move1(ptPnt, m_mgrProcedure->m_bCam);
//				m_mgrProcedure->m_nStepMk[1]++;
//			}
//		}
//		else if (m_mgrProcedure->m_bPriority[3])
//		{
//			if (IsMoveDone1())
//			{
//				m_mgrProcedure->m_bCollision[1] = FALSE;
//				m_mgrProcedure->m_bPriority[3] = FALSE;
//				ptPnt.x = _tstof(pDoc->WorkingInfo.Motion.sSafeZone);
//				ptPnt.y = 0.0;//m_dEnc[AXIS_Y1];
//				Move1(ptPnt, FALSE);
//			}
//		}
//		break;
//	case 10:
//		m_mgrProcedure->m_nStepMk[1]++;
//		break;
//	case 11:
//		m_mgrProcedure->m_nStepMk[1]++;
//		break;
//	case 12:
//		if (IsMoveDone1())
//			m_mgrProcedure->m_nStepMk[1]++;
//		break;
//	case 13:
//		if (!m_mgrProcedure->IsNoMk1())
//		{
//			m_mgrProcedure->m_dwStMkDn[1] = GetTickCount();
//			Mk1();
//		}
//		else
//		{
//			// Verify - Mk1
//			SetDelay1(pDoc->m_nDelayShow, 6);		// [mSec]
//			if(!SaveMk1Img(m_mgrProcedure->m_nMkPcs[1]))
//			{
//				pView->ClrDispMsg();
//				AfxMessageBox(_T("Error-SaveMk1Img()"));
//			}
//		}
//		m_mgrProcedure->m_nStepMk[1]++;
//		break;
//	case 14:
//		m_mgrProcedure->m_nStepMk[1]++;
//		break;
//	case 15:
//		m_mgrProcedure->m_nStepMk[1]++;
//		break;
//	case 16:
//		if (m_mgrProcedure->IsNoMk1())
//		{
//			if (!WaitDelay1(6))		// F:Done, T:On Waiting....
//				m_mgrProcedure->m_nStepMk[1]++;
//		}
//		else
//			m_mgrProcedure->m_nStepMk[1]++;
//		break;
//	case 17:
//		if (!m_mgrProcedure->IsNoMk1())
//		{
//			if (IsMk1Done())
//			{
//				// One more MK On Start....
//				if (!m_mgrProcedure->m_nMkPcs[1] && !m_mgrProcedure->m_bAnswer[3])
//				{
//					m_mgrProcedure->m_bAnswer[3] = TRUE;
//					Mk1();
//				}
//				else
//				{
//					m_mgrProcedure->m_bAnswer[3] = FALSE;
//					m_mgrProcedure->m_nMkPcs[1]++;
//					m_mgrProcedure->m_nStepMk[1]++;
//					m_mgrProcedure->m_nStepMk[1]++;
//				}
//
//			}
//			else
//			{
//				if (m_mgrProcedure->m_dwStMkDn[1] + 5000 < GetTickCount())
//				{
//					BuzzerFromThread(TRUE, 0);
//					DispMain(_T("�� ��"), RGB_RED);
//					m_mgrPunch->m_pVoiceCoil[1]->SearchHomeSmac1();
//
//					nRtn = AsyncMsgBox(_T("���̽�����(��) ��ſϷᰡ �ʵ˴ϴ�.\r\n��ŷ�� �ٽ� �õ��Ͻðڽ��ϱ�?"), 2, MB_YESNO);
//					if (IDYES == nRtn)
//					{
//						DispMain(_T("������"), RGB_RED);
//						m_mgrProcedure->m_nStepMk[1] = 13;
//					}
//					else if (nRtn < 0)
//						m_mgrProcedure->m_nStepMk[1]++; // Wait...
//					else
//					{
//						m_mgrProcedure->m_bAnswer[3] = FALSE;
//						m_mgrProcedure->m_nMkPcs[1]++;
//						m_mgrProcedure->m_nStepMk[1]++;
//						m_mgrProcedure->m_nStepMk[1]++;
//						Stop();
//					}
//				}
//			}
//		}
//		else
//		{
//			m_mgrProcedure->m_nMkPcs[1]++;
//			m_mgrProcedure->m_nStepMk[1]++;
//			m_mgrProcedure->m_nStepMk[1]++;
//		}
//		break;
//	case 18:
//		if ((nRtn = WaitRtnVal(2)) > -1)
//		{
//			if (IDYES == nRtn)
//			{
//				DispMain(_T("������"), RGB_RED);
//				m_mgrProcedure->m_nStepMk[1] = 13;
//			}
//			else
//			{
//				m_mgrProcedure->m_bAnswer[3] = FALSE;
//				m_mgrProcedure->m_nMkPcs[1]++;
//				m_mgrProcedure->m_nStepMk[1]++;
//				StopFromThread();
//			}
//		}
//		break;
//	case 19:
//		m_mgrProcedure->m_nStepMk[1] = MK_DONE_CHECK;
//		break;
//	case MK_DONE_CHECK:
//		nSerial = m_mgrProcedure->m_nBufUpSerial[1]; // Cam1
//
//		if (m_mgrProcedure->m_nMkPcs[1] < GetTotDefPcs(nSerial))
//		{
//			if (m_mgrProcedure->IsNoMk1())
//			{
//				if (!pDoc->WorkingInfo.System.bNoMk)
//				{
//					if (m_mgrProcedure->IsReview1())
//					{
//						if (IsJogRtDn1())
//							m_mgrProcedure->m_nStepMk[1]++;
//					}
//					else if (!pDoc->WorkingInfo.LastJob.bVerify)
//					{
//						m_mgrProcedure->m_nMkPcs[1] = GetTotDefPcs(nSerial);
//						m_mgrProcedure->m_nStepMk[1] = MK_END;
//					}
//					else
//						m_mgrProcedure->m_nStepMk[1] = 8;	// ��ŷ�Ϸ�Check
//				}
//				else
//					m_mgrProcedure->m_nStepMk[1] = 8;	// ��ŷ�Ϸ�Check
//			}
//			else
//				m_mgrProcedure->m_nStepMk[1] = 8;	// ��ŷ�Ϸ�Check
//		}
//		else
//		{
//			if (m_mgrProcedure->IsNoMk1())
//			{
//				if (m_mgrProcedure->IsReview1())
//				{
//					if (IsJogRtDn1())
//						m_mgrProcedure->m_nStepMk[1] = MK_END;
//				}
//				else
//				{
//					m_mgrProcedure->m_nStepMk[1] = MK_END;
//				}
//			}
//			else
//				m_mgrProcedure->m_nStepMk[1] = MK_END;
//		}
//		break;
//	case MK_DONE_CHECK + 1:
//		nSerial = m_mgrProcedure->m_nBufUpSerial[1]; // Cam1
//
//		if (m_mgrProcedure->m_nMkPcs[1] < GetTotDefPcs(nSerial))
//		{
//			if (m_mgrProcedure->IsNoMk1())
//			{
//				if (m_mgrProcedure->IsReview1())
//				{
//					if (IsJogRtUp1())
//						m_mgrProcedure->m_nStepMk[1] = 8;	// ��ŷ�Ϸ�Check
//				}
//				else
//				{
//					m_mgrProcedure->m_nMkPcs[1] = GetTotDefPcs(nSerial);
//					m_mgrProcedure->m_nStepMk[1] = MK_END;
//				}
//			}
//			else
//				m_mgrProcedure->m_nStepMk[1] = 8;	// ��ŷ�Ϸ�Check
//		}
//		else
//		{
//			m_mgrProcedure->m_nStepMk[1] = MK_END;
//		}
//		break;
//
//
//	case MK_END:
//		if (IsMoveDone1())
//			m_mgrProcedure->m_nStepMk[1]++;
//		break;
//	case 101:
//		SetDelay1(100, 6);		// [mSec]
//		m_mgrProcedure->m_nStepMk[1]++;
//		break;
//	case 102:
//		if (!WaitDelay1(6))		// F:Done, T:On Waiting....
//			m_mgrProcedure->m_nStepMk[1]++;
//		break;
//	case 103:
//		if (!IsMkEdPos1())
//		{
//			m_mgrProcedure->m_dTarget[AXIS_X1] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[1]);
//			m_mgrProcedure->m_dTarget[AXIS_Y1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[1]);
//			m_mgrProcedure->m_dNextTarget[AXIS_X1] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[1]);
//			m_mgrProcedure->m_dNextTarget[AXIS_Y1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[1]);
//
//			m_mgrPunch->MoveMkEdPos1();
//		}
//
//		pDoc->SaveMkCntR();
//		m_mgrProcedure->m_nStepMk[1]++;
//		break;
//	case 104:
//		if (IsMoveDone1())
//		{
//			m_mgrProcedure->m_nStepMk[1]++;
//			SetDelay1(10000, 6);		// [mSec]
//		}
//		break;
//	case 105:
//		if (IsMkEdPos1())
//		{
//			m_mgrProcedure->m_nStepMk[1]++;
//		}
//		else
//		{
//			if (!WaitDelay1(6))		// F:Done, T:On Waiting....
//			{
//				m_mgrProcedure->m_nStepMk[1] = ERR_PROC;
//			}
//		}
//		break;
//	case 106: // MK Done....
//		m_mgrProcedure->m_bDoneMk[1] = TRUE;
//		m_mgrProcedure->m_bTHREAD_MK[1] = FALSE;
//		break;
//
//	case ERR_PROC:
//		DispMain(_T("�� ��"), RGB_RED);
//		m_mgrPunch->m_pVoiceCoil[1]->SearchHomeSmac1();
//		AsyncMsgBox(_T("���̽�����(��) �ʱ���ġ �̵��� ���� �ʽ��ϴ�.\r\n��ŷ���¸� Ȯ���ϼ���."), 2);
//		m_mgrProcedure->m_nStepMk[1]++;
//		break;
//	case ERR_PROC + 1:
//		nSerial = m_mgrProcedure->m_nBufUpSerial[1]; // Cam1
//
//		if (m_mgrProcedure->m_bCam)
//			sMsg.Format(_T("%d�� Shot�� �ٽ� �ҷ�Ȯ���� �Ͻðڽ��ϱ�?"), nSerial);
//		else
//			sMsg.Format(_T("%d�� Shot�� �ٽ� ��ŷ�Ͻðڽ��ϱ�?"), nSerial);
//
//		m_mgrProcedure->m_nRtnMyMsgBoxIdx = 1;
//		m_mgrProcedure->m_bRtnMyMsgBox[1] = FALSE;
//		m_mgrProcedure->m_nRtnMyMsgBox[1] = -1;
//		pView->AsyncMsgBox(sMsg, 2, MB_YESNO);
//		sMsg.Empty();
//		m_mgrProcedure->m_nStepMk[1]++;
//		break;
//	case ERR_PROC + 2:
//		if ((nRtn = WaitRtnVal()) > -1)
//		{
//			if (IDYES == nRtn)
//			{
//				m_mgrProcedure->m_nStepMk[1] = ERR_PROC + 10;
//			}
//			else
//			{
//				m_mgrProcedure->m_nRtnMyMsgBoxIdx = 1;
//				m_mgrProcedure->m_bRtnMyMsgBox[1] = FALSE;
//				m_mgrProcedure->m_nRtnMyMsgBox[1] = -1;
//				sMsg.Format(_T("��� ���� �۾��� �����Ͻðڽ��ϱ�?"), nSerial);
//				pView->AsyncMsgBox(sMsg, 2, MB_YESNO);
//				sMsg.Empty();
//
//				m_mgrProcedure->m_nStepMk[1]++;
//
//			}
//		}
//		break;
//	case ERR_PROC + 3:
//		if ((nRtn = WaitRtnVal()) > -1)
//		{
//			if (IDYES == nRtn)
//			{
//				m_mgrProcedure->m_nStepMk[1] = ERR_PROC + 20;
//			}
//			else
//			{
//				m_bDispMsgDoAuto[9] = TRUE;
//				m_nStepDispMsg[9] = FROM_DOMARK1;
//			}
//		}
//		break;
//	case ERR_PROC + 10:
//		m_mgrProcedure->m_bReMark[1] = TRUE;
//		m_mgrProcedure->m_bTHREAD_MK[1] = FALSE;
//		m_mgrProcedure->m_nStepMk[1] = 0;
//		break;
//	case ERR_PROC + 20: // MK Done....
//		m_mgrProcedure->m_bDoneMk[1] = TRUE;
//		m_mgrProcedure->m_bTHREAD_MK[1] = FALSE;
//		break;
//	}
//}

//BOOL CGvisR2R_PunchView::SaveMk1Img(int nMkPcsIdx) // Cam1
//{
//	if (!pDoc->WorkingInfo.System.bSaveMkImg)
//		return TRUE;
//
//	int nSerial;
//	nSerial = m_mgrProcedure->m_nBufUpSerial[1]; // Cam1
//
//	CString sSrc, sDest, sPath;
//	stModelInfo stInfo;
//
//	sSrc.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufUp, nSerial);
//	if (!m_mgrReelmap->GetPcrInfo(sSrc, stInfo))
//	{
//		pView->DispStsBar(_T("E(2)"), 5);
//		pView->ClrDispMsg();
//		AfxMessageBox(_T("Error-GetPcrInfo(2)"));
//		return FALSE;
//	}
//
//	if (!pDoc->MakeMkDir(stInfo))
//	{
//		pView->ClrDispMsg();
//		AfxMessageBox(_T("Error-MakeMkDir()"));
//		return FALSE;
//	}
//
//	//sDest.Format(_T("%s%s\\%s\\%s\\Punching\\%04d"), pDoc->WorkingInfo.System.sPathOldFile, stInfo.sModel,
//	//	stInfo.sLot, stInfo.sLayer, nSerial);
//
//	//if (!pDoc->DirectoryExists(sDest))
//	//	CreateDirectory(sDest, NULL);
//
//	//sPath.Format(_T("%s\\%d.tif"), sDest, ++m_mgrProcedure->m_nSaveMk1Img);
//
//	sDest.Format(_T("%s%s\\%s\\%s\\Punching"), pDoc->WorkingInfo.System.sPathOldFile, stInfo.sModel,
//		stInfo.sLot, stInfo.sLayer);
//
//	if (!pDoc->DirectoryExists(sDest))
//		CreateDirectory(sDest, NULL);
//
//	if (GetTotDefPcs(nSerial) > 0) // Cam1
//	{
//		sPath.Format(_T("%s\\%s.tif"), sDest, GetMkInfo(nSerial, nMkPcsIdx));
//
//#ifdef USE_VISION
//		if (m_mgrPunch->m_pVision[1])
//			return m_mgrPunch->m_pVision[1]->SaveMkImg(sPath);
//#endif
//	}
//	else
//		return TRUE;
//
//	return FALSE;
//}

//void CGvisR2R_PunchView::DoAuto()
//{
//	if (!IsAuto())
//		return;
//
//	CString str;
//	str.Format(_T("%d : %d"), m_mgrProcedure->m_nStepTHREAD_DISP_DEF, m_mgrProcedure->m_bTHREAD_DISP_DEF ? 1 : 0);
//	pView->DispStsBar(str, 6);
//
//	// LotEnd Start
//	if (DoAutoGetLotEndSignal())
//		return;
//
//	if (m_mgrProcedure)
//		m_mgrProcedure->DoAuto();
//}

//BOOL CGvisR2R_PunchView::DoAutoGetLotEndSignal()
//{
//	int nSerial;
//
//	if (m_pDlgMenu01)
//	{
//		if (m_pDlgMenu01->m_bLotEnd && m_mgrProcedure->m_nStepAuto < LOT_END)
//		{
//			m_mgrProcedure->m_bLotEnd = TRUE;
//			m_mgrProcedure->m_nLotEndAuto = LOT_END;
//		}
//	}
//
//	if (!m_mgrProcedure->IsBuffer(0) && m_mgrProcedure->m_bLastProc && m_mgrProcedure->m_nLotEndAuto < LOT_END)
//	{
//		m_mgrProcedure->m_bLotEnd = TRUE;
//		m_mgrProcedure->m_nLotEndAuto = LOT_END;
//	}
//	else if(!m_mgrProcedure->IsBuffer(0) && m_mgrProcedure->m_nMkStAuto > MK_ST + (Mk2PtIdx::DoneMk) + 4)
//	{
//		m_mgrProcedure->m_nMkStAuto = 0;
//		m_mgrProcedure->m_bLotEnd = TRUE;
//		m_mgrProcedure->m_nLotEndAuto = LOT_END;
//	}
//
//
//	if (m_mgrProcedure->m_bLotEnd)
//	{
//		nSerial = pDoc->GetLastShotMk();
//
//		switch (m_mgrProcedure->m_nLotEndAuto)
//		{
//		case LOT_END:
//			if (!m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_UP && !m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_DN 
//				&& !m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_ALLUP && !m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_ALLDN) // Yield Reelmap
//			{
//				m_mgrReelmap->ReloadReelmap(nSerial);
//				UpdateRst();
//				//pDoc->UpdateYieldOnRmap(); // 20230614
//				m_mgrProcedure->m_nLotEndAuto++;
//			}
//			break;
//		case LOT_END + 1:
//			MpeWrite(_T("MB440180"), 1);			// �۾�����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141031
//			DispMain(_T("�۾�����"), RGB_RED);
//			m_mgrProcedure->m_nLotEndAuto++;
//			break;
//		case LOT_END + 2:
//			Stop();
//			//TowerLamp(RGB_YELLOW, TRUE);
//			Buzzer(TRUE, 0);
//			LotEnd();									// MakeResultMDS
//			m_mgrProcedure->m_nLotEndAuto++;
//			break;
//
//		case LOT_END + 3:
//			MsgBox(_T("�۾��� ����Ǿ����ϴ�."));
//			//m_nStepAuto = 0; // �ڵ�����
//			m_mgrProcedure->m_nLotEndAuto++;
//			//m_bLotEnd = FALSE;
//			m_mgrProcedure->m_bLastProc = FALSE;
//			m_mgrProcedure->m_bMkSt = FALSE;
//			::WritePrivateProfileString(_T("Last Job"), _T("MkSt"), _T("0"), PATH_WORKING_INFO);
//			break;
//		case LOT_END + 4:
//			break;
//		}
//	}
//
//	return m_mgrProcedure->m_bLotEnd;
//}

//void CGvisR2R_PunchView::m_mgrPunch->MoveMkInitPos()
//{
//	m_mgrPunch->MoveMk0InitPos();
//	m_mgrPunch->MoveMk1InitPos();
//}
//
//void CGvisR2R_PunchView::m_mgrPunch->MoveMk0InitPos()
//{
//	double pTgtPos[2];
//	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[0]);
//	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[0]);
//	double dCurrX = m_dEnc[AXIS_X0];
//	double dCurrY = m_dEnc[AXIS_Y0];
//
//	double fLen, fVel, fAcc, fJerk;
//	fLen = sqrt(((pTgtPos[0] - dCurrX) * (pTgtPos[0] - dCurrX)) + ((pTgtPos[1] - dCurrY) * (pTgtPos[1] - dCurrY)));
//	if (fLen > 0.001)
//	{
//		m_mgrPunch->m_pMotion->GetSpeedProfile0(TRAPEZOIDAL, AXIS_X0, fLen, fVel, fAcc, fJerk);
//		if (!m_mgrPunch->m_pMotion->Move0(MS_X0Y0, pTgtPos, fVel / 2.0, fAcc / 2.0, fAcc / 2.0))
//		{
//			if (!m_mgrPunch->m_pMotion->Move0(MS_X0Y0, pTgtPos, fVel / 2.0, fAcc / 2.0, fAcc / 2.0))
//			{
//				pView->ClrDispMsg();
//				AfxMessageBox(_T("Move X0Y0 Error..."));
//			}
//		}
//	}
//}
//
//void CGvisR2R_PunchView::m_mgrPunch->MoveMk1InitPos()
//{
//	double pTgtPos[2];
//	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[1]);
//	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[1]);
//	double dCurrX = m_dEnc[AXIS_X1];
//	double dCurrY = m_dEnc[AXIS_Y1];
//
//	double fLen, fVel, fAcc, fJerk;
//	fLen = sqrt(((pTgtPos[0] - dCurrX) * (pTgtPos[0] - dCurrX)) + ((pTgtPos[1] - dCurrY) * (pTgtPos[1] - dCurrY)));
//	if (fLen > 0.001)
//	{
//		m_mgrPunch->m_pMotion->GetSpeedProfile1(TRAPEZOIDAL, AXIS_X1, fLen, fVel, fAcc, fJerk);
//		if (!m_mgrPunch->m_pMotion->Move1(MS_X1Y1, pTgtPos, fVel / 2.0, fAcc / 2.0, fAcc / 2.0))
//		{
//			if (!m_mgrPunch->m_pMotion->Move1(MS_X1Y1, pTgtPos, fVel / 2.0, fAcc / 2.0, fAcc / 2.0))
//			{
//				pView->ClrDispMsg();
//				AfxMessageBox(_T("Move X1Y1 Error..."));
//			}
//		}
//	}
//}

BOOL CGvisR2R_PunchView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message != WM_KEYDOWN)
		return CFormView::PreTranslateMessage(pMsg);

	if ((pMsg->lParam & 0x40000000) == 0)
	{
		switch (pMsg->wParam)
		{
		case VK_RETURN:
			return TRUE;
		case VK_ESCAPE:
			m_bEscape = TRUE;
			return TRUE;
		case 'S':
		case 's':
			if (GetKeyState(VK_CONTROL) < 0) // Ctrl Ű�� ������ ����
			{
				WINDOWPLACEMENT wndPlace;
				AfxGetMainWnd()->GetWindowPlacement(&wndPlace);
				wndPlace.showCmd |= SW_MAXIMIZE;
				AfxGetMainWnd()->SetWindowPlacement(&wndPlace);
			}
			break;
		}
	}

	return CFormView::PreTranslateMessage(pMsg);
}

int CGvisR2R_PunchView::MyPassword(CString strMsg, int nCtrlId)
{
	CDlgMyPassword dlg1(this);
	dlg1.SetMsg(strMsg, nCtrlId);
	dlg1.DoModal();
	return (dlg1.m_nRtnVal);
}

//void CGvisR2R_PunchView::EStop()
//{
//	if (m_mgrPunch->m_pMotion)
//	{
//		m_mgrPunch->m_pMotion->EStop(MS_X0Y0);
//		m_mgrPunch->m_pMotion->EStop(MS_X1Y1);
//		Sleep(30);
//		ResetMotion(MS_X0Y0);
//		Sleep(30);
//		ResetMotion(MS_X1Y1);
//		Sleep(30);
//		// 		DispMsg(_T("X�� �浹 ������ ���� �����Դϴ�."), _T("�˸�"), RGB_GREEN, DELAY_TIME_MSG);
//		pView->ClrDispMsg();
//		AfxMessageBox(_T("X�� �浹 ������ ���� �����Դϴ�."));
//
//		double dCurrX = m_dEnc[AXIS_X1];
//		double dCurrY = m_dEnc[AXIS_Y1];
//		double pPos[2], fVel, fAcc, fJerk;
//		double fLen = 2.0;
//		pPos[0] = dCurrX + fLen;
//		pPos[1] = dCurrY;
//		m_mgrPunch->m_pMotion->GetSpeedProfile1(TRAPEZOIDAL, AXIS_X1, fLen, fVel, fAcc, fJerk);
//		m_mgrPunch->m_pMotion->Move1(MS_X1Y1, pPos, fVel, fAcc, fAcc, ABS, NO_WAIT);
//	}
//}

//BOOL CGvisR2R_PunchView::IsRunAxisX()
//{
//	if (m_mgrPunch->m_pMotion->IsMotionDone(MS_X0) && m_mgrPunch->m_pMotion->IsMotionDone(MS_X1))
//		return FALSE;
//	return TRUE;
//}

//void CGvisR2R_PunchView::StopAllMk()
//{
//	// Mk0
//	m_mgrProcedure->m_nStepMk[2] = MK_END;
//
//	// Mk1
//	m_mgrProcedure->m_nStepMk[3] = MK_END;
//}

//void CGvisR2R_PunchView::DoAllMk(int nCam)
//{
//	if (nCam == CAM_LF)
//	{
//		double dCurrX = m_dEnc[AXIS_X1];
//		double dCurrY = m_dEnc[AXIS_Y1];
//
//		double pPos[2];
//		pPos[0] = _tstof(pDoc->WorkingInfo.Motion.sSafeZone);
//		pPos[1] = 0.0;
//
//		double fLen, fVel, fAcc, fJerk;
//		fLen = sqrt(((pPos[0] - dCurrX) * (pPos[0] - dCurrX)) + ((pPos[1] - dCurrY) * (pPos[1] - dCurrY)));
//		if (fLen > 0.001)
//		{
//			m_mgrPunch->m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_X1, fLen, fVel, fAcc, fJerk);
//			if (!m_mgrPunch->m_pMotion->Move(MS_X1Y1, pPos, fVel, fAcc, fAcc))
//			{
//				if (!m_mgrPunch->m_pMotion->Move(MS_X1Y1, pPos, fVel, fAcc, fAcc))
//				{
//					pView->ClrDispMsg();
//					AfxMessageBox(_T("Move X1Y1 Error..."));
//				}
//			}
//		}
//		if (!m_mgrProcedure->m_bTHREAD_MK[2])
//		{
//			m_mgrProcedure->m_nStepMk[2] = 0;
//			m_mgrProcedure->m_nMkPcs[2] = 0;
//			m_mgrProcedure->m_bTHREAD_MK[2] = TRUE;
//		}
//		else
//		{
//			m_mgrProcedure->m_bTHREAD_MK[2] = FALSE;
//		}
//	}
//	else if (nCam == CAM_RT)
//	{
//		double dCurrX = m_dEnc[AXIS_X0];
//		double dCurrY = m_dEnc[AXIS_Y0];
//
//		double pPos[2];
//		pPos[0] = 0.0;
//		pPos[1] = 0.0;
//
//		double fLen, fVel, fAcc, fJerk;
//		fLen = sqrt(((pPos[0] - dCurrX) * (pPos[0] - dCurrX)) + ((pPos[1] - dCurrY) * (pPos[1] - dCurrY)));
//		if (fLen > 0.001)
//		{
//			m_mgrPunch->m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_X0, fLen, fVel, fAcc, fJerk);
//			if (!m_mgrPunch->m_pMotion->Move(MS_X0Y0, pPos, fVel, fAcc, fAcc))
//			{
//				if (!m_mgrPunch->m_pMotion->Move(MS_X0Y0, pPos, fVel, fAcc, fAcc))
//				{
//					pView->ClrDispMsg();
//					AfxMessageBox(_T("Move X0Y0 Error..."));
//			}
//		}
//		}
//		if (!m_mgrProcedure->m_bTHREAD_MK[3])
//		{
//			m_mgrProcedure->m_nStepMk[3] = 0;
//			m_mgrProcedure->m_nMkPcs[3] = 0;
//			m_mgrProcedure->m_bTHREAD_MK[3] = TRUE;
//		}
//		else
//		{
//			m_mgrProcedure->m_bTHREAD_MK[3] = FALSE;
//		}
//	}
//	else if (nCam == CAM_BOTH)
//	{
//		if (!m_mgrProcedure->m_bTHREAD_MK[2])
//		{
//			m_mgrProcedure->m_nStepMk[2] = 0;
//			m_mgrProcedure->m_nMkPcs[2] = 0;
//			m_mgrProcedure->m_bTHREAD_MK[2] = TRUE;
//		}
//		else
//		{
//			m_mgrProcedure->m_bTHREAD_MK[2] = FALSE;
//		}
//
//		if (!m_mgrProcedure->m_bTHREAD_MK[3])
//		{
//			m_mgrProcedure->m_nStepMk[3] = 0;
//			m_mgrProcedure->m_nMkPcs[3] = 0;
//			m_mgrProcedure->m_bTHREAD_MK[3] = TRUE;
//		}
//		else
//		{
//			m_mgrProcedure->m_bTHREAD_MK[3] = FALSE;
//		}
//	}
//}

//void CGvisR2R_PunchView::InitPLC()
//{
//#ifdef USE_MPE
//	long lData;
//	lData = (long)(_tstof(pDoc->WorkingInfo.Lot.sTotalReelDist) * 1000.0);
//	MpeWrite(_T("ML45000"), lData);	// ��ü Reel ���� (���� M * 1000)
//	lData = (long)(_tstof(pDoc->WorkingInfo.Lot.sSeparateDist) * 1000.0);
//	MpeWrite(_T("ML45002"), lData);	// Lot �и� ���� (���� M * 1000)
//	lData = (long)(_tstof(pDoc->WorkingInfo.Lot.sCuttingDist) * 1000.0);
//	MpeWrite(_T("ML45004"), lData);	// Lot �и� �� ������ġ (���� M * 1000)
//	lData = (long)(_tstof(pDoc->WorkingInfo.Lot.sStopDist) * 1000.0);
//	MpeWrite(_T("ML45006"), lData);	// �Ͻ����� ���� (���� M * 1000)
//	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sAoiTq) * 1000.0);
//	MpeWrite(_T("ML45042"), lData);	// �˻�� Tension ���� ��ũ�� (���� Kgf * 1000)
//	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sMkTq) * 1000.0);
//	MpeWrite(_T("ML45044"), lData);	// ��ŷ�� Tension ���� ��ũ�� (���� Kgf * 1000)
//	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sEngraveTq) * 1000.0);
//	MpeWrite(_T("ML45050"), lData);	// ���κ� Tension ���� ��ũ�� (���� Kgf * 1000)
//	lData = (long)(_tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen) * 1000.0);
//	MpeWrite(_T("ML45032"), lData);	// �� �ǳ� ���� (���� mm * 1000)
//	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sMkFdVel) * 1000.0);
//	MpeWrite(_T("ML45034"), lData);	// �� �ǳ� Feeding �ӵ� (���� mm/sec * 1000)
//	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sMkJogVel) * 1000.0);
//	MpeWrite(_T("ML45038"), lData);	// ���Ӱ��� �ӵ� (���� mm/sec * 1000)
//	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sAoiJogAcc) * 1000.0);
//	MpeWrite(_T("ML45040"), lData);	// ���Ӱ��� ���ӵ� (���� mm/s^2 * 1000)
//	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sMkFdAcc) * 1000.0);
//	MpeWrite(_T("ML45036"), lData);	// �� �ǳ� Feeding ���ӵ� (���� mm/s^2 * 1000)
//	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sAoiFdLead) * 1000.0);
//	MpeWrite(_T("ML45012"), lData);	// �˻�� Feeding �ѷ� Lead Pitch (���� mm * 1000)
//	MpeWrite(_T("ML45020"), lData);	// ���κ� Feeding �ѷ� Lead Pitch (���� mm * 1000)
//	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sMkFdLead) * 1000.0);
//	MpeWrite(_T("ML45014"), lData);	// ��ŷ�� Feeding �ѷ� Lead Pitch (���� mm * 1000)
//	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sStBufPos) * 1000.0);
//	MpeWrite(_T("ML45016"), lData);	// ���� ���� ���� �ѷ� �ʱ���ġ(���� mm * 1000)
//	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sFdMkAoiInitDist) * 1000.0);
//	MpeWrite(_T("ML45008"), lData);	// AOI(��)���� ��ŷ���� �Ÿ� (���� mm * 1000)
//	lData = (long)(_tstoi(pDoc->WorkingInfo.Motion.sFdAoiAoiDistShot) * 1000);
//	MpeWrite(_T("ML45010"), lData);	// AOI(��)���� AOI(��) Shot�� (���� Shot�� * 1000)
//	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sFdEngraveAoiInitDist) * 1000.0);
//	MpeWrite(_T("ML45024"), lData);	// ���κο��� AOI(��)���� �Ÿ� (���� mm * 1000)
//	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sFdBarcodeOffsetVel) * 1000.0);
//	MpeWrite(_T("ML45060"), lData);	// 2D ���ڵ� ��������ġ���� Feeding �ӵ� (���� mm/sec * 1000)
//	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sFdBarcodeOffsetAcc) * 1000.0);
//	MpeWrite(_T("ML45062"), lData);	// 2D ���ڵ� ��������ġ���� Feeding ���ӵ� (���� mm/s^2 * 1000)
//
//	lData = (long)(_tstof(pDoc->WorkingInfo.LastJob.sUltraSonicCleannerStTim) * 100.0);
//	MpeWrite(_T("MW05940"), lData);	// AOI_Dn (���� [��] * 100) : 1 is 10 mSec.
//	MpeWrite(_T("MW05942"), lData);	// AOI_Up (���� [��] * 100) : 1 is 10 mSec.
//#endif
//}

BOOL CGvisR2R_PunchView::SetCollision(double dCollisionMargin)
{
	BOOL bRtn = FALSE;
	if (m_mgrPunch->m_pMotion)
		bRtn = m_mgrPunch->m_pMotion->SetCollision(dCollisionMargin);
	return bRtn;
}

//void CGvisR2R_PunchView::MpeWrite()
//{
//	for (int i = TOT_M_IO - 1; i >= 0; i--)
//	{
//		if (pDoc->m_pIo[i].nIdx > -1)
//		{
//			if (m_pMpe)
//				MpeWrite(pDoc->m_pIo[i].sAddr, pDoc->m_pIo[i].lData);
//			pDoc->m_pIo[i].nIdx = -1;
//			pDoc->m_pIo[i].sAddr = _T("");
//			pDoc->m_pIo[i].lData = 0;
//		}
//	}
//}

//void CGvisR2R_PunchView::IoWrite(CString sMReg, long lData)
//{
//	int i = 0;
//	for (i = 0; i<TOT_M_IO; i++)
//	{
//		if (pDoc->m_pIo[i].nIdx < 0)
//		{
//			pDoc->m_pIo[i].nIdx = i;
//			pDoc->m_pIo[i].sAddr = sMReg;
//			pDoc->m_pIo[i].lData = lData;
//			break;
//		}
//	}
//	if (i >= TOT_M_IO)
//	{
//		pView->ClrDispMsg();
//		AfxMessageBox(_T("Not enought TOT_M_IO Num!!!"));
//	}
//}

BOOL CGvisR2R_PunchView::IsRdyTest()
{
	BOOL bOn0 = IsRdyTest0();
	BOOL bOn1 = IsRdyTest1();

	if (bOn0 && bOn1)
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_PunchView::IsRdyTest0()
{
#ifdef USE_MPE
	BOOL bOn0 = (pDoc->m_pMpeIb[10] & (0x01 << 11)) ? TRUE : FALSE;		// �˻�� �� �ڵ� ���� <-> X432B I/F
	BOOL bOn1 = (pDoc->m_pMpeIb[10] & (0x01 << 9)) ? TRUE : FALSE;		// �˻�� �� ���̺� ���� �Ϸ� <-> X4329 I/F

	if (bOn0 && bOn1)
		return TRUE;
#endif
	return FALSE;
}

BOOL CGvisR2R_PunchView::IsRdyTest1()
{
#ifdef USE_MPE
	BOOL bOn0 = (pDoc->m_pMpeIb[14] & (0x01 << 11)) ? TRUE : FALSE;		// �˻�� �� �ڵ� ���� <-> X442B I/F
	BOOL bOn1 = (pDoc->m_pMpeIb[14] & (0x01 << 9)) ? TRUE : FALSE;		// �˻�� �� ���̺� ���� �Ϸ� <-> X4329 I/F
	double dCurPosMkFd = (double)pDoc->m_pMpeData[0][0];				// ��ŷ�� Feeding ���ڴ� ��(���� mm )
	double dTgtFd = _tstof(pDoc->WorkingInfo.Motion.sFdAoiAoiDistShot) * _tstof(pDoc->WorkingInfo.Motion.sAoiFdDist);
	if (dCurPosMkFd > dTgtFd - _tstof(pDoc->WorkingInfo.Motion.sAoiFdDist) / 2.0)
	{
		BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
		if (bDualTest)
		{
			if (bOn0 && bOn1)
				return TRUE;
		}
		else
		{
			if (bOn0)
				return TRUE;
		}
	}
#endif
	return FALSE;
}

//BOOL CGvisR2R_PunchView::IsMk0Done()
//{
//	BOOL bDone = FALSE;
//
//	if (pView->m_mgrPunch->m_pVoiceCoil[0])
//		bDone = pView->m_mgrPunch->m_pVoiceCoil[0]->IsDoneMark(0);
//
//	return bDone;
//}
//
//BOOL CGvisR2R_PunchView::IsMk1Done()
//{
//	BOOL bDone = FALSE;
//
//	if (pView->m_mgrPunch->m_pVoiceCoil[1])
//		bDone = pView->m_mgrPunch->m_pVoiceCoil[1]->IsDoneMark(1);
//
//	return bDone;
//}

//void CGvisR2R_PunchView::SetPlcParam()
//{
//#ifdef USE_MPE
//	if (!m_pMpe)
//		return;
//
//	//long lData;
//	MpeWrite(_T("ML45006"), long(_tstof(pDoc->WorkingInfo.LastJob.sTempPauseLen)*1000.0));	// �Ͻ����� ���� (���� M * 1000)
//	MpeWrite(_T("ML45002"), long(_tstof(pDoc->WorkingInfo.LastJob.sLotSepLen)*1000.0));	// Lot �и� ���� (���� M * 1000)
//	MpeWrite(_T("ML45004"), long(_tstof(pDoc->WorkingInfo.LastJob.sLotCutPosLen)*1000.0));	// Lot �и� �� ������ġ (���� M * 1000)
//	MpeWrite(_T("ML45126"), (long)_tstoi(pDoc->WorkingInfo.LastJob.sSampleTestShotNum));	// ���ð˻� Shot��
//
//	if (pDoc->WorkingInfo.LastJob.bTempPause)
//	{
//		MpeWrite(_T("MB440183"), 1);	// �Ͻ��������(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141031
//		ChkTempStop(TRUE);
//	}
//	else
//	{
//		MpeWrite(_T("MB440183"), 0);	// �Ͻ��������(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141031
//		ChkTempStop(FALSE);
//	}
//
//	if (pDoc->WorkingInfo.LastJob.bLotSep)
//	{
//		SetLotLastShot();
//		MpeWrite(_T("MB440184"), 1);	// ��Ʈ�и����(PC�� On��Ű��, PC�� Ȯ���ϰ� Off��Ŵ)-20141031
//	}
//	else
//		MpeWrite(_T("MB440184"), 0);	// ��Ʈ�и����(PC�� On��Ű��, PC�� Ȯ���ϰ� Off��Ŵ)-20141031
//
//	MpeWrite(_T("MB44017A"), (pDoc->WorkingInfo.LastJob.bDualTest) ? 0 : 1);		// �ܸ� �˻� On
//	MpeWrite(_T("MB44017B"), (pDoc->WorkingInfo.LastJob.bSampleTest) ? 1 : 0);		// Sample �˻� On
//	MpeWrite(_T("MB44017D"), (pDoc->WorkingInfo.LastJob.bOneMetal) ? 1 : 0);		// One Metal On
//	MpeWrite(_T("MB44017C"), (pDoc->WorkingInfo.LastJob.bTwoMetal) ? 1 : 0);		// Two Metal On
//	MpeWrite(_T("MB44017E"), (pDoc->WorkingInfo.LastJob.bCore150Recoiler) ? 1 : 0);	// Recoiler Core 150mm On
//	MpeWrite(_T("MB44017F"), (pDoc->WorkingInfo.LastJob.bCore150Uncoiler) ? 1 : 0);	// Uncoiler Core 150mm On
//
//	MpeWrite(_T("MB44010E"), (pDoc->WorkingInfo.LastJob.bUseAoiUpCleanRoler ? 1 : 0));
//	MpeWrite(_T("MB44010F"), (pDoc->WorkingInfo.LastJob.bUseAoiDnCleanRoler ? 1 : 0));
//#endif
//}

//void CGvisR2R_PunchView::GetPlcParam()
//{
//	if (!m_pMpe)
//		return;
//#ifdef USE_MPE
//	// Main
//	pDoc->BtnStatus.Main.Ready = m_pMpe->Read(_T("MB005503")) ? TRUE : FALSE;	// ��ŷ�� �����غ� ����ġ
//	pDoc->BtnStatus.Main.Run = m_pMpe->Read(_T("MB005501")) ? TRUE : FALSE;		// ��ŷ�� ���� ����ġ
//	pDoc->BtnStatus.Main.Reset = m_pMpe->Read(_T("MB005504")) ? TRUE : FALSE;	// ��ŷ�� ���� ����ġ
//	pDoc->BtnStatus.Main.Stop = m_pMpe->Read(_T("MB005502")) ? TRUE : FALSE;	// ��ŷ�� ���� ����ġ
//	//pDoc->BtnStatus.Main.Auto = m_pMpe->Read(_T("MB005505")) ? TRUE : FALSE;	// ��ŷ�� �ڵ�/���� (ON)
//	//pDoc->BtnStatus.Main.Manual = m_pMpe->Read(_T("MB005505")) ? FALSE : TRUE;	// ��ŷ�� �ڵ�/���� (OFF)
//	pDoc->BtnStatus.Main.Auto = m_pMpe->Read(_T("MB003705")) ? TRUE : FALSE;	// ��ŷ�� �ڵ� ���� ����ġ ����
//	pDoc->BtnStatus.Main.Manual = m_pMpe->Read(_T("MB003705")) ? FALSE : TRUE;	// ��ŷ�� �ڵ� ���� ����ġ ����
//
//	if (pDoc->BtnStatus.Main.PrevReady != pDoc->BtnStatus.Main.Ready)
//	{
//		pDoc->BtnStatus.Main.PrevReady = pDoc->BtnStatus.Main.Ready;
//		pDoc->SetMkMenu03(_T("Main"), _T("Ready"), pDoc->BtnStatus.Main.Ready);
//	}
//	if (pDoc->BtnStatus.Main.PrevRun != pDoc->BtnStatus.Main.Run)
//	{
//		pDoc->BtnStatus.Main.PrevRun = pDoc->BtnStatus.Main.Run;
//		pDoc->SetMkMenu03(_T("Main"), _T("Run"), pDoc->BtnStatus.Main.Run);
//		pDoc->SetMkMenu03(_T("Main"), _T("Stop"), !pDoc->BtnStatus.Main.Run);
//	}
//	if (pDoc->BtnStatus.Main.PrevReset != pDoc->BtnStatus.Main.Reset)
//	{
//		pDoc->BtnStatus.Main.PrevReset = pDoc->BtnStatus.Main.Reset;
//		pDoc->SetMkMenu03(_T("Main"), _T("Reset"), pDoc->BtnStatus.Main.Reset);
//	}
//	if (pDoc->BtnStatus.Main.PrevStop != pDoc->BtnStatus.Main.Stop)
//	{
//		pDoc->BtnStatus.Main.PrevStop = pDoc->BtnStatus.Main.Stop;
//		pDoc->SetMkMenu03(_T("Main"), _T("Stop"), pDoc->BtnStatus.Main.Stop);
//		pDoc->SetMkMenu03(_T("Main"), _T("Run"), !pDoc->BtnStatus.Main.Stop);
//	}
//	if (pDoc->BtnStatus.Main.PrevAuto != pDoc->BtnStatus.Main.Auto)
//	{
//		pDoc->BtnStatus.Main.PrevAuto = pDoc->BtnStatus.Main.Auto;
//		pDoc->SetMkMenu03(_T("Main"), _T("Auto"), pDoc->BtnStatus.Main.Auto);
//		pDoc->SetMkMenu03(_T("Main"), _T("Manual"), !pDoc->BtnStatus.Main.Auto);
//	}
//	if (pDoc->BtnStatus.Main.PrevManual != pDoc->BtnStatus.Main.Manual)
//	{
//		pDoc->BtnStatus.Main.PrevManual = pDoc->BtnStatus.Main.Manual;
//		pDoc->SetMkMenu03(_T("Main"), _T("Manual"), pDoc->BtnStatus.Main.Manual);
//		pDoc->SetMkMenu03(_T("Main"), _T("Auto"), !pDoc->BtnStatus.Main.Manual);
//	}
//
//	// TorqueMotor
//	pDoc->BtnStatus.Tq.Mk = m_pMpe->Read(_T("MB440155")) ? TRUE : FALSE;
//	pDoc->BtnStatus.Tq.Aoi = m_pMpe->Read(_T("MB440156")) ? TRUE : FALSE;
//	pDoc->BtnStatus.Tq.Eng = m_pMpe->Read(_T("MB440154")) ? TRUE : FALSE;
//
//	if (pDoc->BtnStatus.Tq.PrevMk != pDoc->BtnStatus.Tq.Mk)
//	{
//		pDoc->BtnStatus.Tq.PrevMk = pDoc->BtnStatus.Tq.Mk;
//		pDoc->SetMkMenu03(_T("TqMotor"), _T("MkTq"), pDoc->BtnStatus.Tq.Mk);
//	}
//	if (pDoc->BtnStatus.Tq.PrevAoi != pDoc->BtnStatus.Tq.Aoi)
//	{
//		pDoc->BtnStatus.Tq.PrevAoi = pDoc->BtnStatus.Tq.Aoi;
//		pDoc->SetMkMenu03(_T("TqMotor"), _T("AoiTq"), pDoc->BtnStatus.Tq.Aoi);
//	}
//	if (pDoc->BtnStatus.Tq.PrevEng != pDoc->BtnStatus.Tq.Eng)
//	{
//		pDoc->BtnStatus.Tq.PrevEng = pDoc->BtnStatus.Tq.Eng;
//		pDoc->SetMkMenu03(_T("TqMotor"), _T("EngTq"), pDoc->BtnStatus.Tq.Eng);
//	}
//
//	// InductionMotor
//	pDoc->BtnStatus.Induct.Rc = m_pMpe->Read(_T("MB44017D")) ? TRUE : FALSE;	//pView->SetTwoMetal(FALSE, TRUE);	// One Metal IDC_CHK_68		
//	pDoc->BtnStatus.Induct.Uc = m_pMpe->Read(_T("MB44017C")) ? TRUE : FALSE;	//pView->SetTwoMetal(TRUE, TRUE);	// Two Metal IDC_CHK_69
//
//	if (pDoc->BtnStatus.Induct.PrevRc != pDoc->BtnStatus.Induct.Rc)
//	{
//		pDoc->BtnStatus.Induct.PrevRc = pDoc->BtnStatus.Induct.Rc;
//		pDoc->SetMkMenu03(_T("Induction"), _T("RcCcw"), pDoc->BtnStatus.Induct.Rc);
//	}
//	if (pDoc->BtnStatus.Induct.PrevUc != pDoc->BtnStatus.Induct.Uc)
//	{
//		pDoc->BtnStatus.Induct.PrevUc = pDoc->BtnStatus.Induct.Uc;
//		pDoc->SetMkMenu03(_T("Induction"), _T("UcCcw"), pDoc->BtnStatus.Induct.Uc);
//	}
//
//	// Core150mm
//	pDoc->BtnStatus.Core150.Rc = m_pMpe->Read(_T("MB44017E")) ? TRUE : FALSE;	// SetCore150mmRecoiler(TRUE);	// Recoiler IDC_CHK_70	
//	pDoc->BtnStatus.Core150.Uc = m_pMpe->Read(_T("MB44017F")) ? TRUE : FALSE;	// SetCore150mmUncoiler(TRUE);	// Uncoiler IDC_CHK_71	
//
//	if (pDoc->BtnStatus.Core150.PrevRc != pDoc->BtnStatus.Core150.Rc)
//	{
//		pDoc->BtnStatus.Core150.PrevRc = pDoc->BtnStatus.Core150.Rc;
//		pDoc->SetMkMenu03(_T("Core"), _T("Rc150"), pDoc->BtnStatus.Core150.Rc);
//	}
//	if (pDoc->BtnStatus.Core150.PrevUc != pDoc->BtnStatus.Core150.Uc)
//	{
//		pDoc->BtnStatus.Core150.PrevUc = pDoc->BtnStatus.Core150.Uc;
//		pDoc->SetMkMenu03(_T("Core"), _T("Uc150"), pDoc->BtnStatus.Core150.Uc);
//	}
//
//	// Etc
//	//pDoc->BtnStatus.Etc.EmgAoi = m_pMpe->Read(_T("")) ? TRUE : FALSE;
//
//	// Recoiler
//	pDoc->BtnStatus.Rc.Relation = m_pMpe->Read(_T("MB004001")) ? TRUE : FALSE;		// �����Ϸ� ���� ��/���� ����ġ ���� 
//	pDoc->BtnStatus.Rc.FdCw = m_pMpe->Read(_T("MB00400C")) ? TRUE : FALSE;			// �����Ϸ� ��ǰ�� ��ȸ�� ����ġ ����
//	pDoc->BtnStatus.Rc.FdCcw = m_pMpe->Read(_T("MB00400D")) ? TRUE : FALSE;			// �����Ϸ� ��ǰ�� ��ȸ�� ����ġ ����
//	pDoc->BtnStatus.Rc.ReelChuck = m_pMpe->Read(_T("MB00400B")) ? TRUE : FALSE;		// �����Ϸ� ��ǰô Ŭ���� ����ġ ����
//	pDoc->BtnStatus.Rc.DcRlUpDn = m_pMpe->Read(_T("MB004002")) ? TRUE : FALSE;		// �����Ϸ� ���� ���/�ϰ� ����ġ ����
//	pDoc->BtnStatus.Rc.ReelJoinL = m_pMpe->Read(_T("MB004005")) ? TRUE : FALSE;		// �����Ϸ� ��ǰ ������(��/��) ����ġ ����
//	pDoc->BtnStatus.Rc.ReelJoinR = m_pMpe->Read(_T("MB004006")) ? TRUE : FALSE;		// �����Ϸ� ��ǰ ������(��/��) ����ġ ����
//	pDoc->BtnStatus.Rc.ReelJoinVac = m_pMpe->Read(_T("MB00400F")) ? TRUE : FALSE;	// �����Ϸ� ��ǰ ������ ���� ����ġ ����
//	pDoc->BtnStatus.Rc.PprChuck = m_pMpe->Read(_T("MB004008")) ? TRUE : FALSE;		// �����Ϸ� ����ô Ŭ���� ����ġ ����
//	pDoc->BtnStatus.Rc.PprCw = m_pMpe->Read(_T("MB004009")) ? TRUE : FALSE;			// �����Ϸ� ������ ��ȸ�� ����ġ ����
//	pDoc->BtnStatus.Rc.PprCcw = m_pMpe->Read(_T("MB00400A")) ? TRUE : FALSE;		// �����Ϸ� ������ ��ȸ�� ����ġ ����
//	pDoc->BtnStatus.Rc.Rewine = m_pMpe->Read(_T("MB004003")) ? TRUE : FALSE;		// �����Ϸ� Rewinder ���� ����ġ ����
//	pDoc->BtnStatus.Rc.RewineReelPpr = m_pMpe->Read(_T("MB004004")) ? TRUE : FALSE;	// �����Ϸ� Rewinder ��ǰ & ���� ����ġ ����
//
//	if (pDoc->BtnStatus.Rc.PrevRelation != pDoc->BtnStatus.Rc.Relation)
//	{
//		pDoc->BtnStatus.Rc.PrevRelation = pDoc->BtnStatus.Rc.Relation;
//		pDoc->SetMkMenu03(_T("Main"), _T("Relation"), pDoc->BtnStatus.Rc.Relation);
//	}
//	if (pDoc->BtnStatus.Rc.PrevFdCw != pDoc->BtnStatus.Rc.FdCw)
//	{
//		pDoc->BtnStatus.Rc.PrevFdCw = pDoc->BtnStatus.Rc.FdCw;
//		pDoc->SetMkMenu03(_T("Recoiler"), _T("MvCw"), pDoc->BtnStatus.Rc.FdCw);
//	}
//	if (pDoc->BtnStatus.Rc.PrevFdCcw != pDoc->BtnStatus.Rc.FdCcw)
//	{
//		pDoc->BtnStatus.Rc.PrevFdCcw = pDoc->BtnStatus.Rc.FdCcw;
//		pDoc->SetMkMenu03(_T("Recoiler"), _T("MvCcw"), pDoc->BtnStatus.Rc.FdCcw);
//	}
//	if (pDoc->BtnStatus.Rc.PrevReelChuck != pDoc->BtnStatus.Rc.ReelChuck)
//	{
//		pDoc->BtnStatus.Rc.PrevReelChuck = pDoc->BtnStatus.Rc.ReelChuck;
//		pDoc->SetMkMenu03(_T("Recoiler"), _T("PrdChuck"), pDoc->BtnStatus.Rc.ReelChuck);
//	}
//	if (pDoc->BtnStatus.Rc.PrevDcRlUpDn != pDoc->BtnStatus.Rc.DcRlUpDn)
//	{
//		pDoc->BtnStatus.Rc.PrevDcRlUpDn = pDoc->BtnStatus.Rc.DcRlUpDn;
//		pDoc->SetMkMenu03(_T("Recoiler"), _T("DancerUp"), pDoc->BtnStatus.Rc.DcRlUpDn);
//	}
//	if (pDoc->BtnStatus.Rc.PrevReelJoinL != pDoc->BtnStatus.Rc.ReelJoinL)
//	{
//		pDoc->BtnStatus.Rc.PrevReelJoinL = pDoc->BtnStatus.Rc.ReelJoinL;
//		pDoc->SetMkMenu03(_T("Recoiler"), _T("PasteUpLf"), pDoc->BtnStatus.Rc.ReelJoinL);
//	}
//	if (pDoc->BtnStatus.Rc.PrevReelJoinR != pDoc->BtnStatus.Rc.ReelJoinR)
//	{
//		pDoc->BtnStatus.Rc.PrevReelJoinR = pDoc->BtnStatus.Rc.ReelJoinR;
//		pDoc->SetMkMenu03(_T("Recoiler"), _T("PasteUpRt"), pDoc->BtnStatus.Rc.ReelJoinR);
//	}
//	if (pDoc->BtnStatus.Rc.PrevReelJoinVac != pDoc->BtnStatus.Rc.ReelJoinVac)
//	{
//		pDoc->BtnStatus.Rc.PrevReelJoinVac = pDoc->BtnStatus.Rc.ReelJoinVac;
//		pDoc->SetMkMenu03(_T("Recoiler"), _T("PasteVac"), pDoc->BtnStatus.Rc.ReelJoinVac);
//	}
//	if (pDoc->BtnStatus.Rc.PrevPprChuck != pDoc->BtnStatus.Rc.PprChuck)
//	{
//		pDoc->BtnStatus.Rc.PrevPprChuck = pDoc->BtnStatus.Rc.PprChuck;
//		pDoc->SetMkMenu03(_T("Recoiler"), _T("PprChuck"), pDoc->BtnStatus.Rc.PprChuck);
//	}
//	if (pDoc->BtnStatus.Rc.PrevPprCw != pDoc->BtnStatus.Rc.PprCw)
//	{
//		pDoc->BtnStatus.Rc.PrevPprCw = pDoc->BtnStatus.Rc.PprCw;
//		pDoc->SetMkMenu03(_T("Recoiler"), _T("PprCw"), pDoc->BtnStatus.Rc.PprCw);
//	}
//	if (pDoc->BtnStatus.Rc.PrevPprCcw != pDoc->BtnStatus.Rc.PprCcw)
//	{
//		pDoc->BtnStatus.Rc.PrevPprCcw = pDoc->BtnStatus.Rc.PprCcw;
//		pDoc->SetMkMenu03(_T("Recoiler"), _T("PprCcw"), pDoc->BtnStatus.Rc.PprCcw);
//	}
//	if (pDoc->BtnStatus.Rc.PrevRewine != pDoc->BtnStatus.Rc.Rewine)
//	{
//		pDoc->BtnStatus.Rc.PrevRewine = pDoc->BtnStatus.Rc.Rewine;
//		pDoc->SetMkMenu03(_T("Recoiler"), _T("DoRewind"), pDoc->BtnStatus.Rc.Rewine);
//	}
//	if (pDoc->BtnStatus.Rc.PrevRewineReelPpr != pDoc->BtnStatus.Rc.RewineReelPpr)
//	{
//		pDoc->BtnStatus.Rc.PrevRewineReelPpr = pDoc->BtnStatus.Rc.RewineReelPpr;
//		pDoc->SetMkMenu03(_T("Recoiler"), _T("PrdPprRewind"), pDoc->BtnStatus.Rc.RewineReelPpr);
//	}
//
//	//pDoc->BtnStatus.Rc.Relation = m_pMpe->Read(_T("MB005801")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.Rc.FdCw = m_pMpe->Read(_T("MB00580C")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.Rc.FdCcw = m_pMpe->Read(_T("MB00580D")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.Rc.ReelChuck = m_pMpe->Read(_T("MB00580B")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.Rc.DcRlUpDn = m_pMpe->Read(_T("MB005802")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.Rc.ReelJoinL = m_pMpe->Read(_T("MB005805")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.Rc.ReelJoinR = m_pMpe->Read(_T("MB005806")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.Rc.ReelJoinVac = m_pMpe->Read(_T("MB00580F")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.Rc.PprChuck = m_pMpe->Read(_T("MB005808")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.Rc.PprCw = m_pMpe->Read(_T("MB005809")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.Rc.PprCcw = m_pMpe->Read(_T("MB00580A")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.Rc.Rewine = m_pMpe->Read(_T("MB005803")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.Rc.RewineReelPpr = m_pMpe->Read(_T("MB005804")) ? TRUE : FALSE;
//
//	// Punch
//	pDoc->BtnStatus.Mk.Relation = m_pMpe->Read(_T("MB003711")) ? TRUE : FALSE;	// ��ŷ�� ���� ��/���� ����ġ ����
//	pDoc->BtnStatus.Mk.FdCw = m_pMpe->Read(_T("MB003713")) ? TRUE : FALSE;		// ��ŷ�� �ǵ� ��ȸ�� ����ġ ����
//	pDoc->BtnStatus.Mk.FdCcw = m_pMpe->Read(_T("MB003714")) ? TRUE : FALSE;		// ��ŷ�� �ǵ� ��ȸ�� ����ġ ����
//	pDoc->BtnStatus.Mk.FdVac = m_pMpe->Read(_T("MB003715")) ? TRUE : FALSE;		// ��ŷ�� �ǵ� ���� ����ġ ����
//	pDoc->BtnStatus.Mk.PushUp = m_pMpe->Read(_T("MB003712")) ? TRUE : FALSE;	// ��ŷ�� ���̺� ��ο� ����ġ ����
//	pDoc->BtnStatus.Mk.TblBlw = m_pMpe->Read(_T("MB003717")) ? TRUE : FALSE;	// ��ŷ�� ���̺� ���� ����ġ ����
//	pDoc->BtnStatus.Mk.TblVac = m_pMpe->Read(_T("MB003716")) ? TRUE : FALSE;	// ��ŷ�� ��ũ ���� ����ġ ����
//	pDoc->BtnStatus.Mk.FdClp = m_pMpe->Read(_T("MB003719")) ? TRUE : FALSE;		// ��ŷ�� �ǵ� Ŭ���� ����ġ ����  
//	pDoc->BtnStatus.Mk.TqClp = m_pMpe->Read(_T("MB00371A")) ? TRUE : FALSE;		// ��ŷ�� �ټ� Ŭ���� ����ġ ����
//	pDoc->BtnStatus.Mk.MvOne = m_pMpe->Read(_T("MB440151")) ? TRUE : FALSE;
//	pDoc->BtnStatus.Mk.LsrPt = m_pMpe->Read(_T("MB003718")) ? TRUE : FALSE;		// ��ŷ�� ������ ������ ����ġ ����
//	pDoc->BtnStatus.Mk.DcRSol = m_pMpe->Read(_T("MB00371B")) ? TRUE : FALSE;	// ��ŷ�� ���� ���/�ϰ� ����ġ ����
//
//	if (pDoc->BtnStatus.Mk.PrevRelation != pDoc->BtnStatus.Mk.Relation)
//	{
//		pDoc->BtnStatus.Mk.PrevRelation = pDoc->BtnStatus.Mk.Relation;
//		pDoc->SetMkMenu03(_T("Main"), _T("Relation"), pDoc->BtnStatus.Mk.Relation);
//	}
//	if (pDoc->BtnStatus.Mk.PrevFdCw != pDoc->BtnStatus.Mk.FdCw)
//	{
//		pDoc->BtnStatus.Mk.PrevFdCw = pDoc->BtnStatus.Mk.FdCw;
//		pDoc->SetMkMenu03(_T("Punching"), _T("MvCw"), pDoc->BtnStatus.Mk.FdCw);
//	}
//	if (pDoc->BtnStatus.Mk.PrevFdCcw != pDoc->BtnStatus.Mk.FdCcw)
//	{
//		pDoc->BtnStatus.Mk.PrevFdCcw = pDoc->BtnStatus.Mk.FdCcw;
//		pDoc->SetMkMenu03(_T("Punching"), _T("MvCcw"), pDoc->BtnStatus.Mk.FdCcw);
//	}
//	if (pDoc->BtnStatus.Mk.PrevFdVac != pDoc->BtnStatus.Mk.FdVac)
//	{
//		pDoc->BtnStatus.Mk.PrevFdVac = pDoc->BtnStatus.Mk.FdVac;
//		pDoc->SetMkMenu03(_T("Punching"), _T("FdVac"), pDoc->BtnStatus.Mk.FdVac);
//	}
//	if (pDoc->BtnStatus.Mk.PrevPushUp != pDoc->BtnStatus.Mk.PushUp)
//	{
//		pDoc->BtnStatus.Mk.PrevPushUp = pDoc->BtnStatus.Mk.PushUp;
//		pDoc->SetMkMenu03(_T("Punching"), _T("PushUp"), pDoc->BtnStatus.Mk.PushUp);
//	}
//	if (pDoc->BtnStatus.Mk.PrevTblBlw != pDoc->BtnStatus.Mk.TblBlw)
//	{
//		pDoc->BtnStatus.Mk.PrevTblBlw = pDoc->BtnStatus.Mk.TblBlw;
//		pDoc->SetMkMenu03(_T("Punching"), _T("TblBlw"), pDoc->BtnStatus.Mk.TblBlw);
//	}
//	if (pDoc->BtnStatus.Mk.PrevTblVac != pDoc->BtnStatus.Mk.TblVac)
//	{
//		pDoc->BtnStatus.Mk.PrevTblVac = pDoc->BtnStatus.Mk.TblVac;
//		pDoc->SetMkMenu03(_T("Punching"), _T("TblVac"), pDoc->BtnStatus.Mk.TblVac);
//	}
//	if (pDoc->BtnStatus.Mk.PrevFdClp != pDoc->BtnStatus.Mk.FdClp)
//	{
//		pDoc->BtnStatus.Mk.PrevFdClp = pDoc->BtnStatus.Mk.FdClp;
//		pDoc->SetMkMenu03(_T("Punching"), _T("FdClamp"), pDoc->BtnStatus.Mk.FdClp);
//	}
//	if (pDoc->BtnStatus.Mk.PrevTqClp != pDoc->BtnStatus.Mk.TqClp)
//	{
//		pDoc->BtnStatus.Mk.PrevTqClp = pDoc->BtnStatus.Mk.TqClp;
//		pDoc->SetMkMenu03(_T("Punching"), _T("TensClamp"), pDoc->BtnStatus.Mk.TqClp);
//	}
//	if (pDoc->BtnStatus.Mk.PrevMvOne != pDoc->BtnStatus.Mk.MvOne)
//	{
//		pDoc->BtnStatus.Mk.PrevMvOne = pDoc->BtnStatus.Mk.MvOne;
//		pDoc->SetMkMenu03(_T("Punching"), _T("OnePnl"), pDoc->BtnStatus.Mk.MvOne);
//	}
//	if (pDoc->BtnStatus.Mk.PrevLsrPt != pDoc->BtnStatus.Mk.LsrPt)
//	{
//		pDoc->BtnStatus.Mk.PrevLsrPt = pDoc->BtnStatus.Mk.LsrPt;
//		pDoc->SetMkMenu03(_T("Punching"), _T("Lsr"), pDoc->BtnStatus.Mk.LsrPt);
//	}
//	if (pDoc->BtnStatus.Mk.PrevDcRSol != pDoc->BtnStatus.Mk.DcRSol)
//	{
//		pDoc->BtnStatus.Mk.PrevDcRSol = pDoc->BtnStatus.Mk.DcRSol;
//		pDoc->SetMkMenu03(_T("Punching"), _T("DancerUp"), pDoc->BtnStatus.Mk.DcRSol);
//	}
//
//	//pDoc->BtnStatus.Mk.Relation = m_pMpe->Read(_T("MB005511")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.Mk.FdCw = m_pMpe->Read(_T("MB005513")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.Mk.FdCcw = m_pMpe->Read(_T("MB005514")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.Mk.FdVac = m_pMpe->Read(_T("MB005515")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.Mk.PushUp = m_pMpe->Read(_T("MB005516")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.Mk.TblBlw = m_pMpe->Read(_T("MB005512")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.Mk.TblVac = m_pMpe->Read(_T("MB005517")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.Mk.FdClp = m_pMpe->Read(_T("MB005519")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.Mk.TqClp = m_pMpe->Read(_T("MB00551A")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.Mk.MvOne = m_pMpe->Read(_T("MB440151")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.Mk.LsrPt = m_pMpe->Read(_T("MB005518")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.Mk.DcRSol = m_pMpe->Read(_T("MB00551B")) ? TRUE : FALSE;
//
//	// AOIDn
//	pDoc->BtnStatus.AoiDn.Relation = m_pMpe->Read(_T("MB003901")) ? TRUE : FALSE;	// �˻�� �� ���� ��/���� ����ġ ����
//	pDoc->BtnStatus.AoiDn.FdCw = m_pMpe->Read(_T("MB003903")) ? TRUE : FALSE;		// �˻�� �� �ǵ� ��ȸ�� ����ġ ����
//	pDoc->BtnStatus.AoiDn.FdCcw = m_pMpe->Read(_T("MB003904")) ? TRUE : FALSE;		// �˻�� �� �ǵ� ��ȸ�� ����ġ ����
//	pDoc->BtnStatus.AoiDn.FdVac = m_pMpe->Read(_T("MB003905")) ? TRUE : FALSE;		// �˻�� �� �ǵ� ���� ����ġ ����
//	pDoc->BtnStatus.AoiDn.PushUp = m_pMpe->Read(_T("MB003906")) ? TRUE : FALSE;		// �˻�� �� ��ũ ���� ����ġ ����
//	pDoc->BtnStatus.AoiDn.TblBlw = m_pMpe->Read(_T("MB003902")) ? TRUE : FALSE;		// �˻�� �� ���̺� ��ο� ����ġ ����
//	pDoc->BtnStatus.AoiDn.TblVac = m_pMpe->Read(_T("MB003907")) ? TRUE : FALSE;		// �˻�� �� ���̺� ���� ����ġ ����
//	pDoc->BtnStatus.AoiDn.FdClp = m_pMpe->Read(_T("MB003909")) ? TRUE : FALSE;		// �˻�� �� �ǵ� Ŭ���� ����ġ ����
//	pDoc->BtnStatus.AoiDn.TqClp = m_pMpe->Read(_T("MB00390A")) ? TRUE : FALSE;		// �˻�� �� �ټ� Ŭ���� ����ġ ����
//	pDoc->BtnStatus.AoiDn.MvOne = m_pMpe->Read(_T("MB440151")) ? TRUE : FALSE;
//	pDoc->BtnStatus.AoiDn.LsrPt = m_pMpe->Read(_T("MB003908")) ? TRUE : FALSE;		// �˻�� �� ������ ������ ����ġ ����
//	pDoc->BtnStatus.AoiDn.VelSonicBlw = m_pMpe->Read(_T("MB44014F")) ? TRUE : FALSE;
//
//	if (pDoc->BtnStatus.AoiDn.PrevRelation != pDoc->BtnStatus.AoiDn.Relation)
//	{
//		pDoc->BtnStatus.AoiDn.PrevRelation = pDoc->BtnStatus.AoiDn.Relation;
//		pDoc->SetMkMenu03(_T("Main"), _T("Relation"), pDoc->BtnStatus.AoiDn.Relation);
//	}
//	if (pDoc->BtnStatus.AoiDn.PrevFdCw != pDoc->BtnStatus.AoiDn.FdCw)
//	{
//		pDoc->BtnStatus.AoiDn.PrevFdCw = pDoc->BtnStatus.AoiDn.FdCw;
//		pDoc->SetMkMenu03(_T("AoiDn"), _T("MvCw"), pDoc->BtnStatus.AoiDn.FdCw);
//	}
//	if (pDoc->BtnStatus.AoiDn.PrevFdCcw != pDoc->BtnStatus.AoiDn.FdCcw)
//	{
//		pDoc->BtnStatus.AoiDn.PrevFdCcw = pDoc->BtnStatus.AoiDn.FdCcw;
//		pDoc->SetMkMenu03(_T("AoiDn"), _T("MvCcw"), pDoc->BtnStatus.AoiDn.FdCcw);
//	}
//	if (pDoc->BtnStatus.AoiDn.PrevFdVac != pDoc->BtnStatus.AoiDn.FdVac)
//	{
//		pDoc->BtnStatus.AoiDn.PrevFdVac = pDoc->BtnStatus.AoiDn.FdVac;
//		pDoc->SetMkMenu03(_T("AoiDn"), _T("FdVac"), pDoc->BtnStatus.AoiDn.FdVac);
//	}
//	if (pDoc->BtnStatus.AoiDn.PrevPushUp != pDoc->BtnStatus.AoiDn.PushUp)
//	{
//		pDoc->BtnStatus.AoiDn.PrevPushUp = pDoc->BtnStatus.AoiDn.PushUp;
//		pDoc->SetMkMenu03(_T("AoiDn"), _T("PushUp"), pDoc->BtnStatus.AoiDn.PushUp);
//	}
//	if (pDoc->BtnStatus.AoiDn.PrevTblBlw != pDoc->BtnStatus.AoiDn.TblBlw)
//	{
//		pDoc->BtnStatus.AoiDn.PrevTblBlw = pDoc->BtnStatus.AoiDn.TblBlw;
//		pDoc->SetMkMenu03(_T("AoiDn"), _T("TblBlw"), pDoc->BtnStatus.AoiDn.TblBlw);
//	}
//	if (pDoc->BtnStatus.AoiDn.PrevTblVac != pDoc->BtnStatus.AoiDn.TblVac)
//	{
//		pDoc->BtnStatus.AoiDn.PrevTblVac = pDoc->BtnStatus.AoiDn.TblVac;
//		pDoc->SetMkMenu03(_T("AoiDn"), _T("TblVac"), pDoc->BtnStatus.AoiDn.TblVac);
//	}
//	if (pDoc->BtnStatus.AoiDn.PrevFdClp != pDoc->BtnStatus.AoiDn.FdClp)
//	{
//		pDoc->BtnStatus.AoiDn.PrevFdClp = pDoc->BtnStatus.AoiDn.FdClp;
//		pDoc->SetMkMenu03(_T("AoiDn"), _T("FdClamp"), pDoc->BtnStatus.AoiDn.FdClp);
//	}
//	if (pDoc->BtnStatus.AoiDn.PrevTqClp != pDoc->BtnStatus.AoiDn.TqClp)
//	{
//		pDoc->BtnStatus.AoiDn.PrevTqClp = pDoc->BtnStatus.AoiDn.TqClp;
//		pDoc->SetMkMenu03(_T("AoiDn"), _T("TensClamp"), pDoc->BtnStatus.AoiDn.TqClp);
//	}
//	if (pDoc->BtnStatus.AoiDn.PrevMvOne != pDoc->BtnStatus.AoiDn.MvOne)
//	{
//		pDoc->BtnStatus.AoiDn.PrevMvOne = pDoc->BtnStatus.AoiDn.MvOne;
//		pDoc->SetMkMenu03(_T("AoiDn"), _T("OnePnl"), pDoc->BtnStatus.AoiDn.MvOne);
//	}
//	if (pDoc->BtnStatus.AoiDn.PrevLsrPt != pDoc->BtnStatus.AoiDn.LsrPt)
//	{
//		pDoc->BtnStatus.AoiDn.PrevLsrPt = pDoc->BtnStatus.AoiDn.LsrPt;
//		pDoc->SetMkMenu03(_T("AoiDn"), _T("Lsr"), pDoc->BtnStatus.AoiDn.LsrPt);
//	}
//	if (pDoc->BtnStatus.AoiDn.PrevVelSonicBlw != pDoc->BtnStatus.AoiDn.VelSonicBlw)
//	{
//		pDoc->BtnStatus.AoiDn.PrevVelSonicBlw = pDoc->BtnStatus.AoiDn.VelSonicBlw;
//		pDoc->SetMkMenu03(_T("AoiDn"), _T("VelClrSonic"), pDoc->BtnStatus.AoiDn.VelSonicBlw);
//	}
//
//	//pDoc->BtnStatus.AoiDn.Relation = m_pMpe->Read(_T("MB005701")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.AoiDn.FdCw = m_pMpe->Read(_T("MB005703")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.AoiDn.FdCcw = m_pMpe->Read(_T("MB005704")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.AoiDn.FdVac = m_pMpe->Read(_T("MB005705")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.AoiDn.PushUp = m_pMpe->Read(_T("MB005706")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.AoiDn.TblBlw = m_pMpe->Read(_T("MB005702")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.AoiDn.TblVac = m_pMpe->Read(_T("MB005707")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.AoiDn.FdClp = m_pMpe->Read(_T("MB005709")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.AoiDn.TqClp = m_pMpe->Read(_T("MB00570A")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.AoiDn.MvOne = m_pMpe->Read(_T("MB440151")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.AoiDn.LsrPt = m_pMpe->Read(_T("MB005708")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.AoiDn.VelSonicBlw = m_pMpe->Read(_T("MB44014F")) ? TRUE : FALSE;
//
//	// AOIUp
//	pDoc->BtnStatus.AoiUp.Relation = m_pMpe->Read(_T("MB003801")) ? TRUE : FALSE;	// �˻�� �� ���� ��/���� ����ġ ����
//	pDoc->BtnStatus.AoiUp.FdCw = m_pMpe->Read(_T("MB003803")) ? TRUE : FALSE;		// �˻�� �� �ǵ� ��ȸ�� ����ġ ����
//	pDoc->BtnStatus.AoiUp.FdCcw = m_pMpe->Read(_T("MB003804")) ? TRUE : FALSE;		// �˻�� �� �ǵ� ��ȸ�� ����ġ ����
//	pDoc->BtnStatus.AoiUp.FdVac = m_pMpe->Read(_T("MB003805")) ? TRUE : FALSE;		// �˻�� �� �ǵ� ���� ����ġ ����
//	pDoc->BtnStatus.AoiUp.PushUp = m_pMpe->Read(_T("MB003806")) ? TRUE : FALSE;		// �˻�� �� ��ũ ���� ����ġ ����
//	pDoc->BtnStatus.AoiUp.TblBlw = m_pMpe->Read(_T("MB003802")) ? TRUE : FALSE;		// �˻�� �� ���̺� ��ο� ����ġ ����
//	pDoc->BtnStatus.AoiUp.TblVac = m_pMpe->Read(_T("MB003807")) ? TRUE : FALSE;		// �˻�� �� ���̺� ���� ����ġ ����
//	pDoc->BtnStatus.AoiUp.FdClp = m_pMpe->Read(_T("MB003809")) ? TRUE : FALSE;		// �˻�� �� �ǵ� Ŭ���� ����ġ ����
//	pDoc->BtnStatus.AoiUp.TqClp = m_pMpe->Read(_T("MB00380A")) ? TRUE : FALSE;		// �˻�� �� �ټ� Ŭ���� ����ġ ����
//	pDoc->BtnStatus.AoiUp.MvOne = m_pMpe->Read(_T("MB440151")) ? TRUE : FALSE;
//	pDoc->BtnStatus.AoiUp.LsrPt = m_pMpe->Read(_T("MB003808")) ? TRUE : FALSE;		// �˻�� �� ������ ������ ����ġ ����
//
//	if (pDoc->BtnStatus.AoiUp.PrevRelation != pDoc->BtnStatus.AoiUp.Relation)
//	{
//		pDoc->BtnStatus.AoiUp.PrevRelation = pDoc->BtnStatus.AoiUp.Relation;
//		pDoc->SetMkMenu03(_T("Main"), _T("Relation"), pDoc->BtnStatus.AoiUp.Relation);
//	}
//	if (pDoc->BtnStatus.AoiUp.PrevFdCw != pDoc->BtnStatus.AoiUp.FdCw)
//	{
//		pDoc->BtnStatus.AoiUp.PrevFdCw = pDoc->BtnStatus.AoiUp.FdCw;
//		pDoc->SetMkMenu03(_T("AoiUp"), _T("MvCw"), pDoc->BtnStatus.AoiUp.FdCw);
//	}
//	if (pDoc->BtnStatus.AoiUp.PrevFdCcw != pDoc->BtnStatus.AoiUp.FdCcw)
//	{
//		pDoc->BtnStatus.AoiUp.PrevFdCcw = pDoc->BtnStatus.AoiUp.FdCcw;
//		pDoc->SetMkMenu03(_T("AoiUp"), _T("MvCcw"), pDoc->BtnStatus.AoiUp.FdCcw);
//	}
//	if (pDoc->BtnStatus.AoiUp.PrevFdVac != pDoc->BtnStatus.AoiUp.FdVac)
//	{
//		pDoc->BtnStatus.AoiUp.PrevFdVac = pDoc->BtnStatus.AoiUp.FdVac;
//		pDoc->SetMkMenu03(_T("AoiUp"), _T("FdVac"), pDoc->BtnStatus.AoiUp.FdVac);
//	}
//	if (pDoc->BtnStatus.AoiUp.PrevPushUp != pDoc->BtnStatus.AoiUp.PushUp)
//	{
//		pDoc->BtnStatus.AoiUp.PrevPushUp = pDoc->BtnStatus.AoiUp.PushUp;
//		pDoc->SetMkMenu03(_T("AoiUp"), _T("PushUp"), pDoc->BtnStatus.AoiUp.PushUp);
//	}
//	if (pDoc->BtnStatus.AoiUp.PrevTblBlw != pDoc->BtnStatus.AoiUp.TblBlw)
//	{
//		pDoc->BtnStatus.AoiUp.PrevTblBlw = pDoc->BtnStatus.AoiUp.TblBlw;
//		pDoc->SetMkMenu03(_T("AoiUp"), _T("TblBlw"), pDoc->BtnStatus.AoiUp.TblBlw);
//	}
//	if (pDoc->BtnStatus.AoiUp.PrevTblVac != pDoc->BtnStatus.AoiUp.TblVac)
//	{
//		pDoc->BtnStatus.AoiUp.PrevTblVac = pDoc->BtnStatus.AoiUp.TblVac;
//		pDoc->SetMkMenu03(_T("AoiUp"), _T("TblVac"), pDoc->BtnStatus.AoiUp.TblVac);
//	}
//	if (pDoc->BtnStatus.AoiUp.PrevFdClp != pDoc->BtnStatus.AoiUp.FdClp)
//	{
//		pDoc->BtnStatus.AoiUp.PrevFdClp = pDoc->BtnStatus.AoiUp.FdClp;
//		pDoc->SetMkMenu03(_T("AoiUp"), _T("FdClamp"), pDoc->BtnStatus.AoiUp.FdClp);
//	}
//	if (pDoc->BtnStatus.AoiUp.PrevTqClp != pDoc->BtnStatus.AoiUp.TqClp)
//	{
//		pDoc->BtnStatus.AoiUp.PrevTqClp = pDoc->BtnStatus.AoiUp.TqClp;
//		pDoc->SetMkMenu03(_T("AoiUp"), _T("TensClamp"), pDoc->BtnStatus.AoiUp.TqClp);
//	}
//	if (pDoc->BtnStatus.AoiUp.PrevMvOne != pDoc->BtnStatus.AoiUp.MvOne)
//	{
//		pDoc->BtnStatus.AoiUp.PrevMvOne = pDoc->BtnStatus.AoiUp.MvOne;
//		pDoc->SetMkMenu03(_T("AoiUp"), _T("OnePnl"), pDoc->BtnStatus.AoiUp.MvOne);
//	}
//	if (pDoc->BtnStatus.AoiUp.PrevLsrPt != pDoc->BtnStatus.AoiUp.LsrPt)
//	{
//		pDoc->BtnStatus.AoiUp.PrevLsrPt = pDoc->BtnStatus.AoiUp.LsrPt;
//		pDoc->SetMkMenu03(_T("AoiUp"), _T("Lsr"), pDoc->BtnStatus.AoiUp.LsrPt);
//	}
//
//	//pDoc->BtnStatus.AoiUp.Relation = m_pMpe->Read(_T("MB005601")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.AoiUp.FdCw = m_pMpe->Read(_T("MB005603")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.AoiUp.FdCcw = m_pMpe->Read(_T("MB005604")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.AoiUp.FdVac = m_pMpe->Read(_T("MB005605")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.AoiUp.PushUp = m_pMpe->Read(_T("MB005606")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.AoiUp.TblBlw = m_pMpe->Read(_T("MB005602")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.AoiUp.TblVac = m_pMpe->Read(_T("MB005607")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.AoiUp.FdClp = m_pMpe->Read(_T("MB005609")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.AoiUp.TqClp = m_pMpe->Read(_T("MB00560A")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.AoiUp.MvOne = m_pMpe->Read(_T("MB440151")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.AoiUp.LsrPt = m_pMpe->Read(_T("MB005608")) ? TRUE : FALSE;
//
//	// Engrave
//	pDoc->BtnStatus.Eng.Relation = m_pMpe->Read(_T("MB004511")) ? TRUE : FALSE;
//	pDoc->BtnStatus.Eng.FdCw = m_pMpe->Read(_T("MB004513")) ? TRUE : FALSE;
//	pDoc->BtnStatus.Eng.FdCcw = m_pMpe->Read(_T("MB004514")) ? TRUE : FALSE;
//	pDoc->BtnStatus.Eng.FdVac = m_pMpe->Read(_T("MB004515")) ? TRUE : FALSE;
//	pDoc->BtnStatus.Eng.PushUp = m_pMpe->Read(_T("MB004516")) ? TRUE : FALSE;
//	pDoc->BtnStatus.Eng.TblBlw = m_pMpe->Read(_T("MB00451F")) ? TRUE : FALSE;
//	pDoc->BtnStatus.Eng.TblVac = m_pMpe->Read(_T("MB004517")) ? TRUE : FALSE;
//	pDoc->BtnStatus.Eng.FdClp = m_pMpe->Read(_T("MB004519")) ? TRUE : FALSE;
//	pDoc->BtnStatus.Eng.TqClp = m_pMpe->Read(_T("MB00451A")) ? TRUE : FALSE;
//	pDoc->BtnStatus.Eng.MvOne = m_pMpe->Read(_T("MB440151")) ? TRUE : FALSE;
//	pDoc->BtnStatus.Eng.LsrPt = m_pMpe->Read(_T("MB004518")) ? TRUE : FALSE;
//	pDoc->BtnStatus.Eng.DcRSol = m_pMpe->Read(_T("MB00451B")) ? TRUE : FALSE;
//	pDoc->BtnStatus.Eng.VelSonicBlw = m_pMpe->Read(_T("MB44014E")) ? TRUE : FALSE;
//
//	if (pDoc->BtnStatus.Eng.PrevRelation != pDoc->BtnStatus.Eng.Relation)
//	{
//		pDoc->BtnStatus.Eng.PrevRelation = pDoc->BtnStatus.Eng.Relation;
//		pDoc->SetMkMenu03(_T("Main"), _T("Relation"), pDoc->BtnStatus.Eng.Relation);
//	}
//	if (pDoc->BtnStatus.Eng.PrevFdCw != pDoc->BtnStatus.Eng.FdCw)
//	{
//		pDoc->BtnStatus.Eng.PrevFdCw = pDoc->BtnStatus.Eng.FdCw;
//		pDoc->SetMkMenu03(_T("Engraving"), _T("MvCw"), pDoc->BtnStatus.Eng.FdCw);
//	}
//	if (pDoc->BtnStatus.Eng.PrevFdCcw != pDoc->BtnStatus.Eng.FdCcw)
//	{
//		pDoc->BtnStatus.Eng.PrevFdCcw = pDoc->BtnStatus.Eng.FdCcw;
//		pDoc->SetMkMenu03(_T("Engraving"), _T("MvCcw"), pDoc->BtnStatus.Eng.FdCcw);
//	}
//	if (pDoc->BtnStatus.Eng.PrevFdVac != pDoc->BtnStatus.Eng.FdVac)
//	{
//		pDoc->BtnStatus.Eng.PrevFdVac = pDoc->BtnStatus.Eng.FdVac;
//		pDoc->SetMkMenu03(_T("Engraving"), _T("FdVac"), pDoc->BtnStatus.Eng.FdVac);
//	}
//	if (pDoc->BtnStatus.Eng.PrevPushUp != pDoc->BtnStatus.Eng.PushUp)
//	{
//		pDoc->BtnStatus.Eng.PrevPushUp = pDoc->BtnStatus.Eng.PushUp;
//		pDoc->SetMkMenu03(_T("Engraving"), _T("PushUp"), pDoc->BtnStatus.Eng.PushUp);
//	}
//	if (pDoc->BtnStatus.Eng.PrevTblBlw != pDoc->BtnStatus.Eng.TblBlw)
//	{
//		pDoc->BtnStatus.Eng.PrevTblBlw = pDoc->BtnStatus.Eng.TblBlw;
//		pDoc->SetMkMenu03(_T("Engraving"), _T("TblBlw"), pDoc->BtnStatus.Eng.TblBlw);
//	}
//	if (pDoc->BtnStatus.Eng.PrevTblVac != pDoc->BtnStatus.Eng.TblVac)
//	{
//		pDoc->BtnStatus.Eng.PrevTblVac = pDoc->BtnStatus.Eng.TblVac;
//		pDoc->SetMkMenu03(_T("Engraving"), _T("TblVac"), pDoc->BtnStatus.Eng.TblVac);
//	}
//	if (pDoc->BtnStatus.Eng.PrevFdClp != pDoc->BtnStatus.Eng.FdClp)
//	{
//		pDoc->BtnStatus.Eng.PrevFdClp = pDoc->BtnStatus.Eng.FdClp;
//		pDoc->SetMkMenu03(_T("Engraving"), _T("FdClamp"), pDoc->BtnStatus.Eng.FdClp);
//	}
//	if (pDoc->BtnStatus.Eng.PrevTqClp != pDoc->BtnStatus.Eng.TqClp)
//	{
//		pDoc->BtnStatus.Eng.PrevTqClp = pDoc->BtnStatus.Eng.TqClp;
//		pDoc->SetMkMenu03(_T("Engraving"), _T("TensClamp"), pDoc->BtnStatus.Eng.TqClp);
//	}
//	if (pDoc->BtnStatus.Eng.PrevMvOne != pDoc->BtnStatus.Eng.MvOne)
//	{
//		pDoc->BtnStatus.Eng.PrevMvOne = pDoc->BtnStatus.Eng.MvOne;
//		pDoc->SetMkMenu03(_T("Engraving"), _T("OnePnl"), pDoc->BtnStatus.Eng.MvOne);
//	}
//	if (pDoc->BtnStatus.Eng.PrevLsrPt != pDoc->BtnStatus.Eng.LsrPt)
//	{
//		pDoc->BtnStatus.Eng.PrevLsrPt = pDoc->BtnStatus.Eng.LsrPt;
//		pDoc->SetMkMenu03(_T("Engraving"), _T("Lsr"), pDoc->BtnStatus.Eng.LsrPt);
//	}
//	if (pDoc->BtnStatus.Eng.PrevVelSonicBlw != pDoc->BtnStatus.Eng.VelSonicBlw)
//	{
//		pDoc->BtnStatus.Eng.PrevVelSonicBlw = pDoc->BtnStatus.Eng.VelSonicBlw;
//		pDoc->SetMkMenu03(_T("Engraving"), _T("VelClrSonic"), pDoc->BtnStatus.Eng.VelSonicBlw);
//	}
//	if (pDoc->BtnStatus.Eng.PrevDcRSol != pDoc->BtnStatus.Eng.DcRSol)
//	{
//		pDoc->BtnStatus.Eng.PrevDcRSol = pDoc->BtnStatus.Eng.DcRSol;
//		pDoc->SetMkMenu03(_T("Engraving"), _T("DancerUp"), pDoc->BtnStatus.Eng.DcRSol);
//	}
//
//	// Uncoiler
//	pDoc->BtnStatus.Uc.Relation = m_pMpe->Read(_T("MB003601")) ? TRUE : FALSE;	// �����Ϸ� ���� ��/���� ����ġ ����
//	pDoc->BtnStatus.Uc.FdCw = m_pMpe->Read(_T("MB00360C")) ? TRUE : FALSE;	// �����Ϸ� ��ǰ�� ��ȸ�� ����ġ ����
//	pDoc->BtnStatus.Uc.FdCcw = m_pMpe->Read(_T("MB00360D")) ? TRUE : FALSE;	// �����Ϸ� ��ǰ�� ��ȸ�� ����ġ ����
//	pDoc->BtnStatus.Uc.ReelChuck = m_pMpe->Read(_T("MB00360B")) ? TRUE : FALSE;	// �����Ϸ� ��ǰô Ŭ���� ����ġ ����
//	pDoc->BtnStatus.Uc.DcRlUpDn = m_pMpe->Read(_T("MB003602")) ? TRUE : FALSE;	// �����Ϸ� ���� ���/�ϰ� ����ġ ����
//	pDoc->BtnStatus.Uc.ReelJoinL = m_pMpe->Read(_T("MB003605")) ? TRUE : FALSE;	// �����Ϸ� ��ǰ ������(��/��) ����ġ ����
//	pDoc->BtnStatus.Uc.ReelJoinR = m_pMpe->Read(_T("MB003606")) ? TRUE : FALSE;	// �����Ϸ� ��ǰ ������(��/��) ����ġ ����
//	pDoc->BtnStatus.Uc.ReelJoinVac = m_pMpe->Read(_T("MB00360F")) ? TRUE : FALSE;	// �����Ϸ� ��ǰ ������ ���� ����ġ ����
//	pDoc->BtnStatus.Uc.PprChuck = m_pMpe->Read(_T("MB003608")) ? TRUE : FALSE;	// �����Ϸ� ����ô Ŭ���� ����ġ ����
//	pDoc->BtnStatus.Uc.PprCw = m_pMpe->Read(_T("MB003609")) ? TRUE : FALSE;	// �����Ϸ� ������ ��ȸ�� ����ġ ����
//	pDoc->BtnStatus.Uc.PprCcw = m_pMpe->Read(_T("MB00360A")) ? TRUE : FALSE;	// �����Ϸ� ������ ��ȸ�� ����ġ ����
//	pDoc->BtnStatus.Uc.ClRlUpDn = m_pMpe->Read(_T("MB003603")) ? TRUE : FALSE;	// �����Ϸ� Ŭ���ѷ� ���/�ϰ� ����ġ ����
//	pDoc->BtnStatus.Uc.ClRlPshUpDn = m_pMpe->Read(_T("MB003604")) ? TRUE : FALSE;	// �����Ϸ� Ŭ���ѷ����� ���/�ϰ� ����ġ ����
//
//	if (pDoc->BtnStatus.Uc.PrevRelation != pDoc->BtnStatus.Uc.Relation)
//	{
//		pDoc->BtnStatus.Uc.PrevRelation = pDoc->BtnStatus.Uc.Relation;
//		pDoc->SetMkMenu03(_T("Main"), _T("Relation"), pDoc->BtnStatus.Uc.Relation);
//	}
//	if (pDoc->BtnStatus.Uc.PrevFdCw != pDoc->BtnStatus.Uc.FdCw)
//	{
//		pDoc->BtnStatus.Uc.PrevFdCw = pDoc->BtnStatus.Uc.FdCw;
//		pDoc->SetMkMenu03(_T("Uncoiler"), _T("MvCw"), pDoc->BtnStatus.Uc.FdCw);
//	}
//	if (pDoc->BtnStatus.Uc.PrevFdCcw != pDoc->BtnStatus.Uc.FdCcw)
//	{
//		pDoc->BtnStatus.Uc.PrevFdCcw = pDoc->BtnStatus.Uc.FdCcw;
//		pDoc->SetMkMenu03(_T("Uncoiler"), _T("MvCcw"), pDoc->BtnStatus.Uc.FdCcw);
//	}
//	if (pDoc->BtnStatus.Uc.PrevReelChuck != pDoc->BtnStatus.Uc.ReelChuck)
//	{
//		pDoc->BtnStatus.Uc.PrevReelChuck = pDoc->BtnStatus.Uc.ReelChuck;
//		pDoc->SetMkMenu03(_T("Uncoiler"), _T("PrdChuck"), pDoc->BtnStatus.Uc.ReelChuck);
//	}
//	if (pDoc->BtnStatus.Uc.PrevDcRlUpDn != pDoc->BtnStatus.Uc.DcRlUpDn)
//	{
//		pDoc->BtnStatus.Uc.PrevDcRlUpDn = pDoc->BtnStatus.Uc.DcRlUpDn;
//		pDoc->SetMkMenu03(_T("Uncoiler"), _T("DancerUp"), pDoc->BtnStatus.Uc.DcRlUpDn);
//	}
//	if (pDoc->BtnStatus.Uc.PrevReelJoinL != pDoc->BtnStatus.Uc.ReelJoinL)
//	{
//		pDoc->BtnStatus.Uc.PrevReelJoinL = pDoc->BtnStatus.Uc.ReelJoinL;
//		pDoc->SetMkMenu03(_T("Uncoiler"), _T("PasteUpLf"), pDoc->BtnStatus.Uc.ReelJoinL);
//	}
//	if (pDoc->BtnStatus.Uc.PrevReelJoinR != pDoc->BtnStatus.Uc.ReelJoinR)
//	{
//		pDoc->BtnStatus.Uc.PrevReelJoinR = pDoc->BtnStatus.Uc.ReelJoinR;
//		pDoc->SetMkMenu03(_T("Uncoiler"), _T("PasteUpRt"), pDoc->BtnStatus.Uc.ReelJoinR);
//	}
//	if (pDoc->BtnStatus.Uc.PrevReelJoinVac != pDoc->BtnStatus.Uc.ReelJoinVac)
//	{
//		pDoc->BtnStatus.Uc.PrevReelJoinVac = pDoc->BtnStatus.Uc.ReelJoinVac;
//		pDoc->SetMkMenu03(_T("Uncoiler"), _T("PasteVac"), pDoc->BtnStatus.Uc.ReelJoinVac);
//	}
//	if (pDoc->BtnStatus.Uc.PrevPprChuck != pDoc->BtnStatus.Uc.PprChuck)
//	{
//		pDoc->BtnStatus.Uc.PrevPprChuck = pDoc->BtnStatus.Uc.PprChuck;
//		pDoc->SetMkMenu03(_T("Uncoiler"), _T("PprChuck"), pDoc->BtnStatus.Uc.PprChuck);
//	}
//	if (pDoc->BtnStatus.Uc.PrevPprCw != pDoc->BtnStatus.Uc.PprCw)
//	{
//		pDoc->BtnStatus.Rc.PrevPprCw = pDoc->BtnStatus.Uc.PprCw;
//		pDoc->SetMkMenu03(_T("Uncoiler"), _T("PprCw"), pDoc->BtnStatus.Uc.PprCw);
//	}
//	if (pDoc->BtnStatus.Uc.PrevPprCcw != pDoc->BtnStatus.Uc.PprCcw)
//	{
//		pDoc->BtnStatus.Uc.PrevPprCcw = pDoc->BtnStatus.Uc.PprCcw;
//		pDoc->SetMkMenu03(_T("Uncoiler"), _T("PprCcw"), pDoc->BtnStatus.Uc.PprCcw);
//	}
//	if (pDoc->BtnStatus.Uc.PrevClRlUpDn != pDoc->BtnStatus.Uc.ClRlUpDn)
//	{
//		pDoc->BtnStatus.Uc.PrevClRlUpDn = pDoc->BtnStatus.Uc.ClRlUpDn;
//		pDoc->SetMkMenu03(_T("Uncoiler"), _T("ClrRollUp"), pDoc->BtnStatus.Uc.ClRlUpDn);
//	}
//	if (pDoc->BtnStatus.Uc.PrevClRlPshUpDn != pDoc->BtnStatus.Uc.ClRlPshUpDn)
//	{
//		pDoc->BtnStatus.Uc.PrevClRlPshUpDn = pDoc->BtnStatus.Uc.ClRlPshUpDn;
//		pDoc->SetMkMenu03(_T("Uncoiler"), _T("ClrRollPush"), pDoc->BtnStatus.Uc.ClRlPshUpDn);
//	}
//
//	//pDoc->BtnStatus.Uc.Relation = m_pMpe->Read(_T("MB005401")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.Uc.FdCw = m_pMpe->Read(_T("MB00540C")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.Uc.FdCcw = m_pMpe->Read(_T("MB00540D")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.Uc.ReelChuck = m_pMpe->Read(_T("MB00540B")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.Uc.DcRlUpDn = m_pMpe->Read(_T("MB005402")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.Uc.ReelJoinL = m_pMpe->Read(_T("MB005405")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.Uc.ReelJoinR = m_pMpe->Read(_T("MB005406")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.Uc.ReelJoinVac = m_pMpe->Read(_T("MB00540F")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.Uc.PprChuck = m_pMpe->Read(_T("MB005408")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.Uc.PprCw = m_pMpe->Read(_T("MB005409")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.Uc.PprCcw = m_pMpe->Read(_T("MB00540A")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.Uc.ClRlUpDn = m_pMpe->Read(_T("MB005403")) ? TRUE : FALSE;
//	//pDoc->BtnStatus.Uc.ClRlPshUpDn = m_pMpe->Read(_T("MB005404")) ? TRUE : FALSE;
//
//#else
//	pDoc->BtnStatus.Main.Manual = TRUE;
//#endif
//}

//void CGvisR2R_PunchView::InitIoWrite()
//{
//#ifdef USE_MPE
//	if (!m_pMpe)
//		return;
//
//	//IoWrite(_T("MB44015E"), 0); // ����1 On  (PC�� ON, OFF) - 20141020
//	MpeWrite(_T("MB44015E"), 0);
//	//IoWrite(_T("MB44015F"), 0); // ����2 On  (PC�� ON, OFF) - 20141020
//	MpeWrite(_T("MB44015F"), 0);
//
//	//IoWrite(_T("MB003828"), 0); // �˻�� �� �˻� ���� <-> Y4368 I/F
//	MpeWrite(_T("MB003828"), 0);
//	//IoWrite(_T("MB003829"), 0); // �˻�� �� �˻� ���̺� ���� SOL <-> Y4369 I/F
//	MpeWrite(_T("MB003829"), 0);
//	//IoWrite(_T("MB00382A"), 0); // �˻�� �� Reset <-> Y436A I/F
//	MpeWrite(_T("MB00382A"), 0);
//	//IoWrite(_T("MB00382B"), 0); // ��ŷ�� Lot End <-> Y436B I/F
//	MpeWrite(_T("MB00382B"), 0);
//
//	//IoWrite(_T("MB003928"), 0); // �˻�� �� �˻� ���� <-> Y4468 I/F
//	MpeWrite(_T("MB003928"), 0);
//	//IoWrite(_T("MB003929"), 0); // �˻�� �� �˻� ���̺� ���� SOL <-> Y4369 I/F
//	MpeWrite(_T("MB003929"), 0);
//	//IoWrite(_T("MB00392A"), 0); // �˻�� �� Reset <-> Y436A I/F
//	MpeWrite(_T("MB00392A"), 0);
//	//IoWrite(_T("MB00392B"), 0); // ��ŷ�� Lot End <-> Y436B I/F
//	MpeWrite(_T("MB00392B"), 0);
//
//	//IoWrite(_T("MB44015D"), 0); // �ڵ� �ʱ� ��������(PC�� On/Off ��Ŵ, PLC�� ���������� ��ũ����, on->off�� �������� on, �ٽ� ��������ġ�� �������� off) - 20141017
//	MpeWrite(_T("MB44015D"), 0);
//	//IoWrite(_T("ML45064"), 0); // �˻�� Feeding �ѷ� Offset(*1000, +:�� ����, -�� ����, PC�� ���� PLC���� ����)
//	MpeWrite(_T("ML45064"), 0);
//	//IoWrite(_T("ML45066"), 0); // ��ŷ�� Feeding �ѷ� Offset(*1000, +:�� ����, -�� ����, PC�� ���� PLC���� ����)
//	MpeWrite(_T("ML45066"), 0);
//
//	//IoWrite(_T("MB600000"), 0); // PC�� PLC�� Alarm �߻����θ� Ȯ��
//	MpeWrite(_T("MB600000"), 0);
//	//IoWrite(_T("MB600008"), 0); // PC�� PLC�� Alarm �߻����θ� Ȯ��
//	MpeWrite(_T("MB600008"), 0);
//	//IoWrite(_T("ML60002"), 0); // �˶��� �߻��� ������ ����(PLC�� ǥ�� �� �˶��� �������� ����Ŵ).
//	MpeWrite(_T("ML60002"), 0);
//#endif
//}

//BOOL CGvisR2R_PunchView::MoveAlign0(int nPos)
//{
//	if (!m_mgrPunch->m_pMotion)
//		return FALSE;
//
//	if (m_pDlgMenu02)
//		m_pDlgMenu02->SetLight();
//
//	if (m_mgrPunch->m_pMotion->m_dPinPosY[0] > 0.0 && m_mgrPunch->m_pMotion->m_dPinPosX[0] > 0.0)
//	{
//		double dCurrX = m_dEnc[AXIS_X0];
//		double dCurrY = m_dEnc[AXIS_Y0];
//
//		double pPos[2];
//		if (nPos == 0)
//		{
//			pPos[0] = m_mgrReelmap->m_Master[0].m_stAlignMk.X0 + m_mgrPunch->m_pMotion->m_dPinPosX[0];
//			pPos[1] = m_mgrReelmap->m_Master[0].m_stAlignMk.Y0 + m_mgrPunch->m_pMotion->m_dPinPosY[0];
//		}
//		else if (nPos == 1)
//		{
//			pPos[0] = m_mgrReelmap->m_Master[0].m_stAlignMk.X1 + m_mgrPunch->m_pMotion->m_dPinPosX[0];
//			pPos[1] = m_mgrReelmap->m_Master[0].m_stAlignMk.Y1 + m_mgrPunch->m_pMotion->m_dPinPosY[0];
//		}
//
//		if (m_mgrProcedure->ChkCollision(AXIS_X0, pPos[0]))
//			return FALSE;
//
//		double fLen, fVel, fAcc, fJerk;
//		fLen = sqrt(((pPos[0] - dCurrX) * (pPos[0] - dCurrX)) + ((pPos[1] - dCurrY) * (pPos[1] - dCurrY)));
//		if (fLen > 0.001)
//		{
//			m_mgrPunch->m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_X0, fLen, fVel, fAcc, fJerk);
//			if (!m_mgrPunch->m_pMotion->Move(MS_X0Y0, pPos, fVel, fAcc, fAcc, ABS, NO_WAIT))
//			{
//				if (!m_mgrPunch->m_pMotion->Move(MS_X0Y0, pPos, fVel, fAcc, fAcc, ABS, NO_WAIT))
//				{
//					pView->ClrDispMsg();
//					AfxMessageBox(_T("Error - Move MoveAlign0 ..."));
//					return FALSE;
//				}
//			}
//		}
//
//		return TRUE;
//	}
//
//	return FALSE;
//}
//
//BOOL CGvisR2R_PunchView::MoveAlign1(int nPos)
//{
//	if (!m_mgrPunch->m_pMotion)
//		return FALSE;
//
//	if (m_pDlgMenu02)
//		m_pDlgMenu02->SetLight2();
//
//	if (m_mgrPunch->m_pMotion->m_dPinPosY[1] > 0.0 && m_mgrPunch->m_pMotion->m_dPinPosX[1] > 0.0)
//	{
//		double dCurrX = m_dEnc[AXIS_X1];
//		double dCurrY = m_dEnc[AXIS_Y1];
//
//		double pPos[2];
//		if (nPos == 0)
//		{
//			pPos[0] = m_mgrReelmap->m_Master[0].m_stAlignMk.X0 + m_mgrPunch->m_pMotion->m_dPinPosX[1];
//			pPos[1] = m_mgrReelmap->m_Master[0].m_stAlignMk.Y0 + m_mgrPunch->m_pMotion->m_dPinPosY[1];
//		}
//		else if (nPos == 1)
//		{
//			pPos[0] = m_mgrReelmap->m_Master[0].m_stAlignMk.X1 + m_mgrPunch->m_pMotion->m_dPinPosX[1];
//			pPos[1] = m_mgrReelmap->m_Master[0].m_stAlignMk.Y1 + m_mgrPunch->m_pMotion->m_dPinPosY[1];
//		}
//
//		if (m_mgrProcedure->ChkCollision(AXIS_X1, pPos[0]))
//			return FALSE;
//
//		double fLen, fVel, fAcc, fJerk;
//		fLen = sqrt(((pPos[0] - dCurrX) * (pPos[0] - dCurrX)) + ((pPos[1] - dCurrY) * (pPos[1] - dCurrY)));
//		if (fLen > 0.001)
//		{
//			m_mgrPunch->m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_X1, fLen, fVel, fAcc, fJerk);
//			if (!m_mgrPunch->m_pMotion->Move(MS_X1Y1, pPos, fVel, fAcc, fAcc, ABS, NO_WAIT))
//			{
//				if (!m_mgrPunch->m_pMotion->Move(MS_X1Y1, pPos, fVel, fAcc, fAcc, ABS, NO_WAIT))
//				{
//					pView->ClrDispMsg();
//					AfxMessageBox(_T("Error - Move MoveAlign1 ..."));
//					return FALSE;
//				}
//			}
//		}
//
//		return TRUE;
//	}
//
//	return FALSE;
//}

void CGvisR2R_PunchView::SetLastProc()
{
	if (m_pDlgMenu01)
		m_pDlgMenu01->SetLastProc();
}

BOOL CGvisR2R_PunchView::IsLastProc()
{
	BOOL bRtn = FALSE;
	if (m_pDlgMenu01)
		bRtn = m_pDlgMenu01->IsLastProc();
	else
		bRtn = FALSE;

	return bRtn;
}

BOOL CGvisR2R_PunchView::IsLastJob(int nAoi) // 0 : AOI-Up , 1 : AOI-Dn , 2 : AOI-UpDn
{
	switch (nAoi)
	{
	case 0: // AOI-Up
		if (pDoc->WorkingInfo.System.sPathCamSpecDir.IsEmpty() ||
			pDoc->WorkingInfo.LastJob.sModelUp.IsEmpty() ||
			pDoc->WorkingInfo.LastJob.sLayerUp.IsEmpty())
			return FALSE;
		break;
	case 1: // AOI-Dn
		if (pDoc->WorkingInfo.System.sPathCamSpecDir.IsEmpty() ||
			pDoc->WorkingInfo.LastJob.sModelUp.IsEmpty() ||
			//pDoc->WorkingInfo.LastJob.sModelDn.IsEmpty() ||
			pDoc->WorkingInfo.LastJob.sLayerDn.IsEmpty())
			return FALSE;
		break;
	case 2: // AOI-UpDn
		if (pDoc->WorkingInfo.System.sPathCamSpecDir.IsEmpty() ||
			pDoc->WorkingInfo.LastJob.sModelUp.IsEmpty() ||
			pDoc->WorkingInfo.LastJob.sLayerUp.IsEmpty())
			return FALSE;
		if (pDoc->WorkingInfo.System.sPathCamSpecDir.IsEmpty() ||
			pDoc->WorkingInfo.LastJob.sModelUp.IsEmpty() ||
			//pDoc->WorkingInfo.LastJob.sModelDn.IsEmpty() ||
			pDoc->WorkingInfo.LastJob.sLayerDn.IsEmpty())
			return FALSE;
		break;
	}

	return TRUE;
}

void CGvisR2R_PunchView::MonPlcSignal()
{
#ifdef USE_MPE
	if (pDoc->m_pMpeSignal[3] & (0x01 << 0))		// ���κ� 2D Leading �۾��Ϸ�(PLC�� ON/OFF)
	{
		if (m_pDlgMenu03)
			m_pDlgMenu03->SetLed(0, TRUE);
	}
	else
	{
		if (m_pDlgMenu03)
			m_pDlgMenu03->SetLed(0, FALSE);
	}

	if (pDoc->m_pMpeSignal[3] & (0x01 << 2))		// ���κ� Laser �۾��Ϸ�(PLC�� ON/OFF)
	{
		if (m_pDlgMenu03)
			m_pDlgMenu03->SetLed(1, TRUE);
	}
	else
	{
		if (m_pDlgMenu03)
			m_pDlgMenu03->SetLed(1, FALSE);
	}

	if (pDoc->m_pMpeSignal[3] & (0x01 << 3))		// �˻�� ��� �˻� �۾��Ϸ�(PLC�� ON/OFF)
	{
		if (m_pDlgMenu03)
			m_pDlgMenu03->SetLed(2, TRUE);
	}
	else
	{
		if (m_pDlgMenu03)
			m_pDlgMenu03->SetLed(2, FALSE);
	}

	if (pDoc->m_pMpeSignal[3] & (0x01 << 4))		// �˻�� �ϸ� �˻� �۾��Ϸ�(PLC�� ON/OFF)
	{
		if (m_pDlgMenu03)
			m_pDlgMenu03->SetLed(3, TRUE);
	}
	else
	{
		if (m_pDlgMenu03)
			m_pDlgMenu03->SetLed(3, FALSE);
	}

	if (pDoc->m_pMpeSignal[3] & (0x01 << 5))		// ��ŷ�� ��ŷ �۾��Ϸ�(PLC�� ON/OFF)
	{
		if (m_pDlgMenu03)
			m_pDlgMenu03->SetLed(4, TRUE);
	}
	else
	{
		if (m_pDlgMenu03)
			m_pDlgMenu03->SetLed(4, FALSE);
	}

#endif
}

//void CGvisR2R_PunchView::MonDispMain()
//{
//	BOOL bDispStop = TRUE;
//
//#ifdef USE_MPE
//	if (pDoc->m_pMpeSignal[2] & (0x01 << 0))		// ������(PLC�� PC�� �˷��ִ� ���� ����) - 20141031
//	{
//		if (m_sDispMain != _T("������"))
//		{
//			DispMain(_T("������"), RGB_GREEN);	
//			pDoc->SetMkMenu03(_T("Main"), _T("Run"), TRUE);
//			pDoc->SetMkMenu03(_T("Main"), _T("Stop"), FALSE);
//		}
//		else
//		{
//		}
//	}
//
//	if (pDoc->m_pMpeSignal[2] & (0x01 << 2))		// �����غ�(PLC�� PC�� �˷��ִ� ���� ����) - 20141031
//	{
//		bDispStop = FALSE;
//		//if(!WatiDispMain(10))
//		{
//			if (m_sDispMain != _T("�����غ�"))
//			{
//				DispMain(_T("�����غ�"), RGB_GREEN);
//				pDoc->SetMkMenu03(_T("Main"), _T("Ready"), TRUE);
//			}
//		}
//	}
//	else
//	{
//		if (pDoc->m_pMpeSignal[2] & (0x01 << 3))		// �ʱ����(PLC�� PC�� �˷��ִ� ���� ����) - 20141031
//		{
//			bDispStop = FALSE;
//			//if(!WatiDispMain(10))
//			{
//				if (pDoc->WorkingInfo.LastJob.bSampleTest)
//				{
//					if (pDoc->WorkingInfo.LastJob.bDualTest)
//					{
//						if (m_sDispMain != _T("������"))
//						{
//							DispMain(_T("������"), RGB_GREEN);
//						}
//					}
//					else
//					{
//						if (m_sDispMain != _T("�ܸ����"))
//						{
//							DispMain(_T("�ܸ����"), RGB_GREEN);
//						}
//					}
//				}
//				else if (pDoc->WorkingInfo.LastJob.bDualTest)
//				{
//					if (m_sDispMain != _T("���˻�"))
//					{
//						DispMain(_T("���˻�"), RGB_GREEN);
//					}
//				}
//				else
//				{
//					if (m_sDispMain != _T("�ܸ�˻�"))
//					{
//						DispMain(_T("�ܸ�˻�"), RGB_GREEN);
//					}
//				}
//			}
//		}
//		else
//		{
//			if (m_sDispMain != _T("�����غ�"))
//			{
//				bDispStop = TRUE;
//			}
//			else
//			{
//				bDispStop = FALSE;
//			}
//		}
//	}
//
//
//	if (pDoc->m_pMpeSignal[2] & (0x01 << 1))		// ����(PLC�� PC�� �˷��ִ� ���� ����) - 20141031
//	{
//		if (bDispStop)
//		{
//			if (m_sDispMain != _T("�� ��"))
//			{
//				DispMain(_T("�� ��"), RGB_RED);
//				pDoc->SetMkMenu03(_T("Main"), _T("Stop"), TRUE);
//				pDoc->SetMkMenu03(_T("Main"), _T("Run"), FALSE);
//			}
//		}
//	}
//#endif
//}

//void CGvisR2R_PunchView::MonPlcAlm()
//{
//	BOOL bMon, bClr;
//	long lOn = m_pMpe->Read(_T("ML60000"));
//
//	bMon = lOn & (0x01 << 0);
//	bClr = lOn & (0x01 << 1);
//	if (bMon)
//		PlcAlm(bMon, 0);
//	else if (bClr)
//		PlcAlm(0, bClr);
//	else
//		PlcAlm(bMon, bClr);
//
//	if (!pDoc->m_sAlmMsg.IsEmpty())
//	{
//		if (pDoc->m_sAlmMsg != pDoc->m_sPrevAlmMsg)
//		{
//			pDoc->m_sPrevAlmMsg = pDoc->m_sAlmMsg;
//			CycleStop();
//		}
//	}
//}

//void CGvisR2R_PunchView::PlcAlm(BOOL bMon, BOOL bClr)
//{
//	if (bMon && !m_mgrProcedure->m_nMonAlmF)
//	{
//		m_mgrProcedure->m_nMonAlmF = 1;
//		//ResetMonAlm();
//		FindAlarm();
//		if (pView->m_mgrPunch->m_pEngrave)
//		{
//			pDoc->m_sIsAlmMsg = _T("");
//			pView->m_mgrPunch->m_pEngrave->SetAlarm(pDoc->m_sAlmMsg);
//		}
//
//		Sleep(300);
//		MpeWrite(_T("MB600008"), 1);
//	}
//	else if (!bMon && m_mgrProcedure->m_nMonAlmF)
//	{
//		m_mgrProcedure->m_nMonAlmF = 0;
//		ResetMonAlm();
//	}
//	else
//	{
//		if (pView->m_mgrPunch->m_pEngrave)
//		{
//			if (pDoc->m_sIsAlmMsg != pDoc->m_sAlmMsg)
//			{
//				if(pView->m_mgrPunch->m_pEngrave)
//					pView->m_mgrPunch->m_pEngrave->SetAlarm(pDoc->m_sAlmMsg);
//			}
//		}
//	}
//
//
//	if (bClr && !m_mgrProcedure->m_nClrAlmF)
//	{
//		m_mgrProcedure->m_nClrAlmF = 1;
//		ClrAlarm();
//		if (pView->m_mgrPunch->m_pEngrave)
//		{
//			pDoc->m_sAlmMsg = _T("");
//			pView->m_mgrPunch->m_pEngrave->SetAlarm(pDoc->m_sAlmMsg);
//		}
//		Sleep(300);
//		MpeWrite(_T("MB600009"), 1);
//
//	}
//	else if (!bClr && m_mgrProcedure->m_nClrAlmF)
//	{
//		m_mgrProcedure->m_nClrAlmF = 0;
//		ResetClear();
//	}
//	else
//	{
//		if (pView->m_mgrPunch->m_pEngrave)
//		{
//			if (pDoc->m_sIsAlmMsg != pDoc->m_sAlmMsg)
//			{
//				if (pView->m_mgrPunch->m_pEngrave)
//					pView->m_mgrPunch->m_pEngrave->SetAlarm(pDoc->m_sAlmMsg);
//			}
//		}
//	}
//}

//void CGvisR2R_PunchView::FindAlarm()
//{
//	// �˶��� �߻��� ������
//	long lAlmPage = -1;
//	lAlmPage = m_pMpe->Read(_T("ML60002"));
//
//	//char szData[200];
//	TCHAR szData[200];
//	CString str1, str2, str3, strM, str, strH = _T("");
//
//	str1.Format(_T("%d"), lAlmPage);
//	str2 = _T("Address");
//	if (0 < ::GetPrivateProfileString(str1, str2, NULL, szData, sizeof(szData), PATH_ALARM))
//		strM = CString(szData);
//	else
//		strM = _T("");
//
//	if (strM.IsEmpty())
//		return;
//
//	long lAlm = m_pMpe->Read(strM); // lAlm : (32Bits << Row)
//	for (int i = 0; i<32; i++)
//	{
//		if (lAlm & (0x01 << i))
//		{
//			str3.Format(_T("%d"), i);
//			if (0 < ::GetPrivateProfileString(str1, str3, NULL, szData, sizeof(szData), PATH_ALARM))
//				strH = CString(szData);
//			else
//				strH.Format(_T("%s = %d"), strM, lAlm);
//				//strH = _T("");
//
//			if (str.IsEmpty())
//				str = strH;
//			else
//			{
//				str += _T("\r\n");
//				str += strH;
//			}
//		}
//	}
//
//	pDoc->m_sAlmMsg = strH;
//}

//void CGvisR2R_PunchView::ResetMonAlm()
//{
//	MpeWrite(_T("MB600008"), 0);
//}

void CGvisR2R_PunchView::ClrAlarm()
{
	m_mgrPunch->ClrAlarm();
}

//void CGvisR2R_PunchView::ClrAlarm()
//{
//	if (!pDoc->m_sAlmMsg.IsEmpty())
//	{
//		pDoc->m_sAlmMsg = _T("");
//		pDoc->m_sIsAlmMsg = _T("");
//		pDoc->m_sPrevAlmMsg = _T("");
//	}
//}

//void CGvisR2R_PunchView::ResetClear()
//{
//	MpeWrite(_T("MB600009"), 0);
//}

//void CGvisR2R_PunchView::ChkTempStop(BOOL bChk)
//{
//	m_mgrPunch->ChkTempStop(bChk);
//}
void CGvisR2R_PunchView::ChkTempStop(BOOL bChk)
{
	if (bChk)
	{
		if (!m_bTIM_CHK_TEMP_STOP)
		{
			m_bTIM_CHK_TEMP_STOP = TRUE;
			SetTimer(TIM_CHK_TEMP_STOP, 500, NULL);
		}
	}
	else
	{
		m_bTIM_CHK_TEMP_STOP = FALSE;
	}
}

void CGvisR2R_PunchView::ChgLot()
{
	pDoc->WorkingInfo.LastJob.sLotUp = pDoc->Status.PcrShare[0].sLot;
	pDoc->SetModelInfoUp();

	pDoc->WorkingInfo.LastJob.sLotDn = pDoc->Status.PcrShare[1].sLot;
	pDoc->SetModelInfoDn();

	//	pDoc->m_bDoneChgLot = TRUE;

	m_mgrReelmap->SetPathAtBuf();
}

void  CGvisR2R_PunchView::SetLotLastShot()
{
	pDoc->m_nLotLastShot = int(_tstof(pDoc->WorkingInfo.LastJob.sLotSepLen)*1000.0 / _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen));
}

BOOL CGvisR2R_PunchView::IsMkStrip(int nStripIdx)
{
	if (!m_pDlgMenu01 || nStripIdx < 1 || nStripIdx > MAX_STRIP_NUM)
		return TRUE;

	return (m_pDlgMenu01->GetChkStrip(nStripIdx - 1));
}

//void CGvisR2R_PunchView::CycleStop()
//{
//	m_bCycleStop = TRUE;
//}

BOOL CGvisR2R_PunchView::ChkLotCutPos()
{
	if (pDoc->WorkingInfo.LastJob.bLotSep && pDoc->m_bDoneChgLot)
	{
		double dFdTotLen = GetMkFdLen();
		double dLotCutPos = _tstof(pDoc->WorkingInfo.LastJob.sLotCutPosLen)*1000.0;
		if (dFdTotLen >= dLotCutPos)
		{
			pDoc->WorkingInfo.LastJob.bLotSep = FALSE;
#ifdef USE_MPE
			MpeWrite(_T("MB440184"), 0);	// ��Ʈ�и����(PC�� On��Ű��, PC�� Ȯ���ϰ� Off��Ŵ)-20141031
#endif
			if (m_mgrReelmap->m_pReelMap)
				m_mgrReelmap->m_pReelMap->m_bUseLotSep = FALSE;

			::WritePrivateProfileString(_T("Last Job"), _T("Use Lot seperate"), _T("0"), PATH_WORKING_INFO);

			if (m_pDlgMenu01)
				m_pDlgMenu01->UpdateData();

			return TRUE;
		}
	}

	return FALSE;
}

void CGvisR2R_PunchView::SetFixPcs(int nSerial)
{
	m_mgrReelmap->SetFixPcs(nSerial);
}

BOOL CGvisR2R_PunchView::DoElecChk(CString &sRst)
{
	return m_mgrPunch->DoElecChk(sRst);
}
//BOOL CGvisR2R_PunchView::DoElecChk(CString &sRst)
//{
//	BOOL bDone = FALSE;
//#ifdef USE_FLUCK
//	double dVal;
//	if (!pDoc->WorkingInfo.Fluck.bUse)
//		return TRUE;
//
//	if (!pDoc->WorkingInfo.Probing[0].bUse)
//		return TRUE;
//
//	if (!m_pFluck || !m_mgrPunch->m_pVoiceCoil[0] || !m_mgrPunch->m_pVoiceCoil[1])
//		return FALSE;
//
//	switch (m_nStepElecChk)
//	{
//	case 0: // Move to left point for measuring.
//		pDoc->m_sElecChk = _T("");
//		m_dElecChkVal = 0.0;
//		pDoc->WorkingInfo.Probing[0].nRepeatTest = 0;
//		MoveMeasPos(0);
//		m_nStepElecChk++;
//		break;
//	case 1: // Check move done
//		if (m_mgrPunch->m_pMotion->IsMotionDone(MS_X0))
//			m_nStepElecChk++;
//		break;
//	case 2: // Move to right point for measuring.
//		MoveMeasPos(1);
//		m_nStepElecChk++;
//		break;
//	case 3: // Check move done
//		if (m_mgrPunch->m_pMotion->IsMotionDone(MS_X1))
//			m_nStepElecChk++;
//		break;
//	case 4: // Delay
//		m_nStepElecChk++;
//		break;
//	case 5: // left & right Prob down
//		if (m_mgrPunch->m_pVoiceCoil[0])
//			m_mgrPunch->m_pVoiceCoil[0]->SetProbing(0);
//		m_nStepElecChk++;
//		break;
//	case 6: // Delay
//		Sleep(100);
//		m_nStepElecChk++;
//		break;
//	case 7: // left & right Prob down
//		if (m_mgrPunch->m_pVoiceCoil[1])
//			m_mgrPunch->m_pVoiceCoil[1]->SetProbing(1);
//		m_nStepElecChk++;
//		break;
//	case 8: // Delay
//		Sleep(30);
//		m_nStepElecChk++;
//		break;
//	case 9: // Delay
//		m_pFluck->Clear();
//		Sleep(100);
//		m_nStepElecChk++;
//		break;
//	case 10: // Delay
//		m_nStepElecChk++;
//		break;
//	case 11: // Measure registance
//		m_pFluck->Trig();
//		m_nStepElecChk++;
//		break;
//	case 12: // Delay
//		Sleep(100);
//		if (m_pFluck->Get(dVal))
//		{
//			m_dElecChkVal += dVal;
//			m_nStepElecChk++;
//		}
//		m_nStepElecChk++;
//		break;
//	case 13: // Measure registance
//			 //m_pFluck->Clear();
//		m_pFluck->Trig();
//		m_nStepElecChk++;
//		break;
//	case 14: // Delay
//
//		Sleep(100);
//		if (m_pFluck->Get(dVal))
//		{
//			m_dElecChkVal += dVal;
//			m_nStepElecChk++;
//		}
//		break;
//	case 15: // Measure registance
//			 //m_pFluck->Clear();
//		m_pFluck->Trig();
//		m_nStepElecChk++;
//		break;
//	case 16: // Delay
//		Sleep(100);
//		if (m_pFluck->Get(dVal))
//		{
//			m_dElecChkVal += dVal;
//			m_nStepElecChk++;
//		}
//		break;
//	case 17: // Result OK , NG
//		dVal = m_dElecChkVal / 3.0;
//
//		if (dVal >= FLUCK_OVER)
//		{
//			sRst.Format(_T("Open"), dVal);
//			if (pDoc->WorkingInfo.Probing[0].nRepeatTest < _tstoi(pDoc->WorkingInfo.Probing[0].sRepeatTestNum))
//			{
//				pDoc->WorkingInfo.Probing[0].nRepeatTest++;
//				// 				m_mgrPunch->m_pVoiceCoil[0]->SearchHomeSmac0();
//				// 				Sleep(100);
//				// 				m_mgrPunch->m_pVoiceCoil[1]->SearchHomeSmac1();
//				Sleep(100);
//				m_nStepElecChk = 4;
//				break;
//			}
//		}
//		else if (dVal == FLUCK_ERROR)
//		{
//			sRst.Format(_T("Error"), dVal);
//			if (pDoc->WorkingInfo.Probing[0].nRepeatTest < _tstoi(pDoc->WorkingInfo.Probing[0].sRepeatTestNum))
//			{
//				pDoc->WorkingInfo.Probing[0].nRepeatTest++;
//				// 				m_mgrPunch->m_pVoiceCoil[0]->SearchHomeSmac0();
//				// 				Sleep(100);
//				// 				m_mgrPunch->m_pVoiceCoil[1]->SearchHomeSmac1();
//				Sleep(100);
//				m_nStepElecChk = 4;
//				break;
//			}
//		}
//		else
//		{
//			sRst.Format(_T("%8f"), dVal);
//		}
//
//		pDoc->m_sElecChk = sRst;
//		m_nStepElecChk++;
//		break;
//	case 18: // Write Data
//		pDoc->WriteElecData(pDoc->m_sElecChk);
//		m_nStepElecChk++;
//		break;
//	case 19: // left & right Prob up
//			 //		m_mgrPunch->m_pVoiceCoil[0]->MoveSmacShiftPos0();
//		m_mgrPunch->m_pVoiceCoil[1]->SearchHomeSmac1();
//		m_nStepElecChk++;
//		break;
//	case 20: // Delay
//		m_nStepElecChk++;
//		break;
//	case 21: // left & right Prob up
//			 //		m_mgrPunch->m_pVoiceCoil[1]->MoveSmacShiftPos1();
//		m_mgrPunch->m_pVoiceCoil[0]->SearchHomeSmac0();
//		m_nStepElecChk++;
//		break;
//	case 22: // Delay
//		m_nStepElecChk++;
//		break;
//	case 23: // Move to left init pos
//			 //		if(m_mgrPunch->m_pVoiceCoil[0]->IsDoneMoveSmacShiftPos0() && m_mgrPunch->m_pVoiceCoil[1]->IsDoneMoveSmacShiftPos1())
//		if (m_mgrPunch->m_pVoiceCoil[0]->IsDoneSearchHomeSmac0() && m_mgrPunch->m_pVoiceCoil[1]->IsDoneSearchHomeSmac1())
//		{
//			if (!IsInitPos0())
//				m_mgrPunch->MoveInitPos0();
//			if (!IsInitPos1())
//				m_mgrPunch->MoveInitPos1();
//			m_nStepElecChk++;
//		}
//		break;
//	case 24: // Move to right init pos
//		if (IsMoveDone0() && IsMoveDone1())
//		{
//			//Sleep(500);
//			m_nStepElecChk++;
//		}
//		break;
//	case 25: // Move to left init pos
//		m_mgrPunch->m_pVoiceCoil[1]->MoveSmacShiftPos1();
//		m_nStepElecChk++;
//		break;
//	case 26: // Delay
//		m_nStepElecChk++;
//		break;
//	case 27: // Move to left init pos
//		m_mgrPunch->m_pVoiceCoil[0]->MoveSmacShiftPos0();
//		m_nStepElecChk++;
//		break;
//	case 28: // Delay
//		m_nStepElecChk++;
//		break;
//	case 29: // Move to left init pos
//		if (m_mgrPunch->m_pVoiceCoil[0]->IsDoneMoveSmacShiftPos0() && m_mgrPunch->m_pVoiceCoil[1]->IsDoneMoveSmacShiftPos1())
//		{
//			m_nStepElecChk++;
//		}
//		break;
//	case 30: // Delay
//			 //Sleep(100);
//		m_nStepElecChk++;
//		break;
//	case 31: // Done Elec Check.
//		bDone = TRUE;
//		break;
//	}
//
//	sRst = pDoc->m_sElecChk;
//#else
//	bDone = TRUE;
//#endif
//	return bDone;
//}

//BOOL CGvisR2R_PunchView::MoveMeasPos(int nId)
//{
//	if (!m_mgrPunch->m_pMotion)
//		return FALSE;
//
//	if (!m_pLight)
//		return FALSE;
//
//	if (nId == 0)
//	{
//		if (m_pDlgMenu02)
//			m_pDlgMenu02->SetLight();
//
//		double dMkOffsetX, dMkOffsetY;
//		if (pDoc->WorkingInfo.Vision[0].sMkOffsetX.IsEmpty())
//			dMkOffsetX = 0.0;
//		else
//			dMkOffsetX = _tstof(pDoc->WorkingInfo.Vision[0].sMkOffsetX);
//
//		if (pDoc->WorkingInfo.Vision[0].sMkOffsetY.IsEmpty())
//			dMkOffsetY = 0.0;
//		else
//			dMkOffsetY = _tstof(pDoc->WorkingInfo.Vision[0].sMkOffsetY);
//
//
//		double pPos[2];
//		if (m_mgrProcedure->m_bFailAlign[0][0] || m_mgrProcedure->m_bFailAlign[0][1])
//		{
//			pPos[0] = _tstof(pDoc->WorkingInfo.Probing[nId].sMeasurePosX) + dMkOffsetX;
//			pPos[1] = _tstof(pDoc->WorkingInfo.Probing[nId].sMeasurePosY) + dMkOffsetY;
//		}
//		else
//		{
//			pPos[0] = _tstof(pDoc->WorkingInfo.Probing[nId].sMeasurePosX) + dMkOffsetX - m_dMkFdOffsetX[0][0];
//			pPos[1] = _tstof(pDoc->WorkingInfo.Probing[nId].sMeasurePosY) + dMkOffsetY - m_dMkFdOffsetY[0][0];
//		}
//
//		if (pPos[1] > 0.0 && pPos[0] > 0.0)
//		{
//			double dCurrX = m_dEnc[AXIS_X0];
//			double dCurrY = m_dEnc[AXIS_Y0];
//
//			if (m_mgrProcedure->ChkCollision(AXIS_X0, pPos[0]))
//			{
//				CfPoint ptPnt;
//				ptPnt.x = _tstof(pDoc->WorkingInfo.Motion.sSafeZone);
//				ptPnt.y = 0.0; // m_dEnc[AXIS_Y1];
//				Move1(ptPnt);
//			}
//
//			double fLen, fVel, fAcc, fJerk;
//			fLen = sqrt(((pPos[0] - dCurrX) * (pPos[0] - dCurrX)) + ((pPos[1] - dCurrY) * (pPos[1] - dCurrY)));
//			if (fLen > 0.001)
//			{
//				m_mgrPunch->m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_X0, fLen, fVel, fAcc, fJerk);
//				if (!m_mgrPunch->m_pMotion->Move(MS_X0Y0, pPos, fVel, fAcc, fAcc))
//				{
//					if (!m_mgrPunch->m_pMotion->Move(MS_X0Y0, pPos, fVel, fAcc, fAcc))
//					{
//						pView->ClrDispMsg();
//						AfxMessageBox(_T("Move XY Error..."));
//					}
//				}
//			}
//
//			return TRUE;
//		}
//	}
//	else if (nId == 1)
//	{
//		if (m_pDlgMenu02)
//			m_pDlgMenu02->SetLight2();
//
//		double dMkOffsetX, dMkOffsetY;
//		if (pDoc->WorkingInfo.Vision[1].sMkOffsetX.IsEmpty())
//			dMkOffsetX = 0.0;
//		else
//			dMkOffsetX = _tstof(pDoc->WorkingInfo.Vision[1].sMkOffsetX);
//
//		if (pDoc->WorkingInfo.Vision[1].sMkOffsetY.IsEmpty())
//			dMkOffsetY = 0.0;
//		else
//			dMkOffsetY = _tstof(pDoc->WorkingInfo.Vision[1].sMkOffsetY);
//
//
//		double pPos[2];
//		if (m_mgrProcedure->m_bFailAlign[1][0] || m_mgrProcedure->m_bFailAlign[1][1])
//		{
//			pPos[0] = _tstof(pDoc->WorkingInfo.Probing[nId].sMeasurePosX) + dMkOffsetX;
//			pPos[1] = _tstof(pDoc->WorkingInfo.Probing[nId].sMeasurePosY) + dMkOffsetY;
//		}
//		else
//		{
//			pPos[0] = _tstof(pDoc->WorkingInfo.Probing[nId].sMeasurePosX) + dMkOffsetX - m_dMkFdOffsetX[1][0];
//			pPos[1] = _tstof(pDoc->WorkingInfo.Probing[nId].sMeasurePosY) + dMkOffsetY - m_dMkFdOffsetY[1][0];
//		}
//
//		if (pPos[1] > 0.0 && pPos[0] > 0.0)
//		{
//			double dCurrX = m_dEnc[AXIS_X1];
//			double dCurrY = m_dEnc[AXIS_Y1];
//
//			if (m_mgrProcedure->ChkCollision(AXIS_X1, pPos[0]))
//			{
//				CfPoint ptPnt;
//				ptPnt.x = 0.0;
//				ptPnt.y = 0.0;//m_dEnc[AXIS_Y0];
//				Move0(ptPnt);
//			}
//
//			double fLen, fVel, fAcc, fJerk;
//			fLen = sqrt(((pPos[0] - dCurrX) * (pPos[0] - dCurrX)) + ((pPos[1] - dCurrY) * (pPos[1] - dCurrY)));
//			if (fLen > 0.001)
//			{
//				m_mgrPunch->m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_X0, fLen, fVel, fAcc, fJerk);
//				if (!m_mgrPunch->m_pMotion->Move(MS_X1Y1, pPos, fVel, fAcc, fAcc))
//				{
//					if (!m_mgrPunch->m_pMotion->Move(MS_X1Y1, pPos, fVel, fAcc, fAcc))
//					{
//						pView->ClrDispMsg();
//						AfxMessageBox(_T("Move XY Error..."));
//					}
//				}
//			}
//
//			return TRUE;
//		}
//	}
//
//	return FALSE;
//}

//void CGvisR2R_PunchView::DoInterlock()
//{
//	if (m_dEnc[AXIS_Y0] < 20.0 && m_dEnc[AXIS_Y1] < 20.0)
//	{
//		if (m_mgrProcedure->m_bStopFeeding)
//		{
//			m_mgrProcedure->m_bStopFeeding = FALSE;
//			MpeWrite(_T("MB440115"), 0); // ��ŷ��Feeding����
//		}
//	}
//	else
//	{
//		if (!m_mgrProcedure->m_bStopFeeding)
//		{
//			m_mgrProcedure->m_bStopFeeding = TRUE;
//			MpeWrite(_T("MB440115"), 1); // ��ŷ��Feeding����
//		}
//	}
//}

//BOOL CGvisR2R_PunchView::ChkLightErr()
//{
//	int nSerial, nErrCode;
//	BOOL bError = FALSE;
//	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
//
//	nSerial = m_mgrProcedure->m_nBufUpSerial[0]; // Cam0
//
//	if (m_mgrProcedure->m_bSerialDecrese)
//	{
//		if (nSerial > 0 && nSerial >= m_mgrProcedure->m_nLotEndSerial)
//		{
//			if ((nErrCode = GetErrCode(nSerial)) != 1)
//			{
//				if (nErrCode == -1) // -1(Align Error, �뱤�ҷ�)
//					bError = TRUE;
//			}
//		}
//
//		nSerial = m_mgrProcedure->m_nBufUpSerial[1]; // Cam1
//
//		if (nSerial > 0 && nSerial >= m_mgrProcedure->m_nLotEndSerial)
//		{
//			if ((nErrCode = GetErrCode(nSerial)) != 1)
//			{
//				if (nErrCode == -1) // -1(Align Error, �뱤�ҷ�)
//					bError = TRUE;
//			}
//		}
//	}
//	else
//	{
//		if (nSerial > 0 && nSerial <= m_mgrProcedure->m_nLotEndSerial)
//		{
//			if ((nErrCode = GetErrCode(nSerial)) != 1)
//			{
//				if (nErrCode == -1) // -1(Align Error, �뱤�ҷ�)
//					bError = TRUE;
//			}
//		}
//
//		nSerial = m_mgrProcedure->m_nBufUpSerial[1]; // Cam1
//
//		if (nSerial > 0 && nSerial <= m_mgrProcedure->m_nLotEndSerial)
//		{
//			if ((nErrCode = GetErrCode(nSerial)) != 1)
//			{
//				if (nErrCode == -1) // -1(Align Error, �뱤�ҷ�)
//					bError = TRUE;
//			}
//		}
//	}
//
//	if (bError)
//	{
//		Stop();
//		//TowerLamp(RGB_RED, TRUE);
//		Buzzer(TRUE, 0);
//		DispMain(_T("�� ��"), RGB_RED);
//	}
//
//	return bError;
//}

//void CGvisR2R_PunchView::CntMk()
//{
//#ifdef USE_MPE
//	if (m_mgrProcedure->m_nPrevTotMk[0] != m_mgrProcedure->m_nTotMk[0])
//	{
//		m_mgrProcedure->m_nPrevTotMk[0] = m_mgrProcedure->m_nTotMk[0];
//		MpeWrite(_T("ML45096"), (long)m_mgrProcedure->m_nTotMk[0]);	// ��ŷ�� (��) �� ��ŷ�� 
//	}
//	if (m_mgrProcedure->m_nPrevCurMk[0] != m_mgrProcedure->m_nMkPcs[0])//m_nCurMk[0])
//	{
//		m_mgrProcedure->m_nPrevCurMk[0] = m_mgrProcedure->m_nMkPcs[0];//m_nCurMk[0];
//		MpeWrite(_T("ML45098"), (long)m_mgrProcedure->m_nMkPcs[0]);	// ��ŷ�� (��) ���� ��ŷ�� ��
//	}
//
//	if (m_mgrProcedure->m_nPrevTotMk[1] != m_mgrProcedure->m_nTotMk[1])
//	{
//		m_mgrProcedure->m_nPrevTotMk[1] = m_mgrProcedure->m_nTotMk[1];
//		MpeWrite(_T("ML45100"), (long)m_mgrProcedure->m_nTotMk[1]);	// ��ŷ�� (��) �� ��ŷ�� 
//	}
//	if (m_mgrProcedure->m_nPrevCurMk[1] != m_mgrProcedure->m_nMkPcs[1])//m_nCurMk[1])
//	{
//		m_mgrProcedure->m_nPrevCurMk[1] = m_mgrProcedure->m_nMkPcs[1];//m_nCurMk[1];
//		MpeWrite(_T("ML45102"), (long)m_mgrProcedure->m_nMkPcs[1]);	// ��ŷ�� (��) ���� ��ŷ�� ��
//	}
//#endif
//}

//BOOL CGvisR2R_PunchView::IsOnMarking0()
//{
//	if (m_mgrProcedure->m_nMkPcs[0] < m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)	// ��ŷ�Ϸ�Check
//		return TRUE;
//
//	return FALSE;
//}
//
//BOOL CGvisR2R_PunchView::IsOnMarking1()
//{
//	if (m_mgrProcedure->m_nMkPcs[1] < m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)	// ��ŷ�Ϸ�Check
//		return TRUE;
//
//	return FALSE;
//}

void CGvisR2R_PunchView::SetDualTest(BOOL bOn)
{
#ifdef USE_MPE
	//if (pView->m_pMpe)
		MpeWrite(_T("MB44017A"), bOn ? 0 : 1);		// �ܸ� �˻� On
#endif
	if (m_pDlgFrameHigh)
		m_pDlgFrameHigh->SetDualTest(bOn);
	if (m_pDlgMenu01)
		m_pDlgMenu01->SetDualTest(bOn);
	if (m_pDlgMenu03)
		m_pDlgMenu03->SetDualTest(bOn);
	if (m_pDlgMenu06)
		m_pDlgMenu06->SetDualTest(bOn);
}

void CGvisR2R_PunchView::SetTwoMetal(BOOL bSel, BOOL bOn)
{
	if (bSel)
	{
		if (bOn)
		{
			pDoc->WorkingInfo.LastJob.bTwoMetal = TRUE;
			MpeWrite(_T("MB44017C"), 1);
			::WritePrivateProfileString(_T("Last Job"), _T("Two Metal On"), _T("1"), PATH_WORKING_INFO);// IDC_CHK_TWO_METAL - Uncoiler\r������ ON : TRUE	
		}
		else
		{
			pDoc->WorkingInfo.LastJob.bTwoMetal = FALSE;
			MpeWrite(_T("MB44017C"), 0);
			::WritePrivateProfileString(_T("Last Job"), _T("Two Metal On"), _T("0"), PATH_WORKING_INFO);// IDC_CHK_TWO_METAL - Uncoiler\r������ ON : TRUE	
		}

//#ifdef USE_ENGRAVE
//		if (pView && pView->m_mgrPunch->m_pEngrave)
//			pView->m_mgrPunch->m_pEngrave->SetUncoilerCcw();	//_stSigInx::_UncoilerCcw
//#endif
	}
	else
	{
		if (bOn)
		{
			pDoc->WorkingInfo.LastJob.bOneMetal = TRUE;
			MpeWrite(_T("MB44017D"), 1);
			::WritePrivateProfileString(_T("Last Job"), _T("One Metal On"), _T("1"), PATH_WORKING_INFO);// IDC_CHK_ONE_METAL - Recoiler\r������ CW : FALSE
		}
		else
		{
			pDoc->WorkingInfo.LastJob.bOneMetal = FALSE;
			MpeWrite(_T("MB44017D"), 0);
			::WritePrivateProfileString(_T("Last Job"), _T("One Metal On"), _T("0"), PATH_WORKING_INFO);// IDC_CHK_ONE_METAL - Recoiler\r������ CW : FALSE
		}

//#ifdef USE_ENGRAVE
//		if (pView && pView->m_mgrPunch->m_pEngrave)
//			pView->m_mgrPunch->m_pEngrave->SetRecoilerCcw();	//_stSigInx::_RecoilerCcw
//#endif
	}
}

void CGvisR2R_PunchView::AdjPinPos()
{
	if (m_pDlgMenu02)
	{
		if (m_dMkFdOffsetY[0][0] > -2.0 && m_dMkFdOffsetY[1][0] > -2.0 &&
			m_dMkFdOffsetY[0][0] < 2.0 && m_dMkFdOffsetY[1][0] < 2.0)
		{
			double dOffsetY0 = -1.0 * m_dMkFdOffsetY[0][0];
			dOffsetY0 *= pDoc->m_dShiftAdjustRatio;
			double dOffsetY1 = -1.0 * m_dMkFdOffsetY[1][0];
			dOffsetY1 *= pDoc->m_dShiftAdjustRatio;
			//double dOffsetY = -1.0*(m_dMkFdOffsetY[0][0] + m_dMkFdOffsetY[1][0]) / 2.0;
			//dOffsetY *= pDoc->m_dShiftAdjustRatio;

			CfPoint ptPnt[2];
			ptPnt[0].x = _tstof(pDoc->WorkingInfo.Motion.sPinPosX[0]);
			ptPnt[0].y = _tstof(pDoc->WorkingInfo.Motion.sPinPosY[0]) + dOffsetY0;
			//ptPnt[0].y = _tstof(pDoc->WorkingInfo.Motion.sPinPosY[0]) + dOffsetY;
			ptPnt[1].x = _tstof(pDoc->WorkingInfo.Motion.sPinPosX[1]);
			ptPnt[1].y = _tstof(pDoc->WorkingInfo.Motion.sPinPosY[1]) + dOffsetY1;
			//ptPnt[1].y = _tstof(pDoc->WorkingInfo.Motion.sPinPosY[1]) + dOffsetY;

			m_pDlgMenu02->SetPinPos(0, ptPnt[0]);
			m_pDlgMenu02->SetPinPos(1, ptPnt[1]);

			CString sData, sPath = PATH_WORKING_INFO;
			pDoc->WorkingInfo.Fluck.dMeasPosY[0] = _tstof(pDoc->WorkingInfo.Probing[0].sMeasurePosY) + dOffsetY0;
			//pDoc->WorkingInfo.Fluck.dMeasPosY[0] = _tstof(pDoc->WorkingInfo.Probing[0].sMeasurePosY) + dOffsetY;
			sData.Format(_T("%.2f"), pDoc->WorkingInfo.Fluck.dMeasPosY[0]);
			pDoc->WorkingInfo.Probing[0].sMeasurePosY = sData;
			::WritePrivateProfileString(_T("Probing0"), _T("PROBING_MEASURE_POSY"), sData, sPath);

			pDoc->WorkingInfo.Fluck.dMeasPosY[1] = _tstof(pDoc->WorkingInfo.Probing[1].sMeasurePosY) + dOffsetY1;
			//pDoc->WorkingInfo.Fluck.dMeasPosY[1] = _tstof(pDoc->WorkingInfo.Probing[1].sMeasurePosY) + dOffsetY;
			sData.Format(_T("%.2f"), pDoc->WorkingInfo.Fluck.dMeasPosY[1]);
			pDoc->WorkingInfo.Probing[1].sMeasurePosY = sData;
			::WritePrivateProfileString(_T("Probing1"), _T("PROBING_MEASURE_POSY"), sData, sPath);
		}
	}
}

// Engrave
void CGvisR2R_PunchView::SetEngraveSts(int nStep)
{
	if (!m_pDlgMenu03)
		return;

	// ��ŷ�� - TBL�ı� OFF, TBL���� ON, FD/TQ ���� OFF, 
	switch (nStep)
	{
	case 0:
		m_pDlgMenu03->SwMkTblBlw(FALSE);
		m_pDlgMenu03->SwMkFdVac(FALSE);
		m_pDlgMenu03->SwMkTqVac(FALSE);
		break;
	case 1:
		m_pDlgMenu03->SwMkTblVac(TRUE);
		break;
	}
}

void CGvisR2R_PunchView::SetEngraveStopSts()
{
	if (!m_pDlgMenu03)
		return;

	// ��ŷ�� - FD/TQ ���� ON, TBL���� OFF, TBL�ı� ON, 
	m_pDlgMenu03->SwMkTblBlw(FALSE);
	m_pDlgMenu03->SwMkTblVac(FALSE);
	m_pDlgMenu03->SwMkFdVac(FALSE);
	// 	m_pDlgMenu03->SwMkTqVac(FALSE);
}

void CGvisR2R_PunchView::SetEngraveFdSts()
{
	if (!m_pDlgMenu03)
		return;

	// ��ŷ�� - FD/TQ ���� ON, TBL���� OFF, TBL�ı� ON, 
	m_pDlgMenu03->SwMkFdVac(TRUE);
	// 	m_pDlgMenu03->SwMkTqVac(TRUE);
	m_pDlgMenu03->SwMkTblVac(FALSE);
	m_pDlgMenu03->SwMkTblBlw(TRUE);
}

BOOL CGvisR2R_PunchView::IsEngraveFdSts()
{
	if (!m_pDlgMenu03)
		return FALSE;

	BOOL bOn[4] = { 0 };
	// ��ŷ�� - FD/TQ ���� ON, TBL���� OFF, TBL�ı� ON, 
	bOn[0] = m_pDlgMenu03->IsMkFdVac(); // TRUE
	bOn[1] = m_pDlgMenu03->IsMkTqVac(); // TRUE
	bOn[2] = m_pDlgMenu03->IsMkTblVac(); // FALSE
	bOn[3] = m_pDlgMenu03->IsMkTblBlw(); // TRUE

	if (bOn[0] && bOn[1] && !bOn[2] && bOn[3])
		return TRUE;

	return FALSE;
}

void CGvisR2R_PunchView::SetEngraveFd()
{
	if (!pDoc->WorkingInfo.LastJob.bAoiOnePnl)
	{
#ifdef USE_MPE
		MpeWrite(_T("MB440151"), 1);	// ���ǳ� �̼ۻ��� ON (PC�� ON, OFF)
#endif
		CString sData, sPath = PATH_WORKING_INFO;
		pDoc->WorkingInfo.LastJob.bMkOnePnl = pDoc->WorkingInfo.LastJob.bAoiOnePnl = TRUE;
		sData.Format(_T("%d"), pDoc->WorkingInfo.LastJob.bMkOnePnl ? 1 : 0);
		::WritePrivateProfileString(_T("Last Job"), _T("Marking One Pannel Move On"), sData, sPath);
		::WritePrivateProfileString(_T("Last Job"), _T("AOI One Pannel Move On"), sData, sPath);
	}
}

void CGvisR2R_PunchView::SetEngraveFd(double dDist)
{
	double fLen = pDoc->GetOnePnlLen();
	double dOffset = dDist - (fLen*2.0);
	m_mgrPunch->MoveMk(dOffset);
}

void CGvisR2R_PunchView::MoveEngrave(double dOffset)
{
#ifdef USE_MPE
	long lData = (long)(dOffset * 1000.0);
	MpeWrite(_T("MB440199"), 1);		// ���κ� �ǵ� CW ON (PLC�� �ǵ��Ϸ� �� OFF)
	MpeWrite(_T("ML45078"), lData);		// ���κ� Feeding �ѷ� Offset(*1000, +:�� ����, -�� ����, PC�� ���� PLC���� ����)
#endif
}

BOOL CGvisR2R_PunchView::IsEngraveFd()
{
	if (m_mgrProcedure->m_nShareDnCnt > 0)
	{
		if (!(m_mgrProcedure->m_nShareDnCnt % 2))
		{
#ifdef USE_MPE
			if (pDoc->m_pMpeSignal[5] & (0x01 << 1))	// ��ŷ�� �ǵ� CW ON (PLC�� �ǵ��Ϸ� �� OFF)
				return TRUE;
#endif
			return FALSE;
		}
	}
	else
	{
		if (m_mgrProcedure->m_nShareUpCnt > 0)
		{
			if (!(m_mgrProcedure->m_nShareUpCnt % 2))
			{
#ifdef USE_MPE
				if (pDoc->m_pMpeSignal[5] & (0x01 << 1))	// ��ŷ�� �ǵ� CW ON (PLC�� �ǵ��Ϸ� �� OFF)
					return TRUE;
#endif
				return FALSE;
			}
		}
	}

	return TRUE;
}

double CGvisR2R_PunchView::GetEngraveFdLen()
{
	int nLast = pDoc->GetLastShotEngrave();

	double dLen = (double)nLast * _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen);
	return dLen;
}

double CGvisR2R_PunchView::GetAoiInitDist()
{
	double dInitD1 = _tstof(pDoc->WorkingInfo.Motion.sFdEngraveAoiInitDist) - _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen);
	return dInitD1;
}

double CGvisR2R_PunchView::GetAoiRemain()
{
#ifdef USE_MPE
	double dCurPosEngraveFd = (double)pDoc->m_pMpeData[1][0];	// ML44052	,	���κ� Feeding ���ڴ� ��(���� mm)
	double dRemain = _tstof(pDoc->WorkingInfo.Motion.sFdEngraveAoiInitDist) - dCurPosEngraveFd;
	return dRemain;
#else
	return 0.0;
#endif
}

//LRESULT CGvisR2R_PunchView::wmClientReceived(WPARAM wParam, LPARAM lParam)
//{
//	if (!m_mgrPunch->m_pEngrave)
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
//		if (m_mgrPunch->m_pEngrave && m_mgrPunch->m_pEngrave->IsConnected())
//			m_mgrPunch->m_pEngrave->GetSysSignal(rSockData);
//
//		pView->m_bSetSig = TRUE;
//		break;
//	case _SetData:
//		if (m_mgrPunch->m_pEngrave && m_mgrPunch->m_pEngrave->IsConnected())
//			m_mgrPunch->m_pEngrave->GetSysData(rSockData);
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
//
//LRESULT CGvisR2R_PunchView::wmClientReceivedSr(WPARAM wParam, LPARAM lParam)
//{
//	int nCmd = (int)wParam;
//	CString* sReceived = (CString*)lParam;
//
//	switch (nCmd)
//	{
//	case SrTriggerInputOn:
//		Get2dCode(m_sGet2dCodeLot, m_nGet2dCodeSerial);
//		break;
//	default:
//		break;
//	}
//
//	return (LRESULT)1;
//}

void CGvisR2R_PunchView::SetEngraveFdPitch(double dPitch)
{
	pDoc->SetEngraveFdPitch(dPitch);
}

BOOL CGvisR2R_PunchView::IsConnectedEng()
{
	if (m_bDestroyedView)
		return FALSE;

#ifdef USE_ENGRAVE
	if (m_mgrPunch->m_pEngrave)
	{
		if(m_mgrPunch->m_pEngrave->IsConnected())
		{
			if(!m_mgrProcedure->m_bContEngraveF)
			{
				m_mgrProcedure->m_bContEngraveF = TRUE;
				DWORD dwStartTick = GetTickCount();
			}
			return TRUE;
		}
		else
		{
			if (m_mgrProcedure->m_bContEngraveF)
			{
				m_mgrProcedure->m_bContEngraveF = FALSE;
			}
			return FALSE;
		}
	}
#endif
	return FALSE;
}

//BOOL CGvisR2R_PunchView::IsPinPos0()
//{
//	if (!m_mgrPunch->m_pMotion)
//		return FALSE;
//
//	double pPos[2];
//	pPos[0] = m_mgrPunch->m_pMotion->m_dPinPosX[0];
//	pPos[1] = m_mgrPunch->m_pMotion->m_dPinPosY[0];
//
//	double dCurrX = m_dEnc[AXIS_X0];
//	double dCurrY = m_dEnc[AXIS_Y0];
//
//	if (dCurrX < pPos[0] - 4.0 || dCurrX > pPos[0] + 4.0)
//		return FALSE;
//	if (dCurrY < pPos[1] - 4.0 || dCurrY > pPos[1] + 4.0)
//		return FALSE;
//
//	return TRUE;
//}
//
//BOOL CGvisR2R_PunchView::IsPinPos1()
//{
//	if (!m_mgrPunch->m_pMotion)
//		return FALSE;
//
//	double pPos[2];
//	pPos[0] = m_mgrPunch->m_pMotion->m_dPinPosX[1];
//	pPos[1] = m_mgrPunch->m_pMotion->m_dPinPosY[1];
//
//	double dCurrX = m_dEnc[AXIS_X1];
//	double dCurrY = m_dEnc[AXIS_Y1];
//
//	if (dCurrX < pPos[0] - 4.0 || dCurrX > pPos[0] + 4.0)
//		return FALSE;
//	if (dCurrY < pPos[1] - 4.0 || dCurrY > pPos[1] + 4.0)
//		return FALSE;
//
//	return TRUE;
//}

void CGvisR2R_PunchView::RunShift2Mk()
{
	Shift2Mk();			// PCR �̵�(Buffer->Marked) // ���(WorkingInfo.LastJob.sSerial)
}

BOOL CGvisR2R_PunchView::LoadMasterSpec()
{
	return TRUE;
}

// Call from Thread Function
void CGvisR2R_PunchView::UpdateYield(int nSerial)
{
	m_mgrReelmap->UpdateYield(nSerial);
}

void CGvisR2R_PunchView::UpdateYield()
{
	int nSerial = m_mgrProcedure->m_nBufUpSerial[0];
	if (nSerial < 1)
	{
		Stop();
		MsgBox(_T("Error-UpdateYield() : m_nBufUpSerial[0] < 1"));
		return;
	}

	m_mgrReelmap->UpdateYieldOnThread(nSerial);
}

//void CGvisR2R_PunchView::DoAutoEng()
//{
//	//if (!IsAuto() || (MODE_INNER != pDoc->WorkingInfo.LastJob.nTestMode))
//	if (!IsAuto() || (MODE_INNER != pDoc->GetTestMode()))
//		return;
//
//	// ���κ� ��ŷ���� ��ȣ�� Ȯ��
//	DoAtuoGetEngStSignal();
//
//	// ���κ� 2D �ڵ� Reading��ȣ�� Ȯ��
//	DoAtuoGet2dReadStSignal();
//}

//void CGvisR2R_PunchView::DoAtuoGetEngStSignal()
//{
//#ifdef USE_MPE
//	if ((pDoc->m_pMpeSignal[0] & (0x01 << 3) || m_mgrProcedure->m_bEngStSw) && !pDoc->BtnStatus.EngAuto.MkStF)// 2D(GUI) ���� ���� Start��ȣ(PLC On->PC Off)
//	{
//		pDoc->BtnStatus.EngAuto.MkStF = TRUE;
//		m_mgrProcedure->m_bEngStSw = FALSE;
//
//		pDoc->BtnStatus.EngAuto.IsMkSt = FALSE;
//		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqMkSt, TRUE);
//	}
//	else if (pDoc->BtnStatus.EngAuto.IsMkSt && pDoc->BtnStatus.EngAuto.MkStF)
//	{
//		pDoc->BtnStatus.EngAuto.MkStF = FALSE;
//		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqMkSt, FALSE);
//		//if (m_pMpe)
//		{
//			MpeWrite(_T("MB440103"), 0);			// 2D(GUI) ���� ���� Start��ȣ(PLC On->PC Off)
//
//			//if (pDoc->m_pMpeSignal[0] & (0x01 << 2))	// ���κ� Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)
//			//	MpeWrite(_T("MB440102"), 0);		// ���κ� Feeding�Ϸ�
//		}
//	}
//
//	if (pDoc->m_pMpeSignal[0] & (0x01 << 2) && !pDoc->BtnStatus.EngAuto.FdDoneF)	// ���κ� Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)
//	{
//		pDoc->BtnStatus.EngAuto.FdDoneF = TRUE;
//
//		pDoc->BtnStatus.EngAuto.IsFdDone = FALSE;
//		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqFdDone, TRUE);
//	}
//	else if (pDoc->BtnStatus.EngAuto.IsFdDone && pDoc->BtnStatus.EngAuto.FdDoneF)
//	{
//		pDoc->BtnStatus.EngAuto.FdDoneF = FALSE;
//		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqFdDone, FALSE);
//		//if (m_pMpe)
//		{
//			MpeWrite(_T("MB440102"), 0);		// ���κ� Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)
//		}
//	}
//
//#endif
//}

//void CGvisR2R_PunchView::DoAtuoGet2dReadStSignal()
//{
//	if ((pDoc->m_pMpeSignal[0] & (0x01 << 5) || m_mgrProcedure->m_bEng2dStSw) && !pDoc->BtnStatus.EngAuto.Read2dStF)// ���κ� 2D ���� ���۽�ȣ(PLC On->PC Off)
//	{
//		pDoc->BtnStatus.EngAuto.Read2dStF = TRUE;
//		m_mgrProcedure->m_bEng2dStSw = FALSE;
//
//		pDoc->BtnStatus.EngAuto.IsRead2dSt = FALSE;
//		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeq2dReadSt, TRUE);
//	}
//	else if (pDoc->BtnStatus.EngAuto.IsRead2dSt && pDoc->BtnStatus.EngAuto.Read2dStF)
//	{
//		pDoc->BtnStatus.EngAuto.Read2dStF = FALSE;
//		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeq2dReadSt, FALSE);
//#ifdef USE_MPE
//		//if (m_pMpe)
//		{
//			MpeWrite(_T("MB440105"), 0);			// ���κ� 2D ���� ���۽�ȣ(PLC On->PC Off)
//			//if (pDoc->m_pMpeSignal[0] & (0x01 << 2))	// ���κ� Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)
//			//	MpeWrite(_T("MB440102"), 0);		// ���κ� Feeding�Ϸ�
//		}
//#endif
//	}
//}

void CGvisR2R_PunchView::SetEngFd()
{
	CfPoint OfSt;
	if (GetEngOffset(OfSt))
	{
		if (m_pDlgMenu02)
		{
			m_dEngFdOffsetX = OfSt.x;
			m_dEngFdOffsetY = OfSt.y;
		}
	}

	MoveEng(-1.0*OfSt.x);
	if (m_pDlgMenu03)
		m_pDlgMenu03->ChkDoneEngrave();

	if (!pDoc->WorkingInfo.LastJob.bAoiOnePnl)
	{
#ifdef USE_MPE
		MpeWrite(_T("MB440151"), 1);	// ���ǳ� �̼ۻ��� ON (PC�� ON, OFF)
#endif
		CString sData, sPath = PATH_WORKING_INFO;
		pDoc->WorkingInfo.LastJob.bMkOnePnl = pDoc->WorkingInfo.LastJob.bAoiOnePnl = pDoc->WorkingInfo.LastJob.bEngraveOnePnl = TRUE;
		sData.Format(_T("%d"), pDoc->WorkingInfo.LastJob.bMkOnePnl ? 1 : 0);
		::WritePrivateProfileString(_T("Last Job"), _T("Marking One Pannel Move On"), sData, sPath);
		::WritePrivateProfileString(_T("Last Job"), _T("AOI One Pannel Move On"), sData, sPath);
		::WritePrivateProfileString(_T("Last Job"), _T("Engrave One Pannel Move On"), sData, sPath);
	}
}

BOOL CGvisR2R_PunchView::GetEngOffset(CfPoint &OfSt)
{
	return pDoc->GetEngOffset(OfSt);
}

void CGvisR2R_PunchView::MoveEng(double dOffset)
{
	long lData = (long)(dOffset * 1000.0);
	MpeWrite(_T("MB440160"), 1);	// �˻�� �ǵ� ON (PLC�� �ǵ��Ϸ� �� OFF)
	MpeWrite(_T("ML45064"), lData);	// �˻�� Feeding �ѷ� Offset(*1000, +:�� ����, -�� ����)
}

void CGvisR2R_PunchView::SetMyMsgYes()
{
	if (m_pDlgMyMsg)
	{
		if (m_pDlgMyMsg->m_pDlgMyMsgSub01)
		{
			((CDlgMyMsgSub01*)(m_pDlgMyMsg->m_pDlgMyMsgSub01))->ClickYes();
		}
	}
}

void CGvisR2R_PunchView::SetMyMsgNo()
{
	if (m_pDlgMyMsg)
	{
		if (m_pDlgMyMsg->m_pDlgMyMsgSub01)
		{
			((CDlgMyMsgSub01*)(m_pDlgMyMsg->m_pDlgMyMsgSub01))->ClickNo();
		}
	}
}

void CGvisR2R_PunchView::SetMyMsgOk()
{
	if (m_pDlgMyMsg)
	{
		if (m_pDlgMyMsg->m_pDlgMyMsgSub02)
		{
			((CDlgMyMsgSub02*)(m_pDlgMyMsg->m_pDlgMyMsgSub02))->ClickOk();
		}
	}
}

void CGvisR2R_PunchView::InitAutoEngSignal()
{
	pDoc->BtnStatus.EngAuto._Init();

	m_mgrProcedure->m_bEngFdWrite = FALSE;
	m_mgrProcedure->m_bEngFdWriteF = FALSE;
	m_mgrProcedure->m_bEngTest = FALSE;
	m_mgrProcedure->m_bEngTestF = FALSE;

	MpeWrite(_T("MB440103"), 0); // 2D(GUI) ���� ���� Start��ȣ(PLC On->PC Off)
	MpeWrite(_T("MB440173"), 0); // 2D(GUI) ���� ����Running��ȣ(PC On->PC Off)
	MpeWrite(_T("MB440174"), 0); // ���κ� �۾��Ϸ�.(PC�� On, PLC�� Ȯ�� �� Off)

	MpeWrite(_T("MB440105"), 0); // ���κ� 2D ���� ���۽�ȣ(PLC On->PC Off)
	MpeWrite(_T("MB440178"), 0); // ���κ� 2D ���� �۾��� ��ȣ(PC On->PC Off)
	MpeWrite(_T("MB440179"), 0); // ���κ� 2D ���� �۾��Ϸ� ��ȣ.(PC�� On, PLC�� Ȯ�� �� Off)

	MpeWrite(_T("MB440102"), 0); // ���κ� Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)

	pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqMkSt, FALSE);
	pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeq2dReadSt, FALSE);
	pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqOnMkIng, FALSE);
	pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqMkDone, FALSE);
	pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqOnReading2d, FALSE);
	pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeq2dReadDone, FALSE);
	pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqFdDone, FALSE);
}

BOOL CGvisR2R_PunchView::GetCurrentInfoSignal()
{
	pDoc->BtnStatus.EngAuto.IsInit = pDoc->GetCurrentInfoSignal(_SigInx::_EngAutoInit);
	pDoc->BtnStatus.EngAuto.IsMkSt = pDoc->GetCurrentInfoSignal(_SigInx::_EngAutoSeqMkSt);
	pDoc->BtnStatus.EngAuto.IsOnMking = pDoc->GetCurrentInfoSignal(_SigInx::_EngAutoSeqOnMkIng);
	pDoc->BtnStatus.EngAuto.IsMkDone = pDoc->GetCurrentInfoSignal(_SigInx::_EngAutoSeqMkDone);
	pDoc->BtnStatus.EngAuto.IsRead2dSt = pDoc->GetCurrentInfoSignal(_SigInx::_EngAutoSeq2dReadSt);
	pDoc->BtnStatus.EngAuto.IsOnRead2d = pDoc->GetCurrentInfoSignal(_SigInx::_EngAutoSeqOnReading2d);
	pDoc->BtnStatus.EngAuto.IsRead2dDone = pDoc->GetCurrentInfoSignal(_SigInx::_EngAutoSeq2dReadDone);
	pDoc->BtnStatus.EngAuto.IsFdDone = pDoc->GetCurrentInfoSignal(_SigInx::_EngAutoSeqFdDone);
	
	return TRUE;
}

void CGvisR2R_PunchView::SetLastSerialEng(int nSerial)
{
	if (m_pDlgFrameHigh)
		m_pDlgFrameHigh->SetEngraveLastShot(nSerial);

	pDoc->SetLastSerialEng(nSerial);
}

BOOL CGvisR2R_PunchView::GetCurrentInfoEng()
{
	return pDoc->GetCurrentInfoEng();
}

BOOL CGvisR2R_PunchView::IsConnectedSr()
{
	if (m_bDestroyedView)
		return FALSE;

	if (m_mgrPunch->m_pSr1000w)
	{
		return m_mgrPunch->m_pSr1000w->IsConnected();
	}

	return FALSE;
}

BOOL CGvisR2R_PunchView::Set2dRead(BOOL bRun)	// Marking Start
{
	if (!pView || !pView->m_mgrPunch->m_pSr1000w)
		return FALSE;

	return (pView->m_mgrPunch->m_pSr1000w->DoRead2DCode());
}

BOOL  CGvisR2R_PunchView::Is2dReadDone()
{
	if (!pView || !pView->m_mgrPunch->m_pSr1000w)
		return FALSE;

	return (!pView->m_mgrPunch->m_pSr1000w->IsRunning());
}

BOOL CGvisR2R_PunchView::Get2dCode(CString &sLot, int &nSerial)
{
	if (!m_mgrPunch->m_pSr1000w)
		return FALSE;

	CString sData;
	if (m_mgrPunch->m_pSr1000w->Get2DCode(sData))
	{
		int nPos = sData.ReverseFind('-');
		if (nPos != -1)
		{
			pDoc->m_sOrderNum = sData.Left(nPos);
			pDoc->m_sShotNum = sData.Right(sData.GetLength() - nPos - 1);
			pDoc->m_nShotNum = _tstoi(pDoc->m_sShotNum);
			sLot = pDoc->m_sOrderNum;
			nSerial = pDoc->m_nShotNum;
		}
		else
		{
			pView->MsgBox(sData);
		}

		return TRUE;
	}

	return FALSE;
}

CString CGvisR2R_PunchView::GetCurrentDBName()
{
	BOOL bRtn = FALSE;
	CString sName = _T("");

	if (m_mgrPunch->m_pDts)
	{
		if (m_mgrPunch->m_pDts->IsUseDts())
		{
			bRtn = m_mgrPunch->m_pDts->GetCurrentDBName(sName);
			if (!bRtn)
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Error - GetCurrentDBName()."), MB_ICONSTOP | MB_OK);
			}
		}
	}

	return sName;
}

BOOL CGvisR2R_PunchView::GetDtsPieceOut(int nSerial, int* pPcsOutIdx, int& nTotPcsOut)
{
	BOOL bRtn = FALSE;
	int nIdx = m_mgrReelmap->GetPcrIdx(nSerial);					// ����ȭ����� �ε���
	CString sLot = m_mgrReelmap->m_pPcr[0][nIdx]->m_sLot;

	if (m_mgrPunch->m_pDts)
	{
		if (m_mgrPunch->m_pDts->IsUseDts())
		{
			bRtn = m_mgrPunch->m_pDts->LoadPieceOut(sLot, nSerial, pPcsOutIdx, nTotPcsOut);
			if (!bRtn)
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Error - GetDtsPieceOut()."), MB_ICONSTOP | MB_OK);
			}
		}
	}

	return bRtn;
}

//void CGvisR2R_PunchView::DoMark0Its()
//{
//#ifdef TEST_MODE
//	return;
//#endif
//
//	if (!m_mgrPunch->m_bAuto)
//		return;
//
//	int nSerial, nIdx, nErrCode, nRtn;
//	CfPoint ptPnt;
//	CString sMsg;
//	double dStripOut = (m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs / MAX_STRIP_NUM) * _tstof(pDoc->WorkingInfo.LastJob.sStripOutRatio) / 100.0;
//	int nStripOut = int(dStripOut);
//	if (dStripOut > nStripOut)
//		nStripOut++;			// ��Ʈ�� ���� ����
//
//	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
//
//	if (!IsRun())																		// �������¿���
//	{
//		if (IsOnMarking0())																// ��ŷ�߿�
//		{
//			if (m_mgrPunch->m_pMotion->IsEnable(MS_X0) && m_mgrPunch->m_pMotion->IsEnable(MS_Y0))				// ����� Enable�����̰�
//			{
//				if (m_mgrPunch->m_pMotion->IsMotionDone(MS_X0) && m_mgrPunch->m_pMotion->IsMotionDone(MS_Y0))	// ��� Done�����̸�,
//				{
//					if (!IsInitPos0() && !IsPinPos0())									// �ʱ���ġ�� �ƴϰų�, ����ġ�� �ƴҶ�
//						m_mgrPunch->MoveInitPos0();													// �ʱ���ġ�� �̵�
//				}
//			}
//
//			if (m_mgrProcedure->m_nStepMk[0] < 13 && m_mgrProcedure->m_nStepMk[0] > 8) // Mk0();
//			{
//				m_mgrProcedure->m_nStepMk[0] = 8;	// ��ŷ�Ϸ�Check
//			}
//		}
//		return;
//	}
//
//	m_mgrProcedure->m_sDispSts[0].Format(_T("%d"), m_mgrProcedure->m_nStepMk[0]);
//
//	switch (m_mgrProcedure->m_nStepMk[0])
//	{
//	case 0:
//		if (m_mgrProcedure->IsNoMk())
//			ShowLive();
//		m_mgrProcedure->m_nStepMk[0]++;
//		break;
//	case 1:
//		if (!IsInitPos0())
//			m_mgrPunch->MoveInitPos0();
//		m_mgrProcedure->m_nStepMk[0]++;
//		break;
//	case 2:
//
//		nSerial = m_mgrProcedure->m_nBufUpSerial[0]; // Cam0
//
//		if (nSerial > 0)
//		{
//			if ((nErrCode = m_mgrReelmap->GetErrCode0Its(nSerial)) != 1)
//			{
//				m_mgrProcedure->m_nMkPcs[0] = m_mgrReelmap->GetTotDefPcsIts(nSerial);
//				m_mgrProcedure->m_nStepMk[0] = MK_END;
//			}
//			else
//			{
//				m_mgrProcedure->m_nStepMk[0]++;
//			}
//		}
//		else
//		{
//			StopFromThread();
//			AsyncMsgBox(_T("������ �ø����� �����ʽ��ϴ�."), 1);
//			BuzzerFromThread(TRUE, 0);
//			DispMain(_T("�� ��"), RGB_RED);
//		}
//		break;
//	case 3:
//		m_mgrProcedure->m_nStepMk[0]++;
//		break;
//	case 4:
//		m_mgrProcedure->m_nStepMk[0]++;
//		break;
//	case 5:
//		m_mgrProcedure->m_nStepMk[0]++;
//		break;
//	case 6:
//		nSerial = m_mgrProcedure->m_nBufUpSerial[0]; // Cam0
//
//		if (m_mgrProcedure->m_nMkPcs[0] < m_mgrReelmap->GetTotDefPcsIts(nSerial))
//		{
//			if (!m_mgrProcedure->IsNoMk0())
//			{
//				;
//			}
//			else
//			{
//				if (!m_mgrProcedure->IsReview0())
//				{
//					if (m_mgrProcedure->m_bReview)
//					{
//						m_mgrProcedure->m_nMkPcs[0] = m_mgrReelmap->GetTotDefPcsIts(nSerial);
//						m_mgrProcedure->m_nStepMk[0] = MK_END;
//						break;
//					}
//				}
//			}
//			// Punching On�̰ų� Review�̸� �������� ����
//			SetDelay0(100, 1);		// [mSec]
//			m_mgrProcedure->m_nStepMk[0]++;
//		}
//		else
//		{
//			m_mgrProcedure->m_nStepMk[0] = MK_END;
//		}
//		break;
//	case 7:
//		nSerial = m_mgrProcedure->m_nBufUpSerial[0]; // Cam0
//
//		if (!WaitDelay0(1))		// F:Done, T:On Waiting....		// Delay�Ŀ�
//		{
//			m_mgrProcedure->m_nMkPcs[0] = 0;
//
//			if (!m_mgrProcedure->IsNoMk0())										// Punching On�̸�
//			{
//				m_mgrProcedure->m_nStepMk[0]++;
//			}
//			else												// Punching�� Off�̰�
//			{
//				if (m_mgrProcedure->IsReview0())								// Review�̸� ��������
//				{
//					m_mgrProcedure->m_nStepMk[0]++;
//				}
//				else											// Review�� �ƴϸ�
//				{
//					if (m_mgrProcedure->m_bReview)
//					{
//						m_mgrProcedure->m_nMkPcs[0] = m_mgrReelmap->GetTotDefPcsIts(nSerial);
//						m_mgrProcedure->m_nStepMk[0] = MK_END;
//					}
//					else
//						m_mgrProcedure->m_nStepMk[0]++;
//				}
//			}
//		}
//		break;
//	case 8:
//		nSerial = m_mgrProcedure->m_nBufUpSerial[0]; // Cam0
//
//		if (m_mgrProcedure->m_nMkPcs[0] < m_mgrReelmap->GetTotDefPcsIts(nSerial))	// ��ŷ�Ϸ�Check
//		{
//			if (m_mgrProcedure->m_nMkPcs[0] + 1 < m_mgrReelmap->GetTotDefPcsIts(nSerial))		// ���� ��ŷ��ġ�� ������
//			{
//				ptPnt = m_mgrReelmap->GetMkPntIts(nSerial, m_mgrProcedure->m_nMkPcs[0] + 1);	// ���� ��ŷ��ġ
//				m_mgrProcedure->m_dNextTarget[AXIS_X0] = ptPnt.x;
//				m_mgrProcedure->m_dNextTarget[AXIS_Y0] = ptPnt.y;
//			}
//			else												// ���� ��ŷ��ġ�� ������
//			{
//				m_mgrProcedure->m_dNextTarget[AXIS_X0] = -1.0;
//				m_mgrProcedure->m_dNextTarget[AXIS_Y0] = -1.0;
//			}
//
//			ptPnt = m_mgrReelmap->GetMkPntIts(nSerial, m_mgrProcedure->m_nMkPcs[0]);			// �̹� ��ŷ��ġ
//			if (ptPnt.x < 0.0 && ptPnt.y < 0.0) // ��ǰȭ. (��ŷ���� ����)
//			{
//				m_mgrProcedure->m_nMkPcs[0]++;
//				m_mgrProcedure->m_nStepMk[0] = MK_DONE_CHECK;
//				break;
//			}
//
//			nIdx = m_mgrReelmap->GetMkStripIdxIts(nSerial, m_mgrProcedure->m_nMkPcs[0]);		// 1 ~ 4 : strip index
//			if (nIdx > 0)										// Strip index�� �����̸�,
//			{
//				if (!IsMkStrip(nIdx)) // Strip[] Mk Off
//				{
//					m_mgrProcedure->m_nMkPcs[0]++;
//					m_mgrProcedure->m_nStepMk[0] = MK_DONE_CHECK;
//					break;
//				}
//				else
//				{
//					if (m_mgrProcedure->m_nMkStrip[0][nIdx - 1] >= nStripOut)
//					{
//						m_mgrProcedure->m_nMkPcs[0]++;
//						m_mgrProcedure->m_nStepMk[0] = MK_DONE_CHECK;
//						break;
//					}
//					else
//						m_mgrProcedure->m_nMkStrip[0][nIdx - 1]++;
//				}
//			}
//			else
//			{
//				pView->ClrDispMsg();
//				AfxMessageBox(_T("Strip Index Failed."));
//				break;
//			}
//
//			m_mgrProcedure->m_dTarget[AXIS_X0] = ptPnt.x;
//			m_mgrProcedure->m_dTarget[AXIS_Y0] = ptPnt.y;
//
//			m_mgrProcedure->m_nStepMk[0]++;
//		}
//		else
//		{
//			m_mgrProcedure->m_nStepMk[0] = MK_END;
//		}
//		break;
//	case 9:
//		// Cam0 : m_bPriority[0] <-, m_bPriority[3] ->
//		// Cam1 : m_bPriority[1] ->, m_bPriority[2] <-
//		ptPnt.x = m_mgrProcedure->m_dTarget[AXIS_X0];
//		ptPnt.y = m_mgrProcedure->m_dTarget[AXIS_Y0];
//
//		if (m_mgrProcedure->m_dNextTarget[AXIS_X0] < 0)
//			m_mgrProcedure->m_bCollision[0] = m_mgrProcedure->ChkCollision(AXIS_X0, m_mgrProcedure->m_dTarget[AXIS_X0]);
//		else
//			m_mgrProcedure->m_bCollision[0] = m_mgrProcedure->ChkCollision(AXIS_X0, m_mgrProcedure->m_dTarget[AXIS_X0], m_mgrProcedure->m_dNextTarget[AXIS_X0]);
//
//		if (!m_mgrProcedure->m_bCollision[0])
//		{
//			if (IsMoveDone0())
//			{
//				Move0(ptPnt, m_mgrProcedure->m_bCam);
//				m_mgrProcedure->m_nStepMk[0]++;
//			}
//		}
//		else if (m_mgrProcedure->m_bPriority[0])
//		{
//			if (IsMoveDone0())
//			{
//				m_mgrProcedure->m_bCollision[0] = FALSE;
//				m_mgrProcedure->m_bPriority[0] = FALSE;
//				Move0(ptPnt, m_mgrProcedure->m_bCam);
//				m_mgrProcedure->m_nStepMk[0]++;
//			}
//		}
//		else if (m_mgrProcedure->m_bPriority[2])
//		{
//			if (IsMoveDone0())
//			{
//				m_mgrProcedure->m_bCollision[0] = FALSE;
//				m_mgrProcedure->m_bPriority[2] = FALSE;
//				ptPnt.x = 0.0;
//				ptPnt.y = 0.0;//m_dEnc[AXIS_Y0];
//				Move0(ptPnt, FALSE);
//			}
//		}
//		break;
//	case 10:
//		m_mgrProcedure->m_nStepMk[0]++;
//		break;
//	case 11:
//		m_mgrProcedure->m_nStepMk[0]++;
//		break;
//	case 12:
//		if (IsMoveDone0())
//			m_mgrProcedure->m_nStepMk[0]++;
//		break;
//	case 13:
//		if (!m_mgrProcedure->IsNoMk0())
//		{
//			m_mgrProcedure->m_dwStMkDn[0] = GetTickCount();
//			Mk0();
//		}
//		else
//		{
//			// Verify - Mk0
//			SetDelay0(pDoc->m_nDelayShow, 1);		// [mSec]
//			if (!SaveMk0Img(m_mgrProcedure->m_nMkPcs[0]))
//			{
//				pView->ClrDispMsg();
//				AfxMessageBox(_T("Error-SaveMk0Img()"));
//			}
//		}
//		m_mgrProcedure->m_nStepMk[0]++;
//		break;
//	case 14:
//		m_mgrProcedure->m_nStepMk[0]++;
//		break;
//	case 15:
//		m_mgrProcedure->m_nStepMk[0]++;
//		break;
//	case 16:
//		if (m_mgrProcedure->IsNoMk0())
//		{
//			if (!WaitDelay0(1))		// F:Done, T:On Waiting....
//				m_mgrProcedure->m_nStepMk[0]++;
//		}
//		else
//			m_mgrProcedure->m_nStepMk[0]++;
//		break;
//	case 17:
//		if (!m_mgrProcedure->IsNoMk0())
//		{
//			if (IsMk0Done())
//			{
//				// One more MK On Start....
//				if (!m_mgrProcedure->m_nMkPcs[0] && !m_mgrProcedure->m_bAnswer[2])
//				{
//					m_mgrProcedure->m_bAnswer[2] = TRUE;
//					Mk0();
//				}
//				else
//				{
//					m_mgrProcedure->m_bAnswer[2] = FALSE;
//					m_mgrProcedure->m_nMkPcs[0]++;
//					m_mgrProcedure->m_nStepMk[0]++;
//					m_mgrProcedure->m_nStepMk[0]++;
//				}
//			}
//			else
//			{
//				if (m_mgrProcedure->m_dwStMkDn[0] + 5000 < GetTickCount())
//				{
//					BuzzerFromThread(TRUE, 0);
//					DispMain(_T("�� ��"), RGB_RED);
//					m_mgrPunch->m_pVoiceCoil[0]->SearchHomeSmac0();
//
//					nRtn = AsyncMsgBox(_T("���̽�����(��) ��ſϷᰡ �ʵ˴ϴ�.\r\n��ŷ�� �ٽ� �õ��Ͻðڽ��ϱ�?"), 1, MB_YESNO);
//					if (IDYES == nRtn)
//					{
//						DispMain(_T("������"), RGB_RED);
//						m_mgrProcedure->m_nStepMk[0] = 13;
//					}
//					else if (nRtn < 0)
//						m_mgrProcedure->m_nStepMk[0]++; // Wait...
//					else
//					{
//						m_mgrProcedure->m_bAnswer[2] = FALSE;
//						m_mgrProcedure->m_nMkPcs[0]++;
//						m_mgrProcedure->m_nStepMk[0]++;
//						m_mgrProcedure->m_nStepMk[0]++;
//						Stop();
//					}
//				}
//			}
//		}
//		else
//		{
//			m_mgrProcedure->m_nMkPcs[0]++;
//			m_mgrProcedure->m_nStepMk[0]++;
//			m_mgrProcedure->m_nStepMk[0]++;
//		}
//		break;
//	case 18:
//		if ((nRtn = WaitRtnVal(1)) > -1)
//		{
//			if (IDYES == nRtn)
//			{
//				DispMain(_T("������"), RGB_RED);
//				m_mgrProcedure->m_nStepMk[0] = 13;
//			}
//			else
//			{
//				m_mgrProcedure->m_bAnswer[2] = FALSE;
//				m_mgrProcedure->m_nMkPcs[0]++;
//				m_mgrProcedure->m_nStepMk[0]++;
//				StopFromThread();
//			}
//		}
//		break;
//	case 19:
//		m_mgrProcedure->m_nStepMk[0] = MK_DONE_CHECK;
//		break;
//	case MK_DONE_CHECK:
//		nSerial = m_mgrProcedure->m_nBufUpSerial[0]; // Cam0
//
//		if (m_mgrProcedure->m_nMkPcs[0] < m_mgrReelmap->GetTotDefPcsIts(nSerial))
//		{
//			if (m_mgrProcedure->IsNoMk0())
//			{
//				if (!pDoc->WorkingInfo.System.bNoMk)
//				{
//					if (m_mgrProcedure->IsReview0())
//					{
//						if (IsJogRtDn0())
//							m_mgrProcedure->m_nStepMk[0]++;
//					}
//					else if (!pDoc->WorkingInfo.LastJob.bVerify)
//					{
//						m_mgrProcedure->m_nMkPcs[0] = m_mgrReelmap->GetTotDefPcsIts(nSerial);
//						m_mgrProcedure->m_nStepMk[0] = MK_END;
//					}
//					else
//						m_mgrProcedure->m_nStepMk[0] = 8;	// ��ŷ�Ϸ�Check
//				}
//				else
//					m_mgrProcedure->m_nStepMk[0] = 8;	// ��ŷ�Ϸ�Check
//			}
//			else
//				m_mgrProcedure->m_nStepMk[0] = 8;	// ��ŷ�Ϸ�Check
//		}
//		else
//		{
//			if (m_mgrProcedure->IsNoMk0())
//			{
//				if (m_mgrProcedure->IsReview0())
//				{
//					if (IsJogRtDn0())
//						m_mgrProcedure->m_nStepMk[0] = MK_END;
//				}
//				else
//				{
//					m_mgrProcedure->m_nStepMk[0] = MK_END;
//				}
//			}
//			else
//				m_mgrProcedure->m_nStepMk[0] = MK_END;
//		}
//		break;
//	case MK_DONE_CHECK + 1:
//		nSerial = m_mgrProcedure->m_nBufUpSerial[0]; // Cam0
//
//		if (m_mgrProcedure->m_nMkPcs[0] < m_mgrReelmap->GetTotDefPcsIts(nSerial))
//		{
//			if (m_mgrProcedure->IsNoMk0())
//			{
//				if (m_mgrProcedure->IsReview0())
//				{
//					if (IsJogRtUp0())
//						m_mgrProcedure->m_nStepMk[0] = 8;	// ��ŷ�Ϸ�Check
//				}
//				else
//				{
//					m_mgrProcedure->m_nMkPcs[0] = m_mgrReelmap->GetTotDefPcsIts(nSerial);
//					m_mgrProcedure->m_nStepMk[0] = MK_END;
//				}
//			}
//			else
//				m_mgrProcedure->m_nStepMk[0] = 8;	// ��ŷ�Ϸ�Check
//		}
//		else
//		{
//			m_mgrProcedure->m_nStepMk[0] = MK_END;
//		}
//		break;
//
//	case MK_END:
//		if (IsMoveDone0())
//			m_mgrProcedure->m_nStepMk[0]++;
//		break;
//	case 101:
//		SetDelay0(100, 1);		// [mSec]
//		m_mgrProcedure->m_nStepMk[0]++;
//		break;
//	case 102:
//		if (!WaitDelay0(1))		// F:Done, T:On Waiting....
//		{
//			m_mgrProcedure->m_nStepMk[0]++;
//		}
//		break;
//	case 103:
//		if (!IsInitPos0())
//		{
//			m_mgrProcedure->m_dTarget[AXIS_X0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[0]);
//			m_mgrProcedure->m_dTarget[AXIS_Y0] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[0]);
//			m_mgrProcedure->m_dNextTarget[AXIS_X0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[0]);
//			m_mgrProcedure->m_dNextTarget[AXIS_Y0] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[0]);
//
//			m_mgrPunch->MoveInitPos0();
//		}
//
//		pDoc->SaveMkCntL();
//		m_mgrProcedure->m_nStepMk[0]++;
//		break;
//	case 104:
//		if (IsMoveDone0())
//		{
//			m_mgrProcedure->m_nStepMk[0]++;
//			SetDelay0(10000, 1);		// [mSec]
//		}
//		break;
//	case 105:
//		if (IsInitPos0())
//		{
//			m_mgrProcedure->m_nStepMk[0]++;
//		}
//		else
//		{
//			if (!WaitDelay0(1))		// F:Done, T:On Waiting....
//			{
//				m_mgrProcedure->m_nStepMk[0] = ERR_PROC;
//			}
//		}
//		break;
//	case 106: // MK Done....
//		m_mgrProcedure->m_bDoneMk[0] = TRUE;
//		m_mgrProcedure->m_bTHREAD_MK[0] = FALSE;
//		break;
//
//	case ERR_PROC:
//		DispMain(_T("�� ��"), RGB_RED);
//		m_mgrPunch->m_pVoiceCoil[0]->SearchHomeSmac0();
//		AsyncMsgBox(_T("���̽�����(��) �ʱ���ġ �̵��� ���� �ʽ��ϴ�.\r\n��ŷ���¸� Ȯ���ϼ���."), 1);
//		m_mgrProcedure->m_nStepMk[0]++;
//		break;
//	case ERR_PROC + 1:
//		nSerial = m_mgrProcedure->m_nBufUpSerial[0];
//
//		if (m_mgrProcedure->m_bCam)
//			sMsg.Format(_T("%d�� Shot�� �ٽ� �ҷ�Ȯ���� �Ͻðڽ��ϱ�?"), nSerial);
//		else
//			sMsg.Format(_T("%d�� Shot�� �ٽ� ��ŷ�Ͻðڽ��ϱ�?"), nSerial);
//
//		m_mgrProcedure->m_nRtnMyMsgBoxIdx = 0;
//		m_mgrProcedure->m_bRtnMyMsgBox[0] = FALSE;
//		m_mgrProcedure->m_nRtnMyMsgBox[0] = -1;
//		pView->AsyncMsgBox(sMsg, 1, MB_YESNO);
//		sMsg.Empty();
//		m_mgrProcedure->m_nStepMk[0]++;
//		break;
//	case ERR_PROC + 2:
//		if ((nRtn = WaitRtnVal()) > -1)
//		{
//			if (IDYES == nRtn)
//			{
//				m_mgrProcedure->m_nStepMk[0] = ERR_PROC + 10;
//			}
//			else
//			{
//				m_mgrProcedure->m_nRtnMyMsgBoxIdx = 0;
//				m_mgrProcedure->m_bRtnMyMsgBox[0] = FALSE;
//				m_mgrProcedure->m_nRtnMyMsgBox[0] = -1;
//				sMsg.Format(_T("��� ���� �۾��� �����Ͻðڽ��ϱ�?"), nSerial);
//				pView->AsyncMsgBox(sMsg, 1, MB_YESNO);
//				sMsg.Empty();
//
//				m_mgrProcedure->m_nStepMk[0]++;
//			}
//		}
//		break;
//	case ERR_PROC + 3:
//		if ((nRtn = WaitRtnVal()) > -1)
//		{
//			if (IDYES == nRtn)
//			{
//				m_mgrProcedure->m_nStepMk[0] = ERR_PROC + 20;
//			}
//			else
//			{
//				m_bDispMsgDoAuto[8] = TRUE;
//				m_nStepDispMsg[8] = FROM_DOMARK0;
//			}
//		}
//		break;
//	case ERR_PROC + 10:
//		m_mgrProcedure->m_bReMark[0] = TRUE;
//		m_mgrProcedure->m_bTHREAD_MK[0] = FALSE;
//		m_mgrProcedure->m_nStepMk[0] = 0;
//		break;
//	case ERR_PROC + 20: // MK Done....
//		m_mgrProcedure->m_bDoneMk[0] = TRUE;
//		m_mgrProcedure->m_bTHREAD_MK[0] = FALSE;
//		break;
//	}
//}
//
//void CGvisR2R_PunchView::DoMark1Its()
//{
//#ifdef TEST_MODE
//	return;
//#endif
//
//	if (!m_mgrPunch->m_bAuto)
//		return;
//
//	//BOOL bOn;
//	int nSerial, nIdx, nErrCode, nRtn;
//	CfPoint ptPnt;
//	CString sMsg;
//	double dStripOut = (m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs / MAX_STRIP_NUM) * _tstof(pDoc->WorkingInfo.LastJob.sStripOutRatio) / 100.0;
//	int nStripOut = int(dStripOut);
//	if (dStripOut > nStripOut)
//		nStripOut++;
//
//	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
//
//
//	if (!IsRun())
//	{
//		if (IsOnMarking1())
//		{
//			if (m_mgrPunch->m_pMotion->IsEnable(MS_X1) && m_mgrPunch->m_pMotion->IsEnable(MS_Y1))
//			{
//				if (m_mgrPunch->m_pMotion->IsMotionDone(MS_X1) && m_mgrPunch->m_pMotion->IsMotionDone(MS_Y1))
//				{
//					if (!IsMkEdPos1() && !IsPinPos1())
//						m_mgrPunch->MoveMkEdPos1();
//				}
//			}
//
//			if (m_mgrProcedure->m_nStepMk[1] < 13 && m_mgrProcedure->m_nStepMk[1] > 8) // Mk1();
//			{
//				m_mgrProcedure->m_nStepMk[1] = 8;	// ��ŷ�Ϸ�Check
//			}
//		}
//		return;
//	}
//
//
//	m_mgrProcedure->m_sDispSts[1].Format(_T("%d"), m_mgrProcedure->m_nStepMk[1]);
//
//	switch (m_mgrProcedure->m_nStepMk[1])
//	{
//	case 0:
//		if (m_mgrProcedure->IsNoMk())
//			ShowLive();
//		m_mgrProcedure->m_nStepMk[1]++;
//		break;
//	case 1:
//		if (!IsInitPos1())
//			m_mgrPunch->MoveInitPos1();
//		m_mgrProcedure->m_nStepMk[1]++;
//		break;
//	case 2:
//
//		nSerial = m_mgrProcedure->m_nBufUpSerial[1]; // Cam1
//
//		if (nSerial > 0)
//		{
//			if ((nErrCode = m_mgrReelmap->GetErrCode1Its(nSerial)) != 1)
//			{
//				m_mgrProcedure->m_nMkPcs[1] = m_mgrReelmap->GetTotDefPcsIts(nSerial);
//				m_mgrProcedure->m_nStepMk[1] = MK_END;
//			}
//			else
//			{
//				m_mgrProcedure->m_nStepMk[1]++;
//			}
//		}
//		else
//		{
//			StopFromThread();
//			AsyncMsgBox(_T("������ �ø����� �����ʽ��ϴ�."), 2);
//			BuzzerFromThread(TRUE, 0);
//			//pView->DispStsBar(_T("����-34"), 0);
//			DispMain(_T("�� ��"), RGB_RED);
//		}
//		break;
//	case 3:
//		m_mgrProcedure->m_nStepMk[1]++;
//		break;
//	case 4:
//		m_mgrProcedure->m_nStepMk[1]++;
//		break;
//	case 5:
//		m_mgrProcedure->m_nStepMk[1]++;
//		break;
//	case 6:
//
//		nSerial = m_mgrProcedure->m_nBufUpSerial[1]; // Cam1
//
//		if (m_mgrProcedure->m_nMkPcs[1] < m_mgrReelmap->GetTotDefPcsIts(nSerial))
//		{
//			if (!m_mgrProcedure->IsNoMk1())
//			{
//				;
//			}
//			else
//			{
//				if (!m_mgrProcedure->IsReview1())
//				{
//					if (m_mgrProcedure->m_bReview)
//					{
//						m_mgrProcedure->m_nMkPcs[1] = m_mgrReelmap->GetTotDefPcsIts(nSerial);
//						m_mgrProcedure->m_nStepMk[1] = MK_END;
//						break;
//					}
//				}
//			}
//			SetDelay1(100, 6);		// [mSec]
//			m_mgrProcedure->m_nStepMk[1]++;
//		}
//		else
//		{
//			m_mgrProcedure->m_nStepMk[1] = MK_END;
//		}
//		break;
//	case 7:
//		nSerial = m_mgrProcedure->m_nBufUpSerial[1]; // Cam1
//
//		if (!WaitDelay1(6))		// F:Done, T:On Waiting....
//		{
//			m_mgrProcedure->m_nMkPcs[1] = 0;
//
//			if (!m_mgrProcedure->IsNoMk1())
//			{
//				m_mgrProcedure->m_nStepMk[1]++;
//			}
//			else
//			{
//				if (m_mgrProcedure->IsReview1())
//				{
//					m_mgrProcedure->m_nStepMk[1]++;
//				}
//				else
//				{
//					if (m_mgrProcedure->m_bReview)
//					{
//						m_mgrProcedure->m_nMkPcs[1] = m_mgrReelmap->GetTotDefPcsIts(nSerial);
//						m_mgrProcedure->m_nStepMk[1] = MK_END;
//					}
//					else
//						m_mgrProcedure->m_nStepMk[1]++;
//				}
//			}
//		}
//		break;
//	case 8:
//		nSerial = m_mgrProcedure->m_nBufUpSerial[1]; // Cam1
//
//		if (m_mgrProcedure->m_nMkPcs[1] < m_mgrReelmap->GetTotDefPcsIts(nSerial))	// ��ŷ�Ϸ�Check
//		{
//			if (m_mgrProcedure->m_nMkPcs[1] + 1 < m_mgrReelmap->GetTotDefPcsIts(nSerial))
//			{
//				ptPnt = m_mgrReelmap->GetMkPntIts(nSerial, m_mgrProcedure->m_nMkPcs[1] + 1);
//				m_mgrProcedure->m_dNextTarget[AXIS_X1] = ptPnt.x;
//				m_mgrProcedure->m_dNextTarget[AXIS_Y1] = ptPnt.y;
//			}
//			else
//			{
//				m_mgrProcedure->m_dNextTarget[AXIS_X1] = -1.0;
//				m_mgrProcedure->m_dNextTarget[AXIS_Y1] = -1.0;
//			}
//
//			ptPnt = m_mgrReelmap->GetMkPntIts(nSerial, m_mgrProcedure->m_nMkPcs[1]);
//			if (ptPnt.x < 0.0 && ptPnt.y < 0.0) // ��ǰȭ.
//			{
//				m_mgrProcedure->m_nMkPcs[1]++;
//				m_mgrProcedure->m_nStepMk[1] = MK_DONE_CHECK;
//				break;
//			}
//
//			nIdx = m_mgrReelmap->GetMkStripIdxIts(nSerial, m_mgrProcedure->m_nMkPcs[1]);
//			if (nIdx > 0)
//			{
//				if (!IsMkStrip(nIdx)) // Strip[] Mk Off
//				{
//					m_mgrProcedure->m_nMkPcs[1]++;
//					m_mgrProcedure->m_nStepMk[1] = MK_DONE_CHECK;
//					break;
//				}
//				else
//				{
//					if (m_mgrProcedure->m_nMkStrip[1][nIdx - 1] >= nStripOut)
//					{
//						m_mgrProcedure->m_nMkPcs[1]++;
//						m_mgrProcedure->m_nStepMk[1] = MK_DONE_CHECK;
//						break;
//					}
//					else
//						m_mgrProcedure->m_nMkStrip[1][nIdx - 1]++;
//				}
//			}
//			else
//			{
//				pView->ClrDispMsg();
//				AfxMessageBox(_T("Strip Index Failed."));
//				break;
//			}
//
//			m_mgrProcedure->m_dTarget[AXIS_X1] = ptPnt.x;
//			m_mgrProcedure->m_dTarget[AXIS_Y1] = ptPnt.y;
//
//			m_mgrProcedure->m_nStepMk[1]++;
//		}
//		else
//		{
//			m_mgrProcedure->m_nStepMk[1] = MK_END;
//		}
//		break;
//	case 9:
//		// Cam0 : m_bPriority[0] <-, m_bPriority[3] ->
//		// Cam1 : m_bPriority[1] ->, m_bPriority[2] <-
//		ptPnt.x = m_mgrProcedure->m_dTarget[AXIS_X1];
//		ptPnt.y = m_mgrProcedure->m_dTarget[AXIS_Y1];
//
//		if (m_mgrProcedure->m_dNextTarget[AXIS_X1] < 0)
//			m_mgrProcedure->m_bCollision[1] = m_mgrProcedure->ChkCollision(AXIS_X1, m_mgrProcedure->m_dTarget[AXIS_X1]);
//		else
//			m_mgrProcedure->m_bCollision[1] = m_mgrProcedure->ChkCollision(AXIS_X1, m_mgrProcedure->m_dTarget[AXIS_X1], m_mgrProcedure->m_dNextTarget[AXIS_X1]);
//
//		if (!m_mgrProcedure->m_bCollision[1])
//		{
//			if (IsMoveDone1())
//			{
//				Move1(ptPnt, m_mgrProcedure->m_bCam);
//				m_mgrProcedure->m_nStepMk[1]++;
//			}
//		}
//		else if (m_mgrProcedure->m_bPriority[1])
//		{
//			if (IsMoveDone1())
//			{
//				m_mgrProcedure->m_bCollision[1] = FALSE;
//				m_mgrProcedure->m_bPriority[1] = FALSE;
//				Move1(ptPnt, m_mgrProcedure->m_bCam);
//				m_mgrProcedure->m_nStepMk[1]++;
//			}
//		}
//		else if (m_mgrProcedure->m_bPriority[3])
//		{
//			if (IsMoveDone1())
//			{
//				m_mgrProcedure->m_bCollision[1] = FALSE;
//				m_mgrProcedure->m_bPriority[3] = FALSE;
//				ptPnt.x = _tstof(pDoc->WorkingInfo.Motion.sSafeZone);
//				ptPnt.y = 0.0;//m_dEnc[AXIS_Y1];
//				Move1(ptPnt, FALSE);
//			}
//		}
//		break;
//	case 10:
//		m_mgrProcedure->m_nStepMk[1]++;
//		break;
//	case 11:
//		m_mgrProcedure->m_nStepMk[1]++;
//		break;
//	case 12:
//		if (IsMoveDone1())
//			m_mgrProcedure->m_nStepMk[1]++;
//		break;
//	case 13:
//		if (!m_mgrProcedure->IsNoMk1())
//		{
//			m_mgrProcedure->m_dwStMkDn[1] = GetTickCount();
//			Mk1();
//		}
//		else
//		{
//			// Verify - Mk1
//			SetDelay1(pDoc->m_nDelayShow, 6);		// [mSec]
//			if (!SaveMk1Img(m_mgrProcedure->m_nMkPcs[1]))
//			{
//				pView->ClrDispMsg();
//				AfxMessageBox(_T("Error-SaveMk1Img()"));
//			}
//		}
//		m_mgrProcedure->m_nStepMk[1]++;
//		break;
//	case 14:
//		m_mgrProcedure->m_nStepMk[1]++;
//		break;
//	case 15:
//		m_mgrProcedure->m_nStepMk[1]++;
//		break;
//	case 16:
//		if (m_mgrProcedure->IsNoMk1())
//		{
//			if (!WaitDelay1(6))		// F:Done, T:On Waiting....
//				m_mgrProcedure->m_nStepMk[1]++;
//		}
//		else
//			m_mgrProcedure->m_nStepMk[1]++;
//		break;
//	case 17:
//		if (!m_mgrProcedure->IsNoMk1())
//		{
//			if (IsMk1Done())
//			{
//				// One more MK On Start....
//				if (!m_mgrProcedure->m_nMkPcs[1] && !m_mgrProcedure->m_bAnswer[3])
//				{
//					m_mgrProcedure->m_bAnswer[3] = TRUE;
//					Mk1();
//				}
//				else
//				{
//					m_mgrProcedure->m_bAnswer[3] = FALSE;
//					m_mgrProcedure->m_nMkPcs[1]++;
//					m_mgrProcedure->m_nStepMk[1]++;
//					m_mgrProcedure->m_nStepMk[1]++;
//				}
//
//			}
//			else
//			{
//				if (m_mgrProcedure->m_dwStMkDn[1] + 5000 < GetTickCount())
//				{
//					BuzzerFromThread(TRUE, 0);
//					DispMain(_T("�� ��"), RGB_RED);
//					m_mgrPunch->m_pVoiceCoil[1]->SearchHomeSmac1();
//
//					nRtn = AsyncMsgBox(_T("���̽�����(��) ��ſϷᰡ �ʵ˴ϴ�.\r\n��ŷ�� �ٽ� �õ��Ͻðڽ��ϱ�?"), 2, MB_YESNO);
//					if (IDYES == nRtn)
//					{
//						DispMain(_T("������"), RGB_RED);
//						m_mgrProcedure->m_nStepMk[1] = 13;
//					}
//					else if (nRtn < 0)
//						m_mgrProcedure->m_nStepMk[1]++; // Wait...
//					else
//					{
//						m_mgrProcedure->m_bAnswer[3] = FALSE;
//						m_mgrProcedure->m_nMkPcs[1]++;
//						m_mgrProcedure->m_nStepMk[1]++;
//						m_mgrProcedure->m_nStepMk[1]++;
//						Stop();
//					}
//				}
//			}
//		}
//		else
//		{
//			m_mgrProcedure->m_nMkPcs[1]++;
//			m_mgrProcedure->m_nStepMk[1]++;
//			m_mgrProcedure->m_nStepMk[1]++;
//		}
//		break;
//	case 18:
//		if ((nRtn = WaitRtnVal(2)) > -1)
//		{
//			if (IDYES == nRtn)
//			{
//				DispMain(_T("������"), RGB_RED);
//				m_mgrProcedure->m_nStepMk[1] = 13;
//			}
//			else
//			{
//				m_mgrProcedure->m_bAnswer[3] = FALSE;
//				m_mgrProcedure->m_nMkPcs[1]++;
//				m_mgrProcedure->m_nStepMk[1]++;
//				StopFromThread();
//			}
//		}
//		break;
//	case 19:
//		m_mgrProcedure->m_nStepMk[1] = MK_DONE_CHECK;
//		break;
//	case MK_DONE_CHECK:
//		nSerial = m_mgrProcedure->m_nBufUpSerial[1]; // Cam1
//
//		if (m_mgrProcedure->m_nMkPcs[1] < m_mgrReelmap->GetTotDefPcsIts(nSerial))
//		{
//			if (m_mgrProcedure->IsNoMk1())
//			{
//				if (!pDoc->WorkingInfo.System.bNoMk)
//				{
//					if (m_mgrProcedure->IsReview1())
//					{
//						if (IsJogRtDn1())
//							m_mgrProcedure->m_nStepMk[1]++;
//					}
//					else if (!pDoc->WorkingInfo.LastJob.bVerify)
//					{
//						m_mgrProcedure->m_nMkPcs[1] = m_mgrReelmap->GetTotDefPcsIts(nSerial);
//						m_mgrProcedure->m_nStepMk[1] = MK_END;
//					}
//					else
//						m_mgrProcedure->m_nStepMk[1] = 8;	// ��ŷ�Ϸ�Check
//				}
//				else
//					m_mgrProcedure->m_nStepMk[1] = 8;	// ��ŷ�Ϸ�Check
//			}
//			else
//				m_mgrProcedure->m_nStepMk[1] = 8;	// ��ŷ�Ϸ�Check
//		}
//		else
//		{
//			if (m_mgrProcedure->IsNoMk1())
//			{
//				if (m_mgrProcedure->IsReview1())
//				{
//					if (IsJogRtDn1())
//						m_mgrProcedure->m_nStepMk[1] = MK_END;
//				}
//				else
//				{
//					m_mgrProcedure->m_nStepMk[1] = MK_END;
//				}
//			}
//			else
//				m_mgrProcedure->m_nStepMk[1] = MK_END;
//		}
//		break;
//	case MK_DONE_CHECK + 1:
//		nSerial = m_mgrProcedure->m_nBufUpSerial[1]; // Cam1
//
//		if (m_mgrProcedure->m_nMkPcs[1] < m_mgrReelmap->GetTotDefPcsIts(nSerial))
//		{
//			if (m_mgrProcedure->IsNoMk1())
//			{
//				if (m_mgrProcedure->IsReview1())
//				{
//					if (IsJogRtUp1())
//						m_mgrProcedure->m_nStepMk[1] = 8;	// ��ŷ�Ϸ�Check
//				}
//				else
//				{
//					m_mgrProcedure->m_nMkPcs[1] = m_mgrReelmap->GetTotDefPcsIts(nSerial);
//					m_mgrProcedure->m_nStepMk[1] = MK_END;
//				}
//			}
//			else
//				m_mgrProcedure->m_nStepMk[1] = 8;	// ��ŷ�Ϸ�Check
//		}
//		else
//		{
//			m_mgrProcedure->m_nStepMk[1] = MK_END;
//		}
//		break;
//
//
//	case MK_END:
//		if (IsMoveDone1())
//			m_mgrProcedure->m_nStepMk[1]++;
//		break;
//	case 101:
//		SetDelay1(100, 6);		// [mSec]
//		m_mgrProcedure->m_nStepMk[1]++;
//		break;
//	case 102:
//		if (!WaitDelay1(6))		// F:Done, T:On Waiting....
//			m_mgrProcedure->m_nStepMk[1]++;
//		break;
//	case 103:
//		if (!IsMkEdPos1())
//		{
//			m_mgrProcedure->m_dTarget[AXIS_X1] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[1]);
//			m_mgrProcedure->m_dTarget[AXIS_Y1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[1]);
//			m_mgrProcedure->m_dNextTarget[AXIS_X1] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[1]);
//			m_mgrProcedure->m_dNextTarget[AXIS_Y1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[1]);
//
//			m_mgrPunch->MoveMkEdPos1();
//		}
//
//		pDoc->SaveMkCntR();
//		m_mgrProcedure->m_nStepMk[1]++;
//		break;
//	case 104:
//		if (IsMoveDone1())
//		{
//			m_mgrProcedure->m_nStepMk[1]++;
//			SetDelay1(10000, 6);		// [mSec]
//		}
//		break;
//	case 105:
//		if (IsMkEdPos1())
//		{
//			m_mgrProcedure->m_nStepMk[1]++;
//		}
//		else
//		{
//			if (!WaitDelay1(6))		// F:Done, T:On Waiting....
//			{
//				m_mgrProcedure->m_nStepMk[1] = ERR_PROC;
//			}
//		}
//		break;
//	case 106: // MK Done....
//		m_mgrProcedure->m_bDoneMk[1] = TRUE;
//		m_mgrProcedure->m_bTHREAD_MK[1] = FALSE;
//		break;
//
//	case ERR_PROC:
//		DispMain(_T("�� ��"), RGB_RED);
//		m_mgrPunch->m_pVoiceCoil[1]->SearchHomeSmac1();
//		AsyncMsgBox(_T("���̽�����(��) �ʱ���ġ �̵��� ���� �ʽ��ϴ�.\r\n��ŷ���¸� Ȯ���ϼ���."), 2);
//		m_mgrProcedure->m_nStepMk[1]++;
//		break;
//	case ERR_PROC + 1:
//		nSerial = m_mgrProcedure->m_nBufUpSerial[1]; // Cam1
//
//		if (m_mgrProcedure->m_bCam)
//			sMsg.Format(_T("%d�� Shot�� �ٽ� �ҷ�Ȯ���� �Ͻðڽ��ϱ�?"), nSerial);
//		else
//			sMsg.Format(_T("%d�� Shot�� �ٽ� ��ŷ�Ͻðڽ��ϱ�?"), nSerial);
//
//		m_mgrProcedure->m_nRtnMyMsgBoxIdx = 1;
//		m_mgrProcedure->m_bRtnMyMsgBox[1] = FALSE;
//		m_mgrProcedure->m_nRtnMyMsgBox[1] = -1;
//		pView->AsyncMsgBox(sMsg, 2, MB_YESNO);
//		sMsg.Empty();
//		m_mgrProcedure->m_nStepMk[1]++;
//		break;
//	case ERR_PROC + 2:
//		if ((nRtn = WaitRtnVal()) > -1)
//		{
//			if (IDYES == nRtn)
//			{
//				m_mgrProcedure->m_nStepMk[1] = ERR_PROC + 10;
//			}
//			else
//			{
//				m_mgrProcedure->m_nRtnMyMsgBoxIdx = 1;
//				m_mgrProcedure->m_bRtnMyMsgBox[1] = FALSE;
//				m_mgrProcedure->m_nRtnMyMsgBox[1] = -1;
//				sMsg.Format(_T("��� ���� �۾��� �����Ͻðڽ��ϱ�?"), nSerial);
//				pView->AsyncMsgBox(sMsg, 2, MB_YESNO);
//				sMsg.Empty();
//
//				m_mgrProcedure->m_nStepMk[1]++;
//
//			}
//		}
//		break;
//	case ERR_PROC + 3:
//		if ((nRtn = WaitRtnVal()) > -1)
//		{
//			if (IDYES == nRtn)
//			{
//				m_mgrProcedure->m_nStepMk[1] = ERR_PROC + 20;
//			}
//			else
//			{
//				m_bDispMsgDoAuto[9] = TRUE;
//				m_nStepDispMsg[9] = FROM_DOMARK1;
//			}
//		}
//		break;
//	case ERR_PROC + 10:
//		m_mgrProcedure->m_bReMark[1] = TRUE;
//		m_mgrProcedure->m_bTHREAD_MK[1] = FALSE;
//		m_mgrProcedure->m_nStepMk[1] = 0;
//		break;
//	case ERR_PROC + 20: // MK Done....
//		m_mgrProcedure->m_bDoneMk[1] = TRUE;
//		m_mgrProcedure->m_bTHREAD_MK[1] = FALSE;
//		break;
//	}
//}

void CGvisR2R_PunchView::UpdateRstInner()
{
	if (m_pDlgMenu06)
		m_pDlgMenu06->UpdateRst();
}

BOOL CGvisR2R_PunchView::IsDoneDispMkInfoInner()
{
	BOOL bRtn = FALSE;
	if (m_pDlgMenu06)
		bRtn = m_pDlgMenu06->IsDoneDispMkInfo();
	return bRtn;
}

BOOL CGvisR2R_PunchView::SetSerialReelmapInner(int nSerial, BOOL bDumy)
{
	if (!m_pDlgMenu06)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Error - SetSerialReelmap : m_pDlgMenu06 is NULL."));
		return FALSE;
	}

	return m_pDlgMenu06->SetSerialReelmap(nSerial, bDumy);
}

BOOL CGvisR2R_PunchView::SetSerialMkInfoInner(int nSerial, BOOL bDumy)
{
	if (!m_pDlgMenu06)
		return FALSE;
	return m_pDlgMenu06->SetSerialMkInfo(nSerial, bDumy);
}

CString CGvisR2R_PunchView::GetTimeIts()
{
	stLotTime ItsTime;

	CString strVal;
	time_t osBinTime;			// C run-time time (defined in <time.h>)
	time(&osBinTime);		// Get the current time from the 
							// operating system.
	CTime Tim(osBinTime);

	ItsTime.nYear = Tim.GetYear();
	ItsTime.nMonth = Tim.GetMonth();
	ItsTime.nDay = Tim.GetDay();
	ItsTime.nHour = Tim.GetHour();
	ItsTime.nMin = Tim.GetMinute();
	ItsTime.nSec = Tim.GetSecond();

	strVal.Format(_T("%04d%02d%02d%02d%02d%02d"),
		ItsTime.nYear, ItsTime.nMonth, ItsTime.nDay,
		ItsTime.nHour, ItsTime.nMin, ItsTime.nSec);

	return strVal;
}

void CGvisR2R_PunchView::MakeResultMDS()
{
	CString sPath, strMsg;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	pDoc->WorkingInfo.LastJob.sProcessNum = pDoc->GetProcessNum();
	pDoc->UpdateProcessNum(pDoc->WorkingInfo.LastJob.sProcessNum);


	// for SAPP3
	GetResult();		// Load data From Reelmap.txt
	MakeResult();		// Result.txt
	MakeResultIts();	// Result.txt
	MakeSapp3();		// GetSapp3Txt()

	m_mgrReelmap->RemakeReelmap();

	if (pDoc->GetTestMode() == MODE_OUTER)
		m_mgrReelmap->RemakeReelmapInner();
}

void CGvisR2R_PunchView::GetResult()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (bDualTest)
	{
		if (m_mgrReelmap->m_pReelMapAllUp)
			m_mgrReelmap->m_pReelMapAllUp->GetResult();
	}
	else
	{
		if (m_mgrReelmap->m_pReelMapUp)
			m_mgrReelmap->m_pReelMapUp->GetResult();
	}

	if (pDoc->GetTestMode() == MODE_OUTER)
	{
		if (m_mgrReelmap->m_pReelMapIts)
			m_mgrReelmap->m_pReelMapIts->GetResult();
	}
}

void CGvisR2R_PunchView::MakeResult()
{	
	// TODO: Add your control notification handler code here
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	// File Save......

	CString strDestPath;
	strDestPath.Format(_T("%s%s\\%s\\%s.txt"), pDoc->WorkingInfo.System.sPathOldFile,
		pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLotUp,
		_T("Result"));

	CFileFind cFile;
	if (cFile.FindFile(strDestPath))
		DeleteFile(strDestPath);

	//////////////////////////////////////////////////////////
	// Directory location of Work file
	CString strData;
	//strData = m_pDlgMenu05->TxtDataMDS();
	if (bDualTest)
	{
		if (m_mgrReelmap->m_pReelMapAllUp)
			strData = m_mgrReelmap->m_pReelMapAllUp->GetResultTxt();
	}
	else
	{
		if (m_mgrReelmap->m_pReelMapUp)
			strData = m_mgrReelmap->m_pReelMapUp->GetResultTxt();
	}


	TCHAR lpszCurDirPathFile[MAX_PATH];
	_stprintf(lpszCurDirPathFile, _T("%s"), strDestPath);

	CFile file;
	CFileException pError;
	if (!file.Open(lpszCurDirPathFile, CFile::modeWrite, &pError))
	{
		if (!file.Open(lpszCurDirPathFile, CFile::modeCreate | CFile::modeWrite, &pError))
		{
			// ���� ���¿� ���н� 
#ifdef _DEBUG
			afxDump << _T("File could not be opened ") << pError.m_cause << _T("\n");
#endif
			return;
		}
	}
	//������ ������ file�� �����Ѵ�.
	char* pRtn = NULL;
	file.SeekToBegin();
	file.Write(pRtn = StringToChar(strData), strData.GetLength());
	file.Close();
	if (pRtn)
		delete pRtn;
}

void CGvisR2R_PunchView::MakeResultIts()
{
	// TODO: Add your control notification handler code here
	if (pDoc->GetTestMode() != MODE_OUTER)
		return;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;

	// File Save......

	CString strDestPath;
	strDestPath.Format(_T("%s%s\\%s\\%s.txt"), pDoc->WorkingInfo.System.sPathItsFile,
		pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sEngItsCode,
		_T("Result"));

	CFileFind cFile;
	if (cFile.FindFile(strDestPath))
		DeleteFile(strDestPath);

	//////////////////////////////////////////////////////////
	// Directory location of Work file
	CString strData;
	if (m_mgrReelmap->m_pReelMapIts)
		strData = m_mgrReelmap->m_pReelMapIts->GetResultTxt();


	TCHAR lpszCurDirPathFile[MAX_PATH];
	_stprintf(lpszCurDirPathFile, _T("%s"), strDestPath);

	CFile file;
	CFileException pError;
	if (!file.Open(lpszCurDirPathFile, CFile::modeWrite, &pError))
	{
		if (!file.Open(lpszCurDirPathFile, CFile::modeCreate | CFile::modeWrite, &pError))
		{
			// ���� ���¿� ���н� 
#ifdef _DEBUG
			afxDump << _T("File could not be opened ") << pError.m_cause << _T("\n");
#endif
			return;
		}
	}
	//������ ������ file�� �����Ѵ�.
	char* pRtn = NULL;
	file.SeekToBegin();
	file.Write(pRtn = StringToChar(strData), strData.GetLength());
	file.Close();
	if (pRtn)
		delete pRtn;
}

void CGvisR2R_PunchView::MakeSapp3()
{
	if (pDoc->WorkingInfo.LastJob.sProcessNum.IsEmpty() || pDoc->WorkingInfo.System.sPathSapp3.IsEmpty())
		return;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	FILE *fp = NULL;
	TCHAR FileName[MAX_PATH];
	CString sPath, sVal, strMsg, sSapp3Data;

	if (bDualTest)
	{
		if (m_mgrReelmap->m_pReelMapAllUp)
			sSapp3Data = m_mgrReelmap->m_pReelMapAllUp->GetSapp3Txt();
	}
	else
	{
		if (m_mgrReelmap->m_pReelMapUp)
			sSapp3Data = m_mgrReelmap->m_pReelMapUp->GetSapp3Txt();
	}

	sPath.Format(_T("%s%9s_%4s_%5s.txt"), pDoc->WorkingInfo.System.sPathSapp3,
		pDoc->WorkingInfo.LastJob.sLotUp,
		pDoc->WorkingInfo.LastJob.sProcessNum,
		pDoc->WorkingInfo.System.sMcName);

	_stprintf(FileName, _T("%s"), sPath);
	char* pRtn = NULL;
	fp = fopen(pRtn = TCHARToChar(FileName), "w+");
	if (pRtn) delete pRtn;
	pRtn = NULL;

	if (fp != NULL)
	{
		fprintf(fp, "%s\n", pRtn = StringToChar(sSapp3Data));
	}
	else
	{
		strMsg.Format(_T("It is trouble to open file.\r\n%s"), sPath);
		pView->ClrDispMsg();
		AfxMessageBox(strMsg, MB_ICONWARNING | MB_OK);
	}

	if (pRtn)
		delete pRtn;
	fclose(fp);
}

BOOL CGvisR2R_PunchView::UpdateReelmap(int nSerial)
{
	return m_mgrReelmap->UpdateReelmap(nSerial);
}

BOOL CGvisR2R_PunchView::UpdateReelmapInner(int nSerial)
{
	return m_mgrReelmap->UpdateReelmapInner(nSerial);
}

void CGvisR2R_PunchView::Shift2Mk()
{
	int nSerialL = m_mgrProcedure->m_nBufUpSerial[0];
	int nSerialR = m_mgrProcedure->m_nBufUpSerial[1];
	int nLastShot = 0;

	if (m_mgrProcedure->m_bLastProc && nSerialL == m_mgrProcedure->m_nLotEndSerial)
	{
		if (nSerialL > 0)
			pDoc->Shift2Mk(nSerialL);	// Cam0

		if (m_pDlgFrameHigh)
			m_pDlgFrameHigh->SetMkLastShot(nSerialL);
	}
	else
	{
		if (nSerialL > 0)
		{
			pDoc->Shift2Mk(nSerialL);	// Cam0
			nLastShot = nSerialL;

			if (nSerialR > 0)
			{
				pDoc->Shift2Mk(nSerialR);	// Cam1
				nLastShot = nSerialR;
			}

			if (m_pDlgFrameHigh)
				m_pDlgFrameHigh->SetMkLastShot(nLastShot);
		}
	}

	m_mgrProcedure->m_bShift2Mk = FALSE;
}

//void CGvisR2R_PunchView::ChkReTestAlarmOnAoiUp()
//{
//	int nSerial = m_mgrProcedure->m_pBufSerial[0][m_mgrProcedure->m_nBufTot[0] - 1];
//
//	if (m_mgrProcedure->m_bSerialDecrese)
//	{
//		if (m_mgrProcedure->m_nLotEndSerial > 0 && nSerial > m_mgrProcedure->m_nLotEndSerial)
//		{
//			//if (pDoc->m_sAlmMsg == GetAoiUpAlarmRestartMsg())
//			{
//				SetAoiUpAutoStep(2); // Wait for AOI �˻���� ��ȣ.
//				Sleep(300);
//				if (m_pMpe)
//					MpeWrite(_T("MB44013B"), 1); // �˻�� ��� ���۾� (���۽�ȣ) : PC�� On��Ű�� PLC�� Off
//			}
//		}
//		else if(m_mgrProcedure->m_nLotEndSerial > 0 && nSerial <= m_mgrProcedure->m_nLotEndSerial)
//		{
//			if (m_pMpe)
//				MpeWrite(_T("MB44012B"), 1); // AOI �� : PCR���� Received
//		}
//	}
//	else
//	{
//		if (m_mgrProcedure->m_nLotEndSerial > 0 && nSerial < m_mgrProcedure->m_nLotEndSerial)
//		{
//			//if (pDoc->m_sAlmMsg == GetAoiUpAlarmRestartMsg())
//			{
//				SetAoiUpAutoStep(2); // Wait for AOI �˻���� ��ȣ.
//				Sleep(300);
//				if (m_pMpe)
//					MpeWrite(_T("MB44013B"), 1); // �˻�� ��� ���۾� (���۽�ȣ) : PC�� On��Ű�� PLC�� Off
//			}
//		}
//		else if (m_mgrProcedure->m_nLotEndSerial > 0 && nSerial >= m_mgrProcedure->m_nLotEndSerial)
//		{
//			if (m_pMpe)
//				MpeWrite(_T("MB44012B"), 1); // AOI �� : PCR���� Received
//		}
//	}
//
//}

//void CGvisR2R_PunchView::ChkReTestAlarmOnAoiDn()
//{
//	int nSerial = m_mgrProcedure->m_pBufSerial[1][m_mgrProcedure->m_nBufTot[1] - 1];
//
//	if (m_mgrProcedure->m_bSerialDecrese)
//	{
//		if (m_mgrProcedure->m_nLotEndSerial > 0 && nSerial > m_mgrProcedure->m_nLotEndSerial)
//		{
//			//if (pDoc->m_sAlmMsg == GetAoiDnAlarmRestartMsg())
//			{
//				SetAoiDnAutoStep(2); // Wait for AOI �˻���� ��ȣ.
//				Sleep(300);
//				if (m_pMpe)
//					MpeWrite(_T("MB44013C"), 1); // �˻�� �Ϻ� ���۾� (���۽�ȣ) : PC�� On��Ű�� PLC�� Off
//			}
//		}
//		else if(m_mgrProcedure->m_nLotEndSerial > 0 && nSerial <= m_mgrProcedure->m_nLotEndSerial)
//		{
//			//if (m_pMpe)
//			//	MpeWrite(_T("MB44012C"), 1); // AOI �� : PCR���� Received
//		}
//	}
//	else
//	{
//		if (m_mgrProcedure->m_nLotEndSerial > 0 && nSerial < m_mgrProcedure->m_nLotEndSerial)
//		{
//			//if (pDoc->m_sAlmMsg == GetAoiDnAlarmRestartMsg())
//			{
//				SetAoiDnAutoStep(2); // Wait for AOI �˻���� ��ȣ.
//				Sleep(300);
//				if (m_pMpe)
//					MpeWrite(_T("MB44013C"), 1); // �˻�� �Ϻ� ���۾� (���۽�ȣ) : PC�� On��Ű�� PLC�� Off
//			}
//		}
//		else if (m_mgrProcedure->m_nLotEndSerial > 0 && nSerial >= m_mgrProcedure->m_nLotEndSerial)
//		{
//			//if (m_pMpe)
//			//	MpeWrite(_T("MB44012C"), 1); // AOI �� : PCR���� Received
//		}
//	}
//
//}

void CGvisR2R_PunchView::ShowMsgBox()
{
	if (m_pDlgMyMsg)
	{
		m_pDlgMyMsg->ShowWindow(SW_SHOW);
		m_pDlgMyMsg->SetForegroundWindow();
		m_pDlgMyMsg->RedrawWindow();
	}
}

BOOL CGvisR2R_PunchView::FinalCopyItsFiles()
{
	if (m_mgrReelmap->m_pReelMapUp)
		m_mgrReelmap->m_pReelMapUp->StartThreadFinalCopyItsFiles();

	return TRUE;
}

int CGvisR2R_PunchView::GetAoiUpCamMstInfo()
{
	return m_mgrReelmap->GetAoiUpCamMstInfo();
}

int CGvisR2R_PunchView::GetAoiDnCamMstInfo()
{
	return m_mgrReelmap->GetAoiDnCamMstInfo();
}

int CGvisR2R_PunchView::GetMkStAuto()
{
	int nStep = 0;
	TCHAR szData[512];

	CString strFolder, strTemp;
	strFolder.Format(_T("%s\\AutoStep.ini"), pDoc->WorkingInfo.System.sPathMkWork);

	if (0 < ::GetPrivateProfileString(_T("Auto"), _T("nStep"), NULL, szData, sizeof(szData), strFolder))
		nStep = _ttoi(szData);
	else
		nStep = 0;

	m_mgrProcedure->m_nMkStAuto = nStep;

	return nStep;
}

void CGvisR2R_PunchView::SetMkStAuto()
{
	CString strFolder, strTemp;
	strFolder.Format(_T("%s\\AutoStep.ini"), pDoc->WorkingInfo.System.sPathMkWork);
	strTemp.Format(_T("%d"), m_mgrProcedure->m_nMkStAuto);
	::WritePrivateProfileString(_T("Auto"), _T("nStep"), strTemp, strFolder);
}

BOOL CGvisR2R_PunchView::GetMkStSignal()
{
	BOOL bMkSt = FALSE;
	TCHAR szData[512];

	CString strFolder, strTemp;
	strFolder.Format(_T("%s\\AutoStep.ini"), pDoc->WorkingInfo.System.sPathMkWork);

	if (0 < ::GetPrivateProfileString(_T("Auto"), _T("bMkStart"), NULL, szData, sizeof(szData), strFolder))
		bMkSt = _ttoi(szData) ? TRUE : FALSE;
	else
		bMkSt = FALSE;

	m_mgrProcedure->m_bMkSt = bMkSt;

	return bMkSt;
}

void CGvisR2R_PunchView::LoadSerial()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (bDualTest)
	{
		if (pDoc->m_ListBuf[1].nTot > 0) // AOI-Dn
		{
			m_mgrProcedure->m_nBufDnSerial[0] = pDoc->m_ListBuf[1].Pop();
			m_mgrProcedure->m_nBufUpSerial[0] = m_mgrProcedure->m_nBufDnSerial[0];
			if (pDoc->m_ListBuf[1].nTot > 0) // AOI-Dn
			{
				m_mgrProcedure->m_nBufDnSerial[1] = pDoc->m_ListBuf[1].Pop();
				m_mgrProcedure->m_nBufUpSerial[1] = m_mgrProcedure->m_nBufDnSerial[1];
			}
			else
			{
				m_mgrProcedure->m_nBufDnSerial[1] = 0;
				m_mgrProcedure->m_nBufUpSerial[1] = 0;
			}
		}
		else
		{
			m_mgrProcedure->m_bLotEnd = TRUE;
			m_mgrProcedure->m_nLotEndAuto = LOT_END;
		}

		if (pDoc->WorkingInfo.LastJob.bSampleTest)
		{
			if (m_mgrProcedure->m_nBufUpSerial[0] == 1)
			{
				m_mgrProcedure->m_nLotEndSerial = _tstoi(pDoc->WorkingInfo.LastJob.sSampleTestShotNum);
				m_mgrProcedure->m_bLastProcFromUp = FALSE;
				m_mgrProcedure->m_bLastProcFromEng = FALSE;
				m_mgrProcedure->m_bLastProc = TRUE;
				if (m_pDlgMenu01)
					m_pDlgMenu01->m_bLastProc = TRUE;
				//if (m_pMpe)
				{
					MpeWrite(_T("MB440186"), 1);			// �ܷ�ó�� AOI(��) ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141112
					MpeWrite(_T("MB440181"), 1);			// �ܷ�ó��(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141031
				}
			}
		}
	}
	else
	{
		if (pDoc->m_ListBuf[0].nTot > 0) // AOI-Up
		{
			m_mgrProcedure->m_nBufUpSerial[0] = pDoc->m_ListBuf[0].Pop();
			if (pDoc->m_ListBuf[0].nTot > 0) // AOI-Up
				m_mgrProcedure->m_nBufUpSerial[1] = pDoc->m_ListBuf[0].Pop();
			else
				m_mgrProcedure->m_nBufUpSerial[1] = 0;
		}
		else
		{
			m_mgrProcedure->m_bLotEnd = TRUE;
			m_mgrProcedure->m_nLotEndAuto = LOT_END;
		}

		if (pDoc->WorkingInfo.LastJob.bSampleTest)
		{
			if (m_mgrProcedure->m_nBufUpSerial[0] == 1)
			{
				m_mgrProcedure->m_nLotEndSerial = _tstoi(pDoc->WorkingInfo.LastJob.sSampleTestShotNum);
				m_mgrProcedure->m_bLastProcFromUp = FALSE;
				m_mgrProcedure->m_bLastProcFromEng = FALSE;
				m_mgrProcedure->m_bLastProc = TRUE;
				if (m_pDlgMenu01)
					m_pDlgMenu01->m_bLastProc = TRUE;
				//if (m_pMpe)
				{
					MpeWrite(_T("MB440186"), 1);			// �ܷ�ó�� AOI(��) ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141112
					MpeWrite(_T("MB440181"), 1);			// �ܷ�ó��(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141031
				}
			}
		}
	}
}

BOOL CGvisR2R_PunchView::DoReset()
{
	pView->DispThreadTick();

	m_mgrPunch->m_bSwRun = FALSE;
	m_bSwStop = FALSE;
	m_bSwReady = FALSE;
	m_bSwReset = TRUE;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pDoc->Status.bManual)
	{
		BOOL bInit = TRUE;

		if (m_mgrProcedure->m_bTIM_CHK_DONE_READY)
		{
			m_mgrProcedure->m_bTIM_CHK_DONE_READY = FALSE;
			m_mgrProcedure->m_bReadyDone = FALSE;
#ifdef USE_MPE
			//if (pView->m_pMpe)
				MpeWrite(_T("MB440100"), 0);	// PLC �����غ� �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)
#endif
		}
		pView->ClrDispMsg();

		if (pView->m_mgrPunch->m_pEngrave)
		{
			pDoc->BtnStatus.EngAuto.Init = TRUE;
			pDoc->BtnStatus.EngAuto.IsInit = FALSE;
			pView->m_mgrPunch->m_pEngrave->SwEngAutoInit(TRUE);
		}

		if (IDNO == pView->MsgBox(_T("�ʱ�ȭ�� �Ͻðڽ��ϱ�?"), 0, MB_YESNO, DEFAULT_TIME_OUT, FALSE))
			bInit = FALSE;
		else
		{
			pDoc->m_bDoneChgLot = FALSE;
			m_mgrProcedure->m_nNewLot = 0;

			if (m_pDlgMenu01)
				m_pDlgMenu01->ClrInfo();
		}

		if (!bInit)
		{
			if (IDNO == pView->MsgBox(_T("�̾�⸦ �Ͻðڽ��ϱ�?"), 0, MB_YESNO, DEFAULT_TIME_OUT, FALSE))
			{
				m_mgrProcedure->m_bCont = FALSE;
				return FALSE;
			}
			m_mgrProcedure->m_bCont = TRUE;
		}
		InitAuto(bInit);

		m_mgrReelmap->SetPathAtBuf();

		m_mgrProcedure->SetAoiDummyShot(0, m_mgrProcedure->GetAoiUpDummyShot());

		if (bDualTest)
			m_mgrProcedure->SetAoiDummyShot(1, m_mgrProcedure->GetAoiDnDummyShot());

		m_mgrProcedure->m_bAoiFdWrite[0] = FALSE;
		m_mgrProcedure->m_bAoiFdWrite[1] = FALSE;
		m_mgrProcedure->m_bAoiFdWriteF[0] = FALSE;
		m_mgrProcedure->m_bAoiFdWriteF[1] = FALSE;
		//m_bCycleStop = FALSE;
		m_mgrPunch->CycleStop(FALSE);
		m_mgrProcedure->m_bContDiffLot = FALSE;

		m_mgrProcedure->m_bInit = bInit;

		CFileFind cFile;
		BOOL bExistSup, bExistBup, bExistSdn, bExistBdn;

		bExistSup = cFile.FindFile(pDoc->WorkingInfo.System.sPathVrsShareUp + _T("*.pcr"));
		bExistBup = cFile.FindFile(pDoc->WorkingInfo.System.sPathVrsBufUp + _T("*.pcr"));
		if (bDualTest)
		{
			bExistSdn = cFile.FindFile(pDoc->WorkingInfo.System.sPathVrsShareDn + _T("*.pcr"));
			bExistBdn = cFile.FindFile(pDoc->WorkingInfo.System.sPathVrsBufDn + _T("*.pcr"));
			if (bExistSup || bExistSdn ||
				bExistBup || bExistBdn)
			{
				m_mgrReelmap->DelPcrAll();
			}
			else
			{
				m_mgrProcedure->m_bIsBuf[0] = FALSE;
				m_mgrProcedure->m_bIsBuf[1] = FALSE;
			}
		}
		else
		{
			if (bExistSup || bExistBup)
			{
				m_mgrReelmap->DelPcrAll();
			}
			else
			{
				m_mgrProcedure->m_bIsBuf[0] = FALSE;
				m_mgrProcedure->m_bIsBuf[1] = FALSE;
			}
		}

		//pView->TowerLamp(RGB_RED, TRUE, FALSE);
		pView->DispMain(_T("�� ��"), RGB_RED);
		if(m_pDlgMenu03)
			m_pDlgMenu03->SwAoiReset(TRUE);
		m_mgrReelmap->OpenReelmap();

		if (bInit)
		{
			// �ʱ�ȭ
			pDoc->m_ListBuf[0].Clear();
			pDoc->m_ListBuf[1].Clear();
		}
		else
		{
			// �̾��
			SetListBuf();
		}

		return TRUE;
	}

	return FALSE;
}

void CGvisR2R_PunchView::InitMgrPunch()
{
	if (m_mgrPunch)
	{
		delete m_mgrPunch;
		m_mgrPunch = NULL;
	}
	m_mgrPunch = new CManagerPunch(this);
	//m_mgrPunch->Init();
}

void CGvisR2R_PunchView::InitMgrProcedure()
{
	if (m_mgrProcedure)
	{
		delete m_mgrProcedure;
		m_mgrProcedure = NULL;
	}
	m_mgrProcedure = new CManagerProcedure(this);
	//m_mgrProcedure->Init();
}

void CGvisR2R_PunchView::InitMgrReelmap()
{
	if (m_mgrReelmap)
	{
		delete m_mgrReelmap;
		m_mgrReelmap = NULL;
	}
	m_mgrReelmap = new CManagerReelmap(this);
	//m_mgrReelmap->Init();
}

void CGvisR2R_PunchView::SwMenu01DispDefImg(BOOL bOn)
{
	if (m_mgrPunch->m_pEngrave)
		m_mgrPunch->m_pEngrave->SwMenu01DispDefImg(bOn);
}

//BOOL CGvisR2R_PunchView::IsMotionDone(int nAxisId)
//{
//	BOOL bDone = FALSE;
//	if(m_mgrPunch->m_pMotion)
//		bDone = m_mgrPunch->m_pMotion->IsMotionDone(nAxisId);
//
//	return bDone;
//}

int CGvisR2R_PunchView::GetLastShotMk()
{
	int nLastShot = 0;
	if (m_pDlgFrameHigh)
		nLastShot = m_pDlgFrameHigh->m_nMkLastShot;
	return nLastShot;
}

int CGvisR2R_PunchView::GetLastShotUp()
{
	int nLastShot = 0;
	if (m_pDlgFrameHigh)
		nLastShot = m_pDlgFrameHigh->m_nAoiLastShot[0];

	return (nLastShot);
}

int CGvisR2R_PunchView::GetLastShotDn()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return 0;

	int nLastShot = 0;
	if (m_pDlgFrameHigh)
		nLastShot = m_pDlgFrameHigh->m_nAoiLastShot[1];

	return (nLastShot);
}

BOOL CGvisR2R_PunchView::MpeReadBit(CString strRegAddr)
{
	return (m_mgrPunch->m_pMpe->Read(strRegAddr) ? TRUE: FALSE);
}

BOOL CGvisR2R_PunchView::MpeWrite(CString strRegAddr, long lData, BOOL bCheck)
{
	return m_mgrPunch->MpeWrite(strRegAddr, lData, bCheck);
}
//BOOL CGvisR2R_PunchView::MpeWrite(CString strRegAddr, long lData, BOOL bCheck)
//{
//	if (m_pMpe)
//		return MpeWrite(strRegAddr, lData, bCheck);
//	else
//		return FALSE;
//}

void CGvisR2R_PunchView::UpdateData(int nDlgId)
{
	switch (nDlgId)
	{
	case DlgInfo:
		m_pDlgInfo->UpdateData();
		break;
	case DlgFrameHigh:
		break;
	case DlgMenu01:
		m_pDlgMenu01->UpdateData();
		break;
	case DlgMenu02:
		break;
	case DlgMenu03:
		m_pDlgMenu03->UpdateData();
		break;
	case DlgMenu04:
		m_pDlgMenu04->UpdateData();
		break;
	case DlgMenu05:
		break;
	case DlgMenu06:
		m_pDlgMenu06->UpdateData();
		break;
	}
}

int CGvisR2R_PunchView::GetTotDefPcsIts(int nSerial)
{
	return m_mgrReelmap->GetTotDefPcsIts(nSerial);
}

//BOOL CGvisR2R_PunchView::IsMotionEnable()
//{
//	if (m_mgrPunch->m_pMotion->IsEnable(MS_X0) && m_mgrPunch->m_pMotion->IsEnable(MS_Y0) &&
//		m_mgrPunch->m_pMotion->IsEnable(MS_X1) && m_mgrPunch->m_pMotion->IsEnable(MS_Y1))
//		return TRUE;
//
//	return FALSE;
//}
//
BOOL CGvisR2R_PunchView::GetItsSerialInfo(int nItsSerial, BOOL &bDualTest, CString &sLot, CString &sLayerUp, CString &sLayerDn, int nOption)		// ���������� ITS �ø����� ����
{
	return m_mgrReelmap->GetItsSerialInfo(nItsSerial, bDualTest, sLot, sLayerUp, sLayerDn, nOption);
}

void CGvisR2R_PunchView::InitMstData()
{
	m_mgrReelmap->InitMstData();
}

void CGvisR2R_PunchView::SwMenu01UpdateWorking(BOOL bOn)
{
	m_mgrPunch->m_pEngrave->SwMenu01UpdateWorking(bOn);
}

void CGvisR2R_PunchView::ChangeModel()
{
	m_mgrProcedure->InitInfo();
	pView->InitMstData();

	//if (m_mgrPunch->m_pEngrave)
	//	m_mgrPunch->m_pEngrave->SwMenu01UpdateWorking(TRUE);
	pView->SwMenu01UpdateWorking(TRUE);

	SetAlignPos();
	ModelChange(0); // 0 : AOI-Up , 1 : AOI-Dn

	if (m_pDlgMenu01)
	{
		m_pDlgMenu01->InitGL();
		m_bDrawGL = TRUE;
		m_pDlgMenu01->RefreshRmap();
		m_pDlgMenu01->InitCadImg();
		m_pDlgMenu01->SetPnlNum();
		m_pDlgMenu01->SetPnlDefNum();
	}

	if (m_pDlgMenu02)
	{
		m_pDlgMenu02->ChgModelUp(); // PinImg, AlignImg�� Display��.
		m_pDlgMenu02->InitCadImg();
	}
}

CfPoint CGvisR2R_PunchView::GetMkPntIts(int nSerial, int nMkPcs) // pcr �ø���, pcr �ҷ� �ǽ� ���� ���� �ε���
{
	return m_mgrReelmap->GetMkPntIts(nSerial, nMkPcs);
}

void CGvisR2R_PunchView::SetPathAtBuf()
{
	//m_mgrReelmap->m_pReelMap->SetPathAtBuf();
	m_mgrReelmap->SetPathAtBuf();
}

void CGvisR2R_PunchView::SetAoiLastShot(int nAoi, int nSerial)
{
	m_pDlgFrameHigh->SetAoiLastShot(nAoi, nSerial);
}

void CGvisR2R_PunchView::DelSharePcrUp()
{
	m_mgrReelmap->DelSharePcrUp();
}

void CGvisR2R_PunchView::DelSharePcrDn()
{
	m_mgrReelmap->DelSharePcrDn();
}

BOOL CGvisR2R_PunchView::OpenReelmapFromBuf(int nSerial)
{
	return m_mgrReelmap->OpenReelmapFromBuf(nSerial);
}

void CGvisR2R_PunchView::UpdateYieldOnRmap()
{
	m_mgrReelmap->UpdateYieldOnRmap();
}

BOOL CGvisR2R_PunchView::ChkYield() //(double &dTotLmt, double &dPrtLmt, double &dRatio) // ���� ��ȣ : TRUE , ���� �ҷ� : FALSE
{
	return m_mgrReelmap->ChkYield();
}

BOOL CGvisR2R_PunchView::IsBuffer(int nNum)
{
	return m_mgrProcedure->IsBuffer(nNum);
}

void CGvisR2R_PunchView::CycleStop(BOOL bStop)
{
	m_mgrPunch->CycleStop(bStop);
}

BOOL CGvisR2R_PunchView::IsCycleStop()
{
	return m_mgrPunch->IsCycleStop();
}

BOOL CGvisR2R_PunchView::IsRun()
{
	return m_mgrProcedure->IsRun();
}

BOOL CGvisR2R_PunchView::IsNoMk()
{
	return m_mgrPunch->IsNoMk();
}

void CGvisR2R_PunchView::SwAoiRelation()
{
	if (m_pDlgMenu03)
		m_pDlgMenu03->SwAoiRelation();
}

void CGvisR2R_PunchView::SwAoiTblBlw()
{
	if (m_pDlgMenu03)
		m_pDlgMenu03->SwAoiTblBlw();
}

void CGvisR2R_PunchView::SwAoiFdVac()
{
	if (m_pDlgMenu03)
		m_pDlgMenu03->SwAoiFdVac();
}

void CGvisR2R_PunchView::SwAoiTqVac()
{
	if (m_pDlgMenu03)
		m_pDlgMenu03->SwAoiTqVac();
}

void CGvisR2R_PunchView::SetPinPos(int nCam, CfPoint ptPnt)
{
	if (pView->m_mgrPunch->m_pMotion)
		pView->m_mgrPunch->m_pMotion->SetPinPos(nCam, ptPnt.x, ptPnt.y);
	if (pView->m_mgrReelmap->m_Master[0].m_pPcsRgn)
		pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->SetPinPos(nCam, ptPnt);
	if (pDoc->m_pSpecLocal)
		pDoc->m_pSpecLocal->SavePinPos(nCam, ptPnt);

	CString sData, sPath = PATH_WORKING_INFO;
	sData.Format(_T("%.3f"), ptPnt.x);
	pDoc->WorkingInfo.Motion.sPinPosX[nCam] = sData;

	sData.Format(_T("%.3f"), ptPnt.y);
	pDoc->WorkingInfo.Motion.sPinPosY[nCam] = sData;

	if (nCam == 0)
	{
		::WritePrivateProfileString(_T("Motion"), _T("PIN_POS0_X"), pDoc->WorkingInfo.Motion.sPinPosX[nCam], sPath);
		::WritePrivateProfileString(_T("Motion"), _T("PIN_POS0_Y"), pDoc->WorkingInfo.Motion.sPinPosY[nCam], sPath);
	}
	else if (nCam == 1)
	{
		::WritePrivateProfileString(_T("Motion"), _T("PIN_POS1_X"), pDoc->WorkingInfo.Motion.sPinPosX[nCam], sPath);
		::WritePrivateProfileString(_T("Motion"), _T("PIN_POS1_Y"), pDoc->WorkingInfo.Motion.sPinPosY[nCam], sPath);
	}

	SetAlignPos();
}
