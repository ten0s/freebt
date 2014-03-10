# Microsoft Developer Studio Project File - Name="fbtusb" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=fbtusb - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "fbtusb.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "fbtusb.mak" CFG="fbtusb - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "fbtusb - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "fbtusb - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "fbtusb - Win32 Release"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\objfre\i386"
# PROP Intermediate_Dir ".\objfre\i386"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /Gz /W3 /WX /Oy /Gy /I "." /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DDKROOT)\private\ntos\inc" /I "..\..\include" /FI"warning.h" /D _X86_=1 /D i386=1 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=1 /D "WMI_SUPPORT" /D "USB2" /Zel -cbstring /GF /Oxs       /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "$(CRT_INC_PATH)" /d "NDEBUG"
# ADD RSC /l 0x409 /i "$(CRT_INC_PATH)" /i "$(DDKROOT)\private\ntos\inc" /i "..\..\include" /d "NDEBUG" /d "WMI_SUPPORT" /d "USB2"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 ntoskrnl.lib hal.lib wmilib.lib  hidclass.lib   usbd.lib   ntoskrnl.lib  /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /map /machine:IX86 /nodefaultlib /out:"objfre\i386\fbtusb.sys" /libpath:"$(TARGET_LIB_PATH)" /driver /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text  /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.00 /subsystem:native,1.10  /debug:MINIMAL
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Generating .SYM file
PostBuild_Cmds=mapsym -o .\objfre\i386\fbtusb.sym .\objfre\i386\fbtusb.map
# End Special Build Tool

!ELSEIF  "$(CFG)" == "fbtusb - Win32 Debug"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\objchk\i386"
# PROP Intermediate_Dir ".\objchk\i386"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /Gz /ML /W3 /WX /Zi /Oi /Gy /I "." /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DDKROOT)\private\ntos\inc" /I "..\..\include" /FI"warning.h" /D _X86_=1 /D i386=1 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D DBG=1 /D FPO=0 /D "WMI_SUPPORT" /D "USB2" /Fd".\objchk\i386\fbtusb.pdb" /Zel -cbstring /GF        /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "$(CRT_INC_PATH)" /d "_DEBUG"
# ADD RSC /l 0x409 /i "$(CRT_INC_PATH)" /i "$(DDKROOT)\private\ntos\inc" /i "..\..\include" /d "_DEBUG" /d "WMI_SUPPORT" /d "USB2"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 ntoskrnl.lib hal.lib wmilib.lib  hidclass.lib   usbd.lib   ntoskrnl.lib  /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /incremental:no /map /machine:IX86 /nodefaultlib /out:".\objchk\i386\fbtusb.sys" /libpath:"$(TARGET_LIB_PATH)" /driver /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text  /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.00 /subsystem:native,1.10  /debug:FULL
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Generating SoftICE Symbol file fbtusb.nms
PostBuild_Cmds=C:\DEVELO~1\SOFTIC~1\SoftICE\nmsym.exe /translate:source,package,always .\objchk\i386\fbtusb.sys	mapsym -o .\objchk\i386\fbtusb.sym .\objchk\i386\fbtusb.map
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "fbtusb - Win32 Release"
# Name "fbtusb - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ".c;.cpp"
# Begin Source File

SOURCE=.\fbtdev.c
DEP_CPP_FBTDE=\
	"..\..\include\btdefs.h"\
	"..\..\include\HCIErrors.h"\
	"..\fbtusr.h"\
	".\fbtdev.h"\
	".\fbtpnp.h"\
	".\fbtpwr.h"\
	".\fbtrwr.h"\
	".\fbtusb.h"\
	".\fbtwmi.h"\
	"C:\WINDDK\2600.1106\inc\crt\basetsd.h"\
	"C:\WINDDK\2600.1106\inc\ddk\wdm\wxp\usbdlib.h"\
	"C:\WINDDK\2600.1106\inc\ddk\wdm\wxp\wdm.h"\
	"C:\WINDDK\2600.1106\inc\ddk\wdm\wxp\wmilib.h"\
	"C:\WINDDK\2600.1106\inc\ddk\wxp\mce.h"\
	"C:\WINDDK\2600.1106\inc\wxp\bugcodes.h"\
	"C:\WINDDK\2600.1106\inc\wxp\evntrace.h"\
	"C:\WINDDK\2600.1106\inc\wxp\guiddef.h"\
	"C:\WINDDK\2600.1106\inc\wxp\ia64reg.h"\
	"C:\WINDDK\2600.1106\inc\wxp\ntdef.h"\
	"C:\WINDDK\2600.1106\inc\wxp\ntiologc.h"\
	"C:\WINDDK\2600.1106\inc\wxp\ntstatus.h"\
	"C:\WINDDK\2600.1106\inc\wxp\usb100.h"\
	"C:\WINDDK\2600.1106\inc\wxp\usbdi.h"\
	"C:\WINDDK\2600.1106\inc\wxp\usbioctl.h"\
	"C:\WINDDK\2600.1106\inc\wxp\usbiodef.h"\
	"C:\WINDDK\2600.1106\inc\wxp\wmistr.h"\
	
