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

#define  _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "fbtutil.h"

#include "BTTest.h"
#include "BTTestDlg.h"
#include "fbtHciRoundTrip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// One of these structs is attached to each list item
typedef struct _BTDevice
{
	BYTE	BD_ADDR[FBT_HCI_BDADDR_SIZE];
	BYTE	PageScanRepetitionMode;
	BYTE	PageScanPeriodMode;
	BYTE	PageScanMode;
	BYTE	ClassOfDevice[FBT_HCI_DEVICE_CLASS_SIZE];
	USHORT	ClockOffset;
	BYTE	Name[FBT_HCI_NAME_SIZE];
	USHORT	ConnectionHandle;

} BTDevice, *PBTDevice;

typedef struct _LogItem
{
	CBTTestDlg	*pThis;
	BOOL		bEvent;
	BYTE		*pBuffer;
	DWORD		dwLength;

} LogItem, *PLogItem;

/////////////////////////////////////////////////////////////////////////////
// CBTTestDlg dialog

CBTTestDlg::CBTTestDlg(CWnd* pParent) : CDialog(CBTTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBTTestDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

}

void CBTTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBTTestDlg)
	DDX_Control(pDX, IDS_LMP_VERSION, m_stLMPVersion);
	DDX_Control(pDX, IDS_HCI_VERSION, m_stHCIVersion);
	DDX_Control(pDX, IDS_DEVICE, m_stDevice);
	DDX_Control(pDX, IDS_ADDRESS, m_stAddress);
	DDX_Control(pDX, IDS_MANUFACTURER, m_stManufacturer);
	DDX_Control(pDX, IDE_SENDTEXT, m_edtSendText);
	DDX_Control(pDX, IDL_HCI, m_lbHCI);
	DDX_Control(pDX, IDL_DEVICES, m_lbDevices);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBTTestDlg, CDialog)
	//{{AFX_MSG_MAP(CBTTestDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDB_INQUIRY, OnInquiry)
	ON_BN_CLICKED(IDB_GET_NAMES, OnGetNames)
	ON_BN_CLICKED(IDB_CONNECT, OnConnect)
	ON_BN_CLICKED(IDB_DISCONNECT, OnDisconnect)
	ON_BN_CLICKED(IDB_SEND, OnSend)
	//}}AFX_MSG_MAP

	ON_MESSAGE(WM_USER+1, OnInitialise)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBTTestDlg message handlers

