#include "stdafx.h"
#include "string.h"
#include "resource.h"
#include "process.h"
#include <stddef.h>
#include <stdlib.h>
#include <conio.h>
#include "H323CallInfo.h"
#include <cm.h>
#include <q931asn1.h>

#include "UbiDialer.h"
#include "H323CallInfo.h"
#include "UbiDialerDlg.h"

#include "wmsg_define.h"

extern HAPP hApp;

#define RTPCHK_TID		35000
#define RTPCHK_TVAL		600

int		m_g729Support=0;
int		m_InitOfRtpPort;
BOOL	m_bRegisteredBISCon = 0;

extern HWND gMediaWnd;

char *CodecName[]=
{
	"g7231","g711Ulaw64k","g711Alaw64k","g729","none"
};

#ifndef UINT16
typedef unsigned short UINT16;
#endif

//#define G729_ENABLE

//#endif //FAST_START

/*
"g711Ulaw"	=1;
"g711Alaw"	=2;
"g7231"		=0;
*/

/* Set default data */
// 포트 번호를 설정.

//#ifdef FAST_START
void FillGlobalFSData(PVOID _pConf, PVOID _pCon)
{
	int i;
	CH323CallInfo * pCon = (CH323CallInfo * )_pCon;
	CCallConf * pConf = (CCallConf *)_pConf;
	memset( &pCon->GlobalFSDataRequested, 0, sizeof(cmFastStartMessage));
	pCon->GlobalFSDataRequested.partnerChannelsNum=1;
	
	pCon->GlobalFSDataRequested.partnerChannels[0].transmit.altChannelNumber=pConf->m_iEnableCodecCount;
	pCon->GlobalFSDataRequested.partnerChannels[0].receive.altChannelNumber=pConf->m_iEnableCodecCount;
	for( i =0; i < pConf->m_iEnableCodecCount; i++ )
	{
		pCon->GlobalFSDataRequested.partnerChannels[0].transmit.channels[i].rtp.port= 0;
		pCon->GlobalFSDataRequested.partnerChannels[0].transmit.channels[i].rtp.ip =  0;
		pCon->GlobalFSDataRequested.partnerChannels[0].transmit.channels[i].rtcp.port= pConf->m_InitOfRtpPort+1;
		pCon->GlobalFSDataRequested.partnerChannels[0].transmit.channels[i].rtcp.ip = 0;
		pCon->GlobalFSDataRequested.partnerChannels[0].transmit.channels[i].dataTypeHandle = -1;
		pCon->GlobalFSDataRequested.partnerChannels[0].receive.channels[i].rtp.port= pConf->m_InitOfRtpPort;
		pCon->GlobalFSDataRequested.partnerChannels[0].receive.channels[i].rtp.ip =  0;
		pCon->GlobalFSDataRequested.partnerChannels[0].receive.channels[i].rtcp.port= pConf->m_InitOfRtpPort+1;
		pCon->GlobalFSDataRequested.partnerChannels[0].receive.channels[i].rtcp.ip = 0;
		pCon->GlobalFSDataRequested.partnerChannels[0].receive.channels[i].dataTypeHandle = -1;
	}
	i = 0;
	if( pConf->m_EnableG7231 )
	{
		pCon->GlobalFSDataRequested.partnerChannels[0].transmit.channels[i].channelName = "g7231";
		pCon->GlobalFSDataRequested.partnerChannels[0].receive.channels[i++].channelName = "g7231";
	}
	if( pConf->m_EnableG711u )
	{
		pCon->GlobalFSDataRequested.partnerChannels[0].transmit.channels[i].channelName = "g711Ulaw64k";
		pCon->GlobalFSDataRequested.partnerChannels[0].receive.channels[i++].channelName = "g711Ulaw64k";
	}
	if( pConf->m_EnableG711a )
	{
		pCon->GlobalFSDataRequested.partnerChannels[0].transmit.channels[i].channelName = "g711Alaw64k";
		pCon->GlobalFSDataRequested.partnerChannels[0].receive.channels[i++].channelName = "g711Alaw64k";
	}
	if( pConf->m_g729Support)
	{
		pCon->GlobalFSDataRequested.partnerChannels[0].transmit.channels[i].channelName = "g729";
		pCon->GlobalFSDataRequested.partnerChannels[0].receive.channels[i].channelName = "g729";
	}
	pCon->GlobalFSDataRequested.partnerChannels[0].type = cmCapAudio;
}

