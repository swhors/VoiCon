#include "stdafx.h"
#include "stdio.h"
#include <cmras.h>
#include <cmsize.h> 
#include "rtp.h"
#include "rtcp.h"
#include <stkutils.h>
#include "msg_handler.h"
#include "msg_define.h"
#include "scm.h"
#include "plugin.h"
#include "cmfunction.h"
#include "Operate.h"
#include "voice.h"
#include "ivrkeydata.h"
#include "KeyLockCtrlDll.h"
#include "SystemBaseSet.h"

#include "management.h"
#include "win32dll.h"

#include "rtptest.h"

#include "typedef.h"

#include "sky_rtp.h"

#include "operate_ment.h"
#include "operate_scenario.h"

#include "netutils.h"

//#define FILE_CFG

//#include <winsock2.h> 
//#include <ws2tcpip.h> 
#include <iptypes.h> 
#pragma comment(lib, "ws2_32")

//#include <iphlpapi.h> 

#pragma comment(lib, "iphlpapi")   

#define USE_VENINFO

SStack *mpStack;

extern SCM		*scm;

extern BOOL bRunState;
extern BOOL bTerminate;

//extern int		logCount;

int		allowedCodec[5];

void WriteLogL(char * filename, int a, char * c, char * b);
//void WriteLogTest(char * filename, int a, char * c, char * b);

void SetEnableRTPChannelRecord(BOOL bVal);
void SetRTPRearrange(BOOL bVal);

#define SHAREDMEM_SIZE	160

//
//터미널 처리를 위해서 사용되어짐
//
// 터미널과 IVR간의 정보 전달 버퍼의 핸들
extern HANDLE hSharedMem1; // 콜의 정보를 전달하기 위한 버퍼.
// 버퍼의 참조 포인트
extern char *sSharedMemory1; // 콜의 정보를 전달하는 버퍼의 참조 포인터

int iG7231Enable,iG711UEnable,iG711AEnable;

bool lwin32_getMyIP(char * myip) 
{
	struct hostent *myent;
	struct in_addr *myen;
	long int *add;
	char tbuf[32];
    myen = (struct in_addr *)malloc( sizeof( struct in_addr ));
	memset(myen, 0, sizeof( struct in_addr ));
	myent = gethostbyname("");
	if( myent )
	{
		while(*myent->h_addr_list != NULL)
		{
			add = (long int *)*myent->h_addr_list;
			myen->s_addr = *add;
			sprintf(tbuf,"%s",inet_ntoa(*myen));
			if(memcmp(tbuf, "127.0.0.1", 9))
			{
				sprintf(myip, "%s", tbuf);
			    break;
			}
			myent->h_addr_list++;
		}
	}
	free(myen);
    return true; 
}   

void CALLBACK ManualRegister(HWND m_hWnd, UINT uMsg, UINT eEventId, DWORD dwTime)
{
	KillTimer(m_hWnd, RAD_MANUALREGISTRATION_TID);
	//if( hApp )
		cmRegister(hApp);
	//if( bRunState )
		SetTimer(m_hWnd, RAD_MANUALREGISTRATION_TID, RAD_MANUALREGISTRATION_TVAL, ManualRegister);
		WriteLogL(PROG_NAME, -1, "RRQ", "[TIMER EVENT] Send RRQ.");
}

void CALLBACK CheckLicense(HWND m_hWnd, UINT uMsg, UINT eEventId, DWORD dwTime)
{
	KillTimer(m_hWnd, eEventId);
	
	unsigned long ulBytes = 0, ulAddr = 0, ulRet = 0;
	char *pData = NULL;
	
	// Read string variable from the dog
	char strData[42];
	memset(strData, 0, 42);
	
	ulBytes = 20;			// The number of bytes read
	ulAddr = 0;				// The address read
	pData = strData;		// The data read
#if 0
	ulRet = DogRead(ulBytes, ulAddr, pData);
	
	if (ulRet == 0)			// Success
	{
		if( strcmp( pData, "www.skycom.ne.kr-ivr" ) !=  0 )
		{
//			EndOfOperate();
//			Sleep(100);
//			bReleaseGlobalMemory();
//			MessageBox(NULL, "License Data BAD!!", "Error : BisIvr", MB_OK | MB_SYSTEMMODAL );
//			PostQuitMessage(0);
		}
	}
	else if (ulRet == 30002 )
	{
//			EndOfOperate();
//			Sleep(100);
//			bReleaseGlobalMemory();
//			MessageBox(NULL, "USB LOCKKEY NOT FOUND!!", "Error : BisIvr", MB_OK | MB_SYSTEMMODAL);
//			PostQuitMessage(0);
	}
#endif
	SYSTEMTIME systemTime;	
	char tBuffer[64];
	memset( tBuffer, 0, 64);
	GetLocalTime(&systemTime);
	sprintf(tBuffer, "%02d%02d%02d", systemTime.wYear, systemTime.wMonth, systemTime.wDay);
	if( memcmp( tBuffer, keyData.szDueDay, 8) >= 0)
	{
		// 기간이 경과함.
		bRunState = FALSE;
		MessageBox(m_hWnd, "Expired license.", "Error : BisIvr", MB_OK);
	}
	else
	{
		//CheckLicense
		if( bIsUSBEnable )
		{
			FindAutoStorageDevice();
			if( IsUSBEnable() )
			{
				SetTimer(m_hWnd, LICENSE_CHK_TID, LICENSE_CHK_TIME*1000,CheckLicense);
			}
			else
			{
				// 키를 분실함.
				bRunState = FALSE;
				MessageBox(m_hWnd, "Error : Key Lost.", "Error : BisIvr", MB_OK);
			}
		}
		else
		{
			SetTimer(m_hWnd, LICENSE_CHK_TID, LICENSE_CHK_TIME*1000,CheckLicense);
		}
	}
}

void CALLBACK LogManager(HWND m_hWnd, UINT uMsg, UINT eEventId, DWORD dwTime)
{
	KillTimer(m_hWnd,eEventId);
	char *sCurPos=NULL;
	sCurPos = sSharedMemory1;
	for(int logCount = 0; logCount< iMaxAllowedCall; logCount++)
	{
		sCurPos = sSharedMemory1 + SHAREDMEM_SIZE*logCount;
		if ( strlen( pCon[logCount].src_e164 )  > 0 )
		{
			sprintf(sCurPos, "200/c%d/s%s/t%d/",
				pCon[logCount].iCallNumber,
				pCon[logCount].src_e164,
				pCon[logCount].iCallState);
		}
		else
		{
			sprintf(sCurPos, "200/c%d/sNONE/t%d/",
				pCon[logCount].iCallNumber,
				pCon[logCount].iCallState);
		}
		if( strlen( pCon[logCount].des_e164) <= 0)
		{
			char tbuf[124];
			sprintf(tbuf,"dNONE/u%d", scm[logCount].iCurStep);
			strcat(sCurPos, tbuf);
		}
		else
		{
			char tbuf[124];
			sprintf(tbuf,"d%s/u%d", pCon[logCount].des_e164,scm[logCount].iCurStep);
			strcat(sCurPos, tbuf);
		}
	}
	SendMessage(HWND_BROADCAST, RECVCALLINFO, (long)logCount, NULL);
	SetTimer(m_hWnd, LOG_TTL_TID, LOG_TTL_TVAL, LogManager);
}