LRESULT CBTTestDlg::OnInitialise(WPARAM wParam, LPARAM lParam)
{
	// Start listening for HCI events from the HW
	if (StartEventListener()!=ERROR_SUCCESS)
	{
		MessageBox(_T("Failed to start HCI Event Listener!"));
		exit(0);

	}

	// Send a reset command and wait for it to complete
	SendReset();

	// Wait a bit to make sure the device is ready...
	Sleep(1000);

	FBT_HCI_READ_LOCAL_VERSION_INFORMATION_COMPLETE CommandComplete;
	if (FBT_HCI_SUCCESS(SendReadLocalVersionInformation(CommandComplete)))
	{
		fbtLog(fbtLog_Notice, "HCI Version=%d, HCI Revsion=%d, LMP Version=%d, LMP SubVersion=%d, Manufacturer=%s",
			CommandComplete.HCIVersion,
			CommandComplete.HCIRevision,
			CommandComplete.LMPVersion,
			CommandComplete.LMPSubVersion,
			GetManufacturerName(CommandComplete.Manufacturer));

		m_stManufacturer.SetWindowText(GetManufacturerName(CommandComplete.Manufacturer));

		CString szTemp;
		szTemp.Format("%d, %d", CommandComplete.HCIVersion, CommandComplete.HCIRevision);
		m_stHCIVersion.SetWindowText(szTemp);

		szTemp.Format("%d, %d", CommandComplete.LMPVersion, CommandComplete.LMPSubVersion);
		m_stLMPVersion.SetWindowText(szTemp);

	}

	// Make the device discoverable
	SendWriteScanEnable(3);

	// Set default connect timeout
	SendWritePageTimeout(0x2000);

	// Set the the class of device
	BYTE ClassOfDeviceBytes[FBT_HCI_DEVICE_CLASS_SIZE]={0};

	// PC
	ClassOfDeviceBytes[0]=0x10;
	ClassOfDeviceBytes[1]=0x01;
	ClassOfDeviceBytes[2]=0x02;

	fbtLog(fbtLog_Notice, _T("CBTTestDlg::OnInitDialog: Calling SendWriteClassOfDevice"));
	SendWriteClassOfDevice(ClassOfDeviceBytes);

	fbtLog(fbtLog_Notice, _T("CBTTestDlg::OnInitDialog: Calling SendSetEventFilter"));

	// Enable connection auto-accept
	BYTE Condition[FBT_HCI_MAX_CONDITION_SIZE]={0};
	Condition[0]=2;
	SendSetEventFilter(
		FBT_HCI_FILTER_CONNECTION_SETUP,
		FBT_HCI_FILTER_ALL,
		Condition,
		1);

	// Set the Device Name
	TCHAR szDeviceName[80]={0};
	ZeroMemory(szDeviceName, 80);
	DWORD dwLength=80;
	if (GetComputerName(szDeviceName, &dwLength))
	{
		BYTE szLocalName[FBT_HCI_NAME_SIZE]={0};
		strcpy((char*)szLocalName, szDeviceName);
		SendChangeLocalName(szLocalName);

	}

	// Read the local BD_ADDR
	BYTE BD_ADDR[FBT_HCI_BDADDR_SIZE];
	if (ReadBDADDR(BD_ADDR)==ERROR_SUCCESS)
	{
		CString szDevice;
		szDevice.Format(_T("%02X:%02X:%02X:%02X:%02X:%02X"),
							BD_ADDR[5],
							BD_ADDR[4],
							BD_ADDR[3],
							BD_ADDR[2],
							BD_ADDR[1],
							BD_ADDR[0]);

		m_stAddress.SetWindowText(szDevice);

	}

	else
		fbtLog(fbtLog_Notice, _T("CBTTestDlg::OnInitDialog: Failed to read BDADDR"));

	// Start the reader thread
	HANDLE hThread = CreateThread( NULL, 0, Reader, (LPVOID)this, 0, NULL );
   CloseHandle( hThread );

	return TRUE;

}

BOOL CBTTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CFont *font=new CFont;
	font->CreateFont(
			14,                        // nHeight
			0,                         // nWidth
			0,                         // nEscapement
			0,                         // nOrientation
			0,                 // nWeight
			FALSE,                     // bItalic
			FALSE,                     // bUnderline
			FALSE,                         // cStrikeOut
			DEFAULT_CHARSET,              // nCharSet
			OUT_DEFAULT_PRECIS,        // nOutPrecision
			CLIP_DEFAULT_PRECIS,       // nClipPrecision
			DEFAULT_QUALITY,           // nQuality
			FF_DONTCARE,  // nPitchAndFamily
			"Courier New");           // lpszFacename

	m_lbHCI.SetFont(font);

	fbtLogSetFile("bttest.log");
	fbtLogSetLevel(255);

	UINT i=0;
	CString szString;
	while (i<255 && !IsAttached())
	{
		szString.Format("\\\\.\\FbtUsb%02d", i++);
		CHciRoundTrip::Attach(szString);

	}

	if (!IsAttached())
	{
		MessageBox(_T("No Bluetooth hardware detected!"));
		exit(0);

	}

	// Retrieve the HW driver device name
	fbtLog(fbtLog_Notice, _T("CBTTestDlg::OnInitDialog: Connected to device %s"), szString);
	char szDeviceName[80]={0};
	if (GetDeviceName(szDeviceName, 80)>0)
		m_stDevice.SetWindowText(szDeviceName);

	// Let the dialog finish creating itself and displaying the window
	// When its done, it will process its message queue on catch WM_USER+1
	PostMessage(WM_USER+1, 0, 0);

	return TRUE;  // return TRUE  unless you set the focus to a control

}

void CBTTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);

}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CBTTestDlg::OnPaint()
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
HCURSOR CBTTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BOOL CBTTestDlg::InitBT(void)
{
	return true;

}

void CBTTestDlg::OnInquiry()
{
	SetCursor(LoadCursor(NULL, IDC_WAIT));
	m_lbDevices.ResetContent();
	SendInquiry(FBT_HCI_LAP_GIAC, 5, 0);

}

