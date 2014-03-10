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

// HW Driver Abstraction layer

/**
 * The original and principal author is Antony C. Roberts http://www.freebt.net.  
 * Other authors are noted in the change history that follows (in reverse chronological order):
 *
 * 2008-11-30 Dmitry Klionsky Added UNICODE support.
 * 2008-11-30 Dmitry Klionsky Removed virtuality. This class won't be a base class any more.
 */

#define  _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <winioctl.h>

#include <tchar.h>

#include "fbtutil.h"
#include "fbthw.h"

CBTHW::CBTHW()
{
	FBT_TRY

	   m_hDriver = INVALID_HANDLE_VALUE;
	   m_szDeviceName[0] = '\0';

	FBT_CATCH_NORETURN
}

CBTHW::~CBTHW()
{
	FBT_TRY

	   fbtLog( fbtLog_Notice, _T("CBTHW::~CBTHW") );

	FBT_CATCH_NORETURN
}

void CBTHW::SetDeviceName( LPCTSTR szDeviceName )
{
	FBT_TRY

	   _tcscpy( m_szDeviceName, szDeviceName );

	FBT_CATCH_NORETURN
}

DWORD CBTHW::GetDeviceName( LPTSTR szBuffer, DWORD dwBufferSize )
{
	FBT_TRY

	if ( ( _tcslen( m_szDeviceName ) + 1 ) > dwBufferSize )
		return 0;

	_tcscpy( szBuffer, m_szDeviceName );

	return _tcslen( m_szDeviceName ) + 1;

	FBT_CATCH_RETURN(0)
}

DWORD CBTHW::Attach( LPCTSTR szDeviceName )
{
	FBT_TRY

    if ( IsAttached() )
        return ERROR_SUCCESS;

    m_hDriver = CreateFile(
                szDeviceName,
                GENERIC_READ | GENERIC_WRITE,
                0, // Don't share this I/F
                NULL,
                OPEN_EXISTING,
                FILE_FLAG_OVERLAPPED,
                NULL );

   if ( m_hDriver == INVALID_HANDLE_VALUE )
   {
      DWORD dwLastError = GetLastError();
      fbtLog( fbtLog_Failure, _T("CBTHW::Attach: Failed to open driver device %s, error %d"), szDeviceName, dwLastError );
      return dwLastError;
   }

   fbtLog( fbtLog_Notice, _T("CBTHW::Attach: Successfully opened driver device %s"), szDeviceName );

   SetDeviceName( szDeviceName );

   return ERROR_SUCCESS;

	FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)
}

DWORD CBTHW::Detach()
{
   FBT_TRY

      if ( !IsAttached() )
         return ERROR_SUCCESS;

      if ( !CloseHandle( m_hDriver ) )
      {
         DWORD dwLastError = GetLastError();
         fbtLog( fbtLog_Failure, _T("CBTHW::Detach: CloseHandle failed, error %d"), dwLastError );
         return dwLastError;
      }

      m_hDriver = INVALID_HANDLE_VALUE;

      return ERROR_SUCCESS;

   FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)
}

HANDLE CBTHW::GetDriverHandle() const
{
	FBT_TRY

	return m_hDriver;

	FBT_CATCH_RETURN(INVALID_HANDLE_VALUE)
}

DWORD CBTHW::SendCommand( DWORD dwCommand, LPCVOID lpInBuffer, DWORD dwInBufferSize, LPVOID lpOutBuffer, DWORD dwOutBufferSize, OVERLAPPED* pOverlapped )
{
   FBT_TRY

      if ( !IsAttached() )
         return ERROR_INVALID_HANDLE;

      OVERLAPPED Overlapped;
      BOOL bAdHocOverlapped = FALSE;
      if ( pOverlapped == NULL )
      {
         ZeroMemory( &Overlapped, sizeof(OVERLAPPED) );
         pOverlapped = &Overlapped;
         Overlapped.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
         bAdHocOverlapped = TRUE;
      }

      ResetEvent( pOverlapped->hEvent );

      DWORD dwLength;
      if ( !DeviceIoControl(
                        m_hDriver,
                        dwCommand,
                        (LPVOID)lpInBuffer,
                        dwInBufferSize,
                        lpOutBuffer,
                        dwOutBufferSize,
                        &dwLength,
                        pOverlapped ) )
      {
         DWORD dwReturnCode = GetLastError();
         if ( dwReturnCode != ERROR_IO_PENDING )
         {
            fbtLog( fbtLog_Failure, _T("CBTHW::SendCommand: Asynchronous DeviceIoControl %u failed, last error=%u"), dwCommand, dwReturnCode );
            return dwReturnCode;
         }

         if ( bAdHocOverlapped )
         {
            fbtLog( fbtLog_Notice, _T("CBTHW::SendCommand: Waiting on GetOverlappedResult") );
            if ( !GetOverlappedResult( GetDriverHandle(), &Overlapped, &dwLength, TRUE ) )
            {
               dwReturnCode = GetLastError();
               fbtLog( fbtLog_Notice, _T("CBTHW::SendCommand: GetOverlappedResult failed, error %d"), dwReturnCode );
               return dwReturnCode;
            }
            CloseHandle( Overlapped.hEvent );
         }
      }

      fbtLog( fbtLog_Notice, _T("CBTHW::SendCommand: Completed successfully") );

      return ERROR_SUCCESS;

   FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)
}