int MakeCall(PVOID _pDlg, PVOID _pCon, char *szCaller)
{
	CUbiDialerDlg *pDlg;
	CH323CallInfo * pCon;
	//int	iLen = 16;
	//int i_error = 0;
	//int iOutCall = 0;
	pCon = (CH323CallInfo * ) _pCon;
	pDlg = (CUbiDialerDlg * ) _pDlg;
	if( pCon->hsCall )
		pCon->hsCall = NULL;
	if( cmCallNew(hApp,(HAPPCALL)pCon,&(pCon->hsCall)) < 0)
	{
		return 0;
	}
	else
	{
		if( pCon->hsCall )
		{
			pCon->iCallState = 3;
			for( int i = 0; i < 64 ; i++)
			{
				if( pCon->m_sOtherE164[i] == 0x20)
					pCon->m_sOtherE164[i] = 0;
			}
			// Outbound 콜의 정보를 초기화
			cmCallSetParam(pCon->hsCall, cmParamCanOverlapSending,0,0,NULL);
			cmCallSetParam(pCon->hsCall, cmParamEarlyH245,        0,1,NULL);
			if( pDlg->m_CallConf.m_iFastStart )
			{
				pDlg->cH323CallInfo.m_iFastStart = 1;
				cmCallSetParam(pCon->hsCall, cmParamH245Tunneling,    0,0,NULL);
			}
			else
			{
				pDlg->cH323CallInfo.m_iFastStart = 0;
				cmCallSetParam(pCon->hsCall, cmParamH245Tunneling,    0,1,NULL);
			}
			if( pDlg->m_CallConf.m_iFastStart )
				FillGlobalFSData(&pDlg->m_CallConf,pCon);
			pCon->bCalling = 1;
			SendMessage(gMediaWnd,UM_OPENINDEV, 0, 0);
			//AfxGetApp()->m_pMainWnd->SendMessage(UM_OPENINDEV, 0, 0);
			//AfxGetApp()->m_pMainWnd->SendMessage(UM_REQINRTP, 0, 0);
			//AfxGetApp()->m_pMainWnd->SendMessage(UM_REQOUTRTP, 0, 0);
			cmCallMake(pCon->hsCall, 64000, 0, pCon->m_sOtherE164,szCaller,PHONE_NAME,"Setup", 5);
			pDlg->bCalling = 1;
			SendMessage(gMediaWnd,WM_USER_MSG, WM_UMSG_SD_INV, 0);
			return 1;
		}
	}
	return 0;
}

void DropCall(PVOID _pCon, cmReasonType iReason)
{
	CH323CallInfo * pCon = (CH323CallInfo *) _pCon;
	if( pCon->hsCall)
	{
		//WriteLogL(PROG_NAME, iCallNum, "D-C", "DropCall_0");
		//if( pCon->bUsedThisCall )
		{
			//WriteLogL(PROG_NAME, iCallNum, "D-C", "DropCall_1");
			if( iReason != 0)
			{
				cmCallDropParam(pCon->hsCall, iReason);
			}
			else
			{
				cmCallDrop(pCon->hsCall);
			}
		}
	}
}

