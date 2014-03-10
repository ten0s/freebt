// Copyright (C) 2008-2009 Dmitry Klionsky aka ten0s <dm.klionsky@gmail.com>

// Use of this file is subject to the terms
// described in the LICENSE.TXT file that
// accompanies this file.
//
// Your use of this file indicates your
// acceptance of the terms described in
// LICENSE.TXT.

#include "BthEmulHci.h"
#include "fbtutil.h"    // FBT_TRY
#include "fbtusr.h"     // IOCTL_FREEBT_HCI_SEND_CMD, IOCTL_FREEBT_HCI_GET_EVENT
#include "fbtHciLocal.h"

#define BUFFER_SIZE (16 * 1024)
CRITICAL_SECTION CBthEmulHci::s_criticalSection;

CBthEmulHci::CBthEmulHci( CBTHW& btHw ) : CHci( btHw ), m_btHw( btHw ), m_hciEventListener( NULL ), m_hReaderThread( NULL ), m_hStopReadingEvent( NULL ), m_hReaderReadyEvent( NULL )
{
   InitializeCriticalSection( &s_criticalSection );
   m_hStopReadingEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
   memset( &m_devInfo, 0, sizeof(DEVICE_INFO) );
}

CBthEmulHci::~CBthEmulHci()
{
   CloseHandle( m_hStopReadingEvent );
   m_hStopReadingEvent = NULL;
   DeleteCriticalSection( &s_criticalSection );
}

BOOL CBthEmulHci::SubscribeHCIEvent( HCI_EVENT_LISTENER hciEventListener )
{
   if ( NULL != hciEventListener )
   {
      m_hciEventListener = hciEventListener;
      return TRUE;
   }

   return FALSE;
}

DWORD CBthEmulHci::SendHCICommand( const BYTE* lpBuffer, DWORD dwBufferSize )
{
   FBT_TRY

      DWORD dwResult = ERROR_SUCCESS;
      DWORD dwBytesSent = 0;

      // check received buffer.
      if ( lpBuffer != NULL && dwBufferSize > 0 )
      {
         // determine what type of command received.
         BYTE hciType = lpBuffer[0];
         fbtLog( fbtLog_Notice, _T("CBthEmulHci::SendHCICommand hciType: %02d buffer (%d):"), hciType, dwBufferSize );
         fbtLogDumpBuf( fbtLog_Notice, lpBuffer, dwBufferSize );

         switch( hciType )
         {
         case FBT_HCI_SYNC_HCI_COMMAND_PACKET:
            dwResult = SendCommand( IOCTL_FREEBT_HCI_SEND_CMD, lpBuffer + 1, dwBufferSize - 1 );
            break;

         case FBT_HCI_SYNC_ACL_DATA_PACKET:
            dwResult = SendData( lpBuffer + 1, dwBufferSize - 1, &dwBytesSent, NULL );
            break;

         case FBT_HCI_SYNC_SCO_DATA_PACKET:
            dwResult = ERROR_CALL_NOT_IMPLEMENTED;
            break;

         case FBT_HCI_SYNC_HCI_EVENT_PACKET:
            dwResult = ERROR_INVALID_PARAMETER;
            break;
         }
      }
      else
      {
         dwResult = ERROR_INSUFFICIENT_BUFFER;
      }

      return dwResult;

   FBT_CATCH_RETURN( ERROR_INTERNAL_ERROR )
}

DWORD CBthEmulHci::StartEventListener()
{
   FBT_TRY

      DWORD dwResult = CHci::StartEventListener();
      if ( dwResult == ERROR_SUCCESS )
      {
         if ( m_hReaderThread != NULL )
         {
            fbtLog( fbtLog_Failure, _T("CBthEmulHci:StartEventListener: Reader already running") );
            return ERROR_INTERNAL_ERROR;
         }

         // start the reader thread.
         m_hReaderReadyEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
         m_hReaderThread = CreateThread( NULL, 0, DataReader, this, 0, NULL );

         if ( m_hReaderThread == NULL )
         {
            DWORD dwLastError = GetLastError();
            CloseHandle( m_hReaderReadyEvent );
            fbtLog( fbtLog_Failure, _T("CBthEmulHci::StartEventListener: Failed to create reader thread, error %d"), dwLastError );
            return dwLastError;
         }

         WaitForSingleObject( m_hReaderReadyEvent, INFINITE );
         CloseHandle( m_hReaderReadyEvent );

         return ERROR_SUCCESS;
      }

      return dwResult;

   FBT_CATCH_RETURN( ERROR_INTERNAL_ERROR )
}