DWORD CBTHW::SendData( LPCVOID lpBuffer, DWORD dwBufferSize, DWORD* dwBytesWritten, OVERLAPPED* pOverlapped )
{
   FBT_TRY

      if ( !IsAttached() )
         return ERROR_INVALID_HANDLE;

      OVERLAPPED Overlapped;
      BOOL bAdHocOverlapped = FALSE;
      if ( pOverlapped == NULL )
      {
         ZeroMemory( &Overlapped, sizeof(OVERLAPPED) );
         pOverlapped = &Overlapped;
         Overlapped.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
         bAdHocOverlapped=TRUE;
      }

      ResetEvent( pOverlapped->hEvent );

      if ( !WriteFile( GetDriverHandle(), lpBuffer, dwBufferSize, dwBytesWritten, pOverlapped ) )
      {
         DWORD dwReturnCode = GetLastError();
         if ( dwReturnCode != ERROR_IO_PENDING )
         {
            fbtLog( fbtLog_Failure, _T("CBTHW::SendData: Asynchronous WriteFile failed, last error=%u"), dwReturnCode );
            return dwReturnCode;
         }

         if ( bAdHocOverlapped )
         {
            fbtLog( fbtLog_Notice, _T("CBTHW::SendData: Waiting on GetOverlappedResult") );
            if ( !GetOverlappedResult( GetDriverHandle(), &Overlapped, dwBytesWritten, TRUE ) )
            {
               dwReturnCode = GetLastError();
               fbtLog( fbtLog_Notice, _T("CBTHW::SendData: GetOverlappedResult failed, error %d"), dwReturnCode );
               return dwReturnCode;
            }
            CloseHandle( Overlapped.hEvent );
         }
      }

      fbtLog( fbtLog_Notice, _T("CBTHW::SendData: Completed successfully") );

      return ERROR_SUCCESS;

   FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)
}

DWORD CBTHW::GetData( LPVOID lpBuffer, DWORD dwBufferSize, DWORD* dwBytesRead, OVERLAPPED* pOverlapped )
{
   FBT_TRY

      if ( !IsAttached() )
         return ERROR_INVALID_HANDLE;

      OVERLAPPED Overlapped;
      BOOL bAdHocOverlapped = FALSE;
      if ( pOverlapped == NULL )
      {
         ZeroMemory( &Overlapped, sizeof(OVERLAPPED) );
         pOverlapped = &Overlapped;
         Overlapped.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
         bAdHocOverlapped = TRUE;
      }

      ResetEvent( pOverlapped->hEvent );

      if ( !ReadFile( GetDriverHandle(), lpBuffer, dwBufferSize, dwBytesRead, pOverlapped ) )
      {
         DWORD dwReturnCode = GetLastError();
         if ( dwReturnCode!=ERROR_IO_PENDING )
         {
            fbtLog( fbtLog_Failure, _T("CBTHW::GetData: Asynchronous ReadFile failed, last error=%u"), dwReturnCode );
            return dwReturnCode;
         }

         if ( bAdHocOverlapped )
         {
            fbtLog( fbtLog_Notice, _T("CBTHW::GetData: Waiting on GetOverlappedResult") );
            if ( !GetOverlappedResult( GetDriverHandle(), &Overlapped, dwBytesRead, TRUE ) )
            {
               dwReturnCode = GetLastError();
               fbtLog( fbtLog_Notice, _T("CBTHW::GetData: GetOverlappedResult failed, error %d"), dwReturnCode );
               return dwReturnCode;
            }
            CloseHandle( Overlapped.hEvent );
         }
      }

      fbtLog( fbtLog_Notice, _T("CBTHW::GetData: Completed successfully") );

      return ERROR_SUCCESS;

   FBT_CATCH_RETURN(ERROR_INTERNAL_ERROR)
}