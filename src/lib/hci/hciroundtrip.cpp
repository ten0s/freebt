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
 * 2008-12-04 Dmitry Klionsky Removed ReadBDADDR function.
 * 2008-11-30 Dmitry Klionsky Added UNICODE support.
 * 2008-11-30 Dmitry Klionsky CBTHW class is no longer the base class. The CHciRoundTrip class receives a reference on CBTHW class at its construction.
 */

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <winioctl.h>
#include <tchar.h>

#include "fbtutil.h"
#include "fbtHciRoundTrip.h"

CHciRoundTrip::CHciRoundTrip( CBTHW& btHw ) : CHciLocal( btHw )
{
	fbtLog(fbtLog_Enter, _T("CHciRoundTrip::CHciRoundTrip: Enter"));

	// Used to signal the arrival of an event
	m_hEventSignal=CreateEvent(NULL, FALSE, FALSE, NULL);

	fbtLog(fbtLog_Exit, _T("CHciRoundTrip::CHciRoundTrip: Exit"));
}

CHciRoundTrip::~CHciRoundTrip()
{
	CloseHandle(m_hEventSignal);

}

DWORD CHciRoundTrip::OnEvent(PFBT_HCI_EVENT_HEADER pEvent, DWORD Length)
{
	FBT_TRY

	fbtLog(fbtLog_Enter, _T("CHciRoundTrip::OnEvent: Enter"));

	if (pEvent->EventCode==m_PendingEvent)
	{
		fbtLog(fbtLog_Failure, _T("CHciRoundTrip::OnEvent: Handling queued event 0x%02x"), m_PendingEvent);
		if (Length>m_dwEventParameterLength)
		{
			fbtLog(fbtLog_Failure, _T("CHciRoundTrip::OnEvent: Parameter buffer provided for event 0x%02x too short %d vs %d"), m_PendingEvent, m_dwEventParameterLength, Length);
			m_dwEventParameterLength=0;
			SetEvent(m_hEventSignal);
			return ERROR_INTERNAL_ERROR;

		}

		m_dwEventParameterLength=Length;
		if (Length>0)
			CopyMemory(m_pEventParameters, pEvent, Length);

		SetEvent(m_hEventSignal);

	}

	else
		CHciLocal::OnEvent(pEvent, Length);

	fbtLog(fbtLog_Enter, _T("CHciRoundTrip::OnEvent: Exit"));

    return ERROR_SUCCESS;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}


