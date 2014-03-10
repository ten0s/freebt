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

/**
 * The original and principal author is Antony C. Roberts http://www.freebt.net.  
 * Other authors are noted in the change history that follows (in reverse chronological order):
 *
 * 2009-03-03 Dmitry Klionsky Changed GetManufacturerName method's getting results. Looks like problems with Vista ASLR & DEP.
 * 2008-11-30 Dmitry Klionsky Added UNICODE support.
 * 2008-11-30 Dmitry Klionsky CBTHW class is no longer the base class. The CHci class receives a reference on CBTHW class at its construction.
 * 2008-11-30 Dmitry Klionsky More manufacturers were added.
 */

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

#include "fbtutil.h"
#include "fbtusr.h"
#include "fbthci.h"

LPCTSTR g_fbt_hci_Status[] =
{
    _T("Success"),
    _T("Unknown HCI Command"),
    _T("No Connection"),
    _T("Hardware Failure"),
    _T("Page Timeout"),
    _T("Authentication Failure"),
    _T("Key Missing"),
    _T("Memory Full"),
    _T("Connection Timeout"),
    _T("Max Number Of Connections"),
    _T("Max Number Of SCO Connections To A Device"),
    _T("ACL connection already exists"),
    _T("Command Disallowed"),
    _T("Host Rejected due to limited resources"),
    _T("Host Rejected due to security reasons"),
    _T("Host Rejected due to remote device is only a personal device"),
    _T("Host Timeout"),
    _T("Unsupported Feature or Parameter Value"),
    _T("Invalid HCI Command Parameters"),
    _T("Other End Terminated Connection: User Ended Connection"),
    _T("Other End Terminated Connection: Low Resources"),
    _T("Other End Terminated Connection: About to Power Off"),
    _T("Connection Terminated by Local Host"),
    _T("Repeated Attempts"),
    _T("Pairing Not Allowed"),
    _T("Unknown LMP PDU"),
    _T("Unsupported Remote Feature"),
    _T("SCO Offset Rejected"),
    _T("SCO Interval Rejected"),
    _T("SCO Air Mode Rejected"),
    _T("Invalid LMP Parameters"),
    _T("Unspecified Error"),
    _T("Unsupported LMP Parameter Value")

};

LPCTSTR g_fbt_hci_Event[] =
{
    _T("<Invalid Event>"),
    _T("Inquiry Complete"),
    _T("Inquiry Result"),
    _T("Connection Complete"),
    _T("Connection Request"),
    _T("Disconnection Complete"),
    _T("Authentication Complete"),
    _T("Remote Name Request Complete"),
    _T("Encryption Change"),
    _T("Change Connection Link Key Complete"),
    _T("Master Link Key Complete"),
    _T("Read Remote Supported Features Complete"),
    _T("Read Remote Version Information"),
    _T("QoS Setup Complete"),
    _T("Command Complete"),
    _T("Command Status"),
    _T("Hardware Error"),
    _T("Flush Occurred"),
    _T("Role Change"),
    _T("Number Of Completed Packets"),
    _T("Mode Change"),
    _T("Return Link Keys"),
    _T("PIN Code Request"),
    _T("Link Key Request"),
    _T("Link Key Notification"),
    _T("Loopback Command"),
    _T("Data Buffer Overflow"),
    _T("Max Slots Change"),
    _T("Read Clock Offset Complete"),
    _T("Connection Packet Type Changed"),
    _T("QoS Violation"),
    _T("Page Scan Mode Change"),
    _T("Page Scan Repetition Mode Change")

};

LPCTSTR g_fbt_hci_Company[] =
{
	_T("Ericsson Technology Licensing"),
	_T("Nokia Mobile Phones"),
	_T("Intel Corp."),
	_T("IBM Corp."),
	_T("Toshiba Corp."),
	_T("3Com"),
	_T("Microsoft"),
	_T("Lucent"),
	_T("Motorola"),
	_T("Infineon Technologies AG"),
	_T("Cambridge Silicon Radio"),
	_T("Silicon Wave"),
	_T("Digianswer A/S"),
	_T("Texas Instruments Inc."),
	_T("Parthus Technologies Inc."),
	_T("Broadcom Corporation"),
	_T("Mitel Semiconductor"),
	_T("Widcomm, Inc."),
	_T("Zeevo, Inc."),
	_T("Atmel Corporation"),
	_T("Mitsubishi Electric Corporation"),
	_T("RTX Telecom A/S"),
	_T("KC Technology Inc."),
	_T("Newlogic"),
	_T("Transilica, Inc."),
	_T("Rohde & Schwarz GmbH & Co. KG"),
	_T("TTPCom Limited"),
	_T("Signia Technologies, Inc."),
	_T("Conexant Systems Inc."),
	_T("Qualcomm"),
	_T("Inventel"),
	_T("AVM Berlin"),
	_T("BandSpeed, Inc."),
	_T("Mansella Ltd"),
	_T("NEC Corporation"),
	_T("WavePlus Technology Co., Ltd."),
	_T("Alcatel"),
	_T("Philips Semiconductors"),
	_T("C Technologies"),
	_T("Open Interface"),
	_T("R F Micro Devices"),
	_T("Hitachi Ltd"),
	_T("Symbol Technologies, Inc."),
	_T("Tenovis"),
	_T("Macronix International Co. Ltd."),
	_T("GCT Semiconductor"),
	_T("Norwood Systems"),
	_T("MewTel Technology Inc."),
	_T("ST Microelectronics"),
	_T("Synopsys"),
	_T("Red-M (Communications) Ltd"),
	_T("Commil Ltd"),
	_T("Computer Access Technology Corporation (CATC)"),
	_T("Eclipse (HQ Espana) S.L."),
	_T("Renesas Technology Corp."),
	_T("Mobilian Corporation"),
	_T("Terax"),
	_T("Integrated System Solution Corp."),
	_T("Matsushita Electric Industrial Co., Ltd."),
	_T("Gennum Corporation"),
	_T("Research In Motion"),
   _T("IPextreme, Inc."),
   _T("Systems and Chips, Inc"),
   _T("Bluetooth SIG, Inc"),
   _T("Seiko Epson Corporation"),
   _T("Integrated Silicon Solution Taiwain, Inc."),
   _T("CONWISE Technology Corporation Ltd"),
   _T("PARROT SA"),
   _T("Socket Communications"),
   _T("Atheros Communications, Inc."),
   _T("MediaTek, Inc."),
   _T("Bluegiga"),
   _T("Marvell Technology Group Ltd."),
   _T("3DSP Corporation"),
   _T("Accel Semiconductor Ltd."),
   _T("Continental Automotive Systems"),
   _T("Apple, Inc."),
   _T("Staccato Communications, Inc."),
   _T("Avago Technologies"),
   _T("APT Ltd."),
   _T("SiRF Technology, Inc."),
   _T("Tzero Technologies, Inc."),
   _T("J&M Corporation")
};

CHci::CHci( CBTHW& btHw ) : m_btHw( btHw )
{
	FBT_TRY

      fbtLog(fbtLog_Enter, _T("CHci::CHci: Enter"));

      m_hListenerThread=NULL;
    m_hStopListeningEvent=CreateEvent(NULL, FALSE, FALSE, NULL);
    DWORD dwError = GetLastError();

    fbtLog(fbtLog_Exit, _T("CHci::CHci: Exit"));

    FBT_CATCH_NORETURN

}

CHci::~CHci()
{
	FBT_TRY

    StopEventListener();
    CloseHandle(m_hStopListeningEvent);
    m_hStopListeningEvent = NULL;

    FBT_CATCH_NORETURN
}

DWORD CHci::StartEventListener()
{
	FBT_TRY

	if (m_hListenerThread!=NULL)
	{
		fbtLog(fbtLog_Failure, _T("CHci:StartEventListener: Listener already running"));
		return ERROR_INTERNAL_ERROR;

	}

	m_hListenerReadyEvent=CreateEvent(NULL, FALSE, FALSE, NULL);
    m_hListenerThread=CreateThread(NULL, 0, Listener, this, 0, &m_dwListenerThreadId);
    if (m_hListenerThread==NULL)
    {
        DWORD dwLastError=GetLastError();
        CloseHandle(m_hListenerReadyEvent);
        fbtLog(fbtLog_Failure, _T("CHci::StartEventListener: Failed to create listener thread, error %d"), dwLastError);
        return dwLastError;

    }

    WaitForSingleObject(m_hListenerReadyEvent, INFINITE);
    CloseHandle(m_hListenerReadyEvent);

    return ERROR_SUCCESS;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHci::StopEventListener()
{
	FBT_TRY

    SetEvent(m_hStopListeningEvent);
    DWORD dwResult=WaitForSingleObject(m_hListenerThread, INFINITE);
    CloseHandle(m_hListenerThread);
    m_hListenerThread = NULL;

	 return dwResult;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)
}