int pvtGetIntVal(HPVT hVal, int iNodeID, int *iRetVal)
{
		int synNodeId;
		int iFieldID = 0;
		return pvtGet(hVal, iNodeID, &iFieldID , &synNodeId, iRetVal, NULL);
}

// RAD 스택의 BMP String을 일반 스트링으로 복사
// modified at 2005-01-20 PM 07:26
// 메모리침범 수정함. by whitwolf@skycom.ne.kr,swhors@skycom.ne.kr
void memcvt(IN char *wTbuf,OUT char *tbuf,IN int inbufLen,IN int outbufLen)
{
	int len = inbufLen;
	if( inbufLen >= (outbufLen*2) )
		len = outbufLen*2;
	for( int i = 1, b = 0; i < len; i=i+2,b++)
	{
		tbuf[b] = wTbuf[i];
	}
}

void memCVTSTR2Str(IN char *tbuf,OUT char *wTbuf,IN int inbufLen,IN int outBufLen)
{
	int len = inbufLen;
	memset(wTbuf, 0, outBufLen);
	for( int i = 1, b = 0; i < len; i=i+2,b++)
	{
		wTbuf[i] = tbuf[b];
	}
}

void getip(char *oTbuf, char *tbuf, int iLen)
{
	unsigned char *ttbuf = (unsigned char * )oTbuf;
	sprintf(tbuf, "%d.%d.%d.%d", ttbuf[0], ttbuf[1], ttbuf[2], ttbuf[3]);
}
void setip(PBYTE oTbuf, char *tbuf, int iLen)
{
	unsigned int uip;
	uip = ip_to_uint32(tbuf);
	memcpy(oTbuf, &uip, 4);
}

void EndOfOperate()
{
//	lRtpEnd();
	disable_rtp_manager_flag();
	HANDLE hEvent=CreateEvent(NULL,FALSE,FALSE,"TEMP");
	if( hEvent )
	{
		WaitForSingleObject(hEvent,150);
		CloseHandle(hEvent);
	}
	free_rtp_buff();
	UnLoadingMentFile();
	UnInitPlugInModule();
	release_plug();
	WriteLogL(PROG_NAME, -1, "OOP", "BIS_IVR Ending");
}

BOOL bReleaseGlobalMemory()
{
	if( sSharedMemory1 )
	{
		UnmapViewOfFile(sSharedMemory1);
		sSharedMemory1 = NULL;
	}
	
	if( hSharedMem1)
	{
		CloseHandle(hSharedMem1);
	}
	return TRUE;
}

