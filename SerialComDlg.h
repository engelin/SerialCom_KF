// SerialComDlg.h : header file
//

#if !defined(AFX_SERIALCOMDLG_H__4FB3E547_C3BA_4697_81B7_AB1991BEA61A__INCLUDED_)
#define AFX_SERIALCOMDLG_H__4FB3E547_C3BA_4697_81B7_AB1991BEA61A__INCLUDED_
#include "CommThread.h"	// Added by ClassView
#include "afxwin.h"

//#include "kalman_yr.h"
#include "kalman_config.h"

#define WINDOW_LENGTH 8
#define PI 3.14159265
#define D_ANCHOR 300/2 //30cm...
#define RAWDIST_WINDOW_LENGTH 8

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CSerialComDlg dialog

class CSerialComDlg :  public CDialog
{
// Construction
public:
	CSerialComDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSerialComDlg)
	enum { IDD = IDD_SERIALCOM_DIALOG };
	CEdit	m_EditReceive;
	CEdit	m_EditSend;
	CEdit	m_EditControl;
	CComboBox	m_cStopBit;
	CComboBox	m_cSerialPort;
	CComboBox	m_cSendDataType;
	CComboBox	m_cReceiveDataType;
	CComboBox	m_cParity;
	CComboBox	m_cDataBit;
	CComboBox	m_cBaudRate;
	int		m_iSendDataType;
	int		m_iReceiveDataType;
	int		m_iStopBit;
	int		m_iSerialPort;
	int		m_iParity;
	int		m_iDataBit;
	int		m_iBaudRate;
	CString	m_strControl;
	CString	m_strSend;
	CString	m_strReceive;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSerialComDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSerialComDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnQuit();
	afx_msg long OnCommunication(WPARAM wParam, LPARAM lParam);// 추가 시킨 내용
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnPortOpen();
	afx_msg void OnSendData();
	afx_msg void OnReceiveDataClear();
	afx_msg void OnSendDataClear();
	afx_msg void OnPortClose();
	afx_msg void OnControlboxclear();
	afx_msg void OnSelchangeBaudRate();
	afx_msg void OnSelchangeSerialPort();
	afx_msg void OnSelchangeDataBit();
	afx_msg void OnSelchangeStopBit();
	afx_msg void OnSelchangeParity();
	afx_msg void OnSelchangeSendDataType();
	afx_msg void OnSelchangeReceiveDataType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	
	CCommThread m_ComuPort;
private:
	int m_iSize; //설정창에 시리얼로 들어온 데이터 갯수를 보여주기 위해  갯수를 받을 변수 
	int m_iCount;// 받은 데이터를 화면에 보여줄때 정렬 하기 위해 사용 한줄에 8묶음 2개 (16개 데이터)
	int nMakeHexData(); //보낼 데이터 타입이 hex일 경우 hex데이터로 만들어서 전송
	int nMakeAsciiData();//보낼 데이터 타입이 ascii일 경우 ascii데이터로 만들어서 전송
	BYTE byCode2AsciiValue(char cData);// 문자를 hex값으로 변경 0~F 까지는 그대로 그 외에 글자는 0으로 
	CString byIndexComPort(int xPort);// 포트이름 받기 
	DWORD byIndexBaud(int xBaud);// 전송률 받기
	BYTE byIndexData(int xData);//데이터 비트 받기
	BYTE byIndexStop(int xStop);// 스톱비트 받기 
	BYTE byIndexParity(int xParity);// 펠리티 받기
	int result_raw_dist[2];
	double calc_dist_result;
	double calc_theta_result;
	int direction;
	bool kalman_flag;
	double temp_tag_x;
	double temp_tag_y;
	double maf_x;
	double maf_y;
	double kf_x;
	double kf_y;
	int window_idx;
	double window_distance[WINDOW_LENGTH];
	double window_theta[WINDOW_LENGTH];
	double fin_distance;
	double fin_theta;
	int min_idx;
	int max_idx;
	int dist_win_idx;
	int result_avr_dist[2];
	double raw_distbuf1[WINDOW_LENGTH]; // RAW Distance Buffer
	double raw_distbuf2[WINDOW_LENGTH];
public:
	double dist1;
	double dist2;
	double dist3;
	double theta;
	CEdit m_edit_dist1;
	CEdit m_edit_dist2;
	CEdit m_edit_dist3;
	CEdit m_edit_theta;
	CStatic m_relLange;
	kf_t kf;
	void invMat(double *a);
	void MulMat(double *a, double *b, double *c, int arows, int acols, int bcols);
	void MulVec(double *a, double *x, double *y, int m, int n);
	void TransMat(double *a, double *at, int m, int n);
	void AccumMat(double *a, double *b, int m, int n);
	void AddVec(double *a, double *b, double *c, int n);
	void AddMat(double *a, double *b, double *c, int n);
	void SubMat(double *a, double *b, double *c, int n);
	void NegMat(double *a, int m, int n);
	void MatAddEye(double *a, int n);
	void MulConst(double *a, double c, int n, int m);
	void MeasSet(double x, double y);
	void KalmanInit();
	void KalmanRun();
	bool CalculateKF();
	bool CalculateMAF();
	bool SlidingWindow();
	bool AveragingResult();
	bool DistanceWindow(double Distance1, double Distance2);
	bool AveragingDistance(double * raw_distbuf1, double * raw_distbuf2);
	afx_msg void OnCbnSelchangeFilterType();
	CComboBox m_cFilterType;
	int m_iFilterType;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERIALCOMDLG_H__4FB3E547_C3BA_4697_81B7_AB1991BEA61A__INCLUDED_)
