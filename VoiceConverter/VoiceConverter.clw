; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CAudioDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "audio.h"
LastPage=0

ClassCount=4
Class1=CAudioApp
Class2=CAboutDlg
Class3=CAudioDlg
Class4=CProgressDlg

ResourceCount=6
Resource1=IDD_ABOUTBOX (English (U.S.))
Resource2=IDR_MENU_WAVE_CON (English (U.S.))
Resource3=IDR_MAINFRAME (English (U.S.))
Resource4=IDD_AUDIO_DIALOG (English (U.S.))
Resource5=IDD_PROGRESS (English (U.S.))
Resource6=IDR_MENU_CON (English (U.S.))

[CLS:CAudioApp]
Type=0
BaseClass=CWinApp
HeaderFile=Audio.h
ImplementationFile=Audio.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=AudioDlg.cpp
ImplementationFile=AudioDlg.cpp
LastObject=IDM_G7231_WAV

[CLS:CAudioDlg]
Type=0
BaseClass=CDialog
HeaderFile=AudioDlg.h
ImplementationFile=AudioDlg.cpp
Filter=D
VirtualFilter=dWC
LastObject=CAudioDlg

[CLS:CProgressDlg]
Type=0
BaseClass=CDialog
HeaderFile=ProgressDlg.h
ImplementationFile=ProgressDlg.cpp

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg

[DLG:IDD_AUDIO_DIALOG]
Type=1
Class=CAudioDlg

[DLG:IDD_PROGRESS]
Type=1
Class=CProgressDlg

[DLG:IDD_AUDIO_DIALOG (English (U.S.))]
Type=1
Class=?
ControlCount=4
Control1=IDC_CB_DRIVE,combobox,1344340226
Control2=IDC_ED_PATH,edit,1350633600
Control3=IDC_FILE,SysListView32,1350631425
Control4=IDC_LIST_PATH,listbox,1352728835

[MNU:IDR_MENU_WAVE_CON (English (U.S.))]
Type=1
Class=?
Command1=ID_MENU_W_7231_R
Command2=ID_MENU_W_711U_R
Command3=ID_MENU_W_711A_R
Command4=IDM_G729A
Command5=IDM_G7231_WAV
Command6=IDM_DELETE
CommandCount=6

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=?
ControlCount=5
Control1=IDC_STATIC,static,1342308480
Control2=IDC_STATIC,static,1342308352
Control3=IDOK,button,1342373889
Control4=IDC_STATIC,static,1342177283
Control5=IDC_VERSION,static,1342308864

[DLG:IDD_PROGRESS (English (U.S.))]
Type=1
Class=?
ControlCount=2
Control1=IDC_PROGRESS,msctls_progress32,1350565888
Control2=IDC_FNAME,static,1342312448

[MNU:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=IDM_EXIT
Command2=IDM_ALL
Command3=IDM_REFRESH
Command4=IDM_ABOUT
CommandCount=4

[MNU:IDR_MENU_CON (English (U.S.))]
Type=1
Class=?
Command1=IDM_DELETE
Command2=IDM_REFRESH
CommandCount=2

