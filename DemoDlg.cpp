// DemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Demo.h"
#include "DemoDlg.h"
#include "JHCap.h"
#include <process.h>
#include <time.h>
#include <string>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#ifdef WIN64
#pragma comment(lib,"../../../SDK64/JHCap2.lib")
#else
#pragma comment(lib,"JHCap2.lib")
#endif

int work0 = 0;
int work1 = 0;

struct PicInformation{
	int device_id;
	long width;
	long height;
    unsigned char *image;		
};

UINT MyThreadProc( LPVOID pParam )
{
	int id = (int)pParam;
	
	while(1)
	{
		if((id==0 && work0==0) || (id==1 && work1==0)) break;
		int m_width=0, m_height=0, len=0;
		CameraGetImageSize(id,&m_width, &m_height);
		CameraGetImageBufferSize(id,&len, CAMERA_IMAGE_BMP);	
		unsigned char *m_inBuf = new unsigned char[len];

		TRACE("mem %x\n", m_inBuf);
		
		if(CameraQueryImage(id,m_inBuf, &len, CAMERA_IMAGE_BMP)==API_OK)			
		{
			PicInformation info;
			info.device_id=id;
			info.image=m_inBuf;
			info.width=m_width;
			info.height=m_height;
			CDemoDlg *hwnd=(CDemoDlg * )AfxGetMainWnd();  
			hwnd->SendMessage(WM_PROCESS_MESSAGE,0,(LPARAM)&info);
		}
		else
		{
			delete []m_inBuf;
			Sleep(10);
		}
    }
	TRACE("Thread for camera %d end\n", id);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


///////////////////////
/////////////////////////////////////////////////////////////////////////////
// CDemoDlg dialog

CDemoDlg::CDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDemoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDemoDlg)
	m_RowStart = 0;
	m_ROIHeight = 0;
	m_ColumnStart = 0;
	m_ROIWidth = 0;
	m_Gain = 0;
	m_Exposure = 0;
	m_Contrast = 0;
	m_Gamma = 0;
	m_fps0=0;
	m_fps1=0;
	m_index0=0;
	m_index1=1;
	m_totalFrame0=0;
	m_totalFrame1=0;
	num_image = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDemoDlg)
	DDX_Control(pDX, IDC_GAMMA, m_GammaCtrl);
	DDX_Control(pDX, IDC_CONTRAST, m_ContrastCtrl);
	DDX_Control(pDX, IDC_EXPOSURE, m_ExposureCtrl);
	DDX_Control(pDX, IDC_GAIN, m_GainCtrl);
	DDX_Text(pDX, IDC_ROIOFFSETX, m_RowStart);
	DDV_MinMaxLong(pDX, m_RowStart, 0, 5000);
	DDX_Text(pDX, IDC_ROIHEIGHT, m_ROIHeight);
	DDV_MinMaxLong(pDX, m_ROIHeight, 0, 5000);
	DDX_Text(pDX, IDC_ROIOFFSETY, m_ColumnStart);
	DDV_MinMaxLong(pDX, m_ColumnStart, 0, 5000);
	DDX_Text(pDX, IDC_ROIWIDTH, m_ROIWidth);
	DDV_MinMaxLong(pDX, m_ROIWidth, 0, 5000);
	DDX_Slider(pDX, IDC_GAIN, m_Gain);
	DDX_Slider(pDX, IDC_EXPOSURE, m_Exposure);
	DDX_Slider(pDX, IDC_CONTRAST, m_Contrast);
	DDX_Slider(pDX, IDC_GAMMA, m_Gamma);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDemoDlg, CDialog)
	//{{AFX_MSG_MAP(CDemoDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_MESSAGE(WM_PROCESS_MESSAGE,ProcessImage)
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_CAMERA0, OnSelchangeCamera0)
	ON_CBN_SELCHANGE(IDC_CAMERA1, OnSelchangeCamera1)
	ON_BN_CLICKED(IDC_PLAY, OnPlay)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	ON_BN_CLICKED(IDC_FLIPX, OnFlipx)
	ON_BN_CLICKED(IDC_FLIPY, OnFlipy)
	ON_BN_CLICKED(IDC_ROIAPPLY, OnROIApply)
	ON_BN_CLICKED(IDC_SAVEPARAMETER, OnSaveparameter)
	ON_BN_CLICKED(IDC_GROUP0, OnGroup0)
	ON_BN_CLICKED(IDC_GROUP1, OnGroup1)
	ON_BN_CLICKED(IDC_AUTOGAIN, OnAutogain)
	ON_BN_CLICKED(IDC_AUTOEXPOSURE, OnAutoexposure)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_EXPOSURE, OnCustomdrawExposure)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_GAIN, OnCustomdrawGain)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_GAMMA, OnCustomdrawGamma)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_CONTRAST, OnCustomdrawContrast)
	ON_BN_CLICKED(IDC_RADIOCAMERA0, OnRadiocamera0)
	ON_BN_CLICKED(IDC_RADIOCAMERA1, OnRadiocamera1)
	ON_BN_CLICKED(IDC_ONEPUSHWB, OnOnepushwb)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_RESOLUTION0, OnCbnSelchangeResolution0)
	ON_CBN_SELCHANGE(IDC_RESOLUTION1, OnCbnSelchangeResolution1)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDemoDlg message handlers

