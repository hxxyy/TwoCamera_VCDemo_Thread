// DemoDlg.h : header file
//

#if !defined(AFX_DEMODLG_H__B796397E_11ED_425C_88F3_6BF421831E76__INCLUDED_)
#define AFX_DEMODLG_H__B796397E_11ED_425C_88F3_6BF421831E76__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "DemoDlg.h"
#define WM_PROCESS_MESSAGE WM_USER+101 
#define TIMER0 0
#define TIMER1 1

/////////////////////////////////////////////////////////////////////////////
// CDemoDlg dialog

class CDemoDlg : public CDialog
{
// Construction
public:
	CDemoDlg(CWnd* pParent = NULL);	// standard constructor
    void updateParam();
// Dialog Data
	//{{AFX_DATA(CDemoDlg)
	enum { IDD = IDD_DEMO_DIALOG };
	CSliderCtrl	m_GammaCtrl;
	CSliderCtrl	m_ContrastCtrl;
	CSliderCtrl	m_ExposureCtrl;
	CSliderCtrl	m_GainCtrl;
	CComboBox *m_NameCtrl0;
	CComboBox *m_NameCtrl1;
	CComboBox *m_ResoCtrl0;
	CComboBox *m_ResoCtrl1;
	long	m_RowStart;
	long	m_ROIHeight;
	long	m_ColumnStart;
	long	m_ROIWidth;
	int		m_Gain;
	int		m_Exposure;
	int		m_Contrast;
	int		m_Gamma;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDemoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	int m_device_id;
	double m_fps0;
	double m_fps1;
	int m_index0;
	int m_index1;
	int m_totalFrame0;
	int m_totalFrame1;
	int num_image;

	// Generated message map functions
	//{{AFX_MSG(CDemoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg LRESULT ProcessImage(WPARAM wParam, LPARAM lParam);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelchangeCamera0();
	afx_msg void OnSelchangeCamera1();
	afx_msg void OnPlay();
	afx_msg void OnStop();
	afx_msg void OnFlipx();
	afx_msg void OnFlipy();
	afx_msg void OnROIApply();
	afx_msg void OnSaveparameter();
	afx_msg void OnGroup0();
	afx_msg void OnGroup1();
	afx_msg void OnAutogain();
	afx_msg void OnAutoexposure();
	afx_msg void OnCustomdrawExposure(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdrawGain(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdrawGamma(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdrawContrast(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRadiocamera0();
	afx_msg void OnRadiocamera1();
	afx_msg void OnOnepushwb();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnCbnSelchangeResolution0();
	afx_msg void OnCbnSelchangeResolution1();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEMODLG_H__B796397E_11ED_425C_88F3_6BF421831E76__INCLUDED_)
