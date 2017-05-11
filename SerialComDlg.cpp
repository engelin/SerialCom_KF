// SerialComDlg.cpp : implementation file
//
#include "stdafx.h"
#include "SerialCom.h"
#include "SerialComDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define UWB_MSG_SIZE 65
//#define PI 3.14159265
//#define D_ANCHOR 300/2 //30cm...
#define ROUNDING(x, dig)    ( floor((x) * pow(float(10), dig) + 0.5f) / pow(float(10), dig) )

 HWND hCommWnd;
/////////////////////////////////////////////////////////////////////////////
// CSerialComDlg dialog

CSerialComDlg::CSerialComDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSerialComDlg::IDD, pParent)
	, dist1(0)
	, dist2(0)
	, dist3(0)
	, theta(0)
	, m_iFilterType(0)
{
	//{{AFX_DATA_INIT(CSerialComDlg)
	m_iSendDataType = 0;
	m_iReceiveDataType = 1;
	m_iStopBit = 0;
	m_iSerialPort = 0;
	m_iParity = 0;
	m_iDataBit = 3;
	m_iBaudRate = 7;
	m_iCount =0;
	m_iSize =0;
	m_strControl = _T("");
	m_strSend = _T("");
	m_strReceive = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	kalman_flag = TRUE;
	dist_win_idx = 0;
	min_idx = 0;
	max_idx = 0;
	maf_x = 0;
	maf_y = 0;
	kf_x = 0;
	kf_y = 0;
	calc_dist_result = 0;
	calc_theta_result = 0;

	window_idx = 0;
	memset(window_distance, 0x00, sizeof(window_distance));
	memset(window_theta, 0x00, sizeof(window_theta));

	fin_distance = 0;
	fin_theta = 0;
}

void CSerialComDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSerialComDlg)
	DDX_Control(pDX, IDC_RECEIVE, m_EditReceive);
	DDX_Control(pDX, IDC_SEND, m_EditSend);
	DDX_Control(pDX, IDC_CONTROL, m_EditControl);
	DDX_Control(pDX, IDC_STOP_BIT, m_cStopBit);
	DDX_Control(pDX, IDC_SERIAL_PORT, m_cSerialPort);
	DDX_Control(pDX, IDC_SEND_DATA_TYPE, m_cSendDataType);
	DDX_Control(pDX, IDC_RECEIVE_DATA_TYPE, m_cReceiveDataType);
	DDX_Control(pDX, IDC_PARITY, m_cParity);
	DDX_Control(pDX, IDC_DATA_BIT, m_cDataBit);
	DDX_Control(pDX, IDC_BAUD_RATE, m_cBaudRate);
	DDX_CBIndex(pDX, IDC_SEND_DATA_TYPE, m_iSendDataType);
	DDX_CBIndex(pDX, IDC_RECEIVE_DATA_TYPE, m_iReceiveDataType);
	DDX_CBIndex(pDX, IDC_STOP_BIT, m_iStopBit);
	DDX_CBIndex(pDX, IDC_SERIAL_PORT, m_iSerialPort);
	DDX_CBIndex(pDX, IDC_PARITY, m_iParity);
	DDX_CBIndex(pDX, IDC_DATA_BIT, m_iDataBit);
	DDX_CBIndex(pDX, IDC_BAUD_RATE, m_iBaudRate);
	DDX_Text(pDX, IDC_CONTROL, m_strControl);
	DDX_Text(pDX, IDC_SEND, m_strSend);
	DDX_Text(pDX, IDC_RECEIVE, m_strReceive);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT_dist1, dist1);
	DDX_Text(pDX, IDC_EDIT_dist2, dist2);
	DDX_Text(pDX, IDC_EDIT_dist3, dist3);
	DDX_Text(pDX, IDC_EDIT_theta, theta);
	DDX_Control(pDX, IDC_EDIT_dist1, m_edit_dist1);
	DDX_Control(pDX, IDC_EDIT_dist2, m_edit_dist2);
	DDX_Control(pDX, IDC_EDIT_dist3, m_edit_dist3);
	DDX_Control(pDX, IDC_EDIT_theta, m_edit_theta);
	DDX_Control(pDX, IDC_STATIC_DECA_VIEW, m_relLange);
	DDX_Control(pDX, IDC_FILTER_TYPE, m_cFilterType);
	DDX_CBIndex(pDX, IDC_FILTER_TYPE, m_iFilterType);
}

BEGIN_MESSAGE_MAP(CSerialComDlg, CDialog)
	//{{AFX_MSG_MAP(CSerialComDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_QUIT, OnQuit)
	ON_MESSAGE(WM_COMM_READ , OnCommunication) //추가
	ON_BN_CLICKED(IDC_FILE_OPEN, OnFileOpen)
	ON_BN_CLICKED(IDC_FILE_SAVE, OnFileSave)
	ON_BN_CLICKED(IDC_PORT_OPEN, OnPortOpen)
	ON_BN_CLICKED(IDC_SEND_DATA, OnSendData)
	ON_BN_CLICKED(IDC_RECEIVE_DATA_CLEAR, OnReceiveDataClear)
	ON_BN_CLICKED(IDC_SEND_DATA_CLEAR, OnSendDataClear)
	ON_BN_CLICKED(IDC_PORT_CLOSE, OnPortClose)
	ON_BN_CLICKED(IDC_CONTROLBOXCLEAR, OnControlboxclear)
	ON_CBN_SELCHANGE(IDC_BAUD_RATE, OnSelchangeBaudRate)
	ON_CBN_SELCHANGE(IDC_SERIAL_PORT, OnSelchangeSerialPort)
	ON_CBN_SELCHANGE(IDC_DATA_BIT, OnSelchangeDataBit)
	ON_CBN_SELCHANGE(IDC_STOP_BIT, OnSelchangeStopBit)
	ON_CBN_SELCHANGE(IDC_PARITY, OnSelchangeParity)
	ON_CBN_SELCHANGE(IDC_SEND_DATA_TYPE, OnSelchangeSendDataType)
	ON_CBN_SELCHANGE(IDC_RECEIVE_DATA_TYPE, OnSelchangeReceiveDataType)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_FILTER_TYPE, &CSerialComDlg::OnCbnSelchangeFilterType)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSerialComDlg message handlers

