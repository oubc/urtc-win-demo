// DeviceTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SdkTestDemo.h"
#include "DeviceTestDlg.h"
#include "afxdialogex.h"
#include "SdkUtil.h"
#include "UCloudRtcComDefine.h"


// CDeviceTestDlg dialog

IMPLEMENT_DYNAMIC(CDeviceTestDlg, CDialogEx)

CDeviceTestDlg::CDeviceTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SDKTESTDEMO_DIALOG, pParent)
{
}

CDeviceTestDlg::~CDeviceTestDlg()
{
}

void CDeviceTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO1, m_videocom);
	DDX_Control(pDX, IDC_COMBO2, m_miccom);
	DDX_Control(pDX, IDC_MIC_ENY, m_micenypro);
	DDX_Control(pDX, IDC_STATIC_VIEW, m_videoview);
	DDX_Control(pDX, IDC_COMBO_SPEAKER, m_speaker);
	DDX_Control(pDX, IDC_SLIDER_MIC_VOL, m_micvol);
	DDX_Control(pDX, IDC_SLIDER_SPEAKER_VOL, m_speakervol);
}

BOOL CDeviceTestDlg::OnInitDialog()
{
	m_mictest = false;
	m_speakertest = false;
	m_videotest = false;
	CDialogEx::OnInitDialog();

	m_mediacallback = new MediaCallback(this->GetSafeHwnd());
	m_mediadevice = UCloudRtcMediaDevice::sharedInstance();

	m_mediadevice->InitAudioMoudle();
	m_mediadevice->InitVideoMoudle();
	
	int num = m_mediadevice->getCamNums();
	for (int i=0; i<num; i++)
	{
		tUCloudRtcDeviceInfo info;
		memset(&info, 0, sizeof(tUCloudRtcDeviceInfo));
		int ret = m_mediadevice->getVideoDevInfo(i, &info);
		if (ret == 0)
		{
			m_videolist.push_back(info);
			m_videocom.InsertString(i,Utf8ToWide(info.mDeviceName).data());
		}
	}

	int audionum = m_mediadevice->getRecordDevNums();
	for (int i = 0; i < audionum; i++)
	{
		tUCloudRtcDeviceInfo info;
		memset(&info, 0, sizeof(tUCloudRtcDeviceInfo));
		int ret = m_mediadevice->getRecordDevInfo(i, &info);
		if (ret == 0)
		{
			if (i == 0)
			{
				m_mediadevice->setRecordDevice(&info);
			}
			m_miclist.push_back(info) ;
			m_miccom.InsertString(i, Utf8ToWide(info.mDeviceName).data());
		}
	}

	int speakernum = m_mediadevice->getPlayoutDevNums();
	for (int i = 0; i < audionum; i++)
	{
		tUCloudRtcDeviceInfo info;
		memset(&info, 0, sizeof(tUCloudRtcDeviceInfo));
		int ret = m_mediadevice->getPlayoutDevInfo(i, &info);
		if (ret == 0)
		{
			if (i == 0)
			{
				m_mediadevice->setPlayoutDevice(&info);
			}
			m_spkeakerlist.push_back(info);
			m_speaker.InsertString(i,Utf8ToWide(info.mDeviceName).data());
		}
	}
	m_micenypro.SetRange(0, 255);
	m_miccom.UpdateData();
	m_videocom.UpdateData();
	m_miccom.SetCurSel(0);
	m_videocom.SetCurSel(0);
	m_speaker.UpdateData();
	m_speaker.SetCurSel(0);


	m_micvol.SetRange(0, 255);//设置范围
	m_micvol.SetTicFreq(5);//设置显示刻度的间隔
	m_micvol.SetPos(0);//当前停留的位置
	m_micvol.SetLineSize(5);//一行的大小，对应键盘的方向键
	m_speakervol.SetRange(0, 255);//设置范围
	m_speakervol.SetTicFreq(5);//设置显示刻度的间隔
	m_speakervol.SetPos(0);//当前停留的位置
	m_speakervol.SetLineSize(5);//一行的大小，对应键盘的方向键

	int micvol = 0;
	int ret = m_mediadevice->getRecordingDeviceVolume(&micvol);
	m_micvol.SetPos(micvol);

	int playvol = 0;
	ret = m_mediadevice->getPlaybackDeviceVolume(&playvol);
	m_speakervol.SetPos(playvol);

	CRect comboxrect;
	GetDlgItem(IDC_COMBO1)->GetWindowRect(&comboxrect);
	ScreenToClient(comboxrect);
	m_videownd = new CVideoWnd(this);
	m_videownd->SetType(UCLOUD_RTC_MEDIATYPE_VIDEO);
	if (!m_videownd->Create(IDD_VIDEO_WND, this)) {
		MessageBox(L"Create Failed");
		return FALSE;
	}

	m_videownd->MoveWindow(comboxrect.left, comboxrect.bottom+10, 298, 224, true);
	m_videownd->TestEnble();
	m_videownd->ShowWindow(SW_SHOW);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void  CDeviceTestDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar)
	{
		if (pScrollBar->GetDlgCtrlID() == IDC_SLIDER_SPEAKER_VOL)
		{
			int res = m_speakervol.GetPos();
			m_mediadevice->setPlaybackDeviceVolume(res);
		}
		else if (pScrollBar->GetDlgCtrlID() == IDC_SLIDER_MIC_VOL)
		{
			int res = m_micvol.GetPos();
			m_mediadevice->setRecordingDeviceVolume(res);
		}
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}