BOOL CDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	int CamAllNum;
	int width=0,height=0,depth=0, count=0;
	CameraGetCount(&CamAllNum);
	char *name=new char[255];
	char *model=new char[255];
	m_NameCtrl0=(CComboBox*)GetDlgItem(IDC_CAMERA0);
	m_NameCtrl1=(CComboBox*)GetDlgItem(IDC_CAMERA1);
    m_ResoCtrl0=(CComboBox*)GetDlgItem(IDC_RESOLUTION0);
	m_ResoCtrl1=(CComboBox*)GetDlgItem(IDC_RESOLUTION1);
    
	m_GainCtrl.SetRange(0,255,true);
	m_ExposureCtrl.SetRange(0,5000,true);
	m_GammaCtrl.SetRange(0,200,true);
	m_ContrastCtrl.SetRange(0,200,true);

	if(CamAllNum>0)
	{
		for(int i=0; i<CamAllNum; i++)
		{
		    CameraGetName(i, name, model);
		    CString temp;
		    temp.Format("%s #%d",model,i);
		    m_NameCtrl0->InsertString(i,_T(temp));
		    m_NameCtrl1->InsertString(i,_T(temp));

		    CameraInit(i);
			Sleep(10);

			CameraGetResolutionCount(i,&count);

			for(int j=0;j<count;j++)
			{
				CameraGetResolution( i,j,&width, &height);
				CString t_model;
				t_model.Format("%d*%d",width,height);
				if(i==0)
				{
					m_ResoCtrl0->InsertString(j, _T(t_model));
					m_ResoCtrl0->SetCurSel(0); //设置选中的项
				}
				else
				{
					m_ResoCtrl1->InsertString(j, _T(t_model));
					m_ResoCtrl1->SetCurSel(0); //设置选中的项
				}
			}
		}
		m_device_id=0;
		//updateParam();
	
		m_NameCtrl0->SetCurSel(0);
		m_NameCtrl1->SetCurSel(1);
	}
	((CButton*)GetDlgItem(IDC_RADIOCAMERA0))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_GROUP0))->SetCheck(1);
	
	delete[] model;
	delete[] name;
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDemoDlg::OnClose() 
{
	//Click <Stop> before close the application to avoid memory leak
	
	work0 = 0;
	work1 = 0;


	KillTimer(TIMER0);
	KillTimer(TIMER1);

	CameraFree(0);
	CameraFree(1);
	CDialog::OnOK();
}

void CDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDemoDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDemoDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDemoDlg::OnSelchangeCamera0() 
{
	// select camera 0
	int nIndex = m_NameCtrl0->GetCurSel();//取得选中的索引

	////////////////make up resolution/////////////
	m_ResoCtrl0=(CComboBox*)GetDlgItem(IDC_RESOLUTION0);
	m_ResoCtrl0->Clear();   //清除上次选择的分辨率

	m_index0=0;
	int width=0,height=0,depth=0, count=0;
	CameraGetResolutionCount(0,&count);

	for(int j=0;j<count;j++)
	{
		CameraGetResolution( 0,j,&width, &height);
		CString t_model;
		t_model.Format("%d*%d",width,height);
		m_ResoCtrl0->InsertString(j, _T(t_model)); 
	}
	m_ResoCtrl0->SetCurSel(0); //设置选中的项
}

