
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
	ON_MESSAGE(WM_CLIENT_RECEIVED, wmClientReceived)
END_MESSAGE_MAP()

// CGvisR2R_PunchView 생성/소멸

CGvisR2R_PunchView::CGvisR2R_PunchView()
	: CFormView(IDD_GVISR2R_PUNCH_FORM)
{
	// TODO: 여기에 생성 코드를 추가합니다.
	int nCam, nPoint;

	pView = this;
	m_mgrReelmap = NULL;
	m_mgrProcedure = NULL;
	m_mgrPunch = NULL;
	m_pEngrave = NULL;

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
	m_bSwStop = FALSE; m_bSwStopF = FALSE;
	m_bSwReset = FALSE; m_bSwResetF = FALSE;
	m_bSwReady = FALSE; m_bSwReadyF = FALSE;
	m_bTIM_SAFTY_STOP = FALSE;
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

	m_bTIM_MPE_IO = FALSE;
	m_bTIM_DISP_STATUS = FALSE;

	m_bDestroyedView = FALSE;
	m_bLoadMstInfo = FALSE;
	m_bLoadMstInfoF = FALSE;

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
		delete m_mgrPunch; // H/W Device 소멸.....
		m_mgrPunch = NULL;
	}

	if (m_pEngrave)
	{
		m_pEngrave->Close();
		delete m_pEngrave;
		m_pEngrave = NULL;
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

	if (!m_bTIM_INIT_VIEW)
	{
		m_nStepInitView = 0;
		m_bTIM_INIT_VIEW = TRUE;
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

BOOL CGvisR2R_PunchView::CheckConectionSr1000w()
{
	return m_mgrPunch->CheckConectionSr1000w();
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

		switch (m_nStepInitView)
		{
		case 0:
			m_nStepInitView++;
			DoDispMsg(_T("프로그램을 초기화합니다."), _T("알림"), RGB_GREEN, DELAY_TIME_MSG);
			InitMgrPunch(); //HwInit();	
			InitMgrReelmap();
			break;
		case 1:
#ifdef USE_TCPIP
			if (!m_pEngrave)
			{
				m_pEngrave = new CEngrave(pDoc->WorkingInfo.System.sIpClient[ID_PUNCH], pDoc->WorkingInfo.System.sIpServer[ID_ENGRAVE], pDoc->WorkingInfo.System.sPort[ID_ENGRAVE], this);
				m_pEngrave->SetHwnd(this->GetSafeHwnd());
			}
#endif
			m_nStepInitView++;
			break;
		case 2:
			m_nStepInitView++;
			m_mgrProcedure->m_nMonAlmF = 0;
			m_mgrProcedure->m_nClrAlmF = 0;
			break;
		case 3:
			m_nStepInitView++;
			break;
		case 4:
			m_nStepInitView++;
			break;
		case 5:
			m_nStepInitView++;
			DispMsg(_T("화면구성을 생성합니다.- 1"), _T("알림"), RGB_GREEN, DELAY_TIME_MSG);
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
			DispMsg(_T("화면구성을 생성합니다.-2"), _T("알림"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_MENU_01);
			if (bDualTest)
				m_pDlgMenu01->SelMap(ALL);
			else
				m_pDlgMenu01->SelMap(UP);
			break;
		case 9:
			m_nStepInitView++;
			DispMsg(_T("화면구성을 생성합니다.- 3"), _T("알림"), RGB_GREEN, DELAY_TIME_MSG);
			//ShowDlg(IDD_DLG_MENU_02);
			break;
		case 10:
			m_nStepInitView++;
			DispMsg(_T("화면구성을 생성합니다.- 4"), _T("알림"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_MENU_03);
			break;
		case 11:
			m_nStepInitView++;
			DispMsg(_T("화면구성을 생성합니다.- 5"), _T("알림"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_MENU_04);
			break;
		case 12:
			m_nStepInitView++;
			DispMsg(_T("화면구성을 생성합니다.- 6"), _T("알림"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_MENU_05);
			break;
		case 13:
			m_nStepInitView++;
			DispMsg(_T("화면구성을 생성합니다.- 7"), _T("알림"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_MENU_06);
			break;
		case 14:
			m_nStepInitView++;
			DispMsg(_T("화면구성을 생성합니다.- 8"), _T("알림"), RGB_GREEN, DELAY_TIME_MSG);
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
			DispMsg(_T("H/W를 초기화합니다."), _T("알림"), RGB_GREEN, DELAY_TIME_MSG);
			m_mgrPunch->InitAct();
			m_mgrProcedure->m_bStopFeeding = TRUE;
			MpeWrite(_T("MB440115"), 1); // 마킹부Feeding금지
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
			MpeWrite(_T("MB440115"), 0); // 마킹부Feeding금지
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
			m_mgrPunch->SetPlcParam();
			m_mgrPunch->GetPlcParam();
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
		if (!(pDoc->m_pMpeSignal[7] & (0x01 << 3)))	// 일시정지사용(PC가 On시키고, PLC가 확인하고 Off시킴)-20141031
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
		MsgBox(_T("일시정지 - 마킹부 안전센서가 감지되었습니다."));
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
				m_pDlgMenu02->ChgModelUp(); // PinImg, AlignImg를 Display함.
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

BOOL CGvisR2R_PunchView::MemChk() // 릴맵화면에 표시할 데이터를 담을 구조체 할당을 확인함.
{
	if (!m_mgrReelmap->m_pPcr[0] || !m_mgrReelmap->m_pPcr[1])// || !m_mgrReelmap->m_pReelMap)
		return FALSE;
	return TRUE;
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

void CGvisR2R_PunchView::Buzzer(BOOL bOn, int nCh)
{
	m_mgrPunch->Buzzer(bOn, nCh);
}

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

int CGvisR2R_PunchView::ChkSerial() // // 0: Continue, -: Previous, +: Discontinue --> 0: Same Serial, -: Decrese Serial, +: Increase Serial
{
	int nSerial0 = m_mgrProcedure->GetBuffer0(); // 첫번째 버퍼 시리얼 : 상하 시리얼이 다르면 0
	int nSerial1 = m_mgrProcedure->GetBuffer1(); // 두번째 버퍼 시리얼 : 상하 시리얼이 다르면 0
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

void CGvisR2R_PunchView::GetEnc()
{
	m_mgrPunch->GetEnc();
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

void CGvisR2R_PunchView::SetAoiFdPitch(double dPitch)
{
	pDoc->SetAoiFdPitch(dPitch);
}

void CGvisR2R_PunchView::SetMkFdPitch(double dPitch)
{
	pDoc->SetMkFdPitch(dPitch);
	long lData = long(dPitch*1000.0);
#ifdef USE_MPE
	MpeWrite(_T("ML45014"), lData); // 마킹부 Feeding 롤러 Lead Pitch
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

void CGvisR2R_PunchView::Shift2Buf()	// 버퍼폴더의 마지막 시리얼과 Share폴더의 시리얼이 연속인지 확인 후 옮김.
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
	// 마킹부 - FD/TQ 진공 ON, TBL진공 OFF, TBL파기 ON, 
	bOn[0] = m_pDlgMenu03->IsMkFdVac(); // TRUE
	bOn[1] = m_pDlgMenu03->IsMkTqVac(); // TRUE
	bOn[2] = m_pDlgMenu03->IsMkTblVac(); // FALSE
	bOn[3] = m_pDlgMenu03->IsMkTblBlw(); // TRUE

	if (bOn[0] && bOn[1] && !bOn[2] && bOn[3])
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
		MpeWrite(_T("MB440151"), 1);	// 한판넬 이송상태 ON (PC가 ON, OFF)
#endif
		CString sData, sPath = PATH_WORKING_INFO;
		pDoc->WorkingInfo.LastJob.bMkOnePnl = pDoc->WorkingInfo.LastJob.bAoiOnePnl = TRUE;
		sData.Format(_T("%d"), pDoc->WorkingInfo.LastJob.bMkOnePnl ? 1 : 0);
		::WritePrivateProfileString(_T("Last Job"), _T("Marking One Pannel Move On"), sData, sPath);
		::WritePrivateProfileString(_T("Last Job"), _T("AOI One Pannel Move On"), sData, sPath);
	}
}

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
		MpeWrite(_T("MB440151"), 1);	// 한판넬 이송상태 ON (PC가 ON, OFF)

		CString sData, sPath = PATH_WORKING_INFO;
		pDoc->WorkingInfo.LastJob.bMkOnePnl = pDoc->WorkingInfo.LastJob.bAoiOnePnl = TRUE;
		sData.Format(_T("%d"), pDoc->WorkingInfo.LastJob.bMkOnePnl ? 1 : 0);
		::WritePrivateProfileString(_T("Last Job"), _T("Marking One Pannel Move On"), sData, sPath);
		::WritePrivateProfileString(_T("Last Job"), _T("AOI One Pannel Move On"), sData, sPath);
	}
}

BOOL CGvisR2R_PunchView::IsMkFd()
{
	if (m_mgrProcedure->m_nShareDnCnt > 0)
	{
		if (!(m_mgrProcedure->m_nShareDnCnt % 2))
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
		if (m_mgrProcedure->m_nShareUpCnt > 0)
		{
			if (!(m_mgrProcedure->m_nShareUpCnt % 2))
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
	if (m_sDispMain == _T("정 지"))
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
		MpeWrite(_T("MB003828"), 1); // 검사부 상 검사 시작 <-> Y4368 I/F
		MpeWrite(_T("MB003928"), 1); // 검사부 하 검사 시작 <-> Y4468 I/F
	}
	else
	{
		MpeWrite(_T("MB003828"), 0); // 검사부 상 검사 시작 <-> Y4368 I/F
		MpeWrite(_T("MB003928"), 0); // 검사부 하 검사 시작 <-> Y4468 I/F
	}
#endif
}

void CGvisR2R_PunchView::SetTest0(BOOL bOn)
{
#ifdef USE_MPE
	if (bOn)
		MpeWrite(_T("MB003828"), 1); // 검사부 상 검사 시작 <-> Y4368 I/F
	else
		MpeWrite(_T("MB003828"), 0); // 검사부 상 검사 시작 <-> Y4368 I/F
#endif
}

void CGvisR2R_PunchView::SetTest1(BOOL bOn)
{
#ifdef USE_MPE
	if (bOn)
		MpeWrite(_T("MB003928"), 1); // 검사부 하 검사 시작 <-> Y4468 I/F
	else
		MpeWrite(_T("MB003928"), 0); // 검사부 하 검사 시작 <-> Y4468 I/F
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

	MpeWrite(_T("MB440100"), 0); // PLC 운전준비 완료(PC가 확인하고 Reset시킴.)
	MpeWrite(_T("MB440110"), 0); // 마킹시작(PC가 확인하고 Reset시킴.)-20141029
	MpeWrite(_T("MB440150"), 0); // 마킹부 마킹중 ON (PC가 ON, OFF)
	MpeWrite(_T("MB440170"), 0); // 마킹완료(PLC가 확인하고 Reset시킴.)-20141029

	InitAutoEngSignal();

	m_mgrPunch->MoveInitPos1();
	Sleep(30);
	m_mgrPunch->MoveInitPos0();

	m_mgrPunch->InitIoWrite();

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

	pDoc->m_nEjectBufferLastShot = -1;

	pDoc->m_bUpdateForNewJob[0] = FALSE;
	pDoc->m_bUpdateForNewJob[1] = FALSE;

	if (bInit) // 이어가기가 아닌경우.
	{
		MpeWrite(_T("MB440187"), 0); // 이어가기(PC가 On시키고, PLC가 확인하고 Off시킴)-20141121

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
		MpeWrite(_T("MB440187"), 1); // 이어가기(PC가 On시키고, PLC가 확인하고 Off시킴)-20141121
		m_mgrReelmap->ClrFixPcs();

#ifndef TEST_MODE
		m_mgrReelmap->ReloadReelmap();
		UpdateRst();
#endif
		DispLotStTime();
		m_mgrReelmap->RestoreReelmap();
	}
}

void CGvisR2R_PunchView::SetListBuf()	// pDoc->m_ListBuf에 버퍼 폴더의 시리얼번호를 가지고 재갱신함.
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
			if (m_pEngrave)
				m_pEngrave->SwMenu01UpdateWorking(TRUE);
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

	return m_mgrReelmap->GetErrCodeUp(nSerial);
}

int CGvisR2R_PunchView::GetErrCodeDn(int nSerial) // 1(정상), -1(Align Error, 노광불량), -2(Lot End)
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

CfPoint CGvisR2R_PunchView::GetMkPnt(int nSerial, int nMkPcs) // pcr 시리얼, pcr 불량 피스 읽은 순서 인덱스
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

void CGvisR2R_PunchView::Move1(CfPoint pt, BOOL bCam)
{
	m_mgrPunch->Move1(pt, bCam);
}

BOOL CGvisR2R_PunchView::LoadPcrUp(int nSerial, BOOL bFromShare)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.52"));
		return 0;
	}

	int nHeadInfo = m_mgrReelmap->LoadPCR0(nSerial); // 2(Failed), 1(정상), -1(Align Error, 노광불량), -2(Lot End)
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

	int nHeadInfo = m_mgrReelmap->LoadPCR1(nSerial); // 2(Failed), 1(정상), -1(Align Error, 노광불량), -2(Lot End)
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
		OfSt.x = m_dMkFdOffsetX[0][0]; // -: 제품 덜옴, +: 제품 더옴.
		OfSt.y = m_dMkFdOffsetY[0][0]; // -: 제품 덜옴, +: 제품 더옴.
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

	//if (m_pEngrave)
	//	m_pEngrave->SwMenu01DispDefImg(TRUE);
	SwMenu01DispDefImg(TRUE);
}

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
	return m_mgrProcedure->m_nLotEndSerial; // 테이블상에 정지하는 Serial.
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

BOOL CGvisR2R_PunchView::IsReview()
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

BOOL CGvisR2R_PunchView::SetCollision(double dCollisionMargin)
{
	BOOL bRtn = FALSE;
	if (m_mgrPunch->m_pMotion)
		bRtn = m_mgrPunch->m_pMotion->SetCollision(dCollisionMargin);
	return bRtn;
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

void CGvisR2R_PunchView::ClrAlarm()
{
	m_mgrPunch->ClrAlarm();
}

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
	int nMaxStrip;
#ifdef USE_CAM_MASTER
	if (pView->m_mgrReelmap)
		nMaxStrip = pView->m_mgrReelmap->m_Master[0].GetStripNum(); // 총 스트립의 갯수
	else
		nMaxStrip = MAX_STRIP;
#else
	nMaxStrip = MAX_STRIP;
#endif

	if (!m_pDlgMenu01 || nStripIdx < 1 || nStripIdx > nMaxStrip)
		return TRUE;

	return (m_pDlgMenu01->GetChkStrip(nStripIdx - 1));
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
			MpeWrite(_T("MB440184"), 0);	// 로트분리사용(PC가 On시키고, PC가 확인하고 Off시킴)-20141031
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

void CGvisR2R_PunchView::SetDualTest(BOOL bOn)
{
#ifdef USE_MPE
	//if (pView->m_pMpe)
		MpeWrite(_T("MB44017A"), bOn ? 0 : 1);		// 단면 검사 On
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
			::WritePrivateProfileString(_T("Last Job"), _T("Two Metal On"), _T("1"), PATH_WORKING_INFO);// IDC_CHK_TWO_METAL - Uncoiler\r역방향 ON : TRUE	
		}
		else
		{
			pDoc->WorkingInfo.LastJob.bTwoMetal = FALSE;
			MpeWrite(_T("MB44017C"), 0);
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
			MpeWrite(_T("MB44017D"), 1);
			::WritePrivateProfileString(_T("Last Job"), _T("One Metal On"), _T("1"), PATH_WORKING_INFO);// IDC_CHK_ONE_METAL - Recoiler\r정방향 CW : FALSE
		}
		else
		{
			pDoc->WorkingInfo.LastJob.bOneMetal = FALSE;
			MpeWrite(_T("MB44017D"), 0);
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
		MpeWrite(_T("MB440151"), 1);	// 한판넬 이송상태 ON (PC가 ON, OFF)
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
	MpeWrite(_T("MB440199"), 1);		// 각인부 피딩 CW ON (PLC가 피딩완료 후 OFF)
	MpeWrite(_T("ML45078"), lData);		// 각인부 Feeding 롤러 Offset(*1000, +:더 보냄, -덜 보냄, PC가 쓰고 PLC에서 지움)
#endif
}

BOOL CGvisR2R_PunchView::IsEngraveFd()
{
	if (m_mgrProcedure->m_nShareDnCnt > 0)
	{
		if (!(m_mgrProcedure->m_nShareDnCnt % 2))
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
		if (m_mgrProcedure->m_nShareUpCnt > 0)
		{
			if (!(m_mgrProcedure->m_nShareUpCnt % 2))
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
		MpeWrite(_T("MB440151"), 1);	// 한판넬 이송상태 ON (PC가 ON, OFF)
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
	MpeWrite(_T("MB440160"), 1);	// 검사부 피딩 ON (PLC가 피딩완료 후 OFF)
	MpeWrite(_T("ML45064"), lData);	// 검사부 Feeding 롤러 Offset(*1000, +:더 보냄, -덜 보냄)
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

	MpeWrite(_T("MB440103"), 0); // 2D(GUI) 각인 동작 Start신호(PLC On->PC Off)
	MpeWrite(_T("MB440173"), 0); // 2D(GUI) 각인 동작Running신호(PC On->PC Off)
	MpeWrite(_T("MB440174"), 0); // 각인부 작업완료.(PC가 On, PLC가 확인 후 Off)

	MpeWrite(_T("MB440105"), 0); // 각인부 2D 리더 시작신호(PLC On->PC Off)
	MpeWrite(_T("MB440178"), 0); // 각인부 2D 리더 작업중 신호(PC On->PC Off)
	MpeWrite(_T("MB440179"), 0); // 각인부 2D 리더 작업완료 신호.(PC가 On, PLC가 확인 후 Off)

	MpeWrite(_T("MB440102"), 0); // 각인부 Feeding완료(PLC가 On시키고 PC가 확인하고 Reset시킴.)

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

BOOL CGvisR2R_PunchView::Is2dReadDone()
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
	int nIdx = m_mgrReelmap->GetPcrIdx(nSerial);					// 릴맵화면버퍼 인덱스
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
					MpeWrite(_T("MB440186"), 1);			// 잔량처리 AOI(하) 부터(PC가 On시키고, PLC가 확인하고 Off시킴)-20141112
					MpeWrite(_T("MB440181"), 1);			// 잔량처리(PC가 On시키고, PLC가 확인하고 Off시킴)-20141031
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
					MpeWrite(_T("MB440186"), 1);			// 잔량처리 AOI(하) 부터(PC가 On시키고, PLC가 확인하고 Off시킴)-20141112
					MpeWrite(_T("MB440181"), 1);			// 잔량처리(PC가 On시키고, PLC가 확인하고 Off시킴)-20141031
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
				MpeWrite(_T("MB440100"), 0);	// PLC 운전준비 완료(PC가 확인하고 Reset시킴.)
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
			m_mgrProcedure->m_nNewLot = 0;

			if (m_pDlgMenu01)
				m_pDlgMenu01->ClrInfo();
		}

		if (!bInit)
		{
			if (IDNO == pView->MsgBox(_T("이어가기를 하시겠습니까?"), 0, MB_YESNO, DEFAULT_TIME_OUT, FALSE))
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
		pView->DispMain(_T("정 지"), RGB_RED);
		if(m_pDlgMenu03)
			m_pDlgMenu03->SwAoiReset(TRUE);
		m_mgrReelmap->OpenReelmap();

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
	if (m_pEngrave)
		m_pEngrave->SwMenu01DispDefImg(bOn);
}

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

BOOL CGvisR2R_PunchView::GetItsSerialInfo(int nItsSerial, BOOL &bDualTest, CString &sLot, CString &sLayerUp, CString &sLayerDn, int nOption)		// 내층에서의 ITS 시리얼의 정보
{
	return m_mgrReelmap->GetItsSerialInfo(nItsSerial, bDualTest, sLot, sLayerUp, sLayerDn, nOption);
}

void CGvisR2R_PunchView::InitMstData()
{
	m_mgrReelmap->InitMstData();
}

void CGvisR2R_PunchView::SwMenu01UpdateWorking(BOOL bOn)
{
	m_pEngrave->SwMenu01UpdateWorking(bOn);
}

void CGvisR2R_PunchView::ChangeModel()
{
	m_mgrProcedure->InitInfo();
	pView->InitMstData();

	//if (m_pEngrave)
	//	m_pEngrave->SwMenu01UpdateWorking(TRUE);
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
		m_pDlgMenu02->ChgModelUp(); // PinImg, AlignImg를 Display함.
		m_pDlgMenu02->InitCadImg();
	}
}

CfPoint CGvisR2R_PunchView::GetMkPntIts(int nSerial, int nMkPcs) // pcr 시리얼, pcr 불량 피스 읽은 순서 인덱스
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

BOOL CGvisR2R_PunchView::ChkYield() //(double &dTotLmt, double &dPrtLmt, double &dRatio) // 수율 양호 : TRUE , 수율 불량 : FALSE
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