DWORD CHciRoundTrip::QueueEvent(BYTE EventCode, LPVOID pParameters, DWORD dwParameterLength)
{
	FBT_TRY

	ResetEvent(m_hEventSignal);

	m_PendingEvent=EventCode;
	m_pEventParameters=pParameters;
	m_dwEventParameterLength=dwParameterLength;

	return ERROR_SUCCESS;

	FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHciRoundTrip::WaitForEvent()
{
	FBT_TRY

	if (WaitForSingleObject(m_hEventSignal, INFINITE)!=WAIT_OBJECT_0)
	{
		DWORD dwLastError=GetLastError();
		fbtLog(fbtLog_Failure, _T("CHciRoundTrip::WaitForEvent: WaitForSingleObject failed, error %d"), dwLastError);
		return dwLastError;

	}

	return ERROR_SUCCESS;

	FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHciRoundTrip::ReadClassOfDevice(BYTE *ClassOfDevice)
{
	FBT_TRY

	fbtLog(fbtLog_Enter, _T("CHciRoundTrip::ReadClassOfDevice: Enter"));

    BYTE nStatus=0;
	int nSlot=QueueCommand(FBT_HCI_CMD_INQUIRY, &nStatus, sizeof(nStatus));

	DWORD dwResult=SendReadClassOfDevice();
	if (dwResult!=ERROR_SUCCESS)
	{
		fbtLog(fbtLog_Failure, _T("CHciRoundTrip::ReadClassOfDevice: SendReadClassOfDevice failed, error %d"), dwResult);
		DeQueueCommand(nSlot);
		return dwResult;

	}

	dwResult=WaitForCommandComplete(nSlot);
	if (dwResult!=ERROR_SUCCESS)
	{
		fbtLog(fbtLog_Failure, _T("CHciRoundTrip::ReadClassOfDevice: WaitForCommandComplete failed, error %d"), dwResult);
		DeQueueCommand(nSlot);
		return dwResult;

	}

	if (nStatus!=0)
	{
		fbtLog(fbtLog_Failure, _T("CHciRoundTrip::ReadClassOfDevice: HCI_Read_Class_of_Device failed, status %s (0x%02x)"), GetStatusText(nStatus), nStatus);
		DeQueueCommand(nSlot);
		return nStatus;

	}

	PQueuedCommand pQueuedCommand=GetQueuedCommand(nSlot);
	CopyMemory(ClassOfDevice, pQueuedCommand->pResultBuffer+1, FBT_HCI_DEVICE_CLASS_SIZE);

	DeQueueCommand(nSlot);

	return ERROR_SUCCESS;

	FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHciRoundTrip::CreateConnection(BYTE BD_ADDR[FBT_HCI_BDADDR_SIZE], USHORT PacketType, BYTE  PageScanRepetitionMode, BYTE  PageScanMode, USHORT ClockOffset, BYTE  AllowRoleSwitch, USHORT &ConnectionHandle)
{
	FBT_TRY

	fbtLog(fbtLog_Enter, _T("CHciRoundTrip::CreateConnection: Enter"));

	FBT_HCI_CONNECTION_COMPLETE ConnectionComplete;
	QueueEvent(FBT_HCI_EVENT_CONNECTION_COMPLETE, &ConnectionComplete, sizeof(ConnectionComplete));

	DWORD dwResult=SendCreateConnection(BD_ADDR, PacketType, PageScanRepetitionMode, PageScanMode, ClockOffset, AllowRoleSwitch);
	if (dwResult!=ERROR_SUCCESS)
	{
		fbtLog(fbtLog_Failure, _T("CHciRoundTrip::CreateConnection: SendCreateConnection failed, error %d"), dwResult);
		return dwResult;

	}

	dwResult=WaitForEvent();
	if (dwResult!=ERROR_SUCCESS)
	{
		fbtLog(fbtLog_Failure, _T("CHciRoundTrip::CreateConnection: WaitForEvent failed, error %d"), dwResult);
		return dwResult;

	}

	ConnectionHandle=ConnectionComplete.ConnectionHandle;

    fbtLog(fbtLog_Exit, _T("CHciRoundTrip::CreateConnection: Complete, status %s (0x%02x)"), GetStatusText(ConnectionComplete.Status), ConnectionComplete.Status);

	return ConnectionComplete.Status;

	FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHciRoundTrip::Disconnect(USHORT ConnectionHandle, BYTE Reason)
{
	FBT_TRY

	fbtLog(fbtLog_Enter, _T("CHciRoundTrip::Disconnect: Enter"));

	FBT_HCI_CONNECTION_COMPLETE DisconnectionComplete;
	QueueEvent(FBT_HCI_EVENT_DISCONNECTION_COMPLETE, &DisconnectionComplete, sizeof(DisconnectionComplete));

	DWORD dwResult=SendDisconnect(ConnectionHandle, Reason);
	if (dwResult!=ERROR_SUCCESS)
	{
		fbtLog(fbtLog_Failure, _T("CHciRoundTrip::Disconnect: SendCreateConnection failed, error %d"), dwResult);
		return dwResult;

	}

	dwResult=WaitForEvent();
	if (dwResult!=ERROR_SUCCESS)
	{
		fbtLog(fbtLog_Failure, _T("CHciRoundTrip::Disconnect: WaitForEvent failed, error %d"), dwResult);
		return dwResult;

	}

    fbtLog(fbtLog_Exit, _T("CHciRoundTrip::Disconnect: Complete, status %s (0x%02x)"), GetStatusText(DisconnectionComplete.Status), DisconnectionComplete.Status);

	return DisconnectionComplete.Status;

	FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHciRoundTrip::SwitchRole(BYTE BD_ADDR[FBT_HCI_BDADDR_SIZE], BYTE Role)
{
	FBT_TRY

	fbtLog(fbtLog_Enter, _T("CHciRoundTrip::SwitchRole: Enter"));

	FBT_HCI_ROLE_CHANGE RoleChange;
	QueueEvent(FBT_HCI_EVENT_ROLE_CHANGE, &RoleChange, sizeof(RoleChange));

	DWORD dwResult=SendSwitchRole(BD_ADDR, Role);
	if (dwResult!=ERROR_SUCCESS)
	{
		fbtLog(fbtLog_Failure, _T("CHciRoundTrip::SwitchRole: SendSwitchRole failed, error %d"), dwResult);
		return dwResult;

	}

	dwResult=WaitForEvent();
	if (dwResult!=ERROR_SUCCESS)
	{
		fbtLog(fbtLog_Failure, _T("CHciRoundTrip::SwitchRole: WaitForEvent failed, error %d"), dwResult);
		return dwResult;

	}

    fbtLog(fbtLog_Exit, _T("CHciRoundTrip::SwitchRole: Complete, status %s (0x%02x)"), GetStatusText(RoleChange.Status), RoleChange.Status);

	return RoleChange.Status;

	FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHciRoundTrip::ReadLocalName(BYTE *Name)
{
	FBT_TRY

	fbtLog(fbtLog_Enter, _T("CHciRoundTrip::ReadLocalName: Enter"));

	struct _Result
	{
		BYTE nStatus;
		BYTE LocalName[FBT_HCI_NAME_SIZE];

	} Result;

	int nSlot=QueueCommand(FBT_HCI_CMD_READ_LOCAL_NAME, (BYTE*)&Result, sizeof(Result));

	DWORD dwResult=SendReadLocalName();
	if (dwResult!=ERROR_SUCCESS)
	{
		fbtLog(fbtLog_Failure, _T("CHciRoundTrip::ReadLocalName: SendReadLocalName failed, error %d"), dwResult);
		DeQueueCommand(nSlot);
		return dwResult;

	}

	dwResult=WaitForCommandComplete(nSlot);
	if (dwResult!=ERROR_SUCCESS)
	{
		fbtLog(fbtLog_Failure, _T("CHciRoundTrip::ReadLocalName: WaitForCommandComplete failed, error %d"), dwResult);
		DeQueueCommand(nSlot);
		return dwResult;

	}

	if (Result.nStatus!=0)
	{
		fbtLog(fbtLog_Failure, _T("CHciRoundTrip::ReadLocalName: HCI_Read_Local_Name failed, status %s (0x%02x)"), GetStatusText(Result.nStatus), Result.nStatus);
		DeQueueCommand(nSlot);
		return Result.nStatus;

	}

	CopyMemory(Name, Result.LocalName, FBT_HCI_NAME_SIZE);

	DeQueueCommand(nSlot);

    fbtLog(fbtLog_Exit, _T("CHciRoundTrip::ReadLocalName: Name=%s"), Name);

	return ERROR_SUCCESS;

	FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHciRoundTrip::RemoteNameRequest(BYTE BD_ADDR[FBT_HCI_BDADDR_SIZE], BYTE PageScanRepetitionMode, BYTE PageScanMode, USHORT ClockOffset, BYTE Name[FBT_HCI_NAME_SIZE])
{
	FBT_TRY

	fbtLog(fbtLog_Enter, _T("CHciRoundTrip::RemoteNameRequest: Enter"));

	FBT_HCI_REMOTE_NAME_REQUEST_COMPLETE RemoteNameComplete;
	ZeroMemory(&RemoteNameComplete, sizeof(RemoteNameComplete));

	QueueEvent(FBT_HCI_EVENT_REMOTE_NAME_REQUEST_COMPLETE, &RemoteNameComplete, sizeof(RemoteNameComplete));

	DWORD dwResult=SendRemoteNameRequest(BD_ADDR, PageScanRepetitionMode, PageScanMode, ClockOffset);
	if (dwResult!=ERROR_SUCCESS)
	{
		fbtLog(fbtLog_Failure, _T("CHciRoundTrip::RemoteNameRequest: SendRemoteNameRequest failed, error %d"), dwResult);
		return dwResult;

	}

	fbtLog(fbtLog_Notice, _T("CHciRoundTrip::RemoteNameRequest: Command sent successfully"));

	dwResult=WaitForEvent();
	if (dwResult!=ERROR_SUCCESS)
	{
		fbtLog(fbtLog_Failure, _T("CHciRoundTrip::RemoteNameRequest: WaitForEvent failed"));
		return ERROR_INTERNAL_ERROR;

	}

	if (RemoteNameComplete.Status==0)
		CopyMemory(Name, RemoteNameComplete.RemoteName, FBT_HCI_NAME_SIZE);

    fbtLog(fbtLog_Exit, _T("CHciRoundTrip::RemoteNameRequest: Exit (Status 0x%02x)"), RemoteNameComplete.Status);

    return RemoteNameComplete.Status;

	FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}