DWORD CBTTestDlg::OnInquiryComplete(BYTE Status, BYTE NumResponses)
{
	SetCursor(LoadCursor(NULL, IDC_ARROW));
	if (Status!=0)
		MessageBox("Inquiry failed");

	else
		MessageBox("Inquiry complete");

	return 0;

}

DWORD CBTTestDlg::OnInquiryResult(BYTE NumResponses, BYTE BD_ADDR[FBT_HCI_VARIABLE_SIZE][FBT_HCI_BDADDR_SIZE], BYTE PageScanRepetitionMode[FBT_HCI_VARIABLE_SIZE], BYTE PageScanPeriodMode[FBT_HCI_VARIABLE_SIZE], BYTE PageScanMode[FBT_HCI_VARIABLE_SIZE], BYTE ClassOfDevice[FBT_HCI_VARIABLE_SIZE][FBT_HCI_DEVICE_CLASS_SIZE], USHORT ClockOffset[FBT_HCI_VARIABLE_SIZE])
{
	CString		szDevice;
	int			nListItem=0;
	PBTDevice	pBTDevice=NULL;

	for (UINT i=0; i<NumResponses; i++)
	{
		szDevice.Format(_T("%02X:%02X:%02X:%02X:%02X:%02X"),
							BD_ADDR[i][5],
							BD_ADDR[i][4],
							BD_ADDR[i][3],
							BD_ADDR[i][2],
							BD_ADDR[i][1],
							BD_ADDR[i][0]);
		if (m_lbDevices.FindString(-1, szDevice)==LB_ERR)
		{
			nListItem=m_lbDevices.AddString(szDevice);
			if (nListItem!=-1)
			{
				pBTDevice=(PBTDevice)malloc(sizeof(BTDevice));
				memset(pBTDevice, 0, sizeof(BTDevice));

				memcpy(pBTDevice->BD_ADDR, BD_ADDR[i], FBT_HCI_BDADDR_SIZE);
				memcpy(pBTDevice->ClassOfDevice, ClassOfDevice[i], FBT_HCI_DEVICE_CLASS_SIZE);

				pBTDevice->ClockOffset=ClockOffset[i];
				pBTDevice->PageScanMode=PageScanMode[i];
				pBTDevice->PageScanPeriodMode=PageScanPeriodMode[i];
				pBTDevice->PageScanRepetitionMode=PageScanRepetitionMode[i];
				m_lbDevices.SetItemData(nListItem, (DWORD)pBTDevice);

			}

		}

	}

	return 0;

}

void CBTTestDlg::OnGetNames()
{
	PBTDevice	pBTDevice=NULL;
	int			nDevice=0;

	for (int i=0; i<m_lbDevices.GetCount(); i++)
	{
		m_lbDevices.SetCurSel(i);
		pBTDevice=NULL;
		pBTDevice=(PBTDevice)m_lbDevices.GetItemData(i);
		if (pBTDevice)
		{
			if (FBT_HCI_SUCCESS(RemoteNameRequest(
									pBTDevice->BD_ADDR,
									pBTDevice->PageScanRepetitionMode,
									pBTDevice->PageScanMode,
									pBTDevice->ClockOffset,
									pBTDevice->Name)))
			{
				m_lbDevices.DeleteString(i);
				nDevice=m_lbDevices.InsertString(i, (LPCSTR)pBTDevice->Name);
				m_lbDevices.SetItemData(nDevice, (DWORD)pBTDevice);

			}

			else
			{
				MessageBox("Failed to get name");

			}

		}

	}

	MessageBox("Name retrieval complete");

}

void CBTTestDlg::OnConnect()
{
	int nDevice=m_lbDevices.GetCurSel();
	if (nDevice==LB_ERR)
	{
		MessageBox("Select a device");
		return;

	}

	PBTDevice pBTDevice=(PBTDevice)m_lbDevices.GetItemData(nDevice);
	if (pBTDevice)
	{
		USHORT ConnectionHandle=0;
		DWORD dwReturn=CreateConnection(
						pBTDevice->BD_ADDR,
						FBT_HCI_PACKET_TYPE_ALL,
						pBTDevice->PageScanRepetitionMode,
						pBTDevice->PageScanMode,
						pBTDevice->ClockOffset,
						TRUE,
						pBTDevice->ConnectionHandle);

		CString szMsg;
		if (FBT_HCI_SUCCESS(dwReturn))
		{
			szMsg.Format("Connected on handle 0x%04x", pBTDevice->ConnectionHandle);
			MessageBox(szMsg);

		}

		else
		{
			szMsg.Format("Connect failed (%s)", GetStatusText((BYTE)dwReturn));
			MessageBox(szMsg);

		}

	}

}

