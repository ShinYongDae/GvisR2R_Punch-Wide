
// GvisR2R_PunchView.cpp : CGvisR2R_PunchView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
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

#include "Device/SR1000WDEF.h"
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
	ON_MESSAGE(WM_CLIENT_RECEIVED_SR, wmClientReceivedSr)
	ON_MESSAGE(WM_CLIENT_RECEIVED, wmClientReceived)
END_MESSAGE_MAP()

// CGvisR2R_PunchView 생성/소멸

CGvisR2R_PunchView::CGvisR2R_PunchView()
	: CFormView(IDD_GVISR2R_PUNCH_FORM)
{
	// TODO: 여기에 생성 코드를 추가합니다.
	pView = this;
	m_mgrReelmap = NULL;
	m_mgrProcedure = NULL;

	m_pMpe = NULL;
	m_pDlgMyMsg = NULL;
	m_pDlgMsgBox = NULL;

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

	// H/W Device 초기화.....
	m_pMpe = NULL;
	m_pMotion = NULL;
	m_pVoiceCoil[0] = NULL;
	m_pVoiceCoil[1] = NULL;
	m_pLight = NULL;
#ifdef USE_FLUCK
	m_pFluck = NULL;
#endif

#ifdef USE_VISION
	m_pVision[0] = NULL;
	m_pVision[1] = NULL;
	m_pVisionInner[0] = NULL;
	m_pVisionInner[1] = NULL;
#endif

	//m_bTIM_DISP_STATUS = FALSE;

	//// DoMark0(), DoMark1()
	//m_bThread[0] = FALSE;
	//m_dwThreadTick[0] = 0;

	//// ChkCollision
	//m_bThread[1] = FALSE;
	//m_dwThreadTick[1] = 0;

	//// DispDefImg
	//m_bThread[2] = FALSE;
	//m_dwThreadTick[2] = 0;

	//// Engrave Auto Sequence - Response Check
	//m_bThread[3] = FALSE;
	//m_dwThreadTick[3] = 0;

	//m_bTIM_MPE_IO = FALSE;
	
	// client for SR-1000W
	m_pSr1000w = NULL;

	// client for engrave
	m_pEngrave = NULL;
	m_pDts = NULL;

	m_bDestroyedView = FALSE;
	//pView->m_mgrProcedure->m_bContEngraveF = FALSE;

	//pView->m_mgrProcedure->m_nSaveMk0Img = 0;
	//pView->m_mgrProcedure->m_nSaveMk1Img = 0;

	//pView->m_mgrProcedure->m_bStopF_Verify = FALSE;
	//pView->m_mgrProcedure->m_bInitAuto = TRUE;
	//pView->m_mgrProcedure->m_bInitAutoLoadMstInfo = FALSE;

	m_bLoadMstInfo = FALSE;
	m_bLoadMstInfoF = FALSE;

	m_sGet2dCodeLot = _T("");
	m_nGet2dCodeSerial = 0;

	m_bSetSig = FALSE;
	m_bSetSigF = FALSE;
	m_bSetData = FALSE;
	m_bSetDataF = FALSE;

	m_bTIM_START_UPDATE = FALSE;
	m_bEscape = FALSE;
}

CGvisR2R_PunchView::~CGvisR2R_PunchView()
{
	m_mgrProcedure->m_bTIM_MPE_IO = FALSE;
	m_mgrProcedure->m_bTIM_DISP_STATUS = FALSE;
	m_mgrProcedure->m_bTIM_INIT_VIEW = FALSE;
	Sleep(100);

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


	InitIoWrite();
	SetMainMc(FALSE);

	// H/W Device 소멸.....
	HwKill();

	CloseMyMsg();

	if (m_ArrayMyMsgBox.GetSize() > 0)
	{
		m_ArrayMyMsgBox.RemoveAll();
	}

	if (m_mgrReelmap)
	{
		delete m_mgrReelmap;
		m_mgrReelmap = NULL;
	}

	if (m_mgrProcedure)
	{
		delete m_mgrProcedure;
		m_mgrProcedure = NULL;
	}

}

void CGvisR2R_PunchView::DestroyView()
{
	CString sData;
	sData.Format(_T("%d"), pView->m_mgrProcedure->m_nMkStAuto);
	::WritePrivateProfileString(_T("Last Job"), _T("MkStAuto"), sData, PATH_WORKING_INFO);

	if (!m_bDestroyedView)
	{
		m_bDestroyedView = TRUE;

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
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CFormView::PreCreateWindow(cs);
}

void CGvisR2R_PunchView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	InitMyMsg();
	//DoDispMsg(_T("마킹부 프로그램을 실행합니다."), _T("CAD 이미지"), RGB_GREEN, DELAY_TIME_MSG);

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
		sMsg.Format(_T("캠마스터에 스펙폴더가 없습니다. : \n 1.SpecFolder : %s"), sDir);
		pView->ClrDispMsg();
		AfxMessageBox(sMsg, MB_ICONSTOP | MB_OK);
		ExitProgram();
		return;
	}
#endif

	InitMgrProcedure();

	if (!pView->m_mgrProcedure->m_bTIM_INIT_VIEW)
	{
		pView->m_mgrProcedure->m_nStepInitView = 0;
		pView->m_mgrProcedure->m_bTIM_INIT_VIEW = TRUE;
		SetTimer(TIM_INIT_VIEW, 300, NULL);
	}

	pDoc->SetMonDispMain(_T(""));
}


// CGvisR2R_PunchView 진단

#ifdef _DEBUG
void CGvisR2R_PunchView::AssertValid() const
{
	CFormView::AssertValid();
}

void CGvisR2R_PunchView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CGvisR2R_PunchDoc* CGvisR2R_PunchView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGvisR2R_PunchDoc)));
	return (CGvisR2R_PunchDoc*)m_pDocument;
}
#endif //_DEBUG


// CGvisR2R_PunchView 메시지 처리기


void CGvisR2R_PunchView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	int nInc = 0, nSrl = 0;
	CString str, sMsg, sPath, sLot, sLayerUp, sLayerDn;
	BOOL bDualTestInner;
	BOOL bExist = FALSE; 
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	BOOL bRtn = FALSE;

	if (nIDEvent == TIM_INIT_VIEW)
	{
		KillTimer(TIM_INIT_VIEW);

		switch (pView->m_mgrProcedure->m_nStepInitView)
		{
		case 0:
			pView->m_mgrProcedure->m_nStepInitView++;
			DoDispMsg(_T("프로그램을 초기화합니다."), _T("알림"), RGB_GREEN, DELAY_TIME_MSG);
			InitMgrReelmap();
			DtsInit();
			// H/W Device 초기화.....
			HwInit();

			break;
		case 1:
			pView->m_mgrProcedure->m_nStepInitView++;
			InitIO();
			break;
		case 2:
			pView->m_mgrProcedure->m_nStepInitView++;
			InitIoWrite();
			SetMainMc(TRUE);
			pView->m_mgrProcedure->m_nMonAlmF = 0;
			pView->m_mgrProcedure->m_nClrAlmF = 0;
			break;
		case 3:
			pView->m_mgrProcedure->m_nStepInitView++;
			//ThreadInit();
			break;
		case 4:
			pView->m_mgrProcedure->m_nStepInitView++;
			break;
		case 5:
			pView->m_mgrProcedure->m_nStepInitView++;
			DispMsg(_T("화면구성을 생성합니다.- 1"), _T("알림"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_MENU_02);
			//ShowDlg(IDD_DLG_UTIL_01);
			break;
		case 6:
			pView->m_mgrProcedure->m_nStepInitView++;
			//ShowDlg(IDD_DLG_UTIL_02);
			break;
		case 7:
			pView->m_mgrProcedure->m_nStepInitView++;
			break;
		case 8:
			pView->m_mgrProcedure->m_nStepInitView++;
			DispMsg(_T("화면구성을 생성합니다.-2"), _T("알림"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_MENU_01);

			if (bDualTest)
				pView->m_pDlgMenu01->SelMap(ALL);
			else
				pView->m_pDlgMenu01->SelMap(UP);
			break;
		case 9:
			pView->m_mgrProcedure->m_nStepInitView++;
			DispMsg(_T("화면구성을 생성합니다.- 3"), _T("알림"), RGB_GREEN, DELAY_TIME_MSG);
			//ShowDlg(IDD_DLG_MENU_02);
			break;
		case 10:
			pView->m_mgrProcedure->m_nStepInitView++;
			DispMsg(_T("화면구성을 생성합니다.- 4"), _T("알림"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_MENU_03);
			break;
		case 11:
			pView->m_mgrProcedure->m_nStepInitView++;
			DispMsg(_T("화면구성을 생성합니다.- 5"), _T("알림"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_MENU_04);
			break;
		case 12:
			pView->m_mgrProcedure->m_nStepInitView++;
			DispMsg(_T("화면구성을 생성합니다.- 6"), _T("알림"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_MENU_05);
			break;
		case 13:
			pView->m_mgrProcedure->m_nStepInitView++;
			DispMsg(_T("화면구성을 생성합니다.- 7"), _T("알림"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_MENU_06);
			break;
		case 14:
			pView->m_mgrProcedure->m_nStepInitView++;
			DispMsg(_T("화면구성을 생성합니다.- 8"), _T("알림"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_FRAME_HIGH);
			if (m_pDlgFrameHigh)
				m_pDlgFrameHigh->ChkMenu01();
			SetDualTest(pDoc->WorkingInfo.LastJob.bDualTest);

			if (pDoc->GetCurrentInfoEng())
			{
				if (pView->m_mgrReelmap->GetItsSerialInfo(0, bDualTestInner, sLot, sLayerUp, sLayerDn, 0))
				{
					//if (pDoc->GetTestMode() == MODE_OUTER)
					if (pView->m_mgrReelmap->m_Master[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, sLayerUp))
					{
						if (m_pDlgMenu06)
							m_pDlgMenu06->RedrawWindow();
					}
				}
			}

			Init();
			Sleep(300);
			break;
		case 15:
			pView->m_mgrProcedure->m_nStepInitView++;
			m_bLoadMstInfo = TRUE;
			DispMsg(_T("H/W를 초기화합니다."), _T("알림"), RGB_GREEN, DELAY_TIME_MSG);
			InitAct();
			pView->m_mgrProcedure->m_bStopFeeding = TRUE;
			m_pMpe->Write(_T("MB440115"), 1); // 마킹부Feeding금지
			Sleep(300);
			break;
		case 16:
			if (m_pMotion)
			{
				DispMsg(_T("Homming"), _T("Searching Home Position..."), RGB_GREEN, 2000, TRUE);
				m_pMotion->SearchHome();
				pView->m_mgrProcedure->m_nStepInitView++;
			}
			else
			{
				pView->m_mgrProcedure->m_bTIM_INIT_VIEW = FALSE;
				pView->ClrDispMsg();
				AfxMessageBox(_T("Motion is failed."));
				PostMessage(WM_CLOSE);
			}
			break;
		case 17:
			if (m_pMotion)
			{
				if (m_pMotion->IsHomeDone())// && m_pMotion->IsHomeDone(MS_MKFD))
				{
					pView->m_mgrProcedure->m_nStepInitView++;
					Sleep(300);
				}

				sMsg.Format(_T("X0(%s) , Y0(%s)\r\nX1(%s) , Y1(%s)"), m_pMotion->IsHomeDone(MS_X0) ? _T("Done") : _T("Doing"),
					m_pMotion->IsHomeDone(MS_Y0) ? _T("Done") : _T("Doing"),
					m_pMotion->IsHomeDone(MS_X1) ? _T("Done") : _T("Doing"),
					m_pMotion->IsHomeDone(MS_Y1) ? _T("Done") : _T("Doing"));
				DispMsg(sMsg, _T("Homming"), RGB_GREEN, 2000, TRUE);
			}
			else
			{
				pView->m_mgrProcedure->m_bTIM_INIT_VIEW = FALSE;
				pView->ClrDispMsg();
				AfxMessageBox(_T("Motion is failed."));
				PostMessage(WM_CLOSE);
			}
			break;
		case 18:
			pView->m_mgrProcedure->m_nStepInitView++;
			break;
		case 19:
			pView->m_mgrProcedure->m_nStepInitView++;
			DispMsg(_T("Completed Searching Home Pos..."), _T("Homming"), RGB_GREEN, 2000, TRUE);
			m_pMotion->ObjectMapping();
			m_bEscape = FALSE;

			if (m_pVoiceCoil[0])
			{
				m_pVoiceCoil[0]->SetMarkFinalData(0);
			}

			if (m_pVoiceCoil[1])
			{
				m_pVoiceCoil[1]->SetMarkFinalData(1);
			}

			break;
		case 20:
			pView->m_mgrProcedure->m_bStopFeeding = FALSE;
			m_pMpe->Write(_T("MB440115"), 0); // 마킹부Feeding금지
			pView->m_mgrProcedure->m_nStepInitView++;
			if(m_pDlgMenu02)
				m_pDlgMenu02->SetJogSpd(_tstoi(pDoc->WorkingInfo.LastJob.sJogSpd));
			if (m_pDlgMenu03)
				m_pDlgMenu03->InitRelation();
			m_pMotion->SetR2RConf();
			TowerLamp(RGB_YELLOW, TRUE);

			if (!SetCollision(-1.0*_tstof(pDoc->WorkingInfo.Motion.sCollisionLength)))
			{
				DispMsg(_T("Collision"), _T("Failed to Set Collision ..."), RGB_GREEN, 2000, TRUE);
			}

			break;
		case 21:
			pView->m_mgrProcedure->m_nStepInitView++;
			InitPLC();
			SetPlcParam();
			GetPlcParam();
			TcpIpInit();
			DtsInit();
			pView->m_mgrProcedure->m_bTIM_DISP_STATUS = TRUE;
			SetTimer(TIM_DISP_STATUS, 100, NULL);

			m_bTIM_START_UPDATE = TRUE;
			SetTimer(TIM_START_UPDATE, 500, NULL);
			break;
		case 22:
			if (m_bLoadMstInfo || m_bLoadMstInfoF)
				break;

			pView->m_mgrProcedure->m_nStepInitView++;
			ClrDispMsg();

			bRtn = pView->m_mgrReelmap->LoadPcrFromBuf();

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
				if (pView->m_mgrReelmap->m_pReelMap && bRtn)
				{
					pView->m_mgrReelmap->ReloadReelmap();
					UpdateRst();
					UpdateLotTime();
				}

				//if (pDoc->GetTestMode() == MODE_OUTER)
				if (pDoc->GetCurrentInfoEng())
				{
					if (pView->m_mgrReelmap->GetItsSerialInfo(0, bDualTestInner, sLot, sLayerUp, sLayerDn, 3))
					{
						if (pView->m_mgrReelmap->m_Master[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, sLayerUp))
						{
							if (m_pDlgMenu06)
							{
								m_pDlgMenu06->SetStripAllMk();
								m_pDlgMenu06->RefreshRmap();
								m_pDlgMenu06->ResetLastProc();
							}

							if (pView->m_mgrReelmap->m_pReelMapInner)
							{
								pView->m_mgrReelmap->ReloadReelmapInner();
								UpdateRstInner();
							}
						}
					}
				}

			}

			MoveInitPos1();
			Sleep(30);
			MoveInitPos0();

			SetLotLastShot();
			StartLive();

			if (pDoc->GetTestMode() == MODE_OUTER)
			{
				if (m_pDlgMenu01)
					m_pDlgMenu01->EnableItsMode();
			}

			pView->m_mgrProcedure->m_bTIM_MPE_IO = TRUE;
			SetTimer(TIM_MPE_IO, 50, NULL);

			if(pView->m_mgrProcedure->m_bMkSt)
				LoadSerial();

			pView->m_mgrProcedure->m_bTIM_INIT_VIEW = FALSE;
			break;
		}

		if (pView->m_mgrProcedure->m_bTIM_INIT_VIEW)
			SetTimer(TIM_INIT_VIEW, 100, NULL);
	}

	if (nIDEvent == TIM_MPE_IO)
	{
		KillTimer(TIM_MPE_IO);

		CntMk();
		GetMpeIO();
		GetMpeSignal();
		//GetMpeData();
		DoIO();
		ChkMyMsg();

		//if (m_pDlgMenu03)
		//	m_pDlgMenu03->UpdateSignal();

		if (pView->m_mgrProcedure->m_bTIM_MPE_IO)
			SetTimer(TIM_MPE_IO, 100, NULL);
	}

	if (nIDEvent == TIM_TOWER_WINKER)
	{
		KillTimer(TIM_TOWER_WINKER);
		DispTowerWinker();
		if (pView->m_mgrProcedure->m_bTimTowerWinker)
			SetTimer(TIM_TOWER_WINKER, 100, NULL);
	}


	if (nIDEvent == TIM_BUZZER_WARN)
	{
		KillTimer(TIM_BUZZER_WARN);
		pView->m_mgrProcedure->m_nCntBz++;
		if (pView->m_mgrProcedure->m_nCntBz > BUZZER_DELAY)
		{
			pView->m_mgrProcedure->m_bTimBuzzerWarn = FALSE;
			Buzzer(FALSE);
		}
		if (pView->m_mgrProcedure->m_bTimBuzzerWarn)
			SetTimer(TIM_BUZZER_WARN, 100, NULL);
	}

	if (nIDEvent == TIM_DISP_STATUS)
	{
		KillTimer(TIM_DISP_STATUS);

		DispStsBar();
		DoDispMain();
		//DispMyMsgBox();

		if (pView->m_mgrProcedure->m_bStopFromThread)
		{
			pView->m_mgrProcedure->m_bStopFromThread = FALSE;
			Stop();
		}
		if (pView->m_mgrProcedure->m_bBuzzerFromThread)
		{
			pView->m_mgrProcedure->m_bBuzzerFromThread = FALSE;
			Buzzer(TRUE, 0);
		}


		//SetMpeIO();

		ChkEmg();
		ChkSaftySen();
		ChkDoor();

		if (pView->m_mgrProcedure->m_bTIM_DISP_STATUS)
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
		if (!(pDoc->m_pMpeSignal[7] & (0x01 << 3)))	// 일시정지사용(PC가 On시키고, PLC가 확인하고 Off시킴)-20141031
		{
			pView->m_mgrProcedure->m_bTIM_CHK_TEMP_STOP = FALSE;
			m_pDlgMenu01->SetTempStop(FALSE);
		}
#endif
		if (pView->m_mgrProcedure->m_bTIM_CHK_TEMP_STOP)
			SetTimer(TIM_CHK_TEMP_STOP, 500, NULL);
	}

	if (nIDEvent == TIM_SAFTY_STOP)
	{
		KillTimer(TIM_SAFTY_STOP);
		MsgBox(_T("일시정지 - 마킹부 안전센서가 감지되었습니다."));
		pView->m_mgrProcedure->m_bTIM_SAFTY_STOP = FALSE;
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

			if (m_pEngrave->m_bGetOpInfo || m_pEngrave->m_bGetInfo)
			{
				if (m_pDlgInfo)
					m_pDlgInfo->UpdateData();

				if (m_pDlgMenu01)
					m_pDlgMenu01->UpdateData();

				m_pEngrave->m_bGetOpInfo = FALSE;
				m_pEngrave->m_bGetInfo = FALSE;
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

			if (m_pEngrave->m_bGetOpInfo || m_pEngrave->m_bGetInfo)
			{
				if (m_pDlgInfo)
					m_pDlgInfo->UpdateData();

				if (m_pDlgMenu01)
					m_pDlgMenu01->UpdateData();

				m_pEngrave->m_bGetOpInfo = FALSE;
				m_pEngrave->m_bGetInfo = FALSE;
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
		if (pView->m_mgrReelmap->LoadMstInfo())
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
				pView->m_mgrProcedure->m_bDrawGL = TRUE;
				m_pDlgMenu01->RefreshRmap();
				m_pDlgMenu01->InitCadImg();
				m_pDlgMenu01->SetPnlNum();
				m_pDlgMenu01->SetPnlDefNum();
			}

			if (m_pDlgMenu02)
			{
				m_pDlgMenu02->ChgModelUp(); // PinImg, AlignImg를 Display함.
				m_pDlgMenu02->InitCadImg();
			}

			if (m_pDlgMenu01)
				m_pDlgMenu01->RedrawWindow();

			int nSrl = pDoc->GetLastShotMk();
			SetMkFdLen();
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
		if (m_pEngrave)
		{
			pDoc->m_sMsgBox = sMsg;
			if (pDoc->m_sIsMsgBox != pDoc->m_sMsgBox)
			{
				if (m_pEngrave)
					m_pEngrave->SetMsgBox(pDoc->m_sMsgBox, nType);
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



void CGvisR2R_PunchView::ChkMyMsg()
{
	return;

	CWnd *pWndForeground;

	if (pView->m_mgrProcedure->m_bShowMyMsg && pView->m_mgrProcedure->m_pMyMsgForeground)
	{
		pWndForeground = pFrm->GetForegroundWindow();
		if (pWndForeground != pView->m_mgrProcedure->m_pMyMsgForeground)
			pView->m_mgrProcedure->m_pMyMsgForeground->SetForegroundWindow();
	}
}

void CGvisR2R_PunchView::UpdateLotTime()
{
	pView->m_mgrProcedure->m_dwLotSt = (DWORD)pDoc->WorkingInfo.Lot.dwStTick;

	if (m_pDlgMenu01)
		m_pDlgMenu01->UpdateLotTime();
}

void CGvisR2R_PunchView::DispStsBar(CString sMsg, int nIdx)
{
	if (pView->m_mgrProcedure->m_sDispMsg[nIdx] != sMsg)
		pView->m_mgrProcedure->m_sDispMsg[nIdx] = sMsg;
	sMsg.Empty();
}

void CGvisR2R_PunchView::DispStsBar()
{
	DispStsMainMsg(); // 0
	//DispStsMainMsg(6); // 6
	//DispStsMainMsg(5); // 5
	//DispThreadTick(); // 5, 6
	DispTime(); // 7
	pView->m_mgrProcedure->ChkShare(); // 2, 4
	if (!pView->m_mgrProcedure->m_bShift2Mk && !pView->m_mgrProcedure->m_bTHREAD_SHIFT2MK 
		&& (pView->m_mgrProcedure->m_nMkStAuto < MK_ST + (Mk2PtIdx::DoneMk) 
			|| pView->m_mgrProcedure->m_nMkStAuto > MK_ST + (Mk2PtIdx::DoneMk) + 5 
			|| pView->m_mgrProcedure->m_bTIM_INIT_VIEW))
	{
		pView->m_mgrProcedure->ChkBuf(); // 1, 3
		if (pView->m_mgrProcedure->m_bTIM_INIT_VIEW)
			SetListBuf();
	}
}

BOOL CGvisR2R_PunchView::MemChk() // 릴맵화면에 표시할 데이터를 담을 구조체 할당을 확인함.
{
	if (!pView->m_mgrReelmap->m_pPcr[0] || !pView->m_mgrReelmap->m_pPcr[1])// || !pView->m_mgrReelmap->m_pReelMap)
		return FALSE;
	return TRUE;
}

void CGvisR2R_PunchView::SetMainMc(BOOL bOn)
{
	if (bOn)
	{
		if (m_pMpe)
			m_pMpe->Write(_T("MB440159"), 1); // 마킹부 Door Interlock ON
			//m_pMpe->Write(_T("MB440159"), 1); // 마킹부 MC ON
	}
	else
	{
		if (m_pMpe)
			m_pMpe->Write(_T("MB440158"), 0); // 마킹부 Door Interlock OFF
			//m_pMpe->Write(_T("MB440159"), 0); // 마킹부 MC OFF
	}
}

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

void CGvisR2R_PunchView::Init()
{
	int nAxis;
	if (m_pMotion)
	{
		for (nAxis = 0; nAxis<m_pMotion->m_ParamCtrl.nTotAxis; nAxis++)
		{
			m_pMotion->AmpReset(nAxis);
			Sleep(30);
		}
	}
}

void CGvisR2R_PunchView::InitIO()
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
		if (pDoc->m_nMpeIb>0)
		{
			pDoc->m_pMpeIF = new unsigned short[pDoc->m_nMpeIb];
			for (i = 0; i<pDoc->m_nMpeIb; i++)
				pDoc->m_pMpeIF[i] = 0;
		}
	}

	if (!pDoc->m_pMpeIo)
	{
		if (pDoc->m_nMpeIo>0)
		{
			pDoc->m_pMpeIo = new unsigned short[pDoc->m_nMpeIo];
			for (i = 0; i<pDoc->m_nMpeIo; i++)
				pDoc->m_pMpeIo[i] = 0;
		}
	}

	if (!pDoc->m_pMpeIoF)
	{
		if (pDoc->m_nMpeIo>0)
		{
			pDoc->m_pMpeIoF = new unsigned short[pDoc->m_nMpeIo];
			for (i = 0; i<pDoc->m_nMpeIo; i++)
				pDoc->m_pMpeIoF[i] = 0;
		}
	}


	pDoc->m_nMpeSignal = pDoc->MkIo.MpeSignal.nMaxSeg;
	if (!pDoc->m_pMpeSignal)
	{
		if (pDoc->m_nMpeSignal>0)
		{
			pDoc->m_pMpeSignal = new unsigned short[pDoc->m_nMpeSignal];
			for (i = 0; i<pDoc->m_nMpeSignal; i++)
				pDoc->m_pMpeSignal[i] = 0;
		}
	}


	pDoc->m_nMpeData = pDoc->MkIo.MpeData.nMaxSeg;
	if (!pDoc->m_pMpeData)
	{
		if (pDoc->m_nMpeData>0)
		{
			pDoc->m_pMpeData = new long*[pDoc->m_nMpeData];
			for (i = 0; i<pDoc->m_nMpeData; i++)
			{
				pDoc->m_pMpeData[i] = new long[16];
				for (k = 0; k<16; k++)
					pDoc->m_pMpeData[i][k] = 0;
			}
		}
	}
#endif
}

BOOL CGvisR2R_PunchView::InitAct()
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
		for (nAxis = 0; nAxis<m_pMotion->m_ParamCtrl.nTotAxis; nAxis++)
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
	if (m_pDlgMenu02)
	{
		m_pDlgMenu02->SetLight(_tstoi(pDoc->WorkingInfo.Light.sVal[0]));
		m_pDlgMenu02->SetLight2(_tstoi(pDoc->WorkingInfo.Light.sVal[1]));
	}

	// Homming
	if (m_pVoiceCoil[0])
		m_pVoiceCoil[0]->SearchHomeSmac(0);
	if (m_pVoiceCoil[1])
		m_pVoiceCoil[1]->SearchHomeSmac(1);

	return TRUE;
}

BOOL CGvisR2R_PunchView::TcpIpInit()
{
#ifdef USE_SR1000W
	if (!m_pSr1000w)
	{
		m_pSr1000w = new CSr1000w(pDoc->WorkingInfo.System.sIpClient[ID_SR1000W], pDoc->WorkingInfo.System.sIpServer[ID_SR1000W], pDoc->WorkingInfo.System.sPort[ID_SR1000W], this);
		//m_pSr1000w->SetHwnd(this->GetSafeHwnd());
	}
#endif	

#ifdef USE_TCPIP
	if (!m_pEngrave)
	{
		m_pEngrave = new CEngrave(pDoc->WorkingInfo.System.sIpClient[ID_PUNCH], pDoc->WorkingInfo.System.sIpServer[ID_ENGRAVE], pDoc->WorkingInfo.System.sPort[ID_ENGRAVE], this);
		m_pEngrave->SetHwnd(this->GetSafeHwnd());
	}
#endif

	return TRUE;
}

void CGvisR2R_PunchView::DtsInit()
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

BOOL CGvisR2R_PunchView::HwInit()
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
		MsgBox(_T("XMP 보드 초기화 실패, 다시 시작하세요.!!!"));
		PostMessage(WM_CLOSE);
		return FALSE;
	}

#ifdef USE_MPE
	if (!m_pMpe)
		m_pMpe = new CMpDevice(this);
	if (!m_pMpe->Init(1, 1))
	{
		MsgBox(_T("메카트로링크(MC0)의 통신프로그램을 실행 후, 다시 시작하세요.!!!"));
		PostMessage(WM_CLOSE);
		return FALSE;
	}
#endif

	return TRUE;
}

void CGvisR2R_PunchView::HwKill()
{
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

	if (m_pEngrave)
	{
		m_pEngrave->Close();
		delete m_pEngrave;
		m_pEngrave = NULL;
	}

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

void CGvisR2R_PunchView::GetDispMsg(CString &strMsg, CString &strTitle)
{
	if (m_pDlgMsgBox)
		m_pDlgMsgBox->GetDispMsg(strMsg, strTitle);
}

void CGvisR2R_PunchView::DispMsg(CString strMsg, CString strTitle, COLORREF color, DWORD dwDispTime, BOOL bOverWrite)
{
	if (!pView->m_mgrProcedure)
		return;

	if (pView->m_mgrProcedure->m_bDispMsg)
		return;

	if (pView->m_mgrProcedure->m_bAuto)
	{
		return;
	}

	pView->m_mgrProcedure->m_bDispMsg = TRUE;
	DoDispMsg(strMsg, strTitle, color, dwDispTime, bOverWrite);
	pView->m_mgrProcedure->m_bDispMsg = FALSE;
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
	pView->m_mgrProcedure->m_bWaitClrDispMsg = TRUE;
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
	pView->m_mgrProcedure->m_bWaitClrDispMsg = FALSE;
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

void CGvisR2R_PunchView::TowerLamp(COLORREF color, BOOL bOn, BOOL bWink)
{
	if (bWink)
	{
		if (color == RGB_RED)
		{
			pView->m_mgrProcedure->m_bTowerWinker[0] = bOn;
			if (bOn)
			{
				pView->m_mgrProcedure->m_bTowerWinker[1] = FALSE;
				pView->m_mgrProcedure->m_bTowerWinker[2] = FALSE;
			}
		}
		if (color == RGB_GREEN)
		{
			pView->m_mgrProcedure->m_bTowerWinker[1] = bOn;
			if (bOn)
			{
				pView->m_mgrProcedure->m_bTowerWinker[0] = FALSE;
				pView->m_mgrProcedure->m_bTowerWinker[2] = FALSE;
			}
		}
		if (color == RGB_YELLOW)
		{
			pView->m_mgrProcedure->m_bTowerWinker[2] = bOn;
			if (bOn)
			{
				pView->m_mgrProcedure->m_bTowerWinker[1] = FALSE;
				pView->m_mgrProcedure->m_bTowerWinker[0] = FALSE;
			}
		}

		if (!pView->m_mgrProcedure->m_bTowerWinker[0] && !pView->m_mgrProcedure->m_bTowerWinker[1] && !pView->m_mgrProcedure->m_bTowerWinker[2])
			pView->m_mgrProcedure->m_bTimTowerWinker = FALSE;
		else
		{
			if (!pView->m_mgrProcedure->m_bTimTowerWinker)
			{
				pView->m_mgrProcedure->m_bTimTowerWinker = TRUE;
				SetTimer(TIM_TOWER_WINKER, 100, NULL);
			}
		}
	}
	else
	{
		pView->m_mgrProcedure->m_bTimTowerWinker = FALSE;

#ifdef USE_MPE
		if (!pDoc->m_pMpeIo)
			return;
#endif
	}
}

void CGvisR2R_PunchView::DispTowerWinker()
{
	pView->m_mgrProcedure->m_nCntTowerWinker++;
	if (pView->m_mgrProcedure->m_nCntTowerWinker > LAMP_DELAY)
	{
		pView->m_mgrProcedure->m_nCntTowerWinker = 0;
	}

}

void CGvisR2R_PunchView::BuzzerFromThread(BOOL bOn, int nCh)
{
	pView->m_mgrProcedure->m_bBuzzerFromThread = TRUE;
}

void CGvisR2R_PunchView::Buzzer(BOOL bOn, int nCh)
{
	//	return; // PLC제어
#ifdef USE_MPE
	if (!m_pMpe)
		return;

	if (!bOn)
	{
		switch (nCh)
		{
		case 0:
			m_pMpe->Write(_T("MB44015E"), 0);
			break;
		case 1:
			m_pMpe->Write(_T("MB44015F"), 0);
			break;
		}
	}
	else
	{
		switch (nCh)
		{
		case 0:
			pView->m_pMpe->Write(_T("MB44015E"), 0);
			Sleep(300);
			pView->m_pMpe->Write(_T("MB44015E"), 1);
			break;
		case 1:
			pView->m_pMpe->Write(_T("MB44015E"), 0);
			Sleep(300);
			pView->m_pMpe->Write(_T("MB44015F"), 1);
			break;
		}
	}
#endif
}

//void CGvisR2R_PunchView::ThreadInit()
//{/*
//	// DoMark0(), DoMark1()                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
//	if (!m_bThread[0])
//		m_Thread[0].Start(GetSafeHwnd(), this, ThreadProc0);
//
//	// ChkCollision
//	if (!m_bThread[1])
//		m_Thread[1].Start(GetSafeHwnd(), this, ThreadProc1);
//
//	// DispDefImg
//	if (!m_bThread[2])
//		m_Thread[2].Start(GetSafeHwnd(), this, ThreadProc2);
//
//	// Engrave Auto Sequence - Response Check
//	if (!m_bThread[3])
//		m_Thread[3].Start(GetSafeHwnd(), this, ThreadProc3);
//
//	// DispDefImgInner
//	if (!m_bThread[4])
//		m_Thread[4].Start(GetSafeHwnd(), this, ThreadProc4);
//
//	// DoShift2Mk
//	if (!m_bThread[5])
//		m_Thread[5].Start(GetSafeHwnd(), this, ThreadProc5);
//		*/
//}

//void CGvisR2R_PunchView::ThreadKill()
//{
//	if (m_bThread[0])	// DoMark0(), DoMark1()
//	{
//		m_Thread[0].Stop();
//		Sleep(20);
//		while (m_bThread[0])
//		{
//			Sleep(20);
//		}
//	}
//
//	if (m_bThread[1])	// ChkCollision
//	{
//		m_Thread[1].Stop();
//		Sleep(20);
//		while (m_bThread[1])
//		{
//			Sleep(20);
//		}
//	}
//
//	if (m_bThread[2])	// DispDefImg
//	{
//		m_Thread[2].Stop();
//		Sleep(20);
//		while (m_bThread[2])
//		{
//			Sleep(20);
//		}
//	}
//
//	if (m_bThread[3])	// Engrave Auto Sequence - Response Check
//	{
//		m_Thread[3].Stop();
//		Sleep(20);
//		while (m_bThread[3])
//		{
//			Sleep(20);
//		}
//	}
//
//	//if (m_bThread[4])	// DispDefImgInner
//	//{
//	//	m_Thread[4].Stop();
//	//	Sleep(20);
//	//	while (m_bThread[4])
//	//	{
//	//		Sleep(20);
//	//	}
//	//}
//
//	//if (m_bThread[5])	// DoShift2Mk
//	//{
//	//	m_Thread[5].Stop();
//	//	Sleep(20);
//	//	while (m_bThread[5])
//	//	{
//	//		Sleep(20);
//	//	}
//	//}
//}

/*
UINT CGvisR2R_PunchView::ThreadProc0(LPVOID lpContext)	// DoMark0(), DoMark1()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast< CGvisR2R_PunchView* >(lpContext);

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
			if (pView->m_mgrReelmap->m_Master[0].MasterInfo.nActionCode == 1 || pView->m_mgrReelmap->m_Master[0].MasterInfo.nActionCode == 3)	// 0 : Rotation / Mirror 적용 없음(CAM Data 원본), 1 : 좌우 미러, 2 : 상하 미러, 3 : 180 회전, 4 : 270 회전(CCW), 5 : 90 회전(CW)
			{
				if (pThread->m_bTHREAD_MK[0])
				{
					if (pThread->m_nBufUpSerial[0] > 0)
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
				if (pThread->m_bTHREAD_MK[1])
				{
					if (pThread->m_nBufUpSerial[1] > 0)
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
			else
			{
				if (pThread->m_bTHREAD_MK[1])
				{
					if (pThread->m_nBufUpSerial[1] > 0)
						pThread->DoMark1();
					else
					{
						pThread->m_bDoneMk[1] = TRUE;
						pThread->m_bTHREAD_MK[1] = FALSE;
					}
				}
				if (pThread->m_bTHREAD_MK[0])
				{
					if (pThread->m_nBufUpSerial[0] > 0)
						pThread->DoMark0();
					else
					{
						pThread->m_bDoneMk[0] = TRUE;
						pThread->m_bTHREAD_MK[0] = FALSE;
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

UINT CGvisR2R_PunchView::ThreadProc1(LPVOID lpContext)	// ChkCollision()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast< CGvisR2R_PunchView* >(lpContext);

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

UINT CGvisR2R_PunchView::ThreadProc2(LPVOID lpContext)	// DispDefImg()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast< CGvisR2R_PunchView* >(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[2] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[2].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[2] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (!bLock)
		{
			bLock = TRUE;

			if (pThread->m_bTHREAD_DISP_DEF)
			{
				pThread->DispDefImg();
				Sleep(0);
			}
			else
				Sleep(30);
			

			bLock = FALSE;
		}
	}
	pThread->m_bThread[2] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc3(LPVOID lpContext)	// GetCurrentInfoSignal()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[3] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[3].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[3] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (!pThread->m_bDestroyedView)
		{
			if (!bLock)
			{
				bLock = TRUE;
				pThread->GetCurrentInfoSignal();
				bLock = FALSE;
			}
		}
		Sleep(100);
	}
	pThread->m_bThread[3] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc4(LPVOID lpContext) // DispDefImgInner()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[4] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[4].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[4] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (!bLock)
		{
			bLock = TRUE;

			if (pThread->m_bTHREAD_DISP_DEF_INNER)
			{
				pThread->DispDefImgInner();
				Sleep(0);
			}
			else
				Sleep(30);


			bLock = FALSE;
		}
	}
	pThread->m_bThread[4] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc5(LPVOID lpContext)	// RunShift2Mk()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[5] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[5].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[5] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_SHIFT2MK)
		{
			if (!pThread->m_bTHREAD_UPDATAE_YIELD[0] && !pThread->m_bTHREAD_UPDATAE_YIELD[1])
			{
				pThread->RunShift2Mk();
				pThread->m_bTHREAD_SHIFT2MK = FALSE;
				Sleep(0);
			}
			else
				Sleep(30);

		}
		else
			Sleep(30);
	}

	pThread->m_bThread[5] = FALSE;

	return 0;
}
*/
void CGvisR2R_PunchView::DispStsMainMsg(int nIdx)
{
	CString str;
	str = pView->m_mgrProcedure->m_sDispMsg[nIdx];
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
	if (m_pVision[0])
		m_pVision[0]->GetFramesPerSecond(&dFPS[0]);
	if (m_pVision[1])
		m_pVision[1]->GetFramesPerSecond(&dFPS[1]);
	str.Format(_T("%.1f,%.1f"), dFPS[0], dFPS[1]);
	pFrm->DispStatusBar(str, 6);
#else
	str.Format(_T("%d,%d,%d"), pView->m_mgrProcedure->m_nStepAuto, pView->m_mgrProcedure->m_nMkStAuto, pView->m_mgrProcedure->m_nLotEndAuto);
	pFrm->DispStatusBar(str, 6);
#endif
}

void CGvisR2R_PunchView::SwJog(int nAxisID, int nDir, BOOL bOn)
{
	if (!pView->m_pMotion)
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
			double dPos = pView->m_mgrProcedure->m_dEnc[nAxisID]; // m_pMotion->GetActualPosition(nAxisID);
			if (nDir == M_CW)
				dPos += dStep;
			else if (nDir == M_CCW)
				dPos -= dStep;

			if (nAxisID == AXIS_X0)
			{
				if (pView->m_mgrProcedure->m_bAuto && pView->m_mgrProcedure->m_bTHREAD_MK[0] && pView->m_mgrProcedure->m_bTHREAD_MK[1] && IsReview())
				{
					if (nDir == M_CW) // ▶ Jog 버튼.
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
				if (pView->m_mgrProcedure->m_bAuto && pView->m_mgrProcedure->m_bTHREAD_MK[0] && pView->m_mgrProcedure->m_bTHREAD_MK[1] && IsReview())
				{
					if (nDir == M_CW) // ▶ Jog 버튼.
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
		if (!m_pDlgMenu02)
			return;

		if (nAxisID == AXIS_Y0)
		{
			if (nDir == M_CCW)		// Up
			{
				if (bOn)
					m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_UP);
				else
					m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_UP);
			}
			else if (nDir == M_CW)	// Dn
			{
				if (bOn)
					m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_DN);
				else
					m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_DN);
			}
		}
		else if (nAxisID == AXIS_X0)
		{
			if (pView->m_mgrProcedure->m_bAuto && pView->m_mgrProcedure->m_bTHREAD_MK[0] && pView->m_mgrProcedure->m_bTHREAD_MK[1] && IsReview())
			{
				if (nDir == M_CW) // ▶ Jog 버튼.
					return;
			}

			if (nDir == M_CW)		// Rt
			{
				if (bOn)
					m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_RT);
				else
					m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_RT);
			}
			else if (nDir == M_CCW)	// Lf
			{
				if (bOn)
					m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_LF);
				else
					m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_LF);
			}
		}
		else if (nAxisID == AXIS_Y1)
		{
			if (nDir == M_CCW)		// Up
			{
				if (bOn)
					m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_UP2);
				else
					m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_UP2);
			}
			else if (nDir == M_CW)	// Dn
			{
				if (bOn)
					m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_DN2);
				else
					m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_DN2);
			}
		}
		else if (nAxisID == AXIS_X1)
		{
			if (pView->m_mgrProcedure->m_bAuto && pView->m_mgrProcedure->m_bTHREAD_MK[0] && pView->m_mgrProcedure->m_bTHREAD_MK[1] && IsReview())
			{
				if (nDir == M_CW) // ▶ Jog 버튼.
					return;
			}

			if (nDir == M_CW)		// Rt
			{
				if (bOn)
					m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_RT2);
				else
					m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_RT2);
			}
			else if (nDir == M_CCW)	// Lf
			{
				if (bOn)
					m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_LF2);
				else
					m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_LF2);
			}
		}
	}
}


//BOOL CGvisR2R_PunchView::ChkShareIdx(int *pBufSerial, int nBufTot, int nShareSerial)
//{
//	if (nBufTot<1)
//		return TRUE;
//	for (int i = 0; i<nBufTot; i++)
//	{
//		if (pBufSerial[i] == nShareSerial)
//			return FALSE;
//	}
//	return TRUE;
//}
//
//BOOL CGvisR2R_PunchView::ChkVsShare(int &nSerial)
//{
//	int nS0, nS1;
//	BOOL b0 = ChkVsShareUp(nS0);
//	BOOL b1 = ChkVsShareDn(nS1);
//
//	if (!b0 || !b1)
//	{
//		nSerial = -1;
//		return FALSE;
//	}
//	else if (nS0 != nS1)
//	{
//		nSerial = -1;
//		return FALSE;
//	}
//
//	nSerial = nS0;
//	return TRUE;
//}
//
//BOOL CGvisR2R_PunchView::ChkVsShareUp(int &nSerial)
//{
//	CFileFind cFile;
//	BOOL bExist = cFile.FindFile(pDoc->WorkingInfo.System.sPathVsShareUp + _T("*.pcr"));
//	if (!bExist)
//		return FALSE; // pcr파일이 존재하지 않음.
//
//	int nPos;
//	CString sFileName, sSerial;
//	while (bExist)
//	{
//		bExist = cFile.FindNextFile();
//		if (cFile.IsDots()) continue;
//		if (!cFile.IsDirectory())
//		{
//			// 파일명을 얻음.
//			sFileName = cFile.GetFileName();
//			nPos = sFileName.ReverseFind('.');
//			if (nPos > 0)
//				sSerial = sFileName.Left(nPos);
//
//			nSerial = _tstoi(sSerial);
//			if (nSerial > 0)
//				return TRUE;
//			else
//			{
//				nSerial = 0;
//				return FALSE;
//			}
//		}
//	}
//
//	return FALSE;
//}
//
//BOOL CGvisR2R_PunchView::ChkVsShareDn(int &nSerial)
//{
//	CFileFind cFile;
//	BOOL bExist = cFile.FindFile(pDoc->WorkingInfo.System.sPathVsShareDn + _T("*.pcr"));
//	if (!bExist)
//		return FALSE; // pcr파일이 존재하지 않음.
//
//	int nPos;
//	CString sFileName, sSerial;
//	while (bExist)
//	{
//		bExist = cFile.FindNextFile();
//		if (cFile.IsDots()) continue;
//		if (!cFile.IsDirectory())
//		{
//			// 파일명을 얻음.
//			sFileName = cFile.GetFileName();
//			nPos = sFileName.ReverseFind('.');
//			if (nPos > 0)
//				sSerial = sFileName.Left(nPos);
//
//			nSerial = _tstoi(sSerial);
//			if (nSerial > 0)
//				return TRUE;
//			else
//			{
//				nSerial = 0;
//				return FALSE;
//			}
//		}
//	}
//
//	return FALSE;
//}
//
//BOOL CGvisR2R_PunchView::ChkShare(int &nSerial)
//{
//	int nS0, nS1;
//	BOOL b0 = ChkShareUp(nS0);
//	BOOL b1 = ChkShareDn(nS1);
//
//	if (!b0 || !b1)
//	{
//		nSerial = -1;
//		return FALSE;
//	}
//	else if (nS0 != nS1)
//	{
//		nSerial = -1;
//		return FALSE;
//	}
//
//	nSerial = nS0;
//	return TRUE;
//}
//
//BOOL CGvisR2R_PunchView::ChkShareUp(int &nSerial)
//{
//	CFileFind cFile;
//	BOOL bExist = cFile.FindFile(pDoc->WorkingInfo.System.sPathVrsShareUp + _T("*.pcr"));
//	if (!bExist)
//		return FALSE; // pcr파일이 존재하지 않음.
//
//	int nPos;
//	CString sFileName, sSerial;
//	while (bExist)
//	{
//		bExist = cFile.FindNextFile();
//		if (cFile.IsDots()) continue;
//		if (!cFile.IsDirectory())
//		{
//			// 파일명을 얻음.
//			sFileName = cFile.GetFileName();
//			nPos = sFileName.ReverseFind('.');
//			if (nPos > 0)
//				sSerial = sFileName.Left(nPos);
//
//			nSerial = _tstoi(sSerial);
//			if (nSerial > 0)
//				return TRUE;
//			else
//			{
//				nSerial = 0;
//				return FALSE;
//			}
//		}
//	}
//
//	return FALSE;
//}
//
//BOOL CGvisR2R_PunchView::ChkShareDn(int &nSerial)
//{
//	CFileFind cFile;
//	BOOL bExist = cFile.FindFile(pDoc->WorkingInfo.System.sPathVrsShareDn + _T("*.pcr"));
//	if (!bExist)
//		return FALSE; // pcr파일이 존재하지 않음.
//
//	int nPos;
//	CString sFileName, sSerial;
//	while (bExist)
//	{
//		bExist = cFile.FindNextFile();
//		if (cFile.IsDots()) continue;
//		if (!cFile.IsDirectory())
//		{
//			// 파일명을 얻음.
//			sFileName = cFile.GetFileName();
//			nPos = sFileName.ReverseFind('.');
//			if (nPos > 0)
//				sSerial = sFileName.Left(nPos);
//
//			nSerial = _tstoi(sSerial);
//			if (nSerial > 0)
//				return TRUE;
//			else
//			{
//				nSerial = 0;
//				return FALSE;
//			}
//		}
//	}
//
//	return FALSE;
//}

//void CGvisR2R_PunchView::ChkShare()
//{
//	ChkShareUp();
//	ChkShareDn();
//}
//
//void CGvisR2R_PunchView::ChkShareUp()
//{
//	CString str;
//	int nSerial;
//	if (ChkShareUp(nSerial))
//	{
//		str.Format(_T("US: %d"), nSerial);
//		pDoc->Status.PcrShare[0].bExist = TRUE;
//		pDoc->Status.PcrShare[0].nSerial = nSerial;
//		if (m_pMpe)
//		{
//			pView->m_pMpe->Write(_T("ML45112"), (long)nSerial);	// 검사한 Panel의 AOI 상 Serial
//			m_pMpe->Write(_T("MB44012B"), 1); // AOI 상 : PCR파일 Received
//		}
//	}
//	else
//	{
//		pDoc->Status.PcrShare[0].bExist = FALSE;
//		pDoc->Status.PcrShare[0].nSerial = -1;
//		str.Format(_T("US: "));
//	}
//	if (pFrm)
//	{
//		if (pView->m_mgrProcedure->m_sShare[0] != str)
//		{
//			pView->m_mgrProcedure->m_sShare[0] = str;
//			pFrm->DispStatusBar(str, 4);
//		}
//	}
//}
//
//void CGvisR2R_PunchView::ChkShareDn()
//{
//	CString str;
//	int nSerial;
//	if (ChkShareDn(nSerial))
//	{
//		str.Format(_T("DS: %d"), nSerial);
//		pDoc->Status.PcrShare[1].bExist = TRUE;
//		pDoc->Status.PcrShare[1].nSerial = nSerial;
//		if (m_pMpe)
//		{
//			pView->m_pMpe->Write(_T("ML45114"), (long)nSerial);	// 검사한 Panel의 AOI 하 Serial
//			m_pMpe->Write(_T("MB44012C"), 1); // AOI 하 : PCR파일 Received
//		}
//	}
//	else
//	{
//		pDoc->Status.PcrShare[1].bExist = FALSE;
//		pDoc->Status.PcrShare[1].nSerial = -1;
//		str.Format(_T("DS: "));
//	}
//	if (pFrm)
//	{
//		if (pView->m_mgrProcedure->m_sShare[1] != str)
//		{
//			pView->m_mgrProcedure->m_sShare[1] = str;
//			pFrm->DispStatusBar(str, 2);
//		}
//	}
//}
//
//BOOL CGvisR2R_PunchView::ChkBufIdx(int* pSerial, int nTot)
//{
//	if (nTot < 2)
//		return TRUE;
//
//	for (int i = 0; i<(nTot - 1); i++)
//	{
//		if (pSerial[i + 1] != pSerial[i] + 1)
//			return FALSE;
//	}
//	return TRUE;
//}
//
//void CGvisR2R_PunchView::SwapUp(__int64 *num1, __int64 *num2) 	// 위치 바꾸는 함수
//{
//	__int64 temp;
//
//	temp = *num2;
//	*num2 = *num1;
//	*num1 = temp;
//}
//
//BOOL CGvisR2R_PunchView::SortingInUp(CString sPath, int nIndex)
//{
//	struct _stat buf;
//	struct tm *t;
//
//	CString sMsg, sFileName, sSerial;
//	int nPos, nSerial;
//	char filename[MAX_PATH];
//	StringToChar(sPath, filename);
//
//	if (_stat(filename, &buf) != 0)
//	{
//		sMsg.Format(_T("일시정지 - Failed getting information."));
//		//MsgBox(sMsg);
//		pView->ClrDispMsg();
//		AfxMessageBox(sMsg);
//		return FALSE;
//	}
//	else
//	{
//		sFileName = sPath;
//		nPos = sFileName.ReverseFind('.');
//		if (nPos > 0)
//		{
//			sSerial = sFileName.Left(nPos);
//			sSerial = sSerial.Right(4);
//		}
//
//		nSerial = _tstoi(sSerial);
//
//		t = localtime(&buf.st_mtime);
//		//sFileName.Format(_T("%04d%02d%02d%02d%02d%02d%04d"), t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, 
//		//														t->tm_hour, t->tm_min, t->tm_sec, nSerial);
//		//m_nBufSerialSorting[0][nIndex] = _tstoi(sFileName);
//
//		CString sYear, sMonth, sDay, sHour, sMin, sSec;
//		sYear.Format(_T("%04d"), t->tm_year + 1900);
//		sMonth.Format(_T("%02d"), t->tm_mon + 1);
//		sDay.Format(_T("%02d"), t->tm_mday);
//		sHour.Format(_T("%02d"), t->tm_hour);
//		sMin.Format(_T("%02d"), t->tm_min);
//		sSec.Format(_T("%02d"), t->tm_sec);
//
//		__int64 nYear = _tstoi(sYear);
//		__int64 nMonth = _tstoi(sMonth);
//		__int64 nDay = _tstoi(sDay);
//		__int64 nHour = _tstoi(sHour);
//		__int64 nMin = _tstoi(sMin);
//		__int64 nSec = _tstoi(sSec);
//
//		pView->m_mgrProcedure->m_nBufSerialSorting[0][nIndex] = nYear * 100000000000000 + nMonth * 1000000000000 + nDay * 10000000000 +
//			nHour * 100000000 + nMin * 1000000 + nSec * 10000 + nSerial;
//	}
//
//	return TRUE;
//}
//
//BOOL CGvisR2R_PunchView::SortingOutUp(int* pSerial, int nTot)
//{
//	int i, k;
//
//	for (k = 0; k < nTot; k++) 			// 버블 정렬 소스 시작
//	{
//		for (i = 0; i < (nTot - 1) - k; i++)
//		{
//
//			if (pView->m_mgrProcedure->m_nBufSerialSorting[0][i] > pView->m_mgrProcedure->m_nBufSerialSorting[0][i + 1])
//			{
//				SwapUp(&pView->m_mgrProcedure->m_nBufSerialSorting[0][i + 1], &pView->m_mgrProcedure->m_nBufSerialSorting[0][i]);
//			}
//		}
//	}									// 버블 정렬 소스 끝
//
//	for (i = 0; i < nTot; i++)
//	{
//		pSerial[i] = (int)(pView->m_mgrProcedure->m_nBufSerialSorting[0][i] % 10000);
//	}
//	return TRUE;
//}
//
//BOOL CGvisR2R_PunchView::ChkBufUp(int* pSerial, int &nTot)
//{
//	CFileFind cFile;
//	BOOL bExist = cFile.FindFile(pDoc->WorkingInfo.System.sPathVrsBufUp + _T("*.pcr"));
//	if (!bExist)
//	{
//		pView->m_mgrProcedure->m_bBufEmpty[0] = TRUE;
//		if (!pView->m_mgrProcedure->m_bBufEmptyF[0])
//			pView->m_mgrProcedure->m_bBufEmptyF[0] = TRUE;		// 최초 한번 버퍼가 비어있으면(초기화를 하고 난 이후) TRUE.
//
//		return FALSE; // pcr파일이 존재하지 않음.
//	}
//
//	int nPos, nSerial;
//
//	CString sFileName, sSerial;
//	CString sNewName;
//
//	nTot = 0;
//	while (bExist)
//	{
//		bExist = cFile.FindNextFile();
//		if (cFile.IsDots()) continue;
//		if (!cFile.IsDirectory())
//		{
//			sFileName = cFile.GetFileName();
//
//			if (!SortingInUp(pDoc->WorkingInfo.System.sPathVrsBufUp + sFileName, nTot))
//				return FALSE;
//
//			nTot++;
//		}
//	}
//
//	BOOL bRtn = SortingOutUp(pSerial, nTot);
//
//	if (nTot == 0)
//		pView->m_mgrProcedure->m_bBufEmpty[0] = TRUE;
//	else
//	{
//		pView->m_mgrProcedure->m_bBufEmpty[0] = FALSE;
//		pView->m_mgrProcedure->m_bIsBuf[0] = TRUE;
//	}
//
//	return (bRtn);
//}
//
//BOOL CGvisR2R_PunchView::ChkBufDn(int* pSerial, int &nTot)
//{
//	CFileFind cFile;
//	BOOL bExist = cFile.FindFile(pDoc->WorkingInfo.System.sPathVrsBufDn + _T("*.pcr"));
//	if (!bExist)
//	{
//		pView->m_mgrProcedure->m_bBufEmpty[1] = TRUE;
//		if (!pView->m_mgrProcedure->m_bBufEmptyF[1])
//			pView->m_mgrProcedure->m_bBufEmptyF[1] = TRUE;
//		return FALSE; // pcr파일이 존재하지 않음.
//	}
//
//	int nPos, nSerial;
//
//	CString sFileName, sSerial;
//	nTot = 0;
//	while (bExist)
//	{
//		bExist = cFile.FindNextFile();
//		if (cFile.IsDots()) continue;
//		if (!cFile.IsDirectory())
//		{
//			sFileName = cFile.GetFileName();
//			//nPos = sFileName.ReverseFind('.');
//			//if (nPos > 0)
//			//	sSerial = sFileName.Left(nPos);
//
//			//nSerial = _tstoi(sSerial);
//			//if (nSerial > 0)
//			//{
//			//	pSerial[nTot] = nSerial;
//			//	nTot++;
//			//}
//
//			if (!SortingInDn(pDoc->WorkingInfo.System.sPathVrsBufDn + sFileName, nTot))
//				return FALSE;
//
//			nTot++;
//		}
//	}
//
//
//	BOOL bRtn = SortingOutDn(pSerial, nTot);
//
//	if (nTot == 0)
//		pView->m_mgrProcedure->m_bBufEmpty[1] = TRUE;
//	else
//	{
//		pView->m_mgrProcedure->m_bBufEmpty[1] = FALSE;
//		pView->m_mgrProcedure->m_bIsBuf[1] = TRUE;
//	}
//
//	return (bRtn);
//	//return TRUE;
//}
//
//void CGvisR2R_PunchView::SwapDn(__int64 *num1, __int64 *num2) 	// 위치 바꾸는 함수
//{
//	__int64 temp;
//
//	temp = *num2;
//	*num2 = *num1;
//	*num1 = temp;
//}
//
//BOOL CGvisR2R_PunchView::SortingInDn(CString sPath, int nIndex)
//{
//	struct _stat buf;
//	struct tm *t;
//
//	CString sMsg, sFileName, sSerial;
//	int nPos, nSerial;
//	char filename[MAX_PATH];
//	StringToChar(sPath, filename);
//
//	if (_stat(filename, &buf) != 0)
//	{
//		sMsg.Format(_T("일시정지 - Failed getting information."));
//		//MsgBox(sMsg);
//		pView->ClrDispMsg();
//		AfxMessageBox(sMsg);
//		return FALSE;
//	}
//	else
//	{
//		sFileName = sPath;
//		nPos = sFileName.ReverseFind('.');
//		if (nPos > 0)
//		{
//			sSerial = sFileName.Left(nPos);
//			sSerial = sSerial.Right(4);
//		}
//
//		nSerial = _tstoi(sSerial);
//
//		t = localtime(&buf.st_mtime);
//		//sFileName.Format(_T("%04d%02d%02d%02d%02d%02d%04d"), t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, 
//		//														t->tm_hour, t->tm_min, t->tm_sec, nSerial);
//		//m_nBufSerialSorting[1][nIndex] = _tstoi(sFileName);
//
//
//		CString sYear, sMonth, sDay, sHour, sMin, sSec;
//		sYear.Format(_T("%04d"), t->tm_year + 1900);
//		sMonth.Format(_T("%02d"), t->tm_mon + 1);
//		sDay.Format(_T("%02d"), t->tm_mday);
//		sHour.Format(_T("%02d"), t->tm_hour);
//		sMin.Format(_T("%02d"), t->tm_min);
//		sSec.Format(_T("%02d"), t->tm_sec);
//
//		__int64 nYear = _tstoi(sYear);
//		__int64 nMonth = _tstoi(sMonth);
//		__int64 nDay = _tstoi(sDay);
//		__int64 nHour = _tstoi(sHour);
//		__int64 nMin = _tstoi(sMin);
//		__int64 nSec = _tstoi(sSec);
//
//		pView->m_mgrProcedure->m_nBufSerialSorting[1][nIndex] = nYear * 100000000000000 + nMonth * 1000000000000 + nDay * 10000000000 +
//			nHour * 100000000 + nMin * 1000000 + nSec * 10000 + nSerial;
//	}
//
//	return TRUE;
//}
//
//BOOL CGvisR2R_PunchView::SortingOutDn(int* pSerial, int nTot)
//{
//	int i, k;
//
//	for (k = 0; k < nTot; k++) 			// 버블 정렬 소스 시작
//	{
//		for (i = 0; i < (nTot - 1) - k; i++)
//		{
//
//			if (pView->m_mgrProcedure->m_nBufSerialSorting[1][i] > pView->m_mgrProcedure->m_nBufSerialSorting[1][i + 1])
//			{
//				SwapUp(&m_nBufSerialSorting[1][i + 1], &m_nBufSerialSorting[1][i]);
//			}
//		}
//	}									// 버블 정렬 소스 끝
//
//	for (i = 0; i < nTot; i++)
//	{
//		pSerial[i] = (int)(pView->m_mgrProcedure->m_nBufSerialSorting[1][i] % 10000);
//	}
//	return TRUE;
//}

BOOL CGvisR2R_PunchView::ChkSaftySen() // 감지 : TRUE , 비감지 : FALSE
{
	if (pDoc->WorkingInfo.LastJob.bMkSftySen)
	{
		if (pDoc->Status.bSensSaftyMk && !pDoc->Status.bSensSaftyMkF)
		{
			pDoc->Status.bSensSaftyMkF = TRUE;
			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
			pView->m_mgrProcedure->m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("정 지"), RGB_RED);			
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
			pView->m_mgrProcedure->m_bTIM_SAFTY_STOP = TRUE;//MsgBox(_T("일시정지 - 마킹부 안전센서가 감지되었습니다."));
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

void CGvisR2R_PunchView::ResetMotion()
{
	for (int i = 0; i<MAX_MS; i++)
	{
		ResetMotion(i);

		if (i<MAX_AXIS)
		{
			while (!m_pMotion->IsServoOn(i))
			{
				if (i == MS_X0 || i == MS_Y0)
					pView->m_pMotion->Clear(MS_X0Y0);
				else if (i == MS_X1 || i == MS_Y1)
					pView->m_pMotion->Clear(MS_X1Y1);
				else
					pView->m_pMotion->Clear(i);
				Sleep(30);
				m_pMotion->ServoOnOff(i, TRUE);
				Sleep(30);
			}
		}
	}
}

void CGvisR2R_PunchView::ResetMotion(int nMsId)
{
	if (m_pDlgMenu02)
		m_pDlgMenu02->ResetMotion(nMsId);
}

unsigned long CGvisR2R_PunchView::ChkDoor() // 0: All Closed , Open Door Index : Doesn't all closed. (Bit3: F, Bit2: L, Bit1: R, Bit0; B)
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
			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
			pView->m_mgrProcedure->m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 검사부 상 전면 중앙 도어 Open"));
			TowerLamp(RGB_RED, TRUE);
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
			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
			pView->m_mgrProcedure->m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 검사부 상 전면 좌측 도어 Open"));
			TowerLamp(RGB_RED, TRUE);
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
			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
			pView->m_mgrProcedure->m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 검사부 상 전면 우측 도어 Open"));
			TowerLamp(RGB_RED, TRUE);
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
			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
			pView->m_mgrProcedure->m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 검사부 상 후면 중앙 도어 Open"));
			TowerLamp(RGB_RED, TRUE);
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
			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
			pView->m_mgrProcedure->m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 검사부 상 후면 좌측 도어 Open"));
			TowerLamp(RGB_RED, TRUE);
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
			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
			pView->m_mgrProcedure->m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 검사부 상 후면 우측 도어 Open"));
			TowerLamp(RGB_RED, TRUE);
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
			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
			pView->m_mgrProcedure->m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 검사부 하 전면 중앙 도어 Open"));
			TowerLamp(RGB_RED, TRUE);
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
			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
			pView->m_mgrProcedure->m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 검사부 하 전면 좌측 도어 Open"));
			TowerLamp(RGB_RED, TRUE);
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
			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
			pView->m_mgrProcedure->m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 검사부 하 전면 우측 도어 Open"));
			TowerLamp(RGB_RED, TRUE);
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
			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
			pView->m_mgrProcedure->m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 검사부 하 후면 중앙 도어 Open"));
			TowerLamp(RGB_RED, TRUE);
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
			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
			pView->m_mgrProcedure->m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 검사부 하 후면 좌측 도어 Open"));
			TowerLamp(RGB_RED, TRUE);
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
			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
			pView->m_mgrProcedure->m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 검사부 하 후면 우측 도어 Open"));
			TowerLamp(RGB_RED, TRUE);
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
			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
			pView->m_mgrProcedure->m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 마킹부 전면 좌측 도어 Open"));
			TowerLamp(RGB_RED, TRUE);
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
			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
			pView->m_mgrProcedure->m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 마킹부 전면 우측 도어 Open"));
			TowerLamp(RGB_RED, TRUE);
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
			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
			pView->m_mgrProcedure->m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 마킹부 후면 좌측 도어 Open"));
			TowerLamp(RGB_RED, TRUE);
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
			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
			pView->m_mgrProcedure->m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 마킹부 후면 우측 도어 Open"));
			TowerLamp(RGB_RED, TRUE);
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
			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
			pView->m_mgrProcedure->m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 각인부 전면 좌측 도어 Open"));
			TowerLamp(RGB_RED, TRUE);
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
			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
			pView->m_mgrProcedure->m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 각인부 전면 우측 도어 Open"));
			TowerLamp(RGB_RED, TRUE);
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
			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
			pView->m_mgrProcedure->m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 각인부 후면 좌측 도어 Open"));
			TowerLamp(RGB_RED, TRUE);
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
			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
			pView->m_mgrProcedure->m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 각인부 후면 우측 도어 Open"));
			TowerLamp(RGB_RED, TRUE);
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
			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
			pView->m_mgrProcedure->m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 언코일러부 전면 좌측 도어 Open"));
			TowerLamp(RGB_RED, TRUE);
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
			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
			pView->m_mgrProcedure->m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 언코일러부 측면 도어 Open"));
			TowerLamp(RGB_RED, TRUE);
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
			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
			pView->m_mgrProcedure->m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 언코일러부 후면 좌측 도어 Open"));
			TowerLamp(RGB_RED, TRUE);
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
			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
			pView->m_mgrProcedure->m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 언코일러부 후면 우측 도어 Open"));
			TowerLamp(RGB_RED, TRUE);
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
			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
			pView->m_mgrProcedure->m_bSwRunF = FALSE;
			Stop();
			pView->DispStsBar(_T("정지-19"), 0);
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 리코일러부 전면 우측 도어 Open"));
			TowerLamp(RGB_RED, TRUE);
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
			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
			pView->m_mgrProcedure->m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 리코일러부 측면 도어 Open"));
			TowerLamp(RGB_RED, TRUE);
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
			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
			pView->m_mgrProcedure->m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 리코일러부 후면 좌측 도어 Open"));
			TowerLamp(RGB_RED, TRUE);
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
			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
			pView->m_mgrProcedure->m_bSwRunF = FALSE;
			Stop();
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(_T("일시정지 - 리코일러부 후면 우측 도어 Open"));
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}
	}

	return ulOpenDoor;
}

void CGvisR2R_PunchView::ChkEmg()
{
	if (pDoc->Status.bEmgAoi[EMG_F_AOI_UP] && !pDoc->Status.bEmgAoiF[EMG_F_AOI_UP])
	{
		pDoc->Status.bEmgAoiF[EMG_F_AOI_UP] = TRUE;
		pView->m_mgrProcedure->m_bSwStopNow = TRUE;
		pView->m_mgrProcedure->m_bSwRunF = FALSE;
		Stop();
		DispMain(_T("정 지"), RGB_RED);
		MsgBox(_T("비상정지 - 검사부 상 전면 스위치"));
		TowerLamp(RGB_RED, TRUE);
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
		pView->m_mgrProcedure->m_bSwStopNow = TRUE;
		pView->m_mgrProcedure->m_bSwRunF = FALSE;
		Stop();
		DispMain(_T("정 지"), RGB_RED);
		MsgBox(_T("비상정지 - 검사부 상 후면 스위치"));
		TowerLamp(RGB_RED, TRUE);
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
		pView->m_mgrProcedure->m_bSwStopNow = TRUE;
		pView->m_mgrProcedure->m_bSwRunF = FALSE;
		Stop();
		DispMain(_T("정 지"), RGB_RED);
		MsgBox(_T("비상정지 - 검사부 하 전면 스위치"));
		TowerLamp(RGB_RED, TRUE);
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
		pView->m_mgrProcedure->m_bSwStopNow = TRUE;
		pView->m_mgrProcedure->m_bSwRunF = FALSE;
		Stop();
		DispMain(_T("정 지"), RGB_RED);
		MsgBox(_T("비상정지 - 검사부 하 후면 스위치"));
		TowerLamp(RGB_RED, TRUE);
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
		pView->m_mgrProcedure->m_bSwStopNow = TRUE;
		pView->m_mgrProcedure->m_bSwRunF = FALSE;
		Stop();
		DispMain(_T("정 지"), RGB_RED);
		MsgBox(_T("비상정지 - 마킹부 메인 스위치"));
		TowerLamp(RGB_RED, TRUE);
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
		pView->m_mgrProcedure->m_bSwStopNow = TRUE;
		pView->m_mgrProcedure->m_bSwRunF = FALSE;
		Stop();
		DispMain(_T("정 지"), RGB_RED);
		MsgBox(_T("비상정지 - 마킹부 스위치"));
		TowerLamp(RGB_RED, TRUE);
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
		pView->m_mgrProcedure->m_bSwStopNow = TRUE;
		pView->m_mgrProcedure->m_bSwRunF = FALSE;
		Stop();
		DispMain(_T("정 지"), RGB_RED);
		MsgBox(_T("비상정지 - 언코일러부 스위치"));
		TowerLamp(RGB_RED, TRUE);
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
		pView->m_mgrProcedure->m_bSwStopNow = TRUE;
		pView->m_mgrProcedure->m_bSwRunF = FALSE;
		Stop();
		DispMain(_T("정 지"), RGB_RED);
		MsgBox(_T("비상정지 - 리코일러부 스위치"));
		TowerLamp(RGB_RED, TRUE);
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
		pView->m_mgrProcedure->m_bSwStopNow = TRUE;
		pView->m_mgrProcedure->m_bSwRunF = FALSE;
		Stop();
		DispMain(_T("정 지"), RGB_RED);
		MsgBox(_T("비상정지 - 각인부 모니터"));
		TowerLamp(RGB_RED, TRUE);
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
		pView->m_mgrProcedure->m_bSwStopNow = TRUE;
		pView->m_mgrProcedure->m_bSwRunF = FALSE;
		Stop();
		DispMain(_T("정 지"), RGB_RED);
		MsgBox(_T("비상정지 - 각인부 스위치"));
		TowerLamp(RGB_RED, TRUE);
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

int CGvisR2R_PunchView::ChkSerial() // // 0: Continue, -: Previous, +: Discontinue --> 0: Same Serial, -: Decrese Serial, +: Increase Serial
{
	int nSerial0 = GetBuffer0(); // 첫번째 버퍼 시리얼 : 상하 시리얼이 다르면 0
	int nSerial1 = GetBuffer1(); // 두번째 버퍼 시리얼 : 상하 시리얼이 다르면 0
	int nLastShot = pDoc->GetLastShotMk();

	// Last shot 다음 연속 시리얼 체크
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

//void CGvisR2R_PunchView::ChkBuf()
//{
//	if (!pView->m_mgrProcedure->m_bShift2Mk)
//		ChkBufUp();
//	if (!pView->m_mgrProcedure->m_bShift2Mk)
//		ChkBufDn();
//}

//void CGvisR2R_PunchView::ChkBufUp()
//{
//	CString str, sTemp;
//
//	str = _T("UB: ");
//	if (ChkBufUp(pView->m_mgrProcedure->m_pBufSerial[0], pView->m_mgrProcedure->m_nBufTot[0]))
//	{
//		for (int i = 0; i<pView->m_mgrProcedure->m_nBufTot[0]; i++)
//		{
//			if (pView->m_mgrProcedure->m_bShift2Mk)
//				return;
//
//			DelOverLotEndSerialUp(pView->m_mgrProcedure->m_pBufSerial[0][i]);
//
//			if (i == m_nBufTot[0] - 1)
//				sTemp.Format(_T("%d"), m_pBufSerial[0][i]);
//			else
//				sTemp.Format(_T("%d,"), m_pBufSerial[0][i]);
//			str += sTemp;
//		}
//	}
//	else
//	{
//		pView->m_mgrProcedure->m_nBufTot[0] = 0;
//	}
//
//	if (pFrm)
//	{
//		if (pView->m_mgrProcedure->m_sBuf[0] != str)
//		{
//			m_sBuf[0] = str;
//			pFrm->DispStatusBar(str, 3);
//
//			pDoc->SetCurrentInfoBufUpTot(pView->m_mgrProcedure->m_nBufTot[0]);
//			for (int k = 0; k<m_nBufTot[0]; k++)
//				pDoc->SetCurrentInfoBufUp(k, pView->m_mgrProcedure->m_pBufSerial[0][k]);
//		}
//	}
//}
//
//void CGvisR2R_PunchView::ChkBufDn()
//{
//	CString str, sTemp;
//
//	str = _T("DB: ");
//	if (ChkBufDn(pView->m_mgrProcedure->m_pBufSerial[1], pView->m_mgrProcedure->m_nBufTot[1]))
//	{
//		for (int i = 0; i<pView->m_mgrProcedure->m_nBufTot[1]; i++)
//		{
//			if (pView->m_mgrProcedure->m_bShift2Mk)
//				return;
//
//			DelOverLotEndSerialDn(pView->m_mgrProcedure->m_pBufSerial[1][i]);
//
//			if (i == m_nBufTot[1] - 1)
//				sTemp.Format(_T("%d"), m_pBufSerial[1][i]);
//			else
//				sTemp.Format(_T("%d,"), m_pBufSerial[1][i]);
//			str += sTemp;
//		}
//	}
//	else
//	{
//		pView->m_mgrProcedure->m_nBufTot[1] = 0;
//	}
//
//	if (pFrm)
//	{
//		if (pView->m_mgrProcedure->m_sBuf[1] != str)
//		{
//			m_sBuf[1] = str;
//			pFrm->DispStatusBar(str, 1);
//
//			pDoc->SetCurrentInfoBufDnTot(pView->m_mgrProcedure->m_nBufTot[1]);
//			for (int k = 0; k < m_nBufTot[1]; k++)
//				pDoc->SetCurrentInfoBufDn(k, pView->m_mgrProcedure->m_pBufSerial[1][k]);
//		}
//	}
//}

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

CString CGvisR2R_PunchView::GetAoiUpAlarmRestartMsg()
{
	CString sMsg = _T("Not Find Message.");
	TCHAR szData[512];
	CString sPath = PATH_ALARM;
	if (0 < ::GetPrivateProfileString(_T("11"), _T("27"), NULL, szData, sizeof(szData), sPath))
		sMsg = CString(szData);

	return sMsg;
}

CString CGvisR2R_PunchView::GetAoiDnAlarmRestartMsg()
{
	CString sMsg = _T("Not Find Message.");
	TCHAR szData[512];
	CString sPath = PATH_ALARM;
	if (0 < ::GetPrivateProfileString(_T("11"), _T("28"), NULL, szData, sizeof(szData), sPath))
		sMsg = CString(szData);

	return sMsg;
}

void CGvisR2R_PunchView::DoIO()
{
	DoEmgSens();	//20220607
	DoSaftySens();	//20220603
	DoDoorSens();	//20220607

	DoModeSel();
	DoMainSw();
	DoEngraveSens();

	DoInterlock();

	MonPlcAlm();
	MonDispMain();
	MonPlcSignal();

	if (pView->m_mgrProcedure->m_bCycleStop)
	{
		pView->m_mgrProcedure->m_bCycleStop = FALSE;
		TowerLamp(RGB_YELLOW, TRUE);
		Buzzer(TRUE);
		if (!pDoc->m_sAlmMsg.IsEmpty())
		{
			MsgBox(pDoc->m_sAlmMsg, 0, 0, DEFAULT_TIME_OUT, FALSE);

			if (pDoc->m_sAlmMsg == GetAoiUpAlarmRestartMsg())
			{
				ChkReTestAlarmOnAoiUp();
			}
			else if(pDoc->m_sAlmMsg == GetAoiDnAlarmRestartMsg())
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

	if (pView->m_mgrProcedure->IsRun())
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

void CGvisR2R_PunchView::GetEnc()
{
	if (!m_pMotion)
		return;

	pView->m_mgrProcedure->m_dEnc[AXIS_X0] = m_pMotion->GetActualPosition(AXIS_X0);
	pView->m_mgrProcedure->m_dEnc[AXIS_Y0] = m_pMotion->GetActualPosition(AXIS_Y0);
	pView->m_mgrProcedure->m_dEnc[AXIS_X1] = m_pMotion->GetActualPosition(AXIS_X1);
	pView->m_mgrProcedure->m_dEnc[AXIS_Y1] = m_pMotion->GetActualPosition(AXIS_Y1);
}

void CGvisR2R_PunchView::ChkMRegOut()
{
	pView->m_mgrProcedure->m_bChkMpeIoOut = TRUE;
}

void CGvisR2R_PunchView::DoSens()
{
}

void CGvisR2R_PunchView::DoBoxSw()
{
}

void CGvisR2R_PunchView::DoEmgSw()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)
		return;
#endif
}

void CGvisR2R_PunchView::GetMpeIO()
{
#ifdef USE_MPE
	if (!m_pMpe || !pDoc->m_pMpeIo)
		return;

	m_pMpe->GetMpeIO();
#endif
}

void CGvisR2R_PunchView::GetMpeSignal()
{
#ifdef USE_MPE
	if (!m_pMpe || !pDoc->m_pMpeSignal)
		return;
	m_pMpe->GetMpeSignal();
#endif
}


void CGvisR2R_PunchView::GetMpeData()
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
	for (nIdx = 0; nIdx<nInSeg; nIdx++)
	{
		pData0[nIdx] = new long[16];
		memset(pData0[nIdx], 0, sizeof(long) * 16);
	}

	// Group Read...
	int nGrpStep = pDoc->MkIo.MpeData.nGrpStep;
	nSt = pDoc->MkIo.MpeData.nGrpInSt;
	for (nLoop = 0; nLoop<pDoc->MkIo.MpeData.nGrpIn; nLoop++)
	{
		for (nIdx = 0; nIdx<nGrpStep; nIdx++)
			pSeg0[nIdx] = nIdx + nSt;
		nSize = m_pMpe->ReadData(pSeg0, nGrpStep, pData0);

		for (nIdx = 0; nIdx<nGrpStep; nIdx++)
		{
			for (k = 0; k<16; k++)
				pDoc->m_pMpeData[nIdx + nSt][k] = pData0[nIdx][k];
		}

		nSt += nGrpStep;
	}
	for (nIdx = 0; nIdx<nInSeg; nIdx++)
		delete[] pData0[nIdx];
	delete[] pData0;
	delete[] pSeg0;

	pView->m_mgrProcedure->m_bChkMpeIoOut = FALSE;

	// MpeData - [Out]
	int *pSeg2 = new int[nOutSeg];
	long **pData2 = new long*[nOutSeg];
	for (nIdx = 0; nIdx<nOutSeg; nIdx++)
	{
		pData2[nIdx] = new long[16];
		memset(pData2[nIdx], 0, sizeof(unsigned long) * 16);
	}

	// Group Read...
	nSt = pDoc->MkIo.MpeData.nGrpOutSt;
	for (nLoop = 0; nLoop<pDoc->MkIo.MpeData.nGrpOut; nLoop++)
	{
		for (nIdx = 0; nIdx<nGrpStep; nIdx++)
			pSeg2[nIdx] = nIdx + nSt;
		nSize = m_pMpe->ReadData(pSeg2, nGrpStep, pData2);

		for (nIdx = 0; nIdx<nGrpStep; nIdx++)
		{
			for (k = 0; k<16; k++)
				pDoc->m_pMpeData[nIdx + nSt][k] = pData2[nIdx][k];
		}

		nSt += nGrpStep;
	}
	for (nIdx = 0; nIdx<nOutSeg; nIdx++)
		delete[] pData2[nIdx];
	delete[] pData2;
	delete[] pSeg2;
#endif
}

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
	if (pView->m_mgrProcedure->m_sDispTime != str)
	{
		pView->m_mgrProcedure->m_sDispTime = str;
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
void CGvisR2R_PunchView::DoSaftySens()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIb || !pDoc->m_pMpeIF)
		return;

	if (!pView->m_mgrProcedure->IsRun())
		return;

	unsigned short usIn = pDoc->m_pMpeIb[7];
	unsigned short *usInF = &pDoc->m_pMpeIF[7];

	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))		// 마킹부 안전 센서
	{
		*usInF |= (0x01 << 8);								
		pDoc->Status.bSensSaftyMk = TRUE;
	}
	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))	// 마킹부 안전 센서
	{
		*usInF &= ~(0x01 << 8);
		pDoc->Status.bSensSaftyMk = FALSE;
	}
#endif
}

void CGvisR2R_PunchView::DoDoorSens()
{
#ifdef USE_MPE
	unsigned short usIn;
	unsigned short *usInF;

	if (!pDoc->m_pMpeIb || !pDoc->m_pMpeIF)
		return;

	if (!pView->m_mgrProcedure->IsRun())
		return;

	usIn = pDoc->m_pMpeIb[1];
	usInF = &pDoc->m_pMpeIF[1];

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))		// 언코일러 전면 도어 센서
	{
		*usInF |= (0x01 << 12);
		pDoc->Status.bDoorUc[DOOR_FL_UC] = TRUE;
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))	// 언코일러 전면 도어 센서
	{
		*usInF &= ~(0x01 << 12);
		pDoc->Status.bDoorUc[DOOR_FL_UC] = FALSE;
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))		// 언코일러 측면 도어 센서
	{
		*usInF |= (0x01 << 13);
		pDoc->Status.bDoorUc[DOOR_FR_UC] = TRUE;
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))	// 언코일러 측면 도어 센서
	{
		*usInF &= ~(0x01 << 13);
		pDoc->Status.bDoorUc[DOOR_FR_UC] = FALSE;
	}

	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))		// 언코일러 후면 도어 센서(좌)
	{
		*usInF |= (0x01 << 14);
		pDoc->Status.bDoorUc[DOOR_BL_UC] = TRUE;
	}
	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))	// 언코일러 후면 도어 센서(좌)
	{
		*usInF &= ~(0x01 << 14);
		pDoc->Status.bDoorUc[DOOR_BL_UC] = FALSE;
	}

	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))		// 언코일러 후면 도어 센서(우)
	{
		*usInF |= (0x01 << 15);
		pDoc->Status.bDoorUc[DOOR_BR_UC] = TRUE;
	}
	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))	// 언코일러 후면 도어 센서(우)
	{
		*usInF &= ~(0x01 << 15);
		pDoc->Status.bDoorUc[DOOR_BR_UC] = FALSE;
	}

	usIn = pDoc->m_pMpeIb[7];
	usInF = &pDoc->m_pMpeIF[7];

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))		// 마킹부 도어 센서 1 
	{
		*usInF |= (0x01 << 10);
		pDoc->Status.bDoorMk[DOOR_FL_MK] = TRUE;
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))	// 마킹부 도어 센서 1 
	{
		*usInF &= ~(0x01 << 10);
		pDoc->Status.bDoorMk[DOOR_FL_MK] = FALSE;
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))		// 마킹부 도어 센서 2 
	{
		*usInF |= (0x01 << 11);
		pDoc->Status.bDoorMk[DOOR_FR_MK] = TRUE;
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))	// 마킹부 도어 센서 2
	{
		*usInF &= ~(0x01 << 11);
		pDoc->Status.bDoorMk[DOOR_FR_MK] = FALSE;
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))		// 마킹부 도어 센서 3 
	{
		*usInF |= (0x01 << 12);
		pDoc->Status.bDoorMk[DOOR_BL_MK] = TRUE;
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))	// 마킹부 도어 센서 3
	{
		*usInF &= ~(0x01 << 12);
		pDoc->Status.bDoorMk[DOOR_BL_MK] = FALSE;
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))		// 마킹부 도어 센서 4 
	{
		*usInF |= (0x01 << 13);
		pDoc->Status.bDoorMk[DOOR_BR_MK] = TRUE;
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))	// 마킹부 도어 센서 4
	{
		*usInF &= ~(0x01 << 13);
		pDoc->Status.bDoorMk[DOOR_BR_MK] = FALSE;
	}

	usIn = pDoc->m_pMpeIb[11];
	usInF = &pDoc->m_pMpeIF[11];

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))		// 검사부 상 도어 센서 1 
	{
		*usInF |= (0x01 << 10);
		pDoc->Status.bDoorMk[DOOR_FL_AOI_UP] = TRUE;
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))	// 검사부 상 도어 센서 1
	{
		*usInF &= ~(0x01 << 10);
		pDoc->Status.bDoorMk[DOOR_FL_AOI_UP] = FALSE;
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))		// 검사부 상 도어 센서 2 
	{
		*usInF |= (0x01 << 11);
		pDoc->Status.bDoorMk[DOOR_FR_AOI_UP] = TRUE;
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))	// 검사부 상 도어 센서 2
	{
		*usInF &= ~(0x01 << 11);
		pDoc->Status.bDoorMk[DOOR_FR_AOI_UP] = FALSE;
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))		// 검사부 상 도어 센서 3 
	{
		*usInF |= (0x01 << 12);
		pDoc->Status.bDoorMk[DOOR_BL_AOI_UP] = TRUE;
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))	// 검사부 상 도어 센서 3
	{
		*usInF &= ~(0x01 << 12);
		pDoc->Status.bDoorMk[DOOR_BL_AOI_UP] = FALSE;
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))		// 검사부 상 도어 센서 4 
	{
		*usInF |= (0x01 << 13);
		pDoc->Status.bDoorMk[DOOR_BR_AOI_UP] = TRUE;
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))	// 검사부 상 도어 센서 4
	{
		*usInF &= ~(0x01 << 13);
		pDoc->Status.bDoorMk[DOOR_BR_AOI_UP] = FALSE;
	}

	usIn = pDoc->m_pMpeIb[15];
	usInF = &pDoc->m_pMpeIF[15];

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))		// 검사부 상 도어 센서 1 
	{
		*usInF |= (0x01 << 10);
		pDoc->Status.bDoorAoi[DOOR_FL_AOI_UP] = TRUE;
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))	// 검사부 상 도어 센서 1
	{
		*usInF &= ~(0x01 << 11);
		pDoc->Status.bDoorAoi[DOOR_FL_AOI_UP] = FALSE;
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))		// 검사부 상 도어 센서 2 
	{
		*usInF |= (0x01 << 11);
		pDoc->Status.bDoorAoi[DOOR_FR_AOI_UP] = TRUE;
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))	// 검사부 상 도어 센서 2
	{
		*usInF &= ~(0x01 << 11);
		pDoc->Status.bDoorAoi[DOOR_FR_AOI_UP] = FALSE;
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))		// 검사부 상 도어 센서 3 
	{
		*usInF |= (0x01 << 12);
		pDoc->Status.bDoorAoi[DOOR_BL_AOI_UP] = TRUE;
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))	// 검사부 상 도어 센서 3
	{
		*usInF &= ~(0x01 << 12);
		pDoc->Status.bDoorAoi[DOOR_BL_AOI_UP] = FALSE;
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))		// 검사부 상 도어 센서 4 
	{
		*usInF |= (0x01 << 13);
		pDoc->Status.bDoorAoi[DOOR_BR_AOI_UP] = TRUE;
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))	// 검사부 상 도어 센서 4
	{
		*usInF &= ~(0x01 << 13);
		pDoc->Status.bDoorAoi[DOOR_BR_AOI_UP] = FALSE;
	}

	usIn = pDoc->m_pMpeIb[15];
	usInF = &pDoc->m_pMpeIF[15];

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))		// 검사부 하 도어 센서 1 
	{
		*usInF |= (0x01 << 10);
		pDoc->Status.bDoorAoi[DOOR_FL_AOI_DN] = TRUE;
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))	// 검사부 하 도어 센서 1
	{
		*usInF &= ~(0x01 << 10);
		pDoc->Status.bDoorAoi[DOOR_FL_AOI_DN] = FALSE;
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))		// 검사부 하 도어 센서 2 
	{
		*usInF |= (0x01 << 11);
		pDoc->Status.bDoorAoi[DOOR_FR_AOI_DN] = TRUE;
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))	// 검사부 하 도어 센서 2
	{
		*usInF &= ~(0x01 << 11);
		pDoc->Status.bDoorAoi[DOOR_FR_AOI_DN] = FALSE;
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))		// 검사부 하 도어 센서 3 
	{
		*usInF |= (0x01 << 12);
		pDoc->Status.bDoorAoi[DOOR_BL_AOI_DN] = TRUE;
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))	// 검사부 하 도어 센서 3
	{
		*usInF &= ~(0x01 << 12);
		pDoc->Status.bDoorAoi[DOOR_BL_AOI_DN] = FALSE;
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))		// 검사부 하 도어 센서 4 
	{
		*usInF |= (0x01 << 13);
		pDoc->Status.bDoorAoi[DOOR_BR_AOI_DN] = TRUE;
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))	// 검사부 하 도어 센서 4
	{
		*usInF &= ~(0x01 << 13);
		pDoc->Status.bDoorAoi[DOOR_BR_AOI_DN] = FALSE;
	}

	usIn = pDoc->m_pMpeIb[17];
	usInF = &pDoc->m_pMpeIF[17];

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))		// 리코일러 전면 도어 센서
	{
		*usInF |= (0x01 << 12);
		pDoc->Status.bDoorRe[DOOR_FL_RC] = TRUE;
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))	// 리코일러 전면 도어 센서
	{
		*usInF &= ~(0x01 << 12);
		pDoc->Status.bDoorRe[DOOR_FL_RC] = FALSE;
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))		// 리코일러 측면 도어 센서
	{
		*usInF |= (0x01 << 13);
		pDoc->Status.bDoorRe[DOOR_FR_RC] = TRUE;
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))	// 리코일러 측면 도어 센서
	{
		*usInF &= ~(0x01 << 13);
		pDoc->Status.bDoorRe[DOOR_FR_RC] = FALSE;
	}

	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))		// 리코일러 후면 도어 센서
	{
		*usInF |= (0x01 << 14);
		pDoc->Status.bDoorRe[DOOR_BL_RC] = TRUE;
	}
	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))	// 리코일러 후면 도어 센서(좌)
	{
		*usInF &= ~(0x01 << 14);
		pDoc->Status.bDoorRe[DOOR_BL_RC] = FALSE;
	}

	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))		// 리코일러 후면 도어 센서(우)
	{
		*usInF |= (0x01 << 15);
		pDoc->Status.bDoorRe[DOOR_BR_RC] = TRUE;
	}
	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))	// 리코일러 후면 도어 센서(우)
	{
		*usInF &= ~(0x01 << 15);
		pDoc->Status.bDoorRe[DOOR_BR_RC] = FALSE;
	}

	usIn = pDoc->m_pMpeIb[27];
	usInF = &pDoc->m_pMpeIF[27];

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))		// 각인부 도어 센서 1
	{
		*usInF |= (0x01 << 10);
		pDoc->Status.bDoorEngv[DOOR_FL_ENGV] = TRUE;
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))	// 각인부 도어 센서 1
	{
		*usInF &= ~(0x01 << 10);
		pDoc->Status.bDoorEngv[DOOR_FL_ENGV] = FALSE;
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))		// 각인부 도어 센서 2
	{
		*usInF |= (0x01 << 11);
		pDoc->Status.bDoorEngv[DOOR_FR_ENGV] = TRUE;
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))	// 각인부 도어 센서 2
	{
		*usInF &= ~(0x01 << 11);
		pDoc->Status.bDoorEngv[DOOR_FR_ENGV] = FALSE;
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))		// 각인부 도어 센서 3
	{
		*usInF |= (0x01 << 12);
		pDoc->Status.bDoorEngv[DOOR_BL_ENGV] = TRUE;
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))	// 각인부 도어 센서 3
	{
		*usInF &= ~(0x01 << 12);
		pDoc->Status.bDoorEngv[DOOR_BL_ENGV] = FALSE;
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))		// 각인부 도어 센서 4
	{
		*usInF |= (0x01 << 13);
		pDoc->Status.bDoorEngv[DOOR_BR_ENGV] = TRUE;
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))	// 각인부 도어 센서 4
	{
		*usInF &= ~(0x01 << 13);
		pDoc->Status.bDoorEngv[DOOR_BR_ENGV] = FALSE;
	}

	usIn = pDoc->m_pMpeIb[28];
	usInF = &pDoc->m_pMpeIF[28];


	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// 2D 리셋 스위치
		pView->m_mgrProcedure->m_bSwStopNow = FALSE;
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwReset();
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
	}

#endif
}

void CGvisR2R_PunchView::DoEngraveSens()
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
		*usInF |= (0x01 << 4);								// 2D 리셋 스위치
		pView->m_mgrProcedure->m_bSwStopNow = FALSE;
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwReset();
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
	}
#endif
}

void CGvisR2R_PunchView::DoModeSel()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)
		return;

	BOOL bMode;// [2];
	bMode = pDoc->m_pMpeIo[36] & (0x01 << 5) ? TRUE : FALSE;	// 마킹부 자동 상태 스위치 램프

	if (bMode)
	{
		pDoc->Status.bAuto = TRUE;
		pDoc->Status.bManual = FALSE;
		pDoc->Status.bOneCycle = FALSE;

		pView->m_mgrProcedure->m_bAuto = TRUE;
		pView->m_mgrProcedure->m_bManual = FALSE;
		pView->m_mgrProcedure->m_bOneCycle = FALSE;
	}
	else
	{
		pDoc->Status.bManual = TRUE;
		pDoc->Status.bAuto = FALSE;
		pDoc->Status.bOneCycle = FALSE;

		pView->m_mgrProcedure->m_bManual = TRUE;
		pView->m_mgrProcedure->m_bAuto = FALSE;
		pView->m_mgrProcedure->m_bOneCycle = FALSE;
	}

#else
	pDoc->Status.bAuto = FALSE;
	pDoc->Status.bManual = TRUE;
	pDoc->Status.bOneCycle = FALSE;
#endif

}

void CGvisR2R_PunchView::DoMainSw()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIb || !pDoc->m_pMpeIF)
		return;

	unsigned short usIn = pDoc->m_pMpeIb[4];
	unsigned short *usInF = &pDoc->m_pMpeIF[4];

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
	{
		*usInF |= (0x01 << 0);								// 마킹부 비상정지 스위치(모니터부)
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
	}

	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
	{
		*usInF |= (0x01 << 1);								// 마킹부 운전 스위치
		pView->m_mgrProcedure->m_bSwStopNow = FALSE;
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwRun();
	}
	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
	{
		*usInF &= ~(0x01 << 1);
	}

	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
	{
		*usInF |= (0x01 << 2);								// 마킹부 정지 스위치
	}
	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
	{
		*usInF &= ~(0x01 << 2);
	}

	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
	{
		*usInF |= (0x01 << 3);								// 마킹부 운전준비 스위치
		pView->m_mgrProcedure->m_bSwStopNow = FALSE;
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwReady();
	}
	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
	{
		*usInF &= ~(0x01 << 3);
	}

	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// 마킹부 리셋 스위치
		pView->m_mgrProcedure->m_bSwStopNow = FALSE;
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwReset();
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
	}

	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
	{
		*usInF |= (0x01 << 7);								// 마킹부 JOG 버튼(상)
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
		*usInF |= (0x01 << 8);								// 마킹부 JOG 버튼(하)
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
		*usInF |= (0x01 << 9);								// 마킹부 JOG 버튼(좌)
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
		*usInF |= (0x01 << 10);								// 마킹부 JOG 버튼(우)
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
		*usInF |= (0x01 << 11);								// 마킹부 모션 선택(LEFT)
		pDoc->Status.bSwJogLeft = TRUE;
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
	{
		*usInF &= ~(0x01 << 11);
		pDoc->Status.bSwJogLeft = FALSE;
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
	{
		*usInF |= (0x01 << 12);								// 마킹부 속도 선택
		pDoc->Status.bSwJogFast = TRUE;
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
	{
		*usInF &= ~(0x01 << 12);
		pDoc->Status.bSwJogFast = FALSE;
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
	{
		*usInF |= (0x01 << 13);								// 마킹부 운전 선택(INDEX)
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


void CGvisR2R_PunchView::DoMkSens()
{
	unsigned short usIn = 0;
	unsigned short *usInF = NULL;

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
	{
		*usInF |= (0x01 << 0);								// 마킹부 테이블 진공 센서
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
	}

	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
	{
		*usInF |= (0x01 << 1);								// 마킹부 테이블 압력 스위치
	}
	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
	{
		*usInF &= ~(0x01 << 1);
	}

	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
	{
		*usInF |= (0x01 << 2);								// 마킹부 피딩 진공 센서
	}
	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
	{
		*usInF &= ~(0x01 << 2);
	}

	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
	{
		*usInF |= (0x01 << 3);								// 마킹부 피딩 클램프 상승 센서
	}
	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
	{
		*usInF &= ~(0x01 << 3);
	}

	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// 마킹부 피딩 클램프 하강 센서
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
	}

	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
	{
		*usInF |= (0x01 << 5);								// 마킹부 토크 진공 센서
	}
	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
	{
		*usInF &= ~(0x01 << 5);
	}

	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
	{
		*usInF |= (0x01 << 6);								// 마킹부 토크 클램프 상승 센서
	}
	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
	{
		*usInF &= ~(0x01 << 6);
	}

	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
	{
		*usInF |= (0x01 << 7);								// 마킹부 토크 클램프 하강 센서
	}
	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
	{
		*usInF &= ~(0x01 << 7);
	}

	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
	{
		*usInF |= (0x01 << 8);								// 마킹부 댄서롤 상승 센서
	}
	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
	{
		*usInF &= ~(0x01 << 8);
	}

	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
	{
		*usInF |= (0x01 << 9);								// 마킹부 댄서롤 하강 센서
	}
	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
	{
		*usInF &= ~(0x01 << 9);
	}

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
	{
		*usInF |= (0x01 << 10);								// 검사부 피딩 진공 센서
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
	{
		*usInF &= ~(0x01 << 10);
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
	{
		*usInF |= (0x01 << 11);								// 검사부 피딩 클램프 상승 센서
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
	{
		*usInF &= ~(0x01 << 11);
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
	{
		*usInF |= (0x01 << 12);								// 검사부 피딩 클램프 하강 센서
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
	{
		*usInF &= ~(0x01 << 12);
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
	{
		*usInF |= (0x01 << 13);								// 검사부 토크 진공 센서
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
	{
		*usInF &= ~(0x01 << 13);
	}

	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))
	{
		*usInF |= (0x01 << 14);								// 검사부 토크 클램프 상승 센서
	}
	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))
	{
		*usInF &= ~(0x01 << 14);
	}

	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
	{
		*usInF |= (0x01 << 15);								// 검사부 토크 클램프 하강 센서
	}
	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
	{
		*usInF &= ~(0x01 << 15);
	}
}

void CGvisR2R_PunchView::DoAoiBoxSw()
{
	unsigned short usIn = 0;
	unsigned short *usInF = NULL;

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
	{
		*usInF |= (0x01 << 0);								// 검사부 연동 온/오프 스위치
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwAoiRelation();
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
	}

	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
	{
		*usInF |= (0x01 << 1);								// 검사부 테이블 브로워 스위치
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwAoiTblBlw();
	}
	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
	{
		*usInF &= ~(0x01 << 1);
	}

	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
	{
		*usInF |= (0x01 << 2);								// 검사부 피딩 정회전 스위치
	}
	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
	{
		*usInF &= ~(0x01 << 2);
	}

	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
	{
		*usInF |= (0x01 << 3);								// 검사부 피딩 역회전 스위치
	}
	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
	{
		*usInF &= ~(0x01 << 3);
	}

	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// 검사부 피딩 진공 스위치
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwAoiFdVac();
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
	}

	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
	{
		*usInF |= (0x01 << 5);								// 검사부 토크 진공 스위치
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwAoiTqVac();
	}
	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
	{
		*usInF &= ~(0x01 << 5);
	}

	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
	{
		*usInF |= (0x01 << 6);								// 검사부 테이블 진공 스위치
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwAoiTblVac();
	}
	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
	{
		*usInF &= ~(0x01 << 6);
	}

	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
	{
		*usInF |= (0x01 << 7);								// 검사부 레이져 포인터 스위치
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwAoiLsrPt(TRUE);
	}
	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
	{
		*usInF &= ~(0x01 << 7);
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwAoiLsrPt(FALSE);
	}

	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
	{
		*usInF |= (0x01 << 8);								// 검사부 피딩 클램프 스위치
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwAoiFdClp();
	}
	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
	{
		*usInF &= ~(0x01 << 8);
	}

	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
	{
		*usInF |= (0x01 << 9);								// 검사부 토크 클램프 스위치
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwAoiTqClp();
	}
	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
	{
		*usInF &= ~(0x01 << 9);
	}
}

void CGvisR2R_PunchView::DoEmgSens()
{
#ifdef USE_MPE
	unsigned short usIn, *usInF;

	if (!pDoc->m_pMpeIb || !pDoc->m_pMpeIF)
		return;

	usIn = pDoc->m_pMpeIb[0];
	usInF = &pDoc->m_pMpeIF[0];

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))		// 언코일러 비상정지 스위치
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

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))		// 마킹부 비상정지 스위치(모니터부)
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

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))		// 마킹부 비상정지 스위치(스위치부)	
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

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))		// 검사부 상 비상정지 스위치(후면) 
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

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))		// 검사부 하 비상정지 스위치(후면) 
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

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))		// 리코일러 비상정지 스위치
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

void CGvisR2R_PunchView::DoSignal()
{
	unsigned short usIn = 0;
	unsigned short *usInF = NULL;

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
	{
		*usInF |= (0x01 << 0);								// 검사부 검사 완료
		pDoc->Status.bSigTestDoneAoi = TRUE;
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
		pDoc->Status.bSigTestDoneAoi = FALSE;
	}

	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
	{
		*usInF |= (0x01 << 1);								// 검사부 테이블 진공 완료
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
		*usInF |= (0x01 << 8);								// 검사부 시그날 타워-적색
		TowerLamp(RGB_RED, TRUE, FALSE);
	}
	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
	{
		*usInF &= ~(0x01 << 8);
		TowerLamp(RGB_RED, FALSE, FALSE);
	}

	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
	{
		*usInF |= (0x01 << 9);								// 검사부 시그날 타워-황색
		TowerLamp(RGB_YELLOW, TRUE, FALSE);
	}
	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
	{
		*usInF &= ~(0x01 << 9);
		TowerLamp(RGB_YELLOW, FALSE, FALSE);
	}

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
	{
		*usInF |= (0x01 << 10);								// 검사부 시그날 타워-녹색
		TowerLamp(RGB_GREEN, TRUE, FALSE);
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
	{
		*usInF &= ~(0x01 << 10);
		TowerLamp(RGB_GREEN, FALSE, FALSE);
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
	{
		*usInF |= (0x01 << 11);								// 검사부 부져 1
		Buzzer(TRUE, 0);
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
	{
		*usInF &= ~(0x01 << 11);
		Buzzer(FALSE, 0);
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
	{
		*usInF |= (0x01 << 12);								// 검사부 부져 2
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

void CGvisR2R_PunchView::DoUcBoxSw()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)// || !pDoc->m_pMpeIoF)
		return;

	unsigned short usIn = 0;
	unsigned short *usInF = NULL;

	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
	{
		*usInF |= (0x01 << 1);								// 언코일러 연동 온/오프 스위치
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwUcRelation();
	}
	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
	{
		*usInF &= ~(0x01 << 1);
	}

	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
	{
		*usInF |= (0x01 << 2);								// 언코일러 댄서롤 상승/하강 스위치
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwUcDcRlUpDn();
	}
	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
	{
		*usInF &= ~(0x01 << 2);
	}

	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
	{
		*usInF |= (0x01 << 3);								// 언코일러 클린롤러 상승/하강 스위치
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwUcClRlUpDn();
	}
	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
	{
		*usInF &= ~(0x01 << 3);
	}

	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// 언코일러 클린롤러누름 상승/하강 스위치
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwUcClRlPshUpDn();
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
	}

	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
	{
		*usInF |= (0x01 << 5);								// 언코일러 제품 이음매(좌) 스위치
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwUcReelJoinL();
	}
	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
	{
		*usInF &= ~(0x01 << 5);
	}

	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
	{
		*usInF |= (0x01 << 6);								// 언코일러 제품 이음매(우) 스위치	
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwUcReelJoinR();
	}
	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
	{
		*usInF &= ~(0x01 << 6);
	}

	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
	{
		*usInF |= (0x01 << 7);								// 언코일러 제품휠 지지 스위치
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwUcReelWheel();
	}
	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
	{
		*usInF &= ~(0x01 << 7);
	}

	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
	{
		*usInF |= (0x01 << 8);								// 언코일러 간지척 클램프 스위치
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwUcPprChuck();
	}
	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
	{
		*usInF &= ~(0x01 << 8);
	}

	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
	{
		*usInF |= (0x01 << 9);								// 언코일러 간지휠 정회전 스위치
	}
	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
	{
		*usInF &= ~(0x01 << 9);
	}

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
	{
		*usInF |= (0x01 << 10);								// 언코일러 간지휠 역회전 스위치
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
	{
		*usInF &= ~(0x01 << 10);
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
	{
		*usInF |= (0x01 << 11);								// 언코일러 제품척 클램프 스위치
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwUcReelChuck();
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
	{
		*usInF &= ~(0x01 << 11);
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
	{
		*usInF |= (0x01 << 12);								// 언코일러 제품휠 정회전 스위치
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
	{
		*usInF &= ~(0x01 << 12);
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
	{
		*usInF |= (0x01 << 13);								// 언코일러 제품휠 역회전 스위치
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

void CGvisR2R_PunchView::DoUcSens1()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)// || !pDoc->m_pMpeIoF)
		return;

	unsigned short usIn = 0;
	unsigned short *usInF = NULL;

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
	{
		*usInF |= (0x01 << 0);								// 언코일러 간지모터조절 1	
		// Late....
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
	}

	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
	{
		*usInF |= (0x01 << 1);								// 언코일러 간지모터조절 2	
		// Late....
	}
	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
	{
		*usInF &= ~(0x01 << 1);
	}

	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
	{
		*usInF |= (0x01 << 2);								// 언코일러 간지모터조절 3	
		// Late....
	}
	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
	{
		*usInF &= ~(0x01 << 2);
	}

	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
	{
		*usInF |= (0x01 << 3);								// 언코일러 간지모터조절 4	
		// Late....
	}
	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
	{
		*usInF &= ~(0x01 << 3);
	}

	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// 언코일러 제품모터조절 1	
		// Late....
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
	}

	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
	{
		*usInF |= (0x01 << 5);								// 언코일러 제품모터조절 2
		// Late....
	}
	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
	{
		*usInF &= ~(0x01 << 5);
	}

	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
	{
		*usInF |= (0x01 << 6);								// 언코일러 제품모터조절 3	
		// Late....
	}
	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
	{
		*usInF &= ~(0x01 << 6);
	}

	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
	{
		*usInF |= (0x01 << 7);								// 언코일러 제품모터조절 4	
		// Late....
	}
	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
	{
		*usInF &= ~(0x01 << 7);
	}

	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
	{
		*usInF |= (0x01 << 8);								// 언코일러 제품 EPC POS 리미트 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
	{
		*usInF &= ~(0x01 << 8);
	}

	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
	{
		*usInF |= (0x01 << 9);								// 언코일러 제품 EPC NEG 리미트 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
	{
		*usInF &= ~(0x01 << 9);
	}

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
	{
		*usInF |= (0x01 << 10);								// 언코일러 제품 EPC 원점 센서 
		// Late....
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
	{
		*usInF &= ~(0x01 << 10);
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
	{
		*usInF |= (0x01 << 11);								// 언코일러 전면 도어 센서(좌)
		pDoc->Status.bDoorUc[DOOR_FL_UC] = TRUE;
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
	{
		*usInF &= ~(0x01 << 11);
		pDoc->Status.bDoorUc[DOOR_FL_UC] = FALSE;
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
	{
		*usInF |= (0x01 << 12);								// 언코일러 전면 도어 센서(우)
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
	{
		*usInF &= ~(0x01 << 12);
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
	{
		*usInF |= (0x01 << 13);								// 언코일러 측면 도어 센서
		pDoc->Status.bDoorUc[DOOR_FR_UC] = TRUE;
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
	{
		*usInF &= ~(0x01 << 13);
		pDoc->Status.bDoorUc[DOOR_FR_UC] = FALSE;
	}

	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))
	{
		*usInF |= (0x01 << 14);								// 언코일러 후면 도어 센서(좌)
		pDoc->Status.bDoorUc[DOOR_BL_UC] = TRUE;
	}
	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))
	{
		*usInF &= ~(0x01 << 14);
		pDoc->Status.bDoorUc[DOOR_BL_UC] = FALSE;
	}

	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
	{
		*usInF |= (0x01 << 15);								// 언코일러 후면 도어 센서(우)
		pDoc->Status.bDoorUc[DOOR_BR_UC] = TRUE;
	}
	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
	{
		*usInF &= ~(0x01 << 15);
		pDoc->Status.bDoorUc[DOOR_BR_UC] = FALSE;
	}
#endif
}

void CGvisR2R_PunchView::DoUcSens2()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)
		return;

	unsigned short usIn = 0;
	unsigned short *usInF = NULL;

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
	{
		*usInF |= (0x01 << 0);								// 언코일러 간지텐션 POS 리미트 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
	}

	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
	{
		*usInF |= (0x01 << 1);								// 언코일러 간지텐션 상한 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
	{
		*usInF &= ~(0x01 << 1);
	}

	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
	{
		*usInF |= (0x01 << 2);								// 언코일러 간지텐션 2/3 지점 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
	{
		*usInF &= ~(0x01 << 2);
	}

	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
	{
		*usInF |= (0x01 << 3);								// 언코일러 간지텐션 1/3 지점 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
	{
		*usInF &= ~(0x01 << 3);
	}

	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// 언코일러 간지텐션 하한 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
	}

	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
	{
		*usInF |= (0x01 << 5);								// 언코일러 간지텐션 NEG 리미트 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
	{
		*usInF &= ~(0x01 << 5);
	}

	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
	{
		*usInF |= (0x01 << 6);								// 언코일러 간지척 진공 스위치
		// Late....
	}
	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
	{
		*usInF &= ~(0x01 << 6);
	}

	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
	{
		*usInF |= (0x01 << 7);								// 언코일러 간지버퍼 압력 스위치	
		// Late....
	}
	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
	{
		*usInF &= ~(0x01 << 7);
	}

	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
	{
		*usInF |= (0x01 << 8);								// 언코일러 클린롤러 상승 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
	{
		*usInF &= ~(0x01 << 8);
	}

	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
	{
		*usInF |= (0x01 << 9);								// 언코일러 클린롤러 하강 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
	{
		*usInF &= ~(0x01 << 9);
	}

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
	{
		*usInF |= (0x01 << 10);								// 언코일러 클린롤러 누름(전면) 상승 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
	{
		*usInF &= ~(0x01 << 10);
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
	{
		*usInF |= (0x01 << 11);								// 언코일러 클린롤러 누름(전면) 하강 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
	{
		*usInF &= ~(0x01 << 11);
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
	{
		*usInF |= (0x01 << 12);								// 언코일러 클린롤러 누름(후면) 상승 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
	{
		*usInF &= ~(0x01 << 12);
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
	{
		*usInF |= (0x01 << 13);								// 언코일러 클린롤러 누름(후면) 하강 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
	{
		*usInF &= ~(0x01 << 13);
	}

	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))
	{
		*usInF |= (0x01 << 14);								// 언코일러 댄서롤 상승 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))
	{
		*usInF &= ~(0x01 << 14);
	}

	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
	{
		*usInF |= (0x01 << 15);								// 언코일러 댄서롤 하강 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
	{
		*usInF &= ~(0x01 << 15);
	}
#endif
}

void CGvisR2R_PunchView::DoUcSens3()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)
		return;

	unsigned short usIn = 0;
	unsigned short *usInF = NULL;

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
	{
		*usInF |= (0x01 << 0);								// 언코일러 댄서롤 POS 리미트 센서
		pDoc->Status.bSensLmtBufUc[LMT_POS] = TRUE;
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
		pDoc->Status.bSensLmtBufUc[LMT_POS] = FALSE;
	}

	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
	{
		*usInF |= (0x01 << 1);								// 언코일러 댄서롤 상한 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
	{
		*usInF &= ~(0x01 << 1);
	}

	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
	{
		*usInF |= (0x01 << 2);								// 언코일러 댄서롤 2/3 지점 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
	{
		*usInF &= ~(0x01 << 2);
	}

	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
	{
		*usInF |= (0x01 << 3);								// 언코일러 댄서롤 1/3 지점 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
	{
		*usInF &= ~(0x01 << 3);
	}

	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// 언코일러 댄서롤 하한 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
	}

	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
	{
		*usInF |= (0x01 << 5);								// 언코일러 댄서롤 NEG 리미트 센서
		pDoc->Status.bSensLmtBufUc[LMT_NEG] = TRUE;
	}
	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
	{
		*usInF &= ~(0x01 << 5);
		pDoc->Status.bSensLmtBufUc[LMT_NEG] = FALSE;
	}

	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
	{
		*usInF |= (0x01 << 6);								// 언코일러 제품척 진공 스위치
		// Late....
	}
	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
	{
		*usInF &= ~(0x01 << 6);
	}

	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
	{
		*usInF |= (0x01 << 7);								// 언코일러 메인 에어 
		// Late....
	}
	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
	{
		*usInF &= ~(0x01 << 7);
	}

	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
	{
		*usInF |= (0x01 << 8);								// 언코일러 제품휠 지지 상승 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
	{
		*usInF &= ~(0x01 << 8);
	}

	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
	{
		*usInF |= (0x01 << 9);								// 언코일러 제품휠 지지 하강 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
	{
		*usInF &= ~(0x01 << 9);
	}

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
	{
		*usInF |= (0x01 << 10);								// 언코일러 제품 이음매(좌) 상승 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
	{
		*usInF &= ~(0x01 << 10);
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
	{
		*usInF |= (0x01 << 11);								// 언코일러 제품 이음매(좌) 하강 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
	{
		*usInF &= ~(0x01 << 11);
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
	{
		*usInF |= (0x01 << 12);								// 언코일러 제품 이음매(우) 상승 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
	{
		*usInF &= ~(0x01 << 12);
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
	{
		*usInF |= (0x01 << 13);								// 언코일러 제품 이음매(우) 하강 센서
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
		*usInF |= (0x01 << 15);								// 언코일러 제품휠 지지 정위치 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
	{
		*usInF &= ~(0x01 << 15);
	}
#endif
}

void CGvisR2R_PunchView::DoRcBoxSw()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)
		return;

	unsigned short usIn = 0;
	unsigned short *usInF = NULL;

	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
	{
		*usInF |= (0x01 << 1);								// 리코일러 연동 온/오프 스위치
	}
	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
	{
		*usInF &= ~(0x01 << 1);
	}

	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
	{
		*usInF |= (0x01 << 2);								// 리코일러 댄서롤 상승/하강 스위치
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
		*usInF |= (0x01 << 5);								// 리코일러 제품 이음매(좌) 스위치
	}
	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
	{
		*usInF &= ~(0x01 << 5);
	}

	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
	{
		*usInF |= (0x01 << 6);								// 리코일러 제품 이음매(우) 스위치
	}
	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
	{
		*usInF &= ~(0x01 << 6);
	}

	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
	{
		*usInF |= (0x01 << 7);								// 리코일러 제품휠 지지 스위치
	}
	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
	{
		*usInF &= ~(0x01 << 7);
	}

	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
	{
		*usInF |= (0x01 << 8);								// 리코일러 간지척 클램프 스위치
	}
	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
	{
		*usInF &= ~(0x01 << 8);
	}

	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
	{
		*usInF |= (0x01 << 9);								// 리코일러 간지휠 정회전 스위치
	}
	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
	{
		*usInF &= ~(0x01 << 9);
	}

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
	{
		*usInF |= (0x01 << 10);								// 리코일러 간지휠 역회전 스위치
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
	{
		*usInF &= ~(0x01 << 10);
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
	{
		*usInF |= (0x01 << 11);								// 리코일러 제품척 클램프 스위치
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
	{
		*usInF &= ~(0x01 << 11);
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
	{
		*usInF |= (0x01 << 12);								// 리코일러 제품휠 정회전 스위치
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
	{
		*usInF &= ~(0x01 << 12);
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
	{
		*usInF |= (0x01 << 13);								// 리코일러 제품휠 역회전 스위치
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

void CGvisR2R_PunchView::DoRcSens1()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)
		return;

	unsigned short usIn = 0;
	unsigned short *usInF = NULL;

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
	{
		*usInF |= (0x01 << 0);								// 리코일러 간지모터조절 1
		// Late....
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
	}

	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
	{
		*usInF |= (0x01 << 1);								// 리코일러 간지모터조절 2
		// Late....
	}
	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
	{
		*usInF &= ~(0x01 << 1);
	}

	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
	{
		*usInF |= (0x01 << 2);								// 리코일러 간지모터조절 3
		// Late....
	}
	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
	{
		*usInF &= ~(0x01 << 2);
	}

	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
	{
		*usInF |= (0x01 << 3);								// 리코일러 간지모터조절 4
		// Late....
	}
	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
	{
		*usInF &= ~(0x01 << 3);
	}

	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// 리코일러 제품모터조절 1
		// Late....
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
	}

	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
	{
		*usInF |= (0x01 << 5);								// 리코일러 제품모터조절 2
		// Late....
	}
	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
	{
		*usInF &= ~(0x01 << 5);
	}

	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
	{
		*usInF |= (0x01 << 6);								// 리코일러 제품모터조절 3
		// Late....
	}
	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
	{
		*usInF &= ~(0x01 << 6);
	}

	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
	{
		*usInF |= (0x01 << 7);								// 리코일러 제품모터조절 4
		// Late....
	}
	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
	{
		*usInF &= ~(0x01 << 7);
	}

	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
	{
		*usInF |= (0x01 << 8);								// 리코일러 제품 EPC POS 리미트 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
	{
		*usInF &= ~(0x01 << 8);
	}

	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
	{
		*usInF |= (0x01 << 9);								// 리코일러 제품 EPC NEG 리미트 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
	{
		*usInF &= ~(0x01 << 9);
	}

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
	{
		*usInF |= (0x01 << 10);								// 리코일러 제품 EPC 원점 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
	{
		*usInF &= ~(0x01 << 10);
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
	{
		*usInF |= (0x01 << 11);								// 리코일러 전면 도어 센서(좌)
		pDoc->Status.bDoorRe[DOOR_FL_RC] = TRUE;
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
	{
		*usInF &= ~(0x01 << 11);
		pDoc->Status.bDoorRe[DOOR_FL_RC] = FALSE;
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
	{
		*usInF |= (0x01 << 12);								// 리코일러 전면 도어 센서(우)
		pDoc->Status.bDoorRe[DOOR_FR_RC] = TRUE;
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
	{
		*usInF &= ~(0x01 << 12);
		pDoc->Status.bDoorRe[DOOR_FR_RC] = FALSE;
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
	{
		*usInF |= (0x01 << 13);								// 리코일러 측면 도어 센서
		pDoc->Status.bDoorRe[DOOR_S_RC] = TRUE;
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
	{
		*usInF &= ~(0x01 << 13);
		pDoc->Status.bDoorRe[DOOR_S_RC] = FALSE;
	}

	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))
	{
		*usInF |= (0x01 << 14);								// 리코일러 후면 도어 센서(좌)
		pDoc->Status.bDoorRe[DOOR_BL_RC] = TRUE;
	}
	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))
	{
		*usInF &= ~(0x01 << 14);
		pDoc->Status.bDoorRe[DOOR_BL_RC] = FALSE;
	}

	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
	{
		*usInF |= (0x01 << 15);								// 리코일러 후면 도어 센서(우)
		pDoc->Status.bDoorRe[DOOR_BR_RC] = TRUE;
	}
	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
	{
		*usInF &= ~(0x01 << 15);
		pDoc->Status.bDoorRe[DOOR_BR_RC] = FALSE;
	}
#endif
}

void CGvisR2R_PunchView::DoRcSens2()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)
		return;

	unsigned short usIn = 0;
	unsigned short *usInF = NULL;

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
	{
		*usInF |= (0x01 << 0);								// 리코일러 간지텐션 POS 리미트 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
	}

	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
	{
		*usInF |= (0x01 << 1);								// 리코일러 간지텐션 상한 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
	{
		*usInF &= ~(0x01 << 1);
	}

	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
	{
		*usInF |= (0x01 << 2);								// 리코일러 간지텐션 2/3 지점 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
	{
		*usInF &= ~(0x01 << 2);
	}

	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
	{
		*usInF |= (0x01 << 3);								// 리코일러 간지텐션 1/3 지점 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
	{
		*usInF &= ~(0x01 << 3);
	}

	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// 리코일러 간지텐션 하한 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
	}

	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
	{
		*usInF |= (0x01 << 5);								// 리코일러 간지텐션 NEG 리미트 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
	{
		*usInF &= ~(0x01 << 5);
	}

	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
	{
		*usInF |= (0x01 << 6);								// 리코일러 간지척 진공 스위치
		// Late....
	}
	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
	{
		*usInF &= ~(0x01 << 6);
	}

	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
	{
		*usInF |= (0x01 << 7);								// 리코일러 간지버퍼 압력 스위치
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
		*usInF |= (0x01 << 14);								// 리코일러 댄서롤 상승 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))
	{
		*usInF &= ~(0x01 << 14);
	}

	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
	{
		*usInF |= (0x01 << 15);								// 리코일러 댄서롤 하강 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
	{
		*usInF &= ~(0x01 << 15);
	}
#endif
}

void CGvisR2R_PunchView::DoRcSens3()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)
		return;

	unsigned short usIn = 0;
	unsigned short *usInF = NULL;

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
	{
		*usInF |= (0x01 << 0);								// 리코일러 댄서롤 POS 리미트 센서
		pDoc->Status.bSensLmtBufRc[LMT_POS] = TRUE;
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
		pDoc->Status.bSensLmtBufRc[LMT_POS] = FALSE;
	}

	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
	{
		*usInF |= (0x01 << 1);								// 리코일러 댄서롤 상한 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
	{
		*usInF &= ~(0x01 << 1);
	}

	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
	{
		*usInF |= (0x01 << 2);								// 리코일러 댄서롤 2/3 지점 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
	{
		*usInF &= ~(0x01 << 2);
	}

	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
	{
		*usInF |= (0x01 << 3);								// 리코일러 댄서롤 1/3 지점 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
	{
		*usInF &= ~(0x01 << 3);
	}

	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// 리코일러 댄서롤 하한 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
	}

	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
	{
		*usInF |= (0x01 << 5);								// 리코일러 댄서롤 NEG 리미트 센서
		pDoc->Status.bSensLmtBufRc[LMT_NEG] = TRUE;
	}
	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
	{
		*usInF &= ~(0x01 << 5);
		pDoc->Status.bSensLmtBufRc[LMT_NEG] = FALSE;
	}

	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
	{
		*usInF |= (0x01 << 6);								// 리코일러 제품척 진공 스위치
		// Late....
	}
	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
	{
		*usInF &= ~(0x01 << 6);
	}

	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
	{
		*usInF |= (0x01 << 7);								// 리코일러 메인 에어 
		// Late....
	}
	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
	{
		*usInF &= ~(0x01 << 7);
	}

	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
	{
		*usInF |= (0x01 << 8);								// 리코일러 제품휠 지지 상승 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
	{
		*usInF &= ~(0x01 << 8);
	}

	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
	{
		*usInF |= (0x01 << 9);								// 리코일러 제품휠 지지 하강 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
	{
		*usInF &= ~(0x01 << 9);
	}

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
	{
		*usInF |= (0x01 << 10);								// 리코일러 제품 이음매(좌) 상승 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
	{
		*usInF &= ~(0x01 << 10);
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
	{
		*usInF |= (0x01 << 11);								// 리코일러 제품 이음매(좌) 하강 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
	{
		*usInF &= ~(0x01 << 11);
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
	{
		*usInF |= (0x01 << 12);								// 리코일러 제품 이음매(우) 상승 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
	{
		*usInF &= ~(0x01 << 12);
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
	{
		*usInF |= (0x01 << 13);								// 리코일러 제품 이음매(우) 하강 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
	{
		*usInF &= ~(0x01 << 13);
	}

	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))
	{
		*usInF |= (0x01 << 14);								// 리코일러 제품 잔량 감지 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))
	{
		*usInF &= ~(0x01 << 14);
	}

	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
	{
		*usInF |= (0x01 << 15);								// 리코일러 제품휠 지지 정위치 센서
		// Late....
	}
	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
	{
		*usInF &= ~(0x01 << 15);
	}
#endif
}

void CGvisR2R_PunchView::SetAoiFdPitch(double dPitch)
{
	pDoc->SetAoiFdPitch(dPitch);
}

void CGvisR2R_PunchView::SetMkFdPitch(double dPitch)
{
	pDoc->SetMkFdPitch(dPitch);
	long lData = long(dPitch*1000.0);
#ifdef USE_MPE
	pView->m_pMpe->Write(_T("ML45014"), lData); // 마킹부 Feeding 롤러 Lead Pitch
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

void CGvisR2R_PunchView::SetBufHomeParam(double dVel, double dAcc)
{
	long lVel = long(dVel*1000.0);
	long lAcc = long(dAcc*1000.0);
	//	pView->m_pMpe->Write(_T("ML00000"), lVel); // 마킹부 버퍼 홈 속도
	//	pView->m_pMpe->Write(_T("ML00000"), lAcc); // 마킹부 버퍼 홈 가속도
	//	pDoc->SetBufInitPos(dVel, dAcc);
}

LRESULT CGvisR2R_PunchView::OnBufThreadDone(WPARAM wPara, LPARAM lPara)
{
	//if(m_pMotion)
	//{
	// 	m_pMotion->SetOriginPos(AXIS_AOIFD);
	// 	m_pMotion->SetOriginPos(AXIS_BUF);
	// 	m_pMotion->SetOriginPos(AXIS_RENC);
	//}
	// 
	//if(m_pVision[0])
	// 	m_pVision[0]->SetClrOverlay();
	// 
	//if(m_pVision[1])
	// 	m_pVision[1]->SetClrOverlay();

	return 0L;
}

//.........................................................................................

BOOL CGvisR2R_PunchView::WatiDispMain(int nDelay)
{
	if (pView->m_mgrProcedure->m_nWatiDispMain % nDelay)
	{
		pView->m_mgrProcedure->m_nWatiDispMain++;
		return TRUE;
	}

	pView->m_mgrProcedure->m_nWatiDispMain = 0;
	pView->m_mgrProcedure->m_nWatiDispMain++;
	return FALSE;
}

void CGvisR2R_PunchView::DispMain(CString sMsg, COLORREF rgb)
{
	pDoc->SetMonDispMain(sMsg);

	m_csDispMain.Lock();
	pView->m_mgrProcedure->m_bDispMain = FALSE;
	stDispMain stData(sMsg, rgb);
	m_ArrayDispMain.Add(stData);
	pView->m_mgrProcedure->m_bDispMain = TRUE;

	if (sMsg == _T("정 지"))
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

	if (!pView->m_mgrProcedure->m_bDispMain)
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
			pView->m_mgrProcedure->m_sDispMain = sMsg;
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

void CGvisR2R_PunchView::Shift2Buf()	// 버퍼폴더의 마지막 시리얼과 Share폴더의 시리얼이 연속인지 확인 후 옮김.
{
	int nLastListBuf;
	if (pView->m_mgrProcedure->m_nShareUpS > 0)
	{
		pView->m_mgrProcedure->m_bLoadShare[0] = TRUE;
		pDoc->m_ListBuf[0].Push(pView->m_mgrProcedure->m_nShareUpS);
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (bDualTest)
	{
		if (pView->m_mgrProcedure->m_nShareDnS > 0)
		{
			pView->m_mgrProcedure->m_bLoadShare[1] = TRUE;
			pDoc->m_ListBuf[1].Push(pView->m_mgrProcedure->m_nShareDnS);

			if (pView->m_mgrProcedure->m_nShareDnS == pView->m_mgrProcedure->m_nAoiLastSerial[0] - 3 && pView->m_mgrProcedure->m_nAoiLastSerial[0] > 0)
			{
				if (IsVsDn())
				{
					SetDummyDn();
					Sleep(30);
					SetDummyDn();
					Sleep(30);
					SetDummyDn();
					Sleep(30);
				}
			}
		}
	}

	pView->m_mgrReelmap->CopyPcrAll();
	pView->m_mgrReelmap->DelSharePcr();
}


void CGvisR2R_PunchView::CompletedMk(int nCam) // 0: Only Cam0, 1: Only Cam1, 2: Cam0 and Cam1, 3: None
{
	int nSerial = -1;

	switch (nCam)
	{
	case 0:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0];
		break;
	case 1:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1];
		break;
	case 2:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1];
		break;
	case 3:
		return;
	default:
		return;
	}

	pDoc->SetCompletedSerial(nSerial);
}

//pView->m_pMpe->Write(_T("MB003829"), 1); // 검사부 상 검사 테이블 진공 SOL <-> Y4369 I/F
//pView->m_pMpe->Write(_T("MB003829"), 0); // 검사부 상 검사 테이블 진공 SOL <-> Y4369 I/F
//pView->m_pMpe->Write(_T("MB003929"), 1); // 검사부 하 검사 테이블 진공 SOL <-> Y4369 I/F
//pView->m_pMpe->Write(_T("MB003929"), 0); // 검사부 하 검사 테이블 진공 SOL <-> Y4369 I/F

BOOL CGvisR2R_PunchView::IsMkFdSts()
{
	if (!m_pDlgMenu03)
		return FALSE;

	BOOL bOn[4] = { 0 };
	// 마킹부 - FD/TQ 진공 ON, TBL진공 OFF, TBL파기 ON, 
	bOn[0] = m_pDlgMenu03->IsMkFdVac(); // TRUE
	bOn[1] = m_pDlgMenu03->IsMkTqVac(); // TRUE
	bOn[2] = m_pDlgMenu03->IsMkTblVac(); // FALSE
	bOn[3] = m_pDlgMenu03->IsMkTblBlw(); // TRUE

	if (bOn[0] && bOn[1] && !bOn[2] && bOn[3])
		return TRUE;

	return FALSE;
}

void CGvisR2R_PunchView::SetDelay(int mSec, int nId)
{
	if (nId > 10)
		nId = 9;
	else if (nId < 0)
		nId = 0;

	pView->m_mgrProcedure->m_dwSetDlySt[nId] = GetTickCount();
	pView->m_mgrProcedure->m_dwSetDlyEd[nId] = pView->m_mgrProcedure->m_dwSetDlySt[nId] + mSec;
}

void CGvisR2R_PunchView::SetDelay0(int mSec, int nId)
{
	if (nId > 10)
		nId = 9;
	else if (nId < 0)
		nId = 0;

	pView->m_mgrProcedure->m_dwSetDlySt0[nId] = GetTickCount();
	pView->m_mgrProcedure->m_dwSetDlyEd0[nId] = pView->m_mgrProcedure->m_dwSetDlySt0[nId] + mSec;
}

void CGvisR2R_PunchView::SetDelay1(int mSec, int nId)
{
	if (nId > 10)
		nId = 9;
	else if (nId < 0)
		nId = 0;

	pView->m_mgrProcedure->m_dwSetDlySt1[nId] = GetTickCount();
	pView->m_mgrProcedure->m_dwSetDlyEd1[nId] = pView->m_mgrProcedure->m_dwSetDlySt1[nId] + mSec;
}

BOOL CGvisR2R_PunchView::WaitDelay(int nId) // F:Done, T:On Waiting....
{
	if (nId > 10)
		nId = 9;
	else if (nId < 0)
		nId = 0;

	DWORD dwCur = GetTickCount();
	if (dwCur < pView->m_mgrProcedure->m_dwSetDlyEd[nId])
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_PunchView::WaitDelay0(int nId) // F:Done, T:On Waiting....
{
	if (nId > 10)
		nId = 9;
	else if (nId < 0)
		nId = 0;

	DWORD dwCur = GetTickCount();
	if (dwCur < pView->m_mgrProcedure->m_dwSetDlyEd0[nId])
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_PunchView::WaitDelay1(int nId) // F:Done, T:On Waiting....
{
	if (nId > 10)
		nId = 9;
	else if (nId < 0)
		nId = 0;

	DWORD dwCur = GetTickCount();
	if (dwCur < pView->m_mgrProcedure->m_dwSetDlyEd1[nId])
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_PunchView::GetDelay(int &mSec, int nId) // F:Done, T:On Waiting....
{
	DWORD dwCur = GetTickCount();
	mSec = int(dwCur - pView->m_mgrProcedure->m_dwSetDlySt[nId]);
	if (dwCur <pView->m_mgrProcedure->m_dwSetDlyEd[nId])
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_PunchView::GetDelay0(int &mSec, int nId) // F:Done, T:On Waiting....
{
	DWORD dwCur = GetTickCount();
	mSec = int(dwCur - pView->m_mgrProcedure->m_dwSetDlySt0[nId]);
	if (dwCur < pView->m_mgrProcedure->m_dwSetDlyEd0[nId])
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_PunchView::GetDelay1(int &mSec, int nId) // F:Done, T:On Waiting....
{
	DWORD dwCur = GetTickCount();
	mSec = int(dwCur - pView->m_mgrProcedure->m_dwSetDlySt1[nId]);
	if (dwCur < pView->m_mgrProcedure->m_dwSetDlyEd1[nId])
		return TRUE;
	return FALSE;
}

void CGvisR2R_PunchView::SetAoiFd()
{
	CfPoint OfSt;
	if (GetAoiUpOffset(OfSt))
	{
		if (m_pDlgMenu02)
		{
			m_pDlgMenu02->m_dAoiUpFdOffsetX = OfSt.x;
			m_pDlgMenu02->m_dAoiUpFdOffsetY = OfSt.y;
		}
	}

	MoveAoi(-1.0*OfSt.x);
	if (m_pDlgMenu03)
		m_pDlgMenu03->ChkDoneAoi();

	if (!pDoc->WorkingInfo.LastJob.bAoiOnePnl)
	{
#ifdef USE_MPE
		pView->m_pMpe->Write(_T("MB440151"), 1);	// 한판넬 이송상태 ON (PC가 ON, OFF)
#endif
		CString sData, sPath = PATH_WORKING_INFO;
		pDoc->WorkingInfo.LastJob.bMkOnePnl = pDoc->WorkingInfo.LastJob.bAoiOnePnl = TRUE;
		sData.Format(_T("%d"), pDoc->WorkingInfo.LastJob.bMkOnePnl ? 1 : 0);
		::WritePrivateProfileString(_T("Last Job"), _T("Marking One Pannel Move On"), sData, sPath);
		::WritePrivateProfileString(_T("Last Job"), _T("AOI One Pannel Move On"), sData, sPath);
	}
}

void CGvisR2R_PunchView::MoveAoi(double dOffset)
{
	long lData = (long)(dOffset * 1000.0);		// 검사부 피딩 ON (PLC가 피딩완료 후 OFF)
	pView->m_pMpe->Write(_T("MB440160"), 1);	// 검사부 Feeding 롤러 Offset(*1000, +:더 보냄, -덜 보냄)
	pView->m_pMpe->Write(_T("ML45064"), lData);
}

void CGvisR2R_PunchView::SetMkFd()
{
	CfPoint OfSt;
	GetMkOffset(OfSt);

	if (pView->m_mgrProcedure->m_nShareDnCnt > 0)
	{
		if (!(pView->m_mgrProcedure->m_nShareDnCnt % 2))
			MoveMk(-1.0*OfSt.x);
	}
	else
	{
		if (pView->m_mgrProcedure->m_nShareUpCnt > 0)
		{
			if (!(pView->m_mgrProcedure->m_nShareUpCnt % 2))
				MoveMk(-1.0*OfSt.x);
		}
	}
	if (m_pDlgMenu03)
		m_pDlgMenu03->ChkDoneMk();

	if (!pDoc->WorkingInfo.LastJob.bAoiOnePnl)
	{
		pView->m_pMpe->Write(_T("MB440151"), 1);	// 한판넬 이송상태 ON (PC가 ON, OFF)

		CString sData, sPath = PATH_WORKING_INFO;
		pDoc->WorkingInfo.LastJob.bMkOnePnl = pDoc->WorkingInfo.LastJob.bAoiOnePnl = TRUE;
		sData.Format(_T("%d"), pDoc->WorkingInfo.LastJob.bMkOnePnl ? 1 : 0);
		::WritePrivateProfileString(_T("Last Job"), _T("Marking One Pannel Move On"), sData, sPath);
		::WritePrivateProfileString(_T("Last Job"), _T("AOI One Pannel Move On"), sData, sPath);
	}
}

void CGvisR2R_PunchView::MoveMk(double dOffset)
{
#ifdef USE_MPE
	long lData = (long)(dOffset * 1000.0);
	pView->m_pMpe->Write(_T("MB440161"), 1);		// 마킹부 피딩 CW ON (PLC가 피딩완료 후 OFF))
	pView->m_pMpe->Write(_T("ML45066"), lData);		// 마킹부 Feeding 롤러 Offset(*1000, +:더 보냄, -덜 보냄)
#endif
}

BOOL CGvisR2R_PunchView::IsMkFd()
{
	if (pView->m_mgrProcedure->m_nShareDnCnt > 0)
	{
		if (!(pView->m_mgrProcedure->m_nShareDnCnt % 2))
		{
#ifdef USE_MPE
			if (pDoc->m_pMpeSignal[5] & (0x01 << 1))	// 마킹부 피딩 CW ON (PLC가 피딩완료 후 OFF)
				return TRUE;
#endif
			return FALSE;
		}
	}
	else
	{
		if (pView->m_mgrProcedure->m_nShareUpCnt > 0)
		{
			if (!(pView->m_mgrProcedure->m_nShareUpCnt % 2))
			{
#ifdef USE_MPE
				if (pDoc->m_pMpeSignal[5] & (0x01 << 1))	// 마킹부 피딩 CW ON (PLC가 피딩완료 후 OFF)
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
	if (pDoc->m_pMpeSignal[5] & (0x01 >> 0))	// 검사부 피딩 ON (PLC가 피딩완료 후 OFF)
		return TRUE;
#endif
	return FALSE;
}

void CGvisR2R_PunchView::SetMkFd(double dDist)
{
	double fLen = pDoc->GetOnePnlLen();
	double dOffset = dDist - (fLen*2.0);
	MoveMk(dOffset);
}

void CGvisR2R_PunchView::SetCycTime()
{
	DWORD dCur = GetTickCount();
	if (pView->m_mgrProcedure->m_dwCycSt > 0)
	{
		pView->m_mgrProcedure->m_dwCycTim = (double)(dCur - pView->m_mgrProcedure->m_dwCycSt);
		if (pView->m_mgrProcedure->m_dwCycTim < 0.0)
			pView->m_mgrProcedure->m_dwCycTim *= (-1.0);
	}
	else
		pView->m_mgrProcedure->m_dwCycTim = 0.0;
}

int CGvisR2R_PunchView::GetCycTime()
{
	if (pView->m_mgrProcedure->m_dwCycTim < 0)
		pView->m_mgrProcedure->m_dwCycTim = 0;

	int nTim = int(pView->m_mgrProcedure->m_dwCycTim);
	return nTim;
}

BOOL CGvisR2R_PunchView::IsMkFdDone()
{
	if (pView->m_mgrProcedure->m_nShareDnCnt > 0)
	{
		if (pView->m_mgrProcedure->m_nShareDnCnt % 2)
			return TRUE;
	}
#ifdef USE_MPE
	if (!(pDoc->m_pMpeSignal[5] & (0x01 << 1)))	// 마킹부 피딩 ON (PLC가 피딩완료 후 OFF)
		return TRUE;
#endif
	return FALSE;
}

BOOL CGvisR2R_PunchView::IsAoiFdDone()
{
#ifdef USE_MPE
	if (!(pDoc->m_pMpeSignal[5] & (0x01 << 0)))	// 검사부 피딩 ON (PLC가 피딩완료 후 OFF)
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
	double dCurPosMkFd = (double)pDoc->m_pMpeData[0][0];	// 마킹부 Feeding 엔코더 값(단위 mm )
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
	if (m_pEngrave)
		m_pEngrave->SwMenu01UpdateWorking(TRUE);
}

void CGvisR2R_PunchView::StopFromThread()
{
	pView->m_mgrProcedure->m_bStopFromThread = TRUE;
}

void CGvisR2R_PunchView::Stop()
{
	CString sMsg;
	if (m_pDlgMenu03)
	{
		if (IsAuto() && pView->m_mgrProcedure->IsRun())
		{
			pView->m_mgrProcedure->m_bStopF_Verify = TRUE;
		}

		m_pDlgMenu03->SwStop();
	}
}

BOOL CGvisR2R_PunchView::IsStop()
{
	if (pView->m_mgrProcedure->m_sDispMain == _T("정 지"))
	{
		return TRUE;
	}
	return FALSE;
}

//BOOL CGvisR2R_PunchView::IsRun()
//{
//	//return TRUE; // AlignTest
//	if (pView->m_mgrProcedure->m_sDispMain == _T("운전중") || pView->m_mgrProcedure->m_sDispMain == _T("초기운전") 
//		|| pView->m_mgrProcedure->m_sDispMain == _T("단면샘플") || pView->m_mgrProcedure->m_sDispMain == _T("단면검사") 
//		|| pView->m_mgrProcedure->m_sDispMain == _T("양면검사") || pView->m_mgrProcedure->m_sDispMain == _T("양면샘플"))
//		return TRUE;
//	return FALSE;
//}

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

	pView->m_mgrProcedure->m_dwLotSt = GetTickCount();
	pDoc->SaveLotTime(pView->m_mgrProcedure->m_dwLotSt);
	DispLotTime();

	pView->m_mgrReelmap->SetLotSt();
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

	pView->m_mgrProcedure->m_dwLotEd = GetTickCount();

	pDoc->SaveLotTime(pDoc->WorkingInfo.Lot.dwStTick);
	DispLotTime();
	pView->m_mgrReelmap->SetLotEd();
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

	BOOL bOn0 = (pDoc->m_pMpeIo[nInSeg + 10] & 0x01 << 8) ? TRUE : FALSE; //[34] 검사부 상 검사 시작 <-> Y4368 I/F
	BOOL bOn1 = (pDoc->m_pMpeIo[nInSeg + 14] & 0x01 << 8) ? TRUE : FALSE; //[38] 검사부 하 검사 시작 <-> Y4468 I/F

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

	BOOL bOn0 = (pDoc->m_pMpeIo[nInSeg + 10] & 0x01 << 8) ? TRUE : FALSE; //[34] 검사부 상 검사 시작 <-> Y4368 I/F
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

	BOOL bOn1 = (pDoc->m_pMpeIo[nInSeg + 14] & 0x01 << 8) ? TRUE : FALSE; //[38] 검사부 하 검사 시작 <-> Y4468 I/F
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

	BOOL bOn0 = (pDoc->m_pMpeIo[nInSeg + 10] & 0x01 << 9) ? TRUE : FALSE; //[34] 검사부 상 검사 테이블 진공 SOL <-> Y4469 I/F
	BOOL bOn1 = (pDoc->m_pMpeIo[nInSeg + 14] & 0x01 << 9) ? TRUE : FALSE; //[38] 검사부 하 검사 테이블 진공 SOL <-> Y4469 I/F
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
		pView->m_pMpe->Write(_T("MB003828"), 1); // 검사부 상 검사 시작 <-> Y4368 I/F
		pView->m_pMpe->Write(_T("MB003928"), 1); // 검사부 하 검사 시작 <-> Y4468 I/F
	}
	else
	{
		pView->m_pMpe->Write(_T("MB003828"), 0); // 검사부 상 검사 시작 <-> Y4368 I/F
		pView->m_pMpe->Write(_T("MB003928"), 0); // 검사부 하 검사 시작 <-> Y4468 I/F
	}
#endif
}

void CGvisR2R_PunchView::SetTest0(BOOL bOn)
{
#ifdef USE_MPE
	if (bOn)
		pView->m_pMpe->Write(_T("MB003828"), 1); // 검사부 상 검사 시작 <-> Y4368 I/F
	else
		pView->m_pMpe->Write(_T("MB003828"), 0); // 검사부 상 검사 시작 <-> Y4368 I/F
#endif
}

void CGvisR2R_PunchView::SetTest1(BOOL bOn)
{
#ifdef USE_MPE
	if (bOn)
		pView->m_pMpe->Write(_T("MB003928"), 1); // 검사부 하 검사 시작 <-> Y4468 I/F
	else
		pView->m_pMpe->Write(_T("MB003928"), 0); // 검사부 하 검사 시작 <-> Y4468 I/F
#endif
}

BOOL CGvisR2R_PunchView::IsTestDone()
{
#ifdef USE_MPE
	BOOL bOn0 = (pDoc->m_pMpeIb[10] & (0x01 << 8)) ? TRUE : FALSE;	// 검사부 상 검사 완료 <-> X4328 I/F
	BOOL bOn1 = (pDoc->m_pMpeIb[14] & (0x01 << 8)) ? TRUE : FALSE;	// 검사부 하 검사 완료 <-> X4428 I/F
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

	double dCurPosMkFd = (double)pDoc->m_pMpeData[0][0];	// 마킹부 Feeding 엔코더 값(단위 mm )
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
	BOOL bOn0 = (pDoc->m_pMpeIb[10] & (0x01 << 9)) ? TRUE : FALSE;	// 검사부 상 테이블 진공 완료 <-> X4329 I/F
	BOOL bOn1 = (pDoc->m_pMpeIb[14] & (0x01 << 9)) ? TRUE : FALSE;	// 검사부 하 테이블 진공 완료 <-> X4329 I/F
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

	double dCurPosMkFd = (double)pDoc->m_pMpeData[0][0];	// 마킹부 Feeding 엔코더 값(단위 mm )
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
	BOOL bOn0 = (pDoc->m_pMpeIb[10] & (0x01 << 8)) ? TRUE : FALSE;	// 검사부 상 검사 완료 <-> X4328 I/F
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

	BOOL bOn1 = (pDoc->m_pMpeIb[14] & (0x01 << 8)) ? TRUE : FALSE;	// 검사부 하 검사 완료 <-> X4428 I/F
	if (bOn1)
		return TRUE;
#endif
	return TRUE;
}

void CGvisR2R_PunchView::SetReMk(BOOL bMk0, BOOL bMk1)
{
	CfPoint ptPnt;
	int nSerial, nTot;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (bMk0)
	{
		if (!pView->m_mgrProcedure->m_bTHREAD_MK[0])
		{
			pView->m_mgrProcedure->m_nStepMk[0] = 0;
			pView->m_mgrProcedure->m_nMkPcs[0] = 0;
			pView->m_mgrProcedure->m_bDoneMk[0] = FALSE;
			//m_bReMark[0] = FALSE;
			pView->m_mgrProcedure->m_bTHREAD_MK[0] = TRUE;

			nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Cam0

			nTot = GetTotDefPcs(nSerial);
			if (nTot>0)
			{
				ptPnt = GetMkPnt(nSerial, 0);
				pView->m_mgrProcedure->m_dTarget[AXIS_X0] = ptPnt.x;
				pView->m_mgrProcedure->m_dTarget[AXIS_Y0] = ptPnt.y;
				if (nTot>1)
				{
					ptPnt = GetMkPnt(nSerial, 1);
					pView->m_mgrProcedure->m_dNextTarget[AXIS_X0] = ptPnt.x;
					pView->m_mgrProcedure->m_dNextTarget[AXIS_Y0] = ptPnt.y;
				}
				else
				{
					pView->m_mgrProcedure->m_dNextTarget[AXIS_X0] = -1.0;
					pView->m_mgrProcedure->m_dNextTarget[AXIS_Y0] = -1.0;
				}
			}
			else
			{
				pView->m_mgrProcedure->m_dTarget[AXIS_X0] = -1.0;
				pView->m_mgrProcedure->m_dTarget[AXIS_Y0] = -1.0;
				pView->m_mgrProcedure->m_dNextTarget[AXIS_X0] = -1.0;
				pView->m_mgrProcedure->m_dNextTarget[AXIS_Y0] = -1.0;
			}
		}
	}

	if (bMk1)
	{
		if (!pView->m_mgrProcedure->m_bTHREAD_MK[1])
		{
			pView->m_mgrProcedure->m_nStepMk[1] = 0;
			pView->m_mgrProcedure->m_nMkPcs[1] = 0;
			pView->m_mgrProcedure->m_bDoneMk[1] = FALSE;
			//m_bReMark[1] = FALSE;
			pView->m_mgrProcedure->m_bTHREAD_MK[1] = TRUE;

			nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

			nTot = GetTotDefPcs(nSerial);
			if (nTot>0)
			{
				ptPnt = GetMkPnt(nSerial, 0);
				pView->m_mgrProcedure->m_dTarget[AXIS_X1] = ptPnt.x;
				pView->m_mgrProcedure->m_dTarget[AXIS_Y1] = ptPnt.y;
				if (nTot>1)
				{
					ptPnt = GetMkPnt(nSerial, 1);
					pView->m_mgrProcedure->m_dNextTarget[AXIS_X1] = ptPnt.x;
					pView->m_mgrProcedure->m_dNextTarget[AXIS_Y1] = ptPnt.y;
				}
				else
				{
					pView->m_mgrProcedure->m_dNextTarget[AXIS_X1] = -1.0;
					pView->m_mgrProcedure->m_dNextTarget[AXIS_Y1] = -1.0;
				}
			}
			else
			{
				pView->m_mgrProcedure->m_dTarget[AXIS_X1] = -1.0;
				pView->m_mgrProcedure->m_dTarget[AXIS_Y1] = -1.0;
				pView->m_mgrProcedure->m_dNextTarget[AXIS_X1] = -1.0;
				pView->m_mgrProcedure->m_dNextTarget[AXIS_Y1] = -1.0;
			}
		}
	}
}

BOOL CGvisR2R_PunchView::SetMk(BOOL bRun)	// Marking Start
{
	CfPoint ptPnt;
	int nSerial, nTot, a, b;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	for (a = 0; a<2; a++)
	{
		for (b = 0; b<MAX_STRIP_NUM; b++)
		{
			pView->m_mgrProcedure->m_nMkStrip[a][b] = 0;			// [nCam][nStrip]:[2][4] - [좌/우][] : 스트립에 펀칭한 피스 수 count
			pView->m_mgrProcedure->m_bRejectDone[a][b] = FALSE;	// [nCam][nStrip]:[2][4] - [좌/우][] : 스트립에 펀칭한 피스 수 count가 스트립 폐기 설정수 완료 여부 
		}
	}

	if (bRun)
	{
		if (pView->m_mgrProcedure->m_bDoMk[0])
		{
			if (!pView->m_mgrProcedure->m_bTHREAD_MK[0])
			{
				pView->m_mgrProcedure->m_nStepMk[0] = 0;
				pView->m_mgrProcedure->m_nMkPcs[0] = 0;
				pView->m_mgrProcedure->m_bDoneMk[0] = FALSE;
				//m_bReMark[0] = FALSE;
				pView->m_mgrProcedure->m_bTHREAD_MK[0] = TRUE;

				nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Cam0

				pView->m_mgrProcedure->m_nTotMk[0] = nTot = GetTotDefPcs(nSerial);
				pView->m_mgrProcedure->m_nCurMk[0] = 0;
				if (nTot>0)
				{
					ptPnt = GetMkPnt(nSerial, 0);
					pView->m_mgrProcedure->m_dTarget[AXIS_X0] = ptPnt.x;
					pView->m_mgrProcedure->m_dTarget[AXIS_Y0] = ptPnt.y;
					if (nTot>1)
					{
						ptPnt = GetMkPnt(nSerial, 1);
						pView->m_mgrProcedure->m_dNextTarget[AXIS_X0] = ptPnt.x;
						pView->m_mgrProcedure->m_dNextTarget[AXIS_Y0] = ptPnt.y;
					}
					else
					{
						pView->m_mgrProcedure->m_dNextTarget[AXIS_X0] = -1.0;
						pView->m_mgrProcedure->m_dNextTarget[AXIS_Y0] = -1.0;
					}
				}
				else
				{
					pView->m_mgrProcedure->m_dTarget[AXIS_X0] = -1.0;
					pView->m_mgrProcedure->m_dTarget[AXIS_Y0] = -1.0;
					pView->m_mgrProcedure->m_dNextTarget[AXIS_X0] = -1.0;
					pView->m_mgrProcedure->m_dNextTarget[AXIS_Y0] = -1.0;
				}
			}
		}

		if (pView->m_mgrProcedure->m_bDoMk[1])
		{
			if (!pView->m_mgrProcedure->m_bTHREAD_MK[1])
			{
				pView->m_mgrProcedure->m_nStepMk[1] = 0;
				pView->m_mgrProcedure->m_nMkPcs[1] = 0;
				pView->m_mgrProcedure->m_bDoneMk[1] = FALSE;
				//m_bReMark[1] = FALSE;
				pView->m_mgrProcedure->m_bTHREAD_MK[1] = TRUE;

				nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

				pView->m_mgrProcedure->m_nTotMk[1] = nTot = GetTotDefPcs(nSerial);
				pView->m_mgrProcedure->m_nCurMk[1] = 0;
				if (nTot>0)
				{
					ptPnt = GetMkPnt(nSerial, 0);
					pView->m_mgrProcedure->m_dTarget[AXIS_X1] = ptPnt.x;
					pView->m_mgrProcedure->m_dTarget[AXIS_Y1] = ptPnt.y;
					if (nTot>1)
					{
						ptPnt = GetMkPnt(nSerial, 1);
						pView->m_mgrProcedure->m_dNextTarget[AXIS_X1] = ptPnt.x;
						pView->m_mgrProcedure->m_dNextTarget[AXIS_Y1] = ptPnt.y;
					}
					else
					{
						pView->m_mgrProcedure->m_dNextTarget[AXIS_X1] = -1.0;
						pView->m_mgrProcedure->m_dNextTarget[AXIS_Y1] = -1.0;
					}
				}
				else
				{
					pView->m_mgrProcedure->m_dTarget[AXIS_X1] = -1.0;
					pView->m_mgrProcedure->m_dTarget[AXIS_Y1] = -1.0;
					pView->m_mgrProcedure->m_dNextTarget[AXIS_X1] = -1.0;
					pView->m_mgrProcedure->m_dNextTarget[AXIS_Y1] = -1.0;
				}
			}
		}
	}
	else
	{
		pView->m_mgrProcedure->m_bTHREAD_MK[0] = FALSE;
		pView->m_mgrProcedure->m_bTHREAD_MK[1] = FALSE;
	}
	return TRUE;
}

BOOL CGvisR2R_PunchView::IsReMk()
{
	if (IsMoveDone())
		return FALSE;

	if (pView->m_mgrProcedure->m_bReMark[0] && pView->m_mgrProcedure->m_bDoneMk[1])
	{
		pView->m_mgrProcedure->m_bReMark[0] = FALSE;
		SetReMk(TRUE, FALSE);
		return TRUE;
	}
	else if (pView->m_mgrProcedure->m_bDoneMk[0] && pView->m_mgrProcedure->m_bReMark[1])
	{
		pView->m_mgrProcedure->m_bReMark[1] = FALSE;
		SetReMk(FALSE, TRUE);
		return TRUE;
	}
	else if (pView->m_mgrProcedure->m_bReMark[0] && pView->m_mgrProcedure->m_bReMark[1])
	{
		pView->m_mgrProcedure->m_bReMark[0] = FALSE;
		pView->m_mgrProcedure->m_bReMark[1] = FALSE;
		SetReMk(TRUE, TRUE);
		return TRUE;
	}

	return FALSE;
}

BOOL CGvisR2R_PunchView::IsMkDone()
{
	if (pView->m_mgrProcedure->m_bDoneMk[0] && pView->m_mgrProcedure->m_bDoneMk[1] && !pView->m_mgrProcedure->m_bTHREAD_DISP_DEF)
		return TRUE;

	return FALSE;
}

int CGvisR2R_PunchView::GetSerial()
{
	int nSerial = 0;
	if (m_pDlgMenu01)
		nSerial = m_pDlgMenu01->GetSerial();
	return nSerial;
}

void CGvisR2R_PunchView::SetMkFdLen()
{
	int nLast = pDoc->GetLastShotMk();
	pDoc->WorkingInfo.Motion.sMkFdTotLen.Format(_T("%.1f"), (double)nLast * _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen));

	CString sData, sPath = PATH_WORKING_INFO;
	sData = pDoc->WorkingInfo.Motion.sMkFdTotLen;
	::WritePrivateProfileString(_T("Motion"), _T("MARKING_FEEDING_SERVO_TOTAL_DIST"), sData, sPath);
}

double CGvisR2R_PunchView::GetMkFdLen()
{
	int nLast = pDoc->GetLastShotMk();
	double dLen = (double)nLast * _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen);
	return dLen;
}

double CGvisR2R_PunchView::GetAoiUpFdLen()
{
	int nLast = pDoc->GetLastShotUp();
	double dLen = (double)nLast * _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen);
	return dLen;
}

double CGvisR2R_PunchView::GetAoiDnFdLen()
{
	int nLast = pDoc->GetLastShotDn();
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
		nDiff = (GetTickCount() - pView->m_mgrProcedure->m_dwLotSt) / 1000;
		nHour = int(nDiff / 3600);
		nMin = int((nDiff - 3600 * nHour) / 60);
		nSec = nDiff % 60;
	}
	else
	{
		nDiff = (pView->m_mgrProcedure->m_dwLotEd - pView->m_mgrProcedure->m_dwLotSt) / 1000;
		nHour = int(nDiff / 3600);
		nMin = int((nDiff - 3600 * nHour) / 60);
		nSec = nDiff % 60;
	}

	nTotSec = nHour * 3600 + nMin * 60 + nSec;
	double dVel = 0.0;
	if (nTotSec > 0)
		dVel = dMkFdLen / (double)nTotSec; // [mm/sec]

	pView->m_mgrProcedure->m_dTotVel = dVel;
	return dVel;
}

double CGvisR2R_PunchView::GetPartVel()
{
	double dLen = _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen) * 2.0;
	double dSec = (double)GetCycTime() / 1000.0;
	double dVel = 0.0;
	if (dSec > 0.0)
		dVel = dLen / dSec; // [mm/sec]
	pView->m_mgrProcedure->m_dPartVel = dVel;
	return dVel;
}

BOOL CGvisR2R_PunchView::IsBuffer(int nNum)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (bDualTest)
	{
		if (pView->m_mgrProcedure->m_nBufTot[0] > nNum && pView->m_mgrProcedure->m_nBufTot[1] > nNum) // [0]: AOI-Up , [1]: AOI-Dn
			return TRUE;

		if (pView->m_mgrProcedure->m_bLastProc)
		{
			if ((pView->m_mgrProcedure->m_nBufTot[0] > nNum || !pView->m_mgrProcedure->m_bIsBuf[0])
				&& (pView->m_mgrProcedure->m_nBufTot[1] > nNum || !pView->m_mgrProcedure->m_bIsBuf[1])) // [0]: AOI-Up , [1]: AOI-Dn
				return TRUE;
		}
	}
	else
	{
		if (pView->m_mgrProcedure->m_nBufTot[0] > nNum) // [0]: AOI-Up
			return TRUE;
	}

	return FALSE;
}

BOOL CGvisR2R_PunchView::IsBufferUp()
{
	if (pView->m_mgrProcedure->m_nBufTot[0] > 0)
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_PunchView::IsBufferDn()
{
	if (pView->m_mgrProcedure->m_nBufTot[1] > 0)
		return TRUE;
	return FALSE;
}

int CGvisR2R_PunchView::GetBuffer(int *pPrevSerial)
{
	int nS0 = GetBufferUp(pPrevSerial);
	int nS1 = GetBufferDn(pPrevSerial);
	if (nS0 != nS1)
		return 0;
	return nS0;
}

int CGvisR2R_PunchView::GetBufferUp(int *pPrevSerial)
{
	if (IsBufferUp())
		return pView->m_mgrProcedure->m_pBufSerial[0][0];
	else if (pPrevSerial)
		*pPrevSerial = pView->m_mgrProcedure->m_pBufSerial[0][0];
	return 0;
}

int CGvisR2R_PunchView::GetBufferDn(int *pPrevSerial)
{
	if (IsBufferDn())
		return pView->m_mgrProcedure->m_pBufSerial[1][0];
	else if (pPrevSerial)
		*pPrevSerial = pView->m_mgrProcedure->m_pBufSerial[1][0];
	return 0;
}

BOOL CGvisR2R_PunchView::IsBuffer0()
{
	if (pView->m_mgrProcedure->m_nBufTot[0] > 0 && pView->m_mgrProcedure->m_nBufTot[1] > 0)
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_PunchView::IsBufferUp0()
{
	if (pView->m_mgrProcedure->m_nBufTot[0] > 0)
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_PunchView::IsBufferDn0()
{
	if (pView->m_mgrProcedure->m_nBufTot[1] > 0)
		return TRUE;
	return FALSE;
}

int CGvisR2R_PunchView::GetBuffer0(int *pPrevSerial)
{
	int nS0 = GetBufferUp0(pPrevSerial);
	int nS1 = GetBufferDn0(pPrevSerial);
	if (nS0 != nS1)
		return 0;
	return nS0;
}

int CGvisR2R_PunchView::GetBufferUp0(int *pPrevSerial)
{
	if (IsBufferUp0())
		return pView->m_mgrProcedure->m_pBufSerial[0][0];
	else if (pPrevSerial)
		*pPrevSerial = pView->m_mgrProcedure->m_pBufSerial[0][0];
	return 0;
}

int CGvisR2R_PunchView::GetBufferDn0(int *pPrevSerial)
{
	if (IsBufferDn0())
		return pView->m_mgrProcedure->m_pBufSerial[1][0];
	else if (pPrevSerial)
		*pPrevSerial = pView->m_mgrProcedure->m_pBufSerial[1][0];
	return 0;
}

BOOL CGvisR2R_PunchView::IsBuffer1()
{
	if (pView->m_mgrProcedure->m_nBufTot[0] > 1 && pView->m_mgrProcedure->m_nBufTot[1] > 1)
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_PunchView::IsBufferUp1()
{
	if (pView->m_mgrProcedure->m_nBufTot[0] > 1)
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_PunchView::IsBufferDn1()
{
	if (pView->m_mgrProcedure->m_nBufTot[1] > 1)
		return TRUE;
	return FALSE;
}

int CGvisR2R_PunchView::GetBuffer1(int *pPrevSerial)
{
	int nS0 = GetBufferUp1(pPrevSerial);
	int nS1 = GetBufferDn1(pPrevSerial);
	if (nS0 != nS1)
		return 0;
	return nS0;
}

int CGvisR2R_PunchView::GetBufferUp1(int *pPrevSerial)
{
	if (IsBufferUp1())
		return pView->m_mgrProcedure->m_pBufSerial[0][1];
	else if (pPrevSerial)
		*pPrevSerial = pView->m_mgrProcedure->m_pBufSerial[0][1];
	return 0;
}

int CGvisR2R_PunchView::GetBufferDn1(int *pPrevSerial)
{
	if (IsBufferDn1())
		return pView->m_mgrProcedure->m_pBufSerial[1][1];
	else if (pPrevSerial)
		*pPrevSerial = pView->m_mgrProcedure->m_pBufSerial[1][1];
	return 0;
}

BOOL CGvisR2R_PunchView::IsShare()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (bDualTest)
	{
		if (pView->m_mgrProcedure->m_bWaitPcr[0] && pView->m_mgrProcedure->m_bWaitPcr[1])
		{
			if (IsShareUp() && IsShareDn())
			{
				pView->m_mgrProcedure->m_bWaitPcr[0] = FALSE;
				pView->m_mgrProcedure->m_bWaitPcr[1] = FALSE;
				return TRUE;
			}
		}
		else if (pView->m_mgrProcedure->m_bWaitPcr[0] && !pView->m_mgrProcedure->m_bWaitPcr[1])
		{
			if (IsShareUp())
			{
				pView->m_mgrProcedure->m_bWaitPcr[0] = FALSE;
				return TRUE;
			}
		}
		else if (!pView->m_mgrProcedure->m_bWaitPcr[0] && pView->m_mgrProcedure->m_bWaitPcr[1])
		{
			if (IsShareDn())
			{
				pView->m_mgrProcedure->m_bWaitPcr[1] = FALSE;
				return TRUE;
			}
		}
		else
		{
			if (IsShareUp() || IsShareDn())
				return TRUE;
		}
	}
	else
	{
		if (pView->m_mgrProcedure->m_bWaitPcr[0])
		{
			if (IsShareUp())
			{
				pView->m_mgrProcedure->m_bWaitPcr[0] = FALSE;
				return TRUE;
			}
		}
		else
		{
			if (IsShareUp())
				return TRUE;
		}
	}
	return FALSE;
}

BOOL CGvisR2R_PunchView::IsShareUp()
{
	return pDoc->Status.PcrShare[0].bExist;
}

BOOL CGvisR2R_PunchView::IsShareDn()
{
	return pDoc->Status.PcrShare[1].bExist;
}

BOOL CGvisR2R_PunchView::IsVsShare()
{
	int nSerial;
	BOOL bExist = pView->m_mgrProcedure->ChkVsShare(nSerial);
	return bExist;
}

int CGvisR2R_PunchView::GetShareUp()
{
	return pDoc->Status.PcrShare[0].nSerial;
}

int CGvisR2R_PunchView::GetShareDn()
{
	return pDoc->Status.PcrShare[1].nSerial;
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

BOOL CGvisR2R_PunchView::ChkLotEnd(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.25"));
		return FALSE;
	}

	BOOL bEnd0 = ChkLotEndUp(nSerial);
	BOOL bEnd1 = ChkLotEndDn(nSerial);
	if (bEnd0 || bEnd1)
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_PunchView::ChkLotEndUp(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.26"));
		return 0;
	}

	CString sPath;
	sPath.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufUp, nSerial);
	return pDoc->ChkLotEnd(sPath);
}

BOOL CGvisR2R_PunchView::ChkLotEndDn(int nSerial)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return FALSE;

	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.27"));
		return 0;
	}

	CString sPath;
	sPath.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufDn, nSerial);
	return pDoc->ChkLotEnd(sPath);
}

BOOL CGvisR2R_PunchView::IsMkTmpStop()
{
	return pView->m_mgrProcedure->m_bMkTmpStop;
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

	for (kk = 0; kk < 10; kk++)
	{
		pView->m_mgrProcedure->m_bDispMsgDoAuto[kk] = FALSE;
		pView->m_mgrProcedure->m_nStepDispMsg[kk] = 0;
	}
	pView->m_mgrProcedure->m_sFixMsg[0] = _T("");
	pView->m_mgrProcedure->m_sFixMsg[1] = _T("");

	pView->m_mgrProcedure->m_bReadyDone = FALSE;
	pView->m_mgrProcedure->m_bChkLastProcVs = FALSE;
	pView->m_mgrProcedure->m_nDummy[0] = 0;
	pView->m_mgrProcedure->m_nDummy[1] = 0;
	pView->m_mgrProcedure->m_nAoiLastSerial[0] = 0;
	pView->m_mgrProcedure->m_nAoiLastSerial[1] = 0;
	pView->m_mgrProcedure->m_nStepAuto = 0;
	pView->m_mgrProcedure->m_nPrevStepAuto = 0;
	pView->m_mgrProcedure->m_nPrevMkStAuto = 0;
	pView->m_mgrProcedure->m_bAoiLdRun = TRUE;
	pView->m_mgrProcedure->m_bAoiLdRunF = FALSE;
	pView->m_mgrProcedure->m_bNewModel = FALSE;
	pView->m_mgrProcedure->m_nLotEndSerial = 0;
	if(m_pDlgMenu01)
		m_pDlgMenu01->DispLotEndSerial(0);
	pView->m_mgrProcedure->m_bCam = FALSE;
	pView->m_mgrProcedure->m_bReview = FALSE;

	pView->m_mgrProcedure->m_bChkBufIdx[0] = TRUE;
	pView->m_mgrProcedure->m_bChkBufIdx[1] = TRUE;

	pView->m_mgrProcedure->m_nErrCnt = 0;

	pView->m_mgrProcedure->m_nStepMk[0] = 0;
	pView->m_mgrProcedure->m_nStepMk[1] = 0;
	pView->m_mgrProcedure->m_nStepMk[2] = 0;
	pView->m_mgrProcedure->m_nStepMk[3] = 0;
	pView->m_mgrProcedure->m_bTHREAD_MK[0] = FALSE;
	pView->m_mgrProcedure->m_bTHREAD_MK[1] = FALSE;
	pView->m_mgrProcedure->m_bTHREAD_MK[2] = FALSE;
	pView->m_mgrProcedure->m_bTHREAD_MK[3] = FALSE;
	pView->m_mgrProcedure->m_nMkPcs[0] = 0;
	pView->m_mgrProcedure->m_nMkPcs[1] = 0;
	pView->m_mgrProcedure->m_nMkPcs[2] = 0;
	pView->m_mgrProcedure->m_nMkPcs[3] = 0;

	pView->m_mgrProcedure->m_bMkTmpStop = FALSE;

	pView->m_mgrProcedure->m_bWaitPcr[0] = FALSE;
	pView->m_mgrProcedure->m_bWaitPcr[1] = FALSE;


	pView->m_mgrProcedure->m_nShareUpS = 0;
	pView->m_mgrProcedure->m_nShareUpSerial[0] = 0;
	pView->m_mgrProcedure->m_nShareUpSerial[1] = 0;
	pView->m_mgrProcedure->m_nShareUpCnt = 0;

	pView->m_mgrProcedure->m_nShareDnS = 0;
	pView->m_mgrProcedure->m_nShareDnSerial[0] = 0;
	pView->m_mgrProcedure->m_nShareDnSerial[1] = 0;
	pView->m_mgrProcedure->m_nShareDnCnt = 0;

	pView->m_mgrProcedure->m_nBufUpSerial[0] = 0;
	pView->m_mgrProcedure->m_nBufUpSerial[1] = 0;
	pView->m_mgrProcedure->m_nBufUpCnt = 0;

	pView->m_mgrProcedure->m_nBufDnSerial[0] = 0;
	pView->m_mgrProcedure->m_nBufDnSerial[1] = 0;
	pView->m_mgrProcedure->m_nBufDnCnt = 0;

	for (nCam = 0; nCam < 2; nCam++)
	{
		for (nPoint = 0; nPoint < 4; nPoint++)
		{
			m_pDlgMenu02->m_dMkFdOffsetX[nCam][nPoint] = 0.0;
			m_pDlgMenu02->m_dMkFdOffsetY[nCam][nPoint] = 0.0;
		}
	}


	m_pDlgMenu02->m_dAoiUpFdOffsetX = 0.0;
	m_pDlgMenu02->m_dAoiUpFdOffsetY = 0.0;
	m_pDlgMenu02->m_dAoiDnFdOffsetX = 0.0;
	m_pDlgMenu02->m_dAoiDnFdOffsetY = 0.0;

	pView->m_mgrProcedure->m_bReAlign[0][0] = FALSE;	// [nCam][nPos]
	pView->m_mgrProcedure->m_bReAlign[0][1] = FALSE;	// [nCam][nPos]
	pView->m_mgrProcedure->m_bReAlign[0][2] = FALSE;	// [nCam][nPos]
	pView->m_mgrProcedure->m_bReAlign[0][3] = FALSE;	// [nCam][nPos]
	pView->m_mgrProcedure->m_bReAlign[1][0] = FALSE;	// [nCam][nPos]
	pView->m_mgrProcedure->m_bReAlign[1][1] = FALSE;	// [nCam][nPos]
	pView->m_mgrProcedure->m_bReAlign[1][2] = FALSE;	// [nCam][nPos]
	pView->m_mgrProcedure->m_bReAlign[1][3] = FALSE;	// [nCam][nPos]

	pView->m_mgrProcedure->m_bSkipAlign[0][0] = FALSE;	// [nCam][nPos]
	pView->m_mgrProcedure->m_bSkipAlign[0][1] = FALSE;	// [nCam][nPos]
	pView->m_mgrProcedure->m_bSkipAlign[0][2] = FALSE;	// [nCam][nPos]
	pView->m_mgrProcedure->m_bSkipAlign[0][3] = FALSE;	// [nCam][nPos]
	pView->m_mgrProcedure->m_bSkipAlign[1][0] = FALSE;	// [nCam][nPos]
	pView->m_mgrProcedure->m_bSkipAlign[1][1] = FALSE;	// [nCam][nPos]
	pView->m_mgrProcedure->m_bSkipAlign[1][2] = FALSE;	// [nCam][nPos]
	pView->m_mgrProcedure->m_bSkipAlign[1][3] = FALSE;	// [nCam][nPos]

	pView->m_mgrProcedure->m_bFailAlign[0][0] = FALSE;	// [nCam][nPos]
	pView->m_mgrProcedure->m_bFailAlign[0][1] = FALSE;	// [nCam][nPos]
	pView->m_mgrProcedure->m_bFailAlign[0][2] = FALSE;	// [nCam][nPos]
	pView->m_mgrProcedure->m_bFailAlign[0][3] = FALSE;	// [nCam][nPos]
	pView->m_mgrProcedure->m_bFailAlign[1][0] = FALSE;	// [nCam][nPos]
	pView->m_mgrProcedure->m_bFailAlign[1][1] = FALSE;	// [nCam][nPos]
	pView->m_mgrProcedure->m_bFailAlign[1][2] = FALSE;	// [nCam][nPos]
	pView->m_mgrProcedure->m_bFailAlign[1][3] = FALSE;	// [nCam][nPos]

	pView->m_mgrProcedure->m_bDoMk[0] = TRUE;			// [nCam]
	pView->m_mgrProcedure->m_bDoMk[1] = TRUE;			// [nCam]
	pView->m_mgrProcedure->m_bDoneMk[0] = FALSE;		// [nCam]
	pView->m_mgrProcedure->m_bDoneMk[1] = FALSE;		// [nCam]
	pView->m_mgrProcedure->m_bReMark[0] = FALSE;		// [nCam]
	pView->m_mgrProcedure->m_bReMark[1] = FALSE;		// [nCam]

	pView->m_mgrProcedure->m_nTotMk[0] = 0;
	pView->m_mgrProcedure->m_nCurMk[0] = 0;
	pView->m_mgrProcedure->m_nTotMk[1] = 0;
	pView->m_mgrProcedure->m_nCurMk[1] = 0;
	pView->m_mgrProcedure->m_nPrevTotMk[0] = 0;
	pView->m_mgrProcedure->m_nPrevCurMk[0] = 0;
	pView->m_mgrProcedure->m_nPrevTotMk[1] = 0;
	pView->m_mgrProcedure->m_nPrevCurMk[1] = 0;


	pView->m_mgrProcedure->m_bMkSt = FALSE;
	::WritePrivateProfileString(_T("Last Job"), _T("MkSt"), _T("0"), PATH_WORKING_INFO);
	pView->m_mgrProcedure->m_bMkStSw = FALSE;
	pView->m_mgrProcedure->m_nMkStAuto = 0;

	pView->m_mgrProcedure->m_bEngSt = FALSE;
	pView->m_mgrProcedure->m_bEngStSw = FALSE;
	pView->m_mgrProcedure->m_nEngStAuto = 0;

	pView->m_mgrProcedure->m_bEng2dSt = FALSE;
	pView->m_mgrProcedure->m_bEng2dStSw = FALSE;
	pView->m_mgrProcedure->m_nEng2dStAuto = 0;

	pView->m_mgrProcedure->m_bLotEnd = FALSE;
	pView->m_mgrProcedure->m_nLotEndAuto = 0;

	pView->m_mgrProcedure->m_bLastProc = FALSE;
	if(MODE_INNER != pDoc->GetTestMode())
		pView->m_mgrProcedure->m_bLastProcFromUp = TRUE;
	else
		pView->m_mgrProcedure->m_bLastProcFromEng = TRUE;

	pView->m_mgrProcedure->m_nLastProcAuto = 0;

	pDoc->m_sAlmMsg = _T("");
	pDoc->m_sIsAlmMsg = _T("");
	pDoc->m_sPrevAlmMsg = _T("");

	pView->m_mgrProcedure->m_dwCycSt = 0;
	pView->m_mgrProcedure->m_sNewLotUp = _T("");
	pView->m_mgrProcedure->m_sNewLotDn = _T("");

	pView->m_mgrProcedure->m_nStop = 0;

	pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF = 0;
	pView->m_mgrProcedure->m_bTHREAD_DISP_DEF = FALSE;				// CopyDefImg Stop
	pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF_INNER = 0;
	pView->m_mgrProcedure->m_bTHREAD_DISP_DEF_INNER = FALSE;		// DispDefImg Stop

	pDoc->SetMkMenu01(_T("Signal"), _T("DispDefImg"), _T("0"));

	for (a = 0; a<2; a++)
	{
		for (b = 0; b<4; b++)
		{
			pView->m_mgrProcedure->m_nMkStrip[a][b] = 0;
			pView->m_mgrProcedure->m_bRejectDone[a][b] = FALSE;
		}
	}

	m_pMpe->Write(_T("MB440100"), 0); // PLC 운전준비 완료(PC가 확인하고 Reset시킴.)
	m_pMpe->Write(_T("MB440110"), 0); // 마킹시작(PC가 확인하고 Reset시킴.)-20141029
	m_pMpe->Write(_T("MB440150"), 0); // 마킹부 마킹중 ON (PC가 ON, OFF)
	m_pMpe->Write(_T("MB440170"), 0); // 마킹완료(PLC가 확인하고 Reset시킴.)-20141029

	InitAutoEngSignal();

	MoveInitPos1();
	Sleep(30);
	MoveInitPos0();

	InitIoWrite();
	SetTest(FALSE);	// 검사부 상/하 검사 시작 (Off)
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


	//m_bTHREAD_UPDATAE_YIELD[0] = FALSE;
	//m_bTHREAD_UPDATAE_YIELD[1] = FALSE;
	//m_nSerialTHREAD_UPDATAE_YIELD[0] = 0;
	//m_nSerialTHREAD_UPDATAE_YIELD[1] = 0;

	pView->m_mgrProcedure->InitVal();

	pDoc->m_nEjectBufferLastShot = -1;
	pView->m_mgrProcedure->m_bSerialDecrese = FALSE;
	pView->m_mgrProcedure->m_bStopF_Verify = TRUE;
	pView->m_mgrProcedure->m_bInitAuto = TRUE;
	pView->m_mgrProcedure->m_bInitAutoLoadMstInfo = TRUE;

	pDoc->m_bUpdateForNewJob[0] = FALSE;
	pDoc->m_bUpdateForNewJob[1] = FALSE;

	if (bInit) // 이어가기가 아닌경우.
	{
		m_pMpe->Write(_T("MB440187"), 0); // 이어가기(PC가 On시키고, PLC가 확인하고 Off시킴)-20141121

		pView->m_mgrProcedure->m_nRstNum = 0;
		pView->m_mgrProcedure->m_bCont = FALSE;
		pView->m_mgrProcedure->m_dTotVel = 0.0;
		pView->m_mgrProcedure->m_dPartVel = 0.0;
		pView->m_mgrProcedure->m_dwCycSt = 0;
		pView->m_mgrProcedure->m_dwCycTim = 0;

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

		pView->m_mgrProcedure->m_nDebugStep = 24; pView->DispThreadTick();
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
		m_pMpe->Write(_T("MB440187"), 1); // 이어가기(PC가 On시키고, PLC가 확인하고 Off시킴)-20141121
		pView->m_mgrReelmap->ClrFixPcs();

#ifndef TEST_MODE
		pView->m_mgrReelmap->ReloadReelmap();
		UpdateRst();
#endif
		DispLotStTime();
		pView->m_mgrReelmap->RestoreReelmap();
	}
}

void CGvisR2R_PunchView::SetListBuf()	// pDoc->m_ListBuf에 버퍼 폴더의 시리얼번호를 가지고 재갱신함.
{
	pDoc->m_ListBuf[0].Clear();
	if (pView->m_mgrProcedure->ChkBufUp(pView->m_mgrProcedure->m_pBufSerial[0], pView->m_mgrProcedure->m_nBufTot[0]))
	{
		for (int i = 0; i<pView->m_mgrProcedure->m_nBufTot[0]; i++)
			pDoc->m_ListBuf[0].Push(pView->m_mgrProcedure->m_pBufSerial[0][i]);
	}

	pDoc->m_ListBuf[1].Clear();
	if (pView->m_mgrProcedure->ChkBufDn(pView->m_mgrProcedure->m_pBufSerial[1], pView->m_mgrProcedure->m_nBufTot[1]))
	{
		for (int i = 0; i<pView->m_mgrProcedure->m_nBufTot[1]; i++)
			pDoc->m_ListBuf[1].Push(pView->m_mgrProcedure->m_pBufSerial[1][i]);
	}
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

	pView->m_mgrProcedure->m_dwLotSt = (DWORD)pDoc->WorkingInfo.Lot.dwStTick;
	DispLotTime();
}

void CGvisR2R_PunchView::ClrMkInfo()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	pView->m_mgrReelmap->ClearReelmap();
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
		if (pView->m_mgrReelmap->GetItsSerialInfo(0, bDualTestInner, sLot, sLayerUp, sLayerDn, 3))
		{
			if (pView->m_mgrReelmap->m_Master[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, sLayerUp))
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
		pView->m_mgrReelmap->OpenReelmapUp(); // At Start...
		pView->m_mgrReelmap->SetPathAtBufUp();
		if (m_pDlgMenu01)
			m_pDlgMenu01->UpdateData();

		pDoc->m_pSpecLocal->MakeDir(pDoc->Status.PcrShare[0].sModel, pDoc->Status.PcrShare[0].sLayer);

		if (pDoc->GetTestMode() == MODE_OUTER)
			pView->m_mgrReelmap->OpenReelmapInner();
	}
	else if (nAoi == 1)
	{
		pDoc->SetModelInfoDn();
		pView->m_mgrReelmap->OpenReelmapDn(); // At Start...
		pView->m_mgrReelmap->SetPathAtBufDn();
		if (pView->m_pDlgMenu01)
			pView->m_pDlgMenu01->UpdateData();
		pDoc->m_pSpecLocal->MakeDir(pDoc->Status.PcrShare[1].sModel, pDoc->Status.PcrShare[1].sLayer);
	}
}

void CGvisR2R_PunchView::ResetMkInfo(int nAoi) // 0 : AOI-Up , 1 : AOI-Dn , 2 : AOI-UpDn
{
	pView->m_mgrReelmap->ResetMkInfo(nAoi);

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	BOOL bDualTestInner, bGetCurrentInfoEng;
	CString sLot, sLayerUp, sLayerDn;
	bGetCurrentInfoEng = pDoc->GetCurrentInfoEng();

	if (nAoi == 0 || nAoi == 2)
	{
		if (!bDualTest)
		{
			pView->m_mgrProcedure->m_bDrawGL = FALSE;
			if (m_pDlgMenu01)
				m_pDlgMenu01->ResetMkInfo();
		}

		if (bGetCurrentInfoEng)
		{
			if (pView->m_mgrReelmap->GetItsSerialInfo(0, bDualTestInner, sLot, sLayerUp, sLayerDn, 3))
			{
				if (pView->m_mgrReelmap->m_Master[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, sLayerUp))
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
			if (m_pEngrave)
				m_pEngrave->SwMenu01UpdateWorking(TRUE);
		}

		SetAlignPosUp();

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
			if (pView->m_mgrReelmap->GetItsSerialInfo(0, bDualTestInner, sLot, sLayerUp, sLayerDn, 0))
			{
				if (pView->m_mgrReelmap->m_Master[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, sLayerUp))
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
				pView->m_mgrProcedure->m_bDrawGL = TRUE;
				m_pDlgMenu01->RefreshRmap();
			}
		}
	}

	if (bDualTest)
	{
		if (nAoi == 1 || nAoi == 2)
		{
			pView->m_mgrProcedure->m_bDrawGL = FALSE;
			if (m_pDlgMenu01)
				m_pDlgMenu01->ResetMkInfo();

			SetAlignPosDn();

			if (m_pDlgMenu02)
				m_pDlgMenu02->ChgModelDn();

			if (m_pDlgMenu01)
			{
				m_pDlgMenu01->InitCadImgDn();
				m_pDlgMenu01->InitGL();
				pView->m_mgrProcedure->m_bDrawGL = TRUE;
				m_pDlgMenu01->RefreshRmap();
			}
		}
	}
}

void CGvisR2R_PunchView::SetAlignPos()
{
	if (m_pMotion)
	{
		CfPoint ptPos1, ptPos2;
		pView->m_mgrReelmap->GetAlignPos(ptPos1, ptPos2);

		m_pMotion->m_dAlignPosX[0][0] = ptPos1.x + pView->m_pMotion->m_dPinPosX[0];
		m_pMotion->m_dAlignPosY[0][0] = ptPos1.y + pView->m_pMotion->m_dPinPosY[0];
		m_pMotion->m_dAlignPosX[0][1] = ptPos2.x + pView->m_pMotion->m_dPinPosX[0];
		m_pMotion->m_dAlignPosY[0][1] = ptPos2.y + pView->m_pMotion->m_dPinPosY[0];

		m_pMotion->m_dAlignPosX[1][0] = ptPos1.x + pView->m_pMotion->m_dPinPosX[1];
		m_pMotion->m_dAlignPosY[1][0] = ptPos1.y + pView->m_pMotion->m_dPinPosY[1];
		m_pMotion->m_dAlignPosX[1][1] = ptPos2.x + pView->m_pMotion->m_dPinPosX[1];
		m_pMotion->m_dAlignPosY[1][1] = ptPos2.y + pView->m_pMotion->m_dPinPosY[1];
	}
}

void CGvisR2R_PunchView::SetAlignPosUp()
{
	if (m_pMotion)
	{
		CfPoint ptPos1, ptPos2;
		pView->m_mgrReelmap->GetAlignPos(ptPos1, ptPos2);

		m_pMotion->m_dAlignPosX[0][0] = ptPos1.x + pView->m_pMotion->m_dPinPosX[0];
		m_pMotion->m_dAlignPosY[0][0] = ptPos1.y + pView->m_pMotion->m_dPinPosY[0];
		m_pMotion->m_dAlignPosX[0][1] = ptPos2.x + pView->m_pMotion->m_dPinPosX[0];
		m_pMotion->m_dAlignPosY[0][1] = ptPos2.y + pView->m_pMotion->m_dPinPosY[0];
	}
}

void CGvisR2R_PunchView::SetAlignPosDn()
{
	if (m_pMotion)
	{
		CfPoint ptPos1, ptPos2;
		pView->m_mgrReelmap->GetAlignPos(ptPos1, ptPos2);

		m_pMotion->m_dAlignPosX[1][0] = ptPos1.x + pView->m_pMotion->m_dPinPosX[1];
		m_pMotion->m_dAlignPosY[1][0] = ptPos1.y + pView->m_pMotion->m_dPinPosY[1];
		m_pMotion->m_dAlignPosX[1][1] = ptPos2.x + pView->m_pMotion->m_dPinPosX[1];
		m_pMotion->m_dAlignPosY[1][1] = ptPos2.y + pView->m_pMotion->m_dPinPosY[1];
	}
}

BOOL CGvisR2R_PunchView::InitMk()
{
	pView->m_mgrProcedure->m_nStepElecChk = 0;
	int nRSer = ChkSerial(); // (nSerial0 - nLastShot) -> 0: Same Serial, -: Decrese Serial, +: Increase Serial 
	int nSerial = GetBuffer0();

	if (nRSer)
	{
		pView->m_mgrProcedure->m_bSerialDecrese = FALSE;

		if (pDoc->m_bUseRTRYShiftAdjust)
		{
			if (nSerial > 2)
				AdjPinPos();
		}

		return TRUE;
	}

	return FALSE;
}

int CGvisR2R_PunchView::GetErrCode(int nSerial) // 1(정상), -1(Align Error, 노광불량), -2(Lot End)
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

int CGvisR2R_PunchView::GetErrCodeUp(int nSerial) // 1(정상), -1(Align Error, 노광불량), -2(Lot End)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.30"));
		return 0;
	}

	return pView->m_mgrReelmap->GetErrCodeUp(nSerial);
}

int CGvisR2R_PunchView::GetErrCodeDn(int nSerial) // 1(정상), -1(Align Error, 노광불량), -2(Lot End)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return 1;

	return pView->m_mgrReelmap->GetErrCodeDn(nSerial);
}

int CGvisR2R_PunchView::GetTotDefPcs(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.38"));
		return 0;
	}

	return pView->m_mgrReelmap->GetTotDefPcs(nSerial);
}

int CGvisR2R_PunchView::GetTotDefPcsUp(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.39"));
		return 0;
	}

	return pView->m_mgrReelmap->GetTotDefPcsUp(nSerial);
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

	return pView->m_mgrReelmap->GetTotDefPcsDn(nSerial);
}

CfPoint CGvisR2R_PunchView::GetMkPnt(int nMkPcs)
{
	return pView->m_mgrReelmap->GetMkPnt(nMkPcs);
}

CfPoint CGvisR2R_PunchView::GetMkPnt(int nSerial, int nMkPcs) // pcr 시리얼, pcr 불량 피스 읽은 순서 인덱스
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.47"));
		return 0;
	}

	return pView->m_mgrReelmap->GetMkPnt(nSerial, nMkPcs);
}

int CGvisR2R_PunchView::GetMkStripIdx(int nDefPcsId) // 0 : Fail , 1~4 : Strip Idx
{
	return pView->m_mgrReelmap->GetMkStripIdx(nDefPcsId);
}

CString CGvisR2R_PunchView::GetMkInfo(int nSerial, int nMkPcs) // return Cam0 : "Serial_Strip_Col_Row"
{
	if (nSerial <= 0)
	{
		AfxMessageBox(_T("Serial Error.48"));
		return _T("");
	}

	return pView->m_mgrReelmap->GetMkInfo(nSerial, nMkPcs);
}

int CGvisR2R_PunchView::GetMkStripIdx(int nSerial, int nMkPcs) // 0 : Fail , 1~4 : Strip Idx
{
	if (nSerial <= 0)
	{
		AfxMessageBox(_T("Serial Error.48"));
		return 0;
	}

	return pView->m_mgrReelmap->GetMkStripIdx(nSerial, nMkPcs);
}

void CGvisR2R_PunchView::Move0(CfPoint pt, BOOL bCam)
{
	if (pView->m_mgrProcedure->m_bProbDn[0])
	{
		if (m_pVoiceCoil[0])
		{
			m_pVoiceCoil[0]->SearchHomeSmac(0);
			m_pVoiceCoil[0]->MoveSmacShiftPos(0);
			pView->m_mgrProcedure->m_bProbDn[0] = FALSE;
			//if (m_pDlgMenu02->m_pDlgUtil06)
			//	m_pDlgMenu02->m_pDlgUtil06->myBtn[2].SetCheck(FALSE);
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

	double dCurrX = pView->m_mgrProcedure->m_dEnc[AXIS_X0];
	double dCurrY = pView->m_mgrProcedure->m_dEnc[AXIS_Y0];
	fLen = sqrt(((pPos[0] - dCurrX) * (pPos[0] - dCurrX)) + ((pPos[1] - dCurrY) * (pPos[1] - dCurrY)));
	if (fLen > 0.001)
	{
		m_pMotion->GetSpeedProfile0(TRAPEZOIDAL, AXIS_X0, fLen, fVel, fAcc, fJerk);
		m_pMotion->Move0(MS_X0Y0, pPos, fVel, fAcc, fAcc, ABS, NO_WAIT);
	}
}

void CGvisR2R_PunchView::Move1(CfPoint pt, BOOL bCam)
{
	if (pView->m_mgrProcedure->m_bProbDn[1])
	{
		if (m_pVoiceCoil[1])
		{
			m_pVoiceCoil[1]->SearchHomeSmac(1);
			m_pVoiceCoil[1]->MoveSmacShiftPos(1);
			pView->m_mgrProcedure->m_bProbDn[1] = FALSE;
			//if (m_pDlgMenu02->m_pDlgUtil06)
			//	m_pDlgMenu02->m_pDlgUtil06->myBtn[6].SetCheck(FALSE);
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

	double dCurrX = pView->m_mgrProcedure->m_dEnc[AXIS_X1];
	double dCurrY = pView->m_mgrProcedure->m_dEnc[AXIS_Y1];
	fLen = sqrt(((pPos[0] - dCurrX) * (pPos[0] - dCurrX)) + ((pPos[1] - dCurrY) * (pPos[1] - dCurrY)));
	if (fLen > 0.001)
	{
		m_pMotion->GetSpeedProfile1(TRAPEZOIDAL, AXIS_X1, fLen, fVel, fAcc, fJerk);
		m_pMotion->Move1(MS_X1Y1, pPos, fVel, fAcc, fAcc, ABS, NO_WAIT);
	}
}

BOOL CGvisR2R_PunchView::IsMoveDone()
{
	if (!m_pMotion)
		return FALSE;

	if (IsMoveDone0() && IsMoveDone1())
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_PunchView::IsMoveDone0()
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

BOOL CGvisR2R_PunchView::IsMoveDone1()
{
	if (!m_pMotion)
		return FALSE;

	if (m_pMotion->IsMotionDone(MS_X1) && m_pMotion->IsMotionDone(MS_Y1))
	{
		//	Sleep(300);
		return TRUE;
	}
	return FALSE;
}

void CGvisR2R_PunchView::Mk0()
{
	if (pView->m_pVoiceCoil[0])
	{
		pView->m_pVoiceCoil[0]->SetMark(0);
		pDoc->AddMkCntL();
		pView->m_mgrProcedure->m_nCurMk[0]++;
	}
}

void CGvisR2R_PunchView::Mk1()
{
	if (pView->m_pVoiceCoil[1])
	{
		pView->m_pVoiceCoil[1]->SetMark(1);
		pDoc->AddMkCntR();
		pView->m_mgrProcedure->m_nCurMk[1]++;
	}
}

void CGvisR2R_PunchView::Ink(BOOL bOn)
{
	if (bOn)
	{
		// Ink Marking ON
		//pDoc->m_pSliceIo[7] |= (0x01<<13);	// 마킹부 마킹 콘트롤러 출력 2(전원공급) -> Sen
	}
	else
	{
		// Ink Marking OFF
		//pDoc->m_pSliceIo[7] &= ~(0x01<<13);	// 마킹부 마킹 콘트롤러 출력 2(전원공급) -> Sen
	}
}

BOOL CGvisR2R_PunchView::LoadPcrUp(int nSerial, BOOL bFromShare)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.52"));
		return 0;
	}

	int nHeadInfo = pView->m_mgrReelmap->LoadPCR0(nSerial); // 2(Failed), 1(정상), -1(Align Error, 노광불량), -2(Lot End)
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

	int nHeadInfo = pView->m_mgrReelmap->LoadPCR1(nSerial); // 2(Failed), 1(정상), -1(Align Error, 노광불량), -2(Lot End)
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
	pView->m_mgrReelmap->InitReelmap();
	pView->m_mgrReelmap->SetReelmap(ROT_NONE);
	//pDoc->SetReelmap(ROT_CCW_90);
	pDoc->UpdateData();
}

void CGvisR2R_PunchView::InitReelmapUp()
{
	pView->m_mgrReelmap->InitReelmapUp();
	pView->m_mgrReelmap->SetReelmap(ROT_NONE);
	//pDoc->SetReelmap(ROT_CCW_90);
	pDoc->UpdateData();
}

void CGvisR2R_PunchView::InitReelmapDn()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (bDualTest)
	{
		pView->m_mgrReelmap->InitReelmapDn();
		pView->m_mgrReelmap->SetReelmap(ROT_NONE);
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

BOOL CGvisR2R_PunchView::IsPinData()
{
	return pDoc->IsPinData();
}

BOOL CGvisR2R_PunchView::CopyDefImg(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.55"));
		return 0;
	}

	return pView->m_mgrReelmap->CopyDefImg(nSerial);
}

BOOL CGvisR2R_PunchView::CopyDefImg(int nSerial, CString sNewLot)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.56"));
		return 0;
	}

	return pView->m_mgrReelmap->CopyDefImg(nSerial, sNewLot);
}

BOOL CGvisR2R_PunchView::CopyDefImgUp(int nSerial, CString sNewLot)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.57"));
		return 0;
	}

	return pView->m_mgrReelmap->CopyDefImgUp(nSerial, sNewLot);
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

	return pView->m_mgrReelmap->CopyDefImgDn(nSerial, sNewLot);
}

BOOL CGvisR2R_PunchView::MovePinPos()
{
	if (!m_pDlgMenu02)
		return FALSE;

	return m_pDlgMenu02->MovePinPos();
}


BOOL CGvisR2R_PunchView::TwoPointAlign0(int nPos)
{
	if (!m_pDlgMenu02)
		return FALSE;

	return m_pDlgMenu02->Do2PtAlign0(nPos);
}

BOOL CGvisR2R_PunchView::TwoPointAlign1(int nPos)
{
	if (!m_pDlgMenu02)
		return FALSE;

	return m_pDlgMenu02->Do2PtAlign1(nPos);
}

BOOL CGvisR2R_PunchView::FourPointAlign0(int nPos)
{
	if (!m_pDlgMenu02)
		return FALSE;

	return m_pDlgMenu02->Do4PtAlign0(nPos);
}

BOOL CGvisR2R_PunchView::FourPointAlign1(int nPos)
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

BOOL CGvisR2R_PunchView::TwoPointAlign() // return FALSE; 인경우 정지.
{
	if (!m_pDlgMenu02)
		return FALSE;

	BOOL bRtn[2];

	// TwoPointAlign(0)
	bRtn[0] = m_pDlgMenu02->TwoPointAlign(0);

	if (!bRtn[0])
		return FALSE;

	// TwoPointAlign(1)
	bRtn[1] = m_pDlgMenu02->TwoPointAlign(1); // Align 보정.

	if (bRtn[0] && bRtn[1])
		return TRUE;

	return FALSE;
}

BOOL CGvisR2R_PunchView::IsNoMk()
{
	BOOL bNoMk = (pDoc->WorkingInfo.System.bNoMk | pView->m_mgrProcedure->m_bCam); // pDoc->WorkingInfo.LastJob.bVerify
	return bNoMk;
}

BOOL CGvisR2R_PunchView::IsNoMk0()
{
	BOOL bNoMk = (pDoc->WorkingInfo.System.bNoMk | pView->m_mgrProcedure->m_bCam); // pDoc->WorkingInfo.LastJob.bVerify
	return bNoMk;
}

BOOL CGvisR2R_PunchView::IsNoMk1()
{
	BOOL bNoMk = (pDoc->WorkingInfo.System.bNoMk | pView->m_mgrProcedure->m_bCam); // pDoc->WorkingInfo.LastJob.bVerify
	return bNoMk;
}

BOOL CGvisR2R_PunchView::IsHomeDone(int nMsId)
{
	if (!m_pMotion)
		return FALSE;

	return m_pMotion->IsHomeDone(nMsId);
}

BOOL CGvisR2R_PunchView::GetAoiUpInfo(int nSerial, int *pNewLot, BOOL bFromBuf)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.59"));
		return 0;
	}

	BOOL bRtn = pView->m_mgrReelmap->GetAoiUpInfo(nSerial, pNewLot, bFromBuf);

	if (pDoc->m_bUpdateForNewJob[0])
	{
		pDoc->m_bUpdateForNewJob[0] = FALSE;

		if (pView->m_pDlgMenu01)
			pView->m_pDlgMenu01->UpdateData();
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

	return pView->m_mgrReelmap->GetAoiDnInfo(nSerial, pNewLot, bFromBuf);

	if (pDoc->m_bUpdateForNewJob[1])
	{
		pDoc->m_bUpdateForNewJob[1] = FALSE;

		if (pView->m_pDlgMenu01)
			pView->m_pDlgMenu01->UpdateData();
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
		OfSt.x = m_pDlgMenu02->m_dMkFdOffsetX[0][0]; // -: 제품 덜옴, +: 제품 더옴.
		OfSt.y = m_pDlgMenu02->m_dMkFdOffsetY[0][0]; // -: 제품 덜옴, +: 제품 더옴.
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

BOOL CGvisR2R_PunchView::IsInitPos0()
{
	if (!m_pMotion)
		return FALSE;

	double pTgtPos[2];
	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[0]);
	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[0]);
	double dCurrX = pView->m_mgrProcedure->m_dEnc[AXIS_X0];
	double dCurrY = pView->m_mgrProcedure->m_dEnc[AXIS_Y0];

	if (dCurrX < pTgtPos[0] - 2.0 || dCurrX > pTgtPos[0] + 2.0)
		return FALSE;
	if (dCurrY < pTgtPos[1] - 2.0 || dCurrY > pTgtPos[1] + 2.0)
		return FALSE;

	return TRUE;
}

void CGvisR2R_PunchView::MoveInitPos0(BOOL bWait)
{
	if (!m_pMotion)
		return;

	double pTgtPos[2];
	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[0]);
	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[0]);
	double dCurrX = pView->m_mgrProcedure->m_dEnc[AXIS_X0];
	double dCurrY = pView->m_mgrProcedure->m_dEnc[AXIS_Y0];

	double fLen, fVel, fAcc, fJerk;
	fLen = sqrt(((pTgtPos[0] - dCurrX) * (pTgtPos[0] - dCurrX)) + ((pTgtPos[1] - dCurrY) * (pTgtPos[1] - dCurrY)));
	if (fLen > 0.001)
	{
		m_pMotion->GetSpeedProfile0(TRAPEZOIDAL, AXIS_X0, fLen, fVel, fAcc, fJerk);
		if(bWait)
			m_pMotion->Move0(MS_X0Y0, pTgtPos, fVel, fAcc, fAcc, ABS, WAIT);
		else
			m_pMotion->Move0(MS_X0Y0, pTgtPos, fVel, fAcc, fAcc, ABS, NO_WAIT);
	}
}

BOOL CGvisR2R_PunchView::IsInitPos1()
{
	if (!m_pMotion)
		return FALSE;

	double pTgtPos[2];
	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[1]);
	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[1]);
	double dCurrX = pView->m_mgrProcedure->m_dEnc[AXIS_X1];
	double dCurrY = pView->m_mgrProcedure->m_dEnc[AXIS_Y1];

	if (dCurrX < pTgtPos[0] - 2.0 || dCurrX > pTgtPos[0] + 2.0)
		return FALSE;
	if (dCurrY < pTgtPos[1] - 2.0 || dCurrY > pTgtPos[1] + 2.0)
		return FALSE;

	return TRUE;
}

BOOL CGvisR2R_PunchView::IsMkEdPos1()
{
	if (!m_pMotion)
		return FALSE;

	double pTgtPos[2];
	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sMkEdPosX[1]);
	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sMkEdPosY[1]);
	double dCurrX = pView->m_mgrProcedure->m_dEnc[AXIS_X1];
	double dCurrY = pView->m_mgrProcedure->m_dEnc[AXIS_Y1];

	if (dCurrX < pTgtPos[0] - 2.0 || dCurrX > pTgtPos[0] + 2.0)
		return FALSE;
	if (dCurrY < pTgtPos[1] - 2.0 || dCurrY > pTgtPos[1] + 2.0)
		return FALSE;

	return TRUE;
}

void CGvisR2R_PunchView::MoveInitPos1(BOOL bWait)
{
	if (!m_pMotion)
		return;

	double pTgtPos[2];
	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[1]);
	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[1]);
	double dCurrX = pView->m_mgrProcedure->m_dEnc[AXIS_X1];
	double dCurrY = pView->m_mgrProcedure->m_dEnc[AXIS_Y1];

	double fLen, fVel, fAcc, fJerk;
	fLen = sqrt(((pTgtPos[0] - dCurrX) * (pTgtPos[0] - dCurrX)) + ((pTgtPos[1] - dCurrY) * (pTgtPos[1] - dCurrY)));
	if (fLen > 0.001)
	{
		m_pMotion->GetSpeedProfile1(TRAPEZOIDAL, AXIS_X1, fLen, fVel, fAcc, fJerk);
		if(bWait)
			m_pMotion->Move1(MS_X1Y1, pTgtPos, fVel, fAcc, fAcc, ABS, WAIT);
		else
			m_pMotion->Move1(MS_X1Y1, pTgtPos, fVel, fAcc, fAcc, ABS, NO_WAIT);
	}
}

void CGvisR2R_PunchView::MoveMkEdPos1()
{
	if (!m_pMotion)
		return;

	double pTgtPos[2];
	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sMkEdPosX[1]);
	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sMkEdPosY[1]);
	double dCurrX = pView->m_mgrProcedure->m_dEnc[AXIS_X1];
	double dCurrY = pView->m_mgrProcedure->m_dEnc[AXIS_Y1];

	double fLen, fVel, fAcc, fJerk;
	fLen = sqrt(((pTgtPos[0] - dCurrX) * (pTgtPos[0] - dCurrX)) + ((pTgtPos[1] - dCurrY) * (pTgtPos[1] - dCurrY)));
	if (fLen > 0.001)
	{
		m_pMotion->GetSpeedProfile1(TRAPEZOIDAL, AXIS_X1, fLen, fVel, fAcc, fJerk);
		m_pMotion->Move1(MS_X1Y1, pTgtPos, fVel, fAcc, fAcc, ABS, NO_WAIT);
	}
}


void CGvisR2R_PunchView::LotEnd()
{
	if (m_pDlgMenu01)
		m_pDlgMenu01->LotEnd();
	if (m_pDlgMenu03)
		m_pDlgMenu03->SwAoiLotEnd(TRUE);

	pView->m_mgrProcedure->m_bCont = FALSE;
	SetLotEd();

	MakeResultMDS();

	if (pDoc->GetTestMode() == MODE_INNER || pDoc->GetTestMode() == MODE_OUTER)
	{
		FinalCopyItsFiles();
	}

	pView->m_mgrReelmap->DuplicateRmap();

	//if (m_pEngrave)
	//	m_pEngrave->SwMenu01DispDefImg(TRUE);
	SwMenu01DispDefImg(TRUE);
}

void CGvisR2R_PunchView::TimWinker(int nId, int nDly) // 0:Ready, 1:Reset, 2:Run, 3:Stop
{
	pView->m_mgrProcedure->m_bBtnWinker[nId] = TRUE;
	if (!pView->m_mgrProcedure->m_bTimBtnWinker)
	{
		pView->m_mgrProcedure->m_bTimBtnWinker = TRUE;
		pView->m_mgrProcedure->m_nDlyWinker[nId] = nDly;
		SetTimer(TIM_BTN_WINKER, 100, NULL);
	}
	// 	DispBtnWinker();
}

void CGvisR2R_PunchView::StopTimWinker(int nId) // 0:Ready, 1:Reset, 2:Run, 3:Stop
{
	pView->m_mgrProcedure->m_bBtnWinker[nId] = FALSE;
	pView->m_mgrProcedure->m_bTimBtnWinker = FALSE;
	for (int i = 0; i<4; i++)
	{
		if (pView->m_mgrProcedure->m_bBtnWinker[i])
			pView->m_mgrProcedure->m_bTimBtnWinker = TRUE;
	}
	// 	DispBtnWinker();
}

void CGvisR2R_PunchView::Winker(int nId, int nDly) // 0:Ready, 1:Reset, 2:Run, 3:Stop
{
#ifdef USE_MPE
	if (nId == MN_RUN)
	{
		if (pView->m_pMpe)
		{
			pView->m_mgrProcedure->m_bBtnWinker[nId] = TRUE;
			m_pMpe->Write(_T("MB44015D"), 1); // 자동 초기 운전상태(PC가 On/Off 시킴, PLC가 운전램프를 윙크동작, on->off시 운전램프 on, 다시 운전스위치가 눌러지면 off) - 20141017
		}
	}
#endif
}

void CGvisR2R_PunchView::ResetWinker() // 0:Ready, 1:Reset, 2:Run, 3:Stop
{
#ifdef USE_MPE
	m_pMpe->Write(_T("MB44015D"), 0);
#endif
	for (int i = 0; i<4; i++)
	{
		pView->m_mgrProcedure->m_bBtnWinker[i] = FALSE;
		pView->m_mgrProcedure->m_nCntBtnWinker[i] = FALSE;
	}
}

void CGvisR2R_PunchView::SetOrigin()
{
}

BOOL CGvisR2R_PunchView::IsSetLotEnd()
{
	if (pView->m_mgrProcedure->m_nLotEndSerial > 0)
		return TRUE;
	return FALSE;
}

void CGvisR2R_PunchView::SetLotEnd(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.61"));
		return;
	}
	pView->m_mgrProcedure->m_nLotEndSerial = nSerial;

	CString str;
	str.Format(_T("%d"), pView->m_mgrProcedure->m_nLotEndSerial);
	DispStsBar(str, 0);
	if (m_pDlgMenu01)
		m_pDlgMenu01->DispLotEndSerial(pView->m_mgrProcedure->m_nLotEndSerial);
}

int CGvisR2R_PunchView::GetLotEndSerial()
{
	return pView->m_mgrProcedure->m_nLotEndSerial; // 테이블상에 정지하는 Serial.
}

BOOL CGvisR2R_PunchView::StartLive()
{
	if (StartLive0() && StartLive1())
		return TRUE;

	return  FALSE;
}

BOOL CGvisR2R_PunchView::StartLive0()
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

BOOL CGvisR2R_PunchView::StartLive1()
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

BOOL CGvisR2R_PunchView::StopLive()
{
#ifdef TEST_MODE
	return TRUE;
#endif

	if (StopLive0() && StopLive1())
		return TRUE;

	return FALSE;
}

BOOL CGvisR2R_PunchView::StopLive0()
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

BOOL CGvisR2R_PunchView::StopLive1()
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

BOOL CGvisR2R_PunchView::IsVerify()
{
	BOOL bVerify = FALSE;
	int nSerial0 = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Cam0
	int nSerial1 = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1
	int nPeriod = pDoc->WorkingInfo.LastJob.nVerifyPeriod;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;


	if (pDoc->WorkingInfo.LastJob.bVerify)
	{
		double dFdLen = GetMkFdLen();
		double dVerifyLen = _tstof(pDoc->WorkingInfo.LastJob.sVerifyLen)*1000.0;

		if (dFdLen < dVerifyLen)
		{
			if (nSerial0 == 1 || nPeriod == 0 || nPeriod == 1 || nPeriod == 2 || pView->m_mgrProcedure->m_bStopF_Verify)
			{
				pView->m_mgrProcedure->m_bStopF_Verify = FALSE;
				bVerify = TRUE;
			}
			else
			{
				if(!(nSerial0 % nPeriod) || !(nSerial1 % nPeriod))
					bVerify = TRUE;
			}
		}
		else
		{
			pDoc->WorkingInfo.LastJob.bVerify = FALSE;
			if (m_pDlgMenu01)
				m_pDlgMenu01->UpdateData();
		}
	}

	return bVerify;
}

int CGvisR2R_PunchView::GetVsBufLastSerial()
{
	int nLastShot = pDoc->GetLastShotMk();
	if (nLastShot > 0 && pView->m_mgrProcedure->m_bCont)
		return (nLastShot + 4);

	return 4;
}

int CGvisR2R_PunchView::GetVsUpBufLastSerial()
{
	int nLastShot = pDoc->GetLastShotUp();
	if (nLastShot > 0 && pView->m_mgrProcedure->m_bCont)
		return (nLastShot + 4);

	return 4;
}

int CGvisR2R_PunchView::GetVsDnBufLastSerial()
{
	int nLastShot = pDoc->GetLastShotDn();
	if (nLastShot > 0 && pView->m_mgrProcedure->m_bCont)
		return (nLastShot + 4);

	return 4;
}

BOOL CGvisR2R_PunchView::IsReview()
{
	return (pDoc->WorkingInfo.LastJob.bReview);
}

BOOL CGvisR2R_PunchView::IsReview0()
{
	return (pDoc->WorkingInfo.LastJob.bReview);
}

BOOL CGvisR2R_PunchView::IsReview1()
{
	return (pDoc->WorkingInfo.LastJob.bReview);
}

BOOL CGvisR2R_PunchView::IsJogRtDn()
{
	BOOL bOn = FALSE;
#ifdef USE_MPE
	bOn = pDoc->m_pMpeIb[4] & (0x01 << 10) ? TRUE : FALSE;	// 마킹부 JOG 버튼(우)
#endif
	return bOn;
}

BOOL CGvisR2R_PunchView::IsJogRtUp()
{
	BOOL bOn = FALSE;
#ifdef USE_MPE
	bOn = pDoc->m_pMpeIb[4] & (0x01 << 10) ? FALSE : TRUE;	// 마킹부 JOG 버튼(우)
#endif
	return bOn;
}

BOOL CGvisR2R_PunchView::IsJogRtDn0()
{
	BOOL bOn = FALSE;
#ifdef USE_MPE
	bOn = pDoc->m_pMpeIb[4] & (0x01 << 10) ? TRUE : FALSE;	// 마킹부 JOG 버튼(우)
#endif
	return bOn;
}

BOOL CGvisR2R_PunchView::IsJogRtUp0()
{
	BOOL bOn = FALSE;
#ifdef USE_MPE
	bOn = pDoc->m_pMpeIb[4] & (0x01 << 10) ? FALSE : TRUE;	// 마킹부 JOG 버튼(우)
#endif
	return bOn;
}

BOOL CGvisR2R_PunchView::IsJogRtDn1()
{
	BOOL bOn = FALSE;
#ifdef USE_MPE
	bOn = pDoc->m_pMpeIb[4] & (0x01 << 10) ? TRUE : FALSE;	// 마킹부 JOG 버튼(우)
#endif
	return bOn;
}

BOOL CGvisR2R_PunchView::IsJogRtUp1()
{
	BOOL bOn = FALSE;
#ifdef USE_MPE
	bOn = pDoc->m_pMpeIo[4] & (0x01 << 10) ? FALSE : TRUE;	// 마킹부 JOG 버튼(우)
#endif
	return bOn;
}


void CGvisR2R_PunchView::SwAoiEmg(BOOL bOn)
{
	if (m_pDlgMenu03)
		m_pDlgMenu03->SwAoiEmg(bOn);

	if (bOn)
	{
		// 		pDoc->m_pMpeIo[8] |= (0x01<<0);		// 언코일러 비상정지 스위치 램프
		// 		pDoc->m_pMpeIo[12] |= (0x01<<0);	// 리코일러 비상정지 스위치 램프

		// 		pDoc->m_pSliceIo[9] |= (0x01<<14);	// 검사부 비상정지 스위치(스위치)
		// 		pDoc->m_pSliceIo[9] |= (0x01<<15);	// 검사부 비상정지 스위치(후면)
		// 		pDoc->m_pSliceIo[6] |= (0x01<<0);	// 마킹부 비상정지 스위치(모니터)
		// 		pDoc->m_pSliceIo[7] |= (0x01<<0);	// 마킹부 비상정지 스위치(스위치)
	}
	else
	{
		// 		pDoc->m_pMpeIo[8] &= ~(0x01<<0);	// 언코일러 비상정지 스위치 램프
		// 		pDoc->m_pMpeIo[12] &= ~(0x01<<0);	// 리코일러 비상정지 스위치 램프

		// 		pDoc->m_pSliceIo[9] &= ~(0x01<<14);	// 검사부 비상정지 스위치(스위치)
		// 		pDoc->m_pSliceIo[9] &= ~(0x01<<15);	// 검사부 비상정지 스위치(후면)
		// 		pDoc->m_pSliceIo[6] &= ~(0x01<<0);	// 마킹부 비상정지 스위치(모니터)
		// 		pDoc->m_pSliceIo[7] &= ~(0x01<<0);	// 마킹부 비상정지 스위치(스위치)
	}

}

BOOL CGvisR2R_PunchView::IsVs()
{
	if (!pView->m_mgrProcedure->m_bChkLastProcVs)
	{
		// 		int nTotPnl = int(_tstof(pDoc->WorkingInfo.Motion.sFdMkAoiInitDist) / _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen));;

		BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
		if (bDualTest)
		{
			if (GetAoiDnVsStatus())
				return TRUE;
			else if (GetAoiUpVsStatus())
				return TRUE;

			//if(m_nBufTot[1] > 2 && m_nBufTot[1] < nTotPnl-1)	// AOI 하면 버퍼 기준.
			//	return TRUE;
			//else 
			//{
			//	nTotPnl += _tstoi(pDoc->WorkingInfo.Motion.sFdAoiAoiDistShot);
			//	if(m_nBufTot[0] > 2 && m_nBufTot[0] < nTotPnl-1)	// AOI 상면 버퍼 기준. 20160807
			//		return TRUE;
			//}
		}
		else
		{
			if (GetAoiUpVsStatus())
				return TRUE;
			//nTotPnl += _tstoi(pDoc->WorkingInfo.Motion.sFdAoiAoiDistShot);
			//if(m_nBufTot[0] > 2 && m_nBufTot[0] < nTotPnl-1)	// AOI 상면 버퍼 기준.
			//	return TRUE;
		}
	}

	return FALSE;
}

BOOL CGvisR2R_PunchView::IsVsUp()
{
	//int nTotPnl = int(_tstof(pDoc->WorkingInfo.Motion.sFdMkAoiInitDist) / _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen));
	 
	//nTotPnl += _tstoi(pDoc->WorkingInfo.Motion.sFdAoiAoiDistShot);
	//if(m_nBufTot[0] > 2 && m_nBufTot[0] < nTotPnl-1)	// AOI 상면 버퍼 기준.
	//	return TRUE;
	//
	//return FALSE;

	return GetAoiUpVsStatus();
}

BOOL CGvisR2R_PunchView::IsVsDn()
{
	//int nTotPnl = int(_tstof(pDoc->WorkingInfo.Motion.sFdMkAoiInitDist) / _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen));
	 
	//if(m_nBufTot[1] > 2 && m_nBufTot[1] < nTotPnl-1)	// AOI 하면 버퍼 기준.
	//	return TRUE;
	//
	//return FALSE;

	return GetAoiDnVsStatus();
}

void CGvisR2R_PunchView::SetDummyUp()
{
	pView->m_mgrProcedure->m_nDummy[0]++;
	if (pView->m_mgrProcedure->m_nDummy[0] == 3)
	{
		if (!MakeDummyUp(-2))
		{
			pView->ClrDispMsg();
			AfxMessageBox(_T("Error - MakeDummyUp(-2)"));
		}
	}
	else
	{
		if (!MakeDummyUp(-1))
		{
			pView->ClrDispMsg();
			AfxMessageBox(_T("Error - MakeDummyUp(-1)"));
		}
	}
}

void CGvisR2R_PunchView::SetDummyDn()
{
	pView->m_mgrProcedure->m_nDummy[1]++;
	if (pView->m_mgrProcedure->m_nDummy[1] == 3)
	{
		if (!MakeDummyDn(-2))
		{
			pView->ClrDispMsg();
			AfxMessageBox(_T("Error - MakeDummyDn(-2)"));
	}
	}
	else
	{
		if (!MakeDummyDn(-1))
		{
			pView->ClrDispMsg();
			AfxMessageBox(_T("Error - MakeDummyDn(-1)"));
		}
	}
}

BOOL CGvisR2R_PunchView::MakeDummyUp(int nErr) // AOI 상면 기준.
{
	int nSerial = pView->m_mgrProcedure->m_nAoiLastSerial[0] + pView->m_mgrProcedure->m_nDummy[0];
	FILE *fpPCR = NULL;

	CString sMsg, strRstPath, strRstPath2, sDummyRst;
	strRstPath.Format(_T("%s%s\\%s\\%s\\%04d.RST"), pDoc->WorkingInfo.System.sPathAoiUpVrsData,
		pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLayerUp,
		pDoc->WorkingInfo.LastJob.sLotUp,
		pView->m_mgrProcedure->m_nAoiLastSerial[0]);

	CDataFile *pDataFile = new CDataFile;

	if (!pDataFile->Open(strRstPath))
	{
		sMsg.Format(_T("%s File not found."), strRstPath);
		pView->MsgBox(sMsg);
		delete pDataFile;
		return FALSE;
	}
	CString sLine, sRemain, sNewLine;
	sLine = pDataFile->GetLineString(1);
	int nPos = sLine.Find(',', 0);
	sLine.Delete(0, nPos);
	sNewLine.Format(_T("%d%s"), 0, sLine);
	pDataFile->ReplaceLine(1, sNewLine);

	strRstPath2.Format(_T("%s%s\\%s\\%s\\%04d.RST"), pDoc->WorkingInfo.System.sPathAoiUpVrsData,
		pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLayerUp,
		pDoc->WorkingInfo.LastJob.sLotUp,
		nSerial);
	char* pRtn = NULL;
	fpPCR = fopen(pRtn = StringToChar(strRstPath2), "w+"); if(pRtn) delete pRtn; pRtn = NULL;
	if (fpPCR == NULL)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("TROUBLE_SAVE_PIECEOUT_VRS"), MB_ICONWARNING | MB_OK);
	}
	fprintf(fpPCR, pRtn = StringToChar(pDataFile->GetAllString())); if (pRtn) delete pRtn; pRtn = NULL;
	if (fpPCR != NULL)
		fclose(fpPCR);

	delete pDataFile;


	CString sDummyPath;
	sDummyPath.Format(_T("%s%s\\%s\\%s\\%04d.PCR"), pDoc->WorkingInfo.System.sPathAoiUpVrsData,
		pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLayerUp,
		pDoc->WorkingInfo.LastJob.sLotUp,
		nSerial);

	fpPCR = fopen(pRtn = StringToChar(sDummyPath), "w+"); if (pRtn) delete pRtn; pRtn = NULL;
	if (fpPCR == NULL)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("TROUBLE_SAVE_PIECEOUT_VRS"), MB_ICONWARNING | MB_OK);
	}

	fprintf(fpPCR, "%04d,%s,%s,%s\n", nErr, pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLayerUp,
		pDoc->WorkingInfo.LastJob.sLotUp);

	fprintf(fpPCR, "%04d\n", 0);

	fprintf(fpPCR, "%s", pRtn = StringToChar(_T("파일마지막"))); if (pRtn) delete pRtn; pRtn = NULL;
	fprintf(fpPCR, "\n");

	if (fpPCR != NULL)
		fclose(fpPCR);



	sDummyPath.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVsShareUp, nSerial);

	fpPCR = fopen(pRtn = StringToChar(sDummyPath), "w+"); if(pRtn) delete pRtn; pRtn = NULL;
	if (fpPCR == NULL)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("TROUBLE_SAVE_PIECEOUT_VRS"), MB_ICONWARNING | MB_OK);
	}

	fprintf(fpPCR, "%04d,%s,%s,%s\n", nErr, pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLayerUp,
		pDoc->WorkingInfo.LastJob.sLotUp);

	fprintf(fpPCR, "%04d\n", 0);

	fprintf(fpPCR, "%s", pRtn = StringToChar(_T("파일마지막"))); if (pRtn) delete pRtn; pRtn = NULL;
	fprintf(fpPCR, "\n");

	if (fpPCR != NULL)
		fclose(fpPCR);

	return TRUE;
}

BOOL CGvisR2R_PunchView::MakeDummyDn(int nErr) // AOI 상면 기준.
{
	pView->m_mgrProcedure->m_nAoiLastSerial[1] = pView->m_mgrProcedure->m_nAoiLastSerial[0] - 3;
	int nSerial = pView->m_mgrProcedure->m_nAoiLastSerial[0] + pView->m_mgrProcedure->m_nDummy[1];
	FILE *fpPCR = NULL;

	CString sMsg, strRstPath, strRstPath2, sDummyRst;
	strRstPath.Format(_T("%s%s\\%s\\%s\\%04d.RST"), pDoc->WorkingInfo.System.sPathAoiDnVrsData,
		pDoc->WorkingInfo.LastJob.sModelUp,
		//pDoc->WorkingInfo.LastJob.sModelDn,
		pDoc->WorkingInfo.LastJob.sLayerDn,
		pDoc->WorkingInfo.LastJob.sLotUp,
		//pDoc->WorkingInfo.LastJob.sLotDn,
		pView->m_mgrProcedure->m_nAoiLastSerial[1]);

	CDataFile *pDataFile = new CDataFile;
	char* pRtn = NULL;
	if (!pDataFile->Open(strRstPath))
	{
		sMsg.Format(_T("%s File not found."), strRstPath);
		pView->MsgBox(sMsg);
		delete pDataFile;
		return FALSE;
	}
	CString sLine, sRemain, sNewLine;
	sLine = pDataFile->GetLineString(1);
	int nPos = sLine.Find(',', 0);
	sLine.Delete(0, nPos);
	sNewLine.Format(_T("%d%s"), 0, sLine);
	pDataFile->ReplaceLine(1, sNewLine);

	strRstPath2.Format(_T("%s%s\\%s\\%s\\%04d.RST"), pDoc->WorkingInfo.System.sPathAoiDnVrsData,
		pDoc->WorkingInfo.LastJob.sModelUp,
		//pDoc->WorkingInfo.LastJob.sModelDn,
		pDoc->WorkingInfo.LastJob.sLayerDn,
		pDoc->WorkingInfo.LastJob.sLotUp,
		//pDoc->WorkingInfo.LastJob.sLotDn,
		nSerial);
	fpPCR = fopen(pRtn = StringToChar(strRstPath2), "w+"); if (pRtn) delete pRtn; pRtn = NULL;
	if (fpPCR == NULL)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("TROUBLE_SAVE_PIECEOUT_VRS"), MB_ICONWARNING | MB_OK);
	}
	fprintf(fpPCR, pRtn = StringToChar(pDataFile->GetAllString())); if (pRtn) delete pRtn; pRtn = NULL;
	if (fpPCR != NULL)
		fclose(fpPCR);

	delete pDataFile;


	CString sDummyPath;
	sDummyPath.Format(_T("%s%s\\%s\\%s\\%04d.PCR"), pDoc->WorkingInfo.System.sPathAoiDnVrsData,
		pDoc->WorkingInfo.LastJob.sModelUp,
		//pDoc->WorkingInfo.LastJob.sModelDn,
		pDoc->WorkingInfo.LastJob.sLayerDn,
		pDoc->WorkingInfo.LastJob.sLotUp,
		//pDoc->WorkingInfo.LastJob.sLotDn,
		nSerial);

	fpPCR = fopen(pRtn = StringToChar(sDummyPath), "w+"); if (pRtn) delete pRtn; pRtn = NULL;
	if (fpPCR == NULL)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("TROUBLE_SAVE_PIECEOUT_VRS"), MB_ICONWARNING | MB_OK);
	}

	fprintf(fpPCR, "%04d,%s,%s,%s\n", nErr, pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLayerDn,
		pDoc->WorkingInfo.LastJob.sLotUp);
	//fprintf(fpPCR, "%04d,%s,%s,%s\n", nErr, pDoc->WorkingInfo.LastJob.sModelDn,
	//	pDoc->WorkingInfo.LastJob.sLayerDn,
	//	pDoc->WorkingInfo.LastJob.sLotDn);

	fprintf(fpPCR, "%04d\n", 0);

	fprintf(fpPCR, "%s", pRtn = StringToChar(_T("파일마지막"))); if (pRtn) delete pRtn; pRtn = NULL;
	fprintf(fpPCR, "\n");

	if (fpPCR != NULL)
		fclose(fpPCR);



	sDummyPath.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVsShareDn, nSerial);

	fpPCR = fopen(pRtn = StringToChar(sDummyPath), "w+"); delete pRtn;
	if (fpPCR == NULL)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("TROUBLE_SAVE_PIECEOUT_VRS"), MB_ICONWARNING | MB_OK);
	}

	fprintf(fpPCR, "%04d,%s,%s,%s\n", nErr, pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLayerDn,
		pDoc->WorkingInfo.LastJob.sLotUp);
	//fprintf(fpPCR, "%04d,%s,%s,%s\n", nErr, pDoc->WorkingInfo.LastJob.sModelDn,
	//	pDoc->WorkingInfo.LastJob.sLayerDn,
	//	pDoc->WorkingInfo.LastJob.sLotDn);

	fprintf(fpPCR, "%04d\n", 0);

	fprintf(fpPCR, "%s", pRtn = StringToChar(_T("파일마지막"))); if (pRtn) delete pRtn; pRtn = NULL;
	fprintf(fpPCR, "\n");

	if (fpPCR != NULL)
		fclose(fpPCR);

	return TRUE;
}

void CGvisR2R_PunchView::SetAoiDummyShot(int nAoi, int nDummy)
{
	pDoc->AoiDummyShot[nAoi] = nDummy;
	switch (nAoi)
	{
	case 0:
		if (m_pMpe)
			m_pMpe->Write(_T("ML45068"), (long)nDummy);	// 검사부 (상) 작업종료 더미 Shot수 - 20141104
		break;
	case 1:
		if (m_pMpe)
			m_pMpe->Write(_T("ML45070"), (long)nDummy);	// 검사부 (하) 작업종료 더미 Shot수 - 20141104
		break;
	}
}

int CGvisR2R_PunchView::GetAoiUpDummyShot()
{
	int nDummy = 0;
	//char szData[200];
	TCHAR szData[200];
	CString sPath = pDoc->WorkingInfo.System.sPathAoiUpCurrInfo;
	if (0 < ::GetPrivateProfileString(_T("Dummy"), _T("Shot"), NULL, szData, sizeof(szData), sPath))
		nDummy = _tstoi(szData);
	else
		nDummy = 0;

	return nDummy;
}

int CGvisR2R_PunchView::GetAoiDnDummyShot()
{
	int nDummy = 0;
	//char szData[200];
	TCHAR szData[200];
	CString sPath = pDoc->WorkingInfo.System.sPathAoiDnCurrInfo;
	if (0 < ::GetPrivateProfileString(_T("Dummy"), _T("Shot"), NULL, szData, sizeof(szData), sPath))
		nDummy = _tstoi(szData);
	else
		nDummy = 0;

	return nDummy;
}

int CGvisR2R_PunchView::GetAoiUpSerial()
{
	int nSerial = 0;
	//char szData[200];
	TCHAR szData[200];
	CString sPath = pDoc->WorkingInfo.System.sPathAoiUpCurrInfo;
	if (0 < ::GetPrivateProfileString(_T("Infomation"), _T("Current Serial"), NULL, szData, sizeof(szData), sPath))
		nSerial = _tstoi(szData);
	else
		nSerial = 0;
	if (nSerial > 1)
		nSerial--;

	return nSerial;
}

BOOL CGvisR2R_PunchView::GetAoiUpVsStatus()
{
	BOOL bVsStatus = FALSE;
	//char szData[200];
	TCHAR szData[200];
	CString sPath = pDoc->WorkingInfo.System.sPathAoiUpCurrInfo;
	if (0 < ::GetPrivateProfileString(_T("Infomation"), _T("Current VS Status"), NULL, szData, sizeof(szData), sPath))
		bVsStatus = _tstoi(szData) > 0 ? TRUE : FALSE;

	return bVsStatus;
}

int CGvisR2R_PunchView::GetAoiDnSerial()
{
	int nSerial = 0;
	//char szData[200];
	TCHAR szData[200];
	CString sPath = pDoc->WorkingInfo.System.sPathAoiDnCurrInfo;
	if (0 < ::GetPrivateProfileString(_T("Infomation"), _T("Current Serial"), NULL, szData, sizeof(szData), sPath))
		nSerial = _tstoi(szData);
	else
		nSerial = 0;
	if (nSerial > 1)
		nSerial--;

	return nSerial;
}

BOOL CGvisR2R_PunchView::GetAoiDnVsStatus()
{
	BOOL bVsStatus = FALSE;
	//char szData[200];
	TCHAR szData[200];
	CString sPath = pDoc->WorkingInfo.System.sPathAoiDnCurrInfo;
	if (0 < ::GetPrivateProfileString(_T("Infomation"), _T("Current VS Status"), NULL, szData, sizeof(szData), sPath))
		bVsStatus = _tstoi(szData) > 0 ? TRUE : FALSE;

	return bVsStatus;
}

BOOL CGvisR2R_PunchView::IsDoneDispMkInfo()
{
	BOOL bRtn = FALSE;
	if (m_pDlgMenu01)
		bRtn = m_pDlgMenu01->IsDoneDispMkInfo();
	return bRtn;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void CGvisR2R_PunchView::Marking()
{
}

void CGvisR2R_PunchView::DoReject0()
{
#ifdef TEST_MODE
	return;
#endif

	//BOOL bOn;
	int nIdx, nRtn;//nSerial, 
	CfPoint ptPnt;

	if (!pView->m_mgrProcedure->IsRun() && pView->m_mgrProcedure->m_bAuto)
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


	switch (pView->m_mgrProcedure->m_nStepMk[2])
	{
	case 0:
		if (IsNoMk())
			ShowLive();
		pView->m_mgrProcedure->m_nMkStrip[0][0] = 0;
		pView->m_mgrProcedure->m_nMkStrip[0][1] = 0;
		pView->m_mgrProcedure->m_nMkStrip[0][2] = 0;
		pView->m_mgrProcedure->m_nMkStrip[0][3] = 0;
		pView->m_mgrProcedure->m_bRejectDone[0][0] = FALSE;
		pView->m_mgrProcedure->m_bRejectDone[0][1] = FALSE;
		pView->m_mgrProcedure->m_bRejectDone[0][2] = FALSE;
		pView->m_mgrProcedure->m_bRejectDone[0][3] = FALSE;
		pView->m_mgrProcedure->m_nStepMk[2]++;
		break;
	case 1:
		if (pView->m_mgrProcedure->m_nMkPcs[2] < pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)
		{
			pView->m_mgrProcedure->m_nStepMk[2]++;
		}
		else
		{
			pView->m_mgrProcedure->m_nStepMk[2] = MK_END;
		}
		break;
	case 2:
		pView->m_mgrProcedure->m_nStepMk[2]++;
		break;
	case 3:
		if (pView->m_mgrProcedure->m_nMkPcs[2] < pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)	// 마킹완료Check
		{
			ptPnt = GetMkPnt(pView->m_mgrProcedure->m_nMkPcs[2]);

			if (ptPnt.x < 0.0 && ptPnt.y < 0.0) // 양품화.
			{
				pView->m_mgrProcedure->m_nMkPcs[2]++;
				break;
			}

			nIdx = GetMkStripIdx(pView->m_mgrProcedure->m_nMkPcs[2]);
			if (nIdx > 0)
			{
				if (!IsMkStrip(nIdx)) // Strip[] Mk Off
				{
					pView->m_mgrProcedure->m_nMkPcs[2]++;
					break;
				}
				else
				{
					if (pView->m_mgrProcedure->m_nMkStrip[0][nIdx - 1] > pDoc->GetStripRejectMkNum())
					{
						pView->m_mgrProcedure->m_bRejectDone[0][nIdx - 1] = TRUE;
						pView->m_mgrProcedure->m_nMkPcs[2]++;
						break;
					}
					else
						pView->m_mgrProcedure->m_nMkStrip[0][nIdx - 1]++;
				}
			}
			else
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Strip Index Failed."));
				break;
			}

			if (pView->m_mgrProcedure->m_bRejectDone[0][0] && pView->m_mgrProcedure->m_bRejectDone[0][1] &&
				pView->m_mgrProcedure->m_bRejectDone[0][2] && pView->m_mgrProcedure->m_bRejectDone[0][3])
			{
				pView->m_mgrProcedure->m_nStepMk[2] = MK_END;
				break;
			}

			pView->m_mgrProcedure->m_dTarget[AXIS_X0] = ptPnt.x;

			// Cam0 : m_bPriority[0], m_bPriority[3]
			// Cam1 : m_bPriority[1], m_bPriority[2]

			pView->m_mgrProcedure->m_bCollision[0] = pView->m_mgrProcedure->ChkCollision(AXIS_X0, ptPnt.x);
			if (!pView->m_mgrProcedure->m_bCollision[0])
			{
				Move0(ptPnt, pView->m_mgrProcedure->m_bCam);
				pView->m_mgrProcedure->m_nStepMk[2]++;
			}
			else if (pView->m_mgrProcedure->m_bPriority[0])
			{
				pView->m_mgrProcedure->m_bPriority[0] = FALSE;
				Move0(ptPnt, pView->m_mgrProcedure->m_bCam);
				pView->m_mgrProcedure->m_nStepMk[2]++;
			}
			else if (pView->m_mgrProcedure->m_bPriority[2])
			{
				pView->m_mgrProcedure->m_bPriority[2] = FALSE;
				ptPnt.x = 0.0;
				ptPnt.y = 0.0;//m_dEnc[AXIS_Y0];
				Move0(ptPnt, pView->m_mgrProcedure->m_bCam);
			}
		}
		else
		{
			pView->m_mgrProcedure->m_nStepMk[2] = MK_END;
		}
		break;
	case 4:
		pView->m_mgrProcedure->m_nStepMk[2]++;
		break;
	case 5:
		pView->m_mgrProcedure->m_nStepMk[2]++;
		break;
	case 6:
		if (IsMoveDone0())
			pView->m_mgrProcedure->m_nStepMk[2]++;
		break;
	case 7:
		if (!IsNoMk0())
		{
			pView->m_mgrProcedure->m_dwStMkDn[0] = GetTickCount();
			Mk0();
		}
		else
			SetDelay0(pDoc->m_nDelayShow, 1);		// [mSec]
		pView->m_mgrProcedure->m_nStepMk[2]++;
		break;
	case 8:
		if (IsNoMk0())
		{
			if (!WaitDelay0(1))		// F:Done, T:On Waiting....
				pView->m_mgrProcedure->m_nStepMk[2]++;
		}
		else
			pView->m_mgrProcedure->m_nStepMk[2]++;
		break;
	case 9:
		pView->m_mgrProcedure->m_nStepMk[2]++;
		break;
	case 10:
		if (!IsNoMk0())
		{
			//Mk0(FALSE);
			if (IsMk0Done())
			{
				pView->m_mgrProcedure->m_nMkPcs[2]++;
				pView->m_mgrProcedure->m_nStepMk[2]++;
				pView->m_mgrProcedure->m_nStepMk[2]++;
			}
			else
			{
				if (pView->m_mgrProcedure->m_dwStMkDn[0] + 5000 < GetTickCount())
				{
					Buzzer(TRUE, 0);
					//pView->DispStsBar(_T("정지-29"), 0);
					DispMain(_T("정 지"), RGB_RED);
					m_pVoiceCoil[0]->SearchHomeSmac0();

					nRtn = AsyncMsgBox(_T("보이스코일(좌) 통신완료가 않됩니다.\r\n마킹을 다시 시도하시겠습니까?"), 1, MB_YESNO);
					if (IDYES == nRtn)
					{
						DispMain(_T("운전중"), RGB_RED);
						pView->m_mgrProcedure->m_nStepMk[2] = 7;
					}
					else if (nRtn < 0)
						pView->m_mgrProcedure->m_nStepMk[2]++;
					else
					{
						pView->m_mgrProcedure->m_nMkPcs[2]++;
						pView->m_mgrProcedure->m_nStepMk[2]++;
						pView->m_mgrProcedure->m_nStepMk[2]++;
						Stop();
					}
				}
			}
		}
		else
		{
			pView->m_mgrProcedure->m_nMkPcs[2]++;
			pView->m_mgrProcedure->m_nStepMk[2]++;
			pView->m_mgrProcedure->m_nStepMk[2]++;
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
				DispMain(_T("운전중"), RGB_RED);
				pView->m_mgrProcedure->m_nStepMk[2] = 7;
			}
			else
			{
				pView->m_mgrProcedure->m_nMkPcs[2]++;
				pView->m_mgrProcedure->m_nStepMk[2]++;
				Stop();
			}
		}
		break;
	case 12:
		pView->m_mgrProcedure->m_nStepMk[2]++;
		break;
	case 13:
		if (pView->m_mgrProcedure->m_nMkPcs[2] < pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)	// 마킹완료Check
		{
			pView->m_mgrProcedure->m_nStepMk[2] = 3;
		}
		else
		{
			pView->m_mgrProcedure->m_nStepMk[2] = MK_END;
		}
		break;
	case MK_END:
		m_pVoiceCoil[0]->SearchHomeSmac0();
		SetDelay0(500, 1);		// [mSec]
		pView->m_mgrProcedure->m_nStepMk[2]++;
		break;
	case 101:
		if (!WaitDelay0(1))		// F:Done, T:On Waiting....
		{
			MoveInitPos0();
			pView->m_mgrProcedure->m_nStepMk[2]++;
		}
		break;
	case 102:
		if (IsMoveDone0())
		{
			pView->m_mgrProcedure->m_bTHREAD_MK[2] = FALSE;
			pView->m_mgrProcedure->m_bDoneMk[0] = TRUE;
			pView->m_mgrProcedure->m_nStepMk[2] = 0;
			pView->m_mgrProcedure->m_nMkStrip[0][0] = 0;
			pView->m_mgrProcedure->m_nMkStrip[0][1] = 0;
			pView->m_mgrProcedure->m_nMkStrip[0][2] = 0;
			pView->m_mgrProcedure->m_nMkStrip[0][3] = 0;
		}
		break;
	}
}

void CGvisR2R_PunchView::DoReject1()
{
#ifdef TEST_MODE
	return;
#endif

	//BOOL bOn;
	int nIdx, nRtn;//nSerial, 
	CfPoint ptPnt;

	if (!pView->m_mgrProcedure->IsRun() && pView->m_mgrProcedure->m_bAuto)
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


	switch (pView->m_mgrProcedure->m_nStepMk[3])
	{
	case 0:
		if (IsNoMk())
			ShowLive();
		pView->m_mgrProcedure->m_nMkStrip[1][0] = 0;
		pView->m_mgrProcedure->m_nMkStrip[1][1] = 0;
		pView->m_mgrProcedure->m_nMkStrip[1][2] = 0;
		pView->m_mgrProcedure->m_nMkStrip[1][3] = 0;
		pView->m_mgrProcedure->m_bRejectDone[1][0] = FALSE;
		pView->m_mgrProcedure->m_bRejectDone[1][1] = FALSE;
		pView->m_mgrProcedure->m_bRejectDone[1][2] = FALSE;
		pView->m_mgrProcedure->m_bRejectDone[1][3] = FALSE;
		pView->m_mgrProcedure->m_nStepMk[3]++;
		break;
	case 1:
		if (pView->m_mgrProcedure->m_nMkPcs[3] < pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)
		{
			pView->m_mgrProcedure->m_nStepMk[3]++;
		}
		else
		{
			pView->m_mgrProcedure->m_nStepMk[3] = MK_END;
		}
		break;
	case 2:
		pView->m_mgrProcedure->m_nStepMk[3]++;
		break;
	case 3:
		if (pView->m_mgrProcedure->m_nMkPcs[3] < pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)	// 마킹완료Check
		{
			ptPnt = GetMkPnt(pView->m_mgrProcedure->m_nMkPcs[3]);

			if (ptPnt.x < 0.0 && ptPnt.y < 0.0) // 양품화.
			{
				pView->m_mgrProcedure->m_nMkPcs[3]++;
				break;
			}

			nIdx = GetMkStripIdx(pView->m_mgrProcedure->m_nMkPcs[3]);
			if (nIdx > 0)
			{
				if (!IsMkStrip(nIdx)) // Strip[] Mk Off
				{
					pView->m_mgrProcedure->m_nMkPcs[3]++;
					break;
				}
				else
				{
					if (pView->m_mgrProcedure->m_nMkStrip[1][nIdx - 1] > pDoc->GetStripRejectMkNum())
					{
						pView->m_mgrProcedure->m_bRejectDone[1][nIdx - 1] = TRUE;
						pView->m_mgrProcedure->m_nMkPcs[3]++;
						break;
					}
					else
						pView->m_mgrProcedure->m_nMkStrip[1][nIdx - 1]++;
				}
			}
			else
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Strip Index Failed."));
				break;
			}

			if (pView->m_mgrProcedure->m_bRejectDone[1][0] && pView->m_mgrProcedure->m_bRejectDone[1][1] &&
				pView->m_mgrProcedure->m_bRejectDone[1][2] && pView->m_mgrProcedure->m_bRejectDone[1][3])
			{
				pView->m_mgrProcedure->m_nStepMk[2] = MK_END;
				break;
			}

			pView->m_mgrProcedure->m_dTarget[AXIS_X1] = ptPnt.x;

			// Cam0 : m_bPriority[0], m_bPriority[3]
			// Cam1 : m_bPriority[1], m_bPriority[2]

			pView->m_mgrProcedure->m_bCollision[1] = pView->m_mgrProcedure->ChkCollision(AXIS_X1, ptPnt.x);
			if (!pView->m_mgrProcedure->m_bCollision[1])
			{
				Move1(ptPnt, pView->m_mgrProcedure->m_bCam);
				pView->m_mgrProcedure->m_nStepMk[3]++;
			}
			else if (pView->m_mgrProcedure->m_bPriority[1])
			{
				pView->m_mgrProcedure->m_bPriority[1] = FALSE;
				Move1(ptPnt, pView->m_mgrProcedure->m_bCam);
				pView->m_mgrProcedure->m_nStepMk[3]++;
			}
			else if (pView->m_mgrProcedure->m_bPriority[3])
			{
				pView->m_mgrProcedure->m_bPriority[3] = FALSE;
				ptPnt.x = _tstof(pDoc->WorkingInfo.Motion.sSafeZone);
				ptPnt.y = 0.0;//m_dEnc[AXIS_Y1];
				Move1(ptPnt, pView->m_mgrProcedure->m_bCam);
			}
		}
		else
		{
			pView->m_mgrProcedure->m_nStepMk[3] = MK_END;
		}
		break;
	case 4:
		pView->m_mgrProcedure->m_nStepMk[3]++;
		break;
	case 5:
		pView->m_mgrProcedure->m_nStepMk[3]++;
		break;
	case 6:
		if (IsMoveDone1())
			pView->m_mgrProcedure->m_nStepMk[3]++;
		break;
	case 7:
		if (!IsNoMk1())
		{
			pView->m_mgrProcedure->m_dwStMkDn[1] = GetTickCount();
			Mk1();
		}
		else
			SetDelay1(pDoc->m_nDelayShow, 1);		// [mSec]
		pView->m_mgrProcedure->m_nStepMk[3]++;
		break;
	case 8:
		if (IsNoMk1())
		{
			if (!WaitDelay1(1))		// F:Done, T:On Waiting....
				pView->m_mgrProcedure->m_nStepMk[3]++;
		}
		else
			pView->m_mgrProcedure->m_nStepMk[3]++;
		break;
	case 9:
		pView->m_mgrProcedure->m_nStepMk[3]++;
		break;
	case 10:
		if (!IsNoMk1())
		{
			if (IsMk1Done())
			{
				pView->m_mgrProcedure->m_nMkPcs[3]++;
				pView->m_mgrProcedure->m_nStepMk[3]++;
				pView->m_mgrProcedure->m_nStepMk[3]++;
			}
			else
			{
				if (pView->m_mgrProcedure->m_dwStMkDn[1] + 5000 < GetTickCount())
				{
					Buzzer(TRUE, 0);
					//pView->DispStsBar(_T("정지-30"), 0);
					DispMain(_T("정 지"), RGB_RED);
					m_pVoiceCoil[1]->SearchHomeSmac1();

					nRtn = AsyncMsgBox(_T("보이스코일(우) 통신완료가 않됩니다.\r\n마킹을 다시 시도하시겠습니까?"), 2, MB_YESNO);
					if (IDYES == nRtn)
					{
						DispMain(_T("운전중"), RGB_RED);
						pView->m_mgrProcedure->m_nStepMk[3] = 7;
					}
					else if (nRtn < 0)
						pView->m_mgrProcedure->m_nStepMk[3]++;
					else
					{
						pView->m_mgrProcedure->m_nMkPcs[3]++;
						pView->m_mgrProcedure->m_nStepMk[3]++;
						pView->m_mgrProcedure->m_nStepMk[3]++;
						Stop();
					}
				}
			}
		}
		else
		{
			pView->m_mgrProcedure->m_nMkPcs[3]++;
			pView->m_mgrProcedure->m_nStepMk[3]++;
			pView->m_mgrProcedure->m_nStepMk[3]++;
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
				DispMain(_T("운전중"), RGB_RED);
				pView->m_mgrProcedure->m_nStepMk[3] = 7;
			}
			else
			{
				pView->m_mgrProcedure->m_nMkPcs[3]++;
				pView->m_mgrProcedure->m_nStepMk[3]++;
				Stop();
			}
		}
		break;
	case 12:
		pView->m_mgrProcedure->m_nStepMk[3]++;
		break;
	case 13:
		if (pView->m_mgrProcedure->m_nMkPcs[3] < pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)	// 마킹완료Check
		{
			pView->m_mgrProcedure->m_nStepMk[3] = 3;
		}
		else
		{
			pView->m_mgrProcedure->m_nStepMk[3] = MK_END;
		}
		break;
	case MK_END:
		m_pVoiceCoil[1]->SearchHomeSmac1();
		SetDelay1(500, 1);		// [mSec]
		pView->m_mgrProcedure->m_nStepMk[3]++;
		break;
	case 101:
		if (!WaitDelay1(1))		// F:Done, T:On Waiting....
		{
			MoveMkEdPos1();
			pView->m_mgrProcedure->m_nStepMk[3]++;
		}
		break;
	case 102:
		if (IsMoveDone1())
		{
			pView->m_mgrProcedure->m_bTHREAD_MK[3] = FALSE;
			pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
			pView->m_mgrProcedure->m_nStepMk[3] = 0;
			pView->m_mgrProcedure->m_nMkStrip[1][0] = 0;
			pView->m_mgrProcedure->m_nMkStrip[1][1] = 0;
			pView->m_mgrProcedure->m_nMkStrip[1][2] = 0;
			pView->m_mgrProcedure->m_nMkStrip[1][3] = 0;
		}
		break;
	}
}

void CGvisR2R_PunchView::DoMark0All()
{
	//BOOL bOn;
	//int nSerial;
	CfPoint ptPnt;

	if (!pView->m_mgrProcedure->IsRun())
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

			if (pView->m_mgrProcedure->m_nStepMk[2] < 5 && pView->m_mgrProcedure->m_nStepMk[2] > 3) // Mk0();
			{
				pView->m_mgrProcedure->m_nStepMk[2] = 3;	// 마킹완료Check
			}
		}
		return;
	}


	switch (pView->m_mgrProcedure->m_nStepMk[2])
	{
	case 0:
		pView->m_mgrProcedure->m_nStepMk[2]++;
		break;
	case 1:
		if (pView->m_mgrProcedure->m_nMkPcs[2] < pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)
		{
			pView->m_mgrProcedure->m_nStepMk[2]++;
		}
		else
		{
			pView->m_mgrProcedure->m_nStepMk[2] = MK_END;
		}
		break;
	case 2:
		pView->m_mgrProcedure->m_nStepMk[2]++;
		break;
	case 3:
		if (pView->m_mgrProcedure->m_nMkPcs[2] < pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)	// 마킹완료Check
		{
			ptPnt = GetMkPnt(pView->m_mgrProcedure->m_nMkPcs[2]);

			if (ptPnt.x < 0.0 && ptPnt.y < 0.0) // 양품화.
			{
				pView->m_mgrProcedure->m_nMkPcs[2]++;
				break;
			}

			pView->m_mgrProcedure->m_dTarget[AXIS_X0] = ptPnt.x;

			// Cam0 : m_bPriority[0] <-, m_bPriority[3] ->
			// Cam1 : m_bPriority[1] ->, m_bPriority[2] <-

			pView->m_mgrProcedure->m_bCollision[0] = pView->m_mgrProcedure->ChkCollision(AXIS_X0, ptPnt.x);
			if (!pView->m_mgrProcedure->m_bCollision[0])
			{
				if (IsMoveDone0())
				{
					Move0(ptPnt, pView->m_mgrProcedure->m_bCam);
					pView->m_mgrProcedure->m_nStepMk[2]++;
				}
			}
			else if (pView->m_mgrProcedure->m_bPriority[0])
			{
				if (IsMoveDone0())
				{
					pView->m_mgrProcedure->m_bPriority[0] = FALSE;
					Move0(ptPnt, pView->m_mgrProcedure->m_bCam);
					pView->m_mgrProcedure->m_nStepMk[2]++;
				}
			}
			else if (pView->m_mgrProcedure->m_bPriority[2])
			{
				if (IsMoveDone0())
				{
					pView->m_mgrProcedure->m_bPriority[2] = FALSE;
					ptPnt.x = 0.0;
					ptPnt.y = 0.0;//m_dEnc[AXIS_Y0];
					Move0(ptPnt, FALSE);
				}
			}
		}
		else
		{
			pView->m_mgrProcedure->m_nStepMk[2] = MK_END;
		}
		break;
	case 4:
		if (IsMoveDone0())
			pView->m_mgrProcedure->m_nStepMk[2]++;
		break;
	case 5:
		if (!IsNoMk0())
			Mk0();
		else
			SetDelay0(pDoc->m_nDelayShow, 1);		// [mSec]
		pView->m_mgrProcedure->m_nStepMk[2]++;
		break;
	case 6:
		if (IsNoMk0())
		{
			if (!WaitDelay0(1))		// F:Done, T:On Waiting....
				pView->m_mgrProcedure->m_nStepMk[2]++;
		}
		else
			pView->m_mgrProcedure->m_nStepMk[2]++;
		break;
	case 7:
		pView->m_mgrProcedure->m_nMkPcs[2]++;
		pView->m_mgrProcedure->m_nStepMk[2]++;
		break;
	case 8:
		if (pView->m_mgrProcedure->m_nMkPcs[2] < pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)	// 마킹완료Check
		{
			pView->m_mgrProcedure->m_nStepMk[2] = 3;
		}
		else
		{
			pView->m_mgrProcedure->m_nStepMk[2] = MK_END;
		}
		break;
	case MK_END:
		SetDelay0(100, 1);		// [mSec]
		pView->m_mgrProcedure->m_nStepMk[2]++;
		break;
	case 101:
		if (!WaitDelay0(1))		// F:Done, T:On Waiting....
		{
			if (IsMoveDone0())
			{
				MoveInitPos0();
				pView->m_mgrProcedure->m_nStepMk[2]++;
			}
		}
		break;
	case 102:
		if (IsMoveDone0())
			pView->m_mgrProcedure->m_nStepMk[2]++;
		break;
	case 103:
		pView->m_mgrProcedure->m_bDoneMk[0] = TRUE;
		pView->m_mgrProcedure->m_bTHREAD_MK[2] = FALSE;
		break;
	}
}

void CGvisR2R_PunchView::DoMark1All()
{
	//BOOL bOn;
	//int nSerial;
	CfPoint ptPnt;

	if (!pView->m_mgrProcedure->IsRun())
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

			if (pView->m_mgrProcedure->m_nStepMk[3] < 5 && pView->m_mgrProcedure->m_nStepMk[3] > 3) // Mk1();
			{
				pView->m_mgrProcedure->m_nStepMk[3] = 3;	// 마킹완료Check
			}
		}
		return;
	}


	switch (pView->m_mgrProcedure->m_nStepMk[3])
	{
	case 0:
		pView->m_mgrProcedure->m_nStepMk[3]++;
		break;
	case 1:
		if (pView->m_mgrProcedure->m_nMkPcs[3] < pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)
		{
			pView->m_mgrProcedure->m_nStepMk[3]++;
		}
		else
		{
			pView->m_mgrProcedure->m_nStepMk[3] = MK_END;
		}
		break;
	case 2:
		pView->m_mgrProcedure->m_nStepMk[3]++;
		break;
	case 3:
		if (pView->m_mgrProcedure->m_nMkPcs[3] < pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)	// 마킹완료Check
		{
			ptPnt = GetMkPnt(pView->m_mgrProcedure->m_nMkPcs[3]);

			if (ptPnt.x < 0.0 && ptPnt.y < 0.0) // 양품화.
			{
				pView->m_mgrProcedure->m_nMkPcs[3]++;
				break;
			}

			pView->m_mgrProcedure->m_dTarget[AXIS_X1] = ptPnt.x;

			// Cam0 : m_bPriority[0] <-, m_bPriority[3] ->
			// Cam1 : m_bPriority[1] ->, m_bPriority[2] <-

			pView->m_mgrProcedure->m_bCollision[1] = pView->m_mgrProcedure->ChkCollision(AXIS_X1, ptPnt.x);
			if (!pView->m_mgrProcedure->m_bCollision[1])
			{
				if (IsMoveDone1())
				{
					Move1(ptPnt, pView->m_mgrProcedure->m_bCam);
					pView->m_mgrProcedure->m_nStepMk[3]++;
				}
			}
			else if (pView->m_mgrProcedure->m_bPriority[1])
			{
				if (IsMoveDone1())
				{
					pView->m_mgrProcedure->m_bPriority[1] = FALSE;
					Move1(ptPnt, pView->m_mgrProcedure->m_bCam);
					pView->m_mgrProcedure->m_nStepMk[3]++;
				}
			}
			else if (pView->m_mgrProcedure->m_bPriority[3])
			{
				if (IsMoveDone1())
				{
					pView->m_mgrProcedure->m_bPriority[3] = FALSE;
					ptPnt.x = _tstof(pDoc->WorkingInfo.Motion.sSafeZone);
					ptPnt.y = 0.0;//m_dEnc[AXIS_Y1];
					Move1(ptPnt, FALSE);
				}
			}
		}
		else
		{
			pView->m_mgrProcedure->m_nStepMk[3] = MK_END;
		}
		break;
	case 4:
		if (IsMoveDone1())
			pView->m_mgrProcedure->m_nStepMk[3]++;
		break;
	case 5:
		if (!IsNoMk1())
			Mk1();
		else
			SetDelay1(pDoc->m_nDelayShow, 1);		// [mSec]
		pView->m_mgrProcedure->m_nStepMk[3]++;
		break;
	case 6:
		if (IsNoMk1())
		{
			if (!WaitDelay1(1))		// F:Done, T:On Waiting....
				pView->m_mgrProcedure->m_nStepMk[3]++;
		}
		else
			pView->m_mgrProcedure->m_nStepMk[3]++;
		break;
	case 7:
		pView->m_mgrProcedure->m_nMkPcs[3]++;
		pView->m_mgrProcedure->m_nStepMk[3]++;
		break;
	case 8:
		if (pView->m_mgrProcedure->m_nMkPcs[3] < pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)	// 마킹완료Check
		{
			pView->m_mgrProcedure->m_nStepMk[3] = 3;
		}
		else
		{
			//StopLive1();
			pView->m_mgrProcedure->m_nStepMk[3] = MK_END;
		}
		break;
	case MK_END:
		SetDelay1(100, 1);		// [mSec]
		pView->m_mgrProcedure->m_nStepMk[3]++;
		break;
	case 101:
		if (!WaitDelay1(1))		// F:Done, T:On Waiting....
		{
			if (pView->m_mgrProcedure->m_nMkPcs[2] < pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)
			{
				if (pView->m_mgrProcedure->m_nMkPcs[2] > 0)
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
						pView->m_mgrProcedure->m_nStepMk[3]++;
					}
				}
			}
			else
			{
				if (IsMoveDone1())
				{
					pView->m_mgrProcedure->m_Flag &= ~(0x01 << 0);
					MoveMkEdPos1();
					pView->m_mgrProcedure->m_nStepMk[3]++;
				}
			}
		}
		break;
	case 102:
		if (IsMoveDone0())
			pView->m_mgrProcedure->m_nStepMk[3]++;
		break;
	case 103:
		pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
		pView->m_mgrProcedure->m_bTHREAD_MK[3] = FALSE;
		break;
	}
}

void CGvisR2R_PunchView::DoMark0()
{
#ifdef TEST_MODE
	return;
#endif

	if (!pView->m_mgrProcedure->m_bAuto)
		return;


	//BOOL bOn;
	int nSerial, nIdx, nErrCode, nRtn;
	CfPoint ptPnt;
	CString sMsg;
	double dStripOut = (pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs / MAX_STRIP_NUM) * _tstof(pDoc->WorkingInfo.LastJob.sStripOutRatio) / 100.0;
	int nStripOut = int(dStripOut);
	if (dStripOut > nStripOut)
		nStripOut++;			// 스트립 양폐 비율

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (!pView->m_mgrProcedure->IsRun())																		// 정지상태에서
	{
		if (IsOnMarking0())																// 마킹중에
		{
			if (m_pMotion->IsEnable(MS_X0) && m_pMotion->IsEnable(MS_Y0))				// 모션이 Enable상태이고
			{
				if (m_pMotion->IsMotionDone(MS_X0) && m_pMotion->IsMotionDone(MS_Y0))	// 모션 Done상태이면,
				{
					if (!IsInitPos0() && !IsPinPos0())									// 초기위치가 아니거나, 핀위치가 아닐때
						MoveInitPos0();													// 초기위치로 이동
				}
			}

			if (pView->m_mgrProcedure->m_nStepMk[0] < 13 && pView->m_mgrProcedure->m_nStepMk[0] > 8) // Mk0();
			{
				pView->m_mgrProcedure->m_nStepMk[0] = 8;	// 마킹완료Check
			}
		}
		return;
	}

	pView->m_mgrProcedure->m_sDispSts[0].Format(_T("%d"), pView->m_mgrProcedure->m_nStepMk[0]);

	switch (pView->m_mgrProcedure->m_nStepMk[0])
	{
	case 0:
		if (IsNoMk())
			ShowLive();
		pView->m_mgrProcedure->m_nStepMk[0]++;
		break;
	case 1:
		if (!IsInitPos0())
			MoveInitPos0();
		pView->m_mgrProcedure->m_nStepMk[0]++;
		break;
	case 2:

		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Cam0

		//nSerial = 1;

		if (nSerial > 0)
		{
			//if ((nErrCode = GetErrCode0(nSerial)) != 1)
			if ((nErrCode = GetErrCode(nSerial)) != 1)
			{
				pView->m_mgrProcedure->m_nMkPcs[0] = GetTotDefPcs(nSerial);
				pView->m_mgrProcedure->m_nStepMk[0] = MK_END;
			}
			else
			{
				pView->m_mgrProcedure->m_nStepMk[0]++;
			}
		}
		else
		{
			StopFromThread();
			AsyncMsgBox(_T("버퍼의 시리얼이 맞지않습니다."), 1);
			BuzzerFromThread(TRUE, 0);
			//pView->DispStsBar(_T("정지-31"), 0);
			DispMain(_T("정 지"), RGB_RED);
		}
		break;
	case 3:
		pView->m_mgrProcedure->m_nStepMk[0]++;
		break;
	case 4:
		pView->m_mgrProcedure->m_nStepMk[0]++;
		break;
	case 5:
		pView->m_mgrProcedure->m_nStepMk[0]++;
		break;
	case 6:

		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Cam0

		if (pView->m_mgrProcedure->m_nMkPcs[0] < GetTotDefPcs(nSerial))
		{
			if (!IsNoMk0())
			{
				;
			}
			else
			{
				if (!IsReview0())
				{
					if (pView->m_mgrProcedure->m_bReview)
					{
						pView->m_mgrProcedure->m_nMkPcs[0] = GetTotDefPcs(nSerial);
						pView->m_mgrProcedure->m_nStepMk[0] = MK_END;
						break;
					}
				}
			}
			// Punching On이거나 Review이면 다음으로 진행
			SetDelay0(100, 1);		// [mSec]
			pView->m_mgrProcedure->m_nStepMk[0]++;
		}
		else
		{
			pView->m_mgrProcedure->m_nStepMk[0] = MK_END;
		}
		break;
	case 7:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Cam0

		if (!WaitDelay0(1))		// F:Done, T:On Waiting....		// Delay후에
		{
			pView->m_mgrProcedure->m_nMkPcs[0] = 0;

			if (!IsNoMk0())										// Punching On이면
			{
				pView->m_mgrProcedure->m_nStepMk[0]++;
			}
			else												// Punching이 Off이고
			{
				if (IsReview0())								// Review이면 다음으로
				{
					pView->m_mgrProcedure->m_nStepMk[0]++;
				}
				else											// Review가 아니면
				{
					if (pView->m_mgrProcedure->m_bReview)
					{
						pView->m_mgrProcedure->m_nMkPcs[0] = GetTotDefPcs(nSerial);
						pView->m_mgrProcedure->m_nStepMk[0] = MK_END;
					}
					else
						pView->m_mgrProcedure->m_nStepMk[0]++;
				}
			}
		}
		break;
	case 8:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Cam0

		if (pView->m_mgrProcedure->m_nMkPcs[0] < GetTotDefPcs(nSerial))	// 마킹완료Check
		{
			if (pView->m_mgrProcedure->m_nMkPcs[0] + 1 < GetTotDefPcs(nSerial))		// 다음 마킹위치가 있으면
			{
				ptPnt = GetMkPnt(nSerial, pView->m_mgrProcedure->m_nMkPcs[0] + 1);	// 다음 마킹위치
				pView->m_mgrProcedure->m_dNextTarget[AXIS_X0] = ptPnt.x;
				pView->m_mgrProcedure->m_dNextTarget[AXIS_Y0] = ptPnt.y;
			}
			else												// 다음 마킹위치가 없으면
			{
				pView->m_mgrProcedure->m_dNextTarget[AXIS_X0] = -1.0;
				pView->m_mgrProcedure->m_dNextTarget[AXIS_Y0] = -1.0;
			}

			ptPnt = GetMkPnt(nSerial, pView->m_mgrProcedure->m_nMkPcs[0]);			// 이번 마킹위치
			if (ptPnt.x < 0.0 && ptPnt.y < 0.0) // 양품화. (마킹하지 않음)
			{
				pView->m_mgrProcedure->m_nMkPcs[0]++;
				pView->m_mgrProcedure->m_nStepMk[0] = MK_DONE_CHECK;
				break;
			}

			nIdx = GetMkStripIdx(nSerial, pView->m_mgrProcedure->m_nMkPcs[0]);		// 1 ~ 4 : strip index
			if (nIdx > 0)										// Strip index가 정상이면,
			{
				if (!IsMkStrip(nIdx)) // Strip[] Mk Off
				{
					pView->m_mgrProcedure->m_nMkPcs[0]++;
					pView->m_mgrProcedure->m_nStepMk[0] = MK_DONE_CHECK;
					break;
				}
				else
				{
					if (pView->m_mgrProcedure->m_nMkStrip[0][nIdx - 1] >= nStripOut)
					{
						pView->m_mgrProcedure->m_nMkPcs[0]++;
						pView->m_mgrProcedure->m_nStepMk[0] = MK_DONE_CHECK;
						break;
					}
					else
						pView->m_mgrProcedure->m_nMkStrip[0][nIdx - 1]++;
				}
			}
			else
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Strip Index Failed."));
				break;
			}

			pView->m_mgrProcedure->m_dTarget[AXIS_X0] = ptPnt.x;
			pView->m_mgrProcedure->m_dTarget[AXIS_Y0] = ptPnt.y;

			pView->m_mgrProcedure->m_nStepMk[0]++;
		}
		else
		{
			pView->m_mgrProcedure->m_nStepMk[0] = MK_END;
		}
		break;
	case 9:
		// Cam0 : m_bPriority[0] <-, m_bPriority[3] ->
		// Cam1 : m_bPriority[1] ->, m_bPriority[2] <-
		ptPnt.x = pView->m_mgrProcedure->m_dTarget[AXIS_X0];
		ptPnt.y = pView->m_mgrProcedure->m_dTarget[AXIS_Y0];

		if (pView->m_mgrProcedure->m_dNextTarget[AXIS_X0] < 0)
			pView->m_mgrProcedure->m_bCollision[0] = pView->m_mgrProcedure->ChkCollision(AXIS_X0, pView->m_mgrProcedure->m_dTarget[AXIS_X0]);
		else
			pView->m_mgrProcedure->m_bCollision[0] = pView->m_mgrProcedure->ChkCollision(AXIS_X0, pView->m_mgrProcedure->m_dTarget[AXIS_X0], pView->m_mgrProcedure->m_dNextTarget[AXIS_X0]);

		if (!pView->m_mgrProcedure->m_bCollision[0])
		{
			if (IsMoveDone0())
			{
				Move0(ptPnt, pView->m_mgrProcedure->m_bCam);
				pView->m_mgrProcedure->m_nStepMk[0]++;
			}
		}
		else if (pView->m_mgrProcedure->m_bPriority[0])
		{
			if (IsMoveDone0())
			{
				pView->m_mgrProcedure->m_bCollision[0] = FALSE;
				pView->m_mgrProcedure->m_bPriority[0] = FALSE;
				Move0(ptPnt, pView->m_mgrProcedure->m_bCam);
				pView->m_mgrProcedure->m_nStepMk[0]++;
			}
		}
		else if (pView->m_mgrProcedure->m_bPriority[2])
		{
			if (IsMoveDone0())
			{
				pView->m_mgrProcedure->m_bCollision[0] = FALSE;
				pView->m_mgrProcedure->m_bPriority[2] = FALSE;
				ptPnt.x = 0.0;
				ptPnt.y = 0.0;//m_dEnc[AXIS_Y0];
				Move0(ptPnt, FALSE);
			}
		}
		break;
	case 10:
		pView->m_mgrProcedure->m_nStepMk[0]++;
		break;
	case 11:
		pView->m_mgrProcedure->m_nStepMk[0]++;
		break;
	case 12:
		if (IsMoveDone0())
			pView->m_mgrProcedure->m_nStepMk[0]++;
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
			if(!SaveMk0Img(pView->m_mgrProcedure->m_nMkPcs[0]))
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Error-SaveMk0Img()"));
			}
			//m_nDebugStep = m_nMkPcs[0]; DispThreadTick();
		}
		pView->m_mgrProcedure->m_nStepMk[0]++;
		break;
	case 14:
		pView->m_mgrProcedure->m_nStepMk[0]++;
		break;
	case 15:
		pView->m_mgrProcedure->m_nStepMk[0]++;
		break;
	case 16:
		if (IsNoMk0())
		{
			if (!WaitDelay0(1))		// F:Done, T:On Waiting....
				pView->m_mgrProcedure->m_nStepMk[0]++;
		}
		else
			pView->m_mgrProcedure->m_nStepMk[0]++;
		break;
	case 17:
		if (!IsNoMk0())
		{
			if (IsMk0Done())
			{
				// One more MK On Start....
				if (!pView->m_mgrProcedure->m_nMkPcs[0] && !pView->m_mgrProcedure->m_bAnswer[2])
				{
					pView->m_mgrProcedure->m_bAnswer[2] = TRUE;
					Mk0();
				}
				else
				{
					pView->m_mgrProcedure->m_bAnswer[2] = FALSE;
					pView->m_mgrProcedure->m_nMkPcs[0]++;
					pView->m_mgrProcedure->m_nStepMk[0]++;
					pView->m_mgrProcedure->m_nStepMk[0]++;
				}
			}
			else
			{
				if (pView->m_mgrProcedure->m_dwStMkDn[0] + 5000 < GetTickCount())
				{
					BuzzerFromThread(TRUE, 0);
					DispMain(_T("정 지"), RGB_RED);
					m_pVoiceCoil[0]->SearchHomeSmac0();

					nRtn = AsyncMsgBox(_T("보이스코일(좌) 통신완료가 않됩니다.\r\n마킹을 다시 시도하시겠습니까?"), 1, MB_YESNO);
					if (IDYES == nRtn)
					{
						DispMain(_T("운전중"), RGB_RED);
						pView->m_mgrProcedure->m_nStepMk[0] = 13;
					}
					else if (nRtn < 0)
						pView->m_mgrProcedure->m_nStepMk[0]++; // Wait...
					else
					{
						pView->m_mgrProcedure->m_bAnswer[2] = FALSE;
						pView->m_mgrProcedure->m_nMkPcs[0]++;
						pView->m_mgrProcedure->m_nStepMk[0]++;
						pView->m_mgrProcedure->m_nStepMk[0]++;
						Stop();
					}
				}
			}
		}
		else
		{
			pView->m_mgrProcedure->m_nMkPcs[0]++;
			pView->m_mgrProcedure->m_nStepMk[0]++;
			pView->m_mgrProcedure->m_nStepMk[0]++;
		}
		break;
	case 18:
		if ((nRtn = WaitRtnVal(1)) > -1)
		{
			if (IDYES == nRtn)
			{
				DispMain(_T("운전중"), RGB_RED);
				pView->m_mgrProcedure->m_nStepMk[0] = 13;
			}
			else
			{
				pView->m_mgrProcedure->m_bAnswer[2] = FALSE;
				pView->m_mgrProcedure->m_nMkPcs[0]++;
				pView->m_mgrProcedure->m_nStepMk[0]++;
				StopFromThread();
			}
		}
		break;
	case 19:
		pView->m_mgrProcedure->m_nStepMk[0] = MK_DONE_CHECK;
		break;
	case MK_DONE_CHECK:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Cam0

		if (pView->m_mgrProcedure->m_nMkPcs[0] < GetTotDefPcs(nSerial))
		{
			if (IsNoMk0())
			{
				if (!pDoc->WorkingInfo.System.bNoMk)
				{
					if (IsReview0())
					{
						if (IsJogRtDn0())
							pView->m_mgrProcedure->m_nStepMk[0]++;
					}
					else if (!pDoc->WorkingInfo.LastJob.bVerify)
					{
						pView->m_mgrProcedure->m_nMkPcs[0] = GetTotDefPcs(nSerial);
						pView->m_mgrProcedure->m_nStepMk[0] = MK_END;
					}
					else
						pView->m_mgrProcedure->m_nStepMk[0] = 8;	// 마킹완료Check
				}
				else
					pView->m_mgrProcedure->m_nStepMk[0] = 8;	// 마킹완료Check
			}
			else
				pView->m_mgrProcedure->m_nStepMk[0] = 8;	// 마킹완료Check
		}
		else
		{
			if (IsNoMk0())
			{
				if (IsReview0())
				{
					if (IsJogRtDn0())
						pView->m_mgrProcedure->m_nStepMk[0] = MK_END;
				}
				else
				{
					pView->m_mgrProcedure->m_nStepMk[0] = MK_END;
				}
			}
			else
				pView->m_mgrProcedure->m_nStepMk[0] = MK_END;
		}
		break;
	case MK_DONE_CHECK + 1:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Cam0

		if (pView->m_mgrProcedure->m_nMkPcs[0] < GetTotDefPcs(nSerial))
		{
			if (IsNoMk0())
			{
				if (IsReview0())
				{
					if (IsJogRtUp0())
						pView->m_mgrProcedure->m_nStepMk[0] = 8;	// 마킹완료Check
				}
				else
				{
					pView->m_mgrProcedure->m_nMkPcs[0] = GetTotDefPcs(nSerial);
					pView->m_mgrProcedure->m_nStepMk[0] = MK_END;
				}
			}
			else
				pView->m_mgrProcedure->m_nStepMk[0] = 8;	// 마킹완료Check
		}
		else
		{
			pView->m_mgrProcedure->m_nStepMk[0] = MK_END;
		}
		break;

	case MK_END:
		if (IsMoveDone0())
			pView->m_mgrProcedure->m_nStepMk[0]++;
		break;
	case 101:
		SetDelay0(100, 1);		// [mSec]
		pView->m_mgrProcedure->m_nStepMk[0]++;
		break;
	case 102:
		if (!WaitDelay0(1))		// F:Done, T:On Waiting....
		{
			pView->m_mgrProcedure->m_nStepMk[0]++;
		}
		break;
	case 103:
		if (!IsInitPos0())
		{
			pView->m_mgrProcedure->m_dTarget[AXIS_X0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[0]);
			pView->m_mgrProcedure->m_dTarget[AXIS_Y0] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[0]);
			pView->m_mgrProcedure->m_dNextTarget[AXIS_X0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[0]);
			pView->m_mgrProcedure->m_dNextTarget[AXIS_Y0] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[0]);

			MoveInitPos0();
		}

		pDoc->SaveMkCntL();
		pView->m_mgrProcedure->m_nStepMk[0]++;
		break;
	case 104:
		if (IsMoveDone0())
		{
			pView->m_mgrProcedure->m_nStepMk[0]++;
			SetDelay0(10000, 1);		// [mSec]
		}
		break;
	case 105:
		if (IsInitPos0())
		{
			pView->m_mgrProcedure->m_nStepMk[0]++;
		}
		else
		{
			if (!WaitDelay0(1))		// F:Done, T:On Waiting....
			{
				pView->m_mgrProcedure->m_nStepMk[0] = ERR_PROC;
			}
		}
		break;
	case 106: // MK Done....
		pView->m_mgrProcedure->m_bDoneMk[0] = TRUE;
		pView->m_mgrProcedure->m_bTHREAD_MK[0] = FALSE;
		break;

	case ERR_PROC:
		DispMain(_T("정 지"), RGB_RED);
		m_pVoiceCoil[0]->SearchHomeSmac0();
		AsyncMsgBox(_T("보이스코일(좌) 초기위치 이동이 되지 않습니다.\r\n마킹상태를 확인하세요."), 1);
		pView->m_mgrProcedure->m_nStepMk[0]++;
		break;
	case ERR_PROC + 1:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Cam0

		if (pView->m_mgrProcedure->m_bCam)
			sMsg.Format(_T("%d번 Shot을 다시 불량확인을 하시겠습니까?"), nSerial);
		else
			sMsg.Format(_T("%d번 Shot을 다시 마킹하시겠습니까?"), nSerial);

		pView->m_mgrProcedure->m_nRtnMyMsgBoxIdx = 0;
		pView->m_mgrProcedure->m_bRtnMyMsgBox[0] = FALSE;
		pView->m_mgrProcedure->m_nRtnMyMsgBox[0] = -1;
		pView->AsyncMsgBox(sMsg, 1, MB_YESNO);
		sMsg.Empty();
		pView->m_mgrProcedure->m_nStepMk[0]++;
		break;
	case ERR_PROC + 2:
		if ((nRtn = WaitRtnVal()) > -1)
		{
			if (IDYES == nRtn)
			{
				pView->m_mgrProcedure->m_nStepMk[0] = ERR_PROC + 10;
			}
			else
			{
				pView->m_mgrProcedure->m_nRtnMyMsgBoxIdx = 0;
				pView->m_mgrProcedure->m_bRtnMyMsgBox[0] = FALSE;
				pView->m_mgrProcedure->m_nRtnMyMsgBox[0] = -1;
				sMsg.Format(_T("계속 다음 작업을 진행하시겠습니까?"), nSerial);
				pView->AsyncMsgBox(sMsg, 1, MB_YESNO);
				sMsg.Empty();

				pView->m_mgrProcedure->m_nStepMk[0]++;
			}
		}
		break;
	case ERR_PROC + 3:
		if ((nRtn = WaitRtnVal()) > -1)
		{
			if (IDYES == nRtn)
			{
				pView->m_mgrProcedure->m_nStepMk[0] = ERR_PROC + 20;
			}
			else
			{
				pView->m_mgrProcedure->m_bDispMsgDoAuto[8] = TRUE;
				pView->m_mgrProcedure->m_nStepDispMsg[8] = FROM_DOMARK0;
			}
		}
		break;
	case ERR_PROC + 10:
		pView->m_mgrProcedure->m_bReMark[0] = TRUE;
		pView->m_mgrProcedure->m_bTHREAD_MK[0] = FALSE;
		pView->m_mgrProcedure->m_nStepMk[0] = 0;
		break;
	case ERR_PROC + 20: // MK Done....
		pView->m_mgrProcedure->m_bDoneMk[0] = TRUE;
		pView->m_mgrProcedure->m_bTHREAD_MK[0] = FALSE;
		break;
	}
}

BOOL CGvisR2R_PunchView::SaveMk0Img(int nMkPcsIdx) // Cam0
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
		sPath.Format(_T("%s\\%s.tif"), sDest, GetMkInfo(nSerial, nMkPcsIdx));

#ifdef USE_VISION
		if (m_pVision[0])
			return m_pVision[0]->SaveMkImg(sPath);
#endif
	}
	else
		return TRUE;

	return FALSE;
}

void CGvisR2R_PunchView::DoMark1()
{
#ifdef TEST_MODE
	return;
#endif

	if (!pView->m_mgrProcedure->m_bAuto)
		return;

	//BOOL bOn;
	int nSerial, nIdx, nErrCode, nRtn;
	CfPoint ptPnt;
	CString sMsg;
	double dStripOut = (pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs / MAX_STRIP_NUM) * _tstof(pDoc->WorkingInfo.LastJob.sStripOutRatio) / 100.0;
	int nStripOut = int(dStripOut);
	if (dStripOut > nStripOut)
		nStripOut++;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (!pView->m_mgrProcedure->IsRun())
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

			if (pView->m_mgrProcedure->m_nStepMk[1] < 13 && pView->m_mgrProcedure->m_nStepMk[1] > 8) // Mk1();
			{
				pView->m_mgrProcedure->m_nStepMk[1] = 8;	// 마킹완료Check
			}
		}
		return;
	}


	pView->m_mgrProcedure->m_sDispSts[1].Format(_T("%d"), pView->m_mgrProcedure->m_nStepMk[1]);

	switch (pView->m_mgrProcedure->m_nStepMk[1])
	{
	case 0:
		if (IsNoMk())
			ShowLive();
		pView->m_mgrProcedure->m_nStepMk[1]++;
		break;
	case 1:
		if (!IsInitPos1())
			MoveInitPos1();
		pView->m_mgrProcedure->m_nStepMk[1]++;
		break;
	case 2:

		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

		if (nSerial > 0)
		{
			//if ((nErrCode = GetErrCode1(nSerial)) != 1)
			if ((nErrCode = GetErrCode(nSerial)) != 1)
			{
				pView->m_mgrProcedure->m_nMkPcs[1] = GetTotDefPcs(nSerial);
				pView->m_mgrProcedure->m_nStepMk[1] = MK_END;
			}
			else
			{
				pView->m_mgrProcedure->m_nStepMk[1]++;
			}
		}
		else
		{
			StopFromThread();
			AsyncMsgBox(_T("버퍼의 시리얼이 맞지않습니다."), 2);
			BuzzerFromThread(TRUE, 0);
			DispMain(_T("정 지"), RGB_RED);
		}
		break;
	case 3:
		pView->m_mgrProcedure->m_nStepMk[1]++;
		break;
	case 4:
		pView->m_mgrProcedure->m_nStepMk[1]++;
		break;
	case 5:
		pView->m_mgrProcedure->m_nStepMk[1]++;
		break;
	case 6:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

		if (pView->m_mgrProcedure->m_nMkPcs[1] < GetTotDefPcs(nSerial))
		{
			if (!IsNoMk1())
			{
				;
			}
			else
			{
				if (!IsReview1())
				{
					if (pView->m_mgrProcedure->m_bReview)
					{
						pView->m_mgrProcedure->m_nMkPcs[1] = GetTotDefPcs(nSerial);
						pView->m_mgrProcedure->m_nStepMk[1] = MK_END;
						break;
					}
				}
			}
			SetDelay1(100, 6);		// [mSec]
			pView->m_mgrProcedure->m_nStepMk[1]++;
		}
		else
		{
			pView->m_mgrProcedure->m_nStepMk[1] = MK_END;
		}
		break;
	case 7:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

		if (!WaitDelay1(6))		// F:Done, T:On Waiting....
		{
			pView->m_mgrProcedure->m_nMkPcs[1] = 0;

			if (!IsNoMk1())
			{
				pView->m_mgrProcedure->m_nStepMk[1]++;
			}
			else
			{
				if (IsReview1())
				{
					pView->m_mgrProcedure->m_nStepMk[1]++;
				}
				else
				{
					if (pView->m_mgrProcedure->m_bReview)
					{
						pView->m_mgrProcedure->m_nMkPcs[1] = GetTotDefPcs(nSerial);
						pView->m_mgrProcedure->m_nStepMk[1] = MK_END;
					}
					else
						pView->m_mgrProcedure->m_nStepMk[1]++;
				}
			}
		}
		break;
	case 8:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

		if (pView->m_mgrProcedure->m_nMkPcs[1] < GetTotDefPcs(nSerial))	// 마킹완료Check
		{
			if (pView->m_mgrProcedure->m_nMkPcs[1] + 1 < GetTotDefPcs(nSerial))
			{
				ptPnt = GetMkPnt(nSerial, pView->m_mgrProcedure->m_nMkPcs[1] + 1);
				pView->m_mgrProcedure->m_dNextTarget[AXIS_X1] = ptPnt.x;
				pView->m_mgrProcedure->m_dNextTarget[AXIS_Y1] = ptPnt.y;
			}
			else
			{
				pView->m_mgrProcedure->m_dNextTarget[AXIS_X1] = -1.0;
				pView->m_mgrProcedure->m_dNextTarget[AXIS_Y1] = -1.0;
			}

			ptPnt = GetMkPnt(nSerial, pView->m_mgrProcedure->m_nMkPcs[1]);
			if (ptPnt.x < 0.0 && ptPnt.y < 0.0) // 양품화.
			{
				pView->m_mgrProcedure->m_nMkPcs[1]++;
				pView->m_mgrProcedure->m_nStepMk[1] = MK_DONE_CHECK;
				break;
			}

			nIdx = GetMkStripIdx(nSerial, pView->m_mgrProcedure->m_nMkPcs[1]);
			if (nIdx > 0)
			{
				if (!IsMkStrip(nIdx)) // Strip[] Mk Off
				{
					pView->m_mgrProcedure->m_nMkPcs[1]++;
					pView->m_mgrProcedure->m_nStepMk[1] = MK_DONE_CHECK;
					break;
				}
				else
				{
					if (pView->m_mgrProcedure->m_nMkStrip[1][nIdx - 1] >= nStripOut)
					{
						pView->m_mgrProcedure->m_nMkPcs[1]++;
						pView->m_mgrProcedure->m_nStepMk[1] = MK_DONE_CHECK;
						break;
					}
					else
						pView->m_mgrProcedure->m_nMkStrip[1][nIdx - 1]++;
				}
			}
			else
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Strip Index Failed."));
				break;
			}

			pView->m_mgrProcedure->m_dTarget[AXIS_X1] = ptPnt.x;
			pView->m_mgrProcedure->m_dTarget[AXIS_Y1] = ptPnt.y;

			pView->m_mgrProcedure->m_nStepMk[1]++;
		}
		else
		{
			pView->m_mgrProcedure->m_nStepMk[1] = MK_END;
		}
		break;
	case 9:
		// Cam0 : m_bPriority[0] <-, m_bPriority[3] ->
		// Cam1 : m_bPriority[1] ->, m_bPriority[2] <-
		ptPnt.x = pView->m_mgrProcedure->m_dTarget[AXIS_X1];
		ptPnt.y = pView->m_mgrProcedure->m_dTarget[AXIS_Y1];

		if (pView->m_mgrProcedure->m_dNextTarget[AXIS_X1] < 0)
			pView->m_mgrProcedure->m_bCollision[1] = pView->m_mgrProcedure->ChkCollision(AXIS_X1, pView->m_mgrProcedure->m_dTarget[AXIS_X1]);
		else
			pView->m_mgrProcedure->m_bCollision[1] = pView->m_mgrProcedure->ChkCollision(AXIS_X1, pView->m_mgrProcedure->m_dTarget[AXIS_X1], pView->m_mgrProcedure->m_dNextTarget[AXIS_X1]);

		if (!pView->m_mgrProcedure->m_bCollision[1])
		{
			if (IsMoveDone1())
			{
				Move1(ptPnt, pView->m_mgrProcedure->m_bCam);
				pView->m_mgrProcedure->m_nStepMk[1]++;
			}
		}
		else if (pView->m_mgrProcedure->m_bPriority[1])
		{
			if (IsMoveDone1())
			{
				pView->m_mgrProcedure->m_bCollision[1] = FALSE;
				pView->m_mgrProcedure->m_bPriority[1] = FALSE;
				Move1(ptPnt, pView->m_mgrProcedure->m_bCam);
				pView->m_mgrProcedure->m_nStepMk[1]++;
			}
		}
		else if (pView->m_mgrProcedure->m_bPriority[3])
		{
			if (IsMoveDone1())
			{
				pView->m_mgrProcedure->m_bCollision[1] = FALSE;
				pView->m_mgrProcedure->m_bPriority[3] = FALSE;
				ptPnt.x = _tstof(pDoc->WorkingInfo.Motion.sSafeZone);
				ptPnt.y = 0.0;//m_dEnc[AXIS_Y1];
				Move1(ptPnt, FALSE);
			}
		}
		break;
	case 10:
		pView->m_mgrProcedure->m_nStepMk[1]++;
		break;
	case 11:
		pView->m_mgrProcedure->m_nStepMk[1]++;
		break;
	case 12:
		if (IsMoveDone1())
			pView->m_mgrProcedure->m_nStepMk[1]++;
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
			if(!SaveMk1Img(pView->m_mgrProcedure->m_nMkPcs[1]))
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Error-SaveMk1Img()"));
			}
		}
		pView->m_mgrProcedure->m_nStepMk[1]++;
		break;
	case 14:
		pView->m_mgrProcedure->m_nStepMk[1]++;
		break;
	case 15:
		pView->m_mgrProcedure->m_nStepMk[1]++;
		break;
	case 16:
		if (IsNoMk1())
		{
			if (!WaitDelay1(6))		// F:Done, T:On Waiting....
				pView->m_mgrProcedure->m_nStepMk[1]++;
		}
		else
			pView->m_mgrProcedure->m_nStepMk[1]++;
		break;
	case 17:
		if (!IsNoMk1())
		{
			if (IsMk1Done())
			{
				// One more MK On Start....
				if (!pView->m_mgrProcedure->m_nMkPcs[1] && !pView->m_mgrProcedure->m_bAnswer[3])
				{
					pView->m_mgrProcedure->m_bAnswer[3] = TRUE;
					Mk1();
				}
				else
				{
					pView->m_mgrProcedure->m_bAnswer[3] = FALSE;
					pView->m_mgrProcedure->m_nMkPcs[1]++;
					pView->m_mgrProcedure->m_nStepMk[1]++;
					pView->m_mgrProcedure->m_nStepMk[1]++;
				}

			}
			else
			{
				if (pView->m_mgrProcedure->m_dwStMkDn[1] + 5000 < GetTickCount())
				{
					BuzzerFromThread(TRUE, 0);
					DispMain(_T("정 지"), RGB_RED);
					m_pVoiceCoil[1]->SearchHomeSmac1();

					nRtn = AsyncMsgBox(_T("보이스코일(우) 통신완료가 않됩니다.\r\n마킹을 다시 시도하시겠습니까?"), 2, MB_YESNO);
					if (IDYES == nRtn)
					{
						DispMain(_T("운전중"), RGB_RED);
						pView->m_mgrProcedure->m_nStepMk[1] = 13;
					}
					else if (nRtn < 0)
						pView->m_mgrProcedure->m_nStepMk[1]++; // Wait...
					else
					{
						pView->m_mgrProcedure->m_bAnswer[3] = FALSE;
						pView->m_mgrProcedure->m_nMkPcs[1]++;
						pView->m_mgrProcedure->m_nStepMk[1]++;
						pView->m_mgrProcedure->m_nStepMk[1]++;
						Stop();
					}
				}
			}
		}
		else
		{
			pView->m_mgrProcedure->m_nMkPcs[1]++;
			pView->m_mgrProcedure->m_nStepMk[1]++;
			pView->m_mgrProcedure->m_nStepMk[1]++;
		}
		break;
	case 18:
		if ((nRtn = WaitRtnVal(2)) > -1)
		{
			if (IDYES == nRtn)
			{
				DispMain(_T("운전중"), RGB_RED);
				pView->m_mgrProcedure->m_nStepMk[1] = 13;
			}
			else
			{
				pView->m_mgrProcedure->m_bAnswer[3] = FALSE;
				pView->m_mgrProcedure->m_nMkPcs[1]++;
				pView->m_mgrProcedure->m_nStepMk[1]++;
				StopFromThread();
			}
		}
		break;
	case 19:
		pView->m_mgrProcedure->m_nStepMk[1] = MK_DONE_CHECK;
		break;
	case MK_DONE_CHECK:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

		if (pView->m_mgrProcedure->m_nMkPcs[1] < GetTotDefPcs(nSerial))
		{
			if (IsNoMk1())
			{
				if (!pDoc->WorkingInfo.System.bNoMk)
				{
					if (IsReview1())
					{
						if (IsJogRtDn1())
							pView->m_mgrProcedure->m_nStepMk[1]++;
					}
					else if (!pDoc->WorkingInfo.LastJob.bVerify)
					{
						pView->m_mgrProcedure->m_nMkPcs[1] = GetTotDefPcs(nSerial);
						pView->m_mgrProcedure->m_nStepMk[1] = MK_END;
					}
					else
						pView->m_mgrProcedure->m_nStepMk[1] = 8;	// 마킹완료Check
				}
				else
					pView->m_mgrProcedure->m_nStepMk[1] = 8;	// 마킹완료Check
			}
			else
				pView->m_mgrProcedure->m_nStepMk[1] = 8;	// 마킹완료Check
		}
		else
		{
			if (IsNoMk1())
			{
				if (IsReview1())
				{
					if (IsJogRtDn1())
						pView->m_mgrProcedure->m_nStepMk[1] = MK_END;
				}
				else
				{
					pView->m_mgrProcedure->m_nStepMk[1] = MK_END;
				}
			}
			else
				pView->m_mgrProcedure->m_nStepMk[1] = MK_END;
		}
		break;
	case MK_DONE_CHECK + 1:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

		if (pView->m_mgrProcedure->m_nMkPcs[1] < GetTotDefPcs(nSerial))
		{
			if (IsNoMk1())
			{
				if (IsReview1())
				{
					if (IsJogRtUp1())
						pView->m_mgrProcedure->m_nStepMk[1] = 8;	// 마킹완료Check
				}
				else
				{
					pView->m_mgrProcedure->m_nMkPcs[1] = GetTotDefPcs(nSerial);
					pView->m_mgrProcedure->m_nStepMk[1] = MK_END;
				}
			}
			else
				pView->m_mgrProcedure->m_nStepMk[1] = 8;	// 마킹완료Check
		}
		else
		{
			pView->m_mgrProcedure->m_nStepMk[1] = MK_END;
		}
		break;


	case MK_END:
		if (IsMoveDone1())
			pView->m_mgrProcedure->m_nStepMk[1]++;
		break;
	case 101:
		SetDelay1(100, 6);		// [mSec]
		pView->m_mgrProcedure->m_nStepMk[1]++;
		break;
	case 102:
		if (!WaitDelay1(6))		// F:Done, T:On Waiting....
			pView->m_mgrProcedure->m_nStepMk[1]++;
		break;
	case 103:
		if (!IsMkEdPos1())
		{
			pView->m_mgrProcedure->m_dTarget[AXIS_X1] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[1]);
			pView->m_mgrProcedure->m_dTarget[AXIS_Y1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[1]);
			pView->m_mgrProcedure->m_dNextTarget[AXIS_X1] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[1]);
			pView->m_mgrProcedure->m_dNextTarget[AXIS_Y1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[1]);

			MoveMkEdPos1();
		}

		pDoc->SaveMkCntR();
		pView->m_mgrProcedure->m_nStepMk[1]++;
		break;
	case 104:
		if (IsMoveDone1())
		{
			pView->m_mgrProcedure->m_nStepMk[1]++;
			SetDelay1(10000, 6);		// [mSec]
		}
		break;
	case 105:
		if (IsMkEdPos1())
		{
			pView->m_mgrProcedure->m_nStepMk[1]++;
		}
		else
		{
			if (!WaitDelay1(6))		// F:Done, T:On Waiting....
			{
				pView->m_mgrProcedure->m_nStepMk[1] = ERR_PROC;
			}
		}
		break;
	case 106: // MK Done....
		pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
		pView->m_mgrProcedure->m_bTHREAD_MK[1] = FALSE;
		break;

	case ERR_PROC:
		DispMain(_T("정 지"), RGB_RED);
		m_pVoiceCoil[1]->SearchHomeSmac1();
		AsyncMsgBox(_T("보이스코일(우) 초기위치 이동이 되지 않습니다.\r\n마킹상태를 확인하세요."), 2);
		pView->m_mgrProcedure->m_nStepMk[1]++;
		break;
	case ERR_PROC + 1:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

		if (pView->m_mgrProcedure->m_bCam)
			sMsg.Format(_T("%d번 Shot을 다시 불량확인을 하시겠습니까?"), nSerial);
		else
			sMsg.Format(_T("%d번 Shot을 다시 마킹하시겠습니까?"), nSerial);

		pView->m_mgrProcedure->m_nRtnMyMsgBoxIdx = 1;
		pView->m_mgrProcedure->m_bRtnMyMsgBox[1] = FALSE;
		pView->m_mgrProcedure->m_nRtnMyMsgBox[1] = -1;
		pView->AsyncMsgBox(sMsg, 2, MB_YESNO);
		sMsg.Empty();
		pView->m_mgrProcedure->m_nStepMk[1]++;
		break;
	case ERR_PROC + 2:
		if ((nRtn = WaitRtnVal()) > -1)
		{
			if (IDYES == nRtn)
			{
				pView->m_mgrProcedure->m_nStepMk[1] = ERR_PROC + 10;
			}
			else
			{
				pView->m_mgrProcedure->m_nRtnMyMsgBoxIdx = 1;
				pView->m_mgrProcedure->m_bRtnMyMsgBox[1] = FALSE;
				pView->m_mgrProcedure->m_nRtnMyMsgBox[1] = -1;
				sMsg.Format(_T("계속 다음 작업을 진행하시겠습니까?"), nSerial);
				pView->AsyncMsgBox(sMsg, 2, MB_YESNO);
				sMsg.Empty();

				pView->m_mgrProcedure->m_nStepMk[1]++;

			}
		}
		break;
	case ERR_PROC + 3:
		if ((nRtn = WaitRtnVal()) > -1)
		{
			if (IDYES == nRtn)
			{
				pView->m_mgrProcedure->m_nStepMk[1] = ERR_PROC + 20;
			}
			else
			{
				pView->m_mgrProcedure->m_bDispMsgDoAuto[9] = TRUE;
				pView->m_mgrProcedure->m_nStepDispMsg[9] = FROM_DOMARK1;
			}
		}
		break;
	case ERR_PROC + 10:
		pView->m_mgrProcedure->m_bReMark[1] = TRUE;
		pView->m_mgrProcedure->m_bTHREAD_MK[1] = FALSE;
		pView->m_mgrProcedure->m_nStepMk[1] = 0;
		break;
	case ERR_PROC + 20: // MK Done....
		pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
		pView->m_mgrProcedure->m_bTHREAD_MK[1] = FALSE;
		break;
	}
}

BOOL CGvisR2R_PunchView::SaveMk1Img(int nMkPcsIdx) // Cam1
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
		sPath.Format(_T("%s\\%s.tif"), sDest, GetMkInfo(nSerial, nMkPcsIdx));

#ifdef USE_VISION
		if (m_pVision[1])
			return m_pVision[1]->SaveMkImg(sPath);
#endif
	}
	else
		return TRUE;

	return FALSE;
}

//void CGvisR2R_PunchView::DispDefImg()
//{
//	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
//
//	int nSerialL = pView->m_mgrProcedure->m_nBufUpSerial[0]; // 좌측 Camera
//	int nSerialR = pView->m_mgrProcedure->m_nBufUpSerial[1]; // 우측 Camera
//	CString sNewLot = pView->m_mgrProcedure->m_sNewLotUp;;
//	CString str;
//	BOOL bNewModel = FALSE;
//	int nNewLot = 0;
//	int nBreak = 0;
//	int nLastShot = 0;
//
//	switch (pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF)
//	{
//	// CopyDefImg Start ============================================
//	case 0:
//		pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF++;
//
//		if (nSerialL == pView->m_mgrProcedure->m_nLotEndSerial)
//			nBreak = 1;
//
//		if (nSerialL > 0)
//		{
//			if (!CopyDefImg(nSerialL, sNewLot)) // 좌측 Camera
//			{
//				sNewLot.Empty();
//				pView->m_mgrProcedure->m_bDispMsgDoAuto[7] = TRUE;
//				pView->m_mgrProcedure->m_nStepDispMsg[7] = FROM_DISPDEFIMG + 7;
//				break;
//			}
//
//			if (pView->m_mgrProcedure->m_bSerialDecrese)
//			{
//				if (pView->m_mgrProcedure->m_bLastProc && nSerialR < pView->m_mgrProcedure->m_nLotEndSerial)
//				{
//					nLastShot = nSerialL;
//				}
//				else
//				{
//					if (nSerialR > 0)
//					{
//						if (!CopyDefImg(nSerialR, sNewLot)) // 우측 Camera
//						{
//							sNewLot.Empty();
//							pView->m_mgrProcedure->m_bDispMsgDoAuto[6] = TRUE;
//							pView->m_mgrProcedure->m_nStepDispMsg[6] = FROM_DISPDEFIMG + 6;
//							break;
//						}
//					}
//				}
//			}
//			else
//			{
//				if (pView->m_mgrProcedure->m_bLastProc && nSerialR > pView->m_mgrProcedure->m_nLotEndSerial)
//				{
//					nLastShot = nSerialL;
//				}
//				else
//				{
//					if (nSerialR > 0)
//					{
//						if (!CopyDefImg(nSerialR, sNewLot)) // 우측 Camera
//						{
//							sNewLot.Empty();
//							pView->m_mgrProcedure->m_bDispMsgDoAuto[6] = TRUE;
//							pView->m_mgrProcedure->m_nStepDispMsg[6] = FROM_DISPDEFIMG + 6;
//							break;
//						}
//					}
//				}
//			}
//
//		}
//		else
//		{
//			if (!pView->m_mgrProcedure->m_bLastProc)
//			{
//				pView->m_mgrProcedure->m_bDispMsgDoAuto[5] = TRUE;
//				pView->m_mgrProcedure->m_nStepDispMsg[5] = FROM_DISPDEFIMG + 5;
//			}
//		}
//		sNewLot.Empty();
//		break;
//	case 1:
//		//Sleep(300);
//		pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF++;
//		if (pView->m_mgrReelmap->m_pReelMap)
//		{
//			str = pView->m_mgrReelmap->m_pReelMap->GetIpPath();
//			pDoc->SetMkMenu01(_T("DispDefImg"), _T("ReelmapPath"), str);
//		}
//		str.Format(_T("%d"), nSerialL);
//		pDoc->SetMkMenu01(_T("DispDefImg"), _T("SerialL"), str);
//		str.Format(_T("%d"), nSerialR);
//		pDoc->SetMkMenu01(_T("DispDefImg"), _T("SerialR"), str);
//		break;
//	case 2:
//		if (IsDoneDispMkInfo())	 // Check 불량이미지 Display End
//		{
//			if (pView->m_mgrProcedure->m_bSerialDecrese)
//			{
//				if (ChkLastProc() && (nSerialL < pView->m_mgrProcedure->m_nLotEndSerial))
//				{
//					nLastShot = nSerialL;
//					SetSerialReelmap(nSerialL, TRUE);	// Reelmap(좌) Display Start
//					SetSerialMkInfo(nSerialL, TRUE);		// 불량이미지(좌) Display Start
//				}
//				else if (ChkLastProc() && nSerialL < 1)
//				{
//					SetSerialReelmap(pView->m_mgrProcedure->m_nLotEndSerial + 1, TRUE);		// Reelmap(좌) Display Start
//					SetSerialMkInfo(pView->m_mgrProcedure->m_nLotEndSerial + 1, TRUE);		// 불량이미지(좌) Display Start
//				}
//				else
//				{
//					SetFixPcs(nSerialL);
//					SetSerialReelmap(nSerialL);	// Reelmap(좌) Display Start
//					SetSerialMkInfo(nSerialL);	// 불량이미지(좌) Display Start
//				}
//			}
//			else
//			{
//				if (ChkLastProc() && (nSerialL > pView->m_mgrProcedure->m_nLotEndSerial))
//				{
//					nLastShot = nSerialL;
//					SetSerialReelmap(nSerialL, TRUE);	// Reelmap(좌) Display Start
//					SetSerialMkInfo(nSerialL, TRUE);		// 불량이미지(좌) Display Start
//				}
//				else if (ChkLastProc() && nSerialL < 1)
//				{
//					SetSerialReelmap(pView->m_mgrProcedure->m_nLotEndSerial + 1, TRUE);		// Reelmap(좌) Display Start
//					SetSerialMkInfo(pView->m_mgrProcedure->m_nLotEndSerial + 1, TRUE);		// 불량이미지(좌) Display Start
//				}
//				else
//				{
//					SetFixPcs(nSerialL);
//					SetSerialReelmap(nSerialL);	// Reelmap(좌) Display Start
//					SetSerialMkInfo(nSerialL);	// 불량이미지(좌) Display Start
//				}
//			}
//
//
//			if (pView->m_mgrReelmap->IsFixPcsUp(nSerialL))
//			{
//				pView->m_mgrProcedure->m_bDispMsgDoAuto[2] = TRUE;
//				pView->m_mgrProcedure->m_nStepDispMsg[2] = FROM_DISPDEFIMG + 2;
//			}
//			if (pView->m_mgrReelmap->IsFixPcsDn(nSerialL))
//			{
//				pView->m_mgrProcedure->m_bDispMsgDoAuto[3] = TRUE;
//				pView->m_mgrProcedure->m_nStepDispMsg[3] = FROM_DISPDEFIMG + 3;
//			}
//
//			pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF++;
//		}
//
//		break;
//
//	case 3:
//		if (IsDoneDispMkInfo())	 // Check 불량이미지(좌) Display End
//		{
//			if (nSerialR > 0) // 우측 Camera
//			{
//				if (pView->m_mgrProcedure->m_bSerialDecrese)
//				{
//					if (ChkLastProc() && (nSerialR < pView->m_mgrProcedure->m_nLotEndSerial))
//						SetSerialMkInfo(nSerialR, TRUE);	// 불량이미지(우) Display Start
//					else
//						SetSerialMkInfo(nSerialR);		// 불량이미지(우) Display Start
//				}
//				else
//				{
//					if (ChkLastProc() && (nSerialR > pView->m_mgrProcedure->m_nLotEndSerial))
//						SetSerialMkInfo(nSerialR, TRUE);	// 불량이미지(우) Display Start
//					else
//						SetSerialMkInfo(nSerialR);		// 불량이미지(우) Display Start
//				}
//			}
//			else
//			{
//				if (ChkLastProc())
//				{
//					if (pView->m_mgrProcedure->m_bSerialDecrese)
//						SetSerialMkInfo(pView->m_mgrProcedure->m_nLotEndSerial - 1, TRUE);	// 불량이미지(우) Display Start
//					else
//						SetSerialMkInfo(pView->m_mgrProcedure->m_nLotEndSerial + 1, TRUE);	// 불량이미지(우) Display Start
//				}
//			}
//			pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF++;
//		}
//		break;
//	case 4:
//		if (nSerialR > 0) // 우측 Camera
//		{
//			pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF++;
//
//			if (pView->m_mgrProcedure->m_bSerialDecrese)
//			{
//				if (ChkLastProc() && (nSerialR < pView->m_mgrProcedure->m_nLotEndSerial))
//				{
//					SetSerialReelmap(nSerialR, TRUE);	// Reelmap(우) Display Start
//				}
//				else
//				{
//					SetFixPcs(nSerialR);
//					SetSerialReelmap(nSerialR);			// Reelmap(우) Display Start
//				}
//			}
//			else
//			{
//				if (ChkLastProc() && (nSerialR > pView->m_mgrProcedure->m_nLotEndSerial))
//				{
//					SetSerialReelmap(nSerialR, TRUE);	// Reelmap(우) Display Start
//				}
//				else
//				{
//					SetFixPcs(nSerialR);
//					SetSerialReelmap(nSerialR);			// Reelmap(우) Display Start
//				}
//			}
//
//			if (bDualTest)
//			{
//				if (pView->m_mgrReelmap->IsFixPcsUp(nSerialR))
//				{
//					pView->m_mgrProcedure->m_bDispMsgDoAuto[2] = TRUE;
//					pView->m_mgrProcedure->m_nStepDispMsg[2] = FROM_DISPDEFIMG + 2;
//				}
//				if (pView->m_mgrReelmap->IsFixPcsDn(nSerialR))
//				{
//					pView->m_mgrProcedure->m_bDispMsgDoAuto[3] = TRUE;
//					pView->m_mgrProcedure->m_nStepDispMsg[3] = FROM_DISPDEFIMG + 3;
//				}
//			}
//			else
//			{
//				if (pView->m_mgrReelmap->IsFixPcsUp(nSerialR))
//				{
//					pView->m_mgrProcedure->m_bDispMsgDoAuto[2] = TRUE;
//					pView->m_mgrProcedure->m_nStepDispMsg[2] = FROM_DISPDEFIMG + 2;
//				}
//			}
//		}
//		else
//		{
//			if (ChkLastProc())
//			{
//				pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF++;
//				if (pView->m_mgrProcedure->m_bSerialDecrese)
//					SetSerialReelmap(pView->m_mgrProcedure->m_nLotEndSerial - 1, TRUE);	// 불량이미지(우) Display Start
//				else
//					SetSerialReelmap(pView->m_mgrProcedure->m_nLotEndSerial + 1, TRUE);	// 불량이미지(우) Display Start
//			}
//			else
//			{
//				if (pView->m_mgrProcedure->m_bLastProc && nSerialL == pView->m_mgrProcedure->m_nLotEndSerial)
//					pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF++;
//				else
//				{
//					pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF++;
//				}
//			}
//		}
//		break;
//	case 5:
//		pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF++;
//		break;
//	case 6:
//		pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF++;
//		break;
//	case 7:
//		pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF++;
//		break;
//	case 8:
//		pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF++;
//		break;
//	case 9:
//		pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF++;
//		break;
//	case 10:
//		pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF++;
//		break;
//
//	case 11:
//		if (IsDoneDispMkInfo() && pView->m_mgrProcedure->IsRun())	 // Check 불량이미지(우) Display End
//			pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF++;
//		break;
//	case 12:
//		pView->m_mgrProcedure->m_bTHREAD_DISP_DEF = FALSE;
//		pDoc->SetMkMenu01(_T("Signal"), _T("DispDefImg"), _T("0"));
//		if (m_pEngrave)
//			m_pEngrave->SwMenu01DispDefImg(TRUE);
//
//		break;
//		// CopyDefImg End ============================================
//	}
//
//}

void CGvisR2R_PunchView::DoAuto()
{
	if (!IsAuto())
		return;

	CString str;
	str.Format(_T("%d : %d"), pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF, pView->m_mgrProcedure->m_bTHREAD_DISP_DEF ? 1 : 0);
	pView->DispStsBar(str, 6);

	//BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	// LotEnd Start
	if (DoAutoGetLotEndSignal())
	{
		return;
	}

	// 마킹시작 신호를 확인
	DoAtuoGetMkStSignal();

	// LastProc Start
	DoAutoSetLastProcAtPlc();

	// AOI Feeding Offset Start on LastProc
	DoAutoSetFdOffsetLastProc();

	// AOI Feeding Offset Start
	DoAutoSetFdOffset();

	// Engrave Feeding Offset Start
	DoAutoSetFdOffsetEngrave();

	// CycleStop
	DoAutoChkCycleStop();

	// DispMsg
	DoAutoDispMsg();

	// Check Share Folder Start
	DoAutoChkShareFolder();

	// Marking Start
	DoAutoMarking();

	// Engrave Marking Start
	DoAutoMarkingEngrave();
}

BOOL CGvisR2R_PunchView::DoAutoGetLotEndSignal()
{
	int nSerial;

	if (m_pDlgMenu01)
	{
		if (m_pDlgMenu01->m_bLotEnd && pView->m_mgrProcedure->m_nStepAuto < LOT_END)
		{
			pView->m_mgrProcedure->m_bLotEnd = TRUE;
			pView->m_mgrProcedure->m_nLotEndAuto = LOT_END;
		}
	}

	if (!IsBuffer(0) && pView->m_mgrProcedure->m_bLastProc && pView->m_mgrProcedure->m_nLotEndAuto < LOT_END)
	{
		pView->m_mgrProcedure->m_bLotEnd = TRUE;
		pView->m_mgrProcedure->m_nLotEndAuto = LOT_END;
	}
	else if(!IsBuffer(0) && pView->m_mgrProcedure->m_nMkStAuto > MK_ST + (Mk2PtIdx::DoneMk) + 4)
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
			m_pMpe->Write(_T("MB440180"), 1);			// 작업종료(PC가 On시키고, PLC가 확인하고 Off시킴)-20141031
			DispMain(_T("작업종료"), RGB_RED);
			pView->m_mgrProcedure->m_nLotEndAuto++;
			break;
		case LOT_END + 2:
			Stop();
			TowerLamp(RGB_YELLOW, TRUE);
			Buzzer(TRUE, 0);
			LotEnd();									// MakeResultMDS
			pView->m_mgrProcedure->m_nLotEndAuto++;
			break;

		case LOT_END + 3:
			MsgBox(_T("작업이 종료되었습니다."));
			//m_nStepAuto = 0; // 자동종료
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

void CGvisR2R_PunchView::DoAtuoGetMkStSignal()
{
#ifdef USE_MPE
	if (m_pMpe)
	{
		if (!pView->m_mgrProcedure->m_bMkSt)
		{
			if (pView->m_mgrProcedure->IsRun())
			{
				if (pDoc->m_pMpeSignal[1] & (0x01 << 0) || pView->m_mgrProcedure->m_bMkStSw) // AlignTest		// 마킹시작(PC가 확인하고 Reset시킴.)-20141029
				{
					pView->m_mgrProcedure->m_bMkStSw = FALSE;

					m_pMpe->Write(_T("MB440110"), 0);			// 마킹시작(PC가 확인하고 Reset시킴.)-20141029

					if (pDoc->m_pMpeSignal[0] & (0x01 << 1))	// 마킹부 Feeding완료(PLC가 On시키고 PC가 확인하고 Reset시킴.)-20141030
						m_pMpe->Write(_T("MB440101"), 0);		// 마킹부 Feeding완료

					pView->m_mgrProcedure->m_bMkSt = TRUE;
					pView->m_mgrProcedure->m_nMkStAuto = MK_ST;
					::WritePrivateProfileString(_T("Last Job"), _T("MkSt"), _T("1"), PATH_WORKING_INFO);

					if (!pDoc->GetLastShotMk())
						SetLotSt();		// 새로운 로트의 시작시간을 설정함. // 전체속도의 처음 시작되는 시간 설정.
				}
			}
		}
	}
#endif
}

void CGvisR2R_PunchView::DoAutoSetLastProcAtPlc()
{
	if (pView->m_mgrProcedure->m_bLastProc)
	{
		switch (pView->m_mgrProcedure->m_nLastProcAuto)
		{
		case LAST_PROC:	// 잔량처리 1
			if (pView->m_mgrProcedure->IsRun())
			{
				if (MODE_INNER != pDoc->GetTestMode())
				{
					if (ChkLastProcFromUp())
					{
						m_pMpe->Write(_T("MB440185"), 1);			// 잔량처리 AOI(상) 부터(PC가 On시키고, PLC가 확인하고 Off시킴)-20141112
						m_pMpe->Write(_T("MB44012B"), 1);			// AOI 상 : PCR파일 Received
					}
					else
						m_pMpe->Write(_T("MB440186"), 1);			// 잔량처리 AOI(하) 부터(PC가 On시키고, PLC가 확인하고 Off시킴)-20141112
				}
				else
				{
					if (ChkLastProcFromEng())
						m_pMpe->Write(_T("MB44019D"), 1);			// 잔량처리 각인부 부터(PC가 On시키고, PLC가 확인하고 Off시킴)-20141112
					else if (ChkLastProcFromUp())
					{
						m_pMpe->Write(_T("MB440185"), 1);			// 잔량처리 AOI(상) 부터(PC가 On시키고, PLC가 확인하고 Off시킴)-20141112
						m_pMpe->Write(_T("MB44012B"), 1);			// AOI 상 : PCR파일 Received
					}
					else
						m_pMpe->Write(_T("MB440186"), 1);			// 잔량처리 AOI(하) 부터(PC가 On시키고, PLC가 확인하고 Off시킴)-20141112
				}

				pView->m_mgrProcedure->m_nLastProcAuto++;
			}
			break;
		case LAST_PROC + 1:
			m_pMpe->Write(_T("MB440181"), 1);			// 잔량처리(PC가 On시키고, PLC가 확인하고 Off시킴)-20141031
			pView->m_mgrProcedure->m_nLastProcAuto++;
			break;
		case LAST_PROC + 2:
			;
			break;
		}
	}
}

void CGvisR2R_PunchView::DoAutoSetFdOffsetLastProc()
{
#ifdef USE_MPE
	BOOL bOn0 = (pDoc->m_pMpeIb[10] & (0x01 << 11)) ? TRUE : FALSE;		// 검사부 상 자동 운전 <-> X432B I/F
	BOOL bOn1 = (pDoc->m_pMpeIb[14] & (0x01 << 11)) ? TRUE : FALSE;		// 검사부 하 자동 운전 <-> X442B I/F

	if (bOn0 && !(pView->m_mgrProcedure->m_Flag & (0x01 << 2)))
	{
		pView->m_mgrProcedure->m_Flag |= (0x01 << 2);
	}
	else if (!bOn0 && (pView->m_mgrProcedure->m_Flag & (0x01 << 2)))
	{
		pView->m_mgrProcedure->m_Flag &= ~(0x01 << 2);

		pView->m_mgrProcedure->m_bAoiTest[0] = FALSE;
		pView->m_mgrProcedure->m_bWaitPcr[0] = FALSE;
		pView->m_mgrProcedure->m_bAoiTestF[0] = FALSE;
		pView->m_mgrProcedure->m_bAoiFdWriteF[0] = FALSE;
		pView->m_pMpe->Write(_T("MB440111"), 0); // 검사부(상) Feeding Offset Write 완료(PC가 확인하고 Reset시킴.)-20141103
	}

	if (bOn1 && !(pView->m_mgrProcedure->m_Flag & (0x01 << 3)))
	{
		pView->m_mgrProcedure->m_Flag |= (0x01 << 3);
	}
	else if (!bOn1 && (pView->m_mgrProcedure->m_Flag & (0x01 << 3)))
	{
		pView->m_mgrProcedure->m_Flag &= ~(0x01 << 3);

		pView->m_mgrProcedure->m_bAoiTest[1] = FALSE;
		pView->m_mgrProcedure->m_bWaitPcr[1] = FALSE;
		pView->m_mgrProcedure->m_bAoiTestF[1] = FALSE;
		pView->m_mgrProcedure->m_bAoiFdWriteF[1] = FALSE;
		pView->m_pMpe->Write(_T("MB440112"), 0); // 검사부(하) Feeding Offset Write 완료(PC가 확인하고 Reset시킴.)-20141103
	}
#endif
}

void CGvisR2R_PunchView::DoAutoSetFdOffset()
{
#ifdef USE_MPE
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	double dAveX, dAveY;
	CfPoint OfStUp, OfStDn;

	if (pDoc->m_pMpeSignal[1] & (0x01 << 3) && !pView->m_mgrProcedure->m_bAoiTestF[0])		// 검사부(상) 검사중-20141103
	{
		pView->m_mgrProcedure->m_bAoiTestF[0] = TRUE;
		pView->m_mgrProcedure->m_bAoiTest[0] = TRUE;
		pView->m_mgrProcedure->m_bWaitPcr[0] = TRUE;
	}
	else if (!(pDoc->m_pMpeSignal[1] & (0x01 << 3)) && pView->m_mgrProcedure->m_bAoiTestF[0])
	{
		pView->m_mgrProcedure->m_bAoiTestF[0] = FALSE;
		pView->m_mgrProcedure->m_bAoiTest[0] = FALSE;//
		pView->m_mgrProcedure->m_bAoiFdWriteF[0] = FALSE;

		pView->m_pMpe->Write(_T("MB440111"), 0); // 검사부(상) Feeding Offset Write 완료(PC가 확인하고 Reset시킴.)-20141103
	}

	if (pDoc->m_pMpeSignal[1] & (0x01 << 4) && !pView->m_mgrProcedure->m_bAoiTestF[1])		// 검사부(하) 검사중-20141103
	{
		pView->m_mgrProcedure->m_bAoiTestF[1] = TRUE;
		pView->m_mgrProcedure->m_bAoiTest[1] = TRUE;
		pView->m_mgrProcedure->m_bWaitPcr[1] = TRUE;
	}
	else if (!(pDoc->m_pMpeSignal[1] & (0x01 << 4)) && pView->m_mgrProcedure->m_bAoiTestF[1])
	{
		pView->m_mgrProcedure->m_bAoiTestF[1] = FALSE;
		pView->m_mgrProcedure->m_bAoiTest[1] = FALSE;//
		pView->m_mgrProcedure->m_bAoiFdWriteF[1] = FALSE;
		pView->m_pMpe->Write(_T("MB440112"), 0); // 검사부(하) Feeding Offset Write 완료(PC가 확인하고 Reset시킴.)-20141103
	}


	if (pDoc->m_pMpeSignal[1] & (0x01 << 1) && !pView->m_mgrProcedure->m_bAoiFdWrite[0])		// 검사부(상) Feeding Offset Write 완료-20141103
		pView->m_mgrProcedure->m_bAoiFdWrite[0] = TRUE;
	else if (!(pDoc->m_pMpeSignal[1] & (0x01 << 1)) && pView->m_mgrProcedure->m_bAoiFdWrite[0])
		pView->m_mgrProcedure->m_bAoiFdWrite[0] = FALSE;

	if (pDoc->m_pMpeSignal[1] & (0x01 << 2) && !pView->m_mgrProcedure->m_bAoiFdWrite[1])		// 검사부(하) Feeding Offset Write 완료-20141103
		pView->m_mgrProcedure->m_bAoiFdWrite[1] = TRUE;
	else if (!(pDoc->m_pMpeSignal[1] & (0x01 << 2)) && pView->m_mgrProcedure->m_bAoiFdWrite[1])
		pView->m_mgrProcedure->m_bAoiFdWrite[1] = FALSE;


	if (bDualTest) // 20160721-syd-temp
	{
		if ((pView->m_mgrProcedure->m_bAoiFdWrite[0] && pView->m_mgrProcedure->m_bAoiFdWrite[1]) 
			&& (!pView->m_mgrProcedure->m_bAoiFdWriteF[0] && !pView->m_mgrProcedure->m_bAoiFdWriteF[1]))
		{
			pView->m_mgrProcedure->m_bAoiFdWriteF[0] = TRUE;
			pView->m_mgrProcedure->m_bAoiFdWriteF[1] = TRUE;

			GetAoiUpOffset(OfStUp);
			GetAoiDnOffset(OfStDn);

			dAveX = OfStUp.x;
			dAveY = OfStUp.y; // syd - 20230327
			//dAveX = (OfStUp.x + OfStDn.x) / 2.0;
			//dAveY = (OfStUp.y + OfStDn.y) / 2.0;

			if (m_pDlgMenu02)
			{
				m_pDlgMenu02->m_dAoiUpFdOffsetX = OfStUp.x;
				m_pDlgMenu02->m_dAoiUpFdOffsetY = OfStUp.y;
				m_pDlgMenu02->m_dAoiDnFdOffsetX = OfStDn.x;
				m_pDlgMenu02->m_dAoiDnFdOffsetY = OfStDn.y;
			}

			pView->m_pMpe->Write(_T("ML45064"), (long)(-1.0*dAveX*1000.0));
			pView->m_pMpe->Write(_T("MB440111"), 0); // 검사부(상) Feeding Offset Write 완료(PC가 확인하고 Reset시킴.)-20141103
			pView->m_pMpe->Write(_T("MB440112"), 0); // 검사부(하) Feeding Offset Write 완료(PC가 확인하고 Reset시킴.)-20141103
		}
		else if ((!pView->m_mgrProcedure->m_bAoiFdWrite[0] && !pView->m_mgrProcedure->m_bAoiFdWrite[1]) 
			&& (pView->m_mgrProcedure->m_bAoiFdWriteF[0] && pView->m_mgrProcedure->m_bAoiFdWriteF[1]))
		{
			pView->m_mgrProcedure->m_bAoiFdWriteF[0] = FALSE;
			pView->m_mgrProcedure->m_bAoiFdWriteF[1] = FALSE;
			pView->m_mgrProcedure->m_bAoiTest[0] = FALSE;
			pView->m_mgrProcedure->m_bAoiTest[1] = FALSE;
		}

		if (pView->m_mgrProcedure->m_bAoiTest[0] && !pView->m_mgrProcedure->m_bAoiTest[1])
		{
			if (pView->m_mgrProcedure->m_bAoiFdWrite[0] && !pView->m_mgrProcedure->m_bAoiFdWriteF[0])
			{
				pView->m_mgrProcedure->m_bAoiFdWriteF[0] = TRUE;

				GetAoiUpOffset(OfStUp);

				if (m_pDlgMenu02)
				{
					m_pDlgMenu02->m_dAoiUpFdOffsetX = OfStUp.x;
					m_pDlgMenu02->m_dAoiUpFdOffsetY = OfStUp.y;
					m_pDlgMenu02->m_dAoiDnFdOffsetX = 0.0;
					m_pDlgMenu02->m_dAoiDnFdOffsetY = 0.0;
				}

				pView->m_pMpe->Write(_T("ML45064"), (long)(-1.0*OfStUp.x*1000.0));
				pView->m_pMpe->Write(_T("MB440111"), 0); // 검사부(상) Feeding Offset Write 완료(PC가 확인하고 Reset시킴.)-20141103
			}
			else if (!pView->m_mgrProcedure->m_bAoiFdWrite[0] && pView->m_mgrProcedure->m_bAoiFdWriteF[0])
			{
				pView->m_mgrProcedure->m_bAoiFdWriteF[0] = FALSE;
				pView->m_mgrProcedure->m_bAoiTest[0] = FALSE;
			}
		}

		if (!pView->m_mgrProcedure->m_bAoiTest[0] && pView->m_mgrProcedure->m_bAoiTest[1])
		{
			if (pView->m_mgrProcedure->m_bAoiFdWrite[1] && !pView->m_mgrProcedure->m_bAoiFdWriteF[1])
			{
				pView->m_mgrProcedure->m_bAoiFdWriteF[1] = TRUE;

				GetAoiDnOffset(OfStDn);

				if (m_pDlgMenu02)
				{
					m_pDlgMenu02->m_dAoiUpFdOffsetX = 0.0;
					m_pDlgMenu02->m_dAoiUpFdOffsetY = 0.0;
					m_pDlgMenu02->m_dAoiDnFdOffsetX = OfStDn.x;
					m_pDlgMenu02->m_dAoiDnFdOffsetY = OfStDn.y;
				}

				pView->m_pMpe->Write(_T("ML45064"), (long)(-1.0*OfStDn.x*1000.0));
				pView->m_pMpe->Write(_T("MB440112"), 0); // 검사부(하) Feeding Offset Write 완료(PC가 확인하고 Reset시킴.)-20141103
			}
			else if (!pView->m_mgrProcedure->m_bAoiFdWrite[1] && pView->m_mgrProcedure->m_bAoiFdWriteF[1])
			{
				pView->m_mgrProcedure->m_bAoiFdWriteF[1] = FALSE;
				pView->m_mgrProcedure->m_bAoiTest[1] = FALSE;
			}
		}
	}
	else // 20160721-syd-temp
	{
		if (pView->m_mgrProcedure->m_bAoiFdWrite[0] && !pView->m_mgrProcedure->m_bAoiFdWriteF[0])
		{
			pView->m_mgrProcedure->m_bAoiFdWriteF[0] = TRUE;

			GetAoiUpOffset(OfStUp);

			dAveX = (OfStUp.x);
			dAveY = (OfStUp.y);

			if (m_pDlgMenu02)
			{
				m_pDlgMenu02->m_dAoiUpFdOffsetX = dAveX;
				m_pDlgMenu02->m_dAoiUpFdOffsetY = dAveY;
			}

			pView->m_pMpe->Write(_T("ML45064"), (long)(-1.0*dAveX*1000.0));	// 검사부 Feeding 롤러 Offset(*1000, +:더 보냄, -덜 보냄)
			pView->m_pMpe->Write(_T("MB440111"), 0); // 검사부(상) Feeding Offset Write 완료(PC가 확인하고 Reset시킴.)-20141103
			pView->m_pMpe->Write(_T("MB440112"), 0); // 검사부(하) Feeding Offset Write 완료(PC가 확인하고 Reset시킴.)-20141103  // 20160721-syd-temp
		}
		else if (!pView->m_mgrProcedure->m_bAoiFdWrite[0] && pView->m_mgrProcedure->m_bAoiFdWriteF[0])
		{
			pView->m_mgrProcedure->m_bAoiFdWriteF[0] = FALSE;
			pView->m_mgrProcedure->m_bAoiTest[0] = FALSE;
		}
	}
#endif
}

void CGvisR2R_PunchView::DoAutoChkCycleStop()
{
	if (pView->m_mgrProcedure->m_bCycleStop)
	{
		pView->m_mgrProcedure->m_bCycleStop = FALSE;
		TowerLamp(RGB_YELLOW, TRUE);
		Buzzer(TRUE);
		//MyMsgBox(pDoc->m_sAlmMsg);
		if (!pDoc->m_sAlmMsg.IsEmpty())
		{
			MsgBox(pDoc->m_sAlmMsg, 0, 0, DEFAULT_TIME_OUT, FALSE);

			if (pDoc->m_sAlmMsg == GetAoiUpAlarmRestartMsg())
			{
				SetAoiUpAutoStep(2); // Wait for AOI 검사시작 신호.
				Sleep(300);
				if (m_pMpe)
					m_pMpe->Write(_T("MB44013B"), 1); // 검사부 상부 재작업 (시작신호) : PC가 On시키고 PLC가 Off
			}
			else if (pDoc->m_sAlmMsg == GetAoiDnAlarmRestartMsg())
			{
				SetAoiDnAutoStep(2); // Wait for AOI 검사시작 신호.
				Sleep(300);
				if (m_pMpe)
					m_pMpe->Write(_T("MB44013C"), 1); // 검사부 하부 재작업 (시작신호) : PC가 On시키고 PLC가 Off
			}
		}
		pDoc->m_sAlmMsg = _T("");
		pDoc->m_sIsAlmMsg = _T("");
		pDoc->m_sPrevAlmMsg = _T("");
	}
}

void CGvisR2R_PunchView::DoAutoDispMsg()
{
	BOOL bDispMsg = FALSE;
	int idx, nStepDispMsg;

	// [2] : 고정불량-상, [3] : 고정불량-하, [5] : nSerialL <= 0, [6] : CopyDefImg 우측 Camera,  
	// [7] : CopyDefImg 좌측 Camera, [8] : 보이스코일(좌) 초기위치 이동, [9] : 보이스코일(우) 초기위치 이동
	for (idx = 0; idx < 10; idx++)
	{
		if (pView->m_mgrProcedure->m_bDispMsgDoAuto[idx])
		{
			bDispMsg = TRUE;
			nStepDispMsg = pView->m_mgrProcedure->m_nStepDispMsg[idx];
			break;
		}
	}
	if (bDispMsg && pView->m_mgrProcedure->IsRun())
	{
		switch (nStepDispMsg)
		{
		case 0:
			break;
		case FROM_DOMARK0:
			pView->m_mgrProcedure->m_bDispMsgDoAuto[8] = FALSE;
			pView->m_mgrProcedure->m_nStepDispMsg[8] = 0;
			Stop();
			TowerLamp(RGB_YELLOW, TRUE);
			Buzzer(TRUE, 0);
			//pView->DispStsBar(_T("정지-37"), 0);
			DispMain(_T("정 지"), RGB_RED);
			break;
		case FROM_DOMARK1:
			pView->m_mgrProcedure->m_bDispMsgDoAuto[9] = FALSE;
			pView->m_mgrProcedure->m_nStepDispMsg[9] = 0;
			Stop();
			TowerLamp(RGB_YELLOW, TRUE);
			Buzzer(TRUE, 0);
			//pView->DispStsBar(_T("정지-38"), 0);
			DispMain(_T("정 지"), RGB_RED);
			break;
		case FROM_DISPDEFIMG:
			pView->m_mgrProcedure->m_bDispMsgDoAuto[0] = FALSE;
			pView->m_mgrProcedure->m_nStepDispMsg[0] = 0;
			Stop();
			MsgBox(_T("버퍼(우) Serial이 맞지않습니다."));
			TowerLamp(RGB_YELLOW, TRUE);
			break;
		case FROM_DISPDEFIMG + 1:
			pView->m_mgrProcedure->m_bDispMsgDoAuto[1] = FALSE;
			pView->m_mgrProcedure->m_nStepDispMsg[1] = 0;
			Stop();
			MsgBox(_T("버퍼(우) Serial이 맞지않습니다."));
			TowerLamp(RGB_YELLOW, TRUE);
			break;
		case FROM_DISPDEFIMG + 2: // IsFixUp
			pView->m_mgrProcedure->m_bDispMsgDoAuto[2] = FALSE;
			pView->m_mgrProcedure->m_nStepDispMsg[2] = 0;
			Stop();
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
			pView->m_mgrProcedure->m_bSwRunF = FALSE;
			//pView->DispStsBar(_T("정지-39"), 0);
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(pView->m_mgrProcedure->m_sFixMsg[0]);
			pView->m_mgrProcedure->m_sFixMsg[0] = _T("");
			break;
		case FROM_DISPDEFIMG + 3: // IsFixDn
			pView->m_mgrProcedure->m_bDispMsgDoAuto[3] = FALSE;
			pView->m_mgrProcedure->m_nStepDispMsg[3] = 0;
			Stop();
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
			pView->m_mgrProcedure->m_bSwRunF = FALSE;
			//pView->DispStsBar(_T("정지-40"), 0);
			DispMain(_T("정 지"), RGB_RED);
			MsgBox(pView->m_mgrProcedure->m_sFixMsg[1]);
			pView->m_mgrProcedure->m_sFixMsg[1] = _T("");
			break;
		case FROM_DISPDEFIMG + 4:
			//m_bDispMsgDoAuto[4] = FALSE;
			//m_nStepDispMsg[4] = 0;
			//Stop();
			//TowerLamp(RGB_RED, TRUE);
			//Buzzer(TRUE, 0);
			//m_bSwStopNow = TRUE;
			//m_bSwRunF = FALSE;
			//pView->DispStsBar(_T("정지-41"), 0);
			//DispMain(_T("정 지"), RGB_RED);	
			//MsgBox(m_sFixMsg);
			//m_sFixMsg = _T("");
			break;
		case FROM_DISPDEFIMG + 5:
			pView->m_mgrProcedure->m_bDispMsgDoAuto[5] = FALSE;
			pView->m_mgrProcedure->m_nStepDispMsg[5] = 0;
			Stop();
			MsgBox(_T("버퍼(좌) Serial이 맞지않습니다."));
			TowerLamp(RGB_YELLOW, TRUE);
			break;
		case FROM_DISPDEFIMG + 6:
			pView->m_mgrProcedure->m_bDispMsgDoAuto[6] = FALSE;
			pView->m_mgrProcedure->m_nStepDispMsg[6] = 0;
			Stop();
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
			pView->m_mgrProcedure->m_bSwRunF = FALSE;
			//pView->DispStsBar(_T("정지-42"), 0);
			DispMain(_T("정 지"), RGB_RED);
			break;
		case FROM_DISPDEFIMG + 7:
			pView->m_mgrProcedure->m_bDispMsgDoAuto[7] = FALSE;
			pView->m_mgrProcedure->m_nStepDispMsg[7] = 0;
			Stop();
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
			pView->m_mgrProcedure->m_bSwStopNow = TRUE;
			pView->m_mgrProcedure->m_bSwRunF = FALSE;
			//pView->DispStsBar(_T("정지-43"), 0);
			DispMain(_T("정 지"), RGB_RED);
			break;
		}
	}
}

void CGvisR2R_PunchView::DoAutoChkShareFolder()	// 20170727-잔량처리 시 계속적으로 반복해서 이함수가 호출됨으로 좌우 마킹 인덱스 동일 현상 발생.(case AT_LP + 8:)
{
	CString sLot, sLayerUp, sLayerDn;
	BOOL bDualTestInner;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nSerial = 0;
	CString sNewLot;
	int nNewLot = 0;
	BOOL bPcrInShare[2];
	BOOL bNewModel = FALSE;

	switch (pView->m_mgrProcedure->m_nStepAuto)
	{
	case 0:
		pView->m_mgrProcedure->m_bSwRun = FALSE;
		pView->m_mgrProcedure->m_bSwStop = TRUE;
		pView->m_mgrProcedure->m_nStepAuto++;
		break;
	case 1:
		if (IsReady())		// 운전준비
		{
			TowerLamp(RGB_YELLOW, TRUE, TRUE);
			pView->m_mgrProcedure->m_nStepAuto++;
		}
		break;
	case 2:
		if (pView->m_mgrProcedure->IsRun())
			pView->m_mgrProcedure->m_nStepAuto++;
		break;
	case 3:
		ClrDispMsg();
		TowerLamp(RGB_YELLOW, TRUE, FALSE);
		pView->m_mgrProcedure->m_nStepAuto++;
		break;
	case 4:
		if (pView->m_mgrProcedure->IsRun())		// 초기운전
		{
			if (!IsAoiLdRun())
			{
				Stop();
				TowerLamp(RGB_YELLOW, TRUE);
			}
			else
			{
				ResetWinker(); // 20151126 : 운전스위치 램프 동작 문제로 수정.

				TowerLamp(RGB_GREEN, TRUE, TRUE);
				if (pDoc->WorkingInfo.LastJob.bSampleTest)
				{
					if (pDoc->WorkingInfo.LastJob.bDualTest)
					{
						if (pView->m_mgrProcedure->m_sDispMain != _T("양면샘플"))
							DispMain(_T("양면샘플"), RGB_GREEN);
					}
					else
					{
						if (pView->m_mgrProcedure->m_sDispMain != _T("단면샘플"))
							DispMain(_T("단면샘플"), RGB_GREEN);
					}
				}
				else if (pDoc->WorkingInfo.LastJob.bDualTest)
				{
					if (pView->m_mgrProcedure->m_sDispMain != _T("양면검사"))
						DispMain(_T("양면검사"), RGB_GREEN);
				}
				else
				{
					if (pView->m_mgrProcedure->m_sDispMain != _T("단면검사"))
						DispMain(_T("단면검사"), RGB_GREEN);
 					//if(m_sDispMain != _T("초기운전")
 					//	DispMain(_T("초기운전", RGB_GREEN);
				}
				pView->m_mgrProcedure->m_nVsBufLastSerial[0] = GetVsUpBufLastSerial();
				if (bDualTest)
					pView->m_mgrProcedure->m_nVsBufLastSerial[1] = GetVsDnBufLastSerial();

				SetListBuf();

				if (MODE_INNER == pDoc->GetTestMode() || MODE_OUTER == pDoc->GetTestMode()) // Please modify for outer mode.-20221226
				{
					GetCurrentInfoEng();
					if (m_pDlgMenu01)
						m_pDlgMenu01->UpdateData();
				}

				pView->m_mgrProcedure->m_nStepAuto = AT_LP;
			}
		}
		else
			Winker(MN_RUN); // Run Button - 20151126 : 운전스위치 램프 동작 문제로 수정.
		break;

	case AT_LP:
		if (IsShare())
		{
			bPcrInShare[0] = FALSE;
			bPcrInShare[1] = FALSE;

			//if (IsShareUp() && IsTestDoneUp() && !m_bAoiTestF[0]) // 검사부(상) 검사중
			if (IsShareUp())
			{
				nSerial = GetShareUp();
				if (nSerial > 0)
				{
					if (pView->m_mgrProcedure->m_bSerialDecrese)
					{
						if (pView->m_mgrProcedure->m_nLotEndSerial > 0 && nSerial < pView->m_mgrProcedure->m_nLotEndSerial)
						{
							// Delete PCR File
							pView->m_mgrReelmap->DelSharePcrUp();
						}
						else
						{
							pView->m_mgrProcedure->m_nShareUpS = nSerial;
							bPcrInShare[0] = TRUE;
						}
					}
					else
					{
						if (pView->m_mgrProcedure->m_nLotEndSerial > 0 && nSerial > pView->m_mgrProcedure->m_nLotEndSerial)
						{
							// Delete PCR File
							pView->m_mgrReelmap->DelSharePcrUp();
						}
						else
						{
							pView->m_mgrProcedure->m_nShareUpS = nSerial;
							bPcrInShare[0] = TRUE;
						}
					}
				}
				else
				{
					pView->m_mgrProcedure->m_bLoadShare[0] = FALSE;
				}
			}
			else
				pView->m_mgrProcedure->m_bLoadShare[0] = FALSE;


			if (bDualTest)
			{
				if (IsShareDn())
				{
					nSerial = GetShareDn();
					if (nSerial > 0)
					{
						if (pView->m_mgrProcedure->m_bSerialDecrese)
						{
							if (pView->m_mgrProcedure->m_nLotEndSerial > 0 && nSerial < pView->m_mgrProcedure->m_nLotEndSerial)
							{
								// Delete PCR File
								pView->m_mgrReelmap->DelSharePcrDn();
							}
							else
							{
								pView->m_mgrProcedure->m_nShareDnS = nSerial;
								bPcrInShare[1] = TRUE;
							}
						}
						else
						{
							if (pView->m_mgrProcedure->m_nLotEndSerial > 0 && nSerial > pView->m_mgrProcedure->m_nLotEndSerial)
							{
								// Delete PCR File
								pView->m_mgrReelmap->DelSharePcrDn();
							}
							else
							{
								pView->m_mgrProcedure->m_nShareDnS = nSerial;
								bPcrInShare[1] = TRUE;
							}
						}
					}
					else
					{
						pView->m_mgrProcedure->m_bLoadShare[1] = FALSE;
					}
				}
				else
					pView->m_mgrProcedure->m_bLoadShare[1] = FALSE;

				if (bPcrInShare[0] || bPcrInShare[1])
					pView->m_mgrProcedure->m_nStepAuto++;
			}
			else
			{
				if (bPcrInShare[0])
					pView->m_mgrProcedure->m_nStepAuto++;
			}
		}
		else
		{
			if (!pView->m_mgrProcedure->m_bLastProc)
			{
				if (ChkLastProc())
				{
					pView->m_mgrProcedure->m_nLastProcAuto = LAST_PROC;
					pView->m_mgrProcedure->m_bLastProc = TRUE;
					nSerial = GetShareUp();

					if (IsVs())
					{
						if (pView->m_mgrProcedure->m_nAoiLastSerial[0] < 1)
							pView->m_mgrProcedure->m_nAoiLastSerial[0] = nSerial;

						pView->m_mgrProcedure->m_nPrevStepAuto = pView->m_mgrProcedure->m_nStepAuto;
						pView->m_mgrProcedure->m_nStepAuto = LAST_PROC_VS_ALL;		 // 잔량처리 3
						break;
					}
					else
					{
						if (bDualTest)
						{
							if (ChkLastProcFromEng())
								nSerial = pDoc->GetCurrentInfoEngShotNum();
							else if (ChkLastProcFromUp())
								nSerial = pDoc->m_ListBuf[0].GetLast();
							else
								nSerial = pDoc->m_ListBuf[1].GetLast();
						}
						else
						{
							if (ChkLastProcFromEng())
								nSerial = pDoc->GetCurrentInfoEngShotNum();
							else
								nSerial = pDoc->m_ListBuf[0].GetLast();
						}

						if (!IsSetLotEnd()) // 20160810
						{
							SetLotEnd(nSerial);//+pDoc->AoiDummyShot[1]); // 3
							pView->m_mgrProcedure->m_nAoiLastSerial[0] = nSerial;
						}

						pView->m_mgrProcedure->m_nStepAuto++;
					}
				}
			}
			else
			{
				if (ChkLastProcFromEng())
				{
					nSerial = pDoc->GetCurrentInfoEngShotNum();

					if (!IsSetLotEnd())
					{
						SetLotEnd(nSerial);
						pView->m_mgrProcedure->m_nAoiLastSerial[0] = nSerial;
					}
				}

				pView->m_mgrProcedure->m_bWaitPcr[0] = FALSE;
				pView->m_mgrProcedure->m_bWaitPcr[1] = FALSE;
				pView->m_mgrProcedure->m_nStepAuto++;
			}
			
			if (MODE_INNER == pDoc->GetTestMode())
			{
				nSerial = pDoc->GetCurrentInfoEngShotNum();

				if (ChkLastProc())
				{
					if (ChkLastProcFromEng())
					{
						if (!IsSetLotEnd())
							SetLotEnd(nSerial);
					}
				}
			}

		}
		break;

	case AT_LP + 1:
		if (!pView->m_mgrProcedure->m_bCont) // 이어가기 아닌 경우.
		{
			if (!ChkStShotNum())
			{
				Stop();
				TowerLamp(RGB_YELLOW, TRUE);
			}
		}
		else
		{
			//if (!ChkContShotNum())
			//{
			//	Stop();
			//	TowerLamp(RGB_YELLOW, TRUE);
			//}
		}
		pView->m_mgrProcedure->m_nStepAuto++;
		break;
	case AT_LP + 2:
		if (pView->m_mgrProcedure->IsRun())
		{
			pView->m_mgrProcedure->m_bBufEmpty[0] = pView->m_mgrProcedure->m_bBufEmpty[0]; // Up
			pView->m_mgrProcedure->m_nStepAuto++;
		}
		break;

	case AT_LP + 3:
		Shift2Buf();			// PCR 이동(Share->Buffer)
		pView->m_mgrProcedure->m_nStepAuto++;
		break;

	case AT_LP + 4:
		if (!pView->m_mgrProcedure->IsRun())
			break;

		if (!bDualTest)
		{
			if (pDoc->GetTestMode() == MODE_OUTER)
			{
				if (pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_UP || pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_INNER_UP) // Write Reelmap
					break;

				if (pDoc->WorkingInfo.LastJob.bDualTestInner)
				{
					if (pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_INNER_DN || pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_INNER_ALLUP 
						|| pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_INNER_ALLDN) // Write Reelmap
						break;
				}
			}
			else
			{
				if (pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_UP) // Write Reelmap
					break;
			}
		}

		pView->m_mgrProcedure->m_nStepAuto++;

		if (pView->m_mgrProcedure->m_nShareUpS > 0)
		{
			//if (m_nShareUpS % 2)
			//	m_nShareUpSerial[0] = m_nShareUpS; // 홀수
			//else
			//	m_nShareUpSerial[1] = m_nShareUpS; // 짝수
			pView->m_mgrProcedure->m_nShareUpCnt++;

			if (pDoc->GetCurrentInfoEng())
				pView->m_mgrReelmap->GetItsSerialInfo(pView->m_mgrProcedure->m_nShareUpS, bDualTestInner, sLot, sLayerUp, sLayerDn, 0);

			bNewModel = GetAoiUpInfo(pView->m_mgrProcedure->m_nShareUpS, &nNewLot); // Buffer에서 PCR파일의 헤드 정보를 얻음.

			if (bNewModel)	// AOI 정보(AoiCurrentInfoPath) -> AOI Feeding Offset
			{
				pView->m_mgrProcedure->m_bNewModel = TRUE;
				InitInfo();
				pView->m_mgrReelmap->InitMstData();
	
				if (m_pEngrave)
					m_pEngrave->SwMenu01UpdateWorking(TRUE);

				SetAlignPos();
				ModelChange(0); // 0 : AOI-Up , 1 : AOI-Dn

				if (m_pDlgMenu01)
				{
					m_pDlgMenu01->InitGL();
					pView->m_mgrProcedure->m_bDrawGL = TRUE;
					m_pDlgMenu01->RefreshRmap();
					m_pDlgMenu01->InitCadImg();
					m_pDlgMenu01->SetPnlNum();
					m_pDlgMenu01->SetPnlDefNum();
				}

				if (m_pDlgMenu02)
				{
					m_pDlgMenu02->ChgModelUp(); // PinImg, AlignImg를 Display함.
					m_pDlgMenu02->InitCadImg();
				}

			}
			else
			{
				if (pView->m_mgrProcedure->m_nShareUpS == 1)
				{
					pView->m_mgrProcedure->m_nAoiCamInfoStrPcs[0] = GetAoiUpCamMstInfo();
					if ((pView->m_mgrProcedure->m_nAoiCamInfoStrPcs[0] == 1 ? TRUE : FALSE) != pDoc->WorkingInfo.System.bStripPcsRgnBin)
					{
						if (pView->m_mgrProcedure->m_nAoiCamInfoStrPcs[0])
							pView->MsgBox(_T("현재 마킹부는 일반 모드 인데, \r\n상면 AOI는 DTS 모드에서 검사를 진행하였습니다."));
						else
							pView->MsgBox(_T("현재 마킹부는 DTS 모드 인데, \r\n상면 AOI는 일반 모드에서 검사를 진행하였습니다."));

						Stop();
						TowerLamp(RGB_RED, TRUE);
						break;
					}
				}
			}
			
			if (nNewLot)
			{
				if (!pDoc->m_bNewLotShare[0])
				{
					pDoc->m_bNewLotShare[0] = TRUE;// Lot Change.
					if (!bDualTest)
						pView->m_mgrReelmap->OpenReelmapFromBuf(pView->m_mgrProcedure->m_nShareUpS);
				}
			}
			
			LoadPcrUp(pView->m_mgrProcedure->m_nShareUpS);				// Default: From Buffer, TRUE: From Share
			if (pDoc->m_bUpdateForNewJob[0])
			{
				pDoc->m_bUpdateForNewJob[0] = FALSE; 
				if (pView->m_pDlgMenu01)
					pView->m_pDlgMenu01->UpdateData();
			}

			if (!bDualTest)
			{
				if (pView->m_mgrProcedure->m_nShareUpS != pView->m_mgrProcedure->m_nShareUpSprev)
				{
					pView->m_mgrProcedure->m_nShareUpSprev = pView->m_mgrProcedure->m_nShareUpS;
					pView->m_mgrReelmap->UpdateReelmap(pView->m_mgrProcedure->m_nShareUpS); // 시리얼파일의 정보로 릴맵을 만듬 
				}
			}

			if (!pView->m_mgrProcedure->m_bLastProc)
			{
				if (!IsSetLotEnd())
				{
					if (ChkLotEndUp(pView->m_mgrProcedure->m_nShareUpS))// 파일의 내용 중에 Lot End (-2) 잔량처리를 체크함. (연속 3Pnl:-2) -> 로트완료 
					{
						SetLotEnd(pView->m_mgrProcedure->m_nShareUpS - pDoc->AoiDummyShot[0]);
						if (pView->m_mgrProcedure->m_nAoiLastSerial[0] < 1)
							pView->m_mgrProcedure->m_nAoiLastSerial[0] = pView->m_mgrProcedure->m_nShareUpS;

						if (!bDualTest)
						{
							pView->m_mgrProcedure->m_bLastProc = TRUE;
							pView->m_mgrProcedure->m_nLastProcAuto = LAST_PROC;
						}
					}
				}
				if (ChkLastProc())
				{
					pView->m_mgrProcedure->m_nLastProcAuto = LAST_PROC;
					pView->m_mgrProcedure->m_bLastProc = TRUE;

					if (IsVs())
					{
						if (pView->m_mgrProcedure->m_nAoiLastSerial[0] < 1)
							pView->m_mgrProcedure->m_nAoiLastSerial[0] = pView->m_mgrProcedure->m_nShareUpS;

						pView->m_mgrProcedure->m_nPrevStepAuto = pView->m_mgrProcedure->m_nStepAuto;
						pView->m_mgrProcedure->m_nStepAuto = LAST_PROC_VS_ALL;		 // 잔량처리 3
						break;
					}
					else
					{
						if (bDualTest)
						{
							if (ChkLastProcFromEng())
								nSerial = pDoc->GetCurrentInfoEngShotNum();
							else if (ChkLastProcFromUp())
								nSerial = pDoc->m_ListBuf[0].GetLast();
							else
								nSerial = pDoc->m_ListBuf[1].GetLast();
						}
						else
						{
							if (ChkLastProcFromEng())
								nSerial = pDoc->GetCurrentInfoEngShotNum();
							else
								nSerial = pDoc->m_ListBuf[0].GetLast();
						}

						if (!IsSetLotEnd()) // 20160810
						{
							SetLotEnd(nSerial);//+pDoc->AoiDummyShot[1]); // 3
							pView->m_mgrProcedure->m_nAoiLastSerial[0] = nSerial;
						}
					}
				}
			}
			else
			{
				if (ChkLastProcFromEng())
				{
					nSerial = pDoc->GetCurrentInfoEngShotNum();

					if (!IsSetLotEnd())
					{
						SetLotEnd(nSerial);
						pView->m_mgrProcedure->m_nAoiLastSerial[0] = nSerial;
					}
				}
			}
		}
		break;

	case AT_LP + 5:
		if (!pView->m_mgrProcedure->IsRun())
			break;

		if (!bDualTest)
		{
			pView->m_mgrProcedure->m_nStepAuto++;
			break;
		}

		if (pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_DN || pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_ALLUP 
			|| pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_ALLDN) // Write Reelmap
			break;

		pView->m_mgrProcedure->m_nStepAuto++;

		if (pView->m_mgrProcedure->m_bChkLastProcVs)
		{
			if (pView->m_mgrProcedure->m_nShareDnS > pView->m_mgrProcedure->m_nAoiLastSerial[0] 
				&& pView->m_mgrProcedure->m_nAoiLastSerial[0] > 0)
				break;
		}
		else
		{
			if (IsSetLotEnd())
			{
				if (pView->m_mgrProcedure->m_nShareDnS > pView->m_mgrProcedure->m_nAoiLastSerial[0] 
					&& pView->m_mgrProcedure->m_nAoiLastSerial[0] > 0)
					break;
			}
		}


		if (pView->m_mgrProcedure->m_nShareDnS > 0)
		{
			if (pView->m_mgrProcedure->m_nShareDnS % 2)
				pView->m_mgrProcedure->m_nShareDnSerial[0] = pView->m_mgrProcedure->m_nShareDnS; // 홀수
			else
				pView->m_mgrProcedure->m_nShareDnSerial[1] = pView->m_mgrProcedure->m_nShareDnS; // 짝수
			pView->m_mgrProcedure->m_nShareDnCnt++;


			bNewModel = GetAoiDnInfo(pView->m_mgrProcedure->m_nShareDnS, &nNewLot);

			if (bNewModel)	// AOI 정보(AoiCurrentInfoPath) -> AOI Feeding Offset
			{
				//MsgBox(_T("신규 모델에 의한 AOI(하)에서 로트 분리가 되었습니다.\r\n이전 로트를 잔량처리 합니다.");
				InitInfo();
				ResetMkInfo(1); // 0 : AOI-Up , 1 : AOI-Dn , 2 : AOI-UpDn	
				ModelChange(1); // 0 : AOI-Up , 1 : AOI-Dn

			}
			else
			{
				if (pView->m_mgrProcedure->m_nShareDnS == 1)
				{
					pView->m_mgrProcedure->m_nAoiCamInfoStrPcs[1] = GetAoiDnCamMstInfo();
					if ((pView->m_mgrProcedure->m_nAoiCamInfoStrPcs[1] == 1 ? TRUE : FALSE) != pDoc->WorkingInfo.System.bStripPcsRgnBin)
					{
						if (pView->m_mgrProcedure->m_nAoiCamInfoStrPcs[1])
							pView->MsgBox(_T("현재 마킹부는 일반 모드 인데, \r\n하면 AOI는 DTS 모드에서 검사를 진행하였습니다."));
						else
							pView->MsgBox(_T("현재 마킹부는 DTS 모드 인데, \r\n하면 AOI는 일반 모드에서 검사를 진행하였습니다."));

						Stop();
						TowerLamp(RGB_RED, TRUE);
						break;
					}
				}
			}

			if (nNewLot)
			{
				if (!pDoc->m_bNewLotShare[1])
				{
					pDoc->m_bNewLotShare[1] = TRUE;// Lot Change.				
					if (bDualTest)
						pView->m_mgrReelmap->OpenReelmapFromBuf(pView->m_mgrProcedure->m_nShareDnS);
				}
			}

			LoadPcrDn(pView->m_mgrProcedure->m_nShareDnS);
			if (pDoc->m_bUpdateForNewJob[1])
			{
				pDoc->m_bUpdateForNewJob[1] = FALSE;
				if (pView->m_pDlgMenu01)
					pView->m_pDlgMenu01->UpdateData();
			}


			if (bDualTest)
			{
				if (pView->m_mgrProcedure->m_nShareDnS != pView->m_mgrProcedure->m_nShareDnSprev)
				{
					pView->m_mgrProcedure->m_nShareDnSprev = pView->m_mgrProcedure->m_nShareDnS;
					pView->m_mgrReelmap->UpdateReelmap(pView->m_mgrProcedure->m_nShareDnS);  // 시리얼파일의 정보로 릴맵을 만듬  // After inspect bottom side.
				}
			}


			if (!pView->m_mgrProcedure->m_bLastProc)
			{
				if (!IsSetLotEnd())
				{
					if (ChkLotEndDn(pView->m_mgrProcedure->m_nShareDnS))// 파일의 내용 중에 Lot End (-2) 잔량처리를 체크함. (연속 3Pnl:-2) -> 로트완료 
					{
						if (!IsSetLotEnd())
							SetLotEnd(pView->m_mgrProcedure->m_nShareDnS - pDoc->AoiDummyShot[1]);
						if (pView->m_mgrProcedure->m_nAoiLastSerial[0] < 1)
							pView->m_mgrProcedure->m_nAoiLastSerial[0] = pView->m_mgrProcedure->m_nShareDnS;
						if (bDualTest)
						{
							pView->m_mgrProcedure->m_bLastProc = TRUE;
							pView->m_mgrProcedure->m_nLastProcAuto = LAST_PROC;
						}
					}
				}
				if (ChkLastProc())
				{
					pView->m_mgrProcedure->m_nLastProcAuto = LAST_PROC;
					pView->m_mgrProcedure->m_bLastProc = TRUE;

					if (IsVs())
					{
						if (pView->m_mgrProcedure->m_nAoiLastSerial[0] < 1)
							pView->m_mgrProcedure->m_nAoiLastSerial[0] = pView->m_mgrProcedure->m_nShareDnS;

						pView->m_mgrProcedure->m_nPrevStepAuto = pView->m_mgrProcedure->m_nStepAuto;
						pView->m_mgrProcedure->m_nStepAuto = LAST_PROC_VS_ALL;		 // 잔량처리 3
						break;
					}
					else
					{
						if (ChkLastProcFromEng())
							nSerial = pDoc->GetCurrentInfoEngShotNum();
						else if (ChkLastProcFromUp())
							nSerial = pDoc->m_ListBuf[0].GetLast();
						else
							nSerial = pDoc->m_ListBuf[1].GetLast();

						if (!IsSetLotEnd()) // 20160810
						{
							SetLotEnd(nSerial);//+pDoc->AoiDummyShot[1]); // 3
							pView->m_mgrProcedure->m_nAoiLastSerial[0] = nSerial;
						}
					}
				}
			}
			else
			{
				if (ChkLastProcFromEng())
				{
					nSerial = pDoc->GetCurrentInfoEngShotNum();

					if (!IsSetLotEnd())
					{
						SetLotEnd(nSerial);
						pView->m_mgrProcedure->m_nAoiLastSerial[0] = nSerial;
					}
				}
			}
		}
		break;

	case AT_LP + 6:
		pView->m_mgrProcedure->m_nStepAuto++;
		if (pView->m_mgrProcedure->m_nShareUpS > 0)
		{
			if (pView->m_pDlgFrameHigh)
				pView->m_pDlgFrameHigh->SetAoiLastShot(0, pView->m_mgrProcedure->m_nShareUpS);
		}
		if (bDualTest)
		{
			if (pView->m_mgrProcedure->m_nShareDnS > 0)
			{
				if (pView->m_mgrProcedure->m_bChkLastProcVs)
				{
					if (pView->m_mgrProcedure->m_nShareDnS > pView->m_mgrProcedure->m_nAoiLastSerial[0] 
						&& pView->m_mgrProcedure->m_nAoiLastSerial[0] > 0)
						break;
				}
				else
				{
					if (IsSetLotEnd())
					{
						if (pView->m_mgrProcedure->m_nShareDnS > pView->m_mgrProcedure->m_nAoiLastSerial[0] 
							&& pView->m_mgrProcedure->m_nAoiLastSerial[0] > 0)
							break;
					}
				}

				if (pView->m_pDlgFrameHigh)
					pView->m_pDlgFrameHigh->SetAoiLastShot(1, pView->m_mgrProcedure->m_nShareDnS);
			}
		}
		break;

	case AT_LP + 7:
		pView->m_mgrProcedure->m_nShareUpS = 0;
		pView->m_mgrProcedure->m_nShareDnS = 0;
		pView->m_mgrProcedure->m_nStepAuto++;
		break;

	case AT_LP + 8:
		if (pView->m_mgrProcedure->IsRun())
		{
			if (pDoc->GetTestMode() == MODE_OUTER)
			{
				if (pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_INNER_UP) // Write Reelmap
					break;

				if (pDoc->WorkingInfo.LastJob.bDualTestInner)
				{
					if (pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_INNER_DN || pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_INNER_ALLUP 
						|| pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_INNER_ALLDN) // Write Reelmap
						break;
				}
			}

			if (bDualTest)
			{
				if (pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_UP || pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_DN 
					|| pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_ALLUP || pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_ALLDN) // Write Reelmap
					break;
			}
			else
			{
				if (pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_UP) // Write Reelmap
					break;
			}
			//SetListBuf(); // 20170727-잔량처리 시 계속적으로 반복해서 이함수가 호출됨으로 좌우 마킹 인덱스 동일 현상 발생.(case AT_LP + 8:)
			pView->m_mgrProcedure->m_bLoadShare[0] = FALSE;
			pView->m_mgrProcedure->m_bLoadShare[1] = FALSE;
			pView->m_mgrProcedure->m_nStepAuto = AT_LP;
		}
		break;


	case LAST_PROC_VS_ALL:			 // 잔량처리 3
		pView->m_mgrProcedure->m_nDummy[0] = 0;
		pView->m_mgrProcedure->m_nDummy[1] = 0;
		pView->m_mgrProcedure->m_bChkLastProcVs = TRUE;
		TowerLamp(RGB_GREEN, TRUE);
		DispMain(_T("상면VS잔량"), RGB_GREEN);
		if (pView->m_mgrProcedure->m_nAoiLastSerial[0] < 1)
			pView->m_mgrProcedure->m_nAoiLastSerial[0] = GetAoiUpSerial();
		if (!IsSetLotEnd())
			SetLotEnd(pView->m_mgrProcedure->m_nAoiLastSerial[0]);
		//m_nAoiLastSerial[1] = GetAoiDnSerial();
		pView->m_mgrProcedure->m_nStepAuto++;
		break;

	case LAST_PROC_VS_ALL + 1:
		if (IsVsUp())
			pView->m_mgrProcedure->m_nStepAuto++;
		else
			pView->m_mgrProcedure->m_nStepAuto = pView->m_mgrProcedure->m_nPrevStepAuto;
		break;

	case LAST_PROC_VS_ALL + 2:
		SetDummyUp();
		pView->m_mgrProcedure->m_nStepAuto++;
		break;

	case LAST_PROC_VS_ALL + 3:
		SetDummyUp();
		pView->m_mgrProcedure->m_nStepAuto++;
		break;

	case LAST_PROC_VS_ALL + 4:
		SetDummyUp();
		pView->m_mgrProcedure->m_nStepAuto++;
		break;

	case LAST_PROC_VS_ALL + 5:
		pView->m_mgrProcedure->m_nStepAuto = pView->m_mgrProcedure->m_nPrevStepAuto;
		break;
	}
}

void CGvisR2R_PunchView::DoAutoMarking()
{
	if(pDoc->WorkingInfo.LastJob.nAlignMethode == TWO_POINT)
		MarkingWith2PointAlign();
	else if(pDoc->WorkingInfo.LastJob.nAlignMethode == FOUR_POINT)
		MarkingWith4PointAlign();
	else
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("마킹을 위한 Align방식이 정해지지 않았습니다."));
	}
}

void CGvisR2R_PunchView::Mk2PtReady()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	CString sMsg;

	if (pView->m_mgrProcedure->m_bMkSt)
	{
		switch (pView->m_mgrProcedure->m_nMkStAuto)
		{
		case MK_ST:	// PLC MK 신호 확인	
			if (pView->m_mgrProcedure->IsRun())
			{
				m_pMpe->Write(_T("MB440150"), 1);// 마킹부 마킹중 ON (PC가 ON, OFF)
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
						sMsg.Format(_T("2D 리더기가 연결이 되지 않았습니다."));
						AfxMessageBox(sMsg);
						m_nMkStAuto = MK_ST;
						break;
					}
				}
#endif
				pView->m_mgrProcedure->m_nMkStAuto++;
			}
			break;
		case MK_ST + 1:
			if (!pView->m_mgrProcedure->m_bTHREAD_SHIFT2MK)
			{
				SetListBuf();
				pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::Start);
			}
			break;
		case MK_ST + (Mk2PtIdx::Start) :	// 2
			if (bDualTest)
			{
				if (pDoc->m_ListBuf[1].nTot > 0) // AOI-Dn
				{
					pView->m_mgrProcedure->m_nMkStAuto++;

					pView->m_mgrProcedure->m_nBufDnSerial[0] = pDoc->m_ListBuf[1].Pop();
					pView->m_mgrProcedure->m_nBufUpSerial[0] = pView->m_mgrProcedure->m_nBufDnSerial[0];
					if (pDoc->m_ListBuf[1].nTot > 0) // AOI-Dn
					{
						pView->m_mgrProcedure->m_nBufDnSerial[1] = pDoc->m_ListBuf[1].Pop();
						pView->m_mgrProcedure->m_nBufUpSerial[1] = pView->m_mgrProcedure->m_nBufDnSerial[1];
					}
					else
					{
						pView->m_mgrProcedure->m_nBufDnSerial[1] = 0;
						pView->m_mgrProcedure->m_nBufUpSerial[1] = 0;
					}
				}
				else
				{
					pView->m_mgrProcedure->m_bLotEnd = TRUE;
					pView->m_mgrProcedure->m_nLotEndAuto = LOT_END;
				}

				if (pDoc->WorkingInfo.LastJob.bSampleTest)
				{
					if (pView->m_mgrProcedure->m_nBufUpSerial[0] == 1)
					{
						pView->m_mgrProcedure->m_nLotEndSerial = _tstoi(pDoc->WorkingInfo.LastJob.sSampleTestShotNum);
						pView->m_mgrProcedure->m_bLastProcFromUp = FALSE;
						pView->m_mgrProcedure->m_bLastProcFromEng = FALSE;
						pView->m_mgrProcedure->m_bLastProc = TRUE;
						if (m_pDlgMenu01)
							m_pDlgMenu01->m_bLastProc = TRUE;
						if (m_pMpe)
						{
							m_pMpe->Write(_T("MB440186"), 1);			// 잔량처리 AOI(하) 부터(PC가 On시키고, PLC가 확인하고 Off시킴)-20141112
							m_pMpe->Write(_T("MB440181"), 1);			// 잔량처리(PC가 On시키고, PLC가 확인하고 Off시킴)-20141031
						}
					}
				}
			}
			else
			{
				if (pDoc->m_ListBuf[0].nTot > 0) // AOI-Up
				{
					pView->m_mgrProcedure->m_nMkStAuto++;
					pView->m_mgrProcedure->m_nBufUpSerial[0] = pDoc->m_ListBuf[0].Pop();
					if (pDoc->m_ListBuf[0].nTot > 0) // AOI-Up
						pView->m_mgrProcedure->m_nBufUpSerial[1] = pDoc->m_ListBuf[0].Pop();
					else
						pView->m_mgrProcedure->m_nBufUpSerial[1] = 0;
				}
				else
				{
					pView->m_mgrProcedure->m_bLotEnd = TRUE;
					pView->m_mgrProcedure->m_nLotEndAuto = LOT_END;
				}

				if (pDoc->WorkingInfo.LastJob.bSampleTest)
				{
					if (pView->m_mgrProcedure->m_nBufUpSerial[0] == 1)
					{
						pView->m_mgrProcedure->m_nLotEndSerial = _tstoi(pDoc->WorkingInfo.LastJob.sSampleTestShotNum);
						pView->m_mgrProcedure->m_bLastProcFromUp = FALSE;
						pView->m_mgrProcedure->m_bLastProcFromEng = FALSE;
						pView->m_mgrProcedure->m_bLastProc = TRUE;
						if (m_pDlgMenu01)
							m_pDlgMenu01->m_bLastProc = TRUE;
						if (m_pMpe)
						{
							m_pMpe->Write(_T("MB440186"), 1);			// 잔량처리 AOI(하) 부터(PC가 On시키고, PLC가 확인하고 Off시킴)-20141112
							m_pMpe->Write(_T("MB440181"), 1);			// 잔량처리(PC가 On시키고, PLC가 확인하고 Off시킴)-20141031
						}
					}
				}
			}
			break;
		case MK_ST + (Mk2PtIdx::Start) + 1:
			if (pDoc->GetTestMode() == MODE_OUTER)
			{
				if (!pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_UP && !pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_DN
					&& !pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_ALLUP && !pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_ALLDN
					&& !pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_INNER_UP && !pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_INNER_DN
					&& !pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_INNER_ALLUP && !pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_INNER_ALLDN) // Write Reelmap
					pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::ChkSn);
			}
			else
			{
				if (!pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_UP && !pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_DN
					&& !pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_ALLUP && !pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_ALLDN) // Write Reelmap
					pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::ChkSn);
			}
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk2PtChkSerial()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	CString sNewLot;
	BOOL bNewModel = FALSE;
	int nSerial = 0;
	int nNewLot = 0;
	int nLastShot = 0;
	double dFdEnc;
	CString sLot, sMsg;

	if (pView->m_mgrProcedure->m_bTHREAD_SHIFT2MK)
		return;

	if (pView->m_mgrProcedure->m_bMkSt)
	{
		switch (pView->m_mgrProcedure->m_nMkStAuto)
		{
		case MK_ST + (Mk2PtIdx::ChkSn) :
			if (pView->m_mgrProcedure->m_nBufUpSerial[0] == pView->m_mgrProcedure->m_nBufUpSerial[1])
			{
				Stop();
				pView->ClrDispMsg();
				AfxMessageBox(_T("좌/우 마킹 시리얼이 같습니다."));
				SetListBuf();
				pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::Start);
				break;
			}

			if (pDoc->GetTestMode() == MODE_OUTER)
			{
				if (!pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_UP && !pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_DN
					&& !pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_ALLUP && !pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_ALLDN
					&& !pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_INNER_UP && !pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_INNER_DN
					&& !pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_INNER_ALLUP && !pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_INNER_ALLDN) // Write Reelmap
				{
					if (!pView->m_mgrProcedure->m_bTHREAD_DISP_DEF && !pView->m_mgrProcedure->m_bTHREAD_DISP_DEF_INNER)
					{
						pView->m_mgrProcedure->m_nMkStAuto++;
						pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF = 0;
						pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF_INNER = 0;

						pView->m_mgrProcedure->m_bTHREAD_DISP_DEF = TRUE;				// DispDefImg() : CopyDefImg Start -> Disp Reelmap Start
						pView->m_mgrProcedure->m_bTHREAD_DISP_DEF_INNER = TRUE;		// DispDefImgInner() : Disp Reelmap Start

						pDoc->SetMkMenu01(_T("Signal"), _T("DispDefImg"), _T("1"));
					}
				}
			}
			else
			{
				if (!pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_UP && !pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_DN &&
					!pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_ALLUP && !pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_ALLDN) // Write Reelmap
				{
					if (!pView->m_mgrProcedure->m_bTHREAD_DISP_DEF)
					{
						pView->m_mgrProcedure->m_nMkStAuto++;
						pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF = 0;

						pView->m_mgrProcedure->m_bTHREAD_DISP_DEF = TRUE;		// DispDefImg() : CopyDefImg Start -> Disp Reelmap Start
						pDoc->SetMkMenu01(_T("Signal"), _T("DispDefImg"), _T("1"));
					}
				}
			}

			break;

		case MK_ST + (Mk2PtIdx::ChkSn) + 1:
			pView->m_mgrProcedure->m_nMkStAuto++;
			nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0];
			sNewLot = pView->m_mgrProcedure->m_sNewLotUp;

			if (nSerial > 0)
			{
				if (pView->m_mgrProcedure->m_bSerialDecrese)
				{
					if (pView->m_mgrProcedure->m_bLastProc && pView->m_mgrProcedure->m_nBufUpSerial[1] < pView->m_mgrProcedure->m_nLotEndSerial)
						nLastShot = pView->m_mgrProcedure->m_nBufUpSerial[0];
				}
				else
				{
					if (pView->m_mgrProcedure->m_bLastProc && pView->m_mgrProcedure->m_nBufUpSerial[1] > pView->m_mgrProcedure->m_nLotEndSerial)
					{
						nLastShot = pView->m_mgrProcedure->m_nBufUpSerial[0];
						//nSerial = m_nBufUpSerial[0];
					}
				}

				bNewModel = GetAoiUpInfo(nSerial, &nNewLot, TRUE);
				if (bDualTest)
				{
					bNewModel = GetAoiDnInfo(nSerial, &nNewLot, TRUE);

					if (!IsSameUpDnLot() && !pView->m_mgrProcedure->m_bContDiffLot)
					{
						pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::LotDiff);
						break;
					}
				}

				if (bNewModel)	// AOI 정보(AoiCurrentInfoPath) -> AOI Feeding Offset
				{
					;
				}
				if (nNewLot)
				{
					// Lot Change.
					ChgLot();
#ifdef USE_MPE
					dFdEnc = (double)pDoc->m_pMpeData[0][0];	// 마킹부 Feeding 엔코더 값(단위 mm )
					if ((pDoc->WorkingInfo.LastJob.bLotSep || pDoc->m_bDoneChgLot) && (dFdEnc + _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen)*2.0) > _tstof(pDoc->WorkingInfo.LastJob.sLotSepLen)*1000.0)
					{
						pDoc->m_bDoneChgLot = TRUE;
						pView->m_mgrReelmap->SetPathAtBuf();
					}
#endif
				}
			}
			else
			{
				Stop();
				MsgBox(_T("버퍼(좌) Serial이 맞지않습니다."));
				TowerLamp(RGB_YELLOW, TRUE);
			}
			//sNewLot.Empty();
			break;

		case MK_ST + (Mk2PtIdx::ChkSn) + 2:
			//pDoc->UpdateYieldOnRmap(); // 20230614

#ifdef USE_SR1000W
			if (pDoc->GetTestMode() == MODE_INNER || pDoc->GetTestMode() == MODE_OUTER)
			{
				if (m_pSr1000w && m_pSr1000w->IsConnected())
				{
					//Set2dRead(TRUE);
					//Get2dCode(sLot, nSerial);
					if (m_sGet2dCodeLot != _T("") && m_nGet2dCodeSerial != 0)
					{
						if (m_nBufUpSerial[0] == m_nGet2dCodeSerial)
						{
							m_nMkStAuto = MK_ST + (Mk2PtIdx::InitMk);			// InitMk()
						}
						else
						{
							Stop();
							pView->ClrDispMsg();
							sMsg.Format(_T("2D각인 시리얼(%d)과 검사파일 시리얼(%d)이 다릅니다."), m_nGet2dCodeSerial, m_nBufUpSerial[0]);
							AfxMessageBox(sMsg);
							m_nMkStAuto = MK_ST + (Mk2PtIdx::Start);
							break;
						}
					}
					else
						m_nMkStAuto = MK_ST + (Mk2PtIdx::InitMk);			// InitMk()

				}
				else
				{
					Stop();
					pView->ClrDispMsg();
					sMsg.Format(_T("2D 리더기가 연결이 되지 않았습니다."));
					AfxMessageBox(sMsg);
					m_nMkStAuto = MK_ST + (Mk2PtIdx::Start);
					break;
				}
			}
			else
				m_nMkStAuto = MK_ST + (Mk2PtIdx::InitMk);					// InitMk()
#else
			pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::InitMk);					// InitMk()
#endif
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk2PtInit()
{
	if (pView->m_mgrProcedure->m_bTHREAD_SHIFT2MK)
		return;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pView->m_mgrProcedure->m_bMkSt)
	{
		switch (pView->m_mgrProcedure->m_nMkStAuto)
		{
		case MK_ST + (Mk2PtIdx::InitMk) :
			if (InitMk()) // 시리얼이 증가 또는 감소 : TRUE 아니고 같으면 : FALSE
			{
				if (pView->m_mgrProcedure->m_bSerialDecrese)
				{
					if ((pView->m_mgrProcedure->m_nBufUpSerial[0] <= pView->m_mgrProcedure->m_nLotEndSerial 
						|| pView->m_mgrProcedure->m_nBufUpSerial[1] <= pView->m_mgrProcedure->m_nLotEndSerial) 
						&& pView->m_mgrProcedure->m_nLotEndSerial > 0)
						m_pMpe->Write(_T("MB440171"), 1); // 마킹부 작업완료.(PC가 On, PLC가 확인 후 Off) - 20160718
				}
				else
				{
					if ((pView->m_mgrProcedure->m_nBufUpSerial[0] >= pView->m_mgrProcedure->m_nLotEndSerial 
						|| pView->m_mgrProcedure->m_nBufUpSerial[1] >= pView->m_mgrProcedure->m_nLotEndSerial) 
						&& pView->m_mgrProcedure->m_nLotEndSerial > 0)
						m_pMpe->Write(_T("MB440171"), 1); // 마킹부 작업완료.(PC가 On, PLC가 확인 후 Off) - 20160718
				}
			}
			else // Same Serial
			{
				Stop();
				MsgBox(_T("Serial 연속 되지않습니다."));
				TowerLamp(RGB_YELLOW, TRUE);
			}
										pView->m_mgrProcedure->m_nMkStAuto++;
			break;

		case MK_ST + (Mk2PtIdx::InitMk) + 1:
			if (pView->m_mgrProcedure->IsRun())
			{
				if (MODE_INNER != pDoc->GetTestMode())
					pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::Move0Cam1);	// Move - Cam1 - Pt0
				else
					pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::MoveInitPt);
			}
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk2PtAlignPt0()
{
	if (!pView->m_mgrProcedure->IsRun())
		return;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pView->m_mgrProcedure->m_bMkSt)
	{
		switch (pView->m_mgrProcedure->m_nMkStAuto)
		{
		case MK_ST + (Mk2PtIdx::Move0Cam1) :	// Move - Cam1 - Pt0
			if (bDualTest)
			{
				if (pView->m_mgrProcedure->m_bSerialDecrese)
				{
					if (pView->m_mgrProcedure->m_bLastProc && pView->m_mgrProcedure->m_nBufDnSerial[1] < pView->m_mgrProcedure->m_nLotEndSerial)	// AOI하면 Serial
					{
						pView->m_mgrProcedure->m_bSkipAlign[1][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
						pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						pView->m_mgrProcedure->m_nMkStAuto++;
					}
					else if (pView->m_mgrProcedure->m_nBufDnSerial[1] == 0)
					{
						pView->m_mgrProcedure->m_bSkipAlign[1][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
						pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						pView->m_mgrProcedure->m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(0))	// Move - Cam1 - Pt0
							pView->m_mgrProcedure->m_nMkStAuto++;
					}
				}
				else
				{
					if (pView->m_mgrProcedure->m_bLastProc && pView->m_mgrProcedure->m_nBufDnSerial[1] > pView->m_mgrProcedure->m_nLotEndSerial)	// AOI하면 Serial
					{
						pView->m_mgrProcedure->m_bSkipAlign[1][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
						pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						pView->m_mgrProcedure->m_nMkStAuto++;
					}
					else if (pView->m_mgrProcedure->m_nBufDnSerial[1] == 0)
					{
						pView->m_mgrProcedure->m_bSkipAlign[1][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
						pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						pView->m_mgrProcedure->m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(0))	// Move - Cam1 - Pt0
							pView->m_mgrProcedure->m_nMkStAuto++;
					}
				}
			}
			else
			{
				if (pView->m_mgrProcedure->m_bSerialDecrese)
				{
					if (pView->m_mgrProcedure->m_bLastProc && pView->m_mgrProcedure->m_nBufUpSerial[1] < pView->m_mgrProcedure->m_nLotEndSerial)	// AOI상면 Serial
					{
						pView->m_mgrProcedure->m_bSkipAlign[1][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
						pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						pView->m_mgrProcedure->m_nMkStAuto++;
					}
					else if (pView->m_mgrProcedure->m_nBufUpSerial[1] == 0)
					{
						pView->m_mgrProcedure->m_bSkipAlign[1][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
						pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						pView->m_mgrProcedure->m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(0)) 	// Move - Cam1 - Pt0
							pView->m_mgrProcedure->m_nMkStAuto++;
					}
				}
				else
				{
					if (pView->m_mgrProcedure->m_bLastProc && pView->m_mgrProcedure->m_nBufUpSerial[1] > pView->m_mgrProcedure->m_nLotEndSerial)	// AOI상면 Serial
					{
						pView->m_mgrProcedure->m_bSkipAlign[1][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
						pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						pView->m_mgrProcedure->m_nMkStAuto++;
					}
					else if (pView->m_mgrProcedure->m_nBufUpSerial[1] == 0)
					{
						pView->m_mgrProcedure->m_bSkipAlign[1][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
						pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						pView->m_mgrProcedure->m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(0)) 	// Move - Cam1 - Pt0
							pView->m_mgrProcedure->m_nMkStAuto++;
					}
				}
			}
			break;
		case MK_ST + (Mk2PtIdx::Move0Cam1) + 1:
			if (pView->m_mgrProcedure->IsRun())
				pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::Move0Cam0);
			break;
		case MK_ST + (Mk2PtIdx::Move0Cam0) :	// Move - Cam0 - Pt0
			if (MoveAlign0(0))
				pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk2PtIdx::Move0Cam0) + 1:
			if (pView->m_mgrProcedure->IsRun())
				pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk2PtIdx::Move0Cam0) + 2:
			if (IsMoveDone())
			{
				Sleep(100);
				pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::Align1_0);
			}
			break;
		case MK_ST + (Mk2PtIdx::Align1_0) :	// 2PtAlign - Cam1 - Pt0
			if (!pView->m_mgrProcedure->m_bSkipAlign[1][0])
			{
				if (TwoPointAlign1(0))
					pView->m_mgrProcedure->m_bFailAlign[1][0] = FALSE;
				else
					pView->m_mgrProcedure->m_bFailAlign[1][0] = TRUE;
			}
											pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::Align0_0);
			break;
		case MK_ST + (Mk2PtIdx::Align0_0) :	// 2PtAlign - Cam0 - Pt0
			if (!pView->m_mgrProcedure->m_bSkipAlign[0][0])
			{
				if (TwoPointAlign0(0))
					pView->m_mgrProcedure->m_bFailAlign[0][0] = FALSE;
				else
					pView->m_mgrProcedure->m_bFailAlign[0][0] = TRUE;
			}
											pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk2PtIdx::Align0_0) + 1:
			if (pView->m_mgrProcedure->m_bFailAlign[0][0])
			{
				Buzzer(TRUE, 0);
				if (IDNO == MsgBox(_T("카메라(좌)의 검사판넬을 다시 정렬하시겠습니까?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);

					if (IDYES == MsgBox(_T("카메라(좌)의 검사판넬 정렬을 정말 않하시겠습니까?"), 0, MB_YESNO))
					{
						pView->m_mgrProcedure->m_bReAlign[0][0] = FALSE;
						pView->m_mgrProcedure->m_bSkipAlign[0][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[0][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[0][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[0][3] = TRUE;
						if (IDNO == MsgBox(_T("카메라(좌)의 검사판넬을 불량마킹 하시겠습니까?"), 0, MB_YESNO))
						{
							pView->m_mgrProcedure->m_bDoMk[0] = FALSE;
							pView->m_mgrProcedure->m_bDoneMk[0] = TRUE;
						}
						else
						{
							pView->m_mgrProcedure->m_bDoMk[0] = TRUE;
							pView->m_mgrProcedure->m_bDoneMk[0] = FALSE;
						}
					}
					else
					{
						pView->m_mgrProcedure->m_bReAlign[0][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[0][0] = FALSE;
						pView->m_mgrProcedure->m_bSkipAlign[0][1] = FALSE;
						pView->m_mgrProcedure->m_bSkipAlign[0][2] = FALSE;
						pView->m_mgrProcedure->m_bSkipAlign[0][3] = FALSE;
						pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::Move0Cam0); // TwoPointAlign0(0) 으로 진행. - 카메라 재정렬
						Stop();
						TowerLamp(RGB_YELLOW, TRUE);
					}
				}
				else
				{
					Buzzer(FALSE, 0);

					pView->m_mgrProcedure->m_bReAlign[0][0] = TRUE;
					pView->m_mgrProcedure->m_bSkipAlign[0][0] = FALSE;
					pView->m_mgrProcedure->m_bSkipAlign[0][1] = FALSE;
					pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::Move0Cam0); // TwoPointAlign0(0) 으로 진행. - 카메라 재정렬
					Stop();
					TowerLamp(RGB_YELLOW, TRUE);
				}
			}
			if (pView->m_mgrProcedure->m_bFailAlign[1][0])
			{
				Buzzer(TRUE, 0);

				if (IDNO == MsgBox(_T("카메라(우)의 검사판넬을 다시 정렬하시겠습니까?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);

					if (IDYES == MsgBox(_T("카메라(우)의 검사판넬 정렬을 정말 않하시겠습니까?"), 0, MB_YESNO))
					{
						pView->m_mgrProcedure->m_bReAlign[1][0] = FALSE;
						pView->m_mgrProcedure->m_bSkipAlign[1][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = TRUE;
						if (IDNO == MsgBox(_T("카메라(우)의 검사판넬을 불량마킹 하시겠습니까?"), 0, MB_YESNO))
						{
							pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
							pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						}
						else
						{
							pView->m_mgrProcedure->m_bDoMk[1] = TRUE;
							pView->m_mgrProcedure->m_bDoneMk[1] = FALSE;
						}
					}
					else
					{
						pView->m_mgrProcedure->m_bReAlign[1][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][0] = FALSE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = FALSE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = FALSE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = FALSE;
						pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::Move0Cam1); // TwoPointAlign1(0) 으로 진행. - 카메라 재정렬
						Stop();
						TowerLamp(RGB_YELLOW, TRUE);
					}
				}
				else
				{
					Buzzer(FALSE, 0);

					pView->m_mgrProcedure->m_bReAlign[1][0] = TRUE;
					pView->m_mgrProcedure->m_bSkipAlign[1][0] = FALSE;
					pView->m_mgrProcedure->m_bSkipAlign[1][1] = FALSE;
					pView->m_mgrProcedure->m_bSkipAlign[1][2] = FALSE;
					pView->m_mgrProcedure->m_bSkipAlign[1][3] = FALSE;
					pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::Move0Cam1); // TwoPointAlign1(0) 으로 진행. - 카메라 재정렬
					Stop();
					TowerLamp(RGB_YELLOW, TRUE);
				}
			}

			if (pView->m_mgrProcedure->m_bFailAlign[0][0] || pView->m_mgrProcedure->m_bFailAlign[1][0])
			{
				if (!pView->m_mgrProcedure->m_bReAlign[0][0] && !pView->m_mgrProcedure->m_bReAlign[1][0])
				{
					if (pView->m_mgrProcedure->m_bDoMk[0] || pView->m_mgrProcedure->m_bDoMk[1])
						pView->m_mgrProcedure->m_nMkStAuto++; //m_nMkStAuto = MK_ST + 27; // MoveInitPos0()
					else
					{
						if (!IsInitPos0())
							MoveInitPos0();
						if (!IsInitPos1())
							MoveInitPos1();

						pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::DoneMk); // // 불량이미지 Display, Align변수 초기화 (Skip 65 : Mk())
					}
				}
				else
				{
					pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::Move0Cam1); // TwoPointAlign1(0) 으로 진행. - 카메라 재정렬
				}
			}
			else
				pView->m_mgrProcedure->m_nMkStAuto++;

			break;
		case MK_ST + (Mk2PtIdx::Align0_0) + 2:
			if (pView->m_mgrProcedure->IsRun())
				pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::Move1Cam1);
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk2PtAlignPt1()
{
	if (!pView->m_mgrProcedure->IsRun())
		return;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pView->m_mgrProcedure->m_bMkSt)
	{
		switch (pView->m_mgrProcedure->m_nMkStAuto)
		{
		case MK_ST + (Mk2PtIdx::Move1Cam1) :
			if (bDualTest)
			{
				if (pView->m_mgrProcedure->m_bSerialDecrese)
				{
					if (pView->m_mgrProcedure->m_bLastProc && pView->m_mgrProcedure->m_nBufDnSerial[1] < pView->m_mgrProcedure->m_nLotEndSerial)	// AOI하면 Serial
					{
						pView->m_mgrProcedure->m_bSkipAlign[1][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
						pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						pView->m_mgrProcedure->m_nMkStAuto++;
					}
					else if (pView->m_mgrProcedure->m_nBufDnSerial[1] == 0)
					{
						pView->m_mgrProcedure->m_bSkipAlign[1][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
						pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						pView->m_mgrProcedure->m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(1))	// Move - Cam1 - Pt1
							pView->m_mgrProcedure->m_nMkStAuto++;
					}
				}
				else
				{
					if (pView->m_mgrProcedure->m_bLastProc && pView->m_mgrProcedure->m_nBufDnSerial[1] > pView->m_mgrProcedure->m_nLotEndSerial)	// AOI하면 Serial
					{
						pView->m_mgrProcedure->m_bSkipAlign[1][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
						pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						pView->m_mgrProcedure->m_nMkStAuto++;
					}
					else if (pView->m_mgrProcedure->m_nBufDnSerial[1] == 0)
					{
						pView->m_mgrProcedure->m_bSkipAlign[1][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
						pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						pView->m_mgrProcedure->m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(1))	// Move - Cam1 - Pt1
							pView->m_mgrProcedure->m_nMkStAuto++;
					}
				}
			}
			else
			{
				if (pView->m_mgrProcedure->m_bSerialDecrese)
				{
					if (pView->m_mgrProcedure->m_bLastProc && pView->m_mgrProcedure->m_nBufUpSerial[1] < pView->m_mgrProcedure->m_nLotEndSerial)	// AOI상면 Serial
					{
						pView->m_mgrProcedure->m_bSkipAlign[1][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
						pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						pView->m_mgrProcedure->m_nMkStAuto++;
					}
					else if (pView->m_mgrProcedure->m_nBufUpSerial[1] == 0)
					{
						pView->m_mgrProcedure->m_bSkipAlign[1][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
						pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						pView->m_mgrProcedure->m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(1))	// Move - Cam1 - Pt1
							pView->m_mgrProcedure->m_nMkStAuto++;
					}
				}
				else
				{
					if (pView->m_mgrProcedure->m_bLastProc && pView->m_mgrProcedure->m_nBufUpSerial[1] > pView->m_mgrProcedure->m_nLotEndSerial)	// AOI상면 Serial
					{
						pView->m_mgrProcedure->m_bSkipAlign[1][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
						pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						pView->m_mgrProcedure->m_nMkStAuto++;
					}
					else if (pView->m_mgrProcedure->m_nBufUpSerial[1] == 0)
					{
						pView->m_mgrProcedure->m_bSkipAlign[1][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
						pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						pView->m_mgrProcedure->m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(1))	// Move - Cam1 - Pt1
							pView->m_mgrProcedure->m_nMkStAuto++;
					}
				}
			}
		   break;
		case MK_ST + (Mk2PtIdx::Move1Cam1) + 1:
			if (pView->m_mgrProcedure->IsRun())
				pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk2PtIdx::Move1Cam0) :
			if (MoveAlign0(1))	// Move - Cam0 - Pt1
				pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk2PtIdx::Move1Cam0) + 1:
			if (pView->m_mgrProcedure->IsRun())
				pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk2PtIdx::Move1Cam0) + 2:
			if (IsMoveDone())
			{
				Sleep(100);
				pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::Align1_1);
			}
			break;
		case MK_ST + (Mk2PtIdx::Align1_1) :	// 2PtAlign - Cam1 - Pt1
			if (!pView->m_mgrProcedure->m_bFailAlign[1][0])
			{
				if (!pView->m_mgrProcedure->m_bSkipAlign[1][1])
				{
					if (!TwoPointAlign1(1))
						pView->m_mgrProcedure->m_bFailAlign[1][1] = TRUE;
					else
						pView->m_mgrProcedure->m_bFailAlign[1][1] = FALSE;
				}
				else
					pView->m_mgrProcedure->m_bFailAlign[1][1] = FALSE;
			}
			else
				pView->m_mgrProcedure->m_bFailAlign[1][1] = FALSE;

			pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::Align0_1);
			break;
		case MK_ST + (Mk2PtIdx::Align0_1) :	// 2PtAlign - Cam0 - Pt1
			if (!pView->m_mgrProcedure->m_bFailAlign[0][0])
			{
				if (!pView->m_mgrProcedure->m_bSkipAlign[0][1])
				{
					if (!TwoPointAlign0(1))
						pView->m_mgrProcedure->m_bFailAlign[0][1] = TRUE;
					else
						pView->m_mgrProcedure->m_bFailAlign[0][1] = FALSE;
				}
				else
					pView->m_mgrProcedure->m_bFailAlign[0][1] = FALSE;
			}
			else
				pView->m_mgrProcedure->m_bFailAlign[0][1] = FALSE;

			pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk2PtIdx::Align0_1) + 1:
			if (pView->m_mgrProcedure->m_bFailAlign[0][1])
			{
				Buzzer(TRUE, 0);

				if (IDNO == MsgBox(_T("카메라(좌)의 검사판넬을 다시 정렬하시겠습니까?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);

					if (IDYES == MsgBox(_T("카메라(좌)의 검사판넬 정렬을 정말 않하시겠습니까?"), 0, MB_YESNO))
					{
						pView->m_mgrProcedure->m_bReAlign[0][1] = FALSE;
						pView->m_mgrProcedure->m_bSkipAlign[0][1] = TRUE;
						if (IDNO == MsgBox(_T("카메라(좌)의 검사판넬을 불량마킹 하시겠습니까?"), 0, MB_YESNO))
						{
							pView->m_mgrProcedure->m_bDoMk[0] = FALSE;
							pView->m_mgrProcedure->m_bDoneMk[0] = TRUE;
						}
						else
						{
							pView->m_mgrProcedure->m_bDoMk[0] = TRUE;
							pView->m_mgrProcedure->m_bDoneMk[0] = FALSE;
						}
					}
					else
					{
						pView->m_mgrProcedure->m_bReAlign[0][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[0][1] = FALSE;
						pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::Move1Cam0); // TwoPointAlign0(1) 으로 진행. - 카메라 재정렬
						Stop();
						TowerLamp(RGB_YELLOW, TRUE);
					}
				}
				else
				{
					Buzzer(FALSE, 0);

					pView->m_mgrProcedure->m_bReAlign[0][1] = TRUE;
					pView->m_mgrProcedure->m_bSkipAlign[0][1] = FALSE;
					pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::Move1Cam0); // TwoPointAlign1(1) 으로 진행. - 카메라 재정렬
					Stop();
					TowerLamp(RGB_YELLOW, TRUE);
				}
			}
			if (pView->m_mgrProcedure->m_bFailAlign[1][1])
			{
				Buzzer(TRUE, 0);

				if (IDNO == MsgBox(_T("카메라(우)의 검사판넬을 다시 정렬하시겠습니까?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);

					if (IDYES == MsgBox(_T("카메라(우)의 검사판넬 정렬을 정말 않하시겠습니까?"), 0, MB_YESNO))
					{
						pView->m_mgrProcedure->m_bReAlign[1][1] = FALSE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = TRUE;
						if (IDNO == MsgBox(_T("카메라(우)의 검사판넬을 불량마킹 하시겠습니까?"), 0, MB_YESNO))
						{
							pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
							pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						}
						else
						{
							pView->m_mgrProcedure->m_bDoMk[1] = TRUE;
							pView->m_mgrProcedure->m_bDoneMk[1] = FALSE;
						}
					}
					else
					{
						pView->m_mgrProcedure->m_bReAlign[1][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = FALSE;
						pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::Move1Cam1); // TwoPointAlign1(1) 으로 진행. - 카메라 재정렬
						Stop();
						TowerLamp(RGB_YELLOW, TRUE);
					}
				}
				else
				{
					Buzzer(FALSE, 0);

					pView->m_mgrProcedure->m_bReAlign[1][1] = TRUE;
					pView->m_mgrProcedure->m_bSkipAlign[1][1] = FALSE;
					pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::Move1Cam1); // TwoPointAlign1(1) 으로 진행. - 카메라 재정렬
					Stop();
					TowerLamp(RGB_YELLOW, TRUE);
				}
			}

			if (pView->m_mgrProcedure->m_bFailAlign[0][1] || pView->m_mgrProcedure->m_bFailAlign[1][1])
			{
				if (!pView->m_mgrProcedure->m_bReAlign[0][1] && !pView->m_mgrProcedure->m_bReAlign[1][1])
				{
					if (pView->m_mgrProcedure->m_bDoMk[0] || pView->m_mgrProcedure->m_bDoMk[1])
						pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::MoveInitPt); //m_nMkStAuto = MK_ST + 29;  // MoveInitPos0()
					else
					{
						if (!IsInitPos0())
							MoveInitPos0();
						if (!IsInitPos1())
							MoveInitPos1();

						pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::DoneMk); // 불량이미지 Display, Align변수 초기화 (Skip 65 : Mk())
					}
				}
				else
					pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::Move1Cam1); // TwoPointAlign1(1) 으로 진행. - 카메라 재정렬
			}
			else
				pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::MoveInitPt);

			break;
		}
	}
}

void CGvisR2R_PunchView::Mk2PtMoveInitPos()
{
	if (pView->m_mgrProcedure->m_bMkSt)
	{
		switch (pView->m_mgrProcedure->m_nMkStAuto)
		{
		case MK_ST + (Mk2PtIdx::MoveInitPt) :
			MoveInitPos0(FALSE);
			MoveInitPos1(FALSE); // 20220526
			pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk2PtIdx::MoveInitPt) + 1:
			pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk2PtIdx::MoveInitPt) + 2:
			if (IsMoveDone())
				pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::ChkElec);
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk2PtElecChk()
{
	CString sRst;

	if (pView->m_mgrProcedure->m_bMkSt)
	{
		switch (pView->m_mgrProcedure->m_nMkStAuto)
		{
		case MK_ST + (Mk2PtIdx::ChkElec) : // DoElecChk
			if (DoElecChk(sRst))
			{
				if (pDoc->WorkingInfo.Probing[0].bUse)
				{
					if (sRst == _T("Open"))
					{
						if (pDoc->WorkingInfo.Probing[0].bStopOnOpen)
							pView->m_mgrProcedure->m_nMkStAuto = REJECT_ST;
						else
							pView->m_mgrProcedure->m_nMkStAuto++;
					}
					else if (sRst == _T("Error"))
					{
						pView->m_mgrProcedure->m_nMkStAuto = ERROR_ST;
					}
					else
					{
						pView->m_mgrProcedure->m_nMkStAuto++;
					}
				}
				else
					pView->m_mgrProcedure->m_nMkStAuto++;
			}
		   break;

		case MK_ST + (Mk2PtIdx::ChkElec) + 1:
			if (ChkLightErr())
			{
				pView->m_mgrProcedure->m_bChkLightErr = FALSE;
				pView->m_mgrProcedure->m_nMkStAuto++;
			}
			else
			{
				if (MODE_INNER != pDoc->GetTestMode())
					pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::DoMk); 	// Mk 마킹 시작
				else
					pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::Shift2Mk);
			}
			break;

		case MK_ST + (Mk2PtIdx::ChkElec) + 2:
			if (pView->m_mgrProcedure->IsRun())
			{
				if (m_pMotion->IsEnable(MS_X0) && m_pMotion->IsEnable(MS_Y0) &&
					m_pMotion->IsEnable(MS_X1) && m_pMotion->IsEnable(MS_Y1))
				{
					if (MODE_INNER != pDoc->GetTestMode())
						pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::DoMk); 	// Mk 마킹 시작
					else
						pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::Shift2Mk);
				}
				else
				{
					Stop();
					MsgBox(_T("마킹부 모션이 비활성화 되었습니다."));
					TowerLamp(RGB_RED, TRUE);
				}
			}
			else
			{
				if (!pView->m_mgrProcedure->m_bChkLightErr)
				{
					pView->m_mgrProcedure->m_bChkLightErr = TRUE;
					MsgBox(_T("노광불량 정지 - 기판을 확인하세요.\r\n계속진행하려면 운전스위치를 누르세요."));
				}
			}
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk2PtDoMarking()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	CString sRst, sMsg;
	int a, b, nSerial, nPrevSerial;
	double dTotLmt, dPrtLmt, dRatio;

	if (pView->m_mgrProcedure->m_bMkSt)
	{
		switch (pView->m_mgrProcedure->m_nMkStAuto)
		{
		case MK_ST + (Mk2PtIdx::DoMk) :				// Mk 마킹 시작
			if (pDoc->GetTestMode() == MODE_OUTER)
				SetMkIts(TRUE);						// ITS 마킹 시작
			else
				SetMk(TRUE);						// Mk 마킹 시작

			pView->m_mgrProcedure->m_nMkStAuto++;
			break;

		case MK_ST + (Mk2PtIdx::DoMk) + 1:
			if (!pView->m_mgrProcedure->m_bUpdateYield)
			{
				if (!pView->m_mgrProcedure->m_bTHREAD_UPDATAE_YIELD[0] && !pView->m_mgrProcedure->m_bTHREAD_UPDATAE_YIELD[1])
				{
					pView->m_mgrProcedure->m_bUpdateYield = TRUE;
					UpdateYield(); // Cam[0],  Cam[1]
					pView->m_mgrProcedure->m_nMkStAuto++;
				}
			}
			else
			{
				Sleep(100);
				pView->m_mgrProcedure->m_nMkStAuto++;
			}
			break;

		case MK_ST + (Mk2PtIdx::DoMk) + 2:
			if (!pView->m_mgrProcedure->m_bTHREAD_UPDATAE_YIELD[0] && !pView->m_mgrProcedure->m_bTHREAD_UPDATAE_YIELD[1])
			{
				if (pView->m_mgrProcedure->m_bUpdateYieldOnRmap)
				{
					if (!pView->m_mgrProcedure->m_bTHREAD_UPDATE_YIELD_UP && !pView->m_mgrProcedure->m_bTHREAD_UPDATE_YIELD_DN 
						&& !pView->m_mgrProcedure->m_bTHREAD_UPDATE_YIELD_ALLUP && !pView->m_mgrProcedure->m_bTHREAD_UPDATE_YIELD_ALLDN)
					{
						pView->m_mgrProcedure->m_bUpdateYieldOnRmap = TRUE;
						pView->m_mgrReelmap->UpdateYieldOnRmap(); // 20230614
						pView->m_mgrProcedure->m_nMkStAuto++;
					}
				}
				else
				{
					Sleep(100);
					pView->m_mgrProcedure->m_nMkStAuto++;
				}
			}
			break;

		case MK_ST + (Mk2PtIdx::Verify) :
			if (IsMkDone() && IsMoveDone())
			{
				if (IsVerify() && !pView->m_mgrProcedure->m_nPrevMkStAuto)
				{
					pView->m_mgrProcedure->m_nPrevMkStAuto = MK_ST + (Mk2PtIdx::Verify);
					pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::DoMk);		// Mk 마킹 시작
					pView->m_mgrProcedure->m_bCam = TRUE;
				}
				else
				{
					if (IsReview())
					{
						if (!pView->m_mgrProcedure->m_bCam)
						{
							pView->m_mgrProcedure->m_nPrevStepAuto = MK_ST + (Mk2PtIdx::Verify);
							pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::DoMk);		// Mk 마킹 시작
							pView->m_mgrProcedure->m_bCam = TRUE;
							MsgBox(_T("▶ Jog 버튼을 이용하여 마킹위치를 확인하여 주세요."));
						}
						else
						{
							pView->m_mgrProcedure->m_bCam = FALSE;
							pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::DoneMk);	// Mk 마킹 완료
						}
					}
					else
					{
						pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::DoneMk);	// Mk 마킹 완료
					}
				}
			}
			else if (IsReMk())
			{
				pView->m_mgrProcedure->m_nPrevMkStAuto = MK_ST + (Mk2PtIdx::Verify);
				pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::DoMk);		// Mk 재시작
			}
			else
			{
				sMsg = _T("");
				sMsg += pView->m_mgrProcedure->m_sDispSts[0];
				sMsg += _T(",");
				sMsg += pView->m_mgrProcedure->m_sDispSts[1];
			}
			break;

		case MK_ST + (Mk2PtIdx::DoneMk) :	 // Align변수 초기화
			if (!pView->m_mgrProcedure->IsRun()) 
				break;
			
			if (pView->m_mgrProcedure->m_bInitAuto)
			{
				pView->m_mgrProcedure->m_bInitAuto = FALSE;
				MsgBox(_T("마킹위치를 확인하세요."));
				Stop();
				TowerLamp(RGB_YELLOW, TRUE);
				break;
			}

			if (pView->m_mgrProcedure->m_nBufUpSerial[0] == 0)
			{
				pView->m_mgrProcedure->m_bSkipAlign[0][0] = TRUE;
				pView->m_mgrProcedure->m_bSkipAlign[0][1] = TRUE;
			}
			if (pView->m_mgrProcedure->m_nBufUpSerial[1] == 0)
			{
				pView->m_mgrProcedure->m_bSkipAlign[1][0] = TRUE;
				pView->m_mgrProcedure->m_bSkipAlign[1][1] = TRUE;
			}

			if( (!pView->m_mgrProcedure->m_bSkipAlign[0][0] && !pView->m_mgrProcedure->m_bSkipAlign[0][1]) 
				&& (!pView->m_mgrProcedure->m_bSkipAlign[1][0] && !pView->m_mgrProcedure->m_bSkipAlign[1][1]) )
				CompletedMk(2); // 0: Only Cam0, 1: Only Cam1, 2: Cam0 and Cam1, 3: None
			else if( (pView->m_mgrProcedure->m_bSkipAlign[0][0] || pView->m_mgrProcedure->m_bSkipAlign[0][1]) 
				&& (!pView->m_mgrProcedure->m_bSkipAlign[1][0] && !pView->m_mgrProcedure->m_bSkipAlign[1][1]) )
				CompletedMk(1); // 0: Only Cam0, 1: Only Cam1, 2: Cam0 and Cam1, 3: None
			else if( (!pView->m_mgrProcedure->m_bSkipAlign[0][0] && !pView->m_mgrProcedure->m_bSkipAlign[0][1]) 
				&& (pView->m_mgrProcedure->m_bSkipAlign[1][0] || pView->m_mgrProcedure->m_bSkipAlign[1][1]) )
				CompletedMk(0); // 0: Only Cam0, 1: Only Cam1, 2: Cam0 and Cam1, 3: None
			else
				CompletedMk(3); // 0: Only Cam0, 1: Only Cam1, 2: Cam0 and Cam1, 3: None

			pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk2PtIdx::DoneMk) + 1:  // Mk변수 초기화
			pView->m_mgrProcedure->m_bReAlign[0][0] = FALSE; // [nCam][nPos] 
			pView->m_mgrProcedure->m_bReAlign[0][1] = FALSE; // [nCam][nPos] 
			pView->m_mgrProcedure->m_bReAlign[1][0] = FALSE; // [nCam][nPos] 
			pView->m_mgrProcedure->m_bReAlign[1][1] = FALSE; // [nCam][nPos] 

			pView->m_mgrProcedure->m_bSkipAlign[0][0] = FALSE; // [nCam][nPos] 
			pView->m_mgrProcedure->m_bSkipAlign[0][1] = FALSE; // [nCam][nPos] 
			pView->m_mgrProcedure->m_bSkipAlign[1][0] = FALSE; // [nCam][nPos] 
			pView->m_mgrProcedure->m_bSkipAlign[1][1] = FALSE; // [nCam][nPos] 

			pView->m_mgrProcedure->m_bFailAlign[0][0] = FALSE; // [nCam][nPos] 
			pView->m_mgrProcedure->m_bFailAlign[0][1] = FALSE; // [nCam][nPos] 
			pView->m_mgrProcedure->m_bFailAlign[1][0] = FALSE; // [nCam][nPos] 
			pView->m_mgrProcedure->m_bFailAlign[1][1] = FALSE; // [nCam][nPos] 

			pView->m_mgrProcedure->m_bDoMk[0] = TRUE;
			pView->m_mgrProcedure->m_bDoMk[1] = TRUE;
			pView->m_mgrProcedure->m_bDoneMk[0] = FALSE;
			pView->m_mgrProcedure->m_bDoneMk[1] = FALSE;
			pView->m_mgrProcedure->m_bReMark[0] = FALSE;
			pView->m_mgrProcedure->m_bReMark[1] = FALSE;
			pView->m_mgrProcedure->m_bCam = FALSE;
			pView->m_mgrProcedure->m_nPrevMkStAuto = 0;

			pView->m_mgrProcedure->m_bUpdateYield = FALSE;
			pView->m_mgrProcedure->m_bUpdateYieldOnRmap = FALSE;

			for (a = 0; a < 2; a++)
			{
				for (b = 0; b < MAX_STRIP_NUM; b++)
				{
					pView->m_mgrProcedure->m_nMkStrip[a][b] = 0;
					pView->m_mgrProcedure->m_bRejectDone[a][b] = FALSE;
				}
			}

			pView->m_mgrProcedure->m_nSaveMk0Img = 0;
			pView->m_mgrProcedure->m_nSaveMk1Img = 0;

			m_pMpe->Write(_T("MB440150"), 0);	// 마킹부 마킹중 ON (PC가 ON, OFF)
			m_pMpe->Write(_T("MB440170"), 1);	// 마킹완료(PLC가 확인하고 Reset시킴.)-20141029
			if (IsNoMk() || IsShowLive())
				ShowLive(FALSE);

			pView->m_mgrProcedure->m_nMkStAuto++;
			break;

		case MK_ST + (Mk2PtIdx::DoneMk) + 2:
			if (pDoc->m_pMpeSignal[0] & (0x01 << 1))	// 마킹부 Feeding완료(PLC가 On시키고 PC가 확인하고 Reset시킴.)-20141030
			{
				pView->m_mgrProcedure->m_nMkStAuto++;
			}
			break;

		case MK_ST + (Mk2PtIdx::DoneMk) + 3:
#ifdef USE_MPE
			//if (pDoc->m_pMpeSignal[0] & (0x01 << 1))	// 마킹부 Feeding완료(PLC가 On시키고 PC가 확인하고 Reset시킴.)-20141030
			{
				if (!pView->m_mgrProcedure->m_bTHREAD_SHIFT2MK)
				{
					m_pMpe->Write(_T("MB440101"), 0);	// 마킹부 Feeding완료

					//Shift2Mk();			// PCR 이동(Buffer->Marked) // 기록(WorkingInfo.LastJob.sSerial)
					pView->m_mgrProcedure->m_bShift2Mk = TRUE;
					DoShift2Mk();

					SetMkFdLen();
					SetCycTime();
					pView->m_mgrProcedure->m_dwCycSt = GetTickCount();

					UpdateRst();

					//UpdateWorking();	// Update Working Info...
					pView->m_mgrProcedure->m_nMkStAuto++;
				}
			}
#endif
			break;
		case MK_ST + (Mk2PtIdx::DoneMk) + 4:
			//sMsg.Format(_T("%d%d%d%d%d"), m_bTHREAD_SHIFT2MK ? 1 : 0, m_bTHREAD_REELMAP_YIELD_UP ? 1 : 0, m_bTHREAD_REELMAP_YIELD_DN ? 1 : 0, m_bTHREAD_REELMAP_YIELD_ALLUP ? 1 : 0, m_bTHREAD_REELMAP_YIELD_ALLDN ? 1 : 0);
			//DispStsBar(sMsg, 0);

			if (!pView->m_mgrProcedure->m_bTHREAD_SHIFT2MK && !pView->m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_UP 
				&& !pView->m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_DN && !pView->m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_ALLUP 
				&& !pView->m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_ALLDN) // Yield Reelmap
			{
				if (pDoc->GetTestMode() == MODE_OUTER)
				{
					if (pView->m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_ITS) // Yield Reelmap
						break;
				}

				pView->m_mgrProcedure->m_nMkStAuto++;
				//UpdateRst();
				//UpdateWorking();	// Update Working Info...
				if (!pView->m_mgrReelmap->ChkYield())// (dTotLmt, dPrtLmt, dRatio))
				{
					if(dTotLmt == 0.0 && dPrtLmt == 0.0)
					{
						Stop();
						sMsg.Format(_T("일시정지 - Failed ChkYield()."));
						MsgBox(sMsg);
						TowerLamp(RGB_RED, TRUE);
						Buzzer(TRUE, 0);

					}
					else if (dPrtLmt == 0.0)
					{
						Stop();
						sMsg.Format(_T("일시정지 - 전체 수율 제한범위 : %.1f 미달 ( %.1f )"), dTotLmt, dRatio);
						MsgBox(sMsg);
						TowerLamp(RGB_RED, TRUE);
						Buzzer(TRUE, 0);
					}
					else
					{
						Stop();
						sMsg.Format(_T("일시정지 - 구간 수율 제한범위 : %.1f 미달 ( %.1f )"), dPrtLmt, dRatio);
						MsgBox(sMsg);
						TowerLamp(RGB_RED, TRUE);
						Buzzer(TRUE, 0);
					}
				}
			}
			break;
		case MK_ST + (Mk2PtIdx::DoneMk) + 5:
			if (!pView->m_mgrProcedure->m_bTHREAD_SHIFT2MK)
			{
				SetListBuf();
				ChkLotCutPos();
				UpdateWorking();	// Update Working Info...
				pView->m_mgrProcedure->m_nMkStAuto++;
			}
			break;
		case MK_ST + (Mk2PtIdx::DoneMk) + 6:
			pView->m_mgrProcedure->m_nMkStAuto++;
			if (!IsBuffer(0))
			{
				pView->m_mgrProcedure->m_bLotEnd = TRUE;
				pView->m_mgrProcedure->m_nLotEndAuto = LOT_END;
			}
			else
			{
				// [0]: AOI-Up , [1]: AOI-Dn
				if (bDualTest)
				{
					nSerial = pDoc->m_ListBuf[0].Pop(); // Up (좌)
					nSerial = pDoc->m_ListBuf[0].Pop(); // Up (우)

					SetListBuf();
				}

				if (pView->m_mgrProcedure->m_nLotEndSerial > 0)
				{
					nSerial = GetBufferUp(&nPrevSerial);

					if (pView->m_mgrProcedure->m_bSerialDecrese)
					{
						if (nSerial < pView->m_mgrProcedure->m_nLotEndSerial || nSerial <= 0) // 노광불량 3번째 Lot End ( -1, -1, -2)
						{
							if (IsDoneDispMkInfo())
							{
								if (IsBuffer() && nSerial > 0)
									SetSerial(nSerial);
								else
									SetSerial(nPrevSerial - 1, TRUE);

								if (IsBuffer() && nSerial > 0)
									SetSerial(nSerial - 1);
								else
									SetSerial(nPrevSerial - 2, TRUE);

								//m_nStepAuto = LOT_END;
								pView->m_mgrProcedure->m_bLotEnd = TRUE;
								pView->m_mgrProcedure->m_nLotEndAuto = LOT_END;
							}
						}
					}
					else
					{
						if (nSerial > pView->m_mgrProcedure->m_nLotEndSerial || nSerial <= 0) // 노광불량 3번째 Lot End ( -1, -1, -2)
						{
							if (IsDoneDispMkInfo())
							{
								if (IsBuffer() && nSerial > 0)
									SetSerial(nSerial);
								else
									SetSerial(nPrevSerial + 1, TRUE);

								if (IsBuffer() && nSerial > 0)
									SetSerial(nSerial + 1);
								else
									SetSerial(nPrevSerial + 2, TRUE);

								//m_nStepAuto = LOT_END;
								pView->m_mgrProcedure->m_bLotEnd = TRUE;
								pView->m_mgrProcedure->m_nLotEndAuto = LOT_END;
							}
						}
					}

					if (bDualTest)
					{
						nSerial = GetBufferDn(&nPrevSerial);

						if (pView->m_mgrProcedure->m_bSerialDecrese)
						{
							if (nSerial < pView->m_mgrProcedure->m_nLotEndSerial || nSerial <= 0) // 노광불량 3번째 Lot End ( -1, -1, -2)
							{
								if (IsDoneDispMkInfo())
								{
									if (IsBuffer() && nSerial > 0)
										SetSerial(nSerial);
									else
										SetSerial(nPrevSerial - 1, TRUE);

									if (IsBuffer() && nSerial > 0)
										SetSerial(nSerial - 1);
									else
										SetSerial(nPrevSerial - 2, TRUE);

									pView->m_mgrProcedure->m_bLotEnd = TRUE;
									pView->m_mgrProcedure->m_nLotEndAuto = LOT_END;
								}
							}
						}
						else
						{
							if (nSerial > pView->m_mgrProcedure->m_nLotEndSerial || nSerial <= 0) // 노광불량 3번째 Lot End ( -1, -1, -2)
							{
								if (IsDoneDispMkInfo())
								{
									if (IsBuffer() && nSerial > 0)
										SetSerial(nSerial);
									else
										SetSerial(nPrevSerial + 1, TRUE);

									if (IsBuffer() && nSerial > 0)
										SetSerial(nSerial + 1);
									else
										SetSerial(nPrevSerial + 2, TRUE);

									pView->m_mgrProcedure->m_bLotEnd = TRUE;
									pView->m_mgrProcedure->m_nLotEndAuto = LOT_END;
								}
							}
						}
					}
				}
			}

			break;
		case MK_ST + (Mk2PtIdx::DoneMk) + 7:
			pView->m_mgrProcedure->m_bMkSt = FALSE;
			::WritePrivateProfileString(_T("Last Job"), _T("MkSt"), _T("0"), PATH_WORKING_INFO);
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk2PtShift2Mk() // MODE_INNER
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	CString sRst, sMsg;
	int a, b, nSerial, nPrevSerial;

	if (pView->m_mgrProcedure->m_bMkSt)
	{
		switch (pView->m_mgrProcedure->m_nMkStAuto)
		{
		case MK_ST + (Mk2PtIdx::Shift2Mk) :
			m_pMpe->Write(_T("MB440150"), 0);	// 마킹부 마킹중 ON (PC가 ON, OFF)
			m_pMpe->Write(_T("MB440170"), 1);	// 마킹완료(PLC가 확인하고 Reset시킴.)-20141029
			pView->m_mgrProcedure->m_nMkStAuto++;
			break;

		case MK_ST + (Mk2PtIdx::Shift2Mk) + 1:
			if (pDoc->m_pMpeSignal[0] & (0x01 << 1))	// 마킹부 Feeding완료(PLC가 On시키고 PC가 확인하고 Reset시킴.)-20141030
			{
				pView->m_mgrProcedure->m_nMkStAuto++;
				//if (!m_bTHREAD_UPDATAE_YIELD[0] && !m_bTHREAD_UPDATAE_YIELD[1])
				//{
				//	UpdateYield(); // Cam[0],  Cam[1]
				//	m_nMkStAuto++;
				//}
			}
			break;

		case MK_ST + (Mk2PtIdx::Shift2Mk) + 2:
#ifdef USE_MPE
			//if (pDoc->m_pMpeSignal[0] & (0x01 << 1))	// 마킹부 Feeding완료(PLC가 On시키고 PC가 확인하고 Reset시킴.)-20141030
			{
				if (!pView->m_mgrProcedure->m_bTHREAD_SHIFT2MK)
				{
					m_pMpe->Write(_T("MB440101"), 0);	// 마킹부 Feeding완료
					pView->m_mgrProcedure->m_bShift2Mk = TRUE;
					DoShift2Mk();

					SetMkFdLen();
					SetCycTime();
					pView->m_mgrProcedure->m_dwCycSt = GetTickCount();

					pView->m_mgrProcedure->m_nMkStAuto++;
				}
			}
#endif
			break;
		case MK_ST + (Mk2PtIdx::Shift2Mk) + 3:
			if (!pView->m_mgrProcedure->m_bTHREAD_SHIFT2MK && !pView->m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_UP 
				&& !pView->m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_DN && !pView->m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_ALLUP 
				&& !pView->m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_ALLDN) // Yield Reelmap
			{
				if (pDoc->GetTestMode() == MODE_OUTER)
				{
					if (pView->m_mgrProcedure->m_bTHREAD_REELMAP_YIELD_ITS) // Yield Reelmap
						break;
				}

				pView->m_mgrProcedure->m_nMkStAuto++;
				UpdateRst();
				UpdateWorking();	// Update Working Info...
				pView->m_mgrReelmap->ChkYield();// (dTotLmt, dPrtLmt, dRatio);
			}
			break;
		case MK_ST + (Mk2PtIdx::Shift2Mk) + 4:
			if (!pView->m_mgrProcedure->m_bTHREAD_SHIFT2MK)
			{
				SetListBuf();
				ChkLotCutPos();
				pView->m_mgrProcedure->m_nMkStAuto++;
			}
			break;
		case MK_ST + (Mk2PtIdx::Shift2Mk) + 5:
			pView->m_mgrProcedure->m_nMkStAuto++;
			if (!IsBuffer(0))
			{
				pView->m_mgrProcedure->m_bLotEnd = TRUE;
				pView->m_mgrProcedure->m_nLotEndAuto = LOT_END;
			}
			else
			{
				// [0]: AOI-Up , [1]: AOI-Dn
				nSerial = pDoc->m_ListBuf[0].Pop(); // Up (좌)
				nSerial = pDoc->m_ListBuf[0].Pop(); // Up (우)

				SetListBuf();

				if (pView->m_mgrProcedure->m_bSerialDecrese)
				{
					if (pView->m_mgrProcedure->m_nLotEndSerial > 0)
					{
						nSerial = GetBufferUp(&nPrevSerial); // m_pBufSerial[0][0] : 버퍼 첫번째 시리얼

						if (nSerial < pView->m_mgrProcedure->m_nLotEndSerial || nSerial <= 0) // 노광불량 3번째 Lot End ( -1, -1, -2)
						{
							if (IsDoneDispMkInfo())
							{
								if (IsBuffer() && nSerial > 0)
									SetSerial(nSerial);
								//else
								//	SetSerial(nPrevSerial + 1, TRUE);

								nSerial = pView->m_mgrProcedure->m_pBufSerial[0][1]; // m_pBufSerial[0][1] : 버퍼 두번째 시리얼
								if (IsBuffer() && nSerial > 0)
									SetSerial(nSerial);	//SetSerial(nSerial + 1);
								//else
								//	SetSerial(nPrevSerial + 2, TRUE);

								pView->m_mgrProcedure->m_bLotEnd = TRUE;
								pView->m_mgrProcedure->m_nLotEndAuto = LOT_END;
							}
						}
					}
				}
				else
				{
					if (pView->m_mgrProcedure->m_nLotEndSerial > 0)
					{
						nSerial = GetBufferUp(&nPrevSerial); // m_pBufSerial[0][0] : 버퍼 첫번째 시리얼

						if (nSerial > pView->m_mgrProcedure->m_nLotEndSerial || nSerial <= 0) // 노광불량 3번째 Lot End ( -1, -1, -2)
						{
							if (IsDoneDispMkInfo())
							{
								if (IsBuffer() && nSerial > 0)
									SetSerial(nSerial);
								//else
								//	SetSerial(nPrevSerial + 1, TRUE);

								nSerial = pView->m_mgrProcedure->m_pBufSerial[0][1]; // m_pBufSerial[0][1] : 버퍼 두번째 시리얼
								if (IsBuffer() && nSerial > 0)
									SetSerial(nSerial);	//SetSerial(nSerial + 1);
								//else
								//	SetSerial(nPrevSerial + 2, TRUE);

								pView->m_mgrProcedure->m_bLotEnd = TRUE;
								pView->m_mgrProcedure->m_nLotEndAuto = LOT_END;
							}
						}
					}
				}
			}

			break;
		case MK_ST + (Mk2PtIdx::Shift2Mk) + 6:
			pView->m_mgrProcedure->m_bMkSt = FALSE;
			::WritePrivateProfileString(_T("Last Job"), _T("MkSt"), _T("0"), PATH_WORKING_INFO);
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk2PtLotDiff()
{
	if (pView->m_mgrProcedure->m_bMkSt)
	{
		switch (pView->m_mgrProcedure->m_nMkStAuto)
		{
		case MK_ST + (Mk2PtIdx::LotDiff) :
			Stop();
			TowerLamp(RGB_YELLOW, TRUE);
			pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk2PtIdx::LotDiff) + 1:
			//if(IDYES == DoMyMsgBox(_T("상면과 하면의 Lot가 다릅니다.\r\n계속 작업을 진행하시겠습니까?"), MB_YESNO))
			if (IDYES == MsgBox(_T("상면과 하면의 Lot가 다릅니다.\r\n계속 작업을 진행하시겠습니까?"), 0, MB_YESNO))
			{
				pView->m_mgrProcedure->m_bContDiffLot = TRUE;
			}
			else
			{
				pView->m_mgrProcedure->m_bContDiffLot = FALSE;
				pView->m_mgrProcedure->m_bLotEnd = TRUE;
				pView->m_mgrProcedure->m_nLotEndAuto = LOT_END;
			}
			pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk2PtIdx::LotDiff) + 2:
			if (pView->m_mgrProcedure->IsRun())
			{
				if (pView->m_mgrProcedure->m_bContDiffLot)
					pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::ChkSn);
				else
					pView->m_mgrProcedure->m_nMkStAuto++;
			}
			break;
		case MK_ST + (Mk2PtIdx::LotDiff) + 3:
			pView->m_mgrProcedure->m_bContDiffLot = FALSE;
			pView->m_mgrProcedure->m_bLotEnd = TRUE;
			pView->m_mgrProcedure->m_nLotEndAuto = LOT_END;
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk2PtReject()
{
	int a, b;

	if (pView->m_mgrProcedure->m_bMkSt)
	{
		switch (pView->m_mgrProcedure->m_nMkStAuto)
		{
		case REJECT_ST:
			Stop();
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);

			if (IDYES == MsgBox(_T("쇼트 체크 불량입니다.\r\n리젝 처리를 진행하시겠습니까?"), 0, MB_YESNO))
			{
				pView->m_mgrProcedure->m_bAnswer[0] = TRUE;
				pView->m_mgrProcedure->m_nMkStAuto++;
			}
			else
			{
				if (IDYES == MsgBox(_T("리젝 처리를 취소하였습니다.\r\n불량만 마킹 처리를 진행하시겠습니까?"), 0, MB_YESNO))
				{
					pView->m_mgrProcedure->m_bAnswer[1] = TRUE;
					pView->m_mgrProcedure->m_nMkStAuto++;
				}
			}

			Buzzer(FALSE, 0);
			break;
		case REJECT_ST + 1:
			if (pView->m_mgrProcedure->IsRun())
			{
				if (pView->m_mgrProcedure->m_bAnswer[0])
				{
					pView->m_mgrProcedure->m_bAnswer[0] = FALSE;
					pView->m_mgrProcedure->m_nMkStAuto++;
				}
				else if (pView->m_mgrProcedure->m_bAnswer[1])
				{
					pView->m_mgrProcedure->m_bAnswer[1] = FALSE;
					pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::DoMk);	// Mk 마킹 시작
				}
				else
					pView->m_mgrProcedure->m_nMkStAuto = REJECT_ST;
			}
			break;
		case REJECT_ST + 2:
			SetReject();
			pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case REJECT_ST + 3:
			pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case REJECT_ST + 4:
			if (IsMkDone() && IsMoveDone())
			{
				if (IsVerify() && !pView->m_mgrProcedure->m_nPrevMkStAuto)
				{
					pView->m_mgrProcedure->m_nPrevMkStAuto = REJECT_ST + 4;
					pView->m_mgrProcedure->m_nMkStAuto = REJECT_ST + 2;		// Mk 마킹 시작
					pView->m_mgrProcedure->m_bCam = TRUE;

					pView->m_mgrProcedure->m_bDoneMk[0] = FALSE;
					pView->m_mgrProcedure->m_bDoneMk[1] = FALSE;

					for (a = 0; a < 2; a++)
					{
						for (b = 0; b < MAX_STRIP_NUM; b++)
						{
							pView->m_mgrProcedure->m_nMkStrip[a][b] = 0;
							pView->m_mgrProcedure->m_bRejectDone[a][b] = FALSE;
						}
					}
				}
				else
				{
					if (IsReview())
					{
						if (!pView->m_mgrProcedure->m_bCam)
						{
							pView->m_mgrProcedure->m_nPrevStepAuto = REJECT_ST + 4;
							pView->m_mgrProcedure->m_nMkStAuto = REJECT_ST + 2;		// Mk 마킹 시작
							pView->m_mgrProcedure->m_bCam = TRUE;
							MsgBox(_T("▶ Jog 버튼을 이용하여 마킹위치를 확인하여 주세요."));
						}
						else
						{
							pView->m_mgrProcedure->m_bCam = FALSE;
							pView->m_mgrProcedure->m_nMkStAuto++;	// Mk 마킹 완료
						}
					}
					else
					{
						pView->m_mgrProcedure->m_nMkStAuto++;	// Mk 마킹 완료
					}
				}
			}
			break;
		case REJECT_ST + 5:
			pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::DoneMk);				// Align변수 초기화
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk2PtErrStop()
{
	if (pView->m_mgrProcedure->m_bMkSt)
	{
		switch (pView->m_mgrProcedure->m_nMkStAuto)
		{
		case ERROR_ST:
			Stop();
			TowerLamp(RGB_RED, TRUE);
			if (IDYES == MsgBox(_T("쇼트 체크 Error입니다.\r\n다시 쇼트 체크를 진행하시겠습니까?"), 0, MB_YESNO))
			{
				pView->m_mgrProcedure->m_bAnswer[0] = TRUE;
				pView->m_mgrProcedure->m_nMkStAuto++;
			}
			else
			{
				if (IDYES == MsgBox(_T("쇼트 체크를 취소하였습니다.\r\n불량만 마킹 처리를 진행하시겠습니까?"), 0, MB_YESNO))
				{
					pView->m_mgrProcedure->m_bAnswer[1] = TRUE;
					pView->m_mgrProcedure->m_nMkStAuto++;
				}
				else
					pView->m_mgrProcedure->m_nMkStAuto++;
			}
			break;
		case ERROR_ST + 1:
			pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case ERROR_ST + 2:
			if (pView->m_mgrProcedure->IsRun())
			{
				if (pView->m_mgrProcedure->m_bAnswer[0])
				{
					pView->m_mgrProcedure->m_bAnswer[0] = FALSE;
					pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::ChkElec); // DoElecChk
				}
				else if (pView->m_mgrProcedure->m_bAnswer[1])
				{
					pView->m_mgrProcedure->m_bAnswer[1] = FALSE;
					pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk2PtIdx::DoMk);	// Mk 마킹 시작
				}
				else
					pView->m_mgrProcedure->m_nMkStAuto++;
			}
			break;
		case ERROR_ST + 3:
			pView->m_mgrProcedure->m_nMkStAuto = ERROR_ST;
			break;
		}
	}
}

void CGvisR2R_PunchView::MarkingWith2PointAlign()
{
	Mk2PtReady();
	Mk2PtChkSerial();
	Mk2PtInit();
	Mk2PtAlignPt0();
	Mk2PtAlignPt1();
	Mk2PtMoveInitPos();
	Mk2PtElecChk();
	if (MODE_INNER != pDoc->GetTestMode())
		Mk2PtDoMarking();
	else
		Mk2PtShift2Mk();
	Mk2PtLotDiff();
	Mk2PtReject();
	Mk2PtErrStop();
}

void CGvisR2R_PunchView::Mk4PtReady()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pView->m_mgrProcedure->m_bMkSt)
	{
		switch (pView->m_mgrProcedure->m_nMkStAuto)
		{
		case MK_ST:	// PLC MK 신호 확인	
			if (pView->m_mgrProcedure->IsRun())
			{
				SetListBuf();
				pView->m_mgrProcedure->m_nMkStAuto++;
			}
			break;
		case MK_ST + 1:
			m_pMpe->Write(_T("MB440150"), 1);// 마킹부 마킹중 ON (PC가 ON, OFF)
			pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Start) :	// 2
			if (bDualTest)
			{
				if (pDoc->m_ListBuf[1].nTot > 0) // AOI-Dn
				{
					pView->m_mgrProcedure->m_nMkStAuto++;

					pView->m_mgrProcedure->m_nBufDnSerial[0] = pDoc->m_ListBuf[1].Pop();
					pView->m_mgrProcedure->m_nBufUpSerial[0] = pView->m_mgrProcedure->m_nBufDnSerial[0];
					if (pDoc->m_ListBuf[1].nTot > 0) // AOI-Dn
					{
						pView->m_mgrProcedure->m_nBufDnSerial[1] = pDoc->m_ListBuf[1].Pop();
						pView->m_mgrProcedure->m_nBufUpSerial[1] = pView->m_mgrProcedure->m_nBufDnSerial[1];
					}
					else
					{
						pView->m_mgrProcedure->m_nBufDnSerial[1] = 0;
						pView->m_mgrProcedure->m_nBufUpSerial[1] = 0;
					}
				}
				else
				{
					pView->m_mgrProcedure->m_bLotEnd = TRUE;
					pView->m_mgrProcedure->m_nLotEndAuto = LOT_END;
				}

				if (pDoc->WorkingInfo.LastJob.bSampleTest)
				{
					if (pView->m_mgrProcedure->m_nBufUpSerial[0] == 1)
					{
						pView->m_mgrProcedure->m_nLotEndSerial = _tstoi(pDoc->WorkingInfo.LastJob.sSampleTestShotNum);
						pView->m_mgrProcedure->m_bLastProcFromUp = FALSE;
						pView->m_mgrProcedure->m_bLastProcFromEng = FALSE;
						pView->m_mgrProcedure->m_bLastProc = TRUE;
						if (m_pDlgMenu01)
							m_pDlgMenu01->m_bLastProc = TRUE;
						if (m_pMpe)
						{
							m_pMpe->Write(_T("MB440186"), 1);			// 잔량처리 AOI(하) 부터(PC가 On시키고, PLC가 확인하고 Off시킴)-20141112
							m_pMpe->Write(_T("MB440181"), 1);			// 잔량처리(PC가 On시키고, PLC가 확인하고 Off시킴)-20141031
						}
					}
				}
			}
			else
			{
				if (pDoc->m_ListBuf[0].nTot > 0) // AOI-Up
				{
					pView->m_mgrProcedure->m_nMkStAuto++;
					pView->m_mgrProcedure->m_nBufUpSerial[0] = pDoc->m_ListBuf[0].Pop();
					if (pDoc->m_ListBuf[0].nTot > 0) // AOI-Up
						pView->m_mgrProcedure->m_nBufUpSerial[1] = pDoc->m_ListBuf[0].Pop();
					else
						pView->m_mgrProcedure->m_nBufUpSerial[1] = 0;
				}
				else
				{
					pView->m_mgrProcedure->m_bLotEnd = TRUE;
					pView->m_mgrProcedure->m_nLotEndAuto = LOT_END;
				}

				if (pDoc->WorkingInfo.LastJob.bSampleTest)
				{
					if (pView->m_mgrProcedure->m_nBufUpSerial[0] == 1)
					{
						pView->m_mgrProcedure->m_nLotEndSerial = _tstoi(pDoc->WorkingInfo.LastJob.sSampleTestShotNum);
						pView->m_mgrProcedure->m_bLastProcFromUp = FALSE;
						pView->m_mgrProcedure->m_bLastProcFromEng = FALSE;
						pView->m_mgrProcedure->m_bLastProc = TRUE;
						if (m_pDlgMenu01)
							m_pDlgMenu01->m_bLastProc = TRUE;
						if (m_pMpe)
						{
							m_pMpe->Write(_T("MB440186"), 1);			// 잔량처리 AOI(하) 부터(PC가 On시키고, PLC가 확인하고 Off시킴)-20141112
							m_pMpe->Write(_T("MB440181"), 1);			// 잔량처리(PC가 On시키고, PLC가 확인하고 Off시킴)-20141031
						}
					}
				}
			}
			break;
		case MK_ST + (Mk4PtIdx::Start) + 1:
			pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk4PtChkSerial()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	CString sNewLot;
	BOOL bNewModel = FALSE;
	int nSerial = 0;
	int nNewLot = 0;
	double dFdEnc;

	if (pView->m_mgrProcedure->m_bMkSt)
	{
		switch (pView->m_mgrProcedure->m_nMkStAuto)
		{
		case MK_ST + (Mk4PtIdx::ChkSn) :
			if (pView->m_mgrProcedure->m_nBufUpSerial[0] == pView->m_mgrProcedure->m_nBufUpSerial[1])
			{
				Stop();
				pView->ClrDispMsg();
				AfxMessageBox(_T("좌/우 마킹 시리얼이 같습니다."));
				SetListBuf();
				pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::Start);
				break;
			}

			if (!pView->m_mgrProcedure->m_bTHREAD_DISP_DEF)
			{
				pView->m_mgrProcedure->m_nMkStAuto++;
				pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF = 0;
				pView->m_mgrProcedure->m_bTHREAD_DISP_DEF = TRUE;		// DispDefImg() : CopyDefImg Start
				pDoc->SetMkMenu01(_T("Signal"), _T("DispDefImg"), _T("1"));
			}
			break;

		case MK_ST + (Mk4PtIdx::ChkSn) + 1:
			pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::InitMk);			// InitMk()
			nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0];
			sNewLot = pView->m_mgrProcedure->m_sNewLotUp;

			if (nSerial > 0)
			{
				if (pView->m_mgrProcedure->m_bSerialDecrese)
				{
					if (pView->m_mgrProcedure->m_bLastProc && pView->m_mgrProcedure->m_nBufUpSerial[1] < pView->m_mgrProcedure->m_nLotEndSerial)
					{
						nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Test
					}
				}
				else
				{
					if (pView->m_mgrProcedure->m_bLastProc && pView->m_mgrProcedure->m_nBufUpSerial[1] > pView->m_mgrProcedure->m_nLotEndSerial)
					{
						nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Test
					}
				}

				bNewModel = GetAoiUpInfo(nSerial, &nNewLot, TRUE);
				if (bDualTest)
				{
					bNewModel = GetAoiDnInfo(nSerial, &nNewLot, TRUE);

					if (!IsSameUpDnLot() && !pView->m_mgrProcedure->m_bContDiffLot)
					{
						//sNewLot.Empty();
						pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::LotDiff);
						break;
					}
				}

				if (bNewModel)	// AOI 정보(AoiCurrentInfoPath) -> AOI Feeding Offset
				{
					;
				}
				if (nNewLot)
				{
					// Lot Change.
					ChgLot();
#ifdef USE_MPE
					dFdEnc = (double)pDoc->m_pMpeData[0][0];	// 마킹부 Feeding 엔코더 값(단위 mm )
					if ((pDoc->WorkingInfo.LastJob.bLotSep || pDoc->m_bDoneChgLot) && (dFdEnc + _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen)*2.0) > _tstof(pDoc->WorkingInfo.LastJob.sLotSepLen)*1000.0)
					{
						pDoc->m_bDoneChgLot = TRUE;
						pView->m_mgrReelmap->SetPathAtBuf();
					}
#endif
				}
			}
			else
			{
				Stop();
				MsgBox(_T("버퍼(좌) Serial이 맞지않습니다."));
				TowerLamp(RGB_YELLOW, TRUE);
			}
			//sNewLot.Empty();
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk4PtInit()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pView->m_mgrProcedure->m_bMkSt)
	{
		switch (pView->m_mgrProcedure->m_nMkStAuto)
		{
		case MK_ST + (Mk4PtIdx::InitMk) :
			if (InitMk())	// 시리얼이 증가 또는 감소 : TRUE 아니고 같으면 : FALSE
			{
				if (pView->m_mgrProcedure->m_bSerialDecrese)
				{
					if ((pView->m_mgrProcedure->m_nBufUpSerial[0] <= pView->m_mgrProcedure->m_nLotEndSerial 
						|| pView->m_mgrProcedure->m_nBufUpSerial[1] <= pView->m_mgrProcedure->m_nLotEndSerial) 
						&& pView->m_mgrProcedure->m_nLotEndSerial > 0)
						m_pMpe->Write(_T("MB440171"), 1); // 마킹부 작업완료.(PC가 On, PLC가 확인 후 Off) - 20160718
				}
				else
				{
					if ((pView->m_mgrProcedure->m_nBufUpSerial[0] >= pView->m_mgrProcedure->m_nLotEndSerial 
						|| pView->m_mgrProcedure->m_nBufUpSerial[1] >= pView->m_mgrProcedure->m_nLotEndSerial) 
						&& pView->m_mgrProcedure->m_nLotEndSerial > 0)
						m_pMpe->Write(_T("MB440171"), 1); // 마킹부 작업완료.(PC가 On, PLC가 확인 후 Off) - 20160718
				}
			}
			else
			{
				Stop();
				MsgBox(_T("Serial 연속 되지않습니다."));
				TowerLamp(RGB_YELLOW, TRUE);
			}
			pView->m_mgrProcedure->m_nMkStAuto++;
			break;

		case MK_ST + (Mk4PtIdx::InitMk) + 1:
			if (pView->m_mgrProcedure->IsRun())
				pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk4PtAlignPt0()
{
	if (!pView->m_mgrProcedure->IsRun())
		return;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pView->m_mgrProcedure->m_bMkSt)
	{
		switch (pView->m_mgrProcedure->m_nMkStAuto)
		{
		case MK_ST + (Mk4PtIdx::Move0Cam1) :	// Move - Cam1 - Pt0
			if (bDualTest)
			{
				if (pView->m_mgrProcedure->m_bSerialDecrese)
				{
					if (pView->m_mgrProcedure->m_bLastProc && pView->m_mgrProcedure->m_nBufDnSerial[0] - 1 < pView->m_mgrProcedure->m_nLotEndSerial)	// AOI하면 Serial
					{
						pView->m_mgrProcedure->m_bSkipAlign[1][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
						pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						pView->m_mgrProcedure->m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(0))
							pView->m_mgrProcedure->m_nMkStAuto++;
					}
				}
				else
				{
					if (pView->m_mgrProcedure->m_bLastProc && pView->m_mgrProcedure->m_nBufDnSerial[0] + 1 > pView->m_mgrProcedure->m_nLotEndSerial)	// AOI하면 Serial
					{
						pView->m_mgrProcedure->m_bSkipAlign[1][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
						pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						pView->m_mgrProcedure->m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(0))
							pView->m_mgrProcedure->m_nMkStAuto++;
					}
				}
			}
			else
			{
				if (pView->m_mgrProcedure->m_bSerialDecrese)
				{
					if (pView->m_mgrProcedure->m_bLastProc && pView->m_mgrProcedure->m_nBufUpSerial[0] - 1 < pView->m_mgrProcedure->m_nLotEndSerial)	// AOI상면 Serial
					{
						pView->m_mgrProcedure->m_bSkipAlign[1][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
						pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						pView->m_mgrProcedure->m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(0))
							pView->m_mgrProcedure->m_nMkStAuto++;
					}
				}
				else
				{
					if (pView->m_mgrProcedure->m_bLastProc && pView->m_mgrProcedure->m_nBufUpSerial[0] + 1 > pView->m_mgrProcedure->m_nLotEndSerial)	// AOI상면 Serial
					{
						pView->m_mgrProcedure->m_bSkipAlign[1][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
						pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						pView->m_mgrProcedure->m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(0))
							pView->m_mgrProcedure->m_nMkStAuto++;
					}
				}
			}
			break;
		case MK_ST + (Mk4PtIdx::Move0Cam1) + 1:
			if (pView->m_mgrProcedure->IsRun())
				pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Move0Cam0) :	// Move - Cam0 - Pt0
			if (MoveAlign0(0))
				pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Move0Cam0) + 1:
			if (pView->m_mgrProcedure->IsRun())
				pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Move0Cam0) + 2:
			if (IsMoveDone())
			{
				Sleep(100);
				pView->m_mgrProcedure->m_nMkStAuto++;
			}
			break;
		case MK_ST + (Mk4PtIdx::Align1_0) :	// 4PtAlign - Cam1 - Pt0
			if (!pView->m_mgrProcedure->m_bSkipAlign[1][0])
			{
				if (FourPointAlign1(0))
					pView->m_mgrProcedure->m_bFailAlign[1][0] = FALSE;
				else
					pView->m_mgrProcedure->m_bFailAlign[1][0] = TRUE;
			}
											pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Align0_0) :	// 4PtAlign - Cam0 - Pt0
			if (!pView->m_mgrProcedure->m_bSkipAlign[0][0])
			{
				if (FourPointAlign0(0))
					pView->m_mgrProcedure->m_bFailAlign[0][0] = FALSE;
				else
					pView->m_mgrProcedure->m_bFailAlign[0][0] = TRUE;
			}
											pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Align0_0) + 1:
			if (pView->m_mgrProcedure->m_bFailAlign[0][0])
			{
				Buzzer(TRUE, 0);

				if (IDNO == MsgBox(_T("카메라(좌)의 검사판넬을 다시 정렬하시겠습니까?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);

					if (IDYES == MsgBox(_T("카메라(좌)의 검사판넬 정렬을 정말 않하시겠습니까?"), 0, MB_YESNO))
					{
						pView->m_mgrProcedure->m_bReAlign[0][0] = FALSE;
						pView->m_mgrProcedure->m_bSkipAlign[0][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[0][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[0][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[0][3] = TRUE;

						if (IDNO == MsgBox(_T("카메라(좌)의 검사판넬을 불량마킹 하시겠습니까?"), 0, MB_YESNO))
						{
							pView->m_mgrProcedure->m_bDoMk[0] = FALSE;
							pView->m_mgrProcedure->m_bDoneMk[0] = TRUE;
						}
						else
						{
							pView->m_mgrProcedure->m_bDoMk[0] = TRUE;
							pView->m_mgrProcedure->m_bDoneMk[0] = FALSE;
						}
					}
					else
					{
						pView->m_mgrProcedure->m_bReAlign[0][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[0][0] = FALSE;
						pView->m_mgrProcedure->m_bSkipAlign[0][1] = FALSE;
						pView->m_mgrProcedure->m_bSkipAlign[0][2] = FALSE;
						pView->m_mgrProcedure->m_bSkipAlign[0][3] = FALSE;

						pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::Move0Cam0); // FourPointAlign0(0) 으로 진행. - 카메라 재정렬
						Stop();
						TowerLamp(RGB_YELLOW, TRUE);
					}
				}
				else
				{
					Buzzer(FALSE, 0);

					pView->m_mgrProcedure->m_bReAlign[0][0] = TRUE;
					pView->m_mgrProcedure->m_bSkipAlign[0][0] = FALSE;
					pView->m_mgrProcedure->m_bSkipAlign[0][1] = FALSE;

					pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::Move0Cam0); // FourPointAlign0(0) 으로 진행. - 카메라 재정렬
					Stop();
					TowerLamp(RGB_YELLOW, TRUE);
				}
			}
			if (pView->m_mgrProcedure->m_bFailAlign[1][0])
			{
				Buzzer(TRUE, 0);

				if (IDNO == MsgBox(_T("카메라(우)의 검사판넬을 다시 정렬하시겠습니까?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);

					if (IDYES == MsgBox(_T("카메라(우)의 검사판넬 정렬을 정말 않하시겠습니까?"), 0, MB_YESNO))
					{
						pView->m_mgrProcedure->m_bReAlign[1][0] = FALSE;
						pView->m_mgrProcedure->m_bSkipAlign[1][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = TRUE;

						if (IDNO == MsgBox(_T("카메라(우)의 검사판넬을 불량마킹 하시겠습니까?"), 0, MB_YESNO))
						{
							pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
							pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						}
						else
						{
							pView->m_mgrProcedure->m_bDoMk[1] = TRUE;
							pView->m_mgrProcedure->m_bDoneMk[1] = FALSE;
						}
					}
					else
					{
						pView->m_mgrProcedure->m_bReAlign[1][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][0] = FALSE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = FALSE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = FALSE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = FALSE;

						pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::Move0Cam1); // FourPointAlign1(0) 으로 진행. - 카메라 재정렬
						Stop();
						TowerLamp(RGB_YELLOW, TRUE);
					}
				}
				else
				{
					Buzzer(FALSE, 0);

					pView->m_mgrProcedure->m_bReAlign[1][0] = TRUE;
					pView->m_mgrProcedure->m_bSkipAlign[1][0] = FALSE;
					pView->m_mgrProcedure->m_bSkipAlign[1][1] = FALSE;
					pView->m_mgrProcedure->m_bSkipAlign[1][2] = FALSE;
					pView->m_mgrProcedure->m_bSkipAlign[1][3] = FALSE;

					pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::Move0Cam1); // FourPointAlign1(0) 으로 진행. - 카메라 재정렬
					Stop();
					TowerLamp(RGB_YELLOW, TRUE);
				}
			}

			if (pView->m_mgrProcedure->m_bFailAlign[0][0] || pView->m_mgrProcedure->m_bFailAlign[1][0])
			{
				if (!pView->m_mgrProcedure->m_bReAlign[0][0] && !pView->m_mgrProcedure->m_bReAlign[1][0])
				{
					if (pView->m_mgrProcedure->m_bDoMk[0] || pView->m_mgrProcedure->m_bDoMk[1])
						pView->m_mgrProcedure->m_nMkStAuto++; //m_nMkStAuto = MK_ST + 27; // MoveInitPos0()
					else
					{
						if (!IsInitPos0())
							MoveInitPos0();
						if (!IsInitPos1())
							MoveInitPos1();

						pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::DoneMk); // // 불량이미지 Display, Align변수 초기화 (Skip 65 : Mk())
					}
				}
				else
				{
					//m_nMkStAuto = MK_ST + (Mk4PtIdx::Align1_0); // TwoPointAlign1(0) 으로 진행. - 카메라 재정렬
					pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::Move0Cam1); // TwoPointAlign1(0) 으로 진행. - 카메라 재정렬
				}
			}
			else
				pView->m_mgrProcedure->m_nMkStAuto++;

			break;
		case MK_ST + (Mk4PtIdx::Align0_0) + 2:
			if (pView->m_mgrProcedure->IsRun())
				pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk4PtAlignPt1()
{
	if (!pView->m_mgrProcedure->IsRun())
		return;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pView->m_mgrProcedure->m_bMkSt)
	{
		switch (pView->m_mgrProcedure->m_nMkStAuto)
		{
		case MK_ST + (Mk4PtIdx::Move1Cam1) :
			if (bDualTest)
			{
				if (pView->m_mgrProcedure->m_bSerialDecrese)
				{
					if (pView->m_mgrProcedure->m_bLastProc && pView->m_mgrProcedure->m_nBufDnSerial[0] - 1 < pView->m_mgrProcedure->m_nLotEndSerial)	// AOI하면 Serial
					{
						pView->m_mgrProcedure->m_bSkipAlign[1][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
						pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						pView->m_mgrProcedure->m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(1))	// Move - Cam1 - Pt1
							pView->m_mgrProcedure->m_nMkStAuto++;
					}
				}
				else
				{
					if (pView->m_mgrProcedure->m_bLastProc && pView->m_mgrProcedure->m_nBufDnSerial[0] + 1 > pView->m_mgrProcedure->m_nLotEndSerial)	// AOI하면 Serial
					{
						pView->m_mgrProcedure->m_bSkipAlign[1][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
						pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						pView->m_mgrProcedure->m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(1))	// Move - Cam1 - Pt1
							pView->m_mgrProcedure->m_nMkStAuto++;
					}
				}
			}
			else
			{
				if (pView->m_mgrProcedure->m_bSerialDecrese)
				{
					if (pView->m_mgrProcedure->m_bLastProc && pView->m_mgrProcedure->m_nBufUpSerial[0] - 1 < pView->m_mgrProcedure->m_nLotEndSerial)	// AOI상면 Serial
					{
						pView->m_mgrProcedure->m_bSkipAlign[1][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
						pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						pView->m_mgrProcedure->m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(1))	// Move - Cam1 - Pt1
							pView->m_mgrProcedure->m_nMkStAuto++;
					}
				}
				else
				{
					if (pView->m_mgrProcedure->m_bLastProc && pView->m_mgrProcedure->m_nBufUpSerial[0] + 1 > pView->m_mgrProcedure->m_nLotEndSerial)	// AOI상면 Serial
					{
						pView->m_mgrProcedure->m_bSkipAlign[1][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
						pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						pView->m_mgrProcedure->m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(1))	// Move - Cam1 - Pt1
							pView->m_mgrProcedure->m_nMkStAuto++;
					}
				}
			}
										   break;
		case MK_ST + (Mk4PtIdx::Move1Cam1) + 1:
			if (pView->m_mgrProcedure->IsRun())
				pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Move1Cam0) :
			if (MoveAlign0(1))	// Move - Cam0 - Pt1
				pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Move1Cam0) + 1:
			if (pView->m_mgrProcedure->IsRun())
				pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Move1Cam0) + 2:
			if (IsMoveDone())
			{ 
				Sleep(100);
				pView->m_mgrProcedure->m_nMkStAuto++;
			}
			break;
		case MK_ST + (Mk4PtIdx::Align1_1) :	// 4PtAlign - Cam1 - Pt1
			if (!pView->m_mgrProcedure->m_bFailAlign[1][0])
			{
				if (!pView->m_mgrProcedure->m_bSkipAlign[1][1])
				{
					if (!FourPointAlign1(1))
						pView->m_mgrProcedure->m_bFailAlign[1][1] = TRUE;
					else
						pView->m_mgrProcedure->m_bFailAlign[1][1] = FALSE;
				}
				else
					pView->m_mgrProcedure->m_bFailAlign[1][1] = FALSE;
			}
			else
				pView->m_mgrProcedure->m_bFailAlign[1][1] = FALSE;

			pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Align0_1) :	// 4PtAlign - Cam0 - Pt1
			if (!pView->m_mgrProcedure->m_bFailAlign[0][0])
			{
				if (!pView->m_mgrProcedure->m_bSkipAlign[0][1])
				{
					if (!FourPointAlign0(1))
						pView->m_mgrProcedure->m_bFailAlign[0][1] = TRUE;
					else
						pView->m_mgrProcedure->m_bFailAlign[0][1] = FALSE;
				}
				else
					pView->m_mgrProcedure->m_bFailAlign[0][1] = FALSE;
			}
			else
				pView->m_mgrProcedure->m_bFailAlign[0][1] = FALSE;

			pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Align0_1) + 1:
			if (pView->m_mgrProcedure->m_bFailAlign[0][1])
			{
				Buzzer(TRUE, 0);

				if (IDNO == MsgBox(_T("카메라(좌)의 검사판넬을 다시 정렬하시겠습니까?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);

					if (IDYES == MsgBox(_T("카메라(좌)의 검사판넬 정렬을 정말 않하시겠습니까?"), 0, MB_YESNO))
					{
						pView->m_mgrProcedure->m_bReAlign[0][1] = FALSE;
						pView->m_mgrProcedure->m_bSkipAlign[0][1] = TRUE;

						if (IDNO == MsgBox(_T("카메라(좌)의 검사판넬을 불량마킹 하시겠습니까?"), 0, MB_YESNO))
						{
							pView->m_mgrProcedure->m_bDoMk[0] = FALSE;
							pView->m_mgrProcedure->m_bDoneMk[0] = TRUE;
						}
						else
						{
							pView->m_mgrProcedure->m_bDoMk[0] = TRUE;
							pView->m_mgrProcedure->m_bDoneMk[0] = FALSE;
						}
					}
					else
					{
						pView->m_mgrProcedure->m_bReAlign[0][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[0][1] = FALSE;

						pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::Move1Cam0); // FourPointAlign0(1) 으로 진행. - 카메라 재정렬
						Stop();
						TowerLamp(RGB_YELLOW, TRUE);
					}
				}
				else
				{
					Buzzer(FALSE, 0);

					pView->m_mgrProcedure->m_bReAlign[0][1] = TRUE;
					pView->m_mgrProcedure->m_bSkipAlign[0][1] = FALSE;

					pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::Move1Cam0); // FourPointAlign1(1) 으로 진행. - 카메라 재정렬
					Stop();
					TowerLamp(RGB_YELLOW, TRUE);
				}
			}
			if (pView->m_mgrProcedure->m_bFailAlign[1][1])
			{
				Buzzer(TRUE, 0);

				if (IDNO == MsgBox(_T("카메라(우)의 검사판넬을 다시 정렬하시겠습니까?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);

					if (IDYES == MsgBox(_T("카메라(우)의 검사판넬 정렬을 정말 않하시겠습니까?"), 0, MB_YESNO))
					{
						pView->m_mgrProcedure->m_bReAlign[1][1] = FALSE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = TRUE;

						if (IDNO == MsgBox(_T("카메라(우)의 검사판넬을 불량마킹 하시겠습니까?"), 0, MB_YESNO))
						{
							pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
							pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						}
						else
						{
							pView->m_mgrProcedure->m_bDoMk[1] = TRUE;
							pView->m_mgrProcedure->m_bDoneMk[1] = FALSE;
						}
					}
					else
					{
						pView->m_mgrProcedure->m_bReAlign[1][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = FALSE;

						pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::Move1Cam1); // FourPointAlign1(1) 으로 진행. - 카메라 재정렬
						Stop();
						TowerLamp(RGB_YELLOW, TRUE);
					}
				}
				else
				{
					Buzzer(FALSE, 0);

					pView->m_mgrProcedure->m_bReAlign[1][1] = TRUE;
					pView->m_mgrProcedure->m_bSkipAlign[1][1] = FALSE;

					pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::Move1Cam1); // FourPointAlign1(1) 으로 진행. - 카메라 재정렬
					Stop();
					TowerLamp(RGB_YELLOW, TRUE);
				}
			}

			if (pView->m_mgrProcedure->m_bFailAlign[0][1] || pView->m_mgrProcedure->m_bFailAlign[1][1])
			{
				if (!pView->m_mgrProcedure->m_bReAlign[0][1] && !pView->m_mgrProcedure->m_bReAlign[1][1])
				{
					if (pView->m_mgrProcedure->m_bDoMk[0] || pView->m_mgrProcedure->m_bDoMk[1])
						pView->m_mgrProcedure->m_nMkStAuto++;//m_nMkStAuto = MK_ST + 29;  // MoveInitPos0()
					else
					{
						if (!IsInitPos0())
							MoveInitPos0();
						if (!IsInitPos1())
							MoveInitPos1();

						pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::DoneMk); // // 불량이미지 Display, Align변수 초기화 (Skip 65 : Mk())
					}
				}
				else
					pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::Move1Cam1); // FourPointAlign1(1) 으로 진행. - 카메라 재정렬
			}
			else
				pView->m_mgrProcedure->m_nMkStAuto++;

			break;
		}
	}
}

void CGvisR2R_PunchView::Mk4PtAlignPt2()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pView->m_mgrProcedure->m_bMkSt)
	{
		switch (pView->m_mgrProcedure->m_nMkStAuto)
		{
		case MK_ST + (Mk4PtIdx::Move2Cam1) :
			if (bDualTest)
			{
				if (pView->m_mgrProcedure->m_bSerialDecrese)
				{
					if (pView->m_mgrProcedure->m_bLastProc && pView->m_mgrProcedure->m_nBufDnSerial[0] - 1 < pView->m_mgrProcedure->m_nLotEndSerial)	// AOI하면 Serial
					{
						pView->m_mgrProcedure->m_bSkipAlign[1][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
						pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						pView->m_mgrProcedure->m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(2))	// Move - Cam1 - Pt2
							pView->m_mgrProcedure->m_nMkStAuto++;
					}
				}
				else
				{
					if (pView->m_mgrProcedure->m_bLastProc && pView->m_mgrProcedure->m_nBufDnSerial[0] + 1 > pView->m_mgrProcedure->m_nLotEndSerial)	// AOI하면 Serial
					{
						pView->m_mgrProcedure->m_bSkipAlign[1][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
						pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						pView->m_mgrProcedure->m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(2))	// Move - Cam1 - Pt2
							pView->m_mgrProcedure->m_nMkStAuto++;
					}
				}
			}
			else
			{
				if (pView->m_mgrProcedure->m_bSerialDecrese)
				{
					if (pView->m_mgrProcedure->m_bLastProc && pView->m_mgrProcedure->m_nBufUpSerial[0] - 1 < pView->m_mgrProcedure->m_nLotEndSerial)	// AOI상면 Serial
					{
						pView->m_mgrProcedure->m_bSkipAlign[1][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
						pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						pView->m_mgrProcedure->m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(2))	// Move - Cam1 - Pt2
							pView->m_mgrProcedure->m_nMkStAuto++;
					}
				}
				else
				{
					if (pView->m_mgrProcedure->m_bLastProc && pView->m_mgrProcedure->m_nBufUpSerial[0] + 1 > pView->m_mgrProcedure->m_nLotEndSerial)	// AOI상면 Serial
					{
						pView->m_mgrProcedure->m_bSkipAlign[1][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
						pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						pView->m_mgrProcedure->m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(2))	// Move - Cam1 - Pt2
							pView->m_mgrProcedure->m_nMkStAuto++;
					}
				}
			}
			break;
		case MK_ST + (Mk4PtIdx::Move2Cam1) + 1:
			if (pView->m_mgrProcedure->IsRun())
				pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Move2Cam0) :
			if (MoveAlign0(2))	// Move - Cam0 - Pt2
				pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Move2Cam0) + 1:
			if (pView->m_mgrProcedure->IsRun())
				pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Move2Cam0) + 2:
			if (IsMoveDone())
			{
				Sleep(100);
				pView->m_mgrProcedure->m_nMkStAuto++;
			}
			break;
		case MK_ST + (Mk4PtIdx::Align1_2) :	// 4PtAlign - Cam1 - Pt2
			if (!pView->m_mgrProcedure->m_bFailAlign[1][0])
			{
				if (!pView->m_mgrProcedure->m_bSkipAlign[1][1])
				{
					if (!pView->m_mgrProcedure->m_bSkipAlign[1][2])
					{
						if (!FourPointAlign1(2))
							pView->m_mgrProcedure->m_bFailAlign[1][2] = TRUE;
						else
							pView->m_mgrProcedure->m_bFailAlign[1][2] = FALSE;
					}
					else
						pView->m_mgrProcedure->m_bFailAlign[1][2] = FALSE;
				}
				else
					pView->m_mgrProcedure->m_bFailAlign[1][2] = FALSE;
			}
			else
				pView->m_mgrProcedure->m_bFailAlign[1][2] = FALSE;

			pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Align0_2) :	// 4PtAlign - Cam0 - Pt2
			if (!pView->m_mgrProcedure->m_bFailAlign[0][0])
			{
				if (!pView->m_mgrProcedure->m_bSkipAlign[0][1])
				{
					if (!pView->m_mgrProcedure->m_bSkipAlign[0][2])
					{
						if (!FourPointAlign0(2))
							pView->m_mgrProcedure->m_bFailAlign[0][2] = TRUE;
						else
							pView->m_mgrProcedure->m_bFailAlign[0][2] = FALSE;
					}
					else
						pView->m_mgrProcedure->m_bFailAlign[0][2] = FALSE;
				}
				else
					pView->m_mgrProcedure->m_bFailAlign[0][2] = FALSE;
			}
			else
				pView->m_mgrProcedure->m_bFailAlign[0][2] = FALSE;

			pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Align0_2) + 1:
			if (pView->m_mgrProcedure->m_bFailAlign[0][2])
			{
				Buzzer(TRUE, 0);

				if (IDNO == MsgBox(_T("카메라(좌)의 검사판넬을 다시 정렬하시겠습니까?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);

					if (IDYES == MsgBox(_T("카메라(좌)의 검사판넬 정렬을 정말 않하시겠습니까?"), 0, MB_YESNO))
					{
						pView->m_mgrProcedure->m_bReAlign[0][2] = FALSE;
						pView->m_mgrProcedure->m_bSkipAlign[0][2] = TRUE;

						if (IDNO == MsgBox(_T("카메라(좌)의 검사판넬을 불량마킹 하시겠습니까?"), 0, MB_YESNO))
						{
							pView->m_mgrProcedure->m_bDoMk[0] = FALSE;
							pView->m_mgrProcedure->m_bDoneMk[0] = TRUE;
						}
						else
						{
							pView->m_mgrProcedure->m_bDoMk[0] = TRUE;
							pView->m_mgrProcedure->m_bDoneMk[0] = FALSE;
						}
					}
					else
					{
						pView->m_mgrProcedure->m_bReAlign[0][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[0][2] = FALSE;
						pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::Move2Cam0); // FourPointAlign0(2) 으로 진행. - 카메라 재정렬
						Stop();
						TowerLamp(RGB_YELLOW, TRUE);
					}
				}
				else
				{
					Buzzer(FALSE, 0);

					pView->m_mgrProcedure->m_bReAlign[0][2] = TRUE;
					pView->m_mgrProcedure->m_bSkipAlign[0][2] = FALSE;
					pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::Move2Cam0); // FourPointAlign0(2) 으로 진행. - 카메라 재정렬
					Stop();
					TowerLamp(RGB_YELLOW, TRUE);
				}
			}
			if (pView->m_mgrProcedure->m_bFailAlign[1][2])
			{
				Buzzer(TRUE, 0);

				if (IDNO == MsgBox(_T("카메라(우)의 검사판넬을 다시 정렬하시겠습니까?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);

					if (IDYES == MsgBox(_T("카메라(우)의 검사판넬 정렬을 정말 않하시겠습니까?"), 0, MB_YESNO))
					{
						pView->m_mgrProcedure->m_bReAlign[1][2] = FALSE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = TRUE;

						if (IDNO == MsgBox(_T("카메라(우)의 검사판넬을 불량마킹 하시겠습니까?"), 0, MB_YESNO))
						{
							pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
							pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						}
						else
						{
							pView->m_mgrProcedure->m_bDoMk[1] = TRUE;
							pView->m_mgrProcedure->m_bDoneMk[1] = FALSE;
						}
					}
					else
					{
						pView->m_mgrProcedure->m_bReAlign[1][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = FALSE;

						pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::Move2Cam1); // FourPointAlign1(2) 으로 진행. - 카메라 재정렬
						Stop();
						TowerLamp(RGB_YELLOW, TRUE);
					}
				}
				else
				{
					Buzzer(FALSE, 0);

					pView->m_mgrProcedure->m_bReAlign[1][2] = TRUE;
					pView->m_mgrProcedure->m_bSkipAlign[1][2] = FALSE;

					pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::Move2Cam1); // FourPointAlign1(2) 으로 진행. - 카메라 재정렬
					Stop();
					TowerLamp(RGB_YELLOW, TRUE);
				}
			}

			if (pView->m_mgrProcedure->m_bFailAlign[0][2] || pView->m_mgrProcedure->m_bFailAlign[1][2])
			{
				if (!pView->m_mgrProcedure->m_bReAlign[0][2] && !pView->m_mgrProcedure->m_bReAlign[1][2])
				{
					if (pView->m_mgrProcedure->m_bDoMk[0] || pView->m_mgrProcedure->m_bDoMk[1])
						pView->m_mgrProcedure->m_nMkStAuto++;//m_nMkStAuto = MK_ST + 29;  // MoveInitPos0()
					else
					{
						if (!IsInitPos0())
							MoveInitPos0();
						if (!IsInitPos1())
							MoveInitPos1();

						pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::DoneMk); // // 불량이미지 Display, Align변수 초기화 (Skip 65 : Mk())
					}
				}
				else
					pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::Move2Cam1); // FourPointAlign1(2) 으로 진행. - 카메라 재정렬
			}
			else
				pView->m_mgrProcedure->m_nMkStAuto++;

			break;
		}
	}
}

void CGvisR2R_PunchView::Mk4PtAlignPt3()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pView->m_mgrProcedure->m_bMkSt)
	{
		switch (pView->m_mgrProcedure->m_nMkStAuto)
		{
		case MK_ST + (Mk4PtIdx::Move3Cam1) :
			if (bDualTest)
			{
				if (pView->m_mgrProcedure->m_bSerialDecrese)
				{
					if (pView->m_mgrProcedure->m_bLastProc && pView->m_mgrProcedure->m_nBufDnSerial[0] - 1 < pView->m_mgrProcedure->m_nLotEndSerial)	// AOI하면 Serial
					{
						pView->m_mgrProcedure->m_bSkipAlign[1][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
						pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						pView->m_mgrProcedure->m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(3))	// Move - Cam1 - Pt3
							pView->m_mgrProcedure->m_nMkStAuto++;
					}
				}
				else
				{
					if (pView->m_mgrProcedure->m_bLastProc && pView->m_mgrProcedure->m_nBufDnSerial[0] + 1 > pView->m_mgrProcedure->m_nLotEndSerial)	// AOI하면 Serial
					{
						pView->m_mgrProcedure->m_bSkipAlign[1][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
						pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						pView->m_mgrProcedure->m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(3))	// Move - Cam1 - Pt3
							pView->m_mgrProcedure->m_nMkStAuto++;
					}
				}
			}
			else
			{
				if (pView->m_mgrProcedure->m_bSerialDecrese)
				{
					if (pView->m_mgrProcedure->m_bLastProc && pView->m_mgrProcedure->m_nBufUpSerial[0] - 1 < pView->m_mgrProcedure->m_nLotEndSerial)	// AOI상면 Serial
					{
						pView->m_mgrProcedure->m_bSkipAlign[1][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
						pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						pView->m_mgrProcedure->m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(3))	// Move - Cam1 - Pt3
							pView->m_mgrProcedure->m_nMkStAuto++;
					}
				}
				else
				{
					if (pView->m_mgrProcedure->m_bLastProc && pView->m_mgrProcedure->m_nBufUpSerial[0] + 1 > pView->m_mgrProcedure->m_nLotEndSerial)	// AOI상면 Serial
					{
						pView->m_mgrProcedure->m_bSkipAlign[1][0] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][1] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][2] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = TRUE;
						pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
						pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						pView->m_mgrProcedure->m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(3))	// Move - Cam1 - Pt3
							pView->m_mgrProcedure->m_nMkStAuto++;
					}
				}
			}
			break;
		case MK_ST + (Mk4PtIdx::Move3Cam1) + 1:
			if (pView->m_mgrProcedure->IsRun())
				pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Move3Cam0) :
			if (MoveAlign0(3))	// Move - Cam0 - Pt3
				pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Move3Cam0) + 1:
			if (pView->m_mgrProcedure->IsRun())
				pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Move3Cam0) + 2:
			if (IsMoveDone())
			{
				Sleep(100);
				pView->m_mgrProcedure->m_nMkStAuto++;
			}
			break;
		case MK_ST + (Mk4PtIdx::Align1_3) :	// 4PtAlign - Cam1 - Pt3
			if (!pView->m_mgrProcedure->m_bFailAlign[1][0])
			{
				if (!pView->m_mgrProcedure->m_bSkipAlign[1][1])
				{
					if (!pView->m_mgrProcedure->m_bSkipAlign[1][2])
					{
						if (!pView->m_mgrProcedure->m_bSkipAlign[1][3])
						{
							if (!FourPointAlign1(3))
								pView->m_mgrProcedure->m_bFailAlign[1][3] = TRUE;
							else
								pView->m_mgrProcedure->m_bFailAlign[1][3] = FALSE;
						}
						else
							pView->m_mgrProcedure->m_bFailAlign[1][3] = FALSE;
					}
					else
						pView->m_mgrProcedure->m_bFailAlign[1][3] = FALSE;
				}
				else
					pView->m_mgrProcedure->m_bFailAlign[1][3] = FALSE;
			}
			else
				pView->m_mgrProcedure->m_bFailAlign[1][3] = FALSE;

			pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Align0_3) :	// 4PtAlign - Cam0 - Pt3
			if (!pView->m_mgrProcedure->m_bFailAlign[0][0])
			{
				if (!pView->m_mgrProcedure->m_bSkipAlign[0][1])
				{
					if (!pView->m_mgrProcedure->m_bSkipAlign[0][2])
					{
						if (!pView->m_mgrProcedure->m_bSkipAlign[0][3])
						{
							if (!FourPointAlign0(3))
								pView->m_mgrProcedure->m_bFailAlign[0][3] = TRUE;
							else
								pView->m_mgrProcedure->m_bFailAlign[0][3] = FALSE;
						}
						else
							pView->m_mgrProcedure->m_bFailAlign[0][3] = FALSE;
					}
					else
						pView->m_mgrProcedure->m_bFailAlign[0][3] = FALSE;
				}
				else
					pView->m_mgrProcedure->m_bFailAlign[0][3] = FALSE;
			}
			else
				pView->m_mgrProcedure->m_bFailAlign[0][3] = FALSE;

			pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Align0_3) + 1:
			if (pView->m_mgrProcedure->m_bFailAlign[0][3])
			{
				Buzzer(TRUE, 0);

				if (IDNO == MsgBox(_T("카메라(좌)의 검사판넬을 다시 정렬하시겠습니까?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);

					if (IDYES == MsgBox(_T("카메라(좌)의 검사판넬 정렬을 정말 않하시겠습니까?"), 0, MB_YESNO))
					{
						pView->m_mgrProcedure->m_bReAlign[0][3] = FALSE;
						pView->m_mgrProcedure->m_bSkipAlign[0][3] = TRUE;

						if (IDNO == MsgBox(_T("카메라(좌)의 검사판넬을 불량마킹 하시겠습니까?"), 0, MB_YESNO))
						{
							pView->m_mgrProcedure->m_bDoMk[0] = FALSE;
							pView->m_mgrProcedure->m_bDoneMk[0] = TRUE;
						}
						else
						{
							pView->m_mgrProcedure->m_bDoMk[0] = TRUE;
							pView->m_mgrProcedure->m_bDoneMk[0] = FALSE;
						}
					}
					else
					{
						pView->m_mgrProcedure->m_bReAlign[0][3] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[0][3] = FALSE;

						pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::Move3Cam0); // FourPointAlign0(3) 으로 진행. - 카메라 재정렬
						Stop();
						TowerLamp(RGB_YELLOW, TRUE);
					}
				}
				else
				{
					Buzzer(FALSE, 0);

					pView->m_mgrProcedure->m_bReAlign[0][3] = TRUE;
					pView->m_mgrProcedure->m_bSkipAlign[0][3] = FALSE;

					pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::Move3Cam0); // FourPointAlign0(3) 으로 진행. - 카메라 재정렬
					Stop();
					TowerLamp(RGB_YELLOW, TRUE);
				}
			}
			if (pView->m_mgrProcedure->m_bFailAlign[1][3])
			{
				Buzzer(TRUE, 0);

				if (IDNO == MsgBox(_T("카메라(우)의 검사판넬을 다시 정렬하시겠습니까?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);

					if (IDYES == MsgBox(_T("카메라(우)의 검사판넬 정렬을 정말 않하시겠습니까?"), 0, MB_YESNO))
					{
						pView->m_mgrProcedure->m_bReAlign[1][3] = FALSE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = TRUE;

						if (IDNO == MsgBox(_T("카메라(우)의 검사판넬을 불량마킹 하시겠습니까?"), 0, MB_YESNO))
						{
							pView->m_mgrProcedure->m_bDoMk[1] = FALSE;
							pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
						}
						else
						{
							pView->m_mgrProcedure->m_bDoMk[1] = TRUE;
							pView->m_mgrProcedure->m_bDoneMk[1] = FALSE;
						}
					}
					else
					{
						pView->m_mgrProcedure->m_bReAlign[1][3] = TRUE;
						pView->m_mgrProcedure->m_bSkipAlign[1][3] = FALSE;

						pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::Move3Cam1); // FourPointAlign1(3) 으로 진행. - 카메라 재정렬
						Stop();
						TowerLamp(RGB_YELLOW, TRUE);
					}
				}
				else
				{
					Buzzer(FALSE, 0);

					pView->m_mgrProcedure->m_bReAlign[1][3] = TRUE;
					pView->m_mgrProcedure->m_bSkipAlign[1][3] = FALSE;

					pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::Move3Cam1); // FourPointAlign1(3) 으로 진행. - 카메라 재정렬
					Stop();
					TowerLamp(RGB_YELLOW, TRUE);
				}
			}

			if (pView->m_mgrProcedure->m_bFailAlign[0][3] || pView->m_mgrProcedure->m_bFailAlign[1][3])
			{
				if (!pView->m_mgrProcedure->m_bReAlign[0][3] && !pView->m_mgrProcedure->m_bReAlign[1][3])
				{
					if (pView->m_mgrProcedure->m_bDoMk[0] || pView->m_mgrProcedure->m_bDoMk[1])
						pView->m_mgrProcedure->m_nMkStAuto++;//m_nMkStAuto = MK_ST + 29;  // MoveInitPos0()
					else
					{
						if (!IsInitPos0())
							MoveInitPos0();
						if (!IsInitPos1())
							MoveInitPos1();

						pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::DoneMk); // // 불량이미지 Display, Align변수 초기화 (Skip 65 : Mk())
					}
				}
				else
					pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::Move3Cam1); // FourPointAlign1(3) 으로 진행. - 카메라 재정렬
			}
			else
				pView->m_mgrProcedure->m_nMkStAuto++;

			break;
		}
	}
}

void CGvisR2R_PunchView::Mk4PtMoveInitPos()
{
	if (pView->m_mgrProcedure->m_bMkSt)
	{
		switch (pView->m_mgrProcedure->m_nMkStAuto)
		{
		case MK_ST + (Mk4PtIdx::MoveInitPt) :
			MoveInitPos0();
			pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::MoveInitPt) + 1:
			if (pView->m_mgrProcedure->m_bDoMk[1])
				MoveInitPos1();
			else
				MoveMkEdPos1();
			pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::MoveInitPt) + 2:
			if (IsMoveDone())
				pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk4PtElecChk()
{
	CString sRst;

	if (pView->m_mgrProcedure->m_bMkSt)
	{
		switch (pView->m_mgrProcedure->m_nMkStAuto)
		{
		case MK_ST + (Mk4PtIdx::ChkElec) : // DoElecChk
			if (DoElecChk(sRst))
			{
				if (pDoc->WorkingInfo.Probing[0].bUse)
				{
					if (sRst == _T("Open"))
					{
						if (pDoc->WorkingInfo.Probing[0].bStopOnOpen)
							pView->m_mgrProcedure->m_nMkStAuto = REJECT_ST;
						else
							pView->m_mgrProcedure->m_nMkStAuto++;
					}
					else if (sRst == _T("Error"))
					{
						pView->m_mgrProcedure->m_nMkStAuto = ERROR_ST;
					}
					else
					{
						pView->m_mgrProcedure->m_nMkStAuto++;
					}
				}
				else
					pView->m_mgrProcedure->m_nMkStAuto++;
			}
		   break;

		case MK_ST + (Mk4PtIdx::ChkElec) + 1:
			if (ChkLightErr())
			{
				pView->m_mgrProcedure->m_bChkLightErr = FALSE;
				pView->m_mgrProcedure->m_nMkStAuto++;
			}
			else
				pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::DoMk);	// Mk 마킹 시작
			break;

		case MK_ST + (Mk4PtIdx::ChkElec) + 2:
			if (pView->m_mgrProcedure->IsRun())
			{
				if (m_pMotion->IsEnable(MS_X0) && m_pMotion->IsEnable(MS_Y0) &&
					m_pMotion->IsEnable(MS_X1) && m_pMotion->IsEnable(MS_Y1))
				{
					if (MODE_INNER != pDoc->GetTestMode())
						pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::DoMk); 	// Mk 마킹 시작
					else
						pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::Shift2Mk);
				}
				else
				{
					Stop();
					MsgBox(_T("마킹부 모션이 비활성화 되었습니다."));
					TowerLamp(RGB_RED, TRUE);
				}
			}
			else
			{
				if (!pView->m_mgrProcedure->m_bChkLightErr)
				{
					pView->m_mgrProcedure->m_bChkLightErr = TRUE;
					MsgBox(_T("노광불량 정지 - 기판을 확인하세요.\r\n계속진행하려면 운전스위치를 누르세요."));
				}
			}
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk4PtDoMarking()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	CString sRst, sMsg;
	int a, b, nSerial, nPrevSerial;

	if (pView->m_mgrProcedure->m_bMkSt)
	{
		switch (pView->m_mgrProcedure->m_nMkStAuto)
		{
		case MK_ST + (Mk4PtIdx::DoMk) :				// Mk 마킹 시작
			if (pDoc->GetTestMode() == MODE_OUTER)
				SetMkIts(TRUE);						// ITS 마킹 시작
			else
				SetMk(TRUE);						// Mk 마킹 시작

			pView->m_mgrProcedure->m_nMkStAuto++;
			break;

		case MK_ST + (Mk4PtIdx::DoMk) + 1:
			Sleep(100);
			pView->m_mgrProcedure->m_nMkStAuto++;
			break;

		case MK_ST + (Mk4PtIdx::Verify) :
			if (IsMkDone() && IsMoveDone())
			{
				if (IsVerify() && !pView->m_mgrProcedure->m_nPrevMkStAuto)
				{
					pView->m_mgrProcedure->m_nPrevMkStAuto = MK_ST + (Mk4PtIdx::Verify);
					pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::DoMk);		// Mk 마킹 시작
					pView->m_mgrProcedure->m_bCam = TRUE;
				}
				else
				{
					if (IsReview())
					{
						if (!pView->m_mgrProcedure->m_bCam)
						{
							pView->m_mgrProcedure->m_nPrevStepAuto = MK_ST + (Mk4PtIdx::Verify);
							pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::DoMk);		// Mk 마킹 시작
							pView->m_mgrProcedure->m_bCam = TRUE;
							MsgBox(_T("▶ Jog 버튼을 이용하여 마킹위치를 확인하여 주세요."));
						}
						else
						{
							pView->m_mgrProcedure->m_bCam = FALSE;
							pView->m_mgrProcedure->m_nMkStAuto++;	// Mk 마킹 완료
						}
					}
					else
					{
						pView->m_mgrProcedure->m_nMkStAuto++;	// Mk 마킹 완료
					}
				}
			}
			else if (IsReMk())
			{
				pView->m_mgrProcedure->m_nPrevMkStAuto = MK_ST + (Mk4PtIdx::Verify);
				pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::DoMk);		// Mk 재시작
			}
			else
			{
				sMsg = _T("");
				sMsg += pView->m_mgrProcedure->m_sDispSts[0];
				sMsg += _T(",");
				sMsg += pView->m_mgrProcedure->m_sDispSts[1];
				//DispStsBar(sMsg, 0);
			}
										break;

		case MK_ST + (Mk4PtIdx::DoneMk) :	 // Align변수 초기화
			pView->m_mgrProcedure->m_bReAlign[0][0] = FALSE; // [nCam][nPos] 
			pView->m_mgrProcedure->m_bReAlign[0][1] = FALSE; // [nCam][nPos] 
			pView->m_mgrProcedure->m_bReAlign[1][0] = FALSE; // [nCam][nPos] 
			pView->m_mgrProcedure->m_bReAlign[1][1] = FALSE; // [nCam][nPos] 

			pView->m_mgrProcedure->m_bSkipAlign[0][0] = FALSE; // [nCam][nPos] 
			pView->m_mgrProcedure->m_bSkipAlign[0][1] = FALSE; // [nCam][nPos] 
			pView->m_mgrProcedure->m_bSkipAlign[1][0] = FALSE; // [nCam][nPos] 
			pView->m_mgrProcedure->m_bSkipAlign[1][1] = FALSE; // [nCam][nPos] 

			pView->m_mgrProcedure->m_bFailAlign[0][0] = FALSE; // [nCam][nPos] 
			pView->m_mgrProcedure->m_bFailAlign[0][1] = FALSE; // [nCam][nPos] 
			pView->m_mgrProcedure->m_bFailAlign[1][0] = FALSE; // [nCam][nPos] 
			pView->m_mgrProcedure->m_bFailAlign[1][1] = FALSE; // [nCam][nPos] 
			pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::DoneMk) + 1:  // Mk변수 초기화
			pView->m_mgrProcedure->m_bDoMk[0] = TRUE;
			pView->m_mgrProcedure->m_bDoMk[1] = TRUE;
			pView->m_mgrProcedure->m_bDoneMk[0] = FALSE;
			pView->m_mgrProcedure->m_bDoneMk[1] = FALSE;
			pView->m_mgrProcedure->m_bReMark[0] = FALSE;
			pView->m_mgrProcedure->m_bReMark[1] = FALSE;
			pView->m_mgrProcedure->m_bCam = FALSE;
			pView->m_mgrProcedure->m_nPrevMkStAuto = 0;

			for (a = 0; a < 2; a++)
			{
				for (b = 0; b < MAX_STRIP_NUM; b++)
				{
					pView->m_mgrProcedure->m_nMkStrip[a][b] = 0;
					pView->m_mgrProcedure->m_bRejectDone[a][b] = FALSE;
				}
			}

			pView->m_mgrProcedure->m_nMkStAuto++;
			break;

		case MK_ST + (Mk4PtIdx::DoneMk) + 2:
			m_pMpe->Write(_T("MB440150"), 0);	// 마킹부 마킹중 ON (PC가 ON, OFF)
			m_pMpe->Write(_T("MB440170"), 1);	// 마킹완료(PLC가 확인하고 Reset시킴.)-20141029
			if (IsNoMk() || IsShowLive())
				ShowLive(FALSE);

			pView->m_mgrProcedure->m_nMkStAuto++;
			break;

		case MK_ST + (Mk4PtIdx::DoneMk) + 3:
#ifdef USE_MPE
			if (pDoc->m_pMpeSignal[0] & (0x01 << 1))	// 마킹부 Feeding완료(PLC가 On시키고 PC가 확인하고 Reset시킴.)-20141030
			{
				m_pMpe->Write(_T("MB440101"), 0);	// 마킹부 Feeding완료

				Shift2Mk();			// PCR 이동(Buffer->Marked) // 기록(WorkingInfo.LastJob.sSerial)
				UpdateRst();
				SetMkFdLen();

				SetCycTime();
				pView->m_mgrProcedure->m_dwCycSt = GetTickCount();

				UpdateWorking();	// Update Working Info...
				pView->m_mgrReelmap->ChkYield();// (dTotLmt, dPrtLmt, dRatio);
				pView->m_mgrProcedure->m_nMkStAuto++;
			}
#endif
			break;
		case MK_ST + (Mk4PtIdx::DoneMk) + 4:
			ChkLotCutPos();
			pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::DoneMk) + 5:
			pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::DoneMk) + 6:
			pView->m_mgrProcedure->m_nMkStAuto++;
			if (!IsBuffer(0))
			{
				pView->m_mgrProcedure->m_bLotEnd = TRUE;
				pView->m_mgrProcedure->m_nLotEndAuto = LOT_END;
			}
			else
			{
				// [0]: AOI-Up , [1]: AOI-Dn
				if (bDualTest)
				{
					nSerial = pDoc->m_ListBuf[0].Pop(); // Up (좌)
					nSerial = pDoc->m_ListBuf[0].Pop(); // Up (우)

					SetListBuf();
				}

				if (pView->m_mgrProcedure->m_nLotEndSerial > 0)
				{
					nSerial = GetBufferUp(&nPrevSerial);

					if (pView->m_mgrProcedure->m_bSerialDecrese)
					{
						if (nSerial < pView->m_mgrProcedure->m_nLotEndSerial || nSerial <= 0) // 노광불량 3번째 Lot End ( -1, -1, -2)
						{
							if (IsDoneDispMkInfo())
							{
								if (IsBuffer() && nSerial > 0)
									SetSerial(nSerial);
								else
									SetSerial(nPrevSerial - 1, TRUE);

								if (IsBuffer() && nSerial > 0)
									SetSerial(nSerial + 1);
								else
									SetSerial(nPrevSerial - 2, TRUE);

								pView->m_mgrProcedure->m_bLotEnd = TRUE;
								pView->m_mgrProcedure->m_nLotEndAuto = LOT_END;
							}
						}
					}
					else
					{
						if (nSerial > pView->m_mgrProcedure->m_nLotEndSerial || nSerial <= 0) // 노광불량 3번째 Lot End ( -1, -1, -2)
						{
							if (IsDoneDispMkInfo())
							{
								if (IsBuffer() && nSerial > 0)
									SetSerial(nSerial);
								else
									SetSerial(nPrevSerial + 1, TRUE);

								if (IsBuffer() && nSerial > 0)
									SetSerial(nSerial + 1);
								else
									SetSerial(nPrevSerial + 2, TRUE);

								pView->m_mgrProcedure->m_bLotEnd = TRUE;
								pView->m_mgrProcedure->m_nLotEndAuto = LOT_END;
							}
						}
					}

					if (bDualTest)
					{
						nSerial = GetBufferDn(&nPrevSerial);

						if (pView->m_mgrProcedure->m_bSerialDecrese)
						{
							if (nSerial < pView->m_mgrProcedure->m_nLotEndSerial || nSerial <= 0) // 노광불량 3번째 Lot End ( -1, -1, -2)
							{
								if (IsDoneDispMkInfo())
								{
									if (IsBuffer() && nSerial > 0)
										SetSerial(nSerial);
									else
										SetSerial(nPrevSerial - 1, TRUE);

									if (IsBuffer() && nSerial > 0)
										SetSerial(nSerial + 1);
									else
										SetSerial(nPrevSerial - 2, TRUE);

									pView->m_mgrProcedure->m_bLotEnd = TRUE;
									pView->m_mgrProcedure->m_nLotEndAuto = LOT_END;
								}
							}
						}
						else
						{
							if (nSerial > pView->m_mgrProcedure->m_nLotEndSerial || nSerial <= 0) // 노광불량 3번째 Lot End ( -1, -1, -2)
							{
								if (IsDoneDispMkInfo())
								{
									if (IsBuffer() && nSerial > 0)
										SetSerial(nSerial);
									else
										SetSerial(nPrevSerial + 1, TRUE);

									if (IsBuffer() && nSerial > 0)
										SetSerial(nSerial + 1);
									else
										SetSerial(nPrevSerial + 2, TRUE);

									pView->m_mgrProcedure->m_bLotEnd = TRUE;
									pView->m_mgrProcedure->m_nLotEndAuto = LOT_END;
								}
							}
						}
					}
				}
			}

			break;
		case MK_ST + (Mk4PtIdx::DoneMk) + 7:
			pView->m_mgrProcedure->m_bMkSt = FALSE;
			::WritePrivateProfileString(_T("Last Job"), _T("MkSt"), _T("0"), PATH_WORKING_INFO);
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk4PtLotDiff()
{
	if (pView->m_mgrProcedure->m_bMkSt)
	{
		switch (pView->m_mgrProcedure->m_nMkStAuto)
		{
		case MK_ST + (Mk4PtIdx::LotDiff) :
			Stop();
			TowerLamp(RGB_YELLOW, TRUE);
			pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::LotDiff) + 1:
			if (IDYES == MsgBox(_T("상면과 하면의 Lot가 다릅니다.\r\n계속 작업을 진행하시겠습니까?"), 0, MB_YESNO))
			{
				pView->m_mgrProcedure->m_bContDiffLot = TRUE;
			}
			else
			{
				pView->m_mgrProcedure->m_bContDiffLot = FALSE;
				pView->m_mgrProcedure->m_bLotEnd = TRUE;
				pView->m_mgrProcedure->m_nLotEndAuto = LOT_END;
			}
			pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::LotDiff) + 2:
			if (pView->m_mgrProcedure->IsRun())
			{
				if (pView->m_mgrProcedure->m_bContDiffLot)
					pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::ChkSn);
				else
					pView->m_mgrProcedure->m_nMkStAuto++;
			}
			break;
		case MK_ST + (Mk4PtIdx::LotDiff) + 3:
			pView->m_mgrProcedure->m_bContDiffLot = FALSE;
			pView->m_mgrProcedure->m_bLotEnd = TRUE;
			pView->m_mgrProcedure->m_nLotEndAuto = LOT_END;
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk4PtReject()
{
	int a, b;

	if (pView->m_mgrProcedure->m_bMkSt)
	{
		switch (pView->m_mgrProcedure->m_nMkStAuto)
		{
		case REJECT_ST:
			Stop();
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);

			if (IDYES == MsgBox(_T("쇼트 체크 불량입니다.\r\n리젝 처리를 진행하시겠습니까?"), 0, MB_YESNO))
			{
				pView->m_mgrProcedure->m_bAnswer[0] = TRUE;
				pView->m_mgrProcedure->m_nMkStAuto++;
			}
			else
			{
				if (IDYES == MsgBox(_T("리젝 처리를 취소하였습니다.\r\n불량만 마킹 처리를 진행하시겠습니까?"), 0, MB_YESNO))
				{
					pView->m_mgrProcedure->m_bAnswer[1] = TRUE;
					pView->m_mgrProcedure->m_nMkStAuto++;
				}
			}

			Buzzer(FALSE, 0);
			break;
		case REJECT_ST + 1:
			if (pView->m_mgrProcedure->IsRun())
			{
				if (pView->m_mgrProcedure->m_bAnswer[0])
				{
					pView->m_mgrProcedure->m_bAnswer[0] = FALSE;
					pView->m_mgrProcedure->m_nMkStAuto++;
				}
				else if (pView->m_mgrProcedure->m_bAnswer[1])
				{
					pView->m_mgrProcedure->m_bAnswer[1] = FALSE;
					pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::DoMk);	// Mk 마킹 시작
				}
				else
					pView->m_mgrProcedure->m_nMkStAuto = REJECT_ST;
			}
			break;
		case REJECT_ST + 2:
			SetReject();
			pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case REJECT_ST + 3:
			pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case REJECT_ST + 4:
			if (IsMkDone() && IsMoveDone())
			{
				if (IsVerify() && !pView->m_mgrProcedure->m_nPrevMkStAuto)
				{
					pView->m_mgrProcedure->m_nPrevMkStAuto = REJECT_ST + 4;
					pView->m_mgrProcedure->m_nMkStAuto = REJECT_ST + 2;		// Mk 마킹 시작
					pView->m_mgrProcedure->m_bCam = TRUE;

					pView->m_mgrProcedure->m_bDoneMk[0] = FALSE;
					pView->m_mgrProcedure->m_bDoneMk[1] = FALSE;

					for (a = 0; a < 2; a++)
					{
						for (b = 0; b < MAX_STRIP_NUM; b++)
						{
							pView->m_mgrProcedure->m_nMkStrip[a][b] = 0;
							pView->m_mgrProcedure->m_bRejectDone[a][b] = FALSE;
						}
					}
				}
				else
				{
					if (IsReview())
					{
						if (!pView->m_mgrProcedure->m_bCam)
						{
							pView->m_mgrProcedure->m_nPrevStepAuto = REJECT_ST + 4;
							pView->m_mgrProcedure->m_nMkStAuto = REJECT_ST + 2;		// Mk 마킹 시작
							pView->m_mgrProcedure->m_bCam = TRUE;
							MsgBox(_T("▶ Jog 버튼을 이용하여 마킹위치를 확인하여 주세요."));
						}
						else
						{
							pView->m_mgrProcedure->m_bCam = FALSE;
							pView->m_mgrProcedure->m_nMkStAuto++;	// Mk 마킹 완료
						}
					}
					else
					{
						pView->m_mgrProcedure->m_nMkStAuto++;	// Mk 마킹 완료
					}
				}
			}
			break;
		case REJECT_ST + 5:
			pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::DoneMk);				// Align변수 초기화
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk4PtErrStop()
{
	if (pView->m_mgrProcedure->m_bMkSt)
	{
		switch (pView->m_mgrProcedure->m_nMkStAuto)
		{
		case ERROR_ST:
			Stop();
			TowerLamp(RGB_RED, TRUE);

			if (IDYES == MsgBox(_T("쇼트 체크 Error입니다.\r\n다시 쇼트 체크를 진행하시겠습니까?"), 0, MB_YESNO))
			{
				pView->m_mgrProcedure->m_bAnswer[0] = TRUE;
				pView->m_mgrProcedure->m_nMkStAuto++;
			}
			else
			{
				if (IDYES == MsgBox(_T("쇼트 체크를 취소하였습니다.\r\n불량만 마킹 처리를 진행하시겠습니까?"), 0, MB_YESNO))
				{
					pView->m_mgrProcedure->m_bAnswer[1] = TRUE;
					pView->m_mgrProcedure->m_nMkStAuto++;
				}
				else
					pView->m_mgrProcedure->m_nMkStAuto++;
			}
			break;
		case ERROR_ST + 1:
			pView->m_mgrProcedure->m_nMkStAuto++;
			break;
		case ERROR_ST + 2:
			if (pView->m_mgrProcedure->IsRun())
			{
				if (pView->m_mgrProcedure->m_bAnswer[0])
				{
					pView->m_mgrProcedure->m_bAnswer[0] = FALSE;
					pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::ChkElec); // DoElecChk
				}
				else if (pView->m_mgrProcedure->m_bAnswer[1])
				{
					pView->m_mgrProcedure->m_bAnswer[1] = FALSE;
					pView->m_mgrProcedure->m_nMkStAuto = MK_ST + (Mk4PtIdx::DoMk);	// Mk 마킹 시작
				}
				else
					pView->m_mgrProcedure->m_nMkStAuto++;
			}
			break;
		case ERROR_ST + 3:
			pView->m_mgrProcedure->m_nMkStAuto = ERROR_ST;
			break;
		}
	}
}

void CGvisR2R_PunchView::MarkingWith4PointAlign()
{
	Mk4PtReady();
	Mk4PtChkSerial();
	Mk4PtInit();
	Mk4PtAlignPt0();
	Mk4PtAlignPt1();
	Mk4PtAlignPt2();
	Mk4PtAlignPt3();
	Mk4PtMoveInitPos();
	Mk4PtElecChk();
	Mk4PtDoMarking();
	Mk4PtLotDiff();
	Mk4PtReject();
	Mk4PtErrStop();
}

BOOL CGvisR2R_PunchView::IsSameUpDnLot()
{
	if (pDoc->Status.PcrShare[0].sLot == pDoc->Status.PcrShare[1].sLot)
		return TRUE;

	return FALSE;
}

int CGvisR2R_PunchView::GetAutoStep()
{
	return pView->m_mgrProcedure->m_nStepAuto;
}

void CGvisR2R_PunchView::MoveMkInitPos()
{
	MoveMk0InitPos();
	MoveMk1InitPos();
}

void CGvisR2R_PunchView::MoveMk0InitPos()
{
	double pTgtPos[2];
	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[0]);
	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[0]);
	double dCurrX = pView->m_mgrProcedure->m_dEnc[AXIS_X0];
	double dCurrY = pView->m_mgrProcedure->m_dEnc[AXIS_Y0];

	double fLen, fVel, fAcc, fJerk;
	fLen = sqrt(((pTgtPos[0] - dCurrX) * (pTgtPos[0] - dCurrX)) + ((pTgtPos[1] - dCurrY) * (pTgtPos[1] - dCurrY)));
	if (fLen > 0.001)
	{
		pView->m_pMotion->GetSpeedProfile0(TRAPEZOIDAL, AXIS_X0, fLen, fVel, fAcc, fJerk);
		if (!pView->m_pMotion->Move0(MS_X0Y0, pTgtPos, fVel / 2.0, fAcc / 2.0, fAcc / 2.0))
		{
			if (!pView->m_pMotion->Move0(MS_X0Y0, pTgtPos, fVel / 2.0, fAcc / 2.0, fAcc / 2.0))
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Move X0Y0 Error..."));
			}
		}
	}
}

void CGvisR2R_PunchView::MoveMk1InitPos()
{
	double pTgtPos[2];
	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[1]);
	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[1]);
	double dCurrX = pView->m_mgrProcedure->m_dEnc[AXIS_X1];
	double dCurrY = pView->m_mgrProcedure->m_dEnc[AXIS_Y1];

	double fLen, fVel, fAcc, fJerk;
	fLen = sqrt(((pTgtPos[0] - dCurrX) * (pTgtPos[0] - dCurrX)) + ((pTgtPos[1] - dCurrY) * (pTgtPos[1] - dCurrY)));
	if (fLen > 0.001)
	{
		pView->m_pMotion->GetSpeedProfile1(TRAPEZOIDAL, AXIS_X1, fLen, fVel, fAcc, fJerk);
		if (!pView->m_pMotion->Move1(MS_X1Y1, pTgtPos, fVel / 2.0, fAcc / 2.0, fAcc / 2.0))
		{
			if (!pView->m_pMotion->Move1(MS_X1Y1, pTgtPos, fVel / 2.0, fAcc / 2.0, fAcc / 2.0))
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Move X1Y1 Error..."));
			}
		}
	}
}


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
			if (GetKeyState(VK_CONTROL) < 0) // Ctrl 키가 눌려진 상태
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

void CGvisR2R_PunchView::EStop()
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
		// 		DispMsg(_T("X축 충돌 범위에 의한 정지입니다."), _T("알림"), RGB_GREEN, DELAY_TIME_MSG);
		pView->ClrDispMsg();
		AfxMessageBox(_T("X축 충돌 범위에 의한 정지입니다."));

		double dCurrX = pView->m_mgrProcedure->m_dEnc[AXIS_X1];
		double dCurrY = pView->m_mgrProcedure->m_dEnc[AXIS_Y1];
		double pPos[2], fVel, fAcc, fJerk;
		double fLen = 2.0;
		pPos[0] = dCurrX + fLen;
		pPos[1] = dCurrY;
		m_pMotion->GetSpeedProfile1(TRAPEZOIDAL, AXIS_X1, fLen, fVel, fAcc, fJerk);
		m_pMotion->Move1(MS_X1Y1, pPos, fVel, fAcc, fAcc, ABS, NO_WAIT);
	}
}

//void CGvisR2R_PunchView::ResetPriority()
//{
//	pView->m_mgrProcedure->m_bPriority[0] = FALSE;
//	pView->m_mgrProcedure->m_bPriority[1] = FALSE;
//	pView->m_mgrProcedure->m_bPriority[2] = FALSE;
//	pView->m_mgrProcedure->m_bPriority[3] = FALSE;
//}
//
//void CGvisR2R_PunchView::SetPriority()
//{
//	int nDir[2];
//	nDir[0] = (pView->m_mgrProcedure->m_dTarget[AXIS_X0] - pView->m_mgrProcedure->m_dEnc[AXIS_X0]) >= 0.0 ? 1 : -1;
//	nDir[1] = (pView->m_mgrProcedure->m_dTarget[AXIS_X1] - pView->m_mgrProcedure->m_dEnc[AXIS_X1]) >= 0.0 ? 1 : -1;
//
//	// Cam0 : m_bPriority[0], m_bPriority[3]
//	// Cam1 : m_bPriority[1], m_bPriority[2]
//	if (nDir[1] > 0) // Cam1 ->
//	{
//		pView->m_mgrProcedure->m_bPriority[0] = FALSE;
//		pView->m_mgrProcedure->m_bPriority[1] = TRUE;
//		pView->m_mgrProcedure->m_bPriority[2] = FALSE;
//		pView->m_mgrProcedure->m_bPriority[3] = FALSE;
//	}
//	else if (nDir[0] < 0) // Cam0 <-
//	{
//		pView->m_mgrProcedure->m_bPriority[0] = TRUE;
//		pView->m_mgrProcedure->m_bPriority[1] = FALSE;
//		pView->m_mgrProcedure->m_bPriority[2] = FALSE;
//		pView->m_mgrProcedure->m_bPriority[3] = FALSE;
//	}
//	else if (nDir[1] < 0) // Cam1 <-
//	{
//		pView->m_mgrProcedure->m_bPriority[0] = FALSE;
//		pView->m_mgrProcedure->m_bPriority[1] = FALSE;
//		pView->m_mgrProcedure->m_bPriority[2] = TRUE;
//		pView->m_mgrProcedure->m_bPriority[3] = FALSE;
//	}
//	else // Cam0 ->
//	{
//		pView->m_mgrProcedure->m_bPriority[0] = FALSE;
//		pView->m_mgrProcedure->m_bPriority[1] = FALSE;
//		pView->m_mgrProcedure->m_bPriority[2] = FALSE;
//		pView->m_mgrProcedure->m_bPriority[3] = TRUE;
//	}
//}

BOOL CGvisR2R_PunchView::IsRunAxisX()
{
	if (m_pMotion->IsMotionDone(MS_X0) && m_pMotion->IsMotionDone(MS_X1))
		return FALSE;
	return TRUE;
}

//BOOL CGvisR2R_PunchView::ChkCollision()
//{
//	double dMg = _tstof(pDoc->WorkingInfo.Motion.sCollisionLength) - _tstof(pDoc->WorkingInfo.Motion.sCollisionMargin);
//
//	if (pView->m_mgrProcedure->m_bTHREAD_MK[0] || pView->m_mgrProcedure->m_bTHREAD_MK[1] 
//		|| pView->m_mgrProcedure->m_bTHREAD_MK[2] || pView->m_mgrProcedure->m_bTHREAD_MK[3])
//	{
//		if (pDoc->WorkingInfo.System.bNoMk || pView->m_mgrProcedure->m_bCam)
//			dMg += (_tstof(pDoc->WorkingInfo.Vision[0].sMkOffsetX) - _tstof(pDoc->WorkingInfo.Vision[1].sMkOffsetX));
//	}
//
//	if ((pView->m_mgrProcedure->m_dEnc[AXIS_X0] - dMg) > pView->m_mgrProcedure->m_dEnc[AXIS_X1])
//		return TRUE;
//
//	if (pView->m_mgrProcedure->m_dEnc[AXIS_X0] < -1.0 || pView->m_mgrProcedure->m_dEnc[AXIS_X1] < -1.0)
//		return TRUE;
//
//	return FALSE;
//}
//
//BOOL CGvisR2R_PunchView::ChkCollision(int nAxisId, double dTgtPosX)
//{
//	double dMg = _tstof(pDoc->WorkingInfo.Motion.sCollisionLength) - _tstof(pDoc->WorkingInfo.Motion.sCollisionMargin);
//
//
//	if (pView->m_mgrProcedure->m_bTHREAD_MK[0] || pView->m_mgrProcedure->m_bTHREAD_MK[1] 
//		|| pView->m_mgrProcedure->m_bTHREAD_MK[2] || pView->m_mgrProcedure->m_bTHREAD_MK[3])
//	{
//		if (pDoc->WorkingInfo.System.bNoMk || pView->m_mgrProcedure->m_bCam)
//			dMg += (_tstof(pDoc->WorkingInfo.Vision[0].sMkOffsetX) - _tstof(pDoc->WorkingInfo.Vision[1].sMkOffsetX));
//	}
//
//	if (nAxisId == AXIS_X0)
//	{
//		if (dTgtPosX > pView->m_mgrProcedure->m_dEnc[AXIS_X1] + dMg)
//			return TRUE;
//		if (pView->m_mgrProcedure->m_bDoMk[1] && !pView->m_mgrProcedure->m_bDoneMk[1])
//		{
//			if (!pView->m_mgrProcedure->m_bDoneMk[0] && pView->m_mgrProcedure->m_bTHREAD_MK[0])
//			{
//				if (pView->m_mgrProcedure->m_dTarget[AXIS_X1] > 0.0)
//				{
//					if (dTgtPosX > pView->m_mgrProcedure->m_dTarget[AXIS_X1] + dMg && pView->m_mgrProcedure->m_dTarget[AXIS_X1] > 0.0)
//						return TRUE;
//					else if (pView->m_mgrProcedure->m_dNextTarget[AXIS_X1] > 0.0)
//					{
//						if (dTgtPosX > pView->m_mgrProcedure->m_dNextTarget[AXIS_X1] + dMg)
//							return TRUE;
//					}
//				}
//			}
//		}
//	}
//	else if (nAxisId == AXIS_X1)
//	{
//		if (dTgtPosX < pView->m_mgrProcedure->m_dEnc[AXIS_X0] - dMg)
//			return TRUE;
//	}
//
//	return FALSE;
//}
//
//BOOL CGvisR2R_PunchView::ChkCollision(int nAxisId, double dTgtPosX, double dTgtNextPosX)
//{
//	double dMg = _tstof(pDoc->WorkingInfo.Motion.sCollisionLength) - _tstof(pDoc->WorkingInfo.Motion.sCollisionMargin);
//
//	if (pView->m_mgrProcedure->m_bTHREAD_MK[0] || pView->m_mgrProcedure->m_bTHREAD_MK[1] 
//		|| pView->m_mgrProcedure->m_bTHREAD_MK[2] || pView->m_mgrProcedure->m_bTHREAD_MK[3])
//	{
//		if (pDoc->WorkingInfo.System.bNoMk || pView->m_mgrProcedure->m_bCam)
//			dMg += (_tstof(pDoc->WorkingInfo.Vision[0].sMkOffsetX) - _tstof(pDoc->WorkingInfo.Vision[1].sMkOffsetX));
//	}
//
//	if (nAxisId == AXIS_X0)
//	{
//		if (dTgtPosX > pView->m_mgrProcedure->m_dEnc[AXIS_X1] + dMg)
//			return TRUE;
//		if (pView->m_mgrProcedure->m_bDoMk[1] && !pView->m_mgrProcedure->m_bDoneMk[1])
//		{
//			if (!pView->m_mgrProcedure->m_bDoneMk[0] && pView->m_mgrProcedure->m_bTHREAD_MK[0])
//			{
//				if (dTgtPosX > pView->m_mgrProcedure->m_dTarget[AXIS_X1] + dMg && pView->m_mgrProcedure->m_dTarget[AXIS_X1] > 0.0)
//					return TRUE;
//				else if (dTgtPosX > pView->m_mgrProcedure->m_dNextTarget[AXIS_X1] + dMg && pView->m_mgrProcedure->m_dNextTarget[AXIS_X1] > 0.0)
//					return TRUE;
//			}
//		}
//	}
//	else if (nAxisId == AXIS_X1)
//	{
//		if (dTgtPosX < pView->m_mgrProcedure->m_dEnc[AXIS_X0] - dMg)
//			return TRUE;
//		if (pView->m_mgrProcedure->m_bDoMk[0])
//		{
//			if (!pView->m_mgrProcedure->m_bDoneMk[1] && pView->m_mgrProcedure->m_bTHREAD_MK[1])
//			{
//				if (!m_pMotion->IsMotionDone(MS_X0))
//				{
//					if (dTgtPosX < pView->m_mgrProcedure->m_dTarget[AXIS_X0] - dMg && pView->m_mgrProcedure->m_dTarget[AXIS_X0] > 0.0)
//						return TRUE;
//					else if (dTgtPosX < pView->m_mgrProcedure->m_dNextTarget[AXIS_X0] - dMg && pView->m_mgrProcedure->m_dNextTarget[AXIS_X0] > 0.0)
//						return TRUE;
//				}
//			}
//		}
//	}
//
//	return FALSE;
//}

void CGvisR2R_PunchView::StopAllMk()
{
	// Mk0
	pView->m_mgrProcedure->m_nStepMk[2] = MK_END;

	// Mk1
	pView->m_mgrProcedure->m_nStepMk[3] = MK_END;
}

void CGvisR2R_PunchView::DoAllMk(int nCam)
{
	if (nCam == CAM_LF)
	{
		double dCurrX = pView->m_mgrProcedure->m_dEnc[AXIS_X1];
		double dCurrY = pView->m_mgrProcedure->m_dEnc[AXIS_Y1];

		double pPos[2];
		pPos[0] = _tstof(pDoc->WorkingInfo.Motion.sSafeZone);
		pPos[1] = 0.0;

		double fLen, fVel, fAcc, fJerk;
		fLen = sqrt(((pPos[0] - dCurrX) * (pPos[0] - dCurrX)) + ((pPos[1] - dCurrY) * (pPos[1] - dCurrY)));
		if (fLen > 0.001)
		{
			pView->m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_X1, fLen, fVel, fAcc, fJerk);
			if (!pView->m_pMotion->Move(MS_X1Y1, pPos, fVel, fAcc, fAcc))
			{
				if (!pView->m_pMotion->Move(MS_X1Y1, pPos, fVel, fAcc, fAcc))
				{
					pView->ClrDispMsg();
					AfxMessageBox(_T("Move X1Y1 Error..."));
				}
			}
		}
		if (!pView->m_mgrProcedure->m_bTHREAD_MK[2])
		{
			pView->m_mgrProcedure->m_nStepMk[2] = 0;
			pView->m_mgrProcedure->m_nMkPcs[2] = 0;
			pView->m_mgrProcedure->m_bTHREAD_MK[2] = TRUE;
		}
		else
		{
			pView->m_mgrProcedure->m_bTHREAD_MK[2] = FALSE;
		}
	}
	else if (nCam == CAM_RT)
	{
		double dCurrX = pView->m_mgrProcedure->m_dEnc[AXIS_X0];
		double dCurrY = pView->m_mgrProcedure->m_dEnc[AXIS_Y0];

		double pPos[2];
		pPos[0] = 0.0;
		pPos[1] = 0.0;

		double fLen, fVel, fAcc, fJerk;
		fLen = sqrt(((pPos[0] - dCurrX) * (pPos[0] - dCurrX)) + ((pPos[1] - dCurrY) * (pPos[1] - dCurrY)));
		if (fLen > 0.001)
		{
			pView->m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_X0, fLen, fVel, fAcc, fJerk);
			if (!pView->m_pMotion->Move(MS_X0Y0, pPos, fVel, fAcc, fAcc))
			{
				if (!pView->m_pMotion->Move(MS_X0Y0, pPos, fVel, fAcc, fAcc))
				{
					pView->ClrDispMsg();
					AfxMessageBox(_T("Move X0Y0 Error..."));
			}
		}
		}
		if (!pView->m_mgrProcedure->m_bTHREAD_MK[3])
		{
			pView->m_mgrProcedure->m_nStepMk[3] = 0;
			pView->m_mgrProcedure->m_nMkPcs[3] = 0;
			pView->m_mgrProcedure->m_bTHREAD_MK[3] = TRUE;
		}
		else
		{
			pView->m_mgrProcedure->m_bTHREAD_MK[3] = FALSE;
		}
	}
	else if (nCam == CAM_BOTH)
	{
		if (!pView->m_mgrProcedure->m_bTHREAD_MK[2])
		{
			pView->m_mgrProcedure->m_nStepMk[2] = 0;
			pView->m_mgrProcedure->m_nMkPcs[2] = 0;
			pView->m_mgrProcedure->m_bTHREAD_MK[2] = TRUE;
		}
		else
		{
			pView->m_mgrProcedure->m_bTHREAD_MK[2] = FALSE;
		}

		if (!pView->m_mgrProcedure->m_bTHREAD_MK[3])
		{
			pView->m_mgrProcedure->m_nStepMk[3] = 0;
			pView->m_mgrProcedure->m_nMkPcs[3] = 0;
			pView->m_mgrProcedure->m_bTHREAD_MK[3] = TRUE;
		}
		else
		{
			pView->m_mgrProcedure->m_bTHREAD_MK[3] = FALSE;
		}
	}
}


void CGvisR2R_PunchView::InitPLC()
{
#ifdef USE_MPE
	long lData;
	lData = (long)(_tstof(pDoc->WorkingInfo.Lot.sTotalReelDist) * 1000.0);
	m_pMpe->Write(_T("ML45000"), lData);	// 전체 Reel 길이 (단위 M * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Lot.sSeparateDist) * 1000.0);
	m_pMpe->Write(_T("ML45002"), lData);	// Lot 분리 길이 (단위 M * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Lot.sCuttingDist) * 1000.0);
	m_pMpe->Write(_T("ML45004"), lData);	// Lot 분리 후 절단위치 (단위 M * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Lot.sStopDist) * 1000.0);
	m_pMpe->Write(_T("ML45006"), lData);	// 일시정지 길이 (단위 M * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sAoiTq) * 1000.0);
	m_pMpe->Write(_T("ML45042"), lData);	// 검사부 Tension 모터 토크값 (단위 Kgf * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sMkTq) * 1000.0);
	m_pMpe->Write(_T("ML45044"), lData);	// 마킹부 Tension 모터 토크값 (단위 Kgf * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sEngraveTq) * 1000.0);
	m_pMpe->Write(_T("ML45050"), lData);	// 각인부 Tension 모터 토크값 (단위 Kgf * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen) * 1000.0);
	m_pMpe->Write(_T("ML45032"), lData);	// 한 판넬 길이 (단위 mm * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sMkFdVel) * 1000.0);
	m_pMpe->Write(_T("ML45034"), lData);	// 한 판넬 Feeding 속도 (단위 mm/sec * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sMkJogVel) * 1000.0);
	m_pMpe->Write(_T("ML45038"), lData);	// 연속공급 속도 (단위 mm/sec * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sAoiJogAcc) * 1000.0);
	m_pMpe->Write(_T("ML45040"), lData);	// 연속공급 가속도 (단위 mm/s^2 * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sMkFdAcc) * 1000.0);
	m_pMpe->Write(_T("ML45036"), lData);	// 한 판넬 Feeding 가속도 (단위 mm/s^2 * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sAoiFdLead) * 1000.0);
	m_pMpe->Write(_T("ML45012"), lData);	// 검사부 Feeding 롤러 Lead Pitch (단위 mm * 1000)
	m_pMpe->Write(_T("ML45020"), lData);	// 각인부 Feeding 롤러 Lead Pitch (단위 mm * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sMkFdLead) * 1000.0);
	m_pMpe->Write(_T("ML45014"), lData);	// 마킹부 Feeding 롤러 Lead Pitch (단위 mm * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sStBufPos) * 1000.0);
	pView->m_pMpe->Write(_T("ML45016"), lData);	// 버퍼 관련 설정 롤러 초기위치(단위 mm * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sFdMkAoiInitDist) * 1000.0);
	pView->m_pMpe->Write(_T("ML45008"), lData);	// AOI(하)에서 마킹까지 거리 (단위 mm * 1000)
	lData = (long)(_tstoi(pDoc->WorkingInfo.Motion.sFdAoiAoiDistShot) * 1000);
	pView->m_pMpe->Write(_T("ML45010"), lData);	// AOI(상)에서 AOI(하) Shot수 (단위 Shot수 * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sFdEngraveAoiInitDist) * 1000.0);
	pView->m_pMpe->Write(_T("ML45024"), lData);	// 각인부에서 AOI(상)까지 거리 (단위 mm * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sFdBarcodeOffsetVel) * 1000.0);
	m_pMpe->Write(_T("ML45060"), lData);	// 2D 바코드 리더기위치까지 Feeding 속도 (단위 mm/sec * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sFdBarcodeOffsetAcc) * 1000.0);
	m_pMpe->Write(_T("ML45062"), lData);	// 2D 바코드 리더기위치까지 Feeding 가속도 (단위 mm/s^2 * 1000)

	lData = (long)(_tstof(pDoc->WorkingInfo.LastJob.sUltraSonicCleannerStTim) * 100.0);
	m_pMpe->Write(_T("MW05940"), lData);	// AOI_Dn (단위 [초] * 100) : 1 is 10 mSec.
	m_pMpe->Write(_T("MW05942"), lData);	// AOI_Up (단위 [초] * 100) : 1 is 10 mSec.
#endif
}

BOOL CGvisR2R_PunchView::SetCollision(double dCollisionMargin)
{
	BOOL bRtn = FALSE;
	if (m_pMotion)
		bRtn = m_pMotion->SetCollision(dCollisionMargin);
	return bRtn;
}

//BOOL CGvisR2R_PunchView::ChkXmpCollision()
//{
//	CString sM;
//	long addressActPos1, addressActPos2, addressDifferenceStored;
//	m_pMotion->GetData(&addressActPos1, &addressActPos2, &addressDifferenceStored);
//	sM.Format(_T("%d, %d, %d"), (int)addressActPos1, (int)addressActPos2, (int)addressDifferenceStored);
//	//DispStsBar(sM);
//	return TRUE;
//}

void CGvisR2R_PunchView::MpeWrite()
{
	for (int i = TOT_M_IO - 1; i >= 0; i--)
	{
		if (pDoc->m_pIo[i].nIdx > -1)
		{
			if (m_pMpe)
				m_pMpe->Write(pDoc->m_pIo[i].sAddr, pDoc->m_pIo[i].lData);
			pDoc->m_pIo[i].nIdx = -1;
			pDoc->m_pIo[i].sAddr = _T("");
			pDoc->m_pIo[i].lData = 0;
		}
	}
}

void CGvisR2R_PunchView::IoWrite(CString sMReg, long lData)
{
	int i = 0;
	for (i = 0; i<TOT_M_IO; i++)
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
	BOOL bOn0 = (pDoc->m_pMpeIb[10] & (0x01 << 11)) ? TRUE : FALSE;		// 검사부 상 자동 운전 <-> X432B I/F
	BOOL bOn1 = (pDoc->m_pMpeIb[10] & (0x01 << 9)) ? TRUE : FALSE;		// 검사부 상 테이블 진공 완료 <-> X4329 I/F

	if (bOn0 && bOn1)
		return TRUE;
#endif
	return FALSE;
}

BOOL CGvisR2R_PunchView::IsRdyTest1()
{
#ifdef USE_MPE
	BOOL bOn0 = (pDoc->m_pMpeIb[14] & (0x01 << 11)) ? TRUE : FALSE;		// 검사부 하 자동 운전 <-> X442B I/F
	BOOL bOn1 = (pDoc->m_pMpeIb[14] & (0x01 << 9)) ? TRUE : FALSE;		// 검사부 하 테이블 진공 완료 <-> X4329 I/F
	double dCurPosMkFd = (double)pDoc->m_pMpeData[0][0];				// 마킹부 Feeding 엔코더 값(단위 mm )
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

BOOL CGvisR2R_PunchView::IsMk0Done()
{
	BOOL bDone = FALSE;

	if (pView->m_pVoiceCoil[0])
		bDone = pView->m_pVoiceCoil[0]->IsDoneMark(0);

	return bDone;
}

BOOL CGvisR2R_PunchView::IsMk1Done()
{
	BOOL bDone = FALSE;

	if (pView->m_pVoiceCoil[1])
		bDone = pView->m_pVoiceCoil[1]->IsDoneMark(1);

	return bDone;
}

void CGvisR2R_PunchView::SetPlcParam()
{
#ifdef USE_MPE
	if (!m_pMpe)
		return;

	//long lData;
	m_pMpe->Write(_T("ML45006"), long(_tstof(pDoc->WorkingInfo.LastJob.sTempPauseLen)*1000.0));	// 일시정지 길이 (단위 M * 1000)
	m_pMpe->Write(_T("ML45002"), long(_tstof(pDoc->WorkingInfo.LastJob.sLotSepLen)*1000.0));	// Lot 분리 길이 (단위 M * 1000)
	m_pMpe->Write(_T("ML45004"), long(_tstof(pDoc->WorkingInfo.LastJob.sLotCutPosLen)*1000.0));	// Lot 분리 후 절단위치 (단위 M * 1000)
	m_pMpe->Write(_T("ML45126"), (long)_tstoi(pDoc->WorkingInfo.LastJob.sSampleTestShotNum));	// 샘플검사 Shot수

	if (pDoc->WorkingInfo.LastJob.bTempPause)
	{
		m_pMpe->Write(_T("MB440183"), 1);	// 일시정지사용(PC가 On시키고, PLC가 확인하고 Off시킴)-20141031
		ChkTempStop(TRUE);
	}
	else
	{
		m_pMpe->Write(_T("MB440183"), 0);	// 일시정지사용(PC가 On시키고, PLC가 확인하고 Off시킴)-20141031
		ChkTempStop(FALSE);
	}

	if (pDoc->WorkingInfo.LastJob.bLotSep)
	{
		SetLotLastShot();
		m_pMpe->Write(_T("MB440184"), 1);	// 로트분리사용(PC가 On시키고, PC가 확인하고 Off시킴)-20141031
	}
	else
		m_pMpe->Write(_T("MB440184"), 0);	// 로트분리사용(PC가 On시키고, PC가 확인하고 Off시킴)-20141031

	m_pMpe->Write(_T("MB44017A"), (pDoc->WorkingInfo.LastJob.bDualTest) ? 0 : 1);		// 단면 검사 On
	m_pMpe->Write(_T("MB44017B"), (pDoc->WorkingInfo.LastJob.bSampleTest) ? 1 : 0);		// Sample 검사 On
	m_pMpe->Write(_T("MB44017D"), (pDoc->WorkingInfo.LastJob.bOneMetal) ? 1 : 0);		// One Metal On
	m_pMpe->Write(_T("MB44017C"), (pDoc->WorkingInfo.LastJob.bTwoMetal) ? 1 : 0);		// Two Metal On
	m_pMpe->Write(_T("MB44017E"), (pDoc->WorkingInfo.LastJob.bCore150Recoiler) ? 1 : 0);	// Recoiler Core 150mm On
	m_pMpe->Write(_T("MB44017F"), (pDoc->WorkingInfo.LastJob.bCore150Uncoiler) ? 1 : 0);	// Uncoiler Core 150mm On

	m_pMpe->Write(_T("MB44010E"), (pDoc->WorkingInfo.LastJob.bUseAoiUpCleanRoler ? 1 : 0));
	m_pMpe->Write(_T("MB44010F"), (pDoc->WorkingInfo.LastJob.bUseAoiDnCleanRoler ? 1 : 0));
#endif
}

void CGvisR2R_PunchView::GetPlcParam()
{
	if (!m_pMpe)
		return;
#ifdef USE_MPE
	// Main
	pDoc->BtnStatus.Main.Ready = m_pMpe->Read(_T("MB005503")) ? TRUE : FALSE;	// 마킹부 운전준비 스위치
	pDoc->BtnStatus.Main.Run = m_pMpe->Read(_T("MB005501")) ? TRUE : FALSE;		// 마킹부 운전 스위치
	pDoc->BtnStatus.Main.Reset = m_pMpe->Read(_T("MB005504")) ? TRUE : FALSE;	// 마킹부 리셋 스위치
	pDoc->BtnStatus.Main.Stop = m_pMpe->Read(_T("MB005502")) ? TRUE : FALSE;	// 마킹부 정지 스위치
	//pDoc->BtnStatus.Main.Auto = m_pMpe->Read(_T("MB005505")) ? TRUE : FALSE;	// 마킹부 자동/수동 (ON)
	//pDoc->BtnStatus.Main.Manual = m_pMpe->Read(_T("MB005505")) ? FALSE : TRUE;	// 마킹부 자동/수동 (OFF)
	pDoc->BtnStatus.Main.Auto = m_pMpe->Read(_T("MB003705")) ? TRUE : FALSE;	// 마킹부 자동 상태 스위치 램프
	pDoc->BtnStatus.Main.Manual = m_pMpe->Read(_T("MB003705")) ? FALSE : TRUE;	// 마킹부 자동 상태 스위치 램프

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
	pDoc->BtnStatus.Rc.Relation = m_pMpe->Read(_T("MB004001")) ? TRUE : FALSE;		// 리코일러 연동 온/오프 스위치 램프 
	pDoc->BtnStatus.Rc.FdCw = m_pMpe->Read(_T("MB00400C")) ? TRUE : FALSE;			// 리코일러 제품휠 정회전 스위치 램프
	pDoc->BtnStatus.Rc.FdCcw = m_pMpe->Read(_T("MB00400D")) ? TRUE : FALSE;			// 리코일러 제품휠 역회전 스위치 램프
	pDoc->BtnStatus.Rc.ReelChuck = m_pMpe->Read(_T("MB00400B")) ? TRUE : FALSE;		// 리코일러 제품척 클램프 스위치 램프
	pDoc->BtnStatus.Rc.DcRlUpDn = m_pMpe->Read(_T("MB004002")) ? TRUE : FALSE;		// 리코일러 댄서롤 상승/하강 스위치 램프
	pDoc->BtnStatus.Rc.ReelJoinL = m_pMpe->Read(_T("MB004005")) ? TRUE : FALSE;		// 리코일러 제품 이음매(상/좌) 스위치 램프
	pDoc->BtnStatus.Rc.ReelJoinR = m_pMpe->Read(_T("MB004006")) ? TRUE : FALSE;		// 리코일러 제품 이음매(하/우) 스위치 램프
	pDoc->BtnStatus.Rc.ReelJoinVac = m_pMpe->Read(_T("MB00400F")) ? TRUE : FALSE;	// 리코일러 제품 이음매 진공 스위치 램프
	pDoc->BtnStatus.Rc.PprChuck = m_pMpe->Read(_T("MB004008")) ? TRUE : FALSE;		// 리코일러 간지척 클램프 스위치 램프
	pDoc->BtnStatus.Rc.PprCw = m_pMpe->Read(_T("MB004009")) ? TRUE : FALSE;			// 리코일러 간지휠 정회전 스위치 램프
	pDoc->BtnStatus.Rc.PprCcw = m_pMpe->Read(_T("MB00400A")) ? TRUE : FALSE;		// 리코일러 간지휠 역회전 스위치 램프
	pDoc->BtnStatus.Rc.Rewine = m_pMpe->Read(_T("MB004003")) ? TRUE : FALSE;		// 리코일러 Rewinder 동작 스위치 램프
	pDoc->BtnStatus.Rc.RewineReelPpr = m_pMpe->Read(_T("MB004004")) ? TRUE : FALSE;	// 리코일러 Rewinder 제품 & 간지 스위치 램프

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
	pDoc->BtnStatus.Mk.Relation = m_pMpe->Read(_T("MB003711")) ? TRUE : FALSE;	// 마킹부 연동 온/오프 스위치 램프
	pDoc->BtnStatus.Mk.FdCw = m_pMpe->Read(_T("MB003713")) ? TRUE : FALSE;		// 마킹부 피딩 정회전 스위치 램프
	pDoc->BtnStatus.Mk.FdCcw = m_pMpe->Read(_T("MB003714")) ? TRUE : FALSE;		// 마킹부 피딩 역회전 스위치 램프
	pDoc->BtnStatus.Mk.FdVac = m_pMpe->Read(_T("MB003715")) ? TRUE : FALSE;		// 마킹부 피딩 진공 스위치 램프
	pDoc->BtnStatus.Mk.PushUp = m_pMpe->Read(_T("MB003712")) ? TRUE : FALSE;	// 마킹부 테이블 브로워 스위치 램프
	pDoc->BtnStatus.Mk.TblBlw = m_pMpe->Read(_T("MB003717")) ? TRUE : FALSE;	// 마킹부 테이블 진공 스위치 램프
	pDoc->BtnStatus.Mk.TblVac = m_pMpe->Read(_T("MB003716")) ? TRUE : FALSE;	// 마킹부 토크 진공 스위치 램프
	pDoc->BtnStatus.Mk.FdClp = m_pMpe->Read(_T("MB003719")) ? TRUE : FALSE;		// 마킹부 피딩 클램프 스위치 램프  
	pDoc->BtnStatus.Mk.TqClp = m_pMpe->Read(_T("MB00371A")) ? TRUE : FALSE;		// 마킹부 텐션 클램프 스위치 램프
	pDoc->BtnStatus.Mk.MvOne = m_pMpe->Read(_T("MB440151")) ? TRUE : FALSE;
	pDoc->BtnStatus.Mk.LsrPt = m_pMpe->Read(_T("MB003718")) ? TRUE : FALSE;		// 마킹부 레이져 포인터 스위치 램프
	pDoc->BtnStatus.Mk.DcRSol = m_pMpe->Read(_T("MB00371B")) ? TRUE : FALSE;	// 마킹부 댄서롤 상승/하강 스위치 램프

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
	pDoc->BtnStatus.AoiDn.Relation = m_pMpe->Read(_T("MB003901")) ? TRUE : FALSE;	// 검사부 하 연동 온/오프 스위치 램프
	pDoc->BtnStatus.AoiDn.FdCw = m_pMpe->Read(_T("MB003903")) ? TRUE : FALSE;		// 검사부 하 피딩 정회전 스위치 램프
	pDoc->BtnStatus.AoiDn.FdCcw = m_pMpe->Read(_T("MB003904")) ? TRUE : FALSE;		// 검사부 하 피딩 역회전 스위치 램프
	pDoc->BtnStatus.AoiDn.FdVac = m_pMpe->Read(_T("MB003905")) ? TRUE : FALSE;		// 검사부 하 피딩 진공 스위치 램프
	pDoc->BtnStatus.AoiDn.PushUp = m_pMpe->Read(_T("MB003906")) ? TRUE : FALSE;		// 검사부 하 토크 진공 스위치 램프
	pDoc->BtnStatus.AoiDn.TblBlw = m_pMpe->Read(_T("MB003902")) ? TRUE : FALSE;		// 검사부 하 테이블 브로워 스위치 램프
	pDoc->BtnStatus.AoiDn.TblVac = m_pMpe->Read(_T("MB003907")) ? TRUE : FALSE;		// 검사부 하 테이블 진공 스위치 램프
	pDoc->BtnStatus.AoiDn.FdClp = m_pMpe->Read(_T("MB003909")) ? TRUE : FALSE;		// 검사부 하 피딩 클램프 스위치 램프
	pDoc->BtnStatus.AoiDn.TqClp = m_pMpe->Read(_T("MB00390A")) ? TRUE : FALSE;		// 검사부 하 텐션 클램프 스위치 램프
	pDoc->BtnStatus.AoiDn.MvOne = m_pMpe->Read(_T("MB440151")) ? TRUE : FALSE;
	pDoc->BtnStatus.AoiDn.LsrPt = m_pMpe->Read(_T("MB003908")) ? TRUE : FALSE;		// 검사부 하 레이져 포인터 스위치 램프
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
	pDoc->BtnStatus.AoiUp.Relation = m_pMpe->Read(_T("MB003801")) ? TRUE : FALSE;	// 검사부 상 연동 온/오프 스위치 램프
	pDoc->BtnStatus.AoiUp.FdCw = m_pMpe->Read(_T("MB003803")) ? TRUE : FALSE;		// 검사부 상 피딩 정회전 스위치 램프
	pDoc->BtnStatus.AoiUp.FdCcw = m_pMpe->Read(_T("MB003804")) ? TRUE : FALSE;		// 검사부 상 피딩 역회전 스위치 램프
	pDoc->BtnStatus.AoiUp.FdVac = m_pMpe->Read(_T("MB003805")) ? TRUE : FALSE;		// 검사부 상 피딩 진공 스위치 램프
	pDoc->BtnStatus.AoiUp.PushUp = m_pMpe->Read(_T("MB003806")) ? TRUE : FALSE;		// 검사부 상 토크 진공 스위치 램프
	pDoc->BtnStatus.AoiUp.TblBlw = m_pMpe->Read(_T("MB003802")) ? TRUE : FALSE;		// 검사부 상 테이블 브로워 스위치 램프
	pDoc->BtnStatus.AoiUp.TblVac = m_pMpe->Read(_T("MB003807")) ? TRUE : FALSE;		// 검사부 상 테이블 진공 스위치 램프
	pDoc->BtnStatus.AoiUp.FdClp = m_pMpe->Read(_T("MB003809")) ? TRUE : FALSE;		// 검사부 상 피딩 클램프 스위치 램프
	pDoc->BtnStatus.AoiUp.TqClp = m_pMpe->Read(_T("MB00380A")) ? TRUE : FALSE;		// 검사부 상 텐션 클램프 스위치 램프
	pDoc->BtnStatus.AoiUp.MvOne = m_pMpe->Read(_T("MB440151")) ? TRUE : FALSE;
	pDoc->BtnStatus.AoiUp.LsrPt = m_pMpe->Read(_T("MB003808")) ? TRUE : FALSE;		// 검사부 상 레이져 포인터 스위치 램프

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
	pDoc->BtnStatus.Uc.Relation = m_pMpe->Read(_T("MB003601")) ? TRUE : FALSE;	// 언코일러 연동 온/오프 스위치 램프
	pDoc->BtnStatus.Uc.FdCw = m_pMpe->Read(_T("MB00360C")) ? TRUE : FALSE;	// 언코일러 제품휠 정회전 스위치 램프
	pDoc->BtnStatus.Uc.FdCcw = m_pMpe->Read(_T("MB00360D")) ? TRUE : FALSE;	// 언코일러 제품휠 역회전 스위치 램프
	pDoc->BtnStatus.Uc.ReelChuck = m_pMpe->Read(_T("MB00360B")) ? TRUE : FALSE;	// 언코일러 제품척 클램프 스위치 램프
	pDoc->BtnStatus.Uc.DcRlUpDn = m_pMpe->Read(_T("MB003602")) ? TRUE : FALSE;	// 언코일러 댄서롤 상승/하강 스위치 램프
	pDoc->BtnStatus.Uc.ReelJoinL = m_pMpe->Read(_T("MB003605")) ? TRUE : FALSE;	// 언코일러 제품 이음매(상/좌) 스위치 램프
	pDoc->BtnStatus.Uc.ReelJoinR = m_pMpe->Read(_T("MB003606")) ? TRUE : FALSE;	// 언코일러 제품 이음매(하/우) 스위치 램프
	pDoc->BtnStatus.Uc.ReelJoinVac = m_pMpe->Read(_T("MB00360F")) ? TRUE : FALSE;	// 언코일러 제품 이음매 진공 스위치 램프
	pDoc->BtnStatus.Uc.PprChuck = m_pMpe->Read(_T("MB003608")) ? TRUE : FALSE;	// 언코일러 간지척 클램프 스위치 램프
	pDoc->BtnStatus.Uc.PprCw = m_pMpe->Read(_T("MB003609")) ? TRUE : FALSE;	// 언코일러 간지휠 정회전 스위치 램프
	pDoc->BtnStatus.Uc.PprCcw = m_pMpe->Read(_T("MB00360A")) ? TRUE : FALSE;	// 언코일러 간지휠 역회전 스위치 램프
	pDoc->BtnStatus.Uc.ClRlUpDn = m_pMpe->Read(_T("MB003603")) ? TRUE : FALSE;	// 언코일러 클린롤러 상승/하강 스위치 램프
	pDoc->BtnStatus.Uc.ClRlPshUpDn = m_pMpe->Read(_T("MB003604")) ? TRUE : FALSE;	// 언코일러 클린롤러누름 상승/하강 스위치 램프

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

void CGvisR2R_PunchView::InitIoWrite()
{
#ifdef USE_MPE
	if (!m_pMpe)
		return;

	//IoWrite(_T("MB44015E"), 0); // 부저1 On  (PC가 ON, OFF) - 20141020
	m_pMpe->Write(_T("MB44015E"), 0);
	//IoWrite(_T("MB44015F"), 0); // 부저2 On  (PC가 ON, OFF) - 20141020
	m_pMpe->Write(_T("MB44015F"), 0);

	//IoWrite(_T("MB003828"), 0); // 검사부 상 검사 시작 <-> Y4368 I/F
	m_pMpe->Write(_T("MB003828"), 0);
	//IoWrite(_T("MB003829"), 0); // 검사부 상 검사 테이블 진공 SOL <-> Y4369 I/F
	m_pMpe->Write(_T("MB003829"), 0);
	//IoWrite(_T("MB00382A"), 0); // 검사부 상 Reset <-> Y436A I/F
	m_pMpe->Write(_T("MB00382A"), 0);
	//IoWrite(_T("MB00382B"), 0); // 마킹부 Lot End <-> Y436B I/F
	m_pMpe->Write(_T("MB00382B"), 0);

	//IoWrite(_T("MB003928"), 0); // 검사부 하 검사 시작 <-> Y4468 I/F
	m_pMpe->Write(_T("MB003928"), 0);
	//IoWrite(_T("MB003929"), 0); // 검사부 하 검사 테이블 진공 SOL <-> Y4369 I/F
	m_pMpe->Write(_T("MB003929"), 0);
	//IoWrite(_T("MB00392A"), 0); // 검사부 하 Reset <-> Y436A I/F
	m_pMpe->Write(_T("MB00392A"), 0);
	//IoWrite(_T("MB00392B"), 0); // 마킹부 Lot End <-> Y436B I/F
	m_pMpe->Write(_T("MB00392B"), 0);

	//IoWrite(_T("MB44015D"), 0); // 자동 초기 운전상태(PC가 On/Off 시킴, PLC가 운전램프를 윙크동작, on->off시 운전램프 on, 다시 운전스위치가 눌러지면 off) - 20141017
	m_pMpe->Write(_T("MB44015D"), 0);
	//IoWrite(_T("ML45064"), 0); // 검사부 Feeding 롤러 Offset(*1000, +:더 보냄, -덜 보냄, PC가 쓰고 PLC에서 지움)
	m_pMpe->Write(_T("ML45064"), 0);
	//IoWrite(_T("ML45066"), 0); // 마킹부 Feeding 롤러 Offset(*1000, +:더 보냄, -덜 보냄, PC가 쓰고 PLC에서 지움)
	m_pMpe->Write(_T("ML45066"), 0);

	//IoWrite(_T("MB600000"), 0); // PC가 PLC의 Alarm 발생여부를 확인
	m_pMpe->Write(_T("MB600000"), 0);
	//IoWrite(_T("MB600008"), 0); // PC가 PLC의 Alarm 발생여부를 확인
	m_pMpe->Write(_T("MB600008"), 0);
	//IoWrite(_T("ML60002"), 0); // 알람이 발생한 페이지 지정(PLC가 표시 할 알람의 페이지를 가리킴).
	m_pMpe->Write(_T("ML60002"), 0);
#endif
}


BOOL CGvisR2R_PunchView::MoveAlign0(int nPos)
{
	if (!m_pMotion)
		return FALSE;

	if (m_pDlgMenu02)
		m_pDlgMenu02->SetLight();

	if (m_pMotion->m_dPinPosY[0] > 0.0 && m_pMotion->m_dPinPosX[0] > 0.0)
	{
		double dCurrX = pView->m_mgrProcedure->m_dEnc[AXIS_X0];
		double dCurrY = pView->m_mgrProcedure->m_dEnc[AXIS_Y0];

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

		if (pView->m_mgrProcedure->ChkCollision(AXIS_X0, pPos[0]))
			return FALSE;

		double fLen, fVel, fAcc, fJerk;
		fLen = sqrt(((pPos[0] - dCurrX) * (pPos[0] - dCurrX)) + ((pPos[1] - dCurrY) * (pPos[1] - dCurrY)));
		if (fLen > 0.001)
		{
			pView->m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_X0, fLen, fVel, fAcc, fJerk);
			if (!pView->m_pMotion->Move(MS_X0Y0, pPos, fVel, fAcc, fAcc, ABS, NO_WAIT))
			{
				if (!pView->m_pMotion->Move(MS_X0Y0, pPos, fVel, fAcc, fAcc, ABS, NO_WAIT))
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

BOOL CGvisR2R_PunchView::MoveAlign1(int nPos)
{
	if (!m_pMotion)
		return FALSE;

	if (m_pDlgMenu02)
		m_pDlgMenu02->SetLight2();

	if (m_pMotion->m_dPinPosY[1] > 0.0 && m_pMotion->m_dPinPosX[1] > 0.0)
	{
		double dCurrX = pView->m_mgrProcedure->m_dEnc[AXIS_X1];
		double dCurrY = pView->m_mgrProcedure->m_dEnc[AXIS_Y1];

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

		if (pView->m_mgrProcedure->ChkCollision(AXIS_X1, pPos[0]))
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
	if (pDoc->m_pMpeSignal[3] & (0x01 << 0))		// 각인부 2D Leading 작업완료(PLC가 ON/OFF)
	{
		if (m_pDlgMenu03)
			m_pDlgMenu03->SetLed(0, TRUE);
	}
	else
	{
		if (m_pDlgMenu03)
			m_pDlgMenu03->SetLed(0, FALSE);
	}

	if (pDoc->m_pMpeSignal[3] & (0x01 << 2))		// 각인부 Laser 작업완료(PLC가 ON/OFF)
	{
		if (m_pDlgMenu03)
			m_pDlgMenu03->SetLed(1, TRUE);
	}
	else
	{
		if (m_pDlgMenu03)
			m_pDlgMenu03->SetLed(1, FALSE);
	}

	if (pDoc->m_pMpeSignal[3] & (0x01 << 3))		// 검사부 상면 검사 작업완료(PLC가 ON/OFF)
	{
		if (m_pDlgMenu03)
			m_pDlgMenu03->SetLed(2, TRUE);
	}
	else
	{
		if (m_pDlgMenu03)
			m_pDlgMenu03->SetLed(2, FALSE);
	}

	if (pDoc->m_pMpeSignal[3] & (0x01 << 4))		// 검사부 하면 검사 작업완료(PLC가 ON/OFF)
	{
		if (m_pDlgMenu03)
			m_pDlgMenu03->SetLed(3, TRUE);
	}
	else
	{
		if (m_pDlgMenu03)
			m_pDlgMenu03->SetLed(3, FALSE);
	}

	if (pDoc->m_pMpeSignal[3] & (0x01 << 5))		// 마킹부 마킹 작업완료(PLC가 ON/OFF)
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

void CGvisR2R_PunchView::MonDispMain()
{
	BOOL bDispStop = TRUE;

#ifdef USE_MPE
	if (pDoc->m_pMpeSignal[2] & (0x01 << 0))		// 운전중(PLC가 PC에 알려주는 설비 상태) - 20141031
	{
		if (pView->m_mgrProcedure->m_sDispMain != _T("운전중"))
		{
			DispMain(_T("운전중"), RGB_GREEN);	
			pDoc->SetMkMenu03(_T("Main"), _T("Run"), TRUE);
			pDoc->SetMkMenu03(_T("Main"), _T("Stop"), FALSE);
		}
		else
		{
		}
	}

	if (pDoc->m_pMpeSignal[2] & (0x01 << 2))		// 운전준비(PLC가 PC에 알려주는 설비 상태) - 20141031
	{
		bDispStop = FALSE;
		//if(!WatiDispMain(10))
		{
			if (pView->m_mgrProcedure->m_sDispMain != _T("운전준비"))
			{
				DispMain(_T("운전준비"), RGB_GREEN);
				pDoc->SetMkMenu03(_T("Main"), _T("Ready"), TRUE);
			}
		}
	}
	else
	{
		if (pDoc->m_pMpeSignal[2] & (0x01 << 3))		// 초기운전(PLC가 PC에 알려주는 설비 상태) - 20141031
		{
			bDispStop = FALSE;
			//if(!WatiDispMain(10))
			{
				if (pDoc->WorkingInfo.LastJob.bSampleTest)
				{
					if (pDoc->WorkingInfo.LastJob.bDualTest)
					{
						if (pView->m_mgrProcedure->m_sDispMain != _T("양면샘플"))
						{
							DispMain(_T("양면샘플"), RGB_GREEN);
						}
					}
					else
					{
						if (pView->m_mgrProcedure->m_sDispMain != _T("단면샘플"))
						{
							DispMain(_T("단면샘플"), RGB_GREEN);
						}
					}
				}
				else if (pDoc->WorkingInfo.LastJob.bDualTest)
				{
					if (pView->m_mgrProcedure->m_sDispMain != _T("양면검사"))
					{
						DispMain(_T("양면검사"), RGB_GREEN);
					}
				}
				else
				{
					if (pView->m_mgrProcedure->m_sDispMain != _T("단면검사"))
					{
						DispMain(_T("단면검사"), RGB_GREEN);
					}
				}
			}
		}
		else
		{
			if (pView->m_mgrProcedure->m_sDispMain != _T("운전준비"))
			{
				bDispStop = TRUE;
			}
			else
			{
				bDispStop = FALSE;
			}
		}
	}


	if (pDoc->m_pMpeSignal[2] & (0x01 << 1))		// 정지(PLC가 PC에 알려주는 설비 상태) - 20141031
	{
		if (bDispStop)
		{
			if (pView->m_mgrProcedure->m_sDispMain != _T("정 지"))
			{
				DispMain(_T("정 지"), RGB_RED);
				pDoc->SetMkMenu03(_T("Main"), _T("Stop"), TRUE);
				pDoc->SetMkMenu03(_T("Main"), _T("Run"), FALSE);
			}
		}
	}
#endif
}

void CGvisR2R_PunchView::MonPlcAlm()
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

void CGvisR2R_PunchView::PlcAlm(BOOL bMon, BOOL bClr)
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
		m_pMpe->Write(_T("MB600008"), 1);
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
				if(pView->m_pEngrave)
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
		m_pMpe->Write(_T("MB600009"), 1);

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

void CGvisR2R_PunchView::FindAlarm()
{
	// 알람이 발생한 페이지
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
	for (int i = 0; i<32; i++)
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

void CGvisR2R_PunchView::ResetMonAlm()
{
	m_pMpe->Write(_T("MB600008"), 0);
}

void CGvisR2R_PunchView::ClrAlarm()
{
	if (!pDoc->m_sAlmMsg.IsEmpty())
	{
		pDoc->m_sAlmMsg = _T("");
		pDoc->m_sIsAlmMsg = _T("");
		pDoc->m_sPrevAlmMsg = _T("");
	}
}

void CGvisR2R_PunchView::ResetClear()
{
	m_pMpe->Write(_T("MB600009"), 0);
}

void CGvisR2R_PunchView::ChkTempStop(BOOL bChk)
{
	if (bChk)
	{
		if (!pView->m_mgrProcedure->m_bTIM_CHK_TEMP_STOP)
		{
			pView->m_mgrProcedure->m_bTIM_CHK_TEMP_STOP = TRUE;
			SetTimer(TIM_CHK_TEMP_STOP, 500, NULL);
		}
	}
	else
	{
		pView->m_mgrProcedure->m_bTIM_CHK_TEMP_STOP = FALSE;
	}
}

void CGvisR2R_PunchView::ChgLot()
{
	pDoc->WorkingInfo.LastJob.sLotUp = pDoc->Status.PcrShare[0].sLot;
	pDoc->SetModelInfoUp();

	pDoc->WorkingInfo.LastJob.sLotDn = pDoc->Status.PcrShare[1].sLot;
	pDoc->SetModelInfoDn();

	//	pDoc->m_bDoneChgLot = TRUE;

	pView->m_mgrReelmap->SetPathAtBuf();
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

void CGvisR2R_PunchView::CycleStop()
{
	pView->m_mgrProcedure->m_bCycleStop = TRUE;
}

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
			pView->m_pMpe->Write(_T("MB440184"), 0);	// 로트분리사용(PC가 On시키고, PC가 확인하고 Off시킴)-20141031
#endif
			if (pView->m_mgrReelmap->m_pReelMap)
				pView->m_mgrReelmap->m_pReelMap->m_bUseLotSep = FALSE;

			::WritePrivateProfileString(_T("Last Job"), _T("Use Lot seperate"), _T("0"), PATH_WORKING_INFO);

			if (m_pDlgMenu01)
				m_pDlgMenu01->UpdateData();

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CGvisR2R_PunchView::ChkStShotNum()
{
	CString sMsg;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pDoc->m_ListBuf[0].nTot == 0)
	{
		if (pView->m_mgrProcedure->m_nShareUpS > 0 && !(pView->m_mgrProcedure->m_nShareUpS % 2))
		{
			sMsg.Format(_T("AOI 상면의 시리얼이 짝수로 시작하였습니다.\r\n- 시리얼 번호: %d"), pView->m_mgrProcedure->m_nShareUpS);
			MsgBox(sMsg);
			return FALSE;
		}
	}

	if (bDualTest)
	{
		if (pDoc->m_ListBuf[1].nTot == 0)
		{
			if (pView->m_mgrProcedure->m_nShareDnS > 0 && !(pView->m_mgrProcedure->m_nShareDnS % 2))
			{
				sMsg.Format(_T("AOI 하면의 시리얼이 짝수로 시작하였습니다.\r\n- 시리얼 번호: %d"), pView->m_mgrProcedure->m_nShareDnS);
				MsgBox(sMsg);
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL CGvisR2R_PunchView::ChkContShotNum()
{
	CString sMsg;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (!m_pDlgFrameHigh)
		return FALSE;

	if (pDoc->m_ListBuf[0].nTot == 0)
	{
		if (pView->m_mgrProcedure->m_nShareUpS > 0 && m_pDlgFrameHigh->m_nMkLastShot + 1 != pView->m_mgrProcedure->m_nShareUpS)
		{
			sMsg.Format(_T("AOI 상면의 시작Shot(%d)이 마지막Shot(%d)과 불연속입니다.\r\n계속 진행하시겠습니까?"), pView->m_mgrProcedure->m_nShareUpS, m_pDlgFrameHigh->m_nMkLastShot);
			if (IDNO == MsgBox(sMsg, 0, MB_YESNO))
				return FALSE;
		}
	}

	return TRUE;
}

void CGvisR2R_PunchView::SetFixPcs(int nSerial)
{
	pView->m_mgrReelmap->SetFixPcs(nSerial);
}

BOOL CGvisR2R_PunchView::DoElecChk(CString &sRst)
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

BOOL CGvisR2R_PunchView::MoveMeasPos(int nId)
{
	if (!m_pMotion)
		return FALSE;

	if (!m_pLight)
		return FALSE;

	if (nId == 0)
	{
		if (m_pDlgMenu02)
			m_pDlgMenu02->SetLight();

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
			pPos[0] = _tstof(pDoc->WorkingInfo.Probing[nId].sMeasurePosX) + dMkOffsetX - m_pDlgMenu02->m_dMkFdOffsetX[0][0];
			pPos[1] = _tstof(pDoc->WorkingInfo.Probing[nId].sMeasurePosY) + dMkOffsetY - m_pDlgMenu02->m_dMkFdOffsetY[0][0];
		}

		if (pPos[1] > 0.0 && pPos[0] > 0.0)
		{
			double dCurrX = pView->m_mgrProcedure->m_dEnc[AXIS_X0];
			double dCurrY = pView->m_mgrProcedure->m_dEnc[AXIS_Y0];

			if (pView->m_mgrProcedure->ChkCollision(AXIS_X0, pPos[0]))
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
		if (m_pDlgMenu02)
			m_pDlgMenu02->SetLight2();

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
			pPos[0] = _tstof(pDoc->WorkingInfo.Probing[nId].sMeasurePosX) + dMkOffsetX - m_pDlgMenu02->m_dMkFdOffsetX[1][0];
			pPos[1] = _tstof(pDoc->WorkingInfo.Probing[nId].sMeasurePosY) + dMkOffsetY - m_pDlgMenu02->m_dMkFdOffsetY[1][0];
		}

		if (pPos[1] > 0.0 && pPos[0] > 0.0)
		{
			double dCurrX = pView->m_mgrProcedure->m_dEnc[AXIS_X1];
			double dCurrY = pView->m_mgrProcedure->m_dEnc[AXIS_Y1];

			if (pView->m_mgrProcedure->ChkCollision(AXIS_X1, pPos[0]))
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

void CGvisR2R_PunchView::SetReject()
{
	CfPoint ptPnt;

	if (pView->m_mgrProcedure->m_bDoMk[0])
	{
		if (!pView->m_mgrProcedure->m_bTHREAD_MK[2])
		{
			pView->m_mgrProcedure->m_bDoneMk[0] = FALSE;
			pView->m_mgrProcedure->m_nStepMk[2] = 0;
			pView->m_mgrProcedure->m_nMkPcs[2] = 0;
			pView->m_mgrProcedure->m_bTHREAD_MK[2] = TRUE;
		}
	}

	if (pView->m_mgrProcedure->m_bDoMk[1])
	{
		if (!pView->m_mgrProcedure->m_bTHREAD_MK[3])
		{
			pView->m_mgrProcedure->m_bDoneMk[1] = FALSE;
			pView->m_mgrProcedure->m_nStepMk[3] = 0;
			pView->m_mgrProcedure->m_nMkPcs[3] = 0;
			pView->m_mgrProcedure->m_bTHREAD_MK[3] = TRUE;
		}
	}
}

void CGvisR2R_PunchView::DoInterlock()
{
	if (pView->m_mgrProcedure->m_dEnc[AXIS_Y0] < 20.0 && pView->m_mgrProcedure->m_dEnc[AXIS_Y1] < 20.0)
	{
		if (pView->m_mgrProcedure->m_bStopFeeding)
		{
			pView->m_mgrProcedure->m_bStopFeeding = FALSE;
			m_pMpe->Write(_T("MB440115"), 0); // 마킹부Feeding금지
		}
	}
	else
	{
		if (!pView->m_mgrProcedure->m_bStopFeeding)
		{
			pView->m_mgrProcedure->m_bStopFeeding = TRUE;
			m_pMpe->Write(_T("MB440115"), 1); // 마킹부Feeding금지
		}
	}
}

BOOL CGvisR2R_PunchView::ChkLightErr()
{
	int nSerial, nErrCode;
	BOOL bError = FALSE;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Cam0

	if (pView->m_mgrProcedure->m_bSerialDecrese)
	{
		if (nSerial > 0 && nSerial >= pView->m_mgrProcedure->m_nLotEndSerial)
		{
			if ((nErrCode = GetErrCode(nSerial)) != 1)
			{
				if (nErrCode == -1) // -1(Align Error, 노광불량)
					bError = TRUE;
			}
		}

		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

		if (nSerial > 0 && nSerial >= pView->m_mgrProcedure->m_nLotEndSerial)
		{
			if ((nErrCode = GetErrCode(nSerial)) != 1)
			{
				if (nErrCode == -1) // -1(Align Error, 노광불량)
					bError = TRUE;
			}
		}
	}
	else
	{
		if (nSerial > 0 && nSerial <= pView->m_mgrProcedure->m_nLotEndSerial)
		{
			if ((nErrCode = GetErrCode(nSerial)) != 1)
			{
				if (nErrCode == -1) // -1(Align Error, 노광불량)
					bError = TRUE;
			}
		}

		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

		if (nSerial > 0 && nSerial <= pView->m_mgrProcedure->m_nLotEndSerial)
		{
			if ((nErrCode = GetErrCode(nSerial)) != 1)
			{
				if (nErrCode == -1) // -1(Align Error, 노광불량)
					bError = TRUE;
			}
		}
	}

	if (bError)
	{
		Stop();
		TowerLamp(RGB_RED, TRUE);
		Buzzer(TRUE, 0);
		DispMain(_T("정 지"), RGB_RED);
	}

	return bError;
}

void CGvisR2R_PunchView::CntMk()
{
#ifdef USE_MPE
	if (pView->m_mgrProcedure->m_nPrevTotMk[0] != pView->m_mgrProcedure->m_nTotMk[0])
	{
		pView->m_mgrProcedure->m_nPrevTotMk[0] = pView->m_mgrProcedure->m_nTotMk[0];
		pView->m_pMpe->Write(_T("ML45096"), (long)pView->m_mgrProcedure->m_nTotMk[0]);	// 마킹부 (좌) 총 마킹수 
	}
	if (pView->m_mgrProcedure->m_nPrevCurMk[0] != pView->m_mgrProcedure->m_nMkPcs[0])//m_nCurMk[0])
	{
		pView->m_mgrProcedure->m_nPrevCurMk[0] = pView->m_mgrProcedure->m_nMkPcs[0];//m_nCurMk[0];
		pView->m_pMpe->Write(_T("ML45098"), (long)pView->m_mgrProcedure->m_nMkPcs[0]);	// 마킹부 (좌) 현재 마킹한 수
	}

	if (pView->m_mgrProcedure->m_nPrevTotMk[1] != pView->m_mgrProcedure->m_nTotMk[1])
	{
		pView->m_mgrProcedure->m_nPrevTotMk[1] = pView->m_mgrProcedure->m_nTotMk[1];
		pView->m_pMpe->Write(_T("ML45100"), (long)pView->m_mgrProcedure->m_nTotMk[1]);	// 마킹부 (우) 총 마킹수 
	}
	if (pView->m_mgrProcedure->m_nPrevCurMk[1] != pView->m_mgrProcedure->m_nMkPcs[1])//m_nCurMk[1])
	{
		pView->m_mgrProcedure->m_nPrevCurMk[1] = pView->m_mgrProcedure->m_nMkPcs[1];//m_nCurMk[1];
		pView->m_pMpe->Write(_T("ML45102"), (long)pView->m_mgrProcedure->m_nMkPcs[1]);	// 마킹부 (우) 현재 마킹한 수
	}
#endif
}

BOOL CGvisR2R_PunchView::IsOnMarking0()
{
	if (pView->m_mgrProcedure->m_nMkPcs[0] < pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)	// 마킹완료Check
		return TRUE;

	return FALSE;
}

BOOL CGvisR2R_PunchView::IsOnMarking1()
{
	if (pView->m_mgrProcedure->m_nMkPcs[1] < pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs)	// 마킹완료Check
		return TRUE;

	return FALSE;
}

void CGvisR2R_PunchView::SetDualTest(BOOL bOn)
{
#ifdef USE_MPE
	if (pView->m_pMpe)
		pView->m_pMpe->Write(_T("MB44017A"), bOn ? 0 : 1);		// 단면 검사 On
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
			m_pMpe->Write(_T("MB44017C"), 1);
			::WritePrivateProfileString(_T("Last Job"), _T("Two Metal On"), _T("1"), PATH_WORKING_INFO);// IDC_CHK_TWO_METAL - Uncoiler\r역방향 ON : TRUE	
		}
		else
		{
			pDoc->WorkingInfo.LastJob.bTwoMetal = FALSE;
			m_pMpe->Write(_T("MB44017C"), 0);
			::WritePrivateProfileString(_T("Last Job"), _T("Two Metal On"), _T("0"), PATH_WORKING_INFO);// IDC_CHK_TWO_METAL - Uncoiler\r정방향 ON : TRUE	
		}

//#ifdef USE_ENGRAVE
//		if (pView && pView->m_pEngrave)
//			pView->m_pEngrave->SetUncoilerCcw();	//_stSigInx::_UncoilerCcw
//#endif
	}
	else
	{
		if (bOn)
		{
			pDoc->WorkingInfo.LastJob.bOneMetal = TRUE;
			m_pMpe->Write(_T("MB44017D"), 1);
			::WritePrivateProfileString(_T("Last Job"), _T("One Metal On"), _T("1"), PATH_WORKING_INFO);// IDC_CHK_ONE_METAL - Recoiler\r정방향 CW : FALSE
		}
		else
		{
			pDoc->WorkingInfo.LastJob.bOneMetal = FALSE;
			m_pMpe->Write(_T("MB44017D"), 0);
			::WritePrivateProfileString(_T("Last Job"), _T("One Metal On"), _T("0"), PATH_WORKING_INFO);// IDC_CHK_ONE_METAL - Recoiler\r정방향 CW : FALSE
		}

//#ifdef USE_ENGRAVE
//		if (pView && pView->m_pEngrave)
//			pView->m_pEngrave->SetRecoilerCcw();	//_stSigInx::_RecoilerCcw
//#endif
	}
}

void CGvisR2R_PunchView::AdjPinPos()
{
	if (m_pDlgMenu02)
	{
		if (m_pDlgMenu02->m_dMkFdOffsetY[0][0] > -2.0 && m_pDlgMenu02->m_dMkFdOffsetY[1][0] > -2.0 &&
			m_pDlgMenu02->m_dMkFdOffsetY[0][0] < 2.0 && m_pDlgMenu02->m_dMkFdOffsetY[1][0] < 2.0)
		{
			double dOffsetY0 = -1.0 * m_pDlgMenu02->m_dMkFdOffsetY[0][0];
			dOffsetY0 *= pDoc->m_dShiftAdjustRatio;
			double dOffsetY1 = -1.0 * m_pDlgMenu02->m_dMkFdOffsetY[1][0];
			dOffsetY1 *= pDoc->m_dShiftAdjustRatio;
			//double dOffsetY = -1.0*(m_pDlgMenu02->m_dMkFdOffsetY[0][0] + m_pDlgMenu02->m_dMkFdOffsetY[1][0]) / 2.0;
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

	// 마킹부 - TBL파기 OFF, TBL진공 ON, FD/TQ 진공 OFF, 
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

	// 마킹부 - FD/TQ 진공 ON, TBL진공 OFF, TBL파기 ON, 
	m_pDlgMenu03->SwMkTblBlw(FALSE);
	m_pDlgMenu03->SwMkTblVac(FALSE);
	m_pDlgMenu03->SwMkFdVac(FALSE);
	// 	m_pDlgMenu03->SwMkTqVac(FALSE);
}

void CGvisR2R_PunchView::SetEngraveFdSts()
{
	if (!m_pDlgMenu03)
		return;

	// 마킹부 - FD/TQ 진공 ON, TBL진공 OFF, TBL파기 ON, 
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
	// 마킹부 - FD/TQ 진공 ON, TBL진공 OFF, TBL파기 ON, 
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
		pView->m_pMpe->Write(_T("MB440151"), 1);	// 한판넬 이송상태 ON (PC가 ON, OFF)
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
	MoveMk(dOffset);
}

void CGvisR2R_PunchView::MoveEngrave(double dOffset)
{
#ifdef USE_MPE
	long lData = (long)(dOffset * 1000.0);
	pView->m_pMpe->Write(_T("MB440199"), 1);		// 각인부 피딩 CW ON (PLC가 피딩완료 후 OFF)
	pView->m_pMpe->Write(_T("ML45078"), lData);		// 각인부 Feeding 롤러 Offset(*1000, +:더 보냄, -덜 보냄, PC가 쓰고 PLC에서 지움)
#endif
}

BOOL CGvisR2R_PunchView::IsEngraveFd()
{
	if (pView->m_mgrProcedure->m_nShareDnCnt > 0)
	{
		if (!(pView->m_mgrProcedure->m_nShareDnCnt % 2))
		{
#ifdef USE_MPE
			if (pDoc->m_pMpeSignal[5] & (0x01 << 1))	// 마킹부 피딩 CW ON (PLC가 피딩완료 후 OFF)
				return TRUE;
#endif
			return FALSE;
		}
	}
	else
	{
		if (pView->m_mgrProcedure->m_nShareUpCnt > 0)
		{
			if (!(pView->m_mgrProcedure->m_nShareUpCnt % 2))
			{
#ifdef USE_MPE
				if (pDoc->m_pMpeSignal[5] & (0x01 << 1))	// 마킹부 피딩 CW ON (PLC가 피딩완료 후 OFF)
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
	double dCurPosEngraveFd = (double)pDoc->m_pMpeData[1][0];	// ML44052	,	각인부 Feeding 엔코더 값(단위 mm)
	double dRemain = _tstof(pDoc->WorkingInfo.Motion.sFdEngraveAoiInitDist) - dCurPosEngraveFd;
	return dRemain;
#else
	return 0.0;
#endif
}


LRESULT CGvisR2R_PunchView::wmClientReceived(WPARAM wParam, LPARAM lParam)
{
	if (!m_pEngrave)
		return (LRESULT)0;

	int nAcceptId = (int)wParam;
	SOCKET_DATA sSockData;
	SOCKET_DATA *pSocketData = (SOCKET_DATA*)lParam;
	SOCKET_DATA rSockData = *pSocketData;
	int nCmdCode = rSockData.nCmdCode;
	int nMsgId = rSockData.nMsgID;

	switch (nCmdCode)
	{
	case _GetSig:
		break;
	case _GetData:
		break;
	case _SetSig:
		if (m_pEngrave && m_pEngrave->IsConnected())
			m_pEngrave->GetSysSignal(rSockData);

		pView->m_bSetSig = TRUE;
		break;
	case _SetData:
		if (m_pEngrave && m_pEngrave->IsConnected())
			m_pEngrave->GetSysData(rSockData);

		pView->m_bSetData = TRUE;
		break;
	default:
		break;
	}


	return (LRESULT)1;
}

LRESULT CGvisR2R_PunchView::wmClientReceivedSr(WPARAM wParam, LPARAM lParam)
{
	int nCmd = (int)wParam;
	CString* sReceived = (CString*)lParam;

	switch (nCmd)
	{
	case SrTriggerInputOn:
		Get2dCode(m_sGet2dCodeLot, m_nGet2dCodeSerial);
		break;
	default:
		break;
	}

	return (LRESULT)1;
}
void CGvisR2R_PunchView::SetEngraveFdPitch(double dPitch)
{
	pDoc->SetEngraveFdPitch(dPitch);
}

BOOL CGvisR2R_PunchView::IsConnectedEng()
{
	if (m_bDestroyedView)
		return FALSE;

#ifdef USE_ENGRAVE
	if (m_pEngrave)
	{
		if(m_pEngrave->IsConnected())
		{
			if(!pView->m_mgrProcedure->m_bContEngraveF)
			{
				pView->m_mgrProcedure->m_bContEngraveF = TRUE;
				DWORD dwStartTick = GetTickCount();
			}
			return TRUE;
		}
		else
		{
			if (pView->m_mgrProcedure->m_bContEngraveF)
			{
				pView->m_mgrProcedure->m_bContEngraveF = FALSE;
			}
			return FALSE;
		}
	}
#endif
	return FALSE;
}


BOOL CGvisR2R_PunchView::IsPinPos0()
{
	if (!m_pMotion)
		return FALSE;

	double pPos[2];
	pPos[0] = m_pMotion->m_dPinPosX[0];
	pPos[1] = m_pMotion->m_dPinPosY[0];

	double dCurrX = pView->m_mgrProcedure->m_dEnc[AXIS_X0];
	double dCurrY = pView->m_mgrProcedure->m_dEnc[AXIS_Y0];

	if (dCurrX < pPos[0] - 4.0 || dCurrX > pPos[0] + 4.0)
		return FALSE;
	if (dCurrY < pPos[1] - 4.0 || dCurrY > pPos[1] + 4.0)
		return FALSE;

	return TRUE;
}

BOOL CGvisR2R_PunchView::IsPinPos1()
{
	if (!m_pMotion)
		return FALSE;

	double pPos[2];
	pPos[0] = m_pMotion->m_dPinPosX[1];
	pPos[1] = m_pMotion->m_dPinPosY[1];

	double dCurrX = pView->m_mgrProcedure->m_dEnc[AXIS_X1];
	double dCurrY = pView->m_mgrProcedure->m_dEnc[AXIS_Y1];

	if (dCurrX < pPos[0] - 4.0 || dCurrX > pPos[0] + 4.0)
		return FALSE;
	if (dCurrY < pPos[1] - 4.0 || dCurrY > pPos[1] + 4.0)
		return FALSE;

	return TRUE;
}

void CGvisR2R_PunchView::RunShift2Mk()
{
	Shift2Mk();			// PCR 이동(Buffer->Marked) // 기록(WorkingInfo.LastJob.sSerial)
}

BOOL CGvisR2R_PunchView::LoadMasterSpec()
{
	return TRUE;
}

// Call from Thread Function
void CGvisR2R_PunchView::UpdateYield(int nSerial)
{
	pView->m_mgrReelmap->UpdateYield(nSerial);
}

void CGvisR2R_PunchView::UpdateYield()
{
	int nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0];
	if (nSerial < 1)
	{
		Stop();
		MsgBox(_T("Error-UpdateYield() : m_nBufUpSerial[0] < 1"));
		return;
	}

	pView->m_mgrReelmap->UpdateYieldOnThread(nSerial);
}

void CGvisR2R_PunchView::DoShift2Mk()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nSerial;

	if (pView->m_mgrProcedure->m_bLastProc && pView->m_mgrProcedure->m_nBufUpSerial[0] == pView->m_mgrProcedure->m_nLotEndSerial)
	{
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0];
		//if (nSerial > 0 && (nSerial % 2)) // First Shot number must be odd.
		if (nSerial > 0) // 20130202
			pView->m_mgrProcedure->m_bTHREAD_SHIFT2MK = TRUE;
	}
	else
	{
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0];
		if (!pView->m_mgrProcedure->m_bCont)
		{
			//if (nSerial > 0 && (nSerial % 2)) // First Shot number must be odd.
			if (nSerial > 0)
				pView->m_mgrProcedure->m_bTHREAD_SHIFT2MK = TRUE;
			else
				Stop();
		}
		else
		{
			if (nSerial > 0)
				pView->m_mgrProcedure->m_bTHREAD_SHIFT2MK = TRUE;
			else
				Stop();
		}
	}
}

void CGvisR2R_PunchView::DoAutoEng()
{
	//if (!IsAuto() || (MODE_INNER != pDoc->WorkingInfo.LastJob.nTestMode))
	if (!IsAuto() || (MODE_INNER != pDoc->GetTestMode()))
		return;

	// 각인부 마킹시작 신호를 확인
	DoAtuoGetEngStSignal();

	// 각인부 2D 코드 Reading신호를 확인
	DoAtuoGet2dReadStSignal();
}

void CGvisR2R_PunchView::DoAtuoGetEngStSignal()
{
#ifdef USE_MPE
	if ((pDoc->m_pMpeSignal[0] & (0x01 << 3) || pView->m_mgrProcedure->m_bEngStSw) && !pDoc->BtnStatus.EngAuto.MkStF)// 2D(GUI) 각인 동작 Start신호(PLC On->PC Off)
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
		if (m_pMpe)
		{
			m_pMpe->Write(_T("MB440103"), 0);			// 2D(GUI) 각인 동작 Start신호(PLC On->PC Off)

			//if (pDoc->m_pMpeSignal[0] & (0x01 << 2))	// 각인부 Feeding완료(PLC가 On시키고 PC가 확인하고 Reset시킴.)
			//	m_pMpe->Write(_T("MB440102"), 0);		// 각인부 Feeding완료
		}
	}

	if (pDoc->m_pMpeSignal[0] & (0x01 << 2) && !pDoc->BtnStatus.EngAuto.FdDoneF)	// 각인부 Feeding완료(PLC가 On시키고 PC가 확인하고 Reset시킴.)
	{
		pDoc->BtnStatus.EngAuto.FdDoneF = TRUE;

		pDoc->BtnStatus.EngAuto.IsFdDone = FALSE;
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqFdDone, TRUE);
	}
	else if (pDoc->BtnStatus.EngAuto.IsFdDone && pDoc->BtnStatus.EngAuto.FdDoneF)
	{
		pDoc->BtnStatus.EngAuto.FdDoneF = FALSE;
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqFdDone, FALSE);
		if (m_pMpe)
		{
			m_pMpe->Write(_T("MB440102"), 0);		// 각인부 Feeding완료(PLC가 On시키고 PC가 확인하고 Reset시킴.)
		}
	}

#endif
}

void CGvisR2R_PunchView::DoAtuoGet2dReadStSignal()
{
	if ((pDoc->m_pMpeSignal[0] & (0x01 << 5) || pView->m_mgrProcedure->m_bEng2dStSw) && !pDoc->BtnStatus.EngAuto.Read2dStF)// 각인부 2D 리더 시작신호(PLC On->PC Off)
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
		if (m_pMpe)
		{
			m_pMpe->Write(_T("MB440105"), 0);			// 각인부 2D 리더 시작신호(PLC On->PC Off)
			//if (pDoc->m_pMpeSignal[0] & (0x01 << 2))	// 각인부 Feeding완료(PLC가 On시키고 PC가 확인하고 Reset시킴.)
			//	m_pMpe->Write(_T("MB440102"), 0);		// 각인부 Feeding완료
		}
#endif
	}
}


void CGvisR2R_PunchView::DoAutoSetFdOffsetEngrave()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	double dAveX, dAveY;
	CfPoint OfSt;

	if ((pDoc->BtnStatus.EngAuto.IsOnMking && !pView->m_mgrProcedure->m_bEngTestF) 
		|| (pDoc->BtnStatus.EngAuto.IsOnRead2d && !pView->m_mgrProcedure->m_bEngTestF)) // 각인부 검사중
	{
		pView->m_mgrProcedure->m_bEngTestF = TRUE;
		pView->m_mgrProcedure->m_bEngTest = TRUE;
	}
	else if ((!pDoc->BtnStatus.EngAuto.IsOnMking && pView->m_mgrProcedure->m_bEngTestF) 
		|| (!pDoc->BtnStatus.EngAuto.IsOnRead2d && pView->m_mgrProcedure->m_bEngTestF))
	{
		pView->m_mgrProcedure->m_bEngTestF = FALSE;
		pView->m_mgrProcedure->m_bEngTest = FALSE;
		pView->m_mgrProcedure->m_bEngFdWriteF = FALSE;
#ifdef USE_MPE
		if(m_pMpe)
			m_pMpe->Write(_T("MB44011A"), 0);					// 각인부 Feeding Offset Write 완료(PC가 확인하고 Reset시킴.)
#endif
	}

	if (pDoc->m_pMpeSignal[1] & (0x01 << 10) && !pView->m_mgrProcedure->m_bEngFdWrite)		// 각인부 Feeding Offset Write 완료(PC가 확인하고 Reset시킴.)
	{
		pView->m_mgrProcedure->m_bEngFdWrite = TRUE;
	}
	else if (!(pDoc->m_pMpeSignal[1] & (0x01 << 10)) && pView->m_mgrProcedure->m_bEngFdWrite)
	{
		pView->m_mgrProcedure->m_bEngFdWrite = FALSE;
	}


	if (pView->m_mgrProcedure->m_bEngFdWrite && !pView->m_mgrProcedure->m_bEngFdWriteF)
	{
		pView->m_mgrProcedure->m_bEngFdWriteF = TRUE;

		//if (MODE_INNER == pDoc->GetTestMode())
		//	GetCurrentInfoEng();

		GetEngOffset(OfSt);

		dAveX = OfSt.x;
		dAveY = OfSt.y;

		if (m_pDlgMenu02)
		{
			m_pDlgMenu02->m_dEngFdOffsetX = OfSt.x;
			m_pDlgMenu02->m_dEngFdOffsetY = OfSt.y;
		}

#ifdef USE_MPE
		if (m_pMpe)
		{
			m_pMpe->Write(_T("ML45078"), (long)(dAveX*1000.0));	// 각인부 Feeding 롤러 Offset(*1000, +:더 보냄, -덜 보냄, PC가 쓰고 PLC에서 지움)
			m_pMpe->Write(_T("MB44011A"), 0);					// 각인부 Feeding Offset Write 완료(PC가 확인하고 Reset시킴.)
		}
		Sleep(10);
#endif
	}
	else if (!pView->m_mgrProcedure->m_bEngFdWrite && pView->m_mgrProcedure->m_bEngFdWriteF)
	{
		pView->m_mgrProcedure->m_bEngFdWriteF = FALSE;
		pView->m_mgrProcedure->m_bEngTest = FALSE;
	}

}

void CGvisR2R_PunchView::DoAutoMarkingEngrave()
{	
	// 각인부 마킹중 ON (PC가 ON, OFF)
	if ( pDoc->BtnStatus.EngAuto.IsOnMking && !(pDoc->m_pMpeSignal[6] & (0x01 << 3)) ) // 각인부 마킹중 ON (PC가 ON, OFF)
	{
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqOnMkIng, TRUE);
#ifdef USE_MPE
		if (m_pMpe)
			m_pMpe->Write(_T("MB440173"), 1); // 2D(GUI) 각인 동작Running신호(PC On->PC Off)
#endif
	}
	else if ( !pDoc->BtnStatus.EngAuto.IsOnMking && (pDoc->m_pMpeSignal[6] & (0x01 << 3)) )
	{
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqOnMkIng, FALSE);
#ifdef USE_MPE
		if (m_pMpe)
			m_pMpe->Write(_T("MB440173"), 0); // 2D(GUI) 각인 동작Running신호(PC On->PC Off)
#endif
	}

	// 각인부 마킹완료 ON (PC가 ON, OFF)
	if (pDoc->BtnStatus.EngAuto.IsMkDone && !(pDoc->m_pMpeSignal[6] & (0x01 << 4))) // 각인부 작업완료.(PC가 On, PLC가 확인 후 Off)
	{
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqMkDone, TRUE);
#ifdef USE_MPE
		if (m_pMpe)
			m_pMpe->Write(_T("MB440174"), 1); // 각인부 작업완료.(PC가 On, PLC가 확인 후 Off)
#endif
	}
	else if (!pDoc->BtnStatus.EngAuto.IsMkDone && (pDoc->m_pMpeSignal[6] & (0x01 << 4))) // 각인부 작업완료.(PC가 On, PLC가 확인 후 Off)
	{
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqMkDone, FALSE);
//#ifdef USE_MPE
//		if (m_pMpe)
//			m_pMpe->Write(_T("MB440174"), 0); // 각인부 작업완료.(PC가 On, PLC가 확인 후 Off)
//#endif
	}

	// 각인부 2D 리더 작업중 신호
	if (pDoc->BtnStatus.EngAuto.IsOnRead2d && !(pDoc->m_pMpeSignal[6] & (0x01 << 8))) // 각인부 2D 리더 작업중 신호(PC On->PC Off)
	{
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqOnReading2d, TRUE);
#ifdef USE_MPE
		if (m_pMpe)
			m_pMpe->Write(_T("MB440178"), 1); // 각인부 2D 리더 작업중 신호(PC On->PC Off)
#endif
	}
	else if (!pDoc->BtnStatus.EngAuto.IsOnRead2d && (pDoc->m_pMpeSignal[6] & (0x01 << 8)))
	{
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqOnReading2d, FALSE);
#ifdef USE_MPE
		if (m_pMpe)
			m_pMpe->Write(_T("MB440178"), 0); // 각인부 2D 리더 작업중 신호(PC On->PC Off)
#endif
	}

	// 각인부 2D 리더 작업완료 신호
	if (pDoc->BtnStatus.EngAuto.IsRead2dDone && !(pDoc->m_pMpeSignal[6] & (0x01 << 9))) // 각인부 2D 리더 작업완료 신호.(PC가 On, PLC가 확인 후 Off)
	{
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeq2dReadDone, TRUE);
#ifdef USE_MPE
		if (m_pMpe)
			m_pMpe->Write(_T("MB440179"), 1); // 각인부 2D 리더 작업완료 신호.(PC가 On, PLC가 확인 후 Off)
#endif
	}
	else if (!pDoc->BtnStatus.EngAuto.IsRead2dDone && (pDoc->m_pMpeSignal[6] & (0x01 << 9)))
	{
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeq2dReadDone, FALSE);
//#ifdef USE_MPE
//		if (m_pMpe)
//			m_pMpe->Write(_T("MB440179"), 0); // 각인부 2D 리더 작업완료 신호.(PC가 On, PLC가 확인 후 Off)
//#endif
	}

}

void CGvisR2R_PunchView::SetEngFd()
{
	CfPoint OfSt;
	if (GetEngOffset(OfSt))
	{
		if (m_pDlgMenu02)
		{
			m_pDlgMenu02->m_dEngFdOffsetX = OfSt.x;
			m_pDlgMenu02->m_dEngFdOffsetY = OfSt.y;
		}
	}

	MoveEng(-1.0*OfSt.x);
	if (m_pDlgMenu03)
		m_pDlgMenu03->ChkDoneEngrave();

	if (!pDoc->WorkingInfo.LastJob.bAoiOnePnl)
	{
#ifdef USE_MPE
		pView->m_pMpe->Write(_T("MB440151"), 1);	// 한판넬 이송상태 ON (PC가 ON, OFF)
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
	pView->m_pMpe->Write(_T("MB440160"), 1);	// 검사부 피딩 ON (PLC가 피딩완료 후 OFF)
	pView->m_pMpe->Write(_T("ML45064"), lData);	// 검사부 Feeding 롤러 Offset(*1000, +:더 보냄, -덜 보냄)
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

	pView->m_mgrProcedure->m_bEngFdWrite = FALSE;
	pView->m_mgrProcedure->m_bEngFdWriteF = FALSE;
	pView->m_mgrProcedure->m_bEngTest = FALSE;
	pView->m_mgrProcedure->m_bEngTestF = FALSE;

	m_pMpe->Write(_T("MB440103"), 0); // 2D(GUI) 각인 동작 Start신호(PLC On->PC Off)
	m_pMpe->Write(_T("MB440173"), 0); // 2D(GUI) 각인 동작Running신호(PC On->PC Off)
	m_pMpe->Write(_T("MB440174"), 0); // 각인부 작업완료.(PC가 On, PLC가 확인 후 Off)

	m_pMpe->Write(_T("MB440105"), 0); // 각인부 2D 리더 시작신호(PLC On->PC Off)
	m_pMpe->Write(_T("MB440178"), 0); // 각인부 2D 리더 작업중 신호(PC On->PC Off)
	m_pMpe->Write(_T("MB440179"), 0); // 각인부 2D 리더 작업완료 신호.(PC가 On, PLC가 확인 후 Off)

	m_pMpe->Write(_T("MB440102"), 0); // 각인부 Feeding완료(PLC가 On시키고 PC가 확인하고 Reset시킴.)

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

	if (m_pSr1000w)
	{
		return m_pSr1000w->IsConnected();
	}

	return FALSE;
}

BOOL CGvisR2R_PunchView::Set2dRead(BOOL bRun)	// Marking Start
{
	if (!pView || !pView->m_pSr1000w)
		return FALSE;

	return (pView->m_pSr1000w->DoRead2DCode());
}

BOOL  CGvisR2R_PunchView::Is2dReadDone()
{
	if (!pView || !pView->m_pSr1000w)
		return FALSE;

	return (!pView->m_pSr1000w->IsRunning());
}

BOOL CGvisR2R_PunchView::Get2dCode(CString &sLot, int &nSerial)
{
	if (!m_pSr1000w)
		return FALSE;

	CString sData;
	if (m_pSr1000w->Get2DCode(sData))
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

	if (m_pDts)
	{
		if (m_pDts->IsUseDts())
		{
			bRtn = m_pDts->GetCurrentDBName(sName);
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
	int nIdx = pView->m_mgrReelmap->GetPcrIdx(nSerial);					// 릴맵화면버퍼 인덱스
	CString sLot = pView->m_mgrReelmap->m_pPcr[0][nIdx]->m_sLot;

	if (m_pDts)
	{
		if (m_pDts->IsUseDts())
		{
			bRtn = m_pDts->LoadPieceOut(sLot, nSerial, pPcsOutIdx, nTotPcsOut);
			if (!bRtn)
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Error - GetDtsPieceOut()."), MB_ICONSTOP | MB_OK);
			}
		}
	}

	return bRtn;
}

void CGvisR2R_PunchView::DoMark0Its()
{
#ifdef TEST_MODE
	return;
#endif

	if (!pView->m_mgrProcedure->m_bAuto)
		return;

	int nSerial, nIdx, nErrCode, nRtn;
	CfPoint ptPnt;
	CString sMsg;
	double dStripOut = (pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs / MAX_STRIP_NUM) * _tstof(pDoc->WorkingInfo.LastJob.sStripOutRatio) / 100.0;
	int nStripOut = int(dStripOut);
	if (dStripOut > nStripOut)
		nStripOut++;			// 스트립 양폐 비율

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (!pView->m_mgrProcedure->IsRun())																		// 정지상태에서
	{
		if (IsOnMarking0())																// 마킹중에
		{
			if (m_pMotion->IsEnable(MS_X0) && m_pMotion->IsEnable(MS_Y0))				// 모션이 Enable상태이고
			{
				if (m_pMotion->IsMotionDone(MS_X0) && m_pMotion->IsMotionDone(MS_Y0))	// 모션 Done상태이면,
				{
					if (!IsInitPos0() && !IsPinPos0())									// 초기위치가 아니거나, 핀위치가 아닐때
						MoveInitPos0();													// 초기위치로 이동
				}
			}

			if (pView->m_mgrProcedure->m_nStepMk[0] < 13 && pView->m_mgrProcedure->m_nStepMk[0] > 8) // Mk0();
			{
				pView->m_mgrProcedure->m_nStepMk[0] = 8;	// 마킹완료Check
			}
		}
		return;
	}

	pView->m_mgrProcedure->m_sDispSts[0].Format(_T("%d"), pView->m_mgrProcedure->m_nStepMk[0]);

	switch (pView->m_mgrProcedure->m_nStepMk[0])
	{
	case 0:
		if (IsNoMk())
			ShowLive();
		pView->m_mgrProcedure->m_nStepMk[0]++;
		break;
	case 1:
		if (!IsInitPos0())
			MoveInitPos0();
		pView->m_mgrProcedure->m_nStepMk[0]++;
		break;
	case 2:

		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Cam0

		if (nSerial > 0)
		{
			if ((nErrCode = pView->m_mgrReelmap->GetErrCode0Its(nSerial)) != 1)
			{
				pView->m_mgrProcedure->m_nMkPcs[0] = pView->m_mgrReelmap->GetTotDefPcsIts(nSerial);
				pView->m_mgrProcedure->m_nStepMk[0] = MK_END;
			}
			else
			{
				pView->m_mgrProcedure->m_nStepMk[0]++;
			}
		}
		else
		{
			StopFromThread();
			AsyncMsgBox(_T("버퍼의 시리얼이 맞지않습니다."), 1);
			BuzzerFromThread(TRUE, 0);
			DispMain(_T("정 지"), RGB_RED);
		}
		break;
	case 3:
		pView->m_mgrProcedure->m_nStepMk[0]++;
		break;
	case 4:
		pView->m_mgrProcedure->m_nStepMk[0]++;
		break;
	case 5:
		pView->m_mgrProcedure->m_nStepMk[0]++;
		break;
	case 6:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Cam0

		if (pView->m_mgrProcedure->m_nMkPcs[0] < pView->m_mgrReelmap->GetTotDefPcsIts(nSerial))
		{
			if (!IsNoMk0())
			{
				;
			}
			else
			{
				if (!IsReview0())
				{
					if (pView->m_mgrProcedure->m_bReview)
					{
						pView->m_mgrProcedure->m_nMkPcs[0] = pView->m_mgrReelmap->GetTotDefPcsIts(nSerial);
						pView->m_mgrProcedure->m_nStepMk[0] = MK_END;
						break;
					}
				}
			}
			// Punching On이거나 Review이면 다음으로 진행
			SetDelay0(100, 1);		// [mSec]
			pView->m_mgrProcedure->m_nStepMk[0]++;
		}
		else
		{
			pView->m_mgrProcedure->m_nStepMk[0] = MK_END;
		}
		break;
	case 7:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Cam0

		if (!WaitDelay0(1))		// F:Done, T:On Waiting....		// Delay후에
		{
			pView->m_mgrProcedure->m_nMkPcs[0] = 0;

			if (!IsNoMk0())										// Punching On이면
			{
				pView->m_mgrProcedure->m_nStepMk[0]++;
			}
			else												// Punching이 Off이고
			{
				if (IsReview0())								// Review이면 다음으로
				{
					pView->m_mgrProcedure->m_nStepMk[0]++;
				}
				else											// Review가 아니면
				{
					if (pView->m_mgrProcedure->m_bReview)
					{
						pView->m_mgrProcedure->m_nMkPcs[0] = pView->m_mgrReelmap->GetTotDefPcsIts(nSerial);
						pView->m_mgrProcedure->m_nStepMk[0] = MK_END;
					}
					else
						pView->m_mgrProcedure->m_nStepMk[0]++;
				}
			}
		}
		break;
	case 8:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Cam0

		if (pView->m_mgrProcedure->m_nMkPcs[0] < pView->m_mgrReelmap->GetTotDefPcsIts(nSerial))	// 마킹완료Check
		{
			if (pView->m_mgrProcedure->m_nMkPcs[0] + 1 < pView->m_mgrReelmap->GetTotDefPcsIts(nSerial))		// 다음 마킹위치가 있으면
			{
				ptPnt = pView->m_mgrReelmap->GetMkPntIts(nSerial, pView->m_mgrProcedure->m_nMkPcs[0] + 1);	// 다음 마킹위치
				pView->m_mgrProcedure->m_dNextTarget[AXIS_X0] = ptPnt.x;
				pView->m_mgrProcedure->m_dNextTarget[AXIS_Y0] = ptPnt.y;
			}
			else												// 다음 마킹위치가 없으면
			{
				pView->m_mgrProcedure->m_dNextTarget[AXIS_X0] = -1.0;
				pView->m_mgrProcedure->m_dNextTarget[AXIS_Y0] = -1.0;
			}

			ptPnt = pView->m_mgrReelmap->GetMkPntIts(nSerial, pView->m_mgrProcedure->m_nMkPcs[0]);			// 이번 마킹위치
			if (ptPnt.x < 0.0 && ptPnt.y < 0.0) // 양품화. (마킹하지 않음)
			{
				pView->m_mgrProcedure->m_nMkPcs[0]++;
				pView->m_mgrProcedure->m_nStepMk[0] = MK_DONE_CHECK;
				break;
			}

			nIdx = pView->m_mgrReelmap->GetMkStripIdxIts(nSerial, pView->m_mgrProcedure->m_nMkPcs[0]);		// 1 ~ 4 : strip index
			if (nIdx > 0)										// Strip index가 정상이면,
			{
				if (!IsMkStrip(nIdx)) // Strip[] Mk Off
				{
					pView->m_mgrProcedure->m_nMkPcs[0]++;
					pView->m_mgrProcedure->m_nStepMk[0] = MK_DONE_CHECK;
					break;
				}
				else
				{
					if (pView->m_mgrProcedure->m_nMkStrip[0][nIdx - 1] >= nStripOut)
					{
						pView->m_mgrProcedure->m_nMkPcs[0]++;
						pView->m_mgrProcedure->m_nStepMk[0] = MK_DONE_CHECK;
						break;
					}
					else
						pView->m_mgrProcedure->m_nMkStrip[0][nIdx - 1]++;
				}
			}
			else
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Strip Index Failed."));
				break;
			}

			pView->m_mgrProcedure->m_dTarget[AXIS_X0] = ptPnt.x;
			pView->m_mgrProcedure->m_dTarget[AXIS_Y0] = ptPnt.y;

			pView->m_mgrProcedure->m_nStepMk[0]++;
		}
		else
		{
			pView->m_mgrProcedure->m_nStepMk[0] = MK_END;
		}
		break;
	case 9:
		// Cam0 : m_bPriority[0] <-, m_bPriority[3] ->
		// Cam1 : m_bPriority[1] ->, m_bPriority[2] <-
		ptPnt.x = pView->m_mgrProcedure->m_dTarget[AXIS_X0];
		ptPnt.y = pView->m_mgrProcedure->m_dTarget[AXIS_Y0];

		if (pView->m_mgrProcedure->m_dNextTarget[AXIS_X0] < 0)
			pView->m_mgrProcedure->m_bCollision[0] = m_mgrProcedure->ChkCollision(AXIS_X0, pView->m_mgrProcedure->m_dTarget[AXIS_X0]);
		else
			pView->m_mgrProcedure->m_bCollision[0] = m_mgrProcedure->ChkCollision(AXIS_X0, pView->m_mgrProcedure->m_dTarget[AXIS_X0], pView->m_mgrProcedure->m_dNextTarget[AXIS_X0]);

		if (!pView->m_mgrProcedure->m_bCollision[0])
		{
			if (IsMoveDone0())
			{
				Move0(ptPnt, pView->m_mgrProcedure->m_bCam);
				pView->m_mgrProcedure->m_nStepMk[0]++;
			}
		}
		else if (pView->m_mgrProcedure->m_bPriority[0])
		{
			if (IsMoveDone0())
			{
				pView->m_mgrProcedure->m_bCollision[0] = FALSE;
				pView->m_mgrProcedure->m_bPriority[0] = FALSE;
				Move0(ptPnt, pView->m_mgrProcedure->m_bCam);
				pView->m_mgrProcedure->m_nStepMk[0]++;
			}
		}
		else if (pView->m_mgrProcedure->m_bPriority[2])
		{
			if (IsMoveDone0())
			{
				pView->m_mgrProcedure->m_bCollision[0] = FALSE;
				pView->m_mgrProcedure->m_bPriority[2] = FALSE;
				ptPnt.x = 0.0;
				ptPnt.y = 0.0;//m_dEnc[AXIS_Y0];
				Move0(ptPnt, FALSE);
			}
		}
		break;
	case 10:
		pView->m_mgrProcedure->m_nStepMk[0]++;
		break;
	case 11:
		pView->m_mgrProcedure->m_nStepMk[0]++;
		break;
	case 12:
		if (IsMoveDone0())
			pView->m_mgrProcedure->m_nStepMk[0]++;
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
			if (!SaveMk0Img(pView->m_mgrProcedure->m_nMkPcs[0]))
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Error-SaveMk0Img()"));
			}
		}
		pView->m_mgrProcedure->m_nStepMk[0]++;
		break;
	case 14:
		pView->m_mgrProcedure->m_nStepMk[0]++;
		break;
	case 15:
		pView->m_mgrProcedure->m_nStepMk[0]++;
		break;
	case 16:
		if (IsNoMk0())
		{
			if (!WaitDelay0(1))		// F:Done, T:On Waiting....
				pView->m_mgrProcedure->m_nStepMk[0]++;
		}
		else
			pView->m_mgrProcedure->m_nStepMk[0]++;
		break;
	case 17:
		if (!IsNoMk0())
		{
			if (IsMk0Done())
			{
				// One more MK On Start....
				if (!pView->m_mgrProcedure->m_nMkPcs[0] && !pView->m_mgrProcedure->m_bAnswer[2])
				{
					pView->m_mgrProcedure->m_bAnswer[2] = TRUE;
					Mk0();
				}
				else
				{
					pView->m_mgrProcedure->m_bAnswer[2] = FALSE;
					pView->m_mgrProcedure->m_nMkPcs[0]++;
					pView->m_mgrProcedure->m_nStepMk[0]++;
					pView->m_mgrProcedure->m_nStepMk[0]++;
				}
			}
			else
			{
				if (pView->m_mgrProcedure->m_dwStMkDn[0] + 5000 < GetTickCount())
				{
					BuzzerFromThread(TRUE, 0);
					DispMain(_T("정 지"), RGB_RED);
					m_pVoiceCoil[0]->SearchHomeSmac0();

					nRtn = AsyncMsgBox(_T("보이스코일(좌) 통신완료가 않됩니다.\r\n마킹을 다시 시도하시겠습니까?"), 1, MB_YESNO);
					if (IDYES == nRtn)
					{
						DispMain(_T("운전중"), RGB_RED);
						pView->m_mgrProcedure->m_nStepMk[0] = 13;
					}
					else if (nRtn < 0)
						pView->m_mgrProcedure->m_nStepMk[0]++; // Wait...
					else
					{
						pView->m_mgrProcedure->m_bAnswer[2] = FALSE;
						pView->m_mgrProcedure->m_nMkPcs[0]++;
						pView->m_mgrProcedure->m_nStepMk[0]++;
						pView->m_mgrProcedure->m_nStepMk[0]++;
						Stop();
					}
				}
			}
		}
		else
		{
			pView->m_mgrProcedure->m_nMkPcs[0]++;
			pView->m_mgrProcedure->m_nStepMk[0]++;
			pView->m_mgrProcedure->m_nStepMk[0]++;
		}
		break;
	case 18:
		if ((nRtn = WaitRtnVal(1)) > -1)
		{
			if (IDYES == nRtn)
			{
				DispMain(_T("운전중"), RGB_RED);
				pView->m_mgrProcedure->m_nStepMk[0] = 13;
			}
			else
			{
				pView->m_mgrProcedure->m_bAnswer[2] = FALSE;
				pView->m_mgrProcedure->m_nMkPcs[0]++;
				pView->m_mgrProcedure->m_nStepMk[0]++;
				StopFromThread();
			}
		}
		break;
	case 19:
		pView->m_mgrProcedure->m_nStepMk[0] = MK_DONE_CHECK;
		break;
	case MK_DONE_CHECK:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Cam0

		if (pView->m_mgrProcedure->m_nMkPcs[0] < pView->m_mgrReelmap->GetTotDefPcsIts(nSerial))
		{
			if (IsNoMk0())
			{
				if (!pDoc->WorkingInfo.System.bNoMk)
				{
					if (IsReview0())
					{
						if (IsJogRtDn0())
							pView->m_mgrProcedure->m_nStepMk[0]++;
					}
					else if (!pDoc->WorkingInfo.LastJob.bVerify)
					{
						pView->m_mgrProcedure->m_nMkPcs[0] = pView->m_mgrReelmap->GetTotDefPcsIts(nSerial);
						pView->m_mgrProcedure->m_nStepMk[0] = MK_END;
					}
					else
						pView->m_mgrProcedure->m_nStepMk[0] = 8;	// 마킹완료Check
				}
				else
					pView->m_mgrProcedure->m_nStepMk[0] = 8;	// 마킹완료Check
			}
			else
				pView->m_mgrProcedure->m_nStepMk[0] = 8;	// 마킹완료Check
		}
		else
		{
			if (IsNoMk0())
			{
				if (IsReview0())
				{
					if (IsJogRtDn0())
						pView->m_mgrProcedure->m_nStepMk[0] = MK_END;
				}
				else
				{
					pView->m_mgrProcedure->m_nStepMk[0] = MK_END;
				}
			}
			else
				pView->m_mgrProcedure->m_nStepMk[0] = MK_END;
		}
		break;
	case MK_DONE_CHECK + 1:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Cam0

		if (pView->m_mgrProcedure->m_nMkPcs[0] < pView->m_mgrReelmap->GetTotDefPcsIts(nSerial))
		{
			if (IsNoMk0())
			{
				if (IsReview0())
				{
					if (IsJogRtUp0())
						pView->m_mgrProcedure->m_nStepMk[0] = 8;	// 마킹완료Check
				}
				else
				{
					pView->m_mgrProcedure->m_nMkPcs[0] = pView->m_mgrReelmap->GetTotDefPcsIts(nSerial);
					pView->m_mgrProcedure->m_nStepMk[0] = MK_END;
				}
			}
			else
				pView->m_mgrProcedure->m_nStepMk[0] = 8;	// 마킹완료Check
		}
		else
		{
			pView->m_mgrProcedure->m_nStepMk[0] = MK_END;
		}
		break;

	case MK_END:
		if (IsMoveDone0())
			pView->m_mgrProcedure->m_nStepMk[0]++;
		break;
	case 101:
		SetDelay0(100, 1);		// [mSec]
		pView->m_mgrProcedure->m_nStepMk[0]++;
		break;
	case 102:
		if (!WaitDelay0(1))		// F:Done, T:On Waiting....
		{
			pView->m_mgrProcedure->m_nStepMk[0]++;
		}
		break;
	case 103:
		if (!IsInitPos0())
		{
			pView->m_mgrProcedure->m_dTarget[AXIS_X0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[0]);
			pView->m_mgrProcedure->m_dTarget[AXIS_Y0] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[0]);
			pView->m_mgrProcedure->m_dNextTarget[AXIS_X0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[0]);
			pView->m_mgrProcedure->m_dNextTarget[AXIS_Y0] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[0]);

			MoveInitPos0();
		}

		pDoc->SaveMkCntL();
		pView->m_mgrProcedure->m_nStepMk[0]++;
		break;
	case 104:
		if (IsMoveDone0())
		{
			pView->m_mgrProcedure->m_nStepMk[0]++;
			SetDelay0(10000, 1);		// [mSec]
		}
		break;
	case 105:
		if (IsInitPos0())
		{
			pView->m_mgrProcedure->m_nStepMk[0]++;
		}
		else
		{
			if (!WaitDelay0(1))		// F:Done, T:On Waiting....
			{
				pView->m_mgrProcedure->m_nStepMk[0] = ERR_PROC;
			}
		}
		break;
	case 106: // MK Done....
		pView->m_mgrProcedure->m_bDoneMk[0] = TRUE;
		pView->m_mgrProcedure->m_bTHREAD_MK[0] = FALSE;
		break;

	case ERR_PROC:
		DispMain(_T("정 지"), RGB_RED);
		m_pVoiceCoil[0]->SearchHomeSmac0();
		AsyncMsgBox(_T("보이스코일(좌) 초기위치 이동이 되지 않습니다.\r\n마킹상태를 확인하세요."), 1);
		pView->m_mgrProcedure->m_nStepMk[0]++;
		break;
	case ERR_PROC + 1:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0];

		if (pView->m_mgrProcedure->m_bCam)
			sMsg.Format(_T("%d번 Shot을 다시 불량확인을 하시겠습니까?"), nSerial);
		else
			sMsg.Format(_T("%d번 Shot을 다시 마킹하시겠습니까?"), nSerial);

		pView->m_mgrProcedure->m_nRtnMyMsgBoxIdx = 0;
		pView->m_mgrProcedure->m_bRtnMyMsgBox[0] = FALSE;
		pView->m_mgrProcedure->m_nRtnMyMsgBox[0] = -1;
		pView->AsyncMsgBox(sMsg, 1, MB_YESNO);
		sMsg.Empty();
		pView->m_mgrProcedure->m_nStepMk[0]++;
		break;
	case ERR_PROC + 2:
		if ((nRtn = WaitRtnVal()) > -1)
		{
			if (IDYES == nRtn)
			{
				pView->m_mgrProcedure->m_nStepMk[0] = ERR_PROC + 10;
			}
			else
			{
				pView->m_mgrProcedure->m_nRtnMyMsgBoxIdx = 0;
				pView->m_mgrProcedure->m_bRtnMyMsgBox[0] = FALSE;
				pView->m_mgrProcedure->m_nRtnMyMsgBox[0] = -1;
				sMsg.Format(_T("계속 다음 작업을 진행하시겠습니까?"), nSerial);
				pView->AsyncMsgBox(sMsg, 1, MB_YESNO);
				sMsg.Empty();

				pView->m_mgrProcedure->m_nStepMk[0]++;
			}
		}
		break;
	case ERR_PROC + 3:
		if ((nRtn = WaitRtnVal()) > -1)
		{
			if (IDYES == nRtn)
			{
				pView->m_mgrProcedure->m_nStepMk[0] = ERR_PROC + 20;
			}
			else
			{
				pView->m_mgrProcedure->m_bDispMsgDoAuto[8] = TRUE;
				pView->m_mgrProcedure->m_nStepDispMsg[8] = FROM_DOMARK0;
			}
		}
		break;
	case ERR_PROC + 10:
		pView->m_mgrProcedure->m_bReMark[0] = TRUE;
		pView->m_mgrProcedure->m_bTHREAD_MK[0] = FALSE;
		pView->m_mgrProcedure->m_nStepMk[0] = 0;
		break;
	case ERR_PROC + 20: // MK Done....
		pView->m_mgrProcedure->m_bDoneMk[0] = TRUE;
		pView->m_mgrProcedure->m_bTHREAD_MK[0] = FALSE;
		break;
	}
}


void CGvisR2R_PunchView::DoMark1Its()
{
#ifdef TEST_MODE
	return;
#endif

	if (!pView->m_mgrProcedure->m_bAuto)
		return;

	//BOOL bOn;
	int nSerial, nIdx, nErrCode, nRtn;
	CfPoint ptPnt;
	CString sMsg;
	double dStripOut = (pView->m_mgrReelmap->m_Master[0].m_pPcsRgn->nTotPcs / MAX_STRIP_NUM) * _tstof(pDoc->WorkingInfo.LastJob.sStripOutRatio) / 100.0;
	int nStripOut = int(dStripOut);
	if (dStripOut > nStripOut)
		nStripOut++;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;


	if (!pView->m_mgrProcedure->IsRun())
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

			if (pView->m_mgrProcedure->m_nStepMk[1] < 13 && pView->m_mgrProcedure->m_nStepMk[1] > 8) // Mk1();
			{
				pView->m_mgrProcedure->m_nStepMk[1] = 8;	// 마킹완료Check
			}
		}
		return;
	}


	pView->m_mgrProcedure->m_sDispSts[1].Format(_T("%d"), pView->m_mgrProcedure->m_nStepMk[1]);

	switch (pView->m_mgrProcedure->m_nStepMk[1])
	{
	case 0:
		if (IsNoMk())
			ShowLive();
		pView->m_mgrProcedure->m_nStepMk[1]++;
		break;
	case 1:
		if (!IsInitPos1())
			MoveInitPos1();
		pView->m_mgrProcedure->m_nStepMk[1]++;
		break;
	case 2:

		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

		if (nSerial > 0)
		{
			if ((nErrCode = pView->m_mgrReelmap->GetErrCode1Its(nSerial)) != 1)
			{
				pView->m_mgrProcedure->m_nMkPcs[1] = pView->m_mgrReelmap->GetTotDefPcsIts(nSerial);
				pView->m_mgrProcedure->m_nStepMk[1] = MK_END;
			}
			else
			{
				pView->m_mgrProcedure->m_nStepMk[1]++;
			}
		}
		else
		{
			StopFromThread();
			AsyncMsgBox(_T("버퍼의 시리얼이 맞지않습니다."), 2);
			BuzzerFromThread(TRUE, 0);
			//pView->DispStsBar(_T("정지-34"), 0);
			DispMain(_T("정 지"), RGB_RED);
		}
		break;
	case 3:
		pView->m_mgrProcedure->m_nStepMk[1]++;
		break;
	case 4:
		pView->m_mgrProcedure->m_nStepMk[1]++;
		break;
	case 5:
		pView->m_mgrProcedure->m_nStepMk[1]++;
		break;
	case 6:

		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

		if (pView->m_mgrProcedure->m_nMkPcs[1] < pView->m_mgrReelmap->GetTotDefPcsIts(nSerial))
		{
			if (!IsNoMk1())
			{
				;
			}
			else
			{
				if (!IsReview1())
				{
					if (pView->m_mgrProcedure->m_bReview)
					{
						pView->m_mgrProcedure->m_nMkPcs[1] = pView->m_mgrReelmap->GetTotDefPcsIts(nSerial);
						pView->m_mgrProcedure->m_nStepMk[1] = MK_END;
						break;
					}
				}
			}
			SetDelay1(100, 6);		// [mSec]
			pView->m_mgrProcedure->m_nStepMk[1]++;
		}
		else
		{
			pView->m_mgrProcedure->m_nStepMk[1] = MK_END;
		}
		break;
	case 7:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

		if (!WaitDelay1(6))		// F:Done, T:On Waiting....
		{
			pView->m_mgrProcedure->m_nMkPcs[1] = 0;

			if (!IsNoMk1())
			{
				pView->m_mgrProcedure->m_nStepMk[1]++;
			}
			else
			{
				if (IsReview1())
				{
					pView->m_mgrProcedure->m_nStepMk[1]++;
				}
				else
				{
					if (pView->m_mgrProcedure->m_bReview)
					{
						pView->m_mgrProcedure->m_nMkPcs[1] = pView->m_mgrReelmap->GetTotDefPcsIts(nSerial);
						pView->m_mgrProcedure->m_nStepMk[1] = MK_END;
					}
					else
						pView->m_mgrProcedure->m_nStepMk[1]++;
				}
			}
		}
		break;
	case 8:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

		if (pView->m_mgrProcedure->m_nMkPcs[1] < pView->m_mgrReelmap->GetTotDefPcsIts(nSerial))	// 마킹완료Check
		{
			if (pView->m_mgrProcedure->m_nMkPcs[1] + 1 < pView->m_mgrReelmap->GetTotDefPcsIts(nSerial))
			{
				ptPnt = pView->m_mgrReelmap->GetMkPntIts(nSerial, pView->m_mgrProcedure->m_nMkPcs[1] + 1);
				pView->m_mgrProcedure->m_dNextTarget[AXIS_X1] = ptPnt.x;
				pView->m_mgrProcedure->m_dNextTarget[AXIS_Y1] = ptPnt.y;
			}
			else
			{
				pView->m_mgrProcedure->m_dNextTarget[AXIS_X1] = -1.0;
				pView->m_mgrProcedure->m_dNextTarget[AXIS_Y1] = -1.0;
			}

			ptPnt = pView->m_mgrReelmap->GetMkPntIts(nSerial, pView->m_mgrProcedure->m_nMkPcs[1]);
			if (ptPnt.x < 0.0 && ptPnt.y < 0.0) // 양품화.
			{
				pView->m_mgrProcedure->m_nMkPcs[1]++;
				pView->m_mgrProcedure->m_nStepMk[1] = MK_DONE_CHECK;
				break;
			}

			nIdx = pView->m_mgrReelmap->GetMkStripIdxIts(nSerial, pView->m_mgrProcedure->m_nMkPcs[1]);
			if (nIdx > 0)
			{
				if (!IsMkStrip(nIdx)) // Strip[] Mk Off
				{
					pView->m_mgrProcedure->m_nMkPcs[1]++;
					pView->m_mgrProcedure->m_nStepMk[1] = MK_DONE_CHECK;
					break;
				}
				else
				{
					if (pView->m_mgrProcedure->m_nMkStrip[1][nIdx - 1] >= nStripOut)
					{
						pView->m_mgrProcedure->m_nMkPcs[1]++;
						pView->m_mgrProcedure->m_nStepMk[1] = MK_DONE_CHECK;
						break;
					}
					else
						pView->m_mgrProcedure->m_nMkStrip[1][nIdx - 1]++;
				}
			}
			else
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Strip Index Failed."));
				break;
			}

			pView->m_mgrProcedure->m_dTarget[AXIS_X1] = ptPnt.x;
			pView->m_mgrProcedure->m_dTarget[AXIS_Y1] = ptPnt.y;

			pView->m_mgrProcedure->m_nStepMk[1]++;
		}
		else
		{
			pView->m_mgrProcedure->m_nStepMk[1] = MK_END;
		}
		break;
	case 9:
		// Cam0 : m_bPriority[0] <-, m_bPriority[3] ->
		// Cam1 : m_bPriority[1] ->, m_bPriority[2] <-
		ptPnt.x = pView->m_mgrProcedure->m_dTarget[AXIS_X1];
		ptPnt.y = pView->m_mgrProcedure->m_dTarget[AXIS_Y1];

		if (pView->m_mgrProcedure->m_dNextTarget[AXIS_X1] < 0)
			pView->m_mgrProcedure->m_bCollision[1] = m_mgrProcedure->ChkCollision(AXIS_X1, pView->m_mgrProcedure->m_dTarget[AXIS_X1]);
		else
			pView->m_mgrProcedure->m_bCollision[1] = m_mgrProcedure->ChkCollision(AXIS_X1, pView->m_mgrProcedure->m_dTarget[AXIS_X1], pView->m_mgrProcedure->m_dNextTarget[AXIS_X1]);

		if (!pView->m_mgrProcedure->m_bCollision[1])
		{
			if (IsMoveDone1())
			{
				Move1(ptPnt, pView->m_mgrProcedure->m_bCam);
				pView->m_mgrProcedure->m_nStepMk[1]++;
			}
		}
		else if (pView->m_mgrProcedure->m_bPriority[1])
		{
			if (IsMoveDone1())
			{
				pView->m_mgrProcedure->m_bCollision[1] = FALSE;
				pView->m_mgrProcedure->m_bPriority[1] = FALSE;
				Move1(ptPnt, pView->m_mgrProcedure->m_bCam);
				pView->m_mgrProcedure->m_nStepMk[1]++;
			}
		}
		else if (pView->m_mgrProcedure->m_bPriority[3])
		{
			if (IsMoveDone1())
			{
				pView->m_mgrProcedure->m_bCollision[1] = FALSE;
				pView->m_mgrProcedure->m_bPriority[3] = FALSE;
				ptPnt.x = _tstof(pDoc->WorkingInfo.Motion.sSafeZone);
				ptPnt.y = 0.0;//m_dEnc[AXIS_Y1];
				Move1(ptPnt, FALSE);
			}
		}
		break;
	case 10:
		pView->m_mgrProcedure->m_nStepMk[1]++;
		break;
	case 11:
		pView->m_mgrProcedure->m_nStepMk[1]++;
		break;
	case 12:
		if (IsMoveDone1())
			pView->m_mgrProcedure->m_nStepMk[1]++;
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
			if (!SaveMk1Img(pView->m_mgrProcedure->m_nMkPcs[1]))
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Error-SaveMk1Img()"));
			}
		}
		pView->m_mgrProcedure->m_nStepMk[1]++;
		break;
	case 14:
		pView->m_mgrProcedure->m_nStepMk[1]++;
		break;
	case 15:
		pView->m_mgrProcedure->m_nStepMk[1]++;
		break;
	case 16:
		if (IsNoMk1())
		{
			if (!WaitDelay1(6))		// F:Done, T:On Waiting....
				pView->m_mgrProcedure->m_nStepMk[1]++;
		}
		else
			pView->m_mgrProcedure->m_nStepMk[1]++;
		break;
	case 17:
		if (!IsNoMk1())
		{
			if (IsMk1Done())
			{
				// One more MK On Start....
				if (!pView->m_mgrProcedure->m_nMkPcs[1] && !pView->m_mgrProcedure->m_bAnswer[3])
				{
					pView->m_mgrProcedure->m_bAnswer[3] = TRUE;
					Mk1();
				}
				else
				{
					pView->m_mgrProcedure->m_bAnswer[3] = FALSE;
					pView->m_mgrProcedure->m_nMkPcs[1]++;
					pView->m_mgrProcedure->m_nStepMk[1]++;
					pView->m_mgrProcedure->m_nStepMk[1]++;
				}

			}
			else
			{
				if (pView->m_mgrProcedure->m_dwStMkDn[1] + 5000 < GetTickCount())
				{
					BuzzerFromThread(TRUE, 0);
					DispMain(_T("정 지"), RGB_RED);
					m_pVoiceCoil[1]->SearchHomeSmac1();

					nRtn = AsyncMsgBox(_T("보이스코일(우) 통신완료가 않됩니다.\r\n마킹을 다시 시도하시겠습니까?"), 2, MB_YESNO);
					if (IDYES == nRtn)
					{
						DispMain(_T("운전중"), RGB_RED);
						pView->m_mgrProcedure->m_nStepMk[1] = 13;
					}
					else if (nRtn < 0)
						pView->m_mgrProcedure->m_nStepMk[1]++; // Wait...
					else
					{
						pView->m_mgrProcedure->m_bAnswer[3] = FALSE;
						pView->m_mgrProcedure->m_nMkPcs[1]++;
						pView->m_mgrProcedure->m_nStepMk[1]++;
						pView->m_mgrProcedure->m_nStepMk[1]++;
						Stop();
					}
				}
			}
		}
		else
		{
			pView->m_mgrProcedure->m_nMkPcs[1]++;
			pView->m_mgrProcedure->m_nStepMk[1]++;
			pView->m_mgrProcedure->m_nStepMk[1]++;
		}
		break;
	case 18:
		if ((nRtn = WaitRtnVal(2)) > -1)
		{
			if (IDYES == nRtn)
			{
				DispMain(_T("운전중"), RGB_RED);
				pView->m_mgrProcedure->m_nStepMk[1] = 13;
			}
			else
			{
				pView->m_mgrProcedure->m_bAnswer[3] = FALSE;
				pView->m_mgrProcedure->m_nMkPcs[1]++;
				pView->m_mgrProcedure->m_nStepMk[1]++;
				StopFromThread();
			}
		}
		break;
	case 19:
		pView->m_mgrProcedure->m_nStepMk[1] = MK_DONE_CHECK;
		break;
	case MK_DONE_CHECK:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

		if (pView->m_mgrProcedure->m_nMkPcs[1] < pView->m_mgrReelmap->GetTotDefPcsIts(nSerial))
		{
			if (IsNoMk1())
			{
				if (!pDoc->WorkingInfo.System.bNoMk)
				{
					if (IsReview1())
					{
						if (IsJogRtDn1())
							pView->m_mgrProcedure->m_nStepMk[1]++;
					}
					else if (!pDoc->WorkingInfo.LastJob.bVerify)
					{
						pView->m_mgrProcedure->m_nMkPcs[1] = pView->m_mgrReelmap->GetTotDefPcsIts(nSerial);
						pView->m_mgrProcedure->m_nStepMk[1] = MK_END;
					}
					else
						pView->m_mgrProcedure->m_nStepMk[1] = 8;	// 마킹완료Check
				}
				else
					pView->m_mgrProcedure->m_nStepMk[1] = 8;	// 마킹완료Check
			}
			else
				pView->m_mgrProcedure->m_nStepMk[1] = 8;	// 마킹완료Check
		}
		else
		{
			if (IsNoMk1())
			{
				if (IsReview1())
				{
					if (IsJogRtDn1())
						pView->m_mgrProcedure->m_nStepMk[1] = MK_END;
				}
				else
				{
					pView->m_mgrProcedure->m_nStepMk[1] = MK_END;
				}
			}
			else
				pView->m_mgrProcedure->m_nStepMk[1] = MK_END;
		}
		break;
	case MK_DONE_CHECK + 1:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

		if (pView->m_mgrProcedure->m_nMkPcs[1] < pView->m_mgrReelmap->GetTotDefPcsIts(nSerial))
		{
			if (IsNoMk1())
			{
				if (IsReview1())
				{
					if (IsJogRtUp1())
						pView->m_mgrProcedure->m_nStepMk[1] = 8;	// 마킹완료Check
				}
				else
				{
					pView->m_mgrProcedure->m_nMkPcs[1] = pView->m_mgrReelmap->GetTotDefPcsIts(nSerial);
					pView->m_mgrProcedure->m_nStepMk[1] = MK_END;
				}
			}
			else
				pView->m_mgrProcedure->m_nStepMk[1] = 8;	// 마킹완료Check
		}
		else
		{
			pView->m_mgrProcedure->m_nStepMk[1] = MK_END;
		}
		break;


	case MK_END:
		if (IsMoveDone1())
			pView->m_mgrProcedure->m_nStepMk[1]++;
		break;
	case 101:
		SetDelay1(100, 6);		// [mSec]
		pView->m_mgrProcedure->m_nStepMk[1]++;
		break;
	case 102:
		if (!WaitDelay1(6))		// F:Done, T:On Waiting....
			pView->m_mgrProcedure->m_nStepMk[1]++;
		break;
	case 103:
		if (!IsMkEdPos1())
		{
			pView->m_mgrProcedure->m_dTarget[AXIS_X1] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[1]);
			pView->m_mgrProcedure->m_dTarget[AXIS_Y1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[1]);
			pView->m_mgrProcedure->m_dNextTarget[AXIS_X1] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[1]);
			pView->m_mgrProcedure->m_dNextTarget[AXIS_Y1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[1]);

			MoveMkEdPos1();
		}

		pDoc->SaveMkCntR();
		pView->m_mgrProcedure->m_nStepMk[1]++;
		break;
	case 104:
		if (IsMoveDone1())
		{
			pView->m_mgrProcedure->m_nStepMk[1]++;
			SetDelay1(10000, 6);		// [mSec]
		}
		break;
	case 105:
		if (IsMkEdPos1())
		{
			pView->m_mgrProcedure->m_nStepMk[1]++;
		}
		else
		{
			if (!WaitDelay1(6))		// F:Done, T:On Waiting....
			{
				pView->m_mgrProcedure->m_nStepMk[1] = ERR_PROC;
			}
		}
		break;
	case 106: // MK Done....
		pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
		pView->m_mgrProcedure->m_bTHREAD_MK[1] = FALSE;
		break;

	case ERR_PROC:
		DispMain(_T("정 지"), RGB_RED);
		m_pVoiceCoil[1]->SearchHomeSmac1();
		AsyncMsgBox(_T("보이스코일(우) 초기위치 이동이 되지 않습니다.\r\n마킹상태를 확인하세요."), 2);
		pView->m_mgrProcedure->m_nStepMk[1]++;
		break;
	case ERR_PROC + 1:
		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

		if (pView->m_mgrProcedure->m_bCam)
			sMsg.Format(_T("%d번 Shot을 다시 불량확인을 하시겠습니까?"), nSerial);
		else
			sMsg.Format(_T("%d번 Shot을 다시 마킹하시겠습니까?"), nSerial);

		pView->m_mgrProcedure->m_nRtnMyMsgBoxIdx = 1;
		pView->m_mgrProcedure->m_bRtnMyMsgBox[1] = FALSE;
		pView->m_mgrProcedure->m_nRtnMyMsgBox[1] = -1;
		pView->AsyncMsgBox(sMsg, 2, MB_YESNO);
		sMsg.Empty();
		pView->m_mgrProcedure->m_nStepMk[1]++;
		break;
	case ERR_PROC + 2:
		if ((nRtn = WaitRtnVal()) > -1)
		{
			if (IDYES == nRtn)
			{
				pView->m_mgrProcedure->m_nStepMk[1] = ERR_PROC + 10;
			}
			else
			{
				pView->m_mgrProcedure->m_nRtnMyMsgBoxIdx = 1;
				pView->m_mgrProcedure->m_bRtnMyMsgBox[1] = FALSE;
				pView->m_mgrProcedure->m_nRtnMyMsgBox[1] = -1;
				sMsg.Format(_T("계속 다음 작업을 진행하시겠습니까?"), nSerial);
				pView->AsyncMsgBox(sMsg, 2, MB_YESNO);
				sMsg.Empty();

				pView->m_mgrProcedure->m_nStepMk[1]++;

			}
		}
		break;
	case ERR_PROC + 3:
		if ((nRtn = WaitRtnVal()) > -1)
		{
			if (IDYES == nRtn)
			{
				pView->m_mgrProcedure->m_nStepMk[1] = ERR_PROC + 20;
			}
			else
			{
				pView->m_mgrProcedure->m_bDispMsgDoAuto[9] = TRUE;
				pView->m_mgrProcedure->m_nStepDispMsg[9] = FROM_DOMARK1;
			}
		}
		break;
	case ERR_PROC + 10:
		pView->m_mgrProcedure->m_bReMark[1] = TRUE;
		pView->m_mgrProcedure->m_bTHREAD_MK[1] = FALSE;
		pView->m_mgrProcedure->m_nStepMk[1] = 0;
		break;
	case ERR_PROC + 20: // MK Done....
		pView->m_mgrProcedure->m_bDoneMk[1] = TRUE;
		pView->m_mgrProcedure->m_bTHREAD_MK[1] = FALSE;
		break;
	}
}


BOOL CGvisR2R_PunchView::SetMkIts(BOOL bRun)	// Marking Start
{
	CfPoint ptPnt;
	int nSerial, nTot, a, b;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	for (a = 0; a < 2; a++)
	{
		for (b = 0; b < MAX_STRIP_NUM; b++)
		{
			pView->m_mgrProcedure->m_nMkStrip[a][b] = 0;			// [nCam][nStrip]:[2][4] - [좌/우][] : 스트립에 펀칭한 피스 수 count
			pView->m_mgrProcedure->m_bRejectDone[a][b] = FALSE;	// [nCam][nStrip]:[2][4] - [좌/우][] : 스트립에 펀칭한 피스 수 count가 스트립 폐기 설정수 완료 여부 
		}
	}

	if (bRun)
	{
		if (pView->m_mgrProcedure->m_bDoMk[0])
		{
			if (!pView->m_mgrProcedure->m_bTHREAD_MK[0])
			{
				pView->m_mgrProcedure->m_nStepMk[0] = 0;
				pView->m_mgrProcedure->m_nMkPcs[0] = 0;
				pView->m_mgrProcedure->m_bDoneMk[0] = FALSE;
				pView->m_mgrProcedure->m_bTHREAD_MK[0] = TRUE;

				nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // Cam0

				pView->m_mgrProcedure->m_nTotMk[0] = nTot = pView->m_mgrReelmap->GetTotDefPcsIts(nSerial);
				pView->m_mgrProcedure->m_nCurMk[0] = 0;
				if (nTot > 0)
				{
					ptPnt = pView->m_mgrReelmap->GetMkPntIts(nSerial, 0);
					pView->m_mgrProcedure->m_dTarget[AXIS_X0] = ptPnt.x;
					pView->m_mgrProcedure->m_dTarget[AXIS_Y0] = ptPnt.y;
					if (nTot > 1)
					{
						ptPnt = pView->m_mgrReelmap->GetMkPntIts(nSerial, 1);
						pView->m_mgrProcedure->m_dNextTarget[AXIS_X0] = ptPnt.x;
						pView->m_mgrProcedure->m_dNextTarget[AXIS_Y0] = ptPnt.y;
					}
					else
					{
						pView->m_mgrProcedure->m_dNextTarget[AXIS_X0] = -1.0;
						pView->m_mgrProcedure->m_dNextTarget[AXIS_Y0] = -1.0;
					}
				}
				else
				{
					pView->m_mgrProcedure->m_dTarget[AXIS_X0] = -1.0;
					pView->m_mgrProcedure->m_dTarget[AXIS_Y0] = -1.0;
					pView->m_mgrProcedure->m_dNextTarget[AXIS_X0] = -1.0;
					pView->m_mgrProcedure->m_dNextTarget[AXIS_Y0] = -1.0;
				}
			}
		}

		if (pView->m_mgrProcedure->m_bDoMk[1])
		{
			if (!pView->m_mgrProcedure->m_bTHREAD_MK[1])
			{
				pView->m_mgrProcedure->m_nStepMk[1] = 0;
				pView->m_mgrProcedure->m_nMkPcs[1] = 0;
				pView->m_mgrProcedure->m_bDoneMk[1] = FALSE;
				pView->m_mgrProcedure->m_bTHREAD_MK[1] = TRUE;

				nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // Cam1

				pView->m_mgrProcedure->m_nTotMk[1] = nTot = pView->m_mgrReelmap->GetTotDefPcsIts(nSerial);
				pView->m_mgrProcedure->m_nCurMk[1] = 0;
				if (nTot > 0)
				{
					ptPnt = pView->m_mgrReelmap->GetMkPntIts(nSerial, 0);
					pView->m_mgrProcedure->m_dTarget[AXIS_X1] = ptPnt.x;
					pView->m_mgrProcedure->m_dTarget[AXIS_Y1] = ptPnt.y;
					if (nTot > 1)
					{
						ptPnt = pView->m_mgrReelmap->GetMkPntIts(nSerial, 1);
						pView->m_mgrProcedure->m_dNextTarget[AXIS_X1] = ptPnt.x;
						pView->m_mgrProcedure->m_dNextTarget[AXIS_Y1] = ptPnt.y;
					}
					else
					{
						pView->m_mgrProcedure->m_dNextTarget[AXIS_X1] = -1.0;
						pView->m_mgrProcedure->m_dNextTarget[AXIS_Y1] = -1.0;
					}
				}
				else
				{
					pView->m_mgrProcedure->m_dTarget[AXIS_X1] = -1.0;
					pView->m_mgrProcedure->m_dTarget[AXIS_Y1] = -1.0;
					pView->m_mgrProcedure->m_dNextTarget[AXIS_X1] = -1.0;
					pView->m_mgrProcedure->m_dNextTarget[AXIS_Y1] = -1.0;
				}
			}
		}
	}
	else
	{
		pView->m_mgrProcedure->m_bTHREAD_MK[0] = FALSE;
		pView->m_mgrProcedure->m_bTHREAD_MK[1] = FALSE;
	}
	return TRUE;
}

void CGvisR2R_PunchView::UpdateRstInner()
{
	if (m_pDlgMenu06)
		m_pDlgMenu06->UpdateRst();
}

//void CGvisR2R_PunchView::DispDefImgInner()
//{
//	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;
//
//	CString sNewLot;
//	BOOL bNewModel = FALSE;
//	int nSerial = 0;
//	int nNewLot = 0;
//	int nBreak = 0;
//
//	switch (pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF_INNER)
//	{
//		// CopyDefImg Start ============================================
//	case 0:
//		pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF_INNER++;
//		break;
//	case 1:
//		Sleep(300);
//		pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF_INNER++;
//		break;
//	case 2:
//		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[0]; // 좌측 Camera
//
//		if (IsDoneDispMkInfoInner())	 // Check 불량이미지 Display End
//		{
//			if (pView->m_mgrProcedure->m_bSerialDecrese)
//			{
//				if (ChkLastProc() && (nSerial < pView->m_mgrProcedure->m_nLotEndSerial))
//				{
//					SetSerialReelmapInner(nSerial, TRUE);	// Reelmap(좌) Display Start
//					SetSerialMkInfoInner(nSerial, TRUE);		// 불량이미지(좌) Display Start
//				}
//				else if (ChkLastProc() && nSerial < 1)
//				{
//					SetSerialReelmapInner(pView->m_mgrProcedure->m_nLotEndSerial - 1, TRUE);		// Reelmap(좌) Display Start
//					SetSerialMkInfoInner(pView->m_mgrProcedure->m_nLotEndSerial - 1, TRUE);		// 불량이미지(좌) Display Start
//				}
//				else
//				{
//					//SetFixPcs(nSerial);
//					SetSerialReelmapInner(nSerial);	// Reelmap(좌) Display Start
//					SetSerialMkInfoInner(nSerial);	// 불량이미지(좌) Display Start
//				}
//			}
//			else
//			{
//				if (ChkLastProc() && (nSerial > pView->m_mgrProcedure->m_nLotEndSerial))
//				{
//					SetSerialReelmapInner(nSerial, TRUE);	// Reelmap(좌) Display Start
//					SetSerialMkInfoInner(nSerial, TRUE);		// 불량이미지(좌) Display Start
//				}
//				else if (ChkLastProc() && nSerial < 1)
//				{
//					SetSerialReelmapInner(pView->m_mgrProcedure->m_nLotEndSerial + 1, TRUE);		// Reelmap(좌) Display Start
//					SetSerialMkInfoInner(pView->m_mgrProcedure->m_nLotEndSerial + 1, TRUE);		// 불량이미지(좌) Display Start
//				}
//				else
//				{
//					//SetFixPcs(nSerial);
//					SetSerialReelmapInner(nSerial);	// Reelmap(좌) Display Start
//					SetSerialMkInfoInner(nSerial);	// 불량이미지(좌) Display Start
//				}
//			}
//
//			pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF_INNER++;
//		}
//
//		break;
//
//	case 3:
//		if (IsDoneDispMkInfoInner())	 // Check 불량이미지(좌) Display End
//		{
//			nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // 우측 Camera
//
//			if (nSerial > 0)
//			{
//				if (pView->m_mgrProcedure->m_bSerialDecrese)
//				{
//					if (ChkLastProc() && (nSerial < pView->m_mgrProcedure->m_nLotEndSerial))
//						SetSerialMkInfoInner(nSerial, TRUE);	// 불량이미지(우) Display Start
//					else
//						SetSerialMkInfoInner(nSerial);		// 불량이미지(우) Display Start
//				}
//				else
//				{
//					if (ChkLastProc() && (nSerial > pView->m_mgrProcedure->m_nLotEndSerial))
//						SetSerialMkInfoInner(nSerial, TRUE);	// 불량이미지(우) Display Start
//					else
//						SetSerialMkInfoInner(nSerial);		// 불량이미지(우) Display Start
//				}
//			}
//			else
//			{
//				if (pView->m_mgrProcedure->m_bSerialDecrese)
//				{
//					if (ChkLastProc())
//						SetSerialMkInfoInner(pView->m_mgrProcedure->m_nLotEndSerial - 1, TRUE);	// 불량이미지(우) Display Start
//				}
//				else
//				{
//					if (ChkLastProc())
//						SetSerialMkInfoInner(pView->m_mgrProcedure->m_nLotEndSerial + 1, TRUE);	// 불량이미지(우) Display Start
//				}
//			}
//			pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF_INNER++;
//		}
//		break;
//	case 4:
//		nSerial = pView->m_mgrProcedure->m_nBufUpSerial[1]; // 우측 Camera
//
//		if (nSerial > 0)
//		{
//			pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF_INNER++;
//
//			if (pView->m_mgrProcedure->m_bSerialDecrese)
//			{
//				if (ChkLastProc() && (nSerial < pView->m_mgrProcedure->m_nLotEndSerial))
//				{
//					SetSerialReelmapInner(nSerial, TRUE);	// Reelmap(우) Display Start
//				}
//				else
//				{
//					//SetFixPcs(nSerial);
//					SetSerialReelmapInner(nSerial);			// Reelmap(우) Display Start
//				}
//			}
//			else
//			{
//				if (ChkLastProc() && (nSerial > pView->m_mgrProcedure->m_nLotEndSerial))
//				{
//					SetSerialReelmapInner(nSerial, TRUE);	// Reelmap(우) Display Start
//				}
//				else
//				{
//					//SetFixPcs(nSerial);
//					SetSerialReelmapInner(nSerial);			// Reelmap(우) Display Start
//				}
//			}
//		}
//		else
//		{
//			if (ChkLastProc())
//			{
//				pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF_INNER++;
//				if (pView->m_mgrProcedure->m_bSerialDecrese)
//					SetSerialReelmapInner(pView->m_mgrProcedure->m_nLotEndSerial - 1, TRUE);	// 불량이미지(우) Display Start
//				else
//					SetSerialReelmapInner(pView->m_mgrProcedure->m_nLotEndSerial + 1, TRUE);	// 불량이미지(우) Display Start
//			}
//			else
//			{
//				if (bDualTest)
//				{
//					if (pView->m_mgrProcedure->m_bLastProc && pView->m_mgrProcedure->m_nBufDnSerial[0] == pView->m_mgrProcedure->m_nLotEndSerial)
//						pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF_INNER++;
//					else
//					{
//						pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF_INNER++;
//					}
//				}
//				else
//				{
//					if (pView->m_mgrProcedure->m_bLastProc && pView->m_mgrProcedure->m_nBufUpSerial[0] == pView->m_mgrProcedure->m_nLotEndSerial)
//						pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF_INNER++;
//					else
//					{
//						pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF_INNER++;
//					}
//				}
//			}
//		}
//		break;
//	case 5:
//		pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF_INNER++;
//		break;
//	case 6:
//		pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF_INNER++;
//		break;
//	case 7:
//		pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF_INNER++;
//		break;
//	case 8:
//		pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF_INNER++;
//		break;
//	case 9:
//		pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF_INNER++;
//		break;
//	case 10:
//		pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF_INNER++;
//		break;
//
//	case 11:
//		if (IsDoneDispMkInfoInner() && IsRun())	 // Check 불량이미지(우) Display End
//			pView->m_mgrProcedure->m_nStepTHREAD_DISP_DEF_INNER++;
//		break;
//	case 12:
//		pView->m_mgrProcedure->m_bTHREAD_DISP_DEF_INNER = FALSE;
//		break;
//		// Disp DefImg End ============================================
//	}
//
//}

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

	pView->m_mgrReelmap->RemakeReelmap();

	if (pDoc->GetTestMode() == MODE_OUTER)
		pView->m_mgrReelmap->RemakeReelmapInner();
}

void CGvisR2R_PunchView::GetResult()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (bDualTest)
	{
		if (pView->m_mgrReelmap->m_pReelMapAllUp)
			pView->m_mgrReelmap->m_pReelMapAllUp->GetResult();
	}
	else
	{
		if (pView->m_mgrReelmap->m_pReelMapUp)
			pView->m_mgrReelmap->m_pReelMapUp->GetResult();
	}

	if (pDoc->GetTestMode() == MODE_OUTER)
	{
		if (pView->m_mgrReelmap->m_pReelMapIts)
			pView->m_mgrReelmap->m_pReelMapIts->GetResult();
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
		if (pView->m_mgrReelmap->m_pReelMapAllUp)
			strData = pView->m_mgrReelmap->m_pReelMapAllUp->GetResultTxt();
	}
	else
	{
		if (pView->m_mgrReelmap->m_pReelMapUp)
			strData = pView->m_mgrReelmap->m_pReelMapUp->GetResultTxt();
	}


	TCHAR lpszCurDirPathFile[MAX_PATH];
	_stprintf(lpszCurDirPathFile, _T("%s"), strDestPath);

	CFile file;
	CFileException pError;
	if (!file.Open(lpszCurDirPathFile, CFile::modeWrite, &pError))
	{
		if (!file.Open(lpszCurDirPathFile, CFile::modeCreate | CFile::modeWrite, &pError))
		{
			// 파일 오픈에 실패시 
#ifdef _DEBUG
			afxDump << _T("File could not be opened ") << pError.m_cause << _T("\n");
#endif
			return;
		}
	}
	//버퍼의 내용을 file에 복사한다.
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
	if (pView->m_mgrReelmap->m_pReelMapIts)
		strData = pView->m_mgrReelmap->m_pReelMapIts->GetResultTxt();


	TCHAR lpszCurDirPathFile[MAX_PATH];
	_stprintf(lpszCurDirPathFile, _T("%s"), strDestPath);

	CFile file;
	CFileException pError;
	if (!file.Open(lpszCurDirPathFile, CFile::modeWrite, &pError))
	{
		if (!file.Open(lpszCurDirPathFile, CFile::modeCreate | CFile::modeWrite, &pError))
		{
			// 파일 오픈에 실패시 
#ifdef _DEBUG
			afxDump << _T("File could not be opened ") << pError.m_cause << _T("\n");
#endif
			return;
		}
	}
	//버퍼의 내용을 file에 복사한다.
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
		if (pView->m_mgrReelmap->m_pReelMapAllUp)
			sSapp3Data = pView->m_mgrReelmap->m_pReelMapAllUp->GetSapp3Txt();
	}
	else
	{
		if (pView->m_mgrReelmap->m_pReelMapUp)
			sSapp3Data = pView->m_mgrReelmap->m_pReelMapUp->GetSapp3Txt();
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

//BOOL CGvisR2R_PunchView::UpdateReelmap(int nSerial)
//{
//	if (nSerial <= 0)
//	{
//		pView->ClrDispMsg();
//		AfxMessageBox(_T("Serial Error.54"));
//		return 0;
//	}
//
//	if (!pView->m_mgrReelmap->MakeMkDir())
//		return FALSE;
//
//	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
//
//	pView->m_mgrProcedure->m_nSerialRmapUpdate = nSerial;
//
//	pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_UP = TRUE;
//	if (bDualTest)
//	{
//		pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_DN = TRUE;
//		pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_ALLUP = TRUE;
//		pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_ALLDN = TRUE;
//	}
//
//	if (pDoc->GetTestMode() == MODE_INNER || pDoc->GetTestMode() == MODE_OUTER)
//		UpdateReelmapInner(nSerial);
//
//	Sleep(100);
//	return TRUE;
//}
//
//BOOL CGvisR2R_PunchView::UpdateReelmapInner(int nSerial)
//{
//	if (nSerial <= 0)
//	{
//		pView->ClrDispMsg();
//		AfxMessageBox(_T("Serial Error.54"));
//		return 0;
//	}
//
//	if (!pDoc->MakeMkDir())
//		return FALSE;
//
//	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
//
//	pView->m_mgrProcedure->m_nSerialRmapInnerUpdate = nSerial;
//
//	pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_INNER_UP = TRUE;
//	if (bDualTest)
//	{
//		pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_INNER_DN = TRUE;
//		pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_INNER_ALLUP = TRUE;
//		pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_INNER_ALLDN = TRUE;
//	}
//
//	pView->m_mgrProcedure->m_bTHREAD_UPDATE_REELMAP_ITS = TRUE;
//
//	Sleep(100);
//	return TRUE;
//}

void CGvisR2R_PunchView::Shift2Mk()
{
	int nSerialL = pView->m_mgrProcedure->m_nBufUpSerial[0];
	int nSerialR = pView->m_mgrProcedure->m_nBufUpSerial[1];
	int nLastShot = 0;

	if (pView->m_mgrProcedure->m_bLastProc && nSerialL == pView->m_mgrProcedure->m_nLotEndSerial)
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

	pView->m_mgrProcedure->m_bShift2Mk = FALSE;
}

//void CGvisR2R_PunchView::DelOverLotEndSerialUp(int nSerial)
//{
//	CString sSrc;
//
//	if (nSerial > 0)
//	{
//		sSrc.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufUp, nSerial);
//
//		if (pView->m_bSerialDecrese)
//		{
//			if (m_nLotEndSerial > 0 && nSerial < m_nLotEndSerial)
//			{
//				// Delete PCR File
//				pDoc->m_pFile->DeleteFolerOrFile(sSrc);
//			}
//		}
//		else
//		{
//			if (m_nLotEndSerial > 0 && nSerial > m_nLotEndSerial)
//			{
//				// Delete PCR File
//				pDoc->m_pFile->DeleteFolerOrFile(sSrc);
//			}
//		}
//	}
//
//}
//
//void CGvisR2R_PunchView::DelOverLotEndSerialDn(int nSerial)
//{
//	CString sSrc;
//
//	if (nSerial > 0)
//	{
//		sSrc.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufDn, nSerial);
//
//		if (pView->m_bSerialDecrese)
//		{
//			if (m_nLotEndSerial > 0 && nSerial < m_nLotEndSerial)
//			{
//				// Delete PCR File
//				pDoc->m_pFile->DeleteFolerOrFile(sSrc);
//			}
//		}
//		else
//		{
//			if (m_nLotEndSerial > 0 && nSerial > m_nLotEndSerial)
//			{
//				// Delete PCR File
//				pDoc->m_pFile->DeleteFolerOrFile(sSrc);
//			}
//		}
//	}
//
//}



void CGvisR2R_PunchView::ChkReTestAlarmOnAoiUp()
{
	int nSerial = pView->m_mgrProcedure->m_pBufSerial[0][pView->m_mgrProcedure->m_nBufTot[0] - 1];

	if (pView->m_mgrProcedure->m_bSerialDecrese)
	{
		if (pView->m_mgrProcedure->m_nLotEndSerial > 0 && nSerial > pView->m_mgrProcedure->m_nLotEndSerial)
		{
			//if (pDoc->m_sAlmMsg == GetAoiUpAlarmRestartMsg())
			{
				SetAoiUpAutoStep(2); // Wait for AOI 검사시작 신호.
				Sleep(300);
				if (m_pMpe)
					m_pMpe->Write(_T("MB44013B"), 1); // 검사부 상부 재작업 (시작신호) : PC가 On시키고 PLC가 Off
			}
		}
		else if(pView->m_mgrProcedure->m_nLotEndSerial > 0 && nSerial <= pView->m_mgrProcedure->m_nLotEndSerial)
		{
			if (m_pMpe)
				m_pMpe->Write(_T("MB44012B"), 1); // AOI 상 : PCR파일 Received
		}
	}
	else
	{
		if (pView->m_mgrProcedure->m_nLotEndSerial > 0 && nSerial < pView->m_mgrProcedure->m_nLotEndSerial)
		{
			//if (pDoc->m_sAlmMsg == GetAoiUpAlarmRestartMsg())
			{
				SetAoiUpAutoStep(2); // Wait for AOI 검사시작 신호.
				Sleep(300);
				if (m_pMpe)
					m_pMpe->Write(_T("MB44013B"), 1); // 검사부 상부 재작업 (시작신호) : PC가 On시키고 PLC가 Off
			}
		}
		else if (pView->m_mgrProcedure->m_nLotEndSerial > 0 && nSerial >= pView->m_mgrProcedure->m_nLotEndSerial)
		{
			if (m_pMpe)
				m_pMpe->Write(_T("MB44012B"), 1); // AOI 상 : PCR파일 Received
		}
	}

}


void CGvisR2R_PunchView::ChkReTestAlarmOnAoiDn()
{
	int nSerial = pView->m_mgrProcedure->m_pBufSerial[1][pView->m_mgrProcedure->m_nBufTot[1] - 1];

	if (pView->m_mgrProcedure->m_bSerialDecrese)
	{
		if (pView->m_mgrProcedure->m_nLotEndSerial > 0 && nSerial > pView->m_mgrProcedure->m_nLotEndSerial)
		{
			//if (pDoc->m_sAlmMsg == GetAoiDnAlarmRestartMsg())
			{
				SetAoiDnAutoStep(2); // Wait for AOI 검사시작 신호.
				Sleep(300);
				if (m_pMpe)
					m_pMpe->Write(_T("MB44013C"), 1); // 검사부 하부 재작업 (시작신호) : PC가 On시키고 PLC가 Off
			}
		}
		else if(pView->m_mgrProcedure->m_nLotEndSerial > 0 && nSerial <= pView->m_mgrProcedure->m_nLotEndSerial)
		{
			//if (m_pMpe)
			//	m_pMpe->Write(_T("MB44012C"), 1); // AOI 하 : PCR파일 Received
		}
	}
	else
	{
		if (pView->m_mgrProcedure->m_nLotEndSerial > 0 && nSerial < pView->m_mgrProcedure->m_nLotEndSerial)
		{
			//if (pDoc->m_sAlmMsg == GetAoiDnAlarmRestartMsg())
			{
				SetAoiDnAutoStep(2); // Wait for AOI 검사시작 신호.
				Sleep(300);
				if (m_pMpe)
					m_pMpe->Write(_T("MB44013C"), 1); // 검사부 하부 재작업 (시작신호) : PC가 On시키고 PLC가 Off
			}
		}
		else if (pView->m_mgrProcedure->m_nLotEndSerial > 0 && nSerial >= pView->m_mgrProcedure->m_nLotEndSerial)
		{
			//if (m_pMpe)
			//	m_pMpe->Write(_T("MB44012C"), 1); // AOI 하 : PCR파일 Received
		}
	}

}

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
	if (pView->m_mgrReelmap->m_pReelMapUp)
		pView->m_mgrReelmap->m_pReelMapUp->StartThreadFinalCopyItsFiles();

	return TRUE;
}

int CGvisR2R_PunchView::GetAoiUpCamMstInfo()
{
	return pView->m_mgrReelmap->GetAoiUpCamMstInfo();
}

int CGvisR2R_PunchView::GetAoiDnCamMstInfo()
{
	return pView->m_mgrReelmap->GetAoiDnCamMstInfo();
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

	pView->m_mgrProcedure->m_nMkStAuto = nStep;

	return nStep;
}

void CGvisR2R_PunchView::SetMkStAuto()
{
	CString strFolder, strTemp;
	strFolder.Format(_T("%s\\AutoStep.ini"), pDoc->WorkingInfo.System.sPathMkWork);
	strTemp.Format(_T("%d"), pView->m_mgrProcedure->m_nMkStAuto);
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

	pView->m_mgrProcedure->m_bMkSt = bMkSt;

	return bMkSt;
}

void CGvisR2R_PunchView::SetMkStSignal()
{
	CString strFolder, strTemp;
	strFolder.Format(_T("%s\\AutoStep.ini"), pDoc->WorkingInfo.System.sPathMkWork);
	strTemp.Format(_T("%d"), pView->m_mgrProcedure->m_bMkSt ? 1 : 0);
	::WritePrivateProfileString(_T("Auto"), _T("bMkStart"), strTemp, strFolder);
}

void CGvisR2R_PunchView::LoadSerial()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (bDualTest)
	{
		if (pDoc->m_ListBuf[1].nTot > 0) // AOI-Dn
		{
			pView->m_mgrProcedure->m_nBufDnSerial[0] = pDoc->m_ListBuf[1].Pop();
			pView->m_mgrProcedure->m_nBufUpSerial[0] = pView->m_mgrProcedure->m_nBufDnSerial[0];
			if (pDoc->m_ListBuf[1].nTot > 0) // AOI-Dn
			{
				pView->m_mgrProcedure->m_nBufDnSerial[1] = pDoc->m_ListBuf[1].Pop();
				pView->m_mgrProcedure->m_nBufUpSerial[1] = pView->m_mgrProcedure->m_nBufDnSerial[1];
			}
			else
			{
				pView->m_mgrProcedure->m_nBufDnSerial[1] = 0;
				pView->m_mgrProcedure->m_nBufUpSerial[1] = 0;
			}
		}
		else
		{
			pView->m_mgrProcedure->m_bLotEnd = TRUE;
			pView->m_mgrProcedure->m_nLotEndAuto = LOT_END;
		}

		if (pDoc->WorkingInfo.LastJob.bSampleTest)
		{
			if (pView->m_mgrProcedure->m_nBufUpSerial[0] == 1)
			{
				pView->m_mgrProcedure->m_nLotEndSerial = _tstoi(pDoc->WorkingInfo.LastJob.sSampleTestShotNum);
				pView->m_mgrProcedure->m_bLastProcFromUp = FALSE;
				pView->m_mgrProcedure->m_bLastProcFromEng = FALSE;
				pView->m_mgrProcedure->m_bLastProc = TRUE;
				if (m_pDlgMenu01)
					m_pDlgMenu01->m_bLastProc = TRUE;
				if (m_pMpe)
				{
					m_pMpe->Write(_T("MB440186"), 1);			// 잔량처리 AOI(하) 부터(PC가 On시키고, PLC가 확인하고 Off시킴)-20141112
					m_pMpe->Write(_T("MB440181"), 1);			// 잔량처리(PC가 On시키고, PLC가 확인하고 Off시킴)-20141031
				}
			}
		}
	}
	else
	{
		if (pDoc->m_ListBuf[0].nTot > 0) // AOI-Up
		{
			pView->m_mgrProcedure->m_nBufUpSerial[0] = pDoc->m_ListBuf[0].Pop();
			if (pDoc->m_ListBuf[0].nTot > 0) // AOI-Up
				pView->m_mgrProcedure->m_nBufUpSerial[1] = pDoc->m_ListBuf[0].Pop();
			else
				pView->m_mgrProcedure->m_nBufUpSerial[1] = 0;
		}
		else
		{
			pView->m_mgrProcedure->m_bLotEnd = TRUE;
			pView->m_mgrProcedure->m_nLotEndAuto = LOT_END;
		}

		if (pDoc->WorkingInfo.LastJob.bSampleTest)
		{
			if (pView->m_mgrProcedure->m_nBufUpSerial[0] == 1)
			{
				pView->m_mgrProcedure->m_nLotEndSerial = _tstoi(pDoc->WorkingInfo.LastJob.sSampleTestShotNum);
				pView->m_mgrProcedure->m_bLastProcFromUp = FALSE;
				pView->m_mgrProcedure->m_bLastProcFromEng = FALSE;
				pView->m_mgrProcedure->m_bLastProc = TRUE;
				if (m_pDlgMenu01)
					m_pDlgMenu01->m_bLastProc = TRUE;
				if (m_pMpe)
				{
					m_pMpe->Write(_T("MB440186"), 1);			// 잔량처리 AOI(하) 부터(PC가 On시키고, PLC가 확인하고 Off시킴)-20141112
					m_pMpe->Write(_T("MB440181"), 1);			// 잔량처리(PC가 On시키고, PLC가 확인하고 Off시킴)-20141031
				}
			}
		}
	}
}

BOOL CGvisR2R_PunchView::DoReset()
{
	pView->DispThreadTick();

	pView->m_mgrProcedure->m_bSwRun = FALSE;
	pView->m_mgrProcedure->m_bSwStop = FALSE;
	pView->m_mgrProcedure->m_bSwReady = FALSE;
	pView->m_mgrProcedure->m_bSwReset = TRUE;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pDoc->Status.bManual)
	{
		BOOL bInit = TRUE;

		if (pView->m_mgrProcedure->m_bTIM_CHK_DONE_READY)
		{
			pView->m_mgrProcedure->m_bTIM_CHK_DONE_READY = FALSE;
			pView->m_mgrProcedure->m_bReadyDone = FALSE;
#ifdef USE_MPE
			if (pView->m_pMpe)
				pView->m_pMpe->Write(_T("MB440100"), 0);	// PLC 운전준비 완료(PC가 확인하고 Reset시킴.)
#endif
		}
		pView->ClrDispMsg();

		if (pView->m_pEngrave)
		{
			pDoc->BtnStatus.EngAuto.Init = TRUE;
			pDoc->BtnStatus.EngAuto.IsInit = FALSE;
			pView->m_pEngrave->SwEngAutoInit(TRUE);
		}

		if (IDNO == pView->MsgBox(_T("초기화를 하시겠습니까?"), 0, MB_YESNO, DEFAULT_TIME_OUT, FALSE))
			bInit = FALSE;
		else
		{
			pDoc->m_bDoneChgLot = FALSE;
			pView->m_mgrProcedure->m_nNewLot = 0;

			if (pView->m_pDlgMenu01)
				pView->m_pDlgMenu01->ClrInfo();
		}

		if (!bInit)
		{
			if (IDNO == pView->MsgBox(_T("이어가기를 하시겠습니까?"), 0, MB_YESNO, DEFAULT_TIME_OUT, FALSE))
			{
				pView->m_mgrProcedure->m_bCont = FALSE;
				return FALSE;
			}
			pView->m_mgrProcedure->m_bCont = TRUE;
		}
		pView->InitAuto(bInit);

		pView->m_mgrReelmap->SetPathAtBuf();

		pView->SetAoiDummyShot(0, pView->GetAoiUpDummyShot());

		if (bDualTest)
			pView->SetAoiDummyShot(1, pView->GetAoiDnDummyShot());

		pView->m_mgrProcedure->m_bAoiFdWrite[0] = FALSE;
		pView->m_mgrProcedure->m_bAoiFdWrite[1] = FALSE;
		pView->m_mgrProcedure->m_bAoiFdWriteF[0] = FALSE;
		pView->m_mgrProcedure->m_bAoiFdWriteF[1] = FALSE;
		pView->m_mgrProcedure->m_bCycleStop = FALSE;
		pView->m_mgrProcedure->m_bContDiffLot = FALSE;

		pView->m_mgrProcedure->m_bInit = bInit;

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
				pView->m_mgrReelmap->DelPcrAll();
			}
			else
			{
				pView->m_mgrProcedure->m_bIsBuf[0] = FALSE;
				pView->m_mgrProcedure->m_bIsBuf[1] = FALSE;
			}
		}
		else
		{
			if (bExistSup || bExistBup)
			{
				pView->m_mgrReelmap->DelPcrAll();
			}
			else
			{
				pView->m_mgrProcedure->m_bIsBuf[0] = FALSE;
				pView->m_mgrProcedure->m_bIsBuf[1] = FALSE;
			}
		}

		pView->TowerLamp(RGB_RED, TRUE, FALSE);
		pView->DispMain(_T("정 지"), RGB_RED);
		if(m_pDlgMenu03)
			m_pDlgMenu03->SwAoiReset(TRUE);
		pView->m_mgrReelmap->OpenReelmap();

		if (bInit)
		{
			// 초기화
			pDoc->m_ListBuf[0].Clear();
			pDoc->m_ListBuf[1].Clear();
		}
		else
		{
			// 이어가기
			SetListBuf();
		}

		return TRUE;
	}

	return FALSE;
}

//void CGvisR2R_PunchView::DoReady()
//{
//	//pView->IoWrite("MB440162", 0); // 마킹부 정지 스위치 램프 ON(PC가 On/Off시킴)  - 20141021	
//	//pView->m_pMpe->Write(_T("MB440162", 0);
//	if (pView->m_mgrProcedure->m_bTIM_CHK_DONE_READY)
//	{
//		pView->m_mgrProcedure->m_bTIM_CHK_DONE_READY = FALSE;
//		KillTimer(TIM_CHK_DONE_READY);
//	}
//	ChkReadyDone();
//	pView->m_mgrReelmap->DelPcrAll();
//}


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
	if (m_pEngrave)
		m_pEngrave->SwMenu01DispDefImg(bOn);
}

BOOL CGvisR2R_PunchView::IsMotionDone(int nAxisId)
{
	BOOL bDone = FALSE;
	if(m_pMotion)
		bDone = m_pMotion->IsMotionDone(nAxisId);

	return bDone;
}