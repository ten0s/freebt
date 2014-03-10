// Copyright (C) 2008-2009 Dmitry Klionsky aka ten0s <dm.klionsky@gmail.com>

// Use of this file is subject to the terms
// described in the LICENSE.TXT file that
// accompanies this file.
//
// Your use of this file indicates your
// acceptance of the terms described in
// LICENSE.TXT.

using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.Threading;

namespace fbtrtsharp
{
    class Program
    {
        private static HciEventListenerDelegate hciEventListener = null;

        enum W32_ERROR
        {
            ERROR_SUCCESS = 0,
            ERROR_DEVICE_IN_USE = 2404,
            ERROR_DEVICE_NOT_AVAILABLE = 4319
        }

        enum HCI_TYPE
        {
            COMMAND_PACKET = 1,
            DATA_PACKET_ACL = 2,
            DATA_PACKET_SCO = 3,
            EVENT_PACKET = 4,
            ETYPE_FINISH = 5
        };

        [StructLayout(LayoutKind.Sequential)]
        public struct BT_ADDR
        {
            public byte btAddr0;
            public byte btAddr1;
            public byte btAddr2;
            public byte btAddr3;
            public byte btAddr4;
            public byte btAddr5;
        };

        [StructLayout(LayoutKind.Sequential)]
        public struct DEVICE_INFO
        {
            public BT_ADDR btAddr;
            public byte hciVersion;
            public ushort hciRevision;
            public byte lmpVersion;
            public ushort manufacturer;
            public ushort lmpSubVersion;
        };

        [DllImport("kernel32.dll")]
        public static extern int GetLastError();

        [DllImport("fbtrt.dll", SetLastError = true)]
        public static extern int OpenDevice();

        [DllImport("fbtrt.dll", SetLastError = true)]
        public static extern int CloseDevice(int devId);

        [DllImport("fbtrt.dll", SetLastError = true)]
        public static extern int SendHCICommand(int devId, [MarshalAs(UnmanagedType.LPArray)] byte[] cmdBuf, uint cmdLen);

        [DllImport("fbtrt.dll", SetLastError = true)]
        public static extern int GetDeviceInfo(int devId, ref DEVICE_INFO devInfo);

        [DllImport("fbtrt.dll", SetLastError = true)]
        public static extern int GetManufacturerName(ushort manufacturer, IntPtr pBuffer, uint bufferLen);

        public delegate int HciEventListenerDelegate(IntPtr pEventBuf, uint eventLen);

        [DllImport("fbtrt.dll", SetLastError = true)]
        public static extern int SubscribeHCIEvent(int devId, HciEventListenerDelegate hciEventListener);

        [DllImport("fbtrt.dll", SetLastError = true, CharSet = CharSet.Unicode)]
        public static extern int SetLogFileName(string fileName);

        [DllImport("fbtrt.dll", SetLastError = true)]
        public static extern int SetLogLevel(int level);


        static void Main( string[] args )
        {
            SetLogFileName("fbtrt.log");
            SetLogLevel(255);

            int devId = OpenDevice();
            if ( devId == -1 )
            {
                W32_ERROR error = (W32_ERROR)GetLastError();
                if ( W32_ERROR.ERROR_DEVICE_IN_USE == error )
                {
                    Console.WriteLine( "ERROR_DEVICE_IN_USE" );
                }
                else if ( W32_ERROR.ERROR_DEVICE_NOT_AVAILABLE == error )
                {
                    Console.WriteLine("ERROR_DEVICE_NOT_AVAILABLE" );
                }
            }
            else
            {
                hciEventListener = new HciEventListenerDelegate( OnHciEvent );
                SubscribeHCIEvent( devId, hciEventListener );

                //Console.WriteLine("Enter HCI command (empty for exit):");
                //for (;;)
                //{
                  //  string cmd = Console.ReadLine();
                  //  if (cmd.Length == 0) break;
                //}

                byte[] reset = new byte[] { 0x01, 0x03, 0x0c, 0x00 };
                //byte[] connect = new byte[] { 0x05, 0x04, 0x0d, 0xf7, 0x00, 0xfd, 0x76, 0x02, 0x00, 0x18, 0xcc, 0x01, 0x00, 0x00, 0x00, 0x01 };

                //byte[] acl = new byte[] { 0x01, 0x20, 0x0c, 0x00, 0x08, 0x00, 0x01, 0x00, 0x02, 0x0a, 0x04, 0x00, 0x01, 0x00, 0x43, 0x00 };

                byte[] buffer = reset;
                int ret = SendHCICommand(devId, buffer, (uint)buffer.Length);
                string result = ( ret != 0 ) ? "OK" : "Fail";

                string packetType = "COMMAND_PACKET";
                string packetData = BytesToHex(buffer, buffer.Length);
                Console.WriteLine( string.Format( "{0}: {1} {2}", packetType, packetData, result ) );

                Thread.Sleep(1000);

                int size = 256;
                IntPtr hglobal = Marshal.AllocHGlobal(size);

                for (ushort i = ushort.MinValue; i < ushort.MaxValue; ++i)
                {
                    ret = GetManufacturerName(i, hglobal, (uint)size);
                    if (ret == 1)
                    {
                        string manufacturer = Marshal.PtrToStringUni(hglobal);
                        Console.WriteLine(manufacturer);
                    }
                    else
                    {
                        break;
                    }
                }

                Marshal.FreeHGlobal(hglobal);

                Console.WriteLine("Press any key...");
                Console.ReadLine();

                /*
                uint sent = 0;
                buffer = acl;
                ret = SendData(buffer, (uint)buffer.Length, ref sent);
                result = (ret != 0) ? "OK" : "Fail";
                packetType = "ACL_PACKET";
                packetData = BytesToHex(buffer, buffer.Length);
                Console.WriteLine(string.Format("{0}: {1} {2}", packetType, packetData, result));

                Console.ReadLine();

                byte[] bigBuffer = new byte[1024];
                uint readed = 0;
                ret = GetData(bigBuffer, (uint)bigBuffer.Length, ref readed);
                result = (ret != 0) ? "OK" : "Fail";
                packetType = "Get Data";
                packetData = BytesToHex(bigBuffer, (int)readed);
                Console.WriteLine(string.Format("{0}: {1} {2}", packetType, packetData, result));
                 */

                CloseDevice(devId);
            }
        }

        public static int OnHciEvent( IntPtr pEventBuffer, uint dwEventLength )
        {
            byte[] eventBuffer = new byte[dwEventLength];
            Marshal.Copy( pEventBuffer, eventBuffer, 0, (int)dwEventLength );

            string packetType = "EVENT_PACKET";
            string result = "OK";

            string packetData = BytesToHex( eventBuffer, (int)dwEventLength );
            Console.WriteLine( string.Format( "{0}: {1} {2}", packetType, packetData, result ) );

            byte[] buf = new byte[dwEventLength + 1];
            buf[0] = (byte)HCI_TYPE.EVENT_PACKET;
            for ( int i = 1; i < dwEventLength; ++i )
            {
                buf[i] = eventBuffer[i - 1];
            }

            return (int)W32_ERROR.ERROR_SUCCESS;
        }


        public static string BytesToHex(byte[] bytes, int length)
        {
            StringBuilder hexString = new StringBuilder(length);
            for (int i = 0; i < length; i++)
            {
                hexString.Append(bytes[i].ToString("x2"));
            }
            return hexString.ToString();
        }
    }
}