void CBTTestDlg::OnDisconnect()
{
	int nDevice=m_lbDevices.GetCurSel();
	if (nDevice==LB_ERR)
	{
		MessageBox("Select a device");
		return;

	}

	PBTDevice pBTDevice=(PBTDevice)m_lbDevices.GetItemData(nDevice);
	if (pBTDevice)
	{
		if (FBT_HCI_SUCCESS(Disconnect(
								pBTDevice->ConnectionHandle,
								FBT_HCI_REMOTE_USER_TERMINATED_CONNECTION)))
		{
			MessageBox("Disconnected");

		}

		else
		{
			CString szMsg;
			szMsg.Format("Disconnect on handle 0x%04x failed", pBTDevice->ConnectionHandle);
			MessageBox(szMsg);
			pBTDevice->ConnectionHandle=0;

		}

	}

}


DWORD CALLBACK LogItemHandler(LPVOID lpParameter)
{
	PLogItem pLogItem=(PLogItem)lpParameter;

	char szBuffer[1024] = {0};
	char szBufferShort[10] = {0};

	if (pLogItem->bEvent)
		sprintf(szBuffer, "E: ");

	else
		sprintf(szBuffer, "C: ");

	for (DWORD i=0; i<pLogItem->dwLength; i++)
	{
		sprintf(szBufferShort, "%02x ", pLogItem->pBuffer[i]);
		strcat(szBuffer, szBufferShort);

	}

	pLogItem->pThis->m_lbHCI.AddString(szBuffer);

	delete[] pLogItem->pBuffer;
	delete (pLogItem);

	return 0;

}

// Hook the Event handler so that we can log events
DWORD CBTTestDlg::OnEvent(PFBT_HCI_EVENT_HEADER pEvent, DWORD Length)
{
	// This isn't particularly efficient, but this is just a test app...

	// Spin a thread to handle processing the event,
	// otherwise we'll hang up the dialog
	PLogItem pLogItem=new LogItem;
	pLogItem->pThis=this;
	pLogItem->bEvent=TRUE;
	pLogItem->pBuffer=(BYTE*)malloc(Length);
	memcpy(pLogItem->pBuffer, pEvent, Length);
	pLogItem->dwLength=Length;

	HANDLE hThread = CreateThread( NULL, 0, LogItemHandler, pLogItem, 0, NULL );
	CloseHandle( hThread );

	// Let the superclass do the actual handling
	return CHciRoundTrip::OnEvent( pEvent, Length );

}

// Hook SendHciCommand, so that we can log the commands
DWORD CBTTestDlg::SendHciCommand(const PFBT_HCI_CMD_HEADER lpCommand, DWORD dwBufferSize)
{
	// This isn't particularly efficient, but this is just a test app...

	// Spin a thread to handle processing the event,
	// otherwise we'll hang up the dialog
	PLogItem pLogItem = new LogItem;
	pLogItem->pThis=this;
	pLogItem->bEvent=FALSE;
	pLogItem->pBuffer= new BYTE[dwBufferSize];
	memcpy( pLogItem->pBuffer, lpCommand, dwBufferSize );
	pLogItem->dwLength=dwBufferSize;

	HANDLE hThread = CreateThread( NULL, 0, LogItemHandler, pLogItem, 0, NULL );
	CloseHandle( hThread );

	// Let the superclass do the actual handling
	return CHciLocal::SendHciCommand( lpCommand, dwBufferSize );

}

BOOL CBTTestDlg::AddressToString(BYTE BD_ADDR[FBT_HCI_BDADDR_SIZE], CString &szAddress)
{
	szAddress.Format(_T("%02X:%02X:%02X:%02X:%02X:%02X"),
						BD_ADDR[5],
						BD_ADDR[4],
						BD_ADDR[3],
						BD_ADDR[2],
						BD_ADDR[1],
						BD_ADDR[0]);

	return TRUE;

}

