#include "stdafx.h"
#include "stdio.h"
#include <cm.h>
#include <cmras.h>
#include <cmsize.h> 
#include <stkutils.h>

#include "H323CallInfo.h"
#include "UbiDialer.h"
#include "UbiDialerDlg.h"
#include "CallConf.h"

#include "cmFunction.h"

#include "Operate.h"

#include "netutils.h"

#define USE_VENINFO

int iG7231Enable,iG711UEnable,iG711AEnable;

int allowedCodec[4];

//int iMaxAllowedCall=1;

BOOL init_call_stack(PVOID pObject);

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
	iVal = ::GetPrivateProfileInt("BISCON_RAS", "RAS_ResponseTimeOut", 4, H323PHONE_INI);
	if( iVal > 0 )
	{
		if( !SetPVT(hVal, handleRAS, "responseTimeOut", iVal, NULL ) )
			return FALSE;
	}
	
	// set RAS Port
	iVal = ::GetPrivateProfileInt("BISCON_RAS", "RAS_RasPort", 1719, H323PHONE_INI);
	if( !SetPVT(hVal, handleRAS, "rasPort", iVal, NULL ) )
		return FALSE;
		
	// Set RAS Multicast IP
	::GetPrivateProfileString("BISCON_RAS", "RAS_RasMulticastAddress_ip", "224.0.1.41",tbuf, 64, H323PHONE_INI);
	setip((PBYTE)&uIP, tbuf, strlen(tbuf));
	if( !SetPVT(hVal, handleRAS, "rasMulticastAddress.ipAddress.ip", 4, (char*)&uIP ) )
		return FALSE;
	
	// set RAS Multicast Port
	iVal = ::GetPrivateProfileInt("BISCON_RAS", "RAS_RasMulticastAddress_port", 1718, H323PHONE_INI);
	if( !SetPVT(hVal, handleRAS, "rasMulticastAddress.ipAddress.port", iVal, NULL ) )
		return FALSE;
	
	// set GK IP
	::GetPrivateProfileString("BISCON_RAS", "RAS_GKipAddress_ip", "127.0.0.1",tbuf, 64, H323PHONE_INI);
	setip((PBYTE)&uIP, tbuf, strlen(tbuf));
	if( !SetPVT(hVal, handleRAS, "manualDiscovery.defaultGatekeeper.ipAddress.ip", 4, (char*)&uIP ) )
		return FALSE;
	
	// set GK Port
	iVal = ::GetPrivateProfileInt("BISCON_RAS", "RAS_GKipAddress_port", 1719, H323PHONE_INI);
	if( !SetPVT(hVal, handleRAS, "manualDiscovery.defaultGatekeeper.ipAddress.port", iVal, NULL ) )
		return FALSE;
	
	// set MaxFail
	iVal = ::GetPrivateProfileInt("BISCON_RAS", "RAS_MaxFail", 10, H323PHONE_INI);
	if( iVal > 0 )
	{
		if( !SetPVT(hVal, handleRAS, "maxFail", iVal, NULL ) )
			return FALSE;
	}
	
	// Set H323-ID
	::GetPrivateProfileString("BISCON_RAS", "H323-ID", "12345", tbuf, 64, H323PHONE_INI);
	memCVTSTR2Str(tbuf, tbuf1, 64, 128);
	if( !SetPVT(hVal, handleRAS, "registrationInfo.terminalAlias.1.h323-ID", strlen(tbuf)*2, tbuf1 ) )
		return FALSE;
	// set E164
	::GetPrivateProfileString("BISCON_RAS", "E164_ID_1", "", tbuf, 64, H323PHONE_INI);
	if( strlen(tbuf)> 0 )
	{
		if( !SetPVT(hVal, handleRAS, "registrationInfo.terminalAlias.2.e164", strlen(tbuf), tbuf ) )
			return FALSE;
		::GetPrivateProfileString("BISCON_RAS", "E164_ID_2", "", tbuf, 64, H323PHONE_INI);
		if( strlen(tbuf)> 0 )
		{
			if( !SetPVT(hVal, handleRAS, "registrationInfo.terminalAlias.3.e164", strlen(tbuf), tbuf ) )
				return FALSE;
			::GetPrivateProfileString("BISCON_RAS", "E164_ID_3", "", tbuf, 64, H323PHONE_INI);
			if( strlen(tbuf)> 0 )
			{
				if( !SetPVT(hVal, handleRAS, "registrationInfo.terminalAlias.4.e164", strlen(tbuf), tbuf ) )
					return FALSE;
				::GetPrivateProfileString("BISCON_RAS", "E164_ID_4", "", tbuf, 64, H323PHONE_INI);
				if( strlen(tbuf)> 0 )
				{
					if( !SetPVT(hVal, handleRAS, "registrationInfo.terminalAlias.5.e164", strlen(tbuf), tbuf ) )
						return FALSE;
					::GetPrivateProfileString("BISCON_RAS", "E164_ID_5", "", tbuf, 64, H323PHONE_INI);
					if( strlen(tbuf)> 0 )
					{
						if( !SetPVT(hVal, handleRAS, "registrationInfo.terminalAlias.6.e164", strlen(tbuf), tbuf ) )
							return FALSE;
						::GetPrivateProfileString("BISCON_RAS", "E164_ID_6", "", tbuf, 64, H323PHONE_INI);
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
	
	// Set OverLapSending
	iVal = ::GetPrivateProfileInt("BISCON_RAS", "RAS_OverlappedSending", 0, H323PHONE_INI);
	if( iVal == 1 )
	{
		if( !SetPVT(hVal, handleRAS, "overlapSending",0, NULL ) )
			return FALSE;
	}
	
	iVal = ::GetPrivateProfileInt("BISCON_RAS", "RAS_ResponseTimeOut", 3, H323PHONE_INI);
	if( iVal > 0 )
	{
		if( !SetPVT(hVal, handleRAS, "responseTimeOut", iVal, NULL ) )
			return FALSE;
	}
	
	// 아래의 값은 설정되지 않고 시스템 고유의 값을 사용하는 것을 권장 함.
	// 따라서 설정하지 않음.
	// 2006/11/24/1359 swhors@naver.com
	// mark start --
	// Set TTL Value
	//iVal = ::GetPrivateProfileInt("BISCON_RAS", "RAS_TimeToLive", 0, H323PHONE_INI);
	//if( iVal > 0 )
	//{
	//	if( !SetPVT(hVal, handleRAS, "registrationInfo.timeToLive", iVal, NULL ) )
	//		return FALSE;
	//}
	// mark end --

#if 1
	// Set MC Value
	if( !SetPVT(hVal, handleRAS, "registrationInfo.terminalType.mc", 0, NULL ) )
		return FALSE;
	
	// Set undefineNode
	if( !SetPVT(hVal, handleRAS, "registrationInfo.terminalType.undefinedNode", 1, NULL ) )
		return FALSE;
	
	// Set Terminal Type
	if( !SetPVT(hVal, handleRAS, "registrationInfo.terminalType.terminal", 0, NULL ) )
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

BOOL SetParamH245CodecType(PVOID _pConf, HAPP hAPP,HPVT hVal,UINT hH45Handle,int count, int iCodecType=0)
{
	int iVal;
	char sPath[256];
	CCallConf *pConf = (CCallConf*) _pConf;
	sprintf(sPath, "capabilities.terminalCapabilitySet.capabilityTable.%d.capabilityTableEntryNumber", count+1);
	if( !_pConf )
		return FALSE;
	switch(iCodecType)
	{
	case 0:// G7231-64K
		{
			allowedCodec[count] = 7231;
			if( !SetPVT(hVal, hH45Handle, sPath, 7231, NULL ) )
				return FALSE;
			iVal = ::GetPrivateProfileInt("BISCON_RTP", "G7231_maxAl-sduAudioFrames", 1, H323PHONE_INI);
			pConf->m_iG7231AudioFrame = iVal;
			//iIvrDefaultSilenceSupression = iVal;
			sprintf(sPath, "capabilities.terminalCapabilitySet.capabilityTable.%d.capability.receiveAndTransmitAudioCapability.g7231.maxAl-sduAudioFrames", count+1);
			if( !SetPVT(hVal, hH45Handle, sPath, iVal, NULL ) )
				return FALSE;
			iVal = ::GetPrivateProfileInt("BISCON_RTP", "G7231_SilenceSupression", 1, H323PHONE_INI);
			pConf->m_EnbaleSilenceSupression = iVal;
			sprintf(sPath, "capabilities.terminalCapabilitySet.capabilityTable.%d.capability.receiveAndTransmitAudioCapability.g7231.silenceSuppression", count+1);
			if( !SetPVT(hVal, hH45Handle, sPath, iVal, NULL ) )
				return FALSE;
			pConf->m_iEnableCodecCount++;
			pConf->m_EnableG7231 = 1;
		}
		break;
	case 1:// G711Ulaw
		allowedCodec[count] = 7111;
		if( !SetPVT(hVal, hH45Handle, sPath, 7111, NULL ) )
			return FALSE;
		sprintf(sPath, "capabilities.terminalCapabilitySet.capabilityTable.%d.capability.receiveAndTransmitAudioCapability.g711Ulaw64k", count+1);
		if( !SetPVT(hVal, hH45Handle, sPath, 60, NULL ) )
			return FALSE;
		pConf->m_iEnableCodecCount++;
		pConf->m_EnableG711u = 1;
		break;
	case 2:// G711Alaw
		allowedCodec[count] = 7112;
		if( !SetPVT(hVal, hH45Handle, sPath, 7112, NULL ) )
			return FALSE;
		sprintf(sPath, "capabilities.terminalCapabilitySet.capabilityTable.%d.capability.receiveAndTransmitAudioCapability.g711Alaw64k", count+1);
		if( !SetPVT(hVal, hH45Handle, sPath, 60, NULL ) )
			return FALSE;
		pConf->m_iEnableCodecCount++;
		pConf->m_EnableG711a = 1;
		break;
	case 6: // dtmf
		allowedCodec[count] = 6000;
		if( !SetPVT(hVal, hH45Handle, sPath, 6000, NULL ) )
			return FALSE;
		sprintf(sPath, "capabilities.terminalCapabilitySet.capabilityTable.%d.capability.receiveAndTransmitUserInputCapability.dtmf", count+1);
		if( !SetPVT(hVal, hH45Handle, sPath, 0, NULL ) )
			return FALSE;
		pConf->m_EnableDtmf = 1;
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
				iVal = ::GetPrivateProfileInt("BISCON_RTP", "G7231_maxAl-sduAudioFrames", 1, H323PHONE_INI);
				if( !SetPVT(hVal, hAndle, sPath, iVal, NULL ) )
					return FALSE;
				wsprintf(sPath, "channels.%d.dataType.audioData.g7231.silenceSuppression", i+1);
				iVal = ::GetPrivateProfileInt("BISCON_RTP", "G7231_SilenceSupression", 1, H323PHONE_INI);
				if( !SetPVT(hVal, hAndle, sPath, iVal, NULL ) )
					return FALSE;
			}
			break;
		case 7111:
			{
				if( !SetPVT(hVal, hAndle, sPath, 11, "g711Ulaw64k") )
					return FALSE;
				wsprintf(sPath, "channels.%d.dataType.audioData.g711Ulaw56k", i+1);
				if( !SetPVT(hVal, hAndle, sPath, 60, NULL ) )
					return FALSE;
			}
			break;
		case 7112:
			{
				if( !SetPVT(hVal, hAndle, sPath, 11, "g711Alaw64k") )
					return FALSE;
				wsprintf(sPath, "channels.%d.dataType.audioData.g711Alaw56k", i+1);
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

BOOL SetParamH245(PVOID _pMainWnd, HPVT hVal)
{
	BYTE pID[2];
	INT32 handleH245;
	int iCodecType,iCodecNum=0;
	CUbiDialerDlg *pMainWnd = (CUbiDialerDlg *)_pMainWnd;

	if( !pMainWnd )
		return FALSE;
	CCallConf *pConf = (CCallConf*)&pMainWnd->m_CallConf;
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
	iCodecType = ::GetPrivateProfileInt("BISCON_RTP", "CODEC_TYPE1", -1, H323PHONE_INI);
	if( iCodecType >= 0 )
	{
		if( !SetParamH245CodecType(pConf, hApp, hVal, handleH245, iCodecNum++, iCodecType) )
			return FALSE;
	}
	iCodecType = ::GetPrivateProfileInt("BISCON_RTP", "CODEC_TYPE2", -1, H323PHONE_INI);
	if( iCodecType >= 0 )
	{
		if( !SetParamH245CodecType(pConf, hApp, hVal, handleH245, iCodecNum++, iCodecType) )
			return FALSE;
	}
	iCodecType = ::GetPrivateProfileInt("BISCON_RTP", "CODEC_TYPE3", -1, H323PHONE_INI);
	if( iCodecType >= 0 )
	{
		if( !SetParamH245CodecType(pConf, hApp, hVal, handleH245, iCodecNum++, iCodecType) )
			return FALSE;
	}
	iCodecType = ::GetPrivateProfileInt("BISCON_RTP", "CODEC_TYPE4", -1, H323PHONE_INI);
	if( iCodecType >= 0 )
	{
		if( !SetParamH245CodecType(pConf, hApp, hVal, handleH245, iCodecNum++, iCodecType) )
			return FALSE;
	}
	iCodecType = ::GetPrivateProfileInt("BISCON_RTP", "DTMF_TYPE", -1, H323PHONE_INI);
	if( iCodecType >= 0 )
	{
		if( !SetParamH245CodecType(pConf, hApp, hVal, handleH245, iCodecNum, iCodecType+6) )
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
	iVal = ::GetPrivateProfileInt("BISCON_Q931", "Q931_ResponseTimeOut", 4, H323PHONE_INI);
	if( !SetPVT(hVal, handleQ931, "responseTimeOut", iVal, NULL ) )
		return FALSE;
	// set connection timeout value.
	iVal = ::GetPrivateProfileInt("BISCON_Q931", "Q931_ConnectTimeOut", 100, H323PHONE_INI);
	if( !SetPVT(hVal, handleQ931, "connectTimeOut", iVal, NULL ) )
		return FALSE;
	// set callSignallingPort
	iVal = ::GetPrivateProfileInt("BISCON_Q931", "Q931_CallSignalingPort", 1720, H323PHONE_INI);
	if( !SetPVT(hVal, handleQ931, "callSignalingPort", iVal, NULL ) )
		return FALSE;
	// set maxCalls
	//iVal = (iMaxAllowedCall*2)+10;
	if( !SetPVT(hVal, handleQ931, "maxCalls", 2, NULL ) )
		return FALSE;
	// set h245Tunneling
	iVal = ::GetPrivateProfileInt("BISCON_Q931", "Q931_H245Tunneling", 0, H323PHONE_INI);
	if( !SetPVT(hVal, handleQ931, "h245Tunneling", iVal, NULL ) )
		return FALSE;
	return TRUE;
}

BOOL ParseConfigure(PVOID pObject)
{
	CUbiDialerDlg *pMainWnd = (CUbiDialerDlg *)pObject;
	BOOL bRet = FALSE;
	hApp = NULL;

	if( !pMainWnd )
		return FALSE;
	//pMainWnd->m_CallConf
	pMainWnd->m_CallConf.m_iFastStart = GetPrivateProfileInt("BISCON_Q931", "FastStart", 0,H323PHONE_INI);
	
	pMainWnd->m_CallConf.m_iH245Tunneling = GetPrivateProfileInt("BISCON_Q931", "Q931_H245Tunneling", 0,H323PHONE_INI);
	
	pMainWnd->m_CallConf.m_g729Support = GetPrivateProfileInt("SystemBase Setup", "G729Support", 0, H323PHONE_INI);
	pMainWnd->m_CallConf.m_InitOfRtpPort = ::GetPrivateProfileInt("BISCON_RTP", "RTP_port", 30000, H323PHONE_INI);	
	
	if( GetPrivateProfileInt("BISCON_RTP", "RTP_Record", 0,H323PHONE_INI))
		pMainWnd->m_CallConf.m_EnableRecordRtp = TRUE;
	else
		pMainWnd->m_CallConf.m_EnableRecordRtp = FALSE;
	
	// BISCon.tmp 파일을 확인 생성한다.
	// 2006.11.24.1355 swhors@naver.com
	FILE * fp;
	fp = fopen("BISCon.tmp", "a+t");
	if( fp )
	{
		fclose(fp);
	}
	
	HCFG hCfg = ciConstructEx("BISCon.tmp",1024,30);
	if( hCfg )
	{
		ciSetValue(hCfg, "system.maxCalls",FALSE,(INT32)(2),NULL);
		ciSave(hCfg,"BISCon.tmp");
		ciDestruct(hCfg);
	}
	
	if(cmInitialize("BISCon.tmp", &hApp) <0)
	{
		// cmInitialize 후에 
		// BISCon.tmp 파일을 삭제한다.
		// 2006.11.24.1355 swhors@naver.com
		::DeleteFile("BISCon.tmp");
		return FALSE;
	}
	if( hApp)
	{
		int iNodeID =0;
		int iRasCFH = 0;
		int len = 0;
		
		HPVT hVal;
		
		// cmInitialize 후에 
		// BISCon.tmp 파일을 삭제한다.
		// 2006.11.24.1355 swhors@naver.com
		::DeleteFile("BISCon.tmp");
		
		hVal=cmGetValTree(hApp);
		
		if( hVal <= 0 )
			return FALSE;
		
		if( !::GetPrivateProfileInt("BISCON", "RAS_FROM_FILE", 0, "../biscon.ini"))
			SetParamRAS(hVal);
		iRasCFH=cmGetRASConfigurationHandle(hApp);
		iNodeID=pvtGetNodeIdByPath(hVal,iRasCFH,"registrationInfo.terminalAlias.2.e164");
		len=pvtGetString(hVal,iNodeID,256,pMainWnd->m_CallConf.m_sSelfE164);
		if (len>=0)
			pMainWnd->m_CallConf.m_sSelfE164[len]=0;
		
		if( !::GetPrivateProfileInt("BISCON", "Q931_FROM_FILE", 0, "../biscon.ini"))
			SetParamQ931(hVal);
		if( !::GetPrivateProfileInt("BISCON", "H245_FROM_FILE", 0, "../biscon.ini"))
			SetParamH245(pObject, hVal);
		
		// Ivr Initialize...
		init_call_stack(pObject);
		//unsigned long dwThreadId;
		return TRUE;
	}
	return FALSE;
}

BOOL init_call_stack(PVOID pObject)
{
	int i;
	CUbiDialerDlg *pMainDlg = (CUbiDialerDlg *) pObject;
	if( !pMainDlg )
		return NULL;
	CH323CallInfo *pCon = (CH323CallInfo *)&pMainDlg->cH323CallInfo;
	//H323CallInfo = hApp
	//CH323CallInfo *pCon = (CH323CallInfo*)pMainDlg
	cmSetHandle(hApp, (HAPPAPP)NULL);
	pCon->init();
	for (i=0;i<10;i++)
	{
		pCon->out[i].port = 0;
		pCon->out[i].hsChan = NULL;
		pCon->out[i].state=cmChannelStateIdle;
		pCon->out[i].sNum=i;
		pCon->out[i].haCall=(HAPPCALL)pCon;
		pCon->in[i].port = 0;
		pCon->in[i].hsChan = NULL;
		pCon->in[i].state=cmChannelStateIdle;
		pCon->in[i].sNum=i;
		pCon->in[i].haCall=(HAPPCALL)pCon;
	}
	//int nDataNodeId = -1;
	//nDataNodeId = cmUserInputSupportIndicationBuild(hApp, cmSupportGeneralString, &nDataNodeId);
	
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
//	lRtpInit(pMainDlg->m_hWnd, 1);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// end of file..
//