BOOL CSerialComDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	hCommWnd = m_hWnd;
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSerialComDlg::OnPaint() 
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
HCURSOR CSerialComDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void CSerialComDlg::OnQuit() 
{
	// TODO: Add your control notification handler code here
	CDialog::OnOK();//프로그램 종료
}

void CSerialComDlg::OnFileOpen() 
{
	// TODO: Add your control notification handler code here
	//txt파일을 열어서 SEND에디터 창에 데이터 를 넣음
	CString GetFileName;
	char *buf;
	CFile file;
	CFileDialog dlg(TRUE,NULL,NULL,OFN_FILEMUSTEXIST, "Text File(*.txt)|*txt", NULL);
	if(dlg.DoModal() == IDOK)
	{
		file.Open(dlg.GetFileName(),CFile::modeRead, NULL);
		DWORD len = file.GetLength();				
		buf = new char[len];
		memset(buf,0,len);
		file.Read(buf, len);
		SetDlgItemText(IDC_SEND,buf);
		delete [] buf;
		file.Close();
	}
}


void CSerialComDlg::OnFileSave() 
{
	// TODO: Add your control notification handler code here
	//받은 데이터 내용을 txt파일로 저장 
	UpdateData(TRUE);
	CFileException ex;
	CFile file;
	CString name;
	CFileDialog dlg(FALSE,NULL,NULL,OFN_FILEMUSTEXIST, "Text File (*.*)|*", NULL);
	if(dlg.DoModal() == IDOK)
	{
		name = dlg.GetFileName();
		name.MakeLower();
		if(name.Right(4) != "" )
		{
			name += "";
		}
		file.Open(name, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary, &ex);
		file.Write(m_strReceive,m_strReceive.GetLength());
		file.Close();
	}

}

void CSerialComDlg::OnPortOpen() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CString PortName;
	PortName.Format("OPEN PORT: %s\r\n",byIndexComPort(m_iSerialPort));
	if(m_ComuPort.m_bConnected == FALSE)//포트가 닫혀 있을 경우에만 포트를 열기 위해
	{
		if(m_ComuPort.OpenPort(byIndexComPort(m_iSerialPort), byIndexBaud(m_iBaudRate), byIndexData(m_iDataBit), byIndexStop(m_iStopBit), byIndexParity(m_iParity)) ==TRUE)
		{
			m_EditControl.SetSel(-1,0);
			m_EditControl.ReplaceSel(PortName);
		}
	}
	else
	{
		AfxMessageBox("Already Port open");
	}
}


void CSerialComDlg::OnPortClose() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CString PortName;
	if(m_ComuPort.m_bConnected == TRUE)
	{	
		m_ComuPort.ClosePort();
		PortName.Format("CLOSE PORT: %s \r\n",byIndexComPort(m_iSerialPort));
		m_EditControl.SetSel(-1,0);
		m_EditControl.ReplaceSel(PortName);
	}
	else
	{
		PortName.Format("%s Port not yet open \r\n",byIndexComPort(m_iSerialPort));
		m_EditControl.SetSel(-1,0);
		m_EditControl.ReplaceSel(PortName);

	}
}


void CSerialComDlg::OnSelchangeSerialPort() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CString change,str;
	m_iSerialPort = m_cSerialPort.GetCurSel();
	m_cSerialPort.GetLBText(m_iSerialPort,str);
	change.Format("ComPort change: %s \r\n",str);
	m_EditControl.SetSel(-1,0);
	m_EditControl.ReplaceSel(change);
	
}

void CSerialComDlg::OnSelchangeBaudRate() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CString change,str;
	m_iBaudRate = m_cBaudRate.GetCurSel();
	if(m_ComuPort.m_bConnected == TRUE)
	{
		m_ComuPort.ClosePort();
		m_ComuPort.OpenPort(byIndexComPort(m_iSerialPort), byIndexBaud(m_iBaudRate), byIndexData(m_iDataBit), byIndexStop(m_iStopBit), byIndexParity(m_iParity));
	}
	m_cBaudRate.GetLBText(m_iBaudRate,str);
	change.Format("BaudRate change: %s \r\n",str);
	m_EditControl.SetSel(-1,0);
	m_EditControl.ReplaceSel(change);
}

void CSerialComDlg::OnSelchangeDataBit() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CString change,str;
	m_iDataBit = m_cDataBit.GetCurSel();
	if(m_ComuPort.m_bConnected == TRUE)
	{
		m_ComuPort.ClosePort();
		m_ComuPort.OpenPort(byIndexComPort(m_iSerialPort), byIndexBaud(m_iBaudRate), byIndexData(m_iDataBit), byIndexStop(m_iStopBit), byIndexParity(m_iParity));
	}
	m_cDataBit.GetLBText(m_iDataBit,str);
	change.Format("DataBit change: %s \r\n",str);
	m_EditControl.SetSel(-1,0);
	m_EditControl.ReplaceSel(change);
	
	
}

