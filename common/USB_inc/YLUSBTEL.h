// YLUSBTEL.h : main header file for the YLUSBTEL DLL
//

#if !defined(AFX_YLUSBTEL_H__AE40BE2D_0B84_4FAB_984A_51ADC44D6187__INCLUDED_)
#define AFX_YLUSBTEL_H__AE40BE2D_0B84_4FAB_984A_51ADC44D6187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#define	BUZZERDATA_LENGTH			0x7F
/////////////////////////////////////////////////////////////////////////////
// CYLUSBTELApp
// See YLUSBTEL.cpp for the implementation of this class
//

class CYLUSBTELApp : public CWinApp
{
public:
	CYLUSBTELApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CYLUSBTELApp)
	public:
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CYLUSBTELApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


BOOL YL_DeviceIoControl(DWORD dwIoControlCode, 
		LPVOID	lpInBuffer = NULL,	DWORD nInBufferSize = 0, 
		LPVOID	lpOutBuffer = NULL,	DWORD nOutBufferSize = 0, 
		LPDWORD	lpBytesReturned = NULL);

/*
Parameters: 
dwIoControlCode 
	[in] Specifies the control code for the operation. This value identifies the specific operation to be performed and the type of device on which the operation is to be performed. No specific values are defined for the dwIoControlCode parameter. However, the writer of a custom device driver can define IOCTL_XXXX control codes, per the CTL_CODE macro. These control codes can then be advertised, and an application can use these control codes with DeviceIoControl to perform the driver-specific functions. 
lpInBuffer 
	[in] Long pointer to a buffer that contains the data required to perform the operation. 
	This parameter can be NULL if the dwIoControlCode parameter specifies an operation that does not require input data. 

nInBufferSize 
	[in] Size, in bytes, of the buffer pointed to by lpInBuffer. 
lpOutBuffer 
	[out] Long pointer to a buffer that receives the operation’s output data. 
	This parameter can be NULL if the dwIoControlCode parameter specifies an operation that does not produce output data. 

nOutBufferSize 
	[in] Size, in bytes, of the buffer pointed to by lpOutBuffer. 
lpBytesReturned 
	[out] Long pointer to a variable that receives the size, in bytes, of the data stored into the buffer pointed to by lpOutBuffer. 
	The lpBytesReturned parameter cannot be NULL. Even when an operation produces no output data, and lpOutBuffer can be NULL, the DeviceIoControl function makes use of the variable pointed to by lpBytesReturned. After such an operation, the value of the variable is without meaning. 

Return Values
	Nonzero indicates success. Zero indicates failure. To get extended error information, see as below.

Remarks
*/

#define		YL_IOCTL_OPEN_DEVICE				0x0000
enum {
	YL_CALLBACK_MSG_WARNING = -2,
	YL_CALLBACK_MSG_ERROR = -1,
	YL_CALLBACK_MSG_USBPHONE_VERSION,
	YL_CALLBACK_MSG_USBPHONE_SERIALNO,

	YL_CALLBACK_MSG_DIGIT_PRESSED,
	YL_CALLBACK_MSG_KEYBUF_CHANGED,
	YL_CALLBACK_MSG_KEYDOWN,
	YL_CALLBACK_MSG_KEYUP,
	YL_CALLBACK_MSG_OFFHOOK,
	YL_CALLBACK_MSG_HANGUP,
	YL_CALLBACK_MSG_HANDSET_OFFHOOK,
	YL_CALLBACK_MSG_HANDSET_HANGUP,

	//For General Control Function
	YL_CALLBACK_GEN_KEYBUF_CHANGED,
	YL_CALLBACK_GEN_KEYDOWN,
	YL_CALLBACK_GEN_OFFHOOK,
	YL_CALLBACK_GEN_HANGUP,
	YL_CALLBACK_GEN_INUSB,
	YL_CALLBACK_GEN_INPSTN,

	YL_CALLBACK_GEN_CALLERID,
};

//For General Control Function
#define		YL_IOCTL_GEN_READY					0x1001
#define		YL_IOCTL_GEN_UNREADY				0x1002
#define		YL_IOCTL_GEN_CALLIN					0x1003
#define		YL_IOCTL_GEN_CALLOUT				0x1004
#define		YL_IOCTL_GEN_RINGBACK				0x1005
#define		YL_IOCTL_GEN_TALKING				0x1006

