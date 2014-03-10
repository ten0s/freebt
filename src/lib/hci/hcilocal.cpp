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
 * 2008-12-04 Dmitry Klionsky Added SendReadBufferSize and SendReadBDADDR functions.
 * 2008-11-30 Dmitry Klionsky Added UNICODE support.
 * 2008-11-30 Dmitry Klionsky CBTHW class is no longer the base class. The CHciLocal class receives a reference on CBTHW class at its construction.
 */

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <winioctl.h>
#include <tchar.h>

#include "fbtutil.h"
#include "fbtHciLocal.h"


CHciLocal::CHciLocal( CBTHW& btHw ) : CHci( btHw )
{
    fbtLog(fbtLog_Enter, _T("CHciLocal::CHciLocal: Enter"));

	InitializeCriticalSection(&m_QueueCriticalSection);

	for (int i=0; i<MAX_QUEUED_COMMANDS; i++)
	{
		memset(&(m_QueuedCommands[i]), 0, sizeof(QueuedCommand));
		m_QueuedCommands[i].hEvent=CreateEvent(NULL, FALSE, FALSE, NULL);

	}

    fbtLog(fbtLog_Exit, _T("CHciLocal::CHciLocal: Exit"));

}

CHciLocal::~CHciLocal()
{
	FBT_TRY

	for (int i=0; i<MAX_QUEUED_COMMANDS; i++)
		CloseHandle(m_QueuedCommands[i].hEvent);

	FBT_CATCH_NORETURN

}

// Find a free slot for the command and queue it
int CHciLocal::QueueCommand(USHORT nCommand, BYTE *pResultBuffer, DWORD dwBufferSize)
{
    FBT_TRY

	// Stop any other threads from trampling over our
	// slot allocation
	EnterCriticalSection(&m_QueueCriticalSection);

	BOOL bFoundSlot=FALSE;
	int i=0;
	while (i<MAX_QUEUED_COMMANDS && !bFoundSlot)
	{
		if (m_QueuedCommands[i].nCommand==0)
		{
			m_QueuedCommands[i].nCommand=nCommand;
			bFoundSlot=TRUE;

		}

		else
			i++;

	}

	LeaveCriticalSection(&m_QueueCriticalSection);

	if (i==MAX_QUEUED_COMMANDS)
		return -1;

	ResetEvent(m_QueuedCommands[i].hEvent);
	m_QueuedCommands[i].pResultBuffer=pResultBuffer;
	m_QueuedCommands[i].dwBufferSize=dwBufferSize;
	fbtLog(fbtLog_Verbose, _T("CHciLocal::QueueCommand: Queue OGF=0x%02x OCF=0x%02x at slot %d"), FBT_HCI_OGF_FROM_COMMAND(nCommand), FBT_HCI_OCF_FROM_COMMAND(nCommand), i);

    return i;

    FBT_CATCH_RETURN(-1)

}

int CHciLocal::QueueCommandStatus(USHORT nCommand)
{
    FBT_TRY

	PFBT_HCI_COMMAND_STATUS pCommandStatus=(PFBT_HCI_COMMAND_STATUS)malloc(sizeof(FBT_HCI_COMMAND_STATUS));
	memset(pCommandStatus, 0, sizeof(FBT_HCI_COMMAND_STATUS));
	return QueueCommand(nCommand, (BYTE*)pCommandStatus, sizeof(FBT_HCI_COMMAND_STATUS));

    FBT_CATCH_RETURN(-1)

}

void CHciLocal::DeQueueCommand(int nSlot)
{
    FBT_TRY

	m_QueuedCommands[nSlot].nCommand=0;
	m_QueuedCommands[nSlot].pResultBuffer=NULL;
	m_QueuedCommands[nSlot].dwBufferSize=0;

	FBT_CATCH_NORETURN

}

PQueuedCommand CHciLocal::GetQueuedCommand(int nSlot)
{
	FBT_TRY

	if (nSlot>MAX_QUEUED_COMMANDS)
		return NULL;

	return &(m_QueuedCommands[nSlot]);

	FBT_CATCH_RETURN(NULL)

}