// first enter
int OnEvNewCall(HCALL hsCall, LPHAPPCALL lphaCall)
{
	CUbiDialerApp *pApp = (CUbiDialerApp *)AfxGetApp();
	CH323CallInfo * pCon=NULL;
	if( pApp )
	{
		CUbiDialerDlg * pDlg = (CUbiDialerDlg * )pApp->m_pMainWnd;
		if( pDlg )
		{
			if( pDlg->bCalled || pDlg->bCalling)
			{
				*lphaCall=NULL;
				return -1;
			}
			if( !pDlg->cH323CallInfo.bCalled && !pDlg->cH323CallInfo.bCalling )
			{
				pDlg->cH323CallInfo.hsCall = hsCall;
				pDlg->cH323CallInfo.iCallState = 1;
				*lphaCall = (HAPPCALL)&pDlg->cH323CallInfo;
				pCon = &pDlg->cH323CallInfo;
				pCon->bCalled = 1;
				cmCallSetParam(hsCall,cmParamEarlyH245,0,1,NULL);
				cmCallGetParam(hsCall,cmParamH245Tunneling,0,&pCon->m_iH245Tunneling,NULL);
				if( pCon->m_iH245Tunneling )
				{
					if( !pDlg->m_CallConf.m_iH245Tunneling )
					{
						pCon->m_iFastStart = pDlg->m_CallConf.m_iFastStart;
						pCon->m_iH245Tunneling = pDlg->m_CallConf.m_iH245Tunneling;
						cmCallSetParam(hsCall,cmParamH245Tunneling,0,pCon->m_iH245Tunneling,NULL);
					}
					else
						pCon->m_iFastStart = 0;
				}
				else
					pCon->m_iFastStart = pDlg->m_CallConf.m_iFastStart;
				pDlg->bCalled = 1;
				AfxGetApp()->m_pMainWnd->SendMessage(WM_USER_MSG, WM_UMSG_RV_INV, 0);
				return 1;
			}
			else
			{
				*lphaCall=NULL;
			}
		}
	}
	return -1;
}

