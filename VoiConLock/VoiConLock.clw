; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CVoiConLockDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "VoiConLock.h"

ClassCount=3
Class1=CVoiConLockApp
Class2=CVoiConLockDlg

ResourceCount=3
Resource2=IDD_CALENDAR (English (U.S.))
Resource1=IDR_MAINFRAME
Class3=CCalendarDlg
Resource3=IDD_VOICONLOCK_DIALOG (English (U.S.))

[CLS:CVoiConLockApp]
Type=0
HeaderFile=VoiConLock.h
ImplementationFile=VoiConLock.cpp
Filter=N

[CLS:CVoiConLockDlg]
Type=0
HeaderFile=VoiConLockDlg.h
ImplementationFile=VoiConLockDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=CVoiConLockDlg



[DLG:IDD_VOICONLOCK_DIALOG (English (U.S.))]
Type=1
Class=CVoiConLockDlg
ControlCount=17
Control1=IDOK,button,1342242817
Control2=IDC_COMPANY,edit,1350631552
Control3=IDC_USERNUMBER,edit,1350631552
Control4=IDC_STATIC,static,1342308864
Control5=IDC_STATIC,static,1342308864
Control6=IDC_TIME,edit,1350631552
Control7=IDC_GENERATE,button,1342242817
Control8=IDC_TIME_BTN,button,1342242816
Control9=IDC_LOAD,button,1342242817
Control10=IDC_STATIC,button,1342177287
Control11=IDC_ED_HOST_MAC,edit,1350631552
Control12=IDC_MAC_VERIFY,button,1342242817
Control13=IDC_STATIC,static,1342308864
Control14=IDC_ED_KEY_MAC2,edit,1484849280
Control15=IDC_STATIC,static,1342308864
Control16=IDC_SET,button,1342242817
Control17=IDC_BTN_SELF,button,1342242816

[DLG:IDD_CALENDAR (English (U.S.))]
Type=1
Class=CCalendarDlg
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_MONTHCALENDAR1,SysMonthCal32,1342242816

[CLS:CCalendarDlg]
Type=0
HeaderFile=CalendarDlg.h
ImplementationFile=CalendarDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_MONTHCALENDAR1

