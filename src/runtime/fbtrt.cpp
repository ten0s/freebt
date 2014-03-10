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
#include <crtdbg.h>

#include "fbtrt.h"
#include "fbtutil.h"       // fbtLogSetFile, fbtLogSetLevel
#include "fbtHciSizes.h"   // FBT_HCI_EVENT_MAX_SIZE
#include "BthEmulHci.h"

#define MAX_DEVICES        255
#define DEVICE_SYM_LINK    _T("\\\\.\\FbtUsb")

CBTHW* g_bthHw[MAX_DEVICES];
CBthEmulHci* g_bthHci[MAX_DEVICES];
DEVICE_INFO* g_bthDevInfo[MAX_DEVICES];
CRITICAL_SECTION g_hciCritSection; // defends g_bthHw, g_bthHci
CRITICAL_SECTION g_addCritSection; // defends g_bthDevInfo, information and logs functions.

BOOL AttachHardware( CBTHW& hw );
BOOL DetachHardware( CBthEmulHci& hw );
BOOL GetDeviceInfo( CBthEmulHci& hw, DEVICE_INFO* pDevInfo );
BOOL SendHCICommand( CBthEmulHci& hw, BYTE* /*in*/pCmdBuffer, DWORD /*in*/dwCmdLength );
BOOL SubscribeHCIEvent( CBthEmulHci& hw, HCI_EVENT_LISTENER hciEventListener );

void InitDevicesArray();
void UninitDevicesArray();

#pragma optimize( "", off )
BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
   // to resolve R6002 - floating point support not loaded
   float dummy = 0.0f;

   switch ( ul_reason_for_call )
   {
   case DLL_PROCESS_ATTACH:
      DisableThreadLibraryCalls( hModule );
      InitializeCriticalSection( &g_hciCritSection );
      InitializeCriticalSection( &g_addCritSection );
      InitDevicesArray();
      break;
   case DLL_PROCESS_DETACH:
      UninitDevicesArray();
      DeleteCriticalSection( &g_hciCritSection );
      DeleteCriticalSection( &g_addCritSection );
      _CrtDumpMemoryLeaks();
      break;
   }

    return TRUE;
}
#pragma optimize( "", on )

extern "C" int __stdcall Export_OpenDevice()
{
   EnterCriticalSection( &g_hciCritSection );

   int devId = INVALID_DEVICE_ID;
   SetLastError( ERROR_NO_MORE_ITEMS );

   for( int i = 0; i < MAX_DEVICES; ++i )
   {
      // there is an available slot.
      if ( g_bthHw[i] == NULL )
      {
         CBTHW* bthHw = new CBTHW();
         if ( bthHw )
         {
            BOOL bRet = AttachHardware( *bthHw );
            if ( bRet )
            {
               CBthEmulHci* bthHci = new CBthEmulHci( *bthHw );
               if ( bthHci )
               {
                  // try to get as much device info as possible.
                  DEVICE_INFO* devInfo = new DEVICE_INFO();
                  memset( devInfo, 0, sizeof(DEVICE_INFO) );
                  if ( GetDeviceInfo( *bthHci, devInfo ) )
                  {
                     EnterCriticalSection( &g_addCritSection );
                     g_bthDevInfo[i] = devInfo;
                     LeaveCriticalSection( &g_addCritSection );
                  }

                  if ( ERROR_SUCCESS == bthHci->StartEventListener() )
                  {
                     g_bthHw[i] = bthHw;
                     g_bthHci[i] = bthHci;
                     devId = i;
                  }
                  else
                  {
                     delete bthHci;
                     bthHci = NULL;

                     delete bthHw;
                     bthHw = NULL;
                  }
                  // exit cycle in any case...
                  break;
               }
               else
               {
                  delete bthHw;
                  bthHw = NULL;
               }
            }
         }
      }
   }

   LeaveCriticalSection( &g_hciCritSection );
   return devId;
}