BOOL bInitGlobalMemory()
{
	FindAutoStorageDevice();
	bIsUSBEnable = IsUSBEnable();
	if( GetKeyLock() == -1 )
		return FALSE;
	hSharedMem1 = NULL;
	hSharedMem1 = ::CreateFileMapping((HANDLE)0XFFFFFFFF,\
								NULL, PAGE_READWRITE, 0, \
								SHAREDMEM_SIZE*iMaxAllowedCall,\
								"AcroIVR-Shared-Global-1");
	
	if( hSharedMem1 == NULL )
		return FALSE;
	
	sSharedMemory1 = (char *)MapViewOfFile( hSharedMem1, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	
	if( sSharedMemory1 == NULL )
	{
		bReleaseGlobalMemory();
		return FALSE;
	}
	return TRUE;
}

BOOL registerGlobalMessage()
{
	MessageState = RegisterWindowMessage(MESSAGE_GLOBAL_STATE);
	MessageReq = RegisterWindowMessage(MESSAGE_GLOBAL_ACK);
	MessageRes = RegisterWindowMessage(MESSAGE_GLOBAL_RES);
	MessageReg = RegisterWindowMessage(MESSAGE_GLOBAL_REG);
	MessageRek = RegisterWindowMessage(MESSAGE_GLOBAL_REK);
	RECVCALLINFO = RegisterWindowMessage(MESSAGE_RECVCALLINFO);
	MessageShow   = RegisterWindowMessage( MESSAGE_MESSAGESHOW );
	
	if( MessageReq==0 || 
		MessageRes==0 || 
		MessageReg==0 || 
		MessageRek==0 || 
		MessageShow==0 ||
		RECVCALLINFO == 0 )
	{
		return FALSE;
	}
	
	return TRUE;
}

int GetInstalledLanguage()
{
	TCHAR *ttbuf;
	FILE *fp;
	int iCount = 0;
	char tbuf[8];
	if( (fp = fopen("language.ini", "r+t")) == NULL)
		return -1;
	
	for( iCount = 0; iCount < 8 ; iCount++)
		memset(sLanguage[iCount], 0, 4);
	
	iCount = 0;
	memset( tbuf, 0, 8);
	while((ttbuf = fgets(tbuf, 8, fp)) != NULL )
	{
		ttbuf = strstr(ttbuf, "\n");
		if( ttbuf )
		{
			memset( ttbuf, 0, 2);
		}
		memcpy(sLanguage[iCount++], tbuf, strlen(tbuf));
		memset(tbuf, 0, 8);
	}
	fclose(fp);
	
	if( iCount == 0)
	{
		return -1;
	}
	return 0;
}

//#define DEBUG_PVT_FN

BOOL SetPVT(HPVT hVal,INT32 handle,char * sPath, int iVal, char *sVal)
{
	int iNodeID;
	iNodeID = pvtGetNodeIdByPath(hVal, handle, sPath);
	if( iNodeID <= 0)
	{
		iNodeID=pvtBuildByPath(hVal, handle, sPath, iVal, sVal);
		if( iNodeID <= 0 )
			return FALSE;
	}
	else
		pvtSet(hVal, iNodeID, -1, iVal, sVal);
#ifdef DEBUG_PVT_FN
	if( sVal )
	{
		char tbuf[64];
		pvtGetString(hVal, iNodeID, 64,tbuf);
	}
	else
	{
		int iVal;
		pvtGetIntVal(hVal, iNodeID, &iVal);
	}
#endif
	return TRUE;
}

BOOL SetParamRAS(HPVT hVal)
{
	INT32 handleRAS;
	int iVal;
	char tbuf[64], tbuf1[128];;
	unsigned char uIP[4];
	handleRAS = cmGetRASConfigurationHandle(hApp);
	if( hVal <= 0 )
		return FALSE;
	// set responseTimeOut Value.
	//iVal = ::GetPrivateProfileInt("BISCON_RAS", "RAS_ResponseTimeOut", 6, "./biscon.ini");
	//if( iVal > 0 )
	//{
	//	if( !SetPVT(hVal, handleRAS, "responseTimeOut", iVal, NULL ) )
	//		return FALSE;
	//}
	
	// set RAS Port
	//iVal = ::GetPrivateProfileInt("BISCON_RAS", "RAS_RasPort", 1719, "./biscon.ini");
	//if( !SetPVT(hVal, handleRAS, "rasPort", iVal, NULL ) )
	//	return FALSE;
		
	// Set RAS Multicast IP
	::GetPrivateProfileString("BISCON_RAS", "RAS_RasMulticastAddress_ip", "224.0.1.41",tbuf, 64, "./biscon.ini");
	setip((PBYTE)&uIP, tbuf, strlen(tbuf));
	if( !SetPVT(hVal, handleRAS, "rasMulticastAddress.ipAddress.ip", 4, (char*)&uIP ) )
		return FALSE;
	
	// set RAS Multicast Port
	iVal = ::GetPrivateProfileInt("BISCON_RAS", "RAS_RasMulticastAddress_port", 1718, "./biscon.ini");
	if( !SetPVT(hVal, handleRAS, "rasMulticastAddress.ipAddress.port", iVal, NULL ) )
		return FALSE;
	
	// set GK IP
	::GetPrivateProfileString("BISCON_RAS", "RAS_GKipAddress_ip", "127.0.0.1",tbuf, 64, "./biscon.ini");
	setip((PBYTE)&uIP, tbuf, strlen(tbuf));
	if( !SetPVT(hVal, handleRAS, "manualDiscovery.defaultGatekeeper.ipAddress.ip", 4, (char*)&uIP ) )
		return FALSE;
	
	// set GK Port
	iVal = ::GetPrivateProfileInt("BISCON_RAS", "RAS_GKipAddress_port", 1719, "./biscon.ini");
	if( !SetPVT(hVal, handleRAS, "manualDiscovery.defaultGatekeeper.ipAddress.port", iVal, NULL ) )
		return FALSE;
	
	// set MaxFail
	//iVal = ::GetPrivateProfileInt("BISCON_RAS", "RAS_MaxFail", 10, "./biscon.ini");
	//if( iVal > 0 )
	//{
	//	if( !SetPVT(hVal, handleRAS, "maxFail", iVal, NULL ) )
	//		return FALSE;
	//}
	
	// Set H323-ID
#if 0
	::GetPrivateProfileString("BISCON_RAS", "H323-ID", "12345", tbuf, 64, "./biscon.ini");
	memCVTSTR2Str(tbuf, tbuf1, 64, 128);
	if( !SetPVT(hVal, handleRAS, "registrationInfo.terminalAlias.1.h323-ID", strlen(tbuf)*2, tbuf1 ) )
		return FALSE;
	// set E164
	::GetPrivateProfileString("BISCON_RAS", "E164_ID_1", "", tbuf, 64, "./biscon.ini");
	if( strlen(tbuf)> 0 )
	{
		if( !SetPVT(hVal, handleRAS, "registrationInfo.terminalAlias.2.e164", strlen(tbuf), tbuf ) )
			return FALSE;
		::GetPrivateProfileString("BISCON_RAS", "E164_ID_2", "", tbuf, 64, "./biscon.ini");
		if( strlen(tbuf)> 0 )
		{
			if( !SetPVT(hVal, handleRAS, "registrationInfo.terminalAlias.3.e164", strlen(tbuf), tbuf ) )
				return FALSE;
			::GetPrivateProfileString("BISCON_RAS", "E164_ID_3", "", tbuf, 64, "./biscon.ini");
			if( strlen(tbuf)> 0 )
			{
				if( !SetPVT(hVal, handleRAS, "registrationInfo.terminalAlias.4.e164", strlen(tbuf), tbuf ) )
					return FALSE;
				::GetPrivateProfileString("BISCON_RAS", "E164_ID_4", "", tbuf, 64, "./biscon.ini");
				if( strlen(tbuf)> 0 )
				{
					if( !SetPVT(hVal, handleRAS, "registrationInfo.terminalAlias.5.e164", strlen(tbuf), tbuf ) )
						return FALSE;
					::GetPrivateProfileString("BISCON_RAS", "E164_ID_5", "", tbuf, 64, "./biscon.ini");
					if( strlen(tbuf)> 0 )
					{
						if( !SetPVT(hVal, handleRAS, "registrationInfo.terminalAlias.6.e164", strlen(tbuf), tbuf ) )
							return FALSE;
						::GetPrivateProfileString("BISCON_RAS", "E164_ID_6", "", tbuf, 64, "./biscon.ini");
						if( strlen(tbuf)> 0 )
						{
							if( !SetPVT(hVal, handleRAS, "registrationInfo.terminalAlias.7.e164", strlen(tbuf), tbuf ) )
								return FALSE;
						}
					}
				}
			}
		}
	}
#endif
	// Set OverLapSending
	iVal = ::GetPrivateProfileInt("BISCON_RAS", "RAS_OverlappedSending", 0, "./biscon.ini");
	if( iVal == 1 )
	{
		if( !SetPVT(hVal, handleRAS, "overlapSending",0, NULL ) )
			return FALSE;
	}
	
	// 아래의 값은 설정되지 않고 시스템 고유의 값을 사용하는 것을 권장 함.
	// 따라서 설정하지 않음.
	// 2006/11/24/1359 swhors@naver.com
	// mark start --
	// Set TTL Value
	//iVal = ::GetPrivateProfileInt("BISCON_RAS", "RAS_TimeToLive", 0, "./biscon.ini");
	//if( iVal > 0 )
	//{
	//	if( !SetPVT(hVal, handleRAS, "registrationInfo.timeToLive", 120, NULL ) )
	//		return FALSE;
	//}
	//mark end --

#if 1
	// Set MC Value
	if( !SetPVT(hVal, handleRAS, "registrationInfo.terminalType.mc", 0, NULL ) )
		return FALSE;
	
	// Set undefineNode
	if( !SetPVT(hVal, handleRAS, "registrationInfo.terminalType.undefinedNode", 0, NULL ) )
		return FALSE;
	
	//// Set Terminal Type
	if( !SetPVT(hVal, handleRAS, "registrationInfo.terminalType.gateway", 0, NULL ) )
		return FALSE;
#endif
#ifdef USE_VENINFO
	// Set Terminal Type
	if( !SetPVT(hVal, handleRAS, "registrationInfo.terminalType.vendor.vendor.t35CountryCode", 0, NULL ) )
		return FALSE;
	if( !SetPVT(hVal, handleRAS, "registrationInfo.terminalType.vendor.vendor.t35Extension", 0, NULL ) )
		return FALSE;
	if( !SetPVT(hVal, handleRAS, "registrationInfo.terminalType.vendor.vendor.manufacturerCode", 10000, NULL ) )
		return FALSE;
	if( !SetPVT(hVal, handleRAS, "registrationInfo.terminalType.vendor.productId", 18, "BISCon_BIS-SYS IVR" ) )
		return FALSE;
	if( !SetPVT(hVal, handleRAS, "registrationInfo.terminalType.vendor.versionId", 3, "1.0" ) )
		return FALSE;
#endif
	return TRUE;
}

BOOL SetParamH245CodecType(HAPP hAPP,HPVT hVal,UINT hH45Handle,int count, int iCodecType=0)
{
	int iVal;
	char sPath[256];
	sprintf(sPath, "capabilities.terminalCapabilitySet.capabilityTable.%d.capabilityTableEntryNumber", count+1);
	switch(iCodecType)
	{
	case 0:// G7231-64K
		{
			allowedCodec[count] = 7231;
			if( !SetPVT(hVal, hH45Handle, sPath, 7231, NULL ) )
				return FALSE;
			iVal = ::GetPrivateProfileInt("BISCON_RTP", "G7231_maxAl-sduAudioFrames", 1, "./biscon.ini");
			//iIvrDefaultSilenceSupression = iVal;
			sprintf(sPath, "capabilities.terminalCapabilitySet.capabilityTable.%d.capability.receiveAndTransmitAudioCapability.g7231.maxAl-sduAudioFrames", count+1);
			if( !SetPVT(hVal, hH45Handle, sPath, iVal, NULL ) )
				return FALSE;
			
			iVal = ::GetPrivateProfileInt("BISCON_RTP", "G7231_SilenceSupression", 1, "./biscon.ini");
			sprintf(sPath, "capabilities.terminalCapabilitySet.capabilityTable.%d.capability.receiveAndTransmitAudioCapability.g7231.silenceSuppression", count+1);
			if( !SetPVT(hVal, hH45Handle, sPath, iVal, NULL ) )
				return FALSE;
		}
		break;
	case 1:// G711Ulaw
		allowedCodec[count] = 7111;
		if( !SetPVT(hVal, hH45Handle, sPath, 7111, NULL ) )
			return FALSE;
		sprintf(sPath, "capabilities.terminalCapabilitySet.capabilityTable.%d.capability.receiveAndTransmitAudioCapability.g711Ulaw64k", count+1);
		if( !SetPVT(hVal, hH45Handle, sPath, 60, NULL ) )
			return FALSE;
		break;
	case 2:// G711Alaw
		allowedCodec[count] = 7112;
		if( !SetPVT(hVal, hH45Handle, sPath, 7112, NULL ) )
			return FALSE;
		sprintf(sPath, "capabilities.terminalCapabilitySet.capabilityTable.%d.capability.receiveAndTransmitAudioCapability.g711Alaw64k", count+1);
		if( !SetPVT(hVal, hH45Handle, sPath, 60, NULL ) )
			return FALSE;
		break;
	case 6: // dtmf
		allowedCodec[count] = 6000;
		if( !SetPVT(hVal, hH45Handle, sPath, 6000, NULL ) )
			return FALSE;
		sprintf(sPath, "capabilities.terminalCapabilitySet.capabilityTable.%d.capability.receiveAndTransmitUserInputCapability.dtmf", count+1);
		if( !SetPVT(hVal, hH45Handle, sPath, 0, NULL ) )
			return FALSE;
		break;
	case 7: // basicString
		allowedCodec[count] = 6001;
		if( !SetPVT(hVal, hH45Handle, sPath, 6001, NULL ) )
			return FALSE;
		sprintf(sPath, "capabilities.terminalCapabilitySet.capabilityTable.%d.capability.receiveAndTransmitUserInputCapability.basicString", count+1);
		if( !SetPVT(hVal, hH45Handle, sPath, 0, NULL ) )
			return FALSE;
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

BOOL SetCapabilityDescriptors(HPVT hVal,UINT hAndle)
{
	char sPath[256];
	wsprintf(sPath,"%s","capabilities.terminalCapabilitySet.capabilityDescriptors.1.capabilityDescriptorNumber");
	if( !SetPVT(hVal, hAndle, sPath, 0, NULL ) )
		return FALSE;
	for( int i = 0,iCount=1; i < 5 ; i++,iCount++)
	{
		if( allowedCodec[i] < 0 )
			break;
		if( allowedCodec[i] == 6000 || allowedCodec[i] == 6001 )
		{
			wsprintf(sPath, "%s", "capabilities.terminalCapabilitySet.capabilityDescriptors.1.simultaneousCapabilities.2.1");
			if( !SetPVT(hVal, hAndle, sPath, allowedCodec[i], NULL ) )
				return FALSE;
		}
		else
		{
			wsprintf(sPath, "capabilities.terminalCapabilitySet.capabilityDescriptors.1.simultaneousCapabilities.1.%d", iCount);
			if( !SetPVT(hVal, hAndle, sPath, allowedCodec[i], NULL ) )
				return FALSE;
		}
	}
	return TRUE;
}

BOOL SetH245Channel(HPVT hVal,UINT hAndle)
{
	char sPath[256];
	int iVal;
	for(int i = 0; i < 5; i++)
	{
		wsprintf(sPath, "channels.%d.name", i+1);
		switch(allowedCodec[i])
		{
		case 7231:
			{
				if( !SetPVT(hVal, hAndle, sPath, 5, "g7231") )
					return FALSE;
				wsprintf(sPath, "channels.%d.dataType.audioData.g7231.maxAl-sduAudioFrames", i+1);
				iVal = ::GetPrivateProfileInt("BISCON_RTP", "G7231_maxAl-sduAudioFrames", 1, "./biscon.ini");
				if( !SetPVT(hVal, hAndle, sPath, iVal, NULL ) )
					return FALSE;
				wsprintf(sPath, "channels.%d.dataType.audioData.g7231.silenceSuppression", i+1);
				iVal = ::GetPrivateProfileInt("BISCON_RTP", "G7231_SilenceSupression", 1, "./biscon.ini");
				if( !SetPVT(hVal, hAndle, sPath, iVal, NULL ) )
					return FALSE;
			}
			break;
		case 7111:
			{
				if( !SetPVT(hVal, hAndle, sPath, 11, "g711Ulaw64k") )
					return FALSE;
				wsprintf(sPath, "channels.%d.dataType.audioData.g711Ulaw64k", i+1);
				if( !SetPVT(hVal, hAndle, sPath, 60, NULL ) )
					return FALSE;
			}
			break;
		case 7112:
			{
				if( !SetPVT(hVal, hAndle, sPath, 11, "g711Alaw64k") )
					return FALSE;
				wsprintf(sPath, "channels.%d.dataType.audioData.g711Alaw64k", i+1);
				if( !SetPVT(hVal, hAndle, sPath, 60, NULL ) )
					return FALSE;
			}
			break;
		case 6000:
		case 6001:
			break;
		default:
			return TRUE;
		}
	}
	return TRUE;
}

BOOL SetParamH245(HPVT hVal)
{
	BYTE pID[2];
	INT32 handleH245;
	int iCodecType,iCodecNum=0;
	/* iCodecType 
		0 - G7231/64
		1 - G7231/53
		2 - G711Ulaw
		3 - G711Alaw
	*/
	
	handleH245 = cmGetH245ConfigurationHandle(hApp);
	if( hVal <= 0 )
		return FALSE;
	// set responseTimeOut Value.
	if( !SetPVT(hVal, handleH245, "masterSlave.timeout", 7, NULL ) )
		return FALSE;
	for(int i = 0; i < 5; i++)
	{
		allowedCodec[i] = -1;
	}
#if 1
	if( !SetPVT(hVal, handleH245, "capabilities.terminalCapabilitySet.sequenceNumber", 0, NULL ) )
		return FALSE;
	pID[0] = 0;
	pID[1] = 1;
	if( !SetPVT(hVal, handleH245, "capabilities.terminalCapabilitySet.protocolIdentifier", 2, (char*)&pID[0]) )
		return FALSE;
#endif

	// Set capability
	iCodecType = ::GetPrivateProfileInt("BISCON_RTP", "CODEC_TYPE1", -1, "./biscon.ini");
	if( iCodecType >= 0 )
	{
		if( !SetParamH245CodecType(hApp, hVal, handleH245, iCodecNum++, iCodecType) )
			return FALSE;
	}
	iCodecType = ::GetPrivateProfileInt("BISCON_RTP", "CODEC_TYPE2", -1, "./biscon.ini");
	if( iCodecType >= 0 )
	{
		if( !SetParamH245CodecType(hApp, hVal, handleH245, iCodecNum++, iCodecType) )
			return FALSE;
	}
	iCodecType = ::GetPrivateProfileInt("BISCON_RTP", "CODEC_TYPE3", -1, "./biscon.ini");
	if( iCodecType >= 0 )
	{
		if( !SetParamH245CodecType(hApp, hVal, handleH245, iCodecNum++, iCodecType) )
			return FALSE;
	}
	iCodecType = ::GetPrivateProfileInt("BISCON_RTP", "CODEC_TYPE4", -1, "./biscon.ini");
	if( iCodecType >= 0 )
	{
		if( !SetParamH245CodecType(hApp, hVal, handleH245, iCodecNum++, iCodecType) )
			return FALSE;
	}
	iCodecType = ::GetPrivateProfileInt("BISCON_RTP", "DTMF_TYPE", -1, "./biscon.ini");
	if( iCodecType >= 0 )
	{
		if( !SetParamH245CodecType(hApp, hVal, handleH245, iCodecNum, iCodecType+6) )
			return FALSE;
	}
	if( !SetCapabilityDescriptors(hVal, handleH245) )
		return FALSE;
	if( !SetH245Channel(hVal, handleH245))
		return FALSE;
	return TRUE;
}

BOOL SetParamQ931(HPVT hVal)
{
	INT32 handleQ931;
	int iVal;
	handleQ931 = cmGetQ931ConfigurationHandle(hApp);
	if( hVal <= 0 )
		return FALSE;
	// set responseTimeOut Value.
	iVal = ::GetPrivateProfileInt("BISCON_Q931", "Q931_ResponseTimeOut", 4, "./biscon.ini");
	if( !SetPVT(hVal, handleQ931, "responseTimeOut", iVal, NULL ) )
		return FALSE;
	// set connection timeout value.
	iVal = ::GetPrivateProfileInt("BISCON_Q931", "Q931_ConnectTimeOut", 100, "./biscon.ini");
	if( !SetPVT(hVal, handleQ931, "connectTimeOut", iVal, NULL ) )
		return FALSE;
	// set callSignallingPort
	iVal = ::GetPrivateProfileInt("BISCON_Q931", "Q931_CallSignalingPort", 1720, "./biscon.ini");
	if( !SetPVT(hVal, handleQ931, "callSignalingPort", iVal, NULL ) )
		return FALSE;
	// set maxCalls
	iVal = (iMaxAllowedCall*2)+10;
	if( !SetPVT(hVal, handleQ931, "maxCalls", iVal, NULL ) )
		return FALSE;
	// set h245Tunneling
	iVal = ::GetPrivateProfileInt("BISCON_Q931", "Q931_H245Tunneling", 0, "./biscon.ini");
	if( !SetPVT(hVal, handleQ931, "h245Tunneling", iVal, NULL ) )
		return FALSE;
	return TRUE;
}

BOOL ParseConfigure(HWND hWnd)
{
	BOOL bRet = FALSE;
	hApp = NULL;
	SetTimer(m_hWnd, LICENSE_CHK_TID, LICENSE_CHK_TIME*1000,CheckLicense);		
	
	m_iFastStart = GetPrivateProfileInt("BISCON_Q931", "FastStart", 0,"./BisCon.ini");
	
	m_iH245Tunneling = GetPrivateProfileInt("BISCON_Q931", "Q931_H245Tunneling", 0,"./BisCon.ini");
	
	bEnableCallCoupl = GetPrivateProfileInt("SystemBase Setup", "EnableCallCouple", 1,"./BisCon.ini");
	
	iInputWaitTime = GetPrivateProfileInt("BISCON_RTP", "InputWaitTime", 6,"./BisCon.ini");
	
	iInputInitWaitTime = GetPrivateProfileInt("BISCON_RTP", "InputInitWaitTime", 60,"./BisCon.ini");

	m_g729Support = GetPrivateProfileInt("SystemBase Setup", "G729Support", 0, "./BisCon.ini");
	m_InitOfRtpPort = ::GetPrivateProfileInt("BISCON_RTP", "RTP_port", 30000, "./biscon.ini");	
	
	//bAutoRun = GetPrivateProfileInt("SystemBase Setup", "AutoStart", 1,"./BisCon.ini");
	if( GetPrivateProfileInt("BISCON_RTP", "RTP_Record", 0,"./biscon.ini"))
		SetEnableRTPChannelRecord(TRUE);
	else
		SetEnableRTPChannelRecord(FALSE);
	
//	if( GetPrivateProfileInt("BISCON_RTP", "RTP_ReArrange", 0,"./biscon.ini") )
//		SetRTPRearrange(TRUE);
//	else
//		SetRTPRearrange(FALSE);
	
	// BISCon.tmp 파일을 확인 생성한다.
	// 2006.11.24.1355 swhors@naver.com
#ifndef FILE_CFG
	FILE * fp;
	fp = fopen("BISCon.tmp", "a+t");
	if( fp )
		fclose(fp);
	
	HCFG hCfg = ciConstructEx("BISCon.tmp",2048,100);
	if( hCfg )
	{
		ciSetValue(hCfg, "system.manualStart",FALSE,0,NULL);
		ciSetValue(hCfg, "system.maxCalls",FALSE,(INT32)((iMaxAllowedCall*2)+10),NULL);
		ciSetValue(hCfg, "system.maxChannels",FALSE,(INT32)(2),NULL);
		ciSetValue(hCfg, "RAS.rasPort",FALSE,(INT32)(GetPrivateProfileInt("BISCON_RAS", "RAS_RasPort", 1719, "./biscon.ini")),NULL);
#if 1
		char tbuf1[128];
		char tbuf[64];
		::GetPrivateProfileString("BISCON_RAS", "H323-ID", "12345", tbuf, 64, "./biscon.ini");
		memCVTSTR2Str(tbuf, tbuf1, 64, 128);
		ciSetValue(hCfg, "RAS.registrationInfo.terminalAlias.1.h323-ID", TRUE, strlen(tbuf)*2, tbuf1 );
		// set E164
		::GetPrivateProfileString("BISCON_RAS", "E164_ID_1", "", tbuf, 64, "./biscon.ini");
		if( strlen(tbuf)> 0 )
		{
			if( ciSetValue(hCfg, "RAS.registrationInfo.terminalAlias.2.e164", TRUE, strlen(tbuf), tbuf ) <0)
				return FALSE;
			::GetPrivateProfileString("BISCON_RAS", "E164_ID_2", "", tbuf, 64, "./biscon.ini");
			if( strlen(tbuf)> 0 )
			{
				if( ciSetValue(hCfg, "RAS.registrationInfo.terminalAlias.3.e164", TRUE, strlen(tbuf), tbuf ) <0 )
					return FALSE;
				::GetPrivateProfileString("BISCON_RAS", "E164_ID_3", "", tbuf, 64, "./biscon.ini");
				if( strlen(tbuf)> 0 )
				{
					if( ciSetValue(hCfg, "RAS.registrationInfo.terminalAlias.4.e164", TRUE, strlen(tbuf), tbuf ) <0 )
						return FALSE;
					::GetPrivateProfileString("BISCON_RAS", "E164_ID_4", "", tbuf, 64, "./biscon.ini");
					if( strlen(tbuf)> 0 )
					{
						if( ciSetValue(hCfg, "RAS.registrationInfo.terminalAlias.5.e164", TRUE, strlen(tbuf), tbuf ) <0 )
							return FALSE;
						::GetPrivateProfileString("BISCON_RAS", "E164_ID_5", "", tbuf, 64, "./biscon.ini");
						if( strlen(tbuf)> 0 )
						{
							if( ciSetValue(hCfg, "RAS.registrationInfo.terminalAlias.6.e164", TRUE, strlen(tbuf), tbuf )  <0)
								return FALSE;
							::GetPrivateProfileString("BISCON_RAS", "E164_ID_6", "", tbuf, 64, "./biscon.ini");
							if( strlen(tbuf)> 0 )
							{
								if( ciSetValue(hCfg, "RAS.registrationInfo.terminalAlias.7.e164", TRUE, strlen(tbuf), tbuf )  <0)
									return FALSE;
							}
						}
					}
				}
			}
		}
#endif
		if( GetPrivateProfileInt("BISCON_RAS", "RAS_ManualRegistration", 0, "./biscon.ini") )
		{
			ciSetValue(hCfg, "RAS.manualRegistration", FALSE, 0, NULL );
			char tttbuf[64];
			wsprintf(tttbuf, "Start Manual_Registration Mode.[%ud]", hWnd);
			WriteLogL(PROG_NAME, -1, "OOP", tttbuf);
			//SetTimer(m_hWnd, RAD_MANUALREGISTRATION_TID, RAD_MANUALREGISTRATION_TVAL, ManualRegister);
			//SetTimer(hWnd, RAD_MANUALREGISTRATION_TID, RAD_MANUALREGISTRATION_TVAL, NULL);
			SendMessage(hWnd, WM_BISCON_MANUALREGIST, NULL, NULL);
		}
		else
			WriteLogL(PROG_NAME, -1, "OOP", "Start Automatic_Registration Mode.");
		//ciSetValue(hCfg, "RAS.keepAlive", FALSE,1, NULL );
		int iValue = GetPrivateProfileInt("BISCON_RAS", "RAS_TimeToLive", 120000, "./biscon.ini");
		if( iValue < 10000 && iValue > 480000)
			iValue = 120000;
		iValue = iValue / 1000;
		ciSetValue(hCfg, "RAS.registrationInfo.timeToLive", FALSE,(INT32)iValue, NULL );
		//ciSetValue(hCfg, "RAS.responseTimeOut",FALSE,4,NULL);
		//ciSetValue(hCfg, "RAS.maxFail", FALSE, 10, NULL);
		ciSetValue(hCfg, "Q931.callSignalingPort",FALSE,(INT32)(GetPrivateProfileInt("BISCON_Q931", "Q931_CallSignalingPort", 1720, "./biscon.ini")),NULL);
		ciSetValue(hCfg, "Q931.maxCalls",FALSE,(INT32)((iMaxAllowedCall*2)+10),NULL);
		ciSave(hCfg,"BISCon.tmp");
		ciDestruct(hCfg);
	}
	else
	{
		WriteLogL(PROG_NAME, -1, "OOP", "error:open cfg");
	}
	hApp = NULL;
	if(cmInitialize("BISCon.tmp", &hApp) <0)
	{
		// cmInitialize 후에 
		// BISCon.tmp 파일을 삭제한다.
		// 2006.11.24.1355 swhors@naver.com
		::DeleteFile("BISCon.tmp");
		return FALSE;
	}
#else // FILE_CFG
	if(cmInitialize("BISCon.cfg", &hApp) <0)
	{
		return FALSE;
	}
	//if( GetPrivateProfileInt("BISCON_RAS", "RAS_ManualRegistration", 0, "./biscon.ini") )
	{
		char tttbuf[64];
		wsprintf(tttbuf, "Start Manual_Registration Mode.[%ud]", hWnd);
		WriteLogL(PROG_NAME, -1, "OOP", tttbuf);
		SendMessage(hWnd, WM_BISCON_MANUALREGIST, NULL, NULL);
	}

#endif // FILE_CFG
	if( hApp)
	{
		int iNodeID = 0,iRootID=0;
		int iRasCFH = 0;
		char tbuff[256];
		int len = 0;
		
		HPVT hVal;
		
		// cmInitialize 후에 
		// BISCon.tmp 파일을 삭제한다.
		// 2006.11.24.1355 swhors@naver.com
#ifndef FILE_CFG
		::DeleteFile("BISCon.tmp");
#endif
		
		memset(szH323ID, 0, 32);
		memset(szE164ID, 0, 32);
		memset(szGlobalGkIp, 0, 24);
		
		hVal=cmGetValTree(hApp);
		
		if( hVal <= 0 )
			return FALSE;
#ifndef FILE_CFG		
		if( !::GetPrivateProfileInt("BISCON", "RAS_FROM_FILE", 0, "../biscon.ini"))
			SetParamRAS(hVal);
#endif
		iRasCFH=cmGetRASConfigurationHandle(hApp);
		iNodeID=pvtGetNodeIdByPath(hVal,iRasCFH,"registrationInfo.terminalAlias.2.e164");
		len=pvtGetString(hVal,iNodeID,256,szE164ID);
		if (len>=0)
			szE164ID[len]=0;
		iNodeID = pvtGetNodeIdByPath(hVal,iRasCFH,"registrationInfo.terminalAlias.1.h323-ID");
		memset(tbuff, 0, 256);
		len=pvtGetString(hVal,iNodeID,256,tbuff);
		if (len>=0)
			tbuff[len]=0;
		memcvt(tbuff, szH323ID, len,H323BUFFERLEN);
		iNodeID = pvtGetNodeIdByPath(hVal,iRasCFH,"manualDiscovery.defaultGatekeeper.ipAddress.ip");
		len=pvtGetString(hVal,iNodeID,256,tbuff);
		if (len>=0)
			tbuff[len]=0;
		getip(tbuff, szGlobalGkIp, len);
		iNodeID = pvtGetNodeIdByPath(hVal,iRasCFH,"manualDiscovery.defaultGatekeeper.ipAddress.port");
		len = pvtGetIntVal(hVal, iNodeID, &iGkPort);
#ifndef FILE_CFG
		if( !::GetPrivateProfileInt("BISCON", "Q931_FROM_FILE", 0, "../biscon.ini"))
			SetParamQ931(hVal);
		if( !::GetPrivateProfileInt("BISCON", "H245_FROM_FILE", 0, "../biscon.ini"))
			SetParamH245(hVal);
#endif		
		// Ivr Initialize...
		init_call_stack(iMaxAllowedCall);
		//unsigned long dwThreadId;
		//CreateThread(NULL, 0, LogManager, NULL, 0, &dwThreadId);
		LoadSystemDefaultMent();
		if( InitScenarioManager() )
		{
			mentfileList_sort();
			WriteLogL(PROG_NAME, -1, "OOP", "Load Ment File.");
			LoadingMentFile();
			WriteLogL(PROG_NAME, -1, "OOP", "Load PlugIn Module.");
			LoadPlugDll();
			WriteLogL(PROG_NAME, -1, "OOP", "Init PlugIn Module.");
			InitPlugInModule();
			WriteLogL(PROG_NAME, -1, "OOP", "Success to start.");
			mentfileList_erase(NULL);
			return TRUE;
		}

		return FALSE;
	}
	return FALSE;
}

BOOL init_call_stack(int iMaxValue)
{
	int i,j;
	UINT32 uIP;
//	cmRASTransport tr;
	char IpAdr[30];

	iMaxAllowedCall = iMaxValue;
	
	cmSetHandle(hApp, (HAPPAPP)NULL); 
	
#if 0	
	cmGetLocalCallSignalAddress(hApp,&tr);
	sprintf(IpAdr,"%d.%d.%d.%d",
		(int)((unsigned char*)&(tr.ip))[0],(int)((unsigned char*)&(tr.ip))[1],
		(int)((unsigned char*)&(tr.ip))[2],(int)((unsigned char*)&(tr.ip))[3]);
	printf("%s\n", IpAdr);
#else
	lwin32_getMyIP(IpAdr);
#endif

	uIP = ip_to_uint32(IpAdr);		
	for( j = 0; j< iMaxAllowedCall*2; j++)
	{
		pCon[j].bTimerSet = FALSE;
		wsprintf(pCon[j].src_e164 , "none");
		wsprintf(pCon[j].des_e164 , "none");
		pCon[j].iCallNumber=j;
		if( j < iMaxAllowedCall )
		{
			pCon[j].thread_sce = NULL;
		}
		pCon[j].iCallState = 0;
		pCon[j].bOpendChannel = FALSE;
		pCon[j].bUsedThisCall = FALSE;
		pCon[j].bCallReusing = TRUE;
		pCon[j].hsCall = NULL;
		pCon[j].iChannelCodec = 0;
		pCon[j].overlapState.running=(overlapStateT)0;
		pCon[j].bVoiceRec = FALSE;
		pCon[j].bInChannelConnect = 0;
		pCon[j].bRetrialCall = 0;
		for (i=0;i<10;i++)
		{
			pCon[j].out[i].port = 0;
			pCon[j].out[i].uIP = uIP;
			//pCon[j].out[i].uPort = m_InitOfRtpPort + (j*10 + i)*2;
//			pCon[j].out[i].uPort = ((j<iMaxAllowedCall)?m_InitOfRtpPort:(m_InitOfRtpPort+iMaxAllowedCall*2)) + j*2;
			pCon[j].out[i].uPort = m_InitOfRtpPort + j*2;
			pCon[j].out[i].hsChan = NULL;
			pCon[j].out[i].state=cmChannelStateIdle;
			pCon[j].out[i].sNum=i;
			pCon[j].out[i].haCall=(HAPPCALL)&pCon[j];
			//pCon[j].in[i].uPort = m_InitOfRtpPort + (j*10 + i)*2;
			//pCon[j].in[i].uPort = ((j<iMaxAllowedCall)?m_InitOfRtpPort:(m_InitOfRtpPort+iMaxAllowedCall*2)) + j*2;
			pCon[j].in[i].uPort = m_InitOfRtpPort + j*2;
			//pConnect->iCallNumber*10+c->sNum
			pCon[j].in[i].uIP = uIP;
			pCon[j].in[i].port = 0;
			pCon[j].in[i].hsChan = NULL;
			pCon[j].in[i].state=cmChannelStateIdle;
			pCon[j].in[i].sNum=i;
			pCon[j].in[i].haCall=(HAPPCALL)&pCon[j];
		}
	}
	
	int nDataNodeId = -1;
	nDataNodeId = cmUserInputSupportIndicationBuild(hApp, cmSupportGeneralString, &nDataNodeId);
	
	SCMEVENT cmEvent={cmEvNewCall,cmEvRegEvent};
	SCMCALLEVENT cmCallEvent={ 
		cmEvCallStateChanged,
		NULL/*cmEvCallNewRate*/,
		NULL/*cmEvCallInfo*/,
		NULL/*cmEvCallNonStandardParam*/,
		cmEvCallFacility,
		cmEvCallFastStartSetup,
		NULL,
		cmEvCallUserInfo,
		NULL,
		cmEvCallIncompleteAddress,
		cmEvCallAdditionalAddress};
	
	SCMCONTROLEVENT cmControlEvent={
		cmEvCallCapabilities,
		cmEvCallCapabilitiesExt,
		cmEvChannelNew,
		NULL/*cmEvCallCapabilitiesResponse*/,
		cmEvCallMasterSlaveStatus,
		NULL/*cmEvCallRoundTripDelay*/,
		cmEvCallUserInput,
		NULL/*cmEvCallRequestMode*/,
		NULL/*cmEvCallMiscStatus*/,
		cmEvCallControlStateChanged,
		cmEvCallMasterSlave};
/*
	SCMCHANEVENT cmChannelEvent={
		cmEvChannelStateChanged,
		cmEvChannelNewRate, 
		cmEvChannelMaxSkew, 
		cmEvChannelSetAddress, 
		cmEvChannelSetRTCPAddress, 
		cmEvChannelParameters, 
		cmEvChannelRTPDynamicPayloadType,
		cmEvChannelVideoFastUpdatePicture,
		cmEvChannelVideoFastUpdateGOB,
		cmEvChannelVideoFastUpdateMB, 
		cmEvChannelHandle,
		NULL,
		cmEvChannelRequestCloseStatus,
		NULL,
		cmEvChannelMediaLoopStatus,
		cmEvChannelReplace,
		cmEvChannelFlowControlToZero, 
		cmEvChannelMiscCommand,
		cmEvChannelTransportCapInd,
		cmEvChannelSetNSAPAddress,
		cmEvChannelSetATMVC
	};
*/	
	SCMCHANEVENT cmChannelEvent={ // 21
		cmEvChannelStateChanged,
		NULL/*cmEvChannelNewRate*/,
		NULL/*cmEvChannelMaxSkew*/, 
		cmEvChannelSetAddress,
		cmEvChannelSetRTCPAddress,
		cmEvChannelParameters, 
		cmEvChannelRTPDynamicPayloadType,
		NULL/*cmEvChannelVideoFastUpdatePicture*/,
		NULL/*cmEvChannelVideoFastUpdateGOB*/,
		NULL/*cmEvChannelVideoFastUpdateMB*/, 
		cmEvChannelHandle,
		NULL/*cmEvChannelGetRTCPAddress*/,
		cmEvChannelRequestCloseStatus,
		NULL,
		cmEvChannelMediaLoopStatus,
		NULL/*cmEvChannelReplace*/,
		NULL/*cmEvChannelFlowControlToZero*/,
		NULL/*cmEvChannelMiscCommand*/,
		NULL/*cmEvChannelTransportCapInd*/,
		NULL/*cmEvChannelSetNSAPAddress*/,
		NULL/*cmEvChannelSetATMVC*/
	};
	
	SCMRASEVENT cmRASEvent={/*cmEvRASRequest*/NULL, cmEvRASConfirm, /*cmEvRASReject*/NULL,cmEvRASTimeout};

	SCMPROTOCOLEVENT cmProtocolEvent={cmHookListen,cmHookListening,cmHookConnecting,cmHookInConn,
		cmHookOutConn,cmHookSend,cmHookRecv,cmHookSendTo,cmHookRecvFrom,cmHookClose};
		
	cmSetGenEventHandler(hApp,&cmEvent,sizeof(cmEvent));
	
	cmSetCallEventHandler(hApp,&cmCallEvent,sizeof(cmCallEvent));
		
	cmSetControlEventHandler(hApp,&cmControlEvent,sizeof(cmControlEvent));
	
	cmSetChannelEventHandler(hApp,&cmChannelEvent,sizeof(cmChannelEvent));
	
	cmSetProtocolEventHandler(hApp,&cmProtocolEvent,sizeof(cmProtocolEvent));
	cmRASSetEventHandler(hApp,&cmRASEvent,sizeof(cmRASEvent));
#if 0
	lRtpInit(m_hWnd, iMaxAllowedCall,uIP);
#endif	
	return 0;
}

int GetKeyLock()
{
	int iRetCode = 0;
	iRetCode = KeyVerifyWithMac(key_auto);
	switch(iRetCode)
	{
	case -1: // 비정상
		MessageBox(NULL, "Data Error [ file or USB device : type 0].", "Error : BisIvr", MB_OK);
		return -1;
	case 0: // 빈 라이센스 키임.
		if( !KeySetUser(key_auto) )
		{   // 서버의 맥 주소를 입력.
			MessageBox(NULL, "Data Error [ file or USB device : type 1].", "Error : BisIvr", MB_OK);
			return -1;
		}
	case 1:
		{  // 정상적인 데이터.
			if( !KeyRead("license.dat", (PBYTE)&keyData, sizeof(IVRFILELOCK), key_auto) )
			{
				MessageBox(NULL, "Data Error [ file or USB device : type 2].", "Error : BisIvr", MB_OK);
				return -1;
			}
			//기간을 검사.
			SYSTEMTIME sysTime;
			::GetSystemTime(&sysTime);
			char tBuff[64];
			memset(tBuff, 0, 64);
			wsprintf(tBuff, "%04d%02d%02d", sysTime.wYear, sysTime.wMonth, sysTime.wDay);
			if( memcmp(tBuff, keyData.szDueDay, strlen(keyData.szDueDay) ) >= 0 )
			{
				return -1;
			}
			iMaxAllowedCall = atoi(keyData.szMaxCall);
			if ( iMaxAllowedCall <= 0 )
				iMaxAllowedCall = 20;
		}
		break;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// end of file..
//