int OnEvChannelStateChanged(PVOID _pCon, HAPPCHAN haChannel, HCHAN hsChannel, UINT32 state, UINT32 stateMode)
{
	channel * c = (channel *) haChannel;
	CH323CallInfo * pConnect=(CH323CallInfo * )_pCon;
	HCALL hsCall = pConnect->hsCall;
	
	if(!haChannel)
	{
		cmChannelClose(hsChannel);
		return ERROR;
	}
	if( !hsChannel)
	{
		return ERROR;
	}
	RvBool origin = FALSE;
	cmChannelGetOrigin(hsChannel,&origin);
	c->state = (int)state;
	c->stateMode = (int) stateMode;
	switch(state)
	{
	case cmChannelStateDisconnected:
		pConnect->bOpendChannel = FALSE;
#if 0
		if( stateMode == cmChannelStateModeDisconnectedLocal )
			WriteLogL(PROG_NAME, pConnect->iCallNumber, "CHA", "cmChannelStateModeDisconnectedLocal");
		else if( stateMode == cmChannelStateModeDisconnectedRemote )
			WriteLogL(PROG_NAME, pConnect->iCallNumber, "CHA", "cmChannelStateModeDisconnectedRemote");
		else if( stateMode == cmChannelStateModeDisconnectedMasterSlaveConflict )
			WriteLogL(PROG_NAME, pConnect->iCallNumber, "CHA", "cmChannelStateModeDisconnectedMasterSlaveConflict");
		else if( stateMode == cmChannelStateModeDisconnectedReasonUnknown )
			WriteLogL(PROG_NAME, pConnect->iCallNumber, "CHA", "cmChannelStateModeDisconnectedReasonUnknown");
		else if( stateMode == cmChannelStateModeDisconnectedReasonReopen )
			WriteLogL(PROG_NAME, pConnect->iCallNumber, "CHA", "cmChannelStateModeDisconnectedReasonReopen");
		else if( stateMode == cmChannelStateModeDisconnectedReasonReservationFailure )
			WriteLogL(PROG_NAME, pConnect->iCallNumber, "CHA", "cmChannelStateModeDisconnectedReasonReservationFailure");
#endif
		SendMessage(gMediaWnd, UM_CLOSEOUTRTP, 0 , 0);
		break;
	case cmChannelStateConnected:
		{
			if( origin)
			{
				pConnect->bOpendChannel = TRUE;
				//cmAlias alias;
				//alias.type =  cmAliasTypeE164;
				//alias.string = &pConnect->m_sOtherE164[0];
				//alias.length = sizeof(pConnect->m_sOtherE164);
				//alias.length = strlen(pConnect->des_e164);
				//int iLen = sizeof(cmAlias);
				//cmCallGetParam(pConnect->hsCall, cmParamCalledPartyNumber, 1, &iLen, (char *)&alias);
				//iLen = 18;
				//iLen = sizeof(cmAlias);
				//memset( pConnect->m_sSelfE164, 0, 64);
				//alias.string = pConnect->m_sSelfE164;
				//alias.type = cmAliasTypeE164;
				//alias.length = 64;
				//cmCallGetParam(pConnect->hsCall, cmParamCallingPartyNumber, 0, &iLen, (char *)&alias);
				pConnect->iCallState = 2;
			}
		}
		break;
	case cmChannelStateIdle:
		{
#if 0
			lRtpClose(c->sNum);
#else
			SendMessage(gMediaWnd,UM_CLOSEINRTP, 0, 0);
			SendMessage(gMediaWnd,UM_CLOSEOUTRTP, 0, 0);
#endif
			cmChannelClose(hsChannel);
			c->dpt=0;
			c->hsChan=NULL;
		}
		break;
	case cmChannelStateOffering:
		{
			if (stateMode!=cmChannelStateModeDuplex)
			{
				cmChannelSetAddress(hsChannel,0,(UINT16)c->port);
				cmChannelSetRTCPAddress(hsChannel,0,(UINT16)(c->port+1));
			}
			else
			{
				cmTransportAddress ta={0,0,(UINT16)c->port,cmTransportTypeIP};
				cmChannelSetDuplexAddress(hsChannel,ta,1,"2",FALSE);
			}
			cmChannelSetFlowControlToZero(hsChannel,TRUE);
			cmChannelAnswer(hsChannel);
			if( !pConnect->out[c->sNum].hsChan && !cmChannelIsDuplex(pConnect->in[c->sNum].hsChan))
			{
				UINT rate = 64000;
#if 0
				int port = lRtpOpen(c->sNum, "testChannel", m_InitOfRtpPort);
#else
				int port = ((CUbiDialerDlg*)AfxGetApp()->m_pMainWnd)->m_CallConf.m_InitOfRtpPort;
				SendMessage(gMediaWnd,UM_REQINRTP, 0, 0);
#endif
				cmChannelNew(pConnect->hsCall,
					(HAPPCHAN)&(pConnect->out[c->sNum]),
					&(pConnect->out[c->sNum].hsChan));
				cmChannelSetRTCPAddress(pConnect->out[c->sNum].hsChan,
					0,(UINT16)(port+1));
				strcpy(pConnect->out[c->sNum].channelName,
					pConnect->in[c->sNum].channelName);
				
				if (pConnect->in[c->sNum].dpt)
				{
					cmChannelSetDynamicRTPPayloadType(pConnect->out[c->sNum].hsChan,
						pConnect->in[c->sNum].dpt);
				}
				cmChannelOpenDynamic(pConnect->out[c->sNum].hsChan,
					pvtParent(cmGetValTree(hApp),pConnect->in[c->sNum].handle)
					,NULL/*in[c->sNum].hsChan*/,NULL,FALSE);
			}
		}
		break;
	}
	return 0;
}