extern "C" BOOL __stdcall Export_CloseDevice( int devId )
{
   EnterCriticalSection( &g_hciCritSection );

   BOOL bRet = FALSE;

   if ( devId >= 0 && devId < MAX_DEVICES )
   {
      int index = devId;
      CBTHW* bthHw = g_bthHw[index];
      CBthEmulHci* bthHci = g_bthHci[index];

      if ( bthHw && bthHci )
      {
         bRet = DetachHardware( *bthHci );
         if ( bRet )
         {
            delete bthHci;
            bthHci = NULL;
            g_bthHci[index] = NULL;

            delete bthHw;
            bthHw = NULL;
            g_bthHw[index] = NULL;
         }
      }
      else
      {
         SetLastError( ERROR_INVALID_PARAMETER );
      }
   }
   else
   {
      SetLastError( ERROR_INVALID_PARAMETER );
   }

   LeaveCriticalSection( &g_hciCritSection );
   return bRet;
}

extern "C" BOOL __stdcall Export_SendHCICommand( int devId, BYTE* /*in*/pCmdBuffer, DWORD /*in*/dwCmdLength )
{
   EnterCriticalSection( &g_hciCritSection );

   BOOL bRet = FALSE;

   if ( devId >= 0 && devId < MAX_DEVICES )
   {
      int index = devId;
      CBTHW* bthHw = g_bthHw[index];
      CBthEmulHci* bthHci = g_bthHci[index];

      if ( bthHw && bthHci )
      {
         bRet = SendHCICommand( *bthHci, pCmdBuffer, dwCmdLength );
      }
      else
      {
         SetLastError( ERROR_INVALID_PARAMETER );
      }
   }
   else
   {
      SetLastError( ERROR_INVALID_PARAMETER );
   }

   LeaveCriticalSection( &g_hciCritSection );
   return bRet;
}

extern "C" BOOL __stdcall Export_GetDeviceInfo( int devId, LOCAL_DEVICE_INFO* pDevInfo )
{
   EnterCriticalSection( &g_addCritSection );

   BOOL bRet = FALSE;

   if ( devId >= 0 && devId < MAX_DEVICES )
   {
      int index = devId;
      LOCAL_DEVICE_INFO* bthDevInfo = g_bthDevInfo[index];

      if ( bthDevInfo )
      {
         __try
         {
            memcpy( pDevInfo, bthDevInfo, sizeof(LOCAL_DEVICE_INFO) );
            bRet = TRUE;
         }
         __except( EXCEPTION_EXECUTE_HANDLER )
         {
            SetLastError( ERROR_INVALID_PARAMETER );
         }
      }
      else
      {
         SetLastError( ERROR_NO_DATA );
      }
   }
   else
   {
      SetLastError( ERROR_INVALID_PARAMETER );
   }

   LeaveCriticalSection( &g_addCritSection );
   return bRet;
}

extern "C" BOOL __stdcall Export_GetManufacturerName( USHORT usManufacturer, LPTSTR /*in*/szInBuffer, DWORD dwBufferLength )
{
   EnterCriticalSection( &g_addCritSection );

   BOOL bRet = FALSE;

   DWORD dwRet = CHci::GetManufacturerName( usManufacturer, szInBuffer, dwBufferLength );
   if ( dwRet == ERROR_SUCCESS )
   {
      bRet = TRUE;
   }
   else
   {
      SetLastError( dwRet );
   }

   LeaveCriticalSection( &g_addCritSection );
   return bRet;
}