DWORD CBthEmulHci::StopEventListener()
{
   FBT_TRY

      DWORD dwParentResult = CHci::StopEventListener();

      SetEvent( m_hStopReadingEvent );
      DWORD dwResult = WaitForSingleObject( m_hReaderThread, 5000/**INFINITE*/ );
      if ( dwResult != WAIT_OBJECT_0 )
      {
         TerminateThread( m_hReaderThread, -1 );
      }

      CloseHandle( m_hReaderThread );
      m_hReaderThread = NULL;

      return dwParentResult || dwResult;

   FBT_CATCH_RETURN( ERROR_INTERNAL_ERROR )
}

DWORD CBthEmulHci::OnEvent( PFBT_HCI_EVENT_HEADER pEvent, DWORD dwLength )
{
   FBT_TRY

      DWORD dwResult = 0;

      if ( m_hciEventListener )
      {
         EnterCriticalSection( &s_criticalSection );

         fbtLog( fbtLog_Notice, _T("CBthEmulHci::OnEvent buffer (%d):"), dwLength );
         fbtLogDumpBuf( fbtLog_Notice, (unsigned char*)pEvent, dwLength );

         BYTE eventBuffer[FBT_HCI_EVENT_MAX_SIZE + 1];
         memset( eventBuffer, 0, sizeof(eventBuffer) );

         eventBuffer[0] = FBT_HCI_SYNC_HCI_EVENT_PACKET;
         memcpy( eventBuffer + 1, pEvent, dwLength );

         dwResult = m_hciEventListener( eventBuffer, dwLength + 1 );
         if ( dwResult != ERROR_SUCCESS )
         {
            fbtLog( fbtLog_Failure, _T("CBthEmulHci::OnEvent: HciEventListener failed, error %d"), dwResult );
         }

         fbtLog( fbtLog_Notice, _T("CBthEmulHci::OnEvent: Event handling complete") );

         LeaveCriticalSection( &s_criticalSection );
      }

      return dwResult;

   FBT_CATCH_RETURN( ERROR_INTERNAL_ERROR )
}