#define		YL_IOCTL_GEN_RINGSELECT				0x1008
#define		YL_IOCTL_GEN_GETRINGNO				0x1009

#define		YL_IOCTL_GEN_GOTOUSB				0x1010
#define		YL_IOCTL_GEN_GOTOPSTN				0x1011
#define		YL_IOCTL_GEN_DEFAULTPSTN			0x1012
#define		YL_IOCTL_GEN_DEFAULTUSB				0x1013
#define		YL_IOCTL_GEN_ONLYUSB				0x1014
#define		YL_IOCTL_GEN_GETCHANNEL				0x1015
#define		YL_IOCTL_GEN_GETROUTECODE			0x1016
#define		YL_IOCTL_GEN_SETROUTECODE			0x1017

#define		KEY_0			 0x80
#define		KEY_1			 0x81
#define		KEY_2			 0x82
#define		KEY_3			 0x83
#define		KEY_4			 0x84
#define		KEY_5			 0x85
#define		KEY_6			 0x86
#define		KEY_7			 0x87
#define		KEY_8			 0x88
#define		KEY_9			 0x89
#define		KEY_STAR		 0x8B
#define		KEY_POUND		 0x8C

#define		KEY_SEND		 0x91
#define		KEY_CENTER		 0x92
#define		KEY_HOT			 0x93
#define		KEY_WEB			 0x94
#define		KEY_HELP		 0x95
#define		KEY_SHOP		 0x96
#define		KEY_VDOWN		 0x97
#define		KEY_VUP			 0x98
#define		KEY_STOP		 0xB0
//End of General Control Function
/*
YL_IOCTL_OPEN_DEVICE:	
	Open USB ACR Telephone device.
	lpInBuffer ,nInBufferSize, lpOutBuffer, nOutBufferSize is ignore
Example:
	DWORD BytesReturned;
	DWORD dwIoControlCode = YL_IOCTL_OPEN_DEVICE;
	if(!YL_DeviceIoControl(dwIoControlCode,NULL,0,NULL,0,&BytesReturned))
		return BytesReturned;
		
Extend Function:
	DWORD ParameterOfAppications = (DWORD)this;		//ParameterOfAppications is the parameter of FuncName
	if(!YL_DeviceIoControl(YL_IOCTL_OPEN_DEVICE,FuncName,ParameterOfAppications,NULL,0,&BytesReturned))
		return BytesReturned;
	//the FuncName is the name of callback function
	//and you must declare the callback function and you can do so:

void FuncName(DWORD dwFlags,LPVOID lParam,LPVOID pParam,DWORD ParameterOfAppications)
{
	CMyApplicationName *pThis = (CMyApplicationName *)ParameterOfAppications;
	switch(dwFlags){
	case YL_CALLBACK_MSG_DIGIT_PRESSED: 	//measn have digit button pressed
		{
			DWORD dwButton = (DWORD)lParam;
		}
		break;
	case YL_CALLBACK_MSG_KEYDOWN: 	//measn have digit button pressed
		{
			PYL_STATUS pStatus = (PYL_STATUS)lParam;
		}
		break;
		
		...
	}
	...
}
*/

#define		YL_IOCTL_CLOSE_DEVICE				0xFFFF
/*
YL_IOCTL_CLOSE_DEVICE:	
	Close USB Phone device.
	lpInBuffer ,nInBufferSize, lpOutBuffer, nOutBufferSize is ignore
Example:
	DWORD BytesReturned;
	DWORD dwIoControlCode = YL_IOCTL_CLOSE_DEVICE;
	if(!YL_DeviceIoControl(dwIoControlCode,NULL,0,NULL,0,&BytesReturned))
		return BytesReturned;
*/

//#define		YL_IOCTL_SET_BUZZER					0x0102
typedef struct {
	BYTE	bCount;
	struct {
		WORD	wFreq;			//周期
		WORD	wTime;			//时间(ms)
	}u[BUZZERDATA_LENGTH];
}USBTEL_BUZZER,*PUSBTEL_BUZZER;

#define		YL_IOCTL_OPEN_BUZZER				0x0103
#define		YL_IOCTL_CLOSE_BUZZER				0x0203
#define		YL_IOCTL_RING_BUZZER				0x0303