// Event handler thread routine
DWORD CALLBACK EventHandler(LPVOID pContext)
{
    FBT_TRY

	PHCI_EVENT	pEvent=(PHCI_EVENT)pContext;
    CHci		*pThis=(CHci*)pEvent->pThis;

	DWORD dwResult=pThis->OnEvent(pEvent->pEvent, pEvent->dwLength);
	if (dwResult!=ERROR_SUCCESS)
		fbtLog(fbtLog_Failure, _T("CHci::EventHandler: OnEvent failed, error %d"), dwResult);

	fbtLog(fbtLog_Notice, _T("CHci::EventHandler: Event handling complete"));

	free(pEvent->pEvent);
	free(pEvent);

    return ERROR_SUCCESS;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CALLBACK Listener(LPVOID pContext)
{
    FBT_TRY

    CHci *pThis=(CHci*)pContext;

    HANDLE      hEvents[HCI_NUMBER_OF_OVERLAPPED_LISTENS+1];

    hEvents[0]=pThis->m_hStopListeningEvent;

    // Start the listeners. Each call will 'hang' in the driver
	// until an event arrives, which causes it to be completed
    for (int i=0; i<HCI_NUMBER_OF_OVERLAPPED_LISTENS; i++)
    {
        ZeroMemory(pThis->m_pEventBuffers[i], FBT_HCI_EVENT_MAX_SIZE);
        ZeroMemory(&pThis->m_Overlappeds[i], sizeof(OVERLAPPED));

        pThis->m_Overlappeds[i].hEvent=CreateEvent(NULL, TRUE, FALSE, NULL);
        if (pThis->m_Overlappeds[i].hEvent==NULL)
            fbtLog(fbtLog_Failure, _T("CHci::Listener: Failed to create event %d"), i);

        else
        {
            hEvents[i+1]=pThis->m_Overlappeds[i].hEvent;
            if (pThis->m_btHw.SendCommand(
                        IOCTL_FREEBT_HCI_GET_EVENT,
                        NULL,
                        0,
                        pThis->m_pEventBuffers[i],
                        FBT_HCI_EVENT_MAX_SIZE,
                        &pThis->m_Overlappeds[i])!=ERROR_SUCCESS)
                fbtLog(fbtLog_Failure, _T("CHci::Listener: Failed to send listen request %d"), i);

        }

    }

	SetEvent(pThis->m_hListenerReadyEvent);

    BOOL bListen=TRUE;
    while (bListen)
    {
        // Wait for Listens to complete
        DWORD dwWaitResult=WaitForMultipleObjects(HCI_NUMBER_OF_OVERLAPPED_LISTENS+1,
                                                    hEvents,
                                                    FALSE,
                                                    INFINITE);

        DWORD dwLastError;
        if (dwWaitResult==WAIT_FAILED)
        {
            dwLastError=GetLastError();
            fbtLog(fbtLog_Failure, _T("CHci::Listener: WaitForMultipleObjects failed, error %d"), dwLastError);
            return dwLastError;

        }

        if (dwWaitResult==WAIT_TIMEOUT)
        {
            fbtLog(fbtLog_Failure, _T("CHci::Listener: WaitForMultipleObjects timed out"));
            return ERROR_TIMEOUT;

        }

        // Find out which listener got complete
        ULONG nEventIndex=dwWaitResult-WAIT_OBJECT_0;
        if (nEventIndex>0)
        {
            nEventIndex--;
            fbtLog(fbtLog_Notice, _T("CHci::Listener: Event signalled on listener %d"), nEventIndex);
            DWORD dwLength;
            if (!GetOverlappedResult(pThis->m_btHw.GetDriverHandle(), &pThis->m_Overlappeds[nEventIndex], &dwLength, TRUE))
            {
                dwLastError=GetLastError();
                fbtLog(fbtLog_Failure, _T("CHci::Listener: GetOverlappedResult failed, error %d"), dwLastError);
                return dwLastError;

            }

            // Handle the event
            PFBT_HCI_EVENT_HEADER pEvent=(PFBT_HCI_EVENT_HEADER)pThis->m_pEventBuffers[nEventIndex];
            fbtLog(fbtLog_Notice, _T("CHci::Listener: Received %s event (0x%02x)"), pThis->GetEventText(pEvent->EventCode), pEvent->EventCode);

			// Filter for must-handle events (OnEvent can be overridden)
			PHCI_EVENT pEventParameters=(PHCI_EVENT)malloc(sizeof(HCI_EVENT));
			pEventParameters->pEvent=(PFBT_HCI_EVENT_HEADER)malloc(dwLength);
			CopyMemory(pEventParameters->pEvent, pEvent, dwLength);
			pEventParameters->dwLength=dwLength;
			pEventParameters->pThis=pThis;

			// Spin handling off in a thread in order to reduce turnaround time
			HANDLE hThread=CreateThread(NULL, 0, ::EventHandler, pEventParameters, 0, NULL);
			if (hThread==NULL)
			{
				DWORD dwLastError=GetLastError();
				CloseHandle(hThread);
				fbtLog(fbtLog_Failure, _T("CHci::StartEventListener: Failed to create listener thread, error %d"), dwLastError);
				return dwLastError;

			}

			CloseHandle(hThread);

            // Send the listen for events request back down to the driver
            ZeroMemory(pThis->m_pEventBuffers[nEventIndex], FBT_HCI_EVENT_MAX_SIZE);
            ResetEvent(pThis->m_Overlappeds[nEventIndex].hEvent);
            if (pThis->m_btHw.SendCommand(
                        IOCTL_FREEBT_HCI_GET_EVENT,
                        NULL,
                        0,
                        pThis->m_pEventBuffers[nEventIndex],
                        FBT_HCI_EVENT_MAX_SIZE,
                        &pThis->m_Overlappeds[nEventIndex])!=ERROR_SUCCESS)
                fbtLog(fbtLog_Failure, _T("CHci::Listener: Failed to resend listen request %d"), nEventIndex);

        }

        else
            bListen=FALSE;


    }

    fbtLog(fbtLog_Notice, _T("CHci::Listener: Stop signalled, exitting"));

    return ERROR_SUCCESS;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

// Get the description that corresponds to an HCI status code
LPCTSTR CHci::GetStatusText(BYTE Status)
{
	FBT_TRY

    if (Status>32)
        return (LPTSTR)NULL;

    return g_fbt_hci_Status[Status];

    FBT_CATCH_RETURN(NULL)

}

// Get the name of an HCI event
LPCTSTR CHci::GetEventText(BYTE Event)
{
	FBT_TRY

    if (Event>32)
        return (LPTSTR)NULL;

    return g_fbt_hci_Event[Event];

    FBT_CATCH_RETURN(NULL)
}

// Get the name of a Manufacturer
DWORD CHci::GetManufacturerName(USHORT Company, LPTSTR /*in*/szInBuffer, DWORD dwBufferLength)
{
   FBT_TRY
      
      // zero the buffer.
      memset(szInBuffer, 0, dwBufferLength);
   
      // check for correct input.
      if (Company >= sizeof(g_fbt_hci_Company)/sizeof(g_fbt_hci_Company[0])) {
         return ERROR_INVALID_PARAMETER;
      }

      LPCTSTR szManufacturer = g_fbt_hci_Company[Company];

      // check for buffer size.
      if (_tcslen(szManufacturer) + 1 > dwBufferLength) {
         return ERROR_INSUFFICIENT_BUFFER;
      }

      _tcscpy(szInBuffer, szManufacturer);

      return ERROR_SUCCESS;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)
}

// Compare two BDADDRs
DWORD CHci::CompareBDADDRs(BYTE BD_ADDR1[FBT_HCI_BDADDR_SIZE], BYTE BD_ADDR2[FBT_HCI_BDADDR_SIZE])
{
	FBT_TRY

	for (int i=0; i<FBT_HCI_BDADDR_SIZE; i++)
	{
		if (BD_ADDR1[i]!=BD_ADDR2[i])
		{
			return 1;
		}
	}

	return ERROR_SUCCESS;

	FBT_CATCH_RETURN(0)
}

// Commands
DWORD CHci::SendHciCommand(const PFBT_HCI_CMD_HEADER lpCommand, DWORD dwBufferSize)
{
    FBT_TRY

    fbtLog(fbtLog_Enter, _T("CHci::SendHciCommand: Enter(lpCommand=%X, dwBufferSize=%d"), lpCommand, dwBufferSize);

    if (GetCurrentThreadId()==m_dwListenerThreadId)
    {
        fbtLog(fbtLog_Failure, _T("CHci::SendHciCommand: ERROR: Command dispatch sent from Listener thread context!"));
        return ERROR_INTERNAL_ERROR;

    }

    fbtLog(fbtLog_Notice, _T("CHci::SendHciCommand: Sending command OGF=0x%02x OCF=0x%02x"), lpCommand->OpCode>>10, lpCommand->OpCode&0x3FF);
    if (dwBufferSize-sizeof(FBT_HCI_CMD_HEADER)>0 && fbtLogGetLevel()>=fbtLog_Verbose)
    {
      fbtLog(fbtLog_Notice, _T("CHci::SendHciCommand: Command paramemters: "));
      byte *lpBuffer=(byte*)lpCommand;
		TCHAR szDebugBite[4];
		TCHAR *szDebugString=(TCHAR*)malloc((dwBufferSize*3)+1);
		szDebugString[0]=0;
        for (UINT i=0; i<dwBufferSize; i++)
		{
			_sntprintf(szDebugBite, 4, _T("%02X "), lpBuffer[i]);
			szDebugBite[3]=0;
         _tcscat(szDebugString, szDebugBite);

		}

		fbtLog(fbtLog_Verbose, _T("CHci::SendHciCommand: HCI Command buffer contents: %s"), szDebugString);
		free(szDebugString);


    }

    DWORD dwResult=m_btHw.SendCommand(IOCTL_FREEBT_HCI_SEND_CMD, lpCommand, dwBufferSize, NULL, 0);
    if (dwResult!=ERROR_SUCCESS)
    {
        fbtLog(fbtLog_Failure, _T("CHci::SendHciCommand: SendCommand failed, last error=%d"), dwResult);
        return dwResult;

    }

    fbtLog(fbtLog_Exit, _T("CHci::SendHciCommand: Exit"));

    return ERROR_SUCCESS;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)
}