int OnEvRegEvent(IN HAPP hApp,cmRegState regState,cmRegEvent regEvent,int regEventHandle)
{
	if (regState==cmRegistered// || regState==cmResourceAvailabilityConfirmation
		|| regState == cmPermanentAlternateGatekeeperConfirmation)
	{
		m_bRegisteredBISCon = TRUE;
		AfxGetApp()->m_pMainWnd->SendMessage(WM_USER_MSG, WM_UMSG_SD_REG_SUCC, 0);
		//SendMessage(AfxGetApp()->m_pMainWnd->m_hWnd, WM_UMSG_SD_REG_SUCC, NULL, NULL);
	}
//	else if( regState == 
	else
	{
		SendMessage(AfxGetApp()->m_pMainWnd->m_hWnd, WM_USER_MSG, WM_UMSG_SD_REG_FAIL, 0);
		m_bRegisteredBISCon = FALSE;
		if( regEvent ==  cmUnregistrationRequest)
		{
			if( hApp)
			{
				cmUnregister(hApp);
				cmRegister(hApp);
			//WriteLogL(PROG_NAME, -2, "URJ", "Receive URQ message.");
			}
			//else
			//	WriteLog("error", -2, "URJ", "fail");
		}
		else// if (regEvent==cmRegistrationReject)
		{
			char sErrorbuf[120];
			memset(sErrorbuf, 0, 120);
			HPVT hVal=cmGetValTree(hApp);
			int handle=pvtChild(hVal,regEventHandle);
			if (pvtGetNodeIdByPath(hVal,handle,"rejectReason.discoveryRequired")>=0)
				sprintf(sErrorbuf, "rejectReason.discoveryRequired");
			else if (pvtGetNodeIdByPath(hVal,handle,"rejectReason.invalidRevision")>=0)
				sprintf(sErrorbuf, "rejectReason.invalidRevision");
			else if (pvtGetNodeIdByPath(hVal,handle,"rejectReason.invalidCallSignalAddress")>=0)
				sprintf(sErrorbuf, "rejectReason.invalidCallSignalAddress");
			else if (pvtGetNodeIdByPath(hVal,handle,"rejectReason.invalidRASAddress")>=0)
				sprintf(sErrorbuf, "rejectReason.invalidRASAddress");
			else if (pvtGetNodeIdByPath(hVal,handle,"rejectReason.duplicateAlias")>=0)
				sprintf(sErrorbuf, "rejectReason.duplicateAlias");
			else if (pvtGetNodeIdByPath(hVal,handle,"rejectReason.invalidTerminalType")>=0)
				sprintf(sErrorbuf, "rejectReason.invalidTerminalType");
			else if (pvtGetNodeIdByPath(hVal,handle,"rejectReason.undefinedReason")>=0)
				sprintf(sErrorbuf, "rejectReason.undefinedReason");
			else if (pvtGetNodeIdByPath(hVal,handle,"rejectReason.transportNotSupported")>=0)
				sprintf(sErrorbuf, "rejectReason.transportNotSupported");
			else if (pvtGetNodeIdByPath(hVal,handle,"rejectReason.fullRegistrationRequired")>=0)
				sprintf(sErrorbuf, "rejectReason.fullRegistrationRequired");
			else
				sprintf(sErrorbuf, "rejectReason.UnknownReason");
			//WriteLogL(PROG_NAME, -2, "RRJ", sErrorbuf);
			if( hApp )
				cmRegister(hApp);
			//else
			//	WriteLog("error", -2, "RRJ", "null point hApp");
		}
	}
	return 0;
}

