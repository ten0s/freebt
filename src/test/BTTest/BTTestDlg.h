// Copyright (c) 2004, Antony C. Roberts

// Use of this file is subject to the terms
// described in the LICENSE.TXT file that
// accompanies this file.
//
// Your use of this file indicates your
// acceptance of the terms described in
// LICENSE.TXT.
//
// http://www.freebt.net

#if !defined(AFX_BTTESTDLG_H__B19F39D0_F34F_484E_AC77_D46583C501C6__INCLUDED_)
#define AFX_BTTESTDLG_H__B19F39D0_F34F_484E_AC77_D46583C501C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "fbtHciRoundTrip.h"

/////////////////////////////////////////////////////////////////////////////
// CBTTestDlg dialog

class CBTTestDlg : public CDialog, CHciRoundTrip
{
// Construction
public:
	CBTTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CBTTestDlg)
	enum { IDD = IDD_BTTEST_DIALOG };
	CStatic	m_stLMPVersion;
	CStatic	m_stHCIVersion;
	CStatic	m_stDevice;
	CStatic	m_stAddress;
	CStatic	m_stManufacturer;
	CEdit	m_edtSendText;
	CListBox	m_lbHCI;
	CListBox	m_lbDevices;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBTTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

	DWORD OnInquiryComplete(BYTE Status, BYTE NumResponses);
	DWORD OnInquiryResult(BYTE NumResponses, BYTE BD_ADDR[FBT_HCI_VARIABLE_SIZE][FBT_HCI_BDADDR_SIZE], BYTE PageScanRepetitionMode[FBT_HCI_VARIABLE_SIZE], BYTE PageScanPeriodMode[FBT_HCI_VARIABLE_SIZE], BYTE PageScanMode[FBT_HCI_VARIABLE_SIZE], BYTE ClassOfDevice[FBT_HCI_VARIABLE_SIZE][FBT_HCI_DEVICE_CLASS_SIZE], USHORT ClockOffset[FBT_HCI_VARIABLE_SIZE]);
	DWORD OnConnectionRequest(BYTE BD_ADDR[FBT_HCI_BDADDR_SIZE], ULONG ClassOfDevice[FBT_HCI_DEVICE_CLASS_SIZE], BYTE LinkType);
	DWORD OnConnectionComplete(BYTE nStatus, USHORT nConnectionHandle, BYTE BD_ADDR[FBT_HCI_BDADDR_SIZE], BYTE nLinkType, BYTE nEncryptionMode);

	// Our event hook, for logging HCI events
	DWORD OnEvent(PFBT_HCI_EVENT_HEADER pEvent, DWORD Length);

	// Our command hook, for logging HCI commands
	DWORD SendHciCommand(const PFBT_HCI_CMD_HEADER lpCommand, DWORD dwBufferSize);


// Implementation
protected:
	HICON					m_hIcon;

	BOOL					InitBT(void);
	static DWORD CALLBACK	Reader(LPVOID lpParameter);
	int						FindDeviceByAddress(BYTE BD_ADDR[FBT_HCI_BDADDR_SIZE]);
	int						FindDeviceByConnectionHandle(USHORT nConnectionHandle);
	BOOL					AddressToString(BYTE BD_ADDR[FBT_HCI_BDADDR_SIZE], CString &szAddress);

	// Generated message map functions
	//{{AFX_MSG(CBTTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnInquiry();
	afx_msg void OnGetNames();
	afx_msg void OnConnect();
	afx_msg void OnDisconnect();
	afx_msg void OnSend();
	virtual void OnOK();
	//}}AFX_MSG

	afx_msg LRESULT OnInitialise(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BTTESTDLG_H__B19F39D0_F34F_484E_AC77_D46583C501C6__INCLUDED_)