DWORD CHci::SendReset()
{
    FBT_TRY

    fbtLog(fbtLog_Enter, _T("CHci::SendReset: Enter"));

    FBT_HCI_RESET commandBuffer;

    commandBuffer.CommandHeader.OpCode = FBT_HCI_CMD_RESET;
    commandBuffer.CommandHeader.ParameterLength = sizeof(FBT_HCI_RESET) - sizeof(FBT_HCI_CMD_HEADER);

    DWORD dwReturnCode=SendHciCommand((PFBT_HCI_CMD_HEADER)&commandBuffer, sizeof(commandBuffer));

    fbtLog(fbtLog_Exit, _T("CHci::SendHciReset: Exit"));

    return dwReturnCode;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHci::SendInquiry(ULONG LAP, BYTE InquiryLength, BYTE NumResponses)
{
	FBT_TRY

    fbtLog(fbtLog_Enter, _T("CHci::SendInquiry: Enter(LAP=%d, InquiryLength=%d, NumResponses=%d)"), LAP, InquiryLength, NumResponses);

    FBT_HCI_INQUIRY commandBuffer;

    commandBuffer.CommandHeader.OpCode = FBT_HCI_CMD_INQUIRY;
    commandBuffer.CommandHeader.ParameterLength = sizeof(FBT_HCI_INQUIRY) - sizeof(FBT_HCI_CMD_HEADER);

    PBYTE pBuffer=(PBYTE)&LAP;
    commandBuffer.LAP[0] = pBuffer[0]; //(BYTE)(LAP & 0x000000FF);
    commandBuffer.LAP[1] = pBuffer[1]; //(BYTE)((LAP & 0x0000FF00) >> 8);
    commandBuffer.LAP[2] = pBuffer[2]; //(BYTE)((LAP & 0x00FF0000) >> 16);

	commandBuffer.InquiryLength = InquiryLength;
    commandBuffer.NumResponses = NumResponses;

    DWORD dwReturnCode=SendHciCommand((PFBT_HCI_CMD_HEADER)&commandBuffer, sizeof(commandBuffer));

    fbtLog(fbtLog_Exit, _T("CHci::SendInquiry: Exit"));

    return dwReturnCode;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHci::SendInquiryCancel()
{
	FBT_TRY

    fbtLog(fbtLog_Enter, _T("CHci::SendInquiryCancel: Enter"));

    FBT_HCI_INQUIRY_CANCEL commandBuffer;

    commandBuffer.CommandHeader.OpCode = FBT_HCI_CMD_INQUIRY_CANCEL;
    commandBuffer.CommandHeader.ParameterLength = 0;

    DWORD dwReturnCode=SendHciCommand((PFBT_HCI_CMD_HEADER)&commandBuffer, sizeof(commandBuffer));

    fbtLog(fbtLog_Exit, _T("CHci::SendInquiryCancel: Exit"));

    return dwReturnCode;

	FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHci::SendReadBDADDR()
{
	FBT_TRY

    FBT_HCI_READ_BD_ADDR commandBuffer;
    commandBuffer.CommandHeader.OpCode=FBT_HCI_CMD_READ_BD_ADDR;
    commandBuffer.CommandHeader.ParameterLength=sizeof(FBT_HCI_READ_BD_ADDR) - sizeof(FBT_HCI_CMD_HEADER);

    return SendHciCommand((PFBT_HCI_CMD_HEADER)&commandBuffer, sizeof(commandBuffer));

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHci::SendReadBufferSize()
{
   FBT_TRY

   FBT_HCI_READ_BUFFER_SIZE commandBuffer;
   commandBuffer.CommandHeader.OpCode=FBT_HCI_CMD_READ_BUFFER_SIZE;
   commandBuffer.CommandHeader.ParameterLength=sizeof(FBT_HCI_READ_BUFFER_SIZE) - sizeof(FBT_HCI_CMD_HEADER);

   return SendHciCommand((PFBT_HCI_CMD_HEADER)&commandBuffer, sizeof(commandBuffer));

   FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHci::SendWriteScanEnable(BYTE ScanEnable)
{
	FBT_TRY

	fbtLog(fbtLog_Enter, _T("CHci::SendWriteScanEnable: Enter, ScanEnable=%d"), ScanEnable);

    FBT_HCI_WRITE_SCAN_ENABLE commandBuffer;

    commandBuffer.CommandHeader.OpCode = FBT_HCI_CMD_WRITE_SCAN_ENABLE;
    commandBuffer.CommandHeader.ParameterLength = sizeof(FBT_HCI_WRITE_SCAN_ENABLE) - sizeof(FBT_HCI_CMD_HEADER);

    commandBuffer.ScanEnable = ScanEnable;

    DWORD dwResult=SendHciCommand((PFBT_HCI_CMD_HEADER)&commandBuffer, sizeof(commandBuffer));

    fbtLog(fbtLog_Exit, _T("CHci::SendWriteScanEnable: Exit, result %d"), dwResult);

    return dwResult;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHci::SendWriteAuthenticationEnable(BYTE AuthenticationEnable)
{
	FBT_TRY

    FBT_HCI_WRITE_AUTHENTICATION_ENABLE commandBuffer;

    commandBuffer.CommandHeader.OpCode = FBT_HCI_CMD_WRITE_AUTHENTICATION_ENABLE;
    commandBuffer.CommandHeader.ParameterLength = sizeof(FBT_HCI_WRITE_AUTHENTICATION_ENABLE) - sizeof(FBT_HCI_CMD_HEADER);

    commandBuffer.AuthenticationEnable = AuthenticationEnable;

    return SendHciCommand((PFBT_HCI_CMD_HEADER)&commandBuffer, sizeof(commandBuffer));

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHci::SendSetEventFilter(BYTE FilterType, BYTE FilterConditionType, BYTE Condition[FBT_HCI_MAX_CONDITION_SIZE], BYTE ConditionBytes)
{
	FBT_TRY

    FBT_HCI_SET_EVENT_FILTER commandBuffer;
	BYTE BufSize=sizeof(commandBuffer)-(FBT_HCI_MAX_CONDITION_SIZE-ConditionBytes);

    commandBuffer.CommandHeader.OpCode = FBT_HCI_CMD_SET_EVENT_FILTER;
    commandBuffer.CommandHeader.ParameterLength = BufSize - sizeof(FBT_HCI_CMD_HEADER);

    commandBuffer.FilterType = FilterType;
    commandBuffer.FilterConditionType = FilterConditionType;

	// Initialise the buffer (not all bytes are used for all conditions)
	memset(commandBuffer.Condition, 0, sizeof(commandBuffer.Condition));
    CopyMemory(commandBuffer.Condition, Condition, ConditionBytes);

    return SendHciCommand((PFBT_HCI_CMD_HEADER)&commandBuffer, BufSize);

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHci::SendReadClassOfDevice()
{
	FBT_TRY

    FBT_HCI_READ_CLASS_OF_DEVICE commandBuffer;
    commandBuffer.CommandHeader.OpCode = FBT_HCI_CMD_READ_CLASS_OF_DEVICE;
    commandBuffer.CommandHeader.ParameterLength = sizeof(FBT_HCI_READ_CLASS_OF_DEVICE) - sizeof(FBT_HCI_CMD_HEADER);

    return SendHciCommand((PFBT_HCI_CMD_HEADER)&commandBuffer, sizeof(commandBuffer));

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

// Set the class of device bits which indicate what type of device this is
// to inquirying devices
DWORD CHci::SendWriteClassOfDevice(BYTE ClassOfDevice[FBT_HCI_DEVICE_CLASS_SIZE])
{
	FBT_TRY

	fbtLog(fbtLog_Enter, _T("SendWriteClassOfDevice: Enter, ClassOfDevice=%02x %02x %02x"), ClassOfDevice[0], ClassOfDevice[1], ClassOfDevice[2]);

    FBT_HCI_WRITE_CLASS_OF_DEVICE commandBuffer;
    commandBuffer.CommandHeader.OpCode = FBT_HCI_CMD_WRITE_CLASS_OF_DEVICE;
    commandBuffer.CommandHeader.ParameterLength = sizeof(FBT_HCI_WRITE_CLASS_OF_DEVICE) - sizeof(FBT_HCI_CMD_HEADER);

    CopyMemory(commandBuffer.ClassOfDevice, ClassOfDevice, FBT_HCI_DEVICE_CLASS_SIZE);

    DWORD dwResult=SendHciCommand((PFBT_HCI_CMD_HEADER)&commandBuffer, sizeof(commandBuffer));

    fbtLog(fbtLog_Exit, _T("SendWriteClassOfDevice: Exit, dwResult=%d"), dwResult);

    return dwResult;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHci::SendCreateConnection(BYTE	BD_ADDR[FBT_HCI_BDADDR_SIZE],
                                 USHORT	PacketType,
                                 BYTE	PageScanRepetitionMode,
                                 BYTE	PageScanMode,
                                 USHORT	ClockOffset,
                                 BYTE	AllowRoleSwitch)
{
	FBT_TRY

    FBT_HCI_CREATE_CONNECTION commandBuffer;

    commandBuffer.CommandHeader.OpCode = FBT_HCI_CMD_CREATE_CONNECTION;
    commandBuffer.CommandHeader.ParameterLength = sizeof(FBT_HCI_CREATE_CONNECTION) - sizeof(FBT_HCI_CMD_HEADER);

    CopyMemory(commandBuffer.BD_ADDR, BD_ADDR, FBT_HCI_BDADDR_SIZE);
    commandBuffer.PacketType = PacketType;
    commandBuffer.PageScanRepetitionMode = PageScanRepetitionMode;
    commandBuffer.PageScanMode = PageScanMode;
    commandBuffer.ClockOffset = ClockOffset;
    commandBuffer.AllowRoleSwitch = AllowRoleSwitch;

    DWORD dwResult=SendHciCommand((PFBT_HCI_CMD_HEADER)&commandBuffer, sizeof(commandBuffer));
    fbtLog(fbtLog_Notice, _T("CHci::SendCreateConnection: Create_Connection sent"));
    return dwResult;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHci::SendAcceptConnectionRequest(BYTE BD_ADDR[FBT_HCI_BDADDR_SIZE], BYTE Role)
{
	FBT_TRY

    FBT_HCI_ACCEPT_CONNECTION_REQUEST commandBuffer;

    commandBuffer.CommandHeader.OpCode = FBT_HCI_CMD_CREATE_CONNECTION;
    commandBuffer.CommandHeader.ParameterLength = sizeof(FBT_HCI_CREATE_CONNECTION) - sizeof(FBT_HCI_CMD_HEADER);

    CopyMemory(commandBuffer.BD_ADDR, BD_ADDR, FBT_HCI_BDADDR_SIZE);
    commandBuffer.Role = Role;

    return SendHciCommand((PFBT_HCI_CMD_HEADER)&commandBuffer, sizeof(commandBuffer));

	FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHci::SendDisconnect(USHORT ConnectionHandle, BYTE Reason)
{
	FBT_TRY

    fbtLog(fbtLog_Enter, _T("CHci::SendDisconnect: Enter(ConnectionHandle=%02x, Reason=0x%x)"), ConnectionHandle, Reason);

    FBT_HCI_DISCONNECT commandBuffer;

    commandBuffer.CommandHeader.OpCode = FBT_HCI_CMD_DISCONNECT;
    commandBuffer.CommandHeader.ParameterLength = sizeof(FBT_HCI_DISCONNECT) - sizeof(FBT_HCI_CMD_HEADER);

    commandBuffer.ConnectionHandle = ConnectionHandle;
    commandBuffer.Reason = Reason;

    DWORD dwResult=SendHciCommand((PFBT_HCI_CMD_HEADER)&commandBuffer, sizeof(commandBuffer));

    fbtLog(fbtLog_Exit, _T("CHci::SendDisconnect: Exit"));

    return dwResult;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

// Max timeout betweem packets. Used for determining link loss
DWORD CHci::SendWriteLinkSupervisionTimeout(USHORT ConnectionHandle, USHORT LinkSupervisionTimeout)
{
	FBT_TRY

    FBT_HCI_WRITE_LINK_SUPERVISION_TIMEOUT commandBuffer;

    commandBuffer.CommandHeader.OpCode = FBT_HCI_CMD_WRITE_LINK_SUPERVISION_TIMEOUT;
    commandBuffer.CommandHeader.ParameterLength = sizeof(FBT_HCI_WRITE_LINK_SUPERVISION_TIMEOUT) - sizeof(FBT_HCI_CMD_HEADER);

    commandBuffer.ConnectionHandle = ConnectionHandle;
    commandBuffer.LinkSupervisionTimeout = LinkSupervisionTimeout;

    return SendHciCommand((PFBT_HCI_CMD_HEADER)&commandBuffer, sizeof(commandBuffer));

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

// Timeout within which a connection should be established
DWORD CHci::SendWritePageTimeout(USHORT PageTimeout)
{
	FBT_TRY

	fbtLog(fbtLog_Enter, _T("CHci::SendWritePageTimeout: Enter, PageTimeout=%d"), PageTimeout);

    FBT_HCI_WRITE_PAGE_TIMEOUT commandBuffer;

    commandBuffer.CommandHeader.OpCode = FBT_HCI_CMD_WRITE_PAGE_TIMEOUT;
    commandBuffer.CommandHeader.ParameterLength = sizeof(FBT_HCI_WRITE_PAGE_TIMEOUT) - sizeof(FBT_HCI_CMD_HEADER);

    commandBuffer.PageTimeout = PageTimeout;

    DWORD dwResult=SendHciCommand((PFBT_HCI_CMD_HEADER)&commandBuffer, sizeof(commandBuffer));

    fbtLog(fbtLog_Exit, _T("CHci::SendWritePageTimeout: Exit, dwResult=%d"), dwResult);

    return dwResult;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHci::SendRemoteNameRequest(BYTE BD_ADDR[FBT_HCI_BDADDR_SIZE], BYTE PageScanRepetitionMode, BYTE PageScanMode, USHORT ClockOffset)
{
	FBT_TRY

    TCHAR szAddress[40];
    _stprintf(szAddress, _T("%02X %02X %02X %02X %02X %02X"),
                            BD_ADDR[5],
                            BD_ADDR[4],
                            BD_ADDR[3],
                            BD_ADDR[2],
                            BD_ADDR[1],
                            BD_ADDR[0]);

    fbtLog(fbtLog_Enter, _T("CHci::SendRemoteNameRequest: Enter(Address=%s, PageScanRepetitionMode=%d, PageScanMode=%d, ClockOffset=%d)"), szAddress, PageScanRepetitionMode, PageScanMode, ClockOffset);

    FBT_HCI_REMOTE_NAME_REQUEST commandBuffer;

    commandBuffer.CommandHeader.OpCode = FBT_HCI_CMD_REMOTE_NAME_REQUEST;
    commandBuffer.CommandHeader.ParameterLength = sizeof(FBT_HCI_REMOTE_NAME_REQUEST) - sizeof(FBT_HCI_CMD_HEADER);

    CopyMemory(commandBuffer.BD_ADDR, BD_ADDR, FBT_HCI_BDADDR_SIZE);

    commandBuffer.PageScanRepetitionMode=PageScanRepetitionMode;
    commandBuffer.PageScanMode=PageScanMode;
    commandBuffer.ClockOffset=ClockOffset;

    DWORD dwReturnCode=SendHciCommand((PFBT_HCI_CMD_HEADER)&commandBuffer, sizeof(commandBuffer));

    fbtLog(fbtLog_Exit, _T("CHci::SendRemoteNameRequest: Exit"));

    return dwReturnCode;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHci::SendReadLocalName()
{
	FBT_TRY

    FBT_HCI_READ_LOCAL_NAME commandBuffer;

    commandBuffer.CommandHeader.OpCode = FBT_HCI_CMD_READ_LOCAL_NAME;
    commandBuffer.CommandHeader.ParameterLength = sizeof(FBT_HCI_READ_LOCAL_NAME) - sizeof(FBT_HCI_CMD_HEADER);

    SendHciCommand((PFBT_HCI_CMD_HEADER)&commandBuffer, sizeof(commandBuffer));

    return ERROR_SUCCESS;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHci::SendChangeLocalName(BYTE Name[FBT_HCI_NAME_SIZE])
{
	FBT_TRY

    FBT_HCI_CHANGE_LOCAL_NAME commandBuffer;

    commandBuffer.CommandHeader.OpCode = FBT_HCI_CMD_CHANGE_LOCAL_NAME;
    commandBuffer.CommandHeader.ParameterLength = sizeof(FBT_HCI_CHANGE_LOCAL_NAME) - sizeof(FBT_HCI_CMD_HEADER);

    CopyMemory(commandBuffer.Name, Name, FBT_HCI_NAME_SIZE);

    return SendHciCommand((PFBT_HCI_CMD_HEADER)&commandBuffer, sizeof(commandBuffer));

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHci::SendSwitchRole(BYTE BD_ADDR[FBT_HCI_BDADDR_SIZE], BYTE Role)
{
	FBT_TRY

    TCHAR szAddress[40];
    _stprintf(szAddress, _T("%02X %02X %02X %02X %02X %02X"),
                            BD_ADDR[5],
                            BD_ADDR[4],
                            BD_ADDR[3],
                            BD_ADDR[2],
                            BD_ADDR[1],
                            BD_ADDR[0]);

    fbtLog(fbtLog_Enter, _T("CHci::SendSwitchRole: Enter(BD_ADDR=%S, Role=%d"), szAddress, Role);

    FBT_HCI_SWITCH_ROLE commandBuffer;

    commandBuffer.CommandHeader.OpCode = FBT_HCI_CMD_SWITCH_ROLE;
    commandBuffer.CommandHeader.ParameterLength = sizeof(FBT_HCI_SWITCH_ROLE) - sizeof(FBT_HCI_CMD_HEADER);

    CopyMemory(commandBuffer.BD_ADDR, BD_ADDR, FBT_HCI_BDADDR_SIZE);

    commandBuffer.Role=Role;

    return SendHciCommand((PFBT_HCI_CMD_HEADER)&commandBuffer, sizeof(commandBuffer));

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHci::SendPINCodeRequestReply(BYTE BD_ADDR[FBT_HCI_BDADDR_SIZE], BYTE PINCodeLength, BYTE PINCode[FBT_HCI_PIN_CODE_SIZE])
{
	FBT_TRY

    TCHAR szAddress[40];
    _stprintf(szAddress, _T("%02X %02X %02X %02X %02X %02X"),
                            BD_ADDR[5],
                            BD_ADDR[4],
                            BD_ADDR[3],
                            BD_ADDR[2],
                            BD_ADDR[1],
                            BD_ADDR[0]);

    fbtLog(fbtLog_Enter, _T("CHci::SendPINCodeRequestReply: Enter(BD_ADDR=%S, PINCodeLength=%d"), szAddress, PINCodeLength);

    FBT_HCI_PIN_CODE_REQUEST_REPLY commandBuffer;
    commandBuffer.CommandHeader.OpCode = FBT_HCI_CMD_PIN_CODE_REQUEST_REPLY;
    commandBuffer.CommandHeader.ParameterLength = sizeof(FBT_HCI_PIN_CODE_REQUEST_REPLY) - sizeof(FBT_HCI_CMD_HEADER);

    CopyMemory(commandBuffer.BD_ADDR, BD_ADDR, FBT_HCI_BDADDR_SIZE);

    commandBuffer.PINCodeLength=PINCodeLength;

    CopyMemory(commandBuffer.PINCode, PINCode, FBT_HCI_PIN_CODE_SIZE);

    return SendHciCommand((PFBT_HCI_CMD_HEADER)&commandBuffer, sizeof(commandBuffer));

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHci::SendPINCodeRequestNegativeReply(BYTE BD_ADDR[FBT_HCI_BDADDR_SIZE])
{
	FBT_TRY

    TCHAR szAddress[40];
    _stprintf(szAddress, _T("%02X %02X %02X %02X %02X %02X"),
                            BD_ADDR[5],
                            BD_ADDR[4],
                            BD_ADDR[3],
                            BD_ADDR[2],
                            BD_ADDR[1],
                            BD_ADDR[0]);

    fbtLog(fbtLog_Enter, _T("CHci::SendPINCodeRequestNegativeReply: Enter(BD_ADDR=%S"), szAddress);

    FBT_HCI_PIN_CODE_REQUEST_NEGATIVE_REPLY commandBuffer;
    commandBuffer.CommandHeader.OpCode = FBT_HCI_CMD_PIN_CODE_REQUEST_NEGATIVE_REPLY;
    commandBuffer.CommandHeader.ParameterLength = sizeof(FBT_HCI_PIN_CODE_REQUEST_NEGATIVE_REPLY) - sizeof(FBT_HCI_CMD_HEADER);

    CopyMemory(commandBuffer.BD_ADDR, BD_ADDR, FBT_HCI_BDADDR_SIZE);

    return SendHciCommand((PFBT_HCI_CMD_HEADER)&commandBuffer, sizeof(commandBuffer));

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHci::SendLinkKeyRequestReply(BYTE BD_ADDR[FBT_HCI_BDADDR_SIZE], BYTE LinkKey[FBT_HCI_LINK_KEY_SIZE])
{
	FBT_TRY

    TCHAR szAddress[40];
    _stprintf(szAddress, _T("%02X %02X %02X %02X %02X %02X"),
                            BD_ADDR[5],
                            BD_ADDR[4],
                            BD_ADDR[3],
                            BD_ADDR[2],
                            BD_ADDR[1],
                            BD_ADDR[0]);

    fbtLog(fbtLog_Enter, _T("CHci::SendLinkKeyRequestReply: Enter(BD_ADDR=%S"), szAddress);

    FBT_HCI_LINK_KEY_REQUEST_REPLY commandBuffer;
    commandBuffer.CommandHeader.OpCode = FBT_HCI_CMD_LINK_KEY_REQUEST_REPLY;
    commandBuffer.CommandHeader.ParameterLength = sizeof(FBT_HCI_LINK_KEY_REQUEST_REPLY) - sizeof(FBT_HCI_CMD_HEADER);

    CopyMemory(commandBuffer.BD_ADDR, BD_ADDR, FBT_HCI_BDADDR_SIZE);
    CopyMemory(commandBuffer.LinkKey, LinkKey, FBT_HCI_LINK_KEY_SIZE);

    return SendHciCommand((PFBT_HCI_CMD_HEADER)&commandBuffer, sizeof(commandBuffer));

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHci::SendLinkKeyRequestNegativeReply(BYTE BD_ADDR[FBT_HCI_BDADDR_SIZE])
{
	FBT_TRY

    TCHAR szAddress[40];
    _stprintf(szAddress, _T("%02X %02X %02X %02X %02X %02X"),
                            BD_ADDR[5],
                            BD_ADDR[4],
                            BD_ADDR[3],
                            BD_ADDR[2],
                            BD_ADDR[1],
                            BD_ADDR[0]);

    fbtLog(fbtLog_Enter, _T("CHci::SendLinkKeyRequestNegativeReply: Enter(BD_ADDR=%S"), szAddress);

    FBT_HCI_LINK_KEY_REQUEST_NEGATIVE_REPLY commandBuffer;
    commandBuffer.CommandHeader.OpCode = FBT_HCI_CMD_LINK_KEY_REQUEST_NEGATIVE_REPLY;
    commandBuffer.CommandHeader.ParameterLength = sizeof(FBT_HCI_LINK_KEY_REQUEST_NEGATIVE_REPLY) - sizeof(FBT_HCI_CMD_HEADER);

    CopyMemory(commandBuffer.BD_ADDR, BD_ADDR, FBT_HCI_BDADDR_SIZE);

    return SendHciCommand((PFBT_HCI_CMD_HEADER)&commandBuffer, sizeof(commandBuffer));

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHci::SendReadLocalVersionInformation()
{
	FBT_TRY

    fbtLog(fbtLog_Enter, _T("CHci::SendReadLocalVersionInformation: Enter"));

	FBT_HCI_READ_LOCAL_VERSION_INFORMATION commandBuffer;
    commandBuffer.CommandHeader.OpCode = FBT_HCI_CMD_READ_LOCAL_VERSION_INFORMATION;
    commandBuffer.CommandHeader.ParameterLength = 0;

    return SendHciCommand((PFBT_HCI_CMD_HEADER)&commandBuffer, sizeof(commandBuffer));

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

// Events
DWORD CHci::OnEvent(PFBT_HCI_EVENT_HEADER pEvent, DWORD Length)
{
    FBT_TRY

    DWORD returnCode=0;

    fbtLog(fbtLog_Notice, _T("CHci::OnEvent: Received event 0x%02X"), pEvent->EventCode);

    switch (pEvent->EventCode)
    {
        case FBT_HCI_EVENT_INQUIRY_COMPLETE:
            returnCode = OnInquiryComplete(
                            ((PFBT_HCI_INQUIRY_COMPLETE)pEvent)->Status,
                            ((PFBT_HCI_INQUIRY_COMPLETE)pEvent)->NumResponses);
            break;

        case FBT_HCI_EVENT_INQUIRY_RESULT:
        {
            PFBT_HCI_INQUIRY_RESULT pInquiryResult=(PFBT_HCI_INQUIRY_RESULT)pEvent;
            returnCode = OnInquiryResult(
                            pInquiryResult->NumResponses,
                            pInquiryResult->BD_ADDR,
                            pInquiryResult->PageScanRepetitionMode,
                            pInquiryResult->PageScanPeriodMode,
                            pInquiryResult->PageScanMode,
                            pInquiryResult->ClassOfDevice,
                            pInquiryResult->ClockOffset);
        }
        break;

        case FBT_HCI_EVENT_COMMAND_COMPLETE:
            returnCode = OnCommandComplete(
                        ((PFBT_HCI_COMMAND_COMPLETE)pEvent)->NumHCICommandPackets,
                        ((PFBT_HCI_COMMAND_COMPLETE)pEvent)->OpCode,
                        ((PFBT_HCI_COMMAND_COMPLETE)pEvent)->Parameters,
                        ((PFBT_HCI_COMMAND_COMPLETE)pEvent)->EventHeader.ParameterLength - (sizeof(PFBT_HCI_COMMAND_COMPLETE) - sizeof(FBT_HCI_EVENT_HEADER) - 1));
            break;

        case FBT_HCI_EVENT_COMMAND_STATUS:
            returnCode = OnCommandStatus(
                            ((PFBT_HCI_COMMAND_STATUS)pEvent)->Status,
                            ((PFBT_HCI_COMMAND_STATUS)pEvent)->NumHCICommandPackets,
                            ((PFBT_HCI_COMMAND_STATUS)pEvent)->OpCode);
            break;

        case FBT_HCI_EVENT_CONNECTION_REQUEST:
            returnCode = OnConnectionRequest(
                            ((PFBT_HCI_CONNECTION_REQUEST)pEvent)->BD_ADDR,
                            ((PFBT_HCI_CONNECTION_REQUEST)pEvent)->ClassOfDevice,
                            ((PFBT_HCI_CONNECTION_REQUEST)pEvent)->LinkType);
            break;

        case FBT_HCI_EVENT_CONNECTION_COMPLETE:
            returnCode = OnConnectionComplete(
                            ((PFBT_HCI_CONNECTION_COMPLETE)pEvent)->Status,
                            ((PFBT_HCI_CONNECTION_COMPLETE)pEvent)->ConnectionHandle,
                            ((PFBT_HCI_CONNECTION_COMPLETE)pEvent)->BD_ADDR,
                            ((PFBT_HCI_CONNECTION_COMPLETE)pEvent)->LinkType,
                            ((PFBT_HCI_CONNECTION_COMPLETE)pEvent)->EncryptionMode);
            break;

        case FBT_HCI_EVENT_DISCONNECTION_COMPLETE:
            returnCode = OnDisconnectionComplete(
                            ((PFBT_HCI_DISCONNECTION_COMPLETE)pEvent)->Status,
                            ((PFBT_HCI_DISCONNECTION_COMPLETE)pEvent)->ConnectionHandle,
                            ((PFBT_HCI_DISCONNECTION_COMPLETE)pEvent)->Reason);
            break;

        case FBT_HCI_EVENT_REMOTE_NAME_REQUEST_COMPLETE:
            returnCode=OnRemoteNameRequestComplete(
                        ((PFBT_HCI_REMOTE_NAME_REQUEST_COMPLETE)pEvent)->Status,
                        ((PFBT_HCI_REMOTE_NAME_REQUEST_COMPLETE)pEvent)->BD_ADDR,
                        ((PFBT_HCI_REMOTE_NAME_REQUEST_COMPLETE)pEvent)->RemoteName);
            break;

        case FBT_HCI_EVENT_ROLE_CHANGE:
            returnCode=OnRoleChange(
                        ((PFBT_HCI_ROLE_CHANGE)pEvent)->Status,
                        ((PFBT_HCI_ROLE_CHANGE)pEvent)->BD_ADDR,
                        ((PFBT_HCI_ROLE_CHANGE)pEvent)->NewRole);

            break;

        case FBT_HCI_EVENT_PIN_CODE_REQUEST:
            returnCode=OnPINCodeRequest(((PFBT_HCI_PIN_CODE_REQUEST)pEvent)->BD_ADDR);
            break;

        case FBT_HCI_EVENT_LINK_KEY_NOTIFICATION:
            returnCode=OnLinkKeyNotification(
                        ((PFBT_HCI_LINK_KEY_NOTIFICATION)pEvent)->BD_ADDR,
                        ((PFBT_HCI_LINK_KEY_NOTIFICATION)pEvent)->LinkKey);

            break;

        case FBT_HCI_EVENT_AUTHENTICATION_COMPLETE:
            returnCode=OnAuthenticationComplete(
                        ((PFBT_HCI_AUTHENTICATION_COMPLETE)pEvent)->Status,
                        ((PFBT_HCI_AUTHENTICATION_COMPLETE)pEvent)->ConnectionHandle);

            break;

        case FBT_HCI_EVENT_LINK_KEY_REQUEST:
            returnCode=OnLinkKeyRequest(((PFBT_HCI_LINK_KEY_REQUEST)pEvent)->BD_ADDR);
            break;

        default:
            fbtLog(fbtLog_Notice, _T("CHci::OnEvent: Event 0x%02X unhandled"), pEvent->EventCode);
            returnCode = OnUnknown(pEvent, Length);
            break;

    }

    return returnCode;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHci::OnCommandComplete(BYTE NumHCICommandPackets, USHORT CommandOpcode, BYTE *Parameters, DWORD ParameterLength)
{
	FBT_TRY

    fbtLog(fbtLog_Notice, _T("CHci::OnCommandComplete: Command Complete Event:"));
    fbtLog(fbtLog_Notice, _T("CHci::OnCommandComplete: NumHCICommandPackets: 0x%02X"), NumHCICommandPackets);
    fbtLog(fbtLog_Notice, _T("CHci::OnCommandComplete: CommandOpcode: 0x%02X"), CommandOpcode);
    fbtLog(fbtLog_Notice, _T("CHci::OnCommandComplete: Parameters: "));
    ULONG fixedParameterLength = (sizeof(FBT_HCI_COMMAND_COMPLETE) - sizeof(FBT_HCI_EVENT_HEADER) - 1 /* variable sized array */);
    for (ULONG ii = 0; ii < ParameterLength; ii++)
        fbtLog(fbtLog_Notice, _T("%02X "), Parameters[ii]);

    if (CommandOpcode==FBT_HCI_CMD_READ_LOCAL_NAME)
        OnReadLocalNameComplete(Parameters[0], &Parameters[1]);

    return ERROR_SUCCESS;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}


DWORD CHci::OnCommandStatus(BYTE Status, BYTE NumHCICommandPackets, USHORT CommandOpcode)
{
    FBT_TRY

    fbtLog(fbtLog_Notice, _T("CHci::OnCommandStatus: Command Status Event:"));
    fbtLog(fbtLog_Notice, _T("CHci::OnCommandStatus: Status: 0x%02X (%s)"), Status, GetStatusText(Status));
    fbtLog(fbtLog_Notice, _T("CHci::OnCommandStatus: NumHCICommandPackets: 0x%02X"), NumHCICommandPackets);
    fbtLog(fbtLog_Notice, _T("CHci::OnCommandStatus: CommandOpcode: 0x%02X"), CommandOpcode);

    return ERROR_SUCCESS;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHci::OnInquiryResult(BYTE NumResponses, BYTE BD_ADDR[FBT_HCI_VARIABLE_SIZE][FBT_HCI_BDADDR_SIZE], BYTE PageScanRepetitionMode[FBT_HCI_VARIABLE_SIZE], BYTE PageScanPeriodMode[FBT_HCI_VARIABLE_SIZE], BYTE PageScanMode[FBT_HCI_VARIABLE_SIZE], BYTE ClassOfDevice[FBT_HCI_VARIABLE_SIZE][FBT_HCI_DEVICE_CLASS_SIZE], USHORT ClockOffset[FBT_HCI_VARIABLE_SIZE])
{
    FBT_TRY

    fbtLog(fbtLog_Notice, _T("CHci::OnInquiryResult: Inquiry Result Event:"));
    fbtLog(fbtLog_Notice, _T("CHci::OnInquiryResult: Responses: 0x%02X"), NumResponses);
    if (NumResponses==0)
        return ERROR_SUCCESS;

    for (int i=0; i<NumResponses; i++)
    {
        fbtLog(fbtLog_Notice, _T("CHci::OnInquiryResult: Response %02d:"), i+1);
        fbtLog(fbtLog_Notice, _T("CHci::OnInquiryResult: Addresses:"));
        fbtLog(fbtLog_Notice, _T("CHci::OnInquiryResult: BD_ADDR: 0x%02X"), &(BD_ADDR[i]));
        fbtLog(fbtLog_Notice, _T("CHci::OnInquiryResult: PageScanRepetitionMode: 0x%02X"), &(PageScanRepetitionMode[i]));
        fbtLog(fbtLog_Notice, _T("CHci::OnInquiryResult: PageScanPeriodMode: 0x%02X"), &(PageScanPeriodMode[i]));
        fbtLog(fbtLog_Notice, _T("CHci::OnInquiryResult: PageScanMode: 0x%02X"), &(PageScanMode[i]));
        fbtLog(fbtLog_Notice, _T("CHci::OnInquiryResult: ClassOfDevice: 0x%02X"), &(ClassOfDevice[i]));
        fbtLog(fbtLog_Notice, _T("CHci::OnInquiryResult: ClockOffset: 0x%02X"), &(ClockOffset[i]));
        fbtLog(fbtLog_Notice, _T("CHci::OnInquiryResult: BD_ADDR: %02X %02X %02X %02X %02X %02X"),
            BD_ADDR[i][5],
            BD_ADDR[i][4],
            BD_ADDR[i][3],
            BD_ADDR[i][2],
            BD_ADDR[i][1],
            BD_ADDR[i][0]);

        fbtLog(fbtLog_Notice, _T("CHci::OnInquiryResult: PageScanRepetitionMode: 0x%02X"), PageScanRepetitionMode[i]);
        fbtLog(fbtLog_Notice, _T("CHci::OnInquiryResult: PageScanPeriodMode: 0x%02X"), PageScanPeriodMode[i]);
        fbtLog(fbtLog_Notice, _T("CHci::OnInquiryResult: PageScanMode: 0x%02X"), PageScanMode[i]);
        fbtLog(fbtLog_Notice, _T("CHci::OnInquiryResult: ClassOfDevice: %02X %02X %02X"), ClassOfDevice[i][0], ClassOfDevice[i][1], ClassOfDevice[i][2]);
        USHORT TempClockOffset;

        CopyMemory(&TempClockOffset, &(ClockOffset[i]), sizeof(TempClockOffset));
        fbtLog(fbtLog_Notice, _T("CHci::OnInquiryResult: ClockOffset: 0x%02X"), TempClockOffset);
        fbtLog(fbtLog_Notice, _T(""));

    }

    return ERROR_SUCCESS;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHci::OnInquiryComplete(BYTE Status, BYTE NumResponses)
{
    FBT_TRY

    fbtLog(fbtLog_Notice, _T("CHci::OnInquiryComplete: Inquiry Complete Event:"));
    fbtLog(fbtLog_Notice, _T("CHci::OnInquiryComplete: Status: 0x%02X (%s)"), Status, GetStatusText(Status));
    fbtLog(fbtLog_Notice, _T("CHci::OnInquiryComplete: NumResponses: 0x%02X"), NumResponses);
    fbtLog(fbtLog_Notice, _T(""));

    return ERROR_SUCCESS;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHci::OnConnectionRequest(BYTE BD_ADDR[FBT_HCI_BDADDR_SIZE], ULONG ClassOfDevice[FBT_HCI_DEVICE_CLASS_SIZE], BYTE  LinkType)
{
	FBT_TRY

    fbtLog(fbtLog_Notice, _T("CHci::OnConnectionRequest: Connection Request Event:"));
    fbtLog(fbtLog_Notice, _T("CHci::OnConnectionRequest: BD_ADDR: %02X %02X %02X %02X %02X %02X"),
        BD_ADDR[5],
        BD_ADDR[4],
        BD_ADDR[3],
        BD_ADDR[2],
        BD_ADDR[1],
        BD_ADDR[0]);

    fbtLog(fbtLog_Notice, _T("CHci::OnConnectionRequest: ClassOfDevice: 0x%02X"), ClassOfDevice);
    fbtLog(fbtLog_Notice, _T("CHci::OnConnectionRequest: LinkType: 0x%02X"), LinkType);

    return ERROR_SUCCESS;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHci::OnConnectionComplete(BYTE Status, USHORT ConnectionHandle, BYTE BD_ADDR[FBT_HCI_BDADDR_SIZE], BYTE LinkType, BYTE EncryptionMode)
{
	FBT_TRY

    // it was a create connection, see if we timed out
    if (Status!=0)
    {
        fbtLog(fbtLog_Notice, _T("CHci::OnConnectionComplete: Failed, status %d (%s)"), Status, GetStatusText(Status));
        return Status;

    }

    fbtLog(fbtLog_Notice, _T("CHci::OnConnectionComplete: Connection Complete Event:"));
    fbtLog(fbtLog_Notice, _T("CHci::OnConnectionComplete: Status: 0x%02X %s"), Status, GetStatusText(Status));
    fbtLog(fbtLog_Notice, _T("CHci::OnConnectionComplete: Connection Handle: 0x%04X"), ConnectionHandle);
    fbtLog(fbtLog_Notice, _T("CHci::OnConnectionComplete: BD_ADDR: %02X-%02X-%02X-%02X-%02X-%02X"),
                                    BD_ADDR[5],
                                    BD_ADDR[4],
                                    BD_ADDR[3],
                                    BD_ADDR[2],
                                    BD_ADDR[1],
                                    BD_ADDR[0]);
    fbtLog(fbtLog_Notice, _T("CHci::OnConnectionComplete: Link Type: 0x%02X"), LinkType);
    fbtLog(fbtLog_Notice, _T("CHci::OnConnectionComplete: Encryption Mode: 0x%02X"), EncryptionMode);

    return ERROR_SUCCESS;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHci::OnDisconnectionComplete(BYTE Status, USHORT ConnectionHandle, BYTE Reason)
{
	FBT_TRY

    fbtLog(fbtLog_Notice, _T("CHci::OnDisconnectionComplete: Disconnection Complete Event:"));
    fbtLog(fbtLog_Notice, _T("CHci::OnDisconnectionComplete: Status: 0x%02X (%s)"), Status, GetStatusText(Status));
    fbtLog(fbtLog_Notice, _T("CHci::OnDisconnectionComplete: Connection Handle: 0x%04X"), ConnectionHandle);
    fbtLog(fbtLog_Notice, _T("CHci::OnDisconnectionComplete: Reason: 0x%02X (%s)"), Reason, GetStatusText(Reason));

    return ERROR_SUCCESS;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHci::OnRemoteNameRequestComplete(BYTE Status, BYTE BD_ADDR[FBT_HCI_BDADDR_SIZE], BYTE RemoteName[FBT_HCI_NAME_SIZE])
{
    FBT_TRY

    fbtLog(fbtLog_Notice, _T("CHci::OnRemoteNameRequestComplete: Remote Name Request Complete Event:"));
    fbtLog(fbtLog_Notice, _T("CHci::OnRemoteNameRequestComplete: Status: %02X (%s)"), Status, GetStatusText(Status));
    if (Status==0)
    {
        fbtLog(fbtLog_Notice, _T("CHci::OnRemoteNameRequestComplete: BD_ADDR: %02X %02X %02X %02X %02X %02X"),
                BD_ADDR[5],
                BD_ADDR[4],
                BD_ADDR[3],
                BD_ADDR[2],
                BD_ADDR[1],
                BD_ADDR[0]);

        fbtLog(fbtLog_Notice, _T("CHci::OnRemoteNameRequestComplete: Name: %s"), RemoteName);

    }

    return ERROR_SUCCESS;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHci::OnRoleChange(BYTE Status, BYTE BD_ADDR[FBT_HCI_BDADDR_SIZE], BYTE Role)
{
    FBT_TRY

    fbtLog(fbtLog_Notice, _T("CHci::OnRoleChange: Roll Change Event:"));
    fbtLog(fbtLog_Notice, _T("CHci::OnRoleChange: Status: 0x%02X"), Status);
    fbtLog(fbtLog_Notice, _T("CHci::OnRoleChange: BD_ADDR: %02X %02X %02X %02X %02X %02X"),
            BD_ADDR[5],
            BD_ADDR[4],
            BD_ADDR[3],
            BD_ADDR[2],
            BD_ADDR[1],
            BD_ADDR[0]);

    fbtLog(fbtLog_Notice, _T("CHci::OnRoleChange: Role: 0x%02X"), Role);

    return ERROR_SUCCESS;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHci::OnPINCodeRequest(BYTE BD_ADDR[FBT_HCI_BDADDR_SIZE])
{
    FBT_TRY

    fbtLog(fbtLog_Notice, _T("CHci::OnPINCodeRequest: PIN Code Request Event:"));
    fbtLog(fbtLog_Notice, _T("CHci::OnPINCodeRequest: BD_ADDR: %02X %02X %02X %02X %02X %02X"),
            BD_ADDR[5],
            BD_ADDR[4],
            BD_ADDR[3],
            BD_ADDR[2],
            BD_ADDR[1],
            BD_ADDR[0]);

    return ERROR_SUCCESS;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHci::OnLinkKeyNotification(BYTE BD_ADDR[FBT_HCI_BDADDR_SIZE], BYTE LinkKey[FBT_HCI_LINK_KEY_SIZE])
{
    FBT_TRY

    fbtLog(fbtLog_Notice, _T("CHci::OnLinkKeyNotification: Link Key Notification Event:"));
    TCHAR szAddress[32];
    _stprintf(szAddress, _T("%02X %02X %02X %02X %02X %02X"),
            BD_ADDR[5],
            BD_ADDR[4],
            BD_ADDR[3],
            BD_ADDR[2],
            BD_ADDR[1],
            BD_ADDR[0]);

    fbtLog(fbtLog_Notice, _T("CHci::OnLinkKeyNotification: BD_ADDR: %s"), szAddress);

	if (fbtLogGetLevel()>=fbtLog_Verbose)
	{
		fbtLog(fbtLog_Verbose, _T("\tLink Key: "));
		for (UINT i=0; i<FBT_HCI_LINK_KEY_SIZE; i++)
			fbtLog(fbtLog_Verbose, _T("%02X "), ((PBYTE)LinkKey)[i]);

		fbtLog(fbtLog_Verbose, _T("\n"));

	}

    return ERROR_SUCCESS;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHci::OnLinkKeyRequest(BYTE BD_ADDR[FBT_HCI_BDADDR_SIZE])
{
    FBT_TRY

    fbtLog(fbtLog_Notice, _T("CHci::OnLinkKeyRequest: Link Key Request Event:"));
    TCHAR szAddress[32];
    _stprintf(szAddress, _T("%02X %02X %02X %02X %02X %02X"),
            BD_ADDR[5],
            BD_ADDR[4],
            BD_ADDR[3],
            BD_ADDR[2],
            BD_ADDR[1],
            BD_ADDR[0]);

    fbtLog(fbtLog_Notice, _T("CHci::OnLinkKeyRequest: BD_ADDR: %s"), szAddress);

    return ERROR_SUCCESS;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHci::OnAuthenticationComplete(BYTE Status, USHORT ConnectionHandle)
{
    FBT_TRY

    fbtLog(fbtLog_Notice, _T("CHci::OnAuthenticationComplete: Authentication Complete Event"));
    fbtLog(fbtLog_Notice, _T("CHci::OnAuthenticationComplete: Status: 0x%0x (%s)"), Status, GetStatusText(Status));
    fbtLog(fbtLog_Notice, _T("CHci::OnAuthenticationComplete: ConnectionHandle: 0x%02x"), ConnectionHandle);

    return ERROR_SUCCESS;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHci::OnReadLocalNameComplete(BYTE Status, BYTE Name[FBT_HCI_NAME_SIZE])
{
    FBT_TRY

    fbtLog(fbtLog_Notice, _T("CHci::OnReadLocalNameComplete: Read Local Name Complete Event"));
    fbtLog(fbtLog_Notice, _T("CHci::OnReadLocalNameComplete: Status: 0x%0x (%s)"), Status, GetStatusText(Status));
    fbtLog(fbtLog_Notice, _T("CHci::OnReadLocalNameComplete: Name: %s"), Name);

    return ERROR_SUCCESS;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

// Handler for events that are not explicitly handled by the other On... event
// handlers
DWORD CHci::OnUnknown(PFBT_HCI_EVENT_HEADER pEvent, DWORD Length)
{
	FBT_TRY

    fbtLog(fbtLog_Notice, _T("CHci::OnUnknown: Unhandled event:"));
    fbtLog(fbtLog_Notice, _T("CHci::OnUnknown: EventCode: 0x%02X"), pEvent->EventCode);
    fbtLog(fbtLog_Notice, _T("CHci::OnUnknown: ParameterLength: 0x%02X"), pEvent->ParameterLength);

	if (fbtLogGetLevel()>fbtLog_Verbose)
	{
		TCHAR szLog[1024]={0};
		TCHAR szBuf[50]={0};

		_stprintf(szLog, _T("CHci::OnUnknown: Event data: "));
		for (DWORD i=0; i<(pEvent->ParameterLength + sizeof(FBT_HCI_EVENT_HEADER)); i++)
		{
			_stprintf(szBuf, _T("%02x "), ((PBYTE)pEvent)[i]);
			_tcscat(szLog, szBuf);

		}

		fbtLog(fbtLog_Verbose, _T("%s"), szLog);

	}

    return ERROR_SUCCESS;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}