int CBTTestDlg::FindDeviceByConnectionHandle(USHORT nConnectionHandle)
{
	PBTDevice pBTDevice=NULL;
	int i=0;
	while (i<m_lbDevices.GetCount())
	{
		pBTDevice=(PBTDevice)m_lbDevices.GetItemData(i);
		if (pBTDevice && pBTDevice->ConnectionHandle==nConnectionHandle)
		{
			return i;

		}

		i++;

	}

	return -1;

}

int CBTTestDlg::FindDeviceByAddress(BYTE BD_ADDR[FBT_HCI_BDADDR_SIZE])
{
	PBTDevice pBTDevice=NULL;
	int i=0;
	while (i<m_lbDevices.GetCount())
	{
		pBTDevice=(PBTDevice)m_lbDevices.GetItemData(i);
		if (pBTDevice && CompareBDADDRs(BD_ADDR, pBTDevice->BD_ADDR)==0)
		{
			return i;

		}

		i++;

	}

	return -1;

}

DWORD CBTTestDlg::OnConnectionRequest(BYTE BD_ADDR[FBT_HCI_BDADDR_SIZE], ULONG ClassOfDevice[FBT_HCI_DEVICE_CLASS_SIZE], BYTE LinkType)
{
	CString szDevice;
	szDevice.Format(_T("%02X:%02X:%02X:%02X:%02X:%02X"),
						BD_ADDR[5],
						BD_ADDR[4],
						BD_ADDR[3],
						BD_ADDR[2],
						BD_ADDR[1],
						BD_ADDR[0]);

	CString szMsg;
	szMsg.Format("Connection request from %s, accept?", szDevice);

	if (MessageBox(szMsg, "Connection request", MB_YESNO)==IDNO)
		return 0;

	SendAcceptConnectionRequest(BD_ADDR, FBT_HCI_ROLE_SLAVE);

	return 1;

}

DWORD CBTTestDlg::OnConnectionComplete(BYTE nStatus, USHORT nConnectionHandle, BYTE BD_ADDR[FBT_HCI_BDADDR_SIZE], BYTE nLinkType, BYTE nEncryptionMode)
{
	if (nStatus==0)
	{
		CString szDevice;
		AddressToString(BD_ADDR, szDevice);

		PBTDevice pBTDevice=NULL;
		int nDevice=FindDeviceByAddress(BD_ADDR);
		if (nDevice==-1)
		{
			int nDevice=m_lbDevices.AddString(szDevice);
			if (nDevice!=LB_ERR)
			{
				pBTDevice=(PBTDevice)malloc(sizeof(BTDevice));
				memset(pBTDevice, 0, sizeof(BTDevice));
				m_lbDevices.SetItemData(nDevice, (DWORD)pBTDevice);

			}

		}

		else
		{
			pBTDevice=(PBTDevice)m_lbDevices.GetItemData(nDevice);

		}

		memcpy(pBTDevice->BD_ADDR, BD_ADDR, FBT_HCI_BDADDR_SIZE);
		pBTDevice->ConnectionHandle=nConnectionHandle;

		CString szMsg;
		szMsg.Format("Connected to %s", szDevice);
		MessageBox(szMsg, "Device connected", MB_OK);

	}

	return 0;

}