void CSerialComDlg::OnSelchangeStopBit() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CString change, str;
	m_iStopBit= m_cStopBit.GetCurSel();
	if(m_ComuPort.m_bConnected == TRUE)
	{
		m_ComuPort.ClosePort();
		m_ComuPort.OpenPort(byIndexComPort(m_iSerialPort), byIndexBaud(m_iBaudRate), byIndexData(m_iDataBit), byIndexStop(m_iStopBit), byIndexParity(m_iParity));
	}
	m_cStopBit.GetLBText(m_iStopBit,str);
	change.Format("StopBit change: %s \r\n",str);
	m_EditControl.SetSel(-1,0);
	m_EditControl.ReplaceSel(change);
}

void CSerialComDlg::OnSelchangeParity() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CString change,str;
	m_iParity = m_cParity.GetCurSel();
	if(m_ComuPort.m_bConnected == TRUE)
	{
		m_ComuPort.ClosePort();
		m_ComuPort.OpenPort(byIndexComPort(m_iSerialPort), byIndexBaud(m_iBaudRate), byIndexData(m_iDataBit), byIndexStop(m_iStopBit), byIndexParity(m_iParity));
	}
	m_cParity.GetLBText(m_iParity,str);
	change.Format("Parity change: %s \r\n",str);
	m_EditControl.SetSel(-1,0);
	m_EditControl.ReplaceSel(change);
	
}

void CSerialComDlg::OnSelchangeSendDataType() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CString change, data;
	m_cSendDataType.GetLBText(m_iSendDataType,data);
	change.Format("Send Data Type change: %s \r\n",data);
	m_EditControl.SetSel(-1,0);
	m_EditControl.ReplaceSel(change);

	
}

void CSerialComDlg::OnSelchangeReceiveDataType() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CString change,data;
	m_cReceiveDataType.GetLBText(m_iReceiveDataType,data);
	change.Format("Receive Data Type change: %s \r\n",data);
	m_EditControl.SetSel(-1,0);
	m_EditControl.ReplaceSel(change);
	
}

CString CSerialComDlg::byIndexComPort(int xPort)
{
	CString PortName;
	switch(xPort)
	{
		case 0:		PortName = "COM1"; 			break;
			
		case 1:		PortName = "COM2";			break;

		case 2:		PortName = "COM3"; 			break;
		
		case 3:		PortName = "COM4";			break;
			
		case 4:		PortName = "\\\\.\\COM5"; 	break;
			
		case 5:		PortName = "\\\\.\\COM6";	break;
			
		case 6:		PortName = "\\\\.\\COM7"; 	break;
			
		case 7:		PortName = "\\\\.\\COM8";	break;
		
		case 8:		PortName = "\\\\.\\COM9"; 	break;
			
		case 9:		PortName = "\\\\.\\COM10";	break;
	}


	
	return PortName;
}

DWORD CSerialComDlg::byIndexBaud(int xBaud)
{
	DWORD dwBaud;
	switch(xBaud)
	{
		case 0:		dwBaud = CBR_4800;		break;
			
		case 1:		dwBaud = CBR_9600;		break;
	
		case 2:		dwBaud = CBR_14400;		break;
	
		case 3:		dwBaud = CBR_19200;		break;
	
		case 4:		dwBaud = CBR_38400;		break;
	
		case 5:		dwBaud = CBR_56000;		break;
	
		case 6:		dwBaud = CBR_57600;		break;
	
		case 7:		dwBaud = CBR_115200;	break;
	}

	return dwBaud;
}

BYTE CSerialComDlg::byIndexData(int xData)
{
	BYTE byData;
	switch(xData)
	{
		case 0 :	byData = 5;			break;
	
		case 1 :	byData = 6;			break;
		
		case 2 :	byData = 7;			break;
		
		case 3 :	byData = 8;			break;
	}
	return byData;
}

BYTE CSerialComDlg::byIndexStop(int xStop)
{
	BYTE byStop;
	if(xStop == 0)
	{
		byStop = ONESTOPBIT;
	}
	else
	{
		byStop = TWOSTOPBITS;
	}
	return byStop;
}
BYTE CSerialComDlg::byIndexParity(int xParity)
{
	BYTE byParity;
	switch(xParity)
	{
	case 0 :	byParity = NOPARITY;	break;
	
	case 1 :	byParity = ODDPARITY;	break;
	
	case 2 :	byParity = EVENPARITY;	break;
	}

	return byParity;
}

BYTE CSerialComDlg::byCode2AsciiValue(char cData)
{
	//이 함수는 char문자를 hex값으로 변경해 주는 함수 입니다.
	BYTE byAsciiValue;
	if( ( '0' <= cData ) && ( cData <='9' ) )
	{
		byAsciiValue = cData - '0';
	}
	else if( ( 'A' <= cData ) && ( cData <= 'F' ) )
	{
		byAsciiValue = (cData - 'A') + 10;
	}
	else if( ( 'a' <= cData ) && ( cData <= 'f' ) )
	{
		byAsciiValue = (cData - 'a') + 10;
	}
	else
	{
		byAsciiValue = 0;
	}
	return byAsciiValue;
}

