; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CVoiceDecoderDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "VoiceDecoder.h"

ClassCount=3
Class1=CVoiceDecoderApp
Class2=CVoiceDecoderDlg
Class3=CAboutDlg

ResourceCount=5
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_VOICEDECODER_DIALOG
Resource4=IDD_ABOUTBOX (English (U.S.))
Resource5=IDD_VOICEDECODER_DIALOG (English (U.S.))

[CLS:CVoiceDecoderApp]
Type=0
HeaderFile=VoiceDecoder.h
ImplementationFile=VoiceDecoder.cpp
Filter=N

[CLS:CVoiceDecoderDlg]
Type=0
HeaderFile=VoiceDecoderDlg.h
ImplementationFile=VoiceDecoderDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=IDC_BTN_RESETTITLE

[CLS:CAboutDlg]
Type=0
HeaderFile=VoiceDecoderDlg.h
ImplementationFile=VoiceDecoderDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Class=CAboutDlg


[DLG:IDD_VOICEDECODER_DIALOG]
Type=1
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Class=CVoiceDecoderDlg

[DLG:IDD_VOICEDECODER_DIALOG (English (U.S.))]
Type=1
Class=CVoiceDecoderDlg
ControlCount=9
Control1=IDOK,button,1342242817
Control2=IDC_ED_FILENAME,edit,1350631552
Control3=IDC_BTN_OPEN,button,1342242816
Control4=IDC_BTN_DECODE,button,1342242816
Control5=IDC_ED_FILENAME2,edit,1350631552
Control6=IDC_BTN_OPEN2,button,1342242816
Control7=IDC_BTN_MIX,button,1342242816
Control8=IDC_BTN_ABOUT,button,1342242816
Control9=IDC_BTN_RESETTITLE,button,1342242816

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=?
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