DWORD WINAPI CBthEmulHci::DataReader( LPVOID lpParam )
{
   FBT_TRY

      CBthEmulHci* pThis = (CBthEmulHci*)lpParam;
      HANDLE hStopReadingEvent = pThis->m_hStopReadingEvent;
      BYTE readBuffer[FBT_HCI_DATA_MAX_SIZE];
      memset( readBuffer, 0, sizeof(readBuffer) );
      int nBufferPos = 0;
      int nPacketSize = 0;

      SetEvent( pThis->m_hReaderReadyEvent );

      OVERLAPPED overlapped;
      memset( &overlapped, 0, sizeof(overlapped) );
      overlapped.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

      for( ;; )
      {
         ResetEvent( overlapped.hEvent );

         DWORD dwBytesReaded = 0;
         DWORD dwResult = pThis->m_btHw.GetData( readBuffer + nBufferPos, sizeof(readBuffer) - nBufferPos, &dwBytesReaded, &overlapped );
         if ( ERROR_SUCCESS == dwResult )
         {
            HANDLE handles[] = { hStopReadingEvent, overlapped.hEvent };

            DWORD dwWait = WaitForMultipleObjects( sizeof(handles)/sizeof(handles[0]), handles, FALSE, INFINITE );
            if ( dwWait == WAIT_OBJECT_0 )
            {
               // stop event. exit cycle...
               break;
            }
            else if ( dwWait == WAIT_OBJECT_0 + 1 )
            {
               // reading completed.
               DWORD dwLength;
               if ( GetOverlappedResult( pThis->m_btHw.GetDriverHandle(), &overlapped, &dwLength, FALSE ) )
               {
                  fbtLog( fbtLog_Notice, _T("CBthEmulHci::DataReader GetOverlappedResult OK %d"), dwLength );

                  // the first data packet.
                  if ( nBufferPos == 0 )
                  {
                     // determine the whole packet size.
                     nPacketSize = 0;
                     memcpy( &nPacketSize, readBuffer + 2, 2 );
                     nPacketSize += 4;

                     fbtLog( fbtLog_Notice, _T("CBthEmulHci::DataReader data packet size %d"), nPacketSize );
                  }

                  nBufferPos += dwLength;
                  fbtLog( fbtLog_Notice, _T("CBthEmulHci::DataReader nBufferPos %d"), nBufferPos );

                  if ( nPacketSize == nBufferPos )
                  {
                     fbtLog( fbtLog_Notice, _T("CBthEmulHci::DataReader send packet further") );
                     PHCI_EVENT pEventParameters = (PHCI_EVENT)malloc( sizeof(HCI_EVENT) );
                     pEventParameters->pEvent = (PFBT_HCI_EVENT_HEADER)malloc( nPacketSize );
                     memcpy( pEventParameters->pEvent, readBuffer, nPacketSize );
                     pEventParameters->dwLength = nPacketSize;
                     pEventParameters->pThis = pThis;

                     // restore read buffer to zero state.
                     memset( readBuffer, 0, sizeof(readBuffer) );
                     nBufferPos = 0;
                     nPacketSize = 0;

                     // spin handling off in a thread in order to reduce turnaround time.
                     HANDLE hThread = CreateThread( NULL, 0, DataEventHandler, pEventParameters, 0, NULL );
                     if ( hThread == NULL )
                     {
                        DWORD dwLastError = GetLastError();
                        fbtLog( fbtLog_Failure, _T("CBthEmulHci::DataReader: Failed to create listener thread, error %d"), dwLastError );
                     }

                     CloseHandle( hThread );
                  }
               }
               else
               {
                  DWORD dwLastError = GetLastError();
                  fbtLog( fbtLog_Failure, _T("CBthEmulHci::DataReader: GetOverlappedResult failed, error %d"), dwLastError );
               }
            }
            else
            {
               // an error occured.
               fbtLog( fbtLog_Failure, _T("CBthEmulHci::DataReader: WaitForMultipleObjects ret: 0x%08x"), dwWait );
            }
         }
         else
         {
            fbtLog( fbtLog_Failure, _T("CBthEmulHci::GetData: Asynchronous Read failed, last error=%u"), dwResult );
         }
      }

      CloseHandle( overlapped.hEvent );

      return ERROR_SUCCESS;

   FBT_CATCH_RETURN( ERROR_INTERNAL_ERROR )
}

DWORD WINAPI CBthEmulHci::DataEventHandler( LPVOID lpParam )
{
   FBT_TRY

      PHCI_EVENT pEvent = (PHCI_EVENT)lpParam;
      CBthEmulHci* pThis = (CBthEmulHci*)pEvent->pThis;
      DWORD dwLength = pEvent->dwLength;
      BYTE eventBuffer[FBT_HCI_DATA_MAX_SIZE + 1];

      if ( pThis->m_hciEventListener )
      {
         EnterCriticalSection( &s_criticalSection );

         fbtLog( fbtLog_Notice, _T("CBthEmulHci::DataEventHandler buffer (%d):"), dwLength );
         fbtLogDumpBuf( fbtLog_Notice, (unsigned char*)pEvent->pEvent, dwLength );

         memset( eventBuffer, 0, sizeof(eventBuffer) );
         eventBuffer[0] = FBT_HCI_SYNC_ACL_DATA_PACKET;
         memcpy( eventBuffer + 1, pEvent->pEvent, dwLength );

         DWORD dwResult = pThis->m_hciEventListener( eventBuffer, dwLength + 1 );
         if ( dwResult != ERROR_SUCCESS )
         {
            fbtLog( fbtLog_Failure, _T("CBthEmulHci::DataEventHandler: HciEventListener failed, error %d"), dwResult );
         }

         fbtLog( fbtLog_Notice, _T("CBthEmulHci::DataEventHandler: Event handling complete") );

         LeaveCriticalSection( &s_criticalSection );
      }

      free( pEvent->pEvent );
      free( pEvent );

      return ERROR_SUCCESS;

   FBT_CATCH_RETURN( ERROR_INTERNAL_ERROR )
}