# End Source File
# Begin Source File

SOURCE=.\fbtpnp.c
DEP_CPP_FBTPN=\
	"..\fbtusr.h"\
	".\fbtdev.h"\
	".\fbtpnp.h"\
	".\fbtpwr.h"\
	".\fbtrwr.h"\
	".\fbtusb.h"\
	".\fbtwmi.h"\
	"C:\WINDDK\2600.1106\inc\crt\basetsd.h"\
	"C:\WINDDK\2600.1106\inc\ddk\wdm\wxp\usbdlib.h"\
	"C:\WINDDK\2600.1106\inc\ddk\wdm\wxp\wdm.h"\
	"C:\WINDDK\2600.1106\inc\ddk\wdm\wxp\wmilib.h"\
	"C:\WINDDK\2600.1106\inc\ddk\wxp\mce.h"\
	"C:\WINDDK\2600.1106\inc\wxp\bugcodes.h"\
	"C:\WINDDK\2600.1106\inc\wxp\evntrace.h"\
	"C:\WINDDK\2600.1106\inc\wxp\guiddef.h"\
	"C:\WINDDK\2600.1106\inc\wxp\ia64reg.h"\
	"C:\WINDDK\2600.1106\inc\wxp\ntdef.h"\
	"C:\WINDDK\2600.1106\inc\wxp\ntiologc.h"\
	"C:\WINDDK\2600.1106\inc\wxp\ntstatus.h"\
	"C:\WINDDK\2600.1106\inc\wxp\usb100.h"\
	"C:\WINDDK\2600.1106\inc\wxp\usbdi.h"\
	"C:\WINDDK\2600.1106\inc\wxp\usbioctl.h"\
	"C:\WINDDK\2600.1106\inc\wxp\usbiodef.h"\
	"C:\WINDDK\2600.1106\inc\wxp\wmistr.h"\
	
# End Source File
# Begin Source File

SOURCE=.\fbtpwr.c
DEP_CPP_FBTPW=\
	"..\fbtusr.h"\
	".\fbtdev.h"\
	".\fbtpnp.h"\
	".\fbtpwr.h"\
	".\fbtrwr.h"\
	".\fbtusb.h"\
	".\fbtwmi.h"\
	"C:\WINDDK\2600.1106\inc\crt\basetsd.h"\
	"C:\WINDDK\2600.1106\inc\ddk\wdm\wxp\usbdlib.h"\
	"C:\WINDDK\2600.1106\inc\ddk\wdm\wxp\wdm.h"\
	"C:\WINDDK\2600.1106\inc\ddk\wdm\wxp\wmilib.h"\
	"C:\WINDDK\2600.1106\inc\ddk\wxp\mce.h"\
	"C:\WINDDK\2600.1106\inc\wxp\bugcodes.h"\
	"C:\WINDDK\2600.1106\inc\wxp\evntrace.h"\
	"C:\WINDDK\2600.1106\inc\wxp\guiddef.h"\
	"C:\WINDDK\2600.1106\inc\wxp\ia64reg.h"\
	"C:\WINDDK\2600.1106\inc\wxp\ntdef.h"\
	"C:\WINDDK\2600.1106\inc\wxp\ntiologc.h"\
	"C:\WINDDK\2600.1106\inc\wxp\ntstatus.h"\
	"C:\WINDDK\2600.1106\inc\wxp\usb100.h"\
	"C:\WINDDK\2600.1106\inc\wxp\usbdi.h"\
	"C:\WINDDK\2600.1106\inc\wxp\usbioctl.h"\
	"C:\WINDDK\2600.1106\inc\wxp\usbiodef.h"\
	"C:\WINDDK\2600.1106\inc\wxp\wmistr.h"\
	
# End Source File
# Begin Source File

SOURCE=.\fbtrwr.c
DEP_CPP_FBTRW=\
	"..\fbtusr.h"\
	".\fbtdev.h"\
	".\fbtpnp.h"\
	".\fbtpwr.h"\
	".\fbtrwr.h"\
	".\fbtusb.h"\
	".\fbtwmi.h"\
	"C:\WINDDK\2600.1106\inc\crt\basetsd.h"\
	"C:\WINDDK\2600.1106\inc\ddk\wdm\wxp\usbdlib.h"\
	"C:\WINDDK\2600.1106\inc\ddk\wdm\wxp\wdm.h"\
	"C:\WINDDK\2600.1106\inc\ddk\wdm\wxp\wmilib.h"\
	"C:\WINDDK\2600.1106\inc\ddk\wxp\mce.h"\
	"C:\WINDDK\2600.1106\inc\wxp\bugcodes.h"\
	"C:\WINDDK\2600.1106\inc\wxp\evntrace.h"\
	"C:\WINDDK\2600.1106\inc\wxp\guiddef.h"\
	"C:\WINDDK\2600.1106\inc\wxp\ia64reg.h"\
	"C:\WINDDK\2600.1106\inc\wxp\ntdef.h"\
	"C:\WINDDK\2600.1106\inc\wxp\ntiologc.h"\
	"C:\WINDDK\2600.1106\inc\wxp\ntstatus.h"\
	"C:\WINDDK\2600.1106\inc\wxp\usb100.h"\
	"C:\WINDDK\2600.1106\inc\wxp\usbdi.h"\
	"C:\WINDDK\2600.1106\inc\wxp\usbioctl.h"\
	"C:\WINDDK\2600.1106\inc\wxp\usbiodef.h"\
	"C:\WINDDK\2600.1106\inc\wxp\wmistr.h"\
	