int OnEvCallStateChanged(PVOID _pCon, HCALL _hsCall, UINT32 state, UINT32 stateMode)
{
	int return_code = ERROR;
	int call_tag = 0;
	int ilen = 64;
	int ilen2 = 30;
	CH323CallInfo * pConnect=(CH323CallInfo *)_pCon;
	if( !pConnect->hsCall )
	{
		cmCallClose(_hsCall);
		return ERROR;
	}
	if( _hsCall != pConnect->hsCall )
	{
		cmCallClose(_hsCall);
		return ERROR;
	}
	HCALL hsCall = pConnect->hsCall;
	RvBool origin=cmCallGetOrigin(_hsCall,NULL);
	if (state!=cmCallStateWaitAddressAck)
		pConnect->overlapState.running=overlapNotInit;
	switch(state)
	{
	case cmCallStateWaitAddressAck:
		{
			cmAlias alias;
			pConnect->overlapState.running=overlapWait;
			pConnect->overlapState.address[0]=0;
			alias.string=pConnect->overlapState.address;
			alias.length=sizeof(pConnect->overlapState.address);
			cmCallGetParam(_hsCall,cmParamCalledPartyNumber,0,0,(char*)&alias);
			alias.length = strlen(alias.string);
		}
		break;
	case cmCallStateConnected:
		{
			// 아래의 루틴은 Q931의 Facility를 전송하는 루틴이다.
			// 이 루틴은 startH245를 전송한다.
			// 2006/07/26 swhors@naver.com
			if( pConnect->bCalled )
				AfxGetApp()->m_pMainWnd->SendMessage(WM_USER_MSG, WM_UMSG_RV_INV_SUCC, 0 );
			if( pConnect->bCalling )
				AfxGetApp()->m_pMainWnd->SendMessage(WM_USER_MSG, WM_UMSG_SD_INV_SUCC, 0 );

			switch(stateMode)
			{
			case cmCallStateModeConnectedCall:
				if( !origin )
				{
					//SetTimer(m_hWnd, RTPCHK_TVAL, RTPCHK_TID, RTPOpenCheck); 
				}
				else
				{
					// 자기 자신이 건 전화인 경우, 음성 채널이 열리지 않은 경우, 
					// 인바운드 콜의 코덱 정보로 채널을 열어 준다.
					//if( !pConnect->m_iH245Tunneling && !pConnect->bOpendChannel)
					//	RTPChannelOpen(pConnect->iCallNumber);
					// 아웃 바운드가 열린 경우, 최대 통화 시간을 결정해서
					// 타이머를 가동시켜서 일정 시간 후, 호를 강제로 종료 시킨다.
				}
				break;
			case cmCallStateModeConnectedControl:
				break;
			case cmCallStateModeConnectedCallSetup:
				{
					if( !pConnect->m_iH245Tunneling )
					{
						INT32 value;
						cmCallGetParam(hsCall, cmParamEstablishH245, 0, &value, NULL);
						if( !cmCallHasControlSession( hsCall ) )
						{
							int newId;
							HPVT valH = cmGetValTree(hApp);
							int nodeId = cmProtocolCreateMessage(hApp, cmProtocolQ931);
							pvtAdd(valH, nodeId, __q931(callReferenceValue), 0,NULL, NULL);
							pvtAdd(valH, nodeId, __q931(protocolDiscriminator), 8,NULL, NULL);				
							newId = pvtBuildByPath(valH,nodeId, "message.facility", 0,NULL);
							
							int nid1 = pvtAdd(valH, newId, __q931(userUser), 0,NULL, NULL);
							pvtAdd(valH, nid1, __q931(protocolDiscriminator), 5,NULL, NULL);
							
							int newId1 = pvtBuildByPath(valH,nid1,"h323-UserInformation.h323-uu-pdu.h245Tunneling", 0, NULL);
							newId1 = pvtBuildByPath(valH,nid1,"h323-UserInformation.h323-uu-pdu.h323-message-body", 0, NULL);
							//newId1 = pvtAdd(valH, newId1, __q931(facility), 0, NULL, NULL);
							
							int iResult = pvtBuildByPath(valH, newId1, "facility.protocolIdentifier", 6, NULL);
							iResult = pvtBuildByPath(valH, newId1, "facility.reason", 0, NULL);
							iResult = pvtBuildByPath(valH, newId1, "facility.reason.startH245", 0, NULL);
							
							cmCallFacility( hsCall,nodeId );
							//pvtDelete(valH, nid1); /* !! remove temporary cap definition */
						}
					}
				}
				break;
			}
		}
		break;
	case cmCallStateDisconnected:
		{
			switch(stateMode)
			{
			case cmCallStateModeDisconnectedBusy:
			case cmCallStateModeDisconnectedReject:
			case cmCallStateModeDisconnectedUnreachable:
				{
					if( pConnect->bCalled )
						AfxGetApp()->m_pMainWnd->SendMessage(WM_USER_MSG, WM_UMSG_RV_INV_FAIL, 0 );
					if( pConnect->bCalling )
						AfxGetApp()->m_pMainWnd->SendMessage(WM_USER_MSG, WM_UMSG_SD_INV_FAIL, 0 );
				}
				//WriteLogL(PROG_NAME, pConnect->iCallNumber, "CAL", "DisconnectedUnreachable");
				break;
			case cmCallStateModeDisconnectedUnknown:
				//WriteLogL(PROG_NAME, pConnect->iCallNumber, "CAL", "DisconnectedUnknown");
				break;
			case cmCallStateModeDisconnectedNormal:
			case cmCallStateModeDisconnectedLocal:
				{
					AfxGetApp()->m_pMainWnd->SendMessage(WM_USER_MSG, WM_UMSG_SD_BYE_SUCC, 0 );
				}
				//WriteLogL(PROG_NAME, pConnect->iCallNumber, "CAL", "DisconnectedLocal");
				break;
			default:
				//WriteLogL(PROG_NAME, pConnect->iCallNumber, "CAL", "Unkown");
				break;
			}
			cmReasonType type;
			cmCallGetParam(_hsCall, cmParamReleaseCompleteReason, 0,(RvInt32*)&type, NULL );
			//pConnect->bUsedThisCall = FALSE;
		}
		break;
	case cmCallStateIdle:
		//WriteLogL(PROG_NAME, pConnect->iCallNumber, "CAL", "Idle Call");
		{
			//int i = 0;
			//for( i = 0; i < 10 ; i++)
			//{
				SendMessage(gMediaWnd,UM_CLOSEINRTP, 0, 0);
				SendMessage(gMediaWnd,UM_CLOSEOUTRTP, 0, 0);
//				lRtpClose(i);
//				lRtpClose(i);
			//}
		}
		//if( pConnect->hsCall )
		//	cmCallClose(pConnect->hsCall);
		cmCallClose(_hsCall);
		pConnect->hsCall = NULL;
		pConnect->iCallState = 0;
		pConnect->bCallReusing = TRUE;
		break;
	case cmCallStateOffering:
		cmCallAnswerExt(hsCall,"BISCon IVR","Connect - message",17);
		break;
	case cmCallStateRingBack:
		cmCallGetParam(_hsCall,cmParamH245Tunneling,0,&pConnect->m_iH245Tunneling,NULL);
		break;
	case cmCallStateDialtone:
		if( pConnect->m_iFastStart )
			cmFastStartOpenChannels(_hsCall,&pConnect->GlobalFSDataRequested);
		break;
	}
	return 0;
}