void CSerialComDlg::OnSendData() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	int iSize;
	char sSize[128];
	if(m_iSendDataType == 0) //전송할  데이터가 ascii or hex로 구분 
	{
		 iSize = nMakeHexData(); //데이터를  hex로 보낼 경우
	}
	else
	{
		 iSize = nMakeAsciiData();// 데이터를 ascii로 보낼 경우 
	}
	wsprintf(sSize,"Send %d Characters \r\n",iSize);
	m_EditControl.SetSel(-1,0);
	m_EditControl.ReplaceSel(sSize);//설정창에 보낸 데이터 갯수를 보여줌
	
}
int CSerialComDlg::nMakeHexData()
{
	int bufPos = 0;
	int datasize,bufsize, i,j;
	BYTE *Send_buff, byHigh, byLow;
	CString byGetData = m_strSend; //컨트롤 맴버 변수를 받는다.
	byGetData.Replace(" ","");// 공백 없애기 
	byGetData.Replace("\r\n","");//엔터 없애기
	datasize = byGetData.GetLength(); // 공백을 없앤 문자열 길이 얻기 
	
	// 문자 길이를 %2로 나눈 값이 0이 아니면 홀수 이기 때문에 마지막 문자를 처리 해줘야 함
	if(datasize %2 == 0)
	{
		bufsize = datasize;
	}
	else
	{
		bufsize = datasize -1; 
	}

	Send_buff = new BYTE[bufsize];
	
	for( i = 0; i < bufsize ; i+=2)
	{
		byHigh = byCode2AsciiValue(byGetData[i]);
		byLow  = byCode2AsciiValue(byGetData[i+1]);
		Send_buff[bufPos++] = (byHigh <<4) | byLow;
		
	}
	//마지막 문자가 1자리수 일때 처리 하기 위해  예) 1 -> 01
	if(datasize %2 !=0)
	{	
		Send_buff[bufPos++] = byCode2AsciiValue(byGetData[datasize-1]);
	}

	int etc = bufPos % 8;
	//포트에 데이터를 8개씩 쓰기 위해
	//데이터의 길이가 8의 배수가 아니면 나머지 데이터는 따로 보내줌
	for(j =0; j < bufPos-etc; j+= 8)//8의 배수 보냄
	{
		m_ComuPort.WriteComm(&Send_buff[j], 8) ;
	}
	if(etc != 0)//나머지 데이터 전송
	{
		m_ComuPort.WriteComm(&Send_buff[bufPos -etc], etc) ;// 포트에 데이터 쓰기 
	}

	delete [] Send_buff;
	return bufPos;
	
}
int CSerialComDlg::nMakeAsciiData()
{
	int nSize = m_strSend.GetLength();
	int bufPos = 0;
	BYTE *Send_buff;
	Send_buff = new BYTE[nSize];
	for(int i = 0; i < nSize; i++)//ASCII 데이터 이기 때문에 버터에 그대로 저장
	{
		Send_buff[bufPos++] = m_strSend[i];
	}
	int etc = bufPos % 8;
	//포트에 데이터를 8개씩 쓰기 위해
	//데이터의 길이가 8의 배수가 아니면 나머지 데이터는 따로 보내줌
	for(int j =0; j < bufPos-etc; j+= 8)
	{
		m_ComuPort.WriteComm(&Send_buff[j], 8) ;
	}
	if(etc != 0)//나머지 데이터 전송
	{
		m_ComuPort.WriteComm(&Send_buff[bufPos -etc], etc) ;
	}
	delete [] Send_buff;
	return nSize;
	
}
long CSerialComDlg::OnCommunication(WPARAM wParam, LPARAM lParam)
{
	UpdateData(TRUE);//받는 데이터 타입을 알기 위해
	CString str = "";
	CString result;
	char size[128];
	BYTE aByte; //데이터를 저장할 변수 
	int iSize =  (m_ComuPort.m_QueueRead).GetSize(); //포트로 들어온 데이터 갯수
												 //SetCommState에 설정된 내용 때문에 거의 8개씩 들어옴
	m_iSize += iSize;	//받은 데이터의 총 갯수를 설정창에 보여주기 위해 
	//wsprintf(size,"Receive %d characters\r\n",m_iSize);
	wsprintf(size, "Receive DECAWAVE LANGING DATA\r\n");
	if(iSize != 0)//받은 갯수가 0이 아니면 화면에 들어온 갯수를 설정창에  출력
	{
		m_EditControl.SetSel(-1,0);
		m_EditControl.ReplaceSel(size);
	}

	for(int i  = 0 ; i < iSize; i++)//들어온 갯수 만큼 데이터를 읽어 와 화면에 보여줌
	{
		(m_ComuPort.m_QueueRead).GetByte(&aByte);//큐에서 데이터 한개를 읽어옴
		switch(m_iReceiveDataType)//받는 데이터 타입이 ASCII or HEX 인가에 따라 데이터 받는 법을 다르게
		{
		case 0: //데이터가 HEX이면
			str.Format("%02X ", aByte);	
			m_iCount++;//데이터 갯수 세기
			if((m_iCount % 16) == 0) //16개를 받으면 줄 바꿈
			{
				str += _T("\r\n");
			}
			result += str;
			break;
		case 1://데이터가 ASCII이면
			str.Format("%c", aByte);
			result+= str;
			break;
		}
	}

	char line[UWB_MSG_SIZE];
	memcpy(line, result, UWB_MSG_SIZE);

	double d1_result = 0;
	double d2_result = 0;
	double d3_result = 0;
	double theta_result = 0;

	double maf_tag_x = 0;
	double maf_tag_y = 0;

	double kf_tag_x = 0;
	double kf_tag_y = 0;

	int a1_x = 175;
	int a1_y = 50;
	int a2_x = 325;
	int a2_y = 50;

	if (line[1] == 'c') {
		// GET DISTANCE d1, d2
		char distance1[10] = { 0, };
		char distance2[10] = { 0, };
		int d1 = 0;
		int d2 = 0;
		for (int i = 0; i < 9; i++) {
			distance1[i] = line[i + 6];
			distance2[i] = line[i + 15];
		}
		sscanf(distance1, "%x", &d1);
		sscanf(distance2, "%x", &d2);
		result_raw_dist[0] = d1;
		result_raw_dist[1] = d2;

		DistanceWindow(result_raw_dist[0], result_raw_dist[1]);
		AveragingDistance(raw_distbuf1, raw_distbuf2);

		d1 = result_avr_dist[0];
		d2 = result_avr_dist[1];
		d1_result = (double)d1 / (double)1000;
		d2_result = (double)d2 / (double)1000;

		if (CalculateKF() == true) {
			if (SlidingWindow() == true) {
				if (AveragingResult() == true) {

				}
			}
		}

		/*if (m_iFilterType == 1) {
			if (CalculateKF() == true) {

			}
		}
		else if (m_iFilterType == 0) {
			if (CalculateMAF() == true) {
				
			}
		}
		else {
			if (CalculateKF() == true) {

			}
		}*/
	}

	CPaintDC dc(this);
	CDC *picture = m_relLange.GetWindowDC();
	CDC memDC;
	CRect rect;
	CBitmap pOldBmp;
	CBrush red_b(RGB(0xFF, 0x00, 0x00));
	CBrush blue_b(RGB(0x00, 0x00, 0xFF));
	CBrush green_b(RGB(0x00, 0xFF, 0x00));

	GetClientRect(&rect);
	memDC.CreateCompatibleDC(&dc);
	pOldBmp.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	memDC.SelectObject(pOldBmp);

	memDC.PatBlt(0, 0, rect.Width(), rect.Height(), WHITENESS);

	maf_tag_x = maf_x / (double)4 + 250;
	maf_tag_y = maf_y / (double)4 + 50;

	kf_tag_x = kf_x / (double)4 + 250;
	kf_tag_y = kf_y / (double)4 + 50;

	memDC.SelectObject(red_b);
	memDC.MoveTo(a1_x, a1_y);
	memDC.LineTo(a2_x, a2_y);
	memDC.MoveTo(250, 50);
	memDC.LineTo(maf_tag_x, maf_tag_y);
	memDC.MoveTo(250, 50);
	memDC.LineTo(kf_tag_x, kf_tag_y);
	memDC.MoveTo(a1_x, a1_y);
	memDC.LineTo(maf_tag_x, maf_tag_y);
	memDC.MoveTo(a1_x, a1_y);
	memDC.LineTo(kf_tag_x, kf_tag_y);
	memDC.TextOutA(((a1_x + kf_tag_x) / 2) - 5, (a1_y + kf_tag_y) / 2, "D1");
	memDC.Ellipse(a1_x + 6, a1_y + 6, a1_x - 6, a1_y - 6);
	memDC.SelectObject(red_b);
	memDC.MoveTo(a2_x, a2_y);
	memDC.LineTo(maf_tag_x, maf_tag_y);
	memDC.MoveTo(a2_x, a2_y);
	memDC.LineTo(kf_tag_x, kf_tag_y);
	memDC.TextOutA(((a2_x + kf_tag_x) / 2) - 5, (a2_y + kf_tag_y) / 2, "D2");
	memDC.Ellipse(a2_x + 6, a2_y + 6, a2_x - 6, a2_y - 6);

	memDC.SelectObject(green_b);
	memDC.Ellipse(maf_tag_x + 8, maf_tag_y + 8, maf_tag_x - 8, maf_tag_y - 8);
	memDC.TextOutA(maf_tag_x + 9, maf_tag_y, "MAF");

	memDC.SelectObject(blue_b);
	memDC.Ellipse(kf_tag_x + 8, kf_tag_y + 8, kf_tag_x - 8, kf_tag_y - 8);
	memDC.TextOutA(kf_tag_x + 9, kf_tag_y, "KF");

	picture->BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
	memDC.DeleteDC();
	pOldBmp.DeleteObject();

	CDialog::OnPaint();

	UpdateData();

	m_edit_dist1.SetSel(-1, 0);
	dist1 = d1_result;

	m_edit_dist2.SetSel(-1, 0);
	dist2 = d2_result;

	m_edit_dist3.SetSel(-1, 0);
	d3_result = calc_dist_result / (double)1000;
	dist3 = ROUNDING(d3_result, 3);

	m_edit_theta.SetSel(-1, 0);
	theta_result = calc_theta_result;
	theta = ROUNDING(theta_result, 3);

	UpdateData(FALSE);

	m_EditReceive.SetSel(-1,0);
	m_EditReceive.ReplaceSel(result);//화면에 받은 데이터 보여줌

	return 1;
}