DWORD CBthEmulHci::Attach( LPCTSTR szDeviceName )
{
   return m_btHw.Attach( szDeviceName );
}

DWORD CBthEmulHci::Detach()
{
   return m_btHw.Detach();
}

HANDLE CBthEmulHci::GetDriverHandle() const
{
   return m_btHw.GetDriverHandle();
}

BOOL CBthEmulHci::IsAttached() const
{
   return m_btHw.IsAttached();
}

BOOL CBthEmulHci::GetDeviceInfo( DEVICE_INFO* pDevInfo )
{
   BOOL bRet = FALSE;

   CHciLocal hci( this->m_btHw );
   memset( &m_devInfo, 0, sizeof(DEVICE_INFO) );

   hci.StartEventListener();

   FBT_HCI_READ_BD_ADDR_COMPLETE rdBdAddrComplete = {0};
   if ( ERROR_SUCCESS == hci.SendReadBDADDR( rdBdAddrComplete ) )
   {
      memcpy( m_devInfo.bdaddr, rdBdAddrComplete.BD_ADDR, sizeof(m_devInfo.bdaddr) );
      bRet = TRUE;
   }

   FBT_HCI_READ_LOCAL_VERSION_INFORMATION_COMPLETE rdLocalVerInfComplete = {0};
   if ( ERROR_SUCCESS == hci.SendReadLocalVersionInformation( rdLocalVerInfComplete ) )
   {
      m_devInfo.hci_ver = rdLocalVerInfComplete.HCIVersion;
      m_devInfo.hci_rev = rdLocalVerInfComplete.HCIRevision;
      m_devInfo.lmp_ver = rdLocalVerInfComplete.LMPVersion;
      m_devInfo.lmp_subver = rdLocalVerInfComplete.LMPSubVersion;
      m_devInfo.manufacturer = rdLocalVerInfComplete.Manufacturer;
      bRet &= TRUE;
   }

   FBT_HCI_READ_BUFFER_SIZE_COMPLETE rdBufferSizeComplete = {0};
   if ( ERROR_SUCCESS == hci.SendReadBufferSize( rdBufferSizeComplete ) )
   {
      m_devInfo.acl_mtu = rdBufferSizeComplete.acl_mtu;
      m_devInfo.sco_mtu = rdBufferSizeComplete.sco_mtu;
      m_devInfo.acl_max_pkt = rdBufferSizeComplete.acl_max_pkt;
      m_devInfo.sco_max_pkt = rdBufferSizeComplete.sco_max_pkt;
      bRet &= TRUE;
   }

   hci.StopEventListener();

   if ( bRet )
   {
      memcpy( pDevInfo, &m_devInfo, sizeof(DEVICE_INFO) );
   }

   return bRet;
}

DWORD CBthEmulHci::SendData( LPCVOID lpBuffer, DWORD dwBufferSize, DWORD* dwBytesSent, OVERLAPPED* pOverlapped )
{
   return m_btHw.SendData( lpBuffer, dwBufferSize, dwBytesSent, pOverlapped );
}

DWORD CBthEmulHci::SendCommand( DWORD dwCommand, LPCVOID lpInBuffer, DWORD dwInBufferSize, LPVOID lpOutBuffer, DWORD dwOutBufferSize, OVERLAPPED* pOverlapped )
{
   return m_btHw.SendCommand( dwCommand, lpInBuffer, dwInBufferSize, lpOutBuffer, dwOutBufferSize, pOverlapped );
}