int OnUserInput(PVOID _pCon, HCALL hsCall, INT32 userInputId)
{
	CH323CallInfo *pConnect = (CH323CallInfo *)_pCon;
	char str[7];
	char data[7];
	INT32 dataLength;
	int nodeId;
	cmUserInputIndication userInputIndication;
	cmUserInputSignalStruct userInputSignalStruct;
	cmUserInputData userData={str,sizeof(str)};
	cmNonStandardIdentifier identifier;
	
	//if(!pConnect->bUsedThisCall)
	//{
		//WriteLog("UserInput", pConnect->iCallNumber,"INP", "uncorrect");
	//	return ERROR;
	//}
	//cmUserInputSupportIndication userInputSupportIndication;
	nodeId = cmUserInputGetDetail(hApp,userInputId,&userInputIndication );
	switch(userInputIndication)
	{
	case cmUserInputNonStandard:
		break;
	case cmUserInputAlphanumeric:
		{
			memset( data, 0, 7);
			dataLength = 7;
			nodeId = cmUserInputGet(hApp,userInputId,&identifier,data,&dataLength,&userData);
		}
		break;
	case cmUserInputSupport:
		//nodeId = cmUserInputSupportGet(hApp,nodeId,&userInputSupportIndication);
		break;
	case cmUserInputSignal:
		{
			memset( data, 0, 7);
			dataLength = 7;
			nodeId = cmUserInputGetSignal(hApp,nodeId,&userInputSignalStruct);
		}
		break;
	case cmUserInputSignalUpdate:
		//cmUserInputGetSignalUpdate(hApp,nodeId,&userInputSignalStruct);
		break;
	}
	return 0;
}