void CSerialComDlg::OnReceiveDataClear() 
{
	// TODO: Add your control notification handler code here
	m_iCount =0;
	m_EditReceive.SetSel(0, -1);
	m_EditReceive.Clear();
	m_strReceive.Empty();
}

void CSerialComDlg::OnSendDataClear() 
{
	// TODO: Add your control notification handler code here
	m_EditSend.SetSel(0,-1);
	m_EditSend.Clear();
	m_strSend.Empty();
	m_EditSend.SetFocus();	
}

void CSerialComDlg::OnControlboxclear() 
{
	// TODO: Add your control notification handler code here
	m_iSize =0;
	m_EditControl.SetSel(0,-1);
	m_EditControl.Clear();
	m_strControl.Empty();
}

void CSerialComDlg::invMat(double *a)
{
	int i, j;
	double temp[N / 2][N / 2];
	double det = a[0] * a[3] - a[1] * a[2];

	memcpy(temp, a, sizeof(temp));

	a[0] = temp[1][1] / det;
	a[1] = -(temp[0][1] / det);
	a[2] = -(temp[1][0] / det);
	a[3] = temp[0][0] / det;
}

void CSerialComDlg::MulMat(double *a, double *b, double *c, int arows, int acols, int bcols)
{
	int i, j, l;

	for (i = 0; i < arows; ++i) {
		for (j = 0; j < bcols; ++j) {
			c[i * bcols + j] = 0;
			for (l = 0; l < acols; ++l) {
				c[i * bcols + j] += a[i * acols + l] * b[l * bcols + j];
			}
		}
	}
}