void CDemoDlg::OnSelchangeCamera1() 
{
	// select camera 1
	int nIndex = m_NameCtrl1->GetCurSel();//取得选中的索引

	////////////////make up resolution/////////////
	m_ResoCtrl1=(CComboBox*)GetDlgItem(IDC_RESOLUTION1);
   	m_ResoCtrl0->Clear();   //清除上次选择的分辨率

    m_index1=0;
	int width=0,height=0,depth=0, count=0;
	CameraGetResolutionCount(1,&count);

	for(int j=0;j<count;j++)
	{
		CameraGetResolution( 1,j,&width, &height);
		CString t_model;
		t_model.Format("%d*%d",width,height);
		m_ResoCtrl1->InsertString(j, _T(t_model)); 
	}
	m_ResoCtrl1->SetCurSel(0); //设置选中的项
}

void CDemoDlg::OnPlay()  	//play  the camera
{
	if(((CButton*)GetDlgItem(IDC_RADIOCAMERA0))->GetCheck())
	{
		work0 = 1;
		AfxBeginThread(MyThreadProc, (void*)m_index0);
		//AfxBeginThread(MyThreadProc, (void*)m_index0); //more thread to pull data
		SetTimer(TIMER0,1000,NULL);
	}
	else
	{
		work1 = 1;
		AfxBeginThread(MyThreadProc, (void*)m_index1);
		//AfxBeginThread(MyThreadProc, (void*)m_index1); //more thread to pull data
		SetTimer(TIMER1,1000,NULL);
    }
}

void CDemoDlg::OnStop()    //stop all camera
{

	if(((CButton*)GetDlgItem(IDC_RADIOCAMERA0))->GetCheck())
	{
		KillTimer(TIMER0);
	
		work0=false;
		
		CString t_fps;
		m_fps0=0;
		t_fps.Format("%2.2f",m_fps0);
		GetDlgItem(IDC_Fps0)->SetWindowText(t_fps);
		m_totalFrame0=0;
	}
	else
	{
		KillTimer(TIMER1);
		
		work1 = false;

		CString t_fps;
		m_fps1=0;
		t_fps.Format("%2.2f",m_fps1);
		GetDlgItem(IDC_Fps2)->SetWindowText(t_fps);
		m_totalFrame1=0;
	}
}

void CDemoDlg::OnFlipx()    //set horizontal flip
{
    bool flag=false;	
    CameraGetMirrorX(m_device_id,&flag);
    CameraSetMirrorX(m_device_id,!flag);
}

void CDemoDlg::OnFlipy()    //set vertical flip
{
	bool flag=false;
	CameraGetMirrorY(m_device_id,&flag);
    CameraSetMirrorY(m_device_id,!flag);	
}

void CDemoDlg::OnROIApply()  //set ROI
{
	UpdateData(true);
	long rowOffset=m_RowStart;
	long columnOffset=m_ColumnStart;
	long width=m_ROIWidth;
	long height=m_ROIHeight;

	if((width>=1) &&  (height>=1)&&(rowOffset>=0)&&(columnOffset>=0))
    	CameraSetROI(m_device_id,rowOffset,columnOffset,width,height);
	else
		AfxMessageBox("please enter a valid number!");	
}

void CDemoDlg::OnSaveparameter()     //save image
{
	if (((CButton *)GetDlgItem(IDC_GROUP0))->GetCheck())
	{
		/*CameraSetGain(0, 10);
		CameraSetExposure(0,100);
		Sleep(5000);
		CameraSaveJpegB(0, "res/0.jpg", 1);
		CameraSetGain(0, 10);
		CameraSetExposure(0, 2000);
		Sleep(5000);
		CameraSaveJpegB(0, "res/1.jpg", 1);		
		CameraSetGain(0, 10);
		CameraSetExposure(0, 3000);
		Sleep(5000);*/
		char buffer[65];
		_itoa(num_image, buffer, 10);
		CameraSaveJpegB(0, strcat(buffer,".jpg"), 1);
		num_image++; 
	}
		
	//CameraSaveParameter(m_device_id,0);
	else
		CameraSaveJpegB(1, "xx.jpg", 1);
		//CameraSaveParameter(m_device_id,1);
}

void CDemoDlg::OnGroup0()           //load group 0
{	
	CameraLoadParameter(m_device_id,0);
	updateParam();
}

void CDemoDlg::OnGroup1()           //load group 1
{	
	CameraLoadParameter(m_device_id,1);
	updateParam();
}