extern "C" BOOL __stdcall Export_SubscribeHCIEvent( int devId, HCI_EVENT_LISTENER hciEventListener )
{
   EnterCriticalSection( &g_hciCritSection );

   BOOL bRet = FALSE;

   if ( devId >= 0 && devId < MAX_DEVICES )
   {
      int index = devId;
      CBTHW* bthHw = g_bthHw[index];
      CBthEmulHci* bthHci = g_bthHci[index];

      if ( bthHw && bthHci )
      {
         bRet = SubscribeHCIEvent( *bthHci, hciEventListener );
      }
      else
      {
         SetLastError( ERROR_INVALID_PARAMETER );
      }
   }
   else
   {
      SetLastError( ERROR_INVALID_PARAMETER );
   }

   LeaveCriticalSection( &g_hciCritSection );
   return bRet;
}

extern "C" BOOL __stdcall Export_SetLogFileName( LPCTSTR szFileName )
{
   EnterCriticalSection( &g_addCritSection );
   BOOL bRet = fbtLogSetFile( szFileName );
   LeaveCriticalSection( &g_addCritSection );
   return bRet;
}

extern "C" BOOL __stdcall Export_SetLogLevel( UINT uLevel )
{
   EnterCriticalSection( &g_addCritSection );
   fbtLogSetLevel( uLevel );
   LeaveCriticalSection( &g_addCritSection );
   return TRUE;
}

BOOL AttachHardware( CBTHW& hw )
{
   if ( hw.IsAttached() )
   {
      SetLastError( ERROR_DEVICE_IN_USE );
      return FALSE;
   }

   TCHAR szDeviceName[MAX_PATH];
   for( int i = 0; i < MAX_DEVICES && !hw.IsAttached(); ++i )
   {
      _stprintf_s( szDeviceName, sizeof(szDeviceName)/sizeof(szDeviceName[0]), _T("%s%02d"), DEVICE_SYM_LINK, i );
      hw.Attach( szDeviceName );
   }

   if ( !hw.IsAttached() )
   {
      SetLastError( ERROR_DEVICE_NOT_AVAILABLE );
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

BOOL GetDeviceInfo( CBthEmulHci& hw, DEVICE_INFO* pDevInfo )
{
   if ( !hw.IsAttached() )
   {
      SetLastError( ERROR_DEVICE_NOT_AVAILABLE );
      return FALSE;
   }

   return hw.GetDeviceInfo( pDevInfo );
}

BOOL SendHCICommand( CBthEmulHci& hw, BYTE* /*in*/pCmdBuffer, DWORD /*in*/dwCmdLength )
{
   DWORD dwResult = hw.SendHCICommand( pCmdBuffer, dwCmdLength );
   SetLastError( dwResult );

   return ( dwResult == ERROR_SUCCESS );
}

BOOL SubscribeHCIEvent( CBthEmulHci& hw, HCI_EVENT_LISTENER hciEventListener )
{
   return hw.SubscribeHCIEvent( hciEventListener );
}

void InitDevicesArray()
{
   EnterCriticalSection( &g_hciCritSection );

   for( int i = 0; i < MAX_DEVICES; ++i )
   {
      g_bthHw[i] = NULL;
      g_bthHci[i] = NULL;
      EnterCriticalSection( &g_addCritSection );
      g_bthDevInfo[i] = NULL;
      LeaveCriticalSection( &g_addCritSection );
   }

   LeaveCriticalSection( &g_hciCritSection );
}

void UninitDevicesArray()
{
   EnterCriticalSection( &g_hciCritSection );

   for( int i = 0; i < MAX_DEVICES; ++i )
   {
      if ( g_bthHci[i] != NULL )
      {
         delete g_bthHci[i];
         g_bthHci[i] = NULL;
      }

      if ( g_bthHw[i] != NULL )
      {
         delete g_bthHw[i];
         g_bthHw[i] = NULL;
      }

      if ( g_bthDevInfo[i] != NULL )
      {
         EnterCriticalSection( &g_addCritSection );
         delete g_bthDevInfo[i];
         g_bthDevInfo[i] = NULL;
         LeaveCriticalSection( &g_addCritSection );
      }
   }

   LeaveCriticalSection( &g_hciCritSection );
}