DWORD CALLBACK CBTTestDlg::Reader(LPVOID lpParameter)
{
	FBT_TRY

	CBTTestDlg *pThis=(CBTTestDlg*)lpParameter;

	BYTE			pBuffer[1024]={0};
	DWORD			dwBytesRead=0;

	char szMsg[256]={0};
	char szBuf[50]={0};

	DWORD i;
	while (true)
	{
		memset(pBuffer, 0, 1024);
		dwBytesRead=0;
		if (pThis->GetData(pBuffer, 1024, &dwBytesRead, NULL)==ERROR_SUCCESS)
		{
			fbtLog(fbtLog_Verbose, "CBTTestDlg::Reader: Read %d bytes", dwBytesRead);
			if (dwBytesRead>0)
			{
				sprintf(szMsg, "R: ");
				for (i=0; i<dwBytesRead; i++)
				{
					sprintf(szBuf, "%02x ", ((PBYTE)pBuffer)[i]);
					strcat(szMsg, szBuf);

				}

				if (dwBytesRead>=sizeof(FBT_HCI_DATA_PACKET))
				{
					PFBT_HCI_DATA_PACKET pData=(PFBT_HCI_DATA_PACKET)pBuffer;
					fbtLog(fbtLog_Verbose, "CBTTestDlg::Reader: %d bytes ACL data on ConnectionHandle %02x", pData->DataLength, pData->ConnectionHandle);

					int nDevice=pThis->FindDeviceByConnectionHandle(pData->ConnectionHandle);
					if (nDevice!=-1)
					{
						PBTDevice pBTDevice=(PBTDevice)pThis->m_lbDevices.GetItemData(nDevice);
						if (pBTDevice)
						{
							CString szDeviceName;
							if (strlen((LPCSTR)(pBTDevice->Name))>0)
							{
								szDeviceName=(LPCSTR)(pBTDevice->Name);

							}

							else
							{
								pThis->AddressToString(pBTDevice->BD_ADDR, szDeviceName);

							}

							szDeviceName+=" Says \"";
							szDeviceName+=(LPCSTR)pData->Data;
							szDeviceName+="\"";

							pThis->m_lbDevices.DeleteString(nDevice);
							nDevice=pThis->m_lbDevices.AddString(szDeviceName);
							pThis->m_lbDevices.SetItemData(nDevice, (DWORD)pBTDevice);
							pThis->m_lbDevices.SetCurSel(nDevice);

						}

						else
						{
							fbtLog(fbtLog_Failure, "CBTTestDlg::Reader: Failed to retrieve device structure");

						}

					}

					else
					{
						fbtLog(fbtLog_Failure, "CBTTestDlg::Reader: Unable to find connection handle %02x", pData->ConnectionHandle);

					}

				}

				else
				{
					fbtLog(fbtLog_Failure, "CBTTestDlg::Reader: Not enough data for a valida ACL packet");

				}

				pThis->m_lbHCI.AddString(szMsg);
				fbtLog(fbtLog_Verbose, "CBTTestDlg::Reader: %s", szMsg);


			}

			else
			{
				fbtLog(fbtLog_Failure, "CBTTestDlg::Reader: 0 bytes read");

			}

		}

		else
			fbtLog(fbtLog_Failure, "CBTTestDlg::Reader: GetData failed");


	}

	return 0;

	FBT_CATCH_RETURN(0)

}

void CBTTestDlg::OnSend()
{
	int nDevice=m_lbDevices.GetCurSel();
	if (nDevice==LB_ERR)
	{
		MessageBox("Select a device");
		return;

	}

	CString szSendText;
	m_edtSendText.GetWindowText(szSendText);
	if (szSendText.IsEmpty())
	{
		MessageBox("Enter some text to send");
		return;

	}

	PBTDevice pBTDevice=(PBTDevice)m_lbDevices.GetItemData(nDevice);
	if (pBTDevice)
	{
		USHORT dwTextLength=szSendText.GetLength();

		// Allocate a data packet with enough space
		DWORD dwDataLength=sizeof(FBT_HCI_DATA_PACKET)+dwTextLength;
		PFBT_HCI_DATA_PACKET pData=(PFBT_HCI_DATA_PACKET)malloc(dwDataLength);
		memset(pData, 0, dwDataLength);

		pData->ConnectionHandle=pBTDevice->ConnectionHandle;
		pData->DataLength=dwTextLength+1; // Include enough space for the terminator
		pData->Broadcast=FBT_HCI_BROADCAST_POINT_TO_POINT;
		pData->PacketBoundary=FBT_HCI_PACKET_BOUNDARY_START;
		sprintf((char*)(pData->Data), szSendText);

		DWORD dwBytesWritten=0;
		if (SendData(pData, dwDataLength, &dwBytesWritten, NULL)==ERROR_SUCCESS)
		{
			char szBuffer[1024]={0};
			char szBufferShort[10]={0};

			sprintf(szBuffer, "W: ");
			for (DWORD i=0; i<dwDataLength; i++)
			{
				sprintf(szBufferShort, "%02x ", ((PBYTE)pData)[i]);
				strcat(szBuffer, szBufferShort);

			}

			m_lbHCI.AddString(szBuffer);
			m_edtSendText.SetWindowText("");

		}

		else
			MessageBox("Send failed");

		free(pData);

	}

}

void CBTTestDlg::OnOK()
{
	OnSend();
}