# End Source File
# Begin Source File

SOURCE=.\fbtusb.c
DEP_CPP_FBTUS=\
	"..\fbtusr.h"\
	".\fbtdev.h"\
	".\fbtpnp.h"\
	".\fbtpwr.h"\
	".\fbtrwr.h"\
	".\fbtusb.h"\
	".\fbtwmi.h"\
	"C:\WINDDK\2600.1106\inc\crt\basetsd.h"\
	"C:\WINDDK\2600.1106\inc\ddk\wdm\wxp\usbdlib.h"\
	"C:\WINDDK\2600.1106\inc\ddk\wdm\wxp\wdm.h"\
	"C:\WINDDK\2600.1106\inc\ddk\wdm\wxp\wmilib.h"\
	"C:\WINDDK\2600.1106\inc\ddk\wxp\mce.h"\
	"C:\WINDDK\2600.1106\inc\wxp\bugcodes.h"\
	"C:\WINDDK\2600.1106\inc\wxp\evntrace.h"\
	"C:\WINDDK\2600.1106\inc\wxp\guiddef.h"\
	"C:\WINDDK\2600.1106\inc\wxp\ia64reg.h"\
	"C:\WINDDK\2600.1106\inc\wxp\ntdef.h"\
	"C:\WINDDK\2600.1106\inc\wxp\ntiologc.h"\
	"C:\WINDDK\2600.1106\inc\wxp\ntstatus.h"\
	"C:\WINDDK\2600.1106\inc\wxp\usb100.h"\
	"C:\WINDDK\2600.1106\inc\wxp\usbdi.h"\
	"C:\WINDDK\2600.1106\inc\wxp\usbioctl.h"\
	"C:\WINDDK\2600.1106\inc\wxp\usbiodef.h"\
	"C:\WINDDK\2600.1106\inc\wxp\wmistr.h"\
	
# End Source File
# Begin Source File

SOURCE=.\fbtwmi.c
DEP_CPP_FBTWM=\
	"..\fbtusr.h"\
	".\fbtdev.h"\
	".\fbtpnp.h"\
	".\fbtpwr.h"\
	".\fbtrwr.h"\
	".\fbtusb.h"\
	".\fbtwmi.h"\
	"C:\WINDDK\2600.1106\inc\crt\basetsd.h"\
	"C:\WINDDK\2600.1106\inc\ddk\wdm\wxp\usbdlib.h"\
	"C:\WINDDK\2600.1106\inc\ddk\wdm\wxp\wdm.h"\
	"C:\WINDDK\2600.1106\inc\ddk\wdm\wxp\wmilib.h"\
	"C:\WINDDK\2600.1106\inc\ddk\wxp\mce.h"\
	"C:\WINDDK\2600.1106\inc\wxp\bugcodes.h"\
	"C:\WINDDK\2600.1106\inc\wxp\evntrace.h"\
	"C:\WINDDK\2600.1106\inc\wxp\guiddef.h"\
	"C:\WINDDK\2600.1106\inc\wxp\ia64reg.h"\
	"C:\WINDDK\2600.1106\inc\wxp\ntdef.h"\
	"C:\WINDDK\2600.1106\inc\wxp\ntiologc.h"\
	"C:\WINDDK\2600.1106\inc\wxp\ntstatus.h"\
	"C:\WINDDK\2600.1106\inc\wxp\usb100.h"\
	"C:\WINDDK\2600.1106\inc\wxp\usbdi.h"\
	"C:\WINDDK\2600.1106\inc\wxp\usbioctl.h"\
	"C:\WINDDK\2600.1106\inc\wxp\usbiodef.h"\
	"C:\WINDDK\2600.1106\inc\wxp\wmistr.h"\
	
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=.\fbtdev.h
# End Source File
# Begin Source File

SOURCE=.\fbtpnp.h
# End Source File
# Begin Source File

SOURCE=.\fbtpwr.h
# End Source File
# Begin Source File

SOURCE=.\fbtrwr.h
# End Source File
# Begin Source File

SOURCE=.\fbtusb.h
# End Source File
# Begin Source File

SOURCE=.\fbtwmi.h
# End Source File
# Begin Source File

SOURCE=.\usbutil.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ".rc;.mc;.mof"
# Begin Source File

SOURCE=.\fbtusb.rc
# End Source File
# End Group
# End Target
# End Project
