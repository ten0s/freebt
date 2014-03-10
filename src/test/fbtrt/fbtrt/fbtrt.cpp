// Copyright (C) 2008-2009 Dmitry Klionsky aka ten0s <dm.klionsky@gmail.com>

// Use of this file is subject to the terms
// described in the LICENSE.TXT file that
// accompanies this file.
//
// Your use of this file indicates your
// acceptance of the terms described in
// LICENSE.TXT.

#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#define __USE_DLL__

#ifndef __USE_DLL__
#include "BthEmulHci.h"

CBthEmulHci g_bthHw;
BOOL AttachHardware( CBthEmulHci& hw );
BOOL DetachHardware( CBthEmulHci& hw );
BOOL SendHCICommand( CBthEmulHci& hw, BYTE* /*in*/pCmdBuffer, DWORD /*in*/dwCmdLength );
BOOL GetHCIEvent( CBthEmulHci& hw, BYTE* /*out*/pEventBuffer, DWORD& /*in/out*/dwEventLength );
BOOL SubscribeHCIEvent( CBthEmulHci& hw, HCI_EVENT_LISTENER hciEventListener );

BOOL Export_AttachHardware();
BOOL Export_DetachHardware();
BOOL Export_SendHCICommand( BYTE* /*in*/pCmdBuffer, DWORD /*in*/dwCmdLength );
BOOL Export_GetHCIEvent( BYTE* /*out*/pEventBuffer, DWORD& /*in/out*/dwEventLength );
BOOL Export_SubscribeHCIEvent( HCI_EVENT_LISTENER hciEventListener );

#else
#include "fbtrt.h"
#endif

#include "fbtlog.h"
#include "fbtHciCmds.h"

#define DEVICE_SYM_LINK	_T("\\\\.\\FbtUsb")


#define MAX_BUFFER_SIZE	257
void PrintBuffer( const CHAR* szMsg, BYTE* pBuffer, DWORD dwLength );


extern "C" DWORD __stdcall OnEvent( BYTE* /*in*/pEventBuffer, DWORD /*in*/dwEventLength )
{
   PrintBuffer( "Event", pEventBuffer, dwEventLength );

   return TRUE;
}

int _tmain(int argc, _TCHAR* argv[])
{
	unsigned char buffer[MAX_BUFFER_SIZE];

   SetLogFileName(_T("fbtrt.log"));
   SetLogLevel(255);

   int deviceId = OpenDevice();
   if ( deviceId != INVALID_DEVICE_ID )
	{
      SubscribeHCIEvent( deviceId, OnEvent );

      buffer[0] = FBT_HCI_SYNC_HCI_COMMAND_PACKET;
      buffer[1] = 0x03;
		buffer[2] = 0x0c;
		buffer[3] = 0x00;
		DWORD dwInLength = 4;
		PrintBuffer( "Command", buffer, dwInLength );

		DWORD dwOutLength = MAX_BUFFER_SIZE;
		BOOL bRet = SendHCICommand( deviceId, buffer, dwInLength );
		if ( !bRet )
		{
			printf( "Error: %lu\n", GetLastError() );
		}

      Sleep(1000);

      TCHAR szBuffer[256];
      for (short i = 0; i < MAXINT16; ++i)
      {
         bRet = GetManufacturerName(i, szBuffer, sizeof(szBuffer)/sizeof(szBuffer[0]));
         if (bRet == TRUE)
         {
            wprintf(L"%s\n", szBuffer);
         }
         else
         {
            break;
         }
      }

      printf("Press any key...\n");
      getchar();
      CloseDevice( deviceId );
	}

	return 0;
}

void PrintBuffer( const CHAR* szMsg, BYTE* pBuffer, DWORD dwLength )
{
	printf( "%s: ", szMsg );
	for ( DWORD i = 0; i < dwLength; ++i )
	{
		printf( "%02x ", pBuffer[i] );
	}
	printf( "\r\n" );
}

#ifndef __USE_DLL__

BOOL AttachHardware( CBthEmulHci& hw )
{
   if ( hw.IsAttached() ) {
      SetLastError( ERROR_DEVICE_IN_USE );
      return FALSE;
   }

   TCHAR szDeviceName[MAX_PATH];
   for( int i = 0; i < 255 && !hw.IsAttached(); ++i ) {
      wsprintf( szDeviceName, _T("%s%02d"), DEVICE_SYM_LINK, i );
      hw.Attach( szDeviceName );
   }

   if ( !hw.IsAttached() )
   {
      SetLastError( ERROR_DEVICE_NOT_AVAILABLE );
      return FALSE;
   }

   if ( ERROR_SUCCESS != hw.StartEventListener() )
   {
      hw.Detach();
      return FALSE;
   }

   return TRUE;
}

BOOL DetachHardware( CBthEmulHci& hw )
{
   if ( !hw.IsAttached() )
   {
      SetLastError( ERROR_DEVICE_NOT_AVAILABLE );
      return FALSE;
   }

   hw.StopEventListener();
   hw.Detach();
   return TRUE;
}

BOOL SendHCICommand( CBthEmulHci& hw, BYTE* /*in*/pCmdBuffer, DWORD /*in*/dwCmdLength )
{
   DWORD dwResult = hw.SendHCICommand( pCmdBuffer, dwCmdLength );
   return ( dwResult == ERROR_SUCCESS );
}

BOOL SubscribeHCIEvent( CBthEmulHci& hw, HCI_EVENT_LISTENER hciEventListener )
{
   return hw.SubscribeHCIEvent( hciEventListener );
}

#endif