void CDemoDlg::OnAutogain()         //set auto gain
{
	if(((CButton*)GetDlgItem(IDC_AUTOGAIN))->GetCheck())
		CameraSetAGC(m_device_id,true);
		 
	else
		CameraSetAGC(m_device_id,false);
	updateParam();
}

void CDemoDlg::OnAutoexposure()      //set auto exposure
{
	if(((CButton*)GetDlgItem(IDC_AUTOEXPOSURE))->GetCheck())
		CameraSetAEC(m_device_id,true);	
	else
		CameraSetAEC(m_device_id,false);
	updateParam();
}

void CDemoDlg::OnCustomdrawExposure(NMHDR* pNMHDR, LRESULT* pResult) //change exposure
{
	UpdateData(true);
	long ExposureValue = m_Exposure;
	CameraSetExposure(m_device_id,ExposureValue);
	CString t_exposure;
	t_exposure.Format("%d",ExposureValue);
	GetDlgItem(IDC_EXPOSUREVALUE)->SetWindowText(t_exposure);
	*pResult = 0;
}

void CDemoDlg::OnCustomdrawGain(NMHDR* pNMHDR, LRESULT* pResult)    //change gain
{
	UpdateData(true);
	int GainValue=m_Gain;
	CameraSetGain(m_device_id,GainValue);
	CString t_gain;
	t_gain.Format("%d",GainValue);
	GetDlgItem(IDC_GAINVALUE)->SetWindowText(t_gain);
	*pResult = 0;
}

void CDemoDlg::OnCustomdrawGamma(NMHDR* pNMHDR, LRESULT* pResult)   //change gamma
{
	UpdateData(true);
	double GammaValue = m_Gamma/100.0 ;
	CameraSetGamma(m_device_id,GammaValue);	
	CString t_gamma;
	t_gamma.Format("%2.2f",GammaValue);
	GetDlgItem(IDC_GAMMAVALUE)->SetWindowText(t_gamma);
	*pResult = 0;
}

void CDemoDlg::OnCustomdrawContrast(NMHDR* pNMHDR, LRESULT* pResult)  //change contrast
{
	UpdateData(true);	
	double ContrastValue=m_Contrast/100.0; 	
	CameraSetContrast(m_device_id,ContrastValue);
	CString t_contrast;
	t_contrast.Format("%2.2f",ContrastValue);
	GetDlgItem(IDC_CONTRASTVALUE)->SetWindowText(t_contrast);
	*pResult = 0;
}

LRESULT CDemoDlg::ProcessImage(WPARAM wParam, LPARAM lParam)    //show image
{
	PicInformation *info=(PicInformation *)lParam; 

	if(info->device_id==0)  //计数器1
		m_totalFrame0++;
	else
		m_totalFrame1++;

	BITMAPINFO bitInfo;
	BITMAPINFOHEADER * header=&bitInfo.bmiHeader;
  	header->biSize=40; 					    	// header size
	header->biWidth=info->width;
	header->biHeight=info->height;
	header->biPlanes=1;
	header->biBitCount=24;				    	// RGB encoded, 24 bit
	header->biCompression=BI_RGB;		    	// no compression
	header->biSizeImage=0;
	header->biXPelsPerMeter=0;
	header->biYPelsPerMeter=0;
	header->biClrUsed=0;
	header->biClrImportant=0;

 	CDC *pDC=NULL;
	CRect rect;	
	
	CWnd *pWnd=NULL;
	if(info->device_id>0)
		pWnd=GetDlgItem(IDC_IMAGE1);
	else
		pWnd=GetDlgItem(IDC_IMAGE0);
	if(pWnd)
	{
		pWnd->GetClientRect(&rect);
		pDC = pWnd->GetDC();	
     
		if(pDC)
		{
			//::SetDIBitsToDevice(pDC->m_hDC,rect.left, rect.top, rect.right, rect.bottom, 0, 0, 0, info->height, info->m_image, &bitInfo, DIB_RGB_COLORS);
			
			SetStretchBltMode(pDC->m_hDC,COLORONCOLOR);                 //防止自适应窗口图像显示失真
			StretchDIBits(pDC->m_hDC,rect.left, rect.top, rect.right, rect.bottom, 0, 0, 
						  info->width, info->height, info->image, &bitInfo, BI_RGB, SRCCOPY);
			ReleaseDC(pDC);
		}
	}

	delete []info->image;
	info->image=NULL;	
	return true;
}