void CSerialComDlg::MulVec(double *a, double *x, double *y, int m, int n)
{
	int i, j;

	for (i = 0; i < m; ++i) {
		y[i] = 0;
		for (j = 0; j < n; ++j)
			y[i] += a[i * n + j] * x[j];
	}
}

void CSerialComDlg::TransMat(double *a, double *at, int m, int n)
{
	int i, j;

	for (i = 0; i < m; ++i) {
		for (j = 0; j < n; ++j) {
			at[j * m + i] = a[i * n + j];
		}
	}
}

void CSerialComDlg::AccumMat(double *a, double *b, int m, int n)
{
	int i, j;

	for (i = 0; i < m; ++i) {
		for (j = 0; j < n; ++j) {
			a[i * n + j] += b[i * n + j];
		}
	}
}

void CSerialComDlg::AddVec(double *a, double *b, double *c, int n)
{
	int i;
	for (i = 0; i < n; ++i) {
		c[i] = a[i] + b[i];
	}
}

void CSerialComDlg::AddMat(double *a, double *b, double *c, int n)
{
	int i, j;

	for (i = 0; i < n; ++i) {
		for (j = 0; j < n; ++j) {
			c[i + j] = a[i + j] + b[i + j];
		}
	}
}

void CSerialComDlg::SubMat(double *a, double *b, double *c, int n)
{
	int i, j;

	for (i = 0; i < n; ++i) {
		for (j = 0; j < n; ++j) {
			c[i + j] = a[i + j] - b[i + j];
		}
	}
}

void CSerialComDlg::NegMat(double *a, int m, int n)
{
	int i, j;

	for (i = 0; i < m; ++i) {
		for (j = 0; j < n; ++j) {
			a[i * n + j] = -a[i * n + j];
		}
	}
}

void CSerialComDlg::MatAddEye(double *a, int n)
{
	int i;
	for (i = 0; i < n; ++i)
		a[i * n + i] += 1;
}

void CSerialComDlg::MulConst(double *a, double c, int n, int m)
{
	int i, j;

	for (i = 0; i < m; ++i) {
		for (j = 0; j < n; ++j) {
			a[i * n + j] *= c;
		}
	}
}

void CSerialComDlg::MeasSet(double x, double y)
{
	kf.z[0] = x;
	kf.z[1] = y;
}

void
CSerialComDlg::KalmanInit()
{
	double tempVX[N] = { 1.0, 1.0, 0, 0 };
	double tempQ[N][N] = { { T4 / 4, 0, T2 / 2, 0 }, { 0, T4 / 4, 0, T3 / 2 }, { T3 / 2, 0, T2, 0 }, { 0, T3, 0, T2 } };
	double tempR[N / 2][N / 2] = { { 1.0, 0 }, { 0, 1.0 } };
	double tempA[N][N] = { { 1.0, 0, T, 0 }, { 0, 1.0, 0, T }, { 0, 0, 1.0, 0 }, { 0, 0, 0, 0 } };
	double tempB[N] = { T2 / 2, T2 / 2, T, T };
	double tempH[N / 2][N] = { { 1.0, 0, 0, 0 }, { 0, 1.0, 0, 0 } };

	double tempMat[N][N] = { 0, };
	double tempVec[N / 2] = { 0, };

	kf.u = 0.005;
	memcpy(kf.x, tempVX, sizeof(tempVX));
	memcpy(kf.xp, tempVX, sizeof(tempVX));

	memcpy(kf.Q, tempQ, sizeof(tempQ));
	memcpy(kf.R, tempR, sizeof(tempR));
	memcpy(kf.P, tempQ, sizeof(tempQ));

	memcpy(kf.A, tempA, sizeof(tempA));
	memcpy(kf.B, tempB, sizeof(tempB));
	memcpy(kf.H, tempH, sizeof(tempH));

	memcpy(kf.z, tempVec, sizeof(kf.z));
	memcpy(kf.Pp, tempMat, sizeof(kf.Pp));
	memcpy(kf.K, tempMat, sizeof(kf.K));
}