DWORD CHciLocal::ClearQueue()
{
    FBT_TRY

    return ERROR_SUCCESS;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHciLocal::WaitForCommandComplete(int nSlot)
{
    FBT_TRY

	PQueuedCommand pQueuedCommand=&(m_QueuedCommands[nSlot]);
	fbtLog(fbtLog_Notice, _T("CHciLocal::WaitForCommandComplete: Waiting for completion of command OGF=0x%02x OCF=0x%02x"), FBT_HCI_OGF_FROM_COMMAND(pQueuedCommand->nCommand), FBT_HCI_OCF_FROM_COMMAND(pQueuedCommand->nCommand));
    DWORD dwResult=WaitForSingleObject(pQueuedCommand->hEvent, INFINITE);
    if (dwResult!=WAIT_OBJECT_0)
    {
        DWORD dwLastError=GetLastError();
        fbtLog(fbtLog_Failure, _T("CHciLocal::WaitForCommandComplete: WaitForSingleObject failed, error %d"), dwLastError);
        return dwLastError;

    }

    return ERROR_SUCCESS;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHciLocal::WaitForCommandStatus(int nSlot, BYTE &nStatus)
{
    FBT_TRY

	PQueuedCommand pQueuedCommand=&(m_QueuedCommands[nSlot]);
	fbtLog(fbtLog_Notice, _T("CHciLocal::WaitForCommandStatus: Waiting for command status of command OGF=0x%02x OCF=0x%02x, at slot %d"), FBT_HCI_OGF_FROM_COMMAND(pQueuedCommand->nCommand), FBT_HCI_OCF_FROM_COMMAND(pQueuedCommand->nCommand), nSlot);
	DWORD dwResult=WaitForSingleObject(pQueuedCommand->hEvent, INFINITE);
    if (dwResult!=WAIT_OBJECT_0)
    {
        DWORD dwLastError=GetLastError();
        fbtLog(fbtLog_Failure, _T("CHciLocal::WaitForCommandStatus: WaitForSingleObject failed, error %d"), dwLastError);
        return dwLastError;

    }

	PFBT_HCI_COMMAND_STATUS pCommandStatus=(PFBT_HCI_COMMAND_STATUS)pQueuedCommand->pResultBuffer;
	nStatus=pCommandStatus->Status;
	free(pCommandStatus);

    return ERROR_SUCCESS;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHciLocal::SendSetEventFilter(BYTE FilterType, BYTE FilterConditionType, BYTE Condition[FBT_HCI_MAX_CONDITION_SIZE], BYTE ConditionBytes)
{
    FBT_TRY

	fbtLog(fbtLog_Enter, _T("Entered CHciLocal::SendEventFilter"));

    BYTE Status=0;
	int nSlot=QueueCommand(FBT_HCI_CMD_SET_EVENT_FILTER, &Status, sizeof(Status));

    DWORD dwResult=CHci::SendSetEventFilter(FilterType, FilterConditionType, Condition, ConditionBytes);
    if (dwResult!=ERROR_SUCCESS)
    {
        fbtLog(fbtLog_Failure, _T("CHciLocal::SendSetEventFilter: CHci::SendSetEventFilter Failed, error %d"), dwResult);
		DeQueueCommand(nSlot);
        return dwResult;

    }

    dwResult=WaitForCommandComplete(nSlot);
    if (dwResult!=ERROR_SUCCESS)
    {
        fbtLog(fbtLog_Failure, _T("CHciLocal::SendSetEventFilter: WaitForCommandComplete failed, error %d"), dwResult);
		DeQueueCommand(nSlot);
        return dwResult;

    }

    fbtLog(fbtLog_Notice, _T("CHciLocal::SendSetEventFilter: Complete, Status %x (%s)"), Status, GetStatusText(Status));

	DeQueueCommand(nSlot);

    return Status;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHciLocal::SendInquiry(ULONG LAP, BYTE InquiryLength, BYTE NumResponses)
{
    FBT_TRY

	int nSlot=QueueCommandStatus(FBT_HCI_CMD_INQUIRY);

    DWORD dwResult=CHci::SendInquiry(LAP, InquiryLength, NumResponses);
    if (dwResult!=ERROR_SUCCESS)
    {
        fbtLog(fbtLog_Failure, _T("CHciLocal::SendInquiry: CHci::SendInquiry Failed, error %d"), dwResult);
		DeQueueCommand(nSlot);
        return dwResult;

    }

	BYTE nStatus=0;
    dwResult=WaitForCommandStatus(nSlot, nStatus);
    if (dwResult!=ERROR_SUCCESS)
    {
        fbtLog(fbtLog_Failure, _T("CHciLocal::SendInquiry: WaitForCommandStatus failed, error %d"), dwResult);
		DeQueueCommand(nSlot);
        return dwResult;

    }

	fbtLog(fbtLog_Failure, _T("CHciLocal::SendInquiry: Command completed with status %s (0x%x)"), GetStatusText(nStatus), nStatus);

	DeQueueCommand(nSlot);

    return nStatus;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}


DWORD CHciLocal::SendInquiryCancel()
{
    FBT_TRY

    BYTE nStatus=0;
	int nSlot=QueueCommand(FBT_HCI_CMD_INQUIRY_CANCEL, &nStatus, sizeof(nStatus));

    DWORD dwResult=CHci::SendInquiryCancel();
    if (dwResult!=ERROR_SUCCESS)
    {
        fbtLog(fbtLog_Failure, _T("CHciLocal::SendInquiryCancel: CHci::SendInquiryCancel Failed, error %d"), dwResult);
		DeQueueCommand(nSlot);
        return dwResult;

    }

    dwResult=WaitForCommandComplete(nSlot);
    if (dwResult!=ERROR_SUCCESS)
    {
        fbtLog(fbtLog_Failure, _T("CHciLocal::SendInquiryCancel: WaitForCommandComplete failed, error %d"), dwResult);
		DeQueueCommand(nSlot);
        return dwResult;

    }

    fbtLog(fbtLog_Notice, _T("CHciLocal::SendInquiryCancel: Complete, Status %d"), nStatus);

	DeQueueCommand(nSlot);

    return nStatus;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHciLocal::SendCreateConnection(BYTE BD_ADDR[FBT_HCI_BDADDR_SIZE], USHORT nPacketType, BYTE nPageScanRepetitionMode, BYTE nPageScanMode, USHORT nClockOffset, BYTE nAllowRoleSwitch)
{
    FBT_TRY

    fbtLog(fbtLog_Enter, _T("CHciLocal::SendCreateConnection: Enter"));

	int nSlot=QueueCommandStatus(FBT_HCI_CMD_CREATE_CONNECTION);

    DWORD dwResult=CHci::SendCreateConnection(BD_ADDR, nPacketType, nPageScanRepetitionMode, nPageScanMode, nClockOffset, nAllowRoleSwitch);
    if (dwResult!=ERROR_SUCCESS)
    {
        fbtLog(fbtLog_Failure, _T("CHciLocal::SendCreateConnection: CHci::SendCreateConnection: Failed, error %d"), dwResult);
		DeQueueCommand(nSlot);
        return dwResult;

    }

	BYTE nStatus=0;
    dwResult=WaitForCommandStatus(nSlot, nStatus);
    if (dwResult!=ERROR_SUCCESS)
    {
        fbtLog(fbtLog_Failure, _T("CHciLocal::SendCreateConnection: WaitForCmmandStatus failed, error %d"), dwResult);
		DeQueueCommand(nSlot);
        return dwResult;

    }

    fbtLog(fbtLog_Notice, _T("CHciLocal::SendCreateConnection: Complete, %s (0x%02x)"), GetStatusText(nStatus), nStatus);

	DeQueueCommand(nSlot);

    return nStatus;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHciLocal::SendDisconnect(USHORT nConnectionHandle, BYTE nReason)
{
    FBT_TRY

    fbtLog(fbtLog_Enter, _T("CHciLocal::SendDisconnect: Enter"));

	int nSlot=QueueCommandStatus(FBT_HCI_CMD_DISCONNECT);

    DWORD dwResult=CHci::SendDisconnect(nConnectionHandle, nReason);
    if (dwResult!=ERROR_SUCCESS)
    {
        fbtLog(fbtLog_Failure, _T("CHciLocal::SendDisconnect: CHci::SendDisconnect: Failed, error %d"), dwResult);
		DeQueueCommand(nSlot);
        return dwResult;

    }

	BYTE nStatus=0;
    dwResult=WaitForCommandStatus(nSlot, nStatus);
    if (dwResult!=ERROR_SUCCESS)
    {
        fbtLog(fbtLog_Failure, _T("CHciLocal::SendDisconnect: WaitForCmmandStatus failed, error %d"), dwResult);
		DeQueueCommand(nSlot);
        return dwResult;

    }

    fbtLog(fbtLog_Notice, _T("CHciLocal::SendDisconnect: Complete, %s (0x%02x)"), GetStatusText(nStatus), nStatus);

	DeQueueCommand(nSlot);

    return nStatus;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHciLocal::SendSwitchRole(BYTE BD_ADDR[FBT_HCI_BDADDR_SIZE], BYTE nRole)
{
    FBT_TRY

    fbtLog(fbtLog_Enter, _T("CHciLocal::SendSwitchRole: Enter"));

	int nSlot=QueueCommandStatus(FBT_HCI_CMD_SWITCH_ROLE);

    DWORD dwResult=CHci::SendSwitchRole(BD_ADDR, nRole);
    if (dwResult!=ERROR_SUCCESS)
    {
        fbtLog(fbtLog_Failure, _T("CHciLocal::SendSwitchRole: CHci::SendSwitchRole: Failed, error %d"), dwResult);
		DeQueueCommand(nSlot);
        return dwResult;

    }

	BYTE nStatus=0;
    dwResult=WaitForCommandStatus(nSlot, nStatus);
    if (dwResult!=ERROR_SUCCESS)
    {
        fbtLog(fbtLog_Failure, _T("CHciLocal::SendSwitchRole: WaitForCmmandStatus failed, error %d"), dwResult);
		DeQueueCommand(nSlot);
        return dwResult;

    }

    fbtLog(fbtLog_Notice, _T("CHciLocal::SendSwitchRole: Complete, %s (0x%02x)"), GetStatusText(nStatus), nStatus);

	DeQueueCommand(nSlot);

    return nStatus;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHciLocal::SendRemoteNameRequest(BYTE BD_ADDR[FBT_HCI_BDADDR_SIZE], BYTE nPageScanRepetitionMode, BYTE nPageScanMode, USHORT nClockOffset)
{
    FBT_TRY

	int nSlot=QueueCommandStatus(FBT_HCI_CMD_REMOTE_NAME_REQUEST);

    DWORD dwResult=CHci::SendRemoteNameRequest(BD_ADDR, nPageScanRepetitionMode, nPageScanMode, nClockOffset);
    if (dwResult!=ERROR_SUCCESS)
    {
        fbtLog(fbtLog_Failure, _T("CHciLocal::SendRemoteNameRequest: CHci::SendRemoteNameRequest Failed, error %d"), dwResult);
		DeQueueCommand(nSlot);
        return dwResult;

    }

    BYTE nStatus=0;
    dwResult=WaitForCommandStatus(nSlot, nStatus);
    if (dwResult!=ERROR_SUCCESS)
    {
        fbtLog(fbtLog_Failure, _T("CHciLocal::SendRemoteNameRequest: WaitForCommandStatus failed, error %d"), dwResult);
		DeQueueCommand(nSlot);
        return dwResult;

    }

    fbtLog(fbtLog_Failure, _T("CHciLocal::SendRemoteNameRequest: Command complete, %s (0x%x)"), GetStatusText(nStatus), nStatus);

	DeQueueCommand(nSlot);

    return nStatus;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHciLocal::SendReset()
{
    FBT_TRY

    BYTE nStatus=0;
	int nSlot=QueueCommand(FBT_HCI_CMD_RESET, &nStatus, sizeof(nStatus));

    DWORD dwResult=CHci::SendReset();
    if (dwResult!=ERROR_SUCCESS)
    {
        fbtLog(fbtLog_Failure, _T("CHciLocal::SendReset: CHci::SendReset failed, error %d"), dwResult);
		DeQueueCommand(nSlot);
        return dwResult;

    }

    dwResult=WaitForCommandComplete(nSlot);
    if (dwResult!=ERROR_SUCCESS)
    {
        fbtLog(fbtLog_Failure, _T("CHciLocal::SendReset: WaitForCommandComplete failed, error %d"), dwResult);
		DeQueueCommand(nSlot);
        return dwResult;

    }

    fbtLog(fbtLog_Notice, _T("CHciLocal::SendReset: Complete, %s (0x%02x)"), GetStatusText(nStatus), nStatus);

	DeQueueCommand(nSlot);

    return nStatus;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHciLocal::SendWriteClassOfDevice(BYTE ClassOfDevice[FBT_HCI_DEVICE_CLASS_SIZE])
{
    FBT_TRY

    BYTE nStatus=0;
	int nSlot=QueueCommand(FBT_HCI_CMD_WRITE_CLASS_OF_DEVICE, &nStatus, sizeof(nStatus));

    DWORD dwResult=CHci::SendWriteClassOfDevice(ClassOfDevice);
    if (dwResult!=ERROR_SUCCESS)
    {
        fbtLog(fbtLog_Failure, _T("CHciLocal::SendWriteClassOfDevice: CHci::SendWriteClassOfDevice failed, error %d"), dwResult);
		DeQueueCommand(nSlot);
        return dwResult;

    }

    dwResult=WaitForCommandComplete(nSlot);
    if (dwResult!=ERROR_SUCCESS)
    {
        fbtLog(fbtLog_Failure, _T("CHciLocal::SendWriteClassOfDevice: WaitForCommandComplete failed, error %d"), dwResult);
		DeQueueCommand(nSlot);
        return dwResult;

    }

    fbtLog(fbtLog_Notice, _T("CHciLocal::SendWriteClassOfDevice: Complete, %s (0x%02x)"), GetStatusText(nStatus), nStatus);

	DeQueueCommand(nSlot);

    return nStatus;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHciLocal::SendReadLocalVersionInformation(FBT_HCI_READ_LOCAL_VERSION_INFORMATION_COMPLETE &CommandComplete)
{
    FBT_TRY

	int nSlot=QueueCommand(FBT_HCI_CMD_READ_LOCAL_VERSION_INFORMATION, (LPBYTE)&CommandComplete, sizeof(CommandComplete));

    DWORD dwResult=CHci::SendReadLocalVersionInformation();
    if (dwResult!=ERROR_SUCCESS)
    {
        fbtLog(fbtLog_Failure, _T("CHciLocal::SendReadLocalVersionInformation: CHci::SendReadLocalVersionInformation failed, error %d"), dwResult);
		DeQueueCommand(nSlot);
        return dwResult;

    }

    dwResult=WaitForCommandComplete(nSlot);
    if (dwResult!=ERROR_SUCCESS)
    {
        fbtLog(fbtLog_Failure, _T("CHciLocal::SendReadLocalVersionInformation: WaitForCommandComplete failed, error %d"), dwResult);
		DeQueueCommand(nSlot);
        return dwResult;

    }

    fbtLog(fbtLog_Notice, _T("CHciLocal::SendReadLocalVersionInformation: Complete, %s (0x%02x)"), GetStatusText(CommandComplete.Status), CommandComplete.Status);

	DeQueueCommand(nSlot);

    return CommandComplete.Status;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHciLocal::SendReadBDADDR(FBT_HCI_READ_BD_ADDR_COMPLETE &CommandComplete)
{
   FBT_TRY

      int nSlot=QueueCommand(FBT_HCI_CMD_READ_BD_ADDR, (LPBYTE)&CommandComplete, sizeof(CommandComplete));

   DWORD dwResult=CHci::SendReadBDADDR();
   if (dwResult!=ERROR_SUCCESS)
   {
      fbtLog(fbtLog_Failure, _T("CHciLocal::SendReadBDADDR: CHci::SendReadBDADDR failed, error %d"), dwResult);
      DeQueueCommand(nSlot);
      return dwResult;

   }

   dwResult=WaitForCommandComplete(nSlot);
   if (dwResult!=ERROR_SUCCESS)
   {
      fbtLog(fbtLog_Failure, _T("CHciLocal::SendReadBDADDR: WaitForCommandComplete failed, error %d"), dwResult);
      DeQueueCommand(nSlot);
      return dwResult;

   }

   fbtLog(fbtLog_Notice, _T("CHciLocal::SendReadBDADDR: Complete, %s (0x%02x)"), GetStatusText(CommandComplete.Status), CommandComplete.Status);

   DeQueueCommand(nSlot);

   return CommandComplete.Status;

   FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHciLocal::SendReadBufferSize(FBT_HCI_READ_BUFFER_SIZE_COMPLETE &CommandComplete)
{
   FBT_TRY

      int nSlot=QueueCommand(FBT_HCI_CMD_READ_BUFFER_SIZE, (LPBYTE)&CommandComplete, sizeof(CommandComplete));

   DWORD dwResult=CHci::SendReadBufferSize();
   if (dwResult!=ERROR_SUCCESS)
   {
      fbtLog(fbtLog_Failure, _T("CHciLocal::SendReadBufferSize: CHci::SendReadBufferSize failed, error %d"), dwResult);
      DeQueueCommand(nSlot);
      return dwResult;

   }

   dwResult=WaitForCommandComplete(nSlot);
   if (dwResult!=ERROR_SUCCESS)
   {
      fbtLog(fbtLog_Failure, _T("CHciLocal::SendReadBufferSize: WaitForCommandComplete failed, error %d"), dwResult);
      DeQueueCommand(nSlot);
      return dwResult;

   }

   fbtLog(fbtLog_Notice, _T("CHciLocal::SendReadBufferSize: Complete, %s (0x%02x)"), GetStatusText(CommandComplete.Status), CommandComplete.Status);

   DeQueueCommand(nSlot);

   return CommandComplete.Status;

   FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)
}

// Override superclass in order to provide CommandComplete and CommandStatus wrappers
DWORD CHciLocal::OnEvent(PFBT_HCI_EVENT_HEADER pEvent, DWORD Length)
{
    FBT_TRY

    switch (pEvent->EventCode)
    {
        case FBT_HCI_EVENT_COMMAND_COMPLETE:
            CommandCompleteHandler(
                ((PFBT_HCI_COMMAND_COMPLETE)pEvent)->OpCode,
                ((PFBT_HCI_COMMAND_COMPLETE)pEvent)->Parameters,
                ((PFBT_HCI_COMMAND_COMPLETE)pEvent)->EventHeader.ParameterLength - (sizeof(FBT_HCI_COMMAND_COMPLETE) - sizeof(FBT_HCI_EVENT_HEADER) - 1));

            return ERROR_SUCCESS;

        case FBT_HCI_EVENT_COMMAND_STATUS:
            CommandStatusHandler(
                ((PFBT_HCI_COMMAND_STATUS)pEvent)->Status,
                ((PFBT_HCI_COMMAND_STATUS)pEvent)->OpCode);

            return ERROR_SUCCESS;

    }

    CHci::OnEvent(pEvent, Length);

    return ERROR_SUCCESS;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

int CHciLocal::FindCommandSlot(USHORT nCommand)
{
	FBT_TRY

	for (int i=0; i<MAX_QUEUED_COMMANDS; i++)
	{
		if (m_QueuedCommands[i].nCommand==nCommand)
		{
			return i;

		}

	}

	return -1;

	FBT_CATCH_RETURN(-1)

}

// Called in the context of the event thread
// Find and complete the corresponding event
DWORD CHciLocal::CommandCompleteHandler(USHORT nCommand, BYTE *pParameters, DWORD dwParameterLength)
{
    FBT_TRY

	int nSlot=FindCommandSlot(nCommand);
    if (nSlot==-1)
	{
		fbtLog(fbtLog_Notice, _T("CHciLocal::CommandCompleteHandler: Command OGF=0x%02x, OGF=0x%02x, not queued"), FBT_HCI_OGF_FROM_COMMAND(nCommand), FBT_HCI_OCF_FROM_COMMAND(nCommand));
        return ERROR_SUCCESS;

	}

	PQueuedCommand pQueuedCommand=&(m_QueuedCommands[nSlot]);
    fbtLog(fbtLog_Notice, _T("CHciLocal::CommandCompleteHandler: Handling CommandComplete for command OGF=0x%02x, OGF=0x%02x, ParameterLength=%d"), FBT_HCI_OGF_FROM_COMMAND(nCommand), FBT_HCI_OCF_FROM_COMMAND(nCommand), dwParameterLength);
    if (dwParameterLength>pQueuedCommand->dwBufferSize)
    {
        fbtLog(fbtLog_Failure, _T("CHciLocal::CommandCompleteHandler: Command OGF=0x%02x, OGF=0x%02x complete with %d bytes of parameters, only a %d byte parameter buffer was supplied"), FBT_HCI_OGF_FROM_COMMAND(nCommand), FBT_HCI_OCF_FROM_COMMAND(nCommand), dwParameterLength, pQueuedCommand->dwBufferSize);
        return ERROR_INTERNAL_ERROR;

    }

	fbtLog(fbtLog_Notice, _T("CHciLocal::CommandCompleteHandler: ParameterLength=%d"), dwParameterLength);
    if (dwParameterLength>0)
	{
        fbtLog(fbtLog_Notice, _T("CHciLocal::CommandCompleteHandler: Status=%02x"), *pParameters);
		if (fbtLogGetLevel()>=fbtLog_Verbose)
		{
			TCHAR szLog[1024]={0};
			TCHAR szBuf[10]={0};
			for (UINT i=0; i<dwParameterLength; i++)
			{
				_stprintf(szBuf, _T("%02x "), pParameters[i]);
				_tcscat(szLog, szBuf);

			}

			fbtLog(fbtLog_Notice, _T("CHciLocal::CommandCompleteHandler: Parameters: %s"), szLog);

		}

		CopyMemory(pQueuedCommand->pResultBuffer, pParameters, dwParameterLength);
		pQueuedCommand->dwBufferSize=dwParameterLength;

	}

    SetEvent(pQueuedCommand->hEvent);

    return ERROR_SUCCESS;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}

DWORD CHciLocal::CommandStatusHandler(BYTE nStatus, USHORT nCommand)
{
    FBT_TRY

	int nSlot=FindCommandSlot(nCommand);
    if (nSlot==-1)
	{
		fbtLog(fbtLog_Notice, _T("CHciLocal::CommandStatusHandler: Command OGF=0x%02x, OGF=0x%02x, not queued"), FBT_HCI_OGF_FROM_COMMAND(nCommand), FBT_HCI_OCF_FROM_COMMAND(nCommand));
        return ERROR_SUCCESS;

	}

	PQueuedCommand pQueuedCommand=&(m_QueuedCommands[nSlot]);
    fbtLog(fbtLog_Notice, _T("CHciLocal::CommandStatusHandler: Handling CommandStatus for command OGF=0x%02x, OGF=0x%02x"), FBT_HCI_OGF_FROM_COMMAND(nCommand), FBT_HCI_OCF_FROM_COMMAND(nCommand));
    if (pQueuedCommand->dwBufferSize<sizeof(FBT_HCI_COMMAND_STATUS))
    {
        fbtLog(fbtLog_Failure, _T("CHciLocal::CommandStatusHandler: Command OGF=0x%02x, OGF=0x%02x, not enough buffer provided (%d)"), FBT_HCI_OGF_FROM_COMMAND(nCommand), FBT_HCI_OCF_FROM_COMMAND(nCommand), pQueuedCommand->dwBufferSize);
        return ERROR_INTERNAL_ERROR;

    }


	fbtLog(fbtLog_Notice, _T("CHciLocal::CommandStatusHandler: Writing status into buffer at %08x"), pQueuedCommand->pResultBuffer);

	PFBT_HCI_COMMAND_STATUS pCommandStatus=(PFBT_HCI_COMMAND_STATUS)pQueuedCommand->pResultBuffer;
	pCommandStatus->Status=nStatus;
	pQueuedCommand->dwBufferSize=sizeof(FBT_HCI_COMMAND_STATUS);

	fbtLog(fbtLog_Notice, _T("CHciLocal::CommandStatusHandler: Command OGF=0x%02x, OGF=0x%02x, status %s on slot %d"), FBT_HCI_OGF_FROM_COMMAND(nCommand), FBT_HCI_OCF_FROM_COMMAND(nCommand), GetStatusText(nStatus), nSlot);

    SetEvent(pQueuedCommand->hEvent);

    return nStatus;

    FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)

}