#define		YL_IOCTL_SHOW_LCD					0x0004
#define		YL_IOCTL_SET_LCD					0x0104
#define		YL_IOCTL_SET_CID_CODE				0x0204
#define		YL_IOCTL_SET_SID_CODE				0x0304
#define		YL_IOCTL_SET_STORE_CODE				0x0404
typedef struct {
	UINT	iStoreID;		//1-199
	CString	sTeleNumber;
}YL_ADDRBOOK,*PYL_ADDRBOOK;

#define		YL_IOCTL_OPEN_AUDIO_CIRCLE			0x0105
#define		YL_IOCTL_CLOSE_AUDIO_CIRCLE			0x0205

#define		YL_IOCTL_OPEN_LED					0x0106
#define		YL_IOCTL_CLOSE_LED					0x0206
#define		YL_IOCTL_FLASH_LED					0x0306
typedef struct {
	UINT	iOpenPeriod;
	UINT	iClosePeriod;
}YL_FLASH_LED,*PYL_FLASH_LED;

#define		YL_IOCTL_OPEN_SIGNAL				0x0107
#define		YL_IOCTL_CLOSE_SIGNAL				0x0207
/*
#define		YL_IOCTL_OPEN_MIC					0x0108
#define		YL_IOCTL_CLOSE_MIC					0x0208
*/
#define		YL_IOCTL_OPEN_SPEAKER				0x0109
#define		YL_IOCTL_CLOSE_SPEAKER				0x0209

#define		YL_IOCTL_SEND_VIRTUAL_KEY			0x0506
#define		YL_IOCTL_LCD_TEST					0x9904

#define		VERSION_P1K		0x0100
#define		VERSION_P2K		0x0200
#define		VERSION_V1K		0x0300
#define		VERSION_B1K		0x0500
#define		VERSION_T1K		0x0600

#define		KEY1_SEND		 0x03
#define		KEY1_END		 0x13
#define		KEY1_CLEAR		 0x14
#define		KEY1_IN			 0x23
#define		KEY1_OUT		 0x04
#define		KEY1_UP			 0x33
#define		KEY1_DOWN		 0x24
#define		KEY1_1			 0x00
#define		KEY1_2			 0x01
#define		KEY1_3			 0x02
#define		KEY1_4			 0x10
#define		KEY1_5			 0x11
#define		KEY1_6			 0x12
#define		KEY1_7			 0x20
#define		KEY1_8			 0x21
#define		KEY1_9			 0x22
#define		KEY1_STAR		 0x30
#define		KEY1_0			 0x31
#define		KEY1_POUND		 0x32

#define		KEY1_CENTER		 0x05
#define		KEY1_VDOWN		 0x15
#define		KEY1_VUP		 0x25
#define		KEY1_WEB		 0x34

#define		KEY2_IN			 0x34
#define		KEY2_OUT		 0x10
#define		KEY2_UP			 0x32
#define		KEY2_DOWN		 0x30
#define		KEY2_DEL		 0x33
#define		KEY2_DIAL		 0x00
#define		KEY2_HOOK		 0x20
#define		KEY2_SEND		 0x25
#define		KEY2_FLASH		 0x15
#define		KEY2_CENTER		 0x05
#define		KEY2_1			 0x21
#define		KEY2_2			 0x11
#define		KEY2_3			 0x01
#define		KEY2_4			 0x22
#define		KEY2_5			 0x12
#define		KEY2_6			 0x02
#define		KEY2_7			 0x23
#define		KEY2_8			 0x13
#define		KEY2_9			 0x03
#define		KEY2_STAR		 0x24
#define		KEY2_0			 0x14
#define		KEY2_POUND		 0x04

#define		KEY2_VDOWN		 0x31
#define		KEY2_VUP		 0x35

enum ID_TYPE{
	ID_LEDTIMER1,
	ID_LEDTIMER2,
	ID_LEDTIMER3,

	ID_LCDTIMER1,
	ID_LCDTIMER2,
	ID_LCDTIMER3,

	ID_RING,
	ID_CALLING,

	ID_MAX
};

enum {
	STATUS_INQUERY,
	STATUS_OUTQUERY,
	STATUS_STOREQUERY,
	STATUS_DIALING,
	STATUS_IDLE
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_YLUSBTEL_H__AE40BE2D_0B84_4FAB_984A_51ADC44D6187__INCLUDED_)