void
CSerialComDlg::KalmanRun()
{
	/* predict xp */
	MulVec(kf.A, kf.x, kf.tmpV1, N, N);
	MulConst(kf.B, kf.u, N, 1);
	AddVec(kf.tmpV1, kf.B, kf.xp, N);

	/* predict Pp */
	MulMat(kf.A, kf.P, kf.tmp1, N, N, N);
	TransMat(kf.A, kf.tmp2, N, N);
	MulMat(kf.tmp1, kf.tmp2, kf.tmp3, N, N, N);
	AddMat(kf.tmp3, kf.Q, kf.Pp, N);

	memset(kf.tmp1, 0x00, sizeof(kf.tmp1));
	memset(kf.tmp2, 0x00, sizeof(kf.tmp2));
	memset(kf.tmp3, 0x00, sizeof(kf.tmp3));
	memset(kf.tmpV1, 0x00, sizeof(kf.tmpV1));

	/* calculate kalman gain */
	TransMat(kf.H, kf.HT, N / 2, N);
	MulMat(kf.Pp, kf.HT, kf.tmpR1, N, N / 2, N);
	MulMat(kf.H, kf.Pp, kf.tmpR2, N / 2, N, N);
	MulMat(kf.tmpR2, kf.HT, kf.tmp4, N / 2, N, N / 2);
	AccumMat(kf.tmp4, kf.R, N / 2, N / 2); //H*Pp*HT+R 2x2
	invMat(kf.tmp4);
	MulMat(kf.tmpR1, kf.tmp4, kf.K, N, N / 2, N / 2);

	memset(kf.tmpR1, 0x00, sizeof(kf.tmpR1));
	memset(kf.tmpR2, 0x00, sizeof(kf.tmpR2));
	memset(kf.tmp4, 0x00, sizeof(kf.tmp4));

	/* calculate x */
	MulVec(kf.H, kf.xp, kf.tmpV2, N, N);
	NegMat(kf.tmpV2, N, 1);
	AddVec(kf.z, kf.tmpV2, kf.tmpV3, N);
	MulVec(kf.K, kf.tmpV3, kf.tmpV1, N / 2, N / 2);
	AddVec(kf.xp, kf.tmpV1, kf.x, N);

	memset(kf.tmpV1, 0x00, sizeof(kf.tmpV1));
	memset(kf.tmpV2, 0x00, sizeof(kf.tmpV2));
	memset(kf.tmpV3, 0x00, sizeof(kf.tmpV3));

	/* calculate P */
	MulMat(kf.K, kf.H, kf.tmp1, N, N / 2, N);
	NegMat(kf.tmp1, N, N);
	MatAddEye(kf.tmp1, N);
	MulMat(kf.tmp1, kf.Pp, kf.P, N, N, N);
}

bool
CSerialComDlg::CalculateKF(){
	double long_d = 0;
	double short_d = 0;

	if (result_raw_dist[0] - result_raw_dist[1] > 0) {
		long_d = result_raw_dist[0];
		short_d = result_raw_dist[1];
		direction = 1;
	}
	else if (result_raw_dist[0] - result_raw_dist[1] < 0) {
		long_d = result_raw_dist[1];
		short_d = result_raw_dist[0];
		direction = -1;
	}
	else {
		long_d = result_raw_dist[0];
		short_d = result_raw_dist[1];
		direction = 1;
	}

	double x1_pow = abs(((long_d * long_d - short_d * short_d) * (long_d * long_d - short_d * short_d)) / (16 * D_ANCHOR * D_ANCHOR));
	double y1_pow = abs((long_d * long_d + short_d * short_d - 2 * D_ANCHOR * D_ANCHOR - 2 * x1_pow) / 2);

	maf_x = direction * sqrt(x1_pow);
	maf_y = sqrt(y1_pow);

	temp_tag_x = direction * sqrt(x1_pow);
	temp_tag_y = sqrt(y1_pow);
	
	if (kalman_flag == TRUE) {
		KalmanInit();
		kalman_flag = FALSE;
	}
	MeasSet(temp_tag_x, temp_tag_y);
	KalmanRun();

	kf_x = kf.x[0];
	kf_y = kf.x[1];

	double x_pow = kf_x * kf_x;
	double y_pow = kf_y * kf_y;
	double d3_pow = x_pow + y_pow;
	calc_dist_result = sqrt(d3_pow);

	double radian = abs(PI / 2 - acos(kf_x / calc_dist_result));
	calc_theta_result = direction * abs((radian * 180 / PI));

	switch (direction) {
	case 1:
		return true;
	case -1:
		return true;
	default:
		return false;
	}
}

/* MAF */
bool
CSerialComDlg::CalculateMAF(){
	double long_d = 0;
	double short_d = 0;

	if (result_raw_dist[0] - result_raw_dist[1] > 0) {
		long_d = result_raw_dist[0];
		short_d = result_raw_dist[1];
		direction = 1;
	}
	else if (result_raw_dist[0] - result_raw_dist[1] < 0) {
		long_d = result_raw_dist[1];
		short_d = result_raw_dist[0];
		direction = -1;
	}
	else {
		long_d = result_raw_dist[0];
		short_d = result_raw_dist[1];
		direction = 1;
	}

	double x1_pow = abs(((long_d * long_d - short_d * short_d) * (long_d * long_d - short_d * short_d)) / (16 * D_ANCHOR * D_ANCHOR));
	double y1_pow = abs((long_d * long_d + short_d * short_d - 2 * D_ANCHOR * D_ANCHOR - 2 * x1_pow) / 2);

	double d3_pow = x1_pow + y1_pow;
	//d3_result = sqrt(d3_pow);
	calc_dist_result = sqrt(d3_pow);

	maf_x = direction * sqrt(x1_pow);
	maf_y = sqrt(y1_pow);

	//double radian = abs(PI / 2 - acos(x / calc_dist_result));
	//calc_theta_result = direction * abs((radian * 180 / PI));

	switch (direction) {
	case 1:
		return true;
	case -1:
		return true;
	default:
		return false;
	}
}

