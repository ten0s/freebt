// Copyright (C) 2008-2009 Dmitry Klionsky aka ten0s <dm.klionsky@gmail.com>

// Use of this file is subject to the terms
// described in the LICENSE.TXT file that
// accompanies this file.
//
// Your use of this file indicates your
// acceptance of the terms described in
// LICENSE.TXT.

#ifndef __BTH_EMUL_HCI_H__
#define __BTH_EMUL_HCI_H__

#include <windows.h>
#include "fbthci.h"           // CHci
#include "fbtrt.h"            // HCI_EVENT_LISTENER

struct DEVICE_INFO : public LOCAL_DEVICE_INFO
{
   unsigned short acl_mtu;
   unsigned char sco_mtu;
   unsigned short acl_max_pkt;
   unsigned short sco_max_pkt;
};

class CBthEmulHci : public CHci
{
public:
   CBthEmulHci( CBTHW& btHw );
   virtual ~CBthEmulHci();

public: // from CHci
   virtual DWORD StartEventListener();
   virtual DWORD StopEventListener();
   virtual DWORD OnEvent( PFBT_HCI_EVENT_HEADER pEvent, DWORD dwLength );

public:
   DWORD Attach( LPCTSTR szDeviceName );
   DWORD Detach();
   HANDLE GetDriverHandle() const;
   BOOL IsAttached() const;
   DWORD SendHCICommand( const BYTE* lpBuffer, DWORD dwBufferSize );
   BOOL SubscribeHCIEvent( HCI_EVENT_LISTENER hciEventListener );
   BOOL GetDeviceInfo( DEVICE_INFO* pDevInfo );

private:
   static DWORD WINAPI DataReader( LPVOID lpParam );
   static DWORD WINAPI DataEventHandler( LPVOID lpParam );
   static CRITICAL_SECTION s_criticalSection;

private:
   DWORD SendCommand( DWORD dwCommand, LPCVOID lpInBuffer = NULL, DWORD dwInBufferSize = 0, LPVOID lpOutBuffer = NULL, DWORD dwOutBufferSize = 0, OVERLAPPED* pOverlapped = NULL );
   DWORD	SendData( LPCVOID lpBuffer, DWORD dwBufferSize, DWORD* dwBytesSent, OVERLAPPED* pOverlapped );

private:
   CBTHW& m_btHw;
   HCI_EVENT_LISTENER m_hciEventListener;
   HANDLE m_hReaderThread;
   HANDLE m_hStopReadingEvent;
   HANDLE m_hReaderReadyEvent;
   DEVICE_INFO m_devInfo;
};

#endif //__BTH_EMUL_HCI_H__