int getFreeOutChan(PVOID _pCon)
{
	CH323CallInfo * pConnect =(CH323CallInfo *)_pCon; 
	for(int i=0;i<10;i++)
	{
		if (!pConnect->out[i].hsChan)
			return i;
	}
	return ERROR;
}

int getFreeInChan(PVOID _pCon)
{
	CH323CallInfo * pConnect =(CH323CallInfo *)_pCon; 
	for(int i=0;i<10;i++)
	{
		if (!pConnect->in[i].hsChan)
		{
			return i;
		}
	}
	return ERROR;
}

int OnChannelNew(PVOID _pCon, HCALL _hsCall, HCHAN hsChannel, LPHAPPCHAN lphaChannel)
{
	int sNum = 0;
	CH323CallInfo * pConnect =(CH323CallInfo *)_pCon;
	HAPPCHAN haBase;
	HCHAN	hsBase;
	if( !pConnect->hsCall)
		return ERROR;
	if( _hsCall != pConnect->hsCall)
		return ERROR;
	if( !hsChannel)
		return ERROR;
	RvBool origin = FALSE;
	cmChannelGetOrigin(hsChannel,&origin);
	if (origin)
	{
		int ret = cmChannelGetDependency(hsChannel, &haBase, &hsBase);
		sNum=getFreeOutChan(pConnect);
		pConnect->out[sNum].hsChan=hsChannel;
		pConnect->out[sNum].sNum = sNum;
		*lphaChannel=(HAPPCHAN)&(pConnect->out[sNum]);
#if 0
		pConnect->out[sNum].port = lRtpOpen(/*pConnect->iCallNumber*10+*/sNum,"testChannel",m_InitOfRtpPort);
#else
		SendMessage(gMediaWnd,UM_REQOUTRTP, 0, 0);
#endif
	}
	else
	{
		sNum=getFreeInChan(pConnect);
		if (sNum<0)
		{
			*lphaChannel=0;
			cmChannelDrop(hsChannel);
			return 0;
		}
		pConnect->in[sNum].hsChan=hsChannel;
		*lphaChannel=(HAPPCHAN)&(pConnect->in[sNum]);
		pConnect->in[sNum].sNum = sNum;
#if 0
		pConnect->in[sNum].port = lRtpOpen(/*pConnect->iCallNumber*10+*/sNum,"testChannel",m_InitOfRtpPort);
#else
		SendMessage(gMediaWnd,UM_REQINRTP, 0, 0);
		//SendMessage(gMediaWnd,UM_REQOUTRTP, 0, 0);
		//SendMessage(gMediaWnd,UM_REQINRTP, 0, 0);
#endif
	}
	return 0;
}

int OnChannelNewRate(PVOID _pCon, channel* c, HCHAN hsChan, UINT32 rate)
{
	CH323CallInfo * pConnect =(CH323CallInfo *)_pCon;
	RvBool origin=FALSE;
	if( hsChan)
		cmChannelGetOrigin(hsChan,&origin);
	if (origin && pConnect->in[c->sNum].hsChan)
		cmChannelFlowControl(pConnect->in[c->sNum].hsChan, rate);
	return 0;
}

//서원바보