bool
CSerialComDlg::SlidingWindow() {
	/* Distance & Theta Calculation Result Window */
	if (window_idx < WINDOW_LENGTH) { /* INIT WINDOW COPY */
		window_distance[window_idx] = calc_dist_result;
		window_theta[window_idx] = calc_theta_result;
		window_idx++;
	}
	else if (window_idx == WINDOW_LENGTH) { /* FULL WINDOW */
		for (int i = 0; i < window_idx - 1; i++) {
			window_distance[i] = window_distance[i + 1];
			window_theta[i] = window_theta[i + 1];
		}
		window_distance[window_idx - 1] = calc_dist_result;
		window_theta[window_idx - 1] = calc_theta_result;
	}
	else {
		MessageBox(_T("Sliding Window Index Error"), NULL, NULL);
		return false;
	}

	return true;
}

bool CSerialComDlg::AveragingResult() {
	double sum_distance = 0;
	double sum_theta = 0;
	double max_value = 0;
	double min_value = 0;
	double max_theta = 0;
	double min_theta = 0;
	int max_idx = 0;
	int min_idx = 0;

	/* MIN, MAX FINDING */
	min_value = max_value = window_distance[0];

	for (int i = 0; i < window_idx; i++) {
		if (window_distance[i] > max_value) { // MAX
			max_idx = i;
			max_value = window_distance[max_idx];
			max_theta = window_theta[max_idx];
		}
		if (window_distance[i] < min_value) { // MIN
			min_idx = i;
			min_value = window_distance[min_idx];
			min_theta = window_theta[min_idx];
		}
	}

	/* AVERAGING without MIN, MAX  */
	for (int i = 0; i < WINDOW_LENGTH; i++) {
		sum_distance += window_distance[i];
		sum_theta += window_theta[i];
	}

	if (window_idx > 2) {
		fin_distance = (sum_distance - max_value - min_value) / (window_idx - 2);
		fin_theta = (sum_theta - max_theta - min_theta) / (window_idx - 2);
	}
	else {
		fin_distance = sum_distance / window_idx;
		fin_theta = sum_theta / window_idx;
	}

	return true;
}

bool
CSerialComDlg::DistanceWindow(double Distance1, double Distance2) {
	/* WINDOW2 */
	if (dist_win_idx < RAWDIST_WINDOW_LENGTH) { /* INIT WINDOW COPY */
		raw_distbuf1[dist_win_idx] = Distance1;
		raw_distbuf2[dist_win_idx] = Distance2;
		dist_win_idx++;
	}
	else if (dist_win_idx == RAWDIST_WINDOW_LENGTH) { /* FULL WINDOW */
		for (int i = 0; i < dist_win_idx - 1; i++) {
			raw_distbuf1[i] = raw_distbuf1[i + 1];
			raw_distbuf2[i] = raw_distbuf2[i + 1];
		}
		raw_distbuf1[dist_win_idx - 1] = Distance1;
		raw_distbuf2[dist_win_idx - 1] = Distance2;
	}
	else {
		MessageBox(_T("Distance Window Index Error"), NULL, NULL);
		return false;
	}

	return true;
}

bool
CSerialComDlg::AveragingDistance(double * distance1_buffer, double * distance2_buffer) {
	double sum_distance1 = 0;
	double sum_distance2 = 0;
	double fin_distance = 0;
	double max_value = 0;
	double min_value = 0;

	/* Buffer 1 */
	min_idx = max_idx = 0;
	min_value = max_value = distance1_buffer[0];
	for (int i = 0; i < dist_win_idx; i++) {
		if (distance1_buffer[i] > max_value) { // MAX
			max_idx = i;
			max_value = distance1_buffer[max_idx];
		}
		if (distance1_buffer[i] < min_value) { // MIN
			min_idx = i;
			min_value = distance1_buffer[min_idx];
		}
	}
	for (int i = 0; i < dist_win_idx; i++) {
		sum_distance1 += distance1_buffer[i];
	}
	if (dist_win_idx > 2)
		result_avr_dist[0] = (sum_distance1 - max_value - min_value) / (dist_win_idx - 2);
	else
		result_avr_dist[0] = (sum_distance1) / (dist_win_idx);

	/* Buffer 2 */
	min_idx = max_idx = 0;
	min_value = max_value = distance2_buffer[0];
	for (int i = 0; i < dist_win_idx; i++) {
		if (distance2_buffer[i] > max_value) { // MAX
			max_idx = i;
			max_value = distance2_buffer[max_idx];
		}
		if (distance2_buffer[i] < min_value) { // MIN
			min_idx = i;
			min_value = distance2_buffer[min_idx];
		}
	}
	for (int i = 0; i < dist_win_idx; i++) {
		sum_distance2 += distance2_buffer[i];
	}
	if (dist_win_idx > 2)
		result_avr_dist[1] = (sum_distance2 - max_value - min_value) / (dist_win_idx - 2);
	else
		result_avr_dist[1] = (sum_distance2) / (dist_win_idx);

	return true;
}

void CSerialComDlg::OnCbnSelchangeFilterType()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	CString change, str;
	m_iFilterType = m_cFilterType.GetCurSel();
	m_cFilterType.GetLBText(m_iFilterType, str);
	change.Format("Filter Type change: %s \r\n", str);
	m_EditControl.SetSel(-1, 0);
	m_EditControl.ReplaceSel(change);
}