BEGIN_MESSAGE_MAP(CDeviceTestDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_MESSAGE(10001, &CDeviceTestDlg::OnAudioVol)
	ON_BN_CLICKED(IDC_BUTTON_OK, &CDeviceTestDlg::OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CAM_TEST, &CDeviceTestDlg::OnBnClickedButtonCamTest)
	ON_BN_CLICKED(IDC_BUTTON_MIC_TEST, &CDeviceTestDlg::OnBnClickedButtonMicTest)
	ON_BN_CLICKED(IDC_BUTTON_SPEAKER_TEST, &CDeviceTestDlg::OnBnClickedButtonSpeakerTest)
END_MESSAGE_MAP()


void CDeviceTestDlg::OnPaint()
{
	CDialogEx::OnPaint();
}

// CDeviceTestDlg message handlers
void CDeviceTestDlg::OnBnClickedButtonOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
	m_mediadevice->setRecordingDeviceVolume(200);
	m_mediadevice->setPlaybackDeviceVolume(200);
	m_mediadevice->stopCamTest();
	m_mediadevice->stopRecordingDeviceTest();
	m_mediadevice->stopPlaybackDeviceTest();
	
	if (m_videolist.size()>0)
	{
		int selectv = m_videocom.GetCurSel();
		tUCloudRtcDeviceInfo& infov = m_videolist[selectv];
		m_mediadevice->setVideoDevice(&infov);
	}
	
	if (m_miclist.size()>0)
	{
		int selecta = m_miccom.GetCurSel();
		tUCloudRtcDeviceInfo& infoa = m_miclist[selecta];
		m_mediadevice->setRecordDevice(&infoa);
	}

	if (m_spkeakerlist.size()>0)
	{
		int selects = m_speaker.GetCurSel();
		tUCloudRtcDeviceInfo& infos = m_spkeakerlist[selects];
		m_mediadevice->setPlayoutDevice(&infos);
	}
	m_mediadevice->UnInitAudioMoudle();
	m_mediadevice->UnInitVideoMoudle();
	m_mediadevice->destory();
	m_mediadevice = nullptr;
}


void CDeviceTestDlg::OnBnClickedButtonCamTest()
{
	// TODO: Add your control notification handler code here
	int select = m_videocom.GetCurSel();
	tUCloudRtcDeviceInfo& info = m_videolist[select];
	
	if (m_videotest)
	{
		int ret = m_mediadevice->stopCamTest();
		SetDlgItemText(IDC_BUTTON_CAM_TEST, L"开始测试");
	}
	else 
	{
		int ret = m_mediadevice->startCamTest(info.mDeviceId, UCLOUD_RTC_VIDEO_PROFILE_640_360,
			m_videownd->GetVideoHwnd());
		SetDlgItemText(IDC_BUTTON_CAM_TEST, L"停止测试");
	}
	m_videotest = !m_videotest;

}


void CDeviceTestDlg::OnBnClickedButtonMicTest()
{
	// TODO: Add your control notification handler code here
	int select = m_miccom.GetCurSel();
	tUCloudRtcDeviceInfo& info = m_miclist[select];

	if (m_mictest)
	{
		int ret = m_mediadevice->stopRecordingDeviceTest();
		m_micenypro.SetPos(0);
		SetDlgItemText(IDC_BUTTON_MIC_TEST, L"开始测试");
	}
	else {

		int ret = m_mediadevice->startRecordingDeviceTest(m_mediacallback);
		SetDlgItemText(IDC_BUTTON_MIC_TEST, L"停止测试");
	}
	m_mictest = !m_mictest;
}

LRESULT CDeviceTestDlg::OnAudioVol(WPARAM data, LPARAM lp) {
	m_micenypro.SetPos(data);
	return 0;
}


void CDeviceTestDlg::OnBnClickedButtonSpeakerTest()

{
	int select = m_speaker.GetCurSel();
	tUCloudRtcDeviceInfo& info = m_spkeakerlist[select];

	if (m_speakertest)
	{
		int ret = m_mediadevice->stopPlaybackDeviceTest();
		SetDlgItemText(IDC_BUTTON_SPEAKER_TEST, L"开始测试");
	}
	else {
		int ret = m_mediadevice->startPlaybackDeviceTest( "d:/test.WAV");
		SetDlgItemText(IDC_BUTTON_SPEAKER_TEST, L"停止测试");
	}
	m_speakertest = !m_speakertest;
}

