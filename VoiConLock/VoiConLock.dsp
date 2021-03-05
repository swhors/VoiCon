# Microsoft Developer Studio Project File - Name="VoiConLock" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=VoiConLock - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "VoiConLock.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "VoiConLock.mak" CFG="VoiConLock - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "VoiConLock - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "VoiConLock - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "VoiConLock - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "././" /I "./inc" /I "../common/include" /I "../SecureLibrary" /I "../sdk_library" /I "../ConfManage/inc" /I "../mf_1_4_2_1/inc/asn1" /I "../mf_1_4_2_1/inc/common" /I "../mf_1_4_2_1/inc/h245" /I "../mf_1_4_2_1/inc/h323" /I "../mf_1_4_2_1/inc/ipp" /I "../mf_1_4_2_1/inc/rtp" /I "../mf_1_4_2_1/inc/sdp" /I "../mf_1_4_2_1/netutil" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 IPHlpApi.Lib /nologo /subsystem:windows /machine:I386 /out:"../rbin4/BISLock.exe" /libpath:"../common/library" /libpath:"../sdk_library/" /libpath:"../mf_1_4_2_1/lib"

!ELSEIF  "$(CFG)" == "VoiConLock - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "././" /I "./inc" /I "../common/include" /I "../SecureLibrary" /I "../sdk_library" /I "../ConfManage/inc" /I "../mf_1_4_2_1/inc/asn1" /I "../mf_1_4_2_1/inc/common" /I "../mf_1_4_2_1/inc/h245" /I "../mf_1_4_2_1/inc/h323" /I "../mf_1_4_2_1/inc/ipp" /I "../mf_1_4_2_1/inc/rtp" /I "../mf_1_4_2_1/inc/sdp" /I "../mf_1_4_2_1/netutil" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /fo"Debug/BISLock.res" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 IPHlpApi.Lib /nologo /subsystem:windows /debug /machine:I386 /out:"../bin4/BISLock.exe" /pdbtype:sept /libpath:"../common/library" /libpath:"../sdk_library/" /libpath:"../mf_1_4_2_1/lib"

!ENDIF 

# Begin Target

# Name "VoiConLock - Win32 Release"
# Name "VoiConLock - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CalendarDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\VoiConLock.cpp
# End Source File
# Begin Source File

SOURCE=.\VoiConLockDlg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CalendarDlg.h
# End Source File
# Begin Source File

SOURCE=..\common\include\KeyLockCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\VoiConLock.h
# End Source File
# Begin Source File

SOURCE=.\VoiConLockDlg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\key_l.ico
# End Source File
# Begin Source File

SOURCE=.\res\VoiConLock.ico
# End Source File
# Begin Source File

SOURCE=.\VoiConLock.rc
# End Source File
# Begin Source File

SOURCE=.\res\VoiConLock.rc2
# End Source File
# End Group
# Begin Group "dll_src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\common\src\BlowFish.cpp
# End Source File
# Begin Source File

SOURCE=..\common\src\CryptoDsit.cpp
# End Source File
# Begin Source File

SOURCE=..\common\src\GetMac.cpp
# End Source File
# Begin Source File

SOURCE=..\common\src\KeyLockCtrl.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