void CDemoDlg::updateParam()   //update parameter
{	
	bool agc,aec;
	CameraGetAGC(m_device_id,&agc);
	CameraGetAEC(m_device_id,&aec);

	if(!agc)
	{
		int gain;
		CameraGetGain(m_device_id,&gain);
		m_Gain=gain;
		CString t_gain;
		t_gain.Format("%d",gain);
		GetDlgItem(IDC_GAINVALUE)->SetWindowText(t_gain);
		CButton* pBtn = (CButton*)GetDlgItem(IDC_AUTOGAIN);
		pBtn->SetCheck(0);
	    GetDlgItem(IDC_GAIN)->EnableWindow(true);
		GetDlgItem(IDC_GAINVALUE)->EnableWindow(true);
	}
	else
	{
		CButton* pBtn = (CButton*)GetDlgItem(IDC_AUTOGAIN);
		pBtn->SetCheck(1);
		GetDlgItem(IDC_GAIN)->EnableWindow(false);
		GetDlgItem(IDC_GAINVALUE)->EnableWindow(false);
	}
	     
	int exposure;
	CameraGetExposure(m_device_id,&exposure);
	if(!aec)
	{
		m_Exposure=exposure;
		CString t_exposure;
		t_exposure.Format("%d",exposure);
		GetDlgItem(IDC_EXPOSUREVALUE)->SetWindowText(t_exposure);
		CButton* pBtn = (CButton*)GetDlgItem(IDC_AUTOEXPOSURE);
		pBtn->SetCheck(0);
		GetDlgItem(IDC_EXPOSURE)->EnableWindow(true);
		GetDlgItem(IDC_EXPOSUREVALUE)->EnableWindow(true);
	}
	else
	{
		CButton* pBtn = (CButton*)GetDlgItem(IDC_AUTOEXPOSURE);
		pBtn->SetCheck(1);
		GetDlgItem(IDC_EXPOSURE)->EnableWindow(false);
		GetDlgItem(IDC_EXPOSUREVALUE)->EnableWindow(false);;
	}
	   
	double gamma;
	CameraGetGamma(m_device_id,&gamma);
	m_Gamma=(int)(gamma*100);
	CString t_gamma;
	t_gamma.Format("%2.2f",gamma);
	GetDlgItem(IDC_GAMMAVALUE)->SetWindowText(t_gamma);
	 
	   
	double contrast;
	CameraGetContrast(m_device_id,&contrast);
	m_Contrast=(int)(contrast*100);
	CString t_contrast;
	t_contrast.Format("%2.2f",contrast);
	GetDlgItem(IDC_CONTRASTVALUE)->SetWindowText(t_contrast);
	UpdateData(false);
}

void CDemoDlg::OnRadiocamera0()  //camera 0
{
	m_device_id=0;
	updateParam();	
}

void CDemoDlg::OnRadiocamera1()  //camera 1
{
	m_device_id=1;
	updateParam();
}

void CDemoDlg::OnOnepushwb()   //One push WB
{
	CameraOnePushWB(m_device_id);	
}



void CDemoDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent)
	{
		case TIMER0:
			{
				if(m_fps0==0)
					m_fps0=m_totalFrame0;
				else
					m_fps0 = 0.4*m_fps0 + 0.6*m_totalFrame0;
				CString t_fps;
				t_fps.Format("%2.2f",m_fps0);
				GetDlgItem(IDC_Fps0)->SetWindowText(t_fps);
				m_totalFrame0=0;
			}break;
		case TIMER1:
			{
				if(m_fps1==0)
					m_fps1=m_totalFrame1;
				else
					m_fps1 = 0.4*m_fps1 + 0.6*m_totalFrame1;
				CString t_fps;
				t_fps.Format("%2.2f",m_fps1);
				GetDlgItem(IDC_Fps2)->SetWindowText(t_fps);
				m_totalFrame1=0;
			}
	}
	CDialog::OnTimer(nIDEvent);
}


void CDemoDlg::OnCbnSelchangeResolution0()
{
	// change resolution
	int width=0,height=0,depth=0;
	int nIndex = m_ResoCtrl0->GetCurSel();//取得选中的索引
	CameraSetResolution(m_index0,nIndex,&width,&height);
}


void CDemoDlg::OnCbnSelchangeResolution1()
{
	// change resolution
	int width=0,height=0,depth=0;
	int nIndex = m_ResoCtrl1->GetCurSel();//取得选中的索引
	CameraSetResolution(m_index1,nIndex,&width,&height);
}
